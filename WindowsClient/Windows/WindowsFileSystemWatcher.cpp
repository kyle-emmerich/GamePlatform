#include "Windows/WindowsFileSystemWatcher.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>

namespace {
    std::wstring Utf8ToWide(const std::string& str) {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    std::string WideToUtf8(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }
}

WindowsFileSystemWatcher::~WindowsFileSystemWatcher() {
    // Stop all directory watches
    for (auto& pair : directoryWatches) {
        auto& watch = pair.second;
        if (watch.running) {
            *(watch.running) = false;
            // Cancel any pending IO
            if (watch.dirHandle != nullptr && watch.dirHandle != INVALID_HANDLE_VALUE) {
                CancelIoEx(watch.dirHandle, NULL);
            }
            if (watch.watchThread.joinable()) {
                watch.watchThread.join();
            }
        }
    }
}

DirectorySubscription* WindowsFileSystemWatcher::Subscribe(const std::string& directory) {
    auto it = subscriptions.find(directory);
    if (it != subscriptions.end()) {
        return &it->second;
    }

    //check if directory exists
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        throw std::runtime_error("Directory does not exist: " + directory);
    }

    DirectorySubscription subscription;
    subscription.directory = directory;
    subscriptions[directory] = subscription;

    std::shared_ptr<std::atomic<bool>> running = std::make_shared<std::atomic<bool>>(true);
    auto directoryWatchEntry = directoryWatches.try_emplace(directory);
    DirectoryWatch& newWatch = directoryWatchEntry.first->second;
    newWatch.directory = directory;
    newWatch.running = running;
    newWatch.watchThread = std::thread([this, directory, running, &newWatch]() {
        HANDLE dirHandle = CreateFileW(
            Utf8ToWide(directory).c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS, // Synchronous (Blocking)
            NULL
        );

        if (dirHandle == INVALID_HANDLE_VALUE) {
            return;
        }
        
        // Store handle so we can CancelIoEx later
        newWatch.dirHandle = dirHandle;

        alignas(DWORD) char buffer[4096]; // Larger buffer, aligned
        DWORD bytesReturned;

        while (*running) {
            if (ReadDirectoryChangesW(
                dirHandle,
                &buffer,
                sizeof(buffer),
                TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
                &bytesReturned,
                NULL, // No Overlapped
                NULL
            )) {
                if (bytesReturned == 0) continue;

                FILE_NOTIFY_INFORMATION* info = (FILE_NOTIFY_INFORMATION*)buffer;
                do {
                    std::wstring filenameW(info->FileName, info->FileNameLength / sizeof(WCHAR));
                    std::string filename = WideToUtf8(filenameW);

                    // Simple debounce could go here, but for now just fire events
                    switch (info->Action) {
                        case FILE_ACTION_MODIFIED:
                            onFileChanged(directory, filename);
                            break;
                        case FILE_ACTION_ADDED:
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            onFileCreated(directory, filename);
                            break;
                        case FILE_ACTION_REMOVED:
                        case FILE_ACTION_RENAMED_OLD_NAME:
                            onFileDeleted(directory, filename);
                            break;
                    }

                    if (info->NextEntryOffset == 0) break;
                    info = (FILE_NOTIFY_INFORMATION*)((char*)info + info->NextEntryOffset);
                } while (true);
            } else {
                // If ReadDirectoryChangesW fails, it might be because the handle was closed or cancelled
                break;
            }
        }
        
        CloseHandle(dirHandle);
        newWatch.dirHandle = INVALID_HANDLE_VALUE;
    });
    newWatch.watchThread.detach();

    return &subscriptions[directory];
}

void WindowsFileSystemWatcher::Unsubscribe(DirectorySubscription* subscription) {
    if (subscription) {
        auto it = directoryWatches.find(subscription->directory);
        if (it != directoryWatches.end()) {
            DirectoryWatch& watch = it->second;
            *watch.running = false;
            if (watch.dirHandle != INVALID_HANDLE_VALUE) {
                CancelIoEx((HANDLE)watch.dirHandle, NULL);
            }
            // Since we detached the thread, we can't join it. 
            // The thread will exit on its own when ReadDirectoryChangesW returns or CancelIoEx is called.
            directoryWatches.erase(it);
        }
        subscriptions.erase(subscription->directory);
    }
}

std::vector<std::string> WindowsFileSystemWatcher::GetSubscribedDirectories() const {
    std::vector<std::string> dirs;
    for (const auto& pair : subscriptions) {
        dirs.push_back(pair.first);
    }
    return dirs;
}

void WindowsFileSystemWatcher::onFileChanged(const std::string& directory, const std::string& filename) {
    std::scoped_lock lock(eventQueueMutex);
    eventQueue.push_back({ FileChangeType::Changed, directory, filename });
}

void WindowsFileSystemWatcher::onFileCreated(const std::string& directory, const std::string& filename) {
    std::scoped_lock lock(eventQueueMutex);
    eventQueue.push_back({ FileChangeType::Created, directory, filename });
}

void WindowsFileSystemWatcher::onFileDeleted(const std::string& directory, const std::string& filename) {
    std::scoped_lock lock(eventQueueMutex);
    eventQueue.push_back({ FileChangeType::Deleted, directory, filename });
}

void WindowsFileSystemWatcher::Update() {
    std::deque<FileChangeEvent> eventsToProcess;
    {
        std::scoped_lock lock(eventQueueMutex);
        eventsToProcess.swap(eventQueue);
    }

    //todo: consider adding directory to the filename to remove ambiguity
    //todo: consider coalescing multiple events for the same file into one
    for (const auto& event : eventsToProcess) {
        auto it = subscriptions.find(event.directory);
        if (it != subscriptions.end()) {
            DirectorySubscription& subscription = it->second;
            switch (event.type) {
                case FileChangeType::Changed:
                    subscription.FileChanged.Fire(event.filename);
                    break;
                case FileChangeType::Created:
                    subscription.FileCreated.Fire(event.filename);
                    break;
                case FileChangeType::Deleted:
                    subscription.FileDeleted.Fire(event.filename);
                    break;
            }
        }
    }
}
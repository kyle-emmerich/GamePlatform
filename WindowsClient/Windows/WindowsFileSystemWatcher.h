#pragma once

#include <deque>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include "Core/IFileSystemWatcher.h"

class WindowsFileSystemWatcher : public IFileSystemWatcher {
public: 
    WindowsFileSystemWatcher() = default;
    virtual ~WindowsFileSystemWatcher();

    /// @brief Watches a directory (must exist) for file changes, creations, and deletions.
    /// @param directory the directory to watch
    /// @return DirectorySubscription* object containing MulticastEvents for file updates
    /// @throws std::runtime_error if directory does not exist 
    virtual DirectorySubscription* Subscribe(const std::string& directory) override;
    virtual void Unsubscribe(DirectorySubscription* subscription) override;

    std::vector<std::string> GetSubscribedDirectories() const;



    void Update();

private:
    std::unordered_map<std::string, DirectorySubscription> subscriptions;

    void onFileChanged(const std::string& directory, const std::string& filename);
    void onFileCreated(const std::string& directory, const std::string& filename);
    void onFileDeleted(const std::string& directory, const std::string& filename);

    enum class FileChangeType {
        Changed,
        Created,
        Deleted
    };
    struct FileChangeEvent {
        FileChangeType type;
        std::string directory;
        std::string filename;
    };
    std::mutex eventQueueMutex;
    std::deque<FileChangeEvent> eventQueue;

    struct DirectoryWatch {
        std::string directory;
        std::thread watchThread;
        std::shared_ptr<std::atomic<bool>> running;
        void* dirHandle = nullptr; // HANDLE is void*
    };
    std::unordered_map<std::string, DirectoryWatch> directoryWatches;
};
#pragma once

#include <string>
#include "Core/Event.h"

struct DirectorySubscription {
    std::string directory;
    MulticastEvent<std::string> FileChanged;
    MulticastEvent<std::string> FileCreated;
    MulticastEvent<std::string> FileDeleted;
};

class IFileSystemWatcher {
public:
    virtual DirectorySubscription* Subscribe(const std::string& directory) = 0;
    virtual void Unsubscribe(DirectorySubscription* subscription) = 0;
};

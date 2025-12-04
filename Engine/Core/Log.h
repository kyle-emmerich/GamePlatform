#pragma once

#include <string>
#include <sstream>
#include <ostream>

#include "Core/Event.h"
#include "Core/Export.h"
#include "Instance/System.h"

namespace Engine {

class Engine;

class GP_EXPORT Log : public System {
public:
    Log(Engine* engine);
    ~Log();

    enum class Level : uint8_t {
        Verbose,
        Info,
        Warning,
        Error,
        Max
    };

    void Write(const std::string& message, Level level = Level::Info);

    std::stringstream& GetStream(Level level);

    inline void SetLogLevel(Level level) {
        currentLevel = level;
    }
    Level GetLogLevel() const {
        return currentLevel;
    }

    void Clear();

    void Update(double deltaTime);

    MulticastEvent<std::string, Level> MessageLogged;
private:
    Engine* engine = nullptr;

    std::stringstream verboseStream;
    std::stringstream infoStream;
    std::stringstream warningStream;
    std::stringstream errorStream;

    Level currentLevel = Level::Info;
};

}

    void pullMessagesFromStream(std::stringstream& stream, Level level);
};
#pragma once

#include <string>
#include <sstream>
#include <ostream>

#include "Core/Event.h"
#include "Core/Export.h"
#include "Instance/System.h"
#include "LogSystem.generated.h"

class Engine;

class GP_EXPORT [[reflect()]] LogSystem : public System, BaseInstance<LogSystem> {
    REFLECTION()
public:
    LogSystem(Engine* engine);
    ~LogSystem();

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

    void pullMessagesFromStream(std::stringstream& stream, Level level);
};

//REFLECTION_END()
namespace Reflection { 
 
	inline void register_LogSystem() { 
		Reflection::GetRegistry().classes["LogSystem"] = &Reflection::reflected_LogSystem; 
		Reflection::GetRegistry().classesById[2491798829761274318] = &Reflection::reflected_LogSystem; 
 
	} 
	inline ::Instance* instantiate_LogSystem(Engine* engine) { 
		return (::Instance*)new LogSystem(engine); 
	} 
	inline ::std::shared_ptr<::Instance> instantiateShared_LogSystem(Engine* engine) { 
		return ::std::static_pointer_cast<::Instance>(::std::make_shared<LogSystem>(engine)); 
	} 
} 


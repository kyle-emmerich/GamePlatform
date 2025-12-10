#pragma once
#include <map>
#include <type_traits>
#include <string_view>
#include <deque>
#include "Core/Export.h"
#include "Instance/Instance.h"
#include "Instance/System.h"
#include "Instance/Reflection.h"
#include "Scripting/LuaState.h"
#include "Core/TimeProvider.h"
#include "Engine.generated.h"

namespace Rendering {
	class IRenderer;
}

class Engine;
class SystemInitOrder {
protected:
	friend class Engine;
	enum class SystemRelation {
		None,
		Before,
		After
	};
	struct Rule {
		System* system;
		std::string_view relativeToName;
		SystemRelation relation;
	};

	std::deque<Rule> rules;
	std::vector<System*> Resolve() const;
public:
	void Before(System* system, std::string_view beforeSystemName);
	void After(System* system, std::string_view afterSystemName);
	void Add(System* system);
};

struct EngineInitParams {
	ITimeProvider* timeProvider = nullptr;
	Rendering::IRenderer* renderer = nullptr;
};

class Log;

class GP_EXPORT [[reflect(Engine)]] Engine : public Instance, BaseInstance<Engine> {
	REFLECTION()
public:
	Engine(Engine* engine = nullptr);
	~Engine();

	void Initialize(const EngineInitParams& params = EngineInitParams());
	void Shutdown();

	Rendering::IRenderer* GetRenderer() { return renderer; }

	template<typename T>
	T* GetSystem() {
		static_assert(std::is_base_of<System, T>::value == true, "GetSystem<T> can only be used with types derived from System!");

		const std::string& className = T::ClassName();
		return static_cast<T*>(GetSystem(className));
	}
	[[reflect()]]
	System* GetSystem(std::string_view systemName);

	bool ExecuteConsoleLua(const std::string& code);

	Log& GetLog() {
		return *log;
	}

	double GetTime();

	void Update();
	void Render();
protected:
	ITimeProvider* timeProvider = nullptr;
	Log* log = nullptr;

	double startTime = 0.0;
	double lastFrameStart = 0.0;

	Rendering::IRenderer* renderer = nullptr;

	Lua::State* consoleState = nullptr;

	bool systemsInitialized = false;
	std::map<std::string_view, System*> systems;
	std::vector<System*> orderedSystems;
};

REFLECTION_END()
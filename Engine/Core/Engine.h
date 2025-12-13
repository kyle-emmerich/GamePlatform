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
#include "Core/IFileSystemWatcher.h"
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
	IFileSystemWatcher* fileSystemWatcher = nullptr;
	class IFontInfoProvider* fontInfoProvider = nullptr;
	Rendering::IRenderer* renderer = nullptr;

	std::string_view assetCacheDirectory = "AssetCache";
	std::string_view contentDirectory = "Content";
};

class Log;

class GP_EXPORT [[reflect(Engine)]] Engine : public Instance, BaseInstance<Engine> {
	REFLECTION()
public:
	Engine(Engine* engine = nullptr);
	~Engine();

	void Initialize(const EngineInitParams& params = EngineInitParams());
	void Shutdown();

	Rendering::IRenderer* GetRenderer() const { return renderer; }
	const std::string_view& GetAssetCacheDirectory() const { return assetCacheDirectory; }
	const std::string_view& GetContentDirectory() const { return contentDirectory; }

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
	IFileSystemWatcher* GetFileSystemWatcher() {
		return fileSystemWatcher;
	}

	void Update();
protected:
	ITimeProvider* timeProvider = nullptr;
	IFileSystemWatcher* fileSystemWatcher = nullptr;
	std::string_view assetCacheDirectory;
	std::string_view contentDirectory;
	Log* log = nullptr;

	double startTime = 0.0;
	double lastFrameStart = 0.0;

	Rendering::IRenderer* renderer = nullptr;

	Lua::State* consoleState = nullptr;

	bool systemsInitialized = false;
	std::map<std::string_view, System*> systems;
	std::vector<System*> orderedSystems;

	std::vector<class Viewport*> viewports;
	friend class Viewport;

	void registerViewport(class Viewport* viewport);
	void unregisterViewport(class Viewport* viewport);
};

REFLECTION_END()
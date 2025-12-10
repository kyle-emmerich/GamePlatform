#include "Core/Engine.h"
#include "Core/LogSystem.h"
#include "Instance/System.h"
#include "Instance/Reflection.h"
#include "UI/UISystem.h"
#include "Rendering/IRenderer.h"
#include <unordered_map>
#include "ReflectionRegistry.h"

void SystemInitOrder::Before(System* system, std::string_view beforeSystemName) {
	rules.push_back({ system, beforeSystemName, SystemRelation::Before });
}
void SystemInitOrder::After(System* system, std::string_view afterSystemName) {
	rules.push_back({ system, afterSystemName, SystemRelation::After });
}
void SystemInitOrder::Add(System* system) {
	rules.push_back({ system, "", SystemRelation::None });
}

std::vector<System*> SystemInitOrder::Resolve() const {
	std::unordered_map<std::string_view, System*> systemsByName;
	for (const Rule& rule : rules) {
		if (rule.relation != SystemRelation::None) {
			systemsByName[rule.system->ClassName()] = rule.system;
		}
	}

	//Now, process the rules in order.
	std::vector<System*> orderedSystems;
	std::deque<Rule> pendingRules = rules;

	//Keep processing until no more rules can be applied.
	bool progressMade = true;
	while (progressMade && !pendingRules.empty()) {
		progressMade = false;
		Rule& rule = pendingRules.front();

		if (rule.relation == SystemRelation::None) {
			//No dependencies, can add directly.
			orderedSystems.push_back(rule.system);
			pendingRules.pop_front();
			progressMade = true;
		} else {
			//Has a dependency, check if it's already in the ordered list.
			auto it = systemsByName.find(rule.relativeToName);
			if (it != systemsByName.end()) {
				System* relativeSystem = it->second;
				auto posIt = std::find(orderedSystems.begin(), orderedSystems.end(), relativeSystem);
				if (posIt != orderedSystems.end()) {
					//Found the relative system in the ordered list.
					if (rule.relation == SystemRelation::Before) {
						orderedSystems.insert(posIt, rule.system);
					} else { //After
						orderedSystems.insert(posIt + 1, rule.system);
					}
					pendingRules.pop_front();
					progressMade = true;
				} else {
					//Relative system not yet added, move this rule to the back of the queue.
					pendingRules.push_back(rule);
					pendingRules.pop_front();
				}
			} else {
				//Relative system not found, treat as no dependency.
				orderedSystems.push_back(rule.system);
				pendingRules.pop_front();
				progressMade = true;
			}
		}
	}

	return orderedSystems;
}

Engine::Engine(Engine* engine) : Instance(this) {

}

Engine::~Engine() {

}

void Engine::Initialize(const EngineInitParams& params) {
	Reflection::registerAllClasses();

	renderer = params.renderer;
	timeProvider = params.timeProvider;
	if (!timeProvider) {
		timeProvider = new StdTimeProvider();
	}

	startTime = timeProvider->GetTimeSeconds();

	//Look up all the classes that derive from System and create them.
	SystemInitOrder initOrder;

	for (const auto& classEntry : Reflection::GetRegistry().classes) {
		Reflection::Class* cls = classEntry.second;
		if (cls->IsA("System") && cls->className != "System") {
			//Create an instance of the system.
			System* system = cls->InstantiateAs<System>(this);
			if (system) {
				systems[cls->className] = system;
				
				system->Register(initOrder);
			}
		}
	}
	
	orderedSystems = initOrder.Resolve();
	for (System* system : orderedSystems) {
		system->Initialize();
	}
	systemsInitialized = true;

	//Initialize console Lua state
	consoleState = new Lua::State(Lua::StateContext::Developer); //todo: change the context depending on how the engine is initialized

}

void Engine::Shutdown() {

}

System* Engine::GetSystem(std::string_view systemName) {
	if (!systemsInitialized) {
		throw std::runtime_error("Systems have not been initialized yet!");
	}

	auto it = systems.find(systemName);
	if (it != systems.end()) {
		return it->second;
	}
	throw std::runtime_error("System not found: " + std::string(systemName));
}

bool Engine::ExecuteConsoleLua(const std::string& code) {
	return consoleState->Execute(code);
}

double Engine::GetTime() {
	return timeProvider->GetTimeSeconds() - startTime;
}

void Engine::Update() {
	double now = timeProvider->GetTimeSeconds();
	double deltaTime = now - lastFrameStart;
	lastFrameStart = now;

	for (System* system : orderedSystems) {
		system->Update(deltaTime);
	}
}

void Engine::Render() {
	if (renderer) {
		renderer->BeginFrame();

		//Find all active cameras and render them

		//Render UI last
		if (UISystem* uiSystem = GetSystem<UISystem>()) {
			uiSystem->Render(renderer);
		}

		renderer->EndFrame();
	}
}
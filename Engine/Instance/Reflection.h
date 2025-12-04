#pragma once
#include <typeindex>
#include <vector>
#include <string>
#include <any>
#include <string_view>
#include <functional>
#include <cassert>
#include <unordered_map>
#include <exception>
#include <stdexcept>
#include <memory>
#include "Scripting/StateContext.h"

class Instance;
class Engine;

namespace Reflection {

	constexpr uint64_t fnv1a64(std::string_view s) {
		uint64_t h = 1469598103934665603ull;
		for (char c : s) {
			h ^= static_cast<unsigned char>(c);
			h *= 1099511628211ull;
		}
		return h;
	}

	struct Attribute {
		std::string key;
		std::string value;
	};

	using PropFlagsType = uint8_t;
	enum class PropFlags : PropFlagsType {
		None = 0,
		ReadOnly = 1u << 0,
		Hidden = 1u << 1,
		Serializable = 1u << 2,
		Storable = 1u << 3,
		Replicated = 1u << 4
	};
	inline constexpr PropFlags operator| (PropFlags a, PropFlags b) {
		return static_cast<PropFlags>(static_cast<PropFlagsType>(a) | static_cast<PropFlagsType>(b));
	}
	inline constexpr bool HasFlag(PropFlags flags, PropFlags flag) {
		return (static_cast<PropFlagsType>(flags) & static_cast<PropFlagsType>(flag)) != 0;
	}

	static constexpr PropFlags DefaultPropFlags = PropFlags::Serializable | PropFlags::Storable;
	static constexpr PropFlags ReadOnlyPropFlags = PropFlags::ReadOnly | PropFlags::Serializable | PropFlags::Storable;
	static constexpr PropFlags HiddenPropFlags = PropFlags::Hidden | PropFlags::Serializable | PropFlags::Storable;
	static constexpr PropFlags ReplicatedPropFlags = PropFlags::Replicated | PropFlags::Serializable | PropFlags::Storable;
	static constexpr PropFlags OnDemandPropFlags = PropFlags::ReadOnly;

	using PropTypeFlagsType = uint8_t;
	enum class PropTypeFlags : PropTypeFlagsType {
		None = 0,
		RawPointer = 1,
		SmartPointer = 2,
		Vector = 3,
		Map = 4,
		Array = 5
	};

	enum class AccessLevel : uint8_t {
		Public = 0,
		Protected = 1,
		Private = 2
	};

	struct Property {
		std::string name;
		std::string type;
		uint64_t id;
		PropFlags flags;
		PropTypeFlags typeFlags;
		AccessLevel accessLevel = AccessLevel::Public;
		Lua::StateContext minimumContext = Lua::StateContext::Client;
		void* setter = nullptr;
		void* getter = nullptr;
	};

	struct Event {
		std::string name;
		std::vector<std::pair<std::string, std::string>> args;
		uint64_t id;
		AccessLevel accessLevel = AccessLevel::Public;
		Lua::StateContext minimumContext = Lua::StateContext::Client;
		//flags?
	};

	struct Method {
		std::string name;
		std::string returnType;
		std::vector<std::pair<std::string, std::string>> args;
		uint64_t id;
		AccessLevel accessLevel = AccessLevel::Public;
		Lua::StateContext minimumContext = Lua::StateContext::Client;
		//flags?
	};

	struct Class {
		std::string className;
		uint64_t id;
		std::vector<Property> properties;
		std::vector<Event> events;
		std::vector<Method> methods;

		::std::function<::Instance* (Engine*)> constructor = nullptr;
		::std::function<::std::shared_ptr<::Instance>(Engine*)> sharedConstructor = nullptr;

		std::vector<uint64_t> publicBaseClasses;
		std::vector<uint64_t> protectedBaseClasses;
		std::vector<uint64_t> privateBaseClasses;

		std::vector<uint64_t> derivedClasses;

		bool isInterface = false;

		/// Builds a vector of all base classes all the way up the chain, closest ancestors first.
		void GetAllBaseClasses(std::vector<Class*>& classes) {
			//todo: resolve the class ids to actual class pointers
		}

		/// Builds a vector of all derived classes all the way down the chain, closest descendants first.
		void GetAllDerivedClasses(std::vector<Class*>& classes) {
			//todo: resolve the class ids to actual class pointers
		}

		void ResolvePropSetter(uint64_t propId, void* setter) {
			for (auto& prop : properties) {
				if (prop.id == propId) {
					prop.setter = setter;
					return;
				}
			}
		}
		void ResolvePropGetter(uint64_t propId, void* getter) {
			for (auto& prop : properties) {
				if (prop.id == propId) {
					prop.getter = getter;
					return;
				}
			}
		}

		const ::std::string& GetPropName(uint64_t propId) {
			for (auto& prop : properties) {
				if (prop.id == propId) {
					return prop.name;
				}
			}
			throw std::runtime_error("Invalid property id given to Class::getPropName()!");
		}

		bool IsA(std::string_view className);
		bool IsA(uint64_t classId);

		Instance* Instantiate(Engine* engine) {
			if (isInterface) {
				throw std::runtime_error("Cannot instantiate an interface class: " + className);
			}
			if (constructor) {
				return constructor(engine);
			}
			return nullptr;
		}
		std::shared_ptr<Instance> InstantiateShared(Engine* engine) {
			if (isInterface) {
				throw std::runtime_error("Cannot instantiate an interface class: " + className);
			}
			if (sharedConstructor) {
				return sharedConstructor(engine);
			}
			return nullptr;
		}

		template<typename T>
		T* InstantiateAs(Engine* engine) {
			if (!IsA(T::StaticClass().id)) {
				throw std::runtime_error("Cannot instantiate class " + className + " as " + T::StaticClass().className + "!");
			}

			Instance* inst = Instantiate(engine);
			if (inst) {
				return static_cast<T*>(inst);
			}
			return nullptr;
		}
		template<typename T>
		std::shared_ptr<T> InstantiateSharedAs(Engine* engine) {
			if (!IsA(T::StaticClass().id)) {
				throw std::runtime_error("Cannot instantiate class " + className + " as " + T::StaticClass().className + "!");
			}
			std::shared_ptr<Instance> inst = InstantiateShared(engine);
			if (inst) {
				return std::static_pointer_cast<T>(inst);
			}
			return nullptr;
		}
	};

	struct Registry {
		::std::unordered_map<::std::string_view, Class*> classes;
		::std::unordered_map<uint64_t, Class*> classesById;

		Class* GetClass(std::string_view className) {
			auto it = classes.find(className);
			if (it != classes.end()) {
				return it->second;
			}
			return nullptr;
		}
		Class* GetClassById(uint64_t classId) {
			auto it = classesById.find(classId);
			if (it != classesById.end()) {
				return it->second;
			}
			return nullptr;
		}
	};

	inline static Registry registry;

	void TrySetClass(Instance* obj, Class* cls);
	inline void TrySetClass(...) {}

	inline bool Class::IsA(std::string_view className) {
		if (this->className == className) {
			return true;
		}
		//check base classes
		for (uint64_t baseId : publicBaseClasses) {
			Class* baseClass = Reflection::registry.GetClassById(baseId);
			if (baseClass && baseClass->IsA(className)) {
				return true;
			}
		}
		for (uint64_t baseId : protectedBaseClasses) {
			Class* baseClass = Reflection::registry.GetClassById(baseId);
			if (baseClass && baseClass->IsA(className)) {
				return true;
			}
		}
		for (uint64_t baseId : privateBaseClasses) {
			Class* baseClass = Reflection::registry.GetClassById(baseId);
			if (baseClass && baseClass->IsA(className)) {
				return true;
			}
		}
		return false;
	}

	inline bool Class::IsA(uint64_t classId) {
		if (this->id == classId) {
			return true;
		}
		//check base classes
		for (uint64_t baseId : publicBaseClasses) {
			Class* baseClass = Reflection::registry.GetClassById(baseId);
			if (baseClass && baseClass->IsA(classId)) {
				return true;
			}
		}
		for (uint64_t baseId : protectedBaseClasses) {
			Class* baseClass = Reflection::registry.GetClassById(baseId);
			if (baseClass && baseClass->IsA(classId)) {
				return true;
			}
		}
		for (uint64_t baseId : privateBaseClasses) {
			Class* baseClass = Reflection::registry.GetClassById(baseId);
			if (baseClass && baseClass->IsA(classId)) {
				return true;
			}
		}
		return false;
	}
}

#define reflect(...)
#define argNames(...)
#define boundName(...)
#define summary(...)
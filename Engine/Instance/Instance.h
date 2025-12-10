#pragma once
#include "Instance/UUID.h"
#include "Instance/Reflection.h"
#include "Core/Event.h"

#include <string>
#include <memory>
#include <unordered_map>
#include "Instance.generated.h"

#pragma once
#include "Instance/UUID.h"
#include "Instance/Reflection.h"
#include "Core/Event.h"

#include <string>
#include <memory>
#include <unordered_map>
#include "Instance.generated.h"

class Engine;

template<typename Derived>
class BaseInstance {
protected:
	static bool __IsA(std::string className) {
		Reflection::Class* thisClass = Reflection::GetRegistry().GetClass(Derived::ClassName());
		if (thisClass) {
			return thisClass->IsA(className);
		}
		return false;
	}
};

class [[reflect()]] Instance : BaseInstance<Instance> {
	REFLECTION()
public: //reflected properties

	/// Do not change at runtime unless you know what you're doing.
	[[reflect(Hidden, ReadOnly, Replicated)]]
	[[summary("An internal universally unique identifier for the instance.")]]
	EngineUUID Id;

	[[reflect(Replicated)]]
	[[summary("Determines if the instance can be stored in a level file.")]]
	bool Archivable = true;

	[[reflect(Replicated)]]
	[[summary("The name of the instance, not necessarily unique.")]]
	std::string Name = "Instance";

	[[reflect(Replicated)]]
	[[summary("The parent of the instance in the tree structure.")]]
	Instance* Parent = nullptr;

	[[reflect(ReadOnly)]]
	[[summary("The children of the instance in the tree structure.")]]
	std::vector<Instance*> Children;

	[[reflect(Replicated, Hidden)]]
	[[argNames("uint64_t", "PropId")]]
	[[summary("Used internally for network replication of property changes.")]]
	MulticastEvent<uint64_t> PropertyChanged;
	
	[[reflect()]]
	[[boundName("Changed")]]
	[[argNames("PropName")]]
	[[summary("Fires when a property is changed.")]]
	MulticastEvent<std::string> LuaPropertyChanged;

	[[reflect()]]
	[[argNames("Child")]]
	[[summary("Fires when a child is added to the instance in the tree structure. The instance will already be in its parent's Children table when this is fired.")]]
	MulticastEvent<Instance*> ChildAdded;

	[[reflect()]] 
	[[argNames("Child")]] 
	[[summary("Fires when a child is removed from the instance in the tree structure. The instance will no longer be in its parent's Children table when this is fired.")]]
	MulticastEvent<Instance*> ChildRemoved;

	[[reflect()]]
	[[argNames("Child", "Parent")]] 
	[[summary("Fires when any descendant of this instance is added, and receives both the new descendant and its immediate parent. The immediate parent can be this instance. Fires immediately after ChildAdded is fired on the immediate parent.")]]
	MulticastEvent<Instance*, Instance*> DescendantAdded;

	[[reflect()]]
	[[argNames("Child", "Parent")]]
	[[summary("Fires when any descendant of this instance is removed, and receives both the removed descendant and its former immediate parent. The immediate parent can be this instance. Fires immediately after ChildRemoved is fired on the immediate parent.")]]
	MulticastEvent<Instance*, Instance*> DescendantRemoved;

	[[reflect()]] 
	[[summary("Returns a string representing the instance's location in the hierarchy, using '.' to separate names. If RelativeTo is provided, the path will be relative to that instance. If RelativeTo is not an ancestor of this instance, the full path will be returned.")]]
	std::string GetPath(Instance* RelativeTo = nullptr);

	[[reflect()]]
	[[summary("Returns true if this instance is a descendant of the other instance, false otherwise.")]]
	bool IsDescendantOf(Instance* other);

	[[reflect()]]
	[[summary("Returns true if this instance is an ancestor of the other instance, false otherwise.")]]
	bool IsAncestorOf(Instance* other);

	[[reflect()]]
	[[summary("Finds the first child with the given name, or nil if no such child exists.")]]
	Instance* FindFirstChild(std::string name);

	[[reflect()]]
	[[summary("Finds the first child of the given class, or nil if no such child exists. The optional second argument allows an \"IsA\" class check (the instance may be a subclass of the className provided.) and defaults to true.")]]
	Instance* FindFirstChildOfClass(std::string className, bool allowSubClasses = true);

	[[reflect()]]
	[[summary("Returns true if this instance is of the given class or a subclass of it, false otherwise.")]]
	bool IsA(std::string className);

	[[reflect()]]
	[[summary("Returns all descendants of this instance.")]]
	std::vector<Instance*> GetDescendants();

	template<typename T>
	bool IsA() const {
		return reflectionClass->IsA(T::StaticClass().id);
	}

	void SetClass(Reflection::Class* cls) {
		reflectionClass = cls;
	}

	Reflection::Class* GetClass() const {
		return reflectionClass;
	}
	
public: // reflected property getters and setters
	[[reflect(Setter)]]
	void SetParent(Instance* newValue);
	[[reflect(Getter)]]
	Instance* GetParent() { return Parent; }

protected:
	Reflection::Class* reflectionClass = &Reflection::reflected_Instance;

	void __onChildAdded(Instance* child);
	void __onChildRemoved(Instance* child);
	void __onDescendantAdded(Instance* child, Instance* parent);
	void __onDescendantRemoved(Instance* child, Instance* parent);
	void __onParentChanged(Instance* newParent);

	static bool __IsA(std::string className);

	std::unordered_map<std::string, MulticastEvent<>> luaPropChangeEvents;

	Engine* engine;

public:
	Engine* GetEngine() {
		return engine;
	}
	Instance(Engine* _engine);
	Instance() = delete;
};

REFLECTION_END()

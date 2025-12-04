#include "Instance/Instance.h"

Instance::Instance(Engine* _engine)
	: engine(_engine) {
	Name = ClassName();
	Id = EngineUUID();

}

std::string Instance::GetPath(Instance* RelativeTo) {
	if (Parent && RelativeTo != this) {
		return Parent->GetPath() + "." + Name;
	}
	return Name;
}

bool Instance::IsDescendantOf(Instance* other) {
	if (Parent != nullptr) {
		if (Parent == other) {
			return true;
		}
		return Parent->IsDescendantOf(other);
	}
	//can't be descendant if no parent!
	return false;
}

//todo: double check this logic lol
bool Instance::IsAncestorOf(Instance* other) {
	return other->IsDescendantOf(this);
}

Instance* Instance::FindFirstChild(std::string name) {
	for (auto& it : Children) {
		if (it->Name == name) {
			return it;
		}
	}
	return nullptr;
}

Instance* Instance::FindFirstChildOfClass(std::string className, bool allowSubClasses) {
	for (auto& it : Children) {
		if (!allowSubClasses && it->ClassName() == className) {
			return it;
		} else if (allowSubClasses && it->IsA(className)) {
			return it;
		}
	}
	return nullptr;
}

bool Instance::IsA(std::string className) {
	return reflectionClass->IsA(className);
}
bool Instance::__IsA(std::string className) {
	return Base::__IsA(className);
}

void Instance::SetParent(Instance* newValue) {
	if (newValue != Parent) {
		Instance* oldParent = Parent;
		Parent = newValue;

		if (oldParent != nullptr) {
			oldParent->__onChildRemoved(this);
		}
		if (Parent != nullptr) {
			Parent->__onChildAdded(this);
		}
		__onParentChanged(newValue);
		raisePropChanged(prop_Parent);
	}
}

void Instance::__onChildAdded(Instance* child) {
	Children.push_back(child);
	raisePropChanged(prop_Children);

	ChildAdded.Fire(child);
}

void Instance::__onChildRemoved(Instance* child) {
	for (auto it = Children.rbegin(); it != Children.rend(); it++) {
		if (*it == child) {
			Children.erase(--(it.base()));
			raisePropChanged(prop_Children);

			ChildRemoved.Fire(child);

			if (Parent) {
				Parent->__onDescendantRemoved(child, this);
			}
			break;
		}
	}
}

void Instance::__onDescendantAdded(Instance* child, Instance* parent) {
	DescendantAdded.Fire(child, parent);
	if (Parent) {
		Parent->__onDescendantAdded(child, parent);
	}
}

void Instance::__onDescendantRemoved(Instance* child, Instance* parent) {
	DescendantRemoved.Fire(child, parent);
	if (Parent) {
		Parent->__onDescendantRemoved(child, parent);
	}
}

void Instance::__onParentChanged(Instance* newParent) {
	//no-op
}

namespace Reflection {
	void TrySetClass(Instance* obj, Class* cls) {
		obj->SetClass(cls);
	}
}

std::vector<Instance*> Instance::GetDescendants() {
	std::vector<Instance*> descendants;
	for (auto child : Children) {
		descendants.push_back(child);
		auto childDescendants = child->GetDescendants();
		descendants.insert(descendants.end(), childDescendants.begin(), childDescendants.end());
	}
	return descendants;
}
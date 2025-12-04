#include "Instance/ObjectInstance.h"
#include "Instance/World.h"

void ObjectInstance::__onParentChanged(Instance* newParent) {
    World* lastWorld = world;
    if (newParent->IsA<World>()) {
        world = static_cast<World*>(newParent);
    } else if (newParent->IsA<ObjectInstance>()) {
        ObjectInstance* parentObject = static_cast<ObjectInstance*>(newParent);
        world = parentObject->world;
    }
    
    //if we see a change in world, let's propagate to all descendants.
    if (world != lastWorld) {
        std::vector<Instance*> descendants = GetDescendants();
        for (Instance* descendant : descendants) {
            if (descendant->IsA<ObjectInstance>()) {
                ObjectInstance* objDescendant = static_cast<ObjectInstance*>(descendant);
                objDescendant->world = world;
            }
        }
    }
}
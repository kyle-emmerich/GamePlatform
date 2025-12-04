#include "Physics/PhysicsSystem.h"
#include <algorithm>

void PhysicsSystem::Update(double deltaTime) {
    // Physics simulation step would go here
}

void PhysicsSystem::registerRigidBody(RigidBody* body) {
    rigidBodies.push_back(body);
}

void PhysicsSystem::unregisterRigidBody(RigidBody* body) {
    auto it = std::find(rigidBodies.begin(), rigidBodies.end(), body);
    if (it != rigidBodies.end()) {
        rigidBodies.erase(it);
    }
}

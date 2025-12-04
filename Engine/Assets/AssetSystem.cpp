#include "Assets/AssetSystem.h"

AssetHandle::AssetHandle() : system(nullptr), assetId(0) {}

AssetHandle::AssetHandle(AssetSystem* system, uint64_t assetId) 
    : system(system), assetId(assetId) {
    if (system && assetId != 0) {
        system->Acquire(assetId);
    }
}

AssetHandle::~AssetHandle() {
    if (system && assetId != 0) {
        system->Release(assetId);
    }
}

AssetHandle::AssetHandle(const AssetHandle& other) 
    : system(other.system), assetId(other.assetId) {
    if (system && assetId != 0) {
        system->Acquire(assetId);
    }
}

AssetHandle& AssetHandle::operator=(const AssetHandle& other) {
    if (this != &other) {
        if (system && assetId != 0) {
            system->Release(assetId);
        }
        system = other.system;
        assetId = other.assetId;
        if (system && assetId != 0) {
            system->Acquire(assetId);
        }
    }
    return *this;
}

AssetHandle::AssetHandle(AssetHandle&& other) noexcept 
    : system(other.system), assetId(other.assetId) {
    other.system = nullptr;
    other.assetId = 0;
}

AssetHandle& AssetHandle::operator=(AssetHandle&& other) noexcept {
    if (this != &other) {
        if (system && assetId != 0) {
            system->Release(assetId);
        }
        system = other.system;
        assetId = other.assetId;
        other.system = nullptr;
        other.assetId = 0;
    }
    return *this;
}

bool AssetHandle::IsValid() const {
    return system != nullptr && assetId != 0;
}

uint64_t AssetHandle::GetId() const {
    return assetId;
}

void AssetSystem::Acquire(uint64_t assetId) {
    referenceCounts[assetId]++;
}

void AssetSystem::Release(uint64_t assetId) {
    auto it = referenceCounts.find(assetId);
    if (it != referenceCounts.end()) {
        it->second--;
        if (it->second <= 0) {
            referenceCounts.erase(it);
        }
    }
}

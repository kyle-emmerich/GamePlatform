#include "Assets/AssetHandle.h"
#include "Assets/AssetSystem.h"

AssetHandle::AssetHandle() : system(nullptr), assetId(0) {}

AssetHandle::AssetHandle(AssetSystem* system, uint64_t assetId) 
    : system(system), assetId(assetId) {
    if (system && assetId != 0) {
        system->acquire(assetId);
    }
}

AssetHandle::~AssetHandle() {
    if (system && assetId != 0) {
        system->release(assetId);
    }
}

AssetHandle::AssetHandle(const AssetHandle& other) 
    : system(other.system), assetId(other.assetId) {
    if (system && assetId != 0) {
        system->acquire(assetId);
    }
}

AssetHandle& AssetHandle::operator=(const AssetHandle& other) {
    if (this != &other) {
        if (system && assetId != 0) {
            system->release(assetId);
        }
        system = other.system;
        assetId = other.assetId;
        if (system && assetId != 0) {
            system->acquire(assetId);
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
            system->release(assetId);
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

AssetData* AssetHandle::GetAssetData() const {
    if (system && assetId != 0) {
        return system->GetAssetData(assetId);
    }
    return nullptr;
}
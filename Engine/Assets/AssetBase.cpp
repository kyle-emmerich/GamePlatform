#include "Assets/AssetBase.h"
#include "Assets/AssetSystem.h"
#include "Core/Engine.h"

AssetBase::AssetBase(Engine* engine, uint64_t id)
    : Instance(engine) {
    if (engine) {
        AssetSystem* assetSystem = engine->GetSystem<AssetSystem>();
        if (assetSystem) {
            handle = AssetHandle(assetSystem, id);
            if (handle.IsValid()) {
                LoadFromData(handle.GetAssetData());
            }
        }
    }
}

void AssetBase::Load() {
    // Handle automatically acquires on construction/assignment
    if (handle.IsValid()) {
        LoadFromData(handle.GetAssetData());
    }
}

void AssetBase::Unload() {
    // Handle automatically releases on destruction/reassignment
    // To force unload, we could clear the handle, but we need the ID to reload.
    // For now, do nothing or implement a way to release the handle without losing ID.
}

AssetData* AssetBase::GetAssetData() const {
    return handle.GetAssetData();
}


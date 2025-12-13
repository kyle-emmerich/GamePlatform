#pragma once

#include <unordered_map>
#include "Instance/System.h"
#include "Assets/AssetBase.h"
#include "Assets/AssetHandle.h"
#include "AssetSystem.generated.h"

class AssetData;

class [[reflect()]] AssetSystem : public System, BaseInstance<AssetSystem> {
    REFLECTION()
public:
    AssetSystem(Engine* engine);
    ~AssetSystem();

    void Initialize();
    void Shutdown();

    AssetHandle LoadAssetById(uint64_t assetId);
    AssetHandle LoadAsset(const std::string& assetURI);
    void ReloadAsset(uint64_t assetId);

    bool IsAssetLoaded(uint64_t assetId);
    AssetData* GetAssetData(uint64_t assetId);

    static constexpr uint64_t LOCAL_ASSET_ID_THRESHOLD = 1ULL << 50;
    static bool IsLocalAsset(uint64_t assetId) { return assetId >= LOCAL_ASSET_ID_THRESHOLD; }

private:
    std::unordered_map<uint64_t, AssetData*> loadedAssets;
    std::unordered_map<uint64_t, int> refCounts;
    std::unordered_map<uint64_t, std::string> assetSources;
    std::unordered_map<std::string, uint64_t> uriToAssetId;

    uint64_t nextLocalAssetId = 0xFFFFFFFFFFFFFFFF;

    class DirectorySubscription* assetDirectorySubscription = nullptr;
protected:
    friend class AssetData;
    friend class AssetHandle;
    void acquire(uint64_t assetId);
    void release(uint64_t assetId);

    AssetData* getAssetData(uint64_t assetId);
};

REFLECTION_END()
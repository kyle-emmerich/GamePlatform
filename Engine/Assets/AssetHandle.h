#pragma once

#include <cinttypes>

class AssetData;
class AssetSystem;

class AssetHandle {
public:
    AssetHandle();
    AssetHandle(AssetSystem* system, uint64_t assetId);
    ~AssetHandle();

    AssetHandle(const AssetHandle& other);
    AssetHandle& operator=(const AssetHandle& other);

    AssetHandle(AssetHandle&& other) noexcept;
    AssetHandle& operator=(AssetHandle&& other) noexcept;

    bool IsValid() const;
    uint64_t GetId() const;

    AssetData* GetAssetData() const;

private:
    AssetSystem* system = nullptr;
    uint64_t assetId = 0;
};
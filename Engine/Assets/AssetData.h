#pragma once

#include <cinttypes>

enum class AssetDataOrigin : uint8_t {
    Memory = 0,
    Disk,
    CDN
};

class AssetData {
public:
    AssetData();
    AssetData(AssetDataOrigin origin, uint64_t assetId, uint8_t* rawData, size_t dataSize);
    ~AssetData();

    AssetDataOrigin GetOrigin() const { return origin; }
    uint64_t GetAssetId() const { return assetId; }
    const uint8_t* GetRawData() const { return rawData; }
    size_t GetDataSize() const { return dataSize; }

    bool IsLoaded() const { return isLoaded; }
    bool IsLoading() const { return isLoading; }
protected:
    AssetDataOrigin origin = AssetDataOrigin::Memory;
    uint64_t assetId = 0;
    uint8_t* rawData = nullptr;
    size_t dataSize = 0;

    bool isLoaded = false;
    bool isLoading = false;


};
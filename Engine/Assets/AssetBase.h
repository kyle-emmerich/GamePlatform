#pragma once
#include "Instance/Instance.h"

enum class AssetType : uint8_t {
    Unknown,
    Texture,
    Mesh,
    Audio,
    Script,
    Model,
    Font,

};

class AssetBase {
public:
    AssetBase(uint64_t id);
    ~AssetBase() = default;

    bool IsLoaded() const { return loaded; }
    bool IsLoading() const { return loading; }

    void Load();
    void Unload();

    uint64_t GetAssetId() const { return assetId; }
protected:
    bool loaded = false;
    bool loading = false;

    uint64_t assetId = 0;
};
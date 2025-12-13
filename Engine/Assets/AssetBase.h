#pragma once
#include "Instance/Instance.h"
#include "Assets/AssetHandle.h"
#include "AssetBase.generated.h"

class AssetData;

enum class AssetType : uint8_t {
    Unknown,
    Texture,
    Mesh,
    Audio,
    Script,
    Model,
    FontFace,
    FontFamily
};

class [[reflect(Abstract)]] AssetBase : public Instance, BaseInstance<AssetBase> {
    REFLECTION()
public:
    AssetBase(Engine* engine) : Instance(engine) {}
    AssetBase(Engine* engine, uint64_t id);
    ~AssetBase() = default;

    bool IsLoaded() const { return loaded; }
    bool IsLoading() const { return loading; }

    void Load();
    void Unload();

    virtual void LoadFromData(AssetData* data) {}

    uint64_t GetAssetId() const { return handle.GetId(); }
    class AssetData* GetAssetData() const;
protected:
    bool loaded = false;
    bool loading = false;

    AssetHandle handle;
};

REFLECTION_END()

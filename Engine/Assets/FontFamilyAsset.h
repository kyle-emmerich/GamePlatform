#pragma once

#include "Assets/AssetBase.h"
#include "FontFamilyAsset.generated.h"

class [[reflect()]] FontFamilyAsset : public AssetBase {
    REFLECTION()
public:
    FontFamilyAsset(Engine* engine) : AssetBase(engine) {}
    FontFamilyAsset(Engine* engine, uint64_t id) : AssetBase(engine, id) {}

    [[reflect()]]
    std::string FamilyName = "Default";
    [[reflect()]]
    std::vector<uint64_t> FontFaceAssetIds;

    void LoadFromData(AssetData* data) override;
};

REFLECTION_END()
#pragma once

#include "Assets/AssetBase.h"
#include "MeshAsset.generated.h"

class [[reflect()]] MeshAsset : public AssetBase {
    REFLECTION()
public:
    MeshAsset(Engine* engine) : AssetBase(engine) {}
    MeshAsset(Engine* engine, uint64_t id) : AssetBase(engine, id) {}
    
};

REFLECTION_END()

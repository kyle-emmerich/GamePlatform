#include "Assets/AssetBase.h"

AssetBase::AssetBase(Engine* engine, uint64_t id)
    : Instance(engine), assetId(id) {
}


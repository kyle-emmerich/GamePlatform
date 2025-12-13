#include "Assets/AssetData.h"

AssetData::AssetData() 
    : origin(AssetDataOrigin::Memory), assetId(0), rawData(nullptr), dataSize(0),
      isLoaded(false), isLoading(false) {
    
}

AssetData::AssetData(AssetDataOrigin origin, uint64_t assetId, uint8_t* rawData, size_t dataSize) 
    : origin(origin), assetId(assetId), rawData(rawData), dataSize(dataSize),
      isLoaded(true), isLoading(false) {
    
}

AssetData::~AssetData() {
    if (rawData) {
        delete[] rawData;
        rawData = nullptr;
    }
}


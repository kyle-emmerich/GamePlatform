#include "Assets/FontFamilyAsset.h"
#include "Assets/FontFaceAsset.h"
#include "Assets/AssetSystem.h"
#include "Assets/AssetData.h"
#include "Core/Engine.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void FontFamilyAsset::LoadFromData(AssetData* data) {
    if (!data) return;

    std::string jsonString((const char*)data->GetRawData(), data->GetDataSize());
    
    try {
        json j = json::parse(jsonString);

        if (j.contains("FamilyName")) {
            FamilyName = j["FamilyName"].get<std::string>();
        }

        if (j.contains("Faces") && j["Faces"].is_array()) {
            AssetSystem* assetSystem = engine->GetSystem<AssetSystem>();
            
            for (const auto& faceJson : j["Faces"]) {
                if (faceJson.contains("URI")) {
                    std::string uri = faceJson["URI"].get<std::string>();
                    
                    // Load the asset to ensure it has an ID and is available
                    AssetHandle handle = assetSystem->LoadAsset(uri);
                    uint64_t id = handle.GetId();

                    // Create the FontFaceAsset
                    // We make it a child of this FontFamilyAsset
                    FontFaceAsset* face = new FontFaceAsset(engine, id);
                    face->SetParent(this);

                    if (faceJson.contains("Weight")) {
                        face->Weight = faceJson["Weight"].get<int>();
                    }
                    if (faceJson.contains("Italic")) {
                        face->Italic = faceJson["Italic"].get<bool>();
                    }
                    
                    // We don't strictly need to store the ID in a vector if we have children,
                    // but the header has it, so let's populate it.
                    FontFaceAssetIds.push_back(id);
                }
            }
        }

    } catch (const std::exception& e) {
        // Log error?
    }
}

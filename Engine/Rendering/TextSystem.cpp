#include "Rendering/TextSystem.h"
#include "Assets/AssetSystem.h"
#include "Assets/FontFaceAsset.h"
#include "Assets/FontFamilyAsset.h"
#include "Assets/AssetData.h"
#include "Core/Engine.h"

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ot.h>
#include <iostream>

TextSystem::TextSystem(Engine* engine) : System(engine) {
    
}

void TextSystem::Initialize() {
    std::cout << "TextSystem initialized, loading default font family." << std::endl;
    LoadFontFamily("content://Fonts/ZTNature/ZTNatureFamily.json");
}

void TextSystem::Shutdown() {
    // Cleanup font cache
    for (auto& pair : fontCache) {
        hb_font_destroy(static_cast<hb_font_t*>(pair.second));
    }
    fontCache.clear();
}

void TextSystem::LoadFontFamily(const std::string& uri) {
    AssetSystem* assetSystem = engine->GetSystem<AssetSystem>();
    if (!assetSystem) return;

    AssetHandle handle = assetSystem->LoadAsset(uri);
    if (!handle.IsValid()) return;

    // Create FontFamilyAsset
    // The constructor calls LoadFromData because we updated AssetBase.cpp.
    FontFamilyAsset* family = new FontFamilyAsset(engine, handle.GetId());
    
    if (!family->FamilyName.empty()) {
        loadedFamilies[family->FamilyName] = family;
        if (!defaultFamily) {
            defaultFamily = family;
        }
    }

    std::cout << "Loaded font family: " << family->FamilyName << " with " << family->FontFaceAssetIds.size() << " faces." << std::endl;
}

FontFaceAsset* TextSystem::GetFontAsset(const std::string& familyName, int weight, bool italic) {
    FontFamilyAsset* family = nullptr;
    auto it = loadedFamilies.find(familyName);
    if (it != loadedFamilies.end()) {
        family = it->second;
    } else if (defaultFamily) {
        family = defaultFamily;
    }

    if (!family) return nullptr;

    // Find best match
    FontFaceAsset* bestMatch = nullptr;
    int bestScore = 2147483647; // INT_MAX

    const auto& children = family->GetChildren();
    for (Instance* child : children) {
        if (child->IsA("FontFaceAsset")) {
            FontFaceAsset* face = static_cast<FontFaceAsset*>(child);
            
            int score = std::abs(face->Weight - weight);
            if (face->Italic != italic) {
                score += 1000; // Penalty for wrong italic
            }

            if (score < bestScore) {
                bestScore = score;
                bestMatch = face;
            }
        }
    }

    return bestMatch;
}

FontFaceAsset* TextSystem::GetDefaultFontAsset() {
    return GetFontAsset("ZT Nature", 400, false);
}

Math::Vector2<float> TextSystem::GetTextSize(FontFaceAsset* font, float fontSize, const std::string& text) {
    if (!font) return {0, 0};
    
    auto it = fontCache.find(font->GetAssetId());
    hb_font_t* hbFont = nullptr;

    if (it != fontCache.end()) {
        hbFont = static_cast<hb_font_t*>(it->second);
    } else {
        AssetData* data = font->GetAssetData();
        if (!data) return {0, 0}; // Not loaded

        // We duplicate the data to ensure the blob remains valid even if AssetData is unloaded.
        // In a real engine, we would manage lifetime better.
        hb_blob_t* blob = hb_blob_create(
            (const char*)data->GetRawData(), 
            (unsigned int)data->GetDataSize(), 
            HB_MEMORY_MODE_DUPLICATE, 
            nullptr, 
            nullptr
        );
        
        hb_face_t* face = hb_face_create(blob, 0);
        hbFont = hb_font_create(face);
        
        // Use OpenType font functions
        hb_ot_font_set_funcs(hbFont);

        hb_face_destroy(face);
        hb_blob_destroy(blob);

        fontCache[font->GetAssetId()] = hbFont;
    }

    // Set scale to font size * 64 (26.6 fixed point)
    // We do this every time because different FontAssets might share the same underlying hb_font_t (same file) but have different sizes.
    int scale = (int)(fontSize * 64);
    hb_font_set_scale(hbFont, scale, scale);

    hb_buffer_t* buffer = hb_buffer_create();
    hb_buffer_add_utf8(buffer, text.c_str(), -1, 0, -1);
    hb_buffer_guess_segment_properties(buffer);

    hb_shape(hbFont, buffer, nullptr, 0);

    unsigned int len;
    hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(buffer, &len);

    float width = 0;
    float height = 0;

    for (unsigned int i = 0; i < len; i++) {
        width += pos[i].x_advance / 64.0f;
    }
    
    hb_font_extents_t extents;
    hb_font_get_extents_for_direction(hbFont, HB_DIRECTION_LTR, &extents);
    height = (extents.ascender - extents.descender) / 64.0f;

    hb_buffer_destroy(buffer);

    return {width, height};
}
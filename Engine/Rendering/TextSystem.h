#pragma once

#include "Instance/System.h"
#include "Math/Vector2.h"
#include "Core/Engine.h"
#include "TextSystem.generated.h"

class FontFaceAsset;
class FontFamilyAsset;

class [[reflect()]] TextSystem : public System, BaseInstance<TextSystem> {
    REFLECTION()
public:
    TextSystem(Engine* engine);

    void Register(SystemInitOrder& initOrder) {
        initOrder.After(this, "AssetSystem");
    }
    void Initialize();
    void Shutdown();

    void LoadFontFamily(const std::string& uri);

    FontFaceAsset* GetFontAsset(const std::string& familyName, int weight, bool italic);
    FontFaceAsset* GetDefaultFontAsset();

    Math::Vector2<float> GetTextSize(FontFaceAsset* font, float fontSize, const std::string& text);

private:
    std::unordered_map<uint64_t, void*> fontCache; // assetId -> hb_font_t*
    std::unordered_map<std::string, FontFamilyAsset*> loadedFamilies;
    FontFamilyAsset* defaultFamily = nullptr;
};

REFLECTION_END()    
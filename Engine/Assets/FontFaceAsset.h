#pragma once

#include "Assets/AssetBase.h"
#include "Math/Vector2.h"
#include "FontFaceAsset.generated.h"

enum class FontWeight {
    Thin = 100,
    ExtraLight = 200,
    Light = 300,
    Normal = 400,
    Medium = 500,
    SemiBold = 600,
    Bold = 700,
    ExtraBold = 800,
    Black = 900
};

class [[reflect()]] FontFaceAsset : public AssetBase {
    REFLECTION()
public:
    FontFaceAsset(Engine* engine) : AssetBase(engine) {}
    FontFaceAsset(Engine* engine, uint64_t id) : AssetBase(engine, id) {}

    [[reflect()]]
    std::string FontFamilyName = "Normal";

    [[reflect()]]
    int Weight = (int)FontWeight::Normal; // Normal weight

    [[reflect()]]
    bool Italic = false;

    Math::Vector2<float> GetTextSize(float fontSize, const std::string& text);

};

REFLECTION_END()
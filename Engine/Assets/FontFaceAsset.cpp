#include "Assets/FontFaceAsset.h"
#include "Rendering/TextSystem.h"
#include "Core/Engine.h"

Math::Vector2<float> FontFaceAsset::GetTextSize(float fontSize, const std::string& text) {
    if (engine) {
        TextSystem* textSystem = engine->GetSystem<TextSystem>();
        if (textSystem) {
            return textSystem->GetTextSize(this, fontSize, text);
        }
    }
    return Math::Vector2<float>(0.0f, 0.0f);
}
#include "Rendering/Texture2D.h"

bool Texture2D::Create(size_t width, size_t height, bgfx::TextureFormat::Enum format, uint16_t flags) {
    this->width = width;
    this->height = height;

    bgfx::TextureHandle textureHandle = bgfx::createTexture2D(
        static_cast<uint16_t>(width),
        static_cast<uint16_t>(height),
        false,
        1,
        format,
        flags
    );

    if (!bgfx::isValid(textureHandle)) {
        return false;
    }

    handle = textureHandle;
    return true;
}

void Texture2D::SetData(uint8_t* data, size_t dataSize, uint8_t mipLevel) {
    if (!bgfx::isValid(handle)) {
        return;
    }

    bgfx::updateTexture2D(
        handle,
        mipLevel,
        0,
        0,
        0,
        static_cast<uint16_t>(width >> mipLevel),
        static_cast<uint16_t>(height >> mipLevel),
        bgfx::copy(data, dataSize)
    );
}

void Texture2D::GenerateMips() {
    if (!bgfx::isValid(handle)) {
        return;
    }

    bgfx::generateMips(handle);
}
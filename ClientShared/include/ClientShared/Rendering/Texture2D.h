#pragma once

#include <bgfx/bgfx.h>

class Texture2D {
public:
    Texture2D() = default;

    bool Create(size_t width, size_t height, bgfx::TextureFormat::Enum format, uint16_t flags);
    void SetData(uint8_t* data, size_t dataSize, uint8_t mipLevel = 0);

    void GenerateMips();

protected:
    bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;

    size_t width = 0;
    size_t height = 0;
};
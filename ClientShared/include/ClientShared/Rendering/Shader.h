#pragma once
#include <bgfx/bgfx.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "Core/Export.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include "Math/Transform.h"

namespace ClientShared {

class Shader {
public:
    Shader(const std::string& vsPath, const std::string& fsPath);
    ~Shader();

    bgfx::ProgramHandle GetHandle() const { return m_program; }
    bool IsValid() const { return bgfx::isValid(m_program); }

    void SetUniform(const std::string& name, const Math::Vector4<float>& value);
    void SetUniform(const std::string& name, const Math::Color& value);
    void SetUniform(const std::string& name, const Math::Transform<float>& value);
    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, int value);

    static std::shared_ptr<Shader> Load(const std::string& name);

private:
    bgfx::ProgramHandle m_program = BGFX_INVALID_HANDLE;
    std::unordered_map<std::string, bgfx::UniformHandle> uniforms;

    bgfx::UniformHandle GetUniformHandle(const std::string& name, bgfx::UniformType::Enum type);
    
    static const bgfx::Memory* LoadMem(const std::string& path);
};

}

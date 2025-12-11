#include "ClientShared/Rendering/Shader.h"
#include "Math/Transform.h"
#include <fstream>
#include <iostream>
#include <vector>

namespace ClientShared {

Shader::Shader(const std::string& vsPath, const std::string& fsPath) {
    const bgfx::Memory* vsMem = LoadMem(vsPath);
    const bgfx::Memory* fsMem = LoadMem(fsPath);

    if (vsMem && fsMem) {
        bgfx::ShaderHandle vsh = bgfx::createShader(vsMem);
        bgfx::ShaderHandle fsh = bgfx::createShader(fsMem);
        
        if (bgfx::isValid(vsh) && bgfx::isValid(fsh)) {
            m_program = bgfx::createProgram(vsh, fsh, true); // true = destroy shaders when program is destroyed
        } else {
            std::cerr << "Failed to create shaders from files: " << vsPath << ", " << fsPath << std::endl;
            if (bgfx::isValid(vsh)) {
                bgfx::destroy(vsh);
            }
            if (bgfx::isValid(fsh)) {
                bgfx::destroy(fsh);
            }
        }
    } else {
        std::cerr << "Failed to load shader files: " << vsPath << ", " << fsPath << std::endl;
    }
}

Shader::~Shader() {
    if (bgfx::isValid(m_program)) {
        bgfx::destroy(m_program);
    }
    for (auto& pair : uniforms) {
        bgfx::destroy(pair.second);
    }
}

void Shader::SetUniform(const std::string& name, const Math::Vector4<float>& value) {
    bgfx::UniformHandle handle = GetUniformHandle(name, bgfx::UniformType::Vec4);
    bgfx::setUniform(handle, &value);
}

void Shader::SetUniform(const std::string& name, const Math::Color& value) {
    bgfx::UniformHandle handle = GetUniformHandle(name, bgfx::UniformType::Vec4);
    float params[4] = { value.R, value.G, value.B, value.A };
    bgfx::setUniform(handle, params);
}

void Shader::SetUniform(const std::string& name, const Math::Transform<float>& value) {
    // std::cout << "Setting uniform (Transform): " << name << std::endl;
    bgfx::UniformHandle handle = GetUniformHandle(name, bgfx::UniformType::Mat4);
    bgfx::setUniform(handle, value.mm);
}

void Shader::SetUniform(const std::string& name, float value) {
    bgfx::UniformHandle handle = GetUniformHandle(name, bgfx::UniformType::Vec4); // bgfx uniforms are vec4
    float params[4] = { value, 0.0f, 0.0f, 0.0f };
    bgfx::setUniform(handle, params);
}

void Shader::SetUniform(const std::string& name, int value) {
    bgfx::UniformHandle handle = GetUniformHandle(name, bgfx::UniformType::Sampler);
    bgfx::setUniform(handle, &value);
}

bgfx::UniformHandle Shader::GetUniformHandle(const std::string& name, bgfx::UniformType::Enum type) {
    auto it = uniforms.find(name);
    if (it != uniforms.end()) {
        return it->second;
    }

    bgfx::UniformHandle handle = bgfx::createUniform(name.c_str(), type);
    uniforms[name] = handle;
    return handle;
}

std::shared_ptr<Shader> Shader::Load(const std::string& name) {
    // Try multiple paths
    std::vector<std::string> searchPaths = {
        "Shaders/",
        "build/bin/Shaders/",
        "../Shaders/",
        "../../Shaders/"
    };

    for (const auto& path : searchPaths) {
        std::string vsPath = path + "vs_" + name + ".bin";
        std::string fsPath = path + "fs_" + name + ".bin";
        
        // Check if files exist
        std::ifstream vsFile(vsPath);
        std::ifstream fsFile(fsPath);
        
        if (vsFile.good() && fsFile.good()) {
            std::cout << "Loading shader from: "  << vsPath <<" and " << fsPath << std::endl;
            return std::make_shared<Shader>(vsPath, fsPath);
        }
    }
    
    std::cerr << "Failed to find shader: " << name << std::endl;
    return nullptr;
}

const bgfx::Memory* Shader::LoadMem(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    const bgfx::Memory* mem = bgfx::alloc(static_cast<uint32_t>(size) + 1);
    file.read(reinterpret_cast<char*>(mem->data), size);
    mem->data[mem->size - 1] = '\0'; // Null terminate just in case

    return mem;
}

}

project(ClientShared)

find_package(bgfx CONFIG REQUIRED)

# Define sources
set(CLIENT_SHARED_SOURCES
    src/Console.cpp
    include/ClientShared/Console.h
    src/Rendering/BgfxRenderer.cpp
    include/ClientShared/Rendering/BgfxRenderer.h
    src/Rendering/Shader.cpp
    include/ClientShared/Rendering/Shader.h
)

add_library(ClientShared STATIC ${CLIENT_SHARED_SOURCES})

# Include directories
target_include_directories(ClientShared PUBLIC 
    include
)

# Link dependencies
target_link_libraries(ClientShared PUBLIC 
    Engine
    bgfx::bgfx
)

# Definitions
target_compile_definitions(ClientShared PRIVATE CLIENT_SHARED_EXPORTS)
target_compile_definitions(ClientShared PUBLIC CLIENT_STATIC)

# Shader Compilation
if(CMAKE_HOST_WIN32)
    set(HOST_TRIPLET "x64-windows")
    set(SHADERC_NAME "shaderc.exe")
elseif(CMAKE_HOST_APPLE)
    set(HOST_TRIPLET "arm64-osx")
    set(SHADERC_NAME "shaderc")
else()
    set(HOST_TRIPLET "x64-linux")
    set(SHADERC_NAME "shaderc")
endif()

# Try to find shaderc in vcpkg tools
find_program(SHADERC_EXECUTABLE ${SHADERC_NAME} PATHS 
    "${CMAKE_BINARY_DIR}/vcpkg_installed/${HOST_TRIPLET}/tools/bgfx"
    "${CMAKE_SOURCE_DIR}/vcpkg_installed/${HOST_TRIPLET}/tools/bgfx"
    NO_DEFAULT_PATH
)

# Fallback to system search
if(NOT SHADERC_EXECUTABLE)
    find_program(SHADERC_EXECUTABLE ${SHADERC_NAME})
endif()

if(SHADERC_EXECUTABLE)
    message(STATUS "Found shaderc: ${SHADERC_EXECUTABLE}")
    
    set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders")
    set(SHADER_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders")
    file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

    file(GLOB SHADER_SOURCES "${SHADER_SOURCE_DIR}/*.sc")
    set(SHADER_OUTPUTS "")

    # Find bgfx include path
    find_path(BGFX_INCLUDE_PATH bgfx/bgfx.h PATHS
        "${CMAKE_BINARY_DIR}/vcpkg_installed/${HOST_TRIPLET}/include"
        "${CMAKE_SOURCE_DIR}/vcpkg_installed/${HOST_TRIPLET}/include"
    )
    
    # Determine platform/profile
    if(WIN32)
        set(SHADER_PLATFORM "windows")
        set(SHADER_PROFILE_VS "s_5_0")
        set(SHADER_PROFILE_FS "s_5_0")
    elseif(APPLE)
        set(SHADER_PLATFORM "osx")
        set(SHADER_PROFILE_VS "metal")
        set(SHADER_PROFILE_FS "metal")
    elseif(UNIX)
        set(SHADER_PLATFORM "linux")
        set(SHADER_PROFILE_VS "120") # OpenGL 2.1
        set(SHADER_PROFILE_FS "120")
    endif()

    foreach(SHADER_FILE ${SHADER_SOURCES})
        get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
        
        if(${SHADER_NAME} MATCHES "^vs_")
            set(SHADER_TYPE "vertex")
            set(SHADER_PROFILE ${SHADER_PROFILE_VS})
            set(OUTPUT_FILE "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.bin")
        elseif(${SHADER_NAME} MATCHES "^fs_")
            set(SHADER_TYPE "fragment")
            set(SHADER_PROFILE ${SHADER_PROFILE_FS})
            set(OUTPUT_FILE "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.bin")
        else()
            continue()
        endif()
        
        add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${SHADERC_EXECUTABLE}
            ARGS -f ${SHADER_FILE} -o ${OUTPUT_FILE} --type ${SHADER_TYPE} --platform ${SHADER_PLATFORM} --profile ${SHADER_PROFILE} -i ${SHADER_SOURCE_DIR} -i "${CMAKE_BINARY_DIR}/vcpkg_installed/${HOST_TRIPLET}/include" -i "${CMAKE_BINARY_DIR}/vcpkg_installed/${HOST_TRIPLET}/include/bgfx" -i ${BGFX_INCLUDE_PATH} -i ${BGFX_INCLUDE_PATH}/bgfx --varyingdef ${SHADER_SOURCE_DIR}/ui_varying.def.sc
            DEPENDS ${SHADER_FILE}
            COMMENT "Compiling shader ${SHADER_NAME}"
        )
        list(APPEND SHADER_OUTPUTS ${OUTPUT_FILE})
    endforeach()

    add_custom_target(CompileShaders ALL DEPENDS ${SHADER_OUTPUTS})
    add_dependencies(ClientShared CompileShaders)
else()
    message(WARNING "shaderc not found! Shaders will not be compiled.")
endif()

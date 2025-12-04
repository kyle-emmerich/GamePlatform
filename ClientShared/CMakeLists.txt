project(ClientShared)

find_package(bgfx CONFIG REQUIRED)

# Define sources
set(CLIENT_SHARED_SOURCES
    src/Viewport.cpp
    include/ClientShared/Viewport.h
    src/PlatformWindow.cpp
    include/ClientShared/PlatformWindow.h
    src/Console.cpp
    include/ClientShared/Console.h
    include/ClientShared/IRenderable.h
    src/Rendering/BgfxRenderer.cpp
    include/ClientShared/Rendering/BgfxRenderer.h
)

add_library(ClientShared SHARED ${CLIENT_SHARED_SOURCES})

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

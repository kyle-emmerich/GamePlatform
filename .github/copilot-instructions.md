# GamePlatform AI Instructions

## Project Overview
GamePlatform is a C++17 multiplayer game engine and platform. It uses **Luau** for scripting and **CMake** for building. The architecture consists of a core `Engine` library, shared client code, and platform-specific entry points (Windows, macOS, GameServer).

## Architecture & Core Systems

### 1. Engine Core (`Engine/`)
- **Instance System:** The root of the object hierarchy is `Instance`. All game objects must inherit from `Instance` and `BaseInstance<T>`. This provides parent/child relationships, lifecycle management, and reflection capabilities.
- **Reflection:** A custom Python-based reflection system (`ReflectionGenerator/parse.py`) parses C++ headers and generates C++ code for runtime type information, serialization, networking, and scripting bindings. It also generates `reflection_data.json` for direct use by the documentation generator.
- **Engine:** The `Engine` class manages core systems, including the main update loop, system initialization, and shutdown. No global state is used; everything is encapsulated within the `Engine` instance.
- **Systems:** Core systems inherit from `System` and are automatically discovered and initialized via the reflection system.
- **Scripting:** **Luau** is used for game logic. The engine binds C++ objects to Luau via the reflection system.
- **Rendering:** Uses **bgfx**.
- **Networking:** Uses **GameNetworkingSockets**.

### 2. Reflection System
The reflection system is critical for serialization, networking, and scripting.
- **Generator:** `ReflectionGenerator/parse.py` uses `tree-sitter-cpp` to parse headers.
- **Output:** Generates `*.generated.h` files in `Engine/Generated/` and a central `ReflectionRegistry.h`. The `Engine` class initializes the registry and immediately uses it to find all `System`-derived classes and initialize them.
- **Triggers:** The generator runs as a CMake custom command.

**How to Reflect a Class:**
1. Inherit from `Instance` and `BaseInstance<ClassName>`.
2. Add `[[reflect()]]` attribute to the class.
3. Add `REFLECTION()` macro inside the class body.
4. Add `REFLECTION_END()` macro after the class definition (or at end of file).
5. Mark properties/methods with `[[reflect()]]`.
6. Use `GP_EXPORT` for cross-module visibility.
7. Reflected properties _must_ be accessed via getters/setters in C++ to ensure property change events are fired and state remains consistent.

**Example:**
```cpp
#include "Instance/Instance.h"
#include "MyClass.generated.h" // Generated header

class GP_EXPORT [[reflect()]] MyClass : public Instance, BaseInstance<MyClass> {
    REFLECTION()
public:
    [[reflect(ReadOnly, Replicated)]]
    int myValue;

    [[reflect()]]
    void MyMethod();
};
REFLECTION_END()
```

### 3. Directory Structure
- `Engine/`: Core engine source code.
- `ClientShared/`: Code shared between Mac and Windows clients.
- `MacClient/`, `WindowsClient/`: Platform-specific application entry points.
- `GameServerWindows/`: Dedicated server application.
- `ReflectionGenerator/`: Python tools for code generation.
- `vcpkg.json`: Dependency manifest.

## Build & Development

### Build System
- **CMake** is the build system.
- **vcpkg** manages dependencies (`bgfx`, `protobuf`, `openssl`).
- **Presets:** Use `CMakePresets.json` if available, or standard CMake commands.

**Common Commands:**
- **Configure:** `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake`
- **Build:** `cmake --build build`

### Key Workflows
- **Adding a new System:** Create a new class in `Engine/` (e.g., `Engine/MySystem/`), inherit from `System` (if applicable) or `Instance`. Ensure it's reflected.
- **Updating Reflection:** The build system should handle this, but if reflection data is stale, force a rebuild or check `ReflectionGenerator/parse.py` output.
- **Missing Generated Methods:** If you encounter missing methods that should be generated (like `GetProperty` for a reflected property), run the reflection parser manually: `python3 ReflectionGenerator/parse.py Engine`.
- **Scripting:** Lua scripts interact with reflected C++ objects. Ensure properties/methods intended for scripts are marked `[[reflect()]]`.

## Conventions
- **Code Style:** Follow existing C++17 patterns.
- **Macros:** Use `GP_EXPORT` for classes/functions exposed from the Engine DLL.
- **Headers:** Include generated headers as `#include "ClassName.generated.h"`.
- **Memory:** Use `std::shared_ptr` and `std::weak_ptr` for object lifetime management where appropriate, but `Instance` hierarchy has its own parent/child management.
- **Initialization:** Do not call `renderer->Initialize()` manually in client entry points. It is called automatically when the first `Viewport` is attached to a window via `Viewport::AttachToWindow`.

## Rendering Guidelines (bgfx)
- **Matrices:** `Math::Transform` uses **Column-Major** layout to match `bgfx` expectations.
- **Shaders:**
  - Use `bgfx_shader.sh` include.
  - Vertex Shaders: Use `mul(u_viewProj, vec4(pos, 1.0))` for projection.
  - Uniforms: `bgfx` uniforms are typically `vec4` or `mat4`.
- **View Management:**
  - Set View Transform (Projection/View matrices) in `BeginFrame` or when the camera changes.
  - **DO NOT** call `bgfx::setViewTransform` with `NULL` inside draw calls unless you intend to reset the projection for that specific call (which usually results in invisible geometry if not handled correctly).
- **Debugging:**
  - Use `bgfx::setViewClear` with distinct colors to verify viewports are rendering.
  - If geometry is invisible, check:
    1. Winding order (try disabling culling `bgfx::setState(..., 0)`).
    2. Projection matrix (Ortho vs Perspective).
    3. View Transform resets (ensure `setViewTransform` isn't being cleared).
    4. Shader output (force a constant color in Fragment shader).

## Dependencies
- **Luau:** Scripting language.
- **bgfx:** Rendering backend.
- **GameNetworkingSockets:** UDP networking.
- **protobuf:** Data serialization.

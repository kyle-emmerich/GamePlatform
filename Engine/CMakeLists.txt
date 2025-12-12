project(Engine)

include(FetchContent)

# Fetch bgfx and dependencies via vcpkg
find_package(bgfx CONFIG REQUIRED)

# Add ThirdParty dependencies
# We need to make sure we don't build tests/examples for them to save time/mess
set(LUAU_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LUAU_BUILD_CLI OFF CACHE BOOL "" FORCE)
add_subdirectory(ThirdParty/luau)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIB OFF CACHE BOOL "" FORCE)
add_subdirectory(ThirdParty/GameNetworkingSockets)

# Reflection Generation
set(PYTHON_EXECUTABLE "${CMAKE_SOURCE_DIR}/ReflectionGenerator/.venv/Scripts/python.exe")
if(NOT EXISTS ${PYTHON_EXECUTABLE})
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
endif()

# Install requirements
message(STATUS "Installing Python requirements...")
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -m pip install -r ${CMAKE_SOURCE_DIR}/ReflectionGenerator/requirements.txt
    OUTPUT_QUIET
    ERROR_VARIABLE PIP_ERROR
    RESULT_VARIABLE PIP_RESULT
)
if(NOT PIP_RESULT EQUAL 0)
    message(WARNING "Failed to install Python requirements: ${PIP_ERROR}")
endif()

# Ensure Generated directory exists
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Generated)

# Collect headers for dependency tracking
file(GLOB_RECURSE ENGINE_HEADERS 
    "Core/*.h"
    "Assets/*.h"
    "Input/*.h"
    "Instance/*.h"
    "Math/*.h"
    "Network/*.h"
    "Platform/*.h"
    "Physics/*.h"
    "Rendering/*.h"
    "Replication/*.h"
    "Scripting/*.h"
    "UI/*.h"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Generated/ReflectionRegistry.h
           ${CMAKE_CURRENT_SOURCE_DIR}/Generated/reflection_data.json
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/ReflectionGenerator/parse.py ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${CMAKE_SOURCE_DIR}/ReflectionGenerator/parse.py ${ENGINE_HEADERS}
    COMMENT "Generating Reflection data..."
)

# Collect source files
file(GLOB_RECURSE ENGINE_SOURCES 
    "Core/*.cpp" "Core/*.h"
    "Assets/*.cpp" "Assets/*.h"
    "Input/*.cpp" "Input/*.h"
    "Instance/*.cpp" "Instance/*.h"
    "Math/*.cpp" "Math/*.h"
    "Network/*.cpp" "Network/*.h"
    "Platform/*.cpp" "Platform/*.h"
    "Physics/*.cpp" "Physics/*.h"
    "Rendering/*.cpp" "Rendering/*.h"
    "Replication/*.cpp" "Replication/*.h"
    "Scripting/*.cpp" "Scripting/*.h"
    "UI/*.cpp" "UI/*.h"
    "Generated/*.cpp" "Generated/*.h"
)

# Add generated files to sources to trigger generation
list(APPEND ENGINE_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Generated/ReflectionRegistry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Generated/reflection_data.json
)

message(STATUS "Engine sources: ${ENGINE_SOURCES}")

# Add the library
add_library(Engine STATIC ${ENGINE_SOURCES})

if(TARGET Engine)
    message(STATUS "Engine target created successfully")
else()
    message(FATAL_ERROR "Engine target NOT created")
endif()

# Export all symbols on Windows (since we might not have declspec(dllexport) everywhere)
set_target_properties(Engine PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Include directories
target_include_directories(Engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Generated
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/luau/VM/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/luau/Compiler/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GameNetworkingSockets/include
)

# Link libraries
target_link_libraries(Engine PUBLIC 
    GameNetworkingSockets
    Luau.Compiler
    Luau.VM
    bgfx::bgfx
)

# Definitions
target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)
target_compile_definitions(Engine PUBLIC GP_STATIC)


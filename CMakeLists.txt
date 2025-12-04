cmake_minimum_required(VERSION 3.20)

# Set toolchain if available and not already set
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(VCPKG_TOOLCHAIN "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    if(EXISTS "${VCPKG_TOOLCHAIN}")
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN}")
    endif()
endif()

project(GamePlatform VERSION 0.1.0 LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
    add_compile_options(/Zc:preprocessor)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Platform Detection
if(WIN32)
    add_compile_definitions(PLATFORM_WINDOWS)
    message(STATUS "Platform: Windows")
elseif(APPLE)
    # Check for iOS vs macOS
    if(IOS)
        add_compile_definitions(PLATFORM_IOS)
        message(STATUS "Platform: iOS")
    else()
        add_compile_definitions(PLATFORM_MAC)
        message(STATUS "Platform: macOS")
    endif()
elseif(UNIX AND NOT APPLE)
    if(ANDROID)
        add_compile_definitions(PLATFORM_ANDROID)
        message(STATUS "Platform: Android")
    else()
        add_compile_definitions(PLATFORM_LINUX)
        message(STATUS "Platform: Linux")
    endif()
endif()

option(BUILD_XBOX "Build for Xbox" OFF)
if(BUILD_XBOX)
    add_compile_definitions(PLATFORM_XBOX)
endif()

# Add subdirectories
add_subdirectory(ReflectionGenerator)
add_subdirectory(Engine)
add_subdirectory(ClientShared)

if(WIN32)
    add_subdirectory(WindowsClient)
    add_subdirectory(GameServerWindows)
endif()

if(APPLE AND NOT IOS)
    add_subdirectory(MacClient)
endif()

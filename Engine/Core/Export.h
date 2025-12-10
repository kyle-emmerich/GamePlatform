#pragma once

#if defined(GP_STATIC)
    #define GP_EXPORT
    #define GP_IMPORT
#elif defined(WIN32)
    #define GP_EXPORT __declspec(dllexport)
    #define GP_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) && __GNUC__ >= 4
    #define GP_EXPORT __attribute__((visibility("default")))
    #define GP_IMPORT __attribute__((visibility("default")))
#elif defined(__clang__) && __clang_major__ >= 4
    #define GP_EXPORT __attribute__((visibility("default")))
    #define GP_IMPORT __attribute__((visibility("default")))
#else
    #define GP_EXPORT
    #define GP_IMPORT
#endif

#pragma once

#ifdef WIN32
    #define GP_EXPORT __declspec(dllexport)
    #define GP_IMPORT __declspec(dllimport)
#else
    #define GP_EXPORT
    #define GP_IMPORT
#endif

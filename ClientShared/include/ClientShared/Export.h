#pragma once
#include "Core/Export.h"

#if defined(CLIENT_STATIC)
    #define CLIENT_API
#elif defined(CLIENT_SHARED_EXPORTS)
    #define CLIENT_API GP_EXPORT
#else
    #define CLIENT_API GP_IMPORT
#endif

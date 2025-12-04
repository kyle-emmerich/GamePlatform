#pragma once
#include "Core/Export.h"

#ifdef CLIENT_SHARED_EXPORTS
    #define CLIENT_API GP_EXPORT
#else
    #define CLIENT_API GP_IMPORT
#endif

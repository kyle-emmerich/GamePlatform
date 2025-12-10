#include "Instance/Reflection.h"

Reflection::Registry& Reflection::GetRegistry() {
    if (!registry) {
        registry = new Registry();
    }
    return *registry;
}
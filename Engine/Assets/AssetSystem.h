#pragma once

#include "Instance/System.h"
#include "AssetSystem.generated.h"
#include <unordered_map>

class AssetSystem;

class AssetHandle {
public:
    AssetHandle();
    AssetHandle(AssetSystem* system, uint64_t assetId);
    ~AssetHandle();

    AssetHandle(const AssetHandle& other);
    AssetHandle& operator=(const AssetHandle& other);

    AssetHandle(AssetHandle&& other) noexcept;
    AssetHandle& operator=(AssetHandle&& other) noexcept;

    bool IsValid() const;
    uint64_t GetId() const;

private:
    AssetSystem* system = nullptr;
    uint64_t assetId = 0;
};

class [[reflect()]] AssetSystem : public System, BaseInstance<AssetSystem> {
    REFLECTION()
public:
    AssetSystem(Engine* engine) : System(engine) {}

    void Acquire(uint64_t assetId);
    void Release(uint64_t assetId);

private:
    std::unordered_map<uint64_t, int> referenceCounts;
};

REFLECTION_END()
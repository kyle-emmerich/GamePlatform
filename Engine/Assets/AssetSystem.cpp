#include "Assets/AssetSystem.h"
#include "Assets/AssetData.h"
#include "Core/Engine.h"
#include "Core/IFileSystemWatcher.h"

#include <filesystem>
#include <string>
#include <fstream>
#include <vector>

// FNV-1a hash
constexpr uint64_t HashString(const std::string_view& str) {
    uint64_t hash = 14695981039346656037ULL;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}

AssetSystem::AssetSystem(Engine* engine)
    : System(engine) {
    //We'll need a file watcher.
    const std::string_view& assetCacheDir = engine->GetAssetCacheDirectory();

    //ensure asset cache directory exists
    std::filesystem::create_directories(std::string(assetCacheDir));

    assetDirectorySubscription = engine->GetFileSystemWatcher()->Subscribe(std::string(assetCacheDir));
}

AssetSystem::~AssetSystem() {
    if (assetDirectorySubscription) {
        Engine* engine = static_cast<Engine*>(GetEngine());
        if (engine && engine->GetFileSystemWatcher()) {
            engine->GetFileSystemWatcher()->Unsubscribe(assetDirectorySubscription);
        }
        assetDirectorySubscription = nullptr;
    }

    //Unload all assets
    for (auto& pair : loadedAssets) {
        delete pair.second;
    }
    loadedAssets.clear();
}

void AssetSystem::Initialize() {
    // Initialization logic if needed
}

void AssetSystem::Shutdown() {
    
}

void AssetSystem::acquire(uint64_t assetId) {
    refCounts[assetId]++;

    if (loadedAssets.find(assetId) != loadedAssets.end()) {
        return;
    }

    // Load the asset data
    auto sourceIt = assetSources.find(assetId);
    if (sourceIt != assetSources.end()) {
        std::string path = sourceIt->second;
        
        // If path starts with "file://", strip it
        if (path.rfind("file://", 0) == 0) {
            path = path.substr(7);
        } else if (path.rfind("content://", 0) == 0) {
            // content:// maps to Content/ folder relative to working directory
            Engine* engine = static_cast<Engine*>(GetEngine());
            std::string contentDir(engine->GetContentDirectory());
            if (!contentDir.empty() && contentDir.back() != '/' && contentDir.back() != '\\') {
                contentDir += '/';
            }
            path = contentDir + path.substr(10);
        }

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            if (size > 0) {
                uint8_t* buffer = new uint8_t[size];
                if (file.read((char*)buffer, size)) {
                    AssetData* assetData = new AssetData(AssetDataOrigin::Disk, assetId, buffer, size);
                    loadedAssets[assetId] = assetData;
                } else {
                    delete[] buffer;
                    // Failed to read
                }
            }
        }
    }
}

void AssetSystem::release(uint64_t assetId) {
    if (refCounts[assetId] > 0) {
        refCounts[assetId]--;
        if (refCounts[assetId] == 0) {
            auto it = loadedAssets.find(assetId);
            if (it != loadedAssets.end()) {
                delete it->second;
                loadedAssets.erase(it);
            }
        }
    }
}

AssetData* AssetSystem::GetAssetData(uint64_t assetId) {
    auto it = loadedAssets.find(assetId);
    if (it != loadedAssets.end()) {
        return it->second;
    }
    return nullptr;
}

bool AssetSystem::IsAssetLoaded(uint64_t assetId) {
    return loadedAssets.find(assetId) != loadedAssets.end();
}

AssetHandle AssetSystem::LoadAssetById(uint64_t assetId) {
    // This assumes the asset source is already known or we can't load it.
    // But AssetHandle constructor calls acquire(), which will try to load.
    return AssetHandle(this, assetId);
}

AssetHandle AssetSystem::LoadAsset(const std::string& assetURI) {
    uint64_t assetId = 0;
    
    // Check for "asset://<id>" format
    if (assetURI.rfind("asset://", 0) == 0) {
        std::string idStr = assetURI.substr(8);
        try {
            assetId = std::stoull(idStr);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid asset URI: " + assetURI);
        }
    } else {
        // Assume it's a file path or "file://" URI
        
        // Check if we already have an ID for this URI
        auto it = uriToAssetId.find(assetURI);
        if (it != uriToAssetId.end()) {
            assetId = it->second;
        } else {
            // Assign new local ID
            assetId = nextLocalAssetId--;
            // Ensure we don't collide with non-local assets (though unlikely with 2^50 gap)
            if (assetId < LOCAL_ASSET_ID_THRESHOLD) {
                // Reset or handle overflow? For now, just log or throw if we were using logging.
                // In practice, 2^14 assets is a lot for a session.
            }
            
            uriToAssetId[assetURI] = assetId;
            assetSources[assetId] = assetURI;
        }
    }

    return AssetHandle(this, assetId);
}

void AssetSystem::ReloadAsset(uint64_t assetId) {
    //not yet implemented
}
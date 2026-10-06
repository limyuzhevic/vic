#pragma once

#include "PluginLoader.hpp"
#include "../core/Logger/Logger.hpp"
#include <dlfcn.h>
#include <fstream>
#include <sstream>
#include <cstring>

namespace nlm {

class PluginLoader {
public:
    bool validatePluginSignature(const std::string& pluginPath) {
        // In a real implementation, this would validate the plugin signature
        // For now, just check if file exists and has read permissions
        std::ifstream file(pluginPath, std::ios::binary);
        if (!file) {
            NLM_LOG_ERROR(std::string("Plugin file not found: ") + pluginPath);
            return false;
        }
        
        // Check file size (should be reasonable for a plugin)
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        if (fileSize < 1024) {
            NLM_LOG_ERROR(std::string("Plugin file too small: ") + pluginPath);
            return false;
        }
        
        if (fileSize > 100 * 1024 * 1024) { // 100MB limit
            NLM_LOG_ERROR(std::string("Plugin file too large: ") + pluginPath);
            return false;
        }
        
        return true;
    }
    
    bool loadPluginLibrary(const std::string& pluginPath, void*& handle) {
        // Load dynamic library
        handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
        if (!handle) {
            NLM_LOG_ERROR(std::string("Failed to load plugin library: ") + dlerror());
            return false;
        }
        
        // Cache the handle for later cleanup
        cachedHandles_[pluginPath] = handle;
        return true;
    }
    
    void* getPluginSymbol(void* handle, const std::string& symbolName) {
        if (!handle) {
            return nullptr;
        }
        
        // Clear any existing dlerror
        dlerror();
        
        // Get symbol
        void* symbol = dlsym(handle, symbolName.c_str());
        
        // Check for errors
        const char* error = dlerror();
        if (error) {
            NLM_LOG_ERROR(std::string("Failed to get symbol ") + symbolName + ": " + error);
            return nullptr;
        }
        
        return symbol;
    }
    
    void closePluginLibrary(void* handle) {
        if (!handle) {
            return;
        }
        
        // Close library
        if (dlclose(handle) != 0) {
            NLM_LOG_ERROR(std::string("Failed to close plugin library: ") + dlerror());
        }
        
        // Remove from cache
        for (auto it = cachedHandles_.begin(); it != cachedHandles_.end(); ++it) {
            if (it->second == handle) {
                cachedHandles_.erase(it);
                break;
            }
        }
    }
    
    void clearCache() {
        // Close all cached libraries
        for (const auto& pair : cachedHandles_) {
            closePluginLibrary(pair.second);
        }
        cachedHandles_.clear();
    }
    
private:
    std::unordered_map<std::string, void*> cachedHandles_;
};

} // namespace nlm

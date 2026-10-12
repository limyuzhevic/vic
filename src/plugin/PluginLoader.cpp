#include "PluginLoader.hpp"
#include "../core/ErrorHandling.hpp"
#include <dlfcn.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <cstring>

namespace nlm {

struct PluginLoader::Impl {
    std::map<std::string, PluginFactory> factories;
    std::map<std::string, void*> loadedLibraries;
    std::mutex mutex;
};

PluginLoader::PluginLoader() : pImpl(std::make_unique<Impl>()) {}
PluginLoader::~PluginLoader() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& pair : pImpl->loadedLibraries) {
        if (pair.second) {
            unloadNativeLibrary(pair.second);
        }
    }
    pImpl->loadedLibraries.clear();
}

PluginLoader::LoadResult PluginLoader::loadPlugin(const std::string& filepath, PluginType expectedType) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LoadResult result;
    
    if (!isValidPluginFile(filepath)) {
        result.errorMessage = "Invalid plugin file: " + filepath;
        return result;
    }
    
    try {
        // Load native library
        void* handle = loadNativeLibrary(filepath);
        if (!handle) {
            result.errorMessage = "Failed to load native library: " + filepath;
            return result;
        }
        
        // Try to get factory function
        PluginFactory factory = getFactoryFromLibrary(handle);
        if (!factory) {
            unloadNativeLibrary(handle);
            result.errorMessage = "No plugin factory found in: " + filepath;
            return result;
        }
        
        // Create plugin instance
        std::shared_ptr<Plugin> plugin = factory();
        if (!plugin) {
            unloadNativeLibrary(handle);
            result.errorMessage = "Failed to create plugin instance from: " + filepath;
            return result;
        }
        
        // Store handle for later unloading
        pImpl->loadedLibraries[plugin->getInstanceId()] = handle;
        
        // Set factory for potential reuse
        registerPluginFactory(plugin->getMetadata().name, factory, expectedType);
        
        result = LoadResult(plugin, true, "", plugin->getMetadata().version);
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception loading plugin: ") + e.what();
    }
    
    return result;
}

void PluginLoader::registerPluginFactory(const std::string& name, PluginFactory factory, PluginType type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->factories[name] = factory;
}

PluginLoader::LoadResult PluginLoader::createPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LoadResult result;
    
    auto it = pImpl->factories.find(name);
    if (it == pImpl->factories.end()) {
        result.errorMessage = "No factory registered for plugin: " + name;
        return result;
    }
    
    try {
        std::shared_ptr<Plugin> plugin = it->second();
        if (!plugin) {
            result.errorMessage = "Failed to create plugin from factory: " + name;
            return result;
        }
        
        result = LoadResult(plugin, true, "", plugin->getMetadata().version);
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception creating plugin: ") + e.what();
    }
    
    return result;
}

bool PluginLoader::unloadPlugin(const std::string& instanceId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->loadedLibraries.find(instanceId);
    if (it == pImpl->loadedLibraries.end()) {
        return false; // Plugin not found
    }
    
    unloadNativeLibrary(it->second);
    pImpl->loadedLibraries.erase(it);
    return true;
}

bool PluginLoader::isValidPluginFile(const std::string& filepath) const {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    // Check file extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    static const std::vector<std::string> validExtensions = {".so", ".dll", ".dylib"};
    return std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end();
}

std::vector<std::string> PluginLoader::getSupportedExtensions() {
    return {".so", ".dll", ".dylib"};
}

void* PluginLoader::loadNativeLibrary(const std::string& filepath) {
    int flags = RTLD_NOW | RTLD_LOCAL;
    
    void* handle = dlopen(filepath.c_str(), flags);
    if (!handle) {
        char* error = dlerror();
        if (error) {
            std::cerr << "dlopen error: " << error << std::endl;
        }
    }
    
    return handle;
}

void PluginLoader::unloadNativeLibrary(void* handle) {
    if (handle) {
        dlclose(handle);
    }
}

void* PluginLoader::getNativeSymbol(void* handle, const std::string& symbol) {
    if (!handle) return nullptr;
    
    dlerror(); // Clear any existing error
    void* result = dlsym(handle, symbol.c_str());
    
    char* error = dlerror();
    if (error) {
        std::cerr << "dlsym error for symbol '" << symbol << "': " << error << std::endl;
        return nullptr;
    }
    
    return result;
}

PluginLoader::PluginFactory PluginLoader::getFactoryFromLibrary(void* handle, const std::string& factorySymbol) {
    if (!handle) return nullptr;
    
    void* symbol = getNativeSymbol(handle, factorySymbol);
    if (!symbol) {
        // Try alternative factory names
        const std::vector<std::string> alternativeSymbols = {
            "PluginFactory",
            "getPlugin",
            "createPluginInstance",
            "CreatePlugin"
        };
        
        for (const auto& alt : alternativeSymbols) {
            symbol = getNativeSymbol(handle, alt);
            if (symbol) {
                return reinterpret_cast<PluginFactory>(symbol);
            }
        }
        
        return nullptr;
    }
    
    return reinterpret_cast<PluginFactory>(symbol);
}

} // namespace nlm
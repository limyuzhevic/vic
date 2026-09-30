#include "include/nlm/plugin/PluginLoader.h"
#include "include/nlm/plugin/IPlugin.h"
#include <dlfcn.h>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace nlm {
namespace plugin {

PluginLoader::PluginLoader() {
}

PluginLoader::~PluginLoader() {
    for (const auto& pair : loadedLibraries_) {
        unloadLibrary(pair.second);
    }
}

bool PluginLoader::loadPlugin(const std::string& pluginPath, std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);

    void* handle = loadLibrary(pluginPath);
    if (!handle) {
        return false;
    }

    if (!resolveSymbols(handle, pluginId)) {
        unloadLibrary(handle);
        return false;
    }

    loadedLibraries_[pluginId] = handle;
    return true;
}

bool PluginLoader::unloadPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = loadedLibraries_.find(pluginId);
    if (it == loadedLibraries_.end()) {
        return false;
    }

    if (unloadLibrary(it->second)) {
        destroyFunctions_.erase(pluginId);
        createFunctions_.erase(pluginId);
        loadedLibraries_.erase(it);
        return true;
    }

    return false;
}

bool PluginLoader::isPluginLoaded(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return loadedLibraries_.find(pluginId) != loadedLibraries_.end();
}

template<typename T>
bool PluginLoader::getPluginSymbol(const std::string& pluginId, const std::string& symbolName, T& outSymbol) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = loadedLibraries_.find(pluginId);
    if (it == loadedLibraries_.end()) {
        return false;
    }

    void* symbol = dlsym(it->second, symbolName.c_str());
    if (!symbol) {
        return false;
    }

    outSymbol = reinterpret_cast<T>(symbol);
    return true;
}

void* PluginLoader::loadLibrary(const std::string& pluginPath) {
#ifdef _WIN32
    return LoadLibrary(pluginPath.c_str());
#else
    return dlopen(pluginPath.c_str(), RTLD_LAZY);
#endif
}

bool PluginLoader::unloadLibrary(void* handle) {
#ifdef _WIN32
    return FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
    return dlclose(handle) == 0;
#endif
}

bool PluginLoader::resolveSymbols(void* handle, const std::string& pluginId) {
    CreatePluginFunction createFunc = reinterpret_cast<CreatePluginFunction>(dlsym(handle, "createPlugin"));
    DestroyPluginFunction destroyFunc = reinterpret_cast<DestroyPluginFunction>(dlsym(handle, "destroyPlugin"));

    if (!createFunc || !destroyFunc) {
        return false;
    }

    createFunctions_[pluginId] = createFunc;
    destroyFunctions_[pluginId] = destroyFunc;

    return true;
}

std::vector<std::string> PluginLoader::getSupportedPluginExtensions() {
    return {".so", ".dll", ".dylib"};
}

std::vector<std::string> PluginLoader::findPluginsInDirectory(const std::string& directory) {
    std::vector<std::string> plugins;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string extension = entry.path().extension().string();

                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                auto supportedExtensions = getSupportedPluginExtensions();
                if (std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end()) {
                    plugins.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error scanning directory " << directory << ": " << e.what() << std::endl;
    }

    return plugins;
}

std::string PluginLoader::getCurrentPlatformName() {
#ifdef _WIN32
    return "Windows";
#elif __APPLE__
    return "macOS";
#else
    return "Linux";
#endif
}

} // namespace plugin
} // namespace nlm

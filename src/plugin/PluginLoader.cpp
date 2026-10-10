// PluginLoader.cpp - Implementation of dynamic plugin loader
#include "PluginLoader.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace nlm {

DynamicPluginLoader::~DynamicPluginLoader() {
    // Unload all loaded plugins
    for (const auto& pair : loadedHandles) {
        unloadLibrary(pair.first, pair.second);
    }
}

bool DynamicPluginLoader::loadLibrary(const std::string& path, PluginHandle& handle) {
#ifdef _WIN32
    handle = LoadLibraryA(path.c_str());
    if (!handle) {
        lastError = "Failed to load library: " + path;
        return false;
    }
#else
    // Try loading as shared library (.so, .dylib)
    handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        lastError = "Failed to load library: " + std::string(dlerror());
        return false;
    }
#endif
    return true;
}

void DynamicPluginLoader::unloadLibrary(const std::string& path, PluginHandle& handle) {
    if (!handle) return;
    
#ifdef _WIN32
    FreeLibrary(static_cast<HMODULE>(handle));
#else
    dlclose(handle);
#endif
    handle = nullptr;
}

void* DynamicPluginLoader::getSymbol(PluginHandle handle, const std::string& symbol) {
#ifdef _WIN32
    return GetProcAddress(static_cast<HMODULE>(handle), symbol.c_str());
#else
    dlerror(); // Clear any existing errors
    void* symbolPtr = dlsym(handle, symbol.c_str());
    const char* error = dlerror();
    if (error) {
        lastError = "Failed to get symbol " + symbol + ": " + std::string(error);
    }
    return symbolPtr;
#endif
}

std::shared_ptr<IPlugin> DynamicPluginLoader::loadPlugin(const std::string& pluginPath) {
    if (!canLoadPlugin(pluginPath)) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check if already loaded
    if (isPluginLoaded(pluginPath)) {
        return nullptr;
    }
    
    PluginHandle handle;
    if (!loadLibrary(pluginPath, handle)) {
        return nullptr;
    }
    
    // Try to get the plugin factory function
    PluginCreateFunc createFunc = nullptr;
    void* symbol = getSymbol(handle, "createPlugin");
    if (symbol) {
        createFunc = reinterpret_cast<PluginCreateFunc>(symbol);
    }
    
    if (!createFunc) {
        unloadLibrary(pluginPath, handle);
        lastError = "Plugin does not export createPlugin function";
        return nullptr;
    }
    
    // Create the plugin instance
    IPlugin* plugin = createFunc();
    if (!plugin) {
        unloadLibrary(pluginPath, handle);
        lastError = "Failed to create plugin instance";
        return nullptr;
    }
    
    // Store the handle
    loadedHandles[pluginPath] = handle;
    
    // Create shared pointer with custom deleter
    auto pluginPtr = std::shared_ptr<IPlugin>(plugin, [this, pluginPath](IPlugin* p) {
        // Find and unload the library when the plugin is destroyed
        auto it = loadedHandles.find(pluginPath);
        if (it != loadedHandles.end()) {
            unloadLibrary(pluginPath, it->second);
            loadedHandles.erase(it);
        }
        delete p;
    });
    
    return pluginPtr;
}

std::shared_ptr<IPlugin> DynamicPluginLoader::loadPlugin(const PluginManifest& manifest) {
    if (manifest.pluginFilePath.empty()) {
        lastError = "Plugin manifest missing plugin file path";
        return nullptr;
    }
    
    return loadPlugin(manifest.pluginFilePath);
}

bool DynamicPluginLoader::canLoadPlugin(const std::string& pluginPath) const {
    if (pluginPath.empty()) {
        return false;
    }
    
    std::filesystem::path path(pluginPath);
    
    // Check if file exists
    if (!std::filesystem::exists(path)) {
        lastError = "Plugin file does not exist: " + pluginPath;
        return false;
    }
    
    // Check file extension
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
#ifdef _WIN32
    if (extension != ".dll") {
        lastError = "Plugin file must have .dll extension on Windows";
        return false;
    }
#else
    if (extension != ".so" && extension != ".dylib") {
        lastError = "Plugin file must have .so or .dylib extension on Unix";
        return false;
    }
#endif
    
    return true;
}

std::vector<std::string> DynamicPluginLoader::getAvailablePlugins(const std::string& directory) const {
    std::vector<std::string> plugins;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && canLoadPlugin(entry.path().string())) {
                plugins.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        lastError = "Failed to scan directory " + directory + ": " + std::string(e.what());
    }
    
    return plugins;
}

void DynamicPluginLoader::unloadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = loadedHandles.find(pluginPath);
    if (it != loadedHandles.end()) {
        unloadLibrary(pluginPath, it->second);
        loadedHandles.erase(it);
    }
}

bool DynamicPluginLoader::isPluginLoaded(const std::string& pluginPath) const {
    std::lock_guard<std::mutex> lock(mutex);
    return loadedHandles.find(pluginPath) != loadedHandles.end();
}

std::vector<std::string> DynamicPluginLoader::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> plugins;
    plugins.reserve(loadedHandles.size());
    for (const auto& pair : loadedHandles) {
        plugins.push_back(pair.first);
    }
    return plugins;
}

PluginRegistry::PluginRegistry(std::shared_ptr<IPluginLoader> loader) : loader(loader) {
    if (!loader) {
        this->loader = std::make_shared<DynamicPluginLoader>();
    }
}

bool PluginRegistry::registerPlugin(const std::string& name, std::shared_ptr<IPlugin> plugin) {
    if (!plugin || name.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex);
    plugins[name] = plugin;
    return true;
}

std::shared_ptr<IPlugin> PluginRegistry::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = plugins.find(name);
    return it != plugins.end() ? it->second : nullptr;
}

bool PluginRegistry::unregisterPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = plugins.find(name);
    if (it == plugins.end()) {
        return false;
    }
    plugins.erase(it);
    return true;
}

bool PluginRegistry::isPluginRegistered(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    return plugins.find(name) != plugins.end();
}

bool PluginRegistry::registerManifest(const PluginManifest& manifest) {
    std::lock_guard<std::mutex> lock(mutex);
    pluginManifests[manifest.name] = manifest;
    return true;
}

PluginManifest PluginRegistry::getManifest(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = pluginManifests.find(name);
    return it != pluginManifests.end() ? it->second : PluginManifest{};
}

std::vector<PluginManifest> PluginRegistry::getAllManifests() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<PluginManifest> manifests;
    manifests.reserve(pluginManifests.size());
    for (const auto& pair : pluginManifests) {
        manifests.push_back(pair.second);
    }
    return manifests;
}

std::vector<std::string> PluginRegistry::discoverPlugins(const std::string& directory) {
    if (!loader) {
        lastError = "Plugin loader not available";
        return {};
    }
    
    return loader->getAvailablePlugins(directory);
}

bool PluginRegistry::loadPluginFromFile(const std::string& pluginPath) {
    if (!loader) {
        lastError = "Plugin loader not available";
        return false;
    }
    
    auto plugin = loader->loadPlugin(pluginPath);
    if (!plugin) {
        return false;
    }
    
    // Try to extract name from plugin
    std::string pluginName = plugin->getName();
    if (pluginName.empty()) {
        pluginName = "plugin_" + std::to_string(std::hash<std::string>{}(pluginPath));
    }
    
    return registerPlugin(pluginName, plugin);
}

bool PluginRegistry::loadPluginFromManifest(const PluginManifest& manifest) {
    if (!loader) {
        lastError = "Plugin loader not available";
        return false;
    }
    
    auto plugin = loader->loadPlugin(manifest);
    if (!plugin) {
        return false;
    }
    
    return registerPlugin(manifest.name, plugin) && registerManifest(manifest);
}

std::vector<std::string> PluginRegistry::getLoadedPluginNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> names;
    names.reserve(plugins.size());
    for (const auto& pair : plugins) {
        names.push_back(pair.first);
    }
    return names;
}

PluginCategory PluginRegistry::getPluginCategory(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = plugins.find(name);
    if (it == plugins.end()) return PluginCategory::UNKNOWN;
    
    auto manifestIt = pluginManifests.find(name);
    if (manifestIt != pluginManifests.end()) {
        return manifestIt->second.category;
    }
    
    return PluginCategory::UNKNOWN;
}

std::vector<std::string> PluginRegistry::getPluginDependencies(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto manifestIt = pluginManifests.find(name);
    if (manifestIt != pluginManifests.end()) {
        return manifestIt->second.dependencies;
    }
    return {};
}

std::string PluginRegistry::getLastError() const {
    if (loader) {
        return loader->getLastError();
    }
    return "Plugin registry error";
}

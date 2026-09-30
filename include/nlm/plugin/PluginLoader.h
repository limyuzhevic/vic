#ifndef NLM_PLUGIN_PLUGINLOADER_H
#define NLM_PLUGIN_PLUGINLOADER_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <mutex>

#ifdef _WIN32
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
#else
    #define DLL_EXPORT __attribute__((visibility("default")))
    #define DLL_IMPORT __attribute__((visibility("default")))
#endif

namespace nlm {
namespace plugin {

class IPlugin;

class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();

    // Dynamic loading
    bool loadPlugin(const std::string& pluginPath, std::string& pluginId);
    bool unloadPlugin(const std::string& pluginId);
    bool isPluginLoaded(const std::string& pluginId) const;

    // Symbol resolution
    template<typename T>
    bool getPluginSymbol(const std::string& pluginId, const std::string& symbolName, T& outSymbol);

    // Platform-specific operations
    static std::vector<std::string> getSupportedPluginExtensions();
    static std::vector<std::string> findPluginsInDirectory(const std::string& directory);
    static std::string getCurrentPlatformName();

    // Plugin entry point
    typedef IPlugin* (*CreatePluginFunction)();
    typedef void (*DestroyPluginFunction)(IPlugin*);

protected:
    void* loadLibrary(const std::string& pluginPath);
    void unloadLibrary(void* handle);
    bool resolveSymbols(void* handle, const std::string& pluginId);

    std::map<std::string, void*> loadedLibraries_;
    std::map<std::string, CreatePluginFunction> createFunctions_;
    std::map<std::string, DestroyPluginFunction> destroyFunctions_;
    mutable std::mutex mutex_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINLOADER_H

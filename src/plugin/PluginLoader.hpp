// PluginLoader.hpp
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <filesystem>

namespace nlm {

class PluginLoader {
public:
    // Dynamic loading from file
    static std::unique_ptr<PluginBase> loadPlugin(const std::string& filepath);

    // Loading from plugin directories
    static bool loadAllPluginsFromDirectory(const std::string& directory);
    static std::vector<std::string> getPluginFiles(const std::string& directory);

    // Plugin manifest and metadata
    struct PluginManifest {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string pluginInterface;
        std::string dependencies;
        std::string entryPoint;
    };

    // Plugin validation
    static bool validatePluginFile(const std::string& filepath);
    static PluginManifest loadPluginManifest(const std::string& filepath);

    // Error handling
    static std::string getLastError();

private:
    static std::string lastError_;
    static std::mutex mutex_;
};

} // namespace nlm
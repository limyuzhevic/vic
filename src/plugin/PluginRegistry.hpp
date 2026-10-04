// PluginRegistry.hpp
#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <typeindex>

namespace nlm {

class PluginRegistry {
public:
    // Plugin registration
    template<typename T>
    bool registerPlugin(const std::string& name,
                       const std::string& version,
                       const std::string& description,
                       const std::string& author,
                       const std::function<std::unique_ptr<T>()>& factory,
                       const std::vector<std::string>& dependencies = {});

    // Plugin discovery
    std::vector<std::string> getPluginNames() const;
    bool hasPlugin(const std::string& name) const;
    std::string getPluginVersion(const std::string& name) const;
    std::string getPluginDescription(const std::string& name) const;
    std::string getPluginAuthor(const std::string& name) const;
    std::vector<std::string> getPluginDependencies(const std::string& name) const;

    // Plugin type information
    std::type_index getPluginTypeIndex(const std::string& name) const;
    std::type_index getPluginBaseTypeIndex() const;

    // Plugin lifecycle
    bool initializeAllPlugins();
    bool shutdownAllPlugins();

    // Plugin validation
    bool validatePluginConfiguration(const std::string& name, const class Config& config) const;

    // Statistics
    size_t getPluginCount() const;
    size_t getLoadedPluginCount() const;

    // Error handling
    std::string getLastError() const;

private:
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::type_index typeIndex;
        std::vector<std::string> dependencies;
        std::function<std::unique_ptr<PluginBase>()> factory;
        bool loaded;
        std::string error;
    };

    std::unordered_map<std::string, PluginInfo> plugins_;
    mutable std::mutex mutex_;
    std::string lastError_;
};

} // namespace nlm
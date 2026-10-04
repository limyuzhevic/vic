// PluginManager.hpp
#pragma once

#include "PluginBase.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <typeindex>
#include "../core/Config/Config.hpp"
#include <chrono>

namespace nlm {

class PluginManager {
public:
    // Plugin registration and loading
    template<typename T>
    bool registerPlugin(const std::string& name,
                       const std::string& version,
                       const std::string& description,
                       const std::string& author,
                       const std::function<std::unique_ptr<T>()>& factory);

    // Plugin management
    bool initializePlugin(const std::string& name);
    bool shutdownPlugin(const std::string& name);
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);

    // Plugin discovery
    std::vector<std::string> getPluginNames() const;
    bool hasPlugin(const std::string& name) const;
    std::string getPluginVersion(const std::string& name) const;
    std::string getPluginDescription(const std::string& name) const;

    // Plugin access
    template<typename T>
    std::shared_ptr<T> getPlugin(const std::string& name);

    // Configuration management
    template<typename T>
    bool setPluginConfiguration(const std::string& name, const Config& config);

    // Validation
    template<typename T>
    bool validatePlugin(const std::string& name) const;

    // Plugin lifecycle
    bool initializeAll();
    bool shutdownAll();
    bool enableAll();
    bool disableAll();

    // Statistics
    size_t getInitializedPluginCount() const;
    size_t getEnabledPluginCount() const;
    size_t getTotalPluginCount() const;

    // Error handling
    std::string getLastError() const;

private:
    struct PluginEntry {
        std::unique_ptr<PluginBase> plugin;
        bool initialized;
        bool enabled;
        Config configuration;
        std::string error;
        std::chrono::steady_clock::time_point loadTime;
    };

    std::unordered_map<std::string, PluginEntry> plugins_;
    mutable std::mutex mutex_;
    std::string lastError_;
};

} // namespace nlm
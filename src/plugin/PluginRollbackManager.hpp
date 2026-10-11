#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include "PluginBase.hpp"

namespace nlm {

struct ResourceUsage {
    double cpuUsage;
    double memoryUsage;
    size_t threadCount;
    double networkUsage;
    size_t fileHandles;
    
    ResourceUsage() : cpuUsage(0.0), memoryUsage(0.0), threadCount(0),
                     networkUsage(0.0), fileHandles(0) {}
};

struct PluginChangeRecord {
    double timestamp;
    std::string action;  // "LOAD", "UNLOAD", "UPDATE", "ERROR"
    std::string version;
    std::string details;
    bool successful;
};

class PluginRollbackManager {
public:
    PluginRollbackManager();
    ~PluginRollbackManager();
    
    // Version management
    bool savePluginVersion(const std::string& pluginName);
    bool restorePluginVersion(const std::string& pluginName, const std::string& version);
    std::vector<std::string> getAvailableVersions(const std::string& pluginName) const;
    
    // State management
    bool savePluginState(const std::string& pluginName);
    bool restorePluginState(const std::string& pluginName);
    bool rollbackLastChange(const std::string& pluginName);
    
    // History tracking
    std::vector<PluginChangeRecord> getPluginHistory(const std::string& pluginName) const;
    PluginChangeRecord getLastChange(const std::string& pluginName) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <functional>
#include "../core/Config/Config.hpp"
#include "AdvancedConfig.hpp"

namespace nlm {

class ConfigInheritance {
public:
    ConfigInheritance();
    ~ConfigInheritance();
    
    // Disable copying, enable moving
    ConfigInheritance(const ConfigInheritance&) = delete;
    ConfigInheritance& operator=(const ConfigInheritance&) = delete;
    ConfigInheritance(ConfigInheritance&&) noexcept;
    ConfigInheritance& operator=(ConfigInheritance&&) noexcept;
    
    // Inheritance lifecycle
    bool initialize();
    void update(double timestep);
    void shutdown();
    
    // Inheritance management
    bool addInheritancePath(const std::string& path, const std::string& priority = "medium");
    bool removeInheritancePath(const std::string& path);
    bool updateInheritancePath(const std::string& path, const std::string& newPriority);
    std::vector<std::string> getInheritancePaths() const;
    std::map<std::string, std::string> getPathPriorities() const;
    
    // Inheritance resolution
    Config resolveInheritance(const std::vector<std::string>& paths = {}) const;
    bool validateInheritanceChain(const std::vector<std::string>& paths) const;
    std::vector<std::string> getResolvedInheritanceOrder(const std::vector<std::string>& paths) const;
    
    // Inheritance merging
    Config mergeConfigurations(const std::map<std::string, Config>& configs) const;
    Config mergeConfigurations(const std::vector<Config>& configs) const;
    std::map<std::string, ConfigValue> mergeValues(const std::map<std::string, ConfigValue>& baseValues,
                                                  const std::map<std::string, ConfigValue>& overrideValues) const;
    
    // Inheritance conflict resolution
    bool detectConflicts(const std::map<std::string, Config>& configs) const;
    std::vector<std::string> getConflicts(const std::map<std::string, Config>& configs) const;
    std::string resolveConflict(const std::string& key, const std::map<std::string, ConfigValue>& conflictingValues) const;
    
    // Inheritance monitoring
    bool enableInheritanceMonitoring(bool enable);
    bool isInheritanceMonitoringEnabled() const;
    std::string getInheritanceStatus() const;
    std::chrono::system_clock::time_point getLastInheritanceUpdate() const;
    
    // Inheritance validation
    bool validateInheritancePaths(const std::vector<std::string>& paths) const;
    bool validateConfiguration(const Config& config) const;
    std::vector<std::string> getInheritanceWarnings() const;
    
    // Inheritance analytics
    size_t getInheritanceChainDepth() const;
    std::map<std::string, size_t> getConfigurationUsageCounts() const;
    std::map<std::string, double> getConfigurationChangeRates() const;
    std::string getInheritanceAnalytics() const;
    
    // File-based inheritance
    bool loadInheritanceFromYAML(const std::string& filepath);
    bool loadInheritanceFromJSON(const std::string& filepath);
    bool saveInheritanceToYAML(const std::string& filepath) const;
    bool saveInheritanceToJSON(const std::string& filepath) const;
    std::string exportInheritanceToYAML() const;
    std::string exportInheritanceToJSON() const;
    
    // Dynamic inheritance
    void addDynamicInheritanceRule(const std::string& ruleName,
                                  std::function<bool(const std::string&)> rule);
    void removeDynamicInheritanceRule(const std::string& ruleName);
    bool evaluateDynamicRules(const std::string& path) const;
    
    // Inheritance events
    using InheritanceCallback = std::function<void(const std::string& path, const Config& config, bool added)>;
    void registerInheritanceCallback(const std::string& eventType, InheritanceCallback callback);
    void triggerInheritanceEvent(const std::string& eventType, const std::string& path, const Config& config, bool added);
    
    // Inheritance statistics
    struct InheritanceStats {
        size_t totalPaths;
        size_t activePaths;
        size_t conflictsDetected;
        size_t mergesPerformed;
        std::chrono::system_clock::time_point lastUpdate;
        std::map<std::string, size_t> pathUsageCounts;
        double averageConflictResolutionTime;
        
        InheritanceStats()
            : totalPaths(0), activePaths(0), conflictsDetected(0), mergesPerformed(0),
              lastUpdate(std::chrono::system_clock::now()), averageConflictResolutionTime(0.0) {}
    };
    
    InheritanceStats getInheritanceStats() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Inheritance event data structure
struct InheritanceEvent {
    std::string eventType;
    std::string path;
    Config config;
    bool added;
    std::chrono::system_clock::time_point timestamp;
    std::string eventId;
    
    InheritanceEvent() : added(false) {}
    InheritanceEvent(const std::string& type, const std::string& p, const Config& c, bool add)
        : eventType(type), path(p), config(c), added(add), timestamp(std::chrono::system_clock::now()), eventId("event_" + std::to_string(timestamp.time_since_epoch().count())) {}
};

} // namespace nlm

#ifndef NLM_PLUGIN_PERFORMANCEPLUGINSYSTEM_HPP
#define NLM_PLUGIN_PERFORMANCEPLUGINSYSTEM_HPP

#include "PluginBase.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <functional>

namespace nlm {
namespace plugin {

struct PerformanceMetric {
    std::string name;
    std::string category;
    double value;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    bool isMonotonic;
};

struct PerformanceProfile {
    std::string name;
    std::vector<PerformanceMetric> metrics;
    std::chrono::duration<double> totalTime;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    bool isActive;
};

class PerformancePluginSystem {
public:
    PerformancePluginSystem();
    ~PerformancePluginSystem();
    
    // Plugin registration
    bool registerPerformancePlugin(std::unique_ptr<IPlugin> plugin);
    bool unregisterPerformancePlugin(const std::string& name);
    
    // Plugin access
    IPlugin* getPerformancePlugin(const std::string& name) const;
    
    // Performance monitoring
    bool startMonitoring(const std::string& metricName, const std::string& category, bool isMonotonic = false);
    bool stopMonitoring(const std::string& metricName);
    bool updateMetric(const std::string& metricName, double value);
    PerformanceMetric getMetric(const std::string& metricName) const;
    
    // Performance profiling
    PerformanceProfile startProfile(const std::string& profileName);
    bool stopProfile(const std::string& profileName);
    PerformanceProfile getProfile(const std::string& profileName) const;
    std::vector<PerformanceProfile> getAllProfiles() const;
    
    // Performance analysis
    std::unordered_map<std::string, double> analyzePerformance(const std::string& metricCategory) const;
    std::vector<std::string> getPerformanceBottlenecks() const;
    bool optimizePerformance(const std::string& optimizationStrategy);
    
    // Performance configuration
    bool configurePerformance(const std::string& pluginName, const std::unordered_map<std::string, std::string>& config);
    std::unordered_map<std::string, std::string> getPerformanceConfig(const std::string& pluginName) const;
    
    // Performance lifecycle
    bool initializePerformance(const std::string& pluginName);
    bool activatePerformance(const std::string& pluginName);
    bool shutdownPerformance(const std::string& pluginName);
    
    // Performance monitoring lifecycle
    bool startSystemMonitoring();
    bool stopSystemMonitoring();
    bool isSystemMonitoringActive() const;
    
    // Performance statistics
    struct PerformanceStats {
        std::string name;
        PluginState state;
        uint64_t metricUpdateCount = 0;
        uint64_t profileRunCount = 0;
        uint64_t optimizationCount = 0;
        std::string lastError;
        std::chrono::duration<double> averageMetricUpdateTime;
        std::chrono::duration<double> averageProfileTime;
        std::chrono::duration<double> averageOptimizationTime;
    };
    
    std::vector<PerformanceStats> getAllPerformanceStats() const;
    PerformanceStats getPerformanceStats(const std::string& pluginName) const;
    
    // Performance composition
    bool composePerformancePlugins(const std::vector<std::string>& pluginNames, const std::string& composedPluginName);
    bool decomposePerformancePlugin(const std::string& composedPluginName, std::vector<std::string>& componentPluginNames);
    
    // Performance validation
    bool validatePerformanceConfiguration(const std::string& pluginName) const;
    std::vector<std::string> getPerformanceValidationErrors(const std::string& pluginName) const;
    
    // Performance monitoring integration
    bool integrateWithExternalMonitor(const std::string& monitorName, const std::unordered_map<std::string, std::string>& config);
    bool deintegrateFromExternalMonitor(const std::string& monitorName);
    bool isIntegratedWithExternalMonitor(const std::string& monitorName) const;
    
    // Performance security
    bool setPerformanceSecurityPolicy(const std::string& pluginName, const std::string& policy);
    std::string getPerformanceSecurityPolicy(const std::string& pluginName) const;
    bool enablePerformanceEncryption(const std::string& pluginName, bool enabled);
    bool isPerformanceEncrypted(const std::string& pluginName) const;
    
    // Performance debugging
    std::string getPerformanceDebugInfo(const std::string& pluginName) const;
    void dumpPerformanceState(const std::string& filename) const;
    
    // Performance alerts
    using PerformanceAlertCallback = std::function<void(const std::string& metricName, double value, const std::string& alertType)>;
    void addAlertCallback(const PerformanceAlertCallback& callback);
    void removeAlertCallback(const PerformanceAlertCallback& callback);
    void checkPerformanceAlerts();
    
    // Performance export
    bool exportPerformanceData(const std::string& filename, const std::vector<std::string>& metricNames) const;
    bool importPerformanceData(const std::string& filename);
    
private:
    struct PerformanceEntry {
        std::unique_ptr<IPlugin> plugin;
        PluginMetadata metadata;
        std::unordered_map<std::string, std::string> config;
        PluginState state;
        std::string error;
        std::chrono::system_clock::time_point creationTime;
        std::unordered_map<std::string, PerformanceMetric> metrics;
        std::vector<PerformanceProfile> profiles;
        std::string securityPolicy;
        bool encrypted;
        PerformanceStats stats;
    };
    
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, PerformanceEntry> performancePlugins_;
    bool systemMonitoringActive_;
    
    std::vector<PerformanceAlertCallback> alertCallbacks_;
    
    bool validatePerformanceEntry(const PerformanceEntry& entry) const;
    bool updateMetricInternal(const std::string& pluginName, const std::string& metricName, double value);
    bool checkPerformanceAlertsInternal();
    std::vector<std::string> detectPerformanceBottlenecks() const;
    double calculatePerformanceScore(const PerformanceEntry& entry) const;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PERFORMANCEPLUGINSYSTEM_HPP
#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include "PluginBase.hpp"

namespace nlm {

enum class PluginHealth {
    HEALTHY,
    WARNING,
    DEGRADED,
    FAILED,
    UNKNOWN
};

struct DiagnosticEntry {
    std::string name;
    std::string description;
    PluginHealth status;
    std::string details;
    double timestamp;
    bool required;
    
    DiagnosticEntry() : status(PluginHealth::UNKNOWN), timestamp(0.0), required(false) {}
};

class PluginMonitor {
public:
    PluginMonitor();
    ~PluginMonitor();
    
    // Performance monitoring
    void startPerformanceMonitoring(const std::string& pluginName);
    void stopPerformanceMonitoring(const std::string& pluginName);
    double getPluginCPUUsage(const std::string& pluginName) const;
    double getPluginMemoryUsage(const std::string& pluginName) const;
    size_t getPluginThreadCount(const std::string& pluginName) const;
    double getPluginNetworkUsage(const std::string& pluginName) const;
    size_t getPluginFileHandles(const std::string& pluginName) const;
    
    // Health monitoring
    PluginHealth getPluginHealth(const std::string& pluginName) const;
    std::string getPluginErrorMessage(const std::string& pluginName) const;
    std::vector<std::string> getPluginWarnings(const std::string& pluginName) const;
    
    // Resource monitoring
    bool checkResourceLimits(const std::string& pluginName) const;
    void getResourceUsage(const std::string& pluginName, ResourceUsage& usage) const;
    
    // Diagnostic
    std::string getDiagnosticReport(const std::string& pluginName) const;
    std::vector<DiagnosticEntry> runDiagnostics(const std::string& pluginName) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

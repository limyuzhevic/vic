#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentIntegration.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// External system types
enum class ExternalSystemType {
    Database,
    MessageQueue,
    Cache,
    Storage,
    API,
    WebService,
    FileSystem,
    CloudService,
    Analytics,
    Monitoring,
    Security,
    Authentication,
    Custom
};

// External system connection result
struct ExternalSystemConnectionResult {
    bool success;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> systemInfo;
    
    ExternalSystemConnectionResult() : success(false), timestamp(std::chrono::system_clock::now()) {}
    ExternalSystemConnectionResult(bool success, const std::string& message)
        : success(success), message(message), timestamp(std::chrono::system_clock::now()) {}
};

class ExperimentExternalSystems {
public:
    ExperimentExternalSystems();
    ~ExperimentExternalSystems();
    
    // Disable copying, enable moving
    ExperimentExternalSystems(const ExperimentExternalSystems&) = delete;
    ExperimentExternalSystems& operator=(const ExperimentExternalSystems&) = delete;
    ExperimentExternalSystems(ExperimentExternalSystems&&) noexcept;
    ExperimentExternalSystems& operator=(ExperimentExternalSystems&&) noexcept;
    
    // System management
    bool initializeExternalSystems(const std::string& configPath = "");
    bool shutdownExternalSystems();
    bool isExternalSystemsInitialized() const;
    
    // System discovery
    std::vector<ExternalSystemType> discoverExternalSystems() const;
    bool registerExternalSystem(const std::string& name, ExternalSystemType type,
                              const std::map<std::string, std::string>& capabilities);
    bool unregisterExternalSystem(const std::string& name);
    ExternalSystemType getExternalSystemType(const std::string& name) const;
    std::map<std::string, ExternalSystemType> getAllExternalSystems() const;
    
    // Connection management
    ExternalSystemConnectionResult connectToSystem(const std::string& name,
                                                 const std::map<std::string, std::string>& parameters);
    bool disconnectFromSystem(const std::string& name);
    bool reconnectToSystem(const std::string& name);
    bool isSystemConnected(const std::string& name) const;
    
    // System capabilities
    std::map<std::string, std::string> getSystemCapabilities(const std::string& name) const;
    bool setSystemCapabilities(const std::string& name, 
                             const std::map<std::string, std::string>& capabilities);
    
    // Data exchange
    std::map<std::string, std::string> querySystem(const std::string& name,
                                                 const std::map<std::string, std::string>& query);
    bool updateSystem(const std::string& name, const std::map<std::string, std::string>& data);
    bool insertData(const std::string& name, const std::string& table,
                   const std::map<std::string, std::string>& data);
    bool deleteData(const std::string& name, const std::string& table,
                   const std::map<std::string, std::string>& conditions);
    
    // Analytics and monitoring
    std::map<std::string, double> getSystemMetrics(const std::string& name) const;
    std::vector<std::map<std::string, std::string>> getSystemLogs(const std::string& name,
                                                              const std::chrono::system_clock::time_point& fromTime,
                                                              const std::chrono::system_clock::time_point& toTime) const;
    
    // Backup and recovery
    bool backupSystemData(const std::string& name, const std::string& backupPath) const;
    bool restoreSystemData(const std::string& name, const std::string& backupPath);
    
    // System configuration
    bool configureSystem(const std::string& name, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getSystemConfig(const std::string& name) const;
    
    // System maintenance
    bool restartSystem(const std::string& name);
    bool updateSystemSoftware(const std::string& name, const std::string& version);
    bool checkSystemHealth(const std::string& name);
    
    // System statistics
    std::map<std::string, double> getOverallSystemStatistics() const;
    std::vector<std::string> getHealthySystems() const;
    std::vector<std::string> getUnhealthySystems() const;
    
    // Cleanup and maintenance
    bool cleanupSystemLogs(const std::string& name, const std::chrono::system_clock::time_point& cutoffTime);
    bool resetSystem(const std::string& name);
    bool clearAllSystemData();
    
    // System management
    bool enableSystemMonitoring(const std::string& name, bool enable);
    bool isSystemMonitoringEnabled(const std::string& name) const;
    bool enableRealTimeSystemUpdates(bool enable);
    bool isRealTimeSystemUpdatesEnabled() const;
    
    // Utilities
    static std::string generateSystemName(const std::string& prefix = "system") const;
    static bool isValidSystemName(const std::string& name) const;
    static ExternalSystemType stringToSystemType(const std::string& typeStr) const;
    static std::string systemTypeToString(ExternalSystemType type) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
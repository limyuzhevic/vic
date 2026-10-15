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
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Integration status
enum class IntegrationStatus {
    NotIntegrated,
    Connecting,
    Connected,
    Disconnected,
    Failed,
    Unknown
};

// External system interface
class ExternalSystem {
public:
    virtual ~ExternalSystem() = default;
    
    virtual std::string getSystemName() const = 0;
    virtual std::string getSystemVersion() const = 0;
    virtual IntegrationStatus getIntegrationStatus() const = 0;
    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual std::map<std::string, std::string> getSystemCapabilities() const = 0;
};

// Integration configuration
struct IntegrationConfig {
    std::string integrationName;
    std::string systemName;
    std::string systemVersion;
    std::string connectionString;
    std::map<std::string, std::string> connectionParameters;
    bool autoReconnect;
    std::chrono::seconds reconnectionInterval;
    std::chrono::seconds timeout;
    bool enableCompression;
    bool enableEncryption;
    IntegrationStatus status;
    std::chrono::system_clock::time_point lastConnected;
    std::chrono::system_clock::time_point lastDisconnected;
    
    IntegrationConfig() : autoReconnect(true), reconnectionInterval(std::chrono::seconds(30)),
                        timeout(std::chrono::seconds(60)), enableCompression(false),
                        enableEncryption(false), status(IntegrationStatus::NotIntegrated),
                        lastConnected(std::chrono::system_clock::now()),
                        lastDisconnected(std::chrono::system_clock::now()) {}
};

// Integration event
struct IntegrationEvent {
    std::string eventId;
    std::string integrationName;
    std::string eventType; // "connection", "disconnection", "error", "data_transfer"
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::map<std::string, std::string> eventData;
    bool isSuccess;
    
    IntegrationEvent() : timestamp(std::chrono::system_clock::now()), isSuccess(false) {}
    IntegrationEvent(const std::string& eventId, const std::string& integrationName,
                    const std::string& eventType, const std::string& message,
                    bool success)
        : eventId(eventId), integrationName(integrationName), eventType(eventType),
          timestamp(std::chrono::system_clock::now()), message(message), isSuccess(success) {}
};

// Integration callback
using IntegrationCallback = std::function<void(const IntegrationEvent&)>
;
class ExperimentIntegration {
public:
    ExperimentIntegration();
    ~ExperimentIntegration();
    
    // Disable copying, enable moving
    ExperimentIntegration(const ExperimentIntegration&) = delete;
    ExperimentIntegration& operator=(const ExperimentIntegration&) = delete;
    ExperimentIntegration(ExperimentIntegration&&) noexcept;
    ExperimentIntegration& operator=(ExperimentIntegration&&) noexcept;
    
    // Integration management
    bool initializeIntegration(const std::string& integrationName, 
                             const IntegrationConfig& config);
    bool shutdownIntegration(const std::string& integrationName);
    bool isIntegrationInitialized(const std::string& integrationName) const;
    
    // Connection management
    bool connectToExternalSystem(const std::string& integrationName);
    bool disconnectFromExternalSystem(const std::string& integrationName);
    bool reconnectToExternalSystem(const std::string& integrationName);
    IntegrationStatus getIntegrationStatus(const std::string& integrationName) const;
    
    // System management
    bool addExternalSystem(const std::string& integrationName, 
                         std::unique_ptr<ExternalSystem> system);
    bool removeExternalSystem(const std::string& integrationName);
    ExternalSystem* getExternalSystem(const std::string& integrationName) const;
    std::map<std::string, std::unique_ptr<ExternalSystem>> getAllExternalSystems(const std::string& integrationName) const;
    
    // Integration configuration
    bool updateIntegrationConfig(const std::string& integrationName, 
                               const IntegrationConfig& config);
    IntegrationConfig getIntegrationConfig(const std::string& integrationName) const;
    std::vector<std::string> getAllIntegrationNames() const;
    
    // Event management
    bool addIntegrationCallback(const std::string& integrationName, IntegrationCallback callback);
    bool removeIntegrationCallback(const std::string& integrationName, IntegrationCallback callback);
    bool clearIntegrationCallbacks(const std::string& integrationName);
    
    bool logIntegrationEvent(const IntegrationEvent& event);
    std::vector<IntegrationEvent> getIntegrationEvents(const std::string& integrationName) const;
    std::vector<IntegrationEvent> getIntegrationEventsByType(const std::string& integrationName,
                                                         const std::string& eventType) const;
    
    // Data exchange
    bool sendDataToExternalSystem(const std::string& integrationName, 
                               const std::map<std::string, std::string>& data);
    std::map<std::string, std::string> receiveDataFromExternalSystem(const std::string& integrationName);
    std::vector<std::map<std::string, std::string>> exchangeData(const std::string& integrationName,
                                                             const std::vector<std::map<std::string, std::string>>& data);
    
    // Integration analytics
    double calculateIntegrationEfficiency(const std::string& integrationName) const;
    std::string analyzeIntegrationPerformance(const std::string& integrationName) const;
    std::chrono::system_clock::time_point getLastSuccessfulConnection(const std::string& integrationName) const;
    std::chrono::system_clock::time_point getLastFailedConnection(const std::string& integrationName) const;
    
    // Integration backup and recovery
    bool backupIntegrationData(const std::string& integrationName, const std::string& backupPath) const;
    bool restoreIntegrationData(const std::string& integrationName, const std::string& backupPath);
    bool exportIntegrationToJSON(const std::string& integrationName, const std::string& filepath) const;
    bool importIntegrationFromJSON(const std::string& integrationName, const std::string& filepath);
    
    // Integration cleanup
    bool cleanupIntegrationEvents(const std::string& integrationName, 
                                const std::chrono::system_clock::time_point& cutoffTime);
    bool resetIntegration(const std::string& integrationName);
    bool clearAllIntegrationData();
    
    // System management
    bool enableIntegrationMonitoring(const std::string& integrationName, bool enable);
    bool isIntegrationMonitoringEnabled(const std::string& integrationName) const;
    bool enableRealTimeIntegrationUpdates(bool enable);
    bool isRealTimeIntegrationUpdatesEnabled() const;
    
    // Utilities
    static std::string generateIntegrationName(const std::string& prefix = "integration") const;
    static bool isValidIntegrationName(const std::string& name) const;
    static IntegrationStatus stringToIntegrationStatus(const std::string& statusStr) const;
    static std::string integrationStatusToString(IntegrationStatus status) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
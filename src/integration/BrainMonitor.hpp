#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/MonitoringManager.hpp"
#include "../performance/VisualizationEngine.hpp"
#include "../performance/ExportManager.hpp"
#include <memory>
#include <mutex>

namespace nlm {

// Integration configuration
struct IntegrationConfig {
    bool enableBrainMonitoring = true;
    bool enableVisualization = true;
    bool enableExport = true;
    bool enableWebInterface = false;
    std::chrono::milliseconds updateInterval = std::chrono::milliseconds(100);
    std::string exportDirectory = "./exports";
    bool enableNotifications = true;
    std::string notificationCallback;
};

// Brain integration state
struct BrainIntegrationState {
    std::chrono::high_resolution_clock::time_point lastUpdate;
    size_t lastStep = 0;
    double lastTime = 0.0;
    bool isInitialized = false;
    std::string lastStatus;
};

// Brain monitor for integration with monitoring system
class BrainMonitor {
public:
    BrainMonitor();
    ~BrainMonitor();
    
    // Disable copying
    BrainMonitor(const BrainMonitor&) = delete;
    BrainMonitor& operator=(const BrainMonitor&) = delete;
    
    // Initialize brain monitor
    bool initialize(const IntegrationConfig& config = IntegrationConfig());
    
    // Start/stop monitoring
    void start();
    void stop();
    bool isRunning() const { return running_.load(); }
    
    // Configure monitoring
    void configure(const IntegrationConfig& config);
    IntegrationConfig getConfig() const { return config_; }
    
    // Monitor a brain
    void monitorBrain(const std::shared_ptr<Brain>& brain);
    void monitorBrain(std::shared_ptr<Brain>&& brain);
    
    // Get monitoring data
    std::string getBrainStatus() const;
    std::string getIntegrationStatus() const;
    std::string getPerformanceStats() const;
    
    // Export functionality
    bool exportBrainData(const std::string& filename, const std::string& format = "csv");
    bool exportAllData(const std::string& prefix = "brain_export");
    
    // Configuration getters
    bool isBrainMonitoringEnabled() const { return config_.enableBrainMonitoring; }
    bool isVisualizationEnabled() const { return config_.enableVisualization; }
    bool isExportEnabled() const { return config_.enableExport; }
    bool isWebInterfaceEnabled() const { return config_.enableWebInterface; }
    
    // Get singleton instance
    static BrainMonitor& getInstance() {
        static BrainMonitor instance;
        return instance;
    }
    
    // Global brain monitoring functions
    static void monitorGlobalBrain(const std::shared_ptr<Brain>& brain);
    static void startGlobalMonitoring();
    static void stopGlobalMonitoring();
    static bool isGlobalMonitoringRunning();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::shared_mutex mutex_;
    std::atomic<bool> running_;
    IntegrationConfig config_;
    std::shared_ptr<Brain> monitoredBrain_;
    BrainIntegrationState integrationState_;
    
    // Internal helper methods
    void updateBrainState();
    void updateMonitoring();
    void updateVisualization();
    void updateExport();
    void updateWebInterface();
    void processBrainUpdates();
    void handleBrainEvents();
    
    // Brain integration helpers
    void initializeBrain(const std::shared_ptr<Brain>& brain);
    void cleanupBrain(const std::shared_ptr<Brain>& brain);
    void updateBrainMetrics(const std::shared_ptr<Brain>& brain);
    void updateBrainVisualization(const std::shared_ptr<Brain>& brain);
    
    // Export helpers
    bool exportBrainMetrics(const std::string& filename, const std::string& format);
    bool exportBrainVisualization(const std::string& filename, const std::string& format);
    bool exportBrainState(const std::string& filename, const std::string& format);
    
    // Configuration helpers
    void setupDefaultConfiguration();
    void setupExportDirectory();
    void setupNotificationCallback();
    
    // Status and monitoring
    std::string generateBrainStatus() const;
    std::string generateIntegrationStatus() const;
    std::string generatePerformanceStats() const;
    
    // Event system
    void fireBrainEvent(const std::string& eventType, const std::string& message);
    
    // File management
    void cleanOldExports(const std::chrono::milliseconds& age) const;
    size_t getExportDirectorySize() const;
    std::vector<std::string> getExportFiles() const;
};

} // namespace nlm

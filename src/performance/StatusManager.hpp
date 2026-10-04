#pragma once

#include "Types.hpp"
#include "Logger/Logger.hpp"
#include "ProgressReporter.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <mutex>
#include <optional>
#include <sstream>
#include <fstream>

namespace nlm {

// Status levels
enum class StatusLevel {
    Offline,
    Initializing,
    Ready,
    Running,
    Paused,
    Saving,
    Loading,
    Error,
    Completed
};

// Status information structure
struct BrainStatus {
    SimulationStep currentStep{0};
    Timestamp currentTime{0.0};
    size_t neuronCount{0};
    size_t synapseCount{0};
    size_t activeNeuronCount{0};
    size_t firingNeuronCount{0};
    double averageFiringRate{0.0};
    size_t totalSpikeCount{0};
    size_t pendingSpikeCount{0};
    double excitationInhibitionRatio{0.0};
    size_t memoryUsage{0};
    double cpuUsage{0.0};
    std::vector<std::string> activeProcesses;
    
    // Add memory usage information
    void setMemoryUsage(size_t bytes) {
        memoryUsage = bytes;
    }
    
    // Calculate firing efficiency
    double getFiringEfficiency() const {
        if (neuronCount == 0) return 0.0;
        return (static_cast<double>(firingNeuronCount) / neuronCount) * 100.0;
    }
};

// System status structure
struct SystemStatus {
    double uptime{0.0};  // seconds
    size_t cpuCores{0};
    double cpuUsage{0.0};
    size_t memoryTotal{0};
    size_t memoryUsed{0};
    size_t memoryAvailable{0};
    std::vector<std::string> runningProcesses;
    double diskUsage{0.0};
    std::chrono::system_clock::time_point lastCheckpoint;
    size_t checkpointCount{0};
    double performanceScore{0.0};  // 0-100 scale
    
    // Calculate memory usage percentage
    double getMemoryUsagePercentage() const {
        if (memoryTotal == 0) return 0.0;
        return (static_cast<double>(memoryUsed) / memoryTotal) * 100.0;
    }
    
    // Calculate performance rating
    double getPerformanceRating() const {
        return std::min(100.0, performanceScore);
    }
};

// Experiment status structure
struct ExperimentStatus {
    std::string name;
    bool isRunning{false};
    bool isPaused{false};
    SimulationStep currentStep{0};
    SimulationStep maxSteps{0};
    size_t totalExperiments{0};
    size_t completedExperiments{0};
    double completionPercentage{0.0};
    double averageFiringRate{0.0};
    double totalReward{0.0};
    Timestamp startTime{0.0};
    Timestamp endTime{0.0};
    Timestamp lastUpdateTime{0.0};
    std::vector<std::string> currentObjectives;
    std::vector<std::string> completedObjectives;
    
    // Update completion percentage
    void updateCompletion(SimulationStep current, SimulationStep max) {
        if (max > 0) {
            completionPercentage = (static_cast<double>(current) / max) * 100.0;
        }
    }
    
    // Calculate average reward per step
    double getAverageRewardPerStep() const {
        if (maxSteps > 0) {
            return totalReward / static_cast<double>(maxSteps);
        }
        return 0.0;
    }
};

// Status configuration structure
struct StatusConfig {
    StatusLevel defaultLevel{StatusLevel::Ready};
    bool enableStatusLogging{true};
    bool enableStatusSnapshots{true};
    std::chrono::milliseconds statusInterval{5000};  // milliseconds
    bool enableRealTimeUpdates{true};
    bool enableStatusExport{true};
    std::string statusExportFile{};
    std::string statusSnapshotFile{};
    
    // Status verbosity levels
    enum class VerbosityLevel {
        Quiet,
        Summary,
        Detailed,
        Debug
    } verbosity{VerbosityLevel::Summary};
    
    // Status notification levels
    enum class NotificationLevel {
        None,
        Low,
        Medium,
        High,
        Critical
    } notificationLevel{NotificationLevel::Medium};
    
    // Status update sources
    struct UpdateSource {
        std::string name;
        StatusLevel level;
        std::string description;
        bool isActive{true};
        std::chrono::system_clock::time_point lastUpdate;
    };
    
    // Status timeline tracking
    bool enableTimelineTracking{true};
    size_t maxTimelineEvents{1000};
    
    // Performance monitoring
    bool enablePerformanceMonitoring{true};
    std::chrono::milliseconds performanceSampleInterval{10000};  // milliseconds
    
    // Export formats
    enum class ExportFormat {
        Text,
        JSON,
        CSV,
        XML,
        YAML
    } exportFormat{ExportFormat::JSON};
    
    // Status snapshot options
    struct SnapshotOptions {
        bool includeMetrics{true};
        bool includeProcesses{true};
        bool includeMemory{true};
        bool includePerformance{true};
        bool includeTimeline{true};
        double compressionRatio{0.8};  // for compressed snapshots
    } snapshotOptions{};
    
    // Status export options
    struct ExportOptions {
        bool includeTimestamps{true};
        bool includeMetrics{true};
        bool includePerformance{true};
        bool includeHistory{true};
        std::string customFields{};
    } exportOptions{};
};

// Status snapshot structure
struct StatusSnapshot {
    std::chrono::system_clock::time_point timestamp;
    StatusLevel level;
    BrainStatus brainStatus;
    SystemStatus systemStatus;
    ExperimentStatus experimentStatus;
    std::string statusText;
    std::vector<std::string> activeAlerts;
    std::vector<std::string> warningMessages;
    
    // Convert to JSON string
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"timestamp\": \"" << std::chrono::system_clock::to_time_t(timestamp) << "\",\n";
        ss << "  \"level\": \"" << static_cast<int>(level) << "\",\n";
        ss << "  \"brainStatus\": {\n";
        ss << "    \"currentStep\": " << brainStatus.currentStep << ",\n";
        ss << "    \"neuronCount\": " << brainStatus.neuronCount << ",\n";
        ss << "    \"synapseCount\": " << brainStatus.synapseCount << ",\n";
        ss << "    \"averageFiringRate\": " << brainStatus.averageFiringRate << ",\n";
        ss << "    \"totalSpikeCount\": " << brainStatus.totalSpikeCount << "\n";
        ss << "  },\n";
        ss << "  \"systemStatus\": {\n";
        ss << "    \"uptime\": " << systemStatus.uptime << ",\n";
        ss << "    \"cpuUsage\": " << systemStatus.cpuUsage << ",\n";
        ss << "    \"memoryUsed\": " << systemStatus.memoryUsed << ",\n";
        ss << "    \"memoryAvailable\": " << systemStatus.memoryAvailable << "\n";
        ss << "  },\n";
        ss << "  \"experimentStatus\": {\n";
        ss << "    \"name\": \"" << experimentStatus.name << "\",\n";
        ss << "    \"isRunning\": " << (experimentStatus.isRunning ? "true" : "false") << ",\n";
        ss << "    \"completionPercentage\": " << experimentStatus.completionPercentage << "\n";
        ss << "  }\n";
        ss << "}\n";
        return ss.str();
    }
    
    // Convert to text format
    std::string toText() const {
        std::stringstream ss;
        ss << "Status Snapshot\n";
        ss << "==============\n";
        ss << "Timestamp: " << std::chrono::system_clock::to_time_t(timestamp) << "\n";
        ss << "Level: " << static_cast<int>(level) << "\n";
        ss << "\nBrain Status:\n";
        ss << "  Current Step: " << brainStatus.currentStep << "\n";
        ss << "  Neurons: " << brainStatus.neuronCount << "\n";
        ss << "  Synapses: " << brainStatus.synapseCount << "\n";
        ss << "  Average Firing Rate: " << brainStatus.averageFiringRate << "%\n";
        ss << "  Total Spikes: " << brainStatus.totalSpikeCount << "\n";
        ss << "\nSystem Status:\n";
        ss << "  Uptime: " << systemStatus.uptime << " seconds\n";
        ss << "  CPU Usage: " << systemStatus.cpuUsage << "%\n";
        ss << "  Memory Used: " << systemStatus.memoryUsed << " bytes\n";
        ss << "\nExperiment Status:\n";
        ss << "  Name: " << experimentStatus.name << "\n";
        ss << "  Running: " << (experimentStatus.isRunning ? "Yes" : "No") << "\n";
        ss << "  Completion: " << experimentStatus.completionPercentage << "%\n";
        ss << "\nStatus Text:\n" << statusText << "\n";
        
        if (!activeAlerts.empty()) {
            ss << "\nActive Alerts:\n";
            for (const auto& alert : activeAlerts) {
                ss << "  - " << alert << "\n";
            }
        }
        
        if (!warningMessages.empty()) {
            ss << "\nWarnings:\n";
            for (const auto& warning : warningMessages) {
                ss << "  - " << warning << "\n";
            }
        }
        
        return ss.str();
    }
};

// Status event structure
struct StatusEvent {
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    std::string message;
    StatusLevel level;
    std::unordered_map<std::string, std::string> metadata;
    
    // Get age of event in seconds
    double getAge() const {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp);
        return static_cast<double>(duration.count());
    }
};

// StatusManager class - provides comprehensive status reporting
class StatusManager {
public:
    StatusManager(const StatusConfig& config = StatusConfig());
    ~StatusManager();
    
    // Disable copying
    StatusManager(const StatusManager&) = delete;
    StatusManager& operator=(const StatusManager&) = delete;
    
    // Configure the status manager
    void configure(const StatusConfig& config);
    
    // Update status components
    void updateBrainStatus(const BrainStatus& status);
    void updateSystemStatus(const SystemStatus& status);
    void updateExperimentStatus(const ExperimentStatus& status);
    
    // Get current status
    BrainStatus getBrainStatus() const;
    SystemStatus getSystemStatus() const;
    ExperimentStatus getExperimentStatus() const;
    
    // Get overall status level
    StatusLevel getOverallStatusLevel() const;
    
    // Get status text
    std::string getStatusText() const;
    
    // Create status snapshot
    StatusSnapshot createStatusSnapshot() const;
    
    // Save status snapshot
    bool saveStatusSnapshot(const std::string& filepath, const std::string& format = "json");
    
    // Load status snapshot
    bool loadStatusSnapshot(const StatusSnapshot& snapshot);
    
    // Get status history
    std::vector<StatusSnapshot> getStatusHistory() const;
    
    // Get status events
    std::vector<StatusEvent> getStatusEvents() const;
    
    // Add status event
    void addStatusEvent(const std::string& source,
                       const std::string& message,
                       StatusLevel level,
                       const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Get active alerts
    std::vector<std::string> getActiveAlerts() const;
    
    // Get warning messages
    std::vector<std::string> getWarningMessages() const;
    
    // Check if status has issues
    bool hasIssues() const;
    
    // Get issue count
    size_t getIssueCount() const;
    
    // Enable/disable status monitoring
    void enableStatusMonitoring(bool enable) { config_.enableRealTimeUpdates = enable; }
    void enableStatusLogging(bool enable) { config_.enableStatusLogging = enable; }
    void enableStatusSnapshots(bool enable) { config_.enableStatusSnapshots = enable; }
    
    // Get configuration
    const StatusConfig& getConfig() const { return config_; }
    
    // Export status to file
    bool exportStatus(const std::string& filepath, const std::string& format = "json");
    
    // Import status from file
    bool importStatus(const std::string& filepath, const std::string& format = "json");
    
    // Get global status manager instance
    static StatusManager& getGlobalStatusManager();
    
    // Set global status manager
    static void setGlobalStatusManager(std::shared_ptr<StatusManager> manager);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void updateStatusLevel();
    void createStatusSnapshotIfNeeded();
    void logStatusIfNeeded();
    void exportStatusIfNeeded();
    
    // Thread safety
    mutable std::mutex mutex_;
};

} // namespace nlm
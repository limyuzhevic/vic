#pragma once

#include "Types.hpp"
#include "Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <optional>
#include <sstream>

namespace nlm {

// Progress metrics enumeration
enum class ProgressMetric {
    SimulationProgress,
    MemoryUsage,
    PerformanceMetrics,
    SpikeCount,
    FiringRate,
    NeuronCount,
    SynapseCount,
    CheckpointProgress,
    ExperimentProgress,
    Custom
};

// Progress level enumeration
enum class ProgressLevel {
    Minimal,
    Normal,
    Detailed,
    Verbose
};

// Progress callback function signature
using ProgressCallback = std::function<void(
    double progressPercentage,
    const std::string& message,
    const std::unordered_map<std::string, double>& metrics,
    const std::chrono::system_clock::time_point& timestamp
)>;

// Progress milestone structure
struct ProgressMilestone {
    std::string name;
    double percentage;
    std::string description;
    bool reached{false};
    std::chrono::system_clock::time_point reachedTime;
    
    ProgressMilestone(const std::string& n, double p, const std::string& desc = "")
        : name(n), percentage(p), description(desc), reached(false) {}
};

// Progress metrics structure
struct ProgressMetrics {
    double simulationProgress{0.0};  // 0-100%
    size_t spikeCount{0};
    size_t neuronCount{0};
    size_t synapseCount{0};
    size_t firingNeuronCount{0};
    double firingRate{0.0};
    double excitationInhibitionRatio{0.0};
    size_t memoryUsage{0};  // in bytes
    double cpuUsage{0.0};   // percentage
    std::unordered_map<std::string, double> customMetrics;
    
    // Add a custom metric
    void addMetric(const std::string& name, double value) {
        customMetrics[name] = value;
    }
    
    // Get all metrics as a combined percentage (for backward compatibility)
    double getOverallProgress() const {
        // Weight different metrics
        double totalWeight = 0.5;  // simulation progress weight
        double weightedProgress = simulationProgress * 0.5;
        
        // Add firing rate contribution
        if (neuronCount > 0) {
            weightedProgress += (firingRate / 100.0) * 0.3;
            totalWeight += 0.3;
        }
        
        // Add spike count contribution
        if (synapseCount > 0) {
            weightedProgress += std::min(100.0, (static_cast<double>(spikeCount) / synapseCount) * 100.0) * 0.2;
            totalWeight += 0.2;
        }
        
        return totalWeight > 0 ? (weightedProgress / totalWeight) * 100.0 : simulationProgress;
    }
};

// Progress configuration structure
struct ProgressConfig {
    ProgressLevel verbosityLevel{ProgressLevel::Normal};
    std::chrono::milliseconds reportInterval{1000};  // milliseconds
    bool enableProgressBars{true};
    bool enableLogging{true};
    bool enableCallbacks{false};
    bool enableMilestones{true};
    bool enableETACalculation{true};
    double etaSmoothingFactor{0.3};  // for EMA calculation
    size_t maxProgressHistory{1000};
    std::string progressLogFile{};
    std::string statusExportFile{};
    
    // Custom progress metrics to track
    std::vector<ProgressMetric> trackedMetrics;
    
    // Progress bar style
    enum class ProgressBarStyle {
        Simple,
        Unicode,
        ASCII,
        Colored,
        Custom
    } progressBarStyle{ProgressBarStyle::Unicode};
    
    // Status verbosity levels
    enum class StatusVerbosity {
        Quiet,
        Summary,
        Detailed,
        Debug
    } statusVerbosity{StatusVerbosity::Summary};
    
    // Output stream for progress reporting
    std::ostream* outputStream{nullptr};
    
    ProgressConfig() {
        trackedMetrics = {
            ProgressMetric::SimulationProgress,
            ProgressMetric::SpikeCount,
            ProgressMetric::FiringRate,
            ProgressMetric::NeuronCount,
            ProgressMetric::SynapseCount
        };
    }
};

// Estimated time remaining calculation
struct ETAInfo {
    double etaSeconds{0.0};
    double etaPercentage{0.0};
    bool isEstimated{false};
    std::chrono::system_clock::time_point lastUpdate;
    
    ETAInfo() : etaSeconds(0.0), etaPercentage(0.0), isEstimated(false) {
        lastUpdate = std::chrono::system_clock::now();
    }
};

// ProgressReporter class - main interface for progress tracking
class ProgressReporter {
public:
    ProgressReporter(const ProgressConfig& config = ProgressConfig());
    ~ProgressReporter();
    
    // Disable copying
    ProgressReporter(const ProgressReporter&) = delete;
    ProgressReporter& operator=(const ProgressReporter&) = delete;
    
    // Configure the reporter
    void configure(const ProgressConfig& config);
    
    // Initialize progress
    void startProgress(const std::string& operationName,
                      size_t totalSteps = 0,
                      double totalWork = 100.0);
    
    // Update progress
    void updateProgress(double progressPercentage,
                       const std::string& message = "",
                       const ProgressMetrics& metrics = ProgressMetrics());
    
    // Update progress with current step information
    void updateProgress(size_t currentStep,
                      size_t totalSteps,
                      const std::string& message = "",
                      const ProgressMetrics& metrics = ProgressMetrics());
    
    // Set the current operation
    void setOperation(const std::string& operationName);
    
    // Set total work/steps
    void setTotalWork(size_t totalSteps, double totalWork = 100.0);
    
    // Add or remove progress callbacks
    void addProgressCallback(const ProgressCallback& callback);
    void removeProgressCallback(const ProgressCallback& callback);
    
    // Add or remove milestones
    void addMilestone(const ProgressMilestone& milestone);
    void removeMilestone(const ProgressMilestone& milestone);
    
    // Pause/resume progress reporting
    void pauseProgress();
    void resumeProgress();
    bool isPaused() const { return isPaused_; }
    
    // Finish progress
    void finishProgress(const std::string& finalMessage = "");
    
    // Get current progress
    double getProgressPercentage() const { return currentProgress_; }
    const std::string& getCurrentOperation() const { return currentOperationName_; }
    const ProgressMetrics& getCurrentMetrics() const { return currentMetrics_; }
    
    // Get ETA information
    const ETAInfo& getETAInfo() const { return etaInfo_; }
    
    // Get elapsed time
    double getElapsedTime() const;
    
    // Get progress history
    std::vector<std::pair<double, ProgressMetrics>> getProgressHistory() const;
    
    // Get completed milestones
    std::vector<ProgressMilestone> getCompletedMilestones() const;
    
    // Check if milestone reached
    bool isMilestoneReached(const std::string& milestoneName) const;
    
    // Reset reporter
    void reset();
    
    // Enable/disable logging
    void enableLogging(bool enable) { config_.enableLogging = enable; }
    void enableProgressBars(bool enable) { config_.enableProgressBars = enable; }
    void enableCallbacks(bool enable) { config_.enableCallbacks = enable; }
    
    // Get configuration
    const ProgressConfig& getConfig() const { return config_; }
    
    // Format progress for output
    std::string formatProgress() const;
    
    // Format metrics for output
    std::string formatMetrics(const ProgressMetrics& metrics) const;
    
    // Format ETA information
    std::string formatETA(const ETAInfo& eta) const;
    
    // Export status to file
    bool exportStatus(const std::string& filepath, const std::string& format = "json");
    
    // Log progress to file
    bool logProgress(const std::string& message, LogLevel level = LogLevel::Info);
    
    // Get global reporter instance
    static ProgressReporter& getGlobalReporter();
    
    // Set global reporter
    static void setGlobalReporter(std::shared_ptr<ProgressReporter> reporter);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void updateETA(double progressPercentage);
    void checkMilestones(double progressPercentage);
    void invokeCallbacks(double progressPercentage, const std::string& message,
                        const ProgressMetrics& metrics);
    void updateProgressHistory(double progressPercentage,
                              const ProgressMetrics& metrics);
    void outputProgress(double progressPercentage,
                       const std::string& message,
                       const ProgressMetrics& metrics);
    
    // Thread safety
    mutable std::mutex mutex_;
};

} // namespace nlm
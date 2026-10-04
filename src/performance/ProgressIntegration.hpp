#pragma once

#include "ProgressReporter.hpp"
#include "StatusManager.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>

namespace nlm {

// Integration configuration
struct IntegrationConfig {
    bool enableProgressTracking{true};
    bool enableStatusReporting{true};
    bool enableBrainIntegration{true};
    bool enableExperimentIntegration{true};
    bool enableClockIntegration{true};
    bool enableCheckpointIntegration{true};
    
    // Progress reporting configuration
    ProgressReporter::ProgressConfig progressConfig;
    
    // Status reporting configuration
    StatusManager::StatusConfig statusConfig;
    
    // Integration-specific configurations
    struct {
        bool reportStepProgress{true};
        bool reportMetrics{true};
        bool reportMemoryUsage{true};
        bool reportPerformanceMetrics{true};
        bool reportCheckpointProgress{true};
    } brainIntegration;
    
    struct {
        bool reportExperimentProgress{true};
        bool reportMetrics{true};
        bool reportExperimentStatus{true};
    } experimentIntegration;
    
    struct {
        bool reportTimeProgress{true};
        bool reportClockStatus{true};
    } clockIntegration;
    
    struct {
        bool reportCheckpointProgress{true};
        bool reportSaveLoadProgress{true};
    } checkpointIntegration;
};

// Brain integration wrapper
class BrainProgressIntegration {
public:
    BrainProgressIntegration(std::shared_ptr<ProgressReporter> progressReporter,
                           std::shared_ptr<StatusManager> statusManager,
                           const IntegrationConfig& config);
    
    // Initialize integration with Brain instance
    void initialize(std::shared_ptr<Brain> brain);
    
    // Update progress based on Brain state
    void updateProgress();
    
    // Get current progress metrics
    ProgressMetrics getProgressMetrics() const;
    
    // Get current status
    BrainStatus getStatus() const;
    
    // Start monitoring Brain progress
    void startMonitoring();
    
    // Stop monitoring
    void stopMonitoring();
    
    // Check if monitoring
    bool isMonitoring() const { return isMonitoring_; }
    
    // Set operation name for progress reporting
    void setOperation(const std::string& operationName);
    
    // Set total steps
    void setTotalSteps(SimulationStep totalSteps);
    
    // Record a simulation step
    void recordStep(SimulationStep currentStep);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety
    mutable std::mutex mutex_;
};

// ExperimentRunner integration wrapper
class ExperimentProgressIntegration {
public:
    ExperimentProgressIntegration(std::shared_ptr<ProgressReporter> progressReporter,
                                 std::shared_ptr<StatusManager> statusManager,
                                 const IntegrationConfig& config);
    
    // Initialize integration with ExperimentRunner instance
    void initialize(std::shared_ptr<ExperimentRunner> experimentRunner);
    
    // Update progress based on experiment state
    void updateProgress();
    
    // Get current progress metrics
    ProgressMetrics getProgressMetrics() const;
    
    // Get current status
    ExperimentStatus getStatus() const;
    
    // Start monitoring experiment progress
    void startMonitoring();
    
    // Stop monitoring
    void stopMonitoring();
    
    // Check if monitoring
    bool isMonitoring() const { return isMonitoring_; }
    
    // Set operation name for progress reporting
    void setOperation(const std::string& operationName);
    
    // Set total experiments
    void setTotalExperiments(size_t totalExperiments);
    
    // Record experiment completion
    void recordExperimentCompletion(const std::string& experimentName);
    
    // Record experiment progress
    void recordExperimentProgress(double progress);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety
    mutable std::mutex mutex_;
};

// SimulationClock integration wrapper
class ClockProgressIntegration {
public:
    ClockProgressIntegration(std::shared_ptr<ProgressReporter> progressReporter,
                           std::shared_ptr<StatusManager> statusManager,
                           const IntegrationConfig& config);
    
    // Initialize integration with SimulationClock instance
    void initialize(std::shared_ptr<SimulationClock> clock);
    
    // Update progress based on clock state
    void updateProgress();
    
    // Get current progress metrics
    ProgressMetrics getProgressMetrics() const;
    
    // Get current status
    SystemStatus getStatus() const;
    
    // Start monitoring clock progress
    void startMonitoring();
    
    // Stop monitoring
    void stopMonitoring();
    
    // Check if monitoring
    bool isMonitoring() const { return isMonitoring_; }
    
    // Set operation name for progress reporting
    void setOperation(const std::string& operationName);
    
    // Set total simulation time
    void setTotalSimulationTime(Timestamp totalTime);
    
    // Record clock step
    void recordClockStep(SimulationStep currentStep, Timestamp currentTime);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety
    mutable std::mutex mutex_;
};

// CheckpointManager integration wrapper
class CheckpointProgressIntegration {
public:
    CheckpointProgressIntegration(std::shared_ptr<ProgressReporter> progressReporter,
                                 std::shared_ptr<StatusManager> statusManager,
                                 const IntegrationConfig& config);
    
    // Initialize integration with CheckpointManager instance
    void initialize(std::shared_ptr<CheckpointManager> checkpointManager);
    
    // Update progress based on checkpoint state
    void updateProgress();
    
    // Get current progress metrics
    ProgressMetrics getProgressMetrics() const;
    
    // Get current status
    SystemStatus getStatus() const;
    
    // Start monitoring checkpoint progress
    void startMonitoring();
    
    // Stop monitoring
    void stopMonitoring();
    
    // Check if monitoring
    bool isMonitoring() const { return isMonitoring_; }
    
    // Set operation name for progress reporting
    void setOperation(const std::string& operationName);
    
    // Set total checkpoints to save
    void setTotalCheckpoints(size_t totalCheckpoints);
    
    // Record checkpoint save
    void recordCheckpointSave(const std::string& checkpointName);
    
    // Record checkpoint load
    void recordCheckpointLoad(const std::string& checkpointName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety
    mutable std::mutex mutex_;
};

// Main integration manager
class ProgressIntegrationManager {
public:
    ProgressIntegrationManager(const IntegrationConfig& config = IntegrationConfig());
    ~ProgressIntegrationManager();
    
    // Disable copying
    ProgressIntegrationManager(const ProgressIntegrationManager&) = delete;
    ProgressIntegrationManager& operator=(const ProgressIntegrationManager&) = delete;
    
    // Configure integration
    void configure(const IntegrationConfig& config);
    
    // Initialize all integrations
    void initialize(std::shared_ptr<Brain> brain,
                   std::shared_ptr<ExperimentRunner> experimentRunner,
                   std::shared_ptr<SimulationClock> clock,
                   std::shared_ptr<CheckpointManager> checkpointManager);
    
    // Update all progress integrations
    void updateProgress();
    
    // Start all monitoring
    void startMonitoring();
    
    // Stop all monitoring
    void stopMonitoring();
    
    // Check if monitoring
    bool isMonitoring() const;
    
    // Get global integration manager instance
    static ProgressIntegrationManager& getGlobalIntegrationManager();
    
    // Set global integration manager
    static void setGlobalIntegrationManager(std::shared_ptr<ProgressIntegrationManager> manager);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety
    mutable std::mutex mutex_;
};

} // namespace nlm
// Progress tracking and monitoring for NLM
// Provides real-time progress reporting for long-running operations

#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <functional>
#include <atomic>

namespace nlm {

// Progress bar style
enum class ProgressBarStyle {
    Simple,
    Fancy,
    ProgressLine
};

// Progress update callback
using ProgressCallback = std::function<void(double, const std::string&, const std::string&)>;

class ProgressReporter {
public:
    ProgressReporter();
    ~ProgressReporter();
    
    // Configure progress reporting
    void setProgressBarStyle(ProgressBarStyle style);
    void setShowPercentage(bool show);
    void setShowTimeRemaining(bool show);
    void setUpdateInterval(std::chrono::milliseconds interval);
    void setUseColors(bool useColors);
    
    // Start/Stop progress tracking
    void startProgress(const std::string& operationName, int totalSteps = 0);
    void updateProgress(int currentStep, const std::string& message = "");
    void updateProgress(double percentage, const std::string& message = "");
    void stopProgress();
    
    // Progress monitoring for long operations
    void startMonitoring(const std::string& operationName, const std::chrono::seconds& estimatedDuration = std::chrono::seconds(0));
    void updateMonitoring(double progress, const std::string& message = "");
    void stopMonitoring();
    
    // Experiment progress reporting
    void startExperimentProgress(const std::string& experimentName, int totalPhases = 0);
    void updateExperimentPhase(int phaseNumber, const std::string& phaseName, double progress = 0.0);
    void updateExperimentStep(int stepNumber, const std::string& stepDescription);
    void stopExperimentProgress();
    
    // Simulation progress reporting
    void startSimulationProgress(const std::string& simulationName, SimulationStep totalSteps);
    void updateSimulationStep(SimulationStep currentStep, const std::string& statusMessage = "");
    void stopSimulationProgress();
    
    // File operation progress
    void startFileOperationProgress(const std::string& operation, const std::string& filename, size_t totalBytes = 0);
    void updateFileOperationProgress(size_t bytesTransferred, const std::string& filename);
    void stopFileOperationProgress();
    
    // Initialization progress
    void startInitializationProgress(const std::string& componentName);
    void updateInitializationStep(const std::string& stepName, double progress = 0.0);
    void stopInitializationProgress();
    
    // Set callback for progress updates
    void setProgressCallback(const ProgressCallback& callback);
    
    // Get current progress information
    double getCurrentProgress() const;
    std::string getCurrentOperation() const;
    std::chrono::system_clock::time_point getStartTime() const;
    std::chrono::milliseconds getElapsedTime() const;
    std::chrono::milliseconds getEstimatedTimeRemaining() const;
    
    // Generate progress report
    std::string generateProgressReport() const;
    
    // Export progress data
    bool exportProgressData(const std::string& filepath, const std::string& format = "csv") const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    void displayProgressBar(double progress, const std::string& operation, const std::string& message) const;
    void displayProgressLine(double progress, const std::string& operation, const std::string& message) const;
    void displaySimpleProgress(double progress, const std::string& operation, const std::string& message) const;
    
    // Time calculations
    std::chrono::milliseconds calculateElapsedTime(std::chrono::system_clock::time_point startTime) const;
    std::chrono::milliseconds calculateEstimatedTimeRemaining(double progress, std::chrono::system_clock::time_point startTime) const;
    
    // Format display strings
    std::string formatProgressPercentage(double progress) const;
    std::string formatElapsedTime(std::chrono::milliseconds ms) const;
    std::string formatTimeRemaining(std::chrono::milliseconds ms) const;
};

} // namespace nlm

// Progress Monitoring and Status Reporting System
//
// This file provides comprehensive progress monitoring and status reporting
// capabilities for NLM experiments and simulations.

#ifndef PROGRESS_MONITOR_HPP
#define PROGRESS_MONITOR_HPP

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <memory>
#include <functional>

namespace nlm {

class ProgressMonitor {
public:
    ProgressMonitor();
    ~ProgressMonitor();
    
    // Set the total work units
    void setTotalSteps(int totalSteps);
    
    // Set the current work unit
    void setCurrentStep(int currentStep);
    
    // Set the operation description
    void setOperation(const std::string& operation);
    
    // Update progress with a message
    void updateProgress(int step, const std::string& message);
    
    // Complete the progress monitor
    void complete(const std::string& finalMessage = "");
    
    // Get the current progress percentage
    float getProgressPercentage() const;
    
    // Get the current step
    int getCurrentStep() const;
    
    // Get the total steps
    int getTotalSteps() const;
    
    // Get the operation description
    const std::string& getOperation() const;
    
    // Check if progress monitoring is enabled
    bool isEnabled() const;
    
    // Enable or disable progress monitoring
    void enable(bool enabled);
    
    // Set the output stream for progress messages
    void setOutputStream(std::ostream& outputStream);
    
    // Get the elapsed time in seconds
    double getElapsedTime() const;
    
    // Get the estimated time remaining in seconds
    double getEstimatedTimeRemaining() const;
    
    // Start timing
    void startTiming();
    
    // Stop timing
    void stopTiming();
    
    // Set the update frequency (how often to update the display)
    void setUpdateFrequency(int frequency);
    
    // Check if it's time to update the display
    bool shouldUpdateDisplay(int currentStep) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Status Reporter for comprehensive system status
class StatusReporter {
public:
    StatusReporter();
    ~StatusReporter();
    
    // Add a status entry
    void addStatusEntry(const std::string& category, const std::string& message);
    
    // Add a performance metric
    void addPerformanceMetric(const std::string& name, double value, const std::string& unit = "");
    
    // Add a system metric
    void addSystemMetric(const std::string& name, double value, const std::string& unit = "");
    
    // Generate a comprehensive status report
    std::string generateReport() const;
    
    // Print status report to output stream
    void printReport(std::ostream& outputStream = std::cout) const;
    
    // Clear all status entries
    void clear();
    
    // Set the verbosity level
    void setVerbosityLevel(int level);
    
    // Get the verbosity level
    int getVerbosityLevel() const;
    
    // Enable or disable color output
    void setColorOutput(bool enabled);
    
    // Check if color output is enabled
    bool getColorOutput() const;
    
    // Add a timing checkpoint
    void addTimingCheckpoint(const std::string& checkpointName);
    
    // Get timing information for a checkpoint
    double getCheckpointTime(const std::string& checkpointName) const;
    
    // Get total time since first checkpoint
    double getTotalTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experiment Progress Tracker for detailed experiment monitoring
class ExperimentProgressTracker {
public:
    ExperimentProgressTracker();
    ~ExperimentProgressTracker();
    
    // Start tracking an experiment
    void startExperiment(const std::string& experimentName);
    
    // End tracking an experiment
    void endExperiment();
    
    // Record a progress milestone
    void recordMilestone(const std::string& milestone, int step, const std::string& description = "");
    
    // Record a system event
    void recordEvent(const std::string& event, const std::string& details = "");
    
    // Record a performance event
    void recordPerformance(const std::string& event, double value, const std::string& unit = "");
    
    // Get the experiment name
    const std::string& getExperimentName() const;
    
    // Get the total steps
    int getTotalSteps() const;
    
    // Get the current step
    int getCurrentStep() const;
    
    // Set the total steps
    void setTotalSteps(int totalSteps);
    
    // Get the elapsed time in seconds
    double getElapsedTime() const;
    
    // Generate a progress report
    std::string generateProgressReport() const;
    
    // Print progress report to output stream
    void printProgressReport(std::ostream& outputStream = std::cout) const;
    
    // Check if experiment is currently running
    bool isRunning() const;
    
    // Enable or disable detailed tracking
    void enableDetailedTracking(bool enabled);
    
    // Check if detailed tracking is enabled
    bool getDetailedTracking() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // PROGRESS_MONITOR_HPP
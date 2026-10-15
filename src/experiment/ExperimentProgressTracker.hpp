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
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"

namespace nlm {

// Experiment progress tracking configuration
struct ExperimentProgressTrackerConfig {
    std::string experimentName;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::atomic<double> progressPercentage;
    std::string currentStage;
    std::map<std::string, double> stageProgress;
    std::string currentTask;
    std::atomic<size_t> completedTasks;
    std::atomic<size_t> totalTasks;
    std::string currentError;
    bool isPaused;
    bool isCompleted;
    
    ExperimentProgressTrackerConfig() 
        : progressPercentage(0.0), completedTasks(0), totalTasks(0), 
          isPaused(false), isCompleted(false),
          startTime(std::chrono::system_clock::now()) {}
};

// Progress update event
struct ProgressUpdateEvent {
    std::string experimentName;
    std::string stageName;
    double progressPercentage;
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::map<std::string, std::string> metadata;
    
    ProgressUpdateEvent() : progressPercentage(0.0), timestamp(std::chrono::system_clock::now()) {}
    ProgressUpdateEvent(const std::string& expName, const std::string& stage, 
                       double progress, const std::string& msg)
        : experimentName(expName), stageName(stage), progressPercentage(progress),
          message(msg), timestamp(std::chrono::system_clock::now()) {}
};

// Progress callback type
using ProgressCallback = std::function<void(const ProgressUpdateEvent&)>;

class ExperimentProgressTracker {
public:
    ExperimentProgressTracker();
    ~ExperimentProgressTracker();
    
    // Disable copying, enable moving
    ExperimentProgressTracker(const ExperimentProgressTracker&) = delete;
    ExperimentProgressTracker& operator=(const ExperimentProgressTracker&) = delete;
    ExperimentProgressTracker(ExperimentProgressTracker&&) noexcept;
    ExperimentProgressTracker& operator=(ExperimentProgressTracker&&) noexcept;
    
    // Progress tracking
    bool startTracking(const std::string& experimentName);
    bool stopTracking(const std::string& experimentName);
    bool pauseTracking(const std::string& experimentName);
    bool resumeTracking(const std::string& experimentName);
    
    // Progress reporting
    bool reportProgress(const std::string& experimentName, double progressPercentage);
    bool reportStageProgress(const std::string& experimentName, const std::string& stageName, 
                           double progressPercentage);
    bool reportTaskCompletion(const std::string& experimentName, const std::string& taskName);
    bool reportError(const std::string& experimentName, const std::string& errorMessage);
    bool reportWarning(const std::string& experimentName, const std::string& warningMessage);
    
    // Progress querying
    ExperimentProgressTrackerConfig getProgressConfig(const std::string& experimentName) const;
    double getProgressPercentage(const std::string& experimentName) const;
    std::string getCurrentStage(const std::string& experimentName) const;
    std::string getCurrentTask(const std::string& experimentName) const;
    size_t getCompletedTasks(const std::string& experimentName) const;
    size_t getTotalTasks(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getExperimentStartTime(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getExperimentEndTime(const std::string& experimentName) const;
    std::chrono::seconds getElapsedTime(const std::string& experimentName) const;
    std::chrono::seconds getEstimatedTimeRemaining(const std::string& experimentName) const;
    
    // Callback management
    bool addProgressCallback(const std::string& experimentName, ProgressCallback callback);
    bool removeProgressCallback(const std::string& experimentName, ProgressCallback callback);
    bool clearProgressCallbacks(const std::string& experimentName);
    std::vector<ProgressCallback> getProgressCallbacks(const std::string& experimentName) const;
    
    // Progress history and analytics
    std::vector<ProgressUpdateEvent> getProgressHistory(const std::string& experimentName, 
                                                      size_t limit = 100) const;
    std::vector<ProgressUpdateEvent> getStageProgressHistory(const std::string& experimentName, 
                                                          const std::string& stageName,
                                                          size_t limit = 50) const;
    std::vector<double> getProgressTimeline(const std::string& experimentName, 
                                           const std::chrono::system_clock::time_point& fromTime,
                                           const std::chrono::system_clock::time_point& toTime) const;
    
    // Progress analysis
    double calculateProgressEfficiency(const std::string& experimentName) const;
    double calculateProgressVariance(const std::string& experimentName) const;
    std::string analyzeProgressTrends(const std::string& experimentName) const;
    bool predictCompletionTime(const std::string& experimentName, 
                              std::chrono::system_clock::time_point& predictedTime) const;
    
    // Progress optimization
    bool autoAdjustReportingFrequency(const std::string& experimentName);
    bool enableProgressSmoothing(const std::string& experimentName, bool enable);
    bool setProgressThresholds(const std::string& experimentName, 
                              const std::map<std::string, double>& thresholds);
    
    // Cleanup and maintenance
    bool cleanupProgressData(const std::string& experimentName);
    bool cleanupExpiredProgress(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetProgress(const std::string& experimentName);
    bool clearAllProgress(const std::string& experimentName);
    std::vector<std::string> getAllTrackingExperiments() const;
    size_t getTrackingExperimentCount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
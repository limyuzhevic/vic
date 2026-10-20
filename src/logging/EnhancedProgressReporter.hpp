// Advanced progress reporting with enhanced features
// This file implements an enhanced ProgressReporter with:
// - Sophisticated progress bar animations
// - Hierarchical progress reporting
// - Progress milestones and notifications
// - Multi-stage progress tracking
// - Multiple progress visualization styles
// - Advanced progress estimation algorithms

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <set>
#include "core/Logger/Logger.hpp"

namespace nlm {

// Enhanced ProgressReport structure with additional metadata
struct EnhancedProgressReport {
    std::string testName;
    size_t currentStep;
    size_t totalSteps;
    double percentage;
    std::string statusMessage;
    std::chrono::milliseconds estimatedTimeRemaining;
    std::chrono::milliseconds actualTimeElapsed;
    double progressSpeed; // steps per second
    
    // Enhanced fields
    std::chrono::steady_clock::time_point startTime;
    std::string description;
    std::string stage;
    std::vector<std::string> milestones;
    std::map<std::string, double> customMetrics;
    std::atomic<bool> paused{false};
    std::atomic<bool> completed{false};
    std::atomic<bool> failed{false};
    
    EnhancedProgressReport(const std::string& name, size_t current, size_t total)
        : testName(name), currentStep(current), totalSteps(total), 
          percentage(total > 0 ? (static_cast<double>(current) / total) * 100.0 : 0.0),
          statusMessage("In Progress"), estimatedTimeRemaining(0), 
          actualTimeElapsed(0), progressSpeed(0.0) {
        startTime = std::chrono::steady_clock::now();
    }
};

// Progress bar style enumeration
enum class ProgressBarStyle {
    Classic,          // Traditional [=====>    ] style
    Sparkle,          // Animated with sparkle effects
    Blocks,           // Unicode block characters
    ASCII,            // ASCII-only characters
    Custom1,         // Custom style 1
    Custom2          // Custom style 2
};

// Progress notification types
enum class NotificationType {
    Milestone,        // Progress milestone reached
    StageChange,      // Moving to a new stage
    SpeedWarning,     // Performance warning
    EstimatedCompletion, // Estimated completion
    Custom          // Custom notification
};

// Progress notification
struct ProgressNotification {
    NotificationType type;
    std::string message;
    std::string testName;
    std::chrono::steady_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    
    ProgressNotification(NotificationType t, const std::string& msg, const std::string& name)
        : type(t), message(msg), testName(name) {
        timestamp = std::chrono::steady_clock::now();
    }
};

// Progress milestone tracking
struct ProgressMilestone {
    std::string name;
    size_t step;           // Step at which milestone occurs
    double percentage;     // Percentage threshold
    std::string description;
    std::function<void(const EnhancedProgressReport&)> callback;
    
    ProgressMilestone(const std::string& n, size_t s, double p, const std::string& desc)
        : name(n), step(s), percentage(p), description(desc) {}
};

// Hierarchical progress context
struct ProgressStage {
    std::string name;
    size_t totalSteps;
    size_t currentSteps;
    std::map<std::string, EnhancedProgressReport> subProgress;
    
    ProgressStage(const std::string& n, size_t total) 
        : name(n), totalSteps(total), currentSteps(0) {}
};

class EnhancedProgressReporter {
public:
    EnhancedProgressReporter(std::shared_ptr<Logger> logger);
    ~EnhancedProgressReporter();
    
    // Initialize progress with enhanced options
    void startProgress(const std::string& testName, size_t totalSteps,
                      const std::string& description = "",
                      const std::string& stage = "");
    
    // Update progress with milestone detection
    void updateProgress(const std::string& testName, size_t currentStep,
                       const std::string& status = "In Progress",
                       std::map<std::string, double> customMetrics = {});
    
    // Multi-stage progress
    void startStage(const std::string& stageName, size_t totalSteps,
                   const std::string& description = "");
    void updateStage(const std::string& stageName, size_t currentStep,
                    const std::string& description = "");
    void completeStage(const std::string& stageName, bool success,
                      const std::string& finalMessage = "");
    
    // Milestone management
    void addMilestone(const std::string& testName, const ProgressMilestone& milestone);
    void removeMilestone(const std::string& testName, const std::string& milestoneName);
    void clearMilestones(const std::string& testName);
    
    // Progress completion
    void completeProgress(const std::string& testName, bool success,
                         const std::string& finalMessage = "");
    void stopProgress(const std::string& testName);
    
    // Display and configuration
    void setProgressBarStyle(ProgressBarStyle style);
    void setEnableProgressBar(bool enable);
    void setEnableTimeEstimates(bool enable);
    void setEnableColors(bool enable);
    void setEnableAnimations(bool enable);
    void setDisplayInterval(size_t interval);
    void setUpdateFrequency(double frequency);
    void setProgressUpdateThreshold(double threshold); // Percentage threshold for updates
    
    // Output and formatting
    void setOutputStream(std::ostream& stream);
    void setProgressBarWidth(int width);
    void setShowPercentage(bool show);
    void setShowSpeed(bool show);
    void setShowETA(bool show);
    void setShowCustomMetrics(bool show);
    
    // Notifications
    void setNotificationEnabled(bool enable);
    void setNotificationCallback(std::function<void(const ProgressNotification&)> callback);
    void addSpeedWarningThreshold(double stepsPerSecond);
    
    // Progress tracking
    std::vector<EnhancedProgressReport> getActiveProgress() const;
    bool isProgressActive(const std::string& testName) const;
    std::vector<ProgressStage> getActiveStages() const;
    
    // Statistics and analysis
    std::chrono::milliseconds getTotalElapsedTime() const;
    double getAverageProgressSpeed() const;
    size_t getCompletedSteps(const std::string& testName) const;
    
    // Configuration
    void setProgressCallback(std::function<void(const EnhancedProgressReport&)> callback);
    void setProgressThresholdCallback(std::function<void(double)> callback); // Callback for overall progress
    
    // Advanced features
    void enableHierarchicalProgress(bool enable);
    void enableParallelProgress(bool enable);
    void enableAdaptiveThrottling(bool enable);
    void setMaxMemoryUsage(size_t maxBytes);
    
    // Statistics collection
    struct PerformanceStats {
        double averageSpeed;
        double minSpeed;
        double maxSpeed;
        std::chrono::milliseconds totalTime;
        size_t completedSteps;
        double efficiencyScore;
        
        PerformanceStats() : averageSpeed(0.0), minSpeed(0.0), maxSpeed(0.0),
                           totalTime(0), completedSteps(0), efficiencyScore(0.0) {}
    };
    
    PerformanceStats getPerformanceStats(const std::string& testName) const;
    std::string generateProgressReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void displayProgress(const EnhancedProgressReport& report);
    void updateProgressInternal(const std::string& testName, size_t currentStep,
                               const std::string& status,
                               std::map<std::string, double> customMetrics);
    void triggerNotifications(const std::string& testName, const EnhancedProgressReport& report);
    void checkMilestones(const std::string& testName, const EnhancedProgressReport& report);
    void updateStageProgress(const std::string& stageName, const EnhancedProgressReport& subReport);
    void calculateAdvancedETA(EnhancedProgressReport& report) const;
    double calculateEfficiencyScore(const EnhancedProgressReport& report) const;
    void cleanupCompletedProgress();
    std::string formatProgressBar(const EnhancedProgressReport& report, ProgressBarStyle style) const;
    std::string formatCustomMetrics(const std::map<std::string, double>& metrics) const;
    void logProgressEvent(const std::string& event, const std::string& testName,
                         const std::map<std::string, std::string>& metadata = {});
};
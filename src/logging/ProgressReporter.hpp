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
#include "core/Logger/Logger.hpp"

namespace nlm {

// Progress reporting types and classes
struct ProgressReport {
    std::string testName;
    size_t currentStep;
    size_t totalSteps;
    double percentage;
    std::string statusMessage;
    std::chrono::milliseconds estimatedTimeRemaining;
    std::chrono::milliseconds actualTimeElapsed;
    double progressSpeed; // steps per second
    
    ProgressReport(const std::string& name, size_t current, size_t total)
        : testName(name), currentStep(current), totalSteps(total), 
          percentage(total > 0 ? (static_cast<double>(current) / total) * 100.0 : 0.0),
          statusMessage("In Progress"), estimatedTimeRemaining(0), 
          actualTimeElapsed(0), progressSpeed(0.0) {}
};

class ProgressReporter {
public:
    ProgressReporter(std::shared_ptr<Logger> logger);
    ~ProgressReporter();
    
    // Start progress reporting for a test
    void startProgress(const std::string& testName, size_t totalSteps);
    
    // Update progress
    void updateProgress(const std::string& testName, size_t currentStep, 
                       const std::string& status = "In Progress");
    
    // Complete progress reporting
    void completeProgress(const std::string& testName, bool success, 
                         const std::string& finalMessage = "Completed");
    
    // Stop progress reporting for a test
    void stopProgress(const std::string& testName);
    
    // Set display options
    void setDisplayInterval(size_t interval);
    void setEnableColors(bool enable);
    void setOutputStream(std::ostream& stream);
    void setEnableProgressBar(bool enable);
    void setEnableTimeEstimates(bool enable);
    
    // Progress tracking
    std::vector<ProgressReport> getActiveProgress() const;
    bool isProgressActive(const std::string& testName) const;
    
    // Statistics
    size_t getTotalProgressUpdates() const;
    std::chrono::milliseconds getTotalElapsedTime() const;
    
    // Configuration
    void setUpdateFrequency(double frequency); // updates per second
    void setProgressCallback(std::function<void(const ProgressReport&)> callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void displayProgress(const ProgressReport& report);
    void updateProgressInternal(const std::string& testName, size_t currentStep, const std::string& status);
    void completeProgressInternal(const std::string& testName, bool success, const std::string& finalMessage);
    std::chrono::milliseconds calculateEstimatedTimeRemaining(const ProgressReport& report) const;
    double calculateProgressSpeed(const ProgressReport& report) const;
};

// Advanced logging utilities
struct LogContext {
    std::string operation;
    std::string component;
    std::string testName;
    std::chrono::steady_clock::time_point startTime;
    std::map<std::string, std::string> metadata;
    
    LogContext(const std::string& op, const std::string& comp, const std::string& test)
        : operation(op), component(comp), testName(test) {
        startTime = std::chrono::steady_clock::now();
    }
};

class AdvancedLogger {
public:
    AdvancedLogger(std::shared_ptr<Logger> baseLogger);
    ~AdvancedLogger();
    
    // Operation timing and logging
    void logOperation(const std::string& operation, const std::string& component,
                     const std::string& testName = "", bool success = true,
                     const std::string& errorMessage = "");
    
    void logWithContext(const LogContext& context, const std::string& message,
                       const std::string& level = "INFO");
    
    // Progress-aware logging
    void logProgress(const std::string& testName, size_t step, size_t total,
                    const std::string& message);
    
    // Performance metrics
    void logPerformanceMetric(const std::string& metricName, double value,
                             const std::string& unit = "", 
                             const std::string& testName = "");
    
    // Structured logging for scripting
    std::string generateLogJSON(const std::vector<LogContext>& contexts) const;
    void exportLogs(const std::string& filepath) const;
    
    // Error handling with context
    void logErrorWithContext(const std::string& error, const std::string& operation,
                           const std::string& component, const std::string& testName = "");
    
    // Batch logging
    void logBatch(const std::vector<std::string>& messages, 
                 const std::string& level = "INFO");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void formatLogEntry(std::stringstream& stream, const std::string& level,
                       const std::string& message, const LogContext& context) const;
    std::string generateTimestamp() const;
};

} // namespace nlm

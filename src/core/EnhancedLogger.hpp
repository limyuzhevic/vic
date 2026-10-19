#pragma once

#include <string>
#include <memory>
#include <map>
#include "core/Logger/Logger.hpp"

namespace nlm {

class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger() = default;
    
    // Initialize logger with session information
    void initialize(const std::string& sessionId = "", bool enableProgressTracking = false);
    
    // Log session start/end
    void logSessionStart(const std::string& sessionId);
    void logSessionEnd(const std::string& sessionId, double duration);
    
    // Log progress
    void logProgress(const std::string& stage, double progress, const std::string& message = "");
    
    // Log experiment run
    void logExperiment(const std::string& name, double duration, bool success, const std::string& details = "");
    
    // Get session statistics
    std::string getSessionStats() const;
    
    // Get performance metrics
    std::string getPerformanceMetrics() const;
    
    // Enable/disable features
    void enableProgressTracking(bool enable) { progressTrackingEnabled_ = enable; }
    void enableSessionLogging(bool enable) { sessionLoggingEnabled_ = enable; }
    void enablePerformanceMetrics(bool enable) { performanceMetricsEnabled_ = enable; }
    void setLevel(LogLevel level) { Logger::getGlobal().setLevel(level); }
    
private:
    double sessionStartTime_;
    bool progressTrackingEnabled_;
    bool sessionLoggingEnabled_;
    bool performanceMetricsEnabled_;
    std::map<std::string, double> experimentTimes_;
    std::vector<std::string> experimentLogs_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

} // namespace nlm
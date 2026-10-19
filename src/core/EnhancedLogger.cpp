// EnhancedLogger implementation
#include "EnhancedLogger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

EnhancedLogger::EnhancedLogger() 
    : sessionStartTime_(0.0), progressTrackingEnabled_(false), 
      sessionLoggingEnabled_(false), performanceMetricsEnabled_(false) {
    startTime_ = std::chrono::high_resolution_clock::now();
}

void EnhancedLogger::initialize(const std::string& sessionId, bool enableProgressTracking) {
    logSessionStart(sessionId);
    progressTrackingEnabled_ = enableProgressTracking;
}

void EnhancedLogger::logSessionStart(const std::string& sessionId) {
    if (sessionLoggingEnabled_) {
        NLM_LOG_INFO("Session started: " + sessionId);
        NLM_LOG_INFO("Session start time: " + std::to_string(sessionStartTime_));
    }
}

void EnhancedLogger::logSessionEnd(const std::string& sessionId, double duration) {
    if (sessionLoggingEnabled_) {
        NLM_LOG_INFO("Session ended: " + sessionId);
        NLM_LOG_INFO("Session duration: " + std::to_string(duration) + "s");
        NLM_LOG_INFO("Session statistics: " + getSessionStats());
    }
}

void EnhancedLogger::logProgress(const std::string& stage, double progress, const std::string& message) {
    if (progressTrackingEnabled_) {
        std::stringstream ss;
        ss << "Stage: " << stage << " | Progress: " << std::fixed << std::setprecision(1) << (progress * 100.0) << "%";
        if (!message.empty()) {
            ss << " | " << message;
        }
        NLM_LOG_INFO(ss.str());
    }
}

void EnhancedLogger::logExperiment(const std::string& name, double duration, bool success, const std::string& details) {
    if (sessionLoggingEnabled_) {
        std::stringstream ss;
        ss << "Experiment: " << name << " | Duration: " << std::fixed << std::setprecision(3) << duration << "s" <<
              " | Result: " << (success ? "SUCCESS" : "FAILED");
        if (!details.empty()) {
            ss << " | Details: " << details;
        }
        NLM_LOG_INFO(ss.str());
    }
}

std::string EnhancedLogger::getSessionStats() const {
    std::stringstream ss;
    ss << "Experiments: " << experimentLogs_.size() << " | ";
    ss << "Session duration: " << std::fixed << std::setprecision(3) << (sessionStartTime_ > 0 ? std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime_).count() : 0.0) << "s";
    return ss.str();
}

std::string EnhancedLogger::getPerformanceMetrics() const {
    std::stringstream ss;
    ss << "=== Performance Metrics ===" << std::endl;
    ss << "Total experiments: " << experimentLogs_.size() << std::endl;
    ss << "Average experiment time: " << (experimentLogs_.empty() ? 0.0 : std::accumulate(experimentTimes_.begin(), experimentTimes_.end(), 0.0) / experimentLogs_.size()) << "s" << std::endl;
    ss << "Session time: " << std::fixed << std::setprecision(3) << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime_).count() << "s" << std::endl;
    ss << "=== End Performance Metrics ===" << std::endl;
    return ss.str();
}

void EnhancedLogger::setLogFile(const std::string& filepath) {
    // Set log file would go here
    NLM_LOG_INFO("Log file set to: " + filepath);
}

} // namespace nlm
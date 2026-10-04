#include "PerformanceLogger.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

struct PerformanceLogger::Impl {
    std::shared_ptr<ILogger> targetLogger;
    LogLevel level = LogLevel::Info;
    
    // Performance tracking
    struct TimingInfo {
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point pauseTime;
        bool isRunning = false;
        bool isPaused = false;
        double totalPausedTime = 0.0;
    };
    
    std::map<std::string, TimingInfo> activeTimers;
    std::map<std::string, PerformanceMetrics> metrics;
    std::mutex mutex;
    
    // Configuration
    bool logPerformanceEntries = true;
    std::string performanceLogCategory = "performance";
};

PerformanceLogger::PerformanceLogger(std::shared_ptr<ILogger> targetLogger, 
                                     const std::string& category)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->targetLogger = targetLogger;
    pImpl->performanceLogCategory = category;
}

PerformanceLogger::~PerformanceLogger() = default;

void PerformanceLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Check if this is a performance log entry
    if (pImpl->logPerformanceEntries && entry.file.find("PerformanceLogger") != std::string::npos) {
        // This is a performance metric entry, handle separately
        return;
    }
    
    // Log performance statistics if requested
    if (!pImpl->activeTimers.empty()) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::string stats = formatPerformanceStats();
        if (!stats.empty()) {
            LogEntry perfEntry;
            perfEntry.level = LogLevel::Debug;
            perfEntry.message = "Performance Stats: " + stats;
            perfEntry.file = "";
            perfEntry.line = 0;
            perfEntry.function = "";
            perfEntry.timestamp = std::chrono::system_clock::now();
            
            pImpl->targetLogger->log(perfEntry);
        }
    }
    
    // Forward to target logger
    pImpl->targetLogger->log(entry);
}

void PerformanceLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel PerformanceLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void PerformanceLogger::startTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->activeTimers.find(operation) == pImpl->activeTimers.end()) {
        TimingInfo timer;
        timer.startTime = std::chrono::system_clock::now();
        timer.isRunning = true;
        pImpl->activeTimers[operation] = timer;
    }
}

void PerformanceLogger::stopTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->activeTimers.find(operation);
    if (it == pImpl->activeTimers.end() || !it->second.isRunning) {
        return; // No active timer
    }
    
    auto endTime = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - it->second.startTime).count() -
                     it->second.totalPausedTime;
    
    if (elapsed >= 0) {
        // Record the measurement
        if (pImpl->metrics.find(operation) == pImpl->metrics.end()) {
            pImpl->metrics[operation] = PerformanceMetrics();
        }
        pImpl->metrics[operation].addMeasurement(elapsed);
    }
    
    // Log performance if enabled
    if (pImpl->logPerformanceEntries && elapsed >= 0) {
        LogEntry perfEntry;
        perfEntry.level = LogLevel::Debug;
        perfEntry.message = "Performance: " + operation + " took " + 
                           std::to_string(elapsed) + " seconds";
        perfEntry.file = "PerformanceLogger.cpp";
        perfEntry.line = __LINE__;
        perfEntry.function = "stopTiming";
        perfEntry.timestamp = endTime;
        
        pImpl->targetLogger->log(perfEntry);
    }
    
    it->second.isRunning = false;
    it->second.isPaused = false;
}

void PerformanceLogger::pauseTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->activeTimers.find(operation);
    if (it == pImpl->activeTimers.end() || !it->second.isRunning || it->second.isPaused) {
        return;
    }
    
    it->second.isPaused = true;
    it->second.pauseTime = std::chrono::system_clock::now();
}

void PerformanceLogger::resumeTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->activeTimers.find(operation);
    if (it == pImpl->activeTimers.end() || !it->second.isPaused) {
        return;
    }
    
    auto resumeTime = std::chrono::system_clock::now();
    double pauseDuration = std::chrono::duration<double>(resumeTime - it->second.pauseTime).count();
    
    it->second.totalPausedTime += pauseDuration;
    it->second.isPaused = false;
}

double PerformanceLogger::getAverageTime(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->metrics.find(operation);
    return (it != pImpl->metrics.end()) ? it->second.getAverage() : 0.0;
}

double PerformanceLogger::getMaxTime(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->metrics.find(operation);
    return (it != pImpl->metrics.end()) ? it->second.getMaxTime() : 0.0;
}

double PerformanceLogger::getMinTime(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->metrics.find(operation);
    return (it != pImpl->metrics.end()) ? it->second.getMinTime() : 0.0;
}

size_t PerformanceLogger::getOperationCount(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->metrics.find(operation);
    return (it != pImpl->metrics.end()) ? static_cast<size_t>(it->second.count) : 0;
}

std::map<std::string, PerformanceMetrics> PerformanceLogger::getPerformanceStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->metrics;
}

std::string PerformanceLogger::generatePerformanceReport() const {
    return formatPerformanceStats();
}

void PerformanceLogger::clearStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metrics.clear();
}

bool PerformanceLogger::checkThreshold(const std::string& operation, double threshold) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->metrics.find(operation);
    if (it == pImpl->metrics.end()) {
        return false;
    }
    
    return it->second.getAverage() > threshold;
}

std::string PerformanceLogger::formatPerformanceStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->metrics.empty()) {
        return "No performance statistics available.";
    }
    
    std::ostringstream oss;
    oss << "Performance Statistics:" << std::endl;
    
    for (const auto& pair : pImpl->metrics) {
        const std::string& operation = pair.first;
        const PerformanceMetrics& metrics = pair.second;
        
        oss << "  " << operation << ":" << std::endl;
        oss << "    Count: " << metrics.count << std::endl;
        oss << "    Average: " << std::fixed << std::setprecision(4) << metrics.getAverage() << "s" << std::endl;
        oss << "    Min: " << std::fixed << std::setprecision(4) << metrics.getMinTime() << "s" << std::endl;
        oss << "    Max: " << std::fixed << std::setprecision(4) << metrics.getMaxTime() << "s" << std::endl;
        oss << "    Std Dev: " << std::fixed << std::setprecision(4) << metrics.getStandardDeviation() << "s" << std::endl;
        oss << "    Coefficient of Variation: " << std::fixed << std::setprecision(4) << metrics.getCoefficientOfVariation() << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

} // namespace nlm

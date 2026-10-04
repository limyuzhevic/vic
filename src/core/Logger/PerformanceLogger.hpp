#pragma once

#include "Logger.hpp"
#include <map>
#include <string>
#include <chrono>
#include <mutex>

namespace nlm {

// Performance tracking structures
struct PerformanceMetrics {
    double totalTime = 0.0;
    double minTime = 0.0;
    double maxTime = 0.0;
    double count = 0;
    double sumSquares = 0.0;
    
    void addMeasurement(double time) {
        if (count == 0) {
            minTime = maxTime = time;
        } else {
            minTime = std::min(minTime, time);
            maxTime = std::max(maxTime, time);
        }
        totalTime += time;
        sumSquares += time * time;
        count++;
    }
    
    double getAverage() const {
        return count > 0 ? totalTime / count : 0.0;
    }
    
    double getStandardDeviation() const {
        if (count <= 1) return 0.0;
        double mean = getAverage();
        double variance = (sumSquares - count * mean * mean) / (count - 1);
        return variance > 0 ? std::sqrt(variance) : 0.0;
    }
    
    double getCoefficientOfVariation() const {
        double mean = getAverage();
        return mean > 0 ? getStandardDeviation() / mean : 0.0;
    }
};

// Performance logger implementation
class PerformanceLogger : public ILogger {
public:
    PerformanceLogger(std::shared_ptr<ILogger> targetLogger, 
                     const std::string& category = "default");
    ~PerformanceLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Performance tracking
    void startTiming(const std::string& operation);
    void stopTiming(const std::string& operation);
    void pauseTiming(const std::string& operation);
    void resumeTiming(const std::string& operation);
    
    // Get performance statistics
    double getAverageTime(const std::string& operation) const;
    double getMaxTime(const std::string& operation) const;
    double getMinTime(const std::string& operation) const;
    size_t getOperationCount(const std::string& operation) const;
    
    // Get performance statistics
    std::map<std::string, PerformanceMetrics> getPerformanceStats() const;
    
    // Report generation
    std::string generatePerformanceReport() const;
    
    // Clear statistics
    void clearStatistics();
    
    // Threshold checking
    bool checkThreshold(const std::string& operation, double threshold) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatPerformanceStats() const;
};

} // namespace nlm

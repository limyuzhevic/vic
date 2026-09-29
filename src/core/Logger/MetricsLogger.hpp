// Metrics logger implementation - Enhanced
#pragma once

#include "Logger.hpp"
#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

class MetricsLogger : public BaseLogger {
public:
    MetricsLogger();
    explicit MetricsLogger(LogLevel level);
    ~MetricsLogger() override;
    
    void doCustomLog(const LogEntry& entry) override;
    void flush() override;
    
    // Performance metrics
    void recordMetric(const std::string& name, double value);
    void recordCounter(const std::string& name, uint64_t count = 1);
    void recordTimer(const std::string& name, std::chrono::duration<double> duration);
    
    // Metrics retrieval
    double getMetric(const std::string& name) const;
    uint64_t getCounter(const std::string& name) const;
    double getAverageTimer(const std::string& name) const;
    
    // Metrics reporting
    void reportMetrics();
    std::string getMetricsReport() const;
    
    // Performance monitoring
    uint64_t getTotalLogCount() const;
    double getSystemAverageLogTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
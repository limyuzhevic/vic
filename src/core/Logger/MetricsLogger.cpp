// Metrics logger implementation - Enhanced
#include "MetricsLogger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <mutex>

namespace nlm {

struct MetricsLogger::Impl {
    // Performance metrics
    std::unordered_map<std::string, double> metrics;
    std::unordered_map<std::string, uint64_t> counters;
    std::unordered_map<std::string, std::vector<double>> timers;
    std::mutex metricsMutex;
    
    // System performance
    std::atomic<uint64_t> totalLogCount;
    std::chrono::system_clock::time_point startTime;
    std::chrono::duration<double> totalLogTime;
};

MetricsLogger::MetricsLogger() : BaseLogger() {
    pImpl.reset(new Impl);
    pImpl->totalLogCount = 0;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->totalLogTime = std::chrono::duration<double>::zero();
}

MetricsLogger::MetricsLogger(LogLevel level) : BaseLogger(level) {
    pImpl.reset(new Impl);
    pImpl->totalLogCount = 0;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->totalLogTime = std::chrono::duration<double>::zero();
}

MetricsLogger::~MetricsLogger() {
    reportMetrics();
}

void MetricsLogger::doCustomLog(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Update system metrics
    pImpl->totalLogCount++;
    auto now = std::chrono::system_clock::now();
    pImpl->totalLogTime += std::chrono::duration<double>(now - pImpl->startTime);
    pImpl->startTime = now;
    
    // Format the log entry
    std::string formatted = getFormatter()->format(entry);
    std::cout << formatted << std::endl;
}

void MetricsLogger::flush() {
    // No buffering in this implementation
}

void MetricsLogger::recordMetric(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->metrics[name] = value;
}

void MetricsLogger::recordCounter(const std::string& name, uint64_t count) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->counters[name] += count;
}

void MetricsLogger::recordTimer(const std::string& name, std::chrono::duration<double> duration) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->timers[name].push_back(duration.count());
}

void MetricsLogger::reportMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    std::cout << "=== Performance Metrics ===" << std::endl;
    std::cout << "Total log count: " << pImpl->totalLogCount << std::endl;
    std::cout << "Average log time: " << (pImpl->totalLogCount > 0 ? 
        pImpl->totalLogTime.count() / pImpl->totalLogCount : 0.0) << " seconds" << std::endl;
    
    if (!pImpl->metrics.empty()) {
        std::cout << "\nCustom metrics:" << std::endl;
        for (const auto& pair : pImpl->metrics) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    if (!pImpl->counters.empty()) {
        std::cout << "\nCounters:" << std::endl;
        for (const auto& pair : pImpl->counters) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    if (!pImpl->timers.empty()) {
        std::cout << "\nTimers:" << std::endl;
        for (const auto& pair : pImpl->timers) {
            double sum = std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
            double avg = pair.second.size() > 0 ? sum / pair.second.size() : 0.0;
            std::cout << "  " << pair.first << ": " << avg << " seconds (" << 
                pair.second.size() << " samples)" << std::endl;
        }
    }
}

std::string MetricsLogger::getMetricsReport() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    std::ostringstream oss;
    oss << "=== Performance Metrics ===" << std::endl;
    oss << "Total log count: " << pImpl->totalLogCount << std::endl;
    oss << "Average log time: " << (pImpl->totalLogCount > 0 ? 
        pImpl->totalLogTime.count() / pImpl->totalLogCount : 0.0) << " seconds" << std::endl;
    
    if (!pImpl->metrics.empty()) {
        oss << "\nCustom metrics:" << std::endl;
        for (const auto& pair : pImpl->metrics) {
            oss << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    if (!pImpl->counters.empty()) {
        oss << "\nCounters:" << std::endl;
        for (const auto& pair : pImpl->counters) {
            oss << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    if (!pImpl->timers.empty()) {
        oss << "\nTimers:" << std::endl;
        for (const auto& pair : pImpl->timers) {
            double sum = std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
            double avg = pair.second.size() > 0 ? sum / pair.second.size() : 0.0;
            oss << "  " << pair.first << ": " << avg << " seconds (" << 
                pair.second.size() << " samples)" << std::endl;
        }
    }
    
    return oss.str();
}

double MetricsLogger::getMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    auto it = pImpl->metrics.find(name);
    if (it != pImpl->metrics.end()) {
        return it->second;
    }
    return 0.0;
}

uint64_t MetricsLogger::getCounter(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    auto it = pImpl->counters.find(name);
    if (it != pImpl->counters.end()) {
        return it->second;
    }
    return 0;
}

double MetricsLogger::getAverageTimer(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    auto it = pImpl->timers.find(name);
    if (it != pImpl->timers.end() && !it->second.empty()) {
        double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
        return sum / it->second.size();
    }
    return 0.0;
}

uint64_t MetricsLogger::getTotalLogCount() const {
    return pImpl->totalLogCount.load();
}

double MetricsLogger::getSystemAverageLogTime() const {
    auto totalCount = getTotalLogCount();
    if (totalCount == 0) return 0.0;
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->totalLogTime.count() / totalCount;
}

} // namespace nlm
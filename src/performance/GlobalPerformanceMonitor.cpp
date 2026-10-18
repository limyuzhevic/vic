#include "PerformanceMonitor.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <ctime>
#include <random>
#include <algorithm>

namespace nlm {

PerformanceMonitor& getGlobalPerformanceMonitor() {
    static PerformanceMonitor globalMonitor;
    return globalMonitor;
}

// Convenience functions
void startPerformanceMonitoring() {
    getGlobalPerformanceMonitor().initialize(PerformanceMonitor::MonitorMode::NORMAL);
}

void stopPerformanceMonitoring() {
    getGlobalPerformanceMonitor().shutdown();
}

void logPerformanceSample() {
    // Log a performance sample
    auto& monitor = getGlobalPerformanceMonitor();
    auto& realTimeMonitor = monitor.getRealTimeMonitor();
    realTimeMonitor.samplePerformance();
}

PerformanceMetrics getCurrentPerformanceMetrics() {
    return getGlobalPerformanceMonitor().getRealTimeMonitor().getCurrentMetrics();
}

void addPerformanceAlert(AlertType type, const std::string& message, double value, double threshold) {
    auto& monitor = getGlobalPerformanceMonitor();
    auto& realTimeMonitor = monitor.getRealTimeMonitor();
    
    PerformanceAlert alert;
    alert.type = type;
    alert.message = message;
    alert.value = value;
    alert.threshold = threshold;
    alert.timestamp = std::chrono::system_clock::now();
    alert.source = "application";
    
    // Add to alerts
    std::lock_guard<std::mutex> lock(realTimeMonitor.pImpl->dataMutex);
    realTimeMonitor.pImpl->alerts.push_back(alert);
    
    // Notify callbacks
    for (const auto& callback : realTimeMonitor.pImpl->alertCallbacks) {
        callback(alert);
    }
}

} // namespace nlm

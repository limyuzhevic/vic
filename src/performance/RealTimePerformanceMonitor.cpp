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

namespace nlm {

// RealTimePerformanceMonitor implementation
struct RealTimePerformanceMonitor::Impl {
    std::atomic<bool> monitoring{false};
    std::atomic<bool> paused{false};
    double samplingInterval = 1.0; // seconds
    std::thread monitoringThread;
    std::mutex dataMutex;
    std::condition_variable dataCondition;
    
    std::vector<PerformanceSample> samples;
    std::vector<PerformanceAlert> alerts;
    std::vector<AlertCallback> alertCallbacks;
    
    // Thresholds
    double cpuThreshold = 80.0;      // %
    double memoryThreshold = 80.0;   // %
    double simulationSpeedThreshold = 1.0; // simulation steps per second
    
    // Alert enabled flags
    std::map<AlertType, bool> alertEnabled;
    
    // Current metrics
    PerformanceMetrics currentMetrics;
    
    // Session management
    std::string sessionId;
    std::chrono::system_clock::time_point sessionStart;
    
    // Resource monitoring
    std::thread resourceMonitoringThread;
    std::atomic<bool> resourceMonitoring{false};
};

RealTimePerformanceMonitor::RealTimePerformanceMonitor() : pImpl(std::make_unique<Impl>()) {
    // Generate unique session ID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);
    pImpl->sessionId = "session_" + std::to_string(dis(gen)) + "_" + 
                      std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    pImpl->sessionStart = std::chrono::system_clock::now();
    
    // Initialize alert enabled flags
    pImpl->alertEnabled[AlertType::CPU_USAGE_HIGH] = true;
    pImpl->alertEnabled[AlertType::MEMORY_USAGE_HIGH] = true;
    pImpl->alertEnabled[AlertType::SIMULATION_SLOW] = true;
    pImpl->alertEnabled[AlertType::EVENT_RATE_HIGH] = true;
    pImpl->alertEnabled[AlertType::EVENT_RATE_LOW] = true;
    pImpl->alertEnabled[AlertType::CIRCUIT_OVERLOADED] = true;
    pImpl->alertEnabled[AlertType::NEURON_SLOWDOWN] = true;
    pImpl->alertEnabled[AlertType::SYNAPSE_BACKLOG] = true;
    pImpl->alertEnabled[AlertType::CUSTOM] = false;
}

RealTimePerformanceMonitor::~RealTimePerformanceMonitor() {
    stopMonitoring();
}

void RealTimePerformanceMonitor::startMonitoring() {
    if (pImpl->monitoring) return;
    
    pImpl->monitoring = true;
    pImpl->paused = false;
    
    // Start monitoring thread
    pImpl->monitoringThread = std::thread([this]() {
        while (pImpl->monitoring && !pImpl->paused) {
            try {
                samplePerformance();
                std::this_thread::sleep_for(std::chrono::duration<double>(pImpl->samplingInterval));
            } catch (const std::exception& e) {
                // Log error but continue monitoring
                std::cerr << "Performance monitoring error: " << e.what() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    });
}

void RealTimePerformanceMonitor::stopMonitoring() {
    if (pImpl->monitoring) {
        pImpl->monitoring = false;
        if (pImpl->monitoringThread.joinable()) {
            pImpl->monitoringThread.join();
        }
    }
}

void RealTimePerformanceMonitor::pauseMonitoring() {
    pImpl->paused = true;
}

void RealTimePerformanceMonitor::resumeMonitoring() {
    pImpl->paused = false;
}

void RealTimePerformanceMonitor::samplePerformance() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    PerformanceSample sample;
    sample.timestamp = std::chrono::system_clock::now();
    sample.sessionId = pImpl->sessionId;
    
    // Get current system time
    auto now = std::chrono::system_clock::now();
    double elapsedSeconds = std::chrono::duration<double>(now - pImpl->sessionStart).count();
    
    // Update metrics
    pImpl->currentMetrics.realTime = elapsedSeconds;
    pImpl->currentMetrics.simulationTime = elapsedSeconds * pImpl->currentMetrics.simulationSpeed;
    
    sample.metrics = pImpl->currentMetrics;
    samples.push_back(sample);
    
    // Keep only recent samples (last 10000)
    if (samples.size() > 10000) {
        samples.erase(samples.begin(), samples.begin() + (samples.size() - 10000));
    }
    
    // Check for alerts
    checkForAlerts(sample.metrics);
}

PerformanceSample RealTimePerformanceMonitor::getCurrentSample() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    if (samples.empty()) {
        return PerformanceSample();
    }
    return samples.back();
}

void RealTimePerformanceMonitor::addAlertCallback(AlertCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->alertCallbacks.push_back(callback);
}

void RealTimePerformanceMonitor::removeAlertCallback(AlertCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->alertCallbacks.erase(std::remove(pImpl->alertCallbacks.begin(), 
                                           pImpl->alertCallbacks.end(), callback),
                                pImpl->alertCallbacks.end());
}

void RealTimePerformanceMonitor::setThreshold(AlertType type, double threshold) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    switch (type) {
        case AlertType::CPU_USAGE_HIGH:
            pImpl->cpuThreshold = threshold;
            break;
        case AlertType::MEMORY_USAGE_HIGH:
            pImpl->memoryThreshold = threshold;
            break;
        case AlertType::SIMULATION_SLOW:
            pImpl->simulationSpeedThreshold = threshold;
            break;
        default:
            // For other alert types, store in a map
            break;
    }
}

void RealTimePerformanceMonitor::setAlertEnabled(AlertType type, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->alertEnabled[type] = enabled;
}

std::vector<PerformanceAlert> RealTimePerformanceMonitor::getRecentAlerts(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    size_t start = std::max<size_t>(0, pImpl->alerts.size() - count);
    return std::vector<PerformanceAlert>(pImpl->alerts.begin() + start, pImpl->alerts.end());
}

void RealTimePerformanceMonitor::setSamplingInterval(double intervalSeconds) {
    pImpl->samplingInterval = intervalSeconds;
}

void RealTimePerformanceMonitor::setCpuThreshold(double threshold) {
    setThreshold(AlertType::CPU_USAGE_HIGH, threshold);
}

void RealTimePerformanceMonitor::setMemoryThreshold(double threshold) {
    setThreshold(AlertType::MEMORY_USAGE_HIGH, threshold);
}

void RealTimePerformanceMonitor::setSimulationSpeedThreshold(double threshold) {
    setThreshold(AlertType::SIMULATION_SLOW, threshold);
}

PerformanceMetrics RealTimePerformanceMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->currentMetrics;
}

std::vector<PerformanceSample> RealTimePerformanceMonitor::getRecentSamples(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    size_t start = std::max<size_t>(0, samples.size() - count);
    return std::vector<PerformanceSample>(samples.begin() + start, samples.end());
}

std::vector<PerformanceSample> RealTimePerformanceMonitor::getSamplesInTimeRange(
    std::chrono::system_clock::time_point start,
    std::chrono::system_clock::time_point end) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<PerformanceSample> result;
    for (const auto& sample : samples) {
        if (sample.timestamp >= start && sample.timestamp <= end) {
            result.push_back(sample);
        }
    }
    return result;
}

double RealTimePerformanceMonitor::getAverageCpuUsage(std::chrono::seconds duration) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    auto now = std::chrono::system_clock::now();
    auto start = now - duration;
    
    double totalCpu = 0.0;
    size_t count = 0;
    
    for (const auto& sample : samples) {
        if (sample.timestamp >= start && sample.timestamp <= now) {
            totalCpu += sample.metrics.cpuUsage;
            count++;
        }
    }
    
    return count > 0 ? totalCpu / count : 0.0;
}

// Implementation continues...

// Helper function to get current session ID
std::string RealTimePerformanceMonitor::getCurrentSessionId() {
    // This would need to access a global monitor instance
    // For now, return a placeholder
    return "session_000000";
}

// Performance alert helper
void RealTimePerformanceMonitor::checkForAlerts(const PerformanceMetrics& metrics) {
    // Check each alert type
    if (pImpl->alertEnabled[AlertType::CPU_USAGE_HIGH] && metrics.cpuUsage > pImpl->cpuThreshold) {
        PerformanceAlert alert;
        alert.type = AlertType::CPU_USAGE_HIGH;
        alert.message = "High CPU usage: " + std::to_string(metrics.cpuUsage) + "% (threshold: " + 
                       std::to_string(pImpl->cpuThreshold) + "%)";
        alert.value = metrics.cpuUsage;
        alert.threshold = pImpl->cpuThreshold;
        alert.timestamp = std::chrono::system_clock::now();
        alert.source = "system";
        
        pImpl->alerts.push_back(alert);
        
        // Notify callbacks
        for (const auto& callback : pImpl->alertCallbacks) {
            callback(alert);
        }
    }
    
    if (pImpl->alertEnabled[AlertType::MEMORY_USAGE_HIGH] && metrics.memoryPercent > pImpl->memoryThreshold) {
        PerformanceAlert alert;
        alert.type = AlertType::MEMORY_USAGE_HIGH;
        alert.message = "High memory usage: " + std::to_string(metrics.memoryPercent) + "% (threshold: " + 
                       std::to_string(pImpl->memoryThreshold) + "%)";
        alert.value = metrics.memoryPercent;
        alert.threshold = pImpl->memoryThreshold;
        alert.timestamp = std::chrono::system_clock::now();
        alert.source = "system";
        
        pImpl->alerts.push_back(alert);
        
        // Notify callbacks
        for (const auto& callback : pImpl->alertCallbacks) {
            callback(alert);
        }
    }
    
    if (pImpl->alertEnabled[AlertType::SIMULATION_SLOW] && metrics.eventRate < pImpl->simulationSpeedThreshold) {
        PerformanceAlert alert;
        alert.type = AlertType::SIMULATION_SLOW;
        alert.message = "Simulation running slow: " + std::to_string(metrics.eventRate) + " events/sec (threshold: " + 
                       std::to_string(pImpl->simulationSpeedThreshold) + " events/sec)";
        alert.value = metrics.eventRate;
        alert.threshold = pImpl->simulationSpeedThreshold;
        alert.timestamp = std::chrono::system_clock::now();
        alert.source = "simulation";
        
        pImpl->alerts.push_back(alert);
        
        // Notify callbacks
        for (const auto& callback : pImpl->alertCallbacks) {
            callback(alert);
        }
    }
}

std::string RealTimePerformanceMonitor::formatDuration(std::chrono::duration<double> duration) {
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours.count() << ":"
       << std::setfill('0') << std::setw(2) << minutes.count() << ":"
       << std::setfill('0') << std::setw(2) << seconds.count() << "."
       << std::setfill('0') << std::setw(3) << milliseconds.count();
    
    return ss.str();
}

std::string RealTimePerformanceMonitor::formatMemorySize(double bytes) {
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int suffix = 0;
    double size = bytes;
    
    while (size >= 1024.0 && suffix < 4) {
        size /= 1024.0;
        suffix++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << suffixes[suffix];
    return ss.str();
}

std::string RealTimePerformanceMonitor::formatPercentage(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value << "%";
    return ss.str();
}

} // namespace nlm

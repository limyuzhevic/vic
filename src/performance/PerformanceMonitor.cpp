#include "PerformanceMonitor.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}

void PerformanceMonitor::startMonitoring(size_t samplingInterval) {
    monitoring_ = true;
    samplingInterval_ = samplingInterval;
    lastSampleTime_ = 0;
}

void PerformanceMonitor::stopMonitoring() {
    monitoring_ = false;
}

PerformanceMonitor::SystemMetrics PerformanceMonitor::getSystemMetrics() const {
    SystemMetrics metrics;
    metrics.cpuUsage = 0.0; // Placeholder
    metrics.memoryUsage = 0.0; // Placeholder
    metrics.diskUsage = 0.0; // Placeholder
    metrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
    return metrics;
}

PerformanceMonitor::NeuralMetrics PerformanceMonitor::getNeuralMetrics() const {
    NeuralMetrics metrics;
    metrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
    return metrics;
}

void PerformanceMonitor::recordSystemSnapshot() {
    if (!monitoring_) return;
    
    SystemMetrics metrics = getSystemMetrics();
    pImpl->systemHistory.push_back(metrics);
}

void PerformanceMonitor::recordNeuralSnapshot(std::shared_ptr<Brain> brain) {
    if (!monitoring_) return;
    
    NeuralMetrics metrics = getNeuralMetrics();
    // TODO: Extract actual metrics from brain
    pImpl->neuralHistory.push_back(metrics);
}

double PerformanceMonitor::getAverageCpuUsage() const {
    if (pImpl->systemHistory.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& metrics : pImpl->systemHistory) {
        total += metrics.cpuUsage;
    }
    return total / pImpl->systemHistory.size();
}

double PerformanceMonitor::getMaxCpuUsage() const {
    if (pImpl->systemHistory.empty()) return 0.0;
    
    double max = 0.0;
    for (const auto& metrics : pImpl->systemHistory) {
        max = std::max(max, metrics.cpuUsage);
    }
    return max;
}

double PerformanceMonitor::getAverageMemoryUsage() const {
    if (pImpl->systemHistory.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& metrics : pImpl->systemHistory) {
        total += metrics.memoryUsage;
    }
    return total / pImpl->systemHistory.size();
}

double PerformanceMonitor::getMaxMemoryUsage() const {
    if (pImpl->systemHistory.empty()) return 0.0;
    
    double max = 0.0;
    for (const auto& metrics : pImpl->systemHistory) {
        max = std::max(max, metrics.memoryUsage);
    }
    return max;
}

std::string PerformanceMonitor::generatePerformanceReport() const {
    std::ostringstream oss;
    oss << "NLM Performance Monitoring Report" << std::endl;
    oss << std::endl;
    
    oss << "System Performance:" << std::endl;
    oss << "  Average CPU usage: " << getAverageCpuUsage() << "%" << std::endl;
    oss << "  Max CPU usage: " << getMaxCpuUsage() << "%" << std::endl;
    oss << "  Average memory usage: " << getAverageMemoryUsage() << " MB" << std::endl;
    oss << "  Max memory usage: " << getMaxMemoryUsage() << " MB" << std::endl;
    oss << "  Total samples: " << pImpl->systemHistory.size() << std::endl;
    oss << std::endl;
    
    oss << "Neural Performance:" << std::endl;
    oss << "  Neural samples: " << pImpl->neuralHistory.size() << std::endl;
    
    return oss.str();
}

std::string PerformanceMonitor::exportMonitoringData() const {
    std::ostringstream oss;
    oss << "NLM Performance Monitoring Data" << std::endl;
    oss << std::endl;
    
    oss << "System Metrics:" << std::endl;
    for (const auto& metrics : pImpl->systemHistory) {
        oss << "  " << metrics.timestamp << ": "
            << "CPU: " << metrics.cpuUsage << "%, "
            << "Memory: " << metrics.memoryUsage << " MB" << std::endl;
    }
    
    oss << std::endl << "Neural Metrics:" << std::endl;
    for (const auto& metrics : pImpl->neuralHistory) {
        oss << "  " << metrics.timestamp << ": "
            << "Neurons: " << metrics.totalNeurons << ", "
            << "Spikes: " << metrics.totalSpikeCount << ", "
            << "Firing rate: " << metrics.averageFiringRate << std::endl;
    }
    
    return oss.str();
}

void PerformanceMonitor::exportMonitoringToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << exportMonitoringData();
        file.close();
    }
}

void PerformanceMonitor::clearHistory() {
    pImpl->systemHistory.clear();
    pImpl->neuralHistory.clear();
}

} // namespace nlm

#include "PerformanceMonitor.hpp"
#include "../core/Config/Config.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <thread>
#include <sys/types.h>
#include <unistd.h>

namespace nlm {

PerformanceMonitor::PerformanceMonitor()
    : memoryThreshold(0), eventThreshold(0), cpuThreshold(0.0)
    , alertCount(0), maxHistorySize(1000)
    , monitoringInterval(std::chrono::milliseconds(100)) // Update every 100ms
{
    lastUpdateTime = std::chrono::steady_clock::now();
    
    // Initialize metrics with zeros
    currentMetrics.memory = {0, 0, 0, 0, 0, 0, 0.0};
    currentMetrics.event = {0, 0, 0, 0.0, 0, 0.0};
    currentMetrics.cpu = {0.0, 0.0, 0.0, 0};
    currentMetrics.simulation = {0, 0.0, 0, 0, 0.0, 0.0};
    currentMetrics.systemLoad = 0.0;
    currentMetrics.networkLatency = 0.0;
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::update(float timestep) {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime);
    
    if (elapsed >= monitoringInterval) {
        // Record current metrics
        recordMemoryStats();
        recordEventStats();
        recordCpuStats();
        
        // Update time series
        metricHistory.push_back({now, currentMetrics});
        if (metricHistory.size() > maxHistorySize) {
            metricHistory.erase(metricHistory.begin());
        }
        
        // Check thresholds
        if (checkThresholds()) {
            generateAlerts("performance", 0.0); // Dummy value, will show type in alert
        }
        
        lastUpdateTime = now;
    }
}

void PerformanceMonitor::recordMemoryStats() {
    // This would integrate with actual MemoryPool implementation
    // For now, we'll use placeholder values
    currentMetrics.memory.allocatedNeurons = 0;
    currentMetrics.memory.freeNeurons = 0;
    currentMetrics.memory.allocatedSynapses = 0;
    currentMetrics.memory.freeSynapses = 0;
    currentMetrics.memory.memoryPoolHits = 0;
    currentMetrics.memory.memoryPoolMisses = 0;
    currentMetrics.memory.memoryAllocationTime = 0.0;
}

void PerformanceMonitor::recordEventStats() {
    // This would integrate with EventQueue
    // For now, we'll use placeholder values
    currentMetrics.event.queueSize = 0;
    currentMetrics.event.overflowCount = 0;
    currentMetrics.event.undercountCount = 0;
    currentMetrics.event.spikeProcessingLatency = 0.0;
    currentMetrics.event.totalSpikeEvents = 0;
    currentMetrics.event.averageProcessingTime = 0.0;
}

void PerformanceMonitor::recordCpuStats() {
    // Get system CPU usage
    currentMetrics.cpu.cpuUsage = 0.0;
    currentMetrics.cpu.memoryUsage = 0.0;
    currentMetrics.cpu.peakMemoryUsage = 0.0;
    currentMetrics.cpu.activeThreads = std::thread::hardware_concurrency();
    currentMetrics.systemLoad = 0.0;
}

void PerformanceMonitor::recordSimulationStats(uint64_t step, Timestamp time, 
                                                size_t activeNeurons, size_t totalSpikes) {
    currentMetrics.simulation.currentStep = step;
    currentMetrics.simulation.currentTime = time;
    currentMetrics.simulation.activeNeurons = activeNeurons;
    currentMetrics.simulation.totalSpikes = totalSpikes;
    
    if (step > 0) {
        currentMetrics.simulation.averageFiringRate = 
            static_cast<double>(totalSpikes) / static_cast<double>(step);
        currentMetrics.simulation.maxFiringRate = 
            currentMetrics.simulation.averageFiringRate * 1.1; // Placeholder
    }
}

void PerformanceMonitor::onMemoryAllocation(size_t size, bool hit) {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    if (hit) {
        currentMetrics.memory.memoryPoolHits++;
    } else {
        currentMetrics.memory.memoryPoolMisses++;
    }
}

void PerformanceMonitor::onMemoryFree(size_t size) {
    std::lock_guard<std::mutex> lock(metricsMutex);
    // Could track freed memory for fragment analysis
}

void PerformanceMonitor::onEventProcessed(double latency) {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    currentMetrics.event.totalSpikeEvents++;
    currentMetrics.event.spikeProcessingLatency = 
        (currentMetrics.event.spikeProcessingLatency * (currentMetrics.event.totalSpikeEvents - 1) + latency) /
        currentMetrics.event.totalSpikeEvents;
}

void PerformanceMonitor::onEventOverflow() {
    std::lock_guard<std::mutex> lock(metricsMutex);
    currentMetrics.event.overflowCount++;
}

void PerformanceMonitor::onEventUndercount() {
    std::lock_guard<std::mutex> lock(metricsMutex);
    currentMetrics.event.undercountCount++;
}

PerformanceMetrics PerformanceMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    return currentMetrics;
}

std::string PerformanceMonitor::getPerformanceReport() const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    std::stringstream ss;
    
    ss << "=== NLM Performance Report ===" << std::endl;
    ss << std::endl;
    
    // Memory stats
    ss << "Memory Statistics:" << std::endl;
    ss << "  Allocated Neurons: " << currentMetrics.memory.allocatedNeurons << std::endl;
    ss << "  Free Neurons: " << currentMetrics.memory.freeNeurons << std::endl;
    ss << "  Allocated Synapses: " << currentMetrics.memory.allocatedSynapses << std::endl;
    ss << "  Free Synapses: " << currentMetrics.memory.freeSynapses << std::endl;
    ss << "  Memory Pool Hits: " << currentMetrics.memory.memoryPoolHits << std::endl;
    ss << "  Memory Pool Misses: " << currentMetrics.memory.memoryPoolMisses << std::endl;
    if (currentMetrics.memory.memoryPoolHits + currentMetrics.memory.memoryPoolMisses > 0) {
        double hitRate = static_cast<double>(currentMetrics.memory.memoryPoolHits) / 
                       (currentMetrics.memory.memoryPoolHits + currentMetrics.memory.memoryPoolMisses);
        ss << "  Hit Rate: " << std::fixed << std::setprecision(2) << (hitRate * 100) << "%" << std::endl;
    }
    ss << std::endl;
    
    // Event stats
    ss << "Event Processing Statistics:" << std::endl;
    ss << "  Queue Size: " << currentMetrics.event.queueSize << std::endl;
    ss << "  Overflow Count: " << currentMetrics.event.overflowCount << std::endl;
    ss << "  Processing Latency: " << std::fixed << std::setprecision(4) 
       << currentMetrics.event.spikeProcessingLatency << " ms" << std::endl;
    ss << "  Total Events: " << currentMetrics.event.totalSpikeEvents << std::endl;
    ss << "  Average Processing Time: " << std::fixed << std::setprecision(4) 
       << currentMetrics.event.averageProcessingTime << " ms" << std::endl;
    ss << std::endl;
    
    // CPU stats
    ss << "System Resources:" << std::endl;
    ss << "  CPU Usage: " << std::fixed << std::setprecision(1) 
       << (currentMetrics.cpu.cpuUsage * 100) << "%" << std::endl;
    ss << "  Memory Usage: " << currentMetrics.cpu.memoryUsage << " MB" << std::endl;
    ss << "  Peak Memory: " << currentMetrics.cpu.peakMemoryUsage << " MB" << std::endl;
    ss << "  Active Threads: " << currentMetrics.cpu.activeThreads << std::endl;
    ss << std::endl;
    
    // Simulation stats
    ss << "Simulation Statistics:" << std::endl;
    ss << "  Current Step: " << currentMetrics.simulation.currentStep << std::endl;
    ss << "  Current Time: " << std::fixed << std::setprecision(4) 
       << currentMetrics.simulation.currentTime << " s" << std::endl;
    ss << "  Active Neurons: " << currentMetrics.simulation.activeNeurons << std::endl;
    ss << "  Total Spikes: " << currentMetrics.simulation.totalSpikes << std::endl;
    ss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) 
       << currentMetrics.simulation.averageFiringRate << " Hz" << std::endl;
    ss << "  Max Firing Rate: " << std::fixed << std::setprecision(2) 
       << currentMetrics.simulation.maxFiringRate << " Hz" << std::endl;
    ss << std::endl;
    
    // Additional metrics
    ss << "Advanced Metrics:" << std::endl;
    ss << "  System Load: " << std::fixed << std::setprecision(2) 
       << currentMetrics.systemLoad << std::endl;
    ss << "  Network Latency: " << std::fixed << std::setprecision(4) 
       << currentMetrics.networkLatency << " ms" << std::endl;
    ss << std::endl;
    
    // Alerts
    if (alertCount > 0) {
        ss << "Alerts: " << alertCount << " performance alerts triggered" << std::endl;
    }
    
    std::stringstream ss;
    
    ss << "=== Performance Report ===" << std::endl;
    ss << "Timestamp: " << getTimestamp() << std::endl;
    ss << "Monitoring Interval: " << monitoringInterval.count() << "ms" << std::endl;
    ss << "Alert Count: " << alertCount << std::endl;
    ss << std::endl;
    
    // Memory statistics
    ss << "=== Memory Statistics ===" << std::endl;
    ss << "  Allocated Neurons: " << currentMetrics.memory.allocatedNeurons << std::endl;
    ss << "  Free Neurons: " << currentMetrics.memory.freeNeurons << std::endl;
    ss << "  Available Neurons: " << currentMetrics.memory.allocatedNeurons + currentMetrics.memory.freeNeurons << std::endl;
    ss << "  Memory Pool Hits: " << currentMetrics.memory.memoryPoolHits << std::endl;
    ss << "  Memory Pool Misses: " << currentMetrics.memory.memoryPoolMisses << std::endl;
    ss << std::endl;
    
    // Event statistics
    ss << "=== Event Statistics ===" << std::endl;
    ss << "  Queue Size: " << currentMetrics.event.queueSize << std::endl;
    ss << "  Overflow Count: " << currentMetrics.event.overflowCount << std::endl;
    ss << "  Processing Latency: " << std::fixed << std::setprecision(4) 
       << currentMetrics.event.spikeProcessingLatency << " ms" << std::endl;
    ss << "  Total Events: " << currentMetrics.event.totalSpikeEvents << std::endl;
    ss << std::endl;
    
    // CPU statistics
    ss << "=== CPU Statistics ===" << std::endl;
    ss << "  CPU Usage: " << std::fixed << std::setprecision(1) 
       << (currentMetrics.cpu.cpuUsage * 100) << "%" << std::endl;
    ss << "  Memory Usage: " << currentMetrics.cpu.memoryUsage << " MB" << std::endl;
    ss << "  Peak Memory: " << currentMetrics.cpu.peakMemoryUsage << " MB" << std::endl;
    ss << "  Active Threads: " << currentMetrics.cpu.activeThreads << std::endl;
    ss << std::endl;
    
    // Simulation statistics
    ss << "=== Simulation Statistics ===" << std::endl;
    ss << "  Current Step: " << currentMetrics.simulation.currentStep << std::endl;
    ss << "  Current Time: " << std::fixed << std::setprecision(4) 
       << currentMetrics.simulation.currentTime << " s" << std::endl;
    ss << "  Active Neurons: " << currentMetrics.simulation.activeNeurons << std::endl;
    ss << "  Total Spikes: " << currentMetrics.simulation.totalSpikes << std::endl;
    ss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) 
       << currentMetrics.simulation.averageFiringRate << " Hz" << std::endl;
    ss << "  Max Firing Rate: " << std::fixed << std::setprecision(2) 
       << currentMetrics.simulation.maxFiringRate << " Hz" << std::endl;
    ss << std::endl;
    
    // Additional metrics
    ss << "=== Additional Metrics ===" << std::endl;
    ss << "  System Load: " << std::fixed << std::setprecision(2) 
       << currentMetrics.systemLoad << std::endl;
    ss << "  Network Latency: " << std::fixed << std::setprecision(4) 
       << currentMetrics.networkLatency << " ms" << std::endl;
    ss << std::endl;
    
    // Performance trends
    if (!metricHistory.empty()) {
        ss << "=== Performance Trends ===" << std::endl;
        double latestMem = static_cast<double>(currentMetrics.memory.allocatedNeurons);
        double latestCpu = currentMetrics.cpu.cpuUsage;
        
        if (metricHistory.size() >= 2) {
            double previousMem = static_cast<double>(metricHistory.back().metrics.memory.allocatedNeurons);
            double previousCpu = metricHistory.back().metrics.cpu.cpuUsage;
            
            double memTrend = calculatePercentageChange(previousMem, latestMem);
            double cpuTrend = calculatePercentageChange(previousCpu, latestCpu);
            
            ss << "  Memory Usage Trend: " << std::fixed << std::setprecision(1) << memTrend << "% " 
               << (memTrend > 0 ? "(increasing)" : "(decreasing)") << std::endl;
            ss << "  CPU Usage Trend: " << std::fixed << std::setprecision(1) << cpuTrend << "% " 
               << (cpuTrend > 0 ? "(increasing)" : "(decreasing)") << std::endl;
        }
        ss << std::endl;
    }
    
    // Alerts
    if (alertCount > 0) {
        ss << "=== Active Alerts ===" << std::endl;
        ss << "  Total Alerts: " << alertCount << std::endl;
        ss << "  (Alerts were generated during monitoring)" << std::endl;
        ss << std::endl;
    }
    
    ss << "=== End Report ===" << std::endl;
    
    return ss.str();
}

std::string PerformanceMonitor::getDetailedReport(const std::string& format) const {
    if (format == "json") {
        return generateJsonReport();
    } else if (format == "csv") {
        return generateCsvReport();
    } else {
        return generateTextReport();
    }
}

std::string PerformanceMonitor::generateJsonReport() const {
    std::stringstream ss;
    ss << "{" << std::endl;
    
    ss << "  \"timestamp\": \"" << getTimestamp() << "\",\"" << std::endl;
    ss << "  \"monitoring_interval_ms\": " << monitoringInterval.count() << ",\"" << std::endl;
    ss << "  \"alert_count\": " << alertCount << ",\"" << std::endl;
    
    // Performance metrics
    ss << "  \"performance_metrics\": {" << std::endl;
    ss << "    \"simulation\": {" << std::endl;
    ss << "      \"current_step\": " << currentMetrics.simulation.currentStep << ",\"" << std::endl;
    ss << "      \"current_time\": " << currentMetrics.simulation.currentTime << ",\"" << std::endl;
    ss << "      \"active_neurons\": " << currentMetrics.simulation.activeNeurons << ",\"" << std::endl;
    ss << "      \"total_spikes\": " << currentMetrics.simulation.totalSpikes << ",\"" << std::endl;
    ss << "      \"average_firing_rate\": " << currentMetrics.simulation.averageFiringRate << ",\"" << std::endl;
    ss << "    },\"" << std::endl;
    
    ss << "  \"memory_metrics\": {" << std::endl;
    ss << "    \"allocated_neurons\": " << currentMetrics.memory.allocatedNeurons << ",\"" << std::endl;
    ss << "    \"free_neurons\": " << currentMetrics.memory.freeNeurons << ",\"" << std::endl;
    ss << "  },\"" << std::endl;
    
    ss << "  \"event_metrics\": {" << std::endl;
    ss << "    \"queue_size\": " << currentMetrics.event.queueSize << ",\"" << std::endl;
    ss << "    \"overflow_count\": " << currentMetrics.event.overflowCount << ",\"" << std::endl;
    ss << "  }" << std::endl;
    
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string PerformanceMonitor::generateCsvReport() const {
    std::stringstream ss;
    ss << "timestamp,monitoring_interval_ms,alert_count,current_step,current_time,active_neurons,total_spikes,average_firing_rate,memory_usage,event_queue_size,cpu_usage" << std::endl;
    
    ss << getTimestamp() << ","
       << monitoringInterval.count() << ","
       << alertCount << ","
       << currentMetrics.simulation.currentStep << ","
       << currentMetrics.simulation.currentTime << ","
       << currentMetrics.simulation.activeNeurons << ","
       << currentMetrics.simulation.totalSpikes << ","
       << currentMetrics.simulation.averageFiringRate << ","
       << currentMetrics.cpu.memoryUsage << ","
       << currentMetrics.event.queueSize << ","
       << currentMetrics.cpu.cpuUsage << std::endl;
    
    return ss.str();
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(metricsMutex);
    currentMetrics = PerformanceMetrics();
    metricHistory.clear();
    alertCount = 0;
}

void PerformanceMonitor::setMemoryThreshold(size_t threshold) {
    memoryThreshold = threshold;
}

void PerformanceMonitor::setEventThreshold(size_t threshold) {
    eventThreshold = threshold;
}

void PerformanceMonitor::setCpuThreshold(double threshold) {
    cpuThreshold = threshold;
}

size_t PerformanceMonitor::getAlertCount() const {
    std::lock_guard<std::mutex> lock(metricsMutex);
    return alertCount;
}

std::chrono::milliseconds PerformanceMonitor::getMonitoringInterval() const {
    return monitoringInterval;
}

double PerformanceMonitor::calculatePercentageChange(double previous, double current) const {
    if (previous == 0.0) {
        return current * 100.0; // Treat as 100% increase from zero
    }
    return ((current - previous) / previous) * 100.0;
}

bool PerformanceMonitor::checkThresholds() const {
    bool thresholdExceeded = false;
    
    // Check memory threshold (if set)
    if (memoryThreshold > 0) {
        size_t totalMemory = currentMetrics.memory.allocatedNeurons + currentMetrics.memory.freeNeurons;
        if (totalMemory > memoryThreshold) {
            thresholdExceeded = true;
            generateAlerts("memory", static_cast<double>(totalMemory));
        }
    }
    
    // Check event threshold (if set)
    if (eventThreshold > 0 && currentMetrics.event.queueSize > eventThreshold) {
        thresholdExceeded = true;
        generateAlerts("event_queue", static_cast<double>(currentMetrics.event.queueSize));
    }
    
    // Check CPU threshold (if set)
    if (cpuThreshold > 0.0 && currentMetrics.cpu.cpuUsage > cpuThreshold) {
        thresholdExceeded = true;
        generateAlerts("cpu", currentMetrics.cpu.cpuUsage);
    }
    
    return thresholdExceeded;
}

void PerformanceMonitor::generateAlerts(const std::string& category, double value) const {
    alertCount++;
    
    // Log alert
    if (category == "memory") {
        NLM_LOG_WARNING("Performance Monitor Alert: Memory usage exceeded threshold - " 
                       + std::to_string(value) + " elements");
    } else if (category == "event_queue") {
        NLM_LOG_WARNING("Performance Monitor Alert: Event queue size exceeded threshold - " 
                       + std::to_string(value) + " events");
    } else if (category == "cpu") {
        NLM_LOG_WARNING("Performance Monitor Alert: CPU usage exceeded threshold - " 
                       + std::to_string(value * 100) + "%");
    }
}

std::string PerformanceMonitor::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
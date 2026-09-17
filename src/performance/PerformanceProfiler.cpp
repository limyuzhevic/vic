#include "PerformanceProfiler.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace nlm {

struct PerformanceProfiler::Impl {
    struct OperationMetrics {
        std::vector<std::chrono::microseconds> times;
        std::vector<std::chrono::microseconds> durations;  // For timeline events
        size_t operations;
        double cpuTime;
        size_t memoryUsage;
        size_t peakMemoryUsage;
        std::chrono::microseconds totalTime;
        
        OperationMetrics() : operations(0), cpuTime(0.0), memoryUsage(0), 
                           peakMemoryUsage(0), totalTime(0) {}
    };
    
    std::map<std::string, OperationMetrics> operationMetrics;
    std::map<std::string, double> cpuTimeThresholds;
    std::map<std::string, size_t> memoryUsageThresholds;
    std::map<std::string, double> opsPerSecondThresholds;
    
    std::vector<TimelineEvent> timeline;
    
    size_t totalMemoryUsage;
    std::chrono::microseconds totalTime;
    
    Impl() : totalMemoryUsage(0), totalTime(0) {}
};

PerformanceProfiler::PerformanceProfiler() : pImpl(new Impl()) {}

PerformanceProfiler::~PerformanceProfiler() = default;

PerformanceProfiler::PerformanceProfiler(PerformanceProfiler&&) noexcept = default;

PerformanceProfiler& PerformanceProfiler::operator=(PerformanceProfiler&&) noexcept = default;

PerformanceProfiler::Timer PerformanceProfiler::createTimer(const std::string& operation, const std::string& details) {
    return Timer(*this, operation, details);
}

void PerformanceProfiler::recordEvent(const std::string& operation, const std::chrono::microseconds& duration, 
                                     const std::string& details) {
    if (!pImpl->operationMetrics.count(operation)) {
        pImpl->operationMetrics[operation] = OperationMetrics();
    }
    
    auto& metrics = pImpl->operationMetrics[operation];
    metrics.times.push_back(duration);
    metrics.totalTime += duration;
    metrics.operations++;
    pImpl->totalTime += duration;
    
    if (details.empty()) {
        TimelineEvent event(operation, duration, "");
        pImpl->timeline.push_back(event);
    } else {
        TimelineEvent event(operation, duration, details);
        pImpl->timeline.push_back(event);
        metrics.durations.push_back(duration);
    }
}

void PerformanceProfiler::updateMemoryUsage(size_t memoryUsage, size_t peakMemoryUsage) {
    pImpl->totalMemoryUsage = std::max(pImpl->totalMemoryUsage, memoryUsage);
    for (auto& pair : pImpl->operationMetrics) {
        pair.second.memoryUsage = std::max(pair.second.memoryUsage, memoryUsage);
        pair.second.peakMemoryUsage = std::max(pair.second.peakMemoryUsage, peakMemoryUsage);
    }
}

void PerformanceProfiler::incrementOperations(const std::string& operation, size_t count) {
    if (!pImpl->operationMetrics.count(operation)) {
        pImpl->operationMetrics[operation] = OperationMetrics();
    }
    pImpl->operationMetrics[operation].operations += count;
}

PerformanceProfiler::PerformanceMetrics PerformanceProfiler::getMetrics(const std::string& operation) const {
    PerformanceMetrics metrics;
    
    if (!pImpl->operationMetrics.count(operation)) {
        return metrics;
    }
    
    const auto& opMetrics = pImpl->operationMetrics.at(operation);
    
    if (!opMetrics.times.empty()) {
        metrics.cpuTime = opMetrics.cpuTime;
        metrics.memoryUsage = opMetrics.memoryUsage;
        metrics.peakMemoryUsage = opMetrics.peakMemoryUsage;
        metrics.totalTime = opMetrics.totalTime;
        metrics.operationsCount = opMetrics.operations;
        
        if (!opMetrics.times.empty()) {
            double totalMicroseconds = std::accumulate(opMetrics.times.begin(), opMetrics.times.end(), 
                                                      std::chrono::microseconds(0)).count() / 1000.0;
            metrics.cpuTime = totalMicroseconds / 1000000.0;  // Convert to seconds
            
            if (metrics.operationsCount > 0) {
                metrics.operationsPerSecond = metrics.operationsCount / metrics.cpuTime;
            }
        }
    }
    
    return metrics;
}

std::vector<std::string> PerformanceProfiler::getAllOperations() const {
    std::vector<std::string> operations;
    operations.reserve(pImpl->operationMetrics.size());
    for (const auto& pair : pImpl->operationMetrics) {
        operations.push_back(pair.first);
    }
    return operations;
}

double PerformanceProfiler::getTotalTime() const {
    return std::chrono::duration<double, std::chrono::microseconds::period>(pImpl->totalTime).count() / 1000000.0;
}

size_t PerformanceProfiler::getTotalMemoryUsage() const {
    return pImpl->totalMemoryUsage;
}

size_t PerformanceProfiler::getTotalOperations() const {
    size_t totalOps = 0;
    for (const auto& pair : pImpl->operationMetrics) {
        totalOps += pair.second.operations;
    }
    return totalOps;
}

std::string PerformanceProfiler::generateReport() const {
    std::ostringstream oss;
    oss << "Performance Report" << std::endl;
    oss << "===================" << std::endl;
    oss << std::endl;
    
    for (const auto& pair : pImpl->operationMetrics) {
        const auto& operation = pair.first;
        const auto& metrics = pair.second;
        
        oss << "Operation: " << operation << std::endl;
        oss << "  Times: " << metrics.times.size() << " events" << std::endl;
        oss << "  Operations: " << metrics.operations << std::endl;
        oss << "  Total time: " << std::fixed << std::setprecision(6) 
            << std::chrono::duration<double, std::chrono::microseconds::period>(metrics.totalTime).count() 
            << " seconds" << std::endl;
        
        if (!metrics.times.empty()) {
            auto minIt = std::min_element(metrics.times.begin(), metrics.times.end());
            auto maxIt = std::max_element(metrics.times.begin(), metrics.times.end());
            double avg = std::accumulate(metrics.times.begin(), metrics.times.end(), 
                                       std::chrono::microseconds(0)).count() / 
                                       static_cast<double>(metrics.times.size()) / 1000.0;
            
            oss << "  Min time: " << minIt->count() / 1000.0 << " ms" << std::endl;
            oss << "  Max time: " << maxIt->count() / 1000.0 << " ms" << std::endl;
            oss << "  Avg time: " << avg / 1000.0 << " ms" << std::endl;
        }
        
        oss << "  Memory usage: " << metrics.memoryUsage << " bytes" << std::endl;
        oss << "  Peak memory: " << metrics.peakMemoryUsage << " bytes" << std::endl;
        
        if (metrics.operations > 0) {
            double opsPerSec = metrics.operations / std::max(1.0, 
                std::chrono::duration<double, std::chrono::microseconds::period>(metrics.totalTime).count());
            oss << "  Operations/sec: " << opsPerSec << std::endl;
        }
        
        oss << std::endl;
    }
    
    oss << "Total Operations: " << getTotalOperations() << std::endl;
    oss << "Total Time: " << getTotalTime() << " seconds" << std::endl;
    oss << "Total Memory Usage: " << getTotalMemoryUsage() << " bytes" << std::endl;
    
    return oss.str();
}

std::string PerformanceProfiler::generateDetailedReport() const {
    std::ostringstream oss;
    oss << generateReport() << std::endl;
    oss << std::endl;
    oss << "Detailed Timeline:" << std::endl;
    oss << "===================" << std::endl;
    
    for (const auto& event : pImpl->timeline) {
        auto timeSinceStart = std::chrono::duration<double, std::chrono::microseconds::period>(event.duration).count() / 1000000.0;
        oss << "[" << std::fixed << std::setprecision(6) << timeSinceStart << "s] "
            << event.operation << ": " << event.duration.count() / 1000.0 << " ms";
        if (!event.details.empty()) {
            oss << " - " << event.details;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

std::string PerformanceProfiler::getOperationSummary(const std::string& operation) const {
    if (!pImpl->operationMetrics.count(operation)) {
        return "Operation not found";
    }
    
    const auto& metrics = pImpl->operationMetrics.at(operation);
    std::ostringstream oss;
    
    oss << "Summary for operation: " << operation << std::endl;
    oss << "====================" << std::endl;
    oss << "Event count: " << metrics.times.size() << std::endl;
    oss << "Operation count: " << metrics.operations << std::endl;
    oss << "Total duration: " << std::chrono::duration<double, std::chrono::microseconds::period>(metrics.totalTime).count() << " seconds" << std::endl;
    
    if (!metrics.times.empty()) {
        double avgTime = std::accumulate(metrics.times.begin(), metrics.times.end(), 
                                       std::chrono::microseconds(0)).count() / 
                                       static_cast<double>(metrics.times.size()) / 1000.0;
        oss << "Average time per event: " << avgTime << " ms" << std::endl;
    }
    
    return oss.str();
}

void PerformanceProfiler::clear() {
    pImpl->operationMetrics.clear();
    pImpl->timeline.clear();
    pImpl->totalMemoryUsage = 0;
    pImpl->totalTime = std::chrono::microseconds(0);
}

void PerformanceProfiler::setPerformanceThreshold(const std::string& operation, double maxCpuTime, 
                                                 size_t maxMemoryUsage, double maxOperationsPerSecond) {
    pImpl->cpuTimeThresholds[operation] = maxCpuTime;
    pImpl->memoryUsageThresholds[operation] = maxMemoryUsage;
    if (maxOperationsPerSecond > 0.0) {
        pImpl->opsPerSecondThresholds[operation] = maxOperationsPerSecond;
    }
}

bool PerformanceProfiler::validatePerformance(const std::string& operation) const {
    if (!pImpl->operationMetrics.count(operation)) {
        return false;
    }
    
    const auto& metrics = pImpl->operationMetrics.at(operation);
    
    if (pImpl->cpuTimeThresholds.count(operation)) {
        double totalTime = std::chrono::duration<double, std::chrono::microseconds::period>(metrics.totalTime).count();
        if (totalTime > pImpl->cpuTimeThresholds.at(operation)) {
            return false;
        }
    }
    
    if (pImpl->memoryUsageThresholds.count(operation)) {
        if (metrics.memoryUsage > pImpl->memoryUsageThresholds.at(operation)) {
            return false;
        }
    }
    
    if (pImpl->opsPerSecondThresholds.count(operation)) {
        double opsPerSec = metrics.operations / std::max(1.0, 
            std::chrono::duration<double, std::chrono::microseconds::period>(metrics.totalTime).count());
        if (opsPerSec > pImpl->opsPerSecondThresholds.at(operation)) {
            return false;
        }
    }
    
    return true;
}

std::string PerformanceProfiler::getPerformanceError(const std::string& operation) const {
    if (!pImpl->operationMetrics.count(operation)) {
        return "Operation not found";
    }
    
    std::ostringstream oss;
    bool hasErrors = false;
    
    const auto& metrics = pImpl->operationMetrics.at(operation);
    
    if (pImpl->cpuTimeThresholds.count(operation)) {
        double totalTime = std::chrono::duration<double, std::chrono::microseconds::period>(metrics.totalTime).count();
        if (totalTime > pImpl->cpuTimeThresholds.at(operation)) {
            oss << "CPU time " << totalTime << "s exceeds threshold " << pImpl->cpuTimeThresholds.at(operation) << "s\n";
            hasErrors = true;
        }
    }
    
    if (pImpl->memoryUsageThresholds.count(operation)) {
        if (metrics.memoryUsage > pImpl->memoryUsageThresholds.at(operation)) {
            oss << "Memory usage " << metrics.memoryUsage << " exceeds threshold " << pImpl->memoryUsageThresholds.at(operation) << "\n";
            hasErrors = true;
        }
    }
    
    if (pImpl->opsPerSecondThresholds.count(operation)) {
        double opsPerSec = metrics.operations / std::max(1.0, 
            std::chrono::duration<double, std::chrono::microseconds::period>(metrics.totalTime).count());
        if (opsPerSec > pImpl->opsPerSecondThresholds.at(operation)) {
            oss << "Operations/sec " << opsPerSec << " exceeds threshold " << pImpl->opsPerSecondThresholds.at(operation) << "\n";
            hasErrors = true;
        }
    }
    
    return hasErrors ? oss.str() : "Performance meets all thresholds";
}

void PerformanceProfiler::addTimelineEvent(const TimelineEvent& event) {
    pImpl->timeline.push_back(event);
}

std::vector<PerformanceProfiler::TimelineEvent> PerformanceProfiler::getTimeline() const {
    return pImpl->timeline;
}

void PerformanceProfiler::recordStepMetrics(const size_t step, const size_t spikeCount, 
                                          const std::chrono::microseconds& stepTime) {
    std::ostringstream oss;
    oss << "Step " << step << ", " << spikeCount << " spikes, " << stepTime.count() << " us";
    recordEvent("SimulationStep", stepTime, oss.str());
}

std::string PerformanceProfiler::getSimulationProfile(size_t maxSteps) const {
    std::ostringstream oss;
    oss << "Simulation Performance Profile" << std::endl;
    oss << "===============================" << std::endl;
    oss << std::endl;
    
    // Find simulation step events
    size_t stepCount = 0;
    size_t totalSpikes = 0;
    double totalTime = 0.0;
    size_t minStepTime = std::numeric_limits<size_t>::max();
    size_t maxStepTime = 0;
    double avgStepTime = 0.0;
    
    for (const auto& event : pImpl->timeline) {
        if (event.operation == "SimulationStep") {
            if (maxSteps == 0 || stepCount < maxSteps) {
                stepCount++;
                
                // Parse event details to extract spike count
                size_t spikeCount = 0;
                size_t stepTimeUs = 0;
                std::string details = event.details;
                
                // Simple parsing of format: "Step X, Y spikes, Z us"
                size_t comma1 = details.find(',');
                if (comma1 != std::string::npos) {
                    size_t comma2 = details.find(',', comma1 + 1);
                    if (comma2 != std::string::npos) {
                        // Extract spike count
                        std::string spikeStr = details.substr(comma1 + 2, comma2 - comma1 - 3);
                        spikeCount = std::stoull(spikeStr);
                        
                        // Extract step time
                        size_t usPos = details.find("us", comma2);
                        if (usPos != std::string::npos) {
                            std::string timeStr = details.substr(comma2 + 3, usPos - comma2 - 3);
                            stepTimeUs = std::stoull(timeStr);
                        }
                    }
                }
                
                totalSpikes += spikeCount;
                stepTimeUs = static_cast<size_t>(event.duration.count());
                minStepTime = std::min(minStepTime, stepTimeUs);
                maxStepTime = std::max(maxStepTime, stepTimeUs);
                totalTime += event.duration.count() / 1000000.0;
            }
        }
    }
    
    if (stepCount > 0) {
        avgStepTime = totalTime / stepCount;
        
        oss << "Simulation Steps Processed: " << stepCount << std::endl;
        oss << "Total Spikes Generated: " << totalSpikes << std::endl;
        oss << "Average Step Time: " << avgStepTime * 1000.0 << " ms" << std::endl;
        oss << "Min Step Time: " << minStepTime / 1000.0 << " ms" << std::endl;
        oss << "Max Step Time: " << maxStepTime / 1000.0 << " ms" << std::endl;
        oss << "Average Firing Rate: " << static_cast<double>(totalSpikes) / stepCount << " spikes/step" << std::endl;
        oss << "Average Time per Spike: " << (totalTime * 1000.0) / std::max(1.0, static_cast<double>(totalSpikes)) << " ms/spike" << std::endl;
        
        oss << std::endl;
        oss << "Performance Rating:" << std::endl;
        
        // Simple performance rating based on step times
        if (avgStepTime < 0.001) {  // Less than 1ms per step
            oss << "  EXCELLENT - Very fast simulation" << std::endl;
        } else if (avgStepTime < 0.01) {  // Less than 10ms per step
            oss << "  GOOD - Acceptable simulation speed" << std::endl;
        } else if (avgStepTime < 0.1) {  // Less than 100ms per step
            oss << "  FAIR - Slow simulation, consider optimization" << std::endl;
        } else {
            oss << "  POOR - Very slow simulation, major optimization needed" << std::endl;
        }
    }
    
    return oss.str();
}

double PerformanceProfiler::getAverageTime(const std::string& operation) const {
    if (!pImpl->operationMetrics.count(operation) || pImpl->operationMetrics.at(operation).times.empty()) {
        return 0.0;
    }
    
    double totalTime = std::accumulate(pImpl->operationMetrics.at(operation).times.begin(),
                                      pImpl->operationMetrics.at(operation).times.end(),
                                      std::chrono::microseconds(0)).count() / 1000.0;
    return totalTime / pImpl->operationMetrics.at(operation).times.size();
}

double PerformanceProfiler::getMedianTime(const std::string& operation) const {
    if (!pImpl->operationMetrics.count(operation) || pImpl->operationMetrics.at(operation).times.empty()) {
        return 0.0;
    }
    
    auto times = pImpl->operationMetrics.at(operation).times;
    size_t n = times.size() / 2;
    std::nth_element(times.begin(), times.begin() + n, times.end());
    return times[n].count() / 1000.0;  // Convert to milliseconds
}

size_t PerformanceProfiler::getMinTime(const std::string& operation) const {
    if (!pImpl->operationMetrics.count(operation) || pImpl->operationMetrics.at(operation).times.empty()) {
        return 0;
    }
    
    auto minIt = std::min_element(pImpl->operationMetrics.at(operation).times.begin(),
                                 pImpl->operationMetrics.at(operation).times.end());
    return minIt->count();  // Return in microseconds
}

size_t PerformanceProfiler::getMaxTime(const std::string& operation) const {
    if (!pImpl->operationMetrics.count(operation) || pImpl->operationMetrics.at(operation).times.empty()) {
        return 0;
    }
    
    auto maxIt = std::max_element(pImpl->operationMetrics.at(operation).times.begin(),
                                 pImpl->operationMetrics.at(operation).times.end());
    return maxIt->count();  // Return in microseconds
}

size_t PerformanceProfiler::getEventCount(const std::string& operation) const {
    if (!pImpl->operationMetrics.count(operation)) {
        return 0;
    }
    return pImpl->operationMetrics.at(operation).times.size();
}

} // namespace nlm
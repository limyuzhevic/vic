#include "BrainProfiler.hpp"
#include "../brain/Brain.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/Performance.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <thread>

using json = nlohmann::json;

namespace nlm {

struct BrainProfiler::Impl {
    // Performance metrics storage
    std::unordered_map<std::string, std::vector<PerformanceMetric>> metrics;
    
    // Memory tracking
    MemoryStats currentMemoryStats;
    std::vector<size_t> memoryAllocations;
    
    // Firing statistics
    FiringStats currentFiringStats;
    std::unordered_map<size_t, std::vector<double>> firingPatterns;
    
    // Benchmark results
    std::vector<BenchmarkResult> benchmarks;
    
    // Simulation state
    size_t currentStep;
    double currentSimulationTime;
    
    // Bottleneck detection
    std::vector<std::string> recentOperations;
    
    // System monitoring
    std::atomic<double> cpuUsage{0.0};
    std::atomic<size_t> memoryAllocationsCounter{0};
    
    // Thread safety
    mutable std::mutex metricsMutex;
    
    Impl() : currentStep(0), currentSimulationTime(0.0) {}
};

BrainProfiler::BrainProfiler() : pImpl(std::make_unique<Impl>()) {}

BrainProfiler::~BrainProfiler() = default;

void BrainProfiler::recordSimulationMetrics(size_t step, double simulationTime,
                                             size_t neuronsProcessed, size_t spikesProcessed,
                                             const MemoryStats& memoryStats,
                                             const FiringStats& firingStats) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->currentStep = step;
    pImpl->currentSimulationTime = simulationTime;
    pImpl->currentMemoryStats = memoryStats;
    pImpl->currentFiringStats = firingStats;
    
    // Record simulation-specific metrics
    if (step > 0) {
        double stepTime = pImpl->currentSimulationTime - 
                         (pImpl->currentStep > 1 ? (pImpl->currentStep - 1) * pImpl->currentSimulationTime / pImpl->currentStep : 0.0);
        
        // Record step metrics
        pImpl->metrics["step_time"].emplace_back(stepTime, "seconds", "Time taken for this simulation step");
        pImpl->metrics["spikes_per_neuron"].emplace_back(
            static_cast<double>(firingStats.spikesPerStep) / neuronsProcessed, "spikes/neuron", "Spikes processed per neuron");
        
        // Record memory efficiency
        if (memoryStats.totalAllocated > 0) {
            double efficiency = static_cast<double>(memoryStats.used) / memoryStats.totalAllocated;
            pImpl->metrics["memory_efficiency"].emplace_back(efficiency, "ratio", "Memory usage efficiency");
        }
    }
    
    // Record neuron and spike counts
    pImpl->metrics["neurons_processed"].emplace_back(
        static_cast<double>(neuronsProcessed), "count", "Number of neurons processed");
    pImpl->metrics["spikes_processed"].emplace_back(
        static_cast<double>(spikesProcessed), "count", "Number of spikes processed");
    
    // Update firing history
    if (firingStats.firingHistory.size() > pImpl->currentFiringStats.firingHistory.size()) {
        pImpl->currentFiringStats.firingHistory = firingStats.firingHistory;
    }
}

void BrainProfiler::recordWallClockTime(const std::string& operation, double timeNs) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->metrics[operation + "_time"].emplace_back(timeNs, "nanoseconds", 
                                                   "Wall clock time for " + operation);
    
    pImpl->recentOperations.push_back(operation);
    if (pImpl->recentOperations.size() > 100) {
        pImpl->recentOperations.erase(pImpl->recentOperations.begin());
    }
}

void BrainProfiler::recordMemoryAllocation(size_t bytes, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->memoryAllocations.push_back(bytes);
    pImpl->memoryAllocationsCounter.fetch_add(bytes, std::memory_order_relaxed);
    
    if (!context.empty()) {
        pImpl->metrics[context + "_allocation"].emplace_back(
            static_cast<double>(bytes), "bytes", "Memory allocated for " + context);
    }
}

void BrainProfiler::recordFiringPattern(size_t neuronId, double firingRate, double activation) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->firingPatterns[neuronId].push_back(firingRate);
    pImpl->currentFiringStats.firingNeurons++;
    pImpl->currentFiringStats.totalSpikes += static_cast<size_t>(firingRate * 100); // Approximation
    
    // Record activation pattern
    pImpl->metrics["neuron_" + std::to_string(neuronId) + "_activation"].emplace_back(
        activation, "activation", "Neural activation level");
}

std::vector<BrainProfiler::PerformanceMetric> BrainProfiler::getMetrics(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    auto it = pImpl->metrics.find(name);
    if (it != pImpl->metrics.end()) {
        return it->second;
    }
    return {};
}

BrainProfiler::MemoryStats BrainProfiler::getMemoryStats() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->currentMemoryStats;
}

BrainProfiler::FiringStats BrainProfiler::getFiringStats() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->currentFiringStats;
}

BrainProfiler::BenchmarkResult BrainProfiler::getAverageBenchmark(const std::string& scenario) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<BenchmarkResult> matching;
    for (const auto& benchmark : pImpl->benchmarks) {
        if (benchmark.scenario == scenario) {
            matching.push_back(benchmark);
        }
    }
    
    if (matching.empty()) {
        return BenchmarkResult(scenario, 0.0, 0, 0, 0, 0.0);
    }
    
    double totalTime = 0.0;
    size_t totalNeurons = 0;
    size_t totalSpikes = 0;
    size_t totalMemory = 0;
    
    for (const auto& result : matching) {
        totalTime += result.executionTime;
        totalNeurons += result.neuronsProcessed;
        totalSpikes += result.spikesProcessed;
        totalMemory += result.memoryAllocated;
    }
    
    size_t count = matching.size();
    double avgTime = totalTime / count;
    double avgNeurons = static_cast<double>(totalNeurons) / count;
    double avgSpikes = static_cast<double>(totalSpikes) / count;
    double avgMemory = static_cast<double>(totalMemory) / count;
    
    double memoryEfficiency = (avgMemory > 0) ? (avgSpikes / avgMemory) : 0.0;
    
    return BenchmarkResult(scenario, avgTime, static_cast<size_t>(avgNeurons),
                           static_cast<size_t>(avgSpikes), static_cast<size_t>(avgMemory),
                           memoryEfficiency);
}

std::string BrainProfiler::generateReport() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::ostringstream oss;
    oss << "=== NLM Brain Performance Report ===\n";
    oss << "Generated at: " << std::chrono::steady_clock::now().time_since_epoch().count() << "\n\n";
    
    // Summary metrics
    oss << "--- Summary ---\n";
    oss << "Current Step: " << pImpl->currentStep << "\n";
    oss << "Simulation Time: " << pImpl->currentSimulationTime << " seconds\n";
    oss << "Total Memory Allocations: " << pImpl->memoryAllocations.size() << "\n";
    oss << "Total CPU Usage: " << pImpl->cpuUsage << "%\n";
    oss << "Memory Usage Counter: " << pImpl->memoryAllocationsCounter << " bytes\n\n";
    
    // Memory Statistics
    oss << "--- Memory Statistics ---\n";
    oss << "Total Allocated: " << pImpl->currentMemoryStats.totalAllocated << " bytes\n";
    oss << "Currently Used: " << pImpl->currentMemoryStats.used << " bytes\n";
    oss << "Available: " << pImpl->currentMemoryStats.free << " bytes\n";
    oss << "Usage Percentage: " << pImpl->currentMemoryStats.usagePercentage << "%\n\n";
    
    // Firing Statistics
    oss << "--- Firing Statistics ---\n";
    oss << "Total Spikes: " << pImpl->currentFiringStats.totalSpikes << "\n";
    oss << "Firing Neurons: " << pImpl->currentFiringStats.firingNeurons << "\n";
    oss << "Average Firing Rate: " << pImpl->currentFiringStats.averageFiringRate << "\n";
    oss << "E/I Ratio: " << pImpl->currentFiringStats.excitationInhibitionRatio << "\n";
    
    if (!pImpl->currentFiringStats.firingHistory.empty()) {
        oss << "Firing History (last 10 steps): ";
        size_t start = std::max(0ULL, pImpl->currentFiringStats.firingHistory.size() - 10);
        for (size_t i = start; i < pImpl->currentFiringStats.firingHistory.size(); ++i) {
            oss << pImpl->currentFiringStats.firingHistory[i] << " ";
        }
        oss << "\n";
    }
    
    oss << "\n--- Performance Metrics ---\n";
    for (const auto& pair : pImpl->metrics) {
        const auto& metricName = pair.first;
        const auto& values = pair.second;
        if (!values.empty()) {
            oss << metricName << ": ";
            for (size_t i = std::max(0ULL, values.size() - 3); i < values.size(); ++i) {
                oss << values[i].value;
                if (i < values.size() - 1) oss << ", ";
            }
            oss << "\n";
        }
    }
    
    // Bottleneck detection
    oss << "\n--- Bottleneck Analysis ---\n";
    auto bottlenecks = detectBottlenecks();
    if (bottlenecks.empty()) {
        oss << "No bottlenecks detected.\n";
    } else {
        for (const auto& bottleneck : bottlenecks) {
            oss << "- " << bottleneck << "\n";
        }
    }
    
    return oss.str();
}

std::vector<std::pair<double, double>> BrainProfiler::getPerformanceTimeSeries(
    const std::string& metricName) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<std::pair<double, double>> timeSeries;
    
    auto it = pImpl->metrics.find(metricName);
    if (it != pImpl->metrics.end()) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            timeSeries.emplace_back(static_cast<double>(i), it->second[i].value);
        }
    }
    
    return timeSeries;
}

void BrainProfiler::reset() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->metrics.clear();
    pImpl->memoryAllocations.clear();
    pImpl->firingPatterns.clear();
    pImpl->benchmarks.clear();
    pImpl->recentOperations.clear();
    pImpl->currentStep = 0;
    pImpl->currentSimulationTime = 0.0;
    pImpl->memoryAllocationsCounter.store(0);
}

std::string BrainProfiler::exportToJSON() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    json j;
    
    // Convert metrics to JSON
    json metricsJson = json::object();
    for (const auto& pair : pImpl->metrics) {
        json array = json::array();
        for (const auto& metric : pair.second) {
            array.push_back(metric.value);
        }
        metricsJson[pair.first] = array;
    }
    
    j["metrics"] = metricsJson;
    j["current_step"] = pImpl->currentStep;
    j["simulation_time"] = pImpl->currentSimulationTime;
    j["memory_allocations_count"] = pImpl->memoryAllocations.size();
    
    return j.dump(2);
}

std::string BrainProfiler::exportToCSV() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::ostringstream oss;
    oss << "Metric,Step,Value,Unit,Description\n";
    
    for (const auto& pair : pImpl->metrics) {
        for (size_t i = 0; i < pair.second.size(); ++i) {
            const auto& metric = pair.second[i];
            oss << pair.first << "," << i << "," 
                << metric.value << "," << metric.unit << "," << metric.description << "\n";
        }
    }
    
    return oss.str();
}

std::vector<std::string> BrainProfiler::detectBottlenecks() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<std::string> bottlenecks;
    
    // Check for high memory usage
    if (pImpl->currentMemoryStats.usagePercentage > 80.0) {
        bottlenecks.push_back("High memory usage detected (" + 
                             std::to_string(pImpl->currentMemoryStats.usagePercentage) + "%)");
    }
    
    // Check for slow step times
    auto stepTimes = getMetrics("step_time");
    if (!stepTimes.empty()) {
        double avgStepTime = 0.0;
        for (const auto& metric : stepTimes) {
            avgStepTime += metric.value;
        }
        avgStepTime /= stepTimes.size();
        
        if (avgStepTime > 100.0) { // 100ms threshold
            bottlenecks.push_back("Slow step processing detected (avg: " + 
                                 std::to_string(avgStepTime) + " ms)");
        }
    }
    
    // Check for memory leaks
    if (pImpl->memoryAllocations.size() > 1000) {
        bottlenecks.push_back("Potential memory leak: high allocation count");
    }
    
    // Check firing rate patterns
    if (!pImpl->currentFiringStats.firingHistory.empty()) {
        double avgFiring = pImpl->currentFiringStats.averageFiringRate;
        if (avgFiring > 100.0) {
            bottlenecks.push_back("High firing rate detected (average: " + 
                                 std::to_string(avgFiring) + ")");
        } else if (avgFiring < 0.1) {
            bottlenecks.push_back("Low firing rate detected (average: " + 
                                 std::to_string(avgFiring) + ")");
        }
    }
    
    // Check operation frequency
    if (pImpl->recentOperations.size() > 10) {
        std::unordered_map<std::string, size_t> operationCount;
        for (const auto& op : pImpl->recentOperations) {
            operationCount[op]++;
        }
        
        for (const auto& pair : operationCount) {
            if (pair.second > 10) {
                bottlenecks.push_back("Frequent operation: " + pair.first + " (called " + 
                                     std::to_string(pair.second) + " times)");
            }
        }
    }
    
    return bottlenecks;
}

BrainProfiler::SystemHealth BrainProfiler::getSystemHealth() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    SystemHealth health;
    
    // Calculate CPU usage (simplified)
    health.cpuUsage = pImpl->cpuUsage;
    
    // Calculate memory usage percentage
    health.memoryUsage = (pImpl->currentMemoryStats.totalAllocated > 0) ? 
                         pImpl->currentMemoryStats.usagePercentage : 0.0;
    
    // Calculate stability score based on firing history
    if (!pImpl->currentFiringStats.firingHistory.empty()) {
        double sum = 0.0, sumSquares = 0.0;
        for (double value : pImpl->currentFiringStats.firingHistory) {
            sum += value;
            sumSquares += value * value;
        }
        
        double mean = sum / pImpl->currentFiringStats.firingHistory.size();
        double variance = (sumSquares / pImpl->currentFiringStats.firingHistory.size()) - (mean * mean);
        double stdDev = std::sqrt(std::max(0.0, variance));
        
        // Stability score is inverse of coefficient of variation (CV)
        health.stabilityScore = (mean > 0) ? (1.0 / (1.0 + (stdDev / mean))) : 0.5;
    }
    
    // Check for memory leaks
    health.hasMemoryLeaks = detectBottlenecks().end() != 
                            std::find_if(detectBottlenecks().begin(), detectBottlenecks().end(),
                                        [](const std::string& b) { return b.find("memory leak") != std::string::npos; });
    
    // Determine overall status
    if (health.cpuUsage > 90.0 || health.memoryUsage > 90.0 || health.hasMemoryLeaks) {
        health.status = "critical";
    } else if (health.cpuUsage > 70.0 || health.memoryUsage > 70.0 || health.stabilityScore < 0.5) {
        health.status = "warning";
    } else {
        health.status = "healthy";
    }
    
    return health;
}

} // namespace nlm

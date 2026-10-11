#include "PerformanceVisualizer.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../performance/PerformanceProfiler.hpp"
#include "../performance/SIMDVectorization.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <numeric>

namespace nlm {

struct PerformanceVisualizer::Impl {
    bool active;
    double updateRate;
    PerformanceMetrics lastMetrics;
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::vector<double> cpuHistory;
    std::vector<double> memoryHistory;
    std::vector<double> simdHistory;
    uint64_t stepCount;
    
    Impl() : active(false), updateRate(1.0), stepCount(0) {
        cpuHistory.reserve(1000);
        memoryHistory.reserve(1000);
        simdHistory.reserve(1000);
    }
};

PerformanceVisualizer::PerformanceVisualizer() : active_(false), updateRate_(1.0) {
    pImpl = std::make_unique<Impl>();
    initialize();
}

PerformanceVisualizer::~PerformanceVisualizer() = default;

bool PerformanceVisualizer::initialize() {
    active_ = true;
    NLM_LOG_INFO("PerformanceVisualizer initialized");
    return true;
}

void PerformanceVisualizer::update(std::shared_ptr<PerformanceProfiler> profiler, std::shared_ptr<Brain> brain) {
    if (!active_ || !profiler) return;
    
    pImpl->stepCount++;
    
    // Collect profiler metrics
    collectProfilerMetrics(profiler);
    
    // Collect system metrics
    if (brain) {
        collectSystemMetrics(brain);
    }
    
    // Calculate derived metrics
    calculateDerivedMetrics();
    
    // Detect performance bottlenecks
    detectPerformanceBottlenecks();
    
    // Update historical data
    updateHistoricalData();
    
    if (updateCallback_) {
        updateCallback_(currentMetrics_);
    }
}

void PerformanceVisualizer::renderDashboard(const PerformanceOptions& options) const {
    std::cout << "=== Performance Dashboard ===" << std::endl;
    
    if (currentMetrics_.timestamp == 0.0) {
        std::cout << "No performance data available." << std::endl;
        return;
    }
    
    std::cout << "Last Update: " << std::fixed << std::setprecision(2) 
              << currentMetrics_.timestamp << "s ago" << std::endl;
    
    if (options.showRealTime) {
        renderRealTimeMetrics(options);
    }
    
    if (options.showHistorical) {
        renderHistoricalMetrics(options);
    }
    
    if (options.showAlerts && !alerts_.empty()) {
        renderAlerts(options);
    }
    
    if (pImpl->stepCount % 100 == 0) {
        std::cout << "\nPerformance Rating: " << getPerformanceRating() << std::endl;
    }
}

void PerformanceVisualizer::renderMemoryVisualizer() const {
    std::cout << "\n=== Memory Usage Visualizer ===" << std::endl;
    
    double memoryGB = currentMetrics_.memoryUsed / (1024.0 * 1024.0 * 1024.0);
    double peakGB = currentMetrics_.memoryPeak / (1024.0 * 1024.0 * 1024.0);
    double efficiency = currentMetrics_.memoryEfficiency * 100.0;
    
    std::cout << "Current Memory Usage: " << std::fixed << std::setprecision(2) 
              << memoryGB << " GB" << std::endl;
    std::cout << "Peak Memory Usage: " << std::fixed << std::setprecision(2) 
              << peakGB << " GB" << std::endl;
    std::cout << "Memory Efficiency: " << std::fixed << std::setprecision(1) 
              << efficiency << "%" << std::endl;
    std::cout << "Allocation Rate: " << std::fixed << std::setprecision(1) 
              << (currentMetrics_.memoryAllocationRate / (1024.0 * 1024.0)) << " MB/s" << std::endl;
    
    if (!memoryHistory.empty()) {
        std::cout << "\nMemory History (last " << memoryHistory.size() << " samples):" << std::endl;
        double minMem = *std::min_element(memoryHistory.begin(), memoryHistory.end());
        double maxMem = *std::max_element(memoryHistory.begin(), memoryHistory.end());
        double range = maxMem - minMem;
        
        for (size_t i = 0; i < std::min(size_t(20), memoryHistory.size()); ++i) {
            size_t idx = memoryHistory.size() - 20 + i;
            if (idx >= memoryHistory.size()) idx = memoryHistory.size() - 1;
            
            double mem = memoryHistory[idx];
            int barWidth = range > 0 ? static_cast<int>((mem - minMem) / range * 40) : 0;
            
            std::cout << "  " << std::setw(3) << static_cast<int>(mem / (1024.0 * 1024.0)) << " MB";
            
            for (int j = 0; j < barWidth; ++j) std::cout << "#";
            std::cout << std::endl;
        }
    }
}

void PerformanceVisualizer::renderCPUPerformanceChart() const {
    std::cout << "\n=== CPU Performance Chart ===" << std::endl;
    
    double cpuPercent = currentMetrics_.cpuUtilization;
    double cpuLoad = currentMetrics_.cpuLoadAvg;
    
    std::cout << "CPU Utilization: " << std::fixed << std::setprecision(1) 
              << cpuPercent << "%" << std::endl;
    std::cout << "System Load: " << std::fixed << std::setprecision(2) 
              << cpuLoad << std::endl;
    
    if (!cpuHistory.empty()) {
        std::cout << "\nCPU Usage History (last " << cpuHistory.size() << " samples):" << std::endl;
        double minCPU = *std::min_element(cpuHistory.begin(), cpuHistory.end());
        double maxCPU = *std::max_element(cpuHistory.begin(), cpuHistory.end());
        double range = maxCPU - minCPU;
        
        for (size_t i = 0; i < std::min(size_t(20), cpuHistory.size()); ++i) {
            size_t idx = cpuHistory.size() - 20 + i;
            if (idx >= cpuHistory.size()) idx = cpuHistory.size() - 1;
            
            double cpu = cpuHistory[idx];
            int barWidth = range > 0 ? static_cast<int>((cpu - minCPU) / range * 40) : 0;
            
            std::cout << "  " << std::setw(4) << static_cast<int>(cpu) << "%";
            
            std::string color;
            if (cpu > 90.0) color = "🔴"; // Critical
            else if (cpu > 70.0) color = "🟡"; // Warning
            else color = "🟢"; // Good
            
            for (int j = 0; j < barWidth; ++j) std::cout << "#";
            std::cout << " " << color << std::endl;
        }
    }
    
    // Check for CPU alerts
    if (cpuPercent > 95.0) {
        std::cout << "\n[ALERT] CPU usage is critical (" << std::fixed << std::setprecision(1) 
                  << cpuPercent << "%)" << std::endl;
    } else if (cpuPercent > 85.0) {
        std::cout << "\n[WARNING] CPU usage is high (" << std::fixed << std::setprecision(1) 
                  << cpuPercent << "%)" << std::endl;
    }
}

void PerformanceVisualizer::renderSIMDUtilizationChart() const {
    std::cout << "\n=== SIMD Utilization Chart ===" << std::endl;
    
    double simdRatio = currentMetrics_.simdUtilizationRatio * 100.0;
    uint64_t totalOps = currentMetrics_.simdOperations;
    
    std::cout << "SIMD Operations: " << totalOps << std::endl;
    std::cout << "SIMD Utilization: " << std::fixed << std::setprecision(1) 
              << simdRatio << "%" << std::endl;
    
    std::cout << "\nSIMD Operation Breakdown:" << std::endl;
    std::cout << "  LIF Operations:     " << currentMetrics_.simdLIFOperations << std::endl;
    std::cout << "  Synapse Operations: " << currentMetrics_.simdSynapseOperations << std::endl;
    std::cout << "  Memory Operations:  " << currentMetrics_.simdMemoryOperations << std::endl;
    std::cout << "  Similarity Ops:    " << currentMetrics_.simdSimilarityOperations << std::endl;
    
    if (!simdHistory.empty()) {
        std::cout << "\nSIMD Utilization History (last " << simdHistory.size() << " samples):" << std::endl;
        double minSIMD = *std::min_element(simdHistory.begin(), simdHistory.end());
        double maxSIMD = *std::max_element(simdHistory.begin(), simdHistory.end());
        double range = maxSIMD - minSIMD;
        
        for (size_t i = 0; i < std::min(size_t(20), simdHistory.size()); ++i) {
            size_t idx = simdHistory.size() - 20 + i;
            if (idx >= simdHistory.size()) idx = simdHistory.size() - 1;
            
            double simd = simdHistory[idx];
            int barWidth = range > 0 ? static_cast<int>((simd - minSIMD) / range * 40) : 0;
            
            std::cout << "  " << std::setw(4) << static_cast<int>(simd * 100) << "%";
            
            std::string efficiency;
            if (simd > 0.8) efficiency = "🟢 Efficient";
            else if (simd > 0.5) efficiency = "🟡 Moderate";
            else efficiency = "🔴 Low";
            
            for (int j = 0; j < barWidth; ++j) std::cout << "#";
            std::cout << " " << efficiency << std::endl;
        }
    }
}

void PerformanceVisualizer::renderEventQueueMetrics() const {
    std::cout << "\n=== Event Queue Metrics ===" << std::endl;
    
    std::cout << "Event Queue Size: " << currentMetrics_.eventQueueSize << std::endl;
    std::cout << "Events Processed: " << currentMetrics_.eventsProcessed << std::endl;
    std::cout << "Processing Rate: " << std::fixed << std::setprecision(1) 
              << currentMetrics_.eventProcessingRate << " events/sec" << std::endl;
    std::cout << "Queue Growth Rate: " << std::fixed << std::setprecision(3) 
              << currentMetrics_.eventQueueGrowthRate << " events/sec^2" << std::endl;
    
    // Check for queue issues
    if (currentMetrics_.eventQueueSize > 1000) {
        std::cout << "\n[ALERT] Event queue size is large (" << currentMetrics_.eventQueueSize << ")" << std::endl;
    }
    
    if (currentMetrics_.eventQueueGrowthRate > 10.0) {
        std::cout << "\n[ALERT] Event queue growth rate is high (" << std::fixed << std::setprecision(3) 
                  << currentMetrics_.eventQueueGrowthRate << " events/sec^2)" << std::endl;
    }
    
    if (currentMetrics_.eventProcessingRate < 100.0) {
        std::cout << "\n[WARNING] Event processing rate is low (" << std::fixed << std::setprecision(1) 
                  << currentMetrics_.eventProcessingRate << " events/sec)" << std::endl;
    }
}

void PerformanceVisualizer::renderParallelProcessingGraph() const {
    std::cout << "\n=== Parallel Processing Graph ===" << std::endl;
    
    double parallelEff = currentMetrics_.parallelEfficiency * 100.0;
    uint64_t parallelTasks = currentMetrics_.parallelTasks;
    uint64_t serialTasks = currentMetrics_.serialTasks;
    
    std::cout << "Parallel Tasks:  " << parallelTasks << std::endl;
    std::cout << "Serial Tasks:    " << serialTasks << std::endl;
    std::cout << "Parallel Efficiency: " << std::fixed << std::setprecision(1) 
              << parallelEff << "%" << std::endl;
    std::cout << "Thread Pool Size: " << currentMetrics_.threadPoolSize << std::endl;
    std::cout << "Avg Task Duration: " << currentMetrics_.averageTaskDuration << " μs" << std::endl;
    
    std::cout << "\nParallelization Ratio: ";
    if (serialTasks == 0) {
        std::cout << "N/A (no serial tasks)" << std::endl;
    } else {
        double ratio = static_cast<double>(parallelTasks) / serialTasks;
        std::cout << std::fixed << std::setprecision(2) << ratio << std::endl;
        
        if (ratio > 2.0) {
            std::cout << "[EXCELLENT] High parallel efficiency" << std::endl;
        } else if (ratio > 1.0) {
            std::cout << "[GOOD] Good parallel efficiency" << std::endl;
        } else if (ratio > 0.5) {
            std::cout << "[MODERATE] Moderate parallel efficiency" << std::endl;
        } else {
            std::cout << "[POOR] Low parallel efficiency" << std::endl;
        }
    }
}

void PerformanceVisualizer::generatePerformanceReport(const std::string& filename) const {
    std::cout << "Generating performance report to " << filename << std::endl;
    
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to create report file: " << filename << std::endl;
        return;
    }
    
    file << "=== Phase 6 Performance Report ===" << std::endl;
    file << "Generated at: " << std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count() << "s ago" << std::endl;
    file << "\n=== Performance Summary ===" << std::endl;
    file << "Wall Clock Time: " << std::fixed << std::setprecision(4) 
         << currentMetrics_.wallClockTime << " seconds" << std::endl;
    file << "Steps Completed: " << pImpl->stepCount << std::endl;
    file << "Step Time: " << std::fixed << std::setprecision(6) 
         << (pImpl->stepCount > 0 ? currentMetrics_.stepTime : 0.0) << " seconds per step" << std::endl;
    file << "\n=== Memory Performance ===" << std::endl;
    file << "Memory Used: " << currentMetrics_.memoryUsed << " bytes" << std::endl;
    file << "Memory Peak: " << currentMetrics_.memoryPeak << " bytes" << std::endl;
    file << "Memory Efficiency: " << std::fixed << std::setprecision(3) 
         << currentMetrics_.memoryEfficiency * 100.0 << "%" << std::endl;
    file << "Allocation Rate: " << std::fixed << std::setprecision(1) 
         << (currentMetrics_.memoryAllocationRate / (1024.0 * 1024.0)) << " MB/s" << std::endl;
    file << "\n=== SIMD Performance ===" << std::endl;
    file << "SIMD Operations: " << currentMetrics_.simdOperations << std::endl;
    file << "SIMD Utilization: " << std::fixed << std::setprecision(3) 
         << currentMetrics_.simdUtilizationRatio * 100.0 << "%" << std::endl;
    file << "LIF SIMD Ops: " << currentMetrics_.simdLIFOperations << std::endl;
    file << "Synapse SIMD Ops: " << currentMetrics_.simdSynapseOperations << std::endl;
    file << "\n=== Parallel Performance ===" << std::endl;
    file << "Parallel Tasks: " << currentMetrics_.parallelTasks << std::endl;
    file << "Serial Tasks: " << currentMetrics_.serialTasks << std::endl;
    file << "Parallel Efficiency: " << std::fixed << std::setprecision(3) 
         << currentMetrics_.parallelEfficiency * 100.0 << "%" << std::endl;
    file << "Thread Pool Size: " << currentMetrics_.threadPoolSize << std::endl;
    file << "\n=== Bottleneck Analysis ===" << std::endl;
    for (size_t i = 0; i < currentMetrics_.detectedBottlenecks.size(); ++i) {
        file << (i > 0 ? "  " : "") << "- " << currentMetrics_.detectedBottlenecks[i] 
             << " (Impact: " << std::fixed << std::setprecision(3) 
             << currentMetrics_.bottleneckImpact[i] << ")" << std::endl;
    }
    
    if (currentMetrics_.detectedBottlenecks.empty()) {
        file << "No significant bottlenecks detected." << std::endl;
    }
    
    file << "\n=== Alerts ===" << std::endl;
    if (alerts_.empty()) {
        file << "No alerts." << std::endl;
    } else {
        for (const auto& alert : alerts_) {
            file << "- " << std::get<0>(alert) << ": " << std::get<1>(alert) 
                 << " (threshold: " << std::get<2>(alert) << ", severity: " 
                 << std::get<3>(alert) << ")" << std::endl;
        }
    }
    
    file << "\n=== End Report ===" << std::endl;
    file.close();
}

void PerformanceVisualizer::generatePerformanceComparison(const ComparisonOptions& options) const {
    std::cout << "Generating performance comparison report" << std::endl;
    
    if (options.benchmarkFiles.empty()) {
        std::cout << "No benchmark files specified for comparison." << std::endl;
        return;
    }
    
    std::cout << "Comparing against " << options.benchmarkFiles.size() << " benchmark files:" << std::endl;
    for (const auto& file : options.benchmarkFiles) {
        std::cout << "  - " << file << std::endl;
    }
    
    std::cout << "\nComparison type: " << options.comparisonType << std::endl;
    std::cout << "Show delta: " << (options.showDelta ? "Yes" : "No") << std::endl;
    std::cout << "Show percentage: " << (options.showPercentage ? "Yes" : "No") << std::endl;
    
    // Note: In a real implementation, this would load the benchmark data
    // and perform actual comparisons. For this prototype, we'll just output
    // a placeholder.
    
    std::cout << "\n[Note: Full comparison functionality would require loading benchmark data]" << std::endl;
}

bool PerformanceVisualizer::exportData(const std::string& filename, const std::string& format) const {
    std::cout << "Exporting performance data to " << filename << " (format: " << format << ")" << std::endl;
    
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to create export file: " << filename << std::endl;
        return false;
    }
    
    if (format == "csv") {
        file << "timestamp,wall_clock_time,memory_used,memory_peak,simd_utilization,parallel_efficiency" << std::endl;
        
        // Write current metrics
        file << std::fixed << std::setprecision(6) 
             << currentMetrics_.timestamp << ","
             << currentMetrics_.wallClockTime << ","
             << currentMetrics_.memoryUsed << ","
             << currentMetrics_.memoryPeak << ","
             << currentMetrics_.simdUtilizationRatio << ","
             << currentMetrics_.parallelEfficiency << std::endl;
        
        // Note: In a full implementation, this would export historical data as well
    } else {
        std::cerr << "Unsupported export format: " << format << std::endl;
        return false;
    }
    
    file.close();
    return true;
}

void PerformanceVisualizer::addAlert(const std::string& metric, double value, double threshold, const std::string& severity) {
    alerts_.emplace_back(metric, value, threshold, severity);
}

void PerformanceVisualizer::clearAlerts() {
    alerts_.clear();
}

void PerformanceVisualizer::collectProfilerMetrics(std::shared_ptr<PerformanceProfiler> profiler) {
    if (!profiler) return;
    
    auto metrics = profiler->getCompleteMetrics();
    
    currentMetrics_.wallClockTime = metrics.wallClockTime;
    currentMetrics_.simdOperations = metrics.simdMetrics.totalSimdOperations;
    currentMetrics_.simdLIFOperations = metrics.simdMetrics.lifSimdOperations;
    currentMetrics_.simdSynapseOperations = metrics.simdMetrics.synapseSimdOperations;
    currentMetrics_.simdMemoryOperations = metrics.simdMetrics.memorySimdOperations;
    currentMetrics_.simdSimilarityOperations = metrics.simdMetrics.similaritySimdOperations;
    currentMetrics_.simdUtilizationRatio = metrics.simdUtilization;
    currentMetrics_.parallelTasks = metrics.parallelMetrics.parallelTasks;
    currentMetrics_.serialTasks = metrics.parallelMetrics.serialTasks;
    currentMetrics_.parallelEfficiency = metrics.parallelMetrics.parallelEfficiency;
    currentMetrics_.threadPoolSize = metrics.parallelMetrics.threadPoolSize;
    currentMetrics_.averageTaskDuration = metrics.parallelMetrics.averageTaskDuration;
    currentMetrics_.cacheHits = metrics.cacheMetrics.cacheHits;
    currentMetrics_.cacheMisses = metrics.cacheMetrics.cacheMisses;
    currentMetrics_.cacheHitRate = metrics.cacheHitRate;
    currentMetrics_.totalCacheAccesses = metrics.cacheMetrics.totalAccesses;
    currentMetrics_.cacheLinesFetched = metrics.cacheMetrics.linesFetched;
    currentMetrics_.cacheLinesEvicted = metrics.cacheMetrics.linesEvicted;
    
    // Update history
    cpuHistory.push_back(currentMetrics_.cpuUtilization);
    memoryHistory.push_back(static_cast<double>(currentMetrics_.memoryUsed));
    simdHistory.push_back(currentMetrics_.simdUtilizationRatio);
    
    // Keep history limited
    const size_t maxHistorySize = 1000;
    if (cpuHistory.size() > maxHistorySize) cpuHistory.erase(cpuHistory.begin());
    if (memoryHistory.size() > maxHistorySize) memoryHistory.erase(memoryHistory.begin());
    if (simdHistory.size() > maxHistorySize) simdHistory.erase(simdHistory.begin());
}

void PerformanceVisualizer::collectSystemMetrics(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    // Simple system metrics collection
    currentMetrics_.cpuUtilization = 50.0 + (rand() % 30); // Simulated CPU usage (50-80%)
    currentMetrics_.cpuLoadAvg = 1.0 + (rand() % 100) / 100.0; // Load average (1.0-2.0)
    currentMetrics_.eventQueueSize = rand() % 100; // Simulated event queue size
    currentMetrics_.eventsProcessed = pImpl->stepCount * 100; // Estimated
    currentMetrics_.eventProcessingRate = 1000.0; // events/sec
    currentMetrics_.eventQueueGrowthRate = (rand() % 20) / 1000.0; // growth rate
    
    // Update timestamp
    currentMetrics_.timestamp = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count() - pImpl->lastUpdateTime.count();
}

void PerformanceVisualizer::calculateDerivedMetrics() {
    if (currentMetrics_.wallClockTime > 0.0) {
        currentMetrics_.stepTime = currentMetrics_.wallClockTime / pImpl->stepCount;
    }
    
    if (currentMetrics_.memoryAllocated > 0) {
        currentMetrics_.memoryEfficiency = static_cast<double>(currentMetrics_.memoryUsed) / 
                                          currentMetrics_.memoryAllocated;
    }
    
    if (currentMetrics_.totalCacheAccesses > 0) {
        currentMetrics_.cacheHitRate = static_cast<double>(currentMetrics_.cacheHits) / 
                                       currentMetrics_.totalCacheAccesses;
    }
    
    // Calculate event queue growth rate
    static uint64_t lastQueueSize = currentMetrics_.eventQueueSize;
    currentMetrics_.eventQueueGrowthRate = static_cast<double>(currentMetrics_.eventQueueSize - lastQueueSize);
    lastQueueSize = currentMetrics_.eventQueueSize;
    
    // Generate performance alerts
    generatePerformanceAlerts();
}

void PerformanceVisualizer::detectPerformanceBottlenecks() {
    currentMetrics_.detectedBottlenecks.clear();
    currentMetrics_.bottleneckImpact.clear();
    
    // Detect memory bottleneck
    if (currentMetrics_.memoryEfficiency < 0.5) {
        currentMetrics_.detectedBottlenecks.push_back("Memory efficiency");
        currentMetrics_.bottleneckImpact.push_back(1.0 - currentMetrics_.memoryEfficiency);
    }
    
    // Detect SIMD bottleneck
    if (currentMetrics_.simdUtilizationRatio < 0.3) {
        currentMetrics_.detectedBottlenecks.push_back("SIMD utilization");
        currentMetrics_.bottleneckImpact.push_back(1.0 - currentMetrics_.simdUtilizationRatio);
    }
    
    // Detect CPU bottleneck
    if (currentMetrics_.cpuUtilization > 90.0) {
        currentMetrics_.detectedBottlenecks.push_back("CPU utilization");
        currentMetrics_.bottleneckImpact.push_back((currentMetrics_.cpuUtilization - 90.0) / 10.0);
    }
    
    // Detect parallel processing bottleneck
    if (currentMetrics_.parallelEfficiency < 0.3) {
        currentMetrics_.detectedBottlenecks.push_back("Parallel processing efficiency");
        currentMetrics_.bottleneckImpact.push_back(1.0 - currentMetrics_.parallelEfficiency);
    }
    
    // Detect event queue bottleneck
    if (currentMetrics_.eventQueueGrowthRate > 5.0) {
        currentMetrics_.detectedBottlenecks.push_back("Event queue growth");
        currentMetrics_.bottleneckImpact.push_back(std::min(1.0, currentMetrics_.eventQueueGrowthRate / 10.0));
    }
}

void PerformanceVisualizer::updateHistoricalData() {
    PerformanceDataPoint dataPoint;
    dataPoint.timestamp = currentMetrics_.timestamp;
    dataPoint.metrics["wall_clock_time"] = currentMetrics_.wallClockTime;
    dataPoint.metrics["memory_used"] = static_cast<double>(currentMetrics_.memoryUsed);
    dataPoint.metrics["simd_utilization"] = currentMetrics_.simdUtilizationRatio;
    dataPoint.metrics["parallel_efficiency"] = currentMetrics_.parallelEfficiency;
    dataPoint.metrics["cpu_utilization"] = currentMetrics_.cpuUtilization;
    
    historicalData_.push_back(dataPoint);
    
    // Keep historical data limited
    const size_t maxHistorySize = 1000;
    if (historicalData_.size() > maxHistorySize) {
        historicalData_.erase(historicalData_.begin());
    }
}

std::string PerformanceVisualizer::getPerformanceRating() const {
    double avgSIMD = 0.0, avgParallel = 0.0, avgMemory = 0.0;
    int count = 0;
    
    for (const auto& dataPoint : historicalData_) {
        if (dataPoint.metrics.count("simd_utilization")) {
            avgSIMD += dataPoint.metrics.at("simd_utilization");
            count++;
        }
        if (dataPoint.metrics.count("parallel_efficiency")) {
            avgParallel += dataPoint.metrics.at("parallel_efficiency");
            count++;
        }
        if (dataPoint.metrics.count("memory_used")) {
            avgMemory += dataPoint.metrics.at("memory_used");
            count++;
        }
    }
    
    if (count == 0) return "Unknown";
    
    avgSIMD /= count;
    avgParallel /= count;
    avgMemory /= count;
    
    double score = (avgSIMD * 0.4) + (avgParallel * 0.3) + ((1.0 - avgMemory) * 0.3);
    
    if (score > 0.8) return "Excellent";
    else if (score > 0.6) return "Good";
    else if (score > 0.4) return "Fair";
    else return "Needs Improvement";
}

void PerformanceVisualizer::generatePerformanceAlerts() {
    // Clear existing alerts
    alerts_.clear();
    
    // Memory alerts
    if (currentMetrics_.memoryEfficiency < 0.3) {
        addAlert("Memory Efficiency", currentMetrics_.memoryEfficiency * 100.0, 30.0, "critical");
    } else if (currentMetrics_.memoryEfficiency < 0.5) {
        addAlert("Memory Efficiency", currentMetrics_.memoryEfficiency * 100.0, 50.0, "warning");
    }
    
    // SIMD alerts
    if (currentMetrics_.simdUtilizationRatio < 0.2) {
        addAlert("SIMD Utilization", currentMetrics_.simdUtilizationRatio * 100.0, 20.0, "critical");
    } else if (currentMetrics_.simdUtilizationRatio < 0.4) {
        addAlert("SIMD Utilization", currentMetrics_.simdUtilizationRatio * 100.0, 40.0, "warning");
    }
    
    // CPU alerts
    if (currentMetrics_.cpuUtilization > 95.0) {
        addAlert("CPU Utilization", currentMetrics_.cpuUtilization, 95.0, "critical");
    } else if (currentMetrics_.cpuUtilization > 85.0) {
        addAlert("CPU Utilization", currentMetrics_.cpuUtilization, 85.0, "warning");
    }
    
    // Parallel efficiency alerts
    if (currentMetrics_.parallelEfficiency < 0.2) {
        addAlert("Parallel Efficiency", currentMetrics_.parallelEfficiency * 100.0, 20.0, "critical");
    } else if (currentMetrics_.parallelEfficiency < 0.4) {
        addAlert("Parallel Efficiency", currentMetrics_.parallelEfficiency * 100.0, 40.0, "warning");
    }
    
    // Event queue alerts
    if (currentMetrics_.eventQueueSize > 1000) {
        addAlert("Event Queue Size", static_cast<double>(currentMetrics_.eventQueueSize), 1000.0, "critical");
    } else if (currentMetrics_.eventQueueSize > 500) {
        addAlert("Event Queue Size", static_cast<double>(currentMetrics_.eventQueueSize), 500.0, "warning");
    }
    
    if (currentMetrics_.eventQueueGrowthRate > 10.0) {
        addAlert("Event Queue Growth Rate", currentMetrics_.eventQueueGrowthRate, 10.0, "critical");
    } else if (currentMetrics_.eventQueueGrowthRate > 5.0) {
        addAlert("Event Queue Growth Rate", currentMetrics_.eventQueueGrowthRate, 5.0, "warning");
    }
}

void PerformanceVisualizer::renderRealTimeMetrics(const PerformanceOptions& options) const {
    std::cout << "\n=== Real-time Metrics ===" << std::endl;
    
    std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(4) 
              << currentMetrics_.wallClockTime << "s" << std::endl;
    std::cout << "Memory Usage: " << currentMetrics_.memoryUsed << " bytes" << std::endl;
    std::cout << "SIMD Operations: " << currentMetrics_.simdOperations << std::endl;
    std::cout << "Parallel Efficiency: " << std::fixed << std::setprecision(1) 
              << (currentMetrics_.parallelEfficiency * 100.0) << "%" << std::endl;
    
    // Show specific metrics based on options
    for (const auto& metric : options.metricsToShow) {
        if (metric == "cpu_utilization") {
            std::cout << "CPU Utilization: " << std::fixed << std::setprecision(1) 
                      << currentMetrics_.cpuUtilization << "%" << std::endl;
        } else if (metric == "memory_efficiency") {
            std::cout << "Memory Efficiency: " << std::fixed << std::setprecision(1) 
                      << (currentMetrics_.memoryEfficiency * 100.0) << "%" << std::endl;
        } else if (metric == "cache_hit_rate") {
            std::cout << "Cache Hit Rate: " << std::fixed << std::setprecision(1) 
                      << (currentMetrics_.cacheHitRate * 100.0) << "%" << std::endl;
        } else if (metric == "event_queue_size") {
            std::cout << "Event Queue Size: " << currentMetrics_.eventQueueSize << std::endl;
        } else if (metric == "simd_utilization") {
            std::cout << "SIMD Utilization: " << std::fixed << std::setprecision(1) 
                      << (currentMetrics_.simdUtilizationRatio * 100.0) << "%" << std::endl;
        }
    }
}

void PerformanceVisualizer::renderHistoricalMetrics(const PerformanceOptions& options) const {
    if (historicalData_.empty()) return;
    
    std::cout << "\n=== Historical Metrics ===" << std::endl;
    std::cout << "Data Points: " << historicalData_.size() << std::endl;
    
    double latestTimestamp = historicalData_.back().timestamp;
    double timeRange = latestTimestamp - historicalData_.front().timestamp;
    
    std::cout << "Time Range: " << std::fixed << std::setprecision(2) 
              << timeRange << "s" << std::endl;
    
    if (options.timeRange == "last_minute" && timeRange > 60.0) {
        std::cout << "[Note: Showing full history, use 'last_minute' for recent data only]" << std::endl;
    }
}

void PerformanceVisualizer::renderAlerts(const PerformanceOptions& options) const {
    std::cout << "\n=== Performance Alerts ===" << std::endl;
    if (alerts_.empty()) {
        std::cout << "No active alerts." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < alerts_.size(); ++i) {
        const auto& alert = alerts_[i];
        std::cout << "  " << std::get<0>(alert) << ": " << std::fixed << std::setprecision(1) 
                  << std::get<1>(alert);
        
        std::string severity = std::get<3>(alert);
        if (severity == "critical") {
            std::cout << " [" << severity << "]";
        } else if (severity == "warning") {
            std::cout << " [" << severity << "]";
        }
        
        std::cout << std::endl;
    }
}

} // namespace nlm

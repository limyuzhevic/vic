#include "PerformanceAnalyzer.hpp"
#include "../core/Logger.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace nlm {

struct PerformanceAnalyzer::Impl {
    // Performance monitoring
    std::vector<PerformanceProfile> performanceHistory;
    PerformanceProfile currentProfile;
    double analysisInterval; // seconds
    size_t maxHistoricalData;
    
    // Bottleneck detection
    std::vector<BottleneckAnalysis> bottleneckAnalysis;
    bool enableMemoryLeakDetection;
    bool enableBottleneckDetection;
    bool enableOptimizationSuggestions;
    
    // Optimization hints
    std::vector<OptimizationHint> optimizationHints;
    
    // Memory tracking
    size_t baselineMemoryUsage;
    size_t peakMemoryUsage;
    size_t leakThreshold;
    std::map<std::string, size_t> componentMemoryUsage;
    
    // Timing tracking
    std::map<std::string, double> componentExecutionTimes;
    std::map<std::string, uint64_t> componentCallCounts;
    
    // Neural network specific tracking
    std::map<std::string, size_t> neuronCounts;
    std::map<std::string, size_t> synapseCounts;
    std::map<std::string, float> firingRates;
    
    Impl() : analysisInterval(1.0), maxHistoricalData(10000),
             enableMemoryLeakDetection(true), enableBottleneckDetection(true),
             enableOptimizationSuggestions(true), baselineMemoryUsage(0),
             peakMemoryUsage(0), leakThreshold(1024 * 1024) { // 1MB leak threshold
    }
};

PerformanceAnalyzer::PerformanceAnalyzer() : pImpl(std::make_unique<Impl>()) {}
PerformanceAnalyzer::~PerformanceAnalyzer() = default;

bool PerformanceAnalyzer::initialize() {
    try {
        Logger::getInstance().logInfo("PerformanceAnalyzer initialized");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize PerformanceAnalyzer: " + std::string(e.what()));
        return false;
    }
}

void PerformanceAnalyzer::shutdown() {
    Logger::getInstance().logInfo("PerformanceAnalyzer shutdown");
}

void PerformanceAnalyzer::recordPerformanceSample(const PerformanceProfile& profile) {
    pImpl->performanceHistory.push_back(profile);
    pImpl->currentProfile = profile;
    
    while (pImpl->performanceHistory.size() > pImpl->maxHistoricalData) {
        pImpl->performanceHistory.erase(pImpl->performanceHistory.begin());
    }
}

void PerformanceAnalyzer::analyzeComponentPerformance(const std::string& componentName) {
    // Analyze performance for a specific component
    double totalTime = 0.0;
    uint64_t callCount = 0;
    
    for (const auto& profile : pImpl->performanceHistory) {
        if (componentName == "memory") {
            totalTime += static_cast<double>(profile.memory.totalMemoryUsage);
        } else if (componentName == "network") {
            totalTime += profile.network.spikeRate * 1000.0;
        } else if (componentName == "io") {
            totalTime += profile.io.cpuUtilization;
        }
        callCount++;
    }
    
    if (callCount > 0) {
        pImpl->componentExecutionTimes[componentName] = totalTime / callCount;
        pImpl->componentCallCounts[componentName] = callCount;
    }
}

void PerformanceAnalyzer::detectMemoryLeaks() {
    if (!pImpl->enableMemoryLeakDetection) return;
    
    for (const auto& [componentName, usage] : pImpl->componentMemoryUsage) {
        if (usage > pImpl->baselineMemoryUsage + pImpl->leakThreshold) {
            BottleneckAnalysis bottleneck;
            bottleneck.bottleneckType = "memory";
            bottleneck.componentName = componentName;
            bottleneck.severity = std::min(1.0, static_cast<double>(usage - pImpl->baselineMemoryUsage) / static_cast<double>(pImpl->leakThreshold));
            bottleneck.impactOnThroughput = bottleneck.severity * 100.0;
            bottleneck.estimatedImprovement = bottleneck.severity * 80.0;
            bottleneck.rootCauses.push_back("Memory allocation without corresponding deallocation");
            bottleneck.suggestedSolutions.push_back("Implement memory pools or reference counting");
            bottleneck.isResolved = false;
            
            pImpl->bottleneckAnalysis.push_back(bottleneck);
        }
    }
}

void PerformanceAnalyzer::analyzeNeuralNetworkTopology() {
    for (const auto& profile : pImpl->performanceHistory) {
        for (const auto& [regionName, neuronCount] : profile.network.regionMetrics) {
            pImpl->neuronCounts[regionName] += neuronCount;
        }
    }
}

void PerformanceAnalyzer::identifyBottlenecks() {
    if (!pImpl->enableBottleneckDetection) return;
    
    // Analyze execution times to identify bottlenecks
    double maxTime = 0.0;
    std::string bottleneckComponent;
    
    for (const auto& [componentName, avgTime] : pImpl->componentExecutionTimes) {
        if (avgTime > maxTime) {
            maxTime = avgTime;
            bottleneckComponent = componentName;
        }
    }
    
    if (!bottleneckComponent.empty() && maxTime > 0.0) {
        BottleneckAnalysis bottleneck;
        bottleneck.bottleneckType = "cpu";
        bottleneck.componentName = bottleneckComponent;
        bottleneck.severity = std::min(1.0, maxTime / 1000.0); // Normalize to 1.0
        bottleneck.impactOnThroughput = (maxTime / maxTime) * 100.0;
        bottleneck.estimatedImprovement = bottleneck.severity * 70.0;
        bottleneck.rootCauses.push_back("Computationally intensive operations");
        bottleneck.rootCauses.push_back("Inefficient algorithms");
        bottleneck.suggestedSolutions.push_back("Optimize critical loops");
        bottleneck.suggestedSolutions.push_back("Use vectorization");
        bottleneck.suggestedSolutions.push_back("Implement parallelization");
        bottleneck.isResolved = false;
        
        pImpl->bottleneckAnalysis.push_back(bottleneck);
    }
}

void PerformanceAnalyzer::analyzeSpikePropagationBottlenecks() {
    double totalSpikeTime = 0.0;
    size_t totalSpikes = 0;
    
    for (const auto& profile : pImpl->performanceHistory) {
        totalSpikeTime += profile.network.spikeRate * 10.0; // Approximate spike processing time
        totalSpikes += static_cast<size_t>(profile.network.spikeRate * 1000.0);
    }
    
    if (totalSpikes > 0 && totalSpikeTime / totalSpikes > 1.0) { // > 1ms per spike
        BottleneckAnalysis bottleneck;
        bottleneck.bottleneckType = "network";
        bottleneck.componentName = "spike_propagation";
        bottleneck.severity = std::min(1.0, (totalSpikeTime / totalSpikes - 1.0) / 10.0);
        bottleneck.impactOnThroughput = 50.0; // Estimated impact
        bottleneck.estimatedImprovement = 60.0;
        bottleneck.rootCauses.push_back("Serial spike processing");
        bottleneck.rootCauses.push_back("Inefficient connectivity representation");
        bottleneck.suggestedSolutions.push_back("Parallel spike processing");
        bottleneck.suggestedSolutions.push_back("Sparse connectivity optimization");
        bottleneck.suggestedSolutions.push_back("Event-driven processing");
        bottleneck.isResolved = false;
        
        pImpl->bottleneckAnalysis.push_back(bottleneck);
    }
}

void PerformanceAnalyzer::analyzeMemorySystemBottlenecks() {
    double memoryPressure = 0.0;
    
    for (const auto& profile : pImpl->performanceHistory) {
        double usageRatio = static_cast<double>(profile.memory.totalMemoryUsage) / 1024.0 / 1024.0 / 1024.0; // GB
        memoryPressure += usageRatio;
    }
    
    if (memoryPressure / pImpl->performanceHistory.size() > 2.0) { // > 2GB average usage
        BottleneckAnalysis bottleneck;
        bottleneck.bottleneckType = "memory";
        bottleneck.componentName = "memory_system";
        bottleneck.severity = std::min(1.0, (memoryPressure / pImpl->performanceHistory.size() - 2.0) / 10.0);
        bottleneck.impactOnThroughput = 40.0;
        bottleneck.estimatedImprovement = 50.0;
        bottleneck.rootCauses.push_back("Memory fragmentation");
        bottleneck.rootCauses.push_back("Inefficient data structures");
        bottleneck.suggestedSolutions.push_back("Use memory pools");
        bottleneck.suggestedSolutions.push_back("Implement compaction");
        bottleneck.suggestedSolutions.push_back("Optimize data layouts");
        bottleneck.isResolved = false;
        
        pImpl->bottleneckAnalysis.push_back(bottleneck);
    }
}

void PerformanceAnalyzer::analyzePlasticityComputationBottlenecks() {
    double plasticityTime = 0.0;
    
    for (const auto& profile : pImpl->performanceHistory) {
        plasticityTime += profile.network.plasticityFactor * 100.0; // Approximate plasticity computation time
    }
    
    if (plasticityTime / pImpl->performanceHistory.size() > 10.0) { // > 10ms average
        BottleneckAnalysis bottleneck;
        bottleneck.bottleneckType = "cpu";
        bottleneck.componentName = "plasticity_computation";
        bottleneck.severity = std::min(1.0, (plasticityTime / pImpl->performanceHistory.size() - 10.0) / 100.0);
        bottleneck.impactOnThroughput = 30.0;
        bottleneck.estimatedImprovement = 70.0;
        bottleneck.rootCauses.push_back("Serial STDP computation");
        bottleneck.rootCauses.push_back("Inefficient Hebbian updates");
        bottleneck.rootCauses.push_back("Expensive structural plasticity");
        bottleneck.suggestedSolutions.push_back("Parallel plasticity computation");
        bottleneck.suggestedSolutions.push_back("Approximate STDP");
        bottleneck.suggestedSolutions.push_back("Lazy plasticity updates");
        bottleneck.isResolved = false;
        
        pImpl->bottleneckAnalysis.push_back(bottleneck);
    }
}

void PerformanceAnalyzer::analyzeIntegrationLoopBottlenecks() {
    double integrationTime = 0.0;
    
    for (const auto& profile : pImpl->performanceHistory) {
        integrationTime += profile.network.averageFiringRate * 10.0; // Approximate integration time
    }
    
    if (integrationTime / pImpl->performanceHistory.size() > 100.0) { // > 100ms average
        BottleneckAnalysis bottleneck;
        bottleneck.bottleneckType = "cpu";
        bottleneck.componentName = "integration_loop";
        bottleneck.severity = std::min(1.0, (integrationTime / pImpl->performanceHistory.size() - 100.0) / 1000.0);
        bottleneck.impactOnThroughput = 60.0;
        bottleneck.estimatedImprovement = 80.0;
        bottleneck.rootCauses.push_back("Serial integration");
        bottleneck.rootCauses.push_back("Inefficient spike processing");
        bottleneck.rootCauses.push_back("Redundant computations");
        bottleneck.suggestedSolutions.push_back("Parallel integration");
        bottleneck.suggestedSolutions.push_back("Optimized spike handling");
        bottleneck.suggestedSolutions.push_back("Integration caching");
        bottleneck.isResolved = false;
        
        pImpl->bottleneckAnalysis.push_back(bottleneck);
    }
}

void PerformanceAnalyzer::generateOptimizationHints() {
    if (!pImpl->enableOptimizationSuggestions) return;
    
    // Memory optimization hints
    if (pImpl->componentMemoryUsage.size() > 0) {
        for (const auto& [componentName, usage] : pImpl->componentMemoryUsage) {
            if (usage > 100 * 1024 * 1024) { // > 100MB
                OptimizationHint hint;
                hint.componentName = componentName;
                hint.optimizationType = "memory";
                hint.description = "Consider implementing memory pools for " + componentName;
                hint.potentialImprovement = std::min(80.0, static_cast<double>(usage) / 10.0 / 1024.0 / 1024.0); // Percentage
                hint.isImplemented = false;
                hint.isCritical = usage > 1024 * 1024 * 1024; // > 1GB
                hint.suggestedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()).count() + 86400000; // 1 day from now
                
                pImpl->optimizationHints.push_back(hint);
            }
        }
    }
    
    // Parallelization hints
    if (pImpl->componentExecutionTimes.size() > 0) {
        double maxTime = 0.0;
        std::string maxComponent;
        
        for (const auto& [componentName, avgTime] : pImpl->componentExecutionTimes) {
            if (avgTime > maxTime) {
                maxTime = avgTime;
                maxComponent = componentName;
            }
        }
        
        if (!maxComponent.empty() && maxTime > 100.0) { // > 100ms
            OptimizationHint hint;
            hint.componentName = maxComponent;
            hint.optimizationType = "parallel";
            hint.description = "Implement parallelization for " + maxComponent;
            hint.potentialImprovement = std::min(90.0, maxTime / 10.0);
            hint.isImplemented = false;
            hint.isCritical = maxTime > 1000.0; // > 1s
            hint.suggestedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count() + 2592000000; // 1 month from now
            
            pImpl->optimizationHints.push_back(hint);
        }
    }
}

void PerformanceAnalyzer::analyzeMemoryAllocationPatterns() {
    // Analyze allocation patterns for optimization opportunities
    for (const auto& profile : pImpl->performanceHistory) {
        if (profile.memory.currentAllocationCount > 100) {
            BottleneckAnalysis bottleneck;
            bottleneck.bottleneckType = "memory";
            bottleneck.componentName = "allocation_manager";
            bottleneck.severity = std::min(1.0, static_cast<double>(profile.memory.currentAllocationCount) / 10000.0);
            bottleneck.impactOnThroughput = 20.0;
            bottleneck.estimatedImprovement = 40.0;
            bottleneck.rootCauses.push_back("Frequent small allocations");
            bottleneck.rootCauses.push_back("Manual memory management");
            bottleneck.suggestedSolutions.push_back("Use memory pools");
            bottleneck.suggestedSolutions.push_back("Implement allocation caches");
            bottleneck.suggestedSolutions.push_back("Use smart pointers");
            bottleneck.isResolved = false;
            
            pImpl->bottleneckAnalysis.push_back(bottleneck);
            break;
        }
    }
}

void PerformanceAnalyzer::analyzeCacheMissPatterns() {
    // Simple cache analysis based on available data
    for (const auto& profile : pImpl->performanceHistory) {
        if (profile.network.spikeRate > 100.0) { // High spike rate
            BottleneckAnalysis bottleneck;
            bottleneck.bottleneckType = "cache";
            bottleneck.componentName = "spike_cache";
            bottleneck.severity = std::min(1.0, profile.network.spikeRate / 1000.0);
            bottleneck.impactOnThroughput = 25.0;
            bottleneck.estimatedImprovement = 35.0;
            bottleneck.rootCauses.push_back("Cache thrashing due to random access");
            bottleneck.rootCauses.push_back("Poor data locality");
            bottleneck.suggestedSolutions.push_back("Use cache-friendly data structures");
            bottleneck.suggestedSolutions.push_back("Implement spatial locality");
            bottleneck.suggestedSolutions.push_back("Use blocking algorithms");
            bottleneck.isResolved = false;
            
            pImpl->bottleneckAnalysis.push_back(bottleneck);
            break;
        }
    }
}

void PerformanceAnalyzer::analyzeParallelizationOpportunities() {
    // Analyze opportunities for parallelization
    for (const auto& [componentName, avgTime] : pImpl->componentExecutionTimes) {
        if (avgTime > 50.0) { // > 50ms
            OptimizationHint hint;
            hint.componentName = componentName;
            hint.optimizationType = "parallel";
            hint.description = "Parallelize " + componentName + " using threads or SIMD";
            hint.potentialImprovement = std::min(95.0, avgTime / 5.0); // Potential speedup
            hint.isImplemented = false;
            hint.isCritical = avgTime > 500.0; // > 500ms
            hint.suggestedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count() + 604800000; // 1 week from now
            
            pImpl->optimizationHints.push_back(hint);
        }
    }
}

void PerformanceAnalyzer::generatePerformanceReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().logError("Failed to open performance report file: " + filename);
        return;
    }
    
    file << "=== NLM Performance Analysis Report ===" << std::endl;
    file << "Generated: " << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count() << " ms" << std::endl;
    file << "Total Samples: " << pImpl->performanceHistory.size() << std::endl;
    file << "Bottlenecks Identified: " << pImpl->bottleneckAnalysis.size() << std::endl;
    file << "Optimization Hints: " << pImpl->optimizationHints.size() << std::endl;
    file << std::endl;
    
    // Performance summary
    file << "=== Performance Summary ===" << std::endl;
    if (!pImpl->performanceHistory.empty()) {
        double totalMemory = 0.0, totalSpikes = 0.0, totalCPU = 0.0;
        size_t count = 0;
        
        for (const auto& profile : pImpl->performanceHistory) {
            totalMemory += static_cast<double>(profile.memory.totalMemoryUsage);
            totalSpikes += profile.network.spikeRate;
            totalCPU += profile.io.cpuUtilization;
            count++;
        }
        
        if (count > 0) {
            file << "Average Memory Usage: " << (totalMemory / count) / 1024.0 / 1024.0 << " MB" << std::endl;
            file << "Average Spike Rate: " << (totalSpikes / count) << " Hz" << std::endl;
            file << "Average CPU Utilization: " << (totalCPU / count) << "%" << std::endl;
        }
    }
    file << std::endl;
    
    // Bottleneck analysis
    file << "=== Bottleneck Analysis ===" << std::endl;
    for (const auto& bottleneck : pImpl->bottleneckAnalysis) {
        file << "Type: " << bottleneck.bottleneckType << std::endl;
        file << "Component: " << bottleneck.componentName << std::endl;
        file << "Severity: " << std::fixed << std::setprecision(2) << bottleneck.severity << std::endl;
        file << "Impact on Throughput: " << bottleneck.impactOnThroughput << "%" << std::endl;
        file << "Estimated Improvement: " << bottleneck.estimatedImprovement << "%" << std::endl;
        file << "Root Causes: ";
        for (size_t i = 0; i < bottleneck.rootCauses.size(); ++i) {
            if (i > 0) file << ", ";
            file << bottleneck.rootCauses[i];
        }
        file << std::endl;
        file << "Suggested Solutions: ";
        for (size_t i = 0; i < bottleneck.suggestedSolutions.size(); ++i) {
            if (i > 0) file << ", ";
            file << bottleneck.suggestedSolutions[i];
        }
        file << std::endl;
        file << "Status: " << (bottleneck.isResolved ? "RESOLVED" : "ACTIVE") << std::endl;
        file << "---" << std::endl;
    }
    
    // Optimization hints
    file << "=== Optimization Hints ===" << std::endl;
    for (const auto& hint : pImpl->optimizationHints) {
        file << "Component: " << hint.componentName << std::endl;
        file << "Type: " << hint.optimizationType << std::endl;
        file << "Description: " << hint.description << std::endl;
        file << "Potential Improvement: " << std::fixed << std::setprecision(1) << hint.potentialImprovement << "%" << std::endl;
        file << "Critical: " << (hint.isCritical ? "YES" : "NO") << std::endl;
        file << "Implemented: " << (hint.isImplemented ? "YES" : "NO") << std::endl;
        file << "Suggested Time: " << hint.suggestedTime << std::endl;
        file << "---" << std::endl;
    }
    
    file.close();
    Logger::getInstance().logInfo("Performance analysis report generated: " + filename);
}

void PerformanceAnalyzer::exportPerformanceData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().logError("Failed to open performance data file: " + filename);
        return;
    }
    
    file << "timestamp,total_memory,allocated_memory,neurons,synapses,spike_rate,plasticity_factor,avg_cpu_usage" << std::endl;
    
    for (const auto& profile : pImpl->performanceHistory) {
        file << profile.timestamp << ","
             << profile.memory.totalMemoryUsage << ","
             << profile.memory.allocatedMemory << ","
             << profile.network.totalNeurons << ","
             << profile.network.totalSynapses << ","
             << std::fixed << std::setprecision(2) << profile.network.spikeRate << ","
             << profile.network.plasticityFactor << ","
             << profile.io.cpuUtilization << std::endl;
    }
    
    file.close();
    Logger::getInstance().logInfo("Performance data exported: " + filename);
}

void PerformanceAnalyzer::generateOptimizationPlan(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().logError("Failed to open optimization plan file: " + filename);
        return;
    }
    
    file << "=== NLM Optimization Plan ===" << std::endl;
    file << "Generated: " << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count() << " ms" << std::endl;
    file << std::endl;
    
    // Critical optimizations first
    file << "=== CRITICAL OPTIMIZATIONS ===" << std::endl;
    for (const auto& hint : pImpl->optimizationHints) {
        if (hint.isCritical && !hint.isImplemented) {
            file << hint.componentName << ": " << hint.description << std::endl;
            file << "  Potential improvement: " << std::fixed << std::setprecision(1) << hint.potentialImprovement << "%" << std::endl;
            file << "  Suggested time: " << hint.suggestedTime << std::endl;
            file << std::endl;
        }
    }
    
    // Bottlenecks that should be addressed
    file << "=== BOTTLENECK REMEDIATION ===" << std::endl;
    for (const auto& bottleneck : pImpl->bottleneckAnalysis) {
        if (!bottleneck.isResolved) {
            file << bottleneck.componentName << " (" << bottleneck.bottleneckType << ")" << std::endl;
            file << "  Severity: " << std::fixed << std::setprecision(2) << bottleneck.severity << std::endl;
            file << "  Impact: " << bottleneck.impactOnThroughput << "%" << std::endl;
            file << "  Solution: ";
            if (!bottleneck.suggestedSolutions.empty()) {
                file << bottleneck.suggestedSolutions[0];
                for (size_t i = 1; i < bottleneck.suggestedSolutions.size(); ++i) {
                    file << ", " << bottleneck.suggestedSolutions[i];
                }
            }
            file << std::endl;
            file << std::endl;
        }
    }
    
    // Estimated performance improvements
    double totalPotentialImprovement = 0.0;
    size_t criticalCount = 0;
    
    for (const auto& hint : pImpl->optimizationHints) {
        if (!hint.isImplemented && hint.isCritical) {
            totalPotentialImprovement += hint.potentialImprovement;
            criticalCount++;
        }
    }
    
    for (const auto& bottleneck : pImpl->bottleneckAnalysis) {
        if (!bottleneck.isResolved) {
            totalPotentialImprovement += bottleneck.estimatedImprovement;
        }
    }
    
    file << "=== SUMMARY ===" << std::endl;
    file << "Critical optimizations: " << criticalCount << std::endl;
    file << "Total estimated improvement: " << std::fixed << std::setprecision(1) << totalPotentialImprovement << "%" << std::endl;
    file << "Projected performance score: " << std::min(100.0, 50.0 + totalPotentialImprovement / 2.0) << "/100" << std::endl;
    
    file.close();
    Logger::getInstance().logInfo("Optimization plan generated: " + filename);
}

const PerformanceProfile& PerformanceAnalyzer::getCurrentProfile() const {
    return pImpl->currentProfile;
}

const std::vector<PerformanceProfile>& PerformanceAnalyzer::getPerformanceHistory() const {
    return pImpl->performanceHistory;
}

const std::vector<OptimizationHint>& PerformanceAnalyzer::getOptimizationHints() const {
    return pImpl->optimizationHints;
}

const std::vector<BottleneckAnalysis>& PerformanceAnalyzer::getBottleneckAnalysis() const {
    return pImpl->bottleneckAnalysis;
}

void PerformanceAnalyzer::setAnalysisInterval(double seconds) {
    pImpl->analysisInterval = seconds;
}

void PerformanceAnalyzer::setMaxHistoricalData(size_t maxDataPoints) {
    pImpl->maxHistoricalData = maxDataPoints;
}

void PerformanceAnalyzer::enableMemoryLeakDetection(bool enable) {
    pImpl->enableMemoryLeakDetection = enable;
}

void PerformanceAnalyzer::enableBottleneckDetection(bool enable) {
    pImpl->enableBottleneckDetection = enable;
}

void PerformanceAnalyzer::enableOptimizationSuggestions(bool enable) {
    pImpl->enableOptimizationSuggestions = enable;
}

size_t PerformanceAnalyzer::getSampleCount() const {
    return pImpl->performanceHistory.size();
}

size_t PerformanceAnalyzer::getBottleneckCount() const {
    return pImpl->bottleneckAnalysis.size();
}

size_t PerformanceAnalyzer::getOptimizationHintCount() const {
    return pImpl->optimizationHints.size();
}

} // namespace nlm
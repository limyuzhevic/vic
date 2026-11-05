// Performance Monitor Implementation
// Detailed implementation of performance monitoring tools for NLM

#include "PerformanceMonitor.hpp"
#include <numeric>
#include <algorithm>
#include <iostream>
#include <cmath>

namespace nlm {

// Performance Monitor Implementation Details

PerformanceMonitor::PerformanceMonitor() : isMonitoring(false) {
    sessionId = "perf_" + std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    currentMetrics.totalTime = 0.0;
    currentMetrics.cpuTime = 0.0;
    currentMetrics.memoryUsage = 0.0;
    currentMetrics.totalSteps = 0;
    currentMetrics.totalSpikes = 0;
    currentMetrics.neuronCount = 0;
    currentMetrics.synapseCount = 0;
    currentMetrics.stepsPerSecond = 0.0;
    currentMetrics.spikesPerSecond = 0.0;
    currentMetrics.efficiency = 0.0;
    currentMetrics.throughput = 0.0;
    currentMetrics.scalability = 0.0;
}

void PerformanceMonitor::calculateFinalMetrics() {
    // Calculate duration from time measurements
    double totalDuration = 0.0;
    for (const auto& tm : timeMeasurements) {
        if (!tm.isRunning) { // Only stopped measurements
            totalDuration += tm.duration;
        }
    }
    currentMetrics.totalTime = totalDuration;
    
    if (totalDuration > 0.0) {
        currentMetrics.stepsPerSecond = static_cast<double>(currentMetrics.totalSteps) / totalDuration;
        currentMetrics.spikesPerSecond = static_cast<double>(currentMetrics.totalSpikes) / totalDuration;
    }
    
    calculateEfficiency();
    calculateThroughput();
    calculateScalability();
}

double PerformanceMonitor::calculateAverageMemory() const {
    if (memoryProfile.samples.empty()) {
        return 0.0;
    }
    double sum = std::accumulate(memoryProfile.samples.begin(), memoryProfile.samples.end(), 0.0);
    return sum / memoryProfile.samples.size();
}

void PerformanceMonitor::calculateEfficiency() {
    // Efficiency is based on steps per second relative to theoretical maximum
    double theoreticalMaxStepsPerSecond = 1000000.0; // Assumed maximum
    currentMetrics.efficiency = (currentMetrics.stepsPerSecond / theoreticalMaxStepsPerSecond) * 100.0;
    
    // Clamp efficiency to reasonable bounds
    if (currentMetrics.efficiency < 0.0) currentMetrics.efficiency = 0.0;
    if (currentMetrics.efficiency > 100.0) currentMetrics.efficiency = 100.0;
}

void PerformanceMonitor::calculateThroughput() {
    // Throughput considers both computational and memory operations
    double computationalThroughput = currentMetrics.stepsPerSecond * static_cast<double>(currentMetrics.synapseCount);
    double memoryThroughput = currentMetrics.spikesPerSecond * 10.0; // Estimate for spike processing
    currentMetrics.throughput = computationalThroughput + memoryThroughput;
}

void PerformanceMonitor::calculateScalability() {
    // Simple scalability calculation based on synapse density
    if (currentMetrics.neuronCount > 0) {
        double synapseDensity = static_cast<double>(currentMetrics.synapseCount) / currentMetrics.neuronCount;
        currentMetrics.scalability = std::min(synapseDensity / 100.0, 1.0); // Normalize to 1.0
    } else {
        currentMetrics.scalability = 0.0;
    }
}

// Convenience function implementations
void startPerformanceMonitoring() {
    globalPerformanceMonitor.startMonitoring();
}

void stopPerformanceMonitoring() {
    globalPerformanceMonitor.stopMonitoring();
}

void recordPerformanceMetrics(size_t neurons, size_t synapses, size_t steps, size_t spikes, double memoryMB, double timestep) {
    globalPerformanceMonitor.updateBrainStats(neurons, synapses, steps);
    globalPerformanceMonitor.recordSpikeCount(spikes, timestep);
    globalPerformanceMonitor.updateMemoryUsage(memoryMB);
}

void generatePerformanceReport(const std::string& filename) {
    globalPerformanceMonitor.generateReport(filename);
}

std::string getPerformanceSessionId() {
    return globalPerformanceMonitor.getSessionId();
}

PerformanceMetrics getCurrentPerformanceMetrics() {
    return globalPerformanceMonitor.getMetrics();
}

// PerformanceBatchAnalyzer implementation
PerformanceBatchAnalyzer::PerformanceBatchAnalyzer() {
    batchId = "batch_" + std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

void PerformanceBatchAnalyzer::addRunMetrics(const PerformanceMetrics& metrics) {
    runMetrics.push_back(metrics);
}

void PerformanceBatchAnalyzer::generateBatchReport(const std::string& filename) {
    std::ofstream reportFile(filename);
    if (!reportFile.is_open()) {
        std::cerr << "Warning: Could not open performance report file: " << filename << std::endl;
        return;
    }
    
    reportFile << "NLM Performance Batch Analysis" << std::endl;
    reportFile << "==============================" << std::endl;
    reportFile << "Batch ID: " << batchId << std::endl;
    reportFile << "Number of Runs: " << runMetrics.size() << std::endl;
    
    if (runMetrics.empty()) {
        reportFile.close();
        return;
    }
    
    // Calculate statistics
    double avgStepsPerSec = 0.0;
    double avgSpikesPerSec = 0.0;
    double avgMemory = 0.0;
    double avgEfficiency = 0.0;
    double avgThroughput = 0.0;
    double avgScalability = 0.0;
    
    for (const auto& metrics : runMetrics) {
        avgStepsPerSec += metrics.stepsPerSecond;
        avgSpikesPerSec += metrics.spikesPerSecond;
        avgMemory += metrics.memoryUsage;
        avgEfficiency += metrics.efficiency;
        avgThroughput += metrics.throughput;
        avgScalability += metrics.scalability;
    }
    
    size_t count = runMetrics.size();
    avgStepsPerSec /= count;
    avgSpikesPerSec /= count;
    avgMemory /= count;
    avgEfficiency /= count;
    avgThroughput /= count;
    avgScalability /= count;
    
    reportFile << "\nAverage Metrics:" << std::endl;
    reportFile << "  Steps per Second: " << avgStepsPerSec << std::endl;
    reportFile << "  Spikes per Second: " << avgSpikesPerSec << std::endl;
    reportFile << "  Memory Usage: " << avgMemory << " MB" << std::endl;
    reportFile << "  Efficiency: " << avgEfficiency << "%" << std::endl;
    reportFile << "  Throughput: " << avgThroughput << " ops/second" << std::endl;
    reportFile << "  Scalability: " << avgScalability << std::endl;
    
    // Individual run details
    reportFile << "\nIndividual Run Details:" << std::endl;
    for (size_t i = 0; i < runMetrics.size(); ++i) {
        reportFile << "  Run " << i + 1 << ": "
                  << "Steps/sec: " << runMetrics[i].stepsPerSecond
                  << ", Spikes/sec: " << runMetrics[i].spikesPerSecond
                  << ", Memory: " << runMetrics[i].memoryUsage << " MB"
                  << ", Efficiency: " << runMetrics[i].efficiency << "%"
                  << std::endl;
    }
    
    // Identify outliers and bottlenecks
    reportFile << "\nAnalysis:" << std::endl;
    reportFile << "  Fastest Run: " << findFastestRun() << std::endl;
    reportFile << "  Most Memory Efficient: " << findMostMemoryEfficient() << std::endl;
    reportFile << "  Most Scalable: " << findMostScalable() << std::endl;
    
    // Performance trend analysis
    reportFile << "  Performance Trend: " << analyzeTrend() << std::endl;
    
    // Recommendations
    reportFile << "  Recommendations: " << generateRecommendations() << std::endl;
    
    reportFile.close();
}

std::string PerformanceBatchAnalyzer::findFastestRun() const {
    if (runMetrics.empty()) return "N/A";
    
    size_t fastestIdx = 0;
    double fastestSpeed = runMetrics[0].stepsPerSecond;
    
    for (size_t i = 1; i < runMetrics.size(); ++i) {
        if (runMetrics[i].stepsPerSecond > fastestSpeed) {
            fastestSpeed = runMetrics[i].stepsPerSecond;
            fastestIdx = i;
        }
    }
    
    return "Run " + std::to_string(fastestIdx + 1) + " (" + std::to_string(fastestSpeed) + " steps/sec)";
}

std::string PerformanceBatchAnalyzer::findMostMemoryEfficient() const {
    if (runMetrics.empty()) return "N/A";
    
    size_t efficientIdx = 0;
    double mostEfficient = runMetrics[0].memoryUsage;
    
    for (size_t i = 1; i < runMetrics.size(); ++i) {
        if (runMetrics[i].memoryUsage < mostEfficient) {
            mostEfficient = runMetrics[i].memoryUsage;
            efficientIdx = i;
        }
    }
    
    return "Run " + std::to_string(efficientIdx + 1) + " (" + std::to_string(mostEfficient) + " MB)";
}

std::string PerformanceBatchAnalyzer::findMostScalable() const {
    if (runMetrics.empty()) return "N/A";
    
    size_t mostScalableIdx = 0;
    double mostScalable = runMetrics[0].scalability;
    
    for (size_t i = 1; i < runMetrics.size(); ++i) {
        if (runMetrics[i].scalability > mostScalable) {
            mostScalable = runMetrics[i].scalability;
            mostScalableIdx = i;
        }
    }
    
    return "Run " + std::to_string(mostScalableIdx + 1) + " (" + std::to_string(mostScalable * 100) + "% scalability)";
}

std::string PerformanceBatchAnalyzer::analyzeTrend() const {
    if (runMetrics.size() < 2) {
        return "Insufficient data for trend analysis";
    }
    
    // Simple trend analysis based on steps per second
    double firstHalfAvg = 0.0;
    double secondHalfAvg = 0.0;
    size_t halfSize = runMetrics.size() / 2;
    
    for (size_t i = 0; i < halfSize; ++i) {
        firstHalfAvg += runMetrics[i].stepsPerSecond;
        secondHalfAvg += runMetrics[i + halfSize].stepsPerSecond;
    }
    
    firstHalfAvg /= halfSize;
    secondHalfAvg /= halfSize;
    
    if (secondHalfAvg > firstHalfAvg * 1.1) {
        return "Improving trend (performance increasing)";
    } else if (secondHalfAvg < firstHalfAvg * 0.9) {
        return "Degrading trend (performance decreasing)";
    } else {
        return "Stable trend (performance consistent)";
    }
}

std::string PerformanceBatchAnalyzer::generateRecommendations() const {
    if (runMetrics.empty()) {
        return "No recommendations available";
    }
    
    std::vector<std::string> recommendations;
    
    // Find runs with poor efficiency
    for (size_t i = 0; i < runMetrics.size(); ++i) {
        if (runMetrics[i].efficiency < 50.0) {
            recommendations.push_back("Consider optimizing run " + std::to_string(i + 1) +
                                    " for better efficiency (current: " +
                                    std::to_string(runMetrics[i].efficiency) + "%)");
        }
    }
    
    // Find runs with high memory usage
    for (size_t i = 0; i < runMetrics.size(); ++i) {
        if (runMetrics[i].memoryUsage > 200.0) {
            recommendations.push_back("Consider memory optimization for run " +
                                    std::to_string(i + 1) +
                                    " (current: " +
                                    std::to_string(runMetrics[i].memoryUsage) + " MB)");
        }
    }
    
    if (recommendations.empty()) {
        return "All runs performing well. Consider scaling up for production.";
    }
    
    // Return top recommendation
    return recommendations[0];
}

// Example usage implementations
void examplePerformanceMonitoring() {
    std::cout << "=== NLM Performance Monitoring Example ===" << std::endl;
    
    // Start monitoring
    startPerformanceMonitoring();
    
    // Simulate some work with realistic brain metrics
    for (int i = 0; i < 1000; ++i) {
        // Simulate brain operation with realistic parameters
        size_t neurons = 1000;
        size_t synapses = neurons * 10; // Average connectivity
        size_t steps = i;
        size_t spikes = (i % 100) + 10; // Variable spike count
        double memory = 100.0 + (neurons * 0.1) + (synapses * 0.01); // Memory estimate
        double timestep = 0.001;
        
        // Record performance metrics
        recordPerformanceMetrics(neurons, synapses, steps, spikes, memory, timestep);
        
        if (i % 200 == 0) {
            PerformanceMetrics current = getCurrentPerformanceMetrics();
            std::cout << "Step " << i << ": "
                      << "Efficiency: " << current.efficiency << "%"
                      << ", Throughput: " << current.throughput << " ops/sec"
                      << ", Memory: " << current.memoryUsage << " MB"
                      << std::endl;
        }
    }
    
    // Stop monitoring and generate report
    stopPerformanceMonitoring();
    generatePerformanceReport("nlm_performance_report.txt");
    
    std::cout << "\nPerformance monitoring completed!" << std::endl;
    std::cout << "Report saved to: nlm_performance_report.txt" << std::endl;
    std::cout << "Session ID: " << getPerformanceSessionId() << std::endl;
}

void exampleBatchAnalysis() {
    std::cout << "=== NLM Performance Batch Analysis Example ===" << std::endl;
    
    PerformanceBatchAnalyzer analyzer;
    
    // Simulate multiple runs with different parameters (simulating different scenarios)
    for (int run = 0; run < 5; ++run) {
        PerformanceMetrics metrics;
        metrics.totalSteps = (run + 1) * 1000;
        metrics.totalSpikes = (run + 1) * 500;
        metrics.stepsPerSecond = static_cast<double>(metrics.totalSteps) / 10.0;
        metrics.spikesPerSecond = static_cast<double>(metrics.totalSpikes) / 10.0;
        metrics.memoryUsage = 100.0 + (run * 20);
        metrics.efficiency = 85.0 - (run * 5);
        metrics.throughput = metrics.stepsPerSecond * 100.0;
        metrics.scalability = 0.5 + (run * 0.1);
        
        analyzer.addRunMetrics(metrics);
        
        std::cout << "Completed run " << run + 1 << ": "
                  << metrics.stepsPerSecond << " steps/sec"
                  << ", Efficiency: " << metrics.efficiency << "%"
                  << ", Memory: " << metrics.memoryUsage << " MB"
                  << std::endl;
    }
    
    // Generate comprehensive batch analysis
    analyzer.generateBatchReport("nlm_batch_performance_report.txt");
    
    std::cout << "\nBatch analysis completed!" << std::endl;
    std::cout << "Report saved to: nlm_batch_performance_report.txt" << std::endl;
}

void exampleRealTimeMonitoring() {
    std::cout << "=== NLM Real-Time Performance Monitoring Example ===" << std::endl;
    
    // Start real-time monitoring
    startPerformanceMonitoring();
    
    std::cout << "Real-time monitoring started. Simulating real-time updates..." << std::endl;
    
    // Simulate real-time monitoring updates
    for (int i = 0; i < 10; ++i) {
        // Simulate real-time brain metrics
        size_t neurons = 500 + (i * 100);
        size_t synapses = neurons * 8;
        size_t steps = (i + 1) * 500;
        size_t spikes = (i + 1) * 250;
        double memory = 80.0 + (neurons * 0.05);
        double timestep = 0.001;
        
        // Update metrics in real-time
        recordPerformanceMetrics(neurons, synapses, steps, spikes, memory, timestep);
        
        // Display real-time metrics
        PerformanceMetrics current = getCurrentPerformanceMetrics();
        std::cout << "\rUpdate " << i + 1 << ": "
                  << "Efficiency: " << current.efficiency << "%"
                  << " | Throughput: " << current.throughput << " ops/sec"
                  << " | Memory: " << current.memoryUsage << " MB"
                  << " | Steps/sec: " << current.stepsPerSecond << std::flush;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Stop monitoring
    stopPerformanceMonitoring();
    
    // Generate final report
    generatePerformanceReport("nlm_realtime_performance_report.txt");
    
    std::cout << "\n\nReal-time monitoring completed!" << std::endl;
    std::cout << "Final report: nlm_realtime_performance_report.txt" << std::endl;
}

} // namespace nlm

// Thread support for real-time monitoring (if available)
#ifdef _WIN32
    #include <windows.h>
#else
    #include <thread>
    #include <chrono>
#endif

// Cross-platform sleep function
inline void sleepFor(double seconds) {
#ifdef _WIN32
    Sleep(static_cast<DWORD>(seconds * 1000));
#else
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
#endif
}

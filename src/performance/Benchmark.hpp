#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace nlm {

struct BenchmarkResult {
    std::string testName;
    double executionTimeMs;
    double stepsPerSecond;
    double avgFiringRate;
    double memoryUsageMB;
    double cpuUsagePercent;
    size_t totalSpikesGenerated;
    size_t neuronsActive;
    
    BenchmarkResult(const std::string& name) 
        : testName(name)
        , executionTimeMs(0.0)
        , stepsPerSecond(0.0)
        , avgFiringRate(0.0)
        , memoryUsageMB(0.0)
        , cpuUsagePercent(0.0)
        , totalSpikesGenerated(0)
        , neuronsActive(0) {}
};

class PerformanceProfiler {
public:
    PerformanceProfiler() {}
    ~PerformanceProfiler() {}
    
    // Start profiling a test
    void startBenchmark(const std::string& testName) {
        currentTestName = testName;
        startTime = std::chrono::high_resolution_clock::now();
        
        // Initialize metrics
        spikeCounts.clear();
        neuronActivityHistory.clear();
        stepCount = 0;
        totalSpikes = 0;
        maxActiveNeurons = 0;
        
        NLM_LOG_INFO("Benchmark started: " + testName);
    }
    
    // Stop profiling and return results
    BenchmarkResult stopBenchmark() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        BenchmarkResult result(currentTestName);
        result.executionTimeMs = duration.count();
        
        if (duration.count() > 0) {
            result.stepsPerSecond = (stepCount * 1000.0) / duration.count();
        }
        
        if (spikeCounts.empty()) {
            result.totalSpikesGenerated = totalSpikes;
        } else {
            result.totalSpikesGenerated = totalSpikes;
        }
        
        result.neuronsActive = maxActiveNeurons;
        result.avgFiringRate = spikeCounts.empty() ? 0.0 : calculateAverageFiringRate();
        
        NLM_LOG_INFO("Benchmark completed: " + currentTestName);
        NLM_LOG_INFO("  Execution time: " + std::to_string(result.executionTimeMs) + " ms");
        NLM_LOG_INFO("  Steps per second: " + std::to_string(result.stepsPerSecond));
        
        return result;
    }
    
    // Record metrics during benchmark
    void recordStep(const Brain& brain, size_t step) {
        stepCount = step;
        
        // Record spike activity
        size_t spikesThisStep = brain.getFiringNeuronCount();
        totalSpikes += spikesThisStep;
        spikeCounts.push_back(spikesThisStep);
        
        // Record neuron activity
        size_t activeNeurons = brain.getActiveNeuronCount();
        maxActiveNeurons = std::max(maxActiveNeurons, activeNeurons);
        neuronActivityHistory.push_back(static_cast<double>(activeNeurons));
        
        // Log periodically
        if (step % 100 == 0) {
            NLM_LOG_INFO("Benchmark step " + std::to_string(step) + 
                        ": " + std::to_string(spikesThisStep) + " spikes, " +
                        std::to_string(activeNeurons) + " active neurons");
        }
    }
    
    // Calculate average firing rate
    double calculateAverageFiringRate() {
        if (spikeCounts.empty()) return 0.0;
        double total = std::accumulate(spikeCounts.begin(), spikeCounts.end(), 0.0);
        return total / spikeCounts.size();
    }
    
    // Get current step count
    size_t getStepCount() const { return stepCount; }
    
    // Get total spikes recorded
    size_t getTotalSpikes() const { return totalSpikes; }
    
private:
    std::string currentTestName;
    std::chrono::high_resolution_clock::time_point startTime;
    
    std::vector<size_t> spikeCounts;
    std::vector<double> neuronActivityHistory;
    
    size_t stepCount;
    size_t totalSpikes;
    size_t maxActiveNeurons;
};

class PerformanceAnalysis {
public:
    PerformanceAnalysis() {}
    ~PerformanceAnalysis() {}
    
    // Compare performance across different results
    std::string compareResults(const std::vector<BenchmarkResult>& results) {
        std::stringstream ss;
        
        ss << "PERFORMANCE COMPARISON REPORT" << std::endl;
        ss << "=================================" << std::endl;
        
        if (results.empty()) {
            ss << "No results to compare." << std::endl;
            return ss.str();
        }
        
        // Find best and worst performers
        auto bestStepsPerSec = std::max_element(results.begin(), results.end(),
                                              [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                                  return a.stepsPerSecond < b.stepsPerSecond;
                                              });
        
        auto worstStepsPerSec = std::min_element(results.begin(), results.end(),
                                               [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                                   return a.stepsPerSecond < b.stepsPerSecond;
                                               });
        
        auto bestFiringRate = std::max_element(results.begin(), results.end(),
                                             [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                                 return a.avgFiringRate < b.avgFiringRate;
                                             });
        
        auto worstFiringRate = std::min_element(results.begin(), results.end(),
                                              [](const BenchmarkResult& a, const BenchmarkResult& b) {
                                                  return a.avgFiringRate < b.avgFiringRate;
                                              });
        
        ss << "Best Performer Analysis:" << std::endl;
        ss << "  Steps/sec: " << bestStepsPerSec->stepsPerSecond << " (" << bestStepsPerSec->testName << ")" << std::endl;
        ss << "  Avg firing rate: " << bestFiringRate->avgFiringRate << " Hz" << std::endl;
        ss << "  Total spikes: " << bestStepsPerSec->totalSpikesGenerated << std::endl;
        ss << "  Execution time: " << bestStepsPerSec->executionTimeMs << " ms" << std::endl;
        
        ss << std::endl;
        
        ss << "Worst Performer Analysis:" << std::endl;
        ss << "  Steps/sec: " << worstStepsPerSec->stepsPerSecond << " (" << worstStepsPerSec->testName << ")" << std::endl;
        ss << "  Avg firing rate: " << worstFiringRate->avgFiringRate << " Hz" << std::endl;
        ss << "  Total spikes: " << worstStepsPerSec->totalSpikesGenerated << std::endl;
        ss << "  Execution time: " << worstStepsPerSec->executionTimeMs << " ms" << std::endl;
        
        ss << std::endl;
        
        ss << "Performance Ratios:" << std::endl;
        double stepsPerSecRatio = bestStepsPerSec->stepsPerSecond / worstStepsPerSec->stepsPerSecond;
        double firingRateRatio = bestFiringRate->avgFiringRate / worstFiringRate->avgFiringRate;
        
        ss << "  Speed ratio (best/worst): " << std::fixed << std::setprecision(2) << stepsPerSecRatio << "x" << std::endl;
        ss << "  Firing rate ratio (best/worst): " << std::fixed << std::setprecision(2) << firingRateRatio << "x" << std::endl;
        
        ss << std::endl;
        
        ss << "Detailed Results:" << std::endl;
        ss << std::left << std::setw(25) << "Test" << std::setw(15) << "Time (ms)" 
           << std::setw(15) << "Steps/sec" << std::setw(15) << "Avg Firing" 
           << std::setw(15) << "Total Spikes" << std::endl;
        ss << std::string(85, '-') << std::endl;
        
        for (const auto& result : results) {
            ss << std::left << std::setw(25) << result.testName 
               << std::setw(15) << std::fixed << std::setprecision(2) << result.executionTimeMs
               << std::setw(15) << std::fixed << std::setprecision(2) << result.stepsPerSecond
               << std::setw(15) << std::fixed << std::setprecision(3) << result.avgFiringRate
               << std::setw(15) << result.totalSpikesGenerated << std::endl;
        }
        
        return ss.str();
    }
    
    // Analyze memory efficiency
    std::string analyzeMemoryEfficiency(const std::vector<BenchmarkResult>& results) {
        std::stringstream ss;
        
        ss << "MEMORY EFFICIENCY ANALYSIS" << std::endl;
        ss << "============================" << std::endl;
        
        if (results.empty()) {
            ss << "No results to analyze." << std::endl;
            return ss.str();
        }
        
        ss << std::left << std::setw(25) << "Test" << std::setw(15) << "Memory (MB)" 
           << std::setw(15) << "CPU (Percent)"
           << std::setw(15) << "Steps/sec/MB"
           << std::setw(15) << "Spikes/sec/MB" << std::endl;
        ss << std::string(75, '-') << std::endl;
        
        double maxMemory = 0.0;
        for (const auto& result : results) {
            maxMemory = std::max(maxMemory, result.memoryUsageMB);
        }
        
        for (const auto& result : results) {
            double stepsPerMB = maxMemory > 0 ? result.stepsPerSecond / maxMemory : 0.0;
            double spikesPerMB = maxMemory > 0 ? result.totalSpikesGenerated / maxMemory : 0.0;
            
            ss << std::left << std::setw(25) << result.testName 
               << std::setw(15) << std::fixed << std::setprecision(2) << result.memoryUsageMB
               << std::setw(15) << std::fixed << std::setprecision(1) << result.cpuUsagePercent
               << std::setw(15) << std::fixed << std::setprecision(2) << stepsPerMB
               << std::setw(15) << std::fixed << std::setprecision(1) << spikesPerMB << std::endl;
        }
        
        return ss.str();
    }
    
    // Generate performance summary report
    std::string generatePerformanceReport(const std::vector<BenchmarkResult>& results, 
                                          const std::string& title = "PERFORMANCE REPORT") {
        std::stringstream ss;
        
        ss << "=" << std::string(60, '=') << std::endl;
        ss << "  " << title << std::endl;
        ss << "=" << std::string(60, '=') << std::endl;
        
        if (results.empty()) {
            ss << "No benchmark results available." << std::endl;
            return ss.str();
        }
        
        // Summary statistics
        double totalTime = 0.0;
        double totalStepsPerSec = 0.0;
        double totalSpikes = 0.0;
        
        for (const auto& result : results) {
            totalTime += result.executionTimeMs;
            totalStepsPerSec += result.stepsPerSecond;
            totalSpikes += result.totalSpikesGenerated;
        }
        
        size_t numTests = results.size();
        
        ss << "SUMMARY STATISTICS:" << std::endl;
        ss << "  Tests executed: " << numTests << std::endl;
        ss << "  Average execution time: " << std::fixed << std::setprecision(2) 
           << (totalTime / numTests) << " ms" << std::endl;
        ss << "  Average steps per second: " << std::fixed << std::setprecision(2) 
           << (totalStepsPerSec / numTests) << std::endl;
        ss << "  Average spikes per test: " << std::fixed << std::setprecision(1) 
           << (totalSpikes / numTests) << std::endl;
        
        ss << std::endl;
        
        // Performance characteristics
        ss << "PERFORMANCE CHARACTERISTICS:" << std::endl;
        ss << "  Scale efficiency: " << std::fixed << std::setprecision(2) 
           << (totalStepsPerSec / numTests) / 1000.0 << " (relative to baseline)" << std::endl;
        
        double avgMemory = totalSpikes / numTests;
        ss << "  Average memory efficiency: " << std::fixed << std::setprecision(3) 
           << avgMemory << " spikes per unit memory" << std::endl;
        
        ss << std::endl;
        
        // Recommendations
        ss << "PERFORMANCE RECOMMENDATIONS:" << std::endl;
        
        // Check for bottlenecks
        bool hasSlowTests = false;
        for (const auto& result : results) {
            if (result.executionTimeMs > 1000.0) {  // > 1 second
                hasSlowTests = true;
                break;
            }
        }
        
        if (hasSlowTests) {
            ss << "  ⚠️  Some tests are taking longer than expected (>1s). Consider:" << std::endl;
            ss << "     - Reducing simulation steps" << std::endl;
            ss << "     - Optimizing neuron parameters" << std::endl;
            ss << "     - Checking for performance bottlenecks" << std::endl;
        }
        
        bool hasLowSpeed = false;
        for (const auto& result : results) {
            if (result.stepsPerSecond < 1000.0) {  // < 1000 steps/sec
                hasLowSpeed = true;
                break;
            }
        }
        
        if (hasLowSpeed) {
            ss << "  ⚠️  Low simulation speed detected (<1000 steps/sec). Consider:" << std::endl;
            ss << "     - Using release mode compilation" << std::endl;
            ss << "     - Increasing optimization level" << std::endl;
            ss << "     - Reducing model complexity" << std::endl;
        }
        
        // Performance optimization tips
        ss << "  💡 General optimization tips:" << std::endl;
        ss << "     - Use smaller models for faster prototyping" << std::endl;
        ss << "     - Enable parallel processing if available" << std::endl;
        ss << "     - Consider using SIMD optimizations for neuron updates" << std::endl;
        ss << "     - Profile individual components to identify bottlenecks" << std::endl;
        
        return ss.str();
    }
};

} // namespace nlm

#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>

namespace nlm {

class SimulationProfiler {
public:
    SimulationProfiler() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~SimulationProfiler() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << std::endl;
        std::cout << "=== SIMULATION PROFILING ===" << std::endl;
        std::cout << "Total simulation time: " << duration.count() << " ms" << std::endl;
        std::cout << "===============================" << std::endl;
    }
    
    void addTiming(const std::string& label, std::chrono::milliseconds duration) {
        timings.push_back({label, duration});
    }
    
    void printTimings() const {
        if (timings.empty()) return;
        
        std::cout << "Detailed timings:" << std::endl;
        
        double totalMs = 0.0;
        for (const auto& timing : timings) {
            totalMs += timing.duration.count();
        }
        
        for (const auto& timing : timings) {
            double percentage = (timing.duration.count() / totalMs) * 100.0;
            std::cout << "  " << std::left << std::setw(30) << timing.label 
                      << " : " << std::setw(8) << timing.duration.count() << " ms ("
                      << std::fixed << std::setprecision(1) << percentage << "%)" << std::endl;
        }
    }
    
    void clear() {
        timings.clear();
    }
    
private:
    struct TimingEntry {
        std::string label;
        std::chrono::milliseconds duration;
    };
    
    std::chrono::high_resolution_clock::time_point startTime;
    std::vector<TimingEntry> timings;
};

class DetailedSimulationProfiler {
public:
    DetailedSimulationProfiler() {
        startTime = std::chrono::high_resolution_clock::now();
        stepCounts = {0, 0, 0}; // connectivity, plasticity, stdp
    }
    
    ~DetailedSimulationProfiler() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << std::endl;
        std::cout << "=== DETAILED SIMULATION PROFILING ===" << std::endl;
        std::cout << "Connectivity test steps: " << stepCounts[0] << std::endl;
        std::cout << "Plasticity test steps: " << stepCounts[1] << std::endl;
        std::cout << "STDP test steps: " << stepCounts[2] << std::endl;
        std::cout << "Total simulation time: " << duration.count() << " ms" << std::endl;
        
        if (duration.count() > 0) {
            std::cout << "Average time per step: " 
                      << (duration.count() / (stepCounts[0] + stepCounts[1] + stepCounts[2])) 
                      << " ms" << std::endl;
        }
        
        std::cout << "====================================" << std::endl;
    }
    
    void addConnectivityStep(size_t steps) {
        stepCounts[0] += steps;
        totalSteps += steps;
    }
    
    void addPlasticityStep(size_t steps) {
        stepCounts[1] += steps;
        totalSteps += steps;
    }
    
    void addStdpStep(size_t steps) {
        stepCounts[2] += steps;
        totalSteps += steps;
    }
    
    void printPerformanceMetrics() const {
        std::cout << std::endl;
        std::cout << "=== PERFORMANCE METRICS ===" << std::endl;
        
        if (totalSteps > 0) {
            std::cout << "Steps per second: " << (totalSteps * 1000.0 / getTotalTime().count()) << std::endl;
            std::cout << "Average step time: " << (getTotalTime().count() / totalSteps) << " ms" << std::endl;
        }
        
        // Performance grade based on total time
        auto totalTime = getTotalTime();
        if (totalTime.count() < 1000) {
            std::cout << "Performance: Excellent (< 1s)" << std::endl;
        } else if (totalTime.count() < 5000) {
            std::cout << "Performance: Good (< 5s)" << std::endl;
        } else if (totalTime.count() < 10000) {
            std::cout << "Performance: Acceptable (< 10s)" << std::endl;
        } else {
            std::cout << "Performance: Needs optimization (> 10s)" << std::endl;
        }
        
        std::cout << "================================" << std::endl;
    }
    
    std::chrono::milliseconds getTotalTime() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - startTime
        );
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
    size_t stepCounts[3]; // connectivity, plasticity, stdp
    size_t totalSteps;
};

} // namespace nlm
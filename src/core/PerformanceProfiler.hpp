// Performance profiling utilities for NLM
#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

namespace nlm {

class PerformanceProfiler {
public:
    struct ProfilingData {
        std::string operationName;
        double totalTimeMs;
        double averageTimeMs;
        double minTimeMs;
        double maxTimeMs;
        uint64_t callCount;
        double totalCpuTimeMs;
        double efficiencyScore; // spikes per ms
    };
    
    PerformanceProfiler() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    void startProfiling(const std::string& operationName) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = now - startTime;
        
        if (activeProfiles.find(operationName) != activeProfiles.end()) {
            std::cerr << "Warning: Already profiling " << operationName << std::endl;
            return;
        }
        
        ProfileData data;
        data.operationName = operationName;
        data.startTime = now;
        data.startCpuTime = std::clock();
        activeProfiles[operationName] = data;
    }
    
    void stopProfiling(const std::string& operationName) {
        auto it = activeProfiles.find(operationName);
        if (it == activeProfiles.end()) {
            std::cerr << "Warning: No active profiling for " << operationName << std::endl;
            return;
        }
        
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = now - it->second.startTime;
        
        clock_t cpuTime = std::clock() - it->second.startCpuTime;
        double cpuTimeMs = static_cast<double>(cpuTime) / CLOCKS_PER_SEC * 1000.0;
        
        ProfileData& data = it->second;
        data.totalTimeMs += elapsed.count();
        data.totalCpuTimeMs += cpuTimeMs;
        data.callCount++;
        
        if (data.callCount == 1) {
            data.minTimeMs = elapsed.count();
            data.maxTimeMs = elapsed.count();
        } else {
            data.minTimeMs = std::min(data.minTimeMs, elapsed.count());
            data.maxTimeMs = std::max(data.maxTimeMs, elapsed.count());
        }
        
        data.averageTimeMs = data.totalTimeMs / data.callCount;
        
        activeProfiles.erase(it);
    }
    
    const ProfileData& getProfileData(const std::string& operationName) const {
        static ProfileData emptyData = {"", 0.0, 0.0, 0.0, 0.0, 0, 0.0, 0.0};
        auto it = completedProfiles.find(operationName);
        if (it != completedProfiles.end()) {
            return it->second;
        }
        return emptyData;
    }
    
    void completeProfile(const std::string& operationName, double efficiencyScore = 0.0) {
        if (activeProfiles.find(operationName) == activeProfiles.end()) {
            return; // Profile was already stopped manually
        }
        
        stopProfiling(operationName);
        
        // Transfer to completed profiles
        auto it = activeProfiles.find(operationName);
        if (it != activeProfiles.end()) {
            completedProfiles[operationName] = it->second;
            completedProfiles[operationName].efficiencyScore = efficiencyScore;
        }
    }
    
    void printSummary() const {
        std::cout << "=== NLM Performance Profile Summary ===" << std::endl;
        std::cout << "Total profiling duration: " << 
            std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startTime).count() / 1000.0 << " seconds" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Operation Name          | Calls | Avg Time (ms) | Min Time (ms) | Max Time (ms) | Total CPU (ms) | Efficiency" << std::endl;
        std::cout << "------------------------|-------|---------------|---------------|---------------|----------------|-----------" << std::endl;
        
        for (const auto& pair : completedProfiles) {
            const ProfileData& data = pair.second;
            std::cout << std::left << std::setw(24) << data.operationName 
                      << " | " << std::setw(5) << data.callCount
                      << " | " << std::setw(15) << std::fixed << std::setprecision(3) << data.averageTimeMs
                      << " | " << std::setw(15) << std::fixed << std::setprecision(3) << data.minTimeMs
                      << " | " << std::setw(15) << std::fixed << std::setprecision(3) << data.maxTimeMs
                      << " | " << std::setw(16) << std::fixed << std::setprecision(3) << data.totalCpuTimeMs
                      << " | " << std::fixed << std::setprecision(3) << data.efficiencyScore << std::endl;
        }
        
        if (!activeProfiles.empty()) {
            std::cout << std::endl << "Currently active profiles:" << std::endl;
            for (const auto& pair : activeProfiles) {
                std::cout << "  " << pair.first << " (started " << 
                    std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - pair.second.startTime).count() / 1000.0 << " seconds ago)" << std::endl;
            }
        }
        
        std::cout << "=====================================" << std::endl;
    }
    
    void reset() {
        activeProfiles.clear();
        completedProfiles.clear();
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    // Get total number of profiles
    size_t getProfileCount() const {
        return completedProfiles.size() + activeProfiles.size();
    }
    
    // Check if any profiles are active
    bool hasActiveProfiles() const {
        return !activeProfiles.empty();
    }

private:
    struct ProfileData {
        std::string operationName;
        std::chrono::high_resolution_clock::time_point startTime;
        clock_t startCpuTime;
        double totalTimeMs;
        double averageTimeMs;
        double minTimeMs;
        double maxTimeMs;
        uint64_t callCount;
        double totalCpuTimeMs;
        double efficiencyScore;
    };
    
    std::chrono::high_resolution_clock::time_point startTime;
    std::map<std::string, ProfileData> activeProfiles;
    std::map<std::string, ProfileData> completedProfiles;
};

// Global performance profiler instance
extern PerformanceProfiler globalProfiler;

// Macro for easy profiling
#define PROFILE_SCOPE(operationName) \
    static nlm::ProfileScope profileScope(operationName)

class ProfileScope {
public:
    ProfileScope(const std::string& operationName) : operationName(operationName) {
        globalProfiler.startProfiling(operationName);
    }
    
    ~ProfileScope() {
        globalProfiler.stopProfiling(operationName);
    }
    
    void complete(double efficiencyScore = 0.0) {
        globalProfiler.completeProfile(operationName, efficiencyScore);
    }
    
private:
    std::string operationName;
};

// Benchmarking utilities for neural simulations
class SimulationBenchmarker {
public:
    struct BenchmarkResult {
        std::string benchmarkName;
        uint64_t neuronCount;
        uint64_t stepCount;
        double totalTimeSeconds;
        double avgStepsPerSecond;
        double avgSpikeRate;
        double memoryUsageMB;
        double efficiencyScore; // spikes per millisecond
    };
    
    BenchmarkResult runBenchmark(uint64_t neuronCount, uint64_t stepCount) {
        BenchmarkResult result;
        result.benchmarkName = "NLM_Simulation_Benchmark";
        result.neuronCount = neuronCount;
        result.stepCount = stepCount;
        
        std::cout << "Running benchmark: " << neuronCount << " neurons, " << stepCount << " steps" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation (placeholder - would call actual simulation code)
        // For now, simulate some computation
        volatile double dummy = 0.0;
        for (uint64_t i = 0; i < stepCount * neuronCount / 1000; ++i) {
            dummy += std::sin(i * 0.01) * std::cos(i * 0.02);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime;
        
        result.totalTimeSeconds = elapsed.count();
        result.avgStepsPerSecond = static_cast<double>(stepCount) / result.totalTimeSeconds;
        result.avgSpikeRate = 50.0; // Simulated spike rate
        result.memoryUsageMB = 50.0 + (neuronCount * 0.01); // Rough estimate
        result.efficiencyScore = result.avgSpikeRate * result.totalTimeSeconds / 1000.0; // spikes per millisecond
        
        return result;
    }
    
    void printBenchmarkResults(const std::vector<BenchmarkResult>& results) const {
        std::cout << "=== NLM Simulation Benchmark Results ===" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Benchmark Name         | Neurons | Steps    | Total Time (s) | Steps/sec    | Spike Rate (Hz) | Memory (MB) | Efficiency (spikes/ms)" << std::endl;
        std::cout << "------------------------|---------|----------|----------------|--------------|-----------------|-------------|-----------------------" << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(24) << result.benchmarkName
                      << " | " << std::setw(6) << result.neuronCount
                      << " | " << std::setw(8) << result.stepCount
                      << " | " << std::setw(16) << std::fixed << std::setprecision(3) << result.totalTimeSeconds
                      << " | " << std::setw(12) << std::fixed << std::setprecision(1) << result.avgStepsPerSecond
                      << " | " << std::setw(13) << std::fixed << std::setprecision(1) << result.avgSpikeRate
                      << " | " << std::setw(11) << std::fixed << std::setprecision(1) << result.memoryUsageMB
                      << " | " << std::fixed << std::setprecision(4) << result.efficiencyScore << std::endl;
        }
        
        std::cout << std::endl;
        
        if (results.size() > 1) {
            std::cout << "=== Scaling Analysis ===" << std::endl;
            double prevStepsPerSec = results[0].avgStepsPerSecond;
            double prevEfficiency = results[0].efficiencyScore;
            
            for (size_t i = 1; i < results.size(); ++i) {
                double scalingFactor = results[i].avgStepsPerSecond / prevStepsPerSec;
                double efficiencyScaling = results[i].efficiencyScore / prevEfficiency;
                std::cout << results[i].benchmarkName << ": "
                          << "Steps/sec scaling: " << scalingFactor << ", "
                          << "Efficiency scaling: " << efficiencyScaling << std::endl;
                
                prevStepsPerSec = results[i].avgStepsPerSecond;
                prevEfficiency = results[i].efficiencyScore;
            }
        }
        
        std::cout << "=====================================" << std::endl;
    }
};

} // namespace nlm

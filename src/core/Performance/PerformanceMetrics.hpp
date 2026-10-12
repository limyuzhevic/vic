#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>

namespace nlm {

/**
 * Performance metrics for benchmarking and monitoring
 */
struct PerformanceMetrics {
    // Timing metrics
    double totalWallClockTime;
    double averageStepTime;
    double minStepTime;
    double maxStepTime;
    
    // Activity metrics
    double avgFiringRate;
    size_t totalSpikeCount;
    size_t totalSynapseCount;
    size_t totalNeuronCount;
    
    // Memory metrics
    float memoryUsageMB;
    size_t allocatedMemory;
    
    // System metrics
    float cpuUsage;
    float memoryUsage;
    size_t activeThreads;
    
    // Neural metrics
    float excitationInhibitionRatio;
    float synapticWeightMean;
    float synapticWeightStdDev;
    
    // Agent metrics
    float curiosityLevel;
    float noveltyLevel;
    float dopamineLevel;
    uint64_t decisionCount;
    float actionSuccessRate;
    
    // Time tracking
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    PerformanceMetrics()
        : totalWallClockTime(0.0)
        , averageStepTime(0.0)
        , minStepTime(0.0)
        , maxStepTime(0.0)
        , avgFiringRate(0.0)
        , totalSpikeCount(0)
        , totalSynapseCount(0)
        , totalNeuronCount(0)
        , memoryUsageMB(0.0)
        , allocatedMemory(0)
        , cpuUsage(0.0)
        , memoryUsage(0.0)
        , activeThreads(0)
        , excitationInhibitionRatio(0.0)
        , synapticWeightMean(0.0)
        , synapticWeightStdDev(0.0)
        , curiosityLevel(0.0)
        , noveltyLevel(0.0)
        , dopamineLevel(0.0)
        , decisionCount(0)
        , actionSuccessRate(0.0) {}
    
    void startTiming() {
        startTime = std::chrono::system_clock::now();
    }
    
    void endTiming() {
        endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        totalWallClockTime = duration.count();
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3);
        oss << "Performance Metrics:\n";
        oss << "  Wall Clock Time: " << totalWallClockTime << "s\n";
        oss << "  Average Step Time: " << averageStepTime * 1000 << "ms\n";
        oss << "  Min Step Time: " << minStepTime * 1000 << "ms\n";
        oss << "  Max Step Time: " << maxStepTime * 1000 << "ms\n";
        oss << "  Average Firing Rate: " << avgFiringRate << " Hz\n";
        oss << "  Total Spikes: " << totalSpikeCount << "\n";
        oss << "  Total Synapses: " << totalSynapseCount << "\n";
        oss << "  Total Neurons: " << totalNeuronCount << "\n";
        oss << "  Memory Usage: " << memoryUsageMB << " MB\n";
        oss << "  CPU Usage: " << cpuUsage << "%\n";
        oss << "  Excitation/Inhibition Ratio: " << excitationInhibitionRatio << "\n";
        oss << "  Synaptic Weight Mean: " << synapticWeightMean << "\n";
        oss << "  Synaptic Weight StdDev: " << synapticWeightStdDev << "\n";
        oss << "  Curiosity Level: " << curiosityLevel << "\n";
        oss << "  Novelty Level: " << noveltyLevel << "\n";
        oss << "  Dopamine Level: " << dopamineLevel << "\n";
        oss << "  Decision Count: " << decisionCount << "\n";
        oss << "  Action Success Rate: " << (actionSuccessRate * 100) << "%\n";
        return oss.str();
    }
};

/**
 * Advanced logging system with performance monitoring
 */
class PerformanceLogger {
public:
    PerformanceLogger() : startTime(std::chrono::system_clock::now()) {}
    
    void logStep(const std::string& stepName, double stepTime) {
        std::lock_guard<std::mutex> lock(mutex);
        
        StepTiming timing;
        timing.stepName = stepName;
        timing.stepTime = stepTime;
        timing.timestamp = std::chrono::system_clock::now();
        
        stepTimings.push_back(timing);
        
        // Keep only recent timings
        if (stepTimings.size() > 1000) {
            stepTimings.erase(stepTimings.begin());
        }
        
        // Log summary every 100 steps
        if (stepTimings.size() % 100 == 0) {
            logStepSummary();
        }
    }
    
    void logStepSummary() {
        if (stepTimings.empty()) return;
        
        double totalTime = 0.0;
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
        
        for (const auto& timing : stepTimings) {
            totalTime += timing.stepTime;
            minTime = std::min(minTime, timing.stepTime);
            maxTime = std::max(maxTime, timing.stepTime);
        }
        
        double avgTime = totalTime / stepTimings.size();
        
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
        
        std::cout << "[PERF] Step Summary - " << duration.count() << "s elapsed\n";
        std::cout << "[PERF] Steps: " << stepTimings.size() << ", ";
        std::cout << "Avg: " << avgTime * 1000 << "ms, ";
        std::cout << "Min: " << minTime * 1000 << "ms, ";
        std::cout << "Max: " << maxTime * 1000 << "ms\n";
        
        // Calculate steps per second
        if (duration.count() > 0) {
            double stepsPerSec = stepTimings.size() / duration.count();
            std::cout << "[PERF] Performance: " << stepsPerSec << " steps/sec\n";
        }
    }
    
    void logNeuralMetrics(const PerformanceMetrics& metrics) {
        std::lock_guard<std::mutex> lock(mutex);
        
        neuralMetricsHistory.push_back(metrics);
        
        // Keep only recent metrics
        if (neuralMetricsHistory.size() > 100) {
            neuralMetricsHistory.erase(neuralMetricsHistory.begin());
        }
        
        // Log detailed metrics every 10 entries
        if (neuralMetricsHistory.size() % 10 == 0) {
            logNeuralMetricsSummary();
        }
    }
    
    void logNeuralMetricsSummary() {
        if (neuralMetricsHistory.empty()) return;
        
        auto latest = neuralMetricsHistory.back();
        std::cout << "[NEURAL] " << latest.toString();
    }
    
    PerformanceMetrics getAverageMetrics() const {
        PerformanceMetrics avg;
        if (neuralMetricsHistory.empty()) return avg;
        
        size_t count = neuralMetricsHistory.size();
        
        for (const auto& metrics : neuralMetricsHistory) {
            avg.totalWallClockTime += metrics.totalWallClockTime;
            avg.averageStepTime += metrics.averageStepTime;
            avg.avgFiringRate += metrics.avgFiringRate;
            avg.totalSpikeCount += metrics.totalSpikeCount;
            avg.totalSynapseCount += metrics.totalSynapseCount;
            avg.totalNeuronCount += metrics.totalNeuronCount;
            avg.curiosityLevel += metrics.curiosityLevel;
            avg.noveltyLevel += metrics.noveltyLevel;
            avg.dopamineLevel += metrics.dopamineLevel;
            avg.decisionCount += metrics.decisionCount;
            avg.actionSuccessRate += metrics.actionSuccessRate;
        }
        
        avg.totalWallClockTime /= count;
        avg.averageStepTime /= count;
        avg.avgFiringRate /= count;
        avg.curiosityLevel /= count;
        avg.noveltyLevel /= count;
        avg.dopamineLevel /= count;
        avg.actionSuccessRate /= count;
        
        return avg;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        stepTimings.clear();
        neuralMetricsHistory.clear();
        startTime = std::chrono::system_clock::now();
    }
    
private:
    struct StepTiming {
        std::string stepName;
        double stepTime;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::vector<StepTiming> stepTimings;
    std::vector<PerformanceMetrics> neuralMetricsHistory;
    std::chrono::system_clock::time_point startTime;
    mutable std::mutex mutex;
};

/**
 * Benchmark utility for comparing performance
 */
class Benchmark {
public:
    static void runBenchmark(const std::string& name, std::function<void()> func, int iterations = 10) {
        std::cout << "=== Benchmark: " << name << " ===" << std::endl;
        
        std::vector<double> times;
        times.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            func();
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
            times.push_back(duration.count());
            
            std::cout << "Iteration " << (i + 1) << ": " << duration.count() * 1000 << "ms\n";
        }
        
        // Calculate statistics
        double sum = 0.0, min = times[0], max = times[0];
        for (double t : times) {
            sum += t;
            min = std::min(min, t);
            max = std::max(max, t);
        }
        
        double avg = sum / times.size();
        double variance = 0.0;
        for (double t : times) {
            variance += (t - avg) * (t - avg);
        }
        variance /= times.size();
        double stddev = std::sqrt(variance);
        
        std::cout << "\nResults:" << std::endl;
        std::cout << "  Average: " << avg * 1000 << "ms\n";
        std::cout << "  Min: " << min * 1000 << "ms\n";
        std::cout << "  Max: " << max * 1000 << "ms\n";
        std::cout << "  Standard Deviation: " << stddev * 1000 << "ms\n";
        std::cout << "  Operations per second: " << (1.0 / avg) << " ops/sec\n";
    }
};

} // namespace nlm
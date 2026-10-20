// Performance monitoring and metrics collection system
#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace nlm {

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        // Time measurements (in milliseconds)
        double stepTime;           // Average time per simulation step
        double connectivityTime;   // Time for connectivity tests
        double plasticityTime;     // Time for plasticity computations
        double memoryTime;          // Time for memory operations
        double totalSimulationTime; // Total simulation time
        
        // Activity metrics
        size_t totalSpikes;        // Total number of spikes
        size_t activeNeurons;      // Number of active neurons
        size_t firingNeurons;       // Number of firing neurons
        size_t totalSynapses;       // Total number of synapses
        float avgSynapticWeight;   // Average synaptic weight
        float weightVariance;      // Variance in synaptic weights
        
        // Learning metrics
        float learningRate;        // Average learning rate
        size_t plasticSynapses;    // Number of plastic synapses
        size_t structuralEvents;  // Number of structural events
        
        // Network dynamics
        float excitationInhibitionRatio; // E/I balance
        float networkStability;    // Network stability metric
        float informationFlow;     // Information processing rate
        
        // Memory metrics
        size_t workingMemoryUsage; // Working memory utilization
        size_t episodicMemoryUsage; // Episodic memory utilization
        float memoryRetention;     // Memory retention quality
        
        // Neuromodulation
        float dopamineLevel;       // Average dopamine level
        float curiosityLevel;      // Average curiosity level
        float noveltyLevel;        // Average novelty level
        
        // Development metrics
        double developmentalAge;  // Current developmental age
        float plasticityModulator; // Current plasticity modifier
        float maturityLevel;       // Network maturity
        
        PerformanceMetrics() : stepTime(0.0), connectivityTime(0.0), plasticityTime(0.0), 
                             memoryTime(0.0), totalSimulationTime(0.0),
                             totalSpikes(0), activeNeurons(0), firingNeurons(0),
                             totalSynapses(0), avgSynapticWeight(0.0f), weightVariance(0.0f),
                             learningRate(0.0f), plasticSynapses(0), structuralEvents(0),
                             excitationInhibitionRatio(0.0f), networkStability(0.0f),
                             informationFlow(0.0f), workingMemoryUsage(0), episodicMemoryUsage(0),
                             memoryRetention(0.0f), dopamineLevel(0.0f), curiosityLevel(0.0f),
                             noveltyLevel(0.0f), developmentalAge(0.0), plasticityModulator(0.0f),
                             maturityLevel(0.0f) {}
    };
    
    struct TimePoint {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string event;
        double value;
        std::map<std::string, std::string> metadata;
        
        TimePoint(const std::string& ev, double val) 
            : timestamp(std::chrono::high_resolution_clock::now()), event(ev), value(val) {}
    };
    
    struct BenchmarkResult {
        std::string benchmarkName;
        double duration;           // Total duration in seconds
        PerformanceMetrics metrics; // Performance metrics during benchmark
        std::vector<TimePoint> timeline; // Timeline of events
        std::string configuration; // Configuration used
        size_t parameterCount;      // Number of parameters used
        
        BenchmarkResult(const std::string& name) 
            : benchmarkName(name), duration(0.0), parameterCount(0) {}
    };
    
    PerformanceMonitor() = default;
    
    // Start timing a specific operation
    void startTimer(const std::string& operation);
    
    // Stop timing and record duration
    void stopTimer(const std::string& operation);
    
    // Get elapsed time for an operation
    double getElapsedTime(const std::string& operation) const;
    
    // Record a performance metric
    void recordMetric(const std::string& metricName, double value);
    
    // Get a performance metric
    double getMetric(const std::string& metricName) const;
    
    // Update comprehensive performance metrics
    void updateMetrics(const PerformanceMetrics& newMetrics);
    
    // Get current performance metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Start a benchmark
    void startBenchmark(const std::string& benchmarkName);
    
    // End a benchmark and return results
    BenchmarkResult endBenchmark();
    
    // Reset all measurements
    void reset();
    
    // Get benchmark history
    const std::vector<BenchmarkResult>& getBenchmarkHistory() const;
    
    // Export metrics to various formats
    std::string exportToJSON() const;
    std::string exportToCSV() const;
    std::string exportToMarkdown() const;
    
    // Compare with baseline metrics
    struct ComparisonResult {
        std::string metricName;
        double baselineValue;
        double currentValue;
        double improvement;      // Percentage improvement (negative for degradation)
        bool isBetter;           // Whether performance improved
        
        ComparisonResult(const std::string& name, double baseline, double current)
            : metricName(name), baselineValue(baseline), currentValue(current),
              improvement(0.0), isBetter(false) {
            improvement = ((current - baseline) / baseline) * 100.0;
            isBetter = (improvement > 0.0);
        }
    };
    
    std::vector<ComparisonResult> compareWithBaseline(const PerformanceMetrics& baseline) const;
    
    // Calculate performance score (0-100)
    double calculatePerformanceScore(const PerformanceMetrics& metrics) const;
    
    // Get performance recommendations
    std::vector<std::string> getPerformanceRecommendations() const;
    
    // Log performance summary
    void logPerformanceSummary() const;
    
private:
    // Timer management
    struct Timer {
        std::chrono::high_resolution_clock::time_point startTime;
        std::string operation;
        bool isRunning;
        
        Timer(const std::string& op) : operation(op), isRunning(false) {}
    };
    
    std::map<std::string, Timer> timers_;
    
    // Performance metrics
    PerformanceMetrics currentMetrics_;
    
    // Benchmark history
    std::vector<BenchmarkResult> benchmarkHistory_;
    
    // Individual metric storage
    std::map<std::string, std::vector<double>> metricHistory_;
    
    // Timeline events
    std::vector<TimePoint> timeline_;
    
    // Baseline metrics for comparison
    PerformanceMetrics baselineMetrics_;
    
    // Performance scoring weights
    struct PerformanceWeights {
        double stepEfficiency;      // Weight for step timing
        double learningEfficiency;  // Weight for learning metrics
        double stability;           // Weight for network stability
        double memoryUtilization;  // Weight for memory usage
        double neuromodulation;     // Weight for neuromodulation
        
        PerformanceWeights() : stepEfficiency(0.25), learningEfficiency(0.20),
                              stability(0.20), memoryUtilization(0.20), neuromodulation(0.15) {}
    };
    
    PerformanceWeights weights_;
    
    // Internal helper methods
    void updateMetricStatistics(const std::string& metricName, double value);
    double calculateMovingAverage(const std::string& metricName, size_t windowSize) const;
    void addTimelineEvent(const std::string& event, double value, const std::map<std::string, std::string>& metadata = {});
};

// Global performance monitor instance
class GlobalPerformanceMonitor {
public:
    static GlobalPerformanceMonitor& getInstance() {
        static GlobalPerformanceMonitor instance;
        return instance;
    }
    
    PerformanceMonitor& getMonitor() { return monitor_; }
    
    // Convenience functions for timing
    static void startTimer(const std::string& operation) { monitor_.startTimer(operation); }
    static void stopTimer(const std::string& operation) { monitor_.stopTimer(operation); }
    static double getElapsedTime(const std::string& operation) { return monitor_.getElapsedTime(operation); }
    
    // Convenience functions for recording metrics
    static void recordMetric(const std::string& metricName, double value) { monitor_.recordMetric(metricName, value); }
    static double getMetric(const std::string& metricName) { return monitor_.getMetric(metricName); }
    
    // Start/end benchmark
    static void startBenchmark(const std::string& benchmarkName) { monitor_.startBenchmark(benchmarkName); }
    static auto endBenchmark() { return monitor_.endBenchmark(); }
    
    // Get current metrics
    static PerformanceMonitor::PerformanceMetrics getCurrentMetrics() { return monitor_.getCurrentMetrics(); }
    
    // Reset
    static void reset() { monitor_.reset(); }
    
    // Get benchmark history
    static const std::vector<PerformanceMonitor::BenchmarkResult>& getBenchmarkHistory() { return monitor_.getBenchmarkHistory(); }
    
private:
    GlobalPerformanceMonitor() = default;
    ~GlobalPerformanceMonitor() = default;
    GlobalPerformanceMonitor(const GlobalPerformanceMonitor&) = delete;
    GlobalPerformanceMonitor& operator=(const GlobalPerformanceMonitor&) = delete;
    
    static PerformanceMonitor monitor_;
};

// Macros for convenient timing
#define NLM_PERFORMANCE_TIMER(name) nlm::GlobalPerformanceMonitor::startTimer(name)
#define NLM_PERFORMANCE_STOP_TIMER(name) nlm::GlobalPerformanceMonitor::stopTimer(name)
#define NLM_PERFORMANCE_RECORD_METRIC(name, value) nlm::GlobalPerformanceMonitor::recordMetric(name, value)
#define NLM_PERFORMANCE_START_BENCHMARK(name) nlm::GlobalPerformanceMonitor::startBenchmark(name)
#define NLM_PERFORMANCE_END_BENCHMARK() nlm::GlobalPerformanceMonitor::endBenchmark()

// Performance monitoring utilities
namespace PerformanceUtils {
    // Calculate various performance metrics
    class MetricsCalculator {
    public:
        // Calculate average firing rate
        static float calculateAverageFiringRate(size_t totalSpikes, size_t totalTimeSteps, size_t neuronCount);
        
        // Calculate synaptic efficiency
        static float calculateSynapticEfficiency(const std::vector<float>& weights);
        
        // Calculate network stability
        static float calculateNetworkStability(const std::vector<float>& weights);
        
        // Calculate information flow
        static float calculateInformationFlow(const std::vector<float>& inputs, const std::vector<float>& outputs);
        
        // Calculate memory retention
        static float calculateMemoryRetention(const std::vector<float>& memoryTraces);
        
        // Calculate developmental progression
        static float calculateDevelopmentalProgression(double age, float plasticityModifier);
        
        // Calculate neuromodulation effectiveness
        static float calculateNeuromodulationEffectiveness(float dopamine, float curiosity, float novelty);
        
        // Calculate overall network health
        static float calculateNetworkHealth(const PerformanceMonitor::PerformanceMetrics& metrics);
        
        // Detect performance bottlenecks
        static std::vector<std::string> detectBottlenecks(const PerformanceMonitor::PerformanceMetrics& metrics);
        
        // Calculate performance score
        static float calculatePerformanceScore(const PerformanceMonitor::PerformanceMetrics& metrics,
                                            const PerformanceMonitor::PerformanceWeights& weights);
    };
}

} // namespace nlm

// Implementation details (in separate file)
// This header contains only declarations; implementation is in .cpp file
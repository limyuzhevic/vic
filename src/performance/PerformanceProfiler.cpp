// NLM Performance Profiling System
// Comprehensive performance analysis and optimization tools for NLM

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <atomic>
#include <thread>
#include <functional>

namespace nlm {

// Performance measurement types
enum class PerformanceMetric {
    CPU_TIME,              // CPU time usage
    WALL_CLOCK_TIME,        // Real time elapsed
    MEMORY_USAGE,           // Memory consumption
    MEMORY_ALLOCATIONS,     // Number of memory allocations
    MEMORY_DEALLOCS,        // Number of memory deallocations
    SYNAPSE_UPDATES,        // Synapse update operations
    NEURON_STEPS,           // Neuron computation steps
    SPIKE_GENERATION,        // Spike generation events
    PLASTICITY_UPDATES,      // Plasticity update operations
    SENSORY_PROCESSING,     // Sensory input processing time
    MOTOR_OUTPUT,           // Motor output computation time
    INTEGRATION_TIME,       // Overall integration step time
};

// Performance sampling modes
enum class SamplingMode {
    INSTANT,                // Single measurement
    RUNNING_AVERAGE,        // Running average
    MOVING_WINDOW,          // Moving time window
    HISTOGRAM,              // Statistical distribution
    THRESHOLD_ALERT,        // Alert when thresholds exceeded
};

// Performance threshold conditions
struct PerformanceThreshold {
    PerformanceMetric metric;
    double thresholdValue;
    std::string condition;    // "<", "<=", ">", ">=", "==", "!="
    std::string message;      // Alert message
    std::function<void(const PerformanceThreshold&, double)> callback;
    bool enabled;
};

// Performance sample
struct PerformanceSample {
    double timestamp;        // Wall clock time
    std::map<PerformanceMetric, double> metrics;  // Metric values
    std::map<std::string, std::string> metadata;  // Additional context
};

// Performance analysis results
struct PerformanceAnalysis {
    std::vector<PerformanceSample> samples;
    double duration;          // Total measurement duration
    double startTime;         // Start timestamp
    double endTime;           // End timestamp
    
    // Statistical analysis
    struct Statistics {
        double mean;
        double median;
        double standardDeviation;
        double minimum;
        double maximum;
        double variance;
    };
    
    std::map<PerformanceMetric, Statistics> statistics;
    
    // Trends
    std::map<PerformanceMetric, std::vector<double>> trends;
    
    // Bottleneck analysis
    std::vector<std::string> bottlenecks;
    std::map<PerformanceMetric, double> bottleneckImpact;
    
    // Recommendations
    std::vector<std::string> recommendations;
    std::vector<std::string> optimizationSuggestions;
};

// Performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Configuration
    void configure(SamplingMode mode, 
                   double samplingInterval = 0.1,
                   size_t maxSamples = 10000,
                   bool enableThresholdAlerts = true);
    
    // Control
    void start();
    void stop();
    void pause();
    void resume();
    bool isRunning() const;
    
    // Sampling
    void sample();  // Take a measurement sample
    std::vector<PerformanceSample> getSamples() const;
    PerformanceSample getLatestSample() const;
    
    // Threshold management
    void addThreshold(const PerformanceThreshold& threshold);
    void removeThreshold(PerformanceMetric metric);
    std::vector<PerformanceThreshold> getThresholds() const;
    
    // Analysis
    PerformanceAnalysis analyze(bool computeStatistics = true,
                                bool detectBottlenecks = true,
                                bool generateRecommendations = true);
    
    // Report generation
    std::string generateTextReport(const PerformanceAnalysis& analysis) const;
    std::string generateJSONReport(const PerformanceAnalysis& analysis) const;
    std::string generateHTMLReport(const PerformanceAnalysis& analysis) const;
    
    // Export
    bool exportSamples(const std::string& filepath);
    bool exportAnalysis(const std::string& filepath, const PerformanceAnalysis& analysis);
    
    // Specialized profiling
    void startNeuralProfiling();
    void stopNeuralProfiling();
    void startMemoryProfiling();
    void stopMemoryProfiling();
    void startIOProfiling();
    void stopIOProfiling();
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enable);
    std::string getRealTimeMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural-specific profiler
class NeuralPerformanceProfiler {
public:
    NeuralPerformanceProfiler();
    ~NeuralPerformanceProfiler();
    
    // Configure neural profiling
    void configure(NeuralProfilingMode mode = NeuralProfilingMode::COMPREHENSIVE);
    void setBrain(const Brain& brain);
    
    // Start/stop profiling
    void start();
    void stop();
    bool isRunning() const;
    
    // Get profiling data
    std::vector<NeuralProfileSample> getProfileData() const;
    NeuralProfileAnalysis analyze() const;
    
    // Component-specific profiling
    void profileNeuronComputation(const Neuron& neuron, double timeTaken);
    void profileSynapseUpdate(const Synapse& synapse, double timeTaken);
    void profileSpikeGeneration(const std::vector<SpikeEvent>& spikes, double timeTaken);
    void profilePlasticityUpdate(const PlasticityRule& rule, double timeTaken);
    
    // Bottleneck detection
    std::vector<std::string> detectNeuralBottlenecks() const;
    std::map<std::string, double> getComponentPerformance() const;
    
    // Optimization recommendations
    std::vector<std::string> generateOptimizationRecommendations() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory profiler
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Configure memory profiling
    void configure(MemoryTrackingMode mode = MemoryTrackingMode::DETAILED);
    
    // Control
    void start();
    void stop();
    bool isRunning() const;
    
    // Memory tracking
    void trackAllocation(size_t size, const std::string& source = "");
    void trackDeallocation(size_t size, const std::string& source = "");
    void trackMemoryAccess(const void* address, size_t size);
    
    // Get statistics
    MemoryStatistics getCurrentStatistics() const;
    std::vector<MemorySample> getMemorySamples() const;
    MemoryLeakReport getMemoryLeakReport() const;
    
    // Heap analysis
    HeapInfo analyzeHeap() const;
    std::vector<std::string> detectMemoryLeaks() const;
    std::vector<std::string> detectMemoryFragments() const;
    
    // Tracking control
    void enableAllocationTracking(bool enable);
    void enableDeallocationTracking(bool enable);
    void enableAccessTracking(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// I/O profiler
class IOPerformer {
public:
    IOPerformer();
    ~IOPerformer();
    
    // Configure I/O profiling
    void configure(IOOperation tracking = IOOperation::ALL);
    
    // Control
    void start();
    void stop();
    bool isRunning() const;
    
    // I/O operation tracking
    void trackRead(const std::string& filepath, size_t size, double duration);
    void trackWrite(const std::string& filepath, size_t size, double duration);
    void trackMemoryMap(const std::string& path, size_t size, double duration);
    void trackFileOpen(const std::string& filepath, double duration);
    void trackFileClose(const std::string& filepath, double duration);
    
    // Get I/O statistics
    IOStatistics getCurrentStatistics() const;
    std::vector<IOSample> getIOSamples() const;
    IOBottleneckReport getIOBottleneckReport() const;
    
    // Performance analysis
    std::vector<std::string> detectIOBottlenecks() const;
    std::map<std::string, double> getComponentIOUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Comprehensive profiler manager
class ProfilerManager {
public:
    ProfilerManager();
    ~ProfilerManager();
    
    // Add profilers
    void addProfiler(const std::string& name, std::unique_ptr<PerformanceProfiler> profiler);
    void removeProfiler(const std::string& name);
    std::vector<std::string> getProfilerNames() const;
    
    // Coordinated profiling
    void startAll();
    void stopAll();
    void pauseAll();
    void resumeAll();
    
    // Combined analysis
    CombinedPerformanceAnalysis analyzeAll();
    std::string generateCombinedReport(const CombinedPerformanceAnalysis& analysis);
    
    // Configuration
    void configureGlobalSettings(const ProfilerGlobalSettings& settings);
    void setProfilingInterval(double interval);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Factory functions for creating profilers
std::unique_ptr<PerformanceProfiler> createPerformanceProfiler();
std::unique_ptr<NeuralPerformanceProfiler> createNeuralPerformanceProfiler();
std::unique_ptr<MemoryProfiler> createMemoryProfiler();
std::unique_ptr<IOPerformer> createIOPerformer();
std::unique_ptr<ProfilerManager> createProfilerManager();

// Convenience functions
bool startPerformanceProfiling(const std::string& outputFile = "performance.prof");
void stopPerformanceProfiling();
bool runPerformanceBenchmark(const std::vector<std::string>& configurations);
PerformanceAnalysis analyzeConfigurationPerformance(const std::string& configFile);

// Real-time monitoring functions
void enableRealTimePerformanceMonitor(const std::string& endpoint = "localhost:8080");
void disableRealTimePerformanceMonitor();
std::string getRealTimePerformanceMetrics();

// Optimization helper functions
std::vector<std::string> getPerformanceOptimizationRecommendations(const PerformanceAnalysis& analysis);
bool applyPerformanceOptimizations(const std::vector<std::string>& optimizations);

} // namespace nlm
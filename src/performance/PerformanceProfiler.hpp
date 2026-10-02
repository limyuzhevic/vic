#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <atomic>

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

// Neural profiling modes
enum class NeuralProfilingMode {
    BASIC,                 // Basic neural computation tracking
    COMPREHENSIVE,         // Comprehensive neural analysis
    DETAILED,              // Detailed per-component analysis
    INTELLIGENT,           // Intelligent bottleneck detection
};

// Memory tracking modes
enum class MemoryTrackingMode {
    BASIC,                 // Basic allocation tracking
    DETAILED,              // Detailed allocation/deallocation tracking
    ACCESS,                // Memory access pattern tracking
    LEAK_DETECTION,         // Memory leak detection
};

// I/O operation tracking
enum class IOOperation {
    ALL,                   // Track all I/O operations
    FILE_ACCESS,           // File read/write operations
    NETWORK,               // Network I/O operations
    MEMORY_MAPPING,        // Memory-mapped files
    DATABASE,              // Database operations
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
    std::map<PerformanceMetric, double> metrics; // Metric values
    std::map<std::string, std::string> metadata; // Additional context
};

// Memory statistics
struct MemoryStatistics {
    size_t totalAllocated;    // Total memory allocated
    size_t currentUsage;      // Current memory usage
    size_t peakUsage;         // Peak memory usage
    size_t allocationCount;   // Number of allocation calls
    size_t deallocationCount; // Number of deallocation calls
    double fragmentationLevel; // Memory fragmentation (0-1)
};

// Memory sample
struct MemorySample {
    double timestamp;
    MemoryStatistics stats;
    std::vector<std::pair<void*, size_t>> allocations;
    std::vector<std::pair<void*, size_t>> deallocations;
};

// Memory leak report
struct MemoryLeakReport {
    bool hasLeaks;
    std::vector<std::pair<void*, size_t>> leakedBlocks;
    std::vector<std::string> leakReasons;
    double totalLeakSize;
};

// Heap information
struct HeapInfo {
    size_t totalBlocks;
    size_t freeBlocks;
    size_t allocatedBlocks;
    size_t fragmentation;
    std::map<size_t, size_t> sizeDistribution; // Block size -> count
};

// I/O statistics
struct IOStatistics {
    double totalReadTime;     // Total time spent reading
    double totalWriteTime;    // Total time spent writing
    size_t totalReadBytes;    // Total bytes read
    size_t totalWriteBytes;   // Total bytes written
    size_t readCount;         // Number of read operations
    size_t writeCount;        // Number of write operations
    double averageReadTime;  // Average read time
    double averageWriteTime; // Average write time
};

// I/O sample
struct IOSample {
    double timestamp;
    IOStatistics stats;
    std::vector<std::tuple<std::string, std::string, size_t, double>> operations;
};

// I/O bottleneck report
struct IOBottleneckReport {
    bool hasBottlenecks;
    std::vector<std::string> bottlenecks;
    std::map<std::string, double> bottleneckImpact;
    std::vector<std::string> recommendations;
};

// Neural profile sample
struct NeuralProfileSample {
    double timestamp;
    double neuronComputationTime;
    double synapseUpdateTime;
    double spikeGenerationTime;
    double plasticityUpdateTime;
    double sensoryProcessingTime;
    double motorOutputTime;
    size_t neuronCount;
    size_t synapseCount;
    size_t spikeCount;
    double firingRate;
};

// Neural profile analysis
struct NeuralProfileAnalysis {
    std::vector<NeuralProfileSample> samples;
    double duration;
    
    // Component performance
    struct ComponentStats {
        double averageTime;
        double minimumTime;
        double maximumTime;
        double totalTime;
        size_t callCount;
        double efficiency; // 0-1, higher is better
    };
    
    std::map<std::string, ComponentStats> componentStats;
    
    // Bottleneck analysis
    std::vector<std::string> bottlenecks;
    std::map<std::string, double> bottleneckImpact;
    
    // Recommendations
    std::vector<std::string> recommendations;
    std::vector<std::string> optimizationSuggestions;
};

// Global profiler settings
struct ProfilerGlobalSettings {
    double samplingInterval;      // Sampling interval in seconds
    size_t maxSamples;             // Maximum number of samples to store
    bool enableThresholdAlerts;   // Enable threshold-based alerts
    std::string outputDirectory;  // Directory for output files
    std::string reportFormat;      // Default report format (text, json, html)
};

// Combined performance analysis
struct CombinedPerformanceAnalysis {
    std::vector<PerformanceSample> allSamples;
    double totalDuration;
    
    // Overall statistics
    struct OverallStats {
        double cpuEfficiency;
        double memoryEfficiency;
        double computationalEfficiency;
        double systemUtilization;
    };
    
    OverallStats overallStats;
    
    // Cross-component analysis
    std::map<std::string, double> componentCorrelations;
    std::vector<std::string> systemicBottlenecks;
    
    // Optimization opportunities
    std::vector<std::string> optimizationOpportunities;
    std::vector<std::string> costBenefitAnalysis;
};

// Performance profiler interface
class PerformanceProfiler {
public:
    virtual ~PerformanceProfiler() = default;
    
    // Configuration
    virtual void configure(SamplingMode mode,
                           double samplingInterval = 0.1,
                           size_t maxSamples = 10000,
                           bool enableThresholdAlerts = true) = 0;
    
    // Control
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual bool isRunning() const = 0;
    
    // Sampling
    virtual void sample() = 0;
    virtual std::vector<PerformanceSample> getSamples() const = 0;
    virtual PerformanceSample getLatestSample() const = 0;
    
    // Threshold management
    virtual void addThreshold(const PerformanceThreshold& threshold) = 0;
    virtual void removeThreshold(PerformanceMetric metric) = 0;
    virtual std::vector<PerformanceThreshold> getThresholds() const = 0;
    
    // Analysis
    virtual PerformanceAnalysis analyze(bool computeStatistics = true,
                                        bool detectBottlenecks = true,
                                        bool generateRecommendations = true) = 0;
    
    // Report generation
    virtual std::string generateTextReport(const PerformanceAnalysis& analysis) const = 0;
    virtual std::string generateJSONReport(const PerformanceAnalysis& analysis) const = 0;
    virtual std::string generateHTMLReport(const PerformanceAnalysis& analysis) const = 0;
    
    // Export
    virtual bool exportSamples(const std::string& filepath) = 0;
    virtual bool exportAnalysis(const std::string& filepath, const PerformanceAnalysis& analysis) = 0;
    
    // Specialized profiling
    virtual void startNeuralProfiling() = 0;
    virtual void stopNeuralProfiling() = 0;
    virtual void startMemoryProfiling() = 0;
    virtual void stopMemoryProfiling() = 0;
    virtual void startIOProfiling() = 0;
    virtual void stopIOProfiling() = 0;
    
    // Real-time monitoring
    virtual void enableRealTimeMonitoring(bool enable) = 0;
    virtual std::string getRealTimeMetrics() const = 0;
    
    // Properties
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
};

// Neural performance profiler interface
class NeuralPerformanceProfiler {
public:
    virtual ~NeuralPerformanceProfiler() = default;
    
    // Configuration
    virtual void configure(NeuralProfilingMode mode = NeuralProfilingMode::COMPREHENSIVE) = 0;
    virtual void setBrain(const Brain& brain) = 0;
    
    // Control
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    // Get profiling data
    virtual std::vector<NeuralProfileSample> getProfileData() const = 0;
    virtual NeuralProfileAnalysis analyze() const = 0;
    
    // Component-specific profiling
    virtual void profileNeuronComputation(const Neuron& neuron, double timeTaken) = 0;
    virtual void profileSynapseUpdate(const Synapse& synapse, double timeTaken) = 0;
    virtual void profileSpikeGeneration(const std::vector<SpikeEvent>& spikes, double timeTaken) = 0;
    virtual void profilePlasticityUpdate(const PlasticityRule& rule, double timeTaken) = 0;
    
    // Bottleneck detection
    virtual std::vector<std::string> detectNeuralBottlenecks() const = 0;
    virtual std::map<std::string, double> getComponentPerformance() const = 0;
    
    // Optimization recommendations
    virtual std::vector<std::string> generateOptimizationRecommendations() const = 0;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
};

// Memory profiler interface
class MemoryProfiler {
public:
    virtual ~MemoryProfiler() = default;
    
    // Configuration
    virtual void configure(MemoryTrackingMode mode = MemoryTrackingMode::DETAILED) = 0;
    
    // Control
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    // Memory tracking
    virtual void trackAllocation(size_t size, const std::string& source = "") = 0;
    virtual void trackDeallocation(size_t size, const std::string& source = "") = 0;
    virtual void trackMemoryAccess(const void* address, size_t size) = 0;
    
    // Get statistics
    virtual MemoryStatistics getCurrentStatistics() const = 0;
    virtual std::vector<MemorySample> getMemorySamples() const = 0;
    virtual MemoryLeakReport getMemoryLeakReport() const = 0;
    
    // Heap analysis
    virtual HeapInfo analyzeHeap() const = 0;
    virtual std::vector<std::string> detectMemoryLeaks() const = 0;
    virtual std::vector<std::string> detectMemoryFragments() const = 0;
    
    // Tracking control
    virtual void enableAllocationTracking(bool enable) = 0;
    virtual void enableDeallocationTracking(bool enable) = 0;
    virtual void enableAccessTracking(bool enable) = 0;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
};

// I/O profiler interface
class IOPerformer {
public:
    virtual ~IOPerformer() = default;
    
    // Configuration
    virtual void configure(IOOperation tracking = IOOperation::ALL) = 0;
    
    // Control
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    // I/O operation tracking
    virtual void trackRead(const std::string& filepath, size_t size, double duration) = 0;
    virtual void trackWrite(const std::string& filepath, size_t size, double duration) = 0;
    virtual void trackMemoryMap(const std::string& path, size_t size, double duration) = 0;
    virtual void trackFileOpen(const std::string& filepath, double duration) = 0;
    virtual void trackFileClose(const std::string& filepath, double duration) = 0;
    
    // Get statistics
    virtual IOStatistics getCurrentStatistics() const = 0;
    virtual std::vector<IOSample> getIOSamples() const = 0;
    virtual IOBottleneckReport getIOBottleneckReport() const = 0;
    
    // Performance analysis
    virtual std::vector<std::string> detectIOBottlenecks() const = 0;
    virtual std::map<std::string, double> getComponentIOUsage() const = 0;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
};

// Profiler manager interface
class ProfilerManager {
public:
    virtual ~ProfilerManager() = default;
    
    // Add profilers
    virtual void addProfiler(const std::string& name, std::unique_ptr<PerformanceProfiler> profiler) = 0;
    virtual void removeProfiler(const std::string& name) = 0;
    virtual std::vector<std::string> getProfilerNames() const = 0;
    
    // Coordinated profiling
    virtual void startAll() = 0;
    virtual void stopAll() = 0;
    virtual void pauseAll() = 0;
    virtual void resumeAll() = 0;
    
    // Combined analysis
    virtual CombinedPerformanceAnalysis analyzeAll() = 0;
    virtual std::string generateCombinedReport(const CombinedPerformanceAnalysis& analysis) = 0;
    
    // Configuration
    virtual void configureGlobalSettings(const ProfilerGlobalSettings& settings) = 0;
    virtual void setProfilingInterval(double interval) = 0;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
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
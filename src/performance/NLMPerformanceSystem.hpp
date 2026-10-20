//============================================================================
// NLM PERFORMANCE OPTIMIZATION AND PROFILING SYSTEM
//============================================================================
//
// This comprehensive performance system provides:
//
// 1. Performance Measurement Infrastructure:
//    - High-precision timing utilities
//    - Performance profiling framework
//    - Memory usage monitoring
//    - CPU utilization tracking
//    - Network I/O measurement
//    - GPU acceleration detection
//
// 2. Profiling Tools:
//    - Timeline profiler (records execution timeline)
//    - Call stack profiler (samples and analyzes call stacks)
//    - Memory profiler (tracks allocation/deallocation patterns)
//    - CPU profiler (monitors CPU utilization and hotspots)
//    - Network profiler (tracks network I/O)
//    - Thread profiler (monitors thread activity and performance)
//
// 3. Optimization Opportunities:
//    - Memory pool optimizations
//    - SIMD vectorization hints
//    - Cache optimization strategies
//    - Parallelization recommendations
//    - Algorithm optimization opportunities
//    - Data layout optimizations
//
// 4. Performance Monitoring:
//    - Real-time performance monitoring
//    - Performance alerting
//    - Performance dashboards
//    - Performance reports
//    - Automated performance tuning
//
// 5. Benchmarking Framework:
//    - Standard benchmarks
//    - Custom benchmarks
//    - Performance comparison tools
//    - Regression detection
//    - Performance testing automation
//
// 6. Performance Analysis Tools:
//    - Performance bottleneck analysis
//    - Scalability analysis
//    - Load testing tools
//    - Stress testing tools
//    - Chaos engineering tools
//
// 7. Integration with Existing Systems:
//    - Integration with existing monitoring systems
//    - Integration with existing profiling tools
//    - Integration with existing benchmarking tools
//    - Integration with existing performance analysis tools
//
// The system is designed to be:
// - Extensible: New profiling tools and analyzers can be added
// - Configurable: Performance measurement can be tailored to specific needs
// - Scalable: Works from small simulations to large-scale neural networks
// - Integrable: Can be easily integrated with existing NLM systems
// - Automated: Provides automated performance monitoring and optimization
//============================================================================

#ifndef NLM_PERFORMANCE_OPTIMIZATION_HPP
#define NLM_PERFORMANCE_OPTIMIZATION_HPP

#include "Performance.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <optional>

// Forward declarations
namespace nlm {
    class Brain;
    class Neuron;
    class Synapse;
    class Experiment;
}

// Performance System Components
namespace nlm {

/**
 * Main performance system interface
 */
class NLMPerformanceSystem {
public:
    NLMPerformanceSystem();
    ~NLMPerformanceSystem();
    
    // Singleton access
    static NLMPerformanceSystem& getInstance();
    
    // Initialize with configuration
    void initialize(const std::string& configFile = "");
    
    // Start/stop performance system
    void start();
    void stop();
    bool isRunning() const;
    
    // Get component access
    class PerformanceProfiler& getProfiler();
    class TimelineProfiler& getTimelineProfiler();
    class MemoryProfiler& getMemoryProfiler();
    class CPUProfiler& getCPUProfiler();
    class ThreadProfiler& getThreadProfiler();
    class PerformanceMonitor& getMonitor();
    class BenchmarkRunner& getBenchmarkRunner();
    class PerformanceAnalyzer& getAnalyzer();
    class PerformanceTuner& getTuner();
    class PerformanceDashboard& getDashboard();
    
    // Run comprehensive performance test
    struct PerformanceTestResult {
        PerformanceProfiler::ProfileResult profile;
        TimelineProfiler::TimelineEvent timeline;
        MemoryProfiler::MemorySnapshot memory;
        CPUProfiler::CPUSample cpu;
        ThreadProfiler::ThreadSample thread;
        PerformanceStats stats;
        ScalingAnalysis scaling;
        std::vector<BottleneckAnalysis> bottlenecks;
        std::vector<OptimizationRecommendation> recommendations;
        double totalTestTime;
        std::string report;
    };
    
    PerformanceTestResult runPerformanceTest(
        std::function<void()> setupFunction,
        std::function<void()> testFunction,
        std::function<void()> teardownFunction,
        const std::string& testName = "performance_test"
    );
    
    // Save/load performance system state
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);
    
    // Generate comprehensive report
    std::string generateComprehensiveReport() const;
    
    // Export data for external analysis
    bool exportData(const std::string& outputDir) const;
    
private:
    bool running_;
    
    // Components
    std::unique_ptr<PerformanceProfiler> profiler_;
    std::unique_ptr<TimelineProfiler> timelineProfiler_;
    std::unique_ptr<MemoryProfiler> memoryProfiler_;
    std::unique_ptr<CPUProfiler> cpuProfiler_;
    std::unique_ptr<ThreadProfiler> threadProfiler_;
    std::unique_ptr<PerformanceMonitor> monitor_;
    std::unique_ptr<BenchmarkRunner> benchmarkRunner_;
    std::unique_ptr<PerformanceAnalyzer> analyzer_;
    std::unique_ptr<PerformanceTuner> tuner_;
    std::unique_ptr<PerformanceDashboard> dashboard_;
    
    mutable std::mutex mutex_;
};

// Global performance system access
NLMPerformanceSystem& getGlobalPerformanceSystem();

// Convenience initialization functions
void initializePerformanceSystem(const std::string& configFile = "");
bool isPerformanceSystemInitialized();
void shutdownPerformanceSystem();

// Performance testing utilities
template<typename Func>
double profileBrainOperation(Func&& func, const std::string& operationName);

template<typename Func>
double profileNeuralOperation(Func&& func, const std::string& neuronType);

double profileSimulationStep(size_t neuronCount, size_t synapseCount,
                            size_t spikeCount, double duration);

// Performance analysis functions
void checkPerformanceIssues(const PerformanceStats& stats);
std::vector<std::string> generateOptimizationRecommendations(
    const PerformanceStats& stats,
    const PerformanceProfiler::ProfileResult& profile
);

// Benchmarking utilities
BenchmarkRun benchmarkBrain(size_t neuronCount, uint64_t steps, uint64_t seed = 42);
double profileBrainInitialization(size_t neuronCount);
double profileSimulationStep(size_t neuronCount, size_t synapseCount, size_t spikeCount);
double comparePerformance(const BenchmarkRun& current, const BenchmarkRun& baseline);
std::string generatePerformanceReport(const BenchmarkRun& result);

} // namespace nlm

// Performance system initialization and management functions
void initPerformanceSystem();
void shutdownPerformanceSystem();

/**
 * Performance testing utilities
 */
namespace nlm::perf {
    /**
     * Benchmark brain performance
     */
    BenchmarkRun benchmarkBrain(size_t neuronCount, uint64_t steps, uint64_t seed = 42);
    
    /**
     * Profile brain initialization
     */
    double profileBrainInitialization(size_t neuronCount);
    
    /**
     * Profile simulation step
     */
    double profileSimulationStep(size_t neuronCount, size_t synapseCount, size_t spikeCount);
    
    /**
     * Compare performance with baseline
     */
    double comparePerformance(const BenchmarkRun& current, const BenchmarkRun& baseline);
    
    /**
     * Generate performance report
     */
    std::string generatePerformanceReport(const BenchmarkRun& result);
}

#endif // NLM_PERFORMANCE_OPTIMIZATION_HPP

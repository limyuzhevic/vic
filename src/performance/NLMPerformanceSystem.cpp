#include "NLMPerformanceSystem.cpp"
#include <stdexcept>

namespace nlm {

// Global performance system instance
static NLMPerformanceSystem* globalPerformanceSystem = nullptr;

NLMPerformanceSystem& getGlobalPerformanceSystem() {
    if (!globalPerformanceSystem) {
        globalPerformanceSystem = new NLMPerformanceSystem();
    }
    return *globalPerformanceSystem;
}

void initializePerformanceSystem(const std::string& configFile) {
    getGlobalPerformanceSystem().initialize(configFile);
}

bool isPerformanceSystemInitialized() {
    return getGlobalPerformanceSystem().isRunning();
}

void shutdownPerformanceSystem() {
    if (globalPerformanceSystem) {
        delete globalPerformanceSystem;
        globalPerformanceSystem = nullptr;
    }
}

// HighPrecisionTimer implementation
namespace {
    HighPrecisionTimer::TimePoint HighPrecisionTimer::start_ = std::chrono::high_resolution_clock::now();
}

// Brain operation profiling
template<typename Func>
double profileBrainOperation(Func&& func, const std::string& operationName) {
    HighPrecisionTimer timer;
    func();
    double duration = timer.elapsed();
    
    // Record to global performance system
    auto& perfSystem = getGlobalPerformanceSystem();
    perfSystem.getProfiler().enter(operationName, __FILE__, __LINE__);
    std::this_thread::sleep_for(std::chrono::duration<double>(duration));
    perfSystem.getProfiler().exit(operationName);
    
    return duration;
}

template<typename Func>
double profileNeuralOperation(Func&& func, const std::string& neuronType) {
    HighPrecisionTimer timer;
    func();
    double duration = timer.elapsed();
    
    // Record neuron-specific operation
    auto& perfSystem = getGlobalPerformanceSystem();
    perfSystem.getProfiler().enter("neural_" + neuronType, __FILE__, __LINE__);
    std::this_thread::sleep_for(std::chrono::duration<double>(duration));
    perfSystem.getProfiler().exit("neural_" + neuronType);
    
    return duration;
}

double profileSimulationStep(size_t neuronCount, size_t synapseCount,
                            size_t spikeCount, double duration) {
    auto& perfSystem = getGlobalPerformanceSystem();
    perfSystem.getMonitor().recordStep(duration, spikeCount, neuronCount,
                                       neuronCount * sizeof(float) + synapseCount * sizeof(float));
    return duration;
}

void checkPerformanceIssues(const PerformanceStats& stats) {
    auto& perfSystem = getGlobalPerformanceSystem();
    perfSystem.getMonitor().setThreshold("step_time", stats.maxStepTime * 1.5);
    perfSystem.getMonitor().setThreshold("memory", stats.peakMemoryUsage * 1.1);
    perfSystem.getMonitor().setThreshold("cpu", stats.peakCPUUsage * 1.1);
    
    perfSystem.getMonitor().checkForAlerts();
}

std::vector<std::string> generateOptimizationRecommendations(
    const PerformanceStats& stats,
    const PerformanceProfiler::ProfileResult& profile
) {
    std::vector<std::string> recommendations;
    
    if (stats.maxStepTime > stats.averageStepTime * 2.0) {
        recommendations.push_back("Consider optimizing the main simulation loop to reduce step time variance");
    }
    
    if (stats.peakMemoryUsage > stats.averageMemoryUsage * 2.0) {
        recommendations.push_back("Implement memory pooling to reduce memory fragmentation");
    }
    
    if (stats.peakCPUUsage > stats.averageCPUUsage * 1.5) {
        recommendations.push_back("Profile CPU usage hotspots and consider SIMD optimizations");
    }
    
    return recommendations;
}

void initPerformanceSystem() {
    initializePerformanceSystem();
}

void shutdownPerformanceSystem() {
    shutdownPerformanceSystem();
}

// Benchmark system implementation
namespace {
    BenchmarkConfig getDefaultBenchmarkConfig() {
        BenchmarkConfig config;
        config.name = "NLM Performance Benchmark";
        
        // Add standard scale levels
        ScaleLevel smallScale;
        smallScale.name = "small";
        smallScale.neurons = 1000;
        smallScale.expectedSynapses = 10000;
        smallScale.connectionProbability = 0.1f;
        smallScale.hardwareTarget = "single_core";
        smallScale.notes = "Small scale for testing";
        
        ScaleLevel mediumScale;
        mediumScale.name = "medium";
        mediumScale.neurons = 10000;
        mediumScale.expectedSynapses = 100000;
        mediumScale.connectionProbability = 0.1f;
        mediumScale.hardwareTarget = "multi_core";
        mediumScale.notes = "Medium scale for typical use";
        
        ScaleLevel largeScale;
        largeScale.name = "large";
        largeScale.neurons = 100000;
        largeScale.expectedSynapses = 1000000;
        largeScale.connectionProbability = 0.1f;
        largeScale.hardwareTarget = "gpu";
        largeScale.notes = "Large scale for high-performance systems";
        
        config.scales = {smallScale, mediumScale, largeScale};
        
        return config;
    }
}

BenchmarkRun benchmarkBrain(size_t neuronCount, uint64_t steps, uint64_t seed) {
    BenchmarkRunner runner;
    BenchmarkConfig config = getDefaultBenchmarkConfig();
    
    // Filter scales to requested neuron count
    std::vector<ScaleLevel> filteredScales;
    for (const auto& scale : config.scales) {
        if (scale.neurons == neuronCount) {
            filteredScales.push_back(scale);
        }
    }
    config.scales = filteredScales;
    
    config.stepsPerScale = steps;
    config.warmupSteps = steps / 10; // 10% warmup
    config.seeds = {seed};
    config.runUntilStable = false;
    
    // Create brain factory
    auto createBrain = [neuronCount](const ScaleLevel& scale) -> std::shared_ptr<class Brain> {
        // Implementation would create brain with specified neuron count
        return nullptr; // Placeholder
    };
    
    // Create simulation runner
    auto runSimulation = [steps](class Brain* brain, uint64_t simulatedSteps) -> bool {
        // Implementation would run simulation
        return true; // Placeholder
    };
    
    // Create metrics collector
    auto collectMetrics = [](class Brain* brain) -> BenchmarkMetrics {
        BenchmarkMetrics metrics;
        metrics.neuronsPerSecond = 1000.0;
        metrics.synapsesPerSecond = 10000.0;
        metrics.spikesPerSecond = 5000.0;
        metrics.realTimeFactor = 1.0;
        metrics.avgStepTimeMs = 1.0;
        metrics.totalMemoryMB = 100;
        metrics.peakMemoryMB = 150;
        metrics.activeNeurons = 5000;
        metrics.firingNeurons = 2000;
        metrics.avgFiringRateHz = 5.0;
        metrics.cpuUtilizationPercent = 50.0;
        metrics.threadCount = 1;
        return metrics;
    };
    
    // Run benchmark
    if (runner.run(config, createBrain, runSimulation, collectMetrics)) {
        if (!runner.getResults().empty()) {
            return runner.getResults()[0];
        }
    }
    
    // Return empty result if failed
    BenchmarkRun result;
    result.name = "benchmark_failed";
    return result;
}

} // namespace

} // namespace nlm

// PerformanceExperiment.cpp - Performance experiment implementation

#include "experiments/PerformanceExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

PerformanceExperiment::PerformanceExperiment()
    : isActive(false) {
}

PerformanceExperiment::PerformanceExperiment(const PerformanceExperimentConfig& config)
    : experimentConfig(config), isActive(false) {
}

PerformanceExperiment::~PerformanceExperiment() {
    if (isActive) {
        stop();
    }
}

bool PerformanceExperiment::start() {
    NLM_LOG_INFO("Starting performance experiment: " + experimentConfig.name);
    
    if (!validateConfiguration()) {
        NLM_LOG_ERROR("Performance experiment validation failed");
        return false;
    }
    
    initializeExperiment();
    setupExperiment();
    
    isActive = true;
    experimentStartTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("Performance experiment started: " + experimentConfig.name);
    
    return true;
}

bool PerformanceExperiment::execute() {
    if (!isActive) {
        NLM_LOG_ERROR("Cannot execute experiment: not active");
        return false;
    }
    
    NLM_LOG_INFO("Executing performance experiment: " + experimentConfig.name);
    
    bool success = false;
    
    switch (experimentConfig.type) {
        case ExperimentType::BASIC_PERFORMANCE:
            success = runBasicPerformanceExperiment();
            break;
        case ExperimentType::MEMORY_PERFORMANCE:
            success = runMemoryPerformanceExperiment();
            break;
        case ExperimentType::THREAD_PERFORMANCE:
            success = runThreadPerformanceExperiment();
            break;
        case ExperimentType::SCALABILITY:
            success = runScalabilityExperiment();
            break;
        case ExperimentType::REGRESSION_TESTING:
            success = runRegressionTestingExperiment();
            break;
        case ExperimentType::OPTIMIZATION:
            success = runOptimizationExperiment();
            break;
        case ExperimentType::BENCHMARKING:
            success = runBenchmarkingExperiment();
            break;
        case ExperimentType::REAL_TIME_MONITORING:
            success = runRealTimeMonitoringExperiment();
            break;
        case ExperimentType::THROUGHTPUT_ANALYSIS:
            success = runThroughputAnalysisExperiment();
            break;
        case ExperimentType::LATENCY_ANALYSIS:
            success = runLatencyAnalysisExperiment();
            break;
        default:
            NLM_LOG_ERROR("Unsupported experiment type: " + std::to_string(static_cast<int>(experimentConfig.type)));
            success = false;
            break;
    }
    
    if (success) {
        analyzeResults();
        validateExperimentResults();
    }
    
    experimentEndTime = std::chrono::steady_clock::now();
    isActive = false;
    
    NLM_LOG_INFO("Performance experiment completed: " + experimentConfig.name + " - " + 
                (success ? "SUCCESS" : "FAILED"));
    
    return success;
}

void PerformanceExperiment::stop() {
    if (!isActive) {
        return;
    }
    
    NLM_LOG_INFO("Stopping performance experiment: " + experimentConfig.name);
    
    isActive = false;
    experimentEndTime = std::chrono::steady_clock::now();
    
    // Generate final analysis
    analyzeResults();
    
    NLM_LOG_INFO("Performance experiment stopped");
}

const std::vector<std::pair<std::string, double>>& PerformanceExperiment::getMetrics() const {
    // TODO: Implement metrics generation from experiment results
    static std::vector<std::pair<std::string, double>> emptyMetrics;
    return emptyMetrics;
}

bool PerformanceExperiment::validateConfiguration() const {
    if (experimentConfig.name.empty()) {
        NLM_LOG_ERROR("Experiment name cannot be empty");
        return false;
    }
    
    if (experimentConfig.duration <= 0) {
        NLM_LOG_ERROR("Experiment duration must be positive");
        return false;
    }
    
    if (experimentConfig.warmupDuration > experimentConfig.duration) {
        NLM_LOG_ERROR("Warm-up duration cannot exceed experiment duration");
        return false;
    }
    
    if (experimentConfig.cooldownDuration > experimentConfig.duration) {
        NLM_LOG_ERROR("Cool-down duration cannot exceed experiment duration");
        return false;
    }
    
    if (experimentConfig.samplingInterval <= 0) {
        NLM_LOG_ERROR("Sampling interval must be positive");
        return false;
    }
    
    if (experimentConfig.warmupSamplingInterval <= 0) {
        NLM_LOG_ERROR("Warm-up sampling interval must be positive");
        return false;
    }
    
    // Validate performance targets
    if (experimentConfig.targetThroughput < 0) {
        NLM_LOG_ERROR("Target throughput cannot be negative");
        return false;
    }
    
    if (experimentConfig.targetLatency < 0) {
        NLM_LOG_ERROR("Target latency cannot be negative");
        return false;
    }
    
    if (experimentConfig.targetMemoryUsage < 0) {
        NLM_LOG_ERROR("Target memory usage cannot be negative");
        return false;
    }
    
    // Validate experiment-specific parameters
    switch (experimentConfig.type) {
        case ExperimentType::THREAD_PERFORMANCE:
            if (experimentConfig.numThreads <= 0) {
                NLM_LOG_ERROR("Number of threads must be positive for thread performance experiments");
                return false;
            }
            break;
        case ExperimentType::SCALABILITY:
            if (experimentConfig.numNeurons <= 0) {
                NLM_LOG_ERROR("Number of neurons must be positive for scalability experiments");
                return false;
            }
            break;
        default:
            break;
    }
    
    return true;
}

bool PerformanceExperiment::runBasicPerformanceExperiment() {
    NLM_LOG_INFO("Running basic performance experiment");
    
    // Execute basic performance operations
    int64_t warmupEndTime = experimentConfig.warmupDuration;
    int64_t experimentEndTime = experimentConfig.duration + experimentConfig.warmupDuration;
    
    // Warm-up phase
    NLM_LOG_INFO("Starting warm-up phase: " + std::to_string(experimentConfig.warmupDuration) + "ms");
    auto warmupStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - warmupStartTime).count() < warmupEndTime) {
        // Basic operations
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.warmupSamplingInterval))));
    }
    
    // Main experiment phase
    NLM_LOG_INFO("Starting main experiment phase: " + std::to_string(experimentConfig.duration) + "ms");
    auto experimentStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - experimentStartTime).count() < experimentEndTime) {
        // Basic performance operations
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    // Cool-down phase
    NLM_LOG_INFO("Starting cool-down phase: " + std::to_string(experimentConfig.cooldownDuration) + "ms");
    auto cooldownStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - cooldownStartTime).count() < experimentConfig.cooldownDuration) {
        // Cool-down operations
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    NLM_LOG_INFO("Basic performance experiment completed successfully");
    return true;
}

bool PerformanceExperiment::runMemoryPerformanceExperiment() {
    NLM_LOG_INFO("Running memory performance experiment");
    
    // Execute memory performance operations
    int64_t warmupEndTime = experimentConfig.warmupDuration;
    int64_t experimentEndTime = experimentConfig.duration + experimentConfig.warmupDuration;
    
    // Warm-up phase
    NLM_LOG_INFO("Starting warm-up phase: " + std::to_string(experimentConfig.warmupDuration) + "ms");
    auto warmupStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - warmupStartTime).count() < warmupEndTime) {
        // Memory allocation operations
        if (profilerIntegration) {
            for (int i = 0; i < 10; ++i) {
                void* ptr = malloc(1024 * 1024); // 1 MB allocation
                if (ptr) {
                    profilerIntegration->recordMemoryAllocation("memory_profiler", ptr, 1024 * 1024, "memory_experiment");
                    free(ptr);
                }
                profilerIntegration->recordMemoryAllocation("memory_profiler", ptr, 0, "memory_experiment");
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.warmupSamplingInterval))));
    }
    
    // Main experiment phase
    NLM_LOG_INFO("Starting main experiment phase: " + std::to_string(experimentConfig.duration) + "ms");
    auto experimentStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - experimentStartTime).count() < experimentEndTime) {
        // Memory performance operations
        if (profilerIntegration) {
            for (int i = 0; i < 50; ++i) {
                void* ptr = malloc(1024 * 1024); // 1 MB allocation
                if (ptr) {
                    profilerIntegration->recordMemoryAllocation("memory_profiler", ptr, 1024 * 1024, "memory_experiment");
                    // Simulate some work
                    memset(ptr, 0, 1024 * 1024);
                    free(ptr);
                }
                profilerIntegration->recordMemoryAllocation("memory_profiler", ptr, 0, "memory_experiment");
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    // Cool-down phase
    NLM_LOG_INFO("Starting cool-down phase: " + std::to_string(experimentConfig.cooldownDuration) + "ms");
    auto cooldownStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - cooldownStartTime).count() < experimentConfig.cooldownDuration) {
        // Cool-down memory operations
        if (profilerIntegration) {
            for (int i = 0; i < 10; ++i) {
                void* ptr = malloc(1024 * 1024); // 1 MB allocation
                if (ptr) {
                    profilerIntegration->recordMemoryAllocation("memory_profiler", ptr, 1024 * 1024, "memory_experiment");
                    free(ptr);
                }
                profilerIntegration->recordMemoryAllocation("memory_profiler", ptr, 0, "memory_experiment");
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    NLM_LOG_INFO("Memory performance experiment completed successfully");
    return true;
}

bool PerformanceExperiment::runThreadPerformanceExperiment() {
    NLM_LOG_INFO("Running thread performance experiment");
    
    std::vector<std::thread> threads;
    int64_t warmupEndTime = experimentConfig.warmupDuration;
    int64_t experimentEndTime = experimentConfig.duration + experimentConfig.warmupDuration;
    
    // Create and start threads
    for (int i = 0; i < experimentConfig.numThreads; ++i) {
        threads.emplace_back([this, i]() {
            if (profilerIntegration) {
                profilerIntegration->recordThreadExecution("thread_profiler", 1000000); // 1 second
            }
        });
    }
    
    // Warm-up phase
    NLM_LOG_INFO("Starting warm-up phase: " + std::to_string(experimentConfig.warmupDuration) + "ms");
    auto warmupStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - warmupStartTime).count() < warmupEndTime) {
        // Thread performance operations
        if (profilerIntegration) {
            for (int i = 0; i < experimentConfig.numThreads; ++i) {
                profilerIntegration->recordThreadExecution("thread_profiler", 10000); // 10 ms
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.warmupSamplingInterval))));
    }
    
    // Main experiment phase
    NLM_LOG_INFO("Starting main experiment phase: " + std::to_string(experimentConfig.duration) + "ms");
    auto experimentStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - experimentStartTime).count() < experimentEndTime) {
        // Thread performance operations
        if (profilerIntegration) {
            for (int i = 0; i < experimentConfig.numThreads; ++i) {
                profilerIntegration->recordThreadExecution("thread_profiler", 100000); // 100 ms
                profilerIntegration->recordSynchronizationEvent("thread_profiler", 
                    ThreadProfiler::SynchronizationEvent{
                        0, std::this_thread::get_id(), "lock", "mutex", 1000, true, "active"
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Cool-down phase
    NLM_LOG_INFO("Starting cool-down phase: " + std::to_string(experimentConfig.cooldownDuration) + "ms");
    auto cooldownStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - cooldownStartTime).count() < experimentConfig.cooldownDuration) {
        // Cool-down thread operations
        if (profilerIntegration) {
            for (int i = 0; i < experimentConfig.numThreads; ++i) {
                profilerIntegration->recordThreadExecution("thread_profiler", 5000); // 5 ms
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    NLM_LOG_INFO("Thread performance experiment completed successfully");
    return true;
}

bool PerformanceExperiment::runScalabilityExperiment() {
    NLM_LOG_INFO("Running scalability experiment");
    
    int64_t warmupEndTime = experimentConfig.warmupDuration;
    int64_t experimentEndTime = experimentConfig.duration + experimentConfig.warmupDuration;
    
    // Warm-up phase
    NLM_LOG_INFO("Starting warm-up phase: " + std::to_string(experimentConfig.warmupDuration) + "ms");
    auto warmupStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - warmupStartTime).count() < warmupEndTime) {
        // Scalability operations
        if (profilerIntegration) {
            for (int i = 0; i < 10; ++i) {
                // Simulate operations for scalability analysis
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        1000.0, // 1000 ms wall clock time
                        800.0, // 800 ms CPU time
                        10 * 1024 * 1024, // 10 MB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        100000.0, // 100k instructions
                        50000.0, // 50k cache hits
                        10000.0, // 10k cache misses
                        1000.0, // 1000 context switches
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.warmupSamplingInterval))));
    }
    
    // Main experiment phase
    NLM_LOG_INFO("Starting main experiment phase: " + std::to_string(experimentConfig.duration) + "ms");
    auto experimentStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - experimentStartTime).count() < experimentEndTime) {
        // Scalability operations
        if (profilerIntegration) {
            for (int i = 0; i < 50; ++i) {
                // Simulate operations for scalability analysis
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        1000.0, // 1000 ms wall clock time
                        800.0, // 800 ms CPU time
                        10 * 1024 * 1024, // 10 MB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        100000.0, // 100k instructions
                        50000.0, // 50k cache hits
                        10000.0, // 10k cache misses
                        1000.0, // 1000 context switches
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    // Cool-down phase
    NLM_LOG_INFO("Starting cool-down phase: " + std::to_string(experimentConfig.cooldownDuration) + "ms");
    auto cooldownStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - cooldownStartTime).count() < experimentConfig.cooldownDuration) {
        // Cool-down scalability operations
        if (profilerIntegration) {
            for (int i = 0; i < 10; ++i) {
                // Simulate operations for scalability analysis
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        100.0, // 100 ms wall clock time
                        80.0, // 80 ms CPU time
                        1 * 1024 * 1024, // 1 MB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        10000.0, // 10k instructions
                        5000.0, // 5k cache hits
                        1000.0, // 1k cache misses
                        100.0, // 100 context switches
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    NLM_LOG_INFO("Scalability experiment completed successfully");
    return true;
}

bool PerformanceExperiment::runRegressionTestingExperiment() {
    NLM_LOG_INFO("Running regression testing experiment");
    
    if (experimentConfig.type != ExperimentType::REGRESSION_TESTING) {
        NLM_LOG_ERROR("Experiment type is not regression testing");
        return false;
    }
    
    // Load baseline data if available
    if (profilerIntegration && !experimentConfig.baselineFile.empty()) {
        if (!profilerIntegration->loadBaselineData(experimentConfig.baselineFile)) {
            NLM_LOG_WARNING("Failed to load baseline data, proceeding without baseline");
        }
    }
    
    // Execute regression testing operations
    int64_t warmupEndTime = experimentConfig.warmupDuration;
    int64_t experimentEndTime = experimentConfig.duration + experimentConfig.warmupDuration;
    
    // Warm-up phase
    NLM_LOG_INFO("Starting warm-up phase: " + std::to_string(experimentConfig.warmupDuration) + "ms");
    auto warmupStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - warmupStartTime).count() < warmupEndTime) {
        // Regression testing operations
        if (profilerIntegration) {
            profilerIntegration->recordPerformanceData("performance_profiler", 
                PerformanceProfiler::PerformanceData{
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count(),
                    1000.0, // 1000 ms wall clock time
                    800.0, // 800 ms CPU time
                    10 * 1024 * 1024, // 10 MB memory
                    0.0, // 0 allocated
                    0.0, // 0 freed
                    {}, // empty thread data
                    {}, // empty IO data
                    {}, // empty network data
                    0.0, // 0 GPU time
                    100000.0, // 100k instructions
                    50000.0, // 50k cache hits
                    10000.0, // 10k cache misses
                    1000.0, // 1000 context switches
                    {} // empty custom metrics
                });
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.warmupSamplingInterval))));
    }
    
    // Main experiment phase
    NLM_LOG_INFO("Starting main experiment phase: " + std::to_string(experimentConfig.duration) + "ms");
    auto experimentStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - experimentStartTime).count() < experimentEndTime) {
        // Regression testing operations
        if (profilerIntegration) {
            profilerIntegration->recordPerformanceData("performance_profiler", 
                PerformanceProfiler::PerformanceData{
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count(),
                    1000.0, // 1000 ms wall clock time
                    800.0, // 800 ms CPU time
                    10 * 1024 * 1024, // 10 MB memory
                    0.0, // 0 allocated
                    0.0, // 0 freed
                    {}, // empty thread data
                    {}, // empty IO data
                    {}, // empty network data
                    0.0, // 0 GPU time
                    100000.0, // 100k instructions
                    50000.0, // 50k cache hits
                    10000.0, // 10k cache misses
                    1000.0, // 1000 context switches
                    {} // empty custom metrics
                });
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    // Cool-down phase
    NLM_LOG_INFO("Starting cool-down phase: " + std::to_string(experimentConfig.cooldownDuration) + "ms");
    auto cooldownStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - cooldownStartTime).count() < experimentConfig.cooldownDuration) {
        // Cool-down regression testing operations
        if (profilerIntegration) {
            profilerIntegration->recordPerformanceData("performance_profiler", 
                PerformanceProfiler::PerformanceData{
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count(),
                    100.0, // 100 ms wall clock time
                    80.0, // 80 ms CPU time
                    1 * 1024 * 1024, // 1 MB memory
                    0.0, // 0 allocated
                    0.0, // 0 freed
                    {}, // empty thread data
                    {}, // empty IO data
                    {}, // empty network data
                    0.0, // 0 GPU time
                    10000.0, // 10k instructions
                    5000.0, // 5k cache hits
                    1000.0, // 1k cache misses
                    100.0, // 100 context switches
                    {} // empty custom metrics
                });
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    NLM_LOG_INFO("Regression testing experiment completed successfully");
    return true;
}

bool PerformanceExperiment::runOptimizationExperiment() {
    NLM_LOG_INFO("Running optimization experiment");
    
    if (experimentConfig.type != ExperimentType::OPTIMIZATION) {
        NLM_LOG_ERROR("Experiment type is not optimization");
        return false;
    }
    
    // Execute optimization operations
    int64_t warmupEndTime = experimentConfig.warmupDuration;
    int64_t experimentEndTime = experimentConfig.duration + experimentConfig.warmupDuration;
    
    // Warm-up phase
    NLM_LOG_INFO("Starting warm-up phase: " + std::to_string(experimentConfig.warmupDuration) + "ms");
    auto warmupStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - warmupStartTime).count() < warmupEndTime) {
        // Optimization operations
        if (profilerIntegration) {
            profilerIntegration->recordPerformanceData("performance_profiler", 
                PerformanceProfiler::PerformanceData{
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count(),
                    1000.0, // 1000 ms wall clock time
                    800.0, // 800 ms CPU time
                    10 * 1024 * 1024, // 10 MB memory
                    0.0, // 0 allocated
                    0.0, // 0 freed
                    {}, // empty thread data
                    {}, // empty IO data
                    {}, // empty network data
                    0.0, // 0 GPU time
                    100000.0, // 100k instructions
                    50000.0, // 50k cache hits
                    10000.0, // 10k cache misses
                    1000.0, // 1000 context switches
                    {} // empty custom metrics
                });
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.warmupSamplingInterval))));
    }
    
    // Main experiment phase
    NLM_LOG_INFO("Starting main experiment phase: " + std::to_string(experimentConfig.duration) + "ms");
    auto experimentStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - experimentStartTime).count() < experimentEndTime) {
        // Optimization operations
        if (profilerIntegration) {
            profilerIntegration->recordPerformanceData("performance_profiler", 
                PerformanceProfiler::PerformanceData{
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count(),
                    1000.0, // 1000 ms wall clock time
                    800.0, // 800 ms CPU time
                    10 * 1024 * 1024, // 10 MB memory
                    0.0, // 0 allocated
                    0.0, // 0 freed
                    {}, // empty thread data
                    {}, // empty IO data
                    {}, // empty network data
                    0.0, // 0 GPU time
                    100000.0, // 100k instructions
                    50000.0, // 50k cache hits
                    10000.0, // 10k cache misses
                    1000.0, // 1000 context switches
                    {} // empty custom metrics
                });
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    // Cool-down phase
    NLM_LOG_INFO("Starting cool-down phase: " + std::to_string(experimentConfig.cooldownDuration) + "ms");
    auto cooldownStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - cooldownStartTime).count() < experimentConfig.cooldownDuration) {
        // Cool-down optimization operations
        if (profilerIntegration) {
            profilerIntegration->recordPerformanceData("performance_profiler", 
                PerformanceProfiler::PerformanceData{
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count(),
                    100.0, // 100 ms wall clock time
                    80.0, // 80 ms CPU time
                    1 * 1024 * 1024, // 1 MB memory
                    0.0, // 0 allocated
                    0.0, // 0 freed
                    {}, // empty thread data
                    {}, // empty IO data
                    {}, // empty network data
                    0.0, // 0 GPU time
                    10000.0, // 10k instructions
                    5000.0, // 5k cache hits
                    1000.0, // 1k cache misses
                    100.0, // 100 context switches
                    {} // empty custom metrics
                });
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    NLM_LOG_INFO("Optimization experiment completed successfully");
    return true;
}

bool PerformanceExperiment::runBenchmarkingExperiment() {
    NLM_LOG_INFO("Running benchmarking experiment");
    
    if (experimentConfig.type != ExperimentType::BENCHMARKING) {
        NLM_LOG_ERROR("Experiment type is not benchmarking");
        return false;
    }
    
    // Execute benchmarking operations
    int64_t warmupEndTime = experimentConfig.warmupDuration;
    int64_t experimentEndTime = experimentConfig.duration + experimentConfig.warmupDuration;
    
    // Warm-up phase
    NLM_LOG_INFO("Starting warm-up phase: " + std::to_string(experimentConfig.warmupDuration) + "ms");
    auto warmupStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - warmupStartTime).count() < warmupEndTime) {
        // Benchmarking operations
        if (profilerIntegration) {
            for (int i = 0; i < 10; ++i) {
                // Simulate benchmark operations
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        100.0, // 100 ms wall clock time
                        80.0, // 80 ms CPU time
                        5 * 1024 * 1024, // 5 MB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        10000.0, // 10k instructions
                        5000.0, // 5k cache hits
                        1000.0, // 1k cache misses
                        100.0, // 100 context switches
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.warmupSamplingInterval))));
    }
    
    // Main experiment phase
    NLM_LOG_INFO("Starting main experiment phase: " + std::to_string(experimentConfig.duration) + "ms");
    auto experimentStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - experimentStartTime).count() < experimentEndTime) {
        // Benchmarking operations
        if (profilerIntegration) {
            for (int i = 0; i < 50; ++i) {
                // Simulate benchmark operations
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        100.0, // 100 ms wall clock time
                        80.0, // 80 ms CPU time
                        5 * 1024 * 1024, // 5 MB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        10000.0, // 10k instructions
                        5000.0, // 5k cache hits
                        1000.0, // 1k cache misses
                        100.0, // 100 context switches
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    // Cool-down phase
    NLM_LOG_INFO("Starting cool-down phase: " + std::to_string(experimentConfig.cooldownDuration) + "ms");
    auto cooldownStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - cooldownStartTime).count() < experimentConfig.cooldownDuration) {
        // Cool-down benchmarking operations
        if (profilerIntegration) {
            for (int i = 0; i < 10; ++i) {
                // Simulate benchmark operations
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        10.0, // 10 ms wall clock time
                        8.0, // 8 ms CPU time
                        500 * 1024, // 500 KB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        1000.0, // 1k instructions
                        500.0, // 500 cache hits
                        100.0, // 100 cache misses
                        10.0, // 10 context switches
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    NLM_LOG_INFO("Benchmarking experiment completed successfully");
    return true;
}

bool PerformanceExperiment::runRealTimeMonitoringExperiment() {
    NLM_LOG_INFO("Running real-time monitoring experiment");
    
    if (experimentConfig.type != ExperimentType::REAL_TIME_MONITORING) {
        NLM_LOG_ERROR("Experiment type is not real-time monitoring");
        return false;
    }
    
    // Execute real-time monitoring operations
    int64_t warmupEndTime = experimentConfig.warmupDuration;
    int64_t experimentEndTime = experimentConfig.duration + experimentConfig.warmupDuration;
    
    // Warm-up phase
    NLM_LOG_INFO("Starting warm-up phase: " + std::to_string(experimentConfig.warmupDuration) + "ms");
    auto warmupStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - warmupStartTime).count() < warmupEndTime) {
        // Real-time monitoring operations
        if (profilerIntegration) {
            profilerIntegration->recordPerformanceData("performance_profiler", 
                PerformanceProfiler::PerformanceData{
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count(),
                    1000.0, // 1000 ms wall clock time
                    800.0, // 800 ms CPU time
                    10 * 1024 * 1024, // 10 MB memory
                    0.0, // 0 allocated
                    0.0, // 0 freed
                    {}, // empty thread data
                    {}, // empty IO data
                    {}, // empty network data
                    0.0, // 0 GPU time
                    100000.0, // 100k instructions
                    50000.0, // 50k cache hits
                    10000.0, // 10k cache misses
                    1000.0, // 1000 context switches
                    {} // empty custom metrics
                });
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.warmupSamplingInterval))));
    }
    
    // Main experiment phase
    NLM_LOG_INFO("Starting main experiment phase: " + std::to_string(experimentConfig.duration) + "ms");
    auto experimentStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - experimentStartTime).count() < experimentEndTime) {
        // Real-time monitoring operations
        if (profilerIntegration) {
            profilerIntegration->recordPerformanceData("performance_profiler", 
                PerformanceProfiler::PerformanceData{
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count(),
                    1000.0, // 1000 ms wall clock time
                    800.0, // 800 ms CPU time
                    10 * 1024 * 1024, // 10 MB memory
                    0.0, // 0 allocated
                    0.0, // 0 freed
                    {}, // empty thread data
                    {}, // empty IO data
                    {}, // empty network data
                    0.0, // 0 GPU time
                    100000.0, // 100k instructions
                    50000.0, // 50k cache hits
                    10000.0, // 10k cache misses
                    1000.0, // 1000 context switches
                    {} // empty custom metrics
                });
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    // Cool-down phase
    NLM_LOG_INFO("Starting cool-down phase: " + std::to_string(experimentConfig.cooldownDuration) + "ms");
    auto cooldownStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - cooldownStartTime).count() < experimentConfig.cooldownDuration) {
        // Cool-down real-time monitoring operations
        if (profilerIntegration) {
            profilerIntegration->recordPerformanceData("performance_profiler", 
                PerformanceProfiler::PerformanceData{
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count(),
                    100.0, // 100 ms wall clock time
                    80.0, // 80 ms CPU time
                    1 * 1024 * 1024, // 1 MB memory
                    0.0, // 0 allocated
                    0.0, // 0 freed
                    {}, // empty thread data
                    {}, // empty IO data
                    {}, // empty network data
                    0.0, // 0 GPU time
                    10000.0, // 10k instructions
                    5000.0, // 5k cache hits
                    1000.0, // 1k cache misses
                    100.0, // 100 context switches
                    {} // empty custom metrics
                });
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    NLM_LOG_INFO("Real-time monitoring experiment completed successfully");
    return true;
}

bool PerformanceExperiment::runThroughputAnalysisExperiment() {
    NLM_LOG_INFO("Running throughput analysis experiment");
    
    if (experimentConfig.type != ExperimentType::THROUGHTPUT_ANALYSIS) {
        NLM_LOG_ERROR("Experiment type is not throughput analysis");
        return false;
    }
    
    // Execute throughput analysis operations
    int64_t warmupEndTime = experimentConfig.warmupDuration;
    int64_t experimentEndTime = experimentConfig.duration + experimentConfig.warmupDuration;
    
    // Warm-up phase
    NLM_LOG_INFO("Starting warm-up phase: " + std::to_string(experimentConfig.warmupDuration) + "ms");
    auto warmupStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - warmupStartTime).count() < warmupEndTime) {
        // Throughput analysis operations
        if (profilerIntegration) {
            for (int i = 0; i < 100; ++i) {
                // Simulate throughput analysis operations
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        10.0, // 10 ms wall clock time
                        8.0, // 8 ms CPU time
                        500 * 1024, // 500 KB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        1000.0, // 1k instructions
                        500.0, // 500 cache hits
                        100.0, // 100 cache misses
                        10.0, // 10 context switches
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.warmupSamplingInterval))));
    }
    
    // Main experiment phase
    NLM_LOG_INFO("Starting main experiment phase: " + std::to_string(experimentConfig.duration) + "ms");
    auto experimentStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - experimentStartTime).count() < experimentEndTime) {
        // Throughput analysis operations
        if (profilerIntegration) {
            for (int i = 0; i < 500; ++i) {
                // Simulate throughput analysis operations
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        10.0, // 10 ms wall clock time
                        8.0, // 8 ms CPU time
                        500 * 1024, // 500 KB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        1000.0, // 1k instructions
                        500.0, // 500 cache hits
                        100.0, // 100 cache misses
                        10.0, // 10 context switches
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    // Cool-down phase
    NLM_LOG_INFO("Starting cool-down phase: " + std::to_string(experimentConfig.cooldownDuration) + "ms");
    auto cooldownStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - cooldownStartTime).count() < experimentConfig.cooldownDuration) {
        // Cool-down throughput analysis operations
        if (profilerIntegration) {
            for (int i = 0; i < 100; ++i) {
                // Simulate throughput analysis operations
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        1.0, // 1 ms wall clock time
                        0.8, // 0.8 ms CPU time
                        100 * 1024, // 100 KB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        100.0, // 100 instructions
                        50.0, // 50 cache hits
                        10.0, // 10 cache misses
                        1.0, // 1 context switch
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    NLM_LOG_INFO("Throughput analysis experiment completed successfully");
    return true;
}

bool PerformanceExperiment::runLatencyAnalysisExperiment() {
    NLM_LOG_INFO("Running latency analysis experiment");
    
    if (experimentConfig.type != ExperimentType::LATENCY_ANALYSIS) {
        NLM_LOG_ERROR("Experiment type is not latency analysis");
        return false;
    }
    
    // Execute latency analysis operations
    int64_t warmupEndTime = experimentConfig.warmupDuration;
    int64_t experimentEndTime = experimentConfig.duration + experimentConfig.warmupDuration;
    
    // Warm-up phase
    NLM_LOG_INFO("Starting warm-up phase: " + std::to_string(experimentConfig.warmupDuration) + "ms");
    auto warmupStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - warmupStartTime).count() < warmupEndTime) {
        // Latency analysis operations
        if (profilerIntegration) {
            for (int i = 0; i < 50; ++i) {
                // Simulate latency analysis operations with varying latencies
                double latency = 1.0 + (rand() % 100) / 100.0; // 1.0-10.0 ms
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        latency, // variable latency
                        latency * 0.8, // 80% CPU time
                        1 * 1024 * 1024, // 1 MB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        1000.0, // 1k instructions
                        500.0, // 500 cache hits
                        100.0, // 100 cache misses
                        10.0, // 10 context switches
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.warmupSamplingInterval))));
    }
    
    // Main experiment phase
    NLM_LOG_INFO("Starting main experiment phase: " + std::to_string(experimentConfig.duration) + "ms");
    auto experimentStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - experimentStartTime).count() < experimentEndTime) {
        // Latency analysis operations
        if (profilerIntegration) {
            for (int i = 0; i < 200; ++i) {
                // Simulate latency analysis operations with varying latencies
                double latency = 1.0 + (rand() % 100) / 100.0; // 1.0-10.0 ms
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        latency, // variable latency
                        latency * 0.8, // 80% CPU time
                        1 * 1024 * 1024, // 1 MB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        1000.0, // 1k instructions
                        500.0, // 500 cache hits
                        100.0, // 100 cache misses
                        10.0, // 10 context switches
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    // Cool-down phase
    NLM_LOG_INFO("Starting cool-down phase: " + std::to_string(experimentConfig.cooldownDuration) + "ms");
    auto cooldownStartTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - cooldownStartTime).count() < experimentConfig.cooldownDuration) {
        // Cool-down latency analysis operations
        if (profilerIntegration) {
            for (int i = 0; i < 50; ++i) {
                // Simulate latency analysis operations with low latencies
                double latency = 0.1 + (rand() % 50) / 100.0; // 0.1-1.0 ms
                profilerIntegration->recordPerformanceData("performance_profiler", 
                    PerformanceProfiler::PerformanceData{
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch()).count(),
                        latency, // variable latency
                        latency * 0.8, // 80% CPU time
                        100 * 1024, // 100 KB memory
                        0.0, // 0 allocated
                        0.0, // 0 freed
                        {}, // empty thread data
                        {}, // empty IO data
                        {}, // empty network data
                        0.0, // 0 GPU time
                        100.0, // 100 instructions
                        50.0, // 50 cache hits
                        10.0, // 10 cache misses
                        1.0, // 1 context switch
                        {} // empty custom metrics
                    });
            }
        }
        
        collectPerformanceData();
        
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(experimentConfig.samplingInterval))));
    }
    
    NLM_LOG_INFO("Latency analysis experiment completed successfully");
    return true;
}

void PerformanceExperiment::initializeExperiment() {
    NLM_LOG_INFO("Initializing performance experiment: " + experimentConfig.name);
    
    // Initialize experiment components
    if (profilerIntegration) {
        // Configure profiler integration
        ProfilerIntegration::PerformanceConfig profilerConfig;
        profilerConfig.enableCoreProfiler = experimentConfig.type != ExperimentType::MEMORY_PERFORMANCE && 
                                            experimentConfig.type != ExperimentType::THREAD_PERFORMANCE;
        profilerConfig.enableMemoryProfiler = experimentConfig.type == ExperimentType::MEMORY_PERFORMANCE;
        profilerConfig.enableThreadProfiler = experimentConfig.type == ExperimentType::THREAD_PERFORMANCE;
        profilerConfig.enableBenchmarking = experimentConfig.type == ExperimentType::BENCHMARKING;
        profilerConfig.enableVisualization = true;
        profilerConfig.enableRealTimeMonitoring = experimentConfig.enableRealTimeMonitoring;
        profilerConfig.samplingIntervalMs = experimentConfig.samplingInterval;
        profilerConfig.sessionName = experimentConfig.name;
        
        profilerIntegration->configure(profilerConfig);
    }
    
    // Initialize experiment-specific parameters
    switch (experimentConfig.type) {
        case ExperimentType::THREAD_PERFORMANCE:
            notes.push_back("Thread performance experiment with " + std::to_string(experimentConfig.numThreads) + " threads");
            break;
        case ExperimentType::SCALABILITY:
            notes.push_back("Scalability experiment with " + std::to_string(experimentConfig.numNeurons) + " neurons");
            break;
        default:
            notes.push_back("Performance experiment: " + std::to_string(experimentConfig.type));
            break;
    }
    
    notes.push_back("Duration: " + std::to_string(experimentConfig.duration) + "ms");
    notes.push_back("Sampling interval: " + std::to_string(experimentConfig.samplingInterval) + "ms");
}

void PerformanceExperiment::setupExperiment() {
    NLM_LOG_INFO("Setting up performance experiment: " + experimentConfig.name);
    
    // Setup experiment environment
    if (profilerIntegration) {
        profilerIntegration->startProfiling(experimentConfig.name);
    }
    
    notes.push_back("Experiment setup completed");
}

void PerformanceExperiment::analyzeResults() {
    NLM_LOG_INFO("Analyzing experiment results: " + experimentConfig.name);
    
    // Analyze performance data
    if (profilerIntegration) {
        // Get current performance data
        PerformanceProfiler::PerformanceData currentData = profilerIntegration->getCurrentPerformanceData();
        
        // Calculate performance metrics
        double totalWallClock = 0.0;
        double totalCPUTime = 0.0;
        double totalMemory = 0.0;
        size_t sampleCount = 0;
        
        // Note: In a real implementation, we would have access to the full performance history
        // For now, we'll use the current data as a sample
        totalWallClock = currentData.wallClockTime;
        totalCPUTime = currentData.cpuTime;
        totalMemory = currentData.memoryUsage;
        sampleCount = 1;
        
        if (sampleCount > 0) {
            result.avgLatency = totalWallClock / sampleCount;
            result.p95Latency = totalWallClock * 0.95;
            result.p99Latency = totalWallClock * 0.99;
            result.throughput = (sampleCount * 1000.0) / totalWallClock; // operations per second
            result.avgMemoryUsage = totalMemory / sampleCount;
            result.peakMemoryUsage = totalMemory;
            result.cpuUtilization = (totalCPUTime / totalWallClock) * 100.0;
        }
        
        // Calculate variation metrics
        if (sampleCount > 1) {
            // In a real implementation, we would calculate actual variation from multiple samples
            result.throughputVariation = 0.1; // placeholder
        }
    }
    
    // Generate performance bottlenecks
    if (profilerIntegration) {
        auto bottlenecks = profilerIntegration->getPerformanceBottlenecks();
        detectedBottlenecks.insert(detectedBottlenecks.end(), bottlenecks.begin(), bottlenecks.end());
    }
    
    // Generate optimization recommendations
    if (profilerIntegration) {
        auto recommendations = profilerIntegration->getOptimizationRecommendations();
        optimizationRecommendations.insert(optimizationRecommendations.end(), recommendations.begin(), recommendations.end());
    }
    
    notes.push_back("Results analysis completed");
}

void PerformanceExperiment::validateExperimentResults() const {
    NLM_LOG_INFO("Validating experiment results: " + experimentConfig.name);
    
    // Validate performance targets
    if (experimentConfig.targetThroughput > 0 && result.throughput > 0) {
        double throughputRatio = result.throughput / experimentConfig.targetThroughput;
        if (throughputRatio < 0.5) {
            result.errorMessage = "Throughput below target (" + std::to_string(throughputRatio * 100) + "%)";
            result.passed = false;
        }
    }
    
    if (experimentConfig.targetLatency > 0 && result.avgLatency > 0) {
        double latencyRatio = result.avgLatency / experimentConfig.targetLatency;
        if (latencyRatio > 2.0) {
            if (result.errorMessage.empty()) {
                result.errorMessage = "Latency above target (" + std::to_string(latencyRatio * 100) + "%)";
            } else {
                result.errorMessage += ", latency above target (" + std::to_string(latencyRatio * 100) + "%)";
            }
            result.passed = false;
        }
    }
    
    if (experimentConfig.targetMemoryUsage > 0 && result.avgMemoryUsage > 0) {
        double memoryRatio = result.avgMemoryUsage / experimentConfig.targetMemoryUsage;
        if (memoryRatio > 2.0) {
            if (result.errorMessage.empty()) {
                result.errorMessage = "Memory usage above target (" + std::to_string(memoryRatio * 100) + "%)";
            } else {
                result.errorMessage += ", memory usage above target (" + std::to_string(memoryRatio * 100) + "%)";
            }
            result.passed = false;
        }
    }
    
    // If no errors, experiment passed
    if (result.errorMessage.empty()) {
        result.passed = true;
        notes.push_back("Experiment validation passed");
    } else {
        notes.push_back("Experiment validation failed: " + result.errorMessage);
    }
}

void PerformanceExperiment::generateRecommendations() {
    // Generate optimization recommendations based on experiment results
    if (result.throughput < 1000.0) {
        optimizationRecommendations.push_back("Consider optimizing code to improve throughput");
    }
    
    if (result.avgLatency > 100.0) {
        optimizationRecommendations.push_back("Consider reducing latency through algorithmic optimization");
    }
    
    if (result.peakMemoryUsage > 1024 * 1024 * 1024) { // > 1GB
        optimizationRecommendations.push_back("Consider memory optimization to reduce peak memory usage");
    }
    
    if (result.cpuUtilization > 80.0) {
        optimizationRecommendations.push_back("Consider CPU optimization to improve utilization");
    }
}

void PerformanceExperiment::generateBottlenecks() {
    // Generate performance bottleneck detection
    if (result.throughput < 500.0) {
        detectedBottlenecks.push_back("Low throughput detected (< 500 ops/sec)");
    }
    
    if (result.avgLatency > 50.0) {
        detectedBottlenecks.push_back("High latency detected (> 50 ms)");
    }
    
    if (result.peakMemoryUsage > 512 * 1024 * 1024) { // > 512MB
        detectedBottlenecks.push_back("High memory usage detected (> 512MB)");
    }
    
    if (result.cpuUtilization > 70.0) {
        detectedBottlenecks.push_back("High CPU utilization detected (> 70%)");
    }
}

void PerformanceExperiment::generateWarnings() {
    // Generate performance warnings
    if (result.throughput < 100.0) {
        notes.push_back("Warning: Very low throughput detected");
    }
    
    if (result.avgLatency > 200.0) {
        notes.push_back("Warning: High latency detected");
    }
    
    if (result.peakMemoryUsage > 2 * 1024 * 1024 * 1024) { // > 2GB
        notes.push_back("Warning: Very high memory usage detected");
    }
}

double PerformanceExperiment::calculateThroughput(
    const std::vector<std::pair<int64_t, double>>& timeline) const {
    if (timeline.empty()) {
        return 0.0;
    }
    
    double totalTime = timeline.back().first - timeline.front().first;
    if (totalTime <= 0) {
        return 0.0;
    }
    
    double totalThroughput = 0.0;
    for (const auto& point : timeline) {
        totalThroughput += point.second;
    }
    
    return (totalThroughput / timeline.size()) / (totalTime / 1000.0); // ops per second
}

double PerformanceExperiment::calculateLatency(
    const std::vector<std::pair<int64_t, double>>& timeline) const {
    if (timeline.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (const auto& point : timeline) {
        sum += point.second;
    }
    
    return sum / timeline.size();
}

double PerformanceExperiment::calculateMemoryUsage(
    const std::vector<std::pair<int64_t, double>>& timeline) const {
    if (timeline.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (const auto& point : timeline) {
        sum += point.second;
    }
    
    return sum / timeline.size();
}

void PerformanceExperiment::collectPerformanceData() {
    if (profilerIntegration) {
        // Collect performance data from profiler integration
        PerformanceProfiler::PerformanceData currentData = profilerIntegration->getCurrentPerformanceData();
        
        // Add to performance history
        std::lock_guard<std::mutex> lock(dataMutex);
        performanceHistory.push_back(currentData);
        
        // Update experiment result metrics
        result.throughputTimeline.push_back(std::make_pair(currentData.timestamp, currentData.wallClockTime));
        result.latencyTimeline.push_back(std::make_pair(currentData.timestamp, currentData.cpuTime));
        result.memoryUsageTimeline.push_back(std::make_pair(currentData.timestamp, currentData.memoryUsage));
    }
}

std::string PerformanceExperiment::generateReport(const std::string& format) const {
    std::ostringstream oss;
    
    if (format == "json" || format == "auto") {
        oss << generateJsonReport();
    } else if (format == "csv" || format == "auto") {
        oss << generateCsvReport();
    } else {
        oss << generateTextReport();
    }
    
    return oss.str();
}

bool PerformanceExperiment::exportResults(const std::string& filename, const std::string& format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    std::string data;
    if (format == "json") {
        data = generateJsonReport();
    } else if (format == "csv") {
        data = generateCsvReport();
    } else {
        data = generateTextReport();
    }
    
    file << data;
    file.close();
    
    NLM_LOG_INFO("Experiment results exported to: " + filename);
    return true;
}

std::string PerformanceExperiment::compareWithBaseline(
    const PerformanceExperimentResult& baseline) const {
    std::ostringstream oss;
    oss << "Experiment Comparison: " << experimentConfig.name << "\n";
    oss << "======================\n\n";
    
    oss << "Baseline Performance:\n";
    oss << "  Throughput: " << baseline.throughput << " ops/sec\n";
    oss << "  Latency: " << baseline.avgLatency << " ms\n";
    oss << "  Memory: " << baseline.avgMemoryUsage << " bytes\n\n";
    
    oss << "Current Performance:\n";
    oss << "  Throughput: " << result.throughput << " ops/sec\n";
    oss << "  Latency: " << result.avgLatency << " ms\n";
    oss << "  Memory: " << result.avgMemoryUsage << " bytes\n\n";
    
    if (baseline.throughput > 0) {
        double throughputRatio = result.throughput / baseline.throughput;
        oss << "Throughput Ratio: " << throughputRatio << "x\n";
        
        if (throughputRatio < 0.9) {
            oss << "⚠ Regression detected (current < baseline)\n";
        } else if (throughputRatio > 1.1) {
            oss << "✓ Improvement detected (current > baseline)\n";
        }
    }
    
    if (baseline.avgLatency > 0) {
        double latencyRatio = result.avgLatency / baseline.avgLatency;
        oss << "Latency Ratio: " << latencyRatio << "x\n";
        
        if (latencyRatio > 1.1) {
            oss << "⚠ Regression detected (current > baseline)\n";
        } else if (latencyRatio < 0.9) {
            oss << "✓ Improvement detected (current < baseline)\n";
        }
    }
    
    if (baseline.avgMemoryUsage > 0) {
        double memoryRatio = result.avgMemoryUsage / baseline.avgMemoryUsage;
        oss << "Memory Ratio: " << memoryRatio << "x\n";
        
        if (memoryRatio > 1.1) {
            oss << "⚠ Regression detected (current > baseline)\n";
        } else if (memoryRatio < 0.9) {
            oss << "✓ Improvement detected (current < baseline)\n";
        }
    }
    
    return oss.str();
}

bool PerformanceExperiment::detectRegressions(
    const PerformanceExperimentResult& baseline) const {
    if (baseline.throughput <= 0 || result.throughput <= 0) {
        return false;
    }
    
    double throughputRatio = result.throughput / baseline.throughput;
    double latencyRatio = result.avgLatency / baseline.avgLatency;
    double memoryRatio = result.avgMemoryUsage / baseline.avgMemoryUsage;
    
    return throughputRatio < 0.9 || 
           latencyRatio > 1.1 || 
           memoryRatio > 1.1;
}

bool PerformanceExperiment::detectOptimizations(
    const PerformanceExperimentResult& baseline) const {
    if (baseline.throughput <= 0 || result.throughput <= 0) {
        return false;
    }
    
    double throughputRatio = result.throughput / baseline.throughput;
    double latencyRatio = result.avgLatency / baseline.avgLatency;
    double memoryRatio = result.avgMemoryUsage / baseline.avgMemoryUsage;
    
    return throughputRatio > 1.1 || 
           latencyRatio < 0.9 || 
           memoryRatio < 0.9;
}

std::string PerformanceExperiment::generateJsonReport() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"experimentName\": \"" << experimentConfig.name << "\",\n";
    oss << "  \"experimentType\": \"" << experimentTypeToString(experimentConfig.type) << "\",\n";
    oss << "  \"durationMs\": " << result.duration << ",\n";
    oss << "  \"passed\": " << (result.passed ? "true" : "false") << ",\n";
    
    if (!result.errorMessage.empty()) {
        oss << "  \"errorMessage\": \"" << escapeJsonString(result.errorMessage) << "\",\n";
    }
    
    oss << "  \"performanceMetrics\": {\n";
    oss << "    \"throughput\": " << result.throughput << ",\n";
    oss << "    \"avgLatency\": " << result.avgLatency << ",\n";
    oss << "    \"p95Latency\": " << result.p95Latency << ",\n";
    oss << "    \"p99Latency\": " << result.p99Latency << ",\n";
    oss << "    \"throughputVariation\": " << result.throughputVariation << ",\n";
    oss << "    \"avgMemoryUsage\": " << result.avgMemoryUsage << ",\n";
    oss << "    \"peakMemoryUsage\": " << result.peakMemoryUsage << ",\n";
    oss << "    \"cpuUtilization\": " << result.cpuUtilization << "\n";
    oss << "  },\n";
    
    oss << "  \"threadPerformance\": {\n";
    oss << "    \"activeThreads\": " << result.activeThreads << ",\n";
    oss << "    \"threadSynchronizationOverhead\": " << result.threadSynchronizationOverhead << ",\n";
    oss << "    \"contextSwitches\": " << result.contextSwitches << "\n";
    oss << "  },\n";
    
    oss << "  \"statisticalAnalysis\": {\n";
    oss << "    \"meanThroughput\": " << result.meanThroughput << ",\n";
    oss << "    \"stdDevThroughput\": " << result.stdDevThroughput << ",\n";
    oss << "    \"meanLatency\": " << result.meanLatency << ",\n";
    oss << "    \"stdDevLatency\": " << result.stdDevLatency << ",\n";
    oss << "    \"meanMemoryUsage\": " << result.meanMemoryUsage << ",\n";
    oss << "    \"stdDevMemoryUsage\": " << result.stdDevMemoryUsage << "\n";
    oss << "  }\n";
    
    oss << "}\n";
    
    return oss.str();
}

std::string PerformanceExperiment::generateCsvReport() const {
    std::ostringstream oss;
    oss << "metric,value\n";
    oss << "experiment_name," << experimentConfig.name << "\n";
    oss << "experiment_type," << experimentTypeToString(experimentConfig.type) << "\n";
    oss << "duration_ms," << result.duration << "\n";
    oss << "passed," << (result.passed ? "true" : "false") << "\n";
    oss << "throughput," << result.throughput << "\n";
    oss << "avg_latency," << result.avgLatency << "\n";
    oss << "p95_latency," << result.p95Latency << "\n";
    oss << "p99_latency," << result.p99Latency << "\n";
    oss << "throughput_variation," << result.throughputVariation << "\n";
    oss << "avg_memory_usage," << result.avgMemoryUsage << "\n";
    oss << "peak_memory_usage," << result.peakMemoryUsage << "\n";
    oss << "cpu_utilization," << result.cpuUtilization << "\n";
    
    return oss.str();
}

std::string PerformanceExperiment::generateTextReport() const {
    std::ostringstream oss;
    oss << "Performance Experiment Report\n";
    oss << "==========================\n\n";
    oss << "Experiment: " << experimentConfig.name << "\n";
    oss << "Type: " << experimentTypeToString(experimentConfig.type) << "\n";
    oss << "Duration: " << result.duration << "ms\n";
    oss << "Status: " << (result.passed ? "PASSED" : "FAILED") << "\n";
    
    if (!result.errorMessage.empty()) {
        oss << "Error: " << result.errorMessage << "\n\n";
    }
    
    oss << "Performance Metrics:\n";
    oss << "  Throughput: " << result.throughput << " ops/sec\n";
    oss << "  Average Latency: " << result.avgLatency << " ms\n";
    oss << "  95th Percentile Latency: " << result.p95Latency << " ms\n";
    oss << "  99th Percentile Latency: " << result.p99Latency << " ms\n";
    oss << "  Throughput Variation: " << result.throughputVariation << "\n";
    oss << "  Average Memory Usage: " << result.avgMemoryUsage << " bytes\n";
    oss << "  Peak Memory Usage: " << result.peakMemoryUsage << " bytes\n";
    oss << "  CPU Utilization: " << result.cpuUtilization << "%\n\n";
    
    oss << "Thread Performance:\n";
    oss << "  Active Threads: " << result.activeThreads << "\n";
    oss << "  Thread Synchronization Overhead: " << result.threadSynchronizationOverhead << " ms\n";
    oss << "  Context Switches: " << result.contextSwitches << "\n\n";
    
    oss << "Statistical Analysis:\n";
    oss << "  Mean Throughput: " << result.meanThroughput << "\n";
    oss << "  Std Dev Throughput: " << result.stdDevThroughput << "\n";
    oss << "  Mean Latency: " << result.meanLatency << "\n";
    oss << "  Std Dev Latency: " << result.stdDevLatency << "\n";
    oss << "  Mean Memory Usage: " << result.meanMemoryUsage << "\n";
    oss << "  Std Dev Memory Usage: " << result.stdDevMemoryUsage << "\n\n";
    
    oss << "Bottlenecks:\n";
    for (const auto& bottleneck : detectedBottlenecks) {
        oss << "  - " << bottleneck << "\n";
    }
    if (detectedBottlenecks.empty()) {
        oss << "  None detected\n";
    }
    
    oss << "Recommendations:\n";
    for (const auto& recommendation : optimizationRecommendations) {
        oss << "  - " << recommendation << "\n";
    }
    if (optimizationRecommendations.empty()) {
        oss << "  None\n";
    }
    
    return oss.str();
}

std::string PerformanceExperiment::experimentTypeToString(ExperimentType type) const {
    switch (type) {
        case ExperimentType::BASIC_PERFORMANCE: return "basic_performance";
        case ExperimentType::MEMORY_PERFORMANCE: return "memory_performance";
        case ExperimentType::THREAD_PERFORMANCE: return "thread_performance";
        case ExperimentType::SCALABILITY: return "scalability";
        case ExperimentType::REGRESSION_TESTING: return "regression_testing";
        case ExperimentType::OPTIMIZATION: return "optimization";
        case ExperimentType::BENCHMARKING: return "benchmarking";
        case ExperimentType::REAL_TIME_MONITORING: return "real_time_monitoring";
        case ExperimentType::THROUGHTPUT_ANALYSIS: return "throughput_analysis";
        case ExperimentType::LATENCY_ANALYSIS: return "latency_analysis";
        default: return "unknown";
    }
}

std::string PerformanceExperiment::escapeJsonString(const std::string& input) const {
    std::string output;
    output.reserve(input.size());
    
    for (char c : input) {
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    output += "\\u" + toHexString(static_cast<unsigned int>(c));
                } else {
                    output += c;
                }
                break;
        }
    }
    
    return output;
}

std::string PerformanceExperiment::toHexString(unsigned int value) const {
    std::stringstream stream;
    stream << std::hex << std::setw(4) << std::setfill('0') << value;
    return stream.str();
}

} // namespace nlm
// PerformanceTuner.cpp
#include "PerformanceTuner.cpp"
#include <nlohmann/json.hpp>
#include <algorithm>

using json = nlohmann::json;

namespace nlm {

PerformanceTuner::PerformanceTuner() = default;
PerformanceTuner::~PerformanceTuner() = default;

std::vector<PerformanceTuner::TuningResult> PerformanceTuner::analyzeAndSuggestTuning(
    const PerformanceStats& stats,
    const PerformanceProfiler::ProfileResult& profile,
    const TimelineProfiler& timeline,
    const MemoryProfiler& memory,
    const CPUProfiler& cpu,
    const ThreadProfiler& threads
) const {
    std::vector<TuningResult> tuningResults;
    
    // Analyze step time performance
    if (stats.maxStepTime > stats.averageStepTime * 2.0) {
        TuningResult tuning;
        tuning.parameterName = "step_time_optimization";
        tuning.currentValue = "high variance";
        tuning.suggestedValue = "reduce variance through better load balancing";
        tuning.expectedImprovement = 20.0; // 20% improvement
        tuning.confidence = 0.8;
        tuning.reasoning = "High variance in step times indicates uneven workload distribution";
        tuningResults.push_back(tuning);
    }
    
    // Analyze memory usage
    if (stats.peakMemoryUsage > stats.averageMemoryUsage * 3.0) {
        TuningResult tuning;
        tuning.parameterName = "memory_pooling";
        tuning.currentValue = "dynamic allocation";
        tuning.suggestedValue = "pre-allocated memory pools";
        tuning.expectedImprovement = 40.0; // 40% improvement
        tuning.confidence = 0.9;
        tuning.reasoning = "High memory usage variation suggests inefficient allocation patterns";
        tuningResults.push_back(tuning);
    }
    
    // Analyze CPU utilization
    if (stats.peakCPUUsage > 80.0) {
        TuningResult tuning;
        tuning.parameterName = "simd_optimization";
        tuning.currentValue = "scalar operations";
        tuning.suggestedValue = "SIMD vectorization";
        tuning.expectedImprovement = 35.0; // 35% improvement
        tuning.confidence = 0.7;
        tuning.reasoning = "High CPU utilization suggests CPU-bound performance";
        tuningResults.push_back(tuning);
    }
    
    // Analyze thread utilization
    auto threadAnalyses = threads.analyzeThreadPerformance();
    double maxUtilization = 0;
    for (const auto& analysis : threadAnalyses) {
        maxUtilization = std::max(maxUtilization, analysis.cpuUtilization);
    }
    
    if (maxUtilization > 90.0 && threadAnalyses.size() > 1) {
        TuningResult tuning;
        tuning.parameterName = "thread_pool_optimization";
        tuning.currentValue = "fixed thread pool";
        tuning.suggestedValue = "dynamic thread pool with work stealing";
        tuning.expectedImprovement = 25.0; // 25% improvement
        tuning.confidence = 0.6;
        tuning.reasoning = "High thread utilization with multiple threads suggests load balancing issues";
        tuningResults.push_back(tuning);
    }
    
    // Analyze profile data
    if (!profile.functionProfiles.empty()) {
        std::vector<const PerformanceProfiler::ProfileData*> topFunctions;
        for (const auto& func : profile.functionProfiles) {
            topFunctions.push_back(&func);
        }
        
        std::sort(topFunctions.begin(), topFunctions.end(),
                  [](const PerformanceProfiler::ProfileData* a, const PerformanceProfiler::ProfileData* b) {
                      return a->totalTime > b->totalTime;
                  });
        
        if (topFunctions.size() > 0 && topFunctions[0]->totalTime > profile.totalWallTime * 0.3) {
            TuningResult tuning;
            tuning.parameterName = "hot_function_optimization";
            tuning.currentValue = topFunctions[0]->functionName;
            tuning.suggestedValue = "optimize or parallelize " + topFunctions[0]->functionName;
            tuning.expectedImprovement = 50.0; // 50% improvement
            tuning.confidence = 0.9;
            tuning.reasoning = "Single function consuming significant portion of total time";
            tuningResults.push_back(tuning);
        }
    }
    
    return tuningResults;
}

bool PerformanceTuner::applyTuning(const TuningResult& tuning,
                                   std::function<void(const std::string&, const std::string&)> applyFunction) {
    try {
        applyFunction(tuning.parameterName, tuning.suggestedValue);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

PerformanceTuner::TuningConfig PerformanceTuner::generateTuningConfig(
    const PerformanceStats& stats,
    const std::vector<TuningResult>& tuningResults
) const {
    TuningConfig config;
    
    // Set default values based on performance characteristics
    config.neuronCount = 10000;
    config.enableMultithreading = true;
    config.enableSIMD = true;
    config.enableSparseConnectivity = true;
    config.enableEventDriven = true;
    config.threadCount = std::thread::hardware_concurrency();
    config.memoryPoolStrategy = "adaptive";
    config.parallelizationStrategy = "work_stealing";
    
    // Adjust based on performance analysis
    if (stats.maxStepTime > 0.1) { // 100ms threshold
        config.stepOptimization = "aggressive";
    }
    
    if (stats.peakMemoryUsage > 100 * 1024 * 1024) { // 100MB
        config.memoryOptimization = "conservative";
    }
    
    if (stats.peakCPUUsage > 80.0) {
        config.cpuOptimization = "aggressive";
    }
    
    return config;
}

PerformanceTuner::ValidationResult PerformanceTuner::validateTuning(
    const TuningConfig& config,
    const PerformanceStats& baselineStats
) const {
    ValidationResult result;
    
    // Validate configuration
    if (config.threadCount == 0) {
        result.valid = false;
        result.message = "Thread count cannot be zero";
        return result;
    }
    
    if (config.neuronCount > 1000000) { // 1M neuron limit
        result.valid = false;
        result.message = "Neuron count exceeds reasonable limits";
        return result;
    }
    
    // Estimate expected improvement
    double improvement = 0;
    if (config.cpuOptimization == "aggressive") {
        improvement += 25.0;
    }
    if (config.memoryOptimization == "conservative") {
        improvement += 20.0;
    }
    if (config.stepOptimization == "aggressive") {
        improvement += 15.0;
    }
    
    result.expectedImprovement = improvement;
    result.estimatedCost = improvement * 0.01; // Rough estimate: 1% of improvement is cost
    result.valid = true;
    result.message = "Configuration is valid and should improve performance";
    
    return result;
}

} // namespace nlm

// ScalingBenchmark.cpp
// Implementation of the scaling benchmark system

#include "experiments/ScalingBenchmark.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <filesystem>

namespace nlm {

// ============================================================================
// ScaleLevel implementation
// ============================================================================

std::vector<ScaleLevel> ScaleLevel::getStandardLevels() {
    return {
        {"10K Neurons", 10000, 100000, 0.1, "single_core", "Small scale, detailed analysis"},
        {"100K Neurons", 100000, 500000, 0.1, "multi_core", "Medium scale, performance testing"},
        {"1M Neurons", 1000000, 1000000, 0.05, "multi_core", "Large scale, memory testing"},
        {"10M Neurons", 10000000, 1000000, 0.01, "gpu", "Very large scale, GPU testing"},
        {"100M Neurons", 100000000, 1000000, 0.001, "gpu", "Maximum scale testing"},
    };
}

// ============================================================================
// BenchmarkRun implementation
// ============================================================================

std::string BenchmarkRun::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"name\": \"" << name << "\",\n";
    oss << "  \"scale\": {\n";
    oss << "    \"name\": \"" << scale.name << "\",\n";
    oss << "    \"neurons\": " << scale.neurons << ",\n";
    oss << "    \"expectedSynapses\": " << scale.expectedSynapses << ",\n";
    oss << "    \"connectionProbability\": " << scale.connectionProbability << ",\n";
    oss << "    \"hardwareTarget\": \"" << scale.hardwareTarget << "\"\n";
    oss << "  },\n";
    oss << "  \"randomSeed\": " << randomSeed << ",\n";
    oss << "  \"stepsRun\": " << stepsRun << ",\n";
    oss << "  \"wallClockTime\": " << wallClockTime << ",\n";
    oss << "  \"simulatedTime\": " << simulatedTime << ",\n";
    oss << "  \"cpuCores\": " << cpuCores << ",\n";
    oss << "  \"ramGB\": " << ramGB << ",\n";
    oss << "  \"vramGB\": " << vramGB << ",\n";
    oss << "  \"metrics\": {\n";
    oss << metrics.toJSON() << "\n";
    oss << "  },\n";
    oss << "  \"startTime\": " << startTime << ",\n";
    oss << "  \"endTime\": " << endTime << "\n";
    oss << "}\n";
    return oss.str();
}

// ============================================================================
// ScalingBenchmark implementation
// ============================================================================

ScalingBenchmark::ScalingBenchmark() {}

ScalingBenchmark::~ScalingBenchmark() {}

ScalingAnalysis ScalingBenchmark::run(
    const BenchmarkConfig& config,
    std::function<std::shared_ptr<Brain>(const ScaleLevel& scale)> createBrain,
    std::function<bool(Brain* brain, uint64_t steps)> runSimulation,
    std::function<BenchmarkMetrics(Brain* brain)> collectMetrics
) {
    ScalingAnalysis analysis;
    
    NLM_LOG_INFO("Starting scaling benchmark: " + config.name);
    NLM_LOG_INFO("Scale levels: " + std::to_string(config.scales.size()));
    NLM_LOG_INFO("Seeds: " + std::to_string(config.seeds.size()));
    NLM_LOG_INFO("Steps per scale: " + std::to_string(config.stepsPerScale));
    
    for (const auto& scale : config.scales) {
        NLM_LOG_INFO("\nRunning scale: " + scale.name + " (" + std::to_string(scale.neurons) + " neurons)");
        
        for (uint64_t seed : config.seeds) {
            NLM_LOG_INFO("  Seed: " + std::to_string(seed));
            
            BenchmarkRun run;
            run.name = scale.name + "_seed_" + std::to_string(seed);
            run.scale = scale;
            run.randomSeed = seed;
            run.startTime = time(nullptr);
            
            // Create brain
            auto brain = createBrain(scale);
            if (!brain) {
                NLM_LOG_ERROR("Failed to create brain for scale " + scale.name);
                continue;
            }
            
            // Initialize hardware info
            run.cpuModel = getHardwareInfo();
            run.cpuCores = std::thread::hardware_concurrency();
            
            // Warmup
            NLM_LOG_INFO("  Warmup: " + std::to_string(config.warmupSteps) + " steps");
            if (runSimulation(brain.get(), config.warmupSteps)) {
                NLM_LOG_INFO("  Warmup successful");
            } else {
                NLM_LOG_ERROR("Warmup failed");
                continue;
            }
            
            // Run benchmark
            NLM_LOG_INFO("  Running benchmark: " + std::to_string(config.stepsPerScale) + " steps");
            auto startTime = std::chrono::high_resolution_clock::now();
            
            if (runSimulation(brain.get(), config.stepsPerScale)) {
                auto endTime = std::chrono::high_resolution_clock::now();
                run.wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
                run.simulatedTime = config.stepsPerScale;
                run.stepsRun = config.stepsPerScale;
                
                // Collect metrics
                run.metrics = collectMetrics(brain.get());
                run.metrics.realTimeFactor = run.simulatedTime / run.wallClockTime;
                
                run.endTime = time(nullptr);
                analysis.runs.push_back(run);
                
                NLM_LOG_INFO("  Completed in " + std::to_string(run.wallClockTime) + "s");
                NLM_LOG_INFO("  Neurons/sec: " + std::to_string(run.metrics.neuronsPerSecond));
                NLM_LOG_INFO("  Memory usage: " + std::to_string(run.metrics.peakMemoryMB) + "MB");
            } else {
                NLM_LOG_ERROR("Benchmark run failed");
            }
        }
    }
    
    // Analyze results
    analysis = analyze(analysis.runs, config);
    
    return analysis;
}

BenchmarkRun ScalingBenchmark::runSingleScale(
    const ScaleLevel& scale,
    uint64_t seed,
    const BenchmarkConfig& config,
    std::function<std::shared_ptr<Brain>(const ScaleLevel& scale)> createBrain,
    std::function<bool(Brain* brain, uint64_t steps)> runSimulation,
    std::function<BenchmarkMetrics(Brain* brain)> collectMetrics
) {
    BenchmarkRun run;
    run.name = scale.name + "_seed_" + std::to_string(seed);
    run.scale = scale;
    run.randomSeed = seed;
    run.startTime = time(nullptr);
    
    NLM_LOG_INFO("Running single scale: " + scale.name);
    
    // Create brain
    auto brain = createBrain(scale);
    if (!brain) {
        NLM_LOG_ERROR("Failed to create brain");
        return run;
    }
    
    // Warmup
    if (runSimulation(brain.get(), config.warmupSteps)) {
        NLM_LOG_INFO("Warmup completed");
    } else {
        NLM_LOG_ERROR("Warmup failed");
        return run;
    }
    
    // Run benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (runSimulation(brain.get(), config.stepsPerScale)) {
        auto endTime = std::chrono::high_resolution_clock::now();
        run.wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        run.simulatedTime = config.stepsPerScale;
        run.stepsRun = config.stepsPerScale;
        
        // Collect metrics
        run.metrics = collectMetrics(brain.get());
        run.metrics.realTimeFactor = run.simulatedTime / run.wallClockTime;
        
        run.endTime = time(nullptr);
        
        NLM_LOG_INFO("Completed in " + std::to_string(run.wallClockTime) + "s");
        NLM_LOG_INFO("Neurons/sec: " + std::to_string(run.metrics.neuronsPerSecond));
    }
    
    return run;
}

ScalingAnalysis ScalingBenchmark::analyze(
    const std::vector<BenchmarkRun>& runs,
    const BenchmarkConfig& config
) {
    ScalingAnalysis analysis;
    analysis.runs = runs;
    
    if (runs.empty()) {
        NLM_LOG_WARNING("No runs to analyze");
        return analysis;
    }
    
    // Calculate scaling efficiency
    std::vector<double> scalingEfficiencies;
    for (size_t i = 1; i < runs.size(); ++i) {
        const auto& prev = runs[i - 1];
        const auto& curr = runs[i];
        
        // Calculate neurons per second ratio
        double prevPerf = prev.metrics.neuronsPerSecond;
        double currPerf = curr.metrics.neuronsPerSecond;
        
        if (prevPerf > 0) {
            double efficiency = currPerf / prevPerf;
            scalingEfficiencies.push_back(efficiency);
        }
    }
    
    if (!scalingEfficiencies.empty()) {
        double sum = std::accumulate(scalingEfficiencies.begin(), scalingEfficiencies.end(), 0.0);
        analysis.scalingEfficiency = sum / scalingEfficiencies.size();
    }
    
    // Identify bottleneck
    analysis.identifiedBottleneck = "unknown";
    analysis.bottleneckSeverity = 0.0;
    
    for (const auto& run : runs) {
        if (run.metrics.peakMemoryMB > 1024) {  // More than 1GB
            analysis.identifiedBottleneck = "memory";
            analysis.bottleneckSeverity = std::min(1.0, static_cast<double>(run.metrics.peakMemoryMB) / 2048.0);
            break;
        } else if (run.metrics.avgStepTimeMs > 10.0) {  // More than 10ms per step
            analysis.identifiedBottleneck = "compute";
            analysis.bottleneckSeverity = std::min(1.0, static_cast<double>(run.metrics.avgStepTimeMs) / 50.0);
            break;
        }
    }
    
    // Generate recommendations
    if (analysis.identifiedBottleneck == "memory") {
        analysis.recommendations.push_back("Consider implementing more efficient memory management");
        analysis.recommendations.push_back("Use memory pools for small allocations");
        analysis.recommendations.push_back("Implement compressed representations for sparse structures");
    } else if (analysis.identifiedBottleneck == "compute") {
        analysis.recommendations.push_back("Optimize computational kernels");
        analysis.recommendations.push_back("Enable SIMD optimizations");
        analysis.recommendations.push_back("Consider parallel processing");
    }
    
    analysis.recommendations.push_back("Continue scaling to larger sizes for more accurate measurements");
    
    // Calculate projections
    analysis.projectedPerformance10x = analysis.scalingEfficiency * 10.0;
    analysis.projectedPerformance100x = analysis.scalingEfficiency * 100.0;
    
    return analysis;
}

std::string ScalingBenchmark::generateReport(const ScalingAnalysis& analysis) {
    std::ostringstream oss;
    oss << "NLM Scaling Benchmark Report" << std::endl;
    oss << std::string(60, '=') << std::endl;
    oss << std::endl;
    
    oss << "Summary:" << std::endl;
    oss << "  Total runs: " << analysis.runs.size() << std::endl;
    oss << "  Scaling efficiency: " << std::fixed << std::setprecision(2) << analysis.scalingEfficiency << std::endl;
    oss << "  Identified bottleneck: " << analysis.identifiedBottleneck << std::endl;
    oss << "  Bottleneck severity: " << std::fixed << std::setprecision(2) << analysis.bottleneckSeverity << std::endl;
    oss << std::endl;
    
    oss << "Individual Runs:" << std::endl;
    for (const auto& run : analysis.runs) {
        oss << "  " << run.name << ": " << std::fixed << std::setprecision(2) 
            << run.metrics.neuronsPerSecond << " neurons/sec, "
            << run.metrics.peakMemoryMB << "MB peak memory" << std::endl;
    }
    
    oss << std::endl;
    oss << "Recommendations:" << std::endl;
    for (const auto& rec : analysis.recommendations) {
        oss << "  - " << rec << std::endl;
    }
    
    oss << std::endl;
    oss << "Projections:" << std::endl;
    oss << "  10x scale performance: " << std::fixed << std::setprecision(2) 
        << analysis.projectedPerformance10x << "x" << std::endl;
    oss << "  100x scale performance: " << std::fixed << std::setprecision(2) 
        << analysis.projectedPerformance100x << "x" << std::endl;
    
    return oss.str();
}

bool ScalingBenchmark::saveResults(const std::string& dir, const std::vector<BenchmarkRun>& runs) {
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    
    if (ec) {
        NLM_LOG_ERROR("Failed to create directory " + dir + ": " + ec.message());
        return false;
    }
    
    // Save runs as JSON
    std::string filename = getOutputPath(dir, "runs.json");
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << "[";
    for (size_t i = 0; i < runs.size(); ++i) {
        if (i > 0) file << ",";
        file << runs[i].toJSON();
    }
    file << "]";
    
    file.close();
    NLM_LOG_INFO("Saved runs to " + filename);
    
    return true;
}

std::vector<BenchmarkRun> ScalingBenchmark::loadResults(const std::string& dir) {
    std::vector<BenchmarkRun> runs;
    std::string filename = getOutputPath(dir, "runs.json");
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filename);
        return runs;
    }
    
    // This is a simplified JSON parser - in production, use a proper JSON library
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    file.close();
    // TODO: Parse JSON content and reconstruct BenchmarkRun objects
    
    return runs;
}

std::string ScalingBenchmark::getOutputPath(const std::string& dir, const std::string& filename) {
    return dir + "/" + filename;
}

std::string ScalingBenchmark::getHardwareInfo() {
    // Return CPU model name
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    std::string model;
    
    if (cpuinfo.is_open()) {
        while (std::getline(cpuinfo, line)) {
            if (line.find("model name") != std::string::npos) {
                size_t pos = line.find(":");
                if (pos != std::string::npos) {
                    model = line.substr(pos + 1);
                    break;
                }
            }
        }
        cpuinfo.close();
    }
    
    if (model.empty()) {
        model = "Unknown CPU";
    }
    
    return model;
}

// ============================================================================
// PerformanceMonitor implementation
// ============================================================================

PerformanceMonitor::PerformanceMonitor()
    : running_(false)
    , avgStepTimeMs_(0.0)
    , avgSpikesPerStep_(0.0)
    , avgActiveNeurons_(0.0)
    , totalSteps_(0)
    , totalSpikes_(0) {}

PerformanceMonitor::~PerformanceMonitor() {}

void PerformanceMonitor::start() {
    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
    NLM_LOG_INFO("Performance monitoring started");
}

void PerformanceMonitor::stop() {
    if (running_) {
        running_ = false;
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime_).count();
        NLM_LOG_INFO("Performance monitoring stopped after " + std::to_string(elapsed) + " seconds");
    }
}

void PerformanceMonitor::recordStep(double stepTimeMs, size_t spikes, size_t activeNeurons) {
    if (!running_) {
        return;
    }
    
    stepTimes_.push_back(stepTimeMs);
    spikeCounts_.push_back(spikes);
    activeNeuronCounts_.push_back(activeNeurons);
    
    totalSteps_++;
    totalSpikes_ += spikes;
    
    // Update running averages (using simple moving average)
    if (stepTimes_.size() > 1) {
        avgStepTimeMs_ = (avgStepTimeMs_ * (stepTimes_.size() - 2) + stepTimeMs) / (stepTimes_.size() - 1);
        avgSpikesPerStep_ = (avgSpikesPerStep_ * (stepTimes_.size() - 2) + static_cast<double>(spikes)) / (stepTimes_.size() - 1);
        avgActiveNeurons_ = (avgActiveNeurons_ * (stepTimes_.size() - 2) + static_cast<double>(activeNeurons)) / (stepTimes_.size() - 1);
    } else {
        avgStepTimeMs_ = stepTimeMs;
        avgSpikesPerStep_ = static_cast<double>(spikes);
        avgActiveNeurons_ = static_cast<double>(activeNeurons);
    }
}

BenchmarkMetrics PerformanceMonitor::getCurrentMetrics() const {
    BenchmarkMetrics metrics;
    
    if (stepTimes_.empty()) {
        return metrics;
    }
    
    // Calculate basic metrics
    double totalTimeMs = 0.0;
    for (double t : stepTimes_) totalTimeMs += t;
    
    metrics.avgStepTimeMs = avgStepTimeMs_;
    metrics.minStepTimeMs = *std::min_element(stepTimes_.begin(), stepTimes_.end());
    metrics.maxStepTimeMs = *std::max_element(stepTimes_.begin(), stepTimes_.end());
    
    double variance = 0.0;
    for (double t : stepTimes_) {
        double diff = t - avgStepTimeMs_;
        variance += diff * diff;
    }
    metrics.stepTimeStdDev = std::sqrt(variance / stepTimes_.size());
    
    metrics.avgSpikingRateHz = (totalSteps_ > 0) ? (totalSpikes_ / totalSteps_) / (metrics.avgStepTimeMs / 1000.0) : 0.0;
    metrics.activeNeurons = static_cast<size_t>(avgActiveNeurons_);
    metrics.firingNeurons = static_cast<size_t>(avgSpikesPerStep_);
    
    return metrics;
}

BenchmarkMetrics PerformanceMonitor::getSnapshot() const {
    return getCurrentMetrics();
}

bool PerformanceMonitor::isStable(size_t windowSize, double threshold) const {
    if (stepTimes_.size() < windowSize * 2) {
        return false;
    }
    
    // Compare first half and second half of the window
    size_t half = windowSize / 2;
    double firstSum = 0.0, secondSum = 0.0;
    
    for (size_t i = 0; i < half; ++i) {
        firstSum += stepTimes_[stepTimes_.size() - windowSize - i];
        secondSum += stepTimes_[stepTimes_.size() - i];
    }
    
    double firstAvg = firstSum / half;
    double secondAvg = secondSum / half;
    
    double diff = std::abs(firstAvg - secondAvg);
    double relativeDiff = (firstAvg > 0) ? (diff / firstAvg) : 0.0;
    
    return relativeDiff <= threshold;
}

void PerformanceMonitor::reset() {
    stepTimes_.clear();
    spikeCounts_.clear();
    activeNeuronCounts_.clear();
    
    avgStepTimeMs_ = 0.0;
    avgSpikesPerStep_ = 0.0;
    avgActiveNeurons_ = 0.0;
    
    totalSteps_ = 0;
    totalSpikes_ = 0;
    
    running_ = false;
}

// ============================================================================
// MemoryProfiler implementation
// ============================================================================

MemoryProfiler::MemoryProfiler()
    : peakMemoryMB_(0)
    , samples_() {}

MemoryProfiler::~MemoryProfiler() {}

void MemoryProfiler::record() {
    // Simplified implementation - in production, use platform-specific APIs
    size_t currentMB = 0;  // TODO: Implement actual memory measurement
    
    samples_.push_back(currentMB);
    
    if (currentMB > peakMemoryMB_) {
        peakMemoryMB_ = currentMB;
    }
}

// ============================================================================
// Inline implementations continue
// ============================================================================

} // namespace nlm

// ScalingBenchmark.cpp
#include "ScalingBenchmark.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Logger/Logger.hpp"
#include <thread>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <numeric>

namespace nlm {

std::vector<ScaleLevel> ScaleLevel::getStandardLevels() {
    return {
        {"Small", 100, 1000, 0.1f, "single_core", "Baseline test"},
        {"Medium", 1000, 10000, 0.15f, "multi_core", "Performance scaling"},
        {"Large", 10000, 100000, 0.2f, "multi_core", "Large scale simulation"},
        {"XLarge", 100000, 1000000, 0.25f, "multi_core", "Extreme scale"},
        {"Massive", 1000000, 10000000, 0.3f, "gpu", "Super computer scale"}
    };
}

// ScalingBenchmark implementations
ScalingBenchmark::ScalingBenchmark() = default;

ScalingBenchmark::~ScalingBenchmark() = default;

ScalingAnalysis ScalingBenchmark::run(
    const BenchmarkConfig& config,
    std::function<std::shared_ptr<class Brain>(const ScaleLevel& scale)> createBrain,
    std::function<bool(class Brain* brain, uint64_t steps)> runSimulation,
    std::function<BenchmarkMetrics(class Brain* brain)> collectMetrics
) {
    std::cout << "Running scaling benchmark: " << config.name << std::endl;
    
    std::vector<BenchmarkRun> runs;
    
    for (const auto& scale : config.scales) {
        for (uint64_t seed : config.seeds) {
            auto run = runSingleScale(scale, seed, config, createBrain, runSimulation, collectMetrics);
            runs.push_back(run);
            
            // Save intermediate results
            if (!config.outputDir.empty() && config.generateReport) {
                saveResults(config.outputDir, runs);
            }
            
            // Check stability if configured
            if (config.runUntilStable) {
                if (isRunStable(runs.back(), config)) {
                    std::cout << "Run stabilized at scale " << scale.name << ", seed " << seed << std::endl;
                    break;
                }
            }
        }
    }
    
    // Analyze results
    return analyze(runs, config);
}

BenchmarkRun ScalingBenchmark::runSingleScale(
    const ScaleLevel& scale,
    uint64_t seed,
    const BenchmarkConfig& config,
    std::function<std::shared_ptr<class Brain>(const ScaleLevel& scale)> createBrain,
    std::function<bool(class Brain* brain, uint64_t steps)> runSimulation,
    std::function<BenchmarkMetrics(class Brain* brain)> collectMetrics
) {
    std::cout << "Running scale: " << scale.name << " (seed " << seed << ")..." << std::endl;
    
    BenchmarkRun run;
    run.name = scale.name + "_seed" + std::to_string(seed);
    run.scale = scale;
    run.randomSeed = seed;
    run.stepsRun = config.stepsPerScale;
    
    // Set up random seed if needed
    auto random = std::mt19937(seed);
    
    // Create brain
    auto brain = createBrain(scale);
    
    // Warmup
    std::cout << "  Warming up..." << std::flush;
    runSimulation(brain.get(), config.warmupSteps);
    std::cout << " Done!" << std::endl;
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    std::cout << "  Running simulation..." << std::flush;
    bool simSuccess = runSimulation(brain.get(), config.stepsPerScale);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    run.wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    run.simulatedTime = static_cast<double>(config.stepsPerScale);
    run.startTime = std::time(nullptr);
    run.endTime = std::time(nullptr);
    
    // Collect metrics
    run.metrics = collectMetrics(brain.get());
    
    // Get hardware info
    run.cpuModel = getHardwareInfo();
    run.cpuCores = std::thread::hardware_concurrency();
    run.ramGB = getSystemMemoryGB();
    
    std::cout << " Done!" << std::endl;
    
    return run;
}

ScalingAnalysis ScalingBenchmark::analyze(
    const std::vector<BenchmarkRun>& runs,
    const BenchmarkConfig& config
) {
    std::cout << "Analyzing " << runs.size() << " benchmark runs..." << std::endl;
    
    ScalingAnalysis analysis;
    analysis.runs = runs;
    
    // Calculate scaling efficiency
    if (runs.size() >= 2) {
        double prevPerf = 0.0, currentPerf = 0.0;
        size_t i = 0;
        
        for (const auto& run : runs) {
            if (i == 0) {
                prevPerf = run.metrics.neuronsPerSecond;
            } else {
                currentPerf = run.metrics.neuronsPerSecond;
                if (prevPerf > 0.0 && currentPerf > 0.0) {
                    double scaleFactor = static_cast<double>(run.scale.neurons) / 
                                        static_cast<double>(runs[i-1].scale.neurons);
                    double efficiency = currentPerf / prevPerf / scaleFactor;
                    
                    analysis.scalingEfficiency = (analysis.scalingEfficiency + efficiency) / 2.0;
                }
                prevPerf = currentPerf;
            }
            i++;
        }
    }
    
    // Identify bottleneck
    analysis.identifiedBottleneck = identifyBottleneck(runs);
    analysis.bottleneckSeverity = calculateBottleneckSeverity(runs);
    
    // Generate recommendations
    analysis.recommendations = generateRecommendations(runs);
    
    // Project future performance
    analysis.projectedPerformance10x = projectScaling(runs, 10.0);
    analysis.projectedPerformance100x = projectScaling(runs, 100.0);
    
    return analysis;
}

std::string ScalingBenchmark::generateReport(const ScalingAnalysis& analysis) {
    std::stringstream report;
    
    report << "=== NLM Performance Benchmark Report ===\n\n";
    report << "Generated: " << std::put_time(std::localtime(&analysis.runs[0].startTime), "%Y-%m-%d %H:%M:%S") << "\n";
    report << "Total runs: " << analysis.runs.size() << "\n";
    report << "\n";
    
    // Summary table
    report << "Performance Summary:\n";
    report << "===================\n";
    report << "Scale\tNeurons\tSteps/sec\tMemory(MB)\tBottleneck\n";
    report << "-------------------------------------------------\n";
    
    for (const auto& run : analysis.runs) {
        report << run.scale.name << "\t"
               << run.scale.neurons << "\t"
               << std::fixed << std::setprecision(0) << run.metrics.neuronsPerSecond << "\t"
               << run.metrics.totalMemoryMB << "\t"
               << run.scale.hardwareTarget << "\n";
    }
    
    report << "\nScaling Analysis:\n";
    report << "==================\n";
    report << "Scaling Efficiency: " << (analysis.scalingEfficiency * 100) << "%\n";
    report << "Projected 10x performance: " << analysis.projectedPerformance10x << " neurons/sec\n";
    report << "Projected 100x performance: " << analysis.projectedPerformance100x << " neurons/sec\n";
    report << "Identified bottleneck: " << analysis.identifiedBottleneck << " (severity: " << analysis.bottleneckSeverity << ")\n";
    
    report << "\nRecommendations:\n";
    report << "==============\n";
    for (const auto& rec : analysis.recommendations) {
        report << "* " << rec << "\n";
    }
    
    return report.str();
}

bool ScalingBenchmark::saveResults(const std::string& dir, const std::vector<BenchmarkRun>& runs) {
    std::filesystem::create_directories(dir);
    
    // Save as JSON
    std::string jsonPath = dir + "/benchmark_results.json";
    std::ofstream jsonFile(jsonPath);
    if (!jsonFile.is_open()) {
        return false;
    }
    
    // Simple JSON serialization (in production, use a proper JSON library)
    jsonFile << "{\n  \"runs\": [\n";
    
    for (size_t i = 0; i < runs.size(); ++i) {
        jsonFile << "    {\n";
        jsonFile << "      \"name\": \"" << runs[i].name << "\",\n";
        jsonFile << "      \"neurons\": " << runs[i].scale.neurons << ",\n";
        jsonFile << "      \"synapses\": " << runs[i].scale.expectedSynapses << ",\n";
        jsonFile << "      \"wall_clock_time\": " << runs[i].wallClockTime << ",\n";
        jsonFile << "      \"neurons_per_second\": " << runs[i].metrics.neuronsPerSecond << ",\n";
        jsonFile << "      \"total_memory_mb\": " << runs[i].metrics.totalMemoryMB << "\n";
        if (i < runs.size() - 1) jsonFile << "    },\n";
        else jsonFile << "    }\n";
    }
    
    jsonFile << "  ]\n";
    jsonFile << "}\n";
    
    jsonFile.close();
    return true;
}

std::vector<BenchmarkRun> ScalingBenchmark::loadResults(const std::string& dir) {
    // Placeholder - implement proper file loading
    std::vector<BenchmarkRun> runs;
    return runs;
}

std::string ScalingBenchmark::getHardwareInfo() {
    std::stringstream ss;
    ss << "CPU: " << getCPUModel() << "\n";
    ss << "Cores: " << std::thread::hardware_concurrency() << "\n";
    ss << "Memory: " << getSystemMemoryGB() << " GB\n";
    return ss.str();
}

// PerformanceMonitor implementations
PerformanceMonitor::PerformanceMonitor()
    : running_(false)
    , totalSteps_(0)
    , totalSpikes_(0)
    , avgStepTimeMs_(0.0)
    , avgSpikesPerStep_(0.0)
    , avgActiveNeurons_(0.0)
{
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (running_) return;
    
    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) return;
    
    running_ = false;
}

void PerformanceMonitor::recordStep(double stepTimeMs, size_t spikes, size_t activeNeurons) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    stepTimes_.push_back(stepTimeMs);
    spikeCounts_.push_back(spikes);
    activeNeuronCounts_.push_back(activeNeurons);
    
    totalSteps_++;
    totalSpikes_ += spikes;
    
    // Update running averages
    if (!stepTimes_.empty()) {
        // Simple running average
        double sumStepTime = std::accumulate(stepTimes_.begin(), stepTimes_.end(), 0.0);
        double sumSpikes = std::accumulate(spikeCounts_.begin(), spikeCounts_.end(), 0.0);
        double sumActive = std::accumulate(activeNeuronCounts_.begin(), activeNeuronCounts_.end(), 0.0);
        
        avgStepTimeMs_ = sumStepTime / stepTimes_.size();
        avgSpikesPerStep_ = sumSpikes / stepTimes_.size();
        avgActiveNeurons_ = sumActive / stepTimes_.size();
    }
}

BenchmarkMetrics PerformanceMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    BenchmarkMetrics metrics;
    metrics.avgStepTimeMs = avgStepTimeMs_;
    metrics.avgSpikesPerStep_ = avgSpikesPerStep_;
    metrics.avgActiveNeurons_ = avgActiveNeurons_;
    
    if (!stepTimes_.empty()) {
        metrics.minStepTimeMs = *std::min_element(stepTimes_.begin(), stepTimes_.end());
        metrics.maxStepTimeMs = *std::max_element(stepTimes_.begin(), stepTimes_.end());
        
        // Calculate standard deviation
        double mean = avgStepTimeMs_;
        double variance = 0.0;
        for (double time : stepTimes_) {
            variance += (time - mean) * (time - mean);
        }
        metrics.stepTimeStdDev = std::sqrt(variance / stepTimes_.size());
    }
    
    return metrics;
}

BenchmarkMetrics PerformanceMonitor::getSnapshot() const {
    // Same as current metrics for now
    return getCurrentMetrics();
}

bool PerformanceMonitor::isStable(size_t windowSize, double threshold) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (stepTimes_.size() < windowSize) return false;
    
    // Check if step times are stable within the last windowSize steps
    size_t startIdx = stepTimes_.size() - windowSize;
    double mean = 0.0, stddev = 0.0;
    
    for (size_t i = startIdx; i < stepTimes_.size(); ++i) {
        mean += stepTimes_[i];
    }
    mean /= windowSize;
    
    for (size_t i = startIdx; i < stepTimes_.size(); ++i) {
        stddev += (stepTimes_[i] - mean) * (stepTimes_[i] - mean);
    }
    stddev = std::sqrt(stddev / windowSize);
    
    return (stddev / mean) < threshold;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    stepTimes_.clear();
    spikeCounts_.clear();
    activeNeuronCounts_.clear();
    
    totalSteps_ = 0;
    totalSpikes_ = 0;
    avgStepTimeMs_ = 0.0;
    avgSpikesPerStep_ = 0.0;
    avgActiveNeurons_ = 0.0;
}

// MemoryProfiler implementations
MemoryProfiler::MemoryProfiler()
    : peakMemoryMB_(0)
{
}

MemoryProfiler::~MemoryProfiler() = default;

void MemoryProfiler::record() {
    size_t currentMB = getCurrentMemoryMB();
    samples_.push_back(currentMB);
    
    if (currentMB > peakMemoryMB_) {
        peakMemoryMB_ = currentMB;
    }
}

MemoryProfiler::MemoryBreakdown MemoryProfiler::getBreakdown() const {
    MemoryBreakdown breakdown;
    
    // Estimate memory usage by component
    // This is a rough estimate based on known sizes
    // In production, you'd use more precise measurements
    
    breakdown.neuronsMB = peakMemoryMB_ * 0.6f;  // ~60% for neurons
    breakdown.synapsesMB = peakMemoryMB_ * 0.25f; // ~25% for synapses
    breakdown.eventsMB = peakMemoryMB_ * 0.1f;   // ~10% for events
    breakdown.otherMB = peakMemoryMB_ * 0.05f;   // ~5% for other
    
    return breakdown;
}

// Helper functions
std::string ScalingBenchmark::identifyBottleneck(const std::vector<BenchmarkRun>& runs) {
    if (runs.empty()) return "unknown";
    
    // Simple bottleneck identification based on scaling patterns
    double memoryScaling = 0.0, computeScaling = 0.0;
    size_t comparisons = 0;
    
    for (size_t i = 1; i < runs.size(); ++i) {
        const auto& prev = runs[i-1];
        const auto& current = runs[i];
        
        double memRatio = static_cast<double>(current.metrics.totalMemoryMB) / prev.metrics.totalMemoryMB;
        double computeRatio = static_cast<double>(current.metrics.neuronsPerSecond) / prev.metrics.neuronsPerSecond;
        
        double scaleFactor = static_cast<double>(current.scale.neurons) / prev.scale.neurons;
        
        memoryScaling += memRatio / scaleFactor;
        computeScaling += computeRatio / scaleFactor;
        comparisons++;
    }
    
    if (comparisons > 0) {
        memoryScaling /= comparisons;
        computeScaling /= comparisons;
        
        if (memoryScaling < 0.8) return "memory"; // Memory scales sub-linearly
        if (computeScaling < 0.8) return "compute"; // Compute scales sub-linearly
        return "communication"; // Assume communication is the bottleneck
    }
    
    return "unknown";
}

double ScalingBenchmark::calculateBottleneckSeverity(const std::vector<BenchmarkRun>& runs) {
    if (runs.empty()) return 0.0;
    
    // Calculate severity based on performance degradation
    double minEfficiency = 1.0;
    
    for (size_t i = 1; i < runs.size(); ++i) {
        const auto& prev = runs[i-1];
        const auto& current = runs[i];
        
        if (prev.metrics.neuronsPerSecond > 0.0) {
            double efficiency = current.metrics.neuronsPerSecond / prev.metrics.neuronsPerSecond;
            minEfficiency = std::min(minEfficiency, efficiency);
        }
    }
    
    return std::max(0.0, 1.0 - minEfficiency);
}

std::vector<std::string> ScalingBenchmark::generateRecommendations(const std::vector<BenchmarkRun>& runs) {
    std::vector<std::string> recommendations;
    
    if (runs.empty()) return recommendations;
    
    // Analyze scaling patterns and generate recommendations
    double avgMemoryPerNeuron = 0.0;
    double avgComputePerNeuron = 0.0;
    size_t neuronSum = 0;
    
    for (const auto& run : runs) {
        if (run.scale.neurons > 0) {
            avgMemoryPerNeuron += static_cast<double>(run.metrics.totalMemoryMB) / run.scale.neurons;
            avgComputePerNeuron += run.metrics.neuronsPerSecond / run.scale.neurons;
            neuronSum++;
        }
    }
    
    if (neuronSum > 0) {
        avgMemoryPerNeuron /= neuronSum;
        avgComputePerNeuron /= neuronSum;
    }
    
    // Memory optimization recommendations
    if (avgMemoryPerNeuron > 10.0) {  // If > 10MB per neuron
        recommendations.push_back("Consider using memory pools or more efficient data structures to reduce memory overhead");
    }
    
    // Compute optimization recommendations
    if (avgComputePerNeuron < 100.0) {  // If < 100 neurons/sec per neuron
        recommendations.push_back("Optimize neural computation - consider SIMD vectorization or better cache locality");
    }
    
    // Scaling recommendations
    if (analysis.scalingEfficiency < 0.8) {
        recommendations.push_back("Hardware scaling may be suboptimal - investigate communication overhead");
    }
    
    return recommendations;
}

double ScalingBenchmark::projectScaling(const std::vector<BenchmarkRun>& runs, double scaleFactor) {
    if (runs.size() < 2) return 0.0;
    
    // Use the last two runs to extrapolate
    const auto& last = runs.back();
    const auto& secondLast = runs[runs.size() - 2];
    
    if (last.metrics.neuronsPerSecond <= 0.0 || secondLast.metrics.neuronsPerSecond <= 0.0) {
        return 0.0;
    }
    
    double scalingEfficiency = last.metrics.neuronsPerSecond / secondLast.metrics.neuronsPerSecond;
    double neuronScaleFactor = static_cast<double>(last.scale.neurons) / secondLast.scale.neurons;
    
    // Extrapolate to scaleFactor
    double projectedNeuronsPerSec = secondLast.metrics.neuronsPerSecond * 
                                   scalingEfficiency * scaleFactor * neuronScaleFactor;
    
    return projectedNeuronsPerSec;
}

// Helper function implementations
std::string ScalingBenchmark::getCPUModel() {
    // Platform-specific implementation
#ifdef __x86_64__
    return "x86-64"; // Would need to parse /proc/cpuinfo or use cpuid
#else
    return "Unknown";
#endif
}

size_t ScalingBenchmark::getSystemMemoryGB() {
    // Platform-specific implementation
#ifdef __linux__
    // Parse /proc/meminfo
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    size_t memTotal = 0;
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            std::istringstream iss(line);
            std::string key;
            size_t value;
            iss >> key >> value;
            memTotal = value / 1024 / 1024; // Convert to GB
            break;
        }
    }
    return memTotal;
#else
    return 0; // Unknown platform
#endif
}

bool ScalingBenchmark::isRunStable(const BenchmarkRun& run, const BenchmarkConfig& config) {
    // Simple stability check based on step time variability
    // This is a placeholder - real implementation would monitor more metrics
    return true; // Assume stable for now
}

}} // namespace nlm

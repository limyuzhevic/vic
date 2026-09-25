#include "Benchmark.hpp"
#include "../src/brain/NeuronPool.hpp"
#include "../src/brain/SynapsePool.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/performance/ParallelProcessing.hpp"
#include "../src/performance/SIMDVectorization.hpp"
#include "../src/performance/Monitor.hpp"
#include "../src/brain/Brain.hpp"
#include <numeric>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <map>

namespace nlm {

Benchmark::Benchmark()
    : enabled_(true)
    , brain_(nullptr)
    , neuronPool_(nullptr)
    , synapsePool_(nullptr)
    , eventQueue_(nullptr)
    , parallelProc_(nullptr)
    , simd_(nullptr)
    , monitor_(nullptr)
    , currentPhase_(BenchmarkPhase::Initialization)
    , phaseStartTime_(0.0)
    , totalMemoryAccessed_(0)
    , cacheHits_(0)
    , totalCacheMisses_(0)
    , eventsProcessed_(0)
    , totalEventLatency_(0.0)
    , totalSynchronizationOverhead_(0.0) {
}

Benchmark::~Benchmark() {
    // Cleanup
}

Benchmark& Benchmark::getInstance() {
    static Benchmark instance;
    return instance;
}

void Benchmark::configure(const BenchmarkConfig& config) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    currentConfig_ = config;
}

void Benchmark::enable(bool enable) {
    enabled_ = enable;
}

void Benchmark::integrateWithBrain(nlm::Brain* brain) {
    brain_ = brain;
}

void Benchmark::integrateWithNeuronPool(nlm::NeuronPool* neuronPool) {
    neuronPool_ = neuronPool;
}

void Benchmark::integrateWithSynapsePool(nlm::SynapsePool* synapsePool) {
    synapsePool_ = synapsePool;
}

void Benchmark::integrateWithEventQueue(nlm::EventQueue* eventQueue) {
    eventQueue_ = eventQueue;
}

void Benchmark::integrateWithParallelProcessing(nlm::ParallelProcessing* parallelProc) {
    parallelProc_ = parallelProc;
}

void Benchmark::integrateWithSIMD(nlm::SIMDVectorization* simd) {
    simd_ = simd;
}

void Benchmark::integrateWithPerformanceMonitor(nlm::PerformanceMonitor* monitor) {
    monitor_ = monitor;
}

BenchmarkResult Benchmark::run(const std::string& name) {
    if (!enabled_) {
        BenchmarkResult result;
        result.benchmarkName = name;
        return result;
    }
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = std::find_if(benchmarkHistory_.begin(), benchmarkHistory_.end(),
        [&name](const BenchmarkResult& result) { return result.benchmarkName == name; });n
    if (it != benchmarkHistory_.end()) {
        return *it;
    }
    
    BenchmarkResult result;
    result.benchmarkName = name;
    benchmarkHistory_.push_back(result);
    
    return result;
}

void Benchmark::runAll() {
    if (!enabled_) return;
    
    // Run all benchmark categories
    runMicroBenchmarks();
    runNeuronBenchmarks();
    runSynapseBenchmarks();
    runMemoryBenchmarks();
    runParallelBenchmarks();
    runEventDrivenBenchmarks();
    runIntegrationBenchmarks();
}

void Benchmark::stopCurrentBenchmark() {
    if (currentBenchmark_.active) {
        finalizeBenchmark();
    }
}

BenchmarkResult Benchmark::runMicroBenchmarks() {
    return runMicrobenchmarksImpl();
}

BenchmarkResult Benchmark::runNeuronBenchmarks() {
    return runNeuronBenchmarksImpl();
}

BenchmarkResult Benchmark::runSynapseBenchmarks() {
    return runSynapseBenchmarksImpl();
}

BenchmarkResult Benchmark::runMemoryBenchmarks() {
    return runMemoryBenchmarksImpl();
}

BenchmarkResult Benchmark::runParallelBenchmarks() {
    return runParallelBenchmarksImpl();
}

BenchmarkResult Benchmark::runEventDrivenBenchmarks() {
    return runEventDrivenBenchmarksImpl();
}

BenchmarkResult Benchmark::runIntegrationBenchmarks() {
    return runIntegrationBenchmarksImpl();
}

std::vector<ComparativeResult> Benchmark::runComparativeBenchmarks(const std::vector<std::string>& configurations) {
    return runComparativeBenchmarksImpl(configurations);
}

std::vector<ScalingResult> Benchmark::runScalingBenchmarks(const std::vector<size_t>& neuronCounts) {
    return runScalingBenchmarksImpl(neuronCounts);
}

BenchmarkResult Benchmark::runConfigurationBenchmark(const std::string& configName) {
    return runConfigurationBenchmarkImpl(configName);
}

void Benchmark::runConfigurationSeries(const std::vector<std::string>& configNames) {
    for (const auto& configName : configNames) {
        runConfigurationBenchmark(configName);
    }
}

void Benchmark::startPhase(BenchmarkPhase phase) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    currentPhase_ = phase;
    phaseStartTime_ = getWallClockTime();
    
    FlameGraphEvent event;
    event.timestamp = phaseStartTime_;
    event.phase = phase;
    event.operation = "phase_start";
    event.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    event.duration = 0.0;
    event.stackHash = 0;
    
    flameGraphEvents_.push_back(event);
}

void Benchmark::endPhase(BenchmarkPhase phase) {
    if (currentPhase_ != phase) return;
    
    double endTime = getWallClockTime();
    double duration = endTime - phaseStartTime_;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    FlameGraphEvent event;
    event.timestamp = endTime;
    event.phase = phase;
    event.operation = "phase_end";
    event.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    event.duration = duration;
    event.stackHash = 0;
    
    flameGraphEvents_.push_back(event);
}

void Benchmark::recordEvent(const std::string& operation, double duration, BenchmarkPhase phase) {
    if (!enabled_) return;
    
    FlameGraphEvent event;
    event.timestamp = getWallClockTime();
    event.phase = phase;
    event.operation = operation;
    event.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    event.duration = duration;
    event.stackHash = std::hash<std::string>{}(operation);
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    flameGraphEvents_.push_back(event);
}

void Benchmark::recordMemoryAccess(size_t bytes, bool cacheHit) {
    if (!enabled_) return;
    
    totalMemoryAccessed_ += bytes;
    if (cacheHit) {
        cacheHits_++;
    } else {
        totalCacheMisses_++;
    }
}

void Benchmark::recordEventLatency(double latency, const std::string& eventType) {
    if (!enabled_) return;
    
    totalEventLatency_ += latency;
    eventsProcessed_++;
}

void Benchmark::recordSynchronizationOverhead(double overhead) {
    if (!enabled_) return;
    
    totalSynchronizationOverhead_ += overhead;
}

void Benchmark::generateFlameGraph(const std::string& filename) const {
    writeFlameGraph(filename);
}

void Benchmark::generateDetailedReport(const std::string& filename) const {
    writeDetailedReport(filename);
}

void Benchmark::generatePerformanceSummary(const std::string& filename) const {
    writePerformanceSummary(filename);
}

std::string Benchmark::analyzePerformance(const BenchmarkResult& result) const {
    std::stringstream ss;
    ss << "Performance Analysis for " << result.benchmarkName << ":" << std::endl;
    ss << "  Duration: " << std::fixed << std::setprecision(3) << result.duration << "s" << std::endl;
    ss << "  Operations/sec: " << std::fixed << std::setprecision(0) << result.opsPerSecond" << std::endl;
    ss << "  SIMD Level: " << result.simdLevel << std::endl;
    ss << "  Memory Usage: " << (result.memoryUsed / 1024 / 1024) << " MB" << std::endl;
    ss << "  Memory Bandwidth: " << std::fixed << std::setprecision(2) << result.memoryBandwidth << " GB/s" << std::endl;
    ss << "  Efficiency: " << std::fixed << std::setprecision(1) << result.efficiency * 100.0 << "%" << std::endl;
    ss << "  Instructions/cycle: " << std::fixed << std::setprecision(2) << result.instructionsPerCycle" << std::endl;
    
    return ss.str();
}

std::string Benchmark::analyzeScaling(const std::vector<ScalingResult>& results) const {
    std::stringstream ss;
    ss << "Scaling Analysis:" << std::endl;
    
    if (results.empty()) {
        ss << "  No scaling results available." << std::endl;
        return ss.str();
    }
    
    ss << "  Neuron Count\tNeurons/sec\tMemory/Neu\tParallel Speedup\tReal-time Factor" << std::endl;
    for (const auto& result : results) {
        ss << "  " << result.neuronCount << "\t\t"
           << result.neuronsPerSecond << "\t\t"
           << (result.memoryUsage / result.neuronCount) << "\t\t"
           << result.parallelSpeedup << "\t\t"
           << result.realtimeFactor << std::endl;
    }
    
    return ss.str();
}

std::string Benchmark::analyzeComparative(const std::vector<ComparativeResult>& results) const {
    std::stringstream ss;
    ss << "Comparative Analysis:" << std::endl;
    
    if (results.empty()) {
        ss << "  No comparative results available." << std::endl;
        return ss.str();
    }
    
    ss << "  Configuration\tPerformance\tMemory Usage\tQuality Score" << std::endl;
    for (const auto& result : results) {
        ss << "  " << result.configuration << "\t\t"
           << result.performance << "\t\t"
           << (result.memoryUsage / 1024 / 1024) << " MB\t\t"
           << result.qualityScore << std::endl;
    }
    
    return ss.str();
}

std::vector<std::string> Benchmark::getOptimizationRecommendations() const {
    std::vector<std::string> recommendations;
    
    // Check for memory issues
    double cacheEfficiency = cacheHits_.load() / static_cast<double>(cacheHits_.load() + totalCacheMisses_.load());
    if (cacheEfficiency < 0.8) {
        recommendations.push_back("Memory cache efficiency below 80% - consider reducing working set size or improving data layout");
    }
    
    // Check for memory bandwidth issues
    if (totalMemoryAccessed_.load() > 100 * 1024 * 1024 * 1024) { // 100 GB
        recommendations.push_back("High memory bandwidth usage - consider data compression or reducing memory traffic");
    }
    
    // Check for event processing issues
    if (eventsProcessed_.load() > 0) {
        double avgEventLatency = totalEventLatency_.load() / eventsProcessed_.load();
        if (avgEventLatency > 0.001) { // 1 ms
            recommendations.push_back("High event processing latency - consider optimizing event queue or increasing batch size");
        }
    }
    
    // Check for synchronization overhead
    if (totalSynchronizationOverhead_.load() > 0.1) { // 100 ms
        recommendations.push_back("High synchronization overhead - consider reducing lock contention or using more efficient parallelization");
    }
    
    return recommendations;
}

bool Benchmark::applyAutoOptimizations() {
    // Apply auto optimizations based on performance analysis
    // This is a placeholder for actual optimization logic
    return false;
}

void Benchmark::exportResults(const std::string& directory) const {
    // Create directory if it doesn't exist
    std::string mkdirCommand = "mkdir -p " + directory;
    system(mkdirCommand.c_str());
    
    // Export detailed report
    std::string reportFile = directory + "/detailed_report.txt";
    generateDetailedReport(reportFile);
    
    // Export performance summary
    std::string summaryFile = directory + "/performance_summary.txt";
    generatePerformanceSummary(summaryFile);
    
    // Export flame graph
    std::string flameGraphFile = directory + "/flamegraph.svg";
    generateFlameGraph(flameGraphFile);
}

void Benchmark::clearHistory() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    benchmarkHistory_.clear();
    scalingResults_.clear();
    comparativeResults_.clear();
    flameGraphEvents_.clear();
    
    totalMemoryAccessed_ = 0;
    cacheHits_ = 0;
    totalCacheMisses_ = 0;
    eventsProcessed_ = 0;
    totalEventLatency_ = 0.0;
    totalSynchronizationOverhead_ = 0.0;
}

// Implementation functions

void Benchmark::initializeBenchmark(const std::string& name) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    currentBenchmark_.name = name;
    currentBenchmark_.active = true;
    currentBenchmark_.startTime = std::chrono::high_resolution_clock::now();
    currentBenchmark_.cpuStart = std::chrono::high_resolution_clock::now();
}

BenchmarkResult Benchmark::finalizeBenchmark() {
    BenchmarkResult result;
    result.benchmarkName = currentBenchmark_.name;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto cpuEndTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - currentBenchmark_.startTime);
    auto cpuDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(cpuEndTime - currentBenchmark_.cpuStart);
    
    result.duration = duration.count() * 1e-9; // Convert to seconds
    result.cpuTime = cpuDuration.count() * 1e-9;
    
    // Calculate derived metrics
    result.opsPerSecond = 0.0; // Will be calculated based on specific benchmark
    result.simdLevel = simd_.get() ? simd_->getSIMDType() : "Scalar";
    result.numThreads = 1; // Will be updated by integration
    result.efficiency = 1.0; // Will be updated by integration
    result.instructionsPerCycle = 1.0; // Default value
    
    currentBenchmark_.active = false;
    benchmarkHistory_.push_back(result);
    
    return result;
}

double Benchmark::calculatePerformanceScore(const BenchmarkResult& result) const {
    return result.opsPerSecond * result.efficiency;
}

double Benchmark::calculateScalingEfficiency(const ScalingResult& result) const {
    return result.neuronsPerSecond / (result.memoryUsage / result.neuronCount); // neurons/sec per byte of memory
}

double Benchmark::calculateComparativeScore(const ComparativeResult& result) const {
    return result.performance * result.qualityScore;
}

BenchmarkResult Benchmark::runMicrobenchmarksImpl() {
    // Placeholder for microbenchmark implementations
    BenchmarkResult result;
    result.benchmarkName = "Microbenchmarks";
    result.duration = 1.0; // Placeholder
    result.opsPerSecond = 1000000; // 1M ops/sec placeholder
    result.simdLevel = simd_.get() ? simd_->getSIMDType() : "Scalar";
    result.numThreads = 1;
    result.efficiency = 1.0;
    result.instructionsPerCycle = 2.0;
    result.memoryUsage = 1024; // 1 KB placeholder
    result.memoryBandwidth = 10.0; // 10 GB/s placeholder
    
    return result;
}

BenchmarkResult Benchmark::runNeuronBenchmarksImpl() {
    // Placeholder for neuron benchmark implementations
    BenchmarkResult result;
    result.benchmarkName = "NeuronBenchmarks";
    result.duration = 2.0; // Placeholder
    result.opsPerSecond = 500000; // 500K neurons/sec placeholder
    result.simdLevel = simd_.get() ? simd_->getSIMDType() : "Scalar";
    result.numThreads = 4; // Multi-threaded
    result.efficiency = 0.8;
    result.instructionsPerCycle = 3.0;
    result.memoryUsage = 10 * 1024 * 1024; // 10 MB placeholder
    result.memoryBandwidth = 20.0; // 20 GB/s placeholder
    
    return result;
}

BenchmarkResult Benchmark::runSynapseBenchmarksImpl() {
    // Placeholder for synapse benchmark implementations
    BenchmarkResult result;
    result.benchmarkName = "SynapseBenchmarks";
    result.duration = 3.0; // Placeholder
    result.opsPerSecond = 2000000; // 2M synapses/sec placeholder
    result.simdLevel = simd_.get() ? simd_->getSIMDType() : "Scalar";
    result.numThreads = 4;
    result.efficiency = 0.85;
    result.instructionsPerCycle = 4.0;
    result.memoryUsage = 50 * 1024 * 1024; // 50 MB placeholder
    result.memoryBandwidth = 40.0; // 40 GB/s placeholder
    
    return result;
}

BenchmarkResult Benchmark::runMemoryBenchmarksImpl() {
    // Placeholder for memory benchmark implementations
    BenchmarkResult result;
    result.benchmarkName = "MemoryBenchmarks";
    result.duration = 1.5; // Placeholder
    result.opsPerSecond = 0.0; // Memory bandwidth is the metric
    result.simdLevel = simd_.get() ? simd_->getSIMDType() : "Scalar";
    result.numThreads = 8;
    result.efficiency = 0.9;
    result.instructionsPerCycle = 2.0;
    result.memoryUsage = 0; // Not applicable
    result.memoryBandwidth = 60.0; // 60 GB/s placeholder (high cache hit rate)
    
    return result;
}

BenchmarkResult Benchmark::runParallelBenchmarksImpl() {
    // Placeholder for parallel benchmark implementations
    BenchmarkResult result;
    result.benchmarkName = "ParallelBenchmarks";
    result.duration = 2.5; // Placeholder
    result.opsPerSecond = 3000000; // 3M ops/sec placeholder
    result.simdLevel = simd_.get() ? simd_->getSIMDType() : "Scalar";
    result.numThreads = 16;
    result.efficiency = 0.75;
    result.instructionsPerCycle = 3.5;
    result.memoryUsage = 20 * 1024 * 1024; // 20 MB placeholder
    result.memoryBandwidth = 30.0; // 30 GB/s placeholder
    
    return result;
}

BenchmarkResult Benchmark::runEventDrivenBenchmarksImpl() {
    // Placeholder for event-driven benchmark implementations
    BenchmarkResult result;
    result.benchmarkName = "EventDrivenBenchmarks";
    result.duration = 2.0; // Placeholder
    result.opsPerSecond = 800000; // 800K ops/sec placeholder
    result.simdLevel = simd_.get() ? simd_->getSIMDType() : "Scalar";
    result.numThreads = 4;
    result.efficiency = 0.7;
    result.instructionsPerCycle = 2.5;
    result.memoryUsage = 5 * 1024 * 1024; // 5 MB placeholder
    result.memoryBandwidth = 15.0; // 15 GB/s placeholder
    
    return result;
}

BenchmarkResult Benchmark::runIntegrationBenchmarksImpl() {
    // Placeholder for integration benchmark implementations
    BenchmarkResult result;
    result.benchmarkName = "IntegrationBenchmarks";
    result.duration = 5.0; // Placeholder (longer for integration)
    result.opsPerSecond = 1500000; // 1.5M ops/sec placeholder
    result.simdLevel = simd_.get() ? simd_->getSIMDType() : "Scalar";
    result.numThreads = 8;
    result.efficiency = 0.8;
    result.instructionsPerCycle = 3.0;
    result.memoryUsage = 100 * 1024 * 1024; // 100 MB placeholder
    result.memoryBandwidth = 45.0; // 45 GB/s placeholder
    
    return result;
}

std::vector<ComparativeResult> Benchmark::runComparativeBenchmarksImpl(const std::vector<std::string>& configurations) {
    std::vector<ComparativeResult> results;
    
    for (const auto& config : configurations) {
        ComparativeResult result;
        result.configuration = config;
        result.performance = 0.0; // Placeholder
        result.memoryUsage = 0; // Placeholder
        result.energyConsumption = 0.0; // Placeholder
        result.qualityScore = 0.0; // Placeholder
        result.hardware = "Intel i7-9700K"; // Default
        result.instructionsPerCycle = 1.0; // Placeholder
        result.branchPredictionAccuracy = 0.95; // Placeholder
        
        results.push_back(result);
    }
    
    return results;
}

std::vector<ScalingResult> Benchmark::runScalingBenchmarksImpl(const std::vector<size_t>& neuronCounts) {
    std::vector<ScalingResult> results;
    
    for (const auto& neuronCount : neuronCounts) {
        ScalingResult result;
        result.neuronCount = neuronCount;
        result.synapseCount = neuronCount * 10; // Approximate 10 synapses per neuron
        result.neuronsPerSecond = neuronCount / 0.1; // 10 neurons per second placeholder
        result.spikesPerSecond = neuronCount; // 1 spike per neuron per second placeholder
        result.memoryUsage = neuronCount * 1024; // 1 KB per neuron placeholder
        result.memoryPerNeuron = 1024; // 1 KB per neuron
        result.memoryBandwidth = 10.0 * std::log10(neuronCount / 1000 + 1); // Scaling with neuron count
        result.efficiency = 0.9; // Placeholder
        result.parallelSpeedup = std::min(16.0, std::log2(neuronCount / 1000 + 1)); // Better scaling for larger networks
        result.realtimeFactor = 0.01 * std::log10(neuronCount / 1000 + 1); // Worse for larger networks
        
        // Add phase results
        for (size_t i = 0; i < 5; ++i) {
            PerformanceCharacteristics phase;
            phase.neuronsPerSecond = result.neuronsPerSecond * (0.5 + 0.5 * std::sin(i * M_PI / 5));
            phase.spikesPerSecond = result.spikesPerSecond * (0.5 + 0.5 * std::cos(i * M_PI / 5));
            phase.memoryBandwidth = result.memoryBandwidth * (0.8 + 0.2 * std::sin(i * M_PI / 2.5));
            phase.cacheHitRate = 0.7 + 0.3 * std::sin(i * M_PI / 4);
            phase.simdUtilization = 0.8 + 0.2 * std::cos(i * M_PI / 5);
            phase.parallelEfficiency = result.parallelSpeedup / 16.0;
            phase.eventProcessingLatency = 0.001 * (1.0 + std::sin(i * M_PI / 3));
            phase.synchronizationOverhead = 0.0001 * (1.0 + std::cos(i * M_PI / 3));
            phase.customMetrics = {};
            
            result.phases.push_back(phase);
        }
        
        results.push_back(result);
    }
    
    return results;
}

BenchmarkResult Benchmark::runConfigurationBenchmarkImpl(const std::string& configName) {
    BenchmarkResult result;
    result.benchmarkName = "ConfigurationBenchmark_" + configName;
    result.duration = 2.0; // Placeholder
    result.opsPerSecond = 1000000; // 1M ops/sec placeholder
    result.simdLevel = simd_.get() ? simd_->getSIMDType() : "Scalar";
    result.numThreads = 4;
    result.efficiency = 0.8;
    result.instructionsPerCycle = 2.0;
    result.memoryUsage = 50 * 1024 * 1024; // 50 MB placeholder
    result.memoryBandwidth = 20.0; // 20 GB/s placeholder
    
    return result;
}

void Benchmark::writeFlameGraph(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    // Write simple SVG representation
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100%\" height=\"100%\">" << std::endl;
    file << "  <defs>" << std::endl;
    file << "    <style type=\"text/css\">" << std::endl;
    file << "      .stack0 { fill: #e6f2ff; }" << std::endl;
    file << "      .stack1 { fill: #cce5ff; }" << std::endl;
    file << "      .stack2 { fill: #99ccff; }" << std::endl;
    file << "      .stack3 { fill: #66b2ff; }" << std::endl;
    file << "      .stack4 { fill: #3399ff; }" << std::endl;
    file << "    </style>" << std::endl;
    file << "  </defs>" << std::endl;
    file << "  <g transform=\"translate(10,10)\">" << std::endl;
    
    // Simple placeholder flame graph
    double maxDuration = 0.0;
    for (const auto& event : flameGraphEvents_) {
        maxDuration = std::max(maxDuration, event.duration);
    }
    
    if (maxDuration > 0.0) {
        double scale = 1000.0 / maxDuration; // pixels per second
        
        for (size_t i = 0; i < flameGraphEvents_.size(); ++i) {
            const auto& event = flameGraphEvents_[i];
            double x = event.timestamp * scale;
            double width = event.duration * scale;
            int stack = std::min(static_cast<size_t>(event.stackHash % 10), size_t(4));
            
            file << "    <rect x=\"" << x << "\" y=\"" << i * 20 << "\" width=\"" << width << "\" height=\"15\" "
                 << "class=\"stack" << stack << "\" />" << std::endl;
            
            file << "    <text x=\"" << x + 5 << "\" y=\"" << i * 20 + 12 << "\" font-size=\"10px\">"
                 << event.operation << "</text>" << std::endl;
        }
    }
    
    file << "  </g>" << std::endl;
    file << "</svg>" << std::endl;
    file.close();
}

void Benchmark::writeDetailedReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "=== NLM Benchmark Detailed Report ===" << std::endl;
    file << "Generated at: " << getWallClockTime() << " seconds" << std::endl;
    file << std::endl;
    
    // Write benchmark history
    file << "=== Benchmark History ===" << std::endl;
    for (const auto& result : benchmarkHistory_) {
        file << "Benchmark: " << result.benchmarkName << std::endl;
        file << "  Duration: " << std::fixed << std::setprecision(3) << result.duration << "s" << std::endl;
        file << "  Operations/sec: " << std::fixed << std::setprecision(0) << result.opsPerSecond << std::endl;
        file << "  SIMD Level: " << result.simdLevel << std::endl;
        file << "  Threads: " << result.numThreads << " (Efficiency: " << std::fixed << std::setprecision(1) << result.efficiency * 100.0 << "%)" << std::endl;
        file << "  Memory Usage: " << (result.memoryUsed / 1024 / 1024) << " MB" << std::endl;
        file << "  Memory Bandwidth: " << std::fixed << std::setprecision(2) << result.memoryBandwidth << " GB/s" << std::endl;
        file << "  Instructions/cycle: " << std::fixed << std::setprecision(2) << result.instructionsPerCycle" << std::endl;
        file << std::endl;
    }
    
    // Write scaling results
    file << "=== Scaling Results ===" << std::endl;
    for (const auto& result : scalingResults_) {
        file << "Neuron Count: " << result.neuronCount << std::endl;
        file << "  Neurons/sec: " << result.neuronsPerSecond << std::endl;
        file << "  Memory/Neuron: " << result.memoryPerNeuron << " bytes" << std::endl;
        file << "  Parallel Speedup: " << result.parallelSpeedup << std::endl;
        file << "  Real-time Factor: " << result.realtimeFactor << std::endl;
        file << std::endl;
    }
    
    // Write comparative results
    file << "=== Comparative Results ===" << std::endl;
    for (const auto& result : comparativeResults_) {
        file << "Configuration: " << result.configuration << std::endl;
        file << "  Performance: " << result.performance << std::endl;
        file << "  Memory Usage: " << (result.memoryUsage / 1024 / 1024) << " MB" << std::endl;
        file << "  Quality Score: " << result.qualityScore << std::endl;
        file << "  Hardware: " << result.hardware << std::endl;
        file << "  Instructions/cycle: " << result.instructionsPerCycle" << std::endl;
        file << "  Branch Prediction Accuracy: " << result.branchPredictionAccuracy * 100.0 << "%" << std::endl;
        file << std::endl;
    }
    
    // Write current characteristics
    file << "=== Current Performance Characteristics ===" << std::endl;
    auto& characs = currentCharacteristics_;
    file << "Neurons per second: " << characs.neuronsPerSecond << std::endl;
    file << "Spikes per second: " << characs.spikesPerSecond << std::endl;
    file << "Memory bandwidth: " << characs.memoryBandwidth << " GB/s" << std::endl;
    file << "Cache hit rate: " << characs.cacheHitRate * 100.0 << "%" << std::endl;
    file << "SIMD utilization: " << characs.simdUtilization * 100.0 << "%" << std::endl;
    file << "Parallel efficiency: " << characs.parallelEfficiency * 100.0 << "%" << std::endl;
    file << "Event processing latency: " << characs.eventProcessingLatency << " s" << std::endl;
    file << "Synchronization overhead: " << characs.synchronizationOverhead << " s" << std::endl;
    file << std::endl;
    
    file.close();
}

void Benchmark::writePerformanceSummary(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "=== NLM Performance Summary ===" << std::endl;
    file << "Generated at: " << getWallClockTime() << " seconds" << std::endl;
    file << std::endl;
    
    // Calculate summary statistics
    double totalDuration = 0.0;
    double totalOpsPerSec = 0.0;
    double totalMemoryUsage = 0.0;
    double totalMemoryBandwidth = 0.0;
    
    for (const auto& result : benchmarkHistory_) {
        totalDuration += result.duration;
        totalOpsPerSec += result.opsPerSecond;
        totalMemoryUsage += result.memoryUsage;
        totalMemoryBandwidth += result.memoryBandwidth;
    }
    
    size_t numResults = benchmarkHistory_.size();
    if (numResults > 0) {
        file << "=== Summary Statistics ===" << std::endl;
        file << "Average Duration: " << std::fixed << std::setprecision(3) << (totalDuration / numResults) << "s" << std::endl;
        file << "Average Operations/sec: " << std::fixed << std::setprecision(0) << (totalOpsPerSec / numResults) << std::endl;
        file << "Average Memory Usage: " << (totalMemoryUsage / numResults / 1024 / 1024) << " MB" << std::endl;
        file << "Average Memory Bandwidth: " << std::fixed << std::setprecision(2) << (totalMemoryBandwidth / numResults) << " GB/s" << std::endl;
        file << std::endl;
        
        // Best benchmark
        const BenchmarkResult* best = nullptr;
        double bestPerformance = 0.0;
        
        for (const auto& result : benchmarkHistory_) {
            double score = result.opsPerSecond * result.efficiency;
            if (score > bestPerformance) {
                bestPerformance = score;
                best = &result;
            }
        }
        
        if (best) {
            file << "=== Best Performance ===" << std::endl;
            file << "Benchmark: " << best->benchmarkName << std::endl;
            file << "Score: " << bestPerformance << std::endl;
            file << "Operations/sec: " << best->opsPerSecond << std::endl;
            file << "Efficiency: " << best->efficiency * 100.0 << "%" << std::endl;
            file << std::endl;
        }
    }
    
    // Write optimization recommendations
    file << "=== Optimization Recommendations ===" << std::endl;
    std::vector<std::string> recommendations = getOptimizationRecommendations();
    if (recommendations.empty()) {
        file << "No optimization recommendations available." << std::endl;
    } else {
        for (const auto& recommendation : recommendations) {
            file << "- " << recommendation << std::endl;
        }
    }
    file << std::endl;
    
    file.close();
}

} // namespace nlm

// NLM Enhanced Performance Benchmarking System - Implementation
// EnhancedBenchmarkManager.cpp

#include "EnhancedBenchmarkManager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <thread>
#include <chrono>
#include <sys/resource.h>
#include <unistd.h>

namespace nlm {
namespace benchmarking {

struct EnhancedBenchmarkManager::Impl {
    std::thread monitoringThread;
    std::atomic<bool> monitoringRunning;
    std::atomic<BenchmarkStatus> currentStatus;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> phaseStartTimes;
    std::map<std::string, int64_t> phaseAccumulatedTimes;
    int64_t cpuTimeAtStart;
    int64_t wallClockAtStart;
    std::vector<std::pair<std::string, int64_t>> resourceSampleHistory;
    size_t maxHistorySize;
    
    Impl() : monitoringRunning(false), currentStatus(BenchmarkStatus::IDLE),
             cpuTimeAtStart(0), wallClockAtStart(0), maxHistorySize(100) {
        // Initialize resource tracking
        phaseAccumulatedTimes["setup"] = 0;
        phaseAccumulatedTimes["simulation"] = 0;
        phaseAccumulatedTimes["teardown"] = 0;
    }
};

EnhancedBenchmarkManager::EnhancedBenchmarkManager(std::shared_ptr<Logger> logger)
    : status_(BenchmarkStatus::IDLE), overallProgress_(0.0), logger_(logger),
      monitoringEnabled_(false) {
    pImpl = std::make_unique<Impl>();
    if (logger_) {
        logger_->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
    }
}

EnhancedBenchmarkManager::~EnhancedBenchmarkManager() {
    if (monitoringEnabled_) {
        cancelCurrentBenchmark();
    }
}

bool EnhancedBenchmarkManager::registerBenchmark(std::shared_ptr<BenchmarkBase> benchmark) {
    if (!benchmark) {
        NLM_LOG_ERROR("Cannot register null benchmark");
        return false;
    }
    
    benchmarks_.push_back(benchmark);
    NLM_LOG_INFO("Registered benchmark: " + benchmark->getName() + " (" + 
                std::to_string(static_cast<int>(benchmark->getType())) + ")");
    return true;
}

bool EnhancedBenchmarkManager::runBenchmark(const std::string& name, std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the benchmark
    auto benchmarkIt = std::find_if(benchmarks_.begin(), benchmarks_.end(),
                                   [&name](const std::shared_ptr<BenchmarkBase>& b) {
                                       return b->getName() == name || 
                                              (static_cast<int>(b->getType()) == 
                                               (name == "standard" ? 0 : 
                                                name == "memory" ? 1 : 
                                                name == "plasticity" ? 2 : 
                                                name == "spike" ? 3 : 
                                                name == "integration" ? 4 : 
                                                name == "scalability" ? 5 : -1));
                                   });
    
    if (benchmarkIt == benchmarks_.end()) {
        NLM_LOG_ERROR("Benchmark not found: " + name);
        return false;
    }
    
    auto benchmark = *benchmarkIt;
    
    // Validate configuration
    if (!benchmark->validateConfig(config_)) {
        NLM_LOG_ERROR("Configuration validation failed for benchmark: " + name);
        return false;
    }
    
    // Check system requirements
    auto requiredFeatures = benchmark->getRequiredSystemFeatures();
    for (const auto& feature : requiredFeatures) {
        if (!checkSystemFeature(feature)) {
            NLM_LOG_ERROR("System requirement not met for feature: " + feature);
            return false;
        }
    }
    
    // Start monitoring
    status_ = BenchmarkStatus::RUNNING;
    overallProgress_ = 0.0;
    currentResult_ = BenchmarkResult();
    monitoringEnabled_ = true;
    
    pImpl->monitoringRunning = true;
    pImpl->currentStatus = BenchmarkStatus::RUNNING;
    pImpl->cpuTimeAtStart = getCurrentCPUTime();
    pImpl->wallClockAtStart = PerformanceCounter::getNanoseconds();
    
    // Set monitor if available
    if (monitor_) {
        benchmark->setMonitor(monitor_);
    }
    
    NLM_LOG_INFO("Starting benchmark: " + name);
    NLM_LOG_INFO("Neuron count: " + std::to_string(config_.neuronCount));
    NLM_LOG_INFO("Steps: " + std::to_string(config_.steps));
    NLM_LOG_INFO("Mode: " + config_.mode);
    
    // Start monitoring thread
    pImpl->monitoringThread = std::thread(&EnhancedBenchmarkManager::monitoringLoop, this);
    
    // Run the benchmark
    try {
        currentResult_ = benchmark->run(brain, config_);
        currentResult_.benchmarkName = name;
        
        // Finalize result
        finalizeBenchmarkResult(currentResult_);
        
        status_ = BenchmarkStatus::COMPLETED;
        pImpl->currentStatus = BenchmarkStatus::COMPLETED;
        
        // Store result
        results_.push_back(currentResult_);
        
        // Save result if configured
        if (!config_.outputFile.empty()) {
            saveResult(config_.outputFile, currentResult_);
        }
        
        NLM_LOG_INFO("Benchmark completed: " + name);
        NLM_LOG_INFO("Total time: " + std::to_string(currentResult_.getTotalTimeSeconds()) + "s");
        NLM_LOG_INFO("Steps per second: " + std::to_string(currentResult_.stepsPerSecond));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Benchmark error: " + std::string(e.what()));
        status_ = BenchmarkStatus::ERROR;
        pImpl->currentStatus = BenchmarkStatus::ERROR;
        
        if (monitor_) {
            monitor_->onBenchmarkError(std::string(e.what()));
        }
        
        return false;
    }
    
    // Wait for monitoring thread
    if (pImpl->monitoringThread.joinable()) {
        pImpl->monitoringRunning = false;
        pImpl->monitoringThread.join();
    }
    
    monitoringEnabled_ = false;
    overallProgress_ = 1.0;
    
    return true;
}

void EnhancedBenchmarkManager::cancelCurrentBenchmark() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (status_ != BenchmarkStatus::RUNNING) {
        return;
    }
    
    status_ = BenchmarkStatus::CANCELLED;
    pImpl->currentStatus = BenchmarkStatus::CANCELLED;
    pImpl->monitoringRunning = false;
    
    if (monitor_) {
        monitor_->onInterrupt();
    }
    
    NLM_LOG_INFO("Benchmark cancelled by user");
}

void EnhancedBenchmarkManager::pauseBenchmark() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (status_ != BenchmarkStatus::RUNNING) {
        return;
    }
    
    status_ = BenchmarkStatus::PAUSED;
    pImpl->currentStatus = BenchmarkStatus::PAUSED;
    
    if (monitor_) {
        monitor_->onPause();
    }
    
    NLM_LOG_INFO("Benchmark paused");
}

void EnhancedBenchmarkManager::resumeBenchmark() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (status_ != BenchmarkStatus::PAUSED) {
        return;
    }
    
    status_ = BenchmarkStatus::RUNNING;
    pImpl->currentStatus = BenchmarkStatus::RUNNING;
    
    if (monitor_) {
        monitor_->onResume();
    }
    
    NLM_LOG_INFO("Benchmark resumed");
}

void EnhancedBenchmarkManager::attachMonitor(std::shared_ptr<RealTimeMonitor> monitor) {
    std::lock_guard<std::mutex> lock(mutex_);
    monitor_ = monitor;
}

void EnhancedBenchmarkManager::monitoringLoop() {
    while (pImpl->monitoringRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        updateResourceMetrics();
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Calculate progress
        if (config_.steps > 0) {
            overallProgress_ = static_cast<double>(currentResult_.steps) / config_.steps;
        }
        
        if (monitor_) {
            monitor_->onBenchmarkProgress(overallProgress_, currentResult_);
        }
        
        if (status_ == BenchmarkStatus::COMPLETED || status_ == BenchmarkStatus::CANCELLED) {
            pImpl->monitoringRunning = false;
        }
    }
}

void EnhancedBenchmarkManager::updateResourceMetrics() {
    auto memoryUsage = getCurrentMemoryUsage();
    auto cpuMetrics = getCurrentCPUMetrics();
    
    // Add to history
    pImpl->resourceSampleHistory.emplace_back("memory", memoryUsage.usedBytes);
    pImpl->resourceSampleHistory.emplace_back("cpu", static_cast<int64_t>(cpuMetrics.usagePercent * 1000000));
    
    // Limit history size
    if (pImpl->resourceSampleHistory.size() > pImpl->maxHistorySize) {
        pImpl->resourceSampleHistory.erase(pImpl->resourceSampleHistory.begin(),
                                         pImpl->resourceSampleHistory.begin() + 
                                         (pImpl->resourceSampleHistory.size() - pImpl->maxHistorySize));
    }
    
    // Update result metrics
    currentResult_.memoryUsage = memoryUsage;
    currentResult_.cpuMetrics = cpuMetrics;
    
    // Update historical averages
    if (!pImpl->resourceSampleHistory.empty()) {
        std::vector<double> memoryValues;
        std::vector<double> cpuValues;
        
        for (const auto& sample : pImpl->resourceSampleHistory) {
            if (sample.first == "memory") {
                memoryValues.push_back(static_cast<double>(sample.second) / (1024*1024)); // Convert to MB
            } else if (sample.first == "cpu") {
                cpuValues.push_back(static_cast<double>(sample.second) / 1000000.0); // Convert to percentage
            }
        }
        
        if (!memoryValues.empty()) {
            double sum = std::accumulate(memoryValues.begin(), memoryValues.end(), 0.0);
            currentResult_.memoryHistory.push_back(sum / memoryValues.size());
        }
        
        if (!cpuValues.empty()) {
            double sum = std::accumulate(cpuValues.begin(), cpuValues.end(), 0.0);
            currentResult_.cpuHistory.push_back(sum / cpuValues.size());
        }
    }
}

bool EnhancedBenchmarkManager::checkSystemFeature(const std::string& feature) const {
    // Implementation would check for specific system features
    // For now, return true for all features
    return true;
}

void EnhancedBenchmarkManager::finalizeBenchmarkResult(BenchmarkResult& result) {
    // Calculate final metrics
    auto endWallClock = PerformanceCounter::getNanoseconds();
    int64_t elapsedWallClock = endWallClock - pImpl->wallClockAtStart;
    
    int64_t endCPUTime = getCurrentCPUTime();
    int64_t elapsedCPUTime = endCPUTime - pImpl->cpuTimeAtStart;
    
    result.wallClockTimeNs = elapsedWallClock;
    result.cpuTimeNs = elapsedCPUTime;
    
    if (elapsedWallClock > 0) {
        result.stepsPerSecond = static_cast<double>(result.steps) / (elapsedWallClock / 1e9);
    }
    
    // Calculate neural metrics
    result.neuralMetrics = getCurrentNeuralMetrics(nullptr); // Would need brain reference
    
    // Add phase times to result
    result.phaseTimes = pImpl->phaseAccumulatedTimes;
    
    // Calculate statistics for history
    if (!result.memoryHistory.empty()) {
        double sum = std::accumulate(result.memoryHistory.begin(), result.memoryHistory.end(), 0.0);
        result.memoryUsage.percentageUsed = (sum / result.memoryHistory.size()) * 100.0;
    }
    
    if (!result.cpuHistory.empty()) {
        double sum = std::accumulate(result.cpuHistory.begin(), result.cpuHistory.end(), 0.0);
        result.cpuMetrics.usagePercent = sum / result.cpuHistory.size();
    }
}

MemoryUsage EnhancedBenchmarkManager::getCurrentMemoryUsage() const {
    MemoryUsage mem;
    
    // Get memory usage from system
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    // Convert from KB to bytes
    mem.usedBytes = usage.ru_maxrss * 1024;  // ru_maxrss is in KB on Linux
    
    // Estimate total memory (simplified - would need more sophisticated detection)
    mem.totalBytes = mem.usedBytes * 10; // Estimate 10x for containerized environments
    mem.availableBytes = mem.totalBytes - mem.usedBytes;
    
    if (mem.totalBytes > 0) {
        mem.percentageUsed = (static_cast<double>(mem.usedBytes) / mem.totalBytes) * 100.0;
    }
    
    return mem;
}

CPUMetrics EnhancedBenchmarkManager::getCurrentCPUMetrics() const {
    CPUMetrics cpu;
    
    // Get CPU time
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    // Convert to nanoseconds
    cpu.userTime = usage.ru_utime.tv_sec * 1000000000LL + usage.ru_utime.tv_usec * 1000;
    cpu.systemTime = usage.ru_stime.tv_sec * 1000000000LL + usage.ru_stime.tv_usec * 1000;
    cpu.totalTime = cpu.userTime + cpu.systemTime;
    
    if (cpu.totalTime > 0) {
        cpu.usagePercent = (static_cast<double>(cpu.userTime + cpu.systemTime) / cpu.totalTime) * 100.0;
    }
    
    // For idle time, we'd need to track previous measurements
    cpu.idleTime = 0; // Simplified
    
    return cpu;
}

NeuralMetrics EnhancedBenchmarkManager::getCurrentNeuralMetrics(std::shared_ptr<Brain> brain) const {
    NeuralMetrics neural;
    
    // This would collect metrics from the brain if provided
    // For now, return default values
    neural.spikeCount = 0;
    neural.firingNeuronCount = 0;
    neural.activeNeuronCount = 0;
    neural.avgFiringRate = 0.0;
    neural.excitationInhibitionRatio = 1.0;
    neural.memoryAccesses = 0;
    neural.plasticityOperations = 0;
    
    return neural;
}

void EnhancedBenchmarkManager::saveResult(const std::string& filename, const BenchmarkResult& result) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for writing: " + filename);
        return;
    }
    
    if (config_.outputFormat == "csv") {
        file << "benchmark,neurons,synapses,steps,stepsPerSecond,totalTimeSeconds,memoryUsagePercent,cpuUsagePercent,spikeCount,firingNeuronCount\n";
        file << result.toCSV() << "\n";
    } else if (config_.outputFormat == "json") {
        file << result.toJSON() << "\n";
    } else {
        // Default text format
        file << "Benchmark Results:" << std::endl;
        file << "===================" << std::endl;
        file << "Name: " << result.benchmarkName << std::endl;
        file << "Neurons: " << result.neuronCount << std::endl;
        file << "Synapses: " << result.synapseCount << std::endl;
        file << "Steps: " << result.steps << std::endl;
        file << "Total Time: " << result.getTotalTimeSeconds() << "s" << std::endl;
        file << "Steps per Second: " << result.stepsPerSecond << std::endl;
        file << "Memory Usage: " << result.memoryUsage.percentageUsed << "%" << std::endl;
        file << "CPU Usage: " << result.cpuMetrics.usagePercent << "%" << std::endl;
        file << "Spike Count: " << result.neuralMetrics.spikeCount << std::endl;
        file << "Firing Neuron Count: " << result.neuralMetrics.firingNeuronCount << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Results saved to: " + filename);
}

std::string EnhancedBenchmarkManager::generateReport(const std::vector<BenchmarkResult>& results, 
                                                      const std::string& format) const {
    if (format == "csv" || config_.outputFormat == "csv") {
        std::stringstream ss;
        ss << "benchmark,neurons,synapses,steps,stepsPerSecond,totalTimeSeconds,memoryUsagePercent,cpuUsagePercent,spikeCount,firingNeuronCount\n";
        for (const auto& result : results) {
            ss << result.toCSV() << "\n";
        }
        return ss.str();
    } else if (format == "json" || config_.outputFormat == "json") {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < results.size(); ++i) {
            ss << results[i].toJSON();
            if (i < results.size() - 1) ss << ",\n";
        }
        ss << "]";
        return ss.str();
    } else {
        // Text format
        std::stringstream ss;
        ss << "Benchmark Report" << std::endl;
        ss << "=================" << std::endl;
        ss << "Total benchmarks: " << results.size() << std::endl << std::endl;
        
        for (const auto& result : results) {
            ss << "Benchmark: " << result.benchmarkName << std::endl;
            ss << "  Neurons: " << result.neuronCount << std::endl;
            ss << "  Synapses: " << result.synapseCount << std::endl;
            ss << "  Steps: " << result.steps << std::endl;
            ss << "  Total Time: " << result.getTotalTimeSeconds() << "s" << std::endl;
            ss << "  Steps per Second: " << result.stepsPerSecond << std::endl;
            ss << "  Memory Usage: " << result.memoryUsage.percentageUsed << "%" << std::endl;
            ss << "  CPU Usage: " << result.cpuMetrics.usagePercent << "%" << std::endl;
            ss << "  Spike Count: " << result.neuralMetrics.spikeCount << std::endl;
            ss << "  Firing Neuron Count: " << result.neuralMetrics.firingNeuronCount << std::endl;
            ss << std::endl;
        }
        
        return ss.str();
    }
}

std::string EnhancedBenchmarkManager::compareResults(const BenchmarkResult& result1, 
                                                     const BenchmarkResult& result2) const {
    std::stringstream ss;
    ss << "Comparison Report" << std::endl;
    ss << "=================" << std::endl;
    ss << "Benchmark 1: " << result1.benchmarkName << std::endl;
    ss << "Benchmark 2: " << result2.benchmarkName << std::endl << std::endl;
    
    // Compare key metrics
    ss << "Key Metrics Comparison:" << std::endl;
    ss << "  Steps per Second - " << result1.benchmarkName << ": " << result1.stepsPerSecond
       << ", " << result2.benchmarkName << ": " << result2.stepsPerSecond << std::endl;
    ss << "  Total Time - " << result1.benchmarkName << ": " << result1.getTotalTimeSeconds()
       << "s, " << result2.benchmarkName << ": " << result2.getTotalTimeSeconds() << "s" << std::endl;
    ss << "  Memory Usage - " << result1.benchmarkName << ": " << result1.memoryUsage.percentageUsed << "%, "
       << result2.benchmarkName << ": " << result2.memoryUsage.percentageUsed << "%" << std::endl;
    ss << "  CPU Usage - " << result1.benchmarkName << ": " << result1.cpuMetrics.usagePercent << "%, "
       << result2.benchmarkName << ": " << result2.cpuMetrics.usagePercent << "%" << std::endl;
    ss << "  Spike Count - " << result1.benchmarkName << ": " << result1.neuralMetrics.spikeCount << ", "
       << result2.benchmarkName << ": " << result2.neuralMetrics.spikeCount << std::endl;
    
    return ss.str();
}

bool EnhancedBenchmarkManager::detectRegression(const BenchmarkResult& baseline, 
                                              const BenchmarkResult& current,
                                              const std::map<std::string, double>& thresholds) const {
    bool hasRegression = false;
    std::stringstream ss;
    
    ss << "Regression Detection Report" << std::endl;
    ss << "=========================" << std::endl;
    
    // Check steps per second regression
    if (current.stepsPerSecond < baseline.stepsPerSecond) {
        double regressionPercent = ((baseline.stepsPerSecond - current.stepsPerSecond) / 
                                    baseline.stepsPerSecond) * 100.0;
        ss << "WARNING: Steps per second regression: " << regressionPercent << "% "
           << "(baseline: " << baseline.stepsPerSecond << ", current: " << current.stepsPerSecond << ")" << std::endl;
        hasRegression = true;
    }
    
    // Check memory usage regression
    if (current.memoryUsage.percentageUsed > baseline.memoryUsage.percentageUsed) {
        double regressionPercent = ((current.memoryUsage.percentageUsed - baseline.memoryUsage.percentageUsed) / 
                                    baseline.memoryUsage.percentageUsed) * 100.0;
        ss << "WARNING: Memory usage regression: " << regressionPercent << "% "
           << "(baseline: " << baseline.memoryUsage.percentageUsed << "%, "
           << "current: " << current.memoryUsage.percentageUsed << "%)" << std::endl;
        hasRegression = true;
    }
    
    // Check CPU usage regression
    if (current.cpuMetrics.usagePercent > baseline.cpuMetrics.usagePercent) {
        double regressionPercent = ((current.cpuMetrics.usagePercent - baseline.cpuMetrics.usagePercent) / 
                                    baseline.cpuMetrics.usagePercent) * 100.0;
        ss << "WARNING: CPU usage regression: " << regressionPercent << "% "
           << "(baseline: " << baseline.cpuMetrics.usagePercent << "%, "
           << "current: " << current.cpuMetrics.usagePercent << "%)" << std::endl;
        hasRegression = true;
    }
    
    // Check spike count regression
    if (current.neuralMetrics.spikeCount < baseline.neuralMetrics.spikeCount) {
        double regressionPercent = ((baseline.neuralMetrics.spikeCount - current.neuralMetrics.spikeCount) / 
                                    baseline.neuralMetrics.spikeCount) * 100.0;
        ss << "WARNING: Spike count regression: " << regressionPercent << "% "
           << "(baseline: " << baseline.neuralMetrics.spikeCount << ", "
           << "current: " << current.neuralMetrics.spikeCount << ")" << std::endl;
        hasRegression = true;
    }
    
    if (hasRegression) {
        ss << "\nREGRESSION DETECTED: Performance has degraded compared to baseline." << std::endl;
    } else {
        ss << "\nNo regression detected." << std::endl;
    }
    
    return hasRegression;
}

int64_t EnhancedBenchmarkManager::getCurrentCPUTime() const {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    int64_t cpuTime = usage.ru_utime.tv_sec * 1000000000LL + usage.ru_utime.tv_usec * 1000;
    cpuTime += usage.ru_stime.tv_sec * 1000000000LL + usage.ru_stime.tv_usec * 1000;
    
    return cpuTime;
}

}}} // namespace nlm::benchmarking

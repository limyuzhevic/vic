// Performance Profiler implementation
// Provides detailed profiling information for simulation optimization

#include "profiling/PerformanceProfiler.hpp"
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <csignal>
#include <sys/resource.h>
#include <unistd.h>

namespace nlm {
namespace profiling {

PerformanceProfiler& PerformanceProfiler::getInstance() {
    static PerformanceProfiler profiler;
    return profiler;
}

PerformanceProfiler::PerformanceProfiler() : totalTime_(0), enabled_(true) {
}

PerformanceProfiler::~PerformanceProfiler() {
}

void PerformanceProfiler::startProfile(const std::string& sectionName) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::high_resolution_clock::now();
    auto cpuNow = std::chrono::high_resolution_clock::now();
    
    ProfileContext context;
    context.startTime = now;
    context.cpuStartTime = cpuNow;
    context.initialMemory = getCurrentMemoryUsage();
    
    activeProfiles_[sectionName] = std::move(context);
}

void PerformanceProfiler::stopProfile(const std::string& sectionName) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = activeProfiles_.find(sectionName);
    if (it == activeProfiles_.end()) {
        return;
    }
    
    auto now = std::chrono::high_resolution_clock::now();
    auto cpuNow = std::chrono::high_resolution_clock::now();
    
    const auto& context = it->second;
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - context.startTime);
    auto cpuTime = std::chrono::duration_cast<std::chrono::duration<double>>(cpuNow - context.cpuStartTime);
    
    size_t finalMemory = getCurrentMemoryUsage();
    size_t memoryDelta = finalMemory - context.initialMemory;
    
    addMetric(sectionName, elapsed, cpuTime, memoryDelta, 1);
    
    activeProfiles_.erase(it);
}

const PerformanceMetrics* PerformanceProfiler::getMetrics(const std::string& sectionName) const {
    auto it = completedMetrics_.find(sectionName);
    if (it != completedMetrics_.end()) {
        return &it->second;
    }
    return nullptr;
}

void PerformanceProfiler::addMetric(const std::string& name, 
                                   const std::chrono::duration<double>& elapsed,
                                   const std::chrono::duration<double>& cpu,
                                   size_t memoryDelta,
                                   uint64_t callCount) {
    auto it = completedMetrics_.find(name);
    if (it == completedMetrics_.end()) {
        completedMetrics_[name] = PerformanceMetrics(name);
        it = completedMetrics_.find(name);
    }
    
    PerformanceMetrics& metrics = it->second;
    metrics.elapsedTime += elapsed;
    metrics.cpuTime += cpu;
    metrics.memoryUsage += memoryDelta;
    metrics.callCount += callCount;
    
    double currentElapsed = std::chrono::duration<double>(elapsed).count();
    double currentCpu = std::chrono::duration<double>(cpu).count();
    
    if (metrics.callCount == 1) {
        metrics.minTime = currentElapsed;
        metrics.maxTime = currentElapsed;
        metrics.averageTime = currentElapsed;
    } else {
        metrics.averageTime = metrics.elapsedTime.count() / metrics.callCount;
        metrics.minTime = std::min(metrics.minTime, currentElapsed);
        metrics.maxTime = std::max(metrics.maxTime, currentElapsed);
    }
    
    metrics_.push_back(metrics);
    
    // Remove duplicate metrics (keep the most recent)
    if (metrics_.size() > 100) {
        metrics_.erase(metrics_.begin());
    }
}

void PerformanceProfiler::printReport(std::ostream& out) const {
    out << "Performance Profiler Report" << std::endl;
    out << "==========================" << std::endl;
    out << std::endl;
    
    if (metrics_.empty()) {
        out << "No profiling data available." << std::endl;
        return;
    }
    
    out << "Total simulation time: " << totalTime_ << " seconds" << std::endl;
    out << std::endl;
    
    // Print summary table
    out << std::left << std::setw(30) << "Section" 
        << std::right << std::setw(15) << "Calls" 
        << std::right << std::setw(15) << "Avg Time (ms)" 
        << std::right << std::setw(15) << "Min Time (ms)" 
        << std::right << std::setw(15) << "Max Time (ms)" 
        << std::right << std::setw(15) << "Memory (MB)" 
        << std::endl;
    
    out << std::string(100, '-') << std::endl;
    
    for (const auto& metrics : metrics_) {
        double avgTimeMs = metrics.averageTime * 1000;
        double minTimeMs = metrics.minTime * 1000;
        double maxTimeMs = metrics.maxTime * 1000;
        double memoryMB = metrics.memoryUsage / (1024.0 * 1024.0);
        
        out << std::left << std::setw(30) << metrics.name
            << std::right << std::setw(15) << metrics.callCount
            << std::right << std::setw(15) << std::fixed << std::setprecision(3) << avgTimeMs
            << std::right << std::setw(15) << std::fixed << std::setprecision(3) << minTimeMs
            << std::right << std::setw(15) << std::fixed << std::setprecision(3) << maxTimeMs
            << std::right << std::setw(15) << std::fixed << std::setprecision(3) << memoryMB
            << std::endl;
    }
}

void PerformanceProfiler::printDetailedReport(std::ostream& out) const {
    out << "Detailed Performance Profiler Report" << std::endl;
    out << "=====================================" << std::endl;
    out << std::endl;
    
    for (const auto& metrics : metrics_) {
        out << "Section: " << metrics.name << std::endl;
        out << "  Total calls: " << metrics.callCount << std::endl;
        out << "  Total elapsed time: " << metrics.elapsedTime.count() << " seconds" << std::endl;
        out << "  Total CPU time: " << metrics.cpuTime.count() << " seconds" << std::endl;
        out << "  Average time per call: " << metrics.averageTime * 1000 << " milliseconds" << std::endl;
        out << "  Minimum time per call: " << metrics.minTime * 1000 << " milliseconds" << std::endl;
        out << "  Maximum time per call: " << metrics.maxTime * 1000 << " milliseconds" << std::endl;
        out << "  Memory delta: " << metrics.memoryUsage << " bytes" << std::endl;
        out << "  Average CPU time per call: " << (metrics.cpuTime.count() / metrics.callCount) * 1000 << " milliseconds" << std::endl;
        out << std::endl;
    }
}

void PerformanceProfiler::clearMetrics() {
    std::lock_guard<std::mutex> lock(mutex_);
    completedMetrics_.clear();
    metrics_.clear();
    totalTime_ = 0;
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    activeProfiles_.clear();
    completedMetrics_.clear();
    metrics_.clear();
    totalTime_ = 0;
}

size_t PerformanceProfiler::getCurrentMemoryUsage() const {
    // Get memory usage from system
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;  // In KB on Linux
}

// MemoryProfiler implementation

MemoryProfiler::MemoryProfiler() {
}

MemoryProfiler::~MemoryProfiler() {
}

void MemoryProfiler::recordSnapshot(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemorySnapshot snapshot;
    snapshot.name = name;
    snapshot.timestamp = std::chrono::high_resolution_clock::now();
    snapshot.memoryUsage = getCurrentMemoryUsage();
    
    snapshots_.push_back(snapshot);
}

void MemoryProfiler::printMemoryReport(std::ostream& out) const {
    out << "Memory Profiler Report" << std::endl;
    out << "======================" << std::endl;
    out << std::endl;
    
    if (snapshots_.empty()) {
        out << "No memory data available." << std::endl;
        return;
    }
    
    out << "Memory usage over time:" << std::endl;
    out << std::left << std::setw(20) << "Snapshot" 
        << std::left << std::setw(15) << "Time (s)" 
        << std::left << std::setw(20) << "Memory (KB)" 
        << std::left << std::setw(20) << "Memory Growth (KB)" 
        << std::endl;
    
    out << std::string(75, '-') << std::endl;
    
    double startTime = snapshots_.front().timestamp.time_since_epoch().count();
    
    for (size_t i = 0; i < snapshots_.size(); ++i) {
        const auto& snapshot = snapshots_[i];
        double timeSinceStart = (snapshot.timestamp.time_since_epoch().count() - startTime) / 1e9;
        
        double memoryGrowth = 0;
        if (i > 0) {
            memoryGrowth = snapshot.memoryUsage - snapshots_[i-1].memoryUsage;
        }
        
        out << std::left << std::setw(20) << snapshot.name
            << std::left << std::setw(15) << std::fixed << std::setprecision(3) << timeSinceStart
            << std::left << std::setw(20) << snapshot.memoryUsage
            << std::left << std::setw(20) << std::fixed << std::setprecision(3) << memoryGrowth
            << std::endl;
    }
}

double MemoryProfiler::getMemoryGrowth(const std::string& beforeName, 
                                       const std::string& afterName) const {
    auto beforeIt = std::find_if(snapshots_.begin(), snapshots_.end(),
        [&beforeName](const MemorySnapshot& s) { return s.name == beforeName; });
    
    auto afterIt = std::find_if(snapshots_.begin(), snapshots_.end(),
        [&afterName](const MemorySnapshot& s) { return s.name == afterName; });
    
    if (beforeIt == snapshots_.end() || afterIt == snapshots_.end()) {
        return 0.0;
    }
    
    return afterIt->memoryUsage - beforeIt->memoryUsage;
}

double MemoryProfiler::getMemoryUsageMB() const {
    if (snapshots_.empty()) {
        return 0.0;
    }
    
    double totalKB = 0;
    for (const auto& snapshot : snapshots_) {
        totalKB += snapshot.memoryUsage;
    }
    
    return totalKB / 1024.0;
}

void MemoryProfiler::clearSnapshots() {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshots_.clear();
}

// Benchmark implementation

Benchmark::Benchmark() {
}

Benchmark::~Benchmark() {
}

void Benchmark::addBenchmark(const std::string& name, 
                            std::function<void()> test,
                            const std::map<std::string, std::string>& params) {
    BenchmarkTest benchmark;
    benchmark.name = name;
    benchmark.test = test;
    benchmark.params = params;
    
    benchmarks_.push_back(benchmark);
}

void Benchmark::runBenchmarks() {
    PerformanceProfiler::getInstance().clearMetrics();
    PerformanceProfiler::getInstance().setEnabled(true);
    
    for (auto& benchmark : benchmarks_) {
        std::cout << "Running benchmark: " << benchmark.name << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        benchmark.test();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
        benchmark.results.push_back(elapsedTime);
        benchmark.avgTime = elapsedTime;
        
        results_.push_back({benchmark.name, benchmark.results});
        
        std::cout << "  Completed in " << elapsedTime << " seconds" << std::endl;
    }
    
    PerformanceProfiler::getInstance().setEnabled(false);
}

void Benchmark::printBenchmarkResults(std::ostream& out) const {
    out << "Benchmark Results" << std::endl;
    out << "=================" << std::endl;
    out << std::endl;
    
    for (const auto& result : results_) {
        const std::string& name = result.first;
        const std::vector<double>& times = result.second;
        
        if (times.empty()) continue;
        
        double minTime = *std::min_element(times.begin(), times.end());
        double maxTime = *std::max_element(times.begin(), times.end());
        double avgTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        
        out << "Benchmark: " << name << std::endl;
        out << "  Runs: " << times.size() << std::endl;
        out << "  Min time: " << minTime << " seconds" << std::endl;
        out << "  Max time: " << maxTime << " seconds" << std::endl;
        out << "  Average time: " << avgTime << " seconds" << std::endl;
        out << "  Standard deviation: " << std::sqrt(std::accumulate(times.begin(), times.end(), 0.0,
            [avgTime](double sum, double t) { return sum + (t - avgTime) * (t - avgTime); }) / times.size())
            << " seconds" << std::endl;
        out << std::endl;
    }
}

// SimulationProfiler implementation

SimulationProfiler::SimulationProfiler() 
    : maxNeurons_(0), totalActivityLevel_(0), totalSteps_(0) {
}

SimulationProfiler::~SimulationProfiler() {
}

void SimulationProfiler::startSimulationProfiling(const std::string& simulationName) {
    simulationName_ = simulationName;
    startTime_ = std::chrono::high_resolution_clock::now();
}

void SimulationProfiler::stopSimulationProfiling() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime_);
    
    std::cout << "Simulation " << simulationName_ << " completed in " << duration.count() << " seconds" << std::endl;
}

void SimulationProfiler::recordStepMetrics(uint64_t step, 
                                           size_t spikeCount,
                                           size_t neuronCount,
                                           double activityLevel) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    spikeHistory_.push_back({step, spikeCount});
    activityHistory_.push_back(activityLevel);
    
    maxNeurons_ = std::max(maxNeurons_, neuronCount);
    totalActivityLevel_ += activityLevel;
    totalSteps_++;
}

void SimulationProfiler::printSimulationPerformance(std::ostream& out) const {
    out << "Simulation Performance Report" << std::endl;
    out << "============================" << std::endl;
    out << std::endl;
    
    if (totalSteps_ == 0) {
        out << "No simulation data available." << std::endl;
        return;
    }
    
    out << "Total steps: " << totalSteps_ << std::endl;
    out << "Max neurons: " << maxNeurons_ << std::endl;
    out << "Average activity level: " << (totalActivityLevel_ / totalSteps_) << std::endl;
    
    if (!spikeHistory_.empty()) {
        out << std::endl;
        out << "Spike history:" << std::endl;
        out << std::left << std::setw(15) << "Step" 
            << std::left << std::setw(15) << "Spikes" 
            << std::left << std::setw(20) << "Spike Rate" 
            << std::endl;
        
        out << std::string(50, '-') << std::endl;
        
        for (const auto& history : spikeHistory_) {
            double spikeRate = static_cast<double>(history.second) / totalSteps_ * 100;
            out << std::left << std::setw(15) << history.first
                << std::left << std::setw(15) << history.second
                << std::left << std::setw(20) << spikeRate << std::endl;
        }
    }
}

} // namespace profiling
} // namespace nlm
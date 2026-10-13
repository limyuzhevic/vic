#include "Profiler.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

// CallProfiler implementation
CallProfiler::ProfilingMode CallProfiler::currentMode = ProfilingMode::Detailed;
std::shared_ptr<CallProfiler> CallProfiler::globalProfiler = nullptr;

CallProfiler::CallProfiler(const std::string& name) : name_(name), enabled_(true) {
}

CallProfiler::~CallProfiler() {
    if (enabled_) {
        stopProfiling();
    }
}

void CallProfiler::startProfiling() {
    if (!enabled_) return;
    
    if (currentMode == ProfilingMode::None) return;
    
    startTime = std::chrono::high_resolution_clock::now();
}

void CallProfiler::stopProfiling() {
    if (!enabled_) return;
    
    if (currentMode == ProfilingMode::None) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    duration_ = duration.count();
}

void CallProfiler::setProfilingMode(ProfilingMode mode) {
    currentMode = mode;
}

void CallProfiler::setGlobalProfiler(std::shared_ptr<CallProfiler> profiler) {
    globalProfiler = profiler;
}

std::shared_ptr<CallProfiler> CallProfiler::getGlobalProfiler() {
    return globalProfiler;
}

// Profiler implementation
struct Profiler::Impl {
    bool initialized = false;
    bool running = false;
    ProfilingMode mode = ProfilingMode::Detailed;
    std::string scope = "default";
    size_t maxSamples = 1000;
    double sampleInterval = 0.1;  // 100ms
    std::string outputFile;
    
    std::chrono::system_clock::time_point startTime;
    std::chrono::duration<double> cpuTime;
    
    std::vector<double> frameTimes;
    std::vector<size_t> memoryUsages;
    std::vector<size_t> allocations;
    std::map<std::string, std::vector<double>> customMetrics;
    
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    double totalTime = 0.0;
    double minFrameTime = std::numeric_limits<double>::max();
    double maxFrameTime = 0.0;
    size_t peakMemory = 0;
    size_t totalAllocations = 0;
    
    std::mutex sampleMutex;
};

Profiler::Profiler() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = true;
    pImpl->startTime = std::chrono::system_clock::now();
}

Profiler::~Profiler() = default;

void Profiler::start() {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    
    if (pImpl->running) return;
    
    pImpl->running = true;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->lastFrameTime = pImpl->startTime;
    pImpl->cpuTime = std::chrono::duration<double>::zero();
    
    NLM_LOG_INFO("Profiler started");
}

void Profiler::stop() {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    
    if (!pImpl->running) return;
    
    pImpl->running = false;
    auto endTime = std::chrono::system_clock::now();
    pImpl->cpuTime = endTime - pImpl->startTime;
    pImpl->totalTime = pImpl->cpuTime.count();
    
    NLM_LOG_INFO("Profiler stopped");
}

void Profiler::setMode(ProfilingMode mode) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    pImpl->mode = mode;
}

void Profiler::setProfilingScope(const std::string& scope) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    pImpl->scope = scope;
}

void Profiler::setMaxSamples(size_t maxSamples) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    pImpl->maxSamples = maxSamples;
    
    // Trim existing buffers if needed
    if (pImpl->frameTimes.size() > maxSamples) {
        size_t excess = pImpl->frameTimes.size() - maxSamples;
        pImpl->frameTimes.erase(pImpl->frameTimes.begin(), pImpl->frameTimes.begin() + excess);
    }
}

void Profiler::setSampleInterval(double interval) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    pImpl->sampleInterval = interval;
}

void Profiler::setOutputFile(const std::string& filepath) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    pImpl->outputFile = filepath;
}

void Profiler::recordFrame(double frameTime) {
    if (!pImpl->initialized || !pImpl->running) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    
    pImpl->frameTimes.push_back(frameTime);
    
    // Update min/max frame times
    if (frameTime < pImpl->minFrameTime) pImpl->minFrameTime = frameTime;
    if (frameTime > pImpl->maxFrameTime) pImpl->maxFrameTime = frameTime;
    
    // Trim buffer if needed
    if (pImpl->frameTimes.size() > pImpl->maxSamples) {
        pImpl->frameTimes.erase(pImpl->frameTimes.begin());
    }
    
    sampleData();
}

void Profiler::recordMemoryUsage(size_t memoryBytes) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    
    pImpl->memoryUsages.push_back(memoryBytes);
    
    if (memoryBytes > pImpl->peakMemory) {
        pImpl->peakMemory = memoryBytes;
    }
    
    // Trim buffer if needed
    if (pImpl->memoryUsages.size() > pImpl->maxSamples) {
        pImpl->memoryUsages.erase(pImpl->memoryUsages.begin());
    }
    
    sampleData();
}

void Profiler::recordAllocation(size_t bytesAllocated) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    
    pImpl->allocations.push_back(bytesAllocated);
    pImpl->totalAllocations += bytesAllocated;
    
    // Trim buffer if needed
    if (pImpl->allocations.size() > pImpl->maxSamples) {
        pImpl->allocations.erase(pImpl->allocations.begin());
    }
    
    sampleData();
}

void Profiler::recordCustomMetric(const std::string& name, double value) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    
    pImpl->customMetrics[name].push_back(value);
    
    // Trim buffer if needed
    if (pImpl->customMetrics[name].size() > pImpl->maxSamples) {
        pImpl->customMetrics[name].erase(pImpl->customMetrics[name].begin());
    }
    
    sampleData();
}

PerformanceMetrics Profiler::getMetrics() const {
    PerformanceMetrics metrics;
    
    if (!pImpl->initialized) return metrics;
    
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    
    metrics.totalTime = pImpl->totalTime;
    metrics.cpuTime = pImpl->cpuTime.count();
    metrics.memoryUsage = pImpl->peakMemory;
    metrics.allocationCount = pImpl->totalAllocations;
    metrics.minFrameTime = pImpl->minFrameTime;
    metrics.maxFrameTime = pImpl->maxFrameTime;
    
    // Calculate average FPS
    if (!pImpl->frameTimes.empty()) {
        double totalFrames = static_cast<double>(pImpl->frameTimes.size());
        double totalTime = std::accumulate(pImpl->frameTimes.begin(), pImpl->frameTimes.end(), 0.0);
        metrics.averageFPS = totalTime > 0.0 ? (totalFrames / totalTime) : 0.0;
    }
    
    metrics.customMetrics = pImpl->customMetrics;
    
    return metrics;
}

std::string Profiler::generateReport() const {
    PerformanceMetrics metrics = getMetrics();
    std::ostringstream oss;
    
    oss << "=== NLM Profiler Report ===\n";
    oss << "Scope: " << pImpl->scope << "\n";
    oss << "Total Time: " << std::fixed << std::setprecision(3) << metrics.totalTime << "s\n";
    oss << "CPU Time: " << std::fixed << std::setprecision(3) << metrics.cpuTime << "s\n";
    oss << "Peak Memory: " << metrics.memoryUsage << " bytes\n";
    oss << "Total Allocations: " << metrics.allocationCount << " bytes\n";
    
    if (!pImpl->frameTimes.empty()) {
        double avgFrameTime = std::accumulate(pImpl->frameTimes.begin(), pImpl->frameTimes.end(), 0.0) / pImpl->frameTimes.size();
        oss << "Average Frame Time: " << std::fixed << std::setprecision(6) << avgFrameTime << "s\n";
        oss << "Average FPS: " << std::fixed << std::setprecision(2) << metrics.averageFPS << "\n";
        oss << "Frame Time Range: [" << std::fixed << std::setprecision(6) << metrics.minFrameTime << ", " << metrics.maxFrameTime << "] s\n";
    }
    
    oss << "\nCustom Metrics:\n";
    for (const auto& [name, values] : pImpl->customMetrics) {
        if (!values.empty()) {
            double avg = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
            oss << "  " << name << ": " << std::fixed << std::setprecision(4) << avg << " (n=" << values.size() << ")\n";
        }
    }
    
    return oss.str();
}

void Profiler::printReport() const {
    std::cout << generateReport() << std::endl;
}

void Profiler::sampleData() {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->lastFrameTime);
    
    if (elapsed.count() >= pImpl->sampleInterval) {
        // Update CPU time
        auto currentTime = std::chrono::system_clock::now();
        pImpl->cpuTime += currentTime - pImpl->lastFrameTime;
        
        pImpl->lastFrameTime = now;
    }
}

void Profiler::resetRecordingBuffers() {
    std::lock_guard<std::mutex> lock(pImpl->sampleMutex);
    
    pImpl->frameTimes.clear();
    pImpl->memoryUsages.clear();
    pImpl->allocations.clear();
    pImpl->customMetrics.clear();
    
    pImpl->minFrameTime = std::numeric_limits<double>::max();
    pImpl->maxFrameTime = 0.0;
    pImpl->peakMemory = 0;
    pImpl->totalAllocations = 0;
}

// Convenience functions
Profiler& getProfiler() {
    static Profiler profiler;
    return profiler;
}

void startProfiling() {
    getProfiler().start();
}

void stopProfiling() {
    getProfiler().stop();
}

bool isProfiling() {
    return getProfiler().isRunning();
}

// Benchmarker implementation
std::map<std::string, std::vector<double>> Benchmarker::globalBenchmarks_;
std::mutex Benchmarker::benchmarksMutex_;

Benchmarker::Benchmarker(const std::string& benchmarkName) : benchmarkName_(benchmarkName) {
}

Benchmarker::~Benchmarker() {
    if (running_) {
        stop();
    }
}

void Benchmarker::start() {
    if (running_) return;
    
    running_ = true;
    duration_ = 0.0;
    startTime_ = std::chrono::high_resolution_clock::now();
}

void Benchmarker::stop() {
    if (!running_) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime_);
    duration_ = duration.count();
    
    durations_.push_back(duration_);
    
    // Add to global benchmarks
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    globalBenchmarks_[benchmarkName_].push_back(duration_);
    
    running_ = false;
}

double Benchmarker::getDuration() const {
    if (running_) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime_);
        return duration.count();
    }
    return duration_;
}

double Benchmarker::getMean() const {
    if (durations_.empty()) return 0.0;
    
    double sum = std::accumulate(durations_.begin(), durations_.end(), 0.0);
    return sum / durations_.size();
}

double Benchmarker::getMedian() const {
    if (durations_.empty()) return 0.0;
    
    std::vector<double> sorted = durations_;
    std::sort(sorted.begin(), sorted.end());
    
    size_t n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
    } else {
        return sorted[n/2];
    }
}

double Benchmarker::getStandardDeviation() const {
    if (durations_.size() <= 1) return 0.0;
    
    double mean = getMean();
    double sum = 0.0;
    for (double duration : durations_) {
        sum += (duration - mean) * (duration - mean);
    }
    return std::sqrt(sum / (durations_.size() - 1));
}

double Benchmarker::getMin() const {
    if (durations_.empty()) return 0.0;
    return *std::min_element(durations_.begin(), durations_.end());
}

double Benchmarker::getMax() const {
    if (durations_.empty()) return 0.0;
    return *std::max_element(durations_.begin(), durations_.end());
}

void Benchmarker::startBenchmark(const std::string& name) {
    static std::map<std::string, Benchmarker> activeBenchmarks;
    activeBenchmarks[name] = Benchmarker(name);
    activeBenchmarks[name].start();
}

void Benchmarker::stopBenchmark(const std::string& name) {
    static std::map<std::string, Benchmarker>& activeBenchmarks = globalBenchmarks_;
    if (activeBenchmarks.find(name) != activeBenchmarks.end()) {
        activeBenchmarks[name].stop();
    }
}

void Benchmarker::printBenchmarkReport() {
    std::cout << "=== NLM Benchmark Report ===\n" << std::endl;
    
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    
    for (const auto& [name, durations] : globalBenchmarks_) {
        if (durations.empty()) continue;
        
        std::cout << "Benchmark: " << name << "\n";
        std::cout << "  Runs: " << durations.size() << "\n";
        std::cout << "  Mean: " << std::fixed << std::setprecision(4) << getMean(durations) << "s\n";
        std::cout << "  Median: " << std::fixed << std::setprecision(4) << getMedian(durations) << "s\n";
        std::cout << "  Min: " << std::fixed << std::setprecision(4) << getMin(durations) << "s\n";
        std::cout << "  Max: " << std::fixed << std::setprecision(4) << getMax(durations) << "s\n";
        std::cout << "  Std Dev: " << std::fixed << std::setprecision(4) << getStandardDeviation(durations) << "s\n";
        std::cout << std::endl;
    }
}

void Benchmarker::clearBenchmarks() {
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    globalBenchmarks_.clear();
}

// MemoryProfiler implementation
MemoryProfiler::MemoryProfiler() {
}

MemoryProfiler::~MemoryProfiler() {
    if (tracking_) {
        stopTracking();
    }
}

void MemoryProfiler::startTracking() {
    if (tracking_) return;
    
    tracking_ = true;
    currentMemory_ = 0;
    peakMemory_ = 0;
    totalAllocated_ = 0;
    totalDeallocated_ = 0;
}

void MemoryProfiler::stopTracking() {
    if (!tracking_) return;
    
    tracking_ = false;
}

void MemoryProfiler::recordAllocation(size_t bytes, const std::string& allocationSite) {
    if (!tracking_) return;
    
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    currentMemory_ += bytes;
    totalAllocated_ += bytes;
    
    if (currentMemory_ > peakMemory_) {
        peakMemory_ = currentMemory_;
    }
    
    if (!allocationSite.empty()) {
        allocationSites_[allocationSite] += bytes;
    }
}

void MemoryProfiler::recordDeallocation(size_t bytes, const std::string& allocationSite) {
    if (!tracking_) return;
    
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    currentMemory_ -= bytes;
    totalDeallocated_ += bytes;
    
    if (allocationSite.empty()) {
        // For now, just track total deallocations
        // In a full implementation, this would match allocations
    }
}

 size_t MemoryProfiler::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    return currentMemory_;
}

 size_t MemoryProfiler::getPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    return peakMemory_;
}

 size_t MemoryProfiler::getTotalAllocated() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    return totalAllocated_;
}

 size_t MemoryProfiler::getTotalDeallocated() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    return totalDeallocated_;
}

 void MemoryProfiler::detectMemoryLeaks() {
    if (!tracking_) return;
    
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    // Check for potential memory leaks
    if (currentMemory_ > 0) {
        NLM_LOG_WARNING("Potential memory leak detected: " + std::to_string(currentMemory_) + " bytes still allocated");
    }
    
    // Report allocation sites
    if (!allocationSites_.empty()) {
        NLM_LOG_INFO("Allocation sites:");
        for (const auto& [site, bytes] : allocationSites_) {
            NLM_LOG_INFO("  " + site + ": " + std::to_string(bytes) + " bytes");
        }
    }
}

 std::string MemoryProfiler::generateMemoryReport() const {
    std::ostringstream oss;
    
    oss << "=== NLM Memory Profiler Report ===\n";
    oss << "Current Memory Usage: " << getCurrentMemoryUsage() << " bytes\n";
    oss << "Peak Memory Usage: " << getPeakMemoryUsage() << " bytes\n";
    oss << "Total Allocated: " << getTotalAllocated() << " bytes\n";
    oss << "Total Deallocated: " << getTotalDeallocated() << " bytes\n";
    oss << "Net Memory: " << (getTotalAllocated() - getTotalDeallocated()) << " bytes\n";
    
    oss << "\nAllocation Sites:\n";
    for (const auto& [site, bytes] : allocationSites_) {
        oss << "  " << site << ": " << bytes << " bytes\n";
    }
    
    return oss.str();
}

 void MemoryProfiler::printMemoryReport() const {
     std::cout << generateMemoryReport() << std::endl;
 }

} // namespace nlm

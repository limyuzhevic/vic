#include "PerformanceMonitor.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

// Performance counter implementations

PerformanceCounter::PerformanceCounter() : isRunning_(false), accumulatedTime_(0.0) {}

void PerformanceCounter::start() {
    if (!isRunning_) {
        startTime_ = std::chrono::high_resolution_clock::now();
        isRunning_ = true;
    }
}

void PerformanceCounter::stop() {
    if (isRunning_) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        accumulatedTime_ += duration.count() / 1e6; // Convert to seconds
        isRunning_ = false;
    }
}

inline double PerformanceCounter::elapsed() const {
    if (isRunning_) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        return accumulatedTime_ + duration.count() / 1e6;
    }
    return accumulatedTime_;
}

inline double PerformanceCounter::elapsedMs() const {
    return elapsed() * 1000.0;
}

void PerformanceCounter::reset() {
    accumulatedTime_ = 0.0;
    isRunning_ = false;
}

// PerformanceMonitor implementations

PerformanceMonitor& PerformanceMonitor::getInstance() {
    static PerformanceMonitor instance;
    return instance;
}

PerformanceMonitor::PerformanceMonitor() : isActive_(false) {
}

PerformanceMonitor::~PerformanceMonitor() {
    shutdown();
}

void PerformanceMonitor::initialize() {
    std::lock_guard<std::mutex> lock(operationsMutex_);
    startTime_ = std::chrono::high_resolution_clock::now();
    isActive_ = true;
}

void PerformanceMonitor::shutdown() {
    isActive_ = false;
}

void PerformanceMonitor::startOperation(const std::string& name) {
    if (!isActive_) return;
    
    std::lock_guard<std::mutex> lock(operationsMutex_);
    
    auto it = operations_.find(name);
    if (it != operations_.end()) {
        it->second.counter.start();
    } else {
        Operation op(name);
        op.counter.start();
        operations_[name] = op;
    }
}

void PerformanceMonitor::stopOperation(const std::string& name) {
    if (!isActive_) return;
    
    std::lock_guard<std::mutex> lock(operationsMutex_);
    
    auto it = operations_.find(name);
    if (it != operations_.end()) {
        it->second.counter.stop();
        it->second.callCount++;
        it->second.lastUpdate = std::chrono::high_resolution_clock::now();
    }
}

PerformanceMetrics PerformanceMonitor::getMetrics(const std::string& name) const {
    std::lock_guard<std::mutex> lock(operationsMutex_);
    
    PerformanceMetrics metrics;
    auto it = operations_.find(name);
    if (it == operations_.end()) {
        return metrics;
    }
    
    const auto& op = it->second;
    
    // Calculate timing metrics
    metrics.totalTime = op.counter.elapsed();
    metrics.calls = op.callCount;
    
    if (metrics.calls > 0) {
        metrics.avgTime = metrics.totalTime / metrics.calls;
        metrics.minTime = metrics.avgTime; // In real implementation, we'd track min/max
        metrics.maxTime = metrics.avgTime;
    }
    
    return metrics;
}

std::unordered_map<std::string, PerformanceMetrics> PerformanceMonitor::getAllMetrics() const {
    std::lock_guard<std::mutex> lock(operationsMutex_);
    
    std::unordered_map<std::string, PerformanceMetrics> allMetrics;
    for (const auto& pair : operations_) {
        allMetrics[pair.first] = getMetrics(pair.first);
    }
    
    return allMetrics;
}

double PerformanceMonitor::getTotalTime() const {
    std::lock_guard<std::mutex> lock(operationsMutex_);
    
    double total = 0.0;
    for (const auto& pair : operations_) {
        total += pair.second.counter.elapsed();
    }
    return total;
}

size_t PerformanceMonitor::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    return currentMemoryUsage_;
}

size_t PerformanceMonitor::getPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    return peakMemoryUsage_;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock1(operationsMutex_);
    std::lock_guard<std::mutex> lock2(globalMetricsMutex_);
    
    operations_.clear();
    globalMetrics_ = PerformanceMetrics();
    currentMemoryUsage_ = 0;
    peakMemoryUsage_ = 0;
}

void PerformanceMonitor::printReport(std::ostream& out) const {
    auto metrics = getAllMetrics();
    
    out << "=== NLM Performance Monitor Report ===" << std::endl;
    out << std::fixed << std::setprecision(3);
    
    if (!metrics.empty()) {
        out << std::left << std::setw(30) << "Operation" 
            << std::right << std::setw(10) << "Calls" 
            << std::right << std::setw(15) << "Total Time" 
            << std::right << std::setw(15) << "Avg Time" 
            << std::right << std::setw(15) << "Peak Mem" 
            << std::endl;
        out << std::string(90, '-') << std::endl;
        
        for (const auto& pair : metrics) {
            const auto& name = pair.first;
            const auto& m = pair.second;
            
            out << std::left << std::setw(30) << name
                << std::right << std::setw(10) << m.calls
                << std::right << std::setw(15) << m.totalTime
                << std::right << std::setw(15) << m.avgTime
                << std::right << std::setw(15) << m.memoryPeak
                << std::endl;
        }
    }
    
    out << std::endl;
    out << "Global Statistics:" << std::endl;
    out << "  Total Time: " << getTotalTime() << " seconds" << std::endl;
    out << "  Current Memory: " << getCurrentMemoryUsage() << " bytes" << std::endl;
    out << "  Peak Memory: " << getPeakMemoryUsage() << " bytes" << std::endl;
}

bool PerformanceMonitor::saveReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    printReport(file);
    return true;
}

std::vector<std::pair<std::string, double>> PerformanceMonitor::getBottlenecks(size_t count) const {
    auto metrics = getAllMetrics();
    
    // Convert to vector for sorting
    std::vector<std::pair<std::string, double>> bottleneckList;
    for (const auto& pair : metrics) {
        bottleneckList.emplace_back(pair.first, pair.second.avgTime);
    }
    
    // Sort by average time (descending)
    std::sort(bottleneckList.begin(), bottleneckList.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top N bottlenecks
    if (bottleneckList.size() > count) {
        bottleneckList.resize(count);
    }
    
    return bottleneckList;
}

void PerformanceMonitor::calculateThroughput() {
    std::lock_guard<std::mutex> lock(operationsMutex_);
    
    for (auto& pair : operations_) {
        auto& op = pair.second;
        if (op.callCount > 0) {
            op.counter.stop();
            double time = op.counter.elapsed();
            if (time > 0.0) {
                op.counter.stop();
                double throughput = op.callCount / time;
                // Update global metrics if needed
            }
            op.counter.start(); // Restart for next measurement
        }
    }
}

void PerformanceMonitor::updateMemoryMetrics(size_t allocated, size_t peak) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    currentMemoryUsage_ = allocated;
    if (peak > peakMemoryUsage_) {
        peakMemoryUsage_ = peak;
    }
}

void PerformanceMonitor::updateCPUMetrics(double cpuTime, int threadCount) {
    // Update global CPU metrics
    std::lock_guard<std::mutex> lock(globalMetricsMutex_);
    globalMetrics_.cpuTime = cpuTime;
    globalMetrics_.threadCount = threadCount;
}

void PerformanceMonitor::cleanupOldOperations(double maxAgeSeconds) {
    if (!isActive_) return;
    
    std::lock_guard<std::mutex> lock(operationsMutex_);
    
    auto now = std::chrono::high_resolution_clock::now();
    auto maxAge = std::chrono::duration<double>(maxAgeSeconds);
    
    for (auto it = operations_.begin(); it != operations_.end(); ) {
        auto age = std::chrono::duration_cast<std::chrono::duration<double>>(now - it->second.lastUpdate);
        if (age > maxAge) {
            it = operations_.erase(it);
        } else {
            ++it;
        }
    }
}

// MemoryProfiler implementations

MemoryProfiler::MemoryProfiler() : 
    currentMemoryUsage_(0),
    peakMemoryUsage_(0),
    allocationCount_(0),
    deallocationCount_(0),
    totalMemoryAllocated_(0) {
}

void MemoryProfiler::trackAllocation(size_t size) {
    allocationCount_++;
    totalMemoryAllocated_ += size;
    currentMemoryUsage_ += size;
    if (currentMemoryUsage_ > peakMemoryUsage_) {
        peakMemoryUsage_ = currentMemoryUsage_;
    }
}

void MemoryProfiler::trackDeallocation(size_t size) {
    deallocationCount_++;
    currentMemoryUsage_ = (currentMemoryUsage_ >= size) ? (currentMemoryUsage_ - size) : 0;
}

size_t MemoryProfiler::getCurrentMemoryUsage() const {
    return currentMemoryUsage_.load();
}

size_t MemoryProfiler::getPeakMemoryUsage() const {
    return peakMemoryUsage_.load();
}

uint64_t MemoryProfiler::getAllocationCount() const {
    return allocationCount_.load();
}

uint64_t MemoryProfiler::getDeallocationCount() const {
    return deallocationCount_.load();
}

size_t MemoryProfiler::getTotalMemoryAllocated() const {
    return totalMemoryAllocated_.load();
}

void MemoryProfiler::reset() {
    currentMemoryUsage_ = 0;
    peakMemoryUsage_ = 0;
    allocationCount_ = 0;
    deallocationCount_ = 0;
    totalMemoryAllocated_ = 0;
}

// CPUMonitor implementations

CPUMonitor::CPUMonitor() : cpuTime_(0.0), threadCount_(0) {
}

void CPUMonitor::update(double cpuTime, int threadCount) {
    cpuTime_ = cpuTime;
    threadCount_ = threadCount;
}

double CPUMonitor::getCurrentCPUTime() const {
    return cpuTime_.load();
}

int CPUMonitor::getCurrentThreadCount() const {
    return threadCount_.load();
}

double CPUMonitor::getCPUEfficiency() const {
    return threadCount_.load() > 0 ? 1.0 : 0.0;
}

void CPUMonitor::reset() {
    cpuTime_ = 0.0;
    threadCount_ = 0;
}

} // namespace nlm
// debug_profiler.cpp - Implementation of debugging and profiling utilities

#include "debug_profiler.hpp"
#include <sstream>
#include <iomanip>

// PerformanceProfiler implementation
template<typename Func>
auto PerformanceProfiler::profileFunction(const std::string& operationName, Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = func();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration<double>(end - start).count();
    addSample(operationName, duration);
    
    return result;
}

void PerformanceProfiler::addSample(const std::string& operationName, double duration,
                                   const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    samples_.emplace_back(operationName, duration, file, line);
    
    // Update stats
    auto& stats = stats_[operationName];
    stats.operationName = operationName;
    stats.addSample(duration);
    
    // Limit samples to prevent memory issues
    if (samples_.size() > maxSamples_) {
        samples_.erase(samples_.begin(), samples_.begin() + (samples_.size() - maxSamples_));
    }
}

// MemoryDebugger implementation
void MemoryDebugger::trackAllocation(void* address, size_t size, const std::string& site,
                                   int line, const std::string& file) {
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_[address] = MemoryBlock(address, size, site, line, file);
    totalAllocated_ += size;
    peakAllocated_ = std::max(peakAllocated_, totalAllocated_);
}

void MemoryDebugger::trackDeallocation(void* address) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = allocations_.find(address);
    if (it != allocations_.end()) {
        totalAllocated_ -= it->second.size;
        allocations_.erase(it);
    }
}

// BrainDebugger implementation
// Note: BrainDebugger implementation depends on Brain class interface
// which would need to be included from the brain module

#pragma once

#include "MemoryProfiler.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <cmath>

namespace nlm {

class MemoryProfiler {
public:
    struct MemoryAllocation {
        void* address;
        size_t size;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string allocationSite;
        uint64_t threadId;
        bool freed;
        
        MemoryAllocation() : address(nullptr), size(0), freed(false) {}
        MemoryAllocation(void* addr, size_t sz, const std::string& site)
            : address(addr), size(sz), allocationSite(site), freed(false) {
            timestamp = std::chrono::high_resolution_clock::now();
            threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        }
    };

    MemoryProfiler();
    ~MemoryProfiler();

    // Disable copying, enable moving
    MemoryProfiler(const MemoryProfiler&) = delete;
    MemoryProfiler& operator=(const MemoryProfiler&) = delete;
    MemoryProfiler(MemoryProfiler&&) noexcept;
    MemoryProfiler& operator=(MemoryProfiler&&) noexcept;

    // Memory tracking
    void trackAllocation(void* address, size_t size, const std::string& allocationSite);
    void trackDeallocation(void* address);
    void trackMemoryUsage(const std::string& tag, size_t memoryUsage);

    // Query
    std::vector<MemoryAllocation> getActiveAllocations() const;
    size_t getTotalAllocations() const;
    size_t getCurrentMemoryUsage() const;
    float getMemoryLeakRate() const;

    // Analysis
    size_t getMemoryLeakCount() const;
    std::vector<void*> getMemoryLeaks() const;
    double getAverageAllocationSize() const;

    // Integration
    void setBrain(Brain* brain);
    void setNeuralDynamics(NeuralDynamics* dynamics);

    // Statistics
    std::string getStatistics() const;
    std::string generateReport() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

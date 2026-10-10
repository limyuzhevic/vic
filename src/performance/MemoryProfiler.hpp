#ifndef NLM_MEMORYPROFILER_H
#define NLM_MEMORYPROFILER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <ctime>

namespace nlm {

class MemoryProfiler {
public:
    struct MemoryResult {
        std::string operation;
        size_t memoryAllocated;      // in bytes
        size_t peakMemory;          // in bytes
        size_t allocationCount;
        double averageAllocationSize;
    };

    MemoryProfiler() = default;
    ~MemoryProfiler() = default;

    // Record a memory allocation event
    void recordAllocation(const std::string& operationName, size_t bytes, size_t count = 1);

    // Record a memory deallocation event
    void recordDeallocation(const std::string& operationName, size_t bytes, size_t count = 1);

    // Get results for a specific operation
    const MemoryResult* getResult(const std::string& operationName) const;

    // Get all results
    const std::vector<MemoryResult>& getAllResults() const { return results_; }

    // Clear all profiling data
    void clear();

    // Print profiling summary to stdout
    void printSummary() const;

    // Print results for a specific operation
    void printOperationResults(const std::string& operationName) const;

    // Get total memory allocated
    size_t getTotalMemoryAllocated() const;

    // Get total allocation count
    size_t getTotalAllocationCount() const;

    // Reset counters
    void reset();

    // Set whether profiling is enabled
    void setEnabled(bool enabled) { enabled_ = enabled; }

    // Check if profiling is enabled
    bool isEnabled() const { return enabled_; }

    // Get current memory usage (platform-specific implementation needed)
    static size_t getCurrentMemoryUsage();

    // Get peak memory usage (platform-specific implementation needed)
    static size_t getPeakMemoryUsage();

private:
    struct MemoryData {
        size_t allocatedBytes;
        size_t peakBytes;
        size_t allocationCount;
    };

    std::unordered_map<std::string, MemoryData> memoryData_;
    std::vector<MemoryResult> results_;
    bool enabled_{true};
};

} // namespace nlm

#endif // NLM_MEMORYPROFILER_H

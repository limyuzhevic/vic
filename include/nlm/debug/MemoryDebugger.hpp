// ==============================================================================
// Memory Debugger
// ==============================================================================
#ifndef MEMORYDEBUGGER_HPP
#define MEMORYDEBUGGER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include "../src/memory/Memory.hpp"

namespace nlm {
namespace debug {

/**
 * @class MemoryDebugger
 * @brief Memory debugging and leak detection tool for NLM
 * 
 * Provides tools for memory debugging including leak detection, allocation tracking,
 * memory pool debugging, and memory optimization.
 */
class MemoryDebugger {
public:
    /**
     * @enum DebugMode
     * @brief Debug mode
     */
    enum class DebugMode {
        STANDARD,
        DETAILED,
        AGGRESSIVE,
        AUTO,
        PERFORMANCE
    };

    /**
     * @enum LeakDetectionMode
     * @brief Leak detection mode
     */
    enum class LeakDetectionMode {
        BASIC,
        DETAILED,
        COMPREHENSIVE,
        HISTOGRAM_BASED
    };

    /**
     * @struct AllocationInfo
     * @brief Allocation information
     */
    struct AllocationInfo {
        uint64_t address;
        uint64_t size;
        std::chrono::steady_clock::time_point timestamp;
        std::string allocationSite;
        std::string allocatorName;
        std::map<std::string, std::string> metadata;
        bool isLeaked;
        double referenceCount;
        std::chrono::steady_clock::time_point lastAccessed;
    };

    /**
     * @struct LeakInfo
     * @brief Leak information
     */
    struct LeakInfo {
        uint64_t address;
        uint64_t size;
        std::chrono::steady_clock::time_point timestamp;
        std::string allocationSite;
        std::string leakType;
        double confidence;
        std::vector<std::string> suggestions;
        std::map<std::string, std::string> metadata;
    };

    MemoryDebugger();
    ~MemoryDebugger();

    /**
     * @brief Start memory debugging
     * @param memory Memory system reference
     * @param mode Debug mode
     */
    void startDebugging(Memory& memory, DebugMode mode = DebugMode::DETAILED);

    /**
     * @brief Stop memory debugging
     */
    void stopDebugging();

    /**
     * @brief Detect memory leaks
     * @param mode Leak detection mode
     * @return Vector of leaks
     */
    std::vector<LeakInfo> detectMemoryLeaks(LeakDetectionMode mode = LeakDetectionMode::DETAILED);

    /**
     * @brief Get allocation information
     * @param address Memory address
     * @return Allocation information
     */
    AllocationInfo getAllocationInfo(uint64_t address);

    /**
     * @brief Get all allocations
     * @return Map of addresses to allocation information
     */
    std::map<uint64_t, AllocationInfo> getAllAllocations();

    /**
     * @brief Get allocations by allocator
     * @param allocatorName Allocator name
     * @return Vector of allocations
     */
    std::vector<AllocationInfo> getAllocationsByAllocator(const std::string& allocatorName);

    /**
     * @brief Get allocations by site
     * @param allocationSite Allocation site
     * @return Vector of allocations
     */
    std::vector<AllocationInfo> getAllocationsBySite(const std::string& allocationSite);

    /**
     * @brief Get allocations by size
     * @param minSize Minimum size
     * @param maxSize Maximum size
     * @return Vector of allocations
     */
    std::vector<AllocationInfo> getAllocationsBySize(uint64_t minSize, uint64_t maxSize);

    /**
     * @brief Get leaked allocations
     * @return Vector of leaked allocations
     */
    std::vector<AllocationInfo> getLeakedAllocations();

    /**
     * @brief Get allocation statistics
     * @return Allocation statistics
     */
    struct AllocationStatistics {
        uint64_t totalAllocations;
        uint64_t totalDeallocations;
        uint64_t activeAllocations;
        uint64_t leakedAllocations;
        uint64_t totalMemoryAllocated;
        uint64_t totalMemoryDeallocated;
        uint64_t currentMemoryUsage;
        double allocationRate;
        double deallocationRate;
        double averageAllocationSize;
        double averageDeallocationSize;
        double memoryLeakRate;
    };

    AllocationStatistics getAllocationStatistics();

    /**
     * @brief Check for memory leaks
     * @param iterations Number of iterations
     * @return true if memory leaks were detected
     */
    bool checkForMemoryLeaks(size_t iterations = 1000);

    /**
     * @brief Fix memory leaks
     * @param allocationAddress Memory address to fix
     * @return true if leak was fixed
     */
    bool fixMemoryLeak(uint64_t allocationAddress);

    /**
     * @brief Fix all memory leaks
     * @return Number of leaks fixed
     */
    size_t fixAllMemoryLeaks();

    /**
     * @brief Generate leak report
     * @return Leak report
     */
    struct LeakReport {
        size_t totalLeaks;
        uint64_t totalLeakedMemory;
        double leakPercentage;
        std::vector<LeakInfo> leaks;
        std::map<std::string, size_t> leakTypes;
        std::chrono::steady_clock::time_point detectionTime;
    };

    LeakReport generateLeakReport();

    /**
     * @brief Generate allocation report
     * @return Allocation report
     */
    struct AllocationReport {
        size_t totalAllocations;
        uint64_t totalAllocatedMemory;
        double averageAllocationSize;
        double fragmentationIndex;
        std::map<std::string, size_t> allocatorCounts;
        std::chrono::steady_clock::time_point reportTime;
    };

    AllocationReport generateAllocationReport();

    /**
     * @brief Export allocations to file
     * @param filename Output file path
     * @return true if export was successful
     */
    bool exportAllocations(const std::string& filename);

    /**
     * @brief Import allocations from file
     * @param filename Input file path
     * @return true if import was successful
     */
    bool importAllocations(const std::string& filename);

    /**
     * @brief Clear allocation tracking
     */
    void clearAllocationTracking();

    /**
     * @brief Get current memory usage
     * @return Current memory usage in bytes
     */
    uint64_t getCurrentMemoryUsage() const;

    /**
     * @brief Get peak memory usage
     * @return Peak memory usage in bytes
     */
    uint64_t getPeakMemoryUsage() const;

    /**
     * @brief Get memory usage trend
     * @return Memory usage trend
     */
    std::vector<double> getMemoryUsageTrend(size_t numSamples = 100);

    /**
     * @brief Set debug mode
     * @param mode Debug mode
     */
    void setDebugMode(DebugMode mode);

    /**
     * @brief Get debug mode
     * @return Debug mode
     */
    DebugMode getDebugMode() const;

    /**
     * @brief Set leak detection mode
     * @param mode Leak detection mode
     */
    void setLeakDetectionMode(LeakDetectionMode mode);

    /**
     * @brief Get leak detection mode
     * @return Leak detection mode
     */
    LeakDetectionMode getLeakDetectionMode() const;

    /**
     * @brief Start allocation tracking
     * @param memory Memory system reference
     */
    void startAllocationTracking(Memory& memory);

    /**
     * @brief Stop allocation tracking
     */
    void stopAllocationTracking();

    /**
     * @brief Is allocation tracking active?
     * @return true if allocation tracking is active
     */
    bool isAllocationTrackingActive() const;

    /**
     * @brief Generate memory debugging visualization
     * @param filename Output file path
     */
    void generateMemoryDebuggingVisualization(const std::string& filename);

    /**
     * @brief Generate leak detection visualization
     * @param filename Output file path
     */
    void generateLeakDetectionVisualization(const std::string& filename);

    /**
     * @brief Generate allocation tracking visualization
     * @param filename Output file path
     */
    void generateAllocationTrackingVisualization(const std::string& filename);

protected:
    /**
     * @brief Update allocation tracking
     */
    void updateAllocationTracking();

    /**
     * @brief Record allocation
     * @param address Memory address
     * @param size Allocation size
     * @param allocationSite Allocation site
     * @param allocatorName Allocator name
     */
    void recordAllocation(uint64_t address, uint64_t size, const std::string& allocationSite, const std::string& allocatorName);

    /**
     * @brief Record deallocation
     * @param address Memory address
     */
    void recordDeallocation(uint64_t address);

    /**
     * @brief Check for leaks
     */
    void checkForLeaks();

    /**
     * @brief Generate leak report
     */
    void generateLeakReportInternal();

    /**
     * @brief Generate allocation report
     */
    void generateAllocationReportInternal();

    /**
     * @brief Create visualization
     * @param filename Output file path
     */
    void createVisualization(const std::string& filename);

    /**
     * @brief Create leak visualization
     * @param filename Output file path
     */
    void createLeakVisualization(const std::string& filename);

    /**
     * @brief Create allocation visualization
     * @param filename Output file path
     */
    void createAllocationVisualization(const std::string& filename);

    /**
     * @brief Calculate leak probability
     * @param allocation Allocation information
     * @return Leak probability
     */
    double calculateLeakProbability(const AllocationInfo& allocation) const;

    /**
     * @brief Get allocation age
     * @param allocation Allocation information
     * @return Allocation age in seconds
     */
    double getAllocationAge(const AllocationInfo& allocation) const;

    /**
     * @brief Is allocation suspicious?
     * @param allocation Allocation information
     * @return true if allocation is suspicious
     */
    bool isSuspiciousAllocation(const AllocationInfo& allocation) const;

    /**
     * @brief Generate allocation site
     * @param backtrace Backtrace
     * @return Allocation site
     */
    std::string generateAllocationSite(const std::vector<std::string>& backtrace);

    /**
     * @brief Generate allocator name
     * @param allocation Allocation information
     * @return Allocator name
     */
    std::string generateAllocatorName(const AllocationInfo& allocation);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // MEMORYDEBUGGER_HPP

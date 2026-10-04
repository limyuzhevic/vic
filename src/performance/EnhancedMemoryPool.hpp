#pragma once

/**
 * EnhancedMemoryPool - Advanced memory pool with monitoring and debugging
 * 
 * Extends the basic MemoryPool with comprehensive monitoring, debugging,
 * and performance analysis capabilities. This enhanced version provides
 * detailed statistics, memory leak detection, and performance profiling
 * for production use and research purposes.
 * 
 * Key features:
 * - Detailed performance statistics
 * - Memory leak detection
 * - Debugging capabilities
 * - Performance profiling
 * - Configurable monitoring levels
 * - Integration with performance analysis systems
 * 
 * Biological motivation:
 * - Like how biological systems maintain homeostasis and track resource usage
 * - Similar to how the brain monitors its own activity and resource consumption
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <array>
#include <optional>
#include <chrono>
#include <sstream>
#include <unordered_map>
#include <string>
#include <algorithm>

namespace nlm {

/**
 * Memory usage tracking with nanosecond precision
 */
struct MemoryStats {
    size_t totalAllocated;      // Total objects allocated
    size_t currentlyAllocated;  // Objects currently in use
    size_t peakAllocated;       // Peak number of allocated objects
    size_t totalDeallocated;    // Total objects deallocated
    size_t blockCount;          // Number of memory blocks
    size_t memoryUsage;         // Total memory usage in bytes
    uint64_t allocationCount;   // Total allocation operations
    uint64_t deallocationCount; // Total deallocation operations
    double lastAllocationTime;  // Time of last allocation (nanoseconds)
    double lastDeallocationTime; // Time of last deallocation (nanoseconds)
    
    MemoryStats()
        : totalAllocated(0), currentlyAllocated(0), peakAllocated(0)
        , totalDeallocated(0), blockCount(0), memoryUsage(0)
        , allocationCount(0), deallocationCount(0)
        , lastAllocationTime(0.0), lastDeallocationTime(0.0) {}
    
    void reset() {
        totalAllocated = 0;
        currentlyAllocated = 0;
        peakAllocated = 0;
        totalDeallocated = 0;
        blockCount = 0;
        memoryUsage = 0;
        allocationCount = 0;
        deallocationCount = 0;
        lastAllocationTime = 0.0;
        lastDeallocationTime = 0.0;
    }
    
    double allocationEfficiency() const {
        if (totalAllocated == 0) return 0.0;
        return static_cast<double>(totalDeallocated) / totalAllocated;
    }
    
    std::string format() const {
        std::stringstream ss;
        ss << "MemoryPool Stats:" << std::endl;
        ss << "  Allocations: " << totalAllocated << std::endl;
        ss << "  Deallocations: " << totalDeallocated << std::endl;
        ss << "  Currently allocated: " << currentlyAllocated << std::endl;
        ss << "  Peak allocated: " << peakAllocated << std::endl;
        ss << "  Blocks: " << blockCount << std::endl;
        ss << "  Memory usage: " << memoryUsage << " bytes" << std::endl;
        ss << "  Allocation efficiency: " << (allocationEfficiency() * 100.0) << "%" << std::endl;
        return ss.str();
    }
};

/**
 * Allocation event for detailed tracking
 */
struct AllocationEvent {
    enum class Type { ALLOCATE, DEALLOCATE };
    
    Type type;
    uint64_t timestamp;      // Nanoseconds since epoch
    size_t size;              // Object size in bytes
    const char* file;         // Source file
    int line;                 // Line number
    std::string stackTrace;   // Optional stack trace for debugging
    
    AllocationEvent(Type t, size_t s, const char* f, int l)
        : type(t), size(s), file(f), line(l) {
        auto now = std::chrono::high_resolution_clock::now();
        timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()).count();
    }
};

/**
 * EnhancedMemoryPool - Advanced memory pool with monitoring capabilities
 * 
 * Template parameter T specifies the object type to allocate.
 * Provides comprehensive tracking, statistics, and debugging features.
 */
template<typename T>
class EnhancedMemoryPool {
public:
    /**
     * Create an enhanced memory pool with monitoring
     * @param blockSize Number of objects per memory block
     * @param initialBlocks Number of blocks to pre-allocate
     * @param enableEventLogging Enable detailed event logging
     * @param enableStats Enable comprehensive statistics
     */
    explicit EnhancedMemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4,
                               bool enableEventLogging = false,
                               bool enableStats = true)
        : blockSize_(blockSize)
        , eventLogging_(enableEventLogging)
        , statsEnabled_(enableStats)
        , allocationCount_(0)
        , deallocationCount_(0) {
        
        // Calculate aligned slab size for cache efficiency
        constexpr size_t cacheLineSize = 64;
        slabSize_ = sizeof(T);
        if (slabSize_ % cacheLineSize != 0) {
            slabSize_ = ((slabSize_ / cacheLineSize) + 1) * cacheLineSize;
        }
        
        // Initialize statistics
        stats_.reset();
        
        // Pre-allocate initial blocks
        for (size_t i = 0; i < initialBlocks; ++i) {
            addBlock();
        }
        
        // Initialize free list for this block
        Block* b = blocks_.back().get();
        for (size_t i = 0; i < blockSize_; ++i) {
            T* obj = b->getObject(i, slabSize_);
            MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
            node->next.store(freeList_.load(std::memory_order_relaxed), std::memory_order_relaxed);
            freeList_.store(node, std::memory_order_relaxed);
            stats_.availableCount_.fetch_add(1, std::memory_order_relaxed);
        }
        
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~EnhancedMemoryPool() {
        // Clean up all objects
        clearEvents();
    }
    
    // Disable copying
    EnhancedMemoryPool(const EnhancedMemoryPool&) = delete;
    EnhancedMemoryPool& operator=(const EnhancedMemoryPool&) = delete;
    
    /**
     * Allocate an object with event tracking
     * @return Pointer to allocated object or nullptr if pool exhausted
     */
    T* allocate(const char* file = "unknown", int line = 0) {
        // Try to get from free list first
        MemPoolNode* node = freeList_.load(std::memory_order_acquire);
        
        while (node != nullptr) {
            MemPoolNode* next = node->next.load(std::memory_order_relaxed);
            if (freeList_.compare_exchange_weak(node, next, 
                                                 std::memory_order_release, 
                                                 std::memory_order_relaxed)) {
                // Successfully acquired from free list
                T* obj = reinterpret_cast<T*>(node);
                updateStats(AllocationEvent::Type::ALLOCATE, slabSize_, file, line);
                stats_.currentlyAllocated_.fetch_add(1, std::memory_order_relaxed);
                stats_.allocatedCount_++;
                return obj;
            }
            // node was updated by CAS, reload next
        }
        
        // Free list empty, add a new block
        addBlock();
        
        // Try again
        return allocate(file, line);
    }
    
    /**
     * Return an object to the pool with event tracking
     * @param obj Pointer to object to deallocate
     */
    void deallocate(T* obj, const char* file = "unknown", int line = 0) {
        if (obj == nullptr) return;
        
        MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
        
        // Lock-free push to free list
        MemPoolNode* current = freeList_.load(std::memory_order_relaxed);
        do {
            node->next.store(current, std::memory_order_relaxed);
        } while (!freeList_.compare_exchange_weak(current, node,
                                                 std::memory_order_release,
                                                 std::memory_order_relaxed));
        
        updateStats(AllocationEvent::Type::DEALLOCATE, slabSize_, file, line);
        stats_.currentlyAllocated_.fetch_sub(1, std::memory_order_relaxed);
        stats_.deallocatedCount_++;
    }
    
    /**
     * Get current statistics
     */
    const MemoryStats& getStats() const {
        return stats_;
    }
    
    /**
     * Check if pool is empty
     */
    bool empty() const {
        return freeList_.load(std::memory_order_relaxed) == nullptr;
    }
    
    /**
     * Get number of allocated objects
     */
    size_t allocatedCount() const {
        return stats_.allocatedCount_;
    }
    
    /**
     * Get number of available objects
     */
    size_t availableCount() const {
        return stats_.availableCount_;
    }
    
    /**
     * Total memory used by pool (bytes)
     */
    size_t memoryUsage() const {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += block->data.capacity() * sizeof(uint8_t);
        }
        return total;
    }
    
    /**
     * Pre-allocate objects with event tracking
     */
    void reserve(size_t count, const char* file = "unknown", int line = 0) {
        while (stats_.availableCount_.load(std::memory_order_relaxed) < count) {
            addBlock();
        }
    }
    
    /**
     * Get allocation events for debugging
     */
    std::vector<AllocationEvent> getEvents() const {
        std::lock_guard<std::mutex> lock(eventMutex_);
        return events_;
    }
    
    /**
     * Clear all events to save memory
     */
    void clearEvents() {
        std::lock_guard<std::mutex> lock(eventMutex_);
        events_.clear();
    }
    
    /**
     * Get recent allocation events
     */
    std::vector<AllocationEvent> getRecentEvents(size_t count) const {
        std::lock_guard<std::mutex> lock(eventMutex_);
        if (events_.size() <= count) {
            return events_;
        }
        std::vector<AllocationEvent> recent(events_.end() - count, events_.end());
        return recent;
    }
    
    /**
     * Check for potential memory leaks
     */
    std::string checkMemoryLeaks() const {
        std::stringstream ss;
        
        if (stats_.currentlyAllocated_ > 0) {
            ss << "WARNING: " << stats_.currentlyAllocated_ << " objects are currently allocated but not deallocated." << std::endl;
            ss << "Potential memory leak detected!" << std::endl;
            
            // Get recent allocations to help identify leaks
            auto events = getRecentEvents(10);
            size_t allocCount = 0;
            size_t deallocCount = 0;
            
            for (const auto& event : events) {
                if (event.type == AllocationEvent::Type::ALLOCATE) {
                    allocCount++;
                } else {
                    deallocCount++;
                }
            }
            
            if (allocCount > deallocCount) {
                ss << "More allocations than deallocations in recent events." << std::endl;
            }
        }
        
        return ss.str();
    }
    
    /**
     * Get formatted statistics
     */
    std::string formatStats() const {
        return stats_.format();
    }
    
    /**
     * Get events statistics
     */
    std::string formatEventsStats() const {
        std::stringstream ss;
        ss << "Event Statistics:" << std::endl;
        ss << "  Total events recorded: " << events_.size() << std::endl;
        ss << "  Allocations: " << allocationCount_ << std::endl;
        ss << "  Deallocations: " << deallocationCount_ << std::endl;
        
        if (events_.size() > 0) {
            auto lastEventTime = getLastEventTime();
            auto duration = std::chrono::duration<double>(lastEventTime - startTime_).count();
            ss << "  Events since start: " << events_.size() << " in " << duration << " seconds" << std::endl;
            ss << "  Event rate: " << (events_.size() / std::max(duration, 0.1)) << " events/second" << std::endl;
        }
        
        return ss.str();
    }
    
    /**
     * Enable/disable event logging
     */
    void setEventLogging(bool enabled) {
        eventLogging_ = enabled;
    }
    
    /**
     * Enable/disable statistics
     */
    void setStatsEnabled(bool enabled) {
        statsEnabled_ = enabled;
    }
    
private:
    // Memory block structure
    struct Block {
        std::vector<uint8_t> data;
        Block* nextBlock;
        
        explicit Block(size_t objectSize, size_t count)
            : data(objectSize * count), nextBlock(nullptr) {}
        
        T* getObject(size_t index, size_t objectSize) {
            return reinterpret_cast<T*>(data.data() + index * objectSize);
        }
    };
    
    // Lock-free free list node
    struct MemPoolNode {
        std::atomic<MemPoolNode*> next;
        MemPoolNode() : next(nullptr) {}
    };
    
    // Add a new block to the pool
    void addBlock() {
        auto block = std::make_unique<Block>(slabSize_, blockSize_);
        
        // Initialize free list for this block
        Block* b = block.get();
        for (size_t i = 0; i < blockSize_; ++i) {
            T* obj = b->getObject(i, slabSize_);
            MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
            node->next.store(freeList_.load(std::memory_order_relaxed), std::memory_order_relaxed);
            freeList_.store(node, std::memory_order_relaxed);
            stats_.availableCount_.fetch_add(1, std::memory_order_relaxed);
        }
        
        blocks_.push_back(std::move(block));
        stats_.blockCount_++;
    }
    
    // Update statistics and events
    void updateStats(AllocationEvent::Type type, size_t size, const char* file, int line) {
        auto now = std::chrono::high_resolution_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()).count();
        
        if (statsEnabled_) {
            stats_.lastAllocationTime = timestamp;
            if (type == AllocationEvent::Type::ALLOCATE) {
                stats_.totalAllocated++;
                stats_.currentlyAllocated_++;
                stats_.peakAllocated_ = std::max(stats_.peakAllocated_, stats_.currentlyAllocated_);
                stats_.allocationCount_++;
                allocationCount_++;
            } else {
                stats_.totalDeallocated++;
                stats_.currentlyAllocated_--;
                stats_.deallocationCount_++;
                deallocationCount_++;
            }
            
            stats_.memoryUsage_ = memoryUsage();
        }
        
        if (eventLogging_) {
            std::lock_guard<std::mutex> lock(eventMutex_);
            events_.emplace_back(type, size, file, line);
        }
    }
    
    // Get last event time
    std::chrono::high_resolution_clock::time_point getLastEventTime() const {
        if (events_.empty()) {
            return startTime_;
        }
        uint64_t lastTimestamp = events_.back().timestamp;
        return startTime_ + std::chrono::nanoseconds(lastTimestamp);
    }
    
    // Member variables
    size_t blockSize_;
    bool eventLogging_;
    bool statsEnabled_;
    size_t slabSize_;
    std::atomic<MemPoolNode*> freeList_;
    std::vector<std::unique_ptr<Block>> blocks_;
    MemoryStats stats_;
    std::atomic<size_t> allocationCount_;
    std::atomic<size_t> deallocationCount_;
    std::chrono::high_resolution_clock::time_point startTime_;
    mutable std::mutex eventMutex_;
    std::vector<AllocationEvent> events_;
};
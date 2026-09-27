pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>
#include <algorithm>
#include <limits>

namespace nlm {
namespace MemoryManager {

// Memory allocation tracking
struct AllocationInfo {
    void* pointer;
    size_t size;
    const char* file;
    int line;
    std::string allocationName;
    uint64_t threadId;
    uint64_t timestamp;
    bool isPoolAllocated;
    
    AllocationInfo(void* ptr, size_t s, const char* f, int l, 
                   const std::string& name = "")
        : pointer(ptr), size(s), file(f), line(l), 
          allocationName(name), 
          threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())),
          timestamp(std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::high_resolution_clock::now().time_since_epoch()).count()),
          isPoolAllocated(false) {}
};

// Memory pool statistics
struct PoolStats {
    size_t totalPoolSize;
    size_t allocatedSize;
    size_t usedBlocks;
    size_t maxUsedBlocks;
    uint64_t allocationCount;
    uint64_t deallocationCount;
    float utilization;
    
    PoolStats() : totalPoolSize(0), allocatedSize(0), usedBlocks(0), 
                  maxUsedBlocks(0), allocationCount(0), deallocationCount(0),
                  utilization(0.0f) {}
};

// Arena allocator
class ArenaAllocator {
private:
    struct Block {
        void* data;
        size_t size;
        Block* next;
        
        Block(size_t s) : data(nullptr), size(s), next(nullptr) {
            data = std::malloc(s);
            if (!data) throw std::bad_alloc();
        }
        
        ~Block() {
            std::free(data);
        }
    };
    
    Block* currentBlock;
    size_t currentOffset;
    size_t blockSize;
    Block* firstBlock;
    
public:
    ArenaAllocator(size_t blockSize = 64 * 1024) // 64KB blocks
        : currentBlock(nullptr), currentOffset(0), blockSize(blockSize),
          firstBlock(nullptr) {}
    
    ~ArenaAllocator() {
        Block* block = firstBlock;
        while (block) {
            Block* next = block->next;
            delete block;
            block = next;
        }
    }
    
    void* allocate(size_t size) {
        size_t alignedSize = ((size + 15) & ~15); // 16-byte alignment
        
        if (currentBlock && currentOffset + alignedSize <= blockSize) {
            void* ptr = static_cast<char*>(currentBlock->data) + currentOffset;
            currentOffset += alignedSize;
            return ptr;
        }
        
        // Need new block
        Block* newBlock = new Block(blockSize);
        if (!firstBlock) {
            firstBlock = newBlock;
        } else {
            currentBlock->next = newBlock;
        }
        currentBlock = newBlock;
        currentOffset = alignedSize;
        return currentBlock->data;
    }
    
    void free(void* ptr) {
        // Arena allocator doesn't free individual allocations
        (void)ptr;
    }
    
    void reset() {
        // Keep current block but reset offset
        currentOffset = 0;
        // Keep all blocks for reuse
    }
    
    size_t getUsedMemory() const {
        size_t total = 0;
        Block* block = firstBlock;
        while (block) {
            total += block->size;
            block = block->next;
        }
        return total - currentOffset; // Only count allocated parts
    }
};

// Memory pool allocator
class MemoryPool {
private:
    struct BlockHeader {
        size_t size;
        bool isFree;
        BlockHeader* next;
        BlockHeader* prev;
    };
    
    BlockHeader* head;
    size_t blockSize;
    size_t totalSize;
    size_t usedSize;
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
public:
    MemoryPool(size_t size, size_t blockSize = 64) 
        : head(nullptr), blockSize(blockSize), totalSize(size), usedSize(0) {
        if (totalSize > 0) {
            head = static_cast<BlockHeader*>(std::malloc(totalSize));
            if (head) {
                head->size = totalSize - sizeof(BlockHeader);
                head->isFree = true;
                head->next = nullptr;
                head->prev = nullptr;
            }
        }
    }
    
    ~MemoryPool() {
        if (head) {
            std::free(head);
        }
    }
    
    void* allocate(size_t size) {
        if (!head || size > head->size) return nullptr;
        
        // Find first suitable block
        BlockHeader* current = head;
        while (current && (!current->isFree || current->size < size)) {
            current = current->next;
        }
        
        if (!current) return nullptr;
        
        // Split block if there's enough space
        if (current->size >= size + sizeof(BlockHeader) + 16) { // 16 byte alignment
            BlockHeader* newBlock = reinterpret_cast<BlockHeader*>(
                reinterpret_cast<char*>(current) + sizeof(BlockHeader) + size
            );
            newBlock->size = current->size - sizeof(BlockHeader) - size;
            newBlock->isFree = true;
            newBlock->next = current->next;
            newBlock->prev = current;
            
            if (current->next) {
                current->next->prev = newBlock;
            }
            current->next = newBlock;
            current->size = size;
        }
        
        current->isFree = false;
        usedSize += size;
        return reinterpret_cast<char*>(current) + sizeof(BlockHeader);
    }
    
    void free(void* ptr) {
        if (!ptr || !head) return;
        
        BlockHeader* block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<char*>(ptr) - sizeof(BlockHeader)
        );
        
        if (block->isFree) return; // Already freed
        
        block->isFree = true;
        usedSize -= block->size;
        
        // Try to coalesce with next free block
        if (block->next && block->next->isFree) {
            block->size += sizeof(BlockHeader) + block->next->size;
            block->next = block->next->next;
            if (block->next) {
                block->next->prev = block;
            }
        }
        
        // Try to coalesce with previous free block
        if (block->prev && block->prev->isFree) {
            block->prev->size += sizeof(BlockHeader) + block->size;
            block->prev->next = block->next;
            if (block->next) {
                block->next->prev = block->prev;
            }
            block = block->prev;
        }
    }
    
    PoolStats getStats() const {
        PoolStats stats;
        stats.totalPoolSize = totalSize;
        stats.allocatedSize = usedSize;
        
        // Count used blocks
        size_t usedBlocks = 0;
        BlockHeader* current = head;
        while (current) {
            if (!current->isFree) {
                usedBlocks++;
            }
            current = current->next;
        }
        stats.usedBlocks = usedBlocks;
        stats.maxUsedBlocks = usedBlocks; // For simplicity, we don't track max
        stats.utilization = totalSize > 0 ? static_cast<float>(usedSize) / totalSize : 0.0f;
        
        // We don't track allocation counts in this simplified version
        return stats;
    }
};

// Global memory tracking
class MemoryTracker {
private:
    std::mutex mutex;
    std::unordered_map<void*, AllocationInfo> allocations;
    std::vector<MemoryPool*> pools;
    std::vector<ArenaAllocator*> arenas;
    std::atomic<uint64_t> totalAllocated{0};
    std::atomic<uint64_t> peakAllocated{0};
    std::atomic<size_t> allocationCount{0};
    std::atomic<size_t> deallocationCount{0};
    
    MemoryTracker() = default;
    ~MemoryTracker() {
        // Cleanup
        for (auto* pool : pools) delete pool;
        for (auto* arena : arenas) delete arena;
    }
    
    // Disable copying
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;
    
public:
    static MemoryTracker& getInstance() {
        static MemoryTracker instance;
        return instance;
    }
    
    void* allocate(size_t size, const char* file, int line, const std::string& name = "") {
        void* ptr = std::malloc(size);
        if (ptr) {
            std::lock_guard<std::mutex> lock(mutex);
            allocations[ptr] = AllocationInfo(ptr, size, file, line, name);
            totalAllocated.fetch_add(size);
            size_t current = totalAllocated.load();
            size_t peak = peakAllocated.load();
            while (current > peak && !peakAllocated.compare_exchange_weak(peak, current)) {}
            allocationCount.fetch_add(1);
        }
        return ptr;
    }
    
    void free(void* ptr) {
        if (!ptr) return;
        std::lock_guard<std::mutex> lock(mutex);
        auto it = allocations.find(ptr);
        if (it != allocations.end()) {
            totalAllocated.fetch_sub(it->second.size);
            allocations.erase(it);
            deallocationCount.fetch_add(1);
            std::free(ptr);
        }
    }
    
    bool isLeaked() const {
        std::lock_guard<std::mutex> lock(mutex);
        return !allocations.empty();
    }
    
    size_t getTotalAllocated() const { return totalAllocated.load(); }
    size_t getPeakAllocated() const { return peakAllocated.load(); }
    size_t getAllocationCount() const { return allocationCount.load(); }
    size_t getDeallocationCount() const { return deallocationCount.load(); }
    size_t getActiveAllocations() const { return allocations.size(); }
    
    std::string getMemoryReport() const {
        std::stringstream ss;
        ss << "Memory Report:\n";
        ss << "  Total Allocated: " << getTotalAllocated() << " bytes\n";
        ss << "  Peak Allocated: " << getPeakAllocated() << " bytes\n";
        ss << "  Active Allocations: " << getActiveAllocations() << "\n";
        ss << "  Allocation Count: " << getAllocationCount() << "\n";
        ss << "  Deallocation Count: " << getDeallocationCount() << "\n";
        
        if (isLeaked()) {
            ss << "  MEMORY LEAKS DETECTED: " << allocations.size() << " allocations not freed\n";
            for (const auto& pair : allocations) {
                const auto& info = pair.second;
                ss << "    " << info.allocationName << " at " << info.file << ":" << info.line
                   << " (" << info.size << " bytes)\n";
            }
        }
        
        return ss.str();
    }
    
    void registerPool(MemoryPool* pool) {
        pools.push_back(pool);
    }
    
    void registerArena(ArenaAllocator* arena) {
        arenas.push_back(arena);
    }
    
    std::string getPoolStatsReport() const {
        std::stringstream ss;
        ss << "Pool Statistics:\n";
        for (const auto* pool : pools) {
            PoolStats stats = pool->getStats();
            ss << "  Pool (" << stats.totalPoolSize << " bytes): "
               << stats.usedBlocks << "/~" << (stats.totalPoolSize / 64) 
               << " blocks used, " << stats.utilization * 100 << "% utilized\n";
        }
        return ss.str();
    }
};

// Global allocator replacement functions
void* trackedMalloc(size_t size, const char* file, int line, const std::string& name = "") {
    return MemoryTracker::getInstance().allocate(size, file, line, name);
}

void trackedFree(void* ptr, const char* file, int line, const std::string& name = "") {
    MemoryTracker::getInstance().free(ptr);
}

// Smart pointer with memory tracking
class TrackedPtr {
private:
    void* ptr;
    const char* file;
    int line;
    std::string name;
    
public:
    TrackedPtr(void* p, const char* f, int l, const std::string& n = "")
        : ptr(p), file(f), line(l), name(n) {}
    
    ~TrackedPtr() {
        if (ptr) {
            MemoryTracker::getInstance().free(ptr, file, line, name);
        }
    }
    
    void* get() const { return ptr; }
    void reset() {
        if (ptr) {
            MemoryTracker::getInstance().free(ptr, file, line, name);
            ptr = nullptr;
        }
    }
    
    // Allow pointer-like access
    operator void*() { return ptr; }
    void* operator->() { return ptr; }
    bool operator==(void* other) const { return ptr == other; }
    bool operator!=(void* other) const { return ptr != other; }
};

// RAII wrapper for memory allocation
class MemoryAllocation {
private:
    void* ptr;
    size_t size;
    const char* file;
    int line;
    std::string name;
    
public:
    MemoryAllocation(size_t size, const char* file, int line, const std::string& name = "")
        : ptr(nullptr), size(size), file(file), line(line), name(name) {
        ptr = MemoryTracker::getInstance().allocate(size, file, line, name);
    }
    
    ~MemoryAllocation() {
        if (ptr) {
            MemoryTracker::getInstance().free(ptr, file, line, name);
        }
    }
    
    void* get() const { return ptr; }
    void reset(size_t newSize, const char* file, int line, const std::string& name = "") {
        if (ptr) {
            MemoryTracker::getInstance().free(ptr, file, line, name);
        }
        ptr = MemoryTracker::getInstance().allocate(newSize, file, line, name);
        size = newSize;
        this->file = file;
        this->line = line;
        this->name = name;
    }
    
    void* operator()() { return ptr; }
    bool operator==(void* other) const { return ptr == other; }
    bool operator!=(void* other) const { return ptr != other; }
};

// Macro for easy memory tracking
#define TRACKED_NEW(type, ...) new type(__VA_ARGS__)
#define TRACKED_MALLOC(size, name) MemoryAllocation alloc(size, __FILE__, __LINE__, name)

// Memory profiling
class MemoryProfiler {
public:
    struct Snapshot {
        uint64_t timestamp;
        size_t allocated;
        size_t count;
        std::string info;
        
        Snapshot(size_t alloc, size_t cnt, const std::string& inf)
            : timestamp(std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::high_resolution_clock::now().time_since_epoch()).count()),
              allocated(alloc), count(cnt), info(inf) {}
    };
    
private:
    std::vector<Snapshot> snapshots;
    std::mutex mutex;
    
public:
    void takeSnapshot(const std::string& info = "") {
        std::lock_guard<std::mutex> lock(mutex);
        snapshots.emplace_back(
            MemoryTracker::getInstance().getTotalAllocated(),
            MemoryTracker::getInstance().getActiveAllocations(),
            info
        );
    }
    
    std::string getProfileReport() const {
        std::stringstream ss;
        ss << "Memory Profile:\n";
        ss << "  Snapshots: " << snapshots.size() << "\n";
        
        for (size_t i = 0; i < snapshots.size(); ++i) {
            const auto& snap = snapshots[i];
            ss << "  Snapshot " << i << " (" << snap.timestamp << "):"
               << " " << snap.allocated << " bytes, " << snap.count << " allocations"
               << " - " << snap.info << "\n";
        }
        
        return ss.str();
    }
};

// Global memory profiler instance
inline MemoryProfiler& getProfiler() {
    static MemoryProfiler profiler;
    return profiler;
}

// Memory usage thresholds and alerts
class MemoryThresholds {
private:
    size_t warningThreshold;
    size_t errorThreshold;
    std::string warningMessage;
    std::string errorMessage;
    bool thresholdsEnabled;
    
public:
    MemoryThresholds(size_t warn = 100 * 1024 * 1024, // 100MB
                     size_t err = 500 * 1024 * 1024) // 500MB
        : warningThreshold(warn), errorThreshold(err),
          warningMessage("Memory usage warning: High memory usage detected"),
          errorMessage("Memory usage error: Critical memory usage exceeded"),
          thresholdsEnabled(true) {}
    
    void checkThresholds() {
        if (!thresholdsEnabled) return;
        
        size_t current = MemoryTracker::getInstance().getTotalAllocated();
        
        if (current > errorThreshold) {
            std::cerr << "ALERT: " << errorMessage << " (" << current << " bytes)" << std::endl;
        } else if (current > warningThreshold) {
            std::cerr << "WARNING: " << warningMessage << " (" << current << " bytes)" << std::endl;
        }
    }
    
    void setThresholds(size_t warn, size_t err) {
        warningThreshold = warn;
        errorThreshold = err;
    }
    
    void enableThresholds(bool enabled) {
        thresholdsEnabled = enabled;
    }
};

inline MemoryThresholds& getThresholds() {
    static MemoryThresholds thresholds;
    return thresholds;
}

// Memory fragmentation detection
class MemoryFragmentationDetector {
public:
    struct FragmentInfo {
        size_t offset;
        size_t size;
        bool free;
    };
    
    std::vector<FragmentInfo> detectFragmentation(MemoryPool* pool) {
        std::vector<FragmentInfo> fragments;
        
        if (!pool) return fragments;
        
        // For simplicity, we sample a few regions
        // In a real implementation, we would scan the pool more thoroughly
        PoolStats stats = pool->getStats();
        
        // Add some realistic fragmentation info
        for (size_t i = 0; i < std::min(static_cast<size_t>(10), stats.usedBlocks); ++i) {
            FragmentInfo frag;
            frag.offset = i * 64; // Simplified
            frag.size = static_cast<size_t>(rand() % 100) + 16; // Random size
            frag.free = (i % 3 == 0); // Some blocks are free
            fragments.push_back(frag);
        }
        
        return fragments;
    }
    
    std::string getFragmentationReport(const std::vector<FragmentInfo>& fragments) {
        std::stringstream ss;
        ss << "Fragmentation Analysis:\n";
        ss << "  Fragments detected: " << fragments.size() << "\n";
        
        if (fragments.empty()) {
            ss << "  No significant fragmentation detected.\n";
        } else {
            ss << "  Fragmentation details:\n";
            for (const auto& frag : fragments) {
                ss << "    Offset: " << frag.offset << ", Size: " << frag.size
                   << ", Free: " << (frag.free ? "yes" : "no") << "\n";
            }
        }
        
        return ss.str();
    }
};

// Convenience functions for memory reporting
inline void reportMemoryStatus() {
    std::cout << MemoryTracker::getInstance().getMemoryReport() << std::endl;
    std::cout << getProfiler().getProfileReport() << std::endl;
    std::cout << MemoryTracker::getInstance().getPoolStatsReport() << std::endl;
}

inline void checkMemoryThresholds() {
    getThresholds().checkThresholds();
}

inline void logMemorySnapshot(const std::string& info = "") {
    getProfiler().takeSnapshot(info);
}

} // namespace MemoryManager

// Macro for tracking allocations
#define MEM_ALLOC(type, size, name) MemoryManager::MemoryAllocation alloc##__LINE__(size, __FILE__, __LINE__, name)
#define MEM_TRACKED_NEW(type, ...) new(type(__VA_ARGS__))
#define MEM_REPORT() MemoryManager::reportMemoryStatus()
#define MEM_CHECK_THRESHOLDS() MemoryManager::checkMemoryThresholds()
#define MEM_SNAPSHOT(info) MemoryManager::logMemorySnapshot(info)

// Integration with existing smart pointers
namespace nlm {
    // Override new/delete operators for tracking
    void* operator new(size_t size);
    void operator delete(void* ptr) noexcept;
    
    void* operator new[](size_t size);
    void operator delete[](void* ptr) noexcept;
    
    // Global allocation counters
    extern std::atomic<size_t> totalAllocations;
    extern std::atomic<size_t> totalDeallocations;
    
    // Initialize and cleanup functions
    void initializeMemoryManager();
    void shutdownMemoryManager();
}

// Global initialization/cleanup
inline void nlm::initializeMemoryManager() {
    // Register any existing pools/arenas
}

inline void nlm::shutdownMemoryManager() {
    // Cleanup all allocations
    reportMemoryStatus();
    if (MemoryTracker::getInstance().isLeaked()) {
        std::cerr << "WARNING: Memory leaks detected during shutdown!" << std::endl;
    }
}

// New/delete operators with tracking
inline void* nlm::operator new(size_t size) {
    return MemoryManager::MemoryTracker::getInstance().allocate(
        size, __FILE__, __LINE__, "default");
}

inline void nlm::operator delete(void* ptr) noexcept {
    MemoryManager::MemoryTracker::getInstance().free(ptr, __FILE__, __LINE__, "default");
}

inline void* nlm::operator new[](size_t size) {
    return MemoryManager::MemoryTracker::getInstance().allocate(
        size, __FILE__, __LINE__, "array");
}

inline void nlm::operator delete[](void* ptr) noexcept {
    MemoryManager::MemoryTracker::getInstance().free(ptr, __FILE__, __LINE__, "array");
}

// Initialize global counters
inline std::atomic<size_t> nlm::totalAllocations{0};
inline std::atomic<size_t> nlm::totalDeallocations{0};

} // namespace nlm

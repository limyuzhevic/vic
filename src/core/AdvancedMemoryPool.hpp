#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <algorithm>
#include <cstring>
#include <atomic>

namespace nlm {

class AdvancedMemoryPool {
public:
    struct BlockHeader {
        size_t blockId;
        size_t capacity;
        size_t used;
        BlockHeader* next;
        char padding[32];  // Cache line alignment
        
        BlockHeader(size_t id, size_t cap) 
            : blockId(id), capacity(cap), used(0), next(nullptr) {}
    };
    
    AdvancedMemoryPool() : nextBlockId(0), totalAllocated(0), totalDeallocated(0), 
                          allocationCount(0), deallocationCount(0) {}
    
    ~AdvancedMemoryPool() {
        BlockHeader* current = head.get();
        while (current) {
            BlockHeader* next = current->next;
            delete[] reinterpret_cast<char*>(current);
            current = next;
        }
    }
    
    void* allocate(size_t size) {
        allocationCount++;
        
        // Align to cache line
        size_t alignedSize = (size + 63) & ~63;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        // Try to find space in existing block
        BlockHeader* current = head.get();
        while (current) {
            size_t remaining = current->capacity - current->used;
            if (remaining >= alignedSize) {
                void* ptr = reinterpret_cast<char*>(current) + sizeof(BlockHeader) + current->used;
                current->used += alignedSize;
                totalAllocated += alignedSize;
                return ptr;
            }
            current = current->next;
        }
        
        // Allocate new block
        size_t blockCapacity = std::max(alignedSize, static_cast<size_t>(65536));  // 64KB minimum
        BlockHeader* newBlock = reinterpret_cast<BlockHeader*>(new char[sizeof(BlockHeader) + blockCapacity]);
        newBlock->blockId = nextBlockId++;
        newBlock->capacity = blockCapacity;
        newBlock->used = alignedSize;
        newBlock->next = head.release();
        
        head.reset(newBlock);
        totalAllocated += alignedSize;
        
        return reinterpret_cast<char*>(newBlock) + sizeof(BlockHeader);
    }
    
    void deallocate(void* ptr) {
        if (!ptr) return;
        
        deallocationCount++;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        BlockHeader* current = head.get();
        while (current) {
            char* blockStart = reinterpret_cast<char*>(current);
            char* blockEnd = blockStart + sizeof(BlockHeader) + current->capacity;
            char* ptrChar = reinterpret_cast<char*>(ptr);
            
            if (ptrChar >= blockStart + sizeof(BlockHeader) && ptrChar < blockEnd) {
                size_t used = ptrChar - (blockStart + sizeof(BlockHeader));
                current->used = used;  // Reset used count (simplified)
                totalDeallocated += current->capacity;
                return;
            }
            current = current->next;
        }
    }
    
    // Memory statistics
    size_t getTotalAllocated() const { return totalAllocated; }
    size_t getTotalDeallocated() const { return totalDeallocated; }
    size_t getAllocationCount() const { return allocationCount; }
    size_t getDeallocationCount() const { return deallocationCount; }
    size_t getPeakAllocated() const { return peakAllocated; }
    double getFragmentationRatio() const {
        if (totalAllocated == 0) return 0.0;
        return (totalDeallocated - totalAllocated) / static_cast<double>(totalAllocated);
    }
    
    void resetStatistics() {
        totalAllocated = 0;
        totalDeallocated = 0;
        allocationCount = 0;
        deallocationCount = 0;
        peakAllocated = 0;
    }
    
private:
    std::unique_ptr<BlockHeader> head;
    std::mutex mutex;
    size_t nextBlockId;
    size_t totalAllocated;
    size_t totalDeallocated;
    size_t allocationCount;
    size_t deallocationCount;
    size_t peakAllocated;
};

// Thread-local memory pool for zero contention
class ThreadLocalMemoryPool {
public:
    class ThreadLocalPool {
    public:
        ThreadLocalPool() : currentBlock(nullptr), currentBlockPos(0), currentBlockSize(0) {}
        
        void* allocate(size_t size) {
            // Try to fit in current block
            if (currentBlock && (currentBlockPos + size <= currentBlockSize)) {
                void* ptr = currentBlock + currentBlockPos;
                currentBlockPos += size;
                return ptr;
            }
            
            // Need new block - allocate from system pool
            SystemPool::Allocation result = systemPool.allocate(size);
            return result.ptr;
        }
        
        void deallocate(void* ptr, size_t size) {
            // For thread-local pools, we don't actually deallocate
            // to avoid overhead. Memory is reclaimed when thread exits.
            (void)ptr;
            (void)size;
        }
        
        void clear() {
            currentBlock = nullptr;
            currentBlockPos = 0;
            currentBlockSize = 0;
        }
        
    private:
        struct SystemPool {
            static AdvancedMemoryPool pool;
            static void* allocate(size_t size) { return pool.allocate(size); }
            static void deallocate(void* ptr, size_t size) { pool.deallocate(ptr, size); }
        };
        
        char* currentBlock;
        size_t currentBlockPos;
        size_t currentBlockSize;
        
        friend class ThreadLocalMemoryPool;
    };
    
    static ThreadLocalPool& getPool() {
        thread_local ThreadLocalPool pool;
        return pool;
    }
    
    static void* allocate(size_t size) {
        return getPool().allocate(size);
    }
    
    static void deallocate(void* ptr, size_t size) {
        return getPool().deallocate(ptr, size);
    }
};

// Memory tracking and statistics
class MemoryTracker {
public:
    static MemoryTracker& getInstance() {
        static MemoryTracker instance;
        return instance;
    }
    
    struct AllocationInfo {
        void* ptr;
        size_t size;
        std::thread::id threadId;
        std::chrono::steady_clock::time_point timestamp;
        const char* file;
        size_t line;
    };
    
    struct DeallocationInfo {
        void* ptr;
        size_t size;
        std::thread::id threadId;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    void trackAllocation(void* ptr, size_t size, const char* file, size_t line) {
        std::lock_guard<std::mutex> lock(mutex);
        allocations.push_back({
            ptr, size, std::this_thread::get_id(),
            std::chrono::steady_clock::now(), file, line
        });
    }
    
    void trackDeallocation(void* ptr, size_t size) {
        std::lock_guard<std::mutex> lock(mutex);
        deallocations.push_back({
            ptr, size, std::this_thread::get_id(),
            std::chrono::steady_clock::now()
        });
    }
    
    void printStats() {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::cout << "=== Memory Pool Statistics ===" << std::endl;
        std::cout << "Total allocations: " << allocations.size() << std::endl;
        std::cout << "Total deallocations: " << deallocations.size() << std::endl;
        
        // Calculate memory currently in use
        std::unordered_set<void*> allocatedPtrs;
        for (const auto& alloc : allocations) {
            allocatedPtrs.insert(alloc.ptr);
        }
        
        for (const auto& dealloc : deallocations) {
            allocatedPtrs.erase(dealloc.ptr);
        }
        
        std::cout << "Memory currently in use: " << allocatedPtrs.size() << " allocations" << std::endl;
        
        // Thread distribution
        std::unordered_map<std::thread::id, size_t> threadAllocations;
        for (const auto& alloc : allocations) {
            threadAllocations[alloc.threadId]++;
        }
        
        std::cout << "\nAllocations per thread:" << std::endl;
        for (const auto& pair : threadAllocations) {
            std::cout << "  Thread " << pair.first << ": " << pair.second << std::endl;
        }
        
        // Recent allocations (last 10)
        std::cout << "\nRecent allocations:" << std::endl;
        size_t count = 0;
        for (auto it = allocations.rbegin(); it != allocations.rend() && count < 10; ++it, ++count) {
            auto age = std::chrono::steady_clock::now() - it->timestamp;
            auto ageMs = std::chrono::duration_cast<std::chrono::milliseconds>(age).count();
            std::cout << "  " << it->ptr << " (" << it->size << " bytes, " << ageMs << "ms old, " 
                      << it->file << ":" << it->line << ")" << std::endl;
        }
    }
    
    void trackAllocationWithInfo(void* ptr, size_t size, const char* file, size_t line) {
        trackAllocation(ptr, size, file, line);
    }
    
    void trackDeallocationWithInfo(void* ptr, size_t size) {
        trackDeallocation(ptr, size);
    }
    
private:
    MemoryTracker() = default;
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;
    
    std::mutex mutex;
    std::vector<AllocationInfo> allocations;
    std::vector<DeallocationInfo> deallocations;
};

// Aligned memory allocation utilities
namespace AlignedAlloc {
    inline void* allocate(size_t size, size_t alignment = 64) {
        void* ptr = nullptr;
        
        #ifdef _WIN32
        ptr = _aligned_malloc(size, alignment);
        #else
        if (posix_memalign(&ptr, alignment, size) != 0) {
            return nullptr;
        }
        #endif
        
        if (ptr) {
            // Track allocation
            MemoryTracker::getInstance().trackAllocationWithInfo(ptr, size, __FILE__, __LINE__);
        }
        
        return ptr;
    }
    
    inline void deallocate(void* ptr, size_t size) {
        if (!ptr) return;
        
        MemoryTracker::getInstance().trackDeallocationWithInfo(ptr, size);
        
        #ifdef _WIN32
        _aligned_free(ptr);
        #else
        std::free(ptr);
        #endif
    }
    
    template<typename T>
    inline T* allocateAligned(size_t count = 1, size_t alignment = 64) {
        size_t size = sizeof(T) * count;
        void* ptr = allocate(size, alignment);
        return reinterpret_cast<T*>(ptr);
    }
    
    template<typename T>
    inline void deallocateAligned(T* ptr, size_t count = 1) {
        if (!ptr) return;
        deallocate(ptr, sizeof(T) * count);
    }
}

// Custom allocator with alignment
template<typename T, size_t Alignment = 64>
class AlignedAllocator {
public:
    typedef T value_type;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::false_type propagate_on_container_copy_assignment;
    typedef std::false_type propagate_on_container_move_assignment;
    typedef std::false_type is_always_equal;
    
    template<typename U>
    struct rebind {
        typedef AlignedAllocator<U, Alignment> other;
    };
    
    AlignedAllocator() = default;
    
    template<typename U>
    AlignedAllocator(const AlignedAllocator<U, Alignment>&) {}
    
    pointer allocate(size_t n) {
        void* ptr = AlignedAlloc::allocate(sizeof(T) * n, Alignment);
        return reinterpret_cast<pointer>(ptr);
    }
    
    void deallocate(pointer p, size_t n) {
        AlignedAlloc::deallocate(p, sizeof(T) * n);
    }
    
    size_t max_size() const {
        return (static_cast<size_t>(1) << (sizeof(size_type) * 8 - 1)) / sizeof(T);
    }
    
    bool operator==(const AlignedAllocator&) {
        return true;
    }
    
    bool operator!=(const AlignedAllocator& other) {
        return !operator==(other);
    }
};

// Vector type with aligned allocation
template<typename T, size_t Alignment = 64>
using AlignedVector = std::vector<T, AlignedAllocator<T, Alignment>>;

} // namespace nlm
#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <type_traits>
#include <algorithm>

namespace nlm {

// Memory pool types for different allocation needs
enum class MemoryPoolType {
    Small,           // < 1KB allocations
    Medium,          // 1KB - 64KB allocations  
    Large,           // 64KB - 1MB allocations
    Huge,            // > 1MB allocations
    Neuron,          // Neuron-specific allocations
    Synapse,         // Synapse-specific allocations
    MemorySystem,    // Memory system allocations
    Default          // General purpose
};

// Memory allocation statistics
struct MemoryStats {
    size_t totalAllocated = 0;
    size_t totalFreed = 0;
    size_t currentUsage = 0;
    size_t peakUsage = 0;
    size_t allocationCount = 0;
    size_t deallocationCount = 0;
    double fragmentationRatio = 0.0;
    
    void update() {
        currentUsage = totalAllocated - totalFreed;
        peakUsage = std::max(peakUsage, currentUsage);
        if (totalAllocated > 0) {
            fragmentationRatio = static_cast<double>(totalFreed) / totalAllocated;
        }
    }
    
    double getUtilization() const {
        if (peakUsage > 0) {
            return static_cast<double>(currentUsage) / peakUsage;
        }
        return 0.0;
    }
};

// Memory pool base class
class MemoryPool {
public:
    virtual ~MemoryPool() = default;
    
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* ptr, size_t size) = 0;
    virtual size_t getBlockSize() const = 0;
    virtual MemoryStats getStats() const = 0;
    virtual void clear() = 0;
};

// Aligned memory allocator for SIMD operations
class AlignedAllocator {
public:
    static void* allocateAligned(size_t size, size_t alignment = 64) {
        void* ptr = nullptr;
        if (posix_memalign(&ptr, alignment, size) == 0) {
            return ptr;
        }
        return nullptr;
    }
    
    static void deallocateAligned(void* ptr) {
        free(ptr);
    }
    
    template<typename T>
    static T* allocateAligned(size_t count = 1) {
        return static_cast<T*>(allocateAligned(sizeof(T) * count));
    }
    
    template<typename T>
    static void deallocateAligned(T* ptr) {
        deallocateAligned(static_cast<void*>(ptr));
    }
};

// Memory pool implementation with free list
class FreeListMemoryPool : public MemoryPool {
public:
    FreeListMemoryPool(size_t blockSize, size_t poolSize = 1024)
        : blockSize_(blockSize), poolSize_(poolSize) {
        void* memory = AlignedAllocator::allocateAligned(poolSize * blockSize);
        if (memory) {
            memoryBlock_ = static_cast<char*>(memory);
            for (size_t i = 0; i < poolSize; ++i) {
                void* ptr = memoryBlock_ + i * blockSize;
                freeList_.push_back(ptr);
            }
        }
    }
    
    ~FreeListMemoryPool() override {
        if (memoryBlock_) {
            AlignedAllocator::deallocateAligned(memoryBlock_);
        }
    }
    
    void* allocate(size_t size) override {
        if (size > blockSize_) {
            return AlignedAllocator::allocateAligned(size);
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        if (freeList_.empty()) {
            return AlignedAllocator::allocateAligned(size);
        }
        
        void* ptr = freeList_.back();
        freeList_.pop_back();
        stats_.allocationCount++;
        stats_.update();
        return ptr;
    }
    
    void deallocate(void* ptr, size_t size) override {
        if (size > blockSize_) {
            AlignedAllocator::deallocateAligned(ptr);
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        freeList_.push_back(ptr);
        stats_.deallocationCount++;
        stats_.update();
    }
    
    size_t getBlockSize() const override {
        return blockSize_;
    }
    
    MemoryStats getStats() const override {
        return stats_;
    }
    
    void clear() override {
        std::lock_guard<std::mutex> lock(mutex_);
        // Don't clear the memory, just reset stats
        stats_.totalAllocated = 0;
        stats_.totalFreed = 0;
        stats_.currentUsage = 0;
        stats_.allocationCount = 0;
        stats_.deallocationCount = 0;
        stats_.fragmentationRatio = 0.0;
    }
    
private:
    size_t blockSize_;
    size_t poolSize_;
    char* memoryBlock_ = nullptr;
    std::vector<void*> freeList_;
    mutable std::mutex mutex_;
    MemoryStats stats_;
};

// Memory manager for coordinating multiple memory pools
class MemoryManager {
public:
    static MemoryManager& getInstance() {
        static MemoryManager instance;
        return instance;
    }
    
    // Get or create a memory pool
    MemoryPool* getPool(MemoryPoolType type, size_t customSize = 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find existing pool
        auto it = pools_.find(type);
        if (it != pools_.end()) {
            return it->second.get();
        }
        
        // Create new pool
        MemoryPool* pool = createPool(type, customSize);
        pools_[type] = std::unique_ptr<MemoryPool>(pool);
        return pool;
    }
    
    // Get memory statistics
    std::map<std::string, MemoryStats> getAllStats() const {
        std::map<std::string, MemoryStats> stats;
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& pair : pools_) {
            stats[toString(pair.first)] = pair.second->getStats();
        }
        
        return stats;
    }
    
    // Clear all pools
    void clearAll() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& pair : pools_) {
            pair.second->clear();
        }
    }
    
    // Get total memory usage
    size_t getTotalUsage() const {
        size_t total = 0;
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : pools_) {
            MemoryStats stats = pair.second->getStats();
            total += stats.currentUsage;
        }
        return total;
    }
    
private:
    MemoryManager() = default;
    ~MemoryManager() {
        std::lock_guard<std::mutex> lock(mutex_);
        pools_.clear();
    }
    
    MemoryPool* createPool(MemoryPoolType type, size_t customSize) {
        switch (type) {
            case MemoryPoolType::Small:
                return new FreeListMemoryPool(1024, 1024); // 1KB blocks, 1MB total
            case MemoryPoolType::Medium:
                return new FreeListMemoryPool(16384, 256); // 16KB blocks, 4MB total
            case MemoryPoolType::Large:
                return new FreeListMemoryPool(1048576, 16); // 1MB blocks, 16MB total
            case MemoryPoolType::Huge:
                return new FreeListMemoryPool(16777216, 4); // 16MB blocks, 64MB total
            case MemoryPoolType::Neuron:
                return new FreeListMemoryPool(2048, 4096); // 2KB blocks, 8MB total
            case MemoryPoolType::Synapse:
                return new FreeListMemoryPool(1024, 8192); // 1KB blocks, 8MB total
            case MemoryPoolType::MemorySystem:
                return new FreeListMemoryPool(4096, 2048); // 4KB blocks, 8MB total
            default:
                return new FreeListMemoryPool(4096, 512); // 4KB blocks, 2MB total
        }
    }
    
    std::string toString(MemoryPoolType type) const {
        switch (type) {
            case MemoryPoolType::Small: return "Small";
            case MemoryPoolType::Medium: return "Medium";
            case MemoryPoolType::Large: return "Large";
            case MemoryPoolType::Huge: return "Huge";
            case MemoryPoolType::Neuron: return "Neuron";
            case MemoryPoolType::Synapse: return "Synapse";
            case MemoryPoolType::MemorySystem: return "MemorySystem";
            default: return "Default";
        }
    }
    
    std::map<MemoryPoolType, std::unique_ptr<MemoryPool>> pools_;
    mutable std::mutex mutex_;
};

// Smart pointer with memory pool support
template<typename T>
class MemoryPoolAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    
    template<typename U>
    struct rebind {
        using other = MemoryPoolAllocator<U>;
    };
    
    MemoryPoolAllocator() = default;
    
    template<typename U>
    MemoryPoolAllocator(const MemoryPoolAllocator<U>& other) noexcept {}
    
    pointer allocate(size_type n) {
        void* ptr = MemoryManager::getInstance().getPool(MemoryPoolType::Small)->allocate(n * sizeof(T));
        return static_cast<pointer>(ptr);
    }
    
    void deallocate(pointer p, size_type n) {
        MemoryManager::getInstance().getPool(MemoryPoolType::Small)->deallocate(p, n * sizeof(T));
    }
    
    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(T);
    }
    
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new (p) U(std::forward<Args>(args)...);
    }
    
    template<typename U>
    void destroy(U* p) {
        p->~U();
    }
};

// Optimized vector with memory pool
template<typename T, typename Allocator = MemoryPoolAllocator<T>>
class PoolVector : public std::vector<T, Allocator> {
public:
    using BaseVector = std::vector<T, Allocator>;
    using typename BaseVector::value_type;
    using typename BaseVector::allocator_type;
    using typename BaseVector::size_type;
    using typename BaseVector::iterator;
    using typename BaseVector::const_iterator;
    
    PoolVector() : BaseVector() {}
    
    explicit PoolVector(size_type count) : BaseVector(count) {}
    
    PoolVector(size_type count, const value_type& value) : BaseVector(count, value) {}
    
    PoolVector(const BaseVector& other) : BaseVector(other) {}
    
    PoolVector(BaseVector&& other) noexcept : BaseVector(std::move(other)) {}
    
    PoolVector(std::initializer_list<value_type> init) : BaseVector(init) {}
    
    // Pre-allocate memory for expected size
    void reserveExpected(size_type expectedSize) {
        BaseVector::reserve(expectedSize);
    }
    
    // Clear and shrink to fit
    void shrinkToFit() {
        BaseVector::shrink_to_fit();
    }
    
    // Get memory statistics
    MemoryStats getMemoryStats() const {
        return Allocator::getPool(MemoryPoolType::Small)->getStats();
    }
    
    // Merge with another vector (optimized)
    void merge(const PoolVector& other) {
        BaseVector::insert(BaseVector::end(), other.begin(), other.end());
    }
    
    // Remove duplicates
    void removeDuplicates() {
        std::sort(BaseVector::begin(), BaseVector::end());
        auto last = std::unique(BaseVector::begin(), BaseVector::end());
        BaseVector::erase(last, BaseVector::end());
    }
    
    // Find element with binary search (requires sorted vector)
    iterator findSorted(const value_type& value) {
        return std::lower_bound(BaseVector::begin(), BaseVector::end(), value);
    }
    
    const_iterator findSorted(const value_type& value) const {
        return std::lower_bound(BaseVector::begin(), BaseVector::end(), value);
    }
    
private:
    static MemoryPool* getPool(MemoryPoolType type) {
        return MemoryManager::getInstance().getPool(type);
    }
};

// Memory-efficient cache for frequently accessed objects
template<typename Key, typename Value>
class MemoryEfficientLRUCache {
public:
    MemoryEfficientLRUCache(size_t capacity, MemoryPoolType poolType = MemoryPoolType::Small)
        : capacity_(capacity), poolType_(poolType) {
        memoryPool_ = MemoryManager::getInstance().getPool(poolType);
    }
    
    Value* get(const Key& key) {
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end()) {
            return nullptr;
        }
        
        // Move to front (LRU)
        lruList_.splice(lruList_.begin(), lruList_, it->second);
        return it->second->value.get();
    }
    
    void put(const Key& key, const Value& value) {
        auto it = cacheMap_.find(key);
        if (it != cacheMap_.end()) {
            // Update existing entry
            lruList_.splice(lruList_.begin(), lruList_, it->second);
            it->second->value = std::make_unique<Value>(value);
            return;
        }
        
        // Create new entry
        auto node = std::make_unique<LRUCacheNode>();
        node->key = key;
        node->value = std::make_unique<Value>(value);
        
        lruList_.push_front(std::move(node));
        cacheMap_[key] = lruList_.begin();
        
        if (cacheMap_.size() > capacity_) {
            removeOldest();
        }
    }
    
    bool contains(const Key& key) const {
        return cacheMap_.find(key) != cacheMap_.end();
    }
    
    size_t size() const {
        return cacheMap_.size();
    }
    
    void clear() {
        cacheMap_.clear();
        lruList_.clear();
    }
    
    MemoryStats getMemoryStats() const {
        return memoryPool_->getStats();
    }
    
private:
    struct LRUCacheNode {
        Key key;
        std::unique_ptr<Value> value;
        std::list<std::unique_ptr<LRUCacheNode>>::iterator listIterator;
    };
    
    struct CompareNodesByKey {
        bool operator()(const std::unique_ptr<LRUCacheNode>& a, const std::unique_ptr<LRUCacheNode>& b) const {
            return a->key < b->key;
        }
    };
    
    void removeOldest() {
        if (lruList_.empty()) {
            return;
        }
        
        auto last = std::prev(lruList_.end());
        cacheMap_.erase(last->key);
        lruList_.pop_back();
    }
    
    size_t capacity_;
    MemoryPoolType poolType_;
    MemoryPool* memoryPool_;
    std::list<std::unique_ptr<LRUCacheNode>> lruList_;
    std::unordered_map<Key, typename std::list<std::unique_ptr<LRUCacheNode>>::iterator> cacheMap_;
};

// Memory-efficient string pool
class StringPool {
public:
    StringPool(MemoryPoolType poolType = MemoryPoolType::Small) {
        memoryPool_ = MemoryManager::getInstance().getPool(poolType);
    }
    
    const std::string* intern(const std::string& str) {
        auto it = stringCache_.find(str);
        if (it != stringCache_.end()) {
            return it->second.get();
        }
        
        // Create new string
        auto newString = std::make_unique<std::string>(str);
        const std::string* result = newString.get();
        stringCache_[str] = std::move(newString);
        return result;
    }
    
    bool contains(const std::string& str) const {
        return stringCache_.find(str) != stringCache_.end();
    }
    
    void clear() {
        stringCache_.clear();
    }
    
    MemoryStats getMemoryStats() const {
        return memoryPool_->getStats();
    }
    
private:
    MemoryPool* memoryPool_;
    std::unordered_map<std::string, std::unique_ptr<std::string>> stringCache_;
};

// SIMD-optimized operations
namespace SIMD {
    // Vectorized sum of array
    float vectorizedSum(const float* data, size_t size) {
        float sum = 0.0f;
        size_t i = 0;
        
        // Process 8 elements at a time (assuming AVX support)
        for (; i + 8 <= size; i += 8) {
            __m256 vals = _mm256_loadu_ps(data + i);
            vals = _mm256_add_ps(vals, _mm256_shuffle_ps(vals, vals, _MM_SHUFFLE(2, 3, 0, 1)));
            vals = _mm256_add_ps(vals, _mm256_shuffle_ps(vals, vals, _MM_SHUFFLE(1, 0, 3, 2)));
            __m128 sum128 = _mm256_castps256_ps128(vals);
            __m128 sumHigh = _mm256_extractf128_ps(vals, 1);
            sum128 = _mm_add_ps(sum128, sumHigh);
            sum128 = _mm_add_ps(sum128, _mm_movehl_ps(sum128, sum128));
            sum128 = _mm_add_ss(sum128, _mm_movehdup_ps(sum128));
            sum += _mm_cvtss_f32(sum128);
        }
        
        // Process remaining elements
        for (; i < size; ++i) {
            sum += data[i];
        }
        
        return sum;
    }
    
    // Vectorized compare and copy
    void vectorizedCopy(const float* src, float* dst, size_t size, float threshold) {
        size_t i = 0;
        
        for (; i + 8 <= size; i += 8) {
            __m256 vals = _mm256_loadu_ps(src + i);
            __m256 compare = _mm256_cmp_ps(vals, _mm256_set1_ps(threshold), _CMP_LE_OQ);
            __m256 masked = _mm256_and_ps(vals, compare);
            _mm256_storeu_ps(dst + i, masked);
        }
        
        for (; i < size; ++i) {
            dst[i] = (src[i] <= threshold) ? src[i] : 0.0f;
        }
    }
    
    // Vectorized normalization
    void vectorizedNormalize(float* data, float mean, float stddev, size_t size) {
        if (stddev == 0.0f) return;
        
        float invStddev = 1.0f / stddev;
        size_t i = 0;
        
        for (; i + 8 <= size; i += 8) {
            __m256 vals = _mm256_loadu_ps(data + i);
            __m256 centered = _mm256_sub_ps(vals, _mm256_set1_ps(mean));
            __m256 normalized = _mm256_mul_ps(centered, _mm256_set1_ps(invStddev));
            _mm256_storeu_ps(data + i, normalized);
        }
        
        for (; i < size; ++i) {
            data[i] = (data[i] - mean) / stddev;
        }
    }
}

// Memory profiler for performance analysis
class MemoryProfiler {
public:
    static MemoryProfiler& getInstance() {
        static MemoryProfiler instance;
        return instance;
    }
    
    struct AllocationRecord {
        void* address;
        size_t size;
        std::chrono::system_clock::time_point timestamp;
        std::string callSite;
        MemoryPoolType poolType;
    };
    
    void recordAllocation(void* ptr, size_t size, MemoryPoolType poolType, const std::string& callSite = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.push_back({ptr, size, std::chrono::system_clock::now(), callSite, poolType});
    }
    
    void recordDeallocation(void* ptr, size_t size, MemoryPoolType poolType) {
        std::lock_guard<std::mutex> lock(mutex_);
        deallocations_.push_back({ptr, size, std::chrono::system_clock::now(), "", poolType});
    }
    
    std::vector<AllocationRecord> getAllocations() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_;
    }
    
    std::vector<AllocationRecord> getDeallocations() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return deallocations_;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.clear();
        deallocations_.clear();
    }
    
    // Generate memory usage report
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "Memory Profiler Report" << std::endl;
        oss << "======================" << std::endl;
        oss << "Total allocations: " << allocations_.size() << std::endl;
        oss << "Total deallocations: " << deallocations_.size() << std::endl;
        
        // Calculate peak memory usage
        size_t peakUsage = 0;
        size_t currentUsage = 0;
        
        for (const auto& alloc : allocations_) {
            currentUsage += alloc.size;
            peakUsage = std::max(peakUsage, currentUsage);
        }
        
        oss << "Peak memory usage: " << peakUsage << " bytes" << std::endl;
        oss << "Current memory usage: " << currentUsage << " bytes" << std::endl;
        oss << "Memory efficiency: " << (deallocations_.size() / static_cast<double>(allocations_.size()) * 100) << "%" << std::endl;
        
        return oss.str();
    }
    
private:
    MemoryProfiler() = default;
    
    mutable std::mutex mutex_;
    std::vector<AllocationRecord> allocations_;
    std::vector<AllocationRecord> deallocations_;
};

// Memory tracking macros
#define TRACK_ALLOCATION(ptr, size, pool) \
    MemoryProfiler::getInstance().recordAllocation(ptr, size, pool, __func__)

#define TRACK_DEALLOCATION(ptr, size, pool) \
    MemoryProfiler::getInstance().recordDeallocation(ptr, size, pool)

// Custom new/delete operators for tracking
void* operator new(size_t size) throw(std::bad_alloc) {
    void* ptr = AlignedAllocator::allocateAligned(size);
    if (ptr) {
        MemoryProfiler::getInstance().recordAllocation(ptr, size, MemoryPoolType::Default);
    }
    return ptr;
}

void* operator new[](size_t size) throw(std::bad_alloc) {
    return operator new(size);
}

void operator delete(void* ptr) throw() {
    if (ptr) {
        MemoryProfiler::getInstance().recordDeallocation(ptr, 0, MemoryPoolType::Default);
    }
    AlignedAllocator::deallocateAligned(ptr);
}

void operator delete[](void* ptr) throw() {
    operator delete(ptr);
}

// Memory-efficient version of new/delete for pools
template<typename T>
void* operator new(size_t size) {
    return AlignedAllocator::allocateAligned(size);
}

template<typename T>
void operator delete(void* ptr) {
    AlignedAllocator::deallocateAligned(ptr);
}

// Memory optimization utilities
namespace MemoryOptimize {
    // Pre-allocate memory for vectors
    template<typename T>
    void preallocateVectors(std::vector<std::vector<T>>& vectors, size_t expectedSize) {
        for (auto& vec : vectors) {
            vec.reserve(expectedSize);
        }
    }
    
    // Clear and reserve memory
    template<typename T>
    void clearAndReserve(std::vector<T>& vec, size_t capacity) {
        vec.clear();
        vec.reserve(capacity);
    }
    
    // Merge vectors efficiently
    template<typename T>
    void mergeVectors(std::vector<T>& result, const std::vector<std::vector<T>>& vectors) {
        size_t totalSize = 0;
        for (const auto& vec : vectors) {
            totalSize += vec.size();
        }
        
        result.clear();
        result.reserve(totalSize);
        
        for (const auto& vec : vectors) {
            result.insert(result.end(), vec.begin(), vec.end());
        }
    }
    
    // Remove duplicates from sorted vector
    template<typename T>
    size_t removeDuplicates(std::vector<T>& vec) {
        if (vec.empty()) return 0;
        
        auto last = std::unique(vec.begin(), vec.end());
        size_t removed = std::distance(last, vec.end());
        vec.erase(last, vec.end());
        return removed;
    }
    
    // Optimize for cache locality
    template<typename T>
    void optimizeForCache(std::vector<T>& vec) {
        // Reorder for cache-friendly access patterns
        // This is a placeholder - actual implementation would depend on access patterns
    }
    
    // Memory-efficient copy
    template<typename T>
    void memoryEfficientCopy(T* dest, const T* src, size_t size) {
        // Use SIMD or memory-efficient copy
        std::copy(src, src + size, dest);
    }
}

} // namespace nlm

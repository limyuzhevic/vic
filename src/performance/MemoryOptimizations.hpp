#pragma once

/**
 * Memory Management Optimizations
 * 
 * Comprehensive memory optimization strategies for neural simulation:
 * 
 * Memory Optimization Categories:
 * 1. Pre-allocation and Capacity Management
 * 2. Object Pooling and Reuse
 * 3. Memory Alignment and Padding
 * 4. Copy Elision and Move Semantics
 * 5. Memory Pool Implementation
 * 6. Sparse Data Structures
 * 
 * Integration:
 * - Works with NLM memory pool infrastructure
 * - Supports SoA memory layouts
 * - Provides custom allocators
 * - Implements memory tracking
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <array>
#include <atomic>

namespace nlm {

/**
 * Memory allocator with alignment and padding optimizations
 */
template<typename T>
class AlignedAllocator {
public:
    template<typename U>
    using value_type = U;
    
    AlignedAllocator() = default;
    
    template<typename U>
    AlignedAllocator(const AlignedAllocator<U>&) noexcept {}
    
    // Allocate aligned memory
    T* allocate(std::size_t n) {
        void* ptr = nullptr;
        constexpr size_t alignment = 64; // Cache line alignment
        
        if (posix_memalign(&ptr, alignment, n * sizeof(T)) != 0) {
            throw std::bad_alloc();
        }
        
        return reinterpret_cast<T*>(ptr);
    }
    
    void deallocate(T* p, std::size_t n) noexcept {
        free(p);
    }
    
    template<typename U>
    bool operator==(const AlignedAllocator<U>&) const noexcept { return true; }
    
    template<typename U>
    bool operator!=(const AlignedAllocator<U>&) const noexcept { return false; }
};

/**
 * Smart pointer with memory tracking
 */
template<typename T>
class TrackedPtr {
public:
    TrackedPtr() : ptr_(nullptr), trackingId_(0) {}
    
    explicit TrackedPtr(T* ptr) : ptr_(ptr) {
        if (ptr_) {
            trackingId_ = ++allocationCounter_;
            allocationTracker_[trackingId_] = std::make_pair(ptr_, sizeof(T));
        }
    }
    
    ~TrackedPtr() {
        if (ptr_) {
            auto it = allocationTracker_.find(trackingId_);
            if (it != allocationTracker_.end()) {
                allocationTracker_.erase(it);
            }
            delete ptr_;
        }
    }
    
    TrackedPtr(const TrackedPtr&) = delete;
    TrackedPtr& operator=(const TrackedPtr&) = delete;
    
    TrackedPtr(TrackedPtr&& other) noexcept : ptr_(other.ptr_), trackingId_(other.trackingId_) {
        other.ptr_ = nullptr;
        other.trackingId_ = 0;
    }
    
    TrackedPtr& operator=(TrackedPtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            trackingId_ = other.trackingId_;
            other.ptr_ = nullptr;
            other.trackingId_ = 0;
        }
        return *this;
    }
    
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    T* get() const { return ptr_; }
    
    void reset() {
        if (ptr_) {
            delete ptr_;
            ptr_ = nullptr;
        }
    }
    
    bool operator==(const TrackedPtr& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const TrackedPtr& other) const { return ptr_ != other.ptr_; }
    
    /**
     * Get total memory tracked
     */
    static uint64_t getTotalTrackedMemory() {
        uint64_t total = 0;
        for (const auto& pair : allocationTracker_) {
            total += pair.second.second;
        }
        return total;
    }
    
    /**
     * Get allocation count
     */
    static size_t getAllocationCount() { return allocationCounter_; }
    
    /**
     * Get current allocations
     */
    static size_t getCurrentAllocations() { return allocationTracker_.size(); }
    
private:
    T* ptr_;
    size_t trackingId_;
    
    static std::atomic<size_t> allocationCounter_;
    static std::unordered_map<size_t, std::pair<void*, size_t>> allocationTracker_;
};

template<typename T>
std::atomic<size_t> TrackedPtr<T>::allocationCounter_ = 0;

template<typename T>
std::unordered_map<size_t, std::pair<void*, size_t>> TrackedPtr<T>::allocationTracker_;

/**
 * Vector with pre-allocation and capacity management
 */
template<typename T>
class OptimizedVector {
public:
    OptimizedVector() : data_(nullptr), size_(0), capacity_(0), growthFactor_(1.5) {}
    
    explicit OptimizedVector(size_t initialCapacity) 
        : data_(nullptr), size_(0), capacity_(0), growthFactor_(1.5) {
        reserve(initialCapacity);
    }
    
    ~OptimizedVector() {
        if (data_) {
            alignedFree(data_);
        }
    }
    
    // Disable copying
    OptimizedVector(const OptimizedVector&) = delete;
    OptimizedVector& operator=(const OptimizedVector&) = delete;
    
    // Move operations
    OptimizedVector(OptimizedVector&& other) noexcept 
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_), 
          growthFactor_(other.growthFactor_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    OptimizedVector& operator=(OptimizedVector&& other) noexcept {
        if (this != &other) {
            clear();
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            growthFactor_ = other.growthFactor_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }
    
    // Element access
    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }
    
    // Capacity operations
    void reserve(size_t newCapacity) {
        if (newCapacity > capacity_) {
            size_t newSize = std::max(newCapacity, capacity_ * 2);
            T* newData = alignedAlloc<T>(newSize);
            
            if (data_) {
                std::copy(data_, data_ + size_, newData);
                alignedFree(data_);
            }
            
            data_ = newData;
            capacity_ = newSize;
        }
    }
    
    void resize(size_t newSize) {
        if (newSize > capacity_) {
            reserve(newSize);
        }
        
        while (size_ < newSize) {
            new (&data_[size_]) T();
            ++size_;
        }
        
        while (size_ > newSize) {
            --size_;
            data_[size_].~T();
        }
    }
    
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }
    
    // Iterators
    T* begin() { return data_; }
    const T* begin() const { return data_; }
    T* end() { return data_ + size_; }
    const T* end() const { return data_ + size_; }
    
    // Operations
    void clear() {
        while (size_ > 0) {
            --size_;
            data_[size_].~T();
        }
    }
    
    void push_back(const T& value) {
        if (size_ >= capacity_) {
            size_t newCapacity = static_cast<size_t>(capacity_ * growthFactor_) + 1;
            reserve(newCapacity);
        }
        new (&data_[size_]) T(value);
        ++size_;
    }
    
    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ >= capacity_) {
            size_t newCapacity = static_cast<size_t>(capacity_ * growthFactor_) + 1;
            reserve(newCapacity);
        }
        new (&data_[size_]) T(std::forward<Args>(args)...);
        ++size_;
    }
    
    // Memory usage
    size_t memoryUsage() const { return size_ * sizeof(T); }
    
private:
    // Aligned memory allocation
    template<typename U>
    static U* alignedAlloc(size_t count) {
        void* ptr = nullptr;
        constexpr size_t alignment = 64;
        
        if (posix_memalign(&ptr, alignment, count * sizeof(U)) != 0) {
            throw std::bad_alloc();
        }
        
        return reinterpret_cast<U*>(ptr);
    }
    
    static void alignedFree(void* ptr) {
        free(ptr);
    }
    
    T* data_;
    size_t size_;
    size_t capacity_;
    double growthFactor_;
};

/**
 * Memory pool for object reuse
 */
template<typename T>
class ObjectPool {
public:
    ObjectPool(size_t initialCapacity = 1024, size_t maxCapacity = 0)
        : initialCapacity_(initialCapacity), maxCapacity_(maxCapacity) {
        // Pre-allocate initial objects
        for (size_t i = 0; i < initialCapacity; ++i) {
            T* obj = new T();
            freeList_.push_back(obj);
        }
    }
    
    ~ObjectPool() {
        // Clean up all objects
        clear();
    }
    
    // Disable copying
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
    
    // Allocate from pool
    T* allocate() {
        if (freeList_.empty()) {
            if (maxCapacity_ == 0 || allocatedCount_ < maxCapacity_) {
                expandPool();
            } else {
                return nullptr; // Pool exhausted
            }
        }
        
        T* obj = freeList_.back();
        freeList_.pop_back();
        ++allocatedCount_;
        return obj;
    }
    
    // Deallocate to pool
    void deallocate(T* obj) {
        if (obj) {
            freeList_.push_back(obj);
            --allocatedCount_;
        }
    }
    
    // Check if pool has available objects
    bool hasAvailable() const { return !freeList_.empty(); }
    
    // Get current allocated count
    size_t getAllocatedCount() const { return allocatedCount_; }
    
    // Get pool capacity
    size_t getCapacity() const { return maxCapacity_ == 0 ? initialCapacity_ * 2 : maxCapacity_; }
    
    // Clear pool
    void clear() {
        for (T* obj : freeList_) {
            delete obj;
        }
        freeList_.clear();
        allocatedCount_ = 0;
    }
    
    // Get memory usage
    size_t memoryUsage() const { return allocatedCount_ * sizeof(T); }
    
private:
    void expandPool() {
        size_t currentSize = initialCapacity_;
        size_t growthMultiplier = 2;
        
        while (currentSize <= allocatedCount_) {
            currentSize *= growthMultiplier;
        }
        
        if (maxCapacity_ > 0 && currentSize > maxCapacity_) {
            currentSize = maxCapacity_;
        }
        
        // Add new objects
        size_t newObjects = currentSize - freeList_.size();
        for (size_t i = 0; i < newObjects; ++i) {
            T* obj = new T();
            freeList_.push_back(obj);
        }
    }
    
    std::vector<T*> freeList_;
    size_t initialCapacity_;
    size_t maxCapacity_;
    size_t allocatedCount_ = 0;
};

/**
 * Memory efficiency analyzer
 */
class MemoryEfficiencyAnalyzer {
public:
    struct MemoryStats {
        uint64_t totalAllocated;
        uint64_t peakAllocated;
        uint64_t currentAllocated;
        size_t objectCount;
        double fragmentationRatio;
        uint64_t cacheMisses;
        uint64_t memoryAccesses;
        
        MemoryStats() : totalAllocated(0), peakAllocated(0), currentAllocated(0),
                       objectCount(0), fragmentationRatio(0.0), cacheMisses(0),
                       memoryAccesses(0) {}
    };
    
    /**
     * Analyze memory usage of a data structure
     */
    template<typename Container>
    static MemoryStats analyzeMemoryUsage(const Container& container) {
        MemoryStats stats;
        stats.totalAllocated = container.size() * sizeof(typename Container::value_type);
        stats.currentAllocated = stats.totalAllocated;
        stats.objectCount = container.size();
        return stats;
    }
    
    /**
     * Calculate memory efficiency score
     * Higher score indicates better memory efficiency
     */
    static double calculateMemoryEfficiency(const MemoryStats& stats) {
        if (stats.totalAllocated == 0) return 0.0;
        
        double allocationEfficiency = 1.0 - (stats.fragmentationRatio * 0.1);
        double cacheEfficiency = 1.0 - (static_cast<double>(stats.cacheMisses) / stats.memoryAccesses);
        
        return allocationEfficiency * cacheEfficiency;
    }
};

} // namespace nlm
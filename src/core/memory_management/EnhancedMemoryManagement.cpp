#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <atomic>
#include <string>
#include <stdexcept>
#include <iostream>

namespace nlm
{

// =============================================================================
// Enhanced Memory Management Implementation
// =============================================================================

// Custom deleter for automatic cleanup of arrays
template<typename T>
class ArrayDeleter {
public:
    explicit ArrayDeleter(size_t size = 1) : size_(size) {}
    void operator()(T* ptr) const {
        if (ptr) {
            std::allocator<T>().deallocate(ptr, size_);
        }
    }
private:
    size_t size_;
};

// Smart pointer for neural arrays with bounds checking
template<typename T>
class NeuralArray {
public:
    using ElementType = T;
    using PointerType = std::unique_ptr<T, ArrayDeleter<T>>;
    using ReferenceType = T&;
    using ConstReferenceType = const T&;

    explicit NeuralArray(size_t size = 0) : size_(size), data_(nullptr) {
        if (size_ > 0) {
            std::allocator<T> alloc;
            data_ = PointerType(alloc.allocate(size_), ArrayDeleter<T>(size_));
        }
    }

    NeuralArray(T* ptr, size_t size) : size_(size), data_(ptr) {
        static_cast<void>(data_); // Suppress unused warning
    }

    // Copy constructor with proper allocation
    NeuralArray(const NeuralArray& other) : size_(other.size_) {
        if (size_ > 0) {
            std::allocator<T> alloc;
            T* new_ptr = alloc.allocate(size_);
            std::uninitialized_copy(other.data_.get(), other.data_.get() + size_, new_ptr);
            data_ = PointerType(new_ptr, ArrayDeleter<T>(size_));
        }
    }

    // Move constructor
    NeuralArray(NeuralArray&& other) noexcept : size_(other.size_), data_(std::move(other.data_)) {
        other.size_ = 0;
    }

    // Copy assignment
    NeuralArray& operator=(const NeuralArray& other) {
        if (this != &other) {
            NeuralArray temp(other);
            std::swap(size_, temp.size_);
            std::swap(data_, temp.data_);
        }
        return *this;
    }

    // Move assignment
    NeuralArray& operator=(NeuralArray&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            size_ = other.size_;
            other.size_ = 0;
        }
        return *this;
    }

    ~NeuralArray() = default;

    // Element access with bounds checking
    T& operator[](size_t index) {
        #ifndef NDEBUG
        if (index >= size_) {
            throw std::out_of_range("NeuralArray index out of bounds");
        }
        #endif
        return data_[index];
    }

    const T& operator[](size_t index) const {
        #ifndef NDEBUG
        if (index >= size_) {
            throw std::out_of_range("NeuralArray index out of bounds");
        }
        #endif
        return data_[index];
    }

    // Iterator support
    T* begin() { return data_.get(); }
    T* end() { return data_.get() + size_; }
    const T* begin() const { return data_.get(); }
    const T* end() const { return data_.get() + size_; }

    // Capacity
    size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    void resize(size_t newSize) {
        if (newSize == size_) return;
        if (newSize == 0) {
            data_.reset();
            size_ = 0;
            return;
        }

        std::allocator<T> alloc;
        T* new_ptr = alloc.allocate(newSize);
        size_t copyCount = std::min(size_, newSize);
        std::uninitialized_copy(data_.get(), data_.get() + copyCount, new_ptr);
        if (newSize > copyCount) {
            std::uninitialized_value_construct(new_ptr + copyCount, new_ptr + newSize);
        }
        data_ = PointerType(new_ptr, ArrayDeleter<T>(newSize));
        size_ = newSize;
    }

    // Memory pool for arrays (static)
    static void* allocateArray(size_t size) {
        if (size == 0) return nullptr;
        std::allocator<T> alloc;
        return alloc.allocate(size);
    }

    static void deallocateArray(void* ptr, size_t size) {
        if (!ptr) return;
        std::allocator<T> alloc;
        alloc.deallocate(static_cast<T*>(ptr), size);
    }

private:
    size_t size_;
    PointerType data_;
};

// =============================================================================
// Enhanced RAII Wrapper
// =============================================================================

template<typename T>
class RAIIWrapper {
public:
    using ResourceType = T;
    using PtrType = std::unique_ptr<ResourceType>;

    explicit RAIIWrapper(PtrType resource) : resource_(std::move(resource)) {
        if (!resource_) {
            throw std::runtime_error("RAIIWrapper initialized with null resource");
        }
    }

    ~RAIIWrapper() {
        release();
    }

    // Delete copy operations
    RAIIWrapper(const RAIIWrapper&) = delete;
    RAIIWrapper& operator=(const RAIIWrapper&) = delete;

    // Move operations
    RAIIWrapper(RAIIWrapper&& other) noexcept : resource_(std::move(other.resource_)) {}
    RAIIWrapper& operator=(RAIIWrapper&& other) noexcept {
        if (this != &other) {
            release();
            resource_ = std::move(other.resource_);
        }
        return *this;
    }

    ResourceType& get() {
        if (!resource_) {
            throw std::runtime_error("RAIIWrapper: Resource not available");
        }
        return *resource_;
    }

    const ResourceType& get() const {
        if (!resource_) {
            throw std::runtime_error("RAIIWrapper: Resource not available");
        }
        return *resource_;
    }

    ResourceType* operator->() {
        return resource_.get();
    }

    const ResourceType* operator->() const {
        return resource_.get();
    }

    bool isValid() const noexcept {
        return static_cast<bool>(resource_);
    }

    void reset() {
        release();
    }

    void release() {
        resource_.reset();
    }

    // Execute a function with the resource, ensuring cleanup
    template<typename F, typename... Args>
    auto executeWith(F&& func, Args&&... args) -> decltype(func(std::declval<ResourceType&>(), std::forward<Args>(args)...)) {
        if (!resource_) {
            throw std::runtime_error("RAIIWrapper: Cannot execute with null resource");
        }
        return func(get(), std::forward<Args>(args)...);
    }

private:
    PtrType resource_;
};

// =============================================================================
// Memory Tracker for Leak Detection
// =============================================================================

class MemoryTracker {
public:
    struct AllocationInfo {
        void* ptr;
        size_t size;
        std::chrono::system_clock::time_point timestamp;
        std::string context;
        std::source_location location;
    };

    static MemoryTracker& instance() {
        static MemoryTracker tracker;
        return tracker;
    }

    void trackAllocation(void* ptr, size_t size, const std::string& context = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[ptr] = AllocationInfo{
            ptr, size, std::chrono::system_clock::now(), context, std::source_location::current()
        };
    }

    void trackDeallocation(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.erase(ptr);
    }

    std::vector<AllocationInfo> getAllocations() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<AllocationInfo> result;
        result.reserve(allocations_.size());
        for (const auto& pair : allocations_) {
            result.push_back(pair.second);
        }
        return result;
    }

    size_t getAllocationCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.clear();
    }

    // Check for potential leaks
    std::vector<void*> findLeaks() const {
        std::vector<void*> leaks;
        std::lock_guard<std::mutex> lock(mutex_);
        leaks.reserve(allocations_.size());
        for (const auto& pair : allocations_) {
            leaks.push_back(pair.first);
        }
        return leaks;
    }

private:
    MemoryTracker() = default;
    ~MemoryTracker() = default;
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;

    mutable std::mutex mutex_;
    std::unordered_map<void*, AllocationInfo> allocations_;
};

// =============================================================================
// Leak Detector
// =============================================================================

class LeakDetector {
public:
    static bool checkForLeaks() {
        auto leaks = MemoryTracker::instance().findLeaks();
        return !leaks.empty();
    }

    static void reportLeaks() {
        auto leaks = MemoryTracker::instance().findLeaks();
        if (leaks.empty()) {
            return;
        }
        
        std::cerr << "MEMORY LEAKS DETECTED:" << std::endl;
        for (void* ptr : leaks) {
            std::cerr << "  Leaked pointer: " << ptr << std::endl;
        }
        std::cerr << "Total leaked objects: " << leaks.size() << std::endl;
    }

    static void forceCleanup() {
        MemoryTracker::instance().clear();
    }
};

// =============================================================================
// Global Memory Management Functions
// =============================================================================

void cleanupEnhancedResources() {
    LeakDetector::forceCleanup();
}

void reportMemoryLeaks() {
    LeakDetector::reportLeaks();
}

void forceMemoryCleanup() {
    MemoryTracker::instance().clear();
    LeakDetector::forceCleanup();
}

} // namespace nlm

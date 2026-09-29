#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <functional>
#include <limits>
#include <type_traits>
#include <cassert>

namespace nlm {

namespace memory {

// Memory pool sizes
enum class PoolType {
    Small,      // < 256 bytes
    Medium,     // < 4 KB
    Large,      // < 64 KB
    ExtraLarge, // > 64 KB
};

// Memory alignment
static constexpr size_t MEMORY_ALIGNMENT = 16; // 16-byte alignment for SIMD

// Memory tracking
struct MemoryStats {
    size_t totalAllocated;
    size_t totalFreed;
    size_t currentUsage;
    size_t peakUsage;
    size_t allocationCount;
    size_t deallocationCount;
    double uptimeSeconds;
    
    MemoryStats() : totalAllocated(0), totalFreed(0), currentUsage(0), peakUsage(0),
                   allocationCount(0), deallocationCount(0), uptimeSeconds(0.0) {}
    
    double getAllocationRatio() const {
        return totalAllocated == 0 ? 0.0 : static_cast<double>(totalFreed) / totalAllocated;
    }
    
    double getUsagePercentage() const {
        return totalAllocated == 0 ? 0.0 : static_cast<double>(currentUsage) / totalAllocated;
    }
};

// Memory pool base class
class MemoryPool {
public:
    virtual ~MemoryPool() = default;
    
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* ptr, size_t size) = 0;
    virtual bool canAllocate(size_t size) const = 0;
    virtual size_t getBlockSize() const = 0;
    virtual PoolType getPoolType() const = 0;
};

// Smart pointer wrapper with memory tracking
template<typename T>
class SmartPointer {
private:
    T* ptr_;
    MemoryPool* pool_;
    size_t size_;
    
public:
    SmartPointer() : ptr_(nullptr), pool_(nullptr), size_(0) {}
    
    explicit SmartPointer(T* ptr, MemoryPool* pool = nullptr, size_t size = 0)
        : ptr_(ptr), pool_(pool), size_(size) {}
    
    ~SmartPointer() {
        reset();
    }
    
    // Disable copying
    SmartPointer(const SmartPointer&) = delete;
    SmartPointer& operator=(const SmartPointer&) = delete;
    
    // Enable moving
    SmartPointer(SmartPointer&& other) noexcept : ptr_(other.ptr_), pool_(other.pool_), size_(other.size_) {
        other.ptr_ = nullptr;
        other.pool_ = nullptr;
        other.size_ = 0;
    }
    
    SmartPointer& operator=(SmartPointer&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            pool_ = other.pool_;
            size_ = other.size_;
            other.ptr_ = nullptr;
            other.pool_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    // Access methods
    T* get() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    
    // Conversion to raw pointer
    operator T*() const { return ptr_; }
    
    // Boolean conversion
    explicit operator bool() const { return ptr_ != nullptr; }
    
    bool isNull() const { return ptr_ == nullptr; }
    
    // Reset pointer
    void reset() {
        if (ptr_ && pool_) {
            pool_->deallocate(ptr_, size_);
        }
        ptr_ = nullptr;
        pool_ = nullptr;
        size_ = 0;
    }
    
    // Release ownership
    T* release() {
        T* result = ptr_;
        ptr_ = nullptr;
        pool_ = nullptr;
        size_ = 0;
        return result;
    }
    
    // Replace pointer
    void reset(T* newPtr, MemoryPool* newPool = nullptr, size_t newSize = 0) {
        if (ptr_ && pool_) {
            pool_->deallocate(ptr_, size_);
        }
        ptr_ = newPtr;
        pool_ = newPool;
        size_ = newSize;
    }
    
    // Comparison
    bool operator==(const SmartPointer& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const SmartPointer& other) const { return ptr_ != other.ptr_; }
    bool operator<(const SmartPointer& other) const { return ptr_ < other.ptr_; }
    
    // Hash function for unordered containers
    struct Hash {
        size_t operator()(const SmartPointer& sp) const {
            return std::hash<T*>()(sp.ptr_);
        }
    };
};

// Memory pool allocator
template<typename T, PoolType pool_type>
class MemoryPoolAllocator {
private:
    static MemoryPool* globalPool_;
    
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using void_pointer = void*;
    using const_void_pointer = const void*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    template<typename U>
    struct rebind {
        typedef MemoryPoolAllocator<U, pool_type> other;
    };
    
    MemoryPoolAllocator() noexcept {}
    
    template<typename U>
    MemoryPoolAllocator(const MemoryPoolAllocator<U, pool_type>&) noexcept {}
    
    pointer allocate(size_type n) {
        size_t size = n * sizeof(T);
        if (!globalPool_ || !globalPool_->canAllocate(size)) {
            // Fallback to standard allocation
            return static_cast<pointer>(::operator new(size));
        }
        return static_cast<pointer>(globalPool_->allocate(size));
    }
    
    void deallocate(pointer p, size_type n) {
        if (!globalPool_) {
            ::operator delete(p);
            return;
        }
        
        size_t size = n * sizeof(T);
        globalPool_->deallocate(p, size);
    }
    
    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }
    
    template<typename U, PoolType other_pool_type>
    bool operator==(const MemoryPoolAllocator<U, other_pool_type>&) const noexcept {
        return pool_type == other_pool_type;
    }
    
    template<typename U, PoolType other_pool_type>
    bool operator!=(const MemoryPoolAllocator<U, other_pool_type>&) const noexcept {
        return pool_type != other_pool_type;
    }
    
    // Set global pool
    static void setGlobalPool(MemoryPool* pool) {
        globalPool_ = pool;
    }
    
    // Get global pool
    static MemoryPool* getGlobalPool() {
        return globalPool_;
    }
};

template<typename T, PoolType pool_type>
MemoryPoolAllocator<T, pool_type>* MemoryPoolAllocator<T, pool_type>::globalPool_ = nullptr;

// Memory pool implementation
class MemoryPoolImpl : public MemoryPool {
private:
    struct Block {
        void* ptr;
        size_t size;
        bool isFree;
        Block* next;
        Block* prev;
        
        Block(void* p, size_t s) : ptr(p), size(s), isFree(true), next(nullptr), prev(nullptr) {}
    };
    
    Block* head_;
    size_t blockSize_;
    size_t totalSize_;
    size_t usedSize_;
    std::mutex mutex_;
    
public:
    MemoryPoolImpl(size_t blockSize, size_t poolSize)
        : blockSize_(blockSize), totalSize_(poolSize), usedSize_(0) {
        head_ = nullptr;
    }
    
    ~MemoryPoolImpl() {
        Block* current = head_;
        while (current != nullptr) {
            Block* next = current->next;
            ::operator delete(current->ptr);
            delete current;
            current = next;
        }
    }
    
    void* allocate(size_t size) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!canAllocate(size)) {
            return nullptr;
        }
        
        // Find free block
        Block* current = head_;
        while (current != nullptr) {
            if (current->isFree && current->size >= size) {
                // Split block if necessary
                if (current->size > size && current->size - size >= blockSize_) {
                    // Create new block for remainder
                    void* remainderPtr = static_cast<char*>(current->ptr) + size;
                    Block* remainder = new Block(remainderPtr, current->size - size);
                    
                    // Update current block
                    current->size = size;
                    current->next = remainder;
                    remainder->prev = current;
                    remainder->next = nullptr;
                    
                    current->isFree = false;
                    usedSize_ += size;
                    
                    return current->ptr;
                } else {
                    // Use entire block
                    current->isFree = false;
                    usedSize_ += current->size;
                    return current->ptr;
                }
            }
            current = current->next;
        }
        
        // No suitable block found, allocate new one
        if (usedSize_ + size <= totalSize_) {
            void* ptr = ::operator new(size);
            Block* newBlock = new Block(ptr, size);
            
            // Insert at head
            newBlock->next = head_;
            if (head_ != nullptr) {
                head_->prev = newBlock;
            }
            head_ = newBlock;
            
            usedSize_ += size;
            return ptr;
        }
        
        return nullptr;
    }
    
    void deallocate(void* ptr, size_t size) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        Block* current = head_;
        while (current != nullptr) {
            if (current->ptr == ptr && current->size == size) {
                current->isFree = true;
                usedSize_ -= size;
                
                // Merge with adjacent free blocks
                mergeFreeBlocks(current);
                
                return;
            }
            current = current->next;
        }
    }
    
    bool canAllocate(size_t size) const override {
        return size <= blockSize_ && usedSize_ + size <= totalSize_;
    }
    
    size_t getBlockSize() const override {
        return blockSize_;
    }
    
    PoolType getPoolType() const override {
        if (blockSize_ < 256) return PoolType::Small;
        else if (blockSize_ < 4096) return PoolType::Medium;
        else if (blockSize_ < 65536) return PoolType::Large;
        else return PoolType::ExtraLarge;
    }
    
    MemoryStats getStats() const {
        MemoryStats stats;
        stats.totalAllocated = totalSize_;
        stats.currentUsage = usedSize_;
        stats.peakUsage = usedSize_; // Simplified: could track peak
        return stats;
    }
    
private:
    void mergeFreeBlocks(Block* block) {
        if (!block->isFree) return;
        
        Block* current = block;
        
        // Merge with previous free block
        if (current->prev != nullptr && current->prev->isFree) {
            Block* prev = current->prev;
            prev->size += current->size;
            prev->next = current->next;
            if (current->next) {
                current->next->prev = prev;
            }
            delete current;
            current = prev;
        }
        
        // Merge with next free block
        if (current->next != nullptr && current->next->isFree) {
            Block* next = current->next;
            current->size += next->size;
            current->next = next->next;
            if (next->next) {
                next->next->prev = current;
            }
            delete next;
        }
    }
};

// Memory monitoring
class MemoryMonitor {
private:
    static std::atomic<bool> monitoringEnabled_;
    static MemoryStats currentStats_;
    static std::chrono::steady_clock::time_point startTime_;
    static std::mutex statsMutex_;
    
    static void updateStats() {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        // Get memory usage from system (simplified)
        currentStats_.uptimeSeconds = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - startTime_).count();
        
        // Calculate current usage (simplified: return 0)
        currentStats_.currentUsage = 0;
        
        // Update peak usage
        if (currentStats_.currentUsage > currentStats_.peakUsage) {
            currentStats_.peakUsage = currentStats_.currentUsage;
        }
    }
    
    static void collectMemoryStats() {
        // Collect memory statistics from all pools
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        // Reset stats
        MemoryStats newStats;
        newStats.uptimeSeconds = currentStats_.uptimeSeconds;
        
        // Add pool statistics
        for (auto& pair : globalPools_) {
            MemoryStats poolStats = pair.second->getStats();
            newStats.totalAllocated += poolStats.totalAllocated;
            newStats.currentUsage += poolStats.currentUsage;
            newStats.peakUsage = std::max(newStats.peakUsage, poolStats.peakUsage);
            newStats.allocationCount += poolStats.allocationCount;
            newStats.deallocationCount += poolStats.deallocationCount;
        }
        
        currentStats_ = newStats;
    }
    
    static void logMemoryStats() {
        updateStats();
        std::cout << "Memory Statistics:" << std::endl;
        std::cout << "  Total Allocated: " << currentStats_.totalAllocated << " bytes" << std::endl;
        std::cout << "  Current Usage: " << currentStats_.currentUsage << " bytes" << std::endl;
        std::cout << "  Peak Usage: " << currentStats_.peakUsage << " bytes" << std::endl;
        std::cout << "  Allocation Ratio: " << currentStats_.getAllocationRatio() << std::endl;
        std::cout << "  Usage Percentage: " << currentStats_.getUsagePercentage() << "%" << std::endl;
        std::cout << "  Uptime: " << currentStats_.uptimeSeconds << " seconds" << std::endl;
    }
    
public:
    using StatsCallback = std::function<void(const MemoryStats&)>
    
    static void initialize() {
        monitoringEnabled_ = true;
        startTime_ = std::chrono::steady_clock::now();
        
        // Create default memory pools
        createPool(PoolType::Small, 1024 * 1024);   // 1 MB small pool
        createPool(PoolType::Medium, 1024 * 1024);  // 1 MB medium pool
        createPool(PoolType::Large, 1024 * 1024);   // 1 MB large pool
        
        // Start monitoring thread (simplified: disabled for now)
        // monitoringThread_ = std::thread(&MemoryMonitor::monitorLoop);
    }
    
    static void shutdown() {
        monitoringEnabled_ = false;
        
        // Wait for monitoring thread
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
        
        // Clear pools
        std::lock_guard<std::mutex> lock(statsMutex_);
        globalPools_.clear();
    }
    
    static void createPool(PoolType poolType, size_t poolSize) {
        MemoryPoolImpl* pool = nullptr;
        
        switch (poolType) {
            case PoolType::Small:
                pool = new MemoryPoolImpl(256, poolSize);
                break;
            case PoolType::Medium:
                pool = new MemoryPoolImpl(4096, poolSize);
                break;
            case PoolType::Large:
                pool = new MemoryPoolImpl(65536, poolSize);
                break;
            case PoolType::ExtraLarge:
                pool = new MemoryPoolImpl(1024 * 1024, poolSize);
                break;
        }
        
        std::lock_guard<std::mutex> lock(statsMutex_);
        globalPools_[poolType] = pool;
        
        // Set global pool for relevant allocators
        if (poolType == PoolType::Small) {
            MemoryPoolAllocator<void, PoolType::Small>::setGlobalPool(pool);
        }
    }
    
    static MemoryStats getStats() {
        std::lock_guard<std::mutex> lock(statsMutex_);
        updateStats();
        collectMemoryStats();
        return currentStats_;
    }
    
    static void logStats() {
        logMemoryStats();
    }
    
    static std::string getStatsAsJson() {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"totalAllocated\": " << currentStats_.totalAllocated << ",\n";
        oss << "  \"currentUsage\": " << currentStats_.currentUsage << ",\n";
        oss << "  \"peakUsage\": " << currentStats_.peakUsage << ",\n";
        oss << "  \"allocationCount\": " << currentStats_.allocationCount << ",\n";
        oss << "  \"deallocationCount\": " << currentStats_.deallocationCount << ",\n";
        oss << "  \"uptimeSeconds\": " << currentStats_.uptimeSeconds << "\n";
        oss << "}\n";
        return oss.str();
    }
    
    // Get memory usage by pool type
    static std::map<PoolType, size_t> getMemoryUsageByPool() {
        std::map<PoolType, size_t> usage;
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        for (const auto& pair : globalPools_) {
            MemoryStats stats = pair.second->getStats();
            usage[pair.first] = stats.currentUsage;
        }
        
        return usage;
    }
    
    // Force garbage collection
    static void garbageCollect() {
        // Simplified: just clean up unused blocks
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        for (auto& pair : globalPools_) {
            // In a real implementation, we would implement garbage collection
            // For now, just log
            NLM_LOG_INFO(std::string("Garbage collection for pool type ") + 
                        std::to_string(static_cast<int>(pair.first)));
        }
    }
    
    // Set monitoring callback
    static void setStatsCallback(StatsCallback callback) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        statsCallback_ = callback;
    }
    
private:
    static std::map<PoolType, MemoryPoolImpl*> globalPools_;
    static std::thread monitoringThread_;
    static StatsCallback statsCallback_;
};

// Memory monitor static members
std::atomic<bool> MemoryMonitor::monitoringEnabled_(false);
MemoryStats MemoryMonitor::currentStats_;
std::chrono::steady_clock::time_point MemoryMonitor::startTime_;
std::mutex MemoryMonitor::statsMutex_;
std::map<PoolType, MemoryPoolImpl*> MemoryMonitor::globalPools_;
std::thread MemoryMonitor::monitoringThread_;
MemoryMonitor::StatsCallback MemoryMonitor::statsCallback_ = nullptr;

// Memory-managed smart pointer with tracking
template<typename T>
class TrackedSmartPointer : public SmartPointer<T> {
private:
    std::string label_;
    std::chrono::steady_clock::time_point creationTime_;
    
public:
    TrackedSmartPointer() : SmartPointer<T>(), label_(""), creationTime_(std::chrono::steady_clock::now()) {}
    
    TrackedSmartPointer(T* ptr, const std::string& label = "", MemoryPool* pool = nullptr, size_t size = 0)
        : SmartPointer<T>(ptr, pool, size), label_(label), creationTime_(std::chrono::steady_clock::now()) {}
    
    // Get age of the pointer
    std::chrono::milliseconds getAge() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - creationTime_);
    }
    
    // Get label
    const std::string& getLabel() const {
        return label_;
    }
    
    // Set label
    void setLabel(const std::string& label) {
        label_ = label;
    }
    
    // Compare by age
    bool operator<(const TrackedSmartPointer& other) const {
        return this->getAge() < other.getAge();
    }
    
    // Convert to JSON representation
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"label\": \"" << label_ << "\",\n";
        oss << "  \"age_ms\": " << this->getAge().count() << ",\n";
        oss << "  \"is_null\": " << (this->isNull() ? "true" : "false") << "\n";
        oss << "}\n";
        return oss.str();
    }
};

// Memory allocation debugging
class MemoryDebug {
public:
    static void trackAllocation(void* ptr, size_t size, const char* file, int line, const char* function) {
        std::lock_guard<std::mutex> lock(allocationMutex_);
        allocationRecords_.push_back({
            ptr, size, file, line, function, std::chrono::steady_clock::now()
        });
    }
    
    static void trackDeallocation(void* ptr, size_t size) {
        std::lock_guard<std::mutex> lock(allocationMutex_);
        
        auto it = std::find_if(allocationRecords_.begin(), allocationRecords_.end(),
                              [ptr](const AllocationRecord& record) { return record.ptr == ptr; });n        
        if (it != allocationRecords_.end()) {
            it->deallocated = true;
            it->deallocationTime = std::chrono::steady_clock::now();
        }
    }
    
    static void logLeaks() {
        std::lock_guard<std::mutex> lock(allocationMutex_);
        
        std::cout << "=== Memory Leak Detection ===" << std::endl;
        std::cout << "Total allocations: " << allocationRecords_.size() << std::endl;
        
        size_t leakedCount = 0;
        size_t totalLeakedSize = 0;
        
        for (const auto& record : allocationRecords_) {
            if (!record.deallocated) {
                leakedCount++;
                totalLeakedSize += record.size;
                
                std::cout << "LEAK: " << record.size << " bytes at " << record.ptr << std::endl;
                std::cout << "  File: " << record.file << ", Line: " << record.line << std::endl;
                std::cout << "  Function: " << record.function << std::endl;
                std::cout << "  Age: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - record.allocationTime).count() << " ms" << std::endl;
                std::cout << std::endl;
            }
        }
        
        std::cout << "Total leaked allocations: " << leakedCount << std::endl;
        std::cout << "Total leaked size: " << totalLeakedSize << " bytes" << std::endl;
    }
    
    static void clearLeakRecords() {
        std::lock_guard<std::mutex> lock(allocationMutex_);
        allocationRecords_.clear();
    }
    
    static size_t getAllocationCount() {
        std::lock_guard<std::mutex> lock(allocationMutex_);
        return allocationRecords_.size();
    }
    
    static size_t getLeakedCount() {
        std::lock_guard<std::mutex> lock(allocationMutex_);
        return std::count_if(allocationRecords_.begin(), allocationRecords_.end(),
                           [](const AllocationRecord& record) { return !record.deallocated; });
    }
    
    static size_t getLeakedSize() {
        std::lock_guard<std::mutex> lock(allocationMutex_);
        size_t total = 0;
        for (const auto& record : allocationRecords_) {
            if (!record.deallocated) {
                total += record.size;
            }
        }
        return total;
    }
    
private:
    struct AllocationRecord {
        void* ptr;
        size_t size;
        std::string file;
        int line;
        std::string function;
        std::chrono::steady_clock::time_point allocationTime;
        bool deallocated;
        std::chrono::steady_clock::time_point deallocationTime;
        
        AllocationRecord(void* p, size_t s, const char* f, int ln, const char* func)
            : ptr(p), size(s), file(f), line(ln), function(func), 
              allocationTime(std::chrono::steady_clock::now()), 
              deallocated(false), deallocationTime(std::chrono::steady_clock::now()) {}
    };
    
    static std::vector<AllocationRecord> allocationRecords_;
    static std::mutex allocationMutex_;
};

// Memory debug static members
std::vector<MemoryDebug::AllocationRecord> MemoryDebug::allocationRecords_;
std::mutex MemoryDebug::allocationMutex_;

// Convenience macros for memory tracking
#define NLM_TRACK_ALLOCATION(ptr, size) \
    nlm::memory::MemoryDebug::trackAllocation(ptr, size, __FILE__, __LINE__, __func__)

#define NLM_TRACK_DEALLOCATION(ptr, size) \
    nlm::memory::MemoryDebug::trackDeallocation(ptr, size)

#define NLM_NEW(type) \
    new type

#define NLM_DELETE(ptr) \
    do { \
        void* temp = ptr; \
        delete static_cast<decltype(temp)>(temp); \
        nlm::memory::MemoryDebug::trackDeallocation(temp, sizeof(decltype(temp))); \
    } while(0)

// Memory-optimized version of basic data structures
template<typename T>
class MemoryOptimizedVector {
private:
    std::vector<T, MemoryPoolAllocator<T, PoolType::Small>> data_;
    
public:
    MemoryOptimizedVector() = default;
    explicit MemoryOptimizedVector(size_t capacity) {
        data_.reserve(capacity);
    }
    
    // Access methods
    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }
    
    void push_back(const T& value) { data_.push_back(value); }
    void push_back(T&& value) { data_.push_back(std::move(value)); }
    
    void pop_back() { data_.pop_back(); }
    
    size_t size() const { return data_.size(); }
    size_t capacity() const { return data_.capacity(); }
    bool empty() const { return data_.empty(); }
    
    void reserve(size_t capacity) { data_.reserve(capacity); }
    void clear() { data_.clear(); }
    
    // Iterators
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Data access
    const std::vector<T, MemoryPoolAllocator<T, PoolType::Small>>& getData() const { return data_; }
};

// Performance monitoring
class PerformanceMonitor {
private:
    struct PerformanceMetrics {
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point lastSampleTime;
        size_t allocationCount;
        size_t deallocationCount;
        size_t sampleCount;
        
        PerformanceMetrics() : allocationCount(0), deallocationCount(0), sampleCount(0) {
            startTime = std::chrono::steady_clock::now();
            lastSampleTime = startTime;
        }
    };
    
    static PerformanceMetrics metrics_;
    static std::mutex metricsMutex_;
    static std::function<void(const PerformanceMetrics&)> metricsCallback_;
    
public:
    static void initialize() {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.startTime = std::chrono::steady_clock::now();
        metrics_.lastSampleTime = metrics_.startTime;
        metrics_.allocationCount = 0;
        metrics_.deallocationCount = 0;
        metrics_.sampleCount = 0;
    }
    
    static void shutdown() {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        // Log final metrics
        logMetrics();
    }
    
    static void incrementAllocationCount() {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.allocationCount++;
    }
    
    static void incrementDeallocationCount() {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.deallocationCount++;
    }
    
    static void sampleMetrics() {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - metrics_.lastSampleTime);
        
        if (elapsed.count() >= 1000) { // Sample every second
            metrics_.lastSampleTime = now;
            metrics_.sampleCount++;
            
            // Call callback
            if (metricsCallback_) {
                metricsCallback_(metrics_);
            }
        }
    }
    
    static void setMetricsCallback(std::function<void(const PerformanceMetrics&)> callback) {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metricsCallback_ = callback;
    }
    
    static void logMetrics() {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        
        auto totalTime = std::chrono::duration_cast<std::chrono::seconds>(metrics_.startTime - std::chrono::steady_clock::now()).count();
        
        std::cout << "Performance Metrics:" << std::endl;
        std::cout << "  Uptime: " << totalTime << " seconds" << std::endl;
        std::cout << "  Allocations: " << metrics_.allocationCount << std::endl;
        std::cout << "  Deallocations: " << metrics_.deallocationCount << std::endl;
        std::cout << "  Samples: " << metrics_.sampleCount << std::endl;
        
        if (metrics_.allocationCount > 0) {
            double allocationRate = static_cast<double>(metrics_.allocationCount) / totalTime;
            double deallocationRate = static_cast<double>(metrics_.deallocationCount) / totalTime;
            std::cout << "  Allocation Rate: " << allocationRate << " allocations/second" << std::endl;
            std::cout << "  Deallocation Rate: " << deallocationRate << " deallocations/second" << std::endl;
            std::cout << "  Net Allocation Rate: " << (allocationRate - deallocationRate) << " allocations/second" << std::endl;
        }
    }
    
    static std::string getMetricsAsJson() {
        std::ostringstream oss;
        oss << "{\n";
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - metrics_.startTime);
        
        oss << "  \"uptime_seconds\": " << elapsed.count() << ",\n";
        oss << "  \"allocation_count\": " << metrics_.allocationCount << ",\n";
        oss << "  \"deallocation_count\": " << metrics_.deallocationCount << ",\n";
        oss << "  \"sample_count\": " << metrics_.sampleCount << ",\n";
        
        if (metrics_.allocationCount > 0) {
            double allocationRate = static_cast<double>(metrics_.allocationCount) / elapsed.count();
            double deallocationRate = static_cast<double>(metrics_.deallocationCount) / elapsed.count();
            oss << "  \"allocation_rate\": " << allocationRate << ",\n";
            oss << "  \"deallocation_rate\": " << deallocationRate << ",\n";
            oss << "  \"net_allocation_rate\": " << (allocationRate - deallocationRate) << "\n";
        } else {
            oss << "  \"allocation_rate\": 0.0,\n";
            oss << "  \"deallocation_rate\": 0.0,\n";
            oss << "  \"net_allocation_rate\": 0.0\n";
        }
        
        oss << "}\n";
        return oss.str();
    }
};

// Performance monitor static members
PerformanceMonitor::PerformanceMetrics PerformanceMonitor::metrics_;
std::mutex PerformanceMonitor::metricsMutex_;
std::function<void(const PerformanceMonitor::PerformanceMetrics&)> PerformanceMonitor::metricsCallback_ = nullptr;

// Global memory management functions
inline void initializeMemoryManagement() {
    MemoryMonitor::initialize();
    PerformanceMonitor::initialize();
}

inline void shutdownMemoryManagement() {
    PerformanceMonitor::shutdown();
    MemoryMonitor::shutdown();
}

inline MemoryStats getMemoryStats() {
    return MemoryMonitor::getStats();
}

inline void logMemoryStats() {
    MemoryMonitor::logStats();
}

inline void logPerformanceStats() {
    PerformanceMonitor::logMetrics();
}

inline void garbageCollect() {
    MemoryMonitor::garbageCollect();
}

inline void trackAllocation(void* ptr, size_t size, const char* file, int line, const char* function) {
    MemoryDebug::trackAllocation(ptr, size, file, line, function);
}

inline void trackDeallocation(void* ptr, size_t size) {
    MemoryDebug::trackDeallocation(ptr, size);
}

inline void logMemoryLeaks() {
    MemoryDebug::logLeaks();
}

inline void clearLeakRecords() {
    MemoryDebug::clearLeakRecords();
}

} // namespace memory

} // namespace nlm

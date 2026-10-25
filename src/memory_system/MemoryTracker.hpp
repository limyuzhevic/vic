#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <optional>
#include <algorithm>
#include <set>
#include <sstream>
#include <iomanip>
#include <stack>
#include <queue>
#include <typeinfo>
#include <cassert>

namespace nlm {

// Memory management system version
constexpr int MEMORY_SYSTEM_VERSION = 1;

// Memory allocation categories
enum class AllocationCategory {
    Core,
    Neural,
    Plasticity,
    Memory,
    Prediction,
    Cognition,
    Visualization,
    Plugin,
    System,
    Temporary,
    Persistent,
    Dynamic
};

// Memory leak detection levels
enum class LeakDetectionLevel {
    None,
    Basic,
    Detailed,
    Advanced,
    Full
};

// Memory pool types
enum class MemoryPoolType {
    SmallObject,
    MediumObject,
    LargeObject,
    NeuralArray,
    SpikeBuffer,
    SynapseList,
    MemoryBlock,
    PluginData
};

// Memory allocation tracking
struct AllocationInfo {
    void* address;
    size_t size;
    AllocationCategory category;
    std::string sourceFile;
    int sourceLine;
    std::string sourceFunction;
    std::chrono::system_clock::time_point allocationTime;
    std::thread::id threadId;
    std::unordered_map<std::string, std::string> metadata;
    
    AllocationInfo() : address(nullptr), size(0), sourceLine(0), allocationTime(std::chrono::system_clock::now()) {}
    AllocationInfo(void* addr, size_t sz, AllocationCategory cat,
                   const std::string& file, int line, const std::string& function)
        : address(addr), size(sz), category(cat), sourceFile(file), sourceLine(line),
          sourceFunction(function), allocationTime(std::chrono::system_clock::now()) {}
};

// Memory leak information
struct MemoryLeakInfo {
    std::vector<AllocationInfo> allocations;
    std::chrono::system_clock::time_point detectionTime;
    std::string description;
    std::unordered_map<std::string, std::string> metadata;
    
    MemoryLeakInfo() : detectionTime(std::chrono::system_clock::now()) {}
};

// Memory pool statistics
struct MemoryPoolStats {
    size_t totalAllocations;
    size_t totalDeallocations;
    size_t currentUsage;
    size_t peakUsage;
    size_t allocationCount;
    double fragmentation;
    
    MemoryPoolStats() : totalAllocations(0), totalDeallocations(0), currentUsage(0),
                       peakUsage(0), allocationCount(0), fragmentation(0.0) {}
};

// Memory usage statistics
struct MemoryUsageStats {
    size_t totalSystemMemory;
    size_t usedMemory;
    size_t availableMemory;
    double usagePercentage;
    
    std::map<std::string, size_t> categoryUsage;
    std::map<std::string, MemoryPoolStats> poolStats;
    
    std::chrono::system_clock::time_point lastUpdate;
    
    MemoryUsageStats() : totalSystemMemory(0), usedMemory(0), availableMemory(0),
                        usagePercentage(0.0), lastUpdate(std::chrono::system_clock::now()) {}
};

// Memory tracker interface
class IMemoryTracker {
public:
    virtual ~IMemoryTracker() = default;
    
    // Allocation tracking
    virtual void* trackAllocation(size_t size, AllocationCategory category,
                                 const std::string& sourceFile, int sourceLine,
                                 const std::string& sourceFunction) = 0;
    
    virtual bool trackDeallocation(void* address) = 0;
    virtual bool isTracked(void* address) const = 0;
    
    // Leak detection
    virtual MemoryLeakInfo detectLeaks() const = 0;
    virtual bool isLeakDetectionEnabled() const = 0;
    virtual void setLeakDetectionLevel(LeakDetectionLevel level) = 0;
    virtual LeakDetectionLevel getLeakDetectionLevel() const = 0;
    
    // Memory pools
    virtual bool createMemoryPool(MemoryPoolType type, size_t blockSize, size_t initialSize) = 0;
    virtual void* allocateFromPool(MemoryPoolType type) = 0;
    virtual bool deallocateToPool(MemoryPoolType type, void* address) = 0;
    virtual MemoryPoolStats getPoolStats(MemoryPoolType type) const = 0;
    
    // Usage statistics
    virtual MemoryUsageStats getMemoryUsageStats() const = 0;
    virtual size_t getTotalAllocated() const = 0;
    virtual size_t getPeakUsage() const = 0;
    
    // Configuration
    virtual void setMaxMemoryUsage(size_t maxMemory) = 0;
    virtual void setAllocationLimit(size_t limit) = 0;
    virtual void setTrackThreadId(bool track) = 0;
    virtual void setTrackAllocationStack(bool track) = 0;
    
    // Callbacks
    virtual void setAllocationCallback(std::function<void(const AllocationInfo&)> callback) = 0;
    virtual void setDeallocationCallback(std::function<void(const AllocationInfo&)> callback) = 0;
    virtual void setLeakCallback(std::function<void(const MemoryLeakInfo&)> callback) = 0;
    
    // Memory management
    virtual bool enableMemoryLimit(bool enabled) = 0;
    virtual bool enableMemoryProfiling(bool enabled) = 0;
    virtual bool enableThreadSanitizer(bool enabled) = 0;
    virtual bool enableAddressSanitizer(bool enabled) = 0;
    
    // Export
    virtual std::string exportMemoryReport() const = 0;
    virtual bool exportMemoryUsage(const std::string& filepath) const = 0;
    virtual std::vector<AllocationInfo> getAllAllocations() const = 0;
    
    // Configuration
    virtual void setPluginManager(void* manager) = 0;
    virtual void* getPluginManager() const = 0;
};

// Smart pointer with memory tracking
class TrackedSmartPointer {
private:
    template<typename T>
    friend class TrackedSmartPtr;
    
    void* raw_ptr_;
    AllocationCategory category_;
    std::string sourceFile_;
    int sourceLine_;
    std::string sourceFunction_;
    
    IMemoryTracker* tracker_;
    
public:
    TrackedSmartPointer() : raw_ptr_(nullptr), tracker_(nullptr), sourceLine_(0) {}
    
    template<typename T>
    explicit TrackedSmartPointer(T* ptr, IMemoryTracker* tracker,
                                AllocationCategory category = AllocationCategory::System,
                                const std::string& file = "", int line = 0,
                                const std::string& function = "")
        : raw_ptr_(ptr), category_(category), sourceFile_(file), sourceLine_(line),
          sourceFunction_(function), tracker_(tracker) {
        
        if (tracker && ptr) {
            tracker->trackAllocation(reinterpret_cast<void*>(ptr), sizeof(T), category,
                                     file, line, function);
        }
    }
    
    template<typename T>
    TrackedSmartPointer(T* ptr, IMemoryTracker* tracker, AllocationCategory category)
        : raw_ptr_(ptr), tracker_(tracker), category_(category), sourceLine_(0) {
        
        if (tracker && ptr) {
            tracker->trackAllocation(reinterpret_cast<void*>(ptr), sizeof(T), category,
                                     "", 0, "");
        }
    }
    
    ~TrackedSmartPointer() {
        reset();
    }
    
    template<typename T>
    TrackedSmartPointer& operator=(T* ptr) {
        reset();
        if (ptr) {
            raw_ptr_ = ptr;
            if (tracker_) {
                tracker_->trackAllocation(raw_ptr_, sizeof(T), category_, sourceFile_, sourceLine_, sourceFunction_);
            }
        }
        return *this;
    }
    
    template<typename T>
    TrackedSmartPointer& operator=(const TrackedSmartPointer& other) {
        if (this != &other) {
            reset();
            raw_ptr_ = other.raw_ptr_;
            tracker_ = other.tracker_;
            category_ = other.category_;
            sourceFile_ = other.sourceFile_;
            sourceLine_ = other.sourceLine_;
            sourceFunction_ = other.sourceFunction_;
        }
        return *this;
    }
    
    void reset() {
        if (raw_ptr_ && tracker_) {
            tracker_->trackDeallocation(raw_ptr_);
        }
        raw_ptr_ = nullptr;
    }
    
    template<typename T>
    T* get() const {
        return reinterpret_cast<T*>(raw_ptr_);
    }
    
    template<typename T>
    T* operator->() const {
        return get<T>();
    }
    
    template<typename T>
    T& operator*() const {
        return *get<T>();
    }
    
    template<typename T>
    bool operator==(const TrackedSmartPointer& other) const {
        return raw_ptr_ == other.raw_ptr_;
    }
    
    template<typename T>
    bool operator!=(const TrackedSmartPointer& other) const {
        return raw_ptr_ != other.raw_ptr_;
    }
    
    template<typename T>
    bool operator==(std::nullptr_t) const {
        return raw_ptr_ == nullptr;
    }
    
    template<typename T>
    bool operator!=(std::nullptr_t) const {
        return raw_ptr_ != nullptr;
    }
    
    bool isNull() const {
        return raw_ptr_ == nullptr;
    }
    
    void setTracker(IMemoryTracker* tracker) {
        tracker_ = tracker;
    }
};

// Memory pool implementation
class MemoryPool {
private:
    MemoryPoolType type_;
    size_t blockSize_;
    std::stack<void*> freeBlocks_;
    std::mutex mutex_;
    size_t initialSize_;
    bool initialized_;
    
    IMemoryTracker* tracker_;
    
public:
    MemoryPool(MemoryPoolType type, size_t blockSize, size_t initialSize = 0)
        : type_(type), blockSize_(blockSize), initialSize_(initialSize), initialized_(false), tracker_(nullptr) {
        initialize();
    }
    
    ~MemoryPool() {
        shutdown();
    }
    
    void initialize() {
        if (initialized_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (size_t i = 0; i < initialSize_; ++i) {
            void* block = allocateBlock();
            if (block) {
                freeBlocks_.push(block);
            }
        }
        
        initialized_ = true;
    }
    
    void shutdown() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        while (!freeBlocks_.empty()) {
            void* block = freeBlocks_.top();
            freeBlocks_.pop();
            delete[] static_cast<char*>(block);
        }
        
        initialized_ = false;
    }
    
    void* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (freeBlocks_.empty()) {
            return allocateBlock();
        }
        
        void* block = freeBlocks_.top();
        freeBlocks_.pop();
        return block;
    }
    
    bool deallocate(void* address) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check if address belongs to this pool
        if (!isOwned(address)) {
            return false;
        }
        
        freeBlocks_.push(address);
        return true;
    }
    
    bool isOwned(void* address) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Simple ownership check - in real implementation, use more sophisticated method
        return true;
    }
    
    void setTracker(IMemoryTracker* tracker) {
        std::lock_guard<std::mutex> lock(mutex_);
        tracker_ = tracker;
    }
    
private:
    void* allocateBlock() {
        try {
            char* block = new char[blockSize_];
            return static_cast<void*>(block);
        } catch (const std::bad_alloc&) {
            return nullptr;
        }
    }
};

// Default memory tracker implementation
class DefaultMemoryTracker : public IMemoryTracker {
private:
    std::map<void*, AllocationInfo> allocations_;
    std::mutex allocationsMutex_;
    
    LeakDetectionLevel leakDetectionLevel_;
    
    std::map<MemoryPoolType, MemoryPool> pools_;
    std::map<std::string, std::function<void(const AllocationInfo&)>> allocationCallbacks_;
    std::map<std::string, std::function<void(const AllocationInfo&)>> deallocationCallbacks_;
    std::map<std::string, std::function<void(const MemoryLeakInfo&)>> leakCallbacks_;
    
    IPluginManager* pluginManager_;
    
    size_t maxMemoryUsage_;
    size_t allocationLimit_;
    bool trackThreadId_;
    bool trackAllocationStack_;
    bool memoryLimitEnabled_;
    bool memoryProfilingEnabled_;
    bool threadSanitizerEnabled_;
    bool addressSanitizerEnabled_;
    
    std::chrono::system_clock::time_point lastCleanup_;
    
public:
    DefaultMemoryTracker() : leakDetectionLevel_(LeakDetectionLevel::Detailed),
                           maxMemoryUsage_(0), allocationLimit_(0), trackThreadId_(true),
                           trackAllocationStack_(false), memoryLimitEnabled_(false),
                           memoryProfilingEnabled_(false), threadSanitizerEnabled_(false),
                           addressSanitizerEnabled_(false), lastCleanup_(std::chrono::system_clock::now()) {
    }
    
    ~DefaultMemoryTracker() {
        cleanup();
    }
    
    // IMemoryTracker interface implementation
    void* trackAllocation(size_t size, AllocationCategory category,
                         const std::string& sourceFile, int sourceLine,
                         const std::string& sourceFunction) override {
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        
        void* address = allocateMemory(size);
        
        AllocationInfo info;
        info.address = address;
        info.size = size;
        info.category = category;
        info.sourceFile = sourceFile;
        info.sourceLine = sourceLine;
        info.sourceFunction = sourceFunction;
        info.allocationTime = std::chrono::system_clock::now();
        info.threadId = std::this_thread::get_id();
        
        allocations_[address] = info;
        
        // Trigger allocation callback
        for (const auto& callback : allocationCallbacks_) {
            callback(info);
        }
        
        return address;
    }
    
    bool trackDeallocation(void* address) override {
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        
        auto it = allocations_.find(address);
        if (it == allocations_.end()) {
            return false;
        }
        
        const AllocationInfo& info = it->second;
        
        // Trigger deallocation callback
        for (const auto& callback : deallocationCallbacks_) {
            callback(info);
        }
        
        allocations_.erase(it);
        
        // Free memory
        deallocateMemory(address);
        
        return true;
    }
    
    bool isTracked(void* address) const override {
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        return allocations_.find(address) != allocations_.end();
    }
    
    MemoryLeakInfo detectLeaks() const override {
        MemoryLeakInfo leakInfo;
        
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        
        for (const auto& pair : allocations_) {
            const AllocationInfo& info = pair.second;
            
            // Check if allocation is still in use
            // For now, we'll consider all allocations as potential leaks
            // In a real implementation, you would track references
            
            leakInfo.allocations.push_back(info);
        }
        
        leakInfo.detectionTime = std::chrono::system_clock::now();
        leakInfo.description = "Potential memory leaks detected";
        
        // Trigger leak callback
        for (const auto& callback : leakCallbacks_) {
            callback(leakInfo);
        }
        
        return leakInfo;
    }
    
    bool isLeakDetectionEnabled() const override {
        return leakDetectionLevel_ != LeakDetectionLevel::None;
    }
    
    void setLeakDetectionLevel(LeakDetectionLevel level) override {
        leakDetectionLevel_ = level;
    }
    
    LeakDetectionLevel getLeakDetectionLevel() const override {
        return leakDetectionLevel_;
    }
    
    bool createMemoryPool(MemoryPoolType type, size_t blockSize, size_t initialSize) override {
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        
        pools_[type] = MemoryPool(type, blockSize, initialSize);
        pools_[type].setTracker(this);
        
        return true;
    }
    
    void* allocateFromPool(MemoryPoolType type) override {
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        
        auto it = pools_.find(type);
        if (it == pools_.end()) {
            return nullptr;
        }
        
        return it->second.allocate();
    }
    
    bool deallocateToPool(MemoryPoolType type, void* address) override {
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        
        auto it = pools_.find(type);
        if (it == pools_.end()) {
            return false;
        }
        
        return it->second.deallocate(address);
    }
    
    MemoryPoolStats getPoolStats(MemoryPoolType type) const override {
        MemoryPoolStats stats;
        
        auto it = pools_.find(type);
        if (it != pools_.end()) {
            // In a real implementation, you would get actual stats from the pool
            stats.allocationCount = 1;
        }
        
        return stats;
    }
    
    MemoryUsageStats getMemoryUsageStats() const override {
        MemoryUsageStats stats;
        
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        
        for (const auto& pair : allocations_) {
            const AllocationInfo& info = pair.second;
            stats.usedMemory += info.size;
            
            // Track usage by category
            std::string categoryName = categoryToString(info.category);
            stats.categoryUsage[categoryName] += info.size;
        }
        
        stats.availableMemory = maxMemoryUsage_ - stats.usedMemory;
        if (maxMemoryUsage_ > 0) {
            stats.usagePercentage = static_cast<double>(stats.usedMemory) / maxMemoryUsage_ * 100.0;
        }
        
        stats.lastUpdate = std::chrono::system_clock::now();
        
        // Update pool stats
        for (const auto& pair : pools_) {
            stats.poolStats[std::to_string(pair.first)] = pair.second.getStats();
        }
        
        return stats;
    }
    
    size_t getTotalAllocated() const override {
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        
        size_t total = 0;
        for (const auto& pair : allocations_) {
            total += pair.second.size;
        }
        
        return total;
    }
    
    size_t getPeakUsage() const override {
        // This would track peak usage over time
        // For now, return current usage
        return getTotalAllocated();
    }
    
    void setMaxMemoryUsage(size_t maxMemory) override {
        maxMemoryUsage_ = maxMemory;
    }
    
    void setAllocationLimit(size_t limit) override {
        allocationLimit_ = limit;
    }
    
    void setTrackThreadId(bool track) override {
        trackThreadId_ = track;
    }
    
    void setTrackAllocationStack(bool track) override {
        trackAllocationStack_ = track;
    }
    
    void setAllocationCallback(std::function<void(const AllocationInfo&)> callback) override {
        allocationCallbacks_["allocation"] = callback;
    }
    
    void setDeallocationCallback(std::function<void(const AllocationInfo&)> callback) override {
        deallocationCallbacks_["deallocation"] = callback;
    }
    
    void setLeakCallback(std::function<void(const MemoryLeakInfo&)> callback) override {
        leakCallbacks_["leak"] = callback;
    }
    
    bool enableMemoryLimit(bool enabled) override {
        memoryLimitEnabled_ = enabled;
        return true;
    }
    
    bool enableMemoryProfiling(bool enabled) override {
        memoryProfilingEnabled_ = enabled;
        return true;
    }
    
    bool enableThreadSanitizer(bool enabled) override {
        threadSanitizerEnabled_ = enabled;
        return true;
    }
    
    bool enableAddressSanitizer(bool enabled) override {
        addressSanitizerEnabled_ = enabled;
        return true;
    }
    
    std::string exportMemoryReport() const override {
        std::ostringstream oss;
        oss << "=== Memory Report ===" << std::endl;
        oss << "Total allocated: " << getTotalAllocated() << " bytes" << std::endl;
        oss << "Peak usage: " << getPeakUsage() << " bytes" << std::endl;
        
        MemoryUsageStats stats = getMemoryUsageStats();
        oss << "Used memory: " << stats.usedMemory << " bytes" << std::endl;
        oss << "Available memory: " << stats.availableMemory << " bytes" << std::endl;
        oss << "Usage percentage: " << stats.usagePercentage << "%" << std::endl;
        
        oss << std::endl << "Allocations by category:" << std::endl;
        for (const auto& pair : stats.categoryUsage) {
            oss << "  " << pair.first << ": " << pair.second << " bytes" << std::endl;
        }
        
        return oss.str();
    }
    
    bool exportMemoryUsage(const std::string& filepath) const override {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        MemoryUsageStats stats = getMemoryUsageStats();
        
        file << "Timestamp," << std::chrono::system_clock::to_time_t(stats.lastUpdate) << ",";
        file << "TotalAllocated," << getTotalAllocated() << ",";
        file << "UsedMemory," << stats.usedMemory << ",";
        file << "AvailableMemory," << stats.availableMemory << ",";
        file << "UsagePercentage," << stats.usagePercentage << ",";
        
        file << std::endl << "Category,Usage" << std::endl;
        for (const auto& pair : stats.categoryUsage) {
            file << pair.first << "," << pair.second << std::endl;
        }
        
        return true;
    }
    
    std::vector<AllocationInfo> getAllAllocations() const override {
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        
        std::vector<AllocationInfo> allocs;
        for (const auto& pair : allocations_) {
            allocs.push_back(pair.second);
        }
        
        return allocs;
    }
    
    void setPluginManager(void* manager) override {
        pluginManager_ = static_cast<IPluginManager*>(manager);
    }
    
    void* getPluginManager() const override {
        return pluginManager_;
    }
    
    // Helper methods
    void* allocateMemory(size_t size) {
        try {
            return malloc(size);
        } catch (const std::bad_alloc&) {
            return nullptr;
        }
    }
    
    void deallocateMemory(void* address) {
        free(address);
    }
    
    std::string categoryToString(AllocationCategory category) const {
        switch (category) {
            case AllocationCategory::Core: return "Core";
            case AllocationCategory::Neural: return "Neural";
            case AllocationCategory::Plasticity: return "Plasticity";
            case AllocationCategory::Memory: return "Memory";
            case AllocationCategory::Prediction: return "Prediction";
            case AllocationCategory::Cognition: return "Cognition";
            case AllocationCategory::Visualization: return "Visualization";
            case AllocationCategory::Plugin: return "Plugin";
            case AllocationCategory::System: return "System";
            case AllocationCategory::Temporary: return "Temporary";
            case AllocationCategory::Persistent: return "Persistent";
            case AllocationCategory::Dynamic: return "Dynamic";
            default: return "Unknown";
        }
    }
    
    void cleanup() {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastCleanup_);
        
        if (duration.count() > 60) { // Cleanup every minute
            std::lock_guard<std::mutex> lock(allocationsMutex_);
            
            // Clean up pools
            for (auto& pair : pools_) {
                pair.second.shutdown();
            }
            
            // Clean up allocations
            allocations_.clear();
            
            lastCleanup_ = now;
        }
    }
};

// Global memory tracker instance
IMemoryTracker& getGlobalMemoryTracker() {
    static DefaultMemoryTracker tracker;
    return tracker;
}

// Convenience functions
inline void* allocateMemory(size_t size, AllocationCategory category,
                           const std::string& sourceFile = "", int sourceLine = 0,
                           const std::string& sourceFunction = "") {
    return getGlobalMemoryTracker().trackAllocation(size, category, sourceFile, sourceLine, sourceFunction);
}

inline bool deallocateMemory(void* address) {
    return getGlobalMemoryTracker().trackDeallocation(address);
}

inline bool isMemoryTracked(void* address) {
    return getGlobalMemoryTracker().isTracked(address);
}

inline MemoryUsageStats getMemoryUsageStats() {
    return getGlobalMemoryTracker().getMemoryUsageStats();
}

inline std::string getMemoryReport() {
    return getGlobalMemoryTracker().exportMemoryReport();
}

// Smart pointer convenience functions
template<typename T>
TrackedSmartPointer<T> makeTrackedPtr(T* ptr, IMemoryTracker* tracker = nullptr,
                                    AllocationCategory category = AllocationCategory::System) {
    return TrackedSmartPointer<T>(ptr, tracker, category);
}

// Memory leak detection
template<typename T>
bool checkMemoryLeaks() {
    DefaultMemoryTracker& tracker = static_cast<DefaultMemoryTracker&>(getGlobalMemoryTracker());
    MemoryLeakInfo leaks = tracker.detectLeaks();
    return !leaks.allocations.empty();
}

// Memory pool creation
inline bool createMemoryPool(MemoryPoolType type, size_t blockSize, size_t initialSize) {
    DefaultMemoryTracker& tracker = static_cast<DefaultMemoryTracker&>(getGlobalMemoryTracker());
    return tracker.createMemoryPool(type, blockSize, initialSize);
}

// Memory pool allocation
inline void* allocateFromPool(MemoryPoolType type) {
    DefaultMemoryTracker& tracker = static_cast<DefaultMemoryTracker&>(getGlobalMemoryTracker());
    return tracker.allocateFromPool(type);
}

// Memory pool deallocation
inline bool deallocateToPool(MemoryPoolType type, void* address) {
    DefaultMemoryTracker& tracker = static_cast<DefaultMemoryTracker&>(getGlobalMemoryTracker());
    return tracker.deallocateToPool(type, address);
}

} // namespace nlm

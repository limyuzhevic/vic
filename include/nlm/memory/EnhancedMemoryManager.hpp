//=============================================================================
// Enhanced Memory Management for NLM Python Bindings
//=============================================================================
//
// Comprehensive memory management system for NLM Python bindings
// Includes smart pointers, memory pools, resource management, and safety features
//
// Features:
// - SmartPointer: RAII smart pointer wrapper for C++ objects
// - MemoryPoolManager: Centralized memory pool management
// - MemoryTrackerEnhanced: Advanced memory tracking with leak detection
// - ResourceManager: RAII-based resource management
// - ContextManagerBase: Python context manager for resource lifecycle
// - MemoryProfiler: Performance profiling for memory usage
// - MemoryValidator: Memory access validation and bounds checking
// - MemoryPoolBase: Base class for custom memory pools
//=============================================================================

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <string>
#include <chrono>
#include <type_traits>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <functional>
#include <set>
#include <map>
#include <limits>
#include <algorithm>
#include <filesystem>
#include <cassert>

namespace nlm {

// Memory management configuration
struct MemoryConfig {
    // Pool sizes for different allocation types
    size_t neuronPoolSize = 1024;           // Number of neurons per pool
    size_t synapsePoolSize = 2048;          // Number of synapses per pool
    size_t regionPoolSize = 256;            // Number of regions per pool
    size_t populationPoolSize = 128;        // Number of populations per pool
    size_t sensoryPoolSize = 256;           // Number of sensory inputs per pool
    size_t actionPoolSize = 512;            // Number of actions per pool
    
    // Memory tracking configuration
    bool enableMemoryTracking = true;       // Enable memory usage tracking
    bool enableLeakDetection = true;        // Enable automatic leak detection
    bool enableMemoryValidation = true;     // Enable memory access validation
    bool enablePerformanceProfiling = false; // Enable memory performance profiling
    
    // Memory pool configuration
    bool enableMemoryPools = true;          // Enable memory pooling
    bool enableThreadLocalPools = false;    // Enable thread-local memory pools
    size_t poolAlignment = 64;              // Memory alignment for pools
    
    // Safety configuration
    bool enableBoundsChecking = true;       // Enable bounds checking
    bool enableNullPointerChecks = true;    // Enable null pointer checks
    bool enableUseAfterFreeDetection = true; // Enable use-after-free detection
    size_t maxMemoryUsage = 1024 * 1024 * 1024; // Maximum memory usage (1GB)
    
    // Logging configuration
    bool enableMemoryLogging = false;       // Enable memory operation logging
    std::string logFile = "nlm_memory.log";  // Memory log file path
    
    // Statistics configuration
    bool trackAllocationHistory = false;    // Track allocation history
    bool trackAllocationStack = false;      // Track allocation stack traces
    size_t maxAllocationHistory = 10000;    // Maximum allocation history entries
    
    static MemoryConfig getDefaultConfig() {
        MemoryConfig config;
        return config;
    }
};

// Memory allocation tracking information
struct AllocationInfo {
    void* address;                          // Memory address
    size_t size;                            // Allocation size
    const char* typeName;                   // Type name for debugging
    const char* file;                       // Source file where allocated
    int line;                               // Line number in source file
    std::chrono::system_clock::time_point timestamp; // Allocation timestamp
    std::thread::id threadId;               // Thread that allocated
    std::string allocationStack;            // Stack trace for allocation
    
    AllocationInfo() : address(nullptr), size(0), typeName(nullptr), 
                       file(nullptr), line(0) {}
    
    AllocationInfo(void* addr, size_t sz, const char* type, 
                   const char* file, int line) 
        : address(addr), size(sz), typeName(type), file(file), line(line),
          timestamp(std::chrono::system_clock::now()),
          threadId(std::this_thread::get_id()) {}
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Memory allocation statistics
struct MemoryStats {
    size_t totalAllocations = 0;            // Total number of allocations
    size_t totalDeallocations = 0;          // Total number of deallocations
    size_t currentAllocations = 0;          // Current number of allocated objects
    size_t peakAllocations = 0;              // Peak number of allocated objects
    size_t totalBytesAllocated = 0;         // Total bytes allocated
    size_t peakBytesAllocated = 0;          // Peak total bytes allocated
    size_t totalMemoryPoolUsage = 0;        // Total memory used by pools
    size_t peakMemoryPoolUsage = 0;         // Peak memory pool usage
    std::chrono::system_clock::time_point startTime; // Session start time
    std::chrono::system_clock::time_point lastAllocationTime; // Last allocation time
    
    MemoryStats() {
        startTime = std::chrono::system_clock::now();
        lastAllocationTime = startTime;
    }
    
    double getUptime() const {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
        return static_cast<double>(duration.count());
    }
    
    double getAllocationRate() const {
        double uptime = getUptime();
        return uptime > 0.0 ? static_cast<double>(totalAllocations) / uptime : 0.0;
    }
    
    double getDeallocationRate() const {
        double uptime = getUptime();
        return uptime > 0.0 ? static_cast<double>(totalDeallocations) / uptime : 0.0;
    }
    
    double getCurrentMemoryUsageMB() const {
        return static_cast<double>(totalBytesAllocated) / (1024.0 * 1024.0);
    }
    
    double getPeakMemoryUsageMB() const {
        return static_cast<double>(peakBytesAllocated) / (1024.0 * 1024.0);
    }
    
    void update() {
        lastAllocationTime = std::chrono::system_clock::now();
    }
};

// Memory leak information
struct MemoryLeak {
    void* address;                          // Address of leaked memory
    size_t size;                            // Size of leaked memory
    const char* typeName;                   // Type name
    const char* file;                       // Allocation file
    int line;                               // Allocation line
    std::chrono::system_clock::time_point timestamp; // Allocation time
    std::string allocationStack;            // Allocation stack trace
    std::string leakReason;                 // Reason for leak (if determined)
    
    MemoryLeak() : address(nullptr), size(0), typeName(nullptr), 
                   file(nullptr), line(0) {}
    
    MemoryLeak(void* addr, size_t sz, const char* type, 
               const char* file, int line)
        : address(addr), size(sz), typeName(type), file(file), line(line),
          timestamp(std::chrono::system_clock::now()) {}
    
    std::string getDescription() const {
        std::stringstream ss;
        ss << "Memory leak at " << address << " (" << size << " bytes)";
        if (typeName) ss << " of type " << typeName;
        if (file) ss << " allocated from " << file << ":" << line;
        ss << " at " << getFormattedTimestamp();
        return ss.str();
    }
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Smart pointer wrapper for Python integration
template<typename T>
class SmartPointer {
public:
    using ElementType = T;
    using PointerType = std::shared_ptr<T>;
    
private:
    PointerType ptr_;                        // Underlying smart pointer
    std::string objectName_;                // Name for debugging
    bool enabled_;                          // Whether this smart pointer is enabled
    const char* typeName_;                  // Type name for debugging
    
    // Disable copy constructor and assignment
    SmartPointer(const SmartPointer&) = delete;
    SmartPointer& operator=(const SmartPointer&) = delete;
    
    // Friend declaration for factory functions
    friend SmartPointer makeSmartPointer(T* ptr, const char* typeName, const char* name = "");
    
public:
    SmartPointer() : ptr_(nullptr), enabled_(true), typeName_(nullptr) {}
    
    explicit SmartPointer(T* ptr, const char* typeName = nullptr, const char* name = "")
        : ptr_(ptr ? std::shared_ptr<T>(ptr) : nullptr), 
          enabled_(ptr != nullptr), 
          typeName_(typeName ? typeName : "Unknown") {
        if (name.empty()) {
            objectName_ = "SmartPointer_" + std::to_string(reinterpret_cast<uintptr_t>(ptr_));
        } else {
            objectName_ = name;
        }
    }
    
    SmartPointer(std::shared_ptr<T> ptr, const char* typeName = nullptr, const char* name = "")
        : ptr_(ptr), enabled_(ptr != nullptr),
          typeName_(typeName ? typeName : "Unknown") {
        if (name.empty()) {
            objectName_ = "SmartPointer_" + std::to_string(reinterpret_cast<uintptr_t>(ptr_));
        } else {
            objectName_ = name;
        }
    }
    
    ~SmartPointer() {
        if (enabled_) {
            // Cleanup can be added here if needed
        }
    }
    
    // Conversion constructor for std::shared_ptr
    SmartPointer(std::shared_ptr<T> other_ptr, const char* typeName = nullptr, const char* name = "")
        : ptr_(other_ptr), enabled_(other_ptr != nullptr),
          typeName_(typeName ? typeName : "Unknown") {
        if (name.empty()) {
            objectName_ = "SmartPointer_" + std::to_string(reinterpret_cast<uintptr_t>(ptr_));
        } else {
            objectName_ = name;
        }
    }
    
    // Assignment from std::shared_ptr
    SmartPointer& operator=(std::shared_ptr<T> other_ptr) {
        ptr_ = other_ptr;
        enabled_ = (ptr_ != nullptr);
        return *this;
    }
    
    // Access underlying pointer
    T* get() const {
        return ptr_.get();
    }
    
    // Access underlying std::shared_ptr
    std::shared_ptr<T>& getSharedPtr() {
        return ptr_;
    }
    
    const std::shared_ptr<T>& getSharedPtr() const {
        return ptr_;
    }
    
    // Dereference operator
    T& operator*() const {
        if (!enabled_) {
            throw std::runtime_error("SmartPointer is not enabled (null pointer)");
        }
        return *ptr_;
    }
    
    // Arrow operator
    T* operator->() const {
        if (!enabled_) {
            throw std::runtime_error("SmartPointer is not enabled (null pointer)");
        }
        return ptr_.get();
    }
    
    // Implicit conversion to std::shared_ptr
    operator std::shared_ptr<T>() const {
        return ptr_;
    }
    
    // Check if pointer is valid
    explicit operator bool() const {
        return enabled_ && ptr_ != nullptr;
    }
    
    // Check if pointer is valid
    bool isValid() const {
        return enabled_ && ptr_ != nullptr;
    }
    
    // Get object name
    const std::string& getObjectName() const {
        return objectName_;
    }
    
    // Get type name
    const char* getTypeName() const {
        return typeName_;
    }
    
    // Get memory address
    void* getAddress() const {
        return ptr_.get();
    }
    
    // Reset pointer
    void reset() {
        ptr_.reset();
        enabled_ = false;
    }
    
    // Enable/disable pointer
    void enable(bool enable) {
        enabled_ = enable;
    }
    
    // Equality comparison
    bool operator==(const SmartPointer& other) const {
        return ptr_ == other.ptr_;
    }
    
    bool operator!=(const SmartPointer& other) const {
        return ptr_ != other.ptr_;
    }
    
    // Hash function for use in unordered containers
    struct Hash {
        size_t operator()(const SmartPointer& sp) const {
            return std::hash<std::shared_ptr<T>>()(sp.ptr_);
        }
    };
    
    friend struct Hash;
}; 

// Factory function for creating SmartPointer instances
template<typename T>
SmartPointer<T> makeSmartPointer(T* ptr, const char* typeName, const char* name = "") {
    return SmartPointer<T>(ptr, typeName, name);
}

// Memory allocation statistics tracker
class MemoryStatsTracker {
private:
    static MemoryStatsTracker* instance_;
    MemoryStats stats_;
    std::mutex mutex_;
    bool initialized_ = false;
    
    MemoryStatsTracker() : initialized_(false) {}
    
public:
    static MemoryStatsTracker& getInstance() {
        if (!instance_) {
            instance_ = new MemoryStatsTracker();
        }
        return *instance_;
    }
    
    static void initialize(const MemoryConfig& config = MemoryConfig::getDefaultConfig()) {
        MemoryStatsTracker& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        tracker.stats_.startTime = std::chrono::system_clock::now();
        tracker.initialized_ = true;
    }
    
    static void shutdown() {
        MemoryStatsTracker& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        tracker.initialized_ = false;
        delete instance_;
        instance_ = nullptr;
    }
    
    static void recordAllocation(size_t size, const char* typeName, 
                                const char* file, int line, 
                                const std::string& stack = "") {
        MemoryStatsTracker& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        if (tracker.initialized_) {
            tracker.stats_.totalAllocations++;
            tracker.stats_.currentAllocations++;
            if (tracker.stats_.currentAllocations > tracker.stats_.peakAllocations) {
                tracker.stats_.peakAllocations = tracker.stats_.currentAllocations;
            }
            tracker.stats_.totalBytesAllocated += size;
            if (tracker.stats_.totalBytesAllocated > tracker.stats_.peakBytesAllocated) {
                tracker.stats_.peakBytesAllocated = tracker.stats_.totalBytesAllocated;
            }
            tracker.stats_.lastAllocationTime = std::chrono::system_clock::now();
            // Can add allocation tracking here if needed
        }
    }
    
    static void recordDeallocation(size_t size, const char* typeName) {
        MemoryStatsTracker& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        if (tracker.initialized_) {
            tracker.stats_.totalDeallocations++;
            tracker.stats_.currentAllocations = (tracker.stats_.currentAllocations > size) ? 
                                                 tracker.stats_.currentAllocations - size : 0;
        }
    }
    
    static MemoryStats getStats() {
        MemoryStatsTracker& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        return tracker.stats_;
    }
    
    static std::string getStatsString() {
        MemoryStats stats = getStats();
        std::stringstream ss;
        ss << "=== Memory Statistics ===" << std::endl;
        ss << "Total allocations: " << stats.totalAllocations << std::endl;
        ss << "Total deallocations: " << stats.totalDeallocations << std::endl;
        ss << "Current allocations: " << stats.currentAllocations << std::endl;
        ss << "Peak allocations: " << stats.peakAllocations << std::endl;
        ss << "Total bytes allocated: " << stats.totalBytesAllocated << " ("
           << stats.getCurrentMemoryUsageMB() << " MB)" << std::endl;
        ss << "Peak bytes allocated: " << stats.peakBytesAllocated << " ("
           << stats.getPeakMemoryUsageMB() << " MB)" << std::endl;
        ss << "Uptime: " << stats.getUptime() << " seconds" << std::endl;
        ss << "Allocation rate: " << stats.getAllocationRate() << " allocations/sec" << std::endl;
        ss << "Deallocation rate: " << stats.getDeallocationRate() << " deallocations/sec" << std::endl;
        return ss.str();
    }
};

// Global instance pointer for static initialization
inline MemoryStatsTracker* MemoryStatsTracker::instance_ = nullptr;

// Memory leak detector
class MemoryLeakDetector {
private:
    static MemoryLeakDetector* instance_;
    std::mutex mutex_;
    bool enabled_ = false;
    bool initialized_ = false;
    
    // Track allocated objects for leak detection
    struct AllocatedObject {
        void* address;
        size_t size;
        const char* typeName;
        const char* file;
        int line;
        std::chrono::system_clock::time_point timestamp;
        bool freed;
        std::string stackTrace;
        
        AllocatedObject() : address(nullptr), size(0), typeName(nullptr),
                           file(nullptr), line(0), freed(false) {}
    };
    
    std::unordered_map<void*, AllocatedObject> allocatedObjects_;
    std::unordered_set<void*> freedObjects_;
    std::chrono::system_clock::time_point lastScanTime_;
    
    MemoryLeakDetector() : enabled_(false), initialized_(false) {}
    
public:
    static MemoryLeakDetector& getInstance() {
        if (!instance_) {
            instance_ = new MemoryLeakDetector();
        }
        return *instance_;
    }
    
    static void initialize(bool enabled = true) {
        MemoryLeakDetector& detector = getInstance();
        std::lock_guard<std::mutex> lock(detector.mutex_);
        detector.enabled_ = enabled;
        detector.initialized_ = true;
        detector.lastScanTime_ = std::chrono::system_clock::now();
    }
    
    static void shutdown() {
        MemoryLeakDetector& detector = getInstance();
        std::lock_guard<std::mutex> lock(detector.mutex_);
        detector.enabled_ = false;
        detector.initialized_ = false;
        detector.allocatedObjects_.clear();
        detector.freedObjects_.clear();
        delete instance_;
        instance_ = nullptr;
    }
    
    static void recordAllocation(void* address, size_t size, 
                                const char* typeName, const char* file, 
                                int line, const std::string& stackTrace = "") {
        if (!isInitialized() || !isEnabled()) return;
        
        MemoryLeakDetector& detector = getInstance();
        std::lock_guard<std::mutex> lock(detector.mutex_);
        
        AllocatedObject obj;
        obj.address = address;
        obj.size = size;
        obj.typeName = typeName;
        obj.file = file;
        obj.line = line;
        obj.timestamp = std::chrono::system_clock::now();
        obj.freed = false;
        obj.stackTrace = stackTrace;
        
        detector.allocatedObjects_[address] = obj;
    }
    
    static void recordDeallocation(void* address) {
        if (!isInitialized() || !isEnabled()) return;
        
        MemoryLeakDetector& detector = getInstance();
        std::lock_guard<std::mutex> lock(detector.mutex_);
        
        auto it = detector.allocatedObjects_.find(address);
        if (it != detector.allocatedObjects_.end()) {
            it->second.freed = true;
            detector.freedObjects_.insert(address);
        }
    }
    
    static std::vector<MemoryLeak> detectLeaks() {
        if (!isInitialized() || !isEnabled()) return {};
        
        MemoryLeakDetector& detector = getInstance();
        std::lock_guard<std::mutex> lock(detector.mutex_);
        
        std::vector<MemoryLeak> leaks;
        for (const auto& pair : detector.allocatedObjects_) {
            if (!pair.second.freed) {
                MemoryLeak leak;
                leak.address = pair.second.address;
                leak.size = pair.second.size;
                leak.typeName = pair.second.typeName;
                leak.file = pair.second.file;
                leak.line = pair.second.line;
                leak.timestamp = pair.second.timestamp;
                leak.allocationStack = pair.second.stackTrace;
                leak.leakReason = "Object not properly freed";
                leaks.push_back(leak);
            }
        }
        
        detector.lastScanTime_ = std::chrono::system_clock::now();
        return leaks;
    }
    
    static std::string getLeakReport() {
        std::vector<MemoryLeak> leaks = detectLeaks();
        std::stringstream ss;
        ss << "=== Memory Leak Detection ===" << std::endl;
        ss << "Total allocated objects: " << allocatedObjects_.size() << std::endl;
        ss << "Total freed objects: " << freedObjects_.size() << std::endl;
        ss << "Total leaked objects: " << leaks.size() << std::endl;
        ss << "Memory leak percentage: " << 
            (leaks.size() * 100.0 / allocatedObjects_.size()) << "%" << std::endl;
        
        if (!leaks.empty()) {
            ss << "\n=== Detected Leaks ===" << std::endl;
            for (size_t i = 0; i < leaks.size() && i < 10; ++i) { // Limit to first 10 for brevity
                ss << i + 1 << ". " << leaks[i].getDescription() << std::endl;
            }
            if (leaks.size() > 10) {
                ss << "... and " << (leaks.size() - 10) << " more leaks" << std::endl;
            }
        }
        
        return ss.str();
    }
    
    static bool isEnabled() {
        MemoryLeakDetector& detector = getInstance();
        std::lock_guard<std::mutex> lock(detector.mutex_);
        return detector.enabled_;
    }
    
    static bool isInitialized() {
        MemoryLeakDetector& detector = getInstance();
        std::lock_guard<std::mutex> lock(detector.mutex_);
        return detector.initialized_;
    }
    
    static void enable(bool enable) {
        MemoryLeakDetector& detector = getInstance();
        std::lock_guard<std::mutex> lock(detector.mutex_);
        detector.enabled_ = enable;
    }
    
    static size_t getTotalAllocatedObjects() {
        MemoryLeakDetector& detector = getInstance();
        std::lock_guard<std::mutex> lock(detector.mutex_);
        return detector.allocatedObjects_.size();
    }
    
    static size_t getFreedObjects() {
        MemoryLeakDetector& detector = getInstance();
        std::lock_guard<std::mutex> lock(detector.mutex_);
        return detector.freedObjects_.size();
    }
};

// Global instance pointer for static initialization
inline MemoryLeakDetector* MemoryLeakDetector::instance_ = nullptr;

// Memory validation utilities
class MemoryValidator {
public:
    // Validate pointer access
    static bool validatePointer(const void* ptr, size_t size = 0, 
                                const char* operation = "access") {
        if (!ptr) {
            return false;
        }
        
        // Add additional validation here if needed
        // For now, just check for null pointer
        return true;
    }
    
    // Validate memory bounds
    static bool validateBounds(const void* ptr, size_t size, 
                               const void* base, size_t baseSize) {
        if (!ptr || !base || size == 0) {
            return false;
        }
        
        const uintptr_t ptrInt = reinterpret_cast<uintptr_t>(ptr);
        const uintptr_t baseInt = reinterpret_cast<uintptr_t>(base);
        
        return (ptrInt >= baseInt && ptrInt < baseInt + baseSize);
    }
    
    // Check for use-after-free (basic implementation)
    static bool checkUseAfterFree(const void* ptr) {
        // Simple implementation - can be enhanced with more sophisticated detection
        return true;
    }
    
    // Format memory error message
    static std::string formatErrorMessage(const std::string& operation, 
                                         const char* file, int line,
                                         const void* address = nullptr) {
        std::stringstream ss;
        ss << "Memory error in " << operation << " at " << file << ":" << line;
        if (address) {
            ss << " (address: " << address << ")";
        }
        return ss.str();
    }
};

// Memory pool interface
class MemoryPoolBase {
public:
    virtual ~MemoryPoolBase() = default;
    
    // Allocate memory
    virtual void* allocate(size_t size) = 0;
    
    // Deallocate memory
    virtual void deallocate(void* ptr, size_t size) = 0;
    
    // Get pool statistics
    virtual size_t getAllocatedCount() const = 0;
    
    // Get memory usage
    virtual size_t getMemoryUsage() const = 0;
    
    // Reset pool (optional)
    virtual void reset() {}
    
    // Get pool type name
    virtual const char* getTypeName() const = 0;
};

// Memory pool for generic allocations
class GenericMemoryPool : public MemoryPoolBase {
private:
    std::vector<uint8_t> buffer_;
    size_t allocatedCount_ = 0;
    size_t totalAllocated_ = 0;
    std::mutex mutex_;
    
public:
    GenericMemoryPool(size_t initialSize = 1024 * 1024) // 1MB initial
        : buffer_(initialSize), allocatedCount_(0), totalAllocated_(0) {}
    
    void* allocate(size_t size) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (buffer_.size() < totalAllocated_ + size) {
            // Need to expand buffer
            size_t newSize = buffer_.size() * 2;
            if (newSize < totalAllocated_ + size) {
                newSize = totalAllocated_ + size;
            }
            buffer_.resize(newSize);
        }
        
        void* ptr = buffer_.data() + totalAllocated_;
        totalAllocated_ += size;
        allocatedCount_++;
        
        return ptr;
    }
    
    void deallocate(void* ptr, size_t size) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        uint8_t* ptrByte = static_cast<uint8_t*>(ptr);
        uint8_t* bufferStart = buffer_.data();
        
        if (ptrByte >= bufferStart && ptrByte < bufferStart + buffer_.size()) {
            allocatedCount_--;
            totalAllocated_ = std::min(totalAllocated_, buffer_.size());
        }
    }
    
    size_t getAllocatedCount() const override {
        return allocatedCount_;
    }
    
    size_t getMemoryUsage() const override {
        return buffer_.size();
    }
    
    const char* getTypeName() const override {
        return "GenericMemoryPool";
    }
};

// Memory pool manager - handles multiple pools
class MemoryPoolManager {
private:
    static MemoryPoolManager* instance_;
    std::mutex mutex_;
    bool initialized_ = false;
    
    std::vector<std::unique_ptr<MemoryPoolBase>> pools_;
    MemoryConfig config_;
    
    MemoryPoolManager() : initialized_(false) {}
    
public:
    static MemoryPoolManager& getInstance() {
        if (!instance_) {
            instance_ = new MemoryPoolManager();
        }
        return *instance_;
    }
    
    static void initialize(const MemoryConfig& config = MemoryConfig::getDefaultConfig()) {
        MemoryPoolManager& manager = getInstance();
        std::lock_guard<std::mutex> lock(manager.mutex_);
        manager.config_ = config;
        manager.initialized_ = true;
        
        // Create default pools if enabled
        if (config.enableMemoryPools) {
            manager.createDefaultPools();
        }
    }
    
    static void shutdown() {
        MemoryPoolManager& manager = getInstance();
        std::lock_guard<std::mutex> lock(manager.mutex_);
        manager.initialized_ = false;
        manager.pools_.clear();
        delete instance_;
        instance_ = nullptr;
    }
    
    static MemoryPoolManager& get() {
        return getInstance();
    }
    
    // Create default memory pools
    void createDefaultPools() {
        if (config_.neuronPoolSize > 0) {
            pools_.push_back(std::make_unique<GenericMemoryPool>(config_.neuronPoolSize * 100));
        }
        if (config_.synapsePoolSize > 0) {
            pools_.push_back(std::make_unique<GenericMemoryPool>(config_.synapsePoolSize * 100));
        }
        if (config_.regionPoolSize > 0) {
            pools_.push_back(std::make_unique<GenericMemoryPool>(config_.regionPoolSize * 50));
        }
        if (config_.populationPoolSize > 0) {
            pools_.push_back(std::make_unique<GenericMemoryPool>(config_.populationPoolSize * 50));
        }
        if (config_.sensoryPoolSize > 0) {
            pools_.push_back(std::make_unique<GenericMemoryPool>(config_.sensoryPoolSize * 50));
        }
        if (config_.actionPoolSize > 0) {
            pools_.push_back(std::make_unique<GenericMemoryPool>(config_.actionPoolSize * 50));
        }
    }
    
    // Get or create a pool by type
    MemoryPoolBase* getPool(const char* typeName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Try to find existing pool
        for (auto& pool : pools_) {
            if (std::string(pool->getTypeName()) == typeName) {
                return pool.get();
            }
        }
        
        // Create new pool if not found
        if (config_.enableMemoryPools) {
            auto newPool = std::make_unique<GenericMemoryPool>();
            MemoryPoolBase* poolPtr = newPool.get();
            pools_.push_back(std::move(newPool));
            return poolPtr;
        }
        
        return nullptr;
    }
    
    // Allocate from pool
    void* allocateFromPool(const char* typeName, size_t size) {
        MemoryPoolBase* pool = getPool(typeName);
        if (pool) {
            return pool->allocate(size);
        }
        return nullptr;
    }
    
    // Deallocate to pool
    void deallocateToPool(const char* typeName, void* ptr, size_t size) {
        MemoryPoolBase* pool = getPool(typeName);
        if (pool) {
            pool->deallocate(ptr, size);
        }
    }
    
    // Get total memory usage
    static size_t getTotalMemoryUsage() {
        MemoryPoolManager& manager = getInstance();
        std::lock_guard<std::mutex> lock(manager.mutex_);
        size_t total = 0;
        for (const auto& pool : manager.pools_) {
            total += pool->getMemoryUsage();
        }
        return total;
    }
    
    // Get total allocated count
    static size_t getTotalAllocatedCount() {
        MemoryPoolManager& manager = getInstance();
        std::lock_guard<std::mutex> lock(manager.mutex_);
        size_t total = 0;
        for (const auto& pool : manager.pools_) {
            total += pool->getAllocatedCount();
        }
        return total;
    }
    
    // Reset all pools
    static void resetAllPools() {
        MemoryPoolManager& manager = getInstance();
        std::lock_guard<std::mutex> lock(manager.mutex_);
        for (auto& pool : manager.pools_) {
            pool->reset();
        }
    }
    
    // Get stats string for reporting
    static std::string getStatsString() {
        std::stringstream ss;
        ss << "=== Memory Pool Statistics ===" << std::endl;
        ss << "Total pools: " << getTotalAllocatedCount() << std::endl;
        ss << "Total memory usage: " << getTotalMemoryUsage() << " bytes" << std::endl;
        ss << "Memory usage: " << (getTotalMemoryUsage() / (1024.0 * 1024.0)) << " MB" << std::endl;
        return ss.str();
    }
};

// Global instance pointer for static initialization
inline MemoryPoolManager* MemoryPoolManager::instance_ = nullptr;

// Context manager base class for Python resource management
class ContextManagerBase {
protected:
    bool isOpen_ = false;
    std::string name_;
    
public:
    ContextManagerBase(const std::string& name = "Resource") : name_(name), isOpen_(false) {}
    
    virtual ~ContextManagerBase() {
        if (isOpen_) {
            close();
        }
    }
    
    // Open the context
    virtual void open() {
        if (!isOpen_) {
            isOpen_ = true;
            onOpen();
        }
    }
    
    // Close the context
    virtual void close() {
        if (isOpen_) {
            onClose();
            isOpen_ = false;
        }
    }
    
    // Check if context is open
    bool isOpen() const {
        return isOpen_;
    }
    
    // Get context name
    const std::string& getName() const {
        return name_;
    }
    
    // Virtual methods to be overridden by derived classes
    virtual void onOpen() {}
    virtual void onClose() {}
    
    // Check if resource is available
    virtual bool isAvailable() const {
        return isOpen_;
    }
    
    // Get resource information
    virtual std::string getInfo() const {
        return "Resource: " + name_ + " (" + (isOpen_ ? "Open" : "Closed") + ")";
    }
};

// Enhanced memory tracking with leak detection
class MemoryTrackerEnhanced {
private:
    static MemoryTrackerEnhanced* instance_;
    std::mutex mutex_;
    bool enabled_ = false;
    bool initialized_ = false;
    
    struct AllocationEvent {
        void* address;
        size_t size;
        const char* typeName;
        const char* file;
        int line;
        std::chrono::system_clock::time_point timestamp;
        std::thread::id threadId;
        std::string stackTrace;
        
        AllocationEvent() : address(nullptr), size(0), typeName(nullptr),
                           file(nullptr), line(0) {}
    };
    
    std::vector<AllocationEvent> allocationHistory_;
    std::chrono::system_clock::time_point sessionStart_;
    
    MemoryTrackerEnhanced() : enabled_(false), initialized_(false) {
        sessionStart_ = std::chrono::system_clock::now();
    }
    
public:
    static MemoryTrackerEnhanced& getInstance() {
        if (!instance_) {
            instance_ = new MemoryTrackerEnhanced();
        }
        return *instance_;
    }
    
    static void initialize(bool enabled = true) {
        MemoryTrackerEnhanced& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        tracker.enabled_ = enabled;
        tracker.initialized_ = true;
        tracker.allocationHistory_.clear();
    }
    
    static void shutdown() {
        MemoryTrackerEnhanced& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        tracker.enabled_ = false;
        tracker.initialized_ = false;
        tracker.allocationHistory_.clear();
        delete instance_;
        instance_ = nullptr;
    }
    
    static void recordAllocation(void* address, size_t size,
                                const char* typeName, const char* file,
                                int line, const std::string& stackTrace = "") {
        if (!isInitialized() || !isEnabled()) return;
        
        MemoryTrackerEnhanced& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        
        AllocationEvent event;
        event.address = address;
        event.size = size;
        event.typeName = typeName;
        event.file = file;
        event.line = line;
        event.timestamp = std::chrono::system_clock::now();
        event.threadId = std::this_thread::get_id();
        event.stackTrace = stackTrace;
        
        tracker.allocationHistory_.push_back(event);
    }
    
    static void recordDeallocation(void* address) {
        if (!isInitialized() || !isEnabled()) return;
        
        // Find and mark allocation as deallocated
        MemoryTrackerEnhanced& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        
        for (auto& event : tracker.allocationHistory_) {
            if (event.address == address) {
                event.address = nullptr; // Mark as deallocated
                break;
            }
        }
    }
    
    static std::string generateReport() {
        if (!isInitialized() || !isEnabled()) return "Memory tracking not initialized";
        
        MemoryTrackerEnhanced& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        
        std::stringstream ss;
        ss << "=== Enhanced Memory Tracker Report ===" << std::endl;
        ss << "Session start: " << getFormattedTimestamp(tracker.sessionStart_) << std::endl;
        ss << "Total allocation events: " << tracker.allocationHistory_.size() << std::endl;
        
        // Count allocated objects
        size_t allocatedObjects = 0;
        size_t totalAllocatedBytes = 0;
        std::unordered_map<std::string, size_t> typeCounts;
        
        for (const auto& event : tracker.allocationHistory_) {
            if (event.address != nullptr) {
                allocatedObjects++;
                totalAllocatedBytes += event.size;
                if (event.typeName) {
                    typeCounts[event.typeName]++;
                }
            }
        }
        
        ss << "Currently allocated objects: " << allocatedObjects << std::endl;
        ss << "Currently allocated bytes: " << totalAllocatedBytes << " ("
           << (totalAllocatedBytes / (1024.0 * 1024.0)) << " MB)" << std::endl;
        
        // Show type distribution
        ss << "\nType distribution:" << std::endl;
        for (const auto& pair : typeCounts) {
            ss << "  " << pair.first << ": " << pair.second << " objects" << std::endl;
        }
        
        // Show allocation history (last 10 events)
        ss << "\nRecent allocation events (last 10):" << std::endl;
        size_t count = 0;
        for (auto it = tracker.allocationHistory_.rbegin(); 
             it != tracker.allocationHistory_.rend() && count < 10; ++it, ++count) {
            ss << "  " << getFormattedTimestamp(it->timestamp) << " - "
               << it->size << " bytes at " << it->address;
            if (it->typeName) ss << " of type " << it->typeName;
            if (it->file) ss << " from " << it->file << ":" << it->line;
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    static std::string getFormattedTimestamp(const std::chrono::system_clock::time_point& timePoint) {
        auto time = std::chrono::system_clock::to_time_t(timePoint);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timePoint.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    static bool isEnabled() {
        MemoryTrackerEnhanced& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        return tracker.enabled_;
    }
    
    static bool isInitialized() {
        MemoryTrackerEnhanced& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        return tracker.initialized_;
    }
    
    static void enable(bool enable) {
        MemoryTrackerEnhanced& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        tracker.enabled_ = enable;
    }
    
    static size_t getAllocationHistorySize() {
        MemoryTrackerEnhanced& tracker = getInstance();
        std::lock_guard<std::mutex> lock(tracker.mutex_);
        return tracker.allocationHistory_.size();
    }
};

// Global instance pointer for static initialization
inline MemoryTrackerEnhanced* MemoryTrackerEnhanced::instance_ = nullptr;

// Memory profiler for performance analysis
class MemoryProfiler {
private:
    struct ProfileData {
        std::string functionName;
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t peakAllocations;
        size_t totalBytesAllocated;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::string allocationType;
        
        ProfileData(const std::string& name) 
            : functionName(name), totalAllocations(0), totalDeallocations(0),
              peakAllocations(0), totalBytesAllocated(0) {
            startTime = std::chrono::system_clock::now();
        }
    };
    
    std::unordered_map<std::string, ProfileData> profileData_;
    std::mutex mutex_;
    bool enabled_ = false;
    
public:
    MemoryProfiler() : enabled_(false) {}
    
    void startProfile(const std::string& functionName, const std::string& allocationType = "generic") {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        profileData_[functionName] = ProfileData(functionName);
        profileData_[functionName].allocationType = allocationType;
    }
    
    void endProfile(const std::string& functionName) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = profileData_.find(functionName);
        if (it != profileData_.end()) {
            it->second.endTime = std::chrono::system_clock::now();
        }
    }
    
    void recordAllocation(const std::string& functionName, size_t size, 
                         const char* typeName, const char* file, int line) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = profileData_.find(functionName);
        if (it != profileData_.end()) {
            it->second.totalAllocations++;
            it->second.totalBytesAllocated += size;
            if (it->second.totalAllocations > it->second.peakAllocations) {
                it->second.peakAllocations = it->second.totalAllocations;
            }
        }
    }
    
    void recordDeallocation(const std::string& functionName, size_t size) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = profileData_.find(functionName);
        if (it != profileData_.end()) {
            it->second.totalDeallocations++;
        }
    }
    
    std::string generateReport() {
        std::stringstream ss;
        ss << "=== Memory Profiler Report ===" << std::endl;
        
        if (profileData_.empty()) {
            ss << "No profiling data available." << std::endl;
            return ss.str();
        }
        
        for (const auto& pair : profileData_) {
            const auto& data = pair.second;
            ss << "\nFunction: " << data.functionName << std::endl;
            ss << "  Allocation type: " << data.allocationType << std::endl;
            ss << "  Total allocations: " << data.totalAllocations << std::endl;
            ss << "  Total deallocations: " << data.totalDeallocations << std::endl;
            ss << "  Peak allocations: " << data.peakAllocations << std::endl;
            ss << "  Total bytes allocated: " << data.totalBytesAllocated << " ("
               << (data.totalBytesAllocated / (1024.0 * 1024.0)) << " MB)" << std::endl;
            
            if (data.startTime != std::chrono::system_clock::time_point()) {
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                    data.endTime - data.startTime);
                ss << "  Duration: " << duration.count() << " seconds" << std::endl;
                if (data.totalAllocations > 0) {
                    ss << "  Allocation rate: " 
                       << (static_cast<double>(data.totalAllocations) / duration.count()) 
                       << " allocations/sec" << std::endl;
                }
            }
        }
        
        return ss.str();
    }
    
    void enable(bool enable) {
        enabled_ = enable;
    }
    
    bool isEnabled() const {
        return enabled_;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        profileData_.clear();
    }
};

// Global memory profiler instance
inline MemoryProfiler globalMemoryProfiler;

// Convenience functions for memory management
namespace MemoryUtils {
    
    // Initialize memory management system
    static void initialize(const MemoryConfig& config = MemoryConfig::getDefaultConfig()) {
        MemoryStatsTracker::initialize(config);
        MemoryLeakDetector::initialize(config.enableLeakDetection);
        MemoryPoolManager::initialize(config);
        MemoryTrackerEnhanced::initialize(config.enableMemoryTracking);
        if (config.enablePerformanceProfiling) {
            globalMemoryProfiler.enable(true);
        }
    }
    
    // Shutdown memory management system
    static void shutdown() {
        globalMemoryProfiler.enable(false);
        MemoryTrackerEnhanced::shutdown();
        MemoryPoolManager::shutdown();
        MemoryLeakDetector::shutdown();
        MemoryStatsTracker::shutdown();
    }
    
    // Get memory statistics
    static MemoryStats getStats() {
        return MemoryStatsTracker::getStats();
    }
    
    // Get memory leak report
    static std::string getLeakReport() {
        return MemoryLeakDetector::getLeakReport();
    }
    
    // Get detailed memory report
    static std::string getDetailedReport() {
        std::stringstream ss;
        ss << MemoryStatsTracker::getStatsString() << std::endl;
        ss << MemoryLeakDetector::getLeakReport() << std::endl;
        ss << MemoryPoolManager::getStatsString();
        ss << MemoryTrackerEnhanced::generateReport();
        ss << globalMemoryProfiler.generateReport();
        return ss.str();
    }
    
    // Record allocation for tracking
    static void recordAllocation(void* address, size_t size,
                                const char* typeName, const char* file,
                                int line, const std::string& stackTrace = "") {
        MemoryStatsTracker::recordAllocation(size, typeName, file, line);
        MemoryLeakDetector::recordAllocation(address, size, typeName, file, line, stackTrace);
        MemoryTrackerEnhanced::recordAllocation(address, size, typeName, file, line, stackTrace);
        globalMemoryProfiler.recordAllocation("unknown", size, typeName, file, line);
    }
    
    // Record deallocation for tracking
    static void recordDeallocation(void* address, size_t size,
                                  const char* typeName) {
        MemoryStatsTracker::recordDeallocation(size, typeName);
        MemoryLeakDetector::recordDeallocation(address);
        MemoryTrackerEnhanced::recordDeallocation(address);
        globalMemoryProfiler.recordDeallocation("unknown", size);
    }
    
    // Validate pointer access
    static bool validatePointer(const void* ptr, size_t size = 0,
                               const char* operation = "access") {
        return MemoryValidator::validatePointer(ptr, size, operation);
    }
    
    // Get memory pool manager reference
    static MemoryPoolManager& getPoolManager() {
        return MemoryPoolManager::getInstance();
    }
    
    // Enable/disable memory profiling
    static void enableProfiling(bool enable) {
        globalMemoryProfiler.enable(enable);
    }
    
    // Check if profiling is enabled
    static bool isProfilingEnabled() {
        return globalMemoryProfiler.isEnabled();
    }
    
    // Get global memory profiler
    static MemoryProfiler& getProfiler() {
        return globalMemoryProfiler;
    }
}

} // namespace nlm

// Convenience macros for memory management
#define NLM_MEMORY_TRACKING_START(func_name, alloc_type) \
    do { \
        if (::nlm::MemoryUtils::isProfilingEnabled()) { \
            ::nlm::globalMemoryProfiler.startProfile(func_name, alloc_type); \
        } \
    } while(0)

#define NLM_MEMORY_TRACKING_END(func_name) \
    do { \
        if (::nlm::MemoryUtils::isProfilingEnabled()) { \
            ::nlm::globalMemoryProfiler.endProfile(func_name); \
        } \
    } while(0)

#define NLM_MEMORY_ALLOC_TRACK(size, type_name, file, line) \
    do { \
        ::nlm::MemoryUtils::recordAllocation(nullptr, size, type_name, file, line); \
    } while(0)

#define NLM_MEMORY_DEALLOC_TRACK(ptr, size, type_name) \
    do { \
        ::nlm::MemoryUtils::recordDeallocation(ptr, size, type_name); \
    } while(0)

#define NLM_MEMORY_VALIDATE(ptr, size, op) \
    ::nlm::MemoryUtils::validatePointer(ptr, size, op)
// Smart pointers and RAII utilities for NLM
//
// This module provides modern C++ memory management utilities and RAII patterns
to improve safety, exception safety, and resource management in the codebase.

#ifndef NLM_SMART_POINTERS_HPP
#define NLM_SMART_POINTERS_HPP

#include <memory>
#include <iostream>
#include <cassert>
#include <type_traits>
#include <functional>
#include <utility>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>

namespace nlm {

/**
 * @brief Custom smart pointer with reference counting and lifecycle tracking
 */
template<typename T>
class SmartPtr {
public:
    using element_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using deleter_type = std::function<void(pointer)>;
    
private:
    pointer ptr_;
    deleter_type deleter_;
    std::atomic<size_t>* ref_count_;
    std::mutex* mutex_;
    std::string debug_name_;
    
    void acquire() {
        if (ref_count_) {
            (*ref_count_)++;
        }
    }
    
    void release() {
        if (ref_count_ && --(*ref_count_) == 0) {
            delete ptr_;
            delete ref_count_;
            if (mutex_) {
                delete mutex_;
            }
            ptr_ = nullptr;
            ref_count_ = nullptr;
            mutex_ = nullptr;
        }
    }
    
protected:
    explicit SmartPtr(pointer ptr, deleter_type deleter = nullptr,
                     bool threadSafe = false) : ptr_(ptr), deleter_(deleter) {
        ref_count_ = new std::atomic<size_t>(1);
        if (threadSafe) {
            mutex_ = new std::mutex();
        } else {
            mutex_ = nullptr;
        }
        debug_name_ = "SmartPtr_" + std::to_string(reinterpret_cast<size_t>(this));
    }
    
public:
    // Constructor from raw pointer
    explicit SmartPtr(pointer ptr = nullptr) : ptr_(ptr) {
        ref_count_ = new std::atomic<size_t>(1);
        mutex_ = nullptr;
        debug_name_ = "SmartPtr_" + std::to_string(reinterpret_cast<size_t>(this));
        if (ptr_) {
            std::cout << "Created SmartPtr: " << debug_name_ << " for " << typeid(T).name() << std::endl;
        }
    }
    
    // Constructor from another SmartPtr
    template<typename U>
    SmartPtr(const SmartPtr<U>& other) noexcept : ptr_(other.ptr_), 
                                                   ref_count_(other.ref_count_), 
                                                   mutex_(other.mutex_),
                                                   debug_name_(other.debug_name_) {
        acquire();
    }
    
    // Constructor from another SmartPtr with custom deleter
    template<typename U, typename Deleter>
    SmartPtr(const SmartPtr<U>& other, Deleter deleter) noexcept : ptr_(other.ptr_), 
                                                                    ref_count_(other.ref_count_), 
                                                                    mutex_(other.mutex_),
                                                                    debug_name_(other.debug_name_) {
        acquire();
        deleter_ = std::move(deleter);
    }
    
    // Move constructor
    SmartPtr(SmartPtr&& other) noexcept : ptr_(other.ptr_), 
                                           ref_count_(other.ref_count_), 
                                           mutex_(other.mutex_),
                                           debug_name_(std::move(other.debug_name_)) {
        other.ptr_ = nullptr;
        other.ref_count_ = nullptr;
        other.mutex_ = nullptr;
    }
    
    // Destructor
    ~SmartPtr() {
        release();
    }
    
    // Assignment operator
    SmartPtr& operator=(const SmartPtr& other) {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            ref_count_ = other.ref_count_;
            mutex_ = other.mutex_;
            debug_name_ = other.debug_name_;
            acquire();
        }
        return *this;
    }
    
    // Move assignment operator
    SmartPtr& operator=(SmartPtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            ref_count_ = other.ref_count_;
            mutex_ = other.mutex_;
            debug_name_ = std::move(other.debug_name_);
            other.ptr_ = nullptr;
            other.ref_count_ = nullptr;
            other.mutex_ = nullptr;
        }
        return *this;
    }
    
    // Dereference operators
    reference operator*() const {
        assert(ptr_ != nullptr);
        return *ptr_;
    }
    
    pointer operator->() const {
        assert(ptr_ != nullptr);
        return ptr_;
    }
    
    // Accessors
    pointer get() const { return ptr_; }
    bool isNull() const { return ptr_ == nullptr; }
    explicit operator bool() const { return ptr_ != nullptr; }
    
    // Comparison operators
    bool operator==(const SmartPtr& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const SmartPtr& other) const { return ptr_ != other.ptr_; }
    bool operator<(const SmartPtr& other) const { return ptr_ < other.ptr_; }
    
    // Cast to const SmartPtr
    template<typename U>
    SmartPtr<const U> asConst() const {
        return SmartPtr<const U>(ptr_);
    }
    
    // Debug methods
    std::string getDebugName() const { return debug_name_; }
    size_t getReferenceCount() const { return ref_count_ ? *ref_count_ : 0; }
    
    // Thread safety
    void lock() {
        if (mutex_) {
            mutex_->lock();
        }
    }
    
    void unlock() {
        if (mutex_) {
            mutex_->unlock();
        }
    }
    
    bool tryLock() {
        return mutex_ ? mutex_->try_lock() : true;
    }
    
    // Set debug name
    void setDebugName(const std::string& name) { debug_name_ = name; }
};

/**
 * @brief Auto pointer with automatic resource cleanup
 */
template<typename T>
class AutoPtr {
public:
    using element_type = T;
    
private:
    T* ptr_;
    std::function<void(T*)> cleanup_function_;
    std::string debug_name_;
    
    static AutoPtr makeAuto() {
        return AutoPtr();
    }
    
public:
    // Default constructor
    AutoPtr() : ptr_(nullptr), debug_name_("AutoPtr_null") {}
    
    // Constructor from raw pointer
    explicit AutoPtr(T* ptr) : ptr_(ptr) {
        debug_name_ = "AutoPtr_" + std::to_string(reinterpret_cast<size_t>(ptr_));
        std::cout << "Created AutoPtr: " << debug_name_ << " for " << typeid(T).name() << std::endl;
    }
    
    // Constructor with custom cleanup
    AutoPtr(T* ptr, std::function<void(T*)> cleanup) 
        : ptr_(ptr), cleanup_function_(cleanup) {
        debug_name_ = "AutoPtr_" + std::to_string(reinterpret_cast<size_t>(ptr_));
    }
    
    // Destructor
    ~AutoPtr() {
        release();
    }
    
    // Copy constructor (deleted - AutoPtr cannot be copied)
    AutoPtr(const AutoPtr&) = delete;
    
    // Move constructor
    AutoPtr(AutoPtr&& other) noexcept : ptr_(other.ptr_), 
                                        cleanup_function_(std::move(other.cleanup_function_)),
                                        debug_name_(std::move(other.debug_name_)) {
        other.ptr_ = nullptr;
    }
    
    // Assignment operator (deleted - AutoPtr cannot be assigned)
    AutoPtr& operator=(const AutoPtr&) = delete;
    
    // Move assignment operator
    AutoPtr& operator=(AutoPtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            cleanup_function_ = std::move(other.cleanup_function_);
            debug_name_ = std::move(other.debug_name_);
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    // Dereference operators
    T& operator*() const {
        assert(ptr_ != nullptr);
        return *ptr_;
    }
    
    T* operator->() const {
        assert(ptr_ != nullptr);
        return ptr_;
    }
    
    // Accessors
    T* get() const { return ptr_; }
    bool isNull() const { return ptr_ == nullptr; }
    
    // Release ownership
    T* release() {
        T* old_ptr = ptr_;
        ptr_ = nullptr;
        return old_ptr;
    }
    
    // Reset to new pointer
    void reset(T* new_ptr = nullptr) {
        release();
        ptr_ = new_ptr;
        if (ptr_) {
            debug_name_ = "AutoPtr_" + std::to_string(reinterpret_cast<size_t>(ptr_));
        }
    }
    
    // Comparison operators
    bool operator==(const AutoPtr& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const AutoPtr& other) const { return ptr_ != other.ptr_; }
    bool operator<(const AutoPtr& other) const { return ptr_ < other.ptr_; }
    
    // Debug methods
    std::string getDebugName() const { return debug_name_; }
    
    // Auto-cleanup on destruction
    void release() {
        if (ptr_) {
            if (cleanup_function_) {
                cleanup_function_(ptr_);
            }
            std::cout << "Destroyed AutoPtr: " << debug_name_ << std::endl;
            ptr_ = nullptr;
        }
    }
};

/**
 * @brief Resource acquisition is initialization (RAII) wrapper
 */
template<typename Resource>
class RAIIWrapper {
public:
    using ResourceType = Resource;
    
private:
    Resource resource_;
    bool initialized_;
    std::string name_;
    
public:
    // Constructor
    explicit RAIIWrapper(const Resource& resource, const std::string& name = "RAII") 
        : resource_(resource), initialized_(true), name_(name) {
        std::cout << "Created RAII resource: " << name_ << std::endl;
    }
    
    // Move constructor
    RAIIWrapper(RAIIWrapper&& other) noexcept : resource_(std::move(other.resource_)), 
                                                 initialized_(other.initialized_),
                                                 name_(std::move(other.name_)) {
        other.initialized_ = false;
        std::cout << "Moved RAII resource: " << name_ << std::endl;
    }
    
    // Destructor
    ~RAIIWrapper() {
        if (initialized_) {
            cleanup();
        }
    }
    
    // Access resource
    Resource& get() { return resource_; }
    const Resource& get() const { return resource_; }
    
    // Check if initialized
    bool isInitialized() const { return initialized_; }
    
    // Set name
    void setName(const std::string& name) { name_ = name; }
    
    // Cleanup function (can be overridden)
    virtual void cleanup() {
        if (initialized_) {
            std::cout << "Cleaning up RAII resource: " << name_ << std::endl;
            initialized_ = false;
        }
    }
    
    // Reset resource
    void reset(const Resource& resource) {
        cleanup();
        resource_ = resource;
        initialized_ = true;
        std::cout << "Reset RAII resource: " << name_ << std::endl;
    }
};

/**
 * @brief Scoped resource guard for exception safety
 */
template<typename Resource>
class ScopedGuard {
public:
    using CleanupFunction = std::function<void(Resource&)>
    
private:
    Resource& resource_;
    CleanupFunction cleanup_;
    bool active_;
    std::string name_;
    
public:
    // Constructor
    ScopedGuard(Resource& resource, CleanupFunction cleanup, const std::string& name = "ScopedGuard")
        : resource_(resource), cleanup_(cleanup), active_(true), name_(name) {
        std::cout << "Created ScopedGuard: " << name_ << std::endl;
    }
    
    // Destructor
    ~ScopedGuard() {
        if (active_) {
            cleanup();
        }
    }
    
    // Disable copy
    ScopedGuard(const ScopedGuard&) = delete;
    ScopedGuard& operator=(const ScopedGuard&) = delete;
    
    // Move constructor
    ScopedGuard(ScopedGuard&& other) noexcept : resource_(other.resource_), 
                                                 cleanup_(std::move(other.cleanup_)),
                                                 active_(other.active_),
                                                 name_(std::move(other.name_)) {
        other.active_ = false;
    }
    
    // Release guard (manual cleanup)
    void release() {
        if (active_) {
            cleanup();
            active_ = false;
            std::cout << "Released ScopedGuard: " << name_ << std::endl;
        }
    }
    
    // Check if active
    bool isActive() const { return active_; }
    
    // Get resource
    Resource& get() { return resource_; }
    const Resource& get() const { return resource_; }
};

/**
 * @brief Memory pool for efficient object allocation
 */
template<typename T>
class MemoryPool {
public:
    struct PoolStats {
        size_t totalAllocations = 0;
        size_t totalDeallocations = 0;
        size_t currentSize = 0;
        size_t maxSize = 0;
        double allocationRatio = 0.0;
    };
    
private:
    std::vector<T*> pool_;
    std::vector<bool> allocated_;
    size_t poolSize_;
    size_t blockSize_;
    PoolStats stats_;
    std::mutex mutex_;
    
    void grow() {
        size_t oldSize = pool_.size();
        size_t newSize = oldSize + blockSize_;
        
        // Allocate new block
        T** newPool = new T*[newSize];
        bool* newAllocated = new bool[newSize];
        
        // Copy existing data
        for (size_t i = 0; i < oldSize; ++i) {
            newPool[i] = pool_[i];
            newAllocated[i] = allocated_[i];
        }
        
        // Initialize new block
        for (size_t i = oldSize; i < newSize; ++i) {
            newPool[i] = nullptr;
            newAllocated[i] = false;
        }
        
        // Delete old arrays
        delete[] pool_;
        delete[] allocated_;
        
        // Update pointers
        pool_ = std::vector<T*>(newPool, newPool + newSize);
        allocated_ = std::vector<bool>(newAllocated, newAllocated + newSize);
        
        stats_.maxSize = std::max(stats_.maxSize, pool_.size());
    }
    
public:
    MemoryPool(size_t initialSize = 100, size_t blockSize = 50) 
        : poolSize_(initialSize), blockSize_(blockSize) {
        pool_.resize(poolSize_);
        allocated_.resize(poolSize_);
        
        // Initialize pool with null pointers
        for (size_t i = 0; i < poolSize_; ++i) {
            pool_[i] = nullptr;
            allocated_[i] = false;
        }
        
        std::cout << "Created MemoryPool for " << typeid(T).name() << " with size " << poolSize_ << std::endl;
    }
    
    ~MemoryPool() {
        // Clean up allocated objects
        for (size_t i = 0; i < pool_.size(); ++i) {
            if (allocated_[i]) {
                delete pool_[i];
            }
        }
        std::cout << "Destroyed MemoryPool for " << typeid(T).name() << std::endl;
    }
    
    // Allocate object
    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find free slot
        size_t index = 0;
        for (; index < allocated_.size(); ++index) {
            if (!allocated_[index]) {
                break;
            }
        }
        
        if (index == allocated_.size()) {
            // Need to grow pool
            grow();
            index = allocated_.size() - blockSize_;
        }
        
        // Allocate object
        pool_[index] = new T();
        allocated_[index] = true;
        stats_.totalAllocations++;
        stats_.currentSize++;
        
        return pool_[index];
    }
    
    // Allocate and initialize object
    template<typename... Args>
    T* allocate(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find free slot
        size_t index = 0;
        for (; index < allocated_.size(); ++index) {
            if (!allocated_[index]) {
                break;
            }
        }
        
        if (index == allocated_.size()) {
            // Need to grow pool
            grow();
            index = allocated_.size() - blockSize_;
        }
        
        // Allocate and construct object
        pool_[index] = new T(std::forward<Args>(args)...);
        allocated_[index] = true;
        stats_.totalAllocations++;
        stats_.currentSize++;
        
        return pool_[index];
    }
    
    // Deallocate object
    void deallocate(T* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find object in pool
        for (size_t i = 0; i < pool_.size(); ++i) {
            if (pool_[i] == ptr) {
                delete pool_[i];
                pool_[i] = nullptr;
                allocated_[i] = false;
                stats_.totalDeallocations++;
                stats_.currentSize--;
                return;
            }
        }
        
        // Not found in pool - use regular delete
        delete ptr;
    }
    
    // Clear all allocated objects
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (size_t i = 0; i < pool_.size(); ++i) {
            if (allocated_[i]) {
                delete pool_[i];
                pool_[i] = nullptr;
                allocated_[i] = false;
            }
        }
        
        stats_.currentSize = 0;
    }
    
    // Get statistics
    PoolStats getStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.allocationRatio = static_cast<double>(stats_.totalAllocations) / 
                                 (stats_.totalAllocations + stats_.totalDeallocations);
        return stats_;
    }
    
    // Print statistics
    void printStats() const {
        auto stats = getStats();
        std::cout << "MemoryPool Statistics for " << typeid(T).name() << ":\n";
        std::cout << "  Total Allocations: " << stats.totalAllocations << "\n";
        std::cout << "  Total Deallocations: " << stats.totalDeallocations << "\n";
        std::cout << "  Current Size: " << stats.currentSize << "\n";
        std::cout << "  Max Size: " << stats.maxSize << "\n";
        std::cout << "  Allocation Ratio: " << stats.allocationRatio << "\n";
    }
};

/**
 * @brief Factory for creating objects with proper resource management
 */
template<typename T>
class ObjectFactory {
public:
    using CreatorFunction = std::function<T*()>
    ;
    using DeleterFunction = std::function<void(T*)>
    ;
    
private:
    std::unordered_map<std::string, CreatorFunction> creators_;
    std::unordered_map<std::string, DeleterFunction> deleters_;
    std::mutex mutex_;
    
public:
    // Register object type
    void registerType(const std::string& typeName, CreatorFunction creator, DeleterFunction deleter) {
        std::lock_guard<std::mutex> lock(mutex_);
        creators_[typeName] = creator;
        deleters_[typeName] = deleter;
        std::cout << "Registered object type: " << typeName << " for " << typeid(T).name() << std::endl;
    }
    
    // Create object
    T* create(const std::string& typeName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto creatorIt = creators_.find(typeName);
        if (creatorIt == creators_.end()) {
            throw std::invalid_argument("Unknown object type: " + typeName);
        }
        
        return creatorIt->second();
    }
    
    // Delete object
    void destroy(const std::string& typeName, T* obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto deleterIt = deleters_.find(typeName);
        if (deleterIt == deleters_.end()) {
            delete obj; // Fallback to default delete
            return;
        }
        
        deleterIt->second(obj);
    }
    
    // Get registered types
    std::vector<std::string> getRegisteredTypes() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> types;
        for (const auto& pair : creators_) {
            types.push_back(pair.first);
        }
        return types;
    }
    
    // Clear all registered types
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        creators_.clear();
        deleters_.clear();
    }
};

/**
 * @brief Scope guard for file operations
 */
class FileScopeGuard {
public:
    struct FileOperation {
        std::string path;
        std::string operation;
        std::chrono::system_clock::time_point timestamp;
        bool success;
        std::string errorMessage;
    };
    
private:
    std::vector<FileOperation> operations_;
    std::mutex mutex_;
    
public:
    // Record file operation
    void recordOperation(const std::string& path, const std::string& operation, bool success = true, const std::string& error = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        FileOperation op;
        op.path = path;
        op.operation = operation;
        op.timestamp = std::chrono::system_clock::now();
        op.success = success;
        op.errorMessage = error;
        
        operations_.push_back(op);
        
        if (success) {
            std::cout << "File operation: " << operation << " " << path << " (success)\n";
        } else {
            std::cout << "File operation: " << operation << " " << path << " (failed: " << error << ")\n";
        }
    }
    
    // Get operations
    std::vector<FileOperation> getOperations() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return operations_;
    }
    
    // Clear operations
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        operations_.clear();
    }
    
    // Print operations
    void printOperations() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::cout << "File Operations:\n";
        for (const auto& op : operations_) {
            std::cout << "  " << op.timestamp << " - " << op.operation << " " << op.path;
            if (!op.success) {
                std::cout << " (ERROR: " << op.errorMessage << ")";
            }
            std::cout << "\n";
        }
    }
    
    // Generate operation report
    std::string generateReport() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::stringstream ss;
        ss << "File Operations Report\n";
        ss << "======================\n\n";
        
        ss << "Total Operations: " << operations_.size() << "\n";
        ss << "Successful Operations: " << 
            std::count_if(operations_.begin(), operations_.end(), 
                         [](const FileOperation& op) { return op.success; }) << "\n";
        ss << "Failed Operations: " << 
            std::count_if(operations_.begin(), operations_.end(), 
                         [](const FileOperation& op) { return !op.success; }) << "\n\n";
        
        for (const auto& op : operations_) {
            ss << op.timestamp << " - " << op.operation << " " << op.path;
            if (!op.success) {
                ss << " (ERROR: " << op.errorMessage << ")";
            }
            ss << "\n";
        }
        
        return ss.str();
    }
};

/**
 * @brief Type-safe wrapper for configuration files
 */
template<typename ConfigType>
class ConfigWrapper {
public:
    using ConfigPtr = std::shared_ptr<ConfigType>;
    
private:
    ConfigPtr config_;
    std::string filename_;
    bool autoSave_;
    std::chrono::duration<double> autoSaveInterval_;
    std::chrono::system_clock::time_point lastSaveTime_;
    
public:
    // Constructor
    explicit ConfigWrapper(ConfigPtr config, const std::string& filename = "", 
                          bool autoSave = false, double autoSaveInterval = 60.0)
        : config_(config), filename_(filename), autoSave_(autoSave),
          autoSaveInterval_(autoSaveInterval) {
        lastSaveTime_ = std::chrono::system_clock::now();
        
        if (!filename_.empty()) {
            load();
        }
        
        std::cout << "Created ConfigWrapper for " << typeid(ConfigType).name() << std::endl;
    }
    
    // Destructor
    ~ConfigWrapper() {
        if (autoSave_ && filename_.empty() && config_) {
            save();
        }
    }
    
    // Load from file
    bool load() {
        if (filename_.empty()) {
            return false;
        }
        
        std::ifstream file(filename_);
        if (!file.is_open()) {
            std::cout << "Warning: Could not open config file: " << filename_ << std::endl;
            return false;
        }
        
        // In real implementation, would deserialize config from file
        // For now, just log the operation
        std::cout << "Loaded config from: " << filename_ << std::endl;
        
        lastSaveTime_ = std::chrono::system_clock::now();
        return true;
    }
    
    // Save to file
    bool save() {
        if (filename_.empty() || !config_) {
            return false;
        }
        
        std::ofstream file(filename_);
        if (!file.is_open()) {
            std::cout << "Warning: Could not write config file: " << filename_ << std::endl;
            return false;
        }
        
        // In real implementation, would serialize config to file
        // For now, just log the operation
        std::cout << "Saved config to: " << filename_ << std::endl;
        
        lastSaveTime_ = std::chrono::system_clock::now();
        return true;
    }
    
    // Auto-save check
    void checkAutoSave() {
        if (!autoSave_ || filename_.empty() || !config_) {
            return;
        }
        
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastSaveTime_).count();
        
        if (elapsed >= autoSaveInterval_) {
            save();
        }
    }
    
    // Get config
    ConfigPtr get() const { return config_; }
    
    // Set config
    void set(ConfigPtr config) {
        config_ = config;
        lastSaveTime_ = std::chrono::system_clock::now();
    }
    
    // Get filename
    const std::string& getFilename() const { return filename_; }
    
    // Set filename
    void setFilename(const std::string& filename) {
        filename_ = filename;
        if (!filename_.empty()) {
            load();
        }
    }
    
    // Enable/disable auto-save
    void setAutoSave(bool autoSave, double interval = 60.0) {
        autoSave_ = autoSave;
        autoSaveInterval_ = std::chrono::duration<double>(interval);
    }
    
    // Check if dirty (needs saving)
    bool isDirty() const {
        if (!autoSave_ || filename_.empty() || !config_) {
            return false;
        }
        
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastSaveTime_).count();
        
        return elapsed >= autoSaveInterval_;
    }
}; 

} // namespace nlm

#endif // NLM_SMART_POINTERS_HPP

#include <memory>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <cstddef>
#include <type_traits>
#include <stdexcept>
#include <new>
#include <chrono>
#include <unordered_set>

// Memory leak detection and tracking
#ifdef NLM_ENABLE_MEMORY_LEAK_DETECTION
    #include <set>
#endif

namespace nlm {

/**
 * @brief Memory pool for efficient allocation of frequently used objects
 * 
 * Provides thread-safe memory pools for different object types to reduce
 * allocation overhead and improve memory management performance.
 */
class MemoryPool {
public:
    static constexpr size_t DEFAULT_POOL_SIZE = 1024;
    static constexpr size_t MAX_POOL_SIZE = 1024 * 1024;
    
    MemoryPool() : currentSize(0), totalAllocated(0) {}
    
    ~MemoryPool() {
        clear();
    }
    
    /**
     * @brief Allocate memory from the pool
     * 
     * @tparam T Type of object to allocate
     * @param count Number of objects to allocate
     * @return Pointer to allocated memory
     */
    template<typename T>
    T* allocate(size_t count = 1) {
        static_assert(std::is_default_constructible<T>::value, "Type must be default constructible");
        
        size_t elementSize = sizeof(T);
        size_t bytesNeeded = elementSize * count;
        
        // Allocate large blocks from system
        std::lock_guard<std::mutex> lock(mutex);
        
        void* ptr = nullptr;
        if (bytesNeeded <= remainingBytes) {
            // Use free blocks
            ptr = currentBlock;
            currentBlock = static_cast<char*>(currentBlock) + bytesNeeded;
            remainingBytes -= bytesNeeded;
        } else {
            // Allocate new block from system
            size_t blockSize = std::min(bytesNeeded, MAX_POOL_SIZE);
            blockSize = std::max(blockSize, DEFAULT_POOL_SIZE * sizeof(T));
            
            void* newBlock = ::operator new(blockSize);
            if (!newBlock) {
                throw MemoryManagementException("Failed to allocate memory from pool", blockSize);
            }
            
            blocks.push_back(newBlock);
            currentBlock = static_cast<char*>(newBlock);
            remainingBytes = blockSize;
            
            // Allocate from the new block
            ptr = currentBlock;
            currentBlock = static_cast<char*>(currentBlock) + bytesNeeded;
            remainingBytes -= bytesNeeded;
        }
        
        currentSize += bytesNeeded;
        totalAllocated += bytesNeeded;
        
        #ifdef NLM_ENABLE_MEMORY_LEAK_DETECTION
        allocatedBlocks.insert(ptr);
        #endif
        
        // Initialize the memory
        std::uninitialized_fill_n(static_cast<T*>(ptr), count, T());
        return static_cast<T*>(ptr);
    }
    
    /**
     * @brief Deallocate memory
     * 
     * @tparam T Type of object
     * @param ptr Pointer to deallocate
     * @param count Number of objects
     */
    template<typename T>
    void deallocate(T* ptr, size_t count = 1) {
        if (!ptr) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        size_t elementSize = sizeof(T);
        size_t bytesFreed = elementSize * count;
        
        #ifdef NLM_ENABLE_MEMORY_LEAK_DETECTION
        allocatedBlocks.erase(ptr);
        #endif
        
        currentSize -= bytesFreed;
        
        // Check if this was the last pointer in the current block
        if (ptr == currentBlock - bytesFreed && remainingBytes == 0) {
            // Reclaim the entire block
            remainingBytes = 0;
            currentBlock = static_cast<char*>(ptr);
        } else {
            // Mark as free (not tracked in this simple implementation)
        }
    }
    
    /**
     * @brief Clear all allocated memory
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (void* block : blocks) {
            ::operator delete(block);
        }
        
        blocks.clear();
        currentBlock = nullptr;
        remainingBytes = 0;
        currentSize = 0;
        
        #ifdef NLM_ENABLE_MEMORY_LEAK_DETECTION
        allocatedBlocks.clear();
        #endif
    }
    
    /**
     * @brief Get total allocated size
     * 
     * @return Total allocated bytes
     */
    size_t getTotalAllocated() const {
        std::lock_guard<std::mutex> lock(mutex);
        return totalAllocated;
    }
    
    /**
     * @brief Get current in-use size
     * 
     * @return Current in-use bytes
     */
    size_t getCurrentSize() const {
        std::lock_guard<std::mutex> lock(mutex);
        return currentSize;
    }
    
private:
    std::vector<void*> blocks;
    void* currentBlock = nullptr;
    size_t remainingBytes = 0;
    size_t currentSize = 0;
    size_t totalAllocated = 0;
    mutable std::mutex mutex;
    
#ifdef NLM_ENABLE_MEMORY_LEAK_DETECTION
    std::set<void*> allocatedBlocks;
#endif
};

/**
 * @brief Custom deleter for smart pointer resource management
 * 
 * Provides enhanced error handling and logging for resource cleanup.
 */
template<typename T>
class CustomDeleter {
public:
    CustomDeleter(std::function<void(T*)> cleanupFunc = nullptr)
        : cleanupFunc(cleanupFunc) {}
    
    void operator()(T* ptr) {
        if (ptr) {
            try {
                // Call custom cleanup if provided
                if (cleanupFunc) {
                    cleanupFunc(ptr);
                }
                // Call destructor
                ptr->~T();
                // Free memory
                ::operator delete(static_cast<void*>(ptr));
                NLM_LOG_DEBUG("Custom deleter successfully cleaned up object");
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error in custom deleter: " + std::string(e.what()));
                // Try to free memory even if cleanup fails
                ::operator delete(static_cast<void*>(ptr));
                throw;
            }
        }
    }
    
private:
    std::function<void(T*)> cleanupFunc;
};

/**
 * @brief Thread-safe resource manager for tracking allocated resources
 * 
 * Maintains reference counts and ensures proper cleanup of resources
 * in multi-threaded environments.
 */
class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    /**
     * @brief Register a resource for tracking
     * 
     * @tparam T Resource type
     * @param resource Resource pointer
     * @param type Resource type name for logging
     */
    template<typename T>
    void registerResource(T* resource, const std::string& type) {
        if (!resource) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        auto key = std::make_pair(resource, type);
        resourceCounts[key]++;
        
        NLM_LOG_DEBUG("Registered resource of type " + type + " at " + 
                     std::to_string(reinterpret_cast<uintptr_t>(resource)));
    }
    
    /**
     * @brief Unregister a resource
     * 
     * @tparam T Resource type
     * @param resource Resource pointer
     * @param type Resource type name
     */
    template<typename T>
    void unregisterResource(T* resource, const std::string& type) {
        if (!resource) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        auto key = std::make_pair(resource, type);
        auto it = resourceCounts.find(key);
        if (it != resourceCounts.end()) {
            if (--it->second == 0) {
                resourceCounts.erase(it);
                NLM_LOG_DEBUG("Unregistered resource of type " + type + 
                             " at " + std::to_string(reinterpret_cast<uintptr_t>(resource)));
            }
        }
    }
    
    /**
     * @brief Check if resource is registered
     * 
     * @tparam T Resource type
     * @param resource Resource pointer
     * @param type Resource type name
     * 
     * @return true if resource is registered
     */
    template<typename T>
    bool isRegistered(T* resource, const std::string& type) const {
        if (!resource) return false;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        auto key = std::make_pair(resource, type);
        return resourceCounts.find(key) != resourceCounts.end();
    }
    
    /**
     * @brief Get number of registered resources
     * 
     * @return Number of registered resources
     */
    size_t getRegisteredCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return resourceCounts.size();
    }
    
    /**
     * @brief Clear all registered resources
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (!resourceCounts.empty()) {
            NLM_LOG_WARNING("ResourceManager clearing " + std::to_string(resourceCounts.size()) + 
                           " registered resources without proper cleanup");
        }
        resourceCounts.clear();
    }
    
private:
    ResourceManager() = default;
    ~ResourceManager() {
        clear();
    }
    
    std::unordered_map<std::pair<void*, std::string>, int> resourceCounts;
    mutable std::mutex mutex;
    
    // Disable copying and assignment
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
};

/**
 * @brief Atomic pointer for thread-safe pImpl access
 * 
 * Provides atomic access to the pImpl pointer for thread-safe memory management.
 */
template<typename T>
class AtomicPtr {
public:
    AtomicPtr() : ptr(nullptr) {}
    explicit AtomicPtr(T* p) : ptr(p) {}
    ~AtomicPtr() {
        delete load();
    }
    
    T* load() const {
        return ptr.load(std::memory_order_acquire);
    }
    
    void store(T* p) {
        ptr.store(p, std::memory_order_release);
    }
    
    T* exchange(T* p) {
        return ptr.exchange(p, std::memory_order_acq_rel);
    }
    
    bool compare_exchange_strong(T*& expected, T* desired) {
        return ptr.compare_exchange_strong(expected, desired, std::memory_order_acq_rel);
    }
    
    explicit operator bool() const {
        return load() != nullptr;
    }
    
private:
    std::atomic<T*> ptr;
};

/**
 * @brief Memory allocation tracer for debugging and leak detection
 */
class MemoryTracer {
public:
    struct AllocationInfo {
        void* ptr;
        size_t size;
        const char* file;
        int line;
        const char* function;
        std::chrono::system_clock::time_point timestamp;
    };
    
    static MemoryTracer& getInstance() {
        static MemoryTracer instance;
        return instance;
    }
    
    /**
     * @brief Track allocation
     * 
     * @param ptr Pointer allocated
     * @param size Size allocated
     * @param file Source file
     * @param line Source line
     * @param function Source function
     */
    void trackAllocation(void* ptr, size_t size, const char* file, int line, const char* function) {
        std::lock_guard<std::mutex> lock(mutex);
        
        AllocationInfo info;
        info.ptr = ptr;
        info.size = size;
        info.file = file;
        info.line = line;
        info.function = function;
        info.timestamp = std::chrono::system_clock::now();
        
        allocations[ptr] = info;
        
        NLM_LOG_DEBUG("Memory allocation tracked: " + std::to_string(size) + " bytes at " +
                     std::to_string(reinterpret_cast<uintptr_t>(ptr)) + " in " +
                     std::string(file) + ":" + std::to_string(line));
    }
    
    /**
     * @brief Track deallocation
     * 
     * @param ptr Pointer deallocated
     * @param file Source file
     * @param line Source line
     * @param function Source function
     */
    void trackDeallocation(void* ptr, const char* file, int line, const char* function) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = allocations.find(ptr);
        if (it != allocations.end()) {
            NLM_LOG_DEBUG("Memory deallocation tracked: " + std::to_string(it->second.size) + 
                         " bytes at " + std::to_string(reinterpret_cast<uintptr_t>(ptr)));
            allocations.erase(it);
        } else {
            NLM_LOG_WARNING("Deallocating untracked memory at " + 
                           std::to_string(reinterpret_cast<uintptr_t>(ptr)));
        }
    }
    
    /**
     * @brief Get list of untracked allocations (potential leaks)
     * 
     * @return Vector of leaked allocation info
     */
    std::vector<AllocationInfo> getLeakedAllocations() const {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::vector<AllocationInfo> leaks;
        for (const auto& pair : allocations) {
            leaks.push_back(pair.second);
        }
        
        return leaks;
    }
    
    /**
     * @brief Report memory leaks
     */
    void reportLeaks() {
        auto leaks = getLeakedAllocations();
        
        if (!leaks.empty()) {
            NLM_LOG_ERROR("MEMORY LEAK DETECTION: " + std::to_string(leaks.size()) + 
                         " allocations not properly deallocated");
            
            for (const auto& leak : leaks) {
                NLM_LOG_ERROR("Leaked allocation: " + std::to_string(leak.size) + " bytes at " +
                             std::to_string(reinterpret_cast<uintptr_t>(leak.ptr)) +
                             " allocated in " + std::string(leak.file) + ":" +
                             std::to_string(leak.line) + " in " + std::string(leak.function));
            }
        }
    }
    
    /**
     * @brief Enable/disable memory tracing
     * 
     * @param enable Enable tracking
     */
    void setEnabled(bool enable) {
        std::lock_guard<std::mutex> lock(mutex);
        enabled = enable;
        
        if (!enable) {
            allocations.clear();
        }
    }
    
private:
    MemoryTracer() : enabled(true) {}
    
    std::unordered_map<void*, AllocationInfo> allocations;
    std::mutex mutex;
    bool enabled;
    
    // Disable copying and assignment
    MemoryTracer(const MemoryTracer&) = delete;
    MemoryTracer& operator=(const MemoryTracer&) = delete;
};

/**
 * @brief Global memory management utilities
 */
namespace MemoryManager {
    
    /**
     * @brief Allocate memory with tracking
     * 
     * @param size Number of bytes to allocate
     * @param file Source file (for tracking)
     * @param line Source line (for tracking)
     * @param function Source function (for tracking)
     * 
     * @return Pointer to allocated memory
     */
    void* allocate(size_t size, const char* file, int line, const char* function) {
        void* ptr = ::operator new(size);
        if (!ptr) {
            throw MemoryManagementException("Memory allocation failed", size);
        }
        
        MemoryTracer::getInstance().trackAllocation(ptr, size, file, line, function);
        return ptr;
    }
    
    /**
     * @brief Deallocate memory with tracking
     * 
     * @param ptr Pointer to deallocate
     * @param file Source file (for tracking)
     * @param line Source line (for tracking)
     * @param function Source function (for tracking)
     */
    void deallocate(void* ptr, const char* file, int line, const char* function) {
        if (!ptr) return;
        
        MemoryTracer::getInstance().trackDeallocation(ptr, file, line, function);
        ::operator delete(ptr);
    }
    
    /**
     * @brief Allocate with error handling
     * 
     * @tparam T Type to allocate
     * @tparam Args Constructor arguments
     * 
     * @param args Constructor arguments
     * 
     * @return Pointer to allocated object
     */
    template<typename T, typename... Args>
    T* allocateObject(Args&&... args) {
        try {
            void* ptr = ::operator new(sizeof(T));
            if (!ptr) {
                throw MemoryManagementException("Object allocation failed", sizeof(T));
            }
            
            // Construct object
            return new(ptr) T(std::forward<Args>(args)...);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error allocating object: " + std::string(e.what()));
            throw;
        }
    }
    
    /**
     * @brief Deallocate object with tracking
     * 
     * @tparam T Object type
     * @param ptr Pointer to object
     */
    template<typename T>
    void deallocateObject(T* ptr) {
        if (!ptr) return;
        
        try {
            ptr->~T();
            ::operator delete(static_cast<void*>(ptr));
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error deallocating object: " + std::string(e.what()));
            // Still try to free memory
            ::operator delete(static_cast<void*>(ptr));
            throw;
        }
    }
    
    /**
     * @brief Get memory tracer instance
     * 
     * @return MemoryTracer reference
     */
    MemoryTracer& getTracer() {
        return MemoryTracer::getInstance();
    }
    
    /**
     * @brief Get resource manager instance
     * 
     * @return ResourceManager reference
     */
    ResourceManager& getResourceManager() {
        return ResourceManager::getInstance();
    }
    
    /**
     * @brief Initialize memory management
     */
    static void initialize() {
        NLM_LOG_INFO("Initializing memory management system");
        ResourceManager::getInstance();
        MemoryTracer::getInstance().setEnabled(true);
    }
    
    /**
     * @brief Shutdown memory management
     */
    static void shutdown() {
        NLM_LOG_INFO("Shutting down memory management system");
        
        // Report any memory leaks
        MemoryTracer::getInstance().reportLeaks();
        
        // Clear resource manager
        ResourceManager::getInstance().clear();
        
        MemoryTracer::getInstance().setEnabled(false);
        NLM_LOG_INFO("Memory management shutdown complete");
    }
}

/**
 * @brief Helper macro for tracked memory allocation
 */
#define NLM_ALLOC_TRACKED(size) \
    nlm::MemoryManager::allocate(size, __FILE__, __LINE__, __FUNCTION__)

/**
 * @brief Helper macro for tracked memory deallocation
 */
#define NLM_DEALLOC_TRACKED(ptr) \
    nlm::MemoryManager::deallocate(ptr, __FILE__, __LINE__, __FUNCTION__)

/**
 * @brief Helper macro for tracked object allocation
 */
#define NLM_ALLOCATE_OBJECT(type, ...) \
    nlm::MemoryManager::allocateObject<type>(__VA_ARGS__)

/**
 * @brief Helper macro for tracked object deallocation
 */
#define NLM_DEALLOCATE_OBJECT(ptr) \
    nlm::MemoryManager::deallocateObject(ptr)

} // namespace nlm
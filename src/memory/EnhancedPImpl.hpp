#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <optional>

namespace nlm {

// Enhanced base class for pImpl with proper RAII and copy/move support
template<typename Derived, typename ImplType>
class EnhancedPImpl {
public:
    EnhancedPImpl() : pImpl(std::make_unique<ImplType>()) {}
    
    // Copy constructor with deep copy
    EnhancedPImpl(const EnhancedPImpl& other) : pImpl(std::make_unique<ImplType>(*other.pImpl)) {
        static_cast<Derived*>(this)->afterCopyConstruct();
    }
    
    // Move constructor
    EnhancedPImpl(EnhancedPImpl&& other) noexcept : pImpl(std::move(other.pImpl)) {
        static_cast<Derived*>(this)->afterMoveConstruct();
    }
    
    // Copy assignment operator
    EnhancedPImpl& operator=(const EnhancedPImpl& other) {
        if (this != &other) {
            pImpl = std::make_unique<ImplType>(*other.pImpl);
            static_cast<Derived*>(this)->afterCopyAssign();
        }
        return *this;
    }
    
    // Move assignment operator
    EnhancedPImpl& operator=(EnhancedPImpl&& other) noexcept {
        if (this != &other) {
            pImpl = std::move(other.pImpl);
            static_cast<Derived*>(this)->afterMoveAssign();
        }
        return *this;
    }
    
    ~EnhancedPImpl() {
        static_cast<Derived*>(this)->beforeDestroy();
    }
    
    // Access to pImpl
    ImplType& getImpl() { return *pImpl; }
    const ImplType& getImpl() const { return *pImpl; }
    ImplType* getImplPtr() { return pImpl.get(); }
    const ImplType* getImplPtr() const { return pImpl.get(); }
    
    // Swap idiom for efficient copy operations
    void swap(EnhancedPImpl& other) noexcept {
        pImpl.swap(other.pImpl);
    }
    
protected:
    // Hooks for derived classes to override
    virtual void afterCopyConstruct() {}
    virtual void afterMoveConstruct() {}
    virtual void afterCopyAssign() {}
    virtual void afterMoveAssign() {}
    virtual void beforeDestroy() {}
    
private:
    std::unique_ptr<ImplType> pImpl;
};

// Memory tracking wrapper for pImpl implementations
class MemoryTrackedImpl {
public:
    struct AllocationInfo {
        std::string tag;
        size_t size;
        std::string file;
        int line;
        std::string function;
        bool tracked;
        
        AllocationInfo() : size(0), line(0), tracked(false) {}
        AllocationInfo(const std::string& t, size_t s, const std::string& f, 
                      int l, const std::string& func, bool track)
            : tag(t), size(s), file(f), line(l), function(func), tracked(track) {}
    };
    
    virtual ~MemoryTrackedImpl() = default;
    
    virtual void* allocate(size_t size, const std::string& tag, 
                          const std::string& file, int line, 
                          const std::string& function, size_t alignment = 0) {
        // Default implementation uses new
        void* ptr = operator new(size);
        if (trackAllocations_) {
            allocations_.push_back(AllocationInfo(tag, size, file, line, function, true));
        }
        return ptr;
    }
    
    virtual void deallocate(void* ptr, const std::string& tag,
                           const std::string& file, int line,
                           const std::string& function) {
        // Default implementation uses delete
        if (trackAllocations_) {
            // Remove matching allocation (simplified)
            allocations_.erase(
                std::remove_if(allocations_.begin(), allocations_.end(),
                             [ptr](const AllocationInfo& alloc) { return alloc.tracked; }),
                allocations_.end()
            );
        }
        operator delete(ptr);
    }
    
    virtual std::vector<AllocationInfo> getAllocations() const { return allocations_; }
    virtual size_t getTotalAllocated() const {
        size_t total = 0;
        for (const auto& alloc : allocations_) {
            total += alloc.size;
        }
        return total;
    }
    
    static void enableTracking(bool enable) { trackAllocations_ = enable; }
    static bool isTrackingEnabled() { return trackAllocations_; }
    
protected:
    std::vector<AllocationInfo> allocations_;
    static bool trackAllocations_;
};

bool MemoryTrackedImpl::trackAllocations_ = false;

// Enhanced copy constructor with deep copy support
class DeepCopyImpl {
public:
    virtual ~DeepCopyImpl() = default;
    
    virtual void* cloneObject(const void* object, size_t size) const {
        if (!object || size == 0) return nullptr;
        void* clone = operator new(size);
        std::memcpy(clone, object, size);
        return clone;
    }
    
    virtual void freeClonedObject(void* object, size_t size) const {
        if (object) {
            operator delete(object);
        }
    }
};

// Swap idiom implementation helper
class SwapHelper {
public:
    template<typename T>
    static void swap(T& a, T& b) noexcept {
        T temp(std::move(a));
        a = std::move(b);
        b = std::move(temp);
    }
    
    template<typename T, typename... Args>
    static std::unique_ptr<T> makeUnique(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
};

// RAII resource manager
class ResourceManager {
public:
    template<typename Resource, typename... Args>
    static std::unique_ptr<Resource> acquire(Args&&... args) {
        auto resource = std::make_unique<Resource>(std::forward<Args>(args)...);
        acquireResource(resource.get());
        return resource;
    }
    
    template<typename Resource>
    static void release(std::unique_ptr<Resource>& resource) {
        if (resource) {
            releaseResource(resource.get());
            resource.reset();
        }
    }
    
    template<typename Resource>
    static void release(Resource* resource) {
        if (resource) {
            releaseResource(resource);
            delete resource;
        }
    }
    
protected:
    static void acquireResource(void* resource) {}
    static void releaseResource(void* resource) {}
    
    // Subclasses can override these for specific resource management
};

// Memory efficient allocator with pooling
class MemoryPoolAllocator {
public:
    template<typename T>
    static std::vector<T*> allocateBatch(size_t count) {
        std::vector<T*> result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(static_cast<T*>(allocate(sizeof(T), alignof(T))));
        }
        return result;
    }
    
    template<typename T>
    static void deallocateBatch(const std::vector<T*>& ptrs) {
        for (const auto* ptr : ptrs) {
            deallocate(ptr);
        }
    }
    
    static void* allocate(size_t size, size_t alignment = 0) {
        // Use system allocator as default
        if (alignment > 0) {
            return std::aligned_alloc(alignment, size);
        }
        return std::malloc(size);
    }
    
    static void deallocate(void* ptr) {
        std::free(ptr);
    }
    
    static void* allocateWithTag(size_t size, const std::string& tag, 
                               const std::string& file, int line,
                               const std::string& function, size_t alignment = 0) {
        void* ptr = allocate(size, alignment);
        recordAllocation(ptr, size, tag, file, line, function);
        return ptr;
    }
    
    static void deallocateWithTag(void* ptr, const std::string& tag,
                                 const std::string& file, int line,
                                 const std::string& function) {
        recordDeallocation(ptr, tag, file, line, function);
        deallocate(ptr);
    }
    
private:
    static void recordAllocation(void* ptr, size_t size, const std::string& tag,
                                const std::string& file, int line,
                                const std::string& function) {}
    
    static void recordDeallocation(void* ptr, const std::string& tag,
                                  const std::string& file, int line,
                                  const std::string& function) {}
};

// Exception-safe resource wrapper
class ExceptionSafeResource {
public:
    template<typename T, typename... Args>
    static std::unique_ptr<T> makeExceptionSafe(Args&&... args) {
        try {
            auto resource = std::make_unique<T>(std::forward<Args>(args)...);
            onResourceCreated(resource.get());
            return resource;
        } catch (...) {
            onResourceCreationFailed();
            throw;
        }
    }
    
protected:
    static void onResourceCreated(void* resource) {}
    static void onResourceCreationFailed() {}
};

// Concurrency-safe memory operations
class ThreadSafeMemory {
public:
    static void* allocate(size_t size, size_t alignment = 0) {
        std::lock_guard<std::mutex> lock(allocationMutex_);
        if (alignment > 0) {
            return std::aligned_alloc(alignment, size);
        }
        return std::malloc(size);
    }
    
    static void deallocate(void* ptr) {
        std::lock_guard<std::mutex> lock(deallocationMutex_);
        std::free(ptr);
    }
    
private:
    static std::mutex allocationMutex_;
    static std::mutex deallocationMutex_;
};

std::mutex ThreadSafeMemory::allocationMutex_;
std::mutex ThreadSafeMemory::deallocationMutex_;

// Memory statistics collector
class MemoryStatistics {
public:
    struct Stats {
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t currentAllocations;
        size_t peakAllocations;
        size_t totalMemoryAllocated;
        size_t totalMemoryDeallocated;
        size_t currentMemoryUsage;
        size_t peakMemoryUsage;
        double fragmentationRatio;
        std::unordered_map<std::string, size_t> allocationsByTag;
        std::unordered_map<std::string, size_t> deallocationsByTag;
        
        Stats() : totalAllocations(0), totalDeallocations(0), currentAllocations(0),
                 peakAllocations(0), totalMemoryAllocated(0), totalMemoryDeallocated(0),
                 currentMemoryUsage(0), peakMemoryUsage(0), fragmentationRatio(0.0) {}
        
        std::string report() const;
    };
    
    static void recordAllocation(size_t size, const std::string& tag = "DEFAULT");
    static void recordDeallocation(size_t size, const std::string& tag = "DEFAULT");
    static Stats getStats();
    static void reset();
    static std::string generateReport();
    
private:
    static Stats stats_;
    static std::mutex mutex_;
};

// Memory leak detector
class MemoryLeakDetector {
public:
    struct LeakInfo {
        void* pointer;
        size_t size;
        std::string tag;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point allocationTime;
        
        LeakInfo() : pointer(nullptr), size(0), line(0) {}
        LeakInfo(void* ptr, size_t s, const std::string& t, const std::string& f,
                int l, const std::string& func)
            : pointer(ptr), size(s), tag(t), file(f), line(l), function(func),
              allocationTime(std::chrono::system_clock::now()) {}
    };
    
    static std::vector<LeakInfo> detectLeaks();
    static std::vector<LeakInfo> getLeaksByTag(const std::string& tag);
    static std::vector<LeakInfo> getLeaksByFile(const std::string& file);
    static std::vector<LeakInfo> getLeaksByFunction(const std::string& function);
    static size_t getTotalLeakSize();
    static std::string generateLeakReport();
    static void clearLeaks();
    
    // Configuration
    static void setMinLeakSize(size_t size) { minLeakSize_ = size; }
    static void setMaxLeaksToReport(size_t maxLeaks) { maxLeaksToReport_ = maxLeaks; }
    static void enableStackTraces(bool enable) { captureStackTraces_ = enable; }
    
private:
    static std::vector<LeakInfo> leaks_;
    static std::mutex mutex_;
    static size_t minLeakSize_;
    static size_t maxLeaksToReport_;
    static bool captureStackTraces_;
};

// Memory validation
class MemoryValidator {
public:
    enum class ValidationResult {
        PASS,
        FAIL,
        WARNING,
        ERROR
    };
    
    struct ValidationReport {
        ValidationResult result;
        std::string message;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::chrono::system_clock::time_point timestamp;
        
        ValidationReport() : result(ValidationResult::PASS), timestamp(std::chrono::system_clock::now()) {}
        std::string report() const;
    };
    
    static ValidationReport validateMemoryLeaks();
    static ValidationReport validateMemoryCorruption();
    static ValidationReport validateAlignment();
    static ValidationReport validateBufferOverflows();
    static ValidationReport validateUseAfterFree();
    static ValidationReport validateDoubleFree();
    static ValidationReport validateMemoryLeaks();
    
    static void enableStrictValidation(bool enable) { strictValidation_ = enable; }
    static bool isStrictValidationEnabled() { return strictValidation_; }
    
private:
    static bool strictValidation_;
};

// Memory optimization
class MemoryOptimizer {
public:
    struct OptimizationResult {
        std::string optimization;
        double memorySavings;
        double performanceGain;
        std::string implementationDetails;
        
        OptimizationResult() : memorySavings(0.0), performanceGain(0.0) {}
    };
    
    static std::vector<OptimizationResult> analyzeAndOptimize();
    static std::vector<OptimizationResult> getPoolRecommendations();
    static std::vector<OptimizationResult> getReuseRecommendations();
    static std::vector<OptimizationResult> getAllocationPatternRecommendations();
    static std::string generateOptimizationReport();
    
    // Configuration
    static void setMaxMemoryUsage(double maxMB) { maxMemoryUsageMB_ = maxMB; }
    static void setOptimizationLevel(int level) { optimizationLevel_ = level; }
    
private:
    static double maxMemoryUsageMB_;
    static int optimizationLevel_;
};

// Memory monitoring
class MemoryMonitor {
public:
    struct MemoryUsage {
        double systemMemoryUsedMB;
        double systemMemoryAvailableMB;
        double systemMemoryTotalMB;
        double processMemoryUsedMB;
        double processMemoryPeakMB;
        size_t allocationCount;
        size_t deallocationCount;
        double memoryGrowthRateMBPerSec;
        std::chrono::system_clock::time_point lastUpdate;
        
        MemoryUsage() : systemMemoryUsedMB(0), systemMemoryAvailableMB(0),
                       systemMemoryTotalMB(0), processMemoryUsedMB(0),
                       processMemoryPeakMB(0), allocationCount(0), deallocationCount(0),
                       memoryGrowthRateMBPerSec(0), lastUpdate(std::chrono::system_clock::now()) {}
    };
    
    static void startMonitoring(double intervalSeconds = 1.0);
    static void stopMonitoring();
    static bool isMonitoring();
    static MemoryUsage getCurrentUsage();
    static double getMemoryUsageMB();
    static size_t getAllocationCount();
    static size_t getDeallocationCount();
    static bool checkMemoryPressure(double thresholdMB = 1024.0);
    static std::vector<std::string> getAlerts();
    static std::string generateUsageReport();
    static std::string generateAlertReport();
    
    static void integrateWithMemoryTracker();
    
private:
    static bool monitoring_;
    static double monitoringInterval_;
    static MemoryUsage currentUsage_;
    static std::chrono::system_clock::time_point lastReportTime_;
    static std::vector<std::string> alerts_;
    static std::mutex mutex_;
};

// Main memory management system
class MemoryManagementSystem {
public:
    MemoryManagementSystem();
    ~MemoryManagementSystem();
    
    // System control
    void start() noexcept;
    void stop() noexcept;
    bool isRunning() const { return running_; }
    
    // Component access
    MemoryStatistics& getStatistics() { return statistics_; }
    MemoryLeakDetector& getLeakDetector() { return leakDetector_; }
    MemoryValidator& getValidator() { return validator_; }
    MemoryOptimizer& getOptimizer() { return optimizer_; }
    MemoryMonitor& getMonitor() { return monitor_; }
    
    // Reporting
    std::string generateSystemReport() const;
    std::string generateHealthReport() const;
    std::string generateDetailedReport() const;
    
    // Configuration
    void enableAllFeatures(bool enable);
    void setLogLevel(int level) { logLevel_ = level; }
    void setPerformanceThreshold(double threshold) { performanceThreshold_ = threshold; }
    
    // Singleton
    static MemoryManagementSystem& getInstance();
    
private:
    void initializeComponents();
    void cleanupComponents();
    
    MemoryStatistics statistics_;
    MemoryLeakDetector leakDetector_;
    MemoryValidator validator_;
    MemoryOptimizer optimizer_;
    MemoryMonitor monitor_;
    
    bool running_;
    int logLevel_;
    double performanceThreshold_;
    
    std::mutex mutex_;
};

// Global memory management functions
inline void* trackedAllocate(size_t size, const std::string& file = "", 
                           int line = 0, const std::string& function = "",
                           const std::string& tag = "DEFAULT", size_t alignment = 0) {
    return MemoryPoolAllocator::allocateWithTag(size, tag, file, line, function, alignment);
}

inline void trackedDeallocate(void* ptr, const std::string& file = "", 
                             int line = 0, const std::string& function = "",
                             const std::string& tag = "DEFAULT") {
    MemoryPoolAllocator::deallocateWithTag(ptr, tag, file, line, function);
}

inline void* trackedNew(size_t size, const std::string& file = "", 
                       int line = 0, const std::string& function = "",
                       const std::string& tag = "DEFAULT", size_t alignment = 0) {
    return trackedAllocate(size, file, line, function, tag, alignment);
}

inline void trackedDelete(void* ptr, const std::string& file = "", 
                         int line = 0, const std::string& function = "",
                         const std::string& tag = "DEFAULT") {
    trackedDeallocate(ptr, file, line, function, tag);
}

// Memory management macros
#define TRACKED_NEW(type, tag) new(type*) trackedAllocate(sizeof(type), __FILE__, __LINE__, "", tag)
#define TRACKED_DELETE(ptr) trackedDeallocate(ptr)

#define MEMORY_STATS() MemoryStatistics::generateReport()
#define MEMORY_LEAK_CHECK() MemoryLeakDetector::generateLeakReport()
#define MEMORY_VALIDATE() MemoryValidator::validateMemoryLeaks().report()

} // namespace nlm

// Template instantiations
namespace nlm {
    // Forward declarations for common types
    class Neuron;
    class Synapse;
    class NeuralNetwork;
    class Brain;
    
    // Specializations for common types
    template class NeuralMemoryManager<Neuron>;
    template class NeuralMemoryManager<Synapse>;
    template class NeuralMemoryManager<NeuralNetwork>;
    template class NeuralMemoryManager<Brain>;
}
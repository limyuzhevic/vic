#ifndef MEMORY_LEAK_DETECTION_H
#define MEMORY_LEAK_DETECTION_H

#include <unordered_map>
#include <string>
#include <mutex>
#include <iostream>
#include <sstream>

namespace nlm {
namespace MemoryLeakDetection {
    
    struct AllocationInfo {
        std::string typeName;
        std::string file;
        size_t line;
        size_t size;
        const char* stackTrace;
        uint64_t allocationId;
    };
    
    class MemoryTracker {
    public:
        static MemoryTracker& getInstance() {
            static MemoryTracker instance;
            return instance;
        }
        
        void trackAllocation(const std::string& typeName, size_t size, 
                           const std::string& file, size_t line,
                           uint64_t allocationId = 0);
        
        void trackDeallocation(uint64_t allocationId);
        
        void reportLeaks();
        
        void enableTracking(bool enable) { trackingEnabled = enable; }
        bool isTrackingEnabled() const { return trackingEnabled; }
        
    private:
        MemoryTracker() : nextAllocationId(1), trackingEnabled(true) {}
        
        std::unordered_map<uint64_t, AllocationInfo> allocations;
        uint64_t nextAllocationId;
        bool trackingEnabled;
        mutable std::mutex mutex;
    };
    
    // Global memory tracking macros
    #define TRACK_ALLOCATION(type, size) \
        ::nlm::MemoryLeakDetection::MemoryTracker::getInstance().trackAllocation(\
            #type, size, __FILE__, __LINE__)
    
    #define TRACK_DEALLOCATION(id) \
        ::nlm::MemoryLeakDetection::MemoryTracker::getInstance().trackDeallocation(id)
    
    void initializeMemoryTracking();
    void shutdownMemoryTracking();
    
}} // namespace MemoryLeakDetection

#endif // MEMORY_LEAK_DETECTION_H

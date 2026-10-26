#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <mutex>
#include <atomic>
#include <string>
#include <functional>
#include <stdexcept>
#include <limits>

namespace nlm {

// Memory pool management for improved performance
class MemoryPool {
public:
    explicit MemoryPool(size_t blockSize = 1024, size_t maxBlocks = 10000)
        : blockSize_(blockSize), maxBlocks_(maxBlocks), currentBlocks_(0) {
    }
    
    ~MemoryPool() {
        for (auto& block : blocks_) {
            delete[] block;
        }
    }
    
    // Allocate memory from the pool
    void* allocate(size_t size) {
        if (size > blockSize_) {
            // For large allocations, use system malloc
            return std::malloc(size);
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (freeBlocks_.empty()) {
            // Need a new block
            if (currentBlocks_ >= maxBlocks_) {
                throw std::bad_alloc();
            }
            
            void* block = new char[blockSize_];
            blocks_.push_back(block);
            freeBlocks_.push_back(block);
            currentBlocks_++;
        }
        
        void* ptr = freeBlocks_.front();
        freeBlocks_.pop_front();
        return ptr;
    }
    
    // Deallocate memory back to the pool
    void deallocate(void* ptr, size_t size) {
        if (size > blockSize_) {
            // Large allocation - use system free
            std::free(ptr);
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        freeBlocks_.push_back(ptr);
    }
    
    // Get current memory usage in bytes
    size_t getCurrentUsage() const {
        return (currentBlocks_ - freeBlocks_.size()) * blockSize_;
    }
    
    // Get peak memory usage
    size_t getPeakUsage() const {
        return peakUsage_;
    }
    
    // Get total blocks allocated
    size_t getTotalBlocks() const {
        return currentBlocks_;
    }
    
    // Get number of free blocks
    size_t getFreeBlocks() const {
        return freeBlocks_.size();
    }
    
    // Check if memory pool is at capacity
    bool isFull() const {
        return currentBlocks_ >= maxBlocks_;
    }
    
private:
    size_t blockSize_;
    size_t maxBlocks_;
    size_t currentBlocks_;
    size_t peakUsage_ = 0;
    std::vector<void*> blocks_;
    std::list<void*> freeBlocks_;
    mutable std::mutex mutex_;
    
    void updatePeakUsage() {
        size_t current = getCurrentUsage();
        if (current > peakUsage_) {
            peakUsage_ = current;
        }
    }
};

// Smart pointer with memory tracking
template<typename T>
class TrackedPointer {
public:
    TrackedPointer() : ptr_(nullptr), owner_(false) {}
    
    explicit TrackedPointer(T* ptr, bool owner = false) : ptr_(ptr), owner_(owner) {
    }
    
    ~TrackedPointer() {
        if (owner_ && ptr_) {
            delete ptr_;
            ptr_ = nullptr;
        }
    }
    
    // Disable copy, enable move
    TrackedPointer(const TrackedPointer&) = delete;
    TrackedPointer& operator=(const TrackedPointer&) = delete;
    
    TrackedPointer(TrackedPointer&& other) noexcept : ptr_(other.ptr_), owner_(other.owner_) {
        other.ptr_ = nullptr;
        other.owner_ = false;
    }
    
    TrackedPointer& operator=(TrackedPointer&& other) noexcept {
        if (this != &other) {
            if (owner_ && ptr_) {
                delete ptr_;
            }
            ptr_ = other.ptr_;
            owner_ = other.owner_;
            other.ptr_ = nullptr;
            other.owner_ = false;
        }
        return *this;
    }
    
    T* get() const {
        return ptr_;
    }
    
    T* operator->() const {
        return ptr_;
    }
    
    T& operator*() const {
        return *ptr_;
    }
    
    bool isValid() const {
        return ptr_ != nullptr;
    }
    
    bool isOwner() const {
        return owner_;
    }
    
    void reset() {
        if (owner_ && ptr_) {
            delete ptr_;
        }
        ptr_ = nullptr;
        owner_ = false;
    }
    
    void reset(T* ptr, bool owner = false) {
        if (owner_ && ptr_) {
            delete ptr_;
        }
        ptr_ = ptr;
        owner_ = owner;
    }
    
private:
    T* ptr_;
    bool owner_;
};

// Memory leak detector
class MemoryLeakDetector {
public:
    MemoryLeakDetector() {
        enableDetection();
    }
    
    static void enableDetection() {
        instance().enabled_ = true;
    }
    
    static void disableDetection() {
        instance().enabled_ = false;
    }
    
    static void allocate(const std::string& location, size_t size) {
        if (instance().enabled_) {
            std::lock_guard<std::mutex> lock(instance().mutex_);
            instance().allocations_[location] += size;
        }
    }
    
    static void deallocate(const std::string& location, size_t size) {
        if (instance().enabled_) {
            std::lock_guard<std::mutex> lock(instance().mutex_);
            if (instance().allocations_.count(location) > 0) {
                instance().allocations_[location] -= size;
                if (instance().allocations_[location] == 0) {
                    instance().allocations_.erase(location);
                }
            }
        }
    }
    
    static void reportLeaks() {
        if (!instance().enabled_) return;
        
        std::lock_guard<std::mutex> lock(instance().mutex_);
        
        if (instance().allocations_.empty()) {
            std::cout << "✓ No memory leaks detected!" << std::endl;
            return;
        }
        
        std::cout << "Memory leaks detected:" << std::endl;
        size_t totalLeaked = 0;
        for (const auto& allocation : instance().allocations_) {
            std::cout << "  " << allocation.first << ": " << allocation.second << " bytes" << std::endl;
            totalLeaked += allocation.second;
        }
        
        std::cout << "Total leaked memory: " << totalLeaked << " bytes" << std::endl;
    }
    
    static void clearLeaks() {
        std::lock_guard<std::mutex> lock(instance().mutex_);
        instance().allocations_.clear();
    }
    
private:
    MemoryLeakDetector() : enabled_(false) {}
    
    static MemoryLeakDetector& instance() {
        static MemoryLeakDetector detector;
        return detector;
    }
    
    bool enabled_;
    std::unordered_map<std::string, size_t> allocations_;
    mutable std::mutex mutex_;
    
    friend class std::allocator_traits<std::allocator<T>>;
};

// Memory statistics and monitoring
struct MemoryStats {
    size_t totalAllocated = 0;
    size_t peakAllocated = 0;
    size_t currentAllocated = 0;
    size_t allocationCount = 0;
    double allocationTime = 0.0;
};

class MemoryMonitor {
public:
    static MemoryStats getCurrentStats() {
        MemoryStats stats;
        // This would be implemented with platform-specific code
        // For now, return dummy values
        return stats;
    }
    
    static void logMemoryUsage(const std::string& context) {
        MemoryStats stats = getCurrentStats();
        std::cout << "Memory usage (" << context << "):" << std::endl;
        std::cout << "  Total allocated: " << stats.totalAllocated << " bytes" << std::endl;
        std::cout << "  Peak allocated: " << stats.peakAllocated << " bytes" << std::endl;
        std::cout << "  Current allocated: " << stats.currentAllocated << " bytes" << std::endl;
        std::cout << "  Allocation count: " << stats.allocationCount << std::endl;
        std::cout << "  Allocation time: " << stats.allocationTime << " seconds" << std::endl;
    }
    
    static void monitorMemoryUsage(size_t checkIntervalMs = 1000) {
        // This would run a background thread to monitor memory usage
        // For now, it's a placeholder
    }
private:
    static std::chrono::steady_clock::time_point startTime_;
};

// Custom allocators for different types of allocations
//Aligned allocator for SIMD operations
class AlignedAllocator {
public:
    static constexpr size_t alignment = 32; // Typical SIMD alignment
    
    template<typename T>
    using value_type = T;
    
    template<typename T>
    AlignedAllocator() noexcept {}
    
    template<typename T>
    AlignedAllocator(const AlignedAllocator&) noexcept {}
    
    template<typename T>
    AlignedAllocator& operator=(const AlignedAllocator&) noexcept {
        return *this;
    }
    
    template<typename T>
    T* allocate(std::size_t n) {
        void* ptr = std::aligned_alloc(alignment, n * sizeof(T));
        if (!ptr) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(ptr);
    }
    
    template<typename T>
    void deallocate(T* p, std::size_t n) noexcept {
        std::free(p);
    }
    
    template<typename T>
    bool operator==(const AlignedAllocator& other) const noexcept {
        return true;
    }
    
    template<typename T>
    bool operator!=(const AlignedAllocator& other) const noexcept {
        return false;
    }
};

// Memory arena for efficient allocation
class MemoryArena {
public:
    explicit MemoryArena(size_t capacity) : capacity_(capacity), position_(0) {
        buffer_ = static_cast<char*>(std::malloc(capacity));
        if (!buffer_) {
            throw std::bad_alloc();
        }
    }
    
    ~MemoryArena() {
        std::free(buffer_);
    }
    
    // Allocate memory from the arena
    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        size_t size = sizeof(T);
        size_t alignment = alignof(T);
        
        // Align position
        size_t offset = position_ % alignment;
        if (offset != 0) {
            offset = alignment - offset;
        }
        
        if (position_ + offset + size > capacity_) {
            throw std::bad_alloc();
        }
        
        position_ += offset;
        
        // Construct object
        void* ptr = buffer_ + position_;
        position_ += size;
        
        return new (ptr) T(std::forward<Args>(args)...);
    }
    
    // Destroy and deallocate object
    template<typename T>
    void deallocate(T* ptr) {
        ptr->~T();
        // Note: We don't actually deallocate, just move position back
        position_ -= sizeof(T);
    }
    
    // Reset arena (reuse memory)
    void reset() {
        position_ = 0;
    }
    
    // Get used capacity
    size_t getUsedCapacity() const {
        return position_;
    }
    
    // Get total capacity
    size_t getCapacity() const {
        return capacity_;
    }
    
    // Get utilization ratio
    double getUtilization() const {
        return static_cast<double>(position_) / capacity_;
    }
    
private:
    char* buffer_;
    size_t capacity_;
    size_t position_;
};

// Memory snapshot for debugging
class MemorySnapshot {
public:
    struct AllocationInfo {
        std::string location;
        size_t size;
        std::chrono::system_clock::time_point timestamp;
        std::string callStack;
    };
    
    static void takeSnapshot() {
        instance().takeSnapshot();
    }
    
    static std::vector<AllocationInfo> getSnapshot(const std::string& context = "") {
        return instance().getSnapshot(context);
    }
    
    static void printSnapshot(const std::string& context = "") {
        instance().printSnapshot(context);
    }
    
    static void compareSnapshots(const std::string& context1, const std::string& context2) {
        instance().compareSnapshots(context1, context2);
    }
    
private:
    MemorySnapshot() {}
    
    static MemorySnapshot& instance() {
        static MemorySnapshot snapshot;
        return snapshot;
    }
    
    std::vector<AllocationInfo> currentSnapshot_;
    std::map<std::string, std::vector<AllocationInfo>> contextSnapshots_;
    mutable std::mutex mutex_;
    
    void takeSnapshot() {
        std::lock_guard<std::mutex> lock(mutex_);
        currentSnapshot_.clear();
        // This would capture current memory allocations
        // For now, it's a placeholder
    }
    
    std::vector<AllocationInfo> getSnapshot(const std::string& context) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (context.empty()) {
            return currentSnapshot_;
        }
        return contextSnapshots_[context];
    }
    
    void printSnapshot(const std::string& context) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<AllocationInfo> snapshot = getSnapshot(context);
        
        std::cout << "Memory snapshot (" << context << "):" << std::endl;
        std::cout << "Total allocations: " << snapshot.size() << std::endl;
        std::cout << "Total memory: " << calculateTotalMemory(snapshot) << " bytes" << std::endl;
        
        for (const auto& allocation : snapshot) {
            std::cout << "  " << allocation.location << ": " 
                      << allocation.size << " bytes (" 
                      << std::chrono::duration_cast<std::chrono::milliseconds>(
                          allocation.timestamp.time_since_epoch()).count() 
                      << " ms since epoch)" << std::endl;
        }
    }
    
    void compareSnapshots(const std::string& context1, const std::string& context2) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto snapshot1 = getSnapshot(context1);
        auto snapshot2 = getSnapshot(context2);
        
        std::cout << "Memory snapshot comparison:" << std::endl;
        std::cout << "  " << context1 << ": " << snapshot1.size() << " allocations, "
                  << calculateTotalMemory(snapshot1) << " bytes" << std::endl;
        std::cout << "  " << context2 << ": " << snapshot2.size() << " allocations, "
                  << calculateTotalMemory(snapshot2) << " bytes" << std::endl;
        
        // Find differences
        std::unordered_map<std::string, size_t> counts1, counts2;
        for (const auto& allocation : snapshot1) {
            counts1[allocation.location] += allocation.size;
        }
        for (const auto& allocation : snapshot2) {
            counts2[allocation.location] += allocation.size;
        }
        
        std::cout << "  Changes:" << std::endl;
        for (const auto& pair : counts1) {
            if (counts2.count(pair.first) == 0) {
                std::cout << "    Added in " << context1 << ": " << pair.first << 
                          " (" << pair.second << " bytes)" << std::endl;
            }
        }
        for (const auto& pair : counts2) {
            if (counts1.count(pair.first) == 0) {
                std::cout << "    Added in " << context2 << ": " << pair.first << 
                          " (" << pair.second << " bytes)" << std::endl;
            }
        }
    }
    
    size_t calculateTotalMemory(const std::vector<AllocationInfo>& snapshot) const {
        size_t total = 0;
        for (const auto& allocation : snapshot) {
            total += allocation.size;
        }
        return total;
    }
};

// Memory usage tracker
class MemoryUsageTracker {
public:
    struct UsageInfo {
        std::chrono::system_clock::time_point timestamp;
        size_t allocatedBytes;
        size_t peakBytes;
        size_t allocationCount;
    };
    
    static void startTracking() {
        instance().startTracking();
    }
    
    static void stopTracking() {
        instance().stopTracking();
    }
    
    static void recordUsage() {
        instance().recordUsage();
    }
    
    static std::vector<UsageInfo> getUsageHistory() {
        return instance().getUsageHistory();
    }
    
    static void printUsageHistory(size_t maxEntries = 100) {
        instance().printUsageHistory(maxEntries);
    }
    
    static void analyzeTrends() {
        instance().analyzeTrends();
    }
    
private:
    MemoryUsageTracker() {}
    
    static MemoryUsageTracker& instance() {
        static MemoryUsageTracker tracker;
        return tracker;
    }
    
    std::vector<UsageInfo> usageHistory_;
    bool tracking_ = false;
    std::thread trackingThread_;
    mutable std::mutex mutex_;
    size_t checkIntervalMs_ = 1000;
    
    void startTracking() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (tracking_) {
            return;
        }
        
        tracking_ = true;
        trackingThread_ = std::thread(&MemoryUsageTracker::trackingLoop, this);
    }
    
    void stopTracking() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!tracking_) {
            return;
        }
        
        tracking_ = false;
        if (trackingThread_.joinable()) {
            trackingThread_.join();
        }
    }
    
    void trackingLoop() {
        while (tracking_) {
            recordUsage();
            std::this_thread::sleep_for(std::chrono::milliseconds(checkIntervalMs_));
        }
    }
    
    void recordUsage() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        UsageInfo info;
        info.timestamp = std::chrono::system_clock::now();
        info.allocatedBytes = getAllocatedBytes();
        info.peakBytes = getPeakBytes();
        info.allocationCount = getAllocationCount();
        
        usageHistory_.push_back(info);
        
        // Keep only recent history to prevent memory issues
        if (usageHistory_.size() > 10000) {
            usageHistory_.erase(usageHistory_.begin());
        }
    }
    
    std::vector<UsageInfo> getUsageHistory() {
        std::lock_guard<std::mutex> lock(mutex_);
        return usageHistory_;
    }
    
    void printUsageHistory(size_t maxEntries) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        size_t start = 0;
        if (usageHistory_.size() > maxEntries) {
            start = usageHistory_.size() - maxEntries;
        }
        
        std::cout << "Memory usage history (last " << usageHistory_.size() << " entries):" << std::endl;
        std::cout << "  Timestamp                   | Allocated | Peak    | Allocations" << std::endl;
        std::cout << "  ---------------------------|-----------|---------|------------" << std::endl;
        
        for (size_t i = start; i < usageHistory_.size(); ++i) {
            const auto& info = usageHistory_[i];
            auto time = std::chrono::system_clock::to_time_t(info.timestamp);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                info.timestamp.time_since_epoch()) % 1000;
            
            std::cout << "  " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
                      << "." << std::setfill('0') << std::setw(3) << ms.count()
                      << " | " << std::setw(9) << info.allocatedBytes
                      << " | " << std::setw(7) << info.peakBytes
                      << " | " << std::setw(11) << info.allocationCount << std::endl;
        }
    }
    
    void analyzeTrends() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (usageHistory_.size() < 10) {
            std::cout << "Not enough data for trend analysis." << std::endl;
            return;
        }
        
        std::cout << "Memory usage trends:" << std::endl;
        
        // Calculate average growth rate
        size_t totalGrowth = 0;
        for (size_t i = 1; i < usageHistory_.size(); ++i) {
            totalGrowth += usageHistory_[i].allocatedBytes - usageHistory_[i-1].allocatedBytes;
        }
        
        double avgGrowth = static_cast<double>(totalGrowth) / (usageHistory_.size() - 1);
        std::cout << "  Average growth rate: " << avgGrowth << " bytes per check" << std::endl;
        
        // Find peak usage period
        size_t peakIndex = 0;
        for (size_t i = 1; i < usageHistory_.size(); ++i) {
            if (usageHistory_[i].allocatedBytes > usageHistory_[peakIndex].allocatedBytes) {
                peakIndex = i;
            }
        }
        
        std::cout << "  Peak usage at entry " << peakIndex << ": "
                  << usageHistory_[peakIndex].allocatedBytes << " bytes" << std::endl;
        
        // Check for memory leaks (consistent growth)
        if (avgGrowth > 100) { // More than 100 bytes per check suggests a leak
            std::cout << "  WARNING: Possible memory leak detected (consistent growth)" << std::endl;
        }
    }
    
    // Platform-specific implementations (placeholders)
    size_t getAllocatedBytes() const {
        return 0; // Placeholder
    }
    
    size_t getPeakBytes() const {
        return 0; // Placeholder
    }
    
    size_t getAllocationCount() const {
        return 0; // Placeholder
    }
};

} // namespace nlm
#include "MemoryMonitor.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>

namespace nlm {

class MemoryMonitor::Impl {
public:
    Impl() : isTracking_(false), totalAllocated_(0), totalDeallocated_(0), 
             peakUsage_(0), gcEventCount_(0) {
        updateSystemMemoryUsage();
    }
    
    ~Impl() = default;
    
    void startTracking() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!isTracking_) {
            isTracking_ = true;
            startTime_ = std::chrono::high_resolution_clock::now();
            updateSystemMemoryUsage();
        }
    }
    
    void stopTracking() {
        std::lock_guard<std::mutex> lock(mutex_);
        isTracking_ = false;
    }
    
    bool isTracking() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return isTracking_;
    }
    
    void trackAllocation(size_t size, const std::string& source, const std::string& allocationId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        AllocationRecord record;
        record.size = size;
        record.source = source;
        record.timestamp = getCurrentTimestamp();
        record.allocationId = allocationId.empty() ? generateAllocationId() : allocationId;
        
        allocations_.push_back(record);
        totalAllocated_ += size;
        currentUsage_ += size;
        peakUsage_ = std::max(peakUsage_, currentUsage_);
        
        allocationsBySource_[source] += size;
        updateAllocationRate();
    }
    
    void trackAllocation(size_t size, const std::string& source) {
        trackAllocation(size, source, generateAllocationId());
    }
    
    void trackDeallocation(size_t size, const std::string& source, const std::string& allocationId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        DeallocationRecord record;
        record.size = size;
        record.source = source;
        record.timestamp = getCurrentTimestamp();
        record.deallocationId = generateDeallocationId();
        record.allocationId = allocationId;
        
        deallocations_.push_back(record);
        totalDeallocated_ += size;
        currentUsage_ -= size;
        
        updateDeallocationRate();
    }
    
    void trackDeallocation(size_t size, const std::string& source) {
        trackDeallocation(size, source, generateDeallocationId());
    }
    
    size_t getTotalMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return std::max(currentUsage_, systemMemoryUsage_);
    }
    
    size_t getAllocatedMemory() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalAllocated_;
    }
    
    size_t getDeallocatedMemory() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalDeallocated_;
    }
    
    size_t getCurrentMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentUsage_;
    }
    
    size_t getPeakMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return peakUsage_;
    }
    
    void recordPoolUsage(const std::string& poolName, size_t size, size_t capacity) {
        std::lock_guard<std::mutex> lock(mutex_);
        poolStats_[poolName] = {size, capacity};
    }
    
    std::map<std::string, std::pair<size_t, size_t>> getPoolStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return poolStats_;
    }
    
    std::vector<std::string> detectMemoryLeaks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> leaks;
        
        for (const auto& allocation : allocations_) {
            bool foundCorrespondingDeallocation = false;
            
            for (const auto& deallocation : deallocations_) {
                if (deallocation.allocationId == allocation.allocationId) {
                    foundCorrespondingDeallocation = true;
                    break;
                }
            }
            
            if (!foundCorrespondingDeallocation && allocation.size > 1024) { // 1KB threshold
                leaks.push_back("Allocation from " + allocation.source + 
                              " size " + std::to_string(allocation.size) + 
                              " at " + std::to_string(allocation.timestamp) +
                              " (ID: " + allocation.allocationId + ")");
            }
        }
        
        return leaks;
    }
    
    bool hasMemoryLeaks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !detectMemoryLeaks().empty();
    }
    
    MemoryStats getMemoryStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        MemoryStats stats;
        
        stats.totalAllocated = totalAllocated_;
        stats.totalDeallocated = totalDeallocated_;
        stats.currentUsage = currentUsage_;
        stats.peakUsage = peakUsage_;
        stats.allocationRate = allocationRate_;
        stats.deallocationRate = deallocationRate_;
        stats.allocationsBySource = allocationsBySource_;
        
        return stats;
    }
    
    std::vector<AllocationRecord> getAllocationHistory(const std::string& source) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<AllocationRecord> result;
        
        for (const auto& allocation : allocations_) {
            if (source.empty() || allocation.source == source) {
                result.push_back(allocation);
            }
        }
        
        return result;
    }
    
    std::vector<DeallocationRecord> getDeallocationHistory(const std::string& source) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<DeallocationRecord> result;
        
        for (const auto& deallocation : deallocations_) {
            if (source.empty() || deallocation.source == source) {
                result.push_back(deallocation);
            }
        }
        
        return result;
    }
    
    void recordGcEvent(size_t freedMemory, size_t gcTime, const std::string& gcType) {
        std::lock_guard<std::mutex> lock(mutex_);
        gcEvents_.push_back({freedMemory, gcTime, gcType});
        gcEventCount_++;
        
        // Update current usage after GC
        currentUsage_ -= freedMemory;
    }
    
    std::vector<std::tuple<size_t, size_t, std::string>> getGcStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::tuple<size_t, size_t, std::string>> result;
        
        for (const auto& event : gcEvents_) {
            result.push_back(event);
        }
        
        return result;
    }
    
    double getAllocationRate() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocationRate_;
    }
    
    double getDeallocationRate() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return deallocationRate_;
    }
    
    std::map<std::string, double> getAllocationRatesBySource() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocationRatesBySource_;
    }
    
    double getMemoryEfficiency() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (totalDeallocated_ == 0) {
            return 0.0;
        }
        
        // Efficiency = (allocated - deallocated) / allocated
        return static_cast<double>(totalAllocated_ - totalDeallocated_) / totalAllocated_;
    }
    
    size_t getTotalAllocationCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_.size();
    }
    
    size_t getTotalDeallocationCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return deallocations_.size();
    }
    
private:
    Timestamp getCurrentTimestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - startTime_;
        return std::chrono::duration<double>(duration).count();
    }
    
    std::string generateAllocationId() {
        static std::atomic<uint64_t> idCounter{0};
        return "alloc_" + std::to_string(idCounter++);
    }
    
    std::string generateDeallocationId() {
        static std::atomic<uint64_t> idCounter{0};
        return "dealloc_" + std::to_string(idCounter++);
    }
    
    void updateSystemMemoryUsage() {
        // In a real implementation, this would query the OS
        // For now, we use a placeholder implementation
        systemMemoryUsage_ = currentUsage_;
    }
    
    void updateAllocationRate() {
        if (!isTracking_) return;
        
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        
        if (duration > 0.0) {
            allocationRate_ = totalAllocated_ / duration;
        }
        
        // Update per-source rates
        allocationRatesBySource_.clear();
        for (const auto& pair : allocationsBySource_) {
            // Simplified calculation
            allocationRatesBySource_[pair.first] = pair.second / (1000.0 * duration); // bytes per ms
        }
    }
    
    void updateDeallocationRate() {
        if (!isTracking_) return;
        
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        
        if (duration > 0.0) {
            deallocationRate_ = totalDeallocated_ / duration;
        }
    }
    
    mutable std::mutex mutex_;
    bool isTracking_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    size_t totalAllocated_;
    size_t totalDeallocated_;
    size_t currentUsage_;
    size_t peakUsage_;
    size_t systemMemoryUsage_;
    
    double allocationRate_;
    double deallocationRate_;
    
    std::vector<AllocationRecord> allocations_;
    std::vector<DeallocationRecord> deallocations_;
    std::vector<std::tuple<size_t, size_t, std::string>> gcEvents_;
    
    std::map<std::string, size_t> allocationsBySource_;
    std::map<std::string, double> allocationRatesBySource_;
    
    std::map<std::string, std::pair<size_t, size_t>> poolStats_; // name -> (size, capacity)
    size_t gcEventCount_;
};

MemoryMonitor::MemoryMonitor() : pImpl(new Impl()) {}

MemoryMonitor::~MemoryMonitor() {
    delete pImpl;
}

void MemoryMonitor::startTracking() {
    pImpl->startTracking();
}

void MemoryMonitor::stopTracking() {
    pImpl->stopTracking();
}

bool MemoryMonitor::isTracking() const {
    return pImpl->isTracking();
}

void MemoryMonitor::trackAllocation(size_t size, const std::string& source, const std::string& allocationId) {
    pImpl->trackAllocation(size, source, allocationId);
}

void MemoryMonitor::trackAllocation(size_t size, const std::string& source) {
    pImpl->trackAllocation(size, source);
}

void MemoryMonitor::trackDeallocation(size_t size, const std::string& source, const std::string& allocationId) {
    pImpl->trackDeallocation(size, source, allocationId);
}

void MemoryMonitor::trackDeallocation(size_t size, const std::string& source) {
    pImpl->trackDeallocation(size, source);
}

size_t MemoryMonitor::getTotalMemoryUsage() const {
    return pImpl->getTotalMemoryUsage();
}

size_t MemoryMonitor::getAllocatedMemory() const {
    return pImpl->getAllocatedMemory();
}

size_t MemoryMonitor::getDeallocatedMemory() const {
    return pImpl->getDeallocatedMemory();
}

size_t MemoryMonitor::getCurrentMemoryUsage() const {
    return pImpl->getCurrentMemoryUsage();
}

size_t MemoryMonitor::getPeakMemoryUsage() const {
    return pImpl->getPeakMemoryUsage();
}

void MemoryMonitor::recordPoolUsage(const std::string& poolName, size_t size, size_t capacity) {
    pImpl->recordPoolUsage(poolName, size, capacity);
}

std::map<std::string, std::pair<size_t, size_t>> MemoryMonitor::getPoolStats() const {
    return pImpl->getPoolStats();
}

std::vector<std::string> MemoryMonitor::detectMemoryLeaks() const {
    return pImpl->detectMemoryLeaks();
}

bool MemoryMonitor::hasMemoryLeaks() const {
    return pImpl->hasMemoryLeaks();
}

MemoryStats MemoryMonitor::getMemoryStats() const {
    return pImpl->getMemoryStats();
}

std::vector<AllocationRecord> MemoryMonitor::getAllocationHistory(const std::string& source) const {
    return pImpl->getAllocationHistory(source);
}

std::vector<DeallocationRecord> MemoryMonitor::getDeallocationHistory(const std::string& source) const {
    return pImpl->getDeallocationHistory(source);
}

void MemoryMonitor::recordGcEvent(size_t freedMemory, size_t gcTime, const std::string& gcType) {
    pImpl->recordGcEvent(freedMemory, gcTime, gcType);
}

std::vector<std::tuple<size_t, size_t, std::string>> MemoryMonitor::getGcStats() const {
    return pImpl->getGcStats();
}

double MemoryMonitor::getAllocationRate() const {
    return pImpl->getAllocationRate();
}

double MemoryMonitor::getDeallocationRate() const {
    return pImpl->getDeallocationRate();
}

std::map<std::string, double> MemoryMonitor::getAllocationRatesBySource() const {
    return pImpl->getAllocationRatesBySource();
}

double MemoryMonitor::getMemoryEfficiency() const {
    return pImpl->getMemoryEfficiency();
}

size_t MemoryMonitor::getTotalAllocationCount() const {
    return pImpl->getTotalAllocationCount();
}

size_t MemoryMonitor::getTotalDeallocationCount() const {
    return pImpl->getTotalDeallocationCount();
}

} // namespace nlm
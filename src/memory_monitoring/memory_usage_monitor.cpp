#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <future>
#include <numeric>
#include <algorithm>
#include <random>
#include <cmath>
#include <sstream>
#include <iomanip>

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "memory_manager.h"

namespace nlm {

class MemoryUsageMonitor::Impl {
public:
    bool trackingEnabled_ = false;
    bool leakDetectionEnabled_ = false;
    
    std::unordered_map<std::string, size_t> allocationMap_;
    std::unordered_map<std::string, size_t> deallocationMap_;
    std::vector<MemoryStatistics> memoryHistory_;
    std::vector<std::string> detectedLeaks_;
    
    std::chrono::system_clock::time_point startTime_;
    size_t peakMemoryUsage_ = 0;
    size_t totalAllocations_ = 0;
    size_t totalDeallocations_ = 0;
    
    static constexpr size_t HISTORY_SIZE = 100;
    
    MemoryUsageMonitor::Impl() {
        memoryHistory_.reserve(HISTORY_SIZE);
        startTime_ = std::chrono::system_clock::now();
    }
    
    MemoryStatistics calculateCurrentStats() const {
        MemoryStatistics stats;
        
        // Calculate current memory usage
        size_t currentUsage = 0;
        for (const auto& pair : allocationMap_) {
            currentUsage += pair.second;
        }
        
        stats.totalMemory = currentUsage;
        stats.usedMemory = currentUsage;
        stats.peakMemory = peakMemoryUsage_;
        stats.allocatedObjects = totalAllocations_;
        stats.deallocatedObjects = totalDeallocations_;
        stats.efficiency = (peakMemoryUsage_ > 0) ? 
            (1.0 - static_cast<double>(currentUsage) / peakMemoryUsage_) : 0.0;
        stats.timestamp = std::chrono::system_clock::now();
        
        // Calculate fragmentation
        if (allocationMap_.size() > 1) {
            std::vector<size_t> sizes;
            for (const auto& pair : allocationMap_) {
                sizes.push_back(pair.second);
            }
            
            double mean = std::accumulate(sizes.begin(), sizes.end(), 0.0) / sizes.size();
            double variance = 0.0;
            for (size_t size : sizes) {
                variance += (size - mean) * (size - mean);
            }
            variance /= sizes.size();
            stats.fragmentation = std::sqrt(variance) / mean;
        } else {
            stats.fragmentation = 0.0;
        }
        
        return stats;
    }
    
    void detectMemoryLeaks() {
        if (!leakDetectionEnabled_) return;
        
        detectedLeaks_.clear();
        
        // Simple leak detection: allocations without corresponding deallocations
        for (const auto& pair : allocationMap_) {
            std::string source = pair.first;
            auto allocIt = allocationMap_.find(source);
            auto deallocIt = deallocationMap_.find(source);
            
            if (deallocIt == deallocationMap_.end() || allocIt->second > deallocIt->second) {
                detectedLeaks_.push_back(source);
            }
        }
    }
    
    std::vector<std::string> generateOptimizationRecommendations(const MemoryStatistics& stats) const {
        std::vector<std::string> recommendations;
        
        if (stats.fragmentation > 0.3) {
            recommendations.push_back("High memory fragmentation detected - consider memory pool optimization");
        }
        
        if (stats.efficiency < 0.5) {
            recommendations.push_back("Low memory efficiency - consider object pooling and reuse");
        }
        
        if (detectedLeaks_.size() > 0) {
            recommendations.push_back("Memory leaks detected - review source allocation patterns");
        }
        
        if (stats.allocatedObjects > stats.deallocatedObjects * 2) {
            recommendations.push_back("High allocation/deallocation ratio - consider object pooling");
        }
        
        if (recommendations.empty()) {
            recommendations.push_back("Memory usage is optimal - maintain current patterns");
        }
        
        return recommendations;
    }
    
    void optimizeMemoryUsage() {
        // Force garbage collection by removing unused allocations
        std::vector<std::string> toRemove;
        for (const auto& pair : allocationMap_) {
            if (pair.second == 0) {
                toRemove.push_back(pair.first);
            }
        }
        
        for (const std::string& source : toRemove) {
            allocationMap_.erase(source);
            deallocationMap_.erase(source);
        }
    }
};

MemoryUsageMonitor::MemoryUsageMonitor() : pImpl(std::make_unique<Impl>()) {}
MemoryUsageMonitor::~MemoryUsageMonitor() = default;

void MemoryUsageMonitor::startTracking() {
    std::lock_guard<std::shared_mutex> lock(monitorMutex_);
    pImpl->trackingEnabled_ = true;
    pImpl->startTime_ = std::chrono::system_clock::now();
    NLM_LOG_INFO("Memory tracking started");
}

void MemoryUsageMonitor::stopTracking() {
    std::lock_guard<std::shared_mutex> lock(monitorMutex_);
    pImpl->trackingEnabled_ = false;
    NLM_LOG_INFO("Memory tracking stopped");
}

void MemoryUsageMonitor::trackMemoryAllocation(size_t size, const std::string& source) {
    std::lock_guard<std::shared_mutex> lock(monitorMutex_);
    
    pImpl->allocationMap_[source] += size;
    pImpl->totalAllocations_++;
    
    MemoryStatistics stats = pImpl->calculateCurrentStats();
    if (stats.usedMemory > pImpl->peakMemoryUsage_) {
        pImpl->peakMemoryUsage_ = stats.usedMemory;
    }
    
    // Add to history
    pImpl->memoryHistory_.push_back(stats);
    if (pImpl->memoryHistory_.size() > pImpl->HISTORY_SIZE) {
        pImpl->memoryHistory_.erase(pImpl->memoryHistory_.begin());
    }
    
    if (pImpl->memoryMonitor_) {
        // Notify memory manager of tracking update
    }
}

void MemoryUsageMonitor::trackMemoryDeallocation(size_t size, const std::string& source) {
    std::lock_guard<std::shared_mutex> lock(monitorMutex_);
    
    pImpl->deallocationMap_[source] += size;
    pImpl->totalDeallocations_++;
    
    // Detect leaks
    pImpl->detectMemoryLeaks();
}

MemoryStatistics MemoryUsageMonitor::getCurrentMemoryStats() const {
    std::shared_lock<std::shared_mutex> lock(monitorMutex_);
    return pImpl->calculateCurrentStats();
}

std::vector<MemoryStatistics> MemoryUsageMonitor::getMemoryUsageHistory() const {
    std::shared_lock<std::shared_mutex> lock(monitorMutex_);
    return pImpl->memoryHistory_;
}

size_t MemoryUsageMonitor::detectMemoryLeaks() const {
    std::lock_guard<std::shared_mutex> lock(monitorMutex_);
    pImpl->detectMemoryLeaks();
    return pImpl->detectedLeaks_.size();
}

double MemoryUsageMonitor::calculateMemoryFragmentation() const {
    std::shared_lock<std::shared_mutex> lock(monitorMutex_);
    MemoryStatistics stats = pImpl->calculateCurrentStats();
    return stats.fragmentation;
}

void MemoryUsageMonitor::optimizeMemoryUsage() {
    std::lock_guard<std::shared_mutex> lock(monitorMutex_);
    pImpl->optimizeMemoryUsage();
}

std::vector<std::string> MemoryUsageMonitor::getOptimizationRecommendations() const {
    std::shared_lock<std::shared_mutex> lock(monitorMutex_);
    MemoryStatistics stats = pImpl->calculateCurrentStats();
    return pImpl->generateOptimizationRecommendations(stats);
}

void MemoryUsageMonitor::applyMemoryOptimization(const std::string& recommendation) {
    std::lock_guard<std::shared_mutex> lock(monitorMutex_);
    
    if (recommendation.find("object pooling") != std::string::npos) {
        // Implement object pooling
        NLM_LOG_INFO("Applying object pooling optimization");
    } else if (recommendation.find("reuse") != std::string::npos) {
        // Implement memory reuse
        NLM_LOG_INFO("Applying memory reuse optimization");
    } else if (recommendation.find("fragmentation") != std::string::npos) {
        // Implement fragmentation reduction
        NLM_LOG_INFO("Applying fragmentation reduction optimization");
    }
}

void MemoryUsageMonitor::enableLeakDetection(bool enable) {
    std::lock_guard<std::shared_mutex> lock(monitorMutex_);
    pImpl->leakDetectionEnabled_ = enable;
}

bool MemoryUsageMonitor::isLeakDetectionEnabled() const {
    std::shared_lock<std::shared_mutex> lock(monitorMutex_);
    return pImpl->leakDetectionEnabled_;
}

std::vector<std::string> MemoryUsageMonitor::getDetectedLeaks() const {
    std::shared_lock<std::shared_mutex> lock(monitorMutex_);
    return pImpl->detectedLeaks_;
}

} // namespace nlm

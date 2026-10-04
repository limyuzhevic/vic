#include "MemoryProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <unordered_set>

namespace nlm {

MemoryProfiler::MemoryProfiler()
    : totalAllocated_(0),
      totalFreed_(0),
      currentUsage_(0),
      memoryCheckInterval_(100),
      detectMemoryLeaks_(true),
      leakThreshold_(1024), // 1KB leak threshold
      maxSnapshots_(100),
      enableDetailedTracking_(true),
      trackAllocationStack_(false) {
}

MemoryProfiler::MemoryProfiler(std::shared_ptr<DebuggingSystem> debuggingSystem)
    : totalAllocated_(0),
      totalFreed_(0),
      currentUsage_(0),
      memoryCheckInterval_(100),
      detectMemoryLeaks_(true),
      leakThreshold_(1024),
      maxSnapshots_(100),
      enableDetailedTracking_(true),
      trackAllocationStack_(false),
      debuggingSystem_(debuggingSystem) {
}

MemoryProfiler::~MemoryProfiler() = default;

void MemoryProfiler::recordAllocation(size_t size, const std::string& context,
                                     const std::string& site,
                                     const std::vector<std::string>& callStack) {
    MemoryAllocation alloc(size, context, site, callStack);
    
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    allocations_.push_back(alloc);
    
    totalAllocated_ += size;
    currentUsage_ += size;
    
    // Update debugging system if available
    if (debuggingSystem_) {
        debuggingSystem_->recordMemoryAllocation(size, context, site);
    }
    
    // Cleanup old allocations periodically
    if (allocations_.size() % 1000 == 0) {
        cleanupOldAllocations();
    }
}

void MemoryProfiler::recordDeallocation(size_t size, const std::string& context,
                                       const std::string& site,
                                       const std::vector<std::string>& callStack) {
    MemoryDeallocation dealloc(size, context, site, callStack);
    
    std::lock_guard<std::mutex> lock(deallocationsMutex_);
    deallocations_.push_back(dealloc);
    
    totalFreed_ += size;
    currentUsage_ -= size;
    
    // Update debugging system if available
    if (debuggingSystem_) {
        debuggingSystem_->recordMemoryDeallocation(size, context, site);
    }
    
    // Remove corresponding leak if found
    if (detectMemoryLeaks_) {
        std::lock_guard<std::mutex> leakLock(leaksMutex_);
        auto it = std::remove_if(currentLeaks_.begin(), currentLeaks_.end(),
                                 [size, context](const MemoryLeakInfo& leak) {
                                     return leak.leakedSize >= size && leak.context == context;
                                 });
        currentLeaks_.erase(it, currentLeaks_.end());
    }
}

void MemoryProfiler::takeSnapshot() {
    MemorySnapshot snapshot;
    
    {
        std::lock_guard<std::mutex> allocLock(allocationsMutex_);
        std::lock_guard<std::mutex> deallocLock(deallocationsMutex_);
        
        snapshot.totalAllocated = totalAllocated_;
        snapshot.totalFreed = totalFreed_;
        snapshot.currentUsage = currentUsage_;
        
        // Calculate allocations by context
        for (const auto& alloc : allocations_) {
            snapshot.allocationsByContext[alloc.context] += alloc.size;
        }
        
        // Calculate deallocations by context
        for (const auto& dealloc : deallocations_) {
            snapshot.deallocationsByContext[dealloc.context] += dealloc.size;
        }
        
        // Calculate leaks by context
        for (const auto& leak : currentLeaks_) {
            snapshot.leakedByContext[leak.context] += leak.leakedSize;
        }
    }
    
    std::lock_guard<std::mutex> snapLock(snapshotsMutex_);
    snapshots_.push_back(snapshot);
    
    // Keep only last maxSnapshots_ entries
    if (snapshots_.size() > maxSnapshots_) {
        snapshots_.erase(snapshots_.begin());
    }
    
    // Run leak detection if enabled
    if (detectMemoryLeaks_) {
        std::lock_guard<std::mutex> leakLock(leaksMutex_);
        currentLeaks_ = detectMemoryLeaks();
    }
}

std::vector<MemoryProfiler::MemoryLeakInfo> MemoryProfiler::detectMemoryLeaks() const {
    std::vector<MemoryLeakInfo> leaks;
    
    if (!detectMemoryLeaks_) return leaks;
    
    std::lock_guard<std::mutex> allocLock(allocationsMutex_);
    std::lock_guard<std::mutex> deallocLock(deallocationsMutex_);
    
    // Simple leak detection: allocations without corresponding deallocations
    for (const auto& alloc : allocations_) {
        bool foundMatching = false;
        
        // Look for matching deallocation (same context and similar size)
        for (const auto& dealloc : deallocations_) {
            if (dealloc.context == alloc.context && 
                std::abs(static_cast<long>(dealloc.size) - static_cast<long>(alloc.size)) < 100) {
                foundMatching = true;
                break;
            }
        }
        
        if (!foundMatching) {
            // Check if this allocation could be a leak
            if (isMemoryLeak(alloc)) {
                MemoryLeakInfo leak(alloc.context, alloc.size, 1,
                                  std::vector<MemoryAllocation>{alloc}, alloc.allocationSite);
                leaks.push_back(leak);
            }
        }
    }
    
    return leaks;
}

std::string MemoryProfiler::getLeakReport() const {
    std::ostringstream oss;
    oss << "Memory Leak Report\n";
    oss << "==================\n\n";
    
    auto leaks = detectMemoryLeaks();
    
    if (leaks.empty()) {
        oss << "No memory leaks detected.\n";
        return oss.str();
    }
    
    oss << "Detected Memory Leaks:\n";
    for (size_t i = 0; i < leaks.size(); ++i) {
        const auto& leak = leaks[i];
        oss << i + 1 << ". Context: " << leak.context << "\n";
        oss << "   Size: " << leak.leakedSize << " bytes\n";
        oss << "   Count: " << leak.allocationCount << " allocations\n";
        oss << "   Suspected Site: " << leak.suspectedSite << "\n";
        oss << "   First Allocation: " << 
            std::chrono::duration_cast<std::chrono::seconds>(
                leak.firstAllocation.time_since_epoch()).count() << " seconds ago\n";
        oss << "   Leak Score: " << calculateLeakScore(leak) << "\n";
        
        if (leak.leakedSize >= leakThreshold_) {
            oss << "   *** HIGH PRIORITY - Critical leak ***\n";
        } else if (leak.leakedSize >= leakThreshold_ / 2) {
            oss << "   *** MEDIUM PRIORITY - Fix recommended ***\n";
        }
        oss << "\n";
    }
    
    oss << "Total Leaks: " << leaks.size() << "\n";
    oss << "Total Leaked Memory: " << 
        std::accumulate(leaks.begin(), leaks.end(), 0,
                       [](size_t sum, const MemoryLeakInfo& leak) {
                           return sum + leak.leakedSize;
                       }) << " bytes\n";
    
    return oss.str();
}

std::string MemoryProfiler::getDetailedLeakReport() const {
    std::ostringstream oss;
    oss << "=== DETAILED MEMORY LEAK ANALYSIS ===\n\n";
    
    auto leaks = detectMemoryLeaks();
    
    oss << "Memory Allocation Analysis:\n";
    {
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        std::unordered_map<std::string, size_t> allocByContext;
        
        for (const auto& alloc : allocations_) {
            allocByContext[alloc.context] += alloc.size;
        }
        
        for (const auto& [context, size] : allocByContext) {
            oss << "  " << context << ": " << size << " bytes\n";
        }
    }
    
    oss << "\nMemory Deallocation Analysis:\n";
    {
        std::lock_guard<std::mutex> lock(deallocationsMutex_);
        std::unordered_map<std::string, size_t> deallocByContext;
        
        for (const auto& dealloc : deallocations_) {
            deallocByContext[dealloc.context] += dealloc.size;
        }
        
        for (const auto& [context, size] : deallocByContext) {
            oss << "  " << context << ": " << size << " bytes\n";
        }
    }
    
    oss << "\nCurrent Memory Snapshots:\n";
    {
        std::lock_guard<std::mutex> lock(snapshotsMutex_);
        for (size_t i = 0; i < std::min(snapshots_.size(), size_t(5)); ++i) {
            const auto& snap = snapshots_[snapshots_.size() - 1 - i];
            oss << "  Snapshot " << (i + 1) << ": " << snap.currentUsage << " bytes current, "
                << snap.totalAllocated << " bytes allocated\n";
        }
    }
    
    oss << "\nMemory Leak Details:\n";
    for (size_t i = 0; i < std::min(leaks.size(), size_t(5)); ++i) {
        const auto& leak = leaks[i];
        oss << "  Leak " << i + 1 << ": " << leak.context << " (" << leak.leakedSize << " bytes)\n";
        
        if (enableDetailedTracking_ && i < 3) { // Only show first 3 leaks in detail
            oss << "    Allocation sites:\n";
            for (const auto& alloc : leak.allocations) {
                oss << "      - " << alloc.allocationSite << " (" << alloc.size << " bytes)\n";
            }
        }
    }
    
    return oss.str();
}

std::unordered_map<std::string, size_t> MemoryProfiler::getAllocationStats() const {
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    std::unordered_map<std::string, size_t> stats;
    
    for (const auto& alloc : allocations_) {
        stats[alloc.context] += alloc.size;
    }
    
    return stats;
}

std::unordered_map<std::string, size_t> MemoryProfiler::getDeallocationStats() const {
    std::lock_guard<std::mutex> lock(deallocationsMutex_);
    std::unordered_map<std::string, size_t> stats;
    
    for (const auto& dealloc : deallocations_) {
        stats[dealloc.context] += dealloc.size;
    }
    
    return stats;
}

std::string MemoryProfiler::analyzeAllocationPatterns() const {
    std::ostringstream oss;
    oss << "Memory Allocation Pattern Analysis:\n";
    oss << "==============================\n\n";
    
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    
    // Analyze allocation sizes
    std::vector<size_t> allocationSizes;
    for (const auto& alloc : allocations_) {
        allocationSizes.push_back(alloc.size);
    }
    
    if (allocationSizes.empty()) {
        oss << "No allocation data available.\n";
        return oss.str();
    }
    
    std::sort(allocationSizes.begin(), allocationSizes.end());
    
    oss << "Allocation Size Distribution:\n";
    oss << "  Total Allocations: " << allocationSizes.size() << "\n";
    oss << "  Total Allocated Memory: " << totalAllocated_ << " bytes\n";
    oss << "  Current Memory Usage: " << currentUsage_ << " bytes\n";
    oss << "  Average Allocation Size: " << 
        std::accumulate(allocationSizes.begin(), allocationSizes.end(), 0.0) / allocationSizes.size() << " bytes\n";
    oss << "  Min Allocation Size: " << allocationSizes.front() << " bytes\n";
    oss << "  Max Allocation Size: " << allocationSizes.back() << " bytes\n";
    
    // Identify common allocation patterns
    std::unordered_map<size_t, size_t> sizeCounts;
    for (size_t size : allocationSizes) {
        sizeCounts[size]++;
    }
    
    oss << "\nCommon Allocation Sizes:\n";
    std::vector<std::pair<size_t, size_t>> sortedSizes(
        sizeCounts.begin(), sizeCounts.end());
    std::sort(sortedSizes.begin(), sortedSizes.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < std::min(sortedSizes.size(), size_t(10)); ++i) {
        const auto& [size, count] = sortedSizes[i];
        oss << "  " << size << " bytes: " << count << " allocations\n";
    }
    
    return oss.str();
}

std::string MemoryProfiler::analyzeAllocationTrends() const {
    std::ostringstream oss;
    oss << "Memory Allocation Trend Analysis:\n";
    oss << "==============================\n\n";
    
    {
        std::lock_guard<std::mutex> lock(snapshotsMutex_);
        
        if (snapshots_.size() < 2) {
            oss << "Insufficient data for trend analysis.\n";
            return oss.str();
        }
        
        // Analyze recent trend (last 10 snapshots)
        size_t recentSnapshots = std::min(snapshots_.size(), size_t(10));
        std::vector<size_t> currentUsages;
        std::vector<double> growthRates;
        
        for (size_t i = 0; i < recentSnapshots; ++i) {
            currentUsages.push_back(snapshots_[snapshots_.size() - 1 - i].currentUsage);
        }
        
        oss << "Memory Usage Trend (last " << recentSnapshots << " snapshots):\n";
        for (size_t i = 0; i < currentUsages.size(); ++i) {
            oss << "  Snapshot " << i + 1 << ": " << currentUsages[i] << " bytes\n";
        }
        
        // Calculate growth rate
        if (currentUsages.size() >= 2) {
            double first = currentUsages.front();
            double last = currentUsages.back();
            double growthRate = (last - first) / first;
            
            oss << "\nGrowth Rate: " << (growthRate >= 0 ? "+" : "") << 
                (growthRate * 100) << "%\n";
            
            if (std::abs(growthRate) < 0.01) {
                oss << "  Trend: Stable\n";
            } else if (growthRate > 0.01) {
                oss << "  Trend: Increasing (potential leak)\n";
            } else {
                oss << "  Trend: Decreasing\n";
            }
        }
    }
    
    return oss.str();
}

bool MemoryProfiler::exportMemoryProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportMemoryProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string MemoryProfiler::exportMemoryProfileAsString() const {
    std::ostringstream oss;
    oss << "Memory Profile\n";
    oss << "==============\n\n";
    
    oss << "Memory Statistics:\n";
    oss << "  Total Allocations: " << allocations_.size() << "\n";
    oss << "  Total Deallocations: " << deallocations_.size() << "\n";
    oss << "  Current Allocations: " << currentUsage_ << " bytes\n";
    oss << "  Peak Memory Usage: " << (totalAllocated_ > totalFreed_ ? totalAllocated_ - totalFreed_ : 0) << " bytes\n";
    
    oss << "\nAllocation Statistics:\n";
    {
        std::lock_guard<std::mutex> lock(allocationsMutex_);
        std::unordered_map<std::string, size_t> contextStats;
        
        for (const auto& alloc : allocations_) {
            contextStats[alloc.context] += alloc.size;
        }
        
        for (const auto& [context, size] : contextStats) {
            oss << "  " << context << ": " << size << " bytes\n";
        }
    }
    
    oss << "\nDeallocation Statistics:\n";
    {
        std::lock_guard<std::mutex> lock(deallocationsMutex_);
        std::unordered_map<std::string, size_t> contextStats;
        
        for (const auto& dealloc : deallocations_) {
            contextStats[dealloc.context] += dealloc.size;
        }
        
        for (const auto& [context, size] : contextStats) {
            oss << "  " << context << ": " << size << " bytes\n";
        }
    }
    
    oss << "\nCurrent Leaks:\n";
    auto leaks = detectMemoryLeaks();
    for (const auto& leak : leaks) {
        oss << "  " << leak.context << ": " << leak.leakedSize << " bytes\n";
    }
    
    oss << "\nSnapshots:\n";
    {
        std::lock_guard<std::mutex> lock(snapshotsMutex_);
        for (size_t i = 0; i < std::min(snapshots_.size(), size_t(5)); ++i) {
            const auto& snap = snapshots_[snapshots_.size() - 1 - i];
            oss << "  Snapshot " << i + 1 << ": " << snap.currentUsage << " bytes\n";
        }
    }
    
    return oss.str();
}

void MemoryProfiler::addLeakedAllocation(const MemoryAllocation& allocation) {
    std::lock_guard<std::mutex> lock(leaksMutex_);
    
    // Check if this allocation is already tracked as a leak
    bool found = false;
    for (const auto& leak : currentLeaks_) {
        if (leak.context == allocation.context && 
            leak.suspectedSite == allocation.allocationSite) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        MemoryLeakInfo leak(allocation.context, allocation.size, 1,
                          std::vector<MemoryAllocation>{allocation}, allocation.allocationSite);
        currentLeaks_.push_back(leak);
    }
}

void MemoryProfiler::removeLeakedAllocation(const MemoryAllocation& allocation) {
    std::lock_guard<std::mutex> lock(leaksMutex_);
    
    auto it = std::remove_if(currentLeaks_.begin(), currentLeaks_.end(),
                             [&allocation](const MemoryLeakInfo& leak) {
                                 return leak.context == allocation.context && 
                                        leak.suspectedSite == allocation.allocationSite;
                             });
    currentLeaks_.erase(it, currentLeaks_.end());
}

std::string MemoryProfiler::getRealTimeMemoryStats() const {
    std::ostringstream oss;
    oss << "Real-Time Memory Statistics:\n";
    oss << "==========================\n\n";
    
    oss << "Current Usage: " << currentUsage_ << " bytes\n";
    oss << "Total Allocated: " << totalAllocated_ << " bytes\n";
    oss << "Total Freed: " << totalFreed_ << " bytes\n";
    oss << "Allocation Count: " << allocations_.size() << "\n";
    oss << "Deallocation Count: " << deallocations_.size() << "\n";
    
    double memoryUsagePercent = totalAllocated_ > 0 ? 
        (static_cast<double>(currentUsage_) / totalAllocated_) * 100.0 : 0.0;
    
    oss << "Memory Usage Percentage: " << memoryUsagePercent << "%\n";
    
    if (isMemoryUsageHigh()) {
        oss << "STATUS: HIGH MEMORY USAGE - Consider optimization\n";
    } else {
        oss << "STATUS: NORMAL MEMORY USAGE\n";
    }
    
    return oss.str();
}

bool MemoryProfiler::isMemoryUsageHigh() const {
    double memoryUsagePercent = totalAllocated_ > 0 ? 
        (static_cast<double>(currentUsage_) / totalAllocated_) * 100.0 : 0.0;
    
    return memoryUsagePercent > 80.0; // 80% or more
}

void MemoryProfiler::updateStats() {
    std::lock_guard<std::mutex> allocLock(allocationsMutex_);
    std::lock_guard<std::mutex> deallocLock(deallocationsMutex_);
    
    totalAllocated_ = 0;
    totalFreed_ = 0;
    currentUsage_ = 0;
    
    for (const auto& alloc : allocations_) {
        totalAllocated_ += alloc.size;
        currentUsage_ += alloc.size;
    }
    
    for (const auto& dealloc : deallocations_) {
        totalFreed_ += dealloc.size;
        currentUsage_ -= dealloc.size;
    }
}

bool MemoryProfiler::isMemoryLeak(const MemoryAllocation& alloc) const {
    // Simple heuristic: allocation that hasn't been freed yet and is large
    return alloc.size > leakThreshold_ &&
           currentUsage_ > totalAllocated_ * 0.8; // More than 80% of allocated memory still in use
}

std::string MemoryProfiler::getAllocationSite(const std::string& context) const {
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    
    for (const auto& alloc : allocations_) {
        if (alloc.context == context) {
            return alloc.allocationSite;
        }
    }
    
    return "Unknown";
}

double MemoryProfiler::calculateLeakScore(const MemoryLeakInfo& leak) const {
    double sizeScore = static_cast<double>(leak.leakedSize) / leakThreshold_;
    double timeScore = 1.0; // Recent leaks have higher scores
    
    if (!leak.allocations.empty()) {
        auto now = std::chrono::system_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::hours>(
            now - leak.allocations.front().timestamp).count();
        
        timeScore = std::min(1.0, age / 24.0); // Decay over 24 hours
    }
    
    return (sizeScore * 0.7 + timeScore * 0.3);
}

void MemoryProfiler::cleanupOldAllocations() {
    // Remove allocations older than 24 hours
    auto now = std::chrono::system_clock::now();
    auto oneDayAgo = now - std::chrono::hours(24);
    
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    auto it = std::remove_if(allocations_.begin(), allocations_.end(),
                             [oneDayAgo](const MemoryAllocation& alloc) {
                                 return alloc.timestamp < oneDayAgo;
                             });
    allocations_.erase(it, allocations_.end());
}

void MemoryProfiler::cleanupOldDeallocations() {
    // Remove deallocations older than 24 hours
    auto now = std::chrono::system_clock::now();
    auto oneDayAgo = now - std::chrono::hours(24);
    
    std::lock_guard<std::mutex> lock(deallocationsMutex_);
    auto it = std::remove_if(deallocations_.begin(), deallocations_.end(),
                             [oneDayAgo](const MemoryDeallocation& dealloc) {
                                 return dealloc.timestamp < oneDayAgo;
                             });
    deallocations_.erase(it, deallocations_.end());
}

std::vector<MemoryProfiler::MemoryAllocation> MemoryProfiler::findMatchingDeallocation(
    const MemoryAllocation& alloc) const {
    std::lock_guard<std::mutex> lock(deallocationsMutex_);
    std::vector<MemoryAllocation> matches;
    
    for (const auto& dealloc : deallocations_) {
        if (dealloc.context == alloc.context && 
            std::abs(static_cast<long>(dealloc.size) - static_cast<long>(alloc.size)) < 100) {
            matches.push_back(dealloc); // Note: This should be a MemoryAllocation, not MemoryDeallocation
        }
    }
    
    return matches;
}

size_t MemoryProfiler::calculateFragmentation() const {
    // Estimate memory fragmentation
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    
    if (allocations_.empty()) return 0;
    
    // Simple fragmentation metric: variance of allocation sizes
    std::vector<size_t> sizes;
    for (const auto& alloc : allocations_) {
        sizes.push_back(alloc.size);
    }
    
    double mean = std::accumulate(sizes.begin(), sizes.end(), 0.0) / sizes.size();
    double variance = 0.0;
    for (size_t size : sizes) {
        variance += (size - mean) * (size - mean);
    }
    variance /= sizes.size();
    
    return static_cast<size_t>(variance);
}

std::string MemoryProfiler::analyzeAllocationSources() const {
    std::ostringstream oss;
    oss << "Memory Allocation Sources Analysis:\n";
    oss << "==============================\n\n";
    
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    
    std::unordered_map<std::string, size_t> sourceStats;
    for (const auto& alloc : allocations_) {
        // Extract source from context (simple heuristic)
        std::string source = alloc.context;
        if (source.empty()) source = "unknown";
        sourceStats[source] += alloc.size;
    }
    
    std::vector<std::pair<std::string, size_t>> sortedSources(
        sourceStats.begin(), sourceStats.end());
    std::sort(sortedSources.begin(), sortedSources.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (const auto& [source, size] : sortedSources) {
        oss << "  " << source << ": " << size << " bytes\n";
    }
    
    return oss.str();
}

std::string MemoryProfiler::analyzeDeallocationSources() const {
    std::ostringstream oss;
    oss << "Memory Deallocation Sources Analysis:\n";
    oss << "==============================\n\n";
    
    std::lock_guard<std::mutex> lock(deallocationsMutex_);
    
    std::unordered_map<std::string, size_t> sourceStats;
    for (const auto& dealloc : deallocations_) {
        // Extract source from context (simple heuristic)
        std::string source = dealloc.context;
        if (source.empty()) source = "unknown";
        sourceStats[source] += dealloc.size;
    }
    
    std::vector<std::pair<std::string, size_t>> sortedSources(
        sourceStats.begin(), sourceStats.end());
    std::sort(sortedSources.begin(), sortedSources.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (const auto& [source, size] : sortedSources) {
        oss << "  " << source << ": " << size << " bytes\n";
    }
    
    return oss.str();
}

} // namespace nlm

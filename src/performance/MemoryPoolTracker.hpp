#include "OptimizationAnalyzer.hpp"

namespace nlm {

void MemoryPoolTracker::trackMemoryPoolAllocation(
    const std::string& poolName, size_t objectSize, size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = poolStats_.find(poolName);
    if (it == poolStats_.end()) {
        PoolStats stats;
        stats.poolName = poolName;
        stats.totalObjectsAllocated = count;
        stats.totalObjectsDeallocated = 0;
        stats.currentObjects = count;
        stats.totalMemoryBytes = objectSize * count;
        stats.allocationEfficiency = 1.0;
        poolStats_[poolName] = stats;
    } else {
        it->second.totalObjectsAllocated += count;
        it->second.currentObjects += count;
        it->second.totalMemoryBytes += objectSize * count;
        if (it->second.totalObjectsAllocated > 0) {
            it->second.allocationEfficiency = static_cast<double>(it->second.totalObjectsDeallocated) /
                                             it->second.totalObjectsAllocated;
        }
    }
}

void MemoryPoolTracker::trackMemoryPoolDeallocation(
    const std::string& poolName, size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = poolStats_.find(poolName);
    if (it != poolStats_.end()) {
        it->second.totalObjectsDeallocated += count;
        it->second.currentObjects -= count;
        if (it->second.totalObjectsAllocated > 0) {
            it->second.allocationEfficiency = static_cast<double>(it->second.totalObjectsDeallocated) /
                                             it->second.totalObjectsAllocated;
        }
    }
}

std::vector<MemoryPoolTracker::PoolStats> MemoryPoolTracker::getPoolStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PoolStats> result;
    result.reserve(poolStats_.size());
    for (const auto& pair : poolStats_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<std::string> MemoryPoolTracker::getPoolRecommendations() const {
    std::vector<std::string> recommendations;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : poolStats_) {
        const PoolStats& stats = pair.second;
        if (stats.allocationEfficiency < 0.8) {
            recommendations.push_back("Pool " + stats.poolName + ": Improve deallocation efficiency");
        }
        if (stats.totalObjectsAllocated > 10000 && stats.currentObjects > stats.totalObjectsAllocated * 0.5) {
            recommendations.push_back("Pool " + stats.poolName + ": Consider increasing pool size");
        }
        if (stats.totalMemoryBytes > 100 * 1024 * 1024) {  // 100MB
            recommendations.push_back("Pool " + stats.poolName + ": Consider using larger blocks");
        }
    }
    
    return recommendations;
}

std::string MemoryPoolTracker::generatePoolReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::stringstream ss;
    ss << "=== Memory Pool Tracking Report ===\n";
    ss << "Total memory pools: " << poolStats_.size() << "\n\n";
    
    for (const auto& pair : poolStats_) {
        const PoolStats& stats = pair.second;
        ss << "Pool: " << stats.poolName << "\n";
        ss << "  Objects allocated: " << stats.totalObjectsAllocated << "\n";
        ss << "  Objects deallocated: " << stats.totalObjectsDeallocated << "\n";
        ss << "  Current objects: " << stats.currentObjects << "\n";
        ss << "  Total memory: " << stats.totalMemoryBytes << " bytes ("
           << (stats.totalMemoryBytes / (1024 * 1024)) << " MB)\n";
        ss << "  Allocation efficiency: " << (stats.allocationEfficiency * 100) << "%\n\n";
    }
    
    std::vector<std::string> recommendations = getPoolRecommendations();
    if (!recommendations.empty()) {
        ss << "=== Recommendations ===\n";
        for (const auto& recommendation : recommendations) {
            ss << "- " << recommendation << "\n";
        }
    }
    
    return ss.str();
}

} // namespace nlm

#endif // NLM_MEMORY_POOL_TRACKER_HPP
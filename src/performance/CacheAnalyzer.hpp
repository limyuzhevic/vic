#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <memory>
#include <map>

namespace nlm {

// CacheAnalyzer: Cache performance analysis
class CacheAnalyzer {
public:
    CacheAnalyzer();
    ~CacheAnalyzer();
    
    // Configuration
    void setEnableCacheProfiling(bool enable) { enableCacheProfiling_ = enable; }
    void setCacheLineSize(size_t size) { cacheLineSize_ = size; }
    void setEnableCacheHierarchyAnalysis(bool enable) { enableCacheHierarchyAnalysis_ = enable; }
    
    // Cache performance tracking
    void recordCacheMeasurement(const std::string& operation, size_t cacheLevel,
                                double accessTime, double hitRate, size_t accesses,
                                size_t misses, size_t evictions = 0,
                                const std::string& context = "");
    
    // Query results
    double getAverageAccessTime(const std::string& operation, size_t cacheLevel) const;
    double getAverageHitRate(const std::string& operation, size_t cacheLevel) const;
    double getAverageMissRate(const std::string& operation, size_t cacheLevel) const;
    double getAverageMissLatency(const std::string& operation, size_t cacheLevel) const;
    
    // Cache listing
    std::vector<std::string> getCachedOperations() const;
    std::vector<size_t> getCacheLevels() const;
    
    // Analysis
    std::string analyzeCachePerformance() const;
    std::string analyzeCacheHierarchy() const;
    std::string identifyCacheBottlenecks() const;
    std::string recommendCacheOptimizations() const;
    
    // Statistics
    size_t getTotalCacheMeasurements() const { return totalMeasurements_; }
    std::string getOptimalCacheConfiguration(const std::string& operation) const;
    std::string analyzeCacheMissPatterns() const;
    
    // Export
    bool exportCacheProfile(const std::string& filepath) const;
    std::string exportCacheProfileAsString() const;
    
    // Real-time analysis
    std::string getRealTimeCacheStats() const;
    
private:
    struct CacheMeasurement {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        size_t cacheLevel;
        double accessTime;
        double hitRate;
        size_t accesses;
        size_t misses;
        size_t evictions;
        std::string context;
        
        CacheMeasurement(const std::string& op, size_t level, double accessTime,
                        double hitRate, size_t accesses, size_t misses,
                        size_t evictions, const std::string& ctx)
            : operation(op), cacheLevel(level), accessTime(accessTime), hitRate(hitRate),
              accesses(accesses), misses(misses), evictions(evictions), context(ctx) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    std::vector<CacheMeasurement> measurements_;
    mutable std::mutex measurementsMutex_;
    
    bool enableCacheProfiling_;
    bool enableCacheHierarchyAnalysis_;
    size_t cacheLineSize_;
    size_t totalMeasurements_;
    
    // Helper methods
    void cleanupOldMeasurements();
    double calculateMissLatency(double accessTime, double hitRate) const;
    double calculateCacheEfficiency(double hitRate, double accessTime) const;
    std::string getCacheLevelDescription(size_t level) const;
    bool isCacheMissRateAcceptable(double missRate) const;
    bool isAccessTimeAcceptable(double accessTime) const;
};

} // namespace nlm

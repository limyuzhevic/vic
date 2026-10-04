#include "CacheAnalyzer.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

CacheAnalyzer::CacheAnalyzer()
    : enableCacheProfiling_(true),
      enableCacheHierarchyAnalysis_(true),
      cacheLineSize_(64),
      totalMeasurements_(0) {
}

CacheAnalyzer::~CacheAnalyzer() = default;

void CacheAnalyzer::recordCacheMeasurement(const std::string& operation, size_t cacheLevel,
                                          double accessTime, double hitRate, size_t accesses,
                                          size_t misses, size_t evictions,
                                          const std::string& context) {
    CacheMeasurement measurement(operation, cacheLevel, accessTime, hitRate,
                                 accesses, misses, evictions, context);
    
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    measurements_.push_back(measurement);
    totalMeasurements_++;
    
    // Cleanup old measurements periodically
    if (measurements_.size() % 1000 == 0) {
        cleanupOldMeasurements();
    }
}

double CacheAnalyzer::getAverageAccessTime(const std::string& operation, size_t cacheLevel) const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<double> accessTimes;
    for (const auto& measurement : measurements_) {
        if (measurement.operation == operation && measurement.cacheLevel == cacheLevel) {
            accessTimes.push_back(measurement.accessTime);
        }
    }
    
    if (accessTimes.empty()) return 0.0;
    
    double sum = std::accumulate(accessTimes.begin(), accessTimes.end(), 0.0);
    return sum / accessTimes.size();
}

double CacheAnalyzer::getAverageHitRate(const std::string& operation, size_t cacheLevel) const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<double> hitRates;
    for (const auto& measurement : measurements_) {
        if (measurement.operation == operation && measurement.cacheLevel == cacheLevel) {
            hitRates.push_back(measurement.hitRate);
        }
    }
    
    if (hitRates.empty()) return 0.0;
    
    double sum = std::accumulate(hitRates.begin(), hitRates.end(), 0.0);
    return sum / hitRates.size();
}

double CacheAnalyzer::getAverageMissRate(const std::string& operation, size_t cacheLevel) const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<double> missRates;
    for (const auto& measurement : measurements_) {
        if (measurement.operation == operation && measurement.cacheLevel == cacheLevel) {
            double missRate = 1.0 - measurement.hitRate;
            missRates.push_back(missRate);
        }
    }
    
    if (missRates.empty()) return 0.0;
    
    double sum = std::accumulate(missRates.begin(), missRates.end(), 0.0);
    return sum / missRates.size();
}

double CacheAnalyzer::getAverageMissLatency(const std::string& operation, size_t cacheLevel) const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<double> missLatencies;
    for (const auto& measurement : measurements_) {
        if (measurement.operation == operation && measurement.cacheLevel == cacheLevel) {
            double missLatency = calculateMissLatency(measurement.accessTime, measurement.hitRate);
            missLatencies.push_back(missLatency);
        }
    }
    
    if (missLatencies.empty()) return 0.0;
    
    double sum = std::accumulate(missLatencies.begin(), missLatencies.end(), 0.0);
    return sum / missLatencies.size();
}

std::vector<std::string> CacheAnalyzer::getCachedOperations() const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<std::string> operations;
    std::unordered_set<std::string> uniqueOps;
    for (const auto& measurement : measurements_) {
        if (uniqueOps.find(measurement.operation) == uniqueOps.end()) {
            uniqueOps.insert(measurement.operation);
            operations.push_back(measurement.operation);
        }
    }
    
    return operations;
}

std::vector<size_t> CacheAnalyzer::getCacheLevels() const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<size_t> levels;
    std::unordered_set<size_t> uniqueLevels;
    for (const auto& measurement : measurements_) {
        if (uniqueLevels.find(measurement.cacheLevel) == uniqueLevels.end()) {
            uniqueLevels.insert(measurement.cacheLevel);
            levels.push_back(measurement.cacheLevel);
        }
    }
    
    std::sort(levels.begin(), levels.end());
    return levels;
}

std::string CacheAnalyzer::analyzeCachePerformance() const {
    std::ostringstream oss;
    oss << "Cache Performance Analysis:\n";
    oss << "========================\n\n";
    
    auto operations = getCachedOperations();
    auto cacheLevels = getCacheLevels();
    
    oss << "Cache Performance by Operation:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        for (size_t level : cacheLevels) {
            double accessTime = getAverageAccessTime(operation, level);
            double hitRate = getAverageHitRate(operation, level);
            double missRate = getAverageMissRate(operation, level);
            
            oss << "    L" << level << ": " << (accessTime * 1000) << " us, " <<
                (hitRate * 100) << "% hit, " << (missRate * 100) << "% miss\n";
            
            double efficiency = calculateCacheEfficiency(hitRate, accessTime);
            oss << "      Efficiency: " << (efficiency * 100) << "%\n";
            
            if (!isCacheMissRateAcceptable(missRate)) {
                oss << "      *** HIGH MISS RATE ***\n";
            }
            if (!isAccessTimeAcceptable(accessTime)) {
                oss << "      *** HIGH LATENCY ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string CacheAnalyzer::analyzeCacheHierarchy() const {
    std::ostringstream oss;
    oss << "Cache Hierarchy Analysis:\n";
    oss << "=========================\n\n";
    
    auto operations = getCachedOperations();
    auto cacheLevels = getCacheLevels();
    
    oss << "Cache Hierarchy by Operation:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        for (size_t level : cacheLevels) {
            double accessTime = getAverageAccessTime(operation, level);
            double hitRate = getAverageHitRate(operation, level);
            
            std::string levelDesc = getCacheLevelDescription(level);
            oss << "    L" << level << " (" << levelDesc << "): " << (accessTime * 1000) << " us, " <<
                (hitRate * 100) << "% hit\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string CacheAnalyzer::identifyCacheBottlenecks() const {
    std::ostringstream oss;
    oss << "Cache Bottleneck Identification:\n";
    oss << "=============================\n\n";
    
    auto operations = getCachedOperations();
    auto cacheLevels = getCacheLevels();
    
    oss << "Identified Cache Bottlenecks:\n";
    
    for (const auto& operation : operations) {
        bool hasBottleneck = false;
        
        for (size_t level : cacheLevels) {
            double missRate = getAverageMissRate(operation, level);
            double accessTime = getAverageAccessTime(operation, level);
            
            if (missRate > 0.2) {
                hasBottleneck = true;
                oss << "  " << operation << " at L" << level << ": " << (missRate * 100) << "% miss rate\n";
                oss << "    *** CACHE MISS BOTTLENECK ***\n";
                
                if (level == 1) {
                    oss << "    Suggestion: Increase cache size\n";
                } else {
                    oss << "    Suggestion: Optimize data layout\n";
                }
            }
            
            if (accessTime > 0.1) { // 100 us
                hasBottleneck = true;
                oss << "  " << operation << " at L" << level << ": " << (accessTime * 1000) << " us access time\n";
                oss << "    *** CACHE LATENCY BOTTLENECK ***\n";
                oss << "    Suggestion: Optimize working set\n";
            }
        }
        
        if (!hasBottleneck) {
            oss << "  " << operation << ": No cache bottlenecks detected\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string CacheAnalyzer::recommendCacheOptimizations() const {
    std::ostringstream oss;
    oss << "Cache Optimization Recommendations:\n";
    oss << "==============================\n\n";
    
    auto operations = getCachedOperations();
    
    oss << "Cache Optimization Strategies:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        size_t optimalLevel = getOptimalCacheConfiguration(operation).empty() ? 1 : 
                             std::stoull(getOptimalCacheConfiguration(operation).substr(
                                 getOptimalCacheConfiguration(operation).find_last_of(' ') + 1));
        
        oss << "    Recommended Cache Level: L" << optimalLevel << "\n";
        
        if (optimalLevel == 1) {
            oss << "      - Use L1 cache optimizations\n";
            oss << "      - Implement data prefetching\n";
        } else if (optimalLevel == 2) {
            oss << "      - Use L2 cache optimizations\n";
            oss << "      - Implement blocking/tiling\n";
        } else {
            oss << "      - Use L3 cache optimizations\n";
            oss << "      - Implement loop transformations\n";
        }
        
        double hitRate = getAverageHitRate(operation, optimalLevel);
        if (hitRate > 0.9) {
            oss << "      *** EXCELLENT HIT RATE ***\n";
        } else if (hitRate > 0.8) {
            oss << "      *** GOOD HIT RATE ***\n";
        } else {
            oss << "      *** IMPROVE HIT RATE ***\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string CacheAnalyzer::getOptimalCacheConfiguration(const std::string& operation) const {
    auto cacheLevels = getCacheLevels();
    
    if (cacheLevels.empty()) return "No data available";
    
    double maxEfficiency = 0.0;
    size_t optimalLevel = cacheLevels[0];
    
    for (size_t level : cacheLevels) {
        double hitRate = getAverageHitRate(operation, level);
        double accessTime = getAverageAccessTime(operation, level);
        double efficiency = calculateCacheEfficiency(hitRate, accessTime);
        
        if (efficiency > maxEfficiency) {
            maxEfficiency = efficiency;
            optimalLevel = level;
        }
    }
    
    std::ostringstream oss;
    oss << "Optimal cache level for " << operation << ": L" << optimalLevel << " (" << (maxEfficiency * 100) << "% efficiency)";
    
    return oss.str();
}

std::string CacheAnalyzer::analyzeCacheMissPatterns() const {
    std::ostringstream oss;
    oss << "Cache Miss Pattern Analysis:\n";
    oss << "==========================\n\n";
    
    auto operations = getCachedOperations();
    auto cacheLevels = getCacheLevels();
    
    oss << "Cache Miss Pattern Analysis:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        for (size_t level : cacheLevels) {
            double missRate = getAverageMissRate(operation, level);
            double missLatency = getAverageMissLatency(operation, level);
            
            oss << "    L" << level << ": " << (missRate * 100) << "% miss rate, " << (missLatency * 1000) << " us miss latency\n";
            
            if (missRate > 0.1) {
                oss << "      *** HIGH MISS RATE ***\n";
            }
            if (missLatency > 10.0) { // 10 us
                oss << "      *** HIGH MISS LATENCY ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

bool CacheAnalyzer::exportCacheProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportCacheProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string CacheAnalyzer::exportCacheProfileAsString() const {
    std::ostringstream oss;
    oss << "Cache Profile\n";
    oss << "============\n\n";
    
    oss << "Cache Analysis Summary:\n";
    oss << "  Total Cache Measurements: " << totalMeasurements_ << "\n";
    oss << "  Number of Operations: " << getCachedOperations().size() << "\n";
    oss << "  Number of Cache Levels: " << getCacheLevels().size() << "\n\n";
    
    oss << "Cache Levels Analyzed:\n";
    auto cacheLevels = getCacheLevels();
    for (size_t level : cacheLevels) {
        std::string levelDesc = getCacheLevelDescription(level);
        oss << "  L" << level << " (" << levelDesc << ")\n";
    }
    
    oss << "\nCached Operations:\n";
    auto operations = getCachedOperations();
    for (const auto& operation : operations) {
        oss << "  " << operation << "\n";
    }
    
    oss << "\nCache Performance by Operation:\n";
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        size_t optimalLevel = std::stoull(getOptimalCacheConfiguration(operation).substr(
            getOptimalCacheConfiguration(operation).find_last_of(' ') + 1));
        double hitRate = getAverageHitRate(operation, optimalLevel);
        double accessTime = getAverageAccessTime(operation, optimalLevel);
        oss << "    Optimal: L" << optimalLevel << " (" << (hitRate * 100) << "% hit, " << (accessTime * 1000) << " us access)\n";
    }
    
    return oss.str();
}

std::string CacheAnalyzer::getRealTimeCacheStats() const {
    std::ostringstream oss;
    oss << "Real-Time Cache Statistics:\n";
    oss << "==========================\n\n";
    
    oss << "Current Cache State:\n";
    oss << "  Total Cache Measurements: " << totalMeasurements_ << "\n";
    oss << "  Active Operations: " << getCachedOperations().size() << "\n";
    oss << "  Cache Levels Analyzed: " << getCacheLevels().size() << "\n";
    
    if (!getCachedOperations().empty() && !getCacheLevels().empty()) {
        oss << "\nRecent Cache Measurements:\n";
        std::lock_guard<std::mutex> lock(measurementsMutex_);
        
        size_t recentCount = std::min(measurements_.size(), size_t(5));
        for (size_t i = 0; i < recentCount; ++i) {
            const auto& measurement = measurements_[measurements_.size() - 1 - i];
            oss << "  " << measurement.operation << " at L" << measurement.cacheLevel << 
                ": " << (measurement.hitRate * 100) << "% hit, " <<
                (measurement.accessTime * 1000) << " us access\n";
        }
    }
    
    return oss.str();
}

void CacheAnalyzer::cleanupOldMeasurements() {
    // Remove measurements older than 24 hours
    auto now = std::chrono::system_clock::now();
    auto oneDayAgo = now - std::chrono::hours(24);
    
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    auto it = std::remove_if(measurements_.begin(), measurements_.end(),
                             [oneDayAgo](const CacheMeasurement& m) {
                                 return m.timestamp < oneDayAgo;
                             });
    measurements_.erase(it, measurements_.end());
}

double CacheAnalyzer::calculateMissLatency(double accessTime, double hitRate) const {
    double missRate = 1.0 - hitRate;
    double missPenalty = 100.0; // microseconds - typical cache miss penalty
    
    return accessTime + missRate * missPenalty;
}

double CacheAnalyzer::calculateCacheEfficiency(double hitRate, double accessTime) const {
    double efficiency = hitRate * 100.0; // Percentage
    
    if (accessTime > 0.1) { // 100 us
        efficiency *= 0.5; // Penalty for high access time
    }
    
    return std::min(efficiency / 100.0, 1.0); // Cap at 1.0
}

std::string CacheAnalyzer::getCacheLevelDescription(size_t level) const {
    if (level == 1) return "Small, fast";
    else if (level == 2) return "Medium, faster";
    else if (level == 3) return "Large, fastest";
    else return "Unknown";
}

bool CacheAnalyzer::isCacheMissRateAcceptable(double missRate) const {
    return missRate <= 0.2; // 20% or less
}

bool CacheAnalyzer::isAccessTimeAcceptable(double accessTime) const {
    return accessTime <= 0.1; // 100 us or less
}

} // namespace nlm

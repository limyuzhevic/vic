#include "MemoryAccessPatternAnalyzer.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

MemoryAccessPatternAnalyzer::MemoryAccessPatternAnalyzer()
    : enableAccessPatternAnalysis_(true),
      enableTemporalAnalysis_(true),
      enableSpatialAnalysis_(true),
      minAccessPatternSamples_(3),
      totalMeasurements_(0) {
}

MemoryAccessPatternAnalyzer::~MemoryAccessPatternAnalyzer() = default;

void MemoryAccessPatternAnalyzer::recordMemoryAccess(const std::string& operation, size_t accessSize,
                                                    double accessTime, bool cacheHit, size_t cacheLine,
                                                    const std::string& accessPattern, double spatialLocality,
                                                    double temporalLocality, const std::string& context) {
    MemoryAccess access(operation, accessSize, accessTime, cacheHit, cacheLine,
                        accessPattern, spatialLocality, temporalLocality, context);
    
    std::lock_guard<std::mutex> lock(accessesMutex_);
    accesses_.push_back(access);
    totalMeasurements_++;
    
    // Cleanup old accesses periodically
    if (accesses_.size() % 1000 == 0) {
        cleanupOldAccesses();
    }
}

double MemoryAccessPatternAnalyzer::getAverageAccessTime(const std::string& operation, size_t accessSize) const {
    std::lock_guard<std::mutex> lock(accessesMutex_);
    
    std::vector<double> accessTimes;
    for (const auto& access : accesses_) {
        if (access.operation == operation && access.accessSize == accessSize) {
            accessTimes.push_back(access.accessTime);
        }
    }
    
    if (accessTimes.empty()) return 0.0;
    
    double sum = std::accumulate(accessTimes.begin(), accessTimes.end(), 0.0);
    return sum / accessTimes.size();
}

double MemoryAccessPatternAnalyzer::getAccessHitRate(const std::string& operation, size_t accessSize) const {
    std::lock_guard<std::mutex> lock(accessesMutex_);
    
    size_t hits = 0;
    size_t total = 0;
    for (const auto& access : accesses_) {
        if (access.operation == operation && access.accessSize == accessSize) {
            total++;
            if (access.cacheHit) hits++;
        }
    }
    
    if (total == 0) return 0.0;
    return static_cast<double>(hits) / total;
}

double MemoryAccessPatternAnalyzer::getAverageSpatialLocality(const std::string& operation, size_t accessSize) const {
    std::lock_guard<std::mutex> lock(accessesMutex_);
    
    std::vector<double> spatialLocalities;
    for (const auto& access : accesses_) {
        if (access.operation == operation && access.accessSize == accessSize) {
            spatialLocalities.push_back(access.spatialLocality);
        }
    }
    
    if (spatialLocalities.empty()) return 0.0;
    
    double sum = std::accumulate(spatialLocalities.begin(), spatialLocalities.end(), 0.0);
    return sum / spatialLocalities.size();
}

double MemoryAccessPatternAnalyzer::getAverageTemporalLocality(const std::string& operation, size_t accessSize) const {
    std::lock_guard<std::mutex> lock(accessesMutex_);
    
    std::vector<double> temporalLocalities;
    for (const auto& access : accesses_) {
        if (access.operation == operation && access.accessSize == accessSize) {
            temporalLocalities.push_back(access.temporalLocality);
        }
    }
    
    if (temporalLocalities.empty()) return 0.0;
    
    double sum = std::accumulate(temporalLocalities.begin(), temporalLocalities.end(), 0.0);
    return sum / temporalLocalities.size();
}

std::vector<std::string> MemoryAccessPatternAnalyzer::getOperations() const {
    std::lock_guard<std::mutex> lock(accessesMutex_);
    
    std::vector<std::string> operations;
    std::unordered_set<std::string> uniqueOps;
    for (const auto& access : accesses_) {
        if (uniqueOps.find(access.operation) == uniqueOps.end()) {
            uniqueOps.insert(access.operation);
            operations.push_back(access.operation);
        }
    }
    
    return operations;
}

std::vector<size_t> MemoryAccessPatternAnalyzer::getAccessSizes() const {
    std::lock_guard<std::mutex> lock(accessesMutex_);
    
    std::vector<size_t> sizes;
    std::unordered_set<size_t> uniqueSizes;
    for (const auto& access : accesses_) {
        if (uniqueSizes.find(access.accessSize) == uniqueSizes.end()) {
            uniqueSizes.insert(access.accessSize);
            sizes.push_back(access.accessSize);
        }
    }
    
    std::sort(sizes.begin(), sizes.end());
    return sizes;
}

std::string MemoryAccessPatternAnalyzer::analyzeMemoryAccessPatterns() const {
    std::ostringstream oss;
    oss << "Memory Access Pattern Analysis:\n";
    oss << "==========================\n\n";
    
    auto operations = getOperations();
    auto accessSizes = getAccessSizes();
    
    oss << "Memory Access Pattern Analysis:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        for (size_t accessSize : accessSizes) {
            double accessTime = getAverageAccessTime(operation, accessSize);
            double hitRate = getAccessHitRate(operation, accessSize);
            double spatialLocality = getAverageSpatialLocality(operation, accessSize);
            double temporalLocality = getAverageTemporalLocality(operation, accessSize);
            
            oss << "    Size " << accessSize << " bytes: " << (accessTime * 1000) << " us, " <<
                (hitRate * 100) << "% hit, " << (spatialLocality * 100) << "% spatial, " <<
                (temporalLocality * 100) << "% temporal\n";
            
            double efficiency = calculateAccessEfficiency(accessTime, hitRate);
            oss << "      Efficiency: " << (efficiency * 100) << "%\n";
            
            if (isAccessPatternOptimal(spatialLocality, temporalLocality)) {
                oss << "      *** OPTIMAL ACCESS PATTERN ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string MemoryAccessPatternAnalyzer::analyzeSpatialLocality() const {
    std::ostringstream oss;
    oss << "Spatial Locality Analysis:\n";
    oss << "=========================\n\n";
    
    auto operations = getOperations();
    auto accessSizes = getAccessSizes();
    
    oss << "Spatial Locality by Operation:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        for (size_t accessSize : accessSizes) {
            double spatialLocality = getAverageSpatialLocality(operation, accessSize);
            
            oss << "    Size " << accessSize << " bytes: " << (spatialLocality * 100) << "% spatial locality\n";
            
            if (spatialLocality > 0.8) {
                oss << "      *** EXCELLENT SPATIAL LOCALITY ***\n";
            } else if (spatialLocality > 0.6) {
                oss << "      *** GOOD SPATIAL LOCALITY ***\n";
            } else if (spatialLocality > 0.4) {
                oss << "      *** MODERATE SPATIAL LOCALITY ***\n";
            } else {
                oss << "      *** POOR SPATIAL LOCALITY ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string MemoryAccessPatternAnalyzer::analyzeTemporalLocality() const {
    std::ostringstream oss;
    oss << "Temporal Locality Analysis:\n";
    oss << "==========================\n\n";
    
    auto operations = getOperations();
    auto accessSizes = getAccessSizes();
    
    oss << "Temporal Locality by Operation:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        for (size_t accessSize : accessSizes) {
            double temporalLocality = getAverageTemporalLocality(operation, accessSize);
            
            oss << "    Size " << accessSize << " bytes: " << (temporalLocality * 100) << "% temporal locality\n";
            
            if (temporalLocality > 0.8) {
                oss << "      *** EXCELLENT TEMPORAL LOCALITY ***\n";
            } else if (temporalLocality > 0.6) {
                oss << "      *** GOOD TEMPORAL LOCALITY ***\n";
            } else if (temporalLocality > 0.4) {
                oss << "      *** MODERATE TEMPORAL LOCALITY ***\n";
            } else {
                oss << "      *** POOR TEMPORAL LOCALITY ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string MemoryAccessPatternAnalyzer::identifyAccessPatternBottlenecks() const {
    std::ostringstream oss;
    oss << "Access Pattern Bottleneck Identification:\n";
    oss << "====================================\n\n";
    
    auto operations = getOperations();
    auto accessSizes = getAccessSizes();
    
    oss << "Identified Access Pattern Bottlenecks:\n";
    
    for (const auto& operation : operations) {
        bool hasBottleneck = false;
        
        for (size_t accessSize : accessSizes) {
            double hitRate = getAccessHitRate(operation, accessSize);
            double accessTime = getAverageAccessTime(operation, accessSize);
            double spatialLocality = getAverageSpatialLocality(operation, accessSize);
            double temporalLocality = getAverageTemporalLocality(operation, accessSize);
            
            if (!isCacheHitRateAcceptable(hitRate)) {
                hasBottleneck = true;
                oss << "  " << operation << " with size " << accessSize << " bytes: " << (hitRate * 100) << "% hit rate\n";
                oss << "    *** CACHE HIT RATE BOTTLENECK ***\n";
                
                if (spatialLocality < 0.5) {
                    oss << "    Suggestion: Improve spatial locality\n";
                } else {
                    oss << "    Suggestion: Increase cache size\n";
                }
            }
            
            if (!isAccessTimeAcceptable(accessTime)) {
                hasBottleneck = true;
                oss << "  " << operation << " with size " << accessSize << " bytes: " << (accessTime * 1000) << " us access time\n";
                oss << "    *** ACCESS TIME BOTTLENECK ***\n";
                oss << "    Suggestion: Optimize access pattern\n";
            }
            
            if (!isAccessPatternOptimal(spatialLocality, temporalLocality)) {
                hasBottleneck = true;
                oss << "  " << operation << " with size " << accessSize << " bytes: suboptimal locality\n";
                oss << "    *** ACCESS PATTERN BOTTLENECK ***\n";
                oss << "    Suggestion: Use better data layout\n";
            }
        }
        
        if (!hasBottleneck) {
            oss << "  " << operation << ": No access pattern bottlenecks detected\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string MemoryAccessPatternAnalyzer::recommendMemoryOptimizations() const {
    std::ostringstream oss;
    oss << "Memory Optimization Recommendations:\n";
    oss << "==============================\n\n";
    
    auto operations = getOperations();
    
    oss << "Memory Optimization Strategies:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        size_t optimalSize = std::stoull(getOptimalAccessSize(operation).substr(
            getOptimalAccessSize(operation).find_last_of(' ') + 1));
        
        oss << "    Recommended Access Size: " << optimalSize << " bytes\n";
        
        double spatialLocality = getAverageSpatialLocality(operation, optimalSize);
        double temporalLocality = getAverageTemporalLocality(operation, optimalSize);
        
        if (spatialLocality > 0.8 && temporalLocality > 0.8) {
            oss << "      *** EXCELLENT MEMORY ACCESS PATTERN ***\n";
        } else if (spatialLocality > 0.6 && temporalLocality > 0.6) {
            oss << "      *** GOOD MEMORY ACCESS PATTERN ***\n";
        } else {
            oss << "      *** IMPROVE MEMORY ACCESS ***\n";
        }
        
        if (spatialLocality < 0.5) {
            oss << "      - Use contiguous memory allocation\n";
            oss << "      - Implement data tiling\n";
        }
        if (temporalLocality < 0.5) {
            oss << "      - Cache frequently accessed data\n";
            oss << "      - Use data reuse strategies\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string MemoryAccessPatternAnalyzer::getOptimalAccessSize(const std::string& operation) const {
    auto accessSizes = getAccessSizes();
    
    if (accessSizes.empty()) return "No data available";
    
    double maxEfficiency = 0.0;
    size_t optimalSize = accessSizes[0];
    
    for (size_t accessSize : accessSizes) {
        double hitRate = getAccessHitRate(operation, accessSize);
        double accessTime = getAverageAccessTime(operation, accessSize);
        double efficiency = calculateAccessEfficiency(accessTime, hitRate);
        
        if (efficiency > maxEfficiency) {
            maxEfficiency = efficiency;
            optimalSize = accessSize;
        }
    }
    
    std::ostringstream oss;
    oss << "Optimal access size for " << operation << ": " << optimalSize << " bytes (" << (maxEfficiency * 100) << "% efficiency)";
    
    return oss.str();
}

std::string MemoryAccessPatternAnalyzer::analyzeAccessPatternEfficiency() const {
    std::ostringstream oss;
    oss << "Access Pattern Efficiency Analysis:\n";
    oss << "==============================\n\n";
    
    auto operations = getOperations();
    auto accessSizes = getAccessSizes();
    
    oss << "Access Pattern Efficiency:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        for (size_t accessSize : accessSizes) {
            double accessTime = getAverageAccessTime(operation, accessSize);
            double hitRate = getAccessHitRate(operation, accessSize);
            double spatialLocality = getAverageSpatialLocality(operation, accessSize);
            double temporalLocality = getAverageTemporalLocality(operation, accessSize);
            
            double efficiency = calculateAccessEfficiency(accessTime, hitRate);
            
            oss << "    Size " << accessSize << " bytes: " << (efficiency * 100) << "% efficiency\n";
            
            if (efficiency > 0.9) {
                oss << "      *** HIGH EFFICIENCY ***\n";
            } else if (efficiency > 0.7) {
                oss << "      *** GOOD EFFICIENCY ***\n";
            } else if (efficiency > 0.5) {
                oss << "      *** MODERATE EFFICIENCY ***\n";
            } else {
                oss << "      *** LOW EFFICIENCY ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

bool MemoryAccessPatternAnalyzer::exportAccessPatternProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportAccessPatternProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string MemoryAccessPatternAnalyzer::exportAccessPatternProfileAsString() const {
    std::ostringstream oss;
    oss << "Access Pattern Profile\n";
    oss << "====================\n\n";
    
    oss << "Access Pattern Analysis Summary:\n";
    oss << "  Total Access Pattern Measurements: " << totalMeasurements_ << "\n";
    oss << "  Number of Operations: " << getOperations().size() << "\n";
    oss << "  Number of Access Sizes: " << getAccessSizes().size() << "\n\n";
    
    oss << "Access Sizes Analyzed:\n";
    auto accessSizes = getAccessSizes();
    for (size_t size : accessSizes) {
        oss << "  " << size << " bytes\n";
    }
    
    oss << "\nAccessed Operations:\n";
    auto operations = getOperations();
    for (const auto& operation : operations) {
        oss << "  " << operation << "\n";
    }
    
    oss << "\nAccess Efficiency by Operation:\n";
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        size_t optimalSize = std::stoull(getOptimalAccessSize(operation).substr(
            getOptimalAccessSize(operation).find_last_of(' ') + 1));
        double efficiency = calculateAccessEfficiency(getAverageAccessTime(operation, optimalSize),
                                                     getAccessHitRate(operation, optimalSize));
        oss << "    Optimal: " << optimalSize << " bytes (" << (efficiency * 100) << "% efficiency)\n";
    }
    
    return oss.str();
}

std::string MemoryAccessPatternAnalyzer::getRealTimeAccessPatternStats() const {
    std::ostringstream oss;
    oss << "Real-Time Access Pattern Statistics:\n";
    oss << "==========================\n\n";
    
    oss << "Current Access Pattern State:\n";
    oss << "  Total Access Pattern Measurements: " << totalMeasurements_ << "\n";
    oss << "  Active Operations: " << getOperations().size() << "\n";
    oss << "  Access Sizes Analyzed: " << getAccessSizes().size() << "\n";
    
    if (!getOperations().empty() && !getAccessSizes().empty()) {
        oss << "\nRecent Access Pattern Measurements:\n";
        std::lock_guard<std::mutex> lock(accessesMutex_);
        
        size_t recentCount = std::min(accesses_.size(), size_t(5));
        for (size_t i = 0; i < recentCount; ++i) {
            const auto& access = accesses_[accesses_.size() - 1 - i];
            oss << "  " << access.operation << " with size " << access.accessSize << " bytes: " <<
                (access.accessTime * 1000) << " us, " << (access.cacheHit ? "hit" : "miss") << ", " <<
                (access.spatialLocality * 100) << "% spatial, " <<
                (access.temporalLocality * 100) << "% temporal\n";
        }
    }
    
    return oss.str();
}

void MemoryAccessPatternAnalyzer::cleanupOldAccesses() {
    // Remove accesses older than 24 hours
    auto now = std::chrono::system_clock::now();
    auto oneDayAgo = now - std::chrono::hours(24);
    
    std::lock_guard<std::mutex> lock(accessesMutex_);
    auto it = std::remove_if(accesses_.begin(), accesses_.end(),
                             [oneDayAgo](const MemoryAccess& a) {
                                 return a.timestamp < oneDayAgo;
                             });
    accesses_.erase(it, accesses_.end());
}

double MemoryAccessPatternAnalyzer::calculateAccessEfficiency(double accessTime, bool cacheHit) const {
    double efficiency = 1.0 / accessTime; // Inverse of access time
    
    if (cacheHit) {
        efficiency *= 2.0; // Bonus for cache hits
    }
    
    return std::min(efficiency / 1000.0, 1.0); // Normalize and cap at 1.0
}

std::string MemoryAccessPatternAnalyzer::analyzeAccessPatternType(const std::string& pattern) const {
    if (pattern == "sequential") {
        return "Sequential Access";
    } else if (pattern == "random") {
        return "Random Access";
    } else if (pattern == "strided") {
        return "Strided Access";
    } else if (pattern == "block") {
        return "Block Access";
    } else {
        return "Unknown Pattern";
    }
}

double MemoryAccessPatternAnalyzer::calculateSpatialLocalityScore(size_t accessSize, size_t cacheLine) const {
    if (cacheLine == 0) return 0.0;
    return static_cast<double>(accessSize) / cacheLine;
}

double MemoryAccessPatternAnalyzer::calculateTemporalLocalityScore(double timeBetweenAccesses) const {
    if (timeBetweenAccesses <= 0) return 0.0;
    return 1.0 / timeBetweenAccesses;
}

std::string MemoryAccessPatternAnalyzer::getAccessPatternDescription(const std::string& pattern) const {
    if (pattern == "sequential") {
        return "Sequential memory access - good spatial locality";
    } else if (pattern == "random") {
        return "Random memory access - poor spatial locality";
    } else if (pattern == "strided") {
        return "Strided memory access - moderate spatial locality";
    } else if (pattern == "block") {
        return "Block memory access - good spatial locality";
    } else {
        return "Unknown access pattern";
    }
}

bool MemoryAccessPatternAnalyzer::isAccessPatternOptimal(double spatialLocality, double temporalLocality) const {
    return spatialLocality > 0.8 && temporalLocality > 0.8;
}

bool MemoryAccessPatternAnalyzer::isCacheHitRateAcceptable(double hitRate) const {
    return hitRate > 0.8; // 80% or more
}

bool MemoryAccessPatternAnalyzer::isAccessTimeAcceptable(double accessTime) const {
    return accessTime <= 0.01; // 10 us or less
}

} // namespace nlm

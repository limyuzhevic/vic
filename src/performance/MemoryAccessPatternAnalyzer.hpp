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

// MemoryAccessPatternAnalyzer: Memory access pattern analysis
class MemoryAccessPatternAnalyzer {
public:
    MemoryAccessPatternAnalyzer();
    ~MemoryAccessPatternAnalyzer();
    
    // Configuration
    void setEnableAccessPatternAnalysis(bool enable) { enableAccessPatternAnalysis_ = enable; }
    void setEnableTemporalAnalysis(bool enable) { enableTemporalAnalysis_ = enable; }
    void setEnableSpatialAnalysis(bool enable) { enableSpatialAnalysis_ = enable; }
    void setMinAccessPatternSamples(size_t min) { minAccessPatternSamples_ = min; }
    
    // Memory access pattern tracking
    void recordMemoryAccess(const std::string& operation, size_t accessSize,
                           double accessTime, bool cacheHit, size_t cacheLine,
                           const std::string& accessPattern, double spatialLocality,
                           double temporalLocality, const std::string& context = "");
    
    // Query results
    double getAverageAccessTime(const std::string& operation, size_t accessSize) const;
    double getAccessHitRate(const std::string& operation, size_t accessSize) const;
    double getAverageSpatialLocality(const std::string& operation, size_t accessSize) const;
    double getAverageTemporalLocality(const std::string& operation, size_t accessSize) const;
    
    // Access pattern listing
    std::vector<std::string> getOperations() const;
    std::vector<size_t> getAccessSizes() const;
    
    // Analysis
    std::string analyzeMemoryAccessPatterns() const;
    std::string analyzeSpatialLocality() const;
    std::string analyzeTemporalLocality() const;
    std::string identifyAccessPatternBottlenecks() const;
    std::string recommendMemoryOptimizations() const;
    
    // Statistics
    size_t getTotalAccessPatternMeasurements() const { return totalMeasurements_; }
    std::string getOptimalAccessSize(const std::string& operation) const;
    std::string analyzeAccessPatternEfficiency() const;
    
    // Export
    bool exportAccessPatternProfile(const std::string& filepath) const;
    std::string exportAccessPatternProfileAsString() const;
    
    // Real-time analysis
    std::string getRealTimeAccessPatternStats() const;
    
private:
    struct MemoryAccess {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        size_t accessSize;
        double accessTime;
        bool cacheHit;
        size_t cacheLine;
        std::string accessPattern;
        double spatialLocality;
        double temporalLocality;
        std::string context;
        
        MemoryAccess(const std::string& op, size_t size, double time, bool hit,
                    size_t line, const std::string& pattern,
                    double spatial, double temporal, const std::string& ctx)
            : operation(op), accessSize(size), accessTime(time), cacheHit(hit),
              cacheLine(line), accessPattern(pattern), spatialLocality(spatial),
              temporalLocality(temporal), context(ctx) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    std::vector<MemoryAccess> accesses_;
    mutable std::mutex accessesMutex_;
    
    bool enableAccessPatternAnalysis_;
    bool enableTemporalAnalysis_;
    bool enableSpatialAnalysis_;
    size_t minAccessPatternSamples_;
    size_t totalMeasurements_;
    
    // Helper methods
    void cleanupOldAccesses();
    double calculateAccessEfficiency(double accessTime, bool cacheHit) const;
    std::string analyzeAccessPatternType(const std::string& pattern) const;
    double calculateSpatialLocalityScore(size_t accessSize, size_t cacheLine) const;
    double calculateTemporalLocalityScore(double timeBetweenAccesses) const;
    std::string getAccessPatternDescription(const std::string& pattern) const;
    bool isAccessPatternOptimal(double spatialLocality, double temporalLocality) const;
    bool isCacheHitRateAcceptable(double hitRate) const;
    bool isAccessTimeAcceptable(double accessTime) const;
};

} // namespace nlm

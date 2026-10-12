#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <functional>
#include <algorithm>
#include "../core/Logger.hpp"

namespace nlm {

// Performance annotation for code sections
struct PerformanceAnnotation {
    std::string functionName;
    std::string filePath;
    int lineNumber;
    std::string annotationType; // "hot", "cold", "memory_bound", "compute_bound"
    double estimatedImpact; // Potential performance improvement (0-1)
    std::string description;
    bool isOptimized; // Whether this optimization has been applied
    Timestamp createdTime;
    Timestamp lastOptimizedTime;
    std::string optimizationDetails;
};

// Performance hint for code optimization
struct PerformanceHint {
    std::string componentName;
    std::string hintType; // "memory", "cache", "parallel", "vector", "algorithm"
    std::string description;
    double potentialImprovement; // Expected performance improvement percentage
    bool isApplied; // Whether this hint has been implemented
    bool isCritical; // Whether this optimization is critical for performance
    Timestamp suggestedTime; // When to apply this hint
    std::string priority; // "high", "medium", "low"
    std::vector<std::string> implementationSteps;
};

// Cache performance analysis
struct CacheAnalysis {
    std::string cacheType; // "L1", "L2", "L3", "TLB"
    double missRate; // Cache miss rate (0-1)
    double missPenalty; // Time penalty for cache miss
    size_t totalAccesses;
    size_t cacheHits;
    size_t cacheMisses;
    std::map<std::string, double> accessPatterns; // Function -> access frequency
};

// Memory bandwidth analysis
struct MemoryBandwidthAnalysis {
    double measuredBandwidth; // GB/s
    double peakBandwidth; // GB/s
    double bandwidthUtilization; // (0-1)
    std::map<std::string, double> bandwidthPerComponent;
    std::vector<std::string> bandwidthHotspots;
};

// Computational intensity analysis
struct ComputationalIntensityAnalysis {
    double operationsPerByte; // Computational intensity
    std::map<std::string, double> intensityPerFunction;
    std::vector<std::string> computeBoundFunctions;
    std::vector<std::string> memoryBoundFunctions;
};

// Optimization analyzer for NLM components
class OptimizationHints {
public:
    OptimizationHints();
    ~OptimizationHints();

    // Code analysis
    void analyzeCodePerformance(const std::string& functionName, 
                               const std::string& filePath,
                               int lineNumber,
                               const std::vector<std::string>& codePaths);
    void analyzeMemoryAccessPatterns(const std::string& functionName,
                                    const std::vector<std::string>& memoryAccesses);
    void analyzeDataLocality(const std::string& functionName,
                           const std::vector<std::string>& dataStructures);
    
    // Performance prediction
    std::vector<PerformanceAnnotation> predictPerformanceImpact(
        const std::string& componentName,
        const std::map<std::string, double>& currentParameters);
    double estimateOptimizationGain(const std::string& hintType,
                                   const std::string& componentName);
    
    // Optimization hints generation
    void generateOptimizationHints();
    void analyzeCachePerformance();
    void analyzeMemoryBandwidth();
    void analyzeComputationalIntensity();
    
    // Hint management
    void addPerformanceHint(const PerformanceHint& hint);
    void markHintAsApplied(const std::string& componentName, const std::string& hintType);
    std::vector<PerformanceHint> getOptimizationHints() const;
    std::vector<PerformanceAnnotation> getPerformanceAnnotations() const;
    
    // Implementation tracking
    void recordOptimizationImplementation(const std::string& componentName,
                                         const std::string& optimizationType,
                                         const std::string& details);
    void updatePerformanceMetrics(const std::string& componentName,
                                 const std::map<std::string, double>& metrics);
    
    // Analysis configuration
    void setAnalysisDepth(int depth) { analysisDepth = depth; }
    void enableCodeAnnotations(bool enable) { codeAnnotationsEnabled = enable; }
    void enableMemoryAnalysis(bool enable) { memoryAnalysisEnabled = enable; }
    void enableCacheAnalysis(bool enable) { cacheAnalysisEnabled = enable; }
    
    // Statistics
    size_t getHintCount() const;
    size_t getAnnotationCount() const;
    double getOverallOptimizationPotential() const;
    std::string getOptimizationReport() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Analysis settings
    int analysisDepth;
    bool codeAnnotationsEnabled;
    bool memoryAnalysisEnabled;
    bool cacheAnalysisEnabled;
};

// Optimization hint utilities
namespace optimization_utils {
    
    // Hint severity calculation
    double calculateHintSeverity(double improvement, bool isCritical,
                                const std::string& componentType);
    
    // Priority assignment
    std::string assignPriority(double improvement, bool isCritical,
                              const std::string& deadline);
    
    // Implementation cost estimation
    double estimateImplementationCost(const std::string& hintType,
                                     const std::string& componentName,
                                     bool isCritical);
    
    // Validation of optimization hints
    bool validateHint(const PerformanceHint& hint);
    bool validateAnnotation(const PerformanceAnnotation& annotation);
    
    // Hint prioritization
    std::vector<PerformanceHint> prioritizeHints(
        const std::vector<PerformanceHint>& hints);
    
} // namespace optimization_utils

} // namespace nlm
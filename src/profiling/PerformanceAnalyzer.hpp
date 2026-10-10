// Performance Analyzer Header
#pragma once

#include <string>
#include <vector>
#include "NLMProfiler.hpp"

namespace nlm {

/**
 * Performance analyzer for NLM profiles
 */
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    /**
     * Analyze multiple profile files
     */
    void analyzeProfiles(const std::vector<std::string>& profileFiles);
    
    /**
     * Analyze a single profile file
     */
    void analyzeProfile(const std::string& profileFile);
    
    /**
     * Generate comparative report from analyzed profiles
     */
    void generateComparativeReport();
    
    /**
     * Identify performance bottlenecks
     */
    void identifyBottlenecks();
    
    /**
     * Generate optimization recommendations
     */
    void generateOptimizationRecommendations();
    
private:
    NLMProfiler _profiler; // Convenience for accessing profiler data
};

} // namespace nlm
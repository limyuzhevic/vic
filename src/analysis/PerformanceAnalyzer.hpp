// Performance Analyzer Header
#pragma once

#include <string>
#include "NLMAnalysis.hpp"

namespace nlm {

/**
 * Performance analyzer
 */
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    /**
     * Analyze performance metrics
     */
    PerformanceAnalysisResult analyzePerformance(const PerformanceMetrics& metrics);
    
    /**
     * Save results
     */
    void saveResults(const PerformanceAnalysisResult& results, const std::string& filePath);
    
private:
    double calculateEfficiency(const PerformanceMetrics& metrics);
    double calculateThroughput(const PerformanceMetrics& metrics);
    double calculateStability(const PerformanceMetrics& metrics);
    double calculateOptimization(const PerformanceMetrics& metrics);
};

} // namespace nlm
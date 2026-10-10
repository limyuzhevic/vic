// Memory Analyzer Header
#pragma once

#include <string>
#include "NLMAnalysis.hpp"

namespace nlm {

/**
 * Memory analyzer
 */
class MemoryAnalyzer {
public:
    MemoryAnalyzer();
    ~MemoryAnalyzer();
    
    /**
     * Analyze memory metrics
     */
    MemoryAnalysisResult analyzeMemory(const MemoryMetrics& metrics);
    
    /**
     * Save results
     */
    void saveResults(const MemoryAnalysisResult& results, const std::string& filePath);
    
private:
    double calculateWorkingMemoryScore(const MemoryMetrics& metrics);
    double calculateEpisodicMemoryScore(const MemoryMetrics& metrics);
    double calculateSemanticMemoryScore(const MemoryMetrics& metrics);
    double calculateProceduralMemoryScore(const MemoryMetrics& metrics);
};

} // namespace nlm
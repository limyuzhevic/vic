// Neuromodulation Analyzer Header
#pragma once

#include <string>
#include "NLMAnalysis.hpp"

namespace nlm {

/**
 * Neuromodulation analyzer
 */
class NeuromodulationAnalyzer {
public:
    NeuromodulationAnalyzer();
    ~NeuromodulationAnalyzer();
    
    /**
     * Analyze neuromodulation metrics
     */
    NeuromodulationAnalysisResult analyzeNeuromodulation(const NeuromodulationMetrics& metrics);
    
    /**
     * Save results
     */
    void saveResults(const NeuromodulationAnalysisResult& results, const std::string& filePath);
    
private:
    double calculateDopamineScore(const NeuromodulationMetrics& metrics);
    double calculateCuriosityScore(const NeuromodulationMetrics& metrics);
    double calculateNoveltyScore(const NeuromodulationMetrics& metrics);
    double calculatePredictionErrorScore(const NeuromodulationMetrics& metrics);
};

} // namespace nlm
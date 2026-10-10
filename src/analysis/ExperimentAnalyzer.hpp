// Experiment Analyzer Header
#pragma once

#include <string>
#include <vector>
#include "NLMAnalysis.hpp"

namespace nlm {

/**
 * Experiment analyzer
 */
class ExperimentAnalyzer {
public:
    ExperimentAnalyzer();
    ~ExperimentAnalyzer();
    
    /**
     * Analyze experiment result
     */
    AnalysisResult analyzeExperiment(const ExperimentResult& result);
    
    /**
     * Analyze multiple experiment results
     */
    std::vector<AnalysisResult> analyzeExperiments(const std::vector<ExperimentResult>& results);
    
    /**
     * Save analysis results
     */
    void saveAnalysis(const std::vector<AnalysisResult>& analyses, const std::string& filePath);
    
private:
    double calculatePerformanceScore(const ExperimentResult& result);
    double calculateEfficiencyScore(const ExperimentResult& result);
    double calculateLearningScore(const ExperimentResult& result);
    int countSuccessful(const std::vector<AnalysisResult>& analyses);
    double calculateAverage(const std::vector<AnalysisResult>& analyses, const std::string& metric);
};

} // namespace nlm
#pragma once

#include <string>
#include <vector>
#include "AdvancedCLI.hpp"
#include "CommandOutputFormatter.hpp"

namespace nlm {

class AnalysisCommands {
public:
    AnalysisCommands(std::shared_ptr<CommandOutputFormatter> formatter,
                     std::shared_ptr<Logger> logger);
    
    // Analysis commands
    bool analyzeNeuralActivity();
    bool analyzePlasticity();
    bool analyzeMemoryUsage();
    bool analyzePerformance();
    bool analyzeSTDPTiming();
    bool analyzeHebbianCoactivity();
    bool analyzeMemoryConsolidation();
    bool analyzeNeuromodulation();
    bool analyzePredictionErrors();
    bool analyzeExplorationExploitation();
    bool analyzeDevelopmentalStages();
    
private:
    std::shared_ptr<CommandOutputFormatter> formatter;
    std::shared_ptr<Logger> logger;
    
    // Analysis helpers
    void performNeuralActivityAnalysis();
    void performPlasticityAnalysis();
    void performMemoryUsageAnalysis();
    void performPerformanceAnalysis();
    void performSTDPAnalysis();
    void performHebbianAnalysis();
    void performMemoryConsolidationAnalysis();
    void performNeuromodulationAnalysis();
    void performPredictionErrorAnalysis();
    void performExplorationExploitationAnalysis();
    void performDevelopmentalStageAnalysis();
    
    // Data collection helpers
    std::vector<double> collectNeuralActivityData();
    std::vector<double> collectPlasticityData();
    std::vector<double> collectMemoryUsageData();
    std::vector<double> collectPerformanceData();
    
    // Analysis result formatting
    std::string formatAnalysisResults(const std::string& analysisType,
                                      const std::vector<double>& data,
                                      const std::string& summary);
};

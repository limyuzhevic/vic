#pragma once

#include <string>
#include <vector>
#include "AdvancedCLI.hpp"
#include "CommandOutputFormatter.hpp"

namespace nlm {

class AdvancedFeatures {
public:
    AdvancedFeatures(std::shared_ptr<CommandOutputFormatter> formatter,
                    std::shared_ptr<Logger> logger);
    
    // Experiment commands
    bool experimentCreate(const std::string& name);
    bool experimentRun(const std::string& name);
    bool experimentList();
    bool experimentStatus(const std::string& name);
    bool experimentResults(const std::string& name);
    bool experimentExport(const std::string& name);
    bool experimentImport(const std::string& name);
    
    // Parameter analysis commands
    bool parameterSweep(const std::string& configFile);
    bool monteCarlo(int runs);
    bool sensitivityAnalysis(const std::string& param);
    
    // Learning mode commands
    bool onlineLearning();
    bool batchLearning();
    bool curriculumLearning();
    
    // Advanced feature state
    bool isOnlineLearningEnabled() const { return onlineLearningEnabled; }
    bool isBatchLearningEnabled() const { return batchLearningEnabled; }
    bool isCurriculumLearningEnabled() const { return curriculumLearningEnabled; }
    
private:
    std::shared_ptr<CommandOutputFormatter> formatter;
    std::shared_ptr<Logger> logger;
    
    // Feature state
    bool onlineLearningEnabled = false;
    bool batchLearningEnabled = false;
    bool curriculumLearningEnabled = false;
    
    // Experiment management
    std::map<std::string, std::string> experimentStatusMap;
    std::map<std::string, std::vector<double>> experimentResultsMap;
    
    // Experiment helpers
    bool createExperimentDirectory(const std::string& name);
    bool saveExperimentConfig(const std::string& name, const std::string& config);
    bool loadExperimentConfig(const std::string& name, std::string& config);
    bool runExperiment(const std::string& name);
    bool exportExperimentResults(const std::string& name, const std::string& filename);
    bool importExperimentResults(const std::string& name, const std::string& filename);
    
    // Parameter analysis helpers
    bool performParameterSweep(const std::string& configFile);
    bool performMonteCarlo(int runs);
    bool performSensitivityAnalysis(const std::string& param);
    
    // Learning mode helpers
    bool enableOnlineLearning();
    bool enableBatchLearning();
    bool enableCurriculumLearning();
    
    // Validation helpers
    bool validateExperimentName(const std::string& name) const;
    bool validateConfigFile(const std::string& configFile) const;
    bool validateParamName(const std::string& param) const;
};

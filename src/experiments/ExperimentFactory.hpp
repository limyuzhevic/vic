// ExperimentFactory - Factory for creating experiments
#pragma once

#include "IExperiment.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace nlm {

class ExperimentFactory {
public:
    // Configuration structure
    struct ExperimentConfig {
        std::string experimentName;
        std::map<std::string, double> doubleParameters;
        std::map<std::string, std::string> stringParameters;
        std::map<std::string, bool> boolParameters;
        std::vector<std::string> tags;
        bool enableCheckpointing;
        bool enableProgressTracking;
        bool enableLogging;
        double checkpointInterval;
        std::string outputDirectory;
        
        ExperimentConfig(const std::string& name) 
            : experimentName(name), enableCheckpointing(false), 
              enableProgressTracking(true), enableLogging(true), 
              checkpointInterval(100.0), outputDirectory("./results") {}
    };
    
    // Creation
    static std::unique_ptr<IExperiment> createExperiment(
        const std::string& experimentName,
        const ExperimentConfig& config);
    
    // Batch creation
    static std::vector<std::unique_ptr<IExperiment>> createExperimentBatch(
        const std::vector<std::string>& experimentNames,
        const ExperimentConfig& config);
    
    // Configuration from file
    static bool loadConfigFromFile(const std::string& filepath, ExperimentConfig& config);
    static bool saveConfigToFile(const std::string& filepath, const ExperimentConfig& config);
    
    // Preset configurations
    static ExperimentConfig getDefaultConfig(const std::string& experimentName);
    static ExperimentConfig getPhase2Config(const std::string& experimentName);
    static ExperimentConfig getPhase6Config(const std::string& experimentName);
    
private:
    static std::map<std::string, ExperimentConfig> defaultConfigs;
};

} // namespace nlm

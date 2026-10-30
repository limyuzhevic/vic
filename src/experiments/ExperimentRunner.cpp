#include "ExperimentRunner.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

// Global experiment runner instance
static ExperimentRunner& getExperimentRunner() {
    static ExperimentRunner runner;
    return runner;
}

// Phase 6 Integration Experiment implementation
class Phase6IntegrationExperiment : public Experiment {
public:
    std::string getName() const override {
        return "phase6_integration";
    }
    
    bool validateConfig(const ExperimentConfig& config) override {
        if (config.neuronCount < 100 || config.neuronCount > 10000) {
            return false;
        }
        if (config.maxSteps < 100 || config.maxSteps > 100000) {
            return false;
        }
        return true;
    }
    
    std::vector<std::string> getRequiredParameters() const override {
        return {"neuronCount", "maxSteps"};
    }
    
    std::vector<std::string> getOptionalParameters() const override {
        return {"regionCount", "connectionProbability", "enableCheckpointing", 
                "enableReplay", "enableDevelopment", "checkpointPath", "seed"};
    }
    
    ExperimentResult run(const ExperimentConfig& config) override {
        ExperimentResult result;
        result.name = getName();
        
        try {
            // Convert to Phase6Config
            Phase6Config phase6Config;
            phase6Config.neuronCount = config.neuronCount;
            phase6Config.maxSteps = config.maxSteps;
            phase6Config.enableCheckpointing = config.enableCheckpointing;
            phase6Config.enableReplay = config.enableReplay;
            phase6Config.enableDevelopment = config.enableDevelopment;
            phase6Config.regionCount = config.regionCount;
            phase6Config.connectionProbability = config.connectionProbability;
            phase6Config.checkpointPath = config.checkpointPath;
            
            // Run the actual experiment
            Phase6IntegratedExperiment phase6Experiment;
            result = convertResult(phase6Experiment.run(phase6Config));
            
        } catch (const std::exception& e) {
            result.customMetrics["error"] = 1.0;
            result.customMetrics["error_message"] = e.what();
        }
        
        return result;
    }
    
private:
    ExperimentResult convertResult(const Phase6IntegrationResult& phase6Result) {
        ExperimentResult result;
        result.name = getName();
        result.totalReward = phase6Result.totalReward;
        result.avgFiringRate = phase6Result.avgFiringRate;
        result.memoryEpisodesStored = phase6Result.memoryEpisodesStored;
        result.noveltyLevel = phase6Result.noveltyLevel;
        result.curiosityLevel = phase6Result.curiosityLevel;
        result.dopamineLevel = phase6Result.dopamineLevel;
        result.totalWallClockTime = phase6Result.totalWallClockTime;
        result.memoryWorkingMemoryIntegrated = phase6Result.memoryWorkingMemoryIntegrated;
        result.memoryEpisodicMemoryIntegrated = phase6Result.memoryEpisodicMemoryIntegrated;
        result.neuromodulationIntegrated = phase6Result.neuromodulationIntegrated;
        result.predictionIntegrated = phase6Result.predictionIntegrated;
        result.developmentIntegrated = phase6Result.developmentIntegrated;
        result.checkpointingWorks = phase6Result.checkpointingWorks;
        
        return result;
    }
};

// Register the Phase 6 integration experiment
REGISTER_EXPERIMENT(Phase6IntegrationExperiment)

// Helper function to create experiment from command line arguments
ExperimentConfig createConfigFromCLI(const std::map<std::string, std::string>& cliArgs) {
    ExperimentConfig config;
    
    auto it = cliArgs.find("neurons");
    if (it != cliArgs.end()) {
        config.neuronCount = std::stoi(it->second);
    }
    
    it = cliArgs.find("steps");
    if (it != cliArgs.end()) {
        config.maxSteps = std::stoi(it->second);
    }
    
    it = cliArgs.find("regions");
    if (it != cliArgs.end()) {
        config.regionCount = std::stoi(it->second);
    }
    
    it = cliArgs.find("connection_probability");
    if (it != cliArgs.end()) {
        config.connectionProbability = std::stof(it->second);
    }
    
    it = cliArgs.find("checkpoint");
    if (it != cliArgs.end()) {
        config.enableCheckpointing = true;
        config.checkpointPath = it->second;
    }
    
    it = cliArgs.find("replay");
    if (it != cliArgs.end()) {
        config.enableReplay = true;
    }
    
    it = cliArgs.find("development");
    if (it != cliArgs.end()) {
        config.enableDevelopment = true;
    }
    
    it = cliArgs.find("seed");
    if (it != cliArgs.end()) {
        config.seed = std::stoi(it->second);
    }
    
    return config;
}

}
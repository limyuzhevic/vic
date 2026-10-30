#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>

namespace nlm {

// Experiment result structure
struct ExperimentResult {
    std::string name;
    double totalReward;
    double avgFiringRate;
    float memoryEpisodesStored;
    double noveltyLevel;
    double curiosityLevel;
    double dopamineLevel;
    double totalWallClockTime;
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    std::map<std::string, double> customMetrics;
    std::chrono::system_clock::time_point timestamp;
    
    ExperimentResult() : totalReward(0.0), avgFiringRate(0.0), 
                         memoryEpisodesStored(0.0), noveltyLevel(0.0),
                         curiosityLevel(0.0), dopamineLevel(0.0),
                         totalWallClockTime(0.0), memoryWorkingMemoryIntegrated(false),
                         memoryEpisodicMemoryIntegrated(false), neuromodulationIntegrated(false),
                         predictionIntegrated(false), developmentIntegrated(false),
                         checkpointingWorks(false) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Experiment configuration
struct ExperimentConfig {
    std::string name;
    int neuronCount;
    int maxSteps;
    int regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    std::string checkpointPath;
    bool enableReplay;
    bool enableDevelopment;
    int seed;
    std::map<std::string, std::string> customParameters;
    
    ExperimentConfig() : neuronCount(1000), maxSteps(5000), regionCount(1),
                        connectionProbability(0.15f), enableCheckpointing(false),
                        enableReplay(true), enableDevelopment(false), seed(42) {
    }
};

// Abstract experiment base class
class Experiment {
public:
    virtual ~Experiment() = default;
    
    virtual std::string getName() const = 0;
    virtual ExperimentResult run(const ExperimentConfig& config) = 0;
    virtual bool validateConfig(const ExperimentConfig& config) = 0;
    virtual std::vector<std::string> getRequiredParameters() const = 0;
    virtual std::vector<std::string> getOptionalParameters() const = 0;
};

// Experiment runner class
class ExperimentRunner {
public:
    ExperimentRunner() = default;
    
    // Register an experiment
    void registerExperiment(std::unique_ptr<Experiment> experiment) {
        experiments_[experiment->getName()] = std::move(experiment);
    }
    
    // Run experiment by name
    ExperimentResult runExperiment(const std::string& name, const ExperimentConfig& config) {
        auto it = experiments_.find(name);
        if (it == experiments_.end()) {
            throw std::runtime_error("Experiment not found: " + name);
        }
        
        if (!it->second->validateConfig(config)) {
            throw std::runtime_error("Invalid configuration for experiment: " + name);
        }
        
        return it->second->run(config);
    }
    
    // List available experiments
    std::vector<std::string> listExperiments() const {
        std::vector<std::string> names;
        for (const auto& pair : experiments_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get experiment info
    struct ExperimentInfo {
        std::string name;
        std::vector<std::string> requiredParams;
        std::vector<std::string> optionalParams;
        std::string description;
    };
    
    ExperimentInfo getExperimentInfo(const std::string& name) const {
        ExperimentInfo info;
        auto it = experiments_.find(name);
        if (it != experiments_.end()) {
            info.name = name;
            info.requiredParams = it->second->getRequiredParameters();
            info.optionalParams = it->second->getOptionalParameters();
            info.description = "Experiment " + name;
        }
        return info;
    }
    
    // Save experiment result to file
    bool saveResult(const ExperimentResult& result, const std::string& filepath) const {
        // TODO: Implement result serialization
        // For now, just create a simple text file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Experiment: " << result.name << std::endl;
        file << "Timestamp: " << std::chrono::system_clock::to_time_t(result.timestamp) << std::endl;
        file << "Total Reward: " << result.totalReward << std::endl;
        file << "Average Firing Rate: " << result.avgFiringRate << std::endl;
        file << "Memory Episodes: " << result.memoryEpisodesStored << std::endl;
        file << "Novelty Level: " << result.noveltyLevel << std::endl;
        file << "Curiosity Level: " << result.curiosityLevel << std::endl;
        file << "Dopamine Level: " << result.dopamineLevel << std::endl;
        file << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
        file << "Working Memory Integrated: " << (result.memoryWorkingMemoryIntegrated ? "Yes" : "No") << std::endl;
        file << "Episodic Memory Integrated: " << (result.memoryEpisodicMemoryIntegrated ? "Yes" : "No") << std::endl;
        file << "Neuromodulation Integrated: " << (result.neuromodulationIntegrated ? "Yes" : "No") << std::endl;
        file << "Prediction Integrated: " << (result.predictionIntegrated ? "Yes" : "No") << std::endl;
        file << "Development Integrated: " << (result.developmentIntegrated ? "Yes" : "No") << std::endl;
        file << "Checkpointing Works: " << (result.checkpointingWorks ? "Yes" : "No") << std::endl;
        
        file.close();
        return true;
    }
    
    // Load experiment configuration from file
    bool loadConfig(const std::string& filepath, ExperimentConfig& config) const {
        // TODO: Implement proper JSON/YAML config loading
        // For now, just parse a simple format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                config.customParameters[key] = value;
            }
        }
        
        file.close();
        return true;
    }
    
private:
    std::map<std::string, std::unique_ptr<Experiment>> experiments_;
};

// Macro for registering experiments
#define REGISTER_EXPERIMENT(ExperimentClass) \
    namespace { \
        struct ExperimentClassRegistrar { \
            ExperimentClassRegistrar() { \
                static ExperimentRunner& runner = ExperimentRunner::getInstance(); \
                runner.registerExperiment(std::make_unique<ExperimentClass>()); \
            } \
        }; \
        ExperimentClassRegistrar experimentClassRegistrar; \
    }

// Get global experiment runner instance
ExperimentRunner& getExperimentRunner();

}
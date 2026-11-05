// nlm/BrainFactory.hpp - Factory class for creating standardized brain instances
#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

// Factory configuration options for common use cases
struct BrainFactoryConfig {
    // Basic configuration
    size_t neuronCount = constants::DEFAULT_NEURON_COUNT;
    size_t regionCount = constants::DEFAULT_REGION_COUNT;
    float connectionProbability = constants::DEFAULT_CONNECTION_PROBABILITY;
    
    // Plasticity settings
    bool enableSTDP = true;
    bool enableHebbian = true;
    bool enableStructuralPlasticity = true;
    float stdpLTPWeight = constants::DEFAULT_STDP_LTP_WEIGHT;
    float stdpLTDWeight = constants::DEFAULT_STDP_LTD_WEIGHT;
    float stdpTau = constants::DEFAULT_STDP_TIME_CONST;
    float hebbianWeight = constants::DEFAULT_HBP_WEIGHT;
    float synaptogenesisRate = constants::DEFAULT_SYNAPTOGENESIS_RATE;
    float pruningRate = constants::DEFAULT_PRUNING_RATE;
    
    // Neuromodulation
    bool enableDopamine = true;
    bool enableCuriosity = false;
    bool enableNovelty = false;
    bool enablePredictionError = true;
    
    // Memory systems
    bool enableWorkingMemory = true;
    bool enableEpisodicMemory = false;
    bool enableAssociativeMemory = false;
    size_t workingMemorySize = constants::DEFAULT_WORKING_MEMORY_SIZE;
    
    // Cognitive systems
    bool enablePredictionSystem = true;
    bool enablePlanning = false;
    bool enableConceptFormation = false;
    bool enableAttention = false;
    
    // Development
    bool enableDevelopment = false;
    float developmentStartTime = 100.0f;
    float criticalPeriodEnd = 200.0f;
    
    // Performance
    bool enableOptimization = true;
    bool enableSIMD = true;
    bool enableParallel = true;
    
    // Simulation
    double timestep = constants::DEFAULT_TIMESTEP;
    size_t maxSteps = 1000;
    
    // Randomization
    uint64_t seed = constants::DEFAULT_RANDOM_SEED;
    
    // Custom configurations
    std::string customName;
    std::unordered_map<std::string, std::string> customSettings;
};

// Brain factory class for creating standardized brain instances
class BrainFactory {
public:
    // Static factory methods for common brain types
    static std::shared_ptr<Brain> createStandardBrain(const BrainFactoryConfig& config = BrainFactoryConfig{});
    static std::shared_ptr<Brain> createMinimalBrain();
    static std::shared_ptr<Brain> createResearchBrain();
    static std::shared_ptr<Brain> createDemoBrain(const std::string& demoType = "standard");
    
    // Advanced factory methods
    static std::shared_ptr<Brain> createBrainWithDefaults();
    static std::shared_ptr<Brain> createBrainFromConfigFile(const std::string& configPath);
    static std::shared_ptr<Brain> createBrainWithProfile(const std::string& profileName);
    
    // Configuration helper methods
    static void applyConfigToBrain(std::shared_ptr<Brain> brain, const BrainFactoryConfig& config);
    static BrainFactoryConfig getDefaultConfigForDemo(const std::string& demoType);
    static BrainFactoryConfig getConfigFromProfile(const std::string& profileName);
    
    // Validation and setup
    static bool validateConfig(const BrainFactoryConfig& config);
    static void printConfigInfo(const BrainFactoryConfig& config);
    
    // Profile management
    static std::vector<std::string> getAvailableProfiles();
    static bool saveProfile(const std::string& profileName, const BrainFactoryConfig& config);
    static bool loadProfile(const std::string& profileName, BrainFactoryConfig& config);
    
    // Quick setup presets
    struct Presets {
        static BrainFactoryConfig quickTest() {
            BrainFactoryConfig config;
            config.neuronCount = 100;
            config.regionCount = 1;
            config.enableSTDP = true;
            config.enableHebbian = true;
            config.enableDevelopment = false;
            config.maxSteps = 50;
            return config;
        }
        
        static BrainFactoryConfig learningDemo() {
            BrainFactoryConfig config;
            config.neuronCount = 500;
            config.regionCount = 2;
            config.connectionProbability = 0.1f;
            config.enableSTDP = true;
            config.enableHebbian = true;
            config.enableCuriosity = true;
            config.enableDevelopment = true;
            config.maxSteps = 200;
            return config;
        }
        
        static BrainFactoryConfig performanceTest() {
            BrainFactoryConfig config;
            config.neuronCount = 2000;
            config.regionCount = 4;
            config.connectionProbability = 0.05f;
            config.enableOptimization = true;
            config.enableSIMD = true;
            config.enableParallel = true;
            config.maxSteps = 1000;
            return config;
        }
        
        static BrainFactoryConfig integrationTest() {
            BrainFactoryConfig config;
            config.neuronCount = 1000;
            config.regionCount = 2;
            config.connectionProbability = 0.15f;
            config.enableSTDP = true;
            config.enableHebbian = true;
            config.enableStructuralPlasticity = true;
            config.enableWorkingMemory = true;
            config.enableEpisodicMemory = true;
            config.enablePredictionSystem = true;
            config.enableDevelopment = true;
            config.maxSteps = 500;
            return config;
        }
    };
    
private:
    BrainFactory() = delete; // Static class only
    ~BrainFactory() = delete;
    
    // Internal helper methods
    static std::shared_ptr<Config> createConfigFromFactoryConfig(const BrainFactoryConfig& config);
    static void applyDefaultSettings(std::shared_ptr<Config> config, const BrainFactoryConfig& factoryConfig);
    static bool isConfigValid(const BrainFactoryConfig& config);
    static void logFactoryConfig(const BrainFactoryConfig& config);
};

} // namespace nlm

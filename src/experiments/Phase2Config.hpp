// Phase 2 Configuration Header
// Provides experiment-specific configuration for Phase 2 neural computation

#pragma once

#include <string>
#include <map>
#include <vector>
#include <chrono>

namespace nlm {

// Phase 2 specific experiment configuration
struct Phase2Config {
    // Core simulation parameters
    int neuronCount = 500;
    int regionCount = 1;
    double simulationTimestep = 0.001;
    double connectionProbability = 0.15;
    
    // Plasticity parameters
    float stdpLTPWeight = 0.02f;
    float stdpLTDWeight = 0.015f;
    float stdpTimeConstant = 20.0f;
    float hebbianLearningRate = 0.01f;
    float maxSynapticWeight = 2.0f;
    
    // Development parameters
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    float maturationRate = 0.00005f;
    
    // Neuromodulation parameters
    float dopamineBaseline = 0.0f;
    float curiosityBaseline = 0.0f;
    float noveltyThreshold = 0.5f;
    float predictionErrorDecay = 0.1f;
    
    // Reward parameters
    float rewardDiscountFactor = 0.99f;
    float rewardScale = 1.0f;
    
    // Memory parameters
    int workingMemoryCapacity = 100;
    int episodicMemoryCapacity = 1000;
    int associativeMemoryCapacity = 1000;
    int proceduralMemoryCapacity = 100;
    
    // Prediction parameters
    int predictionWindow = 10;
    float predictionConfidenceThreshold = 0.5f;
    
    // Cognition parameters
    int planningHorizon = 5;
    int attentionSpan = 20;
    float conceptUpdateRate = 0.1f;
    
    // Environment parameters
    std::string environmentName = "SimpleWorld";
    int environmentWidth = 10;
    int environmentHeight = 10;
    
    // Logging and visualization
    std::string logLevel = "INFO";
    bool logToFile = false;
    std::string logFilename = "nlm.log";
    bool visualizationEnabled = false;
    float visualizationUpdateRate = 30.0f;
    
    // Experiment parameters
    std::string experimentName = "phase2_default";
    std::string experimentDescription = "Default Phase 2 neural computation experiment";
    bool enableCheckpointing = true;
    int checkpointInterval = 1000;
    bool enableReplay = true;
    int replayInterval = 100;
    bool enableDevelopment = true;
    int developmentInterval = 100;
    
    // Performance and limits
    int maxSimulationSteps = 10000;
    double simulationTimeLimit = 0.0;
    
    // Test configuration
    int connectivityTestSteps = 50;
    int plasticityTestSteps = 1000;
    int stdpTestTrials = 50;
    float currentStrength = 60.0f;
    float excitatoryCurrent = 30.0f;
    float synapticThreshold = 0.01f;
    
    // Result storage
    std::map<std::string, std::string> customParameters;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    // Default values
    static Phase2Config getDefaultPhase2Config() {
        return Phase2Config();
    }
    
    // Load from Config object
    void fromConfig(const std::shared_ptr<const Config>& config);
    
    // Save to Config object
    void toConfig(std::shared_ptr<Config>& config) const;
    
    // Load from file (experiment-specific format)
    bool loadFromFile(const std::string& filepath);
    
    // Save to file (experiment-specific format)
    bool saveToFile(const std::string& filepath) const;
    
    // Validate configuration
    bool validate() const;
    
    // Get configuration summary as string
    std::string summary() const;
};

// Phase 2 experiment results structure
struct Phase2Results {
    // Basic statistics
    double totalWallClockTime = 0.0;
    SimulationStep totalSteps = 0;
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    
    // Learning metrics
    float learningScore = 0.0f;
    float plasticityScore = 0.0f;
    float stdpScore = 0.0f;
    float memoryScore = 0.0f;
    float predictionScore = 0.0f;
    
    // System integration scores
    float integrationScore = 0.0f;
    float neuromodulationScore = 0.0f;
    float developmentScore = 0.0f;
    float cognitionScore = 0.0f;
    
    // Memory system performance
    size_t memoryWorkingMemoryUsed = 0;
    size_t memoryEpisodicMemoryUsed = 0;
    size_t memoryAssociativeMemoryUsed = 0;
    size_t memoryProceduralMemoryUsed = 0;
    
    // Neuromodulation levels
    float dopamineLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float noveltyLevel = 0.0f;
    float predictionErrorLevel = 0.0f;
    
    // Cognitive performance
    size_t episodesStored = 0;
    size_t conceptsFormed = 0;
    size_t plansGenerated = 0;
    size_t actionsSelected = 0;
    
    // Reward and reinforcement
    float totalReward = 0.0f;
    float avgFiringRate = 0.0f;
    size_t totalSpikes = 0;
    size_t firingNeuronCount = 0;
    
    // System health
    bool integrationSuccessful = false;
    bool memoryWorkingMemoryIntegrated = false;
    bool memoryEpisodicMemoryIntegrated = false;
    bool memoryAssociativeMemoryIntegrated = false;
    bool neuromodulationIntegrated = false;
    bool predictionIntegrated = false;
    bool developmentIntegrated = false;
    bool checkpointingWorks = false;
    bool replayWorks = false;
    
    // Experimental observations
    std::vector<std::string> observations;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    // Save/load support
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Get summary as string
    std::string summary() const;
    
    // Add observation
    void addObservation(const std::string& observation);
    
    // Add warning
    void addWarning(const std::string& warning);
    
    // Add error
    void addError(const std::string& error);
};

// Experiment runner with result persistence
class Phase2ExperimentRunner {
public:
    Phase2ExperimentRunner();
    ~Phase2ExperimentRunner();
    
    // Run Phase 2 experiments with configuration and result storage
    bool run(const Phase2Config& config, Phase2Results& results);
    
    // Run specific tests with validation
    bool runConnectivityTest(Brain* brain, Phase2Results& results);
    bool runPlasticityTest(Brain* brain, const Phase2Config& config, Phase2Results& results);
    bool runStdpTest(Brain* brain, const Phase2Config& config, Phase2Results& results);
    
    // Integration tests
    bool testIntegration(Brain* brain, Phase2Results& results);
    bool testMemoryIntegration(Brain* brain, Phase2Results& results);
    bool testNeuromodulationIntegration(Brain* brain, Phase2Results& results);
    bool testDevelopmentIntegration(Brain* brain, Phase2Results& results);
    bool testCheckpointing(Brain* brain, Phase2Results& results);
    bool testReplay(Brain* brain, Phase2Results& results);
    
    // Configuration utilities
    Phase2Config loadConfigFromFile(const std::string& filepath);
    bool saveConfigToFile(const Phase2Config& config, const std::string& filepath);
    Phase2Config mergeConfigs(const Phase2Config& baseConfig, const Phase2Config& overrideConfig);
    
    // Result utilities
    bool saveResultsToFile(const Phase2Results& results, const std::string& filepath);
    Phase2Results loadResultsFromFile(const std::string& filepath);
    
    // Experiment utilities
    std::string generateExperimentName();
    std::string generateTimestamp();
    std::string getExperimentSummary(const Phase2Config& config, const Phase2Results& results);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
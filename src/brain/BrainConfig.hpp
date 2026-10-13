// BrainConfig.hpp - Centralized configuration and constants for NLM

#pragma once

#include <string>
#include <map>
#include <vector>

namespace nlm {

// ========== CORE CONSTANTS ==========

struct CoreConstants {
    // Simulation parameters
    static constexpr double DEFAULT_TIMESTEP = 0.001;  // 1ms
    static constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
    static constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;
    static constexpr size_t DEFAULT_DEVELOPMENT_INTERVAL = 1000;
    static constexpr size_t DEFAULT_STRUCTURAL_PLACEMENT_INTERVAL = 100;
    
    // Neuron and network parameters
    static constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    static constexpr size_t DEFAULT_REGION_COUNT = 1;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    static constexpr float DEFAULT_EXCITATION_INHIBITION_RATIO = 1.0f;
    
    // Plasticity parameters
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    static constexpr float DEFAULT_STDP_TAU = 20.0f;
    static constexpr float DEFAULT_SYNAPTONGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Neuromodulation parameters
    static constexpr float DEFAULT_DOPAMINE_SCALE = 1.0f;
    static constexpr float DEFAULT_NOVELTY_SENSITIVITY = 1.0f;
    static constexpr float DEFAULT_CURIOUSITY_THRESHOLD = 0.3f;
    static constexpr float DEFAULT_EXPLORATION_CHANCE = 0.3f;
    
    // Memory system parameters
    static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY_FACTOR = 10;
    static constexpr size_t DEFAULT_EPISODIC_MEMORY_MAX_EPISODES = 1000;
    static constexpr float DEFAULT_MEMORY_CONSOLIDATION_THRESHOLD = 0.3f;
    
    // Development parameters
    static constexpr float INITIAL_DEVELOPMENT_PLACTIVITY = 1.0f;
    static constexpr float CRITICAL_PERIOD_PLACTIVITY = 0.8f;
    static constexpr float MATURATION_PLACTIVITY = 0.5f;
    static constexpr float ADULT_PLACTIVITY = 0.2f;
    
    // Development stages
    static constexpr double INITIAL_STAGE_DURATION = 60.0;   // ~1 minute
    static constexpr double CRITICAL_PERIOD_DURATION = 240.0;  // ~4 minutes
    static constexpr double MATURATION_DURATION = 720.0;       // ~12 minutes
    static constexpr double ADULT_DURATION = 10800.0;         // ~3 hours
    
    // Logging and debugging
    static constexpr const char* DEFAULT_LOG_FILE = "nlm.log";
    static constexpr size_t DEFAULT_LOG_MAX_SIZE = 10000000;  // 10MB
    static constexpr size_t DEFAULT_LOG_MAX_FILES = 5;
    
    // Checkpointing
    static constexpr const char* DEFAULT_CHECKPOINT_DIR = "./checkpoints";
    static constexpr size_t DEFAULT_CHECKPOINT_MAX_SIZE = 10000;
    static constexpr float DEFAULT_CHECKPOINT_COMPRESSION = 0.7f;
    
    // Visualization
    static constexpr size_t DEFAULT_VISION_WIDTH = 8;
    static constexpr size_t DEFAULT_VISION_HEIGHT = 8;
    static constexpr float DEFAULT_VISION_SENSITIVITY = 1.0f;
};

// ========== CONFIGURATION OPTIONS ==========

struct ConfigurationOptions {
    // Core simulation settings
    std::string configFile;
    std::string checkpointDir;
    std::string logFile;
    
    // Network architecture
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // Neuron dynamics
    float neuronThreshold;
    float neuronRestingPotential;
    float neuronResetPotential;
    float membraneTimeConstant;
    float refractoryPeriod;
    
    // Plasticity mechanisms
    bool enableSTDP;
    float stdpLTPWeight;
    float stdpLTDWeight;
    float stdpTimeConstant;
    
    bool enableHebbian;
    float hebbianLearningRate;
    
    bool enableStructuralPlasticity;
    float synaptogenesisRate;
    float pruningRate;
    
    // Neuromodulation
    bool enableDopamine;
    float dopamineScale;
    
    bool enableCuriosity;
    float curiosityThreshold;
    
    bool enableNovelty;
    float noveltySensitivity;
    
    bool enablePredictionError;
    
    // Memory systems
    bool enableWorkingMemory;
    size_t workingMemoryCapacity;
    
    bool enableEpisodicMemory;
    size_t episodicMemoryMaxEpisodes;
    
    bool enableAssociativeMemory;
    float associativeMemoryLearningRate;
    
    // Development
    bool enableDevelopment;
    double developmentTimestep;
    
    // Cognition
    bool enablePrediction;
    bool enablePlanning;
    int planningDepth;
    
    bool enableAttention;
    float attentionInhibitionStrength;
    float attentionExcitationStrength;
    
    bool enableConceptFormation;
    
    // Simulation control
    size_t maxSteps;
    double simulationTimestep;
    bool enableCheckpointing;
    bool enableReplay;
    
    // Performance
    bool enableVectorization;
    bool enableParallelProcessing;
    size_t numThreads;
    
    // Logging and debugging
    bool enableLogging;
    std::string logLevel;
    bool enableDebugOutput;
    
    // Visualization
    bool enableVisualization;
    size_t visualizationWidth;
    size_t visualizationHeight;
    
    ConfigurationOptions() : 
        configFile(""),
        checkpointDir(CoreConstants::DEFAULT_CHECKPOINT_DIR),
        logFile(CoreConstants::DEFAULT_LOG_FILE),
        neuronCount(CoreConstants::DEFAULT_NEURON_COUNT),
        regionCount(CoreConstants::DEFAULT_REGION_COUNT),
        connectionProbability(CoreConstants::DEFAULT_CONNECTION_PROBABILITY),
        neuronThreshold(-50.0f),
        neuronRestingPotential(-70.0f),
        neuronResetPotential(-75.0f),
        membraneTimeConstant(20.0f),
        refractoryPeriod(2.0f),
        enableSTDP(true),
        stdpLTPWeight(CoreConstants::DEFAULT_STDP_LTP_WEIGHT),
        stdpLTDWeight(CoreConstants::DEFAULT_STDP_LTD_WEIGHT),
        stdpTimeConstant(CoreConstants::DEFAULT_STDP_TAU),
        enableHebbian(true),
        hebbianLearningRate(0.001f),
        enableStructuralPlasticity(true),
        synaptogenesisRate(CoreConstants::DEFAULT_SYNAPTONGENESIS_RATE),
        pruningRate(CoreConstants::DEFAULT_PRUNING_RATE),
        enableDopamine(true),
        dopamineScale(CoreConstants::DEFAULT_DOPAMINE_SCALE),
        enableCuriosity(true),
        curiosityThreshold(CoreConstants::DEFAULT_CURIOUSITY_THRESHOLD),
        enableNovelty(true),
        noveltySensitivity(CoreConstants::DEFAULT_NOVELTY_SENSITIVITY),
        enablePredictionError(true),
        enableWorkingMemory(true),
        workingMemoryCapacity(0),  // Will be calculated dynamically
        enableEpisodicMemory(true),
        episodicMemoryMaxEpisodes(CoreConstants::DEFAULT_EPISODIC_MEMORY_MAX_EPISODES),
        enableAssociativeMemory(true),
        associativeMemoryLearningRate(0.01f),
        enableDevelopment(true),
        developmentTimestep(1.0),
        enablePrediction(true),
        enablePlanning(true),
        planningDepth(5),
        enableAttention(true),
        attentionInhibitionStrength(0.5f),
        attentionExcitationStrength(1.5f),
        enableConceptFormation(true),
        maxSteps(10000),
        simulationTimestep(CoreConstants::DEFAULT_TIMESTEP),
        enableCheckpointing(true),
        enableReplay(true),
        enableVectorization(true),
        enableParallelProcessing(true),
        numThreads(4),
        enableLogging(true),
        logLevel("INFO"),
        enableDebugOutput(false),
        enableVisualization(false),
        visualizationWidth(80),
        visualizationHeight(60) {}
};

// ========== CONFIGURATION UTILITIES ==========

namespace Configuration {
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath, ConfigurationOptions& options);
    
    // Load configuration from command line arguments
    bool loadFromArgs(int argc, char** argv, ConfigurationOptions& options);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath, const ConfigurationOptions& options);
    
    // Get default configuration
    ConfigurationOptions getDefaultConfig();
    
    // Validate configuration
    bool validateConfiguration(const ConfigurationOptions& options);
    
    // Print configuration summary
    void printConfigurationSummary(const ConfigurationOptions& options);
    
    // Get configuration value as string
    template<typename T>
    std::string getValueAsString(const std::string& key, const T& value);
    
    // Set configuration value from string
    template<typename T>
    bool setValueFromString(const std::string& key, const std::string& value, T& output);
}

// ========== ADVANCED CONFIGURATION OPTIONS ==========

struct AdvancedConfiguration {
    // Advanced neural dynamics
    float spikeDetectionThreshold;
    float synapticCurrentScaling;
    bool enableNeuronBursting;
    float burstThreshold;
    float burstDuration;
    
    // Advanced plasticity
    bool enableHomeostaticPlasticity;
    float homeostaticTargetRate;
    float homeostaticLearningRate;
    bool enableMetaPlasticity;
    float metaPlasticityRate;
    
    // Advanced neuromodulation
    bool enableEmotionalModulation;
    float emotionalWeight;
    bool enableStressResponse;
    float stressThreshold;
    
    // Advanced memory
    bool enableEpisodicMemoryReplay;
    size_t replayMaxEpisodes;
    bool enableMemoryPrioritization;
    float priorityThreshold;
    
    // Advanced development
    bool enableExperienceDependentDevelopment;
    float developmentSensitivity;
    bool enableAgingSimulation;
    float agingRate;
    
    // Advanced cognition
    bool enableTheoryOfMind;
    float theoryOfMindConfidence;
    bool enableMetacognition;
    float metacognitionThreshold;
    
    // Advanced simulation
    bool enableTimeWarp;
    float timeWarpSpeed;
    bool enablePauseAndPlay;
    
    AdvancedConfiguration() :
        spikeDetectionThreshold(0.8f),
        synapticCurrentScaling(10.0f),
        enableNeuronBursting(true),
        burstThreshold(1.5f),
        burstDuration(5.0f),
        enableHomeostaticPlasticity(true),
        homeostaticTargetRate(0.1f),
        homeostaticLearningRate(0.001f),
        enableMetaPlasticity(true),
        metaPlasticityRate(0.01f),
        enableEmotionalModulation(true),
        emotionalWeight(0.5f),
        enableStressResponse(true),
        stressThreshold(0.8f),
        enableEpisodicMemoryReplay(true),
        replayMaxEpisodes(10),
        enableMemoryPrioritization(true),
        priorityThreshold(0.5f),
        enableExperienceDependentDevelopment(true),
        developmentSensitivity(0.1f),
        enableAgingSimulation(true),
        agingRate(0.001f),
        enableTheoryOfMind(true),
        theoryOfMindConfidence(0.3f),
        enableMetacognition(true),
        metacognitionThreshold(0.6f),
        enableTimeWarp(false),
        timeWarpSpeed(1.0f),
        enablePauseAndPlay(true) {}
};

} // namespace nlm
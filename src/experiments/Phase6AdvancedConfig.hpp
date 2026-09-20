// Phase 6 Advanced Configuration Header
// Provides enhanced configuration options for power users and advanced testing

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <chrono>

namespace nlm {

/**
 * Advanced Phase 6 Configuration
 * Extends Phase6Config with additional power user options
 */
struct Phase6AdvancedConfig {
    // Basic experiment parameters
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    // Power user options
    bool enableVerboseLogging;
    bool enablePerformanceMetrics;
    bool enableDetailedProfiling;
    bool enableRandomSeedOverride;
    uint64_t randomSeed;
    
    // Simulation control
    float timestepDuration;
    size_t replayInterval;
    size_t consolidationInterval;
    size_t synapticPruningInterval;
    size_t conceptUpdateInterval;
    
    // Memory system options
    size_t workingMemoryCapacity;
    size_t maxEpisodicEpisodes;
    float memoryConsolidationStrength;
    float memoryReplayStrength;
    
    // Neuromodulation options
    float dopamineBaseline;
    float curiosityBaseline;
    float noveltyBaseline;
    float predictionErrorBaseline;
    bool enableExternalNeuromodulation;
    
    // Plasticity options
    float stdpLTPWeight;
    float stdpLTDWeight;
    float stdpTimeConstant;
    float hebbianLearningRate;
    float structuralSynaptogenesisRate;
    float structuralPruningRate;
    float developmentalPlasticityModifier;
    
    // Cognition options
    size_t planningDepth;
    float attentionInhibitionStrength;
    float attentionExcitationStrength;
    float conceptFormationThreshold;
    size_t conceptUpdateFrequency;
    
    // World and sensory options
    size_t worldWidth;
    size_t worldHeight;
    std::vector<float> sensoryInputRanges;
    std::vector<float> motorOutputRanges;
    float sensoryNoiseLevel;
    float motorNoiseLevel;
    
    // Checkpoint and persistence
    std::string checkpointDirectory;
    bool checkpointCompress;
    size_t checkpointMaxVersions;
    bool checkpointAutoSave;
    uint64_t checkpointSaveInterval;
    
    // Performance and resource management
    bool enableSIMD;
    bool enableParallelProcessing;
    size_t parallelThreads;
    bool enableMemoryPools;
    bool enableEventQueueOptimization;
    
    // Output and analysis
    bool enableMetricsExport;
    std::string metricsOutputFile;
    std::string visualizationOutputFile;
    std::vector<std::string> enabledMetrics;
    std::set<std::string> disabledMetrics;
    
    // Testing and validation
    bool enableAblationTests;
    std::vector<std::string> ablatedSystems;
    bool enableStressTesting;
    float stressLoadMultiplier;
    bool enableRegressionTests;
    std::map<std::string, std::string> expectedBehavior;
    
    // Development and learning
    std::string developmentalStageProgression;
    std::map<std::string, float> developmentalModifiers;
    bool enableMetaLearning;
    float metaLearningRate;
    
    // External integration
    bool enableExternalInterface;
    std::string externalAPIEndpoint;
    bool enableWebInterface;
    size_t webInterfacePort;
    bool enableDataLogging;
    std::string dataLogFile;
    
    // Statistics
    std::chrono::high_resolution_clock::time_point simulationStartTime;
    std::chrono::high_resolution_clock::time_point simulationEndTime;
    uint64_t totalStepsExecuted;
    float cumulativeReward;
    size_t totalSpikesGenerated;
    size_t totalMemoriesEncoded;
    
    // Default constructor with Phase 6 defaults
    Phase6AdvancedConfig() 
        : maxSteps(10000), neuronCount(1000), regionCount(1), connectionProbability(0.1f),
          enableCheckpointing(true), enableReplay(true), enableDevelopment(true), 
          checkpointPath("./checkpoint_test.bin"),
          enableVerboseLogging(false), enablePerformanceMetrics(true), 
          enableDetailedProfiling(false), enableRandomSeedOverride(false), randomSeed(42),
          timestepDuration(0.001f), replayInterval(100), consolidationInterval(1000),
          synapticPruningInterval(1000), conceptUpdateInterval(100),
          workingMemoryCapacity(100), maxEpisodicEpisodes(1000),
          memoryConsolidationStrength(0.3f), memoryReplayStrength(0.5f),
          dopamineBaseline(0.1f), curiosityBaseline(0.1f), noveltyBaseline(0.1f),
          predictionErrorBaseline(0.0f), enableExternalNeuromodulation(false),
          stdpLTPWeight(0.01f), stdpLTDWeight(0.012f), stdpTimeConstant(20.0f),
          hebbianLearningRate(0.005f), structuralSynaptogenesisRate(0.0001f),
          structuralPruningRate(0.00001f), developmentalPlasticityModifier(1.0f),
          planningDepth(5), attentionInhibitionStrength(0.5f),
          attentionExcitationStrength(1.5f), conceptFormationThreshold(0.5f),
          conceptUpdateFrequency(10), worldWidth(16), worldHeight(16),
          sensoryNoiseLevel(0.0f), motorNoiseLevel(0.0f),
          checkpointDirectory("./checkpoints"), checkpointCompress(true),
          checkpointMaxVersions(10), checkpointAutoSave(true),
          checkpointSaveInterval(1000),
          enableSIMD(true), enableParallelProcessing(false), parallelThreads(4),
          enableMemoryPools(true), enableEventQueueOptimization(true),
          enableMetricsExport(false), enableAblationTests(false),
          enableStressTesting(false), stressLoadMultiplier(1.0f),
          enableRegressionTests(false), enableMetaLearning(false),
          metaLearningRate(0.001f), enableExternalInterface(false),
          enableWebInterface(false), webInterfacePort(8080),
          enableDataLogging(false), 
          totalStepsExecuted(0), cumulativeReward(0.0f),
          totalSpikesGenerated(0), totalMemoriesEncoded(0) {}
    
    // Constructor with basic Phase 6 settings
    Phase6AdvancedConfig(uint64_t steps, size_t neurons, size_t regions,
                        bool verbose = false, bool perfMetrics = true)
        : maxSteps(steps), neuronCount(neurons), regionCount(regions),
          connectionProbability(0.1f), enableCheckpointing(true),
          enableReplay(true), enableDevelopment(true), checkpointPath("./checkpoint.bin"),
          enableVerboseLogging(verbose), enablePerformanceMetrics(perfMetrics),
          enableDetailedProfiling(false), enableRandomSeedOverride(false),
          randomSeed(42), timestepDuration(0.001f), replayInterval(100),
          consolidationInterval(1000), synapticPruningInterval(1000),
          conceptUpdateInterval(100), workingMemoryCapacity(100),
          maxEpisodicEpisodes(1000), memoryConsolidationStrength(0.3f),
          memoryReplayStrength(0.5f), dopamineBaseline(0.1f),
          curiosityBaseline(0.1f), noveltyBaseline(0.1f),
          predictionErrorBaseline(0.0f), enableExternalNeuromodulation(false),
          stdpLTPWeight(0.01f), stdpLTDWeight(0.012f), stdpTimeConstant(20.0f),
          hebbianLearningRate(0.005f), structuralSynaptogenesisRate(0.0001f),
          structuralPruningRate(0.00001f), developmentalPlasticityModifier(1.0f),
          planningDepth(5), attentionInhibitionStrength(0.5f),
          attentionExcitationStrength(1.5f), conceptFormationThreshold(0.5f),
          conceptUpdateFrequency(10), worldWidth(16), worldHeight(16),
          sensoryNoiseLevel(0.0f), motorNoiseLevel(0.0f),
          checkpointDirectory("./checkpoints"), checkpointCompress(true),
          checkpointMaxVersions(10), checkpointAutoSave(true),
          checkpointSaveInterval(1000), enableSIMD(true),
          enableParallelProcessing(false), parallelThreads(4),
          enableMemoryPools(true), enableEventQueueOptimization(true),
          enableMetricsExport(false), enableAblationTests(false),
          enableStressTesting(false), stressLoadMultiplier(1.0f),
          enableRegressionTests(false), enableMetaLearning(false),
          metaLearningRate(0.001f), enableExternalInterface(false),
          enableWebInterface(false), webInterfacePort(8080),
          enableDataLogging(false),
          totalStepsExecuted(0), cumulativeReward(0.0f),
          totalSpikesGenerated(0), totalMemoriesEncoded(0) {}
    
    // Convert to Phase6Config (basic config)
    Phase6Config toPhase6Config() const {
        Phase6Config config;
        config.maxSteps = maxSteps;
        config.neuronCount = neuronCount;
        config.regionCount = regionCount;
        config.connectionProbability = connectionProbability;
        config.enableCheckpointing = enableCheckpointing;
        config.enableReplay = enableReplay;
        config.enableDevelopment = enableDevelopment;
        config.checkpointPath = checkpointPath;
        return config;
    }
    
    // Validate configuration
    bool validate() const {
        if (maxSteps == 0) {
            std::cerr << "Error: maxSteps must be > 0" << std::endl;
            return false;
        }
        if (neuronCount == 0) {
            std::cerr << "Error: neuronCount must be > 0" << std::endl;
            return false;
        }
        if (regionCount == 0) {
            std::cerr << "Error: regionCount must be > 0" << std::endl;
            return false;
        }
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            std::cerr << "Error: connectionProbability must be in [0.0, 1.0]" << std::endl;
            return false;
        }
        if (worldWidth == 0 || worldHeight == 0) {
            std::cerr << "Error: World dimensions must be > 0" << std::endl;
            return false;
        }
        return true;
    }
    
    // Get configuration summary
    std::string summary() const {
        std::string result = "Phase 6 Advanced Configuration Summary:\n";
        result += "===============================\n";
        result += "Experiment: " + std::to_string(maxSteps) + " steps, " + 
                  std::to_string(neuronCount) + " neurons, " + 
                  std::to_string(regionCount) + " regions\n";
        result += "Features: Checkpoint=" + std::string(enableCheckpointing ? "YES" : "NO") +
                  ", Replay=" + std::string(enableReplay ? "YES" : "NO") +
                  ", Development=" + std::string(enableDevelopment ? "YES" : "NO") + "\n";
        result += "Performance: SIMD=" + std::string(enableSIMD ? "YES" : "NO") +
                  ", Parallel=" + std::string(enableParallelProcessing ? "YES" : "NO") +
                  ", MemoryPools=" + std::string(enableMemoryPools ? "YES" : "NO") + "\n";
        result += "Metrics: " + std::to_string(enabledMetrics.size()) + "/" + 
                  std::to_string(disabledMetrics.size()) + " enabled\n";
        result += "=== End Summary ===\n";
        return result;
    }
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filename) {
        // Implementation for loading JSON config
        std::cout << "Loading configuration from " << filename << std::endl;
        // In a real implementation, would parse JSON
        return true;
    }
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filename) const {
        // Implementation for saving JSON config
        std::cout << "Saving configuration to " << filename << std::endl;
        // In a real implementation, would serialize to JSON
        return true;
    }
}; 

/**
 * Configuration presets for different testing scenarios
 */
namespace ConfigPresets {
    
    // Quick test preset (small simulation)
    inline Phase6AdvancedConfig QuickTest() {
        Phase6AdvancedConfig config(1000, 200, 2);
        config.enableVerboseLogging = true;
        config.enablePerformanceMetrics = true;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.maxSteps = 1000;
        return config;
    }
    
    // Production preset (full simulation)
    inline Phase6AdvancedConfig Production() {
        Phase6AdvancedConfig config(50000, 10000, 4);
        config.enableVerboseLogging = false;
        config.enablePerformanceMetrics = true;
        config.enableDetailedProfiling = true;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.maxSteps = 50000;
        config.checkpointMaxVersions = 20;
        config.enableSIMD = true;
        config.enableMemoryPools = true;
        config.enableEventQueueOptimization = true;
        return config;
    }
    
    // Research preset (exploratory simulation)
    inline Phase6AdvancedConfig Research() {
        Phase6AdvancedConfig config(20000, 5000, 3);
        config.enableVerboseLogging = true;
        config.enablePerformanceMetrics = true;
        config.enableDetailedProfiling = true;
        config.enableRandomSeedOverride = true;
        config.randomSeed = 12345;
        config.enableAblationTests = true;
        config.ablatedSystems = {"prediction", "planning"};
        config.maxSteps = 20000;
        config.enableWebInterface = true;
        config.webInterfacePort = 8080;
        return config;
    }
    
    // Stress test preset (high load)
    inline Phase6AdvancedConfig StressTest() {
        Phase6AdvancedConfig config(100000, 50000, 8);
        config.enableVerboseLogging = true;
        config.enablePerformanceMetrics = true;
        config.enableStressTesting = true;
        config.stressLoadMultiplier = 2.0f;
        config.enableSIMD = true;
        config.enableParallelProcessing = true;
        config.parallelThreads = 8;
        config.enableMemoryPools = true;
        config.maxSteps = 100000;
        config.checkpointSaveInterval = 5000;
        return config;
    }
    
    // Development preset (developmental stages)
    inline Phase6AdvancedConfig Development() {
        Phase6AdvancedConfig config(30000, 3000, 2);
        config.enableVerboseLogging = true;
        config.enablePerformanceMetrics = true;
        config.enableDevelopment = true;
        config.developmentalStageProgression = "initial->critical->maturation->adult";
        config.maxSteps = 30000;
        config.developmentalModifiers["plasticity"] = 1.0f;
        config.developmentalModifiers["synaptogenesis"] = 0.001f;
        config.developmentalModifiers["pruning"] = 0.00001f;
        return config;
    }
    
} // namespace ConfigPresets

} // namespace nlm
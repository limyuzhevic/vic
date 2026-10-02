#pragma once

#include "Brain.hpp"
#include "SensoryInput.hpp"
#include "Action.hpp"
#include "Config.hpp"
#include <string>
#include <functional>
#include <map>
#include <vector>

namespace nlm {

/**
 * High-level user API for NLM (Neural Learning Machine)
 * Provides simplified interface for common operations and advanced features
 */
class NLMUserAPI {
public:
    // ===== BASIC SETUP =====
    
    /**
     * Create and initialize a brain with default configuration
     */
    static std::unique_ptr<Brain> createBrain(const std::string& configFile = "config.json");
    
    /**
     * Create brain with custom configuration parameters
     */
    static std::unique_ptr<Brain> createBrain(const std::map<std::string, std::string>& params);
    
    // ===== SENSOR-MOTOR LOOP =====
    
    /**
     * Process sensory input and produce motor output
     */
    static std::unique_ptr<Action> process(const std::unique_ptr<Brain>& brain,
                                          const SensoryInput& sensoryInput);
    
    /**
     * Process sensory input with custom action selection strategy
     */
    static std::unique_ptr<Action> processWithStrategy(
        const std::unique_ptr<Brain>& brain,
        const SensoryInput& sensoryInput,
        const std::function<ActionType(const std::vector<float>&)>& strategy);
    
    // ===== CONFIGURATION =====
    
    /**
     * Load configuration from JSON file
     */
    static std::shared_ptr<Config> loadConfig(const std::string& filepath);
    
    /**
     * Save current brain state to file
     */
    static bool saveBrain(const std::unique_ptr<Brain>& brain, const std::string& filepath);
    
    /**
     * Load brain state from file
     */
    static bool loadBrain(const std::unique_ptr<Brain>& brain, const std::string& filepath);
    
    // ===== LEARNING AND TRAINING =====
    
    /**
     * Train brain with sequence of experiences
     */
    static void trainSequence(const std::unique_ptr<Brain>& brain,
                            const std::vector<SensoryInput>& experiences,
                            const std::vector<ActionType>& actions,
                            const std::vector<float>& rewards);
    
    /**
     * Learn from demonstration (expert demonstrations)
     */
    static void learnFromDemonstration(const std::unique_ptr<Brain>& brain,
                                      const std::vector<SensoryInput>& states,
                                      const std::vector<ActionType>& actions);
    
    /**
     * Optimize brain parameters for specific task
     */
    static void optimizeForTask(const std::unique_ptr<Brain>& brain,
                               const std::vector<SensoryInput>& trainingData,
                               const std::vector<ActionType>& optimalActions);
    
    // ===== MEMORY MANAGEMENT =====
    
    /**
     * Store important memories for recall
     */
    static void storeImportantMemory(const std::unique_ptr<Brain>& brain,
                                    const std::vector<float>& memoryPattern,
                                    float importance = 1.0f);
    
    /**
     * Recall similar memories
     */
    static std::vector<float> recallMemory(const std::unique_ptr<Brain>& brain,
                                           const std::vector<float>& queryPattern,
                                           float similarityThreshold = 0.7f);
    
    /**
     * Clear all memories
     */
    static void clearMemories(const std::unique_ptr<Brain>& brain);
    
    // ===== ADVANCED FEATURES =====
    
    /**
     * Enable reinforcement learning with reward signal
     */
    static void enableReinforcementLearning(const std::unique_ptr<Brain>& brain,
                                          std::function<float(const ActionType&)> rewardFunction);
    
    /**
     * Set curiosity-driven exploration
     */
    static void enableCuriosityDrivenExploration(const std::unique_ptr<Brain>& brain,
                                                 float noveltyThreshold = 0.5f);
    
    /**
     * Configure prediction-based learning
     */
    static void configurePredictionLearning(const std::unique_ptr<Brain>& brain,
                                           bool enableTemporalPredictions = true,
                                           bool enableActionPredictions = true);
    
    /**
     * Set developmental stages
     */
    static void setDevelopmentalStage(const std::unique_ptr<Brain>& brain,
                                     DevelopmentalStage stage);
    
    // ===== UTILITY FUNCTIONS =====
    
    /**
     * Create default configuration
     */
    static std::map<std::string, std::string> getDefaultConfig();
    
    /**
     * Validate configuration parameters
     */
    static bool validateConfig(const std::map<std::string, std::string>& params);
    
    /**
     * Get available configuration options
     */
    static std::vector<std::string> getAvailableOptions();
    
    /**
     * Print system status and statistics
     */
    static void printSystemStatus(const std::unique_ptr<Brain>& brain);
    
    /**
     * Create a simple experiment configuration
     */
    static std::map<std::string, std::string> createExperimentConfig(
        const std::string& experimentType,
        int neuronCount = 1000,
        int regionCount = 1,
        float simulationTimestep = 0.001);
    
    // ===== EXPERT MODE =====
    
    /**
     * Enable expert mode with advanced options
     */
    static void enableExpertMode(const std::unique_ptr<Brain>& brain,
                               const std::map<std::string, std::string>& expertOptions);
    
    /**
     * Get expert mode options
     */
    static std::map<std::string, std::string> getExpertModeOptions();
    
    /**
     * Batch process multiple sensory inputs (expert feature)
     */
    static std::vector<std::unique_ptr<Action>> batchProcess(
        const std::unique_ptr<Brain>& brain,
        const std::vector<SensoryInput>& inputs,
        bool useParallelProcessing = false);
    
    /**
     * Export brain state for external analysis
     */
    static bool exportBrainState(const std::unique_ptr<Brain>& brain,
                                const std::string& filepath,
                                const std::string& format = "json");
    
    /**
     * Import brain state from external source
     */
    static bool importBrainState(const std::unique_ptr<Brain>& brain,
                                const std::string& filepath,
                                const std::string& format = "json");
};

} // namespace nlm

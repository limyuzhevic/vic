// AgentBrain.hpp - Enhanced version with better encapsulation and organization

#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <optional>

namespace nlm {

// Forward declarations for motor and sensory groups
class MotorGroup;
class SensoryGroup;

// Motor command categories (better organized)
enum class MotorActionCategory {
    Movement,      // Movement-related actions
    Interaction,   // Interaction-related actions
    Information,   // Information gathering actions
    Exploration,   // Exploration behaviors
    Rest,          // Rest/standby actions
    Custom        // Custom actions
};

// Motor command with metadata
struct MotorCommandWithMetadata {
    MotorCommand command;
    MotorActionCategory category;
    float priority;           // Action priority (0-1)
    float energyCost;          // Energy required to execute
    float timeCost;            // Time required to execute
    std::string description;   // Human-readable description
    
    MotorCommandWithMetadata(MotorCommand cmd, MotorActionCategory cat = MotorActionCategory::Custom)
        : command(cmd), category(cat), priority(1.0f), energyCost(1.0f), timeCost(0.1f), description("") {}
};

// Sensory input with metadata
struct SensoryInputWithMetadata {
    SensoryInput input;
    float confidence;          // Confidence level (0-1)
    float importance;           // Importance weight (0-1)
    float freshness;            // How recent the input is (0-1)
    
    SensoryInputWithMetadata(const SensoryInput& inp, float conf = 1.0f, 
                             float importance = 1.0f, float freshness = 1.0f)
        : input(inp), confidence(conf), importance(importance), freshness(freshness) {}
};

// Agent configuration
struct AgentConfig {
    bool enableRewardModulation;
    bool enableStructuralPlasticity;
    bool enableDevelopment;
    bool enableCuriosity;
    bool enableAttention;
    bool enablePlanning;
    
    float rewardModulationStrength;
    float curiosityThreshold;
    float noveltyThreshold;
    float explorationRate;
    float learningRate;
    
    std::string name;
    
    AgentConfig()
        : enableRewardModulation(true),
          enableStructuralPlasticity(true),
          enableDevelopment(true),
          enableCuriosity(true),
          enableAttention(false),
          enablePlanning(false),
          rewardModulationStrength(1.0f),
          curiosityThreshold(0.3f),
          noveltyThreshold(0.5f),
          explorationRate(0.2f),
          learningRate(0.01f),
          name("Agent") {}
};

// Statistical tracking for agent performance
struct AgentStats {
    uint64_t totalSteps;
    uint64_t totalRewards;
    uint64_t successfulActions;
    uint64_t failedActions;
    float averageReward;
    float explorationRate;
    float learningProgress;
    float developmentalStage;
    
    AgentStats()
        : totalSteps(0), totalRewards(0), successfulActions(0), failedActions(0),
          averageReward(0.0f), explorationRate(0.0f), learningProgress(0.0f), 
          developmentalStage(0.0f) {}
    
    void incrementSteps() { totalSteps++; }
    void addReward(float reward) { totalRewards += static_cast<uint64_t>(reward); }
    void recordSuccess() { successfulActions++; }
    void recordFailure() { failedActions++; }
    
    float getSuccessRate() const {
        if (totalSteps == 0) return 0.0f;
        return static_cast<float>(successfulActions) / totalSteps;
    }
};

// AgentBrain: Connects NLM brain to the world with enhanced encapsulation
// Handles sensory transduction and motor decoding with better organization
class AgentBrain {
public:
    // Enhanced constructor with configuration
    explicit AgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config = AgentConfig());
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Initialize with world and configuration
    bool initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept with metadata
    void processSensoryInput(const SensoryInputWithMetadata& percept);
    void processSensoryInput(const SensoryInput& percept) {
        processSensoryInput(SensoryInputWithMetadata(pert));
    }
    
    // Enhanced motor command decoding
    MotorCommand decodeMotorCommand();
    MotorCommandWithMetadata decodeMotorCommandWithMetadata();
    
    // Enhanced neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Enhanced development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Enhanced neuromodulation level getters
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Get statistics
    const AgentStats& getStats() const { return stats_; }
    
    // Enhanced configuration management
    void configure(const AgentConfig& config);
    void enableFeature(std::string feature, bool enable);
    bool isFeatureEnabled(const std::string& feature) const;
    
    // Configuration flags (encapsulated)
    void enableRewardModulation(bool enable) { config_.enableRewardModulation = enable; }
    void enableStructuralPlasticity(bool enable) { config_.enableStructuralPlasticity = enable; }
    void enableDevelopment(bool enable) { config_.enableDevelopment = enable; }
    void enableCuriosity(bool enable) { config_.enableCuriosity = enable; }
    void enableAttention(bool enable) { config_.enableAttention = enable; }
    void enablePlanning(bool enable) { config_.enablePlanning = enable; }
    
    bool isRewardModulationEnabled() const { return config_.enableRewardModulation; }
    bool isStructuralPlasticityEnabled() const { return config_.enableStructuralPlasticity; }
    bool isDevelopmentEnabled() const { return config_.enableDevelopment; }
    bool isCuriosityEnabled() const { return config_.enableCuriosity; }
    bool isAttentionEnabled() const { return config_.enableAttention; }
    bool isPlanningEnabled() const { return config_.enablePlanning; }
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration accessors
    const AgentConfig& getConfig() const { return config_; }
    
    // Performance monitoring
    void incrementStats() { stats_.incrementSteps(); }
    void recordReward(float reward) { stats_.addReward(reward); }
    void recordSuccess() { stats_.recordSuccess(); }
    void recordFailure() { stats_.recordFailure(); }
    
    // Debug and logging
    void logStatus() const;
    std::string getPerformanceSummary() const;
    
    // Validation and error checking
    bool isValid() const;
    std::string getValidationErrors() const;
    
    // Utility functions
    static std::vector<std::string> getAvailableFeatures();
    static MotorCommandWithMetadata getMotorCommandInfo(MotorCommand cmd);
    static float calculateActionCost(const MotorCommandWithMetadata& cmd);
    static float calculateExpectedReward(const SensoryInput& percept, const MotorCommand& action);
    
private:
    // Private implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core components
    std::shared_ptr<Brain> brain_;
    AgentConfig config_;
    AgentStats stats_;
    
    // State management
    std::optional<SensoryInputWithMetadata> lastProcessedInput_;
    std::vector<MotorCommandWithMetadata> actionHistory_;
    std::chrono::system_clock::time_point lastActionTime_;
    
    // Enhanced motor and sensory groups (encapsulated in MotorGroup/SensoryGroup)
    std::vector<MotorGroup*> motorGroups_;
    std::vector<SensoryGroup*> sensoryGroups_;
    
    // Internal helper methods
    MotorCommandWithMetadata selectBestAction(const std::vector<MotorCommandWithMetadata>& candidates) const;
    float calculateCuriosityFactor() const;
    float calculateNoveltyFactor() const;
    void updateExplorationRate(float newRate);
    void applyNeuromodulationToBrain(float reward, float predictionError);
    
    // Validation helpers
    bool validateBrainConnection() const;
    bool validateConfiguration() const;
    bool validateMotorNeurons() const;
    
    // Debug helpers
    void debugMotorDecoding() const;
    void debugSensoryProcessing() const;
    void debugNeuromodulation() const;
};

} // namespace nlm

// Convenience functions for common use cases
namespace nlm {
    
    // Create an agent brain with default configuration
    std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain);
    
    // Create an agent brain with custom configuration
    std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config);
    
    // Create configuration for specific task
    AgentConfig createExplorationConfig();
    AgentConfig createExploitationConfig();
    AgentConfig createBalancedConfig();
    
    // Create configuration for specific learning scenario
    AgentConfig createReinforcementLearningConfig();
    AgentConfig createCuriosityDrivenConfig();
    AgentConfig createDevelopmentalConfig();
    
    // Utility functions
    float calculateNovelty(const std::vector<float>& currentState, 
                          const std::vector<std::vector<float>>& experienceStore);
    float calculateCuriosity(const std::vector<float>& predictionError, 
                           const std::vector<float>& expectedReward);
    MotorCommandWithMetadata suggestExplorationAction(const AgentBrain& agent);
    
    // Performance utilities
    float calculateSuccessRate(const AgentStats& stats);
    float calculateLearningProgress(const AgentStats& current, const AgentStats& previous);
    float calculateEfficiency(const AgentStats& stats, float totalTime);
    
} // namespace nlm
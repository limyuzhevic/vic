#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain configuration constants
namespace AgentBrainConstants {
    // Motor configuration
    const size_t MOTOR_GROUP_COUNT = 16;           // All advanced motor commands including LookLeft/LookRight
    const float MOTOR_THRESHOLD = 0.5f;          // Minimum activity to trigger action
    const float MOTOR_NOISE_LEVEL = 0.1f;        // Randomization in motor selection
    
    // Sensory configuration
    const size_t VISION_WIDTH = 16;              // Vision grid size
    const size_t VISION_HEIGHT = 16;             // Vision grid size
    const size_t VISION_NEUTRAL_SIZE = VISION_WIDTH * VISION_HEIGHT;  // 256
    
    const size_t TOUCH_INPUT_COUNT = 8;          // Touch sensor count
    const size_t INTERNAL_INPUT_COUNT = 4;       // Internal state signals
    const size_t PROPRIOCEPTION_INPUT_COUNT = 6; // Body position/velocity signals
    
    const float NOVELTY_DECAY_RATE = 0.99f;      // Novelty decay per timestep
    const float NEUROMODULATION_CLAMP_MIN = -1.0f;
    const float NEUROMODULATION_CLAMP_MAX = 1.0f;
    const float PREDICTION_ERROR_THRESHOLD = 0.001f;
    
    // Advanced neuromodulation constants
    const float DOPAMINE_PREDICTION_ERROR_WEIGHT = 1.0f;      // Weight of prediction error on dopamine
    const float NOVELTY_SENSITIVITY = 2.0f;                   // Novelty sensitivity for curiosity
    const float PREDICTION_ERROR_SENSITIVITY = 0.5f;          // Prediction error sensitivity
    const float CONSERVATIVE_DOPSINE_CLAMP = 0.8f;           // Conservative dopamine clamp
    
    // Development stage timing constants (in simulation steps)
    const double DEVELOPMENT_STAGE_INITIAL_STEPS = 60.0;     // ~1 minute
    const double DEVELOPMENT_STAGE_CRITICAL_STEPS = 300.0;   // ~5 minutes
    const double DEVELOPMENT_STAGE_MATURE_STEPS = 900.0;     // ~15 minutes
    const double DEVELOPMENT_STAGE_ADULT_STEPS = 1800.0;     // ~30 minutes
    
    // Developmental plasticity dynamics
    const float PLASTICITY_DECAY_RATE = 0.95f;               // Plasticity decay during development
    const float PLASTICITY_MATURATION_RATE = 0.8f;            // Rate of plasticity maturation
    const float PLASTICITY_STABILIZATION = 0.3f;              // Plasticity stabilization factor
    
    // Structural plasticity dynamics
    const float SYNAPTOGENESIS_RATE_INITIAL = 0.0002f;       // Initial synaptogenesis rate
    const float SYNAPTOGENESIS_RATE_FINAL = 0.00001f;         // Final synaptogenesis rate
    const float PRUNING_RATE_INITIAL = 0.000001f;            // Initial pruning rate
    const float PRUNING_RATE_FINAL = 0.00005f;               // Final pruning rate
    
    // Advanced neuromodulation constants
    const float NOVELTY_MEMORY_DECAY = 0.995f;               // Memory decay for novelty
    const float CURIOSITY_CAPACITY = 1.0f;                   // Maximum curiosity level
    const float PREDICTION_ERROR_DECAY = 0.9f;               // Prediction error decay
    const float DOPSINE_FACTOR_BASE = 0.5f;                  // Base dopamine factor
}

// AgentBrain configuration options
struct AgentBrainConfig {
    bool rewardModulationEnabled = true;
    bool structuralPlasticityEnabled = true;
    bool developmentEnabled = true;
    bool curiosityEnabled = true;
    float sensoryNoveltyDecay = AgentBrainConstants::NOVELTY_DECAY_RATE;
    float motorThreshold = AgentBrainConstants::MOTOR_THRESHOLD;
    float explorationChance = AgentBrainConstants::CURIOSITY_EXPLORATION_CHANCE_MAX;
    
    // Advanced configuration
    bool enableAdvancedMotorCommands = false;     // Enable LookLeft/LookRight commands
    bool enableAudioSensing = false;              // Enable audio input processing
    float curiositySensitivity = 1.0f;           // Modify curiosity computation
    float neuromodulationGain = 1.0f;            // Scale neuromodulation effects
};

// AgentBrain forward declarations for better modularity
class SensoryProcessor;
class MotorDecoder;
class NeuromodulationSystem;
class DevelopmentSystem;
class CuriositySystem;

// AgentBrain: Main orchestrator for agent capabilities
// Connects NLM brain to the world
// Handles sensory transduction, motor decoding, and neuromodulation
class AgentBrain {
public:
    // Configuration and initialization
    explicit AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig{});
    ~AgentBrain();
    
    // Core functionality
    void initialize(const SimpleWorld& world);
    void reset();
    
    // Processing methods
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    
    // System access and status
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration management
    void updateConfig(const AgentBrainConfig& newConfig);
    const AgentBrainConfig& getConfig() const { return config_; }
    
    // System information
    size_t getSensoryInputSize() const;
    size_t getMotorOutputSize() const;
    
    // Status queries
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Advanced features
    void enableAdvancedFeatures(bool enable) { advancedFeaturesEnabled_ = enable; }
    bool areAdvancedFeaturesEnabled() const { return advancedFeaturesEnabled_; }
    
    // Performance and diagnostics
    double getProcessingTime() const { return processingTime_; }
    size_t getTotalNeuronCount() const;
    size_t getActiveMotorGroupCount() const;
    
    // Serialization and checkpointing
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
private:
    // Sub-system composition for better modularity
    std::unique_ptr<SensoryProcessor> sensoryProcessor_;
    std::unique_ptr<MotorDecoder> motorDecoder_;
    std::unique_ptr<NeuromodulationSystem> neuromodulationSystem_;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    std::unique_ptr<CuriositySystem> curiositySystem_;
    
    // Core components
    std::shared_ptr<Brain> brain_;
    AgentBrainConfig config_;
    
    // Performance tracking
    double processingTime_;
    mutable size_t lastNeuronCount_;
    mutable size_t lastActiveMotorGroups_;
    
    // Advanced features
    bool advancedFeaturesEnabled_;
    
    // Helper methods
    void initializeSubSystems();
    void updateSubSystems(double timestep);
    void synchronizeState();
    void validateConfiguration() const;
};

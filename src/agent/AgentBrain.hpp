#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace nlm {

// Distribution strategies for neuron allocation
enum class DistributionStrategy {
    Uniform,           // Equal distribution using modulo
    Weighted,          // Weighted distribution based on priorities
    Random,            // Random distribution
    Custom            // User-provided distribution function
};

// Agent brain configuration structure
struct AgentBrainConfig {
    // Neuron distribution configuration
    DistributionStrategy distributionStrategy_;
    std::vector<float> motorGroupWeights_;  // Weights for motor groups: [forward, backward, left, right, interact, wait]
    std::vector<float> sensoryGroupWeights_; // Weights for sensory groups: [vision, touch, internal, proprioception]
    std::function<std::vector<size_t>(size_t, const std::vector<Neuron*>&)> customDistributionFunc_;
    
    // Sensory scaling factors
    float visionScaleFactor_;          // Default: 5.0f (vision[i] * visionScaleFactor_)
    float touchScaleFactor_;           // Default: 8.0f (touch[i] * touchScaleFactor_)
    float internalScaleFactor_;        // Default: 5.0f ((internal[i] * 2.0f - 1.0f) * internalScaleFactor_)
    float proprioceptionScaleFactor_;  // Default: 3.0f ((proprio[i] * 2.0f - 1.0f) * proprioceptionScaleFactor_)
    
    // Curiosity and novelty configuration
    float curiosityThreshold_;         // Default: 0.3f
    float explorationThreshold_;       // Default: 0.5f
    float noveltyDecayRate_;            // Default: 0.99f
    float curiosityGain_;              // Default: 2.0f
    float predictionErrorWeight_;      // Default: 0.5f
    
    // Motor command configuration
    std::vector<float> motorCommandWeights_;  // Weights for each motor command
    float motorActivationThreshold_;          // Default: 0.5f
    bool useCuriosityExploration_;            // Default: true
    
    // Development configuration
    double developmentalStageStart_;   // Default: 60.0
    double criticalPeriodEnd_;         // Default: 300.0
    double maturationEnd_;             // Default: 900.0
    
    // Error handling configuration
    bool strictValidation_;            // Default: true
    bool logWarnings_;                 // Default: true
    
    // Default configuration values
    static constexpr float DEFAULT_VISION_SCALE = 5.0f;
    static constexpr float DEFAULT_TOUCH_SCALE = 8.0f;
    static constexpr float DEFAULT_INTERNAL_SCALE = 5.0f;
    static constexpr float DEFAULT_PROPRIOCEPTION_SCALE = 3.0f;
    static constexpr float DEFAULT_CURIOUSITY_THRESHOLD = 0.3f;
    static constexpr float DEFAULT_EXPLORATION_THRESHOLD = 0.5f;
    static constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    static constexpr float DEFAULT_CURIOUSITY_GAIN = 2.0f;
    static constexpr float DEFAULT_PREDICTION_ERROR_WEIGHT = 0.5f;
    static constexpr float DEFAULT_MOTOR_ACTIVATION_THRESHOLD = 0.5f;
    
    AgentBrainConfig();
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Get current configuration
    const AgentBrainConfig& getConfig() const { return config_; }
    
    // Set configuration
    void setConfig(const AgentBrainConfig& config);
    
    // Configuration getters/setters for backward compatibility
    void setVisionScaleFactor(float factor) { config_.visionScaleFactor_ = factor; }
    float getVisionScaleFactor() const { return config_.visionScaleFactor_; }
    
    void setTouchScaleFactor(float factor) { config_.touchScaleFactor_ = factor; }
    float getTouchScaleFactor() const { return config_.touchScaleFactor_; }
    
    void setInternalScaleFactor(float factor) { config_.internalScaleFactor_ = factor; }
    float getInternalScaleFactor() const { return config_.internalScaleFactor_; }
    
    void setProprioceptionScaleFactor(float factor) { config_.proprioceptionScaleFactor_ = factor; }
    float getProprioceptionScaleFactor() const { return config_.proprioceptionScaleFactor_; }
    
    void setCuriosityThreshold(float threshold) { config_.curiosityThreshold_ = threshold; }
    float getCuriosityThreshold() const { return config_.curiosityThreshold_; }
    
    void setExplorationThreshold(float threshold) { config_.explorationThreshold_ = threshold; }
    float getExplorationThreshold() const { return config_.explorationThreshold_; }
    
    void setNoveltyDecayRate(float rate) { config_.noveltyDecayRate_ = rate; }
    float getNoveltyDecayRate() const { return config_.noveltyDecayRate_; }
    
    void setMotorActivationThreshold(float threshold) { config_.motorActivationThreshold_ = threshold; }
    float getMotorActivationThreshold() const { return config_.motorActivationThreshold_; }
    
    // Distribution configuration
    void setDistributionStrategy(DistributionStrategy strategy) { config_.distributionStrategy_ = strategy; }
    DistributionStrategy getDistributionStrategy() const { return config_.distributionStrategy_; }
    
    void setMotorGroupWeights(const std::vector<float>& weights) { config_.motorGroupWeights_ = weights; }
    const std::vector<float>& getMotorGroupWeights() const { return config_.motorGroupWeights_; }
    
    void setSensoryGroupWeights(const std::vector<float>& weights) { config_.sensoryGroupWeights_ = weights; }
    const std::vector<float>& getSensoryGroupWeights() const { return config_.sensoryGroupWeights_; }
    
    // Dynamic neuron redistribution
    void redistributeNeurons();
    
    // Get current neuron group sizes
    size_t getMotorForwardCount() const { return motorForward_.size(); }
    size_t getMotorBackwardCount() const { return motorBackward_.size(); }
    size_t getMotorTurnLeftCount() const { return motorTurnLeft_.size(); }
    size_t getMotorTurnRightCount() const { return motorTurnRight_.size(); }
    size_t getMotorInteractCount() const { return motorInteract_.size(); }
    size_t getMotorWaitCount() const { return motorWait_.size(); }
    
    size_t getSensoryVisionCount() const { return sensoryVision_.size(); }
    size_t getSensoryTouchCount() const { return sensoryTouch_.size(); }
    size_t getSensoryInternalCount() const { return sensoryInternal_.size(); }
    size_t getSensoryProprioceptionCount() const { return sensoryProprioception_.size(); }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper functions for configuration
    void validateConfig() const;
    void normalizeWeights(std::vector<float>& weights) const;
    size_t calculateWeightedIndex(const std::vector<float>& weights, size_t totalNeurons) const;
    
    // Helper functions for calculations
    float calculateSensoryCurrent(float input, float scale, float offset = 0.0f, float center = 1.0f) const;
    float calculateMotorActivity(const std::vector<Neuron*>& neurons) const;
    float calculateNovelty(const std::vector<float>& current, const std::vector<float>& previous) const;
    
    // Configuration
    AgentBrainConfig config_;
    
    // Neuron distribution
    enum class MotorGroup { Forward, Backward, Left, Right, Interact, Wait };
    enum class SensoryGroup { Vision, Touch, Internal, Proprioception };
    
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Error handling
    void validateSensoryInput(const SensoryPercept& percept) const;
    void validateMotorCommand(MotorCommand& command) const;
    
    // Logging helper
    void logWarning(const std::string& message) const;
};

} // namespace nlm

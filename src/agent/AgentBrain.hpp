#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Configuration constants
    static constexpr size_t DEFAULT_MOTOR_GROUP_COUNT = 6;
    static constexpr size_t DEFAULT_VISION_NEURONS = 256;
    static constexpr size_t DEFAULT_TOUCH_NEURONS = 8;
    static constexpr size_t DEFAULT_INTERNAL_NEURONS = 4;
    static constexpr size_t DEFAULT_PROPRIOCEPTION_NEURONS = 6;
    static constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    static constexpr float DEFAULT_CURIOUSITY_THRESHOLD = 0.3f;
    static constexpr float DEFAULT_EXPLORATION_THRESHOLD = 0.5f;
    static constexpr float DEFAULT_ACTIVITY_THRESHOLD = 0.5f;
    static constexpr float DEFAULT_MAX_EXPLORATION_CHANCE = 0.3f;
    static constexpr float DEFAULT_DOPAMINE_CLAMP_MIN = -1.0f;
    static constexpr float DEFAULT_DOPAMINE_CLAMP_MAX = 1.0f;
    static constexpr float DEFAULT_PLASTICITY_FACTOR_BASE = 0.5f;
    static constexpr float DEFAULT_PLASTICITY_CLAMP_MIN = 0.1f;
    static constexpr float DEFAULT_PLASTICITY_CLAMP_MAX = 2.0f;
    static constexpr double INITIAL_DEVELOPMENT_AGE = 60.0; // ~1 minute
    static constexpr double CRITICAL_PERIOD_AGE = 300.0;   // ~5 minutes
    static constexpr double MATURATION_AGE = 900.0;       // ~15 minutes

    AgentBrain(std::shared_ptr<Brain> brain);
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
    
    // Enhanced configuration methods for advanced users
    void setSensoryNoveltyDecay(float decay) { sensoryNoveltyDecay_ = decay; }
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setExplorationThreshold(float threshold) { explorationThreshold_ = threshold; }
    void setActivityThreshold(float threshold) { activityThreshold_ = threshold; }
    void setMaxExplorationChance(float chance) { maxExplorationChance_ = chance; }
    
    float getSensoryNoveltyDecay() const { return sensoryNoveltyDecay_; }
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getExplorationThreshold() const { return explorationThreshold_; }
    float getActivityThreshold() const { return activityThreshold_; }
    float getMaxExplorationChance() const { return maxExplorationChance_; }

private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
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
    
    // Advanced configuration parameters
    float curiosityThreshold_;
    float explorationThreshold_;
    float activityThreshold_;
    float maxExplorationChance_;
};

} // namespace nlm

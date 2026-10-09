#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Configuration constants for easier tuning
    struct Config {
        // Motor neuron distribution
        static constexpr size_t MAX_MOTOR_GROUPS = 6;
        static constexpr float DEFAULT_MOTOR_SCALE = 5.0f;
        
        // Sensory neuron distribution  
        static constexpr size_t MAX_SENSORY_GROUPS = 4;
        static constexpr float VISION_SCALE_FACTOR = 5.0f;
        static constexpr float TOUCH_SCALE_FACTOR = 8.0f;
        static constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
        static constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
        
        // Activity thresholds and exploration parameters
        static constexpr float ACTIVITY_THRESHOLD = 0.5f;
        static constexpr float MIN_ACTIVITY_FOR_ACTION = 0.5f;
        static constexpr float EXPLORATION_CHANCE_PER_CURIOSITY_UNIT = 0.3f;
        
        // Novelty detection
        static constexpr float NOVELTY_DECAY_FACTOR = 0.99f;
        
        // Development stages (in simulation time units)
        static constexpr double CRITICAL_PERIOD_START = 60.0;
        static constexpr double MATURATION_START = 300.0;
        static constexpr double ADULT_START = 900.0;
        
        // Neuromodulation
        static constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
        static constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
        static constexpr float PLASTICITY_FACTOR_MIN = 0.1f;
        static constexpr float PLASTICITY_FACTOR_MAX = 2.0f;
    };

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
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced configuration methods
    void setActivityThreshold(float threshold) { activityThreshold_ = threshold; }
    void setExplorationWeight(float weight) { explorationWeight_ = weight; }
    void setNoveltyDecayFactor(float decay) { noveltyDecayFactor_ = decay; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper functions
    void distributeNeurons();
    void precomputeActivityThresholds();
    float calculateSensoryActivity(const std::vector<float>& sensoryData, 
                                  const std::vector<Neuron*>& neurons,
                                  float scaleFactor) const;
    void computeNovelty(const std::vector<float>& vision);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups - using weak_ptr to avoid ownership issues
    std::vector<std::weak_ptr<Neuron>> motorForward_;
    std::vector<std::weak_ptr<Neuron>> motorBackward_;
    std::vector<std::weak_ptr<Neuron>> motorTurnLeft_;
    std::vector<std::weak_ptr<Neuron>> motorTurnRight_;
    std::vector<std::weak_ptr<Neuron>> motorInteract_;
    std::vector<std::weak_ptr<Neuron>> motorWait_;
    
    // Sensory neuron groups - using weak_ptr to avoid ownership issues
    std::vector<std::weak_ptr<Neuron>> sensoryVision_;
    std::vector<std::weak_ptr<Neuron>> sensoryTouch_;
    std::vector<std::weak_ptr<Neuron>> sensoryInternal_;
    std::vector<std::weak_ptr<Neuron>> sensoryProprioception_;
    
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
    
    // Performance optimization: cached sizes
    size_t motorGroupCount_;
    size_t sensoryGroupCount_;
    
    // Advanced configuration parameters
    float activityThreshold_;
    float explorationWeight_;
    float noveltyDecayFactor_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Cached thresholds for optimization
    struct ActivityThresholds {
        float minActivityForAction;
        float explorationChancePerUnit;
    } thresholds_;
    
    // Helper: safely get shared_ptr from weak_ptr
    std::shared_ptr<Neuron> safeLock(const std::weak_ptr<Neuron>& weakPtr) const {
        auto sharedPtr = weakPtr.lock();
        return sharedPtr; // Will be null if expired
    }
};

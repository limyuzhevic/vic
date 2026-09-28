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
    
private:
    // Neural constant thresholds and parameters
    struct Constants {
        constexpr static float CURIOUSITY_THRESHOLD = 0.5f;                    // Threshold for curiosity-based exploration
        constexpr static float EXPLORATION_CHANCE_MULTIPLIER = 0.3f;            // Base multiplier for exploration chance
        constexpr static float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;            // Minimum eligibility trace magnitude to process
        constexpr static float ELIGIBILITY_TRACE_DECAY = 0.1f;                  // Decay rate for eligibility traces per timestep
        constexpr static float EXPECTED_REWARD_SMOOTHING = 0.95f;               // Smoothing factor for expected reward updates
        constexpr static float DOPAMINE_CLAMP_MIN = -1.0f;                      // Minimum allowable dopamine level
        constexpr static float DOPAMINE_CLAMP_MAX = 1.0f;                       // Maximum allowable dopamine level
        constexpr static float PLASTICITY_FACTOR_MIN = 0.1f;                    // Minimum plasticity factor
        constexpr static float PLASTICITY_FACTOR_MAX = 2.0f;                    // Maximum plasticity factor
        constexpr static float BASIC_ACTIVITY_THRESHOLD = 0.5f;                 // Minimum neural activity required for action
        constexpr static float DEFAULT_EPS = 0.001f;                           // Default tolerance for numerical comparisons
        
        // STDP (Spike-Timing-Dependent Plasticity) weights
        constexpr static float STDP_LTP_WEIGHT = 0.01f;                         // Long-Term Potentiation weight
        constexpr static float STDP_LTD_WEIGHT = 0.012f;                        // Long-Term Depression weight
        
        // Developmental system parameters
        constexpr static float SYNAPTOGENESIS_RATE_BASE = 0.0001f;              // Base synaptogenesis rate
        constexpr static float PRUNING_RATE_BASE = 0.00001f;                   // Base pruning rate
    };
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper methods for neuron distribution
    void distributeMotorNeurons(const std::vector<Neuron*>& neurons);
    void distributeSensoryNeurons(const std::vector<Neuron*>& neurons);
    
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
};

} // namespace nlm

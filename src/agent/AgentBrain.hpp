#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../memory/SleepConsolidationSystem.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations for sleep system
enum class SleepState : uint8_t;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding, plus sleep/rest cycle management
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
    
    // Sleep/rest state management
    SleepState::State getSleepState() const { return sleepState_; }
    float getSleepPressure() const { return sleepPressure_; }
    float getHomeostaticBalance() const { return homeostaticBalance_; }
    bool isSpontaneousReplayEnabled() const { return spontaneousReplayEnabled_; }
    bool isSleepConsolidationActive() const { return sleepConsolidationActive_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Update sleep state and pressure
    void updateSleepState();
    
    // Update attentional focus based on sensory salience
    void updateAttentionalFocus(const std::vector<float>& vision,
                               const std::vector<float>& touch,
                               const std::vector<float>& internal,
                               const std::vector<float>& proprioception);
    
    // Feed attentional focus back to memory systems
    void feedAttentionToMemorySystems();
    
    // Provide attentional feedback to concept formation
    void provideAttentionToConceptFormation();
    
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
    
    // Additional neuromodulators for sleep-wake control
    float acetylcholineLevel_;
    float norepinephrineLevel_;
    float serotoninLevel_;
    
    // Attentional state
    float attentionStrength_;
    float attentionFocus_;
    float memoryAttentionalBias_;
    
    // Sleep/rest state
    SleepState::State sleepState_;
    float sleepPressure_;
    float lastSleepTime_;
    bool spontaneousReplayEnabled_;
    bool sleepConsolidationActive_;
    float homeostaticBalance_;
    
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

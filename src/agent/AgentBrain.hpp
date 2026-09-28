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
    
    // Decode brain motor activity into motor command (basic version)
    MotorCommand decodeMotorCommand();
    
    // Enhanced versions of key methods
    MotorCommand decodeMotorCommandAdvanced();
    void processSensoryInputAdvanced(const SensoryPercept& percept);
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Enhanced reward modulation
    void applyAdvancedRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Enhanced development with more biological realism
    void updateDevelopmentAdvanced(double timestep);
    
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
    
    // Enhanced reset
    void resetAdvanced();
    
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
    
    // Enhanced getters for debugging and monitoring
    float getEnergyLevel() const;
    bool shouldRest() const;
    void logAdvancedStatus() const;
    
    // State management for checkpointing
    AgentState getState() const;
    void restoreState(const AgentState& state);
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Enhanced motor decoding methods
    MotorCommand decodeMotorCommandWithPreferences();
    void applyMotorPreferences(MotorCommand& command);
    MotorCommand sampleFromLearnedPolicy();
    MotorCommand selectAlternativeCommand(MotorCommand original);
    
    // Advanced processing methods
    void updatePlasticityBasedOnNeuromodulation();
    void updateCuriosityBasedOnPredictionError();
    void applyWeightedRewardModulation(float reward, float predictedReward);
    void generatePredictiveCoding(const std::vector<float>& input, 
                                 std::vector<float>& prediction);
    void processInternalSignalsAdvanced(const SensoryPercept& percept);
    void processProprioceptionAdvanced(const SensoryPercept& percept);
    
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
    
    // Advanced state for checkpointing
    struct AgentState {
        float dopamineLevel;
        float noveltyLevel;
        float curiosityLevel;
        float predictionError;
        float expectedReward;
        double developmentalAge;
        float plasticityModifier;
        size_t motorGroupSizes[6]; // Forward, Backward, TurnLeft, TurnRight, Interact, Wait
        std::vector<float> previousVision;
    };
};

} // namespace nlm

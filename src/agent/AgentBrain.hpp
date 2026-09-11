#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with integrated cognition
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
    // Integrates with planning, attention, and concept formation
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
    
    // Get planner action based on current state and context
    // Integrates with attention and working memory
    MotorCommand getPlannerAction();
    
    // Get concept-based action suggestion
    // Processes current percept against stored concepts
    MotorCommand getConceptBasedAction(const SensoryPercept& percept);
    
    // Update attention based on current state
    void updateAttention(const SensoryPercept& percept);
    
    // Update prediction system
    void updatePrediction(const SensoryPercept& percept);
    
    // Update working memory with new experience
    void updateWorkingMemory(const SensoryPercept& percept);
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void enablePlanning(bool enable) { planningEnabled_ = enable; }
    void enableConceptFormation(bool enable) { conceptFormationEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isPlanningEnabled() const { return planningEnabled_; }
    bool isConceptFormationEnabled() const { return conceptFormationEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command with cognition
    MotorCommand decodeMotorCommand();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Apply planner action to motor command
    MotorCommand applyPlannerAction(MotorCommand baseCmd);
    
    // Apply concept-based modulation to motor commands
    MotorCommand applyConceptModulation(MotorCommand baseCmd, const SensoryPercept& percept);
    
    // Get attention priorities for motor neuron groups
    std::vector<float> getAttentionWeights() const;
    
    // Update prediction error from reward
    void updatePredictionErrorFromReward(float reward);
    
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
    bool planningEnabled_;
    bool conceptFormationEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Concept storage for current episode
    std::vector<std::vector<float>> storedConcepts_;
    
    // Attention weights for motor groups
    std::vector<float> motorAttentionWeights_;
    
    // Current percept for cognition
    SensoryPercept currentPercept_;
    
    // Planning cache
    std::vector<MotorCommand> plannedActions_;
    
    // Concept associations
    std::unordered_map<int, std::vector<float>> conceptAssociations_;
};

} // namespace nlm

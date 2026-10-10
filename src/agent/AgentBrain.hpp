#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include <memory>
#include <vector>

namespace nlm
{

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
    
    // NEW: Concept formation integration
    // Process concepts from sensory input and working memory
    void processConcepts(const std::vector<float>& sensoryInput, float reward);
    // Get current concepts for planning and decision making
    const std::vector<nlm::DiscoveredConcept>& getConcepts() const;
    // Get best concept matching pattern
    size_t getBestConcept(const std::vector<float>& pattern) const;
    // Clear concepts
    void clearConcepts();
    
    // NEW: Neural planner integration
    // Plan action based on current state and goals
    ActionType planAction(const std::vector<float>& currentState, float targetReward = 0.5f);
    // Get current goal for planning
    std::vector<float> getCurrentGoal() const;
    // Set current goal for planning
    void setCurrentGoal(const std::vector<float>& goal);
    // Get planning confidence
    float getPlanningConfidence() const;
    // Clear planning cache
    void clearPlanningCache();
    
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
    
    // Episode and experience management
    // NEW for Phase 6: Episodic memory integration
    NeuralEpisodicMemory* getEpisodicMemory() { return brain_ ? brain_->getEpisodicMemory() : nullptr; }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void enableConceptFormation(bool enable) { conceptFormationEnabled_ = enable; }
    void enableNeuralPlanner(bool enable) { neuralPlannerEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isConceptFormationEnabled() const { return conceptFormationEnabled_; }
    bool isNeuralPlannerEnabled() const { return neuralPlannerEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Concept formation processing
    void processConceptsInternal(const std::vector<float>& sensoryInput, float reward);
    
    std::shared_ptr<Brain> brain_;
    
    // Concept formation system
    nlm::ConceptFormation conceptFormation_;
    bool conceptFormationEnabled_;
    
    // Neural planner system
    nlm::NeuralPlanner neuralPlanner_;
    bool neuralPlannerEnabled_;
    
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

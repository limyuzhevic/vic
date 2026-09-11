#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// Integrates NeuralPlanner for cognitive action selection
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
    
    // Neural planning for cognitive action selection
    void useNeuralPlanner(float curiosityLevel, const std::vector<float>& sensoryInput,
                         const std::vector<ActionType>& recentActions, float reward);
    
    // Get planned action sequences
    std::vector<std::vector<ActionType>> getPlannedActionSequences(size_t depth = 3) const;
    
    // Get plan quality and confidence
    float getPlanQuality() const { return planningQuality_; }
    float getPlanningConfidence() const { return planningConfidence_; }
    
    // Update NeuralPlanner with experience
    void recordActionOutcome(const std::vector<ActionType>& plannedActions,
                            const std::vector<ActionType>& actualActions,
                            float reward);
    
    // Set planning depth
    void setPlanningDepth(size_t depth) { if (neuralPlanner_) { neuralPlanner_->setPlanningDepth(depth); } }
    
    // Set action quality function
    void setActionQuality(ActionType action, float quality) {
        if (neuralPlanner_) neuralPlanner_->setActionQuality(action, quality);
    }
    
    // Set current goal for planning
    void setPlanningGoal(const std::vector<float>& goal) {
        if (neuralPlanner_) neuralPlanner_->setCurrentGoal(goal);
    }
    
    // Get concept for current state
    size_t getCurrentConcept() const { return currentConceptId_; }
    
    // Get concept stability for current concept
    float getCurrentConceptStability() const { return currentConceptStability_; }

private:
    // Neural planning system for cognitive action selection
    std::unique_ptr<NeuralPlanner> neuralPlanner_;
    
    // Concept formation for pattern discovery
    std::unique_ptr<ConceptFormation> conceptFormation_;
    
    // Planning state
    float planningQuality_;
    float planningConfidence_;
    std::vector<ActionType> lastPlannedAction_;
    
    // Current concept state
    size_t currentConceptId_;
    float currentConceptStability_;
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Process planned action sequence
    std::vector<ActionType> processPlannedAction(const std::vector<ActionType>& plannedSequence);
    
    // Evaluate action sequence quality
    float evaluateActionSequence(const std::vector<ActionType>& sequence,
                                const std::vector<float>& state) const;
    
    // Get sensory state vector for planning
    std::vector<float> getSensoryStateVector() const;
    
    // Extract pattern from sensory percept
    std::vector<float> extractPatternFromPercept(const SensoryPercept& percept) const;
    
    // Extract features from state and action
    std::vector<float> extractFeatures(const std::vector<float>& state,
                                       const ActionType& action) const;

} // namespace nlm

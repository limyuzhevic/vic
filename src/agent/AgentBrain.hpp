#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../cognition/ConceptFormation.hpp"
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
    
    // Get neural planner pointer
    NeuralPlanner* getPlanner() { return planner_.get(); }
    
    // Get neural prediction system pointer
    NeuralPrediction* getPredictionSystem() { return predictionSystem_.get(); }
    
    // Get concept formation pointer
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    
    const ConceptFormation* getConceptFormation() const { return conceptFormation_.get(); }
    
    // Concept formation configuration
    void enableConceptFormation(bool enable) { conceptFormationEnabled_ = enable; }
    bool isConceptFormationEnabled() const { return conceptFormationEnabled_; }
    void setConceptFormationThreshold(float threshold) { conceptFormationThreshold_ = threshold; }
    
    // Concept-aware action selection
    MotorCommand selectWithConcepts(MotorCommand defaultCmd, const std::vector<float>& sensoryState);
    
    // Prediction integration with concepts
    void updatePredictionFromConcepts();
    
    // Feature extraction for concepts
    std::vector<float> extractConceptFeatures(const SensoryPercept& percept) const;
    
    // Concept-based planning
    bool hasRelevantConcepts(const std::vector<float>& sensoryState) const;
    
    // Configuration methods for concept formation
    void setConceptFormationEnabled(bool enabled) { conceptFormationEnabled_ = enabled; }
    void setConceptFormationThreshold(float threshold) { conceptFormationThreshold_ = threshold; }
    
    // Concept formation state tracking
    bool isConceptFormationEnabled() const { return conceptFormationEnabled_; }
    float getConceptFormationThreshold() const { return conceptFormationThreshold_; }
    
    // Get concept formation status
    bool hasDiscoveredConcepts() const;
    size_t getDiscoveredConceptCount() const;
    
    // Get concept formation state for planning
    std::vector<float> getConceptState() const;
    
    // Integrate concept formation with prediction system
    void integrateConceptsWithPrediction();
    
    // Process sensory input through concept formation
    void processSensoryWithConcepts(const SensoryPercept& percept);
    
    // Planner configuration methods
    void setPlanningDepth(size_t depth) { if (planner_) planner_->setPlanningDepth(depth); }
    void setPlanningConfidence(float confidence) { if (planner_) planner_->setPlanningConfidence(confidence); }
    void enablePlanning(bool enable) { planningEnabled_ = enable; }
    bool isPlanningEnabled() const { return planningEnabled_; }
    
    // Goal-directed planning methods
    void setCurrentGoal(const std::vector<float>& goal) { 
        if (planner_) planner_->setCurrentGoal(goal); 
    }
    
    // Curiosity-exploration integration
    void updatePlanningFromCuriosity(float curiosityLevel) {
        if (planner_ && curiosityEnabled_) {
            if (curiosityLevel > 0.3f) {
                setExplorationBias(true);
            } else {
                setExplorationBias(false);
            }
        }
    }
    
    // Prediction-error guided planning
    void updatePlanningFromPredictionError(float error) {
        if (planner_) {
            // High prediction error increases exploration and plan modification
            if (error > 0.5f) {
                setPlanningDepth(getPlanningDepth() + 1);
            } else {
                setPlanningDepth(2); // Return to normal depth
            }
        }
    }
    
    void setExplorationBias(bool bias) { if (planner_) planner_->setExplorationBias(bias); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // State
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<NeuralPrediction> predictionSystem_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    bool planningEnabled_;
    bool explorationBias_;
    bool conceptFormationEnabled_;
    float conceptFormationThreshold_;
    
    // Development system
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    double developmentalAge_;
    float plasticityModifier_;
    float sensoryNoveltyDecay_;
    std::vector<float> previousVision_;
};

} // namespace nlm

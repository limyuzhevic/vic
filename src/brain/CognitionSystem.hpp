// CognitionSystem.h - Integrated cognition and decision-making system

#pragma once

#include "BrainImpl.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <memory>
#include <queue>

namespace nlm {

class CognitionSystem {
public:
    CognitionSystem();
    ~CognitionSystem() = default;
    
    // Initialize with brain reference
    void initialize(BrainImpl* brain);
    
    // Core cognition update
    void update(double timestep);
    
    // Planning and decision making
    std::unique_ptr<class Action> planAction(const std::vector<float>& sensoryInput);
    void setPlanningDepth(size_t depth) { planningDepth_ = depth; }
    void setActionSelectionThreshold(float threshold) { actionThreshold_ = threshold; }
    
    // Concept formation and abstraction
    void processSensoryInput(const std::vector<float>& input);
    const std::vector<std::string>& getCurrentConcepts() const { return currentConcepts_; }
    void addConcept(const std::string& concept) { currentConcepts_.push_back(concept); }
    
    // Attentional control
    void allocateAttention(const std::vector<NeuronId>& competingNeurons);
    float getAttentionPriority(NeuronId neuronId) const;
    void resetAttention() { attentionActive_ = false; }
    
    // Working memory integration
    void updateWorkingMemoryRepresentation();
    void retrieveRelevantMemories(const std::vector<float>& query);
    
    // Evaluation and selection
    float evaluateAction(const class Action& action, const std::vector<float>& context);
    std::unique_ptr<class Action> selectBestAction(const std::vector<std::unique_ptr<class Action>>& candidates);
    
    // State management
    void clearPlans() { plannedActions_.clear(); }
    size_t getPlanCount() const { return plannedActions_.size(); }
    bool hasActivePlans() const { return !plannedActions_.empty(); }
    
    // Configuration
    void setPlanningHorizon(double horizon) { planningHorizon_ = horizon; }
    void setAttentionSpan(double span) { attentionSpan_ = span; }
    void setConceptLearningRate(float rate) { conceptLearningRate_ = rate; }
    
    // Statistics
    float getPlanningEfficiency() const { return planningEfficiency_; }
    size_t getActiveConceptCount() const { return currentConcepts_.size(); }
    float getAttentionAllocationRatio() const { return attentionAllocationRatio_; }
    
    // Configuration
    void configure(size_t planningDepth, float actionThreshold, 
                   double planningHorizon, double attentionSpan);
    
private:
    BrainImpl* brain_ = nullptr;
    
    // Cognition subsystems
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    
    // Planning and decision making
    std::queue<std::unique_ptr<class Action>> plannedActions_;
    size_t planningDepth_;
    float actionThreshold_;
    double planningHorizon_;
    float planningEfficiency_;
    
    // Concept formation
    std::vector<std::string> currentConcepts_;
    float conceptLearningRate_;
    
    // Attentional control
    bool attentionActive_;
    double attentionSpan_;
    std::unordered_map<NeuronId, float> attentionWeights_;
    float attentionAllocationRatio_;
    
    // Working memory integration
    std::vector<NeuronId> relevantMemoryNeurons_;
    std::vector<float> memoryActivationLevels_;
    
    // Decision making
    std::vector<std::pair<std::unique_ptr<class Action>, float>> actionCache_;
    double lastDecisionTime_;
    
    // Helper methods
    void maintainPlans();
    void updateConcepts(double timestep);
    void manageAttention(double timestep);
    void updateWorkingMemoryIntegration(double timestep);
    void evaluatePlannedActions();
    void cacheAction(std::unique_ptr<class Action> action, float utility);
    std::unique_ptr<class Action> selectFromCache() const;
    void cleanupExpiredPlans(double currentTime);
    
    friend class BrainImpl;
};

} // namespace nlm

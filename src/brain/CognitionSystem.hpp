#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/BrainSystem.hpp"
#include <memory>
#include <vector>

namespace nlm {

// CognitionSystem: Manages all cognitive subsystems
// Integrates planning, concept formation, and attention
//
// Key mechanisms:
// - Provides unified interface to cognitive capabilities
// - Manages cognitive planning and execution
// - Coordinates concept formation and learning
// - Controls attentional focus and resource allocation

class CognitionSystem {
public:
    CognitionSystem();
    ~CognitionSystem();
    
    // Initialize cognition system
    void initialize(BrainSystem* brainSystem);
    
    // Planning operations
    ActionType planAction(const std::vector<float>& currentState,
                        float targetReward = 0.5f,
                        float confidenceThreshold = 0.7f);
    
    PlanningCandidate evaluateActionSequence(const std::vector<ActionType>& actions,
                                           const std::vector<float>& startState);
    
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                         const std::vector<ActionType>& actualActions,
                         float actualReward);
    
    // Concept formation
    void processCurrentPattern();  // Extract concepts from neural activity
    std::vector<float> getCurrentConcepts() const { return currentConcepts_; }
    void setConceptActivation(const std::vector<float>& concepts) { currentConcepts_ = concepts; }
    
    // Attentional selection
    std::vector<NeuronId> selectAttentionalFocus(const std::vector<NeuronId>& competitors,
                                               float globalInhibition = 0.5f);
    
    void focusAttentionOnRegion(RegionId region);
    void releaseAttention();
    std::vector<RegionId> getAttendedRegions() const { return attendedRegions_; }
    
    // Cognitive state management
    void setPlanningDepth(size_t depth) { planningDepth_ = depth; }
    size_t getPlanningDepth() const { return planningDepth_; }
    
    void setCurrentGoal(const std::vector<float>& goal) { currentGoal_ = goal; }
    std::vector<float> getCurrentGoal() const { return currentGoal_; }
    
    void setAttentionParameters(float inhibition, float excitation,
                               float competitionThreshold);
    
    // Update cognitive systems
    void updateAttention(TimestepDuration dt);
    void updateConceptFormation(TimestepDuration dt);
    
    // Cognitive statistics
    float getPlanningConfidence() const { return planningConfidence_; }
    bool wasRecentPlanSuccessful() const { return recentPlanSuccess_; }
    
    // Reset cognitive systems
    void reset();
    
    // Set brain reference for dependency injection
    void setBrain(BrainSystem* brainSystem) { brainSystem_ = brainSystem; }
    
private:
    BrainSystem* brainSystem_;
    
    // NeuralPlanner
    size_t planningDepth_;
    float planningConfidence_;
    std::vector<float> currentGoal_;
    std::vector<float> actionQuality_;
    std::deque<bool> recentPlanSuccess_;
    
    // ConceptFormation
    std::vector<float> currentConcepts_;
    float conceptActivationThreshold_;
    
    // AttentionalSelection
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> attentionalWinners_;
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> salienceBias_;
    
    // Update cognitive state
    void updateCognitiveState();
};

} // namespace nlm
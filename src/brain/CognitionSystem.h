// CognitionSystem.h - Aggregates neural planner, concept formation, attention
#pragma once

#include "BrainComponentBase.h"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class Brain;

class CognitionSystem : public BrainComponentBase {
public:
    CognitionSystem();
    ~CognitionSystem() override;
    
    // Initialize component with brain reference
    bool initialize(Brain* brain) override;
    
    // Update component state for current timestep
    void update(const TimestepDuration& dt) override;
    
    // Reset component to initial state
    void reset() override;
    
    // Log component status
    void logStatus() const override;
    
    // Get component name
    const char* getName() const override { return "CognitionSystem"; }
    
    // Access to individual cognition components
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    
    // Planner methods
    ActionType planAction(const std::vector<float>& currentState, float targetReward = 0.5f);
    void setPlanningDepth(size_t depth);
    
    // Concept formation methods
    void updateConcept(const std::vector<float>& pattern);
    
    // Attention methods
    void processCompetition(const std::vector<NeuronId>& competitors);
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
};

} // namespace nlm

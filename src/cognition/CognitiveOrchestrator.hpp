#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <memory>

namespace nlm {

class CognitiveOrchestrator {
public:
    CognitiveOrchestrator(std::shared_ptr<Config> config);
    ~CognitiveOrchestrator();
    
    // Initialize cognitive systems
    bool initialize(Brain* brain);
    
    // Update cognitive systems
    void update(Timestamp time, TimestepDuration timestep);
    
    // Reset cognitive systems
    void reset();
    
    // Get component access
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
    // Cognitive statistics
    size_t getActiveConcepts() const;
    size_t getActiveGoals() const;
    
    // Configuration methods
    void setPlanningDepth(size_t depth) { planningDepth = depth; }
    void setAttentionStrength(float excitation, float inhibition) {
        attentionExcitation = excitation;
        attentionInhibition = inhibition;
    }
    
private:
    struct Impl;
    Impl* pImpl;
    
    size_t planningDepth;
    float attentionExcitation;
    float attentionInhibition;
};

} // namespace nlm
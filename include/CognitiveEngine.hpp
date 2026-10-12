#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <memory>

namespace nlm {

// ==========================================
// COGNITIVE ENGINE - Cognitive Systems
// ==========================================
// 
// Manages: neural planning, concept formation, attention
// 
// Contains: NeuralPlanner, ConceptFormation, AttentionalSelection
// Handles: planning, concept formation, attention
// Manages: cognitive state and processes

class CognitiveEngine {
public:
    CognitiveEngine(std::shared_ptr<Config> config);
    ~CognitiveEngine();
    
    // Disable copying, enable moving
    CognitiveEngine(const CognitiveEngine&) = delete;
    CognitiveEngine& operator=(const CognitiveEngine&) = delete;
    CognitiveEngine(CognitiveEngine&&) noexcept;
    CognitiveEngine& operator=(CognitiveEngine&&) noexcept;
    
    // Initialize cognitive engine with brain reference
    bool initialize(Brain* brain);
    
    // Get cognitive system components
    class NeuralPlanner* getPlanner() { return planner_.get(); }
    const class NeuralPlanner* getPlanner() const { return planner_.get(); }
    
    class ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    const class ConceptFormation* getConceptFormation() const { return conceptFormation_.get(); }
    
    class AttentionalSelection* getAttention() { return attention_.get(); }
    const class AttentionalSelection* getAttention() const { return attention_.get(); }
    
    // Get brain reference
    Brain* getBrain() { return brain_; }
    const Brain* getBrain() const { return brain_; }
    
    // Update cognitive systems
    void update(TimestepDuration dt);
    
    // Planning
    void planAction();
    
    // Concept formation from neural activity
    void formConcepts();
    
    // Attention management
    void updateAttention(const std::vector<NeuronId>& competitors);
    
    // Get cognitive statistics
    size_t getPlanningDepth() const;
    float getAttentionFocus() const;
    
    // Set cognitive parameters
    void setPlanningDepth(size_t depth);
    void setAttentionParameters(float inhibition, float excitation);
    
    // Reset cognitive engine
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::shared_ptr<Config> config_;
    
    std::unique_ptr<class NeuralPlanner> planner_;
    std::unique_ptr<class ConceptFormation> conceptFormation_;
    std::unique_ptr<class AttentionalSelection> attention_;
};

} // namespace nlm
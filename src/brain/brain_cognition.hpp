#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;

// Cognitive systems integration
class BrainCognition {
public:
    BrainCognition() = default;
    ~BrainCognition() = default;
    
    // Initialize cognitive systems
    void initialize(std::shared_ptr<class Brain> brain);
    
    // Cognitive system updates (called from main step)
    void update(double timestep,
                std::shared_ptr<NeuralPlanner> planner,
                std::shared_ptr<ConceptFormation> conceptFormation,
                std::shared_ptr<AttentionalSelection> attention,
                std::shared_ptr<class NeuralWorkingMemory> workingMemory);
    
    // Get cognitive system accessors
    NeuralPlanner* getPlanner() const;
    ConceptFormation* getConceptFormation() const;
    AttentionalSelection* getAttention() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

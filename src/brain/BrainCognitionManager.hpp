#pragma once

#include "Brain.hpp"
#include <memory>
#include <vector>

namespace nlm {

class BrainCognitionManager {
public:
    BrainCognitionManager();
    ~BrainCognitionManager();
    
    // Disable copying
    BrainCognitionManager(const BrainCognitionManager&) = delete;
    BrainCognitionManager& operator=(const BrainCognitionManager&) = delete;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update neural planner
    void updatePlanner(Brain* brain, TimestepDuration timestep);
    
    // Update concept formation
    void updateConceptFormation(Brain* brain);
    
    // Update attention system
    void updateAttention(Brain* brain, TimestepDuration timestep);
    
    // Process competition for working memory
    void processWorkingMemoryCompetition(Brain* brain);
    
    // Get cognition system pointers
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

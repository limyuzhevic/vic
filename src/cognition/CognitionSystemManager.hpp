#pragma once

#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"

namespace nlm {

class Brain;

class CognitionSystemManager {
public:
    CognitionSystemManager();
    ~CognitionSystemManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update cognition systems
    void update();
    
    // Get cognition systems
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    
    // Configure from config
    void configureFromConfig(const Config& config);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
};

} // namespace nlm
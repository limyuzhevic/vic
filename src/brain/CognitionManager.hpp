#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class CognitionManager {
public:
    CognitionManager(Brain* brain);
    ~CognitionManager();
    
    bool initialize();
    void step(double timestep);
    void reset();
    
    // Cognition system access
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
    // Cognition processing
    void updatePlanner(double timestep);
    void updateConceptFormation(double timestep);
    void updateAttention(double timestep);
    void updateSelfModel(const std::vector<float>& lastVision, const std::vector<float>& currentVision);
    
private:
    Brain* brain_;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
};

} // namespace nlm

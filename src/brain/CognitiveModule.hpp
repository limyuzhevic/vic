#pragma once

#include "../core/Types/Types.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"

namespace nlm {

class CognitiveModule {
public:
    CognitiveModule(std::shared_ptr<NeuralPlanner> planner,
                   std::shared_ptr<ConceptFormation> conceptFormation,
                   std::shared_ptr<AttentionalSelection> attention);
    
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    void update(SimulationStep currentStep, Timestamp currentTime);
    void selectActions(const std::vector<float>& sensoryInput);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

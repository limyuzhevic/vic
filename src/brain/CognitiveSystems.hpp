// CognitiveSystems.hpp
// Cognitive systems interface

#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"

namespace nlm {

class CognitiveSystems {
public:
    void initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain);
    void processStep(SimulationStep currentStep, Timestamp currentTime);
    
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
private:
    void updateConceptFormation();
    void updatePredictionSystem();
    void extractNeuralActivityPattern(std::vector<float>& pattern);
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    size_t planningDepth_;
    float attentionInhibitionStrength_;
    float attentionExcitationStrength_;
};

} // namespace nlm

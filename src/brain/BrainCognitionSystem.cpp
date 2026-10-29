// Brain cognition system implementation
// Handles cognition system initialization

#include "brain/BrainCognitionSystem.hpp"
#include "brain/Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"

namespace nlm {

void BrainCognitionSystem::initializeCognitionSystems(Brain* brain) {
    if (!brain->pImpl->planner) {
        brain->pImpl->planner = std::make_unique<NeuralPlanner>();
    }
    brain->pImpl->planner->initialize(brain);
    brain->pImpl->planner->setPlanningDepth(5);
    
    if (!brain->pImpl->conceptFormation) {
        brain->pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    }
    brain->pImpl->conceptFormation->initialize(brain);
    
    if (!brain->pImpl->attention) {
        brain->pImpl->attention = std::make_unique<AttentionalSelection>();
    }
    brain->pImpl->attention->initialize(brain);
    brain->pImpl->attention->setInhibitionStrength(0.5f);
    brain->pImpl->attention->setExcitationStrength(1.5f);
}

} // namespace nlm

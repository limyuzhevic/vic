// Brain cognition system implementation - neural planner, concept formation, attention
#include "Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"

namespace nlm {

// Brain cognition system implementation - neural planner, concept formation, attention

// Cognition system accessors
NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

} // namespace nlm
// Brain_cognition.cpp implementation
// Contains:
// - Brain::getPlanner() method (lines 1028-1030 from original Brain.cpp)
// - Brain::getConceptFormation() method (lines 1032-1034 from original Brain.cpp)
// - Brain::getAttention() method (lines 1036-1038 from original Brain.cpp)
// - Any other cognition system methods

namespace nlm {

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}
}
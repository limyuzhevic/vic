#include "brain_memory_systems.hpp"

namespace nlm {

// Memory system accessors (lines 1006-1018 from Brain.cpp)
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// Prediction system accessor (line 1022 from Brain.cpp)
PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// Cognition system accessors (lines 1028-1038 from Brain.cpp)
NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// Development system accessor (lines 1042-1044 from Brain.cpp)
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

// Developmental stage accessors (lines 1046-1052 from Brain.cpp)
DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

} // namespace nlm

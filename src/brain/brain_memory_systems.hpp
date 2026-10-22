#pragma once

#include "Brain.hpp"

namespace nlm {

class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;

// Memory system accessors
NeuralWorkingMemory* Brain::getWorkingMemory();
NeuralEpisodicMemory* Brain::getEpisodicMemory();
NeuralAssociativeMemory* Brain::getAssociativeMemory();

// Prediction system accessor
PredictionSystem* Brain::getPredictionSystem();

// Cognition system accessors
NeuralPlanner* Brain::getPlanner();
ConceptFormation* Brain::getConceptFormation();
AttentionalSelection* Brain::getAttention();

// Development system accessor
DevelopmentSystem* Brain::getDevelopmentSystem();

// Developmental stage accessors
DevelopmentalStage Brain::getDevelopmentalStage() const;
void Brain::setDevelopmentalStage(DevelopmentalStage stage);

} // namespace nlm

#include "BrainComponents.hpp"

namespace nlm {

// Component access methods
SpikeSystem* BrainComponents::getSpikeSystem(Brain& brain) {
    return brain.getSpikeSystem();
}

const SpikeSystem* BrainComponents::getSpikeSystem(const Brain& brain) {
    return brain.getSpikeSystem();
}

STDP* BrainComponents::getSTDP(Brain& brain) {
    return brain.getSTDP();
}

Hebbian* BrainComponents::getHebbian(Brain& brain) {
    return brain.getHebbian();
}

StructuralPlasticity* BrainComponents::getStructuralPlasticity(Brain& brain) {
    return brain.getStructuralPlasticity();
}

// Statistics methods
float BrainComponents::getExcitationInhibitionRatio(const Brain& brain) {
    return brain.getExcitationInhibitionRatio();
}

size_t BrainComponents::getTotalSpikeCount(const Brain& brain) {
    return brain.getTotalSpikeCount();
}

size_t BrainComponents::getPendingSpikeEventCount(const Brain& brain) {
    return brain.getPendingSpikeEventCount();
}

// Global statistics
size_t BrainComponents::getTotalNeuronCount(const Brain& brain) {
    return brain.getTotalNeuronCount();
}

size_t BrainComponents::getTotalSynapseCount(const Brain& brain) {
    return brain.getTotalSynapseCount();
}

size_t BrainComponents::getActiveNeuronCount(const Brain& brain) {
    return brain.getActiveNeuronCount();
}

size_t BrainComponents::getFiringNeuronCount(const Brain& brain) {
    return brain.getFiringNeuronCount();
}

float BrainComponents::getAverageFiringRate(const Brain& brain) {
    return brain.getAverageFiringRate();
}

// Memory system accessors
NeuralWorkingMemory* BrainComponents::getWorkingMemory(Brain& brain) {
    return brain.getWorkingMemory();
}

NeuralEpisodicMemory* BrainComponents::getEpisodicMemory(Brain& brain) {
    return brain.getEpisodicMemory();
}

NeuralAssociativeMemory* BrainComponents::getAssociativeMemory(Brain& brain) {
    return brain.getAssociativeMemory();
}

// Prediction system accessor
PredictionSystem* BrainComponents::getPredictionSystem(Brain& brain) {
    return brain.getPredictionSystem();
}

// Cognition system accessors
NeuralPlanner* BrainComponents::getPlanner(Brain& brain) {
    return brain.getPlanner();
}

ConceptFormation* BrainComponents::getConceptFormation(Brain& brain) {
    return brain.getConceptFormation();
}

AttentionalSelection* BrainComponents::getAttention(Brain& brain) {
    return brain.getAttention();
}

// Development system
DevelopmentSystem* BrainComponents::getDevelopmentSystem(Brain& brain) {
    return brain.getDevelopmentSystem();
}

DevelopmentalStage BrainComponents::getDevelopmentalStage(const Brain& brain) {
    return brain.getDevelopmentalStage();
}

void BrainComponents::setDevelopmentalStage(Brain& brain, DevelopmentalStage stage) {
    brain.setDevelopmentalStage(stage);
}

// Neuromodulation systems
Dopamine* BrainComponents::getDopamine(Brain& brain) {
    return brain.getDopamine();
}

Curiosity* BrainComponents::getCuriosity(Brain& brain) {
    return brain.getCuriosity();
}

Novelty* BrainComponents::getNovelty(Brain& brain) {
    return brain.getNovelty();
}

PredictionError* BrainComponents::getPredictionErrorSignal(Brain& brain) {
    return brain.getPredictionErrorSignal();
}

// Config and random generator
std::shared_ptr<const Config> BrainComponents::getConfig(const Brain& brain) {
    return brain.getConfig();
}

RandomGenerator* BrainComponents::getRandomGenerator(Brain& brain) {
    return brain.getRandomGenerator();
}

} // namespace nlm
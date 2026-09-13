#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class NeuralRegion;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;

struct Brain::Impl;

// Helper methods
void Brain::receiveSensoryInput(const class SensoryInput& input);
void Brain::injectCurrent(NeuronId neuron, MembranePotential current);
void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current);

SpikeSystem* Brain::getSpikeSystem();
const SpikeSystem* Brain::getSpikeSystem() const;

STDP* Brain::getSTDP();

Hebbian* Brain::getHebbian();

StructuralPlasticity* Brain::getStructuralPlasticity();

float Brain::getExcitationInhibitionRatio() const;

size_t Brain::getTotalSpikeCount() const;
size_t Brain::getPendingSpikeEventCount() const;

std::unique_ptr<class Action> Brain::produceAction();

void Brain::applyNeuromodulation(const class Neuromodulator& signal);

void Brain::updatePlasticity();

void Brain::develop();

void Brain::reset();

bool Brain::save(const std::string& filepath) const;

bool Brain::load(const std::string& filepath);

RegionId Brain::addRegion(const std::string& name);

NeuralRegion* Brain::getRegion(RegionId id);

const NeuralRegion* Brain::getRegion(RegionId id) const;

size_t Brain::getRegionCount() const;

std::vector<RegionId> Brain::getRegionIds() const;

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const;

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay);
void Brain::removeInterRegionConnection(RegionId source, RegionId target);

size_t Brain::getTotalNeuronCount() const;
size_t Brain::getTotalSynapseCount() const;
size_t Brain::getActiveNeuronCount() const;
size_t Brain::getFiringNeuronCount() const;
float Brain::getAverageFiringRate() const;

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

// Development system
DevelopmentSystem* Brain::getDevelopmentSystem();
DevelopmentStage Brain::getDevelopmentalStage() const;
void Brain::setDevelopmentalStage(DevelopmentalStage stage);

// Neuromodulation systems
Dopamine* Brain::getDopamine();
Curiosity* Brain::getCuriosity();
Novelty* Brain::getNovelty();
PredictionError* Brain::getPredictionErrorSignal();

// Utility methods
std::shared_ptr<const Config> Brain::getConfig() const;
RandomGenerator* Brain::getRandomGenerator();

void Brain::logStatus() const;

} // namespace nlm
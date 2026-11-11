#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>

namespace nlm {

// Brain constructor
Brain::Brain(std::shared_ptr<Config> cfg)
    : pImpl(std::make_unique<Impl>(cfg)) {
    NLM_LOG_DEBUG("Brain created with config");
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept = default;

Brain& Brain::operator=(Brain&& other) noexcept = default;

// Brain initialization - creates all integrated systems
void Brain::initialize() {
    pImpl->initialize();
}

void Brain::step(SimulationStep step, TimestepDuration timestep) {
    pImpl->step(step, timestep);
}

void Brain::reset() {
    pImpl->reset();
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    pImpl->receiveSensoryInput(input);
}

void Brain::applyReward(float reward) {
    pImpl->applyReward(reward);
}

void Brain::applyNeuromodulation() {
    pImpl->applyNeuromodulation();
}

void Brain::save() {
    pImpl->save();
}

void Brain::load() {
    pImpl->load();
}

void Brain::logStatus() const {
    pImpl->logStatus();
}

bool Brain::injectCurrent(NeuronId neuronId, CurrentInjection current) {
    return pImpl->injectCurrent(neuronId, current);
}

float Brain::getInjectedCurrent() const {
    return pImpl->getInjectedCurrent();
}

void Brain::clearInjection() {
    pImpl->clearInjection();
}

const NeuralRegion* Brain::getRegion(RegionId regionId) const {
    return pImpl->getRegion(regionId);
}

const NeuralPopulation* Brain::getPopulation(RegionId regionId, PopulationId populationId) const {
    return pImpl->getPopulation(regionId, populationId);
}

const Neuron* Brain::getNeuron(NeuronId neuronId) const {
    return pImpl->getNeuron(neuronId);
}

const Synapse* Brain::getSynapse(SynapseHandle synapseHandle) const {
    return pImpl->getSynapse(synapseHandle);
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->getRegionIds();
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->getTotalSynapseCount();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->getTotalSpikeCount();
}

std::size_t Brain::getFiringNeuronCount() const {
    return pImpl->getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->getAverageFiringRate();
}

float Brain::getExcitatoryInhibitionRatio() const {
    return pImpl->getExcitatoryInhibitionRatio();
}

std::size_t Brain::getMostActiveNeuronCount() const {
    return pImpl->getMostActiveNeuronCount();
}

const PredictionSystem* Brain::getPredictionSystem() const {
    return pImpl->getPredictionSystem();
}

const NeuralPlanner* Brain::getPlanner() const {
    return pImpl->getPlanner();
}

const ConceptFormation* Brain::getConceptFormation() const {
    return pImpl->getConceptFormation();
}

const NeuralWorkingMemory* Brain::getWorkingMemory() const {
    return pImpl->getWorkingMemory();
}

const NeuralEpisodicMemory* Brain::getEpisodicMemory() const {
    return pImpl->getEpisodicMemory();
}

const NeuralAssociativeMemory* Brain::getAssociativeMemory() const {
    return pImpl->getAssociativeMemory();
}

const DevelopmentSystem* Brain::getDevelopmentSystem() const {
    return pImpl->getDevelopmentSystem();
}

const Neuromodulator* Brain::getDopamine() const {
    return pImpl->getDopamine();
}

const Curiosity* Brain::getCuriosity() const {
    return pImpl->getCuriosity();
}

const Novelty* Brain::getNovelty() const {
    return pImpl->getNovelty();
}

const PredictionError* Brain::getPredictionError() const {
    return pImpl->getPredictionError();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->getSpikeSystem();
}

const STDP* Brain::getSTDP() const {
    return pImpl->getSTDP();
}

const Hebbian* Brain::getHebbian() const {
    return pImpl->getHebbian();
}

const StructuralPlasticity* Brain::getStructuralPlasticity() const {
    return pImpl->getStructuralPlasticity();
}

const RandomGenerator* Brain::getRandomGenerator() const {
    return pImpl->getRandomGenerator();
}

const CheckpointManager* Brain::getCheckpointManager() const {
    return pImpl->getCheckpointManager();
}

const AttentionSystem* Brain::getAttentionSystem() const {
    return pImpl->getAttentionSystem();
}

void Brain::setLearnedPreference(float preference) {
    pImpl->setLearnedPreference(preference);
}

float Brain::getLearnedPreference() const {
    return pImpl->getLearnedPreference();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->setDevelopmentalStage(stage);
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->getDevelopmentalStage();
}

} // namespace nlm
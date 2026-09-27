// Brain implementation - simplified facade that delegates to Brain::Impl
// This file has been completely refactored to improve modularity and maintainability
// by extracting implementation details into Brain_Impl.hpp

#include "Brain.hpp"
#include "Brain_Impl.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() {
    delete pImpl;
}

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    return pImpl->initialize();
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->step(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    pImpl->receiveSensoryInput(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    pImpl->injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->injectCurrentToNeurons(type, current);
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->getSpikeSystem();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->getSpikeSystem();
}

STDP* Brain::getSTDP() {
    return pImpl->getSTDP();
}

Hebbian* Brain::getHebbian() {
    return pImpl->getHebbian();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->getStructuralPlasticity();
}

float Brain::getExcitationInhibitionRatio() const {
    return pImpl->getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->getTotalSpikeCount();
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->getPendingSpikeEventCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    return pImpl->produceAction();
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    pImpl->applyNeuromodulation(signal);
}

void Brain::updatePlasticity() {
    pImpl->updatePlasticity();
}

void Brain::develop() {
    pImpl->develop();
}

void Brain::reset() {
    pImpl->reset();
}

bool Brain::save(const std::string& filepath) const {
    return pImpl->save(filepath);
}

bool Brain::load(const std::string& filepath) {
    return pImpl->load(filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl->addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target,
                                    float weight, Delay delay) {
    pImpl->addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->removeInterRegionConnection(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->getAverageFiringRate();
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->getAssociativeMemory();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->getPredictionSystem();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->getAttention();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->getDevelopmentSystem();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->setDevelopmentalStage(stage);
}

Dopamine* Brain::getDopamine() {
    return pImpl->getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->getCuriosity();
}

Novelty* Brain::getNovelty() {
    return pImpl->getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->getPredictionErrorSignal();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->getConfig();
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->getRandomGenerator();
}

void Brain::logStatus() const {
    pImpl->logStatus();
}

} // namespace nlm

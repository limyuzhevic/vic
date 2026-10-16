#include "Brain.hpp"
#include "BrainCore.hpp"
#include "BrainStep.hpp"
#include "BrainRegions.hpp"
#include "BrainMemory.hpp"
#include "BrainNeuromodulation.hpp"
#include "BrainUtils.hpp"

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    
    // Component delegation
    std::unique_ptr<BrainCore> core;
    std::unique_ptr<BrainStep> step;
    std::unique_ptr<BrainRegions> regions;
    std::unique_ptr<BrainMemory> memory;
    std::unique_ptr<BrainNeuromodulation> neuromodulation;
    std::unique_ptr<BrainUtils> utils;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , core(std::make_unique<BrainCore>(cfg))
        , step(std::make_unique<BrainStep>(cfg))
        , regions(std::make_unique<BrainRegions>(cfg))
        , memory(std::make_unique<BrainMemory>(cfg))
        , neuromodulation(std::make_unique<BrainNeuromodulation>(cfg))
        , utils(std::make_unique<BrainUtils>(cfg))
    {
        // Initialize the actual implementation from BrainCore
        // For now, just initialize core components
        // In a real implementation, you'd need to wire everything together
    }
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

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
    return pImpl->core->initialize();
}

void Brain::step(SimulationStep currentStep) {
    pImpl->step->step(currentStep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->step->step(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    pImpl->step->receiveSensoryInput(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    pImpl->step->injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Delegate to the regions component for proper implementation
    pImpl->regions->injectCurrentToNeurons(type, current);
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->utils->getSpikeSystem();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->utils->getSpikeSystem();
}

STDP* Brain::getSTDP() {
    return pImpl->utils->getSTDP();
}

Hebbian* Brain::getHebbian() {
    return pImpl->utils->getHebbian();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->utils->getStructuralPlasticity();
}

float Brain::getExcitationInhibitionRatio() const {
    return pImpl->utils->getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->utils->getTotalSpikeCount();
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->utils->getPendingSpikeEventCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    return pImpl->utils->produceAction();
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    pImpl->neuromodulation->applyNeuromodulation(signal);
}

void Brain::updatePlasticity() {
    pImpl->utils->updatePlasticity();
}

void Brain::develop() {
    pImpl->neuromodulation->develop();
}

void Brain::reset() {
    pImpl->core->reset();
}

bool Brain::save(const std::string& filepath) const {
    return pImpl->core->save(filepath);
}

bool Brain::load(const std::string& filepath) {
    return pImpl->core->load(filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl->regions->addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->regions->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->regions->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->regions->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->regions->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target,
                                    float weight, Delay delay) {
    pImpl->regions->addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->regions->removeInterRegionConnection(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->regions->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->regions->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->regions->getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->regions->getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->regions->getAverageFiringRate();
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->memory->getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->memory->getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->memory->getAssociativeMemory();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->memory->getPredictionSystem();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->memory->getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->memory->getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->memory->getAttention();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->neuromodulation->getDevelopmentSystem();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->neuromodulation->getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->neuromodulation->setDevelopmentalStage(stage);
}

Dopamine* Brain::getDopamine() {
    return pImpl->neuromodulation->getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->neuromodulation->getCuriosity();
}

Novelty* Brain::getNovelty() {
    return pImpl->neuromodulation->getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->neuromodulation->getPredictionErrorSignal();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->core->getConfig();
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->core->getRandomGenerator();
}

void Brain::logStatus() const {
    pImpl->utils->logStatus();
}

} // namespace nlm
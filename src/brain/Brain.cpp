#include "Brain.hpp"
#include "BrainCore.hpp"
#include "BrainMemory.hpp"
#include "BrainCognition.hpp"
#include "BrainDevelopment.hpp"
#include "BrainNeuromodulation.hpp"
#include "BrainPlasticity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Modified Brain class - facade that delegates to component classes
// This maintains backward compatibility while using the new modular architecture

struct Brain::Impl {
    std::shared_ptr<Config> config;
    
    // Component classes
    BrainCore brainCore;
    BrainMemory brainMemory;
    BrainCognition brainCognition;
    BrainDevelopment brainDevelopment;
    BrainNeuromodulation brainNeuromodulation;
    BrainPlasticity brainPlasticity;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , brainCore(cfg)
        , brainMemory()
        , brainCognition()
        , brainDevelopment()
        , brainNeuromodulation()
        , brainPlasticity()
    {
        // Integrate all components
        brainCore.integrateWithMemorySystems();
        brainCore.integrateWithCognitionSystems();
        brainCore.integrateWithDevelopmentSystem();
        brainCore.integrateWithNeuromodulationSystems();
        brainCore.integrateWithPlasticitySystems();
        
        brainMemory.integrateWithBrainCore(&brainCore);
        brainCognition.integrateWithBrainCore(&brainCore);
        brainDevelopment.integrateWithBrainCore(&brainCore);
        brainNeuromodulation.integrateWithBrainCore(&brainCore);
        brainPlasticity.integrateWithBrainCore(&brainCore);
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
    return pImpl->brainCore.initialize();
}

void Brain::step(SimulationStep currentStep) {
    pImpl->brainCore.step(currentStep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->brainCore.step(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    pImpl->brainCore.receiveSensoryInput(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    pImpl->brainCore.injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->brainCore.injectCurrentToNeurons(type, current);
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->brainPlasticity.getSpikeSystem();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->brainPlasticity.getSpikeSystem();
}

STDP* Brain::getSTDP() {
    return pImpl->brainPlasticity.getSTDP();
}

Hebbian* Brain::getHebbian() {
    return pImpl->brainPlasticity.getHebbian();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->brainPlasticity.getStructuralPlasticity();
}

float Brain::getExcitationInhibitionRatio() const {
    return pImpl->brainCore.getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->brainCore.getTotalSpikeCount();
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->brainCore.getPendingSpikeEventCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    return pImpl->brainCore.produceAction();
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Neuromodulation is now integrated into the core brain loop
    // This method is kept for API compatibility
}

void Brain::updatePlasticity() {
    // Plasticity is now integrated into the core brain loop
    // This method is kept for API compatibility
}

void Brain::develop() {
    pImpl->brainDevelopment.develop();
}

void Brain::reset() {
    pImpl->brainCore.reset();
    pImpl->brainMemory.clearMemorySystems();
}

bool Brain::save(const std::string& filepath) const {
    // TODO: Implement brain state saving with components
    NLM_LOG_INFO("Brain::save - not yet implemented with component architecture");
    return false;
}

bool Brain::load(const std::string& filepath) {
    // TODO: Implement brain state loading with components
    NLM_LOG_INFO("Brain::load - not yet implemented with component architecture");
    return false;
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl->brainCore.addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->brainCore.getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->brainCore.getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->brainCore.getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->brainCore.getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->brainCore.getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->brainCore.addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->brainCore.removeInterRegionConnection(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->brainCore.getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->brainCore.getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->brainCore.getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->brainCore.getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->brainCore.getAverageFiringRate();
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->brainMemory.getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->brainMemory.getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->brainMemory.getAssociativeMemory();
}

PredictionSystem* Brain::getPredictionSystem() {
    // Prediction system would be in a separate component
    return nullptr;
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->brainCognition.getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->brainCognition.getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->brainCognition.getAttention();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->brainDevelopment.getDevelopmentSystem();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->brainDevelopment.getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->brainDevelopment.setDevelopmentalStage(stage);
}

Dopamine* Brain::getDopamine() {
    return pImpl->brainNeuromodulation.getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->brainNeuromodulation.getCuriosity();
}

Novelty* Brain::getNovelty() {
    return pImpl->brainNeuromodulation.getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->brainNeuromodulation.getPredictionError();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->brainCore.getRandomGenerator();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Component Architecture) ===");
    pImpl->brainCore.logStatus();
    
    if (pImpl->brainMemory.getWorkingMemory()) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->brainMemory.getWorkingMemoryTraceCount()));
    }
    if (pImpl->brainMemory.getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->brainMemory.getEpisodicMemoryEpisodeCount()));
    }
    
    if (pImpl->brainNeuromodulation.getDopamine()) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->brainNeuromodulation.getDopamineLevel()));
    }
    
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->brainDevelopment.getDevelopmentalStage())));
}

} // namespace nlm
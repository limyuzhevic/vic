// Brain.cpp - Implementation of the Brain facade using SynapseNetwork component
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<SynapseNetwork> synapseNetwork;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
    SimulationStep currentStep;
    Timestamp currentTime;
    bool initialized;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg), currentStep(0), currentTime(0.0), initialized(false) {
    }
};

Brain::Brain(std::shared_ptr<Config> config) {
    pImpl = std::make_unique<Impl>(config);
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Brain::initialize() {
    if (pImpl->initialized) {
        return true;
    }
    
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Initialize core synapse network
    pImpl->synapseNetwork = std::make_unique<SynapseNetwork>(pImpl->config);
    if (!pImpl->synapseNetwork->initialize()) {
        NLM_LOG_ERROR("Failed to initialize synapse network!");
        return false;
    }
    
    // Initialize other components
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->attention = std::make_unique<AttentionalSelection>();
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    
    // Set up component interconnections
    pImpl->dopamine->initialize(pImpl->getDopamine());
    pImpl->curiosity->initialize(pImpl->getCuriosity());
    pImpl->novelty->initialize(pImpl->getNovelty());
    pImpl->predictionError->initialize(pImpl->getPredictionErrorSignal());
    
    pImpl->developmentSystem->initialize(pImpl->config.get());
    pImpl->predictionSystem->initialize(pImpl->config.get());
    pImpl->planner->initialize(this);
    pImpl->conceptFormation->initialize(this);
    pImpl->attention->initialize(this);
    
    pImpl->initialized = true;
    
    NLM_LOG_INFO("NLM Brain initialization complete!");
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, currentStep * pImpl->config->getOr<double>("simulation_timestep", 0.001));
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl->initialized) {
        return;
    }
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    
    // Update development system
    pImpl->developmentSystem->update(pImpl->currentTime);
    
    // Update neuromodulation systems
    pImpl->dopamine->update(pImpl->currentTime);
    pImpl->curiosity->update(pImpl->currentTime);
    pImpl->novelty->update(pImpl->currentTime);
    pImpl->predictionError->update(pImpl->currentTime);
    
    // Update spike system
    double timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
    pImpl->synapseNetwork->updateSpiking(timestep);
    
    // Update working memory
    pImpl->workingMemory->update(timestep);
    
    // Update episodic memory
    pImpl->episodicMemory->update(timestep);
    
    // Update cognitive systems
    pImpl->planner->update(timestep);
    pImpl->conceptFormation->update(timestep);
    pImpl->attention->update(timestep);
    
    // Update plasticity
    updatePlasticity();
    
    pImpl->currentStep++;
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    // Convert sensory input to current injections
    // This is a simplified implementation
    if (input.getType() == SensoryInputType::Vision) {
        pImpl->synapseNetwork->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
    } else if (input.getType() == SensoryInputType::Audio) {
        pImpl->synapseNetwork->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    pImpl->synapseNetwork->injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->synapseNetwork->injectCurrentToNeurons(type, current);
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->synapseNetwork->getSpikeSystem();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->synapseNetwork->getSpikeSystem();
}

STDP* Brain::getSTDP() {
    return pImpl->synapseNetwork->getSTDP();
}

Hebbian* Brain::getHebbian() {
    return pImpl->synapseNetwork->getHebbian();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->synapseNetwork->getStructuralPlasticity();
}

float Brain::getExcitationInhibitionRatio() const {
    return pImpl->synapseNetwork->getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->synapseNetwork->getSpikeSystem() ? 
        pImpl->synapseNetwork->getSpikeSystem()->getTotalSpikeCount() : 0;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->synapseNetwork->getSpikeSystem() ? 
        pImpl->synapseNetwork->getSpikeSystem()->getPendingSpikeEventCount() : 0;
}

std::unique_ptr<Action> Brain::produceAction() {
    // Simple action production based on neural activity
    // This is a placeholder implementation
    auto action = std::make_unique<Action>();
    action->setType(ActionType::Wait);
    return action;
}

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    // Apply neuromodulatory signals to relevant neurons
    // This is a simplified implementation
    pImpl->dopamine->applySignal(signal);
}

void Brain::updatePlasticity() {
    // Update plasticity rules based on current neuromodulation state
    // This is a simplified implementation
    if (pImpl->dopamine) {
        float neuromodulationLevel = pImpl->dopamine->getLevel();
        if (auto* stdp = getSTDP()) {
            stdp->update(neuromodulationLevel);
        }
        if (auto* hebbian = getHebbian()) {
            hebbian->update(neuromodulationLevel);
        }
    }
}

void Brain::develop() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(pImpl->currentTime);
    }
}

void Brain::reset() {
    if (pImpl->synapseNetwork) {
        pImpl->synapseNetwork->reset();
    }
    if (pImpl->workingMemory) {
        pImpl->workingMemory->clear();
    }
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->clear();
    }
    pImpl->initialized = false;
}

bool Brain::save(const std::string& filepath) const {
    // Simplified save implementation
    return pImpl->synapseNetwork->save(filepath);
}

bool Brain::load(const std::string& filepath) {
    // Simplified load implementation
    return pImpl->synapseNetwork->load(filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl->synapseNetwork->addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->synapseNetwork->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->synapseNetwork->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->synapseNetwork->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->synapseNetwork->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->synapseNetwork->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target,
                                    float weight, Delay delay) {
    pImpl->synapseNetwork->addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->synapseNetwork->removeInterRegionConnection(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->synapseNetwork->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->synapseNetwork->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->synapseNetwork->getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->synapseNetwork->getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->synapseNetwork->getAverageFiringRate();
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    // Return nullptr for now - this will be implemented when the component is created
    return nullptr;
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentSystem ? 
        pImpl->developmentSystem->getDevelopmentalStage() : 
        DevelopmentalStage::Initial;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->setDevelopmentalStage(stage);
    }
}

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    // Return a random generator - this will need to be set up properly
    static RandomGenerator dummy(42);
    return &dummy;
}

float Brain::getLearnedPreference() const {
    // Return the learned preference from dopamine system
    return pImpl->dopamine ? pImpl->dopamine->getPreference() : 0.0f;
}

void Brain::setLearnedPreference(float preference) {
    if (pImpl->dopamine) {
        pImpl->dopamine->setPreference(preference);
    }
}

void Brain::logStatus() const {
    std::cout << "=== NLM Brain Status ===" << std::endl;
    std::cout << "Initialized: " << (pImpl->initialized ? "Yes" : "No") << std::endl;
    std::cout << "Regions: " << getRegionCount() << std::endl;
    std::cout << "Neurons: " << getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << getTotalSynapseCount() << std::endl;
    std::cout << "Active neurons: " << getActiveNeuronCount() << std::endl;
    std::cout << "Firing neurons: " << getFiringNeuronCount() << std::endl;
    std::cout << "Average firing rate: " << getAverageFiringRate() << std::endl;
    std::cout << "Excitatory/Inhibitory ratio: " << getExcitationInhibitionRatio() << std::endl;
    std::cout << "Total spikes: " << getTotalSpikeCount() << std::endl;
    std::cout << "Pending spike events: " << getPendingSpikeEventCount() << std::endl;
    std::cout << "Working memory traces: " << (getWorkingMemory() ? 
        getWorkingMemory()->getActiveTraces() : 0) << std::endl;
    std::cout << "Episodic memory episodes: " << (getEpisodicMemory() ? 
        getEpisodicMemory()->getEpisodeCount() : 0) << std::endl;
    std::cout << "Learned preference: " << getLearnedPreference() << std::endl;
    std::cout << "================================" << std::endl;
}

} // namespace nlm
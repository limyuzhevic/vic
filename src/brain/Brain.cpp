// Brain.cpp - Phase 2: Basic Neural Computation (Refactored)
// Core brain implementation with improved organization

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

// Helper functions for Brain class implementation
namespace brain_helpers {
    // Initialize neuron population and synaptic connections
    bool initializeNeuronsAndSynapses(Brain::Impl& impl);
    
    // Register spike handlers for event-driven computation
    void registerSpikeHandlers(Brain::Impl& impl);
    
    // Update spike propagation system
    void updateSpikeSystem(Brain::Impl& impl, SimulationStep step, Timestamp time);
    
    // Apply neuromodulation signals to neural populations
    void applyNeuromodulationToNeurons(Brain::Impl& impl);
    
    // Apply plasticity rules to synaptic connections
    void applyPlasticityRules(Brain::Impl& impl, float plasticityMod, Timestamp time);
    
    // Update homeostatically with synaptic scaling
    void applySynapticScaling(Brain::Impl& impl, SimulationStep step);
    
    // Update episodic memory with recent neural activity
    void updateEpisodicMemory(Brain::Impl& impl, SimulationStep step, float reward);
    
    // Create motor action from current neural state
    std::unique_ptr<Action> createMotorAction(const Brain::Impl& impl);
    
    // Save brain state to checkpoint file
    bool saveBrainState(const Brain::Impl& impl, const std::string& filepath);
    
    // Load brain state from checkpoint file
    bool loadBrainState(Brain::Impl& impl, const std::string& filepath);
}

// Brain constructor/destructor
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
    NLM_LOG_INFO("Initializing NLM Brain (Phase 2: Real Neural Computation)...");

    // Re-initialising must rebuild, not append: a previous run left
    // reset()+initialize() reporting 1000 neurons for a 500-neuron brain.
    pImpl->regions.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    pImpl->nextRegionId = RegionId(1);
    pImpl->totalSpikesTotal = 0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    
    NLM_LOG_INFO("  Creating neuron populations...");
    if (!brain_helpers::initializeNeuronsAndSynapses(*pImpl)) {
        NLM_LOG_ERROR("Failed to initialize neurons and synapses");
        return false;
    }
    
    NLM_LOG_INFO("  Registering spike handlers...");
    brain_helpers::registerSpikeHandlers(*pImpl);
    
    // Log initialization success
    logStatus();
    return true;
}

void Brain::step(SimulationStep currentStep) {
    double timestep = pImpl->timestep;
    Timestamp currentTime = currentStep * timestep;
    step(currentStep, currentTime);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Update step counters
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Apply sensory input ==========
    applySensoryInput();
    
    // ========== STEP 2: Propagate neural spikes ==========
    brain_helpers::updateSpikeSystem(*pImpl, currentStep, currentTime);
    
    // ========== STEP 3: Apply neuromodulatory signals ==========
    brain_helpers::applyNeuromodulationToNeurons(*pImpl);
    
    // ========== STEP 4: Update plasticity rules ==========
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    brain_helpers::applyPlasticityRules(*pImpl, plasticityMod, currentTime);
    
    // ========== STEP 5: Apply homeostatic synaptic scaling ==========
    brain_helpers::applySynapticScaling(*pImpl, currentStep);
    
    // ========== STEP 6: Update episodic memory ==========
    float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
    brain_helpers::updateEpisodicMemory(*pImpl, currentStep, reward);
    
    // ========== STEP 7: Create motor action ==========
    // Motor action is created on demand, not in every step
    // Action will be created by calling produceAction() when needed
}

void Brain::applySensoryInput() {
    // This is handled by the sensory system, not directly here
    // The brain receives sensory input via receiveSensoryInput() method
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input pattern
    if (input.isEmpty()) return;
    
    // Process each neuron in the input
    auto inputNeurons = input.getNeurons();
    for (const auto& inputNeuron : inputNeurons) {
        injectCurrent(inputNeuron.getId(), inputNeuron.getCurrent());
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Find neuron by ID and inject current
    auto it = pImpl->neuronById.find(neuron.value);
    if (it != pImpl->neuronById.end()) {
        it->second->injectCurrent(current);
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& population : region->getPopulations()) {
            for (auto& neuron : population->getNeurons()) {
                if (neuron->getType() == type) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    float totalExc = 0.0f, totalInh = 0.0f;
    for (const auto& region : pImpl->regions) {
        float regionExc = 0.0f, regionInh = 0.0f;
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0.0f) regionExc += weight;
            else if (weight < 0.0f) regionInh += std::abs(weight);
        }
        totalExc += regionExc;
        totalInh += regionInh;
    }
    return totalInh > 0.0f ? totalExc / totalInh : 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    size_t total = pImpl->totalSpikesTotal;
    if (pImpl->spikeSystem) {
        total += pImpl->spikeSystem->getTotalSpikeCount();
    }
    return total;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem ? pImpl->spikeSystem->getPendingEventCount() : 0;
}

std::unique_ptr<class Action> Brain::produceAction() {
    return brain_helpers::createMotorAction(*pImpl);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulatory signal to appropriate systems
    if (signal.getType() == NeuromodulatorType::Dopamine) {
        if (pImpl->dopamine) {
            pImpl->dopamine->setLevel(signal.getLevel());
        }
    } else if (signal.getType() == NeuromodulatorType::Curiosity) {
        if (pImpl->curiosity) {
            pImpl->curiosity->setLevel(signal.getLevel());
        }
    }
    // Additional neuromodulator types can be added here
}

void Brain::updatePlasticity() {
    // Plasticity is applied during the step() function
    // This method provides a way to manually trigger plasticity updates
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    brain_helpers::applyPlasticityRules(*pImpl, plasticityMod, pImpl->currentTime);
}

void Brain::develop() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(pImpl->timestep);
    }
}

void Brain::reset() {
    // Reset all neural components
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    // Reset spike system
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    
    // Reset memory systems
    if (pImpl->workingMemory) {
        pImpl->workingMemory->reset();
    }
    
    // Reset neuromodulation systems
    if (pImpl->dopamine) {
        pImpl->dopamine->reset();
    }
    
    // Reset development system
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->reset();
    }
    
    // Reset counters
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->stepsSinceLastEpisode = 0;
    pImpl->isResting = false;
}

bool Brain::save(const std::string& filepath) const {
    return brain_helpers::saveBrainState(*pImpl, filepath);
}

bool Brain::load(const std::string& filepath) {
    return brain_helpers::loadBrainState(*pImpl, filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    auto region = std::make_unique<NeuralRegion>(nextRegionId, name);
    RegionId id = region->getId();
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
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
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
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
    return pImpl->rng.get();
}

float Brain::getLearnedPreference() const {
    return pImpl->learnedPreference;
}

void Brain::setLearnedPreference(float preference) {
    pImpl->learnedPreference = std::clamp(preference, -1.0f, 1.0f);
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 2 - Real Neural Computation) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

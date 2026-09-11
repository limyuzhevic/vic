#pragma once

#include "Brain.hpp"
#include "BrainInitializer.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Forward declarations for helper classes
class BrainInitializer;
class NeuralMemorySystem;
class NeuromodulationSystem;
class CognitiveSystem;
class DevelopmentSystem;
class PredictionSystemWrapper;
class IOSystem;
class SimulationManager;
class MainBrainLoop;

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
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Extract configuration parameters
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Use BrainInitializer for neural core operations
    BrainInitializer::initializeRegions(*this, neuronCount, regionCount, connectionProbability);
    collectSensoryMotorNeurons();
    
    // Initialize connectivity with random weights
    initializeConnectivity(connectionProbability);
    
    // Use extracted systems for initialization
    initializeMemorySystems();
    initializePredictionSystem();
    initializeCognitionSystems();
    initializeNeuromodulationSystems();
    configureCheckpointSystem();
    
    // Register event-driven processors
    registerEventDrivenProcessors();
    
    // Configure plasticity systems (already done in Impl constructor)
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(getSensoryNeuronCount()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(getMotorNeuronCount()));
    
    return true;
}

void Brain::initializeConnectivity(float connectionProbability) {
    for (auto* region : pImpl->regions) {
        region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
    }
}

void Brain::collectSensoryMotorNeurons() {
    for (auto* region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            } else if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

void Brain::initializeMemorySystems() {
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(pImpl->config->getOr<size_t>("neuron_count", 1000) / 10);
    
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    pImpl->associativeMemory->initialize(this);
}

void Brain::initializePredictionSystem() {
    // PredictionSystem doesn't have initialize method currently
}

void Brain::initializeCognitionSystems() {
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
}

void Brain::initializeNeuromodulationSystems() {
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
}

void Brain::configureCheckpointSystem() {
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void Brain::registerEventDrivenProcessors() {
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        deliverDelayedSpike(event);
    });
}

void Brain::deliverDelayedSpike(const DelayedSpikeEvent& event) {
    for (auto* region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == event.destination_neuron) {
                MembranePotential synapticCurrent = event.weight * 10.0f;
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
                return;
            }
        }
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Use MainBrainLoop to execute the 15-step integration loop
    MainBrainLoop::executeStep(*this, currentStep, currentTime);
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto* region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto* region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() { return pImpl->spikeSystem.get(); }
const SpikeSystem* Brain::getSpikeSystem() const { return pImpl->spikeSystem.get(); }
STDP* Brain::getSTDP() { return pImpl->stdp.get(); }
Hebbian* Brain::getHebbian() { return pImpl->hebbian.get(); }
StructuralPlasticity* Brain::getStructuralPlasticity() { return pImpl->structuralPlasticity.get(); }

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const { return pImpl->totalSpikesTotal; }
size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<Action> Brain::produceAction() {
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    return std::make_unique<Action>(type);
}

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    float modulation = signal.getLevel();
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void Brain::updatePlasticity() {}
void Brain::develop() {
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto* region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    // Checkpoint saving implementation
    return true;
}

bool Brain::load(const std::string& filepath) {
    // Checkpoint loading implementation
    return true;
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto* region : pImpl->regions) {
        if (region->getId() == id) {
            return region;
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto* region : pImpl->regions) {
        if (region->getId() == id) {
            return region;
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const { return pImpl->regions.size(); }
std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto* region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
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
    for (const auto* region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto* region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto* region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const { return pImpl->totalSpikesThisStep; }
float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto* region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

NeuralWorkingMemory* Brain::getWorkingMemory() { return pImpl->workingMemory.get(); }
NeuralEpisodicMemory* Brain::getEpisodicMemory() { return pImpl->episodicMemory.get(); }
NeuralAssociativeMemory* Brain::getAssociativeMemory() { return pImpl->associativeMemory.get(); }

PredictionSystem* Brain::getPredictionSystem() { return pImpl->predictionSystem.get(); }

NeuralPlanner* Brain::getPlanner() { return pImpl->planner.get(); }
ConceptFormation* Brain::getConceptFormation() { return pImpl->conceptFormation.get(); }
AttentionalSelection* Brain::getAttention() { return pImpl->attention.get(); }

DevelopmentSystem* Brain::getDevelopmentSystem() { return pImpl->developmentSystem.get(); }
DevelopmentalStage Brain::getDevelopmentalStage() const { return pImpl->developmentalStage; }
void Brain::setDevelopmentalStage(DevelopmentalStage stage) { pImpl->developmentalStage = stage; }

Dopamine* Brain::getDopamine() { return pImpl->dopamine.get(); }
Curiosity* Brain::getCuriosity() { return pImpl->curiosity.get(); }
Novelty* Brain::getNovelty() { return pImpl->novelty.get(); }
PredictionError* Brain::getPredictionErrorSignal() { return pImpl->predictionError.get(); }

CheckpointManager* Brain::getCheckpointManager() { return pImpl->checkpointManager.get(); }

std::shared_ptr<const Config> Brain::getConfig() const { return pImpl->config; }
RandomGenerator* Brain::getRandomGenerator() { return pImpl->rng.get(); }

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage));
    
    for (const auto* region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

// Helper functions for forward declarations
void Brain::collectSensoryMotorNeurons() {
    for (auto* region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            } else if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

size_t Brain::getSensoryNeuronCount() const {
    return pImpl->sensoryNeurons.size();
}

size_t Brain::getMotorNeuronCount() const {
    return pImpl->motorNeurons.size();
}

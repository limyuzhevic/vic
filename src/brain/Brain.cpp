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
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    // ========== INTEGRATED SYSTEMS ==========
    std::unique_ptr<NeuralCore> neuralCore;
    std::unique_ptr<MemorySystem> memorySystem;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<CognitiveSystem> cognitiveSystem;
    std::unique_ptr<NeuromodulationSystem> neuromodulationSystem;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<PlasticitySystem> plasticitySystem;
    std::unique_ptr<CheckpointSystem> checkpointSystem;
    std::unique_ptr<Integration> integration;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000)
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Create integration manager
        integration = std::make_unique<Integration>(std::make_shared<ConfigurationManager>());
        
        // Get integration intervals from config
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
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
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Initialize integration layer
    if (!pImpl->integration->initializeAllSystems()) {
        NLM_LOG_ERROR("Failed to initialize integration layer");
        return false;
    }
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointSystem->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Delegate neural computation to NeuralCore
    pImpl->neuralCore->step(currentStep, currentTime);
    
    // Update memory systems
    if (pImpl->memorySystem) {
        pImpl->memorySystem->update(pImpl->timestep);
    }
    
    // Update prediction system
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->update(pImpl->timestep);
    }
    
    // Update cognitive systems
    if (pImpl->cognitiveSystem) {
        pImpl->cognitiveSystem->update(pImpl->timestep);
    }
    
    // Update neuromodulation
    if (pImpl->neuromodulationSystem) {
        pImpl->neuromodulationSystem->update(pImpl->timestep);
    }
    
    // Update plasticity
    if (pImpl->plasticitySystem) {
        pImpl->plasticitySystem->update(this, *pImpl->rng, pImpl->timestep);
    }
    
    // Update development
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
    }
    
    // Periodically update episodic memory
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10 && pImpl->memorySystem) {
        pImpl->stepsSinceLastEpisode = 0;
        // Store episode logic would be handled by MemorySystem
    }
    
    // Periodic structural plasticity
    if (currentStep % 100 == 0 && pImpl->plasticitySystem) {
        auto* sp = pImpl->plasticitySystem->getStructuralPlasticity();
        if (sp) {
            sp->update(this, *pImpl->rng);
        }
    }
    
    // Memory replay
    if (currentStep % pImpl->replayInterval == 0 && pImpl->memorySystem) {
        // Replay logic would be handled by MemorySystem
    }
    
    // Memory consolidation
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->memorySystem) {
        // Consolidation logic would be handled by MemorySystem
    }
    
    // Checkpoint management
    if (pImpl->checkpointSystem) {
        pImpl->checkpointSystem->update(currentStep, currentTime);
    }
    
    // Collect statistics
    pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Delegate to sensory input handling via neural core
    if (pImpl->neuralCore) {
        pImpl->neuralCore->receiveSensoryInput(input);
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Delegate to neural core
    if (pImpl->neuralCore) {
        pImpl->neuralCore->injectCurrent(neuron, current);
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Delegate to neural core
    if (pImpl->neuralCore) {
        pImpl->neuralCore->injectCurrentToNeurons(type, current);
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getSpikeSystem();
    }
    return nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getSpikeSystem();
    }
    return nullptr;
}

STDP* Brain::getSTDP() {
    if (pImpl->plasticitySystem) {
        return pImpl->plasticitySystem->getSTDP();
    }
    return nullptr;
}

Hebbian* Brain::getHebbian() {
    if (pImpl->plasticitySystem) {
        return pImpl->plasticitySystem->getHebbian();
    }
    return nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    if (pImpl->plasticitySystem) {
        return pImpl->plasticitySystem->getStructuralPlasticity();
    }
    return nullptr;
}

float Brain::getExcitationInhibitionRatio() const {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getExcitationInhibitionRatio();
    }
    return 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    if (pImpl->neuralCore && pImpl->neuralCore->getSpikeSystem()) {
        return pImpl->neuralCore->getSpikeSystem()->getPendingSpikeCount() + 
               pImpl->neuralCore->getSpikeSystem()->getPendingDelayedCount();
    }
    return 0;
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    return action;
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    if (pImpl->neuromodulationSystem) {
        pImpl->neuromodulationSystem->applySignal(signal);
    }
}

void Brain::updatePlasticity() {
    if (pImpl->plasticitySystem) {
        pImpl->plasticitySystem->update(this, *pImpl->rng, pImpl->timestep);
    }
}

void Brain::develop() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    
    // Delegate reset to neural core
    if (pImpl->neuralCore) {
        pImpl->neuralCore->reset();
    }
    
    // Reset memory systems
    if (pImpl->memorySystem) {
        pImpl->memorySystem->clear();
    }
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        if (pImpl->checkpointSystem) {
            return pImpl->checkpointSystem->save(filepath, this);
        }
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        if (pImpl->checkpointSystem) {
            return pImpl->checkpointSystem->load(filepath, this);
        }
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->addRegion(name);
    }
    return RegionId(0);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getRegion(id);
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getRegion(id);
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getRegionCount();
    }
    return 0;
}

std::vector<RegionId> Brain::getRegionIds() const {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getRegionIds();
    }
    return std::vector<RegionId>();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    static const std::vector<std::unique_ptr<NeuralRegion>> empty;
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getRegions();
    }
    return empty;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    if (pImpl->neuralCore) {
        pImpl->neuralCore->addInterRegionConnection(source, target, weight, delay);
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    if (pImpl->neuralCore) {
        pImpl->neuralCore->removeInterRegionConnection(source, target);
    }
}

size_t Brain::getTotalNeuronCount() const {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getTotalNeuronCount();
    }
    return 0;
}

size_t Brain::getTotalSynapseCount() const {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getTotalSynapseCount();
    }
    return 0;
}

size_t Brain::getActiveNeuronCount() const {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getActiveNeuronCount();
    }
    return 0;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->neuralCore) {
        return pImpl->neuralCore->getAverageFiringRate();
    }
    return 0.0f;
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    if (pImpl->memorySystem) {
        return pImpl->memorySystem->getWorkingMemory();
    }
    return nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    if (pImpl->memorySystem) {
        return pImpl->memorySystem->getEpisodicMemory();
    }
    return nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    if (pImpl->memorySystem) {
        return pImpl->memorySystem->getAssociativeMemory();
    }
    return nullptr;
}

PredictionSystem* Brain::getPredictionSystem() {
    if (pImpl->predictionSystem) {
        return pImpl->predictionSystem.get();
    }
    return nullptr;
}

NeuralPlanner* Brain::getPlanner() {
    if (pImpl->cognitiveSystem) {
        return pImpl->cognitiveSystem->getPlanner();
    }
    return nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    if (pImpl->cognitiveSystem) {
        return pImpl->cognitiveSystem->getConceptFormation();
    }
    return nullptr;
}

AttentionalSelection* Brain::getAttention() {
    if (pImpl->cognitiveSystem) {
        return pImpl->cognitiveSystem->getAttention();
    }
    return nullptr;
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    if (pImpl->developmentSystem) {
        return pImpl->developmentSystem->getDevelopmentalStage();
    }
    return DevelopmentalStage::Initial;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->setDevelopmentalStage(stage);
    }
}

Dopamine* Brain::getDopamine() {
    if (pImpl->neuromodulationSystem) {
        return pImpl->neuromodulationSystem->getDopamine();
    }
    return nullptr;
}

Curiosity* Brain::getCuriosity() {
    if (pImpl->neuromodulationSystem) {
        return pImpl->neuromodulationSystem->getCuriosity();
    }
    return nullptr;
}

Novelty* Brain::getNovelty() {
    if (pImpl->neuromodulationSystem) {
        return pImpl->neuromodulationSystem->getNovelty();
    }
    return nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    if (pImpl->neuromodulationSystem) {
        return pImpl->neuromodulationSystem->getPredictionError();
    }
    return nullptr;
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

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
}

} // namespace nlm

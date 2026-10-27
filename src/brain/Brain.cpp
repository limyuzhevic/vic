#include "Brain.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Brain orchestrator for complex step processing
    std::unique_ptr<BrainOrchestrator> orchestrator;
    
    // Object pool manager for efficient memory management
    std::unique_ptr<PoolManager> poolManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , orchestrator(nullptr)
        , poolManager(nullptr)
    {
        // Create orchestrator
        orchestrator = std::make_unique<BrainOrchestrator>(this);
        orchestrator->initialize();
        
        // Initialize pool manager for object reuse
        poolManager = std::make_unique<PoolManager>();
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
                    pImpl->orchestrator->addSensoryNeuron(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->orchestrator->addMotorNeuron(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                         ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->orchestrator->getRandomGenerator(), connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    if (pImpl->orchestrator->getWorkingMemory()) {
        pImpl->orchestrator->getWorkingMemory()->initialize(this);
        pImpl->orchestrator->getWorkingMemory()->setCapacity(neuronCount / 10);
    }
    
    // Initialize episodic memory
    if (pImpl->orchestrator->getEpisodicMemory()) {
        pImpl->orchestrator->getEpisodicMemory()->initialize(this);
        pImpl->orchestrator->getEpisodicMemory()->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    if (pImpl->orchestrator->getAssociativeMemory()) {
        pImpl->orchestrator->getAssociativeMemory()->initialize(this);
    }
    
    // Initialize cognition systems
    if (pImpl->orchestrator->getPlanner()) {
        pImpl->orchestrator->getPlanner()->initialize(this);
        pImpl->orchestrator->getPlanner()->setPlanningDepth(5);
    }
    
    if (pImpl->orchestrator->getConceptFormation()) {
        pImpl->orchestrator->getConceptFormation()->initialize(this);
    }
    
    if (pImpl->orchestrator->getAttention()) {
        pImpl->orchestrator->getAttention()->initialize(this);
        pImpl->orchestrator->getAttention()->setInhibitionStrength(0.5f);
        pImpl->orchestrator->getAttention()->setExcitationStrength(1.5f);
    }
    
    // Initialize neuromodulation
    if (pImpl->orchestrator->getNovelty()) {
        pImpl->orchestrator->getNovelty()->initialize(this);
    }
    if (pImpl->orchestrator->getCuriosity()) {
        pImpl->orchestrator->getCuriosity()->initialize(this);
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (pImpl->orchestrator->getCheckpointManager()) {
        pImpl->orchestrator->getCheckpointManager()->configure(checkpointDir, 10000, 5, true);
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->orchestrator->getSensoryNeurons().size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->orchestrator->getMotorNeurons().size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->orchestrator->getTimestep());
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Delegate to orchestrator for complex step processing
    pImpl->orchestrator->processStep(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->orchestrator->getSensoryNeurons().size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->orchestrator->getSensoryNeurons()[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->orchestrator->getWorkingMemory() && normalizedValue > 0.5f) {
            pImpl->orchestrator->getWorkingMemory()->storeToNeuron(pImpl->orchestrator->getSensoryNeurons()[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (const auto& region : pImpl->orchestrator->getRegions()) {
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
    for (const auto& region : pImpl->orchestrator->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->orchestrator->getSpikeSystem();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->orchestrator->getSpikeSystem();
}

STDP* Brain::getSTDP() {
    return pImpl->orchestrator->getSTDP();
}

Hebbian* Brain::getHebbian() {
    return pImpl->orchestrator->getHebbian();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->orchestrator->getStructuralPlasticity();
}

float Brain::getExcitationInhibitionRatio() const {
    return pImpl->orchestrator->getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->orchestrator->getTotalSpikeCount();
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->orchestrator->getPendingSpikeEventCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    const auto& motorNeurons = pImpl->orchestrator->getMotorNeurons();
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons) {
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
    // Delegate to orchestrator
    pImpl->orchestrator->applyNeuromodulation(signal);
}

void Brain::updatePlasticity() {
    // Delegate to orchestrator
    pImpl->orchestrator->updatePlasticity();
}

void Brain::develop() {
    // Delegate to orchestrator
    pImpl->orchestrator->develop();
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->orchestrator->resetState();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    if (!pImpl->orchestrator->getCheckpointManager()) {
        NLM_LOG_ERROR("Checkpoint manager not available");
        return false;
    }
    
    // The orchestrator handles checkpointing internally
    // For now, delegate to checkpointer
    return pImpl->orchestrator->getCheckpointManager()->saveImmediately("checkpoint");
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    if (!pImpl->orchestrator->getCheckpointManager()) {
        NLM_LOG_ERROR("Checkpoint manager not available");
        return false;
    }
    
    // Delegate to checkpoint manager
    return pImpl->orchestrator->getCheckpointManager()->load(filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->orchestrator->getNextRegionId());
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->orchestrator->addRegion(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->orchestrator->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->orchestrator->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->orchestrator->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->orchestrator->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->orchestrator->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                      float weight, Delay delay) {
    pImpl->orchestrator->addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->orchestrator->removeInterRegionConnection(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->orchestrator->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->orchestrator->getTotalSynapseCount();
}
size_t Brain::getActiveNeuronCount() const {
    return pImpl->orchestrator->getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->orchestrator->getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->orchestrator->getAverageFiringRate();
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->orchestrator->getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->orchestrator->getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->orchestrator->getAssociativeMemory();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->orchestrator->getPredictionSystem();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->orchestrator->getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->orchestrator->getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->orchestrator->getAttention();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->orchestrator->getDevelopmentSystem();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->orchestrator->getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->orchestrator->setDevelopmentalStage(stage);
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    return pImpl->orchestrator->getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->orchestrator->getCuriosity();
}

Novelty* Brain::getNovelty() {
    return pImpl->orchestrator->getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->orchestrator->getPredictionErrorSignal();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->orchestrator->getConfig();
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->orchestrator->getRandomGenerator();
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
    
    // Memory system status
    if (getWorkingMemory()) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(getWorkingMemory()->getActiveTraces()));
    }
    if (getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(getEpisodicMemory()->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (getDopamine()) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(getDopamine()->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(getDevelopmentalStage())));
    
    for (const auto& region : pImpl->orchestrator->getRegions()) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

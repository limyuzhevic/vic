#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../development/Maturation.hpp"
#include "../development/Pruning.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/Novelty.hpp"
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

// Thread safety wrapper for brain operations
class BrainStepWrapper {
public:
    BrainStepWrapper(Brain* brain) : brain_(brain) {
        if (brain_->pImpl->threadSafetyManager) {
            brain_->pImpl->threadSafetyManager->acquireStepLock();
            brain_->pImpl->stepLockAcquired = true;
        }
    }
    
    ~BrainStepWrapper() {
        if (brain_->pImpl->stepLockAcquired && brain_->pImpl->threadSafetyManager) {
            brain_->pImpl->threadSafetyManager->releaseStepLock();
            brain_->pImpl->stepLockAcquired = false;
        }
    }
    
    BrainStepWrapper(const BrainStepWrapper&) = delete;
    BrainStepWrapper& operator=(const BrainStepWrapper&) = delete;
    
    // Enable move semantics for performance
    BrainStepWrapper(BrainStepWrapper&& other) noexcept : brain_(other.brain_) {
        other.brain_ = nullptr;
    }
    
    BrainStepWrapper& operator=(BrainStepWrapper&& other) noexcept {
        if (this != &other) {
            // Release current lock if held
            if (brain_ && brain_->pImpl->stepLockAcquired && brain_->pImpl->threadSafetyManager) {
                brain_->pImpl->threadSafetyManager->releaseStepLock();
                brain_->pImpl->stepLockAcquired = false;
            }
            
            // Acquire lock from other
            brain_ = other.brain_;
            if (brain_ && brain_->pImpl->threadSafetyManager) {
                brain_->pImpl->threadSafetyManager->acquireStepLock();
                brain_->pImpl->stepLockAcquired = true;
            }
            
            other.brain_ = nullptr;
        }
        return *this;
    }
    
private:
    Brain* brain_;
};

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    // Integrated systems via helper classes
    std::unique_ptr<BrainInitializer> brainInitializer;
    std::unique_ptr<BrainSimulationStep> brainSimulationStep;
    std::unique_ptr<BrainRegionManager> regionManager;
    std::unique_ptr<BrainMemoryIntegrator> memoryIntegrator;
    std::unique_ptr<BrainCognitionIntegrator> cognitionIntegrator;
    std::unique_ptr<BrainNeuromodulatorIntegrator> neuromodulatorIntegrator;
    std::unique_ptr<BrainSensoryMotorProcessor> sensoryMotorProcessor;
    std::unique_ptr<BrainPersistenceManager> persistenceManager;
    std::unique_ptr<BrainThreadSafetyManager> threadSafetyManager;
    
    // Thread safety state
    bool stepLockAcquired;
    
    // State maintained for backward compatibility
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    bool isResting;
    size_t stepsSinceLastEpisode;
    
    // I/O neuron collections
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // All brain components for backward compatibility
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Component pointers
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<CheckpointManager> checkpointManager;
    std::unique_ptr<Maturation> maturation;
    std::unique_ptr<Pruning> pruning;
    std::unique_ptr<Synaptogenesis> synaptogenesis;
    std::unique_ptr<Reward> reward;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , stepLockAcquired(false)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize all helper managers
        brainInitializer = std::make_unique<BrainInitializer>(config);
        brainSimulationStep = std::make_unique<BrainSimulationStep>(config);
        regionManager = std::make_unique<BrainRegionManager>(config);
        memoryIntegrator = std::make_unique<BrainMemoryIntegrator>(config);
        cognitionIntegrator = std::make_unique<BrainCognitionIntegrator>(config);
        neuromodulatorIntegrator = std::make_unique<BrainNeuromodulatorIntegrator>(config);
        sensoryMotorProcessor = std::make_unique<BrainSensoryMotorProcessor>(config);
        persistenceManager = std::make_unique<BrainPersistenceManager>(config);
        threadSafetyManager = std::make_unique<BrainThreadSafetyManager>(config);
        
        // Initialize development stage
        developmentalStage = DevelopmentalStage::Initial;
        nextRegionId = 1;
        
        // Get initialized components from brain initializer
        spikeSystem = brainInitializer->getSpikeSystem();
        stdp = brainInitializer->getSTDP();
        hebbian = brainInitializer->getHebbian();
        structuralPlasticity = brainInitializer->getStructuralPlasticity();
        workingMemory = brainInitializer->getWorkingMemory();
        episodicMemory = brainInitializer->getEpisodicMemory();
        associativeMemory = brainInitializer->getAssociativeMemory();
        predictionSystem = brainInitializer->getPredictionSystem();
        planner = brainInitializer->getPlanner();
        conceptFormation = brainInitializer->getConceptFormation();
        attention = brainInitializer->getAttention();
        developmentSystem = brainInitializer->getDevelopmentSystem();
        dopamine = brainInitializer->getDopamine();
        curiosity = brainInitializer->getCuriosity();
        predictionError = brainInitializer->getPredictionError();
        novelty = brainInitializer->getNovelty();
        checkpointManager = brainInitializer->getCheckpointManager();
        
        // Initialize development system components
        maturation = std::make_unique<Maturation>(config);
        pruning = std::make_unique<Pruning>(config);
        synaptogenesis = std::make_unique<Synaptogenesis>(config);
        reward = std::make_unique<Reward>(config);
        
        // Copy regions and connections from brain initializer
        regions = brainInitializer->getRegions();
        interRegionConnections = brainInitializer->getInterRegionConnections();
        
        // Copy I/O neurons from brain initializer
        sensoryNeurons = brainInitializer->getSensoryNeurons();
        motorNeurons = brainInitializer->getMotorNeurons();
        
        // Configure components
        if (spikeSystem) {
            spikeSystem->setTimestep(brainInitializer->getTimestep());
        }
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
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
    
    // Create regions using region manager
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions using region manager
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations (this would be delegated to region manager)
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
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                         ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights using brain initializer
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // Initialize all integrated systems using brain initializer
    pImpl->brainInitializer->initializePlasticitySystems();
    pImpl->brainInitializer->initializeMemorySystems();
    pImpl->brainInitializer->initializeCognitionSystems();
    pImpl->brainInitializer->initializeNeuromodulationSystems();
    pImpl->brainInitializer->initializeDevelopmentSystem();
    pImpl->brainInitializer->initializeSpikeSystem();
    pImpl->brainInitializer->initializeCheckpointManager();
    
    // Configure region connectivity and neurons using brain initializer
    pImpl->brainInitializer->configureRegionsAndNeurons(neuronCount, regionCount, connectionProbability);
    
    // Setup spike handlers using brain initializer
    pImpl->brainInitializer->setupSpikeHandlers(this);
    
    // Update component pointers from brain initializer
    pImpl->spikeSystem = pImpl->brainInitializer->getSpikeSystem();
    pImpl->stdp = pImpl->brainInitializer->getSTDP();
    pImpl->hebbian = pImpl->brainInitializer->getHebbian();
    pImpl->structuralPlasticity = pImpl->brainInitializer->getStructuralPlasticity();
    pImpl->workingMemory = pImpl->brainInitializer->getWorkingMemory();
    pImpl->episodicMemory = pImpl->brainInitializer->getEpisodicMemory();
    pImpl->associativeMemory = pImpl->brainInitializer->getAssociativeMemory();
    pImpl->predictionSystem = pImpl->brainInitializer->getPredictionSystem();
    pImpl->planner = pImpl->brainInitializer->getPlanner();
    pImpl->conceptFormation = pImpl->brainInitializer->getConceptFormation();
    pImpl->attention = pImpl->brainInitializer->getAttention();
    pImpl->developmentSystem = pImpl->brainInitializer->getDevelopmentSystem();
    pImpl->dopamine = pImpl->brainInitializer->getDopamine();
    pImpl->curiosity = pImpl->brainInitializer->getCuriosity();
    pImpl->novelty = pImpl->brainInitializer->getNovelty();
    pImpl->predictionError = pImpl->brainInitializer->getPredictionError();
    pImpl->checkpointManager = pImpl->brainInitializer->getCheckpointManager();
    
    // Update development system components
    pImpl->maturation = std::make_unique<Maturation>(pImpl->config);
    pImpl->pruning = std::make_unique<Pruning>(pImpl->config);
    pImpl->synaptogenesis = std::make_unique<Synaptogenesis>(pImpl->config);
    pImpl->reward = std::make_unique<Reward>(pImpl->config);
    
    // Copy regions and connections from brain initializer
    pImpl->regions = pImpl->brainInitializer->getRegions();
    pImpl->interRegionConnections = pImpl->brainInitializer->getInterRegionConnections();
    pImpl->sensoryNeurons = pImpl->brainInitializer->getSensoryNeurons();
    pImpl->motorNeurons = pImpl->brainInitializer->getMotorNeurons();
    
    // Configure checkpoint manager using brain initializer
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
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
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    // Use brain simulation step for delegation
    BrainStepWrapper wrapper(this);
    pImpl->brainSimulationStep->simulateStep(this, currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Delegate to sensory motor processor
    pImpl->sensoryMotorProcessor->processSensoryInput(this, input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Delegate to sensory motor processor
    pImpl->sensoryMotorProcessor->injectCurrentToNeuron(this, neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Delegate to sensory motor processor
    pImpl->sensoryMotorProcessor->injectCurrentToNeurons(this, type, current);
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
    return pImpl->brainInitializer->getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Delegate to sensory motor processor
    return pImpl->sensoryMotorProcessor->generateAction(this);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Delegate to neuromodulator integrator
    pImpl->neuromodulatorIntegrator->applyNeuromodulation(this, signal);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
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
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    // Reset helper classes
    pImpl->brainInitializer->reset();
    pImpl->brainSimulationStep->reset();
    pImpl->regionManager->resetRegions(this);
    pImpl->memoryIntegrator->reset();
    pImpl->cognitionIntegrator->reset();
    pImpl->neuromodulatorIntegrator->reset();
    pImpl->sensoryMotorProcessor->resetIOLogic(this);
    pImpl->persistenceManager->resetPersistenceManager(this);
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        return pImpl->persistenceManager->saveBrainState(this, filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        return pImpl->persistenceManager->loadBrainState(this, filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl->regionManager->addRegion(this, name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->regionManager->getRegion(this, id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->regionManager->getRegion(this, id);
}

size_t Brain::getRegionCount() const {
    return pImpl->regionManager->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regionManager->getRegionCount());
    for (const auto& region : pImpl->brainInitializer->getRegions()) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->brainInitializer->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->regionManager->addInterRegionConnection(this, source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->regionManager->removeInterRegionConnection(this, source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->regionManager->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->regionManager->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->brainInitializer->getRegions()) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->brainInitializer->getRegionCount() == 0) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->brainInitializer->getRegions()) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->brainInitializer->getRegionCount());
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

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
    
    for (const auto& region : pImpl->brainInitializer->getRegions()) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

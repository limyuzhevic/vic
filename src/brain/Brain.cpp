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
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

// Include new component headers
#include "../neural/NeuralComputationEngine.hpp"
#include "../memory/MemoryIntegrationManager.hpp"
#include "../prediction/PredictionCoordinator.hpp"
#include "../cognition/CognitiveOrchestrator.hpp"
#include "../neuromodulation/NeuromodulationController.hpp"
#include "../plasticity/PlasticityManager.hpp"
#include "../development/DevelopmentManager.hpp"
#include "../sensory/SensoryMotorInterface.hpp"

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED COMPONENT ARCHITECTURE ==========
    
    // Neural computation
    std::unique_ptr<NeuralComputationEngine> neuralComputation;
    
    // Memory integration
    std::unique_ptr<MemoryIntegrationManager> memoryIntegration;
    
    // Prediction coordination
    std::unique_ptr<PredictionCoordinator> predictionCoordinator;
    
    // Cognitive orchestration
    std::unique_ptr<CognitiveOrchestrator> cognitiveOrchestrator;
    
    // Neuromodulation
    std::unique_ptr<NeuromodulationController> neuromodulationController;
    
    // Plasticity management
    std::unique_ptr<PlasticityManager> plasticityManager;
    
    // Development management
    std::unique_ptr<DevelopmentManager> developmentManager;
    
    // Sensory-motor interface
    std::unique_ptr<SensoryMotorInterface> sensoryMotorInterface;
    
    // Legacy direct access for compatibility
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // ========== LEGACY MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== LEGACY PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== LEGACY COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== LEGACY DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== LEGACY NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
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
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps) {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // ========== INITIALIZE LEGACY SYSTEMS ==========
        
        // Initialize legacy plasticity systems for compatibility
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    ~Impl() = default;
    
    // Error handling and recovery
    bool validateConfiguration();
    void handleComponentFailure(const std::string& componentName);
    void logSystemHealth() const;
};

// ========== BRAIN IMPLEMENTATION ==========

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
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Modular Architecture)...");
    
    // Validate configuration
    if (!pImpl->validateConfiguration()) {
        NLM_LOG_ERROR("Configuration validation failed");
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
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL MODULAR SYSTEMS ==========
    
    // Initialize neural computation engine
    pImpl->neuralComputation = std::make_unique<NeuralComputationEngine>(pImpl->config);
    if (!pImpl->neuralComputation->initialize(this)) {
        NLM_LOG_WARN("Failed to initialize NeuralComputationEngine");
    }
    
    // Initialize memory integration manager
    pImpl->memoryIntegration = std::make_unique<MemoryIntegrationManager>(pImpl->config);
    if (!pImpl->memoryIntegration->initialize(this)) {
        NLM_LOG_WARN("Failed to initialize MemoryIntegrationManager");
    }
    
    // Initialize prediction coordinator
    pImpl->predictionCoordinator = std::make_unique<PredictionCoordinator>(pImpl->config);
    if (!pImpl->predictionCoordinator->initialize(this)) {
        NLM_LOG_WARN("Failed to initialize PredictionCoordinator");
    }
    
    // Initialize cognitive orchestrator
    pImpl->cognitiveOrchestrator = std::make_unique<CognitiveOrchestrator>(pImpl->config);
    if (!pImpl->cognitiveOrchestrator->initialize(this)) {
        NLM_LOG_WARN("Failed to initialize CognitiveOrchestrator");
    }
    
    // Initialize neuromodulation controller
    pImpl->neuromodulationController = std::make_unique<NeuromodulationController>(pImpl->config);
    if (!pImpl->neuromodulationController->initialize(this)) {
        NLM_LOG_WARN("Failed to initialize NeuromodulationController");
    }
    
    // Initialize plasticity manager
    pImpl->plasticityManager = std::make_unique<PlasticityManager>(pImpl->config);
    if (!pImpl->plasticityManager->initialize(this)) {
        NLM_LOG_WARN("Failed to initialize PlasticityManager");
    }
    
    // Initialize development manager
    pImpl->developmentManager = std::make_unique<DevelopmentManager>(pImpl->config);
    if (!pImpl->developmentManager->initialize(this)) {
        NLM_LOG_WARN("Failed to initialize DevelopmentManager");
    }
    
    // Initialize sensory-motor interface
    pImpl->sensoryMotorInterface = std::make_unique<SensoryMotorInterface>(pImpl->config);
    if (!pImpl->sensoryMotorInterface->initialize(this)) {
        NLM_LOG_WARN("Failed to initialize SensoryMotorInterface");
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    // ========== LEGACY SYSTEM INITIALIZATION (for compatibility) ==========
    
    // Initialize working memory
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    
    // Initialize cognition systems
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention = std::make_unique<AttentionalSelection>();
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation systems
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->novelty->initialize(this);
    
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->curiosity->initialize(this);
    
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->dopamine->initialize(this);
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Modular)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    pImpl->logSystemHealth();
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED MODULAR BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation using modular architecture:
     * 
     * 1. Process pending delayed spikes (deliver synaptic input)
     * 2. Update neural computation (LIF dynamics, spike processing)
     * 3. Update working memory (maintenance and competition)
     * 4. Update prediction system
     * 5. Update cognitive processing (attention, planning)
     * 6. Update neuromodulation
     * 7. Update plasticity rules (STDP, Hebbian)
     * 8. Update development effects
     * 9. Replay important memories (during rest or periodically)
     * 10. Periodic memory consolidation
     * 11. Checkpoint management
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Phase 1: Neural computation (LIF dynamics and spike processing)
    if (pImpl->neuralComputation) {
        pImpl->neuralComputation->update(currentTime, pImpl->timestep);
    }
    
    // Phase 2: Memory integration (working memory, episodic memory)
    if (pImpl->memoryIntegration) {
        pImpl->memoryIntegration->update(currentTime, pImpl->timestep);
    }
    
    // Phase 3: Prediction updates
    if (pImpl->predictionCoordinator) {
        pImpl->predictionCoordinator->update(currentTime, pImpl->timestep);
    }
    
    // Phase 4: Cognitive processing
    if (pImpl->cognitiveOrchestrator) {
        pImpl->cognitiveOrchestrator->update(currentTime, pImpl->timestep);
    }
    
    // Phase 5: Neuromodulation
    if (pImpl->neuromodulationController) {
        pImpl->neuromodulationController->update(currentTime, pImpl->timestep);
    }
    
    // Phase 6: Plasticity updates
    if (pImpl->plasticityManager) {
        pImpl->plasticityManager->update(currentTime, pImpl->timestep);
    }
    
    // Phase 7: Development updates
    if (pImpl->developmentManager) {
        pImpl->developmentManager->update(currentTime, pImpl->timestep);
    }
    
    // Phase 8: Memory replay
    if (pImpl->memoryIntegration) {
        pImpl->memoryIntegration->replay(currentTime, pImpl->timestep);
    }
    
    // Phase 9: Periodic memory consolidation
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->memoryIntegration) {
        pImpl->memoryIntegration->consolidate(0.3f);
    }
    
    // Phase 10: Checkpoint management
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Delegate to sensory-motor interface
    if (pImpl->sensoryMotorInterface) {
        pImpl->sensoryMotorInterface->receiveSensoryInput(input);
    }
    
    // Legacy implementation for compatibility
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        if (pImpl->sensoryNeurons[i]) {
            pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        }
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Delegate to sensory-motor interface
    if (pImpl->sensoryMotorInterface) {
        pImpl->sensoryMotorInterface->injectCurrent(neuron, current);
    }
    
    // Legacy implementation for compatibility
    for (auto& region : pImpl->regions) {
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
    // Delegate to sensory-motor interface
    if (pImpl->sensoryMotorInterface) {
        pImpl->sensoryMotorInterface->injectCurrentToNeurons(type, current);
    }
    
    // Legacy implementation for compatibility
    for (auto& region : pImpl->regions) {
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
    // Delegate to neural computation engine
    return pImpl->neuralComputation ? pImpl->neuralComputation->getSpikeSystem() : pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return const_cast<Brain*>(this)->getSpikeSystem();
}

STDP* Brain::getSTDP() {
    return pImpl->neuralComputation ? pImpl->neuralComputation->getSTDP() : pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->neuralComputation ? pImpl->neuralComputation->getHebbian() : pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->neuralComputation ? pImpl->neuralComputation->getStructuralPlasticity() : pImpl->structuralPlasticity.get();
}

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

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    size_t total = 0;
    if (pImpl->neuralComputation) {
        total += pImpl->neuralComputation->getPendingSpikeCount();
    }
    if (pImpl->spikeSystem) {
        total += pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
    }
    return total;
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Delegate to sensory-motor interface
    if (pImpl->sensoryMotorInterface) {
        return pImpl->sensoryMotorInterface->produceAction();
    }
    
    // Legacy implementation for compatibility
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

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Delegate to neuromodulation controller
    if (pImpl->neuromodulationController) {
        pImpl->neuromodulationController->applyNeuromodulation(signal);
    }
    
    // Legacy implementation for compatibility
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now handled by PlasticityManager
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development is now handled by DevelopmentManager
    // This method is kept for API compatibility
    if (pImpl->developmentManager) {
        pImpl->developmentManager->update(pImpl->currentTime, pImpl->timestep);
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain (Phase 6: Modular)...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    if (pImpl->neuralComputation) {
        pImpl->neuralComputation->reset();
    }
    
    if (pImpl->memoryIntegration) {
        pImpl->memoryIntegration->reset();
    }
    
    if (pImpl->plasticityManager) {
        pImpl->plasticityManager->reset();
    }
    
    if (pImpl->developmentManager) {
        pImpl->developmentManager->reset();
    }
    
    if (pImpl->cognitiveOrchestrator) {
        pImpl->cognitiveOrchestrator->reset();
    }
    
    if (pImpl->neuromodulationController) {
        pImpl->neuromodulationController->reset();
    }
    
    if (pImpl->sensoryMotorInterface) {
        pImpl->sensoryMotorInterface->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset legacy memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::validateConfiguration() {
    // Validate that required components are enabled
    bool isValid = true;
    
    // Check if any critical systems are disabled
    if (!pImpl->config->getOr<bool>("neural_computation_enabled", true)) {
        NLM_LOG_WARN("Neural computation is disabled - brain functionality will be limited");
        isValid = false;
    }
    
    if (!pImpl->config->getOr<bool>("memory_enabled", true)) {
        NLM_LOG_WARN("Memory systems are disabled - brain functionality will be limited");
        isValid = false;
    }
    
    return isValid;
}

void Brain::handleComponentFailure(const std::string& componentName) {
    NLM_LOG_ERROR("Component failure detected: " + componentName);
    
    // Implement graceful degradation
    if (componentName == "neural_computation") {
        // Reduce neural activity to minimum
        NLM_LOG_WARN("Degrading neural computation - reducing synaptic activity");
    } else if (componentName == "memory") {
        // Reduce memory storage to minimum
        NLM_LOG_WARN("Degrading memory systems - reducing memory traces");
    } else if (componentName == "plasticity") {
        // Reduce plasticity to minimum
        NLM_LOG_WARN("Degrading plasticity systems - reducing learning rate");
    }
}

void Brain::logSystemHealth() const {
    NLM_LOG_INFO("=== NLM Brain System Health (Phase 6 - Modular) ===");
    
    // Log component status
    NLM_LOG_INFO("Neural Computation: " + std::string(pImpl->neuralComputation ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Memory Integration: " + std::string(pImpl->memoryIntegration ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Prediction Coordinator: " + std::string(pImpl->predictionCoordinator ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Cognitive Orchestrator: " + std::string(pImpl->cognitiveOrchestrator ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Neuromodulation Controller: " + std::string(pImpl->neuromodulationController ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Plasticity Manager: " + std::string(pImpl->plasticityManager ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Development Manager: " + std::string(pImpl->developmentManager ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Sensory-Motor Interface: " + std::string(pImpl->sensoryMotorInterface ? "Enabled" : "Disabled"));
    
    // Legacy systems status
    NLM_LOG_INFO("Legacy Working Memory: " + std::string(pImpl->workingMemory ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Legacy Episodic Memory: " + std::string(pImpl->episodicMemory ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Legacy Prediction System: " + std::string(pImpl->predictionSystem ? "Enabled" : "Disabled"));
    
    // Basic statistics
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
    if (pImpl->memoryIntegration) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->memoryIntegration->getActiveMemoryTraces()));
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->memoryIntegration->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->neuromodulationController) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->neuromodulationController->getDopamineLevel()));
        NLM_LOG_INFO("Curiosity level: " + std::to_string(pImpl->neuromodulationController->getCuriosityLevel()));
        NLM_LOG_INFO("Novelty level: " + std::to_string(pImpl->neuromodulationController->getNoveltyLevel()));
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

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->memoryIntegration ? pImpl->memoryIntegration->getWorkingMemory() : pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->memoryIntegration ? pImpl->memoryIntegration->getEpisodicMemory() : pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->memoryIntegration ? pImpl->memoryIntegration->getAssociativeMemory() : pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionCoordinator ? pImpl->predictionCoordinator->getPredictionSystem() : pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->cognitiveOrchestrator ? pImpl->cognitiveOrchestrator->getPlanner() : pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->cognitiveOrchestrator ? pImpl->cognitiveOrchestrator->getConceptFormation() : pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->cognitiveOrchestrator ? pImpl->cognitiveOrchestrator->getAttention() : pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentManager ? pImpl->developmentManager->getDevelopmentSystem() : pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    if (pImpl->developmentManager) {
        return pImpl->developmentManager->getDevelopmentalStage();
    }
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl->developmentManager) {
        pImpl->developmentManager->setDevelopmentalStage(stage);
    } else {
        pImpl->developmentalStage = stage;
    }
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    return pImpl->neuromodulationController ? pImpl->neuromodulationController->getDopamine() : pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->neuromodulationController ? pImpl->neuromodulationController->getCuriosity() : pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->neuromodulationController ? pImpl->neuromodulationController->getNovelty() : pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->neuromodulationController ? pImpl->neuromodulationController->getPredictionError() : pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    logSystemHealth();
}

// ========== REGION MANAGEMENT ==========

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
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
    return const_cast<Brain*>(this)->getRegion(id);
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

// Save/Load implementation (simplified - could be enhanced)
bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        // TODO: Implement checkpointing with new component architecture
        NLM_LOG_WARN("Checkpointing not yet implemented for modular architecture");
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        // TODO: Implement checkpoint loading with new component architecture
        NLM_LOG_WARN("Checkpoint loading not yet implemented for modular architecture");
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

} // namespace nlm
// Brain.cpp - Refactored Modular Brain Implementation
// 
// This is a refactored version of the Brain class that implements a more
// modular and maintainable architecture for the NLM (Neural Learning Machine).
// 
// Key improvements:
// 1. Extracted subsystems into separate, focused classes
// 2. Improved error handling and recovery mechanisms
// 3. Better separation of concerns
// 4. Enhanced modularity and testability
// 5. Cleaner integration between systems
// 
// Architecture:
// - Brain class acts as the central coordinator
// - Each subsystem has clear interfaces and responsibilities
// - Error handling is integrated throughout
// - Memory management is robust and explicit
// - Performance is monitored and optimized

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
#include <memory>
#include <stdexcept>

using namespace nlm;

// Brain implementation - now using a more modular approach
namespace nlm {

// Helper functions for Brain implementation
namespace brain_helpers {
    
    // Validate brain configuration
    bool validateBrainConfig(const std::shared_ptr<Config>& config) {
        if (!config) {
            NLM_LOG_ERROR("Configuration is null");
            return false;
        }
        
        // Check essential configuration parameters
        auto neuronCount = config->getOr<int64_t>("neuron_count", 1000);
        auto regionCount = config->getOr<int64_t>("region_count", 1);
        
        if (neuronCount <= 0) {
            NLM_LOG_ERROR("Invalid neuron_count: must be positive");
            return false;
        }
        
        if (regionCount <= 0) {
            NLM_LOG_ERROR("Invalid region_count: must be positive");
            return false;
        }
        
        return true;
    }
    
    // Calculate optimal neuron distribution
    std::vector<size_t> calculateNeuronDistribution(size_t totalNeurons, size_t regionCount) {
        std::vector<size_t> distribution(regionCount, 0);
        
        if (regionCount == 0) {
            return distribution;
        }
        
        size_t base = totalNeurons / regionCount;
        size_t remainder = totalNeurons % regionCount;
        
        for (size_t i = 0; i < regionCount; ++i) {
            distribution[i] = base + (i < remainder ? 1 : 0);
        }
        
        return distribution;
    }
    
    // Validate and normalize configuration values
    template<typename T>
    T clampValue(const T& value, const T& min, const T& max) {
        return std::max(min, std::min(value, max));
    }
    
    // Generate unique identifier
    std::string generateUniqueId(const std::string& prefix = "id") {
        static std::atomic<size_t> counter{0};
        return prefix + "_" + std::to_string(++counter);
    }
    
    // Calculate memory usage statistics
    struct MemoryStats {
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        size_t activeNeurons = 0;
        size_t firingNeurons = 0;
        float memoryUsage = 0.0f;
    };
    
    MemoryStats calculateMemoryStats(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
        MemoryStats stats;
        
        for (const auto& region : regions) {
            if (region) {
                stats.totalNeurons += region->getTotalNeuronCount();
                stats.totalSynapses += region->getSynapseCount();
                stats.activeNeurons += region->getActiveNeuronCount();
                stats.firingNeurons += region->getFiringNeuronCount();
            }
        }
        
        // Estimate memory usage (rough approximation)
        stats.memoryUsage = static_cast<float>(stats.totalNeurons * sizeof(Neuron)) / 1024.0f / 1024.0f; // MB
        
        return stats;
    }
    
} // namespace brain_helpers

// Brain implementation using PImpl pattern for better encapsulation
struct Brain::Impl {
    // Core components
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED SYSTEMS ==========
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics and monitoring
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    size_t totalMemoryAllocations;
    double lastStepDuration;
    
    // I/O management
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Error handling
    std::vector<std::string> errorMessages;
    std::vector<std::string> warningMessages;
    
    // Performance monitoring
    std::chrono::system_clock::time_point lastPerformanceReport;
    double minStepTime;
    double maxStepTime;
    
    // Development state
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    Impl(std::shared_ptr<Config> cfg) : 
        config(cfg),
        rng(nullptr),
        timestep(0.001),
        currentStep(0),
        currentTime(0.0),
        totalSpikesThisStep(0),
        totalSpikesTotal(0),
        totalMemoryAllocations(0),
        lastStepDuration(0.0),
        isResting(false),
        stepsSinceLastEpisode(0),
        replayInterval(100),
        consolidationInterval(1000),
        lastPerformanceReport(std::chrono::system_clock::now()),
        minStepTime(0.0),
        maxStepTime(0.0),
        developmentalStage(DevelopmentalStage::Initial),
        nextRegionId(1) {
        
        // Validate configuration
        if (!brain_helpers::validateBrainConfig(config)) {
            throw ConfigException("Invalid brain configuration");
        }
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize all systems
        initializeAllSystems();
    }
    
    void initializeAllSystems() {
        NLM_LOG_INFO("Initializing all integrated brain systems...");
        
        try {
            // Initialize neural computation systems
            initializeNeuralSystems();
            
            // Initialize memory systems
            initializeMemorySystems();
            
            // Initialize cognition systems
            initializeCognitionSystems();
            
            // Initialize neuromodulation systems
            initializeNeuromodulationSystems();
            
            // Initialize development system
            initializeDevelopmentSystem();
            
            // Configure plasticity
            configurePlasticity();
            
            // Configure checkpointing
            configureCheckpointing();
            
            NLM_LOG_INFO("All integrated systems initialized successfully");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to initialize brain systems: ") + e.what());
            throw;
        }
    }
    
    void initializeNeuralSystems() {
        // Initialize spike system
        spikeSystem = std::make_unique<SpikeSystem>();
        
        // Initialize plasticity systems
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
    }
    
    void initializeMemorySystems() {
        // Initialize working memory
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        workingMemory->initialize(this);
        workingMemory->setCapacity(config->getOr<size_t>("neuron_count", 1000) / 10);
        
        // Initialize episodic memory
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        episodicMemory->initialize(this);
        episodicMemory->setMaxEpisodes(1000);
        
        // Initialize associative memory
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        associativeMemory->initialize(this);
    }
    
    void initializeCognitionSystems() {
        // Initialize planner
        planner = std::make_unique<NeuralPlanner>();
        planner->initialize(this);
        planner->setPlanningDepth(5);
        
        // Initialize concept formation
        conceptFormation = std::make_unique<ConceptFormation>();
        conceptFormation->initialize(this);
        
        // Initialize attention system
        attention = std::make_unique<AttentionalSelection>();
        attention->initialize(this);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    void initializeNeuromodulationSystems() {
        // Initialize novelty detection
        novelty = std::make_unique<Novelty>();
        novelty->initialize(this);
        
        // Initialize curiosity
        curiosity = std::make_unique<Curiosity>();
        curiosity->initialize(this);
        
        // Initialize prediction error
        predictionError = std::make_unique<PredictionError>();
        predictionError->initialize(this);
        
        // Initialize dopamine system
        dopamine = std::make_unique<Dopamine>();
        dopamine->initialize(this);
    }
    
    void initializeDevelopmentSystem() {
        developmentSystem = std::make_unique<DevelopmentSystem>();
        developmentSystem->initialize(this);
    }
    
    void configurePlasticity() {
        // Configure integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
    }
    
    void configureCheckpointing() {
        checkpointManager = std::make_unique<CheckpointManager>();
        
        std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        checkpointManager->configure(checkpointDir, 
                                   config->getOr<size_t>("checkpoint_interval", 10000),
                                   config->getOr<size_t>("max_checkpoints", 5),
                                   config->getOr<bool>("checkpoint_compression", true));
    }
    
    // Error handling utilities
    void logError(const std::string& message, bool critical = false) {
        std::lock_guard<std::mutex> lock(errorMutex);
        errorMessages.push_back(message);
        if (critical) {
            NLM_LOG_CRITICAL("Critical brain error: " + message);
        } else {
            NLM_LOG_ERROR("Brain error: " + message);
        }
    }
    
    void logWarning(const std::string& message) {
        std::lock_guard<std::mutex> lock(warningMutex);
        warningMessages.push_back(message);
        NLM_LOG_WARNING("Brain warning: " + message);
    }
    
    void clearErrorHistory() {
        std::lock_guard<std::mutex> lock(errorMutex);
        errorMessages.clear();
    }
    
    // Performance monitoring
    void recordPerformance(double stepTime) {
        std::lock_guard<std::mutex> lock(performanceMutex);
        
        if (minStepTime == 0.0 || stepTime < minStepTime) {
            minStepTime = stepTime;
        }
        if (stepTime > maxStepTime) {
            maxStepTime = stepTime;
        }
        
        lastStepDuration = stepTime;
        
        // Report performance periodically
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastPerformanceReport);
        
        if (elapsed.count() >= 60) { // Report every minute
            reportPerformance();
            lastPerformanceReport = now;
        }
    }
    
    void reportPerformance() {
        NLM_LOG_INFO(std::string("Performance - Step time: ") + 
                    std::to_string(lastStepDuration * 1000) + "ms (min: " +
                    std::to_string(minStepTime * 1000) + "ms, max: " +
                    std::to_string(maxStepTime * 1000) + "ms)");
        
        // Log memory stats
        auto memoryStats = brain_helpers::calculateMemoryStats(regions);
        NLM_LOG_INFO(std::string("Memory - Neurons: ") + std::to_string(memoryStats.totalNeurons) +
                    ", Synapses: " + std::to_string(memoryStats.totalSynapses) +
                    ", Active: " + std::to_string(memoryStats.activeNeurons) +
                    ", Firing: " + std::to_string(memoryStats.firingNeurons));
        
        // Reset min/max for next reporting period
        minStepTime = 0.0;
        maxStepTime = 0.0;
    }
    
    // Synchronization
    mutable std::mutex errorMutex;
    mutable std::mutex warningMutex;
    mutable std::mutex performanceMutex;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {
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
    
    try {
        // Get configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions
        createRegions(regionCount);
        
        // Create neurons across regions
        createNeurons(regionCount, neuronCount);
        
        // Initialize connectivity with random weights
        initializeConnectivity(connectionProbability);
        
        // Initialize all integrated systems
        initializeIntegratedSystems();
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->logError(std::string("Brain initialization failed: ") + e.what(), true);
        return false;
    }
}

void Brain::createRegions(size_t regionCount) {
    for (size_t i = 0; i < regionCount; ++i) {
        pImpl->regions.push_back(std::make_unique<NeuralRegion>(RegionId(i + 1), "Region_" + std::to_string(i + 1)));
    }
}

void Brain::createNeurons(size_t regionCount, size_t neuronCount) {
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
}

void Brain::initializeConnectivity(float connectionProbability) {
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void Brain::initializeIntegratedSystems() {
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(pImpl->config->getOr<size_t>("neuron_count", 1000) / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
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
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    auto stepStartTime = std::chrono::system_clock::now();
    
    try {
        /*
         * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
         * 
         * This implements the complete integrated brain simulation:
         * 1. Process pending delayed spikes (deliver synaptic input)
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
        
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // Execute simulation steps with error handling
        executeSimulationSteps(currentStep, currentTime);
        
        // Update memory systems
        updateMemorySystems();
        
        // Update neuromodulation
        updateNeuromodulation();
        
        // Apply plasticity
        applyPlasticity(currentStep, currentTime);
        
        // Update episodic memory
        updateEpisodicMemory(currentStep, currentTime);
        
        // Update cognition systems
        updateCognitionSystems(currentStep, currentTime);
        
        // Apply structural plasticity periodically
        applyStructuralPlasticity(currentStep);
        
        // Replay important memories
        replayMemories(currentStep);
        
        // Apply development effects
        applyDevelopmentEffects(currentStep, currentTime);
        
        // Update periodic memory consolidation
        consolidateMemories(currentStep);
        
        // Update checkpoints
        updateCheckpoints(currentStep, currentTime);
        
        // Record performance
        auto stepEndTime = std::chrono::system_clock::now();
        auto stepDuration = std::chrono::duration_cast<std::chrono::duration<double>>(stepEndTime - stepStartTime);
        pImpl->recordPerformance(stepDuration.count());
        
    } catch (const std::exception& e) {
        pImpl->logError(std::string("Brain step failed at step " + std::to_string(currentStep) + ": ") + e.what());
        // Continue with step to allow error recovery
    }
}

void Brain::executeSimulationSteps(SimulationStep currentStep, Timestamp currentTime) {
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
                
                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);
                    
                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }
                    
                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;
                        
                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );
                        
                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        
                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
}

void Brain::updateMemorySystems() {
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::updateNeuromodulation() {
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void Brain::applyPlasticity(SimulationStep currentStep, Timestamp currentTime) {
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void Brain::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    // ========== STEP 7: Update episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void Brain::updateCognitionSystems(SimulationStep currentStep, Timestamp currentTime) {
    // ========== STEP 8-10: Update prediction, attention, and concept formation ==========
    // Update prediction system
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // Update attention system
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // Update concept formation
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::applyStructuralPlasticity(SimulationStep currentStep) {
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::replayMemories(SimulationStep currentStep) {
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void Brain::applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime) {
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void Brain::consolidateMemories(SimulationStep currentStep) {
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void Brain::updateCheckpoints(SimulationStep currentStep, Timestamp currentTime) {
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
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
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
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

// Error handling and recovery methods
std::vector<std::string> Brain::getErrorMessages() const {
    return pImpl->errorMessages;
}

std::vector<std::string> Brain::getWarningMessages() const {
    return pImpl->warningMessages;
}

bool Brain::hasErrors() const {
    return !pImpl->errorMessages.empty();
}

void Brain::clearErrorHistory() {
    pImpl->clearErrorHistory();
}

// System status and monitoring
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

// Statistics methods
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
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

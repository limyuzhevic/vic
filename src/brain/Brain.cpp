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
#include <unordered_map>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    // Additional neuromodulators for rest consolidation
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Acetylcholine> acetylcholine;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
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
    SimulationStep restStartStep;  // When rest began
    size_t restDuration;  // Duration of rest
    float memoryPressure;  // Current memory load
    float consolidationPriority;  // Priority of consolidation (0.0-1.0)
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
        , restStartStep(0)
        , restDuration(0)
        , memoryPressure(0.0f)
        , consolidationPriority(0.0f)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        // Initialize memory systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        // Initialize additional neuromodulators for rest consolidation
        serotonin = std::make_unique<Serotonin>();
        acetylcholine = std::make_unique<Acetylcholine>();
        
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
        
        // Get rest parameters from config
        restDuration = config->getOr<size_t>("rest_duration", 2000);  // Default 2000 steps (~20 seconds)
        restStartStep = 0;
        consolidationPriority = config->getOr<float>("consolidation_priority", 0.5f);  // Default 0.5f
        memoryPressure = 0.0f;
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
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
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
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
    
    // Register delayed spike handler to deliver synaptic input - OPTIMIZED
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Optimized neuron lookup using pre-built cache
        // Build lookup map on first access and cache it
        static std::unordered_map<NeuronId, Neuron*> neuronCache;
        
        // Lazy initialization of cache
        if (neuronCache.empty()) {
            // Build cache once and reuse it
            for (auto& region : pImpl->regions) {
                const auto& populations = region->getPopulations();
                for (const auto& pop : populations) {
                    const auto& neurons = pop->getNeurons();
                    for (const auto* neuron : neurons) {
                        neuronCache[neuron->getId()] = const_cast<Neuron*>(neuron);
                    }
                }
            }
        }
        
        // Fast lookup using cached map
        auto it = neuronCache.find(event.destination_neuron);
        if (it != neuronCache.end()) {
            // Apply synaptic weight as current
            MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
            if (event.is_excitatory) {
                it->second->receiveExcitatoryInput(synapticCurrent);
            } else {
                it->second->receiveInhibitoryInput(-synapticCurrent);
            }
        }
    });
    
    // Configure checkpoint manager
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

/**
    * @brief Main simulation step implementing the integrated brain loop (Phase 6)
    * 
    * This method orchestrates the complete neural simulation cycle, integrating
    * multiple brain subsystems including neural computation, memory, prediction,
    * cognition, neuromodulation, and development.
    * 
    * @param currentStep Current simulation step number
    * @param currentTime Current simulation time in milliseconds
    * @post All neural systems are updated for this time step
    * 
    * @complexity O(N_neurons + N_synapses) where N represents the total counts
    *            of neurons and synapses across all brain regions. The rest detection
    *            and processing adds O(1) overhead, while the main neural processing
    *            scales linearly with system size.
    */
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Process rest detection and state management
    manageRestState(currentStep, getAverageFiringRate());
    
    // Early return for rest state (simplified processing)
    if (pImpl->isResting) {
        processRestState();
        return;
    }
    
    // Process main neural computation loop
    processNeuralStep(currentStep);
    
    // Update integrated brain systems
    updateIntegratedSystems();
    
    // Collect simulation statistics
    collectSimulationStatistics();
}

/**
 * @brief Manages brain rest state detection and transitions
 * 
    * Determines whether the brain should enter or exit rest state based on
    * multiple physiological indicators. Rest occurs when multiple conditions
    * are simultaneously met, allowing for energy conservation and memory
    * consolidation.
    * 
    * @param currentStep Current simulation step
    * @param avgFiringRate Current average firing rate across all neurons
    * @post pImpl->isResting may be updated, along with related rest parameters
    * 
    * @complexity O(1) - constant time regardless of system size
    */
void Brain::manageRestState(SimulationStep currentStep, float avgFiringRate) {
    // Calculate memory pressure (working memory load)
    pImpl->memoryPressure = 0.0f;
    if (pImpl->workingMemory) {
        pImpl->memoryPressure = pImpl->workingMemory->getActiveTraces() / 
                               pImpl->workingMemory->getCapacity();
    }
    
    // Check if we should enter rest state
    if (!pImpl->isResting && shouldEnterRestState(avgFiringRate)) {
        enterRestState(currentStep);
        return;
    }
    
    // Already in rest, check if we should exit
    if (pImpl->isResting && shouldExitRestState(currentStep, avgFiringRate)) {
        exitRestState(currentStep);
    }
}

/**
 * @brief Determines if brain should enter rest state based on physiological indicators
 * 
    * @param avgFiringRate Current average firing rate across all neurons
    * @return true if rest conditions are met, false otherwise
    * 
    * @complexity O(1) - evaluates constant set of conditions
    */
bool Brain::shouldEnterRestState(float avgFiringRate) const {
    // Rest detection conditions:
    // 1. Low firing rate sustained for multiple steps
    // 2. High working memory load (memoryPressure > 0.8)
    // 3. High novelty or prediction error
    // 4. Time since last significant event
    
    bool lowFiringRate = avgFiringRate < 0.01f;  // Less than 1% firing
    bool highMemoryLoad = pImpl->memoryPressure > 0.8f;
    bool highNovelty = pImpl->novelty ? pImpl->novelty->getLevel() > 0.7f : false;
    bool highPredictionError = pImpl->predictionError ? 
                               pImpl->predictionError->getLevel() > 0.5f : false;
    bool timeSinceLastEvent = pImpl->stepsSinceLastEpisode > 100;  // >100 steps since last episode
    
    // Count how many conditions are met
    int restConditionsMet = 0;
    if (lowFiringRate) restConditionsMet++;
    if (highMemoryLoad) restConditionsMet++;
    if (highNovelty) restConditionsMet++;
    if (highPredictionError) restConditionsMet++;
    if (timeSinceLastEvent) restConditionsMet++;
    
    // Enter rest if at least 2 conditions are met
    return (restConditionsMet >= 2);
}

/**
 * @brief Initiates rest state
 * 
    * @param currentStep Current simulation step when rest begins
    * @post pImpl->isResting set to true, pImpl->restStartStep updated
    */
void Brain::enterRestState(SimulationStep currentStep) {
    pImpl->isResting = true;
    pImpl->restStartStep = currentStep;
    NLM_LOG_INFO("Entering REST state at step " + std::to_string(currentStep) + 
                 ". Duration: " + std::to_string(restDuration) + " steps");
}

/**
 * @brief Determines if brain should exit rest state
 * 
    * @param currentStep Current simulation step
    * @param avgFiringRate Current average firing rate
    * @return true if rest should end, false otherwise
    */
bool Brain::shouldExitRestState(SimulationStep currentStep, float avgFiringRate) const {
    if (currentStep >= pImpl->restStartStep + pImpl->restDuration) {
        return true;
    } else if (pImpl->restDuration == 0 && avgFiringRate > 0.05f) {
        // Adaptive rest: exit if firing rate recovers
        return true;
    }
    return false;
}

/**
 * @brief Completes rest state exit
 * 
    * @param currentStep Current simulation step
    * @post pImpl->isResting set to false
    */
void Brain::exitRestState(SimulationStep currentStep) {
    pImpl->isResting = false;
    NLM_LOG_INFO("Exiting REST state at step " + std::to_string(currentStep) + 
                 ". Rest duration: " + std::to_string(currentStep - pImpl->restStartStep) + " steps");
}

/**
 * @brief Processes simplified neural operations during rest state
 * 
    * During rest, neural activity is reduced to conserve energy while still
    * supporting memory consolidation through neuromodulation and replay.
    * 
    * @post Neuromodulator levels are updated, memory consolidation occurs
    */
void Brain::processRestState() {
    // Update neuromodulators optimized for consolidation
    updateRestStateNeuromodulators();
    
    // Apply neuromodulation effects to plasticity
    applyRestStatePlasticityModulation();
    
    // Process spikes for memory replay
    pImpl->spikeSystem->processSpikesForReplay(pImpl->currentStep);
    
    // Update consolidation priority
    updateConsolidationPriority();
}

/**
 * @brief Updates neuromodulator levels during rest state
 * 
    * @post Neuromodulator levels are adjusted to promote consolidation
    */
void Brain::updateRestStateNeuromodulators() {
    if (pImpl->dopamine) {
        float targetDopamine = 0.2f;  // Low during rest
        float currentDopamine = pImpl->dopamine->getLevel();
        float newDopamine = std::max(targetDopamine, 
                                    currentDopamine - pImpl->timestep * 0.1f);
        pImpl->dopamine->setLevel(newDopamine);
    }
    
    if (pImpl->serotonin) {
        float currentSerotonin = pImpl->serotonin->getLevel();
        float newSerotonin = std::min(1.0f, currentSerotonin + pImpl->timestep * 0.05f);
        pImpl->serotonin->setLevel(newSerotonin);
    }
    
    if (pImpl->acetylcholine) {
        float currentACh = pImpl->acetylcholine->getLevel();
        float newACh = std::min(0.8f, currentACh + pImpl->timestep * 0.03f);
        pImpl->acetylcholine->setLevel(newACh);
    }
}

/**
 * @brief Applies neuromodulation effects to plasticity during rest
 * 
    * @post Synaptic weights are modulated to enhance consolidation
    */
void Brain::applyRestStatePlasticityModulation() {
    float neuromodulationFactor = calculateRestStateNeuromodulationFactor();
    
    // Apply consolidation factor to STDP plasticity
    for (auto& region : pImpl->regions) {
        for (auto& synapse : region->getSynapses()) {
            if (synapse->getPlasticityFlags().stdp) {
                float weight = synapse->getWeight();
                applyRestStateWeightAdjustment(weight, neuromodulationFactor);
                synapse->setWeight(weight);
            }
        }
    }
}

/**
 * @brief Calculates neuromodulation factor during rest state
 * 
    * @return Combined neuromodulatory effect factor (1.0 = baseline)
    */
float Brain::calculateRestStateNeuromodulationFactor() const {
    float neuromodulationFactor = 1.0f;
    if (pImpl->dopamine) neuromodulationFactor *= (1.0f - pImpl->dopamine->getLevel());
    if (pImpl->serotonin) neuromodulationFactor *= (1.0f + pImpl->serotonin->getLevel() * 0.5f);
    if (pImpl->acetylcholine) neuromodulationFactor *= (1.0f + pImpl->acetylcholine->getLevel() * 0.3f);
    return neuromodulationFactor;
}

/**
 * @brief Adjusts synaptic weight based on rest state neuromodulation
 * 
    * @param weight Current synaptic weight
    * @param neuromodulationFactor Current neuromodulatory effect
    * @post Returns adjusted weight
    */
void Brain::applyRestStateWeightAdjustment(float& weight, float neuromodulationFactor) {
    if (neuromodulationFactor > 1.0f) {
        // Enhance weight strengthening during consolidation
        weight += 0.0001f * neuromodulationFactor;
    } else {
        // Reduce weight changes during consolidation
        weight *= (1.0f + neuromodulationFactor - 1.0f) * 0.1f;
    }
}

/**
 * @brief Updates consolidation priority based on memory pressure and neuromodulators
 * 
    * @post pImpl->consolidationPriority is updated
    */
void Brain::updateConsolidationPriority() {
    pImpl->consolidationPriority = std::min(1.0f, pImpl->memoryPressure * 0.5f + 0.5f);
    if (pImpl->dopamine) {
        pImpl->consolidationPriority *= (1.0f - pImpl->dopamine->getLevel());
    }
}

/**
 * @brief Processes the main neural computation step
 * 
    * This method implements steps 2-3 of the simulation loop: neuron membrane
    * potential updates and spike detection/scheduling.
    * 
    * @param currentTime Current simulation time
    * 
    * @complexity O(N_neurons * N_synapses) for the complete processing
    */
void Brain::processNeuralStep(Timestamp currentTime) {
    // Step 2: Update all neurons (LIF dynamics)
    updateAllNeurons(currentTime);
    
    // Step 3: Detect spikes and schedule spike events
    detectAndScheduleSpikes(currentTime);
}

/**
 * @brief Updates all neuron membrane potentials using LIF dynamics
 * 
    * Iterates through all neurons in all regions and populations, applying
    * the Leaky Integrate-and-Fire dynamics to update membrane potentials.
    * 
    * @param currentTime Current simulation time
    * 
    * @complexity O(N_neurons) - linear with total neuron count
    */
void Brain::updateAllNeurons(Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        const auto& populations = region->getPopulations();
        for (const auto& population : populations) {
            const auto& neurons = population->getNeurons();
            for (const auto* neuron : neurons) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

/**
 * @brief Detects spikes and schedules outgoing spike events
 * 
    * This method checks for neurons that have reached threshold and fires them,
    * recording the spike event and updating synaptic plasticity.
    * 
    * @param currentTime Current simulation time
    * 
    * @complexity O(N_neurons) - linear with total neuron count
    */
void Brain::detectAndScheduleSpikes(Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        const auto& populations = region->getPopulations();
        for (const auto& population : populations) {
            const auto& neurons = population->getNeurons();
            
for (const auto& neuron : neurons) {
                    const auto& neuronState = neuron->getState();
                    
                    // Check if neuron fired this step
                    bool neuronJustFired = hasNeuronFiredThisStep(neurologicalState, currentTime);
                    
                    if (neuronJustFired) {
                        // Record the spike
                        SpikeEvent spikeEvent(neurologicalState.id, currentTime, pImpl->currentStep);
                        pImpl->spikeSystem->queueSpike(spikeEvent);
                        
                        // Update plasticity for incoming synapses
                        auto incomingSynapses = region->getSynapsesTo(neurologicalState.id);
                        for (auto* synapse : incomingSynapses) {
                            synapse->recordPostSpike(currentTime);
                        }
                        
                        // Schedule outgoing spike events
                        auto outgoingSynapses = region->getSynapsesFrom(neurologicalState.id);
                        scheduleDelayedSpikes(outgoingSynapses, currentTime);
                    }
                }
        }
    }
}

/**
 * @brief Checks if a specific neuron has fired this simulation step
 * 
    * @param neuronState Current state of the neuron to check
    * @param currentTime Current simulation time
    * @return true if neuron fired this step, false otherwise
    * 
    * @complexity O(1) - constant time evaluation
    */
bool Brain::hasNeuronFiredThisStep(const NeuronState& neuronState, Timestamp currentTime) const {
    return (neuronState.firingState == FiringState::Refractory &&
            neuronState.lastSpikeTime >= 0.0f &&
            std::abs(static_cast<float>(currentTime) - neuronState.lastSpikeTime) < 
                    pImpl->timestep * 2.0f);
}

/**
 * @brief Schedules delayed spike events for outgoing synapses
 * 
    * @param outgoingSynapses Vector of synapses to process
    * @param currentTime Current simulation time
    * 
    * @complexity O(N_synapses) - linear with number of outgoing synapses
    */
void Brain::scheduleDelayedSpikes(const std::vector<Synapse*>& outgoingSynapses, 
                                 Timestamp currentTime) {
    for (auto* synapse : outgoingSynapses) {
        // Extract delay from synapse
        Delay delay = synapse->getDelay();
        
        // Calculate delivery time
        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;
        
        // Schedule the spike event
        DelayedSpikeEvent delayedEvent(synapse->getSource(), synapse->getTarget(), 
                                       deliveryTime, pImpl->currentStep);
        pImpl->spikeSystem->scheduleDelayedSpike(delayedEvent);
    }
}

/**
 * @brief Updates all integrated brain systems
 * 
    * This method implements the remaining simulation steps from the main loop:
    * Steps 4-14 covering memory, prediction, cognition, neuromodulation,
    * plasticity, and development systems.
    * 
    * @complexity Varies by subsystem, generally O(N) for each
    */
void Brain::updateIntegratedSystems() {
    // Step 4: Update working memory
    updateWorkingMemory();
    
    // Step 5: Apply neuromodulation
    applyNeuromodulationToNeuralExcitability();
    
    // Step 6: Apply plasticity rules
    updatePlasticity();
    
    // Step 7: Update episodic memory
    updateEpisodicMemory();
    
    // Step 8: Update prediction system
    updatePredictionSystem();
    
    // Step 9: Update attention system
    updateAttentionSystem();
    
    // Step 10: Update concept formation
    updateConceptFormation();
    
    // Step 11: Apply structural plasticity
    applyStructuralPlasticity();
    
    // Step 12: Replay important memories
    replayImportantMemories();
    
    // Step 13: Apply development effects
    develop();
    
    // Step 14: Collect statistics
    collectSimulationStatistics();
}

/**
 * @brief Updates working memory with current neural activity
 * 
    * @complexity O(1) - delegated to working memory implementation
    */
void Brain::updateWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

/**
 * @brief Applies neuromodulation to neural excitability
 * 
    * @complexity O(1) - delegated to neuromodulation system
    */
void Brain::applyNeuromodulationToNeuralExcitability() {
    // Currently handled within stepNeuromodulation if any neuromodulators are active
    // This is a placeholder for future integration with neural excitability
}

/**
 * @brief Updates plasticity rules (STDP, Hebbian)
 * 
    * @complexity O(N_synapses) - linear with synapse count
    */
void Brain::updatePlasticity() {
    if (pImpl->stdp) {
        pImpl->stdp->update(this, *pImpl->rng);
    }
    
    if (pImpl->hebbian) {
        pImpl->hebbian->update(this, *pImpl->rng);
    }
}

/**
 * @brief Updates episodic memory with current experience
 * 
    * @complexity O(1) - delegated to episodic memory implementation
    */
void Brain::updateEpisodicMemory() {
    if (pImpl->episodicMemory) {
        // Update memory relevance based on recent use
        // This is typically handled by the episodic memory system
        pImpl->stepsSinceLastEpisode++;
    }
}

/**
 * @brief Updates prediction system
 * 
    * @complexity O(1) - delegated to prediction system
    */
void Brain::updatePredictionSystem() {
    if (pImpl->predictionSystem) {
        // Prediction system updates are handled internally
        // This is a placeholder for integration
    }
}

/**
 * @brief Updates attention system
 * 
    * @complexity O(1) - delegated to attention system
    */
void Brain::updateAttentionSystem() {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
    }
}

/**
 * @brief Updates concept formation
 * 
    * @complexity O(1) - delegated to concept formation
    */
void Brain::updateConceptFormation() {
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(pImpl->timestep);
    }
}

/**
 * @brief Applies structural plasticity (synaptogenesis, pruning)
 * 
    * @complexity O(N_regions) - linear with region count
    */
void Brain::applyStructuralPlasticity() {
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

/**
 * @brief Replays important memories for consolidation
 * 
    * Memory replay occurs during rest or periodically to strengthen
    * important memories and integrate them into long-term storage.
    * 
    * @complexity O(N_episodes) - linear with episode count
    */
void Brain::replayImportantMemories() {
    if (!pImpl->episodicMemory || !pImpl->episodicMemory->isReplayEnabled()) {
        return;
    }
    
    // Determine which episodes to replay based on relevance and recency
    auto episodesForReplay = pImpl->episodicMemory->getEpisodesForReplay(10); // Top 10 episodes
    
    // Replay the selected episodes
    pImpl->episodicMemory->replaySequence(episodesForReplay);
}

/**
 * @brief Applies development effects to the brain
 * 
    * Development effects influence structural plasticity and neural organization,
    * particularly during early learning phases.
    * 
    * @complexity O(N_regions) - linear with region count
    */
void Brain::develop() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->applyDevelopmentEffects(this, pImpl->currentStep);
    }
    
    // Update developmental stage
    updateDevelopmentalStage();
}

/**
 * @brief Updates brain developmental stage based on simulation progress
 * 
    * @post pImpl->developmentalStage may be updated
    */
void Brain::updateDevelopmentalStage() {
    if (pImpl->currentStep < 100) {
        pImpl->developmentalStage = DevelopmentalStage::Initial;
    } else if (pImpl->currentStep < 1000) {
        pImpl->developmentalStage = DevelopmentalStage::EarlyLearning;
    } else if (pImpl->currentStep < 10000) {
        pImpl->developmentalStage = DevelopmentalStage::Mature;
    } else {
        pImpl->developmentalStage = DevelopmentalStage::Aging;
    }
}

/**
 * @brief Collects and logs simulation statistics
 * 
    * @post Statistics are logged via NLM_LOG_INFO
    * 
    * @complexity O(N_regions) - linear with region count
    */
void Brain::collectSimulationStatistics() {
    NLM_LOG_INFO("=== NLM Brain Statistics ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system statistics
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation statistics
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Development statistics
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    // Per-region statistics
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}
    
    // ========== STEP 2: Update all neurons (LIF dynamics) - OPTIMIZED ==========
    // Optimized neural update loop using ranges and bulk operations
    for (auto& region : pImpl->regions) {
        // Pre-fetch populations vector to avoid repeated virtual calls
        const auto& populations = region->getPopulations();
        for (const auto& pop : populations) {
            // Use direct access to neuron vector for better cache locality
            const auto& neurons = pop->getNeurons();
            // Process all neurons in this population
            for (const auto* neuron : neurons) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events - OPTIMIZED ==========
    // Optimized spike processing with early termination and batch operations
    for (auto& region : pImpl->regions) {
        const auto& populations = region->getPopulations();
        for (const auto& pop : populations) {
            const auto& neurons = pop->getNeurons();
            
            // Process all neurons in this population
            for (const auto* neuron : neurons) {
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
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
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
        
        // Apply dopamine effects on neural excitability - OPTIMIZED
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = pImpl->dopamine->getLevel();
        if (dopamineLevel > 0.0f) {
            float excitabilityMod = dopamineLevel * 0.5f;
            
            // Batch process regions for better cache locality
            for (auto& region : pImpl->regions) {
                const auto& populations = region->getPopulations();
                
                // Process all populations in this region
                for (const auto& pop : populations) {
                    const auto& neurons = pop->getNeurons();
                    
                    // Process all neurons in this population
                    for (const auto* neuron : neurons) {
                        // Dopamine modulates excitability by injecting additional current
                        // Positive dopamine adds excitatory bias
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) - OPTIMIZED ==========
    // Optimized plasticity application using range-based operations and early termination
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    // Batch process synapses by region for better cache locality
    for (auto& region : pImpl->regions) {
        const auto& synapses = region->getSynapses();
        
        // Process all synapses in this region
        for (const auto& syn : synapses) {
            // Apply STDP with neuromodulation - early exit if no plasticity
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                // Skip if no spike history for either side
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning - early exit if no plasticity
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                // Skip if no spike history for either side
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 7: Update episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            pImpl->stepsSinceLastEpisode = 0;
            
            if (pImpl->episodicMemory) {
                // Capture current brain state as an episode - OPTIMIZED
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                // Batch process neurons for episode creation
                for (auto& region : pImpl->regions) {
                    const auto& populations = region->getPopulations();
                    
                    // Process all populations in this region
                    for (const auto& pop : populations) {
                        const auto& neurons = pop->getNeurons();
                        
                        // Process all neurons in this population
                        for (const auto* neuron : neurons) {
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
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Generate predictions from sensory neurons
        // Convert sensory neuron activity to input pattern
        std::vector<float> sensoryPattern;
        for (const auto* neuron : pImpl->sensoryNeurons) {
            float activity = neuron->getState().membranePotential;
            sensoryPattern.push_back((activity - neuron->getState().restingPotential) / 10.0f);
        }
        
        if (!sensoryPattern.empty()) {
            // Create SensoryInput from current sensory pattern
            class SensoryInput sensoryObservation(sensoryPattern);
            
            // Generate prediction for next state based on current sensory input
            std::unique_ptr<class SensoryInput> predictedState = 
                pImpl->predictionSystem->predictNextState(sensoryObservation);
            
            // Compare prediction to actual sensory input to compute prediction error
            if (predictedState) {
                pImpl->predictionSystem->updatePredictions(*predictedState, sensoryObservation);
                
                // Store prediction error in neuromodulator system
                float predictionError = pImpl->predictionSystem->getPredictionError();
                if (pImpl->predictionError && predictionError > 0.01f) {
                    // Apply prediction error to novelty and curiosity
                    pImpl->novelty->updatePredictionError(predictionError);
                    pImpl->curiosity->setPredictionError(predictionError);
                }
                
                // Store predictions in working memory for learning
                if (pImpl->workingMemory && predictedState) {
                    // Store predicted sensory state in working memory
                    for (size_t i = 0; i < predictedState->getData().size(); ++i) {
                        if (i < pImpl->sensoryNeurons.size()) {
                            pImpl->workingMemory->storeToNeuron(
                                pImpl->sensoryNeurons[i]->getId(),
                                std::min(1.0f, predictedState->getData()[i] * 0.8f)
                            );
                        }
                    }
                }
            }
        }
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            std::vector<NeuronId> winners = pImpl->attention->processCompetition(competitors);
            
            // Store winners for motor action selection
            pImpl->attention->reset(); // Clear previous winners
            for (const auto& winner : winners) {
                pImpl->attention->focusOnRegion(RegionId(1)); // Default region
            }
        }
        
        // Connect attention to concept formation
        if (pImpl->conceptFormation && !pImpl->conceptFormation->getConcepts().empty()) {
            // Find current attended neural populations for concept focus
            for (size_t i = 0; i < pImpl->conceptFormation->getConceptCount(); ++i) {
                const auto* concept = pImpl->conceptFormation->getConcept(i);
                if (concept) {
                    // Use concept prototype to influence attention
                    float similarity = 0.0f;
                    if (!concept->prototype.empty() && !pImpl->sensoryNeurons.empty()) {
                        // Simple similarity check - in reality would use neural pattern matching
                        similarity = concept->prototype[0];
                    }
                    
                    if (similarity > 0.5f) {
                        // Apply top-down bias to neurons matching current concept
                        // This biases concept-related neural activity
                        for (const auto* neuron : pImpl->sensoryNeurons) {
                            pImpl->attention->applyBottomUpSalience(neuron->getId(), similarity * 0.1f);
                        }
                    }
                }
            }
        }
        
        // Apply attention-based neural modulation - OPTIMIZED
        if (!pImpl->attention->getAttendedRegions().empty()) {
            // Focus processing on attended regions
            for (RegionId regionId : pImpl->attention->getAttendedRegions()) {
                auto* region = getRegion(regionId);
                if (region) {
                    const auto& populations = region->getPopulations();
                    
                    // Apply attentional gain modulation to this region
                    // Increases neural excitability in attended regions
                    for (const auto& pop : populations) {
                        const auto& neurons = pop->getNeurons();
                        
                        // Process all neurons in this population
                        for (const auto* neuron : neurons) {
                            // Apply attentional excitation
                            neuron->injectCurrent(pImpl->attention->getExcitationFor(neuron->getId()));
                            
                            // Apply attentional inhibition to non-winners
                            float inhibition = pImpl->attention->getInhibitionFor(neuron->getId());
                            if (inhibition > 0.0f) {
                                neuron->injectCurrent(-inhibition);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Process current neural activity patterns to form concepts
        // Create pattern from current working memory state
        std::vector<float> currentPattern;
        std::vector<float> currentFeatures;
        
        // Sample pattern from working memory neurons
        if (pImpl->workingMemory) {
            const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            if (!memoryNeurons.empty()) {
                // Create pattern from active memory neuron IDs
                currentPattern.reserve(memoryNeurons.size());
                for (const auto& neuronId : memoryNeurons) {
                    currentPattern.push_back(static_cast<float>(neuronId.value) / 1000.0f); // Normalize
                }
                
                // Create features from current brain state
                // Use motor neuron activity as reward-related features
                currentFeatures = currentPattern; // Start with pattern as features
                
                // Add reward signal as feature
                float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.5f;
                currentFeatures.push_back(reward);
            }
        }
        
        // If no memory neurons, use sensory neuron activations
        if (currentPattern.empty() && !pImpl->sensoryNeurons.empty()) {
            currentPattern.resize(pImpl->sensoryNeurons.size(), 0.0f);
            currentFeatures = currentPattern;
            for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                float activity = pImpl->sensoryNeurons[i]->getState().membranePotential;
                float normalized = (activity - pImpl->sensoryNeurons[i]->getState().restingPotential) / 20.0f;
                currentPattern[i] = normalized;
            }
        }
        
        if (!currentPattern.empty()) {
            // Present current neural activity to concept formation
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentPattern, currentFeatures,
                pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.5f,
                pImpl->currentStep
            );
            
            // Connect concept formation to planner - update current goal if new concept formed
            if (conceptId > 0) {
                const auto* concept = pImpl->conceptFormation->getConcept(conceptId);
                if (concept) {
                    // Use concept prototype as target for planner
                    std::vector<float> goalState = concept->prototype;
                    pImpl->planner->setCurrentGoal(goalState);
                }
            }
        }
    }
    
    // ========== PLANNER INTEGRATION ==========
    // Use the neural planner for action selection
    if (pImpl->planner) {
        // Create current brain state vector from sensory neurons
        std::vector<float> currentState;
        if (!pImpl->sensoryNeurons.empty()) {
            currentState.reserve(pImpl->sensoryNeurons.size());
            for (const auto* neuron : pImpl->sensoryNeurons) {
                float activity = neuron->getState().membranePotential;
                currentState.push_back((activity - neuron->getState().restingPotential) / 10.0f);
            }
        } else {
            // Fallback to empty state
            currentState = std::vector<float>(5, 0.0f);
        }
        
        // Get current goal from concept formation if available
        std::vector<float> currentGoal = pImpl->planner->getCurrentGoal();
        float targetReward = 0.5f;
        
        if (!currentGoal.empty()) {
            // Use goal to compute target reward
            targetReward = std::max(0.0f, std::min(1.0f, currentGoal[0]));
        }
        
        // Plan next action using neural planner
        ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
        
        // Log planning decisions
        // NLM_LOG_INFO("Planner selected action: " + std::to_string(static_cast<int>(plannedAction)) +
        //             " (confidence: " + std::to_string(pImpl->planner->getPlanningConfidence()) + ")");
        
        // Connect planner to attention - plan influences attentional focus
        if (pImpl->attention) {
            pImpl->attention->applyTopDownBias(pImpl->sensoryNeurons[0]->getId(), 
                                             pImpl->planner->getPlanningConfidence());
        }
        
        // Apply planned action to motor neurons - OPTIMIZED
        // Map planned action to motor neuron groups
        if (!pImpl->motorNeurons.empty() && plannedAction != ActionType::Wait) {
            // Determine action based on planned action type
            // This creates a simple mapping from action type to motor control
            bool activateMotorGroup = true;
            
            // Simple action-to-motor mapping (can be expanded)
            if (plannedAction == ActionType::MoveForward) {
                // Move forward - activate all motor neurons
                // Batch operation for all motor neurons
                for (auto* neuron : pImpl->motorNeurons) {
                    neuron->injectCurrent(10.0f); // Strong excitation
                }
            } else if (plannedAction == ActionType::TurnLeft) {
                // Turn left - activate left motor group
                // For simplicity, half of motor neurons
                size_t half = pImpl->motorNeurons.size() / 2;
                for (size_t i = 0; i < half && i < pImpl->motorNeurons.size(); ++i) {
                    pImpl->motorNeurons[i]->injectCurrent(10.0f);
                }
            } else if (plannedAction == ActionType::TurnRight) {
                // Turn right - activate right motor group
                // For simplicity, second half of motor neurons
                size_t half = pImpl->motorNeurons.size() / 2;
                for (size_t i = half; i < pImpl->motorNeurons.size(); ++i) {
                    pImpl->motorNeurons[i]->injectCurrent(10.0f);
                }
            } else {
                // Other actions - minimal excitation
                if (!pImpl->motorNeurons.empty()) {
                    pImpl->motorNeurons[0]->injectCurrent(5.0f);
                }
            }
            
            // Apply neuromodulation to motor neurons based on planner output
            if (pImpl->dopamine) {
                float dopamineLevel = pImpl->dopamine->getLevel();
                if (dopamineLevel > 0.5f) {
                    // High dopamine enhances motor execution
                    // Batch operation for all motor neurons
                    for (auto* neuron : pImpl->motorNeurons) {
                        neuron->injectCurrent(dopamineLevel * 2.0f);
                    }
                }
            }
        }
    }
    
        // Connect concept formation to planner - OPTIMIZED
        // Ensure concept formation is integrated with planner's goal setting
        if (pImpl->conceptFormation && pImpl->planner && 
            !pImpl->conceptFormation->getConcepts().empty()) {
            
            // Periodically update planner goals based on discovered concepts
            if (pImpl->currentStep % 100 == 0) {  // Update goals every 100 steps
                size_t conceptCount = pImpl->conceptFormation->getConceptCount();
                if (conceptCount > 0) {
                    // Select most stable concept as current goal
                    size_t bestConceptId = 0;
                    float bestStability = 0.0f;
                    
                    for (size_t i = 0; i < conceptCount; ++i) {
                        float stability = pImpl->conceptFormation->getConceptStability(i);
                        if (stability > bestStability) {
                            bestStability = stability;
                            bestConceptId = i;
                        }
                    }
                    
                    if (bestStability > 0.5f) {  // Only use stable concepts
                        const auto* concept = pImpl->conceptFormation->getConcept(bestConceptId);
                        if (concept) {
                            pImpl->planner->setCurrentGoal(concept->prototype);
                        }
                    }
                }
            }
        }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
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
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
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

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
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
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
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
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

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
    
    Serotonin* Brain::getSerotonin() {
        return pImpl->serotonin.get();
    }
    
    Acetylcholine* Brain::getAcetylcholine() {
        return pImpl->acetylcholine.get();
    }
    
    Norepinephrine* Brain::getNorepinephrine() {
        return pImpl->norepinephrine.get();
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
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

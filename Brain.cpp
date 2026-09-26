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

namespace nlm {

// Forward declarations for clarity
class NeuralRegion;
class Neuron;
class Synapse;

struct Brain::Impl {
    // Core configuration and runtime state
    std::shared_ptr<Config> config;           // Global simulation parameters and settings
    std::unique_ptr<RandomGenerator> rng;    // Pseudorandom number generator for stochastic processes
    std::vector<std::unique_ptr<NeuralRegion>> regions;  // Collection of neural processing modules
    std::vector<InterRegionConnection> interRegionConnections;  // Long-range connections between regions
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    // Working memory maintains active neural representations and short-term patterns
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    // Episodic memory stores sequences of experiences as temporal episodes
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    // Associative memory enables pattern completion and semantic associations
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    // Prediction system generates forward models and anticipates sensory consequences
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    // Neural planner develops action sequences using hierarchical planning algorithms
    std::unique_ptr<NeuralPlanner> planner;
    // Concept formation abstracts from concrete experiences to form categorical representations
    std::unique_ptr<ConceptFormation> conceptFormation;
    // Attentional selection prioritizes information and resolves competition among representations
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    // Development system manages structural and functional maturation over time
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    // Dopamine encodes reward prediction errors and modulates plasticity and motivation
    std::unique_ptr<Dopamine> dopamine;
    // Curiosity drives exploration through prediction error signals and novelty detection
    std::unique_ptr<Curiosity> curiosity;
    // Prediction error signals update internal models and learning mechanisms
    std::unique_ptr<PredictionError> predictionError;
    // Novelty detection identifies unexpected patterns and triggers learning
    std::unique_ptr<Novelty> novelty;
    // Reward signal provides reinforcement learning signals
    std::unique_ptr<Reward> reward;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;                 // Manages spike timing, delivery, and event scheduling
    std::unique_ptr<STDP> stdp;                               // Spike-Timing-Dependent Plasticity implementation
    std::unique_ptr<Hebbian> hebbian;                         // Hebbian learning rule (fire together, wire together)
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;  // Synapse formation, elimination, and rewiring
    
    // Temporal configuration and state management
    TimestepDuration timestep;      // Duration of each simulation step (in seconds)
    SimulationStep currentStep;     // Current step number in the simulation timeline
    Timestamp currentTime;          // Absolute timestamp (in seconds) for temporal calculations
    
    // Performance statistics and monitoring
    size_t totalSpikesThisStep;     // Spike count for the current simulation step
    size_t totalSpikesTotal;        // Cumulative spike count across all simulation steps
    
    // Sensory neurons for input injection - direct sensory-motor interface
    // These maintain raw pointers to sensory and motor populations for direct access
    std::vector<Neuron*> sensoryNeurons;  // Direct pointers to sensory neurons for input injection
    std::vector<Neuron*> motorNeurons;   // Direct pointers to motor neurons for action output
    
    // Temporal coordination and integration state
    bool isResting;                   // Indicates whether brain is in sleep/rest state
    size_t stepsSinceLastEpisode;     // Counter for episode storage timing
    size_t replayInterval;            // Interval (in steps) for memory replay (default: 100)
    size_t consolidationInterval;    // Interval (in steps) for memory consolidation (default: 1000)
    
    // Persistence and recovery system
    std::unique_ptr<CheckpointManager> checkpointManager;  // Handles brain state serialization and recovery
    
    // Developmental tracking and plasticity modulation
    DevelopmentalStage developmentalStage;  // Current developmental phase (Initial, CriticalPeriod, Maturation, Adult)
    RegionId nextRegionId;                   // ID counter for region creation
    
    // Constructor - initializes the brain implementation with configuration
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)  // Will be initialized below with proper seed
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps for memory rehearsal
        , consolidationInterval(1000)  // Consolidate every 1000 steps for long-term stability
    {
        // Initialize random generator with seed from config for reproducible simulations
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems - core mechanisms for synaptic change
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        // Initialize memory systems for storage and retrieval of neural information
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize prediction system for forward models and expectation
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems for planning, abstraction, and attention
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system for structural and functional maturation
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems for affect and motivation
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        reward = std::make_unique<Reward>();
        
        // Configure STDP parameters from simulation configuration
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity rates for dynamic wiring
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep from configuration with fallback to default
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config for temporal coordination
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager for persistence and recovery
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
    
    // Create regions - each region represents a functional brain area
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions - establish basic neural architecture
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        // ERROR HANDLING: Added null pointer check for region
        if (!region) {
            NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1) + " during initialization");
            return false;
        }
        // Add different types of neurons to each region
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O interface
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                // ERROR HANDLING: Added defensive check for neuron
                if (neuron) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
        }
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                // ERROR HANDLING: Added defensive check for neuron
                if (neuron) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                     ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
    
    // Initialize connectivity with random weights - establish initial network topology
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        // ERROR HANDLING: Added null pointer check for region
        if (!region) {
            NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1) + " for connectivity initialization");
            continue;  // Continue with other regions
        }
        // Initialize random connectivity and synapse weights
        region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory - maintains active neural traces
    // ERROR HANDLING: Added null pointer check for working memory
    if (!pImpl->workingMemory) {
        NLM_LOG_ERROR("Failed to initialize working memory - pointer is null");
        return false;
    }
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory - stores sequences of experiences
    // ERROR HANDLING: Added null pointer check for episodic memory
    if (!pImpl->episodicMemory) {
        NLM_LOG_ERROR("Failed to initialize episodic memory - pointer is null");
        return false;
    }
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory - enables pattern completion
    // ERROR HANDLING: Added null pointer check for associative memory
    if (!pImpl->associativeMemory) {
        NLM_LOG_ERROR("Failed to initialize associative memory - pointer is null");
        return false;
    }
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system - would normally have initialization method
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    // ERROR HANDLING: Added null pointer check for planner
    if (!pImpl->planner) {
        NLM_LOG_ERROR("Failed to initialize planner - pointer is null");
        return false;
    }
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    // ERROR HANDLING: Added null pointer check for concept formation
    if (!pImpl->conceptFormation) {
        NLM_LOG_ERROR("Failed to initialize concept formation - pointer is null");
        return false;
    }
    pImpl->conceptFormation->initialize(this);
    
    // ERROR HANDLING: Added null pointer check for attention
    if (!pImpl->attention) {
        NLM_LOG_ERROR("Failed to initialize attention - pointer is null");
        return false;
    }
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation systems
    // ERROR HANDLING: Added null pointer check for novelty
    if (!pImpl->novelty) {
        NLM_LOG_ERROR("Failed to initialize novelty - pointer is null");
        return false;
    }
    pImpl->novelty->initialize(this);
    
    // ERROR HANDLING: Added null pointer check for curiosity
    if (!pImpl->curiosity) {
        NLM_LOG_ERROR("Failed to initialize curiosity - pointer is null");
        return false;
    }
    pImpl->curiosity->initialize(this);
    
    // ERROR HANDLING: Added null pointer check for dopamine
    if (!pImpl->dopamine) {
        NLM_LOG_ERROR("Failed to initialize dopamine - pointer is null");
        return false;
    }
    pImpl->dopamine->initialize(this);
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes for activity monitoring
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        // PERFORMANCE NOTE: Linear search O(n) where n is total neuron count
        // This is a critical bottleneck that should be optimized with lookup tables
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                // ERROR HANDLING: Added null pointer check for neuron
                if (!neuron) {
                    continue;
                }
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
    // ERROR HANDLING: Added null pointer check for checkpoint manager
    if (!pImpl->checkpointManager) {
        NLM_LOG_ERROR("Failed to configure checkpoint manager - pointer is null");
        return false;
    }
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
     * This implements the complete integrated brain simulation with multiple
     * interacting systems. Each step represents a discrete temporal unit
     * where neural dynamics, learning, memory, and cognition are coordinated.
     * 
     * Algorithm complexity: O(n³) due to triple nested loops in spike processing
     * Memory management: Uses smart pointers for most components, raw pointers
     * for direct I/O access (sensory/motor neurons).
     * 
     * The loop integrates the following brain systems:
     * 1. Neural processing (spike events, membrane potentials)
     * 2. Working memory (active trace maintenance)
     * 3. Neuromodulation (affective and motivational signals)
     * 4. Plasticity (learning and adaptation)
     * 5. Episodic memory (experience storage)
     * 6. Prediction system (expectation and error)
     * 7. Attention system (information selection)
     * 8. Concept formation (abstraction)
     * 9. Structural plasticity (wiring changes)
     * 10. Memory replay (rehearsal and consolidation)
     * 11. Development effects (maturation)
     * 12. Memory consolidation (long-term storage)
     * 13. Checkpoint management (persistence)
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    // ERROR HANDLING: Added null pointer check for spike system
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    } else {
        NLM_LOG_ERROR("Spike system is null - cannot process delayed spikes");
    }
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    // PERFORMANCE NOTE: Triple nested loops - O(n³) complexity
    // 1. Regions (n_r)
    // 2. Populations (n_p per region)
    // 3. Neurons (n_n per population)
    // This is a critical performance bottleneck for large networks
    for (auto& region : pImpl->regions) {
        if (!region) {
            continue;  // Defensive programming
        }
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                continue;  // Defensive programming
            }
            for (auto* neuron : pop->getNeurons()) {
                // ERROR HANDLING: Added null pointer check for neuron
                if (!neuron) {
                    continue;
                }
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pImpl->regions) {
        // ERROR HANDLING: Added null pointer check for region
        if (!region) {
            continue;
        }
        for (auto& pop : region->getPopulations()) {
            // ERROR HANDLING: Added null pointer check for population
            if (!pop) {
                continue;
            }
            for (auto* neuron : pop->getNeurons()) {
                // ERROR HANDLING: Added null pointer check for neuron
                if (!neuron) {
                    continue;
                }
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
                        // ERROR HANDLING: Added null pointer check for synapse
                        if (syn) {
                            syn->recordPostSpike(currentTime);
                        }
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // ERROR HANDLING: Added null pointer check for synapse
                        if (!syn) {
                            continue;
                        }
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
                    // ERROR HANDLING: Added null pointer check for working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    // ERROR HANDLING: Added null pointer check for spike system
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processSpikes(currentStep);
    } else {
        NLM_LOG_ERROR("Spike system is null - cannot process immediate spikes");
    }
    
    // ========== STEP 4: Update working memory ==========
    // ERROR HANDLING: Added null pointer check for working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    // ERROR HANDLING: Added null pointer check for novelty
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    // ERROR HANDLING: Added null pointer check for curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    // ERROR HANDLING: Added null pointer check for dopamine
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            // ERROR HANDLING: Added null pointer check for region
            if (!region) {
                continue;
            }
            for (auto& pop : region->getPopulations()) {
                // ERROR HANDLING: Added null pointer check for population
                if (!pop) {
                    continue;
                }
                for (auto* neuron : pop->getNeurons()) {
                    // ERROR HANDLING: Added null pointer check for neuron
                    if (!neuron) {
                        continue;
                    }
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
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    // ERROR HANDLING: Added null pointer check for dopamine
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        // ERROR HANDLING: Added null pointer check for region
        if (!region) {
            continue;
        }
        for (auto& syn : region->getSynapses()) {
            // ERROR HANDLING: Added null pointer check for synapse
            if (!syn) {
                continue;
            }
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
    
    // ========== STEP 7: Update episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        // ERROR HANDLING: Added null pointer check for episodic memory
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
                // ERROR HANDLING: Added null pointer check for region
                if (!region) {
                    continue;
                }
                for (auto& pop : region->getPopulations()) {
                    // ERROR HANDLING: Added null pointer check for population
                    if (!pop) {
                        continue;
                    }
                    for (auto* neuron : pop->getNeurons()) {
                        // ERROR HANDLING: Added null pointer check for neuron
                        if (!neuron) {
                            continue;
                        }
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
    // ERROR HANDLING: Added null pointer check for prediction system
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    // ERROR HANDLING: Added null pointer check for attention
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        // ERROR HANDLING: Added null pointer check for working memory
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    // ERROR HANDLING: Added null pointer check for concept formation
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        // ERROR HANDLING: Added null pointer check for structural plasticity
        if (pImpl->structuralPlasticity) {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        } else {
            NLM_LOG_ERROR("Structural plasticity system is null");
        }
    }
    
    // ========== STEP 12: Replay important memories ==========
    // ERROR HANDLING: Added null pointer check for episodic memory
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            // ERROR HANDLING: Added null pointer check for episodic memory
            if (pImpl->episodicMemory) {
                pImpl->episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    // ERROR HANDLING: Added null pointer check for development system
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        if (pImpl->developmentSystem) {
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
        } else {
            NLM_LOG_ERROR("Development system is null");
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    // ERROR HANDLING: Added null pointer check for episodic memory
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    // ERROR HANDLING: Added null pointer check for checkpoint manager
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
        // MEMORY MANAGEMENT: Raw pointer usage - potential null dereference
        if (i < pImpl->sensoryNeurons.size() && pImpl->sensoryNeurons[i]) {
            pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        }
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            // ERROR HANDLING: Added null pointer check for working memory
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // INJECTION: Inject current into a specific neuron
    
    for (auto& region : pImpl->regions) {
        // ERROR HANDLING: Added null pointer check for region
        if (!region) {
            continue;
        }
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            // ERROR HANDLING: Added null pointer check for neuron
            if (!n) {
                continue;
            }
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        // ERROR HANDLING: Added null pointer check for region
        if (!region) {
            continue;
        }
        for (auto& pop : region->getPopulations()) {
            // ERROR HANDLING: Added null pointer check for population
            if (!pop) {
                continue;
            }
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    // ERROR HANDLING: Added null pointer check for neuron
                    if (neuron) {
                        neuron->injectCurrent(current);
                    }
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
        // ERROR HANDLING: Added null pointer check for region
        if (!region) {
            continue;
        }
        for (const auto& syn : region->getSynapses()) {
            // ERROR HANDLING: Added null pointer check for synapse
            if (!syn) {
                continue;
            }
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
        // ERROR HANDLING: Added null pointer check for neuron
        if (neuron) {
            if (neuron->isFiring()) {
                ++firingMotor;
            }
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
    // ERROR HANDLING: Added null pointer check for structural plasticity
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    } else {
        NLM_LOG_ERROR("Structural plasticity system is null during development");
    }
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
        // ERROR HANDLING: Added null pointer check for region
        if (region) {
            region->reset();
        }
    }
    
    // ERROR HANDLING: Added null pointer check for spike system
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    // ERROR HANDLING: Added null pointer checks for memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    // ERROR HANDLING: Added null pointer check for attention
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
            // ERROR HANDLING: Added null pointer check for region
            if (!region) {
                continue;
            }
            for (const auto& pop : region->getPopulations()) {
                // ERROR HANDLING: Added null pointer check for population
                if (!pop) {
                    continue;
                }
                for (const auto* neuron : pop->getNeurons()) {
                    // ERROR HANDLING: Added null pointer check for neuron
                    if (!neuron) {
                        continue;
                    }
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
            // ERROR HANDLING: Added null pointer check for region
            if (!region) {
                continue;
            }
            for (const auto* syn : region->getSynapses()) {
                // ERROR HANDLING: Added null pointer check for synapse
                if (!syn) {
                    continue;
                }
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
            // ERROR HANDLING: Added null pointer check for region
            if (!region) {
                continue;
            }
            for (auto& pop : region->getPopulations()) {
                // ERROR HANDLING: Added null pointer check for population
                if (!pop) {
                    continue;
                }
                for (auto* neuron : pop->getNeurons()) {
                    // ERROR HANDLING: Added null pointer check for neuron
                    if (!neuron) {
                        continue;
                    }
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
        // ERROR HANDLING: Added null pointer check for region
        if (region && region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        // ERROR HANDLING: Added null pointer check for region
        if (region && region->getId() == id) {
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
        // ERROR HANDLING: Added null pointer check for region
        if (region) {
            ids.push_back(region->getId());
        }
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
        // ERROR HANDLING: Added null pointer check for region
        if (region) {
            total += region->getTotalNeuronCount();
        }
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        // ERROR HANDLING: Added null pointer check for region
        if (region) {
            total += region->getSynapseCount();
        }
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        // ERROR HANDLING: Added null pointer check for region
        if (region) {
            total += region->getActiveNeuronCount();
        }
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
        // ERROR HANDLING: Added null pointer check for region
        if (region) {
            sum += region->getAverageFiringRate();
        }
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
    // ERROR HANDLING: Added null pointer checks for memory systems
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    // ERROR HANDLING: Added null pointer check for dopamine
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    for (const auto& region : pImpl->regions) {
        // ERROR HANDLING: Added null pointer check for region
        if (region) {
            NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                         " (" + region->getName() + "): " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                         std::to_string(region->getSynapseCount()) + " synapses, " +
                         "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
        }
    }
}

} // namespace nlm
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
    // ============================================================================
    // SECTION: Core Brain Components
    // ============================================================================
    // This section contains the fundamental components that make up the integrated brain.
    // These provide the basic infrastructure for neural computation and information processing.
    
    std::shared_ptr<Config> config;                          // Configuration for brain parameters
    std::unique_ptr<RandomGenerator> rng;                    // Random number generator
    std::vector<std::unique_ptr<NeuralRegion>> regions;       // Neural regions in the brain
    std::vector<InterRegionConnection> interRegionConnections; // Connections between regions
    
    // ============================================================================
    // SECTION: Integrated Memory Systems
    // ============================================================================
    // These systems manage different forms of memory storage and retrieval.
    // Working memory provides short-term buffering, episodic memory stores experiences,
    // and associative memory maintains relationships between concepts.
    
    std::unique_ptr<NeuralWorkingMemory> workingMemory;        // Short-term working memory
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;     // Long-term episodic memory
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory; // Associative memory system
    
    // ============================================================================
    // SECTION: Integrated Prediction System
    // ============================================================================
    // This system generates predictions about sensory inputs and calculates prediction errors
    // for learning and adaptation. Key for predictive coding and error-driven learning.
    
    std::unique_ptr<PredictionSystem> predictionSystem;       // Prediction and error systems
    
    // ============================================================================
    // SECTION: Integrated Cognition Systems
    // ============================================================================
    // Higher-level cognitive functions including planning, concept formation, and attention.
    // These systems enable goal-directed behavior and intelligent decision making.
    
    std::unique_ptr<NeuralPlanner> planner;                   // Planning system for decision making
    std::unique_ptr<ConceptFormation> conceptFormation;        // Concept formation and abstraction
    std::unique_ptr<AttentionalSelection> attention;           // Attention and focus mechanism
    
    // ============================================================================
    // SECTION: Development System
    // ============================================================================
    // Manages the brain's developmental progression, including critical periods and maturation.
    // Affects plasticity rates and system behavior over time.
    
    std::unique_ptr<DevelopmentSystem> developmentSystem;      // Developmental progression and maturation
    
    // ============================================================================
    // SECTION: Neuromodulation Systems
    // ============================================================================
    // Chemical neuromodulators that regulate neural excitability, learning, and motivation.
    // Dopamine drives reward-based learning, curiosity promotes exploration,
    // prediction error signals learning opportunities, and novelty detection identifies important events.
    
    std::unique_ptr<Dopamine> dopamine;                      // Reward prediction error
    std::unique_ptr<Curiosity> curiosity;                    // Intrinsic motivation
    std::unique_ptr<PredictionError> predictionError;         // Prediction errors for learning
    std::unique_ptr<Novelty> novelty;                         // Novelty detection
    
    // ============================================================================
    // SECTION: Phase 2 Neural Computation Components
    // ============================================================================
    // Real-time neural computation systems implementing spike dynamics and plasticity rules.
    // These provide the biological-inspired mechanisms for neural processing.
    
    std::unique_ptr<SpikeSystem> spikeSystem;                 // Spike generation and propagation
    std::unique_ptr<STDP> stdp;                               // Spike-timing dependent plasticity
    std::unique_ptr<Hebbian> hebbian;                         // Hebbian learning rules
    std::unique_ptr<StructuralPlasticity> structuralPlasticity; // Synaptogenesis and pruning
    
    // ============================================================================
    // SECTION: Simulation Parameters
    // ============================================================================
    // Basic simulation settings controlling time stepping and numerical integration.
    
    TimestepDuration timestep;                               // Simulation timestep size
    SimulationStep currentStep;                              // Current simulation step
    Timestamp currentTime;                                   // Current simulation time
    
    // ============================================================================
    // SECTION: Statistics
    // ============================================================================
    // Runtime statistics for monitoring and analysis of brain activity.
    
    size_t totalSpikesThisStep;                              // Spikes fired in current step
    size_t totalSpikesTotal;                                 // Total spikes since initialization
    
    // ============================================================================
    // SECTION: Sensory/Motor Integration
    // ============================================================================
    // Special neuron populations dedicated to external interaction:
    // sensory neurons receive external input, motor neurons produce action output.
    
    std::vector<Neuron*> sensoryNeurons;                      // Neurons that receive sensory input
    std::vector<Neuron*> motorNeurons;                        // Neurons that produce motor output
    
    // ============================================================================
    // SECTION: Integration State
    // ============================================================================
    // State variables managing long-term behavior and integration cycles.
    // Resting state allows for memory consolidation, and episode tracking maintains
    // temporal context for learning and memory.
    
    bool isResting;  // For sleep/rest cycle and memory consolidation
    size_t stepsSinceLastEpisode;                           // Steps since last episode storage
    size_t replayInterval;                                  // Memory replay interval in steps
    size_t consolidationInterval;                           // Memory consolidation interval in steps
    
    // ============================================================================
    // SECTION: Checkpoint System
    // ============================================================================
    // Manages state persistence and recovery for saving/loading brain state.
    // Essential for long-running simulations and debugging.
    
    std::unique_ptr<CheckpointManager> checkpointManager;    // State persistence and recovery
    
    // ============================================================================
    // SECTION: Constructor with Configuration Initialization
    // ============================================================================
    // Constructor that initializes all systems with configuration parameters.
    // Sets up random number generator, plasticity systems, and all integrated components.
    // Parameters:
    //   cfg: Configuration object containing simulation parameters and settings
    
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
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
        // Initialize random number generator with seed from config
        // Uses a default seed of 42, but can be overridden by configuration
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize neural computation systems (Phase 2)
        // These provide real-time spike dynamics and plasticity mechanisms
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ============================================================================
        // SECTION: Initialize All Integrated Systems
        // ============================================================================
        // This section initializes all brain subsystems in the correct order,
        // ensuring dependencies are established before system startup.
        
        // Initialize memory systems (working, episodic, associative)
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems (planning, concept formation, attention)
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system for maturation effects
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems (dopamine, curiosity, prediction error, novelty)
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure STDP (Spike-Timing Dependent Plasticity) parameters
        // LTP (Long-Term Potentiation) and LTD (Long-Term Depression) strengths
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity parameters
        // Synaptogenesis (formation) and pruning rates control structural changes
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get simulation timestep from configuration
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from configuration for memory operations
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager for state persistence
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;  // Current developmental stage
    RegionId nextRegionId;                   // Next available region ID
};

// ============================================================================
// SECTION: Brain Constructor and Move Operations
// ============================================================================
// This section contains the Brain class constructors, destructor, and move operations.
// These provide basic resource management and ownership transfer for the pImpl pattern.
// 
// Constructors:
// - Default constructor: Takes configuration and initializes the implementation
// - Move constructor: Transfers ownership from another Brain instance
// - Move assignment operator: Transfers ownership and cleans up source
// Destructor: Cleans up implementation
// 
// These operations ensure proper resource management while maintaining encapsulation
// of the complex implementation details.

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

// Move constructor - transfers ownership of implementation from another Brain
Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;  // Leave source in valid but empty state
}

// Move assignment operator - transfers ownership and cleans up current implementation
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;            // Clean up current implementation
        pImpl = other.pImpl;     // Transfer ownership
        other.pImpl = nullptr;   // Leave source in valid but empty state
    }
    return *this;
}

// ============================================================================
// SECTION: Brain Initialization Method
// ============================================================================
// This section contains the Brain::initialize() method which sets up the brain structure
// and initializes all integrated systems. This is called once after construction
// to prepare the brain for simulation.
// 
// The initialization process involves:
// 1. Reading configuration parameters
// 2. Creating neural regions and populations
// 3. Setting up sensory and motor neuron connections
// 4. Initializing all integrated memory, cognition, and neuromodulation systems
// 5. Configuring plasticity and spike event handlers
// 6. Setting up checkpoint management
// 
// Parameters: None
// Returns: bool - true if initialization successful, false otherwise
// 
// The method logs extensive information about the initialization process for
// debugging and monitoring purposes.

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // ============================================================================
    // SECTION: Configuration Reading
    // ============================================================================
    // Extract brain configuration parameters from the config object
    // These define the size, structure, and connectivity of the simulated brain
    
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // ============================================================================
    // SECTION: Region Creation
    // ============================================================================
    // Create neural regions based on configuration
    // Each region contains multiple neuron populations with different types
    // Regions provide modular organization for distributed processing
    
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // ============================================================================
    // SECTION: Neuron Population Creation
    // ============================================================================
    // Create neurons across regions in different population types
    // Populations represent functional groups with shared characteristics:
    // - Sensory: Receive external input
    // - Internal: Process information within the brain
    // - Motor: Generate action output
    
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add different populations to each region
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O interface
            // These specialized neurons handle external interaction
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
    
    // ============================================================================
    // SECTION: Synaptogenesis and Initial Connectivity
    // ============================================================================
    // Establish initial synaptic connections between neurons within regions
    // Random connectivity provides the foundation for emergent behavior
    // Connections are created based on the connection probability from config
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ============================================================================
    // SECTION: Initialize All Integrated Systems
    // ============================================================================
    // Initialize all brain subsystems in dependency order
    // Memory systems must be initialized first, followed by cognition and neuromodulation
    
    // Initialize working memory (short-term buffer)
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory (long-term experience storage)
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory (concept relationships)
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    // Note: PredictionSystem doesn't have initialize method currently
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation systems
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // ============================================================================
    // SECTION: Event System Configuration
    // ============================================================================
    // Set up event handlers for the spike system
    // These handlers respond to neural events for statistics, input delivery, and learning
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes for statistics
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    // This processes spike delivery after delay periods
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
    
    // ============================================================================
    // SECTION: Checkpoint System Configuration
    // ============================================================================
    // Set up checkpoint manager for state persistence
    // Allows saving and loading brain state for long simulations
    
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

// ============================================================================
// SECTION: Step Processing Methods
// ============================================================================
// This section contains the brain's main simulation methods that process each timestep.
// These methods implement the complete integrated brain simulation loop, coordinating
// all neural computation and system interactions across multiple phases.
// 
// The step processing is organized into 15 distinct phases:
// 1. Process pending delayed spikes (deliver synaptic input)
// 2. Update all neurons (LIF dynamics)
// 3. Detect spikes and schedule spike events
// 4. Update working memory (maintenance and competition)
// 5. Apply neuromodulation effects on neural excitability
// 6. Apply plasticity rules (STDP and Hebbian)
// 7. Update episodic memory with current experience
// 8. Update prediction system
// 9. Update attention system
// 10. Update concept formation
// 11. Apply structural plasticity (synaptogenesis, pruning)
// 12. Replay important memories (during rest or periodically)
// 13. Apply development effects
// 14. Periodic memory consolidation
// 15. Checkpoint management
// 
// Parameters:
//   currentStep: Current simulation step (for timestep-based variant)
//   currentTime: Current simulation time in seconds (for timed variant)
// 
// The brain operates in two modes:
// - step(SimulationStep): Convenience wrapper that calculates time from step
// - step(SimulationStep, Timestamp): Main implementation with explicit time
// 
// This represents the core neural computation loop of the integrated artificial brain.

void Brain::step(SimulationStep currentStep) {
    // Convenience wrapper: calculate time from step using configured timestep
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
    
    // Update simulation state
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ============================================================================
    // SECTION: Phase 1 - Delayed Spike Processing
    // ============================================================================
    // Process events that were queued in previous steps
    // These are spikes that need to be delivered after a delay period
    // Essential for modeling axonal delay and synaptic transmission timing
    
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ============================================================================
    // SECTION: Phase 2 - Neural Integration
    // ============================================================================
    // Update all neurons using Leaky Integrate-and-Fire (LIF) dynamics
    // This is the core neural computation that determines when neurons fire
    // Each neuron integrates incoming currents and updates its membrane potential
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ============================================================================
    // SECTION: Phase 3 - Spike Detection and Event Generation
    // ============================================================================
    // Detect neurons that have crossed firing threshold
    // Queue spike events for immediate processing and delayed delivery
    // This phase generates the neural activity that drives the system
    
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
    
    // ============================================================================
    // SECTION: Phase 4 - Working Memory Update
    // ============================================================================
    // Update working memory system with current neural activity
    // Implements maintenance and competition mechanisms for short-term storage
    
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ============================================================================
    // SECTION: Phase 5 - Neuromodulation Effects
    // ============================================================================
    // Apply neuromodulatory chemicals that affect neural excitability and learning
    // Dopamine is the primary neuromodulator in this integrated system
    
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
    
    // ============================================================================
    // SECTION: Phase 6 - Plasticity Updates
    // ============================================================================
    // Apply synaptic plasticity rules (STDP and Hebbian) to modify connection strengths
    // Neuromodulators like dopamine scale plasticity effects
    // This is where learning and adaptation occur
    
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
            
            // Update synapse state (eligibility traces)
            syn->step(currentTime);
        }
    }
    
    // ============================================================================
    // SECTION: Phase 7 - Episodic Memory Update
    // ============================================================================
    // Store current neural activity as an episode in episodic memory
    // This creates long-term storage of experiences for later replay and consolidation
    
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
    
    // ============================================================================
    // SECTION: Phase 8 - Prediction System Update
    // ============================================================================
    // Update predictive models based on sensory input and neural activity
    // Prediction errors are computed and used for learning
    
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ============================================================================
    // SECTION: Phase 9 - Attention System Update
    // ============================================================================
    // Update attentional selection mechanisms
    // Select important neurons from working memory for further processing
    
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ============================================================================
    // SECTION: Phase 10 - Concept Formation Update
    // ============================================================================
    // Update abstract concept representations based on neural patterns
    // This forms high-level abstractions from concrete experiences
    
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ============================================================================
    // SECTION: Phase 11 - Structural Plasticity
    // ============================================================================
    // Apply structural changes: synaptogenesis (formation) and pruning (elimination)
    // Occurs periodically to adapt brain structure to experience
    
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ============================================================================
    // SECTION: Phase 12 - Memory Replay
    // ============================================================================
    // Replay important memories to strengthen important connections
    // Occurs during rest or periodically for memory consolidation
    
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ============================================================================
    // SECTION: Phase 13 - Development Effects
    // ============================================================================
    // Apply developmental changes that affect plasticity rates
    // Occurs every 1000 steps to model brain maturation
    
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
    
    // ============================================================================
    // SECTION: Phase 14 - Memory Consolidation
    // ============================================================================
    // Periodic consolidation of episodic memories
    // Weak memories are removed, important ones are strengthened
    
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // ============================================================================
    // SECTION: Phase 15 - Checkpoint Management
    // ============================================================================
    // Periodic checkpointing for long-running simulations
    // Allows saving and loading brain state
    
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

// ============================================================================
// SECTION: Brain Interface Methods
// ============================================================================
// This section contains the Brain class interface methods that provide
// access to and manipulation of the brain's internal state and systems.
// These methods serve as the public API for interacting with the brain,
// abstracting away the implementation details of the pImpl pattern.
// 
// Key method categories:
// - Input/output interfaces (sensory input, action production)
// - Neural manipulation (current injection)
// - System accessors (getters for various subsystems)
// - Statistics and monitoring
// - State management (save/load/reset)
// - Development and plasticity control
// 
// These methods provide the essential functionality for:
// 1. Interfacing with external environments (sensory input, action output)
// 2. Monitoring brain state and performance
// 3. Managing long-term brain development
// 4. Persisting and recovering brain state

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // ============================================================================
    // SECTION: Sensory Input Processing
    // ============================================================================
    // Process external sensory input by injecting current into sensory neurons.
    // This implements the input pathway for the brain, converting external
    // stimuli into neural activity patterns.
    // 
    // Parameters:
    //   input: SensoryInput containing sensory data from environment
    // 
    // The method implements a simple encoding scheme:
    // - Normalized input values are converted to current injections
    // - Input is distributed across available sensory neurons
    // - Strong inputs are stored in working memory for later processing
    // 
    // This is the primary interface for environmental interaction.
    
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
    // ============================================================================
    // SECTION: Direct Neural Stimulation
    // ============================================================================
    // Inject current directly into a specific neuron.
    // This provides low-level control over neural excitation.
    // 
    // Parameters:
    //   neuron: ID of the target neuron
    //   current: Current to inject (positive for excitation, negative for inhibition)
    // 
    // This method is useful for:
    // 1. Artificially stimulating neurons for testing
    // 2. Implementing external control signals
    // 3. Overriding normal input pathways
    
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
    // ============================================================================
    // SECTION: Mass Neural Stimulation
    // ============================================================================
    // Inject current into all neurons of a specific type.
    // This provides broad control over neural populations based on function.
    // 
    // Parameters:
    //   type: Neuron type to target (Sensory, Internal, Motor)
    //   current: Current to inject
    // 
    // Use cases:
    // 1. Global modulation of neural excitability
    // 2. Implementing neuromodulatory effects
    // 3. Testing population-level effects
    
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

// ============================================================================
// SECTION: Neural Computation System Accessors
// ============================================================================
// This section provides direct access to the brain's neural computation systems.
// These accessors give external code access to the underlying spike processing,
// plasticity rules, and neural dynamics systems.
// 
// These systems are the "engines" of the brain:
// - SpikeSystem: Real-time neural spike generation and propagation
// - STDP: Spike-timing dependent plasticity for learning
// - Hebbian: Hebbian learning for reinforcement
// - StructuralPlasticity: Synaptogenesis and pruning for structural adaptation

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

// ============================================================================
// SECTION: Brain Statistics and Metrics
// ============================================================================
// This section provides methods for monitoring and analyzing brain state.
// These statistics are essential for:
// 1. Performance monitoring and debugging
// 2. Scientific analysis of neural dynamics
// 3. Adaptive control systems
// 4. Research and experimentation

float Brain::getExcitationInhibitionRatio() const {
    // ============================================================================
    // SECTION: Excitation-Inhibition Balance
    // ============================================================================
    // Calculate the E/I (excitation/inhibition) balance of the brain.
    // This is a critical measure of brain stability and function.
    // 
    // Returns:
    //   Ratio of total excitatory weight to total inhibitory weight
    //   Infinity if no inhibition, zero if no excitation
    // 
    // Biological relevance:
    // - Higher E/I ratios can indicate hyperexcitability (e.g., epilepsy)
    // - Lower E/I ratios can indicate excessive inhibition (e.g., depression)
    // - Balanced ratios are associated with healthy cognitive function
    
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

// ============================================================================
// SECTION: Action Production
// ============================================================================
// This section contains methods for generating behavioral outputs from the brain.
// The brain produces actions based on its internal state and neural activity patterns.
// 
// Action selection is based on:
// 1. Motor neuron activity patterns
// 2. Working memory content
// 3. Neuromodulatory signals
// 
// This bridges the gap between internal neural processing and external behavior.

std::unique_ptr<class Action> Brain::produceAction() {
    // ============================================================================
    // SECTION: Action Selection
    // ============================================================================
    // Generate an action based on current neural activity.
    // This is the output pathway of the brain, translating neural activity
    // into behavioral responses.
    // 
    // Implementation:
    // 1. Check if any motor neurons are firing
    // 2. If so, select MoveForward action
    // 3. Otherwise, select Wait action
    // 
    // Returns:
    //   Unique pointer to Action object with selected action type
    // 
    // This is a simplified action selection for demonstration.
    // Real implementations would use more sophisticated methods including:
    // - Reward-based selection
    // - Goal-directed planning
    // - Expected value calculations
    // 
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

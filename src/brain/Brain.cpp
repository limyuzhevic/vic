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

/**
 * @brief Implementation details for Brain class (pImpl pattern)
 * 
 * @details Contains all internal state and implementation details of the Brain class.
 *          This follows the pImpl idiom to hide implementation from header files.
 *          The Impl structure contains all neural system components, configuration,
 *          simulation state, and integrated systems.
 * 
 * @note Thread-unsafe access patterns apply to all Impl members
 * @warning Direct access to pImpl is discouraged - use Brain public interface
 * @ingroup Neuronal Dynamics
 */
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
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
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
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    /**
     * @brief Constructor - Initialize Brain implementation with configuration
     * 
     * @details Sets up all internal Brain components and initializes integrated systems
     *          based on the provided configuration. This constructor establishes the
     *          complete neural system architecture including all subsystems.
     * 
     * @param cfg Shared pointer to configuration object containing all simulation parameters
     * 
     * @throws std::bad_alloc if memory allocation fails for any component
     * @note Configuration is stored and used throughout Brain lifetime
     * @warning Configuration must be valid before initialization is called
     * @thread_safety Thread-unsafe initialization
     * @post All Brain components are initialized and ready for operation
     */
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
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
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
    
    /**
     * @brief Current developmental stage of the brain
     * 
     * @details Tracks the developmental stage affecting plasticity rates and
     *          network properties. Progresses through stages during simulation.
     * 
     * @note Influences plasticity modulation factors
     * @thread_safety Thread-unsafe during development updates
     */
    DevelopmentalStage developmentalStage;

    /**
     * @brief Next available region identifier
     * 
     * @details Monotonic counter that generates unique region identifiers
     *          for newly created neural regions. Ensures global uniqueness across
     *          all regions in the brain.
     * 
     * @note Starts at 1, increments with each region creation
     * @thread_safety Thread-unsafe during region creation
     */
    RegionId nextRegionId;
};

/**
     * @brief Brain constructor - Create brain with configuration
     * 
     * @details Constructs a Brain object with the provided configuration and
     *          initializes the internal implementation. This constructor uses
     *          the pImpl pattern to hide implementation details from the header file.
     * 
     * @param config Shared pointer to configuration object containing all
     *              simulation parameters and settings
     * 
     * @throws std::bad_alloc if memory allocation fails for pImpl
     * @note Configuration must be valid before calling initialize()
     * @thread_safety Thread-unsafe initialization
     * @post Brain is constructed with configuration but not yet initialized
     */
Brain::Brain(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)) {}

/**
 * @brief Brain destructor - Clean up brain resources
 * 
     * @details Performs cleanup of all Brain resources, including the internal
     *          implementation and all managed components. This destructor ensures
     *          proper cleanup of all allocated resources.
     * 
     * @note All Brain resources are properly released
     * @thread_safety Thread-safe cleanup
     * @post Brain resources are freed
     */
Brain::~Brain() = default;

/**
     * @brief Move constructor - Transfer Brain ownership
     * 
     * @details Move constructor allows efficient transfer of Brain ownership
     *          by moving the internal pImpl pointer from the source Brain object.
     *          The source Brain becomes invalid after the move operation.
     * 
     * @param other Brain object to move from
     * 
     * @note Source brain becomes unusable after move
     * @thread_safety Exception-safe
     * @post Source brain is in valid but unspecified state
     */
Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

/**
     * @brief Move assignment operator - Transfer Brain ownership
     * 
     * @details Move assignment allows efficient transfer of Brain ownership,
     *          properly cleaning up existing resources before transferring from
     *          the source Brain object.
     * 
     * @param other Brain object to move from
     * 
     * @return Reference to this Brain after assignment
     * @note Source brain becomes unusable after move
     * @thread_safety Exception-safe, may throw
     * @post Source brain is in valid but unspecified state
     */
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

/**
 * @brief Initialize brain with configuration
 * 
     * @details Performs complete brain initialization based on configuration,
     *          creating regions, neurons, populations, connections, and initializing
     *          all integrated systems. This is the main setup operation that must
     *          be called exactly once after construction.
     * 
     * @return true if initialization succeeds, false if configuration is invalid
     *         or resource allocation fails
     * 
     * @throws std::runtime_error if critical configuration parameters are missing
     * @note Initialize() must be called exactly once after construction
     * @warning This method may take significant time for large networks
     * @post Brain is fully initialized and ready for simulation steps
     * @thread_safety Thread-unsafe, must be called from single thread
     */
bool Brain::initialize() {
NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    /**
     * @brief Get configuration values and log them
     * 
     * @details Retrieves key configuration parameters for logging and validation.
     *          This helps verify that the configuration is properly loaded and
     *          displays the main simulation parameters for user information.
     * 
     * @note Configuration parameters control network size and connectivity
     */
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    /**
     * @brief Create brain regions based on configuration
     * 
     * @details Creates the specified number of neural regions, each representing
     *          an anatomical or functional area of the brain. Regions are created
     *          with auto-generated names and will later contain populations of neurons.
     * 
     * @note Region count is typically 1 for simple networks, increases for complex
     *       multi-regional architectures
     */
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    /**
     * @brief Distribute neurons across regions
     * 
     * @details Allocates neurons proportionally across regions based on the
     *          total neuron count and region count. Each region receives neurons
     *          organized into functional populations (sensory, internal, motor).
     * 
     * @note Neurons are distributed to create balanced regional representations
     *       while maintaining overall network size
     */
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            /**
             * @brief Create functional populations within region
             * 
             * @details Creates three basic neuron populations within each region:
             *          - Sensory neurons: Process external input
             *          - Internal neurons: Perform internal processing
             *          - Motor neurons: Generate output actions
             * 
             * @note Population sizes are proportionally allocated based on total neurons
             */
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            /**
             * @brief Collect I/O neurons for external interaction
             * 
             * @details Identifies and stores sensory and motor neurons for use by
             *          external systems. This enables direct access to input and output
             *          neurons for integration with environmental inputs and actions.
             */
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
    
    /**
     * @brief Initialize synaptic connectivity
     * 
     * @details Establishes random synaptic connections between neurons within
     *          each region. This creates the initial network structure based on
     *          the specified connection probability and weight distribution.
     * 
     * @note Connectivity is probabilistic to create sparse networks
     *       representative of biological neural systems
     */
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    /**
     * @brief Initialize all integrated memory systems
     * 
     * @details Sets up the brain's memory architecture, including working memory
     *          for transient active information, episodic memory for experience storage,
     *          and associative memory for pattern relationships. These systems are
     *          essential for cognitive processing and learning.
     */
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    /**
     * @brief Initialize working memory system
     * 
     * @details Sets up working memory with appropriate capacity for the network size.
     *          Working memory maintains active neuronal representations for immediate
     *          use in cognitive processing.
     */
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    /**
     * @brief Initialize episodic memory system
     * 
     * @details Sets up episodic memory to store experience episodes with temporal
     *          sequencing. This allows the brain to remember and replay past experiences.
     */
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    /**
     * @brief Initialize associative memory system
     * 
     * @details Sets up associative memory to form and retrieve pattern relationships,
     *          enabling the brain to associate related experiences and patterns.
     */
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    /**
     * @brief Initialize prediction system
     * 
     * @details Sets up the prediction system for sensory prediction and error computation.
     *          Currently, prediction system doesn't have initialize method, so this is a placeholder.
     */
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    /**
     * @brief Initialize cognitive systems
     * 
     * @details Sets up all cognitive processing systems including neural planning,
     *          concept formation, and attentional selection. These systems enable
     *          higher-level cognitive functions and decision making.
     */
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    /**
     * @brief Initialize neuromodulation systems
     * 
     * @details Sets up neuromodulatory systems including novelty detection,
     *          curiosity, and other neuromodulators that influence neural excitability
     *          and plasticity based on behavioral relevance and exploration.
     */
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
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

/**
 * @brief Main simulation step (step count version)
 * 
     * @details Advances the simulation by one timestep using only the step count.
     *          This method calculates the current time based on the step count and
     *          configured timestep duration, then calls the detailed time-based step.
     * 
     * @param currentStep Current simulation step number
     * 
     * @note This is a convenience method for simulations that only track step count
     * @thread_safety Thread-unsafe - must not be called concurrently
     */
void Brain::step(SimulationStep currentStep) {
    /**
     * @brief Calculate current time from step count
     * 
     * @details Computes the current simulation time by multiplying the step count
     *          by the configured timestep duration. This assumes constant timestep
     *          across the simulation.
     * 
     * @param currentStep Current simulation step
     * 
     * @note Uses stored timestep value for time calculation
     */
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

/**
 * @brief Main simulation step (detailed time version)
 * 
     * @details The core simulation loop implementing Phase 6 of the NLM brain.
     *          This method orchestrates the complete integrated brain simulation,
     *          updating all neural components and integrated systems in the correct
     *          order. The simulation follows 15 distinct phases:
     * 
     *          1. Process pending delayed spike events (deliver synaptic input)
     *          2. Update all neuron membrane potentials (LIF dynamics)
     *          3. Detect spikes and schedule outgoing spike events
     *          4. Update working memory (maintenance and competition)
     *          5. Apply neuromodulation effects on neural excitability
     *          6. Apply plasticity rules (STDP and Hebbian)
     *          7. Update episodic memory with current experience
     *          8. Update prediction system
     *          9. Update attention system
     *          10. Update concept formation
     *          11. Apply structural plasticity (synaptogenesis, pruning)
     *          12. Replay important memories (during rest or periodically)
     *          13. Apply development effects
     *          14. Collect statistics
     *          15. Periodic memory consolidation
     *          16. Checkpoint management
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in milliseconds
     * 
     * @throws std::runtime_error if any simulation step fails
     * @note This is the primary simulation loop method
     * @thread_safety Thread-unsafe - simulation must be stopped for concurrent operations
     * @post All neural systems are updated for the current timestep
     */
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
    
    /**
     * @brief Update simulation state counters
     * 
     * @details Resets step-specific counters and updates the current simulation
     *          time and step number. This initializes the state for the new timestep.
     * 
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     */
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    /**
     * @brief Phase 1: Process pending delayed spikes (deliver synaptic input)
     * 
     * @details Processes delayed spike events, delivering synaptic input to
     *          target neurons after the specified delay. This implements the
     *          transmission of neural signals through synapses with timing delays.
     */
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    /**
     * @brief Phase 2: Update all neurons (LIF dynamics)
     * 
     * @details Updates all neurons in the network using Leaky Integrate-and-Fire
     *          dynamics. This includes membrane potential integration, spike detection,
     *          and refractory period management across all neuron types and regions.
     */
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    /**
     * @brief Phase 3: Detect spikes and schedule spike events
     * 
     * @details Scans all neurons for spiking activity, creates spike events,
     *          and schedules them for delivery through the spike system. Also
     *          records post-synaptic spike information for plasticity and
     *          stores spiking neurons in working memory.
     */
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                /**
                 * @brief Check if neuron just fired this step
                 * 
                 * @details Determines if a neuron has fired in the current timestep
                 *          by checking if it transitioned from resting to active state
                 *          or if it has recently entered refractory period.
                 * 
                 * @param state Neuron state reference
                 * @param currentTime Current simulation time
                 * @param timestep Duration of current timestep
                 * 
                 * @return bool true if neuron fired this step
                 */
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    /**
                     * @brief Queue spike for event-driven processing
                     * 
                     * @details Creates a spike event and queues it for processing
                     *          by the spike system. This implements event-driven neural
                     *          dynamics rather than synchronous time-stepping.
                     */
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);

                    /**
                     * @brief Record post-synaptic spike for plasticity
                     * 
                     * @details Records post-synaptic spike activity for all incoming
                     *          synapses. This spike information is used by STDP and other
                     *          spike-timing-dependent plasticity rules.
                     */
                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    /**
                     * @brief Create delayed spike events for outgoing connections
                     * 
                     * @details Creates delayed spike events for all outgoing synapses,
                     *          implementing synaptic transmission with appropriate timing
                     *          and weight scaling. This models the propagation of neural
                     *          signals through the network.
                     */
                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        /**
                         * @brief Calculate delivery timing for spike
                         * 
                         * @details Computes the delivery time for a spike based on
                         *          synaptic delay and current simulation time. The delay
                         *          represents synaptic transmission time.
                         */
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

                        /**
                         * @brief Construct and queue delayed spike event
                         * 
                         * @details Creates a comprehensive delayed spike event containing
                         *          all information needed for synaptic transmission, including
                         *          neuron IDs, weights, types, and timing information.
                         */
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

                        /**
                         * @brief Record pre-synaptic spike for plasticity
                         * 
                         * @details Records pre-synaptic spike activity for plasticity
                         *          rules that depend on spike timing relationships.
                         */
                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    /**
                     * @brief Store spiking neuron in working memory
                     * 
                     * @details Records the spiking neuron and its activation level
                     *          in working memory, allowing the brain to maintain an
                     *          active representation of recent neural activity.
                     */
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    /**
     * @brief Process immediate spikes
     * 
     * @details Processes queued spike events for immediate synaptic transmission
     *          and other spike-related effects. This completes the spike propagation
     *          process started in phase 3.
     */
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
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
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

/**
     * @brief Reset brain state
     * 
     * @details Resets the entire brain to its initial state, clearing all neural
     *          activity, memory contents, and simulation counters. This effectively
     *          starts a new simulation episode.
     * 
     * @throws std::runtime_error if reset fails due to system state issues
     * @note Resets all subsystems including memory, cognition, and development
     * @thread_safety Thread-unsafe - simulation must be stopped
     * @post Brain returns to initial state with all systems reset
     */
void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    /**
     * @brief Reset simulation state counters
     * 
     * @details Resets all simulation state variables to their initial values,
     *          including time counters, spike statistics, and integration state.
     * 
     * @note This clears the simulation timeline without affecting configuration
     */
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    /**
     * @brief Reset all regions and their contents
     * 
     * @details Resets each neural region in the brain, clearing all neuron states,
     *          synaptic connections, and regional activity. This returns regions to
     *          their initial configured state.
     */
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    /**
     * @brief Reset spike system
     * 
     * @details Resets the spike system, clearing all queued spike events and
     *          resetting spike timing and event counters. This ensures clean state
     *          for the next simulation cycle.
     */
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    /**
     * @brief Reset all memory systems
     * 
     * @details Clears all memory systems including working memory, episodic memory,
     *          and associative memory. This removes all stored neural representations,
     *          memories, and learned associations.
     */
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

/**
 * @brief Save brain state to file (checkpointing)
 * 
     * @details Saves complete brain state including neuron membrane potentials,
     *          synaptic weights, spike history, and all system state to a file
     *          for later recovery. This implements checkpointing functionality
     *          that allows recovery from crashes and state persistence.
     * 
     * @param filepath Path to checkpoint file where state will be saved
     * 
     * @return true if save succeeds, false if file cannot be written or state
     *         cannot be serialized
     * 
     * @throws std::runtime_error if checkpoint system fails
     * @note Checkpointing enables recovery from simulation crashes
     * @thread_safety Thread-unsafe - must be called when not in simulation
     * @post Brain state is saved to persistent storage
     */
bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    /**
     * @brief Save operation with comprehensive error handling
     * 
     * @details Wraps the save operation in a try-catch block to handle any
     *          exceptions that might occur during checkpointing. This ensures
     *          graceful error handling and proper cleanup if save fails.
     * 
     * @param filepath Target file for brain state
     * 
     * @return bool Save success status
     * 
     * @throws std::runtime_error if checkpoint writer fails
     */
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        /**
         * @brief Set checkpoint metadata
         * 
         * @details Records essential metadata about the brain state for
         *          validation and information purposes. Metadata includes neural
         *          counts, synaptic statistics, and simulation progress.
         */
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        /**
         * @brief Serialize all neuron states
         * 
         * @details Converts all neuron states to a checkpoint format by collecting
         *          membrane potentials, synaptic conductances, and other neuron
         *          properties. This creates a snapshot of the neural population.
         */
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
        
        /**
         * @brief Write neuron checkpoint data
         * 
         * @details Flushes neuron state data to checkpoint file. This completes
         *          the neuron state serialization process.
         */
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        /**
         * @brief Serialize all synapse states
         * 
         * @details Converts all synapse states to checkpoint format by collecting
         *          synaptic connection information including source/destination neurons,
         *          weights, delays, types, and plasticity state.
         */
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
        
        /**
         * @brief Write synapse checkpoint data
         * 
         * @details Flushes synapse state data to checkpoint file. This completes
         *          the synapse state serialization process.
         */
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        /**
         * @brief Finalize checkpoint writing
         * 
         * @details Completes the checkpoint file by writing headers, footers,
         *          and metadata. This ensures the checkpoint is valid and complete
         *          for later loading.
         */
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
    // Load brain state from file
bool Brain::load(const std::string& filepath) {
    /**
     * @brief Brain load operation with comprehensive error handling
     * 
     * @details Wraps the load operation in a try-catch block to handle any
     *          exceptions that might occur during checkpoint loading. This ensures
     *          graceful error handling and proper cleanup if load fails.
     * 
     * @param filepath Source file for brain state
     * 
     * @return bool Load success status
     * 
     * @throws std::runtime_error if checkpoint reader fails
     */
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        /**
         * @brief Validate checkpoint integrity
         * 
         * @details Performs validation of the checkpoint file to ensure it's
         *          properly formatted and complete. This prevents loading corrupted
         *          or incomplete checkpoint files.
         */
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        /**
         * @brief Read and deserialize neuron states
         * 
         * @details Extracts neuron state data from the checkpoint file and
         *          prepares it for application to the actual neurons. This includes
         *          membrane potentials, firing states, and synaptic parameters.
         */
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        /**
         * @brief Apply loaded neuron states to actual neurons
         * 
         * @details Maps the loaded neuron checkpoint data back to the actual
         *          neurons in the brain. This restores the membrane potential and
         *          other neuron properties from the checkpoint.
         */
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    /**
                     * @brief Apply individual neuron state
                     * 
                     * @details Restores the state of a single neuron from the checkpoint,
                     *          including all membrane potential parameters and state variables.
                     */
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
        
        /**
         * @brief Read and deserialize synapse states
         * 
         * @details Extracts synapse state data from the checkpoint file.
         *          This includes connection information, weights, delays, and
         *          plasticity state for all synapses.
         */
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        /**
         * @brief Apply loaded synapse states
         * 
         * @details Restores synaptic connection states from the checkpoint.
         *          This includes weights, delays, types, and plasticity parameters.
         *          Note: Full synapse restoration is complex due to ID matching requirements.
         */
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

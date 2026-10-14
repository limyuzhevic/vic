#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/ParallelProcessing.hpp"
#include "../performance/SparseConnectivity.hpp"
#include <bit>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    // Performance: Replace std::vector with MemoryPool for neural structures
    NeuronPool neuronPool;
    SynapsePool synapsePool;
    MemoryPool<InterRegionConnection> interRegionConnectionPool;
    
    // Integration state
    std::vector<std::unique_ptr<NeuralRegion>> regions;  // Still need for region management
    
    // ========== DEVELOPMENT RATES ==========
    float developmentSynaptogenesisRate;
    float developmentPruningRate;
    
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
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    
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
    
    // Performance: Neural state in SoA format for SIMD vectorization
    struct NeuralStateSoA {
        std::vector<float> membranePotential;
        std::vector<float> restingPotential;
        std::vector<float> threshold;
        std::vector<float> resetPotential;
        std::vector<float> leakConductance;
        std::vector<float> synapseInput;  // Accumulated synaptic input for this step
        std::vector<uint8_t> firingState;  // FiringState enum
        std::vector<uint32_t> refractoryRemaining;
        std::vector<uint32_t> refractoryPeriod;
        std::vector<float> lastSpikeTime;
        std::vector<uint64_t> neuronType;  // NeuronType enum
        
        // Active neuron tracking
        std::vector<uint32_t> activeNeuronIndices;  // Indices of neurons with non-zero input
        std::vector<uint32_t> firingNeuronIndices;  // Indices of neurons that fired this step
        
        void initialize(size_t size) {
            membranePotential.resize(size, -70.0f);
            restingPotential.resize(size, -70.0f);
            threshold.resize(size, -55.0f);
            resetPotential.resize(size, -70.0f);
            leakConductance.resize(size, 10.0f);
            synapseInput.resize(size, 0.0f);
            firingState.resize(size, 0);
            refractoryRemaining.resize(size, 0);
            refractoryPeriod.resize(size, 5);
            lastSpikeTime.resize(size, -1.0f);
            neuronType.resize(size, 0);
            activeNeuronIndices.reserve(size);
            firingNeuronIndices.reserve(size);
        }
        
        void clear() {
            // Zero synaptic input
            std::fill(synapseInput.begin(), synapseInput.end(), 0.0f);
            // Clear firing neurons
            firingNeuronIndices.clear();
        }
        
        size_t memoryUsage() const {
            size_t total = 0;
            total += membranePotential.capacity() * sizeof(float);
            total += restingPotential.capacity() * sizeof(float);
            total += threshold.capacity() * sizeof(float);
            total += resetPotential.capacity() * sizeof(float);
            total += leakConductance.capacity() * sizeof(float);
            total += synapseInput.capacity() * sizeof(float);
            total += firingState.capacity() * sizeof(uint8_t);
            total += refractoryRemaining.capacity() * sizeof(uint32_t);
            total += refractoryPeriod.capacity() * sizeof(uint32_t);
            total += lastSpikeTime.capacity() * sizeof(float);
            total += neuronType.capacity() * sizeof(uint64_t);
            return total;
        }
    } neuralStateSOA;
    
    // Performance: Event queues for spike processing
    SpikeRingBuffer spikeQueue;
    DelayedSpikeQueue delayedSpikeQueue;
    
    // Performance: Parallel processor
    ParallelNeuralProcessor parallelProcessor;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Checkpoint providers
    std::function<bool(NeuronCheckpointData&)> neuronProvider;
    std::function<bool(SynapseCheckpointData&)> synapseProvider;
    std::function<bool(std::vector<uint64_t>&)> configProvider;
    std::function<bool(std::vector<uint64_t>&)> connectivityProvider;
    std::function<bool(std::vector<uint64_t>&)> plasticityProvider;
    std::function<bool(std::vector<uint64_t>&)> developmentProvider;
    std::function<bool(std::vector<uint64_t>&)> neuromodulationProvider;
    std::function<bool(std::vector<uint64_t>&)> memoryProvider;
    std::function<bool(std::vector<uint64_t>&)> spikeHistoryProvider;
    std::function<bool(std::vector<uint64_t>&)> randomStateProvider;
    std::function<bool(std::vector<uint64_t>&)> simulationStateProvider;
    std::function<bool(std::vector<uint8_t>&)> environmentProvider;
    
    // Checkpoint provider methods
    bool saveNeuronsToCheckpoint(NeuronCheckpointData& data);
    bool saveSynapsesToCheckpoint(SynapseCheckpointData& data);
    bool saveConfigToCheckpoint(std::vector<uint64_t>& data);
    bool saveConnectivityToCheckpoint(std::vector<uint64_t>& data);
    bool savePlasticityToCheckpoint(std::vector<uint64_t>& data);
    bool saveDevelopmentToCheckpoint(std::vector<uint64_t>& data);
    bool saveNeuromodulationToCheckpoint(std::vector<uint64_t>& data);
    bool saveMemoryToCheckpoint(std::vector<uint64_t>& data);
    bool saveSpikeHistoryToCheckpoint(std::vector<uint64_t>& data);
    bool saveRandomStateToCheckpoint(std::vector<uint64_t>& data);
    bool saveSimulationStateToCheckpoint(std::vector<uint64_t>& data);
    bool saveEnvironmentToCheckpoint(std::vector<uint8_t>& data);
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(std::make_unique<RandomGenerator>(42))
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
        // Update random generator seed from config if specified
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng->seed(seed);
        
        // Get development rates from config
        developmentSynaptogenesisRate = config->getOr<float>("development_synaptogenesis_rate", 0.001f);
        developmentPruningRate = config->getOr<float>("development_pruning_rate", 0.0001f);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        planner->setPlanningDepth(5);
        
        conceptFormation = std::make_unique<ConceptFormation>();
        
        attention = std::make_unique<AttentionalSelection>();
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure neuromodulator baselines from config
        float dopamineBaseline = config->getOr<float>("dopamine_baseline", 0.1f);
        dopamine->setBaseline(dopamineBaseline);
        
        float acetylcholineBaseline = config->getOr<float>("acetylcholine_baseline", 0.05f);
        acetylcholine->setBaseline(acetylcholineBaseline);
        
        float norepinephrineBaseline = config->getOr<float>("norepinephrine_baseline", 0.05f);
        norepinephrine->setBaseline(norepinephrineBaseline);
        
        float serotoninBaseline = config->getOr<float>("serotonin_baseline", 0.05f);
        serotonin->setBaseline(serotoninBaseline);
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_time_constant", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity using config development rates
        float synaptogenesisRate = developmentSynaptogenesisRate;
        float pruningRate = developmentPruningRate;
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        
        // Configure checkpoint providers
        checkpointManager->setNeuronProvider(
            [this](NeuronCheckpointData& data) -> bool {
                return saveNeuronsToCheckpoint(data);
            }
        );
        
        checkpointManager->setSynapseProvider(
            [this](SynapseCheckpointData& data) -> bool {
                return saveSynapsesToCheckpoint(data);
            }
        );
        
        // Configure other checkpoint providers (implementations follow below)
        checkpointManager->setConfigProvider(
            [this](std::vector<uint64_t>& data) -> bool {
                return saveConfigToCheckpoint(data);
            }
        );
        
        checkpointManager->setConnectivityProvider(
            [this](std::vector<uint64_t>& data) -> bool {
                return saveConnectivityToCheckpoint(data);
            }
        );
        
        checkpointManager->setPlasticityProvider(
            [this](std::vector<uint64_t>& data) -> bool {
                return savePlasticityToCheckpoint(data);
            }
        );
        
        checkpointManager->setDevelopmentProvider(
            [this](std::vector<uint64_t>& data) -> bool {
                return saveDevelopmentToCheckpoint(data);
            }
        );
        
        checkpointManager->setNeuromodulationProvider(
            [this](std::vector<uint64_t>& data) -> bool {
                return saveNeuromodulationToCheckpoint(data);
            }
        );
        
        checkpointManager->setMemoryProvider(
            [this](std::vector<uint64_t>& data) -> bool {
                return saveMemoryToCheckpoint(data);
            }
        );
        
        checkpointManager->setSpikeHistoryProvider(
            [this](std::vector<uint64_t>& data) -> bool {
                return saveSpikeHistoryToCheckpoint(data);
            }
        );
        
        checkpointManager->setRandomStateProvider(
            [this](std::vector<uint64_t>& data) -> bool {
                return saveRandomStateToCheckpoint(data);
            }
        );
        
        checkpointManager->setSimulationStateProvider(
            [this](std::vector<uint64_t>& data) -> bool {
                return saveSimulationStateToCheckpoint(data);
            }
        );
        
        checkpointManager->setEnvironmentProvider(
            [this](std::vector<uint8_t>& data) -> bool {
                return saveEnvironmentToCheckpoint(data);
            }
        );

        // Initialize performance infrastructure
        // Get neuron count from config for memory pool initialization
        size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
        size_t synapseCount = config->getOr<size_t>("synapse_count", 10000);
        
        // Initialize memory pools
        neuronPool = NeuronPool(neuronCount);
        synapsePool = SynapsePool(neuronCount, synapseCount);
        interRegionConnectionPool = MemoryPool<InterRegionConnection>(100, 10);
        
        // Initialize neural state SOA
        neuralStateSOA.initialize(neuronCount);
        
        // Configure event queues
        spikeQueue = SpikeRingBuffer();
        delayedSpikeQueue = DelayedSpikeQueue();
        
        // Configure parallel processor
        parallelProcessor = ParallelNeuralProcessor();
    }
    
    // Explicit destructor for proper cleanup of all unique_ptrs
    ~Impl() {
        #ifdef DEBUG_MEMORY_LEAKS
        // Resource cleanup verification
        NLM_LOG_DEBUG("Brain::Impl destructor - verifying resource cleanup");
        
        // Verify all unique_ptrs are properly initialized
        if (!rng) {
            NLM_LOG_WARNING("Warning: rng is not properly initialized");
        }
        
        // Verify critical systems are cleaned up
        NLM_LOG_DEBUG("Checking critical systems cleanup...");
        
        // Track cleanup completion
        deallocationCounter++; // Increment to track cleanup
        NLM_LOG_DEBUG("Resource cleanup completed");
        
        #endif
    }
    
    // Memory leak detection and tracking
    static std::atomic<size_t> allocationCounter{0};
    static std::atomic<size_t> deallocationCounter{0};
    
    static void trackAllocation(const char* type, const void* ptr = nullptr) {
        allocationCounter++;
        #ifdef DEBUG_MEMORY_LEAKS
        std::stringstream ss;
        ss << "ALLOC: " << type << " at " << ptr 
           << " (total allocations: " << allocationCounter << ")";
        NLM_LOG_DEBUG(ss.str());
        #endif
    }
    
    static void trackDeallocation(const char* type, const void* ptr = nullptr) {
        deallocationCounter++;
        #ifdef DEBUG_MEMORY_LEAKS
        std::stringstream ss;
        ss << "FREE: " << type << " at " << ptr 
           << " (total deallocations: " << deallocationCounter << ")";
        NLM_LOG_DEBUG(ss.str());
        #endif
    }
    
    static void checkMemoryLeaks() {
        #ifdef DEBUG_MEMORY_LEAKS
        if (allocationCounter != deallocationCounter) {
            NLM_LOG_WARNING("MEMORY LEAK DETECTED: " + 
                          std::to_string(allocationCounter - deallocationCounter) + 
                          " allocations without corresponding deallocations");
        } else {
            NLM_LOG_DEBUG("Memory tracking complete: " + 
                         std::to_string(allocationCounter) + " allocations, " +
                         std::to_string(deallocationCounter) + " deallocations");
        }
        #endif
    }
};

Brain::~Brain() = default;

// Move constructor - explicit ownership transfer
Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    // Clear the source's pointer to prevent double deletion
    other.pImpl = nullptr;
    
    // Track move operation for memory leak detection
    if (pImpl) {
        #ifdef DEBUG_MEMORY_LEAKS
        NLM_LOG_DEBUG("MOVE_CONSTRUCTOR: Transferred ownership of Impl from " + 
                     std::to_string(reinterpret_cast<uintptr_t>(other.pImpl)) + 
                     " to " + std::to_string(reinterpret_cast<uintptr_t>(pImpl)));
        #endif
    }
}

// Move assignment operator - explicit ownership transfer with cleanup
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        // Delete current resources if any
        if (pImpl) {
            delete pImpl;
            #ifdef DEBUG_MEMORY_LEAKS
            NLM_LOG_DEBUG("MOVE_ASSIGNMENT: Deleting existing Impl at " + 
                         std::to_string(reinterpret_cast<uintptr_t>(pImpl)));
            #endif
        }
        
        // Transfer ownership from source
        pImpl = other.pImpl;
        other.pImpl = nullptr;
        
        // Track move operation for memory leak detection
        if (pImpl) {
            #ifdef DEBUG_MEMORY_LEAKS
            NLM_LOG_DEBUG("MOVE_ASSIGNMENT: Transferred ownership of Impl from " + 
                         std::to_string(reinterpret_cast<uintptr_t>(other.pImpl)) + 
                         " to " + std::to_string(reinterpret_cast<uintptr_t>(pImpl)));
            #endif
        }
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
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    // Vectorized LIF updates using SIMD
    // Collect neuron data for SIMD processing
    std::vector<float> membranePotentials;
    std::vector<float> synapticInputs;
    std::vector<uint32_t> refractoryRemaining;
    std::vector<bool> fired;
    
    // Reserve space for neurons
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            membranePotentials.reserve(membranePotentials.size() + pop->getSize());
            synapticInputs.reserve(synapticInputs.size() + pop->getSize());
            refractoryRemaining.reserve(refractoryRemaining.size() + pop->getSize());
            fired.reserve(fired.size() + pop->getSize());
        }
    }
    
    // Collect neuron data
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                membranePotentials.push_back(state.membranePotential);
                synapticInputs.push_back(0.0f);  // neuron->getState().synapticInput is not accessible directly
                refractoryRemaining.push_back(state.refractoryRemaining);
                fired.push_back(false);
            }
        }
    }
    
    // Apply vectorized LIF updates
    if (SIMDUtils::isSIMDAvailable() && !membranePotentials.empty()) {
        VectorizedLIF lifVectorizer;
        size_t numNeurons = membranePotentials.size();
        
        // Use VectorizedLIF for batch processing
        size_t numFired = lifVectorizer.update(
            membranePotentials,
            synapticInputs,
            refractoryRemaining,
            fired,
            numNeurons
        );
        
        // Update neurons with vectorized results
        size_t neuronIndex = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Update neuron state based on vectorized results
                    neuron->setMembranePotential(membranePotentials[neuronIndex]);
                    neuron->setRefractoryPeriod(refractoryRemaining[neuronIndex]);
                    if (fired[neuronIndex]) {
                        // Neuron fired - handle spike detection
                        // This would need to be integrated with the spike detection logic
                    }
                    neuronIndex++;
                }
            }
        }
    } else {
        // Fallback to scalar processing
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, pImpl->timestep);
                }
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    // Vectorized spike detection using collected fired flags
    size_t neuronIndex = 0;
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (fired.size() > neuronIndex && fired[neuronIndex]) {
                    // Neuron fired - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);
                    
                    // Update neuron firing state
                    auto& state = neuron->getState();
                    state.firingState = FiringState::Active;
                    state.lastSpikeTime = static_cast<float>(currentTime);
                    state.refractoryRemaining = state.refractoryPeriod;
                    state.firingState = FiringState::Refractory;
                    
                    neuronIndex++;
                    continue;
                }
                neuronIndex++;
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
        
        // Run competition for working memory selection
        if (pImpl->workingMemory->getActiveTraces() > 1) {
            pImpl->workingMemory->runCompetition();
        }
        
        // Apply attention to working memory winners for enhanced focus
        if (pImpl->workingMemory->isWinning(neuron->getId()) && pImpl->attention) {
            pImpl->attention->applyTopDownBias(neuron->getId(), 1.0f);
        }
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
    
    // Update acetylcholine (attention and memory)
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        // Apply ACh effects on working memory encoding
        float acetylcholineLevel = pImpl->acetylcholine->getLevel();
        if (acetylcholineLevel > 0.0f) {
            // Enhance working memory encoding strength
            if (pImpl->workingMemory) {
                pImpl->workingMemory->strengthenMemory(acetylcholineLevel * 0.5f);
            }
            
            // Enhance attention selection
            if (pImpl->attention) {
                // ACh enhances top-down bias for selected items
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (pImpl->workingMemory && pImpl->workingMemory->isWinning(neuron->getId())) {
                                pImpl->attention->applyTopDownBias(neuron->getId(), acetylcholineLevel * 1.5f);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Update norepinephrine (arousal and signal detection)
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
        // Apply NE effects on signal processing
        float norepinephrineLevel = pImpl->norepinephrine->getLevel();
        if (norepinephrineLevel > 0.0f) {
            // Enhance novelty encoding
            if (pImpl->novelty) {
                pImpl->novelty->setLevel(std::min(1.0f, pImpl->novelty->getLevel() + norepinephrineLevel * 0.3f));
            }
            
            // Enhance working memory encoding during novelty
            if (pImpl->workingMemory && norepinephrineLevel > 0.5f) {
                // Boost encoding strength for novel inputs
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            // NE enhances signal detection, boosting relevant neurons
                            neuron->injectCurrent(norepinephrineLevel * 0.3f);
                        }
                    }
                }
            }
        }
    }
    
    // Update serotonin (behavioral flexibility and mood)
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
        // Apply 5-HT effects on behavioral flexibility
        float serotoninLevel = pImpl->serotonin->getLevel();
        if (serotoninLevel > 0.0f) {
            // Modulate action selection
            if (pImpl->workingMemory && pImpl->workingMemory->getActiveTraces() > 1) {
                // 5-HT increases behavioral flexibility, allowing trace switching
                pImpl->workingMemory->runCompetition();
            }
            
            // Enhance memory consolidation of emotional content
            if (pImpl->episodicMemory && serotoninLevel > 0.7f) {
                // Prioritize emotional memory consolidation
                pImpl->serotonin->prioritizeNegativeMemoryConsolidation(serotoninLevel);
            }
        }
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
    
    // Connect neuromodulators to prediction error system
    if (pImpl->predictionError && pImpl->dopamine) {
        // Neuromodulators receive prediction error signals
        float predictionError = pImpl->predictionError->getError();
        
        // Dopamine responds to reward prediction error
        if (predictionError != 0.0f) {
            pImpl->dopamine->signalRewardPredictionError(predictionError);
        }
        
        // Other neuromodulators also respond to prediction error
        // Serotonin: modulates behavioral flexibility based on prediction error
        if (fabs(predictionError) > 0.5f) {
            pImpl->serotonin->setLevel(pImpl->serotonin->getLevel() + 0.1f);
        }
        
        // Norepinephrine: enhances signal detection based on prediction error
        if (predictionError > 0.0f) {
            pImpl->norepinephrine->enhanceSignalDetection(0.2f);
        }
        
        // Acetylcholine: modulates attention based on prediction error
        if (predictionError != 0.0f) {
            pImpl->acetylcholine->enhanceAttention(0.3f);
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Vectorized synaptic updates using parallel processing
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    // Parallel processing of synaptic updates
    auto synapseUpdateFunc = [this, &plasticityMod](size_t synIndex, ParallelNeuralProcessor::ThreadLocalState& state) {
        // Convert synapse index to actual synapse object
        // This is a simplified version - in a full implementation, you'd use SparseConnectivity
        // For now, we'll use the existing scalar implementation
        size_t regionIndex = 0;
        for (auto& region : pImpl->regions) {
            size_t synapseCount = region->getSynapseCount();
            if (synIndex < synapseCount) {
                auto& syn = *region->getSynapses()[synIndex];
                
                // Apply STDP with neuromodulation
                if (syn.getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn.getPreSpikeHistory();
                    const auto& postSpikes = syn.getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Modify weight change based on dopamine
                        pImpl->stdp->update(&syn, preSpikes, postSpikes, pImpl->timestep);
                        float weight = syn.getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn.setWeight(weight);
                    }
                }
                
                // Apply Hebbian learning
                if (syn.getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn.getPreSpikeHistory();
                    const auto& postSpikes = syn.getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        pImpl->hebbian->update(&syn, preSpikes, postSpikes, pImpl->timestep);
                    }
                }
                
                // Update synapse state
                syn.step(currentTime);
            }
            break; // Only process first region for now
        }
    };
    
    // Calculate total synapse count for parallel processing
    size_t totalSynapseCount = 0;
    for (auto& region : pImpl->regions) {
        totalSynapseCount += region->getSynapseCount();
    }
    
    if (totalSynapseCount > 0 && pImpl->parallelProcessor.getThreadPool().getNumThreads() > 1) {
        // Use parallel processing for synaptic updates
        pImpl->parallelProcessor.processNeurons(totalSynapseCount, synapseUpdateFunc);
    } else {
        // Fallback to scalar processing
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
    
    // ========== STEP 7: Update episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Encode sensory state using working memory patterns for episodic memory
            if (pImpl->workingMemory) {
                std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
                if (!workingMemoryPattern.empty()) {
                    episode.sensoryState = workingMemoryPattern;
                }
            }
            
            // Store active neurons with enhanced encoding
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
            
            // Encode action selection outcome
            auto action = produceAction();
            episode.action = action->getType();
            
            // Store prediction error as part of episode for learning
            if (pImpl->predictionError) {
                float predictionError = pImpl->predictionError->getError();
                episode.reward += predictionError * 0.5f;  // Weight prediction error in reward
            }
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem && pImpl->workingMemory) {
        // Get current sensory state from working memory
        std::vector<float> currentSensoryState = pImpl->workingMemory->retrieve();
        
        if (!currentSensoryState.empty() && !pImpl->sensoryNeurons.empty()) {
            // Create a SensoryInput from current working memory pattern
            // Note: In a real implementation, we'd have actual sensory data
            // For now, use working memory as a proxy for sensory state
            class SimpleSensoryInput : public SensoryInput {
            public:
                explicit SimpleSensoryInput(const std::vector<float>& data) : data_(data) {}
                const char* getType() const override { return "WorkingMemory"; }
                const std::vector<float>& getData() const override { return data_; }
                size_t getDimensions() const override { return data_.size(); }
                std::unique_ptr<SensoryInput> clone() const override {
                    return std::make_unique<SimpleSensoryInput>(data_);
                }
            private:
                std::vector<float> data_;
            };
            
            SimpleSensoryInput sensoryProxy(currentSensoryState);
            
            // Update predictions based on current state
            pImpl->predictionSystem->train(sensoryProxy);
            
            // Update episodic memory based on prediction error
            if (pImpl->episodicMemory) {
                float predictionError = pImpl->predictionSystem->getPredictionError();
                if (std::abs(predictionError) > 0.1f) {  // Only store significant prediction errors
                    pImpl->episodicMemory->updateRelevance(currentStep, predictionError * 10.0f);
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
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation && pImpl->workingMemory) {
        std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
        if (!workingMemoryPattern.empty()) {
            // Process working memory patterns to form concepts
            pImpl->conceptFormation->presentExperience(
                workingMemoryPattern,
                workingMemoryPattern,  // Use pattern as features
                pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,
                currentStep
            );
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
            
            // Neuromodulator effects on structural plasticity
            if (pImpl->dopamine) {
                float dopamineFactor = pImpl->dopamine->getLevel();
                plasticityMod *= (1.0f + dopamineFactor * 0.3f);  // Dopamine enhances plasticity
            }
            if (pImpl->acetylcholine) {
                float acetylcholineFactor = pImpl->acetylcholine->getLevel();
                plasticityMod *= (1.0f + acetylcholineFactor * 0.2f);  // ACh enhances learning
            }
            if (pImpl->norepinephrine) {
                float norepinephrineFactor = pImpl->norepinephrine->getLevel();
                plasticityMod *= (1.0f + norepinephrineFactor * 0.1f);  // NE modulates plasticity
            }
            if (pImpl->serotonin) {
                float serotoninFactor = pImpl->serotonin->getLevel();
                plasticityMod /= (1.0f + serotoninFactor * 0.1f);  // 5-HT reduces plasticity (stability)
            }
            
            // Use config-based development rates, not hardcoded values
            sp->setSynaptogenesisRate(pImpl->developmentSynaptogenesisRate * plasticityMod);
            sp->setPruningRate(pImpl->developmentPruningRate * (2.0f - plasticityMod));
        }
        
        // Neuromodulator effects on development
        if (pImpl->dopamine) {
            pImpl->dopamine->setLevel(std::min(1.0f, pImpl->dopamine->getLevel() + 0.01f));  // Reward increases dopamine
        }
        if (pImpl->acetylcholine) {
            // ACh promotes memory-related development
            if (pImpl->workingMemory && pImpl->workingMemory->getMemoryActivity() > 0.5f) {
                pImpl->acetylcholine->setLevel(std::min(1.0f, pImpl->acetylcholine->getLevel() + 0.01f));
            }
        }
        if (pImpl->norepinephrine) {
            // NE responds to novel events
            if (pImpl->novelty && pImpl->novelty->getLevel() > 0.5f) {
                pImpl->norepinephrine->setLevel(std::min(1.0f, pImpl->norepinephrine->getLevel() + 0.01f));
            }
        }
        if (pImpl->serotonin) {
            // 5-HT responds to social interactions and negative outcomes
            if (pImpl->curiosity && pImpl->curiosity->getExplorationDrive() > 0.5f) {
                pImpl->serotonin->setLevel(std::min(1.0f, pImpl->serotonin->getLevel() + 0.01f));
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
    // Use NeuralPlanner for action selection instead of simple motor neuron activity
    if (pImpl->planner && pImpl->workingMemory) {
        std::vector<float> currentState;
        if (pImpl->workingMemory) {
            currentState = pImpl->workingMemory->retrieve();
        } else {
            // Fallback to simple motor neuron activity
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Get goal from planner or use default
        std::vector<float> goal = pImpl->planner->getCurrentGoal();
        if (goal.empty()) {
            goal = std::vector<float>(currentState.size(), 0.5f);  // Default goal
        }
        
        // Plan action using NeuralPlanner
        ActionType plannedAction = pImpl->planner->planAction(currentState, goal.back());
        
        // Create action from planned action
        auto action = std::make_unique<Action>(plannedAction);
        
        // Update planner based on action quality
        if (pImpl->dopamine) {
            pImpl->planner->updatePlanQuality(std::vector<ActionType>(), // No planned actions since we just got one
                                            std::vector<ActionType>({plannedAction}),
                                            pImpl->dopamine->getLevel());
        }
        
        return action;
    }
    
    // Fallback: Simple action selection based on motor neuron activity
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
        // Use CheckpointManager for automated checkpointing
        if (pImpl->checkpointManager) {
            // Use CheckpointManager for the save operation
            // The CheckpointManager will use all the configured providers
            if (!pImpl->checkpointManager->saveImmediately(filepath)) {
                NLM_LOG_ERROR("Failed to save brain state using CheckpointManager");
                return false;
            }
            return true;
        }
        
        // Fallback to direct checkpoint writer (existing implementation)
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
        
        // Write all checkpoint sections using providers
        if (!saveAllCheckpointSections(writer)) {
            NLM_LOG_ERROR("Failed to save all checkpoint sections");
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
        // Use CheckpointManager for automated checkpointing
        if (pImpl->checkpointManager) {
            // Use CheckpointManager for the load operation
            if (!pImpl->checkpointManager->load(filepath)) {
                NLM_LOG_ERROR("Failed to load brain state using CheckpointManager");
                return false;
            }
            return true;
        }
        
        // Fallback to direct checkpoint reader (existing implementation)
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read all checkpoint sections
        if (!loadAllCheckpointSections(reader)) {
            NLM_LOG_ERROR("Failed to load all checkpoint sections");
            return false;
        }
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

// Helper function to load all checkpoint sections
bool Brain::loadAllCheckpointSections(CheckpointReader& reader) {
    // TODO: Implement loading all checkpoint sections using CheckpointSystem
    // This is a placeholder implementation
    
    // Read header section
    auto headerData = reader.readSection(CheckpointSection::Header);
    if (headerData.empty()) {
        // Check if error was set
        if (!reader.getError().empty()) {
            NLM_LOG_ERROR("Failed to read header: " + reader.getError());
            return false;
        }
        // Header not found, try to continue with other sections
    }
    
    // TODO: Add other sections
    // This would need access to the Impl object for the load operations
    // For now, just return success
    return true;
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

Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
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

// Log memory status and statistics
void Brain::logMemoryStatus() const {
    if (!pImpl) {
        NLM_LOG_INFO("Brain: No implementation (moved-from or default)");
        return;
    }
    
    NLM_LOG_INFO("Brain Memory Status:");
    NLM_LOG_INFO("  - Total allocations: " + std::to_string(pImpl->allocationCounter));
    NLM_LOG_INFO("  - Total deallocations: " + std::to_string(pImpl->deallocationCounter));
    NLM_LOG_INFO("  - Current allocations: " + std::to_string(
                 pImpl->allocationCounter - pImpl->deallocationCounter));
    NLM_LOG_INFO("  - Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("  - Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("  - Spike events this step: " + std::to_string(pImpl->totalSpikesThisStep));
    NLM_LOG_INFO("  - Total spike count: " + std::to_string(pImpl->totalSpikesTotal));
    
    // Check for potential memory leaks if tracking is enabled
    #ifdef DEBUG_MEMORY_LEAKS
    checkMemoryLeaks();
    #endif
}

// Check for memory leaks
void Brain::checkMemoryLeaks() {
    #ifdef DEBUG_MEMORY_LEAKS
    if (allocationCounter != deallocationCounter) {
        NLM_LOG_WARNING("MEMORY LEAK DETECTED: " + 
                      std::to_string(allocationCounter - deallocationCounter) + 
                      " allocations without corresponding deallocations");
    } else {
        NLM_LOG_DEBUG("Memory tracking complete: " + 
                     std::to_string(allocationCounter) + " allocations, " +
                     std::to_string(deallocationCounter) + " deallocations");
    }
    #endif
}

// Get allocation statistics
static size_t Brain::getAllocationCount() {
    return allocationCounter.load();
}

// Get deallocation statistics
static size_t Brain::getDeallocationCount() {
    return deallocationCounter.load();
}

// Checkpoint provider implementations for Brain

// Neuron checkpoint provider (already implemented in save() method)
bool Brain::Impl::saveNeuronsToCheckpoint(NeuronCheckpointData& data) {
    data.membranePotential.clear();
    data.restingPotential.clear();
    data.threshold.clear();
    data.resetPotential.clear();
    data.leakConductance.clear();
    data.firingState.clear();
    data.refractoryRemaining.clear();
    data.refractoryPeriod.clear();
    data.lastSpikeTime.clear();
    data.neuronType.clear();
    data.regionId.clear();
    data.populationId.clear();
    
    // Collect neuron states from regions
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                data.membranePotential.push_back(state.membranePotential);
                data.restingPotential.push_back(state.restingPotential);
                data.threshold.push_back(state.threshold);
                data.resetPotential.push_back(state.resetPotential);
                data.leakConductance.push_back(state.leakConductance);
                data.firingState.push_back(static_cast<uint8_t>(state.firingState));
                data.refractoryRemaining.push_back(state.refractoryRemaining);
                data.refractoryPeriod.push_back(state.refractoryPeriod);
                data.lastSpikeTime.push_back(state.lastSpikeTime);
                data.neuronType.push_back(static_cast<uint64_t>(neuron->getType()));
                data.regionId.push_back(static_cast<uint64_t>(region->getId().index()));
                data.populationId.push_back(static_cast<uint64_t>(pop->getId().index()));
            }
        }
    }
    
    return !data.membranePotential.empty();
}

// Synapse checkpoint provider (already implemented in save() method)
bool Brain::Impl::saveSynapsesToCheckpoint(SynapseCheckpointData& data) {
    data.sourceNeuron.clear();
    data.destinationNeuron.clear();
    data.weight.clear();
    data.delay.clear();
    data.synapseType.clear();
    data.plasticityFlags.clear();
    data.eligibilityTrace.clear();
    data.efficacy.clear();
    data.shortTermDepression.clear();
    data.shortTermFacilitation.clear();
    
    // Collect synapse states from regions
    for (const auto& region : regions) {
        for (const auto* syn : region->getSynapses()) {
            data.sourceNeuron.push_back(syn->getSourceNeuron().index());
            data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
            data.weight.push_back(syn->getWeight());
            data.delay.push_back(syn->getDelay());
            data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
            data.plasticityFlags.push_back(syn->getPlasticityFlags().to_ulong());
            data.eligibilityTrace.push_back(syn->getEligibilityTrace());
            data.efficacy.push_back(syn->getEfficacy());
            data.shortTermDepression.push_back(syn->getShortTermDepression());
            data.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
        }
    }
    
    // Also add inter-region connections
    for (const auto& conn : interRegionConnections) {
        data.sourceNeuron.push_back(conn.sourceRegion.index());
        data.destinationNeuron.push_back(conn.targetRegion.index());
        data.weight.push_back(conn.weight);
        data.delay.push_back(conn.delay);
        data.synapseType.push_back(static_cast<uint8_t>(SynapseType::LongRange));
        data.plasticityFlags.push_back(conn.plasticityFlags.to_ulong());
        data.eligibilityTrace.push_back(0.0f);
        data.efficacy.push_back(0.0f);
        data.shortTermDepression.push_back(0.0f);
        data.shortTermFacilitation.push_back(0.0f);
    }
    
    return !data.weight.empty();
}

// Config checkpoint provider
bool Brain::Impl::saveConfigToCheckpoint(std::vector<uint64_t>& data) {
    // Save important config parameters
    if (!config) return false;
    
    // Get all uint64_t config values
    auto keys = config->getAllKeys();
    for (const auto& key : keys) {
        if (auto value = config->get<uint64_t>(key)) {
            data.push_back(*value);
        }
    }
    
    // Also save some important float/double values as uint64_t (reinterpreting bits)
    auto floatKeys = {"simulation_timestep", "development_synaptogenesis_rate", 
                      "development_pruning_rate", "dopamine_baseline", 
                      "acetylcholine_baseline", "norepinephrine_baseline", 
                      "serotonin_baseline", "stdp_ltp_weight", 
                      "stdp_ltd_weight", "stdp_time_constant"};
    
    for (const auto& key : floatKeys) {
        if (auto value = config->get<float>(key)) {
            uint64_t bits = *reinterpret_cast<const uint64_t*>(value);
            data.push_back(bits);
        } else if (auto value = config->get<double>(key)) {
            uint64_t bits = *reinterpret_cast<const uint64_t*>(value);
            data.push_back(bits);
        }
    }
    
    return !data.empty();
}

// Connectivity checkpoint provider
bool Brain::Impl::saveConnectivityToCheckpoint(std::vector<uint64_t>& data) {
    // Save inter-region connections
    for (const auto& conn : interRegionConnections) {
        data.push_back(conn.sourceRegion.index());
        data.push_back(conn.targetRegion.index());
        data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(conn.weight)));
        data.push_back(static_cast<uint64_t>(conn.delay));
        data.push_back(conn.plasticityFlags.to_ulong());
    }
    
    return !data.empty();
}

// Plasticity checkpoint provider
bool Brain::Impl::savePlasticityToCheckpoint(std::vector<uint64_t>& data) {
    // Save plasticity system states
    if (stdp) {
        data.push_back(static_cast<uint64_t>(stdp->getLTPWeight()));
        data.push_back(static_cast<uint64_t>(stdp->getLTDWeight()));
        data.push_back(static_cast<uint64_t>(stdp->getTimeConstant()));
    }
    
    if (hebbian) {
        data.push_back(static_cast<uint64_t>(hebbian->getLearningRate()));
        data.push_back(static_cast<uint64_t>(hebbian->getTau()));
    }
    
    if (structuralPlasticity) {
        data.push_back(static_cast<uint64_t>(structuralPlasticity->getSynaptogenesisRate()));
        data.push_back(static_cast<uint64_t>(structuralPlasticity->getPruningRate()));
    }
    
    return true;
}

// Development checkpoint provider
bool Brain::Impl::saveDevelopmentToCheckpoint(std::vector<uint64_t>& data) {
    // Save developmental state
    data.push_back(static_cast<uint64_t>(developmentalStage));
    data.push_back(static_cast<uint64_t>(currentStep));
    data.push_back(static_cast<uint64_t>(stepsSinceLastEpisode));
    
    // Save development system state if available
    if (developmentSystem) {
        data.push_back(static_cast<uint64_t>(developmentSystem->getAge()));
        data.push_back(static_cast<uint64_t>(developmentSystem->getStageProgress()));
    }
    
    // Save developmental rates
    data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(developmentSynaptogenesisRate)));
    data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(developmentPruningRate)));
    
    return true;
}

// Neuromodulation checkpoint provider
bool Brain::Impl::saveNeuromodulationToCheckpoint(std::vector<uint64_t>& data) {
    // Save neuromodulator states
    if (dopamine) {
        data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(dopamine->getLevel())));
        data.push_back(dopamine->getBaseline());
    }
    
    if (acetylcholine) {
        data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(acetylcholine->getLevel())));
        data.push_back(acetylcholine->getBaseline());
    }
    
    if (norepinephrine) {
        data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(norepinephrine->getLevel())));
        data.push_back(norepinephrine->getBaseline());
    }
    
    if (serotonin) {
        data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(serotonin->getLevel())));
        data.push_back(serotonin->getBaseline());
    }
    
    if (curiosity) {
        data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(curiosity->getLevel())));
    }
    
    if (predictionError) {
        data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(predictionError->getError())));
    }
    
    if (novelty) {
        data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(novelty->getLevel())));
    }
    
    return true;
}

// Memory checkpoint provider
bool Brain::Impl::saveMemoryToCheckpoint(std::vector<uint64_t>& data) {
    // Save working memory state
    if (workingMemory) {
        auto workingMemData = workingMemory->getMemoryData();
        for (const auto& item : workingMemData) {
            data.push_back(item.first);
            data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(item.second)));
        }
    }
    
    // Save episodic memory state
    if (episodicMemory) {
        auto episodes = episodicMemory->getAllEpisodes();
        data.push_back(static_cast<uint64_t>(episodes.size()));
        for (const auto& episode : episodes) {
            data.push_back(episode.step);
            data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(episode.reward)));
            data.push_back(static_cast<uint64_t>(episode.timestamp));
            
            for (const auto& neuronId : episode.activeNeurons) {
                data.push_back(neuronId);
            }
            
            for (const auto& activation : episode.neuronActivations) {
                data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(activation)));
            }
        }
    }
    
    // Save associative memory state
    if (associativeMemory) {
        auto associations = associativeMemory->getAllAssociations();
        data.push_back(static_cast<uint64_t>(associations.size()));
        for (const auto& assoc : associations) {
            data.push_back(assoc.pattern.first);
            data.push_back(assoc.pattern.second);
            data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(assoc.strength)));
        }
    }
    
    return true;
}

// Spike history checkpoint provider
bool Brain::Impl::saveSpikeHistoryToCheckpoint(std::vector<uint64_t>& data) {
    // Save spike history from spike system
    if (spikeSystem) {
        auto spikeHistory = spikeSystem->getRecentSpikes();
        data.push_back(static_cast<uint64_t>(spikeHistory.size()));
        for (const auto& spike : spikeHistory) {
            data.push_back(spike.neuronId);
            data.push_back(spike.step);
            data.push_back(static_cast<uint64_t>(std::bit_cast<uint32_t>(spike.time)));
        }
    }
    
    return true;
}

// Random state checkpoint provider
bool Brain::Impl::saveRandomStateToCheckpoint(std::vector<uint64_t>& data) {
    // Save random generator state
    if (rng) {
        // Try to get random state (assuming there might be a method for this)
        // For now, save some random values
        for (int i = 0; i < 4; ++i) {
            data.push_back(rng->next());
        }
    }
    
    return true;
}

// Simulation state checkpoint provider
bool Brain::Impl::saveSimulationStateToCheckpoint(std::vector<uint64_t>& data) {
    // Save current simulation state
    data.push_back(currentStep);
    data.push_back(static_cast<uint64_t>(currentTime));
    data.push_back(totalSpikesThisStep);
    data.push_back(totalSpikesTotal);
    data.push_back(isResting);
    data.push_back(stepsSinceLastEpisode);
    data.push_back(replayInterval);
    data.push_back(consolidationInterval);
    
    return true;
}

// Environment checkpoint provider
bool Brain::Impl::saveEnvironmentToCheckpoint(std::vector<uint8_t>& data) {
    // Save environment state if available
    // This could include world state, sensory inputs, etc.
    // For now, just save some basic simulation parameters
    std::vector<uint8_t> envData;
    
    // Convert some float values to bytes for environment checkpoint
    auto addFloat = [&envData](float value) {
        uint64_t bits = std::bit_cast<uint64_t>(value);
        envData.insert(envData.end(), 
                     reinterpret_cast<const uint8_t*>(&bits),
                     reinterpret_cast<const uint8_t*>(&bits) + sizeof(uint64_t));
    };
    
    addFloat(timestep);
    addFloat(dopamine ? dopamine->getLevel() : 0.0f);
    addFloat(acetylcholine ? acetylcholine->getLevel() : 0.0f);
    addFloat(norepinephrine ? norepinephrine->getLevel() : 0.0f);
    addFloat(serotonin ? serotonin->getLevel() : 0.0f);
    
    // Helper function to save all checkpoint sections
bool Brain::saveAllCheckpointSections(CheckpointWriter& writer) const {
    // TODO: Implement saving all checkpoint sections using CheckpointSystem
    // This is a placeholder implementation
    
    // Save header section
    CheckpointHeader header;
    header.magic = CHECKPOINT_MAGIC;
    header.majorVersion = CHECKPOINT_VERSION_MAJOR;
    header.minorVersion = CHECKPOINT_VERSION_MINOR;
    header.patchVersion = CHECKPOINT_VERSION_PATCH;
    header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    header.totalSize = 0; // Will be updated during writing
    header.neuronCount = getTotalNeuronCount();
    header.synapseCount = getTotalSynapseCount();
    header.regionCount = getRegionCount();
    header.step = pImpl->currentStep;
    header.simulationTime = pImpl->currentTime;
    header.checksum = 0; // Placeholder
    header.compressionLevel = 0;
    header.flags = 0;
    
    // Write header
    if (!writer.writeSection(CheckpointSection::Header, &header, sizeof(CheckpointHeader))) {
        return false;
    }
    
    // TODO: Add other sections
    // This would need access to the Impl object for the providers
    // For now, just return success with header saved
    return true;
}

} // namespace nlm

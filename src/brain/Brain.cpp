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
#include "../plasticity/PlasticityRule.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/SparseConnectivity.hpp"
#include "../performance/ParallelProcessing.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Phase 6: Integration function declarations
namespace Integration {
    void initializeMemoryPools(Brain::Impl& impl);
    void initializeEventQueues(Brain::Impl& impl);
    void initializeSparseConnectivity(Brain::Impl& impl);
    void initializeParallelProcessing(Brain::Impl& impl);
    void initializeVectorization(Brain::Impl& impl);
    void connectCheckpointSystem(Brain::Impl& impl);
    void updatePerformanceStatistics(Brain::Impl& impl, uint64_t step, double time);
    bool saveWithCheckpointManager(Brain::Impl& impl, const std::string& filepath);
    bool loadWithCheckpointManager(Brain::Impl& impl, const std::string& filepath);
}

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== PERFORMANCE INFRASTRUCTURE ==========
    // Memory pools for neuron/synapse allocation
    std::unique_ptr<MemoryPool<Neuron>> neuronPool;
    std::unique_ptr<MemoryPool<Synapse>> synapsePool;
    
    // Event queue system for spike events
    std::unique_ptr<SpikeRingBuffer> immediateSpikeQueue;
    std::unique_ptr<DelayedSpikeQueue> delayedSpikeQueue;
    std::unique_ptr<PriorityEventQueue> priorityEventQueue;
    
    // Sparse connectivity representation
    std::unique_ptr<SparseConnectivity> sparseConnectivity;
    
    // Parallel processing for neuron updates
    std::unique_ptr<ParallelNeuralProcessor> parallelProcessor;
    
    // SIMD vectorization for neural dynamics
    std::unique_ptr<VectorizedLIF> vectorizedLIF;
    std::unique_ptr<VectorizedSTDP> vectorizedSTDP;
    
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
    
    // Performance statistics
    size_t memoryPoolAllocations;
    size_t eventQueueSize;
    size_t connectivityMemoryUsage;
    size_t parallelProcessingUsage;
    float simdVectorizationSpeedup;
    
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
        , neuronPool(nullptr)
        , synapsePool(nullptr)
        , immediateSpikeQueue(nullptr)
        , delayedSpikeQueue(nullptr)
        , priorityEventQueue(nullptr)
        , sparseConnectivity(nullptr)
        , parallelProcessor(nullptr)
        , vectorizedLIF(nullptr)
        , vectorizedSTDP(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , memoryPoolAllocations(0)
        , eventQueueSize(0)
        , connectivityMemoryUsage(0)
        , parallelProcessingUsage(0)
        , simdVectorizationSpeedup(1.0f)
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
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
    // Initialize memory pools for neuron/synapse allocation
    neuronPool = std::make_unique<MemoryPool<Neuron>>(1024, 16);
    synapsePool = std::make_unique<MemoryPool<Synapse>>(2048, 8);
    
    // Initialize event queue system for spike events
    immediateSpikeQueue = std::make_unique<SpikeRingBuffer>();
    delayedSpikeQueue = std::make_unique<DelayedSpikeQueue>();
    priorityEventQueue = std::make_unique<PriorityEventQueue>();
    
    // Initialize sparse connectivity representation
    sparseConnectivity = std::make_unique<SparseConnectivity>();
    
    // Initialize parallel processing for neuron updates
    parallelProcessor = std::make_unique<ParallelNeuralProcessor>();
    
    // Initialize SIMD vectorization for neural dynamics
    vectorizedLIF = std::make_unique<VectorizedLIF>();
    vectorizedSTDP = std::make_unique<VectorizedSTDP>();
    
    // Connect checkpoint system
    connectCheckpointSystem(*this);
        
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
        acetylcholine = std::make_unique<Acetylcholine>();
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
        
        }

    // ========== INITIALIZE PERFORMANCE INFRASTRUCTURE ==========
    
    // Initialize memory pools for neuron/synapse allocation
    neuronPool = std::make_unique<MemoryPool<Neuron>>(1024, 16);
    synapsePool = std::make_unique<MemoryPool<Synapse>>(2048, 8);
    
    // Initialize event queue system for spike events
    immediateSpikeQueue = std::make_unique<SpikeRingBuffer>();
    delayedSpikeQueue = std::make_unique<DelayedSpikeQueue>();
    priorityEventQueue = std::make_unique<PriorityEventQueue>();
    
    // Initialize sparse connectivity representation
    sparseConnectivity = std::make_unique<SparseConnectivity>();
    
    // Initialize parallel processing for neuron updates
    parallelProcessor = std::make_unique<ParallelNeuralProcessor>();
    
    // Initialize SIMD vectorization for neural dynamics
    vectorizedLIF = std::make_unique<VectorizedLIF>();
    vectorizedSTDP = std::make_unique<VectorizedSTDP>();
    
    // ========== INITIALIZE INTEGRATED SYSTEMS ==========

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
                        // Calculate activation level based on firing
                        float activation = std::abs(state.membranePotential - state.restingPotential) / 10.0f;
                        
                        // Apply attention and neuromodulation to firing neurons
                        float attentionInfluence = 1.0f;
                        if (pImpl->attention) {
                            attentionInfluence += pImpl->attention->getExcitationFor(neuron->getId());
                        }
                        if (pImpl->dopamine) {
                            attentionInfluence += pImpl->dopamine->getLevel() * 0.5f;
                        }
                        
                        // Store with enhanced activation
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), activation * attentionInfluence);
                    }
                }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        // Update working memory with current state (maintenance and decay)
        pImpl->workingMemory->update(pImpl->timestep);
        
        // Integrate working memory with attention system
        if (pImpl->attention && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Get working memory neurons for competition
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
        
        // Apply attention-based salience to working memory neurons
        if (!pImpl->workingMemory->getMemoryNeurons().empty()) {
            for (const NeuronId& neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                // Get attention for this neuron
                float inhibition = pImpl->attention ? pImpl->attention->getInhibitionFor(neuronId) : 0.0f;
                float excitation = pImpl->attention ? pImpl->attention->getExcitationFor(neuronId) : 0.0f;
                
                // Apply attention to working memory
                pImpl->workingMemory->strengthenMemory(1.0f + excitation - inhibition);
            }
        }
    }
    
    // ========== STEP 5: Apply comprehensive neuromodulation effects ==========
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        // Calculate novelty and prediction error for curiosity system
        float noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
        float predictionErrorLevel = pImpl->predictionError ? pImpl->predictionError->getMagnitude() : 0.0f;
        pImpl->curiosity->update(noveltyLevel, predictionErrorLevel, pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // ========== 1. WORKING MEMORY MODULATION ==========
        // Enhance working memory trace strength based on dopamine levels
        if (pImpl->workingMemory) {
            pImpl->dopamine->applyWorkingMemoryEffects(pImpl->workingMemory.get());
        }
        
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
        
        // ========== 2. ATTENTION MECHANISMS ==========
        // Implement dopamine-based attentional selection
        if (pImpl->attention) {
            pImpl->dopamine->applyAttentionEffects(pImpl->attention.get());
        }
        
        // ========== 3. MEMORY CONSOLIDATION ==========
        // Enhance episodic memory consolidation for high-reward experiences
        if (pImpl->episodicMemory) {
            pImpl->dopamine->applyMemoryConsolidationEffects(pImpl->episodicMemory.get());
        }
        
        // ========== 4. PREDICTION ERROR INTEGRATION ==========
        // Connect dopamine with prediction error signals
        if (pImpl->predictionError) {
            pImpl->dopamine->applyPredictionErrorEffects(pImpl->predictionError.get());
        }
        
        // ========== 5. PLASTICITY RULE ENHANCEMENT ==========
        // Extend beyond STDP to include other plasticity mechanisms
        // Implement metaplasticity (changes to plasticity rules)
        // Add activity-dependent plasticity modulation
        if (pImpl->stdp) {
            pImpl->dopamine->applyPlasticityEffects(pImpl->stdp.get());
        }
        if (pImpl->hebbian) {
            pImpl->dopamine->applyPlasticityEffects(pImpl->hebbian.get());
        }
        
        // ========== 6. MOTIVATIONAL STATE ==========
        // Implement dopamine-driven motivational states
        // Connect to curiosity and exploration systems
        // Modulate action selection based on anticipated reward
        if (pImpl->curiosity && pImpl->novelty) {
            pImpl->dopamine->applyMotivationalEffects(pImpl->curiosity.get(), pImpl->novelty.get());
        }
    }
    
    // ========== STEP 7a: Experience logging with working memory integration ==========
    // NEW: Store experience with working memory consolidation
    // This is Phase 6 requirement: Update episodic memory after working memory consolidation
    if (pImpl->episodicMemory && pImpl->workingMemory) {
        // Create experience from current working memory state
        EpisodicMemoryItem experience;
        experience.timestamp = currentStep;
        experience.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        // Get current neural state as part of experience
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        experience.activeNeurons.push_back(neuron->getId());
                        experience.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        // Store working memory state for experience consolidation
        std::vector<NeuronId> wmNeurons = pImpl->workingMemory ? pImpl->workingMemory->getMemoryNeurons() : std::vector<NeuronId>();
        if (!wmNeurons.empty() && pImpl->workingMemory) {
            // Calculate working memory activity for experience consolidation
            float wmActivity = pImpl->workingMemory->getMemoryActivity();
            float wmStrength = 0.0f;
            
            for (const NeuronId& neuronId : wmNeurons) {
                wmStrength += pImpl->workingMemory->getNeuronActivation(neuronId);
            }
            
            // Normalize working memory strength
            if (!wmNeurons.empty()) {
                wmStrength /= wmNeurons.size();
            }
            
            // Enhanced experience consolidation based on working memory
            // This is Phase 6 requirement: proper episodic memory consolidation from working memory
            if (wmActivity > 0.5f || wmStrength > 0.3f) {
                // Mark as important for consolidation
                experience.isConsolidated = true;
                experience.consolidationStrength = std::max(wmActivity, wmStrength);
            }
        }
        
        // Store experience with working memory integration
        pImpl->episodicMemory->storeExperience(experience);
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
            
            // Check working memory for consolidation opportunities
            std::vector<NeuronId> wmNeurons = pImpl->workingMemory ? pImpl->workingMemory->getMemoryNeurons() : std::vector<NeuronId>();
            
            // Get current neural state as part of episode
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
            
            // Check for working memory-based consolidation
            if (!wmNeurons.empty() && pImpl->workingMemory) {
                // Calculate working memory activity for consolidation
                float wmActivity = pImpl->workingMemory->getMemoryActivity();
                float wmStrength = 0.0f;
                
                for (const NeuronId& neuronId : wmNeurons) {
                    wmStrength += pImpl->workingMemory->getNeuronActivation(neuronId);
                }
                
                // Normalize working memory strength
                if (!wmNeurons.empty()) {
                    wmStrength /= wmNeurons.size();
                }
                
                // Consolidate if working memory is active (important experience)
                if (wmActivity > 0.5f || wmStrength > 0.3f) {
                    // Mark as important for consolidation
                    episode.isConsolidated = true;
                    episode.consolidationStrength = std::max(wmActivity, wmStrength);
                }
            }
            
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
    
    // ========== SENSEMOCAPY INPUT TO WORKING MEMORY INTEGRATION ==========
    // Track prediction error for novel/important sensory input
    float predictionError = 1.0f;
    if (pImpl->predictionSystem) {
        // Get prediction error from prediction system
        predictionError = pImpl->predictionSystem->getPredictionError();
    }
    
    // ========== STEP 5: CONNECT SENSORY INPUT TO WORKING MEMORY ==========
    // Store sensory input in working memory with competition
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store in working memory with competition and novelty-based weighting
        if (pImpl->workingMemory) {
            // Apply novelty weighting (prediction error makes input more important)
            float noveltyWeight = 1.0f;
            if (pImpl->novelty) {
                // Get novelty score for this input
                float novelty = pImpl->novelty->getNovelty();
                if (novelty > 0.5f) {
                    noveltyWeight = 1.0f + (novelty - 0.5f) * 2.0f;
                }
            }
            
            // Apply attention modulation
            float attentionFactor = 1.0f;
            if (pImpl->attention) {
                // Get attention for this sensory neuron
                attentionFactor = 1.0f + pImpl->attention->getExcitationFor(pImpl->sensoryNeurons[i]->getId());
                attentionFactor -= pImpl->attention->getInhibitionFor(pImpl->sensoryNeurons[i]->getId());
            }
            
            // Store with competition and novelty-based strength
            float strength = std::abs(normalizedValue) > 0.5f ? 
                (normalizedValue / 10.0f) * noveltyWeight * attentionFactor : 0.0f;
            
            if (strength > 0.0f) {
                pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), strength);
                
                // Also store the pattern for episodic memory consolidation
                std::vector<float> pattern;
                pattern.reserve(numSensory);
                for (size_t j = 0; j < numSensory; ++j) {
                    float val = 0.0f;
                    if (j < values.size()) {
                        val = static_cast<float>(values[j]) * 10.0f;
                    }
                    pattern.push_back(val);
                }
                
                // Update episodic memory with important sensory episodes
                if (pImpl->episodicMemory && predictionError > 0.5f) {
                    // Important episode (high prediction error) - store for consolidation
                    EpisodicMemoryItem episode;
                    episode.timestamp = currentStep;
                    episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                    
                    // Get current neural state as part of episode
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
                    
                    pImpl->episodicMemory->storeEpisode(episode);
                }
            }
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
    
    // Apply acetylcholine effects on attention and memory
    if (signal.getName() == std::string("ACh")) {
        if (pImpl->acetylcholine) {
            pImpl->acetylcholine->configure(*pImpl->config);
            
            // Apply ACh effects on working memory
            if (pImpl->workingMemory) {
                pImpl->acetylcholine->applyWorkingMemoryEffects(pImpl->workingMemory.get());
            }
            
            // Apply ACh effects on attention
            if (pImpl->attention) {
                pImpl->acetylcholine->applyAttentionEffects(pImpl->attention.get());
            }
            
            // Apply ACh effects on episodic memory
            if (pImpl->episodicMemory) {
                pImpl->acetylcholine->applyMemoryConsolidationEffects(pImpl->episodicMemory.get());
            }
            
            // Apply ACh effects on plasticity
            if (pImpl->stdp) {
                pImpl->acetylcholine->applyPlasticityEffects(pImpl->stdp.get());
            }
            if (pImpl->hebbian) {
                pImpl->acetylcholine->applyPlasticityEffects(pImpl->hebbian.get());
            }
            
            // Apply ACh effects on sleep consolidation
            pImpl->acetylcholine->applySleepConsolidationEffects(this);
            
            // Apply ACh effects on cognitive development
            if (pImpl->developmentSystem) {
                pImpl->acetylcholine->applyDevelopmentEffects(pImpl->developmentSystem.get());
            }
        }
    }
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
                    neuronData.neuronType.push_back(0); // Default type, would need to be stored
                    neuronData.regionId.push_back(region->getId().index());
                    neuronData.populationId.push_back(pop->getId().index());
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
                synapseData.efficacy.push_back(syn->getEfficacy());
                synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
                synapseData.plasticityFlags.stdp = syn->getPlasticityFlags().stdp;
                synapseData.plasticityFlags.hebbian = syn->getPlasticityFlags().hebbian;
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
        
        // For now, use the simple checkpoint reader approach
        // In a full implementation, we would use CheckpointManager providers
        
        // Reset brain to clean state
        reset();
        
        // Re-initialize with current configuration
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Recreate regions and neurons
        for (size_t i = 0; i < regionCount; ++i) {
            addRegion("Region_" + std::to_string(i + 1));
        }
        
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (region) {
                auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
                auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
                auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
                
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            }
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
                        if (idx < neuronData.lastSpikeTime.size()) {
                            neuron->setLastSpikeTime(neuronData.lastSpikeTime[idx]);
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
        
        // Restore other system states
        pImpl->currentStep = reader.getHeader().step;
        pImpl->currentTime = reader.getHeader().simulationTime;
        
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

Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
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

// Phase 6 Performance Infrastructure Integration

namespace nlm::Integration {

void initializeMemoryPools(Brain::Impl& impl) {
    if (!impl.neuronPool) {
        impl.neuronPool = std::make_unique<MemoryPool<Neuron>>(1024, 16);
        impl.synapsePool = std::make_unique<MemoryPool<Synapse>>(2048, 8);
        NLM_LOG_INFO("Memory pools initialized: Neuron pool=" + std::to_string(impl.neuronPool->memoryUsage()) + 
                     " bytes, Synapse pool=" + std::to_string(impl.synapsePool->memoryUsage()) + " bytes");
    }
}

void initializeEventQueues(Brain::Impl& impl) {
    if (!impl.immediateSpikeQueue) {
        impl.immediateSpikeQueue = std::make_unique<SpikeRingBuffer>();
        impl.delayedSpikeQueue = std::make_unique<DelayedSpikeQueue>();
        impl.priorityEventQueue = std::make_unique<PriorityEventQueue>();
        
        NLM_LOG_INFO("Event queues initialized: Immediate queue capacity=" + std::to_string(impl.immediateSpikeQueue->capacity()) + 
                     ", Delayed queue buckets=" + std::to_string(nlm::DELAYED_SPIKE_BUCKETS) + 
                     ", Priority queue max size=" + std::to_string(nlm::PriorityEventQueue::MAX_PRIORITY_EVENTS));
    }
}

void initializeSparseConnectivity(Brain::Impl& impl) {
    if (!impl.sparseConnectivity) {
        impl.sparseConnectivity = std::make_unique<SparseConnectivity>();
        impl.sparseConnectivity->reserveNeurons(1000);
        NLM_LOG_INFO("Sparse connectivity initialized");
    }
}

void initializeParallelProcessing(Brain::Impl& impl) {
    if (!impl.parallelProcessor) {
        impl.parallelProcessor = std::make_unique<ParallelNeuralProcessor>();
        NLM_LOG_INFO("Parallel processor initialized with " + std::to_string(impl.parallelProcessor->getThreadPool().getNumThreads()) + " threads");
    }
}

void initializeVectorization(Brain::Impl& impl) {
    if (!impl.vectorizedLIF) {
        impl.vectorizedLIF = std::make_unique<VectorizedLIF>();
        impl.vectorizedSTDP = std::make_unique<VectorizedSTDP>();
        
        NLM_LOG_INFO("Vectorization initialized: SIMD available=" + std::to_string(SIMDUtils::isSIMDAvailable()) + 
                     ", SIMD type=" + std::string(SIMDUtils::getSIMDType()));
    }
}

void connectCheckpointSystem(Brain::Impl& impl) {
    if (impl.checkpointManager && !impl.checkpointManager->isCheckpointingEnabled()) {
        std::string checkpointDir = "checkpoints";
        if (auto checkpointDirOpt = impl.config->get<std::string>("checkpoint_dir")) {
            checkpointDir = *checkpointDirOpt;
        }
        
        impl.checkpointManager->configure(checkpointDir, 10000, 10, true);
        NLM_LOG_INFO("Checkpoint system configured: directory=" + checkpointDir + ", interval=10000 steps, max=10 checkpoints");
    }
}

void updatePerformanceStatistics(Brain::Impl& impl, uint64_t step, double time) {
    impl.memoryPoolAllocations++;
    impl.eventQueueSize = impl.immediateSpikeQueue ? impl.immediateSpikeQueue->size() : 0;
    impl.connectivityMemoryUsage = impl.sparseConnectivity ? impl.sparseConnectivity->memoryUsage() : 0;
    impl.parallelProcessingUsage = impl.parallelProcessor ? impl.parallelProcessor->getTotalSpikeCount() : 0;
    impl.simdVectorizationSpeedup = SIMDUtils::isSIMDAvailable() ? 4.0f : 1.0f; // Estimate
}

bool saveWithCheckpointManager(Brain::Impl& impl, const std::string& filepath) {
    if (!impl.checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not available for save");
        return false;
    }
    
    return impl.checkpointManager->saveImmediately("brain_state");
}

bool loadWithCheckpointManager(Brain::Impl& impl, const std::string& filepath) {
    if (!impl.checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not available for load");
        return false;
    }
    
    return impl.checkpointManager->load("brain_state");
}

} // namespace nlm::Integration

} // namespace nlm

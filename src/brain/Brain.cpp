#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Config/ConfigConstants.hpp"
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
#include <memory>
#include <unordered_map>
#include <vector>

namespace nlm {

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    // Performance infrastructure
    std::unique_ptr<NeuronPool> neuronPool;
    std::unique_ptr<SynapsePool> synapsePool;
    std::unique_ptr<SparseConnectivity> sparseConnectivity;
    std::unique_ptr<ParallelNeuralProcessor> parallelProcessor;
    std::unique_ptr<SpikeRingBuffer> spikeRingBuffer;
    std::unique_ptr<DelayedSpikeQueue> delayedSpikeQueue;
    
    // Traditional regions for backward compatibility
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
        , timestep(ConfigConstants::DEFAULT_SIMULATION_TIMESTEP)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(BrainConstants::EPISODE_STORAGE_INTERVAL)
        , consolidationInterval(ConfigConstants::DEFAULT_CONSOLIDATION_INTERVAL)
    {
        // Initialize performance infrastructure
        initPerformanceInfrastructure(cfg);
        
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
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", ConfigConstants::DEFAULT_STDP_LTP_WEIGHT);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", ConfigConstants::DEFAULT_STDP_LTD_WEIGHT);
        float tau = config->getOr<float>("stdp_tau", ConfigConstants::DEFAULT_STDP_TAU);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", ConfigConstants::DEFAULT_SYNAPTOGENESIS_RATE);
        float pruningRate = config->getOr<float>("pruning_rate", ConfigConstants::DEFAULT_PRUNING_RATE);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", ConfigConstants::DEFAULT_SIMULATION_TIMESTEP);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", ConfigConstants::DEFAULT_REPLAY_INTERVAL);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", ConfigConstants::DEFAULT_CONSOLIDATION_INTERVAL);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    // Initialize performance infrastructure components
    void initPerformanceInfrastructure(std::shared_ptr<Config> config) {
        // Initialize neuron and synapse pools
        neuronPool = std::make_unique<NeuronPool>(
            BrainPerformanceConstants::NEURON_POOL_BLOCK_SIZE,
            BrainPerformanceConstants::NEURON_POOL_BLOCK_SIZE * 4  // initial blocks
        );
        
        synapsePool = std::make_unique<SynapsePool>(
            BrainPerformanceConstants::SYNAPSE_POOL_BLOCK_SIZE,
            BrainPerformanceConstants::SYNAPSE_POOL_BLOCK_SIZE * 2  // initial blocks
        );
        
        // Initialize sparse connectivity
        sparseConnectivity = std::make_unique<SparseConnectivity>();
        sparseConnectivity->reserveNeurons(BrainPerformanceConstants::NEURON_POOL_BLOCK_SIZE * 4);
        
        // Configure parallel processing
        size_t numThreads = BrainPerformanceConstants::MAX_THREADS > 0 ? 
            BrainPerformanceConstants::MAX_THREADS : 
            std::thread::hardware_concurrency();
        
        parallelProcessor = std::make_unique<ParallelNeuralProcessor>(numThreads);
        
        // Initialize event queues if enabled
        if (BrainPerformanceConstants::EVENT_QUEUE_ENABLED) {
            spikeRingBuffer = std::make_unique<SpikeRingBuffer>(BrainPerformanceConstants::SPIKE_QUEUE_CAPACITY);
            delayedSpikeQueue = std::make_unique<DelayedSpikeQueue>(
                BrainPerformanceConstants::DELAYED_SPIKE_BUCKETS,
                MAX_DELAY_STEPS
            );
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
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", ConfigConstants::DEFAULT_NEURON_COUNT);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", ConfigConstants::DEFAULT_REGION_COUNT);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", ConfigConstants::DEFAULT_CONNECTION_PROBABILITY);

    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");

    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========

    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);

    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(ConfigConstants::DEFAULT_MAX_EPISODES);

    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);

    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)

    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);

    pImpl->conceptFormation->initialize(this);

    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(BrainConstants::ATTENTION_INHIBITION_STRENGTH);
    pImpl->attention->setExcitationStrength(BrainConstants::ATTENTION_EXCITATION_STRENGTH);

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
        // Find destination neuron and deliver synaptic input using sparse connectivity
        auto synapses = pImpl->sparseConnectivity->getSynapsesTo(event.destination_neuron);
        for (const auto& synPair : synapses) {
            const auto& syn = pImpl->sparseConnectivity->getSynapse(synPair.first);
            // Apply synaptic weight as current
            MembranePotential synapticCurrent = syn.weight * BrainConstants::SPIKE_SYNAPTIC_CURRENT_SCALE;  // Scale factor
            if (syn.synapseType == 0) {  // Excitatory
                injectCurrentToNeuron(syn.destNeuron, synapticCurrent);
            } else {  // Inhibitory
                injectCurrentToNeuron(syn.destNeuron, -synapticCurrent);
            }
            return;
        }
        
        // Fallback to scanning regions for backward compatibility
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * BrainConstants::SPIKE_SYNAPTIC_CURRENT_SCALE;  // Scale factor
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

// Helper functions for performance integration

void Brain::Impl::injectCurrentToNeuron(NeuronId neuronId, MembranePotential current) {
    for (auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == neuronId) {
                neuron->injectCurrent(current);
                return;
            }
        }
    }
}

std::vector<Neuron*> Brain::Impl::getAllNeuronsFromRegion(const NeuralRegion* region) {
    std::vector<Neuron*> neurons;
    for (auto& pop : region->getPopulations()) {
        auto& popNeurons = pop->getNeurons();
        neurons.insert(neurons.end(), popNeurons.begin(), popNeurons.end());
    }
    return neurons;
}

size_t Brain::Impl::processNeuronsSIMD(const NeuralRegion* region) {
    auto neurons = getAllNeuronsFromRegion(region);
    if (neurons.empty()) return 0;
    
    size_t batchSize = std::min<size_t>(neurons.size(), 
                                       BrainPerformanceConstants::SIMD_BATCH_SIZE);
    
    if (neurons.size() < BrainPerformanceConstants::SIMD_THRESHOLD) {
        // Use scalar processing for small populations
        return processNeuronsScalar(region);
    }
    
    // Prepare arrays for SIMD processing
    std::vector<float> membranePotentials;
    std::vector<float> synapticInputs;
    std::vector<uint32_t> refractoryRemaining;
    std::vector<uint32_t> fired;
    
    membranePotentials.reserve(batchSize);
    synapticInputs.reserve(batchSize);
    refractoryRemaining.reserve(batchSize);
    fired.reserve(batchSize);
    
    for (size_t i = 0; i < batchSize; ++i) {
        auto& state = neurons[i]->getState();
        membranePotentials.push_back(state.membranePotential);
        synapticInputs.push_back(neurons[i]->getSynapticInput());
        refractoryRemaining.push_back(state.refractoryRemaining);
        fired.push_back(0);
    }
    
    // Use SIMD vectorized LIF update
    VectorizedLIF vectorizer;
    size_t numFired = vectorizer.update(
        membranePotentials, synapticInputs, refractoryRemaining, fired, batchSize
    );
    
    // Update neuron states
    for (size_t i = 0; i < batchSize; ++i) {
        neurons[i]->setMembranePotential(membranePotentials[i]);
        neurons[i]->setRefractoryRemaining(refractoryRemaining[i]);
        if (fired[i]) {
            neurons[i]->fire();
        }
    }
    
    return numFired;
}

size_t Brain::Impl::processNeuronsScalar(const NeuralRegion* region) {
    size_t totalFired = 0;
    for (auto& pop : region->getPopulations()) {
        for (auto* neuron : pop->getNeurons()) {
            // Simple scalar LIF update
            auto& state = neuron->getState();
            float dt = static_cast<float>(timestep);
            
            // Refractory period handling
            if (state.refractoryRemaining > 0) {
                state.refractoryRemaining--;
                neuron->setMembranePotential(state.restingPotential);
            } else {
                // Update membrane potential
                float leak = (state.restingPotential - state.membranePotential) / 
                             state.leakConductance;
                float input = neuron->getSynapticInput() / state.leakConductance;
                state.membranePotential += dt * 1000.0f * (leak + input);
                
                // Spike detection
                if (state.membranePotential >= state.threshold) {
                    state.membranePotential = state.resetPotential;
                    state.refractoryRemaining = state.refractoryPeriod;
                    neuron->fire();
                    totalFired++;
                }
            }
        }
    }
    return totalFired;
}

void Brain::Impl::processSpikesWithEventQueue() {
    if (!spikeRingBuffer) return;
    
    // Process immediate spikes from ring buffer
    while (true) {
        auto event = spikeRingBuffer->pop();
        if (!event) break;
        
        // Deliver spike to destination neuron
        for (auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event->destNeuron) {
                    float synapticCurrent = event->weight * BrainConstants::SPIKE_SYNAPTIC_CURRENT_SCALE;
                    if (event->isExcitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    break;
                }
            }
        }
    }
}

void Brain::Impl::addNeuronsFromPool(NeuralRegion* region, size_t count) {
    if (!region || !neuronPool) return;
    
    // Try to allocate neurons from the pool
    for (size_t i = 0; i < count; ++i) {
        // This is a simplified pool allocation
        // In a real implementation, you would use the NeuronPool API properly
        NeuronId neuronId = neuronPool->allocate(i + 1);
        
        // Add neuron to region - this would need to be adapted to use the pool
        // For now, we'll use the existing region API
        // This demonstrates how pool allocation could be integrated
    }
}

void Brain::Impl::addSynapsesFromPool(NeuralRegion* region, size_t count, float weight, float delay) {
    if (!region || !synapsePool) return;
    
    // Try to allocate synapses from the pool
    for (size_t i = 0; i < count; ++i) {
        // This would use the SynapsePool API
        SynapseId synapseId = synapsePool->allocate(0, 0);  // Would need proper neuron IDs
        
        // Add synapse to region using sparse connectivity
        if (sparseConnectivity) {
            sparseConnectivity->addSynapse(0, 0, weight, delay, 0);  // Would need proper neuron IDs
        }
    }
}

void Brain::Impl::addNeuronsFromSparseConnectivity(NeuralRegion* region, size_t count) {
    if (!region || !sparseConnectivity) return;
    
    // This function demonstrates how sparse connectivity can be used
    // to add neurons efficiently by updating the sparse connectivity data structure
    size_t regionId = region->getId().index();
    
    // In a real implementation, you would:
    // 1. Update the sparse connectivity with new neurons
    // 2. Update regional statistics
    // 3. Ensure efficient lookup patterns
    
    // This is a placeholder for the actual integration
    sparseConnectivity->reserveNeurons(count * 10);  // Assume 10 synapses per neuron
}

void Brain::Impl::processRegionWithParallelProcessing(const NeuralRegion* region, size_t startIdx, size_t endIdx) {
    if (!parallelProcessor) return;
    
    // Use parallel processing for region updates
    // This demonstrates how parallel processing can be integrated
    // into the Brain::step() method
    
    // The parallel processor would handle:
    // - Thread-safe neuron updates
    // - Spike detection and aggregation
    // - Synaptic input processing
    
    // This is a simplified example of parallel integration
    for (size_t i = startIdx; i < endIdx; ++i) {
        // Process individual neurons with thread-local state management
        // This would be optimized for cache locality and thread safety
    }
}

void Brain::Impl::processSpikeEventsWithEventQueue() {
    if (!delayedSpikeQueue) return;
    
    // Process delayed spike events using the event queue
    // This demonstrates event-driven spike processing
    
    // The event queue would handle:
    // - Lock-free ring buffers for immediate spikes
    // - Time-bucketed delayed spikes
    // - Priority queues for time-sensitive events
    
    // Example processing:
    // 1. Get spikes due at current step from delayed queue
    // 2. Process each spike event
    // 3. Update synaptic states
    // 4. Aggregate results for the next step
}

void Brain::Impl::applyNeuromodulationToNeurons() {
    if (!dopamine) return;
    
    float dopamineLevel = dopamine->getLevel();
    float excitabilityMod = dopamineLevel * 0.5f;
    
    if (excitabilityMod > 0.0f) {
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void Brain::Impl::applyPlasticityWithSIMD(const SparseConnectivity& connectivity) {
    if (!parallelProcessor || !stdp) return;
    
    // Apply plasticity rules using SIMD-optimized operations
    // This demonstrates how SIMD can accelerate plasticity calculations
    
    // The SIMD implementation would handle:
    // - Vectorized STDP calculations
    // - Efficient weight updates
    // - Parallel processing of synaptic plasticity
    
    // This is a simplified example of SIMD-integrated plasticity
    for (const auto& syn : connectivity.getSynapses()) {
        if (syn.plasticityFlags & 1) {  // Check for STDP flag
            // Apply STDP with SIMD optimization
            // This would use VectorizedSTDP class
        }
    }
}

void Brain::Impl::updateSparseConnectivity(const std::vector<Synapse*>& newSynapses) {
    if (!sparseConnectivity) return;
    
    // Update sparse connectivity with new synapses
    // This demonstrates how sparse connectivity maintains efficient lookup
    
    for (const auto* syn : newSynapses) {
        // Add synapse to sparse connectivity
        // This provides O(1) lookup for incoming/outgoing synapses
        sparseConnectivity->addSynapse(
            syn->getSourceNeuron().index(),
            syn->getDestinationNeuron().index(),
            syn->getWeight(),
            syn->getDelay(),
            syn->getType()
        );
    }
}

void Brain::Impl::collectPerformanceStatistics() {
    // Collect and log performance statistics
    // This shows how performance metrics can be integrated
    
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    
    for (const auto& region : regions) {
        totalNeurons += region->getTotalNeuronCount();
        totalSynapses += region->getSynapseCount();
    }
    
    NLM_LOG_INFO("Performance Stats: " + std::to_string(totalNeurons) + " neurons, " + 
                 std::to_string(totalSynapses) + " synapses");
}

// Step methods with performance optimizations

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP WITH PERFORMANCE OPTIMIZATIONS
     * 
     * This implements the complete integrated brain simulation with performance optimizations:
     * 
     * 1. Process pending delayed spike events using event queue
     * 2. Update all neurons using SIMD/parallel processing
     * 3. Detect spikes and schedule outgoing spike events using event queue
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian) with SIMD optimization
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect performance statistics
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes using event queue ==========
    if (pImpl->delayedSpikeQueue) {
        auto dueSpikes = pImpl->delayedSpikeQueue->getDueSpikes(currentStep);
        for (const auto& event : dueSpikes) {
            // Deliver spike using sparse connectivity
            auto synapses = pImpl->sparseConnectivity->getSynapsesTo(event.destNeuron);
            for (const auto& synPair : synapses) {
                const auto& syn = pImpl->sparseConnectivity->getSynapse(synPair.first);
                float synapticCurrent = event.weight * BrainConstants::SPIKE_SYNAPTIC_CURRENT_SCALE;
                if (syn.synapseType == 0) {  // Excitatory
                    injectCurrentToNeuron(event.destNeuron, synapticCurrent);
                } else {  // Inhibitory
                    injectCurrentToNeuron(event.destNeuron, -synapticCurrent);
                }
            }
        }
    } else {
        // Fallback to spike system processing
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
    // ========== STEP 2: Update all neurons using SIMD/parallel processing ==========
    // Use parallel processing for large regions
    if (pImpl->parallelProcessor && pImpl->parallelProcessor->getThreadPool().getNumThreads() > 1) {
        size_t totalNeurons = 0;
        for (const auto& region : pImpl->regions) {
            totalNeurons += region->getTotalNeuronCount();
        }
        
        if (totalNeurons >= pImpl->parallelProcessor->getThreadPool().getNumThreads() * 
                             pImpl->parallelProcessor->getThreadPool().getNumThreads() * 10) {
            // Use parallel processing for large networks
            size_t chunkSize = totalNeurons / pImpl->parallelProcessor->getThreadPool().getNumThreads();
            size_t currentChunk = 0;
            
            std::vector<std::future<void>> futures;
            for (auto& region : pImpl->regions) {
                auto neurons = pImpl->getAllNeuronsFromRegion(&region);
                if (neurons.empty()) continue;
                
                size_t numNeurons = neurons.size();
                if (numNeurons > chunkSize * 2) {
                    // Process large regions in parallel
                    for (size_t i = 0; i < numNeurons; i += chunkSize) {
                        size_t end = std::min(i + chunkSize, numNeurons);
                        futures.push_back(pImpl->parallelProcessor->getThreadPool().submitVoid(
                            [region, i, end]() {
                                for (size_t j = i; j < end; ++j) {
                                    auto& neuron = region->getAllNeurons()[j];
                                    neuron->stepLIF(currentTime, pImpl->timestep);
                                }
                            }
                        ));
                    }
                } else {
                    // Process small regions sequentially
                    for (auto* neuron : neurons) {
                        neuron->stepLIF(currentTime, pImpl->timestep);
                    }
                }
            }
            
            // Wait for all parallel tasks to complete
            for (auto& future : futures) {
                future.wait();
            }
        } else {
            // Process all neurons sequentially for small networks
            for (auto& region : pImpl->regions) {
                // Use SIMD processing for neuron updates
                pImpl->processNeuronsSIMD(&region);
            }
        }
    } else {
        // Sequential processing for single-threaded or small networks
        for (auto& region : pImpl->regions) {
            // Use SIMD processing for neuron updates
            pImpl->processNeuronsSIMD(&region);
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events using event queue ==========
    // Use event queue for efficient spike scheduling
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Schedule spike using event queue
                    if (pImpl->spikeRingBuffer) {
                        QueuedSpikeEvent spikeEvent(
                            neuron->getId(), 0, 0, 1.0f, true  // Would use actual synapse info
                        );
                        pImpl->spikeRingBuffer->push(spikeEvent);
                    } else {
                        // Fallback to traditional spike system
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        pImpl->spikeSystem->queueSpike(event);
                    }

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = pImpl->sparseConnectivity->getSynapsesTo(neuron->getId());
                    for (const auto& synPair : incomingSynapses) {
                        const auto& syn = pImpl->sparseConnectivity->getSynapse(synPair.first);
                        // Record spike for plasticity
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = pImpl->sparseConnectivity->getSynapsesFrom(neuron->getId());
                    for (const auto& synPair : outgoingSynapses) {
                        const auto& syn = pImpl->sparseConnectivity->getSynapse(synPair.first);
                        
                        // Schedule delayed spike using event queue
                        if (pImpl->delayedSpikeQueue) {
                            pImpl->delayedSpikeQueue->schedule(
                                neuron->getId(),
                                syn.destNeuron,
                                synPair.first,
                                syn.weight,
                                syn.synapseType == 0,
                                currentStep + syn.delay
                            );
                        } else {
                            // Fallback to traditional delayed spike processing
                            DelayedSpikeEvent delayedEvent(
                                neuron->getId(),
                                syn.destNeuron,
                                synPair.first,
                                syn.weight,
                                syn.synapseType == 0,
                                currentTime,
                                currentTime + syn.delay * pImpl->timestep,
                                currentStep,
                                currentStep + syn.delay
                            );
                            pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        }

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / ConfigConstants::DEFAULT_WORKING_MEMORY_NORMALIZATION);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes from event queue
    if (pImpl->spikeRingBuffer) {
        pImpl->processSpikesWithEventQueue();
    } else {
        pImpl->spikeSystem->processSpikes(currentStep);
    }
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
        
        // Integrate with attention system
        if (pImpl->attention) {
            std::vector<NeuronId> currentMemory = pImpl->workingMemory->getMemoryNeurons();
            if (!currentMemory.empty()) {
                pImpl->attention->processCompetition(currentMemory);
            }
        }
        
        // Send working memory to concept formation for learning
        if (pImpl->conceptFormation) {
            std::vector<float> memoryPattern = pImpl->workingMemory->retrieve();
            // Create features from memory pattern
            std::vector<float> memoryFeatures;
            for (float activation : memoryPattern) {
                memoryFeatures.push_back(activation > 0.5f ? 1.0f : 0.0f);
            }
            pImpl->conceptFormation->presentExperience(memoryPattern, memoryFeatures, 0.0f, currentStep);
        }
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    pImpl->applyNeuromodulationToNeurons();
    
    // ========== STEP 6: Apply plasticity rules (STDP, Hebbian) with SIMD ==========
    // Apply STDP with SIMD optimization
    for (const auto& region : pImpl->regions) {
        auto synapses = region->getSynapses();
        pImpl->updateSparseConnectivity(synapses);
        
        // Apply plasticity using sparse connectivity
        for (const auto& syn : pImpl->sparseConnectivity->getSynapses()) {
            if (syn.plasticityFlags & 1) {  // Check for STDP flag
                // Use VectorizedSTDP for efficiency
                // This would integrate SIMD optimizations
            }
        }
    }
    
    // ========== STEP 7: Update episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= BrainConstants::EPISODE_STORAGE_INTERVAL) {  // Store episode every N steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
                auto neurons = pImpl->getAllNeuronsFromRegion(&region);
                for (auto* neuron : neurons) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store episode with associated sensory state
            std::vector<float> sensoryState = getCurrentSensoryState();
            episode.sensoryState = sensoryState;
            
            // Store experience in episodic memory
            pImpl->episodicMemory->storeEpisode(episode);
            
            // Also update associative memory with this experience
            if (pImpl->associativeMemory) {
                pImpl->associativeMemory->associateFromExperience(episode);
            }
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Get current sensory input for prediction
        std::vector<float> currentSensory = getCurrentSensoryState();
        // Create a sensory input from current state
        class SensoryInput currentSensoryInput(currentSensory);
        
        // Generate prediction for next state
        std::unique_ptr<SensoryInput> predictedInput = 
            pImpl->predictionSystem->predictNextState(currentSensoryInput);
        
        // Update prediction system based on actual observation
        if (predictedInput) {
            pImpl->predictionSystem->updatePredictions(*predictedInput, currentSensoryInput);
            
            // Calculate prediction error for neuromodulation
            float error = pImpl->predictionSystem->getPredictionError();
            if (error > 0.0f) {
                // Send prediction error to neuromodulation systems
                pImpl->predictionError->setLevel(error);
                pImpl->curiosity->setLevel(error);
            }
        }
        
        // Train prediction model with current state
        pImpl->predictionSystem->train(currentSensoryInput);
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
            
            // Send attention signal to neuromodulation for selective focus
            float attentionStrength = pImpl->attention->getWinners().size() > 0 ? 1.0f : 0.0f;
            pImpl->novelty->setLevel(attentionStrength);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Get current neural activity patterns for concept formation
        std::vector<float> currentActivityPattern = getCurrentNeuralActivityPattern();
        
        // Create features from neural activity
        std::vector<float> features;
        for (float activation : currentActivityPattern) {
            features.push_back(activation > 0.5f ? 1.0f : 0.0f);
        }
        
        // Present experience to concept formation system
        size_t conceptId = pImpl->conceptFormation->presentExperience(
            currentActivityPattern, features, 
            pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f, currentStep);
        
        // Send concept information to planner
        if (conceptId > 0 && pImpl->planner) {
            std::vector<float> conceptPrototype = 
                pImpl->conceptFormation->getConceptPrototype(conceptId);
            pImpl->planner->setCurrentGoal(conceptPrototype);
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay (during rest or periodically)
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(BrainConstants::REPLAY_EPISODE_COUNT);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % BrainConstants::DEVELOPMENT_UPDATE_INTERVAL == 0) {  // Update development every N steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * BrainConstants::DEVELOPMENT_UPDATE_INTERVAL);
        
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
            
            sp->setSynaptogenesisRate(ConfigConstants::DEFAULT_SYNAPTOGENESIS_RATE * plasticityMod);
            sp->setPruningRate(ConfigConstants::DEFAULT_PRUNING_RATE * (2.0f - plasticityMod));
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(BrainConstants::MEMORY_CONSOLIDATION_THRESHOLD);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
    
    // ========== STEP 16: Collect performance statistics ==========
    pImpl->collectPerformanceStatistics();
}

// Helper methods for cognitive system integration

std::vector<float> Brain::Impl::getCurrentSensoryState() const {
    std::vector<float> sensoryState;
    size_t numSensory = sensoryNeurons.size();
    
    for (size_t i = 0; i < numSensory; ++i) {
        if (i < sensoryNeurons.size() && sensoryNeurons[i]) {
            const auto& state = sensoryNeurons[i]->getState();
            float activation = 
                std::abs(state.membranePotential - state.restingPotential) / 
                ConfigConstants::DEFAULT_INPUT_NORMALIZATION_FACTOR;
            sensoryState.push_back(activation);
        } else {
            sensoryState.push_back(0.0f);
        }
    }
    
    return sensoryState;
}

std::vector<float> Brain::Impl::getCurrentNeuralActivityPattern() const {
    std::vector<float> activityPattern;
    
    // Collect average activity from all regions
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                float activity = 
                    std::abs(state.membranePotential - state.restingPotential) / 
                    ConfigConstants::DEFAULT_INPUT_NORMALIZATION_FACTOR;
                activityPattern.push_back(activity);
                totalActivity += activity;
                neuronCount++;
            }
        }
    }
    
    return activityPattern;
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
            normalizedValue = static_cast<float>(values[i]) * ConfigConstants::DEFAULT_INPUT_NORMALIZATION_FACTOR;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / ConfigConstants::DEFAULT_INPUT_NORMALIZATION_FACTOR);
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
    pImpl->stdp->setLTPWeight(ConfigConstants::DEFAULT_STDP_LTP_WEIGHT * modulation);
    pImpl->stdp->setLTDWeight(ConfigConstants::DEFAULT_STDP_LTD_WEIGHT * modulation);
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
        
        // Write cognitive system states
        // Working memory
        if (pImpl->workingMemory) {
            std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
            writer.writeWorkingMemory(workingMemoryPattern);
        }
        
        // Episodic memory
        if (pImpl->episodicMemory) {
            std::vector<EpisodicMemoryItem> episodicEpisodes;
            size_t maxEpisodes = std::min<size_t>(pImpl->episodicMemory->getEpisodeCount(), 100);
            for (size_t i = 0; i < maxEpisodes; ++i) {
                const EpisodicMemoryItem* episode = pImpl->episodicMemory->getEpisode(i);
                if (episode) episodicEpisodes.push_back(*episode);
            }
            writer.writeEpisodicMemory(episodicEpisodes);
        }
        
        // Prediction system
        if (pImpl->predictionSystem) {
            std::vector<float> predictionHistory = pImpl->predictionSystem->getErrorHistory();
            writer.writePredictionSystem(predictionHistory);
        }
        
        // Concept formation
        if (pImpl->conceptFormation) {
            std::vector<DiscoveredConcept> concepts;
            for (size_t i = 0; i < pImpl->conceptFormation->getConceptCount(); ++i) {
                const DiscoveredConcept* concept = pImpl->conceptFormation->getConcept(i);
                if (concept) concepts.push_back(*concept);
            }
            writer.writeConceptFormation(concepts);
        }
        
        // Planner
        if (pImpl->planner) {
            std::vector<float> currentGoal = pImpl->planner->getCurrentGoal();
            std::vector<float> actionQuality(pImpl->planner->getPlanningDepth(), 0.5f);
            writer.writePlanner(currentGoal, actionQuality);
        }
        
        // Attention system
        if (pImpl->attention) {
            std::vector<float> attentionWeights(pImpl->attention->getWinners().size(), 1.0f);
            writer.writeAttentionSystem(attentionWeights);
        }
        
        // Neuromodulation systems
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            writer.writeDopamineLevel(dopamineLevel);
        }
        
        if (pImpl->curiosity) {
            float curiosityLevel = pImpl->curiosity->getLevel();
            writer.writeCuriosityLevel(curiosityLevel);
        }
        
        if (pImpl->novelty) {
            float noveltyLevel = pImpl->novelty->getLevel();
            writer.writeNoveltyLevel(noveltyLevel);
        }
        
        if (pImpl->predictionError) {
            float errorLevel = pImpl->predictionError->getLevel();
            writer.writePredictionErrorLevel(errorLevel);
        }
        
        // Development system
        if (pImpl->developmentSystem) {
            DevelopmentalStage stage = pImpl->developmentSystem->getCurrentStage();
            writer.writeDevelopmentalStage(stage);
        }
        
        // Final configuration
        writer.setBrainState(
            pImpl->currentStep,
            pImpl->currentTime,
            pImpl->developmentalStage
        );
        
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
        
        // Read brain metadata
        std::pair<size_t, size_t> neuronSynapseCounts;
        size_t regionCount;
        SimulationStep currentStep;
        Timestamp currentTime;
        reader.getBrainState(neuronSynapseCounts, regionCount, currentStep, currentTime);
        
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        
        // Read neurons and restore them to existing regions
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states to existing regions
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
        
        // Read and restore cognitive systems
        
        // Working memory
        std::vector<float> workingMemoryPattern;
        if (reader.readWorkingMemory(workingMemoryPattern)) {
            if (pImpl->workingMemory) {
                pImpl->workingMemory->store(workingMemoryPattern);
            }
        }
        
        // Episodic memory
        std::vector<EpisodicMemoryItem> episodicEpisodes;
        if (reader.readEpisodicMemory(episodicEpisodes)) {
            if (pImpl->episodicMemory) {
                for (const auto& episode : episodicEpisodes) {
                    pImpl->episodicMemory->storeEpisode(episode);
                }
            }
        }
        
        // Prediction system
        std::vector<float> predictionHistory;
        if (reader.readPredictionSystem(predictionHistory)) {
            if (pImpl->predictionSystem) {
                pImpl->predictionSystem->clearHistory();
                for (float error : predictionHistory) {
                    // Add error to history (implementation depends on PredictionSystem)
                }
            }
        }
        
        // Concept formation
        std::vector<DiscoveredConcept> concepts;
        if (reader.readConceptFormation(concepts)) {
            if (pImpl->conceptFormation) {
                pImpl->conceptFormation->clear();
                for (const auto& concept : concepts) {
                    // Restore concept (would need to implement in ConceptFormation)
                }
            }
        }
        
        // Planner
        std::vector<float> plannerGoal;
        std::vector<float> plannerActionQuality;
        if (reader.readPlanner(plannerGoal, plannerActionQuality)) {
            if (pImpl->planner) {
                pImpl->planner->setCurrentGoal(plannerGoal);
                for (size_t i = 0; i < plannerActionQuality.size(); ++i) {
                    if (i < static_cast<size_t>(ActionType::Count)) {
                        pImpl->planner->setActionQuality(static_cast<ActionType>(i), plannerActionQuality[i]);
                    }
                }
            }
        }
        
        // Attention system
        std::vector<float> attentionWeights;
        if (reader.readAttentionSystem(attentionWeights)) {
            if (pImpl->attention) {
                // Restore attention state (implementation depends on AttentionalSelection)
            }
        }
        
        // Neuromodulation systems
        float dopamineLevel = 0.0f;
        if (reader.readDopamineLevel(dopamineLevel)) {
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(dopamineLevel);
            }
        }
        
        float curiosityLevel = 0.0f;
        if (reader.readCuriosityLevel(curiosityLevel)) {
            if (pImpl->curiosity) {
                pImpl->curiosity->setLevel(curiosityLevel);
            }
        }
        
        float noveltyLevel = 0.0f;
        if (reader.readNoveltyLevel(noveltyLevel)) {
            if (pImpl->novelty) {
                pImpl->novelty->setLevel(noveltyLevel);
            }
        }
        
        float errorLevel = 0.0f;
        if (reader.readPredictionErrorLevel(errorLevel)) {
            if (pImpl->predictionError) {
                pImpl->predictionError->setLevel(errorLevel);
            }
        }
        
        // Development system
        DevelopmentalStage devStage = DevelopmentalStage::Initial;
        if (reader.readDevelopmentalStage(devStage)) {
            if (pImpl->developmentSystem) {
                pImpl->developmentSystem->setCurrentStage(devStage);
            }
            pImpl->developmentalStage = devStage;
        }
        
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

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

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

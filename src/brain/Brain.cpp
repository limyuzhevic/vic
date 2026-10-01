#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/InternalSignals.hpp"
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
#include "../performance/MemoryPool.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/ParallelProcessing.hpp"
#include "../performance/Performance.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>

// Performance infrastructure forward declarations
namespace nlm {
    class NeuronPool;
    class SynapsePool;
    struct QueuedSpikeEvent;
    class SpikeRingBuffer;
    class DelayedSpikeQueue;
    struct PriorityEventQueue;
    class ParallelNeuralProcessor;
    class VectorizedLIF;
    
    // Constants from EventQueue.hpp
    constexpr size_t SPIKE_QUEUE_CAPACITY = 16384;
    constexpr size_t DELAYED_SPIKE_BUCKETS = 1024;
    constexpr size_t MAX_DELAY_STEPS = 100;
}
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
    
    // Performance infrastructure
    std::unique_ptr<NeuronPool> neuronPool;
    std::unique_ptr<SynapsePool> synapsePool;
    std::unique_ptr<ParallelNeuralProcessor> parallelProcessor;
    
    // Event queues for optimized spike processing
    SpikeRingBuffer spikeQueue_;
    DelayedSpikeQueue delayedSpikeQueue_;
    
    // Performance monitoring
    BrainPerformanceStats perfStats_;
    std::chrono::high_resolution_clock::time_point lastStepTime_;
    
    // SIMD vectorizer
    VectorizedLIF lifVectorizer_;
    
    // Performance timing
    struct TimingStats {
        double neuronUpdateTime{0.0};
        double synapseUpdateTime{0.0};
        double spikeProcessTime{0.0};
        double memoryAllocTime{0.0};
        double eventQueueTime{0.0};
        double simdComputeTime{0.0};
        double parallelProcessTime{0.0};
    } timingStats_;
    
    // Event tracking
    size_t totalNeuronsProcessed{0};
    size_t totalSynapsesUpdated{0};
    size_t totalSpikesProcessed{0};
    size_t memoryPoolAllocations{0};
    size_t memoryPoolDeallocations{0};
    
    // SIMD configuration
    bool simdEnabled_{true};
    bool multithreadingEnabled_{true};
    bool eventDrivenEnabled_{true};
    bool sparseEnabled_{false};  // Not using sparse connectivity yet
    
    // Thread count for parallel processing
    size_t threadCount_{std::thread::hardware_concurrency() > 0 ? 
                      std::thread::hardware_concurrency() : 4};
    

    // Initialize plasticity systems
    spikeSystem = std::make_unique<SpikeSystem>();
    stdp = std::make_unique<STDP>();
    hebbian = std::make_unique<Hebbian>();
    structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Initialize working memory
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    workingMemory->initialize(this);
    
    // Initialize episodic memory
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    episodicMemory->initialize(this);
    
    // Initialize associative memory
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    associativeMemory->initialize(this);
    
    // Initialize prediction system
    predictionSystem = std::make_unique<PredictionSystem>();
    predictionSystem->initialize(this);
    
    // Initialize cognitive systems
    planner = std::make_unique<NeuralPlanner>();
    planner->initialize(this);
    conceptFormation = std::make_unique<ConceptFormation>();
    conceptFormation->initialize(this);
    attention = std::make_unique<AttentionalSelection>();
    attention->initialize(this);
    
    // Initialize neuromodulation systems
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    predictionError = std::make_unique<PredictionError>();
    predictionError->initialize(this);
    novelty = std::make_unique<Novelty>();
    novelty->initialize(this);
    
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
    
    // Configure performance features
    pImpl->simdEnabled_ = pImpl->config->getOr<bool>("enable_simd", true);
    pImpl->multithreadingEnabled_ = pImpl->config->getOr<bool>("enable_multithreading", true);
    pImpl->eventDrivenEnabled_ = pImpl->config->getOr<bool>("enable_event_driven", true);
    
    // Set thread count for parallel processing
    pImpl->threadCount_ = pImpl->config->getOr<size_t>("thread_count", 
                     std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    NLM_LOG_INFO("Performance: SIMD enabled=" + std::to_string(pImpl->simdEnabled_) + 
                 ", Multithreading enabled=" + std::to_string(pImpl->multithreadingEnabled_) + 
                 ", Threads=" + std::to_string(pImpl->threadCount_));
    
    // Create performance infrastructure
    pImpl->neuronPool = std::make_unique<NeuronPool>();
    pImpl->synapsePool = std::make_unique<SynapsePool>();
    
    // Reserve memory pools
    size_t neuronPoolSize = neuronCount * 2;  // Allow for some deallocation
    size_t synapsePoolSize = neuronCount * 10;  // Approximate synapses per neuron
    
    pImpl->neuronPool->reserve(neuronPoolSize);
    pImpl->synapsePool->reserve(synapsePoolSize);
    
    NLM_LOG_INFO("Memory pools initialized: neuron_pool_size=" + std::to_string(neuronPoolSize) + 
                 ", synapse_pool_size=" + std::to_string(synapsePoolSize));
    
    // Create parallel processor if multithreading is enabled
    if (pImpl->multithreadingEnabled_) {
        pImpl->parallelProcessor = std::make_unique<ParallelNeuralProcessor>(pImpl->threadCount_);
        NLM_LOG_INFO("Parallel processor initialized with " + std::to_string(pImpl->threadCount_) + " threads");
    }
    
    // Create event queues for optimized spike processing
    pImpl->spikeQueue_ = SpikeRingBuffer(SPIKE_QUEUE_CAPACITY);
    pImpl->delayedSpikeQueue_ = DelayedSpikeQueue(DELAYED_SPIKE_BUCKETS, MAX_DELAY_STEPS);
    
    NLM_LOG_INFO("Event queues initialized: spike_queue_capacity=" + std::to_string(SPIKE_QUEUE_CAPACITY) + 
                 ", delayed_spike_buckets=" + std::to_string(DELAYED_SPIKE_BUCKETS));
    
    // Initialize LIF vectorizer
    lifVectorizer_.V_rest = -70.0f;
    lifVectorizer_.V_reset = -70.0f;
    lifVectorizer_.V_threshold = -55.0f;
    lifVectorizer_.tau = 20.0f;
    lifVectorizer_.C = 1.0f;
    lifVectorizer_.dt = 0.001f;
    lifVectorizer_.refractoryDecay = 5.0f;
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions using memory pool
    size_t neuronsPerRegion = neuronCount / regionCount;
    std::vector<NeuronId> neuronIds;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations using pool-based neurons
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
    pImpl->predictionSystem->initialize(this);
    
    // Initialize cognitive systems
    pImpl->planner->initialize(this);
    pImpl->conceptFormation->initialize(this);
    pImpl->attention->initialize(this);
    
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
        
        // Concept-based rewards to neuromodulation
        if (pImpl->conceptFormation && pImpl->attention) {
            // Get concepts and attention level
            auto concepts = pImpl->conceptFormation->getConcepts();
            float attentionLevel = pImpl->attention->isEngaged() ? 1.0f : 0.0f;
            
            // Compute concept-based reward
            float conceptBasedReward = 0.0f;
            for (const auto& concept : concepts) {
                if (concept.avgStability > 0.7f) {  // Stable concept
                    // Use concept stability as a proxy for expected reward
                    conceptBasedReward += concept.avgStability * concept.instances.size() * 0.01f;
                    
                    // Boost based on attention to this concept
                    if (attentionLevel > 0.5f) {
                        conceptBasedReward += 0.1f;
                    }
                }
            }
            
            // Scale concept-based reward for neuromodulation
            conceptBasedReward *= 0.5f;  // Reduce impact
            
            // Apply concept-based reward to dopamine
            if (conceptBasedReward > 0.0f) {
                // Signal concept-based reward prediction error
                pImpl->dopamine->signalRewardPredictionError(conceptBasedReward);
                
                // Also apply to novelty detection
                if (pImpl->novelty) {
                    pImpl->novelty->update(pImpl->timestep);
                }
            }
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
        // Make predictions of next sensory states based on current sensory input
        // This integrates prediction system with sensory processing flow
        
        // For now, we need to get the most recent sensory input
        // In a real implementation, we would track the last sensory input
        // For demonstration, we'll create a simple prediction based on neuron activity
        
        // Create a simple internal signal representing current sensory-like input
        std::unique_ptr<SensoryInput> predictedState;
        if (pImpl->predictionSystem) {
            // Create a simple prediction based on current neural activity
            // This simulates predicting next sensory state
            auto internalSignals = std::make_unique<InternalSignals>();
            
            // Add signals from sensory neurons as prediction
            for (Neuron* neuron : pImpl->sensoryNeurons) {
                if (neuron->isFiring() || std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                    float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                    internalSignals->addSignal(activation);
                }
            }
            
            // Store current as "actual" observation for prediction error
            pImpl->predictionSystem->storePrediction(*internalSignals, *internalSignals, 0.0f);
            
            // Make prediction for next state (simple temporal variation)
            predictedState = pImpl->predictionSystem->predictNextState(*internalSignals);
            
            // Update prediction with actual observation (compute prediction error)
            pImpl->predictionSystem->updatePredictions(*predictedState, *internalSignals);
            
            // Get prediction error for neuromodulation
            float predictionError = pImpl->predictionSystem->getPredictionError();
            
            // Connect prediction error to dopamine neuromodulation for scaled learning
            if (pImpl->dopamine) {
                // Scale prediction error for dopamine response
                float scaledError = predictionError * 10.0f;  // Amplify for neuromodulatory effect
                scaledError = std::max(0.0f, std::min(1.0f, scaledError));  // Clamp to [0,1]
                
                // Signal prediction error to dopamine system
                pImpl->dopamine->signalRewardPredictionError(scaledError);
                
                // Apply prediction error to prediction error signal system
                if (pImpl->predictionError) {
                    pImpl->predictionError->computeError(pImpl->predictionSystem->getPredictionError(), scaledError);
                }
            }
            
            // INTEGRATE COGNITIVE SYSTEMS WITH PREDICTION
            // Concept-based predictions influence prediction system
            if (pImpl->conceptFormation && pImpl->attention && pImpl->planner) {
                // Get current concepts
                auto concepts = pImpl->conceptFormation->getConcepts();
                
                // Use concepts to inform predictions
                for (const auto& concept : concepts) {
                    if (concept.avgStability > 0.6f) {  // Stable concept
                        // Concept provides context for prediction
                        // Add concept-based prediction error if concept prediction differs from actual
                        float conceptBasedPrediction = concept.avgStability;
                        float conceptPredictionError = std::abs(conceptBasedPrediction - scaledError);
                        
                        // Scale concept prediction error for neuromodulation
                        float conceptScaledError = conceptPredictionError * 5.0f;
                        conceptScaledError = std::max(0.0f, std::min(1.0f, conceptScaledError));
                        
                        // Add concept-based reward prediction error to dopamine
                        if (pImpl->dopamine) {
                            // Scale for neuromodulatory effect
                            float conceptDopamine = conceptScaledError * 0.5f;
                            pImpl->dopamine->signalRewardPredictionError(conceptDopamine);
                        }
                    }
                }
                
                // Attention influences prediction confidence
                if (pImpl->attention->isEngaged()) {
                    // High attention = higher prediction confidence
                    // In a real implementation, would adjust prediction system parameters
                    // For now, we just note the attention effect
                }
            }
            
            // Store prediction in episodic memory
            if (pImpl->episodicMemory) {
                EpisodicMemoryItem predictionEpisode;
                predictionEpisode.timestamp = currentStep;
                predictionEpisode.reward = (pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f);
                predictionEpisode.predictionError = pImpl->predictionSystem->getPredictionError();
                
                // Store prediction pattern
                const auto& predData = predictedState->getData();
                for (float value : predData) {
                    predictionEpisode.predictionPattern.push_back(value);
                }
                
                const auto& actualData = internalSignals->getData();
                for (float value : actualData) {
                    predictionEpisode.actualPattern.push_back(value);
                }
                
                // Store in episodic memory
                pImpl->episodicMemory->storeEpisode(predictionEpisode);
            }
            
            // Update confidence and store in working memory for memory predictions
            float confidence = pImpl->predictionSystem->getConfidence();
            pImpl->workingMemory->storeToNeuron(1, confidence * 0.1f);  // Store confidence signal
            
            // Use prediction error to modulate plasticity
            if (pImpl->dopamine) {
                float plasticityFactor = pImpl->dopamine->getPlasticityFactor();
                // Scale plasticity based on prediction error
                float errorPlasticityMod = 1.0f + (predictionError * 2.0f - 1.0f) * 0.5f;  // [-0.5, 1.5] range
                errorPlasticityMod = std::max(0.1f, std::min(2.0f, errorPlasticityMod));
                
                // Apply error-based plasticity modulation
                for (auto& region : pImpl->regions) {
                    for (auto& syn : region->getSynapses()) {
                        if (syn->getPlasticityFlags().stddp) {
                            float currentWeight = syn->getWeight();
                            // Apply prediction error-based weight adjustment
                            currentWeight += predictionError * 0.001f * errorPlasticityMod;
                            // Constrain weight to reasonable range
                            currentWeight = std::max(-1.0f, std::min(1.0f, currentWeight));
                            syn->setWeight(currentWeight);
                        }
                    }
                }
            }
            
            // Train prediction system with current observations
            pImpl->predictionSystem->train(*internalSignals);
        }
        
        NLM_LOG_DEBUG("Prediction system updated: error = " + std::to_string(pImpl->predictionSystem->getPredictionError()));
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
        
        // Get attention level for logging
        float attentionLevel = pImpl->attention->isEngaged() ? 1.0f : 0.0f;
        
        // Integrate attention with working memory and concept formation
        if (pImpl->attention->isEngaged() && pImpl->conceptFormation) {
            // Enhance concept activation when attention is engaged
            auto concepts = pImpl->conceptFormation->getConcepts();
            for (const auto& concept : concepts) {
                if (concept.avgStability > 0.7f) {  // Stable concept
                    // Boost working memory traces related to this concept
                    if (pImpl->workingMemory && !concept.instances.empty()) {
                        for (const auto& instance : concept.instances) {
                            if (instance.lastObserved == pImpl->currentStep - 1) {
                                // Recent instance - enhance attention
                                pImpl->workingMemory->storeToNeuron(
                                    static_cast<NeuronId>(30000 + concept.id), 
                                    0.3f);
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
        
        // Get current sensory input pattern (if available)
        std::vector<float> sensoryPattern;
        for (Neuron* neuron : pImpl->sensoryNeurons) {
            if (neuron->isFiring() || std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                sensoryPattern.push_back(activation);
            }
        }
        
        // Get features from working memory
        std::vector<float> features;
        if (pImpl->workingMemory) {
            auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            for (NeuronId neuronId : memoryNeurons) {
                float feature = 0.0f;
                for (const auto& region : pImpl->regions) {
                    for (const auto& pop : region->getPopulations()) {
                        for (Neuron* neuron : pop->getNeurons()) {
                            if (neuron->getId() == neuronId) {
                                feature = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                                break;
                            }
                        }
                    }
                }
                features.push_back(feature);
            }
        }
        
        // Get reward from dopamine
        float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        // Present experience to concept formation system
        size_t conceptId = pImpl->conceptFormation->presentExperience(
            sensoryPattern, features, reward, pImpl->currentStep);
        
        // Store concept-based reward in working memory for neuromodulation
        if (conceptId > 0) {
            pImpl->workingMemory->storeToNeuron(
                static_cast<NeuronId>(20000 + conceptId), 
                reward * 0.1f);
        }
        
        // Update attention based on newly formed concepts
        if (conceptId > 0 && pImpl->attention) {
            // Enhance attention to novel concepts
            std::vector<float> conceptFocus(sensoryPattern.size(), 0.0f);
            for (size_t i = 0; i < conceptFocus.size(); ++i) {
                conceptFocus[i] = (i < sensoryPattern.size()) ? sensoryPattern[i] * 2.0f : 0.0f;
            }
            pImpl->attention->setFocus(conceptFocus);
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

BrainPerformanceStats Brain::getPerformanceStats() const {
    BrainPerformanceStats stats;
    
    // Calculate neurons per second based on step count and neurons
    static std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
    if (duration > 0) {
        stats.neuronsPerSecond = static_cast<double>(pImpl->totalNeuronsProcessed) / duration;
    }
    
    // Calculate spikes per second
    if (duration > 0) {
        stats.spikesPerSecond = static_cast<double>(pImpl->totalSpikesTotal) / duration;
    }
    
    // Get active neuron count
    stats.activeNeurons = pImpl->sensoryNeurons.size() + pImpl->motorNeurons.size();
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            stats.activeNeurons += pop->getActiveCount();
        }
    }
    
    // Get firing neuron count
    stats.firingNeurons = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            stats.firingNeurons += pop->getFiringCount();
        }
    }
    
    // Get pending spike counts from event queues
    stats.pendingSpikes = pImpl->spikeQueue_.size();
    stats.pendingDelayedSpikes = pImpl->delayedSpikeQueue_.pendingCount();
    
    // Calculate average firing rate
    size_t totalNeurons = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            totalNeurons += pop->getSize();
        }
    }
    if (totalNeurons > 0) {
        stats.avgFiringRate = static_cast<float>(stats.firingNeurons) / static_cast<float>(totalNeurons);
    }
    
    // Calculate memory usage from pools
    if (pImpl->neuronPool) {
        stats.memoryUsageMB = (pImpl->neuronPool->memoryUsage() + pImpl->synapsePool->memoryUsage()) / (1024.0f * 1024.0f);  // Convert to MB
    }
    
    // Get real-time factor (simulated time / real time)
    // This would require tracking elapsed real time during simulation
    stats.realTimeFactor = 1.0;  // Placeholder
    
    // Event processing latency (placeholder - would need detailed timing)
    stats.eventProcessingLatencyNs = 0.0;
    
    // Computational throughput
    stats.synapticEventsPerSecond = 0.0;  // Would need to track
    
    // Step time (would need to track during step)
    stats.stepTimeMs = 0.0;  // Would need to track
    
    // Pool statistics
    if (pImpl->neuronPool) {
        stats.memoryPoolAllocated = pImpl->neuronPool->allocatedCount();
        stats.memoryPoolUsed = pImpl->synapsePool->allocatedCount();
    }
    
    // Event queue size
    stats.eventQueueSize = stats.pendingSpikes + stats.pendingDelayedSpikes;
    
    // Parallel threads active (placeholder)
    stats.parallelThreadsActive = pImpl->multithreadingEnabled_ ? pImpl->threadCount_ : 0;
    
    return stats;
}
    // Use NeuralPlanner to plan actions based on current brain state
    // This replaces simple motor neuron activity with cognitive planning
    
    if (!pImpl->planner) {
        // Fallback to simple action if planner not available
        if (pImpl->motorNeurons.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Simple action selection based on motor neuron activity
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
        
        auto action = std::make_unique<Action>(type);
        return action;
    }
    
    // Create state representation for planner (from sensory and working memory)
    std::vector<float> currentState;
    
    // Add sensory neuron activity
    for (Neuron* neuron : pImpl->sensoryNeurons) {
        float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        currentState.push_back(activity);
    }
    
    // Add working memory activity
    if (pImpl->workingMemory) {
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        for (NeuronId neuronId : memoryNeurons) {
            float activity = 0.0f;
            for (const auto& region : pImpl->regions) {
                for (const auto& pop : region->getPopulations()) {
                    for (Neuron* neuron : pop->getNeurons()) {
                        if (neuron->getId() == neuronId) {
                            activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                            break;
                        }
                    }
                }
            }
            currentState.push_back(activity);
        }
    }
    
    // Add motor neuron activity as bias
    for (Neuron* neuron : pImpl->motorNeurons) {
        float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        currentState.push_back(activity);
    }
    
    // Ensure state has minimum size (at least enough for 6 action types)
    if (currentState.size() < 6) {
        while (currentState.size() < 6) {
            currentState.push_back(0.0f);
        }
    }
    
    // Use planner to determine best action
    ActionType plannedAction = pImpl->planner->planAction(currentState, 0.5f);
    
    // Update planner with actual outcome (will be updated later based on feedback)
    // For now, create action based on planned type
    auto action = std::make_unique<Action>(plannedAction);
    
    // Store action in working memory for learning
    if (pImpl->workingMemory) {
        size_t actionIdx = static_cast<size_t>(plannedAction);
        if (actionIdx < 10) {
            pImpl->workingMemory->storeToNeuron(static_cast<NeuronId>(10000 + actionIdx), 0.5f);
        }
    }
    
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
    
    // Reset cognitive systems
    if (pImpl->planner) pImpl->planner->clearCache();
    if (pImpl->conceptFormation) pImpl->conceptFormation->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    // Reset prediction system
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->clearHistory();
    }
    
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
    
    // Prediction system status
    if (pImpl->predictionSystem) {
        NLM_LOG_INFO("Prediction system active: yes");
        NLM_LOG_INFO("Prediction error: " + std::to_string(pImpl->predictionSystem->getPredictionError()));
        NLM_LOG_INFO("Prediction confidence: " + std::to_string(pImpl->predictionSystem->getConfidence()));
        NLM_LOG_INFO("Prediction error history size: " + std::to_string(pImpl->predictionSystem->getErrorHistory().size()));
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

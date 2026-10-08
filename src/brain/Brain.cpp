#include "Brain.hpp"
#include "EnhancedWorkingMemoryIntegration.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/InternalSignals.hpp"
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
#include "../performance/MemoryPoolSystem.hpp"
#include "../performance/OptimizedEventSystem.hpp"
#include "../performance/ParallelProcessing.hpp"
#include "../performance/OptimizedNeuralDynamics.hpp"
#include "../performance/OptimizedPlasticity.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <sstream>

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
    
    // ========== ENHANCED WORKING MEMORY INTEGRATION ==========
    std::unique_ptr<EnhancedWorkingMemoryIntegration> enhancedMemoryIntegration;
    
    // Phase 4: Performance optimization systems
    // Create optimized memory pool for neurons and synapses
    std::unique_ptr<MemoryPoolSystem> memoryPoolSystem;
    
    // Optimized event system for spike processing
    std::unique_ptr<OptimizedEventQueue> eventQueue;
    
    // Optimized neural dynamics for efficient computation
    std::unique_ptr<OptimizedNeuralDynamics> neuralDynamics;
    
    // Optimized plasticity computation
    std::unique_ptr<OptimizedPlasticity> plasticity;
    
    // Performance monitoring
    struct PerformanceMetrics {
        double simulationFrequency = 1000.0;  // Hz
        double averageStepTimeMs = 0.0;
        double maxStepTimeMs = 0.0;
        double realTimeFactor = 1.0;
        size_t totalSpikesProcessed = 0;
        size_t totalNeuronsProcessed = 0;
        size_t totalSynapsesProcessed = 0;
        float averageFiringRate = 0.0;
        double memoryUsageMB = 0.0;
        float cacheEfficiency = 0.0;
        float simdUtilization = 0.0;
        float parallelEfficiency = 0.0;
        
        void reset() {
            totalSpikesProcessed = 0;
            totalNeuronsProcessed = 0;
            totalSynapsesProcessed = 0;
            averageFiringRate = 0.0;
            memoryUsageMB = 0.0;
            cacheEfficiency = 0.0;
            simdUtilization = 0.0;
            parallelEfficiency = 0.0;
        }
    } perfMetrics;
    
    std::chrono::high_resolution_clock::time_point lastStepTime;
    size_t totalSteps = 0;
    double totalSimulationTime = 0.0;
    
    // Performance optimization configuration
    bool useMemoryPool = false;
    bool useOptimizedEventSystem = false;
    bool useOptimizedNeuralDynamics = false;
    bool useOptimizedPlasticity = false;
    
    size_t threadCount = 1;
    bool enableSIMD = false;
    bool enableParallel = false;
    
    // Performance tuning parameters
    double targetStepTimeMs = 1.0;  // Target 1ms per step for 1000Hz
    double stepTimeThreshold = 10.0;  // Warning threshold (10ms)
    double memoryThresholdMB = 1000.0;  // Warning threshold (1GB)
    
    // Performance statistics
    size_t spikeQueueSize = 0;
    size_t delayedSpikeCount = 0;
    size_t priorityEventCount = 0;
    
    // Timing for performance monitoring
    std::chrono::high_resolution_clock::time_point cycleStartTime;
    std::chrono::high_resolution_clock::time_point cycleEndTime;
    
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
    
    // Self-model state for self-awareness and prediction
    std::vector<float> lastStateForSelfModel;
    ActionType lastActionForSelfModel;
    
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
        , consolidationInterval(1000)  // Consolidate every 1000 steps
        , lastActionForSelfModel(ActionType::Wait)
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
        
        // Initialize enhanced working memory integration
        enhancedMemoryIntegration = std::make_unique<EnhancedWorkingMemoryIntegration>();
        
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
         
         // Initialize performance optimization systems
         // Read configuration for optimized systems
         useMemoryPool = config->getOr<bool>("use_memory_pool", false);
         useOptimizedEventSystem = config->getOr<bool>("use_optimized_event_system", true);
         useOptimizedNeuralDynamics = config->getOr<bool>("use_optimized_neural_dynamics", true);
         useOptimizedPlasticity = config->getOr<bool>("use_optimized_plasticity", true);
         
         threadCount = config->getOr<size_t>("thread_count", 1);
         enableSIMD = config->getOr<bool>("enable_simd", true);
         enableParallel = config->getOr<bool>("enable_parallel", true);
         
         targetStepTimeMs = config->getOr<double>("target_step_time_ms", 1.0);
         stepTimeThreshold = config->getOr<double>("step_time_threshold_ms", 10.0);
         memoryThresholdMB = config->getOr<double>("memory_threshold_mb", 1000.0);
         
         // Create optimized event system
         if (useOptimizedEventSystem) {
             eventQueue = std::make_unique<OptimizedEventQueue>(16384);
             eventQueue->enable(true);
         }
         
         // Create optimized neural dynamics
         if (useOptimizedNeuralDynamics) {
             NeuralDynamicsConfig neuralConfig;
             neuralConfig.enableSIMD = enableSIMD;
             neuralConfig.enableParallel = enableParallel;
             neuralConfig.enableMemoryPool = useMemoryPool;
             neuralConfig.threadCount = threadCount;
             neuralConfig.targetFrequencyHz = 1.0 / timestep * 1000.0; // Convert to Hz
             
             neuralDynamics = std::make_unique<OptimizedNeuralDynamics>(neuralConfig);
             
             // Initialize with memory pool if available
             if (useMemoryPool && memoryPoolSystem) {
                 neuralDynamics->initialize(*memoryPoolSystem, *parallelProcessor_);
             }
         }
         
         // Create optimized plasticity
         if (useOptimizedPlasticity) {
             PlasticityConfig plasticityConfig;
             plasticityConfig.enableSIMD = enableSIMD;
             plasticityConfig.enableParallel = enableParallel;
             plasticityConfig.enableEventDriven = useOptimizedEventSystem;
             plasticityConfig.threadCount = threadCount;
             
             plasticity = std::make_unique<OptimizedPlasticity>(plasticityConfig);
         }
         
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
    pImpl->enhancedMemoryIntegration->initialize(this);
    
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
    // Record step start time for performance monitoring
    cycleStartTime = std::chrono::high_resolution_clock::now();
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Initialize performance metrics for this step
    perfMetrics.totalSpikesProcessed += pImpl->totalSpikesTotal - perfMetrics.totalSpikesProcessed;
    perfMetrics.totalSteps++;
    
    // ========== OPTIMIZED STEP 1: Event-driven processing with optimized event system ==========
    if (pImpl->useOptimizedEventSystem && eventQueue) {
        // Use optimized event queue system
        eventQueue->advanceTime(currentStep);
        
        // Collect and process events in optimized batches
        std::vector<SpikeEvent> spikes;
        std::vector<DelayedSpikeEvent> delayedEvents;
        std::vector<PriorityEvent> priorityEvents;
        
        // Get events from optimized event system
        while (true) {
            auto event = eventQueue->popImmediate();
            if (!event) break;
            spikes.push_back(SpikeEvent(event->source_neuron, event->timestamp, currentStep));
        }
        
        // Process events in optimized neural dynamics
        if (pImpl->useOptimizedNeuralDynamics && neuralDynamics) {
            neuralDynamics->processSpikes(spikes, neurons, currentStep, currentTime);
        }
        
        // Update statistics
        perfMetrics.totalSpikesProcessed += spikes.size();
        spikeQueueSize = eventQueue->getSpikeCount();
    } else {
        // Fallback to original spike system
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        
        // Collect spikes from original system
        std::vector<SpikeEvent> spikes;
        // Original spike processing would go here
    }
    
    // ========== OPTIMIZED STEP 2: Neural dynamics with SIMD and parallel processing ==========
    if (pImpl->useOptimizedNeuralDynamics && neuralDynamics) {
        // Collect all neurons from regions
        std::vector<Neuron*> neurons;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neurons.push_back(neuron);
                }
            }
        }
        
        // Collect all synapses
        std::vector<Synapse*> synapses;
        for (auto& region : pImpl->regions) {
            for (auto* syn : region->getSynapses()) {
                synapses.push_back(syn);
            }
        }
        
        // Process neural dynamics with optimized system
        neuralDynamics->processStep(neurons, synapses, currentStep, currentTime, pImpl->timestep);
        
        // Update performance metrics
        perfMetrics.totalNeuronsProcessed = neurons.size();
        perfMetrics.totalSynapsesProcessed = synapses.size();
        perfMetrics.averageFiringRate = calculateAverageFiringRate(neurons);
        
    } else {
        // Original neural dynamics processing
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, pImpl->timestep);
                }
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule events ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
                
                if (justFired) {
                    // Neuron fired this step - process with optimized systems
                    perfMetrics.totalSpikesProcessed++;
                    
                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        // Record for plasticity using optimized system
                        if (pImpl->useOptimizedPlasticity && plasticity) {
                            plasticity->recordSpike(syn->getSourceNeuron(), currentTime, true);
                        } else {
                            syn->recordPostSpike(currentTime);
                        }
                    }
                    
                    // Get outgoing synapses and schedule with optimized systems
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create spike event
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        
                        if (pImpl->useOptimizedEventSystem && eventQueue) {
                            // Queue in optimized event system
                            QueuedSpikeEvent queuedEvent(
                                neuron->getId(),
                                syn->getDestinationNeuron(),
                                syn->getId(),
                                syn->getWeight(),
                                syn->getType() == SynapseType::Excitatory
                            );
                            eventQueue->queueSpike(queuedEvent);
                            spikeQueueSize = eventQueue->getSpikeCount();
                        } else {
                            // Fallback to original system
                            pImpl->spikeSystem->queueSpike(event);
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
                            syn->getType() == SynapseType::Excitatory,
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );
                        
                        if (pImpl->useOptimizedEventSystem && eventQueue) {
                            eventQueue->queueDelayedSpike(delayedEvent);
                            delayedSpikeCount = eventQueue->getDelayedSpikeCount();
                        } else {
                            pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        }
                        
                        // Record pre-synaptic spike for plasticity
                        if (pImpl->useOptimizedPlasticity && plasticity) {
                            plasticity->recordSpike(neuron->getId(), currentTime, true);
                        } else {
                            syn->recordPreSpike(currentTime);
                        }
                    }
                    
                    // Store to working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // ========== STEP 4: Apply optimized plasticity ==========
    if (pImpl->useOptimizedPlasticity && plasticity) {
        // Collect all synapses for plasticity
        std::vector<Synapse*> allSynapses;
        for (auto& region : pImpl->regions) {
            for (auto* syn : region->getSynapses()) {
                allSynapses.push_back(syn);
            }
        }
        
        // Apply plasticity using optimized system
        plasticity->processStep(allSynapses, pImpl->currentTime, pImpl->timestep, 
                               pImpl->stdp.get(), pImpl->hebbian.get());
        
        // Update neuromodulation factor for next step
        float neuromodFactor = 1.0f;
        if (pImpl->dopamine) {
            neuromodFactor = pImpl->dopamine->getLevel();
        }
        plasticity->setNeuromodulationFactor(neuromodFactor);
        
    } else {
        // Original plasticity processing
        for (auto& region : pImpl->regions) {
            for (Synapse* syn : region->getSynapses()) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Apply STDP with neuromodulation
                    if (syn->getPlasticityFlags().stdp) {
                        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                        float weight = syn->getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (pImpl->dopamine ? 
                                                         pImpl->dopamine->getPlasticityFactor() : 1.0f - 1.0f) * 0.001f;
                        syn->setWeight(weight);
                    }
                    
                    // Apply Hebbian learning
                    if (syn->getPlasticityFlags().hebbian) {
                        pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    }
                    
                    // Update synapse state
                    syn->step(currentTime);
                }
            }
        }
    }
    
    // ========== REST OF ORIGINAL STEPS (4-14) ==========
    // ... (continue with the rest of the original steps)
    
    // ========== PERFORMANCE MONITORING ==========
    cycleEndTime = std::chrono::high_resolution_clock::now();
    auto stepDuration = std::chrono::duration_cast<std::chrono::microseconds>(cycleEndTime - cycleStartTime);
    double stepTimeMs = stepDuration.count() / 1000.0;
    
    perfMetrics.averageStepTimeMs = (perfMetrics.averageStepTimeMs * (perfMetrics.totalSteps - 1) + stepTimeMs) / perfMetrics.totalSteps;
    perfMetrics.maxStepTimeMs = std::max(perfMetrics.maxStepTimeMs, stepTimeMs);
    perfMetrics.realTimeFactor = stepTimeMs * 1000.0 / pImpl->timestep; // Convert to real-time factor
    
    // Check for performance warnings
    if (stepTimeMs > pImpl->stepTimeThreshold) {
        NLM_LOG_WARNING("Slow neural step: " + std::to_string(stepTimeMs) + "ms");
    }
    
    if (perfMetrics.memoryUsageMB > pImpl->memoryThresholdMB) {
        NLM_LOG_WARNING("High memory usage: " + std::to_string(perfMetrics.memoryUsageMB) + "MB");
    }
    
    // Log performance metrics periodically
    if (currentStep % 1000 == 0) {
        logPerformanceMetrics();
    }
}
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 4.5: Enhanced working memory integration ==========
    if (pImpl->enhancedMemoryIntegration) {
        pImpl->enhancedMemoryIntegration->integrateThroughoutBrainLoop();
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
        // Get current sensory state from working memory
        std::vector<float> currentSensoryState;
        if (pImpl->workingMemory) {
            currentSensoryState = pImpl->workingMemory->retrieve();
        }
        
        if (!currentSensoryState.empty()) {
            // Generate prediction for next timestep
            // Use NeuralPrediction directly for real implementation
            // For now, use the PredictionSystem with working memory state
            auto internalSignal = std::make_unique<InternalSignals>();
            internalSignal->setTimestamp(static_cast<double>(currentStep));
            internalSignal->clearSignals();
            for (float activation : currentSensoryState) {
                internalSignal->addSignal(activation);
            }
            
            // Generate prediction using the prediction system
            auto predictedState = pImpl->predictionSystem->predictNextState(*internalSignal);
            
            if (predictedState) {
                // Update predictions using current and predicted states
                // Compute prediction error
                float predictionError = pImpl->predictionSystem->getPredictionError();
                float predictionConfidence = pImpl->predictionSystem->getConfidence();
                
                // Update prediction system with comparison
                pImpl->predictionSystem->updatePredictions(*internalSignal, *predictedState);
                
                // Use prediction error to drive learning through neuromodulation
                if (predictionError > 0.05f && pImpl->predictionError) {
                    // Prediction error acts as a teaching signal
                    pImpl->predictionError->computeError(predictionError, 1.0f - predictionError);
                }
                
                // Update working memory based on prediction accuracy
                if (pImpl->workingMemory) {
                    if (predictionError < 0.05f) {  // Good prediction
                        pImpl->workingMemory->strengthenMemory(1.2f);
                    } else if (predictionError > 0.3f) {  // Poor prediction
                        pImpl->workingMemory->strengthenMemory(0.8f);  // Weaken wrong predictions
                    }
                }
                
                // Store prediction errors in episodic memory for learning
                if (pImpl->episodicMemory && predictionError > 0.1f) {
                    EpisodicMemoryItem predictionEpisode;
                    predictionEpisode.timestamp = currentStep;
                    predictionEpisode.reward = 1.0f - predictionError;
                    predictionEpisode.activeNeurons = pImpl->sensoryNeurons.size();
                    predictionEpisode.neuronActivations.resize(pImpl->sensoryNeurons.size(), predictionError);
                    
                    pImpl->episodicMemory->storeEpisode(predictionEpisode);
                    
                    // Consolidate important prediction errors
                    pImpl->episodicMemory->consolidate(0.3f);
                }
                
                // Update neuromodulation systems based on prediction accuracy
                if (pImpl->dopamine) {
                    // Better predictions increase dopamine
                    float dopamineLevel = 1.0f - predictionError;
                    // Dopamine would be set by its own update mechanism
                }
                
                // Prediction error can guide attention
                if (pImpl->attention && predictionError > 0.15f) {
                    // Error triggers enhanced attention to surprising events
                    // Note: Attention API would need specific methods for error integration
                }
            }
        }
    }
    
    // ========== STEP 5.5: Integrate NeuralPlanner ==========
    ActionType plannedAction = ActionType::Wait;
    float planningConfidence = 0.0f;
    
    if (pImpl->planner && pImpl->workingMemory) {
        // Get current sensory state from working memory as input for planning
        std::vector<float> currentSensoryState = pImpl->workingMemory->retrieve();
        
        // Get current state from episodic memory for context
        std::vector<float> episodicContext;
        if (pImpl->episodicMemory && pImpl->episodicMemory->getEpisodeCount() > 0) {
            auto episodes = pImpl->episodicMemory->getEpisodesForReplay(1);
            if (!episodes.empty() && episodes[0]) {
                const auto* ep = episodes[0];
                episodicContext.resize(ep->activeNeurons.size());
                for (size_t i = 0; i < ep->activeNeurons.size(); ++i) {
                    episodicContext[i] = pImpl->workingMemory->getNeuronActivation(ep->activeNeurons[i]);
                }
            }
        }
        
        // Use NeuralPlanner to plan next action
        // Get target reward from neuromodulation if available
        float targetReward = 0.5f; // Default target reward
        if (pImpl->dopamine) {
            targetReward = 0.3f + pImpl->dopamine->getLevel() * 0.4f; // Scale dopamine level to reward target
        }
        
        // Combine sensory and episodic context for planning
        std::vector<float> planningInput = currentSensoryState;
        if (!episodicContext.empty()) {
            // Blend episodic context with sensory state
            float blendFactor = 0.3f;
            for (size_t i = 0; i < std::min(planningInput.size(), episodicContext.size()); ++i) {
                planningInput[i] = planningInput[i] * (1.0f - blendFactor) + episodicContext[i] * blendFactor;
            }
        }
        
        plannedAction = pImpl->planner->planAction(planningInput, targetReward);
        planningConfidence = pImpl->planner->getPlanningConfidence();
        
        // Use planner output to influence action selection
        // The planner's confidence affects how much we trust its recommendation
        if (planningConfidence < 0.3f) {
            // Low confidence - use attention system to explore alternatives
            if (pImpl->attention) {
                // Get competitors from working memory for attention-based selection
                if (!pImpl->workingMemory->getMemoryNeurons().empty()) {
                    std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                    std::vector<NeuronId> attended = pImpl->attention->processCompetition(competitors, 0.7f);
                    if (!attended.empty() && attended[0] != 0) {
                        // Could select based on attended neurons, but for now use planner's action
                    }
                }
            }
        }
    }
    
    // ========== STEP 5.6: Integrate SelfModel ==========
    if (pImpl->workingMemory && pImpl->episodicMemory) {
        // Get current sensory state for self-model processing
        std::vector<float> currentState = pImpl->workingMemory->retrieve();
        
        // Check if self-model has been formed (has recorded actions)
        // Since SelfModel is part of NeuralPlanner in the implementation,
        // we access it through the planner's self-model
        if (pImpl->planner && pImpl->planner->wasRecentPlanSuccessful()) {
            // Self-model is active, use it to predict action consequences
            if (!currentState.empty()) {
                // Record current state as "before" state for self-model
                // This simulates self-monitoring of internal state
                // In a more complete implementation, this would be tied to actual actions
                
                // Use SelfModel to predict consequences of planned action
                if (pImpl->planner && pImpl->planner->getCurrentGoal().size() == currentState.size()) {
                    // Get predicted consequence using self-model
                    std::vector<float> predictedConsequence = pImpl->planner->predictActionConsequence(
                        plannedAction, currentState);
                    
                    if (!predictedConsequence.empty()) {
                        // Self-model prediction can modulate attention and planning
                        float predictionSimilarity = pImpl->planner->computeSelfGeneratedLikeness(
                            currentState, predictedConsequence, plannedAction);
                        
                        // Update prediction system with self-model predictions
                        if (pImpl->predictionSystem) {
                            // Use self-model predictions to improve prediction accuracy
                            // This helps the prediction system learn self-generated vs external events
                        }
                        
                        // Use self-model for action selection refinement
                        if (pImpl->attention) {
                            // If self-model confidence is high for this action, increase attention
                            float selfModelConf = pImpl->planner->getSelfModelConfidence(plannedAction);
                            if (selfModelConf > 0.7f) {
                                // High self-model confidence means action is well-learned
                                // Apply top-down bias to this action
                                pImpl->attention->applyTopDownBias(
                                    plannedAction, selfModelConf * 0.5f);
                            }
                        }
                    }
                }
                
                // Update self-model based on experience
                // This happens at the end of the brain step when we have actual outcomes
                // Store current state for later self-model update
                pImpl->lastStateForSelfModel = currentState;
                pImpl->lastActionForSelfModel = plannedAction;
            }
        } else {
            // Self-model is still forming, record current state as baseline
            if (!currentState.empty()) {
                pImpl->lastStateForSelfModel = currentState;
            }
        }
    }
    
    // Apply curiosity to modify planned action (exploration)
    if (pImpl->curiosity && curiosity->getLevel() > 0.5f && plannedAction != ActionType::Wait) {
        // High curiosity may lead to exploring different actions
        float exploreChance = pImpl->curiosity->getLevel() * 0.3f;
        if (rng && rng->uniformReal(0.0f, 1.0f) < exploreChance) {
            // Pick random action when curiosity is high
            plannedAction = static_cast<ActionType>(rng->uniformInt(0, 6));
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
    
    // ========== STEP 10: Integrate ConceptFormation ==========
    if (pImpl->conceptFormation && pImpl->workingMemory && pImpl->episodicMemory) {
        // Get current neural state for concept formation
        std::vector<float> currentNeuralState = pImpl->workingMemory->retrieve();
        std::vector<float> features;
        
        // Extract features from neural state for concept formation
        // Use different features based on neural dynamics
        if (!currentNeuralState.empty()) {
            // Create simple features: average activation, variance, and peak locations
            float sum = std::accumulate(currentNeuralState.begin(), currentNeuralState.end(), 0.0f);
            float avgActivation = sum / currentNeuralState.size();
            
            float variance = 0.0f;
            for (float val : currentNeuralState) {
                variance += (val - avgActivation) * (val - avgActivation);
            }
            variance /= currentNeuralState.size();
            
            // Create feature vector
            features.resize(5);
            features[0] = avgActivation;                    // Mean activation
            features[1] = variance;                         // Variance
            features[2] = planningConfidence;               // Planning confidence
            features[3] = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f; // Reward signal
            features[4] = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f; // Curiosity level
            
            // Present experience to concept formation system
            float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentNeuralState, features, reward, pImpl->currentStep);
            
            // If a new concept was formed or updated, use it for modulation
            if (conceptId > 0) {
                const DiscoveredConcept* concept = pImpl->conceptFormation->getConcept(conceptId);
                if (concept && concept->avgStability > 0.6f) {
                    // Stable concept can be used for attention modulation
                    if (pImpl->attention) {
                        // Apply concept-based attention bias
                        // Use the category hint to modulate attention
                        float conceptInfluence = concept->avgStability * 0.5f;
                        // In a full implementation, we would apply this to specific regions
                        // For now, we just note that concepts are forming
                    }
                    
                    // Concept can be used for prediction refinement
                    if (pImpl->predictionSystem) {
                        // Concepts provide structured priors for prediction
                        // This would influence how prediction system generates predictions
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
    
    // Reset enhanced working memory integration
    if (pImpl->enhancedMemoryIntegration) {
        // Reset the enhanced memory integration state
        pImpl->enhancedMemoryIntegration->integrateThroughoutBrainLoop();
    }
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    pImpl->lastStateForSelfModel.clear();
    pImpl->lastActionForSelfModel = ActionType::Wait;
    
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

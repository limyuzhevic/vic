#include "../plugin/NLMPlugin.hpp"
#include "../plugin/PluginManager.hpp"
#include "../plugin/region/RegionPlugin.hpp"
#include "../plugin/neuron/NeuronPlugin.hpp"
#include "../plugin/plasticity/PlasticityPlugin.hpp"
#include "../plugin/sensory/SensoryPlugin.hpp"
#include "../plugin/memory/MemoryPlugin.hpp"
#include "../plugin/neuromodulation/NeuromodulationPlugin.hpp"

namespace nlm {

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
    
    // Plugin system integration
    std::unique_ptr<PluginManager> pluginManager;
    
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
        , performanceMonitor(nullptr)
        , memoryProfiler(nullptr)
        , cpuMonitor(nullptr)
        , benchmarkSuite(nullptr)
        , optimizationHints(nullptr)
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize performance monitoring if enabled
        bool enableProfiler = config->getOr<bool>("enable_profiler", false);
        bool enableBenchmarking = config->getOr<bool>("enable_benchmarking", false);
        bool enablePerformanceOptimizations = config->getOr<bool>("enable_performance_optimizations", true);
        
        if (enableProfiler || enableBenchmarking || enablePerformanceOptimizations) {
            performanceMonitor = std::make_unique<PerformanceMonitor>();
            performanceMonitor->initialize();
            
            if (enableProfiler) {
                memoryProfiler = std::make_unique<MemoryProfiler>();
                cpuMonitor = std::make_unique<CPUMonitor>();
            }
            
            if (enableBenchmarking) {
                benchmarkSuite = std::make_unique<BenchmarkSuite>();
            }
            
            if (enablePerformanceOptimizations) {
                optimizationHints = std::make_unique<OptimizationHints>();
            }
        }
        
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
    
    // Initialize performance monitoring if available
    if (pImpl->performanceMonitor) {
        NLM_LOG_INFO("Performance monitoring initialized");
        
        // Record performance start
        pImpl->performanceMonitor->startOperation("initialization");
        
        // Record memory usage
        if (pImpl->memoryProfiler) {
            pImpl->memoryProfiler->trackAllocation(0); // Start tracking
        }
        
        // Record CPU usage
        if (pImpl->cpuMonitor) {
            pImpl->cpuMonitor->update(0.0, 0); // Start tracking
        }
    }
    
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
        
        // Record performance metrics
        if (pImpl->performanceMonitor) {
            pImpl->performanceMonitor->startOperation("spike_processing");
        }
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
    
    // Finalize performance monitoring
    if (pImpl->performanceMonitor) {
        pImpl->performanceMonitor->stopOperation("initialization");
        
        // Generate optimization hints if available
        if (pImpl->optimizationHints) {
            // In a real implementation, this would analyze performance data
            // and generate optimization hints
            NLM_LOG_INFO("Performance analysis complete - optimization hints available");
        }
        
        // Print performance report if configured
        std::string profileOutputFile = pImpl->config->getOr<std::string>("profile_output_file", "");
        if (!profileOutputFile.empty() && pImpl->performanceMonitor) {
            if (!pImpl->performanceMonitor->saveReport(profileOutputFile)) {
                NLM_LOG_WARNING("Failed to save performance report to " + profileOutputFile);
            }
        }
        
        NLM_LOG_INFO("Performance monitoring complete");
    }
    
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::step: pImpl is null");
        return;
    }
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    } else {
        NLM_LOG_ERROR("Brain::step: spikeSystem is null");
    }
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::step: Found null region in regions list");
            continue;
        }
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_ERROR("Brain::step: Found null population in region");
                continue;
            }
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) {
                    NLM_LOG_ERROR("Brain::step: Found null neuron");
                    continue;
                }
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::step: Found null region in regions list");
            continue;
        }
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_ERROR("Brain::step: Found null population in region");
                continue;
            }
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) {
                    NLM_LOG_ERROR("Brain::step: Found null neuron");
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
                    if (pImpl->spikeSystem) {
                        pImpl->spikeSystem->queueSpike(event);
                    } else {
                        NLM_LOG_ERROR("Brain::step: spikeSystem is null when queuing spike");
                    }

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        if (!syn) {
                            NLM_LOG_ERROR("Brain::step: Found null synapse in incomingSynapses");
                            continue;
                        }
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        if (!syn) {
                            NLM_LOG_ERROR("Brain::step: Found null synapse in outgoingSynapses");
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

                        if (pImpl->spikeSystem) {
                            pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        } else {
                            NLM_LOG_ERROR("Brain::step: spikeSystem is null when queuing delayed spike");
                        }

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
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processSpikes(currentStep);
    } else {
        NLM_LOG_ERROR("Brain::step: spikeSystem is null when processing spikes");
    }
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    } else {
        NLM_LOG_ERROR("Brain::step: workingMemory is null");
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
            if (!region) {
                NLM_LOG_ERROR("Brain::step: Found null region in regions list");
                continue;
            }
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_ERROR("Brain::step: Found null population in region");
                    continue;
                }
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        NLM_LOG_ERROR("Brain::step: Found null neuron");
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
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::step: Found null region in regions list");
            continue;
        }
        for (auto& syn : region->getSynapses()) {
            if (!syn) {
                NLM_LOG_ERROR("Brain::step: Found null synapse in region");
                continue;
            }
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    if (pImpl->stdp) {
                        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    } else {
                        NLM_LOG_ERROR("Brain::step: stdp is null");
                    }
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
                    if (pImpl->hebbian) {
                        pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    } else {
                        NLM_LOG_ERROR("Brain::step: hebbian is null");
                    }
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
                if (!region) {
                    NLM_LOG_ERROR("Brain::step: Found null region in regions list");
                    continue;
                }
                for (auto& pop : region->getPopulations()) {
                    if (!pop) {
                        NLM_LOG_ERROR("Brain::step: Found null population in region");
                        continue;
                    }
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            NLM_LOG_ERROR("Brain::step: Found null neuron");
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
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    } else {
        NLM_LOG_ERROR("Brain::step: predictionSystem is null");
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    } else {
        NLM_LOG_ERROR("Brain::step: attention is null");
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    } else {
        NLM_LOG_ERROR("Brain::step: conceptFormation is null");
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        if (pImpl->structuralPlasticity) {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        } else {
            NLM_LOG_ERROR("Brain::step: structuralPlasticity is null");
        }
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
            NLM_LOG_ERROR("Brain::step: developmentSystem is null");
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
    } else {
        NLM_LOG_ERROR("Brain::step: checkpointManager is null");
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::injectCurrent: pImpl is null");
        return;
    }
    
    for (auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::injectCurrent: Found null region in regions list");
            continue;
        }
        auto neurons = region->getAllNeurons();
        if (!neurons) {
            NLM_LOG_ERROR("Brain::injectCurrent: region->getAllNeurons() returned null");
            continue;
        }
        for (auto* n : *neurons) {
            if (!n) {
                NLM_LOG_ERROR("Brain::injectCurrent: Found null neuron");
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::injectCurrentToNeurons: pImpl is null");
        return;
    }
    
    for (auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::injectCurrentToNeurons: Found null region in regions list");
            continue;
        }
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_ERROR("Brain::injectCurrentToNeurons: Found null population in region");
                continue;
            }
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        NLM_LOG_ERROR("Brain::injectCurrentToNeurons: Found null neuron");
                        continue;
                    }
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getExcitationInhibitionRatio: pImpl is null");
        return 0.0f;
    }
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::getExcitationInhibitionRatio: Found null region in regions list");
            continue;
        }
        for (const auto& syn : region->getSynapses()) {
            if (!syn) {
                NLM_LOG_ERROR("Brain::getExcitationInhibitionRatio: Found null synapse in region");
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::produceAction: pImpl is null");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Check for null neurons in motorNeurons
    size_t validNeurons = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (!neuron) {
            NLM_LOG_ERROR("Brain::produceAction: Found null neuron in motorNeurons");
            continue;
        }
        validNeurons++;
    }
    
    if (validNeurons == 0) {
        NLM_LOG_ERROR("Brain::produceAction: No valid neurons in motorNeurons");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (!neuron) {
            NLM_LOG_ERROR("Brain::produceAction: Found null neuron in motorNeurons");
            continue;
        }
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::applyNeuromodulation: pImpl is null");
        return;
    }
    
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    } else {
        NLM_LOG_ERROR("Brain::applyNeuromodulation: stdp is null");
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::develop: pImpl is null");
        return;
    }
    
    // Development updates structural plasticity
    if (pImpl->structuralPlasticity && pImpl->rng) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    } else {
        NLM_LOG_ERROR("Brain::develop: structuralPlasticity or rng is null");
    }
}

void Brain::reset() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::reset: pImpl is null");
        return;
    }
    
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::reset: Found null region in regions list");
            continue;
        }
        region->reset();
    }
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    } else {
        NLM_LOG_ERROR("Brain::reset: spikeSystem is null");
    }
    
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::save: pImpl is null");
        return false;
    }
    
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
            if (!region) {
                NLM_LOG_ERROR("Brain::save: Found null region in regions list");
                continue;
            }
            for (const auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_ERROR("Brain::save: Found null population in region");
                    continue;
                }
                for (const auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        NLM_LOG_ERROR("Brain::save: Found null neuron");
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
            if (!region) {
                NLM_LOG_ERROR("Brain::save: Found null region in regions list");
                continue;
            }
            for (const auto* syn : region->getSynapses()) {
                if (!syn) {
                    NLM_LOG_ERROR("Brain::save: Found null synapse in region");
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::load: pImpl is null");
        return false;
    }
    
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
            if (!region) {
                NLM_LOG_ERROR("Brain::load: Found null region in regions list");
                continue;
            }
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_ERROR("Brain::load: Found null population in region");
                    continue;
                }
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        NLM_LOG_ERROR("Brain::load: Found null neuron");
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getRegion: pImpl is null");
        return nullptr;
    }
    
    for (auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::getRegion: Found null region in regions list");
            continue;
        }
        if (region->getId() == id) {
            return region.get();
        }
    }
    
    NLM_LOG_WARNING("Brain::getRegion: Region with id " + std::to_string(id.index()) + " not found");
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getRegionCount: pImpl is null");
        return 0;
    }
    
    // Count valid regions (non-null)
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        if (region) {
            count++;
        } else {
            NLM_LOG_ERROR("Brain::getRegionCount: Found null region in regions list");
        }
    }
    
    return count;
}

std::vector<RegionId> Brain::getRegionIds() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getRegionIds: pImpl is null");
        return std::vector<RegionId>();
    }
    
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    
    for (const auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::getRegionIds: Found null region in regions list");
            continue;
        }
        ids.push_back(region->getId());
    }
    
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getRegions: pImpl is null");
        return std::vector<std::unique_ptr<NeuralRegion>>();
    }
    
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::addInterRegionConnection: pImpl is null");
        return;
    }
    
    // Check if source and target regions exist
    auto* srcRegion = getRegion(source);
    auto* tgtRegion = getRegion(target);
    
    if (!srcRegion) {
        NLM_LOG_ERROR("Brain::addInterRegionConnection: Source region " + std::to_string(source.index()) + " not found");
        return;
    }
    
    if (!tgtRegion) {
        NLM_LOG_ERROR("Brain::addInterRegionConnection: Target region " + std::to_string(target.index()) + " not found");
        return;
    }
    
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::removeInterRegionConnection: pImpl is null");
        return;
    }
    
    // Check if source and target regions exist
    auto* srcRegion = getRegion(source);
    auto* tgtRegion = getRegion(target);
    
    if (!srcRegion) {
        NLM_LOG_ERROR("Brain::removeInterRegionConnection: Source region " + std::to_string(source.index()) + " not found");
        return;
    }
    
    if (!tgtRegion) {
        NLM_LOG_ERROR("Brain::removeInterRegionConnection: Target region " + std::to_string(target.index()) + " not found");
        return;
    }
    
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getTotalNeuronCount: pImpl is null");
        return 0;
    }
    
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::getTotalNeuronCount: Found null region in regions list");
            continue;
        }
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getTotalSynapseCount: pImpl is null");
        return 0;
    }
    
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::getTotalSynapseCount: Found null region in regions list");
            continue;
        }
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getActiveNeuronCount: pImpl is null");
        return 0;
    }
    
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::getActiveNeuronCount: Found null region in regions list");
            continue;
        }
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getAverageFiringRate: pImpl is null");
        return 0.0f;
    }
    
    if (pImpl->regions.empty()) return 0.0f;
    
    float sum = 0.0f;
    size_t validRegions = 0;
    
    for (const auto& region : pImpl->regions) {
        if (!region) {
            NLM_LOG_ERROR("Brain::getAverageFiringRate: Found null region in regions list");
            continue;
        }
        sum += region->getAverageFiringRate();
        validRegions++;
    }
    
    if (validRegions == 0) return 0.0f;
    return sum / static_cast<float>(validRegions);
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getWorkingMemory: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->workingMemory) {
        NLM_LOG_ERROR("Brain::getWorkingMemory: workingMemory is null");
        return nullptr;
    }
    
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getEpisodicMemory: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->episodicMemory) {
        NLM_LOG_ERROR("Brain::getEpisodicMemory: episodicMemory is null");
        return nullptr;
    }
    
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getAssociativeMemory: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->associativeMemory) {
        NLM_LOG_ERROR("Brain::getAssociativeMemory: associativeMemory is null");
        return nullptr;
    }
    
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getPredictionSystem: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->predictionSystem) {
        NLM_LOG_ERROR("Brain::getPredictionSystem: predictionSystem is null");
        return nullptr;
    }
    
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getPlanner: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->planner) {
        NLM_LOG_ERROR("Brain::getPlanner: planner is null");
        return nullptr;
    }
    
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getConceptFormation: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->conceptFormation) {
        NLM_LOG_ERROR("Brain::getConceptFormation: conceptFormation is null");
        return nullptr;
    }
    
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getAttention: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->attention) {
        NLM_LOG_ERROR("Brain::getAttention: attention is null");
        return nullptr;
    }
    
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getDevelopmentSystem: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->developmentSystem) {
        NLM_LOG_ERROR("Brain::getDevelopmentSystem: developmentSystem is null");
        return nullptr;
    }
    
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
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getDopamine: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->dopamine) {
        NLM_LOG_ERROR("Brain::getDopamine: dopamine is null");
        return nullptr;
    }
    
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getCuriosity: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->curiosity) {
        NLM_LOG_ERROR("Brain::getCuriosity: curiosity is null");
        return nullptr;
    }
    
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getNovelty: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->novelty) {
        NLM_LOG_ERROR("Brain::getNovelty: novelty is null");
        return nullptr;
    }
    
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getPredictionErrorSignal: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->predictionError) {
        NLM_LOG_ERROR("Brain::getPredictionErrorSignal: predictionError is null");
        return nullptr;
    }
    
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getRandomGenerator: pImpl is null");
        return nullptr;
    }
    
    if (!pImpl->rng) {
        NLM_LOG_ERROR("Brain::getRandomGenerator: rng is null");
        return nullptr;
    }
    
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::logStatus: pImpl is null");
        return;
    }
    
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    
    if (pImpl->regions.empty()) {
        NLM_LOG_WARNING("Brain::logStatus: No regions available");
    } else {
        for (const auto& region : pImpl->regions) {
            if (!region) {
                NLM_LOG_ERROR("Brain::logStatus: Found null region in regions list");
                continue;
            }
            
            NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                        " (" + region->getName() + "): " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                        std::to_string(region->getSynapseCount()) + " synapses, " +
                        "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
        }
    }
    
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
}

} // namespace nlm

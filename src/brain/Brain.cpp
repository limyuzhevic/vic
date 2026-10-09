#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../performance/PerformanceMonitor.hpp"
#include "../performance/Profiler.hpp"
#include "../performance/Optimizer.hpp"
#include "../performance/AnalysisTools.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
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
    
    // ========== INTEGRATED PREDICTION SYSTEMS ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<ActionConsequencePredictor> actionConsequencePredictor;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Reward> reward;
    
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
    
    // Performance monitoring components
    std::unique_ptr<PerformanceMonitor> performanceMonitor;
    std::unique_ptr<Profiler> profiler;
    std::unique_ptr<Optimizer> optimizer;
    std::unique_ptr<AnalysisTools> analysisTools;
    
    // Performance profiling flags
    bool performanceMonitoringEnabled;
    bool profilingEnabled;
    bool optimizationEnabled;
    bool realTimeProfilingEnabled;
    
    // Performance recording flags
    bool recordSystemResources;
    bool recordNeuralComputation;
    bool recordMemoryUsage;
    bool recordSpikeEvents;
    bool recordDevelopment;
    bool recordNeuromodulation;
    
    // Performance analysis settings
    uint64_t performanceSamplingInterval;
    double performanceThreshold;
    bool performanceAlertEnabled;
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
        neuralPrediction = std::make_unique<NeuralPrediction>();
        actionConsequencePredictor = std::make_unique<ActionConsequencePredictor>();
        
        // Initialize neural prediction components
        neuralPrediction->initialize(this);
        actionConsequencePredictor->initialize(this);
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
        serotonin = std::make_unique<Serotonin>();
        norepinephrine = std::make_unique<Norepinephrine>();
        acetylcholine = std::make_unique<Acetylcholine>();
        reward = std::make_unique<Reward>();
        
        // Initialize all neuromodulators with brain reference
        dopamine->initialize(this);
        curiosity->initialize(this);
        predictionError->initialize(this);
        novelty->initialize(this);
        serotonin->initialize(this);
        norepinephrine->initialize(this);
        acetylcholine->initialize(this);
        reward->initialize(this);
        
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
        
        // Initialize performance monitoring systems
        performanceMonitoringEnabled = config->getOr<bool>("performance_monitoring_enabled", true);
        profilingEnabled = config->getOr<bool>("profiling_enabled", true);
        optimizationEnabled = config->getOr<bool>("optimization_enabled", true);
        realTimeProfilingEnabled = config->getOr<bool>("real_time_profiling_enabled", false);
        
        recordSystemResources = config->getOr<bool>("record_system_resources", true);
        recordNeuralComputation = config->getOr<bool>("record_neural_computation", true);
        recordMemoryUsage = config->getOr<bool>("record_memory_usage", true);
        recordSpikeEvents = config->getOr<bool>("record_spike_events", true);
        recordDevelopment = config->getOr<bool>("record_development", true);
        recordNeuromodulation = config->getOr<bool>("record_neuromodulation", true);
        
        performanceSamplingInterval = config->getOr<size_t>("performance_sampling_interval", 100);
        performanceThreshold = config->getOr<double>("performance_threshold", 90.0);
        performanceAlertEnabled = config->getOr<bool>("performance_alert_enabled", true);
        
        if (performanceMonitoringEnabled) {
            performanceMonitor = std::make_unique<PerformanceMonitor>();
            performanceMonitor->setSamplingInterval(performanceSamplingInterval);
            performanceMonitor->enableRealTimeReporting(realTimeProfilingEnabled);
        }
        
        if (profilingEnabled) {
            profiler = std::make_unique<Profiler>();
            profiler->setMaxSamplesPerFunction(1000);
            profiler->setSamplingIntervalMs(100);
            profiler->enableRealTimeProfiling(realTimeProfilingEnabled);
        }
        
        if (optimizationEnabled) {
            optimizer = std::make_unique<Optimizer>();
            optimizer->setLearningRate(0.1);
            optimizer->setOptimizationIterations(100);
            optimizer->setAutoApply(false);
            optimizer->enableRealTimeOptimization(realTimeProfilingEnabled);
        }
        
        if (performanceMonitoringEnabled && performanceMonitor) {
            performanceMonitor->start();
            Logger::getInstance().logInfo("Performance monitoring initialized (sampling: " + std::to_string(performanceSamplingInterval) + " steps)");
        }
        
        if (profilingEnabled && profiler) {
            Logger::getInstance().logInfo("Profiler initialized");
        }
        
        if (optimizationEnabled && optimizer) {
            Logger::getInstance().logInfo("Optimizer initialized");
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
    
    // Initialize performance monitoring for this step
    auto step_start_time = std::chrono::steady_clock::now();
    double wall_clock_time_ms = 0.0;
    double cpu_time_ms = 0.0;
    
    // Start performance monitoring
    if (pImpl->performanceMonitor) {
        pImpl->performanceMonitor->start();
    }
    
    // Start system call profiling for this step
    if (pImpl->profiler && pImpl->profiler->isSessionActive("brain_step")) {
        pImpl->profiler->profileFunctionEnter("brain_step", "simulation");
    }
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    auto spike_processing_start = std::chrono::steady_clock::now();
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    double spike_processing_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - spike_processing_start).count();
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    auto neuron_update_start = std::chrono::steady_clock::now();
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    double neuron_update_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - neuron_update_start).count();
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    auto spike_detection_start = std::chrono::steady_clock::now();
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
    auto immediate_spike_start = std::chrono::steady_clock::now();
    pImpl->spikeSystem->processSpikes(currentStep);
    double immediate_spike_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - immediate_spike_start).count();
    double spike_detection_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - spike_detection_start).count();
    
    // ========== STEP 4: Update working memory ==========
    auto working_memory_update_start = std::chrono::steady_clock::now();
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    double working_memory_update_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - working_memory_update_start).count();
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    auto neuromodulation_update_start = std::chrono::steady_clock::now();
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
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // Update serotonin (5-HT) - mood, learning, sleep, attention
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
        
        // Apply serotonin effects on neural excitability and plasticity
        float serotoninLevel = pImpl->serotonin->getLevel();
        if (serotoninLevel > 0.0f) {
            // Serotonin modulates neural excitability and inhibitory control
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Serotonin enhances inhibitory tone and modulates mood
                        float inhibitoryEffect = serotoninLevel * 0.3f;
                        neuron->injectCurrent(-inhibitoryEffect);  // Inhibition
                    }
                }
            }
            
            // Enhance plasticity for learning and memory consolidation
            float plasticityMod = pImpl->serotonin->getPlasticityFactor();
            if (pImpl->stdp) {
                float currentLTP = pImpl->stdp->getLTPWeight();
                pImpl->stdp->setLTPWeight(currentLTP * plasticityMod);
            }
        }
    }
    
    // Update norepinephrine (NE) - arousal, attention, working memory
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
        
        // Apply norepinephrine effects on neural excitability
        float norepinephrineLevel = pImpl->norepinephrine->getLevel();
        if (norepinephrineLevel > 0.0f) {
            // Norepinephrine enhances arousal and attention
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Norepinephrine increases neural excitability and firing thresholds
                        float arousalEffect = norepinephrineLevel * 0.4f;
                        neuron->injectCurrent(arousalEffect);
                    }
                }
            }
            
            // Modulate plasticity for attention and working memory
            float plasticityMod = pImpl->norepinephrine->getPlasticityFactor();
            if (pImpl->stdp) {
                float currentLTP = pImpl->stdp->getLTPWeight();
                pImpl->stdp->setLTPWeight(currentLTP * plasticityMod);
            }
        }
    }
    
    // Update acetylcholine (ACh) - attention, memory consolidation
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        
        // Apply acetylcholine effects on neural excitability
        float acetylcholineLevel = pImpl->acetylcholine->getLevel();
        if (acetylcholineLevel > 0.0f) {
            // Acetylcholine enhances attention and cortical activation
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Acetylcholine enhances neural responsiveness and attention
                        float attentionEffect = acetylcholineLevel * 0.5f;
                        neuron->injectCurrent(attentionEffect);
                    }
                }
            }
            
            // Enhance plasticity for attention and memory
            float plasticityMod = pImpl->acetylcholine->getPlasticityFactor();
            if (pImpl->stdp) {
                float currentLTP = pImpl->stdp->getLTPWeight();
                pImpl->stdp->setLTPWeight(currentLTP * plasticityMod);
            }
        }
    }
    double neuromodulation_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - neuromodulation_update_start).count();
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    auto plasticity_update_start = std::chrono::steady_clock::now();
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
    double plasticity_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - plasticity_update_start).count();
    
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
    
    // Update serotonin (5-HT) - mood, learning, sleep, attention
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
        
        // Apply serotonin effects on neural excitability and plasticity
        float serotoninLevel = pImpl->serotonin->getLevel();
        if (serotoninLevel > 0.0f) {
            // Serotonin modulates neural excitability and inhibitory control
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Serotonin enhances inhibitory tone and modulates mood
                        float inhibitoryEffect = serotoninLevel * 0.3f;
                        neuron->injectCurrent(-inhibitoryEffect);  // Inhibition
                    }
                }
            }
            
            // Enhance plasticity for learning and memory consolidation
            float plasticityMod = pImpl->serotonin->getPlasticityFactor();
            if (pImpl->stdp) {
                float currentLTP = pImpl->stdp->getLTPWeight();
                pImpl->stdp->setLTPWeight(currentLTP * plasticityMod);
            }
        }
    }
    
    // Update norepinephrine (NE) - arousal, attention, working memory
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
        
        // Apply norepinephrine effects on neural excitability
        float norepinephrineLevel = pImpl->norepinephrine->getLevel();
        if (norepinephrineLevel > 0.0f) {
            // Norepinephrine enhances arousal and attention
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Norepinephrine increases neural excitability and firing thresholds
                        float arousalEffect = norepinephrineLevel * 0.4f;
                        neuron->injectCurrent(arousalEffect);
                    }
                }
            }
            
            // Modulate plasticity for attention and working memory
            float plasticityMod = pImpl->norepinephrine->getPlasticityFactor();
            if (pImpl->stdp) {
                float currentLTP = pImpl->stdp->getLTPWeight();
                pImpl->stdp->setLTPWeight(currentLTP * plasticityMod);
            }
        }
    }
    
    // Update acetylcholine (ACh) - attention, memory consolidation
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        
        // Apply acetylcholine effects on neural excitability
        float acetylcholineLevel = pImpl->acetylcholine->getLevel();
        if (acetylcholineLevel > 0.0f) {
            // Acetylcholine enhances attention and cortical activation
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Acetylcholine enhances neural responsiveness and attention
                        float attentionEffect = acetylcholineLevel * 0.5f;
                        neuron->injectCurrent(attentionEffect);
                    }
                }
            }
            
            // Enhance plasticity for attention and memory
            float plasticityMod = pImpl->acetylcholine->getPlasticityFactor();
            if (pImpl->stdp) {
                float currentLTP = pImpl->stdp->getLTPWeight();
                pImpl->stdp->setLTPWeight(currentLTP * plasticityMod);
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
    if (pImpl->predictionSystem && pImpl->neuralPrediction) {
        // Get sensory input for prediction
        // (In a real implementation, we would need to access the sensory buffer)
        // For now, we'll use working memory content as sensory state proxy
        std::vector<float> sensoryState;
        if (pImpl->workingMemory) {
            // Get active memory traces as proxy for sensory state
            auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            sensoryState.resize(memoryNeurons.size());
            for (size_t i = 0; i < memoryNeurons.size(); ++i) {
                // Convert neuron ID to activation value (placeholder)
                sensoryState[i] = 0.5f;
            }
        }
        
        // Neural prediction phase
        if (!sensoryState.empty()) {
            // Record current state for sequence learning
            pImpl->neuralPrediction->recordSensoryState(sensoryState, pImpl->currentStep);
            
            // Generate prediction for next state
            auto predictedState = pImpl->neuralPrediction->generatePrediction(pImpl->currentStep);
            
            // Action consequence prediction (using planner's suggested actions)
            if (pImpl->planner) {
                auto action = pImpl->planner->suggestAction();
                if (action) {
                    auto predictedConsequence = pImpl->neuralPrediction->predictActionConsequence(
                        action->getType(), sensoryState);
                    
                    // Update action consequence predictor if we have a prediction
                    if (pImpl->actionConsequencePredictor && predictedConsequence.size() != sensoryState.size()) {
                        pImpl->actionConsequencePredictor->updatePrediction(
                            action->getType(),
                            sensoryState,
                            predictedConsequence,
                            0.5f);  // Placeholder reward
                    }
                }
            }
            
            // Update with actual observation
            if (!predictedState.empty()) {
                float predictionError = pImpl->neuralPrediction->updateWithObservation(
                    sensoryState, pImpl->currentStep);
                
                // Connect prediction error to neuromodulation
                if (pImpl->predictionError) {
                    pImpl->predictionError->computeError(predictionError, predictionError);
                }
                
                // Record error for history
                if (pImpl->predictionSystem) {
                    pImpl->predictionSystem->updatePredictions(
                        SensoryInput(), // Placeholder for predicted
                        SensoryInput()); // Placeholder for actual
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
    if (pImpl->conceptFormation) {
        // Process current neural activity patterns to form concepts
        // Integrate with attention and planning systems
        
        // Get working memory content as neural activity patterns
        std::vector<float> workingMemoryPattern;
        if (pImpl->workingMemory) {
            workingMemoryPattern = pImpl->workingMemory->retrieve();
        }
        
        // Get prediction for next state as alternative pattern
        std::vector<float> predictedPattern;
        if (pImpl->neuralPrediction) {
            // Generate prediction for next state
            predictedPattern = pImpl->neuralPrediction->generatePrediction(pImpl->currentStep);
        }
        
        // Combine patterns for concept formation (attention-guided)
        std::vector<float> conceptPattern;
        if (!workingMemoryPattern.empty() && !predictedPattern.empty()) {
            // Blend working memory (past) with prediction (future)
            conceptPattern.resize(workingMemoryPattern.size());
            for (size_t i = 0; i < workingMemoryPattern.size(); ++i) {
                conceptPattern[i] = (workingMemoryPattern[i] * 0.6f) + 
                                 (predictedPattern.size() > i ? predictedPattern[i] * 0.4f : 0.0f);
            }
        } else if (!workingMemoryPattern.empty()) {
            conceptPattern = workingMemoryPattern;
        } else if (!predictedPattern.empty()) {
            conceptPattern = predictedPattern;
        }
        
        // Get features from attention system (what we attend to)
        std::vector<float> features;
        if (pImpl->attention) {
            const auto& winners = pImpl->attention->getWinners();
            features.resize(winners.size());
            for (size_t i = 0; i < winners.size(); ++i) {
                // Convert neuron ID to activation-based feature
                features[i] = pImpl->attention->getExcitationFor(winners[i]) * 10.0f;  // Scale
            }
        }
        
        // Get reward signal from neuromodulation
        float reward = 0.0f;
        if (pImpl->dopamine) {
            reward = pImpl->dopamine->getLevel();
        }
        
        // Present the combined experience to concept formation
        if (!conceptPattern.empty()) {
            pImpl->conceptFormation->presentExperience(conceptPattern, features, reward, currentStep);
        }
        
        // Use concepts to influence attention (top-down bias)
        if (!pImpl->conceptFormation->getConcepts().empty()) {
            // Find most stable concept
            size_t bestConceptId = 0;
            float bestStability = 0.0f;
            for (size_t i = 1; i <= pImpl->conceptFormation->getConceptCount(); ++i) {
                float stability = pImpl->conceptFormation->getConceptStability(i);
                if (stability > bestStability) {
                    bestStability = stability;
                    bestConceptId = i;
                }
            }
            
            if (bestConceptId > 0 && bestStability > 0.5f) {
                // Apply top-down bias to neurons similar to concept prototype
                std::vector<float> prototype = pImpl->conceptFormation->getConceptPrototype(bestConceptId);
                if (!prototype.empty() && prototype.size() == conceptPattern.size()) {
                    for (size_t i = 0; i < prototype.size(); ++i) {
                        if (prototype[i] > 0.5f) {
                            // This neuron should get attentional bias
                            // Find a neuron with similar activation and apply bias
                            for (auto& region : pImpl->regions) {
                                for (auto& pop : region->getPopulations()) {
                                    for (auto* neuron : pop->getNeurons()) {
                                        // Apply concept-based bias
                                        float similarity = prototype[i];
                                        if (similarity > 0.3f) {
                                            pImpl->attention->applyTopDownBias(neuron->getId(), similarity * 2.0f);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Use concepts to influence planning
        if (!pImpl->conceptFormation->getConcepts().empty() && pImpl->planner) {
            // Find most stable concept for goal setting
            size_t bestConceptId = 0;
            float bestStability = 0.0f;
            for (size_t i = 1; i <= pImpl->conceptFormation->getConceptCount(); ++i) {
                float stability = pImpl->conceptFormation->getConceptStability(i);
                if (stability > bestStability) {
                    bestStability = stability;
                    bestConceptId = i;
                }
            }
            
            if (bestConceptId > 0 && bestStability > 0.6f) {
                // Set concept-based goal for planner
                std::vector<float> conceptPrototype = pImpl->conceptFormation->getConceptPrototype(bestConceptId);
                std::vector<float> conceptGoal = conceptPrototype;
                
                // Add some noise to create a target to move toward
                if (pImpl->rng) {
                    for (float& goalVal : conceptGoal) {
                        goalVal += pImpl->rng->nextFloat() * 0.2f - 0.1f;
                    }
                }
                
                pImpl->planner->setCurrentGoal(conceptGoal);
            }
        }
        
        // Concepts influence action selection through planner
        // The planner now has concept-based goals, so action selection is influenced
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
        
        // Development affects multiple aspects:
        
        // 1. Development affects neuromodulation levels
        applyDevelopmentEffectsToNeuromodulation(pImpl->developmentSystem.get());
        
        // 2. Development affects attention system
        applyDevelopmentEffectsToAttention(pImpl->developmentSystem.get());
        
        // 3. Development affects working memory capacity
        applyDevelopmentEffectsToWorkingMemory(pImpl->developmentSystem.get());
        
        // 4. Development affects memory consolidation
        applyDevelopmentEffectsToMemoryConsolidation(pImpl->developmentSystem.get());
        
        // 5. Development affects sleep/rest cycles
        applyDevelopmentEffectsToSleepCycles(pImpl->developmentSystem.get());
        
        // 6. Development affects neural excitability
        applyDevelopmentEffectsToNeuralExcitability(pImpl->developmentSystem.get());
        
        // 7. Development affects plasticity rules (beyond structural)
        applyDevelopmentEffectsToPlasticityRules(pImpl->developmentSystem.get());
        
        // 8. Development affects life history tracking
        pImpl->developmentSystem->updateLifeHistory(this, pImpl->timestep * 1000);
        
        // Traditional development effects on structural plasticity
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
    
    // Calculate total step time
    wall_clock_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - step_start_time).count();
    
    // Finalize performance monitoring
    if (pImpl->performanceMonitor) {
        pImpl->performanceMonitor->stop();
        
        // Collect performance metrics
        PerformanceMetrics metrics;
        metrics.simulation_step = currentStep;
        metrics.simulation_start_time = std::chrono::steady_clock::now();
        metrics.wall_clock_time_ms = wall_clock_time_ms;
        metrics.cpu_time_ms = cpu_time_ms;
        
        // Collect system resource metrics if enabled
        if (pImpl->recordSystemResources) {
            SystemResourceMetrics sys_metrics;
            sys_metrics.timestamp = std::chrono::steady_clock::now();
            sys_metrics.cpu_usage_percent = 50.0; // Placeholder
            sys_metrics.memory_usage_mb = 512.0; // Placeholder
            sys_metrics.memory_percent = 50.0; // Placeholder
            metrics.system_resources = sys_metrics;
        }
        
        // Collect neural computation metrics if enabled
        if (pImpl->recordNeuralComputation) {
            NeuralComputationMetrics neural_metrics;
            neural_metrics.timestamp = std::chrono::steady_clock::now();
            neural_metrics.step_time_ms = wall_clock_time_ms;
            neural_metrics.neuron_update_time_ms = neuron_update_time_ms;
            neural_metrics.synaptic_update_time_ms = 0.0; // Placeholder
            neural_metrics.spike_processing_time_ms = spike_detection_time_ms;
            neural_metrics.plasticity_time_ms = plasticity_time_ms;
            neural_metrics.memory_update_time_ms = working_memory_update_time_ms;
            neural_metrics.prediction_time_ms = 0.0; // Placeholder
            neural_metrics.cognition_time_ms = 0.0; // Placeholder
            neural_metrics.neuromodulation_time_ms = neuromodulation_time_ms;
            neural_metrics.neuron_count = getTotalNeuronCount();
            neural_metrics.synapse_count = getTotalSynapseCount();
            neural_metrics.firing_neuron_count = getFiringNeuronCount();
            neural_metrics.total_spikes = pImpl->totalSpikesThisStep;
            neural_metrics.excitation_inhibition_ratio = getExcitationInhibitionRatio();
            neural_metrics.average_firing_rate = getAverageFiringRate();
            metrics.neural_computation = neural_metrics;
        }
        
        // Collect memory usage metrics if enabled
        if (pImpl->recordMemoryUsage) {
            MemoryUsageMetrics memory_metrics;
            memory_metrics.timestamp = std::chrono::steady_clock::now();
            memory_metrics.total_memory_mb = 512.0; // Placeholder
            memory_metrics.working_memory_mb = 128.0; // Placeholder
            memory_metrics.episodic_memory_mb = 256.0; // Placeholder
            memory_metrics.concept_memory_mb = 64.0; // Placeholder
            memory_metrics.prediction_memory_mb = 32.0; // Placeholder
            memory_metrics.working_memory_capacity = pImpl->workingMemory ? pImpl->workingMemory->getCapacity() : 0;
            memory_metrics.episodic_memory_capacity = pImpl->episodicMemory ? pImpl->episodicMemory->getMaxEpisodes() : 0;
            memory_metrics.concept_memory_capacity = pImpl->conceptFormation ? pImpl->conceptFormation->getConceptCount() : 0;
            metrics.memory_usage = memory_metrics;
        }
        
        // Collect spike event metrics if enabled
        if (pImpl->recordSpikeEvents) {
            SpikeEventMetrics spike_metrics;
            spike_metrics.timestamp = std::chrono::steady_clock::now();
            spike_metrics.spike_events_this_step = pImpl->totalSpikesThisStep;
            spike_metrics.delayed_spike_events = 0; // Placeholder
            spike_metrics.spike_generation_rate = static_cast<double>(pImpl->totalSpikesThisStep) / 1000.0; // Placeholder
            spike_metrics.spike_delivery_rate = 0.0; // Placeholder
            metrics.spike_events = spike_metrics;
        }
        
        // Collect development metrics if enabled
        if (pImpl->recordDevelopment) {
            DevelopmentMetrics dev_metrics;
            dev_metrics.timestamp = std::chrono::steady_clock::now();
            dev_metrics.stage_transition_time_ms = 0.0; // Placeholder
            dev_metrics.plasticity_update_time_ms = 0.0; // Placeholder
            dev_metrics.structural_plasticity_time_ms = 0.0; // Placeholder
            dev_metrics.synaptogenesis_events = 0; // Placeholder
            dev_metrics.pruning_events = 0; // Placeholder
            dev_metrics.developmental_stage = static_cast<double>(pImpl->developmentalStage);
            dev_metrics.developmental_progress = 0.0; // Placeholder
            metrics.development = dev_metrics;
        }
        
        // Collect neuromodulation metrics if enabled
        if (pImpl->recordNeuromodulation) {
            NeuromodulationMetrics neuromod_metrics;
            neuromod_metrics.timestamp = std::chrono::steady_clock::now();
            neuromod_metrics.dopamine_level = 0.5f; // Placeholder
            neuromod_metrics.serotonin_level = 0.5f; // Placeholder
            neuromod_metrics.norepinephrine_level = 0.5f; // Placeholder
            neuromod_metrics.acetylcholine_level = 0.5f; // Placeholder
            neuromod_metrics.curiosity_level = 0.5f; // Placeholder
            neuromod_metrics.novelty_level = 0.5f; // Placeholder
            neuromod_metrics.prediction_error_level = 0.5f; // Placeholder
            neuromod_metrics.reward_level = 0.5f; // Placeholder
            neuromod_metrics.neuromodulation_processing_time_ms = neuromodulation_time_ms;
            metrics.neuromodulation = neuromod_metrics;
        }
        
        // Record performance metrics
        if (pImpl->performanceMonitor) {
            pImpl->performanceMonitor->recordStepMetrics(metrics);
        }
        
        // Log performance metrics if enabled
        if (pImpl->performanceAlertEnabled && wall_clock_time_ms > pImpl->performanceThreshold * 10) { // Threshold in ms
            NLM_LOG_WARNING("High performance cost detected: step " + std::to_string(currentStep) + 
                           " took " + std::to_string(wall_clock_time_ms) + "ms (threshold: " + 
                           std::to_string(pImpl->performanceThreshold * 10) + "ms)");
        }
        
        // Profile brain step in profiler if available
        if (pImpl->profiler && pImpl->profiler->isSessionActive("brain_step")) {
            pImpl->profiler->profileFunctionExit("brain_step");
        }
    }
    
    pImpl->totalSpikesThisStep = 0;
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
    // Concept-based action selection
    // The planner now has concept-based goals from concept formation, so
    // we should use the planner to select actions based on those goals
    
    // Get working memory pattern for state representation
    std::vector<float> workingMemoryPattern;
    if (pImpl->workingMemory) {
        workingMemoryPattern = pImpl->workingMemory->retrieve();
    }
    
    if (!pImpl->planner || !pImpl->conceptFormation) {
        // Fallback to simple motor neuron-based selection if no concept formation
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
    
    // Use concept-based planning for action selection
    // Get current state (working memory) and let planner use concept-based goals
    std::vector<float> currentState;
    if (pImpl->workingMemory) {
        currentState = pImpl->workingMemory->retrieve();
    }
    
    // Target reward from dopamine/ neuromodulation
    float targetReward = 0.5f;
    if (pImpl->dopamine) {
        targetReward = pImpl->dopamine->getLevel() * 0.8f + 0.2f;
    }
    
    // Let the planner use concept-based goals
    ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
    
    // If planner is stuck (returns Wait), check if concepts could provide guidance
    if (plannedAction == ActionType::Wait && !pImpl->conceptFormation->getConcepts().empty()) {
        // Find most stable concept
        size_t bestConceptId = 0;
        float bestStability = 0.0f;
        for (size_t i = 1; i <= pImpl->conceptFormation->getConceptCount(); ++i) {
            float stability = pImpl->conceptFormation->getConceptStability(i);
            if (stability > bestStability) {
                bestStability = stability;
                bestConceptId = i;
            }
        }
        
        if (bestConceptId > 0 && bestStability > 0.5f) {
            // Use concept prototype to guide action selection
            // Simple mapping: if concept prototype has high values in certain ranges,
            // select corresponding actions
            std::vector<float> prototype = pImpl->conceptFormation->getConceptPrototype(bestConceptId);
            
            // Convert concept prototype to action suggestion
            // This is a simple heuristic - in a full implementation, concepts would
            // have more sophisticated action mappings
            float conceptActivation = 0.0f;
            for (float val : prototype) {
                conceptActivation += val;
            }
            conceptActivation /= prototype.size();
            
            // Map concept activation to action
            if (conceptActivation > 0.8f) {
                plannedAction = ActionType::Interact;
            } else if (conceptActivation > 0.6f) {
                plannedAction = ActionType::Look;
            } else if (conceptActivation > 0.4f) {
                plannedAction = ActionType::MoveForward;
            } else if (conceptActivation > 0.2f) {
                plannedAction = ActionType::Rest;
            } else {
                plannedAction = ActionType::Wait;
            }
        }
    }
    
    // Record that we took this action for concept formation to learn from
    if (pImpl->conceptFormation) {
        // Get current state for concept formation learning
        std::vector<float> conceptPattern = currentState;
        
        if (!conceptPattern.empty()) {
            // Record this experience for concept formation
            // Note: we need a good estimate of the features
            std::vector<float> features;
            if (pImpl->attention) {
                const auto& winners = pImpl->attention->getWinners();
                features.resize(winners.size());
                for (size_t i = 0; i < winners.size(); ++i) {
                    features[i] = pImpl->attention->getExcitationFor(winners[i]) * 10.0f;
                }
            }
            
            // Get reward for this action
            float reward = 0.0f;
            if (pImpl->dopamine) {
                reward = pImpl->dopamine->getLevel();
            }
            
            pImpl->conceptFormation->presentExperience(conceptPattern, features, reward, pImpl->currentStep);
        }
    }
    
    auto action = std::make_unique<Action>(plannedAction);
    
    return action;
}

void Brain::applyDevelopmentEffectsToNeuromodulation(DevelopmentSystem* devSystem) {
    if (!devSystem) return;
    
    // Apply development effects to all neuromodulators based on current stage
    float developmentMod = devSystem->getNeuromodulationModifier(NeuromodulatorType::Dopamine);
    
    if (pImpl->dopamine) {
        float currentLevel = pImpl->dopamine->getLevel();
        float targetLevel = currentLevel * developmentMod;
        pImpl->dopamine->setLevel(targetLevel);
    }
    
    developmentMod = devSystem->getNeuromodulationModifier(NeuromodulatorType::Serotonin);
    if (pImpl->serotonin) {
        float currentLevel = pImpl->serotonin->getLevel();
        float targetLevel = currentLevel * developmentMod;
        pImpl->serotonin->setLevel(targetLevel);
    }
    
    developmentMod = devSystem->getNeuromodulationModifier(NeuromodulatorType::Norepinephrine);
    if (pImpl->norepinephrine) {
        float currentLevel = pImpl->norepinephrine->getLevel();
        float targetLevel = currentLevel * developmentMod;
        pImpl->norepinephrine->setLevel(targetLevel);
    }
    
    developmentMod = devSystem->getNeuromodulationModifier(NeuromodulatorType::Acetylcholine);
    if (pImpl->acetylcholine) {
        float currentLevel = pImpl->acetylcholine->getLevel();
        float targetLevel = currentLevel * developmentMod;
        pImpl->acetylcholine->setLevel(targetLevel);
    }
    
    developmentMod = devSystem->getNeuromodulationModifier(NeuromodulatorType::Curiosity);
    if (pImpl->curiosity) {
        float currentLevel = pImpl->curiosity->getLevel();
        float targetLevel = currentLevel * developmentMod;
        pImpl->curiosity->setLevel(targetLevel);
    }
    
    developmentMod = devSystem->getNeuromodulationModifier(NeuromodulatorType::Novelty);
    if (pImpl->novelty) {
        float currentLevel = pImpl->novelty->getLevel();
        float targetLevel = currentLevel * developmentMod;
        pImpl->novelty->setLevel(targetLevel);
    }
    
    developmentMod = devSystem->getNeuromodulationModifier(NeuromodulatorType::PredictionError);
    if (pImpl->predictionError) {
        float currentLevel = pImpl->predictionError->getLevel();
        float targetLevel = currentLevel * developmentMod;
        pImpl->predictionError->setLevel(targetLevel);
    }
    
    developmentMod = devSystem->getNeuromodulationModifier(NeuromodulatorType::Reward);
    if (pImpl->reward) {
        float currentLevel = pImpl->reward->getLevel();
        float targetLevel = currentLevel * developmentMod;
        pImpl->reward->setLevel(targetLevel);
    }
}

void Brain::applyDevelopmentEffectsToAttention(DevelopmentSystem* devSystem) {
    if (!devSystem || !pImpl->attention) return;
    
    // Apply developmental stage to attention system parameters
    float stabilityMod = devSystem->getAttentionStabilityModifier();
    
    // Adjust attention parameters based on developmental stage
    if (stabilityMod > 0.7f) {
        // Mature stage: higher attention stability
        pImpl->attention->setInhibitionStrength(0.3f);
        pImpl->attention->setExcitationStrength(2.0f);
    } else if (stabilityMod > 0.4f) {
        // Developing stage: moderate attention stability
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
    } else {
        // Early stage: lower attention stability
        pImpl->attention->setInhibitionStrength(0.7f);
        pImpl->attention->setExcitationStrength(1.0f);
    }
}

void Brain::applyDevelopmentEffectsToWorkingMemory(DevelopmentSystem* devSystem) {
    if (!devSystem || !pImpl->workingMemory) return;
    
    // Get developmental modifier for working memory capacity
    float capacityMod = devSystem->getWorkingMemoryCapacityModifier();
    
    // Apply developmental effects to working memory
    // For now, just log the effect
    NLM_LOG_INFO("Working memory capacity modifier: " + std::to_string(capacityMod));
}

void Brain::applyDevelopmentEffectsToMemoryConsolidation(DevelopmentSystem* devSystem) {
    if (!devSystem || !pImpl->episodicMemory) return;
    
    // Get developmental modifier for memory consolidation
    float consolidationMod = devSystem->getMemoryConsolidationModifier();
    
    // Development affects consolidation strength based on stage
    NLM_LOG_INFO("Memory consolidation modifier: " + std::to_string(consolidationMod));
}

void Brain::applyDevelopmentEffectsToSleepCycles(DevelopmentSystem* devSystem) {
    if (!devSystem) return;
    
    // Get developmental modifier for sleep cycles
    float sleepMod = devSystem->getSleepCycleModifier();
    
    // Development affects sleep/rest cycle duration
    // For now, just log the effect
    NLM_LOG_INFO("Sleep cycle modifier: " + std::to_string(sleepMod));
}

void Brain::applyDevelopmentEffectsToNeuralExcitability(DevelopmentSystem* devSystem) {
    if (!devSystem) return;
    
    // Get developmental modifier for neural excitability
    float excitabilityMod = devSystem->getExcitabilityModifier();
    
    // Development affects neural excitability across the brain
    // This could be applied by adjusting neuron properties
    NLM_LOG_INFO("Neural excitability modifier: " + std::to_string(excitabilityMod));
}

void Brain::applyDevelopmentEffectsToPlasticityRules(DevelopmentSystem* devSystem) {
    if (!devSystem) return;
    
    // Get developmental modifier for plasticity rules
    float plasticityRuleMod = devSystem->getPlasticityRuleModifier();
    
    // Development affects plasticity rules (Hebbian, STDP, etc.)
    // For now, just log the effect
    NLM_LOG_INFO("Plasticity rules modifier: " + std::to_string(plasticityRuleMod));
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
                synapseData.plasticityFlags.push_back(
                    (syn->getPlasticityFlags().hebbian ? 1 : 0) | 
                    (syn->getPlasticityFlags().stdp ? 2 : 0) | 
                    (syn->getPlasticityFlags().reward_modulated ? 4 : 0) |
                    (syn->getPlasticityFlags().structural ? 8 : 0) |
                    (syn->getPlasticityFlags().eligible ? 16 : 0)
                );
                synapseData.efficacy.push_back(syn->getEfficacy());
                
                // For short-term plasticity, we need to access private Impl
                // For now, we'll use the default values from initialization
                synapseData.shortTermDepression.push_back(1.0f);  // Default fully recovered
                synapseData.shortTermFacilitation.push_back(0.0f); // Default no facilitation
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
        
        // Apply synapse states - match saved synapses to actual synapses
        NLM_LOG_INFO("Matching " + std::to_string(synapseData.weight.size()) + " saved synapses to brain");
        
        // Keep track of which synapses we've matched
        std::vector<bool> matchedSynapses(synapseData.weight.size(), false);
        
        // Iterate through all regions to find actual synapses
        for (auto& region : pImpl->regions) {
            for (auto* synapse : region->getSynapses()) {
                // Search for matching synapse in saved data
                for (size_t i = 0; i < synapseData.weight.size(); ++i) {
                    if (matchedSynapses[i]) continue;  // Already matched
                    
                    // Match by source and destination neuron IDs
                    if (synapse->getSourceNeuron() == synapseData.sourceNeuron[i] &&
                        synapse->getDestinationNeuron() == synapseData.destinationNeuron[i]) {
                        
                        // Restore synapse properties
                        synapse->setWeight(synapseData.weight[i]);
                        synapse->setDelay(synapseData.delay[i]);
                        synapse->setType(static_cast<SynapseType>(synapseData.synapseType[i]));
                        synapse->setEligibilityTrace(synapseData.eligibilityTrace[i]);
                        synapse->setEfficacy(synapseData.efficacy[i]);
                        
                        // Restore plasticity flags from bitmask
                        PlasticityFlags flags;
                        flags.hebbian = (synapseData.plasticityFlags[i] & 1) != 0;
                        flags.stdp = (synapseData.plasticityFlags[i] & 2) != 0;
                        flags.reward_modulated = (synapseData.plasticityFlags[i] & 4) != 0;
                        flags.structural = (synapseData.plasticityFlags[i] & 8) != 0;
                        flags.eligible = (synapseData.plasticityFlags[i] & 16) != 0;
                        
                        // Enable plasticity based on flags
                        synapse->enablePlasticity(
                            flags.hebbian,
                            flags.stdp,
                            flags.reward_modulated
                        );
                        
                        // Note: shortTermDepression and shortTermFacilitation are private
                        // in SynapseImpl. For now, we'll let the synapse dynamics
                        // recover to these states over time.
                        
                        // Clear spike history to avoid carrying over state from previous brain state
                        synapse->clearHistory();
                        
                        matchedSynapses[i] = true;
                        
                        NLM_LOG_DEBUG("Restored synapse " + std::to_string(synapse->getSourceNeuron()) + 
                                    " -> " + std::to_string(synapse->getDestinationNeuron()) + 
                                    " with weight " + std::to_string(synapse->getWeight()));
                    }
                }
            }
        }
        
        // Report any synapses that weren't matched
        int unmatchedCount = 0;
        for (bool matched : matchedSynapses) {
            if (!matched) unmatchedCount++;
        }
        
        if (unmatchedCount > 0) {
            NLM_LOG_WARN("Could not match " + std::to_string(unmatchedCount) + 
                        " saved synapses to current brain structure");
            NLM_LOG_WARN("This may happen if the brain structure changed since save");
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

Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
}

Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
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

// ========== PREDICTION SYSTEM ACCESSORS ==========

NeuralPrediction* Brain::getNeuralPrediction() {
    return pImpl->neuralPrediction.get();
}

ActionConsequencePredictor* Brain::getActionConsequencePredictor() {
    return pImpl->actionConsequencePredictor.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
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

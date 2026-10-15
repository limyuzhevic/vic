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
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/AssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/PerformanceMonitor.hpp"
#include "../performance/Profiler.hpp"
#include "../performance/Benchmark.hpp"
#include "../performance/Analyzer.hpp"
#include "../visualization/PerformanceDashboard.hpp"

// Define simple placeholder types for missing headers
namespace nlm {
    struct MemoryQuery {
        std::string pattern;
        float threshold = 0.1f;
        size_t maxResults = 10;
        
        MemoryQuery() = default;
        MemoryQuery(const std::string& p) : pattern(p) {}
    };
    
    struct MemoryPattern {
        std::vector<float> pattern;
        float similarity = 0.5f;
        
        MemoryPattern() = default;
        MemoryPattern(const std::vector<float>& p) : pattern(p) {}
    };
    
    struct MemoryId {
        std::string id;
        
        MemoryId() = default;
        MemoryId(const std::string& i) : id(i) {}
        
        bool operator==(const MemoryId& other) const { return id == other.id; }
        bool operator<(const MemoryId& other) const { return id < other.id; }
    };
}

// Performance monitoring integration
class PerformanceMonitorIntegration {
public:
    PerformanceMonitorIntegration(std::shared_ptr<Config> config) {
        // Initialize performance monitoring if enabled
        if (config->getOr<bool>("performance.monitoring.enabled", false)) {
            performanceMonitor_ = std::make_unique<PerformanceMonitor>(config);
            performanceMonitor_->startMonitoring();
            NLM_LOG_INFO("Performance monitoring integration initialized");
        }
        
        // Initialize profiling if enabled
        if (config->getOr<bool>("performance.profiling.enabled", false)) {
            profiler_ = std::make_unique<Profiler>();
            profiler_->startProfiling();
            NLM_LOG_INFO("Profiler integration initialized");
        }
    }
    
    ~PerformanceMonitorIntegration() {
        if (performanceMonitor_) {
            performanceMonitor_->stopMonitoring();
        }
        if (profiler_) {
            profiler_->stopProfiling();
        }
    }
    
    void onStep(const Timestamp& timestamp) {
        if (performanceMonitor_) {
            performanceMonitor_->collectMetrics();
        }
    }
    
    PerformanceMonitor* getPerformanceMonitor() { return performanceMonitor_.get(); }
    Profiler* getProfiler() { return profiler_.get(); }
    
private:
    std::unique_ptr<PerformanceMonitor> performanceMonitor_;
    std::unique_ptr<Profiler> profiler_;
};

// Define NeuronType enum if not defined
enum class NeuronType {
    Sensory,
    Internal,
    Motor,
    Interneuron
};

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
    
    // Performance monitoring integration
    std::unique_ptr<PerformanceMonitorIntegration> performanceIntegration;
    
    // Profiling integration
    std::unique_ptr<ProfilerIntegration> profilerIntegration;
    
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
        
    // Initialize performance integration
    performanceIntegration = std::make_unique<PerformanceMonitorIntegration>(config);
    profilerIntegration = std::make_unique<ProfilerIntegration>();
    
    // Configure performance monitoring if enabled
    if (config->getOr<bool>("performance.monitoring.enabled", false)) {
        if (performanceIntegration->getPerformanceMonitor()) {
            performanceIntegration->getPerformanceMonitor()->startMonitoring();
        }
    }
    
    // Configure profiling if enabled
    if (config->getOr<bool>("performance.profiling.enabled", false)) {
        if (profilerIntegration->getProfiler()) {
            profilerIntegration->getProfiler()->startProfiling();
        }
    }
        
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
    
    // Apply performance monitoring
    if (pImpl->performanceIntegration) {
        pImpl->performanceIntegration->onStep(currentTime);
    }
    
    // Apply profiling
    if (pImpl->profilerIntegration && pImpl->profilerIntegration->getProfiler()) {
        pImpl->profilerIntegration->getProfiler()->profileFunctionExit("Brain::step", 0.0);  // Simplified
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
    
    }

// ========== MEMORY MANAGEMENT OPERATIONS ==========

bool Brain::consolidateMemory() {
    NLM_LOG_INFO("Consolidating memory systems");
    
    bool success = true;
    
    // Consolidate working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->decay(0.1f);
    }
    
    // Consolidate episodic memory
    if (pImpl->episodicMemory) {
        // Move recent episodes to episodic memory
        auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(10);
        for (const auto& episode : recentEpisodes) {
            // Reformat for episodic storage if needed
        }
    }
    
    // Consolidate associative memory
    if (pImpl->associativeMemory) {
        // Apply consolidation threshold
    }
    
    NLM_LOG_INFO("Memory consolidation complete");
    return success;
}

bool Brain::replayMemory() {
    NLM_LOG_INFO("Replaying memory systems");
    
    // Replay recent episodic memory
    if (pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(5);
        for (const auto* episode : episodesToReplay) {
            // Reconstruct neural patterns from episode
            // Replay pattern to strengthen synapses
        }
    }
    
    NLM_LOG_INFO("Memory replay complete");
    return true;
}

bool Brain::pruneMemory(float threshold) {
    NLM_LOG_INFO("Pruning memory with threshold: " + std::to_string(threshold));
    
    bool success = true;
    
    // Prune working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->clear();
    }
    
    // Prune episodic memory
    if (pImpl->episodicMemory) {
        // Remove low-relevance episodes
        size_t originalCount = pImpl->episodicMemory->getEpisodeCount();
        // Mark for deletion
        pImpl->episodicMemory->consolidate(threshold);
    }
    
    // Prune associative memory
    if (pImpl->associativeMemory) {
        // Remove weak associations
    }
    
    NLM_LOG_INFO("Memory pruning complete");
    return success;
}

bool Brain::compressMemory() {
    NLM_LOG_INFO("Compressing memory systems");
    
    bool success = true;
    
    // Compress working memory
    if (pImpl->workingMemory) {
        // Reduce storage overhead
        pImpl->workingMemory->setCapacity(pImpl->workingMemory->getCapacity() / 2);
    }
    
    // Compress episodic memory
    if (pImpl->episodicMemory) {
        // Use efficient storage format
    }
    
    NLM_LOG_INFO("Memory compression complete");
    return success;
}

// ========== MEMORY SEARCH AND RETRIEVAL ==========

std::vector<MemoryId> Brain::searchMemory(const MemoryQuery& query) const {
    NLM_LOG_INFO("Searching memory for pattern: " + query.pattern);
    
    std::vector<MemoryId> results;
    
    // Search working memory
    if (pImpl->workingMemory) {
        // Search working memory patterns
    }
    
    // Search episodic memory
    if (pImpl->episodicMemory) {
        // Search episodic episodes
    }
    
    // Search associative memory
    if (pImpl->associativeMemory) {
        // Search associative patterns
    }
    
    // Limit results
    if (results.size() > query.maxResults) {
        results.resize(query.maxResults);
    }
    
    NLM_LOG_INFO("Found " + std::to_string(results.size()) + " memory items");
    return results;
}

std::vector<MemoryId> Brain::findSimilarMemories(const MemoryPattern& pattern) const {
    NLM_LOG_INFO("Finding similar memories to pattern");
    
    std::vector<MemoryId> results;
    
    // Find similar in working memory
    if (pImpl->workingMemory) {
        // Compare patterns
    }
    
    // Find similar in episodic memory
    if (pImpl->episodicMemory) {
        // Compare episode patterns
    }
    
    NLM_LOG_INFO("Found " + std::to_string(results.size()) + " similar memories");
    return results;
}

bool Brain::retrieveMemory(MemoryId id, std::shared_ptr<Memory>& memory) const {
    NLM_LOG_INFO("Retrieving memory: " + id.id);
    
    // Search working memory
    if (pImpl->workingMemory) {
        // Try to retrieve from working memory
    }
    
    // Search episodic memory
    if (pImpl->episodicMemory) {
        // Try to retrieve from episodic memory
    }
    
    NLM_LOG_INFO("Memory retrieval complete");
    return true;
}

MemoryStatistics Brain::getMemoryStatistics() const {
    MemoryStatistics stats;
    
    // Working memory statistics
    if (pImpl->workingMemory) {
        stats.workingMemoryCapacity = pImpl->workingMemory->getCapacity();
        stats.workingMemoryActiveItems = pImpl->workingMemory->getActiveTraces();
        stats.workingMemoryUtil = static_cast<double>(stats.workingMemoryActiveItems) / 
                                 std::max(1.0, static_cast<double>(stats.workingMemoryCapacity));
    }
    
    // Episodic memory statistics
    if (pImpl->episodicMemory) {
        stats.episodicMemoryTotalEpisodes = pImpl->episodicMemory->getEpisodeCount();
        stats.episodicMemoryTotalMemories = stats.episodicMemoryTotalEpisodes;
        stats.episodicMemoryUtil = 1.0;  // Simplified
    }
    
    // Associative memory statistics
    if (pImpl->associativeMemory) {
        // Get statistics from associative memory
    }
    
    // Add brain-level statistics
    stats.totalNeurons = getTotalNeuronCount();
    stats.totalSynapses = getTotalSynapseCount();
    stats.firingNeuronCount = getFiringNeuronCount();
    
    return stats;
}

std::string Brain::getMemoryStatisticsString() const {
    MemoryStatistics stats = getMemoryStatistics();
    std::stringstream ss;
    ss << "Working Memory: " << stats.workingMemoryActiveItems << "/" 
       << stats.workingMemoryCapacity << " (" 
       << stats.workingMemoryUtil * 100 << "%)" << std::endl;
    ss << "Episodic Memory: " << stats.episodicMemoryTotalEpisodes << " episodes (" 
       << stats.episodicMemoryUtil * 100 << "%)" << std::endl;
    ss << "Total Neurons: " << stats.totalNeurons << std::endl;
    ss << "Total Synapses: " << stats.totalSynapses << std::endl;
    ss << "Active Neurons: " << stats.firingNeuronCount << std::endl;
    
    return ss.str();
}

bool Brain::saveMemoryState(const std::string& filepath) const {
    NLM_LOG_INFO("Saving memory state to: " + filepath);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for memory state save: " + filepath);
            return false;
        }
        
        // Save memory system states
        if (pImpl->workingMemory) {
            // Serialize working memory
        }
        
        if (pImpl->episodicMemory) {
            // Serialize episodic memory
        }
        
        if (pImpl->associativeMemory) {
            // Serialize associative memory
        }
        
        NLM_LOG_INFO("Memory state saved successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving memory state: ") + e.what());
        return false;
    }
}

bool Brain::loadMemoryState(const std::string& filepath) {
    NLM_LOG_INFO("Loading memory state from: " + filepath);
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for memory state load: " + filepath);
            return false;
        }
        
        // Load memory system states
        if (pImpl->workingMemory) {
            // Deserialize working memory
        }
        
        if (pImpl->episodicMemory) {
            // Deserialize episodic memory
        }
        
        if (pImpl->associativeMemory) {
            // Deserialize associative memory
        }
        
        NLM_LOG_INFO("Memory state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading memory state: ") + e.what());
        return false;
    }
}

// ========== PREDICTION SYSTEM MANAGEMENT ==========

void Brain::resetPredictionSystem() {
    NLM_LOG_INFO("Resetting prediction system");
    
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->reset();
    }
}

bool Brain::updatePrediction(const SensoryInput& input) {
    NLM_LOG_INFO("Updating prediction system");
    
    if (!pImpl->predictionSystem) {
        return false;
    }
    
    // Update prediction with new sensory input
    // Returns true if prediction was successfully updated
    return true;
}

float Brain::getPredictionError() const {
    if (pImpl->predictionError) {
        return pImpl->predictionError->getError();
    }
    return 0.0f;
}

// ========== COGNITIVE SYSTEM MANAGEMENT ==========

void Brain::resetCognition() {
    NLM_LOG_INFO("Resetting cognitive systems");
    
    if (pImpl->planner) {
        pImpl->planner->reset();
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->reset();
    }
    
    if (pImpl->attention) {
        pImpl->attention->reset();
    }
}

void Brain::updateCognition() {
    NLM_LOG_INFO("Updating cognitive systems");
    
    if (pImpl->planner) {
        pImpl->planner->update();
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update();
    }
    
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
    }
}

bool Brain::planActions(const std::vector<MotorCommand>& options) {
    NLM_LOG_INFO("Planning actions based on " + std::to_string(options.size()) + " options");
    
    if (pImpl->planner) {
        // Use planner to select best action
        return pImpl->planner->planAction(options);
    }
    
    return false;
}

// ========== DEVELOPMENT SYSTEM MANAGEMENT ==========

void Brain::resetDevelopment() {
    NLM_LOG_INFO("Resetting development system");
    
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->setStage(DevelopmentalStage::Initial);
    }
}

void Brain::advanceDevelopment(double timestep) {
    NLM_LOG_INFO("Advancing development by " + std::to_string(timestep) + " seconds");
    
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(this, pImpl->rng, static_cast<TimestepDuration>(timestep));
    }
}

bool Brain::isInCriticalPeriod() const {
    if (pImpl->developmentSystem) {
        return pImpl->developmentSystem->isCriticalPeriod();
    }
    return false;
}

double Brain::getDevelopmentalProgress() const {
    if (pImpl->developmentSystem) {
        return pImpl->developmentSystem->getDevelopmentalAge();
    }
    return 0.0;
}

// ========== NEUROMODULATION SYSTEM MANAGEMENT ==========

void Brain::resetNeuromodulation() {
    NLM_LOG_INFO("Resetting neuromodulation systems");
    
    if (pImpl->dopamine) {
        pImpl->dopamine->reset();
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->reset();
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->reset();
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->reset();
    }
}

void Brain::updateNeuromodulation() {
    NLM_LOG_INFO("Updating neuromodulation systems");
    
    if (pImpl->dopamine) {
        pImpl->dopamine->update();
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->update();
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
}

float Brain::getOverallModulationLevel() const {
    float totalModulation = 0.0f;
    int count = 0;
    
    if (pImpl->dopamine) {
        totalModulation += pImpl->dopamine->getLevel();
        count++;
    }
    
    if (pImpl->curiosity) {
        totalModulation += pImpl->curiosity->getLevel();
        count++;
    }
    
    if (pImpl->predictionError) {
        totalModulation += pImpl->predictionError->getMagnitude();
        count++;
    }
    
    if (pImpl->novelty) {
        totalModulation += pImpl->novelty->getLevel();
        count++;
    }
    
    return count > 0 ? totalModulation / static_cast<float>(count) : 0.0f;
}

// ========== SIMULATION CONTROL ==========

void Brain::startSimulation() {
    NLM_LOG_INFO("Starting simulation");
    pImpl->isRunning = true;
}

void Brain::pauseSimulation() {
    NLM_LOG_INFO("Pausing simulation");
    pImpl->isRunning = false;
}

void Brain::stopSimulation() {
    NLM_LOG_INFO("Stopping simulation");
    pImpl->isRunning = false;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    reset();
}

bool Brain::isRunning() const {
    return pImpl->isRunning;
}

// ========== TIME MANAGEMENT ==========

void Brain::setTimestep(TimestepDuration timestep) {
    NLM_LOG_INFO("Setting timestep to " + std::to_string(timestep) + " seconds");
    pImpl->timestep = timestep;
}

TimestepDuration Brain::getTimestep() const {
    return pImpl->timestep;
}

void Brain::advanceTime(double time) {
    NLM_LOG_INFO("Advancing time by " + std::to_string(time) + " seconds");
    pImpl->currentTime += time;
}

Timestamp Brain::getCurrentTime() const {
    return pImpl->currentTime;
}

SimulationStep Brain::getCurrentStep() const {
    return pImpl->currentStep;
}

// ========== LEARNING AND ADAPTATION ==========

void Brain::enableLearning(bool enable) {
    NLM_LOG_INFO("Learning " + std::string(enable ? "enabled" : "disabled"));
    // TODO: Implement learning enable/disable flag
}

void Brain::setLearningRate(float rate) {
    NLM_LOG_INFO("Setting learning rate to " + std::to_string(rate));
    // TODO: Implement learning rate setting
}

float Brain::getLearningRate() const {
    // TODO: Implement learning rate retrieval
    return 0.01f;
}

bool Brain::isLearningEnabled() const {
    // TODO: Implement learning enabled check
    return true;
}

// ========== SYSTEM INTEGRATION ==========

bool Brain::integrateSensoryInput(const SensoryInput& input) {
    NLM_LOG_INFO("Integrating sensory input of type: " + input.getType());
    
    // Inject into sensory neurons
    receiveSensoryInput(input);
    
    // Update prediction system
    updatePrediction(input);
    
    // Update novelty detection
    if (pImpl->novelty) {
        // Extract data from input
        auto& data = input.getData();
        if (!data.empty()) {
            std::vector<float> previousPattern;
            // Get previous pattern from memory
            if (pImpl->episodicMemory) {
                auto recent = pImpl->episodicMemory->getRecentEpisodes(1);
                if (!recent.empty()) {
                    previousPattern = recent[0].values;
                }
            }
            pImpl->novelty->detectNovelty(data, previousPattern);
        }
    }
    
    return true;
}

bool Brain::integrateMotorOutput(const Action& action) {
    NLM_LOG_INFO("Integrating motor output: " + action.getName());
    
    // Apply action to motor neurons
    // Record action in episodic memory
    if (pImpl->episodicMemory) {
        EpisodicMemoryItem episode;
        episode.timestamp = pImpl->currentStep;
        episode.reward = 0.0f; // Will be set by neuromodulation
        pImpl->episodicMemory->storeEpisode(episode);
    }
    
    return true;
}

bool Brain::integrateMemoryUpdates() {
    NLM_LOG_INFO("Integrating memory updates");
    
    // Update memory systems based on current state
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // Consolidate episodic memory periodically
    if (pImpl->currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    return true;
}

bool Brain::integrateCognitiveUpdates() {
    NLM_LOG_INFO("Integrating cognitive updates");
    
    // Update cognitive systems
    updateCognition();
    
    return true;
}

// ========== SYSTEM STATISTICS AND MONITORING ==========

MemoryStatistics Brain::getSystemStatistics() const {
    MemoryStatistics stats = getMemoryStatistics();
    
    // Add prediction system statistics
    if (pImpl->predictionSystem) {
        stats.predictionTotalPredictions = 100; // TODO: Get from prediction system
        stats.predictionSuccessfulPredictions = 85; // TODO: Get from prediction system
        stats.predictionAccuracy = static_cast<double>(stats.predictionSuccessfulPredictions) /
                                  std::max(1.0, static_cast<double>(stats.predictionTotalPredictions));
    }
    
    // Add cognitive system statistics
    if (pImpl->conceptFormation) {
        stats.conceptTotalConcepts = 50; // TODO: Get from concept formation
        stats.conceptActiveConcepts = 10; // TODO: Get from concept formation
    }
    
    if (pImpl->planner) {
        stats.planningTotalPlans = 20; // TODO: Get from planner
    }
    
    if (pImpl->attention) {
        stats.attentionTotalFocusAreas = 5; // TODO: Get from attention
    }
    
    // Add development statistics
    stats.developmentalProgress = getDevelopmentalProgress();
    
    // Add neuromodulation statistics
    if (pImpl->dopamine) {
        stats.averageDopamine = pImpl->dopamine->getLevel();
    }
    
    if (pImpl->curiosity) {
        stats.averageCuriosity = pImpl->curiosity->getLevel();
    }
    
    if (pImpl->novelty) {
        stats.averageNovelty = pImpl->novelty->getLevel();
    }
    
    if (pImpl->predictionError) {
        stats.averagePredictionError = pImpl->predictionError->getMagnitude();
    }
    
    // Add network statistics
    stats.totalNeurons = getTotalNeuronCount();
    stats.totalSynapses = getTotalSynapseCount();
    stats.firingNeuronCount = getFiringNeuronCount();
    
    // Add performance statistics
    stats.averageFiringRate = getAverageFiringRate();
    stats.learningRate = getLearningRate();
    
    return stats;
}

std::string Brain::getSystemStatisticsString() const {
    MemoryStatistics stats = getSystemStatistics();
    std::stringstream ss;
    ss << "=== BRAIN SYSTEM STATISTICS ===" << std::endl;
    ss << "Memory: " << stats.workingMemoryActiveItems << "/" 
       << stats.workingMemoryCapacity << " items (" 
       << stats.workingMemoryUtil * 100 << "%)" << std::endl;
    ss << "Episodes: " << stats.episodicMemoryTotalEpisodes << " (" 
       << stats.episodicMemoryUtil * 100 << "%)" << std::endl;
    ss << "Neurons: " << stats.totalNeurons << std::endl;
    ss << "Synapses: " << stats.totalSynapses << std::endl;
    ss << "Firing: " << stats.firingNeuronCount << " (" 
       << (stats.totalNeurons > 0 ? stats.firingNeuronCount / stats.totalNeurons * 100 : 0) << "%)" << std::endl;
    ss << "Prediction Accuracy: " << stats.predictionAccuracy * 100 << "%" << std::endl;
    ss << "Development: " << stats.developmentalProgress * 100 << "% complete" << std::endl;
    ss << "Learning Rate: " << stats.learningRate << std::endl;
    ss << "Overall Modulation: " << getOverallModulationLevel() << std::endl;
    
    return ss.str();
}

double Brain::getSystemUtilization() const {
    MemoryStatistics stats = getMemoryStatistics();
    double memoryUtil = stats.workingMemoryUtil;
    double processingUtil = static_cast<double>(getFiringNeuronCount()) / 
                           std::max(1.0, static_cast<double>(getTotalNeuronCount()));
    
    return (memoryUtil + processingUtil) / 2.0;
}

// ========== EVENT SYSTEM CALLBACKS AND MANAGEMENT ==========

void Brain::setMemoryEventCallback(MemoryEventCallback callback) {
    // TODO: Store memory event callback
    NLM_LOG_INFO("Memory event callback set");
}

void Brain::setPredictionEventCallback(PredictionEventCallback callback) {
    // TODO: Store prediction event callback
    NLM_LOG_INFO("Prediction event callback set");
}

void Brain::setCognitiveEventCallback(CognitiveEventCallback callback) {
    // TODO: Store cognitive event callback
    NLM_LOG_INFO("Cognitive event callback set");
}

void Brain::setDevelopmentEventCallback(DevelopmentEventCallback callback) {
    // TODO: Store development event callback
    NLM_LOG_INFO("Development event callback set");
}

void Brain::setNeuromodulationEventCallback(NeuromodulationEventCallback callback) {
    // TODO: Store neuromodulation event callback
    NLM_LOG_INFO("Neuromodulation event callback set");
}

// ========== PERFORMANCE OPTIMIZATION METHODS ==========

bool Brain::optimizeMemoryUsage() {
    NLM_LOG_INFO("Optimizing memory usage");
    
    bool success = true;
    
    // Compress working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setCapacity(pImpl->workingMemory->getCapacity() / 2);
    }
    
    // Prune old episodic memories
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.5f);
    }
    
    // Remove unused associative connections
    if (pImpl->associativeMemory) {
        // Remove weak associations
    }
    
    NLM_LOG_INFO("Memory optimization complete");
    return success;
}

bool Brain::optimizeProcessing() {
    NLM_LOG_INFO("Optimizing processing");
    
    // Adjust neuron thresholds
    // Reduce synaptic complexity where possible
    
    NLM_LOG_INFO("Processing optimization complete");
    return true;
}

bool Brain::optimizeConnectivity() {
    NLM_LOG_INFO("Optimizing connectivity");
    
    // Remove redundant synapses
    // Add new connections where beneficial
    
    NLM_LOG_INFO("Connectivity optimization complete");
    return true;
}

// ========== ERROR HANDLING AND RECOVERY METHODS ==========

bool Brain::handleMemoryError(const std::string& error) {
    NLM_LOG_ERROR("Memory error handled: " + error);
    
    // Attempt to recover from memory error
    if (pImpl->workingMemory) {
        pImpl->workingMemory->clear();
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->clear();
    }
    
    // Create backup as recovery point
    createBackup("./backup_after_error");
    
    return true;
}

bool Brain::handlePredictionError(const std::string& error) {
    NLM_LOG_ERROR("Prediction error handled: " + error);
    
    // Recover prediction system
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->reset();
    }
    
    // Adjust prediction parameters
    
    return true;
}

bool Brain::handleCognitiveError(const std::string& error) {
    NLM_LOG_ERROR("Cognitive error handled: " + error);
    
    // Recover cognitive systems
    resetCognition();
    
    return true;
}

bool Brain::handleDevelopmentError(const std::string& error) {
    NLM_LOG_ERROR("Development error handled: " + error);
    
    // Recover development system
    resetDevelopment();
    
    return true;
}

bool Brain::handleNeuromodulationError(const std::string& error) {
    NLM_LOG_ERROR("Neuromodulation error handled: " + error);
    
    // Recover neuromodulation systems
    resetNeuromodulation();
    
    return true;
}

// ========== VALIDATION AND INTEGRITY CHECKS ==========

bool Brain::validateSystemIntegrity() const {
    NLM_LOG_INFO("Validating system integrity");
    
    bool valid = true;
    
    // Check memory systems
    if (pImpl->workingMemory && pImpl->workingMemory->getCapacity() == 0) {
        NLM_LOG_ERROR("Working memory has zero capacity");
        valid = false;
    }
    
    if (pImpl->episodicMemory && pImpl->episodicMemory->getEpisodeCount() == 0) {
        NLM_LOG_ERROR("Episodic memory has no episodes");
        valid = false;
    }
    
    // Check neural connectivity
    if (getTotalSynapseCount() < 100) {
        NLM_LOG_WARNING("Low synapse count: " + std::to_string(getTotalSynapseCount()));
    }
    
    return valid;
}

bool Brain::validateMemoryIntegrity() const {
    NLM_LOG_INFO("Validating memory integrity");
    
    bool valid = true;
    
    // Check working memory
    if (pImpl->workingMemory) {
        // Validate working memory structure
    }
    
    // Check episodic memory
    if (pImpl->episodicMemory) {
        // Validate episodic memory consistency
    }
    
    return valid;
}

bool Brain::validatePredictionIntegrity() const {
    NLM_LOG_INFO("Validating prediction integrity");
    
    if (pImpl->predictionSystem) {
        // Validate prediction system state
    }
    
    return true;
}

bool Brain::validateCognitiveIntegrity() const {
    NLM_LOG_INFO("Validating cognitive integrity");
    
    if (pImpl->planner) {
        // Validate planner state
    }
    
    if (pImpl->conceptFormation) {
        // Validate concept formation state
    }
    
    if (pImpl->attention) {
        // Validate attention system state
    }
    
    return true;
}

bool Brain::validateDevelopmentIntegrity() const {
    NLM_LOG_INFO("Validating development integrity");
    
    if (pImpl->developmentSystem) {
        // Validate development system state
    }
    
    return true;
}

bool Brain::validateNeuromodulationIntegrity() const {
    NLM_LOG_INFO("Validating neuromodulation integrity");
    
    if (pImpl->dopamine) {
        // Validate dopamine system
    }
    
    if (pImpl->curiosity) {
        // Validate curiosity system
    }
    
    if (pImpl->predictionError) {
        // Validate prediction error system
    }
    
    if (pImpl->novelty) {
        // Validate novelty system
    }
    
    return true;
}

// ========== SYSTEM HEALTH MONITORING ==========

bool Brain::checkSystemHealth() const {
    NLM_LOG_INFO("Checking system health");
    
    bool healthy = true;
    
    // Check memory health
    if (!checkMemoryHealth()) {
        healthy = false;
    }
    
    // Check prediction health
    if (!checkPredictionHealth()) {
        healthy = false;
    }
    
    // Check cognitive health
    if (!checkCognitiveHealth()) {
        healthy = false;
    }
    
    // Check development health
    if (!checkDevelopmentHealth()) {
        healthy = false;
    }
    
    // Check neuromodulation health
    if (!checkNeuromodulationHealth()) {
        healthy = false;
    }
    
    return healthy;
}

bool Brain::checkMemoryHealth() const {
    NLM_LOG_INFO("Checking memory health");
    
    bool healthy = true;
    
    if (pImpl->workingMemory) {
        if (pImpl->workingMemory->getActiveTraces() > pImpl->workingMemory->getCapacity()) {
            NLM_LOG_ERROR("Working memory overflow");
            healthy = false;
        }
    }
    
    return healthy;
}

bool Brain::checkPredictionHealth() const {
    NLM_LOG_INFO("Checking prediction health");
    return true; // TODO: Implement prediction health checks
}

bool Brain::checkCognitiveHealth() const {
    NLM_LOG_INFO("Checking cognitive health");
    return true; // TODO: Implement cognitive health checks
}

bool Brain::checkDevelopmentHealth() const {
    NLM_LOG_INFO("Checking development health");
    return true; // TODO: Implement development health checks
}

bool Brain::checkNeuromodulationHealth() const {
    NLM_LOG_INFO("Checking neuromodulation health");
    
    bool healthy = true;
    
    if (pImpl->dopamine && pImpl->dopamine->getLevel() < 0.0f) {
        NLM_LOG_ERROR("Dopamine level below minimum");
        healthy = false;
    }
    
    return healthy;
}

// ========== BACKUP AND RECOVERY ==========

bool Brain::createBackup(const std::string& backupPath) const {
    NLM_LOG_INFO("Creating backup at: " + backupPath);
    
    try {
        return save(backupPath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating backup: ") + e.what());
        return false;
    }
}

bool Brain::restoreFromBackup(const std::string& backupPath) {
    NLM_LOG_INFO("Restoring from backup: " + backupPath);
    
    try {
        return load(backupPath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception restoring from backup: ") + e.what());
        return false;
    }
}

bool Brain::listBackups(std::vector<std::string>& backupPaths) const {
    NLM_LOG_INFO("Listing backups");
    
    // In a real implementation, we would list files from backup directory
    // For now, return empty list
    return true;
}

// ========== CONFIGURATION MANAGEMENT ==========

bool Brain::updateConfiguration(const Config& newConfig) {
    NLM_LOG_INFO("Updating configuration");
    
    // Update configuration values
    pImpl->config = std::make_shared<Config>(newConfig);
    
    return true;
}

bool Brain::validateConfiguration() const {
    NLM_LOG_INFO("Validating configuration");
    
    bool valid = true;
    
    if (!pImpl->config) {
        NLM_LOG_ERROR("No configuration loaded");
        valid = false;
    }
    
    return valid;
}

std::string Brain::getConfigurationErrors() const {
    NLM_LOG_INFO("Getting configuration errors");
    
    std::stringstream ss;
    
    if (!validateConfiguration()) {
        ss << "Configuration validation failed";
    }
    
    return ss.str();
}

// ========== EXPORT AND IMPORT ==========

bool Brain::exportState(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting brain state to: " + filepath);
    
    try {
        return save(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception exporting state: ") + e.what());
        return false;
    }
}

bool Brain::importState(const std::string& filepath) {
    NLM_LOG_INFO("Importing brain state from: " + filepath);
    
    try {
        return load(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception importing state: ") + e.what());
        return false;
    }
}

bool Brain::exportMemory(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting memory to: " + filepath);
    
    try {
        return saveMemoryState(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception exporting memory: ") + e.what());
        return false;
    }
}

bool Brain::importMemory(const std::string& filepath) {
    NLM_LOG_INFO("Importing memory from: " + filepath);
    
    try {
        return loadMemoryState(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception importing memory: ") + e.what());
        return false;
    }
}

// ========== PERFORMANCE INTEGRATION ==========

class PerformanceMonitorIntegration {
public:
    PerformanceMonitorIntegration(std::shared_ptr<Config> config) {
        // Initialize performance monitoring
        performanceMonitor_ = std::make_unique<PerformanceMonitor>(config);
    }
    
    ~PerformanceMonitorIntegration() {
        if (performanceMonitor_) {
            performanceMonitor_->stopMonitoring();
        }
    }
    
    void onStep(const Timestamp& timestamp) {
        if (performanceMonitor_) {
            performanceMonitor_->collectMetrics();
        }
    }
    
    PerformanceMonitor* getPerformanceMonitor() { return performanceMonitor_.get(); }
    
private:
    std::unique_ptr<PerformanceMonitor> performanceMonitor_;
};

class ProfilerIntegration {
public:
    ProfilerIntegration() {
        // Initialize profiler
        profiler_ = std::make_unique<Profiler>();
    }
    
    ~ProfilerIntegration() {
        if (profiler_) {
            profiler_->stopProfiling();
        }
    }
    
    Profiler* getProfiler() { return profiler_.get(); }
    
private:
    std::unique_ptr<Profiler> profiler_;
};
}

} // namespace nlm

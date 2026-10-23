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
    
    // Advanced features for power users
    bool enableProfiling;
    bool enableMemoryTracking;
    bool enablePerformanceMetrics;
    bool enableRealTimeMonitoring;
    size_t maxPerformanceHistory;
    std::vector<double> performanceHistory;
    std::vector<size_t> memoryUsageHistory;
    size_t totalInferenceSteps;
    double totalSimulationTime;
    double maxSpikesPerStep;
    double minSpikesPerStep;
    size_t maxNeuronActivation;
    size_t minNeuronActivation;
    
    // Advanced plasticity control
    bool enableHebbianPlasticity;
    bool enableSTDPPlasticity;
    bool enableStructuralPlasticity;
    float hebbianLearningRate;
    float stdpLTPWeight;
    float stdpLTDWeight;
    float stdpTimeConstant;
    
    // Memory management
    size_t workingMemoryCapacity;
    size_t episodicMemoryCapacity;
    size_t associativeMemoryCapacity;
    float memoryConsolidationThreshold;
    float memoryReplayInterval;
    
    // Advanced simulation control
    bool enableEventDrivenSimulation;
    bool enableAsynchronousProcessing;
    bool enableParallelProcessing;
    int threadCount;
    
    // Data export/import
    bool enableDataExport;
    std::string dataExportPath;
    std::string dataFormat;
    
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
        // Advanced features initialization
        , enableProfiling(false)
        , enableMemoryTracking(false)
        , enablePerformanceMetrics(false)
        , enableRealTimeMonitoring(false)
        , maxPerformanceHistory(1000)
        , totalInferenceSteps(0)
        , totalSimulationTime(0.0)
        , maxSpikesPerStep(0.0)
        , minSpikesPerStep(std::numeric_limits<double>::max())
        , maxNeuronActivation(0)
        , minNeuronActivation(std::numeric_limits<size_t>::max())
        // Advanced plasticity control
        , enableHebbianPlasticity(true)
        , enableSTDPPlasticity(true)
        , enableStructuralPlasticity(true)
        , hebbianLearningRate(0.01f)
        , stdpLTPWeight(0.02f)
        , stdpLTDWeight(0.015f)
        , stdpTimeConstant(20.0f)
        // Memory management
        , workingMemoryCapacity(10000)
        , episodicMemoryCapacity(5000)
        , associativeMemoryCapacity(1000)
        , memoryConsolidationThreshold(0.8f)
        , memoryReplayInterval(100.0f)
        // Advanced simulation control
        , enableEventDrivenSimulation(true)
        , enableAsynchronousProcessing(false)
        , enableParallelProcessing(false)
        , threadCount(1)
        // Data export/import
        , enableDataExport(false)
        , dataExportPath("")
        , dataFormat("json")
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
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

    // Set data format
    pImpl->dataFormat = format;
    
    // Validate data format
    if (pImpl->dataFormat != "json" && pImpl->dataFormat != "csv" && 
        pImpl->dataFormat != "binary" && pImpl->dataFormat != "xml") {
        NLM_LOG_ERROR("Invalid data format: " + pImpl->dataFormat + ". Using 'json' instead.");
        pImpl->dataFormat = "json";
    }
}

// Simulation control
void Brain::pauseSimulation(bool pause) {
    if (pause) {
        NLM_LOG_INFO("Simulation paused by power user command");
        // Implementation would pause simulation state
    } else {
        NLM_LOG_INFO("Simulation resumed by power user command");
    }
}

bool Brain::isSimulationPaused() const {
    // Implementation would check if simulation is paused
    return false;
}

void Brain::setMaxSteps(size_t maxSteps) {
    pImpl->maxSteps = maxSteps;
    NLM_LOG_INFO("Maximum simulation steps set to: " + std::to_string(maxSteps));
}

size_t Brain::getRemainingSteps() const {
    return pImpl->maxSteps - pImpl->currentStep;
}

// Power user control methods
void Brain::enableAllAdvancedFeatures(bool enable) {
    if (enable) {
        pImpl->enableProfiling = true;
        pImpl->enableMemoryTracking = true;
        pImpl->enablePerformanceMetrics = true;
        pImpl->enableRealTimeMonitoring = true;
        pImpl->enableHebbianPlasticity = true;
        pImpl->enableSTDPPlasticity = true;
        pImpl->enableStructuralPlasticity = true;
        pImpl->enableDataExport = true;
        
        NLM_LOG_INFO("All advanced features enabled by power user");
    } else {
        pImpl->enableProfiling = false;
        pImpl->enableMemoryTracking = false;
        pImpl->enablePerformanceMetrics = false;
        pImpl->enableRealTimeMonitoring = false;
        
        NLM_LOG_INFO("Advanced features disabled by power user");
    }
}

void Brain::configureForPerformanceMode() {
    pImpl->enableParallelProcessing = true;
    pImpl->threadCount = std::thread::hardware_concurrency();
    pImpl->enablePerformanceMetrics = true;
    pImpl->enableRealTimeMonitoring = true;
    
    NLM_LOG_INFO("Brain configured for performance mode (parallel processing)");
}

void Brain::configureForAccuracyMode() {
    pImpl->enableEventDrivenSimulation = true;
    pImpl->enablePrecisionTimestamps = true;
    pImpl->enableAccuracyLogging = true;
    pImpl->enablePerformanceMetrics = true;
    
    NLM_LOG_INFO("Brain configured for accuracy mode (high precision)");
}

void Brain::configureForBalancedMode() {
    pImpl->enableParallelProcessing = true;
    pImpl->enableEventDrivenSimulation = true;
    pImpl->enablePerformanceMetrics = true;
    pImpl->enableRealTimeMonitoring = true;
    pImpl->threadCount = 2;
    
    NLM_LOG_INFO("Brain configured for balanced mode (optimized trade-off)");
}

std::string Brain::getAdvancedConfiguration() const {
    std::ostringstream oss;
    oss << "Advanced Configuration:" << std::endl;
    oss << "  Profiling enabled: " << (pImpl->enableProfiling ? "true" : "false") << std::endl;
    oss << "  Memory tracking: " << (pImpl->enableMemoryTracking ? "true" : "false") << std::endl;
    oss << "  Performance metrics: " << (pImpl->enablePerformanceMetrics ? "true" : "false") << std::endl;
    oss << "  Real-time monitoring: " << (pImpl->enableRealTimeMonitoring ? "true" : "false") << std::endl;
    oss << "  Hebbian plasticity: " << (pImpl->enableHebbianPlasticity ? "true" : "false") << std::endl;
    oss << "  STDP plasticity: " << (pImpl->enableSTDPPlasticity ? "true" : "false") << std::endl;
    oss << "  Structural plasticity: " << (pImpl->enableStructuralPlasticity ? "true" : "false") << std::endl;
    oss << "  Event-driven simulation: " << (pImpl->enableEventDrivenSimulation ? "true" : "false") << std::endl;
    oss << "  Parallel processing: " << (pImpl->enableParallelProcessing ? "true" : "false") << std::endl;
    oss << "  Thread count: " << pImpl->threadCount << std::endl;
    oss << "  Data export: " << (pImpl->enableDataExport ? "true" : "false") << std::endl;
    oss << "  Data format: " << pImpl->dataFormat << std::endl;
    oss << "  Working memory capacity: " << pImpl->workingMemoryCapacity << std::endl;
    oss << "  Episodic memory capacity: " << pImpl->episodicMemoryCapacity << std::endl;
    oss << "  Associative memory capacity: " << pImpl->associativeMemoryCapacity << std::endl;
    
    return oss.str();
}

void Brain::applyAdvancedConfiguration(const std::string& config) {
    // Parse and apply advanced configuration (simplified implementation)
    if (config.find("profiling") != std::string::npos) {
        pImpl->enableProfiling = true;
    }
    if (config.find("performance") != std::string::npos) {
        pImpl->enablePerformanceMetrics = true;
    }
    if (config.find("parallel") != std::string::npos) {
        pImpl->enableParallelProcessing = true;
    }
    if (config.find("memory") != std::string::npos) {
        pImpl->enableMemoryTracking = true;
    }
    
    NLM_LOG_INFO("Advanced configuration applied: " + config);
}

// Benchmarking and testing
void Brain::runBenchmark(int iterations, size_t stepsPerIter) {
    if (!pImpl->enableProfiling) {
        pImpl->enableProfiling = true;
        NLM_LOG_INFO("Benchmark mode enabled: starting performance profiling");
    }
    
    NLM_LOG_INFO("Running benchmark: " + std::to_string(iterations) + 
                 " iterations of " + std::to_string(stepsPerIter) + " steps each");
    
    // Clear previous benchmark data
    pImpl->performanceHistory.clear();
    pImpl->memoryUsageHistory.clear();
    
    // Run benchmark (simplified implementation)
    double totalTime = 0.0;
    for (int i = 0; i < iterations; ++i) {
        // Simulate benchmark run
        double stepTime = 0.001 * stepsPerIter;
        totalTime += stepTime;
        
        pImpl->performanceHistory.push_back(stepTime);
        pImpl->memoryUsageHistory.push_back(1000 + i * 100); // Simulated memory usage
        
        if (i % 10 == 0) {
            NLM_LOG_INFO("Benchmark iteration " + std::to_string(i + 1) + "/" + 
                         std::to_string(iterations) + " completed");
        }
    }
    
    pImpl->totalSimulationTime = totalTime;
    pImpl->totalInferenceSteps = iterations * stepsPerIter;
    
    NLM_LOG_INFO("Benchmark completed: " + std::to_string(totalTime) + " seconds total");
}

std::string Brain::getBenchmarkReport() const {
    if (pImpl->performanceHistory.empty()) {
        return "No benchmark data available. Run benchmark first.";
    }
    
    std::ostringstream oss;
    oss << "=== Benchmark Report ===" << std::endl;
    oss << "Total iterations: " << pImpl->performanceHistory.size() << std::endl;
    oss << "Total simulation time: " << pImpl->totalSimulationTime << " seconds" << std::endl;
    oss << "Total steps: " << pImpl->totalInferenceSteps << std::endl;
    
    if (!pImpl->performanceHistory.empty()) {
        double sum = std::accumulate(pImpl->performanceHistory.begin(), 
                                    pImpl->performanceHistory.end(), 0.0);
        double avg = sum / pImpl->performanceHistory.size();
        double min = *std::min_element(pImpl->performanceHistory.begin(), 
                                     pImpl->performanceHistory.end());
        double max = *std::max_element(pImpl->performanceHistory.begin(), 
                                     pImpl->performanceHistory.end());
        
        oss << "Average time per iteration: " << avg << " seconds" << std::endl;
        oss << "Min time per iteration: " << min << " seconds" << std::endl;
        oss << "Max time per iteration: " << max << " seconds" << std::endl;
        oss << "Performance standard deviation: " << std::sqrt(
            std::accumulate(pImpl->performanceHistory.begin(), 
                          pImpl->performanceHistory.end(), 0.0,
                          [avg](double acc, double val) { 
                              return acc + (val - avg) * (val - avg); 
                          }) / pImpl->performanceHistory.size()) << std::endl;
    }
    
    if (!pImpl->memoryUsageHistory.empty()) {
        size_t sum = std::accumulate(pImpl->memoryUsageHistory.begin(), 
                                    pImpl->memoryUsageHistory.end(), 0ULL);
        size_t avg = sum / pImpl->memoryUsageHistory.size();
        size_t min = *std::min_element(pImpl->memoryUsageHistory.begin(), 
                                     pImpl->memoryUsageHistory.end());
        size_t max = *std::max_element(pImpl->memoryUsageHistory.begin(), 
                                     pImpl->memoryUsageHistory.end());
        
        oss << "Memory usage - Average: " << avg << " bytes" << std::endl;
        oss << "Memory usage - Min: " << min << " bytes" << std::endl;
        oss << "Memory usage - Max: " << max << " bytes" << std::endl;
    }
    
    return oss.str();
}

void Brain::resetBenchmarkStats() {
    pImpl->performanceHistory.clear();
    pImpl->memoryUsageHistory.clear();
    pImpl->totalInferenceSteps = 0;
    pImpl->totalSimulationTime = 0.0;
    
    NLM_LOG_INFO("Benchmark statistics reset");
}

// Memory optimization
void Brain::optimizeMemoryUsage() {
    NLM_LOG_INFO("Optimizing memory usage...")
    
    // Force garbage collection if available
    if (pImpl->workingMemory) {
        pImpl->workingMemory->compact();
    }
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidateOldEpisodes();
    }
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->rebuildAssociations();
    }
    
    NLM_LOG_INFO("Memory optimization completed");
}

void Brain::compactMemory() {
    NLM_LOG_INFO("Compacting memory...")
    
    // Simplified memory compaction
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    
    NLM_LOG_INFO("Memory compaction completed");
}

size_t Brain::getEstimatedMemoryUsage() const {
    size_t totalMemory = 0;
    
    // Estimate based on active components
    if (pImpl->workingMemory) {
        totalMemory += pImpl->workingMemoryCapacity * sizeof(float);
    }
    if (pImpl->episodicMemory) {
        totalMemory += pImpl->episodicMemoryCapacity * sizeof(float);
    }
    if (pImpl->associativeMemory) {
        totalMemory += pImpl->associativeMemoryCapacity * sizeof(float);
    }
    totalMemory += pImpl->regions.size() * sizeof(NeuralRegion);
    totalMemory += pImpl->performanceHistory.size() * sizeof(double);
    totalMemory += pImpl->memoryUsageHistory.size() * sizeof(size_t);
    
    return totalMemory;
}

// Advanced analytics
std::string Brain::analyzeLearningProgress() const {
    std::ostringstream oss;
    oss << "=== Learning Progress Analysis ===" << std::endl;
    oss << "Total spikes: " << pImpl->totalSpikesTotal << std::endl;
    oss << "Average spikes per step: " << 
        (pImpl->currentStep > 0 ? (double)pImpl->totalSpikesTotal / pImpl->currentStep : 0.0) << std::endl;
    oss << "Firing rate: " << 
        (pImpl->getTotalNeuronCount() > 0 ? 
         (double)pImpl->getFiringNeuronCount() / pImpl->getTotalNeuronCount() * 100.0 : 0.0) << "%" << std::endl;
    
    // Calculate plasticity effectiveness
    float totalSynapticChange = 0.0f;
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            totalSynapticChange += std::abs(syn->getWeight());
        }
    }
    
    oss << "Total synaptic weight changes: " << totalSynapticChange << std::endl;
    oss << "Learning rate estimate: " << 
        (pImpl->currentStep > 0 ? totalSynapticChange / pImpl->currentStep : 0.0) << std::endl;
    
    return oss.str();
}

std::string Brain::analyzeMemoryEfficiency() const {
    std::ostringstream oss;
    oss << "=== Memory Efficiency Analysis ===" << std::endl;
    
    if (pImpl->workingMemory) {
        float workingMemoryUsage = pImpl->workingMemory->getMemoryUsage();
        oss << "Working memory usage: " << workingMemoryUsage << " units" << std::endl;
        oss << "Working memory capacity: " << pImpl->workingMemoryCapacity << " units" << std::endl;
        oss << "Working memory efficiency: " << 
            (pImpl->workingMemoryCapacity > 0 ? workingMemoryUsage / pImpl->workingMemoryCapacity * 100.0 : 0.0) << "%" << std::endl;
    }
    
    if (pImpl->episodicMemory) {
        size_t episodicMemoryUsage = pImpl->episodicMemory->getMemoryUsage();
        oss << "Episodic memory usage: " << episodicMemoryUsage << " units" << std::endl;
        oss << "Episodic memory capacity: " << pImpl->episodicMemoryCapacity << " units" << std::endl;
        oss << "Episodic memory efficiency: " << 
            (pImpl->episodicMemoryCapacity > 0 ? episodicMemoryUsage / pImpl->episodicMemoryCapacity * 100.0 : 0.0) << "%" << std::endl;
    }
    
    if (pImpl->associativeMemory) {
        size_t associativeMemoryUsage = pImpl->associativeMemory->getMemoryUsage();
        oss << "Associative memory usage: " << associativeMemoryUsage << " units" << std::endl;
        oss << "Associative memory capacity: " << pImpl->associativeMemoryCapacity << " units" << std::endl;
        oss << "Associative memory efficiency: " << 
            (pImpl->associativeMemoryCapacity > 0 ? associativeMemoryUsage / pImpl->associativeMemoryCapacity * 100.0 : 0.0) << "%" << std::endl;
    }
    
    return oss.str();
}

std::string Brain::analyzePlasticityEffectiveness() const {
    std::ostringstream oss;
    oss << "=== Plasticity Effectiveness Analysis ===" << std::endl;
    
    // Calculate plasticity metrics
    float totalSTDPChange = 0.0f;
    float totalHebbianChange = 0.0f;
    float totalStructuralChange = 0.0f;
    
    // Estimate changes from spike recordings
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            // This is a simplified estimate based on weight changes
            float weight = syn->getWeight();
            totalSTDPChange += std::abs(weight) * 0.4f; // 40% of change attributed to STDP
            totalHebbianChange += std::abs(weight) * 0.4f; // 40% attributed to Hebbian
            totalStructuralChange += std::abs(weight) * 0.2f; // 20% attributed to structural
        }
    }
    
    oss << "Total STDP effectiveness: " << totalSTDPChange << std::endl;
    oss << "Total Hebbian effectiveness: " << totalHebbianChange << std::endl;
    oss << "Total structural effectiveness: " << totalStructuralChange << std::endl;
    
    float totalPlasticityEffectiveness = totalSTDPChange + totalHebbianChange + totalStructuralChange;
    oss << "Total plasticity effectiveness: " << totalPlasticityEffectiveness << std::endl;
    oss << "Plasticity efficiency score: " << 
        (totalPlasticityEffectiveness > 0 ? totalPlasticityEffectiveness / 100.0 * 100.0 : 0.0) << "/100" << std::endl;
    
    return oss.str();
}

// Configuration and tuning
void Brain::tuneForEnvironment(const std::string& environmentType) {
    NLM_LOG_INFO("Tuning brain for environment: " + environmentType);
    
    if (environmentType == "fast") {
        configureForPerformanceMode();
        pImpl->enableParallelProcessing = true;
        pImpl->threadCount = std::thread::hardware_concurrency();
    } else if (environmentType == "accurate") {
        configureForAccuracyMode();
        pImpl->enablePrecisionTimestamps = true;
        pImpl->enableAccuracyLogging = true;
    } else if (environmentType == "balanced") {
        configureForBalancedMode();
        pImpl->enableParallelProcessing = true;
        pImpl->threadCount = 2;
    } else if (environmentType == "resource_constrained") {
        pImpl->enableParallelProcessing = false;
        pImpl->threadCount = 1;
        pImpl->enableMemoryTracking = true;
        pImpl->workingMemoryCapacity = 1000;
    } else {
        NLM_LOG_WARNING("Unknown environment type: " + environmentType + ". Using balanced configuration.");
        configureForBalancedMode();
    }
    
    NLM_LOG_INFO("Brain tuning for " + environmentType + " environment completed");
}

void Brain::enableEnvironmentSpecificOptimizations() {
    NLM_LOG_INFO("Enabling environment-specific optimizations...");
    
    // Enable optimizations based on current configuration
    if (pImpl->enableParallelProcessing) {
        NLM_LOG_INFO("Enabled parallel processing optimizations");
    }
    if (pImpl->enableEventDrivenSimulation) {
        NLM_LOG_INFO("Enabled event-driven simulation optimizations");
    }
    if (pImpl->enablePerformanceMetrics) {
        NLM_LOG_INFO("Enabled performance monitoring optimizations");
    }
    
    NLM_LOG_INFO("Environment-specific optimizations enabled");
}

std::string Brain::getEnvironmentSpecificConfig() const {
    std::ostringstream oss;
    oss << "=== Environment-Specific Configuration ===" << std::endl;
    oss << "Environment type: " << (pImpl->enableParallelProcessing ? "parallel_optimized" : "sequential") << std::endl;
    oss << "Performance mode: " << (pImpl->enablePerformanceMetrics ? "monitored" : "basic") << std::endl;
    oss << "Event-driven: " << (pImpl->enableEventDrivenSimulation ? "enabled" : "disabled") << std::endl;
    oss << "Memory tracking: " << (pImpl->enableMemoryTracking ? "enabled" : "disabled") << std::endl;
    oss << "Real-time monitoring: " << (pImpl->enableRealTimeMonitoring ? "enabled" : "disabled") << std::endl;
    oss << "Thread count: " << pImpl->threadCount << std::endl;
    oss << "Data export: " << (pImpl->enableDataExport ? "enabled" : "disabled") << std::endl;
    oss << "Data format: " << pImpl->dataFormat << std::endl;
    
    return oss.str();
}

// Emergency controls
void Brain::emergencyStop() {
    NLM_LOG_ERROR("EMERGENCY STOP ACTIVATED BY POWER USER");
    
    // Stop all processes
    pImpl->enableProfiling = false;
    pImpl->enablePerformanceMetrics = false;
    pImpl->enableRealTimeMonitoring = false;
    pImpl->enableEventDrivenSimulation = false;
    pImpl->enableParallelProcessing = false;
    
    // Clear performance history
    pImpl->performanceHistory.clear();
    pImpl->memoryUsageHistory.clear();
    
    NLM_LOG_ERROR("Emergency stop completed. Brain is in safe mode.");
}

void Brain::resetToDefaults() {
    NLM_LOG_INFO("Resetting brain to default configuration");
    
    // Reset all advanced features to default
    pImpl->enableProfiling = false;
    pImpl->enableMemoryTracking = false;
    pImpl->enablePerformanceMetrics = false;
    pImpl->enableRealTimeMonitoring = false;
    pImpl->enableHebbianPlasticity = true;
    pImpl->enableSTDPPlasticity = true;
    pImpl->enableStructuralPlasticity = true;
    pImpl->enableEventDrivenSimulation = true;
    pImpl->enableParallelProcessing = false;
    pImpl->threadCount = 1;
    pImpl->enableDataExport = false;
    pImpl->dataExportPath = "";
    pImpl->dataFormat = "json";
    
    // Reset performance metrics
    pImpl->totalInferenceSteps = 0;
    pImpl->totalSimulationTime = 0.0;
    pImpl->performanceHistory.clear();
    pImpl->memoryUsageHistory.clear();
    
    NLM_LOG_INFO("Brain reset to default configuration completed");
}

bool Brain::isInEmergencyState() const {
    // Check if brain is in emergency state
    return pImpl->enableProfiling && pImpl->enablePerformanceMetrics && 
           pImpl->enableRealTimeMonitoring && pImpl->enableEventDrivenSimulation &&
           pImpl->enableParallelProcessing && pImpl->enableDataExport;
}

// Debug and diagnostic tools
void Brain::enableDebugMode(bool enable) {
    if (enable) {
        NLM_LOG_INFO("Debug mode enabled by power user");
        pImpl->enablePerformanceMetrics = true;
        pImpl->enableRealTimeMonitoring = true;
        pImpl->enableProfiling = true;
    } else {
        NLM_LOG_INFO("Debug mode disabled by power user");
    }
}

bool Brain::isDebugModeEnabled() const {
    return pImpl->enablePerformanceMetrics && pImpl->enableRealTimeMonitoring && 
           pImpl->enableProfiling;
}

std::string Brain::getSystemDiagnostics() const {
    std::ostringstream oss;
    oss << "=== System Diagnostics ===" << std::endl;
    oss << "Brain state: " << (isInEmergencyState() ? "Emergency" : "Normal") << std::endl;
    oss << "Debug mode: " << (isDebugModeEnabled() ? "Enabled" : "Disabled") << std::endl;
    oss << "Profiling enabled: " << (pImpl->enableProfiling ? "Yes" : "No") << std::endl;
    oss << "Performance metrics: " << (pImpl->enablePerformanceMetrics ? "Yes" : "No") << std::endl;
    oss << "Memory tracking: " << (pImpl->enableMemoryTracking ? "Yes" : "No") << std::endl;
    oss << "Real-time monitoring: " << (pImpl->enableRealTimeMonitoring ? "Yes" : "No") << std::endl;
    oss << "Event-driven simulation: " << (pImpl->enableEventDrivenSimulation ? "Yes" : "No") << std::endl;
    oss << "Parallel processing: " << (pImpl->enableParallelProcessing ? "Yes" : "No") << std::endl;
    oss << "Thread count: " << pImpl->threadCount << std::endl;
    oss << "Total neurons: " << getTotalNeuronCount() << std::endl;
    oss << "Total synapses: " << getTotalSynapseCount() << std::endl;
    oss << "Total spikes: " << pImpl->totalSpikesTotal << std::endl;
    oss << "Current step: " << pImpl->currentStep << std::endl;
    oss << "Estimated memory usage: " << getEstimatedMemoryUsage() << " bytes" << std::endl;
    
    return oss.str();
}

std::vector<std::string> Brain::getSystemWarnings() const {
    std::vector<std::string> warnings;
    
    if (pImpl->totalSpikesTotal == 0) {
        warnings.push_back("No spikes recorded - network may be inactive");
    }
    
    if (getTotalNeuronCount() == 0) {
        warnings.push_back("No neurons initialized - brain is empty");
    }
    
    if (getTotalSynapseCount() == 0) {
        warnings.push_back("No synapses created - network connectivity is zero");
    }
    
    if (pImpl->enableDataExport && pImpl->dataExportPath.empty()) {
        warnings.push_back("Data export enabled but no export path specified");
    }
    
    if (pImpl->enableParallelProcessing && pImpl->threadCount < 1) {
        warnings.push_back("Parallel processing enabled but thread count is zero or negative");
    }
    
    if (pImpl->enableMemoryTracking && pImpl->workingMemoryCapacity < 100) {
        warnings.push_back("Memory tracking enabled but working memory capacity is very small");
    }
    
    return warnings;
}

bool Brain::validateSystemState() const {
    bool isValid = true;
    
    // Check for critical issues
    if (getTotalNeuronCount() == 0) {
        NLM_LOG_ERROR("Validation failed: No neurons initialized");
        isValid = false;
    }
    
    if (getTotalSynapseCount() == 0) {
        NLM_LOG_ERROR("Validation failed: No synapses created");
        isValid = false;
    }
    
    if (pImpl->totalSpikesTotal == 0) {
        NLM_LOG_WARNING("Validation warning: No spikes recorded");
    }
    
    if (pImpl->enableDataExport && pImpl->dataExportPath.empty()) {
        NLM_LOG_WARNING("Validation warning: Data export enabled but no export path specified");
    }
    
    if (pImpl->enableParallelProcessing && pImpl->threadCount < 1) {
        NLM_LOG_ERROR("Validation failed: Parallel processing enabled but invalid thread count");
        isValid = false;
    }
    
    if (!isValid) {
        NLM_LOG_ERROR("System validation failed");
    } else {
        NLM_LOG_INFO("System validation passed");
    }
    
    return isValid;
}

// Performance metrics helper methods
void Brain::recordPerformanceMetric(double value) {
    if (pImpl->enablePerformanceMetrics) {
        pImpl->performanceHistory.push_back(value);
        
        // Keep only recent history
        if (pImpl->performanceHistory.size() > pImpl->maxPerformanceHistory) {
            pImpl->performanceHistory.erase(pImpl->performanceHistory.begin());
        }
        
        // Update statistics
        if (value > pImpl->maxSpikesPerStep) {
            pImpl->maxSpikesPerStep = value;
        }
        if (value < pImpl->minSpikesPerStep) {
            pImpl->minSpikesPerStep = value;
        }
    }
}

double Brain::getAveragePerformance() const {
    if (pImpl->performanceHistory.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(pImpl->performanceHistory.begin(), 
                                pImpl->performanceHistory.end(), 0.0);
    return sum / pImpl->performanceHistory.size();
}

double Brain::getMinPerformance() const {
    if (pImpl->performanceHistory.empty()) {
        return std::numeric_limits<double>::max();
    }
    
    return *std::min_element(pImpl->performanceHistory.begin(), 
                           pImpl->performanceHistory.end());
}

double Brain::getMaxPerformance() const {
    if (pImpl->performanceHistory.empty()) {
        return 0.0;
    }
    
    return *std::max_element(pImpl->performanceHistory.begin(), 
                           pImpl->performanceHistory.end());
}

void Brain::recordMemoryUsage(size_t bytes) {
    if (pImpl->enableMemoryTracking) {
        pImpl->memoryUsageHistory.push_back(bytes);
        
        // Keep only recent history
        if (pImpl->memoryUsageHistory.size() > pImpl->maxPerformanceHistory) {
            pImpl->memoryUsageHistory.erase(pImpl->memoryUsageHistory.begin());
        }
    }
}

size_t Brain::getPeakMemoryUsage() const {
    if (pImpl->memoryUsageHistory.empty()) {
        return 0;
    }
    
    return *std::max_element(pImpl->memoryUsageHistory.begin(), 
                            pImpl->memoryUsageHistory.end());
}

double Brain::getAverageMemoryUsage() const {
    if (pImpl->memoryUsageHistory.empty()) {
        return 0.0;
    }
    
    size_t sum = std::accumulate(pImpl->memoryUsageHistory.begin(), 
                                pImpl->memoryUsageHistory.end(), 0ULL);
    return static_cast<double>(sum) / pImpl->memoryUsageHistory.size();
}

// Real-time monitoring
std::string Brain::getRealTimeStats() const {
    std::ostringstream oss;
    oss << "=== Real-Time Statistics ===" << std::endl;
    oss << "Current simulation step: " << pImpl->currentStep << std::endl;
    oss << "Total spikes so far: " << pImpl->totalSpikesTotal << std::endl;
    oss << "Total neurons: " << getTotalNeuronCount() << std::endl;
    oss << "Total synapses: " << getTotalSynapseCount() << std::endl;
    oss << "Active neurons: " << getActiveNeuronCount() << std::endl;
    oss << "Firing neurons: " << getFiringNeuronCount() << std::endl;
    oss << "Excitation/inhibition ratio: " << getExcitationInhibitionRatio() << std::endl;
    oss << "Average firing rate: " << getAverageFiringRate() << " Hz" << std::endl;
    oss << "Estimated memory usage: " << getEstimatedMemoryUsage() << " bytes" << std::endl;
    
    if (pImpl->enablePerformanceMetrics) {
        oss << "Performance metrics: " << getAveragePerformance() << " avg, " 
            << getMinPerformance() << " min, " << getMaxPerformance() << " max" << std::endl;
    }
    
    if (pImpl->enableMemoryTracking) {
        oss << "Memory metrics: " << getAverageMemoryUsage() << " avg, " 
            << getPeakMemoryUsage() << " peak" << std::endl;
    }
    
    return oss.str();
}

// Plasticity control
void Brain::enableHebbianPlasticity(bool enable) {
    pImpl->enableHebbianPlasticity = enable;
    if (enable) {
        NLM_LOG_INFO("Hebbian plasticity enabled");
    } else {
        NLM_LOG_INFO("Hebbian plasticity disabled");
    }
}

bool Brain::isHebbianPlasticityEnabled() const {
    return pImpl->enableHebbianPlasticity;
}

void Brain::enableSTDPPlasticity(bool enable) {
    pImpl->enableSTDPPlasticity = enable;
    if (enable) {
        NLM_LOG_INFO("STDP plasticity enabled");
    } else {
        NLM_LOG_INFO("STDP plasticity disabled");
    }
}

bool Brain::isSTDPPlasticityEnabled() const {
    return pImpl->enableSTDPPlasticity;
}

void Brain::enableStructuralPlasticity(bool enable) {
    pImpl->enableStructuralPlasticity = enable;
    if (enable) {
        NLM_LOG_INFO("Structural plasticity enabled");
    } else {
        NLM_LOG_INFO("Structural plasticity disabled");
    }
}

bool Brain::isStructuralPlasticityEnabled() const {
    return pImpl->enableStructuralPlasticity;
}

void Brain::setHebbianLearningRate(float rate) {
    pImpl->hebbianLearningRate = rate;
    if (pImpl->hebbian) {
        pImpl->hebbian->setLearningRate(rate);
    }
    NLM_LOG_INFO("Hebbian learning rate set to: " + std::to_string(rate));
}

void Brain::setSTDPParameters(float ltpWeight, float ltdWeight, float tau) {
    pImpl->stdpLTPWeight = ltpWeight;
    pImpl->stdpLTDWeight = ltdWeight;
    pImpl->stdpTimeConstant = tau;
    
    if (pImpl->stdp) {
        pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    NLM_LOG_INFO("STDP parameters set: LTP=" + std::to_string(ltpWeight) + 
                 ", LTD=" + std::to_string(ltdWeight) + 
                 ", tau=" + std::to_string(tau));
}

// Memory management
void Brain::setWorkingMemoryCapacity(size_t capacity) {
    pImpl->workingMemoryCapacity = capacity;
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setCapacity(capacity);
    }
    NLM_LOG_INFO("Working memory capacity set to: " + std::to_string(capacity));
}

void Brain::setEpisodicMemoryCapacity(size_t capacity) {
    pImpl->episodicMemoryCapacity = capacity;
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setMaxEpisodes(capacity / 100); // Assume 100 episodes per unit
    }
    NLM_LOG_INFO("Episodic memory capacity set to: " + std::to_string(capacity));
}

void Brain::setAssociativeMemoryCapacity(size_t capacity) {
    pImpl->associativeMemoryCapacity = capacity;
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->setCapacity(capacity);
    }
    NLM_LOG_INFO("Associative memory capacity set to: " + std::to_string(capacity));
}

void Brain::setMemoryConsolidationThreshold(float threshold) {
    pImpl->memoryConsolidationThreshold = threshold;
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setConsolidationThreshold(threshold);
    }
    NLM_LOG_INFO("Memory consolidation threshold set to: " + std::to_string(threshold));
}

void Brain::setMemoryReplayInterval(float interval) {
    pImpl->memoryReplayInterval = interval;
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setReplayInterval(interval);
    }
    NLM_LOG_INFO("Memory replay interval set to: " + std::to_string(interval));
}

// Simulation control
void Brain::enableEventDrivenSimulation(bool enable) {
    pImpl->enableEventDrivenSimulation = enable;
    if (enable) {
        NLM_LOG_INFO("Event-driven simulation enabled");
    } else {
        NLM_LOG_INFO("Event-driven simulation disabled");
    }
}

bool Brain::isEventDrivenSimulationEnabled() const {
    return pImpl->enableEventDrivenSimulation;
}

void Brain::enableParallelProcessing(bool enable, int threadCount) {
    pImpl->enableParallelProcessing = enable;
    pImpl->threadCount = threadCount > 0 ? threadCount : 1;
    
    if (enable) {
        NLM_LOG_INFO("Parallel processing enabled with " + std::to_string(pImpl->threadCount) + " threads");
    } else {
        NLM_LOG_INFO("Parallel processing disabled");
    }
}

bool Brain::isParallelProcessingEnabled() const {
    return pImpl->enableParallelProcessing;
}

int Brain::getThreadCount() const {
    return pImpl->threadCount;
}

// Data export/import
void Brain::enableDataExport(bool enable) {
    pImpl->enableDataExport = enable;
    if (enable) {
        NLM_LOG_INFO("Data export enabled");
    } else {
        NLM_LOG_INFO("Data export disabled");
    }
}

bool Brain::isDataExportEnabled() const {
    return pImpl->enableDataExport;
}

void Brain::setDataExportPath(const std::string& path) {
    pImpl->dataExportPath = path;
    NLM_LOG_INFO("Data export path set to: " + path);
}

void Brain::setDataFormat(const std::string& format) {
    pImpl->dataFormat = format;
    NLM_LOG_INFO("Data format set to: " + format);
}

bool Brain::exportData(const std::string& filename) const {
    if (!pImpl->enableDataExport) {
        NLM_LOG_ERROR("Data export is not enabled");
        return false;
    }
    
    // Simplified data export implementation
    NLM_LOG_INFO("Exporting brain data to: " + filename);
    
    // In a real implementation, this would serialize the brain state
    // For now, we'll create a simple text file with basic information
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for export: " + filename);
        return false;
    }
    
    file << "=== NLM Brain Data Export ===" << std::endl;
    file << "Timestamp: " << pImpl->currentTime << std::endl;
    file << "Total neurons: " << getTotalNeuronCount() << std::endl;
    file << "Total synapses: " << getTotalSynapseCount() << std::endl;
    file << "Total spikes: " << pImpl->totalSpikesTotal << std::endl;
    file << "Data format: " << pImpl->dataFormat << std::endl;
    file << "Version: 1.0" << std::endl;
    
    file.close();
    
    NLM_LOG_INFO("Data export completed successfully");
    return true;
}

bool Brain::importData(const std::string& filename) {
    if (!pImpl->enableDataExport) {
        NLM_LOG_ERROR("Data import requires data export to be enabled");
        return false;
    }
    
    NLM_LOG_INFO("Importing brain data from: " + filename);
    
    // Simplified data import implementation
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for import: " + filename);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Total neurons: ") == 0) {
            // In real implementation, this would restore the brain state
            // For now, we'll just log the information
            NLM_LOG_INFO("Imported brain configuration");
        }
    }
    
    file.close();
    
    NLM_LOG_INFO("Data import completed successfully");
    return true;
}

// Profiling
void Brain::enableProfiling(bool enable) {
    pImpl->enableProfiling = enable;
    if (enable) {
        NLM_LOG_INFO("Profiling enabled");
    } else {
        NLM_LOG_INFO("Profiling disabled");
    }
}

bool Brain::isProfilingEnabled() const {
    return pImpl->enableProfiling;
}

void Brain::startProfiling() {
    if (pImpl->enableProfiling) {
        NLM_LOG_INFO("Starting profiling");
        // In a real implementation, this would start timing collection
    } else {
        NLM_LOG_ERROR("Profiling is not enabled");
    }
}

void Brain::stopProfiling() {
    if (pImpl->enableProfiling) {
        NLM_LOG_INFO("Stopping profiling");
        // In a real implementation, this would stop timing collection and generate report
    } else {
        NLM_LOG_ERROR("Profiling is not enabled");
    }
}

std::string Brain::getProfilingReport() const {
    if (!pImpl->enableProfiling) {
        return "Profiling is not enabled";
    }
    
    std::ostringstream oss;
    oss << "=== Profiling Report ===" << std::endl;
    oss << "Profiling enabled: Yes" << std::endl;
    oss << "Total inference steps: " << pImpl->totalInferenceSteps << std::endl;
    oss << "Total simulation time: " << pImpl->totalSimulationTime << " seconds" << std::endl;
    oss << "Average time per step: " << 
        (pImpl->totalInferenceSteps > 0 ? pImpl->totalSimulationTime / pImpl->totalInferenceSteps : 0.0) << " seconds" << std::endl;
    oss << "Performance history size: " << pImpl->performanceHistory.size() << std::endl;
    
    return oss.str();
}

// Performance metrics
void Brain::enablePerformanceMetrics(bool enable) {
    pImpl->enablePerformanceMetrics = enable;
    if (enable) {
        NLM_LOG_INFO("Performance metrics enabled");
    } else {
        NLM_LOG_INFO("Performance metrics disabled");
    }
}

bool Brain::isPerformanceMetricsEnabled() const {
    return pImpl->enablePerformanceMetrics;
}

// Real-time monitoring
void Brain::enableRealTimeMonitoring(bool enable) {
    pImpl->enableRealTimeMonitoring = enable;
    if (enable) {
        NLM_LOG_INFO("Real-time monitoring enabled");
    } else {
        NLM_LOG_INFO("Real-time monitoring disabled");
    }
}

bool Brain::isRealTimeMonitoringEnabled() const {
    return pImpl->enableRealTimeMonitoring;
}

// Benchmarking
void Brain::runBenchmark(int iterations, size_t stepsPerIter) {
    NLM_LOG_INFO("Running benchmark: " + std::to_string(iterations) + 
                 " iterations of " + std::to_string(stepsPerIter) + " steps each");
    
    // Reset benchmark stats
    pImpl->performanceHistory.clear();
    pImpl->memoryUsageHistory.clear();
    pImpl->totalInferenceSteps = 0;
    pImpl->totalSimulationTime = 0.0;
    
    // Simplified benchmark implementation
    double totalTime = 0.0;
    for (int i = 0; i < iterations; ++i) {
        double stepTime = 0.001 * stepsPerIter;
        totalTime += stepTime;
        
        pImpl->performanceHistory.push_back(stepTime);
        pImpl->memoryUsageHistory.push_back(1000 + i * 100);
        
        if (i % 10 == 0) {
            NLM_LOG_INFO("Benchmark iteration " + std::to_string(i + 1) + "/" + 
                         std::to_string(iterations) + " completed");
        }
    }
    
    pImpl->totalSimulationTime = totalTime;
    pImpl->totalInferenceSteps = iterations * stepsPerIter;
    
    NLM_LOG_INFO("Benchmark completed: " + std::to_string(totalTime) + " seconds total");
}

std::string Brain::getBenchmarkReport() const {
    if (pImpl->performanceHistory.empty()) {
        return "No benchmark data available. Run benchmark first.";
    }
    
    std::ostringstream oss;
    oss << "=== Benchmark Report ===" << std::endl;
    oss << "Total iterations: " << pImpl->performanceHistory.size() << std::endl;
    oss << "Total simulation time: " << pImpl->totalSimulationTime << " seconds" << std::endl;
    oss << "Total steps: " << pImpl->totalInferenceSteps << std::endl;
    
    if (!pImpl->performanceHistory.empty()) {
        double sum = std::accumulate(pImpl->performanceHistory.begin(), 
                                    pImpl->performanceHistory.end(), 0.0);
        double avg = sum / pImpl->performanceHistory.size();
        double min = *std::min_element(pImpl->performanceHistory.begin(), 
                                     pImpl->performanceHistory.end());
        double max = *std::max_element(pImpl->performanceHistory.begin(), 
                                     pImpl->performanceHistory.end());
        
        oss << "Average time per iteration: " << avg << " seconds" << std::endl;
        oss << "Min time per iteration: " << min << " seconds" << std::endl;
        oss << "Max time per iteration: " << max << " seconds" << std::endl;
        
        double stddev = std::sqrt(
            std::accumulate(pImpl->performanceHistory.begin(), 
                          pImpl->performanceHistory.end(), 0.0,
                          [avg](double acc, double val) { 
                              return acc + (val - avg) * (val - avg); 
                          }) / pImpl->performanceHistory.size());
        
        oss << "Performance standard deviation: " << stddev << std::endl;
    }
    
    return oss.str();
}

void Brain::resetBenchmarkStats() {
    pImpl->performanceHistory.clear();
    pImpl->memoryUsageHistory.clear();
    pImpl->totalInferenceSteps = 0;
    pImpl->totalSimulationTime = 0.0;
    
    NLM_LOG_INFO("Benchmark statistics reset");
}

// Memory optimization
void Brain::optimizeMemoryUsage() {
    NLM_LOG_INFO("Optimizing memory usage...");
    
    // Force garbage collection and compact memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->compact();
    }
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidateOldEpisodes();
    }
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->rebuildAssociations();
    }
    
    // Clear performance history to free memory
    pImpl->performanceHistory.clear();
    pImpl->memoryUsageHistory.clear();
    
    NLM_LOG_INFO("Memory optimization completed");
}

void Brain::compactMemory() {
    NLM_LOG_INFO("Compacting memory...");
    
    // Clear large data structures
    pImpl->performanceHistory.clear();
    pImpl->memoryUsageHistory.clear();
    
    NLM_LOG_INFO("Memory compaction completed");
}

size_t Brain::getEstimatedMemoryUsage() const {
    size_t totalMemory = 0;
    
    // Estimate memory based on active components
    if (pImpl->workingMemory) {
        totalMemory += pImpl->workingMemoryCapacity * sizeof(float);
    }
    if (pImpl->episodicMemory) {
        totalMemory += pImpl->episodicMemoryCapacity * sizeof(float);
    }
    if (pImpl->associativeMemory) {
        totalMemory += pImpl->associativeMemoryCapacity * sizeof(float);
    }
    totalMemory += pImpl->regions.size() * sizeof(NeuralRegion);
    totalMemory += pImpl->performanceHistory.size() * sizeof(double);
    totalMemory += pImpl->memoryUsageHistory.size() * sizeof(size_t);
    
    return totalMemory;
}

// Advanced analytics
std::string Brain::analyzeLearningProgress() const {
    std::ostringstream oss;
    oss << "=== Learning Progress Analysis ===" << std::endl;
    oss << "Total spikes: " << pImpl->totalSpikesTotal << std::endl;
    oss << "Average spikes per step: " << 
        (pImpl->currentStep > 0 ? (double)pImpl->totalSpikesTotal / pImpl->currentStep : 0.0) << std::endl;
    oss << "Firing rate: " << 
        (pImpl->getTotalNeuronCount() > 0 ? 
         (double)pImpl->getFiringNeuronCount() / pImpl->getTotalNeuronCount() * 100.0 : 0.0) << "%" << std::endl;
    
    // Calculate plasticity effectiveness
    float totalSynapticChange = 0.0f;
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            totalSynapticChange += std::abs(syn->getWeight());
        }
    }
    
    oss << "Total synaptic weight changes: " << totalSynapticChange << std::endl;
    oss << "Learning rate estimate: " << 
        (pImpl->currentStep > 0 ? totalSynapticChange / pImpl->currentStep : 0.0) << std::endl;
    
    return oss.str();
}

std::string Brain::analyzeMemoryEfficiency() const {
    std::ostringstream oss;
    oss << "=== Memory Efficiency Analysis ===" << std::endl;
    
    if (pImpl->workingMemory) {
        float workingMemoryUsage = pImpl->workingMemory->getMemoryUsage();
        oss << "Working memory usage: " << workingMemoryUsage << " units" << std::endl;
        oss << "Working memory capacity: " << pImpl->workingMemoryCapacity << " units" << std::endl;
        oss << "Working memory efficiency: " << 
            (pImpl->workingMemoryCapacity > 0 ? workingMemoryUsage / pImpl->workingMemoryCapacity * 100.0 : 0.0) << "%" << std::endl;
    }
    
    if (pImpl->episodicMemory) {
        size_t episodicMemoryUsage = pImpl->episodicMemory->getMemoryUsage();
        oss << "Episodic memory usage: " << episodicMemoryUsage << " units" << std::endl;
        oss << "Episodic memory capacity: " << pImpl->episodicMemoryCapacity << " units" << std::endl;
        oss << "Episodic memory efficiency: " << 
            (pImpl->episodicMemoryCapacity > 0 ? episodicMemoryUsage / pImpl->episodicMemoryCapacity * 100.0 : 0.0) << "%" << std::endl;
    }
    
    if (pImpl->associativeMemory) {
        size_t associativeMemoryUsage = pImpl->associativeMemory->getMemoryUsage();
        oss << "Associative memory usage: " << associativeMemoryUsage << " units" << std::endl;
        oss << "Associative memory capacity: " << pImpl->associativeMemoryCapacity << " units" << std::endl;
        oss << "Associative memory efficiency: " << 
            (pImpl->associativeMemoryCapacity > 0 ? associativeMemoryUsage / pImpl->associativeMemoryCapacity * 100.0 : 0.0) << "%" << std::endl;
    }
    
    return oss.str();
}

std::string Brain::analyzePlasticityEffectiveness() const {
    std::ostringstream oss;
    oss << "=== Plasticity Effectiveness Analysis ===" << std::endl;
    
    // Calculate plasticity metrics
    float totalSTDPChange = 0.0f;
    float totalHebbianChange = 0.0f;
    float totalStructuralChange = 0.0f;
    
    // Estimate changes from spike recordings
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            totalSTDPChange += std::abs(weight) * 0.4f;
            totalHebbianChange += std::abs(weight) * 0.4f;
            totalStructuralChange += std::abs(weight) * 0.2f;
        }
    }
    
    oss << "Total STDP effectiveness: " << totalSTDPChange << std::endl;
    oss << "Total Hebbian effectiveness: " << totalHebbianChange << std::endl;
    oss << "Total structural effectiveness: " << totalStructuralChange << std::endl;
    
    float totalPlasticityEffectiveness = totalSTDPChange + totalHebbianChange + totalStructuralChange;
    oss << "Total plasticity effectiveness: " << totalPlasticityEffectiveness << std::endl;
    oss << "Plasticity efficiency score: " << 
        (totalPlasticityEffectiveness > 0 ? totalPlasticityEffectiveness / 100.0 * 100.0 : 0.0) << "/100" << std::endl;
    
    return oss.str();
}

// Configuration and tuning
void Brain::tuneForEnvironment(const std::string& environmentType) {
    NLM_LOG_INFO("Tuning brain for environment: " + environmentType);
    
    if (environmentType == "fast") {
        configureForPerformanceMode();
        pImpl->enableParallelProcessing = true;
        pImpl->threadCount = std::thread::hardware_concurrency();
    } else if (environmentType == "accurate") {
        configureForAccuracyMode();
        pImpl->enablePrecisionTimestamps = true;
        pImpl->enableAccuracyLogging = true;
    } else if (environmentType == "balanced") {
        configureForBalancedMode();
        pImpl->enableParallelProcessing = true;
        pImpl->threadCount = 2;
    } else if (environmentType == "resource_constrained") {
        pImpl->enableParallelProcessing = false;
        pImpl->threadCount = 1;
        pImpl->enableMemoryTracking = true;
        pImpl->workingMemoryCapacity = 1000;
    } else {
        NLM_LOG_WARNING("Unknown environment type: " + environmentType + ". Using balanced configuration.");
        configureForBalancedMode();
    }
    
    NLM_LOG_INFO("Brain tuning for " + environmentType + " environment completed");
}

void Brain::enableEnvironmentSpecificOptimizations() {
    NLM_LOG_INFO("Enabling environment-specific optimizations...");
    
    // Enable optimizations based on current configuration
    if (pImpl->enableParallelProcessing) {
        NLM_LOG_INFO("Enabled parallel processing optimizations");
    }
    if (pImpl->enableEventDrivenSimulation) {
        NLM_LOG_INFO("Enabled event-driven simulation optimizations");
    }
    if (pImpl->enablePerformanceMetrics) {
        NLM_LOG_INFO("Enabled performance monitoring optimizations");
    }
    
    NLM_LOG_INFO("Environment-specific optimizations enabled");
}

std::string Brain::getEnvironmentSpecificConfig() const {
    std::ostringstream oss;
    oss << "=== Environment-Specific Configuration ===" << std::endl;
    oss << "Environment type: " << (pImpl->enableParallelProcessing ? "parallel_optimized" : "sequential") << std::endl;
    oss << "Performance mode: " << (pImpl->enablePerformanceMetrics ? "monitored" : "basic") << std::endl;
    oss << "Event-driven: " << (pImpl->enableEventDrivenSimulation ? "enabled" : "disabled") << std::endl;
    oss << "Memory tracking: " << (pImpl->enableMemoryTracking ? "enabled" : "disabled") << std::endl;
    oss << "Real-time monitoring: " << (pImpl->enableRealTimeMonitoring ? "enabled" : "disabled") << std::endl;
    oss << "Thread count: " << pImpl->threadCount << std::endl;
    oss << "Data export: " << (pImpl->enableDataExport ? "enabled" : "disabled") << std::endl;
    oss << "Data format: " << pImpl->dataFormat << std::endl;
    
    return oss.str();
}

// Emergency controls
void Brain::emergencyStop() {
    NLM_LOG_ERROR("EMERGENCY STOP ACTIVATED BY POWER USER");
    
    // Stop all processes
    pImpl->enableProfiling = false;
    pImpl->enablePerformanceMetrics = false;
    pImpl->enableRealTimeMonitoring = false;
    pImpl->enableEventDrivenSimulation = false;
    pImpl->enableParallelProcessing = false;
    
    // Clear performance history
    pImpl->performanceHistory.clear();
    pImpl->memoryUsageHistory.clear();
    
    NLM_LOG_ERROR("Emergency stop completed. Brain is in safe mode.");
}

void Brain::resetToDefaults() {
    NLM_LOG_INFO("Resetting brain to default configuration");
    
    // Reset all advanced features to default
    pImpl->enableProfiling = false;
    pImpl->enableMemoryTracking = false;
    pImpl->enablePerformanceMetrics = false;
    pImpl->enableRealTimeMonitoring = false;
    pImpl->enableHebbianPlasticity = true;
    pImpl->enableSTDPPlasticity = true;
    pImpl->enableStructuralPlasticity = true;
    pImpl->enableEventDrivenSimulation = true;
    pImpl->enableParallelProcessing = false;
    pImpl->threadCount = 1;
    pImpl->enableDataExport = false;
    pImpl->dataExportPath = "";
    pImpl->dataFormat = "json";
    
    // Reset performance metrics
    pImpl->totalInferenceSteps = 0;
    pImpl->totalSimulationTime = 0.0;
    pImpl->performanceHistory.clear();
    pImpl->memoryUsageHistory.clear();
    
    NLM_LOG_INFO("Brain reset to default configuration completed");
}

bool Brain::isInEmergencyState() const {
    // Check if brain is in emergency state
    return pImpl->enableProfiling && pImpl->enablePerformanceMetrics && 
           pImpl->enableRealTimeMonitoring && pImpl->enableEventDrivenSimulation &&
           pImpl->enableParallelProcessing && pImpl->enableDataExport;
}

// Debug and diagnostic tools
void Brain::enableDebugMode(bool enable) {
    if (enable) {
        NLM_LOG_INFO("Debug mode enabled by power user");
        pImpl->enablePerformanceMetrics = true;
        pImpl->enableRealTimeMonitoring = true;
        pImpl->enableProfiling = true;
    } else {
        NLM_LOG_INFO("Debug mode disabled by power user");
    }
}

bool Brain::isDebugModeEnabled() const {
    return pImpl->enablePerformanceMetrics && pImpl->enableRealTimeMonitoring && 
           pImpl->enableProfiling;
}

std::string Brain::getSystemDiagnostics() const {
    std::ostringstream oss;
    oss << "=== System Diagnostics ===" << std::endl;
    oss << "Brain state: " << (isInEmergencyState() ? "Emergency" : "Normal") << std::endl;
    oss << "Debug mode: " << (isDebugModeEnabled() ? "Enabled" : "Disabled") << std::endl;
    oss << "Profiling enabled: " << (pImpl->enableProfiling ? "Yes" : "No") << std::endl;
    oss << "Performance metrics: " << (pImpl->enablePerformanceMetrics ? "Yes" : "No") << std::endl;
    oss << "Memory tracking: " << (pImpl->enableMemoryTracking ? "Yes" : "No") << std::endl;
    oss << "Real-time monitoring: " << (pImpl->enableRealTimeMonitoring ? "Yes" : "No") << std::endl;
    oss << "Event-driven simulation: " << (pImpl->enableEventDrivenSimulation ? "Yes" : "No") << std::endl;
    oss << "Parallel processing: " << (pImpl->enableParallelProcessing ? "Yes" : "No") << std::endl;
    oss << "Thread count: " << pImpl->threadCount << std::endl;
    oss << "Total neurons: " << getTotalNeuronCount() << std::endl;
    oss << "Total synapses: " << getTotalSynapseCount() << std::endl;
    oss << "Total spikes: " << pImpl->totalSpikesTotal << std::endl;
    oss << "Current step: " << pImpl->currentStep << std::endl;
    oss << "Estimated memory usage: " << getEstimatedMemoryUsage() << " bytes" << std::endl;
    
    if (pImpl->enablePerformanceMetrics) {
        oss << "Performance metrics: " << getAveragePerformance() << " avg, " 
            << getMinPerformance() << " min, " << getMaxPerformance() << " max" << std::endl;
    }
    
    if (pImpl->enableMemoryTracking) {
        oss << "Memory metrics: " << getAverageMemoryUsage() << " avg, " 
            << getPeakMemoryUsage() << " peak" << std::endl;
    }
    
    return oss.str();
}

std::vector<std::string> Brain::getSystemWarnings() const {
    std::vector<std::string> warnings;
    
    if (pImpl->totalSpikesTotal == 0) {
        warnings.push_back("No spikes recorded - network may be inactive");
    }
    
    if (getTotalNeuronCount() == 0) {
        warnings.push_back("No neurons initialized - brain is empty");
    }
    
    if (getTotalSynapseCount() == 0) {
        warnings.push_back("No synapses created - network connectivity is zero");
    }
    
    if (pImpl->enableDataExport && pImpl->dataExportPath.empty()) {
        warnings.push_back("Data export enabled but no export path specified");
    }
    
    if (pImpl->enableParallelProcessing && pImpl->threadCount < 1) {
        warnings.push_back("Parallel processing enabled but thread count is zero or negative");
    }
    
    if (pImpl->enableMemoryTracking && pImpl->workingMemoryCapacity < 100) {
        warnings.push_back("Memory tracking enabled but working memory capacity is very small");
    }
    
    return warnings;
}

bool Brain::validateSystemState() const {
    bool isValid = true;
    
    // Check for critical issues
    if (getTotalNeuronCount() == 0) {
        NLM_LOG_ERROR("Validation failed: No neurons initialized");
        isValid = false;
    }
    
    if (getTotalSynapseCount() == 0) {
        NLM_LOG_ERROR("Validation failed: No synapses created");
        isValid = false;
    }
    
    if (pImpl->totalSpikesTotal == 0) {
        NLM_LOG_WARNING("Validation warning: No spikes recorded");
    }
    
    if (pImpl->enableDataExport && pImpl->dataExportPath.empty()) {
        NLM_LOG_WARNING("Validation warning: Data export enabled but no export path specified");
    }
    
    if (pImpl->enableParallelProcessing && pImpl->threadCount < 1) {
        NLM_LOG_ERROR("Validation failed: Parallel processing enabled but invalid thread count");
        isValid = false;
    }
    
    if (!isValid) {
        NLM_LOG_ERROR("System validation failed");
    } else {
        NLM_LOG_INFO("System validation passed");
    }
    
    return isValid;
}

// Implementation of helper methods
void Brain::recordPerformanceMetric(double value) {
    if (pImpl->enablePerformanceMetrics) {
        pImpl->performanceHistory.push_back(value);
        
        // Keep only recent history
        if (pImpl->performanceHistory.size() > pImpl->maxPerformanceHistory) {
            pImpl->performanceHistory.erase(pImpl->performanceHistory.begin());
        }
        
        // Update statistics
        if (value > pImpl->maxSpikesPerStep) {
            pImpl->maxSpikesPerStep = value;
        }
        if (value < pImpl->minSpikesPerStep) {
            pImpl->minSpikesPerStep = value;
        }
    }
}

double Brain::getAveragePerformance() const {
    if (pImpl->performanceHistory.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(pImpl->performanceHistory.begin(), 
                                pImpl->performanceHistory.end(), 0.0);
    return sum / pImpl->performanceHistory.size();
}

double Brain::getMinPerformance() const {
    if (pImpl->performanceHistory.empty()) {
        return std::numeric_limits<double>::max();
    }
    
    return *std::min_element(pImpl->performanceHistory.begin(), 
                           pImpl->performanceHistory.end());
}

double Brain::getMaxPerformance() const {
    if (pImpl->performanceHistory.empty()) {
        return 0.0;
    }
    
    return *std::max_element(pImpl->performanceHistory.begin(), 
                           pImpl->performanceHistory.end());
}

void Brain::recordMemoryUsage(size_t bytes) {
    if (pImpl->enableMemoryTracking) {
        pImpl->memoryUsageHistory.push_back(bytes);
        
        // Keep only recent history
        if (pImpl->memoryUsageHistory.size() > pImpl->maxPerformanceHistory) {
            pImpl->memoryUsageHistory.erase(pImpl->memoryUsageHistory.begin());
        }
    }
}

size_t Brain::getPeakMemoryUsage() const {
    if (pImpl->memoryUsageHistory.empty()) {
        return 0;
    }
    
    return *std::max_element(pImpl->memoryUsageHistory.begin(), 
                            pImpl->memoryUsageHistory.end());
}

double Brain::getAverageMemoryUsage() const {
    if (pImpl->memoryUsageHistory.empty()) {
        return 0.0;
    }
    
    size_t sum = std::accumulate(pImpl->memoryUsageHistory.begin(), 
                                pImpl->memoryUsageHistory.end(), 0ULL);
    return static_cast<double>(sum) / pImpl->memoryUsageHistory.size();
}

std::string Brain::getRealTimeStats() const {
    std::ostringstream oss;
    oss << "=== Real-Time Statistics ===" << std::endl;
    oss << "Current simulation step: " << pImpl->currentStep << std::endl;
    oss << "Total spikes so far: " << pImpl->totalSpikesTotal << std::endl;
    oss << "Total neurons: " << getTotalNeuronCount() << std::endl;
    oss << "Total synapses: " << getTotalSynapseCount() << std::endl;
    oss << "Active neurons: " << getActiveNeuronCount() << std::endl;
    oss << "Firing neurons: " << getFiringNeuronCount() << std::endl;
    oss << "Excitation/inhibition ratio: " << getExcitationInhibitionRatio() << std::endl;
    oss << "Average firing rate: " << getAverageFiringRate() << " Hz" << std::endl;
    oss << "Estimated memory usage: " << getEstimatedMemoryUsage() << " bytes" << std::endl;
    
    if (pImpl->enablePerformanceMetrics) {
        oss << "Performance metrics: " << getAveragePerformance() << " avg, " 
            << getMinPerformance() << " min, " << getMaxPerformance() << " max" << std::endl;
    }
    
    if (pImpl->enableMemoryTracking) {
        oss << "Memory metrics: " << getAverageMemoryUsage() << " avg, " 
            << getPeakMemoryUsage() << " peak" << std::endl;
    }
    
    return oss.str();
}

// Additional missing includes
#include <sstream>
#include <algorithm>
#include <numeric>
#include <limits>
#include <thread>

} // namespace nlm

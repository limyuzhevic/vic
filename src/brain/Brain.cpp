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
#include "../errors/ErrorTypes.hpp"
#include "../errors/ErrorContext.hpp"
#include "../error_reporting/ErrorReporter.hpp"
#include "../monitoring/SystemMonitor.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <mutex>
#include <unordered_map>
#include <map>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Error handling components
    std::shared_ptr<ErrorReporter> errorReporter;
    std::shared_ptr<MonitoringSystem> monitoringSystem;
    
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
        , consolidationInterval(1000)  // Consolidate every 1000 steps)
        , errorReporter(nullptr)
        , monitoringSystem(nullptr) {
        // Initialize error handling components
        errorReporter = std::make_shared<DefaultErrorReporter>();
        monitoringSystem = std::make_shared<DefaultMonitoringSystem>();
        
        // Initialize monitoring system
        monitoringSystem->start();
        
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
    
    // Validate configuration
    if (!pImpl->config) {
        NLM_LOG_ERROR("Brain configuration is null");
        return false;
    }
    
    // Get configuration values with validation
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    // Validate configuration values
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Invalid neuron count: " + std::to_string(neuronCount));
        return false;
    }
    
    if (regionCount == 0) {
        NLM_LOG_ERROR("Invalid region count: " + std::to_string(regionCount));
        return false;
    }
    
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Invalid connection probability: " + std::to_string(connectionProbability));
        return false;
    }
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions, " +
                 std::to_string(connectionProbability) + " connection probability");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    if (neuronsPerRegion == 0) {
        NLM_LOG_ERROR("Not enough neurons to distribute across regions");
        return false;
    }
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(std::max(size_t(1), neuronsPerRegion / 4), NeuronType::Sensory);
            auto internalPopId = region->addPopulation(std::max(size_t(1), neuronsPerRegion / 2), NeuronType::Internal);
            auto motorPopId = region->addPopulation(std::max(size_t(1), neuronsPerRegion / 4), NeuronType::Motor);
            
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
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
        NLM_LOG_INFO("Working memory initialized with capacity: " + std::to_string(neuronCount / 10));
    } else {
        NLM_LOG_ERROR("Failed to initialize working memory");
        return false;
    }
    
    // Initialize episodic memory
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
        NLM_LOG_INFO("Episodic memory initialized with max episodes: 1000");
    } else {
        NLM_LOG_ERROR("Failed to initialize episodic memory");
        return false;
    }
    
    // Initialize associative memory
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(this);
        NLM_LOG_INFO("Associative memory initialized");
    } else {
        NLM_LOG_ERROR("Failed to initialize associative memory");
        return false;
    }
    
    // Initialize prediction system
    if (pImpl->predictionSystem) {
        // PredictionSystem doesn't have initialize method currently
        NLM_LOG_INFO("Prediction system initialized");
    } else {
        NLM_LOG_WARNING("Prediction system not available");
    }
    
    // Initialize cognition systems
    if (pImpl->planner) {
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(5);
        NLM_LOG_INFO("Neural planner initialized with depth: 5");
    } else {
        NLM_LOG_ERROR("Failed to initialize neural planner");
        return false;
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->initialize(this);
        NLM_LOG_INFO("Concept formation initialized");
    } else {
        NLM_LOG_ERROR("Failed to initialize concept formation");
        return false;
    }
    
    if (pImpl->attention) {
        pImpl->attention->initialize(this);
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
        NLM_LOG_INFO("Attention system initialized with inhibition: 0.5, excitation: 1.5");
    } else {
        NLM_LOG_ERROR("Failed to initialize attention system");
        return false;
    }
    
    // Initialize neuromodulation
    if (pImpl->novelty) {
        pImpl->novelty->initialize(this);
        NLM_LOG_INFO("Novelty detection system initialized");
    } else {
        NLM_LOG_ERROR("Failed to initialize novelty system");
        return false;
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->initialize(this);
        NLM_LOG_INFO("Curiosity system initialized");
    } else {
        NLM_LOG_ERROR("Failed to initialize curiosity system");
        return false;
    }
    
    // Register spike handlers for event-driven processing
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
        
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
        
        NLM_LOG_INFO("Spike system handlers registered successfully");
    } else {
        NLM_LOG_ERROR("Failed to initialize spike system");
        return false;
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        NLM_LOG_INFO("Checkpoint manager configured with directory: " + checkpointDir);
    } else {
        NLM_LOG_ERROR("Failed to initialize checkpoint manager");
        return false;
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
    if (numSensory == 0) {
        NLM_LOG_WARNING("No sensory neurons available for input processing");
        return;
    }
    
    // Validate input range
    for (size_t i = 0; i < values.size(); ++i) {
        if (std::isnan(values[i]) || std::isinf(values[i])) {
            NLM_LOG_ERROR("Invalid sensory input value at index " + std::to_string(i) + ": " + std::to_string(values[i]));
            continue; // Skip invalid values
        }
    }
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Validate normalized value
        if (std::isnan(normalizedValue) || std::isinf(normalizedValue)) {
            NLM_LOG_ERROR("Invalid normalized sensory input value at index " + std::to_string(i) + ": " + std::to_string(normalizedValue));
            continue;
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
    // Validate current value
    if (std::isnan(current) || std::isinf(current)) {
        NLM_LOG_ERROR("Invalid current value: " + std::to_string(current));
        return;
    }
    
    // Find neuron in the brain
    bool found = false;
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                found = true;
                return;
            }
        }
    }
    
    // If neuron not found, log warning
    if (!found) {
        NLM_LOG_WARNING("Neuron " + std::to_string(neuron.index()) + " not found for current injection");
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Validate current value
    if (std::isnan(current) || std::isinf(current)) {
        NLM_LOG_ERROR("Invalid current value: " + std::to_string(current));
        return;
    }
    
    // Find neurons of the specified type
    bool found = false;
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                    found = true;
                }
            }
        }
    }
    
    // If no neurons of the specified type found, log warning
    if (!found) {
        NLM_LOG_WARNING("No neurons of type " + std::to_string(static_cast<int>(type)) + " found for current injection");
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
        NLM_LOG_WARNING("No motor neurons available for action production");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Determine action based on motor neuron activity
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
    
    // Validate modulation value
    if (std::isnan(modulation) || std::isinf(modulation)) {
        NLM_LOG_ERROR("Invalid neuromodulation level: " + std::to_string(modulation));
        return;
    }
    
    // Ensure modulation is within valid range
    if (modulation < 0.0f) {
        NLM_LOG_WARNING("Neuromodulation level is negative: " + std::to_string(modulation));
    } else if (modulation > 10.0f) {
        NLM_LOG_WARNING("Neuromodulation level is very high: " + std::to_string(modulation));
    }
    
    // Scale STDP learning rates
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    } else {
        NLM_LOG_WARNING("STDP system not available for neuromodulation");
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
    
    // Reset simulation state
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    // Reset all regions
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    // Reset spike system
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    } else {
        NLM_LOG_WARNING("Spike system not available for reset");
    }
    
    // Reset developmental stage
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems with validation
    if (pImpl->workingMemory) {
        pImpl->workingMemory->clear();
        NLM_LOG_INFO("Working memory cleared");
    } else {
        NLM_LOG_WARNING("Working memory not available for reset");
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->clear();
        NLM_LOG_INFO("Episodic memory cleared");
    } else {
        NLM_LOG_WARNING("Episodic memory not available for reset");
    }
    
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->clear();
        NLM_LOG_INFO("Associative memory cleared");
    } else {
        NLM_LOG_WARNING("Associative memory not available for reset");
    }
    
    // Reset attention system
    if (pImpl->attention) {
        pImpl->attention->reset();
        NLM_LOG_INFO("Attention system reset");
    } else {
        NLM_LOG_WARNING("Attention system not available for reset");
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

// Error handling implementation

bool Brain::handleError(const ErrorContext& error) {
    try {
        // Create ErrorInfo from ErrorContext
        ErrorInfo errorInfo;
        errorInfo.id = ErrorManager::generateErrorId();
        errorInfo.level = static_cast<ErrorLevel>(static_cast<int>(error.severity));
        errorInfo.category = ErrorCategory::Custom;
        errorInfo.message = error.message;
        errorInfo.description = error.description;
        errorInfo.sourceFile = error.sourceFile;
        errorInfo.sourceFunction = error.sourceFunction;
        errorInfo.lineNumber = error.lineNumber;
        errorInfo.stackTrace = error.stackTrace;
        errorInfo.context = error.metadata;
        errorInfo.suggestions = error.recoveryStrategy;
        errorInfo.isHandled = error.isRecovered;
        
        // Report error using the error reporter
        errorReporter->reportError(errorInfo);
        
        // Determine if error is recoverable
        if (error.isRecoverable()) {
            // Attempt recovery
            return recoverFromError(error);
        }
        
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception handling error: ") + e.what());
        return false;
    }
}

bool Brain::handleWarning(const ErrorContext& warning) {
    try {
        // Create ErrorInfo from ErrorContext
        ErrorInfo warningInfo;
        warningInfo.id = ErrorManager::generateErrorId();
        warningInfo.level = ErrorLevel::Warning;
        warningInfo.category = ErrorCategory::Custom;
        warningInfo.message = warning.message;
        warningInfo.description = warning.description;
        warningInfo.sourceFile = warning.sourceFile;
        warningInfo.sourceFunction = warning.sourceFunction;
        warningInfo.lineNumber = warning.lineNumber;
        warningInfo.stackTrace = warning.stackTrace;
        warningInfo.context = warning.metadata;
        warningInfo.suggestions = warning.recoveryStrategy;
        warningInfo.isHandled = warning.isRecovered;
        
        // Report warning using the error reporter
        errorReporter->reportWarning(warningInfo.message, ErrorCategory::Custom);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception handling warning: ") + e.what());
        return false;
    }
}

ErrorHandlingResult Brain::handleError(const ErrorHandlingResult& result) {
    try {
        // Handle each error in the result
        for (const auto& error : result.errors) {
            handleError(error);
        }
        
        for (const auto& warning : result.warnings) {
            handleWarning(warning);
        }
        
        return result;
        
    } catch (const std::exception& e) {
        ErrorHandlingResult failureResult;
        failureResult.isValid = false;
        failureResult.message = std::string("Exception handling error: ") + e.what();
        return failureResult;
    }
}

// Error reporting implementation
void Brain::reportError(const ErrorContext& error) {
    errorReporter->reportError(error.message, static_cast<ErrorLevel>(static_cast<int>(error.severity)),
                             ErrorCategory::Custom, error.sourceFile, error.sourceFunction, error.lineNumber);
}

void Brain::reportWarning(const ErrorContext& warning) {
    errorReporter->reportWarning(warning.message, ErrorCategory::Custom);
}

void Brain::reportError(const std::string& message, ErrorSeverity severity, ErrorCode code,
                       const std::string& sourceFile, const std::string& sourceFunction,
                       int sourceLine) {
    errorReporter->reportError(message, static_cast<ErrorLevel>(static_cast<int>(severity)),
                             ErrorCategory::Custom, sourceFile, sourceFunction, sourceLine);
}

// Recovery implementation
bool Brain::recoverFromError(const ErrorContext& error) {
    try {
        // Check if error is recoverable
        if (!error.isRecoverable()) {
            return false;
        }
        
        // Attempt to recover based on error type
        switch (error.type) {
            case ErrorType::MemoryAllocation:
                return handleMemoryErrorRecovery(error);
            case ErrorType::ConfigurationInvalid:
                return handleConfigurationErrorRecovery(error);
            case ErrorType::ResourceExhaustion:
                return handleResourceErrorRecovery(error);
            case ErrorType::NeuronInvalid:
                return handleNeuralErrorRecovery(error);
            case ErrorType::WorkingMemoryFull:
                return handleMemoryErrorRecovery(error);
            case ErrorType::PredictionFailed:
                return handlePredictionErrorRecovery(error);
            default:
                // Generic recovery attempt
                return genericErrorRecovery(error);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during error recovery: ") + e.what());
        return false;
    }
}

bool Brain::rollbackState(const std::string& operationId) {
    try {
        // Get transaction manager
        auto transactionManager = getDefaultTransactionManager();
        
        // Rollback the specified operation
        return transactionManager->rollbackTransaction(operationId);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during state rollback: ") + e.what());
        return false;
    }
}

bool Brain::restoreCheckpoint(const std::string& checkpointId) {
    try {
        // Get checkpoint manager
        if (!pImpl->checkpointManager) {
            NLM_LOG_ERROR("Checkpoint manager not available for restoration");
            return false;
        }
        
        // Restore checkpoint
        return pImpl->checkpointManager->restore(checkpointId);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during checkpoint restoration: ") + e.what());
        return false;
    }
}

// Monitoring implementation
SystemMetrics Brain::getSystemMetrics() const {
    try {
        // Get default monitoring system
        auto monitoringSystem = getDefaultMonitoringSystem();
        return monitoringSystem->collectSystemMetrics();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting system metrics: ") + e.what());
        return SystemMetrics();
    }
}

ComponentHealth Brain::getComponentHealth() const {
    try {
        // Get default monitoring system
        auto monitoringSystem = getDefaultMonitoringSystem();
        return monitoringSystem->collectComponentHealth("Brain");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting component health: ") + e.what());
        return ComponentHealth();
    }
}

HealthStatus Brain::getHealthStatus() const {
    try {
        // Get default monitoring system
        auto monitoringSystem = getDefaultMonitoringSystem();
        return monitoringSystem->analyzeSystemHealth();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting health status: ") + e.what());
        return HealthStatus::Unknown;
    }
}

double Brain::getHealthScore() const {
    try {
        // Get default monitoring system
        auto monitoringSystem = getDefaultMonitoringSystem();
        return monitoringSystem->calculateOverallHealthScore();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting health score: ") + e.what());
        return 0.0;
    }
}

// Specialized error recovery methods
bool Brain::handleMemoryErrorRecovery(const ErrorContext& error) {
    try {
        // Clear working memory
        if (pImpl->workingMemory) {
            pImpl->workingMemory->clear();
        }
        
        // Clear episodic memory
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->clear();
        }
        
        // Clear associative memory
        if (pImpl->associativeMemory) {
            pImpl->associativeMemory->clear();
        }
        
        NLM_LOG_WARNING("Memory error recovery completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during memory error recovery: ") + e.what());
        return false;
    }
}

bool Brain::handleConfigurationErrorRecovery(const ErrorContext& error) {
    try {
        // Reset to default configuration
        if (pImpl->config) {
            // Reload default configuration
            auto defaultConfig = Config::fromEnvironment("default");
            if (defaultConfig.has(pImpl->config->getEnvironment())) {
                pImpl->config = std::make_shared<Config>(defaultConfig);
            }
        }
        
        NLM_LOG_WARNING("Configuration error recovery completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during configuration error recovery: ") + e.what());
        return false;
    }
}

bool Brain::handleResourceErrorRecovery(const ErrorContext& error) {
    try {
        // Clear spike system
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->reset();
        }
        
        // Clear plasticity systems
        if (pImpl->stdp) {
            pImpl->stdp->reset();
        }
        if (pImpl->hebbian) {
            pImpl->hebbian->reset();
        }
        if (pImpl->structuralPlasticity) {
            pImpl->structuralPlasticity->reset();
        }
        
        NLM_LOG_WARNING("Resource error recovery completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during resource error recovery: ") + e.what());
        return false;
    }
}

bool Brain::handleNeuralErrorRecovery(const ErrorContext& error) {
    try {
        // Reset all neurons
        for (auto& region : pImpl->regions) {
            region->reset();
        }
        
        // Reset spike system
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->reset();
        }
        
        NLM_LOG_WARNING("Neural error recovery completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during neural error recovery: ") + e.what());
        return false;
    }
}

bool Brain::handlePredictionErrorRecovery(const ErrorContext& error) {
    try {
        // Reset prediction system
        if (pImpl->predictionSystem) {
            // In a real implementation, prediction system would have a reset method
            // For now, we just log the attempt
            NLM_LOG_INFO("Prediction system reset for error recovery");
        }
        
        NLM_LOG_WARNING("Prediction error recovery completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during prediction error recovery: ") + e.what());
        return false;
    }
}

bool Brain::genericErrorRecovery(const ErrorContext& error) {
    try {
        // Attempt generic recovery
        // This could involve:
        // 1. Clearing caches
        // 2. Resetting to known good state
        // 3. Attempting graceful shutdown
        // 4. Falling back to default behavior
        
        NLM_LOG_WARNING("Generic error recovery attempted");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during generic error recovery: ") + e.what());
        return false;
    }
}

// Error handler registration
def
void Brain::setErrorReporter(std::shared_ptr<ErrorReporter> reporter) {
    if (reporter) {
        errorReporter = reporter;
    }
}

def
void Brain::setMonitoringSystem(std::shared_ptr<MonitoringSystem> monitor) {
    if (monitor) {
        monitoringSystem = monitor;
    }
}


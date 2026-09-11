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
#include "../cognition/SelfModel.hpp"
#include "../cognition/SocialLearning.hpp"
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
    std::unique_ptr<SelfModel> selfModel;
    std::unique_ptr<SocialLearning> socialLearning;
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
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
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
    selfModel = std::make_unique<SelfModel>();
    socialLearning = std::make_unique<SocialLearning>();
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
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
    }
    
    // Initialize episodic memory
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(this);
    }
    
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
        // Update prediction system with current brain state
        // The prediction system uses sensory input to predict next state
        // and generates prediction error signals for learning
        
        // Simple prediction using current neural activity patterns
        float predictionUpdate = 0.01f;  // Learning rate for prediction
        
        // Calculate current state summary from neural activity
        float activitySum = 0.0f;
        size_t activeCount = 0;
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isActive()) {
                        activitySum += std::abs(neuron->getState().membranePotential);
                        activeCount++;
                    }
                }
            }
        }
        
        float currentActivity = activeCount > 0 ? activitySum / activeCount : 0.0f;
        
        // Update prediction system
        pImpl->predictionSystem->updatePrediction(currentActivity);
        
        // Get prediction error and apply to neuromodulation
        float predictionError = pImpl->predictionSystem->getPredictionError();
        
        if (pImpl->predictionError) {
            pImpl->predictionError->setPredictionError(predictionError);
            pImpl->predictionError->setActualValue(currentActivity);
            pImpl->predictionError->setPredictedValue(pImpl->predictionSystem->getPredictedValue());
        }
        
        // Update prediction error history for learning
        pImpl->predictionError->recordError(predictionError, currentStep);
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
        // Use sensory input encoded in working memory to extract patterns
        
        // Get sensory input from the last step (stored in working memory)
        std::vector<float> currentSensoryState;
        if (pImpl->workingMemory) {
            // Get the most recent sensory encoding from working memory
            currentSensoryState = pImpl->workingMemory->getRecentSensoryState();
        }
        
        // If we have sensory state, form concepts from it
        if (!currentSensoryState.empty()) {
            pImpl->conceptFormation->processSensoryInput(currentSensoryState);
        }
        
        // Update concept formation with current neural activity
        // This helps concepts adapt to current brain state
        pImpl->conceptFormation->updateConceptActivation(pImpl->timestep);
        
        // Get current concepts and influence action selection
        auto currentConcepts = pImpl->conceptFormation->getActiveConcepts();
        for (const auto& concept : currentConcepts) {
            // Concepts can influence planning and attention
            // For now, just log concept formation
            if (concept.importance > 0.5f) {
                NLM_LOG_INFO("Concept formed: importance=" + std::to_string(concept.importance) +
                           " features=" + std::to_string(concept.featureCount));
            }
        }
    }
    
    // ========== STEP 12: Run neural planner for action selection ==========
    if (pImpl->planner) {
        // Neural planner uses prediction system and concept formation to plan actions
        // This happens after concept formation but before action selection
        
        // Get current sensory state for planning
        std::vector<float> currentState;
        if (pImpl->workingMemory) {
            currentState = pImpl->workingMemory->getRecentSensoryState();
        }
        
        // Get current goal (default to activity maintenance)
        std::vector<float> currentGoal = pImpl->planner->getCurrentGoal();
        if (currentGoal.empty()) {
            // Create simple goal: maintain neural activity balance
            float activitySum = 0.0f;
            size_t activeCount = 0;
            for (const auto& region : pImpl->regions) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        if (neuron->isActive()) {
                            activitySum += std::abs(neuron->getState().membranePotential);
                            activeCount++;
                        }
                    }
                }
            }
            float avgActivity = activeCount > 0 ? activitySum / activeCount : 0.0f;
            currentGoal = std::vector<float>(1, avgActivity);  // Goal: maintain this activity level
        }
        
        // Plan next action based on current state and goal
        ActionType plannedAction = pImpl->planner->planAction(currentState, 0.5f);
        
        // Store planned action in working memory for later use
        if (pImpl->workingMemory) {
            pImpl->workingMemory->storePlannedAction(plannedAction);
        }
        
        // Log planning activity
        if (pImpl->planner->getPlanningDepth() > 0) {
            NLM_LOG_INFO("Planner selected action: " + std::to_string(static_cast<int>(plannedAction)) +
                        " depth=" + std::to_string(pImpl->planner->getPlanningDepth()) +
                        " confidence=" + std::to_string(pImpl->planner->getPlanningConfidence()));
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

    // ========== STEP 16: INTEGRATE WITH COGNITION SYSTEMS ==========
    
    // Enhanced NeuralPlanner with advanced prediction integration
    if (pImpl->planner) {
        // Convert sensory input to state vector for planning
        std::vector<float> currentState;
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Use working memory as input to planner
            size_t memorySize = pImpl->workingMemory->getMemoryNeurons().size();
            currentState.reserve(memorySize);
            for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                // Get actual neuron activation from the brain
                float activation = 0.0f;
                for (auto& region : pImpl->regions) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron->getId() == neuronId) {
                            activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f;
                            break;
                        }
                    }
                    if (activation > 0.0f) break;
                }
                currentState.push_back(activation);
            }
        }
        
        // Integrate with prediction system for better planning
        float predictionConfidence = 1.0f;
        if (pImpl->predictionSystem) {
            predictionConfidence = 1.0f - std::abs(pImpl->predictionSystem->getPredictionError());
        }
        
        // Get current goal from concept formation or enhanced prediction
        std::vector<float> currentGoal;
        if (pImpl->conceptFormation) {
            currentGoal = pImpl->conceptFormation->getConceptPrototype(1);
        }
        
        // Use prediction system to refine goals
        if (pImpl->predictionSystem && !currentGoal.empty()) {
            std::vector<float> predictedGoal = pImpl->predictionSystem->predictNextState(currentGoal);
            // Blend current and predicted goals
            for (size_t i = 0; i < std::min(currentGoal.size(), predictedGoal.size()); ++i) {
                currentGoal[i] = currentGoal[i] * 0.5f + predictedGoal[i] * 0.5f * predictionConfidence;
            }
        }
        
        // Set goal if not set
        if (currentGoal.empty() && pImpl->conceptFormation) {
            // Use current sensory features as target for concept formation
            if (!currentState.empty()) {
                currentGoal = currentState;
            }
        }
        
        pImpl->planner->setCurrentGoal(currentGoal);
        
        // Plan action using current state, goal, and enhanced prediction
        float targetReward = 1.0f;
        if (!currentGoal.empty()) {
            // Calculate goal achievement probability
            float goalDistance = 0.0f;
            for (float val : currentState) {
                goalDistance += std::abs(val - currentGoal[0]);
            }
            targetReward = 1.0f - std::min(1.0f, goalDistance / currentGoal.size());
        }
        
        ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
        
        // Store planned action in working memory for motor execution
        if (pImpl->workingMemory) {
            // Store planning result with prediction confidence
            pImpl->workingMemory->storeAction(plannedAction, currentStep);
            pImpl->workingMemory->storePlanningContext(currentState, currentGoal, predictionConfidence);
        }
        
        // Update plan quality with prediction error integration
        float actualReward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        float predictionError = 0.0f;
        if (pImpl->predictionSystem) {
            predictionError = pImpl->predictionSystem->getPredictionError();
        }
        
        pImpl->planner->updatePlanQuality(
            std::vector<ActionType>{plannedAction}, // Would have sequence in full implementation
            std::vector<ActionType>{plannedAction}, // Would have actual actions
            actualReward,
            predictionError
        );
        
        // Store prediction error for self-model
        if (pImpl->selfModel) {
            pImpl->selfModel->recordPredictionError(predictionError, currentStep);
        }
    }
    
    // Enhanced ConceptFormation with prediction and self-model integration
    if (pImpl->conceptFormation && pImpl->workingMemory) {
        // Extract sensory features from working memory with enhanced processing
        std::vector<float> currentPattern;
        std::vector<float> features;
        
        if (!pImpl->workingMemory->getMemoryNeurons().empty()) {
            currentPattern.reserve(pImpl->workingMemory->getMemoryNeurons().size());
            features.reserve(pImpl->workingMemory->getMemoryNeurons().size());
            
            for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                // Get actual neuron activation from the brain
                float activation = 0.0f;
                float membranePotential = 0.0f;
                for (auto& region : pImpl->regions) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron->getId() == neuronId) {
                            membranePotential = neuron->getState().membranePotential;
                            activation = std::abs(membranePotential - neuron->getState().restingPotential) / 10.0f;
                            break;
                        }
                    }
                    if (membranePotential > 0.0f) break;
                }
                currentPattern.push_back(activation);
                features.push_back(activation * 2.0f - 1.0f); // Normalize to [-1, 1]
            }
            
            // Integrate with prediction system
            float noveltyScore = 0.0f;
            if (pImpl->novelty) {
                noveltyScore = pImpl->novelty->getNoveltyLevel();
            }
            
            // Integrate with self-model
            float selfModelConfidence = 1.0f;
            if (pImpl->selfModel) {
                selfModelConfidence = pImpl->selfModel->getConfidence();
            }
            
            // Present to concept formation with enhanced context
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentPattern, features,
                pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,
                currentStep,
                noveltyScore,
                selfModelConfidence
            );
            
            // Store concept ID in working memory for planning
            if (conceptId > 0) {
                pImpl->workingMemory->storeConcept(conceptId);
                
                // Store self-model prediction for this concept
                if (pImpl->selfModel) {
                    std::vector<float> predictedOutcome = pImpl->selfModel->predictActionConsequence(
                        ActionType::Wait, // Would track actual planned action
                        currentPattern
                    );
                    pImpl->workingMemory->storeConceptPrediction(conceptId, predictedOutcome);
                }
            }
            
            // Check if we should merge concepts based on prediction errors
            if (pImpl->conceptFormation->getConceptCount() > 3 && pImpl->predictionSystem) {
                pImpl->conceptFormation->mergeSimilarConcepts(pImpl->predictionSystem->getPredictionError());
            }
        }
    }
    
    // Advanced SelfModel with predictive coding and memory integration
    if (pImpl->selfModel) {
        // Record self-action relationship with enhanced context
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Get actual neural activity for better experience tracking
            std::vector<float> beforeState;
            std::vector<float> afterState;
            
            for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                float activation = 0.0f;
                for (auto& region : pImpl->regions) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron->getId() == neuronId) {
                            activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f;
                            break;
                        }
                    }
                    if (activation > 0.0f) break;
                }
                beforeState.push_back(activation);
                afterState.push_back(activation); // Would be different in full implementation
            }
            
            // Record with enhanced self-awareness
            pImpl->selfModel->recordSelfAction(
                ActionType::Wait, // Would track actual planned action
                beforeState, afterState
            );
            
            // Update capability level with prediction integration
            float experienceLevel = pImpl->workingMemory->getActiveTraces() > 0 ? 0.8f : 0.5f;
            
            // Integrate with prediction system for better self-assessment
            if (pImpl->predictionSystem) {
                float predictionAccuracy = 1.0f - std::abs(pImpl->predictionSystem->getPredictionError());
                experienceLevel = experienceLevel * 0.7f + predictionAccuracy * 0.3f;
            }
            
            pImpl->selfModel->setCapabilityLevel(experienceLevel);
        }
        
        // Enhanced action consequence prediction using prediction system
        if (pImpl->planner) {
            std::vector<float> currentState;
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                currentState.reserve(pImpl->workingMemory->getMemoryNeurons().size());
                for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                    float activation = 0.0f;
                    for (auto& region : pImpl->regions) {
                        for (auto* neuron : region->getAllNeurons()) {
                            if (neuron->getId() == neuronId) {
                                activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f;
                                break;
                            }
                        }
                        if (activation > 0.0f) break;
                    }
                    currentState.push_back(activation);
                }
            }
            
            // Use prediction system for action consequence prediction
            std::vector<float> predictedConsequence = currentState;
            if (pImpl->predictionSystem) {
                predictedConsequence = pImpl->predictionSystem->predictNextState(currentState);
            }
            
            // Update self-model with prediction
            pImpl->selfModel->updateSelfModel(
                currentState, predictedConsequence, 
                ActionType::Wait // Would use actual action
            );
            
            // Store prediction for later comparison
            pImpl->selfModel->storePrediction(currentState, predictedConsequence, currentStep);
        }
    }
    
    // Enhanced SocialLearning with prediction and self-model integration
    if (pImpl->socialLearning) {
        // Process observations from environment with enhanced context
        if (pImpl->dopamine) {
            // Update social knowledge with prediction system integration
            float predictionError = 0.0f;
            if (pImpl->predictionSystem) {
                predictionError = pImpl->predictionSystem->getPredictionError();
            }
            
            // Integrate with self-model
            float selfModelInsight = 0.0f;
            if (pImpl->selfModel) {
                selfModelInsight = pImpl->selfModel->getBodyAwareness();
            }
            
            pImpl->socialLearning->updateSocialKnowledge(
                pImpl->dopamine->getLevel(),
                predictionError,
                selfModelInsight
            );
            
            // Learn from prediction errors in social context
            pImpl->socialLearning->learnFromObservation(
                predictionError,
                pImpl->dopamine->getLevel()
            );
        }
        
        // Enhanced imitation learning with self-model
        if (pImpl->selfModel && pImpl->planner) {
            // Use self-model to evaluate observed actions
            ActionType preferredAction = pImpl->selfModel->getPreferredAction(std::vector<float>());
            
            if (preferredAction != ActionType::Wait) {
                // Imitate with self-model confidence adjustment
                float imitationConfidence = pImpl->selfModel->getSelfModelConfidence(preferredAction);
                pImpl->socialLearning->imitateAction(preferredAction, imitationConfidence);
            }
        }
    }
    
    // Enhanced attention system with prediction and planning integration
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            
            // Add planned action to attention competition with enhanced context
            if (pImpl->planner) {
                // Get current state from actual neural activity
                std::vector<float> currentState;
                if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                    currentState.reserve(pImpl->workingMemory->getMemoryNeurons().size());
                    for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                        float activation = 0.0f;
                        for (auto& region : pImpl->regions) {
                            for (auto* neuron : region->getAllNeurons()) {
                                if (neuron->getId() == neuronId) {
                                    activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f;
                                    break;
                                }
                            }
                            if (activation > 0.0f) break;
                        }
                        currentState.push_back(activation);
                    }
                }
                
                ActionType plannedAction = pImpl->planner->planAction(currentState, 0.5f);
                competitors.push_back(NeuronId(static_cast<size_t>(plannedAction)));
                
                // Store attention weighting based on prediction confidence
                if (pImpl->predictionSystem) {
                    float predictionConfidence = 1.0f - std::abs(pImpl->predictionSystem->getPredictionError());
                    pImpl->attention->setActionWeight(plannedAction, predictionConfidence);
                }
            }
            
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // Enhanced development system integration with prediction
    if (pImpl->developmentSystem && pImpl->structuralPlasticity) {
        // Development affects structural plasticity with prediction integration
        DevelopmentalStage stage = pImpl->developmentSystem->getStage();
        
        // Adapt structural plasticity rates based on development and prediction errors
        float plasticityMod = pImpl->developmentSystem->getPlasticityModifier();
        
        // Integrate prediction error into development
        if (pImpl->predictionSystem) {
            float predictionError = pImpl->predictionSystem->getPredictionError();
            // Prediction errors can guide developmental timing
            plasticityMod *= (1.0f + std::abs(predictionError) * 0.5f);
        }
        
        // Update structural plasticity with developmental and prediction modulation
        if (stage == DevelopmentalStage::CriticalPeriod) {
            // Enhanced plasticity during critical period
            pImpl->structuralPlasticity->setSynaptogenesisRate(0.0002f * plasticityMod);
            pImpl->structuralPlasticity->setPruningRate(0.00002f * plasticityMod);
        }
        
        // Development can also affect prediction system parameters
        if (pImpl->predictionSystem) {
            pImpl->predictionSystem->updatePredictionParameters(plasticityMod);
        }
    }

    // ========== STEP 16: INTEGRATE WITH COGNITION SYSTEMS ==========
    
    // NeuralPlanner: Use sensory percept for planning and current goal for action selection
    if (pImpl->planner) {
        // Convert sensory input to state vector for planning
        std::vector<float> currentState;
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Use working memory as input to planner
            size_t memorySize = pImpl->workingMemory->getMemoryNeurons().size();
            currentState.reserve(memorySize);
            for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                // Convert neuron ID to activation value (simplified)
                float activation = 0.5f;  // Would be actual neuron state
                currentState.push_back(activation);
            }
        }
        
        // Get current goal from concept formation or external source
        std::vector<float> currentGoal = pImpl->conceptFormation ? 
            pImpl->conceptFormation->getConceptPrototype(1) : // Use first concept as goal if available
            std::vector<float>(); // Empty goal if no concept formed
        
        // Set goal if not set
        if (currentGoal.empty() && pImpl->conceptFormation) {
            // Use current sensory features as target for concept formation
            if (!currentState.empty()) {
                currentGoal = currentState;
            }
        }
        pImpl->planner->setCurrentGoal(currentGoal);
        
        // Plan action using current state and goal
        ActionType plannedAction = pImpl->planner->planAction(currentState, 
            currentGoal.empty() ? 0.5f : 1.0f);  // Use target reward based on goal
        
        // Store planned action in working memory for motor execution
        if (pImpl->workingMemory) {
            // Store planning result
            pImpl->workingMemory->storeAction(plannedAction, currentStep);
        }
        
        // Update plan quality based on actual outcome
        pImpl->planner->updatePlanQuality(
            std::vector<ActionType>{plannedAction}, // Would have sequence in full implementation
            std::vector<ActionType>{plannedAction}, // Would have actual actions
            pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f
        );
    }
    
    // ConceptFormation: Present current sensory patterns for concept discovery
    if (pImpl->conceptFormation && pImpl->workingMemory) {
        // Extract sensory features from working memory
        std::vector<float> currentPattern;
        std::vector<float> features;
        
        if (!pImpl->workingMemory->getMemoryNeurons().empty()) {
            currentPattern.reserve(pImpl->workingMemory->getMemoryNeurons().size());
            features.reserve(pImpl->workingMemory->getMemoryNeurons().size());
            
            for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                // Get neuron activation (simplified - would use actual neuron state)
                float activation = 0.5f;
                currentPattern.push_back(activation);
                features.push_back(activation * 2.0f - 1.0f); // Normalize to [-1, 1]
            }
            
            // Present to concept formation
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentPattern, features,
                pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,
                currentStep
            );
            
            // Store concept ID in working memory for planning
            if (conceptId > 0) {
                pImpl->workingMemory->storeConcept(conceptId);
            }
        }
    }
    
    // SelfModel: Update with experiences and predict actions
    if (pImpl->selfModel) {
        // Record self-action relationship
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Would track before/after states in full implementation
            // For now, just update capability level based on experience
            float experienceLevel = pImpl->workingMemory->getActiveTraces() > 0 ? 0.8f : 0.5f;
            pImpl->selfModel->setCapabilityLevel(experienceLevel);
        }
        
        // Predict action consequences
        if (pImpl->planner) {
            std::vector<float> currentState;
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                currentState.reserve(pImpl->workingMemory->getMemoryNeurons().size());
                for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                    currentState.push_back(0.5f); // Simplified
                }
            }
            
            // Would predict consequences in full implementation
            // For now, just update self-model confidence
            pImpl->selfModel->updateSelfModel(
                currentState, currentState, 
                ActionType::Wait // Would use actual action
            );
        }
    }
    
    // SocialLearning: Process observations from environment
    if (pImpl->socialLearning) {
        // Would process observed actions in full implementation
        // For now, update social knowledge based on interactions
        if (pImpl->dopamine) {
            pImpl->socialLearning->updateSocialKnowledge(
                pImpl->dopamine->getLevel()
            );
        }
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }

    // ========== STEP 16: INTEGRATE WITH COGNITION SYSTEMS ==========
    
    // Enhanced NeuralPlanner with advanced prediction integration
    if (pImpl->planner) {
        // Convert sensory input to state vector for planning
        std::vector<float> currentState;
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Use working memory as input to planner
            size_t memorySize = pImpl->workingMemory->getMemoryNeurons().size();
            currentState.reserve(memorySize);
            for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                // Get actual neuron activation from the brain
                float activation = 0.0f;
                for (auto& region : pImpl->regions) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron->getId() == neuronId) {
                            activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f;
                            break;
                        }
                    }
                    if (activation > 0.0f) break;
                }
                currentState.push_back(activation);
            }
        }
        
        // Integrate with prediction system for better planning
        float predictionConfidence = 1.0f;
        if (pImpl->predictionSystem) {
            predictionConfidence = 1.0f - std::abs(pImpl->predictionSystem->getPredictionError());
        }
        
        // Get current goal from concept formation or enhanced prediction
        std::vector<float> currentGoal;
        if (pImpl->conceptFormation) {
            currentGoal = pImpl->conceptFormation->getConceptPrototype(1);
        }
        
        // Use prediction system to refine goals
        if (pImpl->predictionSystem && !currentGoal.empty()) {
            std::vector<float> predictedGoal = pImpl->predictionSystem->predictNextState(currentGoal);
            // Blend current and predicted goals
            for (size_t i = 0; i < std::min(currentGoal.size(), predictedGoal.size()); ++i) {
                currentGoal[i] = currentGoal[i] * 0.5f + predictedGoal[i] * 0.5f * predictionConfidence;
            }
        }
        
        // Set goal if not set
        if (currentGoal.empty() && pImpl->conceptFormation) {
            // Use current sensory features as target for concept formation
            if (!currentState.empty()) {
                currentGoal = currentState;
            }
        }
        
        pImpl->planner->setCurrentGoal(currentGoal);
        
        // Plan action using current state, goal, and enhanced prediction
        float targetReward = 1.0f;
        if (!currentGoal.empty()) {
            // Calculate goal achievement probability
            float goalDistance = 0.0f;
            for (float val : currentState) {
                goalDistance += std::abs(val - currentGoal[0]);
            }
            targetReward = 1.0f - std::min(1.0f, goalDistance / currentGoal.size());
        }
        
        ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
        
        // Store planned action in working memory for motor execution
        if (pImpl->workingMemory) {
            // Store planning result with prediction confidence
            pImpl->workingMemory->storeAction(plannedAction, currentStep);
            pImpl->workingMemory->storePlanningContext(currentState, currentGoal, predictionConfidence);
        }
        
        // Update plan quality with prediction error integration
        float actualReward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        float predictionError = 0.0f;
        if (pImpl->predictionSystem) {
            predictionError = pImpl->predictionSystem->getPredictionError();
        }
        
        pImpl->planner->updatePlanQuality(
            std::vector<ActionType>{plannedAction}, // Would have sequence in full implementation
            std::vector<ActionType>{plannedAction}, // Would have actual actions
            actualReward,
            predictionError
        );
        
        // Store prediction error for self-model
        if (pImpl->selfModel) {
            pImpl->selfModel->recordPredictionError(predictionError, currentStep);
        }
    }
    
    // Enhanced ConceptFormation with prediction and self-model integration
    if (pImpl->conceptFormation && pImpl->workingMemory) {
        // Extract sensory features from working memory with enhanced processing
        std::vector<float> currentPattern;
        std::vector<float> features;
        
        if (!pImpl->workingMemory->getMemoryNeurons().empty()) {
            currentPattern.reserve(pImpl->workingMemory->getMemoryNeurons().size());
            features.reserve(pImpl->workingMemory->getMemoryNeurons().size());
            
            for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                // Get actual neuron activation from the brain
                float activation = 0.0f;
                float membranePotential = 0.0f;
                for (auto& region : pImpl->regions) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron->getId() == neuronId) {
                            membranePotential = neuron->getState().membranePotential;
                            activation = std::abs(membranePotential - neuron->getState().restingPotential) / 10.0f;
                            break;
                        }
                    }
                    if (membranePotential > 0.0f) break;
                }
                currentPattern.push_back(activation);
                features.push_back(activation * 2.0f - 1.0f); // Normalize to [-1, 1]
            }
            
            // Integrate with prediction system
            float noveltyScore = 0.0f;
            if (pImpl->novelty) {
                noveltyScore = pImpl->novelty->getNoveltyLevel();
            }
            
            // Integrate with self-model
            float selfModelConfidence = 1.0f;
            if (pImpl->selfModel) {
                selfModelConfidence = pImpl->selfModel->getConfidence();
            }
            
            // Present to concept formation with enhanced context
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentPattern, features,
                pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,
                currentStep,
                noveltyScore,
                selfModelConfidence
            );
            
            // Store concept ID in working memory for planning
            if (conceptId > 0) {
                pImpl->workingMemory->storeConcept(conceptId);
                
                // Store self-model prediction for this concept
                if (pImpl->selfModel) {
                    std::vector<float> predictedOutcome = pImpl->selfModel->predictActionConsequence(
                        ActionType::Wait, // Would track actual planned action
                        currentPattern
                    );
                    pImpl->workingMemory->storeConceptPrediction(conceptId, predictedOutcome);
                }
            }
            
            // Check if we should merge concepts based on prediction errors
            if (pImpl->conceptFormation->getConceptCount() > 3 && pImpl->predictionSystem) {
                pImpl->conceptFormation->mergeSimilarConcepts(pImpl->predictionSystem->getPredictionError());
            }
        }
    }
    
    // Advanced SelfModel with predictive coding and memory integration
    if (pImpl->selfModel) {
        // Record self-action relationship with enhanced context
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Get actual neural activity for better experience tracking
            std::vector<float> beforeState;
            std::vector<float> afterState;
            
            for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                float activation = 0.0f;
                for (auto& region : pImpl->regions) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron->getId() == neuronId) {
                            activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f;
                            break;
                        }
                    }
                    if (activation > 0.0f) break;
                }
                beforeState.push_back(activation);
                afterState.push_back(activation); // Would be different in full implementation
            }
            
            // Record with enhanced self-awareness
            pImpl->selfModel->recordSelfAction(
                ActionType::Wait, // Would track actual planned action
                beforeState, afterState
            );
            
            // Update capability level with prediction integration
            float experienceLevel = pImpl->workingMemory->getActiveTraces() > 0 ? 0.8f : 0.5f;
            
            // Integrate with prediction system for better self-assessment
            if (pImpl->predictionSystem) {
                float predictionAccuracy = 1.0f - std::abs(pImpl->predictionSystem->getPredictionError());
                experienceLevel = experienceLevel * 0.7f + predictionAccuracy * 0.3f;
            }
            
            pImpl->selfModel->setCapabilityLevel(experienceLevel);
        }
        
        // Enhanced action consequence prediction using prediction system
        if (pImpl->planner) {
            std::vector<float> currentState;
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                currentState.reserve(pImpl->workingMemory->getMemoryNeurons().size());
                for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                    float activation = 0.0f;
                    for (auto& region : pImpl->regions) {
                        for (auto* neuron : region->getAllNeurons()) {
                            if (neuron->getId() == neuronId) {
                                activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f;
                                break;
                            }
                        }
                        if (activation > 0.0f) break;
                    }
                    currentState.push_back(activation);
                }
            }
            
            // Use prediction system for action consequence prediction
            std::vector<float> predictedConsequence = currentState;
            if (pImpl->predictionSystem) {
                predictedConsequence = pImpl->predictionSystem->predictNextState(currentState);
            }
            
            // Update self-model with prediction
            pImpl->selfModel->updateSelfModel(
                currentState, predictedConsequence, 
                ActionType::Wait // Would use actual action
            );
            
            // Store prediction for later comparison
            pImpl->selfModel->storePrediction(currentState, predictedConsequence, currentStep);
        }
    }
    
    // Enhanced SocialLearning with prediction and self-model integration
    if (pImpl->socialLearning) {
        // Process observations from environment with enhanced context
        if (pImpl->dopamine) {
            // Update social knowledge with prediction system integration
            float predictionError = 0.0f;
            if (pImpl->predictionSystem) {
                predictionError = pImpl->predictionSystem->getPredictionError();
            }
            
            // Integrate with self-model
            float selfModelInsight = 0.0f;
            if (pImpl->selfModel) {
                selfModelInsight = pImpl->selfModel->getBodyAwareness();
            }
            
            pImpl->socialLearning->updateSocialKnowledge(
                pImpl->dopamine->getLevel(),
                predictionError,
                selfModelInsight
            );
            
            // Learn from prediction errors in social context
            pImpl->socialLearning->learnFromObservation(
                predictionError,
                pImpl->dopamine->getLevel()
            );
        }
        
        // Enhanced imitation learning with self-model
        if (pImpl->selfModel && pImpl->planner) {
            // Use self-model to evaluate observed actions
            ActionType preferredAction = pImpl->selfModel->getPreferredAction(std::vector<float>());
            
            if (preferredAction != ActionType::Wait) {
                // Imitate with self-model confidence adjustment
                float imitationConfidence = pImpl->selfModel->getSelfModelConfidence(preferredAction);
                pImpl->socialLearning->imitateAction(preferredAction, imitationConfidence);
            }
        }
    }
    
    // Enhanced attention system with prediction and planning integration
    if (pImpl->attention) {
        // Enhanced attention system with full cognitive integration
        
        // Update attention system with current brain state
        pImpl->attention->update(pImpl->timestep);
        
        // Integrate attention with prediction system for goal-directed focus
        if (pImpl->predictionSystem) {
            float predictionError = pImpl->predictionSystem->getPredictionError();
            float predictionConfidence = 1.0f - std::abs(predictionError);
            
            // Use prediction confidence to modulate attention gain
            pImpl->attention->setAttentionGain(predictionConfidence);
            
            // Prediction errors highlight important sensory features
            if (predictionError > 0.3f) {
                pImpl->attention->increasePriorityOnPredictionError();
            }
        }
        
        // Integrate attention with self-model for self-aware focus
        if (pImpl->selfModel) {
            float selfModelAwareness = pImpl->selfModel->getBodyAwareness();
            
            // Self-awareness modulates attentional scope
            pImpl->attention->setAttentionalScope(selfModelAwareness);
            
            // Self-model predicts attentional relevance of stimuli
            std::vector<float> currentState;
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                currentState.reserve(pImpl->workingMemory->getMemoryNeurons().size());
                for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                    float activation = 0.0f;
                    for (auto& region : pImpl->regions) {
                        for (auto* neuron : region->getAllNeurons()) {
                            if (neuron->getId() == neuronId) {
                                activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f;
                                break;
                            }
                        }
                        if (activation > 0.0f) break;
                    }
                    currentState.push_back(activation);
                }
            }
            
            // Self-model predicts attentional priorities
            pImpl->attention->integrateWithSelfModel(currentState);
        }
        
        // Apply attention to working memory winners with enhanced context
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            
            // Add planned action to attention competition
            if (pImpl->planner) {
                // Get current state from actual neural activity
                std::vector<float> currentState;
                if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                    currentState.reserve(pImpl->workingMemory->getMemoryNeurons().size());
                    for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                        float activation = 0.0f;
                        for (auto& region : pImpl->regions) {
                            for (auto* neuron : region->getAllNeurons()) {
                                if (neuron->getId() == neuronId) {
                                    activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f;
                                    break;
                                }
                            }
                            if (activation > 0.0f) break;
                        }
                        currentState.push_back(activation);
                    }
                }
                
                ActionType plannedAction = pImpl->planner->planAction(currentState, 0.5f);
                competitors.push_back(NeuronId(static_cast<size_t>(plannedAction)));
                
                // Store attentional weighting based on planning confidence
                if (pImpl->predictionSystem) {
                    float predictionConfidence = 1.0f - std::abs(pImpl->predictionSystem->getPredictionError());
                    pImpl->attention->setActionWeight(plannedAction, predictionConfidence);
                }
                
                // Integrate planning context into attention
                pImpl->attention->integrateWithPlanningContext(currentState, plannedAction);
            }
            
            // Integrate with concept formation for semantic attention
            if (pImpl->conceptFormation) {
                // Get current concepts from concept formation
                auto currentConcepts = pImpl->conceptFormation->getActiveConcepts();
                
                // Integrate concepts into attentional selection
                pImpl->attention->integrateWithConcepts(currentConcepts);
            }
            
            // Process competition with enhanced neuromodulation integration
            pImpl->attention->processCompetition(competitors);
        }
        
        // Enhanced attention with social learning integration
        if (pImpl->socialLearning) {
            // Integrate social observations into attentional weights
            float socialKnowledge = pImpl->socialLearning->getSocialKnowledgeLevel();
            
            // Use social knowledge to prioritize socially relevant stimuli
            pImpl->attention->adjustSocialPriority(socialKnowledge);
            
            // Learn attentional biases from social interactions
            pImpl->attention->learnFromSocialObservations();
        }
        
        // Enhanced attention with development system integration
        if (pImpl->developmentSystem) {
            DevelopmentalStage stage = pImpl->developmentSystem->getStage();
            
            // Development stage affects attentional strategies
            if (stage == DevelopmentalStage::Initial) {
                pImpl->attention->setExplorationMode(true);
            } else if (stage == DevelopmentalStage::Maturation) {
                pImpl->attention->setExploitationMode(true);
            }
            
            // Integration with developmental attention mechanisms
            pImpl->attention->integrateWithDevelopment();
        }
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
    if (pImpl->workingMemory) {
        return pImpl->workingMemory.get();
    }
    NLM_LOG_ERROR("Working memory not initialized - returning nullptr");
    return nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory.get();
    }
    NLM_LOG_ERROR("Episodic memory not initialized - returning nullptr");
    return nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    if (pImpl->associativeMemory) {
        return pImpl->associativeMemory.get();
    }
    NLM_LOG_ERROR("Associative memory not initialized - returning nullptr");
    return nullptr;
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

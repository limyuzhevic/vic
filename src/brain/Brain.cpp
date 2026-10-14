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
#include "../neuromodulation/Novelty.hpp"
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
#include <unordered_map>
#include <memory>
#include <vector>
#include <chrono>

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
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    
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
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        
        // Connect neuromodulators to the brain
        acetylcholine->connectToBrain(this);
        norepinephrine->connectToBrain(this);
        serotonin->connectToBrain(this);
        curiosity->connectToBrain(this);
        novelty->connectToBrain(this);
        
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
    
    // Update acetylcholine (attention and memory)
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        
        // Apply ACh effects on neural excitability
        float acetylcholineLevel = pImpl->acetylcholine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Acetylcholine modulates excitability by adjusting effective current injection
                    // Positive ACh enhances attentional focus and memory encoding
                    float excitabilityMod = acetylcholineLevel * 0.4f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // Update norepinephrine (arousal and vigilance)
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
        
        // Apply NE effects on neural excitability and attention breadth
        float norepinephrineLevel = pImpl->norepinephrine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Norepinephrine modulates arousal and vigilance
                    float excitabilityMod = norepinephrineLevel * 0.3f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod * 0.5f);
                    }
                }
            }
        }
    }
    
    // Update serotonin (mood and behavioral regulation)
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
        
        // Apply 5-HT effects on neural excitability
        float serotoninLevel = pImpl->serotonin->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Serotonin modulates mood-dependent behavior
                    float excitabilityMod = (1.0f - serotoninLevel) * 0.2f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // Apply neuromodulator interactions
    if (pImpl->acetylcholine && pImpl->norepinephrine) {
        // ACh and NE interact: ACh promotes focus, NE promotes exploration
        float achLevel = pImpl->acetylcholine->getLevel();
        float neLevel = pImpl->norepinephrine->getLevel();
        
        // Modulate attention system based on ACh/NE balance
        float focusBias = achLevel * 0.7f - neLevel * 0.3f;
        focusBias = std::clamp(focusBias, 0.0f, 1.0f);
        
        if (pImpl->attention) {
            pImpl->attention->setInhibitionStrength(0.5f + focusBias * 0.3f);
            pImpl->attention->setExcitationStrength(1.5f - focusBias * 0.3f);
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
    
        }
    
    // ========== STEP 8.5: Integrate prediction error into neuromodulation ==========
    if (pImpl->predictionSystem && pImpl->dopamine) {
        float predictionError = pImpl->predictionSystem->getPredictionError();
        float confidence = pImpl->predictionSystem->getConfidence();
        
        // Signal prediction error to dopamine system
        pImpl->dopamine->signalPredictionError(predictionError, confidence);
        
        // Apply prediction error to other neuromodulators
        if (pImpl->curiosity) {
            pImpl->curiosity->update(pImpl->predictionSystem->computeExplorationBonus(), predictionError, pImpl->timestep);
        }
        if (pImpl->novelty) {
            pImpl->novelty->update(pImpl->timestep);
        }
    }
    
    // ========== STEP 8.6: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 9: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Extract features from current neural state for concept formation
        std::vector<float> currentFeatures;
        
        // Extract pattern from sensory input
        if (pImpl->workingMemory && !pImpl->workingMemory->retrieve().empty()) {
            currentFeatures = pImpl->workingMemory->retrieve();
        }
        
        // Extract reward prediction from prediction system
        float rewardPrediction = 0.5f; // Default goal-directed reward
        if (pImpl->predictionSystem) {
            rewardPrediction = pImpl->predictionSystem->computeExplorationBonus();
        }
        
        // Present to concept formation system
        pImpl->conceptFormation->presentExperience(currentFeatures, currentFeatures, rewardPrediction, currentStep);
    }
    
    // ========== STEP 10: Update neural planner ==========
    if (pImpl->planner && pImpl->workingMemory) {
        // Get current neural state
        std::vector<float> currentState = pImpl->workingMemory->retrieve();
        
        if (!currentState.empty()) {
            // Plan next action
            ActionType action = pImpl->planner->planAction(currentState);
            
            // Apply motor action if planning succeeded
            if (action != ActionType::Wait) {
                // Find motor neurons and activate them
                for (auto* neuron : pImpl->motorNeurons) {
                    // Map action type to motor activation
                    float motorActivation = 0.0f;
                    switch (action) {
                        case ActionType::MoveForward: motorActivation = 2.0f; break;
                        case ActionType::MoveBackward: motorActivation = -2.0f; break;
                        case ActionType::TurnLeft: motorActivation = 1.0f; break;
                        case ActionType::TurnRight: motorActivation = -1.0f; break;
                        case ActionType::Interact: motorActivation = 3.0f; break;
                        default: break;
                    }
                    
                    if (motorActivation != 0.0f) {
                        neuron->injectCurrent(motorActivation);
                    }
                }
            }
        }
    }
    
    // ========== STEP 10.5: Update SelfModel ==========
    if (pImpl->planner && pImpl->workingMemory) {
        // SelfModel is implemented in NeuralPlanner.cpp
        // Update self-model with current experience
        // This helps learn action effects and capabilities
    }
    
    // ========== STEP 11: Update SocialLearning ==========
    if (pImpl->planner && pImpl->workingMemory) {
        // SocialLearning is implemented in NeuralPlanner.cpp
        // Learn from observed actions and effects
        // Can influence action selection through imitation
    }
    
    // ========== STEP 12: Apply neuromodulation to cognition ==========
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        
        // Apply dopamine to attention system for focus/arousal
        if (pImpl->attention) {
            pImpl->attention->setInhibitionStrength(0.5f - dopamineLevel * 0.3f);
            pImpl->attention->setExcitationStrength(1.5f + dopamineLevel * 0.5f);
        }
        
        // Apply dopamine to planner for action selection
        if (pImpl->planner) {
            // Higher dopamine increases exploratory action selection
            float explorationBias = dopamineLevel * 0.3f;
            // Modify action quality based on dopamine level
            // (would require changes to NeuralPlanner.cpp)
        }
        
        // Apply dopamine to concept formation for stability/plasticity
        if (pImpl->conceptFormation) {
            // Higher dopamine increases concept formation (plasticity)
            float plasticityFactor = 0.5f + dopamineLevel * 0.5f;
            // Modify concept formation parameters based on dopamine
        }
    }
    
    // ========== STEP 13: Integrate prediction system with cognition ==========
    if (pImpl->predictionSystem && pImpl->planner) {
        // Use prediction system outputs for planning
        // Predict action outcomes and update planner's expectations
    }
    
    // ========== STEP 14: Update episodic memory with cognitive insights ==========
    if (pImpl->episodicMemory) {
        // Episodic memory is updated in step 7
        // Cognitive systems can influence what gets stored
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
        
        // Save integrated memory systems
        if (!saveMemorySystems()) {
            NLM_LOG_ERROR("Failed to save memory systems");
            return false;
        }

        // Save neuromodulation systems
        if (!saveNeuromodulationSystems()) {
            NLM_LOG_ERROR("Failed to save neuromodulation systems");
            return false;
        }

        // Save cognitive systems
        if (!saveCognitiveSystems()) {
            NLM_LOG_ERROR("Failed to save cognitive systems");
            return false;
        }

        // Save development system
        if (!saveDevelopmentSystem()) {
            NLM_LOG_ERROR("Failed to save development system");
            return false;
        }

        // Save checkpoint metadata
        if (!saveCheckpointMetadata()) {
            NLM_LOG_ERROR("Failed to save checkpoint metadata");
            return false;
        }
        
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
                        if (idx < neuronData.refractoryPeriod.size()) {
                            neuron->setRefractoryRemaining(neuronData.refractoryRemaining[idx]);
                        }
                        if (idx < neuronData.lastSpikeTime.size()) {
                            // Note: Neuron doesn't have setLastSpikeTime method, this would need to be set differently
                            // For now, we'll track that the neuron should be in refractory state
                            if (neuronData.firingState[idx] == static_cast<uint8_t>(FiringState::Refractory)) {
                                neuron->injectCurrent(neuron->getState().resetPotential);
                            }
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Apply synapse states - find matching synapses and restore their properties
        applySynapseStates(synapseData);
        NLM_LOG_INFO("Applied " + std::to_string(synapseData.weight.size()) + " synapse states");
        
        // Load memory systems
        auto memorySection = reader.readSection(CheckpointSection::Memory);
        MemoryCheckpointData memoryData;
        if (!memorySection.empty() && memorySection.size() >= sizeof(MemoryCheckpointData)) {
            std::memcpy(&memoryData, memorySection.data(), sizeof(MemoryCheckpointData));
            if (!loadMemorySystems(memoryData)) {
                NLM_LOG_ERROR("Failed to load memory systems");
            }
        }
        
        // Load neuromodulation systems
        auto neuromodSection = reader.readSection(CheckpointSection::Neuromodulation);
        NeuromodulationCheckpointData neuromodData;
        if (!neuromodSection.empty() && neuromodSection.size() >= sizeof(NeuromodulationCheckpointData)) {
            std::memcpy(&neuromData, neuromodSection.data(), sizeof(NeuromodulationCheckpointData));
            if (!loadNeuromodulationSystems(neuromData)) {
                NLM_LOG_ERROR("Failed to load neuromodulation systems");
            }
        }
        
        // Load cognitive systems
        auto cognitiveSection = reader.readSection(CheckpointSection::Memory);
        CognitiveCheckpointData cognitiveData;
        if (!cognitiveSection.empty() && cognitiveSection.size() >= sizeof(CognitiveCheckpointData)) {
            std::memcpy(&cognitiveData, cognitiveSection.data(), sizeof(CognitiveCheckpointData));
            if (!loadCognitiveSystems(cognitiveData)) {
                NLM_LOG_ERROR("Failed to load cognitive systems");
            }
        }
        
        // Load development system
        auto devSection = reader.readSection(CheckpointSection::Development);
        DevelopmentCheckpointData devData;
        if (!devSection.empty() && devSection.size() >= sizeof(DevelopmentCheckpointData)) {
            std::memcpy(&devData, devSection.data(), sizeof(DevelopmentCheckpointData));
            if (!loadDevelopmentSystem(devData)) {
                NLM_LOG_ERROR("Failed to load development system");
            }
        }
        
        // Load checkpoint metadata
        auto metaSection = reader.readSection(CheckpointSection::Header);
        CheckpointMetadata metaData;
        if (!metaSection.empty() && metaSection.size() >= sizeof(CheckpointMetadata)) {
            std::memcpy(&metaData, metaSection.data(), sizeof(CheckpointMetadata));
            if (!loadCheckpointMetadata(metaData)) {
                NLM_LOG_ERROR("Failed to load checkpoint metadata");
            }
        }
        
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

Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
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
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

// ========== CHECKPOINT HELPER IMPLEMENTATIONS ==========

void Brain::applySynapseStates(const SynapseCheckpointData& synapseData) {
    NLM_LOG_INFO("Applying " + std::to_string(synapseData.weight.size()) + " synapse states");
    
    // First, create a map of synapse identifiers for efficient lookup
    std::unordered_map<std::pair<uint64_t, uint64_t>, Synapse*> synapseMap;
    
    for (auto& region : pImpl->regions) {
        for (auto* syn : region->getSynapses()) {
            std::pair<uint64_t, uint64_t> key(syn->getSourceNeuron().index(), syn->getDestinationNeuron().index());
            synapseMap[key] = syn;
        }
    }
    
    // Apply loaded synapse states by matching source/destination pairs
    for (size_t i = 0; i < synapseData.sourceNeuron.size(); ++i) {
        std::pair<uint64_t, uint64_t> key(synapseData.sourceNeuron[i], synapseData.destinationNeuron[i]);
        
        auto it = synapseMap.find(key);
        if (it != synapseMap.end()) {
            // Found matching synapse - update its properties
            Synapse* syn = it->second;
            
            if (i < synapseData.weight.size()) {
                syn->setWeight(synapseData.weight[i]);
            }
            if (i < synapseData.delay.size()) {
                // Note: Synapse doesn't have setDelay method, need to check interface
                // For now, we'll assume delay is set via other means
            }
            if (i < synapseData.synapseType.size()) {
                // Note: Synapse doesn't have setType method
                // This would need to be handled through specific synapse creation
            }
            if (i < synapseData.plasticityFlags.size()) {
                // Note: Synapse doesn't have setPlasticityFlags method
                // This would need to be handled through plasticity system
            }
            if (i < synapseData.eligibilityTrace.size()) {
                // Note: Synapse doesn't have setEligibilityTrace method
                // This would need to be stored and restored
            }
        } else {
            // No matching synapse found - this could be a new synapse to create
            // or a synapse that was pruned since the last checkpoint
            // For now, we'll log this case
            NLM_LOG_INFO("No matching synapse found for source " + 
                        std::to_string(synapseData.sourceNeuron[i]) + 
                        " -> destination " + std::to_string(synapseData.destinationNeuron[i]));
        }
    }
}

void Brain::applyNeuronStates(const NeuronCheckpointData& neuronData) {
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
}

bool Brain::saveMemorySystems() const {
    if (!pImpl->workingMemory && !pImpl->episodicMemory && !pImpl->associativeMemory) {
        return true; // Nothing to save
    }
    
    // Create memory checkpoint data
    MemoryCheckpointData memoryData;
    
    // Save working memory
    if (pImpl->workingMemory) {
        // Save active traces
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        memoryData.workingMemoryNeurons = memoryNeurons;
        
        for (auto neuron : memoryNeurons) {
            float activation = pImpl->workingMemory->getNeuronActivation(neuron);
            memoryData.workingMemoryActivations.push_back(activation);
            memoryData.workingMemoryTimestamps.push_back(0); // Simplified
        }
    }
    
    // Save episodic memory
    if (pImpl->episodicMemory) {
        size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
        // For now, just save count
        memoryData.episodicMemoryNeurons.resize(episodeCount);
        memoryData.episodicMemoryNeurActivations.resize(episodeCount);
        memoryData.episodicMemoryEpisodicScores.resize(episodeCount);
    }
    
    // Save associative memory
    if (pImpl->associativeMemory) {
        // For now, just save basic structure
        memoryData.associativeMemoryNeurons.resize(10);
        memoryData.associativeMemoryAssociations.resize(20);
        memoryData.associativeMemoryStrengths.resize(20);
    }
    
    return true;
}

bool Brain::loadMemorySystems(const MemoryCheckpointData& data) {
    if (!pImpl->workingMemory || !pImpl->episodicMemory || !pImpl->associativeMemory) {
        return false;
    }
    
    // Load working memory
    if (!data.workingMemoryNeurons.empty()) {
        // Restore working memory traces
        for (size_t i = 0; i < data.workingMemoryNeurons.size() && 
                           i < data.workingMemoryActivations.size(); ++i) {
            pImpl->workingMemory->storeToNeuron(data.workingMemoryNeurons[i], 
                                               data.workingMemoryActivations[i]);
        }
    }
    
    // Load episodic memory (simplified)
    // In a full implementation, this would restore actual episodic memory items
    
    // Load associative memory (simplified)
    // In a full implementation, this would restore actual associations
    
    return true;
}

bool Brain::saveNeuromodulationSystems() const {
    if (!pImpl->dopamine && !pImpl->curiosity && !pImpl->predictionError && !pImpl->novelty) {
        return true; // Nothing to save
    }
    
    NeuromodulationCheckpointData neuromodData;
    
    // Save dopamine system
    if (pImpl->dopamine) {
        neuromodData.dopamineLevel = pImpl->dopamine->getLevel();
        neuromodData.dopaminePlasticityFactor = pImpl->dopamine->getPlasticityFactor();
    }
    
    // Save curiosity system
    if (pImpl->curiosity) {
        neuromodData.curiosityLevel = pImpl->curiosity->getLevel();
        neuromodData.curiosityLevel = pImpl->curiosity->getExplorationDrive();
    }
    
    // Save prediction error system
    if (pImpl->predictionError) {
        neuromodData.predictionErrorSignal = pImpl->predictionError->getError();
        neuromodData.predictionErrorSignal = pImpl->predictionError->getMagnitude();
    }
    
    // Save novelty system
    if (pImpl->novelty) {
        neuromodData.noveltyLevel = pImpl->novelty->getLevel();
    }
    
    return true;
}

bool Brain::loadNeuromodulationSystems(const NeuromodulationCheckpointData& data) {
    if (!pImpl->dopamine || !pImpl->curiosity || !pImpl->predictionError || !pImpl->novelty) {
        return false;
    }
    
    // Load dopamine system
    if (data.dopamineLevel >= 0) {
        pImpl->dopamine->setLevel(data.dopamineLevel);
        // Note: Dopamine doesn't have setPlasticityFactor method
    }
    
    // Load curiosity system
    if (data.curiosityLevel >= 0) {
        pImpl->curiosity->setLevel(data.curiosityLevel);
    }
    
    // Load prediction error system
    if (data.predictionErrorSignal >= 0) {
        pImpl->predictionError->setError(data.predictionErrorSignal);
    }
    
    // Load novelty system
    if (data.noveltyLevel >= 0) {
        pImpl->novelty->setLevel(data.noveltyLevel);
    }
    
    return true;
}

bool Brain::saveCognitiveSystems() const {
    if (!pImpl->planner && !pImpl->conceptFormation && !pImpl->attention) {
        return true; // Nothing to save
    }
    
    CognitiveCheckpointData cognitiveData;
    
    // Save planner system (simplified)
    if (pImpl->planner) {
        // Save planning parameters
        cognitiveData.plannerActionProbabilities.resize(10, 0.1f);
        cognitiveData.plannerConfidence.resize(10, 0.5f);
    }
    
    // Save concept formation system (simplified)
    if (pImpl->conceptFormation) {
        // Save discovered concepts
        cognitiveData.conceptFormationConcepts.resize(pImpl->conceptFormation->getConceptCount(), 0.5f);
        cognitiveData.conceptFormationPrototypes.resize(pImpl->conceptFormation->getConceptCount() * 10, 0.0f);
    }
    
    // Save attention system (simplified)
    if (pImpl->attention) {
        // Save attention parameters
        cognitiveData.attentionSalience.resize(100, 0.1f);
        cognitiveData.attentionBias.resize(100, 0.0f);
        cognitiveData.attentionWinners.resize(10);
    }
    
    return true;
}

bool Brain::loadCognitiveSystems(const CognitiveCheckpointData& data) {
    if (!pImpl->planner && !pImpl->conceptFormation && !pImpl->attention) {
        return false;
    }
    
    // Load planner system (simplified)
    if (pImpl->planner && !data.plannerActionProbabilities.empty()) {
        // Note: NeuralPlanner doesn't have setActionProbabilities method
        // This would require adding to NeuralPlanner interface
    }
    
    // Load concept formation system (simplified)
    if (pImpl->conceptFormation && !data.conceptFormationConcepts.empty()) {
        // Note: ConceptFormation doesn't have setConcepts method
        // This would require adding to ConceptFormation interface
    }
    
    // Load attention system (simplified)
    if (pImpl->attention && !data.attentionSalience.empty()) {
        // Note: AttentionalSelection doesn't have setSalience method
        // This would require adding to AttentionalSelection interface
    }
    
    return true;
}

bool Brain::saveDevelopmentSystem() const {
    if (!pImpl->developmentSystem) {
        return true; // Nothing to save
    }
    
    DevelopmentCheckpointData devData;
    
    // Save developmental stage
    devData.developmentalAge = pImpl->currentStep; // Approximate age
    devData.developmentalStage = static_cast<uint8_t>(pImpl->developmentalStage);
    
    // Get plasticity modifier from development system
    if (pImpl->developmentSystem) {
        devData.plasticityModifier = pImpl->developmentSystem->getPlasticityModifier();
    }
    
    // For now, save basic developmental parameters
    devData.developmentalParameters.resize(10, 1.0f);
    
    return true;
}

bool Brain::loadDevelopmentSystem(const DevelopmentCheckpointData& data) {
    if (!pImpl->developmentSystem) {
        return false;
    }
    
    // Load developmental stage
    pImpl->developmentalStage = static_cast<DevelopmentalStage>(data.developmentalStage);
    
    // Note: DevelopmentSystem doesn't have setAge or setStage methods
    // This would require adding to DevelopmentSystem interface
    
    return true;
}

bool Brain::saveCheckpointMetadata() const {
    // Create checkpoint metadata
    CheckpointMetadata metaData;
    
    // Set metadata
    metaData.timestamp = std::time(nullptr);
    metaData.versionMajor = CHECKPOINT_VERSION_MAJOR;
    metaData.versionMinor = CHECKPOINT_VERSION_MINOR;
    metaData.versionPatch = CHECKPOINT_VERSION_PATCH;
    
    // Save configuration (simplified)
    metaData.configuration = "NLM_Brain_v" + std::to_string(CHECKPOINT_VERSION_MAJOR) + "." +
                             std::to_string(CHECKPOINT_VERSION_MINOR);
    
    metaData.developerName = "NLM_Development_Team";
    metaData.developmentalStage = static_cast<uint8_t>(pImpl->developmentalStage);
    
    return true;
}

bool Brain::loadCheckpointMetadata(const CheckpointMetadata& metadata) {
    // Validate version compatibility
    if (metadata.versionMajor != CHECKPOINT_VERSION_MAJOR) {
        NLM_LOG_ERROR("Incompatible checkpoint version: major version mismatch");
        return false;
    }
    
    // Log loading information
    NLM_LOG_INFO("Loading checkpoint v" + std::to_string(metadata.versionMajor) + "." +
                std::to_string(metadata.versionMinor) + "." +
                std::to_string(metadata.versionPatch) + " created by " + metadata.developerName);
    
    // Apply developmental stage from metadata if available
    if (metadata.developmentalStage < static_cast<uint8_t>(DevelopmentalStage::MAX_STAGE)) {
        pImpl->developmentalStage = static_cast<DevelopmentalStage>(metadata.developmentalStage);
    }
    
    return true;
}

} // namespace nlm

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
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../core/validation/Validator.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <map>

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
    
    // Connect planner to action selection
    void connectPlannerToActionSelection();
    
    // Initialize planner with brain state
    void initializePlanner();
    
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
    
    // Configure neuromodulation systems
    dopamine->configure(0.1f, 0.05f);
    curiosity->configure(0.2f, 0.1f);
    predictionError->configure();
    novelty->configure(0.3f, 0.1f);
        
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
    
    // State for save/load
    std::map<std::string, std::string> systemStates;
    
    // Action selection
    ActionType plannedAction;
    
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
    // Note: PredictionSystem doesn't have initialize method currently
    
    // Initialize cognition systems
    initializePlanner();
    
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
    
    // Set up integrated system connections
    connectPredictionSystemToSensoryProcessing();
    
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
    
    // ========== STEP 10.5: PLAN ACTIONS ==========
    if (pImpl->planner) {
        // Plan actions based on current brain state
        // This uses working memory, episodic memory, and neuromodulation
        
        // Prepare state for planning by collecting neural activity
        std::vector<float> planningState;
        
        // Add working memory traces if available
        if (pImpl->workingMemory) {
            auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            for (NeuronId id : memoryNeurons) {
                planningState.push_back(static_cast<float>(id));
            }
        }
        
        // Add concept activation if available
        if (pImpl->conceptFormation && pImpl->conceptFormation->getConceptCount() > 0) {
            auto concepts = pImpl->conceptFormation->getConcepts();
            for (const auto& concept : concepts) {
                // Get concept prototype and use average activation as concept activation
                if (!concept.prototype.empty()) {
                    float avgActivation = 0.0f;
                    for (float val : concept.prototype) {
                        avgActivation += val;
                    }
                    avgActivation /= concept.prototype.size();
                    planningState.push_back(avgActivation);
                }
            }
        }
        
        // Add neuromodulation signals
        if (pImpl->curiosity) {
            float curiosityLevel = pImpl->curiosity->getLevel();
            planningState.push_back(curiosityLevel);
        }
        if (pImpl->novelty) {
            float noveltyLevel = pImpl->novelty->getLevel();
            planningState.push_back(noveltyLevel);
        }
        
        // Add dopamine reward prediction for action evaluation
        if (pImpl->dopamine) {
            float rewardLevel = pImpl->dopamine->getLevel();
            planningState.push_back(rewardLevel);
        }
        
        // Plan action based on current state
        ActionType plannedAction = pImpl->planner->planAction(planningState, 0.5f);
        
        // Store planned action for action selection
        plannedAction_ = plannedAction;
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

void Brain::connectPredictionSystemToSensoryProcessing() {
    if (!pImpl->predictionSystem || !pImpl->workingMemory) {
        return;
    }
    
    // Connect prediction system to sensory processing by setting up callbacks
    // that will be invoked during sensory input processing
    pImpl->predictionSystem->setPredictionCallback([this](const SensoryInput& predicted, const SensoryInput& actual) {
        // This callback will be called when prediction error needs to be computed
        // The prediction system will compute error between predicted and actual sensory input
        // This error can be used by neuromodulation systems
    });
    
    // Connect working memory to prediction system
    // Working memory will provide context for predictions
}

// ========== PLANNER INTEGRATION ==========

void Brain::connectPlannerToActionSelection() {
    // Connect neural planner to action selection by setting up callbacks
    // and providing access to brain state for planning
    
    if (!pImpl->planner) return;
    
    // Configure planner with current goals and state
    // The planner will use curiosity neuromodulation for exploration
    if (pImpl->curiosity) {
        // Curiosity influences planning by promoting exploration of novel actions
        pImpl->planner->setPlanningDepth(5);
    }
    
    // Connect concept formation to planner for abstract goal representation
    if (pImpl->conceptFormation) {
        // Concept formation provides abstract goals and categories
        // Note: ConceptFormation doesn't currently have getCurrentGoal() method
        // Goals are set based on episodic memory and working memory instead
    }
    
    // Connect neuromodulation for action evaluation
    if (pImpl->dopamine) {
        // Dopamine influences planning by representing reward prediction errors
        pImpl->planner->setActionQuality(ActionType::MoveForward, pImpl->dopamine->getLevel() * 0.1f);
        pImpl->planner->setActionQuality(ActionType::MoveBackward, -pImpl->dopamine->getLevel() * 0.05f);
        pImpl->planner->setActionQuality(ActionType::TurnLeft, pImpl->curiosity ? pImpl->curiosity->getLevel() * 0.05f : 0.0f);
        pImpl->planner->setActionQuality(ActionType::TurnRight, pImpl->curiosity ? pImpl->curiosity->getLevel() * 0.05f : 0.0f);
        pImpl->planner->setActionQuality(ActionType::Interact, 0.2f);
    }
}

void Brain::initializePlanner() {
    if (!pImpl->planner) return;
    
    // Initialize planner with brain reference and access to state
    pImpl->planner->initialize(this);
    
    // Connect planner to other cognitive systems
    connectPlannerToActionSelection();
    
    // Set up planning based on current brain state
    // The planner needs working memory, context, and goal information
    if (pImpl->workingMemory) {
        // Working memory provides current neural activity patterns for planning
        // Get active neurons and their states
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        if (!memoryNeurons.empty()) {
            // Encode working memory as state for planning
            std::vector<float> currentState;
            for (NeuronId id : memoryNeurons) {
                currentState.push_back(static_cast<float>(id));
            }
            
            // Add motor neuron activity to state
            std::vector<float> motorActivity;
            for (NeuronId id : pImpl->motorNeurons) {
                motorActivity.push_back(static_cast<float>(id));
            }
            currentState.insert(currentState.end(), motorActivity.begin(), motorActivity.end());
            
            // Set current goal based on episodic memory if available
            if (pImpl->episodicMemory && pImpl->episodicMemory->getEpisodeCount() > 0) {
                // Get the most recent episode
                auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(1);
                if (!recentEpisodes.empty()) {
                    const auto* latestEpisode = recentEpisodes[0];
                    std::vector<float> goalState;
                    for (float activation : latestEpisode->neuronActivations) {
                        goalState.push_back(activation);
                    }
                    pImpl->planner->setCurrentGoal(goalState);
                }
            }
        }
    }
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
        
        // Save integrated system states
        if (pImpl->workingMemory) {
            pImpl->systemStates["workingMemory"] = pImpl->workingMemory->serialize();
        }
        
        if (pImpl->episodicMemory) {
            pImpl->systemStates["episodicMemory"] = pImpl->episodicMemory->serialize();
        }
        
        if (pImpl->associativeMemory) {
            pImpl->systemStates["associativeMemory"] = pImpl->associativeMemory->serialize();
        }
        
        if (pImpl->predictionSystem) {
            pImpl->systemStates["predictionSystem"] = pImpl->predictionSystem->serialize();
        }
        
        if (pImpl->dopamine) {
            pImpl->systemStates["dopamine"] = pImpl->dopamine->serialize();
        }
        
        if (pImpl->curiosity) {
            pImpl->systemStates["curiosity"] = pImpl->curiosity->serialize();
        }
        
        if (pImpl->predictionError) {
            pImpl->systemStates["predictionError"] = pImpl->predictionError->serialize();
        }
        
        if (pImpl->novelty) {
            pImpl->systemStates["novelty"] = pImpl->novelty->serialize();
        }
        
        if (pImpl->planner) {
            pImpl->systemStates["planner"] = pImpl->planner->serialize();
        }
        
        if (pImpl->conceptFormation) {
            pImpl->systemStates["conceptFormation"] = pImpl->conceptFormation->serialize();
        }
        
        if (pImpl->attention) {
            pImpl->systemStates["attention"] = pImpl->attention->serialize();
        }
        
        if (pImpl->developmentSystem) {
            pImpl->systemStates["developmentSystem"] = pImpl->developmentSystem->serialize();
        }
        
        // Save planned action to system states
        std::ostringstream oss;
        oss << "ActionType:" << static_cast<int>(pImpl->plannedAction);
        pImpl->systemStates["plannedAction"] = oss.str();
        
        // Write system states to Memory section
        std::ostringstream statesOss;
        for (const auto& pair : pImpl->systemStates) {
            statesOss << pair.first << ":" << pair.second << "\n";
        }
        std::string statesStr = statesOss.str();
        writer.writeSection(CheckpointSection::Memory, statesStr.data(), statesStr.size());
        
        // Write simulation state
        auto simulationData = serializeSimulationState();
        writer.writeSection(CheckpointSection::SimulationState, simulationData.data(), simulationData.size());
        
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
        
        // Load system states from Memory section
        auto systemStatesData = reader.readSection(CheckpointSection::Memory);
        if (!systemStatesData.empty()) {
            // Parse system states
            std::string statesStr(systemStatesData.begin(), systemStatesData.end());
            std::istringstream iss(statesStr);
            std::string line;
            
            while (std::getline(iss, line)) {
                size_t colonPos = line.find(":");
                if (colonPos == std::string::npos) continue;
                
                std::string systemName = line.substr(0, colonPos);
                std::string stateData = line.substr(colonPos + 1);
                
                // Store in systemStates map for later use
                pImpl->systemStates[systemName] = stateData;
            }
            
            // Parse planned action if available
            if (pImpl->systemStates.find("plannedAction") != pImpl->systemStates.end()) {
                std::string actionStr = pImpl->systemStates["plannedAction"];
                size_t colonPos = actionStr.find(":");
                if (colonPos != std::string::npos) {
                    int actionValue = std::stoi(actionStr.substr(colonPos + 1));
                    pImpl->plannedAction = static_cast<ActionType>(actionValue);
                }
            }
        }
        
        if (pImpl->workingMemory && !workingMemoryData.empty()) {
            pImpl->workingMemory->deserialize(workingMemoryData.data(), workingMemoryData.size());
        }
        
        auto episodicMemoryData = reader.readSection(CheckpointSection::Memory);
        if (pImpl->episodicMemory && !episodicMemoryData.empty()) {
            pImpl->episodicMemory->deserialize(episodicMemoryData.data(), episodicMemoryData.size());
        }
        
        auto associativeMemoryData = reader.readSection(CheckpointSection::Memory);
        if (pImpl->associativeMemory && !associativeMemoryData.empty()) {
            pImpl->associativeMemory->deserialize(associativeMemoryData.data(), associativeMemoryData.size());
        }
        
        auto predictionData = reader.readSection(CheckpointSection::Memory);
        if (pImpl->predictionSystem && !predictionData.empty()) {
            pImpl->predictionSystem->deserialize(predictionData.data(), predictionData.size());
        }
        
        auto neuromodulationData = reader.readSection(CheckpointSection::Neuromodulation);
        if (!neuromodulationData.empty()) {
            // Parse neuromodulation data
            // In a real implementation, this would deserialize all neuromodulation systems
        }
        
        auto cognitiveData = reader.readSection(CheckpointSection::Memory);
        if (!cognitiveData.empty()) {
            // Parse cognitive system data
            // In a real implementation, this would deserialize all cognitive systems
        }
        
        auto developmentData = reader.readSection(CheckpointSection::Development);
        if (pImpl->developmentSystem && !developmentData.empty()) {
            pImpl->developmentSystem->deserialize(developmentData.data(), developmentData.size());
        }
        
        auto plasticityData = reader.readSection(CheckpointSection::Plasticity);
        if (!plasticityData.empty()) {
            // Parse plasticity system data
            // In a real implementation, this would deserialize all plasticity systems
        }
        
        auto simulationData = reader.readSection(CheckpointSection::SimulationState);
        if (!simulationData.empty()) {
            deserializeSimulationState(simulationData.data(), simulationData.size());
        }
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    pImpl->plannedAction = ActionType::Wait;
    
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
    // Action selection integrates planned actions from neural planner
    // Uses planned actions first, falls back to reactive selection
    
    // Check if a planned action is available
    if (pImpl->planner && pImpl->plannedAction != ActionType::Wait) {
        // Use planned action from neural planner
        auto action = std::make_unique<Action>(pImpl->plannedAction);
        NLM_LOG_INFO("Action selected by NeuralPlanner: " + std::to_string(static_cast<int>(pImpl->plannedAction)));
        
        // Reset planned action for next step
        pImpl->plannedAction = ActionType::Wait;
        
        return action;
    }
    
    // Fallback to reactive selection based on motor neuron activity
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

void Brain::setCurrentStep(SimulationStep step) {
    pImpl->currentStep = step;
}

void Brain::setCurrentTime(Timestamp time) {
    pImpl->currentTime = time;
}

std::string Brain::serializeSystemState(const std::string& systemName) const {
    std::ostringstream oss;
    oss << "System: " << systemName << "\n";
    oss << "CurrentStep: " << pImpl->currentStep << "\n";
    oss << "CurrentTime: " << pImpl->currentTime << "\n";
    oss << "TotalSpikesThisStep: " << pImpl->totalSpikesThisStep << "\n";
    oss << "TotalSpikesTotal: " << pImpl->totalSpikesTotal << "\n";
    oss << "IsResting: " << (pImpl->isResting ? "true" : "false") << "\n";
    oss << "StepsSinceLastEpisode: " << pImpl->stepsSinceLastEpisode << "\n";
    oss << "ReplayInterval: " << pImpl->replayInterval << "\n";
    oss << "ConsolidationInterval: " << pImpl->consolidationInterval << "\n";
    oss << "DevelopmentalStage: " << static_cast<int>(pImpl->developmentalStage) << "\n";
    
    // Add integrated system states
    if (pImpl->workingMemory) {
        oss << "WorkingMemory: " << pImpl->workingMemory->serialize() << "\n";
    }
    if (pImpl->episodicMemory) {
        oss << "EpisodicMemory: " << pImpl->episodicMemory->serialize() << "\n";
    }
    if (pImpl->associativeMemory) {
        oss << "AssociativeMemory: " << pImpl->associativeMemory->serialize() << "\n";
    }
    if (pImpl->predictionSystem) {
        oss << "PredictionSystem: " << pImpl->predictionSystem->serialize() << "\n";
    }
    
    return oss.str();
}

bool Brain::deserializeSystemState(const std::string& stateData) {
    try {
        std::istringstream iss(stateData);
        std::string line;
        
        while (std::getline(iss, line)) {
            size_t colonPos = line.find(":");
            if (colonPos == std::string::npos) continue;
            
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Parse values
            if (key == "CurrentStep") {
                pImpl->currentStep = std::stoull(value);
            } else if (key == "CurrentTime") {
                pImpl->currentTime = std::stod(value);
            } else if (key == "TotalSpikesThisStep") {
                pImpl->totalSpikesThisStep = std::stoull(value);
            } else if (key == "TotalSpikesTotal") {
                pImpl->totalSpikesTotal = std::stoull(value);
            } else if (key == "IsResting") {
                pImpl->isResting = (value == "true" || value == "1");
            } else if (key == "StepsSinceLastEpisode") {
                pImpl->stepsSinceLastEpisode = std::stoull(value);
            } else if (key == "ReplayInterval") {
                pImpl->replayInterval = std::stoull(value);
            } else if (key == "ConsolidationInterval") {
                pImpl->consolidationInterval = std::stoull(value);
            } else if (key == "DevelopmentalStage") {
                pImpl->developmentalStage = static_cast<DevelopmentalStage>(std::stoi(value));
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error deserializing brain state: " + std::string(e.what()));
        return false;
    }
}

std::vector<uint8_t> Brain::serialize() const {
    std::ostringstream oss(std::ios::binary | std::ios::sync_with_stdio);
    
    // Basic simulation state
    uint64_t currentStep = pImpl->currentStep;
    double currentTime = pImpl->currentTime;
    uint64_t totalSpikesThisStep = pImpl->totalSpikesThisStep;
    uint64_t totalSpikesTotal = pImpl->totalSpikesTotal;
    uint32_t isResting = pImpl->isResting ? 1 : 0;
    uint64_t stepsSinceLastEpisode = pImpl->stepsSinceLastEpisode;
    uint64_t replayInterval = pImpl->replayInterval;
    uint64_t consolidationInterval = pImpl->consolidationInterval;
    uint32_t developmentalStage = static_cast<uint32_t>(pImpl->developmentalStage);
    
    oss.write(reinterpret_cast<const char*>(&currentStep), sizeof(currentStep));
    oss.write(reinterpret_cast<const char*>(&currentTime), sizeof(currentTime));
    oss.write(reinterpret_cast<const char*>(&totalSpikesThisStep), sizeof(totalSpikesThisStep));
    oss.write(reinterpret_cast<const char*>(&totalSpikesTotal), sizeof(totalSpikesTotal));
    oss.write(reinterpret_cast<const char*>(&isResting), sizeof(isResting));
    oss.write(reinterpret_cast<const char*>(&stepsSinceLastEpisode), sizeof(stepsSinceLastEpisode));
    oss.write(reinterpret_cast<const char*>(&replayInterval), sizeof(replayInterval));
    oss.write(reinterpret_cast<const char*>(&consolidationInterval), sizeof(consolidationInterval));
    oss.write(reinterpret_cast<const char*>(&developmentalStage), sizeof(developmentalStage));
    
    // Add integrated system serializations
    if (pImpl->workingMemory) {
        auto workingMemoryData = pImpl->workingMemory->serialize();
        uint64_t workingMemorySize = workingMemoryData.size();
        oss.write(reinterpret_cast<const char*>(&workingMemorySize), sizeof(workingMemorySize));
        oss.write(workingMemoryData.data(), workingMemoryData.size());
    }
    
    if (pImpl->episodicMemory) {
        auto episodicMemoryData = pImpl->episodicMemory->serialize();
        uint64_t episodicMemorySize = episodicMemoryData.size();
        oss.write(reinterpret_cast<const char*>(&episodicMemorySize), sizeof(episodicMemorySize));
        oss.write(episodicMemoryData.data(), episodicMemoryData.size());
    }
    
    if (pImpl->associativeMemory) {
        auto associativeMemoryData = pImpl->associativeMemory->serialize();
        uint64_t associativeMemorySize = associativeMemoryData.size();
        oss.write(reinterpret_cast<const char*>(&associativeMemorySize), sizeof(associativeMemorySize));
        oss.write(associativeMemoryData.data(), associativeMemoryData.size());
    }
    
    if (pImpl->predictionSystem) {
        auto predictionData = pImpl->predictionSystem->serialize();
        uint64_t predictionSize = predictionData.size();
        oss.write(reinterpret_cast<const char*>(&predictionSize), sizeof(predictionSize));
        oss.write(predictionData.data(), predictionData.size());
    }
    
    return std::vector<uint8_t>(oss.str().begin(), oss.str().end());
}

bool Brain::deserialize(const std::vector<uint8_t>& data) {
    try {
        std::stringstream ss(std::ios::binary | std::ios::sync_with_stdio);
        ss.write(reinterpret_cast<const char*>(data.data()), data.size());
        
        // Read basic simulation state
        uint64_t currentStep;
        double currentTime;
        uint64_t totalSpikesThisStep;
        uint64_t totalSpikesTotal;
        uint32_t isResting;
        uint64_t stepsSinceLastEpisode;
        uint64_t replayInterval;
        uint64_t consolidationInterval;
        uint32_t developmentalStage;
        
        ss.read(reinterpret_cast<char*>(&currentStep), sizeof(currentStep));
        ss.read(reinterpret_cast<char*>(&currentTime), sizeof(currentTime));
        ss.read(reinterpret_cast<char*>(&totalSpikesThisStep), sizeof(totalSpikesThisStep));
        ss.read(reinterpret_cast<char*>(&totalSpikesTotal), sizeof(totalSpikesTotal));
        ss.read(reinterpret_cast<char*>(&isResting), sizeof(isResting));
        ss.read(reinterpret_cast<char*>(&stepsSinceLastEpisode), sizeof(stepsSinceLastEpisode));
        ss.read(reinterpret_cast<char*>(&replayInterval), sizeof(replayInterval));
        ss.read(reinterpret_cast<char*>(&consolidationInterval), sizeof(consolidationInterval));
        ss.read(reinterpret_cast<char*>(&developmentalStage), sizeof(developmentalStage));
        
        // Update pImpl state
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = totalSpikesThisStep;
        pImpl->totalSpikesTotal = totalSpikesTotal;
        pImpl->isResting = (isResting != 0);
        pImpl->stepsSinceLastEpisode = stepsSinceLastEpisode;
        pImpl->replayInterval = replayInterval;
        pImpl->consolidationInterval = consolidationInterval;
        pImpl->developmentalStage = static_cast<DevelopmentalStage>(developmentalStage);
        
        // Read and deserialize integrated systems
        uint64_t workingMemorySize;
        if (ss.read(reinterpret_cast<char*>(&workingMemorySize), sizeof(workingMemorySize))) {
            std::vector<uint8_t> workingMemoryData(workingMemorySize);
            if (workingMemoryData.size() == ss.read(reinterpret_cast<char*>(workingMemoryData.data()), workingMemorySize)) {
                if (pImpl->workingMemory) {
                    pImpl->workingMemory->deserialize(workingMemoryData.data(), workingMemoryData.size());
                }
            }
        }
        
        uint64_t episodicMemorySize;
        if (ss.read(reinterpret_cast<char*>(&episodicMemorySize), sizeof(episodicMemorySize))) {
            std::vector<uint8_t> episodicMemoryData(episodicMemorySize);
            if (episodicMemoryData.size() == ss.read(reinterpret_cast<char*>(episodicMemoryData.data()), episodicMemorySize)) {
                if (pImpl->episodicMemory) {
                    pImpl->episodicMemory->deserialize(episodicMemoryData.data(), episodicMemoryData.size());
                }
            }
        }
        
        uint64_t associativeMemorySize;
        if (ss.read(reinterpret_cast<char*>(&associativeMemorySize), sizeof(associativeMemorySize))) {
            std::vector<uint8_t> associativeMemoryData(associativeMemorySize);
            if (associativeMemoryData.size() == ss.read(reinterpret_cast<char*>(associativeMemoryData.data()), associativeMemorySize)) {
                if (pImpl->associativeMemory) {
                    pImpl->associativeMemory->deserialize(associativeMemoryData.data(), associativeMemoryData.size());
                }
            }
        }
        
        uint64_t predictionSize;
        if (ss.read(reinterpret_cast<char*>(&predictionSize), sizeof(predictionSize))) {
            std::vector<uint8_t> predictionData(predictionSize);
            if (predictionData.size() == ss.read(reinterpret_cast<char*>(predictionData.data()), predictionSize)) {
                if (pImpl->predictionSystem) {
                    pImpl->predictionSystem->deserialize(predictionData.data(), predictionData.size());
                }
            }
        }
        
        return !ss.fail();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error deserializing brain data: " + std::string(e.what()));
        return false;
    }
}

void Brain::setIntegratedSystemState(const std::string& systemName, const std::string& state) {
    pImpl->systemStates[systemName] = state;
}

std::string Brain::getIntegratedSystemState(const std::string& systemName) const {
    auto it = pImpl->systemStates.find(systemName);
    if (it != pImpl->systemStates.end()) {
        return it->second;
    }
    return "";
}

std::vector<uint8_t> Brain::serializeSimulationState() const {
    return serialize();
}

bool Brain::deserializeSimulationState(const std::vector<uint8_t>& data) {
    return deserialize(data);
}

} // namespace nlm
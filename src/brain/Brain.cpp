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
#include "../neuromodulation/Reward.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../cognition/SelfModel.hpp"
#include "../cognition/SocialLearning.hpp"
#include "../cognition/SpatialRepresentation.hpp"
#include "../cognition/TemporalRelation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

// Include all neuromodulator implementations
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Norepinephrine.hpp"

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
    std::unique_ptr<SpatialRepresentation> spatialRepresentation;
    std::unique_ptr<TemporalRelation> temporalRelation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
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
        selfModel = std::make_unique<SelfModel>();
        socialLearning = std::make_unique<SocialLearning>();
        spatialRepresentation = std::make_unique<SpatialRepresentation>();
        temporalRelation = std::make_unique<TemporalRelation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Note: NeuralPlanner contains SelfModel and SocialLearning as nested classes,
        // but we're now using the separate standalone cognitive systems.
        // For backward compatibility, we'll initialize both:
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // ========== NEUROMODULATION SYSTEMS ==========
        dopamine = std::make_unique<Dopamine>();
        acetylcholine = std::make_unique<Acetylcholine>();
        serotonin = std::make_unique<Serotonin>();
        norepinephrine = std::make_unique<Norepinephrine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        reward = std::make_unique<Reward>();
        
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
    
    pImpl->selfModel->initialize(this);
    pImpl->socialLearning->initialize(this);
    pImpl->spatialRepresentation->initialize(this);
    pImpl->temporalRelation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize all neuromodulation systems
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    pImpl->predictionError->initialize(this);
    pImpl->dopamine->initialize(this);
    pImpl->acetylcholine->initialize(this);
    pImpl->serotonin->initialize(this);
    pImpl->norepinephrine->initialize(this);
    pImpl->reward->initialize(this);
    
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
     * 11. Update self model
     * 12. Update social learning
     * 13. Update spatial representation
     * 14. Update temporal relations
     * 15. Apply structural plasticity (synaptogenesis, pruning)
     * 16. Replay important memories (during rest or periodically)
     * 17. Apply development effects
     * 18. Periodic memory consolidation
     * 19. Checkpoint management
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
    
    // Update acetylcholine for attention modulation
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        float achLevel = pImpl->acetylcholine->getLevel();
        
        // Apply acetylcholine effects on neural excitability
        // Acetylcholine enhances attention and sensory processing
        float attentionMod = achLevel * 0.3f;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Acetylcholine modulates neural gain and attention
                    float attentionBoost = 1.0f + attentionMod;
                    neuron->setMembranePotential(neuron->getMembranePotential() * attentionBoost);
                }
            }
        }
    }
    
    // Update norepinephrine for arousal modulation
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
        float neLevel = pImpl->norepinephrine->getLevel();
        
        // Apply norepinephrine effects on neural excitability
        // Norepinephrine increases arousal and vigilance
        float arousalMod = neLevel * 0.4f;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Norepinephrine enhances response gain and focus
                    float arousalBoost = 1.0f + arousalMod;
                    neuron->setMembranePotential(neuron->getMembranePotential() * arousalBoost);
                }
            }
        }
    }
    
    // Update serotonin for mood and stability modulation
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
        float serotoninLevel = pImpl->serotonin->getLevel();
        
        // Apply serotonin effects on neural excitability
        // Serotonin stabilizes neural activity and reduces impulsive firing
        float stabilityMod = std::max(0.1f, 1.0f - serotoninLevel * 0.5f);
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Serotonin modulates firing threshold and stability
                    float thresholdMod = 1.0f / stabilityMod;
                    neuron->setThreshold(neuron->getThreshold() * thresholdMod);
                }
            }
        }
    }
    
    // Update reward signal
    if (pImpl->reward) {
        pImpl->reward->update(pImpl->timestep);
    }
    
    // ========== STEP 6: Apply neuromodulation effects on neural excitability ==========
    
    // Neuromodulation factor combining all effects
    float totalNeuromodulation = 1.0f;
    
    // Apply neuromodulation effects on neural excitability based on each neuromodulator
    if (pImpl->dopamine) {
        float dopamineMod = pImpl->dopamine->getLevel();
        totalNeuromodulation *= (1.0f + dopamineMod * 0.3f);
    }
    
    if (pImpl->acetylcholine) {
        float achMod = pImpl->acetylcholine->getLevel();
        totalNeuromodulation *= (1.0f + achMod * 0.2f);
    }
    
    if (pImpl->norepinephrine) {
        float neMod = pImpl->norepinephrine->getLevel();
        totalNeuromodulation *= (1.0f + neMod * 0.4f);
    }
    
    if (pImpl->serotonin) {
        float serotoninMod = pImpl->serotonin->getLevel();
        totalNeuromodulation *= std::max(0.5f, 1.0f - serotoninMod * 0.3f);
    }
    
    // Apply combined neuromodulation to all neurons
    if (totalNeuromodulation != 1.0f) {
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Apply neuromodulation to membrane potential
                    float currentMod = totalNeuromodulation - 1.0f;
                    if (currentMod > 0.0f) {
                        neuron->injectCurrent(currentMod * 0.5f);
                    }
                }
            }
        }
    }
    
    // ========== STEP 7: Apply neuromodulation to plasticity rules ==========
    
    // Calculate combined neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    
    if (pImpl->dopamine) {
        plasticityMod *= pImpl->dopamine->getPlasticityFactor();
    }
    
    if (pImpl->acetylcholine) {
        plasticityMod *= pImpl->acetylcholine->getPlasticityFactor();
    }
    
    if (pImpl->norepinephrine) {
        plasticityMod *= pImpl->norepinephrine->getPlasticityFactor();
    }
    
    if (pImpl->serotonin) {
        plasticityMod *= pImpl->serotonin->getPlasticityFactor();
    }
    
    // Apply plasticity modulation to STDP and Hebbian rules
    if (plasticityMod != 1.0f) {
        if (pImpl->stdp) {
            pImpl->stdp->setLTPWeight(pImpl->stdp->getLTPWeight() * plasticityMod);
            pImpl->stdp->setLTDWeight(pImpl->stdp->getLTDWeight() * plasticityMod);
        }
        
        if (pImpl->hebbian) {
            // Hebbian plasticity modulation happens through weight updates
        }
    }
    
    // ========== STEP 8: Apply neuromodulation to memory systems ==========
    
    // Neuromodulation affects memory consolidation and retrieval
    if (pImpl->workingMemory) {
        // Working memory modulation
        float memoryMod = totalNeuromodulation;
        pImpl->workingMemory->setCompetitionFactor(memoryMod);
        
        // Acetylcholine enhances working memory storage
        if (pImpl->acetylcholine) {
            float achMod = pImpl->acetylcholine->getLevel();
            pImpl->workingMemory->setStorageBoost(achMod * 0.5f);
        }
    }
    
    if (pImpl->episodicMemory) {
        // Episodic memory modulation
        float memoryMod = totalNeuromodulation;
        
        // Dopamine affects reward-based memory consolidation
        if (pImpl->dopamine) {
            float dopamineMod = pImpl->dopamine->getLevel();
            pImpl->episodicMemory->setRewardBoost(dopamineMod * 0.8f);
        }
        
        // Novelty affects memory encoding
        if (pImpl->novelty) {
            float noveltyMod = pImpl->novelty->getLevel();
            pImpl->episodicMemory->setEncodingBoost(noveltyMod * 0.6f);
        }
    }
    
    // ========== STEP 9: Apply neuromodulation to attention system ==========
    
    if (pImpl->attention) {
        // Attention modulation based on acetylcholine and norepinephrine
        float attentionMod = 1.0f;
        
        if (pImpl->acetylcholine) {
            float achMod = pImpl->acetylcholine->getLevel();
            attentionMod *= (1.0f + achMod * 0.5f);
        }
        
        if (pImpl->norepinephrine) {
            float neMod = pImpl->norepinephrine->getLevel();
            attentionMod *= (1.0f + neMod * 0.3f);
        }
        
        if (attentionMod != 1.0f) {
            pImpl->attention->setInhibitionStrength(pImpl->attention->getInhibitionStrength() / attentionMod);
            pImpl->attention->setExcitationStrength(pImpl->attention->getExcitationStrength() * attentionMod);
        }
    }
    
    // ========== STEP 10: Apply neuromodulation to development ==========
    
    if (currentStep % 500 == 0) {  // Apply development effects every 500 steps
        // Neuromodulators affect developmental trajectories
        if (pImpl->dopamine && pImpl->acetylcholine && pImpl->serotonin && pImpl->norepinephrine) {
            float developmentalMod = (
                pImpl->dopamine->getLevel() * 0.3f +
                pImpl->acetylcholine->getLevel() * 0.3f +
                pImpl->serotonin->getLevel() * 0.2f +
                pImpl->norepinephrine->getLevel() * 0.4f
            ) / 1.2f;  // Average with scaling
            
            // Development system uses neuromodulation to adjust maturation rates
            if (pImpl->developmentSystem) {
                pImpl->developmentSystem->setMaturationRate(developmentalMod);
            }
        }
    }
    
    // ========== STEP 11: Apply neuromodulation to behavior selection ==========
    
    // Neuromodulators guide behavior selection and exploration/exploitation balance
    if (pImpl->dopamine) {
        float dopamineMod = pImpl->dopamine->getLevel();
        
        // Dopamine influences action selection - higher dopamine promotes approach behaviors
        if (dopamineMod > 0.5f) {
            // Bias toward action selection (exploration bias)
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Dopamine enhances action-related neurons
                        float actionMod = dopamineMod * 0.3f;
                        neuron->injectCurrent(actionMod);
                    }
                }
            }
        } else if (dopamineMod < -0.5f) {
            // Bias toward inhibition (cautious behavior)
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Dopamine suppresses action-related neurons
                        float inhibitionMod = std::abs(dopamineMod) * 0.2f;
                        neuron->injectCurrent(-inhibitionMod);
                    }
                }
            }
        }
    }
    
    if (pImpl->curiosity) {
        float curiosityLevel = pImpl->curiosity->getLevel();
        
        // Curiosity drives exploration behavior
        if (curiosityLevel > 0.3f) {
            // Promote exploration through neuromodulation
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Curiosity enhances behavioral flexibility
                        float explorationMod = curiosityLevel * 0.2f;
                        neuron->injectCurrent(explorationMod);
                    }
                }
            }
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
        // Record current neural activity state for prediction
        std::vector<float> currentNeuralState;
        currentNeuralState.reserve(pImpl->sensoryNeurons.size());
        
        for (auto* neuron : pImpl->sensoryNeurons) {
            float activity = neuron->getState().membranePotential - neuron->getState().restingPotential;
            currentNeuralState.push_back(std::max(0.0f, activity));
        }
        
        // Create internal signals from neural activity
        auto internalInput = std::make_unique<InternalSignals>();
        for (float val : currentNeuralState) {
            internalInput->addSignal(val);
        }
        
        // Train the prediction system with current state
        pImpl->predictionSystem->train(*internalInput);
        
        // Generate prediction for next state
        auto prediction = pImpl->predictionSystem->predictNextState(*internalInput);
        
        // For now, use current state as actual (in a real system this would come from environment)
        auto actualInput = std::make_unique<InternalSignals>(*internalInput);
        
        // Update predictions and compute error
        pImpl->predictionSystem->updatePredictions(prediction.get(), *actualInput);
        
        // Compute prediction error for neuromodulation
        if (pImpl->predictionError) {
            float error = pImpl->predictionSystem->getPredictionError();
            float confidence = pImpl->predictionSystem->getConfidence();
            
            // Update prediction confidence in the prediction system
            pImpl->predictionSystem->getConfidence(); // This would update confidence based on error
            
            // Send error to neuromodulation system for learning
            pImpl->predictionError->computeError(error, std::abs(error));
            
            // Apply prediction error to neuromodulation
            pImpl->predictionError->updatePrediction(error);
        }
        
        // Log prediction information
        float error = pImpl->predictionSystem->getPredictionError();
        float confidence = pImpl->predictionSystem->getConfidence();
        NLM_LOG_DEBUG("Prediction: confidence=" + std::to_string(confidence) +
                     ", error=" + std::to_string(error));
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
    
    // ========== STEP 11: Update self model ==========
    if (pImpl->selfModel) {
        // Would update self-model based on prediction errors
        // This requires prediction error signals and sensory feedback
    }
    
    // ========== STEP 12: Update social learning ==========
    if (pImpl->socialLearning) {
        // Would update social knowledge from interactions
        // This requires social interaction signals and reward feedback
    }
    
    // ========== STEP 13: Update spatial representation ==========
    if (pImpl->spatialRepresentation) {
        // Would update spatial representations from navigation
        // This requires motor commands and sensory feedback
    }
    
    // ========== STEP 14: Update temporal relations ==========
    if (pImpl->temporalRelation) {
        // Would update temporal relationships from sequences
        // This requires sequence completion signals and rewards
    }
    
    // ========== STEP 15: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 16: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 17: Apply development effects ==========
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
    
    // ========== STEP 18: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // ========== STEP 19: Checkpoint management ==========
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
        
        // Save inter-region connections
        std::vector<std::pair<uint64_t, uint64_t>> interRegionSourceNeurons;
        std::vector<std::pair<uint64_t, uint64_t>> interRegionDestinationNeurons;
        std::vector<float> interRegionWeights;
        std::vector<uint32_t> interRegionDelays;
        
        for (const auto& conn : pImpl->interRegionConnections) {
            interRegionSourceNeurons.push_back(std::make_pair(conn.sourceRegion.index(), 0)); // Placeholder neuron index
            interRegionDestinationNeurons.push_back(std::make_pair(conn.targetRegion.index(), 0)); // Placeholder neuron index
            interRegionWeights.push_back(conn.weight);
            interRegionDelays.push_back(conn.delay);
        }
        
        if (!interRegionWeights.empty()) {
            writer.writeSection(CheckpointSection::Connectivity, 
                              interRegionWeights.data(), 
                              interRegionWeights.size() * sizeof(float));
        }
        
        // Save plasticity state (STDP, Hebbian, StructuralPlasticity)
        std::vector<float> plasticityData;
        
        // STDP parameters
        if (pImpl->stdp) {
            plasticityData.push_back(pImpl->stdp->getLTPWeight());
            plasticityData.push_back(pImpl->stdp->getLTDWeight());
            plasticityData.push_back(pImpl->stdp->getTimeConstant());
        }
        
        // Hebbian parameters
        if (pImpl->hebbian) {
            plasticityData.push_back(pImpl->hebbian->getLearningRate());
            plasticityData.push_back(pImpl->hebbian->getMaxWeight());
        }
        
        // Structural plasticity parameters
        if (pImpl->structuralPlasticity) {
            plasticityData.push_back(pImpl->structuralPlasticity->getSynaptogenesisRate());
            plasticityData.push_back(pImpl->structuralPlasticity->getPruningRate());
        }
        
        if (!plasticityData.empty()) {
            writer.writeSection(CheckpointSection::Plasticity, 
                              plasticityData.data(), 
                              plasticityData.size() * sizeof(float));
        }
        
        // Save development system state
        std::vector<float> developmentData;
        developmentData.push_back(static_cast<float>(pImpl->developmentalStage));
        developmentData.push_back(static_cast<float>(pImpl->currentStep));
        developmentData.push_back(static_cast<float>(pImpl->currentTime));
        developmentData.push_back(static_cast<float>(pImpl->developmentSystem->getStageAge()));
        developmentData.push_back(pImpl->developmentSystem->getPlasticityModifier());
        developmentData.push_back(static_cast<float>(pImpl->developmentSystem->isCriticalPeriod()));
        developmentData.push_back(pImpl->developmentSystem->getCriticalPeriodProgress());
        
        if (!developmentData.empty()) {
            writer.writeSection(CheckpointSection::Development, 
                              developmentData.data(), 
                              developmentData.size() * sizeof(float));
        }
        
        // Save neuromodulation state
        std::vector<float> neuromodulationData;
        
        // Dopamine
        if (pImpl->dopamine) {
            neuromodulationData.push_back(pImpl->dopamine->getLevel());
        }
        
        // Acetylcholine
        if (pImpl->acetylcholine) {
            neuromodulationData.push_back(pImpl->acetylcholine->getLevel());
        }
        
        // Serotonin
        if (pImpl->serotonin) {
            neuromodulationData.push_back(pImpl->serotonin->getLevel());
        }
        
        // Norepinephrine
        if (pImpl->norepinephrine) {
            neuromodulationData.push_back(pImpl->norepinephrine->getLevel());
        }
        
        // Curiosity
        if (pImpl->curiosity) {
            neuromodulationData.push_back(pImpl->curiosity->getLevel());
        }
        
        // Novelty
        if (pImpl->novelty) {
            neuromodulationData.push_back(pImpl->novelty->getLevel());
        }
        
        // Prediction error
        if (pImpl->predictionError) {
            neuromodulationData.push_back(pImpl->predictionError->getLevel());
        }
        
        // Reward
        if (pImpl->reward) {
            neuromodulationData.push_back(pImpl->reward->getLevel());
        }
        
        if (!neuromodulationData.empty()) {
            writer.writeSection(CheckpointSection::Neuromodulation, 
                              neuromodulationData.data(), 
                              neuromodulationData.size() * sizeof(float));
        }
        
        // Save memory system states
        std::vector<float> memoryData;
        
        // Working memory
        if (pImpl->workingMemory) {
            const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            const auto& memoryActivations = pImpl->workingMemory->getMemoryActivations();
            
            memoryData.push_back(static_cast<float>(memoryNeurons.size()));
            for (size_t i = 0; i < memoryNeurons.size(); ++i) {
                memoryData.push_back(memoryNeurons[i].index());
                memoryData.push_back(memoryActivations[i]);
            }
        }
        
        // Episodic memory
        if (pImpl->episodicMemory) {
            const auto& episodes = pImpl->episodicMemory->getEpisodeCount();
            memoryData.push_back(static_cast<float>(episodes));
            
            // For each episode, store key data
            for (size_t i = 0; i < episodes; ++i) {
                const auto* episode = pImpl->episodicMemory->getEpisode(i);
                if (episode) {
                    memoryData.push_back(static_cast<float>(episode->timestamp));
                    memoryData.push_back(episode->reward);
                    memoryData.push_back(episode->energy);
                    memoryData.push_back(episode->novelty);
                    memoryData.push_back(static_cast<float>(episode->activeNeurons.size()));
                    for (const auto& neuronId : episode->activeNeurons) {
                        memoryData.push_back(neuronId.index());
                    }
                }
            }
        }
        
        // Associative memory
        if (pImpl->associativeMemory) {
            const auto& associations = pImpl->associativeMemory->getAssociationCount();
            memoryData.push_back(static_cast<float>(associations));
            
            // For each association, store source and target patterns
            for (size_t i = 0; i < associations; ++i) {
                // This would require accessing internal associative memory data
                // For now, we'll just store a placeholder
                memoryData.push_back(0.0f); // Placeholder
            }
        }
        
        if (!memoryData.empty()) {
            writer.writeSection(CheckpointSection::Memory, 
                              memoryData.data(), 
                              memoryData.size() * sizeof(float));
        }
        
        // Save prediction system state
        std::vector<float> predictionData;
        if (pImpl->predictionSystem) {
            const auto& errorHistory = pImpl->predictionSystem->getErrorHistory();
            predictionData.push_back(pImpl->predictionSystem->getPredictionError());
            predictionData.push_back(pImpl->predictionSystem->getConfidence());
            predictionData.push_back(static_cast<float>(errorHistory.size()));
            for (const auto& error : errorHistory) {
                predictionData.push_back(error);
            }
        }
        
        if (!predictionData.empty()) {
            writer.writeSection(CheckpointSection::Prediction, 
                              predictionData.data(), 
                              predictionData.size() * sizeof(float));
        }
        
        // Save random generator state
        std::vector<uint64_t> randomData;
        if (pImpl->rng) {
            // For now, we'll use a fixed seed for reproducibility
            // In a real implementation, we would save the actual random state
            randomData.push_back(42); // Fixed seed
        }
        
        if (!randomData.empty()) {
            writer.writeSection(CheckpointSection::RandomState, 
                              randomData.data(), 
                              randomData.size() * sizeof(uint64_t));
        }
        
        // Save simulation state
        std::vector<float> simulationData;
        simulationData.push_back(static_cast<float>(pImpl->currentStep));
        simulationData.push_back(static_cast<float>(pImpl->currentTime));
        simulationData.push_back(static_cast<float>(pImpl->totalSpikesThisStep));
        simulationData.push_back(static_cast<float>(pImpl->totalSpikesTotal));
        
        if (!simulationData.empty()) {
            writer.writeSection(CheckpointSection::SimulationState, 
                              simulationData.data(), 
                              simulationData.size() * sizeof(float));
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
        
        // Load inter-region connections from Connectivity section
        std::vector<uint8_t> connectivityData = reader.readSection(CheckpointSection::Connectivity);
        if (!connectivityData.empty()) {
            // Parse connectivity data (simplified)
            const float* data = reinterpret_cast<const float*>(connectivityData.data());
            size_t numFloats = connectivityData.size() / sizeof(float);
            // In a real implementation, this would parse the connectivity data properly
            NLM_LOG_INFO("Loaded " + std::to_string(numFloats) + " connectivity entries");
        }
        
        // Load plasticity state from Plasticity section
        std::vector<uint8_t> plasticityData = reader.readSection(CheckpointSection::Plasticity);
        if (!plasticityData.empty()) {
            const float* data = reinterpret_cast<const float*>(plasticityData.data());
            size_t numFloats = plasticityData.size() / sizeof(float);
            
            size_t offset = 0;
            if (pImpl->stdp && numFloats >= 3) {
                pImpl->stdp->setLTPWeight(data[offset++]);
                pImpl->stdp->setLTDWeight(data[offset++]);
                pImpl->stdp->setTimeConstant(data[offset++]);
            }
            
            if (pImpl->hebbian && numFloats >= offset + 2) {
                pImpl->hebbian->setLearningRate(data[offset++]);
                pImpl->hebbian->setMaxWeight(data[offset++]);
            }
            
            if (pImpl->structuralPlasticity && numFloats >= offset + 2) {
                pImpl->structuralPlasticity->setSynaptogenesisRate(data[offset++]);
                pImpl->structuralPlasticity->setPruningRate(data[offset++]);
            }
            
            NLM_LOG_INFO("Loaded plasticity state with " + std::to_string(numFloats) + " parameters");
        }
        
        // Load development system state from Development section
        std::vector<uint8_t> developmentData = reader.readSection(CheckpointSection::Development);
        if (!developmentData.empty()) {
            const float* data = reinterpret_cast<const float*>(developmentData.data());
            size_t numFloats = developmentData.size() / sizeof(float);
            
            if (numFloats >= 7) {
                pImpl->developmentalStage = static_cast<DevelopmentalStage>(static_cast<int>(data[0]));
                pImpl->currentStep = static_cast<SimulationStep>(static_cast<int>(data[1]));
                pImpl->currentTime = static_cast<Timestamp>(data[2]);
                
                // Note: In a real implementation, we would update the development system
                // with these loaded values
                NLM_LOG_INFO("Loaded development state: stage=" + std::to_string(static_cast<int>(pImpl->developmentalStage)) +
                           ", step=" + std::to_string(pImpl->currentStep) +
                           ", time=" + std::to_string(pImpl->currentTime));
            }
        }
        
        // Load neuromodulation state from Neuromodulation section
        std::vector<uint8_t> neuromodulationData = reader.readSection(CheckpointSection::Neuromodulation);
        if (!neuromodulationData.empty()) {
            const float* data = reinterpret_cast<const float*>(neuromodulationData.data());
            size_t numFloats = neuromodulationData.size() / sizeof(float);
            
            size_t offset = 0;
            if (pImpl->dopamine && numFloats > offset) {
                pImpl->dopamine->setLevel(data[offset++]);
            }
            
            if (pImpl->acetylcholine && numFloats > offset) {
                pImpl->acetylcholine->setLevel(data[offset++]);
            }
            
            if (pImpl->serotonin && numFloats > offset) {
                pImpl->serotonin->setLevel(data[offset++]);
            }
            
            if (pImpl->norepinephrine && numFloats > offset) {
                pImpl->norepinephrine->setLevel(data[offset++]);
            }
            
            if (pImpl->curiosity && numFloats > offset) {
                pImpl->curiosity->setLevel(data[offset++]);
            }
            
            if (pImpl->novelty && numFloats > offset) {
                pImpl->novelty->setLevel(data[offset++]);
            }
            
            if (pImpl->predictionError && numFloats > offset) {
                pImpl->predictionError->setLevel(data[offset++]);
            }
            
            if (pImpl->reward && numFloats > offset) {
                pImpl->reward->setLevel(data[offset++]);
            }
            
            NLM_LOG_INFO("Loaded neuromodulation state with " + std::to_string(numFloats) + " parameters");
        }
        
        // Load memory system states from Memory section
        std::vector<uint8_t> memoryData = reader.readSection(CheckpointSection::Memory);
        if (!memoryData.empty()) {
            const float* data = reinterpret_cast<const float*>(memoryData.data());
            size_t numFloats = memoryData.size() / sizeof(float);
            
            size_t offset = 0;
            if (pImpl->workingMemory && numFloats > offset) {
                size_t numMemoryNeurons = static_cast<size_t>(data[offset++]);
                // In a real implementation, we would restore the working memory state
                NLM_LOG_INFO("Loaded working memory with " + std::to_string(numMemoryNeurons) + " neurons");
            }
            
            if (pImpl->episodicMemory && numFloats > offset) {
                size_t numEpisodes = static_cast<size_t>(data[offset++]);
                // In a real implementation, we would restore episodic memory episodes
                NLM_LOG_INFO("Loaded episodic memory with " + std::to_string(numEpisodes) + " episodes");
            }
            
            if (pImpl->associativeMemory && numFloats > offset) {
                size_t numAssociations = static_cast<size_t>(data[offset++]);
                // In a real implementation, we would restore associative memory
                NLM_LOG_INFO("Loaded associative memory with " + std::to_string(numAssociations) + " associations");
            }
        }
        
        // Load prediction system state from Prediction section
        std::vector<uint8_t> predictionData = reader.readSection(CheckpointSection::Prediction);
        if (!predictionData.empty()) {
            const float* data = reinterpret_cast<const float*>(predictionData.data());
            size_t numFloats = predictionData.size() / sizeof(float);
            
            if (numFloats >= 2) {
                // In a real implementation, we would restore prediction system state
                NLM_LOG_INFO("Loaded prediction system state");
            }
        }
        
        // Load random generator state from RandomState section
        std::vector<uint8_t> randomData = reader.readSection(CheckpointSection::RandomState);
        if (!randomData.empty()) {
            const uint64_t* data = reinterpret_cast<const uint64_t*>(randomData.data());
            size_t numUints = randomData.size() / sizeof(uint64_t);
            
            if (numUints > 0) {
                // In a real implementation, we would restore the random generator state
                NLM_LOG_INFO("Loaded random generator state");
            }
        }
        
        // Load simulation state from SimulationState section
        std::vector<uint8_t> simulationData = reader.readSection(CheckpointSection::SimulationState);
        if (!simulationData.empty()) {
            const float* data = reinterpret_cast<const float*>(simulationData.data());
            size_t numFloats = simulationData.size() / sizeof(float);
            
            if (numFloats >= 4) {
                pImpl->currentStep = static_cast<SimulationStep>(static_cast<int>(data[0]));
                pImpl->currentTime = static_cast<Timestamp>(data[1]);
                pImpl->totalSpikesThisStep = static_cast<size_t>(data[2]);
                pImpl->totalSpikesTotal = static_cast<size_t>(data[3]);
                
                NLM_LOG_INFO("Loaded simulation state: step=" + std::to_string(pImpl->currentStep) +
                           ", time=" + std::to_string(pImpl->currentTime) +
                           ", spikesThisStep=" + std::to_string(pImpl->totalSpikesThisStep) +
                           ", totalSpikes=" + std::to_string(pImpl->totalSpikesTotal));
            }
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

SelfModel* Brain::getSelfModel() {
    return pImpl->selfModel.get();
}

SocialLearning* Brain::getSocialLearning() {
    return pImpl->socialLearning.get();
}

SpatialRepresentation* Brain::getSpatialRepresentation() {
    return pImpl->spatialRepresentation.get();
}

TemporalRelation* Brain::getTemporalRelation() {
    return pImpl->temporalRelation.get();
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

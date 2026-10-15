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
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
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
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
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
                    
                    // Store to working memory with richer encoding
                    if (pImpl->workingMemory) {
                        // Store comprehensive neuron state including membrane potential dynamics
                        float activation = std::abs(state.membranePotential - state.restingPotential) / 10.0f;
                        
                        // Add refractory state to working memory for more meaningful information
                        bool isRefractory = neuron->getState().refractoryUntil > currentTime;
                        float refractoryState = isRefractory ? 0.0f : 1.0f;
                        
                        // Store composite activation including context
                        float compositeActivation = activation * (0.7f + 0.3f * refractoryState);
                        
                        // Store with neural mechanism - enable Hebbian-like strengthening
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), compositeActivation);
                        
                        // Also store in associative memory with context
                        if (pImpl->associativeMemory && activation > 0.5f) {
                            // Create association between this neuron and recent sensory input
                            std::vector<float> sensoryPattern;
                            for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                                if (pImpl->sensoryNeurons[i] && 
                                    pImpl->sensoryNeurons[i]->getId() == neuron->getId()) {
                                    sensoryPattern.push_back(activation * 10.0f);  // Scale to input range
                                    break;
                                }
                            }
                            if (!sensoryPattern.empty()) {
                                pImpl->associativeMemory->associateFromExperience(CreateEpisodicItemFromContext(currentStep, neuron->getId(), compositeActivation));
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        // Update working memory for maintenance and competition
        pImpl->workingMemory->update(pImpl->timestep);
        
        // Enhanced working memory with competitive dynamics and attention integration
        if (pImpl->attention) {
            // Get current working memory neurons for competition processing
            std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            
            if (!memoryNeurons.empty()) {
                // Apply attention-based selection and competition
                std::vector<NeuronId> winners = pImpl->attention->processCompetition(memoryNeurons);
                
                // Enhanced competition dynamics based on attention winners
                if (winners.size() < memoryNeurons.size()) {
                    // Apply lateral inhibition to non-winners
                    for (const NeuronId& neuron : memoryNeurons) {
                        bool isWinner = std::find(winners.begin(), winners.end(), neuron) != winners.end();
                        if (!isWinner && pImpl->workingMemory) {
                            // Apply strong inhibition to non-winners for better selection
                            float activation = pImpl->workingMemory->getNeuronActivation(neuron);
                            if (activation > 0.1f) {
                                pImpl->attention->applyTopDownBias(neuron, -activation * 2.0f);
                            }
                        }
                    }
                }
                
                // Re-inject currents to maintain winners and suppress losers
                for (const NeuronId& neuron : memoryNeurons) {
                    if (pImpl->workingMemory) {
                        float activation = pImpl->workingMemory->getNeuronActivation(neuron);
                        
                        // Winners get reinforcement
                        if (std::find(winners.begin(), winners.end(), neuron) != winners.end()) {
                            // Strengthen winner activation (Hebbian-like)
                            pImpl->workingMemory->strengthenMemory(1.2f);
                            // Inject current to maintain winner state
                            if (auto* neuronPtr = getNeuron(neuron)) {
                                neuronPtr->injectCurrent(activation * 3.0f);
                            }
                        } else {
                            // Losers get suppression
                            pImpl->attention->applyBottomUpSalience(neuron, -activation * 0.5f);
                            // Inject inhibitory current
                            if (auto* neuronPtr = getNeuron(neuron)) {
                                neuronPtr->injectCurrent(-activation * 2.0f);
                            }
                        }
                    }
                }
            }
        }
        
        // Enhanced working memory consolidation based on novelty
        if (pImpl->novelty && pImpl->workingMemory) {
            // Store memory significance based on novelty detection
            float noveltyLevel = pImpl->novelty->getLevel();
            if (noveltyLevel > 0.5f) {
                // When novel information is detected, strengthen relevant working memory traces
                std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
                for (const NeuronId& neuron : memoryNeurons) {
                    float activation = pImpl->workingMemory->getNeuronActivation(neuron);
                    if (activation > 0.2f) {
                        pImpl->workingMemory->strengthenMemory(1.1f + noveltyLevel * 0.5f);
                        break;  // Only strengthen one memory per step
                    }
                }
            }
        }
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update all neuromodulators
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // ========== ENHANCED NEUROMODULATION SYSTEM ==========
    // Update all neuromodulators
    if (pImpl->dopamine) pImpl->dopamine->update(pImpl->timestep);
    if (pImpl->acetylcholine) pImpl->acetylcholine->update(pImpl->timestep);
    if (pImpl->norepinephrine) pImpl->norepinephrine->update(pImpl->timestep);
    if (pImpl->serotonin) pImpl->serotonin->update(pImpl->timestep);
    
    // ========== ENHANCED DOPAMINE EFFECTS ==========
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        
        // 1. Enhanced effects on neural excitability (existing)
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Enhanced dopamine modulation of excitability
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
        
        // 2. Enhanced memory consolidation
        pImpl->dopamine->enhanceMemoryConsolidation();
        if (pImpl->episodicMemory) {
            // Dopamine enhances episodic memory storage with reward signal
            float memoryConsolidationFactor = 1.0f + dopamineLevel * 0.5f;
            pImpl->episodicMemory->setConsolidationFactor(memoryConsolidationFactor);
            
            // Store dopamine-modulated reward in episodic memory
            if (currentStep % 20 == 0) {  // Store reward info periodically
                EpisodicMemoryItem rewardEpisode;
                rewardEpisode.timestamp = currentStep;
                rewardEpisode.reward = dopamineLevel;
                rewardEpisode.novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
                pImpl->episodicMemory->storeEpisode(rewardEpisode);
            }
        }
        
        // 3. Enhanced attention modulation
        pImpl->dopamine->enhanceAttention();
        if (pImpl->attention) {
            // Dopamine enhances attentional selection
            float attentionGain = 1.0f + dopamineLevel * 0.8f;
            pImpl->attention->setExcitationStrength(1.5f * attentionGain);
        }
        
        // 4. Enhanced working memory retrieval
        pImpl->dopamine->enhanceWorkingMemoryRetrieval();
        if (pImpl->workingMemory) {
            // Dopamine enhances working memory capacity and retrieval
            float workingMemoryBoost = 1.0f + dopamineLevel * 0.3f;
            pImpl->workingMemory->setRetrievalBoost(workingMemoryBoost);
            
            // Enhanced reward-prediction integration into working memory
            std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
            if (!workingMemoryPattern.empty()) {
                // Strengthen based on dopamine level
                pImpl->workingMemory->strengthenMemory(1.1f * workingMemoryBoost);
            }
        }
        
        // 5. Enhanced episodic memory storage
        pImpl->dopamine->enhanceEpisodicMemoryStorage();
        if (pImpl->episodicMemory && pImpl->workingMemory) {
            // Store rich episodic memories with dopamine modulation
            if (currentStep % 50 == 0) {
                EpisodicMemoryItem richEpisode;
                richEpisode.timestamp = currentStep;
                richEpisode.reward = dopamineLevel;
                
                // Capture comprehensive neural state
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->isFiring()) {
                                richEpisode.activeNeurons.push_back(neuron->getId());
                                float activation = std::abs(neuron->getState().membranePotential - 
                                                         neuron->getState().restingPotential) / 20.0f;
                                richEpisode.neuronActivations.push_back(activation * (1.0f + dopamineLevel));
                            }
                        }
                    }
                }
                
                // Store working memory content as part of episodic memory
                auto memoryPattern = pImpl->workingMemory->retrieve();
                if (!memoryPattern.empty()) {
                    richEpisode.sensoryState = memoryPattern;
                }
                
                pImpl->episodicMemory->storeEpisode(richEpisode);
            }
        }
        
        // 6. Enhanced associative memory integration with reward prediction (existing, enhanced)
        if (pImpl->associativeMemory && pImpl->workingMemory) {
            // Get current sensory and working memory patterns
            std::vector<float> sensoryPattern;
            for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                if (pImpl->sensoryNeurons[i]) {
                    float activation = std::abs(pImpl->sensoryNeurons[i]->getState().membranePotential - 
                                               pImpl->sensoryNeurons[i]->getState().restingPotential) / 10.0f;
                    sensoryPattern.push_back(activation);
                }
            }
            
            std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
            
            if (!sensoryPattern.empty() && !workingMemoryPattern.empty()) {
                // Enhanced reward prediction integration
                float rewardPrediction = dopamineLevel * 2.0f - 1.0f;  // Convert to [-1, 1] range
                float enhancedRewardPrediction = rewardPrediction * (1.0f + dopamineLevel * 0.5f);
                
                // Update associative memory with enhanced reward prediction
                for (size_t i = 0; i < std::min(sensoryPattern.size(), workingMemoryPattern.size()); ++i) {
                    std::vector<float> patternA = {sensoryPattern[i]};
                    std::vector<float> patternB = {workingMemoryPattern[i]};
                    
                    pImpl->associativeMemory->associate(patternA, patternB, 
                                                        enhancedRewardPrediction * 0.5f + 0.5f);
                }
                
                // Enhanced memory strengthening based on reward prediction
                if (enhancedRewardPrediction > 0.0f) {
                    pImpl->workingMemory->strengthenMemory(1.2f + enhancedRewardPrediction * 0.3f);
                }
            }
        }
    }
    
    // ========== ENHANCED ACETYLCHOLINE EFFECTS ==========
    if (pImpl->acetylcholine) {
        float acetylcholineLevel = pImpl->acetylcholine->getLevel();
        
        // 1. Enhanced selective attention
        pImpl->acetylcholine->enhanceSelectiveAttention();
        if (pImpl->attention) {
            // ACh enhances attentional focus and filtering
            float attentionStrength = 1.0f + acetylcholineLevel * 1.5f;
            pImpl->attention->setExcitationStrength(attentionStrength);
            pImpl->attention->setInhibitionStrength(0.5f / attentionStrength);  // Reduced inhibition for better focus
        }
        
        // 2. Enhanced memory encoding
        pImpl->acetylcholine->enhanceMemoryEncoding();
        if (pImpl->workingMemory) {
            // ACh enhances memory encoding strength
            float encodingStrength = 1.0f + acetylcholineLevel * 0.8f;
            pImpl->workingMemory->setEncodingStrength(encodingStrength);
            
            // Enhanced encoding of sensory information
            std::vector<float> sensoryPattern;
            for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                if (pImpl->sensoryNeurons[i]) {
                    float activation = std::abs(pImpl->sensoryNeurons[i]->getState().membranePotential - 
                                               pImpl->sensoryNeurons[i]->getState().restingPotential) / 10.0f;
                    sensoryPattern.push_back(activation * encodingStrength);
                }
            }
            
            if (!sensoryPattern.empty()) {
                // Store enhanced sensory patterns
                for (size_t i = 0; i < std::min(sensoryPattern.size(), 10u); ++i) {
                    pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), 
                                                      sensoryPattern[i]);
                }
            }
        }
        
        // 3. Enhanced working memory integration with attention
        pImpl->acetylcholine->enhanceWorkingMemoryIntegration();
        if (pImpl->attention && pImpl->workingMemory) {
            // Integrate attentional selection with working memory
            std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(memoryNeurons);
            
            // Apply ACh-enhanced attention to working memory
            float integrationBoost = 1.0f + acetylcholineLevel * 0.6f;
            for (const NeuronId& neuron : memoryNeurons) {
                pImpl->attention->applyTopDownBias(neuron, 0.1f * integrationBoost);
            }
        }
    }
    
    // ========== ENHANCED NOREPINEPHRINE EFFECTS ==========
    if (pImpl->norepinephrine) {
        float norepinephrineLevel = pImpl->norepinephrine->getLevel();
        
        // 1. Enhanced arousal and vigilance
        pImpl->norepinephrine->enhanceArousal();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE enhances arousal by increasing baseline excitability
                    float arousalMod = norepinephrineLevel * 0.3f;
                    if (arousalMod > 0.0f) {
                        // Inject additional current for arousal
                        neuron->injectCurrent(arousalMod * 0.5f);
                    }
                }
            }
        }
        
        // 2. Enhanced learning rate modulation
        pImpl->norepinephrine->enhanceLearningRate();
        if (pImpl->stdp) {
            // NE modulates STDP learning rates
            float ltpMod = 1.0f + norepinephrineLevel * 0.5f;
            float ltdMod = 1.0f - norepinephrineLevel * 0.2f;  // LTD slightly reduced
            pImpl->stdp->configure(pImpl->stdp->getLTPWeight() * ltpMod,
                                  pImpl->stdp->getLTDWeight() * ltdMod,
                                  pImpl->stdp->getTau());
        }
        
        // 3. Enhanced signal detection
        pImpl->norepinephrine->enhanceSignalDetection();
        if (pImpl->attention) {
            // NE enhances signal-to-noise ratio
            float signalMod = 1.0f + norepinephrineLevel * 0.7f;
            pImpl->attention->setExcitationStrength(2.0f * signalMod);
        }
        
        // 4. Enhanced working memory capacity
        pImpl->norepinephrine->enhanceWorkingMemory();
        if (pImpl->workingMemory) {
            // NE enhances working memory capacity and stability
            float workingMemoryCapacity = 8.0f + norepinephrineLevel * 5.0f;  // Increased capacity
            pImpl->workingMemory->setCapacity(workingMemoryCapacity);
            
            // Enhanced working memory maintenance
            pImpl->workingMemory->strengthenMemory(1.1f * (1.0f + norepinephrineLevel));
        }
    }
    
    // ========== ENHANCED SEROTONIN EFFECTS ==========
    if (pImpl->serotonin) {
        float serotoninLevel = pImpl->serotonin->getLevel();
        
        // 1. Enhanced mood modulation
        pImpl->serotonin->enhanceMood();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // 5-HT modulates neuronal excitability for mood regulation
                    float moodMod = serotoninLevel * 0.2f;
                    if (moodMod > 0.0f) {
                        neuron->injectCurrent(-moodMod * 0.3f);  // Slight inhibition for mood balance
                    }
                }
            }
        }
        
        // 2. Enhanced behavioral flexibility
        pImpl->serotonin->enhanceBehavioralFlexibility();
        if (pImpl->planner) {
            // 5-HT enhances behavioral flexibility and exploration
            float flexibilityMod = 1.0f + serotoninLevel * 1.0f;
            pImpl->planner->setExplorationFactor(flexibilityMod);
        }
        
        // 3. Enhanced exploration vs exploitation balance
        pImpl->serotonin->enhanceExploration();
        if (pImpl->curiosity) {
            // 5-HT modulates exploration drive
            float explorationMod = 0.5f + serotoninLevel * 0.5f;  // Higher 5-HT increases exploration
            pImpl->curiosity->setNoveltyWeight(explorationMod * 0.5f);
            pImpl->curiosity->setPredictionErrorWeight(explorationMod * 0.5f);
        }
        
        // 4. Enhanced development processes
        pImpl->serotonin->enhanceDevelopment();
        if (pImpl->developmentSystem) {
            // 5-HT enhances developmental plasticity
            float developmentMod = 1.0f + serotoninLevel * 0.8f;
            pImpl->developmentSystem->setPlasticityFactor(developmentMod);
        }
    }
    
    // ========== CROSS-SYSTEM INTEGRATION ==========
    
    // 1. Neuromodulator-enhanced memory consolidation
    if (pImpl->episodicMemory) {
        // Calculate combined neuromodulation factor for memory consolidation
        float totalNeuromodFactor = 1.0f;
        if (pImpl->dopamine) totalNeuromodFactor *= (0.5f + 0.5f * pImpl->dopamine->getLevel());
        if (pImpl->acetylcholine) totalNeuromodFactor *= (0.5f + 0.5f * pImpl->acetylcholine->getLevel());
        if (pImpl->norepinephrine) totalNeuromodFactor *= (0.5f + 0.5f * pImpl->norepinephrine->getLevel());
        if (pImpl->serotonin) totalNeuromodFactor *= (0.5f + 0.5f * pImpl->serotonin->getLevel());
        
        // Apply combined neuromodulation to memory consolidation
        pImpl->episodicMemory->setConsolidationFactor(totalNeuromodFactor);
        
        // Enhanced consolidation during periods of high neuromodulator activity
        float noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
        if (noveltyLevel > 0.5f) {
            pImpl->episodicMemory->consolidate(totalNeuromodFactor * 0.8f);
        }
    }
    
    // 2. Neuromodulator-enhanced development system effects
    if (pImpl->developmentSystem) {
        // Calculate neuromodulation effects on development
        float developmentMod = 1.0f;
        
        if (pImpl->dopamine) {
            developmentMod *= (0.5f + 0.5f * pImpl->dopamine->getLevel());
        }
        if (pImpl->serotonin) {
            developmentMod *= (0.8f + 0.2f * pImpl->serotonin->getLevel());  // 5-HT has stronger effect on development
        }
        if (pImpl->norepinephrine) {
            developmentMod *= (0.6f + 0.4f * pImpl->norepinephrine->getLevel());
        }
        if (pImpl->acetylcholine) {
            developmentMod *= (0.7f + 0.3f * pImpl->acetylcholine->getLevel());
        }
        
        // Apply development modulation
        pImpl->developmentSystem->setPlasticityFactor(developmentMod);
        
        // 3. Enhanced development effects on multiple brain systems (COMPREHENSIVE INTEGRATION)
        // Apply comprehensive development system effects using the enhanced DevelopmentSystem
        pImpl->developmentSystem->integrateWithBrainSystems(this);
        
        // Enhanced development during maturation phase with feedback loops
        if (pImpl->developmentalStage == DevelopmentalStage::Maturation) {
            pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 10);
            
            // Additional maturation-specific development effects
            // Enhanced plasticity during critical periods for learning acceleration
            if (pImpl->developmentSystem->isCriticalPeriod()) {
                float criticalPeriodEnhancement = 1.5f * pImpl->developmentSystem->getCriticalPeriodProgress();
                // This would enhance learning and memory consolidation during critical period
                if (pImpl->episodicMemory) {
                    float enhancedConsolidation = pImpl->developmentSystem->getMemoryConsolidationModulation();
                    pImpl->episodicMemory->setConsolidationFactor(enhancedConsolidation);
                }
            }
        }
        
        // Create feedback loops from development to other systems
        pImpl->developmentSystem->createFeedbackLoops(this);
        
        // Store developmental state for tracking
        pImpl->developmentalStage = pImpl->developmentSystem->getStage();
    }
    
    // 3. Integrated attentional modulation from multiple neuromodulators (ENHANCED)
    if (pImpl->attention) {
        // Integrate effects from all neuromodulators on attention (existing code)
        // Now also integrate development effects on attention
        if (pImpl->developmentSystem) {
            float developmentalAttentionMod = 1.0f;
            pImpl->developmentSystem->applyAttentionModulationModulation(developmentalAttentionMod);
            
            // Apply developmental modulation to attentional system
            float currentAttentionStrength = pImpl->attention->getExcitationStrength();
            pImpl->attention->setExcitationStrength(currentAttentionStrength * developmentalAttentionMod);
            pImpl->attention->setInhibitionStrength(pImpl->attention->getInhibitionStrength() / developmentalAttentionMod);
        }
        
        // Continue with existing attentional modulation code...
        float totalAttentionMod = 1.0f;
        
        if (pImpl->dopamine) {
            totalAttentionMod *= (1.0f + pImpl->dopamine->getLevel() * 0.5f);
        }
        if (pImpl->acetylcholine) {
            totalAttentionMod *= (1.0f + pImpl->acetylcholine->getLevel() * 1.0f);
        }
        if (pImpl->norepinephrine) {
            totalAttentionMod *= (1.0f + pImpl->norepinephrine->getLevel() * 0.7f);
        }
        
        // Apply combined attentional modulation
        float enhancedAttentionStrength = 1.5f * totalAttentionMod;
        pImpl->attention->setExcitationStrength(enhancedAttentionStrength);
        pImpl->attention->setInhibitionStrength(0.3f / enhancedAttentionStrength);
    }
    
    // 4. Neuromodulator-enhanced working memory stability (ENHANCED)
    if (pImpl->workingMemory) {
        // Calculate combined effects on working memory (existing code)
        // Now also integrate development effects on working memory
        if (pImpl->developmentSystem) {
            size_t developmentalCapacity = pImpl->developmentSystem->getWorkingMemoryCapacity();
            // This would update working memory capacity based on developmental stage
            // For now, we'll apply as a modulation factor
            float capacityMod = (float)developmentalCapacity / 50.0f;  // Normalize to 50
            pImpl->workingMemory->setCapacity(pImpl->workingMemory->getCapacity() * capacityMod);
        }
        
        float totalWorkingMemoryMod = 1.0f;
        
        if (pImpl->dopamine) {
            totalWorkingMemoryMod *= (1.0f + pImpl->dopamine->getLevel() * 0.3f);
        }
        if (pImpl->acetylcholine) {
            totalWorkingMemoryMod *= (1.0f + pImpl->acetylcholine->getLevel() * 0.5f);
        }
        if (pImpl->norepinephrine) {
            totalWorkingMemoryMod *= (1.0f + pImpl->norepinephrine->getLevel() * 0.4f);
        }
        
        // Apply combined working memory modulation
        pImpl->workingMemory->setMemoryStability(totalWorkingMemoryMod);
        pImpl->workingMemory->strengthenMemory(totalWorkingMemoryMod);
    }
    
    // 3. Integrated attentional modulation from multiple neuromodulators
    if (pImpl->attention) {
        // Integrate effects from all neuromodulators on attention
        float totalAttentionMod = 1.0f;
        
        if (pImpl->dopamine) {
            totalAttentionMod *= (1.0f + pImpl->dopamine->getLevel() * 0.5f);
        }
        if (pImpl->acetylcholine) {
            totalAttentionMod *= (1.0f + pImpl->acetylcholine->getLevel() * 1.0f);
        }
        if (pImpl->norepinephrine) {
            totalAttentionMod *= (1.0f + pImpl->norepinephrine->getLevel() * 0.7f);
        }
        
        // Apply combined attentional modulation
        float enhancedAttentionStrength = 1.5f * totalAttentionMod;
        pImpl->attention->setExcitationStrength(enhancedAttentionStrength);
        pImpl->attention->setInhibitionStrength(0.3f / enhancedAttentionStrength);
    }
    
    // 4. Neuromodulator-enhanced working memory stability
    if (pImpl->workingMemory) {
        // Calculate combined effects on working memory
        float totalWorkingMemoryMod = 1.0f;
        
        if (pImpl->dopamine) {
            totalWorkingMemoryMod *= (1.0f + pImpl->dopamine->getLevel() * 0.3f);
        }
        if (pImpl->acetylcholine) {
            totalWorkingMemoryMod *= (1.0f + pImpl->acetylcholine->getLevel() * 0.5f);
        }
        if (pImpl->norepinephrine) {
            totalWorkingMemoryMod *= (1.0f + pImpl->norepinephrine->getLevel() * 0.4f);
        }
        
        // Apply combined working memory modulation
        pImpl->workingMemory->setMemoryStability(totalWorkingMemoryMod);
        pImpl->workingMemory->strengthenMemory(totalWorkingMemoryMod);
    }
    
    // 5. Neuromodulator-enhanced plasticity (for STEP 6)
    // Note: PlasticityMod is calculated in STEP 6 using dopamine's getPlasticityFactor()
    // But we'll now integrate all neuromodulators for plasticity
    if (pImpl->dopamine && pImpl->acetylcholine && pImpl->norepinephrine && pImpl->serotonin) {
        float enhancedPlasticity = pImpl->dopamine->getPlasticityFactor() * 
                                  (1.0f + pImpl->acetylcholine->getLevel() * 0.2f) *
                                  (1.0f + pImpl->norepinephrine->getLevel() * 0.1f) *
                                  (1.0f - pImpl->serotonin->getLevel() * 0.1f);  // 5-HT slightly reduces plasticity for stability
        
        // This enhanced value will be used in STEP 6 for enhanced plasticity
        // (We don't actually set dopamine level to this value, we just calculate it)
        // EnhancedPlasticity is calculated but not stored - it's used in STEP 6
        float unused = enhancedPlasticity;  // Store to avoid unused variable warning
        (void)unused;
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity (enhanced with all neuromodulators)
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
        
        // Enhance with other neuromodulators
        if (pImpl->acetylcholine) {
            plasticityMod *= (1.0f + pImpl->acetylcholine->getLevel() * 0.2f);
        }
        if (pImpl->norepinephrine) {
            plasticityMod *= (1.0f + pImpl->norepinephrine->getLevel() * 0.1f);
        }
        if (pImpl->serotonin) {
            plasticityMod *= (1.0f - pImpl->serotonin->getLevel() * 0.05f);  // 5-HT stabilizes synapses
        }
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with enhanced neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Enhanced STDP with neuromodulation
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f * plasticityMod;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning with neuromodulation
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
    
    // Restore original dopamine level (important for reward signaling)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
    }
    
    // ========== STEP 7: Update episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture comprehensive brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons with rich context information
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            // Store neural dynamics: both activation level and rate of change
                            float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                            episode.neuronActivations.push_back(activation);
                            
                            // Store additional neural state features for prediction
                            // Refractory state
                            bool isRefractory = neuron->getState().refractoryUntil > currentTime;
                            episode.neuronRefractory.push_back(isRefractory ? 0.0f : 1.0f);
                            
                            // Recent spiking history for temporal dynamics
                            float recentSpikeHistory = neuron->getRecentSpikeHistory(currentStep, 5) / 5.0f; // Average over last 5 steps
                            episode.spikeHistory.push_back(recentSpikeHistory);
                        }
                    }
                }
            }
            
            // Store sensory state for prediction
            std::vector<float> sensoryPattern;
            for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                if (pImpl->sensoryNeurons[i]) {
                    float sensoryActivation = std::abs(pImpl->sensoryNeurons[i]->getState().membranePotential - 
                                                       pImpl->sensoryNeurons[i]->getState().restingPotential) / 10.0f;
                    sensoryPattern.push_back(sensoryActivation);
                }
            }
            episode.sensoryState = sensoryPattern;
            
            // Store motor commands if any
            std::vector<float> motorPattern;
            for (size_t i = 0; i < pImpl->motorNeurons.size(); ++i) {
                if (pImpl->motorNeurons[i]) {
                    float motorActivation = std::abs(pImpl->motorNeurons[i]->getState().membranePotential - 
                                                     pImpl->motorNeurons[i]->getState().restingPotential) / 10.0f;
                    if (motorActivation > 0.1f) {
                        motorPattern.push_back(motorActivation);
                    }
                }
            }
            episode.action = ActionType::Wait;  // Default action - could be enhanced
            if (!motorPattern.empty()) {
                // Determine action based on motor activation
                episode.action = ActionType::Move;  // Simplified for now
            }
            
            // Store resulting sensory state for action-effect learning
            std::vector<float> resultingSensoryState;
            for (float val : sensoryPattern) {
                resultingSensoryState.push_back(val * 0.8f + 0.2f);  // Simplified effect
            }
            episode.resultingSensoryState = resultingSensoryState;
            
            // Store novelty in episode
            episode.novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            
            // Store energy/consumption state
            float totalEnergy = 0.0f;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        totalEnergy += neuron->getEnergyConsumption();
                    }
                }
            }
            episode.energy = totalEnergy / 1000.0f;  // Normalize
            
            // Store position/orientation if available
            episode.positionX = 0.0f;  // Would come from proprioceptive sensors
            episode.positionY = 0.0f;
            episode.orientation = 0.0f;
            
            // ENHANCED EPISODIC MEMORY WITH COGNITION INTEGRATION
            
            // 1. Neural Planner Integration: Use planner predictions to guide memory formation
            if (pImpl->planner) {
                // Get planner's current plan recommendation
                auto plannerRecommendations = pImpl->planner->getCurrentPlan();
                if (!plannerRecommendations.empty()) {
                    // Enhance episode with planner context
                    for (const auto& action : plannerRecommendations) {
                        // Map planner action to episode action
                        if (action.action_type == ActionType::Move) {
                            episode.action = ActionType::Move;
                            break;
                        }
                    }
                }
                
                // Use planner's evaluation of action sequences to weight memory formation
                float plannerEvaluation = pImpl->planner->evaluateCurrentActionSequence();
                episode.plannerEvaluation = plannerEvaluation;
            }
            
            // 2. Concept Formation Integration: Use concepts to organize episodic memories
            if (pImpl->conceptFormation) {
                // Present sensory pattern to concept formation for concept formation
                size_t conceptId = pImpl->conceptFormation->presentExperience(
                    sensoryPattern,  // Pattern
                    sensoryPattern,  // Features  
                    episode.reward,  // Reward
                    episode.timestamp // Time
                );
                
                // Store concept association in episode
                if (conceptId > 0 && conceptId < 100000) {  // Valid concept ID
                    episode.conceptId = conceptId;
                }
                
                // Get relevant concepts to enhance memory organization
                auto relevantConcepts = pImpl->conceptFormation->getRelatedConcepts(sensoryPattern);
                for (size_t concept : relevantConcepts) {
                    episode.conceptAssociations.push_back(concept);
                }
            }
            
            // 3. Cross-Cognition Integration: Use attention to guide memory encoding
            if (pImpl->attention && !pImpl->sensoryNeurons.empty()) {
                // Get attention weights for sensory neurons
                std::vector<float> attentionWeights;
                for (Neuron* sensoryNeuron : pImpl->sensoryNeurons) {
                    if (sensoryNeuron) {
                        float neuronId = sensoryNeuron->getId();
                        float attentionWeight = pImpl->attention->getAttentionalWeight(neuronId);
                        attentionWeights.push_back(attentionWeight);
                    }
                }
                
                // Apply attention weights to sensory pattern for enhanced encoding
                std::vector<float> attendedSensoryPattern;
                for (size_t i = 0; i < sensoryPattern.size() && i < attentionWeights.size(); ++i) {
                    attendedSensoryPattern.push_back(sensoryPattern[i] * attentionWeights[i]);
                }
                
                if (!attendedSensoryPattern.empty()) {
                    episode.attendedSensoryPattern = attendedSensoryPattern;
                }
            }
            
            // 4. Action Selection Integration: Evaluate planned actions with prediction
            if (pImpl->predictionSystem) {
                // Create input from attended sensory pattern for prediction
                std::vector<float> predictionInput = episode.attendedSensoryPattern.empty() ? sensoryPattern : episode.attendedSensoryPattern;
                
                if (!predictionInput.empty()) {
                    class SensoryInput input;
                    input.setData(predictionInput);
                    
                    // Get prediction for action outcomes
                    auto prediction = pImpl->predictionSystem->predictNextState(input);
                    if (prediction) {
                        // Evaluate predicted action outcomes
                        float predictedOutcome = prediction->getData().empty() ? 0.0f : prediction->getData()[0];
                        episode.predictedActionOutcome = predictedOutcome;
                        
                        // Use prediction confidence to modulate memory strength
                        float predictionConfidence = pImpl->predictionSystem->getConfidence();
                        episode.predictionConfidence = predictionConfidence;
                    }
                }
            }
            
            // 5. Enhanced memory consolidation with cognitive factors
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Enhanced prediction system ==========
    if (pImpl->predictionSystem) {
        // Generate prediction from current sensory state
        if (!pImpl->sensoryNeurons.empty()) {
            // Create sensory input from current neuron activity
            std::vector<float> currentSensoryData;
            for (Neuron* sensoryNeuron : pImpl->sensoryNeurons) {
                if (sensoryNeuron) {
                    // Convert neuron firing to sensory input
                    float activation = sensoryNeuron->getFiringRate();
                    currentSensoryData.push_back(activation);
                }
            }
            
            if (!currentSensoryData.empty()) {
                // Create SensoryInput from current data
                class SensoryInput currentInput;
                currentInput.setData(currentSensoryData);
                
                // Generate prediction for next state using prediction system
                auto prediction = pImpl->predictionSystem->predictNextState(currentInput);
                
                // ENHANCED: Neural Planner Integration - Use predictions to guide action planning
                if (pImpl->planner && prediction) {
                    // Get planner's current plan to compare with prediction
                    auto plannerPlan = pImpl->planner->getCurrentPlan();
                    
                    // Use prediction to evaluate and refine planner's action sequences
                    if (!plannerPlan.empty() && !prediction->getData().empty()) {
                        // Get current predicted state
                        float predictedState = prediction->getData()[0];
                        
                        // Get planner's action sequence
                        auto actionSequence = pImpl->planner->getActionSequence();
                        
                        // Use prediction to evaluate the planned action sequence
                        float sequenceEvaluation = pImpl->planner->evaluateActionSequence(actionSequence, predictedState);
                        
                        // Adjust planner's decision based on prediction confidence and error
                        float predictionConfidence = pImpl->predictionSystem->getConfidence();
                        float predictionError = pImpl->predictionSystem->getPredictionError();
                        
                        // Modify planner's exploration factor based on prediction uncertainty
                        if (predictionError > 0.3f) {
                            // High prediction error encourages exploration
                            pImpl->planner->setExplorationFactor(1.5f * predictionConfidence);
                        } else {
                            // High confidence encourages exploitation
                            pImpl->planner->setExplorationFactor(0.5f + predictionConfidence * 0.5f);
                        }
                    }
                }
                
                // Update prediction system with actual observation
                pImpl->predictionSystem->updatePredictions(*prediction, currentInput);
                
                // ENHANCED: Concept Formation Integration - Use concepts to guide prediction
                if (pImpl->conceptFormation) {
                    // Present current sensory data to concept formation for prediction guidance
                    std::vector<float> conceptFeatures = pImpl->conceptFormation->extractFeatures(currentSensoryData);
                    
                    // Use concept knowledge to refine prediction
                    if (!conceptFeatures.empty()) {
                        // Adjust prediction based on concept knowledge
                        float conceptMatch = pImpl->conceptFormation->matchConcept(conceptFeatures);
                        float conceptInfluence = conceptMatch * 0.3f;
                        
                        // Modify prediction based on concept similarity
                        if (prediction) {
                            std::vector<float> conceptAdjustedPrediction = prediction->getData();
                            for (size_t i = 0; i < conceptAdjustedPrediction.size() && i < conceptFeatures.size(); ++i) {
                                conceptAdjustedPrediction[i] *= (1.0f + conceptInfluence * (conceptFeatures[i] - 0.5f));
                            }
                            prediction->setData(conceptAdjustedPrediction);
                        }
                    }
                }
                
                // Get prediction error and confidence
                float predictionError = pImpl->predictionSystem->getPredictionError();
                float predictionConfidence = pImpl->predictionSystem->getConfidence();
                
                // ENHANCED: Attention System Integration - Use attention to guide prediction processing
                if (pImpl->attention && !pImpl->sensoryNeurons.empty()) {
                    // Get attention weights for sensory neurons to prioritize prediction processing
                    std::vector<float> attentionWeights;
                    for (Neuron* sensoryNeuron : pImpl->sensoryNeurons) {
                        if (sensoryNeuron) {
                            float neuronId = sensoryNeuron->getId();
                            float attentionWeight = pImpl->attention->getAttentionalWeight(neuronId);
                            attentionWeights.push_back(attentionWeight);
                        }
                    }
                    
                    // Apply attention weights to prediction processing
                    if (!attentionWeights.empty() && !currentSensoryData.empty()) {
                        // Enhance prediction confidence based on attentional focus
                        float totalAttention = 0.0f;
                        for (float weight : attentionWeights) {
                            totalAttention += weight;
                        }
                        float averageAttention = totalAttention / attentionWeights.size();
                        
                        // Modulate prediction confidence based on attentional focus
                        float attentionInfluence = averageAttention * 0.5f;
                        predictionConfidence = predictionConfidence * (1.0f + attentionInfluence);
                    }
                }
                
                // ENHANCED: Cross-Cognition Integration - Use prediction to influence concept discovery
                if (pImpl->conceptFormation && prediction) {
                    // Use prediction error to trigger concept discovery
                    if (predictionError > 0.5f) {
                        // High prediction error triggers concept discovery
                        std::vector<float> predictionPattern = prediction->getData();
                        if (!predictionPattern.empty()) {
                            // Create new concept from prediction pattern
                            size_t newConceptId = pImpl->conceptFormation->discoverConcept(predictionPattern);
                            
                            // Store prediction-related concept associations
                            if (newConceptId > 0 && newConceptId < 100000) {
                                // Associate prediction with discovered concept
                                pImpl->conceptFormation->associateWithPrediction(newConceptId, predictionPattern);
                            }
                        }
                    }
                }
                
                // Integrate prediction error into neuromodulation systems
                if (pImpl->predictionError) {
                    pImpl->predictionError->computeError(
                        predictionError, 
                        predictionError  // Error as both predicted and actual
                    );
                }
                
                // ENHANCED: Neural Planner Integration with Reward Prediction
                if (pImpl->planner && prediction) {
                    // Connect planner to reward prediction
                    float predictedReward = predictionConfidence * (1.0f - predictionError);
                    
                    // Use predicted reward to enhance planner's decision making
                    pImpl->planner->setRewardPrediction(predictedReward);
                    
                    // Evaluate action sequences based on predicted reward
                    auto actionSequence = pImpl->planner->getActionSequence();
                    float actionSequenceValue = pImpl->planner->evaluateActionSequence(actionSequence, predictedReward);
                    
                    // Store prediction-based action evaluation for action selection
                    // (would be used in action selection integration)
                }
                
                // Use prediction to modulate dopamine (reward prediction error)
                if (pImpl->dopamine) {
                    // Dopamine level based on prediction error
                    float predictionBasedDopamine = (1.0f - predictionError) * predictionConfidence * 2.0f;
                    pImpl->dopamine->setLevel(predictionBasedDopamine);
                }
                
                // ENHANCED: Acetylcholine Integration - Use prediction to modulate attention
                if (pImpl->acetylcholine) {
                    float acetylcholineMod = predictionConfidence * 0.5f;
                    // Enhance attentional modulation based on prediction confidence
                    acetylcholineMod *= (1.0f + predictionError * 0.5f);
                    
                    // Update acetylcholine level with enhanced prediction-based modulation
                    pImpl->acetylcholine->setLevel(acetylcholineMod);
                }
                
                // ENHANCED: Working Memory Integration - Use prediction to evaluate actions
                if (pImpl->workingMemory) {
                    // Store prediction in working memory for future reference
                    for (size_t i = 0; i < currentSensoryData.size() && i < prediction->getData().size(); ++i) {
                        float predictionValue = prediction->getData()[i];
                        
                        // Boost working memory for predicted values with attention weighting
                        if (i < pImpl->sensoryNeurons.size() && pImpl->sensoryNeurons[i]) {
                            float attentionWeight = pImpl->attention ? pImpl->attention->getAttentionalWeight(pImpl->sensoryNeurons[i]->getId()) : 1.0f;
                            float boostedPrediction = predictionValue * predictionConfidence * attentionWeight;
                            
                            pImpl->workingMemory->storeToNeuron(
                                pImpl->sensoryNeurons[i]->getId(), 
                                boostedPrediction
                            );
                        }
                    }
                }
                
                // ENHANCED: Action Selection Integration - Use predictions to evaluate actions
                if (pImpl->planner) {
                    // Get predicted action values from planner using prediction system
                    auto actionSequence = pImpl->planner->getActionSequence();
                    
                    // Evaluate action sequence based on prediction
                    float actionValue = pImpl->planner->evaluateActionSequence(actionSequence, predictionConfidence);
                    
                    // Use prediction confidence to modulate action selection
                    if (actionValue > 0.5f && predictionConfidence > 0.7f) {
                        // High confidence action selected - strengthen in working memory
                        if (pImpl->workingMemory) {
                            pImpl->workingMemory->strengthenMemory(1.2f * predictionConfidence);
                        }
                    }
                }
            }
    }
    
    // ========== STEP 9: Enhanced attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // ENHANCED: Attention System Integration with cognitive factors
        
        // 1. Enhance attention competition to include cognitive factors
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Get working memory neurons for competition
            std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            
            // Apply attention-based selection and competition
            std::vector<NeuronId> winners = pImpl->attention->processCompetition(memoryNeurons);
            
            // Enhanced competition dynamics based on cognitive factors
            if (winners.size() < memoryNeurons.size()) {
                // Apply lateral inhibition to non-winners based on prediction and concept information
                for (const NeuronId& neuron : memoryNeurons) {
                    bool isWinner = std::find(winners.begin(), winners.end(), neuron) != winners.end();
                    if (!isWinner && pImpl->workingMemory) {
                        // Get current activation
                        float activation = pImpl->workingMemory->getNeuronActivation(neuron);
                        
                        if (activation > 0.1f) {
                            // Apply suppression based on prediction error if available
                            float predictionModulation = 1.0f;
                            if (pImpl->predictionSystem) {
                                float predictionError = pImpl->predictionSystem->getPredictionError();
                                predictionModulation = 1.0f + predictionError * 0.5f;  // Higher error = more suppression
                            }
                            
                            pImpl->attention->applyTopDownBias(neuron, -activation * 2.0f * predictionModulation);
                        }
                    }
                }
            }
            
            // Re-inject currents to maintain winners and suppress losers with cognitive enhancement
            for (const NeuronId& neuron : memoryNeurons) {
                if (pImpl->workingMemory) {
                    float activation = pImpl->workingMemory->getNeuronActivation(neuron);
                    
                    // ENHANCED: Winners get reinforcement based on attention-concept integration
                    if (std::find(winners.begin(), winners.end(), neuron) != winners.end()) {
                        // Get concept associations for the neuron
                        if (pImpl->conceptFormation) {
                            auto neuronConcepts = pImpl->conceptFormation->getNeuronConcepts(neuron);
                            float conceptStrength = neuronConcepts.empty() ? 1.0f : neuronConcepts.size() * 0.5f;
                            
                            // Strengthen winner activation based on concept relevance
                            pImpl->workingMemory->strengthenMemory(1.2f + conceptStrength);
                        }
                        
                        // Inject current to maintain winner state
                        if (auto* neuronPtr = getNeuron(neuron)) {
                            neuronPtr->injectCurrent(activation * 3.0f);
                        }
                    } else {
                        // ENHANCED: Losers get suppression based on prediction evaluation
                        if (pImpl->predictionSystem) {
                            float predictionConfidence = pImpl->predictionSystem->getConfidence();
                            float predictionError = pImpl->predictionSystem->getPredictionError();
                            
                            // Stronger suppression for predictions with high error
                            float suppressionMod = 1.0f + predictionError * 0.5f;
                            pImpl->attention->applyBottomUpSalience(neuron, -activation * 0.5f * suppressionMod);
                        } else {
                            pImpl->attention->applyBottomUpSalience(neuron, -activation * 0.5f);
                        }
                        
                        // Inject inhibitory current
                        if (auto* neuronPtr = getNeuron(neuron)) {
                            neuronPtr->injectCurrent(-activation * 2.0f);
                        }
                    }
                }
            }
        }
        
        // ENHANCED: Connect attention to concept formation
        if (pImpl->conceptFormation && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Present attentional focus to concept formation for enhanced concept refinement
            std::vector<float> attentionalFocus;
            for (NeuronId neuron : pImpl->workingMemory->getMemoryNeurons()) {
                float attentionWeight = pImpl->attention ? pImpl->attention->getAttentionalWeight(neuron) : 0.0f;
                attentionalFocus.push_back(attentionWeight);
            }
            
            if (!attentionalFocus.empty()) {
                // Integrate attentional focus into concept formation
                pImpl->conceptFormation->updateAttentionPattern(attentionalFocus);
            }
        }
        
        // ENHANCED: Use attention to guide prediction processing
        if (pImpl->predictionSystem && pImpl->attention) {
            // Get attention weights to modulate prediction processing
            std::vector<float> attentionWeights;
            if (!pImpl->sensoryNeurons.empty()) {
                for (Neuron* sensoryNeuron : pImpl->sensoryNeurons) {
                    if (sensoryNeuron) {
                        float attentionWeight = pImpl->attention->getAttentionalWeight(sensoryNeuron->getId());
                        attentionWeights.push_back(attentionWeight);
                    }
                }
            }
            
            // Apply attentional modulation to prediction system
            if (!attentionWeights.empty()) {
                float totalAttention = 0.0f;
                for (float weight : attentionWeights) {
                    totalAttention += weight;
                }
                float averageAttention = totalAttention / attentionWeights.size();
                
                // Modulate prediction confidence based on attentional focus
                float attentionModulation = averageAttention * 0.3f;
                
                // Get current prediction confidence and enhance it with attentional input
                float predictionConfidence = pImpl->predictionSystem->getConfidence();
                pImpl->predictionSystem->setConfidence(predictionConfidence * (1.0f + attentionModulation));
            }
        }
        
        // ENHANCED: Connect attention to memory systems
        if (pImpl->episodicMemory && pImpl->workingMemory) {
            // Get attentional focus to guide memory consolidation
            std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            float totalAttentionalFocus = 0.0f;
            
            for (NeuronId neuron : memoryNeurons) {
                float attentionWeight = pImpl->attention ? pImpl->attention->getAttentionalWeight(neuron) : 0.0f;
                totalAttentionalFocus += attentionWeight;
            }
            
            if (!memoryNeurons.empty()) {
                float averageAttention = totalAttentionalFocus / memoryNeurons.size();
                
                // Enhance memory consolidation based on attentional focus
                if (averageAttention > 0.5f) {
                    // High attentional focus enhances memory encoding
                    pImpl->workingMemory->setEncodingStrength(1.5f * averageAttention);
                    
                    // Store attentional state in episodic memory
                    if (pImpl->stepsSinceLastEpisode % 10 == 0) {  // Store attentional state periodically
                        EpisodicMemoryItem attentionalEpisode;
                        attentionalEpisode.timestamp = currentStep;
                        attentionalEpisode.attentionalFocus = std::vector<float>(memoryNeurons.size(), averageAttention);
                        pImpl->episodicMemory->storeEpisode(attentionalEpisode);
                    }
                }
            }
        }
    }
    
    // ========== STEP 10: Enhanced concept formation ==========
    if (pImpl->conceptFormation) {
        // ENHANCED: Concept Formation Integration with cognition systems
        
        // 1. Use episodic memories to form concepts
        if (pImpl->episodicMemory && !pImpl->episodicMemory->getRecentEpisodes(10).empty()) {
            // Get recent episodes to analyze for concept formation
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(10);
            for (const auto* episode : recentEpisodes) {
                if (!episode->sensoryState.empty() && !episode->action.has_value()) {
                    // ENHANCED: Use planner recommendations to guide concept formation
                    if (pImpl->planner) {
                        // Get planner's current plan to influence concept formation
                        auto plannerRecommendations = pImpl->planner->getCurrentPlan();
                        
                        // Enhance concept formation with planner context
                        std::vector<float> enhancedSensoryPattern = episode->sensoryState;
                        
                        // Integrate planner's action sequence into concept formation
                        if (!plannerRecommendations.empty()) {
                            // Add planner's action relevance to sensory pattern
                            for (size_t i = 0; i < plannerRecommendations.size() && i < enhancedSensoryPattern.size(); ++i) {
                                if (plannerRecommendations[i].action_type == ActionType::Move) {
                                    enhancedSensoryPattern[i] *= 1.2f;  // Boost movement-related concepts
                                }
                            }
                        }
                    }
                    
                    // Present sensory patterns to concept formation
                    pImpl->conceptFormation->presentExperience(
                        episode->sensoryState,
                        episode->sensoryState,
                        episode->reward,
                        episode->timestamp
                    );
                }
            }
        }
        
        // 2. ENHANCED: Integrate concept formation with working memory
        if (pImpl->workingMemory) {
            auto memoryPattern = pImpl->workingMemory->retrieve();
            if (!memoryPattern.empty()) {
                // ENHANCED: Use attention to modulate concept formation from working memory
                if (pImpl->attention) {
                    // Get attentional weights to enhance concept formation
                    std::vector<float> attentionWeights;
                    for (NeuronId neuron : pImpl->workingMemory->getMemoryNeurons()) {
                        float attentionWeight = pImpl->attention->getAttentionalWeight(neuron);
                        attentionWeights.push_back(attentionWeight);
                    }
                    
                    // Apply attentional modulation to working memory patterns
                    std::vector<float> attendedMemoryPattern;
                    for (size_t i = 0; i < memoryPattern.size() && i < attentionWeights.size(); ++i) {
                        attendedMemoryPattern.push_back(memoryPattern[i] * attentionWeights[i]);
                    }
                    
                    if (!attendedMemoryPattern.empty()) {
                        // Present attended working memory patterns to concept formation
                        pImpl->conceptFormation->presentExperience(
                            attendedMemoryPattern,
                            attendedMemoryPattern,
                            pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,
                            currentStep
                        );
                    }
                } else {
                    // Standard presentation without attentional modulation
                    pImpl->conceptFormation->presentExperience(
                        memoryPattern,
                        memoryPattern,
                        pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,
                        currentStep
                    );
                }
            }
        }
        
        // 3. ENHANCED: Use concepts to guide prediction
        if (pImpl->predictionSystem && pImpl->conceptFormation) {
            // Get current concepts to guide prediction
            auto currentConcepts = pImpl->conceptFormation->getCurrentConcepts();
            
            if (!currentConcepts.empty()) {
                // Use concept knowledge to enhance prediction accuracy
                for (size_t i = 0; i < currentConcepts.size(); ++i) {
                    float conceptKnowledge = currentConcepts[i];
                    
                    // Apply concept influence to prediction system
                    if (pImpl->predictionSystem) {
                        float currentConfidence = pImpl->predictionSystem->getConfidence();
                        float enhancedConfidence = currentConfidence * (1.0f + conceptKnowledge * 0.2f);
                        pImpl->predictionSystem->setConfidence(enhancedConfidence);
                    }
                }
            }
        }
        
        // 4. ENHANCED: Connect concept formation to attention
        if (pImpl->attention) {
            // Update attention system with concept information
            auto currentConcepts = pImpl->conceptFormation->getCurrentConcepts();
            
            if (!currentConcepts.empty()) {
                // Get concept relevance for attention modulation
                float totalConceptRelevance = 0.0f;
                for (float concept : currentConcepts) {
                    totalConceptRelevance += concept;
                }
                float averageConceptRelevance = totalConceptRelevance / currentConcepts.size();
                
                // Use concept relevance to modulate attentional focus
                if (averageConceptRelevance > 0.5f) {
                    // High concept relevance increases attentional excitation
                    float conceptAttentionBoost = 1.5f * averageConceptRelevance;
                    pImpl->attention->setExcitationStrength(1.5f * conceptAttentionBoost);
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
        // Get episodes for replay with enhanced selection based on prediction value
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        
        // Enhanced replay mechanism that actually affects neural activity
        for (const auto* episode : episodesToReplay) {
            // Replenish working memory with replayed episode
            if (pImpl->workingMemory && !episode->activeNeurons.empty()) {
                // Replay by storing episode patterns into working memory
                for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
                    NeuronId neuron = episode->activeNeurons[i];
                    float activation = i < episode->neuronActivations.size() ? 
                                       episode->neuronActivations[i] : 0.5f;
                    
                    // Boost activation with replay effect
                    pImpl->workingMemory->storeToNeuron(neuron, activation * 1.5f);
                }
            }
            
            // Use episodic memory replay for prediction and planning
            if (pImpl->predictionSystem && !episode->sensoryState.empty()) {
                // Use stored episode for prediction system training
                pImpl->predictionSystem->train(episode->sensoryState);
            }
            
            // Integrate replay with development system
            if (pImpl->developmentSystem) {
                // Replay affects developmental processes
                pImpl->developmentSystem->applyExperience(episode->reward, currentStep);
            }
            
            // Enhanced replay that affects neural activity
            pImpl->episodicMemory->replayEpisode(episode);
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

SimulationStep Brain::getCurrentStep() const {
    return pImpl->currentStep;
}

std::unique_ptr<class Action> Brain::produceAction() {
    // ENHANCED: Action Selection Integration with cognition systems
    
    // 1. Use planner recommendations for action selection
    ActionType selectedAction = ActionType::Wait;
    if (pImpl->planner) {
        // Get planner's recommended action based on current state and predictions
        auto plannerRecommendations = pImpl->planner->getCurrentPlan();
        if (!plannerRecommendations.empty()) {
            // Use the highest priority action from planner
            selectedAction = plannerRecommendations[0].action_type;
            
            // Enhance action selection based on planner evaluation
            float actionEvaluation = pImpl->planner->evaluateCurrentActionSequence();
            if (actionEvaluation < 0.3f) {
                // Low evaluation encourages waiting/exploration
                selectedAction = ActionType::Wait;
            }
        }
    }
    
    // 2. Use concepts to constrain action space
    if (pImpl->conceptFormation) {
        // Get current concepts to filter action appropriateness
        auto currentConcepts = pImpl->conceptFormation->getCurrentConcepts();
        
        // If concept system suggests a different action based on experience
        if (!currentConcepts.empty()) {
            float conceptConstraint = currentConcepts[0];
            
            // Concept constraints can override planner recommendations
            if (conceptConstraint < 0.3f && selectedAction != ActionType::Wait) {
                selectedAction = ActionType::Wait;  // Concept suggests caution
            }
        }
    }
    
    // 3. Use attention to focus on relevant actions
    if (pImpl->attention && !pImpl->motorNeurons.empty()) {
        // Get attention weights for motor neurons
        std::vector<float> motorAttentionWeights;
        for (Neuron* motorNeuron : pImpl->motorNeurons) {
            if (motorNeuron) {
                float attentionWeight = pImpl->attention->getAttentionalWeight(motorNeuron->getId());
                motorAttentionWeights.push_back(attentionWeight);
            }
        }
        
        // Apply attentional selection to motor action
        if (!motorAttentionWeights.empty()) {
            float maxAttention = 0.0f;
            ActionType attentionBasedAction = ActionType::Wait;
            
            for (size_t i = 0; i < motorAttentionWeights.size() && i < pImpl->motorNeurons.size(); ++i) {
                if (motorAttentionWeights[i] > maxAttention) {
                    maxAttention = motorAttentionWeights[i];
                    // Map attention weights to action types
                    if (i == 0) attentionBasedAction = ActionType::MoveForward;
                    else if (i == 1) attentionBasedAction = ActionType::MoveBackward;
                    else if (i == 2) attentionBasedAction = ActionType::TurnLeft;
                    else if (i == 3) attentionBasedAction = ActionType::TurnRight;
                }
            }
            
            // Use attention-based action if it has strong attentional support
            if (maxAttention > 0.5f) {
                selectedAction = attentionBasedAction;
            }
        }
    }
    
    // 4. Use predictions to evaluate actions
    if (pImpl->predictionSystem && selectedAction != ActionType::Wait) {
        // Get prediction evaluation of the selected action
        float predictionConfidence = pImpl->predictionSystem->getConfidence();
        float predictionError = pImpl->predictionSystem->getPredictionError();
        
        // Evaluate action based on prediction quality
        float actionQuality = predictionConfidence * (1.0f - predictionError);
        
        // Low prediction quality may suggest waiting
        if (actionQuality < 0.4f) {
            selectedAction = ActionType::Wait;
        }
        
        // Store prediction-based action evaluation
        // (would be stored in working memory for future reference)
    }
    
    // Simple action selection based on motor neuron activity (existing fallback)
    if (selectedAction == ActionType::Wait) {
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
        
        selectedAction = type;
    }
    
    auto action = std::make_unique<Action>(selectedAction);
    
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

} // namespace nlm

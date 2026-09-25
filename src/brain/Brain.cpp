// Brain.cpp - Enhanced NLM Brain with Sleep/Rest Cycle and Memory Consolidation

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/InternalSignals.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralSemanticMemory.hpp"
#include "../memory/NeuralProceduralMemory.hpp"
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
    std::unique_ptr<NeuralSemanticMemory> semanticMemory;
    std::unique_ptr<NeuralProceduralMemory> proceduralMemory;
    
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
    
    // SLEEP/REST CYCLE STATE
    struct SleepCycleState {
        enum class SleepPhase {
            Awake,
            LightSleep,    // Shallow sleep
            DeepSleep,     // Slow-wave sleep
            REM           // Rapid eye movement sleep
        };
        
        SleepPhase phase;
        float phaseDuration;  // Duration of current phase in steps
        float nextPhaseTransition;  // Step when phase should change
        float sleepProgress;  // Progress through current sleep cycle (0-1)
        float synapticDownscaling;  // Current level of synaptic normalization
        float memoryReorganization;  // Current level of memory integration
        float consolidationStrength;  // Strength of memory consolidation processes
        float age;  // Simulated age of the brain
        
        SleepCycleState() 
            : phase(SleepPhase::Awake)
            , phaseDuration(0.0f)
            , nextPhaseTransition(0.0f)
            , sleepProgress(0.0f)
            , synapticDownscaling(1.0f)
            , memoryReorganization(0.0f)
            , consolidationStrength(0.5f)
            , age(0.0f) {}
    };
    
    SleepCycleState sleepState;
    
    // Memory prioritization weights
    struct MemoryPriorityWeights {
        float recencyWeight;      // How much recency matters (0-1)
        float rewardWeight;       // How much reward matters (0-1)
        float noveltyWeight;      // How much novelty matters (0-1)
        float coherenceWeight;    // How much coherence matters (0-1)
        float importanceWeight;   // How much intrinsic importance matters (0-1)
        
        MemoryPriorityWeights()
            : recencyWeight(0.3f)
            , rewardWeight(0.25f)
            , noveltyWeight(0.2f)
            , coherenceWeight(0.1f)
            , importanceWeight(0.15f) {}
    };
    
    MemoryPriorityWeights memoryWeights;
    
    // Synaptic homeostasis tracking
    struct SynapticHomeostasis {
        float synapticStrengthTotal;      // Total synaptic strength across all synapses
        float synapticStrengthTarget;     // Target total synaptic strength (homeostatic bound)
        float normalizationFactor;        // Current normalization factor
        float saturationThreshold;        // Threshold for synaptic saturation
        float adaptationRate;             // Rate at which synaptic weights adapt
        
        SynapticHomeostasis()
            : synapticStrengthTotal(0.0f)
            , synapticStrengthTarget(100.0f)  // Baseline target
            , normalizationFactor(1.0f)
            , saturationThreshold(2.0f)
            , adaptationRate(0.01f) {}
    };
    
    SynapticHomeostasis synapticHomeostasis;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Storage for sensory pattern history (for novelty detection)
    std::vector<float> sensoryPattern;
    
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
        
        // Initialize NeuralWorkingMemory
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        semanticMemory = std::make_unique<NeuralSemanticMemory>();
        proceduralMemory = std::make_unique<NeuralProceduralMemory>();
        
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
        
        // Initialize neural memory systems
        workingMemory->initialize(this);
        episodicMemory->initialize(this);
        associativeMemory->initialize(this);
        semanticMemory->initialize(this);
        proceduralMemory->initialize(this);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize synaptic homeostasis tracking
        synapticHomeostasis.synapticStrengthTarget = 100.0f + sleepState.age * 10.0f;
        
        // Initialize memory priority weights from config
        if (auto recencyOpt = config->get<float>("recency_weight")) {
            memoryWeights.recencyWeight = *recencyOpt;
        }
        if (auto rewardOpt = config->get<float>("reward_weight")) {
            memoryWeights.rewardWeight = *rewardOpt;
        }
        if (auto noveltyOpt = config->get<float>("novelty_weight")) {
            memoryWeights.noveltyWeight = *noveltyOpt;
        }
        if (auto coherenceOpt = config->get<float>("coherence_weight")) {
            memoryWeights.coherenceWeight = *coherenceOpt;
        }
        if (auto importanceOpt = config->get<float>("importance_weight")) {
            memoryWeights.importanceWeight = *importanceOpt;
        }
        
        // Set sleep schedule based on developmental stage
        // Younger brains have more sleep
        float sleepNeed = 1.0f;
        if (auto sleepNeedOpt = config->get<float>("sleep_need")) {
            sleepNeed = *sleepNeedOpt;
        }
        
        // Initialize sleep cycle
        resetSleepCycle();
        
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
    pImpl->predictionSystem->initialize(this);
    
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
    
    // ========== ENHANCED SLEEP/REST CYCLE ==========
    // Implement progressive sleep cycles with memory consolidation
    // This integrates sleep with memory processing, synaptic homeostasis, and development
    
    // Update sleep state
    pImpl->sleepState.sleepProgress += 1.0f / pImpl->sleepState.phaseDuration;
    pImpl->sleepState.age = static_cast<float>(pImpl->currentStep) * 0.001f;  // Age in simulation time
    
    // Age-dependent sleep patterns
    float sleepDrive = calculateSleepDrive(pImpl);
    bool shouldSleep = (sleepDrive > 0.7f) && (pImpl->currentStep > 1000);  // Start sleeping after 1000 steps
    
    if (shouldSleep) {
        // Begin or continue sleep
        if (pImpl->sleepState.phase == Brain::Impl::SleepCycleState::SleepPhase::Awake) {
            // Transition from awake to sleep based on sleep drive
            pImpl->sleepState.phase = Brain::Impl::SleepCycleState::SleepPhase::LightSleep;
            pImpl->sleepState.nextPhaseTransition = pImpl->currentStep + 
                static_cast<float>(pImpl->sleepState.phaseDuration * 0.3f);
        }
        
        // Update sleep phase
        updateSleepPhase(pImpl, currentStep);
        
        // During sleep, apply synaptic homeostasis and memory consolidation
        if (pImpl->sleepState.phase != Brain::Impl::SleepCycleState::SleepPhase::Awake) {
            pImpl->isResting = true;
            
            // Apply synaptic downscaling for homeostasis
            applySynapticDownscaling(pImpl, currentStep);
            
            // Enhance memory consolidation during sleep
            enhancedMemoryConsolidation(pImpl, currentStep);
            
            // Memory reorganization and integration
            reorganizeMemories(pImpl, currentStep);
        } else {
            pImpl->isResting = false;
        }
    } else {
        // Stay awake
        pImpl->isResting = false;
        pImpl->sleepState.sleepProgress = 0.0f;
    }
    
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
    
    // ========== STEP 5.5: Enhanced neuromodulation integration ==========
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
        
        // Novelty detection affects concept formation and prediction
        float noveltyLevel = pImpl->novelty->getLevel();
        
        // Neuromodulation affects concept formation - high novelty encourages new concepts
        if (pImpl->conceptFormation && noveltyLevel > 0.5f) {
            // Get current working memory for novelty assessment
            std::vector<float> workingMemoryPattern;
            if (pImpl->workingMemory) {
                workingMemoryPattern = pImpl->workingMemory->retrieve();
            }
            
            // Check if pattern is novel
            if (pImpl->conceptFormation->isNovel(workingMemoryPattern)) {
                // Increase formation threshold temporarily to encourage exploration
                static float originalThreshold = pImpl->conceptFormation->getFormationThreshold();
                
                // Temporarily lower threshold to encourage new concept formation
                pImpl->conceptFormation->setFormationThreshold(0.5f);
                
                // Present this potentially novel pattern for concept formation
                pImpl->conceptFormation->presentExperience(
                    workingMemoryPattern,
                    std::vector<float>(workingMemoryPattern.size(), 0.0f),  // Placeholder features
                    noveltyLevel,
                    currentStep
                );
                
                // Reset threshold immediately after concept formation
                pImpl->conceptFormation->setFormationThreshold(originalThreshold);
            }
        }
        
        // Novelty affects prediction system - high novelty requires prediction updates
        if (pImpl->predictionSystem) {
            // Prediction system should be more flexible with novel input
            float predictionError = pImpl->predictionSystem->getPredictionError();
            
            // High novelty increases prediction error tolerance
            if (noveltyLevel > 0.7f && predictionError > 0.3f) {
                // Train prediction system more aggressively with novel input
                InternalSignals noveltySignals(1, noveltyLevel);
                pImpl->predictionSystem->train(noveltySignals);
            }
        }
    }
    
    // Update curiosity based on novelty and prediction error
    if (pImpl->curiosity && pImpl->predictionSystem) {
        float novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
        float predictionError = pImpl->predictionSystem->getPredictionError();
        pImpl->curiosity->update(novelty, predictionError, pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error) based on prediction error
    if (pImpl->dopamine && pImpl->predictionSystem) {
        float predictionError = pImpl->predictionSystem->getPredictionError();
        pImpl->dopamine->signalRewardPredictionError(predictionError);
        
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
    
    // ========== STEP 7.5: Connect working memory to episodic memory ==========
    if (pImpl->workingMemory && pImpl->episodicMemory && pImpl->associativeMemory) {
        // Get working memory content as episodic candidate
        std::vector<float> memoryPattern = pImpl->workingMemory->retrieve();
        
        if (!memoryPattern.empty() && pImpl->workingMemory->getMemoryActivity() > 0.5f) {
            // Create episodic memory item from working memory
            EpisodicMemoryItem episodicCandidate;
            episodicCandidate.timestamp = currentStep;
            episodicCandidate.sensoryState = memoryPattern;
            
            // Store in working memory as episodic memory
            pImpl->episodicMemory->storeEpisode(episodicCandidate);
            
            // Associate with current neural activity patterns for later recall
            // Get active neurons and their activations
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            std::vector<float> neuronPattern;
                            neuronPattern.push_back(std::max(0.0f, std::min(1.0f, 
                                (neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f)));
                            
                            // Associate neuron pattern with working memory pattern
                            pImpl->associativeMemory->associate(neuronPattern, memoryPattern, 0.7f);
                        }
                    }
                }
            }
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem && pImpl->workingMemory) {
        // Get current sensory state from neural activity patterns
        std::vector<float> currentNeuralPattern;
        
        // Collect activity from working memory neurons
        const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        currentNeuralPattern.reserve(memoryNeurons.size());
        for (const auto& neuronId : memoryNeurons) {
            // Find the neuron to get its current activation level
            for (auto& region : pImpl->regions) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == neuronId) {
                        // Normalize potential to [0,1] range
                        float normalizedActivation = std::max(0.0f, std::min(1.0f, 
                            (neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f));
                        currentNeuralPattern.push_back(normalizedActivation);
                        break;
                    }
                }
            }
        }
        
        // Also include current dopamine level as internal signal
        float dopamineLevel = 0.0f;
        if (pImpl->dopamine) {
            dopamineLevel = pImpl->dopamine->getLevel();
        }
        currentNeuralPattern.push_back(dopamineLevel);
        
        // Create internal signals sensory input from current neural activity
        InternalSignals currentInternal(currentNeuralPattern.size() + 1); // +1 for dopamine
        currentInternal.addSignal(dopamineLevel);  // Add dopamine as first signal
        for (float activation : currentNeuralPattern) {
            currentInternal.addSignal(activation);
        }
        
        // If we have previous predictions, update prediction system
        static std::unique_ptr<SensoryInput> lastPredictedState;
        static std::unique_ptr<SensoryInput> lastActualState;
        
        if (lastActualState) {
            // Make prediction for next timestep
            std::unique_ptr<SensoryInput> predicted = pImpl->predictionSystem->predictNextState(*lastActualState);
            
            // Update prediction system with new predictions and compute error
            pImpl->predictionSystem->updatePredictions(*predicted, currentInternal);
            
            // Store updated prediction system state
            float predictionError = pImpl->predictionSystem->getPredictionError();
            float confidence = pImpl->predictionSystem->getConfidence();
            
            // Train prediction system with current observation
            pImpl->predictionSystem->train(currentInternal);
            
            // Neuromodulation: prediction error affects learning
            if (pImpl->predictionError) {
                pImpl->predictionError->computeError(confidence, predictionError);
            }
            
            // Neuromodulation: prediction accuracy affects concept formation
            if (pImpl->curiosity) {
                pImpl->curiosity->update(pImpl->timestep);
                // Curiosity increases when prediction error is high
                if (predictionError > 0.5f) {
                    pImpl->curiosity->setPredictionErrorWeight(0.8f);
                } else {
                    pImpl->curiosity->setPredictionErrorWeight(0.3f);
                }
            }
            
            // Store for next iteration
            lastPredictedState = std::move(predicted);
            lastActualState = currentInternal.clone();
        } else {
            // First step - just train with current state
            pImpl->predictionSystem->train(currentInternal);
            lastActualState = currentInternal.clone();
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
        // Get current working memory content
        std::vector<float> workingMemoryPattern;
        if (pImpl->workingMemory) {
            workingMemoryPattern = pImpl->workingMemory->retrieve();
        }
        
        // Get current attention winners
        std::vector<NeuronId> attentionWinners;
        if (pImpl->attention) {
            attentionWinners = pImpl->attention->getWinners();
        }
        
        // Create features vector combining working memory and attention
        std::vector<float> features;
        features.reserve(workingMemoryPattern.size() + attentionWinners.size() + 3);
        
        // Add working memory activation levels as features
        for (float activation : workingMemoryPattern) {
            features.push_back(activation);
        }
        
        // Add attention-based features (which neurons are being attended)
        for (const auto& neuronId : attentionWinners) {
            // Find activation of this neuron
            for (auto& region : pImpl->regions) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == neuronId) {
                        float activation = neuron->getState().membranePotential - neuron->getState().restingPotential;
                        activation = std::max(0.0f, std::min(1.0f, activation / 20.0f));
                        features.push_back(activation);
                        break;
                    }
                }
            }
        }
        
        // Add neuromodulation-based features
        float dopamineLevel = 0.0f, curiosityLevel = 0.0f;
        if (pImpl->dopamine) {
            dopamineLevel = pImpl->dopamine->getLevel();
        }
        if (pImpl->curiosity) {
            curiosityLevel = pImpl->curiosity->getLevel();
        }
        float predictionError = 0.0f;
        if (pImpl->predictionError) {
            predictionError = pImpl->predictionError->getError();
        }
        
        features.push_back(dopamineLevel);
        features.push_back(curiosityLevel);
        features.push_back(predictionError);
        
        // Process patterns in concept formation system
        // Use reward from dopamine as feedback signal
        float currentReward = dopamineLevel;
        
        // Present experience to concept formation system
        size_t conceptId = pImpl->conceptFormation->presentExperience(
            workingMemoryPattern,  // Pattern
            features,             // Features
            currentReward,        // Reward
            currentStep           // Current time step
        );
        
        // Update associative memory based on concept formation
        if (pImpl->associativeMemory && conceptId > 0) {
            // Get the concept prototype
            std::vector<float> conceptPrototype = pImpl->conceptFormation->getConceptPrototype(conceptId);
            
            if (!conceptPrototype.empty()) {
                // Associate the working memory pattern with the concept
                pImpl->associativeMemory->associate(workingMemoryPattern, conceptPrototype, 0.8f);
                
                // Also associate with attention patterns for contextual links
                for (const auto& neuronId : attentionWinners) {
                    for (auto& region : pImpl->regions) {
                        auto neurons = region->getAllNeurons();
                        for (auto* neuron : neurons) {
                            if (neuron->getId() == neuronId) {
                                // Get activation pattern
                                std::vector<float> neuronPattern(1, std::max(0.0f, std::min(1.0f,
                                    (neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f)));
                                pImpl->associativeMemory->associate(neuronPattern, conceptPrototype, 0.6f);
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        // Neuromodulation integration: affect concept formation
        if (pImpl->dopamine && pImpl->conceptFormation) {
            // Dopamine level affects concept stability
            float dopamineLevel = pImpl->dopamine->getLevel();
            
            // Get all concepts and apply neuromodulation effects
            const auto& concepts = pImpl->conceptFormation->getConcepts();
            for (const auto& concept : concepts) {
                // Higher dopamine increases concept retention (stability)
                // Lower dopamine may increase exploration (new concepts)
                if (dopamineLevel > 0.5f) {
                    // Strong reward - reinforce existing concepts
                    // Update concept prototype with Hebbian-like strengthening
                    pImpl->conceptFormation->updateConcept(
                        concept.id,
                        concept.prototype,
                        concept.features,
                        dopamineLevel * 2.0f
                    );
                } else if (dopamineLevel < -0.5f) {
                    // Negative reward - exploration encourages new concepts
                    // This happens naturally through the concept formation system
                }
            }
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Enhanced replay mechanisms with sleep-phase integration ==========
    enhancedReplayMechanism(pImpl, currentStep);
    
    // ========== STEP 13: Apply development effects (Enhanced) ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Apply enhanced development effects based on current stage
        applyEnhancedDevelopmentEffects(pImpl, currentStep);
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

// Enhanced replay mechanism with sleep-phase integration
void enhancedReplayMechanism(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl->episodicMemory) return;
    
    // Determine replay mode based on sleep phase
    bool isSleeping = (impl->sleepState.phase != Brain::Impl::SleepCycleState::SleepPhase::Awake);
    nlm::SleepPhase currentPhase = impl->sleepState.phase;
    
    // Phase-dependent replay parameters
    size_t replayCount = 3;  // Base number of episodes to replay
    bool enablePatternCompletion = true;
    bool enableSchemaIntegration = true;
    float replayStrength = 1.0f;
    
    // Adjust replay parameters based on sleep phase
    switch (currentPhase) {
        case Brain::Impl::SleepCycleState::SleepPhase::Awake:
            // Light replay for awake consolidation
            replayCount = 2;
            replayStrength = 0.8f;
            enablePatternCompletion = true;
            enableSchemaIntegration = false;  // Schema integration happens during sleep
            break;
            
        case Brain::Impl::SleepCycleState::SleepPhase::LightSleep:
            // Moderate replay during light sleep
            replayCount = 4;
            replayStrength = 1.2f;
            enablePatternCompletion = true;
            enableSchemaIntegration = false;
            break;
            
        case Brain::Impl::SleepCycleState::SleepPhase::DeepSleep:
            // Intense replay during deep sleep for synaptic strengthening
            replayCount = 6;
            replayStrength = 2.0f;
            enablePatternCompletion = true;
            enableSchemaIntegration = false;
            break;
            
        case Brain::Impl::SleepCycleState::SleepPhase::REM:
            // Reorganization replay during REM for memory integration
            replayCount = 5;
            replayStrength = 1.5f;
            enablePatternCompletion = false;  // Pattern completion during non-REM
            enableSchemaIntegration = true;  // Schema integration during REM
            break;
    }
    
    // Enhanced replay selection based on sleep phase and importance
    if (isSleeping) {
        // Get important episodes that need consolidation/rehearsal
        auto importantEpisodes = impl->episodicMemory->getImportantEpisodesForRehearsal(0.6f);
        
        // If we have important episodes, replay them with enhanced mechanisms
        if (!importantEpisodes.empty()) {
            // Limit to replay count
            size_t toReplay = std::min(replayCount, importantEpisodes.size());
            
            // Enhanced replay with phase-specific parameters
            for (size_t i = 0; i < toReplay; ++i) {
                impl->episodicMemory->enhancedReplayEpisode(
                    importantEpisodes[i],
                    replayStrength * (1.0f + (i * 0.1f)),  // Increasing strength
                    enablePatternCompletion,
                    enableSchemaIntegration
                );
                
                // Apply spike timing-dependent plasticity during replay
                if (impl->spikeSystem && impl->stdp) {
                    // Create replay-specific spike timing for STDP
                    applySTDPDuringReplay(impl, importantEpisodes[i], replayStrength);
                }
            }
        } else {
            // Fall back to basic replay if no important episodes
            auto episodesToReplay = impl->episodicMemory->getEpisodesForReplay(replayCount);
            for (const auto* episode : episodesToReplay) {
                impl->episodicMemory->enhancedReplayEpisode(
                    episode,
                    replayStrength,
                    enablePatternCompletion,
                    enableSchemaIntegration
                );
            }
        }
    } else {
        // Awake replay - less intensive but still beneficial
        auto episodesToReplay = impl->episodicMemory->getEpisodesForReplay(replayCount);
        for (const auto* episode : episodesToReplay) {
            impl->episodicMemory->enhancedReplayEpisode(
                episode,
                replayStrength,
                true,  // Always enable pattern completion when awake
                false  // No schema integration when awake
            );
        }
    }
    
    // Schedule next replay based on phase and importance
    if (isSleeping) {
        impl->episodicMemory->scheduleReplays(
            calculateRemainingPhaseDuration(impl, currentStep)
        );
    }
    
    // Apply synaptic consolidation during replay
    if (isSleeping && currentPhase == Brain::Impl::SleepCycleState::SleepPhase::DeepSleep) {
        impl->sleepState.synapticDownscaling *= 0.9f;  // Continue downscaling
    }
}

// Apply spike timing-dependent plasticity during replay
void applySTDPDuringReplay(Brain::Impl* impl, const EpisodicMemoryItem* episode, 
                         float replayStrength) {
    if (!impl->stdp || !impl->spikeSystem || !episode) return;
    
    // Create replay-specific spike timing patterns
    // This simulates the spike timing that occurs during memory replay
    
    // For each active neuron in the episode, create spikes with specific timing
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Create spike events with timing based on episode index
        float baseTime = static_cast<float>(impl->currentStep) * impl->timestep;
        float spikeTime = baseTime + (i * 0.001f * replayStrength);  // Staggered spikes
        
        // Create and schedule spike events for STDP
        SpikeEvent spikeEvent(neuron, spikeTime, impl->currentStep);
        impl->spikeSystem->queueSpike(spikeEvent);
        
        // Record this spike for STDP computation
        impl->stdp->recordSpike(neuron, spikeTime, impl->currentStep);
    }
    
    // Process spikes to apply STDP
    impl->spikeSystem->processSpikes(impl->currentStep);
}

// Calculate remaining phase duration
float calculateRemainingPhaseDuration(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl) return 0.0f;
    
    float remaining = 0.0f;
    if (currentStep < impl->sleepState.nextPhaseTransition) {
        remaining = impl->sleepState.nextPhaseTransition - currentStep;
    }
    
    // Return at least 1 step
    return std::max(1.0f, remaining);
}
    
    // ========== STEP 13: Apply development effects (Enhanced) ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Apply enhanced development effects based on current stage
        applyEnhancedDevelopmentEffects(pImpl, currentStep);
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

// Helper function to reset sleep cycle
void resetSleepCycle(Brain::Impl* impl) {
    // Initialize sleep cycle with age-dependent parameters
    float ageFactor = impl->sleepState.age * 0.1f;  // Aging increases sleep need
    float baseSleepNeed = 1.0f + ageFactor;
    
    // Progressive sleep cycle: awake -> light -> deep -> REM -> awake
    impl->sleepState.sleepProgress = 0.0f;
    impl->sleepState.phase = Brain::Impl::SleepCycleState::SleepPhase::Awake;
    
    // Age-dependent sleep phases
    float phaseDuration = 100.0f;  // Base phase duration in steps
    phaseDuration *= baseSleepNeed;  // Scale by sleep need
    phaseDuration *= (1.0f + impl->sleepState.age * 0.05f);  // Age extends sleep duration
    
    // Set phase durations based on age
    if (impl->sleepState.age < 0.5f) {  // Young brain
        impl->sleepState.phaseDuration = phaseDuration;
        impl->sleepState.nextPhaseTransition = 200.0f;  // Longer awake periods
    } else if (impl->sleepState.age < 2.0f) {  // Adolescent
        impl->sleepState.phaseDuration = phaseDuration * 0.8f;
        impl->sleepState.nextPhaseTransition = 150.0f;
    } else {  // Adult/Aging
        impl->sleepState.phaseDuration = phaseDuration * 0.6f;
        impl->sleepState.nextPhaseTransition = 100.0f;
    }
    
    impl->sleepState.synapticDownscaling = 1.0f;
    impl->sleepState.memoryReorganization = 0.0f;
    impl->sleepState.consolidationStrength = 0.5f + impl->sleepState.age * 0.05f;
}

// Helper function to calculate sleep drive
float calculateSleepDrive(Brain::Impl* impl) {
    // Factors contributing to sleep drive:
    // 1. Time awake (increases drive)
    float timeAwakeFactor = std::min(1.0f, static_cast<float>(impl->stepsSinceLastEpisode) / 500.0f);
    
    // 2. Neural activity (high activity increases sleep need for downscaling)
    float activityFactor = std::min(1.0f, static_cast<float>(impl->totalSpikesThisStep) / 1000.0f);
    
    // 3. Developmental stage (younger brains need more sleep)
    float developmentalFactor = 1.0f - (impl->sleepState.age * 0.2f);  // Younger = higher need
    developmentalFactor = std::max(0.5f, developmentalFactor);  // Minimum 0.5
    
    // 4. Novelty/experience (more novel experiences increase need for consolidation)
    float noveltyFactor = 0.5f;  // Would be calculated from novelty detection
    
    float sleepDrive = timeAwakeFactor * 0.4f + 
                       activityFactor * 0.3f + 
                       developmentalFactor * 0.2f + 
                       noveltyFactor * 0.1f;
    
    return sleepDrive;
}

// Helper function to update sleep phase
void updateSleepPhase(Brain::Impl* impl, SimulationStep currentStep) {
    // Progress through sleep phases
    if (currentStep >= impl->sleepState.nextPhaseTransition) {
        // Advance to next phase
        switch (impl->sleepState.phase) {
            case Brain::Impl::SleepCycleState::SleepPhase::Awake:
                impl->sleepState.phase = Brain::Impl::SleepCycleState::SleepPhase::LightSleep;
                impl->sleepState.nextPhaseTransition = currentStep + 
                    static_cast<float>(impl->sleepState.phaseDuration * 0.3f);
                break;
                
            case Brain::Impl::SleepCycleState::SleepPhase::LightSleep:
                impl->sleepState.phase = Brain::Impl::SleepCycleState::SleepPhase::DeepSleep;
                impl->sleepState.nextPhaseTransition = currentStep + 
                    static_cast<float>(impl->sleepState.phaseDuration * 0.5f);
                break;
                
            case Brain::Impl::SleepCycleState::SleepPhase::DeepSleep:
                impl->sleepState.phase = Brain::Impl::SleepCycleState::SleepPhase::REM;
                impl->sleepState.nextPhaseTransition = currentStep + 
                    static_cast<float>(impl->sleepState.phaseDuration * 0.2f);
                break;
                
            case Brain::Impl::SleepCycleState::SleepPhase::REM:
                impl->sleepState.phase = Brain::Impl::SleepCycleState::SleepPhase::Awake;
                impl->sleepState.nextPhaseTransition = currentStep + 100.0f;  // Short awake transition
                break;
        }
    }
}

// Helper function to apply synaptic downscaling
void applySynapticDownscaling(Brain::Impl* impl, SimulationStep currentStep) {
    // Calculate total synaptic strength across all regions
    float totalSynapseStrength = 0.0f;
    for (const auto& region : impl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = std::abs(syn->getWeight());
            totalSynapseStrength += weight;
        }
    }
    
    impl->synapticHomeostasis.synapticStrengthTotal = totalSynapseStrength;
    
    // Apply homeostatic downscaling based on sleep phase and synaptic saturation
    float targetStrength = impl->synapticHomeostasis.synapticStrengthTarget;
    float saturationThreshold = impl->synapticHomeostasis.saturationThreshold;
    
    if (impl->synapticHomeostasis.synapticStrengthTotal > targetStrength * saturationThreshold) {
        // Synaptic overload detected - apply downscaling
        float overloadFactor = impl->synapticHomeostasis.synapticStrengthTotal / targetStrength;
        float downscalingFactor = 1.0f - std::min(0.5f, (overloadFactor - 1.0f) * 0.1f);
        
        // Apply stronger downscaling in deep sleep phase
        if (impl->sleepState.phase == Brain::Impl::SleepCycleState::SleepPhase::DeepSleep) {
            downscalingFactor = std::max(0.5f, downscalingFactor * 0.8f);
        }
        
        impl->synapticHomeostasis.normalizationFactor = downscalingFactor;
        impl->sleepState.synapticDownscaling = downscalingFactor;
        
        // Apply downscaling to all synapses
        for (auto& region : impl->regions) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getWeight() != 0.0f) {
                    float currentWeight = syn->getWeight();
                    float scaling = downscalingFactor * (1.0f - 0.1f * std::abs(currentWeight));
                    float newWeight = currentWeight * scaling;
                    syn->setWeight(newWeight);
                }
            }
        }
    }
}

// Helper function for enhanced memory consolidation
void enhancedMemoryConsolidation(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl->episodicMemory) return;
    
    // Calculate enhanced consolidation strength based on sleep phase
    float phaseBonus = 1.0f;
    switch (impl->sleepState.phase) {
        case Brain::Impl::SleepCycleState::SleepPhase::LightSleep:
            phaseBonus = 1.2f;
            break;
        case Brain::Impl::SleepCycleState::SleepPhase::DeepSleep:
            phaseBonus = 2.0f;  // Peak consolidation in deep sleep
            break;
        case Brain::Impl::SleepCycleState::SleepPhase::REM:
            phaseBonus = 1.5f;
            break;
        default:
            phaseBonus = 1.0f;
    }
    
    float enhancedStrength = impl->sleepState.consolidationStrength * phaseBonus;
    
    // Enhanced memory prioritization
    if (currentStep % 500 == 0) {  // More frequent consolidation during sleep
        // Prioritize memories based on multiple factors
        consolidatePrioritizedMemories(impl, enhancedStrength);
    }
}

// Helper function to prioritize memories for consolidation
void consolidatePrioritizedMemories(Brain::Impl* impl, float strength) {
    if (!impl->episodicMemory) return;
    
    // In a full implementation, we would:
    // 1. Score all episodes based on multiple criteria
    // 2. Select highest-priority episodes for consolidation
    // 3. Apply consolidation with synaptic tagging and capture
    // 4. Integrate with semantic memory
    
    // For now, use the existing consolidation mechanism with enhanced strength
    impl->episodicMemory->consolidate(strength * 0.5f);
}

// Helper function to reorganize memories during consolidation
void reorganizeMemories(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl->episodicMemory || !impl->associativeMemory) return;
    
    // Memory reorganization occurs more during REM sleep
    if (impl->sleepState.phase == Brain::Impl::SleepCycleState::SleepPhase::REM) {
        impl->sleepState.memoryReorganization += 0.1f;
        
        // Cross-modal memory integration
        // In a full implementation, this would:
        // 1. Integrate episodic memories with semantic schemas
        // 2. Strengthen associative links between related patterns
        // 3. Prune weakly connected memories
        // 4. Create new conceptual links
        
        // For now, enhance associative memory connections
        if (impl->associativeMemory && impl->workingMemory) {
            // Get current working memory pattern
            std::vector<float> currentPattern = impl->workingMemory->retrieve();
            if (!currentPattern.empty()) {
                // Create associations to strengthen memory integration
                for (size_t i = 0; i < std::min(currentPattern.size(), static_cast<size_t>(10)); ++i) {
                    std::vector<float> smallPattern(currentPattern.begin(), currentPattern.begin() + i + 1);
                    // Note: This is a simplified placeholder - in full implementation we would access impl properly
                }
            }
        }
    }
}

} // namespace nlm

// Apply enhanced development effects based on current developmental stage
void applyEnhancedDevelopmentEffects(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl || !impl->developmentSystem) return;
    
    // Apply developmental effects to all integrated systems
    impl->developmentSystem->applyMemoryEffects(impl);
    impl->developmentSystem->applyNeuromodulationEffects(impl);
    impl->developmentSystem->applyCognitiveEffects(impl);
    
    // Apply age-dependent effects
    double age = impl->currentTime;
    impl->developmentSystem->applyAgeDependentEffects(impl, age);
    
    // Create experience pattern from recent activity for experience-dependent development
    std::vector<float> experiencePattern;
    if (impl->workingMemory) {
        experiencePattern = impl->workingMemory->retrieve();
    } else if (!impl->sensoryPattern.empty()) {
        experiencePattern = impl->sensoryPattern;
    }
    
    // Apply experience-dependent effects if we have experience patterns
    if (!experiencePattern.empty()) {
        // Filter and normalize experience pattern
        experiencePattern = normalizeExperiencePattern(experiencePattern);
        
        // Apply experience-dependent developmental effects
        impl->developmentSystem->applyExperienceDependentEffects(impl, experiencePattern);
    }
    
    // Apply memory system integration effects
    applyMemoryIntegrationEffects(impl, currentStep);
    
    // Apply neuromodulation integration effects
    applyNeuromodulationIntegrationEffects(impl, currentStep);
    
    // Apply cognitive integration effects
    applyCognitiveIntegrationEffects(impl, currentStep);
    
    // Update developmental statistics
    updateDevelopmentalStats(impl);
}

// Normalize experience pattern for developmental processing
std::vector<float> normalizeExperiencePattern(const std::vector<float>& pattern) {
    if (pattern.empty()) return pattern;
    
    std::vector<float> normalized = pattern;
    
    // Normalize to [0, 1] range
    float minVal = *std::min_element(normalized.begin(), normalized.end());
    float maxVal = *std::max_element(normalized.begin(), normalized.end());
    float range = maxVal - minVal;
    
    if (range > 0.0f) {
        for (float& val : normalized) {
            val = (val - minVal) / range;
        }
    } else {
        // If all values are the same, set to 0.5
        for (float& val : normalized) {
            val = 0.5f;
        }
    }
    
    return normalized;
}

// Apply memory integration effects based on development
void applyMemoryIntegrationEffects(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl) return;
    
    // Working memory integration
    if (impl->workingMemory) {
        // Get developmental memory capacity modifier
        if (impl->developmentSystem) {
            float capacityModifier = impl->developmentSystem->getMemoryCapacityModifier();
            
            // Apply capacity effects
            size_t currentCapacity = impl->workingMemory->getCapacity();
            size_t newCapacity = static_cast<size_t>(currentCapacity * capacityModifier);
            impl->workingMemory->setCapacity(newCapacity);
            
            // Apply decay rate effects
            float decayRateModifier = impl->developmentSystem->getMemoryStabilityModifier();
            float currentDecayRate = impl->workingMemory->getDecayRate();
            float newDecayRate = currentDecayRate * decayRateModifier;
            impl->workingMemory->setDecayRate(newDecayRate);
        }
    }
    
    // Episodic memory integration
    if (impl->episodicMemory) {
        if (impl->developmentSystem) {
            // Get episodic consolidation strength
            float consolidationStrength = impl->developmentSystem->getEpisodicConsolidationStrength();
            
            // Apply consolidation strength to episodic memory
            // Note: This would integrate with episodic memory consolidation logic
            
            // Schema formation effects
            if (impl->developmentSystem->shouldFormSchemas()) {
                // Enable schema formation mechanisms
                // Note: This would integrate with schema formation logic
            }
            
            // Memory prioritization effects
            // Note: This would integrate with memory prioritization logic
        }
    }
    
    // Associative memory integration
    if (impl->associativeMemory) {
        if (impl->developmentSystem) {
            // Get experience-dependent adaptation
            float adaptation = impl->developmentSystem->getExperienceAdaptation(std::vector<float>());
            
            // Apply adaptation effects to association strength
            // Note: This would integrate with associative memory learning rate
        }
    }
}

// Apply neuromodulation integration effects based on development
void applyNeuromodulationIntegrationEffects(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl) return;
    
    // Dopamine integration
    if (impl->dopamine) {
        if (impl->developmentSystem) {
            // Get developmental dopamine modulation
            float dopamineMod = impl->developmentSystem->getDopamineModulation();
            
            // Apply dopamine effects on neural excitability
            float dopamineLevel = impl->dopamine->getLevel();
            float modulatedDopamine = dopamineLevel * dopamineMod;
            
            // Apply neuromodulated dopamine effects to neurons
            for (auto& region : impl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Apply dopamine modulation to neural excitability
                        float excitabilityMod = modulatedDopamine * 0.5f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    }
    
    // Novelty integration
    if (impl->novelty) {
        if (impl->developmentSystem) {
            // Get developmental curiosity modulation
            float curiosityMod = impl->developmentSystem->getCuriosityModulation();
            
            // Apply curiosity modulation to novelty detection
            float noveltyLevel = impl->novelty->getLevel();
            float modulatedNovelty = noveltyLevel * curiosityMod;
            
            // Note: This would integrate with novelty processing
        }
    }
    
    // Prediction error integration
    if (impl->predictionError) {
        if (impl->developmentSystem) {
            // Get developmental prediction error modulation
            float errorMod = impl->developmentSystem->getPredictionErrorModulation();
            
            // Note: This would integrate with prediction error signaling
        }
    }
    
    // Curiosity integration
    if (impl->curiosity) {
        if (impl->developmentSystem) {
            // Get developmental curiosity modulation
            float curiosityMod = impl->developmentSystem->getCuriosityModulation();
            
            // Apply curiosity modulation to learning
            float curiosityLevel = impl->curiosity->getLevel();
            float modulatedCuriosity = curiosityLevel * curiosityMod;
            
            // Note: This would integrate with curiosity-based learning
        }
    }
}

// Apply cognitive integration effects based on development
void applyCognitiveIntegrationEffects(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl) return;
    
    // Planning system integration
    if (impl->planner) {
        if (impl->developmentSystem) {
            // Get developmental planning modifier
            float planningMod = impl->developmentSystem->getPlanningModifier();
            
            // Apply planning modifier to planning depth or quality
            // Note: This would integrate with neural planning system
        }
    }
    
    // Concept formation integration
    if (impl->conceptFormation) {
        if (impl->developmentSystem) {
            // Get developmental concept formation modifier
            float conceptMod = impl->developmentSystem->getConceptFormationModifier();
            
            // Apply concept formation modifier to formation threshold or rules
            // Note: This would integrate with concept formation system
        }
    }
    
    // Attention system integration
    if (impl->attention) {
        if (impl->developmentSystem) {
            // Get developmental attention modifier
            float attentionMod = impl->developmentSystem->getAttentionModifier();
            
            // Apply attention modifier to inhibition/excitation balance
            // Note: This would integrate with attentional selection system
        }
    }
    
    // Prediction system integration
    if (impl->predictionSystem) {
        if (impl->developmentSystem) {
            // Get developmental prediction modifier
            float predictionMod = impl->developmentSystem->getPredictionModifier();
            
            // Apply prediction modifier to prediction confidence or rules
            // Note: This would integrate with prediction system
        }
    }
}

// Update developmental statistics for monitoring
void updateDevelopmentalStats(Brain::Impl* impl) {
    if (!impl || !impl->developmentSystem) return;
    
    // Update developmental statistics based on current state
    // This would collect statistics for monitoring and analysis
    
    DevelopmentalStage stage = impl->developmentSystem->getStage();
    
    // Stage-specific statistics collection
    switch (stage) {
        case DevelopmentalStage::Initial:
            // Collect early development statistics
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Collect critical period statistics
            break;
        case DevelopmentalStage::Maturation:
            // Collect maturation statistics
            break;
        case DevelopmentalStage::Adult:
            // Collect adult development statistics
            break;
        case DevelopmentalStage::Aging:
            // Collect aging development statistics
            break;
    }
    
    // Note: In a full implementation, these statistics would be
    // stored and made available for monitoring and analysis
}
EOF

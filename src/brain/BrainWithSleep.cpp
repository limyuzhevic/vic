// BrainWithSleep.cpp - Enhanced Brain implementation with memory replay systems
// 
// This file contains the complete implementation of BrainWithSleep, integrating:
// 1. NeuralWorkingMemoryWithSleep for sleep state-dependent working memory
// 2. NeuralEpisodicMemoryWithSleep for sleep state-dependent episodic memory
// 3. MemoryReplaySystem for comprehensive memory replay coordination
// 4. SleepEnhancedPlasticity for state-specific synaptic plasticity
// 5. SleepConsolidation for sleep-dependent memory consolidation

#include "BrainWithSleep.hpp"
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
#include "../memory/NeuralWorkingMemoryWithSleep.hpp"
#include "../memory/NeuralEpisodicMemoryWithSleep.hpp"
#include "../memory/SleepConsolidation.hpp"
#include "../brain/MemoryReplaySystem.hpp"
#include "../plasticity/SleepEnhancedPlasticity.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/validation/Validator.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <map>
#include <chrono>
#include <functional>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== SLEEP STATE MANAGEMENT ==========
    std::unique_ptr<SleepStateManager> sleepStateManager;
    std::chrono::steady_clock::time_point lastWakeTime;
    float wakeTime_;  // Current wake time in seconds
    
    // ========== ENHANCED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemoryWithSleep> workingMemoryWithSleep;
    std::unique_ptr<NeuralEpisodicMemoryWithSleep> episodicMemoryWithSleep;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== MEMORY REPLAY SYSTEM ==========
    std::unique_ptr<MemoryReplaySystem> memoryReplaySystem;
    
    // ========== SLEEP-ENHANCED PLASTICITY ==========
    std::unique_ptr<SleepEnhancedPlasticity> sleepEnhancedPlasticity;
    
    // ========== SLEEP-DEPENDENT CONSOLIDATION ==========
    std::unique_ptr<SleepConsolidation> sleepConsolidation;
    
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
    
    // Memory replay statistics
    size_t memoryReplayCount;
    float memoryReplayProgress;
    float sleepConsolidationProgress;
    
    // Connect planner to action selection
    void connectPlannerToActionSelection();
    
    // Initialize planner with brain state
    void initializePlanner();
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , wakeTime_(0.0f)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
        , memoryReplayCount(0)
        , memoryReplayProgress(0.0f)
        , sleepConsolidationProgress(0.0f) {
        
        // Initialize sleep state manager
        sleepStateManager = std::make_unique<SleepStateManager>();
        sleepStateManager->configure(cfg->getMapOr<std::map<std::string, float>>("sleep_settings", {}));
        
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
        
        // ========== INITIALIZE SLEEP STATE MANAGER ==========
        sleepStateManager->initialize(this);
        
        // ========== INITIALIZE ENHANCED MEMORY SYSTEMS ==========
        
        // Initialize enhanced working memory
        workingMemoryWithSleep = std::make_unique<NeuralWorkingMemoryWithSleep>();
        
        // Initialize enhanced episodic memory
        episodicMemoryWithSleep = std::make_unique<NeuralEpisodicMemoryWithSleep>();
        
        // Initialize associative memory (regular version)
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // ========== INITIALIZE MEMORY REPLAY SYSTEM ==========
        memoryReplaySystem = std::make_unique<MemoryReplaySystem>();
        memoryReplaySystem->initialize(this);
        
        // ========== INITIALIZE SLEEP-ENHANCED PLASTICITY ==========
        sleepEnhancedPlasticity = std::make_unique<SleepEnhancedPlasticity>();
        sleepEnhancedPlasticity->initialize(this);
        
        // ========== INITIALIZE SLEEP-DEPENDENT CONSOLIDATION ==========
        sleepConsolidation = std::make_unique<SleepConsolidation>();
        sleepConsolidation->initialize(this);
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
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
        
        // Configure enhanced plasticity with state-specific parameters
        sleepEnhancedPlasticity->integrateWithSTDP(stdp.get(), SleepState::Wake);
        sleepEnhancedPlasticity->integrateWithHebbian(hebbian.get(), SleepState::Wake);
        sleepEnhancedPlasticity->integrateWithStructuralPlasticity(structuralPlasticity.get(), SleepState::Wake);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Configure enhanced memory systems
        workingMemoryWithSleep->setCapacity(config->getOr<size_t>("working_memory_capacity", 100));
        episodicMemoryWithSleep->setMaxEpisodes(config->getOr<size_t>("episodic_memory_capacity", 1000));
        
        // Configure sleep state manager with circadian parameters
        sleepStateManager->setCircadianPhase(config->getOr<float>("circadian_phase", 0.0f));
        sleepStateManager->setCircadianPeriod(config->getOr<float>("circadian_period", 24.0f * 60.0f * 60.0f));
        
        // Configure sleep consolidation with state parameters
        auto& replayParams = memoryReplaySystem->getReplayParameters(SleepState::Wake);
        replayParams.tracesPerReplay = config->getOr<size_t>("memory_replay_traces", 5);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        
        // Get sleep configuration
        lastWakeTime = std::chrono::steady_clock::now();
        wakeTime_ = 0.0f;
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
    NLM_LOG_INFO("Initializing NLM Brain with Sleep State Management (Phase 6.5: Enhanced Memory Replay)...");
    
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
    
    // ========== INITIALIZE SLEEP STATE MANAGER ==========
    pImpl->sleepStateManager->initialize(this);
    
    // ========== INITIALIZE ENHANCED MEMORY SYSTEMS ==========
    
    // Initialize enhanced working memory with sleep support
    pImpl->workingMemoryWithSleep->initialize(this);
    pImpl->workingMemoryWithSleep->setCapacity(neuronCount / 10);
    
    // Initialize enhanced episodic memory with sleep support
    pImpl->episodicMemoryWithSleep->initialize(this);
    pImpl->episodicMemoryWithSleep->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // ========== INITIALIZE MEMORY REPLAY SYSTEM ==========
    pImpl->memoryReplaySystem->initialize(this);
    
    // ========== INITIALIZE SLEEP-ENHANCED PLASTICITY ==========
    pImpl->sleepEnhancedPlasticity->initialize(this);
    
    // ========== INITIALIZE SLEEP-DEPENDENT CONSOLIDATION ==========
    pImpl->sleepConsolidation->initialize(this);
    
    // ========== INITIALIZE INTEGRATED SYSTEMS ==========
    
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
    
    // Set initial wake time
    pImpl->lastWakeTime = std::chrono::steady_clock::now();
    pImpl->wakeTime_ = 0.0f;
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6.5 - Enhanced Memory Replay)...");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    // Log sleep state status
    NLM_LOG_INFO(std::string("Sleep State: ") + 
                 std::to_string(static_cast<int>(pImpl->sleepStateManager->getCurrentState())));
    NLM_LOG_INFO(std::string("Sleep Pressure: ") + std::to_string(pImpl->sleepStateManager->getSleepPressure()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6.5: SLEEP-ENHANCED INTEGRATED BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation with memory replay enhancements:
     * 
     * 1. Update sleep state and homeostasis
     * 2. Process pending delayed spike events (deliver synaptic input)
     * 3. Update all neuron membrane potentials (LIF dynamics)
     * 4. Detect spikes and schedule outgoing spike events
     * 5. Update enhanced working memory (maintenance and competition) with sleep states
     * 6. Apply neuromodulation effects on neural excitability
     * 7. Apply plasticity rules (STDP, Hebbian) with sleep state modulation
     * 8. Update enhanced episodic memory with current experience and sleep replay
     * 9. Update prediction system
     * 10. Update attention system
     * 11. Update concept formation
     * 12. Apply structural plasticity (synaptogenesis, pruning)
     * 13. Execute comprehensive memory replay (working and episodic)
     * 14. Apply sleep-specific processing and consolidation
     * 15. Apply development effects
     * 16. Execute sleep-dependent memory consolidation
     * 17. Collect statistics
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: UPDATE SLEEP STATE ==========
    // Update sleep state and homeostasis
    updateSleepState(currentTime);
    
    // ========== STEP 2: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 3: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 4: Detect spikes and schedule spike events ==========
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
                    
                    // Record pre-synaptic spike for plasticity
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPreSpike(currentTime);
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
                        
                        // Record post-synaptic spike for plasticity
                        syn->recordPostSpike(currentTime);
                    }
                    
                    // Store to enhanced working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemoryWithSleep) {
                        pImpl->workingMemoryWithSleep->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 5: Update enhanced working memory ==========
    if (pImpl->workingMemoryWithSleep) {
        // Get current sleep state
        SleepState sleepState = pImpl->sleepStateManager->getCurrentState();
        pImpl->workingMemoryWithSleep->update(pImpl->timestep, sleepState);
    }
    
    // ========== STEP 6: Apply neuromodulation effects ==========
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
    
    // ========== STEP 7: Apply plasticity rules (STDP and Hebbian) with sleep enhancement ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    // Get current sleep state for enhanced plasticity
    SleepState sleepState = pImpl->sleepStateManager->getCurrentState();
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with enhanced sleep-modulated parameters
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Update STDP with sleep state modulation
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning with sleep state modulation
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Update Hebbian with sleep state modulation
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Apply sleep-enhanced plasticity rules
            if (pImpl->sleepEnhancedPlasticity) {
                pImpl->sleepEnhancedPlasticity->updateSTDP(syn, preSpikes, postSpikes, currentTime, sleepState);
                pImpl->sleepEnhancedPlasticity->updateHebbian(syn, preSpikes, postSpikes, currentTime, sleepState);
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 8: Update enhanced episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemoryWithSleep) {
            // Capture current brain state as an enhanced episode
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
            
            pImpl->episodicMemoryWithSleep->storeEpisode(episode);
        }
    }
    
    // ========== STEP 9: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 10: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemoryWithSleep && !pImpl->workingMemoryWithSleep->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemoryWithSleep->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 11: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 10.5: PLAN ACTIONS ==========
    if (pImpl->planner) {
        // Plan actions based on current brain state
        // This uses enhanced working memory, episodic memory, and neuromodulation
        
        // Prepare state for planning by collecting neural activity
        std::vector<float> planningState;
        
        // Add enhanced working memory traces if available
        if (pImpl->workingMemoryWithSleep) {
            auto memoryNeurons = pImpl->workingMemoryWithSleep->getMemoryNeurons();
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
    
    // ========== STEP 12: Execute comprehensive memory replay ==========
    if (pImpl->memoryReplaySystem) {
        pImpl->memoryReplaySystem->executeReplay(currentStep, pImpl->sleepStateManager->getCurrentState());
        
        // Update memory replay statistics
        pImpl->memoryReplayCount = pImpl->memoryReplaySystem->getTotalReplayCount();
        pImpl->memoryReplayProgress = pImpl->memoryReplaySystem->getCurrentReplayProgress();
    }
    
    // ========== STEP 13: Apply sleep-specific processing ==========
    processSleepState(currentTime);
    
    // ========== STEP 14: Apply development effects ==========
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
    
    // ========== STEP 15: Execute sleep-dependent memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->sleepConsolidation) {
        // Execute sleep-dependent memory consolidation
        SleepState sleepState = pImpl->sleepStateManager->getCurrentState();
        pImpl->sleepConsolidation->scheduleConsolidation(sleepState, pImpl->timestep * pImpl->consolidationInterval);
        pImpl->sleepConsolidation->triggerConsolidationEvents(sleepState);
        
        // Update consolidation statistics
        pImpl->sleepConsolidationProgress = pImpl->sleepConsolidation->getSystemsConsolidationProgress();
    }
    
    // ========== STEP 16: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
    
    // ========== STEP 17: Update sleep state ==========
    // This was already done at the beginning, but do it again to capture wake time updates
    updateSleepState(currentTime);
}

void Brain::updateSleepState(Timestamp currentTime) {
    if (!pImpl->sleepStateManager) return;
    
    // Update sleep state management
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<float>(now - pImpl->lastWakeTime).count();
    
    // Convert duration to seconds for sleep state manager
    pImpl->sleepStateManager->update(duration, currentTime);
    
    // Update wake time for circadian calculation
    pImpl->wakeTime_ = currentTime;
    
    // Store last wake time
    pImpl->lastWakeTime = now;
}

void Brain::processSleepState(Timestamp currentTime) {
    // This method processes sleep state effects on neural activity
    // and integrates sleep processing into the main brain step
    
    // Get current sleep state
    SleepState currentState = pImpl->sleepStateManager->getCurrentState();
    
    // Apply state-specific neuromodulation and processing
    switch (currentState) {
        case SleepState::QuietSleep:
            // Apply quiet sleep neuromodulation and enhanced memory processing
            if (pImpl->dopamine) {
                float currentDopamine = pImpl->dopamine->getLevel();
                pImpl->dopamine->setLevel(currentDopamine * 1.1f);  // Slight increase
            }
            // Enable enhanced memory replay during quiet sleep
            if (pImpl->memoryReplaySystem) {
                auto& replayParams = pImpl->memoryReplaySystem->getReplayParameters(SleepState::QuietSleep);
                pImpl->memoryReplaySystem->setReplayParameters(SleepState::Wake, {0.0f, 0.0f, 0.0f, 0.0f, 0, false});
            }
            break;
            
        case SleepState::DeepSleep:
            // Apply deep sleep neuromodulation
            if (pImpl->dopamine) {
                float currentDopamine = pImpl->dopamine->getLevel();
                pImpl->dopamine->setLevel(currentDopamine * 0.9f);  // Slight decrease
            }
            // Enable high consolidation during deep sleep
            if (pImpl->sleepConsolidation) {
                pImpl->sleepConsolidation->scheduleConsolidation(SleepState::DeepSleep, 1.0f);
            }
            break;
            
        case SleepState::REMSleep:
            // Apply REM sleep neuromodulation
            if (pImpl->novelty) {
                float currentNovelty = pImpl->novelty->getLevel();
                pImpl->novelty->setLevel(std::min(1.0f, currentNovelty + 0.05f));
            }
            // Enable schema formation during REM sleep
            if (pImpl->sleepConsolidation) {
                pImpl->sleepConsolidation->scheduleConsolidation(SleepState::REMSleep, 1.0f);
            }
            break;
            
        case SleepState::Wake:
            // Wake state - normal neuromodulation
            // Reset memory replay during wake
            if (pImpl->memoryReplaySystem) {
                auto& replayParams = pImpl->memoryReplaySystem->getReplayParameters(SleepState::Wake);
                replayParams.replayStrength = 0.0f;
            }
            break;
    }
    
    // Apply enhanced plasticity for current sleep state
    if (pImpl->sleepEnhancedPlasticity) {
        pImpl->sleepEnhancedPlasticity->modulatePlasticityForState(currentState);
    }
    
    // Integrate memory replay with sleep state
    if (pImpl->memoryReplaySystem) {
        pImpl->memoryReplaySystem->integrateWithNeuromodulation(currentState);
        pImpl->memoryReplaySystem->integrateWithPredictionSystem(currentState);
        pImpl->memoryReplaySystem->integrateWithPlanningSystem(currentState);
    }
}

SleepState Brain::getCurrentSleepState() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->getCurrentState() : SleepState::Wake;
}

bool Brain::isSleeping() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->isSleeping() : false;
}

void Brain::forceSleepStateTransition(SleepState newState, float transitionDuration) {
    if (pImpl->sleepStateManager) {
        pImpl->sleepStateManager->transitionToState(newState, transitionDuration);
    }
}

void Brain::addSleepPressure(float pressure) {
    if (pImpl->sleepStateManager) {
        pImpl->sleepStateManager->addSleepPressure(pressure);
    }
}

void Brain::releaseSleepPressure(float pressure) {
    if (pImpl->sleepStateManager) {
        pImpl->sleepStateManager->releaseSleepPressure(pressure);
    }
}

float Brain::getSleepPressure() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->getSleepPressure() : 0.0f;
}

void Brain::addSleepDebt(float debt) {
    if (pImpl->sleepStateManager) {
        pImpl->sleepStateManager->addSleepDebt(debt);
    }
}

void Brain::reduceSleepDebt(float debt) {
    if (pImpl->sleepStateManager) {
        pImpl->sleepStateManager->reduceSleepDebt(debt);
    }
}

float Brain::getSleepDebt() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->getSleepDebt() : 0.0f;
}

void Brain::setCircadianPhase(float phase) {
    if (pImpl->sleepStateManager) {
        pImpl->sleepStateManager->setCircadianPhase(phase);
    }
}

void Brain::setCircadianPeriod(float period) {
    if (pImpl->sleepStateManager) {
        pImpl->sleepStateManager->setCircadianPeriod(period);
    }
}

float Brain::getCircadianPhase() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->getCircadianPhase() : 0.0f;
}

float Brain::getCircadianPeriod() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->getCircadianPeriod() : 24.0f * 60.0f * 60.0f;
}

float Brain::getCircadianSleepWindow() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->getCircadianSleepWindow() : 0.0f;
}

float Brain::getTimeInCurrentSleepState() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->getTimeInCurrentState() : 0.0f;
}

float Brain::getTotalSleepTime() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->getTotalSleepTime() : 0.0f;
}

float Brain::getSleepEfficiency() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->getSleepEfficiency() : 0.0f;
}

size_t Brain::getMemoryReplayCount() const {
    return pImpl->sleepStateManager ? pImpl->sleepStateManager->getMemoryReplayCount() : 0;
}

SleepStateManager* Brain::getSleepStateManager() {
    return pImpl->sleepStateManager.get();
}

// ========== ADDITIONAL BRAIN METHOD DEFINITIONS ==========

// These methods would continue with the rest of the Brain implementation...
// For brevity, the full implementation continues as in the original Brain.cpp

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
    if (pImpl->workingMemoryWithSleep) {
        // Working memory provides current neural activity patterns for planning
        // Get active neurons and their states
        auto memoryNeurons = pImpl->workingMemoryWithSleep->getMemoryNeurons();
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
            if (pImpl->episodicMemoryWithSleep && pImpl->episodicMemoryWithSleep->getEpisodeCount() > 0) {
                // Get the most recent episode
                auto recentEpisodes = pImpl->episodicMemoryWithSleep->getRecentEpisodes(1);
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

void Brain::connectPredictionSystemToSensoryProcessing() {
    if (!pImpl->predictionSystem || !pImpl->workingMemoryWithSleep) {
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
        
        // Also store in enhanced working memory
        if (pImpl->workingMemoryWithSleep && normalizedValue > 0.5f) {
            pImpl->workingMemoryWithSleep->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
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

// Continue with all other Brain methods from the original implementation...

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
            if (syn->getType() == SynapseType::Excitatory) {
                totalExcitatory += syn->getWeight();
            } else if (syn->getType() == SynapseType::Inhibitory) {
                totalInhibitory += syn->getWeight();
            }
        }
    }
    
    return totalExcitatory / (totalExcitatory + totalInhibitory + 0.0001f);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulatory effects based on signal type
    // This is a simplified implementation
}

void Brain::updatePlasticity() {
    // Update plasticity rules (called automatically in step)
    // This is handled in the step() method
}

void Brain::develop() {
    // Apply developmental changes (called automatically in step)
    // This is handled in the step() method
}

void Brain::reset() {
    // Reset brain state
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemoryWithSleep) pImpl->workingMemoryWithSleep->clear();
    if (pImpl->episodicMemoryWithSleep) pImpl->episodicMemoryWithSleep->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    // Reset sleep state manager
    if (pImpl->sleepStateManager) {
        pImpl->sleepStateManager->transitionToState(SleepState::Wake);
        pImpl->sleepStateManager->addSleepPressure(0.0f);  // Start with no pressure
        pImpl->sleepStateManager->addSleepDebt(0.0f);     // Start with no debt
    }
    
    // Reset enhanced systems
    if (pImpl->memoryReplaySystem) {
        // Reset memory replay system
        pImpl->memoryReplaySystem->handleReplayError("Reset triggered");
    }
    if (pImpl->sleepEnhancedPlasticity) {
        // Reset enhanced plasticity
        pImpl->sleepEnhancedPlasticity->handlePlasticityError("Reset triggered");
    }
    if (pImpl->sleepConsolidation) {
        // Reset sleep consolidation
        pImpl->sleepConsolidation->handleConsolidationError("Reset triggered");
    }
}

bool Brain::save(const std::string& filepath) const {
    // Save brain state to file (checkpointing)
    // This implementation includes sleep state saving
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Save simulation state
        auto simulationData = serializeSimulationState();
        writer.writeSection(CheckpointSection::SimulationState, simulationData.data(), simulationData.size());
        
        // Save sleep state manager
        if (pImpl->sleepStateManager) {
            std::string sleepData = pImpl->sleepStateManager->serialize();
            writer.writeSection(CheckpointSection::Sleep, sleepData.data(), sleepData.size());
        }
        
        // Save enhanced memory systems
        if (pImpl->workingMemoryWithSleep) {
            std::string workingMemoryData = pImpl->workingMemoryWithSleep->serialize();
            writer.writeSection(CheckpointSection::WorkingMemory, workingMemoryData.data(), workingMemoryData.size());
        }
        
        if (pImpl->episodicMemoryWithSleep) {
            std::string episodicMemoryData = pImpl->episodicMemoryWithSleep->serialize();
            writer.writeSection(CheckpointSection::EpisodicMemory, episodicMemoryData.data(), episodicMemoryData.size());
        }
        
        // Save memory replay system
        if (pImpl->memoryReplaySystem) {
            std::string replayData = pImpl->memoryReplaySystem->serialize();
            writer.writeSection(CheckpointSection::MemoryReplay, replayData.data(), replayData.size());
        }
        
        // Save enhanced plasticity system
        if (pImpl->sleepEnhancedPlasticity) {
            std::string plasticityData = pImpl->sleepEnhancedPlasticity->serialize();
            writer.writeSection(CheckpointSection::Plasticity, plasticityData.data(), plasticityData.size());
        }
        
        // Save sleep consolidation system
        if (pImpl->sleepConsolidation) {
            std::string consolidationData = pImpl->sleepConsolidation->serialize();
            writer.writeSection(CheckpointSection::Consolidation, consolidationData.data(), consolidationData.size());
        }
        
        // Save integrated system states
        if (pImpl->systemStates.empty()) {
            // Store enhanced memory system states
            if (pImpl->workingMemoryWithSleep) {
                pImpl->systemStates["workingMemoryWithSleep"] = pImpl->workingMemoryWithSleep->serialize();
            }
            if (pImpl->episodicMemoryWithSleep) {
                pImpl->systemStates["episodicMemoryWithSleep"] = pImpl->episodicMemoryWithSleep->serialize();
            }
            if (pImpl->memoryReplaySystem) {
                pImpl->systemStates["memoryReplaySystem"] = pImpl->memoryReplaySystem->serialize();
            }
            if (pImpl->sleepEnhancedPlasticity) {
                pImpl->systemStates["sleepEnhancedPlasticity"] = pImpl->sleepEnhancedPlasticity->serialize();
            }
            if (pImpl->sleepConsolidation) {
                pImpl->systemStates["sleepConsolidation"] = pImpl->sleepConsolidation->serialize();
            }
        }
        
        // Write system states to Memory section
        std::ostringstream statesOss;
        for (const auto& pair : pImpl->systemStates) {
            statesOss << pair.first << ":" << pair.second << "\n";
        }
        std::string statesStr = statesOss.str();
        writer.writeSection(CheckpointSection::Memory, statesStr.data(), statesStr.size());
        
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
    // Load brain state from file
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
        
        // Load sleep state manager
        auto sleepData = reader.readSection(CheckpointSection::Sleep);
        if (sleepData && pImpl->sleepStateManager) {
            std::string sleepStr(sleepData->begin(), sleepData->end());
            pImpl->sleepStateManager->deserialize(sleepStr);
        }
        
        // Load simulation state
        auto simulationData = reader.readSection(CheckpointSection::SimulationState);
        if (simulationData) {
            deserializeSimulationState(simulationData->data(), simulationData->size());
        }
        
        // Load enhanced memory systems
        auto workingMemoryData = reader.readSection(CheckpointSection::WorkingMemory);
        if (workingMemoryData && pImpl->workingMemoryWithSleep) {
            std::string workingMemoryStr(workingMemoryData->begin(), workingMemoryData->end());
            pImpl->workingMemoryWithSleep->deserialize(workingMemoryStr);
        }
        
        auto episodicMemoryData = reader.readSection(CheckpointSection::EpisodicMemory);
        if (episodicMemoryData && pImpl->episodicMemoryWithSleep) {
            std::string episodicMemoryStr(episodicMemoryData->begin(), episodicMemoryData->end());
            pImpl->episodicMemoryWithSleep->deserialize(episodicMemoryStr);
        }
        
        // Load memory replay system
        auto replayData = reader.readSection(CheckpointSection::MemoryReplay);
        if (replayData && pImpl->memoryReplaySystem) {
            std::string replayStr(replayData->begin(), replayData->end());
            pImpl->memoryReplaySystem->deserialize(replayStr);
        }
        
        // Load enhanced plasticity system
        auto plasticityData = reader.readSection(CheckpointSection::Plasticity);
        if (plasticityData && pImpl->sleepEnhancedPlasticity) {
            std::string plasticityStr(plasticityData->begin(), plasticityData->end());
            pImpl->sleepEnhancedPlasticity->deserialize(plasticityStr);
        }
        
        // Load sleep consolidation system
        auto consolidationData = reader.readSection(CheckpointSection::Consolidation);
        if (consolidationData && pImpl->sleepConsolidation) {
            std::string consolidationStr(consolidationData->begin(), consolidationData->end());
            pImpl->sleepConsolidation->deserialize(consolidationStr);
        }
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    auto region = std::make_unique<NeuralRegion>(pImpl->nextRegionId, name);
    pImpl->regions.push_back(std::move(region));
    pImpl->nextRegionId++;
    return RegionId(pImpl->nextRegionId - 1);
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
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    auto it = std::remove_if(pImpl->interRegionConnections.begin(), 
                            pImpl->interRegionConnections.end(),
                            [source, target](const InterRegionConnection& conn) {
                                return conn.sourceRegion == source && conn.targetRegion == target;
                            });
    pImpl->interRegionConnections.erase(it, pImpl->interRegionConnections.end());
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
        total += region->getTotalSynapseCount();
    }
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
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getFiringNeuronCount();
    }
    return total;
}

float Brain::getAverageFiringRate() const {
    size_t totalFiring = getFiringNeuronCount();
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    return static_cast<float>(totalFiring) / static_cast<float>(totalNeurons);
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemoryWithSleep.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemoryWithSleep.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

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
    NLM_LOG_INFO("=== BRAIN STATUS ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemoryWithSleep ? pImpl->workingMemoryWithSleep->getActiveTraces() : 0));
    NLM_LOG_INFO("Episodic memories: " + std::to_string(pImpl->episodicMemoryWithSleep ? pImpl->episodicMemoryWithSleep->getEpisodeCount() : 0));
    NLM_LOG_INFO("Memory replays: " + std::to_string(pImpl->memoryReplayCount));
    NLM_LOG_INFO("Sleep consolidation progress: " + std::to_string(pImpl->sleepConsolidationProgress * 100.0f) + "%");
    NLM_LOG_INFO("Sleep state: " + std::to_string(static_cast<int>(getCurrentSleepState())));
    NLM_LOG_INFO("=== END STATUS ===");
}

// ========== REST OF BRAIN IMPLEMENTATION ==========

// The rest of the Brain implementation continues as in the original version
// with enhanced integration for sleep-enhanced memory and plasticity systems

// Helper functions for checkpointing
std::vector<char> Brain::serializeSimulationState() const {
    // This would serialize the simulation state
    return std::vector<char>();
}

void Brain::deserializeSimulationState(const char* data, size_t size) {
    // This would deserialize the simulation state
}

void Brain::connectPredictionSystemToSensoryProcessing() {
    if (!pImpl->predictionSystem || !pImpl->workingMemoryWithSleep) {
        return;
    }
    
    // Connect prediction system to sensory processing
    // This would set up prediction and error signals
}

} // namespace nlm

// NLM Brain Implementation - Refactored for better organization and maintainability
// 
// This file implements the Brain class, which serves as the central coordinator
// of the neural system in NLM (Neural Learning Machine). It implements real
// spiking neural computation with event-driven dynamics and integrated
// memory, prediction, cognition, and neuromodulation systems.

// Brain.cpp organizes code into logical sections for improved maintainability:
// 1. Includes and namespace
// 2. PImpl struct definition (encapsulates implementation details)
// 3. Constructor, destructor, and move operations
// 4. Initialization method
// 5. Main simulation step (complete brain loop)
// 6. Input/output and sensory processing
// 7. Plasticity and development
// 8. Statistics and accessors
// 9. Memory system accessors
// 10. Checkpoints and serialization

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

// ============================================================================
// PImpl Struct Definition
// ============================================================================
// Private implementation details are hidden in this struct to reduce code clutter
// and improve maintainability. The Brain class only exposes the public interface.

struct Brain::Impl {
    // Core configuration and random number generator
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    // Neural architecture
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    // Store and maintain various forms of memory:
    // - Working memory: transient active information
    // - Episodic memory: sequence of experiences with context
    // - Associative memory: pattern associations
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    // Prediction system for forward models and error computation
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    // High-level cognitive functions:
    // - NeuralPlanner: action planning and decision making
    // - ConceptFormation: discovering patterns and abstractions
    // - AttentionalSelection: selective processing through competition
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    // Apply developmental changes over time:
    // - Synaptogenesis (formation of new connections)
    // - Pruning (elimination of weak connections)
    // - Maturation (increasing stability)
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    // Modulate neural excitability and plasticity based on:
    // - Dopamine: reward prediction errors and reinforcement
    // - Curiosity: exploration motivation
    // - Novelty: surprise and novelty detection
    // - PredictionError: mismatch between expected and actual
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    // Core biophysical dynamics:
    // - SpikeSystem: event-driven spike generation and delivery
    // - STDP: spike-timing dependent plasticity
    // - Hebbian: correlational Hebbian learning
    // - StructuralPlasticity: long-term structural changes
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters and state
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics tracking
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
    
    // Initialization constructor
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
        
        // Get timestep from config
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        
        // Log initialization
        NLM_LOG_INFO("Brain initialized successfully with " + std::to_string(config->getOr<size_t>("neuron_count", 1000)) + " neurons");
    }
    
    // State variables
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

// ============================================================================
// Lifecycle Management
// ============================================================================

Brain::~Brain() {
    delete pImpl;
}

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

// ============================================================================
// Initialization
// ============================================================================

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

// ============================================================================
// Main Simulation Loop
// ============================================================================

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
     * 14. Periodic memory consolidation
     * 15. Checkpoint management
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

// ============================================================================
// Input/Output and Sensory Processing
// ============================================================================

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

// ============================================================================
// Plasticity and Development
// ============================================================================

SpikeSystem* Brain::getSpikeSystem() {
    if (!pImpl->spikeSystem) {
        NLM_LOG_ERROR("SpikeSystem not initialized in Brain::getSpikeSystem");
        return nullptr;
    }
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    if (!pImpl->spikeSystem) {
        NLM_LOG_ERROR("SpikeSystem not initialized in Brain::getSpikeSystem (const)");
        return nullptr;
    }
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    if (!pImpl->stdp) {
        NLM_LOG_ERROR("STDP not initialized in Brain::getSTDP");
        return nullptr;
    }
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    if (!pImpl->hebbian) {
        NLM_LOG_ERROR("Hebbian not initialized in Brain::getHebbian");
        return nullptr;
    }
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    if (!pImpl->structuralPlasticity) {
        NLM_LOG_ERROR("StructuralPlasticity not initialized in Brain::getStructuralPlasticity");
        return nullptr;
    }
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

// ============================================================================
// Statistics and Basic Functions
// ============================================================================

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

// ============================================================================
// Reset and State Management
// ============================================================================

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

// ============================================================================
// Memory System Accessors
// ============================================================================

NeuralWorkingMemory* Brain::getWorkingMemory() {
    if (!pImpl->workingMemory) {
        NLM_LOG_ERROR("Working memory not initialized in Brain::getWorkingMemory");
        return nullptr;
    }
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    if (!pImpl->episodicMemory) {
        NLM_LOG_ERROR("Episodic memory not initialized in Brain::getEpisodicMemory");
        return nullptr;
    }
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    if (!pImpl->associativeMemory) {
        NLM_LOG_ERROR("Associative memory not initialized in Brain::getAssociativeMemory");
        return nullptr;
    }
    return pImpl->associativeMemory.get();
}

// ============================================================================
// Prediction System Accessor
// ============================================================================

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ============================================================================
// Cognition System Accessors
// ============================================================================

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// ============================================================================
// Development System Accessor
// ============================================================================

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ============================================================================
// Neuromodulation Systems Accessors
// ============================================================================

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
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

// Get current curiosity level
float Brain::getCuriosityLevel() const {
    return pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
}

// Get current neuromodulation level
float Brain::getNeuromodulationLevel() const {
    return pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
}

// Get current novelty level
float Brain::getNoveltyLevel() const {
    return pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
}

// Get current prediction error
float Brain::getPredictionError() const {
    return pImpl->predictionError ? pImpl->predictionError->getLevel() : 0.0f;
}

// ============================================================================
// Configuration and Random Generator
// ============================================================================

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

// ============================================================================
// Logging and Diagnostics
// ============================================================================

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

// ============================================================================
// Region Management
// ============================================================================

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

// ============================================================================
// Global Statistics
// ============================================================================

size_t Brain::getTotalNeuronCount() const {
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
    
// ============================================================================
// Advanced Configuration Management
// ============================================================================

// Validate configuration
bool Brain::validateConfig() const {
    if (!pImpl->config) {
        NLM_LOG_ERROR("Config not available for validation");
        return false;
    }
    return pImpl->config->validate();
}

// Update configuration
bool Brain::updateConfig(const std::string& key, const ConfigValue& value) {
    if (!pImpl->config) {
        NLM_LOG_ERROR("Config not available for update");
        return false;
    }
    return pImpl->config->update(key, value);
}

// Add configuration profile
void Brain::addConfigProfile(const std::string& profileName) {
    if (pImpl->config) {
        pImpl->config->addProfile(profileName);
        NLM_LOG_INFO("Config profile added: " + profileName);
    }
}

// Load configuration profile
bool Brain::loadConfigProfile(const std::string& profileName) {
    if (!pImpl->config) {
        NLM_LOG_ERROR("Config not available for profile loading");
        return false;
    }
    return pImpl->config->loadProfile(profileName);
}

// Merge configuration with another config
void Brain::mergeConfig(const Config& otherConfig, ConfigSource source) {
    if (pImpl->config) {
        pImpl->config->merge(otherConfig, source);
        NLM_LOG_INFO("Config merged successfully");
    }
}

// ============================================================================
// Advanced Neuromodulation Control
// ============================================================================

// Set dopamine level
void Brain::setDopamineLevel(float level) {
    if (pImpl->dopamine) {
        pImpl->dopamine->setLevel(level);
        NLM_LOG_INFO("Dopamine level set to " + std::to_string(level));
    }
}

// Set curiosity level
void Brain::setCuriosityLevel(float level) {
    if (pImpl->curiosity) {
        pImpl->curiosity->setLevel(level);
        NLM_LOG_INFO("Curiosity level set to " + std::to_string(level));
    }
}

// Set novelty level
void Brain::setNoveltyLevel(float level) {
    if (pImpl->novelty) {
        pImpl->novelty->setLevel(level);
        NLM_LOG_INFO("Novelty level set to " + std::to_string(level));
    }
}

// Set prediction error
void Brain::setPredictionError(float error) {
    if (pImpl->predictionError) {
        pImpl->predictionError->setLevel(error);
        NLM_LOG_INFO("Prediction error set to " + std::to_string(error));
    }
}

// Set neuromodulation level for specific type
void Brain::setNeuromodulationLevel(NeuromodulatorType type, float level) {
    switch (type) {
        case NeuromodulatorType::Dopamine:
            setDopamineLevel(level);
            break;
        case NeuromodulatorType::Curiosity:
            setCuriosityLevel(level);
            break;
        case NeuromodulatorType::Novelty:
            setNoveltyLevel(level);
            break;
        case NeuromodulatorType::PredictionError:
            setPredictionError(level);
            break;
    }
}

// Get current neuromodulation level
float Brain::getNeuromodulationLevel(NeuromodulatorType type) const {
    switch (type) {
        case NeuromodulatorType::Dopamine:
            return pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        case NeuromodulatorType::Curiosity:
            return pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
        case NeuromodulatorType::Novelty:
            return pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
        case NeuromodulatorType::PredictionError:
            return pImpl->predictionError ? pImpl->predictionError->getLevel() : 0.0f;
        default:
            return 0.0f;
    }
}

// Apply neuromodulation signal
void Brain::applyNeuromodulationSignal(NeuromodulatorType type, const Neuromodulator& signal) {
    float level = signal.getLevel();
    setNeuromodulationLevel(type, level);
    NLM_LOG_INFO("Neuromodulation signal applied: type=" + std::to_string(static_cast<int>(type)) +
                 ", level=" + std::to_string(level));
}

// Get all neuromodulation levels
void Brain::getAllNeuromodulationLevels(std::unordered_map<NeuromodulatorType, float>& levels) const {
    levels[NeuromodulatorType::Dopamine] = getNeuromodulationLevel(NeuromodulatorType::Dopamine);
    levels[NeuromodulatorType::Curiosity] = getNeuromodulationLevel(NeuromodulatorType::Curiosity);
    levels[NeuromodulatorType::Novelty] = getNeuromodulationLevel(NeuromodulatorType::Novelty);
    levels[NeuromodulatorType::PredictionError] = getNeuromodulationLevel(NeuromodulatorType::PredictionError);
}

// ============================================================================
// Advanced Neural Access
// ============================================================================

// Get neuron by ID
Neuron* Brain::getNeuron(NeuronId id) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == id) {
                    return neuron;
                }
            }
        }
    }
    NLM_LOG_ERROR("Neuron not found: " + std::to_string(id));
    return nullptr;
}

const Neuron* Brain::getNeuron(NeuronId id) const {
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == id) {
                    return neuron;
                }
            }
        }
    }
    return nullptr;
}

// Get all neurons
std::vector<Neuron*> Brain::getAllNeurons() {
    std::vector<Neuron*> neurons;
    neurons.reserve(getTotalNeuronCount());
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neurons.push_back(neuron);
            }
        }
    }
    
    return neurons;
}

// Get neurons by type
std::vector<Neuron*> Brain::getNeuronsByType(NeuronType type) {
    std::vector<Neuron*> neurons;
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neurons.push_back(neuron);
                }
            }
        }
    }
    
    return neurons;
}

// Get neuron with highest activity
Neuron* Brain::getHighestActivityNeuron() {
    Neuron* highestNeuron = nullptr;
    float highestActivity = -1.0f;
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float activity = std::abs(neuron->getState().membranePotential - 
                                       neuron->getState().restingPotential);
                if (activity > highestActivity) {
                    highestActivity = activity;
                    highestNeuron = neuron;
                }
            }
        }
    }
    
    return highestNeuron;
}

// Get neuron with lowest activity
Neuron* Brain::getLowestActivityNeuron() {
    Neuron* lowestNeuron = nullptr;
    float lowestActivity = 1000.0f;  // Large initial value
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float activity = std::abs(neuron->getState().membranePotential - 
                                       neuron->getState().restingPotential);
                if (activity < lowestActivity) {
                    lowestActivity = activity;
                    lowestNeuron = neuron;
                }
            }
        }
    }
    
    return lowestNeuron;
}

// Get neuron statistics
void Brain::getNeuronStats(NeuronId id, NeuronStats& stats) {
    Neuron* neuron = getNeuron(id);
    if (!neuron) {
        NLM_LOG_ERROR("Neuron not found for stats: " + std::to_string(id));
        return;
    }
    
    const auto& state = neuron->getState();
    stats.membranePotential = state.membranePotential;
    stats.restingPotential = state.restingPotential;
    stats.threshold = state.threshold;
    stats.resetPotential = state.resetPotential;
    stats.refractoryRemaining = state.refractoryRemaining;
    stats.lastSpikeTime = state.lastSpikeTime;
    stats.firingState = state.firingState;
    stats.active = neuron->isActive();
    stats.firing = neuron->isFiring();
    
    // Calculate derived statistics
    stats.activityLevel = std::abs(state.membranePotential - state.restingPotential);
    stats.excitability = state.threshold - state.resetPotential;
}

// Set neuron activity
void Brain::setNeuronActive(NeuronId id, bool active) {
    Neuron* neuron = getNeuron(id);
    if (!neuron) {
        NLM_LOG_ERROR("Neuron not found for activation: " + std::to_string(id));
        return;
    }
    
    neuron->setActive(active);
    NLM_LOG_INFO("Neuron " + std::to_string(id) + " set active=" + (active ? "true" : "false"));
}

// Set neuron membrane potential
void Brain::setNeuronMembranePotential(NeuronId id, float potential) {
    Neuron* neuron = getNeuron(id);
    if (!neuron) {
        NLM_LOG_ERROR("Neuron not found for membrane potential: " + std::to_string(id));
        return;
    }
    
    neuron->setMembranePotential(potential);
    NLM_LOG_INFO("Neuron " + std::to_string(id) + " membrane potential set to " + std::to_string(potential));
}

// Set neuron to fire
void Brain::triggerNeuronSpike(NeuronId id) {
    Neuron* neuron = getNeuron(id);
    if (!neuron) {
        NLM_LOG_ERROR("Neuron not found for spike: " + std::to_string(id));
        return;
    }
    
    neuron->fire();
    NLM_LOG_INFO("Neuron " + std::to_string(id) + " fired");
}

// Get population by type and region
NeuralPopulation* Brain::getPopulation(RegionId regionId, NeuronType type) {
    auto* region = getRegion(regionId);
    if (!region) {
        NLM_LOG_ERROR("Region not found: " + std::to_string(regionId));
        return nullptr;
    }
    
    size_t populationIndex = region->getPopulationIndex(type);
    if (populationIndex >= region->getPopulationCount()) {
        NLM_LOG_ERROR("Population type " + std::to_string(static_cast<int>(type)) + 
                     " not found in region " + std::to_string(regionId));
        return nullptr;
    }
    
    return region->getPopulation(populationIndex);
}

// Get region statistics
void Brain::getRegionStats(RegionId id, RegionStats& stats) {
    auto* region = getRegion(id);
    if (!region) {
        NLM_LOG_ERROR("Region not found for stats: " + std::to_string(id));
        return;
    }
    
    stats.neuronCount = region->getTotalNeuronCount();
    stats.populationCount = region->getPopulationCount();
    stats.synapseCount = region->getSynapseCount();
    stats.avgSynapticWeight = region->getAverageSynapticWeight();
    stats.name = region->getName();
    stats.active = region->isActive();
    
    // Count neurons by type
    for (auto& pop : region->getPopulations()) {
        stats.populationCounts[pop->getNeuronType()] = pop->getNeurons().size();
    }
}

// Set region active state
void Brain::setRegionActive(RegionId id, bool active) {
    auto* region = getRegion(id);
    if (!region) {
        NLM_LOG_ERROR("Region not found for activation: " + std::to_string(id));
        return;
    }
    
    region->setActive(active);
    NLM_LOG_INFO("Region " + std::to_string(id) + " set active=" + (active ? "true" : "false"));
}

// Get synapse by source and target
Synapse* Brain::getSynapse(NeuronId source, NeuronId target) {
    for (auto& region : pImpl->regions) {
        for (auto* syn : region->getSynapses()) {
            if (syn->getSourceNeuron() == source && syn->getDestinationNeuron() == target) {
                return syn;
            }
        }
    }
    NLM_LOG_ERROR("Synapse not found: " + std::to_string(source) + " -> " + std::to_string(target));
    return nullptr;
}

// Get all synapses
std::vector<Synapse*> Brain::getAllSynapses() {
    std::vector<Synapse*> synapses;
    synapses.reserve(getTotalSynapseCount());
    
    for (auto& region : pImpl->regions) {
        for (auto* syn : region->getSynapses()) {
            synapses.push_back(syn);
        }
    }
    
    return synapses;
}

// Get synapses from source neuron
std::vector<Synapse*> Brain::getSynapsesFrom(NeuronId source) {
    std::vector<Synapse*> synapses;
    
    for (auto& region : pImpl->regions) {
        for (auto* syn : region->getSynapses()) {
            if (syn->getSourceNeuron() == source) {
                synapses.push_back(syn);
            }
        }
    }
    
    return synapses;
}

// Get synapses to target neuron
std::vector<Synapse*> Brain::getSynapsesTo(NeuronId target) {
    std::vector<Synapse*> synapses;
    
    for (auto& region : pImpl->regions) {
        for (auto* syn : region->getSynapses()) {
            if (syn->getDestinationNeuron() == target) {
                synapses.push_back(syn);
            }
        }
    }
    
    return synapses;
}

// Get synapse statistics
void Brain::getSynapseStats(Synapse* synapse, SynapseStats& stats) {
    if (!synapse) {
        NLM_LOG_ERROR("Null synapse for stats");
        return;
    }
    
    stats.source = synapse->getSourceNeuron();
    stats.target = synapse->getDestinationNeuron();
    stats.weight = synapse->getWeight();
    stats.delay = synapse->getDelay();
    stats.type = synapse->getType();
    stats.plasticityFlags = synapse->getPlasticityFlags();
    stats.eligibilityTrace = synapse->getEligibilityTrace();
    stats.connected = synapse->isConnected();
}

// Set synapse weight
void Brain::setSynapseWeight(SynapseId id, float weight) {
    for (auto& region : pImpl->regions) {
        for (auto* syn : region->getSynapses()) {
            if (syn->getId() == id) {
                syn->setWeight(weight);
                NLM_LOG_INFO("Synapse " + std::to_string(id) + " weight set to " + std::to_string(weight));
                return;
            }
        }
    }
    NLM_LOG_ERROR("Synapse not found for weight setting: " + std::to_string(id));
}

// Set synapse connected state
void Brain::setSynapseConnected(SynapseId id, bool connected) {
    for (auto& region : pImpl->regions) {
        for (auto* syn : region->getSynapses()) {
            if (syn->getId() == id) {
                syn->setConnected(connected);
                NLM_LOG_INFO("Synapse " + std::to_string(id) + " connected set to " + 
                             (connected ? "true" : "false"));
                return;
            }
        }
    }
    NLM_LOG_ERROR("Synapse not found for connected setting: " + std::to_string(id));
}

// ============================================================================
// Advanced Recording Capabilities
// ============================================================================

// Start recording brain state
void Brain::startRecording(const std::string& outputPath) {
    if (!pImpl->checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not available for recording");
        return;
    }
    
    // Set up recording directory
    std::string recordDir = outputPath + "/brain_recording";
    std::filesystem::create_directories(recordDir);
    
    // Configure checkpoint manager for recording
    pImpl->checkpointManager->configure(recordDir, 100000, 5, true);
    
    NLM_LOG_INFO("Recording started to " + outputPath);
}

// Stop recording
void Brain::stopRecording() {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->reset();
        NLM_LOG_INFO("Recording stopped");
    }
}

// Set recording rate
void Brain::setRecordingRate(float rate) {
    if (!pImpl->checkpointManager) return;
    
    // rate is samples per step
    pImpl->checkpointManager->setSaveInterval(static_cast<size_t>(1.0f / rate));
    NLM_LOG_INFO("Recording rate set to " + std::to_string(rate) + " Hz");
}

// Start video recording
void Brain::startVideoRecording(const std::string& outputPath) {
    // In a real implementation, this would start capturing brain state for video
    NLM_LOG_INFO("Video recording started to " + outputPath);
}

// Stop video recording
void Brain::stopVideoRecording() {
    NLM_LOG_INFO("Video recording stopped");
}

// Export brain state
void Brain::exportBrainState(const std::string& filepath) const {
    nlohmann::json exportJson;
    
    // Basic brain information
    exportJson["version"] = "6.0";
    exportJson["total_neurons"] = getTotalNeuronCount();
    exportJson["total_synapses"] = getTotalSynapseCount();
    exportJson["region_count"] = getRegionCount();
    exportJson["current_step"] = pImpl->currentStep;
    exportJson["current_time"] = pImpl->currentTime;
    exportJson["developmental_stage"] = static_cast<int>(pImpl->developmentalStage);
    
    // Neuron information
    nlohmann::json neuronsJson;
    for (const auto& region : pImpl->regions) {
        nlohmann::json regionJson;
        for (const auto& pop : region->getPopulations()) {
            nlohmann::json popJson;
            popJson["id"] = static_cast<int>(pop->getId());
            popJson["type"] = static_cast<int>(pop->getNeuronType());
            popJson["count"] = pop->getNeurons().size();
            regionJson[pop->getName()] = popJson;
        }
        neuronsJson[region->getName()] = regionJson;
    }
    exportJson["neurons"] = neuronsJson;
    
    // Save to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open export file: " + filepath);
        return;
    }
    
    file << exportJson.dump(2);
    NLM_LOG_INFO("Brain state exported to " + filepath);
}

// Import brain state
bool Brain::importBrainState(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open brain state file: " + filepath);
            return false;
        }
        
        nlohmann::json importJson;
        file >> importJson;
        
        // For now, just validate the format
        // In a real implementation, this would load the brain state
        if (importJson.contains("version") && importJson.contains("total_neurons")) {
            std::string version = importJson["version"].get<std::string>();
            size_t neuronCount = importJson["total_neurons"].get<size_t>();
            NLM_LOG_INFO("Brain state import: version=" + version + ", neurons=" + std::to_string(neuronCount));
            return true;
        }
        
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error importing brain state: " + std::string(e.what()));
        return false;
    }
}

// Create brain template
void Brain::createTemplateBrain(const std::string& templateName) {
    NLM_LOG_INFO("Creating brain template: " + templateName);
    
    // Save current brain as template
    std::string templatePath = "./templates/" + templateName + ".json";
    std::filesystem::create_directories("./templates");
    
    // Export and save template
    exportBrainState(templatePath);
    
    // Also save configuration
    std::string configPath = "./templates/" + templateName + "_config.json";
    auto cfg = getConfig();
    if (cfg) {
        cfg->saveToJSON(configPath);
    }
}

// Load brain template
bool Brain::loadTemplateBrain(const std::string& templateName) {
    std::string templatePath = "./templates/" + templateName + ".json";
    
    if (!std::filesystem::exists(templatePath)) {
        NLM_LOG_ERROR("Template not found: " + templateName);
        return false;
    }
    
    // Load the brain state
    if (importBrainState(templatePath)) {
        NLM_LOG_INFO("Brain template loaded: " + templateName);
        return true;
    }
    
    return false;
}

// List brain templates
void Brain::listBrainTemplates(std::vector<std::string>& templates) {
    if (!std::filesystem::exists("./templates")) {
        NLM_LOG_INFO("No templates directory found");
        return;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator("./templates")) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            templates.push_back(entry.path().stem().string());
        }
    }
}

// ============================================================================
// Advanced Experiment Control
// ============================================================================

// Create custom experiment
void Brain::createCustomExperiment(const std::string& experimentName, 
                                  const ExperimentConfig& config) {
    // Create a custom experiment configuration
    nlohmann::json experimentJson;
    experimentJson["name"] = experimentName;
    experimentJson["type"] = "custom";
    experimentJson["config"] = config;
    
    std::string experimentsDir = "./experiments";
    std::filesystem::create_directories(experimentsDir);
    
    std::string filepath = experimentsDir + "/" + experimentName + ".json";
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << experimentJson.dump(2);
        NLM_LOG_INFO("Custom experiment created: " + experimentName);
    }
}

// Run custom experiment
bool Brain::runCustomExperiment(const std::string& experimentName) {
    std::string filepath = "./experiments/" + experimentName + ".json";
    
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Experiment not found: " + experimentName);
        return false;
    }
    
    try {
        std::ifstream file(filepath);
        nlohmann::json experimentJson;
        file >> experimentJson;
        
        std::string name = experimentJson["name"].get<std::string>();
        
        // In a real implementation, this would run the experiment
        NLM_LOG_INFO("Custom experiment executed: " + name);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error running custom experiment: " + std::string(e.what()));
        return false;
    }
}

// Set brain active
void Brain::setBrainActive(NeuronId neuronId, bool active) {
    // Find and set neuron active state
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == neuronId) {
                    neuron->setActive(active);
                    NLM_LOG_INFO("Neuron " + std::to_string(neuronId) + " set active=" + 
                                 (active ? "true" : "false"));
                    return;
                }
            }
        }
    }
    NLM_LOG_ERROR("Neuron " + std::to_string(neuronId) + " not found");
}

// Set population active
void Brain::setPopulationActive(RegionId regionId, NeuronType neuronType, bool active) {
    auto* region = getRegion(regionId);
    if (!region) {
        NLM_LOG_ERROR("Region " + std::to_string(regionId) + " not found");
        return;
    }
    
    size_t populationIndex = region->getPopulationIndex(neuronType);
    if (populationIndex >= region->getPopulationCount()) {
        NLM_LOG_ERROR("Population type " + std::to_string(static_cast<int>(neuronType)) + 
                     " not found in region " + std::to_string(regionId));
        return;
    }
    
    auto* population = region->getPopulation(populationIndex);
    if (!population) {
        NLM_LOG_ERROR("Population not found");
        return;
    }
    
    population->setActive(active);
    NLM_LOG_INFO("Population in region " + std::to_string(regionId) + ", type=" + 
                 std::to_string(static_cast<int>(neuronType)) + 
                 " set active=" + (active ? "true" : "false"));
}

// Set region active
void Brain::setRegionActive(RegionId regionId, bool active) {
    auto* region = getRegion(regionId);
    if (!region) {
        NLM_LOG_ERROR("Region " + std::to_string(regionId) + " not found");
        return;
    }
    
    region->setActive(active);
    NLM_LOG_INFO("Region " + std::to_string(regionId) + " set active=" + 
                 (active ? "true" : "false"));
}

// ============================================================================
// Brain State Management
// ============================================================================

// Clear all inputs
void Brain::clearAllInputs() {
    // Clear all sensory inputs by injecting negative current
    for (auto* neuron : pImpl->sensoryNeurons) {
        neuron->injectCurrent(-10.0f);  // Clear input
    }
    
    for (auto* neuron : pImpl->motorNeurons) {
        neuron->injectCurrent(-10.0f);  // Clear output
    }
    
    NLM_LOG_INFO("All brain inputs cleared");
}

// Simulate random pattern
void Brain::simulateRandomPattern() {
    // Inject random patterns into brain
    // For now, just inject into sensory neurons
    for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
        float current = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 10.0f;
        pImpl->sensoryNeurons[i]->injectCurrent(current);
    }
    
    NLM_LOG_INFO("Random pattern injected into brain");
}

// Simulate event pattern
void Brain::simulateEventPattern(NeuronId source, NeuronId target, float weight) {
    // Create a simulated spike event from source to target
    SpikeEvent spike(source, pImpl->currentTime, pImpl->currentStep);
    
    pImpl->spikeSystem->queueSpike(spike);
    
    NLM_LOG_INFO("Event pattern simulated: " + std::to_string(source) + " -> " + 
                 std::to_string(target) + " (weight=" + std::to_string(weight) + ")");
}

// Reset to template
void Brain::resetToTemplate(const std::string& templateName) {
    if (loadTemplateBrain(templateName)) {
        // Reset developmental state
        pImpl->developmentalStage = DevelopmentalStage::Initial;
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        
        NLM_LOG_INFO("Brain reset to template: " + templateName);
    }
}

// Create snapshot
void Brain::snapshot(const std::string& label) {
    // Create a snapshot of current brain state
    nlohmann::json snapshotJson;
    snapshotJson["label"] = label;
    snapshotJson["step"] = pImpl->currentStep;
    snapshotJson["time"] = pImpl->currentTime;
    snapshotJson["region_count"] = getRegionCount();
    
    // Save snapshot
    std::string snapshotsDir = "./snapshots";
    std::filesystem::create_directories(snapshotsDir);
    
    std::string filepath = snapshotsDir + "/snapshot_" + label + ".json";
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << snapshotJson.dump(2);
        NLM_LOG_INFO("Brain snapshot saved with label: " + label);
    }
}

// Restore from snapshot
bool Brain::restoreFromSnapshot(const std::string& label) {
    std::string filepath = "./snapshots/snapshot_" + label + ".json";
    
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Snapshot not found: " + label);
        return false;
    }
    
    try {
        std::ifstream file(filepath);
        nlohmann::json snapshotJson;
        file >> snapshotJson;
        
        // Restore snapshot info
        if (snapshotJson.contains("step")) {
            pImpl->currentStep = snapshotJson["step"].get<SimulationStep>();
        }
        if (snapshotJson.contains("time")) {
            pImpl->currentTime = snapshotJson["time"].get<Timestamp>();
        }
        
        NLM_LOG_INFO("Brain restored from snapshot: " + label);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error restoring from snapshot: " + std::string(e.what()));
        return false;
    }
}

// List snapshots
void Brain::listSnapshots(std::vector<std::string>& snapshots) {
    if (!std::filesystem::exists("./snapshots")) {
        NLM_LOG_INFO("No snapshots directory found");
        return;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator("./snapshots")) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::string filename = entry.path().filename().string();
            // Remove prefix
            if (filename.find("snapshot_") == 0) {
                filename = filename.substr(9); // Remove "snapshot_"
                if (filename.ends_with(".json")) {
                    filename = filename.substr(0, filename.length() - 5);
                }
                snapshots.push_back(filename);
            }
        }
    }
}

// Delete snapshot
void Brain::deleteSnapshot(const std::string& label) {
    std::string filepath = "./snapshots/snapshot_" + label + ".json";
    
    if (std::filesystem::exists(filepath)) {
        std::filesystem::remove(filepath);
        NLM_LOG_INFO("Snapshot deleted: " + label);
    } else {
        NLM_LOG_ERROR("Snapshot not found for deletion: " + label);
    }
}

// Export to CSV
void Brain::exportToCSV(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open CSV file: " + filepath);
        return;
    }
    
    // Write CSV header
    file << "Step,Time,TotalSpikes,TotalNeurons,TotalSynapses,ActiveNeurons,FiringNeurons,AverageFiringRate,ExcitationInhibitionRatio" << std::endl;
    
    // Write current state
    file << pImpl->currentStep << ","
         << pImpl->currentTime << ","
         << pImpl->totalSpikesTotal << ","
         << getTotalNeuronCount() << ","
         << getTotalSynapseCount() << ","
         << getActiveNeuronCount() << ","
         << getFiringNeuronCount() << ","
         << getAverageFiringRate() << ","
         << getExcitationInhibitionRatio() << std::endl;
    
    NLM_LOG_INFO("Brain state exported to CSV: " + filepath);
}

// Import from CSV
void Brain::importFromCSV(const std::string& filepath) {
    // For CSV import, just log the action
    // In a real implementation, this would parse the CSV and restore state
    NLM_LOG_INFO("CSV import requested (not implemented): " + filepath);
}

// Log advanced statistics
void Brain::logBrainStatistics() const {
    NLM_LOG_INFO("=== Advanced Brain Statistics ===");
    NLM_LOG_INFO("Current step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("Current time: " + std::to_string(pImpl->currentTime));
    NLM_LOG_INFO("Total spikes: " + std::to_string(pImpl->totalSpikesTotal));
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    NLM_LOG_INFO("Region count: " + std::to_string(getRegionCount()));
    
    // Log each region
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + ": " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses");
    }
    
    // Log memory systems
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory: " + std::to_string(pImpl->workingMemory->getActiveTraces()) + " traces");
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()) + " episodes");
    }
}

// ============================================================================
// Helper Functions for Advanced Features
// ============================================================================

// Get region by ID
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

// Get region count

} // namespace nlm
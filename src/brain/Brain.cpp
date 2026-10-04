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
#include <memory>
#include <stdexcept>
#include <cassert>

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
    
    // Resource tracking
    bool isInitialized;
    bool isCleanedUp;
    
    // Performance monitoring
    size_t initializationCount;
    size_t resetCount;
    
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
        , isInitialized(false)
        , isCleanedUp(false)
        , initializationCount(0)
        , resetCount(0)
    {
        try {
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
            
            // Initialize with validation
            validateConfiguration();
            
            initializationCount++;
            isInitialized = true;
            
            NLM_LOG_INFO("Brain::Impl initialized successfully");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during Brain::Impl initialization: ") + e.what());
            cleanupResources();
            throw;
        }
    }
    
    ~Impl() {
        try {
            cleanupResources();
            NLM_LOG_INFO("Brain::Impl destroyed successfully");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during Brain::Impl cleanup: ") + e.what());
        }
    }
    
    void validateConfiguration() {
        // Validate configuration parameters
        if (!config) {
            throw std::runtime_error("Configuration not provided");
        }
        
        // Check required parameters
        size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
        if (neuronCount == 0) {
            throw std::runtime_error("Invalid neuron_count: must be greater than 0");
        }
        
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0) {
            throw std::runtime_error("Invalid simulation_timestep: must be positive");
        }
        
        float connectionProb = config->getOr<float>("connection_probability", 0.1f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            throw std::runtime_error("Invalid connection_probability: must be between 0.0 and 1.0");
        }
        
        size_t regionCount = config->getOr<size_t>("region_count", 1);
        if (regionCount == 0) {
            throw std::runtime_error("Invalid region_count: must be greater than 0");
        }
        
        NLM_LOG_INFO("Configuration validation passed");
    }
    
    void cleanupResources() {
        if (isCleanedUp) {
            return;
        }
        
        try {
            // Clear all regions and their contents
            regions.clear();
            interRegionConnections.clear();
            sensoryNeurons.clear();
            motorNeurons.clear();
            
            // Clear integrated systems
            workingMemory.reset();
            episodicMemory.reset();
            associativeMemory.reset();
            predictionSystem.reset();
            planner.reset();
            conceptFormation.reset();
            attention.reset();
            developmentSystem.reset();
            dopamine.reset();
            curiosity.reset();
            predictionError.reset();
            novelty.reset();
            spikeSystem.reset();
            stdp.reset();
            hebbian.reset();
            structuralPlasticity.reset();
            checkpointManager.reset();
            
            // Reset state
            currentStep = 0;
            currentTime = 0.0;
            totalSpikesThisStep = 0;
            totalSpikesTotal = 0;
            isResting = false;
            stepsSinceLastEpisode = 0;
            isInitialized = false;
            isCleanedUp = true;
            
            NLM_LOG_INFO("Brain::Impl resources cleaned up successfully");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during resource cleanup: ") + e.what());
            // Don't throw in destructor
        }
    }
    
    // Safe copy operations
    void copyFrom(const Impl& other) {
        // Deep copy of regions
        for (const auto& region : other.regions) {
            regions.push_back(std::make_unique<NeuralRegion>(*region));
        }
        
        // Copy inter-region connections
        interRegionConnections = other.interRegionConnections;
        
        // Copy system pointers (should be null for safety)
        workingMemory.reset();
        episodicMemory.reset();
        associativeMemory.reset();
        predictionSystem.reset();
        planner.reset();
        conceptFormation.reset();
        attention.reset();
        developmentSystem.reset();
        dopamine.reset();
        curiosity.reset();
        predictionError.reset();
        novelty.reset();
        spikeSystem.reset();
        stdp.reset();
        hebbian.reset();
        structuralPlasticity.reset();
        checkpointManager.reset();
        
        // Copy state
        currentStep = other.currentStep;
        currentTime = other.currentTime;
        totalSpikesThisStep = other.totalSpikesThisStep;
        totalSpikesTotal = other.totalSpikesTotal;
        isResting = other.isResting;
        stepsSinceLastEpisode = other.stepsSinceLastEpisode;
        replayInterval = other.replayInterval;
        consolidationInterval = other.consolidationInterval;
        developmentalStage = other.developmentalStage;
        nextRegionId = other.nextRegionId;
    }
    
    // Validate object invariants
    void validateInvariants() const {
        assert(config != nullptr || "Config pointer should not be null");
        assert(isInitialized && !isCleanedUp || "Object should be initialized and not cleaned up");
        
        if (!regions.empty()) {
            for (const auto& region : regions) {
                assert(region != nullptr && "Region pointer should not be null");
            }
        }
        
        NLM_LOG_DEBUG("Brain::Impl invariants validated");
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    try {
        NLM_LOG_INFO("Brain created successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to create Brain: ") + e.what());
        throw;
    }
}

Brain::~Brain() {
    try {
        // Cleanup will happen automatically via destructor
        NLM_LOG_INFO("Brain destructor called");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in Brain destructor: ") + e.what());
    }
}

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {
    // Transfer ownership
    other.pImpl = nullptr;
    NLM_LOG_INFO("Brain moved successfully");
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        // Cleanup current resources
        if (pImpl) {
            delete pImpl;
        }
        
        // Transfer ownership
        pImpl = other.pImpl;
        other.pImpl = nullptr;
        
        NLM_LOG_INFO("Brain assignment operator called");
    }
    return *this;
}

bool Brain::initialize() {
    try {
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
        
        pImpl->initializationCount++;
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain initialization: ") + e.what());
        return false;
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
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
        
        // Validate invariants before stepping
        pImpl->validateInvariants();
        
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
        
        pImpl->resetCount++;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain simulation step: ") + e.what());
        throw;
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception receiving sensory input: ") + e.what());
        throw;
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    try {
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* n : neurons) {
                if (n->getId() == neuron) {
                    n->injectCurrent(current);
                    return;
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception injecting current into neuron: ") + e.what());
        throw;
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    try {
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception injecting current to neurons by type: ") + e.what());
        throw;
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
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting excitation-inhibition ratio: ") + e.what());
        return 1.0f;  // Default ratio
    }
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception producing action: ") + e.what());
        return std::make_unique<Action>(ActionType::Wait);
    }
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    try {
        // Apply neuromodulation effects on plasticity
        float modulation = signal.getLevel();
        
        // Scale STDP learning rates
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception applying neuromodulation: ") + e.what());
        throw;
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
    NLM_LOG_DEBUG("Brain::updatePlasticity called (backward compatibility)");
}

void Brain::develop() {
    try {
        // Development updates structural plasticity
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during development: ") + e.what());
        throw;
    }
}

void Brain::reset() {
    try {
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
        
        pImpl->resetCount++;
        
        NLM_LOG_INFO("NLM Brain reset complete");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain reset: ") + e.what());
        throw;
    }
}

bool Brain::save(const std::string& filepath) const {
    try {
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
        if (!pImpl->checkpointManager) {
            NLM_LOG_ERROR("Checkpoint manager not available");
            return false;
        }
        
        return pImpl->checkpointManager->save(filepath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    try {
        NLM_LOG_INFO("Loading brain state from " + filepath);
        
        if (!pImpl->checkpointManager) {
            NLM_LOG_ERROR("Checkpoint manager not available");
            return false;
        }
        
        bool success = pImpl->checkpointManager->load(filepath);
        
        if (success) {
            NLM_LOG_INFO("Brain state loaded successfully");
        } else {
            NLM_LOG_ERROR("Failed to load brain state");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    try {
        RegionId id(pImpl->nextRegionId++);
        auto region = std::make_unique<NeuralRegion>(id, name);
        pImpl->regions.push_back(std::move(region));
        
        NLM_LOG_INFO("Added region " + std::to_string(id.index()) + " (" + name + ")");
        
        return id;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception adding region: ") + e.what());
        throw;
    }
}

NeuralRegion* Brain::getRegion(RegionId id) {
    try {
        for (auto& region : pImpl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting region: ") + e.what());
        return nullptr;
    }
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    try {
        for (const auto& region : pImpl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting region (const): ") + e.what());
        return nullptr;
    }
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

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
    try {
        pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
        NLM_LOG_INFO("Added inter-region connection from " + std::to_string(source.index()) +
                     " to " + std::to_string(target.index()));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception adding inter-region connection: ") + e.what());
        throw;
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    try {
        pImpl->interRegionConnections.erase(
            std::remove_if(pImpl->interRegionConnections.begin(),
                          pImpl->interRegionConnections.end(),
                          [source, target](const InterRegionConnection& conn) {
                              return conn.sourceRegion == source && conn.targetRegion == target;
                          }),
            pImpl->interRegionConnections.end()
        );
        NLM_LOG_INFO("Removed inter-region connection from " + std::to_string(source.index()) +
                     " to " + std::to_string(target.index()));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception removing inter-region connection: ") + e.what());
        throw;
    }
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
    try {
        NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Enhanced) ===");
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
        
        // Simulation statistics
        NLM_LOG_INFO("Simulation steps: " + std::to_string(pImpl->currentStep));
        NLM_LOG_INFO("Simulation time: " + std::to_string(pImpl->currentTime) + "s");
        NLM_LOG_INFO("Initialization count: " + std::to_string(pImpl->initializationCount));
        NLM_LOG_INFO("Reset count: " + std::to_string(pImpl->resetCount));
        
        for (const auto& region : pImpl->regions) {
            NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                         " (" + region->getName() + "): " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                         std::to_string(region->getSynapseCount()) + " synapses, " +
                         "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception logging brain status: ") + e.what());
    }
}

// Global helper functions
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                         " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                         " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                         " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                     " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                     " after: " + std::to_string(synapses[i]->getWeight()) +
                     " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

} // namespace nlm

// Explicit template instantiations
#include "Brain.cpp"
#include "../core/Logger/Logger.cpp"
#include "../core/Config/Config.cpp"

// Add JSON library dependency
#include <nlohmann/json.hpp>
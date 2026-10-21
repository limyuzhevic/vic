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
    
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot initialize");
        return false;
    }
    
    // CRITICAL NULL CHECK: Ensure config is valid
    if (!pImpl->config) {
        NLM_LOG_ERROR("CRITICAL: Brain config is null - cannot initialize");
        return false;
    }
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Validate configuration values
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount) + ". Must be positive.");
        return false;
    }
    
    if (regionCount == 0) {
        NLM_LOG_ERROR("Invalid region_count: " + std::to_string(regionCount) + ". Must be positive.");
        return false;
    }
    
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connectionProbability) + ". Must be between 0.0 and 1.0.");
        return false;
    }
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // CRITICAL NULL CHECK: Ensure regions were created
    if (pImpl->regions.empty()) {
        NLM_LOG_ERROR("CRITICAL: Failed to create any regions - brain cannot function");
        return false;
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("CRITICAL: Failed to get region " + std::to_string(i + 1) + " - brain cannot function");
            return false;
        }
        
        // Add populations
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        // CRITICAL NULL CHECK: Ensure populations were created
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                if (neuron) {  // CRITICAL NULL CHECK
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
        }
        
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                if (neuron) {  // CRITICAL NULL CHECK
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                    ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
    
    // CRITICAL NULL CHECK: Ensure neurons were created
    if (pImpl->sensoryNeurons.empty() || pImpl->motorNeurons.empty()) {
        NLM_LOG_ERROR("CRITICAL: Failed to create sensory or motor neurons - brain cannot function");
        return false;
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("CRITICAL: Failed to get region " + std::to_string(i + 1) + " during connectivity setup");
            return false;
        }
        
        // CRITICAL NULL CHECK: Ensure random generator exists
        if (!pImpl->rng) {
            NLM_LOG_ERROR("CRITICAL: Random generator is null - cannot initialize connectivity");
            return false;
        }
        
        // Initialize random connectivity and synapse weights
        region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // CRITICAL NULL CHECKS: Ensure all integrated systems can be initialized
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
    } else {
        NLM_LOG_WARNING("Working memory not available");
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
    }
    
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(this);
    }
    
    if (pImpl->predictionSystem) {
        // (PredictionSystem doesn't have initialize method currently)
    }
    
    // CRITICAL NULL CHECK: Ensure cognition systems exist before initializing
    if (pImpl->planner) {
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(5);
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->initialize(this);
    }
    
    if (pImpl->attention) {
        pImpl->attention->initialize(this);
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
    }
    
    // Initialize neuromodulation
    if (pImpl->novelty) {
        pImpl->novelty->initialize(this);
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->initialize(this);
    }
    
    // Register spike handlers for event-driven processing
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
        
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // CRITICAL NULL CHECK: Ensure we can find destination neuron
            for (auto& region : pImpl->regions) {
                if (!region) continue;  // CRITICAL NULL CHECK
                
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (!neuron) continue;  // CRITICAL NULL CHECK
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
    }
    
    // Configure checkpoint manager
    if (pImpl->checkpointManager) {
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
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
    
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot step");
        return;
    }
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // CRITICAL NULL CHECK: Ensure spike system exists
    if (!pImpl->spikeSystem) {
        NLM_LOG_ERROR("CRITICAL: Spike system is null - cannot simulate");
        return;
    }
    
    // CRITICAL NULL CHECK: Ensure random generator exists
    if (!pImpl->rng) {
        NLM_LOG_ERROR("CRITICAL: Random generator is null - cannot simulate");
        return;
    }
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) continue;  // CRITICAL NULL CHECK
            
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) continue;  // CRITICAL NULL CHECK
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) continue;  // CRITICAL NULL CHECK
            
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) continue;  // CRITICAL NULL CHECK
                
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
                        if (!syn) continue;  // CRITICAL NULL CHECK
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        if (!syn) continue;  // CRITICAL NULL CHECK
                        
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
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processSpikes(currentStep);
    }
    
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
            if (!region) continue;  // CRITICAL NULL CHECK
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;  // CRITICAL NULL CHECK
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue;  // CRITICAL NULL CHECK
                    
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
        if (!region) continue;  // CRITICAL NULL CHECK
        
        for (auto& syn : region->getSynapses()) {
            if (!syn) continue;  // CRITICAL NULL CHECK
            
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
                if (!region) continue;  // CRITICAL NULL CHECK
                
                for (auto& pop : region->getPopulations()) {
                    if (!pop) continue;  // CRITICAL NULL CHECK
                    
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) continue;  // CRITICAL NULL CHECK
                        
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
        if (pImpl->structuralPlasticity) {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        }
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
        if (pImpl->developmentSystem) {
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
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot receive sensory input");
        return;
    }
    
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) {
        NLM_LOG_WARNING("No sensory neurons available - cannot process input");
        return;
    }
    
    // CRITICAL NULL CHECK: Ensure sensory neurons array is valid
    if (!pImpl->sensoryNeurons.data()) {
        NLM_LOG_ERROR("CRITICAL: Sensory neurons array is null - cannot process input");
        return;
    }
    
    // Validate input data size
    if (values.size() < numSensory) {
        NLM_LOG_WARNING("Input data size (" + std::to_string(values.size()) + ") < sensory neuron count (" + 
                       std::to_string(numSensory) + "). Using available data only.");
    }
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // CRITICAL NULL CHECK: Ensure each neuron pointer is valid
        if (!pImpl->sensoryNeurons[i]) {
            NLM_LOG_ERROR("CRITICAL: Sensory neuron at index " + std::to_string(i) + " is null");
            continue;
        }
        
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
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot inject current");
        return;
    }
    
    // CRITICAL NULL CHECK: Ensure random generator exists
    if (!pImpl->rng) {
        NLM_LOG_ERROR("CRITICAL: Random generator is null - cannot inject current");
        return;
    }
    
    for (auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (!n) continue;  // CRITICAL NULL CHECK
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
    
    NLM_LOG_WARNING("Neuron " + std::to_string(neuron.index()) + " not found - current not injected");
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot inject current to neurons");
        return;
    }
    
    // Validate current value
    if (std::isnan(current) || std::isinf(current)) {
        NLM_LOG_ERROR("Invalid current value: " + std::to_string(current));
        return;
    }
    
    for (auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) continue;  // CRITICAL NULL CHECK
            
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue;  // CRITICAL NULL CHECK
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get spike system");
        return nullptr;
    }
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get spike system (const)");
        return nullptr;
    }
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get STDP");
        return nullptr;
    }
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get Hebbian");
        return nullptr;
    }
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get structural plasticity");
        return nullptr;
    }
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get E/I ratio");
        return 0.0f;
    }
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        
        for (const auto& syn : region->getSynapses()) {
            if (!syn) continue;  // CRITICAL NULL CHECK
            
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
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get total spike count");
        return 0;
    }
    
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get pending spike count");
        return 0;
    }
    
    // CRITICAL NULL CHECK: Ensure spike system exists
    if (!pImpl->spikeSystem) {
        NLM_LOG_ERROR("CRITICAL: Spike system is null - cannot get pending spike count");
        return 0;
    }
    
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

// ========== SPIKE SYSTEM AND PLASTICITY ACCESSORS ==========

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

std::unique_ptr<class Action> Brain::produceAction() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot produce action");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        NLM_LOG_WARNING("No motor neurons available - cannot produce action");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // CRITICAL NULL CHECK: Ensure motor neurons array is valid
    if (!pImpl->motorNeurons.data()) {
        NLM_LOG_ERROR("CRITICAL: Motor neurons array is null - cannot produce action");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (size_t i = 0; i < pImpl->motorNeurons.size(); ++i) {
        // CRITICAL NULL CHECK: Ensure each neuron pointer is valid
        if (!pImpl->motorNeurons[i]) {
            NLM_LOG_ERROR("CRITICAL: Motor neuron at index " + std::to_string(i) + " is null");
            continue;
        }
        
        if (pImpl->motorNeurons[i]->isFiring()) {
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
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot apply neuromodulation");
        return;
    }
    
    // CRITICAL NULL CHECK: Ensure plasticity system exists
    if (!pImpl->stdp) {
        NLM_LOG_ERROR("CRITICAL: STDP system is null - cannot apply neuromodulation");
        return;
    }
    
    float modulation = signal.getLevel();
    
    // Validate modulation value
    if (std::isnan(modulation) || std::isinf(modulation)) {
        NLM_LOG_ERROR("Invalid neuromodulation level: " + std::to_string(modulation));
        return;
    }
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot develop");
        return;
    }
    
    // CRITICAL NULL CHECK: Ensure structural plasticity system exists
    if (!pImpl->structuralPlasticity) {
        NLM_LOG_ERROR("CRITICAL: Structural plasticity system is null - cannot develop");
        return;
    }
    
    // CRITICAL NULL CHECK: Ensure random generator exists
    if (!pImpl->rng) {
        NLM_LOG_ERROR("CRITICAL: Random generator is null - cannot develop");
        return;
    }
    
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot reset");
        return;
    }
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        region->reset();
    }
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
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
        // CRITICAL NULL CHECK: Ensure pImpl is valid
        if (!pImpl) {
            NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot save");
            return false;
        }
        
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
            if (!region) continue;  // CRITICAL NULL CHECK
            
            for (const auto& pop : region->getPopulations()) {
                if (!pop) continue;  // CRITICAL NULL CHECK
                
                for (const auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue;  // CRITICAL NULL CHECK
                    
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
            if (!region) continue;  // CRITICAL NULL CHECK
            
            for (const auto* syn : region->getSynapses()) {
                if (!syn) continue;  // CRITICAL NULL CHECK
                
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
        // CRITICAL NULL CHECK: Ensure pImpl is valid
        if (!pImpl) {
            NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot load");
            return false;
        }
        
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
            if (!region) continue;  // CRITICAL NULL CHECK
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;  // CRITICAL NULL CHECK
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue;  // CRITICAL NULL CHECK
                    
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
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot add region");
        return RegionId(0);
    }
    
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    if (!region) {
        NLM_LOG_ERROR("CRITICAL: Failed to create neural region - memory allocation failed");
        return RegionId(0);
    }
    
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get region");
        return nullptr;
    }
    
    for (auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get region (const)");
        return nullptr;
    }
    
    for (const auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get region count");
        return 0;
    }
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get region IDs");
        return std::vector<RegionId>();
    }
    
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get regions");
        static const std::vector<std::unique_ptr<NeuralRegion>> empty;
        return empty;
    }
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot add inter-region connection");
        return;
    }
    
    // Validate parameters
    if (std::isnan(weight) || std::isinf(weight)) {
        NLM_LOG_ERROR("Invalid weight value: " + std::to_string(weight));
        return;
    }
    
    if (delay < 0) {
        NLM_LOG_ERROR("Invalid delay value: " + std::to_string(delay));
        return;
    }
    
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot remove inter-region connection");
        return;
    }
    
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
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get total neuron count");
        return 0;
    }
    
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get total synapse count");
        return 0;
    }
    
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get active neuron count");
        return 0;
    }
    
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get firing neuron count");
        return 0;
    }
    
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get average firing rate");
        return 0.0f;
    }
    
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        if (!region) continue;  // CRITICAL NULL CHECK
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get working memory");
        return nullptr;
    }
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get episodic memory");
        return nullptr;
    }
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get associative memory");
        return nullptr;
    }
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get prediction system");
        return nullptr;
    }
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get planner");
        return nullptr;
    }
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get concept formation");
        return nullptr;
    }
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get attention");
        return nullptr;
    }
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get development system");
        return nullptr;
    }
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get developmental stage");
        return DevelopmentalStage::Initial;
    }
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot set developmental stage");
        return;
    }
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get dopamine");
        return nullptr;
    }
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get curiosity");
        return nullptr;
    }
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get novelty");
        return nullptr;
    }
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get prediction error signal");
        return nullptr;
    }
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get config");
        return nullptr;
    }
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot get random generator");
        return nullptr;
    }
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    // CRITICAL NULL CHECK: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("CRITICAL: Brain pImpl is null - cannot log status");
        return;
    }
    
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
        if (!region) continue;  // CRITICAL NULL CHECK
        
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

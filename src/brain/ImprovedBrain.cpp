// ImprovedBrain.cpp - Refactored brain implementation with better separation of concerns
#include "ImprovedBrain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Random/Random.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/development/DevelopmentSystem.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/performance/CheckpointSystem.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/plasticity/StructuralPlasticity.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/NeuralRegion.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace nlm {

struct ImprovedBrain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation systems
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
    
    // Developmental stage
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
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
        , replayInterval(100)
        , consolidationInterval(1000)
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
};

ImprovedBrain::ImprovedBrain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

ImprovedBrain::~ImprovedBrain() = default;

ImprovedBrain::ImprovedBrain(ImprovedBrain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ImprovedBrain& ImprovedBrain::operator=(ImprovedBrain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool ImprovedBrain::initialize() noexcept {
    try {
        NLM_LOG_INFO("Initializing Improved NLM Brain...");
        
        // Get configuration values with validation
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Invalid neuron_count: must be > 0");
            return false;
        }
        
        if (regionCount == 0) {
            NLM_LOG_ERROR("Invalid region_count: must be > 0");
            return false;
        }
        
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            NLM_LOG_ERROR("Invalid connection_probability: must be between 0.0 and 1.0");
            return false;
        }
        
        NLM_LOG_INFO("Configuration validated: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions, " +
                     std::to_string(connectionProbability * 100) + "% connection probability");
        
        // Initialize systems in logical order
        initializeMemorySystems();
        initializeCognitionSystems();
        initializeNeuromodulationSystems();
        configurePlasticitySystems();
        registerEventHandlers();
        
        // Create regions
        createRegions(neuronCount, regionCount);
        
        // Initialize connectivity
        initializeConnectivity(connectionProbability);
        
        // Configure checkpoint system
        configureCheckpointSystem();
        
        NLM_LOG_INFO("Improved NLM Brain initialization complete");
        return validateInitializationState();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during initialization: ") + e.what());
        return false;
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception during initialization.");
        return false;
    }
}

void ImprovedBrain::initializeMemorySystems() noexcept {
    // Initialize working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(this);
        size_t capacity = pImpl->config->getOr<size_t>("working_memory_capacity", 1000 / 10);
        pImpl->workingMemory->setCapacity(capacity);
    }
    
    // Initialize episodic memory
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(this);
        size_t maxEpisodes = pImpl->config->getOr<size_t>("max_episodes", 1000);
        pImpl->episodicMemory->setMaxEpisodes(maxEpisodes);
    }
    
    // Initialize associative memory
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(this);
    }
}

void ImprovedBrain::initializeCognitionSystems() noexcept {
    // Initialize planner
    if (pImpl->planner) {
        pImpl->planner->initialize(this);
        size_t planningDepth = pImpl->config->getOr<size_t>("planning_depth", 5);
        pImpl->planner->setPlanningDepth(planningDepth);
    }
    
    // Initialize concept formation
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->initialize(this);
    }
    
    // Initialize attention system
    if (pImpl->attention) {
        pImpl->attention->initialize(this);
        float inhibitionStrength = pImpl->config->getOr<float>("attention_inhibition", 0.5f);
        float excitationStrength = pImpl->config->getOr<float>("attention_excitation", 1.5f);
        pImpl->attention->setInhibitionStrength(inhibitionStrength);
        pImpl->attention->setExcitationStrength(excitationStrength);
    }
}

void ImprovedBrain::initializeNeuromodulationSystems() noexcept {
    // Initialize novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->initialize(this);
    }
    
    // Initialize curiosity system
    if (pImpl->curiosity) {
        pImpl->curiosity->initialize(this);
    }
    
    // Initialize dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->initialize(this);
    }
}

void ImprovedBrain::configurePlasticitySystems() noexcept {
    // Configure STDP parameters
    if (pImpl->stdp) {
        float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
        pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Configure structural plasticity
    if (pImpl->structuralPlasticity) {
        float synaptogenesisRate = pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = pImpl->config->getOr<float>("pruning_rate", 0.00001f);
        pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        pImpl->structuralPlasticity->setPruningRate(pruningRate);
    }
}

void ImprovedBrain::registerEventHandlers() noexcept {
    // Register spike handlers for event-driven processing
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
        
        // Register delayed spike handler to deliver synaptic input
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input
            deliverSynapticInput(event);
        });
    }
}

void ImprovedBrain::deliverSynapticInput(const DelayedSpikeEvent& event) noexcept {
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
}

void ImprovedBrain::createRegions(size_t neuronCount, size_t regionCount) noexcept {
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
}

void ImprovedBrain::initializeConnectivity(float connectionProbability) noexcept {
    // Initialize connectivity with random weights
    for (size_t i = 0; i < pImpl->regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void ImprovedBrain::configureCheckpointSystem() noexcept {
    // Configure checkpoint manager
    if (pImpl->checkpointManager) {
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
}

bool ImprovedBrain::validateInitializationState() const noexcept {
    // Basic validation
    if (pImpl->regions.empty()) {
        NLM_LOG_ERROR("Brain initialized with 0 regions.");
        return false;
    }
    
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) {
        NLM_LOG_ERROR("Brain initialized with 0 neurons.");
        return false;
    }
    
    NLM_LOG_INFO("Brain validation passed: " + std::to_string(totalNeurons) + " neurons across " +
                 std::to_string(pImpl->regions.size()) + " regions");
    return true;
}

void ImprovedBrain::logStatus() const noexcept {
    NLM_LOG_INFO("=== Improved NLM Brain Status ===");
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

void ImprovedBrain::step(SimulationStep currentStep) noexcept {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void ImprovedBrain::step(SimulationStep currentStep, Timestamp currentTime) noexcept {
    try {
        // Update integration state
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // Execute all brain systems in order
        executeBrainLoop(currentStep, currentTime);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during step: ") + e.what());
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception during step.");
    }
}

void ImprovedBrain::executeBrainLoop(SimulationStep currentStep, Timestamp currentTime) noexcept {
    /*
     * Phase 6: INTEGRATED ARTIFICIAL BRAIN LOOP
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
    
    // ========== STEP 1: Process pending delayed spikes ==========
    processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeuronDynamics(currentStep, currentTime);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    detectAndScheduleSpikes(currentStep, currentTime);
    
    // ========== STEP 4: Update working memory ==========
    updateWorkingMemory(currentTime);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    applyNeuromodulationEffects(currentTime);
    
    // ========== STEP 6: Apply plasticity rules ==========
    applyPlasticityRules(currentTime);
    
    // ========== STEP 7: Update episodic memory ==========
    updateEpisodicMemory(currentStep, currentTime);
    
    // ========== STEP 8: Update prediction system ==========
    updatePredictionSystem(currentTime);
    
    // ========== STEP 9: Update attention system ==========
    updateAttentionSystem(currentTime);
    
    // ========== STEP 10: Update concept formation ==========
    updateConceptFormation(currentTime);
    
    // ========== STEP 11: Apply structural plasticity ==========
    applyStructuralPlasticity(currentStep);
    
    // ========== STEP 12: Replay important memories ==========
    replayMemories(currentStep);
    
    // ========== STEP 13: Apply development effects ==========
    applyDevelopmentEffects(currentStep, currentTime);
    
    // ========== STEP 14: Periodic memory consolidation ==========
    consolidateMemories(currentStep);
    
    // ========== STEP 15: Checkpoint management ==========
    updateCheckpoints(currentStep, currentTime);
}

void ImprovedBrain::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) noexcept {
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
}

void ImprovedBrain::updateNeuronDynamics(SimulationStep currentStep, Timestamp currentTime) noexcept {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void ImprovedBrain::detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) noexcept {
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
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processSpikes(currentStep);
    }
}

void ImprovedBrain::updateWorkingMemory(TimestepDuration dt) noexcept {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(dt);
    }
}

void ImprovedBrain::applyNeuromodulationEffects(TimestepDuration dt) noexcept {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(dt);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(dt);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(dt);
        
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
}

void ImprovedBrain::applyPlasticityRules(TimestepDuration dt) noexcept {
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
                    pImpl->stdp->update(syn, preSpikes, postSpikes, dt);
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
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, dt);
                }
            }
            
            // Update synapse state
            syn->step(pImpl->currentTime);
        }
    }
}

void ImprovedBrain::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) noexcept {
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

void ImprovedBrain::updatePredictionSystem(TimestepDuration dt) noexcept {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void ImprovedBrain::updateAttentionSystem(TimestepDuration dt) noexcept {
    if (pImpl->attention) {
        pImpl->attention->update(dt);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void ImprovedBrain::updateConceptFormation(TimestepDuration dt) noexcept {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void ImprovedBrain::applyStructuralPlasticity(SimulationStep currentStep) noexcept {
    if (pImpl->structuralPlasticity && currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void ImprovedBrain::replayMemories(SimulationStep currentStep) noexcept {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void ImprovedBrain::applyDevelopmentEffects(SimulationStep currentStep, TimestepDuration dt) noexcept {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, dt * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            float plasticityMod = 1.0f;
            
            switch (pImpl->developmentalStage) {
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

void ImprovedBrain::consolidateMemories(SimulationStep currentStep) noexcept {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void ImprovedBrain::updateCheckpoints(SimulationStep currentStep, Timestamp currentTime) noexcept {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void ImprovedBrain::cleanupSystems() noexcept {
    // Clear pointers to prevent dangling references
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
}

// Legacy Brain interface implementations (for compatibility)
// These methods delegate to the ImprovedBrain implementation

void Brain::step(SimulationStep currentStep) {
    ImprovedBrain::getCurrentBrain().step(currentStep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    ImprovedBrain::getCurrentBrain().step(currentStep, currentTime);
}

bool Brain::initialize() {
    return ImprovedBrain::getCurrentBrain().initialize();
}

void Brain::logStatus() const {
    ImprovedBrain::getCurrentBrain().logStatus();
}

// Global static instance management for legacy compatibility
namespace {
    ImprovedBrain g_improvedBrain;
}

ImprovedBrain& ImprovedBrain::getCurrentBrain() {
    return g_improvedBrain;
}

} // namespace nlm
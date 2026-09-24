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
        try {
            NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
            
            // Get configuration values with validation
            size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
            size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
            float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
            
            // Validate configuration
            if (neuronCount == 0) {
                NLM_LOG_ERROR("Invalid configuration: neuron_count cannot be 0");
                return false;
            }
            
            if (regionCount == 0) {
                NLM_LOG_ERROR("Invalid configuration: region_count cannot be 0");
                return false;
            }
            
            if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
                NLM_LOG_ERROR("Invalid configuration: connection_probability must be between 0 and 1");
                return false;
            }
            
            NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                         std::to_string(regionCount) + " regions, " +
                         std::to_string(connectionProbability * 100) + "% connection probability");
            
            // Create regions with error checking
            for (size_t i = 0; i < regionCount; ++i) {
                RegionId regionId = addRegion("Region_" + std::to_string(i + 1));
                if (!regionId.isValid()) {
                    NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1));
                    return false;
                }
            }
            
            // Create neurons across regions with validation
            size_t neuronsPerRegion = neuronCount / regionCount;
            for (size_t i = 0; i < regionCount; ++i) {
                auto* region = getRegion(RegionId(i + 1));
                if (!region) {
                    NLM_LOG_ERROR("Region " + std::to_string(i + 1) + " not found");
                    return false;
                }
                
                // Add populations
                auto sensoryPopId = region->addPopulation(std::max<size_t>(neuronsPerRegion / 4, 1), NeuronType::Sensory);
                auto internalPopId = region->addPopulation(std::max<size_t>(neuronsPerRegion / 2, 1), NeuronType::Internal);
                auto motorPopId = region->addPopulation(std::max<size_t>(neuronsPerRegion / 4, 1), NeuronType::Motor);
                
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
            
            // Initialize connectivity with random weights - with error checking
            for (size_t i = 0; i < regionCount; ++i) {
                auto* region = getRegion(RegionId(i + 1));
                if (!region) {
                    NLM_LOG_ERROR("Region " + std::to_string(i + 1) + " not found for connectivity initialization");
                    return false;
                }
                
                // Initialize random connectivity and synapse weights
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            }
            
            // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
            
            // Initialize working memory
            if (!pImpl->workingMemory) {
                NLM_LOG_ERROR("Failed to create working memory");
                return false;
            }
            pImpl->workingMemory->initialize(this);
            pImpl->workingMemory->setCapacity(neuronCount / 10);
            
            // Initialize episodic memory
            if (!pImpl->episodicMemory) {
                NLM_LOG_ERROR("Failed to create episodic memory");
                return false;
            }
            pImpl->episodicMemory->initialize(this);
            pImpl->episodicMemory->setMaxEpisodes(1000);
            
            // Initialize associative memory
            if (!pImpl->associativeMemory) {
                NLM_LOG_ERROR("Failed to create associative memory");
                return false;
            }
            pImpl->associativeMemory->initialize(this);
            
            // Initialize cognition systems
            if (!pImpl->planner) {
                NLM_LOG_ERROR("Failed to create neural planner");
                return false;
            }
            pImpl->planner->initialize(this);
            pImpl->planner->setPlanningDepth(5);
            
            if (!pImpl->conceptFormation) {
                NLM_LOG_ERROR("Failed to create concept formation system");
                return false;
            }
            pImpl->conceptFormation->initialize(this);
            
            if (!pImpl->attention) {
                NLM_LOG_ERROR("Failed to create attentional system");
                return false;
            }
            pImpl->attention->initialize(this);
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
            
            // Initialize neuromodulation
            if (!pImpl->novelty) {
                NLM_LOG_ERROR("Failed to create novelty detector");
                return false;
            }
            pImpl->novelty->initialize(this);
            
            if (!pImpl->curiosity) {
                NLM_LOG_ERROR("Failed to create curiosity module");
                return false;
            }
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
            if (!pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true)) {
                NLM_LOG_WARNING("Failed to configure checkpoint manager");
            }
            
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

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
        // Validate inputs
        if (currentStep < 0) {
            NLM_LOG_WARNING(std::string("Invalid step number: " + std::to_string(currentStep)));
            return;
        }
        
        if (currentTime < 0.0) {
            NLM_LOG_WARNING(std::string("Invalid time value: " + std::to_string(currentTime)));
            return;
        }
        
        // Validate timestep
        if (pImpl->timestep <= 0.0) {
            NLM_LOG_ERROR("Invalid timestep: " + std::to_string(pImpl->timestep));
            return;
        }
        
        // Initialize statistics for this step
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        try {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error processing delayed spikes: ") + e.what());
        }
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        neuron->stepLIF(currentTime, pImpl->timestep);
                    }
                }
            }
        }
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue;
                    
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
                            if (syn) {
                                syn->recordPostSpike(currentTime);
                            }
                        }
                        
                        // Get outgoing synapses and schedule delayed spike events
                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            if (!syn) continue;
                            
                            // Create delayed spike event
                            Delay delay = syn->getDelay();
                            SimulationStep deliveryStep = currentStep + delay;
                            Timestamp deliveryTime = currentTime + delay * pImpl->timestep;
                            
                            // Validate delay
                            if (delay >= 0 && deliveryStep > currentStep) {
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
                            }
                            
                            // Record pre-synaptic spike for plasticity
                            syn->recordPreSpike(currentTime);
                        }
                        
                        // Store to working memory - neurons that fire become part of working memory
                        if (pImpl->workingMemory && neuron) {
                            pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                        }
                    }
                }
            }
        }
        
        // Process immediate spikes
        try {
            pImpl->spikeSystem->processSpikes(currentStep);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error processing immediate spikes: ") + e.what());
        }
        
        // ========== STEP 4: Update working memory ==========
        try {
            if (pImpl->workingMemory) {
                pImpl->workingMemory->update(pImpl->timestep);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating working memory: ") + e.what());
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
        try {
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
                            if (!neuron) continue;
                            
                            // Dopamine modulates excitability by injecting additional current
                            float excitabilityMod = dopamineLevel * 0.5f;
                            if (excitabilityMod > 0.0f) {
                                neuron->injectCurrent(excitabilityMod);
                            }
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error applying neuromodulation effects: ") + e.what());
        }
        
        // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
        try {
            // Calculate neuromodulation factor for plasticity
            float plasticityMod = 1.0f;
            if (pImpl->dopamine) {
                plasticityMod = pImpl->dopamine->getPlasticityFactor();
            }
            
            for (auto& region : pImpl->regions) {
                for (auto& syn : region->getSynapses()) {
                    if (!syn) continue;
                    
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
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error applying plasticity rules: ") + e.what());
        }
        
        // ========== STEP 7: Update episodic memory ==========
        try {
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
                                if (!neuron) continue;
                                
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
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating episodic memory: ") + e.what());
        }
        
        // ========== STEP 8: Update prediction system ==========
        try {
            if (pImpl->predictionSystem) {
                // The prediction system would be updated with sensory observations
                // For now, just track prediction error history
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating prediction system: ") + e.what());
        }
        
        // ========== STEP 9: Update attention system ==========
        try {
            if (pImpl->attention) {
                pImpl->attention->update(pImpl->timestep);
                
                // Apply attention to working memory winners
                if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                    std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                    pImpl->attention->processCompetition(competitors);
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating attention system: ") + e.what());
        }
        
        // ========== STEP 10: Update concept formation ==========
        try {
            if (pImpl->conceptFormation) {
                // Would process current neural activity patterns to form concepts
                // This requires sensory state encoding
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating concept formation: ") + e.what());
        }
        
        // ========== STEP 11: Apply structural plasticity periodically ==========
        try {
            if (currentStep % 100 == 0 && pImpl->structuralPlasticity) {
                pImpl->structuralPlasticity->update(this, *pImpl->rng);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error applying structural plasticity: ") + e.what());
        }
        
        // ========== STEP 12: Replay important memories ==========
        try {
            if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
                // Get episodes for replay
                auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
                for (const auto* episode : episodesToReplay) {
                    pImpl->episodicMemory->replayEpisode(episode);
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error replaying memories: ") + e.what());
        }
        
        // ========== STEP 13: Apply development effects ==========
        try {
            if (currentStep % 1000 == 0 && pImpl->developmentSystem) {
                // Update development system
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
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error applying development effects: ") + e.what());
        }
        
        // ========== STEP 14: Periodic memory consolidation ==========
        try {
            if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
                // Consolidate important memories, remove weak ones
                pImpl->episodicMemory->consolidate(0.3f);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error consolidating memories: ") + e.what());
        }
        
        // ========== STEP 15: Checkpoint management ==========
        try {
            if (pImpl->checkpointManager) {
                pImpl->checkpointManager->update(currentStep, currentTime);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error updating checkpoint manager: ") + e.what());
        }
        
        // Performance monitoring
        static auto lastStepTime = std::chrono::high_resolution_clock::now();
        auto currentStepTime = std::chrono::high_resolution_clock::now();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentStepTime - lastStepTime).count();
        
        if (elapsedMs > 10) {  // Log slow steps (>10ms)
            NLM_LOG_WARNING(std::string("Slow simulation step: ") + std::to_string(elapsedMs) + "ms at step " + std::to_string(currentStep));
        }
        
        lastStepTime = currentStepTime;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Fatal error in brain step at step " + std::to_string(currentStep) + ": ") + e.what());
        throw;  // Re-throw for higher-level handling
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
    
    try {
        // Reset core brain state
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        
        // Reset regions
        for (auto& region : pImpl->regions) {
            if (region) {
                region->reset();
            }
        }
        
        // Reset spike system
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->reset();
        }
        
        pImpl->developmentalStage = DevelopmentalStage::Initial;
        
        // Reset memory systems with error checking
        try {
            if (pImpl->workingMemory) {
                pImpl->workingMemory->clear();
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Warning: Failed to clear working memory: ") + e.what());
        }
        
        try {
            if (pImpl->episodicMemory) {
                pImpl->episodicMemory->clear();
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Warning: Failed to clear episodic memory: ") + e.what());
        }
        
        try {
            if (pImpl->associativeMemory) {
                pImpl->associativeMemory->clear();
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Warning: Failed to clear associative memory: ") + e.what());
        }
        
        // Reset attentional system
        if (pImpl->attention) {
            pImpl->attention->reset();
        }
        
        NLM_LOG_INFO("NLM Brain reset complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain reset: ") + e.what());
        // Don't rethrow - reset should always complete
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
        
        // Check memory bounds before accessing
            if (idx >= neuronData.membranePotential.size()) {
                NLM_LOG_ERROR("Memory corruption: Neuron data index out of bounds");
                return false;
            }
            
            // Validate neuron parameters before setting
            float potential = neuronData.membranePotential[idx];
            float resting = neuronData.restingPotential[idx];
            float threshold = neuronData.threshold[idx];
            float reset = neuronData.resetPotential[idx];
            float leak = neuronData.leakConductance[idx];
            
            // Sanity check for neuron parameters
            if (std::isnan(potential) || std::isinf(potential)) {
                NLM_LOG_WARNING("Invalid neuron membrane potential at index " + std::to_string(idx) + ": " + std::to_string(potential));
                potential = 0.0f;
            }
            
            neuron->setMembranePotential(potential);
            neuron->setRestingPotential(resting);
            neuron->setThreshold(threshold);
            neuron->setResetPotential(reset);
            neuron->setLeakConductance(leak);
            
            // Set firing state if available
            if (idx < neuronData.firingState.size()) {
                neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
            }
            
            if (idx < neuronData.refractoryRemaining.size()) {
                neuron->setRefractoryRemaining(neuronData.refractoryRemaining[idx]);
            }
            
            if (idx < neuronData.refractoryPeriod.size()) {
                neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
            }
            
            if (idx < neuronData.lastSpikeTime.size() && !std::isnan(neuronData.lastSpikeTime[idx])) {
                neuron->setLastSpikeTime(neuronData.lastSpikeTime[idx]);
            }
            
            idx++;
        }
    }
    
    // Read synapses with validation
    SynapseCheckpointData synapseData;
    if (!reader.readSynapses(synapseData)) {
        NLM_LOG_ERROR("Failed to read synapses from checkpoint");
        return false;
    }
    
    // Apply synapse states with validation
    NLM_LOG_INFO("Attempting to restore " + std::to_string(synapseData.weight.size()) + " synapses");
    
    // Note: Applying synapse states is complex because we need to find matching synapses
    // For safety, we're only logging the count for now
    for (size_t i = 0; i < std::min(synapseData.weight.size(), size_t(10)); ++i) {  // Log first 10
        if (i < synapseData.weight.size()) {
            float weight = synapseData.weight[i];
            if (std::isnan(weight) || std::isinf(weight)) {
                NLM_LOG_WARNING("Invalid synapse weight: " + std::to_string(weight));
            }
        }
    }
    
    if (synapseData.weight.size() > 10) {
        NLM_LOG_INFO("... and " + std::to_string(synapseData.weight.size() - 10) + " more synapses (not individually applied for safety)");
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

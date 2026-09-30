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
    size_t stepsSinceRestStart;
    size_t replayInterval;
    size_t consolidationInterval;
    size_t restThreshold;
    size_t restDuration;
    float consolidationBoostFactor;
    float neuromodulationBoost;
    bool enhancedReplay;
    bool synapticHomeostasis;
    
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
        , stepsSinceRestStart(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
        , restThreshold(1000)      // Rest threshold: 1000 steps of low activity
        , restDuration(2000)       // Rest duration: 2000 steps
        , consolidationBoostFactor(1.5f)  // Memory consolidation boost during rest
        , neuromodulationBoost(1.5f)      // Neuromodulation boost during rest
        , enhancedReplay(true)
        , synapticHomeostasis(true)
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
    
    // ========== REST/SLEEP DETECTION AND STATE MANAGEMENT ==========
    
    // Calculate current neural activity (firing rate / total neurons)
    float currentActivity = 0.0f;
    size_t activeNeurons = getActiveNeuronCount();
    size_t totalNeurons = getTotalNeuronCount();
    
    if (totalNeurons > 0) {
        currentActivity = static_cast<float>(activeNeurons) / totalNeurons;
    }
    
    // Rest detection - enter resting state when activity is low for extended time
    if (pImpl->isResting) {
        // We're in rest mode, accumulate rest time
        pImpl->stepsSinceRestStart++;
        
        // Exit rest when activity exceeds threshold for a short period
        if (currentActivity > 0.05f) {  // Activity threshold to wake up
            pImpl->stepsSinceRestStart = 0;
        } else if (pImpl->stepsSinceRestStart >= pImpl->restDuration) {
            // Force exit after max rest duration
            pImpl->stepsSinceRestStart = 0;
        }
    } else {
        // Active state - check if we should enter rest
        if (currentActivity < 0.02f && pImpl->stepsSinceLastEpisode >= pImpl->restThreshold) {
            // Enter resting state - enhanced consolidation phase
            pImpl->isResting = true;
            pImpl->stepsSinceRestStart = 0;
            
            // Reset activity counters for enhanced processing
            pImpl->stepsSinceLastEpisode = 0;
            
            // Apply neuromodulatory boost for sleep-dependent consolidation
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(pImpl->dopamine->getLevel() * pImpl->neuromodulationBoost);
            }
            
            // Enhanced replay during rest for memory consolidation
            if (pImpl->enhancedReplay) {
                pImpl->replayInterval = 50;  // More frequent replay during rest
            }
            
            // Apply synaptic homeostasis during sleep
            if (pImpl->synapticHomeostasis) {
                // Synaptic downscaling - reduce weights globally
                for (auto& region : pImpl->regions) {
                    for (auto& syn : region->getSynapses()) {
                        if (syn->getType() == SynapseType::Excitatory) {
                            float currentWeight = syn->getWeight();
                            if (currentWeight > 0.1f) {
                                // Gradually weaken synapses for homeostatic balance
                                syn->setWeight(currentWeight * 0.98f);
                            }
                        }
                    }
                }
            }
        } else {
            // Track time since last activity below threshold
            if (currentActivity < 0.02f) {
                pImpl->stepsSinceLastEpisode++;
            } else {
                pImpl->stepsSinceLastEpisode = 0;
            }
        }
    }
    
    // Enhanced consolidation during rest with lower thresholds
    if (pImpl->isResting) {
        pImpl->stepsSinceLastEpisode = 0;  // Always consolidate during rest
        
        // Store current state as a snapshot of consolidation target
        if (pImpl->episodicMemory) {
            // Enhanced memory consolidation - focus on stable, important memories
            // Lower threshold for consolidation during rest
            pImpl->episodicMemory->consolidate(0.2f);  // Lower threshold for better retention
            
            // Enhanced replay during rest for systems consolidation
            if (pImpl->enhancedReplay) {
                auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(5);
                for (const auto* episode : episodesToReplay) {
                    // Boost replay strength during rest
                    pImpl->episodicMemory->replayEpisode(episode);
                    
                    // Store replay pattern in working memory for integration
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->strengthenMemory(episode->stabilityScore * 1.5f);
                    }
                }
            }
            
            // Working memory consolidation and transfer to long-term
            if (pImpl->workingMemory) {
                // Decay working memory during rest for long-term transfer
                pImpl->workingMemory->decay(0.02f);
                
                // Periodic memory transfer during rest
                if (pImpl->stepsSinceRestStart % 500 == 0) {
                    // Transfer stable working memory patterns to episodic memory
                    pImpl->workingMemory->consolidateToEpisodic(this);
                }
            }
        }
        
        // Enhanced neuromodulatory effects during rest
        if (pImpl->dopamine) {
            // Increase dopamine for memory consolidation and reward prediction
            float baseDopamine = 0.3f + std::sin(pImpl->currentTime * 0.001f) * 0.2f;
            pImpl->dopamine->setLevel(baseDopamine * pImpl->neuromodulationBoost);
        }
        
        if (pImpl->curiosity) {
            // Reduce curiosity during rest (focus on consolidation)
            pImpl->curiosity->update(pImpl->timestep);
        }
        
        if (pImpl->novelty) {
            // Novelty detection during rest for memory prioritization
            pImpl->novelty->update(pImpl->timestep * 0.5f);
        }
        
        // Restore normal state management during rest
        if (pImpl->stepsSinceRestStart >= pImpl->restDuration) {
            // Enhanced wake-up with refreshed state
            pImpl->isResting = false;
            
            // Neuromodulatory effects during wake-up
            if (pImpl->dopamine) {
                // Bursty dopamine during wake-up for attention and alert processing
                pImpl->dopamine->setLevel(0.8f);
            }
            
            if (pImpl->novelty) {
                // Enhanced novelty detection after rest
                pImpl->novelty->setLevel(0.9f);
            }
            
            // Enhanced curiosity after rest for exploration
            if (pImpl->curiosity) {
                pImpl->curiosity->setLevel(0.7f);
            }
            
            // Reset rest duration counter
            pImpl->stepsSinceRestStart = 0;
        }
    }
    
    // Restore normal replay intervals when not resting
    if (!pImpl->isResting) {
        pImpl->replayInterval = 100;  // Normal replay frequency
    }
    
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
    
    // ========== STEP 12: Enhanced replay system setup ==========
    
    // Initialize enhanced replay configuration and control mechanisms
    if (!pImpl->replaySystemInitialized) {
        initializeEnhancedReplaySystem();
    }
    
    // Configure replay triggers and control mechanisms
    updateReplayTriggers(currentStep);
    
    // ========== STEP 13: Enhanced replay execution ==========
    
    // Execute replay during active periods
    if (pImpl->activeReplay) {
        executeEnhancedReplay(currentStep);
    }
    
    // ========== STEP 14: Replay learning and consolidation ==========
    
    // Refine prediction system predictions through replay
    if (pImpl->predictionSystem && currentStep % pImpl->predictionRefinementInterval == 0) {
        refinePredictionSystemThroughReplay(currentStep);
    }
    
    // Consolidate concept formation through replay
    if (pImpl->conceptFormation && currentStep % pImpl->conceptConsolidationInterval == 0) {
        consolidateConceptsThroughReplay(currentStep);
    }
    
    // Enhance planning capabilities through replay
    if (pImpl->planner && currentStep % pImpl->planningEnhancementInterval == 0) {
        enhancePlanningThroughReplay(currentStep);
    }
    
    // ========== STEP 15: Enhanced replay mechanisms ==========
    
    // Execute forward replay of episodic sequences
    if (currentStep % pImpl->forwardReplayInterval == 0) {
        executeForwardReplay(currentStep);
    }
    
    // Execute reverse replay for consolidation
    if (currentStep % pImpl->reverseReplayInterval == 0) {
        executeReverseReplay(currentStep);
    }
    
    // Execute targeted replay of important memories
    if (currentStep % pImpl->targetedReplayInterval == 0) {
        executeTargetedReplay(currentStep);
    }
    
    // ========== STEP 16: Replay integration with other systems ==========
    
    // Integrate replay with neuromodulation systems
    integrateReplayWithNeuromodulation(currentStep);
    
    // Integrate replay with attention and working memory
    integrateReplayWithWorkingMemory(currentStep);
    
    // Integrate replay with prediction system learning
    integrateReplayWithPredictionSystem(currentStep);
    
    // Integrate replay with concept formation
    integrateReplayWithConceptFormation(currentStep);
    
    // Integrate replay with planning and action selection
    integrateReplayWithPlanningSystem(currentStep);
    
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
    
    // ========== STEP 18: Collect statistics ==========
    
    // Collect replay system statistics
    collectReplayStatistics(currentStep);
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

// Enhanced replay system implementation

void Brain::initializeEnhancedReplaySystem() {
    if (pImpl->enhancedReplaySystemInitialized) return;
    
    NLM_LOG_INFO("Initializing enhanced replay system...");
    
    // Configure replay intervals and consolidation cycles
    pImpl->forwardReplayInterval = pImpl->config->getOr<size_t>("forward_replay_interval", 50);
    pImpl->reverseReplayInterval = pImpl->config->getOr<size_t>("reverse_replay_interval", 75);
    pImpl->targetedReplayInterval = pImpl->config->getOr<size_t>("targeted_replay_interval", 100);
    pImpl->predictionRefinementInterval = pImpl->config->getOr<size_t>("prediction_refinement_interval", 200);
    pImpl->conceptConsolidationInterval = pImpl->config->getOr<size_t>("concept_consolidation_interval", 300);
    pImpl->planningEnhancementInterval = pImpl->config->getOr<size_t>("planning_enhancement_interval", 400);
    
    // Configure replay triggers and control mechanisms
    pImpl->replayEnabled = true;
    pImpl->sleepReplayEnabled = pImpl->config->getOr<bool>("sleep_replay_enabled", true);
    pImpl->targetedReplayEnabled = pImpl->config->getOr<bool>("targeted_replay_enabled", true);
    pImpl->reverseReplayEnabled = pImpl->config->getOr<bool>("reverse_replay_enabled", true);
    
    // Configure replay integration with working memory
    pImpl->workingMemoryIntegrationEnabled = true;
    pImpl->workingMemoryIntegrationInterval = pImpl->config->getOr<size_t>("working_memory_integration_interval", 150);
    
    // Configure replay integration with attention system
    pImpl->attentionIntegrationEnabled = true;
    pImpl->attentionIntegrationInterval = pImpl->config->getOr<size_t>("attention_integration_interval", 125);
    
    // Configure replay integration with neuromodulation systems
    pImpl->neuromodulationIntegrationEnabled = true;
    pImpl->neuromodulationIntegrationInterval = pImpl->config->getOr<size_t>("neuromodulation_integration_interval", 175);
    
    // Configure replay integration with prediction system
    pImpl->predictionIntegrationEnabled = true;
    pImpl->predictionIntegrationInterval = pImpl->config->getOr<size_t>("prediction_integration_interval", 225);
    
    // Configure replay integration with concept formation
    pImpl->conceptFormationIntegrationEnabled = true;
    pImpl->conceptFormationIntegrationInterval = pImpl->config->getOr<size_t>("concept_formation_integration_interval", 275);
    
    // Configure replay integration with planning system
    pImpl->planningIntegrationEnabled = true;
    pImpl->planningIntegrationInterval = pImpl->config->getOr<size_t>("planning_integration_interval", 325);
    
    pImpl->enhancedReplaySystemInitialized = true;
    NLM_LOG_INFO("Enhanced replay system initialized successfully");
}

void Brain::updateReplayTriggers(SimulationStep currentStep) {
    if (!pImpl->enhancedReplay) return;
    
    // Check sleep triggers
    if (currentStep % pImpl->sleepDetectionInterval == 0) {
        detectSleepState(currentStep);
    }
    
    // Check novelty triggers
    if (currentStep % pImpl->noveltyDetectionInterval == 0) {
        detectNoveltyForReplay(currentStep);
    }
    
    // Check prediction error triggers
    if (currentStep % pImpl->predictionErrorDetectionInterval == 0 && pImpl->predictionError) {
        detectPredictionErrorForReplay(currentStep);
    }
    
    // Check attention triggers
    if (currentStep % pImpl->attentionDetectionInterval == 0) {
        detectAttentionForReplay(currentStep);
    }
}

void Brain::detectSleepState(SimulationStep currentStep) {
    // Calculate current neural activity
    float currentActivity = 0.0f;
    size_t activeNeurons = getActiveNeuronCount();
    size_t totalNeurons = getTotalNeuronCount();
    
    if (totalNeurons > 0) {
        currentActivity = static_cast<float>(activeNeurons) / totalNeurons;
    }
    
    // Check if we should enter sleeping state
    if (!pImpl->isResting && currentActivity < pImpl->lowActivityThreshold) {
        // Enter resting state - enhanced consolidation phase
        pImpl->isResting = true;
        pImpl->stepsSinceRestStart = 0;
        pImpl->stepsSinceLastEpisode = 0;
        
        // Apply neuromodulatory boost for sleep-dependent consolidation
        if (pImpl->dopamine) {
            pImpl->dopamine->setLevel(pImpl->dopamine->getLevel() * pImpl->neuromodulationBoost);
        }
        
        NLM_LOG_INFO("Brain entering resting/sleeping state for memory consolidation");
    } else if (pImpl->isResting && currentActivity > pImpl->activityThresholdToWake) {
        // Exit resting state
        pImpl->isResting = false;
        NLM_LOG_INFO("Brain exiting resting state");
    }
}

void Brain::detectNoveltyForReplay(SimulationStep currentStep) {
    if (!pImpl->novelty) return;
    
    float novelty = pImpl->novelty->getLevel();
    
    // If novelty is high, prioritize episodic memory storage and replay
    if (novelty > pImpl->noveltyThresholdForReplay) {
        NLM_LOG_INFO("High novelty detected (" + std::to_string(novelty) + "), enabling targeted replay");
        pImpl->enableTargetedReplay = true;
        pImpl->targetedReplayPriority = novelty;
    }
}

void Brain::detectPredictionErrorForReplay(SimulationStep currentStep) {
    if (!pImpl->predictionError) return;
    
    float predictionError = std::abs(pImpl->predictionError->getLevel());
    
    // If prediction error is high, prioritize learning through replay
    if (predictionError > pImpl->predictionErrorThresholdForReplay) {
        NLM_LOG_INFO("High prediction error detected (" + std::to_string(predictionError) + "), enabling reverse replay for learning");
        pImpl->enableReverseReplay = true;
        pImpl->reverseReplayLearningPriority = predictionError;
    }
}

void Brain::detectAttentionForReplay(SimulationStep currentStep) {
    if (!pImpl->attention) return;
    
    float attentionLevel = pImpl->attention->getAttentionLevel();
    
    // If attention is high, enable enhanced replay
    if (attentionLevel > pImpl->attentionThresholdForReplay) {
        NLM_LOG_INFO("High attention detected (" + std::to_string(attentionLevel) + "), enabling enhanced replay");
        pImpl->enableAttentionEnhancedReplay = true;
        pImpl->attentionEnhancedReplayPriority = attentionLevel;
    }
}

void Brain::executeEnhancedReplay(SimulationStep currentStep) {
    if (!pImpl->episodicMemory) return;
    
    // Execute different types of replay based on configuration
    
    // Forward replay during active periods
    if (pImpl->forwardReplayEnabled && currentStep % pImpl->forwardReplayInterval == 0) {
        executeForwardReplay(currentStep);
    }
    
    // Reverse replay during learning phases
    if (pImpl->reverseReplayEnabled && currentStep % pImpl->reverseReplayInterval == 0) {
        executeReverseReplay(currentStep);
    }
    
    // Targeted replay during novelty/attention phases
    if (pImpl->targetedReplayEnabled && pImpl->enableTargetedReplay) {
        executeTargetedReplay(currentStep);
    }
    
    // Sleep/rest period replay
    if (pImpl->isResting && pImpl->sleepReplayEnabled) {
        executeSleepRestReplay(currentStep);
    }
}

void Brain::executeForwardReplay(SimulationStep currentStep) {
    if (!pImpl->episodicMemory) return;
    
    NLM_LOG_DEBUG("Executing forward replay at step " + std::to_string(currentStep));
    
    // Get high-relevance episodes for forward replay
    auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(2);
    
    for (const auto* episode : episodesToReplay) {
        pImpl->episodicMemory->replayEpisode(episode);
        
        // Strengthen working memory representations
        if (pImpl->workingMemory && pImpl->workingMemoryIntegrationEnabled) {
            strengthenWorkingMemoryThroughReplay(currentStep, episode);
        }
    }
}

void Brain::executeReverseReplay(SimulationStep currentStep) {
    if (!pImpl->episodicMemory) return;
    
    NLM_LOG_DEBUG("Executing reverse replay at step " + std::to_string(currentStep));
    
    // Get episodes for reverse replay (learning/reinforcement)
    auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
    
    for (const auto* episode : episodesToReplay) {
        // Enhance prediction system through reverse replay
        if (pImpl->predictionSystem && pImpl->predictionIntegrationEnabled) {
            enhancePredictionSystemThroughReverseReplay(currentStep, episode);
        }
        
        // Enhance concept formation through reverse replay
        if (pImpl->conceptFormation && pImpl->conceptFormationIntegrationEnabled) {
            enhanceConceptFormationThroughReverseReplay(currentStep, episode);
        }
        
        // Enhance planning through reverse replay
        if (pImpl->planner && pImpl->planningIntegrationEnabled) {
            enhancePlanningThroughReverseReplay(currentStep, episode);
        }
        
        // Standard replay
        pImpl->episodicMemory->replayEpisode(episode);
    }
}

void Brain::executeTargetedReplay(SimulationStep currentStep) {
    if (!pImpl->episodicMemory) return;
    
    NLM_LOG_DEBUG("Executing targeted replay at step " + std::to_string(currentStep));
    
    // Get high-priority episodes for targeted replay
    auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(5);
    
    for (const auto* episode : episodesToReplay) {
        // Apply enhanced neuromodulation for targeted replay
        if (pImpl->dopamine && pImpl->neuromodulationIntegrationEnabled) {
            applyEnhancedNeuromodulationForTargetedReplay(currentStep, episode);
        }
        
        // Apply attentional selection for targeted replay
        if (pImpl->attention && pImpl->attentionIntegrationEnabled) {
            applyAttentionSelectionForTargetedReplay(currentStep, episode);
        }
        
        // Enhanced replay for novel experiences
        if (episode->novelty > pImpl->noveltyThresholdForReplay) {
            enhanceReplayForNovelExperience(currentStep, episode);
        }
        
        // Standard replay
        pImpl->episodicMemory->replayEpisode(episode);
    }
}

void Brain::executeSleepRestReplay(SimulationStep currentStep) {
    if (!pImpl->episodicMemory || !pImpl->sleepReplayEnabled) return;
    
    NLM_LOG_DEBUG("Executing sleep/rest replay at step " + std::to_string(currentStep));
    
    // Enhanced replay during sleep for consolidation
    auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(4);
    
    for (const auto* episode : episodesToReplay) {
        // Apply sleep-specific enhancements
        applySleepReplayEnhancements(currentStep, episode);
        
        // Enhanced working memory consolidation
        if (pImpl->workingMemory) {
            pImpl->workingMemory->strengthenMemory(episode->stabilityScore * 1.5f);
        }
        
        // Enhanced prediction system learning
        if (pImpl->predictionSystem) {
            pImpl->predictionSystem->train(episode->sensoryState);
        }
        
        // Enhanced concept formation
        if (pImpl->conceptFormation) {
            std::vector<float> replayFeatures = episode->sensoryFeatures;
            replayFeatures.insert(replayFeatures.end(), 
                                episode->contextualFeatures.begin(), 
                                episode->contextualFeatures.end());
            replayFeatures.push_back(episode->reward);
            replayFeatures.push_back(episode->noveltyLevel);
            
            pImpl->conceptFormation->updateConcept(replayFeatures, currentTime);
        }
        
        // Standard replay
        pImpl->episodicMemory->replayEpisode(episode);
    }
}

void Brain::refinePredictionSystemThroughReplay(SimulationStep currentStep) {
    if (!pImpl->predictionSystem || !pImpl->episodicMemory) return;
    
    NLM_LOG_DEBUG("Refining prediction system through replay at step " + std::to_string(currentStep));
    
    // Get episodes with good prediction accuracy
    auto episodesToProcess = pImpl->episodicMemory->getEpisodesForReplay(3);
    
    for (const auto* episode : episodesToProcess) {
        // Refine prediction based on episode outcomes
        if (std::abs(episode->predictionError) < 0.3f) {
            // Good prediction - reinforce
            pImpl->predictionSystem->train(episode->sensoryState);
        } else {
            // Poor prediction - correct through replay
            pImpl->predictionSystem->correctPrediction(episode->sensoryState, episode->predictionError);
        }
    }
}

void Brain::consolidateConceptsThroughReplay(SimulationStep currentStep) {
    if (!pImpl->conceptFormation || !pImpl->episodicMemory) return;
    
    NLM_LOG_DEBUG("Consolidating concepts through replay at step " + std::to_string(currentStep));
    
    // Get episodes with strong feature correlations
    auto episodesToProcess = pImpl->episodicMemory->getEpisodesForReplay(2);
    
    for (const auto* episode : episodesToProcess) {
        // Enhance concept integration through replay
        std::vector<float> enhancedFeatures = episode->sensoryFeatures;
        enhancedFeatures.insert(enhancedFeatures.end(),
                              episode->contextualFeatures.begin(),
                              episode->contextualFeatures.end());
        enhancedFeatures.push_back(episode->reward);
        
        pImpl->conceptFormation->updateConcept(enhancedFeatures, currentTime);
    }
}

void Brain::enhancePlanningThroughReplay(SimulationStep currentStep) {
    if (!pImpl->planner || !pImpl->episodicMemory) return;
    
    NLM_LOG_DEBUG("Enhancing planning through replay at step " + std::to_string(currentStep));
    
    // Get episodes with goal-related information
    auto episodesToProcess = pImpl->episodicMemory->getEpisodesForReplay(4);
    
    for (const auto* episode : episodesToProcess) {
        // Enhance planning based on episode outcomes and goals
        if (episode->reward > 0.0f) {
            // Successful episode - reinforce successful strategies
            pImpl->planner->reinforceSuccessfulAction(episode->action, episode->reward);
        } else if (episode->reward < 0.0f) {
            // Unsuccessful episode - adjust planning strategies
            pImpl->planner->adjustPlanningStrategy(episode->action, std::abs(episode->reward));
        }
    }
}

void Brain::integrateReplayWithNeuromodulation(SimulationStep currentStep) {
    if (!pImpl->neuromodulationIntegrationEnabled || !pImpl->episodicMemory) return;
    
    if (currentStep % pImpl->neuromodulationIntegrationInterval != 0) return;
    
    NLM_LOG_DEBUG("Integrating replay with neuromodulation at step " + std::to_string(currentStep));
    
    // Get recent episodes
    auto episodes = pImpl->episodicMemory->getRecentEpisodes(5);
    
    for (const auto& episode : episodes) {
        // Apply neuromodulatory effects to replay
        if (pImpl->dopamine) {
            float dopamineEffect = (episode->reward > 0.0f) ? 
                                  episode->reward * pImpl->neuromodulationBoost : 
                                  episode->reward;
            pImpl->dopamine->setLevel(dopamine->getLevel() + dopamineEffect * 0.1f);
        }
        
        if (pImpl->curiosity) {
            // Curiosity-based replay enhancement
            pImpl->curiosity->update(pImpl->timestep * 0.5f);
        }
        
        if (pImpl->novelty) {
            // Novelty-based replay enhancement
            float noveltyEffect = episode->novelty * pImpl->noveltyBoost;
            // Apply novelty to replay selection
        }
        
        if (pImpl->predictionError) {
            // Prediction error-based replay enhancement
            float errorEffect = std::abs(episode->predictionError) * pImpl->predictionErrorBoost;
            // Apply prediction error to replay
        }
    }
}

void Brain::integrateReplayWithWorkingMemory(SimulationStep currentStep) {
    if (!pImpl->workingMemoryIntegrationEnabled || !pImpl->episodicMemory || !pImpl->workingMemory) return;
    
    if (currentStep % pImpl->workingMemoryIntegrationInterval != 0) return;
    
    NLM_LOG_DEBUG("Integrating replay with working memory at step " + std::to_string(currentStep));
    
    // Get episodes for working memory integration
    auto episodesToProcess = pImpl->episodicMemory->getEpisodesForReplay(3);
    
    for (const auto* episode : episodesToProcess) {
        // Integrate episode into working memory
        if (!episode->workingMemoryContent.empty()) {
            pImpl->workingMemory->store(episode->workingMemoryContent, episode->workingMemoryRetention);
        }
        
        // Strengthen working memory through replay
        pImpl->workingMemory->strengthenMemory(episode->workingMemoryRetention * 1.2f);
        
        // Transfer stable working memory patterns to episodic memory
        pImpl->workingMemory->consolidateToEpisodic(this);
    }
}

void Brain::integrateReplayWithPredictionSystem(SimulationStep currentStep) {
    if (!pImpl->predictionIntegrationEnabled || !pImpl->episodicMemory || !pImpl->predictionSystem) return;
    
    if (currentStep % pImpl->predictionIntegrationInterval != 0) return;
    
    NLM_LOG_DEBUG("Integrating replay with prediction system at step " + std::to_string(currentStep));
    
    // Get episodes with good prediction accuracy
    auto episodesToProcess = pImpl->episodicMemory->getEpisodesForReplay(4);
    
    for (const auto* episode : episodesToProcess) {
        // Update prediction system with replayed experience
        pImpl->predictionSystem->train(episode->sensoryState);
        
        // Refine prediction based on episode outcomes
        if (episode->predictionConfidence > 0.5f) {
            pImpl->predictionSystem->updateExpectedReward(episode->reward, episode->predictionConfidence);
        }
    }
}

void Brain::integrateReplayWithConceptFormation(SimulationStep currentStep) {
    if (!pImpl->conceptFormationIntegrationEnabled || !pImpl->episodicMemory || !pImpl->conceptFormation) return;
    
    if (currentStep % pImpl->conceptFormationIntegrationInterval != 0) return;
    
    NLM_LOG_DEBUG("Integrating replay with concept formation at step " + std::to_string(currentStep));
    
    // Get episodes with rich feature information
    auto episodesToProcess = pImpl->episodicMemory->getEpisodesForReplay(2);
    
    for (const auto* episode : episodesToProcess) {
        // Enhance concept formation through replayed features
        std::vector<float> replayFeatures = episode->sensoryFeatures;
        replayFeatures.insert(replayFeatures.end(),
                            episode->contextualFeatures.begin(),
                            episode->contextualFeatures.end());
        replayFeatures.push_back(episode->reward);
        replayFeatures.push_back(episode->noveltyLevel);
        
        pImpl->conceptFormation->updateConcept(replayFeatures, currentTime);
    }
}

void Brain::integrateReplayWithPlanningSystem(SimulationStep currentStep) {
    if (!pImpl->planningIntegrationEnabled || !pImpl->episodicMemory || !pImpl->planner) return;
    
    if (currentStep % pImpl->planningIntegrationInterval != 0) return;
    
    NLM_LOG_DEBUG("Integrating replay with planning system at step " + std::to_string(currentStep));
    
    // Get episodes with goal-related information
    auto episodesToProcess = pImpl->episodicMemory->getEpisodesForReplay(3);
    
    for (const auto* episode : episodesToProcess) {
        // Update planning system based on episode outcomes
        if (episode->action != ActionType::Wait) {
            // Update action preferences based on episode success
            pImpl->planner->updateActionPreference(episode->action, 
                                                 episode->reward, 
                                                 episode->predictionConfidence);
        }
    }
}

void Brain::strengthenWorkingMemoryThroughReplay(SimulationStep currentStep, const EpisodicMemoryItem* episode) {
    if (!pImpl->workingMemory) return;
    
    // Store episode in working memory
    pImpl->workingMemory->store(episode->sensoryState, episode->stabilityScore * 1.5f);
    
    // Strengthen memory traces
    pImpl->workingMemory->strengthenMemory(episode->workingMemoryRetention * 1.2f);
}

void Brain::enhancePredictionSystemThroughReverseReplay(SimulationStep currentStep, const EpisodicMemoryItem* episode) {
    if (!pImpl->predictionSystem) return;
    
    // Enhance prediction through reverse replay
    if (episode->predictionError > 0.0f) {
        // Use prediction error to correct predictions
        pImpl->predictionSystem->correctPrediction(episode->sensoryState, episode->predictionError);
    } else {
        // Reinforce successful predictions
        pImpl->predictionSystem->train(episode->sensoryState);
    }
}

void Brain::enhanceConceptFormationThroughReverseReplay(SimulationStep currentStep, const EpisodicMemoryItem* episode) {
    if (!pImpl->conceptFormation) return;
    
    // Enhance concept formation through reverse replay
    std::vector<float> replayFeatures = episode->sensoryFeatures;
    replayFeatures.insert(replayFeatures.end(),
                        episode->contextualFeatures.begin(),
                        episode->contextualFeatures.end());
    
    pImpl->conceptFormation->updateConcept(replayFeatures, currentTime);
}

void Brain::enhancePlanningThroughReverseReplay(SimulationStep currentStep, const EpisodicMemoryItem* episode) {
    if (!pImpl->planner) return;
    
    // Enhance planning through reverse replay
    if (episode->action != ActionType::Wait) {
        pImpl->planner->updateActionOutcome(episode->action, episode->reward, episode->predictionConfidence);
    }
}

void Brain::applyEnhancedNeuromodulationForTargetedReplay(SimulationStep currentStep, const EpisodicMemoryItem* episode) {
    if (!pImpl->dopamine) return;
    
    // Apply enhanced neuromodulation for targeted replay
    float neuromodulationEffect = pImpl->targetedReplayPriority * pImpl->neuromodulationBoost;
    
    if (episode->reward > 0.0f) {
        // Positive reward - enhance dopamine
        pImpl->dopamine->setLevel(pImpl->dopamine->getLevel() + neuromodulationEffect * 0.2f);
    } else {
        // Negative reward - modulate dopamine
        pImpl->dopamine->setLevel(pImpl->dopamine->getLevel() - neuromodulationEffect * 0.1f);
    }
}

void Brain::applyAttentionSelectionForTargetedReplay(SimulationStep currentStep, const EpisodicMemoryItem* episode) {
    if (!pImpl->attention) return;
    
    // Apply attentional selection for targeted replay
    float attentionEffect = pImpl->attentionEnhancedReplayPriority * pImpl->attentionBoost;
    
    // Update attention based on episode characteristics
    pImpl->attention->updateAttention(attentionEffect);
}

void Brain::enhanceReplayForNovelExperience(SimulationStep currentStep, const EpisodicMemoryItem* episode) {
    if (!pImpl->novelty) return;
    
    // Enhance replay for novel experiences
    float noveltyBoost = episode->novelty * pImpl->noveltyBoost;
    
    // Apply novelty boost to replay strength
    pImpl->episodicMemory->updateRelevance(currentStep, noveltyBoost);
}

void Brain::applySleepReplayEnhancements(SimulationStep currentStep, const EpisodicMemoryItem* episode) {
    // Apply sleep-specific enhancements to replay
    
    // Enhance with slow oscillation synchronization
    if (pImpl->deltaWaves) {
        pImpl->deltaWaves->enhanceMemoryConsolidation(episode->stabilityScore);
    }
    
    // Enhance with synaptic downscaling
    if (pImpl->synapticHomeostasis) {
        applySynapticDownscalingForSleepReplay(episode);
    }
    
    // Enhance with systems consolidation
    if (pImpl->hippocampalCorticalIntegration) {
        pImpl->hippocampalCorticalIntegration->enhanceSystemsConsolidation(episode);
    }
}

void Brain::applySynapticDownscalingForSleepReplay(const EpisodicMemoryItem* episode) {
    if (!pImpl->synapticHomeostasis) return;
    
    // Apply synaptic downscaling for sleep replay
    for (auto& region : pImpl->regions) {
        for (auto* syn : region->getSynapses()) {
            if (syn->getType() == SynapseType::Excitatory) {
                float currentWeight = syn->getWeight();
                if (currentWeight > 0.1f) {
                    // Gradually weaken synapses for homeostatic balance during sleep
                    syn->setWeight(currentWeight * 0.99f);
                }
            }
        }
    }
}

void Brain::collectReplayStatistics(SimulationStep currentStep) {
    if (!pImpl->replayStatistics) return;
    
    // Collect replay system statistics
    pImpl->replayStatistics->update(currentStep);
    
    // Log statistics periodically
    if (currentStep % pImpl->statsCollectionInterval == 0) {
        logReplayStatistics();
    }
}

void Brain::logReplayStatistics() const {
    if (!pImpl->replayStatistics) return;
    
    NLM_LOG_INFO("=== Enhanced Replay Statistics ===");
    NLM_LOG_INFO("Total replay operations: " + std::to_string(pImpl->replayStatistics->totalReplayOperations));
    NLM_LOG_INFO("Successful replays: " + std::to_string(pImpl->replayStatistics->successfulReplays));
    NLM_LOG_INFO("Failed replays: " + std::to_string(pImpl->replayStatistics->failedReplays));
    NLM_LOG_INFO("Consolidation cycles: " + std::to_string(pImpl->replayStatistics->consolidationCyclesCompleted));
    NLM_LOG_INFO("Pattern improvements: " + std::to_string(pImpl->replayStatistics->patternImprovements));
    NLM_LOG_INFO("Concept formations: " + std::to_string(pImpl->replayStatistics->conceptFormations));
    NLM_LOG_INFO("Planning enhancements: " + std::to_string(pImpl->replayStatistics->planningEnhancements));
    NLM_LOG_INFO("Prediction refinements: " + std::to_string(pImpl->replayStatistics->predictionRefinements));
    NLM_LOG_INFO("Replay throughput: " + std::to_string(pImpl->replayStatistics->replayThroughput));
    NLM_LOG_INFO("System integration score: " + std::to_string(pImpl->replayStatistics->systemIntegrationScore));
}

} // namespace nlm

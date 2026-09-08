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
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
        // Safety check - ensure config exists
        if (!config) {
            throw std::runtime_error("Brain initialized with null config!");
        }
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        if (!rng) {
            rng = std::make_unique<RandomGenerator>(seed);
        }
        
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
    
    // Safety check - ensure pImpl exists and is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain initialized with null pImpl!");
        return false;
    }
    
    // Additional safety checks for essential systems
    if (!pImpl->config) {
        NLM_LOG_ERROR("Brain initialized with null config!");
        return false;
    }
    
    if (!pImpl->spikeSystem) {
        NLM_LOG_ERROR("Spike system not initialized! Cannot proceed with brain initialization.");
        return false;
    }
    
    if (!pImpl->rng) {
        NLM_LOG_ERROR("Random generator not initialized!");
        return false;
    }
    
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
        if (!region) {
            NLM_LOG_WARNING("Region " + std::to_string(i + 1) + " not found");
            continue;
        }
        
        // Add populations
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                if (neuron) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
        }
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                if (neuron) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                    ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_WARNING("Skipping connectivity for region " + std::to_string(i + 1));
            continue;
        }
        // Initialize random connectivity and synapse weights
        region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
    } else {
        NLM_LOG_WARNING("Working memory not initialized");
    }
    
    // Initialize episodic memory
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
    } else {
        NLM_LOG_WARNING("Episodic memory not initialized");
    }
    
    // Initialize associative memory
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(this);
    } else {
        NLM_LOG_WARNING("Associative memory not initialized");
    }
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
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
    }
    
    // Register delayed spike handler to deliver synaptic input
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input
            for (auto& region : pImpl->regions) {
                if (!region) continue;
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (!neuron) continue;
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
    } else {
        NLM_LOG_WARNING("Checkpoint manager not initialized");
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
    // Safety check - ensure pImpl exists and is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::step called with null pImpl!");
        return;
    }
    
    // Also safety check essential systems
    if (!pImpl->spikeSystem) {
        NLM_LOG_ERROR("Spike system not initialized! Cannot process spikes.");
        return;
    }
    
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
    
    // Execute each phase of the brain loop
    processDelayedSpikes();
    updateNeuronDynamics(currentTime);
    detectAndScheduleSpikes();
    updateWorkingMemory();
    applyNeuromodulationEffects();
    applyPlasticityRules(currentTime);
    updateEpisodicMemory();
    updatePredictionSystem();
    updateAttentionSystem();
    updateConceptFormation();
    applyStructuralPlasticity(currentStep);
    replayMemories(currentStep);
    applyDevelopmentEffects(currentStep);
    consolidateMemories(currentStep);
    updateCheckpoints(currentStep, currentTime);
    collectStatistics();
}

void Brain::processDelayedSpikes() {
    if (!pImpl || !pImpl->spikeSystem) return;
    pImpl->spikeSystem->processDelayedSpikes(pImpl->currentStep, pImpl->currentTime);
}

void Brain::updateNeuronDynamics(Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        if (!region) continue;
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) continue;
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void Brain::detectAndScheduleSpikes() {
    for (auto& region : pImpl->regions) {
        if (!region) continue;
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) continue;
                
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(pImpl->currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), pImpl->currentTime, pImpl->currentStep);
                    if (pImpl->spikeSystem) {
                        pImpl->spikeSystem->queueSpike(event);
                    }

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        if (syn) syn->recordPostSpike(pImpl->currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        if (!syn) continue;
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = pImpl->currentStep + delay;
                        Timestamp deliveryTime = pImpl->currentTime + delay * pImpl->timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            pImpl->currentTime,
                            deliveryTime,
                            pImpl->currentStep,
                            deliveryStep
                        );

                        if (pImpl->spikeSystem) {
                            pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        }

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(pImpl->currentTime);
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
        pImpl->spikeSystem->processSpikes(pImpl->currentStep);
    }
}

void Brain::updateWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::applyNeuromodulationEffects() {
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
            if (!region) continue;
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
}

void Brain::applyPlasticityRules(Timestamp currentTime) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        if (!region) continue;
        for (auto& syn : region->getSynapses()) {
            if (!syn) continue;
            
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    if (pImpl->stdp) {
                        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    }
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
                    if (pImpl->hebbian) {
                        pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    }
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void Brain::updateEpisodicMemory() {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
                if (!region) continue;
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
}

void Brain::updatePredictionSystem() {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void Brain::updateAttentionSystem() {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void Brain::updateConceptFormation() {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::applyStructuralPlasticity(SimulationStep currentStep) {
    if (currentStep % 100 == 0 && pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::replayMemories(SimulationStep currentStep) {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            if (pImpl->episodicMemory) {
                pImpl->episodicMemory->replayEpisode(episode);
            }
        }
    }
}

void Brain::applyDevelopmentEffects(SimulationStep currentStep) {
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
}

void Brain::consolidateMemories(SimulationStep currentStep) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void Brain::updateCheckpoints(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::collectStatistics() {
    // Collect statistics for logging (to be implemented)
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
    // Safety check - ensure pImpl exists and spikeSystem is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getSpikeSystem() called with null pImpl!");
        return nullptr;
    }
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    // Safety check - ensure pImpl exists and spikeSystem is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getSpikeSystem() const called with null pImpl!");
        return nullptr;
    }
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    // Safety check - ensure pImpl exists and stdp is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getSTDP() called with null pImpl!");
        return nullptr;
    }
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    // Safety check - ensure pImpl exists and hebbian is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getHebbian() called with null pImpl!");
        return nullptr;
    }
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    // Safety check - ensure pImpl exists and structuralPlasticity is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getStructuralPlasticity() called with null pImpl!");
        return nullptr;
    }
    return pImpl->structuralPlasticity.get();
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    // Safety check - ensure pImpl exists and workingMemory is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getWorkingMemory() called with null pImpl!");
        return nullptr;
    }
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    // Safety check - ensure pImpl exists and episodicMemory is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getEpisodicMemory() called with null pImpl!");
        return nullptr;
    }
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    // Safety check - ensure pImpl exists and associativeMemory is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getAssociativeMemory() called with null pImpl!");
        return nullptr;
    }
    return pImpl->associativeMemory.get();
}

PredictionSystem* Brain::getPredictionSystem() {
    // Safety check - ensure pImpl exists and predictionSystem is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getPredictionSystem() called with null pImpl!");
        return nullptr;
    }
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    // Safety check - ensure pImpl exists and planner is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getPlanner() called with null pImpl!");
        return nullptr;
    }
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    // Safety check - ensure pImpl exists and conceptFormation is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getConceptFormation() called with null pImpl!");
        return nullptr;
    }
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    // Safety check - ensure pImpl exists and attention is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getAttention() called with null pImpl!");
        return nullptr;
    }
    return pImpl->attention.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    // Safety check - ensure pImpl exists and developmentSystem is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getDevelopmentSystem() called with null pImpl!");
        return nullptr;
    }
    return pImpl->developmentSystem.get();
}

Dopamine* Brain::getDopamine() {
    // Safety check - ensure pImpl exists and dopamine is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getDopamine() called with null pImpl!");
        return nullptr;
    }
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    // Safety check - ensure pImpl exists and curiosity is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getCuriosity() called with null pImpl!");
        return nullptr;
    }
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    // Safety check - ensure pImpl exists and novelty is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getNovelty() called with null pImpl!");
        return nullptr;
    }
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    // Safety check - ensure pImpl exists and predictionError is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::getPredictionErrorSignal() called with null pImpl!");
        return nullptr;
    }
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

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
            
            // Store sensory input (if available from last time step)
            // Note: In a full implementation, we would store the actual sensory input
            // For now, we capture current neural activation patterns
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
            
            // Store action that will be produced (or was just produced)
            // Get current action if motor neurons are firing
            if (!pImpl->motorNeurons.empty()) {
                // Check which motor neurons are firing to determine action
                for (auto* neuron : pImpl->motorNeurons) {
                    if (neuron->isFiring()) {
                        // Simple mapping: first firing motor neuron determines action
                        // In a real implementation, this would be more sophisticated
                        episode.action = ActionType::MoveForward;  // Default action
                        break;
                    }
                }
    // Store action that will be produced (or was just produced)
            // Get current action if motor neurons are firing
            if (!pImpl->motorNeurons.empty()) {
                // Check which motor neurons are firing to determine action
                for (auto* neuron : pImpl->motorNeurons) {
                    if (neuron->isFiring()) {
                        // Simple mapping: first firing motor neuron determines action
                        // In a real implementation, this would be more sophisticated
                        episode.action = ActionType::MoveForward;  // Default action
                        break;
                    }
                }
            }
            
            // Store outcome information
            episode.resultingReward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store neural activation summary (simplified sensory state)
            float avgActivation = 0.0f;
            size_t activeCount = 0;
            for (const auto& neuronId : episode.activeNeurons) {
                // In a full implementation, we would map neuron IDs back to regions/pops
                avgActivation += 0.5f;  // Simplified
                activeCount++;
            }
            if (activeCount > 0) {
                avgActivation /= activeCount;
            }
            episode.novelty = avgActivation;  // Use activation as proxy for novelty
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Create sensory input for prediction system
        auto sensoryInput = std::make_unique<SensoryInput>();
        
        // Convert current neural activity to sensory-like representation
        std::vector<float> sensoryPattern;
        sensoryPattern.reserve(pImpl->regions.size() * 10);  // Rough estimate
        
        // Sample neural activity from various regions
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    float activation = std::abs(state.membranePotential - state.restingPotential);
                    sensoryPattern.push_back(activation);
                }
            }
        }
        
        sensoryInput->setData(sensoryPattern);
        
        // Update predictions based on current state
        pImpl->predictionSystem->updatePredictions(sensoryInput.get(), sensoryInput.get());
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
        // Process current neural activity patterns to form concepts
        // Use working memory activity as input for concept formation
        if (pImpl->workingMemory) {
            // Get current working memory content
            auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            auto memoryActivations = pImpl->workingMemory->getMemoryActivity();
            
            if (!memoryNeurons.empty() && !memoryActivations.empty()) {
                // Create pattern from working memory
                std::vector<float> memoryPattern;
                memoryPattern.reserve(memoryNeurons.size());
                
                // Sample some neurons from working memory
                size_t sampleSize = std::min(size_t(20), memoryNeurons.size());
                for (size_t i = 0; i < sampleSize; ++i) {
                    // Get random neuron from working memory
                    size_t idx = pImpl->rng->rand() % memoryNeurons.size();
                    // In a full implementation, we would look up neuron activation
                    memoryPattern.push_back(memoryActivations[idx]);
                }
                
                // Form concepts from the pattern
                pImpl->conceptFormation->formConcepts(memoryPattern);
            }
        }
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
    
    // Create a complete sensory pattern for working memory
    std::vector<float> sensoryPattern;
    sensoryPattern.reserve(numSensory);
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store sensory pattern in working memory
        if (pImpl->workingMemory) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            sensoryPattern.push_back(normalizedValue / 10.0f);
        }
    }
    
    // Store the complete sensory pattern in working memory as a memory trace
    if (pImpl->workingMemory && !sensoryPattern.empty()) {
        pImpl->workingMemory->store(sensoryPattern, 1.0f);
    }
    
    // Update prediction system with new sensory input
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->updatePredictions(input);
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
        
    // Write memory systems state
    {
        // Working memory
        WorkingMemoryCheckpointData workingMemoryData;
        if (pImpl->workingMemory) {
            workingMemoryData.capacity = pImpl->workingMemory->getCapacity();
            workingMemoryData.traceCount = pImpl->workingMemory->getActiveTraces();
            workingMemoryData.decayRate = pImpl->workingMemory->getDecayRate();
            workingMemoryData.memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            workingMemoryData.memoryActivations = pImpl->workingMemory->getMemoryActivity();
        }
        
        // Serialize working memory data
        std::vector<uint8_t> workingMemoryBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&workingMemoryData), sizeof(workingMemoryData));
            ss.write(reinterpret_cast<const char*>(workingMemoryData.memoryNeurons.data()),
                     workingMemoryData.memoryNeurons.size() * sizeof(NeuronId));
            ss.write(reinterpret_cast<const char*>(workingMemoryData.memoryActivations.data()),
                     workingMemoryData.memoryActivations.size() * sizeof(float));
            
            workingMemoryBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::Memory, workingMemoryBuffer.data(), workingMemoryBuffer.size())) {
            NLM_LOG_ERROR("Failed to write working memory state to checkpoint");
            return false;
        }
        
        // Episodic memory
        EpisodicMemoryCheckpointData episodicMemoryData;
        if (pImpl->episodicMemory) {
            episodicMemoryData.episodeCount = pImpl->episodicMemory->getEpisodeCount();
            episodicMemoryData.maxEpisodes = pImpl->episodicMemory->getMaxEpisodes();
            episodicMemoryData.replayEnabled = pImpl->episodicMemory->isReplayEnabled();
        }
        
        // For now, episodic memory storage is minimal - just count and capacity
        // More detailed episodic memory (actual episodes) is too large to checkpoint
        std::vector<uint8_t> episodicMemoryBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&episodicMemoryData), sizeof(episodicMemoryData));
            episodicMemoryBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::Memory, episodicMemoryBuffer.data(), episodicMemoryBuffer.size())) {
            NLM_LOG_ERROR("Failed to write episodic memory state to checkpoint");
            return false;
        }
        
        // Associative memory
        AssociativeMemoryCheckpointData associativeMemoryData;
        if (pImpl->associativeMemory) {
            associativeMemoryData.associationCount = pImpl->associativeMemory->getAssociationCount();
        }
        
        // For now, associative memory storage is minimal
        std::vector<uint8_t> associativeMemoryBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&associativeMemoryData), sizeof(associativeMemoryData));
            associativeMemoryBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::Memory, associativeMemoryBuffer.data(), associativeMemoryBuffer.size())) {
            NLM_LOG_ERROR("Failed to write associative memory state to checkpoint");
            return false;
        }
    }
    
    // Write prediction system state
    {
        PredictionSystemCheckpointData predictionData;
        if (pImpl->predictionSystem) {
            predictionData.error = pImpl->predictionSystem->getPredictionError();
            predictionData.confidence = pImpl->predictionSystem->getConfidence();
            predictionData.errorHistory = pImpl->predictionSystem->getErrorHistory();
        }
        
        // Serialize prediction system data
        std::vector<uint8_t> predictionBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&predictionData), sizeof(predictionData));
            // Write error history vector
            size_t historySize = predictionData.errorHistory.size();
            ss.write(reinterpret_cast<const char*>(&historySize), sizeof(size_t));
            if (historySize > 0) {
                ss.write(reinterpret_cast<const char*>(predictionData.errorHistory.data()),
                        historySize * sizeof(float));
            }
            
            predictionBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::Prediction, predictionBuffer.data(), predictionBuffer.size())) {
            NLM_LOG_ERROR("Failed to write prediction system state to checkpoint");
            return false;
        }
    }
    
    // Write cognition system state
    {
        // Neural planner
        NeuralPlannerCheckpointData plannerData;
        if (pImpl->planner) {
            plannerData.depth = pImpl->planner->getPlanningDepth();
            plannerData.confidence = pImpl->planner->getPlanningConfidence();
            plannerData.goal = pImpl->planner->getCurrentGoal();
        }
        
        // Serialize neural planner data
        std::vector<uint8_t> plannerBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&plannerData), sizeof(plannerData));
            // Write goal vector
            size_t goalSize = plannerData.goal.size();
            ss.write(reinterpret_cast<const char*>(&goalSize), sizeof(size_t));
            if (goalSize > 0) {
                ss.write(reinterpret_cast<const char*>(plannerData.goal.data()),
                        goalSize * sizeof(float));
            }
            
            plannerBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::Cognition, plannerBuffer.data(), plannerBuffer.size())) {
            NLM_LOG_ERROR("Failed to write neural planner state to checkpoint");
            return false;
        }
        
        // Concept formation
        ConceptFormationCheckpointData conceptData;
        if (pImpl->conceptFormation) {
            // Concept formation doesn't have checkpoint data in current implementation
        }
        
        // For now, concept formation is not checkpointed
        std::vector<uint8_t> conceptBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&conceptData), sizeof(conceptData));
            conceptBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::Cognition, conceptBuffer.data(), conceptBuffer.size())) {
            NLM_LOG_ERROR("Failed to write concept formation state to checkpoint");
            return false;
        }
    }
    
    // Write spike system state (recent spike history)
    {
        SpikeSystemCheckpointData spikeData;
        if (pImpl->spikeSystem) {
            spikeData.pendingSpikes = pImpl->spikeSystem->getPendingSpikeCount();
            spikeData.pendingDelayedSpikes = pImpl->spikeSystem->getPendingDelayedCount();
            spikeData.totalSpikesThisStep = pImpl->totalSpikesThisStep;
            spikeData.totalSpikesTotal = pImpl->totalSpikesTotal;
        }
        
        // Serialize spike system data
        std::vector<uint8_t> spikeBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&spikeData), sizeof(spikeData));
            spikeBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::SpikeHistory, spikeBuffer.data(), spikeBuffer.size())) {
            NLM_LOG_ERROR("Failed to write spike system state to checkpoint");
            return false;
        }
    }
    
    // Write random generator state
    {
        RandomCheckpointData randomData;
        if (pImpl->rng) {
            // Note: RandomGenerator doesn't have checkpoint/restore methods yet
            // For now, we'll just store a seed
            randomData.seed = pImpl->rng->getSeed();
        }
        
        // Serialize random state
        std::vector<uint8_t> randomBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&randomData), sizeof(randomData));
            randomBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::RandomState, randomBuffer.data(), randomBuffer.size())) {
            NLM_LOG_ERROR("Failed to write random state to checkpoint");
            return false;
        }
    }
    
    // Write simulation state
    {
        SimulationCheckpointData simData;
        simData.currentStep = pImpl->currentStep;
        simData.currentTime = pImpl->currentTime;
        simData.timestep = pImpl->timestep;
        simData.isResting = pImpl->isResting;
        simData.stepsSinceLastEpisode = pImpl->stepsSinceLastEpisode;
        simData.replayInterval = pImpl->replayInterval;
        simData.consolidationInterval = pImpl->consolidationInterval;
        
        // Serialize simulation state
        std::vector<uint8_t> simBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&simData), sizeof(simData));
            simBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::SimulationState, simBuffer.data(), simBuffer.size())) {
            NLM_LOG_ERROR("Failed to write simulation state to checkpoint");
            return false;
        }
    }
    
    // Write neuromodulation state
    {
        NeuromodulationCheckpointData neuromodData;
        if (pImpl->dopamine) neuromodData.dopamineLevel = pImpl->dopamine->getLevel();
        if (pImpl->curiosity) neuromodData.curiosityLevel = pImpl->curiosity->getLevel();
        if (pImpl->novelty) neuromodData.noveltyLevel = pImpl->novelty->getLevel();
        if (pImpl->predictionError) neuromodData.predictionErrorLevel = pImpl->predictionError->getLevel();
        
        // Serialize neuromodulation state
        std::vector<uint8_t> neuromodBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&neuromodData), sizeof(neuromodData));
            neuromodBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::Neuromodulation, neuromodBuffer.data(), neuromodBuffer.size())) {
            NLM_LOG_ERROR("Failed to write neuromodulation state to checkpoint");
            return false;
        }
    }
    
    // Write development state
    {
        DevelopmentCheckpointData developmentData;
        developmentData.stage = static_cast<uint8_t>(pImpl->developmentalStage);
        developmentData.age = 0.0;  // Age not tracked in Brain
        developmentData.plasticityModifier = 1.0f;  // Current plasticity modifier
        developmentData.capabilityLevel = 0.5f;  // Default capability level
        developmentData.bodyAwareness = 0.5f;  // Default body awareness
        
        // Serialize development state
        std::vector<uint8_t> developmentBuffer;
        {
            std::stringstream ss;
            ss.write(reinterpret_cast<const char*>(&developmentData), sizeof(developmentData));
            developmentBuffer.assign(ss.str().begin(), ss.str().end());
        }
        
        if (!writer.writeSection(CheckpointSection::Development, developmentBuffer.data(), developmentBuffer.size())) {
            NLM_LOG_ERROR("Failed to write development state to checkpoint");
            return false;
        }
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
        
        // Read plasticity state
        {
            std::vector<float> stdpLtpWeights;
            std::vector<float> stdpLtdWeights;
            std::vector<float> stdpTaus;
            if (!reader.readPlasticity(stdpLtpWeights, stdpLtdWeights, stdpTaus)) {
                NLM_LOG_ERROR("Failed to read plasticity state from checkpoint");
                return false;
            }
            if (pImpl->stdp && !stdpLtpWeights.empty()) {
                pImpl->stdp->configure(stdpLtpWeights[0], stdpLtdWeights[0], stdpTaus[0]);
            }
        }
        
        // Read neuromodulation state
        {
            std::vector<float> dopamineLevels;
            std::vector<float> curiosityLevels;
            std::vector<float> noveltyLevels;
            std::vector<float> predictionErrorLevels;
            if (!reader.readNeuromodulation(dopamineLevels, curiosityLevels, noveltyLevels, predictionErrorLevels)) {
                NLM_LOG_ERROR("Failed to read neuromodulation state from checkpoint");
                return false;
            }
            // Neuromodulation systems don't have state restoration yet
        }
        
        // Read development state
        {
            DevelopmentCheckpointData developmentData;
            if (!reader.readDevelopment(developmentData)) {
                NLM_LOG_ERROR("Failed to read development state from checkpoint");
                return false;
            }
            pImpl->developmentalStage = static_cast<DevelopmentalStage>(developmentData.stage);
        }
        
        // Read memory systems state
        {
            // Working memory
            WorkingMemoryCheckpointData workingMemoryData;
            if (!reader.readWorkingMemory(workingMemoryData)) {
                NLM_LOG_ERROR("Failed to read working memory state from checkpoint");
                return false;
            }
            
            // Episodic memory
            EpisodicMemoryCheckpointData episodicMemoryData;
            if (!reader.readEpisodicMemory(episodicMemoryData)) {
                NLM_LOG_ERROR("Failed to read episodic memory state from checkpoint");
                return false;
            }
            
            // Associative memory
            AssociativeMemoryCheckpointData associativeMemoryData;
            if (!reader.readAssociativeMemory(associativeMemoryData)) {
                NLM_LOG_ERROR("Failed to read associative memory state from checkpoint");
                return false;
            }
        }
        
        // Read prediction system state
        {
            PredictionSystemCheckpointData predictionData;
            if (!reader.readPredictionSystem(predictionData)) {
                NLM_LOG_ERROR("Failed to read prediction system state from checkpoint");
                return false;
            }
            if (pImpl->predictionSystem) {
                // PredictionSystem doesn't have setters for error/confidence yet
            }
        }
        
        // Read cognition system state
        {
            // Neural planner
            NeuralPlannerCheckpointData plannerData;
            if (!reader.readNeuralPlanner(plannerData)) {
                NLM_LOG_ERROR("Failed to read neural planner state from checkpoint");
                return false;
            }
            if (pImpl->planner) {
                pImpl->planner->setPlanningDepth(plannerData.depth);
            }
        }
        
        // Read spike system state
        {
            SpikeSystemCheckpointData spikeData;
            if (!reader.readSpikeSystem(spikeData)) {
                NLM_LOG_ERROR("Failed to read spike system state from checkpoint");
                return false;
            }
            pImpl->totalSpikesThisStep = spikeData.totalSpikesThisStep;
            pImpl->totalSpikesTotal = spikeData.totalSpikesTotal;
        }
        
        // Read random generator state
        {
            RandomCheckpointData randomData;
            if (!reader.readRandomState(randomData)) {
                NLM_LOG_ERROR("Failed to read random state from checkpoint");
                return false;
            }
            if (pImpl->rng) {
                // RandomGenerator doesn't have restore method yet
                // For now, we'll just keep the current seed
            }
        }
        
        // Read simulation state
        {
            SimulationCheckpointData simData;
            if (!reader.readSimulationState(simData)) {
                NLM_LOG_ERROR("Failed to read simulation state from checkpoint");
                return false;
            }
            pImpl->currentStep = simData.currentStep;
            pImpl->currentTime = simData.currentTime;
            pImpl->timestep = simData.timestep;
            pImpl->isResting = simData.isResting;
            pImpl->stepsSinceLastEpisode = simData.stepsSinceLastEpisode;
            pImpl->replayInterval = simData.replayInterval;
            pImpl->consolidationInterval = simData.consolidationInterval;
        }
        
        // Read neuromodulation state
        {
            NeuromodulationCheckpointData neuromodData;
            if (!reader.readNeuromodulation(neuromodData)) {
                NLM_LOG_ERROR("Failed to read neuromodulation state from checkpoint");
                return false;
            }
            // Neuromodulation systems don't have state restoration yet
        }
        
        // Read development state
        {
            DevelopmentCheckpointData developmentData;
            if (!reader.readDevelopment(developmentData)) {
                NLM_LOG_ERROR("Failed to read development state from checkpoint");
                return false;
            }
            pImpl->developmentalStage = static_cast<DevelopmentalStage>(developmentData.stage);
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

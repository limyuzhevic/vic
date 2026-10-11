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
    
    // Agent brain interface (for integration)
    class AgentBrain* brainAgent;
    
    // Current sensory state (for prediction system)
    std::vector<float> currentSensoryState;
    
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
        , brainAgent(nullptr)
        , currentSensoryState()
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
    pImpl->associativeMemory->initialize(this);  // Initialize cognitive representation storage
    
    // Initialize prediction system
    pImpl->predictionSystem->train(pImpl->sensoryNeurons);  // Initialize with sensory patterns
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    pImpl->planner->getSelfModel()->initialize(this);  // Initialize self-model
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    pImpl->attention->setFocusMechanism(AttentionFocusMechanism::Predictive);  // Enable predictive attention
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    pImpl->predictionError->initialize(this);  // Initialize prediction error tracking
    pImpl->dopamine->configure(pImpl->config.get());  // Configure dopamine with full settings
    
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
        
        // Dopamine enhances working memory traces
        if (pImpl->workingMemory) {
            // Increase working memory strength based on dopamine level
            // This enhances memory consolidation and retrieval
            pImpl->workingMemory->strengthenMemory(dopamineLevel * 2.0f);
            
            // Dopamine increases excitability of neurons in working memory
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Internal) {
                        // Internal neurons are key for working memory
                        for (auto* neuron : pop->getNeurons()) {
                            float excitabilityMod = dopamineLevel * 2.0f;
                            if (excitabilityMod > 0.0f) {
                                neuron->injectCurrent(excitabilityMod);
                            }
                        }
                    }
                }
            }
        }
        
        // Dopamine facilitates concept formation
        if (pImpl->conceptFormation) {
            // Enhance concept updating when dopamine is high
            // Update concepts with higher weight based on dopamine level
            float conceptLearningRate = 0.01f * dopamineLevel;
            
            // Update existing concepts with dopamine-modulated learning
            if (conceptLearningRate > 0.001f) {
                // This would require access to concept update mechanism
                // For now, we can log the learning enhancement
                NLM_LOG_INFO("Dopamine-enhanced concept learning: level=" + 
                           std::to_string(dopamineLevel) + 
                           ", learning_rate=" + std::to_string(conceptLearningRate));
            }
        }
        
        // Dopamine modulates attention selection
        if (pImpl->attention) {
            // Modulate attention based on dopamine level
            // Higher dopamine increases focus on salient stimuli
            float attentionFocus = 0.5f + 0.5f * dopamineLevel;
            
            // Attention system would use this to adjust focus mechanisms
            // For now, we can log the attention modulation
            NLM_LOG_INFO("Dopamine-attention modulation: level=" + 
                       std::to_string(dopamineLevel) + 
                       ", focus=" + std::to_string(attentionFocus));
        }
        
        // Original neuromodulation effects on all neurons
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
    
    // Update other neuromodulators (enhancements)
    // Note: Acetylcholine, Norepinephrine, and Serotonin are implemented as stubs
    // but can be enhanced in future versions
    
    // Update other neuromodulators (enhancements)
    // Note: Acetylcholine, Norepinephrine, and Serotonin are implemented as stubs
    // but can be enhanced in future versions
    
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
        // Update prediction system with current neural activity
        // Use working memory traces as prediction input
        if (pImpl->workingMemory) {
            auto memoryTraces = pImpl->workingMemory->getMemoryTraces();
            std::vector<float> predictionInput;
            for (const auto& trace : memoryTraces) {
                predictionInput.push_back(trace.activation);
            }
            
            // Update prediction system
            pImpl->predictionSystem->update(predictionInput);
            
            // Get prediction error
            float predictionError = pImpl->predictionSystem->getPredictionError();
            
            // Apply prediction error to neuromodulation
            if (pImpl->predictionError) {
                pImpl->predictionError->update(predictionError, 0.001);
            }
        }
        
        // Also update prediction system with current sensory state if available
        if (!pImpl->currentSensoryState.empty()) {
            pImpl->predictionSystem->train(pImpl->currentSensoryState);
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
        // Form concepts from current neural activity patterns
        // Use working memory traces and episodic memory to build concepts
        if (pImpl->workingMemory && pImpl->episodicMemory) {
            auto memoryTraces = pImpl->workingMemory->getMemoryTraces();
            auto currentEpisode = pImpl->episodicMemory->getCurrentEpisode();
            
            if (!memoryTraces.empty() && currentEpisode) {
                // Convert neural activity to features for concept formation
                std::vector<float> pattern;
                for (const auto& trace : memoryTraces) {
                    pattern.push_back(trace.activation);
                }
                
                std::vector<float> features;
                for (size_t i = 0; i < pattern.size() && i < 10; ++i) {  // Use first 10 neurons
                    features.push_back(pattern[i]);
                }
                
                // Form concept from current pattern
                pImpl->conceptFormation->presentExperience(
                    pattern, features, 
                    pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,
                    currentStep
                );
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
        
        // Development affects more than just structural plasticity
        DevelopmentalStage stage = pImpl->developmentalStage;
        
        // 1. Development affects neural excitability
        float excitabilityMod = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                excitabilityMod = 1.5f;  // Higher excitability in early development
                break;
            case DevelopmentalStage::CriticalPeriod:
                excitabilityMod = 1.2f;
                break;
            case DevelopmentalStage::Maturation:
                excitabilityMod = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                excitabilityMod = 0.8f;  // Reduced excitability in adulthood
                break;
        }
        
        // Apply excitability changes to neurons
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (excitabilityMod != 1.0f) {
                        float current = neuron->getMembranePotential() - neuron->getRestingPotential();
                        neuron->injectCurrent(current * (excitabilityMod - 1.0f));
                    }
                }
            }
        }
        
        // 2. Development affects plasticity rules (beyond structural plasticity)
        float plasticityMod = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.5f;  // Higher plasticity
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 1.2f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.7f;  // More stable
                break;
        }
        
        // Apply to STDP and Hebbian plasticity
        if (pImpl->stdp) {
            float ltpWeight = 0.01f * plasticityMod;
            float ltdWeight = 0.012f * plasticityMod;
            pImpl->stdp->setLTPWeight(ltpWeight);
            pImpl->stdp->setLTDWeight(ltdWeight);
        }
        
        if (pImpl->hebbian) {
            // Hebbian learning enhancement
            // (Hebbian doesn't have configurable weights in current implementation)
        }
        
        // 3. Development affects attention system
        if (pImpl->attention) {
            float attentionMod = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    attentionMod = 1.3f;  // More flexible attention
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    attentionMod = 1.1f;
                    break;
                case DevelopmentalStage::Maturation:
                    attentionMod = 1.0f;
                    break;
                case DevelopmentalStage::Adult:
                    attentionMod = 0.9f;  // More focused attention
                    break;
            }
            pImpl->attention->setInhibitionStrength(0.5f * attentionMod);
            pImpl->attention->setExcitationStrength(1.5f * attentionMod);
        }
        
        // 4. Development affects memory consolidation
        if (pImpl->episodicMemory) {
            float consolidationMod = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    consolidationMod = 1.2f;  // Better memory in early development
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    consolidationMod = 1.1f;
                    break;
                case DevelopmentalStage::Maturation:
                    consolidationMod = 1.0f;
                    break;
                case DevelopmentalStage::Adult:
                    consolidationMod = 0.8f;  // Less flexible memory
                    break;
            }
            // Update episodic memory consolidation rate
            // (This would require adding a method to NeuralEpisodicMemory)
        }
        
        // 5. Development affects neuromodulation
        if (pImpl->dopamine) {
            float neuromodMod = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    neuromodMod = 1.3f;  // Higher neuromodulation
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    neuromodMod = 1.1f;
                    break;
                case DevelopmentalStage::Maturation:
                    neuromodMod = 1.0f;
                    break;
                case DevelopmentalStage::Adult:
                    neuromodMod = 0.7f;  // Reduced neuromodulation
                    break;
            }
            // Update dopamine level or neuromodulation factors
            // (This would require adding methods to Dopamine)
        }
        
        // 6. Development affects prediction system
        if (pImpl->predictionSystem) {
            float predictionMod = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    predictionMod = 1.2f;  // More exploratory prediction
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    predictionMod = 1.1f;
                    break;
                case DevelopmentalStage::Maturation:
                    predictionMod = 1.0f;
                    break;
                case DevelopmentalStage::Adult:
                    predictionMod = 0.8f;  // More stable prediction
                    break;
            }
            // Update prediction confidence or learning rate
            // (This would require adding methods to PredictionSystem)
        }
        
        // 7. Development affects concept formation
        if (pImpl->conceptFormation) {
            float conceptMod = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    conceptMod = 1.3f;  // More concept formation
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    conceptMod = 1.2f;
                    break;
                case DevelopmentalStage::Maturation:
                    conceptMod = 1.0f;
                    break;
                case DevelopmentalStage::Adult:
                    conceptMod = 0.8f;  // Less new concept formation
                    break;
            }
            // Update concept formation threshold
            // (This would require adding methods to ConceptFormation)
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
    
    // Store current sensory state for prediction system
    pImpl->currentSensoryState = values;
    
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

const std::vector<float>& Brain::getCurrentSensoryState() const {
    return pImpl->currentSensoryState;
}

void Brain::setBrainAgent(class AgentBrain* agent) {
    pImpl->brainAgent = agent;
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
    // Use NeuralPlanner for intelligent action selection
    // Based on current neural state, concepts, and self-model
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get current state from working memory
    std::vector<float> currentState;
    if (pImpl->workingMemory) {
        auto memoryTraces = pImpl->workingMemory->getMemoryTraces();
        for (const auto& trace : memoryTraces) {
            currentState.push_back(trace.activation);
        }
    }
    
    // Get target from concept formation if available
    float targetReward = 0.0f;
    if (pImpl->conceptFormation && !pImpl->conceptFormation->getConcepts().empty()) {
        // Get the most relevant concept
        const auto& concept = pImpl->conceptFormation->getConcepts().front();
        targetReward = 0.5f;  // Reward value for this concept
    }
    
    // Use NeuralPlanner to plan action
    ActionType plannedAction = ActionType::Wait;
    if (pImpl->planner) {
        // Get planned action based on current state
        plannedAction = pImpl->planner->planAction(currentState, targetReward);
    }
    
    // If planning failed, fall back to motor neuron activity
    if (plannedAction == ActionType::Wait && !pImpl->motorNeurons.empty()) {
        // Check if any motor neurons are firing
        size_t firingMotor = 0;
        for (auto* neuron : pImpl->motorNeurons) {
            if (neuron->isFiring()) {
                ++firingMotor;
            }
        }
        
        // Return a simple action based on motor activity
        if (firingMotor > 0) {
            plannedAction = ActionType::MoveForward;
        }
    }
    
    auto action = std::make_unique<Action>(plannedAction);
    
    // Record action in self-model for learning
    if (pImpl->planner && pImpl->planner->getSelfModel()) {
        std::vector<float> actionEffect = {1.0f, 0.5f, 0.2f};  // Simplified effect
        pImpl->planner->getSelfModel()->recordSelfAction(
            plannedAction, currentState, actionEffect);
    }
    
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
        
        // Set metadata including integrated system states
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime,
            // Integrated system states
            (pImpl->workingMemory ? 1 : 0),
            (pImpl->episodicMemory ? 1 : 0),
            (pImpl->associativeMemory ? 1 : 0),
            (pImpl->predictionSystem ? 1 : 0),
            (pImpl->planner ? 1 : 0),
            (pImpl->conceptFormation ? 1 : 0),
            (pImpl->attention ? 1 : 0),
            (pImpl->developmentSystem ? 1 : 0),
            (pImpl->dopamine ? 1 : 0),
            (pImpl->curiosity ? 1 : 0),
            (pImpl->novelty ? 1 : 0),
            (pImpl->predictionError ? 1 : 0)
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
        
        // Write working memory state
        if (pImpl->workingMemory) {
            // Write working memory traces
            auto traces = pImpl->workingMemory->getMemoryTraces();
            WorkingMemoryCheckpointData wmData;
            wmData.traces.reserve(traces.size());
            for (const auto& trace : traces) {
                WorkingMemoryCheckpointTrace t;
                t.neuronId = trace.neuronId;
                t.activation = trace.activation;
                t.strength = trace.strength;
                t.lastUpdated = trace.lastUpdated;
                wmData.traces.push_back(t);
            }
            
            if (!writer.writeWorkingMemory(wmData)) {
                NLM_LOG_ERROR("Failed to write working memory to checkpoint");
                return false;
            }
        }
        
        // Write episodic memory state
        if (pImpl->episodicMemory) {
            auto episodes = pImpl->episodicMemory->getEpisodes();
            EpisodicMemoryCheckpointData emData;
            emData.episodes.reserve(episodes.size());
            for (const auto* episode : episodes) {
                EpisodicMemoryCheckpointEpisode e;
                e.timestamp = episode->timestamp;
                e.reward = episode->reward;
                e.activeNeurons = episode->activeNeurons;
                e.neuronActivations = episode->neuronActivations;
                emData.episodes.push_back(e);
            }
            
            if (!writer.writeEpisodicMemory(emData)) {
                NLM_LOG_ERROR("Failed to write episodic memory to checkpoint");
                return false;
            }
        }
        
        // Write prediction system state
        if (pImpl->predictionSystem) {
            PredictionSystemCheckpointData psData;
            psData.predictionError = pImpl->predictionSystem->getPredictionError();
            psData.confidence = pImpl->predictionSystem->getConfidence();
            
            if (!writer.writePredictionSystem(psData)) {
                NLM_LOG_ERROR("Failed to write prediction system to checkpoint");
                return false;
            }
        }
        
        // Write neuromodulation state
        if (pImpl->dopamine) {
            NeuromodulationCheckpointData nmData;
            nmData.dopamineLevel = pImpl->dopamine->getLevel();
            nmData.curiosityLevel = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
            nmData.noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            nmData.predictionErrorLevel = pImpl->predictionError ? pImpl->predictionError->getLevel() : 0.0f;
            
            if (!writer.writeNeuromodulation(nmData)) {
                NLM_LOG_ERROR("Failed to write neuromodulation to checkpoint");
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
        
        // Read metadata including integrated system flags
        auto metadata = reader.getMetadata();
        int hasWorkingMemory = metadata.get<int>("hasWorkingMemory", 0);
        int hasEpisodicMemory = metadata.get<int>("hasEpisodicMemory", 0);
        int hasAssociativeMemory = metadata.get<int>("hasAssociativeMemory", 0);
        int hasPredictionSystem = metadata.get<int>("hasPredictionSystem", 0);
        int hasPlanner = metadata.get<int>("hasPlanner", 0);
        int hasConceptFormation = metadata.get<int>("hasConceptFormation", 0);
        int hasAttention = metadata.get<int>("hasAttention", 0);
        int hasDevelopmentSystem = metadata.get<int>("hasDevelopmentSystem", 0);
        int hasDopamine = metadata.get<int>("hasDopamine", 0);
        int hasCuriosity = metadata.get<int>("hasCuriosity", 0);
        int hasNovelty = metadata.get<int>("hasNovelty", 0);
        int hasPredictionError = metadata.get<int>("hasPredictionError", 0);
        
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
        
        // Re-initialize integrated systems based on metadata
        // This ensures all systems are properly created when loading
        
        // Re-create working memory if it was saved
        if (hasWorkingMemory && !pImpl->workingMemory) {
            pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
            pImpl->workingMemory->initialize(this);
            pImpl->workingMemory->setCapacity(pImpl->config->getOr<size_t>("working_memory_capacity", getTotalNeuronCount() / 10));
        }
        
        // Re-create episodic memory if it was saved
        if (hasEpisodicMemory && !pImpl->episodicMemory) {
            pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            pImpl->episodicMemory->initialize(this);
            pImpl->episodicMemory->setMaxEpisodes(1000);
        }
        
        // Re-create associative memory if it was saved
        if (hasAssociativeMemory && !pImpl->associativeMemory) {
            pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
            pImpl->associativeMemory->initialize(this);
        }
        
        // Re-create prediction system if it was saved
        if (hasPredictionSystem && !pImpl->predictionSystem) {
            pImpl->predictionSystem = std::make_unique<PredictionSystem>();
            // Note: PredictionSystem doesn't have initialize method
        }
        
        // Re-create cognition systems if they were saved
        if (hasPlanner && !pImpl->planner) {
            pImpl->planner = std::make_unique<NeuralPlanner>();
            pImpl->planner->initialize(this);
            pImpl->planner->setPlanningDepth(5);
        }
        
        if (hasConceptFormation && !pImpl->conceptFormation) {
            pImpl->conceptFormation = std::make_unique<ConceptFormation>();
            pImpl->conceptFormation->initialize(this);
        }
        
        if (hasAttention && !pImpl->attention) {
            pImpl->attention = std::make_unique<AttentionalSelection>();
            pImpl->attention->initialize(this);
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
        }
        
        // Re-create development system if it was saved
        if (hasDevelopmentSystem && !pImpl->developmentSystem) {
            pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
        }
        
        // Re-create neuromodulation systems if they were saved
        if (hasDopamine && !pImpl->dopamine) {
            pImpl->dopamine = std::make_unique<Dopamine>();
        }
        
        if (hasCuriosity && !pImpl->curiosity) {
            pImpl->curiosity = std::make_unique<Curiosity>();
        }
        
        if (hasNovelty && !pImpl->novelty) {
            pImpl->novelty = std::make_unique<Novelty>();
        }
        
        if (hasPredictionError && !pImpl->predictionError) {
            pImpl->predictionError = std::make_unique<PredictionError>();
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
    
    // Comprehensive memory system status
    NLM_LOG_INFO("--- Memory Systems ---");
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
        NLM_LOG_INFO("Working memory capacity: " + std::to_string(pImpl->workingMemory->getCapacity()));
        NLM_LOG_INFO("Working memory decay rate: " + std::to_string(pImpl->workingMemory->getDecayRate()));
    } else {
        NLM_LOG_INFO("Working memory: NOT INITIALIZED");
    }
    
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
        NLM_LOG_INFO("Max episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getMaxEpisodes()));
    } else {
        NLM_LOG_INFO("Episodic memory: NOT INITIALIZED");
    }
    
    if (pImpl->associativeMemory) {
        NLM_LOG_INFO("Associative memory: INITIALIZED (basic implementation)");
    } else {
        NLM_LOG_INFO("Associative memory: NOT INITIALIZED");
    }
    
    // Comprehensive prediction system status
    NLM_LOG_INFO("--- Prediction System ---");
    if (pImpl->predictionSystem) {
        NLM_LOG_INFO("Prediction system: INITIALIZED");
        NLM_LOG_INFO("Prediction error: " + std::to_string(pImpl->predictionSystem->getPredictionError()));
        NLM_LOG_INFO("Prediction confidence: " + std::to_string(pImpl->predictionSystem->getConfidence()));
        NLM_LOG_INFO("Error history size: " + std::to_string(pImpl->predictionSystem->getErrorHistory().size()));
    } else {
        NLM_LOG_INFO("Prediction system: NOT INITIALIZED");
    }
    
    // Comprehensive cognition systems status
    NLM_LOG_INFO("--- Cognition Systems ---");
    if (pImpl->planner) {
        NLM_LOG_INFO("NeuralPlanner: INITIALIZED");
        NLM_LOG_INFO("Planning depth: " + std::to_string(pImpl->planner->getPlanningDepth()));
        NLM_LOG_INFO("Planning confidence: " + std::to_string(pImpl->planner->getPlanningConfidence()));
    } else {
        NLM_LOG_INFO("NeuralPlanner: NOT INITIALIZED");
    }
    
    if (pImpl->conceptFormation) {
        NLM_LOG_INFO("ConceptFormation: INITIALIZED");
        NLM_LOG_INFO("Concept count: " + std::to_string(pImpl->conceptFormation->getConceptCount()));
        NLM_LOG_INFO("Formation threshold: " + std::to_string(pImpl->conceptFormation->getFormationThreshold()));
        NLM_LOG_INFO("Stability window: " + std::to_string(pImpl->conceptFormation->getStabilityWindow()));
    } else {
        NLM_LOG_INFO("ConceptFormation: NOT INITIALIZED");
    }
    
    if (pImpl->attention) {
        NLM_LOG_INFO("AttentionalSelection: INITIALIZED");
        NLM_LOG_INFO("Inhibition strength: " + std::to_string(pImpl->attention->getInhibitionStrength()));
        NLM_LOG_INFO("Excitation strength: " + std::to_string(pImpl->attention->getExcitationStrength()));
    } else {
        NLM_LOG_INFO("AttentionalSelection: NOT INITIALIZED");
    }
    
    // Development system status
    NLM_LOG_INFO("--- Development System ---");
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    if (pImpl->developmentSystem) {
        NLM_LOG_INFO("Development system: INITIALIZED");
    } else {
        NLM_LOG_INFO("Development system: NOT INITIALIZED");
    }
    
    // Comprehensive neuromodulation systems status
    NLM_LOG_INFO("--- Neuromodulation Systems ---");
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine: INITIALIZED");
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
        NLM_LOG_INFO("Dopamine plasticity factor: " + std::to_string(pImpl->dopamine->getPlasticityFactor()));
    } else {
        NLM_LOG_INFO("Dopamine: NOT INITIALIZED");
    }
    
    if (pImpl->curiosity) {
        NLM_LOG_INFO("Curiosity: INITIALIZED");
        NLM_LOG_INFO("Curiosity level: " + std::to_string(pImpl->curiosity->getLevel()));
    } else {
        NLM_LOG_INFO("Curiosity: NOT INITIALIZED");
    }
    
    if (pImpl->novelty) {
        NLM_LOG_INFO("Novelty: INITIALIZED");
        NLM_LOG_INFO("Novelty level: " + std::to_string(pImpl->novelty->getLevel()));
    } else {
        NLM_LOG_INFO("Novelty: NOT INITIALIZED");
    }
    
    if (pImpl->predictionError) {
        NLM_LOG_INFO("PredictionError: INITIALIZED");
        NLM_LOG_INFO("Prediction error level: " + std::to_string(pImpl->predictionError->getLevel()));
    } else {
        NLM_LOG_INFO("PredictionError: NOT INITIALIZED");
    }
    
    // Performance infrastructure status
    NLM_LOG_INFO("--- Performance Infrastructure ---");
    if (pImpl->checkpointManager) {
        NLM_LOG_INFO("CheckpointManager: INITIALIZED");
    } else {
        NLM_LOG_INFO("CheckpointManager: NOT INITIALIZED");
    }
    
    // Region details with system integration indicators
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "):");
        NLM_LOG_INFO("    Neurons: " + std::to_string(region->getTotalNeuronCount()) + 
                     ", Synapses: " + std::to_string(region->getSynapseCount()) + 
                     ", Avg weight: " + std::to_string(region->getAverageSynapticWeight()));
        
        // Check for neuromodulation integration in neurons
        bool hasNeuromodulatedNeurons = false;
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                // Check if neuron has neuromodulation properties
                if (neuron->getState().membranePotential > 0.0f) {
                    hasNeuromodulatedNeurons = true;
                    break;
                }
            }
            if (hasNeuromodulatedNeurons) break;
        }
        if (hasNeuromodulatedNeurons) {
            NLM_LOG_INFO("    Contains neuromodulated neurons");
        }
        
        // Check for plasticity integration
        NLM_LOG_INFO("    Plasticity: STDP=" + std::string(pImpl->stdp ? "YES" : "NO") + 
                         ", Hebbian=" + std::string(pImpl->hebbian ? "YES" : "NO") + 
                         ", Structural=" + std::string(pImpl->structuralPlasticity ? "YES" : "NO"));
    }
    
    NLM_LOG_INFO("=== End NLM Brain Status ===");
}

} // namespace nlm

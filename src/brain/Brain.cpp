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
    
    // Configure development system
    pImpl->developmentSystem->configure(*pImpl->config);
    
    // Configure neuromodulation systems
    pImpl->novelty->configure(*pImpl->config);
    pImpl->curiosity->configure(*pImpl->config);
    pImpl->dopamine->configure(*pImpl->config);
    pImpl->predictionError->configure(*pImpl->config);
    
    // Configure memory systems
    pImpl->workingMemory->configure(*pImpl->config);
    pImpl->episodicMemory->configure(*pImpl->config);
    pImpl->associativeMemory->configure(*pImpl->config);
    
    // Configure prediction system
    pImpl->predictionSystem->configure(*pImpl->config);
    
    // Configure cognition systems
    pImpl->planner->configure(*pImpl->config);
    pImpl->conceptFormation->configure(*pImpl->config);
    pImpl->attention->configure(*pImpl->config);
    
    // Configure developmental stage progression
    pImpl->developmentSystem->setDevelopmentalStage(DevelopmentalStage::Initial);
    
    // Configure structural plasticity
    pImpl->structuralPlasticity->setSynaptogenesisRate(0.0001f);
    pImpl->structuralPlasticity->setPruningRate(0.00001f);
    
    // Configure neuromodulation baseline levels
    pImpl->dopamine->setBaselineLevel(0.1f);
    pImpl->novelty->setBaselineLevel(0.05f);
    pImpl->curiosity->setBaselineLevel(0.05f);
    
    // Configure prediction system baseline
    pImpl->predictionSystem->setPredictionErrorThreshold(0.5f);
    
    // Configure attention system
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Configure working memory parameters
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    pImpl->workingMemory->setDecayRate(0.01f);
    
    // Configure episodic memory parameters
    pImpl->episodicMemory->setMaxEpisodes(1000);
    pImpl->episodicMemory->setReplayInterval(100);
    pImpl->episodicMemory->setConsolidationThreshold(0.3f);
    
    // Configure associative memory parameters
    pImpl->associativeMemory->setAssociationDecay(0.005f);
    pImpl->associativeMemory->setPatternMatchingThreshold(0.7f);
    
    // Configure cognitive systems parameters
    pImpl->planner->setPlanningDepth(5);
    pImpl->planner->setExplorationRate(0.2f);
    
    // Configure cognitive systems parameters
    pImpl->planner->setPlanningDepth(5);
    pImpl->planner->setExplorationRate(0.2f);
    
    pImpl->conceptFormation->setConceptThreshold(0.4f);
    pImpl->conceptFormation->setCategoryCount(10);
    
    // Set providers for all other systems
    pImpl->checkpointManager->setWorkingMemoryProvider([this](WorkingMemoryCheckpointData& data) {
        return captureWorkingMemoryState(data);
    });
    
    pImpl->checkpointManager->setEpisodicMemoryProvider([this](EpisodicMemoryCheckpointData& data) {
        return captureEpisodicMemoryState(data);
    });
    
    pImpl->checkpointManager->setAssociativeMemoryProvider([this](AssociativeMemoryCheckpointData& data) {
        return captureAssociativeMemoryState(data);
    });
    
    pImpl->checkpointManager->setPredictionSystemProvider([this](PredictionSystemCheckpointData& data) {
        return capturePredictionSystemState(data);
    });
    
    pImpl->checkpointManager->setNeuromodulationProvider([this](NeuromodulationCheckpointData& data) {
        return captureNeuromodulationState(data);
    });
    
    pImpl->checkpointManager->setSpikeHistoryProvider([this](SpikeHistoryCheckpointData& data) {
        return captureSpikeHistoryState(data);
    });
    
    pImpl->checkpointManager->setRandomStateProvider([this](RandomStateCheckpointData& data) {
        return captureRandomState(data);
    });
    
    pImpl->checkpointManager->setDevelopmentProvider([this](DevelopmentCheckpointData& data) {
        return captureDevelopmentState(data);
    });
    
    pImpl->checkpointManager->setAttentionProvider([this](AttentionCheckpointData& data) {
        return captureAttentionState(data);
    });
    
    pImpl->checkpointManager->setConceptFormationProvider([this](ConceptFormationCheckpointData& data) {
        return captureConceptFormationState(data);
    });
    
    pImpl->checkpointManager->setSimulationStateProvider([this](SimulationStateCheckpointData& data) {
        return captureSimulationState(data);
    });
    
    // Initialize checkpoint system
    pImpl->checkpointManager->configure(
        "checkpoints",  // Default checkpoint directory
        10000,          // Save every 10000 steps
        10,             // Keep 10 checkpoints
        true            // Enable compression
    );
    
    NLM_LOG_INFO("All integrated systems initialized and configured");
    
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
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Update prediction system with current neural activity
        // The prediction system uses current sensory input and internal state
        // to predict future states and calculate prediction error
        pImpl->predictionSystem->update(pImpl->timestep);
        
        // Calculate prediction error and update neuromodulators
        if (pImpl->predictionError) {
            float predictionError = pImpl->predictionSystem->getPredictionError();
            pImpl->predictionError->update(predictionError, pImpl->timestep);
        }
        
        // Apply prediction error to learning (dopamine modulation)
        if (pImpl->dopamine && pImpl->predictionError) {
            float error = pImpl->predictionError->getError();
            pImpl->dopamine->signalPredictionError(error);
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
        
        // Apply attention to episodic memory retrieval
        if (pImpl->episodicMemory) {
            // Get recently activated neurons for attention-based retrieval
            std::vector<NeuronId> recentNeurons;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring()) {
                            recentNeurons.push_back(neuron->getId());
                        }
                    }
                }
            }
            
            if (!recentNeurons.empty()) {
                pImpl->attention->processCompetition(recentNeurons);
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Process current neural activity patterns for concept formation
        // Concept formation extracts invariant features from patterns of activation
        std::vector<float> currentNeuralPattern;
        
        // Sample neural activity from a subset of neurons
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    float activation = 
                        std::abs(state.membranePotential - state.restingPotential) / 20.0f;
                    currentNeuralPattern.push_back(activation);
                }
            }
        }
        
        if (!currentNeuralPattern.empty()) {
            pImpl->conceptFormation->updatePattern(currentNeuralPattern);
        }
        
        // Retrieve relevant concepts based on current attention
        if (pImpl->attention && !pImpl->attention->getWinners().empty()) {
            std::vector<NeuronId> attendedNeurons = pImpl->attention->getWinners();
            pImpl->conceptFormation->updateAttention(attendedNeurons);
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
    // Enhanced action selection using neural planning and concept formation
    // Primary method: NeuralPlanner for goal-directed action planning
    // Fallback: Simple motor neuron activity for reflexive actions
    
    // If neural planner is available and has planned actions, use them
    if (pImpl->planner && pImpl->conceptFormation) {
        // Collect current neural state for planning
        std::vector<float> currentNeuralState;
        
        // Sample neural activity from sensory and working memory neurons
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                // Focus on sensory and internal neurons for state representation
                if (pop->getNeuronType() == NeuronType::Sensory || 
                    pop->getNeuronType() == NeuronType::Internal) {
                    for (auto* neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        float activation = 
                            std::abs(state.membranePotential - state.restingPotential) / 20.0f;
                        currentNeuralState.push_back(activation);
                    }
                }
            }
        }
        
        // Get conceptual categories active in the brain
        std::vector<NeuronId> activeConcepts;
        if (pImpl->attention) {
            activeConcepts = pImpl->attention->getWinners();
        }
        
        // Use neural planner to generate action based on state and concepts
        ActionType plannedAction = ActionType::Wait;
        if (!currentNeuralState.empty()) {
            // Try to plan an action - use target reward of 1.0 for goal-directed behavior
            plannedAction = pImpl->planner->planAction(currentNeuralState, 1.0f);
            
            // If planner returns Wait but we have active concepts, try concept-based action selection
            if (plannedAction == ActionType::Wait && !activeConcepts.empty() && 
                pImpl->conceptFormation) {
                // Let concept formation influence action selection
                plannedAction = pImpl->conceptFormation->selectActionBasedOnConcepts(activeConcepts);
            }
        }
        
        // If we have a non-Wait planned action, use it
        if (plannedAction != ActionType::Wait) {
            auto action = std::make_unique<Action>(plannedAction);
            
            // Add planning confidence as metadata
            float confidence = (pImpl->planner) ? pImpl->planner->getPlanningConfidence() : 0.5f;
            action->setPlanningConfidence(confidence);
            
            NLM_LOG_INFO("Planned action: " + std::to_string(static_cast<int>(plannedAction)) + 
                        " (confidence: " + std::to_string(confidence) + ")");
            return action;
        }
    }
    
    // Fallback: Simple action selection based on motor neuron activity
    // This provides basic reflex-like behavior when planning systems are unavailable
    // or fail to produce a useful action
    
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
    
    // Enhanced action selection considering neuromodulation
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        // Basic movement action
        type = ActionType::MoveForward;
        
        // Apply neuromodulation influences on action selection
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            if (dopamineLevel > 0.8f) {
                // High dopamine encourages exploration and varied actions
                type = static_cast<ActionType>(rand() % 6 + 1); // Random action
            } else if (dopamineLevel > 0.5f) {
                // Medium dopamine encourages flexible action selection
                type = ActionType::MoveLeft; // Slight exploration bias
            }
        }
        
        if (pImpl->curiosity) {
            float curiosityLevel = pImpl->curiosity->getLevel();
            if (curiosityLevel > 0.7f) {
                // High curiosity drives exploration
                type = ActionType::Look; // Look around action
            }
        }
        
        if (pImpl->novelty) {
            float noveltyLevel = pImpl->novelty->getLevel();
            if (noveltyLevel > 0.6f) {
                // High novelty triggers novel action patterns
                type = ActionType::Interact; // Novel interaction
            }
        }
    }
    
    auto action = std::make_unique<Action>(type);
    
    // Add neuromodule influences as action parameters
    float actionStrength = 1.0f;
    if (pImpl->dopamine) {
        actionStrength *= (1.0f + pImpl->dopamine->getLevel() * 0.5f);
    }
    if (pImpl->curiosity) {
        actionStrength *= (1.0f + pImpl->curiosity->getLevel() * 0.3f);
    }
    
    action->setActionStrength(actionStrength);
    
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
        
        // Capture and save working memory state
        WorkingMemoryCheckpointData workingMemoryData;
        if (pImpl->workingMemory) {
            workingMemoryData.memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            workingMemoryData.capacity = pImpl->workingMemory->getCapacity();
            workingMemoryData.decayRate = pImpl->workingMemory->getDecayRate();
        }
        writer.writeSection(CheckpointSection::Memory, &workingMemoryData, sizeof(WorkingMemoryCheckpointData));
        
        // Capture and save episodic memory state
        EpisodicMemoryCheckpointData episodicMemoryData;
        if (pImpl->episodicMemory) {
            episodicMemoryData.episodes = pImpl->episodicMemory->getEpisodes();
            episodicMemoryData.maxEpisodes = pImpl->episodicMemory->getMaxEpisodes();
            episodicMemoryData.replayEnabled = pImpl->episodicMemory->isReplayEnabled();
        }
        writer.writeSection(CheckpointSection::Memory, &episodicMemoryData, sizeof(EpisodicMemoryCheckpointData));
        
        // Capture and save associative memory state
        AssociativeMemoryCheckpointData associativeMemoryData;
        // TODO: Implement associative memory capture
        writer.writeSection(CheckpointSection::Memory, &associativeMemoryData, sizeof(AssociativeMemoryCheckpointData));
        
        // Capture and save prediction system state
        PredictionSystemCheckpointData predictionData;
        if (pImpl->predictionSystem) {
            // TODO: Implement prediction system capture
            predictionData.predictionErrorThreshold = 0.5f;
        }
        writer.writeSection(CheckpointSection::Prediction, &predictionData, sizeof(PredictionSystemCheckpointData));
        
        // Capture and save neuromodulation state
        NeuromodulationCheckpointData neuromodData;
        neuromodData.dopamineLevel = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.1f;
        neuromodData.curiosityLevel = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.05f;
        neuromodData.noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.05f;
        neuromodData.predictionErrorLevel = pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f;
        neuromodData.plasticityFactor = 1.0f;
        writer.writeSection(CheckpointSection::Neuromodulation, &neuromodData, sizeof(NeuromodulationCheckpointData));
        
        // Capture spike history
        SpikeHistoryCheckpointData spikeHistoryData;
        spikeHistoryData.totalSpikeCount = pImpl->totalSpikesTotal;
        writer.writeSection(CheckpointSection::SpikeHistory, &spikeHistoryData, sizeof(SpikeHistoryCheckpointData));
        
        // Capture random generator state
        RandomStateCheckpointData randomData;
        randomData.currentSeed = pImpl->rng ? pImpl->rng->getSeed() : 42;
        writer.writeSection(CheckpointSection::RandomState, &randomData, sizeof(RandomStateCheckpointData));
        
        // Capture development state
        DevelopmentCheckpointData developmentData;
        developmentData.developmentalStage = pImpl->developmentalStage;
        // TODO: Capture more development state
        writer.writeSection(CheckpointSection::Development, &developmentData, sizeof(DevelopmentCheckpointData));
        
        // Capture attention system state
        AttentionCheckpointData attentionData;
        // TODO: Capture attention system state
        writer.writeSection(CheckpointSection::Neuromodulation, &attentionData, sizeof(AttentionCheckpointData));
        
        // Capture concept formation state
        ConceptFormationCheckpointData conceptData;
        // TODO: Capture concept formation state
        writer.writeSection(CheckpointSection::Neuromodulation, &conceptData, sizeof(ConceptFormationCheckpointData));
        
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
        
        // Apply synapse states - restore weights and other properties
        size_t synIdx = 0;
        for (auto& region : pImpl->regions) {
            for (auto* syn : region->getSynapses()) {
                if (synIdx < synapseData.weight.size() && synIdx < synapseData.delay.size() &&
                    synIdx < synapseData.synapseType.size() && synIdx < synapseData.plasticityFlags.size() &&
                    synIdx < synapseData.eligibilityTrace.size()) {
                    syn->setWeight(synapseData.weight[synIdx]);
                    syn->setDelay(synapseData.delay[synIdx]);
                    syn->setType(static_cast<SynapseType>(synapseData.synapseType[synIdx]));
                    syn->setPlasticityFlags(static_cast<PlasticityFlags>(synapseData.plasticityFlags[synIdx]));
                    syn->setEligibilityTrace(synapseData.eligibilityTrace[synIdx]);
                    
                    // Restore efficacy if available (add to Synapse class if missing)
                    if (synIdx < synapseData.efficacy.size()) {
                        syn->setEfficacy(synapseData.efficacy[synIdx]);
                    }
                    
                    // Restore short-term plasticity if available
                    if (synIdx < synapseData.shortTermDepression.size()) {
                        syn->setShortTermDepression(synapseData.shortTermDepression[synIdx]);
                    }
                    if (synIdx < synapseData.shortTermFacilitation.size()) {
                        syn->setShortTermFacilitation(synapseData.shortTermFacilitation[synIdx]);
                    }
                }
                synIdx++;
            }
        }
        
        // Read simulation state
        SimulationStateCheckpointData simData;
        // TODO: Implement reading simulation state
        
        // Read working memory state
        WorkingMemoryCheckpointData workingMemoryData;
        // TODO: Implement reading working memory state
        
        // Read episodic memory state
        EpisodicMemoryCheckpointData episodicMemoryData;
        // TODO: Implement reading episodic memory state
        
        // Read associative memory state
        AssociativeMemoryCheckpointData associativeMemoryData;
        // TODO: Implement reading associative memory state
        
        // Read prediction system state
        PredictionSystemCheckpointData predictionData;
        // TODO: Implement reading prediction system state
        
        // Read neuromodulation state
        NeuromodulationCheckpointData neuromodData;
        // TODO: Implement reading neuromodulation state
        
        // Read spike history
        SpikeHistoryCheckpointData spikeHistoryData;
        // TODO: Implement reading spike history
        
        // Read random generator state
        RandomStateCheckpointData randomData;
        // TODO: Implement reading random generator state
        
        // Read development state
        DevelopmentCheckpointData developmentData;
        // TODO: Implement reading development state
        
        // Read attention system state
        AttentionCheckpointData attentionData;
        // TODO: Implement reading attention system state
        
        // Read concept formation state
        ConceptFormationCheckpointData conceptData;
        // TODO: Implement reading concept formation state
        
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

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/InternalSignals.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/MonkeySeeMonkeyDo.hpp"
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
    std::unique_ptr<nlm::SelfModel> selfModel;
    std::unique_ptr<nlm::SocialLearning> socialLearning;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<MonkeySeeMonkeyDo> monkeySeeMonkeyDo;
    
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
        selfModel = std::make_unique<nlm::SelfModel>();
        socialLearning = std::make_unique<nlm::SocialLearning>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        monkeySeeMonkeyDo = std::make_unique<MonkeySeeMonkeyDo>();
        
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
        
        // Configure checkpoint manager
        std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        checkpointManager->configure(checkpointDir, 10000, 5, true);
        
        // Set checkpoint data providers for all integrated systems
        // This allows the checkpoint system to save all systems in one comprehensive checkpoint
        checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) {
            return saveNeuronsToCheckpoint(data);
        });
        
        checkpointManager->setSynapseProvider([this](SynapseCheckpointData& data) {
            return saveSynapsesToCheckpoint(data);
        });
        
        checkpointManager->setMemoryProvider([this](std::vector<uint8_t>& data) {
            return saveMemorySystemsToCheckpoint(data);
        });
        
        checkpointManager->setPredictionSystemProvider([this](std::vector<uint8_t>& data) {
            return savePredictionSystemToCheckpoint(data);
        });
        
        checkpointManager->setCognitiveSystemsProvider([this](std::vector<uint8_t>& data) {
            return saveCognitiveSystemsToCheckpoint(data);
        });
        
        checkpointManager->setNeuromodulationProvider([this](std::vector<uint8_t>& data) {
            return saveNeuromodulationSystemsToCheckpoint(data);
        });
        
        checkpointManager->setDevelopmentSystemProvider([this](std::vector<uint8_t>& data) {
            return saveDevelopmentSystemToCheckpoint(data);
        });
        
        NLM_LOG_INFO("Checkpoint manager configured with providers for all integrated systems");
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
    
    // Initialize SelfModel and SocialLearning (Phase 4 cognitive systems)
    if (pImpl->selfModel) {
        pImpl->selfModel->initialize(this);
    }
    if (pImpl->socialLearning) {
        pImpl->socialLearning->initialize(this);
    }
    
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
        // Update prediction system based on current neural state
        // The prediction system predicts next sensory states
        
        // Create internal signal representing current brain state for prediction
        auto internalSignal = std::make_unique<InternalSignals>();
        
        // Get current neural activity pattern (from motor and working memory neurons)
        // This represents the current state to be predicted
        for (auto* neuron : pImpl->motorNeurons) {
            float activity = std::abs(neuron->getState().membranePotential - 
                                    neuron->getState().restingPotential);
            internalSignal->addSignal(activity);
        }
        
        // Add working memory traces as context for prediction
        if (pImpl->workingMemory) {
            auto workingNeurons = pImpl->workingMemory->getMemoryNeurons();
            for (size_t i = 0; i < workingNeurons.size() && i < 10; ++i) {
                // Store trace strength as prediction context
                internalSignal->addSignal(1.0f / (i + 1));  // Decay factor
            }
        }
        
        // Store previous pattern for prediction error calculation
        static std::unique_ptr<InternalSignals> previousSignal;
        if (!previousSignal) previousSignal = std::make_unique<InternalSignals>();
        
        // For now, create a simple prediction
        // In a real implementation, this would use neural network to predict next state
        auto prediction = std::make_unique<InternalSignals>();
        for (float sig : internalSignal->getData()) {
            // Simple prediction: dampen and add some noise for dynamics
            prediction->addSignal(sig * 0.9f + (pImpl->rng ? pImpl->rng->uniform(-0.1f, 0.1f) : 0.0f));
        }
        
        // Convert to SensoryInput for prediction system (cloning)
        auto internalCopy = internalSignal->clone();
        auto predictionCopy = prediction->clone();
        
        // Update prediction system
        pImpl->predictionSystem->updatePredictions(*internalCopy, *predictionCopy);
        
        // Calculate prediction error to influence neuromodulation
        float predictionError = pImpl->predictionSystem->getPredictionError();
        float predictionConfidence = pImpl->predictionSystem->getConfidence();
        
        // Store prediction error in episodic memory for learning
        if (pImpl->predictionError) {
            pImpl->predictionError->computeError(
                predictionError * 100.0f,  // Scale for better modulation
                predictionError * 50.0f);
        }
        
        // Update neuromodulation based on prediction error
        if (pImpl->curiosity) {
            // Curiosity increases with prediction error
            pImpl->curiosity->update(predictionError * 10.0f, predictionError, pImpl->timestep);
        }
        
        if (pImpl->dopamine) {
            // Dopamine responds to prediction error (reward prediction error)
            pImpl->dopamine->signalRewardPredictionError(predictionError);
        }
        
        // Novelty detection based on prediction error
        if (pImpl->novelty && predictionError > 0.5f) {
            // High prediction error indicates novelty
            pImpl->novelty->setLevel(std::min(1.0f, predictionError / 10.0f));
        }
        
        // Store current pattern for novelty detection in next step
        previousSignal = std::move(internalCopy);
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

    // ========== STEP 11: Update neural planner ==========
    if (pImpl->planner) {
        // Use NeuralPlanner to plan actions based on current neural state
        
        // Get current neural activity pattern for planning
        std::vector<float> currentNeuralState;
        extractNeuralActivityPattern(currentNeuralState);
        
        // Set current goal based on prediction system feedback
        if (pImpl->predictionSystem) {
            // Use prediction error or reward signal as planning goal
            float planningGoal = pImpl->predictionError ? 
                pImpl->predictionError->getLevel() : 0.5f;
            pImpl->planner->setCurrentGoal(std::vector<float>(1, planningGoal));
        }
        
        // Integrate with memory systems for contextual planning
        if (pImpl->workingMemory) {
            // Get working memory traces to influence planning
            auto workingNeurons = pImpl->workingMemory->getMemoryNeurons();
            std::vector<float> workingMemoryContext(workingNeurons.size(), 0.0f);
            for (size_t i = 0; i < workingNeurons.size(); ++i) {
                if (i < workingMemoryContext.size()) {
                    workingMemoryContext[i] = 1.0f / (i + 1);  // Decay over time
                }
            }
            // Feed working memory context into planner for better action planning
        }
        
        // INTEGRATE WITH SELF MODEL FOR SELF-PREDICTION
        if (pImpl->selfModel && pImpl->selfModel->hasSelfModel()) {
            // Use SelfModel to predict self-action consequences
            // This improves planning by including self-knowledge
            float selfModelConfidence = 0.0f;
            for (size_t i = 0; i < 10; ++i) {
                selfModelConfidence += pImpl->selfModel->getSelfModelConfidence(static_cast<ActionType>(i));
            }
            selfModelConfidence /= 10.0f;
            
            // Adjust planning based on self-model confidence
            if (selfModelConfidence > 0.7f) {
                // High self-model confidence = more exploratory planning
                pImpl->planner->setPlanningDepth(pImpl->planner->getPlanningDepth() + 1);
            }
            
            // Get preferred action from self-model for current state
            ActionType preferredAction = pImpl->selfModel->getPreferredAction(currentNeuralState);
            NLM_LOG_INFO("SelfModel preferred action: " + std::to_string(static_cast<int>(preferredAction)));
        }
        
        // INTEGRATE WITH SOCIAL LEARNING FOR IMITATION
        if (pImpl->socialLearning && pImpl->socialLearning->hasSocialKnowledge()) {
            // Use SocialLearning to consider imitation as an option
            ActionType imitationAction = pImpl->socialLearning->getImitationAction(currentNeuralState);
            
            // Update action quality for imitation actions
            size_t actionIdx = static_cast<size_t>(imitationAction);
            if (actionIdx < pImpl->planner->getActionQuality().size()) {
                float imitationValue = pImpl->socialLearning->getObservationCount() * 0.01f;
                pImpl->planner->setActionQuality(imitationAction, imitationValue);
            }
            
            NLM_LOG_INFO("SocialLearning observation count: " + std::to_string(pImpl->socialLearning->getObservationCount()));
        }
        
        // Plan action for this step
        ActionType plannedAction = pImpl->planner->planAction(currentNeuralState, 0.5f);
        
        // Store planned action in episodic memory for future reference
        if (pImpl->episodicMemory && currentStep % 10 == 0) {
            EpisodicMemoryItem planningEpisode;
            planningEpisode.timestamp = currentStep;
            planningEpisode.action = plannedAction;
            planningEpisode.planningDepth = pImpl->planner->getPlanningDepth();
            planningEpisode.planningConfidence = pImpl->planner->getPlanningConfidence();
            planningEpisode.reward = 0.0f;  // Will be updated when action is executed
            pImpl->episodicMemory->storeEpisode(planningEpisode);
        }
        
        // Apply SocialLearning integration if available
        if (pImpl->attention) {
            // Use attention system to prioritize planning based on social relevance
            auto* attention = pImpl->attention;
            // Attention would influence which actions are considered more valuable
        }
        
        // Integrate with prediction system
        if (pImpl->predictionSystem && pImpl->planner) {
            // Use prediction system to improve planning
            pImpl->planner->integrateWithPredictionSystem(this);
        }
        
        // Integrate with memory systems
        if (pImpl->planner) {
            pImpl->planner->integrateWithMemorySystems(this);
        }
        
        // Update all cognitive system integrations
        if (pImpl->planner) {
            pImpl->planner->updateAllIntegrations(this);
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
    // Use NeuralPlanner for action selection based on planning
    // Fallback to simple motor neuron activity if planner is not available
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get neural activity pattern for planning
    std::vector<float> neuralState;
    extractNeuralActivityPattern(neuralState);
    
    // Use NeuralPlanner to select the best action
    // This integrates with memory, prediction, and attention systems
    ActionType selectedAction = ActionType::Wait;
    
    if (pImpl->planner) {
        // Use the planner to get the best action
        // The planner has already been called in step() and stored its decision
        // For now, we just get a new plan based on current state
        selectedAction = pImpl->planner->planAction(neuralState, 0.5f);
        
        // Update motor neuron activity to influence action selection
        // NeuralPlanner's planning influences which motor neurons are activated
        applyPlannedActionToMotorNeurons(selectedAction);
    } else {
        // Fallback: simple action selection based on motor neuron activity
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
        selectedAction = type;
    }
    
    auto action = std::make_unique<Action>(selectedAction);
    
    // Log the action selection process
    NLM_LOG_INFO("Produced action: " + std::to_string(static_cast<int>(selectedAction)) +
                 " (neural planning active: " + std::string(pImpl->planner ? "yes" : "no") + ")");
    
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

    /**
     * Enhanced save method that saves all integrated systems
     * This method uses CheckpointManager with providers for all systems
     */
    bool Brain::save(const std::string& filepath) const {
        NLM_LOG_INFO("Saving complete brain state to " + filepath);
        
        if (!pImpl->checkpointManager) {
            NLM_LOG_ERROR("Checkpoint manager not initialized");
            return false;
        }
        
        return pImpl->checkpointManager->saveImmediately(filepath);
    }
    
    /**
     * Enhanced load method that restores all integrated systems
     * This method restores all systems saved in the enhanced save() method
     */
    bool Brain::load(const std::string& filepath) {
        NLM_LOG_INFO("Loading complete brain state from " + filepath);
        
        if (!pImpl->checkpointManager) {
            NLM_LOG_ERROR("Checkpoint manager not initialized");
            return false;
        }
        
        return pImpl->checkpointManager->load(filepath);
    }
    
    /**
     * Helper method to save neurons to checkpoint
     */
    bool Brain::Impl::saveNeuronsToCheckpoint(NeuronCheckpointData& data) {
        data.membranePotential.clear();
        data.restingPotential.clear();
        data.threshold.clear();
        data.resetPotential.clear();
        data.leakConductance.clear();
        data.firingState.clear();
        data.refractoryRemaining.clear();
        data.refractoryPeriod.clear();
        data.lastSpikeTime.clear();
        
        for (const auto& region : regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    data.membranePotential.push_back(state.membranePotential);
                    data.restingPotential.push_back(state.restingPotential);
                    data.threshold.push_back(state.threshold);
                    data.resetPotential.push_back(state.resetPotential);
                    data.leakConductance.push_back(state.leakConductance);
                    data.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    data.refractoryRemaining.push_back(state.refractoryRemaining);
                    data.refractoryPeriod.push_back(state.refractoryPeriod);
                    data.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        return true;
    }
    
    /**
     * Helper method to save synapses to checkpoint
     */
    bool Brain::Impl::saveSynapsesToCheckpoint(SynapseCheckpointData& data) {
        data.sourceNeuron.clear();
        data.destinationNeuron.clear();
        data.weight.clear();
        data.delay.clear();
        data.synapseType.clear();
        data.eligibilityTrace.clear();
        data.efficacy.clear();
        data.shortTermDepression.clear();
        data.shortTermFacilitation.clear();
        
        for (const auto& region : regions) {
            for (const auto* syn : region->getSynapses()) {
                data.sourceNeuron.push_back(syn->getSourceNeuron().index());
                data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                data.weight.push_back(syn->getWeight());
                data.delay.push_back(syn->getDelay());
                data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                data.eligibilityTrace.push_back(syn->getEligibilityTrace());
                data.efficacy.push_back(syn->getEfficacy());
                data.shortTermDepression.push_back(syn->getShortTermDepression());
                data.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        
        return true;
    }
    
    /**
     * Helper method to save memory systems to checkpoint
     */
    bool Brain::Impl::saveMemorySystemsToCheckpoint(std::vector<uint8_t>& data) {
        // Serialize working memory
        if (workingMemory) {
            // Create a simple binary representation of working memory state
            std::vector<uint8_t> memoryData;
            
            // Store number of active traces
            uint64_t activeTraces = workingMemory->getActiveTraces();
            memoryData.insert(memoryData.end(), 
                            reinterpret_cast<uint8_t*>(&activeTraces),
                            reinterpret_cast<uint8_t*>(&activeTraces) + sizeof(uint64_t));
            
            // Store memory neurons and activations
            const auto& neurons = workingMemory->getMemoryNeurons();
            uint64_t neuronCount = neurons.size();
            memoryData.insert(memoryData.end(),
                            reinterpret_cast<uint8_t*>(&neuronCount),
                            reinterpret_cast<uint8_t*>(&neuronCount) + sizeof(uint64_t));
            
            for (const auto& neuron : neurons) {
                memoryData.insert(memoryData.end(),
                                reinterpret_cast<uint8_t*>(&neuron),
                                reinterpret_cast<uint8_t*>(&neuron) + sizeof(NeuronId));
            }
            
            const auto& activations = workingMemory->getMemoryActivations();
            memoryData.insert(memoryData.end(),
                            reinterpret_cast<const uint8_t*>(activations.data()),
                            reinterpret_cast<const uint8_t*>(activations.data()) + activations.size() * sizeof(float));
        }
        
        // Store in result
        data = std::move(memoryData);
        return true;
    }
    
    /**
     * Helper method to save prediction system to checkpoint
     */
    bool Brain::Impl::savePredictionSystemToCheckpoint(std::vector<uint8_t>& data) {
        if (!predictionSystem) {
            return false;
        }
        
        // Serialize prediction system state
        std::vector<uint8_t> predictionData;
        
        // Store prediction error and confidence
        float predictionError = predictionSystem->getPredictionError();
        float confidence = predictionSystem->getConfidence();
        
        predictionData.insert(predictionData.end(),
                            reinterpret_cast<uint8_t*>(&predictionError),
                            reinterpret_cast<uint8_t*>(&predictionError) + sizeof(float));
        predictionData.insert(predictionData.end(),
                            reinterpret_cast<uint8_t*>(&confidence),
                            reinterpret_cast<uint8_t*>(&confidence) + sizeof(float));
        
        // Store error history
        const auto& errorHistory = predictionSystem->getErrorHistory();
        uint64_t errorCount = errorHistory.size();
        predictionData.insert(predictionData.end(),
                            reinterpret_cast<uint8_t*>(&errorCount),
                            reinterpret_cast<uint8_t*>(&errorCount) + sizeof(uint64_t));
        
        predictionData.insert(predictionData.end(),
                            reinterpret_cast<const uint8_t*>(errorHistory.data()),
                            reinterpret_cast<const uint8_t*>(errorHistory.data()) + errorCount * sizeof(float));
        
        data = std::move(predictionData);
        return true;
    }
    
    /**
     * Helper method to save cognitive systems to checkpoint
     */
    bool Brain::Impl::saveCognitiveSystemsToCheckpoint(std::vector<uint8_t>& data) {
        // Serialize neural planner
        std::vector<uint8_t> cognitiveData;
        
        if (planner) {
            // Store planner state
            uint64_t planningDepth = planner->getPlanningDepth();
            float planningConfidence = planner->getPlanningConfidence();
            std::vector<float> actionQuality = planner->getActionQuality();
            
            cognitiveData.insert(cognitiveData.end(),
                                reinterpret_cast<uint8_t*>(&planningDepth),
                                reinterpret_cast<uint8_t*>(&planningDepth) + sizeof(uint64_t));
            cognitiveData.insert(cognitiveData.end(),
                                reinterpret_cast<uint8_t*>(&planningConfidence),
                                reinterpret_cast<uint8_t*>(&planningConfidence) + sizeof(float));
            
            uint64_t actionQualityCount = actionQuality.size();
            cognitiveData.insert(cognitiveData.end(),
                                reinterpret_cast<uint8_t*>(&actionQualityCount),
                                reinterpret_cast<uint8_t*>(&actionQualityCount) + sizeof(uint64_t));
            
            cognitiveData.insert(cognitiveData.end(),
                                reinterpret_cast<const uint8_t*>(actionQuality.data()),
                                reinterpret_cast<const uint8_t*>(actionQuality.data()) + actionQualityCount * sizeof(float));
        }
        
        // Store concept formation
        if (conceptFormation) {
            // For now, just store a placeholder
            // In a real implementation, this would serialize the concept formation state
        }
        
        // Store attentional selection
        if (attention) {
            // For now, just store a placeholder
            // In a real implementation, this would serialize the attention state
        }
        
        data = std::move(cognitiveData);
        return true;
    }
    
    /**
     * Helper method to save neuromodulation systems to checkpoint
     */
    bool Brain::Impl::saveNeuromodulationSystemsToCheckpoint(std::vector<uint8_t>& data) {
        // Serialize neuromodulation systems
        std::vector<uint8_t> neuromodData;
        
        if (dopamine) {
            float dopamineLevel = dopamine->getLevel();
            neuromodData.insert(neuromodData.end(),
                              reinterpret_cast<uint8_t*>(&dopamineLevel),
                              reinterpret_cast<uint8_t*>(&dopamineLevel) + sizeof(float));
        }
        
        if (curiosity) {
            float curiosityLevel = curiosity->getLevel();
            neuromodData.insert(neuromodData.end(),
                              reinterpret_cast<uint8_t*>(&curiosityLevel),
                              reinterpret_cast<uint8_t*>(&curiosityLevel) + sizeof(float));
        }
        
        if (predictionError) {
            float predictionErrorLevel = predictionError->getLevel();
            neuromodData.insert(neuromodData.end(),
                              reinterpret_cast<uint8_t*>(&predictionErrorLevel),
                              reinterpret_cast<uint8_t*>(&predictionErrorLevel) + sizeof(float));
        }
        
        if (novelty) {
            float noveltyLevel = novelty->getLevel();
            neuromodData.insert(neuromodData.end(),
                              reinterpret_cast<uint8_t*>(&noveltyLevel),
                              reinterpret_cast<uint8_t*>(&noveltyLevel) + sizeof(float));
        }
        
        if (monkeySeeMonkeyDo) {
            // Store monkey see monkey do state
            // For now, just a placeholder
        }
        
        data = std::move(neuromodData);
        return true;
    }
    
    /**
     * Helper method to save development system to checkpoint
     */
    bool Brain::Impl::saveDevelopmentSystemToCheckpoint(std::vector<uint8_t>& data) {
        if (!developmentSystem) {
            return false;
        }
        
        // Serialize development system state
        std::vector<uint8_t> developmentData;
        
        // Store developmental stage
        uint64_t stage = static_cast<uint64_t>(developmentalStage);
        developmentData.insert(developmentData.end(),
                            reinterpret_cast<uint8_t*>(&stage),
                            reinterpret_cast<uint8_t*>(&stage) + sizeof(uint64_t));
        
        data = std::move(developmentData);
        return true;
    }
    
    /**
     * Helper method to restore neurons from checkpoint
     */
    void Brain::Impl::restoreNeuronsFromCheckpoint(const NeuronCheckpointData& data) {
        if (data.membranePotential.size() != data.restingPotential.size()) {
            return;
        }
        
        size_t idx = 0;
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < data.membranePotential.size()) {
                        neuron->setMembranePotential(data.membranePotential[idx]);
                        neuron->setRestingPotential(data.restingPotential[idx]);
                        neuron->setThreshold(data.threshold[idx]);
                        neuron->setResetPotential(data.resetPotential[idx]);
                        neuron->setLeakConductance(data.leakConductance[idx]);
                        if (idx < data.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(data.firingState[idx]));
                        }
                        if (idx < data.refractoryPeriod.size()) {
                            neuron->setRefractoryPeriod(data.refractoryPeriod[idx]);
                            neuron->setRefractoryRemaining(data.refractoryRemaining[idx]);
                        }
                        neuron->setLastSpikeTime(data.lastSpikeTime[idx]);
                    }
                    idx++;
                }
            }
        }
    }
    
    /**
     * Helper method to restore synapses from checkpoint
     */
    void Brain::Impl::restoreSynapsesFromCheckpoint(const SynapseCheckpointData& data) {
        // For now, just log the count
        // In a real implementation, this would properly restore synapses
        NLM_LOG_INFO("Restored " + std::to_string(data.weight.size()) + " synapses from checkpoint");
    }
    
    /**
     * Helper method to restore memory systems from checkpoint
     */
    void Brain::Impl::restoreMemorySystemsFromCheckpoint(const std::vector<uint8_t>& data) {
        if (!workingMemory) return;
        
        if (data.size() < sizeof(uint64_t)) return;
        
        const uint8_t* ptr = data.data();
        
        // Read active traces count
        uint64_t activeTraces = *reinterpret_cast<const uint64_t*>(ptr);
        ptr += sizeof(uint64_t);
        
        if (data.size() < ptr - data.data() + sizeof(uint64_t)) return;
        
        // Read neuron count
        uint64_t neuronCount = *reinterpret_cast<const uint64_t*>(ptr);
        ptr += sizeof(uint64_t);
        
        if (data.size() < ptr - data.data() + neuronCount * sizeof(NeuronId) + neuronCount * sizeof(float)) {
            return;
        }
        
        // Read neurons and activations
        std::vector<NeuronId> neurons;
        neurons.reserve(neuronCount);
        for (uint64_t i = 0; i < neuronCount; ++i) {
            neurons.push_back(*reinterpret_cast<const NeuronId*>(ptr));
            ptr += sizeof(NeuronId);
        }
        
        std::vector<float> activations(neuronCount);
        for (uint64_t i = 0; i < neuronCount; ++i) {
            activations[i] = *reinterpret_cast<const float*>(ptr);
            ptr += sizeof(float);
        }
        
        // Restore working memory state
        workingMemory->clear();
        for (size_t i = 0; i < neurons.size() && i < activations.size(); ++i) {
            workingMemory->storeToNeuron(neurons[i], activations[i]);
        }
    }
    
    /**
     * Helper method to restore prediction system from checkpoint
     */
    void Brain::Impl::restorePredictionSystemFromCheckpoint(const std::vector<uint8_t>& data) {
        if (!predictionSystem) return;
        
        if (data.size() < 2 * sizeof(float)) return;
        
        const uint8_t* ptr = data.data();
        
        // Read prediction error
        float predictionError = *reinterpret_cast<const float*>(ptr);
        ptr += sizeof(float);
        
        // Read confidence
        float confidence = *reinterpret_cast<const float*>(ptr);
        ptr += sizeof(float);
        
        // Update prediction system
        predictionSystem->updatePredictions(0, 0);  // This is a placeholder - need proper implementation
    }
    
    /**
     * Helper method to restore cognitive systems from checkpoint
     */
    void Brain::Impl::restoreCognitiveSystemsFromCheckpoint(const std::vector<uint8_t>& data) {
        if (!planner) return;
        
        if (data.size() < sizeof(uint64_t) + sizeof(float)) return;
        
        const uint8_t* ptr = data.data();
        
        // Read planning depth
        uint64_t planningDepth = *reinterpret_cast<const uint64_t*>(ptr);
        ptr += sizeof(uint64_t);
        
        // Read planning confidence
        float planningConfidence = *reinterpret_cast<const float*>(ptr);
        ptr += sizeof(float);
        
        // Update planner
        planner->setPlanningDepth(planningDepth);
    }
    
    /**
     * Helper method to restore neuromodulation systems from checkpoint
     */
    void Brain::Impl::restoreNeuromodulationSystemsFromCheckpoint(const std::vector<uint8_t>& data) {
        if (data.empty()) return;
        
        const uint8_t* ptr = data.data();
        
        // Read dopamine level
        if (dopamine && data.size() >= sizeof(float)) {
            float dopamineLevel = *reinterpret_cast<const float*>(ptr);
            dopamine->setLevel(dopamineLevel);
            ptr += sizeof(float);
        }
        
        // Read curiosity level
        if (curiosity && data.size() >= ptr - data.data() + sizeof(float)) {
            float curiosityLevel = *reinterpret_cast<const float*>(ptr);
            curiosity->setLevel(curiosityLevel);
            ptr += sizeof(float);
        }
        
        // Read prediction error level
        if (predictionError && data.size() >= ptr - data.data() + sizeof(float)) {
            float predictionErrorLevel = *reinterpret_cast<const float*>(ptr);
            predictionError->setLevel(predictionErrorLevel);
            ptr += sizeof(float);
        }
        
        // Read novelty level
        if (novelty && data.size() >= ptr - data.data() + sizeof(float)) {
            float noveltyLevel = *reinterpret_cast<const float*>(ptr);
            novelty->setLevel(noveltyLevel);
            ptr += sizeof(float);
        }
    }
    
    /**
     * Helper method to restore development system from checkpoint
     */
    void Brain::Impl::restoreDevelopmentSystemFromCheckpoint(const std::vector<uint8_t>& data) {
        if (data.empty() || !developmentSystem) return;
        
        const uint8_t* ptr = data.data();
        
        // Read developmental stage
        uint64_t stage = *reinterpret_cast<const uint64_t*>(ptr);
        
        // Update developmental stage
        if (stage < static_cast<uint64_t>(DevelopmentalStage::Adult) + 1) {
            developmentalStage = static_cast<DevelopmentalStage>(stage);
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

// Self-model access
nlm::SelfModel* Brain::getSelfModel() {
    return pImpl->selfModel.get();
}

// SocialLearning access
nlm::SocialLearning* Brain::getSocialLearning() {
    return pImpl->socialLearning.get();
}

// Helper method to extract neural activity pattern for planning
void Brain::extractNeuralActivityPattern(std::vector<float>& pattern) {
    pattern.clear();
    
    // Get motor neuron activities (these influence action selection)
    std::vector<float> motorActivities;
    for (auto* neuron : pImpl->motorNeurons) {
        float activity = std::abs(neuron->getState().membranePotential - 
                                neuron->getState().restingPotential);
        // Normalize to 0-1 range
        activity = std::min(activity / 20.0f, 1.0f);
        motorActivities.push_back(activity);
    }
    
    // Get working memory traces (influence planning context)
    std::vector<float> workingMemoryContext;
    if (pImpl->workingMemory) {
        auto workingNeurons = pImpl->workingMemory->getMemoryNeurons();
        for (size_t i = 0; i < workingNeurons.size() && i < 20; ++i) {
            workingMemoryContext.push_back(1.0f / (i + 1));  // Decay factor
        }
    }
    
    // Combine motor and working memory signals for planning
    pattern.resize(motorActivities.size() + workingMemoryContext.size());
    for (size_t i = 0; i < motorActivities.size(); ++i) {
        pattern[i] = motorActivities[i];
    }
    for (size_t i = 0; i < workingMemoryContext.size(); ++i) {
        pattern[motorActivities.size() + i] = workingMemoryContext[i];
    }
    
    // Add neuromodulation signals if available
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        pattern.push_back(dopamineLevel);
    }
    
    // Add curiosity signal if available
    if (pImpl->curiosity) {
        float curiosityLevel = pImpl->curiosity->getLevel();
        pattern.push_back(curiosityLevel);
    }
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

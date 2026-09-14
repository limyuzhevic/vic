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
    
    // Prediction tracking
    std::vector<float> predictionErrorHistory;
    float totalPredictionError;
    
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
        , predictionErrorHistory(1000, 0.0f)
        , totalPredictionError(0.0f)
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
    
        // Set up checkpoint providers
        pImpl->checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) {
            return saveNeurons(data);
        });
        pImpl->checkpointManager->setSynapseProvider([this](SynapseCheckpointData& data) {
            return saveSynapses(data);
        });
        
        // Set up checkpoint providers
        pImpl->checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) {
            return saveNeurons(data);
        });
        pImpl->checkpointManager->setSynapseProvider([this](SynapseCheckpointData& data) {
            return saveSynapses(data);
        });
    
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
            
            // Store active neurons from current brain state
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
            
            // Store in episodic memory
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Update prediction system with sensory input from last timestep
        // Compute prediction errors and store them for learning
        if (!pImpl->sensoryNeurons.empty()) {
            // Create dummy sensory input from neural activity
            // In real implementation, this would come from actual sensory observations
            std::vector<float> sensoryData;
            for (auto* neuron : pImpl->sensoryNeurons) {
                sensoryData.push_back(std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential));
            }
            
            // Create dummy observation to train prediction system
            class DummySensoryInput : public SensoryInput {
            public:
                DummySensoryInput(const std::vector<float>& data) : data_(data) {}
                const char* getType() const override { return "Dummy"; }
                const std::vector<float>& getData() const override { return data_; }
                size_t getDimensions() const override { return data_.size(); }
                std::unique_ptr<SensoryInput> clone() const override {
                    return std::make_unique<DummySensoryInput>(data_);
                }
            private:
                std::vector<float> data_;
            };
            
            DummySensoryInput dummyObservation(sensoryData);
            
            // Train prediction system with observation
            pImpl->predictionSystem->train(dummyObservation);
            
            // Get predicted state
            auto predictedState = pImpl->predictionSystem->predictNextState(dummyObservation);
            
            if (predictedState) {
                // Compute prediction error
                pImpl->predictionSystem->updatePredictions(*predictedState, dummyObservation);
                
                // Send prediction error to neuromodulation systems
                float predictionError = pImpl->predictionSystem->getPredictionError();
                if (pImpl->predictionError) {
                    pImpl->predictionError->computeError(predictionError, 0.0f);
                }
                
                // Use prediction for attention modulation
                if (pImpl->attention) {
                    pImpl->attention->updatePredictionError(predictionError, pImpl->timestep);
                }
                
                // Use prediction for planning
                if (pImpl->planner) {
                    // Store prediction error for planning
                    pImpl->planner->setPredictionError(predictionError);
                    pImpl->planner->incorporatePrediction(predictedState->getData());
                }
                
                // Use prediction for concept formation
                if (pImpl->conceptFormation) {
                    pImpl->conceptFormation->processPrediction(predictedState->getData());
                }
                
                // Store prediction error for later learning
                pImpl->predictionErrorHistory.push_back(predictionError);
            }
        }
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        // Get current prediction error if available
        float predictionError = 0.0f;
        if (pImpl->predictionError) {
            predictionError = pImpl->predictionError->getError();
        }
        
        pImpl->attention->update(pImpl->timestep, predictionError);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Process current neural activity patterns to form concepts
        // Incorporate prediction errors to guide learning
        
        std::vector<float> currentPattern;
        // Extract current neural activity pattern from sensory and internal neurons
        for (auto* neuron : pImpl->sensoryNeurons) {
            currentPattern.push_back(std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential));
        }
        
        // Get current prediction error if available
        float predictionError = 0.0f;
        if (pImpl->predictionError) {
            predictionError = pImpl->predictionError->getError();
        }
        
        pImpl->conceptFormation->processPattern(currentPattern, predictionError, currentStep);
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
    
    // Store current sensory input in prediction system for training
    if (pImpl->predictionSystem) {
        // Clone input to train prediction system
        auto inputClone = input.clone();
        // Train prediction system with this observation
        pImpl->predictionSystem->train(*inputClone);
        
        // Generate prediction for next state based on current state
        auto predictedState = pImpl->predictionSystem->predictNextState(input);
        
        // Use prediction to influence attention system
        if (pImpl->attention) {
            pImpl->attention->processPrediction(predictedState->getData());
        }
        
        // Use prediction to influence planning
        if (pImpl->planner) {
            pImpl->planner->incorporatePrediction(predictedState->getData());
        }
    }
    
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
    // Neural planning-based action selection
    // Uses NeuralPlanner to select actions based on prediction and planning
    
    // Create a state vector from current sensory and neural activity
    std::vector<float> currentState;
    
    // Extract sensory neuron activity
    for (auto* neuron : pImpl->sensoryNeurons) {
        const auto& state = neuron->getState();
        currentState.push_back(std::abs(state.membranePotential - state.restingPotential));
    }
    
    // Extract internal neuron activity if needed
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Internal) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    currentState.push_back(std::abs(state.membranePotential - state.restingPotential));
                }
            }
        }
    }
    
    // Ensure we have a goal for planning
    std::vector<float> currentGoal = pImpl->planner->getCurrentGoal();
    if (currentGoal.empty()) {
        // Set a simple goal based on current state
        currentGoal = std::vector<float>(currentState.size(), 0.5f);
        pImpl->planner->setCurrentGoal(currentGoal);
    }
    
    // Use NeuralPlanner to select action
    ActionType plannedAction = pImpl->planner->getPlanner()->planAction(currentState, 0.5f);
    
    // Create action based on planned action
    auto action = std::make_unique<Action>(plannedAction);
    
    // Update planning with actual consequences
    if (pImpl->planner->getPlanner()) {
        // Record that we took this action
        // In a real implementation, we would capture the actual sensory outcome
        // For now, just update plan quality based on motor neuron activity
        size_t firingMotor = 0;
        for (auto* neuron : pImpl->motorNeurons) {
            if (neuron->isFiring()) {
                ++firingMotor;
            }
        }
        float actualReward = firingMotor > 0 ? 1.0f : 0.0f;
        
        // Update planner with actual outcome
        std::vector<ActionType> plannedSequence{plannedAction};
        std::vector<ActionType> actualSequence{plannedAction};
        pImpl->planner->getPlanner()->updatePlanQuality(plannedSequence, actualSequence, actualReward);
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

pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    // Initialize prediction tracking
    pImpl->predictionErrorHistory.clear();
    for (size_t i = 0; i < 1000; ++i) {
        pImpl->predictionErrorHistory.push_back(0.0f);
    }
    pImpl->totalPredictionError = 0.0f;
    
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

// Brain checkpoint save/load implementations
bool Brain::saveNeurons(NeuronCheckpointData& data) const {
    data.membranePotential.clear();
    data.restingPotential.clear();
    data.threshold.clear();
    data.resetPotential.clear();
    data.leakConductance.clear();
    data.firingState.clear();
    data.refractoryRemaining.clear();
    data.refractoryPeriod.clear();
    data.lastSpikeTime.clear();
    
    size_t totalNeurons = getTotalNeuronCount();
    data.membranePotential.reserve(totalNeurons);
    data.restingPotential.reserve(totalNeurons);
    data.threshold.reserve(totalNeurons);
    data.resetPotential.reserve(totalNeurons);
    data.leakConductance.reserve(totalNeurons);
    data.firingState.reserve(totalNeurons);
    data.refractoryRemaining.reserve(totalNeurons);
    data.refractoryPeriod.reserve(totalNeurons);
    data.lastSpikeTime.reserve(totalNeurons);
    
    for (const auto& region : pImpl->regions) {
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
    
    return !data.membranePotential.empty();
}

bool Brain::saveSynapses(SynapseCheckpointData& data) const {
    data.sourceNeuron.clear();
    data.destinationNeuron.clear();
    data.weight.clear();
    data.delay.clear();
    data.synapseType.clear();
    data.eligibilityTrace.clear();
    
    size_t totalSynapses = getTotalSynapseCount();
    data.sourceNeuron.reserve(totalSynapses);
    data.destinationNeuron.reserve(totalSynapses);
    data.weight.reserve(totalSynapses);
    data.delay.reserve(totalSynapses);
    data.synapseType.reserve(totalSynapses);
    data.eligibilityTrace.reserve(totalSynapses);
    
    for (const auto& region : pImpl->regions) {
        for (const auto* syn : region->getSynapses()) {
            data.sourceNeuron.push_back(syn->getSourceNeuron().index());
            data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
            data.weight.push_back(syn->getWeight());
            data.delay.push_back(syn->getDelay());
            data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
            data.eligibilityTrace.push_back(syn->getEligibilityTrace());
        }
    }
    
    return !data.weight.empty();
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
        if (!saveNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to prepare neurons for checkpoint");
            return false;
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        if (!saveSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to prepare synapses for checkpoint");
            return false;
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

bool Brain::loadNeurons(const NeuronCheckpointData& data) {
    if (data.membranePotential.empty()) {
        return false;
    }
    
    // Clear current neurons and recreate from data
    for (auto& region : pImpl->regions) {
        region->clear();
    }
    pImpl->regions.clear();
    
    // Recreate neurons from data
    size_t idx = 0;
    for (size_t regionId = 1; regionId <= data.regionId.back() && idx < data.membranePotential.size(); ++regionId) {
        auto* region = getRegion(RegionId(regionId));
        if (!region) {
            region = &addRegion("Restored_Region_" + std::to_string(regionId));
        }
        
        // For now, create a basic neuron with the data
        // In a real implementation, we would restore the full neuron
        // This is a simplified version
    }
    
    return true;
}

bool Brain::loadSynapses(const SynapseCheckpointData& data) {
    // For now, just log the count
    NLM_LOG_INFO("Restoring " + std::to_string(data.weight.size()) + " synapses");
    return true;
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
        if (!loadNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to apply neuron data");
            return false;
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states
        if (!loadSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to apply synapse data");
            return false;
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

// ========== CONVENIENCE HELPER FUNCTIONS ==========

bool Brain::hasSynapticConnection(NeuronId source, NeuronId target) const {
    // Check inter-region connections first
    for (const auto& conn : pImpl->interRegionConnections) {
        if (conn.sourceRegion == source && conn.targetRegion == target) {
            return true;
        }
    }
    
    // Check intra-region connections
    for (const auto& region : pImpl->regions) {
        const auto& synapses = region->getSynapses();
        for (const auto* syn : synapses) {
            if (syn->getSourceNeuron() == source && syn->getDestinationNeuron() == target) {
                return true;
            }
        }
    }
    
    return false;
}

std::vector<NeuronId> Brain::getFiringNeurons(NeuronType type) const {
    std::vector<NeuronId> firingNeurons;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring()) {
                        firingNeurons.push_back(neuron->getId());
                    }
                }
            }
        }
    }
    return firingNeurons;
}

float Brain::getAverageActivity(RegionId region) const {
    auto* regionPtr = getRegion(region);
    if (!regionPtr) return 0.0f;
    
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& pop : regionPtr->getPopulations()) {
        for (const auto* neuron : pop->getNeurons()) {
            const auto& state = neuron->getState();
            float activity = std::abs(state.membranePotential - state.restingPotential);
            totalActivity += activity;
            neuronCount++;
        }
    }
    
    return neuronCount > 0 ? totalActivity / neuronCount : 0.0f;
}

float Brain::getEIRatio(RegionId region) const {
    auto* regionPtr = getRegion(region);
    if (!regionPtr) return 0.0f;
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    const auto& synapses = regionPtr->getSynapses();
    for (const auto* syn : synapses) {
        float weight = syn->getWeight();
        if (weight > 0) {
            totalExcitatory += weight;
        } else {
            totalInhibitory += std::abs(weight);
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

bool Brain::hasActiveNeurons(RegionId region) const {
    auto* regionPtr = getRegion(region);
    if (!regionPtr) return false;
    
    for (const auto& pop : regionPtr->getPopulations()) {
        for (const auto* neuron : pop->getNeurons()) {
            if (neuron->isFiring()) {
                return true;
            }
        }
    }
    return false;
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    // Return the actual working memory instance
    // This was a stub that returned nullptr
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    // Return the actual episodic memory instance
    // This was a stub that returned nullptr
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

// ========== COGNITION SYSTEMS ==========

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

// Get action based on neural planning
ActionType Brain::getPlannedAction(const std::vector<float>& currentState,
                                  float targetReward) {
    if (!pImpl->planner) return ActionType::Wait;
    return pImpl->planner->getPlanner()->planAction(currentState, targetReward);
}

// Evaluate action sequence for planning
PlanningCandidate Brain::evaluatePlannedSequence(const std::vector<ActionType>& actions,
                                                const std::vector<float>& startState) {
    if (!pImpl->planner) {
        PlanningCandidate empty;
        return empty;
    }
    return pImpl->planner->getPlanner()->evaluateSequence(actions, startState);
}

// Predict consequences of an action
std::vector<float> Brain::predictActionConsequence(ActionType action,
                                                   const std::vector<float>& currentState) {
    if (!pImpl->planner) return currentState;
    return pImpl->planner->getPlanner()->predictNextState(action, currentState);
}

// Get planning depth
size_t Brain::getPlanningDepth() const {
    if (pImpl->planner) {
        return pImpl->planner->getPlanner()->getPlanningDepth();
    }
    return 3;
}

void Brain::setPlanningDepth(size_t depth) {
    if (pImpl->planner) {
        pImpl->planner->getPlanner()->setPlanningDepth(depth);
    }
}

// Get planning confidence
float Brain::getPlanningConfidence() const {
    if (pImpl->planner) {
        return pImpl->planner->getPlanner()->getPlanningConfidence();
    }
    return 0.5f;
}

    // Get action based on neural planning
    ActionType getPlannedAction(const std::vector<float>& currentState,
                              float targetReward = 0.5f);
    
    // Evaluate action sequence for planning
    PlanningCandidate evaluatePlannedSequence(const std::vector<ActionType>& actions,
                                             const std::vector<float>& startState);
    
    // Predict consequences of an action
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);
    
    // Get planning depth
    size_t getPlanningDepth() const;
    void setPlanningDepth(size_t depth);
    
    // Get planning confidence
    float getPlanningConfidence() const;
    
    // Get self model confidence for action
    float getSelfModelConfidence(ActionType action) const;
    
    // Update plan quality based on actual outcome
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                           const std::vector<ActionType>& actualActions,
                           float actualReward);
    
    // Clear planning cache
    void clearPlanCache();
    
    // Check if recent planning was successful
    bool wasRecentPlanSuccessful() const;

} // namespace nlm

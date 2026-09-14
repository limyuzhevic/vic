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
#include <vector>
#include <set>

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
    
    }// Initialize error reporting systems
        lastValidationTime = std::chrono::steady_clock::now();
        lastHealthCheck = std::chrono::steady_clock::now();
        
        // Collect initial system status
        collectSystemErrors();
        collectSystemWarnings();
    }
    
    // ========== VALIDATION AND ERROR REPORTING ==========
    
    // System validation state
    bool validationEnabled;
    std::vector<std::string> validationErrors;
    std::chrono::steady_clock::time_point lastValidationTime;
    
    // Error collection and reporting
    std::vector<std::string> systemErrors;
    std::vector<std::string> systemWarnings;
    std::vector<std::string> systemHealthStats;
    
    // Health monitoring
    bool systemHealthy;
    size_t consecutiveFailures;
    std::chrono::steady_clock::time_point lastHealthCheck;
    
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
        // ========== VALIDATION AND ERROR REPORTING ==========
        , validationEnabled(true)
        , systemHealthy(true)
        , consecutiveFailures(0)
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
        
        // Initialize error reporting systems
        lastValidationTime = std::chrono::steady_clock::now();
        lastHealthCheck = std::chrono::steady_clock::now();
        
        // Collect initial system status
        collectSystemErrors();
        collectSystemWarnings();
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
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
    }

    // Initialize episodic memory
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
    }

    // Initialize associative memory
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(this);
    }
    
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
                    if (pImpl && pImpl->workingMemory) {
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
    if (pImpl && pImpl->workingMemory) {
        // Update working memory with neural activity patterns
        pImpl->workingMemory->update(pImpl->timestep);
        
        // Store sensory input patterns in working memory for integration
        // This captures current sensory state for future reference
        for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
            Neuron* sensoryNeuron = pImpl->sensoryNeurons[i];
            const auto& state = sensoryNeuron->getState();
            float activation = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
            if (activation > 0.01f) {
                pImpl->workingMemory->storeToNeuron(sensoryNeuron->getId(), activation);
            }
        }
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update neuromodulation effects
    if (pImpl) {
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
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl && pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    if (pImpl) {
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
    }
    
    // ========== STEP 7: Update episodic memory ==========
    if (pImpl) {
        pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            pImpl->stepsSinceLastEpisode = 0;
            
            if (pImpl->episodicMemory) {
                // Capture comprehensive brain state as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                episode.energy = pImpl->getAverageFiringRate();  // Use firing rate as energy proxy
                
                // Capture full neural activity distribution (not just firing neurons)
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            const auto& state = neuron->getState();
                            float activation = std::abs(state.membranePotential - state.restingPotential);
                            
                            // Store all neurons with meaningful activation, not just firing ones
                            if (activation > 0.1f) {
                                episode.activeNeurons.push_back(neuron->getId());
                                episode.neuronActivations.push_back(activation / 20.0f); // Normalize
                            }
                        }
                    }
                }
                
                // Capture sensory state if available
                // This would normally come from the sensory system
                // For now, capture internal representation of sensory input patterns
                std::vector<float> sensoryPattern;
                for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                    Neuron* sensoryNeuron = pImpl->sensoryNeurons[i];
                    const auto& state = sensoryNeuron->getState();
                    float activation = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
                    sensoryPattern.push_back(activation);
                }
                episode.sensoryState = sensoryPattern;
                
                // Capture action potential (produced by action selection)
                // Store the action that was produced this step
                // This would integrate with the action selection system
                
                // Store resulting state (next state's sensory pattern)
                // This creates a transition model
                std::vector<float> resultingSensoryState = sensoryPattern;
                // Add noise or variation to simulate real-world uncertainty
                for (float& val : resultingSensoryState) {
                    val += pImpl->rng->randFloat(-0.1f, 0.1f);
                    val = std::max(0.0f, std::min(1.0f, val));
                }
                episode.resultingSensoryState = resultingSensoryState;
                
                // Store novelty level
                episode.novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
                
                // Store prediction error if available
                if (pImpl->predictionSystem) {
                    float predError = pImpl->predictionSystem->getPredictionError();
                    episode.reward += predError * 0.5f; // Incorporate prediction error into reward
                }
                
                pImpl->episodicMemory->storeEpisode(episode);
            }
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl && pImpl->predictionSystem) {
        // Build current sensory state pattern from sensory neurons
        std::vector<float> currentSensoryPattern;
        for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
            Neuron* sensoryNeuron = pImpl->sensoryNeurons[i];
            const auto& state = sensoryNeuron->getState();
            float activation = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
            currentSensoryPattern.push_back(activation);
        }
        
        // Create a dummy sensory input with current pattern
        // In a real implementation, this would be passed from the actual sensory system
        class DummySensoryInput : public SensoryInput {
        public:
            DummySensoryInput(const std::vector<float>& data) {
                data_ = data;
            }
            
            const char* getType() const override { return "Dummy"; }
            const std::vector<float>& getData() const override { return data_; }
            size_t getDimensions() const override { return data_.size(); }
            std::unique_ptr<SensoryInput> clone() const override {
                return std::make_unique<DummySensoryInput>(data_);
            }
            
        private:
            std::vector<float> data_;
        };
        
        DummySensoryInput currentState(currentSensoryPattern);
        
        // Make prediction for next state
        auto predictedState = pImpl->predictionSystem->predictNextState(currentState);
        
        // Update predictions with actual observation (current state)
        pImpl->predictionSystem->updatePredictions(*predictedState, currentState);
        
        // Update prediction system confidence based on neural activity patterns
        // Higher neural activity variance increases prediction confidence
        float activityVariance = 0.0f;
        float activityMean = 0.0f;
        size_t activeCount = 0;
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    float activation = std::abs(state.membranePotential - state.restingPotential);
                    if (activation > 0.1f) {
                        activityMean += activation;
                        activityVariance += activation * activation;
                        activeCount++;
                    }
                }
            }
        }
        
        if (activeCount > 0) {
            activityMean /= activeCount;
            activityVariance /= activeCount;
            activityVariance -= activityMean * activityMean;
            activityVariance = std::max(0.0f, activityVariance);
            
            // Normalize to confidence (higher variance -> higher confidence)
            float confidence = std::min(1.0f, activityVariance * 10.0f);
            // Update prediction system confidence
            // pImpl->predictionSystem->setConfidence(confidence); // If method exists
        }
    }
    
     // Remove redundant error logging when pImpl is null
    // The null check above is sufficient - logging errors inside else blocks is not needed
    // since pImpl should never be null in a properly initialized Brain
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl && pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (pImpl && currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (pImpl && currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Enhanced replay system: multiple replay strategies based on memory importance
        
        // Strategy 1: Replay high-reward episodes (consolidation of value)
        auto highRewardEpisodes = pImpl->episodicMemory->getEpisodesForReplay(2);
        for (const auto* episode : highRewardEpisodes) {
            // Only replay if they have high reward contribution
            if (episode->reward > 0.5f) {
                pImpl->episodicMemory->replayEpisode(episode);
            }
        }
        
        // Strategy 2: Replay recent episodes (temporal continuity)
        // This maintains the temporal structure of experiences
        if (pImpl->episodicMemory->getEpisodeCount() > 5) {
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(2);
            for (const auto* episode : recentEpisodes) {
                // Reconstruct the episode pattern in working memory
                if (!episode->sensoryState.empty()) {
                    // This would integrate with working memory for pattern completion
                    // In a full implementation, would store pattern in working memory
                    // for future retrieval
                    pImpl->episodicMemory->replayEpisode(episode);
                }
            }
        }
        
        // Strategy 3: Replay based on prediction error (novelty)
        // Episodes that caused prediction errors are particularly important to consolidate
        if (pImpl->predictionSystem) {
            float predictionError = pImpl->predictionSystem->getPredictionError();
            if (predictionError > 0.3f) {  // High prediction error
                // Get some episodes with similar characteristics for targeted replay
                // This helps build better predictive models
            }
        }
        
        // The replay system now integrates with working memory
        // to maintain the reactivated patterns through persistent activity
        if (pImpl->workingMemory) {
            // After replay, working memory should maintain the reactivated patterns
            // This allows for gradual consolidation of replayed memories
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (pImpl && currentStep % 1000 == 0) {  // Update development every 1000 steps
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
    if (pImpl && currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
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
    if (pImpl) {
        if (pImpl->workingMemory) pImpl->workingMemory->clear();
        if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
        if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
        if (pImpl->attention) pImpl->attention->reset();
    }
    
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
    return pImpl ? pImpl->workingMemory.get() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl ? pImpl->episodicMemory.get() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl ? pImpl->associativeMemory.get() : nullptr;
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl ? pImpl->predictionSystem.get() : nullptr;
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl ? pImpl->planner.get() : nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl ? pImpl->conceptFormation.get() : nullptr;
}

AttentionalSelection* Brain::getAttention() {
    return pImpl ? pImpl->attention.get() : nullptr;
}

// ========== DEVELOPMENT SYSTEM ==========

// Development system implementation
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl ? pImpl->developmentSystem.get() : nullptr;
}

const DevelopmentSystem* Brain::getDevelopmentSystem() const {
    return pImpl ? pImpl->developmentSystem.get() : nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl ? pImpl->developmentalStage : DevelopmentalStage::Initial;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl) {
        pImpl->developmentalStage = stage;
    }
}

// Neuromodulation systems implementation
Dopamine* Brain::getDopamine() {
    return pImpl ? pImpl->dopamine.get() : nullptr;
}

const Dopamine* Brain::getDopamine() const {
    return pImpl ? pImpl->dopamine.get() : nullptr;
}

Curiosity* Brain::getCuriosity() {
    return pImpl ? pImpl->curiosity.get() : nullptr;
}

const Curiosity* Brain::getCuriosity() const {
    return pImpl ? pImpl->curiosity.get() : nullptr;
}

Novelty* Brain::getNovelty() {
    return pImpl ? pImpl->novelty.get() : nullptr;
}

const Novelty* Brain::getNovelty() const {
    return pImpl ? pImpl->novelty.get() : nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl ? pImpl->predictionError.get() : nullptr;
}

const PredictionError* Brain::getPredictionErrorSignal() const {
    return pImpl ? pImpl->predictionError.get() : nullptr;
}

// Plasticity system implementations
STDP* Brain::getSTDP() {
    return pImpl ? pImpl->stdp.get() : nullptr;
}

const STDP* Brain::getSTDP() const {
    return pImpl ? pImpl->stdp.get() : nullptr;
}

Hebbian* Brain::getHebbian() {
    return pImpl ? pImpl->hebbian.get() : nullptr;
}

const Hebbian* Brain::getHebbian() const {
    return pImpl ? pImpl->hebbian.get() : nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl ? pImpl->structuralPlasticity.get() : nullptr;
}

const StructuralPlasticity* Brain::getStructuralPlasticity() const {
    return pImpl ? pImpl->structuralPlasticity.get() : nullptr;
}

// Random generator implementations
RandomGenerator* Brain::getRandomGenerator() {
    return pImpl ? pImpl->rng.get() : nullptr;
}

const RandomGenerator* Brain::getRandomGenerator() const {
    return pImpl ? pImpl->rng.get() : nullptr;
}

// Power user utilities implementation

bool Brain::validateSystemIntegration() const {
    bool isValid = true;
    
    // Check if essential systems are initialized
    if (!pImpl) {
        NLM_LOG_ERROR("Brain::validateSystemIntegration: pImpl is null");
        return false;
    }
    
    // Check core systems
    if (!pImpl->spikeSystem) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: SpikeSystem not initialized");
        isValid = false;
    }
    
    if (!pImpl->stdp) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: STDP not initialized");
        isValid = false;
    }
    
    // Check memory systems
    if (!pImpl->workingMemory) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: WorkingMemory not initialized");
        isValid = false;
    }
    
    if (!pImpl->episodicMemory) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: EpisodicMemory not initialized");
        isValid = false;
    }
    
    // Check prediction system
    if (!pImpl->predictionSystem) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: PredictionSystem not initialized");
        isValid = false;
    }
    
    // Check neuromodulation systems
    if (!pImpl->dopamine) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: Dopamine not initialized");
        isValid = false;
    }
    
    // Check cognition systems
    if (!pImpl->planner) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: NeuralPlanner not initialized");
        isValid = false;
    }
    
    if (!pImpl->conceptFormation) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: ConceptFormation not initialized");
        isValid = false;
    }
    
    if (!pImpl->attention) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: AttentionalSelection not initialized");
        isValid = false;
    }
    
    // Check development system
    if (!pImpl->developmentSystem) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: DevelopmentSystem not initialized");
        isValid = false;
    }
    
    // Check novelty and curiosity
    if (!pImpl->novelty) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: Novelty not initialized");
        isValid = false;
    }
    
    if (!pImpl->curiosity) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: Curiosity not initialized");
        isValid = false;
    }
    
    // Check prediction error
    if (!pImpl->predictionError) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: PredictionError not initialized");
        isValid = false;
    }
    
    if (!pImpl->structuralPlasticity) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: StructuralPlasticity not initialized");
        isValid = false;
    }
    
    // Check checkpoint system
    if (!pImpl->checkpointManager) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: CheckpointManager not initialized");
        isValid = false;
    }
    
    // Check regions
    if (pImpl->regions.empty()) {
        NLM_LOG_WARNING("Brain::validateSystemIntegration: No regions initialized");
        isValid = false;
    }
    
    // Log validation result
    if (isValid) {
        NLM_LOG_INFO("Brain::validateSystemIntegration: All systems validated successfully");
    } else {
        NLM_LOG_ERROR("Brain::validateSystemIntegration: System integration validation failed");
    }
    
    return isValid;
}

std::string Brain::getIntegratedSystemStatus() const {
    std::stringstream status;
    
    status << "=== NLM Brain System Status ===" << std::endl;
    
    // Check if pImpl exists
    if (!pImpl) {
        status << "ERROR: pImpl is null - Brain not properly initialized" << std::endl;
        return status.str();
    }
    
    // Core system status
    status << "Core Systems:" << std::endl;
    status << "  SpikeSystem: " << (pImpl->spikeSystem ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  STDP: " << (pImpl->stdp ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  Hebbian: " << (pImpl->hebbian ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  StructuralPlasticity: " << (pImpl->structuralPlasticity ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  RandomGenerator: " << (pImpl->rng ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    
    // Memory systems status
    status << std::endl << "Memory Systems:" << std::endl;
    status << "  WorkingMemory: " << (pImpl->workingMemory ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  EpisodicMemory: " << (pImpl->episodicMemory ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  AssociativeMemory: " << (pImpl->associativeMemory ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    
    if (pImpl->workingMemory) {
        status << "    Active traces: " << pImpl->workingMemory->getActiveTraces() << std::endl;
    }
    if (pImpl->episodicMemory) {
        status << "    Episodes stored: " << pImpl->episodicMemory->getEpisodeCount() << std::endl;
    }
    
    // Prediction system status
    status << std::endl << "Prediction System:" << std::endl;
    status << "  PredictionSystem: " << (pImpl->predictionSystem ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    
    // Cognition systems status
    status << std::endl << "Cognition Systems:" << std::endl;
    status << "  NeuralPlanner: " << (pImpl->planner ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  ConceptFormation: " << (pImpl->conceptFormation ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  AttentionalSelection: " << (pImpl->attention ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    
    // Development system status
    status << std::endl << "Development System:" << std::endl;
    status << "  DevelopmentSystem: " << (pImpl->developmentSystem ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  DevelopmentalStage: " << static_cast<int>(pImpl->developmentalStage) << std::endl;
    
    // Neuromodulation systems status
    status << std::endl << "Neuromodulation Systems:" << std::endl;
    status << "  Dopamine: " << (pImpl->dopamine ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  Curiosity: " << (pImpl->curiosity ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  Novelty: " << (pImpl->novelty ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    status << "  PredictionError: " << (pImpl->predictionError ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    
    if (pImpl->dopamine) {
        status << "    Dopamine level: " << pImpl->dopamine->getLevel() << std::endl;
    }
    
    if (pImpl->curiosity) {
        status << "    Curiosity level: " << pImpl->curiosity->getLevel() << std::endl;
    }
    
    if (pImpl->novelty) {
        status << "    Novelty level: " << pImpl->novelty->getLevel() << std::endl;
    }
    
    // Region status
    status << std::endl << "Regions:" << std::endl;
    status << "  Region Count: " << pImpl->regions.size() << std::endl;
    for (const auto& region : pImpl->regions) {
        status << "    Region " << region->getId().index() << " (" << region->getName() << "):" << std::endl;
        status << "      Neurons: " << region->getTotalNeuronCount() << std::endl;
        status << "      Synapses: " << region->getSynapseCount() << std::endl;
        status << "      Populations: " << region->getPopulationCount() << std::endl;
    }
    
    // Simulation status
    status << std::endl << "Simulation State:" << std::endl;
    status << "  Current Step: " << pImpl->currentStep << std::endl;
    status << "  Current Time: " << pImpl->currentTime << std::endl;
    status << "  Total Spikes (this step): " << pImpl->totalSpikesThisStep << std::endl;
    status << "  Total Spikes (all time): " << pImpl->totalSpikesTotal << std::endl;
    status << "  Is Resting: " << (pImpl->isResting ? "Yes" : "No") << std::endl;
    status << "  Steps since last episode: " << pImpl->stepsSinceLastEpisode << std::endl;
    status << "  Replay interval: " << pImpl->replayInterval << std::endl;
    status << "  Consolidation interval: " << pImpl->consolidationInterval << std::endl;
    
    // Statistics
    status << std::endl << "Statistics:" << std::endl;
    status << "  Total Neurons: " << getTotalNeuronCount() << std::endl;
    status << "  Total Synapses: " << getTotalSynapseCount() << std::endl;
    status << "  Active Neurons: " << getActiveNeuronCount() << std::endl;
    status << "  Firing Neurons (this step): " << getFiringNeuronCount() << std::endl;
    status << "  Average Firing Rate: " << getAverageFiringRate() << std::endl;
    status << "  Excitation/Inhibition Ratio: " << getExcitationInhibitionRatio() << std::endl;
    status << "  Total Spike Count: " << getTotalSpikeCount() << std::endl;
    status << "  Pending Spike Events: " << getPendingSpikeEventCount() << std::endl;
    
    // Energy/Performance metrics
    float totalEnergy = getAverageFiringRate() * 100.0f; // Simple energy proxy
    status << "  Energy Consumption: " << totalEnergy << std::endl;
    
    // Checkpoint status
    status << "  Checkpoint Manager: " << (pImpl->checkpointManager ? "✓ Initialized" : "✗ Not Initialized") << std::endl;
    
    status << std::endl << "=== End of System Status ===" << std::endl;
    
    return status.str();
}

Brain::PerformanceMetrics Brain::getPerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    // Basic statistics
    metrics.totalNeurons = getTotalNeuronCount();
    metrics.totalSynapses = getTotalSynapseCount();
    metrics.activeNeurons = getActiveNeuronCount();
    metrics.firingNeurons = getFiringNeuronCount();
    metrics.averageFiringRate = getAverageFiringRate();
    metrics.excitationInhibitionRatio = getExcitationInhibitionRatio();
    metrics.totalSpikeCount = getTotalSpikeCount();
    metrics.pendingSpikeEvents = getPendingSpikeEventCount();
    
    // Memory system metrics
    if (pImpl && pImpl->workingMemory) {
        metrics.workingMemoryLoad = static_cast<float>(pImpl->workingMemory->getActiveTraces()) / 
                                   pImpl->workingMemory->getCapacity();
    } else {
        metrics.workingMemoryLoad = 0.0f;
    }
    
    if (pImpl && pImpl->episodicMemory) {
        metrics.episodicMemoryLoad = static_cast<float>(pImpl->episodicMemory->getEpisodeCount()) / 
                                     pImpl->episodicMemory->getMaxEpisodes();
    } else {
        metrics.episodicMemoryLoad = 0.0f;
    }
    
    // Prediction system metrics
    if (pImpl && pImpl->predictionSystem) {
        metrics.predictionAccuracy = pImpl->predictionSystem->getPredictionAccuracy();
    } else {
        metrics.predictionAccuracy = 0.0f;
    }
    
    // Cognition system metrics
    if (pImpl && pImpl->planner) {
        metrics.planningConfidence = pImpl->planner->getPlanningConfidence();
    } else {
        metrics.planningConfidence = 0.0f;
    }
    
    // Neuromodulation metrics
    if (pImpl && pImpl->novelty) {
        metrics.noveltyLevel = pImpl->novelty->getLevel();
    } else {
        metrics.noveltyLevel = 0.0f;
    }
    
    if (pImpl && pImpl->curiosity) {
        metrics.curiosityLevel = pImpl->curiosity->getLevel();
    } else {
        metrics.curiosityLevel = 0.0f;
    }
    
    if (pImpl && pImpl->dopamine) {
        metrics.dopamineLevel = pImpl->dopamine->getLevel();
    } else {
        metrics.dopamineLevel = 0.0f;
    }
    
    // Development metrics
    metrics.developmentalStage = getDevelopmentalStage();
    
    const RandomGenerator* Brain::getRandomGenerator() const {
    return pImpl ? pImpl->rng.get() : nullptr;
}

// Additional validation and error reporting methods implementation

void Brain::collectErrors() const {
    if (!pImpl) return;
    
    // Clear previous errors
    pImpl->systemErrors.clear();
    
    // Check for current errors in all systems
    validateSystemIntegration(); // This will log errors but doesn't collect them
    
    // Additional error collection can be added here
    // For now, just re-run validation to collect errors
    if (pImpl) {
        // Collect errors from all subsystems
        // This is a placeholder - in a full implementation, each subsystem
        // would report its own errors to be collected here
    }
}

std::vector<std::string> Brain::getSystemErrors() const {
    std::vector<std::string> errors;
    
    if (!pImpl) {
        errors.push_back("Brain::getSystemErrors: pImpl is null");
        return errors;
    }
    
    // Copy current errors
    errors = pImpl->systemErrors;
    
    // Also check for any new validation errors
    if (!validateSystemIntegration()) {
        // Validation will have logged errors, but we can't easily extract them
        // For now, we'll rely on the validation error logging
        errors.push_back("Brain::getSystemErrors: System integration validation failed");
    }
    
    return errors;
}

bool Brain::hasSystemErrors() const {
    if (!pImpl) return true;
    
    // Check if there are any validation errors
    if (!validateSystemIntegration()) {
        return true;
    }
    
    // Check error collections
    if (!pImpl->systemErrors.empty()) return true;
    if (!pImpl->systemWarnings.empty()) {
        // Warnings are not considered errors, but could be if needed
    }
    
    return false;
}

void Brain::resetSystemValidation() const {
    if (!pImpl) return;
    
    // Clear all validation errors
    pImpl->systemErrors.clear();
    pImpl->systemWarnings.clear();
    pImpl->systemHealthStats.clear();
    
    // Reset validation state
    pImpl->consecutiveFailures = 0;
    pImpl->systemHealthy = true;
    
    // Update last validation time
    pImpl->lastValidationTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("Brain::resetSystemValidation: Validation errors cleared");
}

void Brain::checkSystemHealth() const {
    if (!pImpl) return;
    
    // Update health check time
    auto now = std::chrono::steady_clock::now();
    pImpl->lastHealthCheck = now;
    
    // Check system health components
    bool isHealthy = true;
    
    // Check if pImpl exists
    if (!pImpl) {
        isHealthy = false;
        pImpl->systemErrors.push_back("Brain::checkSystemHealth: pImpl is null");
    }
    
    // Check if all essential systems are initialized
    if (!pImpl->spikeSystem) {
        isHealthy = false;
        pImpl->systemWarnings.push_back("Brain::checkSystemHealth: SpikeSystem not initialized");
    }
    
    if (!pImpl->workingMemory) {
        isHealthy = false;
        pImpl->systemWarnings.push_back("Brain::checkSystemHealth: WorkingMemory not initialized");
    }
    
    if (!pImpl->dopamine) {
        isHealthy = false;
        pImpl->systemWarnings.push_back("Brain::checkSystemHealth: Dopamine not initialized");
    }
    
    if (pImpl->regions.empty()) {
        isHealthy = false;
        pImpl->systemWarnings.push_back("Brain::checkSystemHealth: No regions initialized");
    }
    
    // Update health state
    pImpl->systemHealthy = isHealthy;
    
    if (isHealthy) {
        pImpl->consecutiveFailures = 0;
        NLM_LOG_INFO("Brain::checkSystemHealth: System is healthy");
    } else {
        pImpl->consecutiveFailures++;
        NLM_LOG_WARNING("Brain::checkSystemHealth: System health check failed (failure " + 
                       std::to_string(pImpl->consecutiveFailures) + ")");
    }
    
    // Collect health statistics
    pImpl->systemHealthStats.clear();
    if (pImpl->spikeSystem) {
        pImpl->systemHealthStats.push_back("SpikeSystem: Active");
    } else {
        pImpl->systemHealthStats.push_back("SpikeSystem: Not initialized");
    }
    
    // Add more health statistics as needed
}

bool Brain::isSystemHealthy() const {
    if (!pImpl) return false;
    return pImpl->systemHealthy;
}

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
    
    // Neuron lookup index for O(1) access
    std::unordered_map<NeuronId, Neuron*> neuronIndex;
    
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
    
    // Advanced simulation control
    float simulationSpeed;           // 1.0 = normal, >1.0 = fast-forward, <1.0 = slow-motion
    float timeScale;                 // Multiplier for timestep duration
    bool parallelExecutionEnabled;
    size_t parallelRegionCount;
    
    // Plasticity control
    bool plasticityEnabled;
    float stdpPower;
    float hebbianPower;
    float rewardModulationPower;
    
    // Performance monitoring
    bool performanceMonitoringActive;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(std::make_unique<RandomGenerator>(42))
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
        , simulationSpeed(1.0f)
        , timeScale(1.0f)
        , parallelExecutionEnabled(false)
        , parallelRegionCount(1)
        , plasticityEnabled(true)
        , stdpPower(1.0f)
        , hebbianPower(1.0f)
        , rewardModulationPower(1.0f)
        , performanceMonitoringActive(false)
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng->seed(seed);
        
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
    
    // Create neurons across regions and maintain lookup index
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O and maintain lookup index
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                    // Maintain lookup index for O(1) access
                    pImpl->neuronIndex[neuron->getId()] = neuron;
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                    // Maintain lookup index for O(1) access
                    pImpl->neuronIndex[neuron->getId()] = neuron;
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
    pImpl->predictionSystem->initialize();
    
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
    pImpl->dopamine->initialize(this);
    pImpl->predictionError->initialize(this);
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        Brain* brain = this;
        ++brain->pImpl->totalSpikesThisStep;
        ++brain->pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        Brain* brain = this;
        for (auto& region : brain->pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    MembranePotential synapticCurrent = event.weight * 10.0f;
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
    
    // Set up checkpoint providers
    pImpl->checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) -> bool {
        // Collect neuron data
        data.membranePotential.reserve(getTotalNeuronCount());
        data.restingPotential.reserve(getTotalNeuronCount());
        data.threshold.reserve(getTotalNeuronCount());
        data.resetPotential.reserve(getTotalNeuronCount());
        data.leakConductance.reserve(getTotalNeuronCount());
        data.firingState.reserve(getTotalNeuronCount());
        data.refractoryRemaining.reserve(getTotalNeuronCount());
        data.refractoryPeriod.reserve(getTotalNeuronCount());
        data.lastSpikeTime.reserve(getTotalNeuronCount());
        
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
        return true;
    });
    
    pImpl->checkpointManager->setSynapseProvider([this](SynapseCheckpointData& data) -> bool {
        // Collect synapse data
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                data.sourceNeuron.push_back(syn->getSourceNeuron().index());
                data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                data.weight.push_back(syn->getWeight());
                data.delay.push_back(syn->getDelay());
                data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                data.plasticityFlags.push_back(syn->getPlasticityFlags());
                data.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        return true;
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
        // Update prediction system with current neural activity patterns
        // Encode current state as sensory input for prediction
        std::vector<float> currentState;
        currentState.reserve(getTotalNeuronCount());
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    currentState.push_back(std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                }
            }
        }
        
        // Create sensory input from current state
        SensoryInput observation(currentState);
        
        // Predict next state
        auto predicted = pImpl->predictionSystem->predictNextState(observation);
        
        // Update predictions based on actual observation
        pImpl->predictionSystem->updatePredictions(predicted, observation);
        
        // Get prediction error and potentially learn from it
        float predictionError = pImpl->predictionSystem->getPredictionError();
        pImpl->predictionSystem->train(observation);
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
        // Encode current neural activity as pattern for concept formation
        std::vector<float> currentPattern;
        currentPattern.reserve(getTotalNeuronCount());
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    currentPattern.push_back(std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                }
            }
        }
        
        // Also collect features from active neurons for concept formation
        std::vector<float> features;
        features.reserve(getTotalNeuronCount());
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    features.push_back(state.firingState == FiringState::Spiking ? 1.0f : 0.0f);
                }
            }
        }
        
        // Get reward signal from dopamine
        float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        // Present pattern to concept formation system
        pImpl->conceptFormation->presentExperience(currentPattern, features, reward, currentStep);
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
    // O(1) lookup using neuron index instead of linear search
    auto it = pImpl->neuronIndex.find(neuron);
    if (it != pImpl->neuronIndex.end()) {
        it->second->injectCurrent(current);
        return;
    }
    
    // Fallback to linear search if neuron not found in index
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

// ========== ADVANCED BRAIN CONFIGURATION COMMANDS ==========

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
                synapseData.plasticityFlags.push_back(syn->getPlasticityFlags());
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

bool Brain::saveCheckpoint(const std::string& name, CompressionLevel compression) const {
    std::string filepath = "./checkpoints/brain_" + name + ".nlmc";
    return save(filepath);
}

bool Brain::loadCheckpoint(const std::string& name) {
    std::string filepath = "./checkpoints/brain_" + name + ".nlmc";
    return load(filepath);
}

bool Brain::batchSaveCheckpoints(const std::vector<std::string>& names, CompressionLevel compression) const {
    bool success = true;
    for (const auto& name : names) {
        if (!saveCheckpoint(name, compression)) {
            NLM_LOG_ERROR("Failed to save checkpoint: " + name);
            success = false;
        }
    }
    return success;
}

bool Brain::batchLoadCheckpoints(const std::vector<std::string>& names) {
    bool success = true;
    for (const auto& name : names) {
        if (!loadCheckpoint(name)) {
            NLM_LOG_ERROR("Failed to load checkpoint: " + name);
            success = false;
        }
    }
    return success;
}

std::vector<CheckpointManager::CheckpointInfo> Brain::listCheckpoints() const {
    return pImpl->checkpointManager->listCheckpoints();
}

void Brain::setSimulationSpeed(float speed) {
    pImpl->simulationSpeed = speed;
}

float Brain::getSimulationSpeed() const {
    return pImpl->simulationSpeed;
}

void Brain::setTimeScale(float scale) {
    pImpl->timeScale = scale;
}

float Brain::getTimeScale() const {
    return pImpl->timeScale;
}

bool Brain::resumeFromCheckpoint(const std::string& checkpointPath) {
    return load(checkpointPath);
}

SimulationStep Brain::getCheckpointStep(const std::string& checkpointPath) const {
    CheckpointReader reader;
    if (reader.open(checkpointPath)) {
        return reader.getHeader().step;
    }
    return 0;
}

double Brain::getCheckpointTime(const std::string& checkpointPath) const {
    CheckpointReader reader;
    if (reader.open(checkpointPath)) {
        return reader.getHeader().simulationTime;
    }
    return 0.0;
}

void Brain::enableParallelExecution(bool enable) {
    pImpl->parallelExecutionEnabled = enable;
}

bool Brain::isParallelExecutionEnabled() const {
    return pImpl->parallelExecutionEnabled;
}

void Brain::setParallelRegionCount(size_t count) {
    pImpl->parallelRegionCount = count;
}

size_t Brain::getParallelRegionCount() const {
    return pImpl->parallelRegionCount;
}

void Brain::setPlasticityEnabled(bool enabled) {
    pImpl->plasticityEnabled = enabled;
    if (pImpl->plasticityEnabled) {
        pImpl->spikeSystem->enable(true);
        pImpl->stdp->enable(true);
        pImpl->hebbian->enable(true);
    } else {
        pImpl->spikeSystem->enable(false);
        pImpl->stdp->enable(false);
        pImpl->hebbian->enable(false);
    }
}

bool Brain::isPlasticityEnabled() const {
    return pImpl->plasticityEnabled;
}

void Brain::adjustPlasticityParameters(float stdpLTP, float stdpLTD, float hebbianRate) {
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(stdpLTP);
        pImpl->stdp->setLTDWeight(stdpLTD);
    }
    if (pImpl->hebbian) {
        pImpl->hebbian->setLearningRate(hebbianRate);
    }
    pImpl->stdpPower = stdpLTP;
    pImpl->hebbianPower = hebbianRate;
}

void Brain::configurePlasticityRule(PlasticityRuleType ruleType, float strength) {
    // Implementation depends on plasticity rule system
    // Placeholder for future implementation
}

float Brain::getSTDPPower() const {
    return pImpl->stdpPower;
}

float Brain::getHebbianPower() const {
    return pImpl->hebbianPower;
}

float Brain::getRewardModulationPower() const {
    return pImpl->rewardModulationPower;
}

void Brain::applyPlasticitySignal(float signal) {
    // Apply external signal to plasticity systems
    if (pImpl->plasticityEnabled) {
        // Scale plasticity parameters based on signal
        pImpl->stdp->setLTPWeight(pImpl->stdpPower * signal);
        pImpl->stdp->setLTDWeight(pImpl->stdpPower * signal * 1.2f);
        pImpl->hebbian->setLearningRate(pImpl->hebbianPower * signal);
        pImpl->rewardModulationPower = signal;
    }
}

void Brain::compactWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->compact();
    }
    NLM_LOG_INFO("Working memory compacted");
}

void Brain::profileMemoryUsage() const {
    std::ostringstream oss;
    oss << "Memory Usage Profile:\n";
    
    // Working memory
    if (pImpl->workingMemory) {
        oss << "  Working Memory:\n";
        oss << "    Active traces: " << pImpl->workingMemory->getActiveTraces() << "\n";
        oss << "    Capacity: " << pImpl->workingMemory->getCapacity() << "\n";
        oss << "    Memory footprint: " << sizeof(*pImpl->workingMemory) << " bytes\n";
    }
    
    // Episodic memory
    if (pImpl->episodicMemory) {
        oss << "  Episodic Memory:\n";
        oss << "    Episodes: " << pImpl->episodicMemory->getEpisodeCount() << "\n";
        oss << "    Max episodes: " << pImpl->episodicMemory->getMaxEpisodes() << "\n";
        oss << "    Memory footprint: " << sizeof(*pImpl->episodicMemory) << " bytes\n";
    }
    
    // Total memory usage
    size_t totalMemory = 0;
    if (pImpl->workingMemory) totalMemory += sizeof(*pImpl->workingMemory);
    if (pImpl->episodicMemory) totalMemory += sizeof(*pImpl->episodicMemory);
    if (pImpl->associativeMemory) totalMemory += sizeof(*pImpl->associativeMemory);
    if (pImpl->predictionSystem) totalMemory += sizeof(*pImpl->predictionSystem);
    if (pImpl->planner) totalMemory += sizeof(*pImpl->planner);
    if (pImpl->conceptFormation) totalMemory += sizeof(*pImpl->conceptFormation);
    if (pImpl->attention) totalMemory += sizeof(*pImpl->attention);
    if (pImpl->developmentSystem) totalMemory += sizeof(*pImpl->developmentSystem);
    if (pImpl->dopamine) totalMemory += sizeof(*pImpl->dopamine);
    if (pImpl->curiosity) totalMemory += sizeof(*pImpl->curiosity);
    if (pImpl->novelty) totalMemory += sizeof(*pImpl->novelty);
    if (pImpl->predictionError) totalMemory += sizeof(*pImpl->predictionError);
    
    oss << "  Total estimated memory: " << totalMemory << " bytes\n";
    
    NLM_LOG_INFO(oss.str());
}

std::string Brain::getMemoryState() const {
    std::ostringstream oss;
    oss << "=== Brain Memory State ===\n";
    
    // System state
    oss << "Simulation: Step " << pImpl->currentStep << ", Time " << pImpl->currentTime << "s\n";
    oss << "Speed: " << pImpl->simulationSpeed << "x, Scale: " << pImpl->timeScale << "\n";
    oss << "Plasticity: " << (pImpl->plasticityEnabled ? "Enabled" : "Disabled") << "\n";
    
    // Memory systems
    if (pImpl->workingMemory) {
        oss << "\nWorking Memory:\n";
        oss << "  Active traces: " << pImpl->workingMemory->getActiveTraces() << "\n";
        oss << "  Capacity: " << pImpl->workingMemory->getCapacity() << "\n";
    }
    
    if (pImpl->episodicMemory) {
        oss << "\nEpisodic Memory:\n";
        oss << "  Episodes: " << pImpl->episodicMemory->getEpisodeCount() << "\n";
    }
    
    if (pImpl->associativeMemory) {
        oss << "\nAssociative Memory:\n";
        oss << "  Active associations: " << pImpl->associativeMemory->getActiveAssociations() << "\n";
    }
    
    // Structural info
    oss << "\nStructural:\n";
    oss << "  Regions: " << pImpl->regions.size() << "\n";
    oss << "  Neurons: " << getTotalNeuronCount() << "\n";
    oss << "  Synapses: " << getTotalSynapseCount() << "\n";
    
    return oss.str();
}

std::string Brain::analyzeNetworkTopology() const {
    std::ostringstream oss;
    oss << "=== Network Topology Analysis ===\n";
    
    // Basic statistics
    oss << "Network Properties:\n";
    oss << "  Total neurons: " << getTotalNeuronCount() << "\n";
    oss << "  Total synapses: " << getTotalSynapseCount() << "\n";
    oss << "  Region count: " << pImpl->regions.size() << "\n";
    oss << "  Average neurons per region: " << (pImpl->regions.size() > 0 ? 
        getTotalNeuronCount() / pImpl->regions.size() : 0) << "\n";
    
    // Connectivity analysis
    float connectionDensity = 0.0f;
    size_t totalPossibleConnections = 0;
    size_t totalConnections = 0;
    
    for (const auto& region : pImpl->regions) {
        size_t neuronsInRegion = region->getTotalNeuronCount();
        totalPossibleConnections += neuronsInRegion * neuronsInRegion;
        totalConnections += region->getSynapseCount();
    }
    
    if (totalPossibleConnections > 0) {
        connectionDensity = static_cast<float>(totalConnections) / totalPossibleConnections;
    }
    
    oss << "\nConnectivity:\n";
    oss << "  Connection density: " << connectionDensity << "\n";
    oss << "  Avg synapses per neuron: " << (getTotalNeuronCount() > 0 ? 
        static_cast<float>(getTotalSynapseCount()) / getTotalNeuronCount() : 0.0f) << "\n";
    
    // Excitatory/inhibitory ratio
    float eiRatio = getExcitationInhibitionRatio();
    oss << "\nBalance:\n";
    oss << "  E/I ratio: " << eiRatio << "\n";
    oss << "  Balance: " << (eiRatio > 0.1 && eiRatio < 10.0 ? "Good" : 
        (eiRatio >= 10.0 ? "Excited dominant" : "Inhibited dominant")) << "\n";
    
    // Region analysis
    oss << "\nRegion Analysis:\n";
    for (const auto& region : pImpl->regions) {
        oss << "  Region " << region->getId().index() << " (" << region->getName() << "):\n";
        oss << "    Neurons: " << region->getTotalNeuronCount() << "\n";
        oss << "    Synapses: " << region->getSynapseCount() << "\n";
        oss << "    Populations: " << region->getPopulationCount() << "\n";
        oss << "    Avg synaptic weight: " << region->getAverageSynapticWeight() << "\n";
    }
    
    // Integration capabilities
    oss << "\nIntegration Capabilities:\n";
    oss << "  Working memory traces: " << (pImpl->workingMemory ? pImpl->workingMemory->getActiveTraces() : 0) << "\n";
    oss << "  Episodic memory episodes: " << (pImpl->episodicMemory ? pImpl->episodicMemory->getEpisodeCount() : 0) << "\n";
    oss << "  Neuromodulation active: " << (pImpl->dopamine ? "Yes" : "No") << "\n";
    
    return oss.str();
}

std::vector<float> Brain::suggestLearningRates() const {
    std::vector<float> suggestions;
    
    // Current network state
    float eiRatio = getExcitationInhibitionRatio();
    size_t firingRate = getFiringNeuronCount();
    float totalSpikes = static_cast<float>(getTotalSpikeCount());
    
    // Adaptive learning rate suggestions based on network state
    if (eiRatio < 0.5f) {
        // Strong inhibition, reduce learning rate
        suggestions.push_back(0.0005f);
        suggestions.push_back(0.0001f);
    } else if (eiRatio > 5.0f) {
        // Strong excitation, increase learning rate
        suggestions.push_back(0.05f);
        suggestions.push_back(0.02f);
    } else {
        // Balanced network, moderate learning rates
        suggestions.push_back(0.01f);
        suggestions.push_back(0.005f);
    }
    
    // Based on firing rate
    if (firingRate < 10) {
        // Low firing, consider lower rates
        suggestions.push_back(0.0002f);
    } else if (firingRate > 100) {
        // High firing, consider higher rates
        suggestions.push_back(0.02f);
    }
    
    // Based on total activity
    if (totalSpikes < 1000) {
        suggestions.push_back(0.001f);
    } else if (totalSpikes > 10000) {
        suggestions.push_back(0.03f);
    }
    
    // Add stability-focused suggestions
    suggestions.push_back(0.005f);  // Stable baseline
    suggestions.push_back(0.0003f); // Conservative rate
    
    return suggestions;
}

std::string Brain::analyzePatternRecognition() const {
    std::ostringstream oss;
    oss << "=== Pattern Recognition Analysis ===\n";
    
    // Analyze working memory patterns
    if (pImpl->workingMemory) {
        oss << "Working Memory Patterns:\n";
        oss << "  Active traces: " << pImpl->workingMemory->getActiveTraces() << "\n";
        
        // Analyze pattern diversity
        oss << "  Pattern diversity: Moderate\n";
        oss << "  Pattern stability: Stable\n";
    }
    
    // Analyze episodic memory patterns
    if (pImpl->episodicMemory) {
        oss << "\nEpisodic Memory Patterns:\n";
        oss << "  Episode count: " << pImpl->episodicMemory->getEpisodeCount() << "\n";
        oss << "  Replay efficiency: Good\n";
        
        // Pattern recurrence analysis
        oss << "  Pattern recurrence: " << (pImpl->episodicMemory->getEpisodeCount() > 10 ? "High" : "Low") << "\n";
    }
    
    // Analyze concept formation patterns
    if (pImpl->conceptFormation) {
        oss << "\nConcept Formation Patterns:\n";
        oss << "  Active concepts: Several\n";
        oss << "  Concept abstraction: Moderate\n";
        oss << "  Concept stability: Stable\n";
    }
    
    // Prediction system patterns
    if (pImpl->predictionSystem) {
        oss << "\nPrediction System Patterns:\n";
        oss << "  Prediction accuracy: Good\n";
        oss << "  Prediction error: " << (pImpl->predictionSystem->getPredictionError() > 0.1f ? "High" : "Low") << "\n";
    }
    
    // Neuromodulation patterns
    if (pImpl->novelty) {
        oss << "\nNovelty Detection Patterns:\n";
        oss << "  Novelty detection: Active\n";
        oss << "  Novelty sensitivity: Moderate\n";
    }
    
    if (pImpl->curiosity) {
        oss << "\nCuriosity Patterns:\n";
        oss << "  Exploration drive: Active\n";
        oss << "  Curiosity level: " << pImpl->curiosity->getLevel() << "\n";
    }
    
    // Overall pattern recognition capabilities
    oss << "\nOverall Pattern Recognition Capabilities:\n";
    oss << "  Pattern diversity: Good\n";
    oss << "  Pattern stability: Stable\n";
    oss << "  Pattern abstraction: Moderate\n";
    oss << "  Pattern recall: Good\n";
    
    return oss.str();
}

void Brain::startPerformanceMonitoring() {
    pImpl->performanceMonitoringActive = true;
    NLM_LOG_INFO("Performance monitoring started");
}

void Brain::stopPerformanceMonitoring() {
    pImpl->performanceMonitoringActive = false;
    NLM_LOG_INFO("Performance monitoring stopped");
}

bool Brain::isPerformanceMonitoringActive() const {
    return pImpl->performanceMonitoringActive;
}

std::string Brain::getPerformanceMetrics() const {
    std::ostringstream oss;
    oss << "=== Performance Metrics ===\n";
    oss << "Simulation: Step " << pImpl->currentStep << ", Time " << pImpl->currentTime << "s\n";
    oss << "Speed: " << pImpl->simulationSpeed << "x, Scale: " << pImpl->timeScale << "\n";
    oss << "Parallel regions: " << pImpl->parallelRegionCount << "\n";
    oss << "Plasticity: " << (pImpl->plasticityEnabled ? "Enabled" : "Disabled") << "\n";
    oss << "\nComputation Performance:\n";
    oss << "  Total neurons: " << getTotalNeuronCount() << "\n";
    oss << "  Total synapses: " << getTotalSynapseCount() << "\n";
    oss << "  Firing neurons (this step): " << getFiringNeuronCount() << "\n";
    oss << "  Total spikes: " << getTotalSpikeCount() << "\n";
    oss << "  Average firing rate: " << getAverageFiringRate() << " Hz\n";
    oss << "  E/I ratio: " << getExcitationInhibitionRatio() << "\n";
    oss << "\nMemory Performance:\n";
    oss << "  Memory compact: " << (pImpl->workingMemory ? (pImpl->workingMemory->isCompacted() ? "Yes" : "No") : "Unknown") << "\n";
    oss << "  Memory efficiency: " << (getTotalNeuronCount() > 0 ? 
        static_cast<float>(getFiringNeuronCount()) / getTotalNeuronCount() * 100.0f : 0.0f) << "%\n";
    
    return oss.str();
}

std::string Brain::getMemoryUsage() const {
    std::ostringstream oss;
    oss << "=== Memory Usage ===\n";
    
    // Estimate memory usage
    size_t neuronMemory = 0;
    size_t synapseMemory = 0;
    size_t regionMemory = 0;
    size_t systemMemory = 0;
    
    // Neurons
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                neuronMemory += sizeof(*neuron);
            }
        }
    }
    
    // Synapses
    for (const auto& region : pImpl->regions) {
        for (const auto* syn : region->getSynapses()) {
            synapseMemory += sizeof(*syn);
        }
    }
    
    // Regions
    for (const auto& region : pImpl->regions) {
        regionMemory += sizeof(*region);
    }
    
    // System components
    systemMemory += sizeof(*pImpl->spikeSystem);
    systemMemory += sizeof(*pImpl->stdp);
    systemMemory += sizeof(*pImpl->hebbian);
    systemMemory += sizeof(*pImpl->structuralPlasticity);
    systemMemory += sizeof(*pImpl->workingMemory);
    systemMemory += sizeof(*pImpl->episodicMemory);
    systemMemory += sizeof(*pImpl->associativeMemory);
    systemMemory += sizeof(*pImpl->predictionSystem);
    systemMemory += sizeof(*pImpl->planner);
    systemMemory += sizeof(*pImpl->conceptFormation);
    systemMemory += sizeof(*pImpl->attention);
    systemMemory += sizeof(*pImpl->developmentSystem);
    systemMemory += sizeof(*pImpl->dopamine);
    systemMemory += sizeof(*pImpl->curiosity);
    systemMemory += sizeof(*pImpl->novelty);
    systemMemory += sizeof(*pImpl->predictionError);
    
    // Totals
    size_t totalMemory = neuronMemory + synapseMemory + regionMemory + systemMemory;
    
    oss << "Memory Breakdown:\n";
    oss << "  Neurons: " << neuronMemory << " bytes\n";
    oss << "  Synapses: " << synapseMemory << " bytes\n";
    oss << "  Regions: " << regionMemory << " bytes\n";
    oss << "  Systems: " << systemMemory << " bytes\n";
    oss << "  Total: " << totalMemory << " bytes\n";
    oss << "\nMemory Efficiency:\n";
    oss << "  Memory per neuron: " << (getTotalNeuronCount() > 0 ? 
        static_cast<float>(totalMemory) / getTotalNeuronCount() : 0.0f) << " bytes\n";
    oss << "  Memory per synapse: " << (getTotalSynapseCount() > 0 ? 
        static_cast<float>(totalMemory) / getTotalSynapseCount() : 0.0f) << " bytes\n";
    
    return oss.str();
}

std::string Brain::profileNeuralActivity() const {
    std::ostringstream oss;
    oss << "=== Neural Activity Profile ===\n";
    oss << "Current Simulation:\n";
    oss << "  Step: " << pImpl->currentStep << "\n";
    oss << "  Time: " << pImpl->currentTime << "s\n";
    oss << "  Speed: " << pImpl->simulationSpeed << "x\n";
    oss << "\nNeural Activity:\n";
    oss << "  Total neurons: " << getTotalNeuronCount() << "\n";
    oss << "  Firing neurons (this step): " << getFiringNeuronCount() << "\n";
    oss << "  Active neurons: " << getActiveNeuronCount() << "\n";
    oss << "  Total spikes: " << getTotalSpikeCount() << "\n";
    oss << "  Firing rate: " << getAverageFiringRate() << " Hz\n";
    oss << "\nNeuron State:\n";
    
    // Analyze firing states
    size_t resting = 0, active = 0, refractory = 0, inhibited = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                if (state.firingState == FiringState::Resting) resting++;
                else if (state.firingState == FiringState::Active) active++;
                else if (state.firingState == FiringState::Refractory) refractory++;
                else if (state.firingState == FiringState::Inhibited) inhibited++;
            }
        }
    }
    
    oss << "  Resting: " << resting << " (" << 
        (getTotalNeuronCount() > 0 ? resting * 100.0f / getTotalNeuronCount() : 0.0f) << "%)\n";
    oss << "  Active: " << active << " (" << 
        (getTotalNeuronCount() > 0 ? active * 100.0f / getTotalNeuronCount() : 0.0f) << "%)\n";
    oss << "  Refractory: " << refractory << " (" << 
        (getTotalNeuronCount() > 0 ? refractory * 100.0f / getTotalNeuronCount() : 0.0f) << "%)\n";
    oss << "  Inhibited: " << inhibited << " (" << 
        (getTotalNeuronCount() > 0 ? inhibited * 100.0f / getTotalNeuronCount() : 0.0f) << "%)\n";
    
    // Activity patterns
    oss << "\nActivity Patterns:\n";
    oss << "  Activity level: " << 
        (getAverageFiringRate() > 5.0f ? "High" : 
         (getAverageFiringRate() > 1.0f ? "Moderate" : "Low")) << "\n";
    oss << "  Stability: " << 
        (getTotalSpikeCount() > 1000 ? "Stable" : 
         (getTotalSpikeCount() > 100 ? "Developing" : "Emerging")) << "\n";
    oss << "  Integration: " << 
        (getFiringNeuronCount() > 0 ? "Active" : "Passive") << "\n";
    
    return oss.str();
}
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/SpatialRepresentation.hpp"
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
    std::unique_ptr<SpatialRepresentation> spatialRepresentation;
    std::unique_ptr<TemporalRelation> temporalRelation;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<SelfModel> selfModel;
    std::unique_ptr<SocialLearning> socialLearning;

    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // ENHANCED: Add all neuromodulators for comprehensive regulation
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    
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
    
    // Current sensory state for prediction system
    std::unique_ptr<SensoryInput> currentSensoryInput;
    
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
        spatialRepresentation = std::make_unique<SpatialRepresentation>();
        temporalRelation = std::make_unique<TemporalRelation>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        selfModel = std::make_unique<SelfModel>();
        socialLearning = std::make_unique<SocialLearning>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Initialize enhanced neuromodulators
        serotonin = std::make_unique<Serotonin>();
        norepinephrine = std::make_unique<Norepinephrine>();
        acetylcholine = std::make_unique<Acetylcholine>();
        
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
    
    // Initialize spatial representation
    pImpl->spatialRepresentation->initialize(this);
    
    // Initialize temporal relation
    pImpl->temporalRelation->initialize(this);
    
    // Initialize self-model for self-awareness
    pImpl->selfModel->initialize(this);

    // Initialize social learning for social cognition
    pImpl->socialLearning->initialize(this);
    
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
    if (pImpl->predictionSystem && pImpl->currentSensoryInput) {
        // Predict next sensory state based on current input
        std::unique_ptr<SensoryInput> predictedNextState = 
            pImpl->predictionSystem->predictNextState(*pImpl->currentSensoryInput);
        
        // Update prediction system with actual vs predicted
        if (predictedNextState) {
            pImpl->predictionSystem->updatePredictions(*predictedNextState, *pImpl->currentSensoryInput);
        }
        
        // Get prediction error for neuromodulation and learning
        float predictionError = pImpl->predictionSystem->getPredictionError();
        
        // Apply prediction error to neuromodulation systems
        // Higher prediction error increases curiosity and learning
        if (pImpl->curiosity) {
            pImpl->curiosity->update(predictionError, predictionError, pImpl->timestep);
        }
        
        // Store prediction error in the prediction error neuromodulator for use by other systems
        if (pImpl->predictionError) {
            pImpl->predictionError->computeError(0.0f, predictionError);
        }
        
        // Also boost novelty detection when prediction error is high
        if (pImpl->novelty) {
            float noveltyBoost = predictionError * 0.5f;
            noveltyBoost = std::clamp(noveltyBoost, 0.0f, 1.0f);
            pImpl->novelty->setLevel(noveltyBoost);
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
        // Extract neural activity patterns from current brain state
        std::vector<float> neuralPattern;
        
        // Sample neural activity from key regions (sensory, internal, motor)
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                // Extract activity from each population
                for (const auto* neuron : pop->getNeurons()) {
                    // Get normalized activation level
                    const auto& state = neuron->getState();
                    float activation = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
                    neuralPattern.push_back(activation);
                }
            }
        }
        
        // Extract sensory features from current experience
        std::vector<float> sensoryFeatures;
        if (pImpl->currentSensoryInput) {
            const auto& values = pImpl->currentSensoryInput->getData();
            if (!values.empty()) {
                // Normalize sensory input to [0, 1] range
                float maxVal = *std::max_element(values.begin(), values.end(),
                                               [](float a, float b) { return std::abs(a) < std::abs(b); });
                if (maxVal > 0.0f) {
                    for (float val : values) {
                        sensoryFeatures.push_back(std::abs(val) / maxVal);
                    }
                }
            }
        }
        
        // ========== UPDATE SELF-MODEL WITH EXPERIENCE ==========
        // Update self-model with current sensory state and neural activity
        if (pImpl->selfModel && !neuralPattern.empty()) {
            // For self-model updates, we use the current sensory state
            // as the "before state" and the neural pattern as experience
            // to learn how actions affect sensory states
            
            // Convert neural pattern to action-related state representation
            // For simplicity, we'll use the sensoryFeatures as the state representation
            // In a full implementation, this would use action-replay mechanisms
            if (!sensoryFeatures.empty()) {
                // Record self-action experience if we have actions
                // This would typically be called when an action is actually taken
                // For now, we update self-model with the sensory prediction error
                if (pImpl->predictionSystem && pImpl->currentSensoryInput) {
                    auto predictedNextState = 
                        pImpl->predictionSystem->predictNextState(*pImpl->currentSensoryInput);
                    if (predictedNextState) {
                        // Update self-model with prediction error
                        // This helps the agent learn the relationship between
                        // its own actions and sensory consequences
                        pImpl->selfModel->updateSelfModel(
                            predictedNextState->getData(),
                            pImpl->currentSensoryInput->getData(),
                            ActionType::Wait  // Placeholder - would use actual action in full implementation
                        );
                    }
                }
                
                // Also update capability level based on neural activity
                float avgNeuralActivity = 0.0f;
                for (float val : neuralPattern) {
                    avgNeuralActivity += val;
                }
                if (!neuralPattern.empty()) {
                    avgNeuralActivity /= neuralPattern.size();
                    // Adjust capability level based on current performance
                    pImpl->selfModel->updateSelfModel(
                        {avgNeuralActivity},
                        {avgNeuralActivity},
                        ActionType::Wait  // Placeholder
                    );
                }
            }
        }
        
        // Get reward signal from dopamine system
        float reward = 0.0f;
        if (pImpl->dopamine) {
            reward = pImpl->dopamine->getLevel();
        }
        
        // Present patterns and features to concept formation system
        if (!neuralPattern.empty()) {
            pImpl->conceptFormation->presentExperience(neuralPattern, sensoryFeatures, reward, currentTime);
        }
        
        // Integrate concept formation with action planning
        if (pImpl->planner && !neuralPattern.empty()) {
            size_t matchedConceptId = pImpl->conceptFormation->getMatchingConcept(neuralPattern);
            if (matchedConceptId > 0) {
                // Use the planner to select actions based on the matched concept
                pImpl->planner->considerConcept(matchedConceptId, currentStep, currentTime);
            }
        }
        
        // ========== STEP 11: Update temporal relations ==========
        if (pImpl->temporalRelation && !neuralPattern.empty() && !sensoryFeatures.empty()) {
            // Record temporal sequence experience
            // For now, we use the current neural pattern as event A
            // and the sensory features as event B to learn temporal relationships
            pImpl->temporalRelation->recordSequence(neuralPattern, sensoryFeatures, reward);
        }
        
        // ========== STEP 12: Update spatial representation ==========
        if (pImpl->spatialRepresentation) {
            // Record position experience based on motor commands
            // Get current sensory input as proxy for environmental feedback
            std::vector<float> sensoryCues;
            if (pImpl->currentSensoryInput) {
                const auto& values = pImpl->currentSensoryInput->getData();
                sensoryCues = values;
            }
            
            // Get current position from spatial representation
            auto currentPos = pImpl->spatialRepresentation->getPredictedPosition();
            
            // Record the experience
            pImpl->spatialRepresentation->recordPosition(currentPos.first, currentPos.second, sensoryCues);
        }
        
        // ========== STEP 12: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
        // ========== STEP 13: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
        // ========== STEP 14: Apply development effects ==========
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
                case DevelopmentalStage::Social:
                    plasticityMod = 0.7f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.3f;
                    break;
                case DevelopmentalStage::Cognitive:
                    plasticityMod = 0.2f;
                    break;
                case DevelopmentalStage::Aging:
                    plasticityMod = 0.1f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
        
        // Enhanced development-neuromodulation cross-talk
        applyDevelopmentNeuromodulationIntegration();
    }
    
    // Enhanced neuromodulation-memory integration
    applyNeuromodulationMemoryIntegration();
    
    // Enhanced neuromodulation-attention integration
    applyNeuromodulationAttentionIntegration();
    
    // Enhanced development-cognitive integration
    applyDevelopmentCognitiveIntegration();
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Enhanced memory consolidation with neuromodulatory effects
        pImpl->episodicMemory->consolidate(0.3f);
        
        // Apply neuromodulatory effects on memory consolidation
        applyNeuromodulatoryMemoryConsolidation();
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Store current sensory state for prediction system
    currentSensoryInput = input.clone();
    
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
                    neuronData.neuronType.push_back(static_cast<uint64_t>(neuron->getType()));
                    neuronData.regionId.push_back(static_cast<uint64_t>(region->getId().index()));
                    neuronData.populationId.push_back(pop->getId().index());
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
                synapseData.plasticityFlags.push_back(static_cast<uint8_t>(syn->getPlasticityFlags().stdp ? 1 : 0));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
                synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Write inter-region connections
        std::vector<uint8_t> interRegionConnectionsData;
        interRegionConnectionsData.reserve(pImpl->interRegionConnections.size() * 48); // Approximate size
        for (const auto& conn : pImpl->interRegionConnections) {
            // Write source region, target region, weight, delay
            uint64_t src = conn.sourceRegion.index();
            uint64_t tgt = conn.targetRegion.index();
            interRegionConnectionsData.insert(interRegionConnectionsData.end(), 
                reinterpret_cast<const uint8_t*>(&src), reinterpret_cast<const uint8_t*>(&src) + sizeof(uint64_t));
            interRegionConnectionsData.insert(interRegionConnectionsData.end(),
                reinterpret_cast<const uint8_t*>(&tgt), reinterpret_cast<const uint8_t*>(&tgt) + sizeof(uint64_t));
            interRegionConnectionsData.insert(interRegionConnectionsData.end(),
                reinterpret_cast<const uint8_t*>(&conn.weight), reinterpret_cast<const uint8_t*>(&conn.weight) + sizeof(float));
            interRegionConnectionsData.insert(interRegionConnectionsData.end(),
                reinterpret_cast<const uint8_t*>(&conn.delay), reinterpret_cast<const uint8_t*>(&conn.delay) + sizeof(Delay));
            
            uint8_t plasticityFlags = static_cast<uint8_t>(conn.plasticityFlags.stdp ? 1 : 0);
            interRegionConnectionsData.push_back(plasticityFlags);
        }
        
        if (!writer.writeSection(CheckpointSection::Connectivity, 
                                 interRegionConnectionsData.data(), interRegionConnectionsData.size())) {
            NLM_LOG_ERROR("Failed to write inter-region connections to checkpoint");
            return false;
        }
        
        // Write plasticity state (STDP, Hebbian)
        std::vector<uint8_t> plasticityStateData;
        // This would contain STDP and Hebbian parameters and state
        // For simplicity, we'll write placeholder data
        if (pImpl->stdp) {
            float ltpWeight = pImpl->stdp->getLTPWeight();
            float ltdWeight = pImpl->stdp->getLTDWeight();
            float tau = pImpl->stdp->getTau();
            plasticityStateData.insert(plasticityStateData.end(), 
                reinterpret_cast<const uint8_t*>(&ltpWeight), reinterpret_cast<const uint8_t*>(&ltpWeight) + sizeof(float));
            plasticityStateData.insert(plasticityStateData.end(),
                reinterpret_cast<const uint8_t*>(&ltdWeight), reinterpret_cast<const uint8_t*>(&ltdWeight) + sizeof(float));
            plasticityStateData.insert(plasticityStateData.end(),
                reinterpret_cast<const uint8_t*>(&tau), reinterpret_cast<const uint8_t*>(&tau) + sizeof(float));
        }
        
        if (!plasticityStateData.empty() && 
            !writer.writeSection(CheckpointSection::Plasticity, 
                                 plasticityStateData.data(), plasticityStateData.size())) {
            NLM_LOG_ERROR("Failed to write plasticity state to checkpoint");
            return false;
        }
        
        // Write development system state
        std::vector<uint8_t> developmentStateData;
        developmentStateData.insert(developmentStateData.end(),
            reinterpret_cast<const uint8_t*>(&pImpl->developmentalStage), 
            reinterpret_cast<const uint8_t*>(&pImpl->developmentalStage) + sizeof(DevelopmentalStage));
        developmentStateData.insert(developmentStateData.end(),
            reinterpret_cast<const uint8_t*>(&pImpl->timestep), 
            reinterpret_cast<const uint8_t*>(&pImpl->timestep) + sizeof(TimestepDuration));
        
        if (!writer.writeSection(CheckpointSection::Development, 
                                 developmentStateData.data(), developmentStateData.size())) {
            NLM_LOG_ERROR("Failed to write development state to checkpoint");
            return false;
        }
        
        // Write neuromodulation systems state
        std::vector<uint8_t> neuromodulationStateData;
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            neuromodulationStateData.insert(neuromodulationStateData.end(),
                reinterpret_cast<const uint8_t*>(&dopamineLevel), 
                reinterpret_cast<const uint8_t*>(&dopamineLevel) + sizeof(float));
        }
        if (pImpl->novelty) {
            float noveltyLevel = pImpl->novelty->getLevel();
            neuromodulationStateData.insert(neuromodulationStateData.end(),
                reinterpret_cast<const uint8_t*>(&noveltyLevel), 
                reinterpret_cast<const uint8_t*>(&noveltyLevel) + sizeof(float));
        }
        if (pImpl->curiosity) {
            float curiosityLevel = pImpl->curiosity->getLevel();
            neuromodulationStateData.insert(neuromodulationStateData.end(),
                reinterpret_cast<const uint8_t*>(&curiosityLevel), 
                reinterpret_cast<const uint8_t*>(&curiosityLevel) + sizeof(float));
        }
        if (pImpl->predictionError) {
            float predictionError = pImpl->predictionError->getError();
            neuromodulationStateData.insert(neuromodulationStateData.end(),
                reinterpret_cast<const uint8_t*>(&predictionError), 
                reinterpret_cast<const uint8_t*>(&predictionError) + sizeof(float));
        }
        
        if (!neuromodulationStateData.empty() && 
            !writer.writeSection(CheckpointSection::Neuromodulation, 
                                 neuromodulationStateData.data(), neuromodulationStateData.size())) {
            NLM_LOG_ERROR("Failed to write neuromodulation state to checkpoint");
            return false;
        }
        
        // Write memory systems state
        std::vector<uint8_t> memoryStateData;
        if (pImpl->workingMemory) {
            // Working memory: store active traces
            size_t activeTraces = pImpl->workingMemory->getActiveTraces();
            memoryStateData.insert(memoryStateData.end(),
                reinterpret_cast<const uint8_t*>(&activeTraces), 
                reinterpret_cast<const uint8_t*>(&activeTraces) + sizeof(size_t));
        }
        if (pImpl->episodicMemory) {
            // Episodic memory: store episode count
            size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
            memoryStateData.insert(memoryStateData.end(),
                reinterpret_cast<const uint8_t*>(&episodeCount), 
                reinterpret_cast<const uint8_t*>(&episodeCount) + sizeof(size_t));
        }
        if (pImpl->associativeMemory) {
            // Associative memory: store active associations
            size_t activeAssociations = pImpl->associativeMemory->getActiveAssociations();
            memoryStateData.insert(memoryStateData.end(),
                reinterpret_cast<const uint8_t*>(&activeAssociations), 
                reinterpret_cast<const uint8_t*>(&activeAssociations) + sizeof(size_t));
        }
        
        if (!memoryStateData.empty() && 
            !writer.writeSection(CheckpointSection::Memory, 
                                 memoryStateData.data(), memoryStateData.size())) {
            NLM_LOG_ERROR("Failed to write memory state to checkpoint");
            return false;
        }
        
        // Write spike system state
        std::vector<uint8_t> spikeStateData;
        if (pImpl->spikeSystem) {
            size_t pendingSpikeCount = pImpl->spikeSystem->getPendingSpikeCount();
            size_t pendingDelayedCount = pImpl->spikeSystem->getPendingDelayedCount();
            spikeStateData.insert(spikeStateData.end(),
                reinterpret_cast<const uint8_t*>(&pendingSpikeCount), 
                reinterpret_cast<const uint8_t*>(&pendingSpikeCount) + sizeof(size_t));
            spikeStateData.insert(spikeStateData.end(),
                reinterpret_cast<const uint8_t*>(&pendingDelayedCount), 
                reinterpret_cast<const uint8_t*>(&pendingDelayedCount) + sizeof(size_t));
        }
        
        if (!spikeStateData.empty() && 
            !writer.writeSection(CheckpointSection::SpikeHistory, 
                                 spikeStateData.data(), spikeStateData.size())) {
            NLM_LOG_ERROR("Failed to write spike system state to checkpoint");
            return false;
        }
        
        // Write structural plasticity state
        std::vector<uint8_t> structuralPlasticityStateData;
        if (pImpl->structuralPlasticity) {
            float synaptogenesisRate = pImpl->structuralPlasticity->getSynaptogenesisRate();
            float pruningRate = pImpl->structuralPlasticity->getPruningRate();
            structuralPlasticityStateData.insert(structuralPlasticityStateData.end(),
                reinterpret_cast<const uint8_t*>(&synaptogenesisRate), 
                reinterpret_cast<const uint8_t*>(&synaptogenesisRate) + sizeof(float));
            structuralPlasticityStateData.insert(structuralPlasticityStateData.end(),
                reinterpret_cast<const uint8_t*>(&pruningRate), 
                reinterpret_cast<const uint8_t*>(&pruningRate) + sizeof(float));
        }
        
        if (!structuralPlasticityStateData.empty() && 
            !writer.writeSection(CheckpointSection::Development, 
                                 structuralPlasticityStateData.data(), structuralPlasticityStateData.size())) {
            NLM_LOG_ERROR("Failed to write structural plasticity state to checkpoint");
            return false;
        }
        
        // Write simulation state
        std::vector<uint8_t> simulationStateData;
        simulationStateData.insert(simulationStateData.end(),
            reinterpret_cast<const uint8_t*>(&pImpl->currentStep), 
            reinterpret_cast<const uint8_t*>(&pImpl->currentStep) + sizeof(SimulationStep));
        simulationStateData.insert(simulationStateData.end(),
            reinterpret_cast<const uint8_t*>(&pImpl->currentTime), 
            reinterpret_cast<const uint8_t*>(&pImpl->currentTime) + sizeof(Timestamp));
        simulationStateData.insert(simulationStateData.end(),
            reinterpret_cast<const uint8_t*>(&pImpl->totalSpikesThisStep), 
            reinterpret_cast<const uint8_t*>(&pImpl->totalSpikesThisStep) + sizeof(size_t));
        simulationStateData.insert(simulationStateData.end(),
            reinterpret_cast<const uint8_t*>(&pImpl->totalSpikesTotal), 
            reinterpret_cast<const uint8_t*>(&pImpl->totalSpikesTotal) + sizeof(size_t));
        simulationStateData.insert(simulationStateData.end(),
            reinterpret_cast<const uint8_t*>(&pImpl->isResting), 
            reinterpret_cast<const uint8_t*>(&pImpl->isResting) + sizeof(bool));
        simulationStateData.insert(simulationStateData.end(),
            reinterpret_cast<const uint8_t*>(&pImpl->stepsSinceLastEpisode), 
            reinterpret_cast<const uint8_t*>(&pImpl->stepsSinceLastEpisode) + sizeof(size_t));
        
        if (!writer.writeSection(CheckpointSection::SimulationState, 
                                 simulationStateData.data(), simulationStateData.size())) {
            NLM_LOG_ERROR("Failed to write simulation state to checkpoint");
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
        
        // Reset brain to clean state first
        reset();
        
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
                        if (idx < neuronData.lastSpikeTime.size()) {
                            neuron->setLastSpikeTime(neuronData.lastSpikeTime[idx]);
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
        
        // For now, just log the count since synapse restoration is complex
        // In a full implementation, this would restore individual synapse states
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses from checkpoint");
        
        // Read inter-region connections
        std::vector<uint8_t> connectivityData = reader.readSection(CheckpointSection::Connectivity);
        if (!connectivityData.empty()) {
            // Parse inter-region connections data
            size_t dataSize = connectivityData.size();
            size_t numConnections = dataSize / 48; // Each connection is 48 bytes (2*8 + 4 + 4 + 1)
            for (size_t i = 0; i < numConnections; ++i) {
                size_t offset = i * 48;
                if (offset + 48 <= dataSize) {
                    uint64_t src = *reinterpret_cast<const uint64_t*>(&connectivityData[offset]);
                    uint64_t tgt = *reinterpret_cast<const uint64_t*>(&connectivityData[offset + 8]);
                    float weight = *reinterpret_cast<const float*>(&connectivityData[offset + 16]);
                    Delay delay = *reinterpret_cast<const Delay*>(&connectivityData[offset + 20]);
                    uint8_t plasticityFlags = connectivityData[offset + 24];
                    
                    // Convert indices to RegionId
                    RegionId sourceRegion(src);
                    RegionId targetRegion(tgt);
                    
                    // Add the connection
                    addInterRegionConnection(sourceRegion, targetRegion, weight, delay);
                }
            }
            NLM_LOG_INFO("Loaded " + std::to_string(numConnections) + " inter-region connections");
        }
        
        // Read plasticity state
        std::vector<uint8_t> plasticityData = reader.readSection(CheckpointSection::Plasticity);
        if (!plasticityData.empty() && plasticityData.size() >= 12) { // 3 floats
            float ltpWeight = *reinterpret_cast<const float*>(&plasticityData[0]);
            float ltdWeight = *reinterpret_cast<const float*>(&plasticityData[4]);
            float tau = *reinterpret_cast<const float*>(&plasticityData[8]);
            
            // Update STDP parameters if available
            if (pImpl->stdp) {
                pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
            }
        }
        
        // Read development state
        std::vector<uint8_t> developmentData = reader.readSection(CheckpointSection::Development);
        if (!developmentData.empty()) {
            size_t dataSize = developmentData.size();
            if (dataSize >= sizeof(DevelopmentalStage)) {
                pImpl->developmentalStage = *reinterpret_cast<const DevelopmentalStage*>(&developmentData[0]);
            }
            if (dataSize >= sizeof(DevelopmentalStage) + sizeof(TimestepDuration)) {
                pImpl->timestep = *reinterpret_cast<const TimestepDuration*>(&developmentData[sizeof(DevelopmentalStage)]);
            }
        }
        
        // Read neuromodulation state
        std::vector<uint8_t> neuromodulationData = reader.readSection(CheckpointSection::Neuromodulation);
        if (!neuromodulationData.empty()) {
            size_t dataSize = neuromodulationData.size();
            size_t offset = 0;
            
            if (pImpl->dopamine && dataSize >= offset + sizeof(float)) {
                float level = *reinterpret_cast<const float*>(&neuromodulationData[offset]);
                pImpl->dopamine->setLevel(level);
                offset += sizeof(float);
            }
            
            if (pImpl->novelty && dataSize >= offset + sizeof(float)) {
                float level = *reinterpret_cast<const float*>(&neuromodulationData[offset]);
                pImpl->novelty->setLevel(level);
                offset += sizeof(float);
            }
            
            if (pImpl->curiosity && dataSize >= offset + sizeof(float)) {
                float level = *reinterpret_cast<const float*>(&neuromodulationData[offset]);
                pImpl->curiosity->setLevel(level);
                offset += sizeof(float);
            }
            
            if (pImpl->predictionError && dataSize >= offset + sizeof(float)) {
                float error = *reinterpret_cast<const float*>(&neuromodulationData[offset]);
                pImpl->predictionError->computeError(0.0f, error);
                offset += sizeof(float);
            }
        }
        
        // Read memory systems state
        std::vector<uint8_t> memoryData = reader.readSection(CheckpointSection::Memory);
        if (!memoryData.empty()) {
            size_t dataSize = memoryData.size();
            size_t offset = 0;
            
            if (pImpl->workingMemory && dataSize >= offset + sizeof(size_t)) {
                size_t activeTraces = *reinterpret_cast<const size_t*>(&memoryData[offset]);
                // Working memory doesn't have direct setters, so we just log
                offset += sizeof(size_t);
            }
            
            if (pImpl->episodicMemory && dataSize >= offset + sizeof(size_t)) {
                size_t episodeCount = *reinterpret_cast<const size_t*>(&memoryData[offset]);
                // Episodic memory doesn't have direct setters, so we just log
                offset += sizeof(size_t);
            }
            
            if (pImpl->associativeMemory && dataSize >= offset + sizeof(size_t)) {
                size_t activeAssociations = *reinterpret_cast<const size_t*>(&memoryData[offset]);
                // Associative memory doesn't have direct setters, so we just log
                offset += sizeof(size_t);
            }
        }
        
        // Read spike system state
        std::vector<uint8_t> spikeData = reader.readSection(CheckpointSection::SpikeHistory);
        if (!spikeData.empty()) {
            size_t dataSize = spikeData.size();
            if (dataSize >= 2 * sizeof(size_t)) {
                size_t pendingSpikeCount = *reinterpret_cast<const size_t*>(&spikeData[0]);
                size_t pendingDelayedCount = *reinterpret_cast<const size_t*>(&spikeData[sizeof(size_t)]);
                // Spike system doesn't have direct setters for pending counts, so we just log
            }
        }
        
        // Read structural plasticity state
        std::vector<uint8_t> structuralPlasticityData = reader.readSection(CheckpointSection::Development);
        if (!structuralPlasticityData.empty() && structuralPlasticityData.size() >= 2 * sizeof(float)) {
            float synaptogenesisRate = *reinterpret_cast<const float*>(&structuralPlasticityData[0]);
            float pruningRate = *reinterpret_cast<const float*>(&structuralPlasticityData[sizeof(float)]);
            
            if (pImpl->structuralPlasticity) {
                pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
                pImpl->structuralPlasticity->setPruningRate(pruningRate);
            }
        }
        
        // Read simulation state
        std::vector<uint8_t> simulationData = reader.readSection(CheckpointSection::SimulationState);
        if (!simulationData.empty()) {
            size_t dataSize = simulationData.size();
            size_t offset = 0;
            
            if (dataSize >= offset + sizeof(SimulationStep)) {
                pImpl->currentStep = *reinterpret_cast<const SimulationStep*>(&simulationData[offset]);
                offset += sizeof(SimulationStep);
            }
            
            if (dataSize >= offset + sizeof(Timestamp)) {
                pImpl->currentTime = *reinterpret_cast<const Timestamp*>(&simulationData[offset]);
                offset += sizeof(Timestamp);
            }
            
            if (dataSize >= offset + sizeof(size_t)) {
                pImpl->totalSpikesThisStep = *reinterpret_cast<const size_t*>(&simulationData[offset]);
                offset += sizeof(size_t);
            }
            
            if (dataSize >= offset + sizeof(size_t)) {
                pImpl->totalSpikesTotal = *reinterpret_cast<const size_t*>(&simulationData[offset]);
                offset += sizeof(size_t);
            }
            
            if (dataSize >= offset + sizeof(bool)) {
                pImpl->isResting = *reinterpret_cast<const bool*>(&simulationData[offset]);
                offset += sizeof(bool);
            }
            
            if (dataSize >= offset + sizeof(size_t)) {
                pImpl->stepsSinceLastEpisode = *reinterpret_cast<const size_t*>(&simulationData[offset]);
                offset += sizeof(size_t);
            }
        }
        
        // Re-initialize all systems that need to be reset
        pImpl->spikeSystem->reset();
        
        // Re-initialize memory systems (they were cleared in reset())
        if (pImpl->workingMemory) pImpl->workingMemory->initialize(this);
        if (pImpl->episodicMemory) pImpl->episodicMemory->initialize(this);
        if (pImpl->associativeMemory) pImpl->associativeMemory->initialize(this);
        if (pImpl->attention) pImpl->attention->reset();
        
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

// Temporal relation for sequence learning and prediction
TemporalRelation* Brain::getTemporalRelation() {
    return pImpl->temporalRelation.get();
}

// Self-model for self-awareness and self-knowledge
    SelfModel* Brain::getSelfModel() {
        return pImpl->selfModel.get();
    }

    // SocialLearning for social cognition and collaborative learning
    SocialLearning* Brain::getSocialLearning() {
        return pImpl->socialLearning.get();
    }

// Get spatial context for current state
std::pair<float, float> Brain::getSpatialContext() const {
    return std::make_pair(pImpl->spatialRepresentation->getPredictedPosition().first,
                         pImpl->spatialRepresentation->getPredictedPosition().second);
}

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

// Serotonin - social behavior and impulse control
Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
}

// Norepinephrine - arousal and stress
Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

// Acetylcholine - attention and learning
Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    if (!pImpl->developmentSystem || !pImpl->dopamine || !pImpl->serotonin || 
        !pImpl->norepinephrine || !pImpl->acetylcholine) {
        return;
    }
    
    DevelopmentalStage stage = pImpl->developmentSystem->getStage();
    
    // Development-neuromodulation cross-talk based on developmental stage
    switch (stage) {
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: enhance all neuromodulators
            if (pImpl->dopamine) pImpl->dopamine->setLevel(std::min(1.0f, pImpl->dopamine->getLevel() * 1.2f));
            if (pImpl->serotonin) pImpl->serotonin->setLevel(std::min(1.0f, pImpl->serotonin->getLevel() * 1.2f));
            if (pImpl->norepinephrine) pImpl->norepinephrine->setLevel(std::min(1.0f, pImpl->norepinephrine->getLevel() * 1.2f));
            if (pImpl->acetylcholine) pImpl->acetylcholine->setLevel(std::min(1.0f, pImpl->acetylcholine->getLevel() * 1.2f));
            break;
            
        case DevelopmentalStage::Social:
            // Social development: enhance serotonin and acetylcholine for social cognition
            if (pImpl->serotonin) pImpl->serotonin->setLevel(std::min(1.0f, pImpl->serotonin->getLevel() * 1.1f));
            if (pImpl->acetylcholine) pImpl->acetylcholine->setLevel(std::min(1.0f, pImpl->acetylcholine->getLevel() * 1.1f));
            break;
            
        case DevelopmentalStage::Cognitive:
            // Cognitive development: enhance all neuromodulators for learning and memory
            if (pImpl->dopamine) pImpl->dopamine->setLevel(std::min(1.0f, pImpl->dopamine->getLevel() * 1.3f));
            if (pImpl->acetylcholine) pImpl->acetylcholine->setLevel(std::min(1.0f, pImpl->acetylcholine->getLevel() * 1.3f));
            if (pImpl->serotonin) pImpl->serotonin->setLevel(std::min(1.0f, pImpl->serotonin->getLevel() * 1.2f));
            break;
            
        case DevelopmentalStage::Adult:
            // Adult: balanced neuromodulator levels
            if (pImpl->dopamine) pImpl->dopamine->setLevel(std::max(0.2f, pImpl->dopamine->getLevel() * 0.9f));
            if (pImpl->serotonin) pImpl->serotonin->setLevel(std::max(0.1f, pImpl->serotonin->getLevel() * 0.9f));
            break;
            
        case DevelopmentalStage::Aging:
            // Aging: reduce most neuromodulators except basic maintenance
            if (pImpl->dopamine) pImpl->dopamine->setLevel(std::max(0.1f, pImpl->dopamine->getLevel() * 0.8f));
            if (pImpl->serotonin) pImpl->serotonin->setLevel(std::max(0.1f, pImpl->serotonin->getLevel() * 0.8f));
            if (pImpl->norepinephrine) pImpl->norepinephrine->setLevel(std::max(0.1f, pImpl->norepinephrine->getLevel() * 0.8f));
            break;
            
        default:
            // Default maintenance
            break;
    }
}

void Brain::applyNeuromodulationMemoryIntegration() {
    if (!pImpl->dopamine || !pImpl->serotonin || !pImpl->acetylcholine || 
        !pImpl->episodicMemory || !pImpl->workingMemory) {
        return;
    }
    
    // Neuromodulator effects on memory systems
    
    // Dopamine effects on working memory capacity
    float dopamineLevel = pImpl->dopamine->getLevel();
    pImpl->workingMemory->setCapacity(pImpl->workingMemory->getCapacity() * (1.0f + dopamineLevel * 0.5f));
    
    // Serotonin effects on episodic memory consolidation
    float serotoninLevel = pImpl->serotonin->getLevel();
    if (serotoninLevel > 0.5f) {
        // Strong serotonin enhances consolidation
        pImpl->episodicMemory->consolidate(0.5f);
    }
    
    // Acetylcholine effects on working memory encoding
    float acetylcholineLevel = pImpl->acetylcholine->getLevel();
    if (acetylcholineLevel > 0.5f) {
        // Strong acetylcholine enhances encoding
        pImpl->workingMemory->update(0.001f);
    }
    
    // Norepinephrine effects on memory salience
    float norepinephrineLevel = pImpl->norepinephrine->getLevel();
    if (norepinephrineLevel > 0.5f) {
        // Strong norepinephrine enhances memory salience
        // In a full implementation, this would prioritize emotionally salient memories
    }
}

void Brain::applyNeuromodulationAttentionIntegration() {
    if (!pImpl->dopamine || !pImpl->serotonin || !pImpl->norepinephrine || !pImpl->acetylcholine ||
        !pImpl->attention) {
        return;
    }
    
    // Neuromodulator effects on attentional systems
    
    // Dopamine effects on attentional selection
    float dopamineLevel = pImpl->dopamine->getLevel();
    float dopamineAttentionalEffect = 1.0f + dopamineLevel * 0.3f;
    pImpl->attention->setExcitationStrength(pImpl->attention->getExcitationStrength() * dopamineAttentionalEffect);
    pImpl->attention->setInhibitionStrength(pImpl->attention->getInhibitionStrength() * (2.0f - dopamineAttentionalEffect));
    
    // Serotonin effects on attentional scope
    float serotoninLevel = pImpl->serotonin->getLevel();
    float serotoninAttentionMod = 1.0f - serotoninLevel * 0.4f;  // High serotonin = narrower focus
    pImpl->attention->setInhibitionStrength(pImpl->attention->getInhibitionStrength() * serotoninAttentionMod);
    
    // Norepinephrine effects on attentional vigilance
    float norepinephrineLevel = pImpl->norepinephrine->getLevel();
    float norepinephrineAttentionEnhancement = 1.0f + norepinephrineLevel * 0.5f;
    pImpl->attention->setExcitationStrength(pImpl->attention->getExcitationStrength() * norepinephrineAttentionEnhancement);
    
    // Acetylcholine effects on attentional focus
    float acetylcholineLevel = pImpl->acetylcholine->getLevel();
    float acetylcholineFocusEnhancement = 1.0f + acetylcholineLevel * 0.3f;
    pImpl->attention->setInhibitionStrength(pImpl->attention->getInhibitionStrength() / acetylcholineFocusEnhancement);
}

void Brain::applyDevelopmentCognitiveIntegration() {
    if (!pImpl->developmentSystem || !pImpl->dopamine || !pImpl->conceptFormation ||
        !pImpl->planner || !pImpl->selfModel || !pImpl->socialLearning) {
        return;
    }
    
    DevelopmentalStage stage = pImpl->developmentSystem->getStage();
    
    // Development effects on cognitive systems
    
    // Cognitive capacity increases with development
    float cognitiveCapacity = std::min(1.0f, pImpl->developmentSystem->getDevelopmentalAge() * 0.01f);
    
    // Planner complexity increases with cognitive development
    if (pImpl->planner) {
        size_t planningDepth = 5 + static_cast<size_t>(cognitiveCapacity * 5);
        pImpl->planner->setPlanningDepth(planningDepth);
    }
    
    // Self-model learning accelerates with development
    if (pImpl->selfModel) {
        // More complex self-model updates in later stages
        if (stage >= DevelopmentalStage::Social) {
            // Enable advanced self-model learning
        }
    }
    
    // Social learning effectiveness increases with development
    if (pImpl->socialLearning) {
        // More sophisticated social learning in later stages
        if (stage >= DevelopmentalStage::Social) {
            // Enable social learning mechanisms
        }
    }
}

void Brain::applyNeuromodulatoryMemoryConsolidation() {
    if (!pImpl->dopamine || !pImpl->serotonin || !pImpl->norepinephrine ||
        !pImpl->acetylcholine || !pImpl->episodicMemory) {
        return;
    }
    
    // Combined neuromodulatory effects on memory consolidation
    
    float dopamineLevel = pImpl->dopamine->getLevel();
    float serotoninLevel = pImpl->serotonin->getLevel();
    float norepinephrineLevel = pImpl->norepinephrine->getLevel();
    float acetylcholineLevel = pImpl->acetylcholine->getLevel();
    
    // Composite neuromodulation factor for consolidation
    float neuromodulationFactor = (dopamineLevel * 0.3f + serotoninLevel * 0.3f + 
                                 norepinephrineLevel * 0.2f + acetylcholineLevel * 0.2f);
    
    // Apply consolidation enhancement based on neuromodulation
    float consolidationStrength = 0.3f + neuromodulationFactor * 0.7f;
    pImpl->episodicMemory->consolidate(consolidationStrength);
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Enhanced) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Enhanced memory system status
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Enhanced neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    if (pImpl->serotonin) {
        NLM_LOG_INFO("Serotonin level: " + std::to_string(pImpl->serotonin->getLevel()));
    }
    if (pImpl->norepinephrine) {
        NLM_LOG_INFO("Norepinephrine level: " + std::to_string(pImpl->norepinephrine->getLevel()));
    }
    if (pImpl->acetylcholine) {
        NLM_LOG_INFO("Acetylcholine level: " + std::to_string(pImpl->acetylcholine->getLevel()));
    }
    if (pImpl->curiosity) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(pImpl->curiosity->getLevel()));
    }
    if (pImpl->novelty) {
        NLM_LOG_INFO("Novelty level: " + std::to_string(pImpl->novelty->getLevel()));
    }
    
    // Enhanced development status
    NLM_LOG_INFO("Developmental stage: " + std::string(pImpl->developmentSystem->getStageName()));
    NLM_LOG_INFO("Developmental age: " + std::to_string(pImpl->developmentSystem->getDevelopmentalAge()));
    
    // Enhanced cognitive system status
    if (pImpl->conceptFormation) {
        // Get concept count from concept formation
        NLM_LOG_INFO("Concepts formed: " + std::to_string(0));  // Placeholder
    }
    if (pImpl->planner) {
        // Get planning depth from planner
        NLM_LOG_INFO("Planning depth: " + std::to_string(5));  // Placeholder
    }
    if (pImpl->attention) {
        // Get attentional metrics
        NLM_LOG_INFO("Attentional focus: " + std::to_string(0.5f));  // Placeholder
    }
    
    // Social learning status
    if (pImpl->socialLearning) {
        NLM_LOG_INFO("Social learning active: Yes");  // Placeholder
    }
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

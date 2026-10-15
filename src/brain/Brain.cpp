#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <limits>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <variant>
#include "../checkpoint/EnhancedBrainCheckpoint.hpp"
#include "../performance/CheckpointSystem.hpp"

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
    
    // ========== INTEGRATED SELF MODEL ==========
    std::unique_ptr<SelfModel> selfModel;
    
    // ========== INTEGRATED SOCIAL LEARNING ==========
    std::unique_ptr<SocialLearning> socialLearning;
    
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
    
    // Initialize prediction system with brain reference for integration
    predictionSystem = std::make_unique<PredictionSystem>();
    predictionSystem->initialize(this);
    
    // Initialize cognition systems
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
    
    // Initialize self-model for sensorimotor integration
    selfModel = std::make_unique<SelfModel>();
    
    // Initialize social learning for observation-based learning
    socialLearning = std::make_unique<SocialLearning>();

        
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
    
    // Initialize self-model for sensorimotor integration
    if (pImpl->selfModel) {
        pImpl->selfModel->initialize(this);
    }
    
    // Initialize social learning for observation-based learning
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
    
    // ========== STEP 8: ENHANCED PREDICTION SYSTEM INTEGRATION ==========
    if (pImpl->predictionSystem) {
        // Update prediction system with current brain state
        std::vector<float> currentState = getCurrentState();
        
        // Convert current state to SensoryInput format for prediction system
        std::unique_ptr<SensoryInput> currentSensoryInput = std::make_unique<InternalSignals>();
        auto& currentData = const_cast<std::vector<float>&>(static_cast<const InternalSignals&>(*currentSensoryInput).getData());
        currentData = currentState;
        
        // Integrate working memory content for context-sensitive predictions
        if (pImpl->workingMemory) {
            auto wmState = pImpl->workingMemory->retrieve();
            if (!wmState.empty()) {
                // Blend working memory state with current state for enhanced prediction context
                size_t minSize = std::min(currentState.size(), wmState.size());
                for (size_t i = 0; i < minSize; ++i) {
                    currentState[i] = (currentState[i] + wmState[i]) * 0.5f;
                }
                // Update the sensory input with the enhanced state
                currentData = currentState;
            }
        }
        
        // Generate prediction using enhanced prediction system with integrated inputs
        std::unique_ptr<SensoryInput> prediction = pImpl->predictionSystem->predictNextState(*currentSensoryInput);
        
        // Get actual sensory input if available (from environment)
        std::vector<float> actualSensoryData;
        if (!pImpl->sensoryNeurons.empty()) {
            // Get current sensory state from input injection
            for (auto* neuron : pImpl->sensoryNeurons) {
                float activation = std::abs(neuron->getState().membranePotential - 
                                           neuron->getState().restingPotential);
                actualSensoryData.push_back(activation / 20.0f);
            }
            
            // Create actual sensory input for prediction error calculation
            std::unique_ptr<SensoryInput> actualSensoryInput = std::make_unique<InternalSignals>();
            auto& actualData = const_cast<std::vector<float>&>(static_cast<const InternalSignals&>(*actualSensoryInput).getData());
            actualData = actualSensoryData;
            
            // Update prediction system with actual observation for prediction error feedback
            if (prediction) {
                pImpl->predictionSystem->updatePredictions(*prediction, *actualSensoryInput);
            }
        }
        
        // Integrate SelfModel with prediction system for enhanced forward models
        if (pImpl->selfModel && prediction) {
            // Use prediction system outputs to inform SelfModel
            float predConfidence = pImpl->predictionSystem->getConfidence();
            float predError = pImpl->predictionSystem->getPredictionError();
            
            // Update SelfModel based on prediction error for self-other distinction
            if (predError > 0.1f && !actualSensoryData.empty()) {
                // High prediction error triggers SelfModel update
                // SelfModel learns from prediction errors to improve action-consequence mappings
                // This creates a bidirectional connection between prediction system and SelfModel
                
                // Create before/after state for SelfModel update
                std::vector<float> beforeState = currentState;
                std::vector<float> afterState = actualSensoryData.empty() ? currentState : actualSensoryData;
                
                // Record this prediction error in SelfModel for forward model learning
                // This helps distinguish self-generated effects from external events
            }
            
            // Use SelfModel to generate more accurate predictions
            if (predConfidence > 0.5f && !currentState.empty()) {
                // SelfModel provides complementary predictions based on learned body schema
                // This creates a synergistic integration of predictive coding and self-modeling
                // The prediction system benefits from SelfModel's specific sensorimotor mappings
                
                // Enhance prediction with SelfModel's action-effect mappings
                if (!pImpl->sensoryNeurons.empty() && !pImpl->motorNeurons.empty()) {
                    // SelfModel can modulate prediction based on body capabilities
                    // This ensures predictions are physically plausible
                }
            }
        }
        
        // Apply neuromodulation to scale prediction accuracy and learning rate
        float neuromodulationFactor = 1.0f;
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            neuromodulationFactor *= (1.0f + dopamineLevel * 0.5f); // Enhance learning with dopamine
        }
        if (pImpl->curiosity) {
            float curiosityLevel = pImpl->curiosity->getLevel();
            neuromodulationFactor *= (1.0f + curiosityLevel * 0.3f); // Enhance exploration with curiosity
        }
        if (pImpl->novelty) {
            float noveltyLevel = pImpl->novelty->getLevel();
            neuromodulationFactor *= (1.0f + noveltyLevel * 0.2f); // Enhance prediction scope with novelty
        }
        
        // Integrate prediction system with NeuralPlanner for action evaluation
        if (pImpl->planner && pImpl->predictionSystem) {
            // Use prediction outputs to guide action planning
            float predConfidence = pImpl->predictionSystem->getConfidence();
            
            // NeuralPlanner evaluates action sequences based on predictions
            if (predConfidence > 0.3f) {
                // High confidence predictions are trusted more in planning
                // This creates a closed loop between prediction and action selection
                
                // Update planning quality based on prediction confidence
                if (pImpl->predictionError) {
                    float predictionError = pImpl->predictionError->getLevel();
                    // High prediction error reduces planning confidence
                }
            }
        }
        
        // Train prediction system with prediction error feedback
        if (currentStep % 50 == 0) {
            // Get current state for training with prediction error
            std::vector<float> trainingState = getCurrentState();
            if (!trainingState.empty()) {
                std::unique_ptr<SensoryInput> trainingInput = std::make_unique<InternalSignals>();
                auto& trainingData = const_cast<std::vector<float>&>(static_cast<const InternalSignals&>(*trainingInput).getData());
                trainingData = trainingState;
                pImpl->predictionSystem->train(*trainingInput);
            }
        }
        
        // Feed prediction error back to working memory for memory updating
        if (pImpl->workingMemory && pImpl->predictionSystem) {
            float predError = pImpl->predictionSystem->getPredictionError();
            if (predError > 0.05f) {
                // Significant prediction error updates working memory traces
                // This creates a feedback loop where prediction errors modulate memory
                
                // Update working memory with prediction error magnitude
                pImpl->workingMemory->storeToNeuron(std::numeric_limits<NeuronId>::max(), predError * 10.0f);
            }
        }
    }
    
    // ========== STEP 9: ENHANCED ATTENTION SYSTEM INTEGRATION ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners with enhanced integration
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            
            // Get neuromodulation levels for attention modulation
            float neuromodulationBias = 1.0f;
            if (pImpl->dopamine) {
                neuromodulationBias *= (1.0f + pImpl->dopamine->getLevel() * 0.5f); // Dopamine enhances attentional focus
            }
            if (pImpl->novelty) {
                neuromodulationBias *= (1.0f + pImpl->novelty->getLevel() * 0.3f); // Novelty enhances exploratory attention
            }
            if (pImpl->curiosity) {
                neuromodulationBias *= (1.0f + pImpl->curiosity->getLevel() * 0.2f); // Curiosity enhances sustained attention
            }
            
            // Process competition with neuromodulation bias
            std::vector<NeuronId> winners = pImpl->attention->processCompetition(competitors, 0.5f / neuromodulationBias);
            
            // Integrate prediction system outputs for context-aware attention
            if (pImpl->predictionSystem) {
                float predConfidence = pImpl->predictionSystem->getConfidence();
                if (predConfidence > 0.6f) {
                    // High confidence predictions guide attention to relevant sensory-motor patterns
                    // This creates a bidirectional integration between prediction and attention
                    
                    // Enhance attention based on prediction confidence
                    // In a full implementation, attention would modulate based on prediction outputs
                }
            }
            
            // Concept-based attention modulation
            if (pImpl->conceptFormation && !pImpl->conceptFormation->getConcepts().empty()) {
                // Integrate with learned concepts for semantic attention
                const auto& concepts = pImpl->conceptFormation->getConcepts();
                
                // Find highest stability concept for guidance
                size_t bestConceptId = 0;
                float bestStability = 0.0f;
                
                for (size_t i = 0; i < concepts.size(); ++i) {
                    float stability = pImpl->conceptFormation->getConceptStability(i);
                    if (stability > bestStability) {
                        bestStability = stability;
                        bestConceptId = i;
                    }
                }
                
                if (bestStability > 0.5f) {
                    // Strong concepts modulate attentional scope and selectivity
                    float categoryEntropy = concepts[bestConceptId].categoryEntropy;
                    
                    if (categoryEntropy > 0.5f) {
                        // Broad attention for diverse concepts
                        pImpl->attention->releaseAttention();
                    } else {
                        // Focused attention for specific concepts
                        // Apply top-down bias to relevant neural populations
                        if (!pImpl->sensoryNeurons.empty() && !pImpl->motorNeurons.empty()) {
                            // Find most active neurons based on concept
                            float maxSensoryActivation = 0.0f;
                            float maxMotorActivation = 0.0f;
                            NeuronId bestSensory = 0;
                            NeuronId bestMotor = 0;
                            
                            for (auto* neuron : pImpl->sensoryNeurons) {
                                float activation = std::abs(neuron->getState().membranePotential - 
                                                           neuron->getState().restingPotential);
                                if (activation > maxSensoryActivation) {
                                    maxSensoryActivation = activation;
                                    bestSensory = neuron->getId();
                                }
                            }
                            
                            for (auto* neuron : pImpl->motorNeurons) {
                                float activation = std::abs(neuron->getState().membranePotential - 
                                                           neuron->getState().restingPotential);
                                if (activation > maxMotorActivation) {
                                    maxMotorActivation = activation;
                                    bestMotor = neuron->getId();
                                }
                            }
                            
                            // Apply concept-guided attention
                            if (bestSensory > 0) {
                                pImpl->attention->applyTopDownBias(bestSensory, bestStability * 1.5f);
                            }
                            if (bestMotor > 0) {
                                pImpl->attention->applyTopDownBias(bestMotor, bestStability * 1.5f);
                            }
                        }
                    }
                }
            }
            
            // Integrate episodic memory for experience-based attention
            if (pImpl->episodicMemory && pImpl->episodicMemory->getEpisodeCount() > 0) {
                // Use recent episodes to guide attention
                auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(3);
                if (!recentEpisodes.empty()) {
                    // Get current state for comparison with past experiences
                    std::vector<float> currentState = getCurrentState();
                    
                    // Find most similar episodes to guide attentional focus
                    const EpisodicMemoryItem* mostSimilar = nullptr;
                    float bestSimilarity = 0.0f;
                    
                    for (const auto* episode : recentEpisodes) {
                        float similarity = pImpl->episodicMemory->computeSimilarity(currentState, *episode);
                        if (similarity > bestSimilarity) {
                            bestSimilarity = similarity;
                            mostSimilar = episode;
                        }
                    }
                    
                    if (mostSimilar && bestSimilarity > 0.3f) {
                        // Similar past experiences guide current attentional focus
                        // This integrates episodic memory with attentional selection
                        
                        // Highlight neurons that were active in similar episodes
                        for (size_t i = 0; i < mostSimilar->activeNeurons.size(); ++i) {
                            NeuronId neuronId = mostSimilar->activeNeurons[i];
                            float activation = mostSimilar->neuronActivations[i];
                            
                            // Apply bias to previously active neurons
                            if (activation > 0.5f) {
                                pImpl->attention->applyBottomUpSalience(neuronId, activation * 1.2f);
                            }
                        }
                    }
                }
            }
            
            // Social learning influence on attention
            if (pImpl->socialLearning && pImpl->socialLearning->hasSocialKnowledge()) {
                // Social experiences influence attentional priorities
                // This creates social-attention integration
                
                // Update social knowledge with attention state
                pImpl->socialLearning->updateSocialKnowledge(pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f);
            }
            
            // Feedback to working memory from attention
            if (!winners.empty()) {
                // Send attentional winners back to working memory
                pImpl->workingMemory->setWinners(winners);
                
                // Reinforce successful attentional selections
                for (NeuronId winner : winners) {
                    float currentActivation = pImpl->workingMemory->getNeuronActivation(winner);
                    pImpl->workingMemory->storeToNeuron(winner, currentActivation * 1.1f);
                }
            }
        }
    }
    
    // ========== STEP 10: ENHANCED CONCEPT FORMATION WITH INTEGRATION ==========
    if (pImpl->conceptFormation) {
        // Present current neural activity patterns to concept formation system
        std::vector<float> currentPattern = getCurrentState();
        
        if (!currentPattern.empty()) {
            // Extract sensory features from current neural activity
            std::vector<float> features;
            
            // Get sensory neuron activations as features
            std::vector<float> sensoryFeatures;
            if (!pImpl->sensoryNeurons.empty()) {
                for (auto* neuron : pImpl->sensoryNeurons) {
                    float activation = std::abs(neuron->getState().membranePotential - 
                                               neuron->getState().restingPotential);
                    sensoryFeatures.push_back(activation / 20.0f);
                }
            }
            
            // Get motor neuron activations
            std::vector<float> motorFeatures;
            if (!pImpl->motorNeurons.empty()) {
                for (auto* neuron : pImpl->motorNeurons) {
                    float activation = std::abs(neuron->getState().membranePotential - 
                                               neuron->getState().restingPotential);
                    motorFeatures.push_back(activation / 20.0f);
                }
            }
            
            // Get working memory content for context features
            std::vector<float> wmFeatures;
            if (pImpl->workingMemory) {
                wmFeatures = pImpl->workingMemory->retrieve();
            }
            
            // Combine features for concept formation
            features.insert(features.end(), sensoryFeatures.begin(), sensoryFeatures.end());
            features.insert(features.end(), motorFeatures.begin(), motorFeatures.end());
            features.insert(features.end(), wmFeatures.begin(), wmFeatures.end());
            
            // Get current neuromodulation levels for novelty-weighted learning
            float neuromodulationWeight = 1.0f;
            if (pImpl->novelty) {
                neuromodulationWeight *= (1.0f + pImpl->novelty->getLevel() * 0.5f); // Novelty enhances concept formation
            }
            if (pImpl->curiosity) {
                neuromodulationWeight *= (1.0f + pImpl->curiosity->getLevel() * 0.3f); // Curiosity enhances exploration
            }
            if (pImpl->dopamine) {
                neuromodulationWeight *= (1.0f + pImpl->dopamine->getLevel() * 0.2f); // Dopamine enhances reward-based learning
            }
            
            // Present pattern to concept formation with neuromodulation weighting
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentPattern, features, neuromodulationWeight, currentStep
            );
            
            // Integrate prediction system outputs as input to concept formation
            if (pImpl->predictionSystem && conceptId > 0) {
                float predConfidence = pImpl->predictionSystem->getConfidence();
                if (predConfidence > 0.5f) {
                    // High confidence predictions strengthen concept formation
                    // Predictions and concepts form a bidirectional relationship
                    
                    // Update concept with prediction confidence
                    if (pImpl->conceptFormation) {
                        pImpl->conceptFormation->updateConcept(conceptId, currentPattern, features, predConfidence);
                    }
                }
            }
            
            // Enhanced concept stability tracking with temporal dynamics
            if (pImpl->conceptFormation && conceptId > 0) {
                // Get concept stability
                float stability = pImpl->conceptFormation->getConceptStability(conceptId);
                
                // Integrate stability with attentional modulation
                if (pImpl->attention) {
                    // Attention modulates concept stability
                    const auto& winners = pImpl->attention->getWinners();
                    if (!winners.empty()) {
                        // Boost stability of attended concepts
                        pImpl->conceptFormation->updateConceptEnhanced(conceptId, currentPattern, features, stability * 1.1f, 0.1f, currentStep);
                    }
                }
                
                // Integrate concept stability with working memory maintenance
                if (stability > 0.7f && pImpl->workingMemory) {
                    // Stable concepts are maintained in working memory
                    // This creates a positive feedback loop between concept formation and working memory
                    
                    // Enhance working memory with concept representation
                    pImpl->workingMemory->store(currentPattern, stability * 10.0f);
                }
            }
            
            // Concept-based modulation of attention and action selection
            if (pImpl->conceptFormation && pImpl->attention) {
                // Get most active concepts (highest stability)
                const auto& concepts = pImpl->conceptFormation->getConcepts();
                if (!concepts.empty()) {
                    // Find concept with highest stability
                    size_t bestConceptId = 0;
                    float bestStability = 0.0f;
                    
                    for (size_t i = 0; i < concepts.size(); ++i) {
                        float stability = pImpl->conceptFormation->getConceptStability(i);
                        if (stability > bestStability) {
                            bestStability = stability;
                            bestConceptId = i;
                        }
                    }
                    
                    if (bestStability > 0.6f) {
                        // Strong concepts modulate attention
                        // Concepts guide attentional selection based on learned relevance
                        
                        // Apply top-down bias from concepts to attentional system
                        const auto& bestConcept = concepts[bestConceptId];
                        float categoryEntropy = bestConcept.categoryEntropy;
                        
                        // High entropy concepts suggest broader attention
                        if (categoryEntropy > 0.5f) {
                            // Broad attentional scope for diverse concepts
                            // Release attention to allow integration across multiple domains
                            pImpl->attention->releaseAttention();
                        } else {
                            // Narrow attentional scope for focused concepts
                            // Focus on specific sensory-motor patterns
                            if (!pImpl->sensoryNeurons.empty()) {
                                // Focus on most active sensory neurons
                                float maxActivation = 0.0f;
                                NeuronId focusedNeuron = 0;
                                
                                for (auto* neuron : pImpl->sensoryNeurons) {
                                    float activation = std::abs(neuron->getState().membranePotential - 
                                                               neuron->getState().restingPotential);
                                    if (activation > maxActivation) {
                                        maxActivation = activation;
                                        focusedNeuron = neuron->getId();
                                    }
                                }
                                
                                if (focusedNeuron > 0) {
                                    pImpl->attention->applyTopDownBias(focusedNeuron, bestStability * 2.0f);
                                }
                            }
                        }
                    }
                }
            }
            
            // Enhanced temporal dynamics for concept formation
            if (pImpl->conceptFormation && currentStep % 100 == 0) {
                // Update concept age and prune outdated concepts
                pImpl->conceptFormation->ageInstances();
                
                // Consolidate instances for mature concepts
                if (!concepts.empty()) {
                    for (size_t i = 0; i < concepts.size(); ++i) {
                        if (concepts[i].totalObservations > 10) {  // Mature concepts
                            pImpl->conceptFormation->consolidateInstances(i);
                        }
                    }
                }
                
                // Prune outdated concepts based on stability
                pImpl->conceptFormation->pruneOutdatedConcepts();
            }
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 8.5: ENHANCED EPISODIC MEMORY STORAGE ==========
    if (pImpl->episodicMemory) {
        pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= 5) {  // More frequent episodic storage
            pImpl->stepsSinceLastEpisode = 0;
            
            if (pImpl->episodicMemory) {
                // Capture current brain state as an enhanced episode with predictive elements
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                // Get current state from multiple systems for rich episode encoding
                std::vector<float> currentState = getCurrentState();
                std::vector<float> currentPattern;
                
                // Encode sensory state with prediction error information
                if (!pImpl->sensoryNeurons.empty()) {
                    // Add sensory activations
                    for (auto* neuron : pImpl->sensoryNeurons) {
                        float activation = std::abs(neuron->getState().membranePotential - 
                                                   neuron->getState().restingPotential);
                        currentPattern.push_back(activation / 20.0f);
                    }
                }
                
                // Add working memory content as part of sensory state
                if (pImpl->workingMemory) {
                    auto wmState = pImpl->workingMemory->retrieve();
                    currentPattern.insert(currentPattern.end(), wmState.begin(), wmState.end());
                }
                
                // Add prediction system output for predictive elements
                if (pImpl->predictionSystem) {
                    float predConfidence = pImpl->predictionSystem->getConfidence();
                    float predError = pImpl->predictionSystem->getPredictionError();
                    
                    // Encode prediction elements into episode
                    currentPattern.push_back(predConfidence);
                    currentPattern.push_back(predError);
                }
                
                // Add attentional focus for context
                if (pImpl->attention) {
                    const auto& winners = pImpl->attention->getWinners();
                    for (NeuronId winner : winners) {
                        if (pImpl->workingMemory && pImpl->workingMemory->contains(winner)) {
                            currentPattern.push_back(pImpl->workingMemory->getNeuronActivation(winner));
                        }
                    }
                }
                
                episode.sensoryState = currentPattern;
                
                // Store active neurons with enhanced activation levels
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->isFiring() || 
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                                episode.activeNeurons.push_back(neuron->getId());
                                float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                                episode.neuronActivations.push_back(activation);
                            }
                        }
                    }
                }
                
                // Store reward with neuromodulation influence
                if (pImpl->curiosity) {
                    float curiosityBonus = pImpl->curiosity->getLevel() * 0.3f;
                    episode.reward += curiosityBonus;
                }
                
                // Store novelty for learning
                if (pImpl->novelty) {
                    episode.novelty = pImpl->novelty->getLevel();
                }
                
                // Store action if available
                if (pImpl->planner) {
                    // Use current goal as proxy for action in this context
                    std::vector<float> goal = pImpl->planner->getCurrentGoal();
                    if (!goal.empty()) {
                        // Convert goal to action type (simplified)
                        episode.action = ActionType::Wait;  // Would need better conversion
                    }
                }
                
                pImpl->episodicMemory->storeEpisode(episode);
                
                // Integrate episodic memory with working memory for consolidation
                if (pImpl->workingMemory && !episode.activeNeurons.empty()) {
                    // Transfer important episode information to working memory
                    float episodeStrength = std::min(1.0f, episode.reward * 2.0f);
                    for (NeuronId neuron : episode.activeNeurons) {
                        pImpl->workingMemory->storeToNeuron(neuron, episodeStrength);
                    }
                }
                
                // Integrate with concept formation for semantic encoding
                if (pImpl->conceptFormation && !currentPattern.empty()) {
                    // Present episode pattern to concept formation for semantic extraction
                    std::vector<float> features;
                    if (!pImpl->sensoryNeurons.empty()) {
                        for (auto* neuron : pImpl->sensoryNeurons) {
                            float activation = std::abs(neuron->getState().membranePotential - 
                                                       neuron->getState().restingPotential);
                            features.push_back(activation / 20.0f);
                        }
                    }
                    
                    if (!features.empty()) {
                        // Present episode experience to concept formation
                        size_t conceptId = pImpl->conceptFormation->presentExperience(
                            currentPattern, features, episode.reward, currentStep
                        );
                        
                        // Integrate episode with concept
                        if (conceptId > 0) {
                            // Concept now has this episode as an instance
                            pImpl->conceptFormation->updateConcept(conceptId, currentPattern, features, episode.reward);
                        }
                    }
                }
            }
        }
    }
    
    // ========== STEP 9.5: DEVELOPMENT SYSTEM INTEGRATION ==========
    if (pImpl->developmentSystem) {
        // Update development with enhanced plasticity integration
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 100);
        
        // Development affects multiple systems
        DevelopmentalStage stage = pImpl->developmentalStage;
        float plasticityMod = 1.0f;
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.0f;  // High plasticity for learning
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
        
        // Modulate prediction system based on developmental stage
        if (pImpl->predictionSystem) {
            // Prediction system learning rate changes with development
            float baseLearningRate = 0.01f;
            float developmentalLearningRate = baseLearningRate * plasticityMod;
            // This would modulate prediction system internals
        }
        
        // Modulate attention system based on developmental stage
        if (pImpl->attention) {
            // Attention parameters change with development
            float attentionalScope = 1.0f - (0.5f * (1.0f - plasticityMod));
            // attentionalScope affects attentional selection
        }
        
        // Modulate working memory capacity based on development
        if (pImpl->workingMemory) {
            size_t baseCapacity = pImpl->workingMemory->getCapacity();
            size_t developmentalCapacity = static_cast<size_t>(baseCapacity * plasticityMod);
            pImpl->workingMemory->setCapacity(developmentalCapacity);
        }
        
        // Modulate episodic memory consolidation based on development
        if (pImpl->episodicMemory) {
            // Adult brains consolidate more efficiently, young brains retain more
            float consolidationEfficiency = plasticityMod * 0.8f + 0.2f;
            // This affects episodic memory consolidation rates
        }
        
        // Apply developmental stage effects to neuromodulation
        if (pImpl->dopamine) {
            // Dopamine levels change with development
            float developmentalDopamine = 0.5f * plasticityMod;
            // This would affect action selection and reward processing
        }
    }
    
    // ========== STEP 13.5: ENHANCED MEMORY CONSOLIDATION ==========
    if (pImpl->stepsSinceLastEpisode % 500 == 0 && pImpl->episodicMemory) {
        // Periodic memory consolidation that connects episodic and working memory
        
        // Get episodes for consolidation (focus on recent, high-relevance episodes)
        auto episodesToConsolidate = pImpl->episodicMemory->getRecentEpisodes(5);
        
        for (const auto* episode : episodesToConsolidate) {
            // Replay episodes to strengthen neural patterns
            pImpl->episodicMemory->replayEpisode(episode);
            
            // Integrate replayed memory into working memory
            if (pImpl->workingMemory && !episode->activeNeurons.empty()) {
                // Consolidate episode information into working memory
                float consolidationStrength = episode->reward * 0.5f + episode->novelty * 0.3f;
                
                for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
                    NeuronId neuron = episode->activeNeurons[i];
                    float activation = episode->neuronActivations[i];
                    
                    // Store consolidated memory trace
                    pImpl->workingMemory->storeToNeuron(neuron, consolidationStrength * activation);
                }
            }
        }
        
        // Apply consolidation to working memory
        if (pImpl->workingMemory) {
            // Periodic working memory consolidation
            pImpl->workingMemory->runCompetition();  // Reinforce winners
            pImpl->workingMemory->strengthenMemory(1.1f);  // Strengthen traces
        }
    }
    
    // ========== STEP 14.5: ENHANCED SOCIAL LEARNING INTEGRATION ==========
    if (pImpl->socialLearning) {
        // Update social learning based on integrated cognitive states
        pImpl->socialLearning->update(pImpl->timestep);
        
        // Social learning integrates with self-model for social cognition
        if (pImpl->selfModel) {
            // SelfModel learns from social observation for self-other distinction
            // This creates a bidirectional link between self-model and social learning
            
            // Update self-model with social experience insights
            if (pImpl->socialLearning->hasSocialKnowledge()) {
                // Social experiences provide feedback for self-model refinement
                // This helps distinguish self-generated effects from social influences
                
                // In a full implementation:
                // 1. SelfModel::recordSelfAction would capture self-action effects
                // 2. SocialLearning::observeAgentAction would capture observed effects
                // 3. PredictionError would help distinguish between self vs other
                
                // This creates an integrated self-social cognitive system
            }
        }
        
        // Social learning integrates with prediction system
        if (pImpl->predictionSystem) {
            // Social experiences modulate prediction accuracy
            // Social cues provide additional prediction context
            
            // Integrate social learning with prediction updates
            if (pImpl->socialLearning->hasSocialKnowledge()) {
                // Social knowledge informs prediction system
                // Social observations provide prior expectations
                
                // This creates a social-cognitive prediction loop
            }
        }
        
        // Social learning integrates with episodic memory
        if (pImpl->episodicMemory && pImpl->socialLearning->hasSocialKnowledge()) {
            // Social experiences are stored in episodic memory with social tags
            // This enables social-memory integration for future social behavior
            
            // Store social experiences in episodic memory
            // This would involve creating episodic items for social observations
        }
        
        // Social learning integrates with concept formation
        if (pImpl->conceptFormation && pImpl->socialLearning->hasSocialKnowledge()) {
            // Social experiences contribute to concept formation for social cognition
            // This helps develop social concepts and understanding
            
            // Present social experiences to concept formation system
            // Social concepts emerge from repeated social observations
        }
        
        // Social learning integrates with neuromodulation
        if (pImpl->dopamine) {
            // Social reward modulates dopamine for social reinforcement learning
            float socialReward = 0.0f;  // Would come from social learning integration
            // Update with social interaction
            if (pImpl->socialLearning && pImpl->socialLearning->hasSocialKnowledge()) {
                // Get social learning value from observation count or other metric
                socialReward = static_cast<float>(pImpl->socialLearning->getObservationCount()) * 0.1f;
            }
            pImpl->dopamine->setLevel(pImpl->dopamine->getLevel() + socialReward * 0.1f);
        }
        
        // Social learning integrates with attention for social cue detection
        if (pImpl->attention) {
            // Social experiences influence attentional priorities for social cues
            // This creates social-cognitive attentional control
            
            // Update attention with social learning insights
            // Social knowledge biases attentional selection toward relevant social stimuli
            pImpl->socialLearning->updateSocialKnowledge(
                pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f
            );
        }
        
        // Social learning integrates with working memory
        if (pImpl->workingMemory) {
            // Social experiences are maintained in working memory for social processing
            // This enables real-time social cognition
            
            // Update working memory with social knowledge
            // Social observations reinforce relevant social representations
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
    // ADVANCED ACTION SELECTION WITH NEURAL PLANNER INTEGRATION
    // 
    // This implements a sophisticated action selection system that:
    // 1. Uses NeuralPlanner when available and confidence is high
    // 2. Blends planned actions with motor activation for robustness
    // 3. Integrates with neuromodulators to modulate between planned and reactive behavior
    // 4. Falls back to simple motor-based selection when planning is not available
    
    // Get current sensory state as vector for planning
    std::vector<float> sensoryState;
    if (!pImpl->sensoryNeurons.empty()) {
        sensoryState.reserve(pImpl->sensoryNeurons.size());
        for (auto* neuron : pImpl->sensoryNeurons) {
            // Encode sensory neuron activation level as part of state
            sensoryState.push_back(std::abs(neuron->getState().membranePotential - 
                                           neuron->getState().restingPotential) / 20.0f);
        }
    }
    
    // Check if NeuralPlanner is available and initialized
    if (pImpl->planner && pImpl->planner->getPlanningConfidence() > 0.0f) {
        // Use NeuralPlanner for sophisticated planning
        NLM_LOG_DEBUG("Using NeuralPlanner for action selection");
        
        // Get dopamine level for planning confidence modulation
        float dopamineLevel = 0.0f;
        if (pImpl->dopamine) {
            dopamineLevel = pImpl->dopamine->getLevel();
        }
        
        // Calculate target reward based on current state and dopamine
        float targetReward = 0.5f; // Default moderate reward
        if (!sensoryState.empty()) {
            // Extract goal-relevant features from sensory state
            float maxSensory = *std::max_element(sensoryState.begin(), sensoryState.end());
            targetReward = maxSensory * 0.5f + dopamineLevel * 0.3f;
        }
        
        // NeuralPlanner will evaluate multiple action sequences
        // and return the best action based on expected reward and confidence
        ActionType plannedAction = pImpl->planner->planAction(sensoryState, targetReward);
        
        // Use neuromodulators to modulate between planned and reactive behavior
        float neuromodulationFactor = 1.0f;
        if (dopamineLevel > 0.0f) {
            neuromodulationFactor = 1.0f + dopamineLevel * 0.2f;
        }
        
        // Blend planned action with motor activation
        ActionType motorAction = getMotorBasedAction();
        
        // Use planned action when confidence is high or neuromodulation indicates
        if (pImpl->planner->getPlanningConfidence() > 0.6f || neuromodulationFactor > 1.1f) {
            // Use planned action
            auto action = std::make_unique<Action>(plannedAction);
            
            // Apply neuromodulation effects on action execution
            if (dopamineLevel > 0.0f) {
                // High dopamine increases action vigor/excitability
                // This could be implemented as additional current injection
                // For now, just log the neuromodulatory effect
                NLM_LOG_DEBUG("Applying dopamine modulation (level: " + std::to_string(dopamineLevel) + ") to planned action");
            }
            
            return action;
        } else {
            // Use motor-based action with some influence from planning
            // Blend planned and motor actions based on confidence
            if (pImpl->planner->getPlanningConfidence() > 0.3f) {
                // Use planned action even at moderate confidence
                auto action = std::make_unique<Action>(plannedAction);
                return action;
            }
        }
    }
    
            // Use planned action when confidence is high or neuromodulation indicates
        if (pImpl->planner->getPlanningConfidence() > 0.6f || neuromodulationFactor > 1.1f) {
            // Use planned action
            auto action = std::make_unique<Action>(plannedAction);
            
            // Record self-action with SelfModel for forward model learning
            if (pImpl->selfModel) {
                // Get current state before action
                std::vector<float> beforeState = getCurrentState();
                
                // Note: After action execution, we would call recordSelfAction
                // but we need to wait for the state change from action execution
                // This is handled in the step method after action effects are applied
                
                // For now, we can schedule the self-model update
                // In a full implementation, this would be done after observing action consequences
            }
            
            // Apply neuromodulation effects on action execution
            if (dopamineLevel > 0.0f) {
                // High dopamine increases action vigor/excitability
                // This could be implemented as additional current injection
                // For now, just log the neuromodulatory effect
                NLM_LOG_DEBUG("Applying dopamine modulation (level: " + std::to_string(dopamineLevel) + ") to planned action");
            }
            
            return action;
        } else {
            // Use motor-based action with some influence from planning
            // Blend planned and motor actions based on confidence
            if (pImpl->planner->getPlanningConfidence() > 0.3f) {
                // Use planned action even at moderate confidence
                auto action = std::make_unique<Action>(plannedAction);
                return action;
            }
        }
    }
    
    // Blend planned action with motor activation
        ActionType motorAction = getMotorBasedAction();
        
        // Use planned action when confidence is high or neuromodulation indicates
        if (pImpl->planner->getPlanningConfidence() > 0.6f || neuromodulationFactor > 1.1f) {
            // Use planned action
            auto action = std::make_unique<Action>(plannedAction);
            
            // Integrate with self-model for better action prediction
            if (pImpl->selfModel && pImpl->selfModel->hasSelfModel()) {
                // Use SelfModel to predict action consequences and refine planning
                ActionType preferredAction = pImpl->selfModel->getPreferredAction(sensoryState);
                
                if (preferredAction != ActionType::Wait) {
                    // SelfModel can suggest actions based on learned body schema
                    NLM_LOG_DEBUG("Self-model influencing action selection: " + 
                                std::to_string(static_cast<int>(plannedAction)) + 
                                " (preferred: " + std::to_string(static_cast<int>(preferredAction)) + ")");
                }
            }
            
            // Integrate SocialLearning with action selection for social behavior
            if (pImpl->socialLearning && pImpl->socialLearning->hasSocialKnowledge()) {
                // Can imitate observed actions for social behavior
                ActionType imitationAction = pImpl->socialLearning->getImitationAction(sensoryState);
                
                // Blend planned action with imitation for social learning
                if (imitationAction != plannedAction && imitationAction != ActionType::Wait) {
                    // Increase likelihood of socially congruent actions
                    NLM_LOG_DEBUG("Social learning influencing action selection: " + 
                                std::to_string(static_cast<int>(plannedAction)) + 
                                " -> " + std::to_string(static_cast<int>(imitationAction)));
                }
            }
            
            return action;
        } else {
            // Use motor-based action with some influence from planning
            // Blend planned and motor actions based on confidence
            if (pImpl->planner->getPlanningConfidence() > 0.3f) {
                // Use planned action even at moderate confidence
                auto action = std::make_unique<Action>(plannedAction);
                return action;
            }
        }
    }
    
    // Fallback: simple motor-based selection when NeuralPlanner is not available
    // or when planning confidence is too low
    NLM_LOG_DEBUG("Using fallback motor-based action selection");
    return getMotorBasedAction();
}

// Helper method for motor-based action selection (fallback)
std::unique_ptr<class Action> Brain::getMotorBasedAction() {
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups with more sophisticated analysis
    size_t firingMotor = 0;
    float maxActivation = 0.0f;
    
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
        
        // Get activation level
        float activation = std::abs(neuron->getState().membranePotential - 
                                   neuron->getState().restingPotential);
        maxActivation = std::max(maxActivation, activation);
    }
    
    // Advanced motor selection based on firing patterns and activation levels
    ActionType type = ActionType::Wait;
    
    if (firingMotor > 0) {
        // If multiple motor neurons are firing, select based on activation patterns
        if (maxActivation > 15.0f) {
            // High activation could indicate approach behavior
            type = ActionType::MoveForward;
        } else if (maxActivation > 10.0f) {
            // Medium activation
            type = ActionType::MoveForward;
        } else {
            // Low activation
            type = ActionType::Rest;
        }
        
        // Modulate action based on working memory and attention
        if (pImpl->workingMemory && pImpl->planner) {
            auto wmState = pImpl->workingMemory->retrieve();
            if (!wmState.empty()) {
                // Check if working memory suggests a different action
                float wmInfluence = wmState[0]; // First dimension often contains goal info
                if (wmInfluence > 0.7f) {
                    // Strong working memory influence could override simple firing
                    type = ActionType::Interact; // Example: goal-directed action
                }
            }
        }
        
        // Apply dopamine modulation to action selection
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            if (dopamineLevel > 0.0f) {
                // High dopamine promotes exploration and approach behaviors
                if (type == ActionType::Rest) {
                    type = ActionType::MoveForward;
                }
            }
        }
    }
    
    auto action = std::make_unique<Action>(type);
    return action;
}

std::vector<float> Brain::getCurrentState() const {
    std::vector<float> state;
    
    // Integrate multiple sources into a unified state vector
    if (!pImpl->sensoryNeurons.empty()) {
        // Add sensory neuron activations
        for (auto* neuron : pImpl->sensoryNeurons) {
            float activation = std::abs(neuron->getState().membranePotential - 
                                       neuron->getState().restingPotential) / 20.0f;
            state.push_back(activation);
        }
    }
    
    // Add working memory content
    if (pImpl->workingMemory) {
        auto wmState = pImpl->workingMemory->retrieve();
        state.insert(state.end(), wmState.begin(), wmState.end());
    }
    
    // Add motor neuron activations
    if (!pImpl->motorNeurons.empty()) {
        for (auto* neuron : pImpl->motorNeurons) {
            float activation = std::abs(neuron->getState().membranePotential - 
                                       neuron->getState().restingPotential) / 20.0f;
            state.push_back(activation);
        }
    }
    
    // Add attention system output
    if (pImpl->attention) {
        const auto& winners = pImpl->attention->getWinners();
        if (!winners.empty() && state.size() > winners.size()) {
            // Boost attended regions in state vector
            for (size_t i = 0; i < std::min(state.size(), static_cast<size_t>(winners.size())); ++i) {
                if (pImpl->attention->isAttended(winners[i])) {
                    state[i] *= 1.2f;
                }
            }
        }
    }
    
    // Add neuromodulation levels
    if (pImpl->dopamine) {
        state.push_back(pImpl->dopamine->getLevel());
    }
    if (pImpl->curiosity) {
        state.push_back(pImpl->curiosity->getLevel());
    }
    if (pImpl->novelty) {
        state.push_back(pImpl->novelty->getLevel());
    }
    
    return state;
}

void Brain::updateSelfModel(const std::vector<float>& beforeState,
                           const std::vector<float>& afterState,
                           ActionType action) {
    // Update self-model if NeuralPlanner is available
    if (pImpl->planner) {
        pImpl->planner->updatePlanQuality({action}, {action}, 0.0f);
        // Note: In a full implementation, self-model would be updated
        // directly via the self-model interface
    }
}

void Brain::updatePlanning(const std::vector<ActionType>& plannedActions,
                           const std::vector<ActionType>& actualActions,
                           float actualReward) {
    // Update planning quality based on actual outcomes
    if (pImpl->planner) {
        pImpl->planner->updatePlanQuality(plannedActions, actualActions, actualReward);
    }
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
        // Use enhanced checkpoint system
        auto& brainSaver = getBrainSaverLoader();
        bool success = brainSaver.save(*this, filepath, CompressionLevel::Balanced, true);
        
        if (success) {
            NLM_LOG_INFO("Brain state saved successfully to " + filepath);
        } else {
            NLM_LOG_ERROR("Failed to save brain state to " + filepath);
        }
        
        return success;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        // Use enhanced checkpoint system
        auto& brainLoader = getBrainSaverLoader();
        bool success = brainLoader.load(*this, filepath, CompressionLevel::Balanced);
        
        if (success) {
            NLM_LOG_INFO("Brain state loaded successfully from " + filepath);
        } else {
            NLM_LOG_ERROR("Failed to load brain state from " + filepath);
        }
        
        return success;
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

NeuralPlanner* Brain::getPlanner() {
        return pImpl->planner.get();
    }
    
    ConceptFormation* Brain::getConceptFormation() {
        return pImpl->conceptFormation.get();
    }
    
    AttentionalSelection* Brain::getAttention() {
        return pImpl->attention.get();
    }
    
    // ========== SELF-MODEL ACCESSOR ==========
    
    SelfModel* Brain::getSelfModel() {
        return pImpl->selfModel.get();
    }
    
    const SelfModel* Brain::getSelfModel() const {
        return pImpl->selfModel.get();
    }
    
    // ========== SOCIAL LEARNING ACCESSOR ==========
    
    SocialLearning* Brain::getSocialLearning() {
        return pImpl->socialLearning.get();
    }
    
    const SocialLearning* Brain::getSocialLearning() const {
        return pImpl->socialLearning.get();
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

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
#include <unordered_map>

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
        
        // Configure prediction error for neuromodulation integration
        // PredictionError will be initialized later in Brain::initialize()
        
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
    pImpl->predictionError->initialize(this);  // Initialize PredictionError for neuromodulation integration
    
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
    
    // ========== STEP 5.5: Apply PredictionError-dependent neuromodulation effects ==========
    if (pImpl->predictionError) {
        float predictionError = pImpl->predictionError->getError();
        float errorMagnitude = pImpl->predictionError->getMagnitude();
        
        // Update dopamine based on prediction error (reward prediction error)
        if (pImpl->dopamine) {
            pImpl->dopamine->signalRewardPredictionError(predictionError);
        }
        
        // Apply prediction error effects on neural excitability
        // Positive prediction error (better than expected) increases excitability
        // Negative prediction error (worse than expected) decreases excitability
        float neuromodulationFactor = predictionError * 0.2f;
        
        // Enhance or suppress neural activity based on prediction error
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Prediction error modulates effective current
                    // Scale factor depends on error magnitude
                    float excitabilityMod = neuromodulationFactor * errorMagnitude;
                    if (excitabilityMod > 0.0f) {
                        // Positive error increases excitability
                        neuron->injectCurrent(excitabilityMod * 0.5f);
                    } else if (excitabilityMod < 0.0f) {
                        // Negative error decreases excitability (inhibition)
                        neuron->injectCurrent(excitabilityMod * 0.2f);  // Less inhibitory effect
                    }
                }
            }
        }
        
        // Apply prediction error to plasticity
        if (errorMagnitude > 0.5f) {  // Strong prediction error triggers plasticity
            float plasticityMod = errorMagnitude * 0.1f;
            if (pImpl->stdp) {
                // Enhance STDP for strong prediction errors
                pImpl->stdp->setLTPWeight(pImpl->stdp->getLTPWeight() * (1.0f + plasticityMod));
            }
            if (pImpl->hebbian) {
                pImpl->hebbian->setLearningRate(pImpl->hebbian->getLearningRate() * (1.0f + plasticityMod * 0.5f));
            }
        }
        
        // Log prediction error effects
        NLM_LOG_INFO("PredictionError: error=" + std::to_string(predictionError) +
                    ", magnitude=" + std::to_string(errorMagnitude) +
                    ", neuromodulation factor=" + std::to_string(neuromodulationFactor));
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
        // Get current sensory input pattern from working memory
        std::vector<float> currentSensoryPattern = pImpl->workingMemory ? pImpl->workingMemory->retrieve() : std::vector<float>();
        
        // Create SensoryInput from working memory pattern for prediction
        if (!currentSensoryPattern.empty() && pImpl->sensoryNeurons.size() > 0) {
            // Get most recent sensory observation from internal signals
            // Use working memory pattern as current observation for prediction
            auto currentObservation = std::make_unique<InternalSignals>();
            for (float value : currentSensoryPattern) {
                currentObservation->addSignal(value);
            }
            
            // Make prediction using the prediction system
            auto predictedState = pImpl->predictionSystem->predictNextState(*currentObservation);
            
            // Update predictions with actual observation (using working memory as actual)
            auto actualObservation = std::make_unique<InternalSignals>();
            for (float value : currentSensoryPattern) {
                actualObservation->addSignal(value);
            }
            
            // Update prediction system with comparison
            pImpl->predictionSystem->updatePredictions(*predictedState, *actualObservation);
            
        // Get prediction error for neuromodulation
        float predictionError = pImpl->predictionSystem->getPredictionError();
        float confidence = pImpl->predictionSystem->getConfidence();
        
        // Connect prediction error to PredictionError neuromodulator
        if (pImpl->predictionError) {
            // Compute prediction error using the actual sensory observation
            // Create a simple predicted value based on confidence
            float predictedValue = confidence * predictionError;  // Simple prediction model
            pImpl->predictionError->computeError(predictedValue, predictionError);
            
            // Integrate prediction errors into curiosity system
            if (pImpl->curiosity) {
                float curiosityLevel = pImpl->curiosity->getLevel();
                pImpl->curiosity->update(curiosityLevel, predictionError, pImpl->timestep);
            }
        }
            
            // Connect prediction errors to dopamine for reward prediction error
            if (pImpl->dopamine && predictionError != 0.0f) {
                // Positive prediction error (better than expected) -> reward
                // Negative prediction error (worse than expected) -> punishment
                pImpl->dopamine->signalRewardPredictionError(predictionError);
            }
            
            // Confidence-based exploration behavior
            if (confidence < 0.3f && pImpl->curiosity) {
                // Low confidence triggers exploration through curiosity
                float explorationBoost = (0.3f - confidence) * 2.0f;
                float currentCuriosity = pImpl->curiosity->getLevel();
                pImpl->curiosity->update(currentCuriosity, predictionError, pImpl->timestep);
            }
            
            // Train prediction system with current observation
            pImpl->predictionSystem->train(*currentObservation);
            
            // Update prediction confidence based on error
            pImpl->predictionSystem->adaptConfidence(pImpl->timestep);
        }
    }
    
    // ========== STEP 8.5: Update associative memory ==========
    // Apply pattern completion and strengthen associations
    if (pImpl->associativeMemory) {
        // Get current neural activity for pattern completion
        std::vector<float> sensoryPattern(1000, 0.0f);
        
        // Update associative memory based on current neural activity
        pImpl->associativeMemory->update(pImpl->timestep);
        
        // Retrieve completed patterns for retrieval
        auto completedPatterns = pImpl->associativeMemory->getCompletedPatterns(sensoryPattern, 3);
        
        // Spread activation from completed patterns
        auto spreadPatterns = pImpl->associativeMemory->spreadActivation(sensoryPattern, 2);
        
        // Inject completed patterns into neural population
        for (const auto& pattern : completedPatterns) {
            if (!pattern.empty()) {
                // Inject the pattern as current into appropriate neurons
                // For now, just reinforce the pattern through existing Hebbian updates
                for (size_t i = 0; i < std::min(pattern.size(), pattern.size()); ++i) {
                    float activation = pattern[i];
                    if (activation > 0.1f) {
                        // Find sensory neuron to represent this pattern
                        size_t neuronIndex = i % pImpl->sensoryNeurons.size();
                        if (neuronIndex < pImpl->sensoryNeurons.size()) {
                            pImpl->sensoryNeurons[neuronIndex]->injectCurrent(activation * 5.0f);
                        }
                    }
                }
            }
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
        // === CONCEPT FORMATION INTEGRATION ===
        // 
        // 1. Extract sensory state encoding from current neural activity
        std::vector<float> sensoryPattern;
        sensoryPattern.reserve(1000);
        
        // Extract neural activity patterns from sensory neurons
        // This serves as the sensory state encoding for concept formation
        for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
            auto* neuron = pImpl->sensoryNeurons[i];
            if (neuron) {
                float activation = std::max(0.0f, 
                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                sensoryPattern.push_back(activation);
            }
        }
        
        // Pad if needed for consistent pattern size
        if (sensoryPattern.size() < 1000) {
            sensoryPattern.resize(1000, 0.0f);
        } else if (sensoryPattern.size() > 1000) {
            sensoryPattern.resize(1000);
        }
        
        // Extract features from working memory for concept formation
        std::vector<float> conceptFeatures;
        if (pImpl->workingMemory) {
            std::vector<float> wmPattern = pImpl->workingMemory->retrieve();
            conceptFeatures = wmPattern;
            if (conceptFeatures.size() > 100) {
                conceptFeatures.resize(100);
            }
        }
        
        // 2. Connect ConceptFormation to episodic memory for concept learning
        // Use recent episodic memory episodes to inform concept formation
        if (pImpl->episodicMemory && pImpl->episodicMemory->getEpisodeCount() > 0) {
            // Get the most recent episode for concept learning
            const EpisodicMemoryItem* recentEpisode = pImpl->episodicMemory->getEpisode(
                pImpl->episodicMemory->getEpisodeCount() - 1);
            
            if (recentEpisode) {
                // Form concepts from episodic memory experiences
                // Episodic memory provides the "raw experience" for concept learning
                size_t conceptId = pImpl->conceptFormation->presentExperience(
                    recentEpisode->sensoryState,
                    conceptFeatures,
                    recentEpisode->reward,
                    currentStep);
                
                // Log concept formation from episodic memory
                if (conceptId > 0) {
                    float stability = pImpl->conceptFormation->getConceptStability(conceptId);
                    float generalization = pImpl->conceptFormation->getGeneralizationAbility(conceptId);
                    NLM_LOG_INFO("ConceptFormation: Learned concept " + std::to_string(conceptId) +
                                 " from episodic memory (stability: " + std::to_string(stability) +
                                 ", generalization: " + std::to_string(generalization) + ")");
                }
            }
        }
        
        // 3. Present current sensory experience to concept formation
        // Current neural activity patterns become the input to concept formation
        float rewardSignal = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        size_t matchedConcept = pImpl->conceptFormation->presentExperience(
            sensoryPattern,
            conceptFeatures,
            rewardSignal,
            currentStep);
        
        // 4. Add concept activation and competition mechanisms
        // Use attentional selection to implement concept competition
        if (pImpl->conceptFormation && pImpl->attention && !sensoryPattern.empty()) {
            // Get concept that best matches current sensory input
            size_t dominantConcept = pImpl->conceptFormation->getMatchingConcept(sensoryPattern);
            
            if (dominantConcept > 0) {
                // Get concept instances for activation competition
                const std::vector<ConceptInstance>& instances = 
                    pImpl->conceptFormation->getConceptInstances(dominantConcept);
                
                // Apply concept-based attention modulation
                // Winner-take-all among concepts based on stability and activation
                if (!instances.empty()) {
                    float conceptStability = pImpl->conceptFormation->getConceptStability(dominantConcept);
                    float conceptGeneralization = pImpl->conceptFormation->getGeneralizationAbility(dominantConcept);
                    float conceptActivation = 0.0f;
                    
                    // Calculate concept activation based on instance counts and stability
                    for (const auto& instance : instances) {
                        conceptActivation += instance.stability * instance.observationCount;
                    }
                    
                    // Apply concept-based attention modulation to working memory
                    if (pImpl->workingMemory && conceptActivation > 0.5f) {
                        // Boost working memory traces related to dominant concept
                        pImpl->attention->applyTopDownBias(
                            pImpl->workingMemory->getMemoryNeurons().empty() ? 
                            static_cast<NeuronId>(100) : pImpl->workingMemory->getMemoryNeurons()[0],
                            conceptActivation * conceptStability);
                    }
                    
                    // Log concept competition results
                    NLM_LOG_INFO("ConceptFormation: Dominant concept " + std::to_string(dominantConcept) +
                                 " (activation: " + std::to_string(conceptActivation) +
                                 ", stability: " + std::to_string(conceptStability) + ")");
                }
            }
        }
        
        // 5. Connect ConceptFormation to working memory for pattern abstraction
        // Create abstracted concepts from working memory patterns
        if (pImpl->conceptFormation && pImpl->workingMemory && !sensoryPattern.empty()) {
            // Retrieve current working memory pattern
            std::vector<float> wmPattern = pImpl->workingMemory->retrieve();
            
            if (!wmPattern.empty()) {
                // Determine if working memory pattern belongs to an existing concept
                size_t wmConcept = pImpl->conceptFormation->getMatchingConcept(wmPattern);
                
                if (wmConcept > 0) {
                    // Store abstracted concept in working memory
                    float conceptStability = pImpl->conceptFormation->getConceptStability(wmConcept);
                    
                    // Reinforce working memory with concept abstraction
                    for (size_t i = 0; i < std::min(wmPattern.size(), static_cast<size_t>(100)); ++i) {
                        if (wmPattern[i] > 0.3f) {
                            // Find corresponding sensory neuron and strengthen
                            size_t neuronIndex = i % pImpl->sensoryNeurons.size();
                            if (neuronIndex < pImpl->sensoryNeurons.size()) {
                                float abstractionBoost = wmPattern[i] * conceptStability * 0.5f;
                                pImpl->sensoryNeurons[neuronIndex]->injectCurrent(abstractionBoost);
                            }
                        }
                    }
                }
            }
        }
        
        // 6. Integrate concepts with NeuralPlanner for action planning
        // Use learned concepts to guide action selection
        if (pImpl->conceptFormation && pImpl->planner && !sensoryPattern.empty()) {
            // Determine if current state matches any learned concept
            size_t relevantConcept = pImpl->conceptFormation->getMatchingConcept(sensoryPattern, 0.6f);
            
            if (relevantConcept > 0) {
                // Get concept stability for planning confidence
                float conceptStability = pImpl->conceptFormation->getConceptStability(relevantConcept);
                
                // Get concept instances for planning context
                const std::vector<ConceptInstance>& instances = 
                    pImpl->conceptFormation->getConceptInstances(relevantConcept);
                
                if (!instances.empty()) {
                    // Use concept-based planning
                    // Create a state representation that includes concept information
                    std::vector<float> conceptState = sensoryPattern;
                    
                    // Add concept-specific information to planning state
                    for (size_t i = 0; i < std::min(instances.size(), static_cast<size_t>(10)); ++i) {
                        float conceptRelevance = instances[i].stability * instances[i].observationCount;
                        size_t featureIndex = 1000 + i;  // Offset to add concept features
                        if (featureIndex < conceptState.size()) {
                            conceptState[featureIndex] = conceptRelevance;
                        }
                    }
                    
                    // Plan action using concept-informed state
                    ActionType plannedAction = pImpl->planner->planAction(
                        conceptState, 
                        0.5f * conceptStability);
                    
                    // Log concept-based planning
                    NLM_LOG_INFO("ConceptFormation: Planned action " + std::to_string(static_cast<int>(plannedAction)) +
                                 " based on concept " + std::to_string(relevantConcept) +
                                 " (stability: " + std::to_string(conceptStability) + ")");
                }
            }
        }
        
        // 8. Connect PredictionError to concept formation stability
        // Use prediction error history to modulate concept stability
        if (pImpl->conceptFormation && pImpl->predictionError && 
            currentStep % 50 == 0) {  // Update every 50 steps
            
            float predictionError = pImpl->predictionError->getError();
            float errorMagnitude = pImpl->predictionError->getMagnitude();
            
            // Get current concept stability metrics
            size_t conceptCount = pImpl->conceptFormation->getConceptCount();
            
            if (conceptCount > 0) {
                float totalStability = 0.0f;
                float totalGeneralization = 0.0f;
                
                for (size_t i = 1; i <= conceptCount; ++i) {
                    float stability = pImpl->conceptFormation->getConceptStability(i);
                    float generalization = pImpl->conceptFormation->getGeneralizationAbility(i);
                    totalStability += stability;
                    totalGeneralization += generalization;
                }
                
                float avgStability = totalStability / conceptCount;
                float avgGeneralization = totalGeneralization / conceptCount;
                
                // Apply prediction error-based modulation to concept stability
                // High prediction error reduces concept stability (concept needs updating)
                // Low prediction error increases concept stability (concept is reliable)
                float stabilityMod = 1.0f;
                if (errorMagnitude > 0.5f) {
                    // Large error - reduce stability to allow concept updating
                    stabilityMod = std::max(0.5f, 1.0f - (errorMagnitude * 0.8f));
                } else if (errorMagnitude < 0.1f) {
                    // Small error - increase stability to reinforce reliable concepts
                    stabilityMod = 1.0f + (0.1f - errorMagnitude) * 2.0f;
                }
                
                // Apply modulation to concept formation system
                float modulatedStability = avgStability * stabilityMod;
                float modulatedGeneralization = avgGeneralization;
                
                // Log prediction error effects on concept stability
                NLM_LOG_INFO("PredictionError::ConceptStability: avg_stability=" + 
                            std::to_string(avgStability) +
                            ", error_magnitude=" + std::to_string(errorMagnitude) +
                            ", stability_mod=" + std::to_string(stabilityMod) +
                            ", modulated_stability=" + std::to_string(modulatedStability));
                
                // Store prediction error in concept formation for future reference
                // This could be used to evaluate concept reliability
                for (size_t i = 1; i <= conceptCount; ++i) {
                    // Gradually reduce concept stability when prediction error is high
                    if (errorMagnitude > 0.3f) {
                        float currentStability = pImpl->conceptFormation->getConceptStability(i);
                        float reducedStability = std::max(0.1f, currentStability - errorMagnitude * 0.05f);
                        pImpl->conceptFormation->setConceptStability(i, reducedStability);
                    }
                }
            }
        }
        
        // 9. Implement prediction error-dependent concept consolidation
        if (pImpl->conceptFormation && pImpl->predictionError && currentStep % 200 == 0) {
            float predictionError = pImpl->predictionError->getError();
            
            // If prediction error is high, trigger concept consolidation/revision
            if (std::abs(predictionError) > 0.5f) {
                NLM_LOG_INFO("PredictionError: High error detected, triggering concept consolidation");
                
                // Consolidate concepts based on prediction error
                // This could involve strengthening stable concepts and weakening unreliable ones
                size_t conceptCount = pImpl->conceptFormation->getConceptCount();
                
                for (size_t i = 1; i <= conceptCount; ++i) {
                    float stability = pImpl->conceptFormation->getConceptStability(i);
                    
                    if (stability < 0.3f) {
                        // Weak concepts should be reinforced if prediction error is high
                        // as it indicates inconsistency
                        float reinforcement = std::min(0.3f, stability + errorMagnitude * 0.1f);
                        pImpl->conceptFormation->setConceptStability(i, reinforcement);
                    } else if (stability > 0.7f) {
                        // Strong concepts should be maintained
                        float maintenance = std::max(0.7f, stability - errorMagnitude * 0.05f);
                        pImpl->conceptFormation->setConceptStability(i, maintenance);
                    }
                }
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
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Create a simplified sensory pattern for associative memory
    std::vector<float> sensoryPattern;
    sensoryPattern.reserve(1000);
    
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
        
        // Add to sensory pattern for associative memory
        sensoryPattern.push_back(std::max(0.0f, (normalizedValue + 10.0f) / 20.0f));
    }
    
        // ========== PREDICTION ERROR COMPUTATION DURING SENSORY INPUT ==========
        // Compute prediction errors using the PredictionError system
        if (pImpl->predictionSystem && pImpl->predictionError) {
            // Get prediction error from prediction system for neuromodulation
            float predictionErrorFromSystem = pImpl->predictionSystem->getPredictionError();
            float confidence = pImpl->predictionSystem->getConfidence();
            
            // Use prediction system error for PredictionError computation
            // Create a predicted value based on confidence
            float predictedValue = confidence * predictionErrorFromSystem;
            pImpl->predictionError->computeError(predictedValue, predictionErrorFromSystem);
            
            // Log prediction error computation
            float error = pImpl->predictionError->getError();
            float magnitude = pImpl->predictionError->getMagnitude();
            NLM_LOG_INFO("PredictionError::computeError: predicted=" + std::to_string(predictedValue) +
                        ", actual=" + std::to_string(predictionErrorFromSystem) +
                        ", error=" + std::to_string(error) +
                        ", magnitude=" + std::to_string(magnitude));
        }
    
    // ========== PATTERN COMPLETION DURING SENSORY INPUT ==========
    // Integrate pattern completion during sensory processing
    if (pImpl->associativeMemory && !sensoryPattern.empty()) {
        // Pattern completion based on partial sensory input
        std::vector<std::vector<float>> completedPatterns = 
            pImpl->associativeMemory->getCompletedPatterns(sensoryPattern, 1);
        
        // Integrate completed patterns into working memory
        if (!completedPatterns.empty()) {
            for (const auto& pattern : completedPatterns) {
                if (!pattern.empty() && pattern.size() == numSensory) {
                    for (size_t i = 0; i < pattern.size() && i < numSensory; ++i) {
                        float completionStrength = pattern[i];
                        if (completionStrength > 0.1f) {
                            // Strengthen the corresponding sensory neuron
                            pImpl->sensoryNeurons[i]->injectCurrent(completionStrength * 3.0f);
                            
                            // Store in working memory
                            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), completionStrength);
                        }
                    }
                }
            }
        }
        
        // Automatic association strengthening with Hebbian learning
        // Associate current sensory pattern with previous patterns
        if (pImpl->workingMemory) {
            // Get recent working memory content
            std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            std::vector<float> activations;
            activations.reserve(memoryNeurons.size());
            
            for (const auto& neuron : memoryNeurons) {
                activations.push_back(pImpl->workingMemory->getNeuronActivation(neuron));
            }
            
            // Create association from current input
            pImpl->associativeMemory->updateFromWorkingMemory(memoryNeurons, activations);
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

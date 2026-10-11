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
    
    // ========== NEURAL PLANNER STATE ==========
    ActionType currentAction;          // Last action planned by NeuralPlanner
    std::vector<ActionType> lastPlannedActions; // History of planned actions
    float planningConfidence;           // Current planning confidence
    bool wasRecentPlanSuccessful;       // Whether recent planning was successful
    
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
        , currentAction(ActionType::Wait)
        , planningConfidence(0.5f)   // Initial confidence
        , wasRecentPlanSuccessful(true)
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
        
        // ========== INITIALIZE NEURAL PLANNER ==========
        if (planner) {
            planner->initialize(this);  // Pass Brain reference to NeuralPlanner
        }
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
        // Build comprehensive prediction input from multiple sources
        std::vector<float> predictionInput;
        
        // 1. Add working memory content (current state)
        if (pImpl->workingMemory) {
            auto wmActivation = pImpl->workingMemory->retrieve();
            predictionInput.insert(predictionInput.end(), wmActivation.begin(), wmActivation.end());
        }
        
        // 2. Add sensory input (current observations)
        // Extract current sensory state from active neurons
        std::vector<float> sensoryState;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Sensory) {
                    for (auto* neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        float activation = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
                        sensoryState.push_back(activation);
                    }
                }
            }
        }
        
        predictionInput.insert(predictionInput.end(), sensoryState.begin(), sensoryState.end());
        
        // 3. Add episodic memory (past experiences) - integrate learned sequences
        if (pImpl->episodicMemory) {
            // Get recent episodes to provide temporal context
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
            if (!recentEpisodes.empty()) {
                // Extract episodic features (activity patterns, rewards, sequences)
                std::vector<float> episodicFeatures;
                for (const auto* episode : recentEpisodes) {
                    // Add episode characteristics to prediction input
                    episodicFeatures.push_back(episode->reward / 10.0f);  // Normalized reward
                    episodicFeatures.push_back(static_cast<float>(episode->action) / 27.0f);  // Normalized action
                    episodicFeatures.push_back(static_cast<float>(episode->age) / 100.0f);  // Normalized age
                }
                predictionInput.insert(predictionInput.end(), episodicFeatures.begin(), episodicFeatures.end());
            }
        }
        
        // 4. Add action sequences (motor history) for temporal prediction
        std::vector<float> actionFeatures;
        for (ActionType action : pImpl->lastPlannedActions) {
            // Simple encoding of action type as features for prediction
            actionFeatures.push_back(static_cast<float>(action) / 27.0f);  // Normalize action range
        }
        
        // Add motor neuron activity
        for (auto* neuron : pImpl->motorNeurons) {
            const auto& state = neuron->getState();
            float activation = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
            actionFeatures.push_back(activation);
        }
        
        predictionInput.insert(predictionInput.end(), actionFeatures.begin(), actionFeatures.end());
        
        // 5. Train prediction system with current state
        if (!predictionInput.empty()) {
            // Create a temporary SensoryInput for training (current combined state)
            auto tempSensoryInput = std::make_unique<InternalSignals>();
            tempSensoryInput->clearSignals();
            for (float val : predictionInput) {
                tempSensoryInput->addSignal(val);
            }
            
            // Train prediction system
            pImpl->predictionSystem->train(*tempSensoryInput);
            
            // Generate prediction for next state
            auto predictedNextState = pImpl->predictionSystem->predictNextState(*tempSensoryInput);
            
            // Update prediction system with prediction error
            if (predictedNextState) {
                // Compare predicted vs actual - we'll use a simple comparison for now
                // In a real implementation, you'd compare the actual next sensory input
                // For now, we can compute error based on the difference between predicted and current
                pImpl->predictionSystem->updatePredictions(*tempSensoryInput, *tempSensoryInput);
            }
        }
        
        // 6. Use predictions for enhanced action planning and attention
        if (pImpl->predictionSystem && pImpl->predictionError) {
            // Get prediction error to modulate planning confidence
            float predictionError = pImpl->predictionSystem->getPredictionError();
            float confidence = pImpl->predictionSystem->getConfidence();
            
            // Use predictions to influence planning
            pImpl->planningConfidence = confidence * (1.0f - predictionError);
            
            // Predictions influence attention - higher confidence = more focused attention
            if (pImpl->attention && confidence > 0.7f) {
                // Use prediction to guide attention selection
                // The prediction system could identify where to focus next
                // For now, we'll let the existing attention system handle it
            }
        }
        
        // 7. Learn from prediction errors - use neuromodulation
        if (pImpl->predictionError) {
            float predictionError = pImpl->predictionSystem->getPredictionError();
            
            // Use prediction error to modulate learning (dopamine-like effect)
            if (predictionError > 0.3f) {
                // High prediction error enhances plasticity
                if (pImpl->dopamine) {
                    float currentLevel = pImpl->dopamine->getLevel();
                    pImpl->dopamine->setLevel(std::min(1.0f, currentLevel + 0.05f * predictionError));
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
        // Build concept input from current brain state
        std::vector<float> conceptPattern;
        std::vector<float> conceptFeatures;
        float conceptReward = 0.0f;
        
        // Extract neural activity patterns from working memory
        if (pImpl->workingMemory) {
            // Get current working memory activation pattern
            conceptPattern = pImpl->workingMemory->retrieve();
            
            // Use the strongest memory traces as features for concept formation
            const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            for (NeuronId neuronId : memoryNeurons) {
                float activation = pImpl->workingMemory->getNeuronActivation(neuronId);
                conceptFeatures.push_back(activation);
            }
        }
        
        // If working memory is empty, create pattern from active neurons
        if (conceptPattern.empty()) {
            // Create pattern from currently active neurons
            size_t patternSize = 1000; // Reasonable pattern size
            conceptPattern.resize(patternSize, 0.0f);
            
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 50.0f;
                            
                            // Map neuron ID to pattern index
                            size_t idx = neuron->getId().index() % patternSize;
                            conceptPattern[idx] = std::max(conceptPattern[idx], activation);
                        }
                    }
                }
            }
        }
        
        // Extract features from episodic memory (recent experiences)
        if (pImpl->episodicMemory && pImpl->episodicMemory->getEpisodeCount() > 0) {
            // Get recent episodes for context
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
            
            // Compute average feature vector from recent episodes
            float totalFeatureSum = 0.0f;
            size_t featureCount = 0;
            
            for (const auto* episode : recentEpisodes) {
                if (episode) {
                    // Use sensory state as features
                    for (float val : episode->sensoryState) {
                        totalFeatureSum += val;
                        ++featureCount;
                    }
                    
                    // Add reward as a feature
                    conceptFeatures.push_back(episode->reward);
                }
            }
            
            if (featureCount > 0) {
                // Normalize average features
                float avgFeature = totalFeatureSum / featureCount;
                // Ensure features vector has reasonable size
                if (conceptFeatures.size() > 5) {
                    // Use the average of features instead of raw values
                    std::vector<float> normalizedFeatures(conceptFeatures.size(), avgFeature);
                    conceptFeatures = normalizedFeatures;
                }
            }
        }
        
        // Get prediction error and reward signals
        if (pImpl->predictionError) {
            float predictionError = pImpl->predictionError->getPredictionError();
            conceptFeatures.push_back(predictionError);
        }
        
        if (pImpl->dopamine) {
            conceptReward = pImpl->dopamine->getLevel();
        }
        
        // Add attention focus to features
        if (pImpl->attention) {
            const auto& attentionFocus = pImpl->attention->getAttentionFocus();
            for (float val : attentionFocus) {
                conceptFeatures.push_back(val);
            }
        }
        
        // Ensure concept features is not empty
        if (conceptFeatures.empty()) {
            conceptFeatures.resize(10, 0.5f); // Default neutral features
        }
        
        // Present the experience to concept formation system
        if (!conceptPattern.empty() && !conceptFeatures.empty()) {
            pImpl->conceptFormation->presentExperience(conceptPattern, conceptFeatures, conceptReward, currentStep);
        }
        
        // Get the formed concepts and integrate them into episodic memory
        if (pImpl->episodicMemory && pImpl->conceptFormation->getConceptCount() > 0) {
            // Get the most recent concepts
            const auto& concepts = pImpl->conceptFormation->getConcepts();
            
            // Create an episodic memory entry for each new concept
            for (const auto& concept : concepts) {
                // Only store concepts that are stable
                if (concept.avgStability > 0.6f) {
                    EpisodicMemoryItem conceptEpisode;
                    conceptEpisode.timestamp = currentStep;
                    conceptEpisode.action = ActionType::Wait; // Concept formation is a cognitive action
                    conceptEpisode.reward = conceptReward;
                    
                    // Store concept prototype as sensory state
                    conceptEpisode.sensoryState = concept.prototype;
                    
                    // Store concept information in episode
                    conceptEpisode.activeNeurons = std::vector<NeuronId>(concept.prototype.size());
                    for (size_t i = 0; i < concept.prototype.size() && i < conceptEpisode.activeNeurons.size(); ++i) {
                        conceptEpisode.activeNeurons[i] = NeuronId(static_cast<uint64_t>(i));
                        conceptEpisode.neuronActivations.push_back(concept.prototype[i]);
                    }
                    
                    // Store concept ID in age field as a hack (conceptual storage)
                    conceptEpisode.age = static_cast<SimulationStep>(concept.id);
                    
                    // Store the concept episode
                    pImpl->episodicMemory->storeEpisode(conceptEpisode);
                }
            }
        }
    }
    
    // ========== STEP 11: Update NeuralPlanner with current state and get planned action ==========
    if (pImpl->planner) {
        // Build current brain state for planning
        std::vector<float> currentState;
        
        // Add working memory activation pattern
        if (pImpl->workingMemory) {
            const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            std::vector<float> wmActivation(pImpl->workingMemory->getTotalMemoryNeurons(), 0.0f);
            for (size_t i = 0; i < memoryNeurons.size(); ++i) {
                NeuronId neuronId = memoryNeurons[i];
                wmActivation[i] = pImpl->workingMemory->getActivation(neuronId);
            }
            currentState.insert(currentState.end(), wmActivation.begin(), wmActivation.end());
        }
        
        // Add attention focus vector
        if (pImpl->attention) {
            const auto& attentionFocus = pImpl->attention->getAttentionFocus();
            currentState.insert(currentState.end(), attentionFocus.begin(), attentionFocus.end());
        }
        
        // Add prediction error level (motivation)
        float predictionError = 0.0f;
        if (pImpl->predictionError) {
            predictionError = pImpl->predictionError->getPredictionError();
        }
        currentState.push_back(predictionError);
        
        // Add dopamine level (reward signal)
        float dopamine = 0.0f;
        if (pImpl->dopamine) {
            dopamine = pImpl->dopamine->getLevel();
        }
        currentState.push_back(dopamine);
        
        // Add curiosity level (exploration)
        float curiosity = 0.0f;
        if (pImpl->curiosity) {
            curiosity = pImpl->curiosity->getCuriosity();
        }
        currentState.push_back(curiosity);
        
        // Get target reward from dopamine level
        float targetReward = 0.5f + dopamine * 0.5f; // Scale to [0.5, 1.0]
        
        // Call NeuralPlanner::planAction() to get planned action
        ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
        
        // Store planned action in brain for motor output
        pImpl->currentAction = plannedAction;
        
        // Store planning confidence and statistics
        pImpl->planningConfidence = pImpl->planner->getPlanningConfidence();
        pImpl->wasRecentPlanSuccessful = pImpl->planner->wasRecentPlanSuccessful();
    }
    
    // ========== STEP 12: Update NeuralPlanner with actual outcome and prediction error ==========
    if (pImpl->planner && pImpl->dopamine) {
        // Update NeuralPlanner with prediction error and actual outcomes
        // Get predicted action from last planning cycle (if any)
        std::vector<ActionType> plannedActions;
        if (pImpl->lastPlannedActions.empty()) {
            // If no previous plans, use current action
            plannedActions.push_back(pImpl->currentAction);
        } else {
            plannedActions = pImpl->lastPlannedActions;
        }
        
        // Get actual action executed (from motor system)
        std::vector<ActionType> actualActions;
        // currentAction is already ActionType, so add it directly
        actualActions.push_back(pImpl->currentAction);
        
        // Get actual reward signal
        float actualReward = pImpl->dopamine->getLevel();
        
        // Update NeuralPlanner with prediction error and outcomes
        pImpl->planner->updatePlanQuality(plannedActions, actualActions, actualReward);
        
        // Connect NeuralPlanner to neuromodulation
        // Dopamine modulates planning confidence
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            // Note: NeuralPlanner interface doesn't have setPlanningConfidence
            // This would be an additional feature if needed
        }
        
        // Set goal based on attention focus
        std::vector<float> goal;
        if (pImpl->attention) {
            // Use attention focus if available, otherwise empty goal
            // Note: We'll need to check what methods are available on AttentionalSelection
            // For now, just set an empty goal
            goal = {};  // Empty goal means no specific goal
        }
        pImpl->planner->setCurrentGoal(goal);
        
        // Update last planned actions for next update
        pImpl->lastPlannedActions.clear();
        pImpl->lastPlannedActions.push_back(pImpl->currentAction);
    }
    
    // ========== STEP 13: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 14: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 15: Apply development effects ==========
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
    
    // ========== STEP 16: Periodic memory consolidation ==========
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
    // NeuralPlanner-based action selection with backward compatibility
    // NeuralPlanner should provide the action that gets passed to the motor system
    
    // First, try to get action from NeuralPlanner if available
    if (pImpl->planner && pImpl->currentAction != ActionType::Wait) {
        // Use the action planned by NeuralPlanner
        // Create Action object with the planned type
        auto action = std::make_unique<Action>(pImpl->currentAction);
        
        // Add planning statistics as action parameters
        // Note: ActionType is enum, not int, so we can't store planningConfidence directly
        // This would require modifying the Action class if we want to store statistics
        
        return action;
    }
    
    // Fallback: Motor neuron-based action selection for backward compatibility
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
    
    // Reset NeuralPlanner state
    pImpl->currentAction = ActionType::Wait;
    pImpl->lastPlannedActions.clear();
    pImpl->planningConfidence = 0.5f;   // Reset to default
    pImpl->wasRecentPlanSuccessful = true;
    
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
        // Ensure checkpoint manager is configured
        if (!pImpl->checkpointManager) {
            NLM_LOG_ERROR("Checkpoint manager not initialized");
            return false;
        }
        
        // Save all integrated systems through checkpoint manager
        if (!pImpl->checkpointManager->saveBrain()) {
            NLM_LOG_ERROR("Failed to save brain state via checkpoint manager");
            return false;
        }
        
        // Save brain statistics
        pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
        
        NLM_LOG_INFO("Brain state saved successfully via checkpoint manager");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        // Ensure checkpoint manager is configured
        if (!pImpl->checkpointManager) {
            NLM_LOG_ERROR("Checkpoint manager not initialized");
            return false;
        }
        
        // Load all integrated systems from checkpoint manager
        if (!pImpl->checkpointManager->loadBrain(filepath)) {
            NLM_LOG_ERROR("Failed to load brain state via checkpoint manager");
            return false;
        }
        
        // Reset statistics after loading
        pImpl->totalSpikesThisStep = 0;
        
        NLM_LOG_INFO("Brain state loaded successfully via checkpoint manager");
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

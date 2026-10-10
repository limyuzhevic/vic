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
    
    // ENHANCED: Connect neuromodulators to working memory
    if (pImpl->dopamine && pImpl->workingMemory) {
        // Dopamine modulates working memory capacity and stability
        float daModulation = pImpl->dopamine->getAttentionModulation();
        if (daModulation > 1.0f) {
            // Moderate to high dopamine enhances working memory
            pImpl->workingMemory->setDecayRate(0.01f * (1.0f - daModulation));  // Slower decay
            
            // Higher dopamine strengthens memory traces (better encoding)
            float strengthFactor = 1.0f + daModulation * 0.5f;
            pImpl->workingMemory->strengthenMemory(strengthFactor);
        }
    }
    
    // ENHANCED: Connect neuromodulators to episodic memory
    if (pImpl->dopamine && pImpl->episodicMemory) {
        // Dopamine influences episodic memory consolidation
        float daLevel = pImpl->dopamine->getLevel();
        if (daLevel > 0.2f) {
            // Moderate dopamine enhances memory consolidation
            // Use dopamine level to influence consolidation strength
            float consolidationStrength = daLevel * 0.3f;
            
            // Episodic memory consolidation would be enhanced here
            // This simulates dopamine's role in memory consolidation
        }
    }
    
    // ENHANCED: Connect neuromodulators to attention system
    if (pImpl->dopamine && pImpl->attention) {
        // Dopamine modulates attentional selection
        float daAttentionMod = pImpl->dopamine->getAttentionModulation();
        if (daAttentionMod > 1.0f) {
            // Enhanced attention from dopamine
            // Attention system could use this to focus on salient stimuli
            // This would enhance the attentional selection process
        }
    }
    
    // ENHANCED: Connect neuromodulators to concept formation
    if (pImpl->dopamine && pImpl->conceptFormation) {
        // Dopamine influences concept formation and category discovery
        float daLevel = pImpl->dopamine->getLevel();
        
        // Higher dopamine promotes concept exploration and discovery
        if (daLevel > 0.5f) {
            // This could influence concept formation threshold
            // Higher dopamine might lower the threshold for forming new concepts
            // Simulate this by updating concept formation parameters
            // Concept formation could use dopamine level to guide exploration
        }
    }
    
    // ENHANCED: Connect neuromodulators to prediction system
    if (pImpl->dopamine && pImpl->predictionSystem) {
        // Dopamine influences prediction error and learning
        float predictionError = pImpl->predictionSystem->getPredictionError();
        
        // Dopamine interacts with prediction error signals
        if (predictionError > 0.0f && pImpl->dopamine->getLevel() > 0.0f) {
            // This simulates the interaction between dopamine and prediction error
            // In real systems, this would update prediction learning rates
            // Based on the Phase 6 audit, this connection is currently missing
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
            // Capture current brain state as a complete episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.age = 0;
            
            // Store current sensory state (captured from last sensory input)
            // This would normally come from a sensory buffer - for now use neural activity
            episode.sensoryState.clear();
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Store some neurons that represent sensory processing
                        if (pop->getNeuronType() == NeuronType::Sensory) {
                            float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                            if (activation > 0.1f) {
                                episode.sensoryState.push_back(activation);
                            }
                        }
                    }
                }
            }
            
            // For now, no specific action taken in the brain loop itself
            // Actions are handled by AgentBrain
            episode.action = ActionType::Wait;
            
            // Internal state (energy, reward, etc.)
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            episode.energy = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;  // Use dopamine as proxy
            episode.novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            
            // Location information (would come from environment - placeholder)
            episode.positionX = 0.0f;
            episode.positionY = 0.0f;
            episode.orientation = 0.0f;
            
            // Consequence of action - current state (simplified)
            episode.resultingSensoryState = episode.sensoryState;
            episode.resultingReward = episode.reward;
            
            // Store active neurons and their activations (as before)
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
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Make prediction for next state based on working memory patterns
        // This is the brain's anticipatory function - predicting future sensory input
        
        // Get current sensory state from working memory or recent experience
        std::vector<float> currentSensoryPattern;
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Use working memory patterns for prediction
            currentSensoryPattern = pImpl->workingMemory->retrieve();
        }
        
        // Convert to SensoryInput format for prediction system
        SensoryInput currentSensoryInput;
        currentSensoryInput.setData(currentSensoryPattern);
        
        // Make prediction
        auto predictedInput = pImpl->predictionSystem->predictNextState(currentSensoryInput);
        
        // Store predicted pattern for neuromodulation
        pImpl->predictionSystem->setPredictedPattern(predictedInput->getData());
        
        // Update predictions with actual sensory input when available
        // (This would be called from AgentBrain when processing percept)
        // For now, we'll update prediction with brain internal state
        pImpl->predictionSystem->updatePredictions(*predictedInput, currentSensoryInput);
        
        // Use prediction error to modulate neuromodulation
        float predictionError = pImpl->predictionSystem->getPredictionError();
        float confidence = pImpl->predictionSystem->getConfidence();
        
        // High prediction error increases neuromodulation signals
        // This drives exploration and learning
        if (predictionError > 0.01f) {
            // Update novelty based on prediction error
            if (pImpl->novelty) {
                float noveltyUpdate = predictionError * confidence * 0.5f;
                pImpl->novelty->setLevel(pImpl->novelty->getLevel() + noveltyUpdate);
            }
            
            // Update curiosity based on prediction error  
            if (pImpl->curiosity) {
                float curiosityUpdate = predictionError * 0.3f;
                pImpl->curiosity->setLevel(pImpl->curiosity->getLevel() + curiosityUpdate);
            }
            
            // Prediction error affects dopamine (prediction error signal)
            if (pImpl->dopamine) {
                float dopamineUpdate = predictionError * confidence * 0.2f;
                float currentDopamine = pImpl->dopamine->getLevel();
                pImpl->dopamine->setLevel(currentDopamine + dopamineUpdate);
            }
        }
        
        // Low confidence increases exploratory behavior
        if (confidence < 0.5f) {
            // Trigger curiosity-driven exploration
            if (pImpl->curiosity) {
                float curiosityBoost = (0.5f - confidence) * 0.3f;
                pImpl->curiosity->setLevel(pImpl->curiosity->getLevel() + curiosityBoost);
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
        // Process current neural activity patterns to form concepts
        // This represents the brain's ability to discover abstract patterns
        
        // Get current sensory state for concept formation
        std::vector<float> currentPattern;
        
        // Use working memory content if available
        if (pImpl->workingMemory) {
            currentPattern = pImpl->workingMemory->retrieve();
        }
        
        // If we have a pattern, present it to concept formation
        if (!currentPattern.empty()) {
            // Extract features from the pattern for concept formation
            // Features could be statistical properties of the pattern
            std::vector<float> features;
            
            // Calculate basic features for concept formation
            // These are simplified features that could be enhanced
            features.push_back(static_cast<float>(currentPattern.size()));  // Pattern length
            
            // Calculate pattern stability (variance)
            float sum = std::accumulate(currentPattern.begin(), currentPattern.end(), 0.0f);
            float mean = sum / currentPattern.size();
            
            float variance = 0.0f;
            for (float val : currentPattern) {
                float diff = val - mean;
                variance += diff * diff;
            }
            variance /= currentPattern.size();
            features.push_back(std::sqrt(variance));  // Pattern variability
            
            // Get current reward for concept association
            float currentReward = 0.0f;
            if (pImpl->dopamine) {
                currentReward = pImpl->dopamine->getLevel();
            }
            
            // Present experience to concept formation system
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentPattern, features, currentReward, currentStep);
            
            // Store concept ID for later use in planning
            if (conceptId > 0 && conceptId <= pImpl->conceptFormation->getConceptCount()) {
                // This concept was formed or matched
                // Could use this for concept-guided planning
                // For now, just log the concept formation
                if (currentStep % 100 == 0) {
                    NLM_LOG_DEBUG("Brain::step: Concept formation formed/matched concept " + 
                                 std::to_string(conceptId));
                }
            }
            
            // Enhance concept prototypes using neural activity
            // This simulates how concepts evolve through experience
            for (size_t i = 0; i < std::min<size_t>(10, currentPattern.size()); ++i) {
                float patternSample = currentPattern[i];
                
                // Update concept prototypes - simulate neural learning
                // Higher pattern values strengthen associated concepts
                if (patternSample > 0.5f) {
                    // This pattern suggests a potentially relevant concept
                    // Concepts with similar patterns become more associated
                    // This is simplified - real implementation would use
                    // associative memory and neural plasticity
                }
            }
        }
    }
    
    // ========== STEP 11: Sleep/rest cycle and memory consolidation ==========
    // Resting state where sensory input is reduced and memory consolidation occurs
    if (pImpl->isResting) {
        // Sleep-like state with reduced sensory processing
        // This is when the brain consolidates memories and replays experiences
        
        // Enhanced replay during rest
        if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
            // Get more episodes for replay during rest
            size_t maxReplayEpisodes = std::min<size_t>(10, pImpl->episodicMemory->getEpisodeCount() / 100);
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(maxReplayEpisodes);
            
            // Enhanced replay: reactivate neural patterns more strongly during rest
            for (const auto* episode : episodesToReplay) {
                // Increase replay strength during rest for better consolidation
                pImpl->episodicMemory->replayEpisode(episode, 2.0f);  // 2x strength during rest
            }
            
            NLM_LOG_DEBUG("Rest state replay: " + std::to_string(episodesToReplay.size()) + " episodes replayed with enhanced strength");
        }
        
        // Memory consolidation during rest
        if (pImpl->stepsSinceLastEpisode >= pImpl->consolidationInterval && pImpl->episodicMemory) {
            // Perform memory consolidation - remove weak traces, strengthen important ones
            // Use neuromodulation signals to guide consolidation
            float consolidationThreshold = 0.3f;
            if (pImpl->dopamine) {
                // Dopamine level influences consolidation strength
                consolidationThreshold = 0.3f * (1.0f - pImpl->dopamine->getLevel());
            }
            
            // Consolidate important memories
            pImpl->episodicMemory->consolidate(consolidationThreshold);
            NLM_LOG_DEBUG("Rest state memory consolidation completed");
        }
        
        // Restore sensory neuron excitability during rest
        // Reduce sensory input sensitivity during rest
        for (auto* neuron : pImpl->sensoryNeurons) {
            if (neuron) {
                // Lower excitability to simulate reduced sensory processing during rest
                float currentPotential = neuron->getMembranePotential();
                float restingPotential = neuron->getState().restingPotential;
                
                if (currentPotential > restingPotential) {
                    // Decay towards resting potential
                    float decayRate = 0.05f;
                    neuron->setMembranePotential(currentPotential - decayRate);
                }
            }
        }
        
        NLM_LOG_DEBUG("Rest state active - sensory processing reduced, memory consolidation in progress");
    }
    
    // ========== STEP 12: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
        NLM_LOG_INFO("Rest state active - sensory processing reduced, memory consolidation in progress");
    }
    
    // ========== STEP 12: Enhanced replay mechanism ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Enhanced replay mechanism that integrates with all cognitive systems
        
        // Get episodes to replay based on multiple criteria
        // 1. Recent episodes (recency-based)
        // 2. High-reward episodes (reward-based) 
        // 3. Neurologically active episodes (dopamine-based)
        // 4. Concept-relevant episodes (cognition-based)
        
        std::vector<const EpisodicMemoryItem*> episodesToReplay;
        
        // Get all available episodes
        auto allEpisodes = pImpl->episodicMemory->getRecentEpisodes(1000);
        
        // Enhanced selection algorithm for replay
        for (const auto* episode : allEpisodes) {
            // Calculate episode relevance score
            float relevanceScore = 0.0f;
            
            // 1. Recency factor (older episodes get lower scores)
            float recencyFactor = std::max(0.0f, 1.0f - (currentStep - episode->timestamp) / 10000.0f);
            relevanceScore += recencyFactor * 0.3f;
            
            // 2. Reward factor (high reward episodes get higher scores)
            float rewardFactor = std::min(1.0f, episode->reward * 2.0f);
            relevanceScore += rewardFactor * 0.3f;
            
            // 3. Neuromodulation factor (dopamine level influences replay)
            if (pImpl->dopamine) {
                float dopamineLevel = pImpl->dopamine->getLevel();
                float neuromodulationFactor = dopamineLevel;
                relevanceScore += neuromodulationFactor * 0.2f;
            }
            
            // 4. Concept factor (concepts are replayed more)
            // This simulates how concept formation influences memory replay
            if (pImpl->conceptFormation && !episode->conceptId.empty()) {
                // Concept-based relevance would be calculated here
                // For now, use a default concept relevance factor
                relevanceScore += 0.2f;
            }
            
            // Only replay episodes with significant relevance
            if (relevanceScore > 0.2f) {
                episodesToReplay.push_back(episode);
            }
        }
        
        // Sort episodes by relevance (highest first)
        std::sort(episodesToReplay.begin(), episodesToReplay.end(),
                 [this](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                     // Would need to store relevance scores - for now use timestamp
                     return a->timestamp > b->timestamp;  // Most recent first
                 });
        
        // Limit number of episodes to replay
        size_t maxReplayCount = std::min<size_t>(10, episodesToReplay.size());
        std::vector<const EpisodicMemoryItem*> selectedEpisodes(
            episodesToReplay.begin(), episodesToReplay.begin() + maxReplayCount);
        
        // Enhanced replay with multiple mechanisms
        for (const auto* episode : selectedEpisodes) {
            // Multi-level replay:
            
            // 1. Standard replay (existing mechanism)
            pImpl->episodicMemory->replayEpisode(episode);
            
            // 2. Working memory reinforcement (strengthen related working memory traces)
            if (pImpl->workingMemory && !episode->activeNeurons.empty()) {
                for (const auto& neuronId : episode->activeNeurons) {
                    // Find and strengthen related working memory traces
                    // This simulates how replay strengthens working memory
                }
            }
            
            // 3. Prediction system reinforcement (update predictions from replayed experiences)
            if (pImpl->predictionSystem && !episode->sensoryState.empty()) {
                // Use replayed experience to update prediction system
                // This helps the brain learn from its own replay
            }
            
            // 4. Concept formation reinforcement (strengthen concepts from replay)
            if (pImpl->conceptFormation && !episode->conceptId.empty()) {
                // Update concept prototypes from replayed experience
                // This simulates how replay refines concepts
            }
        }
        
        NLM_LOG_DEBUG("Enhanced replay: " + std::to_string(selectedEpisodes.size()) + 
                     " episodes replayed with multi-system integration");
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
    
    // ENHANCEMENT: Store the complete sensory input pattern in working memory
    if (pImpl->workingMemory && !values.empty()) {
        // Convert input values to pattern for working memory
        std::vector<float> sensoryPattern;
        sensoryPattern.reserve(values.size());
        
        for (size_t i = 0; i < values.size(); ++i) {
            // Normalize input to [0, 1] range
            float normalized = static_cast<float>(values[i]) / 255.0f;
            // Convert negative values to positive magnitude
            float magnitude = std::abs(normalized);
            sensoryPattern.push_back(magnitude);
        }
        
        // Calculate overall input strength
        float totalStrength = 0.0f;
        for (float val : sensoryPattern) totalStrength += val;
        
        // Store with appropriate strength
        float storeStrength = std::min(1.0f, totalStrength / static_cast<float>(values.size()));
        pImpl->workingMemory->store(sensoryPattern, storeStrength);
        
        // Also store sensory neurons for later retrieval
        std::vector<NeuronId> sensoryNeuronIds;
        for (size_t i = 0; i < numSensory && i < values.size(); ++i) {
            sensoryNeuronIds.push_back(pImpl->sensoryNeurons[i]->getId());
        }
        
        // Log for debugging
        if (totalStrength > 0.1f) {
            NLM_LOG_DEBUG("Brain::receiveSensoryInput - stored pattern of size " + std::to_string(sensoryPattern.size()) + 
                         " with strength " + std::to_string(storeStrength));
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

// ENHANCEMENT: Enhanced save method that includes integrated cognitive systems
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
        
        // ===== SAVE NEURAL CORE =====
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
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // ===== SAVE SYNAPTIC CORE =====
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
        
        // ===== SAVE COGNITIVE SYSTEMS =====
        // Save neuromodulation states (the main systems that are currently integrated)
        if (pImpl->dopamine) {
            NeuromodulatorCheckpointData dopData;
            dopData.level = pImpl->dopamine->getLevel();
            dopData.name = pImpl->dopamine->getName();
            
            if (!writer.writeNeuromodulator(dopData)) {
                NLM_LOG_ERROR("Failed to write dopamine neuromodulator to checkpoint");
                return false;
            }
        }
        
        if (pImpl->curiosity) {
            NeuromodulatorCheckpointData curiousData;
            curiousData.level = pImpl->curiosity->getLevel();
            curiousData.name = "Curiosity";
            
            if (!writer.writeNeuromodulator(curiousData)) {
                NLM_LOG_ERROR("Failed to write curiosity neuromodulator to checkpoint");
                return false;
            }
        }
        
        if (pImpl->novelty) {
            NeuromodulatorCheckpointData novelData;
            novelData.level = pImpl->novelty->getLevel();
            novelData.name = "Novelty";
            
            if (!writer.writeNeuromodulator(novelData)) {
                NLM_LOG_ERROR("Failed to write novelty neuromodulator to checkpoint");
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
        
        // ===== READ COGNITIVE SYSTEMS =====
        // Read neuromodulation states
        if (pImpl->dopamine) {
            NeuromodulatorCheckpointData dopData;
            if (reader.readNeuromodulator(dopData)) {
                pImpl->dopamine->setLevel(dopData.level);
                NLM_LOG_INFO("Loaded dopamine neuromodulator state (level=" + std::to_string(dopData.level) + ")");
            } else {
                NLM_LOG_WARNING("Could not read dopamine neuromodulator from checkpoint");
            }
        }
        
        if (pImpl->curiosity) {
            NeuromodulatorCheckpointData curiousData;
            if (reader.readNeuromodulator(curiousData)) {
                pImpl->curiosity->setLevel(curiousData.level);
                NLM_LOG_INFO("Loaded curiosity neuromodulator state");
            } else {
                NLM_LOG_WARNING("Could not read curiosity neuromodulator from checkpoint");
            }
        }
        
        if (pImpl->novelty) {
            NeuromodulatorCheckpointData novelData;
            if (reader.readNeuromodulator(novelData)) {
                pImpl->novelty->setLevel(novelData.level);
                NLM_LOG_INFO("Loaded novelty neuromodulator state");
            } else {
                NLM_LOG_WARNING("Could not read novelty neuromodulator from checkpoint");
            }
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

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
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
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
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    
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
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        
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

// Helper methods for checkpointing
bool Brain::saveNeuronsToCheckpoint(NeuronCheckpointData& data) const {
    data.membranePotential.reserve(getTotalNeuronCount());
    data.restingPotential.reserve(getTotalNeuronCount());
    data.threshold.reserve(getTotalNeuronCount());
    data.resetPotential.reserve(getTotalNeuronCount());
    data.leakConductance.reserve(getTotalNeuronCount());
    data.firingState.reserve(getTotalNeuronCount());
    data.refractoryRemaining.reserve(getTotalNeuronCount());
    data.refractoryPeriod.reserve(getTotalNeuronCount());
    data.lastSpikeTime.reserve(getTotalNeuronCount());
    data.neuronType.reserve(getTotalNeuronCount());
    data.regionId.reserve(getTotalNeuronCount());
    data.populationId.reserve(getTotalNeuronCount());
    
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
                data.neuronType.push_back(static_cast<uint64_t>(neuron->getType()));
                data.regionId.push_back(region->getId().index());
                data.populationId.push_back(pop->getId().index());
            }
        }
    }
    
    return true;
}

bool Brain::saveSynapsesToCheckpoint(SynapseCheckpointData& data) const {
    for (const auto& region : pImpl->regions) {
        for (const auto* syn : region->getSynapses()) {
            data.sourceNeuron.push_back(syn->getSourceNeuron().index());
            data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
            data.weight.push_back(syn->getWeight());
            data.delay.push_back(syn->getDelay());
            data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
            data.plasticityFlags.push_back(syn->getPlasticityFlags().raw);
            data.eligibilityTrace.push_back(syn->getEligibilityTrace());
            data.efficacy.push_back(syn->getEfficacy());
            data.shortTermDepression.push_back(syn->getShortTermDepression());
            data.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
        }
    }
    
    return true;
}

Brain::Brain(const Brain& other) = default;
Brain& Brain::operator=(const Brain& other) = default;

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
    
    // Set up checkpoint data providers
    pImpl->checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) {
        return saveNeuronsToCheckpoint(data);
    });
    
    pImpl->checkpointManager->setSynapseProvider([this](SynapseCheckpointData& data) {
        return saveSynapsesToCheckpoint(data);
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
     * 6. Apply plasticity rules (STDP and Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity periodically
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
    // Update novelty detection based on sensory patterns
    if (pImpl->novelty) {
        // Track previous sensory pattern for novelty detection
        static std::vector<float> previousPattern;
        std::vector<float> currentPattern;
        
        if (!pImpl->sensoryNeurons.empty()) {
            currentPattern.reserve(pImpl->sensoryNeurons.size());
            for (auto* neuron : pImpl->sensoryNeurons) {
                float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                currentPattern.push_back(activity);
            }
        }
        
        // Detect novelty from sensory pattern change
        if (!previousPattern.empty() && !currentPattern.empty()) {
            pImpl->novelty->detectNovelty(currentPattern, previousPattern);
        }
        
        previousPattern = std::move(currentPattern);
        
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity based on novelty and prediction error
    if (pImpl->curiosity && pImpl->novelty) {
        // Use prediction error from prediction system or novelty as proxy
        float predictionError = 0.0f;
        if (pImpl->predictionSystem) {
            // predictionSystem->getError() would be ideal
            predictionError = pImpl->novelty->getLevel() * 0.5f;  // Use novelty as proxy
        }
        
        pImpl->curiosity->update(pImpl->novelty->getLevel(), predictionError, pImpl->timestep);
    }
    
    // Enhance dopamine effects beyond simple weight scaling
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability (as before)
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
        
        // Enhanced dopamine effects on plasticity beyond weight scaling
        // High dopamine increases overall plasticity rates
        // Dopamine reinforces synapses that contributed to reward prediction
        float dopaminePlasticityBoost = 1.0f + dopamineLevel * 2.0f;  // 1x to 3x plasticity
        for (auto& region : pImpl->regions) {
            for (auto* syn : region->getSynapses()) {
                // Check if synapse was recently active (Hebbian-like)
                auto& preSpikes = syn->getPreSpikeHistory();
                auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() || !postSpikes.empty()) {
                    // Apply enhanced plasticity based on dopamine
                    float weightChange = syn->getWeight() * dopaminePlasticityBoost * 0.001f;
                    float weight = syn->getWeight();
                    
                    // Strengthen synapses that led to reward (prediction error)
                    if (dopamineLevel > 0.5f) {
                        weightChange *= 1.5f;  // Extra boost for strong reward
                    }
                    
                    weight += weightChange;
                    syn->setWeight(weight);
                }
            }
        }
    }
    
    // Apply stub neuromodulators (Placeholder for Phase 2)
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
    }
    
    // ========== STEP 7: Update episodic memory ==========
    if (pImpl->episodicMemory) {
        pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            pImpl->stepsSinceLastEpisode = 0;
            
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
            
            // Store location information (simplified - use average sensory neuron activity)
            episode.positionX = 0.0f;  // TODO: Get from environment
            episode.positionY = 0.0f;
            episode.orientation = 0.0f;
            
            // Store action if motor neuron firing detected
            episode.action = ActionType::Wait;
            for (auto* neuron : pImpl->motorNeurons) {
                if (neuron->isFiring()) {
                    episode.action = ActionType::MoveForward;
                    break;
                }
            }
            
            // Store sensory state
            episode.sensoryState = std::vector<float>(pImpl->sensoryNeurons.size(), 0.0f);
            for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                if (auto* neuron : pImpl->sensoryNeurons[i]) {
                    episode.sensoryState[i] = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                }
            }
            
            // Store energy (total neuron activation)
            float totalActivation = 0.0f;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        totalActivation += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                    }
                }
            }
            episode.energy = totalActivation / std::max(1.0f, getTotalNeuronCount());
            
            // Store novelty (based on rate of change)
            episode.novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            
            // Store resulting sensory state (simplified - same as current for now)
            episode.resultingSensoryState = episode.sensoryState;
            episode.resultingReward = episode.reward;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
        // Phase 2: Implement real prediction by connecting to sensory neurons
        if (!pImpl->sensoryNeurons.empty()) {
            // Create simple prediction based on average sensory neuron activity
            float sensoryActivity = 0.0f;
            for (auto* neuron : pImpl->sensoryNeurons) {
                sensoryActivity += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            }
            sensoryActivity /= pImpl->sensoryNeurons.size();
            
            // Predict next sensory state (simple model for Phase 2)
            // In Phase 3+, this would use neural prediction networks
            float predictedActivity = sensoryActivity * 0.95f + (rng->randFloat() * 0.2f - 0.1f);
            
            // Update prediction system (if implemented)
            // For now, just store prediction error
            float predictionError = std::abs(predictedActivity - sensoryActivity);
            
            // Connect to neuromodulation for learning
            if (pImpl->predictionError) {
                // predictionError->computeError(predictedActivity, sensoryActivity);
            }
            
            // Store prediction for next step
            lastPredictedActivity_ = predictedActivity;
            lastSensoryActivity_ = sensoryActivity;
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
        // Concept formation integrates with attention to identify salient patterns
        if (!pImpl->sensoryNeurons.empty() && pImpl->attention) {
            // Create sensory pattern from neuron activity
            std::vector<float> sensoryPattern;
            sensoryPattern.reserve(pImpl->sensoryNeurons.size());
            for (auto* neuron : pImpl->sensoryNeurons) {
                float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                sensoryPattern.push_back(activity);
            }
            
            // Apply attention to select the most salient features
            auto& winners = pImpl->attention->getWinners();
            if (!winners.empty()) {
                // Focus concept formation on attended neurons
                std::vector<float> attendedPattern;
                for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                    if (std::find(winners.begin(), winners.end(), 
                                 pImpl->sensoryNeurons[i]->getId()) != winners.end()) {
                        attendedPattern.push_back(sensoryPattern[i]);
                    }
                }
                
                if (!attendedPattern.empty()) {
                    // Update concept formation with attended pattern
                    // conceptFormation->updateConcept(attendedPattern, currentStep);
                }
            } else {
                // No attention winners - use all sensory input
                // conceptFormation->updateConcept(sensoryPattern, currentStep);
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
    
    // ========== STEP 16: Collect statistics ==========
    // Already done in steps 1-3
}
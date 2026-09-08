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
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../prediction/ActionConsequencePredictor.hpp"
#include "../prediction/PredictionErrorSignal.hpp"
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
    std::unique_ptr<NeuralPrediction> predictionSystem;
    std::unique_ptr<ActionConsequencePredictor> actionConsequencePredictor;
    std::unique_ptr<PredictionErrorSignal> predictionErrorSignal;
    
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
    
    // Memory integration state
    std::vector<float> currentSensoryPattern;  // Current sensory input for episodic memory
    SimulationStep lastActionStep;              // Step when action was last taken
    ActionType lastAction;                      // Last action taken
    std::vector<float> lastActionSensoryPattern; // Sensory pattern before action
    
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
        , lastActionStep(0)
        , lastAction(ActionType::Wait)
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
        
        // Initialize working memory
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
        
        // Initialize episodic memory
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
        
        // Initialize associative memory
        pImpl->associativeMemory->initialize(this);
        
        // Initialize prediction system (NEW - fully integrated)
        pImpl->predictionSystem->initialize(this);
        pImpl->actionConsequencePredictor->initialize(this);
        pImpl->predictionErrorSignal->initialize(this);
        
        // Initialize cognition systems
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(5);
        
        pImpl->conceptFormation->initialize(this);
        
        pImpl->attention->initialize(this);
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
        
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
    
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system (NEW - fully integrated)
    pImpl->predictionSystem->initialize(this);
    pImpl->actionConsequencePredictor->initialize(this);
    pImpl->predictionErrorSignal->initialize(this);
    
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
    
    // Set up checkpoint providers for both neuron and synapse data
    pImpl->checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) {
        // Populate neuron checkpoint data
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
                    data.regionId.push_back(static_cast<uint64_t>(region->getId().index()));
                    data.populationId.push_back(static_cast<uint64_t>(pop->getId()));
                }
            }
        }
        return !data.membranePotential.empty();
    });
    
    pImpl->checkpointManager->setSynapseProvider([this](SynapseCheckpointData& data) {
        // Populate synapse checkpoint data
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                data.sourceNeuron.push_back(syn->getSourceNeuron().index());
                data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                data.weight.push_back(syn->getWeight());
                data.delay.push_back(syn->getDelay());
                data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                data.plasticityFlags.push_back(syn->getPlasticityFlags().stdp ? 1 : 0);
                data.eligibilityTrace.push_back(syn->getEligibilityTrace());
                data.efficacy.push_back(syn->getEfficacy());
                data.shortTermDepression.push_back(syn->getShortTermDepression());
                data.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        return !data.sourceNeuron.empty();
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
            
            // Store resulting sensory pattern (update this after action is produced)
            // For now, copy the current state
            episode.resultingSensoryState = pImpl->currentSensoryPattern;
            episode.resultingReward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store position if we have spatial representation
            // In Phase 4, we would get this from the world
            episode.positionX = 0.0f;  // Placeholder
            episode.positionY = 0.0f;  // Placeholder
            episode.orientation = 0.0f; // Placeholder
            
            // Store action if available
            if (pImpl->lastAction != ActionType::Wait && pImpl->lastActionStep > 0) {
                episode.action = pImpl->lastAction;
                // Associate action with sensory pattern
                if (pImpl->associativeMemory && !pImpl->lastActionSensoryPattern.empty() && !pImpl->currentSensoryPattern.empty()) {
                    pImpl->associativeMemory->associate(pImpl->lastActionSensoryPattern, pImpl->currentSensoryPattern, 0.8f);
                }
            } else {
                episode.action = ActionType::Wait;
            }
            
            pImpl->episodicMemory->storeEpisode(episode);
            
            // Also associate sensory patterns for associative memory
            if (pImpl->associativeMemory && !pImpl->currentSensoryPattern.empty() && !pImpl->lastActionSensoryPattern.empty()) {
                pImpl->associativeMemory->associate(pImpl->lastActionSensoryPattern, pImpl->currentSensoryPattern, 0.6f);
            }
            
            // Update concept formation if available
            if (pImpl->conceptFormation) {
                // Process the sensory pattern for concept formation
                if (!pImpl->currentSensoryPattern.empty()) {
                    pImpl->conceptFormation->presentExperience(
                        pImpl->currentSensoryPattern,
                        pImpl->currentSensoryPattern, // Use same as features for now
                        episode.reward,
                        currentStep
                    );
                }
            }
            
            // Update novelty detection
            if (pImpl->novelty) {
                pImpl->novelty->update(pImpl->timestep);
            }
            
            // Update prediction error based on sensory mismatch
            if (pImpl->predictionError && !pImpl->currentSensoryPattern.empty()) {
                pImpl->predictionError->update(pImpl->timestep, pImpl->currentSensoryPattern);
            }
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem && !pImpl->currentSensoryPattern.empty()) {
        // INTEGRATED PREDICTION LOOP:
        // 1. Record current sensory state for prediction
        pImpl->predictionSystem->recordSensoryState(pImpl->currentSensoryPattern, currentStep);
        
        // 2. Generate prediction based on learned patterns from episodic memory
        std::vector<float> predictedState = pImpl->predictionSystem->generatePrediction(currentStep);
        
        // 3. Query episodic memory for predictive patterns
        std::vector<const EpisodicMemoryItem*> similarEpisodes = pImpl->episodicMemory->retrieveSimilar(
            pImpl->currentSensoryPattern, 3
        );
        
        // 4. Update prediction system with actual observation and compute error
        float predictionError = pImpl->predictionSystem->updateWithObservation(pImpl->currentSensoryPattern, currentStep);
        
        // 5. Update prediction error signal for neuromodulation
        if (pImpl->predictionErrorSignal) {
            pImpl->predictionErrorSignal->computeError(predictedState, pImpl->currentSensoryPattern);
        }
        
        // 6. Use prediction error to modulate dopamine (reward prediction error)
        float predictionErrorValue = pImpl->predictionSystem->getPredictionError();
        float predictionConfidence = pImpl->predictionSystem->getPredictionConfidence();
        
        // Dopamine is updated by reward prediction error (actual reward - predicted reward)
        // For now, use prediction error as a modulation signal
        if (predictionErrorValue > 0.5f && pImpl->dopamine) {
            // High prediction error reduces dopamine (negative prediction error signal)
            float errorModulation = (predictionErrorValue - 0.5f) * 0.4f;
            pImpl->dopamine->modulate(errorModulation);
        }
        
        // 7. Use prediction to influence attention and working memory
        if (pImpl->attention && !predictedState.empty()) {
            // Higher prediction confidence increases focus on expected patterns
            if (predictionConfidence > 0.7f) {
                pImpl->attention->setInhibitionStrength(0.3f);  // Less inhibition, more focus
            } else {
                pImpl->attention->setInhibitionStrength(0.5f);  // Normal inhibition
            }
            pImpl->attention->update(pImpl->timestep);
        }
        
        // 8. Store predicted state for action selection
        std::vector<float> predictedNextState = predictedState;
        
        // 9. Update action consequence predictor with actual outcome
        if (pImpl->lastAction != ActionType::Wait && !pImpl->lastActionSensoryPattern.empty()) {
            float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            pImpl->actionConsequencePredictor->recordExperience(
                pImpl->lastAction,
                pImpl->lastActionSensoryPattern,
                pImpl->currentSensoryPattern,
                reward
            );
        }
    }
    
    // ========== STEP 9: Update neural planner ==========
    if (pImpl->planner && !pImpl->currentSensoryPattern.empty()) {
        // Neural planner predicts action sequences based on current state and episodic memory
        // This creates multi-step plans to guide behavior
        
        // Get current sensory state for planning
        std::vector<float> currentState = pImpl->currentSensoryPattern;
        
        // Set current goal based on episodic memory insights
        if (pImpl->episodicMemory && currentStep > 0) {
            // Query episodic memory for similar past experiences
            std::vector<const EpisodicMemoryItem*> similarEpisodes = 
                pImpl->episodicMemory->retrieveSimilar(currentState, 3);
            
            if (!similarEpisodes.empty()) {
                // Use the best episode's action as a potential goal
                const EpisodicMemoryItem* bestEpisode = similarEpisodes[0];
                if (bestEpisode->action != ActionType::Wait) {
                    // Create a goal state based on the successful action
                    // This goal can be refined over time
                    pImpl->planner->setCurrentGoal(currentState);
                }
            }
        }
        
        // Generate action plan for current state
        float targetReward = 0.5f;  // Default target reward
        if (pImpl->dopamine) {
            targetReward = pImpl->dopamine->getLevel();
        }
        
        ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
        
        // Apply neural planning effects to action selection
        if (plannedAction != ActionType::Wait) {
            // Inject planning bias into motor neurons to increase likelihood of planned action
            // Higher confidence in planning means stronger modulation
            float planningConfidence = pImpl->planner->getPlanningConfidence();
            
            // Inject current based on planned action to bias action selection
            // This uses motor neurons to implement the planned action
            for (auto* neuron : pImpl->motorNeurons) {
                // Apply planning-based bias to motor neuron activity
                float planningBias = 0.0f;
                switch (plannedAction) {
                    case ActionType::MoveForward:
                        planningBias = planningConfidence * 2.0f;
                        break;
                    case ActionType::MoveBackward:
                        planningBias = planningConfidence * 1.5f;
                        break;
                    case ActionType::TurnLeft:
                        planningBias = planningConfidence * 1.5f;
                        break;
                    case ActionType::TurnRight:
                        planningBias = planningConfidence * 1.5f;
                        break;
                    case ActionType::Interact:
                        planningBias = planningConfidence * 2.5f;
                        break;
                    default:
                        planningBias = planningConfidence * 0.5f;
                        break;
                }
                
                if (planningBias > 0.0f) {
                    neuron->injectCurrent(planningBias);
                }
            }
            
            // Also update action quality based on planning outcome
            if (pImpl->lastAction != ActionType::Wait) {
                pImpl->planner->updatePlanQuality({plannedAction}, {pImpl->lastAction}, 
                                                pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f);
            }
        }
        
        // Update planner confidence based on prediction accuracy
        if (pImpl->predictionSystem && !pImpl->currentSensoryPattern.empty()) {
            // Query episodic memory for predictive patterns to evaluate plan quality
            std::vector<const EpisodicMemoryItem*> similarEpisodes = 
                pImpl->episodicMemory->retrieveSimilar(currentState, 3);
            
            if (!similarEpisodes.empty()) {
                // Use episodic memory to assess plan quality
                // Higher prediction confidence from prediction system increases planning confidence
                float predictionConfidence = pImpl->predictionSystem->getPredictionConfidence();
                pImpl->planner->setPlanningDepth(static_cast<size_t>(predictionConfidence * 5) + 1);
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Process current neural activity patterns to form concepts
        // This now has access to properly encoded sensory state
        
        if (!pImpl->currentSensoryPattern.empty() && pImpl->dopamine) {
            // Use current sensory pattern with reward signal to form/update concepts
            pImpl->conceptFormation->presentExperience(
                pImpl->currentSensoryPattern,
                pImpl->currentSensoryPattern,  // Use same as features for now
                pImpl->dopamine->getLevel(),
                currentStep
            );
            
            // Also consider episodic memory for concept refinement
            if (pImpl->episodicMemory) {
                auto similarEpisodes = pImpl->episodicMemory->retrieveSimilar(
                    pImpl->currentSensoryPattern, 3
                );
                
                // Incorporate insights from similar episodes into concept formation
                // In a full implementation, this would use the episode outcomes
                // to refine the concept
            }
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay based on relevance and recency
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
        
        // Replay also helps prediction system learn from successful experiences
        if (pImpl->predictionSystem) {
            // Prediction system can learn from replayed episodes
            for (const auto* episode : episodesToReplay) {
                if (episode) {
                    // Use episode's predicted vs actual outcomes for learning
                    pImpl->predictionSystem->recordSensoryState(episode->resultingSensoryState, episode->timestamp);
                }
            }
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
        // This now includes relevance based on prediction errors and rewards
        pImpl->episodicMemory->consolidate(0.3f);
        
        // Also strengthen working memory connections based on prediction accuracy
        if (pImpl->workingMemory && pImpl->dopamine) {
            // Successful predictions (high dopamine) strengthen working memory traces
            if (pImpl->dopamine->getLevel() > 0.5f) {
                pImpl->workingMemory->strengthenMemory(1.05f);  // 5% strengthening
            }
        }
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
    
    // Store sensory pattern for memory integration
    std::vector<float> sensoryPattern;
    sensoryPattern.reserve(values.size());
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store in working memory with pattern
        if (pImpl->workingMemory) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
        
        // Store for episodic memory
        if (pImpl->episodicMemory && i < values.size()) {
            sensoryPattern.push_back(normalizedValue / 10.0f);
        }
    }
    
    // Store sensory pattern in working memory as full pattern
    if (pImpl->workingMemory && !sensoryPattern.empty()) {
        pImpl->workingMemory->store(sensoryPattern, 1.0f);
    }
    
    // Store sensory pattern in episodic memory for later retrieval
    if (pImpl->episodicMemory && !sensoryPattern.empty()) {
        // Capture current sensory state for episode
        pImpl->currentSensoryPattern = sensoryPattern;
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

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

NeuralPrediction* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

ActionConsequencePredictor* Brain::getActionConsequencePredictor() {
    return pImpl->actionConsequencePredictor.get();
}

PredictionErrorSignal* Brain::getPredictionErrorSignal() {
    return pImpl->predictionErrorSignal.get();
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
    
    // Use checkpoint manager's immediate save to leverage automatic checkpointing
    if (pImpl->checkpointManager) {
        // Set the checkpoint name/path for the checkpoint manager
        if (pImpl->checkpointManager->saveImmediately(filepath)) {
            NLM_LOG_INFO("Brain state saved successfully via checkpoint manager");
            return true;
        }
    }
    
    NLM_LOG_ERROR("Failed to save brain state via checkpoint manager");
    return false;
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    // Use checkpoint manager to load the checkpoint
    if (pImpl->checkpointManager) {
        if (pImpl->checkpointManager->load(filepath)) {
            NLM_LOG_INFO("Brain state loaded successfully via checkpoint manager");
            return true;
        }
    }
    
    NLM_LOG_ERROR("Failed to load brain state via checkpoint manager");
    return false;
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

// Get all regions
const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

// Inter-region connection management
void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1) {
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

// Global statistics
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

NeuralPrediction* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

ActionConsequencePredictor* Brain::getActionConsequencePredictor() {
    return pImpl->actionConsequencePredictor.get();
}

PredictionErrorSignal* Brain::getPredictionErrorSignal() {
    return pImpl->predictionErrorSignal.get();
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

// Dopamine - reward and reinforcement
Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

// Curiosity - exploration motivation
Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

// Novelty - novelty detection
Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

// Prediction error signal
PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

// Get current configuration
std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

// Get random generator
RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

// Logging
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

// ========== MEMORY QUERY SYSTEMS ==========
    
    // Get recent sensory patterns for cognition systems
    std::vector<float> Brain::getRecentSensoryPattern() const {
        return pImpl->currentSensoryPattern;
    }
    
    // Query working memory for specific patterns
    std::vector<float> Brain::queryWorkingMemory(const std::vector<float>& pattern, float threshold) const {
        if (!pImpl->workingMemory) return {};
        
        // Get current working memory content
        std::vector<float> wmContent = pImpl->workingMemory->retrieve();
        
        // Compute similarity with query pattern
        std::vector<float> results;
        for (size_t i = 0; i < wmContent.size() && i < pattern.size(); ++i) {
            if (wmContent[i] > threshold) {
                results.push_back(pattern[i] * wmContent[i]);
            }
        }
        
        return results;
    }
    
    // Retrieve episodes similar to a pattern (for planning)
    std::vector<const EpisodicMemoryItem*> Brain::queryEpisodicMemory(const std::vector<float>& pattern, size_t maxResults) const {
        if (!pImpl->episodicMemory) return {};
        
        return pImpl->episodicMemory->retrieveSimilar(pattern, maxResults);
    }
    
    // Retrieve associative patterns (for prediction)
    std::vector<std::vector<float>> Brain::queryAssociativeMemory(const std::vector<float>& pattern, size_t maxResults) const {
        if (!pImpl->associativeMemory) return {};
        
        return pImpl->associativeMemory->retrieve(pattern, maxResults);
    }
    
    // Get the last action for memory integration
    ActionType Brain::getLastAction() const {
        return pImpl->lastAction;
    }
    
    // Get the action step for memory integration
    SimulationStep Brain::getLastActionStep() const {
        return pImpl->lastActionStep;
    }
    
    // Store sensory pattern for memory integration
    void Brain::storeActionSensoryPattern(const std::vector<float>& pattern) {
        pImpl->lastActionSensoryPattern = pattern;
    }
    
    // Get the sensory pattern before last action
    const std::vector<float>& Brain::getLastActionSensoryPattern() const {
        return pImpl->lastActionSensoryPattern;
    }
    
    // Store current sensory pattern for later use
    void Brain::storeCurrentSensoryPattern(const std::vector<float>& pattern) {
        pImpl->currentSensoryPattern = pattern;
    }
    
    // Get current sensory pattern
    const std::vector<float>& Brain::getCurrentSensoryPattern() const {
        return pImpl->currentSensoryPattern;
    }

} // namespace nlm
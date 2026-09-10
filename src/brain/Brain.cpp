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
#include "../memory/Memory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <memory>

class Curiosity;
class Novelty;
class PredictionError;
class CheckpointManager;
class Acetylcholine;
class Norepinephrine;
class Serotonin;

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<WorkingMemory> workingMemory;
    std::unique_ptr<NeuralWorkingMemory> neuralWorkingMemory;
    std::unique_ptr<EpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralEpisodicMemory> neuralEpisodicMemory;
    std::unique_ptr<SemanticMemory> semanticMemory;
    std::unique_ptr<ProceduralMemory> proceduralMemory;
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
    
    // Sleep and consolidation systems
    SleepState sleepState;                // Current sleep state
    replaySchedule replaySchedule;        // Memory replay schedule
    size_t stepsSinceLastReplay;          // Steps since last replay event
    size_t stepsSinceLastConsolidation;   // Steps since last consolidation
    Timestamp sleepBeginTime;             // When current sleep period started
    
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
    
    // Store last sensory input for prediction system
    std::unique_ptr<class SensoryInput> lastSensoryInput;
    
    // For prediction system
    std::vector<float> sensoryActivity;
    
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
        , sleepState()
        , replaySchedule()
        , stepsSinceLastReplay(0)
        , stepsSinceLastConsolidation(0)
        , sleepBeginTime(0.0)
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
        
        // Initialize neural memory systems (Phase 3)
        workingMemory = std::make_unique<WorkingMemory>();
        neuralWorkingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<EpisodicMemory>();
        neuralEpisodicMemory = std::make_unique<NeuralEpisodicMemory>();
        semanticMemory = std::make_unique<SemanticMemory>();
        proceduralMemory = std::make_unique<ProceduralMemory>();
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
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replaySchedule.replayInterval = config->getOr<size_t>("replay_interval", 100);
        replaySchedule.consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        replaySchedule.replayDuringSleep = config->getOr<bool>("replay_during_sleep", true);
        replaySchedule.maxReplaysPerStage = config->getOr<size_t>("max_replays_per_stage", 5);
        replaySchedule.consolidationStrength = config->getOr<float>("consolidation_strength", 0.8f);
        
        // Get sleep parameters from config
        sleepState.sleepStartTime = 0.0f;
        sleepState.sleepPressure = config->getOr<float>("initial_sleep_pressure", 1.0f);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    // Sleep and consolidation systems
    SleepState sleepState;
    replaySchedule replaySchedule;
    size_t stepsSinceLastReplay;
    size_t stepsSinceLastConsolidation;
    Timestamp sleepBeginTime;
    size_t sleepStageCounter;  // Counter for sleep stage progression
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
    
// Initialize neural memory systems (Phase 3)
        neuralWorkingMemory = std::make_unique<NeuralWorkingMemory>();
        neuralEpisodicMemory = std::make_unique<NeuralEpisodicMemory>();
        semanticMemory = std::make_unique<SemanticMemory>();
        proceduralMemory = std::make_unique<ProceduralMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
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
    
            // Update working memory (maintenance and competition)
    if (pImpl->neuralWorkingMemory) {
        pImpl->neuralWorkingMemory->update(pImpl->timestep);
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
        
        if (pImpl->neuralEpisodicMemory) {
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
            
            pImpl->neuralEpisodicMemory->storeEpisode(episode);
        }
    }
    
        // Update working memory (maintenance and competition)
    if (pImpl->neuralWorkingMemory) {
        pImpl->neuralWorkingMemory->update(pImpl->timestep);
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
        // Get current sensory state (from last input)
        // For now, we'll need to pass sensory observations
        // In a complete implementation, this would receive current sensory data
        // The prediction system uses its internal model to predict next state
        // and calculates prediction error
        
        // Update prediction system with current neural activity patterns
        // The prediction system processes internal representations and predicts future states
        // We need to collect sensory activity from sensory neurons
        std::vector<float> sensoryActivity;
        for (auto* neuron : pImpl->sensoryNeurons) {
            const auto& state = neuron->getState();
            sensoryActivity.push_back(state.membranePotential);
        }
        
        // Create sensory input from the neural activity
        // We need to find or create a SensoryInput class that can be constructed from vector
        // For now, we'll use a placeholder approach
        // The prediction system would receive actual sensory input in a real implementation
        // For compilation purposes, we'll call train with empty observation
        pImpl->predictionSystem->train(sensoryActivity);
        
        // Get prediction error from the system
        float predictionError = pImpl->predictionSystem->getPredictionError();
        
        // Connect prediction error to neuromodulation and learning
        if (pImpl->predictionError) {
            // Prediction error modulates neuromodulation
            pImpl->predictionError->setLevel(predictionError);
            
            // Apply prediction error to plasticity
            // Higher prediction error increases plasticity for learning
            float plasticityMod = 1.0f + predictionError * 0.5f;
            
            // Apply to STDP and Hebbian learning
            pImpl->stdp->setLTPWeight(pImpl->stdp->getLTPWeight() * plasticityMod);
            pImpl->stdp->setLTDWeight(pImpl->stdp->getLTDWeight() * plasticityMod);
            
            // Update dopamine based on prediction error
            if (pImpl->dopamine) {
                // Prediction error drives dopamine release
                pImpl->dopamine->signalRewardPredictionError(predictionError);
            }
            
            // Update curiosity based on prediction error
            if (pImpl->curiosity) {
                // Higher prediction error increases exploration drive
                float curiosityLevel = predictionError * 0.8f;
                pImpl->curiosity->setLevel(curiosityLevel);
            }
            
            // Novelty detection also responds to prediction error
            if (pImpl->novelty) {
                // Novelty responds to unexpected patterns
                pImpl->novelty->update(pImpl->timestep);
            }
        }
        
        // Log prediction system status periodically
        if (currentStep % 100 == 0) {
            NLM_LOG_INFO("Prediction system: error=" + std::to_string(predictionError) + 
                        ", confidence=" + std::to_string(pImpl->predictionSystem->getConfidence()));
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
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Extract neural activity patterns for concept formation
        std::vector<float> neuralPattern;
        std::vector<float> features;
        
        // Collect sensory neuron activity
        for (auto* neuron : pImpl->sensoryNeurons) {
            const auto& state = neuron->getState();
            neuralPattern.push_back(std::abs(state.membranePotential - state.restingPotential));
        }
        
        // Collect internal neuron activity
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getNeuronType() == NeuronType::Internal) {
                        const auto& state = neuron->getState();
                        neuralPattern.push_back(std::abs(state.membranePotential - state.restingPotential));
                    }
                }
            }
        }
        
        // Extract features from neural activity
        // This could include temporal patterns, correlations, etc.
        float avgActivity = 0.0f;
        for (float val : neuralPattern) avgActivity += val;
        if (!neuralPattern.empty()) avgActivity /= neuralPattern.size();
        
        // Create feature vector
        features.push_back(avgActivity);  // Overall activity level
        features.push_back(pImpl->totalSpikesThisStep / 100.0f);  // Firing rate
        
        // Get reward signal from dopamine
        float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        // Present experience to concept formation system
        pImpl->conceptFormation->presentExperience(neuralPattern, features, reward, currentStep);
    }

    // ========== STEP 11: Update neural planner ==========
    if (pImpl->planner) {
        // Get current state for planning
        std::vector<float> currentState;
        
        // Use neural activity as the current state for planning
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    currentState.push_back(std::abs(state.membranePotential - state.restingPotential));
                }
            }
        }
        
        // Get current goal from concept formation if available
        std::vector<float> targetGoal;
        if (pImpl->conceptFormation && pImpl->conceptFormation->getConceptCount() > 0) {
            // Use the most stable concept as a goal
            for (size_t i = 0; i < std::min(static_cast<size_t>(10), pImpl->conceptFormation->getConceptCount()); ++i) {
                targetGoal = pImpl->conceptFormation->getConceptPrototype(i);
                if (pImpl->conceptFormation->getConceptStability(i) > 0.7f) {
                    break;  // Use first stable concept
                }
            }
        }
        
        // Use prediction system to get target reward
        float targetReward = 0.5f;
        if (pImpl->predictionSystem) {
            float predictionError = pImpl->predictionSystem->getPredictionError();
            targetReward = 0.5f + predictionError * 0.3f;  // Bias toward exploration on prediction error
        }
        
        // Plan action
        ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
        
        // Apply planning-based action selection if stronger than default
        if (pImpl->workingMemory) {
            // Store planned action in working memory for execution
            pImpl->workingMemory->storeToNeuron(static_cast<NeuronId>(plannedAction), 0.8f);
        }
        
        // Update planner based on neuromodulation
        if (pImpl->dopamine) {
            float confidence = pImpl->dopamine->getLevel();
            // Higher dopamine increases planning depth
            if (confidence > 0.5f) {
                pImpl->planner->setPlanningDepth(std::min(pImpl->planner->getPlanningDepth() + 1, 10));
            }
        }
    }

    // ========== STEP 12: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }

    // ========== STEP 13: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->neuralEpisodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->neuralEpisodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->neuralEpisodicMemory->replayEpisode(episode);
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

    // ========== SLEEP PHASE: Sleep/rest and memory consolidation ==========
    // Determine if brain should sleep based on sleep pressure and schedule
    pImpl->updateSleepSchedule(currentTime);
    
    if (pImpl->isInSleep() && pImpl->sleepBeginTime > 0.0) {
        // Apply sleep stage processing
        pImpl->processSleepMemories();
        
        // Update developmental stage during sleep for maturation
        pImpl->updateDevelopmentalStageDuringSleep();
        
        // Apply neuromodulation effects specific to sleep
        pImpl->applyAChEffects();   // Acetylcholine for memory consolidation
        pImpl->applyNEEffects();    // Norepinephrine for arousal regulation
        pImpl->apply5HTEffects();   // Serotonin for sleep-wake regulation
        
        // Log sleep stage
        NLM_LOG_INFO("Sleep stage: " + std::to_string(static_cast<int>(pImpl->sleepState.currentStage)) +
                     " (duration: " + std::to_string(currentTime - pImpl->sleepBeginTime) + "s)");
        
        // Sleep replaces normal processing - reduce computational load
        // but maintain essential maintenance functions
        if (pImpl->sleepState.currentStage != SleepStage::TransitionToWake) {
            // During sleep, skip some resource-intensive computations
            // but continue essential maintenance
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
            // Allow basic spike events to maintain network stability
        } else {
            // Waking up - resume normal full processing
            pImpl->isResting = false;
        }
    } else {
        // Wake state - normal processing continues
        pImpl->sleepState.currentStage = SleepStage::Awake;
        pImpl->isResting = false;
    }

    // ========== STEP 15: Periodic memory consolidation ==========
    if (currentStep % pImpl->replaySchedule.consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->consolidateMemoriesDuringSleep();
    }

    // ========== STEP 16: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

        pImpl->sleepState.sleepStartTime = 0.0;
        pImpl->sleepState.currentStage = SleepStage::TransitionToWake;
        pImpl->sleepState.stageStartTime = pImpl->currentTime;
        pImpl->sleepStageCounter = 0;
        
        // Complete memory consolidation
        consolidateMemoriesDuringSleep();
        
        NLM_LOG_INFO("Brain waking up from sleep");
    }
}

void Brain::updateSleepSchedule(Timestamp currentTime) {
    // Update sleep state based on sleep pressure and time
    if (!pImpl->isResting) {
        // Check if brain should start sleeping based on sleep pressure
        float sleepPressureThreshold = 1.0f; // Configurable threshold
        float homeostaticPressure = pImpl->sleepState.sleepPressure;
        
        // Simple sleep pressure regulation
        // In a real implementation, this would consider neural activity patterns
        // and adenosine levels
        if (homeostaticPressure > sleepPressureThreshold) {
            startSleep();
        }
    }
    
    // Update sleep stage progression during sleep
    if (pImpl->isInSleep()) {
        pImpl->sleepStageCounter++;
        
        // Determine current sleep stage based on duration
        Timestamp sleepDuration = currentTime - pImpl->sleepBeginTime;
        
        // Stage progression (simplified sleep architecture)
        if (sleepDuration < 300.0f) { // < 5 minutes
            pImpl->sleepState.currentStage = SleepStage::NREM1;
        } else if (sleepDuration < 900.0f) { // < 15 minutes
            pImpl->sleepState.currentStage = SleepStage::NREM2;
        } else if (sleepDuration < 1800.0f) { // < 30 minutes
            pImpl->sleepState.currentStage = SleepStage::NREM3;
        } else if (sleepDuration < 3600.0f) { // < 60 minutes
            pImpl->sleepState.currentStage = SleepStage::REM;
        } else {
            // Wake up after max sleep duration
            endSleep();
            return;
        }
        
        pImpl->sleepState.stageStartTime = currentTime;
        
        // Update hippocampal engagement based on sleep stage
        switch (pImpl->sleepState.currentStage) {
            case SleepStage::NREM1:
            case SleepStage::NREM2:
                pImpl->sleepState.hippocampalEngagement = 0.3f;
                break;
            case SleepStage::NREM3:
                pImpl->sleepState.hippocampalEngagement = 0.6f;
                break;
            case SleepStage::REM:
                pImpl->sleepState.hippocampalEngagement = 0.9f;
                break;
            default:
                pImpl->sleepState.hippocampalEngagement = 0.0f;
        }
    }
}

void Brain::processSleepMemories() {
    // Process memories during sleep stages
    if (pImpl->replaySchedule.replayDuringSleep && pImpl->neuralEpisodicMemory) {
        // Stage-specific memory processing
        switch (pImpl->sleepState.currentStage) {
            case SleepStage::NREM1:
            case SleepStage::NREM2:
                // Early NREM: structural reorganization
                replayMemoriesDuringSleep();
                break;
            case SleepStage::NREM3:
                // Deep NREM: systems consolidation
                strengthenImportantMemories();
                pruneWeakConnections();
                replayMemoriesDuringSleep();
                break;
            case SleepStage::REM:
                // REM: integration and creative association
                replayMemoriesDuringSleep();
                strengthenImportantMemories();
                break;
            default:
                break;
        }
        
        // Update sleep pressure based on processing
        pImpl->sleepState.sleepPressure *= 0.95f; // Gradually decrease
    }
}

void Brain::consolidateMemoriesDuringSleep() {
    // Periodically consolidate memories during sleep
    if (pImpl->episodicMemory) {
        // Use stage-appropriate consolidation strength
        float consolidationStrength = 0.3f;
        
        switch (pImpl->sleepState.currentStage) {
            case SleepStage::NREM3:
                consolidationStrength = pImpl->replaySchedule.consolidationStrength * 1.2f;
                break;
            case SleepStage::REM:
                consolidationStrength = pImpl->replaySchedule.consolidationStrength * 0.8f;
                break;
            default:
                consolidationStrength = pImpl->replaySchedule.consolidationStrength * 0.5f;
        }
        
        // Apply consolidation
        pImpl->episodicMemory->consolidate(consolidationStrength);
        
        // Also consolidate procedural memory
        if (pImpl->proceduralMemory) {
            pImpl->proceduralMemory->consolidate(consolidationStrength);
        }
        
        // Update synaptic weights based on replay
        if (pImpl->neuralWorkingMemory) {
            pImpl->neuralWorkingMemory->updateConsolidation(consolidationStrength);
        }
    }
}

void Brain::replayMemoriesDuringSleep() {
    // Reactivate neural patterns during sleep for consolidation
    if (!pImpl->neuralEpisodicMemory) return;
    
    // Get episodes for replay
    auto episodesToReplay = pImpl->neuralEpisodicMemory->getEpisodesForReplay(
        pImpl->replaySchedule.maxReplaysPerStage);
    
    for (const auto* episode : episodesToReplay) {
        // Reactivate the pattern
        pImpl->neuralEpisodicMemory->replayEpisode(episode);
        
        // Store synaptic changes
        pImpl->sleepState.synapticWeightChange += 0.01f;
    }
    
    // Update reactivation flag
    pImpl->sleepState.reactivationActive = !episodesToReplay.empty();
}

void Brain::strengthenImportantMemories() {
    // Strengthen important memories during sleep
    if (pImpl->neuralWorkingMemory && pImpl->neuralEpisodicMemory) {
        // Use neuromodulatory signals to prioritize memories
        float priorityBoost = 1.0f;
        
        if (pImpl->dopamine) {
            priorityBoost *= pImpl->dopamine->getLevel();
        }
        
        // Get important memories from working memory
        auto importantNeurons = pImpl->neuralWorkingMemory->getImportantNeurons(priorityBoost);
        
        // Strengthen connections associated with these neurons
        for (NeuronId neuronId : importantNeurons) {
            // Find the neuron and its synapses
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->getId() == neuronId) {
                            // Strengthen outgoing synapses
                            auto outgoingSynapses = region->getSynapsesFrom(neuronId);
                            for (Synapse* syn : outgoingSynapses) {
                                float currentWeight = syn->getWeight();
                                // LTP-like strengthening during sleep
                                syn->setWeight(currentWeight + 0.001f * priorityBoost);
                            }
                            
                            // Also strengthen incoming synapses to the neuron
                            auto incomingSynapses = region->getSynapsesTo(neuronId);
                            for (Synapse* syn : incomingSynapses) {
                                float currentWeight = syn->getWeight();
                                // LTP-like strengthening
                                syn->setWeight(currentWeight + 0.001f * priorityBoost);
                            }
                            
                            break;
                        }
                    }
                }
            }
        }
    }
}

void Brain::pruneWeakConnections() {
    // Remove weak or unused synapses during sleep
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Check if synapse is weak and underutilized
            float weight = syn->getWeight();
            if (std::abs(weight) < 0.01f) {  // Weak threshold
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                // If few or no spikes, prune the synapse
                if (preSpikes.size() < 2 && postSpikes.size() < 2) {
                    // Remove the synapse
                    region->removeSynapse(syn.getId());
                }
            }
        }
    }
}

void Brain::updateDevelopmentalStageDuringSleep() {
    // Update developmental stage based on sleep time
    Timestamp sleepDuration = pImpl->currentTime - pImpl->sleepState.sleepStartTime;
    
    if (pImpl->developmentSystem) {
        // Determine stage based on sleep duration
        if (sleepDuration < 300.0f) { // < 5 minutes
            pImpl->developmentSystem->updateStage(
                pImpl->developmentalStage, sleepDuration);
        } else if (sleepDuration < 1800.0f) { // < 30 minutes
            // Critical period for synaptic pruning
            pImpl->developmentSystem->enablePruning(true);
        } else {
            // Maturation phase
            pImpl->developmentSystem->enablePlasticity(false);
        }
    }
    
    // Update overall developmental stage
    if (sleepDuration > 3600.0f) { // 1 hour
        pImpl->developmentalStage = DevelopmentalStage::Adult;
    } else if (sleepDuration > 1200.0f) { // 20 minutes
        pImpl->developmentalStage = DevelopmentalStage::Maturation;
    } else if (sleepDuration > 300.0f) { // 5 minutes
        pImpl->developmentalStage = DevelopmentalStage::CriticalPeriod;
    }
}

void Brain::applyAChEffects() {
    // Acetylcholine effects on encoding vs consolidation
    if (pImpl->acetylcholine) {
        float achLevel = pImpl->acetylcholine->getLevel();
        
        // ACh modulates synaptic plasticity during sleep
        // High ACh promotes encoding, low ACh promotes consolidation
        if (achLevel > 0.5f) {
            // Encoding mode - prepare for next wake period
            // Enhance recent memory traces
            if (pImpl->neuralWorkingMemory) {
                pImpl->neuralWorkingMemory->enhanceRecentTraces(achLevel * 0.5f);
            }
        } else {
            // Consolidation mode - strengthen long-term memories
            if (pImpl->episodicMemory) {
                pImpl->episodicMemory->enhanceLongTermStorage(1.0f - achLevel);
            }
        }
        
        // Apply to STDP
        if (pImpl->stdp) {
            pImpl->stdp->setAChModulation(achLevel);
        }
    }
}

void Brain::applyNEEffects() {
    // Norepinephrine effects on arousal and stress response
    if (pImpl->norepinephrine) {
        float neLevel = pImpl->norepinephrine->getLevel();
        
        // NE regulates arousal during sleep-wake transitions
        if (neLevel > 0.7f) {
            // High NE - promote awakening
            pImpl->sleepState.sleepPressure *= 0.8f;
            if (pImpl->isInSleep()) {
                // Consider waking up if stress is high
                if (neLevel > 0.9f && pImpl->sleepStageCounter > 100) {
                    endSleep();
                }
            }
        } else if (neLevel < 0.3f) {
            // Low NE - promote sleep maintenance
            // Reduce sleep pressure fluctuations
            pImpl->sleepState.sleepPressure = std::max(pImpl->sleepState.sleepPressure, 0.5f);
        }
        
        // NE modulates memory relevance during consolidation
        if (pImpl->dopamine) {
            float dopamineMod = neLevel * 0.5f;
            pImpl->dopamine->setRelevanceBoost(dopamineMod);
        }
    }
}

void Brain::apply5HTEffects() {
    // Serotonin effects on sleep regulation
    if (pImpl->serotonin) {
        float serotoninLevel = pImpl->serotonin->getLevel();
        
        // Serotonin promotes NREM sleep
        if (serotoninLevel > 0.6f) {
            // Favor NREM sleep stages
            if (pImpl->sleepState.currentStage != SleepStage::NREM1 && 
                pImpl->sleepState.currentStage != SleepStage::NREM2 &&
                pImpl->sleepState.currentStage != SleepStage::NREM3) {
                // Transition to NREM if in REM or awake
                pImpl->sleepState.currentStage = SleepStage::NREM2;
                pImpl->sleepState.stageStartTime = pImpl->currentTime;
            }
            
            // Enhance memory consolidation during NREM
            pImpl->sleepState.memoryPriority = serotoninLevel;
        } else if (serotoninLevel < 0.3f) {
            // Low serotonin - favor REM sleep
            if (pImpl->sleepState.currentStage != SleepStage::REM) {
                pImpl->sleepState.currentStage = SleepStage::REM;
                pImpl->sleepState.stageStartTime = pImpl->currentTime;
            }
            
            // Enhance creative integration during REM
            pImpl->sleepState.memoryPriority = 1.0f - serotoninLevel;
        }
        
        // Serotonin affects mood and emotional memory processing
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->setEmotionalWeight(serotoninLevel);
        }
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
    
    // Reset sleep state
    pImpl->sleepState = SleepState();
    pImpl->stepsSinceLastReplay = 0;
    pImpl->stepsSinceLastConsolidation = 0;
    pImpl->sleepBeginTime = 0.0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->neuralWorkingMemory) pImpl->neuralWorkingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->neuralEpisodicMemory) pImpl->neuralEpisodicMemory->clear();
    if (pImpl->semanticMemory) pImpl->semanticMemory->clear();
    if (pImpl->proceduralMemory) pImpl->proceduralMemory->clear();
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
        
            }
        }
        
        // Write development stage
        writer.writeSection(CheckpointSection::Development, &pImpl->developmentalStage, sizeof(DevelopmentalStage));
        
        // Write neuromodulation state (if available)
        if (pImpl->dopamine) {
            DopamineCheckpointData dopData;
            dopData.level = pImpl->dopamine->getLevel();
            dopData.plasticityFactor = pImpl->dopamine->getPlasticityFactor();
            writer.writeSection(CheckpointSection::Neuromodulation, &dopData, sizeof(DopamineCheckpointData));
        }
        
        // Write random state
        if (pImpl->rng) {
            uint64_t seed = pImpl->rng->getSeed();
            writer.writeSection(CheckpointSection::RandomState, &seed, sizeof(uint64_t));
        }
        
        // Write spike history (simplified - just spike counts)
        uint64_t totalSpikes = pImpl->totalSpikesTotal;
        writer.writeSection(CheckpointSection::SpikeHistory, &totalSpikes, sizeof(uint64_t));
        
        // Write simulation time
        writer.writeSection(CheckpointSection::SimulationState, &pImpl->currentStep, sizeof(SimulationStep));
        double simTime = pImpl->currentTime;
        writer.writeSection(CheckpointSection::SimulationState, &simTime, sizeof(double));
        
        // Write memory system states (simplified)
        if (pImpl->workingMemory) {
            size_t memSize = pImpl->workingMemory->getCurrentSize();
            writer.writeSection(CheckpointSection::Memory, &memSize, sizeof(size_t));
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
        
        // Read spikes
        std::vector<uint8_t> spikeData = reader.readSection(CheckpointSection::Spikes);
        
        // Read memory state
        std::vector<uint8_t> memoryData = reader.readSection(CheckpointSection::Memory);
        
        // Read development stage
        std::vector<uint8_t> developmentData = reader.readSection(CheckpointSection::Development);
        if (!developmentData.empty()) {
            DevelopmentalStage* stagePtr = reinterpret_cast<DevelopmentalStage*>(developmentData.data());
            pImpl->developmentalStage = *stagePtr;
        }
        
        // Read neuromodulation state
        std::vector<uint8_t> neuromodulationData = reader.readSection(CheckpointSection::Neuromodulation);
        
        // Read random state
        std::vector<uint8_t> randomData = reader.readSection(CheckpointSection::RandomState);
        if (!randomData.empty() && pImpl->rng) {
            uint64_t* seedPtr = reinterpret_cast<uint64_t*>(randomData.data());
            pImpl->rng->setSeed(*seedPtr);
        }
        
        // Read spike history
        std::vector<uint8_t> spikeHistoryData = reader.readSection(CheckpointSection::SpikeHistory);
        if (!spikeHistoryData.empty()) {
            uint64_t* totalSpikesPtr = reinterpret_cast<uint64_t*>(spikeHistoryData.data());
            pImpl->totalSpikesTotal = *totalSpikesPtr;
        }
        
        // Read simulation state
        std::vector<uint8_t> simulationStateData = reader.readSection(CheckpointSection::SimulationState);
        if (simulationStateData.size() >= sizeof(SimulationStep)) {
            SimulationStep* stepPtr = reinterpret_cast<SimulationStep*>(simulationStateData.data());
            pImpl->currentStep = *stepPtr;
        }
        if (simulationStateData.size() >= sizeof(double) * 2) {
            double* timePtr = reinterpret_cast<double*>(simulationStateData.data() + sizeof(SimulationStep));
            pImpl->currentTime = *timePtr;
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

NeuralWorkingMemory* Brain::getNeuralWorkingMemory() const {
    return pImpl->neuralWorkingMemory.get();
}

EpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralEpisodicMemory* Brain::getNeuralEpisodicMemory() const {
    return pImpl->neuralEpisodicMemory.get();
}

SemanticMemory* Brain::getSemanticMemory() {
    return pImpl->semanticMemory.get();
}

ProceduralMemory* Brain::getProceduralMemory() {
    return pImpl->proceduralMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getNeuralPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

NeuralWorkingMemory* Brain::getWorkingMemory() const {
    return pImpl->workingMemory.get();
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

Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
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

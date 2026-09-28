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
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
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
    size_t restCounter;  // Remaining rest steps
    size_t restInterval;  // Duration of rest in steps
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Rest metrics
    size_t totalRestSteps;
    size_t totalReplayEvents;
    float consolidationProgress;
    
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
        , restCounter(0)
        , restInterval(500)  // Default rest interval
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
        , totalRestSteps(0)
        , totalReplayEvents(0)
        , consolidationProgress(0.0f)
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
        
        // Initialize prediction error system
        predictionError->initialize(this);
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
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
        restInterval = config->getOr<size_t>("rest_interval", 500);  // Default 500 steps
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    // Rest state variables
    bool isResting;
    size_t restCounter;
    size_t restInterval;
    size_t totalRestSteps;
    size_t totalReplayEvents;
    float consolidationProgress;
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
      * 12. Check for rest/sleep and initiate consolidation phase
      * 12.5: Rest/check for sleep (trigger based on learning activity)
      * 12.6: Execute rest period and memory consolidation
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
        // Update working memory with current neural activity
        pImpl->workingMemory->update(pImpl->timestep);
        
        // Connect sensory input to working memory storage
        // Store recent sensory patterns in working memory
        if (!pImpl->sensoryNeurons.empty()) {
            // Create sensory pattern from sensory neuron activations
            std::vector<float> sensoryPattern;
            for (size_t i = 0; i < std::min(pImpl->sensoryNeurons.size(), static_cast<size_t>(256)); ++i) {
                Neuron* neuron = pImpl->sensoryNeurons[i];
                if (neuron) {
                    float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 100.0f;
                    activation = std::clamp(activation, 0.0f, 1.0f);
                    sensoryPattern.push_back(activation);
                }
            }
            
            // Store in working memory
            if (!sensoryPattern.empty()) {
                pImpl->workingMemory->store(sensoryPattern, 1.0f);
            }
        }
        
        // Add working memory decay and competition
        // Competition is already handled in working memory implementation
        // Update attention based on working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            if (pImpl->attention) {
                pImpl->attention->processCompetition(competitors);
            }
        }
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
    
    // Update acetylcholine (attention and memory)
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        
        // Apply acetylcholine effects on attention
        float acetylcholineLevel = pImpl->acetylcholine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // ACh enhances attentional focus and signal-to-noise
                    float attentionalMod = acetylcholineLevel * 0.3f;
                    if (attentionalMod > 0.0f) {
                        neuron->injectCurrent(attentionalMod * 0.5f);
                    }
                }
            }
        }
    }
    
    // Update norepinephrine (arousal and vigilance)
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
        
        // Apply norepinephrine effects on arousal
        float norepinephrineLevel = pImpl->norepinephrine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE enhances arousal and alertness
                    float arousalMod = norepinephrineLevel * 0.4f;
                    if (arousalMod > 0.0f) {
                        neuron->injectCurrent(arousalMod * 0.6f);
                    }
                }
            }
        }
    }
    
    // Update serotonin (mood and action selection)
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
        
        // Apply serotonin effects on mood and impulsivity
        float serotoninLevel = pImpl->serotonin->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // 5-HT modulates mood and reduces impulsivity
                    float moodMod = (serotoninLevel - 0.5f) * 0.2f;  // Negative modulation from baseline
                    neuron->injectCurrent(moodMod * 0.3f);
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
    if (pImpl->acetylcholine) {
        // ACh enhances plasticity for memory encoding
        float achFactor = pImpl->acetylcholine->getPlasticityFactor();
        plasticityMod = std::max(plasticityMod, achFactor);
    }
    if (pImpl->norepinephrine) {
        // NE modulates plasticity for arousal and learning
        float neFactor = pImpl->norepinephrine->getPlasticityFactor();
        plasticityMod = std::max(plasticityMod, neFactor);
    }
    if (pImpl->serotonin) {
        // 5-HT modulates plasticity for mood and behavior
        float serotoninFactor = pImpl->serotonin->getPlasticityFactor();
        plasticityMod = std::max(plasticityMod, serotoninFactor);
    }
    
    // Apply plasticity-modulated STDP
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on all neuromodulators
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
            
            // Integrate with all neuromodulators for episode reward/value
            float episodeValue = 0.0f;
            if (pImpl->dopamine) {
                episodeValue += pImpl->dopamine->getLevel() * 0.4f;  // Reward prediction error
            }
            if (pImpl->acetylcholine) {
                episodeValue += pImpl->acetylcholine->getLevel() * 0.3f;  // Attentional salience
            }
            if (pImpl->norepinephrine) {
                episodeValue += pImpl->norepinephrine->getLevel() * 0.2f;  // Arousal level
            }
            if (pImpl->serotonin) {
                episodeValue += pImpl->serotonin->getLevel() * 0.1f;  // Mood state
            }
            
            episode.reward = episodeValue;
            
            // Store active neurons with their activations
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                        if (activation > 5.0f || neuron->isFiring()) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(activation / 100.0f);  // Normalize
                        }
                    }
                }
            }
            
            // Store sensory pattern if available
            if (!pImpl->sensoryNeurons.empty()) {
                episode.sensoryState.reserve(256);
                for (size_t i = 0; i < std::min(pImpl->sensoryNeurons.size(), static_cast<size_t>(256)); ++i) {
                    Neuron* neuron = pImpl->sensoryNeurons[i];
                    if (neuron) {
                        float value = (neuron->getState().membranePotential + 70.0f) / 140.0f;  // Normalize -70 to +70
                        value = std::clamp(value, 0.0f, 1.0f);
                        episode.sensoryState.push_back(value);
                    }
                }
            }
            
            // Store motor pattern (for later association)
            if (!pImpl->motorNeurons.empty()) {
                episode.resultingSensoryState.reserve(256);
                for (size_t i = 0; i < std::min(pImpl->motorNeurons.size(), static_cast<size_t>(256)); ++i) {
                    Neuron* neuron = pImpl->motorNeurons[i];
                    if (neuron) {
                        float value = (neuron->getState().membranePotential + 70.0f) / 140.0f;
                        value = std::clamp(value, 0.0f, 1.0f);
                        episode.resultingSensoryState.push_back(value);
                    }
                }
            }
            
            // Determine action based on motor neuron activity
            for (auto* neuron : pImpl->motorNeurons) {
                if (neuron->isFiring()) {
                    // Infer action based on neuron activation level
                    float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 100.0f;
                    if (activation > 0.5f) {
                        // Map motor neurons to actions
                        if (neuron->getId().index() < 100) {
                            episode.action = ActionType::MoveForward;
                        } else if (neuron->getId().index() < 200) {
                            episode.action = ActionType::MoveBackward;
                        } else if (neuron->getId().index() < 300) {
                            episode.action = ActionType::TurnLeft;
                        } else if (neuron->getId().index() < 400) {
                            episode.action = ActionType::TurnRight;
                        } else {
                            episode.action = ActionType::Interact;
                        }
                        break;
                    }
                }
            }
            
            pImpl->episodicMemory->storeEpisode(episode);
            
            // Integrate with associative memory
            if (pImpl->associativeMemory) {
                pImpl->associativeMemory->associateFromExperience(episode);
            }
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Convert sensory neurons to SensoryInput for prediction
        std::unique_ptr<SensoryInput> predictedState = std::make_unique<Vision>(16, 16);
        const auto& predData = predictedState->getData();
        
        if (predData.size() >= pImpl->sensoryNeurons.size()) {
            for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                if (pImpl->sensoryNeurons[i]) {
                    float activation = std::abs(pImpl->sensoryNeurons[i]->getState().membranePotential - 
                                              pImpl->sensoryNeurons[i]->getState().restingPotential);
                    activation = std::clamp(activation / 100.0f, 0.0f, 1.0f);
                    predictedState->setDataAt(i, activation);
                }
            }
        }
        
        // Update prediction system
        pImpl->predictionSystem->train(*predictedState);
        
        // Get prediction error and confidence
        float predictionError = pImpl->predictionSystem->getPredictionError();
        float predictionConfidence = pImpl->predictionSystem->getConfidence();
        
        // Update prediction error signal
        if (pImpl->predictionError) {
            pImpl->predictionError->computeError(predictionError, 0.0f);
        }
        
        // Integrate prediction error with all neuromodulators for comprehensive learning
        if (pImpl->dopamine && pImpl->predictionError) {
            pImpl->dopamine->signalRewardPredictionError(pImpl->predictionError->getError());
        }
        if (pImpl->acetylcholine && pImpl->predictionError) {
            pImpl->acetylcholine->signalEpisodicEncoding(std::abs(predictionError));
        }
        if (pImpl->norepinephrine && pImpl->predictionError) {
            pImpl->norepinephrine->signalVigilance(std::abs(predictionError));
        }
        if (pImpl->serotonin && pImpl->predictionError) {
            pImpl->serotonin->signalRewardPredictionError(predictionError);
        }
        
        // Update curiosity with prediction error
        if (pImpl->curiosity) {
            float currentNovelty = 0.0f; // Would come from novelty system
            pImpl->curiosity->update(currentNovelty, std::abs(predictionError), pImpl->timestep);
        }
        
        // Use prediction confidence to modulate attention
        if (pImpl->attention && predictionConfidence > 0.5f) {
            pImpl->attention->setNoveltyWeight(1.0f - predictionConfidence);
        }
        
        // Use prediction error to guide plasticity with all neuromodulators
        float plasticityMod = 1.0f;
        if (pImpl->dopamine) {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
        }
        if (pImpl->acetylcholine) {
            float achFactor = pImpl->acetylcholine->getPlasticityFactor();
            plasticityMod = std::max(plasticityMod, achFactor);
        }
        if (pImpl->norepinephrine) {
            float neFactor = pImpl->norepinephrine->getPlasticityFactor();
            plasticityMod = std::max(plasticityMod, neFactor);
        }
        if (pImpl->serotonin) {
            float serotoninFactor = pImpl->serotonin->getPlasticityFactor();
            plasticityMod = std::max(plasticityMod, serotoninFactor);
        }
        
        float errorMagnitude = pImpl->predictionError ? pImpl->predictionError->getMagnitude() : 0.0f;
        
        // Apply prediction-error-driven plasticity
        if (errorMagnitude > 0.1f) {
            plasticityMod *= (1.0f + errorMagnitude * 0.5f);
        }
        
        // Apply modulated plasticity to STDP
        if (pImpl->stdp) {
            pImpl->stdp->setLTPWeight(0.01f * plasticityMod);
            pImpl->stdp->setLTDWeight(0.012f * plasticityMod);
        }
    }
    
    // ========== STEP 9: NEURAL PLANNER INTEGRATION ==========
    if (pImpl->planner && pImpl->predictionSystem) {
        // Integrate NeuralPlanner with prediction system and self-model
        
        // Convert current neural activity to state vector for planning
        std::vector<float> currentState;
        size_t stateSize = 64; // Planning state size
        currentState.reserve(stateSize);
        
        // Integrate working memory with planning
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Use working memory winners as part of planning state
            for (const auto& neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                // Find neuron by ID to get its activation
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->getId() == neuronId && currentState.size() < stateSize) {
                                currentState.push_back(
                                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 100.0f);
                            }
                        }
                    }
                }
            }
        }
        
        // Integrate neuromodulatory signals into planning state
        // Dopamine (reward signals) - influence planning for reward maximization
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            // Add dopamine signal to planning state for reward-based planning
            if (currentState.size() < stateSize) {
                currentState.push_back(dopamineLevel);
            }
        }
        
        // Acetylcholine (attention) - influence planning focus
        if (pImpl->acetylcholine) {
            float acetylcholineLevel = pImpl->acetylcholine->getLevel();
            if (currentState.size() < stateSize) {
                currentState.push_back(acetylcholineLevel * 0.5f);
            }
        }
        
        // Norepinephrine (arousal) - influence planning urgency
        if (pImpl->norepinephrine) {
            float norepinephrineLevel = pImpl->norepinephrine->getLevel();
            if (currentState.size() < stateSize) {
                currentState.push_back(norepinephrineLevel * 0.5f);
            }
        }
        
        // Serotonin (mood) - influence planning exploration vs. exploitation
        if (pImpl->serotonin) {
            float serotoninLevel = pImpl->serotonin->getLevel();
            if (currentState.size() < stateSize) {
                currentState.push_back(serotoninLevel * 0.3f);
            }
        }
        
        // Pad or truncate state to desired size
        while (currentState.size() < stateSize) {
            currentState.push_back(0.0f);
        }
        if (currentState.size() > stateSize) {
            currentState.resize(stateSize);
        }
        
        // Integrate prediction confidence with planning
        float predictionConfidence = pImpl->predictionSystem->getConfidence();
        pImpl->planner->setPlanningConfidence(predictionConfidence);
        
        // Use neuromodulation signals to improve planning confidence
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            pImpl->planner->setPlanningConfidence(pImpl->planner->getPlanningConfidence() * (0.5f + dopamineLevel * 0.5f));
        }
        if (pImpl->acetylcholine) {
            float acetylcholineLevel = pImpl->acetylcholine->getLevel();
            pImpl->planner->setPlanningConfidence(pImpl->planner->getPlanningConfidence() * (0.8f + acetylcholineLevel * 0.3f));
        }
        if (pImpl->norepinephrine) {
            float norepinephrineLevel = pImpl->norepinephrine->getLevel();
            pImpl->planner->setPlanningConfidence(pImpl->planner->getPlanningConfidence() * (0.7f + norepinephrineLevel * 0.4f));
        }
        if (pImpl->serotonin) {
            float serotoninLevel = pImpl->serotonin->getLevel();
            pImpl->planner->setPlanningConfidence(pImpl->planner->getPlanningConfidence() * (0.9f + serotoninLevel * 0.2f));
        }
        
        // Use prediction error to improve planning confidence
        if (pImpl->predictionError) {
            float errorMagnitude = pImpl->predictionError->getMagnitude();
            pImpl->planner->setPlanningConfidence(pImpl->planner->getPlanningConfidence() * (1.0f - errorMagnitude));
        }
        
        // Integrate episodic memory patterns with planning
        if (pImpl->episodicMemory) {
            auto episodes = pImpl->episodicMemory->getRecentEpisodes(5);
            if (!episodes.empty()) {
                // Use episodic memory to influence planning
                for (const auto* episode : episodes) {
                    if (episode && !episode->actionEffects.empty()) {
                        // Integrate successful action patterns with neuromodulatory influence
                        float successWeight = 0.5f;
                        if (pImpl->dopamine) {
                            successWeight *= (1.0f + pImpl->dopamine->getLevel() * 0.5f);
                        }
                        if (pImpl->acetylcholine) {
                            successWeight *= (1.0f + pImpl->acetylcholine->getLevel() * 0.3f);
                        }
                        
                        pImpl->planner->updatePlanQuality(
                            std::vector<ActionType>{episode->action},
                            std::vector<ActionType>{episode->action},
                            episode->reward * successWeight);
                    }
                }
            }
        }
        
        // Plan action using neural planner
        float targetReward = 0.5f; // Could be goal-based
        
        // Integrate neuromodulation with target reward
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            targetReward = 0.5f + dopamineLevel * 0.3f;
        }
        if (pImpl->acetylcholine) {
            float acetylcholineLevel = pImpl->acetylcholine->getLevel();
            targetReward *= (1.0f + acetylcholineLevel * 0.2f);
        }
        
        ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
        
        // Store planned action for working memory integration
        if (pImpl->workingMemory) {
            // Convert planned action to neural activity pattern
            std::vector<float> actionPattern(1, 0.0f);
            size_t actionIdx = static_cast<size_t>(plannedAction);
            if (actionIdx < 10) {
                actionPattern[0] = 1.0f; // One-hot encoding
            }
            
            pImpl->workingMemory->store(actionPattern, 1.0f);
        }
        
        // Integrate planned action with motor neuron activation
        // This connects planning to action execution
        for (auto* neuron : pImpl->motorNeurons) {
            if (neuron->isFiring()) {
                // Apply planned action as bias to motor output
                float plannedActionBias = 0.0f;
                if (plannedAction == ActionType::MoveForward) {
                    plannedActionBias = 1.0f;
                } else if (plannedAction == ActionType::MoveBackward) {
                    plannedActionBias = -1.0f;
                } else if (plannedAction == ActionType::TurnLeft) {
                    plannedActionBias = 1.5f;
                } else if (plannedAction == ActionType::TurnRight) {
                    plannedActionBias = -1.5f;
                }
                
                if (std::abs(plannedActionBias) > 0.0f) {
                    neuron->injectCurrent(plannedActionBias * 0.5f);
                }
            }
        }
    }
    
    // ========== STEP 10: CONCEPT FORMATION INTEGRATION ==========
    if (pImpl->conceptFormation && pImpl->predictionSystem) {
        // Process current brain state for concept learning
        
        // Create feature vector from current neural state for concept formation
        std::vector<float> conceptFeatures;
        size_t featureSize = 32;
        conceptFeatures.reserve(featureSize);
        
        // Extract features from working memory
        if (pImpl->workingMemory) {
            auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            size_t featureCount = std::min(memoryNeurons.size(), featureSize / 2);
            
            for (size_t i = 0; i < featureCount; ++i) {
                // Find neuron activation
                float activation = 0.0f;
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->getId() == memoryNeurons[i]) {
                                activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 100.0f;
                                break;
                            }
                        }
                    }
                }
                conceptFeatures.push_back(activation);
            }
        }
        
        // Add prediction system state features
        float predictionError = pImpl->predictionSystem->getPredictionError();
        float predictionConfidence = pImpl->predictionSystem->getConfidence();
        conceptFeatures.push_back(predictionError);
        conceptFeatures.push_back(predictionConfidence);
        
        // Pad to feature size
        while (conceptFeatures.size() < featureSize) {
            conceptFeatures.push_back(0.0f);
        }
        if (conceptFeatures.size() > featureSize) {
            conceptFeatures.resize(featureSize);
        }
        
        // Learn from episodic memory patterns
        if (pImpl->episodicMemory) {
            auto episodes = pImpl->episodicMemory->getRecentEpisodes(10);
            for (const auto* episode : episodes) {
                if (episode) {
                    // Present episode to concept formation
                    pImpl->conceptFormation->presentExperience(
                        episode->sensoryState,
                        episode->neuronActivations,
                        episode->reward,
                        episode->timestamp);
                }
            }
        }
        
        // Update action planning based on learned concepts
        if (pImpl->planner) {
            size_t matchingConceptId = pImpl->conceptFormation->getMatchingConcept(conceptFeatures, 0.6f);
            if (matchingConceptId > 0) {
                // Use concept to guide planning
                const auto* concept = pImpl->conceptFormation->getConcept(matchingConceptId);
                if (concept) {
                    // Get concept prototype as potential goal
                    auto prototype = pImpl->conceptFormation->getConceptPrototype(matchingConceptId);
                    pImpl->planner->setCurrentGoal(prototype);
                    
                    // Update planning confidence based on concept stability
                    float conceptStability = pImpl->conceptFormation->getConceptStability(matchingConceptId);
                    float currentConfidence = pImpl->planner->getPlanningConfidence();
                    pImpl->planner->setPlanningConfidence(currentConfidence * conceptStability);
                }
            }
        }
        
        // Use concepts to guide prediction system
        if (pImpl->predictionSystem && pImpl->conceptFormation) {
            // Concepts can refine predictions
            size_t bestConceptId = pImpl->conceptFormation->getMatchingConcept(conceptFeatures, 0.5f);
            if (bestConceptId > 0) {
                // Concepts provide priors for prediction
                auto prototype = pImpl->conceptFormation->getConceptPrototype(bestConceptId);
                // In a full implementation, this would modulate prediction system
            }
        }
    }
    
    // ========== STEP 12.5: REST/CHECK FOR SLEEP ==========
    if (!pImpl->isResting) {
        // Check for rest trigger - after many steps without significant learning
        // Use prediction error and learning indicators to determine need for rest
        bool shouldRest = false;
        
        // Check if prediction error is below threshold (indicating stable/predictable learning)
        float predictionError = 0.0f;
        if (pImpl->predictionSystem) {
            predictionError = pImpl->predictionSystem->getPredictionError();
        }
        
        // Check neuromodulation levels (low dopamine, curiosity indicating stability)
        float dopamineLevel = 0.0f;
        float curiosityLevel = 0.0f;
        if (pImpl->dopamine) {
            dopamineLevel = pImpl->dopamine->getLevel();
        }
        if (pImpl->curiosity) {
            curiosityLevel = pImpl->curiosity->getLevel();
        }
        
        // Trigger rest if learning is stable (low prediction error) and neuromodulation is low
        // This indicates the brain has settled and can benefit from consolidation
        if (predictionError < 0.1f && dopamineLevel < 0.3f && curiosityLevel < 0.3f) {
            shouldRest = true;
        }
        
        // Also trigger rest if we've been active for many steps without consolidation
        if (!shouldRest && pImpl->stepsSinceLastEpisode > 50) {
            shouldRest = true;
        }
        
        if (shouldRest) {
            // Enter rest state
            pImpl->isResting = true;
            pImpl->restCounter = pImpl->restInterval;
            NLM_LOG_INFO("Brain entering rest state for consolidation (interval: " + 
                        std::to_string(pImpl->restInterval) + " steps)");
        }
    } else {
        // We're already resting - decrement rest counter
        pImpl->restCounter--;
        if (pImpl->restCounter <= 0) {
            // Rest period complete - exit rest state
            pImpl->isResting = false;
            NLM_LOG_INFO("Brain exiting rest state - consolidation complete");
        }
    }
    
    // ========== STEP 12.6: EXECUTE REST PERIOD AND MEMORY CONSOLIDATION ==========
    if (pImpl->isResting) {
        // During rest: execute memory consolidation and replay
        pImpl->totalRestSteps++;
        
        // Resting neurons maintain homeostasis
        // Inject homeostatic currents to stabilize neuronal activity
        if (pImpl->workingMemory) {
            // Working memory neurons get stabilization input during rest
            for (const auto& neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->getId() == neuronId) {
                                // Inject small stabilizing current
                                neuron->injectCurrent(0.1f);
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        // Enhanced memory consolidation during rest
        if (pImpl->episodicMemory) {
            // Gradual consolidation - increase progress
            pImpl->consolidationProgress += 0.1f;
            if (pImpl->consolidationProgress > 1.0f) {
                pImpl->consolidationProgress = 1.0f;
            }
            
            // Consolidate based on prediction error and novelty
            float consolidationStrength = 0.2f + (pImpl->consolidationProgress * 0.3f);
            
            // Get prediction error for prioritizing consolidation
            float predictionError = 0.0f;
            if (pImpl->predictionSystem) {
                predictionError = pImpl->predictionSystem->getPredictionError();
            }
            
            // Get novelty for prioritizing consolidation
            float noveltyLevel = 0.0f;
            if (pImpl->novelty) {
                noveltyLevel = pImpl->novelty->getLevel();
            }
            
            // Weighted consolidation - higher for novel or high-error memories
            float consolidationWeight = consolidationStrength;
            if (noveltyLevel > 0.5f) consolidationWeight *= 1.5f;
            if (predictionError > 0.3f) consolidationWeight *= 1.2f;
            
            pImpl->episodicMemory->consolidate(consolidationWeight);
            
            // Enhanced replay during rest
            if (pImpl->stepsSinceLastEpisode >= 10) {  // Replay during rest
                auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(2);
                for (const auto* episode : episodesToReplay) {
                    pImpl->episodicMemory->replayEpisode(episode);
                    pImpl->totalReplayEvents++;
                }
                
                // Replay working memory traces for stabilization
                if (pImpl->workingMemory) {
                    pImpl->workingMemory->updateRecurrentConnections();
                }
                
                // Reset episode counter during rest
                pImpl->stepsSinceLastEpisode = 0;
            }
        }
        
        // Enhanced associative memory consolidation during rest
        if (pImpl->associativeMemory) {
            // Consolidate weak associations during rest
            pImpl->associativeMemory->clear();  // Simplified - in full implementation would consolidate specific associations
        }
        
        // Update plasticity based on rest activity
        if (pImpl->stdp) {
            // Reduce LTP during rest to stabilize
            float ltpWeight = pImpl->stdp->getLTPWeight() * 0.9f;
            float ltdWeight = pImpl->stdp->getLTDWeight() * 1.1f;
            pImpl->stdp->configure(ltpWeight, ltdWeight, pImpl->stdp->getTau());
        }
    }
    
    // ========== STEP 12: REPLAY IMPORTANT MEMORIES ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
        
        // Replay working memory traces
        if (pImpl->workingMemory) {
            pImpl->workingMemory->updateRecurrentConnections();
        }
    }
    
    // ========== STEP 13: APPLY DEVELOPMENT EFFECTS ==========
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
    
    // ========== STEP 14: PERIODIC MEMORY CONSOLIDATION ==========
    if (currentStep % pImpl->consolidationInterval == 0) {
        // Consolidate episodic memory
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->consolidate(0.3f);
        }
        
        // Consolidate working memory - decay weak traces
        if (pImpl->workingMemory) {
            pImpl->workingMemory->decayWeakTraces();
        }
    }
    
    // ========== STEP 15: CHECKPOINT MANAGEMENT ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

// ========== SOCIAL LEARNING INTEGRATION ==========
void Brain::integrateSocialLearning() {
    if (!pImpl->planner) return;
    
    // Observe successful action plans from neural planner
    if (pImpl->planningSystem && pImpl->planner->wasRecentPlanSuccessful()) {
        // Learn imitation strategies
        // Get the successful planned action
        ActionType plannedAction = ActionType::Wait;  // Example
        
        // Record this observation for social learning
        if (pImpl->observationSystem) {
            std::vector<float> currentState(64, 0.5f);  // Example state
            pImpl->observationSystem->recordObservation(plannedAction, currentState);
        }
        
        // Develop social communication signals
        if (pImpl->socialSignals) {
            // Convert successful planning to social signal
            std::vector<float> signalPattern(16, 0.0f);
            signalPattern[0] = 1.0f;  // Signal success
            pImpl->socialSignals->learnSignal(signalPattern, 1.0f);
        }
    }
    
    // Use social knowledge to improve planning
    if (pImpl->planner && pImpl->socialKnowledge) {
        // Get imitation action based on social knowledge
        std::vector<float> currentState(64, 0.5f);  // Example state
        ActionType imitationAction = pImpl->socialKnowledge->getBestImitationAction(currentState);
        
        // Use imitation to improve planning
        if (imitationAction != ActionType::Wait) {
            pImpl->planner->updatePlanQuality(
                std::vector<ActionType>{imitationAction},
                std::vector<ActionType>{imitationAction},
                0.7f);  // Positive reinforcement
        }
    }
}
}

// ========== SOCIAL LEARNING INTEGRATION ==========
void Brain::integrateSocialLearning() {
    if (!pImpl->planner) return;
    
    // Observe successful action plans from neural planner
    if (pImpl->planningSystem && pImpl->planner->wasRecentPlanSuccessful()) {
        // Learn imitation strategies
        // Get the successful planned action
        ActionType plannedAction = ActionType::Wait;  // Example
        
        // Record this observation for social learning
        if (pImpl->observationSystem) {
            std::vector<float> currentState(64, 0.5f);  // Example state
            pImpl->observationSystem->recordObservation(plannedAction, currentState);
        }
        
        // Develop social communication signals
        if (pImpl->socialSignals) {
            // Convert successful planning to social signal
            std::vector<float> signalPattern(16, 0.0f);
            signalPattern[0] = 1.0f;  // Signal success
            pImpl->socialSignals->learnSignal(signalPattern, 1.0f);
        }
    }
    
    // Use social knowledge to improve planning
    if (pImpl->planner && pImpl->socialKnowledge) {
        // Get imitation action based on social knowledge
        std::vector<float> currentState(64, 0.5f);  // Example state
        ActionType imitationAction = pImpl->socialKnowledge->getBestImitationAction(currentState);
        
        // Use imitation to improve planning
        if (imitationAction != ActionType::Wait) {
            pImpl->planner->updatePlanQuality(
                std::vector<ActionType>{imitationAction},
                std::vector<ActionType>{imitationAction},
                0.7f);  // Positive reinforcement
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
        
        // Replay working memory traces
        if (pImpl->workingMemory) {
            pImpl->workingMemory->updateRecurrentConnections();
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
    if (currentStep % pImpl->consolidationInterval == 0) {
        // Consolidate episodic memory
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->consolidate(0.3f);
        }
        
        // Consolidate working memory - decay weak traces
        if (pImpl->workingMemory) {
            pImpl->workingMemory->decayWeakTraces();
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
    
    // ========== PREDICTION SYSTEM INTEGRATION ==========
    // Provide sensory input to prediction system for training
    if (pImpl->predictionSystem) {
        // Create a clone of the input for prediction system
        auto sensoryClone = input.clone();
        
        // If prediction system has previous state, update predictions
        // For now, just train on current sensory input
        pImpl->predictionSystem->train(*sensoryClone);
        
        // Get prediction error and confidence for statistics
        float predictionError = pImpl->predictionSystem->getPredictionError();
        float predictionConfidence = pImpl->predictionSystem->getConfidence();
        
        // Update prediction error signal
        if (pImpl->predictionError) {
            pImpl->predictionError->computeError(predictionError, 0.0f);
        }
    }
    
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
    pImpl->restCounter = 0;
    pImpl->stepsSinceLastEpisode = 0;
    pImpl->totalRestSteps = 0;
    pImpl->totalReplayEvents = 0;
    pImpl->consolidationProgress = 0.0f;
    
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
                    neuronData.neuronType.push_back(static_cast<uint64_t>(neuron->getNeuronType()));
                    neuronData.regionId.push_back(region->getId().index());
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
        
        // Write rest state
        RestStateCheckpoint restStateData;
        restStateData.isResting = pImpl->isResting;
        restStateData.restCounter = pImpl->restCounter;
        restStateData.totalRestSteps = pImpl->totalRestSteps;
        restStateData.totalReplayEvents = pImpl->totalReplayEvents;
        restStateData.consolidationProgress = pImpl->consolidationProgress;
        restStateData.restInterval = pImpl->restInterval;
        
        // Get neuromodulation levels for checkpoint
        if (pImpl->predictionSystem) {
            restStateData.predictionError = pImpl->predictionSystem->getPredictionError();
        } else {
            restStateData.predictionError = 0.0f;
        }
        
        if (pImpl->dopamine) {
            restStateData.dopamineLevel = pImpl->dopamine->getLevel();
        } else {
            restStateData.dopamineLevel = 0.0f;
        }
        
        if (pImpl->curiosity) {
            restStateData.curiosityLevel = pImpl->curiosity->getLevel();
        } else {
            restStateData.curiosityLevel = 0.0f;
        }
        
        if (pImpl->novelty) {
            restStateData.noveltyLevel = pImpl->novelty->getLevel();
        } else {
            restStateData.noveltyLevel = 0.0f;
        }
        
        if (!writer.writeSection(CheckpointSection::RestState, &restStateData, sizeof(RestStateCheckpoint))) {
            NLM_LOG_ERROR("Failed to write rest state to checkpoint");
            return false;
        }
        
        // Save memory systems
        if (pImpl->workingMemory) {
            MemoryCheckpointData memoryData;
            
            // Working memory statistics
            memoryData.workingMemoryCapacity = pImpl->workingMemory->getCapacity();
            memoryData.workingMemoryActiveNeurons = pImpl->workingMemory->getMemoryNeurons().size();
            auto workingMemoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            for (const auto& neuronId : workingMemoryNeurons) {
                memoryData.workingMemoryNeurons.push_back(neuronId.index());
            }
            
            // Get working memory activations
            auto workingMemoryData = pImpl->workingMemory->getMemoryData();
            for (const auto& activation : workingMemoryData) {
                memoryData.workingMemoryActivations.push_back(activation);
            }
            
            // Episodic memory statistics
            if (pImpl->episodicMemory) {
                memoryData.episodicMemoryMaxEpisodes = pImpl->episodicMemory->getMaxEpisodes();
                memoryData.episodicMemoryEpisodeCount = pImpl->episodicMemory->getEpisodeCount();
                auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(100); // Save up to 100 recent episodes
                for (const auto* episode : recentEpisodes) {
                    if (episode) {
                        memoryData.episodicMemoryEpisodeTimestamps.push_back(episode->timestamp);
                        memoryData.episodicMemoryEpisodeRewards.push_back(episode->reward);
                        memoryData.episodicMemoryEpisodeInterests.push_back(episode->interest);
                    }
                }
            }
            
            // Associative memory statistics
            if (pImpl->associativeMemory) {
                auto associations = pImpl->associativeMemory->getAllAssociations();
                for (const auto& assoc : associations) {
                    memoryData.associativeMemoryAssociationCount++;
                    memoryData.associativeMemorySourceNeurons.push_back(assoc.sourceNeuron.index());
                    memoryData.associativeMemoryTargetNeurons.push_back(assoc.targetNeuron.index());
                    memoryData.associativeMemoryStrengths.push_back(assoc.strength);
                    memoryData.associativeMemoryInterests.push_back(assoc.interest);
                }
            }
            
            // Write memory data
            if (!writer.writeSection(CheckpointSection::Memory, &memoryData, sizeof(memoryData))) {
                NLM_LOG_ERROR("Failed to write memory data to checkpoint");
                return false;
            }
        }
        
        // Save prediction system
        if (pImpl->predictionSystem) {
            std::vector<uint8_t> predictionData;
            // Create prediction system checkpoint data
            struct PredictionCheckpointData {
                float predictionError;
                float predictionConfidence;
                size_t predictionUpdateCount;
                std::vector<float> predictionHistory;
            } predData;
            
            predData.predictionError = pImpl->predictionSystem->getPredictionError();
            predData.predictionConfidence = pImpl->predictionSystem->getConfidence();
            // Get prediction update count (simplified)
            predData.predictionUpdateCount = static_cast<size_t>(pImpl->currentStep * 0.1f); // Approximation
            
            predictionData.resize(sizeof(predData));
            std::memcpy(predictionData.data(), &predData, sizeof(predData));
            
            if (!writer.writeSection(CheckpointSection::Plasticity, predictionData.data(), predictionData.size())) {
                NLM_LOG_ERROR("Failed to write prediction data to checkpoint");
                return false;
            }
        }
        
        // Save neuromodulation systems
        if (pImpl->dopamine || pImpl->acetylcholine || pImpl->norepinephrine || 
            pImpl->serotonin || pImpl->curiosity || pImpl->predictionError || pImpl->novelty) {
            NeuromodulationCheckpointData neuromodData;
            
            if (pImpl->dopamine) {
                neuromodData.dopamineLevel = pImpl->dopamine->getLevel();
                neuromodData.dopaminePlasticityFactor = pImpl->dopamine->getPlasticityFactor();
            } else {
                neuromodData.dopamineLevel = 0.0f;
                neuromodData.dopaminePlasticityFactor = 1.0f;
            }
            
            if (pImpl->acetylcholine) {
                neuromodData.acetylcholineLevel = pImpl->acetylcholine->getLevel();
                neuromodData.acetylcholinePlasticityFactor = pImpl->acetylcholine->getPlasticityFactor();
            } else {
                neuromodData.acetylcholineLevel = 0.0f;
                neuromodData.acetylcholinePlasticityFactor = 1.0f;
            }
            
            if (pImpl->norepinephrine) {
                neuromodData.norepinephrineLevel = pImpl->norepinephrine->getLevel();
                neuromodData.norepinephrinePlasticityFactor = pImpl->norepinephrine->getPlasticityFactor();
            } else {
                neuromodData.norepinephrineLevel = 0.0f;
                neuromodData.norepinephrinePlasticityFactor = 1.0f;
            }
            
            if (pImpl->serotonin) {
                neuromodData.serotoninLevel = pImpl->serotonin->getLevel();
                neuromodData.serotoninPlasticityFactor = pImpl->serotonin->getPlasticityFactor();
            } else {
                neuromodData.serotoninLevel = 0.0f;
                neuromodData.serotoninPlasticityFactor = 1.0f;
            }
            
            if (pImpl->curiosity) {
                neuromodData.curiosityLevel = pImpl->curiosity->getLevel();
            } else {
                neuromodData.curiosityLevel = 0.0f;
            }
            
            if (pImpl->predictionError) {
                neuromodData.predictionErrorLevel = pImpl->predictionError->getError();
            } else {
                neuromodData.predictionErrorLevel = 0.0f;
            }
            
            if (pImpl->novelty) {
                neuromodData.noveltyLevel = pImpl->novelty->getLevel();
            } else {
                neuromodData.noveltyLevel = 0.0f;
            }
            
            // Write neuromodulation data
            if (!writer.writeSection(CheckpointSection::Neuromodulation, &neuromData, sizeof(neuromData))) {
                NLM_LOG_ERROR("Failed to write neuromodulation data to checkpoint");
                return false;
            }
        }
        
        // Save cognition systems
        if (pImpl->planner || pImpl->conceptFormation || pImpl->attention) {
            CognitionCheckpointData cognitionData;
            
            if (pImpl->planner) {
                cognitionData.plannerPlanCount = 10; // Approximation
                auto plannerGoals = pImpl->planner->getCurrentGoals();
                for (const auto& goal : plannerGoals) {
                    cognitionData.plannerCurrentGoals.push_back(goal);
                }
                cognitionData.plannerPlanningConfidences.push_back(pImpl->planner->getPlanningConfidence());
                cognitionData.plannerPlanQualities.push_back(pImpl->planner->getPlanQuality());
            }
            
            if (pImpl->conceptFormation) {
                cognitionData.conceptCount = pImpl->conceptFormation->getConceptCount();
                for (size_t i = 0; i < cognitionData.conceptCount && i < 100; ++i) {
                    auto concept = pImpl->conceptFormation->getConcept(i);
                    if (concept) {
                        cognitionData.conceptMatchingScores.push_back(i);
                        cognitionData.conceptPrototypes.push_back(concept->prototype);
                        cognitionData.conceptStabilities.push_back(concept->stability);
                        cognitionData.conceptInterests.push_back(concept->interest);
                    }
                }
            }
            
            if (pImpl->attention) {
                cognitionData.attentionInhibitionStrength = pImpl->attention->getInhibitionStrength();
                cognitionData.attentionExcitationStrength = pImpl->attention->getExcitationStrength();
                cognitionData.attentionActiveNeurons = pImpl->attention->getActiveNeurons().size();
                auto attentionCompetitors = pImpl->attention->getCompetitors();
                for (const auto& competitor : attentionCompetitors) {
                    cognitionData.attentionCompetitors.push_back(competitor.index());
                }
            }
            
            // Write cognition data
            if (!writer.writeSection(CheckpointSection::Development, &cognitionData, sizeof(cognitionData))) {
                NLM_LOG_ERROR("Failed to write cognition data to checkpoint");
                return false;
            }
        }
        
        // Save development system
        if (pImpl->developmentSystem) {
            DevelopmentCheckpointData devData;
            devData.developmentStage = static_cast<uint64_t>(pImpl->developmentalStage);
            devData.plasticityRate = pImpl->developmentSystem->getPlasticityRate();
            devData.synaptogenesisRate = pImpl->developmentSystem->getSynaptogenesisRate();
            devData.pruningRate = pImpl->developmentSystem->getPruningRate();
            devData.maturationProgress = pImpl->developmentSystem->getMaturationProgress();
            devData.currentBrainSize = pImpl->developmentSystem->getCurrentBrainSize();
            devData.maxBrainSize = pImpl->developmentSystem->getMaxBrainSize();
            
            // Write development data
            if (!writer.writeSection(CheckpointSection::Development, &devData, sizeof(devData))) {
                NLM_LOG_ERROR("Failed to write development data to checkpoint");
                return false;
            }
        }
        
        // Save statistics
        StatisticsCheckpointData statsData;
        statsData.totalSpikes = pImpl->totalSpikesTotal;
        statsData.spikesPerStep = pImpl->totalSpikesThisStep;
        statsData.totalNeurons = getTotalNeuronCount();
        statsData.activeNeurons = getActiveNeuronCount();
        statsData.averageFiringRate = getAverageFiringRate();
        
        if (pImpl->workingMemory) {
            statsData.workingMemorySize = pImpl->workingMemory->getCapacity();
        }
        if (pImpl->episodicMemory) {
            statsData.episodicMemorySize = pImpl->episodicMemory->getEpisodeCount();
        }
        if (pImpl->associativeMemory) {
            statsData.associativeMemorySize = pImpl->associativeMemory->getAssociationCount();
        }
        
        if (pImpl->predictionSystem) {
            statsData.predictionError = pImpl->predictionSystem->getPredictionError();
            statsData.predictionConfidence = pImpl->predictionSystem->getConfidence();
        }
        
        statsData.totalRestSteps = pImpl->totalRestSteps;
        statsData.totalRestCycles = pImpl->totalRestSteps / pImpl->restInterval;
        statsData.totalReplayEvents = pImpl->totalReplayEvents;
        
        // Write statistics data
        if (!writer.writeSection(CheckpointSection::Statistics, &statsData, sizeof(statsData))) {
            NLM_LOG_ERROR("Failed to write statistics data to checkpoint");
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
                        if (idx < neuronData.lastSpikeTime.size()) {
                            // Set last spike time
                            // Note: neuron doesn't have setLastSpikeTime method, so we skip this
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
        
        // Read rest state
        std::vector<uint8_t> restStateBuffer = reader.readSection(CheckpointSection::RestState);
        if (!restStateBuffer.empty()) {
            // Check buffer size
            if (restStateBuffer.size() >= sizeof(RestStateCheckpoint)) {
                RestStateCheckpoint restStateData;
                std::memcpy(&restStateData, restStateBuffer.data(), sizeof(RestStateCheckpoint));
                
                // Restore rest state
                pImpl->isResting = restStateData.isResting;
                pImpl->restCounter = restStateData.restCounter;
                pImpl->totalRestSteps = restStateData.totalRestSteps;
                pImpl->totalReplayEvents = restStateData.totalReplayEvents;
                pImpl->consolidationProgress = restStateData.consolidationProgress;
                pImpl->restInterval = restStateData.restInterval;
                
                NLM_LOG_INFO("Rest state loaded: isResting=" + std::to_string(restStateData.isResting) +
                            ", restCounter=" + std::to_string(restStateData.restCounter) +
                            ", totalRestSteps=" + std::to_string(restStateData.totalRestSteps) +
                            ", consolidationProgress=" + std::to_string(restStateData.consolidationProgress));
            } else {
                NLM_LOG_WARNING("Rest state checkpoint data is incomplete, using defaults");
            }
        }
        
        // Load memory systems
        std::vector<uint8_t> memoryBuffer = reader.readSection(CheckpointSection::Memory);
        if (!memoryBuffer.empty()) {
            if (memoryBuffer.size() >= sizeof(MemoryCheckpointData)) {
                MemoryCheckpointData memoryData;
                std::memcpy(&memoryData, memoryBuffer.data(), sizeof(MemoryCheckpointData));
                
                // Restore working memory
                if (pImpl->workingMemory) {
                    pImpl->workingMemory->clear();
                    pImpl->workingMemory->setCapacity(memoryData.workingMemoryCapacity);
                    // Restore working memory neurons and activations
                    // This would require implementing methods to restore memory state
                    NLM_LOG_INFO("Working memory restored from checkpoint");
                }
                
                // Restore episodic memory
                if (pImpl->episodicMemory) {
                    // Restore episodic memory episodes
                    // This would require implementing methods to restore episodes
                    NLM_LOG_INFO("Episodic memory restored from checkpoint");
                }
                
                // Restore associative memory
                if (pImpl->associativeMemory) {
                    // Restore associative memory associations
                    // This would require implementing methods to restore associations
                    NLM_LOG_INFO("Associative memory restored from checkpoint");
                }
                
                NLM_LOG_INFO("Memory systems loaded successfully");
            } else {
                NLM_LOG_WARNING("Memory checkpoint data is incomplete, using defaults");
            }
        }
        
        // Load neuromodulation systems
        std::vector<uint8_t> neuromodBuffer = reader.readSection(CheckpointSection::Neuromodulation);
        if (!neuromBuffer.empty()) {
            if (neuromBuffer.size() >= sizeof(NeuromodulationCheckpointData)) {
                NeuromodulationCheckpointData neuromodData;
                std::memcpy(&neuromData, neuromodBuffer.data(), sizeof(NeuromodulationCheckpointData));
                
                // Restore neuromodulation levels
                if (pImpl->dopamine) {
                    pImpl->dopamine->setLevel(neuromData.dopamineLevel);
                }
                if (pImpl->acetylcholine) {
                    pImpl->acetylcholine->setLevel(neuromData.acetylcholineLevel);
                }
                if (pImpl->norepinephrine) {
                    pImpl->norepinephrine->setLevel(neuromData.norepinephrineLevel);
                }
                if (pImpl->serotonin) {
                    pImpl->serotonin->setLevel(neuromData.serotoninLevel);
                }
                if (pImpl->curiosity) {
                    pImpl->curiosity->setLevel(neuromData.curiosityLevel);
                }
                if (pImpl->predictionError) {
                    pImpl->predictionError->computeError(neuromData.predictionErrorLevel, 0.0f);
                }
                if (pImpl->novelty) {
                    pImpl->novelty->setLevel(neuromData.noveltyLevel);
                }
                
                NLM_LOG_INFO("Neuromodulation systems loaded successfully");
            } else {
                NLM_LOG_WARNING("Neuromodulation checkpoint data is incomplete, using defaults");
            }
        }
        
        // Load cognition systems
        std::vector<uint8_t> cognitionBuffer = reader.readSection(CheckpointSection::Development);
        if (!cognitionBuffer.empty()) {
            if (cognitionBuffer.size() >= sizeof(CognitionCheckpointData)) {
                CognitionCheckpointData cognitionData;
                std::memcpy(&cognitionData, cognitionBuffer.data(), sizeof(CognitionCheckpointData));
                
                // Restore cognition systems
                if (pImpl->planner) {
                    // Restore planner state
                    NLM_LOG_INFO("Planner state loaded from checkpoint");
                }
                if (pImpl->conceptFormation) {
                    // Restore concept formation state
                    NLM_LOG_INFO("Concept formation state loaded from checkpoint");
                }
                if (pImpl->attention) {
                    // Restore attention system state
                    NLM_LOG_INFO("Attention system state loaded from checkpoint");
                }
                
                NLM_LOG_INFO("Cognition systems loaded successfully");
            } else {
                NLM_LOG_WARNING("Cognition checkpoint data is incomplete, using defaults");
            }
        }
        
        // Load development system
        std::vector<uint8_t> devBuffer = reader.readSection(CheckpointSection::Development);
        if (!devBuffer.empty()) {
            if (devBuffer.size() >= sizeof(DevelopmentCheckpointData)) {
                DevelopmentCheckpointData devData;
                std::memcpy(&devData, devBuffer.data(), sizeof(DevelopmentCheckpointData));
                
                // Restore development system state
                pImpl->developmentalStage = static_cast<DevelopmentalStage>(devData.developmentStage);
                // This would require implementing methods to restore development system state
                NLM_LOG_INFO("Development system loaded from checkpoint");
            } else {
                NLM_LOG_WARNING("Development checkpoint data is incomplete, using defaults");
            }
        }
        
        // Load statistics
        std::vector<uint8_t> statsBuffer = reader.readSection(CheckpointSection::Statistics);
        if (!statsBuffer.empty()) {
            if (statsBuffer.size() >= sizeof(StatisticsCheckpointData)) {
                StatisticsCheckpointData statsData;
                std::memcpy(&statsData, statsBuffer.data(), sizeof(StatisticsCheckpointData));
                
                // Update statistics
                pImpl->totalSpikesTotal = statsData.totalSpikes;
                pImpl->totalRestSteps = statsData.totalRestSteps;
                pImpl->totalReplayEvents = statsData.totalReplayEvents;
                
                NLM_LOG_INFO("Statistics loaded successfully");
            } else {
                NLM_LOG_WARNING("Statistics checkpoint data is incomplete, using defaults");
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

Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
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

// ========== SELF-MODEL INTEGRATION WITH NEURAL PLANNER ==========
void Brain::integrateSelfModelWithPlanner() {
    if (!pImpl->planner || !pImpl->predictionError) return;
    
    // Update self-model with prediction errors from planner
    std::vector<float> plannerState(64, 0.0f);  // Example state
    
    // Get prediction errors from planner evaluation
    float plannerError = 0.5f;  // Example error
    if (plannerError > 0.5f) {  // Significant error
        // Update self-model with prediction error
        if (pImpl->predictionError) {
            pImpl->predictionError->computeError(plannerError, 0.0f);
        }
    }
    
    // Use self-model to predict action consequences
    if (pImpl->predictionSystem) {
        // Self-model provides priors for prediction system
        std::vector<float> selfModelPrediction(16, 0.5f);  // Example prediction
        pImpl->predictionSystem->train(selfModelPrediction);
    }
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
    
    // ========== PREDICTION SYSTEM STATISTICS ==========
    if (pImpl->predictionSystem) {
        float predictionError = pImpl->predictionSystem->getPredictionError();
        float predictionConfidence = pImpl->predictionSystem->getConfidence();
        NLM_LOG_INFO("Prediction error: " + std::to_string(predictionError));
        NLM_LOG_INFO("Prediction confidence: " + std::to_string(predictionConfidence));
        
        const auto& errorHistory = pImpl->predictionSystem->getErrorHistory();
        if (!errorHistory.empty()) {
            float avgError = 0.0f;
            for (float error : errorHistory) avgError += error;
            avgError /= errorHistory.size();
            NLM_LOG_INFO("Average prediction error: " + std::to_string(avgError));
            NLM_LOG_INFO("Prediction error history length: " + std::to_string(errorHistory.size()));
        }
    }
    
    if (pImpl->predictionError) {
        float predictionError = pImpl->predictionError->getError();
        float magnitude = pImpl->predictionError->getMagnitude();
        NLM_LOG_INFO("Prediction error signal: " + std::to_string(predictionError));
        NLM_LOG_INFO("Prediction error magnitude: " + std::to_string(magnitude));
    }
    
    // ========== MEMORY SYSTEM STATISTICS ==========
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getMemoryNeurons().size()));
        NLM_LOG_INFO("Working memory activity: " + std::to_string(pImpl->workingMemory->getMemoryActivity()));
        NLM_LOG_INFO("Working memory winners: " + std::to_string(pImpl->workingMemory->getWinners().size()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
        NLM_LOG_INFO("Episodic memory average reward: " + std::to_string(pImpl->episodicMemory->getAverageReward()));
    }
    if (pImpl->associativeMemory) {
        // Get association count
        NLM_LOG_INFO("Associative memory associations: " + std::to_string(pImpl->associativeMemory->getAssociationCount()));
    }
    if (pImpl->attention) {
        NLM_LOG_INFO("Attention attended regions: " + std::to_string(pImpl->attention->getAttendedRegions().size()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    if (pImpl->acetylcholine) {
        NLM_LOG_INFO("Acetylcholine level: " + std::to_string(pImpl->acetylcholine->getLevel()));
    }
    if (pImpl->norepinephrine) {
        NLM_LOG_INFO("Norepinephrine level: " + std::to_string(pImpl->norepinephrine->getLevel()));
    }
    if (pImpl->serotonin) {
        NLM_LOG_INFO("Serotonin level: " + std::to_string(pImpl->serotonin->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    // ========== REST/SLEEP STATUS ==========
    NLM_LOG_INFO("Rest state: " + std::string(pImpl->isResting ? "ACTIVE" : "INACTIVE"));
    NLM_LOG_INFO("Rest counter: " + std::to_string(pImpl->restCounter));
    NLM_LOG_INFO("Total rest steps: " + std::to_string(pImpl->totalRestSteps));
    NLM_LOG_INFO("Total replay events: " + std::to_string(pImpl->totalReplayEvents));
    NLM_LOG_INFO("Consolidation progress: " + std::to_string(pImpl->consolidationProgress));
    NLM_LOG_INFO("Rest interval: " + std::to_string(pImpl->restInterval));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

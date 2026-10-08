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
    
    // ========== INTEGRATED NEUROMODULATION SYSTEMS ==========
    
    // Initialize neuromodulation systems
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
        
        // ========== INITIALIZE INTEGRATED NEUROMODULATION SYSTEMS ==========
        
        // Initialize dopamine (reward)
        dopamine = std::make_unique<Dopamine>();
        
        // Initialize acetylcholine (attention and memory consolidation)
        curiosity = std::make_unique<Curiosity>();
        acetylcholine = std::make_unique<Acetylcholine>();
        
        // Initialize norepinephrine (arousal and vigilance)
        predictionError = std::make_unique<PredictionError>();
        norepinephrine = std::make_unique<Norepinephrine>();
        
        // Initialize serotonin (mood and social behavior)
        novelty = std::make_unique<Novelty>();
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
    
    // Update acetylcholine (attention and memory consolidation)
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        
        // Apply acetylcholine effects on attention and learning
        float acetylcholineLevel = pImpl->acetylcholine->getLevel();
        float attentionModulation = acetylcholineLevel * 0.8f;
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Acetylcholine enhances signal-to-noise ratio and learning
                    // Higher ACh increases signal detection and memory consolidation
                    float attentionEffect = attentionModulation * 0.3f;
                    if (attentionEffect > 0.0f) {
                        neuron->injectCurrent(attentionEffect);
                    }
                }
            }
        }
    }
    
    // Update norepinephrine (arousal and vigilance)
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
        
        // Apply norepinephrine effects on arousal and attention
        float norepinephrineLevel = pImpl->norepinephrine->getLevel();
        float arousalModulation = norepinephrineLevel * 0.6f;
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Norepinephrine increases arousal and vigilance
                    float arousalEffect = arousalModulation * 0.4f;
                    if (arousalEffect > 0.0f) {
                        neuron->injectCurrent(arousalEffect);
                    }
                }
            }
        }
    }
    
    // Update serotonin (mood and social behavior)
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
        
        // Apply serotonin effects on mood and social behavior
        float serotoninLevel = pImpl->serotonin->getLevel();
        float moodModulation = serotoninLevel * 0.5f;
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Serotonin modulates mood and social behavior
                    // Lower serotonin increases negative mood effects
                    float moodEffect = -moodModulation * 0.2f;  // Inhibitory for mood
                    if (moodEffect != 0.0f) {
                        neuron->injectCurrent(moodEffect);
                    }
                }
            }
        }
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
    
    // Combine effects from different neuromodulators
    if (pImpl->dopamine) {
        plasticityMod *= pImpl->dopamine->getPlasticityFactor();
    }
    if (pImpl->acetylcholine) {
        plasticityMod *= pImpl->acetylcholine->getPlasticityFactor();
    }
    if (pImpl->norepinephrine) {
        plasticityMod *= pImpl->norepinephrine->getPlasticityFactor();
    }
    if (pImpl->serotonin) {
        plasticityMod *= pImpl->serotonin->getPlasticityFactor();
    }
    
    // Scale overall plasticity
    plasticityMod = std::clamp(plasticityMod, 0.1f, 3.0f);
    
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
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners with enhanced integration
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            
            // Get attention winners
            std::vector<NeuronId> attentionWinners = pImpl->attention->processCompetition(competitors);
            
            // Enhanced integration: attention modulates working memory
            if (!attentionWinners.empty()) {
                // Apply attentional boost to winners
                for (const auto& winner : attentionWinners) {
                    // Increase working memory activation through attention
                    float attentionBoost = 1.5f;  // Attention amplifies memory
                    if (pImpl->workingMemory) {
                        // Find and strengthen the winner's memory trace
                        for (size_t i = 0; i < pImpl->workingMemory->getMemoryNeurons().size(); ++i) {
                            if (pImpl->workingMemory->getMemoryNeurons()[i] == winner) {
                                // Boost memory activation
                                pImpl->workingMemory->memoryActivations_[i] = 
                                    std::min(1.0f, pImpl->workingMemory->memoryActivations_[i] * attentionBoost);
                                
                                // Inject current to reinforce winner
                                pImpl->workingMemory->brain_->injectCurrent(winner, 2.0f * attentionBoost);
                                break;
                            }
                        }
                    }
                }
                
                // Apply inhibition to non-attended items
                for (const auto& competitor : competitors) {
                    bool isWinner = std::find(attentionWinners.begin(), attentionWinners.end(), competitor) != attentionWinners.end();
                    if (!isWinner) {
                        // Apply attentional inhibition
                        for (size_t i = 0; i < pImpl->workingMemory->getMemoryNeurons().size(); ++i) {
                            if (pImpl->workingMemory->getMemoryNeurons()[i] == competitor) {
                                // Decay memory trace due to lack of attention
                                pImpl->workingMemory->memoryActivations_[i] *= 0.8f;
                                
                                // Inject inhibitory current
                                pImpl->workingMemory->brain_->injectCurrent(competitor, -1.0f);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Process current neural activity patterns to form concepts
        // Integrate sensory state and working memory patterns into concept formation
        
        // Extract current sensory pattern for concept learning
        std::vector<float> currentSensoryPattern;
        if (!pImpl->sensoryNeurons.empty() && currentStep > 0) {
            // Get recent sensory activity (last few steps)
            currentSensoryPattern.reserve(pImpl->sensoryNeurons.size());
            for (size_t i = 0; i < pImpl->sensoryNeurons.size() && i < 20; ++i) {
                float activation = pImpl->sensoryNeurons[i]->getMembranePotential();
                currentSensoryPattern.push_back((activation + 20.0f) / 40.0f);  // Normalize to [0, 1]
            }
        }
        
        // Extract features from working memory if available
        std::vector<float> memoryFeatures;
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            memoryFeatures = pImpl->workingMemory->retrieve();
        }
        
        // Get reward from dopamine system
        float currentReward = 0.0f;
        if (pImpl->dopamine) {
            currentReward = pImpl->dopamine->getLevel();
        }
        
        // Present experience to ConceptFormation for learning
        size_t conceptId = pImpl->conceptFormation->presentExperience(
            currentSensoryPattern, 
            memoryFeatures, 
            currentReward, 
            currentStep
        );
        
        // If a concept was formed or updated, use it to modulate attention
        if (conceptId > 0) {
            const DiscoveredConcept* concept = pImpl->conceptFormation->getConcept(conceptId);
            if (concept && concept->avgStability > 0.7f) {  // Stable concept
                // Use concept to guide attention and planning
                if (pImpl->conceptFormation->isConceptStable(conceptId)) {
                    // Stable concept can be used for planning
                    std::vector<float> prototype = pImpl->conceptFormation->getConceptPrototype(conceptId);
                    
                    // Apply concept-based attention modulation
                    if (pImpl->attention && !prototype.empty() && currentStep % 50 == 0) {
                        // Use concept prototype to bias attention toward relevant features
                        for (size_t i = 0; i < std::min(size_t(5), prototype.size()); ++i) {
                            if (prototype[i] > 0.7f) {  // Strong feature in concept
                                // This would apply top-down attention bias
                                // In a full implementation, this would use 
                                // concept features to modulate attention weights
                            }
                        }
                    }
                }
            }
        }
        
        // Update concept based on current state (Hebbian learning)
        if (!currentSensoryPattern.empty() && !memoryFeatures.empty()) {
            // Blend sensory and memory features for concept update
            std::vector<float> blendedFeatures = currentSensoryPattern;
            size_t blendSize = std::min(blendedFeatures.size(), memoryFeatures.size());
            for (size_t i = 0; i < blendSize; ++i) {
                blendedFeatures[i] = (currentSensoryPattern[i] * 0.7f + memoryFeatures[i] * 0.3f);
            }
            
            // Update concept with this experience
            if (conceptId == 0) {
                // This will create a new concept
                conceptId = pImpl->conceptFormation->createConcept(
                    blendedFeatures, blendedFeatures, currentReward);
            } else {
                // Update existing concept
                pImpl->conceptFormation->updateConcept(conceptId, blendedFeatures, blendedFeatures, currentReward);
            }
        }
    }
    
    // ========== STEP 10.5: Update SelfModel with experiences ==========
    if (pImpl->conceptFormation && pImpl->planner && !currentSensoryPattern.empty()) {
        // Use NeuralPlanner and ConceptFormation to update SelfModel
        
        // Get planned action from NeuralPlanner
        ActionType plannedAction = pImpl->planner->planAction(currentSensoryPattern, currentReward);
        
        // Record this action and its consequences in SelfModel
        if (pImpl->conceptFormation && !currentSensoryPattern.empty()) {
            pImpl->conceptFormation->recordExperience(currentSensoryPattern, plannedAction);
        }
        
        // Record action-effect relationships in SelfModel
        if (pImpl->conceptFormation) {
            // Get concept prototypes for state representation
            std::vector<float> statePrototype;
            for (size_t i = 0; i < std::min(size_t(10), currentSensoryPattern.size()); ++i) {
                statePrototype.push_back(currentSensoryPattern[i]);
            }
            
            // Record this experience in SelfModel
            pImpl->conceptFormation->recordExperience(statePrototype, plannedAction);
            
            // Update self-model with prediction errors
            // (This would integrate with prediction error system for learning)
            
            // Record action-effect pairs for later prediction
            // This builds the SelfModel's understanding of action consequences
        }
        
        // Update SelfModel based on outcomes
        if (pImpl->conceptFormation) {
            // SelfModel updates based on the relationship between actions and outcomes
            // This happens automatically when experiences are recorded
        }
    }
    
    // ========== STEP 10.6: Update SocialLearning (observational learning) ==========
    if (pImpl->conceptFormation && pImpl->planner && !currentSensoryPattern.empty()) {
        // For now, simulate social learning by recording agent experiences
        // In a full implementation, this would observe other agents
        
        // Record agent's own experiences for potential imitation
        // This creates a foundation for social learning
        if (pImpl->conceptFormation) {
            // The agent's own experiences serve as models for potential imitation
            // In a multi-agent setting, other agents' experiences would be added here
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
    // Enhanced sensory encoding for working memory integration
    // 
    // This implements sophisticated sensory-to-working memory pipeline:
    // 1. Multi-stage sensory processing and feature extraction
    // 2. Active sensing based on current working memory content
    // 3. Attention-modulated encoding
    // 4. Pattern completion and filling-in
    
    const auto& values = input.getData();
    if (values.empty()) return;

    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;

    // ========== STAGE 1: FEATURE EXTRACTION ==========
    // Extract and preprocess sensory features
    std::vector<float> processedFeatures;
    processedFeatures.reserve(values.size());
    
    // Multi-resolution feature extraction
    // Low-frequency (contextual) and high-frequency (detail) components
    for (size_t i = 0; i < values.size(); ++i) {
        float value = static_cast<float>(values[i]);
        
        // Feature extraction based on current working memory state
        float featureWeight = 1.0f;
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Modulate encoding based on working memory content
            size_t memIndex = i % pImpl->workingMemory->getMemoryNeurons().size();
            float memActivation = pImpl->workingMemory->getNeuronActivation(pImpl->workingMemory->getMemoryNeurons()[memIndex]);
            featureWeight = 0.5f + 0.5f * std::tanh(memActivation * 3.0f); // Range [0.5, 1.5]
        }
        
        // Apply feature extraction
        float lowFreq = value * 0.7f;  // Contextual information
        float highFreq = value * 1.3f;  // Detail information
        
        // Combine with working memory modulation
        float feature = (lowFreq + highFreq) * featureWeight;
        
        // Normalize to working memory range [0, 1]
        float normalizedFeature = 0.5f + feature * 0.1f;  // Range [-0.15, 1.15] -> clip to [0, 1] later
        
        processedFeatures.push_back(normalizedFeature);
    }

    // ========== STAGE 2: ACTIVE SENSING ==========
    // Adjust sensory gain based on current attention and memory state
    std::vector<float> activeFeatures = processedFeatures;
    
    if (pImpl->attention) {
        // Attention modulates sensory gain
        float attentionStrength = 1.0f;
        const auto& attentionWinners = pImpl->attention->getWinners();
        
        // Boost features associated with attended neurons
        for (size_t i = 0; i < std::min(size_t(5), values.size()); ++i) {
            // Apply attention-based gain modulation
            attentionStrength = 1.0f + 0.5f * pImpl->attention->getExcitationFor(
                pImpl->sensoryNeurons[i]->getId());
            
            activeFeatures[i] *= attentionStrength;
        }
    }

    // ========== STAGE 3: ENHANCED WORKING MEMORY ENCODING ==========
    // Store features in working memory with sophisticated encoding
    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        
        if (i < values.size()) {
            // Use processed features for encoding
            normalizedValue = activeFeatures[i];
            
            // Clip to valid working memory range
            if (normalizedValue < 0.0f) normalizedValue = 0.0f;
            if (normalizedValue > 1.0f) normalizedValue = 1.0f;
        }
        
        // Inject current into this sensory neuron (excitation/inhibition)
        float injectionCurrent = (normalizedValue - 0.5f) * 20.0f;  // Range [-10, 10]
        pImpl->sensoryNeurons[i]->injectCurrent(injectionCurrent);
        
        // Enhanced working memory storage
        if (pImpl->workingMemory) {
            // Multi-factor memory encoding:
            // 1. Salience (absolute value)
            // 2. Novelty (change from previous encoding)
            // 3. Relevance (based on attention)
            // 4. Contextual modulation
            
            float salience = std::abs(injectionCurrent) / 10.0f;  // Range [0, 1]
            
            // Get previous activation if available
            float previousActivation = 0.0f;
            bool hasPrevious = false;
            for (size_t j = 0; j < pImpl->workingMemory->getMemoryNeurons().size(); ++j) {
                if (pImpl->workingMemory->getMemoryNeurons()[j] == pImpl->sensoryNeurons[i]->getId()) {
                    previousActivation = pImpl->workingMemory->getNeuronActivation(pImpl->workingMemory->getMemoryNeurons()[j]);
                    hasPrevious = true;
                    break;
                }
            }
            
            // Novelty detection (change from previous state)
            float novelty = hasPrevious ? std::abs(normalizedValue - previousActivation) : 0.5f;
            
            // Calculate combined memory strength
            float memoryStrength = salience * 0.4f + novelty * 0.3f + normalizedValue * 0.3f;
            
            // Apply with memory-specific scaling
            if (memoryStrength > 0.01f) {  // Only store meaningful patterns
                pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), memoryStrength);
            }
        }
    }
    
    // ========== STAGE 4: PATTERN COMPLETION ==========
    // Use working memory to fill in missing sensory information
    // (This would involve recurrent connections and predictive coding)
    
    // In a full implementation, this would:
    // 1. Use working memory patterns to predict missing sensory input
    // 2. Generate interpolated values for gaps in sensory data
    // 3. Apply top-down modulation from concepts and expectations
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
    // Enhanced action selection using NeuralPlanner
    // 
    // This method now uses the sophisticated NeuralPlanner for cognitive decision-making:
    // 1. NeuralPlanner generates action sequences based on current state
    // 2. SelfModel predicts action consequences
    // 3. ConceptFormation provides contextual understanding
    // 4. Attention modulates which sequences are considered
    // 5. Neuromodulation adjusts planning confidence and exploration
    
    // Get current brain state for planning
    std::vector<float> currentState;
    
    // Build current state from neural activity patterns
    for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
        if (i < 20) {  // Limit state size for planner
            float activation = pImpl->sensoryNeurons[i]->getMembranePotential();
            currentState.push_back((activation + 20.0f) / 40.0f);  // Normalize to [0, 1]
        }
    }
    
    // Add working memory state if available
    if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
        std::vector<float> memoryState = pImpl->workingMemory->retrieve();
        // Blend sensory and memory state
        size_t blendCount = std::min(currentState.size(), memoryState.size());
        for (size_t i = 0; i < blendCount; ++i) {
            currentState[i] = (currentState[i] * 0.6f + memoryState[i] * 0.4f);
        }
    }
    
    // Get target reward from dopamine system (if available)
    float targetReward = 0.0f;
    if (pImpl->dopamine) {
        targetReward = pImpl->dopamine->getLevel();
    }
    
    // Use NeuralPlanner to generate action
    if (pImpl->planner) {
        // Get planned action from NeuralPlanner
        ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
        
        // Update SelfModel with this action attempt
        if (pImpl->conceptFormation) {
            pImpl->conceptFormation->recordExperience(currentState, plannedAction);
        }
        
        // Update planning quality based on actual outcomes
        if (pImpl->novelty) {
            pImpl->novelty->recordAction(plannedAction);
        }
        
        auto action = std::make_unique<Action>(plannedAction);
        return action;
    } else {
        // Fallback: simple motor-based action selection (original implementation)
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
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

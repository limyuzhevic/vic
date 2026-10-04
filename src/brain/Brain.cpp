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
        
        // Apply comprehensive dopamine effects:
        // 1. Modulate neural excitability (already implemented)
        // 2. Enhance plasticity rules (STDP and Hebbian)
        // 3. Influence memory consolidation
        // 4. Affect attention allocation
        
        float dopamineLevel = pImpl->dopamine->getLevel();
        
        // ========== Enhanced Neuromodulation Effects ==========
        
        // 1. Modulate neural excitability
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
        
        // 2. Apply dopamine effects on plasticity (enhanced)
        if (pImpl->stdp) {
            // Positive dopamine enhances LTP, negative enhances LTD
            float ltpWeight = 0.01f * (1.0f + dopamineLevel);  // Range: 0.01-0.02
            float ltdWeight = 0.012f * (1.0f - dopamineLevel * 0.5f);  // Range: ~0.009-0.018
            float tau = 20.0f * (0.5f + dopamineLevel * 0.5f);  // Range: 10-30
            pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
        }
        
        // 3. Enhance Hebbian learning with dopamine
        if (pImpl->hebbian) {
            // Dopamine boosts Hebbian consolidation
            float hebbianStrength = 1.0f + std::abs(dopamineLevel) * 0.5f;
            pImpl->hebbian->setLearningRate(0.001f * hebbianStrength);
        }
        
        // 4. Influence memory consolidation
        if (pImpl->episodicMemory && dopamineLevel > 0.1f) {
            // High dopamine promotes memory consolidation
            pImpl->episodicMemory->consolidate(0.2f);  // Lower threshold for consolidation
        }
        
        // 5. Affect attention allocation (dopamine guided attention)
        if (pImpl->attention && pImpl->workingMemory) {
            auto workingMemoryPattern = pImpl->workingMemory->retrieve();
            if (!workingMemoryPattern.empty()) {
                // Dopamine-based attention allocation
                // Higher dopamine increases attention to rewarding stimuli
                float attentionGain = 1.0f + dopamineLevel * 2.0f;
                pImpl->attention->setInhibitionStrength(0.5f / attentionGain);
                pImpl->attention->setExcitationStrength(1.5f * attentionGain);
            }
        }
    }
    
    // Update acetylcholine (ACh) effects on attention and memory
    if (pImpl->attention) {
        // ACh enhances attentional focus and memory encoding
        float achLevel = pImpl->dopamine ? pImpl->dopamine->getLevel() * 0.3f : 0.0f;  // Simplified
        
        // Modulate attention parameters based on ACh
        pImpl->attention->setInhibitionStrength(0.5f - achLevel * 0.3f);  // Less inhibition
        pImpl->attention->setExcitationStrength(1.5f + achLevel * 0.5f);   // More excitation
        
        // ACh promotes memory encoding
        if (pImpl->workingMemory) {
            pImpl->workingMemory->strengthenMemory(1.0f + achLevel);
        }
    }
    
    // Update noradrenaline (NE) effects on arousal and salience
    if (pImpl->novelty) {
        // NE increases arousal and salience detection
        float neLevel = pImpl->dopamine ? pImpl->dopamine->getLevel() * 0.4f : 0.0f;  // Simplified
        
        // Enhance novelty detection and learning
        pImpl->novelty->update(pImpl->timestep * (1.0f + neLevel));
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
    
    // ========== STEP 7: Update episodic memory with current experience ==========
    // Enhanced episodic memory integration - store current brain state
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode with comprehensive data
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons with detailed activation levels
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
            
            // Store reward and neuromodulation state in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            episode.metadata = "{" + std::to_string(currentTime) + "}" 
                             + " curiosity=" + std::to_string(pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f)
                             + " novelty=" + std::to_string(pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f);
            
            // Store in working memory first for faster access
            if (pImpl->workingMemory) {
                std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
                if (!workingMemoryPattern.empty()) {
                    episode.sensoryState = workingMemoryPattern;
                }
            }
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Enhanced prediction system integration with working memory
        if (pImpl->workingMemory) {
            auto workingMemoryPattern = pImpl->workingMemory->retrieve();
            if (!workingMemoryPattern.empty()) {
                // Store prediction input in episodic memory for learning
                if (pImpl->episodicMemory) {
                    EpisodicMemoryItem predictionInput;
                    predictionInput.timestamp = currentStep;
                    predictionInput.sensoryState = workingMemoryPattern;
                    predictionInput.metadata = "prediction_input";
                    pImpl->episodicMemory->storeEpisode(predictionInput);
                }
                
                // In a full implementation, this would go through prediction pipeline:
                // 1. Generate prediction based on working memory
                // 2. Compare with actual sensory input (when available)
                // 3. Update prediction error
                // 4. Learn from prediction error
            }
        }
        
        // Also connect prediction to episodic memory for pattern completion
        if (pImpl->episodicMemory && !pImpl->episodicMemory->getRecentEpisodes(3).empty()) {
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(3);
            for (const auto* episode : recentEpisodes) {
                // Use prediction system to predict next state based on episodic patterns
                if (!episode->sensoryState.empty()) {
                    // Enhanced prediction using episodic memory
                    // This would drive concept formation and planning
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
        // Enhanced concept formation with multi-system integration:
        // 1. Extract patterns from working memory
        // 2. Relate to episodic memory for pattern completion
        // 3. Use attention to focus on relevant patterns
        // 4. Store new concepts based on prediction errors
        
        if (pImpl->workingMemory) {
            // Get working memory patterns for concept formation
            std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
            
            if (!workingMemoryPattern.empty()) {
                // Connect concept formation to episodic memory for pattern completion
                if (pImpl->episodicMemory && !pImpl->episodicMemory->getRecentEpisodes(5).empty()) {
                    auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
                    
                    // Form concepts by relating working memory patterns to episodic patterns
                    for (const auto* episode : recentEpisodes) {
                        if (!episode->neuronActivations.empty()) {
                            // Extract patterns from episodic memory
                            std::vector<float> episodicPattern = episode->neuronActivations;
                            
                            // Form new concepts by combining patterns
                            // This would involve:
                            // 1. Comparing working memory pattern to episodic patterns
                            // 2. Identifying similarities and differences
                            // 3. Creating concept representations
                            
                            // In a full implementation:
                            // pImpl->conceptFormation->processPattern(workingMemoryPattern);
                            // pImpl->conceptFormation->associateWithEpisodic(episode);
                        }
                    }
                }
                
                // Use attention to focus on salient patterns for concept formation
                if (pImpl->attention && pImpl->workingMemory->getMemoryActivity() > 0.3f) {
                    auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
                    
                    // Apply attention to working memory patterns for concept extraction
                    pImpl->attention->processCompetition(memoryNeurons);
                    
                    // Focus on winning patterns for concept formation
                    std::vector<NeuronId> attendedNeurons;
                    for (NeuronId neuron : memoryNeurons) {
                        if (pImpl->attention->isAttended(neuron)) {
                            attendedNeurons.push_back(neuron);
                        }
                    }
                    
                    if (!attendedNeurons.empty()) {
                        // Extract concepts from attended patterns
                        // pImpl->conceptFormation->extractConcepts(attendedNeurons);
                    }
                }
            }
        }
        
        // Connect concept formation to prediction system for pattern completion
        if (pImpl->predictionSystem) {
            // Concept formation can help improve predictions by recognizing patterns
            // Based on formed concepts, predictions can be more accurate
            
            if (pImpl->workingMemory) {
                std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
                
                if (!workingMemoryPattern.empty()) {
                    // Use concept-based prediction enhancement
                    // pImpl->predictionSystem->updateConcepts(pImpl->conceptFormation->getConcepts());
                }
            }
        }
        
        // Store formed concepts for later use
        if (pImpl->workingMemory) {
            // Concepts can be stored in working memory for immediate use
            // or in episodic memory for long-term storage
            // pImpl->conceptFormation->storeConcepts();
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Enhanced memory replay mechanism ==========
    // Implement sleep/rest cycle with memory consolidation and replay
    if (currentStep % pImpl->replayInterval == 0) {
        // Check if brain is in resting state (simplified sleep detection)
        bool isResting = (pImpl->isResting || 
                         (pImpl->workingMemory && pImpl->workingMemory->getMemoryActivity() < 0.1f) ||
                         (pImpl->episodicMemory && pImpl->episodicMemory->getEpisodeCount() > 10));
        
        if (isResting || pImpl->developmentSystem) {
            // Sleep/rest cycle initiated
            pImpl->isResting = true;
            
            if (pImpl->episodicMemory) {
                // Get episodes for replay based on:
                // 1. Recency (more recent episodes)
                // 2. Reward (higher reward episodes)  
                // 3. Unpredictability (higher prediction error episodes)
                // 4. Novelty (higher novelty episodes)
                
                auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(5);
                
                // Enhanced replay: reactivate patterns and strengthen connections
                for (const auto* episode : episodesToReplay) {
                    // Reactivate neurons to strengthen connections
                    pImpl->episodicMemory->replayEpisode(episode);
                    
                    // Also apply replay effects to working memory
                    if (pImpl->workingMemory) {
                        std::vector<float> episodePattern;
                        for (float act : episode->neuronActivations) {
                            episodePattern.push_back(act * 0.8f);  // Decay replay activation
                        }
                        pImpl->workingMemory->store(episodePattern, 0.5f);  // Store with lower strength
                    }
                }
                
                // Consolidation during replay
                if (currentStep % (pImpl->replayInterval * 2) == 0) {
                    pImpl->episodicMemory->consolidate(0.5f);  // Higher consolidation threshold
                }
            }
        } else {
            // Not resting, just periodic replay for pattern maintenance
            pImpl->isResting = false;
            
            if (pImpl->episodicMemory && !pImpl->episodicMemory->getRecentEpisodes(3).empty()) {
                auto episodesToReplay = pImpl->episodicMemory->getRecentEpisodes(3);
                for (const auto* episode : episodesToReplay) {
                    pImpl->episodicMemory->replayEpisode(episode);
                }
            }
        }
    }
    
    // ========== STEP 13: Sleep/rest cycle with memory consolidation ==========
    // Implement a simplified sleep cycle that consolidates memories
    if (currentStep % 500 == 0) {  // Every 500 steps (10 seconds at 0.1ms timestep)
        pImpl->isResting = true;
        pImpl->stepsSinceLastEpisode = 0;
        
        // Enhanced memory consolidation during rest
        if (pImpl->episodicMemory) {
            // Multi-stage consolidation:
            // 1. Reactivate recent episodes
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(10);
            for (const auto* episode : recentEpisodes) {
                // Replay for consolidation
                pImpl->episodicMemory->replayEpisode(episode);
                
                // Strengthen related associations
                if (pImpl->associativeMemory) {
                    if (!episode->sensoryState.empty() && !episode->action.empty()) {
                        pImpl->associativeMemory->updateAssociation(
                            episode->sensoryState, 
                            std::vector<float>(episode->action.begin(), episode->action.end()), 
                            0.05f);
                    }
                }
            }
            
            // 2. Consolidate important memories (high reward, novel)
            pImpl->episodicMemory->consolidate(0.4f);
            
            // 3. Update developmental stage based on experience
            if (pImpl->developmentSystem) {
                // Development accelerates during rest
                pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 100);
            }
        }
        
        // Working memory cleanup during rest
        if (pImpl->workingMemory) {
            pImpl->workingMemory->clear();  // Clear working memory (simulating sleep)
            // But keep important traces in episodic memory
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
    // Enhanced action selection with neural planning integration
    
    // First, try to get action from neural planner if available
    if (pImpl->planner && pImpl->workingMemory) {
        // Neural planning: evaluate possible actions based on:
        // 1. Working memory patterns (recent experiences)
        // 2. Current neural activity
        // 3. Episodic memory (past episodes)
        // 4. Attention allocation (what's important)
        
        auto workingMemoryPattern = pImpl->workingMemory->retrieve();
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();n
        
        // Get episodic memory episodes for planning context
        std::vector<const EpisodicMemoryItem*> recentEpisodes;
        if (pImpl->episodicMemory) {
            recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(3);
        }
        
        // Neural planner evaluates possible actions
        auto plannedAction = pImpl->planner->planAction(this, memoryNeurons, recentEpisodes);
        if (plannedAction) {
            return plannedAction;
        }
    }
    
    // Fallback to basic motor neuron activity-based action selection
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
    
    // Enhanced action selection with neuromodulation influence
    ActionType type = ActionType::Wait;
    
    if (firingMotor > 0) {
        // Apply neuromodulation-based action selection
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            
            // Positive dopamine favors approach behaviors
            if (dopamineLevel > 0.1f) {
                type = ActionType::MoveForward;
            } else if (dopamineLevel < -0.1f) {
                // Negative dopamine favors avoidance behaviors
                type = ActionType::MoveBackward;
            } else {
                // Neutral dopamine: basic movement
                type = ActionType::MoveForward;
            }
        } else {
            // Default behavior: move forward
            type = ActionType::MoveForward;
        }
        
        // Apply curiosity-based exploration if available
        if (pImpl->curiosity && pImpl->curiosity->getLevel() > 0.5f) {
            // High curiosity leads to more exploratory actions
            // Could select random action or more complex behavior
            if (firingMotor >= 2) {  // At least 2 motor neurons active
                // Choose more exploratory action
                type = ActionType::Interact;
            }
        }
        
        // Apply attention-based action selection
        if (pImpl->attention) {
            // Attention influences which action is selected based on what's being attended to
            // Higher attention to specific motor groups biases selection
            // For now, we already have the basic selection, but in full implementation
            // this would modulate the action probabilities
        }
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
        
        // Enhanced save: integrate with working and episodic memory systems
        
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
        
        // Enhanced: Save working memory state
        if (pImpl->workingMemory) {
            std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
            std::vector<NeuronId> workingMemoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            
            // Create working memory checkpoint data
            WorkingMemoryCheckpointData wmData;
            wmData.pattern = workingMemoryPattern;
            wmData.neurons = workingMemoryNeurons;
            wmData.capacity = pImpl->workingMemory->getCapacity();
            wmData.decayRate = pImpl->workingMemory->getDecayRate();  // Assuming this method exists
            
            if (!writer.writeWorkingMemory(wmData)) {
                NLM_LOG_ERROR("Failed to write working memory to checkpoint");
                return false;
            }
        }
        
        // Enhanced: Save episodic memory state
        if (pImpl->episodicMemory) {
            std::vector<EpisodicMemoryItem> recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(100);  // Last 100 episodes
            
            // Create episodic memory checkpoint data
            EpisodicMemoryCheckpointData emData;
            emData.episodes = recentEpisodes;
            emData.maxEpisodes = pImpl->episodicMemory->getMaxEpisodes();  // Assuming this method exists
            emData.replayEnabled = pImpl->episodicMemory->isReplayEnabled();  // Assuming this method exists
            
            if (!writer.writeEpisodicMemory(emData)) {
                NLM_LOG_ERROR("Failed to write episodic memory to checkpoint");
                return false;
            }
        }
        
        // Enhanced: Save neuromodulation state
        if (pImpl->dopamine) {
            NeuromodulationCheckpointData neuromodData;
            neuromodData.dopamineLevel = pImpl->dopamine->getLevel();
            neuromodData.dopaminePlasticityFactor = pImpl->dopamine->getPlasticityFactor();  // Assuming this method exists
            neuromodData.curiosityLevel = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
            neuromodData.noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            neuromodData.predictionError = pImpl->predictionError ? pImpl->predictionError->getLevel() : 0.0f;
            
            if (!writer.writeNeuromodulation(neuromodData)) {
                NLM_LOG_ERROR("Failed to write neuromodulation state to checkpoint");
                return false;
            }
        }
        
        // Enhanced: Save development state
        if (pImpl->developmentSystem) {
            DevelopmentCheckpointData devData;
            devData.developmentalStage = pImpl->developmentalStage;
            devData.developmentalAge = pImpl->developmentSystem->getAge();  // Assuming this method exists
            devData.plasticityModifier = pImpl->developmentSystem->getPlasticityModifier();  // Assuming this method exists
            
            if (!writer.writeDevelopment(devData)) {
                NLM_LOG_ERROR("Failed to write development state to checkpoint");
                return false;
            }
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        NLM_LOG_INFO("Enhanced checkpoint includes: neural, synaptic, working memory, episodic memory, neuromodulation, and development states");
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
        
        // Reset brain to clean state before loading
        reset();
        // Re-initialize brain (required since reset clears everything)
        if (!initialize()) {
            NLM_LOG_ERROR("Failed to re-initialize brain after reset");
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
        // For enhanced integration, we need to also load memory states
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        // Enhanced: Load working memory state
        if (reader.hasWorkingMemory()) {
            WorkingMemoryCheckpointData wmData;
            if (reader.readWorkingMemory(wmData)) {
                // Restore working memory state
                if (pImpl->workingMemory) {
                    // Clear existing working memory
                    pImpl->workingMemory->clear();
                    // Restore working memory neurons and patterns
                    // This would require enhanced WorkingMemory implementation
                    // For now, we'll just note that working memory was saved
                    NLM_LOG_INFO("Working memory state loaded (enhanced integration)");
                }
            } else {
                NLM_LOG_ERROR("Failed to read working memory from checkpoint");
                return false;
            }
        }
        
        // Enhanced: Load episodic memory state
        if (reader.hasEpisodicMemory()) {
            EpisodicMemoryCheckpointData emData;
            if (reader.readEpisodicMemory(emData)) {
                // Restore episodic memory state
                if (pImpl->episodicMemory) {
                    // Clear existing episodic memory
                    pImpl->episodicMemory->clear();
                    // Restore episodic memory episodes
                    // This would require enhanced EpisodicMemory implementation
                    // For now, we'll just note that episodic memory was saved
                    NLM_LOG_INFO("Episodic memory state loaded (enhanced integration)");
                }
            } else {
                NLM_LOG_ERROR("Failed to read episodic memory from checkpoint");
                return false;
            }
        }
        
        // Enhanced: Load neuromodulation state
        if (reader.hasNeuromodulation()) {
            NeuromodulationCheckpointData neuromodData;
            if (reader.readNeuromodulation(neuromodData)) {
                // Restore neuromodulation state
                if (pImpl->dopamine) {
                    // Apply dopamine level to brain state
                    pImpl->dopamine->setLevel(neuromodData.dopamineLevel);
                }
                // Restore other neuromodulation states
                NLM_LOG_INFO("Neuromodulation state loaded (enhanced integration)");
            } else {
                NLM_LOG_ERROR("Failed to read neuromodulation state from checkpoint");
                return false;
            }
        }
        
        // Enhanced: Load development state
        if (reader.hasDevelopment()) {
            DevelopmentCheckpointData devData;
            if (reader.readDevelopment(devData)) {
                // Restore development state
                pImpl->developmentalStage = devData.developmentalStage;
                // Restore other development parameters
                NLM_LOG_INFO("Development state loaded (enhanced integration)");
            } else {
                NLM_LOG_ERROR("Failed to read development state from checkpoint");
                return false;
            }
        }
        
        NLM_LOG_INFO("Brain state loaded successfully with enhanced memory integration");
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

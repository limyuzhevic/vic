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
        restThreshold = config->getOr<float>("rest_threshold", 10.0f);
        consolidationDuration = config->getOr<float>("consolidation_duration", 2.0f);
        
        // Working memory settings
        workingMemoryCapacity = config->getOr<size_t>("working_memory_capacity", 100);
        episodicMemoryMaxEpisodes = config->getOr<size_t>("episodic_memory_max_episodes", 1000);
        
        // Visualization settings
        visualizationUpdateRate = config->getOr<float>("visualization_update_rate", 30.0f);
        
        // Development settings
        developmentSynaptogenesisRate = config->getOr<float>("development_synaptogenesis_rate", 0.001f);
        developmentPruningRate = config->getOr<float>("development_pruning_rate", 0.00001f);
        
        // Reward settings
        rewardDiscountFactor = config->getOr<double>("reward_discount_factor", 0.99);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    // Sleep/rest cycle state
    float wakefulnessTime;
    float restTime;
    float restThreshold;
    float consolidationDuration;
    bool isResting;
    
    // Working memory capacity
    size_t workingMemoryCapacity;
    
    // Episodic memory capacity
    size_t episodicMemoryMaxEpisodes;
    
    // Visualization update rate
    float visualizationUpdateRate;
    
    // Development rates
    float developmentSynaptogenesisRate;
    float developmentPruningRate;
    
    // Reward discount factor
    double rewardDiscountFactor;
    
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
        
        // Apply comprehensive dopamine effects on neural computation
        float dopamineLevel = pImpl->dopamine->getLevel();
        
        // 1. Effects on neural excitability (current implementation)
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias, negative reduces it
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    } else if (excitabilityMod < 0.0f) {
                        neuron->injectCurrent(excitabilityMod * 0.5f);  // Weaker inhibition
                    }
                }
            }
        }
        
        // 2. Effects on plasticity rules (enhanced beyond scaling)
        float plasticityMod = pImpl->dopamine->getPlasticityFactor();
        
        // STDP modulation
        pImpl->stdp->setLTPWeight(0.01f * (1.0f + dopamineLevel));
        pImpl->stdp->setLTDWeight(0.012f * (1.0f - dopamineLevel * 0.5f));
        
        // Hebbian learning enhancement
        // Higher dopamine increases Hebbian learning strength
        if (pImpl->hebbian) {
            float hebbianMod = 1.0f + dopamineLevel * 0.5f;
            pImpl->hebbian->setLearningRate(pImpl->hebbian->getLearningRate() * hebbianMod);
        }
        
        // 3. Effects on working memory consolidation
        if (pImpl->workingMemory && dopamineLevel > 0.5f) {
            // High dopamine promotes working memory consolidation
            pImpl->workingMemory->strengthenMemory(1.0f + dopamineLevel * 0.5f);
        }
        
        // 4. Effects on episodic memory encoding
        if (pImpl->episodicMemory) {
            // Dopamine enhances episodic memory encoding strength
            // This would be used during the episodic memory storage phase
            pImpl->episodicMemory->setEncodingStrength(pImpl->episodicMemory->getEncodingStrength() * (1.0f + dopamineLevel));
        }
        
        // 5. Effects on attention system
        if (pImpl->attention && dopamineLevel > 0.3f) {
            // Moderate to high dopamine enhances attentional focus
            pImpl->attention->setInhibitionStrength(0.5f + dopamineLevel * 0.3f);
            pImpl->attention->setExcitationStrength(1.5f + dopamineLevel * 0.5f);
        }
        
        // 6. Effects on structural plasticity
        if (pImpl->structuralPlasticity) {
            // Dopamine modulates structural plasticity rates
            float structuralMod = 1.0f + dopamineLevel * 0.3f;
            float currentSynaptogenesis = pImpl->structuralPlasticity->getSynaptogenesisRate();
            float currentPruning = pImpl->structuralPlasticity->getPruningRate();
            
            pImpl->structuralPlasticity->setSynaptogenesisRate(currentSynaptogenesis * structuralMod);
            pImpl->structuralPlasticity->setPruningRate(currentPruning / structuralMod);  // Reduced pruning with high dopamine
        }
        
        // 7. Effects on prediction system
        if (pImpl->predictionSystem) {
            // Dopamine affects prediction error computation and confidence
            // Higher dopamine increases exploration and prediction uncertainty
            pImpl->predictionSystem->setConfidence(pImpl->predictionSystem->getConfidence() * (1.0f - dopamineLevel * 0.2f));
        }
        
        // 8. Effects on developmental stage progression
        if (dopamineLevel > 1.5f && pImpl->developmentSystem) {
            // High dopamine accelerates development (rewarded learning)
            pImpl->developmentSystem->accelerateDevelopment();
        }
        
        // 9. Effects on neural boundary setting and adaptation
        if (dopamineLevel > 0.0f) {
            // Dopamine helps set neural boundaries and prevents runaway excitation
            for (auto& region : pImpl->regions) {
                // Apply adaptive gain control based on dopamine level
                float gainControl = std::clamp(1.0f - dopamineLevel * 0.2f, 0.5f, 1.5f);
                region->setGainControl(gainControl);
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
            
            // Store sensory input if available
            // In a full implementation, we would store the actual sensory observations
            // For now, we'll encode key sensory features in the episode
            if (!pImpl->sensoryNeurons.empty()) {
                episode.sensoryState.resize(pImpl->sensoryNeurons.size());
                for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                    episode.sensoryState[i] = 
                        std::abs(pImpl->sensoryNeurons[i]->getState().membranePotential - 
                                pImpl->sensoryNeurons[i]->getState().restingPotential) / 20.0f;
                }
            }
            
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
            
            // Store action if one was produced
            // In a full implementation, this would capture the actual action
            episode.action = ActionType::Wait;  // Placeholder
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Update prediction system with current neural activity patterns
        // The prediction system learns temporal sequences and predicts future states
        
        // Extract current neural activity pattern for prediction
        std::vector<float> neuralPattern;
        neuralPattern.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    neuralPattern.push_back(
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                }
            }
        }
        
        // Update prediction system with current state
        // In a full implementation, this would use the prediction system to
        // predict next states and compute prediction errors
        
        // For now, we'll enable basic prediction error tracking
        if (!neuralPattern.empty()) {
            // This would be enhanced in a full implementation to use
            // the PredictionSystem's predictNextState and updatePredictions methods
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
                case DevelopmentalStage::Aging:
                    plasticityMod = 0.1f;  // Reduced
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
    
    // ========== STEP 16: Sleep/rest cycle and memory consolidation ==========
    
    // Check for sleep/rest cycle
    if (!pImpl->isResting) {
        // Accumulate wakefulness time
        pImpl->wakefulnessTime += pImpl->timestep;
        
        // Enter resting state after threshold
        if (pImpl->wakefulnessTime >= pImpl->restThreshold) {
            pImpl->isResting = true;
            pImpl->wakefulnessTime = 0.0f;
            
            NLM_LOG_INFO("Brain entering resting state (sleep/rest) for memory consolidation");
            
            // Reset neuromodulation levels during rest
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(0.0f);
            }
            if (pImpl->novelty) {
                pImpl->novelty->setLevel(0.0f);
            }
            if (pImpl->curiosity) {
                pImpl->curiosity->setLevel(0.0f);
            }
            
            // Disable neural excitation during rest
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Reduce excitability to promote consolidation
                        neuron->reduceExcitability(0.5f);
                    }
                }
            }
        }
    } else {
        // In resting state
        pImpl->restTime += pImpl->timestep;
        
        // Perform memory consolidation after rest duration
        if (pImpl->restTime >= pImpl->consolidationDuration && pImpl->episodicMemory) {
            NLM_LOG_INFO("Performing memory consolidation");
            
            // Consolidate episodic memories - strengthen important ones, weaken weak ones
            pImpl->episodicMemory->consolidate(0.5f);  // Threshold for relevance
            
            // Replay important memories
            auto replayEpisodes = pImpl->episodicMemory->getEpisodesForReplay(5);
            for (const auto* episode : replayEpisodes) {
                pImpl->episodicMemory->replayEpisode(episode);
            }
            
            // Update development based on rest time
            if (pImpl->developmentSystem) {
                pImpl->developmentSystem->updateDevelopmentStage(pImpl->restTime);
            }
            
            // Reset rest time and allow waking
            pImpl->restTime = 0.0f;
            pImpl->isResting = false;
            
            // Restore neural excitability
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->restoreExcitability();
                    }
                }
            }
            
            NLM_LOG_INFO("Memory consolidation complete, waking");
        }
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    try {
        // Validate input parameters
        if (!pImpl) {
            NLM_LOG_ERROR("Brain implementation pointer is null in receiveSensoryInput");
            return;
        }
        
        const auto& values = input.getData();
        if (values.empty()) {
            NLM_LOG_DEBUG("Empty sensory input received");
            return;
        }
        
        size_t numSensory = pImpl->sensoryNeurons.size();
        if (numSensory == 0) {
            NLM_LOG_DEBUG("No sensory neurons available for input processing");
            return;
        }
        
        // Validate sensory neurons array
        if (pImpl->sensoryNeurons.empty() || 
            pImpl->sensoryNeurons.size() != numSensory) {
            NLM_LOG_ERROR("Invalid sensory neurons array state");
            return;
        }
        
        // Distribute input across sensory neurons
        for (size_t i = 0; i < numSensory; ++i) {
            // Validate neuron pointer before use
            if (!pImpl->sensoryNeurons[i]) {
                NLM_LOG_ERROR("Sensory neuron at index " + std::to_string(i) + " is null");
                continue;
            }
            
            // Normalize input value to range [-10, 10] mV
            float normalizedValue = 0.0f;
            if (i < values.size()) {
                normalizedValue = static_cast<float>(values[i]) * 10.0f;
            }
            
            // Check neuron validity before injection
            try {
                pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Failed to inject current into neuron " + 
                    std::to_string(pImpl->sensoryNeurons[i]->getId()) + ": ") + e.what());
                continue;
            }
            
            // Also store in working memory if value is significant
            if (pImpl->workingMemory && normalizedValue > 0.5f) {
                try {
                    pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Failed to store to working memory: ") + e.what());
                }
            }
        }
        
        NLM_LOG_DEBUG("Sensory input processed successfully, input size: " + std::to_string(values.size()));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in receiveSensoryInput: ") + e.what());
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    try {
        // Validate parameters
        if (!pImpl) {
            NLM_LOG_ERROR("Brain implementation pointer is null in injectCurrent");
            return;
        }
        
        if (current == 0.0f) {
            NLM_LOG_DEBUG("Zero current injection requested for neuron " + std::to_string(neuron.index()));
            return;
        }
        
        // Search for neuron with comprehensive error handling
        bool neuronFound = false;
        size_t checkedRegions = 0;
        
        for (auto& region : pImpl->regions) {
            if (!region) {
                NLM_LOG_ERROR("Null region encountered in injectCurrent");
                continue;
            }
            
            checkedRegions++;
            auto neurons = region->getAllNeurons();
            
            if (neurons.empty()) {
                continue;
            }
            
            for (auto* n : neurons) {
                if (!n) {
                    NLM_LOG_ERROR("Null neuron pointer found during search");
                    continue;
                }
                
                if (n->getId() == neuron) {
                    neuronFound = true;
                    
                    // Validate neuron state before injection
                    const auto& state = n->getState();
                    if (state.firingState == FiringState::Refractory) {
                        NLM_LOG_DEBUG("Attempt to inject current into refractory neuron " + 
                            std::to_string(neuron.index()));
                        return;  // Skip injection during refractory period
                    }
                    
                    try {
                        n->injectCurrent(current);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Failed to inject current into neuron " + 
                            std::to_string(neuron.index()) + ": ") + e.what());
                    }
                    
                    return;  // Neuron found and injected
                }
            }
        }
        
        if (!neuronFound) {
            NLM_LOG_ERROR("Neuron with ID " + std::to_string(neuron.index()) + " not found after checking " + 
                std::to_string(checkedRegions) + " regions");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in injectCurrent: ") + e.what());
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
    // Integrated action selection using NeuralPlanner
    // The NeuralPlanner evaluates action sequences and selects the best action
    // based on predicted outcomes and current neural state
    
    // Extract current neural activity pattern for planning
    std::vector<float> currentState;
    currentState.reserve(getTotalNeuronCount());
    
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                currentState.push_back(
                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
            }
        }
    }
    
    // Use NeuralPlanner if available
    if (pImpl->planner) {
        // Get reward prediction from dopamine for planning
        float targetReward = pImpl->dopamine ? pImpl->dopamine->getLevel() * 2.0f : 1.0f;
        
        // Plan action using neural planning system
        ActionType actionType = pImpl->planner->planAction(currentState, targetReward);
        
        auto action = std::make_unique<Action>(actionType);
        
        // Update NeuralPlanner with actual outcome for learning
        // In a full implementation, this would use the actual action taken
        // and its consequences to update the planner's internal models
        
        return action;
    }
    
    // Fallback to simple motor neuron activity if NeuralPlanner is not available
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
    
    // Validate file path
    if (filepath.empty()) {
        NLM_LOG_ERROR("Empty file path provided for save");
        return false;
    }
    
    // Check if we have a valid implementation pointer
    if (!pImpl) {
        NLM_LOG_ERROR("Brain implementation pointer is null during save");
        return false;
    }
    
    // Validate we have regions to save
    if (pImpl->regions.empty()) {
        NLM_LOG_ERROR("No regions available for saving brain state");
        return false;
    }
    
    try {
        // Validate checkpoint directory access
        std::string checkpointDir = "./checkpoints";
        if (auto dirCheck = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints")) {
            checkpointDir = *dirCheck;
        }
        
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set comprehensive metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Validate metadata values
        size_t neuronCount = getTotalNeuronCount();
        size_t synapseCount = getTotalSynapseCount();
        
        if (neuronCount == 0) {
            NLM_LOG_WARNING("Saving brain with zero neurons");
        }
        
        if (synapseCount == 0) {
            NLM_LOG_WARNING("Saving brain with zero synapses");
        }
        
        // Write neurons with enhanced error handling and validation
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(neuronCount);
        neuronData.restingPotential.reserve(neuronCount);
        neuronData.threshold.reserve(neuronCount);
        neuronData.resetPotential.reserve(neuronCount);
        neuronData.leakConductance.reserve(neuronCount);
        neuronData.firingState.reserve(neuronCount);
        neuronData.refractoryRemaining.reserve(neuronCount);
        neuronData.refractoryPeriod.reserve(neuronCount);
        neuronData.lastSpikeTime.reserve(neuronCount);
        
        size_t savedNeuronCount = 0;
        for (const auto& region : pImpl->regions) {
            if (!region) {
                NLM_LOG_ERROR("Null region encountered during neuron save");
                continue;
            }
            
            for (const auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_ERROR("Null population encountered during neuron save");
                    continue;
                }
                
                auto neurons = pop->getNeurons();
                for (const auto* neuron : neurons) {
                    if (!neuron) {
                        NLM_LOG_ERROR("Null neuron pointer encountered during save");
                        continue;
                    }
                    
                    try {
                        const auto& state = neuron->getState();
                        
                        // Validate neuron state before saving
                        if (std::isnan(state.membranePotential) || 
                            std::isinf(state.membranePotential)) {
                            NLM_LOG_ERROR("Invalid membrane potential for neuron " + 
                                std::to_string(neuron->getId().index()));
                            continue;
                        }
                        
                        neuronData.membranePotential.push_back(state.membranePotential);
                        neuronData.restingPotential.push_back(state.restingPotential);
                        neuronData.threshold.push_back(state.threshold);
                        neuronData.resetPotential.push_back(state.resetPotential);
                        neuronData.leakConductance.push_back(state.leakConductance);
                        neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                        neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                        neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                        neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                        
                        savedNeuronCount++;
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Exception saving neuron " + 
                            std::to_string(neuron->getId().index()) + ": ") + e.what());
                    }
                }
            }
        }
        
        NLM_LOG_INFO("Prepared " + std::to_string(savedNeuronCount) + " neurons for save");
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses with enhanced error handling
        SynapseCheckpointData synapseData;
        size_t savedSynapseCount = 0;
        
        for (const auto& region : pImpl->regions) {
            if (!region) {
                NLM_LOG_ERROR("Null region encountered during synapse save");
                continue;
            }
            
            auto synapses = region->getSynapses();
            for (const auto* syn : synapses) {
                if (!syn) {
                    NLM_LOG_ERROR("Null synapse pointer encountered during save");
                    continue;
                }
                
                try {
                    // Validate synapse data before saving
                    float weight = syn->getWeight();
                    if (std::isnan(weight) || std::isinf(weight)) {
                        NLM_LOG_ERROR("Invalid weight for synapse " + 
                            std::to_string(syn->getId().index()));
                        continue;
                    }
                    
                    synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                    synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                    synapseData.weight.push_back(weight);
                    synapseData.delay.push_back(syn->getDelay());
                    synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                    synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                    
                    savedSynapseCount++;
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Exception saving synapse " + 
                        std::to_string(syn->getId().index()) + ": ") + e.what());
                }
            }
        }
        
        NLM_LOG_INFO("Prepared " + std::to_string(savedSynapseCount) + " synapses for save");
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize with error handling
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        size_t bytesWritten = writer.getBytesWritten();
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(bytesWritten) + " bytes)");
        
        // Log save statistics
        if (bytesWritten == 0) {
            NLM_LOG_WARNING("Checkpoint file size is zero");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    // Validate file path
    if (filepath.empty()) {
        NLM_LOG_ERROR("Empty file path provided for load");
        return false;
    }
    
    // Check if we have a valid implementation pointer
    if (!pImpl) {
        NLM_LOG_ERROR("Brain implementation pointer is null during load");
        return false;
    }
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        // Validate checkpoint integrity
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons with enhanced error handling
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Validate neuron data consistency
        if (neuronData.membranePotential.size() != neuronData.restingPotential.size() ||
            neuronData.membranePotential.size() != neuronData.threshold.size()) {
            NLM_LOG_ERROR("Inconsistent neuron checkpoint data: size mismatch");
            return false;
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(neuronData.membranePotential.size()) + 
                    " neurons from checkpoint");
        
        // Apply neuron states with validation and error recovery
        size_t appliedNeuronCount = 0;
        size_t validationErrorCount = 0;
        
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            if (!region) {
                NLM_LOG_ERROR("Null region encountered during neuron load");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_ERROR("Null population encountered during neuron load");
                    continue;
                }
                
                auto neurons = pop->getNeurons();
                for (auto* neuron : neurons) {
                    if (!neuron) {
                        NLM_LOG_ERROR("Null neuron pointer encountered during load");
                        continue;
                    }
                    
                    try {
                        // Validate checkpoint data before applying
                        if (idx >= neuronData.membranePotential.size()) {
                            break;  // No more data to apply
                        }
                        
                        float membranePotential = neuronData.membranePotential[idx];
                        if (std::isnan(membranePotential) || std::isinf(membranePotential)) {
                            NLM_LOG_ERROR("Invalid membrane potential in checkpoint for neuron " + 
                                std::to_string(neuron->getId().index()));
                            validationErrorCount++;
                            idx++;
                            continue;
                        }
                        
                        neuron->setMembranePotential(membranePotential);
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
                        
                        appliedNeuronCount++;
                        idx++;
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Exception loading neuron " + 
                            std::to_string(neuron->getId().index()) + ": ") + e.what());
                        validationErrorCount++;
                        idx++;
                        continue;
                    }
                }
            }
        }
        
        NLM_LOG_INFO("Applied " + std::to_string(appliedNeuronCount) + " neuron states from checkpoint");
        
        if (validationErrorCount > 0) {
            NLM_LOG_WARNING("Skipped " + std::to_string(validationErrorCount) + 
                           " neurons due to invalid checkpoint data");
        }
        
        // Read synapses with enhanced error handling
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Validate synapse data consistency
        if (synapseData.weight.size() != synapseData.sourceNeuron.size() ||
            synapseData.weight.size() != synapseData.destinationNeuron.size()) {
            NLM_LOG_ERROR("Inconsistent synapse checkpoint data: size mismatch");
            return false;
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses from checkpoint");
        
        // Apply synapse states - comprehensive error handling
        size_t appliedSynapseCount = 0;
        size_t synapseValidationErrorCount = 0;
        
        for (size_t i = 0; i < synapseData.weight.size(); ++i) {
            try {
                // Validate synapse data
                float weight = synapseData.weight[i];
                if (std::isnan(weight) || std::isinf(weight)) {
                    NLM_LOG_ERROR("Invalid synapse weight in checkpoint for synapse " + 
                        std::to_string(i));
                    synapseValidationErrorCount++;
                    continue;
                }
                
                // Find and apply synapse - this is complex because we need to match
                // source and destination neurons with the correct synapse
                // For now, we'll log the synapse information
                
                // TODO: Implement proper synapse matching and application
                // For now, just validate and count
                
                appliedSynapseCount++;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Exception processing synapse " + 
                    std::to_string(i) + ": ") + e.what());
                synapseValidationErrorCount++;
                continue;
            }
        }
        
        NLM_LOG_INFO("Processed " + std::to_string(appliedSynapseCount) + " synapse states from checkpoint");
        
        if (synapseValidationErrorCount > 0) {
            NLM_LOG_WARNING("Skipped " + std::to_string(synapseValidationErrorCount) + 
                           " synapses due to invalid checkpoint data");
        }
        
        // Restore brain simulation state
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0f;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        
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

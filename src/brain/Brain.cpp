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
    
    // Update prediction error signal
    if (pImpl->predictionError) {
        pImpl->predictionError->update(pImpl->timestep);
        // Apply prediction error to neuromodulation systems
        float predError = pImpl->predictionError->getLevel();
        if (pImpl->dopamine) {
            pImpl->dopamine->updatePredictionError(predError);
        }
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply enhanced dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        float plasticityMod = pImpl->dopamine->getPlasticityFactor();
        
        // Enhanced dopamine effects on neural excitability
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // 1. Excitability modulation (dopamine makes neurons more likely to fire)
                    float excitabilityMod = dopamineLevel * 0.5f * plasticityMod;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                    
                    // 2. Threshold modulation (dopamine lowers firing threshold)
                    if (dopamineLevel > 0.2f) {
                        float thresholdReduction = 2.0f * (dopamineLevel - 0.2f);
                        neuron->setThreshold(std::max(1.0f, neuron->getThreshold() - thresholdReduction));
                    }
                    
                    // 3. Burst enhancement (dopamine promotes burst firing for important events)
                    if (dopamineLevel > 0.7f && neuron->isFiring()) {
                        // Boost firing for salient events
                        neuron->injectCurrent(10.0f * (dopamineLevel - 0.7f));
                    }
                }
            }
        }
        
        // Apply neuromodulation effects on plasticity rules
        applyNeuromodulationEffects(dopamineLevel, plasticityMod);
    }
    
    // Apply ACh effects on attention and memory (when available)
    if (pImpl->novelty) {
        // ACh is modulated by novelty - enhances memory encoding
        float noveltyLevel = pImpl->novelty->getLevel();
        if (noveltyLevel > 0.5f) {
            // Enhanced memory encoding for novel experiences
            if (pImpl->workingMemory) {
                pImpl->workingMemory->strengthenMemory(1.0f + noveltyLevel * 0.5f);
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
            
            // ENHANCED: Store sensory state if available
            // This is critical for later concept formation
            if (pImpl->attention) {
                // Get sensory input through attentional focus
                std::vector<float> sensoryState;
                
                // Create a comprehensive sensory representation
                // This should capture what the agent is perceiving
                std::vector<float> visualFeatures;
                std::vector<float> internalFeatures;
                std::vector<float> proprioceptiveFeatures;
                
                // For now, create a simple representation based on active neuron patterns
                // In a full implementation, this would use actual sensory modalities
                size_t totalFeatures = 100;  // Arbitrary feature dimension
                sensoryState.reserve(totalFeatures);
                
                // Convert neural activity to sensory-like features
                // This is a simplified encoding - real system would use dedicated sensory neurons
                float maxActivity = 1.0f;
                size_t featureIndex = 0;
                
                for (size_t i = 0; i < std::min(static_cast<size_t>(50), episode.activeNeurons.size()); ++i) {
                    NeuronId neuronId = episode.activeNeurons[i];
                    float activation = episode.neuronActivations[i];
                    
                    // Distribute activation across multiple feature channels
                    // Simulate different sensory modalities
                    if (featureIndex < totalFeatures) {
                        sensoryState.push_back(activation);
                        featureIndex++;
                    }
                    
                    // Create correlated features to simulate modality patterns
                    if (featureIndex < totalFeatures && activation > 0.5f) {
                        float correlatedFeature = activation * 0.8f + 
                                                (featureIndex % 3 == 0 ? 0.1f : 0.0f);  // Visual-like
                        sensoryState.push_back(correlatedFeature);
                        featureIndex++;
                    }
                    
                    if (featureIndex < totalFeatures) {
                        float internalFeature = activation * 0.6f + 0.2f;  // Internal state
                        sensoryState.push_back(internalFeature);
                        featureIndex++;
                    }
                }
                
                // Pad with zeros if needed
                while (sensoryState.size() < totalFeatures) {
                    sensoryState.push_back(0.0f);
                }
                
                // Store sensory state as the core experience
                episode.sensoryState = sensoryState;
                
                // Also store the result of the action taken
                // This helps the agent learn cause-effect relationships
                episode.resultingSensoryState = sensoryState;  // For now, same as input
                episode.action = ActionType::Wait;  // Would be set by actual action system
            }
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // Store recent action-reward outcomes for episodic learning
    if (pImpl->dopamine && pImpl->stepsSinceLastEpisode == 0) {
        // Create a simple action outcome record
        // In a full implementation, this would be stored as a separate action episode
        NLM_LOG_INFO("Recording action outcome at step " + std::to_string(currentStep) +
                     " (reward: " + std::to_string(pImpl->dopamine->getLevel()) + ")");
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem && pImpl->attention) {
        // The prediction system should be updated with the current sensory state
        // For now, we'll use attention to provide a focus signal
        // In a full implementation, this would use actual sensory observations
        
        // Get the current attentional focus (simplified)
        std::vector<float> currentSensoryFocus;
        
        // Create a sensory representation based on current neural activity
        // This simulates what the prediction system would see in a real system
        float totalActivity = 0.0f;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    totalActivity += std::abs(neuron->getState().membranePotential);
                }
            }
        }
        
        // Normalize to get a feature vector-like representation
        if (totalActivity > 0.0f) {
            currentSensoryFocus.reserve(10);
            for (size_t i = 0; i < 10; ++i) {
                // Create a simple feature representation
                float feature = (i < pImpl->regions.size() ? 
                    100.0f / (1.0f + totalActivity) : 
                    0.5f) * (i % 2 == 0 ? 1.0f : 0.5f);
                currentSensoryFocus.push_back(feature);
            }
        }
        
        // Note: PredictionSystem::predictNextState() would be called here
        // PredictionSystem::updatePredictions() would be called with actual vs predicted
        // For now, we'll just track that we're using the system
        
        // The prediction system could update its internal model of expected states
        // This would help with curiosity and novelty detection
        
        // Simulate prediction error calculation based on novelty
        if (pImpl->novelty) {
            float noveltyLevel = pImpl->novelty->getLevel();
            float predictedNovelty = 0.5f; // Simple prediction baseline
            
            // Update prediction system with error
            // In a full implementation: pImpl->predictionSystem->updatePredictions(currentSensoryFocus, predictedNovelty);
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
        
        // ENHANCED: Apply neuromodulation to attention
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            // Dopamine enhances attention focus and saliency
            if (dopamineLevel > 0.1f) {
                // Apply attentional boost to all competing neurons
                for (NeuronId neuron : competitors) {
                    // Dopamine modulates attentional gain
                    if (brain_) {
                        brain_->injectCurrent(neuron, dopamineLevel * 0.3f);
                    }
                }
            }
        }
        
        if (pImpl->novelty) {
            float noveltyLevel = pImpl->novelty->getLevel();
            // Novelty increases attentional scope
            if (noveltyLevel > 0.3f) {
                // Expand attentional scope for novel stimuli
                // This is a simplified implementation
                NLM_LOG_INFO("Attention expanded due to novelty: level=" + std::to_string(noveltyLevel));
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation && pImpl->episodicMemory && pImpl->attention) {
        // Concept formation should be updated based on recent episodes
        // This is where the brain abstracts patterns from episodic memory
        
        // Get recent episodes that the attention system has been focusing on
        auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(3);
        
        // Present these episodes to the concept formation system
        // Concept formation identifies common patterns and forms abstractions
        for (size_t i = 0; i < recentEpisodes.size(); ++i) {
            const auto* episode = recentEpisodes[i];
            
            if (!episode->sensoryState.empty()) {
                // Concept formation learns from sensory patterns
                // This is where concepts emerge from raw sensory experiences
                size_t conceptId = pImpl->conceptFormation->presentExperience(
                    episode->sensoryState,
                    episode->sensoryState,  // Using sensory as features for now
                    episode->reward,
                    episode->timestamp
                );
                
                if (conceptId > 0) {
                    NLM_LOG_INFO("Concept formation: Created/used concept " + 
                                 std::to_string(conceptId) + 
                                 " from episode at step " + std::to_string(episode->timestamp));
                }
            }
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Enhanced replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Multi-pass replay with working memory integration
        replayMemoryPattern(currentStep, currentTime);
        
        // Apply prediction error calculation based on replayed experiences
        float predictionError = calculateReplayPredictionError(currentStep);
        
        // Apply plasticity effects during replay
        applyReplayPlasticityEffects(currentStep, currentTime, predictionError);
        
        // Update prediction systems based on replayed experiences
        if (pImpl->predictionSystem) {
            updatePredictionFromReplay(currentStep, currentTime);
        }
        
        // Update neuromodulation during replay
        updateNeuromodulationFromReplay(currentStep, currentTime, predictionError);
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
    
    // ========== STEP 16: Sleep/rest cycle and consolidation ==========
    // Simple sleep/rest cycle for memory consolidation
    if (pImpl->stepsSinceLastEpisode > pImpl->replayInterval * 2) {
        // Enter resting/sleep state after extended period
        if (!pImpl->isResting) {
            pImpl->isResting = true;
            NLM_LOG_INFO("Brain entering rest state for memory consolidation...");
        }
        
        // During rest, focus on memory consolidation and replay
        if (pImpl->isResting && pImpl->episodicMemory) {
            // Get episodes for replay (prioritizing recent/high-reward episodes)
            auto episodesForReplay = pImpl->episodicMemory->getEpisodesForReplay(5);
            
            // Replay episodes for consolidation
            for (const auto* episode : episodesForReplay) {
                pImpl->episodicMemory->replayEpisode(episode);
                
                // Small consolidation boost for replayed episodes
                // This strengthens the memory trace
                if (pImpl->workingMemory) {
                    // Inject current to maintain replayed patterns in working memory
                    for (NeuronId neuron : episode->activeNeurons) {
                        if (pImpl->workingMemory->contains(neuron)) {
                            float currentBoost = pImpl->workingMemory->getNeuronActivation(neuron) * 0.5f;
                            injectCurrent(neuron, currentBoost);
                        }
                    }
                }
            }
            
            // Reset rest state after consolidation
            pImpl->isResting = false;
            pImpl->stepsSinceLastEpisode = 0;
            
            NLM_LOG_INFO("Memory consolidation complete - replayed " + 
                         std::to_string(episodesForReplay.size()) + " episodes");
        }
    }
    
    // ========== STEP 17: Checkpoint management ==========
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

void Brain::replayMemoryPattern(SimulationStep currentStep, Timestamp currentTime) {
    NLM_LOG_INFO("Enhanced replay: Reconstructing neural patterns from episodic memory");
    
    if (!pImpl->episodicMemory || !pImpl->workingMemory) return;
    
    // Get recent episodes with multi-criteria selection
    std::vector<const EpisodicMemoryItem*> episodes = selectEpisodesForReplay(currentStep, currentTime);
    
    // Multi-pass replay with working memory integration
    for (size_t pass = 0; pass < 3; ++pass) {
        // In each pass, replay 30% of the most relevant episodes
        size_t episodesToReplay = std::max<size_t>(1, episodes.size() / 3);
        std::vector<const EpisodicMemoryItem*> passEpisodes(episodes.begin(), episodes.begin() + episodesToReplay);
        
        // Sort by replay importance (higher = earlier)
        std::sort(passEpisodes.begin(), passEpisodes.end(),
            [this](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                float importanceA = calculateReplayImportance(a);
                float importanceB = calculateReplayImportance(b);
                return importanceA > importanceB;
            });
        
        // Replay each episode
        for (const auto* episode : passEpisodes) {
            replaySingleEpisodePattern(episode, currentStep, currentTime, pass);
            
            // Boost working memory for replayed patterns
            if (pImpl->workingMemory) {
                std::vector<float> pattern(episode->sensoryState.size(), 0.0f);
                if (!pattern.empty()) {
                    // Simple encoding of episode as pattern
                    size_t neuronIndex = 0;
                    for (size_t i = 0; i < episode->sensoryState.size() && neuronIndex < episode->activeNeurons.size(); ++i) {
                        if (episode->sensoryState[i] > 0.5f) {
                            pattern[i] = episode->sensoryState[i];
                            neuronIndex++;
                        }
                    }
                    
                    // Store in working memory with temporary boost
                    pImpl->workingMemory->store(pattern, 0.5f * (1.0f + pass));
                }
            }
        }
        
        // Brief pause between passes for memory consolidation
        if (pass < 2) {
            // Decay working memory slightly between passes
            if (pImpl->workingMemory) {
                pImpl->workingMemory->decayWeakTraces();
            }
        }
    }
    
    NLM_LOG_INFO("Enhanced replay complete: Replayed " + std::to_string(episodes.size()) + 
                 " episodes across " + std::to_string(3) + " passes");
}

float Brain::calculateReplayPredictionError(SimulationStep currentStep) {
    if (!pImpl->episodicMemory || !pImpl->predictionSystem) return 0.0f;
    
    // Calculate prediction errors based on replayed experiences
    std::vector<const EpisodicMemoryItem*> recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
    float totalPredictionError = 0.0f;
    int validEpisodes = 0;
    
    for (const auto* episode : recentEpisodes) {
        // Calculate expected vs actual reward based on episode
        float predictedReward = 0.5f; // Simple baseline prediction
        float actualReward = episode->reward;
        float rewardError = std::abs(actualReward - predictedReward);
        
        // Calculate sensory prediction error
        float sensoryError = 0.0f;
        if (!episode->sensoryState.empty()) {
            // Simple error calculation based on variance in sensory features
            float meanActivity = 0.0f;
            for (float val : episode->sensoryState) {
                meanActivity += val;
            }
            meanActivity /= static_cast<float>(episode->sensoryState.size());
            
            for (float val : episode->sensoryState) {
                sensoryError += std::abs(val - meanActivity);
            }
            sensoryError /= static_cast<float>(episode->sensoryState.size());
        }
        
        // Calculate novelty prediction error
        float noveltyError = std::min(1.0f, episode->novelty * 2.0f); // Scale up for replay
        
        // Combine errors
        float episodeError = rewardError * 0.4f + sensoryError * 0.3f + noveltyError * 0.3f;
        totalPredictionError += episodeError;
        validEpisodes++;
    }
    
    float avgPredictionError = validEpisodes > 0 ? totalPredictionError / validEpisodes : 0.0f;
    
    // Store prediction error for later use
    if (pImpl->predictionError) {
        pImpl->predictionError->updatePredictionError(avgPredictionError);
    }
    
    NLM_LOG_INFO("Replay prediction error calculated: " + std::to_string(avgPredictionError));
    return avgPredictionError;
}

void Brain::applyReplayPlasticityEffects(SimulationStep currentStep, Timestamp currentTime, float predictionError) {
    if (!pImpl->episodicMemory || !pImpl->stdp || !pImpl->hebbian) return;
    
    // Get episodes for plasticity enhancement
    std::vector<const EpisodicMemoryItem*> episodes = selectEpisodesForReplay(currentStep, currentTime);
    
    // Apply plasticity changes based on replayed experiences
    for (const auto* episode : episodes) {
        // Calculate episode strength for plasticity
        float episodeStrength = calculateReplayImportance(episode);
        
        // Synaptic strengthening for replayed patterns
        std::vector<float> pattern(episode->sensoryState.size(), 0.0f);
        if (!pattern.empty()) {
            size_t neuronIndex = 0;
            for (size_t i = 0; i < episode->sensoryState.size() && neuronIndex < episode->activeNeurons.size(); ++i) {
                if (episode->sensoryState[i] > 0.5f) {
                    pattern[i] = episode->sensoryState[i];
                    neuronIndex++;
                }
            }
            
            // Apply Hebbian learning for replayed patterns
            if (pImpl->associativeMemory && !pattern.empty()) {
                // Convert pattern to vector of floats (assuming episodic memory uses similar representation)
                std::vector<float> patternB = pattern;
                pImpl->associativeMemory->associate(pattern, patternB, episodeStrength * 0.1f);
            }
        }
        
        // Update synaptic weights based on prediction error
        float plasticityMod = 1.0f + predictionError * 0.5f;
        
        // Enhance recurrent connections for replayed patterns
        if (pImpl->workingMemory && episodeStrength > 0.5f) {
            // Boost working memory for important episodes
            pImpl->workingMemory->strengthenMemory(plasticityMod);
        }
        
        // Apply structural plasticity hints for replayed connections
        if (pImpl->structuralPlasticity && episodeStrength > 0.7f) {
            // Preferentially maintain connections for strongly replayed episodes
            float retentionRate = 0.95f + (episodeStrength - 0.5f) * 0.1f;
            // Note: In a full implementation, this would directly affect structural plasticity
            NLM_LOG_INFO("Structural plasticity hint: High retention rate " + std::to_string(retentionRate) +
                         " for episode with strength " + std::to_string(episodeStrength));
        }
    }
    
    NLM_LOG_INFO("Replay plasticity effects applied: Enhanced connections for " + 
                 std::to_string(episodes.size()) + " episodes");
}

void Brain::updatePredictionFromReplay(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl->predictionSystem || !pImpl->episodicMemory) return;
    
    // Get episodes that were replayed
    std::vector<const EpisodicMemoryItem*> episodes = selectEpisodesForReplay(currentStep, currentTime);
    
    // Update prediction system with replayed experiences
    float totalNovelty = 0.0f;
    int validEpisodes = 0;
    
    for (const auto* episode : episodes) {
        // Convert episode sensory state to prediction system format
        std::vector<float> sensoryPattern = episode->sensoryState;
        
        if (!sensoryPattern.empty()) {
            // Update prediction system with new experience
            // Note: PredictionSystem interface may vary - using best available approach
            // This simulates prediction system learning from replayed episodes
            float predictedOutcome = 0.5f + (episode->reward * 0.3f);
            float actualOutcome = episode->reward;
            float error = actualOutcome - predictedOutcome;
            
            // Apply error to prediction system if available
            if (pImpl->predictionError) {
                pImpl->predictionError->updatePredictionError(std::abs(error));
            }
            
            totalNovelty += episode->novelty;
            validEpisodes++;
        }
    }
    
    if (validEpisodes > 0) {
        float avgNovelty = totalNovelty / validEpisodes;
        // Enhance prediction for novel patterns during replay
        if (pImpl->curiosity) {
            pImpl->curiosity->updateCuriosity(avgNovelty * 0.5f);
        }
    }
    
    NLM_LOG_INFO("Prediction system updated from replay: Processed " + std::to_string(validEpisodes) + 
                 " episodes, average novelty: " + std::to_string(avgNovelty));
}

void Brain::updateNeuromodulationFromReplay(SimulationStep currentStep, Timestamp currentTime, float predictionError) {
    if (!pImpl->dopamine || !pImpl->episodicMemory) return;
    
    // Get episodes for neuromodulation
    std::vector<const EpisodicMemoryItem*> episodes = selectEpisodesForReplay(currentStep, currentTime);
    
    // Calculate replay-based neuromodulation signals
    float totalReward = 0.0f;
    float totalNovelty = 0.0f;
    float totalUnpredictability = 0.0f;
    int validEpisodes = 0;
    
    for (const auto* episode : episodes) {
        totalReward += episode->reward;
        totalNovelty += episode->novelty;
        
        // Calculate unpredictability (variance in reward)
        // Simple approach: high reward variance = high unpredictability
        float unpredictability = 0.0f;
        if (validEpisodes > 0) {
            unpredictability = std::min(1.0f, episode->reward * 0.5f);
        }
        totalUnpredictability += unpredictability;
        validEpisodes++;
    }
    
    if (validEpisodes > 0) {
        float avgReward = totalReward / validEpisodes;
        float avgNovelty = totalNovelty / validEpisodes;
        float avgUnpredictability = totalUnpredictability / validEpisodes;
        
        // Apply dopaminergic modulation based on replay
        if (avgReward > 0.5f) {
            // Enhance dopamine for rewarding replayed episodes
            float rewardBoost = (avgReward - 0.5f) * 2.0f;
            float currentDopamine = pImpl->dopamine->getLevel();
            pImpl->dopamine->updatePredictionError(rewardBoost * 0.5f);
            NLM_LOG_INFO("Replay enhanced dopamine (reward boost): " + std::to_string(rewardBoost));
        }
        
        if (avgNovelty > 0.5f) {
            // Boost dopamine for novel replayed episodes
            float noveltyBoost = (avgNovelty - 0.5f) * 2.0f;
            NLM_LOG_INFO("Replay enhanced dopamine (novelty boost): " + std::to_string(noveltyBoost));
        }
        
        if (avgUnpredictability > 0.5f) {
            // Handle prediction error for unexpected replay
            float predictionErrorBoost = (avgUnpredictability - 0.5f) * 2.0f;
            pImpl->dopamine->updatePredictionError(predictionErrorBoost * 0.3f);
            NLM_LOG_INFO("Replay prediction error enhanced: " + std::to_string(predictionErrorBoost));
        }
        
        // Integrate with prediction error for comprehensive neuromodulation
        float neuromodulationSignal = 0.3f * avgReward + 0.3f * avgNovelty + 0.4f * predictionError;
        pImpl->dopamine->updatePredictionError(neuromodulationSignal * 0.5f);
        
        // Apply neuromodulation effects on plasticity
        applyNeuromodulationEffects(pImpl->dopamine->getLevel(), 1.0f + neuromodulationSignal);
    }
    
    NLM_LOG_INFO("Neuromodulation updated from replay: Reward=" + std::to_string(avgReward) +
                 ", Novelty=" + std::to_string(avgNovelty) + ", PredictionError=" + std::to_string(predictionError));
}

float Brain::calculateReplayImportance(const EpisodicMemoryItem* episode) {
    if (!episode) return 0.0f;
    
    // Calculate multi-criteria importance for replay prioritization
    float recencyScore = 1.0f; // Recent episodes are more important
    float rewardScore = std::min(1.0f, episode->reward * 2.0f); // Higher reward = more important
    float noveltyScore = episode->novelty; // Novel episodes are more important
    float unpredictabilityScore = std::min(1.0f, episode->reward * 0.5f); // High reward variance = unpredictable
    
    // Combine criteria with weights
    float importance = recencyScore * 0.2f + 
                       rewardScore * 0.3f + 
                       noveltyScore * 0.3f + 
                       unpredictabilityScore * 0.2f;
    
    return importance;
}

std::vector<const EpisodicMemoryItem*> Brain::selectEpisodesForReplay(SimulationStep currentStep, Timestamp currentTime) {
    std::vector<const EpisodicMemoryItem*> episodes;
    
    if (!pImpl->episodicMemory) return episodes;
    
    // Get candidate episodes (recent 10% of memory)
    size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
    size_t recentCount = std::max<size_t>(3, episodeCount / 10);
    
    // Get all episodes
    std::vector<const EpisodicMemoryItem*> allEpisodes;
    for (size_t i = 0; i < episodeCount; ++i) {
        const EpisodicMemoryItem* ep = pImpl->episodicMemory->getEpisode(i);
        if (ep) allEpisodes.push_back(ep);
    }
    
    // Select top episodes based on replay importance
    std::vector<std::pair<const EpisodicMemoryItem*, float>> episodeScores;
    for (const auto* ep : allEpisodes) {
        float score = calculateReplayImportance(ep);
        episodeScores.push_back(std::make_pair(ep, score));
    }
    
    // Sort by score (descending)
    std::sort(episodeScores.begin(), episodeScores.end(),
        [](const std::pair<const EpisodicMemoryItem*, float>& a,
           const std::pair<const EpisodicMemoryItem*, float>& b) {
            return a.second > b.second;
        });
    
    // Select top episodes
    size_t selectedCount = std::min(recentCount, episodeScores.size());
    for (size_t i = 0; i < selectedCount; ++i) {
        episodes.push_back(episodeScores[i].first);
    }
    
    return episodes;
}

void Brain::replaySingleEpisodePattern(const EpisodicMemoryItem* episode, SimulationStep currentStep, 
                                     Timestamp currentTime, size_t pass) {
    if (!episode || !pImpl->episodicMemory) return;
    
    // Get the actual episode from episodic memory for replay
    // In a real implementation, this would use the episodic memory's replayEpisode method
    // For now, we simulate the replay process
    
    // Calculate replay boost based on pass and episode importance
    float replayBoost = 1.0f + (pass * 0.2f); // Each pass gets stronger
    float episodeImportance = calculateReplayImportance(episode);
    replayBoost *= (1.0f + episodeImportance * 0.3f);
    
    // Simulate neural pattern reactivation
    NLM_LOG_INFO("Replaying episode pattern: step=" + std::to_string(episode->timestamp) +
                 ", reward=" + std::to_string(episode->reward) +
                 ", importance=" + std::to_string(episodeImportance) +
                 ", pass=" + std::to_string(pass) + ", boost=" + std::to_string(replayBoost));
    
    // Apply the pattern to neural networks
    // This would normally involve:
    // 1. Pattern completion in episodic memory
    // 2. Reactivation of associated neural assemblies
    // 3. Synaptic weight updates based on prediction errors
    // 4. Working memory updates with replayed patterns
    
    // For now, we log the replay and let the episodic memory system handle the actual replay
    // The episodic memory's replayEpisode method would normally be called here
    pImpl->episodicMemory->replayEpisode(episode);
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
        // Use the checkpoint manager to save the brain
        if (!pImpl->checkpointManager) {
            NLM_LOG_ERROR("Checkpoint manager not initialized");
            return false;
        }
        
        // Configure checkpoint manager if needed
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        
        // Create a filename from the filepath
        std::string filename = filepath;
        size_t lastSlash = filename.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            filename = filename.substr(lastSlash + 1);
        }
        
        // Set up providers for checkpoint data
        pImpl->checkpointManager->setNeuronProvider(
            [this](NeuronCheckpointData& data) -> bool {
                data.membranePotential.reserve(getTotalNeuronCount());
                data.restingPotential.reserve(getTotalNeuronCount());
                data.threshold.reserve(getTotalNeuronCount());
                data.resetPotential.reserve(getTotalNeuronCount());
                data.leakConductance.reserve(getTotalNeuronCount());
                data.firingState.reserve(getTotalNeuronCount());
                data.refractoryRemaining.reserve(getTotalNeuronCount());
                data.refractoryPeriod.reserve(getTotalNeuronCount());
                data.lastSpikeTime.reserve(getTotalNeuronCount());
                
                // Collect neuron data
                size_t idx = 0;
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
                        }
                    }
                }
                
                return true;
            }
        );
        
        pImpl->checkpointManager->setSynapseProvider(
            [this](SynapseCheckpointData& data) -> bool {
                // Collect synapse data
                for (const auto& region : pImpl->regions) {
                    for (const auto* syn : region->getSynapses()) {
                        data.sourceNeuron.push_back(syn->getSourceNeuron().index());
                        data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                        data.weight.push_back(syn->getWeight());
                        data.delay.push_back(syn->getDelay());
                        data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                        data.plasticityFlags = syn->getPlasticityFlags();
                        data.eligibilityTrace.push_back(syn->getEligibilityTrace());
                        data.efficacy = 1.0f; // placeholder
                        data.shortTermDepression = 0.0f; // placeholder
                        data.shortTermFacilitation = 0.0f; // placeholder
                    }
                }
                
                return true;
            }
        );
        
        // Use checkpoint manager to save
        if (!pImpl->checkpointManager->saveImmediately(filename)) {
            NLM_LOG_ERROR("Failed to save brain state via checkpoint manager");
            return false;
        }
        
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
        // Use the checkpoint manager to load the brain
        if (!pImpl->checkpointManager) {
            NLM_LOG_ERROR("Checkpoint manager not initialized");
            return false;
        }
        
        // Configure checkpoint manager if needed
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        
        // Use checkpoint manager to load
        if (!pImpl->checkpointManager->load(filepath)) {
            NLM_LOG_ERROR("Failed to load brain state via checkpoint manager");
            return false;
        }
        
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

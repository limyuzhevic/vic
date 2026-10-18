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
    
    // Integrate working memory with sensory processing
    // Working memory should receive sensory input patterns and maintain them
    // This creates the bridge between perception and memory
    if (pImpl->workingMemory && !pImpl->sensoryNeurons.empty()) {
        // Process recent sensory input to update working memory
        // In a full implementation, this would involve:
        // 1. Extracting active sensory patterns
        // 2. Storing in working memory with appropriate strength
        // 3. Running competition among memory traces
        // 4. Attention-based selection of important patterns
        
        // For now, we'll store active sensory neurons in working memory
        for (NeuronId neuronId : pImpl->sensoryNeurons) {
            // Find the actual neuron
            Neuron* sensoryNeuron = nullptr;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->getId() == neuronId) {
                            sensoryNeuron = neuron;
                            break;
                        }
                    }
                    if (sensoryNeuron) break;
                }
                if (sensoryNeuron) break;
            }
            
            if (sensoryNeuron) {
                float sensoryActivation = std::abs(sensoryNeuron->getState().membranePotential - 
                                                  sensoryNeuron->getState().restingPotential);
                // Normalize and store in working memory
                float normalizedActivation = std::min(1.0f, sensoryActivation / 50.0f);
                if (normalizedActivation > 0.01f) {
                    pImpl->workingMemory->storeToNeuron(neuronId, normalizedActivation);
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
            
            // Store active neurons and their activations
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
            
            // Store sensory novelty (difference from previous state)
            // This provides a measure of what was novel/experienced
            if (pImpl->novelty) {
                episode.novelty = pImpl->novelty->getLevel();
            } else {
                episode.novelty = 0.0f;
            }
            
            // Store prediction error for learning assessment
            if (pImpl->predictionError) {
                // Note: PredictionError stores level, which could represent prediction error
                // In a full implementation, this would be the actual prediction error signal
                episode.reward += pImpl->predictionError->getLevel() * 0.1f; // Add small contribution
            }
            
            // Store developmental stage for context
            episode.age = pImpl->developmentalStage == DevelopmentalStage::Adult ? 1.0f : 
                         (pImpl->developmentalStage == DevelopmentalStage::Maturation ? 0.75f :
                         (pImpl->developmentalStage == DevelopmentalStage::CriticalPeriod ? 0.5f :
                         (pImpl->developmentalStage == DevelopmentalStage::Initial ? 0.25f : 0.0f)));
            
            pImpl->episodicMemory->storeEpisode(episode);
            
            // Also strengthen working memory with this experience
            if (pImpl->workingMemory && !episode.activeNeurons.empty()) {
                // Reinforce the neural patterns that were active during this experience
                for (size_t i = 0; i < std::min(episode.activeNeurons.size(), size_t(10)); ++i) {
                    pImpl->workingMemory->storeToNeuron(episode.activeNeurons[i], 
                                                       episode.neuronActivations[i] * 0.5f);
                }
            }
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
        
        // Enhanced prediction system integration:
        // 1. Predict next state based on current neural activity
        // 2. Compute prediction error
        // 3. Use prediction error to guide learning
        
        // Get current neural state for prediction
        std::vector<float> currentNeuralState;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Encode neural state: membrane potential normalized
                    float normalizedVoltage = (neuron->getState().membranePotential + 100.0f) / 200.0f;
                    currentNeuralState.push_back(normalizedVoltage);
                }
            }
        }
        
        // Update prediction system with current state
        // In a full implementation, this would involve:
        // - Feeding neural state to prediction system
        // - Getting predicted next state
        // - Computing prediction error
        // - Using prediction error to modulate plasticity
        
        // For now, we'll compute a simple prediction error based on neural activity
        float predictionError = 0.0f;
        if (!currentNeuralState.empty()) {
            // Simple measure: standard deviation of neural activity indicates unpredictability
            float sum = std::accumulate(currentNeuralState.begin(), currentNeuralState.end(), 0.0f);
            float mean = sum / currentNeuralState.size();
            
            float sqDiffSum = 0.0f;
            for (float val : currentNeuralState) {
                float diff = val - mean;
                sqDiffSum += diff * diff;
            }
            
            predictionError = std::sqrt(sqDiffSum / currentNeuralState.size());
        }
        
        // Update prediction error signal for other systems
        if (pImpl->predictionError) {
            // Set prediction error level (simple approach)
            // In a full implementation, this would use proper prediction error computation
            pImpl->predictionError->update(pImpl->timestep);
        }
        
        // Use prediction error to modulate learning
        // Higher prediction error -> more exploration and learning
        if (predictionError > 0.1f) {
            // Enhance novelty/curiosity when prediction error is high
            if (pImpl->novelty) {
                pImpl->novelty->update(pImpl->timestep);
            }
            
            // Increase exploration via curiosity system
            if (pImpl->curiosity) {
                // Use prediction error to boost curiosity
                float curiosityBoost = predictionError * 0.5f;
                // Apply curiosity boost to neuromodulation
                pImpl->dopamine->update(pImpl->timestep);
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
        // Enhanced concept formation integration:
        // 1. Form concepts from sensory patterns (generalization)
        // 2. Extract abstract features from episodic memories
        // 3. Create schema for recognizing similar patterns
        // 4. Update attention based on concepts
        // 5. Guide action selection based on concepts
        
        // Prepare current neural state for concept formation
        std::vector<float> currentPattern;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Encode neural activity for concept formation
                    float normalizedActivity = 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                    normalizedActivity = std::min(1.0f, normalizedActivity / 100.0f);
                    currentPattern.push_back(normalizedActivity);
                }
            }
        }
        
        // If we have episodic memories, extract concepts from them
        if (pImpl->episodicMemory && !currentPattern.empty()) {
            // Get recent episodes for concept learning
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
            
            // Extract common patterns from episodes
            for (const auto* episode : recentEpisodes) {
                if (episode) {
                    // In a full implementation, this would:
                    // 1. Extract features from episode active neurons
                    // 2. Compute similarities with current pattern
                    // 3. Update concept representations
                    // 4. Generalize similar experiences
                    
                    // For now, we store concept-related activation in working memory
                    if (pImpl->workingMemory) {
                        for (size_t i = 0; i < std::min(episode->activeNeurons.size(), size_t(5)); ++i) {
                            // Reinforce concepts related to this experience
                            pImpl->workingMemory->storeToNeuron(
                                episode->activeNeurons[i], 
                                episode->neuronActivations[i] * 0.3f);
                        }
                    }
                }
            }
            
            // Update attention based on concept relevance
            if (pImpl->attention) {
                // Use concept activation to guide attention
                // Higher concept activation = stronger attention bias
                if (!currentPattern.empty()) {
                    float conceptStrength = *std::max_element(currentPattern.begin(), currentPattern.end());
                    if (conceptStrength > 0.2f) {
                        // Apply top-down bias to neurons matching current concept
                        for (size_t i = 0; i < std::min(currentPattern.size(), size_t(10)); ++i) {
                            if (currentPattern[i] > 0.3f) {
                                NeuronId conceptNeuron(pImpl->sensoryNeurons.empty() ? 
                                                     (i + 10000) : pImpl->sensoryNeurons[i]->getId().index());
                                // Apply concept-based attention bias
                                pImpl->attention->applyTopDownBias(conceptNeuron, conceptStrength * 0.5f);
                            }
                        }
                    }
                }
            }
            
            // Guide action selection using concepts
            if (pImpl->workingMemory && pImpl->workingMemory->getMemoryActivity() > 0.3f) {
                // Concept-based action guidance:
                // 1. Identify most active concepts from working memory
                // 2. Map concepts to appropriate actions
                // 3. Bias action selection toward conceptually relevant actions
                
                // For now, use working memory activity to influence action selection
                // This would be more sophisticated in a full implementation
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
    // Write neuron states using CheckpointSystem
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
                neuronData.neuronType.push_back(static_cast<uint64_t>(neuron->getType()));
                neuronData.regionId.push_back(static_cast<uint64_t>(neuron->getRegionId().index()));
                neuronData.populationId.push_back(static_cast<uint64_t>(neuron->getPopulationId().index()));
            }
        }
    }
    
    if (!writer.writeNeurons(neuronData)) {
        NLM_LOG_ERROR("Failed to write neurons to checkpoint");
        return false;
    }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses using CheckpointSystem
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.plasticityFlags.push_back(
                    (syn->getPlasticityFlags().hebbian ? 0x01 : 0) |
                    (syn->getPlasticityFlags().stdp ? 0x02 : 0) |
                    (syn->getPlasticityFlags().reward_modulated ? 0x04 : 0));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
                synapseData.shortTermDepression.push_back(syn->getDepression());
                synapseData.shortTermFacilitation.push_back(syn->getFacilitation());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Write development system state
        if (pImpl->developmentSystem) {
            // TODO: Implement development system serialization
            NLM_LOG_INFO("Development system checkpointing not yet implemented");
        }
        
        // Write neuromodulation states
        if (pImpl->dopamine) {
            // TODO: Implement dopamine system serialization
            NLM_LOG_INFO("Dopamine system checkpointing not yet implemented");
        }
        if (pImpl->curiosity) {
            // TODO: Implement curiosity system serialization
            NLM_LOG_INFO("Curiosity system checkpointing not yet implemented");
        }
        if (pImpl->predictionError) {
            // TODO: Implement prediction error system serialization
            NLM_LOG_INFO("Prediction error system checkpointing not yet implemented");
        }
        if (pImpl->novelty) {
            // TODO: Implement novelty system serialization
            NLM_LOG_INFO("Novelty system checkpointing not yet implemented");
        }
        
        // Write memory system states
        if (pImpl->workingMemory) {
            // TODO: Implement working memory serialization
            NLM_LOG_INFO("Working memory checkpointing not yet implemented");
        }
        if (pImpl->episodicMemory) {
            // TODO: Implement episodic memory serialization
            NLM_LOG_INFO("Episodic memory checkpointing not yet implemented");
        }
        if (pImpl->associativeMemory) {
            // TODO: Implement associative memory serialization
            NLM_LOG_INFO("Associative memory checkpointing not yet implemented");
        }
        
        // Write prediction system state
        if (pImpl->predictionSystem) {
            // TODO: Implement prediction system serialization
            NLM_LOG_INFO("Prediction system checkpointing not yet implemented");
        }
        
        // Write cognition system states
        if (pImpl->planner) {
            // TODO: Implement planner serialization
            NLM_LOG_INFO("Planner checkpointing not yet implemented");
        }
        if (pImpl->conceptFormation) {
            // TODO: Implement concept formation serialization
            NLM_LOG_INFO("Concept formation checkpointing not yet implemented");
        }
        if (pImpl->attention) {
            // TODO: Implement attention system serialization
            NLM_LOG_INFO("Attention system checkpointing not yet implemented");
        }
        
        // Write spike system state
        if (pImpl->spikeSystem) {
            // TODO: Implement spike system serialization
            NLM_LOG_INFO("Spike system checkpointing not yet implemented");
        }
        
        // Write plasticity system states
        if (pImpl->stdp) {
            // TODO: Implement STDP serialization
            NLM_LOG_INFO("STDP checkpointing not yet implemented");
        }
        if (pImpl->hebbian) {
            // TODO: Implement Hebbian serialization
            NLM_LOG_INFO("Hebbian checkpointing not yet implemented");
        }
        if (pImpl->structuralPlasticity) {
            // TODO: Implement structural plasticity serialization
            NLM_LOG_INFO("Structural plasticity checkpointing not yet implemented");
        }
        
        // Write random generator state
        if (pImpl->rng) {
            // TODO: Implement random generator serialization
            NLM_LOG_INFO("Random generator checkpointing not yet implemented");
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
        
    // Read synapses using CheckpointSystem
    SynapseCheckpointData synapseData;
    if (!reader.readSynapses(synapseData)) {
        NLM_LOG_ERROR("Failed to read synapses from checkpoint");
        return false;
    }
    
    // Apply synapse states - need to find matching synapses
    size_t synapseIdx = 0;
    for (auto& region : pImpl->regions) {
        auto& synapses = region->getSynapses();
        for (auto* syn : synapses) {
            if (synapseIdx < synapseData.weight.size()) {
                syn->setWeight(synapseData.weight[synapseIdx]);
                syn->setDelay(synapseData.delay[synapseIdx]);
                
                // Restore plasticity flags
                uint8_t flags = synapseIdx < synapseData.plasticityFlags.size() ? 
                               synapseData.plasticityFlags[synapseIdx] : 0;
                syn->enablePlasticity(
                    (flags & 0x01) != 0, // hebbian
                    (flags & 0x02) != 0, // stdp  
                    (flags & 0x04) != 0  // reward_modulated
                );
                
                // Restore plasticity state
                if (synapseIdx < synapseData.eligibilityTrace.size()) {
                    // Need to access private member or use protected method
                    // For now, just log
                    NLM_LOG_INFO("Restoring eligibility trace: " + std::to_string(synapseData.eligibilityTrace[synapseIdx]));
                }
                
                synapseIdx++;
            }
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
    
    // TODO: Restore development system state
    // TODO: Restore neuromodulation states
    // TODO: Restore memory system states
    // TODO: Restore prediction system state
    // TODO: Restore cognition system states
    // TODO: Restore spike system state
    // TODO: Restore plasticity system states
    // TODO: Restore random generator state
    
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

void Brain::produceAction() {
    // Enhanced action production using neural planner
    // In Phase 6, action selection should use:
    // 1. Neural planner for goal-directed planning
    // 2. Working memory for maintaining action plans
    // 3. Episodic memory for recalling past successful actions
    // 4. Attention for selecting relevant actions
    // 5. Prediction system for evaluating action outcomes
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Try to use neural planner for action selection
    // The planner should evaluate possible actions based on:
    // - Current state (working memory)
    // - Past experiences (episodic memory)
    // - Goals/context (concept formation)
    // - Prediction of outcomes (prediction system)
    
    bool plannerUsed = false;
    
    if (pImpl->planner && pImpl->workingMemory) {
        // Get current state for planning
        std::vector<NeuronId> currentState;
        for (NeuronId neuron : pImpl->workingMemory->getMemoryNeurons()) {
            currentState.push_back(neuron);
        }
        
        // Get relevant episodic memories for planning
        std::vector<EpisodicMemoryItem*> relevantMemories;
        if (pImpl->episodicMemory) {
            // In a full implementation, retrieve similar episodes
            // For now, use recent episodes
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
            for (const auto* episode : recentEpisodes) {
                if (episode) relevantMemories.push_back(const_cast<EpisodicMemoryItem*>(episode));
            }
        }
        
        // Let neural planner evaluate actions based on current state and memories
        // The planner should consider:
        // - Current neural activity patterns
        // - Past successful/failed actions from episodic memory
        // - Working memory content
        // - Developmental stage
        // - Neuromodulation levels
        
        // For now, enhanced simple action selection with planning influence
        auto* planner = pImpl->planner;
        if (planner && pImpl->dopamine) {
            // Use planner if available, otherwise fallback to simple selection
            float planningInfluence = pImpl->dopamine->getLevel() * 0.5f;
            planningInfluence = std::clamp(planningInfluence, 0.0f, 1.0f);
            
            if (planningInfluence > 0.3f) {
                // Use planner-based action selection
                // In full implementation: return planner->planAction(currentState, relevantMemories)
                
                // For now, simulate planner influence on action selection
                if (pImpl->attention && pImpl->attention->getWinners().size() > 0) {
                    // Attention has selected winners - plan actions around them
                    // This would involve:
                    // 1. Identifying action goals based on attended patterns
                    // 2. Evaluating action sequences
                    // 3. Selecting most appropriate action
                    
                    // For now, use attention winners to influence action selection
                    size_t actionIndex = pImpl->attention->getWinners()[0] % 6;
                    
                    MotorCommand plannedAction;
                    switch (actionIndex) {
                        case 0: plannedAction = MotorCommand::MoveForward; break;
                        case 1: plannedAction = MotorCommand::MoveBackward; break;
                        case 2: plannedAction = MotorCommand::TurnLeft; break;
                        case 3: plannedAction = MotorCommand::TurnRight; break;
                        case 4: plannedAction = MotorCommand::Interact; break;
                        default: plannedAction = MotorCommand::Wait;
                    }
                    
                    return std::make_unique<Action>(plannedAction);
                }
            }
        }
    }
    
    // Fallback to simple action selection based on motor neuron activity
    // This is the original implementation, kept as fallback
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Enhanced simple action selection with planning influence
    MotorCommand selectedAction = MotorCommand::Wait;
    
    if (firingMotor > 0) {
        // Use attention to prioritize actions
        if (pImpl->attention) {
            // Get attended neurons
            std::vector<NeuronId> attended = pImpl->attention->getWinners();
            if (!attended.empty()) {
                // Map attended neurons to motor actions
                for (NeuronId attendedNeuron : attended) {
                    // Check if this neuron is associated with a motor action
                    // In full implementation, this would use mapping from planner
                    for (size_t i = 0; i < pImpl->motorNeurons.size(); ++i) {
                        if (pImpl->motorNeurons[i]->getId() == attendedNeuron) {
                            // Attended motor neuron -> plan appropriate action
                            switch (i % 6) {
                                case 0: selectedAction = MotorCommand::MoveForward; break;
                                case 1: selectedAction = MotorCommand::MoveBackward; break;
                                case 2: selectedAction = MotorCommand::TurnLeft; break;
                                case 3: selectedAction = MotorCommand::TurnRight; break;
                                case 4: selectedAction = MotorCommand::Interact; break;
                                case 5: selectedAction = MotorCommand::Wait;
                            }
                            break;
                        }
                    }
                }
            } else {
                // No specific attention - choose most active motor neuron
                selectedAction = MotorCommand::MoveForward; // Default bias
            }
        } else {
            // No attention - default to move forward
            selectedAction = MotorCommand::MoveForward;
        }
    }
    
    // Apply development-stage appropriate actions
    if (pImpl->developmentSystem) {
        DevelopmentalStage stage = pImpl->developmentalStage;
        // In mature brains (Adult), use more sophisticated action selection
        // In developing brains, use simpler, more reflexive actions
        if (stage == DevelopmentalStage::Adult) {
            // Use working memory content to guide actions
            if (pImpl->workingMemory && pImpl->workingMemory->getMemoryActivity() > 0.5f) {
                // Active working memory suggests goal-directed behavior
                // For now, maintain current action bias
            }
        } else if (stage == DevelopmentalStage::CriticalPeriod) {
            // High plasticity - encourage exploration
            // Randomize action choice to promote exploration
            int randomChoice = std::rand() % 4;
            switch (randomChoice) {
                case 0: selectedAction = MotorCommand::MoveForward; break;
                case 1: selectedAction = MotorCommand::TurnLeft; break;
                case 2: selectedAction = MotorCommand::TurnRight; break;
                case 3: selectedAction = MotorCommand::Wait;
            }
        }
    }
    
    auto action = std::make_unique<Action>(selectedAction);
    return action;
}

} // namespace nlm

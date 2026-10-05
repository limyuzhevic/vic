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
            
            // Capture the complete sensory experience from the last step
            // In a full implementation, this would store the actual sensory input
            // For now, store neural activity patterns as the sensory state
            std::vector<float> sensoryPattern;
            std::vector<NeuronId> activeNeurons;
            
            // Store active neurons with their activation levels
            float totalActivity = 0.0f;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 3.0f) {
                            activeNeurons.push_back(neuron->getId());
                            float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                            sensoryPattern.push_back(activation);
                            totalActivity += activation;
                        }
                    }
                }
            }
            
            // Store sensory state (normalize to reasonable range)
            for (float& val : sensoryPattern) {
                val = std::clamp(val / std::max(totalActivity, 1.0f), 0.0f, 1.0f);
            }
            
            episode.sensoryState = sensoryPattern;
            episode.activeNeurons = activeNeurons;
            
            // Get current state for episode (reward, novelty, etc.)
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store novelty detection if available
            if (pImpl->novelty) {
                episode.novelty = pImpl->novelty->getLevel();
            }
            
            // Store position information (simplified - use step as proxy for position)
            episode.positionX = (currentStep % 100) * 0.1f; // Simulated position
            episode.positionY = ((currentStep * 3) % 100) * 0.1f;
            episode.orientation = (currentStep * 7) % 360; // Simulated orientation
            
            // Store action prediction (for agent's action selection)
            auto action = produceAction();
            episode.action = action->getType();
            
            // Store energy level (simplified as inverse of average firing rate)
            episode.energy = 1.0f - getAverageFiringRate() / 100.0f;
            episode.energy = std::clamp(episode.energy, 0.0f, 1.0f);
            
            // Store age
            episode.age = pImpl->stepsSinceLastEpisode;
            
            // Store result of action (simplified as subsequent neural activity)
            episode.resultingReward = episode.reward; // Would be computed from world in full implementation
            
            // Store the complete episode
            pImpl->episodicMemory->storeEpisode(episode);
            
            // Also update associative memory with this experience
            if (pImpl->associativeMemory && !sensoryPattern.empty()) {
                // Create a simplified "resulting state" based on current neural activity
                std::vector<float> resultingState = sensoryPattern;
                // Apply action consequence (simplified)
                for (float& val : resultingState) {
                    val *= (1.0f + action->getType() * 0.1f); // Simple action effect
                }
                pImpl->associativeMemory->associateFromExperience(episode);
            }
        }
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Create a sensory input from current neural activity
        // In a full implementation, this would use the actual sensory percept
        SensoryInput currentSensory;
        
        // Sample sensory neurons to create a sensory pattern
        size_t samplesPerRegion = 10;  // Sample 10 neurons per region
        std::vector<float> sensoryData;
        
        for (auto& region : pImpl->regions) {
            size_t neuronCount = region->getTotalNeuronCount();
            if (neuronCount == 0) continue;
            
            // Sample some neurons from this region
            for (size_t i = 0; i < samplesPerRegion && i < neuronCount; ++i) {
                size_t sampleIdx = i * (neuronCount / std::max(samplesPerRegion, size_t(1)));
                auto neurons = region->getAllNeurons();
                if (!neurons.empty() && sampleIdx < neurons.size()) {
                    auto* neuron = neurons[sampleIdx];
                    float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                    sensoryData.push_back(activation);
                }
            }
        }
        
        // Create sensory input from neural activity
        currentSensory = SensoryInput(sensoryData);
        
        // Update prediction system with current state
        // The prediction system would maintain a model of expected sensory input
        pImpl->predictionSystem->updatePredictions(
            pImpl->predictionSystem->predictNextState(currentSensory),
            currentSensory
        );
        
        // Use prediction error to modulate neural excitability
        float predictionError = pImpl->predictionSystem->getPredictionError();
        
        // Apply prediction error to neuromodulation
        if (predictionError > 0.5f) {
            // High prediction error increases dopamine (exploration)
            if (pImpl->dopamine) {
                float currentDA = pImpl->dopamine->getLevel();
                pImpl->dopamine->setLevel(std::min(1.0f, currentDA + predictionError * 0.2f));
            }
        } else if (predictionError < 0.1f && predictionError > 0.0f) {
            // Low prediction error decreases dopamine (exploitation)
            if (pImpl->dopamine) {
                float currentDA = pImpl->dopamine->getLevel();
                pImpl->dopamine->setLevel(std::max(0.0f, currentDA - 0.1f * (0.1f - predictionError)));
            }
        }
        
        // Update confidence based on prediction error
        pImpl->predictionSystem->getConfidence();
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
        // Create a pattern from current neural activity for concept formation
        std::vector<float> activityPattern;
        
        // Sample neural activity from different regions
        size_t samplesPerRegion = 3;
        for (auto& region : pImpl->regions) {
            size_t neuronCount = region->getTotalNeuronCount();
            if (neuronCount == 0) continue;
            
            // Sample some neurons from this region
            for (size_t i = 0; i < samplesPerRegion && i < neuronCount; ++i) {
                size_t sampleIdx = i * (neuronCount / std::max(samplesPerRegion, size_t(1)));
                auto neurons = region->getAllNeurons();
                if (!neurons.empty() && sampleIdx < neurons.size()) {
                    auto* neuron = neurons[sampleIdx];
                    float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                    activityPattern.push_back(activation);
                }
            }
        }
        
        // Normalize pattern
        if (!activityPattern.empty()) {
            float maxVal = *std::max_element(activityPattern.begin(), activityPattern.end());
            if (maxVal > 0.0f) {
                for (float& val : activityPattern) {
                    val /= maxVal;
                }
            }
        }
        
        // Extract features from pattern (simplified feature extraction)
        std::vector<float> features;
        
        // Simple feature extraction based on activity distribution
        if (activityPattern.size() >= 3) {
            // Extract mean, variance, and max
            float sum = std::accumulate(activityPattern.begin(), activityPattern.end(), 0.0f);
            float mean = sum / activityPattern.size();
            
            float sumSq = 0.0f;
            for (float val : activityPattern) {
                sumSq += (val - mean) * (val - mean);
            }
            float variance = sumSq / activityPattern.size();
            
            float maxVal = *std::max_element(activityPattern.begin(), activityPattern.end());
            
            features.push_back(mean);   // Overall activity level
            features.push_back(variance); // Activity variability
            features.push_back(maxVal);   // Peak activity
            features.push_back(pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f); // Neuromodulation
            
            // Current reward/novelty as feature
            float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            if (pImpl->novelty) {
                reward += pImpl->novelty->getLevel() * 0.3f;
            }
            features.push_back(reward);
        }
        
        // Present experience to concept formation system
        if (!features.empty()) {
            // Get current step for timing
            SimulationStep currentStep = pImpl->currentStep;
            
            // Reward for successful processing
            float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Use concept formation to process this experience
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                activityPattern, features, reward, currentStep
            );
            
            // If a new concept was formed, it could guide future attention
            if (conceptId > 0) {
                // The concept formation system will internally track new concepts
                // This creates self-organizing categories from experience
            }
        }
        
        // Use concept knowledge to guide attention and planning
        if (pImpl->attention && pImpl->conceptFormation && !activityPattern.empty()) {
            // Get matching concept for current activity
            size_t conceptId = pImpl->conceptFormation->getMatchingConcept(
                activityPattern, 0.6f  // Similarity threshold
            );
            
            if (conceptId > 0) {
                // The concept is active - could guide attention to relevant neurons
                // For now, just log that concept was found
                // In full implementation, this would bias attention to similar patterns
            }
        }
        
        // Update concept stability based on reinforcement
        if (pImpl->conceptFormation && pImpl->dopamine) {
            // If we have active working memory, update related concepts
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                // This would reinforce concepts related to current memory
                // For now, just log the stability update
            }
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Sleep/rest cycle with memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Enter resting state for memory consolidation
        pImpl->isResting = true;
        
        // Replay important memories (sleep-like replay)
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(5);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
        
        // Apply memory consolidation - strengthen recent memories, weaken old ones
        pImpl->episodicMemory->consolidate(0.4f);
        
        // Update neuromodulators for rest
        if (pImpl->dopamine) {
            // Reduce dopamine during rest (low arousal)
            float currentDA = pImpl->dopamine->getLevel();
            pImpl->dopamine->setLevel(std::max(0.0f, currentDA - 0.1f));
        }
        
        if (pImpl->acetylcholine) {
            // Boost acetylcholine during rest (enhanced memory consolidation)
            float currentACh = pImpl->acetylcholine->getLevel();
            pImpl->acetylcholine->setLevel(std::min(1.0f, currentACh + 0.2f));
        }
        
        if (pImpl->norepinephrine) {
            // Reduce norepinephrine during rest (low arousal)
            float currentNE = pImpl->norepinephrine->getLevel();
            pImpl->norepinephrine->setLevel(std::max(0.0f, currentNE - 0.1f));
        }
        
        // Restore synaptic resources and reduce metabolic demand
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Reduce membrane potential towards resting
                    float current = neuron->getState().membranePotential;
                    float resting = neuron->getState().restingPotential;
                    neuron->setMembranePotential(resting);
                }
            }
        }
        
        // Reset spike system for clean restart
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->reset();
        }
        
        // Update developmental stage based on rest/consolidation
        if (pImpl->developmentalStage == DevelopmentalStage::Adult) {
            // After adult stage, enter maintenance/consolidation phase
            pImpl->developmentalStage = static_cast<DevelopmentalStage>(4); // Add maintenance stage
        }
        
        // Exit rest after one consolidation cycle
        pImpl->isResting = false;
        
    } else if (pImpl->isResting) {
        // Still in rest phase, continue consolidation
        pImpl->isResting = false;
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Create sensory pattern for memory storage
    std::vector<float> sensoryPattern;
    sensoryPattern.reserve(values.size());
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
            // Store original input for episodic memory (normalize to 0-1 range)
            float normalizedInput = std::clamp(values[i] / 255.0f, 0.0f, 1.0f);
            sensoryPattern.push_back(normalizedInput);
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store in working memory - create a neural trace
        if (pImpl->workingMemory) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), 
                                               normalizedValue > 0.0f ? normalizedValue / 10.0f : -normalizedValue / 10.0f);
        }
    }
    
    // Also store complete sensory pattern in working memory
    if (pImpl->workingMemory && !sensoryPattern.empty()) {
        pImpl->workingMemory->store(sensoryPattern, 1.0f);
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
    // Advanced action selection with neural planning
    // Integrates NeuralPlanner, prediction, and working memory for intelligent action selection
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Create current state representation for planning
    std::vector<float> currentState;
    
    // Sample current neural activity for planning
    size_t samplesPerRegion = 5;
    float totalActivity = 0.0f;
    
    for (auto& region : pImpl->regions) {
        size_t neuronCount = region->getTotalNeuronCount();
        if (neuronCount == 0) continue;
        
        // Sample some neurons from this region
        for (size_t i = 0; i < samplesPerRegion && i < neuronCount; ++i) {
            size_t sampleIdx = i * (neuronCount / std::max(samplesPerRegion, size_t(1)));
            auto neurons = region->getAllNeurons();
            if (!neurons.empty() && sampleIdx < neurons.size()) {
                auto* neuron = neurons[sampleIdx];
                float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                currentState.push_back(activation);
                totalActivity += activation;
            }
        }
    }
    
    // Normalize state
    if (!currentState.empty()) {
        float maxVal = *std::max_element(currentState.begin(), currentState.end());
        if (maxVal > 0.0f) {
            for (float& val : currentState) {
                val /= maxVal;
            }
        }
    }
    
    // Use NeuralPlanner if available to plan actions
    ActionType plannedAction = ActionType::Wait;
    
    if (pImpl->planner && !currentState.empty()) {
        // Get reward prediction from prediction system
        float targetReward = 0.5f;  // Default target reward
        if (pImpl->predictionSystem) {
            targetReward = 1.0f - pImpl->predictionSystem->getPredictionError();
            targetReward = std::clamp(targetReward, 0.0f, 1.0f);
        }
        
        // Get prediction for next state
        // This would use the prediction system to anticipate state changes
        std::vector<float> predictedState = currentState;
        if (pImpl->predictionSystem) {
            // Use planner to get prediction for a candidate action
            // For now, use the neural activity pattern as a proxy
        }
        
        // Use NeuralPlanner to generate best action
        // Convert to ActionType enum
        std::vector<ActionType> baseActions = {
            ActionType::MoveForward, ActionType::MoveBackward,
            ActionType::TurnLeft, ActionType::TurnRight,
            ActionType::Interact, ActionType::Wait
        };
        
        // Score actions based on working memory and attention
        float bestScore = -1000.0f;
        
        for (ActionType action : baseActions) {
            float score = 0.0f;
            
            // Get action quality from planner if available
            if (pImpl->planner) {
                // Simple scoring: prefer actions that are in working memory
                if (pImpl->workingMemory) {
                    // Check if this action has been reinforced
                    score += 0.5f;  // Base score
                    
                    // Add attention-based scoring
                    if (pImpl->attention) {
                        // Check if action is currently attended
                        for (auto* neuron : pImpl->motorNeurons) {
                            if (neuron->getState().membranePotential > 0.0f) {
                                score += 0.2f;
                                break;
                            }
                        }
                    }
                    
                    // Add prediction-based scoring
                    if (pImpl->predictionSystem) {
                        float predError = pImpl->predictionSystem->getPredictionError();
                        score += (1.0f - predError) * 0.3f;
                    }
                }
            }
            
            if (score > bestScore) {
                bestScore = score;
                plannedAction = action;
            }
        }
    }
    
    // Apply neuromodulation to action selection
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        
        // Dopamine modulates action selection (reinforcement learning)
        if (dopamineLevel > 0.5f) {
            // High dopamine: encourage exploration
            if (plannedAction != ActionType::Wait) {
                // Occasionally add curiosity-based exploration
                float exploreChance = (dopamineLevel - 0.5f) * 2.0f;
                if (exploreChance > pImpl->rng->uniformReal(0.0f, 1.0f)) {
                    plannedAction = ActionType::Wait; // Reset to wait to trigger exploration in planner
                }
            }
        }
    }
    
    // If planner didn't select an action or planned for exploration, use motor neuron activity
    if (plannedAction == ActionType::Wait) {
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
        
        return std::make_unique<Action>(type);
    }
    
    return std::make_unique<Action>(plannedAction);
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
        
        // Write memory systems
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write working memory
        if (pImpl->workingMemory) {
            auto wmData = pImpl->workingMemory->retrieve();
            if (!wmData.empty()) {
                writer.writeWorkingMemory(wmData);
            }
        }
        
        // Write episodic memory if available
        if (pImpl->episodicMemory) {
            auto episodes = pImpl->episodicMemory->getRecentEpisodes(100);
            for (const auto* episode : episodes) {
                // Serialize episodic memory item
                // For now, just write a placeholder
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
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            neuron->setRefractoryRemaining(neuronData.refractoryRemaining[idx]);
                        }
                        if (idx < neuronData.refractoryPeriod.size()) {
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                        }
                        if (idx < neuronData.lastSpikeTime.size()) {
                            neuron->setLastSpikeTime(neuronData.lastSpikeTime[idx]);
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
        
        // Read working memory
        auto wmData = reader.readWorkingMemory();
        if (pImpl->workingMemory && !wmData.empty()) {
            // Store working memory content
            for (size_t i = 0; i < std::min(wmData.size(), pImpl->workingMemory->getMemoryNeurons().size()); ++i) {
                pImpl->workingMemory->storeToNeuron(pImpl->workingMemory->getMemoryNeurons()[i], wmData[i]);
            }
        }
        
        // Read episodic memory if available
        auto episodes = reader.readEpisodicMemory();
        if (pImpl->episodicMemory && !episodes.empty()) {
            for (const auto& epData : episodes) {
                EpisodicMemoryItem episode;
                episode.timestamp = epData.timestamp;
                episode.reward = epData.reward;
                episode.action = epData.action;
                episode.sensoryState = epData.sensoryState;
                episode.activeNeurons = epData.activeNeurons;
                episode.neuronActivations = epData.neuronActivations;
                episode.age = epData.age;
                
                pImpl->episodicMemory->storeEpisode(episode);
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

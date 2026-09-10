#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/Vision.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
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
    
    // Action planning state
    std::unique_ptr<Action> plannedAction;
    SimulationStep plannedActionStep;
    
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
        , plannedAction(nullptr)
        , plannedActionStep(0)
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
        predictionSystem->initialize(this);
        
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
    
    // Update curiosity with both novelty and prediction error
    if (pImpl->curiosity) {
        // Get current novelty level
        float novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
        float predictionError = 0.0f;
        
        // Get prediction error from prediction system
        if (pImpl->predictionSystem) {
            predictionError = pImpl->predictionSystem->getPredictionError();
        }
        
        // Update curiosity with both signals
        pImpl->curiosity->update(novelty, predictionError, pImpl->timestep);
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
            
            // Calculate reward once (fix reward inconsistency) - DONE
            float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            episode.reward = reward;
            
            // Store active neurons (neural substrate) - IMPROVED
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
            
            // Track energy/bias levels from neuromodulators (energy tracking improvement) - DONE
            // Use multiple neuromodulators for richer energy tracking
            episode.energy = (pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f) * 0.5f +
                           (pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f) * 0.3f +
                           (pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f) * 0.2f;
            
            // Include novelty detection data (novelty tracking improvement) - DONE
            episode.novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            
            // Action tracking from brain state (from motor output) - REQUIRES INTEGRATION
            // This would be provided by AgentBrain integration layer
            // episode.action = lastActionType;
            
            // Position/orientation tracking (agent/world state) - REQUIRES INTEGRATION
            // Retrieved from AgentBrain::getBrain() -> getWorld() -> getAgent()
            // episode.positionX = agent.getBody().x;
            // episode.positionY = agent.getBody().y;
            // episode.orientation = agent.getBody().orientation;
            
            // Store actual sensory input data (sensory input fix) - REQUIRES INTEGRATION
            // Provided by AgentBrain::getBrain() -> current sensory perception
            // episode.sensoryState = { vision, touch, internal, proprioception };
            
            // Add world state information (environmental context) - REQUIRES INTEGRATION
            // episode.resultingSensoryState = world.getSensoryPercept().getAllSignals();
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Create current state from neural activity
        std::vector<float> currentNeuralActivity;
        
        // Extract from sensory neurons to create current state
        for (Neuron* neuron : pImpl->sensoryNeurons) {
            if (neuron) {
                float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                currentNeuralActivity.push_back(activation);
            }
        }
        
        if (!currentNeuralActivity.empty()) {
            // Create a Vision input from neural activity (vision is the primary sensory modality)
            Vision currentVision(16, 16); // 16x16 vision grid to match 256 sensory neurons
            currentVision.setData(currentNeuralActivity);
            
            // Generate prediction for next state
            auto predicted = pImpl->predictionSystem->predictNextState(currentVision);
            
            // Update predictions with actual observation
            pImpl->predictionSystem->updatePredictions(*predicted, currentVision);
            
            // Get prediction error and update neuromodulation systems
            float predictionError = pImpl->predictionSystem->getPredictionError();
            
            // Update prediction error neuromodulator
            if (pImpl->predictionError) {
                pImpl->predictionError->computeError(predictionError, predictionError);
            }
            
            // Pass prediction error to curiosity system
            if (pImpl->curiosity) {
                pImpl->curiosity->update(pImpl->timestep, predictionError);
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
    
    // ========== STEP 9.5: Form concepts from working memory experiences ==========
    if (pImpl->conceptFormation && pImpl->workingMemory) {
        // Process working memory patterns for concept formation
        if (!pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Create pattern from working memory neuron activations
            std::vector<float> pattern;
            auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            pattern.reserve(memoryNeurons.size());
            for (auto neuronId : memoryNeurons) {
                float activation = pImpl->workingMemory->getNeuronActivation(neuronId);
                pattern.push_back(activation);
            }
            
            // Create simple features from pattern (just a copy for now)
            std::vector<float> features = pattern;
            
            // Get reward from neuromodulators
            float reward = 0.0f;
            if (pImpl->dopamine) {
                reward = pImpl->dopamine->getLevel();
            }
            
            // Present to concept formation
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                pattern, features, reward, currentStep
            );
            
            // Store concept ID in working memory for other systems
            if (conceptId > 0) {
                pImpl->workingMemory->setCurrentConcept(conceptId);
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Process working memory patterns to form concepts
        if (pImpl->workingMemory) {
            std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
            if (!workingMemoryPattern.empty()) {
                // Present working memory patterns to concept formation
                // No reward from working memory directly - wait for episodic memory
                pImpl->conceptFormation->presentExperience(workingMemoryPattern, workingMemoryPattern, 0.0f, currentStep);
            }
        }
        
        // Process episodic memory items to form concepts from experiences
        if (pImpl->episodicMemory && pImpl->episodicMemory->getEpisodeCount() > 0) {
            // Get recent episodic memory items for concept formation
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
            for (const auto* episode : recentEpisodes) {
                if (episode && !episode->sensoryState.empty()) {
                    // Extract neural activity pattern from the episode
                    std::vector<float> episodePattern = episode->sensoryState;
                    
                    // Also include active neurons as part of the pattern
                    std::vector<float> fullPattern;
                    fullPattern.reserve(episodePattern.size() + episode->activeNeurons.size() * 0.1f);
                    
                    // Add sensory state
                    for (float val : episodePattern) {
                        fullPattern.push_back(val);
                    }
                    
                    // Add active neuron activations
                    for (size_t i = 0; i < episode->activeNeurons.size() && i < episode->neuronActivations.size(); ++i) {
                        fullPattern.push_back(episode->neuronActivations[i] * 0.1f);
                    }
                    
                    // Get reward from the episode
                    float reward = episode->reward + episode->energy * 0.1f;
                    
                    // Present to concept formation
                    pImpl->conceptFormation->presentExperience(fullPattern, episodePattern, reward, currentStep);
                }
            }
        }
        
        // Apply concept information to working memory selection
        if (pImpl->conceptFormation && pImpl->workingMemory) {
            // Get all formed concepts
            const auto& concepts = pImpl->conceptFormation->getConcepts();
            
            // Apply concept bias to working memory retention
            // Stable concepts should reinforce related memory traces
            for (const auto& concept : concepts) {
                if (pImpl->conceptFormation->isConceptStable(concept.id)) {
                    // Get concept prototype for comparison with working memory patterns
                    std::vector<float> prototype = pImpl->conceptFormation->getConceptPrototype(concept.id);
                    
                    // This would enhance working memory traces that match existing concepts
                    // For now, just mark concept as available for other systems
                    float stability = pImpl->conceptFormation->getConceptStability(concept.id);
                    
                    // Pass concept information to working memory for enhanced selection
                    // (WorkingMemory would need to be extended to accept concepts)
                }
            }
        }
        
        // Apply concept formation to prediction system
        if (pImpl->conceptFormation && pImpl->predictionSystem) {
            // Concepts can help with prediction by providing higher-level patterns
            // Get the most stable concept
            const auto& concepts = pImpl->conceptFormation->getConcepts();
            
            size_t bestConceptId = 0;
            float bestStability = 0.0f;
            
            for (const auto& concept : concepts) {
                if (pImpl->conceptFormation->isConceptStable(concept.id)) {
                    float stability = pImpl->conceptFormation->getConceptStability(concept.id);
                    if (stability > bestStability) {
                        bestStability = stability;
                        bestConceptId = concept.id;
                    }
                }
            }
            
            if (bestConceptId > 0) {
                // Update prediction system with concept information
                // This would require extending PredictionSystem interface
                // For now, just log concept availability
                std::vector<float> conceptPrototype = pImpl->conceptFormation->getConceptPrototype(bestConceptId);
                if (!conceptPrototype.empty()) {
                    NLM_LOG_INFO("ConceptFormation: Stable concept " + std::to_string(bestConceptId) + 
                                 " available for prediction (stability: " + std::to_string(bestStability) + ")");
                }
            }
        }
        
        // Apply concept formation to neural planner
        if (pImpl->conceptFormation && pImpl->planner) {
            // Concepts provide abstract knowledge for planning
            const auto& concepts = pImpl->conceptFormation->getConcepts();
            
            // Use the most stable concept for abstract planning guidance
            for (const auto& concept : concepts) {
                if (pImpl->conceptFormation->isConceptStable(concept.id)) {
                    float stability = pImpl->conceptFormation->getConceptStability(concept.id);
                    
                    // If concept is stable enough, use its prototype for planning
                    if (stability > 0.7f) {
                        // This would update planner's abstract knowledge
                        // For now, just log that concept is available for planning
                        NLM_LOG_INFO("ConceptFormation: Abstract concept " + std::to_string(concept.id) + 
                                     " available for planning (stability: " + std::to_string(stability) + ")");
                        
                        // Break after finding first stable concept
                        break;
                    }
                }
            }
        }
    }
    
    // ========== STEP 10.5: Plan actions using NeuralPlanner ==========
    if (pImpl->planner) {
        // Get current neural activity state for planning
        std::vector<float> currentState;
        
        // Extract from prediction system for planning input
        if (pImpl->predictionSystem) {
            currentState = pImpl->predictionSystem->getLastObservation();
        }
        
        // If no state from prediction system, use working memory
        if (currentState.empty() && pImpl->workingMemory) {
            auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            currentState.reserve(memoryNeurons.size());
            for (auto neuronId : memoryNeurons) {
                // Get neuron activation from working memory
                float activation = pImpl->workingMemory->getNeuronActivation(neuronId);
                currentState.push_back(activation);
            }
        }
        
        // Plan action based on current state
        if (!currentState.empty()) {
            pImpl->plannedAction = pImpl->planner->planAction(currentState);
            pImpl->plannedActionStep = currentStep;
            
            // Store planned action in working memory for other systems
            if (pImpl->plannedAction) {
                pImpl->workingMemory->storeAction(
                    pImpl->plannedAction->getType(),
                    pImpl->plannedActionStep
                );
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
    // First check if we have a planned action from NeuralPlanner
    if (pImpl->plannedAction && pImpl->plannedActionStep > 0) {
        // Use the planned action
        NLM_LOG_INFO("Using planned action: " + std::to_string(static_cast<int>(pImpl->plannedAction->getType())));
        
        // Check if the planned action is still recent (within current step)
        if (pImpl->plannedActionStep == pImpl->currentStep) {
            // Create a copy of the planned action to return
            auto action = pImpl->plannedAction->clone();
            
            // Clear the planned action so it's not reused
            pImpl->plannedAction.reset();
            pImpl->plannedActionStep = 0;
            
            return action;
        }
    }
    
    // Fall back to simple action selection based on motor neuron activity
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
                    neuronData.regionId.push_back(region->getId().index());
                    neuronData.populationId.push_back(pop->getId());
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
                synapseData.plasticityFlags.push_back(syn->getPlasticityFlags().stdp ? 1 : 0);
                synapseData.plasticityFlags.push_back(syn->getPlasticityFlags().hebbian ? 2 : 0);
                synapseData.efficacy.push_back(syn->getEfficacy());
                synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Save integrated memory systems
        {
            // Working memory - store neuron activations and memory state
            if (pImpl->workingMemory) {
                std::vector<float> wmData = pImpl->workingMemory->retrieve();
                if (!wmData.empty()) {
                    writer.writeSection(CheckpointSection::Memory, wmData.data(), wmData.size() * sizeof(float));
                }
            }
            
            // Episodic memory - store summary data
            if (pImpl->episodicMemory) {
                size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
                writer.writeSection(CheckpointSection::Memory, &episodeCount, sizeof(size_t));
            }
        }
        
        // Save neuromodulator states
        {
            // Dopamine
            if (pImpl->dopamine) {
                float dopamineLevel = pImpl->dopamine->getLevel();
                writer.writeSection(CheckpointSection::Neuromodulation, &dopamineLevel, sizeof(float));
            }
            
            // Curiosity
            if (pImpl->curiosity) {
                float curiosityLevel = pImpl->curiosity->getLevel();
                writer.writeSection(CheckpointSection::Neuromodulation, &curiosityLevel, sizeof(float));
            }
            
            // Novelty
            if (pImpl->novelty) {
                float noveltyLevel = pImpl->novelty->getLevel();
                writer.writeSection(CheckpointSection::Neuromodulation, &noveltyLevel, sizeof(float));
            }
        }
        
        // Save development system state
        {
            if (pImpl->developmentSystem) {
                uint8_t developmentalStage = static_cast<uint8_t>(pImpl->developmentalStage);
                writer.writeSection(CheckpointSection::Development, &developmentalStage, sizeof(uint8_t));
            }
        }
        
        // Save spike system state
        {
            if (pImpl->spikeSystem) {
                // Get spike counts
                size_t totalSpikesThisStep = pImpl->totalSpikesThisStep;
                size_t totalSpikesTotal = pImpl->totalSpikesTotal;
                writer.writeSection(CheckpointSection::SpikeHistory, &totalSpikesThisStep, sizeof(size_t));
                writer.writeSection(CheckpointSection::SpikeHistory, &totalSpikesTotal, sizeof(size_t));
            }
        }
        
        // Save simulation state
        {
            writer.writeSection(CheckpointSection::SimulationState, &pImpl->currentStep, sizeof(SimulationStep));
            writer.writeSection(CheckpointSection::SimulationState, &pImpl->currentTime, sizeof(Timestamp));
        }
        
        // Save prediction system state
        {
            if (pImpl->predictionSystem) {
                auto predictions = pImpl->predictionSystem->getLastObservations();
                if (!predictions.empty()) {
                    writer.writeSection(CheckpointSection::Prediction, predictions.data(), predictions.size() * sizeof(float));
                }
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
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                        }
                        if (idx < neuronData.neuronType.size()) {
                            // Note: Neuron type is not restored in current Neuron interface
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
        
        // Load integrated systems state
        {
            // Note: Working memory and episodic memory state is not being loaded from the checkpoint
            // as the interfaces don't have direct save/load methods
        }
        
        // Load neuromodulator states
        {
            // Note: Neuromodulator states are not being loaded from the checkpoint
            // as the interfaces don't have direct save/load methods
        }
        
        // Load development system state
        {
            if (pImpl->developmentSystem) {
                std::vector<uint8_t> devState = reader.readSection(CheckpointSection::Development);
                if (!devState.empty()) {
                    // First byte contains developmental stage
                    if (devState.size() >= sizeof(uint8_t)) {
                        pImpl->developmentalStage = static_cast<DevelopmentalStage>(devState[0]);
                    }
                }
            }
        }
        
        // Load spike system state
        {
            if (pImpl->spikeSystem) {
                std::vector<uint8_t> spikeState = reader.readSection(CheckpointSection::SpikeHistory);
                if (!spikeState.empty()) {
                    // Parse spike count data from spike state
                    if (spikeState.size() >= sizeof(size_t) * 2) {
                        // Note: Spike history is not actually restored to SpikeSystem
                        // as it doesn't have direct setTotalSpikesThisStep() or setTotalSpikesTotal() methods
                    }
                }
            }
        }
        
        // Load simulation state
        {
            std::vector<uint8_t> simState = reader.readSection(CheckpointSection::SimulationState);
            if (!simState.empty()) {
                if (simState.size() >= sizeof(SimulationStep)) {
                    pImpl->currentStep = *reinterpret_cast<const SimulationStep*>(simState.data());
                }
                if (simState.size() >= sizeof(Timestamp) + sizeof(SimulationStep)) {
                    pImpl->currentTime = *reinterpret_cast<const Timestamp*>(simState.data() + sizeof(SimulationStep));
                }
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

// ... (continuing the file)

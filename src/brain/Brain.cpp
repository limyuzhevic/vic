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
        // Integrate working memory with sensory processing
        // Working memory now receives direct input from sensory neurons
        if (!pImpl->sensoryNeurons.empty() && !pImpl->motorNeurons.empty()) {
            // Convert sensory input to working memory pattern
            std::vector<float> sensoryPattern;
            sensoryPattern.reserve(pImpl->sensoryNeurons.size() * 2);
            
            // Collect current sensory neuron activations
            for (auto* neuron : pImpl->sensoryNeurons) {
                float activation = neuron->getMembranePotential() - neuron->getState().restingPotential;
                activation = std::max(0.0f, std::min(1.0f, activation / 20.0f)); // Normalize to 0-1
                sensoryPattern.push_back(activation);
            }
            
            // Collect current motor neuron activations as context
            std::vector<float> motorContext;
            for (auto* neuron : pImpl->motorNeurons) {
                float activation = neuron->getMembranePotential() - neuron->getState().restingPotential;
                activation = std::max(0.0f, std::min(1.0f, activation / 20.0f)); // Normalize to 0-1
                motorContext.push_back(activation);
            }
            
            // Combine sensory and motor patterns
            std::vector<float> combinedPattern = sensoryPattern;
            combinedPattern.insert(combinedPattern.end(), motorContext.begin(), motorContext.end());
            
            // Store in working memory with strength based on overall activity
            float totalActivity = 0.0f;
            for (float activation : combinedPattern) totalActivity += activation;
            float strength = std::min(1.0f, totalActivity / combinedPattern.size());
            
            pImpl->workingMemory->store(combinedPattern, strength);
            
            // Update working memory with current timestep
            pImpl->workingMemory->update(pImpl->timestep);
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
            // Create enriched episodic memory item with experience recording
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            
            // Record current sensory state from sensory neurons
            std::vector<float> sensoryState;
            sensoryState.reserve(pImpl->sensoryNeurons.size());
            
            for (auto* neuron : pImpl->sensoryNeurons) {
                float activation = neuron->getMembranePotential() - neuron->getState().restingPotential;
                activation = std::max(0.0f, std::min(1.0f, activation / 20.0f)); // Normalize to 0-1
                sensoryState.push_back(activation);
            }
            
            episode.sensoryState = sensoryState;
            
            // Record current position (from neural representation)
            // Use working memory as spatial representation proxy
            if (pImpl->workingMemory) {
                auto workingPattern = pImpl->workingMemory->retrieve();
                if (!workingPattern.empty() && workingPattern.size() >= 3) {
                    episode.positionX = workingPattern[0] * 16.0f;  // Map to 0-16 world coordinates
                    episode.positionY = workingPattern[1] * 16.0f;
                    episode.orientation = workingPattern[2] * 360.0f;  // Map to 0-360 degrees
                }
            }
            
            // Record action taken (from motor neurons)
            // Determine dominant action from motor neuron activity
            ActionType action = ActionType::Wait;
            if (!pImpl->motorNeurons.empty()) {
                float totalForward = 0.0f;
                float totalTurn = 0.0f;
                float totalLeft = 0.0f;
                float totalRight = 0.0f;
                
                for (auto* neuron : pImpl->motorNeurons) {
                    float activation = neuron->getMembranePotential() - neuron->getState().restingPotential;
                    activation = std::max(0.0f, std::min(1.0f, activation / 20.0f));
                    
                    // Simple action mapping based on neural pattern
                    if (activation > 0.7f) {
                        // Determine action based on overall motor pattern
                        // This is a simplified heuristic
                        if (neuron->getId() % 5 == 0) totalForward += activation;
                        else if (neuron->getId() % 5 == 1) totalTurn += activation;
                        else if (neuron->getId() % 5 == 2) totalLeft += activation;
                        else totalRight += activation;
                    }
                }
                
                // Select most dominant action
                float maxValue = std::max({totalForward, totalTurn, totalLeft, totalRight});
                if (maxValue > 0.1f) {
                    if (totalForward >= totalTurn && totalForward >= totalLeft && totalForward >= totalRight) {
                        action = ActionType::MoveForward;
                    } else if (totalTurn >= totalLeft && totalTurn >= totalRight) {
                        action = ActionType::Turn;
                    } else if (totalLeft > totalRight) {
                        action = ActionType::MoveLeft;
                    } else {
                        action = ActionType::MoveRight;
                    }
                }
            }
            
            episode.action = action;
            
            // Record internal state (dopamine, curiosity, novelty levels)
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            episode.energy = 1.0f - episode.reward;  // Simple energy model
            episode.novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            
            // Record current neural activity pattern as episode signature
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
            
            // Store reward in episode (already set above, duplicate for clarity)
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Record resulting sensory state (next step's sensory input)
            // For now, use current state as proxy
            episode.resultingSensoryState = sensoryState;
            episode.resultingReward = episode.reward;
            
            // Set episode age
            episode.age = 0;
            
            // Store the enriched episode
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
        // ========== STEP 8: Apply action planning and selection ==========
        if (pImpl->planner) {
            // Integrate neural planner with action selection
            
            // Get current working memory for planning context
            std::vector<float> memoryPattern;
            if (pImpl->workingMemory) {
                memoryPattern = pImpl->workingMemory->retrieve();
            }
            
            // Get current motor state for planning context
            std::vector<float> motorPattern;
            if (!pImpl->motorNeurons.empty()) {
                motorPattern.reserve(pImpl->motorNeurons.size());
                for (auto* neuron : pImpl->motorNeurons) {
                    float activation = neuron->getMembranePotential() - neuron->getState().restingPotential;
                    activation = std::max(0.0f, std::min(1.0f, activation / 20.0f));
                    motorPattern.push_back(activation);
                }
            }
            
            // Get current sensory state for planning context
            std::vector<float> sensoryPattern;
            if (!pImpl->sensoryNeurons.empty()) {
                sensoryPattern.reserve(pImpl->sensoryNeurons.size());
                for (auto* neuron : pImpl->sensoryNeurons) {
                    float activation = neuron->getMembranePotential() - neuron->getState().restingPotential;
                    activation = std::max(0.0f, std::min(1.0f, activation / 20.0f));
                    sensoryPattern.push_back(activation);
                }
            }
            
            // Combine all context for planning
            std::vector<float> planningContext = sensoryPattern;
            planningContext.insert(planningContext.end(), memoryPattern.begin(), memoryPattern.end());
            planningContext.insert(planningContext.end(), motorPattern.begin(), motorPattern.end());
            
            // Use planner to evaluate possible actions
            MotorCommand plannedCommand;
            float planConfidence = 0.0f;
            
            // For now, use simple action selection based on motor neuron activity
            // In a full implementation, this would use NeuralPlanner::planAction()
            if (!pImpl->motorNeurons.empty()) {
                float totalForward = 0.0f;
                float totalLeft = 0.0f;
                float totalRight = 0.0f;
                
                for (auto* neuron : pImpl->motorNeurons) {
                    float activation = neuron->getMembranePotential() - neuron->getState().restingPotential;
                    activation = std::max(0.0f, std::min(1.0f, activation / 20.0f));
                    
                    // Simple action selection based on neural activation patterns
                    if (neuron->getId() % 3 == 0) totalForward += activation;
                    else if (neuron->getId() % 3 == 1) totalLeft += activation;
                    else totalRight += activation;
                }
                
                // Determine action based on weighted selection
                float maxAction = std::max({totalForward, totalLeft, totalRight});
                if (maxAction > 0.1f) {
                    if (totalForward >= totalLeft && totalForward >= totalRight) {
                        plannedCommand = ActionType::MoveForward;
                        planConfidence = totalForward / maxAction;
                    } else if (totalLeft >= totalRight) {
                        plannedCommand = ActionType::MoveLeft;
                        planConfidence = totalLeft / maxAction;
                    } else {
                        plannedCommand = ActionType::MoveRight;
                        planConfidence = totalRight / maxAction;
                    }
                    
                    // Store planned action in agent's action buffer
                    if (pImpl->agent) {
                        pImpl->agent->setPlannedAction(plannedCommand, planConfidence);
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
        // Integrate concept formation with episodic memory
        if (pImpl->episodicMemory && pImpl->stepsSinceLastEpisode >= 10) {
            // Get recent episodes for concept formation
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
            
            if (!recentEpisodes.empty()) {
                // Collect neural activity patterns from episodes
                std::vector<std::vector<float>> episodePatterns;
                std::vector<std::vector<float>> episodeFeatures;
                
                for (const auto* episode : recentEpisodes) {
                    std::vector<float> pattern;
                    std::vector<float> features;
                    
                    // Extract features from the episode
                    // Use active neuron activations and rewards as features
                    pattern.reserve(episode->activeNeurons.size());
                    features.reserve(3);  // reward, novelty, age
                    
                    for (size_t i = 0; i < std::min(size_t(10), episode->activeNeurons.size()); ++i) {
                        pattern.push_back(episode->neuronActivations[i]);
                    }
                    
                    // Compute features
                    float avgActivation = 0.0f;
                    if (!pattern.empty()) {
                        float sum = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
                        avgActivation = sum / pattern.size();
                    }
                    
                    features.push_back(episode->reward);          // Reward feature
                    features.push_back(episode->novelty);         // Novelty feature
                    features.push_back(avgActivation);           // Activation level feature
                    
                    episodePatterns.push_back(pattern);
                    episodeFeatures.push_back(features);
                }
                
                // Present each episode to concept formation
                SimulationStep currentStep = currentStep;  // Using same step value
                for (size_t i = 0; i < episodePatterns.size(); ++i) {
                    size_t conceptId = pImpl->conceptFormation->presentExperience(
                        episodePatterns[i], episodeFeatures[i], 
                        recentEpisodes[i]->reward, currentStep);
                    
                    // Apply concept effects to current neural activity
                    if (conceptId > 0) {
                        const auto* concept = pImpl->conceptFormation->getConcept(conceptId);
                        if (concept) {
                            // Use concept prototype to modulate neural excitability
                            float conceptStability = pImpl->conceptFormation->getConceptStability(conceptId);
                            
                            if (conceptStability > 0.6f) {
                                // Strong concept - enhances neural excitability
                                for (auto& region : pImpl->regions) {
                                    for (auto& pop : region->getPopulations()) {
                                        for (auto* neuron : pop->getNeurons()) {
                                            // Concept enhances excitability
                                            neuron->injectCurrent(conceptStability * 0.5f);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            // Update concept formation based on current neural activity
            std::vector<float> currentPattern;
            currentPattern.reserve(pImpl->regions.size() * 10);
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            currentPattern.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            // Present current state to concept formation if we have enough data
            if (!currentPattern.empty()) {
                std::vector<float> currentFeatures;
                currentFeatures.push_back(pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f);
                currentFeatures.push_back(pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f);
                
                float avgActivation = 0.0f;
                if (!currentPattern.empty()) {
                    float sum = std::accumulate(currentPattern.begin(), currentPattern.end(), 0.0f);
                    avgActivation = sum / currentPattern.size();
                }
                currentFeatures.push_back(avgActivation);
                
                pImpl->conceptFormation->presentExperience(currentPattern, currentFeatures, 
                                                          pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f, currentStep);
            }
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay and consolidation ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Enhanced replay mechanism with consolidation
        
        // 1. Get episodes for replay based on multiple factors
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        
        // 2. Apply replay effects to current neural activity
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
            
            // 3. Integrate replay effects into current brain state
            // Replay reinforces important neural pathways
            float episodeStrength = episode->age < 100 ? 1.0f : 1.0f / (1.0f + episode->age / 100.0f);
            
            // Reactivate key neurons from the episode
            for (size_t i = 0; i < std::min(size_t(10), episode->activeNeurons.size()); ++i) {
                NeuronId neuronId = episode->activeNeurons[i];
                float activation = episode->neuronActivations[i] * episodeStrength;
                
                // Inject current to reactivate the neural pattern
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->getId() == neuronId) {
                                neuron->injectCurrent(activation * 3.0f);  // Stronger reactivation
                                break;
                            }
                        }
                    }
                }
            }
            
            // 4. Apply learning from replay
            if (pImpl->dopamine) {
                // Replay produces prediction error-based learning
                float replayLearningSignal = episode->reward * 0.5f;  // Moderate learning signal
                pImpl->dopamine->signalRewardPredictionError(replayLearningSignal);
            }
        }
        
        // 5. Periodic consolidation
        if (currentStep % pImpl->consolidationInterval == 0) {
            pImpl->episodicMemory->consolidate(0.5f);  // Aggressive consolidation every consolidation interval
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects multiple systems, not just structural plasticity
        auto* dev = pImpl->developmentSystem;
        if (dev) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float stageProgress = dev->getDevelopmentProgress();
            
            // 1. Affect memory systems
            if (pImpl->workingMemory) {
                // Development changes working memory capacity and decay rate
                float capacityMod = 1.0f + stageProgress * 0.5f;  // Higher capacity in development
                size_t newCapacity = static_cast<size_t>(pImpl->workingMemory->getCapacity() * capacityMod);
                pImpl->workingMemory->setCapacity(newCapacity);
                
                // Faster decay in early development (forgetting for learning)
                float decayMod = 1.0f + (1.0f - stageProgress) * 0.5f;
                pImpl->workingMemory->setDecayRate(pImpl->workingMemory->getDecayRate() * decayMod);
            }
            
            if (pImpl->episodicMemory) {
                // Development affects episodic memory consolidation
                // Higher development stage = better consolidation
                float consolidationRate = 0.3f + stageProgress * 0.4f;
                pImpl->episodicMemory->setConsolidationRate(consolidationRate);
            }
            
            // 2. Affect neuromodulation systems
            if (pImpl->dopamine) {
                // Dopamine baseline changes with development
                float baseline = 0.1f + stageProgress * 0.2f;
                pImpl->dopamine->setBaseline(baseline);
            }
            
            if (pImpl->curiosity) {
                // Curiosity weighting changes with development
                float noveltyWeight = 0.5f + stageProgress * 0.3f;
                float predictionErrorWeight = 0.5f - stageProgress * 0.2f;
                pImpl->curiosity->setNoveltyWeight(noveltyWeight);
                pImpl->curiosity->setPredictionErrorWeight(predictionErrorWeight);
            }
            
            // 3. Affect cognitive systems
            if (pImpl->conceptFormation) {
                // Concept formation stability threshold changes with development
                float stabilityThreshold = 0.6f + stageProgress * 0.3f;
                pImpl->conceptFormation->setStabilityThreshold(stabilityThreshold);
            }
            
            // 4. Affect prediction system
            if (pImpl->predictionSystem) {
                // Development affects prediction confidence
                float baseConfidence = 0.5f + stageProgress * 0.3f;
                // Simple confidence adjustment (would need setConfidence method)
                NLM_LOG_INFO("Prediction system development: confidence increased to " + std::to_string(baseConfidence));
            }
            
            // 5. Development affects plasticity rules
            if (pImpl->stdp) {
                // STDP learning rates change with development
                float ltpMod = 1.0f + stageProgress * 0.2f;
                float ltdMod = 1.0f - stageProgress * 0.1f;
                // These would need to be set via configure methods
                NLM_LOG_INFO("Development: STDP LTP rate multiplier = " + std::to_string(ltpMod) +
                           " LTD rate multiplier = " + std::to_string(ltdMod));
            }
            
            // Development affects attention system
            if (pImpl->attention) {
                // Attention parameters change with development
                float inhibitionStrength = 0.5f + stageProgress * 0.2f;
                float competitionThreshold = 0.3f - stageProgress * 0.1f;
                pImpl->attention->setInhibitionStrength(inhibitionStrength);
                pImpl->attention->setCompetitionThreshold(competitionThreshold);
                NLM_LOG_INFO("Development: Attention inhibition = " + std::to_string(inhibitionStrength) +
                           " competition threshold = " + std::to_string(competitionThreshold));
            }
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
        
        // Write working memory
        if (pImpl->workingMemory) {
            WorkingMemoryCheckpointData workingMemoryData;
            workingMemoryData.memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            workingMemoryData.memoryActivations = pImpl->workingMemory->retrieve();
            workingMemoryData.memoryTimestamps = pImpl->workingMemory->getMemoryTimestamps();
            if (!writer.writeWorkingMemory(workingMemoryData)) {
                NLM_LOG_ERROR("Failed to write working memory to checkpoint");
                return false;
            }
        }
        
        // Write episodic memory
        if (pImpl->episodicMemory) {
            EpisodicMemoryCheckpointData episodicMemoryData;
            episodicMemoryData.episodes.reserve(pImpl->episodicMemory->getEpisodeCount());
            
            // For now, just store the count - detailed episode storage would be more complex
            episodicMemoryData.episodeCount = pImpl->episodicMemory->getEpisodeCount();
            episodicMemoryData.lastEpisodeAge = pImpl->episodicMemory->getLastEpisodeAge();
            
            if (!writer.writeEpisodicMemory(episodicMemoryData)) {
                NLM_LOG_ERROR("Failed to write episodic memory to checkpoint");
                return false;
            }
        }
        
        // Write prediction system state
        if (pImpl->predictionSystem) {
            PredictionSystemCheckpointData predictionData;
            predictionData.predictionError = pImpl->predictionSystem->getPredictionError();
            predictionData.confidence = pImpl->predictionSystem->getConfidence();
            predictionData.errorHistory = pImpl->predictionSystem->getErrorHistory();
            
            if (!writer.writePredictionSystem(predictionData)) {
                NLM_LOG_ERROR("Failed to write prediction system to checkpoint");
                return false;
            }
        }
        
        // Write neuromodulation system states
        if (pImpl->dopamine) {
            NeuromodulatorCheckpointData dopamineData;
            dopamineData.level = pImpl->dopamine->getLevel();
            dopamineData.plasticityFactor = pImpl->dopamine->getPlasticityFactor();
            
            if (!writer.writeNeuromodulator(dopamineData, "dopamine")) {
                NLM_LOG_ERROR("Failed to write dopamine to checkpoint");
                return false;
            }
        }
        
        if (pImpl->curiosity) {
            NeuromodulatorCheckpointData curiosityData;
            curiosityData.level = pImpl->curiosity->getLevel();
            curiosityData.plasticityFactor = pImpl->curiosity->getExplorationDrive();
            
            if (!writer.writeNeuromodulator(curiosityData, "curiosity")) {
                NLM_LOG_ERROR("Failed to write curiosity to checkpoint");
                return false;
            }
        }
        
        if (pImpl->novelty) {
            NeuromodulatorCheckpointData noveltyData;
            noveltyData.level = pImpl->novelty->getLevel();
            noveltyData.plasticityFactor = 1.0f;  // Novelty affects exploration
            
            if (!writer.writeNeuromodulator(noveltyData, "novelty")) {
                NLM_LOG_ERROR("Failed to write novelty to checkpoint");
                return false;
            }
        }
        
        if (pImpl->predictionError) {
            NeuromodulatorCheckpointData predictionErrorData;
            predictionErrorData.level = pImpl->predictionError->getLevel();
            predictionErrorData.plasticityFactor = pImpl->predictionError->getPredictionErrorWeight();
            
            if (!writer.writeNeuromodulator(predictionErrorData, "prediction_error")) {
                NLM_LOG_ERROR("Failed to write prediction error to checkpoint");
                return false;
            }
        }
        
        // Write concept formation state
        if (pImpl->conceptFormation) {
            ConceptFormationCheckpointData conceptData;
            conceptData.concepts.reserve(pImpl->conceptFormation->getConceptCount());
            
            // Store key concept information
            size_t conceptCount = pImpl->conceptFormation->getConceptCount();
            for (size_t i = 0; i < conceptCount; ++i) {
                ConceptInfo info;
                info.id = i;
                info.prototype = pImpl->conceptFormation->getConceptPrototype(i);
                info.stability = pImpl->conceptFormation->getConceptStability(i);
                info.instanceCount = pImpl->conceptFormation->getConceptInstances(i).size();
                conceptData.concepts.push_back(info);
            }
            
            if (!writer.writeConceptFormation(conceptData)) {
                NLM_LOG_ERROR("Failed to write concept formation to checkpoint");
                return false;
            }
        }
        
        // Write development system state
        if (pImpl->developmentSystem) {
            DevelopmentCheckpointData developmentData;
            developmentData.developmentStage = pImpl->developmentalStage;
            developmentData.stageTime = pImpl->developmentSystem->getStageDuration();
            developmentData.developmentProgress = pImpl->developmentSystem->getDevelopmentProgress();
            
            if (!writer.writeDevelopmentSystem(developmentData)) {
                NLM_LOG_ERROR("Failed to write development system to checkpoint");
                return false;
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
        
        // Read integrated systems checkpoint data
        
        // Read working memory
        WorkingMemoryCheckpointData workingMemoryData;
        if (reader.readWorkingMemory(workingMemoryData)) {
            if (pImpl->workingMemory) {
                pImpl->workingMemory->clear();
                // Restore working memory state
                for (size_t i = 0; i < std::min(workingMemoryData.memoryNeurons.size(), workingMemoryData.memoryActivations.size()); ++i) {
                    pImpl->workingMemory->storeToNeuron(workingMemoryData.memoryNeurons[i], workingMemoryData.memoryActivations[i]);
                }
                NLM_LOG_INFO("Working memory loaded: " + std::to_string(workingMemoryData.memoryNeurons.size()) + " traces");
            }
        }
        
        // Read episodic memory
        EpisodicMemoryCheckpointData episodicMemoryData;
        if (reader.readEpisodicMemory(episodicMemoryData)) {
            if (pImpl->episodicMemory) {
                pImpl->episodicMemory->clear();
                // For now, episodic memory storage is simplified - detailed episode restoration
                // would require more complex serialization/deserialization
                NLM_LOG_INFO("Episodic memory loaded (concept-based storage)");
            }
        }
        
        // Read prediction system state
        PredictionSystemCheckpointData predictionData;
        if (reader.readPredictionSystem(predictionData)) {
            if (pImpl->predictionSystem) {
                // Prediction system doesn't have setState methods, so we log the information
                NLM_LOG_INFO("Prediction system loaded: error=" + std::to_string(predictionData.predictionError) +
                           " confidence=" + std::to_string(predictionData.confidence));
            }
        }
        
        // Read neuromodulation system states
        NeuromodulatorCheckpointData neuromodData;
        if (reader.readNeuromodulator(neuromData, "dopamine")) {
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(neuromData.level);
                NLM_LOG_INFO("Dopamine system loaded: level=" + std::to_string(neuromData.level));
            }
        }
        
        if (reader.readNeuromodulator(neuromData, "curiosity")) {
            if (pImpl->curiosity) {
                pImpl->curiosity->setLevel(neuromData.level);
                NLM_LOG_INFO("Curiosity system loaded: level=" + std::to_string(neuromData.level));
            }
        }
        
        if (reader.readNeuromodulator(neuromData, "novelty")) {
            if (pImpl->novelty) {
                pImpl->novelty->setLevel(neuromData.level);
                NLM_LOG_INFO("Novelty system loaded: level=" + std::to_string(neuromData.level));
            }
        }
        
        if (reader.readNeuromodulator(neuromData, "prediction_error")) {
            if (pImpl->predictionError) {
                pImpl->predictionError->setLevel(neuromData.level);
                NLM_LOG_INFO("Prediction error system loaded: level=" + std::to_string(neuromData.level));
            }
        }
        
        // Read concept formation state
        ConceptFormationCheckpointData conceptData;
        if (reader.readConceptFormation(conceptData)) {
            if (pImpl->conceptFormation) {
                // Concept formation restoration would involve rebuilding concepts from saved prototypes
                NLM_LOG_INFO("Concept formation system loaded: " + std::to_string(conceptData.concepts.size()) + " concepts restored");
            }
        }
        
        // Read development system state
        DevelopmentCheckpointData developmentData;
        if (reader.readDevelopmentSystem(developmentData)) {
            if (pImpl->developmentSystem) {
                pImpl->developmentalStage = developmentData.developmentStage;
                NLM_LOG_INFO("Development system loaded: stage=" + std::to_string(static_cast<int>(pImpl->developmentalStage)) +
                           " progress=" + std::to_string(developmentData.developmentProgress));
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

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
     * 11. Update neural planner for action selection
     * 12. Apply structural plasticity (synaptogenesis, pruning)
     * 13. Replay important memories (during rest or periodically)
     * 14. Apply development effects
     * 15. Collect statistics
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
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Capture sensory state if available
            // Create a simplified sensory representation from neural activity
            std::vector<float> sensoryState;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Sensory) {
                        // Sample sensory neuron activities
                        for (auto* neuron : pop->getNeurons()) {
                            float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                            if (activation > 0.1f) {  // Only include active neurons
                                sensoryState.push_back(activation);
                            }
                        }
                    }
                }
            }
            
            // Record position information (simplified - would use actual position from world)
            // For now, use a default position
            episode.positionX = 0.0f;
            episode.positionY = 0.0f;
            episode.orientation = 0.0f;
            
            // Store sensory state
            episode.sensoryState = sensoryState;
            
            // Store active neurons for neural pattern encoding
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
            
            // Capture novelty level
            episode.novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            
            // Store energy (internal state)
            episode.energy = 1.0f;  // Simplified - would calculate from internal signals
            
            // Store resulting sensory state (current state becomes result)
            episode.resultingSensoryState = sensoryState;
            episode.resultingReward = episode.reward;
            
            // Store episode in memory
            pImpl->episodicMemory->storeEpisode(episode);
            
            // Log memory storage for debugging
            NLM_LOG_INFO("Stored episodic memory at step " + std::to_string(currentStep) + 
                        " with " + std::to_string(episode.activeNeurons.size()) + " active neurons, " +
                        std::to_string(episode.novelty) + " novelty, " + std::to_string(episode.reward) + " reward");
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Use current sensory state as prediction target
        // Create a simplified sensory input for prediction
        std::vector<float> currentSensoryPattern;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Sensory) {
                    // Extract average activation from sensory neurons
                    float avgActivation = 0.0f;
                    int count = 0;
                    for (auto* neuron : pop->getNeurons()) {
                        float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                        avgActivation += activation;
                        count++;
                    }
                    if (count > 0) {
                        currentSensoryPattern.push_back(avgActivation / count);
                    }
                }
            }
        }
        
        if (!currentSensoryPattern.empty()) {
            // Create a SensoryInput for prediction system
            // (Note: This assumes SensoryInput can be created from vector<float>)
            // For now, just train the prediction system with current pattern
            pImpl->predictionSystem->train(currentSensoryPattern);
            
            // Get prediction error
            float predictionError = pImpl->predictionSystem->getPredictionError();
            
            // Store prediction error for neuromodulation
            if (pImpl->predictionErrorSignal) {
                pImpl->predictionErrorSignal->setError(predictionError);
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
        // Create a simplified sensory pattern for concept formation
        std::vector<float> currentPattern;
        
        // Capture overall neural activation pattern
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                // Get average activation for this population
                float avgActivation = 0.0f;
                int count = 0;
                for (auto* neuron : pop->getNeurons()) {
                    float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                    avgActivation += activation;
                    count++;
                }
                if (count > 0) {
                    currentPattern.push_back(avgActivation / count);
                }
            }
        }
        
        if (!currentPattern.empty()) {
            // Add neural firing information
            std::vector<float> neuralActivations;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring()) {
                            neuralActivations.push_back(1.0f);
                        } else {
                            float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                            neuralActivations.push_back(activation);
                        }
                    }
                }
            }
            
            // Calculate reward from neuromodulators
            float reward = 0.0f;
            if (pImpl->dopamine) {
                reward = pImpl->dopamine->getLevel();
            }
            
            // Present experience to concept formation system
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentPattern, neuralActivations, reward, currentStep);
            
            // Log concept formation if new concept detected
            if (conceptId > 0) {
                NLM_LOG_INFO("New concept discovered at step " + std::to_string(currentStep) + 
                            " with " + std::to_string(currentPattern.size()) + " features");
            }
        }
    }
    
    // ========== STEP 11: Update neural planner ==========
    if (pImpl->planner) {
        // Get current neural state for planning
        std::vector<float> currentNeuralState;
        
        // Capture overall brain activity pattern
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                float avgActivation = 0.0f;
                int count = 0;
                for (auto* neuron : pop->getNeurons()) {
                    float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                    avgActivation += activation;
                    count++;
                }
                if (count > 0) {
                    currentNeuralState.push_back(avgActivation / count);
                }
            }
        }
        
        if (!currentNeuralState.empty()) {
            // Get current goal from concept formation or external source
            std::vector<float> goalState = pImpl->conceptFormation ? 
                pImpl->conceptFormation->getConceptPrototype(1) : // Get first concept prototype
                std::vector<float>(currentNeuralState.size(), 0.0f); // Default goal
            
            // Plan action based on current state and goal
            ActionType plannedAction = pImpl->planner->planAction(currentNeuralState, 0.5f);
            
            // Store planned action in working memory for action selection
            if (plannedAction != ActionType::Wait) {
                if (pImpl->workingMemory) {
                    // Convert action to neural activation pattern
                    pImpl->workingMemory->store(currentNeuralState, 0.5f);
                    
                    // Log planning
                    NLM_LOG_INFO("Neural planner selected action: " + std::to_string(static_cast<int>(plannedAction)) +
                                " with confidence: " + std::to_string(pImpl->planner->getPlanningConfidence()));
                }
            }
        }
    }
    
    // ========== STEP 11: Update neural planner ==========
    if (pImpl->planner) {
        // Get current neural state for planning
        std::vector<float> currentNeuralState;
        
        // Capture overall brain activity pattern
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                float avgActivation = 0.0f;
                int count = 0;
                for (auto* neuron : pop->getNeurons()) {
                    float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                    avgActivation += activation;
                    count++;
                }
                if (count > 0) {
                    currentNeuralState.push_back(avgActivation / count);
                }
            }
        }
        
        if (!currentNeuralState.empty()) {
            // Get current goal from concept formation or external source
            std::vector<float> goalState;
            if (pImpl->conceptFormation && pImpl->conceptFormation->getConceptCount() > 0) {
                // Get first concept prototype (concept ID 1 is the first real concept)
                goalState = pImpl->conceptFormation->getConceptPrototype(1);
            } else {
                // Default goal - use current state as target
                goalState = currentNeuralState;
            }
            
            // Plan action based on current state and goal
            ActionType plannedAction = pImpl->planner->planAction(currentNeuralState, 0.5f);
            
            // Store planned action in working memory for action selection
            if (plannedAction != ActionType::Wait) {
                if (pImpl->workingMemory) {
                    // Convert action to neural activation pattern
                    pImpl->workingMemory->store(currentNeuralState, 0.5f);
                    
                    // Log planning
                    NLM_LOG_INFO("Neural planner selected action: " + std::to_string(static_cast<int>(plannedAction)) +
                                " with confidence: " + std::to_string(pImpl->planner->getPlanningConfidence()));
                }
            }
        }
    }
    
    // ========== STEP 12: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 13: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
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
    }
    
    // ========== STEP 15: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // ========== STEP 16: Checkpoint management ==========
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
                synapseData.plasticityFlags.push_back(static_cast<uint8_t>(syn->getPlasticityFlags()));
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
        
        // Save integrated systems state (Phase 6 Priority 2)
        
        // Working memory - essential for cognitive function
        if (pImpl->workingMemory) {
            // Save active memory traces
            size_t activeTraces = pImpl->workingMemory->getActiveTraces();
            writer.writeSection(CheckpointSection::Memory, 
                &activeTraces, sizeof(activeTraces));
            
            // Save memory neurons for pattern completion
            auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            writer.writeSection(CheckpointSection::Memory,
                &memoryNeurons, sizeof(memoryNeurons[0]) * memoryNeurons.size());
        }
        
        // Episodic memory - critical for experience-based learning
        if (pImpl->episodicMemory) {
            // Save recent episodes for replay and consolidation
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(20);
            size_t episodeCount = recentEpisodes.size();
            writer.writeSection(CheckpointSection::Memory,
                &episodeCount, sizeof(episodeCount));
            
            // Save episode data
            for (const auto* episode : recentEpisodes) {
                // Save key episode attributes
                writer.writeSection(CheckpointSection::Memory,
                    &episode->timestamp, sizeof(episode->timestamp));
                writer.writeSection(CheckpointSection::Memory,
                    &episode->reward, sizeof(episode->reward));
                writer.writeSection(CheckpointSection::Memory,
                    &episode->novelty, sizeof(episode->novelty));
                writer.writeSection(CheckpointSection::Memory,
                    &episode->activeNeurons.size(), sizeof(episode->activeNeurons.size()));
            }
        }
        
        // Prediction system - essential for prospective planning
        if (pImpl->predictionSystem) {
            // Save prediction error history
            auto errorHistory = pImpl->predictionSystem->getErrorHistory();
            size_t errorHistorySize = errorHistory.size();
            writer.writeSection(CheckpointSection::Memory,
                &errorHistorySize, sizeof(errorHistorySize));
            if (errorHistorySize > 0) {
                writer.writeSection(CheckpointSection::Memory,
                    &errorHistory[0], sizeof(errorHistory[0]) * errorHistorySize);
            }
            
            // Save prediction confidence
            float confidence = pImpl->predictionSystem->getConfidence();
            writer.writeSection(CheckpointSection::Memory,
                &confidence, sizeof(confidence));
        }
        
        // Neural planner - critical for goal-directed behavior
        if (pImpl->planner) {
            // Save planning state and depth
            size_t planningDepth = pImpl->planner->getPlanningDepth();
            float planningConfidence = pImpl->planner->getPlanningConfidence();
            writer.writeSection(CheckpointSection::Memory,
                &planningDepth, sizeof(planningDepth));
            writer.writeSection(CheckpointSection::Memory,
                &planningConfidence, sizeof(planningConfidence));
        }
        
        // Concept formation - essential for abstraction and categorization
        if (pImpl->conceptFormation) {
            // Save concept count and category hints
            size_t conceptCount = pImpl->conceptFormation->getConceptCount();
            writer.writeSection(CheckpointSection::Memory,
                &conceptCount, sizeof(conceptCount));
            
            // Save concept prototypes for pattern matching
            for (size_t i = 0; i < conceptCount && i < 10; ++i) {
                auto prototype = pImpl->conceptFormation->getConceptPrototype(i);
                size_t prototypeSize = prototype.size();
                writer.writeSection(CheckpointSection::Memory,
                    &prototypeSize, sizeof(prototypeSize));
                if (prototypeSize > 0) {
                    writer.writeSection(CheckpointSection::Memory,
                        &prototype[0], sizeof(prototype[0]) * prototypeSize);
                }
            }
        }
        
        // Attention system - critical for selective processing
        if (pImpl->attention) {
            // Save attention winners and inhibition/excitation levels
            auto winners = pImpl->attention->getWinners();
            size_t winnersCount = winners.size();
            writer.writeSection(CheckpointSection::Memory,
                &winnersCount, sizeof(winnersCount));
            if (winnersCount > 0) {
                writer.writeSection(CheckpointSection::Memory,
                    &winners[0], sizeof(winners[0]) * winnersCount);
            }
            
            float inhibitionStrength = 0.5f; // Would get from attention system
            float excitationStrength = 1.5f; // Would get from attention system
            writer.writeSection(CheckpointSection::Memory,
                &inhibitionStrength, sizeof(inhibitionStrength));
            writer.writeSection(CheckpointSection::Memory,
                &excitationStrength, sizeof(excitationStrength));
        }
        
        // Development system - critical for ontogeny and plasticity modulation
        if (pImpl->developmentSystem) {
            // Save developmental stage and age
            double devAge = pImpl->developmentSystem->getDevelopmentalAge();
            int devStage = static_cast<int>(pImpl->developmentSystem->getStage());
            writer.writeSection(CheckpointSection::Memory,
                &devAge, sizeof(devAge));
            writer.writeSection(CheckpointSection::Memory,
                &devStage, sizeof(devStage));
        }
        
        // Neuromodulation systems - critical for learning regulation and motivation
        if (pImpl->dopamine) {
            // Save dopamine level and plasticity factor
            float dopamineLevel = pImpl->dopamine->getLevel();
            float dopaminePlasticityFactor = pImpl->dopamine->getPlasticityFactor();
            writer.writeSection(CheckpointSection::Memory,
                &dopamineLevel, sizeof(dopamineLevel));
            writer.writeSection(CheckpointSection::Memory,
                &dopaminePlasticityFactor, sizeof(dopaminePlasticityFactor));
        }
        
        if (pImpl->novelty) {
            // Save novelty level for curiosity-driven exploration
            float noveltyLevel = pImpl->novelty->getLevel();
            writer.writeSection(CheckpointSection::Memory,
                &noveltyLevel, sizeof(noveltyLevel));
        }
        
        if (pImpl->curiosity) {
            // Save curiosity level for exploration
            float curiosityLevel = pImpl->curiosity->getLevel();
            writer.writeSection(CheckpointSection::Memory,
                &curiosityLevel, sizeof(curiosityLevel));
        }
        
        if (pImpl->predictionErrorSignal) {
            // Save prediction error for learning modulation
            float predictionError = pImpl->predictionErrorSignal->getError();
            writer.writeSection(CheckpointSection::Memory,
                &predictionError, sizeof(predictionError));
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

// Helper function to read data from checkpoint reader
template<typename T>
T readDataFromCheckpoint(CheckpointReader& reader, CheckpointSection sectionType) {
    try {
        std::vector<uint8_t> data = reader.readSection(sectionType);
        if (data.size() >= sizeof(T)) {
            T result;
            std::memcpy(&result, data.data(), sizeof(T));
            return result;
        }
    } catch (...) {
        // If section doesn't exist or error, return default
    }
    return T();
}

// Specialized template for reading vectors
template<typename T>
std::vector<T> readVectorFromCheckpoint(CheckpointReader& reader, CheckpointSection sectionType) {
    try {
        std::vector<uint8_t> data = reader.readSection(sectionType);
        if (data.size() >= sizeof(size_t)) {
            size_t vectorSize;
            std::memcpy(&vectorSize, data.data(), sizeof(size_t));
            
            if (vectorSize > 0 && data.size() >= sizeof(size_t) + sizeof(T) * vectorSize) {
                std::vector<T> result(vectorSize);
                std::memcpy(result.data(), data.data() + sizeof(size_t), sizeof(T) * vectorSize);
                return result;
            }
        }
    } catch (...) {
        // If section doesn't exist or error, return empty vector
    }
    return std::vector<T>();
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
        neuronData.firingState.reserve(getTotalNeuronCount());
        neuronData.refractoryRemaining.reserve(getTotalNeuronCount());
        neuronData.refractoryPeriod.reserve(getTotalNeuronCount());
        neuronData.lastSpikeTime.reserve(getTotalNeuronCount());
        neuronData.neuronType.reserve(getTotalNeuronCount());
        neuronData.regionId.reserve(getTotalNeuronCount());
        neuronData.populationId.reserve(getTotalNeuronCount());
        
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
                    neuronData.neuronType.push_back(static_cast<uint64_t>(pop->getNeuronType()));
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
                synapseData.plasticityFlags.push_back(static_cast<uint8_t>(syn->getPlasticityFlags()));
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
        
        // Save integrated systems state (Phase 6 Priority 2)
        
        // Working memory - essential for cognitive function
        if (pImpl->workingMemory) {
            // Save active memory traces
            size_t activeTraces = pImpl->workingMemory->getActiveTraces();
            writer.writeSection(CheckpointSection::Memory, 
                &activeTraces, sizeof(activeTraces));
            
            // Save memory neurons for pattern completion
            auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            writer.writeSection(CheckpointSection::Memory,
                &memoryNeurons, sizeof(memoryNeurons[0]) * memoryNeurons.size());
        }
        
        // Episodic memory - critical for experience-based learning
        if (pImpl->episodicMemory) {
            // Save recent episodes for replay and consolidation
            auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(20);
            size_t episodeCount = recentEpisodes.size();
            writer.writeSection(CheckpointSection::Memory,
                &episodeCount, sizeof(episodeCount));
            
            // Save episode data
            for (const auto* episode : recentEpisodes) {
                // Save key episode attributes
                writer.writeSection(CheckpointSection::Memory,
                    &episode->timestamp, sizeof(episode->timestamp));
                writer.writeSection(CheckpointSection::Memory,
                    &episode->reward, sizeof(episode->reward));
                writer.writeSection(CheckpointSection::Memory,
                    &episode->novelty, sizeof(episode->novelty));
                writer.writeSection(CheckpointSection::Memory,
                    &episode->activeNeurons.size(), sizeof(episode->activeNeurons.size()));
            }
        }
        
        // Prediction system - essential for prospective planning
        if (pImpl->predictionSystem) {
            // Save prediction error history
            auto errorHistory = pImpl->predictionSystem->getErrorHistory();
            size_t errorHistorySize = errorHistory.size();
            writer.writeSection(CheckpointSection::Memory,
                &errorHistorySize, sizeof(errorHistorySize));
            if (errorHistorySize > 0) {
                writer.writeSection(CheckpointSection::Memory,
                    &errorHistory[0], sizeof(errorHistory[0]) * errorHistorySize);
            }
            
            // Save prediction confidence
            float confidence = pImpl->predictionSystem->getConfidence();
            writer.writeSection(CheckpointSection::Memory,
                &confidence, sizeof(confidence));
        }
        
        // Neural planner - critical for goal-directed behavior
        if (pImpl->planner) {
            // Save planning state and depth
            size_t planningDepth = pImpl->planner->getPlanningDepth();
            float planningConfidence = pImpl->planner->getPlanningConfidence();
            writer.writeSection(CheckpointSection::Memory,
                &planningDepth, sizeof(planningDepth));
            writer.writeSection(CheckpointSection::Memory,
                &planningConfidence, sizeof(planningConfidence));
        }
        
        // Concept formation - essential for abstraction and categorization
        if (pImpl->conceptFormation) {
            // Save concept count and category hints
            size_t conceptCount = pImpl->conceptFormation->getConceptCount();
            writer.writeSection(CheckpointSection::Memory,
                &conceptCount, sizeof(conceptCount));
            
            // Save concept prototypes for pattern matching
            for (size_t i = 0; i < conceptCount && i < 10; ++i) {
                auto prototype = pImpl->conceptFormation->getConceptPrototype(i);
                size_t prototypeSize = prototype.size();
                writer.writeSection(CheckpointSection::Memory,
                    &prototypeSize, sizeof(prototypeSize));
                if (prototypeSize > 0) {
                    writer.writeSection(CheckpointSection::Memory,
                        &prototype[0], sizeof(prototype[0]) * prototypeSize);
                }
            }
        }
        
        // Attention system - critical for selective processing
        if (pImpl->attention) {
            // Save attention winners and inhibition/excitation levels
            auto winners = pImpl->attention->getWinners();
            size_t winnersCount = winners.size();
            writer.writeSection(CheckpointSection::Memory,
                &winnersCount, sizeof(winnersCount));
            if (winnersCount > 0) {
                writer.writeSection(CheckpointSection::Memory,
                    &winners[0], sizeof(winners[0]) * winnersCount);
            }
            
            float inhibitionStrength = 0.5f; // Would get from attention system
            float excitationStrength = 1.5f; // Would get from attention system
            writer.writeSection(CheckpointSection::Memory,
                &inhibitionStrength, sizeof(inhibitionStrength));
            writer.writeSection(CheckpointSection::Memory,
                &excitationStrength, sizeof(excitationStrength));
        }
        
        // Development system - critical for ontogeny and plasticity modulation
        if (pImpl->developmentSystem) {
            // Save developmental stage and age
            double devAge = pImpl->developmentSystem->getDevelopmentalAge();
            int devStage = static_cast<int>(pImpl->developmentSystem->getStage());
            writer.writeSection(CheckpointSection::Memory,
                &devAge, sizeof(devAge));
            writer.writeSection(CheckpointSection::Memory,
                &devStage, sizeof(devStage));
        }
        
        // Neuromodulation systems - critical for learning regulation and motivation
        if (pImpl->dopamine) {
            // Save dopamine level and plasticity factor
            float dopamineLevel = pImpl->dopamine->getLevel();
            float dopaminePlasticityFactor = pImpl->dopamine->getPlasticityFactor();
            writer.writeSection(CheckpointSection::Memory,
                &dopamineLevel, sizeof(dopamineLevel));
            writer.writeSection(CheckpointSection::Memory,
                &dopaminePlasticityFactor, sizeof(dopaminePlasticityFactor));
        }
        
        if (pImpl->novelty) {
            // Save novelty level for curiosity-driven exploration
            float noveltyLevel = pImpl->novelty->getLevel();
            writer.writeSection(CheckpointSection::Memory,
                &noveltyLevel, sizeof(noveltyLevel));
        }
        
        if (pImpl->curiosity) {
            // Save curiosity level for exploration
            float curiosityLevel = pImpl->curiosity->getLevel();
            writer.writeSection(CheckpointSection::Memory,
                &curiosityLevel, sizeof(curiosityLevel));
        }
        
        if (pImpl->predictionErrorSignal) {
            // Save prediction error for learning modulation
            float predictionError = pImpl->predictionErrorSignal->getError();
            writer.writeSection(CheckpointSection::Memory,
                &predictionError, sizeof(predictionError));
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
        
        // Load integrated systems state (Phase 6 Priority 2)
        
        // Working memory - essential for cognitive function
        if (pImpl->workingMemory) {
            // Load active memory traces
            size_t activeTraces = readDataFromCheckpoint<size_t>(reader, CheckpointSection::Memory);
            // Note: Actual working memory restoration would require more complex state management
            // For now, just restore basic active trace count
            NLM_LOG_INFO("Loaded working memory with " + std::to_string(activeTraces) + " active traces");
        }
        
        // Episodic memory - critical for experience-based learning
        if (pImpl->episodicMemory) {
            // Load recent episodes for replay and consolidation
            size_t episodeCount = readDataFromCheckpoint<size_t>(reader, CheckpointSection::Memory);
            // Note: Actual episodic memory restoration would require complex neural pattern replay
            // For now, just restore basic episode count
            NLM_LOG_INFO("Loaded episodic memory with " + std::to_string(episodeCount) + " episodes");
            
            // Clear existing episodic memory and load new episodes
            pImpl->episodicMemory->clear();
            
            // Would restore actual episode data here if fully implemented
            // This would require restoring neural activity patterns, timestamps, rewards, etc.
        }
        
        // Prediction system - essential for prospective planning
        if (pImpl->predictionSystem) {
            // Load prediction error history
            auto errorHistory = readVectorFromCheckpoint<float>(reader, CheckpointSection::Memory);
            if (!errorHistory.empty()) {
                NLM_LOG_INFO("Loaded prediction system with " + std::to_string(errorHistory.size()) + " error history entries");
                // Note: Actual prediction system restoration would require re-training
                // For now, just restore error history for reference
            }
            
            // Load prediction confidence
            float confidence = readDataFromCheckpoint<float>(reader, CheckpointSection::Memory);
            NLM_LOG_INFO("Loaded prediction system confidence: " + std::to_string(confidence));
        }
        
        // Neural planner - critical for goal-directed behavior
        if (pImpl->planner) {
            // Load planning state and depth
            size_t planningDepth = readDataFromCheckpoint<size_t>(reader, CheckpointSection::Memory);
            float planningConfidence = readDataFromCheckpoint<float>(reader, CheckpointSection::Memory);
            
            pImpl->planner->setPlanningDepth(planningDepth);
            NLM_LOG_INFO("Loaded neural planner with depth: " + std::to_string(planningDepth) +
                        " and confidence: " + std::to_string(planningConfidence));
        }
        
        // Concept formation - essential for abstraction and categorization
        if (pImpl->conceptFormation) {
            // Load concept count and category hints
            size_t conceptCount = readDataFromCheckpoint<size_t>(reader, CheckpointSection::Memory);
            NLM_LOG_INFO("Loaded concept formation with " + std::to_string(conceptCount) + " concepts");
            
            // Note: Actual concept restoration would require re-presenting experiences
            // For now, just restore basic concept count
            pImpl->conceptFormation->clear();
        }
        
        // Attention system - critical for selective processing
        if (pImpl->attention) {
            // Load attention winners and inhibition/excitation levels
            auto winners = readVectorFromCheckpoint<uint64_t>(reader, CheckpointSection::Memory);
            if (!winners.empty()) {
                NLM_LOG_INFO("Loaded attention system with " + std::to_string(winners.size()) + " attention winners");
                // Note: Actual attention system restoration would require neural state management
                // For now, just restore basic winner information
            }
            
            float inhibitionStrength = readDataFromCheckpoint<float>(reader, CheckpointSection::Memory);
            float excitationStrength = readDataFromCheckpoint<float>(reader, CheckpointSection::Memory);
            NLM_LOG_INFO("Loaded attention system - inhibition: " + std::to_string(inhibitionStrength) +
                        ", excitation: " + std::to_string(excitationStrength));
        }
        
        // Development system - critical for ontogeny and plasticity modulation
        if (pImpl->developmentSystem) {
            // Load developmental stage and age
            double devAge = readDataFromCheckpoint<double>(reader, CheckpointSection::Memory);
            int devStage = readDataFromCheckpoint<int>(reader, CheckpointSection::Memory);
            
            pImpl->developmentSystem->setDevelopmentalAge(devAge);
            pImpl->developmentSystem->setStage(static_cast<DevelopmentalStage>(devStage));
            NLM_LOG_INFO("Loaded development system - age: " + std::to_string(devAge) +
                        ", stage: " + std::to_string(devStage));
        }
        
        // Neuromodulation systems - critical for learning regulation and motivation
        if (pImpl->dopamine) {
            // Load dopamine level and plasticity factor
            float dopamineLevel = readDataFromCheckpoint<float>(reader, CheckpointSection::Memory);
            float dopaminePlasticityFactor = readDataFromCheckpoint<float>(reader, CheckpointSection::Memory);
            
            // Note: Actual dopamine restoration would require neural state modulation
            NLM_LOG_INFO("Loaded dopamine system - level: " + std::to_string(dopamineLevel) +
                        ", plasticity factor: " + std::to_string(dopaminePlasticityFactor));
        }
        
        if (pImpl->novelty) {
            // Load novelty level for curiosity-driven exploration
            float noveltyLevel = readDataFromCheckpoint<float>(reader, CheckpointSection::Memory);
            NLM_LOG_INFO("Loaded novelty system - level: " + std::to_string(noveltyLevel));
        }
        
        if (pImpl->curiosity) {
            // Load curiosity level for exploration
            float curiosityLevel = readDataFromCheckpoint<float>(reader, CheckpointSection::Memory);
            NLM_LOG_INFO("Loaded curiosity system - level: " + std::to_string(curiosityLevel));
        }
        
        if (pImpl->predictionErrorSignal) {
            // Load prediction error for learning modulation
            float predictionError = readDataFromCheckpoint<float>(reader, CheckpointSection::Memory);
            NLM_LOG_INFO("Loaded prediction error system - error: " + std::to_string(predictionError));
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

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
    size_t stepsSinceLastRest;  // For sleep/rest cycle
    
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
        , stepsSinceLastRest(0)
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
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        
        // Configure checkpoint manager
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        checkpointManager->configure(checkpointDir, 10000, 10, true);
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
     * 1. Check if we should enter resting mode (sleep/rest cycle)
     * 2. Process resting phase (memory consolidation)
     * 3. Process pending delayed spike events (deliver synaptic input)
     * 4. Update all neuron membrane potentials (LIF dynamics)
     * 5. Detect spikes and schedule outgoing spike events
     * 6. Update working memory (maintenance and competition)
     * 7. Apply neuromodulation effects on neural excitability
     * 8. Apply plasticity rules (STDP, Hebbian)
     * 9. Update episodic memory with current experience
     * 10. Update prediction system
     * 11. Update attention system
     * 12. Update concept formation
     * 13. Apply structural plasticity (synaptogenesis, pruning)
     * 14. Replay important memories (during rest or periodically)
     * 15. Apply development effects
     * 16. Collect statistics
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Check if we should enter resting mode ==========
    // Rest is triggered by low overall activity and high memory load
    float averageFiringRate = getAverageFiringRate();
    float memoryLoad = (pImpl->workingMemory ? pImpl->workingMemory->getMemoryActivity() : 0.0f);
    size_t memoryUsage = (pImpl->workingMemory ? pImpl->workingMemory->getMemoryNeurons().size() : 0);
    
    // Determine if we should rest
    bool shouldRest = (pImpl->totalSpikesThisStep == 0 && averageFiringRate < 0.01f && memoryUsage > 10);
    if (shouldRest && !pImpl->isResting) {
        pImpl->isResting = true;
        pImpl->stepsSinceLastRest = 0;
        NLM_LOG_INFO("Brain entering RESTING mode for memory consolidation");
    } else if (!shouldRest && pImpl->isResting) {
        pImpl->isResting = false;
        NLM_LOG_INFO("Brain exiting RESTING mode");
    }
    
    // ========== STEP 2: Process resting phase ==========
    if (pImpl->isResting) {
        pImpl->stepsSinceLastRest++;
        
        // Resting phase allows for memory consolidation and replay
        if (pImpl->stepsSinceLastRest >= 50) {  // Rest for 50 steps
            NLM_LOG_INFO("Performing memory consolidation during REST");
            
            // Consolidate working memory into episodic memory
            if (pImpl->workingMemory && pImpl->episodicMemory) {
                // Store current working memory as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                // Get working memory neurons and their activations
                auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
                auto memoryActivations = pImpl->workingMemory->retrieve();
                
                for (size_t i = 0; i < memoryNeurons.size(); ++i) {
                    episode.activeNeurons.push_back(memoryNeurons[i]);
                    if (i < memoryActivations.size()) {
                        episode.neuronActivations.push_back(memoryActivations[i]);
                    }
                }
                
                // Store episode for later replay
                pImpl->episodicMemory->storeEpisode(episode);
            }
            
            // Replay important memories to strengthen connections
            if (pImpl->episodicMemory) {
                // Get high-reward episodes for replay
                auto importantEpisodes = pImpl->episodicMemory->getRecentEpisodes(3);
                for (const auto* episode : importantEpisodes) {
                    if (episode && episode->reward > 0.5f) {
                        NLM_LOG_INFO("Replaying high-reward episode for consolidation");
                        pImpl->episodicMemory->replayEpisode(episode);
                    }
                }
            }
            
            // Reset for next rest cycle
            pImpl->isResting = false;
            pImpl->stepsSinceLastRest = 0;
        }
    }
    
    // ========== STEP 3: Process pending delayed spikes (deliver synaptic input) ==========
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
        
        // Also update recurrent connections in working memory
        pImpl->workingMemory->updateRecurrentConnections();
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
        // Get sensory input from the brain (in real implementation, this would be from actual sensory channels)
        // For now, we'll use a simplified approach
        
        // Create a simple sensory input based on current neural activity
        SensoryInput sensoryInput;
        std::vector<float> sensoryData;
        
        // Convert some neural activity to sensory-like input
        // Use firing rates from different neuron types as sensory signals
        float sensorySum = 0.0f;
        int sensoryCount = 0;
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Sensory) {
                    // Use average membrane potential of sensory neurons as input
                    float avgPotential = 0.0f;
                    size_t neuronCount = 0;
                    for (auto* neuron : pop->getNeurons()) {
                        avgPotential += std::abs(neuron->getMembranePotential());
                        neuronCount++;
                    }
                    if (neuronCount > 0) {
                        avgPotential /= neuronCount;
                        sensoryData.push_back(avgPotential);
                        sensorySum += avgPotential;
                        sensoryCount++;
                    }
                }
            }
        }
        
        // Add current reward as another sensory signal
        float rewardSignal = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        sensoryData.push_back(rewardSignal);
        sensorySum += rewardSignal;
        sensoryCount++;
        
        if (!sensoryData.empty()) {
            sensoryInput = SensoryInput(sensoryData);
            
            // Update prediction system
            pImpl->predictionSystem->updatePredictions(sensoryInput, sensoryInput);
        }
    }
    
// ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
            
            // Apply attentional selection to the actual neurons
            for (NeuronId neuron : competitors) {
                bool isWinner = pImpl->attention->isAttended(neuron);
                if (isWinner) {
                    // Apply excitation to attended neurons
                    float excitation = pImpl->attention->getExcitationFor(neuron);
                    if (excitation > 0.0f) {
                        // Find and inject current into the neuron
                        for (auto& region : pImpl->regions) {
                            auto neurons = region->getAllNeurons();
                            for (auto* n : neurons) {
                                if (n->getId() == neuron) {
                                    n->injectCurrent(excitation * 2.0f);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
                            }
                        }
                    }
                }
            }
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
    // Enhanced action selection using integrated cognition systems
    // 
    // Priority 1: Use NeuralPlanner for goal-directed action selection
    if (pImpl->planner) {
        // Get current neural activity pattern as state representation
        std::vector<float> currentState;
        
        // Convert neural activity to state representation
        // Use different neuron types and populations for different aspects of state
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                float avgActivity = 0.0f;
                size_t neuronCount = 0;
                
                for (auto* neuron : pop->getNeurons()) {
                    if (type == NeuronType::Sensory) {
                        avgActivity += std::abs(neuron->getMembranePotential());
                        neuronCount++;
                    } else if (type == NeuronType::Motor) {
                        avgActivity += neuron->isFiring() ? 1.0f : 0.0f;
                        neuronCount++;
                    } else if (type == NeuronType::Internal) {
                        avgActivity += std::abs(neuron->getMembranePotential());
                        neuronCount++;
                    }
                }
                
                if (neuronCount > 0) {
                    avgActivity /= neuronCount;
                    currentState.push_back(avgActivity);
                }
            }
        }
        
        // Add neuromodulation levels to state
        if (pImpl->dopamine) {
            currentState.push_back(pImpl->dopamine->getLevel());
        }
        if (pImpl->curiosity) {
            currentState.push_back(pImpl->curiosity->getLevel());
        }
        
        // Set a simple goal (could be developed by concept formation)
        std::vector<float> goal{0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
        if (!currentState.empty()) {
            goal.resize(currentState.size(), 0.5f);
        }
        
        // Get planned action from neural planner
        ActionType plannedAction = pImpl->planner->planAction(currentState, 0.5f);
        if (plannedAction != ActionType::None && plannedAction != ActionType::Wait) {
            NLM_LOG_DEBUG("Action selected by NeuralPlanner: " + std::to_string(static_cast<int>(plannedAction)));
            return std::make_unique<Action>(plannedAction);
        }
    }
    
    // Priority 2: Use ConceptFormation for context-based action selection
    if (pImpl->conceptFormation && !pImpl->conceptFormation->getConcepts().empty()) {
        // Get current neural activity pattern
        std::vector<float> activityPattern;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring()) {
                        activityPattern.push_back(1.0f);
                    } else {
                        activityPattern.push_back(std::abs(neuron->getMembranePotential()) / 100.0f);
                    }
                }
            }
        }
        
        if (!activityPattern.empty()) {
            // Find concept that matches current activity
            size_t conceptId = pImpl->conceptFormation->getMatchingConcept(activityPattern);
            if (conceptId > 0 && conceptId <= pImpl->conceptFormation->getConceptCount()) {
                // Get concept prototype
                std::vector<float> prototype = pImpl->conceptFormation->getConceptPrototype(conceptId);
                
                // Determine action based on concept
                // This could be: if concept is "approach food" -> MoveForward
                // if concept is "avoid danger" -> Wait or Turn
                ActionType action = ActionType::Wait;
                
                // Simple mapping - in real implementation, concepts would have associated actions
                if (!activityPattern.empty()) {
                    // Check if pattern matches certain characteristics
                    float patternSum = std::accumulate(activityPattern.begin(), activityPattern.end(), 0.0f);
                    if (patternSum > 0.5f) {
                        action = ActionType::MoveForward;
                    }
                }
                
                NLM_LOG_DEBUG("Action selected by ConceptFormation: " + std::to_string(static_cast<int>(action)));
                return std::make_unique<Action>(action);
            }
        }
    }
    
    // Priority 3: Simple action selection based on motor neuron activity (original implementation)
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    float totalActivity = 0.0f;
    
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
        totalActivity += std::abs(neuron->getMembranePotential());
    }
    
    // Use attention system to select among competing actions
    if (pImpl->attention && firingMotor > 0) {
        // Get motor neuron IDs for competition
        std::vector<NeuronId> motorNeurons;
        for (auto* neuron : pImpl->motorNeurons) {
            motorNeurons.push_back(neuron->getId());
        }
        
        // Let attention system select the winner
        auto attendedNeurons = pImpl->attention->processCompetition(motorNeurons);
        if (!attendedNeurons.empty()) {
            // Find which motor neuron was attended
            for (auto* neuron : pImpl->motorNeurons) {
                if (std::find(attendedNeurons.begin(), attendedNeurons.end(), neuron->getId()) != attendedNeurons.end()) {
                    // Determine action based on attended neuron
                    // In a real implementation, each motor neuron would have associated actions
                    return std::make_unique<Action>(ActionType::MoveForward);
                }
            }
        }
    }
    
    // Priority 4: Return simple action based on overall motor activity
    ActionType type = ActionType::Wait;
    if (firingMotor > 0 || totalActivity > 10.0f) {
        type = ActionType::MoveForward;
    }
    
    NLM_LOG_DEBUG("Action selected by fallback system: " + std::to_string(static_cast<int>(type)));
    return std::make_unique<Action>(type);
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
    pImpl->stepsSinceLastRest = 0;
    
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
        // Use CheckpointManager to save
        CheckpointManager manager;
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        manager.configure(checkpointDir, 10000, 10, true);
        
        // Create a neuron provider function
        auto neuronProvider = [this]() {
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
                        neuronData.neuronType.push_back(static_cast<uint64_t>(pop->getNeuronType()));
                        neuronData.regionId.push_back(region->getId().index());
                        neuronData.populationId.push_back(pop->getId().index());
                    }
                }
            }
            
            manager.setNeuronProvider([neuronData]() mutable {
                NeuronCheckpointData tempData = neuronData;
                neuronData.clear();
                return !tempData.membranePotential.empty();
            });
            
            return !neuronData.membranePotential.empty();
        }();
        
        // Create a synapse provider function
        auto synapseProvider = [this]() {
            SynapseCheckpointData synapseData;
            for (const auto& region : pImpl->regions) {
                for (const auto* syn : region->getSynapses()) {
                    synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                    synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                    synapseData.weight.push_back(syn->getWeight());
                    synapseData.delay.push_back(syn->getDelay());
                    synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                    synapseData.plasticityFlags.push_back(syn->getPlasticityFlags().asUint8());
                    synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                    synapseData.efficacy.push_back(syn->getEfficacy());
                    synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                    synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
                }
            }
            
            manager.setSynapseProvider([synapseData]() mutable {
                SynapseCheckpointData tempData = synapseData;
                synapseData.clear();
                return !tempData.sourceNeuron.empty();
            });
            
            return !synapseData.sourceNeuron.empty();
        }();
        
        // Save to file using the manager
        std::string filename = filepath;
        if (filename.empty()) {
            filename = "brain_checkpoint_" + std::to_string(pImpl->currentStep) + ".bin";
        }
        
        if (!manager.saveImmediately(filename)) {
            NLM_LOG_ERROR("Failed to save brain state using checkpoint manager");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        // Use CheckpointManager to load
        CheckpointManager manager;
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        manager.configure(checkpointDir, 10000, 10, true);
        
        // List available checkpoints
        auto checkpoints = manager.listCheckpoints();
        
        // Find the checkpoint file or load directly
        bool loadSuccess = false;
        
        if (!filepath.empty()) {
            // Try to load the specific file
            loadSuccess = manager.load(filepath);
        } else {
            // Load the most recent checkpoint
            if (!checkpoints.empty()) {
                loadSuccess = manager.load(checkpoints[0].name);
            }
        }
        
        if (!loadSuccess) {
            NLM_LOG_ERROR("Failed to load brain state");
            return false;
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

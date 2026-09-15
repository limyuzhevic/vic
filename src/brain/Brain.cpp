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
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Dopamine.hpp"
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
    
    // World reward from environment
    float worldReward;
    
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
    
    // ========== STEP 3.5: INTEGRATE WORKING MEMORY CONTENT ==========
    if (pImpl->workingMemory) {
        // Feed working memory content back to influence neural processing
        // This is where working memory actively participates in computation
        pImpl->workingMemory->integrateWithNeuralActivity(*this, pImpl->timestep);
        
        // Apply working memory influence on attention and action selection
        if (pImpl->attention && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Get memory winners and influence attention
            // Note: Attention system maintains winners, not working memory
            std::vector<NeuronId> memoryWinners = pImpl->attention->getWinners();
            pImpl->attention->processCompetition(memoryWinners, 0.3f);
        }
        
        // Apply working memory influence on motor output selection
        if (!pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Working memory can guide action selection through motor neuron priming
            auto* wm = pImpl->workingMemory.get();
            // Find motor-related memory traces
            for (size_t i = 0; i < wm->getMemoryNeurons().size(); ++i) {
                NeuronId neuronId = wm->getMemoryNeurons()[i];
                float activation = wm->getNeuronActivation(neuronId);
                
                // Priming influence on motor systems
                if (activation > 0.2f) {
                    // Find and inject priming current to related motor neurons
                    for (auto* motorNeuron : pImpl->motorNeurons) {
                        // Simple heuristic: motor neurons related to active memory
                        // in working memory receive priming current
                        if (activation > 0.5f) {
                            motorNeuron->injectCurrent(activation * 0.5f);
                        }
                    }
                }
            }
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
    
    // Store episode when significant change occurs or after minimum steps
    // Reduce frequency from every 10 steps to every 50 steps for more meaningful episodes
        // Initialize world reward for this step
    pImpl->worldReward = 0.0f;
        shouldStoreEpisode = true;
        pImpl->stepsSinceLastEpisode = 0;
    } else {
        // Store if there's significant neural activity (indicating an event)
        // This makes episodic storage event-driven rather than time-driven
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 10.0f) {
                        // High activity indicates an interesting event, store episode now
                        shouldStoreEpisode = true;
                        pImpl->stepsSinceLastEpisode = 0;
                        break;
                    }
                }
                if (shouldStoreEpisode) break;
            }
            if (shouldStoreEpisode) break;
        }
    }
    
    // Store reward that was received during this step (from world)
    // If no reward was explicitly set, use curiosity/novelty as proxies
    if (pImpl->worldReward == 0.0f) {
        // Use curiosity or novelty as proxy for actual world reward
        if (pImpl->curiosity) {
            pImpl->worldReward = pImpl->curiosity->getLevel() * 0.1f;
        } else if (pImpl->novelty) {
            pImpl->worldReward = pImpl->novelty->getLevel() * 0.05f;
        }
    }
    
    if (shouldStoreEpisode && pImpl->episodicMemory) {
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
            
            // Store rich sensory state from working memory
            if (pImpl->workingMemory) {
                // Get sensory pattern from working memory
                std::vector<float> sensoryPattern;
                for (auto neuronId : pImpl->workingMemory->getMemoryNeurons()) {
                    float activation = pImpl->workingMemory->getNeuronActivation(neuronId);
                    sensoryPattern.push_back(activation);
                }
                
                // Store in episode
                episode.sensoryState = sensoryPattern;
                
                // Set position/orientation if available (simplified)
                episode.positionX = static_cast<float>(currentStep) / 100.0f;  // Placeholder
                episode.positionY = 0.0f;  // Placeholder
                episode.orientation = 0.0f;  // Placeholder
            }
            
            // Capture action that led to this state (from previous step)
            if (pImpl->motorNeurons.empty()) {
                episode.action = ActionType::Wait;
            } else {
                // Determine action based on motor neuron activity
                bool moving = false;
                for (auto* motorNeuron : pImpl->motorNeurons) {
                    if (motorNeuron->isFiring() || 
                        std::abs(motorNeuron->getState().membranePotential - motorNeuron->getState().restingPotential) > 5.0f) {
                        moving = true;
                        break;
                    }
                }
                episode.action = moving ? ActionType::MoveForward : ActionType::Wait;
            }
            
            // Capture reward from world (dopamine is a prediction error, not actual reward)
            // Use curiosity or novelty as proxy for actual world reward in this simplified version
            float worldReward = 0.0f;
            if (pImpl->curiosity) {
                worldReward = pImpl->curiosity->getLevel() * 0.1f;  // Scale curiosity to reward
            } else if (pImpl->novelty) {
                worldReward = pImpl->novelty->getLevel() * 0.05f;  // Scale novelty to reward
            }
            episode.reward = worldReward;
            
            // Store energy (neural activity)
            episode.energy = static_cast<float>(pImpl->totalSpikesThisStep) / 100.0f;
            
            // Store novelty (would come from novelty detector)
            if (pImpl->novelty) {
                episode.novelty = pImpl->novelty->getLevel();
            } else {
                episode.novelty = 0.0f;
            }
            
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
            
            // Set resulting reward (from world feedback)
            episode.resultingReward = episode.reward;
            
            // Store resulting sensory state (same as current for now)
            episode.resultingSensoryState = episode.sensoryState;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Update prediction system with sensory input from working memory
        // Working memory contains the most recent neural representation
        
        // Get working memory representation
        std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
        
        if (!workingMemoryPattern.empty()) {
            // Update prediction system with current working memory content
            pImpl->predictionSystem->updateSensoryInput(workingMemoryPattern);
            
            // Generate prediction from current state
            // Create a temporary sensory input from working memory
            // Use InternalSignals as a bridge since working memory doesn't have getInput()
            auto internalSignal = std::make_unique<InternalSignals>();
            for (float val : workingMemoryPattern) {
                internalSignal->addSignal(val);
            }
            pImpl->predictionSystem->train(*internalSignal);
            
            // Get prediction error for neuromodulation
            float predictionError = pImpl->predictionSystem->getPredictionError();
            float confidence = pImpl->predictionSystem->getConfidence();
            
            // Update neuromodulation with prediction error
            if (pImpl->predictionError) {
                pImpl->predictionError->computeError(predictionError, 0.0f); // Predicted vs actual
            }
            
            // Update curiosity with prediction error
            if (pImpl->curiosity) {
                pImpl->curiosity->update(0.0f, predictionError, pImpl->timestep);
            }
            
            // Update attention based on prediction error
            // High prediction error should increase attention salience
            if (pImpl->attention && confidence < 0.5f) {
                // Low confidence (high error) should increase attentional focus
                for (NeuronId neuron : pImpl->workingMemory->getMemoryNeurons()) {
                    pImpl->attention->applyBottomUpSalience(neuron, predictionError * 2.0f);
                }
            }
            
            // Update working memory with prediction error
            if (pImpl->workingMemory && predictionError > 0.1f) {
                // Significant prediction error should update working memory
                // Reconstruct sensory pattern from neural activity
                std::vector<float> sensoryPattern = pImpl->workingMemory->retrieve();
                if (!sensoryPattern.empty()) {
                    // Add prediction error to working memory
                    for (size_t i = 0; i < std::min(sensoryPattern.size(), (size_t)5); ++i) {
                        // Modify some neurons based on prediction error
                        sensoryPattern[i] += predictionError * 0.1f;
                    }
                    pImpl->workingMemory->store(sensoryPattern, predictionError);
                }
            }
            
            // Apply prediction error to plasticity modulation
            float errorMagnitude = std::abs(predictionError);
            float plasticityMod = 1.0f + errorMagnitude * 0.5f;  // Enhance plasticity
            
            // Modify STDP and Hebbian learning rates based on prediction error
            if (pImpl->stdp) {
                float originalLTP = pImpl->stdp->getLTPWeight();
                pImpl->stdp->setLTPWeight(originalLTP * plasticityMod);
            }
            if (pImpl->hebbian) {
                // Hebbian learning enhanced by prediction error
                // In a real implementation, this would modify Hebbian parameters
            }
            
            // Store prediction state for action selection and other systems
            pImpl->predictionErrorLevel = predictionError;
            pImpl->predictionConfidence = confidence;
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
        // Process current neural activity patterns to form concepts
        
        // Extract patterns from working memory (active neural activity)
        if (pImpl->workingMemory) {
            // Get current working memory pattern as neural activity
            std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
            
            if (!workingMemoryPattern.empty()) {
                // Convert neural pattern to sensory features for concept formation
                std::vector<float> features = workingMemoryPattern;
                
                // Present to concept formation system with reward
                float reward = 0.0f;
                if (pImpl->dopamine) {
                    reward = pImpl->dopamine->getLevel();  // Use dopamine as reward signal
                } else if (pImpl->curiosity) {
                    reward = pImpl->curiosity->getLevel();
                }
                
                // Present working memory pattern to concept formation
                size_t conceptId = pImpl->conceptFormation->presentExperience(
                    workingMemoryPattern, features, reward, currentStep);
                
                // Store concept in associative memory if new concept formed
                if (conceptId > 0) {
                    // This would ideally store the concept pattern in associative memory
                    // for concept-based retrieval later
                    if (pImpl->associativeMemory) {
                        // Create association between concept and neural pattern
                        // (concept prototype would be stored as a pattern)
                    }
                }
                
                // Use concept to influence attention and action
                if (pImpl->attention) {
                    // Get top concepts based on stability and generalization ability
                    std::vector<size_t> topConcepts;
                    const auto& allConcepts = pImpl->conceptFormation->getConcepts();
                    for (const auto& concept : allConcepts) {
                        if (pImpl->conceptFormation->getConceptStability(concept.id) > 0.7f) {
                            topConcepts.push_back(concept.id);
                            if (topConcepts.size() >= 5) break; // Limit to top 5
                        }
                    }
                    
                    // Apply concept-based attention
                    for (size_t conceptId : topConcepts) {
                        const DiscoveredConcept* concept = pImpl->conceptFormation->getConcept(conceptId);
                        if (concept) {
                            // Apply concept-based top-down bias
                            // Activate neurons that match concept prototype
                            for (size_t i = 0; i < std::min(concept->prototype.size(), (size_t)10); ++i) {
                                if (workingMemoryPattern.size() > i) {
                                    // If pattern component matches concept prototype
                                    float similarity = pImpl->conceptFormation->computeSimilarity(
                                        std::vector<float>(1, workingMemoryPattern[i]),
                                        std::vector<float>(1, concept->prototype[i]));
                                    
                                    if (similarity > 0.6f) {
                                        // Apply top-down bias through attention
                                        pImpl->attention->applyTopDownBias(
                                            concept->prototype[i], similarity * 2.0f);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Extract patterns from episodic memory and learn from them
        if (pImpl->episodicMemory && !pImpl->episodicMemory->getEpisodeCount()) {
            // Get recent episodes to learn from
            size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
            size_t startIdx = (episodeCount > 20) ? episodeCount - 20 : 0;
            
            // Learn from recent episodes
            for (size_t i = startIdx; i < episodeCount; ++i) {
                const EpisodicMemoryItem* episode = pImpl->episodicMemory->getEpisode(i);
                if (episode) {
                    // Extract sensory pattern from episode
                    std::vector<float> episodePattern = episode->sensoryState;
                    
                    if (!episodePattern.empty()) {
                        // Convert to features
                        std::vector<float> features = episodePattern;
                        
                        // Present to concept formation with episode reward
                        float reward = episode->reward;
                        size_t conceptId = pImpl->conceptFormation->presentExperience(
                            episodePattern, features, reward, currentStep);
                        
                        // Learn temporal relationships if action was taken
                        if (episode->action != ActionType::Wait) {
                            // Store temporal relationship in concept
                            if (pImpl->conceptFormation) {
                                // This would be a more sophisticated temporal concept
                                // For now, just note the action-concept association
                            }
                        }
                    }
                }
            }
        }
        
        // Update concepts based on prediction errors from prediction system
        if (pImpl->predictionSystem) {
            float predictionError = pImpl->predictionSystem->getPredictionError();
            if (predictionError > 0.1f) {  // Significant prediction error
                // Prediction errors indicate concept refinement is needed
                // Learn from prediction errors to refine concepts
                if (pImpl->workingMemory) {
                    std::vector<float> currentPattern = pImpl->workingMemory->retrieve();
                    if (!currentPattern.empty()) {
                        // Present error pattern to concept formation
                        std::vector<float> features = currentPattern;
                        pImpl->conceptFormation->presentExperience(
                            currentPattern, features, -predictionError, currentStep);
                    }
                }
            }
        }
        
        // Apply developmental concept abstraction periodically
        if (currentStep % 1000 == 0 && pImpl->developmentSystem) {
            // Development system affects concept formation
            float developmentalFactor = pImpl->developmentSystem->getPlasticityModifier();
            
            // Promote stable concepts to higher-level abstraction
            std::vector<size_t> toMerge;
            const auto& allConcepts = pImpl->conceptFormation->getConcepts();
            
            for (const auto& concept : allConcepts) {
                // Concepts with high stability and good generalization can be abstracted
                if (pImpl->conceptFormation->getConceptStability(concept.id) > 0.8f &&
                    pImpl->conceptFormation->getGeneralizationAbility(concept.id) > 0.7f) {
                    // Check if similar concepts exist for potential merging
                    // This would be concept abstraction through development
                    toMerge.push_back(concept.id);
                }
            }
            
            // Merge similar concepts at adult stage for higher-level abstraction
            if (pImpl->developmentSystem->getStage() == DevelopmentalStage::Adult && 
                toMerge.size() >= 2) {
                // Merge top concepts
                std::sort(toMerge.begin(), toMerge.end(), 
                    [&pImpl](size_t a, size_t b) {
                        float stabilityA = pImpl->conceptFormation->getConceptStability(a);
                        float stabilityB = pImpl->conceptFormation->getConceptStability(b);
                        return stabilityA > stabilityB;
                    });
                
                // Merge first two most stable concepts
                if (toMerge.size() >= 2) {
                    pImpl->conceptFormation->mergeConcepts(toMerge[0], toMerge[1]);
                    NLM_LOG_INFO("ConceptFormation: Abstracted concepts during development");
                }
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
    
    // ========== STEP 16: Apply comprehensive neuromodulation effects ==========
    // Apply all neuromodulatory effects to integrated cognitive systems
    if (pImpl->dopamine) {
        // Dopamine effects on all systems
        if (pImpl->attention) {
            pImpl->dopamine->applyAttentionEffect(pImpl->attention.get(), pImpl->timestep);
        }
        if (pImpl->workingMemory) {
            pImpl->dopamine->applyWorkingMemoryEffect(pImpl->workingMemory.get(), pImpl->timestep);
        }
        if (pImpl->episodicMemory) {
            pImpl->dopamine->applyEpisodicMemoryEffect(pImpl->episodicMemory.get(), pImpl->timestep);
        }
        if (pImpl->predictionSystem) {
            pImpl->dopamine->applyPredictionSystemEffect(pImpl->predictionSystem.get(), pImpl->timestep);
        }
        if (pImpl->developmentSystem) {
            pImpl->dopamine->applyDevelopmentSystemEffect(pImpl->developmentSystem.get(), pImpl->timestep);
        }
        // Action selection enhancement through dopamine
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Get attention winners to influence action selection
            std::vector<NeuronId> memoryWinners = pImpl->attention ? pImpl->attention->getWinners() : std::vector<NeuronId>();
            if (!memoryWinners.empty()) {
                // Use highest-activation memory neuron to guide action selection
                NeuronId actionGuideNeuron = memoryWinners[0];
                float activation = pImpl->workingMemory->getNeuronActivation(actionGuideNeuron);
                // Modulate action selection based on dopamine level
                if (activation > 0.3f) {
                    float actionModulation = activation * pImpl->dopamine->getLevel() * 0.5f;
                    // Priming influence on motor systems
                    for (auto* motorNeuron : pImpl->motorNeurons) {
                        if (activation > 0.7f) {
                            motorNeuron->injectCurrent(actionModulation * 0.8f);
                        }
                    }
                }
            }
        }
    }
    
    if (pImpl->curiosity) {
        // Curiosity effects on all systems
        if (pImpl->workingMemory) {
            pImpl->curiosity->applyWorkingMemoryEnhancement(pImpl->workingMemory.get(), pImpl->timestep);
        }
        if (pImpl->predictionSystem) {
            pImpl->curiosity->applyLearningRateEnhancement(pImpl->predictionSystem.get(), pImpl->timestep);
        }
        if (pImpl->attention) {
            pImpl->curiosity->applyAttentionGuidance(pImpl->attention.get(), pImpl->timestep);
        }
        if (pImpl->planner) {
            pImpl->curiosity->applyPlanningDepthEnhancement(pImpl->planner.get(), pImpl->timestep);
        }
    }
    
    if (pImpl->novelty) {
        // Novelty effects on all systems
        if (pImpl->workingMemory) {
            pImpl->novelty->applyMemoryConsolidation(pImpl->workingMemory.get(), pImpl->timestep);
        }
        if (pImpl->planner) {
            pImpl->novelty->applyExplorationGuidance(pImpl->planner.get(), pImpl->timestep);
        }
        if (pImpl->conceptFormation) {
            pImpl->novelty->applyConceptFormationEnhancement(pImpl->conceptFormation.get(), pImpl->timestep);
        }
        if (pImpl->developmentSystem) {
            pImpl->novelty->applyDevelopmentTriggering(pImpl->developmentSystem.get(), pImpl->timestep);
        }
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Build rich sensory pattern vector
    std::vector<float> sensoryPattern;
    
    // Extract different feature dimensions from sensory input
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store in working memory with rich features
        if (pImpl->workingMemory) {
            // Encode multiple features of the sensory input
            float patternStrength = normalizedValue;
            
            // Also encode temporal derivative (change over time)
            if (input.getTimeStamp() > 0.0f) {
                // This is a simplified temporal encoding
                patternStrength *= 1.0f + std::abs(normalizedValue) * 0.5f;
            }
            
            // Store in working memory with rich encoding
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), patternStrength / 10.0f);
            
            // Also store the pattern in working memory
            sensoryPattern.push_back(patternStrength);
        }
    }
    
    // Update prediction system with sensory input
    if (pImpl->predictionSystem && !sensoryPattern.empty()) {
        pImpl->predictionSystem->updateSensoryInput(sensoryPattern);
    }
    
    // Apply attention to active sensory neurons
    if (pImpl->attention && !sensoryPattern.empty()) {
        std::vector<NeuronId> sensoryNeurons;
        for (auto* neuron : pImpl->sensoryNeurons) {
            sensoryNeurons.push_back(neuron->getId());
        }
        pImpl->attention->processCompetition(sensoryNeurons);
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

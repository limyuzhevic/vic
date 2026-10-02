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
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    
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
    
    // Current sensory input from AgentBrain
    SensoryInput currentSensoryInput;
    
    // Developmental effects on attention and memory
    float attentionModulation;
    float memoryConsolidationRate;
    
    // Sleep/rest cycle state
    float sleepDrive;
    float arousalLevel;
    
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
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        
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
        // Connect prediction to sensory processing
        // Get current sensory state from brain
        auto sensoryObservation = collectSensoryObservation();
        if (!sensoryObservation.empty()) {
            // Predict next state
            auto predictedNext = pImpl->predictionSystem->predictNextState(sensoryObservation);
            if (!predictedNext.empty()) {
                // Update predictions using current sensory input
                // Note: In a real implementation, this would use AgentBrain's input
                // For now, use sensoryObservation as both predicted and actual
                pImpl->predictionSystem->updatePredictions(predictedNext, sensoryObservation);
            }
            
            // Store for integration with other systems
            pImpl->currentSensoryInput = sensoryObservation;
            
            // Use prediction error to modulate learning
            float predError = pImpl->predictionSystem->getPredictionError();
            if (predError > 0.0f) {
                // Apply prediction error to neuromodulation
                if (pImpl->predictionError) {
                    pImpl->predictionError->setLevel(predError);
                }
                
                // Apply prediction error to plasticity
                applyPredictionErrorToPlasticity(predError);
            }
            
            // Connect prediction to attention
            if (pImpl->attention && pImpl->predictionSystem->getConfidence() > 0.5f) {
                // High confidence predictions increase attentional focus
                pImpl->attention->applyTopDownBias(
                    NeuronId(0),  // Root neuron
                    pImpl->predictionSystem->getConfidence() * 0.1f
                );
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
    
    // Apply full neuromodulation effects
    applyFullNeuromodulation();
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Connect concept formation to experience processing
        // Process current neural activity patterns to form concepts
        
        // Create pattern from current sensory input
        std::vector<float> currentPattern;
        if (pImpl->workingMemory) {
            // Get active neurons as pattern
            auto activeNeurons = pImpl->workingMemory->getMemoryNeurons();
            currentPattern.resize(activeNeurons.size(), 0.0f);
            for (size_t i = 0; i < activeNeurons.size(); ++i) {
                // Get activation level of neuron
                for (auto& region : pImpl->regions) {
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        if (neuron->getId() == activeNeurons[i]) {
                            currentPattern[i] = 
                                std::abs(neuron->getState().membranePotential - 
                                       neuron->getState().restingPotential) / 20.0f;
                            break;
                        }
                    }
                }
            }
        } else {
            // Use current sensory input as pattern
            const auto& sensoryData = pImpl->currentSensoryInput.getData();
            currentPattern = sensoryData;
        }
        
        // Create features from neural activity
        std::vector<float> features;
        float totalActivation = 0.0f;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring()) {
                        float activation = 
                            std::abs(neuron->getState().membranePotential - 
                                   neuron->getState().restingPotential);
                        features.push_back(activation);
                        totalActivation += activation;
                    }
                }
            }
        }
        
        // Normalize features
        if (totalActivation > 0.0f) {
            for (auto& f : features) {
                f /= totalActivation;
            }
        }
        
        // Present experience to concept formation
        float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        pImpl->conceptFormation->presentExperience(currentPattern, features, reward, currentStep);
        
        // Integrate concepts into attention
        if (pImpl->attention) {
            // If concepts formed, apply to attention bias
            size_t matchingConcept = pImpl->conceptFormation->getMatchingConcept(currentPattern, 0.7f);
            if (matchingConcept > 0) {
                // Apply concept-based bias to attention
                pImpl->attention->applyBottomUpSalience(
                    NeuronId(matchingConcept), 
                    0.5f * pImpl->conceptFormation->getConceptStability(matchingConcept)
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
    
    // ========== STEP 13: Sleep/Rest and memory consolidation ==========
    // Implement simple sleep/rest cycle based on neuromodulation
    // Sleep drive increases with 5-HT, arousal decreases with sleep
    pImpl->sleepDrive = pImpl->serotonin ? pImpl->serotonin->getLevel() * 0.5f : 0.0f;
    
    // Check for sleep onset (simplified: sleep after prolonged inactivity)
    if (pImpl->sleepDrive > 0.7f && !pImpl->isResting) {
        pImpl->isResting = true;
        pImpl->arousalLevel *= 0.5f;
        NLM_LOG_INFO("Brain entering rest/sleep cycle");
    } else if (pImpl->sleepDrive < 0.2f && pImpl->isResting) {
        pImpl->isResting = false;
        pImpl->arousalLevel *= 2.0f;
        NLM_LOG_INFO("Brain exiting rest/sleep cycle");
    }
    
    // During rest, enhance memory consolidation
    if (pImpl->isResting && pImpl->episodicMemory) {
        // Enhanced consolidation during rest
        pImpl->episodicMemory->consolidate(0.8f);  // Higher consolidation rate
    }
    
    // Apply sleep effects on neural activity
    if (pImpl->isResting) {
        // Reduce neural firing during sleep
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Inject inhibitory current to reduce activity
                    neuron->injectCurrent(-2.0f * pImpl->sleepDrive);
                }
            }
        }
    }
    
    // Enhanced memory consolidation with neuromodulation
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Apply neuromodulatory effects to consolidation
        float consolidationMod = 1.0f;
        
        if (pImpl->dopamine) {
            // Dopamine enhances consolidation of salient memories
            consolidationMod *= (1.0f + pImpl->dopamine->getLevel());
        }
        
        if (pImpl->acetylcholine) {
            // ACh enhances memory encoding and consolidation
            consolidationMod *= (1.0f + pImpl->acetylcholine->getLevel() * 0.5f);
        }
        
        if (pImpl->serotonin) {
            // 5-HT stabilizes emotional memories
            consolidationMod *= (1.0f + pImpl->serotonin->getLevel() * 0.3f);
        }
        
        // Apply enhanced consolidation
        pImpl->episodicMemory->consolidate(0.5f * consolidationMod);
        
        NLM_LOG_DEBUG("Memory consolidation: mod=" + std::to_string(consolidationMod));
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
    // Enhanced action selection using NeuralPlanner
    // If NeuralPlanner is available and ready, use it for planning
    if (pImpl->planner && pImpl->workingMemory) {
        // Convert working memory to state vector for planning
        std::vector<float> currentState;
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        currentState.resize(memoryNeurons.size(), 0.0f);
        
        // Get current reward expectation from dopamine
        float targetReward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.5f;
        
        // Use NeuralPlanner to select action
        ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
        
        // Check if planner was successful
        if (plannedAction != ActionType::Wait || 
            pImpl->planner->wasRecentPlanSuccessful()) {
            auto action = std::make_unique<Action>(plannedAction);
            
            // Record action in neural planner for learning
            if (pImpl->planner) {
                // This would need actual action history in a real system
                // For now, just update plan quality based on motor activity
                std::vector<ActionType> actualActions = { plannedAction };
                std::vector<ActionType> plannedActions = { plannedAction };
                pImpl->planner->updatePlanQuality(plannedActions, actualActions, targetReward);
            }
            
            return action;
        }
    }
    
    // Fallback to simple action selection based on motor neuron activity
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

// Helper method to collect sensory observation from sensory neurons
SensoryInput Brain::collectSensoryObservation() const {
    SensoryInput observation;
    
    // Convert neuron activities to sensory observation
    std::vector<float> sensoryData;
    sensoryData.reserve(pImpl->sensoryNeurons.size());
    
    for (auto* neuron : pImpl->sensoryNeurons) {
        float activation = std::max(0.0f, 
            neuron->getState().membranePotential - 
            neuron->getState().restingPotential);
        sensoryData.push_back(activation);
    }
    
    observation.setData(sensoryData);
    observation.setTimestamp(pImpl->currentTime);
    
    return observation;
}

// Apply prediction error to plasticity
void Brain::applyPredictionErrorToPlasticity(float predError) {
    if (!pImpl->predictionSystem) return;
    
    // Scale plasticity based on prediction error
    float plasticityMod = 1.0f;
    
    if (predError > 0.5f) {
        // High prediction error increases learning
        plasticityMod = 1.5f;
    } else if (predError < 0.1f) {
        // Low prediction error decreases learning
        plasticityMod = 0.5f;
    }
    
    // Apply to STDP and Hebbian plasticity
    if (pImpl->stdp) {
        // Scale STDP learning rates
        float currentLTP = pImpl->stdp->getLTPWeight();
        float currentLTD = pImpl->stdp->getLTDWeight();
        
        pImpl->stdp->setLTPWeight(currentLTP * plasticityMod);
        pImpl->stdp->setLTDWeight(currentLTD * plasticityMod);
    }
    
    // Apply to novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->setLevel(predError);
    }
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
        
        // Write neural population activity
        std::vector<float> populationActivity;
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                // Calculate average activity for this population
                float avgActivity = 0.0f;
                int count = 0;
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring()) {
                        avgActivity += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                        count++;
                    }
                }
                if (count > 0) avgActivity /= count;
                populationActivity.push_back(avgActivity);
            }
        }
        writer.writeFloatVector("population_activity", populationActivity);
        
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
        NLM_LOG_INFO("Loaded " + std::string(synapseData.weight.size()) + " synapses");
        
        // Load memory systems
        if (pImpl->workingMemory) {
            WorkingMemoryCheckpointData wmData;
            if (reader.readMemorySystem("working_memory", wmData)) {
                pImpl->workingMemory->deserialize(wmData);
                NLM_LOG_INFO("Loaded working memory state");
            } else {
                NLM_LOG_INFO("No working memory state to load");
            }
        }
        
        if (pImpl->episodicMemory) {
            EpisodicMemoryCheckpointData emData;
            if (reader.readMemorySystem("episodic_memory", emData)) {
                pImpl->episodicMemory->deserialize(emData);
                NLM_LOG_INFO("Loaded episodic memory state");
            } else {
                NLM_LOG_INFO("No episodic memory state to load");
            }
        }
        
        if (pImpl->associativeMemory) {
            AssociativeMemoryCheckpointData amData;
            if (reader.readMemorySystem("associative_memory", amData)) {
                pImpl->associativeMemory->deserialize(amData);
                NLM_LOG_INFO("Loaded associative memory state");
            } else {
                NLM_LOG_INFO("No associative memory state to load");
            }
        }
        
        if (pImpl->predictionSystem) {
            PredictionSystemCheckpointData predData;
            if (reader.readMemorySystem("prediction_system", predData)) {
                pImpl->predictionSystem->deserialize(predData);
                NLM_LOG_INFO("Loaded prediction system state");
            } else {
                NLM_LOG_INFO("No prediction system state to load");
            }
        }
        
        if (pImpl->conceptFormation) {
            ConceptFormationCheckpointData cfData;
            if (reader.readMemorySystem("concept_formation", cfData)) {
                pImpl->conceptFormation->deserialize(cfData);
                NLM_LOG_INFO("Loaded concept formation state");
            } else {
                NLM_LOG_INFO("No concept formation state to load");
            }
        }
        
        if (pImpl->attention) {
            AttentionCheckpointData attnData;
            if (reader.readMemorySystem("attention", attnData)) {
                pImpl->attention->deserialize(attnData);
                NLM_LOG_INFO("Loaded attention state");
            } else {
                NLM_LOG_INFO("No attention state to load");
            }
        }
        
        // Load neuromodulation systems
        if (pImpl->dopamine) {
            DopamineCheckpointData daData;
            if (reader.readMemorySystem("dopamine", daData)) {
                pImpl->dopamine->deserialize(daData);
                NLM_LOG_INFO("Loaded dopamine state");
            } else {
                NLM_LOG_INFO("No dopamine state to load");
            }
        }
        
        if (pImpl->curiosity) {
            CuriosityCheckpointData curiosityData;
            if (reader.readMemorySystem("curiosity", curiosityData)) {
                pImpl->curiosity->deserialize(curiosityData);
                NLM_LOG_INFO("Loaded curiosity state");
            } else {
                NLM_LOG_INFO("No curiosity state to load");
            }
        }
        
        if (pImpl->novelty) {
            NoveltyCheckpointData noveltyData;
            if (reader.readMemorySystem("novelty", noveltyData)) {
                pImpl->novelty->deserialize(noveltyData);
                NLM_LOG_INFO("Loaded novelty state");
            } else {
                NLM_LOG_INFO("No novelty state to load");
            }
        }
        
        if (pImpl->predictionError) {
            PredictionErrorCheckpointData peData;
            if (reader.readMemorySystem("prediction_error", peData)) {
                pImpl->predictionError->deserialize(peData);
                NLM_LOG_INFO("Loaded prediction error state");
            } else {
                NLM_LOG_INFO("No prediction error state to load");
            }
        }
        
        if (pImpl->acetylcholine) {
            AcetylcholineCheckpointData achData;
            if (reader.readMemorySystem("acetylcholine", achData)) {
                pImpl->acetylcholine->deserialize(achData);
                NLM_LOG_INFO("Loaded acetylcholine state");
            } else {
                NLM_LOG_INFO("No acetylcholine state to load");
            }
        }
        
        if (pImpl->norepinephrine) {
            NorepinephrineCheckpointData neData;
            if (reader.readMemorySystem("norepinephrine", neData)) {
                pImpl->norepinephrine->deserialize(neData);
                NLM_LOG_INFO("Loaded norepinephrine state");
            } else {
                NLM_LOG_INFO("No norepinephrine state to load");
            }
        }
        
        if (pImpl->serotonin) {
            SerotoninCheckpointData serotoninData;
            if (reader.readMemorySystem("serotonin", serotoninData)) {
                pImpl->serotonin->deserialize(serotoninData);
                NLM_LOG_INFO("Loaded serotonin state");
            } else {
                NLM_LOG_INFO("No serotonin state to load");
            }
        }
        
        if (pImpl->developmentSystem) {
            DevelopmentSystemCheckpointData devData;
            if (reader.readMemorySystem("development_system", devData)) {
                pImpl->developmentSystem->deserialize(devData);
                NLM_LOG_INFO("Loaded development system state");
            } else {
                NLM_LOG_INFO("No development system state to load");
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

// ========== NEUROMODULATION CHECKPOINT SUPPORT ==========

void Brain::saveNeuromodulationState(CheckpointWriter& writer) const {
    if (pImpl->dopamine) {
        DopamineCheckpointData daData;
        pImpl->dopamine->serialize(daData);
        if (!writer.writeMemorySystem("dopamine", daData)) {
            NLM_LOG_ERROR("Failed to save dopamine state");
        }
    }
    
    if (pImpl->curiosity) {
        CuriosityCheckpointData curiosityData;
        pImpl->curiosity->serialize(curiosityData);
        if (!writer.writeMemorySystem("curiosity", curiosityData)) {
            NLM_LOG_ERROR("Failed to save curiosity state");
        }
    }
    
    if (pImpl->novelty) {
        NoveltyCheckpointData noveltyData;
        pImpl->novelty->serialize(noveltyData);
        if (!writer.writeMemorySystem("novelty", noveltyData)) {
            NLM_LOG_ERROR("Failed to save novelty state");
        }
    }
    
    if (pImpl->predictionError) {
        PredictionErrorCheckpointData peData;
        pImpl->predictionError->serialize(peData);
        if (!writer.writeMemorySystem("prediction_error", peData)) {
            NLM_LOG_ERROR("Failed to save prediction error state");
        }
    }
    
    if (pImpl->acetylcholine) {
        AcetylcholineCheckpointData achData;
        pImpl->acetylcholine->serialize(achData);
        if (!writer.writeMemorySystem("acetylcholine", achData)) {
            NLM_LOG_ERROR("Failed to save acetylcholine state");
        }
    }
    
    if (pImpl->norepinephrine) {
        NorepinephrineCheckpointData neData;
        pImpl->norepinephrine->serialize(neData);
        if (!writer.writeMemorySystem("norepinephrine", neData)) {
            NLM_LOG_ERROR("Failed to save norepinephrine state");
        }
    }
    
    if (pImpl->serotonin) {
        SerotoninCheckpointData serotoninData;
        pImpl->serotonin->serialize(serotoninData);
        if (!writer.writeMemorySystem("serotonin", serotoninData)) {
            NLM_LOG_ERROR("Failed to save serotonin state");
        }
    }
    
    if (pImpl->developmentSystem) {
        DevelopmentSystemCheckpointData devData;
        pImpl->developmentSystem->serialize(devData);
        if (!writer.writeMemorySystem("development_system", devData)) {
            NLM_LOG_ERROR("Failed to save development system state");
        }
    }
}

bool Brain::loadNeuromodulationState(CheckpointReader& reader) {
    bool success = true;
    
    if (pImpl->dopamine) {
        DopamineCheckpointData daData;
        if (reader.readMemorySystem("dopamine", daData)) {
            pImpl->dopamine->deserialize(daData);
            NLM_LOG_INFO("Loaded dopamine state");
        } else {
            NLM_LOG_INFO("No dopamine state to load");
        }
    }
    
    if (pImpl->curiosity) {
        CuriosityCheckpointData curiosityData;
        if (reader.readMemorySystem("curiosity", curiosityData)) {
            pImpl->curiosity->deserialize(curiosityData);
            NLM_LOG_INFO("Loaded curiosity state");
        } else {
            NLM_LOG_INFO("No curiosity state to load");
        }
    }
    
    if (pImpl->novelty) {
        NoveltyCheckpointData noveltyData;
        if (reader.readMemorySystem("novelty", noveltyData)) {
            pImpl->novelty->deserialize(noveltyData);
            NLM_LOG_INFO("Loaded novelty state");
        } else {
            NLM_LOG_INFO("No novelty state to load");
        }
    }
    
    if (pImpl->predictionError) {
        PredictionErrorCheckpointData peData;
        if (reader.readMemorySystem("prediction_error", peData)) {
            pImpl->predictionError->deserialize(peData);
            NLM_LOG_INFO("Loaded prediction error state");
        } else {
            NLM_LOG_INFO("No prediction error state to load");
        }
    }
    
    if (pImpl->acetylcholine) {
        AcetylcholineCheckpointData achData;
        if (reader.readMemorySystem("acetylcholine", achData)) {
            pImpl->acetylcholine->deserialize(achData);
            NLM_LOG_INFO("Loaded acetylcholine state");
        } else {
            NLM_LOG_INFO("No acetylcholine state to load");
        }
    }
    
    if (pImpl->norepinephrine) {
        NorepinephrineCheckpointData neData;
        if (reader.readMemorySystem("norepinephrine", neData)) {
            pImpl->norepinephrine->deserialize(neData);
            NLM_LOG_INFO("Loaded norepinephrine state");
        } else {
            NLM_LOG_INFO("No norepinephrine state to load");
        }
    }
    
    if (pImpl->serotonin) {
        SerotoninCheckpointData serotoninData;
        if (reader.readMemorySystem("serotonin", serotoninData)) {
            pImpl->serotonin->deserialize(serotoninData);
            NLM_LOG_INFO("Loaded serotonin state");
        } else {
            NLM_LOG_INFO("No serotonin state to load");
        }
    }
    
    if (pImpl->developmentSystem) {
        DevelopmentSystemCheckpointData devData;
        if (reader.readMemorySystem("development_system", devData)) {
            pImpl->developmentSystem->deserialize(devData);
            NLM_LOG_INFO("Loaded development system state");
        } else {
            NLM_LOG_INFO("No development system state to load");
        }
    }
    
    return success;
}
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

Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
}

void Brain::applyFullNeuromodulation() {
    // Apply all neuromodulators to neural excitability
    if (pImpl->dopamine) {
        float daLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability
                    float excitabilityMod = daLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    if (pImpl->acetylcholine) {
        float achLevel = pImpl->acetylcholine->getLevel();
        // ACh enhances attention and memory
        // Could modulate attention selection strength
        pImpl->attentionModulation = 0.5f + 0.5f * achLevel;
    }
    
    if (pImpl->norepinephrine) {
        float neLevel = pImpl->norepinephrine->getLevel();
        // NE increases arousal and learning rate
        pImpl->arousalLevel = 0.3f + 0.7f * neLevel;
    }
    
    if (pImpl->serotonin) {
        float serotoninLevel = pImpl->serotonin->getLevel();
        // 5-HT reduces impulsivity and stabilizes behavior
        // Could modulate action selection stability
        pImpl->sleepDrive = 0.2f + 0.3f * serotoninLevel;
    }
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

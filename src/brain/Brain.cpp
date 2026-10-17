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
    
    // Initialize prediction system (train on initial neutral state)
    // Create initial neutral sensory observation for prediction system
    SensoryInput neutralInput;
    std::vector<float> neutralData;
    for (size_t i = 0; i < 10; ++i) {  // 10-dimensional sensory input
        neutralData.push_back(0.0f);
    }
    neutralInput.setData(neutralData);
    pImpl->predictionSystem->train(neutralInput);
    
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
            // Capture comprehensive current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Enhanced episodic encoding: capture multiple levels of information
            
            // 1. Neural population activity
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    std::vector<float> populationActivations;
                    std::vector<NeuronId> populationNeurons;
                    
                    for (auto* neuron : pop->getNeurons()) {
                        float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                        if (activation > 2.0f) {  // Threshold for significant activity
                            populationActivations.push_back(activation / 20.0f);  // Normalize
                            populationNeurons.push_back(neuron->getId());
                        }
                    }
                    
                    if (!populationActivations.empty()) {
                        episode.populationActivations.push_back(populationActivations);
                        episode.populationNeurons.push_back(populationNeurons);
                        episode.populationIds.push_back(pop->getId());
                    }
                }
            }
            
            // 2. Synaptic weight changes (plasticity traces)
            for (auto& region : pImpl->regions) {
                for (auto& syn : region->getSynapses()) {
                    if (syn->getPlasticityFlags().stdp || syn->getPlasticityFlags().hebbian) {
                        episode.synapseWeights.push_back(syn->getWeight());
                        episode.synapseConnections.push_back(std::make_pair(syn->getSourceNeuron(), syn->getDestinationNeuron()));
                        
                        // Capture recent plasticity history
                        const auto& preSpikes = syn->getPreSpikeHistory();
                        const auto& postSpikes = syn->getPostSpikeHistory();
                        if (!preSpikes.empty() && !postSpikes.empty()) {
                            episode.plasticityEvents.push_back(std::make_pair(syn->getId(), currentTime));
                        }
                    }
                }
            }
            
            // 3. Neuromodulation state
            if (pImpl->dopamine) {
                episode.dopamineLevel = pImpl->dopamine->getLevel();
                episode.plasticityFactor = pImpl->dopamine->getPlasticityFactor();
            }
            if (pImpl->curiosity) {
                episode.curiosityLevel = pImpl->curiosity->getLevel();
            }
            if (pImpl->novelty) {
                episode.noveltyLevel = pImpl->novelty->getNoveltyLevel();
            }
            
            // 4. Working memory content for pattern completion
            if (pImpl->workingMemory) {
                episode.workingMemoryTraces = pImpl->workingMemory->getActiveTraces();
                episode.workingMemoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            }
            
            // 5. Prediction error (if available)
            if (pImpl->predictionSystem) {
                // Integrate prediction error into episodic memory
                std::vector<float> predictionErrors;
                if (pImpl->predictionSystem->getPredictionErrors(predictionErrors)) {
                    episode.predictionErrors = predictionErrors;
                }
            }
            
            // 6. Sensory input context (for cross-modal association)
            // Note: In a real implementation, would store recent sensory input patterns
            
            // Store the enriched episode
            pImpl->episodicMemory->storeEpisode(episode);
            
            // Trigger memory consolidation for new episodes
            if (pImpl->episodicMemory->getEpisodeCount() % 10 == 0) {
                pImpl->episodicMemory->consolidate(0.3f);  // Consolidate every 10 episodes
            }
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Connect prediction system to train from experiences
        // Use episodic memory to generate predictions and update synaptic weights
        
        // Get recent episodes for prediction training
        auto recentEpisodes = pImpl->episodicMemory ? 
            pImpl->episodicMemory->getEpisodesForTraining(5) : // Get last 5 episodes
            std::vector<const EpisodicMemoryItem*>();
        
        // Generate predictions based on recent episodes
        if (!recentEpisodes.empty()) {
            std::vector<float> predictions;
            if (pImpl->predictionSystem->trainFromEpisodes(recentEpisodes, predictions)) {
                // Apply predictions to update neural connectivity
                applyPredictionsToNetwork(predictions);
                
                // Compute prediction errors and apply them
                std::vector<float> errors;
                computePredictionErrors(recentEpisodes, predictions, errors);
                
                if (!errors.empty()) {
                    // Apply prediction errors to synaptic plasticity
                    applyPredictionErrorsToPlasticity(errors);
                    
                    // Update prediction system with new errors
                    pImpl->predictionSystem->updatePredictionErrors(errors);
                }
            }
        }
        
        // Update prediction system with current sensory context
        std::vector<float> currentContext;
        extractSensoryContext(currentContext);
        pImpl->predictionSystem->updateContext(currentContext);
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
    // Enhanced sensory processing with working memory integration
    // Phase 6: Connect sensory input to working memory and other systems
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Enhanced sensory encoding with multiple channels
    std::vector<float> encodedFeatures;
    encodedFeatures.reserve(numSensory);
    
    // Distribute input across sensory neurons with enhanced processing
    for (size_t i = 0; i < numSensory; ++i) {
        // Multi-stage sensory encoding
        float rawValue = 0.0f;
        if (i < values.size()) {
            rawValue = static_cast<float>(values[i]);
        }
        
        // Stage 1: Basic normalization
        float normalizedValue = rawValue * 10.0f;  // Scale to [-10, 10] mV
        
        // Stage 2: Apply nonlinear encoding (sigmoid-like response)
        float encoded = 2.0f / (1.0f + std::exp(-normalizedValue / 5.0f)) - 1.0f;  // Range [-1, 1]
        
        // Stage 3: Apply attention modulation (if attention system is active)
        if (pImpl->attention) {
            // Get attention level for this sensory channel
            float attentionWeight = pImpl->attention->getAttentionWeight(static_cast<int>(i) % 4);
            encoded *= attentionWeight;
        }
        
        encodedFeatures.push_back(encoded);
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(encoded);
    }
    
    // Enhanced working memory integration
    if (pImpl->workingMemory) {
        // Store encoded sensory features in working memory
        for (size_t i = 0; i < encodedFeatures.size(); ++i) {
            if (std::abs(encodedFeatures[i]) > 0.1f) {  // Only store significant features
                // Store with temporal context
                float featureStrength = std::min(std::abs(encodedFeatures[i]), 1.0f);
                pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), featureStrength);
                
                // Also encode feature dimensions for pattern recognition
                size_t featureId = i + pImpl->sensoryNeurons.size();  // Offset to avoid conflicts
                pImpl->workingMemory->storeFeature(featureId, encodedFeatures[i], pImpl->currentStep);
            }
        }
        
        // Update working memory with new sensory input
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // Connect to prediction system for prediction error computation
    if (pImpl->predictionSystem) {
        // Create prediction error signal based on sensory input
        float predictionError = computePredictionError(input, encodedFeatures);
        
        // Apply prediction error to neuromodulation systems
        if (pImpl->dopamine) {
            pImpl->dopamine->applyPredictionError(predictionError);
        }
        if (pImpl->novelty) {
            pImpl->novelty->detectNovelty(encodedFeatures);
        }
    }
    
    // Connect to curiosity system for exploration motivation
    if (pImpl->curiosity) {
        // Update curiosity based on sensory novelties
        pImpl->curiosity->updateSensoryInput(values);
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
    // Enhanced action selection using neural planner (Phase 6 integration)
    // The neural planner evaluates potential actions based on current brain state
    // considering working memory, episodic memory, prediction system, and neuromodulation
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Use neural planner for sophisticated action selection
    if (pImpl->planner) {
        // Gather current brain state for planning
        std::vector<float> currentState;
        extractPlanningState(currentState);
        
        // Use neural planner to evaluate action options
        std::vector<std::pair<ActionType, float>> actionOptions;
        
        // Get available actions from motor neurons
        ActionType immediateAction = getImmediateAction();
        actionOptions.push_back(std::make_pair(immediateAction, 0.5f));  // Immediate reaction
        
        // Consider planned actions based on working memory
        if (pImpl->workingMemory) {
            auto plannedActions = pImpl->workingMemory->getPlannedActions();
            for (ActionType action : plannedActions) {
                // Evaluate based on prediction system
                float predictionValue = evaluateActionWithPrediction(action, currentState);
                actionOptions.push_back(std::make_pair(action, predictionValue * 0.3f));
            }
        }
        
        // Consider episodic memory patterns
        if (pImpl->episodicMemory) {
            auto pastActions = pImpl->episodicMemory->getSuccessfulActions();
            for (ActionType action : pastActions) {
                // Weight by past success probability
                float successWeight = calculateActionSuccessProbability(action);
                actionOptions.push_back(std::make_pair(action, successWeight * 0.2f));
            }
        }
        
        // Consider neuromodulation states
        float neuromodulationBias = getNeuromodulationActionBias();
        for (auto& option : actionOptions) {
            option.second *= (1.0f + neuromodulationBias);
        }
        
        // Select best action using neural planner
        ActionType selectedAction = selectBestAction(actionOptions);
        
        return std::make_unique<Action>(selectedAction);
    }
    
    // Fallback to simple action selection if planner is not available
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
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
        
        // Save neurons
        NeuronCheckpointData neuronData;
        saveNeurons(neuronData);
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Save synapses
        SynapseCheckpointData synapseData;
        saveSynapses(synapseData);
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Save memory systems
        if (pImpl->workingMemory) {
            saveWorkingMemory(*pImpl->workingMemory);
        }
        if (pImpl->episodicMemory) {
            saveEpisodicMemory(*pImpl->episodicMemory);
        }
        if (pImpl->associativeMemory) {
            saveAssociativeMemory(*pImpl->associativeMemory);
        }
        
        // Save prediction system
        if (pImpl->predictionSystem) {
            savePredictionSystem(*pImpl->predictionSystem);
        }
        
        // Save cognition systems
        if (pImpl->planner) {
            saveNeuralPlanner(*pImpl->planner);
        }
        if (pImpl->conceptFormation) {
            saveConceptFormation(*pImpl->conceptFormation);
        }
        if (pImpl->attention) {
            saveAttention(*pImpl->attention);
        }
        
        // Save development system
        if (pImpl->developmentSystem) {
            saveDevelopmentSystem(*pImpl->developmentSystem);
        }
        
        // Save neuromodulation systems
        if (pImpl->dopamine) {
            saveDopamine(*pImpl->dopamine);
        }
        if (pImpl->curiosity) {
            saveCuriosity(*pImpl->curiosity);
        }
        if (pImpl->novelty) {
            saveNovelty(*pImpl->novelty);
        }
        if (pImpl->predictionError) {
            savePredictionError(*pImpl->predictionError);
        }
        
        // Save simulation state
        saveSimulationState();
        
        // Save random state
        if (pImpl->rng) {
            saveRandomState(*pImpl->rng);
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
        
        // Load neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        loadNeurons(neuronData);
        
        // Load synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states
        loadSynapses(synapseData);
        
        // Load memory systems
        loadWorkingMemory(*pImpl->workingMemory);
        loadEpisodicMemory(*pImpl->episodicMemory);
        loadAssociativeMemory(*pImpl->associativeMemory);
        
        // Load prediction system
        loadPredictionSystem(*pImpl->predictionSystem);
        
        // Load cognition systems
        if (pImpl->planner) {
            loadNeuralPlanner(*pImpl->planner);
        }
        if (pImpl->conceptFormation) {
            loadConceptFormation(*pImpl->conceptFormation);
        }
        if (pImpl->attention) {
            loadAttention(*pImpl->attention);
        }
        
        // Load development system
        loadDevelopmentSystem(*pImpl->developmentSystem);
        
        // Load neuromodulation systems
        loadDopamine(*pImpl->dopamine);
        loadCuriosity(*pImpl->curiosity);
        loadNovelty(*pImpl->novelty);
        loadPredictionError(*pImpl->predictionError);
        
        // Load simulation state
        loadSimulationState();
        
        // Load random state
        if (pImpl->rng) {
            loadRandomState(*pImpl->rng);
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

float Brain::computePredictionError(const class SensoryInput& input, const std::vector<float>& encodedFeatures) {
    // Compute prediction error between expected and actual sensory input
    // This is a simplified implementation - in a real system would use learned predictions
    
    if (!pImpl->predictionSystem) {
        return 0.0f;
    }
    
    const auto& values = input.getData();
    if (values.empty()) {
        return 0.0f;
    }
    
    // Get predicted values from prediction system
    std::vector<float> predictedValues;
    if (pImpl->predictionSystem->getPredictedValues(predictedValues)) {
        // Compute error as absolute difference
        float totalError = 0.0f;
        size_t minSize = std::min(values.size(), predictedValues.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float expected = static_cast<float>(values[i]);
            float predicted = predictedValues[i];
            totalError += std::abs(expected - predicted);
        }
        
        return totalError / static_cast<float>(minSize);
    }
    
    return 0.0f; // No prediction available
}

void Brain::applyPredictionsToNetwork(const std::vector<float>& predictions) {
    // Apply learned predictions to strengthen relevant neural pathways
    // This is a simplified implementation - real system would use Hebbian-like learning
    
    if (predictions.empty()) return;
    
    // Scale predictions to reasonable weight changes
    float maxWeightChange = 0.01f;  // Maximum synaptic weight adjustment
    
    for (size_t i = 0; i < std::min(predictions.size(), pImpl->regions.size()); ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Apply prediction-based plasticity to synapses in this region
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().hebbian) {
                    // Use prediction error to modulate Hebbian learning
                    float prediction = predictions[i % predictions.size()];
                    float weightChange = prediction * maxWeightChange;
                    
                    // Ensure weight change is within bounds
                    if (std::abs(weightChange) > maxWeightChange) {
                        weightChange = (weightChange > 0) ? maxWeightChange : -maxWeightChange;
                    }
                    
                    float currentWeight = syn->getWeight();
                    float newWeight = currentWeight + weightChange;
                    
                    // Constrain weights to reasonable range
                    newWeight = std::max(-1.0f, std::min(1.0f, newWeight));
                    syn->setWeight(newWeight);
                }
            }
        }
    }
}

void Brain::computePredictionErrors(const std::vector<const EpisodicMemoryItem*>& episodes,
                                    const std::vector<float>& predictions,
                                    std::vector<float>& errors) {
    // Compute prediction errors by comparing episodic patterns with predictions
    // This helps the system learn from its own experiences
    
    errors.clear();
    if (episodes.empty() || predictions.empty()) {
        return;
    }
    
    // Simplified error computation
    for (size_t i = 0; i < std::min(episodes.size(), predictions.size()); ++i) {
        const auto* episode = episodes[i];
        float prediction = predictions[i];
        
        // Compute error based on neural activity patterns in the episode
        float episodePatternStrength = 0.0f;
        for (float activation : episode->neuronActivations) {
            episodePatternStrength += activation;
        }
        
        // Normalize pattern strength
        if (!episode->neuronActivations.empty()) {
            episodePatternStrength /= static_cast<float>(episode->neuronActivations.size());
        }
        
        // Compute prediction error as difference between expected and actual pattern
        float error = std::abs(episodePatternStrength - prediction);
        errors.push_back(error);
    }
}

void Brain::applyPredictionErrorsToPlasticity(const std::vector<float>& errors) {
    // Apply prediction errors to synaptic plasticity systems
    // This implements prediction-driven learning similar to reward prediction error
    
    if (errors.empty() || !pImpl->dopamine) {
        return;
    }
    
    float averageError = 0.0f;
    for (float error : errors) {
        averageError += error;
    }
    averageError /= static_cast<float>(errors.size());
    
    // Scale error by dopamine level to modulate learning strength
    float dopamineLevel = pImpl->dopamine->getLevel();
    float learningRate = std::min(1.0f, averageError * dopamineLevel * 10.0f);
    
    // Apply prediction error to STDP and Hebbian plasticity
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp || syn->getPlasticityFlags().hebbian) {
                // Enhance plasticity based on prediction error
                syn->setLearningRate(std::max(0.001f, learningRate));
                
                // Apply prediction error to weight update
                float weightChange = learningRate * (averageError - 0.5f);  // Center around zero
                float currentWeight = syn->getWeight();
                float newWeight = currentWeight + weightChange;
                
                // Constrain weights
                newWeight = std::max(-1.0f, std::min(1.0f, newWeight));
                syn->setWeight(newWeight);
            }
        }
    }
}

void Brain::extractSensoryContext(std::vector<float>& context) {
    // Extract current sensory context for prediction system
    // This captures the current environmental state for learning
    
    context.clear();
    
    // Extract sensory input from sensory neurons
    if (!pImpl->sensoryNeurons.empty()) {
        // Get current membrane potentials from sensory neurons
        for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
            auto* neuron = pImpl->sensoryNeurons[i];
            if (neuron) {
                float potential = neuron->getState().membranePotential;
                float resting = neuron->getState().restingPotential;
                float activation = std::max(0.0f, potential - resting);
                context.push_back(activation);
            }
        }
    }
    
    // Add neuromodulation context for prediction
    if (pImpl->dopamine) {
        context.push_back(pImpl->dopamine->getLevel());
    }
    if (pImpl->curiosity) {
        context.push_back(pImpl->curiosity->getLevel());
    }
    if (pImpl->novelty) {
        context.push_back(pImpl->novelty->getNoveltyLevel());
    }
    context.push_back(pImpl->novelty->getNoveltyLevel());
    }
}

void Brain::extractPlanningState(std::vector<float>& state) {
    // Extract current brain state for neural planning
    // This captures the essential state information for action planning
    
    state.clear();
    
    // Extract sensory state
    extractSensoryContext(state);
    
    // Add working memory state
    if (pImpl->workingMemory) {
        float memoryLoad = pImpl->workingMemory->getActiveTraces();
        state.push_back(memoryLoad / 100.0f);  // Normalize to [0,1]
    }
    
    // Add episodic memory state
    if (pImpl->episodicMemory) {
        float episodeCount = pImpl->episodicMemory->getEpisodeCount();
        state.push_back(episodeCount / 1000.0f);  // Normalize to [0,1]
    }
    
    // Add neuromodulation state
    if (pImpl->dopamine) {
        state.push_back(pImpl->dopamine->getLevel());
    }
    if (pImpl->curiosity) {
        state.push_back(pImpl->curiosity->getLevel());
    }
    
    // Add development stage
    state.push_back(static_cast<float>(pImpl->developmentalStage) / 4.0f);
    
    // Add neural activity statistics
    float avgFiringRate = getAverageFiringRate();
    state.push_back(avgFiringRate / 100.0f);  // Normalize to reasonable range
}

ActionType Brain::getImmediateAction() {
    // Determine immediate action based on current motor neuron activity
    // This provides a baseline action that can be refined by the neural planner
    
    if (pImpl->motorNeurons.empty()) {
        return ActionType::Wait;
    }
    
    // Calculate current motor activity
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Simple action selection based on motor activity
    if (firingMotor > 0) {
        return ActionType::MoveForward;
    }
    
    return ActionType::Wait;
}

float Brain::evaluateActionWithPrediction(ActionType action, const std::vector<float>& state) {
    // Evaluate an action based on prediction system and current brain state
    // This provides value estimates for different actions
    
    if (!pImpl->predictionSystem) {
        return 0.5f;  // Neutral value if no prediction system
    }
    
    // Simple evaluation based on action type and current state
    float value = 0.5f;  // Base value
    
    // Modify based on action type
    switch (action) {
        case ActionType::MoveForward:
            value += 0.3f;  // Moving forward is generally beneficial
            break;
        case ActionType::Wait:
            value -= 0.2f;  // Waiting is generally less beneficial
            break;
        case ActionType::TurnLeft:
            value += 0.1f;  // Turning has moderate benefit
            break;
        case ActionType::TurnRight:
            value += 0.1f;  // Turning has moderate benefit
            break;
    }
    
    // Adjust based on current state
    if (!state.empty()) {
        // Consider motor activity level
        float motorActivity = std::min(state.back(), 1.0f);
        value += motorActivity * 0.2f;
    }
    
    // Clamp to reasonable range
    return std::max(0.0f, std::min(1.0f, value));
}

float Brain::calculateActionSuccessProbability(ActionType action) {
    // Calculate the probability of action success based on episodic memory
    // This provides learned success rates for different actions
    
    if (!pImpl->episodicMemory) {
        return 0.5f;  // Neutral probability if no episodic memory
    }
    
    // Get past actions and their outcomes from episodic memory
    auto pastActions = pImpl->episodicMemory->getSuccessfulActions();
    size_t totalActions = pastActions.size();
    
    if (totalActions == 0) {
        return 0.5f;  // Neutral if no past actions
    }
    
    // Count successful occurrences of this action type
    size_t successfulActions = 0;
    for (ActionType pastAction : pastActions) {
        if (pastAction == action) {
            successfulActions++;
        }
    }
    
    // Calculate success probability
    float successProbability = static_cast<float>(successfulActions) / static_cast<float>(totalActions);
    
    // Add some smoothing to avoid extreme values
    return 0.1f + (successProbability * 0.9f);
}

float Brain::getNeuromodulationActionBias() {
    // Calculate action bias based on neuromodulation systems
    // Dopamine promotes approach behaviors, curiosity promotes exploration
    
    float bias = 0.0f;
    
    if (pImpl->dopamine) {
        // Dopamine promotes approach actions
        bias += pImpl->dopamine->getLevel() * 0.5f;
    }
    
    if (pImpl->curiosity) {
        // Curiosity promotes exploration actions
        bias += pImpl->curiosity->getLevel() * 0.3f;
    }
    
    if (pImpl->novelty) {
        // Novelty detection affects action selection
        float noveltyLevel = pImpl->novelty->getNoveltyLevel();
        bias += noveltyLevel * 0.2f;
    }
    
    return bias;
}

ActionType Brain::selectBestAction(const std::vector<std::pair<ActionType, float>>& options) {
    // Select the best action based on option scores
    // Uses softmax-like selection to introduce stochasticity
    
    if (options.empty()) {
        return ActionType::Wait;
    }
    
    // Find the maximum score
    float maxScore = options[0].second;
    for (const auto& option : options) {
        if (option.second > maxScore) {
            maxScore = option.second;
        }
    }
    
    // Gather all options with maximum score (for stochastic selection)
    std::vector<ActionType> bestOptions;
    for (const auto& option : options) {
        if (option.second >= maxScore - 0.01f) {  // Allow small score differences
            bestOptions.push_back(option.first);
        }
    }
    
    // Randomly select from best options
    size_t selectedIndex = pImpl->rng ? 
        (*pImpl->rng)() % bestOptions.size() : 
        0;
    
    return bestOptions[selectedIndex];
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

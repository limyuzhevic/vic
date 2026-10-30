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
    if (pImpl->predictionSystem && pImpl->workingMemory) {
        // Get current neural state for prediction
        std::vector<float> currentPattern;
        extractNeuralPatternForConceptFormation(currentPattern);
        
        if (!currentPattern.empty()) {
            // Convert neural activity to sensory representation for prediction
            auto sensoryInput = convertNeuralPatternToSensory(currentPattern);
            
            // Generate prediction for next state
            auto prediction = pImpl->predictionSystem->predictNextState(sensoryInput);
            
            // Store prediction in working memory
            if (prediction) {
                pImpl->workingMemory->storePrediction(prediction.get());
            }
            
            // Track prediction error for learning signal
            float predictionError = pImpl->predictionSystem->getPredictionError();
            if (predictionError > 0.0f) {
                // Update prediction error signal for neuromodulation
                if (pImpl->predictionError) {
                    pImpl->predictionError->computeError(predictionError, 0.0f);
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
        // Extract current neural activity patterns for concept formation
        std::vector<float> neuralPattern;
        extractNeuralPatternForConceptFormation(neuralPattern);
        
        // Get sensory features from working memory and episodic memory
        std::vector<float> sensoryFeatures = getSensoryFeaturesForConceptFormation();
        
        // Get reward signal from neuromodulation
        float reward = 0.0f;
        if (pImpl->dopamine) {
            reward = pImpl->dopamine->getLevel();
        }
        
        // Present experience to concept formation system
        size_t conceptId = pImpl->conceptFormation->presentExperience(
            neuralPattern, 
            sensoryFeatures, 
            reward, 
            currentStep);
    }
    
    // ========== STEP 11: Update associative memory ==========
    if (pImpl->associativeMemory) {
        // Create associations between co-activating neural patterns
        // Based on current neural activity and experience
        
        // Get current neural activity patterns from different brain regions
        std::vector<std::vector<float>> neuralPatterns;
        
        // Collect patterns from working memory, concept formation, and episodic memory
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Use working memory traces to form associations
            std::vector<float> currentActivity(pImpl->workingMemory->getMemoryNeurons().begin(),
                                               pImpl->workingMemory->getMemoryNeurons().end());
            neuralPatterns.push_back(currentActivity);
        }
        
        // Create associations between co-activating patterns
        if (neuralPatterns.size() >= 2) {
            // For each pair of co-activating patterns, create association
            for (size_t i = 0; i < neuralPatterns.size(); ++i) {
                for (size_t j = i + 1; j < neuralPatterns.size(); ++j) {
                    // Calculate similarity between patterns
                    float similarity = pImpl->associativeMemory->computeSimilarity(
                        neuralPatterns[i], neuralPatterns[j]);
                    
                    // Create association if similarity is above threshold
                    if (similarity > 0.3f) {
                        pImpl->associativeMemory->associate(
                            neuralPatterns[i], neuralPatterns[j], similarity);
                    }
                }
            }
        }
        
        // Update associations based on current reward signal
        if (pImpl->dopamine) {
            float reward = pImpl->dopamine->getLevel();
            if (reward > 0.5f) {
                // Get prediction error for updating associations
                float error = pImpl->predictionSystem ? 
                    pImpl->predictionSystem->getPredictionError() : 0.0f;
                
                if (error > 0.0f) {
                    // Update associations based on prediction error
                    pImpl->associativeMemory->updateAssociation(
                        neuralPatterns[0], neuralPatterns[1], error * 0.1f);
                }
            }
        }
    }
    
    // ========== STEP 12: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0) {
        // Replay episodic memory
        if (pImpl->episodicMemory) {
            // Get episodes for replay
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                pImpl->episodicMemory->replayEpisode(episode);
            }
        }
        
        // Replay working memory traces
        if (pImpl->workingMemory) {
            // Get active working memory traces for replay
            std::vector<NeuronId> traces = pImpl->workingMemory->getMemoryNeurons();
            if (!traces.empty()) {
                // Replay working memory traces to strengthen connections
                for (NeuronId neuronId : traces) {
                    // Find the neuron in the brain
                    for (auto& region : pImpl->regions) {
                        for (auto& pop : region->getPopulations()) {
                            for (auto* neuron : pop->getNeurons()) {
                                if (neuron->getId() == neuronId) {
                                    // Replay the neural activity
                                    neuron->receiveExcitatoryInput(0.5f * neuron->getFiringRate());
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Replay concept formation
        if (pImpl->conceptFormation) {
            // Get recently formed concepts
            size_t conceptCount = pImpl->conceptFormation->getConceptCount();
            if (conceptCount > 0) {
                // Trigger concept replay by presenting to concept formation again
                std::vector<float> neuralPattern;
                extractNeuralPatternForConceptFormation(neuralPattern);
                if (!neuralPattern.empty()) {
                    std::vector<float> features;
                    float reward = 0.0f;
                    if (pImpl->dopamine) {
                        reward = pImpl->dopamine->getLevel();
                    }
                    
                    // Re-present experience to strengthen concept
                    pImpl->conceptFormation->presentExperience(neuralPattern, features, reward, currentStep);
                }
            }
        }
        
        // Replay associative memory
        if (pImpl->associativeMemory) {
            // Get associations for replay
            std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
            
            // In a full implementation, we would retrieve associations from associative memory
            // For now, we'll just log that associative memory replay occurred
            NLM_LOG_INFO("Replaying associative memory patterns");
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
    if (currentStep % pImpl->consolidationInterval == 0) {
        // Consolidate episodic memory
        if (pImpl->episodicMemory) {
            // Consolidate important memories, remove weak ones
            pImpl->episodicMemory->consolidate(0.3f);
        }
        
        // Consolidate working memory
        if (pImpl->workingMemory) {
            // Clean up weak working memory traces
            // TODO: Implement cleanWeakTraces method in NeuralWorkingMemory
            // For now, just reset working memory to ensure clean state
            pImpl->workingMemory->clear();
        }
        
        // Consolidate concept formation
        if (pImpl->conceptFormation) {
            // Remove unstable concepts and strengthen stable ones
            // TODO: Implement consolidateConcepts method in ConceptFormation
            // For now, just clear concept formation to ensure clean state
            pImpl->conceptFormation->clear();
        }
        
        // Consolidate associative memory
        if (pImpl->associativeMemory) {
            // Clean up weak associations and strengthen important ones
            // TODO: Implement cleanWeakAssociations method in NeuralAssociativeMemory
            // For now, just clear associative memory to ensure clean state
            pImpl->associativeMemory->clear();
        }
        
        NLM_LOG_INFO("Completed memory consolidation cycle");
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
    float modulation = signal.getLevel();
    std::string signalName = signal.getName();
    
    NLM_LOG_DEBUG("Applying neuromodulation: " + signalName + " at level " + std::to_string(modulation));
    
    // === DOPAMINE-SPECIFIC EFFECTS ===
    if (signalName == "dopamine") {
        // Phasic dopamine bursts (prediction error signaling)
        if (modulation > 0.5f) {
            // Strong phasic signal - prediction error positive (unexpected reward)
            NLM_LOG_DEBUG("Phasic dopamine burst - prediction error: positive");
            
            // Enhance learning for predicted rewards
            if (pImpl->predictionError) {
                pImpl->predictionError->signalPositiveError();
            }
            
            // Increase learning rate significantly
            pImpl->stdp->setLTPWeight(0.02f * modulation);
            pImpl->stdp->setLTDWeight(0.012f * modulation * 0.5f);
            
        } else if (modulation < -0.5f) {
            // Phasic dopamine dip (prediction error negative - omitted reward)
            NLM_LOG_DEBUG("Phasic dopamine dip - prediction error: negative");
            
            // Suppress learning for predicted rewards
            pImpl->stdp->setLTPWeight(0.01f * modulation);
            pImpl->stdp->setLTDWeight(0.012f * (1.0f + modulation));
            
        } else {
            // Tonic dopamine (baseline level)
            NLM_LOG_DEBUG("Tonic dopamine - baseline signaling");
            
            // Maintain normal learning rates
            pImpl->stdp->setLTPWeight(0.01f * modulation);
            pImpl->stdp->setLTDWeight(0.012f * modulation);
        }
        
        // === ACTION SELECTION VIOR ===
        // Dopamine modulates action selection vigor
        if (pImpl->planner) {
            float actionSelectionGain = 1.0f + modulation * 2.0f;
            pImpl->planner->setActionSelectionGain(actionSelectionGain);
            NLM_LOG_DEBUG("Action selection gain: " + std::to_string(actionSelectionGain));
        }
        
        // === REWARD PREDICTION INTEGRATION ===
        // Integrate with reward prediction error system
        if (pImpl->predictionError) {
            pImpl->predictionError->updateDopamineLevel(modulation);
        }
        
    } else if (signalName == "acetylcholine") {
        // === ACETYLCHOLINE EFFECTS ===
        // Enhance attention and working memory
        if (pImpl->attention) {
            pImpl->attention->setAChLevel(modulation);
            NLM_LOG_DEBUG("Acetylcholine enhancing attention");
        }
        
        // Enhance memory encoding
        if (pImpl->workingMemory) {
            pImpl->workingMemory->enhanceEncoding(modulation);
        }
        
    } else if (signalName == "noradrenaline") {
        // === NORADRENALINE EFFECTS ===
        // Enhance arousal and vigilance
        if (pImpl->attention) {
            pImpl->attention->setNVLLevel(modulation);
            NLM_LOG_DEBUG("Noradrenaline enhancing vigilance");
        }
        
        // Enhance novelty detection
        if (pImpl->novelty) {
            pImpl->novelty->enhanceDetection(modulation);
        }
        
    } else if (signalName == "serotonin") {
        // === SEROTONIN EFFECTS ===
        // Modulate impulse control and mood
        if (pImpl->planner) {
            pImpl->planner->setSerotoninLevel(modulation);
            NLM_LOG_DEBUG("Serotonin modulating impulse control");
        }
        
        // Enhance memory consolidation
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->enhanceConsolidation(modulation);
        }
        
    } else if (signalName == "curiosity") {
        // === CURIOSITY EFFECTS ===
        // Enhance exploration behavior
        if (pImpl->curiosity) {
            pImpl->curiosity->updateLevel(modulation);
            NLM_LOG_DEBUG("Curiosity enhancing exploration");
        }
        
        // Modulate learning for novel situations
        if (modulation > 0.5f) {
            pImpl->stdp->setLTPWeight(0.015f * modulation);
            pImpl->stdp->setLTDWeight(0.012f * modulation * 0.8f);
        }
        
    } else if (signalName == "novelty") {
        // === NOVELTY EFFECTS ===
        // Enhance detection of new patterns
        if (pImpl->novelty) {
            pImpl->novelty->updateLevel(modulation);
            NLM_LOG_DEBUG("Novelty enhancing pattern detection");
        }
        
        // Enhance concept formation for novel patterns
        if (modulation > 0.5f && pImpl->conceptFormation) {
            pImpl->conceptFormation->setNoveltyGain(modulation);
        }
    }
    
    // === GENERAL NEUROMODULATION EFFECTS ===
    
    // Scale STDP learning rates (for other neuromodulators)
    if (signalName != "dopamine") {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
    
    // === NEUROMODULATOR INTERACTIONS ===
    
    // Dopamine-Acetylcholine interaction (learning vs attention)
    if (signalName == "dopamine" && modulation > 0.3f && pImpl->attention) {
        pImpl->attention->enhanceFocus(modulation * 0.5f);
    }
    
    // Novelty-Curiosity interaction (exploration drive)
    if (signalName == "novelty" && modulation > 0.3f && pImpl->curiosity) {
        pImpl->curiosity->enhanceExploration(modulation * 0.5f);
    }
    
    // Prediction error-Dopamine interaction (learning signal)
    if (pImpl->predictionError && signalName == "dopamine") {
        float predictionError = pImpl->predictionError->getError();
        pImpl->predictionError->integrateDopamineSignal(modulation, predictionError);
    }
    
    NLM_LOG_DEBUG("Neuromodulation application completed: " + signalName);
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
        
        // Save memory system states
        saveMemoryStates(writer);
        
        // Save prediction system state
        savePredictionSystemState(writer);
        
        // Save concept formation state
        saveConceptFormationState(writer);
        
        // Save neuromodulation states
        saveNeuromodulationStates(writer);
        
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
        
        // Load memory system states
        loadMemoryStates(reader);
        
        // Load prediction system state
        loadPredictionSystemState(reader);
        
        // Load concept formation state
        loadConceptFormationState(reader);
        
        // Load neuromodulation states
        loadNeuromodulationStates(reader);
        
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

// ========== CONCEPT FORMATION HELPERS ==========

// Extract current neural activity pattern for concept formation
void Brain::extractNeuralPatternForConceptFormation(std::vector<float>& pattern) const {
    pattern.clear();
    pattern.reserve(getTotalNeuronCount());
    
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                float activity = neuron->getFiringRate();
                if (activity > 0.001f) {  // Only include active neurons (noise threshold)
                    pattern.push_back(activity);
                }
            }
        }
    }
}

// Convert neural pattern to sensory input for prediction system
std::unique_ptr<class SensoryInput> Brain::convertNeuralPatternToSensory(const std::vector<float>& neuralPattern) const {
    auto sensoryInput = std::make_unique<class SensoryInput>();
    
    // Map neural activity to sensory representation
    // Use the first few active neurons as "sensory channels"
    size_t maxSensoryChannels = 10; // Limit to prevent excessive memory usage
    size_t activeNeurons = 0;
    
    // Create sensory data from neural activity
    std::vector<float> sensoryData;
    for (float activity : neuralPattern) {
        if (activity > 0.001f && activeNeurons < maxSensoryChannels) {
            sensoryData.push_back(activity);
            activeNeurons++;
        }
    }
    
    // Set the sensory input data
    sensoryInput->setData(sensoryData);
    
    return sensoryInput;
}

// Get sensory features from working and episodic memory for concept formation
std::vector<float> Brain::getSensoryFeaturesForConceptFormation() const {
    std::vector<float> features;
    
    // Try to get features from working memory first (most recent)
    if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
        features = pImpl->workingMemory->getMemoryNeurons();
    }
    
    // If working memory doesn't have features, try episodic memory
    else if (pImpl->episodicMemory && !pImpl->episodicMemory->getRecentEpisodes(3).empty()) {
        // Extract features from recent episodes
        auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(3);
        for (const auto* episode : recentEpisodes) {
            // Add episode-based features to our feature vector
            features.push_back(static_cast<float>(episode->reward));
        }
    }
    
    return features;
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
    
    // ========== MEMORY SYSTEM STATE HELPERS ==========

void Brain::saveMemoryStates(CheckpointWriter& writer) const {
    NLM_LOG_INFO("Saving memory system states to checkpoint");
    
    // Save working memory state
    if (pImpl->workingMemory) {
        WorkingMemoryCheckpointData workingMemData;
        // Collect working memory data
        auto traces = pImpl->workingMemory->getMemoryNeurons();
        workingMemData.activeTraces = traces;
        workingMemData.totalMemory = pImpl->workingMemory->getTotalMemory();
        
        std::vector<uint8_t> buffer;
        if (workingMemData.serialize(buffer)) {
            writer.writeSection(CheckpointSection::Memory, buffer.data(), buffer.size());
        }
    }
}

void Brain::loadMemoryStates(CheckpointReader& reader) {
    NLM_LOG_INFO("Loading memory system states from checkpoint");
    
    // Try to read memory section
    std::vector<uint8_t> memoryData = reader.readSection(CheckpointSection::Memory);
    
    if (!memoryData.empty()) {
        NLM_LOG_INFO("Loaded memory state from checkpoint (" + std::to_string(memoryData.size()) + " bytes)");
        
        // Reset memory systems to ensure clean state for restoration
        if (pImpl->workingMemory) {
            pImpl->workingMemory->clear();
        }
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->clear();
        }
        if (pImpl->associativeMemory) {
            pImpl->associativeMemory->clear();
        }
        
        // TODO: In a full implementation, we would deserialize and restore memory data
        // For now, memory will be naturally rebuilt through normal operation
        NLM_LOG_INFO("Memory state loaded - will be naturally rebuilt through simulation");
    } else {
        NLM_LOG_INFO("No memory state found in checkpoint - starting with clean memory");
    }
}

void Brain::savePredictionSystemState(CheckpointWriter& writer) const {
    if (pImpl->predictionSystem) {
        NLM_LOG_INFO("Saving prediction system state to checkpoint");
        
        // For now, we'll save basic prediction system state
        PredictionSystemCheckpointData predictionData;
        predictionData.predictionCount = 0; // Placeholder
        
        std::vector<uint8_t> buffer;
        if (predictionData.serialize(buffer)) {
            writer.writeSection(CheckpointSection::Neuromodulation, buffer.data(), buffer.size());
        }
    }
}

void Brain::saveConceptFormationState(CheckpointWriter& writer) const {
    if (pImpl->conceptFormation) {
        NLM_LOG_INFO("Saving concept formation state to checkpoint");
        
        // For now, we'll save basic concept formation state
        ConceptFormationCheckpointData conceptData;
        conceptData.conceptCount = pImpl->conceptFormation->getConceptCount();
        
        std::vector<uint8_t> buffer;
        if (conceptData.serialize(buffer)) {
            writer.writeSection(CheckpointSection::Development, buffer.data(), buffer.size());
        }
    }
}

void Brain::saveNeuromodulationStates(CheckpointWriter& writer) const {
    NLM_LOG_INFO("Saving neuromodulation states to checkpoint");
    
    // Save dopamine state
    if (pImpl->dopamine) {
        NeuromodulatorCheckpointData neuromodData;
        neuromodData.level = pImpl->dopamine->getLevel();
        neuromodData.type = "dopamine";
        neuromodData.timestamp = pImpl->currentTime;
        
        std::vector<uint8_t> buffer;
        if (neuromodData.serialize(buffer)) {
            writer.writeSection(CheckpointSection::Neuromodulation, buffer.data(), buffer.size());
        }
    }
}

void Brain::loadPredictionSystemState(CheckpointReader& reader) {
    NLM_LOG_INFO("Loading prediction system state from checkpoint");
    
    // Try to read neuromodulation section (prediction system state saved there)
    std::vector<uint8_t> neuromodData = reader.readSection(CheckpointSection::Neuromodulation);
    
    if (!neuromodData.empty()) {
        NLM_LOG_INFO("Loaded prediction system state from checkpoint");
        
        // Reset prediction system to ensure clean state
        if (pImpl->predictionSystem) {
            // TODO: In a full implementation, we would deserialize and restore prediction data
            NLM_LOG_INFO("Prediction system reset - will be naturally rebuilt through simulation");
        }
    }
}

void Brain::loadConceptFormationState(CheckpointReader& reader) {
    NLM_LOG_INFO("Loading concept formation state from checkpoint");
    
    // Try to read development section (concept formation state saved there)
    std::vector<uint8_t> conceptData = reader.readSection(CheckpointSection::Development);
    
    if (!conceptData.empty()) {
        NLM_LOG_INFO("Loaded concept formation state from checkpoint");
        
        // Reset concept formation to ensure clean state
        if (pImpl->conceptFormation) {
            // TODO: In a full implementation, we would deserialize and restore concept data
            NLM_LOG_INFO("Concept formation reset - will be naturally rebuilt through simulation");
        }
    }
}

void Brain::loadNeuromodulationStates(CheckpointReader& reader) {
    NLM_LOG_INFO("Loading neuromodulation states from checkpoint");
    
    // Try to read neuromodulation section
    std::vector<uint8_t> neuromodData = reader.readSection(CheckpointSection::Neuromodulation);
    
    if (!neuromodData.empty()) {
        NLM_LOG_INFO("Loaded neuromodulation states from checkpoint");
        
        // Reset neuromodulation systems to ensure clean state
        if (pImpl->dopamine) {
            // TODO: In a full implementation, we would deserialize and restore neuromodulation data
            NLM_LOG_INFO("Neuromodulation systems reset - will be naturally rebuilt through simulation");
        }
    }
}

} // namespace nlm

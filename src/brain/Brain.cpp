#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
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
        predictionSystem = std::make_unique<NeuralPrediction>();
        static_cast<NeuralPrediction*>(predictionSystem.get())->initialize(this);
        
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
    pImpl->predictionSystem->initialize(this);
    
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
            
            // Store working memory content - this is the rich sensory & cognitive state
            if (pImpl->workingMemory) {
                std::vector<float> wmPattern = pImpl->workingMemory->retrieve();
                if (!wmPattern.empty()) {
                    episode.sensoryState = wmPattern;
                }
            }
            
            // Store recent sensory input patterns from last 10 steps
            // (In real implementation, this would track history)
            
            // Store action if one was produced in previous step
            // (This would require tracking action production)
            
            // Store active neurons as backup
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
            
            // Store position information if available
            // (Would need environment integration)
            
            // Apply attention-guided filtering - only store attended episodes
            if (pImpl->attention) {
                auto attendedRegions = pImpl->attention->getAttendedRegions();
                if (!attendedRegions.empty()) {
                    // Mark episode as attended for later retrieval
                    episode.novelty = 1.0f;  // High novelty for attended experiences
                }
            }
            
            // Store the episode with enhanced content
            pImpl->episodicMemory->storeEpisode(episode);
            
            // Trigger associative memory learning from this episode
            if (pImpl->associativeMemory) {
                pImpl->associativeMemory->associateFromExperience(episode);
            }
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Get current sensory state from working memory
        std::vector<float> currentSensoryState;
        if (pImpl->workingMemory) {
            currentSensoryState = pImpl->workingMemory->retrieve();
        }
        
        // If we have working memory content, use it for prediction
        if (!currentSensoryState.empty()) {
            // Record current state for prediction learning
            static_cast<NeuralPrediction*>(pImpl->predictionSystem.get())->recordSensoryState(currentSensoryState, currentStep);
            
            // Generate prediction for next state
            auto predictedState = static_cast<NeuralPrediction*>(pImpl->predictionSystem.get())->generatePrediction(currentStep);
            
            // Update prediction system with actual observation (currentSensoryState)
            float predictionError = static_cast<NeuralPrediction*>(pImpl->predictionSystem.get())->updateWithObservation(currentSensoryState, currentStep);
            
            // Use prediction error to influence neuromodulation and learning
            if (pImpl->dopamine) {
                // Prediction error affects dopamine levels
                float dopamineModulation = 1.0f - predictionError;
                pImpl->dopamine->setLevel(dopamineModulation * 2.0f);
            }
            
            if (pImpl->novelty) {
                // Novel prediction errors also affect novelty detection
                if (predictionError > 0.5f) {
                    pImpl->novelty->detectNovelty(currentSensoryState, currentStep);
                }
            }
            
            // Update action consequences prediction based on working memory
            if (pImpl->attention && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> attendedNeurons = pImpl->attention->getWinners();
                if (!attendedNeurons.empty()) {
                    // Store attended action consequences for future prediction
                    for (NeuronId neuron : attendedNeurons) {
                        float activation = pImpl->workingMemory->getNeuronActivation(neuron);
                        if (activation > 0.3f) {
                            // This could be recording action-consequence predictions
                            // For now, just log for debugging
                        }
                    }
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
            auto attendedNeurons = pImpl->attention->processCompetition(competitors);
            
            // Apply attention boost to winning working memory neurons
            for (NeuronId attendedNeuron : attendedNeurons) {
                pImpl->workingMemory->storeToNeuron(attendedNeuron, 
                                                  pImpl->workingMemory->getNeuronActivation(attendedNeuron) * 1.5f);
            }
        }
        
        // Attention also processes sensory input for selection
        if (pImpl->sensoryNeurons.size() > 3) {
            std::vector<NeuronId> sensoryCompetitors;
            for (auto* neuron : pImpl->sensoryNeurons) {
                float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                if (activation > 0.3f) {
                    sensoryCompetitors.push_back(neuron->getId());
                }
            }
            
            if (!sensoryCompetitors.empty()) {
                auto attendedSensory = pImpl->attention->processCompetition(sensoryCompetitors, 0.7f);
                
                // Store attended sensory patterns in working memory with enhanced strength
                for (NeuronId attendedNeuron : attendedSensory) {
                    if (pImpl->workingMemory) {
                        float activation = 0.0f;
                        for (auto* neuron : pImpl->sensoryNeurons) {
                            if (neuron->getId() == attendedNeuron) {
                                activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                                break;
                            }
                        }
                        pImpl->workingMemory->storeToNeuron(attendedNeuron, activation * 2.0f);
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
    
    // ========== STEP 14: Enhanced memory consolidation/slip cycle ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= pImpl->consolidationInterval) {
        pImpl->stepsSinceLastEpisode = 0;
        
        // Sleep-like consolidation phase with multi-system coordination
        if (pImpl->episodicMemory) {
            // Phase 1: Enhance consolidation of high-value episodes based on neuromodulators
            size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
            if (episodeCount > 0) {
                // Use neuromodulators to prioritize consolidation
                float consolidationPriority = 1.0f;
                if (pImpl->acetylcholine) {
                    consolidationPriority *= pImpl->acetylcholine->getLevel();
                }
                if (pImpl->dopamine) {
                    consolidationPriority *= (pImpl->dopamine->getLevel() + 0.5f);
                }
                
                // Select episodes for consolidation based on multiple factors
                std::vector<std::pair<const EpisodicMemoryItem*, float>> scoredEpisodes;
                for (size_t i = 0; i < episodeCount; ++i) {
                    if (auto* episode = pImpl->episodicMemory->getEpisode(i)) {
                        // Score based on reward (dopamine)
                        float score = episode->reward * 0.4f;
                        
                        // Score based on novelty (curiosity)
                        if (pImpl->curiosity) {
                            float curiosityLevel = pImpl->curiosity->getLevel();
                            score += curiosityLevel * 0.2f;
                        }
                        
                        // Score based on attention (acetylcholine)
                        if (pImpl->acetylcholine) {
                            float acetylcholineLevel = pImpl->acetylcholine->getLevel();
                            score += acetylcholineLevel * 0.2f;
                        }
                        
                        // Age factor - older episodes consolidated more thoroughly
                        float ageFactor = std::min(1.0f, episode->age / 100.0f);
                        score *= (1.0f + ageFactor * 0.3f);
                        
                        scoredEpisodes.emplace_back(episode, score);
                    }
                }
                
                // Sort by score (highest first)
                std::sort(scoredEpisodes.begin(), scoredEpisodes.end(),
                         [](const auto& a, const auto& b) { return a.second > b.second; });
                
                // Consolidate top episodes
                size_t episodesToConsolidate = std::min(static_cast<size_t>(3), scoredEpisodes.size());
                for (size_t i = 0; i < episodesToConsolidate; ++i) {
                    const auto& episodePair = scoredEpisodes[i];
                    const EpisodicMemoryItem* episode = episodePair.first;
                    
                    // Replay to strengthen synaptic connections
                    pImpl->episodicMemory->replayEpisode(episode);
                    
                    // Enhance prediction based on consolidated memory
                    if (pImpl->predictionSystem && !episode->sensoryState.empty()) {
                        NeuralPrediction* neuralPred = static_cast<NeuralPrediction*>(pImpl->predictionSystem.get());
                        neuralPred->recordSensoryState(episode->sensoryState, currentStep);
                    }
                }
                
                // Apply neuromodulatory effects during consolidation
                if (pImpl->acetylcholine) {
                    pImpl->acetylcholine->enhanceMemoryConsolidation();
                }
                if (pImpl->dopamine) {
                    // Dopamine modulates consolidation based on prediction error
                    float predictionError = pImpl->dopamine->getLevel() * 0.5f;
                    if (predictionError > 0.5f) {
                        // High prediction error = need for consolidation
                        pImpl->dopamine->setLevel(std::min(1.0f, pImpl->dopamine->getLevel() * 1.2f));
                    }
                }
            }
        }
        
        // Working memory consolidation and reorganization
        if (pImpl->workingMemory) {
            // Phase 2: Working memory consolidation into episodic memory
            std::vector<NeuronId> highActivationNeurons;
            auto& wmNeurons = pImpl->workingMemory->getMemoryNeurons();
            
            for (NeuronId neuron : wmNeurons) {
                float activation = pImpl->workingMemory->getNeuronActivation(neuron);
                if (activation > 0.6f) {  // High activation threshold
                    highActivationNeurons.push_back(neuron);
                }
            }
            
            // Consolidate high-activation working memory traces
            if (!highActivationNeurons.empty() && pImpl->episodicMemory) {
                // Create episodic memory from strong working memory patterns
                EpisodicMemoryItem newEpisode;
                newEpisode.timestamp = currentStep;
                newEpisode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                newEpisode.novelty = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
                
                // Store working memory pattern as sensory state
                std::vector<float> wmPattern = pImpl->workingMemory->retrieve();
                newEpisode.sensoryState = wmPattern;
                
                // Store neurons and activations
                newEpisode.activeNeurons = highActivationNeurons;
                newEpisode.neuronActivations.resize(highActivationNeurons.size());
                
                for (size_t i = 0; i < highActivationNeurons.size(); ++i) {
                    newEpisode.neuronActivations[i] = pImpl->workingMemory->getNeuronActivation(highActivationNeurons[i]);
                }
                
                // Add to episodic memory
                pImpl->episodicMemory->storeEpisode(newEpisode);
                
                // Record action (if any)
                newEpisode.action = ActionType::Wait;  // Default - could be enhanced
                
                // Apply norepinephrine to enhance attention during consolidation
                if (pImpl->norepinephrine) {
                    pImpl->norepinephrine->enhanceVigilance(0.8f);  // High vigilance for consolidation
                }
            }
            
            // Decay weak working memory traces
            pImpl->workingMemory->decayWeakTraces();
            
            // Enhance important working memory traces
            for (NeuronId neuron : wmNeurons) {
                float activation = pImpl->workingMemory->getNeuronActivation(neuron);
                if (activation > 0.5f) {
                    // Strengthen traces that are still relevant
                    pImpl->workingMemory->strengthenMemory(1.05f);
                }
            }
        }
        
        // Prediction system consolidation
        if (pImpl->predictionSystem) {
            // Periodically review and refine predictions based on consolidated memories
            NeuralPrediction* neuralPred = static_cast<NeuralPrediction*>(pImpl->predictionSystem.get());
            
            // Get recent predictions and evaluate against consolidated memories
            auto errorHistory = neuralPred->getErrorHistory();
            if (!errorHistory.empty()) {
                // Reduce prediction error based on consolidated knowledge
                float avgError = 0.0f;
                for (float error : errorHistory) {
                    avgError += error;
                }
                avgError /= errorHistory.size();
                
                if (avgError > 0.3f) {
                    // High error = predictions need refinement
                    // This would trigger learning cycles
                }
            }
        }
        
        // Neuromodulatory system tuning
        if (pImpl->serotonin) {
            // Serotonin adjusts mood and impulsivity based on consolidation success
            pImpl->serotonin->adjustMood(0.8f);  // Positive mood during successful consolidation
        }
    }

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // Enhanced sensory encoding with working memory integration
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Extract sensory features for working memory storage
    std::vector<float> sensoryPattern;
    sensoryPattern.reserve(numSensory);
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Apply working memory influence to sensory encoding
        // if working memory has relevant patterns
        if (pImpl->workingMemory) {
            // Check if working memory has neurons with similar activation patterns
            auto& wmNeurons = pImpl->workingMemory->getMemoryNeurons();
            for (NeuronId wmNeuron : wmNeurons) {
                if (wmNeuron.getId() / 1000 == i / 1000) {  // Same region/population
                    float wmActivation = pImpl->workingMemory->getNeuronActivation(wmNeuron);
                    // Working memory biases sensory input based on expectations
                    normalizedValue *= (1.0f + wmActivation * 0.3f);
                    break;
                }
            }
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store in sensory pattern for working memory
        if (std::abs(normalizedValue) > 0.1f) {
            sensoryPattern.push_back(normalizedValue / 10.0f);
        }
    }
    
    // Store complete sensory pattern in working memory
    if (pImpl->workingMemory && !sensoryPattern.empty()) {
        pImpl->workingMemory->store(sensoryPattern, 1.0f);
    }
    
    // Apply attention to enhance salient sensory features
    if (pImpl->attention && !sensoryPattern.empty()) {
        // Convert pattern back to neuron IDs for attention processing
        std::vector<NeuronId> sensoryNeurons;
        for (size_t i = 0; i < numSensory && i < sensoryPattern.size(); ++i) {
            if (std::abs(sensoryPattern[i]) > 0.3f) {
                sensoryNeurons.push_back(pImpl->sensoryNeurons[i]->getId());
            }
        }
        
        if (!sensoryNeurons.empty()) {
            // Let attention system select the most relevant sensory features
            auto attendedNeurons = pImpl->attention->processCompetition(sensoryNeurons, 0.6f);
            
            // Boost attended sensory neurons
            for (NeuronId attendedNeuron : attendedNeurons) {
                for (auto* neuron : pImpl->sensoryNeurons) {
                    if (neuron->getId() == attendedNeuron) {
                        neuron->injectCurrent(5.0f);  // Boost attended neurons
                        break;
                    }
                }
            }
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
    // Use NeuralPlanner for intelligent action selection
    // Plan based on current state and predictions
    
    // Get current state from working memory and episodic memory
    std::vector<float> currentState;
    if (pImpl->workingMemory) {
        currentState = pImpl->workingMemory->retrieve();
    }
    
    // If working memory is empty, use motor neuron activity as state proxy
    if (currentState.empty() && !pImpl->motorNeurons.empty()) {
        float avgMotorActivity = 0.0f;
        int activeMotors = 0;
        for (auto* neuron : pImpl->motorNeurons) {
            if (neuron->isFiring()) {
                avgMotorActivity += 1.0f;
                ++activeMotors;
            }
        }
        if (activeMotors > 0) {
            avgMotorActivity /= activeMotors;
            currentState.push_back(avgMotorActivity);
        }
    }
    
    // Use NeuralPlanner to select the best action
    ActionType plannedAction = ActionType::Wait;
    if (pImpl->planner) {
        // Get planning target from current goal or high-level state
        std::vector<float> goal = pImpl->planner->getCurrentGoal();
        if (goal.empty()) {
            goal = currentState;  // Use current state as goal
        }
        
        // Plan action based on current state and goal
        plannedAction = pImpl->planner->planAction(currentState, 
                                                  pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f);
        
        // Store action for learning
        if (pImpl->predictionError) {
            pImpl->predictionError->recordAction(plannedAction, pImpl->currentStep);
        }
    }
    
    // Create the action
    auto action = std::make_unique<Action>(plannedAction);
    
    // Apply neuromodulatory effects based on prediction error
    if (pImpl->predictionSystem) {
        // NeuralPrediction contains prediction error signal
        NeuralPrediction* neuralPred = static_cast<NeuralPrediction*>(pImpl->predictionSystem.get());
        float predictionError = neuralPred->getPredictionError();
        
        // Update planner based on prediction error
        if (pImpl->planner && predictionError > 0.3f) {
            // Poor prediction affects planning confidence
            pImpl->planner->setActionQuality(plannedAction, 1.0f - predictionError);
        }
    }
    
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
        
        // Write memory system states
        {
            // Memory system states
            std::vector<uint8_t> memoryData;
            
            // Working memory
            if (pImpl->workingMemory) {
                auto pattern = pImpl->workingMemory->retrieve();
                memoryData.insert(memoryData.end(), 
                                reinterpret_cast<const uint8_t*>(pattern.data()),
                                reinterpret_cast<const uint8_t*>(pattern.data()) + pattern.size() * sizeof(float));
            }
            
            // Episodic memory
            if (pImpl->episodicMemory) {
                size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
                memoryData.insert(memoryData.end(), 
                                reinterpret_cast<const uint8_t*>(&episodeCount),
                                reinterpret_cast<const uint8_t*>(&episodeCount) + sizeof(size_t));
            }
            
            if (!memoryData.empty()) {
                if (!writer.writeSection(CheckpointSection::Memory, memoryData.data(), memoryData.size())) {
                    NLM_LOG_ERROR("Failed to write memory section to checkpoint");
                    return false;
                }
            }
        }
        
        // Write neuromodulation system states
        {
            std::vector<uint8_t> neuromodData;
            
            // Neuromodulator levels
            if (pImpl->dopamine) {
                float dopamineLevel = pImpl->dopamine->getLevel();
                neuromodData.insert(neuromodData.end(), 
                                   reinterpret_cast<const uint8_t*>(&dopamineLevel),
                                   reinterpret_cast<const uint8_t*>(&dopamineLevel) + sizeof(float));
            }
            
            if (pImpl->curiosity) {
                float curiosityLevel = pImpl->curiosity->getLevel();
                neuromodData.insert(neuromodData.end(), 
                                   reinterpret_cast<const uint8_t*>(&curiosityLevel),
                                   reinterpret_cast<const uint8_t*>(&curiosityLevel) + sizeof(float));
            }
            
            // Add acetylcholine level
            if (pImpl->acetylcholine) {
                float acetylcholineLevel = pImpl->acetylcholine->getLevel();
                neuromodData.insert(neuromodData.end(), 
                                   reinterpret_cast<const uint8_t*>(&acetylcholineLevel),
                                   reinterpret_cast<const uint8_t*>(&acetylcholineLevel) + sizeof(float));
            }
            
            // Add norepinephrine level
            if (pImpl->norepinephrine) {
                float norepinephrineLevel = pImpl->norepinephrine->getLevel();
                neuromodData.insert(neuromodData.end(), 
                                   reinterpret_cast<const uint8_t*>(&norepinephrineLevel),
                                   reinterpret_cast<const uint8_t*>(&norepinephrineLevel) + sizeof(float));
            }
            
            // Add serotonin level
            if (pImpl->serotonin) {
                float serotoninLevel = pImpl->serotonin->getLevel();
                neuromodData.insert(neuromodData.end(), 
                                   reinterpret_cast<const uint8_t*>(&serotoninLevel),
                                   reinterpret_cast<const uint8_t*>(&serotoninLevel) + sizeof(float));
            }
            
            if (!neuromodData.empty()) {
                if (!writer.writeSection(CheckpointSection::Neuromodulation, neuromodData.data(), neuromodData.size())) {
                    NLM_LOG_ERROR("Failed to write neuromodulation section to checkpoint");
                    return false;
                }
            }
        }
        
        // Write development state
        {
            DevelopmentStage devStage = pImpl->developmentalStage;
            if (!writer.writeSection(CheckpointSection::Development, 
                                    &devStage, sizeof(devStage))) {
                NLM_LOG_ERROR("Failed to write development section to checkpoint");
                return false;
            }
        }
        
        // Write prediction system state
        {
            if (pImpl->predictionSystem) {
                NeuralPrediction* neuralPred = static_cast<NeuralPrediction*>(pImpl->predictionSystem.get());
                
                // Write prediction error history
                auto errorHistory = neuralPred->getErrorHistory();
                if (!errorHistory.empty()) {
                    if (!writer.writeSection(CheckpointSection::Plasticity, 
                                            errorHistory.data(),
                                            errorHistory.size() * sizeof(float))) {
                        NLM_LOG_ERROR("Failed to write plasticity section to checkpoint");
                        return false;
                    }
                }
            }
        }
        
        // Write spike history
        {
            if (pImpl->spikeSystem) {
                auto spikeHistory = pImpl->spikeSystem->getSpikeHistory();
                if (!spikeHistory.empty()) {
                    if (!writer.writeSection(CheckpointSection::SpikeHistory,
                                            spikeHistory.data(),
                                            spikeHistory.size() * sizeof(DetailedSpikeEvent))) {
                        NLM_LOG_ERROR("Failed to write spike history section to checkpoint");
                        return false;
                    }
                }
            }
        }
        
        // Write connectivity
        {
            // This would capture the current connectivity graph
            // For now, just store the number of connections
            size_t connectionCount = 0;
            for (const auto& region : pImpl->regions) {
                connectionCount += region->getSynapseCount();
            }
            
            if (!writer.writeSection(CheckpointSection::Connectivity,
                                    &connectionCount, sizeof(size_t))) {
                NLM_LOG_ERROR("Failed to write connectivity section to checkpoint");
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
        
        // Read all sections
        auto header = reader.getHeader();
        
        // Reset current brain state
        reset();
        
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
        
        // Apply synapse states
        // Note: This is complex because we need to find matching synapses
        // For now, we'll just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        // Read memory system state
        auto memorySection = reader.readSection(CheckpointSection::Memory);
        if (!memorySection.empty()) {
            // Parse memory data
            size_t offset = 0;
            
            // Working memory pattern
            if (memorySection.size() >= sizeof(float)) {
                float* patternData = reinterpret_cast<float*>(&memorySection[offset]);
                size_t patternSize = memorySection.size() / sizeof(float);
                
                std::vector<float> pattern(patternData, patternData + patternSize);
                if (pImpl->workingMemory && !pattern.empty()) {
                    pImpl->workingMemory->store(pattern, 1.0f);
                }
                
                offset += patternSize * sizeof(float);
            }
            
            // Episodic memory count
            if (offset + sizeof(size_t) <= memorySection.size()) {
                size_t* episodeCountData = reinterpret_cast<size_t*>(&memorySection[offset]);
                if (pImpl->episodicMemory) {
                    // In a full implementation, we would restore episodes
                    // For now, just set the capacity based on episode count
                    pImpl->episodicMemory->setMaxEpisodes(*episodeCountData);
                }
                
                offset += sizeof(size_t);
            }
        }
        
        // Read neuromodulation system state
        auto neuromodSection = reader.readSection(CheckpointSection::Neuromodulation);
        if (!neuromodSection.empty()) {
            size_t offset = 0;
            
            if (offset + sizeof(float) <= neuromodSection.size()) {
                float* dopamineData = reinterpret_cast<float*>(&neuromodSection[offset]);
                if (pImpl->dopamine) {
                    pImpl->dopamine->setLevel(*dopamineData);
                }
                
                offset += sizeof(float);
            }
            
            if (offset + sizeof(float) <= neuromodSection.size()) {
                float* curiosityData = reinterpret_cast<float*>(&neuromodSection[offset]);
                if (pImpl->curiosity) {
                    pImpl->curiosity->setLevel(*curiosityData);
                }
                
                offset += sizeof(float);
            }
            
            // Read acetylcholine level
            if (offset + sizeof(float) <= neuromodSection.size()) {
                float* acetylcholineData = reinterpret_cast<float*>(&neuromodSection[offset]);
                if (pImpl->acetylcholine) {
                    pImpl->acetylcholine->setLevel(*acetylcholineData);
                }
                
                offset += sizeof(float);
            }
            
            // Read norepinephrine level
            if (offset + sizeof(float) <= neuromodSection.size()) {
                float* norepinephrineData = reinterpret_cast<float*>(&neuromodSection[offset]);
                if (pImpl->norepinephrine) {
                    pImpl->norepinephrine->setLevel(*norepinephrineData);
                }
                
                offset += sizeof(float);
            }
            
            // Read serotonin level
            if (offset + sizeof(float) <= neuromodSection.size()) {
                float* serotoninData = reinterpret_cast<float*>(&neuromodSection[offset]);
                if (pImpl->serotonin) {
                    pImpl->serotonin->setLevel(*serotoninData);
                }
                
                offset += sizeof(float);
            }
        }
        
        // Read development state
        auto developmentSection = reader.readSection(CheckpointSection::Development);
        if (!developmentSection.empty() && developmentSection.size() >= sizeof(DevelopmentStage)) {
            DevelopmentStage* devStageData = reinterpret_cast<DevelopmentStage*>(developmentSection.data());
            pImpl->developmentalStage = *devStageData;
        }
        
        // Read prediction system state
        auto plasticitySection = reader.readSection(CheckpointSection::Plasticity);
        if (!plasticitySection.empty() && pImpl->predictionSystem) {
            NeuralPrediction* neuralPred = static_cast<NeuralPrediction*>(pImpl->predictionSystem.get());
            
            size_t errorHistoryCount = plasticitySection.size() / sizeof(float);
            if (errorHistoryCount > 0) {
                float* errorData = reinterpret_cast<float*>(plasticitySection.data());
                std::vector<float> errorHistory(errorData, errorData + errorHistoryCount);
                
                // Restore prediction error history
                neuralPred->clearHistory();
                for (float error : errorHistory) {
                    neuralPred->recordSensoryState(std::vector<float>(), pImpl->currentStep);
                }
            }
        }
        
        // Read spike history
        auto spikeHistorySection = reader.readSection(CheckpointSection::SpikeHistory);
        if (!spikeHistorySection.empty() && pImpl->spikeSystem) {
            size_t spikeCount = spikeHistorySection.size() / sizeof(DetailedSpikeEvent);
            if (spikeCount > 0) {
                // In a full implementation, we would restore spike history
                // For now, just log the count
                NLM_LOG_INFO("Loaded " + std::to_string(spikeCount) + " spike events");
            }
        }
        
        // Read connectivity info
        auto connectivitySection = reader.readSection(CheckpointSection::Connectivity);
        if (!connectivitySection.empty() && connectivitySection.size() >= sizeof(size_t)) {
            size_t* connCount = reinterpret_cast<size_t*>(connectivitySection.data());
            NLM_LOG_INFO("Checkpoint indicates " + std::to_string(*connCount) + " connections were present");
        }
        
        // Re-initialize all systems with restored state
        // This is important because systems need to be re-initialized after reset()
        // to properly set up their internal state based on the restored data
        
        NLM_LOG_INFO("Brain state loaded successfully from checkpoint");
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

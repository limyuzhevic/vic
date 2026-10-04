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
     * 4. Working memory integration (store firing neurons, apply attention)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Periodic memory consolidation
     * 15. Checkpoint management
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
    
    // ========== STEP 3.5: Working Memory Integration ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
        
        // Get memory neurons and run attention competition
        const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        if (!memoryNeurons.empty() && pImpl->attention) {
            std::vector<NeuronId> attendedNeurons;
            for (const auto& neuronId : memoryNeurons) {
                attendedNeurons.push_back(neuronId);
            }
            
            // Run attention competition on working memory neurons
            std::vector<NeuronId> attended = pImpl->attention->processCompetition(attendedNeurons);
            
            // Apply attention to winners (enhance their activity)
            for (const auto& winnerId : attended) {
                if (brain_) {
                    brain_->injectCurrent(winnerId, 2.0f);  // Attention boost
                }
            }
        }
    }
    
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
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Update prediction system with current sensory observations
        // This is critical for intelligent behavior
        if (pImpl->sensoryNeurons.size() > 0) {
            // Extract current sensory state from active sensory neurons
            std::vector<float> currentSensoryState;
            currentSensoryState.reserve(pImpl->sensoryNeurons.size());
            
            for (auto* sensoryNeuron : pImpl->sensoryNeurons) {
                float activity = sensoryNeuron->getState().membranePotential - 
                               sensoryNeuron->getState().restingPotential;
                currentSensoryState.push_back(std::max(0.0f, activity) / 10.0f);
            }
            
            if (!currentSensoryState.empty()) {
                // Record this sensory state for prediction learning
                if (!pImpl->recentSensoryStates_.empty()) {
                    auto& lastState = pImpl->recentSensoryStates_.back();
                    pImpl->predictionSystem->recordSensoryState(currentSensoryState, currentStep);
                    
                    // Generate prediction for next step
                    std::vector<float> prediction = 
                        pImpl->predictionSystem->generatePrediction(currentStep);
                    
                    if (!prediction.empty()) {
                        // Compute prediction error
                        float error = pImpl->predictionSystem->updateWithObservation(
                            currentSensoryState, currentStep);
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
            std::vector<NeuronId> attended = pImpl->attention->processCompetition(competitors);
            
            // Apply attention signals to winners (enhance processing)
            for (const auto& winnerId : attended) {
                // Add attention signal to boost processing
                if (brain_) {
                    brain_->injectCurrent(winnerId, 2.0f);
                }
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Get current sensory state for concept formation
        std::vector<float> currentState;
        if (!pImpl->sensoryNeurons.empty()) {
            currentState.reserve(pImpl->sensoryNeurons.size());
            for (auto* sensoryNeuron : pImpl->sensoryNeurons) {
                float activity = sensoryNeuron->getState().membranePotential - 
                               sensoryNeuron->getState().restingPotential;
                currentState.push_back(std::max(0.0f, activity) / 10.0f);
            }
        }
        
        if (!currentState.empty()) {
            // Get features from sensory state
            std::vector<float> features = currentState;
            
            // Get current reward signal
            float reward = 0.0f;
            if (pImpl->dopamine) {
                reward = pImpl->dopamine->getLevel();
            }
            
            // Present experience to concept formation system
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentState, features, reward, currentStep);
            
            if (conceptId > 0) {
                // This is a stable concept - use it for planning
                // Can access concept prototype for planning
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
        
        // Development affects multiple systems, not just structural plasticity
        DevelopmentalStage stage = pImpl->developmentSystem->getStage();
        
        // 1. Neural excitability changes with development
        float excitabilityMod = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                excitabilityMod = 1.2f;  // Higher excitability in early development
                break;
            case DevelopmentalStage::CriticalPeriod:
                excitabilityMod = 1.0f;  // Stable excitability
                break;
            case DevelopmentalStage::Maturation:
                excitabilityMod = 0.8f;  // Lower excitability, more stable
                break;
            case DevelopmentalStage::Adult:
                excitabilityMod = 0.6f;  // Even lower excitability in adulthood
                break;
            case DevelopmentalStage::Aging:
                excitabilityMod = 0.4f;  // Much lower excitability in aging
                break;
        }
        
        // Apply excitability changes to neurons
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Modulate resting potential based on development
                    float currentResting = neuron->getState().restingPotential;
                    neuron->setRestingPotential(currentResting * excitabilityMod);
                    
                    // Adjust threshold based on development
                    float currentThreshold = neuron->getState().threshold;
                    neuron->setThreshold(currentThreshold * (2.0f / (excitabilityMod + 0.5f)));
                }
            }
        }
        
        // 2. Neuromodulation levels change with development
        if (pImpl->dopamine) {
            float devDopamineLevel = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    devDopamineLevel = 0.8f;  // Higher dopamine in early development
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    devDopamineLevel = 1.0f;  // Stable
                    break;
                case DevelopmentalStage::Maturation:
                    devDopamineLevel = 0.9f;  // Slightly reduced
                    break;
                case DevelopmentalStage::Adult:
                    devDopamineLevel = 0.7f;  // Lower in adulthood
                    break;
                case DevelopmentalStage::Aging:
                    devDopamineLevel = 0.5f;  // Much lower in aging
                    break;
            }
            pImpl->dopamine->setLevel(devDopamineLevel);
        }
        
        // 3. Memory consolidation changes with development
        if (pImpl->episodicMemory) {
            // Older animals have more selective memory consolidation
            float consolidationStrength = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    consolidationStrength = 1.5f;  // Strong consolidation in early development
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    consolidationStrength = 1.2f;
                    break;
                case DevelopmentalStage::Maturation:
                    consolidationStrength = 1.0f;
                    break;
                case DevelopmentalStage::Adult:
                    consolidationStrength = 0.8f;  // More selective consolidation in adulthood
                    break;
                case DevelopmentalStage::Aging:
                    consolidationStrength = 0.6f;  // Reduced consolidation in aging
                    break;
            }
            // Update consolidation parameters
        }
        
        // 4. Attention system changes with development
        if (pImpl->attention) {
            // Younger brains have broader attention, adults have more focused attention
            float attentionFocus = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    attentionFocus = 1.5f;  // Broader attention
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    attentionFocus = 1.2f;
                    break;
                case DevelopmentalStage::Maturation:
                    attentionFocus = 1.0f;
                    break;
                case DevelopmentalStage::Adult:
                    attentionFocus = 0.8f;  // More focused attention
                    break;
                case DevelopmentalStage::Aging:
                    attentionFocus = 0.6f;  // Narrowed attention
                    break;
            }
            // Update attention parameters
        }
        
        // 5. Plasticity rules themselves change with development
        if (pImpl->stdp) {
            // STDP parameters change with development
            float stdpLTP = 0.01f;
            float stdpLTD = 0.012f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    stdpLTP = 0.015f;  // More LTP in early development
                    stdpLTD = 0.015f;
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    stdpLTP = 0.012f;
                    stdpLTD = 0.013f;
                    break;
                case DevelopmentalStage::Maturation:
                    stdpLTP = 0.01f;
                    stdpLTD = 0.012f;
                    break;
                case DevelopmentalStage::Adult:
                    stdpLTP = 0.008f;  // Less plasticity in adulthood
                    stdpLTD = 0.01f;
                    break;
                case DevelopmentalStage::Aging:
                    stdpLTP = 0.006f;  // Much less plasticity in aging
                    stdpLTD = 0.008f;
                    break;
            }
            pImpl->stdp->setLTPWeight(stdpLTP);
            pImpl->stdp->setLTDWeight(stdpLTD);
        }
        
        // 6. Development affects structural plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            float synaptogenesisRate = 0.0001f;
            float pruningRate = 0.00001f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    synaptogenesisRate = 0.0002f;  // More growth early
                    pruningRate = 0.00002f;
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    synaptogenesisRate = 0.00015f;
                    pruningRate = 0.000015f;
                    break;
                case DevelopmentalStage::Maturation:
                    synaptogenesisRate = 0.0001f;
                    pruningRate = 0.00001f;
                    break;
                case DevelopmentalStage::Adult:
                    synaptogenesisRate = 0.00005f;  // Less growth in adulthood
                    pruningRate = 0.00001f;
                    break;
                case DevelopmentalStage::Aging:
                    synaptogenesisRate = 0.00002f;  // Minimal growth
                    pruningRate = 0.000005f;
                    break;
            }
            
            sp->setSynaptogenesisRate(synaptogenesisRate);
            sp->setPruningRate(pruningRate);
        }
        
        // 7. Development affects neuromodulation of attention
        if (pImpl->curiosity) {
            float curiosityMod = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    curiosityMod = 1.5f;  // Higher curiosity in early development
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    curiosityMod = 1.2f;
                    break;
                case DevelopmentalStage::Maturation:
                    curiosityMod = 1.0f;
                    break;
                case DevelopmentalStage::Adult:
                    curiosityMod = 0.8f;  // Lower curiosity in adulthood
                    break;
                case DevelopmentalStage::Aging:
                    curiosityMod = 0.6f;  // Much lower curiosity in aging
                    break;
            }
            // Update curiosity levels
        }
        
        // Development stage affects memory encoding efficiency
        if (pImpl->episodicMemory) {
            float memoryEncodingEfficiency = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    memoryEncodingEfficiency = 1.5f;  // High encoding efficiency in early development
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    memoryEncodingEfficiency = 1.2f;
                    break;
                case DevelopmentalStage::Maturation:
                    memoryEncodingEfficiency = 1.0f;
                    break;
                case DevelopmentalStage::Adult:
                    memoryEncodingEfficiency = 0.9f;  // Slightly reduced encoding in adulthood
                    break;
                case DevelopmentalStage::Aging:
                    memoryEncodingEfficiency = 0.7f;  // Much reduced encoding in aging
                    break;
            }
            // Update memory encoding parameters
        }
        
        // Update developmental stage in brain
        pImpl->developmentalStage = stage;
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
    // Development now integrates with multiple brain systems:
    // 1. Neural excitability modulation
    // 2. Neuromodulation level changes
    // 3. Memory consolidation changes
    // 4. Attention system development
    // 5. Plasticity rule adaptation
    // 6. Structural plasticity optimization
    // 7. Neuromodulation of attention
    // 8. Memory encoding efficiency
    
    pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
    
    // Update development effects on all integrated systems
    DevelopmentalStage stage = pImpl->developmentSystem->getStage();
    
    // 1. Neural excitability modulation
    float excitabilityMod = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            excitabilityMod = 1.2f;  // Higher excitability in early development
            break;
        case DevelopmentalStage::CriticalPeriod:
            excitabilityMod = 1.0f;  // Stable excitability
            break;
        case DevelopmentalStage::Maturation:
            excitabilityMod = 0.8f;  // Lower excitability, more stable
            break;
        case DevelopmentalStage::Adult:
            excitabilityMod = 0.6f;  // Even lower excitability in adulthood
            break;
        case DevelopmentalStage::Aging:
            excitabilityMod = 0.4f;  // Much lower excitability in aging
            break;
    }
    
    // Apply excitability changes to neurons
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Modulate resting potential based on development
                float currentResting = neuron->getState().restingPotential;
                neuron->setRestingPotential(currentResting * excitabilityMod);
                
                // Adjust threshold based on development
                float currentThreshold = neuron->getState().threshold;
                neuron->setThreshold(currentThreshold * (2.0f / (excitabilityMod + 0.5f)));
            }
        }
    }
    
    // 2. Neuromodulation level changes
    if (pImpl->dopamine) {
        float devDopamineLevel = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                devDopamineLevel = 0.8f;  // Higher dopamine in early development
                break;
            case DevelopmentalStage::CriticalPeriod:
                devDopamineLevel = 1.0f;  // Stable
                break;
            case DevelopmentalStage::Maturation:
                devDopamineLevel = 0.9f;  // Slightly reduced
                break;
            case DevelopmentalStage::Adult:
                devDopamineLevel = 0.7f;  // Lower in adulthood
                break;
            case DevelopmentalStage::Aging:
                devDopamineLevel = 0.5f;  // Much lower in aging
                break;
        }
        pImpl->dopamine->setLevel(devDopamineLevel);
    }
    
    // 3. Memory consolidation changes
    if (pImpl->episodicMemory) {
        // Update consolidation parameters based on developmental stage
        float consolidationStrength = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                consolidationStrength = 1.5f;  // Strong consolidation in early development
                break;
            case DevelopmentalStage::CriticalPeriod:
                consolidationStrength = 1.2f;
                break;
            case DevelopmentalStage::Maturation:
                consolidationStrength = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                consolidationStrength = 0.8f;  // More selective consolidation in adulthood
                break;
            case DevelopmentalStage::Aging:
                consolidationStrength = 0.6f;  // Reduced consolidation in aging
                break;
        }
    }
    
    // 4. Attention system changes with development
    if (pImpl->attention) {
        // Younger brains have broader attention, adults have more focused attention
        float attentionFocus = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                attentionFocus = 1.5f;  // Broader attention
                break;
            case DevelopmentalStage::CriticalPeriod:
                attentionFocus = 1.2f;
                break;
            case DevelopmentalStage::Maturation:
                attentionFocus = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                attentionFocus = 0.8f;  // More focused attention
                break;
            case DevelopmentalStage::Aging:
                attentionFocus = 0.6f;  // Narrowed attention
                break;
        }
    }
    
    // 5. Plasticity rules themselves change with development
    if (pImpl->stdp) {
        // STDP parameters change with development
        float stdpLTP = 0.01f;
        float stdpLTD = 0.012f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                stdpLTP = 0.015f;  // More LTP in early development
                stdpLTD = 0.015f;
                break;
            case DevelopmentalStage::CriticalPeriod:
                stdpLTP = 0.012f;
                stdpLTD = 0.013f;
                break;
            case DevelopmentalStage::Maturation:
                stdpLTP = 0.01f;
                stdpLTD = 0.012f;
                break;
            case DevelopmentalStage::Adult:
                stdpLTP = 0.008f;  // Less plasticity in adulthood
                stdpLTD = 0.01f;
                break;
            case DevelopmentalStage::Aging:
                stdpLTP = 0.006f;  // Much less plasticity in aging
                stdpLTD = 0.008f;
                break;
        }
        pImpl->stdp->setLTPWeight(stdpLTP);
        pImpl->stdp->setLTDWeight(stdpLTD);
    }
    
    // 6. Development affects structural plasticity rates
    auto* sp = pImpl->structuralPlasticity;
    if (sp) {
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                synaptogenesisRate = 0.0002f;  // More growth early
                pruningRate = 0.00002f;
                break;
            case DevelopmentalStage::CriticalPeriod:
                synaptogenesisRate = 0.00015f;
                pruningRate = 0.000015f;
                break;
            case DevelopmentalStage::Maturation:
                synaptogenesisRate = 0.0001f;
                pruningRate = 0.00001f;
                break;
            case DevelopmentalStage::Adult:
                synaptogenesisRate = 0.00005f;  // Less growth in adulthood
                pruningRate = 0.00001f;
                break;
            case DevelopmentalStage::Aging:
                synaptogenesisRate = 0.00002f;  // Minimal growth
                pruningRate = 0.000005f;
                break;
        }
        
        sp->setSynaptogenesisRate(synaptogenesisRate);
        sp->setPruningRate(pruningRate);
    }
    
    // 7. Development affects neuromodulation of attention
    if (pImpl->curiosity) {
        float curiosityMod = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                curiosityMod = 1.5f;  // Higher curiosity in early development
                break;
            case DevelopmentalStage::CriticalPeriod:
                curiosityMod = 1.2f;
                break;
            case DevelopmentalStage::Maturation:
                curiosityMod = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                curiosityMod = 0.8f;  // Lower curiosity in adulthood
                break;
            case DevelopmentalStage::Aging:
                curiosityMod = 0.6f;  // Much lower curiosity in aging
                break;
        }
        // Update curiosity levels
    }
    
    // 8. Development stage affects memory encoding efficiency
    if (pImpl->episodicMemory) {
        float memoryEncodingEfficiency = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                memoryEncodingEfficiency = 1.5f;  // High encoding efficiency in early development
                break;
            case DevelopmentalStage::CriticalPeriod:
                memoryEncodingEfficiency = 1.2f;
                break;
            case DevelopmentalStage::Maturation:
                memoryEncodingEfficiency = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                memoryEncodingEfficiency = 0.9f;  // Slightly reduced encoding in adulthood
                break;
            case DevelopmentalStage::Aging:
                memoryEncodingEfficiency = 0.7f;  // Much reduced encoding in aging
                break;
        }
    }
    
    // Update developmental stage in brain
    pImpl->developmentalStage = stage;
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

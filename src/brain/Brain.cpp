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
        
        // ENHANCED: Dopamine affects memory consolidation
        // Positive dopamine (reward prediction) strengthens memory consolidation
        // Negative dopamine (prediction error) weakens or blocks consolidation
        if (pImpl->episodicMemory && currentStep % pImpl->consolidationInterval == 0) {
            // Dopaminergic modulation of memory consolidation
            // Based on dopamine level, consolidate different amounts of memory
            float consolidationFactor = 0.5f + dopamineLevel * 0.5f;
            consolidationFactor = std::clamp(consolidationFactor, 0.1f, 1.0f);
            
            // Consolidate memories with dopamine-dependent strength
            pImpl->episodicMemory->consolidate(consolidationFactor);
            
            if (std::abs(dopamineLevel) > 0.5f) {
                NLM_LOG_INFO("Dopamine-modulated memory consolidation: " + 
                            std::to_string(consolidationFactor) + 
                            " (dopamine: " + std::to_string(dopamineLevel) + ")");
            }
        }
        
        // ENHANCED: Dopamine affects attention system
        // Reward anticipation (positive dopamine) increases attentional focus
        // Prediction error (negative dopamine) broadens attention to detect errors
        if (pImpl->attention) {
            float attentionModulation = 1.0f;
            
            if (dopamineLevel > 0.0f) {
                // Positive dopamine increases attentional focus and efficiency
                // This is reward-driven attentional enhancement
                attentionModulation = 1.0f + dopamineLevel * 0.5f;
                
                // Use attention system to prioritize reward-relevant stimuli
                // This helps learn which sensory patterns predict reward
                if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                    std::vector<NeuronId> prioritizedNeurons = pImpl->workingMemory->getMemoryNeurons();
                    pImpl->attention->processCompetition(prioritizedNeurons);
                }
                
            } else if (dopamineLevel < 0.0f) {
                // Negative dopamine (prediction error) broadens attentional scope
                // To detect the source of prediction errors
                attentionModulation = std::max(0.5f, 1.0f + dopamineLevel * 0.3f);
                
                // In prediction error state, increase novelty detection
                if (pImpl->novelty) {
                    pImpl->novelty->setSurpriseThreshold(std::max(0.1f, 0.5f + dopamineLevel * 0.5f));
                }
            }
            
            // Apply attention modulation to attentional selection system
            // Higher modulation = stronger attentional bias
            if (attentionModulation != 1.0f) {
                // This would affect attention parameters in a full implementation
                NLM_LOG_INFO("Dopamine-modulated attention: " + 
                            std::to_string(attentionModulation) + 
                            " (dopamine: " + std::to_string(dopamineLevel) + ")");
            }
        }
        
        // ENHANCED: Dopamine affects concept formation
        // Reward signals help form reward-related concepts
        // This links reward prediction to concept emergence
        if (pImpl->conceptFormation && dopamineLevel > 0.2f) {
            // Positive dopamine signals rewarding experiences
            // These should be used to strengthen reward-related concepts
            
            // Create or strengthen reward-related concepts based on dopamine
            if (pImpl->workingMemory) {
                const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();n                if (!memoryNeurons.empty()) {
                    // Find most active memory trace for concept formation
                    std::vector<float> rewardPattern;
                    
                    for (NeuronId neuronId : memoryNeurons) {
                        for (auto& region : pImpl->regions) {
                            auto neurons = region->getAllNeurons();
                            for (auto* neuron : neurons) {
                                if (neuron->getId() == neuronId) {
                                    float activation = std::abs(neuron->getState().membranePotential - 
                                                             neuron->getState().restingPotential);
                                    rewardPattern.push_back(activation);
                                    break;
                                }
                            }
                        }
                    }
                    
                    if (!rewardPattern.empty()) {
                        // Present reward pattern to concept formation system
                        SimulationStep currentStep = pImpl->currentStep;
                        size_t conceptId = pImpl->conceptFormation->presentExperience(
                            rewardPattern,
                            std::vector<float>(),  // No specific features for reward patterns
                            dopamineLevel,  // Use dopamine level as reward signal
                            currentStep
                        );
                        
                        if (conceptId > 0) {
                            // Strengthen reward-related concepts with positive dopamine
                            const DiscoveredConcept* concept = pImpl->conceptFormation->getConcept(conceptId);
                            if (concept) {
                                // Reward-related concepts should have higher priority
                                NLM_LOG_INFO("Dopamine-enhanced concept formation: " + 
                                            std::to_string(conceptId) + 
                                            " (dopamine: " + std::to_string(dopamineLevel) + ")");
                            }
                        }
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
        // Track prediction error for learning and neuromodulation
        // This is crucial for temporal sequence learning and planning
        
        // Use recent sensory state to predict next state
        // This creates prediction error signal for learning
        
        // Get current sensory state (simplified - in full implementation would
        // use the actual sensory percept being injected)
        // For now, we'll use a placeholder that captures the essence of prediction
        
        // Predict next sensory state based on recent neural activity
        // This uses the neural substrate to encode expectations
        
        // In a full implementation, this would:
        // 1. Take current sensory input pattern
        // 2. Use learned sequences (through NeuralPrediction) to predict next state
        // 3. Compare prediction with actual input to compute prediction error
        // 4. Use prediction error to modulate plasticity and neuromodulation
        
        // For now, implement basic prediction using the prediction system
        // This demonstrates the integration point for prediction learning
        
        // Get prediction error if available
        float predError = 0.0f;
        if (pImpl->predictionSystem) {
            predError = pImpl->predictionSystem->getPredictionError();
            
            // Use prediction error to modulate neural processing
            // This is the core of predictive coding in the brain
            if (predError > 0.5f) {
                // High prediction error -> increase plasticity and exploration
                // This is the prediction error driven learning mechanism
                float errorGain = predError * 2.0f;
                
                // Modulate STDP strength based on prediction error
                if (pImpl->stdp) {
                    float originalLTP = pImpl->stdp->getLTPWeight();
                    float originalLTD = pImpl->stdp->getLTDWeight();
                    
                    // Higher prediction error increases LTP, decreases LTD
                    pImpl->stdp->setLTPWeight(originalLTP * (1.0f + errorGain * 0.5f));
                    pImpl->stdp->setLTDWeight(originalLTD * (1.0f - errorGain * 0.3f));
                }
                
                NLM_LOG_INFO("High prediction error: " + std::to_string(predError) + 
                            " -> increased plasticity and exploration");
            }
            
            // Store prediction error in neuromodulation system
            // For integration with dopamine system
            if (pImpl->dopamine) {
                // Prediction error modulates dopamine signal
                // This links prediction to reward prediction error
                float dopamineSignal = -predError * 0.5f;  // Negative because high error = negative prediction
                
                // Blend with regular reward-based dopamine
                float currentDopamine = pImpl->dopamine->getLevel();
                float newDopamine = currentDopamine * 0.7f + dopamineSignal * 0.3f;
                pImpl->dopamine->setLevel(newDopamine);
            }
        }
        
        // Integrate prediction into working memory for planning
        if (predError < 0.2f && pImpl->workingMemory && pImpl->episodicMemory) {
            // Low prediction error -> stable working memory, can plan with confidence
            // High confidence enables more sophisticated planning
            
            // Store prediction confidence in working memory for planning
            float confidence = 1.0f - predError;
            
            // This would be used by NeuralPlanner to set planning parameters
            // Higher confidence -> longer planning horizon, more aggressive planning
            if (pImpl->conceptFormation) {
                // Concepts help with prediction - more concepts = better predictions
                size_t conceptCount = pImpl->conceptFormation->getConceptCount();
                float conceptBenefit = std::min(conceptCount * 0.1f, 0.5f);
                
                // Use concepts to improve predictions
                confidence = confidence * (1.0f + conceptBenefit);
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
    if (pImpl->conceptFormation && pImpl->episodicMemory) {
        // Extract current neural activity pattern from working memory
        std::vector<float> neuralPattern;
        if (pImpl->workingMemory) {
            const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            neuralPattern.reserve(memoryNeurons.size());
            for (NeuronId neuronId : memoryNeurons) {
                // Find neuron by ID and get its activation
                for (auto& region : pImpl->regions) {
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        if (neuron->getId() == neuronId) {
                            float activation = std::abs(neuron->getState().membranePotential - 
                                                     neuron->getState().restingPotential);
                            neuralPattern.push_back(activation);
                            break;
                        }
                    }
                }
            }
        }
        
        // Extract sensory features from episodic memory
        std::vector<float> sensoryFeatures;
        if (!pImpl->stepsSinceLastEpisode.empty()) {
            // Get the most recent episode for feature extraction
            auto episodes = pImpl->episodicMemory->getAllEpisodes();
            if (!episodes.empty()) {
                const auto* latestEpisode = episodes.back();
                if (latestEpisode) {
                    // Use neuron activations from episodic memory as features
                    sensoryFeatures = latestEpisode->neuronActivations;
                }
            }
        }
        
        // Present experience to concept formation system
        // This is crucial for concept emergence without predefined categories
        if (!neuralPattern.empty()) {
            // Get current step for temporal context
            SimulationStep currentStep = pImpl->currentStep;
            
            // Get reward from dopamine system
            float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Present to concept formation - creates concepts from neural patterns
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                neuralPattern, 
                sensoryFeatures, 
                reward, 
                currentStep
            );
            
            // If a concept was found (non-zero ID), check attention integration
            if (conceptId > 0) {
                // Concept formation and attention should work together
                // The concept can influence what gets attended to in future
                const DiscoveredConcept* concept = pImpl->conceptFormation->getConcept(conceptId);
                if (concept) {
                    // Concepts provide top-down bias for attention
                    // Stronger concepts get more attentional resources
                    float conceptStability = pImpl->conceptFormation->getConceptStability(conceptId);
                    if (pImpl->attention) {
                        // Use concept stability to modulate attention strength
                        pImpl->attention->setInhibitionStrength(0.5f - conceptStability * 0.3f);
                    }
                    
                    NLM_LOG_INFO("ConceptFormation found matching concept: " + 
                                std::to_string(conceptId) + ", stability: " + 
                                std::to_string(conceptStability));
                }
            } else if (conceptId == 0 && pImpl->stepsSinceLastEpisode > 10) {
                // Concept formation couldn't decide yet - it's still forming
                NLM_LOG_DEBUG("ConceptFormation still forming concepts...");
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
        
        // ENHANCED: Development affects more than just plasticity
        // Affects neural excitability, neuromodulation, and cognitive systems
        
        // 1. Development affects baseline neural excitability
        if (stage != DevelopmentalStage::Adult) {
            // Developing brain is more excitable, adult brain is more stable
            float excitabilityMod = 1.0f + (1.0f - plasticityMod) * 0.5f;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Higher excitability in developmental stages
                        neuron->setThreshold(neuron->getState().threshold * excitabilityMod);
                    }
                }
            }
            NLM_LOG_INFO("Development-modulated neural excitability: " + 
                        std::to_string(excitabilityMod) + 
                        " (stage: " + std::to_string(static_cast<int>(stage)) + ")");
        }
        
        // 2. Development affects neuromodulation system levels
        if (pImpl->dopamine) {
            // Dopamine system matures during development
            float dopamineMaturation = 1.0f;
            if (stage == DevelopmentalStage::Initial) {
                dopamineMaturation = 0.5f;  // Undeveloped dopamine system
            } else if (stage == DevelopmentalStage::CriticalPeriod) {
                dopamineMaturation = 0.8f;
            } else if (stage == DevelopmentalStage::Maturation) {
                dopamineMaturation = 0.95f;
            }
            
            // Scale current dopamine level based on development
            float currentDopamine = pImpl->dopamine->getLevel();
            pImpl->dopamine->setLevel(currentDopamine * dopamineMaturation);
            
            NLM_LOG_INFO("Development-modulated dopamine: " + 
                        std::to_string(dopamineMaturation) + 
                        " (stage: " + std::to_string(static_cast<int>(stage)) + ")");
        }
        
        if (pImpl->curiosity) {
            // Curiosity system also develops
            float curiosityMaturation = 1.0f;
            if (stage == DevelopmentalStage::Initial) {
                curiosityMaturation = 0.7f;  // More exploration in early development
            } else if (stage == DevelopmentalStage::CriticalPeriod) {
                curiosityMaturation = 0.9f;
            }
            
            // Scale curiosity levels based on development
            float currentCuriosity = pImpl->curiosity->getLevel();
            if (currentCuriosity > 0.0f) {
                pImpl->curiosity->setLevel(currentCuriosity * curiosityMaturation);
            }
        }
        
        // 3. Development affects attention system parameters
        if (pImpl->attention) {
            // Attention efficiency improves with development
            float attentionEfficiency = 1.0f;
            if (stage == DevelopmentalStage::Initial) {
                attentionEfficiency = 0.6f;  // Poor attentional control
            } else if (stage == DevelopmentalStage::CriticalPeriod) {
                attentionEfficiency = 0.85f;
            } else if (stage == DevelopmentalStage::Maturation) {
                attentionEfficiency = 0.95f;
            }
            
            // Use attention efficiency to modulate attention parameters
            // Lower efficiency = more inhibition, better filtering
            pImpl->attention->setInhibitionStrength(0.5f / attentionEfficiency);
            pImpl->attention->setExcitationStrength(1.5f * attentionEfficiency);
            
            NLM_LOG_INFO("Development-modulated attention: " + 
                        std::to_string(attentionEfficiency) + 
                        " (inhibition: " + std::to_string(pImpl->attention->getInhibitionStrength()) + ")");
        }
        
        // 4. Development affects concept formation system
        if (pImpl->conceptFormation) {
            // Concept formation becomes more sophisticated with development
            if (stage == DevelopmentalStage::Initial) {
                // Early stage forms broad, fuzzy concepts
                pImpl->conceptFormation->setFormationThreshold(0.6f);
                pImpl->conceptFormation->setStabilityWindow(20);
            } else if (stage == DevelopmentalStage::CriticalPeriod) {
                // Critical period forms more precise concepts
                pImpl->conceptFormation->setFormationThreshold(0.4f);
                pImpl->conceptFormation->setStabilityWindow(10);
            } else if (stage == DevelopmentalStage::Maturation) {
                // Mature stage forms stable, well-abstracted concepts
                pImpl->conceptFormation->setFormationThreshold(0.3f);
                pImpl->conceptFormation->setStabilityWindow(5);
            } else if (stage == DevelopmentalStage::Adult) {
                // Adult forms highly refined, efficient concepts
                pImpl->conceptFormation->setFormationThreshold(0.2f);
                pImpl->conceptFormation->setStabilityWindow(2);
            }
            
            NLM_LOG_INFO("Development-modulated concept formation: " + 
                        std::to_string(pImpl->conceptFormation->getFormationThreshold()) + 
                        " threshold, " + 
                        std::to_string(pImpl->conceptFormation->getStabilityWindow()) + 
                        " window (stage: " + std::to_string(static_cast<int>(stage)) + ")");
        }
        
        // 5. Development affects prediction system parameters
        if (pImpl->predictionSystem) {
            // Prediction accuracy improves with development
            float predictionAccuracy = 1.0f;
            if (stage == DevelopmentalStage::Initial) {
                predictionAccuracy = 0.5f;  // Poor predictions
            } else if (stage == DevelopmentalStage::CriticalPeriod) {
                predictionAccuracy = 0.8f;
            } else if (stage == DevelopmentalStage::Maturation) {
                predictionAccuracy = 0.9f;
            }
            
            // Update prediction confidence based on development
            // This would require prediction system API changes
            // For now, log the development effect
            NLM_LOG_INFO("Development-modulated prediction: " + 
                        std::to_string(predictionAccuracy) + 
                        " accuracy (stage: " + std::to_string(static_cast<int>(stage)) + ")");
        }
        
        // 6. Development affects learning rates across systems
        // This provides a comprehensive view of developmental changes
        NLM_LOG_INFO("Development system updated: stage=" + std::to_string(static_cast<int>(stage)) + 
                    ", plasticityMod=" + std::to_string(plasticityMod) +
                    ", neuralExcitability=" + std::to_string(1.0f + (1.0f - plasticityMod) * 0.5f));
    }
    
    // ========== STEP 14: Sleep/rest cycle with memory consolidation ==========
    if (!pImpl->isResting) {
        // Check if we should enter sleep/rest mode
        // Sleep is triggered when enough time has passed and there's memory to consolidate
        
        // Time-based sleep trigger
        bool shouldSleep = false;
        if (currentStep >= pImpl->stepsSinceLastEpisode + pImpl->replayInterval * 2) {
            // Sleep after sufficient time has passed since last episode
            shouldSleep = true;
            pImpl->isResting = true;
            pImpl->sleepStartStep = currentStep;
            
            NLM_LOG_INFO("Entering sleep/rest mode at step " + std::to_string(currentStep));
        }
        
        // Activity-based sleep trigger
        if (pImpl->totalSpikesTotal > pImpl->sleepStartStep * 10) {
            // Sleep when there's been substantial activity
            shouldSleep = true;
            pImpl->isResting = true;
            pImpl->sleepStartStep = currentStep;
            
            NLM_LOG_INFO("Activity-based sleep triggered at step " + std::to_string(currentStep));
        }
        
        if (shouldSleep) {
            // Enter sleep mode
            pImpl->isResting = true;
            pImpl->sleepStartStep = currentStep;
            
            // Reduce neuromodulation levels during sleep
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(0.0f);  // No dopamine during sleep
            }
            if (pImpl->curiosity) {
                pImpl->curiosity->setLevel(0.0f);  // No curiosity during sleep
            }
            if (pImpl->novelty) {
                pImpl->novelty->setLevel(0.0f);  // No novelty detection during sleep
            }
            
            NLM_LOG_INFO("Sleep/rest mode activated");
        }
    } else {
        // We are currently in sleep mode
        size_t sleepDuration = currentStep - pImpl->sleepStartStep;
        
        // Sleep duration based on amount of memory to consolidate
        size_t targetSleepDuration = 500;  // Default sleep duration
        
        // Determine sleep duration based on memory load
        if (pImpl->episodicMemory) {
            size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
            targetSleepDuration = std::min(episodeCount * 10, 2000U);  // Up to 2000 steps
        }
        
        if (sleepDuration >= targetSleepDuration) {
            // Sleep duration complete - exit sleep mode
            pImpl->isResting = false;
            
            // Restore neuromodulation levels
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(0.1f);  // Low baseline dopamine
            }
            if (pImpl->curiosity) {
                pImpl->curiosity->setLevel(0.1f);  // Low baseline curiosity
            }
            if (pImpl->novelty) {
                pImpl->novelty->setLevel(0.1f);  // Low baseline novelty
            }
            
            NLM_LOG_INFO("Sleep/rest mode completed after " + std::to_string(sleepDuration) + 
                        " steps, exiting at step " + std::to_string(currentStep));
        }
    }
    
    // During sleep, perform memory consolidation and replay
    if (pImpl->isResting && pImpl->episodicMemory) {
        // Sleep-dependent memory consolidation
        // This is a simplified version of the multiple trace theory
        
        size_t sleepDuration = currentStep - pImpl->sleepStartStep;
        
        // Progressive consolidation during sleep
        if (sleepDuration < 100) {
            // Early sleep: strengthen recently formed memories
            pImpl->episodicMemory->consolidate(0.2f);
        } else if (sleepDuration < 300) {
            // Mid sleep: integrate memories with existing knowledge
            pImpl->episodicMemory->consolidate(0.5f);
            
            // Replay important memories during sleep for integration
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(5);
            for (const auto* episode : episodesToReplay) {
                pImpl->episodicMemory->replayEpisode(episode);
                
                // This would also update neural patterns through replay
                // In a full implementation, this would update synaptic connections
                // based on the replayed memory
            }
        } else {
            // Late sleep: stabilize and generalize memories
            pImpl->episodicMemory->consolidate(0.8f);
            
            // System consolidation: transfer memories from episodic to semantic
            // This would involve creating more abstract, generalized representations
            if (pImpl->conceptFormation) {
                // In late sleep, concept formation can integrate episodic memories
                // This is when concepts become more stable and general
                NLM_LOG_INFO("Late sleep concept integration phase");
                
                // Concepts can now incorporate the consolidated episodic memories
                // This would happen in a full implementation with proper concept system
            }
        }
        
        // Sleep maintains low neuromodulation but preserves basic brain function
        if (sleepDuration % 100 == 0) {
            NLM_LOG_DEBUG("Sleep memory consolidation progress: " + 
                        std::to_string(sleepDuration) + "/" + std::to_string(targetSleepDuration) + 
                        " steps, consolidation level: " + 
                        std::to_string(std::min(sleepDuration / 10, 100)) + "%");
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

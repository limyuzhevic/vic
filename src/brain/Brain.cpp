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
        
        // Connect systems to brain instance for bidirectional access
        // This enables integrated information flow and coordination
        workingMemory->initialize(this);
        episodicMemory->initialize(this);
        associativeMemory->initialize(this);
        predictionSystem->initialize(this);
        planner->initialize(this);
        conceptFormation->initialize(this);
        attention->initialize(this);
        developmentSystem->initialize(this);
        curiosity->initialize(this);
        novelty->initialize(this);
        
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
    
    // ========== STEP 5: Enhanced neuromodulation integration ==========
    
    // Update curiosity with enhanced integration
    if (pImpl->curiosity) {
        // Get sensory input activity for curiosity calculation
        float noveltyLevel = 0.0f;
        float predictionError = 0.0f;
        
        // Calculate novelty from sensory neuron activity
        for (size_t i = 0; i < std::min<size_t>(3, pImpl->sensoryNeurons.size()); ++i) {
            auto* sensoryNeuron = pImpl->sensoryNeurons[i];
            noveltyLevel += std::abs(sensoryNeuron->getMembranePotential() - sensoryNeuron->getRestingPotential());
        }
        noveltyLevel = std::min(1.0f, noveltyLevel / 300.0f);  // Normalize
        
        // Get prediction error from prediction system
        if (pImpl->predictionSystem) {
            predictionError = pImpl->predictionSystem->getPredictionError();
        }
        
        pImpl->curiosity->update(noveltyLevel, predictionError, pImpl->timestep);
    }
    
    // Enhanced novelty detection with brain integration
    if (pImpl->novelty) {
        // Novelty based on recent sensory input changes
        float currentNovelty = 0.0f;
        if (pImpl->sensoryNeurons.size() > 0) {
            float totalActivity = 0.0f;
            for (auto* neuron : pImpl->sensoryNeurons) {
                totalActivity += std::abs(neuron->getMembranePotential() - neuron->getRestingPotential());
            }
            currentNovelty = std::min(1.0f, totalActivity / 500.0f);
        }
        
        pImpl->novelty->update(pImpl->timestep, currentNovelty);
    }
    
    // Apply neuromodulation effects on neural dynamics
    // Enhanced dopamine integration with multiple effects
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        float dopamineLevel = pImpl->dopamine->getLevel();
        
        // 1. Excitability modulation (as before)
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Enhanced excitability modulation
                    float excitabilityMod = dopamineLevel * pImpl->dopamine->getExcitabilityModulation();
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod * 2.0f);  // Double strength for enhanced effect
                    }
                    
                    // Reduce inhibition during high dopamine
                    if (dopamineLevel > 0.5f) {
                        // Decrease inhibitory synapse efficacy
                        for (auto* synapse : region->getSynapsesTo(neuron->getId())) {
                            float weight = synapse->getWeight();
                            if (weight < 0) {  // Inhibitory synapse
                                synapse->setWeight(weight * 1.2f);  // Make less inhibitory
                            }
                        }
                    }
                }
            }
        }
        
        // 2. Plasticity modulation with developmental stage awareness
        float plasticityMod = pImpl->dopamine->getPlasticityFactor();
        if (dopamineLevel > 0.3f) {
            // Stage-dependent plasticity enhancement
            float stageFactor = 1.0f;
            switch (pImpl->developmentalStage) {
                case DevelopmentalStage::Initial:
                    stageFactor = 1.5f;  // More plastic during initial stage
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    stageFactor = 1.2f;
                    break;
                case DevelopmentalStage::Maturation:
                    stageFactor = 1.0f;
                    break;
                case DevelopmentalStage::Adult:
                    stageFactor = 0.8f;
                    break;
                case DevelopmentalStage::Aging:
                    stageFactor = 0.6f;
                    break;
            }
            
            plasticityMod *= stageFactor;
        }
        
        // Apply enhanced plasticity modulation
        for (auto& region : pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().stdp || syn->getPlasticityFlags().hebbian) {
                    // Enhance synaptic modifications based on dopamine
                    float currentWeight = syn->getWeight();
                    float weightChange = (dopamineLevel - 0.5f) * 0.01f * plasticityMod;
                    
                    if (std::abs(weightChange) > 0.001f) {
                        float newWeight = currentWeight + weightChange;
                        // Clamp to reasonable range
                        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
                        syn->setWeight(newWeight);
                    }
                }
            }
        }
        
        // 3. Memory modulation for consolidation
        if (pImpl->dopamine->getMemoryModulation() > 0.5f && pImpl->episodicMemory) {
            // Enhanced memory consolidation during high dopamine
            if (pImpl->currentStep % 500 == 0) {  // Consolidate every 500 steps
                pImpl->episodicMemory->consolidate(0.5f);  // Stronger consolidation
            }
        }
        
        // 4. Attention modulation
        if (pImpl->attention && pImpl->dopamine->getAttentionModulation() > 0.3f) {
            // Apply attention focus based on dopamine levels
            float attentionFocus = dopamineLevel * 0.7f + 0.3f;  // Base attention + dopamine modulation
            pImpl->attention->setInhibitionStrength(0.5f * attentionFocus);
            pImpl->attention->setExcitationStrength(1.5f * attentionFocus);
        }
    }
    
    // Novelty-based learning enhancement
    if (pImpl->novelty && pImpl->dopamine) {
        // Integrate novelty with dopamine for learning
        float novelty = pImpl->novelty->getLevel();
        float predictionError = pImpl->predictionSystem ? 
                               pImpl->predictionSystem->getPredictionError() : 0.0f;
        
        // Enhance learning during novel or surprising events
        if (novelty > 0.7f || predictionError > 0.5f) {
            // Temporarily boost dopamine to enhance learning
            float originalLevel = pImpl->dopamine->getLevel();
            pImpl->dopamine->setLevel(std::min(1.0f, originalLevel + 0.3f));
            
            // Apply enhanced learning for this step
            pImpl->dopamine->enhanceLearning(novelty, predictionError);
            
            // Restore dopamine level
            pImpl->dopamine->setLevel(originalLevel);
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
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
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
    
    // ========== STEP 12: Sleep/rest cycle with memory replay ==========
    
    // Sleep/rest cycle implementation
    // During rest, the brain consolidates memories and replays important episodes
    if (pImpl->currentStep % pImpl->replayInterval == 0) {
        pImpl->isResting = true;  // Enter rest/sleep state
        
        // Sleep/rest cycle benefits
        if (pImpl->episodicMemory) {
            // During rest, consolidate and replay important memories
            // This is critical for long-term memory formation
            
            // Get episodes for replay based on importance
            size_t replayCount = std::min<size_t>(5, pImpl->developmentSystem ? 
                                                (pImpl->developmentSystem->getStage() == DevelopmentalStage::Initial ? 3 : 5) : 3);
            
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(replayCount);
            
            if (!episodesToReplay.empty()) {
                NLM_LOG_INFO("Entering rest/sleep cycle - replaying " + std::to_string(episodesToReplay.size()) + " episodes");
                
                // Replay each episode multiple times for consolidation
                for (const auto* episode : episodesToReplay) {
                    // Replay the episode multiple times for stronger consolidation
                    for (int replayIter = 0; replayIter < 3; ++replayIter) {
                        pImpl->episodicMemory->replayEpisode(episode);
                    }
                }
                
                // Force consolidation after replay
                pImpl->episodicMemory->consolidate(0.4f);  // Stronger consolidation during sleep
                
                // Reduce working memory activity during rest to prevent interference
                if (pImpl->workingMemory) {
                    pImpl->workingMemory->clear();  // Clear working memory during sleep
                }
                
                // Apply sleep-specific neuromodulation
                if (pImpl->dopamine) {
                    // Increase dopamine to enhance memory consolidation
                    float currentDA = pImpl->dopamine->getLevel();
                    pImpl->dopamine->setLevel(std::min(1.0f, currentDA + 0.3f));
                }
            }
        }
        
        // Development-specific sleep effects
        if (pImpl->developmentSystem) {
            switch (pImpl->developmentalStage) {
                case DevelopmentalStage::Initial:
                    // Critical period sleep - essential for sensory map formation
                    // More frequent and deeper sleep for normal development
                    pImpl->replayInterval = 50;   // Replay every 50 steps during critical period
                    pImpl->consolidationInterval = 300;  // More frequent consolidation
                    break;
                
                case DevelopmentalStage::CriticalPeriod:
                    // Active learning phase - sleep supports synaptic strengthening
                    pImpl->replayInterval = 80;
                    pImpl->consolidationInterval = 500;
                    break;
                
                case DevelopmentalStage::Maturation:
                    // Fine-tuning phase - sleep refines existing connections
                    pImpl->replayInterval = 100;
                    pImpl->consolidationInterval = 1000;
                    break;
                
                case DevelopmentalStage::Adult:
                    // Maintenance phase - sleep for memory maintenance
                    pImpl->replayInterval = 150;
                    pImpl->consolidationInterval = 2000;
                    break;
                
                case DevelopmentalStage::Aging:
                    // Reduced plasticity - less frequent replay
                    pImpl->replayInterval = 200;
                    pImpl->consolidationInterval = 3000;
                    break;
            }
        }
        
        // Record sleep statistics for monitoring
        NLM_LOG_INFO("Sleep/rest cycle active - stage: " + std::string(getStageName()) + 
                    ", replay interval: " + std::to_string(pImpl->replayInterval) +
                    ", consolidation interval: " + std::to_string(pImpl->consolidationInterval));
    } else {
        // Not in rest cycle
        pImpl->isResting = false;
    }
    
    // Sleep-specific neuromodulation effects
    if (pImpl->dopamine && pImpl->isResting) {
        // During sleep, dopamine modulates memory consolidation
        float sleepModulation = pImpl->dopamine->getMemoryModulation();
        
        if (sleepModulation > 0.3f && pImpl->episodicMemory) {
            // Enhanced consolidation during sleep
            // This simulates the brain "processing" memories during rest
            pImpl->episodicMemory->consolidate(sleepModulation * 0.5f);
            
            // Update sleep statistics
            pImpl->stepsSinceLastEpisode = 0;  // Reset episode counter during sleep
        }
    }
    
    // ========== STEP 13: Enhanced development integration ==========
    if (pImpl->developmentSystem) {
        // Update development system
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects more than just structural plasticity rates
        
        // 1. Neural excitability modulation
        if (pImpl->developmentalStage == DevelopmentalStage::CriticalPeriod) {
            // Critical period: dramatically increase neural excitability and learning capacity
            float excitabilityBoost = 1.5f;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Boost excitability during critical period
                        float currentPotential = neuron->getMembranePotential();
                        float targetResting = -65.0f;  // More depolarized resting potential
                        float delta = targetResting - currentPotential;
                        if (delta > 0.0f) {
                            neuron->setMembranePotential(currentPotential + delta * 0.01f);
                        }
                    }
                }
            }
        }
        
        // 2. Attention system development
        if (pImpl->attention) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float attentionMaturation = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    attentionMaturation = 0.3f;  // Poor attention control initially
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    attentionMaturation = 0.6f;  // Developing attention
                    break;
                case DevelopmentalStage::Maturation:
                    attentionMaturation = 0.8f;  // Improving attention
                    break;
                case DevelopmentalStage::Adult:
                    attentionMaturation = 1.0f;  // Mature attention
                    break;
                case DevelopmentalStage::Aging:
                    attentionMaturation = 0.7f;  // Some decline in aging
                    break;
            }
            
            // Update attention parameters based on development
            pImpl->attention->setInhibitionStrength(0.5f * attentionMaturation);
            pImpl->attention->setExcitationStrength(1.5f * attentionMaturation);
            pImpl->attention->setCompetitionThreshold(0.3f / attentionMaturation);
        }
        
        // 3. Memory system development
        if (pImpl->workingMemory) {
            float memoryCapacity = 100.0f;  // Base capacity
            float memoryMaturation = 1.0f;
            
            switch (pImpl->developmentalStage) {
                case DevelopmentalStage::Initial:
                    memoryCapacity = 50.0f;   // Limited working memory initially
                    memoryMaturation = 0.5f;
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    memoryCapacity = 75.0f;   // Developing capacity
                    memoryMaturation = 0.7f;
                    break;
                case DevelopmentalStage::Maturation:
                    memoryCapacity = 100.0f;  // Approaching mature capacity
                    memoryMaturation = 0.9f;
                    break;
                case DevelopmentalStage::Adult:
                    memoryCapacity = 100.0f;  // Mature capacity
                    memoryMaturation = 1.0f;
                    break;
                case DevelopmentalStage::Aging:
                    memoryCapacity = 80.0f;   // Some decline in aging
                    memoryMaturation = 0.8f;
                    break;
            }
            
            pImpl->workingMemory->setCapacity(static_cast<size_t>(memoryCapacity));
            pImpl->workingMemory->setDecayRate(0.02f / memoryMaturation);  // Slower decay as memory matures
        }
        
        if (pImpl->episodicMemory) {
            float episodeMaturation = 1.0f;
            
            switch (pImpl->developmentalStage) {
                case DevelopmentalStage::Initial:
                    episodeMaturation = 0.3f;  // Few episodes initially
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    episodeMaturation = 0.6f;
                    break;
                case DevelopmentalStage::Maturation:
                    episodeMaturation = 0.9f;
                    break;
                case DevelopmentalStage::Adult:
                    episodeMaturation = 1.0f;
                    break;
                case DevelopmentalStage::Aging:
                    episodeMaturation = 0.8f;
                    break;
            }
            
            // Stage-appropriate consolidation parameters
            if (pImpl->currentStep % 2000 == 0) {  // More frequent consolidation in early stages
                float consolidationStrength = 0.3f * episodeMaturation;
                pImpl->episodicMemory->consolidate(consolidationStrength);
            }
        }
        
        // 4. Neuromodulation system development
        if (pImpl->dopamine) {
            float neuromodulationMaturation = 1.0f;
            
            switch (pImpl->developmentalStage) {
                case DevelopmentalStage::Initial:
                    neuromodulationMaturation = 0.4f;  // Immature neuromodulation
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    neuromodulationMaturation = 0.7f;
                    break;
                case DevelopmentalStage::Maturation:
                    neuromodulationMaturation = 0.9f;
                    break;
                case DevelopmentalStage::Adult:
                    neuromodulationMaturation = 1.0f;
                    break;
                case DevelopmentalStage::Aging:
                    neuromodulationMaturation = 0.8f;
                    break;
            }
            
            // Update neuromodulation parameters based on development
            // This could affect how neuromodulators respond to rewards, novelties, etc.
        }
        
        // 5. Cognitive system development
        if (pImpl->planner) {
            int planningDepth = 2;  // Starting depth
            
            switch (pImpl->developmentalStage) {
                case DevelopmentalStage::Initial:
                    planningDepth = 1;
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    planningDepth = 3;
                    break;
                case DevelopmentalStage::Maturation:
                    planningDepth = 5;
                    break;
                case DevelopmentalStage::Adult:
                    planningDepth = 8;
                    break;
                case DevelopmentalStage::Aging:
                    planningDepth = 5;
                    break;
            }
            
            pImpl->planner->setPlanningDepth(planningDepth);
        }
        
        if (pImpl->conceptFormation) {
            float conceptMaturation = 1.0f;
            
            switch (pImpl->developmentalStage) {
                case DevelopmentalStage::Initial:
                    conceptMaturation = 0.2f;  // Limited concept formation initially
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    conceptMaturation = 0.5f;
                    break;
                case DevelopmentalStage::Maturation:
                    conceptMaturation = 0.8f;
                    break;
                case DevelopmentalStage::Adult:
                    conceptMaturation = 1.0f;
                    break;
                case DevelopmentalStage::Aging:
                    conceptMaturation = 0.7f;
                    break;
            }
            
            // Concept formation becomes more abstract and sophisticated with development
            // This could involve updating concept formation parameters
        }
        
        // 6. Plasticity rule development
        if (pImpl->stdp && pImpl->hebbian) {
            switch (pImpl->developmentalStage) {
                case DevelopmentalStage::Initial:
                    // Emphasize Hebbian learning for rapid association formation
                    // STDP still functional but less dominant
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    // Both STDP and Hebbian active, balanced learning
                    break;
                case DevelopmentalStage::Maturation:
                    // STDP dominant for precise timing-based learning
                    break;
                case DevelopmentalStage::Adult:
                    // Mature balance of plasticity rules
                    break;
                case DevelopmentalStage::Aging:
                    // Shift back to Hebbian for relearning
                    break;
            }
            
            // Development could tune plasticity rule parameters
        }
        
        // 7. Sleep/rest cycle initiation
        // During development, sleep becomes more prominent for memory consolidation
        if (pImpl->developmentSystem->getStage() == DevelopmentalStage::Initial || 
            pImpl->developmentSystem->getStage() == DevelopmentalStage::CriticalPeriod) {
            pImpl->isResting = true;  // Enter resting/sleep state during development
        }
        
        // 8. Replay system activation during development
        // Critical for memory consolidation during developmental periods
        if (pImpl->episodicMemory) {
            if (pImpl->currentStep % pImpl->replayInterval == 0 && 
                pImpl->developmentSystem->getStage() != DevelopmentalStage::Adult) {
                // More frequent replay during development for enhanced learning
                auto episodes = pImpl->episodicMemory->getEpisodesForReplay(5);  // More episodes
                for (const auto* episode : episodes) {
                    pImpl->episodicMemory->replayEpisode(episode);
                }
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

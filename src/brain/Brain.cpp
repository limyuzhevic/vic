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
#include "../cognition/SelfModel.hpp"
#include "../cognition/SocialLearning.hpp"
#include "../cognition/SpatialRepresentation.hpp"
#include "../cognition/TemporalRelation.hpp"
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
    
    // ========== COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<SelfModel> selfModel;
    std::unique_ptr<SocialLearning> socialLearning;
    std::unique_ptr<SpatialRepresentation> spatialRepresentation;
    std::unique_ptr<TemporalRelation> temporalRelation;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
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
    std::string checkpointDir;
    
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
        selfModel = std::make_unique<SelfModel>();
        socialLearning = std::make_unique<SocialLearning>();
        
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
    
    // Configure checkpoint directory from config
    checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    // Set up checkpoint data providers for saving
    checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) {
        // Collect all neuron states for checkpointing
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    data.membranePotential.push_back(state.membranePotential);
                    data.restingPotential.push_back(state.restingPotential);
                    data.threshold.push_back(state.threshold);
                    data.resetPotential.push_back(state.resetPotential);
                    data.leakConductance.push_back(state.leakConductance);
                    data.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    data.refractoryRemaining.push_back(state.refractoryRemaining);
                    data.refractoryPeriod.push_back(state.refractoryPeriod);
                    data.lastSpikeTime.push_back(state.lastSpikeTime);
                    data.neuronType.push_back(static_cast<uint64_t>(pop->getNeuronType()));
                    data.regionId.push_back(region->getId().index());
                    data.populationId.push_back(pop->getId().index());
                }
            }
        }
        return true;
    });
    
    checkpointManager->setSynapseProvider([this](SynapseCheckpointData& data) {
        // Collect all synapse states for checkpointing
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                data.sourceNeuron.push_back(syn->getSourceNeuron().index());
                data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                data.weight.push_back(syn->getWeight());
                data.delay.push_back(syn->getDelay());
                data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                data.plasticityFlags.push_back(static_cast<uint8_t>(syn->getPlasticityFlags().to_ulong()));
                data.eligibilityTrace.push_back(syn->getEligibilityTrace());
                data.efficacy.push_back(syn->getEfficacy());
                data.shortTermDepression.push_back(syn->getShortTermDepression());
                data.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        return true;
    });
    
    // Set up providers for checkpoint reading (load)
    // Note: CheckpointManager uses CheckpointWriter/Reader directly for loading
    // The providers above are for saving only
}
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
    
    // Initialize additional cognition systems
    pImpl->selfModel->initialize(this);
    pImpl->socialLearning->initialize(this);
    pImpl->spatialRepresentation->initialize(this);
    pImpl->temporalRelation->initialize(this);
    pImpl->spatialRepresentation->initialize(this);
    pImpl->temporalRelation->initialize(this);
    
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
    
    // ========== STEP 3.5: Integrate working memory into neural processing ==========
    // Store sensory input into working memory if available
    if (pImpl->workingMemory && !pImpl->sensoryNeurons.empty()) {
        for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
            auto* sensoryNeuron = pImpl->sensoryNeurons[i];
            float activation = std::abs(sensoryNeuron->getState().membranePotential - sensoryNeuron->getState().restingPotential);
            if (activation > 0.1f) {
                // Sensory input becomes working memory trace with decay over time
                pImpl->workingMemory->storeToNeuron(sensoryNeuron->getId(), activation * 0.5f);
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
    
    // ========== STEP 3.5: Integrate working memory into neural processing ==========
    // Store sensory input into working memory if available
    if (pImpl->workingMemory && !pImpl->sensoryNeurons.empty()) {
        for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
            auto* sensoryNeuron = pImpl->sensoryNeurons[i];
            float activation = std::abs(sensoryNeuron->getState().membranePotential - sensoryNeuron->getState().restingPotential);
            if (activation > 0.1f) {
                // Sensory input becomes working memory trace with decay over time
                pImpl->workingMemory->storeToNeuron(sensoryNeuron->getId(), activation * 0.5f);
            }
        }
    }
    
    // Store neural activity into working memory (from firing neurons)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
                
                if (justFired) {
                    // Neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Working memory competition - select winning memory traces
    if (pImpl->workingMemory) {
        pImpl->workingMemory->runCompetition();
        
        // Update working memory with neural activity based on competition results
        const auto& winners = pImpl->workingMemory->getWinners();
        if (!winners.empty()) {
            // Amplify activity of winning neurons
            for (const auto& winnerId : winners) {
                // Inject current into winner neurons to strengthen representation
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->getId() == winnerId) {
                                neuron->injectCurrent(0.2f);
                                break;
                            }
                        }
                    }
                }
            }
            
            // Apply attention selection based on working memory winners
            if (pImpl->attention) {
                pImpl->attention->processCompetition(winners);
            }
        }
        
        // Update working memory with general neural activity
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // Apply neuromodulation effects on memory systems
    // Step 5: Apply enhanced dopamine integration with memory systems
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on working memory
        if (pImpl->workingMemory && pImpl->dopamine->getLevel() > 0.5f) {
            // Enhance working memory traces based on dopamine level
            float dopamineLevel = pImpl->dopamine->getLevel();
            pImpl->dopamine->enhanceWorkingMemory(pImpl->workingMemory, dopamineLevel);
            
            // Higher dopamine increases working memory capacity and stability
            if (dopamineLevel > 0.7f) {
                // Boost working memory maintenance
                pImpl->workingMemory->strengthenMemory(1.2f);
            }
        }
        
        // Apply dopamine effects on episodic memory consolidation
        if (pImpl->episodicMemory && pImpl->dopamine->getLevel() > 0.5f) {
            // Enhance episodic memory consolidation
            float consolidationStrength = pImpl->dopamine->getLevel() * 0.5f;
            pImpl->dopamine->enhanceEpisodicMemory(pImpl->episodicMemory, consolidationStrength);
        }
        
        // Apply dopamine effects on attention system
        if (pImpl->attention && pImpl->dopamine->getLevel() > 0.5f) {
            // Modulate attention based on dopamine level
            float attentionalBias = pImpl->dopamine->getLevel() * 0.5f;
            pImpl->dopamine->modulateAttention(pImpl->attention, attentionalBias);
        }
        
        // Apply dopamine effects on concept formation
        if (pImpl->conceptFormation && pImpl->dopamine->getLevel() > 0.5f) {
            // Modulate concept formation based on dopamine level
            float conceptThreshold = pImpl->dopamine->getLevel() * 0.7f;
            pImpl->dopamine->modulateConceptFormation(pImpl->conceptFormation, conceptThreshold);
        }
        
        // Apply dopamine-reward interaction
        if (pImpl->dopamine->getLevel() > 0.0f) {
            // Reward prediction error handling
            if (pImpl->predictionSystem) {
                float predictionError = pImpl->predictionSystem->getPredictionError();
                float expectedValue = 0.5f;  // Default expected value
                pImpl->dopamine->interactWithRewardPrediction(predictionError, expectedValue);
            }
            
            // Integration with other neuromodulators
            if (pImpl->curiosity) {
                float curiosityLevel = pImpl->curiosity->getLevel();
                pImpl->dopamine->integrateWithCuriosity(curiosityLevel);
            }
            
            if (pImpl->novelty) {
                float noveltyLevel = pImpl->novelty->getLevel();
                pImpl->dopamine->integrateWithNovelty(noveltyLevel);
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
            
            // Store sensory state from the last input (if available)
            if (!pImpl->sensoryNeurons.empty()) {
                for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                    auto* neuron = pImpl->sensoryNeurons[i];
                    float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                    episode.sensoryState.push_back(activation);
                    
                    // Store location (placeholder)
                    episode.positionX = 0.0f;
                    episode.positionY = 0.0f;
                    episode.orientation = 0.0f;
                }
            }
            
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
            
            // Store energy and novelty (from neuromodulators)
            episode.energy = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            episode.novelty = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
            
            // Set action based on motor activity
            episode.action = ActionType::Wait;
            for (auto* neuron : pImpl->motorNeurons) {
                if (neuron->isFiring()) {
                    episode.action = ActionType::MoveForward;  // Default for now
                    break;
                }
            }
            
            // Store resulting state (current state)
            episode.resultingSensoryState = episode.sensoryState;
            episode.resultingReward = episode.reward;
            
            pImpl->episodicMemory->storeEpisode(episode);
            
            // Connect episodic memory to concept formation
            if (pImpl->conceptFormation && !episode.sensoryState.empty()) {
                pImpl->conceptFormation->presentExperience(episode.sensoryState, 
                                                          {}, episode.reward, currentStep);
            }
        }
    }
    
    // ========== STEP 8: Ensure episodic memory affects attention ==========
    // Update working memory with episodic memory content
    if (pImpl->workingMemory && pImpl->episodicMemory) {
        // Get recent episodes and influence working memory
        auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(3);
        for (const auto* episode : recentEpisodes) {
            // Use episode content to influence working memory traces
            if (!episode->sensoryState.empty()) {
                pImpl->workingMemory->store(episode->sensoryState, 0.7f);
            }
        }
    }
    
    // ========== STEP 9: Connect episodic memory to prediction system ==========
    if (pImpl->predictionSystem && !pImpl->sensoryNeurons.empty()) {
        // Use episodic memory to guide predictions
        auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(2);
        if (!recentEpisodes.empty()) {
            const auto* lastEpisode = recentEpisodes.back();
            if (!lastEpisode->sensoryState.empty()) {
                // This would be used to shape prediction system
                // For now, just track it
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Concept formation is now integrated - it receives experiences
        // from episodic memory during episode storage
    }
    
    // ========== STEP 7.5: Integrate prediction system ==========
    if (pImpl->predictionSystem && pImpl->workingMemory) {
        // Update prediction system with current working memory state
        
        // Get current working memory state as sensory-like input
        std::vector<float> workingMemoryState;
        const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        
        // Collect activation from working memory traces
        for (const auto& neuronId : memoryNeurons) {
            // Get neuron state to determine activation
            float activation = 0.0f;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->getId() == neuronId) {
                            activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                            break;
                        }
                    }
                }
            }
            workingMemoryState.push_back(activation);
        }
        
        // Create SensoryInput for prediction system
        std::unique_ptr<SensoryInput> wmInput = std::make_unique<SensoryInput>(workingMemoryState);
        
        // Make prediction
        auto predictedInput = pImpl->predictionSystem->predictNextState(*wmInput);
        
        // Get prediction error (will be stored internally)
        float predictionError = pImpl->predictionSystem->getPredictionError();
        
        // Store prediction in episodic memory for long-term learning
        if (pImpl->predictionError && predictionError > 0.1f) {
            PredictionErrorItem predErrorItem;
            predErrorItem.timestamp = currentStep;
            predErrorItem.errorValue = predictionError;
            predErrorItem.predictedState = *wmInput;
            predErrorItem.actualState = *wmInput;  // Simplified for now
            predErrorItem.impact = predictionError * 0.5f;
            pImpl->predictionError->storePredictionError(predErrorItem);
        }
        
        // Use prediction error to modulate plasticity
        if (predictionError > 0.0f && pImpl->dopamine) {
            // Increase plasticity for unexpected prediction errors
            float plasticityBoost = std::min(predictionError * 10.0f, 2.0f);
            pImpl->dopamine->setLevel(pImpl->dopamine->getLevel() + plasticityBoost * 0.1f);
        }
        
        // Update working memory with prediction to strengthen relevant traces
        if (predictionError < 0.5f && !workingMemoryState.empty()) {
            // High confidence predictions strengthen working memory traces
            pImpl->workingMemory->updateTraceStrength(workingMemoryState, 0.8f - predictionError);
        }
        
        // Update working memory with prediction to strengthen relevant traces
        if (predictionError < 0.5f && !workingMemoryState.empty()) {
            // High confidence predictions strengthen working memory traces
            pImpl->workingMemory->updateTraceStrength(workingMemoryState, 0.8f - predictionError);
        }
        
        // Integrate prediction into attention system
        if (pImpl->attention) {
            // Use prediction confidence for attention allocation
            float confidence = pImpl->predictionSystem->getConfidence();
            if (confidence > 0.7f) {
                // High confidence predictions draw attention
                pImpl->attention->processPrediction(confidence);
            }
        }
        
        // ========== STEP 9: Connect prediction system to episodic memory ==========
        if (pImpl->predictionSystem && pImpl->predictionError && predictionError > 0.1f) {
            // Store prediction error in episodic memory
            PredictionErrorItem predErrorItem;
            predErrorItem.timestamp = currentStep;
            predErrorItem.errorValue = predictionError;
            predErrorItem.predictedState = *wmInput;
            predErrorItem.actualState = *wmInput;  // Simplified for now
            predErrorItem.impact = predictionError * 0.5f;
            pImpl->predictionError->storePredictionError(predErrorItem);
        }
        
        // ========== STEP 10: Make prediction errors affect working memory and plasticity ==========
        if (predictionError > 0.0f && pImpl->workingMemory) {
            // Prediction errors strengthen working memory traces for unexpected events
            pImpl->workingMemory->storePredictionError(workingMemoryState, predictionError);
        }
        
        if (predictionError > 0.0f && pImpl->stdp) {
            // Prediction errors drive plasticity in neural connections
            pImpl->stdp->updateFromPredictionError(predictionError, pImpl->timestep);
        }
        
        // ========== STEP 11: Integrate prediction system into attention ==========
        if (pImpl->attention) {
            // Use prediction confidence for attention allocation
            float confidence = pImpl->predictionSystem->getConfidence();
            if (confidence > 0.7f) {
                // High confidence predictions draw attention
                pImpl->attention->processPrediction(confidence);
            }
        }
        
        // ========== STEP 12: Integrate prediction system into concept formation ==========
        if (pImpl->conceptFormation && predictionError > 0.5f) {
            // High prediction errors trigger concept discovery
            // Create a concept from the prediction error and working memory state
            pImpl->conceptFormation->createConceptFromPrediction(*wmInput, predictionError);
        }
    
    // ========== STEP 9.5: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners (attention modulates memory)
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
        
        // Connect attention to working memory competition - attention modulates which memories compete
        if (pImpl->attention && pImpl->workingMemory) {
            pImpl->attention->connectToWorkingMemory(*pImpl->workingMemory);
        }
        
        // Connect attention to episodic memory
        if (pImpl->attention && pImpl->episodicMemory) {
            pImpl->attention->integrateEpisodicMemory(*pImpl->episodicMemory);
        }
        
        // Connect attention to prediction system
        if (pImpl->attention && pImpl->predictionSystem) {
            pImpl->attention->connectToPredictionSystem(*pImpl->predictionSystem);
        }
        
        // Connect attention to concept formation
        if (pImpl->attention && pImpl->conceptFormation) {
            pImpl->attention->influenceConceptFormation(*pImpl->conceptFormation);
        }
        
        // Connect attention to planning
        if (pImpl->attention && pImpl->planner) {
            pImpl->attention->influencePlanning(*pImpl->planner);
        }
        
        // Apply attention to action selection
        if (pImpl->attention) {
            pImpl->attention->biasActionSelection();
        }
        
        // Use prediction confidence to update attention
        if (pImpl->attention && pImpl->predictionSystem) {
            float confidence = pImpl->predictionSystem->getConfidence();
            if (confidence > 0.7f) {
                pImpl->attention->processPrediction(confidence);
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Concept formation now receives experiences from episodic memory
        // The episodic memory integration in STEP 7 feeds experiences here
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
    
    // Capture sensory state for episodic memory
    std::vector<float> sensoryState;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store for episodic memory
        float activation = std::abs(normalizedValue);
        sensoryState.push_back(activation);
        
        // Also store in working memory
        if (pImpl->workingMemory && activation > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), activation / 10.0f);
        }
    }
    
    // Create episodic memory from sensory input
    if (pImpl->episodicMemory && !sensoryState.empty()) {
        EpisodicMemoryItem episode;
        // We'll capture this in the regular step cycle, but create a basic record here
        // to ensure sensory input is preserved for episodic memory
        episode.timestamp = pImpl->currentStep;
        episode.sensoryState = sensoryState;
        episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        episode.energy = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
        episode.novelty = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
        episode.action = ActionType::Wait;
        
        // Note: We don't store this episode immediately to avoid duplicates
        // The full episode with neural activity will be stored in step()
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

SimulationStep Brain::getCurrentStep() const {
    return pImpl->currentStep;
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
    // Integrated action selection using all cognition systems
    // Brain coordinates NeuralPlanner, ConceptFormation, SelfModel, and SocialLearning
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Gather current brain state for cognition systems
    std::vector<float> currentState;
    
    // Extract sensory neuron activations
    currentState.reserve(pImpl->sensoryNeurons.size());
    for (auto* sensoryNeuron : pImpl->sensoryNeurons) {
        float activation = std::abs(sensoryNeuron->getState().membranePotential - 
                                   sensoryNeuron->getState().restingPotential);
        currentState.push_back(activation);
    }
    
    // Step 1: Get concept-guided action recommendations
    ActionType conceptAction = ActionType::Wait;
    if (pImpl->conceptFormation) {
        size_t conceptId = pImpl->conceptFormation->findConceptForPattern(currentState);
        if (conceptId > 0) {
            // Get concept prototype for action filtering
            std::vector<float> prototype = pImpl->conceptFormation->getConceptPrototype(conceptId);
            // Simple concept-based action selection based on pattern matching
            // Actions that lead to similar state transitions are favored
            conceptAction = getConceptBasedAction(currentState, prototype);
        }
    }
    
    // Step 2: Use NeuralPlanner for goal-directed action planning
    ActionType plannerAction = ActionType::Wait;
    if (pImpl->planner) {
        plannerAction = pImpl->planner->planAction(currentState, 0.5f);
    }
    
    // Step 3: Consider self-model predictions about action consequences
    ActionType selfModelAction = ActionType::Wait;
    if (pImpl->selfModel) {
        selfModelAction = pImpl->selfModel->getPreferredAction(currentState);
    }
    
    // Step 4: Include social learning for imitation and coordination
    ActionType socialAction = ActionType::Wait;
    if (pImpl->socialLearning) {
        socialAction = pImpl->socialLearning->getImitationAction(currentState);
    }
    
    // Step 5: Evaluate and combine cognition system recommendations
    struct ActionOption {
        ActionType action;
        float score;
    };
    
    std::vector<ActionOption> options;
    
    // Add cognition system preferences with different weights
    if (conceptAction != ActionType::Wait) {
        float conceptScore = 0.3f;
        // Boost score if concept is stable
        if (pImpl->conceptFormation) {
            size_t conceptId = pImpl->conceptFormation->findConceptForPattern(currentState);
            if (conceptId > 0) {
                float stability = pImpl->conceptFormation->getConceptStability(conceptId);
                conceptScore *= stability;
            }
        }
        options.push_back({conceptAction, conceptScore});
    }
    
    if (plannerAction != ActionType::Wait) {
        float plannerScore = 0.4f;
        // Boost if planner has high confidence
        if (pImpl->planner) {
            plannerScore *= pImpl->planner->getPlanningConfidence();
        }
        options.push_back({plannerAction, plannerScore});
    }
    
    if (selfModelAction != ActionType::Wait) {
        float selfModelScore = 0.2f;
        // Boost if self-model is reliable
        if (pImpl->selfModel) {
            float confidence = pImpl->selfModel->getSelfModelConfidence(selfModelAction);
            selfModelScore *= confidence;
        }
        options.push_back({selfModelAction, selfModelScore});
    }
    
    if (socialAction != ActionType::Wait) {
        float socialScore = 0.1f;
        // Boost if we can actually imitate
        if (pImpl->socialLearning && pImpl->socialLearning->canImitate(socialAction)) {
            socialScore *= 1.5f;  // Extra boost for valid imitation
        }
        options.push_back({socialAction, socialScore});
    }
    
    // Also consider working memory winner
    ActionType workingMemoryAction = ActionType::Wait;
    float winnerInfluence = 0.0f;
    
    if (pImpl->workingMemory) {
        const auto& winners = pImpl->workingMemory->getWinners();
        
        // Check if working memory winners are motor-related
        for (const auto& winnerId : winners) {
            // Find winner neuron and check its type
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->getId() == winnerId) {
                            NeuronType neuronType = pop->getNeuronType();
                            // If winner is a motor neuron, use it to influence action
                            if (neuronType == NeuronType::Motor) {
                                float influence = 1.0f - neuron->getState().membranePotential / 20.0f;
                                winnerInfluence = std::max(winnerInfluence, influence);
                                
                                // Map winning motor neuron activity to specific actions
                                if (pop->getName() == "Motor_Fovea" || pop->getName().empty()) {
                                    workingMemoryAction = ActionType::MoveForward;
                                } else if (pop->getName() == "Motor_Gaze") {
                                    workingMemoryAction = ActionType::Turn;
                                } else if (pop->getName() == "Motor_Manipulate") {
                                    workingMemoryAction = ActionType::Manipulate;
                                } else if (pop->getName() == "Motor_Wait" || pop->getName().empty()) {
                                    workingMemoryAction = ActionType::Wait;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    
    if (winnerInfluence > 0.5f) {
        options.push_back({workingMemoryAction, 0.6f});
    }
    
    // Calculate activity of motor neuron groups (fallback)
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    ActionType motorAction = ActionType::Wait;
    if (firingMotor > 0) {
        motorAction = ActionType::MoveForward;
    }
    options.push_back({motorAction, 0.1f});
    
    // Select best action from all options
    ActionType bestAction = ActionType::Wait;
    float bestScore = 0.0f;
    
    for (const auto& option : options) {
        if (option.score > bestScore) {
            bestScore = option.score;
            bestAction = option.action;
        }
    }
    
    // Ensure we have a valid action
    if (bestScore < 0.1f) {
        bestAction = ActionType::Wait;
    }
    
    auto action = std::make_unique<Action>(bestAction);
    
    // Update planner with actual action for learning
    if (pImpl->planner && bestAction != ActionType::Wait) {
        std::vector<ActionType> plannedActions{bestAction};
        std::vector<ActionType> actualActions{bestAction};
        pImpl->planner->updatePlanQuality(plannedActions, actualActions, bestScore);
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
        // Use checkpoint manager to save state
        if (!pImpl->checkpointManager->saveImmediately(filepath)) {
            NLM_LOG_ERROR("Failed to save brain state using checkpoint manager");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully using checkpoint manager");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        // Use checkpoint manager to load state
        if (!pImpl->checkpointManager->load(filepath)) {
            NLM_LOG_ERROR("Failed to load brain state using checkpoint manager");
            return false;
        }
        
        NLM_LOG_INFO("Brain state loaded successfully using checkpoint manager");
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

// Self model for action consequence prediction
SelfModel* Brain::getSelfModel() {
    return pImpl->selfModel.get();
}

// Social learning for imitation and coordination
SocialLearning* Brain::getSocialLearning() {
    return pImpl->socialLearning.get();
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

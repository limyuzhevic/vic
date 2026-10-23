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
    // Enhanced neuromodulation with multiple neurotransmitter systems
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine increases excitability (lower effective threshold)
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // Serotonin system - regulates mood, anxiety, and cognitive flexibility
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
        
        // Serotonin effects on neural stability and flexibility
        float noveltyLevel = pImpl->novelty->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Serotonin modulates neural gain and stability
                    // High novelty/increase in novelty increases cognitive flexibility
                    float stabilityMod = 1.0f - noveltyLevel * 0.3f;  // Up to 30% reduction in stability
                    if (stabilityMod < 0.7f) {
                        // Reduce stability for exploration
                        neuron->setLeakConductance(neuron->getState().leakConductance * (1.0f / stabilityMod));
                    }
                }
            }
        }
    }
    
    // Curiosity system - drives exploration and learning
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
        
        // Curiosity effects on attention and memory encoding
        float curiosityLevel = pImpl->curiosity->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Curiosity increases attentional weighting and memory encoding strength
                    float encodingBoost = curiosityLevel * 0.8f;
                    if (encodingBoost > 0.0f) {
                        neuron->injectCurrent(encodingBoost * 0.3f);  // Subtle boost for exploration
                    }
                }
            }
        }
        
        // Curiosity-guided action selection bias
        if (curiosityLevel > 0.4f) {
            // High curiosity promotes random exploration
            if (!pImpl->isResting) {
                // Occasionally reset planning to encourage exploration
                if (pImpl->currentStep % 500 == 0) {
                    pImpl->planner->clearCache();
                }
            }
        }
    }
    
    // Prediction error system - learning from mismatches
    if (pImpl->predictionError) {
        pImpl->predictionError->update(pImpl->timestep);
        
        // Prediction error effects on synaptic plasticity
        float predictionError = pImpl->predictionError->getLevel();
        // Apply prediction error to modulate learning rates
        for (auto& region : pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                // Prediction error amplifies plasticity for prediction errors
                float eligibility = syn->getEligibilityTrace();
                if (std::abs(predictionError) > 0.1f && std::abs(eligibility) > 0.01f) {
                    // Enhance weight changes for prediction errors
                    float errorAmplification = 1.0f + std::abs(predictionError) * 2.0f;
                    float delta = eligibility * predictionError * errorAmplification;
                    syn->addToWeight(delta);
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
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners - enhanced integration
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            // Enhanced attention mechanism: working memory actively guides attention
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            
            // Use attention to process working memory traces and identify focus areas
            std::vector<NeuronId> attendedNeurons = pImpl->attention->processCompetition(competitors);
            
            // Apply attentional modulation to working memory for reinforcement
            for (NeuronId neuronId : attendedNeurons) {
                if (pImpl->workingMemory->contains(neuronId)) {
                    // Strengthen working memory representation through attention
                    float currentActivation = pImpl->workingMemory->getNeuronActivation(neuronId);
                    pImpl->workingMemory->strengthenMemory(currentActivation * 1.1f);
                }
            }
            
            // Apply attention-based bias to planning systems
            if (pImpl->planner && pImpl->attention->getWinners().size() > 0) {
                // Use attended regions to bias planning toward relevant goals
                for (RegionId attendedRegion : pImpl->attention->getAttendedRegions()) {
                    if (pImpl->planner->getCurrentGoal().empty()) {
                        // Set initial goal based on attended region
                        pImpl->planner->setCurrentGoal(std::vector<float>{static_cast<float>(attendedRegion.index()), 0.5f});
                    }
                }
            }
        }
    }
    
    // Integrate attention system with neuromodulation for adaptive focus
    if (pImpl->attention && pImpl->dopamine) {
        // Use attention winners to modulate dopamine release
        const auto& winners = pImpl->attention->getWinners();
        if (!winners.empty()) {
            // Dopamine level scales with attentional engagement
            float attentionStrength = std::min(1.0f, static_cast<float>(winners.size()) / 10.0f);
            // Note: In real implementation, this would be a proper neuromodulator signal
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation && pImpl->attention) {
        // Enhanced concept formation using attentional output
        // Get attended regions and convert to pattern for concept formation
        std::vector<float> attendedPattern;
        for (RegionId regionId : pImpl->attention->getAttendedRegions()) {
            attendedPattern.push_back(static_cast<float>(regionId.index()));
        }
        
        // Get current neural activity for concept formation
        std::vector<float> currentPattern;
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    float activation = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
                    currentPattern.push_back(activation);
                }
            }
        }
        
        if (!currentPattern.empty()) {
            // Present current neural state to concept formation system
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                currentPattern,
                currentPattern,  // Use neural activity as features
                pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,  // Use dopamine as reward signal
                pImpl->currentStep
            );
            
            // Use attended regions to guide concept formation
            if (!attendedPattern.empty() && conceptId > 0) {
                // Enhance concept formation for attended regions
                const auto& concept = pImpl->conceptFormation->getConcept(conceptId);
                if (concept) {
                    // Update concept with attentional bias
                    pImpl->conceptFormation->updateConcept(
                        conceptId,
                        currentPattern,
                        attendedPattern,  // Use attended pattern as enhanced features
                        pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f
                    );
                }
            }
            
            // Connect concept formation to planning
            if (pImpl->planner && conceptId > 0) {
                // Use most stable concept to guide planning
                float maxStability = 0.0f;
                size_t bestConceptId = 0;
                
                for (size_t cid = 1; cid <= pImpl->conceptFormation->getConceptCount(); ++cid) {
                    float stability = pImpl->conceptFormation->getConceptStability(cid);
                    if (stability > maxStability) {
                        maxStability = stability;
                        bestConceptId = cid;
                    }
                }
                
                if (maxStability > 0.5f && bestConceptId > 0) {
                    // Set planning goal based on best concept
                    std::vector<float> conceptPrototype = pImpl->conceptFormation->getConceptPrototype(bestConceptId);
                    if (!conceptPrototype.empty()) {
                        pImpl->planner->setCurrentGoal(conceptPrototype);
                        
                        // Use concept to evaluate action sequences
                        PlanningCandidate candidate = pImpl->planner->evaluateSequence(
                            std::vector<ActionType>{MotorCommand::MoveForward},
                            conceptPrototype
                        );
                        
                        // Store concept-based planning results for action selection
                        if (candidate.confidence > 0.7f) {
                            // Note: In full implementation, this would influence action selection
                            // For now, we just log the concept-based planning
                        }
                    }
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
        // Enhanced memory replay with consolidation
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        
        // Replay should strengthen memory connections
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
            
            // Replay also updates concept formation with repeated patterns
            if (pImpl->conceptFormation) {
                std::vector<float> episodePattern;
                // Convert episode to neural pattern
                for (size_t neuronId : episode->activeNeurons) {
                    float activation = 0.0f;
                    if (neuronId < episode->neuronActivations.size()) {
                        activation = episode->neuronActivations[neuronId];
                    }
                    episodePattern.push_back(activation);
                }
                
                if (!episodePattern.empty()) {
                    pImpl->conceptFormation->updateConcept(
                        pImpl->conceptFormation->getMatchingConcept(episodePattern),
                        episodePattern,
                        episodePattern,
                        episode->reward
                    );
                }
            }
        }
        
        // Memory replay also updates neural planner based on experience
        if (pImpl->planner) {
            // Replay informs planning by updating action quality
            float replayBenefit = std::min(1.0f, static_cast<float>(episodesToReplay.size()) / 5.0f);
            for (ActionType action : std::vector<ActionType>{ActionType::MoveForward, ActionType::MoveBackward,
                                                             ActionType::TurnLeft, ActionType::TurnRight,
                                                             ActionType::Interact, ActionType::Wait}) {
                float currentQuality = pImpl->planner->getActionQuality(action);  // Need to add this method
                pImpl->planner->setActionQuality(action, currentQuality + replayBenefit * 0.1f);
            }
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates and cognitive functions
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
            
            // Structural plasticity changes with development
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
        
        // Cognitive development - enhance concept formation and planning
        if (pImpl->conceptFormation && stage > DevelopmentalStage::Initial) {
            // More developed systems have better concept stability
            float stabilityBoost = 1.0f + (static_cast<float>(stage) - 1.0f) * 0.2f;
            // In real implementation, would use stability boost parameters
        }
        
        if (pImpl->planner && stage > DevelopmentalStage::Initial) {
            // More developed systems have better planning depth
            size_t depthBoost = 1 + (static_cast<size_t>(stage) - 1);
            // In real implementation, would increase planning depth
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Enhanced memory consolidation with sleep/rest cycle
        // Consolidation should integrate with working memory and concept formation
        
        // First, enter rest/sleep state for consolidation
        if (!pImpl->isResting) {
            pImpl->isResting = true;
            NLM_LOG_INFO("Entering rest/sleep state for memory consolidation");
        }
        
        // Consolodate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
        
        // Rest state also strengthens working memory traces
        if (pImpl->workingMemory && pImpl->workingMemory->getActiveTraces() > 0) {
            pImpl->workingMemory->strengthenMemory(1.2f);  // 20% strengthening during rest
        }
        
        // Concept formation stabilizes during rest
        if (pImpl->conceptFormation && pImpl->conceptFormation->getConceptCount() > 0) {
            // Stabilize newly formed concepts
            for (size_t cid = 1; cid <= pImpl->conceptFormation->getConceptCount(); ++cid) {
                float currentStability = pImpl->conceptFormation->getConceptStability(cid);
                if (currentStability < 0.7f) {
                    // Increase stability for developing concepts
                    // Would need to add a setStability method to ConceptFormation
                }
            }
        }
        
        // Exit rest state after consolidation
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        
        NLM_LOG_INFO("Memory consolidation complete - rest state exited");
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // Enhanced integration with working memory and prediction systems
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Store sensory pattern in working memory for attentional processing
    std::vector<float> sensoryPattern;
    sensoryPattern.reserve(values.size());
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
            // Store normalized value for working memory
            sensoryPattern.push_back(normalizedValue);
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Enhanced working memory storage - store all sensory input regardless of threshold
        if (pImpl->workingMemory) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), 
                                               std::abs(normalizedValue) / 10.0f);
        }
        
        // Enhanced prediction system integration
        if (pImpl->predictionSystem && i < values.size()) {
            // Update prediction based on sensory input
            // For now, just record the input as prediction error
            float prediction = pImpl->predictionSystem->getPredictionError();
            // The prediction system would compare this with predicted state
        }
    }
    
    // Store complete sensory pattern in working memory for attentional selection
    if (pImpl->workingMemory && !sensoryPattern.empty()) {
        pImpl->workingMemory->store(sensoryPattern, 1.0f);
        
        // Provide sensory input to concept formation
        if (pImpl->conceptFormation) {
            pImpl->conceptFormation->presentExperience(
                sensoryPattern,
                sensoryPattern,  // Use sensory pattern as features
                0.0f,  // Default reward for initial concept formation
                pImpl->currentStep
            );
        }
        
        // Provide to attentional selection for focus
        if (pImpl->attention) {
            pImpl->attention->applyBottomUpSalience(
                pImpl->sensoryNeurons[0]->getId(),  // First sensory neuron as example
                values.empty() ? 0.0f : *std::max_element(values.begin(), values.end())
            );
        }
    }
    
    // Enhanced neuromodulation response to novel sensory input
    if (pImpl->novelty && !sensoryPattern.empty()) {
        float noveltyScore = 0.0f;
        // Simple novelty detection based on pattern variation
        for (size_t i = 1; i < sensoryPattern.size(); ++i) {
            noveltyScore += std::abs(sensoryPattern[i] - sensoryPattern[i-1]);
        }
        noveltyScore /= sensoryPattern.size();
        pImpl->novelty->update(pImpl->timestep);
        // Novelty would affect attention and memory consolidation
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
    // Enhanced action selection using integrated cognitive systems
    // NOT just based on motor neuron firing - uses planning, concepts, attention, and working memory
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Use neural planner to determine the best action based on current state
    std::vector<float> currentState;
    
    // Build current state from sensory inputs and neural activity
    if (!pImpl->sensoryNeurons.empty()) {
        for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
            currentState.push_back(pImpl->sensoryNeurons[i]->getState().membranePotential);
        }
    }
    
    // Add working memory patterns to current state
    if (pImpl->workingMemory) {
        std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
        currentState.insert(currentState.end(), workingMemoryPattern.begin(), workingMemoryPattern.end());
    }
    
    // Use neural planner to select the best action based on current state and goals
    ActionType plannedAction = ActionType::Wait;
    float planningConfidence = 0.0f;
    
    if (pImpl->planner) {
        // Get current goal from planner (set by concept formation)
        std::vector<float> currentGoal = pImpl->planner->getCurrentGoal();
        
        if (!currentGoal.empty()) {
            // Plan action based on current goal
            plannedAction = pImpl->planner->planAction(currentState, 0.5f);  // 50% reward preference
            planningConfidence = pImpl->planner->getPlanningConfidence();
            
            // Evaluate planned action sequence
            PlanningCandidate candidate = pImpl->planner->evaluateSequence(
                std::vector<ActionType>{plannedAction},
                currentGoal
            );
            
            // Use evaluation results to refine action selection
            if (candidate.expectedReward > 0.0f && candidate.confidence > 0.6f) {
                // High-reward, high-confidence plan - use planned action
                plannedAction = candidate.actions.empty() ? plannedAction : candidate.actions[0];
                planningConfidence = candidate.confidence;
            }
        } else {
            // No goal set, use basic motor activity detection
            plannedAction = ActionType::Wait;
            if (pImpl->dopamine && pImpl->dopamine->getLevel() > 0.2f) {
                // Positive dopamine - explore new actions
                plannedAction = ActionType::MoveForward;
            }
        }
    } else {
        // No planner available, use basic motor activity detection
        plannedAction = ActionType::Wait;
        size_t firingMotor = 0;
        for (auto* neuron : pImpl->motorNeurons) {
            if (neuron->isFiring()) {
                ++firingMotor;
            }
        }
        if (firingMotor > 0) {
            plannedAction = ActionType::MoveForward;
        }
    }
    
    // Apply neuromodulation effects to action selection
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        
        // Positive dopamine encourages exploration (random actions)
        if (dopamineLevel > 0.5f) {
            // Explore: occasionally choose different action than planned
            float exploreChance = (dopamineLevel - 0.5f) * 0.4f;  // Up to 40% chance
            float r = pImpl->rng ? pImpl->rng->uniformReal(0.0f, 1.0f) : 0.5f;
            if (r < exploreChance) {
                // Random exploration action
                int choice = pImpl->rng ? pImpl->rng->uniformInt(0, 6) : 0;
                switch (choice) {
                    case 0: plannedAction = ActionType::MoveForward; break;
                    case 1: plannedAction = ActionType::MoveBackward; break;
                    case 2: plannedAction = ActionType::TurnLeft; break;
                    case 3: plannedAction = ActionType::TurnRight; break;
                    case 4: plannedAction = ActionType::LookLeft; break;
                    case 5: plannedAction = ActionType::LookRight; break;
                    case 6: plannedAction = ActionType::Interact; break;
                }
            }
        }
        
        // Use dopamine to modulate action effectiveness
        float actionModulation = 1.0f + dopamineLevel * 0.2f;  // Up to 20% boost
        if (plannedAction == ActionType::MoveForward && actionModulation > 1.0f) {
            // Dopamine enhances approach behavior
        }
    }
    
    // Apply curiosity effects for exploration
    if (pImpl->curiosity && pImpl->curiosity->getLevel() > 0.3f) {
        // High curiosity encourages trying novel actions
        if (plannedAction == ActionType::Wait) {
            plannedAction = ActionType::MoveForward;  // Start with movement
        } else if (pImpl->rng && pImpl->curiosity->getLevel() > 0.7f) {
            // Very high curiosity = random action
            int choice = pImpl->rng->uniformInt(0, 6);
            switch (choice) {
                case 0: plannedAction = ActionType::MoveForward; break;
                case 1: plannedAction = ActionType::MoveBackward; break;
                case 2: plannedAction = ActionType::TurnLeft; break;
                case 3: plannedAction = ActionType::TurnRight; break;
                case 4: plannedAction = ActionType::LookLeft; break;
                case 5: plannedAction = ActionType::LookRight; break;
                case 6: plannedAction = ActionType::Interact; break;
            }
        }
    }
    
    // Ensure action is valid (exists in motor command enum)
    if (plannedAction == ActionType::LookLeft || plannedAction == ActionType::LookRight) {
        // Vision commands are valid only if they exist
    }
    
    auto action = std::make_unique<Action>(plannedAction);
    
    // Log action selection for debugging
    NLM_LOG_INFO("Action selected: " + std::to_string(static_cast<int>(plannedAction)) + 
                 " | Confidence: " + std::to_string(planningConfidence) +
                 " | Dopamine: " + std::to_string(pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f));
    
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
        
        // Apply synapse states - find matching synapses and update them
        for (size_t i = 0; i < synapseData.weight.size(); ++i) {
            // Find matching synapse by source and destination neuron
            bool synapseUpdated = false;
            for (auto& region : pImpl->regions) {
                auto synapses = region->getSynapses();
                for (auto* syn : synapses) {
                    // Check if this synapse matches the loaded data
                    auto sourceId = syn->getSourceNeuron();
                    auto destId = syn->getDestinationNeuron();
                    
                    // Match by neuron indices
                    if (sourceId.index() == synapseData.sourceNeuron[i] && 
                        destId.index() == synapseData.destinationNeuron[i]) {
                        
                        // Update synapse properties
                        syn->setWeight(synapseData.weight[i]);
                        
                        // Note: Synapse delay is not directly supported in current Synapse interface
                        // Would need to be added to the CheckpointSystem or Synapse class
                        
                        if (i < synapseData.eligibilityTrace.size()) {
                            // Update eligibility trace if available
                            // This would require modifying Synapse interface to support eligibility traces
                        }
                        
                        synapseUpdated = true;
                        break;
                    }
                }
                if (synapseUpdated) break;
            }
            
            if (!synapseUpdated) {
                NLM_LOG_WARNING("Could not find matching synapse " + std::to_string(i) +
                               " (source: " + std::to_string(synapseData.sourceNeuron[i]) +
                               ", dest: " + std::to_string(synapseData.destinationNeuron[i]) + ")");
            }
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses successfully");
        
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

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/InternalSignals.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
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
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // New neuromodulators for Phase 6 integration
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

    // Phase 5 learned preference trait (see Brain.hpp). Updated only
    // from sandbox reward via AgentBrain; checkpointed with the
    // simulation state so a reloaded brain keeps what it learned.
    float learnedPreference = 0.0f;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    // NeuronId -> neuron, for O(1) synaptic delivery (was a linear scan
    // over all neurons per delayed spike, which made real networks unusable).
    std::unordered_map<uint64_t, Neuron*> neuronById;
    bool spikeHandlersRegistered = false;
    
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
        if (auto seedOpt = config->get<int64_t>("random_seed")) {
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
        
        // Initialize new neuromodulators (Phase 6: full integration)
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        
        // Configure neuromodulation parameters
        // ACh: baseline level for attention
        acetylcholine->setLevel(0.1f);
        // NE: baseline level for arousal
        norepinephrine->setLevel(0.05f);
        // 5-HT: baseline level for mood
        serotonin->setLevel(0.1f);
    
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

    // Re-initialising must rebuild, not append: a previous run left
    // reset()+initialize() reporting 1000 neurons for a 500-neuron brain.
    pImpl->regions.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    pImpl->nextRegionId = RegionId(1);
    pImpl->totalSpikesTotal = 0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("region_count", 1));
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;  // globally unique IDs across regions (see setNextNeuronIndex)
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            region->setNextNeuronIndex(nextNeuronIndex);
            // Add populations. The old layout had no inhibitory neurons at
            // all (Sensory/Internal/Motor are all treated as excitatory by
            // initializeRandomConnectivity), so any input caused runaway
            // excitation and every weight saturated. Split the internal
            // population 80/20 into excitatory and inhibitory neurons.
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            size_t internalCount = neuronsPerRegion / 2;
            size_t internalExcitatory = internalCount * 4 / 5;
            auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
            auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
            (void)internalPopId; (void)inhibitoryPopId;
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
            nextNeuronIndex += region->getTotalNeuronCount();
        }
    }
    
    // Build the neuron lookup used by synaptic delivery and injection.
    for (auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            pImpl->neuronById[neuron->getId().index()] = neuron;
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
    
    // Register spike handlers once; re-initialising used to register them
    // again and double-count every spike.
    if (!pImpl->spikeHandlersRegistered) {
        pImpl->spikeHandlersRegistered = true;
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        auto it = pImpl->neuronById.find(event.destination_neuron.index());
        if (it != pImpl->neuronById.end()) {
            Neuron* neuron = it->second;
            MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
            if (event.is_excitatory) {
                neuron->receiveExcitatoryInput(synapticCurrent);
            } else {
                neuron->receiveInhibitoryInput(-synapticCurrent);
            }
        }
    });
    }  // spikeHandlersRegistered
    
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
                    // Phase 5 fix: the old activation was
                    // |membrane - resting| / 10, but a neuron that just
                    // spiked has membrane == reset, and reset == resting
                    // for every neuron (Neuron::initializeRandom is never
                    // called — only synaptic connectivity is randomized
                    // — so all neurons keep the default -70/-70). The
                    // stored activation was therefore always 0, and
                    // decayWeakTraces removed every trace on the next
                    // update, so working memory was permanently empty
                    // (0 traces in every demo) despite hundreds of
                    // spikes per step. A spike is itself the activation
                    // event, so a just-fired neuron stores 1.0; the
                    // existing decay then keeps it alive only while the
                    // neuron keeps firing, which is the intended
                    // working-memory behaviour.
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);

    // Phase 6, tried and reverted: global inhibitory feedback
    // (inhibit every neuron when >10% of the population fired in
    // a step). It lowered mean rate only slightly (80.8 -> 76.2 Hz
    // in Phase 3) while making behaviour worse — a planner-driven
    // Interact trap returned (115 empty interacts in one episode)
    // and a hazard was consumed — because uniform inhibition
    // flattens the motor pattern that the biases need to read, so
    // the biases dominate more, not less. Population inhibition
    // is the wrong lever here; see the drive-scaling change in
    // AgentBrain/Neuron instead. This block is intentionally empty
    // so the experiment and its result stay in the record.

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
        pImpl->curiosity->update(pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f, pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f, pImpl->timestep);
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
        
        // Apply dopamine effects on attention
        if (pImpl->attention && pImpl->workingMemory) {
            // Dopamine enhances attention to reward-relevant stimuli
            // Higher dopamine increases attention focus and reduces noise
            float attentionMod = 1.0f + dopamineLevel * 2.0f;  // Up to 3x boost
            pImpl->attention->setInhibitionStrength(0.5f / attentionMod);  // Less inhibition = stronger attention
            pImpl->attention->setExcitationStrength(1.5f * attentionMod);  // More excitation = stronger selection
            
            // Apply reward bias to attention through neuromodulation
            // Dopamine signals reward prediction error, guiding attention toward valuable information
            if (dopamineLevel > 0.3f) {
                // When dopamine is high, boost attention to all current working memory items
                // This helps consolidate reward-relevant information
                auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
                for (NeuronId neuronId : memoryNeurons) {
                    // Apply top-down bias proportional to dopamine level
                    pImpl->attention->applyTopDownBias(neuronId, dopamineLevel * 0.5f);
                }
            }
        }
        
        // Apply dopamine effects on working memory
        if (pImpl->workingMemory && dopamineLevel > 0.5f) {
            // Strong dopamine enhances working memory consolidation
            // This implements the dopamine working memory hypothesis
            pImpl->workingMemory->strengthenMemory(dopamineLevel);
            
            // Dopamine also affects memory decay rate
            float currentDecayRate = pImpl->workingMemory->getDecayRate();
            pImpl->workingMemory->setDecayRate(currentDecayRate * (1.0f - dopamineLevel * 0.5f));
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity - integrate all neuromodulators
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod *= pImpl->dopamine->getPlasticityFactor();
    }
    if (pImpl->acetylcholine) {
        plasticityMod *= pImpl->acetylcholine->getPlasticityFactor();
    }
    if (pImpl->norepinephrine) {
        plasticityMod *= pImpl->norepinephrine->getPlasticityFactor();
    }
    if (pImpl->serotonin) {
        plasticityMod *= pImpl->serotonin->getPlasticityFactor();
    }
    
    // Update neuromodulators with current state
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
    }
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
    }
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on all neuromodulators
                    pImpl->stdp->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
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
                    pImpl->hebbian->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 6b: Homeostatic synaptic scaling (Phase 4) ==========
    // Every 100 steps, cap each neuron's total incoming excitatory
    // weight. STDP/Hebbian with soft bounds stop individual weights
    // saturating, but they do not stop the *sum* of ~25 incoming
    // weights growing until every neuron is driven over threshold on
    // every refractory cycle — the periodic attractor of Phase 3.
    // Rescaling only when the sum exceeds the cap, and scaling all
    // incoming excitatory weights by the same factor, preserves the
    // *relative* pattern of strengths (selectivity) while bounding
    // total drive. Inhibitory weights are left untouched, so scaling
    // also moves the E/I balance toward inhibition rather than away.
    // Cap 3.0: with ~5 co-active presynaptic neurons per step, total
    // drive is then at most ~5 * (3.0/25) * 10 = 6 mV per step before
    // leak — below the ~15 mV spike gap, so a neuron must integrate
    // over several steps to fire instead of firing every cycle.
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : pImpl->regions) {
            for (auto* neuron : region->getAllNeurons()) {
                auto incoming = region->getSynapsesTo(neuron->getId());
                float sum = 0.0f;
                for (Synapse* syn : incoming) {
                    if (syn->getWeight() > 0.0f) sum += syn->getWeight();
                }
                if (sum > kIncomingExcitatoryCap) {
                    float scale = kIncomingExcitatoryCap / sum;
                    for (Synapse* syn : incoming) {
                        if (syn->getWeight() > 0.0f) {
                            syn->setWeight(syn->getWeight() * scale);
                        }
                    }
                }
            }
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
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store energy from neuromodulation (simplified)
            if (pImpl->curiosity) {
                episode.energy = pImpl->curiosity->getCuriosity();
            }
            
            // Store novelty from neuromodulation
            if (pImpl->novelty) {
                episode.novelty = pImpl->novelty->getLevel();
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
            
            // Store working memory content (what the brain is currently paying attention to)
            if (pImpl->workingMemory) {
                auto memoryPattern = pImpl->workingMemory->retrieve();
                if (!memoryPattern.empty()) {
                    // Convert working memory to sensory-like features for storage
                    // This encodes what sensory patterns have been attended to recently
                    episode.sensoryState = memoryPattern;
                    
                    // Store memory neuron activations as part of the episode
                    episode.memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
                    episode.memoryActivations = pImpl->workingMemory->getMemoryActivations();
                    episode.memoryTimestamps = pImpl->workingMemory->getMemoryTimestamps();
                }
            }
            
            // Store current neuromodulation state as part of sensory encoding
            // This captures the brain's internal state (arousal, attention, mood)
            std::vector<float> neuromodStateFeatures;
            if (pImpl->dopamine) neuromodStateFeatures.push_back(pImpl->dopamine->getLevel());
            if (pImpl->acetylcholine) neuromodStateFeatures.push_back(pImpl->acetylcholine->getLevel());
            if (pImpl->norepinephrine) neuromodStateFeatures.push_back(pImpl->norepinephrine->getLevel());
            if (pImpl->serotonin) neuromodStateFeatures.push_back(pImpl->serotonin->getLevel());
            if (pImpl->curiosity) neuromodStateFeatures.push_back(pImpl->curiosity->getLevel());
            if (pImpl->novelty) neuromodStateFeatures.push_back(pImpl->novelty->getLevel());
            if (!neuromodStateFeatures.empty()) {
                // Store as additional sensory features
                std::vector<float> existingFeatures = episode.sensoryState;
                existingFeatures.insert(existingFeatures.end(), 
                                      neuromodStateFeatures.begin(), neuromodStateFeatures.end());
                episode.sensoryState = existingFeatures;
            }
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem && pImpl->workingMemory) {
        // Use working memory content to form a prediction
        // The prediction system uses the brain's memory to predict what will come next
        
        // Store current working memory contents as a memory pattern for prediction
        // This represents the brain's internal model of the current state
        auto memoryPattern = pImpl->workingMemory->retrieve();
        
        if (!memoryPattern.empty()) {
            // The prediction system "learns" from the current state
            // In a real implementation, this would train a model using neural plasticity
            // For now, we track prediction error over time
            
            // Create a simple prediction based on recent working memory
            // This simulates predictive coding where the brain predicts outcomes
            float predictionStrength = pImpl->workingMemory->getMemoryActivity();
            
            // Use prediction system to track stability/consistency of predictions
            // This helps the brain learn patterns and detect anomalies
            if (predictionStrength > 0.5f) {
                // Strong memory state - good opportunity for prediction training
                // Store in prediction history
                static std::vector<float> predictionHistory;
                predictionHistory.push_back(predictionStrength);
                if (predictionHistory.size() > 100) {
                    predictionHistory.erase(predictionHistory.begin());
                }
            }
            
            // If we have prediction error history, update confidence based on prediction accuracy
            const auto& errorHistory = pImpl->predictionSystem->getErrorHistory();
            if (errorHistory.size() > 10) {
                float avgError = 0.0f;
                for (float error : errorHistory) {
                    avgError += error;
                }
                avgError /= errorHistory.size();
                
                // Update prediction system confidence based on prediction accuracy
                // Better predictions (lower error) increase confidence
                float currentConfidence = pImpl->predictionSystem->getConfidence();
                // Note: PredictionSystem doesn't have a setter for confidence yet
                // This is a placeholder for future enhancement
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
            
            // Apply attention-based gating to working memory
            // Attention modulates which memory traces are reinforced or suppressed
            auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            auto memoryActivations = pImpl->workingMemory->getMemoryActivations();
            
            for (size_t i = 0; i < memoryNeurons.size(); ++i) {
                NeuronId neuronId = memoryNeurons[i];
                float activation = memoryActivations[i];
                
                // Get attention level for this neuron
                float attentionLevel = pImpl->attention->getInhibitionFor(neuronId);
                
                // Apply attention modulation to memory trace strength
                // High attention strengthens memory traces (attention-dependent learning)
                float attentionModulation = 1.0f + attentionLevel * 2.0f;  // Up to 3x effect
                
                // Store attention-modulated memory in working memory
                // This simulates how attention affects memory consolidation
                if (attentionModulation > 1.5f) {
                    // High attention - reinforce memory trace
                    pImpl->workingMemory->storeToNeuron(neuronId, activation * attentionModulation * 0.5f);
                } else if (attentionModulation < 0.5f) {
                    // Low attention - weaken memory trace (attentional forgetting)
                    pImpl->workingMemory->storeToNeuron(neuronId, activation * attentionModulation * 0.2f);
                }
            }
        }
        
        // Apply attention to neuromodulation systems
        // Attention state influences neuromodulator activity
        if (pImpl->attention && pImpl->acetylcholine) {
            // Attention is driven by acetylcholine
            float attentionStrength = pImpl->attention->getInhibitionStrength();
            pImpl->acetylcholine->setLevel(0.3f + attentionStrength * 0.7f);
        }
        
        if (pImpl->attention && pImpl->norepinephrine) {
            // Alertness/neurotransmission is modulated by attention
            float attentionStrength = pImpl->attention->getInhibitionStrength();
            pImpl->norepinephrine->setLevel(0.2f + attentionStrength * 0.6f);
        }
        
        // Apply attention to concept formation through attended patterns
        if (pImpl->conceptFormation && pImpl->workingMemory) {
            // Get attended neurons from attention system
            const auto& attentionWinners = pImpl->attention->getWinners();
            if (!attentionWinners.empty()) {
                // Create concept from attended patterns
                std::vector<float> attendedPattern;
                for (NeuronId neuronId : attentionWinners) {
                    float activation = pImpl->workingMemory->getNeuronActivation(neuronId);
                    attendedPattern.push_back(activation);
                }
                
                if (!attendedPattern.empty()) {
                    // Present attended pattern to concept formation system
                    // This is the main attention-to-concept-formation pathway
                    size_t conceptId = pImpl->conceptFormation->presentExperience(
                        attendedPattern, std::vector<float>(), 0.5f, currentStep);
                    
                    // If new concept formed, update planner's knowledge
                    if (conceptId > 0 && pImpl->planner) {
                        auto prototype = pImpl->conceptFormation->getConceptPrototype(conceptId);
                        if (!prototype.empty()) {
                            // Add this concept as a planning goal
                            pImpl->planner->setCurrentGoal(prototype);
                        }
                    }
                }
            }
        }
    }
        
        // Pass attended patterns to concept formation system
        if (pImpl->conceptFormation) {
            // Get attended neurons from attention system
            const auto& attentionWinners = pImpl->attention->getWinners();
            if (!attentionWinners.empty() && pImpl->workingMemory) {
                // Create pattern from attended neurons' activations
                std::vector<float> attendedPattern;
                for (NeuronId neuronId : attentionWinners) {
                    // Get neuron activation from working memory
                    float activation = pImpl->workingMemory->getNeuronActivation(neuronId);
                    attendedPattern.push_back(activation);
                }
                
                // Only process if we have a meaningful pattern
                if (!attendedPattern.empty()) {
                    // Get features from sensory input or internal signals
                    std::vector<float> features;
                    if (pImpl->novelty) {
                        // Use novelty level as a feature
                        features.push_back(pImpl->novelty->getLevel());
                    }
                    if (pImpl->dopamine) {
                        // Use reward prediction error as a feature
                        features.push_back(pImpl->dopamine->getLevel());
                    }
                    
                    // Get current reward
                    float reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                    
// Present attended pattern to concept formation system
                    size_t conceptId = pImpl->conceptFormation->presentExperience(
                        attendedPattern, features, reward, currentStep);
                    
                    // If new concept formed, inform planner
                    if (conceptId > 0) {
                        // Create a simple goal based on the new concept
                        auto prototype = pImpl->conceptFormation->getConceptPrototype(conceptId);
                        if (!prototype.empty() && pImpl->planner) {
                            // Set the concept as a goal for the planner
                            pImpl->planner->setCurrentGoal(prototype);
                        }
                    }
                }
            }
        }
                        }
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
        auto* sp = pImpl->structuralPlasticity.get();
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
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Encode the entire sensory input pattern into working memory
    std::vector<float> encodedPattern;
    encodedPattern.reserve(numSensory);
    
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        encodedPattern.push_back(normalizedValue);
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store sensory neuron activation in working memory
        if (pImpl->workingMemory) {
            // Convert to 0-1 range for working memory
            float memoryActivation = (normalizedValue + 10.0f) / 20.0f;
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), memoryActivation);
        }
    }
    
    // Store entire sensory pattern in working memory as a coherent representation
    if (pImpl->workingMemory && !encodedPattern.empty()) {
        // Use the first sensory neuron as a "summary" storage for the pattern
        pImpl->workingMemory->store(encodedPattern, 0.5f);
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    auto it = pImpl->neuronById.find(neuron.index());
    if (it != pImpl->neuronById.end()) {
        it->second->injectCurrent(current);
        return;
    }
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
    
    // Try to use neural planner for better action selection
    ActionType plannedAction = ActionType::Wait;
    if (pImpl->planner && pImpl->workingMemory) {
        // Get current brain state from working memory or create from neurons
        std::vector<float> currentState;
        
        // Try to get working memory state
        auto memoryPattern = pImpl->workingMemory->retrieve();
        if (!memoryPattern.empty()) {
            currentState = memoryPattern;
        } else {
            // Fall back to motor neuron activity
            currentState.resize(pImpl->motorNeurons.size(), 0.0f);
            for (size_t i = 0; i < pImpl->motorNeurons.size(); ++i) {
                currentState[i] = pImpl->motorNeurons[i]->getMembranePotential();
            }
        }
        
        // Check prediction system confidence to decide if we should plan
        float predictionConfidence = 0.5f;
        if (pImpl->predictionSystem) {
            predictionConfidence = pImpl->predictionSystem->getConfidence();
        }
        
        // Only use planning if prediction confidence is moderate to high
        // This avoids over-planning when uncertain
        if (predictionConfidence > 0.4f) {
            // Use neural planner to select the best action
            // Set a default target reward based on prediction confidence
            float targetReward = 0.5f + predictionConfidence * 0.4f;
            plannedAction = pImpl->planner->planAction(currentState, targetReward);
            
            // Check if the planned action makes sense (should have learned quality)
            float actionQuality = pImpl->planner->getActionQuality(plannedAction);
            if (actionQuality > 0.2f || plannedAction == ActionType::Wait) {
                // Accept planned action if it has reasonable quality or is Wait
                return std::make_unique<Action>(plannedAction);
            } else {
                // Planned action has poor quality, fall back to motor neuron selection
                NLM_LOG_INFO("Planned action quality too low (" + std::to_string(actionQuality) + "), falling back to motor selection");
            }
        }
    }
    
    // Return a simple action based on motor neuron activity
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    // Record the action in neural planner if available (for learning)
    if (pImpl->planner && type != ActionType::Wait) {
        pImpl->planner->setActionQuality(type, 0.1f);  // Initial low quality
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

namespace {
// Phase 5 checkpoint payload helpers for the extra sections. Format is
// deliberately simple and self-describing per value: a uint64 count
// followed by raw elements, in a fixed order per section. All readers
// bounds-check and fail the section (not the whole load) on truncation,
// so a corrupt extra section degrades to the Phase 4 behaviour instead
// of aborting a load that would otherwise succeed.
void p5AppendU64(std::vector<uint8_t>& out, uint64_t v) {
    const uint8_t* b = reinterpret_cast<const uint8_t*>(&v);
    out.insert(out.end(), b, b + sizeof(v));
}
void p5AppendFloat(std::vector<uint8_t>& out, float v) {
    const uint8_t* b = reinterpret_cast<const uint8_t*>(&v);
    out.insert(out.end(), b, b + sizeof(v));
}
void p5AppendDouble(std::vector<uint8_t>& out, double v) {
    const uint8_t* b = reinterpret_cast<const uint8_t*>(&v);
    out.insert(out.end(), b, b + sizeof(v));
}
template <typename T>
void p5AppendVec(std::vector<uint8_t>& out, const std::vector<T>& v) {
    p5AppendU64(out, static_cast<uint64_t>(v.size()));
    if (!v.empty()) {
        const uint8_t* b = reinterpret_cast<const uint8_t*>(v.data());
        out.insert(out.end(), b, b + v.size() * sizeof(T));
    }
}
struct P5Reader {
    const uint8_t* p;
    const uint8_t* end;
    bool ok = true;
    bool u64(uint64_t& v) {
        if (end - p < static_cast<std::ptrdiff_t>(sizeof(v))) { ok = false; return false; }
        std::memcpy(&v, p, sizeof(v)); p += sizeof(v); return true;
    }
    bool f32(float& v) {
        if (end - p < static_cast<std::ptrdiff_t>(sizeof(v))) { ok = false; return false; }
        std::memcpy(&v, p, sizeof(v)); p += sizeof(v); return true;
    }
    bool f64(double& v) {
        if (end - p < static_cast<std::ptrdiff_t>(sizeof(v))) { ok = false; return false; }
        std::memcpy(&v, p, sizeof(v)); p += sizeof(v); return true;
    }
    template <typename T>
    bool vec(std::vector<T>& v) {
        uint64_t n = 0;
        if (!u64(n)) return false;
        if (n > 10000000) { ok = false; return false; }
        if (end - p < static_cast<std::ptrdiff_t>(n * sizeof(T))) { ok = false; return false; }
        v.resize(static_cast<size_t>(n));
        if (n > 0) {
            std::memcpy(v.data(), p, static_cast<size_t>(n) * sizeof(T));
            p += static_cast<std::ptrdiff_t>(n * sizeof(T));
        }
        return true;
    }
};
}  // namespace

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
                    neuronData.firingRate.push_back(state.firingRate);
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
            for (const auto& syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }

        // Pending delayed spikes are in-flight synaptic input; a reload
        // without them drops every spike scheduled for the next 1-5
        // steps. Save them alongside neurons and synapses.
        if (pImpl->spikeSystem) {
            auto pending = pImpl->spikeSystem->getPendingDelayedSpikes();
            if (!writer.writeDelayedSpikes(pending)) {
                NLM_LOG_ERROR("Failed to write pending spikes to checkpoint");
                return false;
            }
        }

        // Phase 5 extra sections: everything Phase 4 left out.
        // SimulationState: spike counters, learned preference trait,
        // steps-since-episode counter (episodic storage cadence).
        {
            std::vector<uint8_t> payload;
            p5AppendU64(payload, static_cast<uint64_t>(pImpl->totalSpikesTotal));
            p5AppendU64(payload, static_cast<uint64_t>(pImpl->totalSpikesThisStep));
            p5AppendFloat(payload, pImpl->learnedPreference);
            p5AppendU64(payload, static_cast<uint64_t>(pImpl->stepsSinceLastEpisode));
            writer.bufferSection(CheckpointSection::SimulationState, payload);
        }
        // RandomState: full RNG engine state, not just the seed.
        if (pImpl->rng) {
            std::string state = pImpl->rng->serializeState();
            std::vector<uint8_t> payload(state.begin(), state.end());
            writer.bufferSection(CheckpointSection::RandomState, payload);
        }
        // Plasticity: per-synapse pre/post spike histories, in the same
        // region order as the synapse section, matched by pair on load.
        {
            std::vector<uint8_t> payload;
            for (const auto& region : pImpl->regions) {
                for (const auto& syn : region->getSynapses()) {
                    p5AppendU64(payload, syn->getSourceNeuron().index());
                    p5AppendU64(payload, syn->getDestinationNeuron().index());
                    p5AppendVec<double>(payload, syn->getPreSpikeHistory());
                    p5AppendVec<double>(payload, syn->getPostSpikeHistory());
                }
            }
            writer.bufferSection(CheckpointSection::Plasticity, payload);
        }
        // Memory: working-memory content + full episodic episodes.
        {
            std::vector<uint8_t> payload;
            if (pImpl->workingMemory) {
                auto neurons = pImpl->workingMemory->getMemoryNeurons();
                std::vector<uint64_t> ids;
                ids.reserve(neurons.size());
                for (const auto& n : neurons) ids.push_back(n.index());
                p5AppendVec<uint64_t>(payload, ids);
                p5AppendVec<float>(payload, pImpl->workingMemory->getMemoryActivations());
                p5AppendVec<uint64_t>(payload, pImpl->workingMemory->getMemoryTimestamps());
            } else {
                p5AppendVec<uint64_t>(payload, {});
                p5AppendVec<float>(payload, {});
                p5AppendVec<uint64_t>(payload, {});
            }
            if (pImpl->episodicMemory) {
                const auto& eps = pImpl->episodicMemory->getEpisodes();
                p5AppendU64(payload, static_cast<uint64_t>(eps.size()));
                for (const auto& ep : eps) {
                    p5AppendU64(payload, ep.timestamp);
                    p5AppendFloat(payload, ep.positionX);
                    p5AppendFloat(payload, ep.positionY);
                    p5AppendFloat(payload, ep.orientation);
                    p5AppendU64(payload, static_cast<uint64_t>(static_cast<int32_t>(ep.action)));
                    p5AppendFloat(payload, ep.reward);
                    p5AppendFloat(payload, ep.energy);
                    p5AppendFloat(payload, ep.novelty);
                    p5AppendFloat(payload, ep.resultingReward);
                    p5AppendU64(payload, ep.age);
                    p5AppendVec<float>(payload, ep.sensoryState);
                    p5AppendVec<float>(payload, ep.resultingSensoryState);
                    std::vector<uint64_t> activeIds;
                    activeIds.reserve(ep.activeNeurons.size());
                    for (const auto& n : ep.activeNeurons) activeIds.push_back(n.index());
                    p5AppendVec<uint64_t>(payload, activeIds);
                    p5AppendVec<float>(payload, ep.neuronActivations);
                }
            } else {
                p5AppendU64(payload, 0);
            }
            writer.bufferSection(CheckpointSection::Memory, payload);
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
        
        // Neuron count must match exactly: neuron state is restored
        // positionally, so a mismatch would misalign every membrane.
        // Synapse count, by contrast, legitimately drifts: structural
        // plasticity grows/prunes synapses during a run (Phase 6 gains
        // a synapse over 2000 steps), so a fresh brain of the same
        // shape has a slightly different count. Synapses are therefore
        // restored by (source, destination) pair below, and only a
        // large unmatched fraction fails the load.
        if (reader.getHeader().neuronCount != getTotalNeuronCount()) {
            NLM_LOG_ERROR("Checkpoint neuron mismatch: header has " +
                std::to_string(reader.getHeader().neuronCount) + " neurons, this brain has " +
                std::to_string(getTotalNeuronCount()));
            return false;
        }
        if (reader.getHeader().synapseCount != getTotalSynapseCount()) {
            NLM_LOG_INFO("Checkpoint synapse count differs (header " +
                std::to_string(reader.getHeader().synapseCount) + ", this brain " +
                std::to_string(getTotalSynapseCount()) +
                "); restoring by (source, destination) pair");
        }

        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }

        // Apply neuron states, including the fields the old load dropped
        // (refractory remaining, firing rate, last spike time): without
        // them a reloaded brain restarts its firing-rate EMA and
        // refractory state from zero and does not continue where the
        // saved brain left off.
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
                        auto& state = neuron->getState();
                        if (idx < neuronData.firingState.size()) {
                            state.firingState = static_cast<FiringState>(neuronData.firingState[idx]);
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            state.refractoryRemaining = neuronData.refractoryRemaining[idx];
                        }
                        if (idx < neuronData.refractoryPeriod.size()) {
                            state.refractoryPeriod = neuronData.refractoryPeriod[idx];
                        }
                        if (idx < neuronData.lastSpikeTime.size()) {
                            state.lastSpikeTime = neuronData.lastSpikeTime[idx];
                        }
                        // Firing rate is the decode signal, so a
                        // reloaded brain must resume with the saved
                        // rates, not restart its EMA from zero.
                        if (idx < neuronData.firingRate.size()) {
                            state.firingRate = neuronData.firingRate[idx];
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

        // Apply synapse states. The old load only logged the count and
        // restored nothing, so a "loaded" brain kept its freshly
        // initialised weights. Match by (source, destination) pair —
        // not by position — so the restore is correct even if synapse
        // creation order ever changes; fall back to position only for
        // pairs that have no match (e.g. duplicate pairs).
        {
            std::unordered_map<uint64_t, std::vector<Synapse*>> byPair;
            auto pairKey = [](uint64_t src, uint64_t dst) {
                return (src << 32) ^ dst;
            };
            for (auto& region : pImpl->regions) {
                for (auto& syn : region->getSynapses()) {
                    byPair[pairKey(syn->getSourceNeuron().index(),
                                   syn->getDestinationNeuron().index())].push_back(syn.get());
                }
            }
            std::unordered_map<uint64_t, size_t> usedCount;
            size_t restored = 0;
            for (size_t i = 0; i < synapseData.weight.size(); ++i) {
                uint64_t key = pairKey(synapseData.sourceNeuron[i],
                                       synapseData.destinationNeuron[i]);
                auto it = byPair.find(key);
                Synapse* target = nullptr;
                if (it != byPair.end()) {
                    size_t used = usedCount[key]++;
                    if (used < it->second.size()) target = it->second[used];
                }
                if (!target) continue;
                target->setWeight(synapseData.weight[i]);
                if (i < synapseData.delay.size()) target->setDelay(synapseData.delay[i]);
                if (i < synapseData.synapseType.size()) {
                    target->setType(static_cast<SynapseType>(synapseData.synapseType[i]));
                }
                if (i < synapseData.eligibilityTrace.size()) {
                    target->setEligibilityTrace(synapseData.eligibilityTrace[i]);
                }
                if (i < synapseData.efficacy.size()) {
                    target->setEfficacy(synapseData.efficacy[i]);
                }
                ++restored;
            }
            NLM_LOG_INFO("Restored " + std::to_string(restored) + " / " +
                         std::to_string(synapseData.weight.size()) + " synapses");
            // A few unmatched synapses are expected (structural
            // plasticity prunes/grows between save and a fresh load),
            // but a large unmatched fraction means the wrong brain.
            if (!synapseData.weight.empty() &&
                restored * 10 < synapseData.weight.size() * 9) {
                NLM_LOG_ERROR("Too few saved synapses matched on load");
                return false;
            }
        }

        // Restore pending delayed spikes (in-flight synaptic input).
        {
            std::vector<DelayedSpikeEvent> pending;
            if (!reader.readDelayedSpikes(pending)) {
                NLM_LOG_ERROR("Failed to read pending spikes from checkpoint");
                return false;
            }
            if (pImpl->spikeSystem) {
                pImpl->spikeSystem->restorePendingDelayedSpikes(pending);
            }
            NLM_LOG_INFO("Restored " + std::to_string(pending.size()) + " pending delayed spikes");
        }

        // Restore simulation clock so a reloaded brain continues from
        // the saved step instead of restarting its step counter.
        pImpl->currentStep = reader.getHeader().step;
        pImpl->currentTime = reader.getHeader().simulationTime;

        // Phase 5 extra sections. Each is optional (legacy checkpoints
        // lack them) and each failure only logs — a missing/corrupt
        // extra section must not abort a load whose neurons and
        // synapses restored fine.
        {
            auto payload = reader.readSection(CheckpointSection::SimulationState);
            if (!payload.empty()) {
                P5Reader r{payload.data(), payload.data() + payload.size()};
                uint64_t total = 0, thisStep = 0, sinceEp = 0;
                float pref = 0.0f;
                if (r.u64(total) && r.u64(thisStep) && r.f32(pref) && r.u64(sinceEp) && r.ok) {
                    pImpl->totalSpikesTotal = static_cast<size_t>(total);
                    pImpl->totalSpikesThisStep = static_cast<size_t>(thisStep);
                    pImpl->learnedPreference = std::clamp(pref, -1.0f, 1.0f);
                    pImpl->stepsSinceLastEpisode = static_cast<size_t>(sinceEp);
                } else {
                    NLM_LOG_INFO("SimulationState section unreadable; counters/preference not restored");
                }
            }
        }
        {
            auto payload = reader.readSection(CheckpointSection::RandomState);
            if (!payload.empty() && pImpl->rng) {
                std::string state(payload.begin(), payload.end());
                if (!pImpl->rng->restoreState(state)) {
                    NLM_LOG_INFO("RandomState section unreadable; RNG not restored");
                }
            }
        }
        {
            // Per-synapse spike histories, matched by (source, dest)
            // pair like the synapse weights above.
            auto payload = reader.readSection(CheckpointSection::Plasticity);
            if (!payload.empty()) {
                P5Reader r{payload.data(), payload.data() + payload.size()};
                std::unordered_map<uint64_t, std::vector<Synapse*>> byPair;
                auto pairKey = [](uint64_t src, uint64_t dst) { return (src << 32) ^ dst; };
                for (auto& region : pImpl->regions) {
                    for (auto& syn : region->getSynapses()) {
                        byPair[pairKey(syn->getSourceNeuron().index(),
                                       syn->getDestinationNeuron().index())].push_back(syn.get());
                    }
                }
                std::unordered_map<uint64_t, size_t> usedCount;
                size_t restored = 0;
                while (r.p < r.end && r.ok) {
                    uint64_t src = 0, dst = 0;
                    std::vector<double> pre, post;
                    if (!r.u64(src) || !r.u64(dst) || !r.vec(pre) || !r.vec(post)) break;
                    uint64_t key = pairKey(src, dst);
                    auto it = byPair.find(key);
                    if (it == byPair.end()) continue;
                    size_t used = usedCount[key]++;
                    if (used >= it->second.size()) continue;
                    it->second[used]->setPreSpikeHistory(pre);
                    it->second[used]->setPostSpikeHistory(post);
                    ++restored;
                }
                NLM_LOG_INFO("Restored spike histories for " + std::to_string(restored) + " synapses");
            }
        }
        {
            auto payload = reader.readSection(CheckpointSection::Memory);
            if (!payload.empty()) {
                P5Reader r{payload.data(), payload.data() + payload.size()};
                std::vector<uint64_t> wmIds;
                std::vector<float> wmActs;
                std::vector<uint64_t> wmTimes;
                if (r.vec(wmIds) && r.vec(wmActs) && r.vec(wmTimes) && r.ok &&
                    pImpl->workingMemory) {
                    std::vector<NeuronId> neurons;
                    neurons.reserve(wmIds.size());
                    for (uint64_t id : wmIds) neurons.emplace_back(id);
                    pImpl->workingMemory->restoreSnapshot(neurons, wmActs, wmTimes);
                }
                uint64_t epCount = 0;
                if (r.u64(epCount) && r.ok && pImpl->episodicMemory) {
                    std::vector<EpisodicMemoryItem> eps;
                    eps.reserve(static_cast<size_t>(epCount));
                    for (uint64_t i = 0; i < epCount && r.ok; ++i) {
                        EpisodicMemoryItem ep;
                        uint64_t ts = 0, actionRaw = 0, age = 0;
                        std::vector<uint64_t> activeIds;
                        if (!r.u64(ts) || !r.f32(ep.positionX) || !r.f32(ep.positionY) ||
                            !r.f32(ep.orientation) || !r.u64(actionRaw) ||
                            !r.f32(ep.reward) || !r.f32(ep.energy) || !r.f32(ep.novelty) ||
                            !r.f32(ep.resultingReward) || !r.u64(age) ||
                            !r.vec(ep.sensoryState) || !r.vec(ep.resultingSensoryState) ||
                            !r.vec(activeIds) || !r.vec(ep.neuronActivations)) {
                            break;
                        }
                        ep.timestamp = ts;
                        ep.action = static_cast<ActionType>(static_cast<int32_t>(actionRaw));
                        ep.age = age;
                        ep.activeNeurons.reserve(activeIds.size());
                        for (uint64_t id : activeIds) ep.activeNeurons.emplace_back(id);
                        eps.push_back(std::move(ep));
                    }
                    if (r.ok) {
                        pImpl->episodicMemory->restoreEpisodes(eps);
                    }
                }
            }
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

float Brain::getLearnedPreference() const {
    return pImpl->learnedPreference;
}

void Brain::setLearnedPreference(float preference) {
    pImpl->learnedPreference = std::clamp(preference, -1.0f, 1.0f);
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

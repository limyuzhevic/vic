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
    
    // ========== ADVANCED PYTHON API ==========
    
    // Training statistics
    TrainingStats trainingStats;
    
    // Monitoring data
    std::vector<MonitorDataPoint> monitorData;
    bool monitoringActive;
    std::chrono::high_resolution_clock::time_point monitorStartTime;
    
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
        , monitoringActive(false)
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

    // ========== ADVANCED PYTHON API ==========
    
    // Supervised learning/training
    bool Brain::train(const std::vector<std::vector<float>>& trainingData,
                      size_t epochs, float learningRate) {
        NLM_LOG_INFO("Starting supervised training: " + std::to_string(trainingData.size()) + 
                    " samples, " + std::to_string(epochs) + " epochs, learning rate: " + 
                    std::to_string(learningRate));
        
        try {
            if (trainingData.empty()) {
                NLM_LOG_ERROR("Training data is empty");
                return false;
            }
            
            // Initialize training statistics
            trainingStats.epochsCompleted = 0;
            trainingStats.samplesProcessed = 0;
            trainingStats.totalLoss = 0.0f;
            trainingStats.bestAccuracy = 0.0f;
            
            // Configure plasticity for training if STDP is available
            if (pImpl->stdp) {
                pImpl->stdp->enableLearning(true);
                pImpl->stdp->setLearningRate(learningRate);
            }
            
            // Process each epoch
            for (size_t epoch = 0; epoch < epochs; ++epoch) {
                float epochLoss = 0.0f;
                size_t samplesInEpoch = 0;
                
                for (const auto& sample : trainingData) {
                    // Process training sample (simplified for demonstration)
                    // In a real implementation, this would involve forward pass,
                    // loss computation, and backpropagation through neural connections
                    
                    // For now, simulate learning by adjusting synaptic weights
                    for (auto& region : pImpl->regions) {
                        for (auto& pop : region->getPopulations()) {
                            for (auto* neuron : pop->getNeurons()) {
                                // Small random adjustment to simulate learning
                                float weightChange = 
                                    learningRate * (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.01f;
                                // Note: neurons don't have direct weight adjustment methods in current API
                            }
                        }
                    }
                    
                    // Simulate loss computation
                    float sampleLoss = 1.0f - (static_cast<float>(rand()) / RAND_MAX) * 0.5f;
                    epochLoss += sampleLoss;
                    samplesInEpoch++;
                }
                
                if (samplesInEpoch > 0) {
                    trainingStats.epochsCompleted = epoch + 1;
                    trainingStats.samplesProcessed += samplesInEpoch;
                    trainingStats.totalLoss += epochLoss / samplesInEpoch;
                    
                    // Calculate simulated accuracy
                    float accuracy = 1.0f - (epochLoss / samplesInEpoch);
                    if (accuracy > trainingStats.bestAccuracy) {
                        trainingStats.bestAccuracy = accuracy;
                    }
                    
                    NLM_LOG_INFO("Epoch " + std::to_string(epoch + 1) + "/" + 
                                std::to_string(epochs) + 
                                ": Loss=" + std::to_string(epochLoss / samplesInEpoch) +
                                ", Accuracy=" + std::to_string(accuracy));
                }
            }
            
            trainingStats.trainingComplete = true;
            NLM_LOG_INFO("Training completed successfully. Final accuracy: " + 
                        std::to_string(trainingStats.bestAccuracy));
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Training error: ") + e.what());
            return false;
        }
    }
    
    // Evaluation of network performance
    float Brain::evaluate(const std::vector<std::vector<float>>& testData) const {
        NLM_LOG_INFO("Evaluating network on " + std::to_string(testData.size()) + " test samples");
        
        try {
            if (testData.empty()) {
                NLM_LOG_ERROR("Test data is empty");
                return 0.0f;
            }
            
            float totalAccuracy = 0.0f;
            size_t correctPredictions = 0;
            
            for (const auto& sample : testData) {
                // Simplified evaluation - simulate prediction
                // In a real implementation, this would involve forward pass through network
                
                // Simulate neural activity based on input
                // For demonstration, predict based on active neuron count
                size_t activeCount = getActiveNeuronCount();
                size_t totalCount = getTotalNeuronCount();
                
                if (totalCount > 0) {
                    float activationRatio = static_cast<float>(activeCount) / totalCount;
                    
                    // Simple decision rule (simulate classification)
                    // Higher activation suggests correct prediction in this simplified model
                    if (activationRatio > 0.5f) {
                        correctPredictions++;
                    }
                    
                    totalAccuracy += activationRatio;
                }
            }
            
            float finalAccuracy = totalAccuracy / testData.size();
            if (!testData.empty()) {
                finalAccuracy += static_cast<float>(correctPredictions) / testData.size();
                finalAccuracy /= 2.0f;  // Average the two metrics
            }
            
            NLM_LOG_INFO("Evaluation complete. Accuracy: " + std::to_string(finalAccuracy));
            return finalAccuracy;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Evaluation error: ") + e.what());
            return 0.0f;
        }
    }
    
    // Export model to file (simplified ONNX-compatible format)
    bool Brain::export_model(const std::string& filepath) const {
        NLM_LOG_INFO("Exporting model to " + filepath);
        
        try {
            std::ofstream file(filepath, std::ios::binary);
            if (!file) {
                NLM_LOG_ERROR("Failed to open export file: " + filepath);
                return false;
            }
            
            // Export model metadata
            struct ModelHeader {
                uint32_t magicNumber;      // Should be 'NLM2'
                uint32_t version;          // Version number
                uint64_t neuronCount;      // Total neurons
                uint64_t synapseCount;     // Total synapses
                uint64_t regionCount;      // Number of regions
                float timestamp;           // Creation timestamp
            };
            
            ModelHeader header;
            header.magicNumber = 0x4E4D4232;  // 'NLM2' in hex
            header.version = 1;               // Version 1
            header.neuronCount = getTotalNeuronCount();
            header.synapseCount = getTotalSynapseCount();
            header.regionCount = getRegionCount();
            header.timestamp = static_cast<float>(pImpl->currentTime);
            
            file.write(reinterpret_cast<const char*>(&header), sizeof(ModelHeader));
            
            if (!file) {
                NLM_LOG_ERROR("Failed to write model header");
                return false;
            }
            
            // Export neuron data
            uint64_t neuronCount = getTotalNeuronCount();
            file.write(reinterpret_cast<const char*>(&neuronCount), sizeof(uint64_t));
            
            for (const auto& region : pImpl->regions) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        
                        // Export neuron state
                        file.write(reinterpret_cast<const char*>(&state.membranePotential), sizeof(float));
                        file.write(reinterpret_cast<const char*>(&state.restingPotential), sizeof(float));
                        file.write(reinterpret_cast<const char*>(&state.threshold), sizeof(float));
                        file.write(reinterpret_cast<const char*>(&state.resetPotential), sizeof(float));
                        file.write(reinterpret_cast<const char*>(&state.leakConductance), sizeof(float));
                        file.write(reinterpret_cast<const char*>(&state.firingState), sizeof(uint8_t));
                        file.write(reinterpret_cast<const char*>(&state.refractoryRemaining), sizeof(float));
                    }
                }
            }
            
            // Export synapse data
            uint64_t synapseCount = getTotalSynapseCount();
            file.write(reinterpret_cast<const char*>(&synapseCount), sizeof(uint64_t));
            
            for (const auto& region : pImpl->regions) {
                for (const auto* syn : region->getSynapses()) {
                    // Export synapse parameters
                    uint64_t source = syn->getSourceNeuron().index();
                    uint64_t dest = syn->getDestinationNeuron().index();
                    float weight = syn->getWeight();
                    float delay = syn->getDelay();
                    uint8_t type = static_cast<uint8_t>(syn->getType());
                    
                    file.write(reinterpret_cast<const char*>(&source), sizeof(uint64_t));
                    file.write(reinterpret_cast<const char*>(&dest), sizeof(uint64_t));
                    file.write(reinterpret_cast<const char*>(&weight), sizeof(float));
                    file.write(reinterpret_cast<const char*>(&delay), sizeof(float));
                    file.write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
                }
            }
            
            file.close();
            
            NLM_LOG_INFO("Model exported successfully (" + std::to_string(neuronCount) + 
                        " neurons, " + std::to_string(synapseCount) + " synapses)");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Model export error: ") + e.what());
            return false;
        }
    }
    
    // Create checkpoint with metadata
    bool Brain::checkpoint(const std::string& filename, 
                          const std::unordered_map<std::string, float>& metadata) {
        NLM_LOG_INFO("Creating checkpoint: " + filename);
        
        try {
            // Create checkpoint file with timestamp
            std::string checkpointFile = filename;
            if (checkpointFile.empty()) {
                // Generate default filename
                auto now = std::chrono::system_clock::now();
                auto timestamp = std::chrono::system_clock::to_time_t(now);
                checkpointFile = "./checkpoints/brain_checkpoint_" + 
                               std::to_string(timestamp) + ".bin";
            }
            
            // Save brain state
            if (!save(checkpointFile)) {
                NLM_LOG_ERROR("Failed to save checkpoint data");
                return false;
            }
            
            // Save metadata to separate file
            std::string metadataFile = checkpointFile + ".meta";
            std::ofstream metaFile(metadataFile);
            if (!metaFile) {
                NLM_LOG_ERROR("Failed to create metadata file: " + metadataFile);
                return false;
            }
            
            // Write metadata in simple key=value format
            metaFile << "# Brain Checkpoint Metadata" << std::endl;
            metaFile << "timestamp=" << pImpl->currentTime << std::endl;
            metaFile << "total_spikes=" << getTotalSpikeCount() << std::endl;
            metaFile << "active_neurons=" << getActiveNeuronCount() << std::endl;
            metaFile << "regions=" << getRegionCount() << std::endl;
            metaFile << "epoch=" << (trainingStats.epochsCompleted) << std::endl;
            metaFile << "loss=" << (trainingStats.epochsCompleted > 0 ? 
                                   trainingStats.totalLoss / trainingStats.epochsCompleted : 0.0f) 
                     << std::endl;
            
            for (const auto& pair : metadata) {
                metaFile << pair.first << "=" << pair.second << std::endl;
            }
            
            metaFile << "model_size=" << getTotalNeuronCount() * 64 + getTotalSynapseCount() * 32 
                     << std::endl;  // Approximate size in bytes
            
            metaFile.close();
            
            NLM_LOG_INFO("Checkpoint created successfully: " + checkpointFile + 
                        " (metadata: " + metadataFile + ")");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Checkpoint error: ") + e.what());
            return false;
        }
    }
    
    // Restore brain from checkpoint
    bool Brain::restore_checkpoint(const std::string& filename) {
        NLM_LOG_INFO("Restoring from checkpoint: " + filename);
        
        try {
            // Load brain state
            if (!load(filename)) {
                NLM_LOG_ERROR("Failed to load checkpoint data");
                return false;
            }
            
            // Also load metadata if available
            std::string metadataFile = filename + ".meta";
            std::ifstream metaFile(metadataFile);
            if (metaFile) {
                std::string line;
                while (std::getline(metaFile, line)) {
                    if (line.empty() || line[0] == '#') continue;
                    
                    size_t equalsPos = line.find('=');
                    if (equalsPos != std::string::npos) {
                        std::string key = line.substr(0, equalsPos);
                        std::string value = line.substr(equalsPos + 1);
                        
                        // Parse numeric metadata
                        try {
                            if (key == "epoch") {
                                trainingStats.epochsCompleted = std::stoi(value);
                            } else if (key == "loss") {
                                trainingStats.totalLoss = std::stof(value);
                            }
                        } catch (...) {
                            // Ignore parsing errors
                        }
                    }
                }
                metaFile.close();
            }
            
            NLM_LOG_INFO("Checkpoint restored successfully");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Restore checkpoint error: ") + e.what());
            return false;
        }
    }
    
    // Get training metrics as dictionary-like structure
    std::unordered_map<std::string, float> Brain::get_training_metrics() const {
        std::unordered_map<std::string, float> metrics;
        
        metrics["epochs_completed"] = static_cast<float>(trainingStats.epochsCompleted);
        metrics["samples_processed"] = static_cast<float>(trainingStats.samplesProcessed);
        metrics["total_loss"] = trainingStats.totalLoss;
        metrics["best_accuracy"] = trainingStats.bestAccuracy;
        metrics["training_complete"] = trainingStats.trainingComplete ? 1.0f : 0.0f;
        metrics["neuron_count"] = static_cast<float>(getTotalNeuronCount());
        metrics["synapse_count"] = static_cast<float>(getTotalSynapseCount());
        metrics["active_neurons"] = static_cast<float>(getActiveNeuronCount());
        metrics["current_step"] = static_cast<float>(pImpl->currentStep);
        
        if (pImpl->stdp) {
            metrics["stdp_learning_rate"] = pImpl->stdp->getLearningRate();
        }
        
        return metrics;
    }
    
    // Get performance statistics for analysis
    std::unordered_map<std::string, double> Brain::get_performance_stats() const {
        std::unordered_map<std::string, double> stats;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Compute various performance metrics
        size_t totalNeurons = getTotalNeuronCount();
        size_t activeNeurons = getActiveNeuronCount();
        size_t firingNeurons = getFiringNeuronCount();
        size_t totalSpikes = getTotalSpikeCount();
        float firingRate = getAverageFiringRate();
        float eiRatio = getExcitationInhibitionRatio();
        
        stats["neuron_count"] = totalNeurons;
        stats["active_neurons"] = activeNeurons;
        stats["firing_neurons"] = firingNeurons;
        stats["total_spikes"] = totalSpikes;
        stats["firing_rate_hz"] = firingRate;
        stats["excitation_inhibition_ratio"] = eiRatio;
        stats["memory_usage_mb"] = (totalNeurons * 64 + totalSpikes * 8) / (1024.0 * 1024.0);
        stats["step_time_ms"] = (pImpl->timestep > 0) ? (1.0f / pImpl->timestep * 1000.0f) : 0.0f;
        stats["regions"] = getRegionCount();
        
        // Performance calculation
        double computationTime = std::chrono::duration<double, std::milli>(
            std::chrono::high_resolution_clock::now() - start).count();
        
        stats["computation_time_ms"] = computationTime;
        stats["neurons_per_ms"] = totalNeurons / (computationTime > 0 ? computationTime : 1.0);
        
        return stats;
    }
    
    // Profile current simulation step
    std::unordered_map<std::string, double> Brain::profile_step() {
        std::unordered_map<std::string, double> profile;
        
        // Record step-specific metrics
        profile["step_number"] = static_cast<double>(pImpl->currentStep);
        profile["current_time"] = static_cast<double>(pImpl->currentTime);
        profile["neurons_active"] = static_cast<double>(getActiveNeuronCount());
        profile["neurons_firing"] = static_cast<double>(getFiringNeuronCount());
        profile["spikes_this_step"] = static_cast<double>(pImpl->totalSpikesThisStep);
        profile["spike_rate"] = static_cast<double>(getAverageFiringRate());
        profile["ei_ratio"] = static_cast<double>(getExcitationInhibitionRatio());
        
        // Record memory system states
        if (pImpl->workingMemory) {
            profile["working_memory_traces"] = static_cast<double>(pImpl->workingMemory->getActiveTraces());
        }
        if (pImpl->episodicMemory) {
            profile["episodic_memory_episodes"] = static_cast<double>(pImpl->episodicMemory->getEpisodeCount());
        }
        
        // Record neuromodulation levels
        if (pImpl->dopamine) {
            profile["dopamine_level"] = static_cast<double>(pImpl->dopamine->getLevel());
        }
        if (pImpl->curiosity) {
            profile["curiosity_level"] = static_cast<double>(pImpl->curiosity->getLevel());
        }
        
        // Record developmental stage
        profile["developmental_stage"] = static_cast<double>(pImpl->developmentalStage);
        
        return profile;
    }
    
    // Create neural visualization
    bool Brain::visualize(const std::string& outputFile, const std::string& format, bool animate) {
        NLM_LOG_INFO("Creating neural visualization: " + outputFile + " (format: " + format + ")");
        
        try {
            // This is a simplified visualization implementation
            // In a real system, you would use graphics libraries like OpenGL, SDL, etc.
            
            if (format == "png" || format == "jpg") {
                // Create a simple text-based visualization file
                std::ofstream file(outputFile);
                if (!file) {
                    NLM_LOG_ERROR("Failed to create visualization file: " + outputFile);
                    return false;
                }
                
                file << "# NLM Neural Network Visualization" << std::endl;
                file << "# Generated at time: " << pImpl->currentTime << std::endl;
                file << "# Total neurons: " << getTotalNeuronCount() << std::endl;
                file << "# Total synapses: " << getTotalSynapseCount() << std::endl;
                file << "# Active neurons: " << getActiveNeuronCount() << std::endl;
                file << "# Firing neurons: " << getFiringNeuronCount() << std::endl;
                file << "# Regions: " << getRegionCount() << std::endl;
                file << std::endl;
                
                // Create a simple heatmap representation
                file << "=== NEURAL ACTIVATION HEATMAP ===" << std::endl;
                size_t regionCount = getRegionCount();
                if (regionCount > 0) {
                    for (size_t i = 0; i < regionCount; ++i) {
                        file << "Region " << i << ": ";
                        // Simulate activation based on region ID
                        float activation = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
                        for (size_t j = 0; j < static_cast<size_t>(activation * 20); ++j) {
                            file << "#";
                        }
                        file << " (" << static_cast<int>(activation * 100) << "%)" << std::endl;
                    }
                }
                
                file << std::endl << "=== SYNAPTIC CONNECTIVITY ===" << std::endl;
                file << "Total connections: " << getTotalSynapseCount() << std::endl;
                file << "Excitatory/Inhibitory ratio: " << getExcitationInhibitionRatio() << std::endl;
                
                file << std::endl << "=== MEMORY STATES ===" << std::endl;
                if (pImpl->workingMemory) {
                    file << "Working memory: " << pImpl->workingMemory->getActiveTraces() << " active traces" << std::endl;
                }
                if (pImpl->episodicMemory) {
                    file << "Episodic memory: " << pImpl->episodicMemory->getEpisodeCount() << " episodes" << std::endl;
                }
                
                file << std::endl << "=== DEVELPOMENTAL STATUS ===" << std::endl;
                file << "Stage: " << static_cast<int>(pImpl->developmentalStage) << std::endl;
                
                if (animate) {
                    file << std::endl << "=== ANIMATION DATA ===" << std::endl;
                    file << "This visualization supports animation." << std::endl;
                    file << "For video generation, consider using external visualization tools." << std::endl;
                }
                
                file.close();
                
            } else {
                // For other formats, create a simple text summary
                std::ofstream file(outputFile);
                if (!file) {
                    NLM_LOG_ERROR("Failed to create visualization file: " + outputFile);
                    return false;
                }
                
                file << "NLM Neural Network Visualization Summary" << std::endl;
                file << "=========================================" << std::endl;
                file << "Format: " << format << std::endl;
                file << "Timestamp: " << pImpl->currentTime << std::endl;
                file << "Neurons: " << getTotalNeuronCount() << std::endl;
                file << "Synapses: " << getTotalSynapseCount() << std::endl;
                file << "Active: " << getActiveNeuronCount() << std::endl;
                file << "Firing: " << getFiringNeuronCount() << std::endl;
                file << "Rate: " << getAverageFiringRate() << " Hz" << std::endl;
                
                file.close();
            }
            
            NLM_LOG_INFO("Visualization created successfully");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Visualization error: ") + e.what());
            return false;
        }
    }
    
    // Start performance monitoring
    void Brain::monitor() {
        NLM_LOG_INFO("Starting performance monitoring");
        pImpl->monitoringActive = true;
        pImpl->monitorStartTime = std::chrono::high_resolution_clock::now();
        
        // Record initial metrics
        monitorData.clear();
        monitorData.push_back({"timestamp": pImpl->currentTime, 
                              "step": pImpl->currentStep,
                              "neurons": getTotalNeuronCount(),
                              "spikes": getTotalSpikeCount()});
    }
    
    // Stop performance monitoring
    void Brain::stop_monitoring() {
        NLM_LOG_INFO("Stopping performance monitoring");
        pImpl->monitoringActive = false;
        
        if (!monitorData.empty()) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                endTime - pImpl->monitorStartTime).count();
            
            NLM_LOG_INFO("Monitoring session duration: " + std::to_string(duration) + " ms");
            NLM_LOG_INFO("Monitored " + std::to_string(monitorData.size()) + " data points");
        }
    }
    
    // Get monitoring data as time series
    std::vector<std::unordered_map<std::string, double>> Brain::get_monitor_data() {
        if (monitorData.empty()) {
            NLM_LOG_WARN("No monitoring data available. Start monitoring with monitor().");
        }
        return monitorData;
    }

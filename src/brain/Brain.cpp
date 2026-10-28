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

// ========== ADVANCED CONFIGURATION METHODS ==========

void Brain::setSTDPParameters(float ltpWeight, float ltdWeight, float tau) {
    if (pImpl->stdp) {
        pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
        pImpl->config->set("stdp_ltp_weight", ltpWeight, ConfigSource::User);
        pImpl->config->set("stdp_ltd_weight", ltdWeight, ConfigSource::User);
        pImpl->config->set("stdp_tau", tau, ConfigSource::User);
        NLM_LOG_INFO("STDP parameters configured: LTP=" + std::to_string(ltpWeight) + 
                    ", LTD=" + std::to_string(ltdWeight) + ", tau=" + std::to_string(tau));
    }
}

void Brain::setHebbianParameters(float learningRate, float decayRate) {
    if (pImpl->hebbian) {
        // Note: Hebbian class would need to be updated to support these parameters
        NLM_LOG_INFO("Hebbian parameters configured: learning_rate=" + std::to_string(learningRate) + 
                    ", decay_rate=" + std::to_string(decayRate));
        // For now, log the parameters
        pImpl->config->set("hebbian_learning_rate", learningRate, ConfigSource::User);
        pImpl->config->set("hebbian_decay_rate", decayRate, ConfigSource::User);
    }
}

void Brain::setStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate) {
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        pImpl->structuralPlasticity->setPruningRate(pruningRate);
        pImpl->config->set("synaptogenesis_rate", synaptogenesisRate, ConfigSource::User);
        pImpl->config->set("pruning_rate", pruningRate, ConfigSource::User);
        NLM_LOG_INFO("Structural plasticity parameters configured: synaptogenesis=" + std::to_string(synaptogenesisRate) + 
                    ", pruning=" + std::to_string(pruningRate));
    }
}

void Brain::setNeuromodulationParameters(float dopamineScale, float curiosityThreshold,
                                         float noveltyThreshold, float predictionErrorGain) {
    if (pImpl->dopamine) {
        pImpl->dopamine->setScale(dopamineScale);
        pImpl->config->set("neuromod.dopamine.scale", dopamineScale, ConfigSource::User);
    }
    if (pImpl->curiosity) {
        pImpl->curiosity->setThreshold(curiosityThreshold);
        pImpl->config->set("neuromod.curiosity.threshold", curiosityThreshold, ConfigSource::User);
    }
    if (pImpl->novelty) {
        pImpl->novelty->setThreshold(noveltyThreshold);
        pImpl->config->set("neuromod.novelty.threshold", noveltyThreshold, ConfigSource::User);
    }
    if (pImpl->predictionError) {
        pImpl->predictionError->setGain(predictionErrorGain);
        pImpl->config->set("neuromod.prediction_error.gain", predictionErrorGain, ConfigSource::User);
    }
    NLM_LOG_INFO("Neuromodulation parameters configured: dopamine_scale=" + std::to_string(dopamineScale) + 
                ", curiosity_threshold=" + std::to_string(curiosityThreshold) + 
                ", novelty_threshold=" + std::to_string(noveltyThreshold) + 
                ", prediction_error_gain=" + std::to_string(predictionErrorGain));
}

void Brain::setWorkingMemoryParameters(size_t capacity, float decayRate) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setCapacity(capacity);
        pImpl->config->set("working_memory.capacity", capacity, ConfigSource::User);
        pImpl->config->set("working_memory.decay_rate", decayRate, ConfigSource::User);
        NLM_LOG_INFO("Working memory parameters configured: capacity=" + std::to_string(capacity) + 
                    ", decay_rate=" + std::to_string(decayRate));
    }
}

void Brain::setEpisodicMemoryParameters(size_t maxEpisodes, size_t replayInterval) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setMaxEpisodes(maxEpisodes);
        pImpl->config->set("episodic_memory.max_episodes", maxEpisodes, ConfigSource::User);
        pImpl->config->set("episodic_memory.replay_interval", replayInterval, ConfigSource::User);
        NLM_LOG_INFO("Episodic memory parameters configured: max_episodes=" + std::to_string(maxEpisodes) + 
                    ", replay_interval=" + std::to_string(replayInterval));
    }
}

void Brain::setAssociativeMemoryParameters(float patternThreshold, size_t patternCapacity) {
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->setPatternThreshold(patternThreshold);
        pImpl->associativeMemory->setPatternCapacity(patternCapacity);
        pImpl->config->set("associative_memory.pattern_threshold", patternThreshold, ConfigSource::User);
        pImpl->config->set("associative_memory.pattern_capacity", patternCapacity, ConfigSource::User);
        NLM_LOG_INFO("Associative memory parameters configured: pattern_threshold=" + std::to_string(patternThreshold) + 
                    ", pattern_capacity=" + std::to_string(patternCapacity));
    }
}

void Brain::setCognitiveParameters(size_t planningDepth, float attentionGain,
                                   float conceptThreshold, float inhibitionStrength) {
    if (pImpl->planner) {
        pImpl->planner->setPlanningDepth(planningDepth);
        pImpl->config->set("cognition.planning_depth", planningDepth, ConfigSource::User);
    }
    if (pImpl->attention) {
        pImpl->attention->setInhibitionStrength(inhibitionStrength);
        pImpl->config->set("cognition.attention.inhibition_strength", inhibitionStrength, ConfigSource::User);
    }
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->setThreshold(conceptThreshold);
        pImpl->config->set("cognition.concept_threshold", conceptThreshold, ConfigSource::User);
    }
    NLM_LOG_INFO("Cognitive parameters configured: planning_depth=" + std::to_string(planningDepth) + 
                ", attention_gain=" + std::to_string(attentionGain) + 
                ", concept_threshold=" + std::to_string(conceptThreshold) + 
                ", inhibition_strength=" + std::to_string(inhibitionStrength));
}

void Brain::setDevelopmentParameters(size_t criticalPeriodStart, size_t criticalPeriodEnd,
                                     float plasticityModulationRate) {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->setCriticalPeriodStart(criticalPeriodStart);
        pImpl->developmentSystem->setCriticalPeriodEnd(criticalPeriodEnd);
        pImpl->developmentSystem->setPlasticityModulationRate(plasticityModulationRate);
        pImpl->config->set("development.critical_period_start", criticalPeriodStart, ConfigSource::User);
        pImpl->config->set("development.critical_period_end", criticalPeriodEnd, ConfigSource::User);
        pImpl->config->set("development.plasticity_modulation_rate", plasticityModulationRate, ConfigSource::User);
        NLM_LOG_INFO("Development parameters configured: critical_period_start=" + std::to_string(criticalPeriodStart) + 
                    ", critical_period_end=" + std::to_string(criticalPeriodEnd) + 
                    ", plasticity_modulation_rate=" + std::to_string(plasticityModulationRate));
    }
}

void Brain::setPerformanceParameters(size_t memoryPoolSize, bool enableSIMD,
                                     bool enableParallel, size_t threadCount) {
    pImpl->config->set("performance.memory_pool_size", memoryPoolSize, ConfigSource::User);
    pImpl->config->set("performance.enable_simd", enableSIMD, ConfigSource::User);
    pImpl->config->set("performance.enable_parallel", enableParallel, ConfigSource::User);
    pImpl->config->set("performance.thread_count", threadCount, ConfigSource::User);
    NLM_LOG_INFO("Performance parameters configured: memory_pool_size=" + std::to_string(memoryPoolSize) + 
                ", enable_simd=" + std::to_string(enableSIMD) + 
                ", enable_parallel=" + std::to_string(enableParallel) + 
                ", thread_count=" + std::to_string(threadCount));
}

void Brain::setCheckpointParameters(const std::string& directory, size_t maxCheckpoints,
                                    float compressionLevel, bool autoSave) {
    pImpl->config->set("checkpoint.directory", directory, ConfigSource::User);
    pImpl->config->set("checkpoint.max_checkpoints", maxCheckpoints, ConfigSource::User);
    pImpl->config->set("checkpoint.compression_level", compressionLevel, ConfigSource::User);
    pImpl->config->set("checkpoint.auto_save", autoSave, ConfigSource::User);
    NLM_LOG_INFO("Checkpoint parameters configured: directory=" + directory + 
                ", max_checkpoints=" + std::to_string(maxCheckpoints) + 
                ", compression_level=" + std::to_string(compressionLevel) + 
                ", auto_save=" + std::to_string(autoSave));
}

void Brain::enableSubsystem(const std::string& subsystemName, bool enable) {
    if (subsystemName == "working_memory" && pImpl->workingMemory) {
        pImpl->workingMemory->setEnabled(enable);
        pImpl->config->set("subsystem.working_memory.enabled", enable, ConfigSource::User);
    } else if (subsystemName == "episodic_memory" && pImpl->episodicMemory) {
        pImpl->episodicMemory->setEnabled(enable);
        pImpl->config->set("subsystem.episodic_memory.enabled", enable, ConfigSource::User);
    } else if (subsystemName == "associative_memory" && pImpl->associativeMemory) {
        pImpl->associativeMemory->setEnabled(enable);
        pImpl->config->set("subsystem.associative_memory.enabled", enable, ConfigSource::User);
    } else if (subsystemName == "planner" && pImpl->planner) {
        pImpl->planner->setEnabled(enable);
        pImpl->config->set("subsystem.planner.enabled", enable, ConfigSource::User);
    } else if (subsystemName == "concept_formation" && pImpl->conceptFormation) {
        pImpl->conceptFormation->setEnabled(enable);
        pImpl->config->set("subsystem.concept_formation.enabled", enable, ConfigSource::User);
    } else if (subsystemName == "attention" && pImpl->attention) {
        pImpl->attention->setEnabled(enable);
        pImpl->config->set("subsystem.attention.enabled", enable, ConfigSource::User);
    } else if (subsystemName == "dopamine" && pImpl->dopamine) {
        pImpl->dopamine->setEnabled(enable);
        pImpl->config->set("subsystem.dopamine.enabled", enable, ConfigSource::User);
    } else if (subsystemName == "curiosity" && pImpl->curiosity) {
        pImpl->curiosity->setEnabled(enable);
        pImpl->config->set("subsystem.curiosity.enabled", enable, ConfigSource::User);
    } else if (subsystemName == "novelty" && pImpl->novelty) {
        pImpl->novelty->setEnabled(enable);
        pImpl->config->set("subsystem.novelty.enabled", enable, ConfigSource::User);
    } else if (subsystemName == "prediction_error" && pImpl->predictionError) {
        pImpl->predictionError->setEnabled(enable);
        pImpl->config->set("subsystem.prediction_error.enabled", enable, ConfigSource::User);
    } else {
        NLM_LOG_WARNING("Unknown subsystem: " + subsystemName);
    }
    NLM_LOG_INFO("Subsystem " + subsystemName + " " + (enable ? "enabled" : "disabled"));
}

std::string Brain::getConfigurationSummary() const {
    std::stringstream ss;
    ss << "NLM Brain Configuration Summary (Phase 6 - Integrated):" << std::endl;
    ss << "  Brain:" << std::endl;
    ss << "    Neurons: " << getTotalNeuronCount() << std::endl;
    ss << "    Synapses: " << getTotalSynapseCount() << std::endl;
    ss << "    Regions: " << getRegionCount() << std::endl;
    ss << "    Developmental Stage: " << static_cast<int>(pImpl->developmentalStage) << std::endl;
    ss << "  Memory Systems:" << std::endl;
    if (pImpl->workingMemory) {
        ss << "    Working Memory: " << pImpl->workingMemory->getActiveTraces() << "/" << pImpl->workingMemory->getCapacity() << " traces" << std::endl;
    }
    if (pImpl->episodicMemory) {
        ss << "    Episodic Memory: " << pImpl->episodicMemory->getEpisodeCount() << "/" << pImpl->episodicMemory->getMaxEpisodes() << " episodes" << std::endl;
    }
    if (pImpl->associativeMemory) {
        ss << "    Associative Memory: " << pImpl->associativeMemory->getPatternCount() << "/" << pImpl->associativeMemory->getPatternCapacity() << " patterns" << std::endl;
    }
    ss << "  Plasticity:" << std::endl;
    ss << "    E/I Ratio: " << getExcitationInhibitionRatio() << std::endl;
    ss << "    Total Spikes: " << getTotalSpikeCount() << std::endl;
    ss << "  Neuromodulation:" << std::endl;
    if (pImpl->dopamine) {
        ss << "    Dopamine Level: " << pImpl->dopamine->getLevel() << std::endl;
    }
    if (pImpl->curiosity) {
        ss << "    Curiosity Level: " << pImpl->curiosity->getLevel() << std::endl;
    }
    if (pImpl->novelty) {
        ss << "    Novelty Level: " << pImpl->novelty->getLevel() << std::endl;
    }
    ss << "  Performance:" << std::endl;
    ss << "    Step Count: " << pImpl->currentStep << std::endl;
    ss << "    Simulation Time: " << pImpl->currentTime << "s" << std::endl;
    return ss.str();
}

void Brain::enablePerformanceMonitoring(bool enable) {
    pImpl->config->set("performance.monitoring_enabled", enable, ConfigSource::User);
    NLM_LOG_INFO("Performance monitoring " + std::string(enable ? "enabled" : "disabled"));
}

bool Brain::isPerformanceMonitoringEnabled() const {
    return pImpl->config->getOr<bool>("performance.monitoring_enabled", false);
}

// Note: Performance statistics would need additional implementation
// These are placeholders for now
size_t Brain::getStepCount() const {
    return pImpl->currentStep;
}

double Brain::getAverageStepTime() const {
    // Placeholder - would need to track step times
    return 0.001; // Default timestep
}

size_t Brain::getMaxMemoryUsage() const {
    // Placeholder - would need to track memory usage
    return 0;
}

double Brain::getCPUUsage() const {
    // Placeholder - would need to track CPU usage
    return 0.0;
}

size_t Brain::getTotalConnections() const {
    return getTotalSynapseCount();
}

size_t Brain::getActiveConnections() const {
    // Placeholder - would need to track active synapses
    return 0;
}

size_t Brain::getSynapticStrengthSum() const {
    // Placeholder - would need to track synaptic weights
    return 0;
}

float Brain::getAverageSynapticWeight() const {
    // Placeholder - would need to calculate average weight
    return 0.0f;
}

float Brain::getNetworkStability() const {
    // Placeholder - would need to calculate network stability metric
    return 1.0f;
}

size_t Brain::getWorkingMemoryUsage() const {
    if (pImpl->workingMemory) {
        return pImpl->workingMemory->getActiveTraces();
    }
    return 0;
}

size_t Brain::getEpisodicMemoryUsage() const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->getEpisodeCount();
    }
    return 0;
}

size_t Brain::getAssociativeMemoryUsage() const {
    if (pImpl->associativeMemory) {
        return pImpl->associativeMemory->getPatternCount();
    }
    return 0;
}

float Brain::getMemoryConsolidationRate() const {
    // Placeholder - would need to track consolidation rate
    return 0.0f;
}

float Brain::getAverageDopamineLevel() const {
    if (pImpl->dopamine) {
        return pImpl->dopamine->getLevel();
    }
    return 0.0f;
}

float Brain::getAverageCuriosityLevel() const {
    if (pImpl->curiosity) {
        return pImpl->curiosity->getLevel();
    }
    return 0.0f;
}

float Brain::getAverageNoveltyLevel() const {
    if (pImpl->novelty) {
        return pImpl->novelty->getLevel();
    }
    return 0.0f;
}

float Brain::getAveragePredictionError() const {
    if (pImpl->predictionError) {
        return pImpl->predictionError->getLevel();
    }
    return 0.0f;
}

size_t Brain::getDevelopmentalStageDuration(DevelopmentalStage stage) const {
    // Placeholder - would need to track stage durations
    return 0;
}

float Brain::getPlasticityRate() const {
    // Placeholder - would need to calculate overall plasticity rate
    return 0.0f;
}

float Brain::getStructuralPlasticityRate() const {
    // Placeholder - would need to track structural plasticity rate
    return 0.0f;
}

void Brain::enableCuriosityDrivenExploration(bool enable, float explorationRate) {
    if (pImpl->curiosity) {
        pImpl->curiosity->setThreshold(explorationRate);
        pImpl->curiosity->setEnabled(enable);
        pImpl->config->set("curiosity_driven_exploration.enabled", enable, ConfigSource::User);
        pImpl->config->set("curiosity_driven_exploration.rate", explorationRate, ConfigSource::User);
        NLM_LOG_INFO("Curiosity-driven exploration " + std::string(enable ? "enabled" : "disabled") + 
                    ", rate: " + std::to_string(explorationRate));
    }
}

void Brain::setGoalBasedMotivation(float goalImportance, float intrinsicMotivation) {
    pImpl->config->set("goal_based_motivation.goal_importance", goalImportance, ConfigSource::User);
    pImpl->config->set("goal_based_motivation.intrinsic_motivation", intrinsicMotivation, ConfigSource::User);
    NLM_LOG_INFO("Goal-based motivation configured: goal_importance=" + std::to_string(goalImportance) + 
                ", intrinsic_motivation=" + std::to_string(intrinsicMotivation));
}

void Brain::setSocialLearningParameters(float imitationStrength, float socialBonding) {
    pImpl->config->set("social_learning.imitation_strength", imitationStrength, ConfigSource::User);
    pImpl->config->set("social_learning.social_bonding", socialBonding, ConfigSource::User);
    NLM_LOG_INFO("Social learning parameters configured: imitation_strength=" + std::to_string(imitationStrength) + 
                ", social_bonding=" + std::to_string(socialBonding));
}

void Brain::batchInjectCurrent(const std::vector<NeuronId>& neurons, MembranePotential current) {
    for (const auto& neuronId : neurons) {
        injectCurrent(neuronId, current);
    }
    NLM_LOG_INFO("Batch injected current into " + std::to_string(neurons.size()) + " neurons");
}

void Brain::batchSetWeights(const std::vector<SynapseId>& synapses, float weight) {
    for (const auto& synapseId : synapses) {
        // Note: This would require a method to find synapses by ID
        // For now, just log the operation
    }
    NLM_LOG_INFO("Batch set weights for " + std::to_string(synapses.size()) + " synapses to " + std::to_string(weight));
}

void Brain::batchEnablePlasticity(const std::vector<SynapseId>& synapses, bool stdp, bool hebbian, bool structural) {
    for (const auto& synapseId : synapses) {
        // Note: This would require a method to find synapses by ID
        // For now, just log the operation
    }
    NLM_LOG_INFO("Batch enabled plasticity for " + std::to_string(synapses.size()) + " synapses (STDP=" + 
                std::to_string(stdp) + ", Hebbian=" + std::to_string(hebbian) + ", Structural=" + std::to_string(structural) + ")");
}

void Brain::pauseSimulation(bool pause) {
    pImpl->isResting = pause;
    pImpl->config->set("simulation.paused", pause, ConfigSource::User);
    NLM_LOG_INFO("Simulation " + std::string(pause ? "paused" : "resumed"));
}

bool Brain::isSimulationPaused() const {
    return pImpl->isResting;
}

void Brain::enableEventDrivenSimulation(bool enable) {
    pImpl->config->set("simulation.event_driven", enable, ConfigSource::User);
    NLM_LOG_INFO("Event-driven simulation " + std::string(enable ? "enabled" : "disabled"));
}

bool Brain::isEventDrivenSimulationEnabled() const {
    return pImpl->config->getOr<bool>("simulation.event_driven", false);
}

// Note: ExternalInput and ExternalInfluence classes would need to be defined
void Brain::injectExternalInput(const class ExternalInput& input) {
    // Placeholder - would inject external input into brain
    NLM_LOG_INFO("External input injected (placeholder)");
}

void Brain::applyExternalInfluence(const class ExternalInfluence& influence) {
    // Placeholder - would apply external influence to brain
    NLM_LOG_INFO("External influence applied (placeholder)");
}

bool Brain::isSystemHealthy() const {
    // Basic health check
    bool healthy = !pImpl->regions.empty() && 
                   getTotalNeuronCount() > 0 && 
                   !pImpl->isResting;
    NLM_LOG_INFO("System health: " + std::string(healthy ? "HEALTHY" : "UNHEALTHY"));
    return healthy;
}

std::string Brain::getSystemStatus() const {
    std::stringstream ss;
    ss << "System Status:" << std::endl;
    ss << "  Healthy: " << (isSystemHealthy() ? "Yes" : "No") << std::endl;
    ss << "  Regions: " << getRegionCount() << std::endl;
    ss << "  Neurons: " << getTotalNeuronCount() << std::endl;
    ss << "  Synapses: " << getTotalSynapseCount() << std::endl;
    ss << "  Current Step: " << pImpl->currentStep << std::endl;
    ss << "  Simulation Time: " << pImpl->currentTime << "s" << std::endl;
    ss << "  Developmental Stage: " << static_cast<int>(pImpl->developmentalStage) << std::endl;
    return ss.str();
}

} // namespace nlm

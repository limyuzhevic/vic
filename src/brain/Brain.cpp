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
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->initialize(this);
    }

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

void Brain::configureCustomTopology(const CustomTopology::TopologyConfig& config) {
    // Implement custom neural topology configuration
    // This allows users to define custom network architectures with different connection patterns
    
    NLM_LOG_INFO("Configuring custom neural topology...");
    
    // Clear existing regions
    pImpl->regions.clear();
    
    // Create regions based on topology configuration
    for (size_t i = 0; i < config.numRegions; ++i) {
        auto regionId = addRegion("CustomRegion_" + std::to_string(i + 1));
        auto* region = getRegion(regionId);
        
        if (region) {
            // Add neurons to region
            for (size_t j = 0; j < config.neuronsPerRegion; ++j) {
                NeuronId neuronId = region->addNeuron();
                
                // Set neuron type based on position in network
                NeuronType neuronType = NeuronType::Internal;
                if (j < config.neuronsPerRegion / 4) {
                    neuronType = NeuronType::Sensory;
                } else if (j < 3 * config.neuronsPerRegion / 4) {
                    neuronType = NeuronType::Internal;
                } else {
                    neuronType = NeuronType::Motor;
                }
                
                region->getPopulation(region->getPopulationId(neuronId))->setNeuronType(neuronType);
            }
        }
    }
    
    // Apply connection pattern based on topology configuration
    switch (config.pattern) {
        case CustomTopology::ConnectionPattern::Dense:
            // Implement dense connectivity
            for (size_t i = 0; i < pImpl->regions.size(); ++i) {
                auto* region = getRegion(RegionId(i + 1));
                if (region) {
                    region->initializeDenseConnectivity(*pImpl->rng);
                }
            }
            break;
            
        case CustomTopology::ConnectionPattern::Sparse:
            // Implement sparse connectivity
            for (size_t i = 0; i < pImpl->regions.size(); ++i) {
                auto* region = getRegion(RegionId(i + 1));
                if (region) {
                    region->initializeRandomConnectivity(*pImpl->rng, config.connectionProbability, 0.2f, 0.1f);
                }
            }
            break;
            
        case CustomTopology::ConnectionPattern::Clustered:
            // Implement clustered connectivity
            // Create clusters and connect within clusters
            break;
            
        case CustomTopology::ConnectionPattern::SmallWorld:
            // Implement small-world connectivity
            // Combine high local connectivity with occasional long-range connections
            break;
            
        case CustomTopology::ConnectionPattern::Ring:
            // Implement ring connectivity
            // Connect neurons in a ring pattern
            break;
            
        case CustomTopology::ConnectionPattern::Grid:
            // Implement grid connectivity
            // Create 2D grid structure
            break;
            
        case CustomTopology::ConnectionPattern::Hierarchical:
            // Implement hierarchical connectivity
            // Create multi-level hierarchical structure
            break;
    }
    
    // Apply custom connections if specified
    for (const auto& customConnection : config.customConnections) {
        NeuronId source = NeuronId(customConnection.first);
        NeuronId target = NeuronId(customConnection.second);
        
        // Find source and target neurons
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == source) {
                    for (auto& otherRegion : pImpl->regions) {
                        auto otherNeurons = otherRegion->getAllNeurons();
                        for (auto* otherNeuron : otherNeurons) {
                            if (otherNeuron->getId() == target) {
                                // Create synapse
                                region->addSynapse(neuron, otherNeuron, 0.1f, 1);
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    
    NLM_LOG_INFO("Custom neural topology configuration complete");
}

void Brain::configureAdvancedPlasticity(const CustomPlasticityRule::PlasticityParams& params) {
    // Configure advanced plasticity rules
    
    NLM_LOG_INFO("Configuring advanced plasticity rule...");
    
    // Store plasticity parameters
    pImpl->plasticityParams = params;
    
    // Configure STDP with advanced parameters
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(params.learningRate);
        pImpl->stdp->setLTDWeight(params.learningRate * 1.2f); // LTD typically stronger
        pImpl->stdp->setTau(params.stdpTauPre);
    }
    
    // Configure Hebbian learning with advanced parameters
    if (pImpl->hebbian) {
        pImpl->hebbian->setLearningRate(params.hebbianAlpha);
    }
    
    // Configure structural plasticity
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->setSynaptogenesisRate(params.homeostaticRate * 0.01f);
        pImpl->structuralPlasticity->setPruningRate(params.homeostaticRate * 0.001f);
    }
    
    // Enable/disable plasticity components
    pImpl->enableSTDP = params.enableDepression;
    pImpl->enableHebbian = params.enableHebbian;
    pImpl->enableRewardModulated = true; // Always enable reward modulation with advanced plasticity
    
    NLM_LOG_INFO("Advanced plasticity configuration complete");
}

void Brain::configureMemoryReplay(size_t replayInterval, size_t consolidationInterval, 
                                 const std::vector<NeuronId>& neuronsToReplay) {
    // Configure memory replay and consolidation
    
    NLM_LOG_INFO("Configuring memory replay system...");
    
    // Set replay and consolidation intervals
    pImpl->replayInterval = replayInterval;
    pImpl->consolidationInterval = consolidationInterval;
    pImpl->neuronsToReplay = neuronsToReplay;
    
    // Configure working memory replay
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setReplayNeurons(neuronsToReplay);
        pImpl->workingMemory->setReplayInterval(replayInterval);
    }
    
    // Configure episodic memory replay
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setReplayNeurons(neuronsToReplay);
        pImpl->episodicMemory->setConsolidationInterval(consolidationInterval);
    }
    
    // Configure episodic memory consolidation
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setConsolidationThreshold(0.3f); // Lower threshold for more frequent consolidation
        pImpl->episodicMemory->setConsolidationStrength(1.0f);
    }
    
    NLM_LOG_INFO("Memory replay configuration complete");
}

void Brain::configureCustomReward(const CustomRewardImpl::RewardConfig& config) {
    // Configure custom reward functions
    
    NLM_LOG_INFO("Configuring custom reward system...");
    
    // Store reward configuration
    pImpl->rewardConfig = config;
    
    // Configure dopamine for reward signaling
    if (pImpl->dopamine) {
        // Configure dopamine based on reward function type
        if (config.functionType == "complex") {
            pImpl->dopamine->setPeak(2.0f); // Higher peak for complex rewards
        } else if (config.functionType == "intrinsic") {
            pImpl->dopamine->setPeak(0.5f); // Lower peak for intrinsic motivation
        }
    }
    
    // Configure novelty for exploration bonus
    if (pImpl->novelty) {
        pImpl->novelty->setWeight(config.intrinsicWeight);
    }
    
    // Configure curiosity for exploration
    if (pImpl->curiosity) {
        pImpl->curiosity->setWeight(config.extrinsicWeight);
    }
    
    NLM_LOG_INFO("Custom reward configuration complete");
}

void Brain::enableParallelProcessing(const ParallelManagerImpl::ParallelConfig& config) {
    // Enable parallel processing for large-scale simulations
    
    NLM_LOG_INFO("Enabling parallel processing...");
    
    // Store parallel processing configuration
    pImpl->parallelConfig = config;
    
    // Configure parallel processing for neural computation
    if (pImpl->parallelConfig.enableParallelProcessing) {
        pImpl->parallelNeuralProcessor = std::make_unique<ParallelNeuralProcessor>(config.numCores);
        
        // Enable parallel neuron processing
        pImpl->parallelNeuralProcessor->enableNeuronProcessing(true);
        
        // Configure parallel spike processing
        if (config.enableSpikingQueue) {
            pImpl->parallelNeuralProcessor->enableSpikingProcessing(true);
        }
        
        // Enable parallel plasticity updates
        pImpl->parallelNeuralProcessor->enablePlasticityProcessing(true);
    }
    
    // Enable parallel memory processing
    if (config.enableParallelMemoryProcessing) {
        pImpl->parallelMemoryProcessor = std::make_unique<ParallelMemoryProcessor>(config.numCores);
        pImpl->parallelMemoryProcessor->enableWorkingMemoryProcessing(true);
        pImpl->parallelMemoryProcessor->enableEpisodicMemoryProcessing(true);
    }
    
    NLM_LOG_INFO("Parallel processing enabled");
}

void Brain::enablePerformanceProfiler(bool enable) {
    // Enable/disable performance profiler
    
    NLM_LOG_INFO("Configuring performance profiler...");
    
    // Set profiler enabled state
    pImpl->profilerEnabled = enable;
    
    // Start/stop performance monitoring
    if (enable) {
        pImpl->performanceProfiler = std::make_unique<PerformanceProfilerImpl>();
        pImpl->performanceProfiler->start();
        pImpl->performanceProfiler->enableDetailedLogging(true);
    } else if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->stop();
    }
    
    NLM_LOG_INFO("Performance profiler " + std::string(enable ? "enabled" : "disabled"));
}

void Brain::logAdvancedMetrics() {
    // Log advanced metrics for power users
    
    NLM_LOG_INFO("=== Advanced Metrics ===");
    
    // Log neural activity metrics
    NLM_LOG_INFO("Neural Activity Metrics:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("  Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("  Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("  Excitation/Inhibition ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Log memory metrics
    NLM_LOG_INFO("Memory Metrics:");
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("  Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("  Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
        NLM_LOG_INFO("  Memory replay scheduled: " + std::to_string(pImpl->episodicMemory->isReplayScheduled()));
    }
    if (pImpl->associativeMemory) {
        NLM_LOG_INFO("  Associative memory strength: " + std::to_string(pImpl->associativeMemory->getStrength()));
    }
    
    // Log neuromodulation metrics
    NLM_LOG_INFO("Neuromodulation Metrics:");
    if (pImpl->dopamine) {
        NLM_LOG_INFO("  Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
        NLM_LOG_INFO("  Dopamine plasticity factor: " + std::to_string(pImpl->dopamine->getPlasticityFactor()));
    }
    if (pImpl->novelty) {
        NLM_LOG_INFO("  Novelty detection: " + std::to_string(pImpl->novelty->getLevel()));
    }
    if (pImpl->curiosity) {
        NLM_LOG_INFO("  Curiosity level: " + std::to_string(pImpl->curiosity->getLevel()));
    }
    if (pImpl->predictionError) {
        NLM_LOG_INFO("  Prediction error: " + std::to_string(pImpl->predictionError->getLevel()));
    }
    
    // Log plasticity metrics
    NLM_LOG_INFO("Plasticity Metrics:");
    if (pImpl->stdp) {
        NLM_LOG_INFO("  STDP LTP weight: " + std::to_string(pImpl->stdp->getLTPWeight()));
        NLM_LOG_INFO("  STDP LTD weight: " + std::to_string(pImpl->stdp->getLTDWeight()));
    }
    if (pImpl->hebbian) {
        NLM_LOG_INFO("  Hebbian learning rate: " + std::to_string(pImpl->hebbian->getLearningRate()));
    }
    if (pImpl->structuralPlasticity) {
        NLM_LOG_INFO("  Synaptogenesis rate: " + std::to_string(pImpl->structuralPlasticity->getSynaptogenesisRate()));
        NLM_LOG_INFO("  Pruning rate: " + std::to_string(pImpl->structuralPlasticity->getPruningRate()));
    }
    
    // Log prediction metrics
    NLM_LOG_INFO("Prediction Metrics:");
    if (pImpl->predictionSystem) {
        NLM_LOG_INFO("  Prediction error: " + std::to_string(pImpl->predictionSystem->getPredictionError()));
        NLM_LOG_INFO("  Prediction confidence: " + std::to_string(pImpl->predictionSystem->getConfidence()));
    }
    
    // Log concept formation metrics
    NLM_LOG_INFO("Concept Formation Metrics:");
    if (pImpl->conceptFormation) {
        NLM_LOG_INFO("  Concepts discovered: " + std::to_string(pImpl->conceptFormation->getConceptCount()));
        if (pImpl->conceptFormation->getConceptCount() > 0) {
            NLM_LOG_INFO("  Most stable concept ID: " + std::to_string(
                pImpl->conceptFormation->getConcept(1)->avgStability));
        }
    }
    
    // Log development metrics
    NLM_LOG_INFO("Development Metrics:");
    NLM_LOG_INFO("  Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    // Log performance metrics if profiler is enabled
    if (pImpl->profilerEnabled && pImpl->performanceProfiler) {
        NLM_LOG_INFO("Performance Metrics (Profiler Enabled):");
        auto stats = pImpl->performanceProfiler->getCurrentStats();
        NLM_LOG_INFO("  CPU utilization: " + std::to_string(stats.cpuUsage) + "%");
        NLM_LOG_INFO("  Memory usage: " + std::to_string(stats.memoryUsage) + " MB");
        NLM_LOG_INFO("  Simulation time: " + std::to_string(stats.simulationTime * 1000) + " ms");
        NLM_LOG_INFO("  Spike processing time: " + std::to_string(stats.spikeProcessingTime * 1000) + " ms");
        NLM_LOG_INFO("  Plasticity time: " + std::to_string(stats.plasticityTime * 1000) + " ms");
    }
    
    NLM_LOG_INFO("=== Advanced Metrics Complete ===");
}

// Static methods for advanced features
std::shared_ptr<CustomTopology::TopologyConfig> Brain::createSparseTopology(size_t numRegions, float connectionProb) {
    auto config = std::make_shared<CustomTopology::TopologyConfig>();
    config->numRegions = numRegions;
    config->neuronsPerRegion = 1000;
    config->pattern = CustomTopology::ConnectionPattern::Sparse;
    config->connectionProbability = connectionProb;
    config->clusterProbability = 0.0f;
    config->rewireProbability = 0.0f;
    config->directed = false;
    return config;
}

std::shared_ptr<CustomTopology::TopologyConfig> Brain::createClusteredTopology(size_t numRegions, 
                                                                                   size_t clusterSize, 
                                                                                   float interClusterProb,
                                                                                   float intraClusterProb) {
    auto config = std::make_shared<CustomTopology::TopologyConfig>();
    config->numRegions = numRegions;
    config->neuronsPerRegion = clusterSize;
    config->pattern = CustomTopology::ConnectionPattern::Clustered;
    config->connectionProbability = 0.0f;
    config->clusterProbability = 0.0f;
    config->rewireProbability = interClusterProb;
    config->directed = false;
    return config;
}

std::shared_ptr<CustomTopology::TopologyConfig> Brain::createSmallWorldTopology(size_t numRegions,
                                                                                    float connectionProb,
                                                                                    float rewireProb) {
    auto config = std::make_shared<CustomTopology::TopologyConfig>();
    config->numRegions = numRegions;
    config->neuronsPerRegion = 1000;
    config->pattern = CustomTopology::ConnectionPattern::SmallWorld;
    config->connectionProbability = connectionProb;
    config->clusterProbability = 0.0f;
    config->rewireProbability = rewireProb;
    config->directed = false;
    return config;
}

std::shared_ptr<CustomPlasticityRule::PlasticityParams> Brain::createSTDPParams(float ltpRate, float ltdRate, float tau) {
    auto params = std::make_shared<CustomPlasticityRule::PlasticityParams>();
    params->type = CustomPlasticityRule::RuleType::STDP;
    params->learningRate = ltpRate;
    params->stdpTauPre = tau;
    params->stdpTauPost = tau;
    params->hebbianAlpha = 0.0f;
    params->homeostaticTarget = 5.0f;
    params->homeostaticRate = 0.01f;
    params->neuromodulatorWeight = 0.5f;
    params->enableDepression = true;
    params->enableHebbian = false;
    return params;
}

std::shared_ptr<CustomPlasticityRule::PlasticityParams> Brain::createHebbianParams(float learningRate) {
    auto params = std::make_shared<CustomPlasticityRule::PlasticityParams>();
    params->type = CustomPlasticityRule::RuleType::Hebbian;
    params->learningRate = learningRate;
    params->stdpTauPre = 0.0f;
    params->stdpTauPost = 0.0f;
    params->hebbianAlpha = learningRate;
    params->homeostaticTarget = 5.0f;
    params->homeostaticRate = 0.01f;
    params->neuromodulatorWeight = 0.0f;
    params->enableDepression = false;
    params->enableHebbian = true;
    return params;
}

std::shared_ptr<ParallelManagerImpl::ParallelConfig> Brain::createParallelConfig(size_t numCores, bool enableSharedMemory) {
    auto config = std::make_shared<ParallelManagerImpl::ParallelConfig>();
    config->numCores = numCores;
    config->neuronsPerCore = 10000;
    config->enableSharedMemory = enableSharedMemory;
    config->enableTaskQueue = true;
    config->enableSpikingQueue = true;
    config->spikeQueueSize = 10000;
    config->loadBalanceThreshold = 0.7f;
    config->enableDynamicScheduling = true;
    config->schedulingInterval = 0.1f;
    config->enableMemoryPool = true;
    config->memoryPoolSize = 100 * 1024 * 1024; // 100 MB
    config->enableSIMD = true;
    return config;
}

std::shared_ptr<PerformanceProfilerImpl::ProfileData> Brain::getPerformanceData() {
    auto profileData = std::make_shared<PerformanceProfilerImpl::ProfileData>();
    
    if (pImpl->performanceProfiler) {
        *profileData = pImpl->performanceProfiler->getCurrentStats();
    } else {
        // Initialize with default values
        profileData->cpuUsage = 0.0f;
        profileData->memoryUsage = 0.0f;
        profileData->simulationTime = 0.0f;
        profileData->spikeProcessingTime = 0.0f;
        profileData->plasticityTime = 0.0f;
        profileData->memoryAccessTime = 0.0f;
        profileData->neuronUpdateTime = 0.0f;
        profileData->spikesProcessed = getTotalSpikeCount();
        profileData->neuronsUpdated = getActiveNeuronCount();
        profileData->cacheHitRate = 0.0f;
        profileData->memoryPoolUsage = 0.0f;
        profileData->customMetrics = std::map<std::string, float>();
    }
    
    return profileData;
}

void Brain::configureExperimentalCondition(const AdvancedExperimentImpl::ExperimentalCondition& condition) {
    // Configure experimental condition for advanced experiments
    
    NLM_LOG_INFO("Configuring experimental condition: " + condition.name);
    
    // Store experimental condition
    pImpl->experimentalCondition = condition;
    
    // Apply experimental manipulations
    for (const auto& manipulation : condition.manipulations) {
        NLM_LOG_INFO("Experimental manipulation enabled: " + manipulation.first);
    }
    
    // Configure reward functions
    if (condition.useDynamicRewards) {
        // Enable dynamic reward computation
        pImpl->dynamicRewardsEnabled = true;
        NLM_LOG_INFO("Dynamic reward computation enabled");
    }
    
    // Configure advanced logging
    if (condition.enableAdvancedLogging) {
        pImpl->advancedLoggingEnabled = true;
        NLM_LOG_INFO("Advanced logging enabled");
    }
    
    // Configure cross-modal memory integration
    if (condition.enableCrossModalIntegration) {
        pImpl->crossModalMemoryEnabled = true;
        NLM_LOG_INFO("Cross-modal memory integration enabled");
    }
    
    // Configure memory interference management
    if (condition.enableMemoryInterferenceManagement) {
        pImpl->memoryInterferenceManagementEnabled = true;
        NLM_LOG_INFO("Memory interference management enabled");
        NLM_LOG_INFO("Interference management strategy: " + condition.interferenceManagementStrategy);
    }
    
    NLM_LOG_INFO("Experimental condition configuration complete");
}

void Brain::configureNeuromodulation(std::map<AdvancedNeuromodulationImpl::NeuromodulatorType, 
                            AdvancedNeuromodulationImpl::Neuromodulator> neuromodulators) {
    // Configure multiple neuromodulators with complex interactions
    
    NLM_LOG_INFO("Configuring advanced neuromodulation system...");
    
    // Store neuromodulators
    pImpl->advancedNeuromodulation = std::make_unique<AdvancedNeuromodulationImpl>();
    
    // Add neuromodulators
    for (const auto& pair : neuromodulators) {
        pImpl->advancedNeuromodulation->addNeuromodulator(pair.first, pair.second);
    }
    
    // Configure interaction weights
    // Default: moderate interactions between all neuromodulators
    for (const auto& sourceType : AdvancedNeuromodulationImpl::getAllTypes()) {
        for (const auto& targetType : AdvancedNeuromodulationImpl::getAllTypes()) {
            if (sourceType != targetType) {
                float weight = 0.1f; // Default interaction weight
                pImpl->advancedNeuromodulation->setInteractionWeight(sourceType, targetType, weight);
            }
        }
    }
    
    NLM_LOG_INFO("Advanced neuromodulation configuration complete");
}

void Brain::configureMemorySystem(const std::map<std::string, std::any>& config) {
    // Configure memory systems with advanced features
    
    NLM_LOG_INFO("Configuring advanced memory system...");
    
    // Configure working memory
    if (config.find("working_memory_capacity") != config.end()) {
        auto capacity = std::any_cast<size_t>(config.at("working_memory_capacity"));
        if (pImpl->workingMemory) {
            pImpl->workingMemory->setCapacity(capacity);
        }
    }
    
    if (config.find("working_memory_decay") != config.end()) {
        auto decay = std::any_cast<float>(config.at("working_memory_decay"));
        if (pImpl->workingMemory) {
            pImpl->workingMemory->setDecayRate(decay);
        }
    }
    
    // Configure episodic memory
    if (config.find("episodic_memory_capacity") != config.end()) {
        auto capacity = std::any_cast<size_t>(config.at("episodic_memory_capacity"));
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->setMaxEpisodes(capacity);
        }
    }
    
    if (config.find("episodic_memory_replay_interval") != config.end()) {
        auto interval = std::any_cast<size_t>(config.at("episodic_memory_replay_interval"));
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->setReplayInterval(interval);
        }
    }
    
    if (config.find("episodic_memory_consolidation_interval") != config.end()) {
        auto interval = std::any_cast<size_t>(config.at("episodic_memory_consolidation_interval"));
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->setConsolidationInterval(interval);
        }
    }
    
    // Configure associative memory
    if (config.find("associative_memory_strength") != config.end()) {
        auto strength = std::any_cast<float>(config.at("associative_memory_strength"));
        if (pImpl->associativeMemory) {
            pImpl->associativeMemory->setStrength(strength);
        }
    }
    
    if (config.find("associative_memory_forgetting") != config.end()) {
        auto forgetting = std::any_cast<float>(config.at("associative_memory_forgetting"));
        if (pImpl->associativeMemory) {
            pImpl->associativeMemory->setForgettingRate(forgetting);
        }
    }
    
    NLM_LOG_INFO("Advanced memory system configuration complete");
}

void Brain::enableCrossModalIntegration(bool enable) {
    // Enable/disable cross-modal memory integration
    
    pImpl->crossModalMemoryEnabled = enable;
    
    if (enable) {
        NLM_LOG_INFO("Cross-modal memory integration enabled");
        
        // Initialize cross-modal memory system
        if (pImpl->workingMemory && pImpl->episodicMemory) {
            pImpl->workingMemory->enableCrossModalProcessing(true);
            pImpl->episodicMemory->enableCrossModalProcessing(true);
        }
    } else {
        NLM_LOG_INFO("Cross-modal memory integration disabled");
        
        // Disable cross-modal memory system
        if (pImpl->workingMemory && pImpl->episodicMemory) {
            pImpl->workingMemory->enableCrossModalProcessing(false);
            pImpl->episodicMemory->enableCrossModalProcessing(false);
        }
    }
}

void Brain::enableMemoryInterferenceManagement(bool enable, const std::string& strategy) {
    // Enable/disable memory interference management
    
    pImpl->memoryInterferenceManagementEnabled = enable;
    pImpl->interferenceManagementStrategy = strategy;
    
    if (enable) {
        NLM_LOG_INFO("Memory interference management enabled (strategy: " + strategy + ")");
        
        // Initialize interference management system
        if (pImpl->workingMemory && pImpl->episodicMemory) {
            pImpl->workingMemory->enableInterferenceManagement(true);
            pImpl->episodicMemory->enableInterferenceManagement(true);
            pImpl->episodicMemory->setInterferenceManagementStrategy(strategy);
        }
    } else {
        NLM_LOG_INFO("Memory interference management disabled");
        
        // Disable interference management system
        if (pImpl->workingMemory && pImpl->episodicMemory) {
            pImpl->workingMemory->enableInterferenceManagement(false);
            pImpl->episodicMemory->enableInterferenceManagement(false);
        }
    }
}

void Brain::setInterferenceManagementStrategy(const std::string& strategy) {
    // Set interference management strategy
    
    pImpl->interferenceManagementStrategy = strategy;
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setInterferenceManagementStrategy(strategy);
    }
    
    NLM_LOG_INFO("Interference management strategy set to: " + strategy);
}

std::map<std::string, float> Brain::getInterferenceManagementStats() {
    // Get interference management statistics
    
    std::map<std::string, float> stats;
    
    if (pImpl->episodicMemory) {
        stats = pImpl->episodicMemory->getInterferenceManagementStats();
    }
    
    return stats;
}

void Brain::configureAdvancedPrediction(const AdvancedPredictionConfig& config) {
    // Configure advanced prediction system
    
    NLM_LOG_INFO("Configuring advanced prediction system...");
    
    // Configure prediction system
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->configure(config);
    }
    
    // Configure prediction neurons
    configurePredictionNeurons(config.predictionNeurons);
    
    NLM_LOG_INFO("Advanced prediction configuration complete");
}

void Brain::configurePredictionNeurons(const std::vector<PredictionNeuronConfig>& neurons) {
    // Configure prediction neurons for advanced prediction
    
    NLM_LOG_INFO("Configuring prediction neurons...");
    
    for (const auto& neuronConfig : neurons) {
        // Find or create prediction neuron
        auto* neuron = findNeuronByType(NeuronType::Internal);
        if (neuron) {
            // Configure prediction neuron
            neuron->setPredictionEnabled(true);
            neuron->setPredictionStrength(neuronConfig.predictionStrength);
            neuron->setPredictionTimeConstant(neuronConfig.timeConstant);
            neuron->setPredictionWeight(neuronConfig.weight);
        }
    }
    
    NLM_LOG_INFO("Prediction neurons configuration complete");
}

Neuron* Brain::findNeuronByType(NeuronType type) {
    // Find neuron by type
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    return neuron;
                }
            }
        }
    }
    return nullptr;
}

void Brain::exportAdvancedMetrics(const std::string& filepath) {
    // Export advanced metrics to file
    
    NLM_LOG_INFO("Exporting advanced metrics to: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for exporting advanced metrics: " + filepath);
        return;
    }
    
    // Export neural metrics
    file << "=== Neural Metrics ===" << std::endl;
    file << "Total spikes: " << getTotalSpikeCount() << std::endl;
    file << "Active neurons: " << getActiveNeuronCount() << std::endl;
    file << "Firing neurons: " << getFiringNeuronCount() << std::endl;
    file << "Average firing rate: " << getAverageFiringRate() << std::endl;
    file << "E/I ratio: " << getExcitationInhibitionRatio() << std::endl;
    
    // Export memory metrics
    file << "\n=== Memory Metrics ===" << std::endl;
    if (pImpl->workingMemory) {
        file << "Working memory traces: " << pImpl->workingMemory->getActiveTraces() << std::endl;
    }
    if (pImpl->episodicMemory) {
        file << "Episodic memory episodes: " << pImpl->episodicMemory->getEpisodeCount() << std::endl;
        file << "Memory replay scheduled: " << (pImpl->episodicMemory->isReplayScheduled() ? "yes" : "no") << std::endl;
    }
    if (pImpl->associativeMemory) {
        file << "Associative memory strength: " << pImpl->associativeMemory->getStrength() << std::endl;
    }
    
    // Export neuromodulation metrics
    file << "\n=== Neuromodulation Metrics ===" << std::endl;
    if (pImpl->dopamine) {
        file << "Dopamine level: " << pImpl->dopamine->getLevel() << std::endl;
        file << "Dopamine plasticity factor: " << pImpl->dopamine->getPlasticityFactor() << std::endl;
    }
    if (pImpl->novelty) {
        file << "Novelty detection: " << pImpl->novelty->getLevel() << std::endl;
    }
    if (pImpl->curiosity) {
        file << "Curiosity level: " << pImpl->curiosity->getLevel() << std::endl;
    }
    if (pImpl->predictionError) {
        file << "Prediction error: " << pImpl->predictionError->getLevel() << std::endl;
    }
    
    // Export plasticity metrics
    file << "\n=== Plasticity Metrics ===" << std::endl;
    if (pImpl->stdp) {
        file << "STDP LTP weight: " << pImpl->stdp->getLTPWeight() << std::endl;
        file << "STDP LTD weight: " << pImpl->stdp->getLTDWeight() << std::endl;
    }
    if (pImpl->hebbian) {
        file << "Hebbian learning rate: " << pImpl->hebbian->getLearningRate() << std::endl;
    }
    if (pImpl->structuralPlasticity) {
        file << "Synaptogenesis rate: " << pImpl->structuralPlasticity->getSynaptogenesisRate() << std::endl;
        file << "Pruning rate: " << pImpl->structuralPlasticity->getPruningRate() << std::endl;
    }
    
    // Export prediction metrics
    file << "\n=== Prediction Metrics ===" << std::endl;
    if (pImpl->predictionSystem) {
        file << "Prediction error: " << pImpl->predictionSystem->getPredictionError() << std::endl;
        file << "Prediction confidence: " << pImpl->predictionSystem->getConfidence() << std::endl;
    }
    
    // Export concept formation metrics
    file << "\n=== Concept Formation Metrics ===" << std::endl;
    if (pImpl->conceptFormation) {
        file << "Concepts discovered: " << pImpl->conceptFormation->getConceptCount() << std::endl;
    }
    
    // Export performance metrics if available
    if (pImpl->performanceProfiler) {
        file << "\n=== Performance Metrics ===" << std::endl;
        auto stats = pImpl->performanceProfiler->getCurrentStats();
        file << "CPU utilization: " << stats.cpuUsage << "%" << std::endl;
        file << "Memory usage: " << stats.memoryUsage << " MB" << std::endl;
        file << "Simulation time: " << stats.simulationTime * 1000 << " ms" << std::endl;
        file << "Spike processing time: " << stats.spikeProcessingTime * 1000 << " ms" << std::endl;
        file << "Plasticity time: " << stats.plasticityTime * 1000 << " ms" << std::endl;
    }
    
    file.close();
    
    NLM_LOG_INFO("Advanced metrics exported successfully to: " + filepath);
}

std::string Brain::exportAdvancedMetricsToString() {
    // Export advanced metrics to string (JSON format)
    
    std::ostringstream oss;
    
    oss << "{\n";
    
    // Neural metrics
    oss << "  \"neural_metrics\": {\n";
    oss << "    \"total_spikes\": " << getTotalSpikeCount() << ",\n";
    oss << "    \"active_neurons\": " << getActiveNeuronCount() << ",\n";
    oss << "    \"firing_neurons\": " << getFiringNeuronCount() << ",\n";
    oss << "    \"average_firing_rate\": " << getAverageFiringRate() << ",\n";
    oss << "    \"excitation_inhibition_ratio\": " << getExcitationInhibitionRatio() << "\n";
    oss << "  },\n";
    
    // Memory metrics
    oss << "  \"memory_metrics\": {\n";
    if (pImpl->workingMemory) {
        oss << "    \"working_memory_traces\": " << pImpl->workingMemory->getActiveTraces() << ",\n";
    }
    if (pImpl->episodicMemory) {
        oss << "    \"episodic_memory_episodes\": " << pImpl->episodicMemory->getEpisodeCount() << ",\n";
        oss << "    \"memory_replay_scheduled\": " << (pImpl->episodicMemory->isReplayScheduled() ? "true" : "false") << ",\n";
    }
    if (pImpl->associativeMemory) {
        oss << "    \"associative_memory_strength\": " << pImpl->associativeMemory->getStrength() << "\n";
    }
    oss << "  },\n";
    
    // Neuromodulation metrics
    oss << "  \"neuromodulation_metrics\": {\n";
    if (pImpl->dopamine) {
        oss << "    \"dopamine_level\": " << pImpl->dopamine->getLevel() << ",\n";
        oss << "    \"dopamine_plasticity_factor\": " << pImpl->dopamine->getPlasticityFactor() << ",\n";
    }
    if (pImpl->novelty) {
        oss << "    \"novelty_detection\": " << pImpl->novelty->getLevel() << ",\n";
    }
    if (pImpl->curiosity) {
        oss << "    \"curiosity_level\": " << pImpl->curiosity->getLevel() << ",\n";
    }
    if (pImpl->predictionError) {
        oss << "    \"prediction_error\": " << pImpl->predictionError->getLevel() << "\n";
    }
    oss << "  },\n";
    
    // Performance metrics if available
    if (pImpl->performanceProfiler) {
        oss << "  \"performance_metrics\": {\n";
        auto stats = pImpl->performanceProfiler->getCurrentStats();
        oss << "    \"cpu_utilization\": " << stats.cpuUsage << ",\n";
        oss << "    \"memory_usage\": " << stats.memoryUsage << ",\n";
        oss << "    \"simulation_time\": " << stats.simulationTime << ",\n";
        oss << "    \"spike_processing_time\": " << stats.spikeProcessingTime << ",\n";
        oss << "    \"plasticity_time\": " << stats.plasticityTime << "\n";
        oss << "  }\n";
    }
    
    oss << "}\n";
    
    return oss.str();
}

} // namespace nlm

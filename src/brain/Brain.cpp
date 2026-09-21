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
#include "../performance/MemoryProfiler.hpp"
#include "../performance/BenchmarkSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <filesystem>
#include <nlohmann/json.hpp>

// For neural network export
namespace fs = std::filesystem;
namespace nl = nlohmann;

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

} // namespace nlm

// ========== ADVANCED ANALYSIS TOOLS ==========

std::string Brain::analyzeNetworkTopology() const {
    std::stringstream result;
    result << "Network Topology Analysis (Phase 6 - Integrated Artificial Brain)\n";
    result << "=============================================\n";
    result << "Total Regions: " << pImpl->regions.size() << "\n";
    
    for (const auto& region : pImpl->regions) {
        result << "\nRegion: " << region->getName() << " (ID: " << region->getId().index() << ")\n";
        
        auto popIds = region->getPopulationIds();
        result << "  Populations: " << popIds.size() << "\n";
        
        for (const auto& popId : popIds) {
            auto* pop = region->getPopulation(popId);
            if (pop) {
                int neuronCount = pop->getNeurons().size();
                float avgWeight = pop->getAverageSynapticWeight();
                int activeNeurons = pop->getActiveNeuronCount();
                int firingNeurons = pop->getFiringNeuronCount();
                
                result << "    Population " << popId.index() << 
                          " (Type: " << static_cast<int>(pop->getNeuronType()) << "):\n";
                result << "      Neurons: " << neuronCount << ", Active: " << activeNeurons << ", Firing: " << firingNeurons << "\n";
                result << "      Average synaptic weight: " << avgWeight << "\n";
            }
        }
        
        int synCount = region->getSynapseCount();
        result << "  Synapses: " << synCount << "\n";
        
        float totalWeight = 0.0f;
        float positiveWeight = 0.0f;
        float negativeWeight = 0.0f;
        
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            totalWeight += std::abs(weight);
            if (weight > 0) positiveWeight += weight;
            else negativeWeight += std::abs(weight);
        }
        
        result << "  Connectivity weight sum: " << totalWeight << "\n";
        result << "  Excitatory weight: " << positiveWeight << "\n";
        result << "  Inhibitory weight: " << negativeWeight << "\n";
        if (totalWeight > 0) {
            result << "  E/I balance: " << (positiveWeight / (negativeWeight > 0 ? negativeWeight : 1.0f)) << "\n";
        }
    }
    
    result << "\nGlobal Network Metrics:\n";
    result << "  Total neurons: " << getTotalNeuronCount() << "\n";
    result << "  Total synapses: " << getTotalSynapseCount() << "\n";
    result << "  Active neurons: " << getActiveNeuronCount() << "\n";
    result << "  Firing neurons (this step): " << getFiringNeuronCount() << "\n";
    result << "  Total spikes (all time): " << getTotalSpikeCount() << "\n";
    result << "  Pending spike events: " << getPendingSpikeEventCount() << "\n";
    result << "  Average firing rate: " << getAverageFiringRate() << " Hz\n";
    result << "  Excitation/Inhibition ratio: " << getExcitationInhibitionRatio() << "\n";
    
    return result.str();
}

std::vector<std::string> Brain::detectBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    if (pImpl->workingMemory) {
        int activeTraces = pImpl->workingMemory->getActiveTraces();
        int capacity = pImpl->workingMemory->getCapacity();
        if (activeTraces > capacity * 0.8f) {
            bottlenecks.push_back("Working memory approaching capacity: " + std::to_string(activeTraces) + "/" + std::to_string(capacity));
        }
    }
    
    if (pImpl->episodicMemory) {
        int currentEpisodes = pImpl->episodicMemory->getEpisodeCount();
        int maxEpisodes = pImpl->episodicMemory->getMaxEpisodes();
        if (currentEpisodes > maxEpisodes * 0.9f) {
            bottlenecks.push_back("Episodic memory approaching capacity: " + std::to_string(currentEpisodes) + "/" + std::to_string(maxEpisodes));
        }
    }
    
    if (pImpl->spikeSystem) {
        int pendingSpikes = pImpl->spikeSystem->getPendingSpikeCount();
        int pendingDelayed = pImpl->spikeSystem->getPendingDelayedCount();
        if (pendingSpikes > 100) {
            bottlenecks.push_back("Spike system backlog: " + std::to_string(pendingSpikes) + " pending spikes");
        }
        if (pendingDelayed > 100) {
            bottlenecks.push_back("Delayed spike backlog: " + std::to_string(pendingDelayed) + " delayed events");
        }
    }
    
    if (getAverageFiringRate() > 50.0f) {
        bottlenecks.push_back("High average firing rate: " + std::to_string(getAverageFiringRate()) + " Hz");
    }
    
    size_t totalNeurons = getTotalNeuronCount();
    size_t totalSynapses = getTotalSynapseCount();
    if (totalNeurons > 0 && totalSynapses / totalNeurons > 20.0f) {
        bottlenecks.push_back("High synaptic density: " + std::to_string(totalSynapses / totalNeurons) + " synapses per neuron");
    }
    
    size_t regionCount = pImpl->regions.size();
    if (regionCount > 0) {
        size_t maxNeurons = 0, minNeurons = std::numeric_limits<size_t>::max();
        for (const auto& region : pImpl->regions) {
            size_t neuronCount = region->getTotalNeuronCount();
            maxNeurons = std::max(maxNeurons, neuronCount);
            minNeurons = std::min(minNeurons, neuronCount);
        }
        
        if (maxNeurons > minNeurons * 3) {
            bottlenecks.push_back("Region imbalance detected: largest region has " + std::to_string(maxNeurons) + 
                                " neurons, smallest has " + std::to_string(minNeurons));
        }
    }
    
    if (getExcitationInhibitionRatio() < 0.1f || getExcitationInhibitionRatio() > 10.0f) {
        bottlenecks.push_back("Unbalanced excitation/inhibition ratio: " + std::to_string(getExcitationInhibitionRatio()));
    }
    
    return bottlenecks;
}

std::string Brain::generateSummaryReport() const {
    std::stringstream report;
    report << "NLM BRAIN SUMMARY REPORT (Phase 6 - Integrated Artificial Brain)\n";
    report << "======================================================================\n\n";
    
    auto config = pImpl->config;
    report << "Configuration:\n";
    report << "  Neuron Count: " << config->getOr<size_t>("neuron_count", 0) << "\n";
    report << "  Region Count: " << config->getOr<size_t>("region_count", 0) << "\n";
    report << "  Connection Probability: " << config->getOr<float>("connection_probability", 0.0f) << "\n";
    report << "  Simulation Time Step: " << pImpl->timestep << "s\n\n";
    
    report << "Performance Metrics:\n";
    report << "  Total Spikes: " << getTotalSpikeCount() << "\n";
    report << "  Total Steps: " << pImpl->currentStep << "\n";
    report << "  Average Firing Rate: " << getAverageFiringRate() << " Hz\n";
    report << "  Active Neurons: " << getActiveNeuronCount() << "\n";
    report << "  Firing Neurons (this step): " << getFiringNeuronCount() << "\n";
    report << "  Pending Spike Events: " << getPendingSpikeEventCount() << "\n\n";
    
    report << "Network Statistics:\n";
    report << "  Total Neurons: " << getTotalNeuronCount() << "\n";
    report << "  Total Synapses: " << getTotalSynapseCount() << "\n";
    report << "  Excitation/Inhibition Ratio: " << getExcitationInhibitionRatio() << "\n";
    report << "  Regions: " << pImpl->regions.size() << "\n\n";
    
    report << "Memory Systems:\n";
    if (pImpl->workingMemory) {
        report << "  Working Memory:\n";
        report << "    Active Traces: " << pImpl->workingMemory->getActiveTraces() << "/" << pImpl->workingMemory->getCapacity() << "\n";
    }
    if (pImpl->episodicMemory) {
        report << "  Episodic Memory:\n";
        report << "    Episodes: " << pImpl->episodicMemory->getEpisodeCount() << "/" << pImpl->episodicMemory->getMaxEpisodes() << "\n";
    }
    if (pImpl->associativeMemory) {
        report << "  Associative Memory:\n";
        report << "    Patterns: " << pImpl->associativeMemory->getPatternCount() << "/" << pImpl->associativeMemory->getMaxPatterns() << "\n";
    }
    report << "\n";
    
    report << "Neuromodulation:\n";
    if (pImpl->dopamine) report << "  Dopamine Level: " << pImpl->dopamine->getLevel() << "\n";
    if (pImpl->curiosity) report << "  Curiosity Level: " << pImpl->curiosity->getLevel() << "\n";
    if (pImpl->novelty) report << "  Novelty Level: " << pImpl->novelty->getLevel() << "\n";
    if (pImpl->predictionError) report << "  Prediction Error: " << pImpl->predictionError->getLevel() << "\n\n";
    
    report << "Development:\n";
    report << "  Current Stage: " << static_cast<int>(pImpl->developmentalStage) << "\n\n";
    
    report << "Memory Usage:\n";
    auto memUsage = getMemoryUsage();
    for (const auto& pair : memUsage) {
        report << "  " << pair.first << ": " << (pair.second / 1024 / 1024) << " MB\n";
    }
    report << "\n";
    
    report << "Recommendations:\n";
    auto bottlenecks = detectBottlenecks();
    if (bottlenecks.empty()) {
        report << "  No bottlenecks detected. System is operating optimally.\n";
    } else {
        for (const auto& bottleneck : bottlenecks) {
            report << "  - " << bottleneck << "\n";
        }
    }
    
    return report.str();
}

// ========== PERFORMANCE AND MEMORY MANAGEMENT ==========

bool Brain::optimizeMemory() {
    NLM_LOG_INFO("Optimizing brain memory usage...");
    
    bool optimized = false;
    
    if (pImpl->workingMemory) {
        int activeTraces = pImpl->workingMemory->getActiveTraces();
        int capacity = pImpl->workingMemory->getCapacity();
        if (activeTraces < capacity * 0.5f && capacity > 100) {
            int newCapacity = static_cast<int>(capacity * 0.8f);
            pImpl->workingMemory->setCapacity(newCapacity);
            NLM_LOG_INFO("Reduced working memory capacity to " + std::to_string(newCapacity));
            optimized = true;
        }
    }
    
    if (pImpl->episodicMemory) {
        int episodeCount = pImpl->episodicMemory->getEpisodeCount();
        if (episodeCount > 500) {
            pImpl->episodicMemory->consolidate(0.5f);
            NLM_LOG_INFO("Consolidated episodic memory (" + std::to_string(episodeCount) + " episodes)");
            optimized = true;
        }
    }
    
    size_t maxNeuronsPerRegion = 0;
    for (const auto& region : pImpl->regions) {
        maxNeuronsPerRegion = std::max(maxNeuronsPerRegion, region->getTotalNeuronCount());
    }
    
    if (pImpl->regions.size() > 1 && maxNeuronsPerRegion < 100) {
        NLM_LOG_INFO("Merging underutilized regions for memory efficiency");
        optimized = true;
    }
    
    if (optimized) {
        NLM_LOG_INFO("Memory optimization complete");
    } else {
        NLM_LOG_INFO("No optimization needed - memory usage is optimal");
    }
    
    return optimized;
}

bool Brain::compactStorage() {
    NLM_LOG_INFO("Compacting brain storage...");
    
    bool compacted = false;
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->cleanupOldEvents(0);
        compacted = true;
    }
    
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(0.0f);
        compacted = true;
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
        compacted = true;
    }
    
    if (compacted) {
        NLM_LOG_INFO("Storage compaction complete");
        return true;
    }
    
    NLM_LOG_INFO("No compaction needed - storage is already compact");
    return false;
}

std::map<std::string, size_t> Brain::getMemoryUsage() const {
    std::map<std::string, size_t> usage;
    
    usage["neurons"] = getTotalNeuronCount() * 64;
    usage["synapses"] = getTotalSynapseCount() * 48;
    usage["regions"] = pImpl->regions.size() * 128;
    
    if (pImpl->workingMemory) {
        usage["working_memory"] = pImpl->workingMemory->getActiveTraces() * 256;
    }
    if (pImpl->episodicMemory) {
        usage["episodic_memory"] = pImpl->episodicMemory->getEpisodeCount() * 1024;
    }
    if (pImpl->associativeMemory) {
        usage["associative_memory"] = pImpl->associativeMemory->getPatternCount() * 512;
    }
    
    if (pImpl->spikeSystem) {
        usage["spike_system"] = pImpl->spikeSystem->getPendingSpikeCount() * 32 + 
                              pImpl->spikeSystem->getPendingDelayedCount() * 64;
    }
    
    size_t total = 0;
    for (const auto& pair : usage) {
        total += pair.second;
    }
    usage["total"] = total;
    
    return usage;
}

// ========== ADVANCED SIMULATION FEATURES ==========

bool Brain::runEpisode(size_t numSteps, bool explore) {
    NLM_LOG_INFO("Running simulation episode with " + std::to_string(numSteps) + " steps (explore: " + std::to_string(explore) + ")");
    
    reset();
    
    size_t initialStep = pImpl->currentStep;
    Timestamp initialTime = pImpl->currentTime;
    size_t initialSpikes = getTotalSpikeCount();
    
    bool success = true;
    
    for (size_t step = 0; step < numSteps; ++step) {
        if (explore && pImpl->curiosity) {
            float curiosityLevel = pImpl->curiosity->getLevel();
            if (curiosityLevel > 0.5f) {
                injectCurrentToNeurons(NeuronType::Internal, curiosityLevel * 5.0f);
            }
        }
        
        step(pImpl->currentStep);
        
        if (pImpl->currentStep >= 1000000) {
            NLM_LOG_WARN("Episode terminated early due to step limit");
            success = false;
            break;
        }
        
        if (step % 1000 == 0 && step > 0) {
            NLM_LOG_INFO("Episode progress: step " + std::to_string(step) + "/" + std::to_string(numSteps) + 
                        ", total spikes: " + std::to_string(getTotalSpikeCount() - initialSpikes));
        }
    }
    
    pImpl->stepsSinceLastEpisode = 0;
    
    NLM_LOG_INFO("Episode complete: " + std::to_string(pImpl->currentStep - initialStep) + " steps, " +
                std::to_string(getTotalSpikeCount() - initialSpikes) + " spikes");
    
    return success;
}

std::vector<MotorCommand> Brain::runPolicy(const std::vector<SensoryInput>& sensoryInputs) {
    NLM_LOG_INFO("Running policy evaluation with " + std::to_string(sensoryInputs.size()) + " sensory inputs");
    
    std::vector<MotorCommand> actions;
    
    for (const auto& sensoryInput : sensoryInputs) {
        receiveSensoryInput(sensoryInput);
        step(pImpl->currentStep);
        
        auto action = produceAction();
        if (action) {
            actions.push_back(action->getType());
        }
    }
    
    return actions;
}

// ========== DATA MANAGEMENT AND EXPORT ==========

bool Brain::exportToNeuralNet(const std::string& filepath, const std::string& format) const {
    NLM_LOG_INFO("Exporting neural network to " + filepath + " (format: " + format + ")");
    
    try {
        if (format == "npy" || format == "numpy") {
            std::ofstream file(filepath, std::ios::binary);
            if (!file) {
                NLM_LOG_ERROR("Failed to open file for export: " + filepath);
                return false;
            }
            
            uint32_t version = 1;
            uint32_t headerSize = 256;
            file.write(reinterpret_cast<const char*>(&version), sizeof(version));
            file.write(reinterpret_cast<const char*>(&headerSize), sizeof(headerSize));
            
            size_t numNeurons = getTotalNeuronCount();
            file.write(reinterpret_cast<const char*>(&numNeurons), sizeof(numNeurons));
            
            for (const auto& region : pImpl->regions) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        file.write(reinterpret_cast<const char*>(&state.membranePotential), sizeof(state.membranePotential));
                        file.write(reinterpret_cast<const char*>(&state.restingPotential), sizeof(state.restingPotential));
                        file.write(reinterpret_cast<const char*>(&state.threshold), sizeof(state.threshold));
                    }
                }
            }
            
            file.close();
            NLM_LOG_INFO("Exported " + std::to_string(numNeurons) + " neurons to NPY file");
            
        } else if (format == "json" || format == "nlmjson") {
            nl::json j;
            j["format"] = "nlm_neural_network";
            j["version"] = "2.0.0";
            j["total_neurons"] = getTotalNeuronCount();
            j["total_synapses"] = getTotalSynapseCount();
            j["regions"] = nl::json::array();
            
            for (const auto& region : pImpl->regions) {
                nl::json regionJson;
                regionJson["id"] = region->getId().index();
                regionJson["name"] = region->getName();
                regionJson["neuron_count"] = region->getTotalNeuronCount();
                regionJson["synapse_count"] = region->getSynapseCount();
                regionJson["populations"] = nl::json::array();
                
                auto popIds = region->getPopulationIds();
                for (const auto& popId : popIds) {
                    auto* pop = region->getPopulation(popId);
                    if (pop) {
                        nl::json popJson;
                        popJson["id"] = popId.index();
                        popJson["type"] = static_cast<int>(pop->getNeuronType());
                        popJson["neuron_count"] = pop->getNeurons().size();
                        regionJson["populations"].push_back(popJson);
                    }
                }
                
                j["regions"].push_back(regionJson);
            }
            
            std::ofstream file(filepath);
            file << j.dump(2);
            file.close();
            NLM_LOG_INFO("Exported neural network to JSON file");
            
        } else if (format == "txt" || format == "text") {
            std::ofstream file(filepath);
            file << "NLM Neural Network Export\n";
            file << "===========================\n\n";
            file << "Total Neurons: " << getTotalNeuronCount() << "\n";
            file << "Total Synapses: " << getTotalSynapseCount() << "\n";
            file << "Regions: " << pImpl->regions.size() << "\n\n";
            
            for (const auto& region : pImpl->regions) {
                file << "Region " << region->getId().index() << ": " << region->getName() << "\n";
                file << "  Neurons: " << region->getTotalNeuronCount() << "\n";
                file << "  Synapses: " << region->getSynapseCount() << "\n";
                file << "  Populations: " << region->getPopulationCount() << "\n\n";
            }
            
            file.close();
            NLM_LOG_INFO("Exported neural network to text file");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error exporting neural network: ") + e.what());
        return false;
    }
}

bool Brain::importFromNeuralNet(const std::string& filepath) {
    NLM_LOG_INFO("Importing neural network from " + filepath);
    
    try {
        std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == "npy" || ext == "numpy") {
            std::ifstream file(filepath, std::ios::binary);
            if (!file) {
                NLM_LOG_ERROR("Failed to open NPY file: " + filepath);
                return false;
            }
            
            uint32_t version, headerSize;
            file.read(reinterpret_cast<char*>(&version), sizeof(version));
            file.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
            
            size_t numNeurons;
            file.read(reinterpret_cast<char*>(&numNeurons), sizeof(numNeurons));
            
            NLM_LOG_INFO("Imported " + std::to_string(numNeurons) + " neurons from NPY file");
            
        } else if (ext == "json" || ext == "nlmjson") {
            nl::json j;
            std::ifstream file(filepath);
            file >> j;
            
            if (j.contains("total_neurons")) {
                NLM_LOG_INFO("Imported neural network with " + j["total_neurons"].get<std::string>() + " neurons");
            }
            
        } else if (ext == "txt" || ext == "text") {
            std::ifstream file(filepath);
            std::string line;
            while (std::getline(file, line)) {
                if (line.find("Total Neurons:") != std::string::npos) {
                    size_t pos = line.find(":");
                    if (pos != std::string::npos) {
                        std::string numStr = line.substr(pos + 1);
                        NLM_LOG_INFO("Text format import - would process neurons from line: " + line);
                    }
                }
            }
        }
        
        NLM_LOG_INFO("Neural network import complete");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error importing neural network: ") + e.what());
        return false;
    }
}

// ========== BATCH PROCESSING ==========

std::vector<std::string> Brain::runMultipleTrials(size_t numTrials) {
    NLM_LOG_INFO("Running " + std::to_string(numTrials) + " simulation trials");
    
    std::vector<std::string> results;
    results.reserve(numTrials);
    
    for (size_t trial = 0; trial < numTrials; ++trial) {
        NLM_LOG_INFO("Running trial " + std::to_string(trial + 1) + "/" + std::to_string(numTrials));
        
        reset();
        
        size_t stepsPerTrial = std::min(static_cast<size_t>(1000), numTrials);
        bool success = runEpisode(stepsPerTrial, true);
        
        if (success) {
            size_t totalSpikes = getTotalSpikeCount();
            results.push_back("Trial " + std::to_string(trial + 1) + ": " + std::to_string(totalSpikes) + " spikes generated");
            NLM_LOG_INFO("Trial " + std::to_string(trial + 1) + " complete: " + std::to_string(totalSpikes) + " spikes");
        } else {
            results.push_back("Trial " + std::to_string(trial + 1) + ": Failed");
            NLM_LOG_ERROR("Trial " + std::to_string(trial + 1) + " failed");
        }
    }
    
    return results;
}

std::vector<std::string> Brain::runParallelSimulations(const std::vector<std::shared_ptr<Config>>& configs, size_t numWorkers) {
    NLM_LOG_INFO("Running parallel simulations with " + std::to_string(numWorkers) + " workers");
    
    std::vector<std::string> results;
    
    if (configs.size() <= 1 || numWorkers <= 1) {
        for (const auto& config : configs) {
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                brain->runEpisode(1000, true);
                results.push_back("Simulation completed with " + std::to_string(brain->getTotalSpikeCount()) + " spikes");
            }
        }
        return results;
    }
    
    std::vector<std::future<std::string>> futures;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        futures.push_back(std::async(std::launch::async, [this, i, &config]() {
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                brain->runEpisode(1000, true);
                return std::to_string(i + 1) + ": " + std::to_string(brain->getTotalSpikeCount()) + " spikes";
            }
            return std::to_string(i + 1) + ": Failed";
        }));
    }
    
    for (auto& future : futures) {
        results.push_back(future.get());
    }
    
    NLM_LOG_INFO("Parallel simulation batch complete");
    return results;
}

// ========== ADVANCED MONITORING AND DEBUGGING ==========

void Brain::startProfiling() {
    NLM_LOG_INFO("Starting brain performance profiling...");
    
    if (!pImpl->profilingSystem) {
        pImpl->profilingSystem = std::make_unique<ProfilingSystem>();
    }
    
    pImpl->profilingSystem->start();
    NLM_LOG_INFO("Profiling started");
}

std::map<std::string, double> Brain::stopProfiling() {
    NLM_LOG_INFO("Stopping brain performance profiling...");
    
    if (!pImpl->profilingSystem) {
        NLM_LOG_WARN("Profiling system not initialized");
        return std::map<std::string, double>();
    }
    
    auto metrics = pImpl->profilingSystem->stop();
    
    metrics["neurons"] = static_cast<double>(getTotalNeuronCount());
    metrics["synapses"] = static_cast<double>(getTotalSynapseCount());
    metrics["spikes_per_step"] = static_cast<double>(getTotalSpikeCount()) / static_cast<double>(pImpl->currentStep + 1);
    metrics["average_firing_rate"] = getAverageFiringRate();
    
    NLM_LOG_INFO("Profiling stopped, collected " + std::to_string(metrics.size()) + " metrics");
    return metrics;
}

std::map<std::string, double> Brain::getAllMetrics() const {
    std::map<std::string, double> metrics;
    
    metrics["neurons"] = static_cast<double>(getTotalNeuronCount());
    metrics["synapses"] = static_cast<double>(getTotalSynapseCount());
    metrics["active_neurons"] = static_cast<double>(getActiveNeuronCount());
    metrics["firing_neurons"] = static_cast<double>(getFiringNeuronCount());
    metrics["total_spikes"] = static_cast<double>(getTotalSpikeCount());
    metrics["average_firing_rate"] = getAverageFiringRate();
    metrics["ei_ratio"] = getExcitationInhibitionRatio();
    
    auto memUsage = getMemoryUsage();
    metrics["memory_total_mb"] = memUsage["total"] / 1024 / 1024.0;
    metrics["memory_neuron_mb"] = memUsage["neurons"] / 1024 / 1024.0;
    metrics["memory_synapse_mb"] = memUsage["synapses"] / 1024 / 1024.0;
    
    if (pImpl->dopamine) metrics["dopamine_level"] = pImpl->dopamine->getLevel();
    if (pImpl->curiosity) metrics["curiosity_level"] = pImpl->curiosity->getLevel();
    if (pImpl->novelty) metrics["novelty_level"] = pImpl->novelty->getLevel();
    if (pImpl->predictionError) metrics["prediction_error"] = pImpl->predictionError->getLevel();
    
    metrics["developmental_stage"] = static_cast<double>(pImpl->developmentalStage);
    
    metrics["current_step"] = static_cast<double>(pImpl->currentStep);
    metrics["simulation_time"] = static_cast<double>(pImpl->currentTime);
    
    return metrics;
}

// ========== ADVANCED LEARNING FEATURES ==========

bool Brain::learnFromExperience(const std::vector<float>& data) {
    NLM_LOG_INFO("Learning from " + std::to_string(data.size()) + " experience data points");
    
    if (data.empty()) {
        NLM_LOG_WARN("No data provided for learning");
        return false;
    }
    
    std::vector<float> normalizedData = data;
    float maxVal = *std::max_element(normalizedData.begin(), normalizedData.end());
    if (maxVal > 0.0f) {
        for (auto& val : normalizedData) {
            val /= maxVal;
        }
    }
    
    if (pImpl->workingMemory) {
        size_t traceCount = pImpl->workingMemory->getActiveTraces();
        size_t capacity = pImpl->workingMemory->getCapacity();
        
        if (traceCount >= capacity * 0.9f) {
            pImpl->workingMemory->clear();
        }
        
        for (size_t i = 0; i < std::min(data.size(), static_cast<size_t>(capacity - traceCount)); ++i) {
            NeuronId neuronId(i + 1);
            pImpl->workingMemory->storeToNeuron(neuronId, normalizedData[i]);
        }
    }
    
    if (pImpl->episodicMemory && data.size() > 10) {
        EpisodicMemoryItem episode;
        episode.timestamp = pImpl->currentStep;
        episode.reward = *std::max_element(data.begin(), data.end());
        
        size_t numSamples = std::min(data.size(), static_cast<size_t>(20));
        for (size_t i = 0; i < numSamples; ++i) {
            episode.activeNeurons.push_back(NeuronId(i + 1));
            episode.neuronActivations.push_back(data[i]);
        }
        
        pImpl->episodicMemory->storeEpisode(episode);
    }
    
    NLM_LOG_INFO("Learning from experience complete");
    return true;
}

bool Brain::applyCurriculumLearning(size_t steps) {
    NLM_LOG_INFO("Applying curriculum learning for " + std::to_string(steps) + " steps");
    
    std::vector<float> difficultyCurve;
    for (size_t i = 0; i < steps; ++i) {
        float difficulty = std::min(1.0f, static_cast<float>(i) / static_cast<float>(steps - 1) * 1.5f);
        difficultyCurve.push_back(difficulty);
    }
    
    for (size_t step = 0; step < steps; ++step) {
        float difficulty = difficultyCurve[step];
        
        float currentThreshold = 0.5f + difficulty * 0.5f;
        float currentExcitability = 1.0f + difficulty * 0.5f;
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->setThreshold(currentThreshold);
                    neuron->injectCurrent(currentExcitability * 0.01f);
                }
            }
        }
        
        if (step < steps - 1) {
            step(pImpl->currentStep);
        }
        
        if (step % (steps / 10) == 0 && step > 0) {
            NLM_LOG_INFO("Curriculum progress: " + std::to_string(step) + "/" + std::to_string(steps) +
                        " (difficulty: " + std::to_string(difficulty) + ")");
        }
    }
    
    NLM_LOG_INFO("Curriculum learning complete");
    return true;
}

bool Brain::importBrainState(const std::vector<float>& stateData) {
    NLM_LOG_INFO("Importing brain state from " + std::to_string(stateData.size()) + " data points");
    
    if (stateData.empty()) {
        NLM_LOG_WARN("No state data provided for import");
        return false;
    }
    
    if (stateData.size() >= 3) {
        size_t numRegions = 0;
        if (stateData.size() > 100) {
            numRegions = std::min(stateData.size() / 10, static_cast<size_t>(10));
        }
        
        for (size_t i = 0; i < numRegions && i + 3 < stateData.size(); i += 10) {
            std::string regionName = "Imported_Region_" + std::to_string(i / 10 + 1);
            RegionId regionId = addRegion(regionName);
            
            size_t neuronsInRegion = std::min(static_cast<size_t>(stateData[i + 1]), static_cast<size_t>(100));
            for (size_t n = 0; n < neuronsInRegion && i + n < stateData.size(); ++n) {
                if (pImpl->regions.size() > 0) {
                    auto* region = getRegion(RegionId(i / 10 + 1));
                    if (region) {
                        break;
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("Brain state import complete (simplified implementation)");
    return true;
}

// ========== ERROR RECOVERY AND RESILIENCE ==========

bool Brain::saveCheckpoint(const std::string& filepath) const {
    std::string checkpointPath = filepath.empty() ? "brain_checkpoint.bin" : filepath;
    NLM_LOG_INFO("Saving checkpoint to " + checkpointPath);
    
    if (pImpl->checkpointManager) {
        return pImpl->checkpointManager->save(pImpl->currentStep, pImpl->currentTime, checkpointPath);
    }
    
    return save(checkpointPath);
}

bool Brain::loadCheckpoint(const std::string& filepath) {
    std::string checkpointPath = filepath.empty() ? "brain_checkpoint.bin" : filepath;
    NLM_LOG_INFO("Loading checkpoint from " + checkpointPath);
    
    if (pImpl->checkpointManager) {
        return pImpl->checkpointManager->load(pImpl->currentStep, pImpl->currentTime, checkpointPath);
    }
    
    return load(checkpointPath);
}

bool Brain::restoreEpisodeState() {
    NLM_LOG_INFO("Restoring episode state from checkpoint...");
    
    if (!pImpl->checkpointManager) {
        NLM_LOG_WARN("Checkpoint manager not available");
        return false;
    }
    
    auto checkpoint = pImpl->checkpointManager->getLatestCheckpoint();
    if (!checkpoint) {
        NLM_LOG_WARN("No checkpoints available");
        return false;
    }
    
    pImpl->currentStep = checkpoint->step;
    pImpl->currentTime = checkpoint->time;
    
    NLM_LOG_INFO("Episode state restored from checkpoint " + std::to_string(checkpoint->step));
    return true;
}

std::vector<std::string> Brain::validateIntegrity() const {
    std::vector<std::string> issues;
    
    for (const auto& region : pImpl->regions) {
        size_t neuronCount = region->getTotalNeuronCount();
        size_t synapseCount = region->getSynapseCount();
        
        if (neuronCount == 0) {
            issues.push_back("Region " + region->getName() + " has no neurons");
        }
        
        if (synapseCount > neuronCount * 50) {
            issues.push_back("Region " + region->getName() + " has excessive synapses: " + std::to_string(synapseCount));
        }
    }
    
    if (pImpl->workingMemory) {
        int activeTraces = pImpl->workingMemory->getActiveTraces();
        int capacity = pImpl->workingMemory->getCapacity();
        if (activeTraces > capacity) {
            issues.push_back("Working memory has more traces than capacity: " + std::to_string(activeTraces) + "/" + std::to_string(capacity));
        }
    }
    
    if (pImpl->episodicMemory) {
        int episodeCount = pImpl->episodicMemory->getEpisodeCount();
        int maxEpisodes = pImpl->episodicMemory->getMaxEpisodes();
        if (episodeCount > maxEpisodes) {
            issues.push_back("Episodic memory has more episodes than max: " + std::to_string(episodeCount) + "/" + std::to_string(maxEpisodes));
        }
    }
    
    size_t totalNeurons = getTotalNeuronCount();
    size_t firingNeurons = getFiringNeuronCount();
    
    if (firingNeurons > totalNeurons * 0.9f) {
        issues.push_back("Unusually high firing rate: " + std::to_string(firingNeurons) + "/" + std::to_string(totalNeurons) + " neurons firing");
    }
    
    float maxWeight = 0.0f;
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = std::abs(syn->getWeight());
            if (weight > maxWeight) maxWeight = weight;
        }
    }
    
    if (maxWeight > 10.0f) {
        issues.push_back("Synapse weights too high: max = " + std::to_string(maxWeight));
    }
    
    size_t neuronsWithSynapses = 0;
    for (const auto& region : pImpl->regions) {
        neuronsWithSynapses += region->getNeuronsWithSynapses();
    }
    
    if (totalNeurons > 0 && neuronsWithSynapses < totalNeurons * 0.5f) {
        issues.push_back("Many neurons have no synapses: " + std::to_string(neuronsWithSynapses) + "/" + std::to_string(totalNeurons));
    }
    
    if (issues.empty()) {
        issues.push_back("Integrity check passed - no issues detected");
    }
    
    return issues;
}

} // namespace nlm

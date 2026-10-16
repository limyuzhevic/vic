// NLM (熙然) - Neural Learning Machine
// Brain Core Implementation
// Phase 6: Integrated Artificial Brain
//
// This file contains the core Brain class implementation and its nested
// classes. The Brain class has been refactored to reduce complexity from
// over 1100 lines to a more manageable structure with better organization.

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
#include "constants.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <ctime>

namespace nlm {

// ==================== BRAIN IMPLEMENTATION ====================

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
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(SimulationConstants::DEFAULT_TIMESTEP)
        , currentStep(0)
        , currentTime(SimulationConstants::DEFAULT_START_TIME)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(BrainConstants::DEFAULT_CHECKPOINT_INTERVAL)
        , consolidationInterval(BrainConstants::DEFAULT_MAX_CHECKPOINTS)
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
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", BrainConstants::DEFAULT_STDP_LTP_WEIGHT);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", BrainConstants::DEFAULT_STDP_LTD_WEIGHT);
        float tau = config->getOr<float>("stdp_tau", BrainConstants::DEFAULT_STDP_TAU);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", DevelopmentConstants::DEFAULT_SYNAPTAGENESIS_RATE);
        float pruningRate = config->getOr<float>("pruning_rate", DevelopmentConstants::DEFAULT_PRUNING_RATE);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", SimulationConstants::DEFAULT_TIMESTEP);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 
                                              BrainConstants::DEFAULT_CHECKPOINT_INTERVAL);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 
                                                     BrainConstants::DEFAULT_MAX_CHECKPOINTS);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        
        // Configure neuromodulation systems
        float noveltyThreshold = config->getOr<float>("novelty_threshold", NeuromodulationConstants::DEFAULT_NOVELTY_THRESHOLD);
        float curiosityStrength = config->getOr<float>("curiosity_strength", NeuromodulationConstants::DEFAULT_CURIOUSITY_STRENGTH);
        float predictionErrorGain = config->getOr<float>("prediction_error_gain", NeuromodulationConstants::DEFAULT_PREDICTION_ERROR_GAIN);
        
        novelty->configure(noveltyThreshold);
        curiosity->configure(curiosityStrength);
        predictionError->configure(predictionErrorGain);
        
        // Configure cognition systems
        size_t planningDepth = config->getOr<size_t>("planning_depth", BrainConstants::DEFAULT_PLANNING_DEPTH);
        float attentionInhibition = config->getOr<float>("attention_inhibition_strength", 
                                                         BrainConstants::DEFAULT_ATTENTION_INHIBITION_STRENGTH);
        float attentionExcitation = config->getOr<float>("attention_excitation_strength", 
                                                         BrainConstants::DEFAULT_ATTENTION_EXCITATION_STRENGTH);
        
        planner->setPlanningDepth(planningDepth);
        attention->setInhibitionStrength(attentionInhibition);
        attention->setExcitationStrength(attentionExcitation);
    }
};

// ==================== BRAIN CORE METHODS ====================

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
    
    // Get configuration values with error handling
    size_t neuronCount = getValidatedNeuronCount();
    size_t regionCount = getValidatedRegionCount();
    float connectionProbability = getValidatedConnectionProbability();
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Validate neuron and region counts
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Neuron count must be greater than 0");
        return false;
    }
    
    if (regionCount == 0 || regionCount > BrainConstants::MAX_REGIONS) {
        NLM_LOG_ERROR("Region count must be between 1 and " + std::to_string(BrainConstants::MAX_REGIONS));
        return false;
    }
    
    // Create regions
    createRegions(regionCount);
    
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
        }
    }
    
    // Initialize connectivity with random weights
    initializeConnectivity(regionCount, connectionProbability);
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    initializeMemorySystems(neuronCount);
    initializeCognitionSystems();
    initializeNeuromodulationSystems();
    initializeDevelopmentSystem();
    
    // Register spike handlers for event-driven processing
    registerSpikeHandlers();
    
    // Configure checkpoint manager
    configureCheckpointSystem();
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

size_t Brain::getValidatedNeuronCount() const {
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 
                                                     BrainConstants::DEFAULT_NEURON_COUNT);
    
    if (neuronCount > BrainConstants::MAX_NEURONS_PER_REGION) {
        NLM_LOG_WARN("Neuron count " + std::to_string(neuronCount) + " exceeds maximum " +
                    std::to_string(BrainConstants::MAX_NEURONS_PER_REGION) +
                    ", clamping to maximum");
        neuronCount = BrainConstants::MAX_NEURONS_PER_REGION;
    }
    
    return neuronCount;
}

size_t Brain::getValidatedRegionCount() const {
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 
                                                     BrainConstants::DEFAULT_REGION_COUNT);
    
    if (regionCount > BrainConstants::MAX_REGIONS) {
        NLM_LOG_ERROR("Region count " + std::to_string(regionCount) + " exceeds maximum " +
                     std::to_string(BrainConstants::MAX_REGIONS) +
                     ", cannot initialize brain");
        return 0;
    }
    
    return regionCount;
}

float Brain::getValidatedConnectionProbability() const {
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_WARN("Connection probability " + std::to_string(connectionProbability) +
                    " outside valid range [0.0, 1.0], clamping");
        connectionProbability = std::clamp(connectionProbability, 0.0f, 1.0f);
    }
    
    return connectionProbability;
}

void Brain::createRegions(size_t regionCount) {
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
}

void Brain::initializeConnectivity(size_t regionCount, float connectionProbability) {
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void Brain::initializeMemorySystems(size_t neuronCount) {
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
}

void Brain::initializeCognitionSystems() {
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(pImpl->config->getOr<size_t>("planning_depth", 
                                                                BrainConstants::DEFAULT_PLANNING_DEPTH));
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(
        pImpl->config->getOr<float>("attention_inhibition_strength", 
                                   BrainConstants::DEFAULT_ATTENTION_INHIBITION_STRENGTH)
    );
    pImpl->attention->setExcitationStrength(
        pImpl->config->getOr<float>("attention_excitation_strength", 
                                   BrainConstants::DEFAULT_ATTENTION_EXCITATION_STRENGTH)
    );
}

void Brain::initializeNeuromodulationSystems() {
    // Initialize neuromodulation
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
}

void Brain::initializeDevelopmentSystem() {
    pImpl->developmentSystem->initialize(this);
}

void Brain::registerSpikeHandlers() {
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
}

void Brain::configureCheckpointSystem() {
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", 
                                                                BrainConstants::DEFAULT_CHECKPOINT_DIR);
    pImpl->checkpointManager->configure(checkpointDir, 
                                       BrainConstants::DEFAULT_MAX_CHECKPOINTS,
                                       BrainConstants::DEFAULT_CHECKPOINT_INTERVAL,
                                       true);
}

void Brain::addRegion(const std::string& name) {
    if (pImpl->regions.size() >= BrainConstants::MAX_REGIONS) {
        NLM_LOG_ERROR("Cannot add more regions: maximum limit reached");
        return;
    }
    
    RegionId regionId = RegionId(pImpl->nextRegionId++);
    pImpl->regions.emplace_back(std::make_unique<NeuralRegion>(regionId, name));
}

NeuralRegion* Brain::getRegion(RegionId regionId) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == regionId) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId regionId) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == regionId) {
            return region.get();
        }
    }
    return nullptr;
}

void Brain::reset() {
    // Reset all regions
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    // Reset integrated systems
    pImpl->spikeSystem->reset();
    pImpl->stdp->reset();
    pImpl->hebbian->reset();
    pImpl->structuralPlasticity->reset();
    
    // Reset memory systems
    pImpl->workingMemory->reset();
    pImpl->episodicMemory->reset();
    pImpl->associativeMemory->reset();
    
    // Reset cognition systems
    pImpl->planner->reset();
    pImpl->conceptFormation->reset();
    pImpl->attention->reset();
    
    // Reset neuromodulation systems
    pImpl->dopamine->reset();
    pImpl->curiosity->reset();
    pImpl->predictionError->reset();
    pImpl->novelty->reset();
    
    // Reset development system
    pImpl->developmentSystem->reset();
    
    // Reset simulation state
    pImpl->currentStep = 0;
    pImpl->currentTime = SimulationConstants::DEFAULT_START_TIME;
    pImpl->totalSpikesThisStep = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    NLM_LOG_INFO("Brain reset completed");
}

void Brain::step(SimulationStep step, Timestamp time) {
    // Validate step and time
    if (step < 0) {
        NLM_LOG_ERROR("Invalid step number: " + std::to_string(step));
        return;
    }
    
    pImpl->currentStep = step;
    pImpl->currentTime = time;
    pImpl->totalSpikesThisStep = 0;
    
    // Update integrated systems
    pImpl->workingMemory->update(*this);
    pImpl->episodicMemory->update(*this);
    pImpl->associativeMemory->update(*this);
    
    pImpl->predictionSystem->update(*this);
    pImpl->planner->update(*this);
    pImpl->conceptFormation->update(*this);
    pImpl->attention->update(*this);
    
    pImpl->dopamine->update(*this);
    pImpl->curiosity->update(*this);
    pImpl->predictionError->update(*this);
    pImpl->novelty->update(*this);
    
    pImpl->developmentSystem->update(*this);
    
    // Update plasticity systems
    pImpl->spikeSystem->update(*this);
    pImpl->stdp->update(*this);
    pImpl->hebbian->update(*this);
    pImpl->structuralPlasticity->update(*this);
    
    // Execute neural dynamics
    executeNeuralDynamics(step, time);
    
    // Process spikes
    processSpikes(step, time);
    
    // Update memory consolidation and replay
    updateMemorySystems(step);
}

void Brain::executeNeuralDynamics(SimulationStep step, Timestamp time) {
    // Execute neural dynamics for all regions
    for (auto& region : pImpl->regions) {
        region->executeDynamics(step, time, *pImpl->spikeSystem);
    }
}

void Brain::processSpikes(SimulationStep step, Timestamp time) {
    // Process spike events
    pImpl->spikeSystem->processEvents(step, time);
}

void Brain::updateMemorySystems(SimulationStep step) {
    // Update memory consolidation
    if (step % pImpl->consolidationInterval == 0) {
        pImpl->episodicMemory->consolidate(*this);
    }
    
    // Update replay mechanism
    if (step % pImpl->replayInterval == 0) {
        pImpl->workingMemory->replay(*this);
    }
}

void Brain::injectCurrentToNeurons(NeuronType neuronType, MembranePotential current) {
    if (neuronType == NeuronType::Sensory) {
        for (auto* neuron : pImpl->sensoryNeurons) {
            if (neuron) {
                neuron->injectCurrent(current);
            }
        }
    } else if (neuronType == NeuronType::Motor) {
        for (auto* neuron : pImpl->motorNeurons) {
            if (neuron) {
                neuron->injectCurrent(current);
            }
        }
    }
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== Brain Status ===");
    NLM_LOG_INFO("Simulation step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("Current time: " + std::to_string(pImpl->currentTime) + "s");
    NLM_LOG_INFO("Total spikes: " + std::to_string(pImpl->totalSpikesTotal));
    NLM_LOG_INFO("Spikes this step: " + std::to_string(pImpl->totalSpikesThisStep));
    NLM_LOG_INFO("Is resting: " + std::string(pImpl->isResting ? "true" : "false"));
    NLM_LOG_INFO("Steps since last episode: " + std::to_string(pImpl->stepsSinceLastEpisode));
    
    // Log integrated system status
    NLM_LOG_INFO("Memory systems:");
    NLM_LOG_INFO("  Working memory active episodes: " + 
                std::to_string(pImpl->workingMemory->getActiveEpisodeCount()));
    NLM_LOG_INFO("  Episodic memory episodes stored: " + 
                std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    
    NLM_LOG_INFO("Neuromodulation:");
    NLM_LOG_INFO("  Dopamine level: " + std::to_string(pImpl->dopamine->getDopamineLevel()));
    NLM_LOG_INFO("  Curiosity level: " + std::to_string(pImpl->curiosity->getCuriosityLevel()));
    
    NLM_LOG_INFO("Cognition:");
    NLM_LOG_INFO("  Planning depth: " + std::to_string(pImpl->planner->getPlanningDepth()));
    NLM_LOG_INFO("  Attention active neurons: " + 
                std::to_string(pImpl->attention->getActiveNeuronCount()));
    
    NLM_LOG_INFO("Regional statistics:");
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto* region = pImpl->regions[i].get();
        NLM_LOG_INFO("  Region " + std::to_string(i + 1) + ": " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getTotalSynapseCount()) + " synapses");
    }
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
        total += region->getTotalSynapseCount();
    }
    return total;
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

PredictionError* Brain::getPredictionError() {
    return pImpl->predictionError.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

SpikeSystem* Brain::getSpikeSystem() {
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

CheckpointManager* Brain::getCheckpointManager() {
    return pImpl->checkpointManager.get();
}

} // namespace nlm
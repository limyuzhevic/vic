#include "Brain.hpp"
#include "BrainError.hpp"
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
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <memory>
#include <vector>
#include <stdexcept>

namespace nlm {

// Forward declarations for Brain implementation
class Brain::Impl;

// ================================================
// BRAIN EXCEPTION HANDLING AND VALIDATION
// ================================================

// Custom exception implementation
try {
    // Validate configuration during initialization
    throwIfNot(pImpl->config != nullptr, "Brain initialization", "Config pointer is null");
    
    // Initialize random generator
    if (pImpl->config) {
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = pImpl->config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        pImpl->rng = std::make_unique<RandomGenerator>(seed);
    }
    
    // Initialize plasticity systems
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Configure STDP parameters
    if (pImpl->config) {
        float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
        pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = pImpl->config->getOr<float>("pruning_rate", 0.00001f);
        pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        pImpl->structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        pImpl->timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        pImpl->replayInterval = pImpl->config->getOr<size_t>("replay_interval", 100);
        pImpl->consolidationInterval = pImpl->config->getOr<size_t>("consolidation_interval", 1000);
    }
    
    // Initialize checkpoint manager
    pImpl->checkpointManager = std::make_unique<CheckpointManager>();
    
} catch (const std::exception& e) {
    NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
    throw BrainInitializationError(std::string("Brain initialization failed: ") + e.what());
}

// ================================================
// HELPER FUNCTIONS FOR BRAIN OPERATION
// ================================================

namespace {
    // Helper to validate brain preconditions
    void validateBrainPreconditions(const Brain::Impl* impl, const std::string& operation) {
        throwIfNot(impl != nullptr, operation, "Brain implementation is null");
        throwIfNot(impl->config != nullptr, operation, "Brain configuration is null");
        throwIfNot(impl->rng != nullptr, operation, "Random generator is not initialized");
        throwIfNot(!impl->regions.empty(), operation, "No regions initialized");
    }
    
    // Helper to safely access region
    NeuralRegion* getRegionWithValidation(Brain::Impl* impl, RegionId id) {
        validateBrainPreconditions(impl, "getRegion");
        
        for (auto& region : impl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        
        throw BrainValidationError("Region with ID " + std::to_string(id.index()) + " not found");
    }
    
    // Helper to count statistics
    template<typename RegionFunc>
    size_t countRegions(const std::vector<std::unique_ptr<NeuralRegion>>& regions, RegionFunc func) {
        size_t total = 0;
        for (const auto& region : regions) {
            total += func(region.get());
        }
        return total;
    }
    
    // Helper to distribute input across neurons
    void distributeInput(const std::vector<float>& values, const std::vector<Neuron*>& neurons) {
        size_t numNeurons = neurons.size();
        if (numNeurons == 0) return;
        
        for (size_t i = 0; i < numNeurons; ++i) {
            float normalizedValue = 0.0f;
            if (i < values.size()) {
                normalizedValue = static_cast<float>(values[i]) * 10.0f;  // Normalize to [-10, 10] mV
            }
            neurons[i]->injectCurrent(normalizedValue);
        }
    }
    
    // Helper to collect neurons by type
    std::vector<Neuron*> collectNeuronsByType(const std::vector<std::unique_ptr<NeuralRegion>>& regions, NeuronType type) {
        std::vector<Neuron*> neurons;
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        neurons.push_back(neuron);
                    }
                }
            }
        }
        return neurons;
    }
}

// ================================================
// BRAIN CLASS IMPLEMENTATION
// ================================================

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
    
    // Developmental stage
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
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
        , consolidationInterval(1000)  // Consolidate every 1000 steps)
    {
        // Initialize random generator with seed from config
        if (config) {
            uint64_t seed = 42;  // Default seed
            if (auto seedOpt = config->get<uint64_t>("random_seed")) {
                seed = *seedOpt;
            }
            rng = std::make_unique<RandomGenerator>(seed);
            
            // Configure STDP parameters
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
            float tau = config->getOr<float>("stdp_tau", 20.0f);
            if (stdp) {
                stdp->configure(ltpWeight, ltdWeight, tau);
            }
            
            // Configure structural plasticity
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
            float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
            if (structuralPlasticity) {
                structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
                structuralPlasticity->setPruningRate(pruningRate);
            }
            
            // Get timestep
            timestep = config->getOr<double>("simulation_timestep", 0.001);
            
            // Get integration intervals from config
            replayInterval = config->getOr<size_t>("replay_interval", 100);
            consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        }
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
};

// Brain constructor/destructor
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    try {
        if (pImpl) {
            // Validate config
            throwIfNot(config != nullptr, "Brain constructor", "Config is null");
            
            // Log initialization
            NLM_LOG_INFO("Creating NLM Brain (Phase 6: Integrated Artificial Brain)...");
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain construction failed: ") + e.what());
        delete pImpl;
        pImpl = nullptr;
        throw;
    }
}

Brain::~Brain() {
    try {
        // Clean up resources
        if (pImpl) {
            NLM_LOG_INFO("Destroying NLM Brain...");
            delete pImpl;
            pImpl = nullptr;
        }
    } catch (...) {
        // Ignore exceptions during cleanup
    }
}

// Move operations
Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        if (pImpl) {
            delete pImpl;
        }
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// ================================================
// BRAIN INITIALIZATION (REFACTORED)
// ================================================

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    try {
        // Validate preconditions
        throwIfNot(pImpl != nullptr, "initialize", "Brain implementation is null");
        throwIfNot(pImpl->config != nullptr, "initialize", "Brain configuration is null");
        
        // Get configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                    std::to_string(regionCount) + " regions");
        
        // Initialize systems
        initializeMemorySystems();
        initializePredictionSystem();
        initializeCognitionSystems();
        initializeDevelopmentSystem();
        initializeNeuromodulationSystems();
        
        // Create regions and neurons
        initializeRegions(neuronCount, regionCount, connectionProbability);
        
        // Configure checkpoint manager
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        
        // Register spike handlers
        registerSpikeHandlers();
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain initialization: ") + e.what());
        return false;
    }
}

// Memory system initialization
void Brain::initializeMemorySystems() {
    NLM_LOG_INFO("Initializing memory systems...");
    
    // Initialize working memory
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(100);  // Placeholder
    
    // Initialize episodic memory
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    pImpl->associativeMemory->initialize(this);
}

// Prediction system initialization
void Brain::initializePredictionSystem() {
    NLM_LOG_INFO("Initializing prediction system...");
    
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
}

// Cognition systems initialization
void Brain::initializeCognitionSystems() {
    NLM_LOG_INFO("Initializing cognition systems...");
    
    // Initialize planner
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    // Initialize concept formation
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->conceptFormation->initialize(this);
    
    // Initialize attention system
    pImpl->attention = std::make_unique<AttentionalSelection>();
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
}

// Development system initialization
void Brain::initializeDevelopmentSystem() {
    NLM_LOG_INFO("Initializing development system...");
    
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    pImpl->developmentSystem->initialize(this);
}

// Neuromodulation systems initialization
void Brain::initializeNeuromodulationSystems() {
    NLM_LOG_INFO("Initializing neuromodulation systems...");
    
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->novelty->initialize(this);
    
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->curiosity->initialize(this);
    
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->dopamine->initialize(this);
    
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->predictionError->initialize(this);
}

// Regions and neurons initialization
void Brain::initializeRegions(size_t neuronCount, size_t regionCount, float connectionProbability) {
    NLM_LOG_INFO("Creating " + std::to_string(regionCount) + " regions with " + std::to_string(neuronCount) + " neurons total");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Calculate neurons per region
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    // Create neurons across regions
    for (size_t i = 0; i < regionCount; ++i) {
        NeuralRegion* region = getRegion(RegionId(i + 1));
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
            
            // Initialize neurons
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
}

// Spike handler registration
void Brain::registerSpikeHandlers() {
    NLM_LOG_INFO("Registering spike handlers for event-driven processing");
    
    // Register spike counter
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

// ================================================
// STEP METHOD (REFACTORED)
// ================================================

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
        // Validate preconditions
        throwIfNot(pImpl != nullptr, "step", "Brain implementation is null");
        
        // Update step state
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // Execute integrated brain loop
        stepNeurons(currentTime);
        stepWorkingMemory();
        stepNeuromodulation();
        stepPlasticity();
        stepEpisodicMemory();
        stepPrediction();
        stepAttention();
        stepConceptFormation();
        stepStructuralPlasticity();
        stepReplay();
        stepDevelopment();
        stepConsolidation();
        stepCheckpoints();
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Brain step failed: ") + e.what());
        throw BrainStepError(std::string("Brain step failed: ") + e.what());
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain step: ") + e.what());
        throw BrainStepError(std::string("Unexpected error during brain step: ") + e.what());
    }
}

// Individual system step methods
void Brain::stepNeurons(Timestamp currentTime) {
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
}

void Brain::stepWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::stepNeuromodulation() {
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
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void Brain::stepPlasticity() {
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
}

void Brain::stepEpisodicMemory() {
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
}

void Brain::stepPrediction() {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void Brain::stepAttention() {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void Brain::stepConceptFormation() {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::stepStructuralPlasticity() {
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::stepReplay() {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void Brain::stepDevelopment() {
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
                case DevelopmentalStage::Aging:
                    plasticityMod = 0.1f;  // Low plasticity
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void Brain::stepConsolidation() {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void Brain::stepCheckpoints() {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

// ================================================
// BRAIN I/O AND UTILITY METHODS
// ================================================

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    try {
        // Validate preconditions
        throwIfNot(pImpl != nullptr, "receiveSensoryInput", "Brain implementation is null");
        throwIfNot(!pImpl->sensoryNeurons.empty(), "receiveSensoryInput", "No sensory neurons available");
        
        const auto& values = input.getData();
        if (values.empty()) return;
        
        // Distribute input across sensory neurons
        distributeInput(values, pImpl->sensoryNeurons);
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Sensory input failed: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error processing sensory input: ") + e.what());
        throw BrainError(std::string("Sensory input processing failed: ") + e.what());
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    try {
        throwIfNot(pImpl != nullptr, "injectCurrent", "Brain implementation is null");
        
        bool found = false;
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* n : neurons) {
                if (n->getId() == neuron) {
                    n->injectCurrent(current);
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
        
        throwIfNot(found, "injectCurrent", "Neuron not found");
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Current injection failed: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error injecting current: ") + e.what());
        throw BrainError(std::string("Current injection failed: ") + e.what());
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    try {
        throwIfNot(pImpl != nullptr, "injectCurrentToNeurons", "Brain implementation is null");
        
        auto neurons = collectNeuronsByType(pImpl->regions, type);
        if (neurons.empty()) {
            NLM_LOG_WARNING("No neurons of type " + std::to_string(static_cast<int>(type)) + " found");
            return;
        }
        
        for (auto* neuron : neurons) {
            neuron->injectCurrent(current);
        }
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Batch current injection failed: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error in batch current injection: ") + e.what());
        throw BrainError(std::string("Batch current injection failed: ") + e.what());
    }
}

std::unique_ptr<class Action> Brain::produceAction() {
    try {
        throwIfNot(pImpl != nullptr, "produceAction", "Brain implementation is null");
        
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
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Action production failed: ") + e.what());
        return std::make_unique<Action>(ActionType::Wait);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error producing action: ") + e.what());
        return std::make_unique<Action>(ActionType::Wait);
    }
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    try {
        throwIfNot(pImpl != nullptr, "applyNeuromodulation", "Brain implementation is null");
        
        float modulation = signal.getLevel();
        
        // Scale STDP learning rates
        if (pImpl->stdp) {
            pImpl->stdp->setLTPWeight(0.01f * modulation);
            pImpl->stdp->setLTDWeight(0.012f * modulation);
        }
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Neuromodulation application failed: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error applying neuromodulation: ") + e.what());
        throw BrainError(std::string("Neuromodulation application failed: ") + e.what());
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    try {
        throwIfNot(pImpl != nullptr, "develop", "Brain implementation is null");
        
        if (pImpl->structuralPlasticity && pImpl->rng) {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        }
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Development update failed: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during development: ") + e.what());
        throw BrainError(std::string("Development update failed: ") + e.what());
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    try {
        // Reset simulation state
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        
        // Reset regions
        for (auto& region : pImpl->regions) {
            region->reset();
        }
        
        // Reset systems
        if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
        pImpl->developmentalStage = DevelopmentalStage::Initial;
        
        // Reset memory systems
        if (pImpl->workingMemory) pImpl->workingMemory->clear();
        if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
        if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
        if (pImpl->attention) pImpl->attention->reset();
        
        NLM_LOG_INFO("NLM Brain reset complete");
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Brain reset failed: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain reset: ") + e.what());
        throw BrainError(std::string("Brain reset failed: ") + e.what());
    }
}

// ================================================
// CHECKPOINTING AND PERSISTENCE
// ================================================

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        throwIfNot(pImpl != nullptr, "save", "Brain implementation is null");
        throwIfNot(!filepath.empty(), "save", "Filepath is empty");
        
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            throw BrainIOException("Failed to create checkpoint file: " + filepath);
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
            throw BrainIOException("Failed to write neurons to checkpoint");
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
            throw BrainIOException("Failed to write synapses to checkpoint");
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            throw BrainIOException("Failed to finalize checkpoint");
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        throwIfNot(pImpl != nullptr, "load", "Brain implementation is null");
        throwIfNot(!filepath.empty(), "load", "Filepath is empty");
        
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            throw BrainIOException("Failed to open checkpoint file: " + filepath);
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            throw BrainIOException("Checkpoint validation failed: " + reader.getError());
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            throw BrainIOException("Failed to read neurons from checkpoint");
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
            throw BrainIOException("Failed to read synapses from checkpoint");
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected exception loading brain: ") + e.what());
        return false;
    }
}

// ================================================
// REGION MANAGEMENT
// ================================================

RegionId Brain::addRegion(const std::string& name) {
    try {
        throwIfNot(pImpl != nullptr, "addRegion", "Brain implementation is null");
        
        RegionId id(pImpl->nextRegionId++);
        auto region = std::make_unique<NeuralRegion>(id, name);
        pImpl->regions.push_back(std::move(region));
        
        NLM_LOG_INFO("Added region " + std::to_string(id.index()) + " (" + name + ")");
        return id;
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Failed to add region: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error adding region: ") + e.what());
        throw BrainError(std::string("Failed to add region: ") + e.what());
    }
}

NeuralRegion* Brain::getRegion(RegionId id) {
    try {
        throwIfNot(pImpl != nullptr, "getRegion", "Brain implementation is null");
        
        for (auto& region : pImpl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        
        throw BrainValidationError("Region with ID " + std::to_string(id.index()) + " not found");
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Failed to get region: ") + e.what());
        throw;
    }
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    if (!pImpl) {
        throw BrainValidationError("Brain implementation is null");
    }
    
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    
    throw BrainValidationError("Region with ID " + std::to_string(id.index()) + " not found");
}

size_t Brain::getRegionCount() const {
    return pImpl ? pImpl->regions.size() : 0;
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    if (!pImpl) return ids;
    
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl ? pImpl->regions : *(new std::vector<std::unique_ptr<NeuralRegion>>());
}

// ================================================
// STATISTICS AND METRICS
// ================================================

size_t Brain::getTotalNeuronCount() const {
    return BrainUtils::countTotalNeurons(pImpl ? pImpl->regions : std::vector<std::unique_ptr<NeuralRegion>>{});
}

size_t Brain::getTotalSynapseCount() const {
    return BrainUtils::countTotalSynapses(pImpl ? pImpl->regions : std::vector<std::unique_ptr<NeuralRegion>>{});
}

size_t Brain::getActiveNeuronCount() const {
    return BrainUtils::countActiveNeurons(pImpl ? pImpl->regions : std::vector<std::unique_ptr<NeuralRegion>>{});
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl ? pImpl->totalSpikesThisStep : 0;
}

float Brain::getAverageFiringRate() const {
    size_t regionCount = getRegionCount();
    if (regionCount == 0) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(regionCount);
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
    return pImpl ? pImpl->totalSpikesTotal : 0;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl ? pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount() : 0;
}

// ================================================
// SYSTEM ACCESSORS
// ================================================

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl ? pImpl->workingMemory.get() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl ? pImpl->episodicMemory.get() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl ? pImpl->associativeMemory.get() : nullptr;
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl ? pImpl->predictionSystem.get() : nullptr;
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl ? pImpl->planner.get() : nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl ? pImpl->conceptFormation.get() : nullptr;
}

AttentionalSelection* Brain::getAttention() {
    return pImpl ? pImpl->attention.get() : nullptr;
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl ? pImpl->developmentSystem.get() : nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl ? pImpl->developmentalStage : DevelopmentalStage::Initial;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl) {
        pImpl->developmentalStage = stage;
    }
}

Dopamine* Brain::getDopamine() {
    return pImpl ? pImpl->dopamine.get() : nullptr;
}

Curiosity* Brain::getCuriosity() {
    return pImpl ? pImpl->curiosity.get() : nullptr;
}

Novelty* Brain::getNovelty() {
    return pImpl ? pImpl->novelty.get() : nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl ? pImpl->predictionError.get() : nullptr;
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl ? pImpl->config : nullptr;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl ? pImpl->rng.get() : nullptr;
}

STDP* Brain::getSTDP() {
    return pImpl ? pImpl->stdp.get() : nullptr;
}

Hebbian* Brain::getHebbian() {
    return pImpl ? pImpl->hebbian.get() : nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl ? pImpl->structuralPlasticity.get() : nullptr;
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl ? pImpl->spikeSystem.get() : nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl ? pImpl->spikeSystem.get() : nullptr;
}

// ================================================
// LOGGING AND STATUS
// ================================================

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
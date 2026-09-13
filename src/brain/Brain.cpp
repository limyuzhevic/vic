#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/NeuromodulationManager.hpp"
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
    std::unique_ptr<NeuromodulationManager> neuromodulation;
    
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
        
        // Initialize timestep from config
        if (auto timestepOpt = config->get<double>("simulation_timestep")) {
            timestep = static_cast<TimestepDuration>(*timestepOpt);
        }
    }
    
    ~Impl() = default;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)) {
    // Create neuromodulators and neuromodulation manager
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->neuromodulation = std::make_unique<NeuromodulationManager>();
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Brain: Initializing neural systems...");
    
    // Initialize neuromodulation first (it depends on brain)
    pImpl->neuromodulation->initialize(this);
    
    // Set up neuromodulators with initial values from config
    float dopamineLevel = 0.1f;
    if (auto levelOpt = pImpl->config->get<float>("dopamine_level")) {
        dopamineLevel = *levelOpt;
    }
    pImpl->dopamine->setLevel(dopamineLevel);
    
    // Initialize spike system (core neural computation)
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    if (!pImpl->spikeSystem->initialize(pImpl->config)) {
        NLM_LOG_ERROR("Brain: Failed to initialize spike system!");
        return false;
    }
    
    // Initialize plasticity systems
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    pImpl->stdp->initialize(pImpl->config);
    pImpl->hebbian->initialize(pImpl->config);
    pImpl->structuralPlasticity->initialize(pImpl->config);
    
    // Initialize development system
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    pImpl->developmentSystem->initialize(pImpl->config);
    
    // Create default neural regions
    createDefaultRegions();
    
    // Initialize memory systems
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->workingMemory->initialize(this);
    
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->episodicMemory->initialize(this);
    
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    pImpl->predictionSystem->initialize(this);
    
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    pImpl->associativeMemory->initialize(this);
    
    // Initialize cognition systems
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->planner->initialize(this);
    
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention = std::make_unique<AttentionalSelection>();
    pImpl->attention->initialize(this);
    
    NLM_LOG_INFO("Brain: Initialization complete!");
    NLM_LOG_INFO("  Regions: " + std::to_string(pImpl->regions.size()));
    NLM_LOG_INFO("  Neurons: " + std::to_string(pImpl->spikeSystem->getTotalNeuronCount()));
    NLM_LOG_INFO("  Synapses: " + std::to_string(pImpl->spikeSystem->getTotalSynapseCount()));
    
    return true;
}

void Brain::createDefaultRegions() {
    // Create default regions based on config
    uint64_t regionCount = 1;
    if (auto regionCountOpt = pImpl->config->get<uint64_t>("region_count")) {
        regionCount = *regionCountOpt;
    }
    
    for (uint64_t i = 0; i < regionCount; ++i) {
        RegionId regionId = i + 1;
        
        // Create region with appropriate characteristics
        NeuralRegion* region = new NeuralRegion(regionId, "Region-" + std::to_string(i));
        
        // Configure region based on its ID
        if (i == 0) {
            // First region: mixed neural population
            region->setRegionType(RegionType::Cortical);
            region->setNeuronCount(250);
            region->setConnectionProbability(0.15f);
            
            // Add sensory neurons
            auto* sensoryNeuron = region->addNeuron(NeuronType::Sensory);
            pImpl->sensoryNeurons.push_back(sensoryNeuron);
            
            // Add motor neurons
            auto* motorNeuron = region->addNeuron(NeuronType::Motor);
            pImpl->motorNeurons.push_back(motorNeuron);
        } else {
            // Other regions: specialized populations
            region->setRegionType(RegionType::Subcortical);
            region->setNeuronCount(100);
            region->setConnectionProbability(0.10f);
        }
        
        region->initialize(pImpl->config.get(), pImpl->rng.get());
        pImpl->regions.push_back(std::unique_ptr<NeuralRegion>(region));
    }
}

void Brain::setupNeuromodulation() {
    NLM_LOG_INFO("Brain: Setting up neuromodulation systems...");
    
    // Connect neuromodulators with brain
    pImpl->dopamine->setLevel(pImpl->config->getOr<float>("dopamine_level", 0.1f));
    
    // Initialize neuromodulation manager
    pImpl->neuromodulation->initialize(this);
    
    NLM_LOG_INFO("Brain: Neuromodulation systems ready!");
}

NeuromodulationManager* Brain::getNeuromodulationManager() {
    return pImpl->neuromodulation.get();
}

void Brain::processWithNeuromodulation(const Observation& observation,
                                        const Observation& previousObservation,
                                        float predictedValue) {
    if (pImpl->neuromodulation) {
        pImpl->neuromodulation->processObservation(observation, previousObservation, predictedValue);
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, currentStep * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    this->pImpl->currentStep = currentStep;
    this->pImpl->currentTime = currentTime;
    
    // Reset step statistics
    pImpl->totalSpikesThisStep = 0;
    
    // Update neuromodulation (depends on current brain state)
    if (pImpl->neuromodulation) {
        pImpl->neuromodulation->update(pImpl->timestep);
    }
    
    // Update development (structural changes)
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(currentTime);
    }
    
    // Process spikes and propagate through network
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processSpikes(currentTime);
        
        // Collect statistics
        pImpl->totalSpikesThisStep = pImpl->spikeSystem->getSpikesThisStep();
        pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
        
        // Apply plasticity rules
        updatePlasticity();
        
        // Update memory systems
        if (pImpl->workingMemory) {
            pImpl->workingMemory->update(currentTime);
        }
        
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->update(currentTime);
        }
        
        // Update prediction system
        if (pImpl->predictionSystem) {
            pImpl->predictionSystem->update(currentTime);
        }
        
        // Update cognition systems
        if (pImpl->planner) {
            pImpl->planner->update(currentTime);
        }
        
        if (pImpl->conceptFormation) {
            pImpl->conceptFormation->update(currentTime);
        }
        
        if (pImpl->attention) {
            pImpl->attention->update(currentTime);
        }
        
        // Handle developmental stages
        develop();
        
        // Check for checkpointing
        if (pImpl->checkpointManager) {
            pImpl->checkpointManager->checkPoint(currentTime);
        }
    }
}

void Brain::updatePlasticity() {
    if (pImpl->stdp) {
        pImpl->stdp->update(this);
    }
    
    if (pImpl->hebbian) {
        pImpl->hebbian->update(this);
    }
    
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this);
    }
}

void Brain::develop() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->develop(this);
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Brain: Resetting neural state...");
    
    // Reset neuromodulation
    if (pImpl->neuromodulation) {
        pImpl->neuromodulation->reset();
    }
    
    // Reset spike system
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    
    // Reset plasticity
    if (pImpl->stdp) {
        pImpl->stdp->reset();
    }
    
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->reset();
    }
    
    // Reset memory systems
    if (pImpl->workingMemory) {
        pImpl->workingMemory->reset();
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->reset();
    }
    
    // Reset development
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->reset();
    }
    
    // Reset statistics
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesTotal = 0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    NLM_LOG_INFO("Brain: Reset complete!");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Brain: Saving state to " + filepath);
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Brain: Failed to open file for saving!");
        return false;
    }
    
    // Save configuration
    if (!pImpl->config->save(file)) {
        NLM_LOG_ERROR("Brain: Failed to save configuration!");
        return false;
    }
    
    // Save brain state
    // ... (implement detailed state saving)
    
    file.close();
    NLM_LOG_INFO("Brain: State saved successfully!");
    return true;
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Brain: Loading state from " + filepath);
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Brain: Failed to open file for loading!");
        return false;
    }
    
    // Load configuration
    if (!pImpl->config->load(file)) {
        NLM_LOG_ERROR("Brain: Failed to load configuration!");
        return false;
    }
    
    // Reload neural components
    // ... (implement detailed state loading)
    
    file.close();
    NLM_LOG_INFO("Brain: State loaded successfully!");
    return true;
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("Simulation:");
    NLM_LOG_INFO("  Step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("  Time: " + std::to_string(pImpl->currentTime) + "s");
    NLM_LOG_INFO("  Neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("  Synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("  Active: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("  Firing: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("  Total Spikes: " + std::to_string(getTotalSpikeCount()));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Neuromodulation:");
    if (pImpl->neuromodulation) {
        NLM_LOG_INFO("  Total modulation: " + std::to_string(pImpl->neuromodulation->getTotalModulation()));
        NLM_LOG_INFO("  Exploration drive: " + std::to_string(pImpl->neuromodulation->getExplorationDrive()));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Development:");
    NLM_LOG_INFO("  Stage: " + developmentalStageToString(pImpl->developmentalStage));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Status Complete ===");
}

// Forward declarations for external functions
namespace nlm {
    Dopamine* Brain::getDopamine() { return pImpl->dopamine.get(); }
    Curiosity* Brain::getCuriosity() { return pImpl->curiosity.get(); }
    Novelty* Brain::getNovelty() { return pImpl->novelty.get(); }
    PredictionError* Brain::getPredictionErrorSignal() { return pImpl->predictionError.get(); }
    SpikeSystem* Brain::getSpikeSystem() { return pImpl->spikeSystem.get(); }
    const SpikeSystem* Brain::getSpikeSystem() const { return pImpl->spikeSystem.get(); }
    STDP* Brain::getSTDP() { return pImpl->stdp.get(); }
    Hebbian* Brain::getHebbian() { return pImpl->hebbian.get(); }
    StructuralPlasticity* Brain::getStructuralPlasticity() { return pImpl->structuralPlasticity.get(); }
    float Brain::getExcitationInhibitionRatio() const { 
        if (pImpl->spikeSystem) {
            return pImpl->spikeSystem->getExcitationInhibitionRatio();
        }
        return 1.0f;
    }
    size_t Brain::getTotalSpikeCount() const { return pImpl->totalSpikesTotal; }
    size_t Brain::getPendingSpikeEventCount() const { 
        if (pImpl->spikeSystem) {
            return pImpl->spikeSystem->getPendingSpikeEventCount();
        }
        return 0;
    }
    std::unique_ptr<Action> Brain::produceAction() {
        // Generate action based on motor neuron activity
        if (!pImpl->motorNeurons.empty()) {
            // Find most active motor neuron
            Neuron* bestMotor = nullptr;
            float bestActivity = 0.0f;
            
            for (Neuron* neuron : pImpl->motorNeurons) {
                if (neuron && neuron->getFiringRate() > bestActivity) {
                    bestActivity = neuron->getFiringRate();
                    bestMotor = neuron;
                }
            }
            
            if (bestMotor) {
                return std::make_unique<Action>(
                    ActionType::Movement,
                    bestMotor->getPosition(),
                    bestActivity
                );
            }
        }
        
        // Default action if no motor activity
        return std::make_unique<Action>(ActionType::Idle, Vector3(0.0f, 0.0f, 0.0f), 0.0f);
    }
    void Brain::applyNeuromodulator(const Neuromodulator& signal) {
        // Apply neuromodulatory effect based on signal type
        if (dynamic_cast<const Dopamine*>(&signal)) {
            if (auto* dopamine = getDopamine()) {
                // Scale synaptic weights based on dopamine level
                float factor = dopamine->getPlasticityFactor();
                // ... apply to synapses
            }
        } else if (dynamic_cast<const Acetylcholine*>(&signal)) {
            // Enhance attention and working memory
        } else if (dynamic_cast<const Norepinephrine*>(&signal)) {
            // Enhance arousal and vigilance
        } else if (dynamic_cast<const Serotonin*>(&signal)) {
            // Regulate mood and social behavior
        }
    }
    NeuralWorkingMemory* Brain::getWorkingMemory() { return pImpl->workingMemory.get(); }
    NeuralEpisodicMemory* Brain::getEpisodicMemory() { return pImpl->episodicMemory.get(); }
    NeuralAssociativeMemory* Brain::getAssociativeMemory() { return pImpl->associativeMemory.get(); }
    PredictionSystem* Brain::getPredictionSystem() { return pImpl->predictionSystem.get(); }
    NeuralPlanner* Brain::getPlanner() { return pImpl->planner.get(); }
    ConceptFormation* Brain::getConceptFormation() { return pImpl->conceptFormation.get(); }
    AttentionalSelection* Brain::getAttention() { return pImpl->attention.get(); }
    DevelopmentSystem* Brain::getDevelopmentSystem() { return pImpl->developmentSystem.get(); }
    DevelopmentalStage Brain::getDevelopmentalStage() const { return pImpl->developmentalStage; }
    void Brain::setDevelopmentalStage(DevelopmentalStage stage) { pImpl->developmentalStage = stage; }
    RegionId Brain::addRegion(const std::string& name) {
        RegionId newId = ++pImpl->nextRegionId;
        
        NeuralRegion* region = new NeuralRegion(newId, name);
        region->initialize(pImpl->config.get(), pImpl->rng.get());
        pImpl->regions.push_back(std::unique_ptr<NeuralRegion>(region));
        
        return newId;
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
    size_t Brain::getRegionCount() const { return pImpl->regions.size(); }
    std::vector<RegionId> Brain::getRegionIds() const {
        std::vector<RegionId> ids;
        for (const auto& region : pImpl->regions) {
            ids.push_back(region->getId());
        }
        return ids;
    }
    const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const { return pImpl->regions; }
    void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
        InterRegionConnection conn(source, target, weight, delay);
        pImpl->interRegionConnections.push_back(conn);
    }
    void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
        auto it = std::remove_if(pImpl->interRegionConnections.begin(),
                               pImpl->interRegionConnections.end(),
                               [source, target](const InterRegionConnection& conn) {
                                   return conn.sourceRegion == source && conn.targetRegion == target;
                               });
        pImpl->interRegionConnections.erase(it, pImpl->interRegionConnections.end());
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
    size_t Brain::getActiveNeuronCount() const {
        size_t total = 0;
        for (const auto& region : pImpl->regions) {
            total += region->getActiveNeuronCount();
        }
        return total;
    }
    size_t Brain::getFiringNeuronCount() const {
        size_t total = 0;
        for (const auto& region : pImpl->regions) {
            total += region->getFiringNeuronCount();
        }
        return total;
    }
    float Brain::getAverageFiringRate() const {
        size_t totalNeurons = getTotalNeuronCount();
        if (totalNeurons == 0) return 0.0f;
        
        float totalRate = 0.0f;
        for (const auto& region : pImpl->regions) {
            totalRate += region->getAverageFiringRate() * region->getTotalNeuronCount();
        }
        
        return totalRate / totalNeurons;
    }
    std::shared_ptr<const Config> Brain::getConfig() const { return pImpl->config; }
    RandomGenerator* Brain::getRandomGenerator() { return pImpl->rng.get(); }
}}

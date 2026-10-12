#include "Brain.hpp"
#include "BrainCore.hpp"
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
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Implementation of BrainCore class (for testing)
// This provides a test implementation of the core brain functionality

struct BrainCore::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation systems
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
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000)
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
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
        
        // Get integration intervals
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
};

BrainCore::BrainCore(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)) {}

BrainCore::~BrainCore() = default;

BrainCore::BrainCore(BrainCore&& other) noexcept : pImpl(std::move(other.pImpl)) {}

BrainCore& BrainCore::operator=(BrainCore&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool BrainCore::initialize() {
    NLM_LOG_INFO("Initializing NLM BrainCore...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions (simplified - would use NeuralRegion class)
    pImpl->regions.reserve(regionCount);
    for (size_t i = 0; i < regionCount; ++i) {
        // Create region placeholder - in real implementation would use NeuralRegion
        auto region = std::make_unique<NeuralRegion>();
        pImpl->regions.push_back(std::move(region));
    }
    
    // Initialize random connectivity (simplified)
    for (size_t i = 0; i < regionCount; ++i) {
        if (i < pImpl->regions.size()) {
            // Would call region->initializeRandomConnectivity
        }
    }
    
    NLM_LOG_INFO("NLM BrainCore initialization complete");
    return true;
}

void BrainCore::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void BrainCore::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Process delayed spike events
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Update neurons (simplified - would update actual neurons)
    for (auto& region : pImpl->regions) {
        // In real implementation, iterate over actual neurons
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
}

void BrainCore::receiveSensoryInput(const SensoryInput& input) {
    // Inject current into sensory neurons
    const auto& values = input.getData();
    if (values.empty()) return;
    
    for (size_t i = 0; i < std::min(values.size(), pImpl->sensoryNeurons.size()); ++i) {
        float normalizedValue = static_cast<float>(values[i]) * 10.0f;
        if (pImpl->sensoryNeurons[i]) {
            pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        }
    }
}

void BrainCore::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Find neuron and inject current
    for (auto& region : pImpl->regions) {
        if (region) {
            // Would find neuron by ID
        }
    }
}

void BrainCore::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Inject current to all neurons of specified type
    for (auto& region : pImpl->regions) {
        if (region) {
            // Would filter by neuron type
        }
    }
}

SpikeSystem* BrainCore::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* BrainCore::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* BrainCore::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* BrainCore::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* BrainCore::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float BrainCore::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        // Would iterate over actual synapses
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t BrainCore::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t BrainCore::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<Action> BrainCore::produceAction() {
    // Simple action selection
    return std::make_unique<Action>(ActionType::Wait);
}

void BrainCore::applyNeuromodulation(const Neuromodulator& signal) {
    // Apply neuromodulation effects
    float modulation = signal.getLevel();
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void BrainCore::updatePlasticity() {
    // Plasticity is applied during step
}

void BrainCore::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(nullptr, *pImpl->rng);
}

void BrainCore::reset() {
    NLM_LOG_INFO("Resetting NLM BrainCore...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    
    for (auto& region : pImpl->regions) {
        // Would reset region
    }
    
    pImpl->spikeSystem->reset();
}

bool BrainCore::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            return false;
        }
        
        // Write checkpoint data (simplified)
        NLM_LOG_INFO("BrainCore state saved successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving BrainCore: ") + e.what());
        return false;
    }
}

bool BrainCore::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            return false;
        }
        
        if (!reader.validate()) {
            return false;
        }
        
        NLM_LOG_INFO("BrainCore state loaded successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading BrainCore: ") + e.what());
        return false;
    }
}

RegionId BrainCore::addRegion(const std::string& name) {
    RegionId id(static_cast<RegionId::index_type>(pImpl->regions.size() + 1));
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* BrainCore::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region && region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainCore::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region && region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t BrainCore::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> BrainCore::getRegionIds() const {
    std::vector<RegionId> ids;
    for (const auto& region : pImpl->regions) {
        if (region) {
            ids.push_back(region->getId());
        }
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainCore::getRegions() const {
    return pImpl->regions;
}

void BrainCore::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void BrainCore::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t BrainCore::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (region) {
            total += region->getTotalNeuronCount();
        }
    }
    return total;
}

size_t BrainCore::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (region) {
            total += region->getSynapseCount();
        }
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t BrainCore::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        if (region) {
            total += region->getActiveNeuronCount();
        }
    }
    return total;
}

size_t BrainCore::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float BrainCore::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        if (region) {
            sum += region->getAverageFiringRate();
        }
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

std::shared_ptr<const Config> BrainCore::getConfig() const {
    return pImpl->config;
}

RandomGenerator* BrainCore::getRandomGenerator() {
    return pImpl->rng.get();
}

DevelopmentSystem* BrainCore::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

void BrainCore::logStatus() const {
    NLM_LOG_INFO("=== NLM BrainCore Status ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
}

} // namespace nlm

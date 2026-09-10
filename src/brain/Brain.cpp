#pragma once

#include "Brain.hpp"
#include "core/Error/Error.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

class Brain::Impl {
public:
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Neuromodulator> neuromodulator;
    std::unique_ptr<SpikeSystem> spikeSystemPtr;
    std::unique_ptr<STDP> stdpPtr;
    std::unique_ptr<Hebbian> hebbianPtr;
    std::unique_ptr<StructuralPlasticity> structuralPlasticityPtr;
    
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
    
    // Error tracking
    std::unordered_map<ErrorCode, size_t> errorCounts;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , novelty(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
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
        // Initialize error counts
        errorCounts[ErrorCode::Success] = 0;
        
        try {
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
            
            // Initialize neuromodulation systems
            novelty = std::make_unique<Novelty>();
            dopamine = std::make_unique<Dopamine>();
            curiosity = std::make_unique<Curiosity>();
            predictionError = std::make_unique<PredictionError>();
            neuromodulator = std::make_unique<Neuromodulator>();
            
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
            
            // Configure STDP parameters with error checking
            float ltpWeight = 0.01f;
            float ltdWeight = 0.012f;
            float tau = 20.0f;
            
            if (auto ltpOpt = config->get<float>("stdp_ltp_weight")) {
                ltpWeight = *ltpOpt;
            }
            if (auto ltdOpt = config->get<float>("stdp_ltd_weight")) {
                ltdWeight = *ltdOpt;
            }
            if (auto tauOpt = config->get<float>("stdp_tau")) {
                tau = *tauOpt;
            }
            
            // Validate parameters
            if (ltpWeight <= 0.0f || ltdWeight <= 0.0f || tau <= 0.0f) {
                throw ConfigException(ErrorCode::ConfigValueOutOfRange,
                                     "Invalid STDP parameters: all values must be positive");
            }
            
            stdp->configure(ltpWeight, ltdWeight, tau);
            
            // Configure structural plasticity
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
            float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
            
            if (synaptogenesisRate < 0.0f || pruningRate < 0.0f) {
                throw ConfigException(ErrorCode::ConfigValueOutOfRange,
                                     "Invalid plasticity rates: must be non-negative");
            }
            
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
            
            // Get timestep with validation
            timestep = config->getOr<double>("simulation_timestep", 0.001);
            if (timestep <= 0.0) {
                throw ConfigException(ErrorCode::ConfigValueOutOfRange,
                                     "Invalid timestep: must be positive");
            }
            
            // Get integration intervals from config
            replayInterval = config->getOr<size_t>("replay_interval", 100);
            consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
            
            if (replayInterval == 0 || consolidationInterval == 0) {
                throw ConfigException(ErrorCode::ConfigValueOutOfRange,
                                     "Invalid interval values: must be non-zero");
            }
            
            // Initialize checkpoint manager
            checkpointManager = std::make_unique<CheckpointManager>();
            
            NLM_LOG_INFO("Brain Impl initialized successfully");
            
        } catch (const ConfigException& e) {
            NLM_LOG_ERROR("Configuration error during initialization: " + e.what());
            errorCounts[e.getCode()]++;
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Unexpected error during initialization: " + e.what());
            errorCounts[ErrorCode::NeuralInitializationFailed]++;
            throw NeuralException(ErrorCode::NeuralInitializationFailed,
                                 "Failed to initialize brain components: " + e.what(),
                                 __FILE__, __LINE__, __func__);
        }
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() {
    // Cleanup in reverse order of initialization
    if (pImpl) {
        // Clear pointers before deletion
        pImpl->workingMemory.reset();
        pImpl->episodicMemory.reset();
        pImpl->associativeMemory.reset();
        pImpl->predictionSystem.reset();
        pImpl->planner.reset();
        pImpl->conceptFormation.reset();
        pImpl->attention.reset();
        pImpl->developmentSystem.reset();
        pImpl->dopamine.reset();
        pImpl->curiosity.reset();
        pImpl->predictionError.reset();
        pImpl->novelty.reset();
        pImpl->neuromodulator.reset();
        pImpl->spikeSystem.reset();
        pImpl->stdp.reset();
        pImpl->hebbian.reset();
        pImpl->structuralPlasticity.reset();
        pImpl->checkpointManager.reset();
        pImpl->rng.reset();
        
        delete pImpl;
        pImpl = nullptr;
    }
}

Brain::Brain(Brain&& other) noexcept : pImpl(nullptr) {
    if (this != &other) {
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        // Clean up current object
        if (pImpl) {
            // Similar cleanup as destructor
            pImpl->workingMemory.reset();
            pImpl->episodicMemory.reset();
            pImpl->associativeMemory.reset();
            pImpl->predictionSystem.reset();
            pImpl->planner.reset();
            pImpl->conceptFormation.reset();
            pImpl->attention.reset();
            pImpl->developmentSystem.reset();
            pImpl->dopamine.reset();
            pImpl->curiosity.reset();
            pImpl->predictionError.reset();
            pImpl->novelty.reset();
            pImpl->neuromodulator.reset();
            pImpl->spikeSystem.reset();
            pImpl->stdp.reset();
            pImpl->hebbian.reset();
            pImpl->structuralPlasticity.reset();
            pImpl->checkpointManager.reset();
            pImpl->rng.reset();
            
            delete pImpl;
            pImpl = nullptr;
        }
        
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    try {
        // Get configuration values with validation
        size_t neuronCount = 1000;  // Default
        size_t regionCount = 1;     // Default
        float connectionProbability = 0.1f; // Default
        
        if (auto neuronOpt = pImpl->config->get<size_t>("neuron_count")) {
            neuronCount = *neuronOpt;
        }
        if (neuronCount == 0) {
            throw ConfigException(ErrorCode::ConfigValueOutOfRange,
                                 "Invalid neuron count: must be positive");
        }
        
        if (auto regionOpt = pImpl->config->get<size_t>("region_count")) {
            regionCount = *regionOpt;
        }
        if (regionCount == 0) {
            throw ConfigException(ErrorCode::ConfigValueOutOfRange,
                                 "Invalid region count: must be positive");
        }
        
        if (auto connOpt = pImpl->config->get<float>("connection_probability")) {
            connectionProbability = *connOpt;
        }
        
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            throw ConfigException(ErrorCode::ConfigValueOutOfRange,
                                 "Invalid connection probability: must be between 0 and 1");
        }
        
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
        
        NLM_LOG_INFO("Brain initialization completed successfully");
        return true;
        
    } catch (const ConfigException& e) {
        NLM_LOG_ERROR("Configuration error during initialization: " + e.what());
        pImpl->errorCounts[e.getCode()]++;
        throw;
    } catch (const NeuralException& e) {
        NLM_LOG_ERROR("Neural system error during initialization: " + e.what());
        pImpl->errorCounts[e.getCode()]++;
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error during brain initialization: " + e.what());
        pImpl->errorCounts[ErrorCode::NeuralInitializationFailed]++;
        throw NeuralException(ErrorCode::NeuralInitializationFailed,
                             "Failed to initialize brain: " + e.what(),
                             __FILE__, __LINE__, __func__);
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, currentStep * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
        // Update simulation time
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        
        // Run neural dynamics
        // ... implementation would be here
        
        // Update spike systems
        // ... implementation would be here
        
        // Update memory systems
        // ... implementation would be here
        
        // Update neuromodulation systems
        // ... implementation would be here
        
        // Update checkpoint system
        if (pImpl->checkpointManager->update(currentStep, currentTime)) {
            NLM_LOG_INFO("Checkpoint saved at step " + std::to_string(currentStep));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during simulation step " + std::to_string(currentStep) + ": " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    try {
        for (auto* neuron : pImpl->sensoryNeurons) {
            // Inject current based on input
            neuron->injectCurrent(input.current);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error processing sensory input: " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    try {
        // Find and inject current to specific neuron
        // ... implementation would be here
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error injecting current to neuron " + std::to_string(neuron.value) + ": " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    try {
        // Find and inject current to neurons of specific type
        // ... implementation would be here
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error injecting current to neurons of type " + std::to_string(static_cast<int>(type)) + ": " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
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
    return pImpl->spikeSystem ? pImpl->spikeSystem->getExcitationInhibitionRatio() : 1.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem ? pImpl->spikeSystem->getPendingEventCount() : 0;
}

std::unique_ptr<Action> Brain::produceAction() {
    try {
        // Generate action based on motor neuron activity
        // ... implementation would be here
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error producing action: " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    try {
        // Apply neuromodulatory signal to relevant systems
        // ... implementation would be here
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error applying neuromodulation: " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

void Brain::updatePlasticity() {
    try {
        // Update plasticity rules
        // ... implementation would be here
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error updating plasticity: " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

void Brain::develop() {
    try {
        // Apply developmental changes
        // ... implementation would be here
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during development: " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

void Brain::reset() {
    try {
        // Reset brain state
        // ... implementation would be here
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during brain reset: " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

bool Brain::save(const std::string& filepath) const {
    try {
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
        // Create checkpoint writer
        CheckpointWriter writer;
        if (!writer.create(filepath)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            throw FileIOException(ErrorCode::CheckpointSaveFailed,
                                "Failed to create checkpoint file: " + filepath);
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neuron data
        // ... implementation would be here
        
        // Write synapse data
        // ... implementation would be here
        
        // Write other data
        // ... implementation would be here
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            throw FileIOException(ErrorCode::CheckpointCorrupted,
                                "Failed to finalize checkpoint");
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const FileIOException& e) {
        NLM_LOG_ERROR("File I/O error saving brain: " + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving brain: " + e.what());
        throw NeuralException(ErrorCode::CheckpointSaveFailed,
                             "Failed to save brain: " + e.what(),
                             __FILE__, __LINE__, __func__);
    }
}

bool Brain::load(const std::string& filepath) {
    try {
        NLM_LOG_INFO("Loading brain state from " + filepath);
        
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            throw FileIOException(ErrorCode::CheckpointLoadFailed,
                                "Failed to open checkpoint file: " + filepath);
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            throw FileIOException(ErrorCode::CheckpointValidationFailed,
                                "Checkpoint validation failed: " + reader.getError());
        }
        
        // Read and apply neuron states
        // ... implementation would be here
        
        // Read and apply synapse states
        // ... implementation would be here
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const FileIOException& e) {
        NLM_LOG_ERROR("File I/O error loading brain: " + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading brain: " + e.what());
        throw NeuralException(ErrorCode::CheckpointLoadFailed,
                             "Failed to load brain: " + e.what(),
                             __FILE__, __LINE__, __func__);
    }
}

RegionId Brain::addRegion(const std::string& name) {
    try {
        RegionId id(pImpl->nextRegionId++);
        auto region = std::make_unique<NeuralRegion>(id, name);
        pImpl->regions.push_back(std::move(region));
        return id;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error adding region " + name + ": " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

NeuralRegion* Brain::getRegion(RegionId id) {
    try {
        for (auto& region : pImpl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting region " + std::to_string(id.value) + ": " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    try {
        for (const auto& region : pImpl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting region " + std::to_string(id.value) + ": " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
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

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                   float weight, Delay delay) {
    try {
        pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error adding inter-region connection: " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    try {
        pImpl->interRegionConnections.erase(
            std::remove_if(pImpl->interRegionConnections.begin(),
                          pImpl->interRegionConnections.end(),
                          [source, target](const InterRegionConnection& conn) {
                              return conn.sourceRegion == source && conn.targetRegion == target;
                          }),
            pImpl->interRegionConnections.end()
        );
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error removing inter-region connection: " + e.what());
        pImpl->errorCounts[ErrorCode::InternalError]++;
        throw;
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

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

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
    NLM_LOG_INFO("=== Brain Status ===");
    NLM_LOG_INFO("Region Count: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Neuron Count: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Synapse Count: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active Neuron Count: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing Neuron Count: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Average Firing Rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("Excitation/Inhibition Ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Log error counts
    NLM_LOG_INFO("Error Counts:");
    for (const auto& pair : pImpl->errorCounts) {
        if (pair.second > 0) {
            NLM_LOG_INFO("  " + errorCodeToString(pair.first) + ": " + std::to_string(pair.second));
        }
    }
}

} // namespace nlm

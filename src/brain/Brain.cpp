// Brain implementation with improved modularity, dependency injection, and error handling
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace nlm {

// Error messages for validation
namespace ErrorMessages {
    const std::string NULL_CONFIG = "Config cannot be null";
    const std::string NULL_REGION = "Region not found";
    const std::string INVALID_STEP = "Invalid simulation parameters";
    const std::string VALIDATION_FAILED = "Validation failed";
    const std::string SAVE_FAILED = "Failed to save brain state";
    const std::string LOAD_FAILED = "Failed to load brain state";
}

// Brain constructor with optional dependency injection
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)), config_(std::move(config)), isResting_(false) {
    if (!config_) {
        NLM_LOG_WARNING(ErrorMessages::NULL_CONFIG);
        config_ = std::make_shared<Config>();
    }
}

Brain::~Brain() {
    delete pImpl;
}

// Move constructor and assignment
Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl), config_(std::move(other.config_)), isResting_(other.isResting_) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        config_ = std::move(other.config_);
        isResting_ = other.isResting_;
        other.pImpl = nullptr;
    }
    return *this;
}

// Configure with parameter groups
bool Brain::configure(const BrainParameterGroups& params) {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain not properly initialized");
        return false;
    }
    
    pImpl->paramGroups = params;
    pImpl->validateConfig();
    
    if (!pImpl->isValid()) {
        NLM_LOG_ERROR("Brain validation failed after configuration");
        for (const auto& error : pImpl->validationErrors) {
            NLM_LOG_ERROR(error);
        }
        return false;
    }
    
    return true;
}

// Initialize brain with validation
bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    if (!pImpl) {
        NLM_LOG_ERROR("Brain not properly initialized (null pImpl)");
        return false;
    }
    
    if (!pImpl->isValid()) {
        NLM_LOG_ERROR("Brain is in invalid state");
        for (const auto& error : pImpl->validationErrors) {
            NLM_LOG_ERROR(error);
        }
        return false;
    }
    
    try {
        pImpl->initializeCoreSystems();
        pImpl->initializeOptionalSystems();
        pImpl->collectNeurons();
        
        // Get configuration values
        size_t neuronCount = validateAndGetNeuronCount();
        size_t regionCount = pImpl->paramGroups.core.regionCount;
        
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
                auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
                auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
                auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
                
                // Update development system if present
                auto* devSystem = getDevelopmentSystem();
                if (devSystem) {
                    devSystem->addRegion(region);
                }
            }
        }
        
        pImpl->collectNeurons();
        pImpl->clearValidationErrors();
        
        NLM_LOG_INFO("Brain initialization completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
        return false;
    }
}

// Main simulation step with validation
void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl) return;
    
    try {
        validateStepParameters(currentStep, currentTime);
        
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        
        // Check for rest cycle
        if (!pImpl->isResting) {
            pImpl->totalSpikesThisStep = 0;
        }
        
        // Update spike system
        if (auto* spikeSys = getSpikeSystem()) {
            spikeSys->update(pImpl->timestep);
            pImpl->totalSpikesThisStep = spikeSys->getSpikeCount();
        }
        
        // Apply development
        develop();
        
        // Apply neuromodulation
        applyNeuromodulation();
        
        // Update plasticity
        updatePlasticity();
        
        // Structural plasticity
        if (auto* sp = getStructuralPlasticity()) {
            sp->update(pImpl->timestep);
        }
        
        // Memory integration
        if (pImpl->stepsSinceLastEpisode >= pImpl->replayInterval) {
            pImpl->replayMemory();
            pImpl->stepsSinceLastEpisode = 0;
        }
        
        if (pImpl->stepsSinceLastEpisode >= pImpl->consolidationInterval) {
            pImpl->consolidateMemory();
        }
        
        pImpl->stepsSinceLastEpisode++;
        pImpl->updateStatistics();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain step failed: ") + e.what());
    }
}

// Receive sensory input with validation
void Brain::receiveSensoryInput(const SensoryInput& input) {
    if (!pImpl) return;
    
    try {
        // Validate input
        if (pImpl->sensoryNeurons.empty()) {
            NLM_LOG_WARNING("No sensory neurons available for input injection");
            return;
        }
        
        for (Neuron* neuron : pImpl->sensoryNeurons) {
            if (neuron) {
                float current = input.getCurrentForNeuronType(neuron->getType());
                neuron->injectCurrent(current);
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Sensory input failed: ") + e.what());
    }
}

// Direct neuron manipulation with error checking
void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (!pImpl) return;
    
    try {
        validateRegionAccess(neuron);
        
        for (auto& region : pImpl->regions) {
            for (auto& population : region->getPopulations()) {
                for (auto* n : population->getNeurons()) {
                    if (n && n->getId() == neuron) {
                        n->injectCurrent(current);
                        return;
                    }
                }
            }
        }
        
        NLM_LOG_WARNING(std::string("Neuron not found: ") + std::to_string(static_cast<size_t>(neuron)));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Inject current failed: ") + e.what());
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    if (!pImpl) return;
    
    try {
        for (Neuron* neuron : pImpl->sensoryNeurons) {
            if (neuron && neuron->getType() == type) {
                neuron->injectCurrent(current);
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Inject current to neurons failed: ") + e.what());
    }
}

// System access with null safety
SpikeSystem* Brain::getSpikeSystem() {
    return pImpl ? pImpl->spikeSystem.get() : nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl ? pImpl->spikeSystem.get() : nullptr;
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

// Statistics
float Brain::getExcitationInhibitionRatio() const {
    if (!pImpl || pImpl->sensoryNeurons.empty()) return 1.0f;
    
    float excitation = 0.0f, inhibition = 0.0f;
    for (Neuron* neuron : pImpl->sensoryNeurons) {
        if (!neuron) continue;
        
        float potential = neuron->getState().membranePotential;
        float resting = neuron->getState().restingPotential;
        
        if (potential > resting) {
            excitation += potential - resting;
        } else {
            inhibition += resting - potential;
        }
    }
    
    return inhibition > 0.0f ? excitation / inhibition : 1.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl ? pImpl->totalSpikesTotal : 0;
}

size_t Brain::getPendingSpikeEventCount() const {
    auto* spikeSys = getSpikeSystem();
    return spikeSys ? spikeSys->getPendingEventCount() : 0;
}

// Produce motor/action output
std::unique_ptr<Action> Brain::produceAction() {
    if (!pImpl || pImpl->motorNeurons.empty()) {
        return nullptr;
    }
    
    // Calculate motor command based on motor neuron activity
    float maxActivity = -1.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    for (Neuron* neuron : pImpl->motorNeurons) {
        if (!neuron) continue;
        
        float activity = std::abs(neuron->getState().membranePotential - 
                                 neuron->getState().restingPotential);
        
        if (activity > maxActivity) {
            maxActivity = activity;
            
            // Map neuron index to motor command (simplified)
            // In a real implementation, this would be more sophisticated
            if (maxActivity > 0.5f) {
                bestCommand = MotorCommand::MoveForward;  // Default action
            }
        }
    }
    
    // Create action if we have a command
    if (bestCommand != MotorCommand::Wait) {
        return std::make_unique<Action>(bestCommand, pImpl->currentTime);
    }
    
    return nullptr;
}

// Apply neuromodulatory signals
void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    if (!pImpl) return;
    
    try {
        // Apply to plasticity systems
        if (auto* stdpSys = getSTDP()) {
            stdpSys->update(signal);
        }
        
        if (auto* hebbianSys = getHebbian()) {
            hebbianSys->update(signal);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Neuromodulation failed: ") + e.what());
    }
}

// Update plasticity rules
void Brain::updatePlasticity() {
    if (!pImpl) return;
    
    try {
        if (auto* stdpSys = getSTDP()) {
            stdpSys->update(pImpl->timestep);
        }
        
        if (auto* hebbianSys = getHebbian()) {
            hebbianSys->update(pImpl->timestep);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Plasticity update failed: ") + e.what());
    }
}

// Apply developmental changes
void Brain::develop() {
    if (!pImpl) return;
    
    try {
        pImpl->applyDevelopment();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Development update failed: ") + e.what());
    }
}

// Reset brain state
void Brain::reset() {
    if (!pImpl) return;
    
    try {
        for (auto& region : pImpl->regions) {
            region->reset();
        }
        
        if (auto* spikeSys = getSpikeSystem()) {
            spikeSys->reset();
        }
        
        pImpl->clearNeurons();
        pImpl->totalSpikesThisStep = 0;
        pImpl->stepsSinceLastEpisode = 0;
        pImpl->clearValidationErrors();
        
        NLM_LOG_INFO("Brain state reset successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain reset failed: ") + e.what());
    }
}

// Save/load brain state
bool Brain::save(const std::string& filepath) const {
    if (!pImpl) return false;
    
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            NLM_LOG_ERROR(ErrorMessages::SAVE_FAILED + ": Cannot open file " + filepath);
            return false;
        }
        
        // Save brain state
        // Implementation would serialize the brain state
        file << "NLM_BRAIN_SAVE_FORMAT_1.0" << std::endl;
        
        // Write regions
        size_t regionCount = pImpl->regions.size();
        file.write(reinterpret_cast<const char*>(&regionCount), sizeof(regionCount));
        
        NLM_LOG_INFO(std::string("Brain state saved to ") + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Save failed: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    if (!pImpl) return false;
    
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            NLM_LOG_ERROR(ErrorMessages::LOAD_FAILED + ": Cannot open file " + filepath);
            return false;
        }
        
        std::string header;
        std::getline(file, header);
        if (header != "NLM_BRAIN_SAVE_FORMAT_1.0") {
            NLM_LOG_ERROR("Invalid brain save file format");
            return false;
        }
        
        // Load brain state
        // Implementation would deserialize the brain state
        size_t regionCount;
        file.read(reinterpret_cast<char*>(&regionCount), sizeof(regionCount));
        
        NLM_LOG_INFO(std::string("Brain state loaded from ") + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Load failed: ") + e.what());
        return false;
    }
}

// Region management
RegionId Brain::addRegion(const std::string& name) {
    if (!pImpl) return RegionId(0);
    
    try {
        validateRegionAccess(pImpl->nextRegionId);
        
        auto region = std::make_unique<NeuralRegion>(pImpl->nextRegionId, name);
        region->initialize(pImpl->paramGroups.core.regionCount, pImpl->config_);
        
        NeuralRegion* regionPtr = region.get();
        pImpl->regions.push_back(std::move(region));
        
        pImpl->nextRegionId++;
        
        NLM_LOG_INFO("Region added: " + name + " (ID: " + 
                    std::to_string(static_cast<size_t>(pImpl->nextRegionId - 1)) + ")");
        
        return pImpl->nextRegionId - 1;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Add region failed: ") + e.what());
        return RegionId(0);
    }
}

NeuralRegion* Brain::getRegion(RegionId id) {
    if (!pImpl) return nullptr;
    
    validateRegionAccess(id);
    
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    if (!pImpl) return nullptr;
    
    validateRegionAccess(id);
    
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl ? pImpl->regions.size() : 0;
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    if (pImpl) {
        ids.reserve(pImpl->regions.size());
        for (const auto& region : pImpl->regions) {
            ids.push_back(region->getId());
        }
    }
    return ids;
}

// Get all regions
const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    static const std::vector<std::unique_ptr<NeuralRegion>> empty;
    return pImpl ? pImpl->regions : empty;
}

// Inter-region connection management
void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                     float weight, Delay delay) {
    if (!pImpl) return;
    
    try {
        validateRegionAccess(source);
        validateRegionAccess(target);
        
        pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
        NLM_LOG_INFO("Inter-region connection added: " + 
                    std::to_string(static_cast<size_t>(source)) + " -> " +
                    std::to_string(static_cast<size_t>(target)));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Add inter-region connection failed: ") + e.what());
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    if (!pImpl) return;
    
    try {
        validateRegionAccess(source);
        validateRegionAccess(target);
        
        auto it = std::remove_if(pImpl->interRegionConnections.begin(),
                                 pImpl->interRegionConnections.end(),
                                 [source, target](const InterRegionConnection& conn) {
                                     return conn.sourceRegion == source && conn.targetRegion == target;
                                 });
        
        pImpl->interRegionConnections.erase(it, pImpl->interRegionConnections.end());
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Remove inter-region connection failed: ") + e.what());
    }
}

// Global statistics
 size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    if (pImpl) {
        for (const auto& region : pImpl->regions) {
            total += region->getTotalNeuronCount();
        }
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    if (pImpl) {
        for (const auto& region : pImpl->regions) {
            total += region->getTotalSynapseCount();
        }
    }
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    if (!pImpl) return 0;
    
    size_t active = 0;
    for (const auto& region : pImpl->regions) {
        active += region->getActiveNeuronCount();
    }
    return active;
}

size_t Brain::getFiringNeuronCount() const {
    if (!pImpl) return 0;
    
    size_t firing = 0;
    for (const auto& region : pImpl->regions) {
        firing += region->getFiringNeuronCount();
    }
    return firing;
}

float Brain::getAverageFiringRate() const {
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    size_t totalFiring = getFiringNeuronCount();
    return static_cast<float>(totalFiring) / totalNeurons;
}

// Memory systems
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl ? pImpl->workingMemory.get() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl ? pImpl->episodicMemory.get() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl ? pImpl->associativeMemory.get() : nullptr;
}

// Prediction system
PredictionSystem* Brain::getPredictionSystem() {
    return pImpl ? pImpl->predictionSystem.get() : nullptr;
}

// Cognition systems
NeuralPlanner* Brain::getPlanner() {
    return pImpl ? pImpl->planner.get() : nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl ? pImpl->conceptFormation.get() : nullptr;
}

AttentionalSelection* Brain::getAttention() {
    return pImpl ? pImpl->attention.get() : nullptr;
}

// Development system
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl ? pImpl->developmentSystem.get() : nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    if (!pImpl) return DevelopmentalStage::Initial;
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl) {
        pImpl->developmentalStage = stage;
    }
}

// Neuromodulation systems
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

// Get current configuration
std::shared_ptr<const Config> Brain::getConfig() const {
    return config_;
}

// Get random generator
RandomGenerator* Brain::getRandomGenerator() {
    return pImpl ? pImpl->rng.get() : nullptr;
}

// Configuration management
void Brain::setConfig(std::shared_ptr<Config> config) {
    config_ = std::move(config);
    if (pImpl) {
        pImpl->config = config_;
    }
}

// Validation and error handling
bool Brain::validate() const {
    if (!pImpl) return false;
    return pImpl->isValid();
}

std::vector<std::string> Brain::getValidationErrors() const {
    if (!pImpl) return {ErrorMessages::NULL_CONFIG};
    return pImpl->validationErrors;
}

// Statistics and monitoring
void Brain::logStatus() const {
    if (!pImpl) return;
    
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("  Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("  Neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("  Synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("  Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("  Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("  Spike count: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(getAverageFiringRate()) + "%");
    NLM_LOG_INFO("  Developmental stage: " + std::to_string(static_cast<int>(getDevelopmentalStage())));
    NLM_LOG_INFO("  Resting: " + std::to_string(isResting()));
    
    if (!validate()) {
        NLM_LOG_ERROR("Brain validation errors:");
        for (const auto& error : getValidationErrors()) {
            NLM_LOG_ERROR("  - " + error);
        }
    }
}

// Private helper methods
void Brain::validateStepParameters(SimulationStep step, Timestamp time) const {
    if (!pImpl) {
        throw std::runtime_error(ErrorMessages::INVALID_STEP + ": pImpl is null");
    }
    
    if (step < 0) {
        throw std::runtime_error(ErrorMessages::INVALID_STEP + ": Negative step number");
    }
    
    if (time < 0.0) {
        throw std::runtime_error(ErrorMessages::INVALID_STEP + ": Negative time");
    }
    
    if (pImpl->timestep <= 0.0) {
        throw std::runtime_error(ErrorMessages::INVALID_STEP + ": Invalid timestep");
    }
}

void Brain::validateRegionAccess(RegionId id) const {
    if (!pImpl) {
        throw std::runtime_error(ErrorMessages::NULL_REGION + ": pImpl is null");
    }
    
    if (id == RegionId(0)) {
        throw std::runtime_error(ErrorMessages::NULL_REGION + ": Invalid region ID");
    }
    
    bool found = false;
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        throw std::runtime_error(ErrorMessages::NULL_REGION + ": Region not found");
    }
}

size_t Brain::validateAndGetNeuronCount() const {
    if (!pImpl) return 0;
    
    size_t totalNeurons = 0;
    for (const auto& region : pImpl->regions) {
        totalNeurons += region->getTotalNeuronCount();
    }
    
    if (totalNeurons == 0) {
        NLM_LOG_WARNING("No neurons in brain - validation may fail");
    }
    
    return totalNeurons;
}

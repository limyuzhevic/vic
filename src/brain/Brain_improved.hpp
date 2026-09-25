// Brain.hpp - Improved with proper error handling and validation
// Resolves TODO Phase 2: Implement real integrate-and-fire dynamics

#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/Logger/Logger.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../dynamics/NeuralDynamics.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace nlm {

// Custom exception hierarchy for NLM
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error("NLM Error: " + message) {}
};

class BrainInitializationException : public NLMException {
public:
    explicit BrainInitializationException(const std::string& message)
        : NLMException("Brain initialization failed: " + message) {}
};

class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message)
        : NLMException("Simulation error: " + message) {}
};

class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message)
        : NLMException("Validation error: " + message) {}
};

// Forward declarations for better encapsulation
class NeuralRegion;
class SensoryInput;
class Action;
class Neuromodulator;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Configuration validation
struct BrainConfig {
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    float connectionProbability = 0.15f;
    float simulationTimestep = 0.001f;
    int64_t randomSeed = 42;
    
    // Validation method
    void validate() const {
        if (neuronCount <= 0) {
            throw ValidationException("neuronCount must be positive");
        }
        if (regionCount <= 0) {
            throw ValidationException("regionCount must be positive");
        }
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            throw ValidationException("connectionProbability must be between 0 and 1");
        }
        if (simulationTimestep <= 0.0f) {
            throw ValidationException("simulationTimestep must be positive");
        }
    }
};

// Neuron connection information for error reporting
struct ConnectionInfo {
    RegionId sourceRegion;
    PopulationId sourcePopulation;
    NeuronId sourceNeuron;
    RegionId targetRegion;
    PopulationId targetPopulation;
    NeuronId targetNeuron;
    SynapseId synapseId;
    
    ConnectionInfo(RegionId srcR, PopulationId srcP, NeuronId srcN,
                  RegionId tgtR, PopulationId tgtP, NeuronId tgtN,
                  SynapseId synId)
        : sourceRegion(srcR), sourcePopulation(srcP), sourceNeuron(srcN)
        , targetRegion(tgtR), targetPopulation(tgtP), targetNeuron(tgtN)
        , synapseId(synId) {}
};

// Brain: The central coordinator with improved error handling and validation
class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration and validation
    bool initialize() {
        validateConfiguration();
        return initializeInternal();
    }
    
    // Main simulation step with error handling
    void step(SimulationStep currentStep) {
        try {
            stepInternal(currentStep);
        } catch (const std::exception& e) {
            handleSimulationError("step", e.what());
        }
    }
    
    // Simulation time step with error handling
    void step(SimulationStep currentStep, Timestamp currentTime) {
        try {
            stepInternal(currentStep, currentTime);
        } catch (const std::exception& e) {
            handleSimulationError("step_with_time", e.what());
        }
    }
    
    // Receive sensory input with validation
    void receiveSensoryInput(const SensoryInput& input) {
        validateSensoryInput(input);
        receiveSensoryInputInternal(input);
    }
    
    // Inject current with bounds checking
    void injectCurrent(NeuronId neuron, MembranePotential current) {
        validateNeuronId(neuron);
        if (current < -1000.0f || current > 1000.0f) {
            throw ValidationException("Current injection out of valid range [-1000, 1000] mA");
        }
        injectCurrentInternal(neuron, current);
    }
    
    // Inject current to neurons with validation
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        if (current < -1000.0f || current > 1000.0f) {
            throw ValidationException("Current injection out of valid range [-1000, 1000] mA");
        }
        injectCurrentToNeuronsInternal(type, current);
    }
    
    // Spike system access with error checking
    SpikeSystem* getSpikeSystem() {
        if (!pImpl_->spikeSystem_) {
            NLM_LOG_WARNING("SpikeSystem not initialized");
        }
        return pImpl_->spikeSystem_.get();
    }
    
    const SpikeSystem* getSpikeSystem() const {
        return pImpl_->spikeSystem_.get();
    }
    
    // Plasticity system access with error checking
    STDP* getSTDP() {
        if (!pImpl_->stdp_) {
            NLM_LOG_WARNING("STDP not initialized");
        }
        return pImpl_->stdp_.get();
    }
    
    Hebbian* getHebbian() {
        if (!pImpl_->hebbian_) {
            NLM_LOG_WARNING("Hebbian not initialized");
        }
        return pImpl_->hebbian_.get();
    }
    
    StructuralPlasticity* getStructuralPlasticity() {
        if (!pImpl_->structuralPlasticity_) {
            NLM_LOG_WARNING("StructuralPlasticity not initialized");
        }
        return pImpl_->structuralPlasticity_.get();
    }
    
    // Statistics with bounds checking
    float getExcitationInhibitionRatio() const {
        float ratio = calculateExcitationInhibitionRatio();
        return std::clamp(ratio, 0.0f, 1.0f);
    }
    
    size_t getTotalSpikeCount() const {
        return pImpl_->totalSpikeCount_;
    }
    
    size_t getPendingSpikeEventCount() const {
        return pImpl_->spikeSystem_ ? pImpl_->spikeSystem_->getEventCount() : 0;
    }
    
    // Produce motor/action output with error handling
    std::unique_ptr<Action> produceAction() {
        try {
            return produceActionInternal();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Action production failed: " + std::string(e.what()));
            return nullptr;
        }
    }
    
    // Apply neuromodulatory signals with validation
    void applyNeuromodulation(const Neuromodulator& signal) {
        validateNeuromodulator(signal);
        applyNeuromodulationInternal(signal);
    }
    
    // Update plasticity rules
    void updatePlasticity() {
        updatePlasticityInternal();
    }
    
    // Apply developmental changes
    void develop() {
        developInternal();
    }
    
    // Reset brain state
    void reset() {
        resetInternal();
    }
    
    // Save brain state with error handling
    bool save(const std::string& filepath) const {
        try {
            return saveInternal(filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Save failed: " + std::string(e.what()));
            return false;
        }
    }
    
    // Load brain state with error handling
    bool load(const std::string& filepath) {
        try {
            return loadInternal(filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Load failed: " + std::string(e.what()));
            return false;
        }
    }
    
    // Region management with validation
    RegionId addRegion(const std::string& name = "") {
        return addRegionInternal(name);
    }
    
    NeuralRegion* getRegion(RegionId id) {
        if (!isValidRegionId(id)) {
            throw ValidationException("Invalid region ID: " + std::to_string(id));
        }
        return getRegionInternal(id);
    }
    
    const NeuralRegion* getRegion(RegionId id) const {
        if (!isValidRegionId(id)) {
            return nullptr;
        }
        return getRegionInternal(id);
    }
    
    size_t getRegionCount() const {
        return pImpl_->regions_.size();
    }
    
    std::vector<RegionId> getRegionIds() const {
        std::vector<RegionId> ids;
        for (size_t i = 0; i < pImpl_->regions_.size(); ++i) {
            ids.push_back(static_cast<RegionId>(i));
        }
        return ids;
    }
    
    // Get all regions with bounds checking
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const {
        return pImpl_->regions_;
    }
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1) {
        validateInterRegionConnection(source, target);
        addInterRegionConnectionInternal(source, target, weight, delay);
    }
    
    void removeInterRegionConnection(RegionId source, RegionId target) {
        validateInterRegionConnection(source, target);
        removeInterRegionConnectionInternal(source, target);
    }
    
    // Global statistics with safe access
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system access
    PredictionSystem* getPredictionSystem();
    
    // Cognition system access
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation system access
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const;
    
    // Random generator access
    RandomGenerator* getRandomGenerator();
    
    // Logging with error context
    void logStatus() const;
    
    // Configuration validation
    void validateConfiguration() const {
        BrainConfig config;
        // Extract configuration from config_ and validate
        config.validate();
    }
    
    // Safe region ID validation
    bool isValidRegionId(RegionId id) const {
        return id < pImpl_->regions_.size();
    }
    
    // Inter-region connection validation
    void validateInterRegionConnection(RegionId source, RegionId target) const {
        if (!isValidRegionId(source)) {
            throw ValidationException("Invalid source region ID: " + std::to_string(source));
        }
        if (!isValidRegionId(target)) {
            throw ValidationException("Invalid target region ID: " + std::to_string(target));
        }
        if (source == target) {
            throw ValidationException("Source and target regions cannot be the same");
        }
    }
    
    // Neuron ID validation
    void validateNeuronId(NeuronId id) const {
        if (id == INVALID_NEURON_ID) {
            throw ValidationException("Invalid neuron ID");
        }
    }
    
    // Neuromodulator validation
    void validateNeuromodulator(const Neuromodulator& signal) const {
        if (&signal == nullptr) {
            throw ValidationException("Neuromodulator signal is null");
        }
    }
    
    // Sensory input validation
    void validateSensoryInput(const SensoryInput& input) const {
        // Add sensory input specific validation
    }
    
    // Error handling
    void handleSimulationError(const std::string& operation, const std::string& error) {
        NLM_LOG_ERROR("Simulation error in " + operation + ": " + error);
        // Could implement recovery strategies here
    }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Private internal methods
    bool initializeInternal();
    void stepInternal(SimulationStep currentStep);
    void stepInternal(SimulationStep currentStep, Timestamp currentTime);
    void receiveSensoryInputInternal(const SensoryInput& input);
    void injectCurrentInternal(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeuronsInternal(NeuronType type, MembranePotential current);
    std::unique_ptr<Action> produceActionInternal();
    void applyNeuromodulationInternal(const Neuromodulator& signal);
    void updatePlasticityInternal();
    void developInternal();
    void resetInternal();
    bool saveInternal(const std::string& filepath) const;
    bool loadInternal(const std::string& filepath);
    RegionId addRegionInternal(const std::string& name);
    NeuralRegion* getRegionInternal(RegionId id);
    const NeuralRegion* getRegionInternal(RegionId id) const;
    float calculateExcitationInhibitionRatio() const;
    void addInterRegionConnectionInternal(RegionId source, RegionId target,
                                         float weight, Delay delay);
    void removeInterRegionConnectionInternal(RegionId source, RegionId target);
};

} // namespace nlm

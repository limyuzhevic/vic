// Brain class with improved modularity, dependency injection, and error handling
#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

// Forward declarations for optional dependencies
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
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
class Neuromodulator;

// Abstract interface for optional systems
class IOptionalSystem {
public:
    virtual ~IOptionalSystem() = default;
    virtual std::string getName() const = 0;
    virtual bool isEnabled() const = 0;
};

// Configuration parameter groups for better organization
struct BrainParameterGroups {
    // Core parameters
    struct Core {
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
    };
    
    // Plasticity parameters  
    struct Plasticity {
        float stdpLtpWeight = 0.01f;
        float stdpLtdWeight = 0.012f;
        float stdpTau = 20.0f;
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
    };
    
    // Integration parameters
    struct Integration {
        size_t replayInterval = 100;
        size_t consolidationInterval = 1000;
        double simulationTimestep = 0.001;
    };
    
    // Memory parameters
    struct Memory {
        size_t workingMemoryCapacity = 1000;
        size_t episodicMemoryCapacity = 1000;
        size_t associativeMemoryCapacity = 1000;
    };
    
    // Development parameters
    struct Development {
        double criticalPeriodEnd = 300.0;
        double maturationEnd = 900.0;
        float initialPlasticity = 1.0f;
        float adultPlasticity = 0.2f;
    };
    
    // Neuromodulation parameters
    struct Neuromodulation {
        float dopamineRangeMin = -1.0f;
        float dopamineRangeMax = 1.0f;
        float noveltyDecayRate = 0.99f;
    };
};

// Brain: The central coordinator with improved modularity and error handling
class Brain {
public:
    // Create brain with optional dependency injection
    explicit Brain(std::shared_ptr<Config> config = nullptr);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Configure with parameter groups
    bool configure(const BrainParameterGroups& params);
    
    // Initialize brain with validation
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input with validation
    void receiveSensoryInput(const SensoryInput& input);
    
    // Direct neuron manipulation with error checking
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // System access with null safety
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output
    std::unique_ptr<Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Update plasticity rules
    void updatePlasticity();
    
    // Apply developmental changes
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save/load brain state
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Memory systems (optional components)
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system (optional component)
    PredictionSystem* getPredictionSystem();
    
    // Cognition systems (optional components)
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system (optional component)
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems (optional components)
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Configuration management
    void setConfig(std::shared_ptr<Config> config);
    bool hasConfig() const { return config_ != nullptr; }
    
    // Validation and error handling
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Statistics and monitoring
    void logStatus() const;
    
    // Integration state
    bool isResting() const { return isResting_; }
    void setResting(bool resting) { isResting_ = resting; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Integration state
    bool isResting_;
    
    // Helper methods
    void validateStepParameters(SimulationStep step, Timestamp time) const;
    void validateRegionAccess(RegionId id) const;
    size_t validateAndGetNeuronCount() const;
};

// Implementation helper class for PIMPL
struct Brain::Impl {
    // Core systems (required)
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Optional systems (dependency injection)
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
    std::unique_ptr<Novelty> novelty;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Neuron tracking
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Validation state
    mutable std::vector<std::string> validationErrors;
    
    // Configuration parameter groups (for organization)
    BrainParameterGroups paramGroups;
    
    // Constructors
    explicit Impl(std::shared_ptr<Config> cfg);
    ~Impl();
    
    // Copy/move disabled
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
    Impl(Impl&&) = delete;
    Impl& operator=(Impl&&) = delete;
    
    // Validation methods
    void validateConfig() const;
    void validateNeurons() const;
    bool isValid() const;
    
    // Error reporting
    void addValidationError(const std::string& error);
    void clearValidationErrors() { validationErrors.clear(); }
    
    // Neuron management
    void collectNeurons();
    void clearNeurons() { sensoryNeurons.clear(); motorNeurons.clear(); }
    
    // Statistics update
    void updateStatistics();
    
    // System initialization
    void initializeCoreSystems();
    void initializeOptionalSystems();
    
    // Integration methods
    void replayMemory();
    void consolidateMemory();
    void applyDevelopment();
    void applyNeuromodulation();
    
    // Helper methods
    size_t countNeuronsByType(NeuronType type) const;
    float calculateFiringRate() const;
};

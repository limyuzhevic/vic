#pragma once

// NLM Brain Class - Modern C++ Implementation
// Replaces PIMPL pattern with clean, modern C++ design
// Supports both simple and advanced usage patterns

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <optional>
#include <variant>
#include <stdexcept>
#include <functional>

namespace nlm {

// Forward declarations
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
class SensoryInput;
class Action;
class Neuromodulator;
class Environment;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Custom exception types for better error handling
class BrainInitializationError : public std::runtime_error {
public:
    explicit BrainInitializationError(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainSimulationError : public std::runtime_error {
public:
    explicit BrainSimulationError(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainConfigurationError : public std::runtime_error {
public:
    explicit BrainConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Brain: The central coordinator of the neural system
// Modern implementation with better encapsulation and error handling
class Brain {
public:
    // Configuration options
    struct Configuration {
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        float stdpLtpWeight = 0.02f;
        float stdpLtdWeight = 0.015f;
        float stdpTau = 20.0f;
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        double simulationTimestep = 0.001;
        uint64_t randomSeed = 42;
        std::string checkpointDir = "./checkpoints";
        size_t replayInterval = 100;
        size_t consolidationInterval = 1000;
        
        // Development configuration
        float criticalPeriodEnd = 0.3f;  // Fraction of adult stage
        float agingStart = 0.8f;       // Fraction of adult stage
        
        // Neuromodulation configuration
        float dopamineMaxLevel = 1.0f;
        float curiosityMaxLevel = 1.0f;
        float noveltyMaxLevel = 1.0f;
        
        // Working memory configuration
        size_t workingMemoryCapacity = 100;
        
        // Episodic memory configuration
        size_t maxEpisodicMemory = 1000;
        
        // Prediction system configuration
        float predictionConfidenceThreshold = 0.7f;
        
        // Attention system configuration
        float attentionInhibitionStrength = 0.5f;
        float attentionExcitationStrength = 1.5f;
        
        // Planning system configuration
        size_t planningDepth = 5;
        
        // Additional fields can be added here
    };

    // === Core Constructor and Destructor ===
    
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config = nullptr);
    
    // Move constructor
    Brain(Brain&& other) noexcept;
    
    // Move assignment operator
    Brain& operator=(Brain&& other) noexcept;
    
    // Destructor
    ~Brain() = default;
    
    // Disable copying (neural networks are complex and should be managed via moves)
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;

    // === Initialization ===
    
    // Initialize brain with configuration
    bool initialize() noexcept(false);
    
    // Initialize with custom configuration (overwrites current config)
    bool initialize(const Configuration& config) noexcept(false);
    
    // Re-initialize brain (reset and re-initialize)
    bool reinitialize() noexcept(false) {
        reset();
        return initialize();
    }

    // === Simulation Control ===
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step (explicit timestamp)
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Run simulation for a specific duration
    void simulate(double duration);  // duration in seconds
    
    // Run simulation for a specific number of steps
    void simulateSteps(size_t steps);

    // === Input/Output ===
    
    // Receive sensory input from environment
    void receiveSensoryInput(const SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Get action from brain based on current neural activity
    std::unique_ptr<Action> produceAction() const;
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const Neuromodulator& signal);

    // === System Control ===
    
    // Reset brain state
    void reset() noexcept;
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);

    // === Query Methods ===
    
    // Get configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get current internal configuration
    const Configuration& getConfiguration() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Get spike system
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Get plasticity systems
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Get statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Get simulation time
    SimulationStep getCurrentStep() const;
    Timestamp getCurrentTime() const;

    // === Region Management ===
    
    // Add a new region
    RegionId addRegion(const std::string& name = "");
    
    // Get region by ID
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Get region count
    size_t getRegionCount() const;
    
    // Get region IDs
    std::vector<RegionId> getRegionIds() const;
    
    // Remove region (complex operation that requires careful cleanup)
    bool removeRegion(RegionId id);

    // === Connection Management ===
    
    // Add inter-region connection
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    // Remove inter-region connection
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Get all inter-region connections
    const std::vector<InterRegionConnection>& getInterRegionConnections() const;

    // === Memory Systems ===
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    const NeuralAssociativeMemory* getAssociativeMemory() const;

    // === Prediction System ===
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    const PredictionSystem* getPredictionSystem() const;

    // === Cognition Systems ===
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    const NeuralPlanner* getPlanner() const;
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    const ConceptFormation* getConceptFormation() const;
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    const AttentionalSelection* getAttention() const;

    // === Development System ===
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    const DevelopmentSystem* getDevelopmentSystem() const;

    // === Neuromodulation Systems ===
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    const Dopamine* getDopamine() const;
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    const Curiosity* getCuriosity() const;
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    const Novelty* getNovelty() const;
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    const PredictionError* getPredictionErrorSignal() const;

    // === Convenience Methods ===
    
    // Get status summary (for logging/debugging)
    std::string getStatusSummary() const;
    
    // Check if brain is initialized
    bool isInitialized() const;
    
    // Get simulation statistics
    struct Statistics {
        SimulationStep currentStep;
        Timestamp currentTime;
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t totalSpikes;
        size_t pendingSpikes;
        DevelopmentalStage developmentalStage;
    };
    
    Statistics getStatistics() const;
    
    // === Advanced Features ===
    
    // Enable/disable specific subsystems
    void enableSubsystem(const std::string& name, bool enable);
    bool isSubsystemEnabled(const std::string& name) const;
    
    // Set learning rate for plasticity
    void setLearningRate(float rate);
    
    // Add sensory neuron (for receiving external input)
    NeuronId addSensoryNeuron(const std::string& name);
    
    // Add motor neuron (for producing output actions)
    NeuronId addMotorNeuron(const std::string& name);
    
    // Connect two regions
    void connectRegions(RegionId source, RegionId target, 
                       float weight = 0.0f, Delay delay = 1,
                       SynapseType type = SynapseType::Excitatory);
    
    // === Factory Methods ===
    
    // Static factory methods for common configurations
    static std::shared_ptr<Brain> createDefault(size_t neuronCount = 1000);
    static std::shared_ptr<Brain> createLearning(size_t neuronCount = 1000);
    static std::shared_ptr<Brain> createExploration(size_t neuronCount = 1000);
    static std::shared_ptr<Brain> createDevelopment(size_t neuronCount = 1000);
    
    // === Internal Implementation ===
    
private:
    // Internal implementation details
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
    
    // Constructor for internal use
    explicit Brain(std::unique_ptr<Implementation> impl) : pImpl(std::move(impl)) {}
    
    // Friend declarations for testing and internal access
    friend class BrainFactory;
    friend class TestBrain;
};

// Brain factory for advanced brain creation
class BrainFactory {
public:
    static std::shared_ptr<Brain> createStandard(size_t neuronCount = 1000);
    static std::shared_ptr<Brain> createMinimal(size_t neuronCount = 100);
    static std::shared_ptr<Brain> createAdvanced(size_t neuronCount = 5000);
    static std::shared_ptr<Brain> createCustom(const Brain::Configuration& config);
};

// Exception for brain operations
class BrainOperationException : public std::runtime_error {
public:
    enum class Type { Initialization, Simulation, Configuration, Memory, Plasticity };
    
    BrainOperationException(Type type, const std::string& message)
        : std::runtime_error(message), errorType(type) {}
    
    Type getErrorType() const { return errorType; }
    
private:
    Type errorType;
};

} // namespace nlm
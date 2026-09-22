#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class NeuralSemanticMemory;
class NeuralProceduralMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class SelfModel;
class SocialLearning;
class InternalSignals;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class Serotonin;
class Norepinephrine;
class Acetylcholine;

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

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

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
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
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
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Self model for maintaining internal state
    SelfModel* getSelfModel();
    
    // Social learning system
    SocialLearning* getSocialLearning();
    
    // Internal signals processing
    InternalSignals* getInternalSignals();
    
    // Additional memory systems
    NeuralSemanticMemory* getSemanticMemory();
    NeuralProceduralMemory* getProceduralMemory();
    
    // Complete neuromodulation system access
    Serotonin* getSerotonin();
    Norepinephrine* getNorepinephrine();
    Acetylcholine* getAcetylcholine();
    
    // ========== ADDITIONAL ACCESSORS ==========
    
    // Additional memory systems
    NeuralSemanticMemory* getSemanticMemory();
    NeuralProceduralMemory* getProceduralMemory();
    
    // Complete neuromodulation system access
    Serotonin* getSerotonin();
    Norepinephrine* getNorepinephrine();
    Acetylcholine* getAcetylcholine();
    
    // New brain systems for Phase 4+
    SelfModel* getSelfModel();
    SocialLearning* getSocialLearning();
    InternalSignals* getInternalSignals();
    
    // Error handling
    const std::string& getLastError() const;
    bool hasError() const;
    void clearError();
    
    // Performance monitoring
    double getAverageFiringRate() const;
    size_t getTotalSpikeCount() const;
    float getExcitationInhibitionRatio() const;
    
    // Batch processing
    bool runBatch(int numRuns, int stepsPerRun, float variance = 0.0f);
    void setBatchMode(bool enabled);
    bool isBatchMode() const;
    
    // Advanced examples and utilities
    static std::string getExampleCode(const std::string& exampleName);
    static std::vector<std::string> getAvailableExamples();
    
    // Development and debugging
    void enableDebugMode(bool enabled);
    bool isDebugMode() const;
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    
    // System health and metrics
    void collectMetrics();
    std::string getMetricsSummary() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

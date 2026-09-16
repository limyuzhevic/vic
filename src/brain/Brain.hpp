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
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

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
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // ===== SIMULATION CONTROL =====
    
    // Pause/resume simulation
    void pause();
    void resume();
    bool isPaused() const;
    
    // Step control
    void step();
    void step(Timestamp timestep);
    SimulationStep getCurrentStep() const;
    Timestamp getCurrentTime() const;
    
    // Get simulation clock
    SimulationClock* getSimulationClock();
    const SimulationClock* getSimulationClock() const;
    
    // ===== METRICS COLLECTION =====
    
    // Neural statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Performance metrics
    double getAverageSpikeInterval() const;
    double getMaxSpikeFrequency() const;
    double getAverageResponseTime() const;
    
    // Energy and resource tracking
    float getTotalEnergy() const;
    float getAverageEnergyConsumption() const;
    
    // ===== NEURAL PATTERN MANAGEMENT =====
    
    // Pattern utilities
    std::shared_ptr<NeuralPattern> createPattern(const std::string& name);
    std::shared_ptr<NeuralPattern> loadPatternFromJSON(const std::string& json);
    std::string patternToJSON(const std::shared_ptr<NeuralPattern>& pattern) const;
    void registerPattern(const std::shared_ptr<NeuralPattern>& pattern);
    std::vector<std::string> getRegisteredPatternNames() const;
    
    // ===== PERFORMANCE MONITORING =====
    
    // CPU/GPU monitoring
    double getCPUUsage() const;
    double getMemoryUsage() const; // in MB
    double getGPUMemoryUsage() const; // in MB
    
    // Simulation performance
    double getSimulationFPS() const;
    double getAverageStepTime() const;
    double getMaxStepTime() const;
    
    // ===== SERIALIZATION/DESERIALIZATION =====
    
    // Enhanced save/load with compression options
    bool save(const std::string& filepath, bool compress = false, int compressionLevel = 6) const;
    bool load(const std::string& filepath, bool compressed = false);
    std::string saveToString(bool compress = false, int compressionLevel = 6) const;
    bool loadFromString(const std::string& data, bool compressed = false);
    
    // Checkpoint management
    bool createCheckpoint(const std::string& label = "") const;
    bool restoreCheckpoint(const std::string& label = "");
    std::vector<std::string> getAvailableCheckpoints() const;
    
    // ===== RANDOM SEED MANAGEMENT =====
    
    // Random number generation control
    uint64_t getRandomSeed() const;
    void setRandomSeed(uint64_t seed);
    void seedRandom(uint64_t seed);
    
    // Random distribution helpers
    double getRandomNormal(double mean = 0.0, double stddev = 1.0);
    double getRandomUniform(double min = 0.0, double max = 1.0);
    int getRandomInt(int min, int max);
    
    // ===== DEBUGGING AND INTROSPECTION =====
    
    // Memory and resource inspection
    size_t getMemoryFootprint() const; // in bytes
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Neural connectivity inspection
    size_t getIncomingConnections(NeuronId neuron) const;
    size_t getOutgoingConnections(NeuronId neuron) const;
    
    // System diagnostics
    std::string getDiagnosticsReport() const;
    std::map<std::string, std::string> getSystemInfo() const;
    
    // ===== LOGGING =====
    
    // Logging control
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    
    // Performance logging
    void logPerformanceStats() const;
    void logNeuralStats() const;
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

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
    
        // ========== ADVANCED BATCH OPERATIONS ==========
    
    /**
     * Batch inject current into multiple neurons at once
     * More efficient than individual calls for simulation speed
     */
    void batchInjectCurrent(const std::vector<std::pair<NeuronId, MembranePotential>>& injections);
    
    /**
     * Batch collect neuron statistics for performance monitoring
     * Returns statistics for a range of neurons in a single operation
     */
    struct NeuronStatistics {
        size_t neuronCount;
        float avgMembranePotential;
        float minMembranePotential;
        float maxMembranePotential;
        float avgFiringRate;
        float avgThreshold;
        float stdMembranePotential;
    };
    NeuronStatistics batchGetNeuronStatistics(size_t maxNeurons = 1000) const;
    
    /**
     * Batch enable plasticity across all synapses with specified rules
     */
    struct PlasticityRule {
        bool hebbian;
        bool stdp;
        bool rewardModulated;
        float learningRate;
    };
    size_t batchEnablePlasticity(const PlasticityRule& rule);
    
    /**
     * Performance benchmarking methods
     */
    struct PerformanceMetrics {
        double simulationTime;          // Wall clock time (seconds)
        double processingSpeed;         // Steps per second
        size_t totalMemoryUsage;        // Bytes
        float averageFiringRate;        // Hz
        float energyConsumption;        // Arbitrary units
        float plasticityUpdateTime;    // Seconds per step
    };
    PerformanceMetrics getPerformanceMetrics() const;
    
    // ========== ENHANCED ERROR HANDLING ==========
    
    /**
     * Extended error reporting with context
     */
    struct ErrorReport {
        std::string errorCode;
        std::string message;
        std::string context;
        std::string sourceFile;
        int sourceLine;
        std::chrono::system_clock::time_point timestamp;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    /**
     * Get last error (thread-local)
     */
    std::optional<ErrorReport> getLastError() const;
    
    /**
     * Clear error state
     */
    void clearError();
    
    /**
     * Validate brain state for consistency
     */
    bool validateState() const;
    
    /**
     * Get state validation errors
     */
    std::vector<std::string> getStateValidationErrors() const;
    
    // ========== SIMULATION CONTROL ENHANCEMENTS ==========
    
    /**
     * Run simulation with automatic checkpointing and recovery
     */
    bool runSimulationWithRecovery(uint64_t startStep, uint64_t endStep,
                                   const std::string& checkpointDir = "./checkpoints",
                                   uint64_t checkpointInterval = 1000);
    
    /**
     * Get current simulation progress
     */
    struct SimulationProgress {
        uint64_t currentStep;
        uint64_t totalSteps;
        double simulationTime;
        double wallClockTime;
        float progressPercent;
        bool isComplete;
    };
    SimulationProgress getSimulationProgress() const;
    
    /**
     * Pause/resume simulation control
     */
    void pauseSimulation();
    void resumeSimulation();
    bool isSimulationPaused() const;
    
    // ========== MEMORY MANAGEMENT ==========
    
    /**
     * Get detailed memory statistics
     */
    struct MemoryStatistics {
        size_t totalMemoryUsage;
        size_t allocatedObjects;
        size_t freeObjects;
        double fragmentationRatio;
        std::unordered_map<std::string, size_t> poolStats;
    };
    MemoryStatistics getMemoryStatistics() const;
    
    /**
     * Optimize memory usage (defragment, cleanup)
     */
    void optimizeMemory();
    
    /**
     * Force garbage collection
     */
    void forceGarbageCollection();
    
    // ========== DEBUG AND MONITORING ==========
    
    /**
     * Enable detailed logging for specific components
     */
    void enableDebugLogging(const std::vector<std::string>& components);
    
    /**
     * Get performance counters for monitoring
     */
    struct PerformanceCounters {
        uint64_t stepsProcessed;
        uint64_t spikesGenerated;
        uint64_t synapsesModified;
        uint64_t neuronsCreated;
        uint64_t neuronsDestroyed;
        double totalSimulationTime;
        std::chrono::system_clock::time_point startTime;
    };
    PerformanceCounters getPerformanceCounters() const;
    
    /**
     * Reset performance counters
     */
    void resetPerformanceCounters();
    
    // ========== EXPORT/IMPORT ENHANCEMENTS ==========
    
    /**
     * Export brain state with compression options
     */
    enum class ExportFormat {
        Binary,
        Text,
        Compressed,
        MemoryMap,
        Streamed
    };
    
    /**
     * Import brain state with validation and compatibility checking
     */
    bool importWithValidation(const std::string& filepath, ExportFormat format = ExportFormat::Binary);
    
    /**
     * Get brain state compatibility report
     */
    std::string getCompatibilityReport(const std::string& filepath) const;
    
    // ========== CONFIGURATION MANAGEMENT ==========
    
    /**
     * Get all configuration values with formatted summary
     */
    std::string getConfigurationReport() const;
    
    /**
     * Apply configuration preset
     */
    enum class ConfigurationPreset {
        Minimal,           // Minimal simulation, fast startup
        Balanced,          // Good balance of features and performance
        HighPerformance,   // Maximum performance, resource intensive
        Research,          // Optimized for research scenarios
        Production         // Production-ready settings
    };
    
    void applyConfigurationPreset(ConfigurationPreset preset);
    
    /**
     * Get current configuration score (0-100)
     */
    int getConfigurationScore() const;
    
    // ========== THREAD SAFETY ==========
    
    /**
     * Get current thread safety status
     */
    bool isThreadSafe() const { return threadSafe_; }
    
    /**
     * Enable thread-safe mode (with potential performance impact)
     */
    void enableThreadSafeMode() { threadSafe_ = true; }
    
    /**
     * Disable thread-safe mode (for performance)
     */
    void disableThreadSafeMode() { threadSafe_ = false; }
    
    // ========== ADVANCED SIMULATION CONTROLS ==========
    
    /**
     * Set simulation speed multiplier
     */
    void setSimulationSpeed(float multiplier);
    
    /**
     * Get current simulation speed
     */
    float getSimulationSpeed() const { return simulationSpeed_; }
    
    /**
     * Schedule events for future execution
     */
    struct ScheduledEvent {
        uint64_t step;
        std::function<void()> callback;
        std::string name;
    };
    
    void scheduleEvent(uint64_t step, std::function<void()> callback, const std::string& name = "");
    
    /**
     * Cancel scheduled event
     */
    void cancelEvent(const std::string& name);
    
    /**
     * Get scheduled events
     */
    std::vector<ScheduledEvent> getScheduledEvents() const;
    
    // ========== INTEGRATION VERIFICATION ==========
    
    /**
     * Run comprehensive integration tests
     */
    bool runIntegrationTests() const;
    
    /**
     * Get integration test results
     */
    struct IntegrationTestResults {
        bool memorySystemsIntegrated;
        bool neuromodulationIntegrated;
        bool predictionIntegrated;
        bool cognitionIntegrated;
        bool developmentIntegrated;
        bool plasticityIntegrated;
        float overallIntegrationScore;
        std::vector<std::string> errors;
    };
    IntegrationTestResults getIntegrationTestResults() const;
    
    // ========== ADVANCED FEATURE FLAGS ==========
    
    /**
     * Get all available feature flags
     */
    std::unordered_map<std::string, bool> getFeatureFlags() const;
    
    /**
     * Enable/disable feature flags
     */
    void setFeatureFlag(const std::string& name, bool enabled);
    
    /**
     * Check if feature is available
     */
    bool isFeatureAvailable(const std::string& name) const;
    
    // ========== SYSTEM HEALTH ==========
    
    /**
     * Get system health status
     */
    struct SystemHealth {
        bool isHealthy;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        double healthScore; // 0-100
        std::chrono::system_clock::time_point lastCheck;
    };
    
    SystemHealth getSystemHealth() const;
    
    /**
     * Perform system health check
     */
    SystemHealth performHealthCheck() const;

private:
    // Internal implementation
    struct Impl;
    Impl* pImpl;
    
    // Thread safety flag
    bool threadSafe_ = false;
    
    // Simulation control
    bool simulationPaused_ = false;
    float simulationSpeed_ = 1.0f;
    
    // Performance monitoring
    mutable PerformanceCounters perfCounters_;
    std::chrono::system_clock::time_point simulationStartTime_;
    
    // Scheduled events
    std::vector<ScheduledEvent> scheduledEvents_;
    
    // Error handling
    mutable std::optional<ErrorReport> lastError_;
    
    // Memory management
    bool memoryOptimizationScheduled_ = false;
};

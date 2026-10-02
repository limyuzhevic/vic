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
    
    // ========== ADVANCED STATISTICS ==========
    
    // Working memory statistics
    size_t getWorkingMemoryCapacity() const;
    size_t getWorkingMemoryActiveSlots() const;
    size_t getWorkingMemoryAccessCount() const;
    
    // Episodic memory statistics
    size_t getEpisodicMemoryEpisodeCount() const;
    size_t getEpisodicMemoryCapacity() const;
    double getEpisodicMemoryReplaySuccessRate() const;
    
    // Associative memory statistics
    size_t getAssociativeMemoryPatternCount() const;
    size_t getAssociativeMemoryActiveAssociations() const;
    
    // Prediction system statistics
    size_t getPredictionSystemUpdateCount() const;
    double getPredictionSystemAccuracy() const;
    double getPredictionSystemConfidence() const;
    
    // Cognition system statistics
    size_t getPlannerActionCount() const;
    size_t getConceptFormationConceptCount() const;
    size_t getAttentionFocusCount() const;
    
    // Development system statistics
    double getDevelopmentPlasticityRate() const;
    size_t getDevelopmentStructuralChanges() const;
    size_t getDevelopmentMaturationEvents() const;
    
    // Neuromodulation statistics
    double getDopamineLevel() const;
    double getCuriosityLevel() const;
    double getNoveltyLevel() const;
    double getPredictionErrorLevel() const;
    
    // ========== PERFORMANCE MONITORING ==========
    
    // Performance statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Simulation performance
    float getStepsPerSecond() const;
    double getSimulationTime() const;
    size_t getStepsSinceLastCheckpoint() const;
    size_t getStepsSinceLastReplay() const;
    
    // Memory usage statistics
    size_t getMemoryPoolUsage() const;
    size_t getEventQueueUsage() const;
    double getMemoryUtilizationPercentage() const;
    
    // ========== NETWORK ANALYSIS ==========
    
    // Network topology statistics
    size_t getRegionCount() const;
    size_t getConnectionCount() const;
    size_t getIntraRegionConnections() const;
    size_t getInterRegionConnections() const;
    float getAverageConnectionWeight() const;
    float getMaxConnectionWeight() const;
    float getMinConnectionWeight() const;
    
    // Activity statistics
    size_t getTotalNeuronCount() const override;
    size_t getTotalSynapseCount() const override;
    size_t getActiveNeuronCount() const override;
    size_t getFiringNeuronCount() const override;
    float getAverageFiringRate() const override;
    
    // Advanced connectivity
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    std::vector<RegionId> getMostActiveRegions(size_t count) const;
    float getNetworkEfficiency() const;
    float getClusteringCoefficient() const;
    
    // ========== STATE ANALYSIS ==========
    
    // Brain state information
    std::string getWorkingMemoryState() const;
    std::string getEpisodicMemorySummary() const;
    std::string getPredictionSystemStatus() const;
    std::string getDevelopmentStageDescription() const;
    std::string getNeuromodulationStatus() const;
    
    // State validation
    bool validateBrainState() const;
    std::string getStateValidationErrors() const;
    
    // ========== ADVANCED ANALYSIS ==========
    
    // Learning progress tracking
    double getLearningProgress() const;
    double getPlasticityEfficiency() const;
    double getMemoryConsolidationProgress() const;
    
    // Behavioral statistics
    std::unique_ptr<class Action> getActionSequence(size_t steps) const;
    size_t getBehaviorPatternCount() const;
    
    // ========== REAL-TIME MONITORING ==========
    
    // Real-time monitoring
    void startMonitoring(float intervalSeconds = 0.1f);
    void stopMonitoring();
    bool isMonitoring() const;
    std::string getMonitoringData() const;
    
    // Performance profiling
    void enableProfiling(bool enable = true);
    bool isProfilingEnabled() const;
    std::string getProfileStats() const;
    
    // ========== ADVANCED CONTROL ==========
    
    // Advanced simulation control
    void runEpisode(size_t maxSteps, TimestepDuration timestep = TimestepDuration(0.001));
    void stepBatched(size_t steps, TimestepDuration timestep = TimestepDuration(0.001));
    void pause();
    void resume();
    bool isPaused() const;
    float getSimulationSpeed() const;
    void setSimulationSpeed(float speed);
    
    // ========== EXPORT/ANALYSIS ==========
    
    // Export data for analysis
    void exportDebugInfo(const std::string& filepath) const;
    void exportForAnalysis(const std::string& filepath, const std::string& format = "json") const;
    
    // ========== HELPERS ==========
    
    // Brain health check
    bool checkForMemoryLeaks() const;
    bool checkForPerformanceBottlenecks() const;
    bool checkSynchronization() const;
    std::string generateHealthReport() const;
    
    // Get most active neurons
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

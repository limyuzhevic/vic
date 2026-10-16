// NLM Runtime Introspection API
// Provides debugging and inspection capabilities for NLM brain simulation

#ifndef NLM_RUNTIME_INTRrospection_HPP
#define NLM_RUNTIME_INTRrospection_HPP

#include "brain/Brain.hpp"
#include "core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <atomic>

namespace nlm {

// Brain State Snapshot
struct BrainStateSnapshot {
    SimulationStep step;
    std::chrono::duration<double> wallClockTime;
    
    // Global brain metrics
    size_t totalNeuronCount;
    size_t activeNeuronCount;
    size_t firingNeuronCount;
    size_t totalSynapseCount;
    float averageFiringRate;
    float excitationInhibitionRatio;
    uint64_t totalSpikeCount;
    
    // Development state
    std::string developmentalStage;
    float neuromodulationLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    
    // Memory systems
    size_t workingMemoryCapacity;
    size_t episodicMemoryCount;
    size_t semanticMemoryConcepts;
    size_t proceduralMemoryRules;
    
    // Plasticity metrics
    float totalSynapticWeight;
    float meanSynapticWeight;
    size_t plasticityEnabledSynapses;
    size_t strengthenedConnections;
    size_t weakenedConnections;
    
    // Network statistics
    float maxFiringRate;
    float minFiringRate;
    float stdFiringRate;
    
    // Cognitive metrics
    size_t conceptsFormed;
    size_t plansExecuted;
    size_t decisionsMade;
    
    // Time series data
    std::vector<float> firingRateHistory;
    std::vector<size_t> activeNeuronHistory;
    std::vector<float> synapticWeightHistory;
    
    BrainStateSnapshot() : step(0), totalNeuronCount(0), activeNeuronCount(0),
                          firingNeuronCount(0), totalSynapseCount(0), averageFiringRate(0.0f),
                          excitationInhibitionRatio(1.0f), totalSpikeCount(0), neuromodulationLevel(0.0f),
                          curiosityLevel(0.0f), noveltyLevel(0.0f), predictionError(0.0f),
                          workingMemoryCapacity(0), episodicMemoryCount(0), semanticMemoryConcepts(0),
                          proceduralMemoryRules(0), totalSynapticWeight(0.0f), meanSynapticWeight(0.0f),
                          plasticityEnabledSynapses(0), strengthenedConnections(0), weakenedConnections(0),
                          maxFiringRate(0.0f), minFiringRate(0.0f), stdFiringRate(0.0f),
                          conceptsFormed(0), plansExecuted(0), decisionsMade(0) {}
};

// Runtime Inspector Class
class RuntimeInspector {
public:
    RuntimeInspector() = default;
    ~RuntimeInspector() = default;
    
    // Inspection methods
    BrainStateSnapshot captureSnapshot() const;
    std::string getBrainStatus() const;
    std::string getPerformanceMetrics() const;
    std::string getMemoryUsage() const;
    std::string getNetworkTopology() const;
    std::string getPlasticitySummary() const;
    std::string getCognitionSummary() const;
    std::string getNeuromodulationSummary() const;
    std::string getPredictionSummary() const;
    
    // Debug control
    void enableDebugMode(bool enable) { debugMode = enable; }
    bool isDebugMode() const { return debugMode; }
    
    void setMaxHistorySize(size_t size) { maxHistorySize = size; }
    size_t getMaxHistorySize() const { return maxHistorySize; }
    
    void enableDetailedLogging(bool enable) { detailedLogging = enable; }
    bool isDetailedLoggingEnabled() const { return detailedLogging; }
    
    // History management
    void clearHistory() {
        firingRateHistory.clear();
        activeNeuronHistory.clear();
        synapticWeightHistory.clear();
        historyTimestamps.clear();
    }
    
    // Checkpoints and debugging
    bool takeCheckpoint(const std::string& filename) const;
    bool restoreCheckpoint(const std::string& filename);
    bool compareSnapshots(const BrainStateSnapshot& snapshot1, const BrainStateSnapshot& snapshot2) const;
    
    // Configuration
    void setInspectionInterval(SimulationStep interval) { inspectionInterval = interval; }
    SimulationStep getInspectionInterval() const { return inspectionInterval; }
    
    // Runtime control
    void pauseSimulation() { simulationPaused = true; }
    void resumeSimulation() { simulationPaused = false; }
    bool isSimulationPaused() const { return simulationPaused; }
    
    // Performance monitoring
    void startPerformanceTimer() { performanceTimerStart = std::chrono::high_resolution_clock::now(); }
    double getPerformanceTime() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - performanceTimerStart).count();
    }
    
    // Error handling
    void setErrorCallback(std::function<void(const std::string&)> callback) { errorCallback = callback; }
    void reportError(const std::string& error) {
        if (errorCallback) errorCallback(error);
        if (detailedLogging) {
            std::cerr << "[RuntimeInspector ERROR] " << error << std::endl;
        }
    }

private:
    // History and state tracking
    mutable std::vector<BrainStateSnapshot> historySnapshots;
    mutable std::vector<float> firingRateHistory;
    mutable std::vector<size_t> activeNeuronHistory;
    mutable std::vector<float> synapticWeightHistory;
    mutable std::vector<std::chrono::duration<double>> historyTimestamps;
    
    // Configuration
    bool debugMode = false;
    bool detailedLogging = false;
    bool simulationPaused = false;
    size_t maxHistorySize = 1000;
    SimulationStep inspectionInterval = 100;
    
    // Performance monitoring
    mutable std::chrono::high_resolution_clock::time_point performanceTimerStart;
    
    // Error handling
    std::function<void(const std::string&)> errorCallback;
    
    // Helper methods
    void updateHistory(const BrainStateSnapshot& snapshot) const;
    float calculateStandardDeviation(const std::vector<float>& values) const;
    void sanitizeSnapshot(BrainStateSnapshot& snapshot) const;
    std::string formatTimestamp(std::chrono::duration<double> time) const;
};

// Brain Inspector Wrapper
class BrainInspector {
public:
    BrainInspector(std::shared_ptr<Brain> brain) : brain(brain) {}
    ~BrainInspector() = default;
    
    // Direct brain access with safety checks
    std::shared_ptr<Brain> getBrain() const { return brain; }
    
    // Inspection methods
    BrainStateSnapshot captureState() const;
    std::string getRegionInfo(RegionId regionId) const;
    std::string getNeuronInfo(NeuronId neuronId) const;
    std::string getSynapseInfo(SynapseId synapseId) const;
    
    // Network analysis
    std::string analyzeConnectivity() const;
    std::string analyzePlasticity() const;
    std::string analyzeActivity() const;
    
    // Performance analysis
    double getAverageFiringRate() const;
    size_t getPeakActiveNeurons() const;
    double getSimulationEfficiency() const;
    
    // Debugging tools
    std::string debugNeuronState(NeuronId neuronId) const;
    std::string debugSynapseState(SynapseId synapseId) const;
    std::string debugRegionState(RegionId regionId) const;
    
    // State management
    void logStateChange(const std::string& description) const;
    void checkpointState(const std::string& label) const;
    bool compareState(const BrainInspector& other) const;
    
    // Configuration access
    std::shared_ptr<Config> getConfig() const;
    
    // Utility methods
    static std::string formatTime(double seconds);
    static std::string formatBytes(size_t bytes);
    static std::string formatPercentage(double value);

private:
    std::shared_ptr<Brain> brain;
    mutable RuntimeInspector inspector;
};

// Global Runtime Inspector Instance
extern RuntimeInspector globalInspector;

// Convenience functions for quick inspections
namespace Runtime {
    // Quick snapshot capture
    BrainStateSnapshot captureBrainSnapshot();
    
    // Status reporting
    std::string getBrainStatus();
    std::string getPerformanceReport();
    std::string getDebugReport();
    
    // Configuration
    void enableDebugMode(bool enable);
    void setInspectionInterval(SimulationStep interval);
    
    // Control
    void pauseInspection();
    void resumeInspection();
    bool isInspectionPaused();
    
    // History management
    void clearInspectionHistory();
    size_t getHistorySize();
    
    // Error handling
    void setErrorHandler(std::function<void(const std::string&)> handler);
}

} // namespace nlm

#endif // NLM_RUNTIME_INTRrospection_HPP

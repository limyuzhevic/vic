// Brain state debugger for debugging and analysis
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <queue>
#include <chrono>
#include <functional>

namespace nlm {

struct BrainStateChange {
    SimulationStep step;
    Timestamp timestamp;
    std::string changeType;
    std::string component;
    float oldValue;
    float newValue;
    float changeMagnitude;
    std::string description;
    std::vector<float> affectedNeurons;
    std::vector<float> affectedSynapses;
    std::map<std::string, float> metadata;
};

struct ComparisonResult {
    SimulationStep step1;
    SimulationStep step2;
    float totalDifference;
    float neuronDifference;
    float synapticDifference;
    float memoryDifference;
    float activityDifference;
    std::vector<std::string> significantDifferences;
    std::map<std::string, float> componentDifferences;
    std::vector<BrainStateChange> changes;
    bool isSignificant;
    float similarityScore;
};

struct DebugReport {
    SimulationStep step;
    Timestamp timestamp;
    std::string reportType;
    std::string title;
    std::string description;
    std::vector<std::string> findings;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::vector<std::string> recommendations;
    std::map<std::string, float> metrics;
    std::vector<std::string> affectedComponents;
    bool hasCriticalIssues;
    bool requiresIntervention;
    std::chrono::system_clock::time_point timestampObj;
};

struct MemoryLeak {
    std::string leakType;
    std::string sourceComponent;
    SimulationStep firstDetected;
    SimulationStep lastObserved;
    size_t estimatedLeakSize;
    std::string memoryPool;
    std::vector<SimulationStep> leakPoints;
    bool isPersistent;
    std::string description;
    std::map<std::string, float> metadata;
};

struct PerformanceAnomaly {
    std::string anomalyType;
    std::string component;
    SimulationStep detectionStep;
    float baselinePerformance;
    float currentPerformance;
    float performanceDelta;
    float thresholdExceeded;
    std::string description;
    bool isCritical;
    std::vector<std::string> potentialCauses;
};

struct ConfigurationIssue {
    std::string issueType;
    std::string parameterName;
    float currentValue;
    float validRangeMin;
    float validRangeMax;
    std::string description;
    std::string severity;
    bool isBlocking;
    std::vector<std::string> suggestedFixes;
};

struct DevelopmentStageReport {
    SimulationStep step;
    Timestamp timestamp;
    DevelopmentalStage stage;
    float plasticityRate;
    float developmentProgress;
    float maturityLevel;
    std::vector<std::string> developmentalMilestones;
    bool hasCompletedMilestone;
    std::string nextMilestoneDescription;
};

struct NeuromodulationReport {
    SimulationStep step;
    Timestamp timestamp;
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionErrorLevel;
    float overallModulation;
    bool isModulationActive;
    std::vector<std::string> activeModulators;
    std::map<std::string, float> modulationEffects;
};

class BrainStateDebugger {
public:
    BrainStateDebugger();
    ~BrainStateDebugger();
    
    bool initialize(std::shared_ptr<Brain> brain);
    void shutdown();
    
    void update(const VisualizationData& data);
    
    // Brain state management
    BrainSnapshot captureSnapshot(const std::string& description = "") const;
    bool restoreSnapshot(const BrainSnapshot& snapshot);
    void clearSnapshots();
    
    // Comparison and analysis
    ComparisonResult compareSnapshots(const BrainSnapshot& snapshot1, 
                                     const BrainSnapshot& snapshot2) const;
    ComparisonResult compareWithPrevious(const BrainSnapshot& currentSnapshot) const;
    std::vector<ComparisonResult> compareSnapshots(const std::vector<BrainSnapshot>& snapshots) const;
    
    // Difference detection
    std::vector<BrainStateChange> detectDifferences(const BrainSnapshot& snapshot1,
                                                  const BrainSnapshot& snapshot2) const;
    std::vector<BrainStateChange> detectDifferences(const BrainSnapshot& previousSnapshot) const;
    
    // Pattern detection in brain state evolution
    std::vector<std::vector<BrainSnapshot>> detectStateTrajectories(size_t minTrajectoryLength) const;
    std::vector<std::string> analyzeStateTransitions(const BrainSnapshot& snapshot) const;
    
    // Anomaly detection
    bool detectAnomalies(const BrainSnapshot& snapshot) const;
    DebugReport generateAnomalyReport(const BrainSnapshot& snapshot) const;
    
    // Memory leak detection
    std::vector<MemoryLeak> detectMemoryLeaks() const;
    bool hasMemoryLeaks() const;
    DebugReport generateMemoryLeakReport(const std::vector<MemoryLeak>& leaks) const;
    
    // Performance analysis
    std::vector<PerformanceAnomaly> detectPerformanceAnomalies(const PerformanceMetrics& metrics) const;
    DebugReport generatePerformanceReport(const std::vector<PerformanceAnomaly>& anomalies) const;
    
    // Configuration validation
    std::vector<ConfigurationIssue> validateConfiguration() const;
    DebugReport generateConfigurationReport(const std::vector<ConfigurationIssue>& issues) const;
    
    // Development analysis
    DevelopmentStageReport analyzeDevelopmentStage(const BrainSnapshot& snapshot) const;
    bool isDevelopmentOnTrack() const;
    
    // Neuromodulation analysis
    NeuromodulationReport analyzeNeuromodulation(const BrainSnapshot& snapshot) const;
    bool isNeuromodulationBalanced() const;
    
    // Debugging interface
    std::vector<std::string> getDebugInfo(const BrainSnapshot& snapshot) const;
    std::vector<std::string> getRecommendations(const BrainSnapshot& snapshot) const;
    std::vector<std::string> getWarnings(const BrainSnapshot& snapshot) const;
    std::vector<std::string> getErrors(const BrainSnapshot& snapshot) const;
    
    // State comparison visualization
    void visualizeStateTransition(const BrainSnapshot& snapshot1, const BrainSnapshot& snapshot2) const;
    void visualizeMemoryLeaks(const std::vector<MemoryLeak>& leaks) const;
    void visualizePerformanceAnomalies(const std::vector<PerformanceAnomaly>& anomalies) const;
    void visualizeConfigurationIssues(const std::vector<ConfigurationIssue>& issues) const;
    
    // Export and logging
    void exportSnapshots(const std::string& directory, ExportFormat format = ExportFormat::JSON) const;
    void exportDebugReport(const DebugReport& report, const std::string& filepath) const;
    void exportComparisonResults(const ComparisonResult& result, const std::string& filepath) const;
    
    // Configuration
    void setMaxSnapshots(size_t maxSnapshots);
    void setAnomalyDetectionThreshold(float threshold);
    void setMemoryLeakDetectionThreshold(size_t threshold);
    void setPerformanceAnomalyThreshold(float threshold);
    void enableDevelopmentTracking(bool enable);
    void enableNeuromodulationTracking(bool enable);
    void enableMemoryLeakDetection(bool enable);
    void enablePerformanceMonitoring(bool enable);
    
    // Configuration getters
    size_t getMaxSnapshots() const;
    float getAnomalyDetectionThreshold() const;
    size_t getMemoryLeakDetectionThreshold() const;
    float getPerformanceAnomalyThreshold() const;
    bool getEnableDevelopmentTracking() const;
    bool getEnableNeuromodulationTracking() const;
    bool getEnableMemoryLeakDetection() const;
    bool getEnablePerformanceMonitoring() const;
    
    // State access
    std::vector<BrainSnapshot> getSnapshots() const;
    BrainSnapshot getLatestSnapshot() const;
    BrainSnapshot getPreviousSnapshot() const;
    std::vector<DebugReport> getDebugReports() const;
    std::vector<ComparisonResult> getComparisonResults() const;
    
    // Event handling
    void registerStateChangeCallback(std::function<void(const BrainStateChange&)> callback);
    void registerAnomalyCallback(std::function<void(const DebugReport&)> callback);
    void registerMemoryLeakCallback(std::function<void(const MemoryLeak&)> callback);
    void registerPerformanceAnomalyCallback(std::function<void(const PerformanceAnomaly&)> callback);

private:
    std::shared_ptr<Brain> brain_;
    
    // Configuration
    size_t maxSnapshots_;
    float anomalyDetectionThreshold_;
    size_t memoryLeakDetectionThreshold_;
    float performanceAnomalyThreshold_;
    bool enableDevelopmentTracking_;
    bool enableNeuromodulationTracking_;
    bool enableMemoryLeakDetection_;
    bool enablePerformanceMonitoring_;
    
    // State tracking
    std::vector<BrainSnapshot> snapshots_;
    std::vector<DebugReport> debugReports_;
    std::vector<ComparisonResult> comparisonResults_;
    std::vector<BrainStateChange> stateChanges_;
    
    // Callbacks
    std::vector<std::function<void(const BrainStateChange&)>> stateChangeCallbacks_;
    std::vector<std::function<void(const DebugReport&)>> anomalyCallbacks_;
    std::vector<std::function<void(const MemoryLeak&)>> memoryLeakCallbacks_;
    std::vector<std::function<void(const PerformanceAnomaly&)>> performanceAnomalyCallbacks_;
    
    // Analysis helpers
    float computeStateDifference(const BrainSnapshot& snapshot1, const BrainSnapshot& snapshot2) const;
    float computeNeuronStateDifference(const BrainSnapshot& snapshot1, const BrainSnapshot& snapshot2) const;
    float computeSynapticStateDifference(const BrainSnapshot& snapshot1, const BrainSnapshot& snapshot2) const;
    float computeMemoryStateDifference(const BrainSnapshot& snapshot1, const BrainSnapshot& snapshot2) const;
    float computeActivityStateDifference(const BrainSnapshot& snapshot1, const BrainSnapshot& snapshot2) const;
    
    // Trajectory analysis
    void findStateTrajectories(std::vector<std::vector<size_t>>& trajectories) const;
    void extractTrajectoryFeatures(const std::vector<BrainSnapshot>& trajectory,
                                  std::vector<float>& features) const;
    float computeTrajectorySimilarity(const std::vector<BrainSnapshot>& trajectory1,
                                     const std::vector<BrainSnapshot>& trajectory2) const;
    
    // Anomaly detection helpers
    bool detectStateAnomaly(const BrainSnapshot& snapshot) const;
    bool detectPerformanceAnomaly(const PerformanceMetrics& metrics) const;
    bool detectMemoryLeak() const;
    
    // Memory leak detection helpers
    void trackMemoryUsage(const BrainSnapshot& snapshot);
    void analyzeMemoryAllocationPatterns() const;
    void detectLeakSources(std::vector<MemoryLeak>& leaks) const;
    
    // Performance anomaly detection helpers
    void analyzePerformanceMetrics(const PerformanceMetrics& metrics);
    void detectPerformanceTrends() const;
    void classifyPerformanceAnomalies(std::vector<PerformanceAnomaly>& anomalies) const;
    
    // Configuration validation helpers
    void validateConfigParameters(std::vector<ConfigurationIssue>& issues) const;
    void validateNeuralParameters(std::vector<ConfigurationIssue>& issues) const;
    void validateMemoryParameters(std::vector<ConfigurationIssue>& issues) const;
    void validatePlasticityParameters(std::vector<ConfigurationIssue>& issues) const;
    
    // Development analysis helpers
    void analyzeDevelopmentalStage(const BrainSnapshot& snapshot, DevelopmentStageReport& report) const;
    void checkDevelopmentalMilestones(const BrainSnapshot& snapshot, std::vector<std::string>& milestones) const;
    void assessDevelopmentProgress(const BrainSnapshot& snapshot, float& progress) const;
    
    // Neuromodulation analysis helpers
    void analyzeNeuromodulationSignals(const BrainSnapshot& snapshot, NeuromodulationReport& report) const;
    void assessNeuromodulationBalance(const NeuromodulationReport& report, bool& isBalanced) const;
    void computeModulationEffects(const BrainSnapshot& snapshot, std::map<std::string, float>& effects) const;
    
    // Export helpers
    void exportSnapshotToJSON(const BrainSnapshot& snapshot, const std::string& filepath) const;
    void exportSnapshotToBinary(const BrainSnapshot& snapshot, const std::string& filepath) const;
    void exportDebugReportToJSON(const DebugReport& report, const std::string& filepath) const;
    void exportComparisonResultToJSON(const ComparisonResult& result, const std::string& filepath) const;
    
    mutable std::mutex mutex_;
};

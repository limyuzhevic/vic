#ifndef NLM_DEBUGGING_H
#define NLM_DEBUGGING_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <iomanip>

#include "core/Types/Types.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Brain.hpp"

namespace nlm {

// Debug event types for detailed tracing
enum class DebugEventType {
    NEURON_SPIKE,
    SYNAPSE_PLASTICITY,
    WEIGHT_UPDATE,
    CONNECTION_ESTABLISHMENT,
    CONNECTION_PRUNING,
    NEUROMODULATION,
    MEMORY_OPERATIONS,
    PREDICTION_ERROR,
    CONCEPT_FORMATION,
    ATTENTION_SELECTION,
    DEVELOPMENTAL_STAGE_CHANGE,
    REWARD_RECEIVED,
    NOVELTY_DETECTED,
    ERROR_OCCURRENCE,
    PERFORMANCE_METRIC
};

// Debug event structure for detailed tracing
struct DebugEvent {
    DebugEventType type;
    Timestamp timestamp;
    SimulationStep step;
    std::string description;
    std::map<std::string, std::string> metadata;
    std::map<std::string, float> floatMetadata;
    int neuronId;
    int synapseId;
    float value;
    std::string sourceFile;
    int sourceLine;

    DebugEvent() : type(DebugEventType::ERROR_OCCURRENCE), timestamp(0.0), step(0),
                   description(""), neuronId(-1), synapseId(-1), value(0.0f),
                   sourceFile(""), sourceLine(0) {}

    DebugEvent(DebugEventType t, Timestamp ts, SimulationStep s, const std::string& desc)
        : type(t), timestamp(ts), step(s), description(desc),
          neuronId(-1), synapseId(-1), value(0.0f), sourceFile(""), sourceLine(0) {}
};

// Advanced neural debugger with comprehensive tracing capabilities
class NeuralDebugger {
private:
    struct Impl;
    Impl* pImpl;

public:
    NeuralDebugger();
    ~NeuralDebugger();

    // Event logging
    void logEvent(const DebugEvent& event);
    void logNeuronSpike(NeuronId neuron, Timestamp time, SimulationStep step, float voltage);
    void logSynapsePlasticity(SynapseId synapse, float oldWeight, float newWeight, float change, Timestamp time);
    void logWeightUpdate(NeuronId preNeuron, NeuronId postNeuron, float oldWeight, float newWeight, float delta);
    void logConnectionEstablishment(NeuronId preNeuron, NeuronId postNeuron, float weight);
    void logConnectionPruning(NeuronId preNeuron, NeuronId postNeuron, float weight);
    void logNeuromodulation(float reward, float predictionError, float novelty, float curiosity);
    void logMemoryOperation(const std::string& operation, size_t neuronsAffected, size_t memoryUsed);
    void logPredictionError(float error, const std::vector<float>& predictions);
    void logConceptFormation(const std::string& concept, const std::vector<float>& pattern);
    void logAttentionSelection(NeuronId selectedNeuron, float attentionScore, const std::vector<float>& competition);
    void logDevelopmentalStageChange(DevelopmentalStage oldStage, DevelopmentalStage newStage, float maturationFactor);
    void logRewardReceived(float reward, const std::string& source, float predictedReward);
    void logNoveltyDetected(float novelty, const std::vector<float>& features);
    void logError(const std::string& error, const std::string& context);
    void logPerformanceMetric(const std::string& metric, float value, const std::map<std::string, float>& tags);

    // Event filtering and querying
    std::vector<DebugEvent> queryEvents(DebugEventType type, Timestamp startTime, Timestamp endTime) const;
    std::vector<DebugEvent> queryEvents(const std::string& descriptionPattern) const;
    std::vector<DebugEvent> queryEvents(const std::map<std::string, std::string>& filters) const;

    // Real-time monitoring
    void startRealTimeMonitoring(size_t maxEvents = 1000);
    void stopRealTimeMonitoring();
    bool isMonitoring() const;
    size_t getEventCount() const;

    // Event analysis
    std::map<std::string, float> analyzeEventFrequency() const;
    std::map<std::string, float> analyzeEventTiming() const;
    std::vector<std::pair<Timestamp, float>> getSpikeTrain(NeuronId neuron) const;
    std::vector<std::pair<Timestamp, float>> getWeightTrace(SynapseId synapse) const;

    // Debugging visualizations
    std::string generateEventTimeline() const;
    std::string generateSpikeRaster() const;
    std::string generateWeightEvolution() const;
    std::string generateNetworkActivity() const;

    // Configuration
    void setEventFilter(DebugEventType type, bool enabled);
    void setVerbosityLevel(int level);
    void setMaxEvents(size_t maxEvents);
    void enablePerformanceLogging(bool enable);
    void enableDetailedTracing(bool enable);

    // Export functionality
    bool exportEvents(const std::string& filename) const;
    bool exportAnalysis(const std::string& filename) const;
    std::string getDebugReport() const;
};

// Performance profiler with detailed timing analysis
class PerformanceProfiler {
private:
    struct Impl;
    Impl* pImpl;

public:
    PerformanceProfiler();
    ~PerformanceProfiler();

    // Function timing
    void startTiming(const std::string& functionName);
    void stopTiming(const std::string& functionName);
    double getAverageTime(const std::string& functionName) const;
    double getMinTime(const std::string& functionName) const;
    double getMaxTime(const std::string& functionName) const;
    size_t getCallCount(const std::string& functionName) const;

    // System resource monitoring
    struct SystemMetrics {
        double cpuUsage{0.0};
        size_t memoryUsage{0};
        size_t activeThreads{0};
        double diskIO{0.0};
        double networkIO{0.0};
        double temperature{0.0};  // For thermal monitoring
    };

    SystemMetrics getSystemMetrics() const;

    // Neural system specific timing
    void startNeuronTiming(NeuronId neuron);
    void stopNeuronTiming(NeuronId neuron);
    double getNeuronProcessingTime(NeuronId neuron) const;
    std::map<NeuronId, double> getAllNeuronTimes() const;

    void startSynapseTiming(SynapseId synapse);
    void stopSynapseTiming(SynapseId synapse);
    double getSynapseProcessingTime(SynapseId synapse) const;

    // Performance bottlenecks detection
    std::vector<std::string> detectPerformanceBottlenecks() const;
    std::vector<std::pair<std::string, double>> getSortedPerformanceMetrics() const;

    // Configuration
    void setSamplingInterval(double interval);
    void setProfilingDepth(int depth);
    void enableSystemMonitoring(bool enable);
    void enableDetailedFunctionTracing(bool enable);

    // Export and analysis
    bool exportPerformanceData(const std::string& filename) const;
    std::string generatePerformanceReport() const;
    std::map<std::string, double> getPerformanceSummary() const;
};

// Interactive neural network visualizer
class NeuralVisualizer {
private:
    struct Impl;
    Impl* pImpl;

public:
    NeuralVisualizer();
    ~NeuralVisualizer();

    // Network visualization
    void visualizeNetwork(const std::shared_ptr<Brain>& brain) const;
    void visualizeRegion(const NeuralRegion* region) const;
    void visualizePopulation(const NeuralPopulation* population) const;
    void visualizeConnections(const Brain* brain) const;

    // Activity visualization
    void visualizeSpikeActivity(const std::shared_ptr<Brain>& brain, Timestamp startTime, Timestamp endTime) const;
    void visualizeWeightMatrix(const Brain* brain) const;
    void visualizePlasticityChanges(const Brain* brain) const;
    void visualizeNeuromodulation(const Brain* brain) const;

    // State visualization
    void visualizeDevelopmentalStages(const Brain* brain) const;
    void visualizeMemoryActivity(const Brain* brain) const;
    void visualizePredictionAccuracy(const Brain* brain) const;

    // Configuration
    void setVisualizationStyle(const std::string& style);
    void setColorScheme(const std::map<std::string, std::string>& colors);
    void setUpdateInterval(double interval);
    void enableRealTimeUpdates(bool enable);

    // Export
    bool exportVisualization(const std::string& filename, const std::string& format = "png") const;
    std::string generateVisualizationScript() const;
    void saveCurrentView(const std::string& filename) const;

    // Interactive features
    void connectDebugger(std::shared_ptr<NeuralDebugger> debugger);
    void connectProfiler(std::shared_ptr<PerformanceProfiler> profiler);
    void setInteractionCallback(std::function<void(const std::string& action, const std::map<std::string, std::string>& params)> callback);
};

// Comprehensive error reporting system
class ErrorReporter {
private:
    struct Impl;
    Impl* pImpl;

public:
    ErrorReporter();
    ~ErrorReporter();

    // Error reporting
    void reportError(const std::string& error, const std::string& context = "");
    void reportWarning(const std::string& warning, const std::string& context = "");
    void reportInfo(const std::string& info, const std::string& context = "");
    void reportDebug(const std::string& debug, const std::string& context = "");

    // Neural system specific errors
    void reportNeuralError(NeuronId neuron, const std::string& error, const std::string& operation);
    void reportSynapseError(SynapseId synapse, const std::string& error, const std::string& operation);
    void reportMemoryError(const std::string& error, size_t requestedSize, size_t availableSize);
    void reportPlasticityError(const std::string& error, const std::string& ruleName, float parameterValue);
    void reportConfigurationError(const std::string& error, const std::string& key, const std::string& value);

    // Error analysis
    size_t getErrorCount() const;
    size_t getWarningCount() const;
    size_t getInfoCount() const;
    size_t getDebugCount() const;

    std::vector<std::string> getRecentErrors() const;
    std::vector<std::string> getErrorsByType(const std::string& type) const;
    std::string getErrorSummary() const;

    // Configuration
    void setErrorLevel(int level);
    void setErrorReportingFile(const std::string& filename);
    void enableConsoleOutput(bool enable);
    void enableFileOutput(bool enable);
    void setMaxErrorHistory(size_t maxHistory);

    // Export
    bool exportErrorReport(const std::string& filename) const;
    std::string generateErrorReport() const;
    void clearErrorHistory();

    // Integration
    void connectToDebugger(std::shared_ptr<NeuralDebugger> debugger);
    void connectToProfiler(std::shared_ptr<PerformanceProfiler> profiler);
};

// Advanced neural system monitor with real-time analytics
class NeuralSystemMonitor {
private:
    struct Impl;
    Impl* pImpl;

public:
    NeuralSystemMonitor();
    ~NeuralSystemMonitor();

    // System monitoring
    void startMonitoring(const std::shared_ptr<Brain>& brain);
    void stopMonitoring();
    bool isMonitoring() const;

    // Real-time metrics
    struct RealTimeMetrics {
        Timestamp simulationTime{0.0};
        size_t totalSpikeCount{0};
        float averageFiringRate{0.0f};
        float excitationInhibitionRatio{0.0f};
        size_t activeNeuronCount{0};
        size_t activeSynapseCount{0};
        float memoryUsage{0.0f};
        float cpuUsage{0.0f};
        double temperature{0.0};
        std::map<std::string, float> neuromodulationLevels;
    };

    RealTimeMetrics getRealTimeMetrics() const;

    // Event monitoring
    size_t getEventCountSince(Timestamp since) const;
    std::vector<DebugEvent> getRecentEvents(size_t maxEvents = 100) const;

    // Performance tracking
    double getAverageStepTime() const;
    double getMinStepTime() const;
    double getMaxStepTime() const;
    std::vector<double> getStepTimeHistory(size_t lastN = 1000) const;

    // Neural activity monitoring
    std::vector<float> getNeuronalActivityHistory(size_t neuronId, size_t lastN = 1000) const;
    std::vector<float> getSynapticActivityHistory(size_t synapseId, size_t lastN = 1000) const;

    // Configuration
    void setMonitoringInterval(double interval);
    void setMetricsBufferSize(size_t size);
    void enableEventRecording(bool enable);
    void enablePerformanceTracking(bool enable);
    void enableResourceMonitoring(bool enable);

    // Export and analysis
    bool exportMonitoringData(const std::string& filename) const;
    std::string generateMonitoringReport() const;
    std::map<std::string, double> getSystemStatistics() const;

    // Integration
    void connectToDebugger(std::shared_ptr<NeuralDebugger> debugger);
    void connectToProfiler(std::shared_ptr<PerformanceProfiler> profiler);
    void connectToVisualizer(std::shared_ptr<NeuralVisualizer> visualizer);
    void connectToErrorReporter(std::shared_ptr<ErrorReporter> errorReporter);
};

// Neural system analysis utilities
namespace Analysis {

    // Statistical analysis
    struct StatisticalSummary {
        double mean{0.0};
        double median{0.0};
        double stddev{0.0};
        double min{0.0};
        double max{0.0};
        double percentile95{0.0};
        double percentile99{0.0};
    };

    StatisticalSummary calculateStatistics(const std::vector<double>& data);
    StatisticalSummary calculateStatistics(const std::vector<float>& data);

    // Pattern detection
    bool detectBurstActivity(const std::vector<float>& spikeTrain, float threshold = 0.5f);
    bool detectRegularFiring(const std::vector<float>& spikeTrain, float coefficientOfVariation = 0.2f);
    bool detectAsynchronousRegularFiring(const std::vector<float>& spikeTrain, float window = 10.0f);

    // Network analysis
    double calculateNetworkEfficiency(const std::shared_ptr<Brain>& brain);
    double calculateNetworkClusteringCoefficient(const std::shared_ptr<Brain>& brain);
    double calculateSmallWorldness(const std::shared_ptr<Brain>& brain);
    std::vector<std::pair<NeuronId, NeuronId>> findShortestPaths(const std::shared_ptr<Brain>& brain, NeuronId source, NeuronId target);

    // Plasticity analysis
    struct PlasticityMetrics {
        double averageWeightChange{0.0};
        double averageSTDPStrength{0.0};
        double averageHebbianStrength{0.0};
        double plasticityVariability{0.0};
        double homeostaticRegulation{0.0};
    };

    PlasticityMetrics analyzePlasticity(const std::shared_ptr<Brain>& brain);
    std::vector<std::pair<Timestamp, float>> analyzeWeightTrace(const std::shared_ptr<Brain>& brain, SynapseId synapse);
    std::map<std::string, double> analyzePlasticityRules(const std::shared_ptr<Brain>& brain);

    // Memory analysis
    struct MemoryMetrics {
        size_t workingMemorySize{0};
        size_t episodicMemorySize{0};
        size_t associativeMemorySize{0};
        double memoryUtilization{0.0};
        double memoryRetrievalSpeed{0.0};
    };

    MemoryMetrics analyzeMemory(const std::shared_ptr<Brain>& brain);
    std::vector<std::string> analyzeMemoryPatterns(const std::shared_ptr<Brain>& brain);

    // Prediction analysis
    double calculatePredictionAccuracy(const std::shared_ptr<Brain>& brain);
    double calculatePredictionConfidence(const std::shared_ptr<Brain>& brain);
    std::vector<float> analyzePredictionErrors(const std::shared_ptr<Brain>& brain);

    // Development analysis
    double calculateDevelopmentalProgress(const std::shared_ptr<Brain>& brain);
    std::map<std::string, double> analyzeDevelopmentStages(const std::shared_ptr<Brain>& brain);
    std::vector<std::pair<DevelopmentalStage, double>> analyzeStageTransitions(const std::shared_ptr<Brain>& brain);

    // Export utilities
    bool exportAnalysisResults(const std::string& filename, const std::map<std::string, StatisticalSummary>& analysis);
    std::string generateAnalysisReport(const std::map<std::string, StatisticalSummary>& analysis);
}

} // namespace nlm

#endif // NLM_DEBUGGING_H

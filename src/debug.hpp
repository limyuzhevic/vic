// debug.h - Debug and visualization tools for advanced NLM users
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <chrono>

namespace nlm {

class Brain;
class NeuralRegion;
class Neuron;
class Synapse;

// Debug visualization modes
enum class VisualizationMode {
    None,
    NetworkGraph,
    NeuronActivity,
    SynapticWeights,
    MemoryTraces,
    Neuromodulation,
    DevelopmentStages,
    RecordingPlayback
};

// Performance profiling
struct PerformanceMetrics {
    double cpuUsage;
    double memoryUsage;
    size_t activeNeurons;
    size_t totalSpikes;
    float averageFiringRate;
    size_t checkpointSize;
    double simulationSpeed;
    double wallClockTime;
    std::chrono::duration<double> simulationDuration;
    std::chrono::duration<double> wallClockDuration;
};

// Neural network analysis
struct NetworkAnalysis {
    size_t neuronCount;
    size_t synapseCount;
    size_t activeRegionCount;
    float excitationInhibitionRatio;
    std::vector<float> neuronTypeDistribution;
    std::vector<float> firingRateDistribution;
    float averageSynapticWeight;
    float weightStandardDeviation;
    float minWeight;
    float maxWeight;
    std::map<NeuronType, size_t> neuronTypes;
};

// Memory system diagnostics
struct MemoryDiagnostics {
    size_t workingMemoryCapacity;
    size_t workingMemoryActiveTraces;
    size_t episodicMemoryCapacity;
    size_t episodicMemoryEpisodeCount;
    size_t associativeMemoryPatterns;
    float memoryConsolidationProgress;
    float replayEfficiency;
    float memoryRetrievalSpeed;
};

// Plasticity analysis
struct PlasticityAnalysis {
    float totalSTDPChange;
    float totalHebbianChange;
    float totalStructuralChange;
    std::vector<float> plasticityEvolution;
    float learningRate;
    float stability;
    float adaptability;
    std::map<std::string, float> plasticityMetrics;
};

// Development stage analysis
struct DevelopmentAnalysis {
    DevelopmentalStage currentStage;
    float developmentalProgress;
    float plasticityRate;
    float structuralPlasticityLevel;
    float maturationLevel;
    std::vector<float> stageMetrics;
    std::chrono::duration<double> stageDuration;
};

// Advanced debugger class for deep analysis
class NLMAdvancedDebugger {
public:
    NLMAdvancedDebugger() = default;
    ~NLMAdvancedDebugger() = default;
    
    // Core debugging functionality
    void connectToBrain(std::shared_ptr<Brain> brain);
    void disconnect();
    bool isConnected() const;
    
    // Data collection and analysis
    PerformanceMetrics collectPerformanceMetrics() const;
    NetworkAnalysis analyzeNetworkTopology() const;
    MemoryDiagnostics analyzeMemorySystems() const;
    PlasticityAnalysis analyzePlasticity() const;
    DevelopmentAnalysis analyzeDevelopment() const;
    
    // Visualization generation
    std::string generateNetworkGraph(VisualizationMode mode) const;
    std::string generateNeuronActivityHeatmap(VisualizationMode mode) const;
    std::string generateSynapticWeightDistribution(VisualizationMode mode) const;
    std::string generateMemoryTraceAnalysis(VisualizationMode mode) const;
    std::string generateNeuromodulationDynamics(VisualizationMode mode) const;
    std::string generateDevelopmentTimeline(VisualizationMode mode) const;
    
    // Recording and playback
    void startRecording(const std::string& filepath);
    void stopRecording();
    bool isRecording() const;
    void playbackRecording(const std::string& filepath, double speed = 1.0);
    
    // Advanced query methods
    std::vector<NeuronId> getActiveNeurons(NeuronType type) const;
    std::vector<NeuronId> getHighFiringNeurons(float threshold) const;
    std::vector<SynapseId> getStrongSynapses(float threshold) const;
    std::vector<SynapseId> getPlasticSynapses(PlasticityFlags flags) const;
    std::vector<MemoryTrace> getActiveMemoryTraces() const;
    
    // Pattern detection
    std::vector<std::vector<NeuronId>> detectBurstPatterns(size_t minDuration, size_t maxDuration) const;
    std::vector<std::vector<NeuronId>> detectSynchronousGroups(float correlationThreshold) const;
    std::vector<std::vector<NeuronId>> detectWeightedPaths(float minPathStrength) const;
    
    // Simulation control
    void stepSimulations(size_t steps);
    void pauseSimulation();
    void resumeSimulation();
    void fastForwardSimulation(double factor);
    void setDebugLevel(int level);
    
    // Configuration
    void configureVisualization(VisualizationMode mode, const std::string& config);
    void configureAnalysis(const std::string& analysisType, const std::map<std::string, std::string>& parameters);
    void configureOutput(const std::string& format, const std::string& filepath);
    
    // Export and import
    bool exportData(const std::string& filepath, const std::string& format = "json") const;
    bool importData(const std::string& filepath, const std::string& format = "json");
    void exportNetworkGraph(const std::string& filepath, VisualizationMode mode) const;
    void importNetworkGraph(const std::string& filepath);
    
    // Report generation
    std::string generateDiagnosticReport() const;
    std::string generatePerformanceReport() const;
    std::string generateIntegrationReport() const;
    std::string generateHealthReport() const;
    
    // Advanced visualization
    void generateInteractiveHTMLReport(const std::string& filepath) const;
    void generate3DNetworkView(const std::string& filepath) const;
    void generateTimeSeriesPlot(const std::string& filepath, const std::string& dataType) const;
    
    // Machine learning integration
    std::vector<float> extractFeaturesForML(const std::string& featureSet) const;
    void trainBehaviorPredictor(const std::vector<std::vector<float>>& trainingData);
    float predictBehavior(const std::vector<float>& stateVector) const;
    
    // Real-time monitoring
    void startRealTimeMonitoring(double interval);
    void stopRealTimeMonitoring();
    std::function<void(const PerformanceMetrics&)> setPerformanceCallback();
    std::function<void(const NetworkAnalysis&)> setNetworkCallback();
    std::function<void(const MemoryDiagnostics&)> setMemoryCallback();
    
    // Profiling
    void startProfiling(const std::string& profileName);
    void stopProfiling();
    std::map<std::string, double> getProfilingResults() const;
    void resetProfiling();
    
    // State management
    void saveDebugState(const std::string& filepath) const;
    void loadDebugState(const std::string& filepath);
    void clearDebugState();
    
private:
    std::shared_ptr<Brain> brain_;
    bool connected_ = false;
    bool recording_ = false;
    bool profiling_ = false;
    VisualizationMode visualizationMode_ = VisualizationMode::None;
    int debugLevel_ = 1;
    double simulationSpeed_ = 1.0;
    std::string currentRecordingFile_;
    std::map<std::string, double> profilingData_;
    std::chrono::steady_clock::time_point recordingStart_;
    std::chrono::steady_clock::time_point simulationStart_;
    
    // Helper methods
    void updateRecordingData();
    void calculateMetrics() const;
    std::string formatNetworkGraph() const;
    std::string formatNeuronActivity() const;
    std::string formatSynapticWeights() const;
    std::string formatMemoryTraces() const;
    std::string formatNeuromodulation() const;
    std::string formatDevelopment() const;
};

// Simple debug interface for basic usage
class NLMDebugInterface {
public:
    NLMDebugInterface() = default;
    ~NLMDebugInterface() = default;
    
    void log(const std::string& message, int level = 1);
    void warn(const std::string& message);
    void error(const std::string& message);
    void info(const std::string& message);
    
    void enableVerboseLogging();
    void disableLogging();
    void setLogLevel(int level);
    
    std::vector<std::string> getRecentLogs(int count = 10) const;
    void clearLogHistory();
    
    void generateSummaryReport(const std::string& filepath) const;
    void generateDebugLog(const std::string& filepath) const;
    
private:
    std::vector<std::pair<int, std::string>> logHistory_;
    int logLevel_ = 1;
    bool loggingEnabled_ = true;
};

// Utility functions for advanced users
namespace debug_utils {
    std::string generateTimestamp();
    std::string formatDuration(std::chrono::duration<double> duration);
    std::string formatMemorySize(size_t bytes);
    std::string escapeHTML(const std::string& text);
    std::string escapeJSON(const std::string& text);
    std::string escapeYAML(const std::string& text);
    
    template<typename T>
    std::string toString(const T& value);
    
    template<typename T>
    std::vector<T> clamp(const std::vector<T>& values, const T& min, const T& max);
    
    double calculateAverage(const std::vector<double>& values);
    double calculateStandardDeviation(const std::vector<double>& values, double mean);
    
    std::string generateColorGradient(double value, double min, double max);
    std::string getColorForNeuronType(NeuronType type);
    std::string getColorForSynapseType(SynapseType type);
}

} // namespace nlm

// Convenience functions for easy debugging
inline void nlm_debug_log(const std::string& message, int level = 1) {
    static NLMDebugInterface debugInterface;
    debugInterface.log(message, level);
}

inline void nlm_debug_info(const std::string& message) {
    static NLMDebugInterface debugInterface;
    debugInterface.info(message);
}

inline void nlm_debug_warn(const std::string& message) {
    static NLMDebugInterface debugInterface;
    debugInterface.warn(message);
}

inline void nlm_debug_error(const std::string& message) {
    static NLMDebugInterface debugInterface;
    debugInterface.error(message);
}

inline void nlm_start_debugger(std::shared_ptr<nlm::Brain> brain) {
    static NLMAdvancedDebugger debugger;
    debugger.connectToBrain(brain);
}

inline void nlm_stop_debugger() {
    static NLMAdvancedDebugger debugger;
    debugger.disconnect();
}
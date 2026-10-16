// Comprehensive debugging and visualization framework for NLM
// Provides real-time monitoring, analysis tools, and multiple output formats

#pragma once

#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../performance/benchmark.cpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace nlm {

// Forward declarations
class VisualizationBackend;
class DebugMonitor;
class EventLogger;
class PerformanceProfiler;

// Visualization data structures
struct NeuralActivityData {
    SimulationStep step;
    float averageFiringRate;
    size_t activeNeurons;
    size_t firingNeurons;
    std::vector<float> neuronActivities;
    std::vector<SpikeEvent> spikeEvents;
};

struct SynapticData {
    SimulationStep step;
    std::vector<float> weightMatrix;
    std::vector<float> weightChanges;
    std::map<std::pair<NeuronId, NeuronId>, float> synapseWeights;
};

struct MemorySystemData {
    SimulationStep step;
    
    // Working memory
    size_t workingMemoryTraces;
    std::vector<float> workingMemoryActivations;
    
    // Episodic memory
    size_t episodicMemoryCount;
    std::vector<float> episodeRewards;
    std::vector<SimulationStep> episodeTimestamps;
    
    // Associative memory
    size_t associativeMemoryAssociations;
    std::vector<float> associationStrengths;
};

struct NeuromodulationData {
    SimulationStep step;
    
    // Key neuromodulators
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float acetylcholineLevel;
    float norepinephrineLevel;
    float serotoninLevel;
    
    // Plasticity effects
    float plasticityModulation;
};

struct DevelopmentData {
    SimulationStep step;
    DevelopmentalStage stage;
    float developmentProgress;
    std::vector<float> regionMaturation;
    std::vector<float> synaptogenesisRate;
    std::vector<float> pruningRate;
};

struct PerformanceData {
    SimulationStep step;
    double cpuTime;
    double memoryUsage;
    size_t activeThreads;
    std::vector<double> subsystemTimes;
    std::vector<size_t> memoryAllocations;
};

struct EventData {
    SimulationStep step;
    std::string eventType;
    std::string eventDescription;
    float eventValue;
    std::vector<std::string> eventTags;
};

// Visualization configuration
struct VisualizationConfig {
    // Output formats
    bool enableAsciiOutput;
    bool enableSvgOutput;
    bool enableJsonOutput;
    bool enableBinaryOutput;
    bool enableCsvOutput;
    
    // Real-time settings
    bool realTimeEnabled;
    double updateInterval;
    size_t maxHistorySize;
    
    // Output settings
    std::string outputDirectory;
    std::string filenamePrefix;
    bool compressData;
    bool detailedLogging;
    
    // Visualization options
    bool showNeuralActivity;
    bool showSynapticWeights;
    bool showMemorySystems;
    bool showNeuromodulation;
    bool showDevelopment;
    bool showPerformance;
    bool showEvents;
    
    // Brain state tracking
    bool trackBrainState;
    bool trackMemoryState;
    bool trackNeuromodulationState;
    bool trackPlasticityState;
    
    // Debug settings
    bool enableDebugMode;
    bool enableMemoryLeakDetection;
    bool enablePerformanceProfiling;
    bool enableEventTimeline;
    
    // Integration
    bool integrateWithExperiments;
    bool enableAutomatedVisualization;
    bool enablePluginArchitecture;
    bool enableBatchProcessing;
};

// Main visualization and debugging framework
class EnhancedVisualization : public VisualizationInterface {
public:
    EnhancedVisualization();
    ~EnhancedVisualization();
    
    // Initialize with configuration
    bool initialize(const VisualizationConfig& config) override;
    
    // Update visualization data
    void update() override;
    
    // Render current state
    void render() override;
    
    // Close visualization
    void close() override;
    
    // Check if visualization is active
    bool isActive() const override;
    
    // Set update rate
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Enhanced debugging and visualization methods
    
    // Core visualization methods (override interface)
    void visualizeNetwork(const Brain& brain) override;
    void visualizeActivity(const Brain& brain) override;
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes) override;
    void visualizeWeights(const Brain& brain) override;
    
    // Advanced debugging and visualization capabilities
    
    // Neural activity visualization
    void visualizeNeuralActivityHeatMap(const Brain& brain);
    void visualizeSpikeTrains(const Brain& brain, size_t neuronsToShow = 10);
    void visualizeActivityPatterns(const Brain& brain);
    
    // Synaptic connectivity visualization
    void visualizeSynapticConnectivityGraph(const Brain& brain);
    void visualizeWeightMatrix(const Brain& brain);
    void visualizePlasticityDynamics(const Brain& brain);
    void visualizeStructuralConnectivity(const Brain& brain);
    
    // Memory system visualization
    void visualizeWorkingMemory(const Brain& brain);
    void visualizeEpisodicMemory(const Brain& brain);
    void visualizeAssociativeMemory(const Brain& brain);
    void visualizeMemoryConsolidation(const Brain& brain);
    
    // Neuromodulation visualization
    void visualizeDopamineSignals(const Brain& brain);
    void visualizeCuriositySignals(const Brain& brain);
    void visualizeNoveltySignals(const Brain& brain);
    void visualizeNeuromodulationNetwork(const Brain& brain);
    
    // Brain state monitoring
    void monitorBrainState(const Brain& brain);
    void trackDevelopmentStages(const Brain& brain);
    void visualizePredictionSystem(const Brain& brain);
    void visualizeCognitiveFunctions(const Brain& brain);
    
    // Performance profiling
    void profilePerformance(const Brain& brain);
    void detectMemoryLeaks(const Brain& brain);
    void analyzePlasticityEfficiency(const Brain& brain);
    void optimizeRendering(const Brain& brain);
    
    // Event and timeline visualization
    void logEvent(const std::string& eventType, const std::string& description, 
                  float value, const std::vector<std::string>& tags = {});
    void visualizeEventTimeline(const Brain& brain);
    void analyzeEventPatterns(const Brain& brain);
    
    // Experimental integration
    void integrateWithExperiments(const Brain& brain);
    void generateExperimentReports(const Brain& brain);
    void createRealTimeDashboards(const Brain& brain);
    
    // Output formats
    void exportToAscii(const std::string& filename, const Brain& brain);
    void exportToJson(const std::string& filename, const Brain& brain);
    void exportToCsv(const std::string& filename, const Brain& brain);
    void exportToSvg(const std::string& filename, const Brain& brain);
    void exportToBinary(const std::string& filename, const Brain& brain);
    
    // Batch processing
    void batchProcess(const std::vector<std::shared_ptr<Brain>>& brains);
    void processLargeScaleSimulation(const Brain& brain, size_t steps);
    void streamVisualization(const Brain& brain);
    
    // Plugin architecture
    void registerPlugin(std::function<void(const Brain&)> plugin);
    void unregisterPlugin(std::function<void(const Brain&)> plugin);
    void runPlugins(const Brain& brain);
    
    // Configuration
    void updateConfig(const VisualizationConfig& config);
    const VisualizationConfig& getConfig() const { return config_; }
    
    // Data management
    void clearHistory();
    void setHistorySize(size_t size);
    size_t getHistorySize() const { return historySize_; }
    
    // Real-time monitoring
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoringActive() const { return realTimeMonitoring_; }
    
    // Data access
    const std::vector<NeuralActivityData>& getNeuralActivityHistory() const { return neuralActivityHistory_; }
    const std::vector<MemorySystemData>& getMemoryHistory() const { return memoryHistory_; }
    const std::vector<NeuromodulationData>& getNeuromodulationHistory() const { return neuromodulationHistory_; }
    const std::vector<PerformanceData>& getPerformanceHistory() const { return performanceHistory_; }
    const std::vector<EventData>& getEventHistory() const { return eventHistory_; }
    
    // Analysis
    void analyzeNeuralDynamics(const Brain& brain);
    void analyzeMemoryPatterns(const Brain& brain);
    void analyzePlasticityTrajectories(const Brain& brain);
    void analyzeDevelopmentTrajectories(const Brain& brain);
    void analyzePerformanceBottlenecks(const Brain& brain);
    
    // Parameter sensitivity
    void performParameterSensitivityAnalysis(const Brain& brain, 
                                            const std::map<std::string, float>& parameters);
    void analyzeConfigImpact(const VisualizationConfig& config, const Brain& brain);
    
    // Integration with existing experiment framework
    void integrateWithExperimentRunner(const std::shared_ptr<ExperimentRunner>& runner);
    void integrateWithMetricsSystem(const std::shared_ptr<Metrics>& metrics);
    
private:
    // Core components
    VisualizationConfig config_;
    std::unique_ptr<VisualizationBackend> backend_;
    std::unique_ptr<DebugMonitor> debugMonitor_;
    std::unique_ptr<EventLogger> eventLogger_;
    std::unique_ptr<PerformanceProfiler> profiler_;
    
    // History data
    std::vector<NeuralActivityData> neuralActivityHistory_;
    std::vector<MemorySystemData> memoryHistory_;
    std::vector<NeuromodulationData> neuromodulationHistory_;
    std::vector<DevelopmentData> developmentHistory_;
    std::vector<PerformanceData> performanceHistory_;
    std::vector<EventData> eventHistory_;
    
    // State tracking
    SimulationStep currentStep_;
    double lastUpdateTime_;
    size_t historySize_;
    bool realTimeMonitoring_;
    std::thread monitoringThread_;
    std::atomic<bool> monitoringActive_;
    
    // Plugin system
    std::vector<std::function<void(const Brain&)>> plugins_;
    
    // Brain reference for visualization
    std::shared_ptr<Brain> currentBrain_;
    
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // Helper methods
    void updateInternalData(const Brain& brain);
    void generateVisualizationFrames(const Brain& brain);
    void processOutputFormats(const Brain& brain);
    void compressData(std::vector<uint8_t>& data);
    void decompressData(std::vector<uint8_t>& data);
    std::string generateTimestamp() const;
    void logMessage(const std::string& message, const std::string& level = "INFO");
};

} // namespace nlm

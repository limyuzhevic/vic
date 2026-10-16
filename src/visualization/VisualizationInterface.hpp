#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class SpikeEvent;
class VisualizationConfig;
class VisualizationInterfaceImpl;

// Visualization interface for debugging and analysis
// PLACEHOLDER - Phase 2 will have real visualization
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize();
    
    // Update visualization data
    void update();
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
    // Enhanced debugging and visualization methods
    void visualizeNeuralActivityHeatMap(const class Brain& brain);
    void visualizeSpikeTrains(const class Brain& brain, size_t neuronsToShow = 10);
    void visualizeActivityPatterns(const class Brain& brain);
    void visualizeSynapticConnectivityGraph(const class Brain& brain);
    void visualizeWeightMatrix(const class Brain& brain);
    void visualizePlasticityDynamics(const class Brain& brain);
    void visualizeStructuralConnectivity(const class Brain& brain);
    void visualizeWorkingMemory(const class Brain& brain);
    void visualizeEpisodicMemory(const class Brain& brain);
    void visualizeAssociativeMemory(const class Brain& brain);
    void visualizeMemoryConsolidation(const class Brain& brain);
    void visualizeDopamineSignals(const class Brain& brain);
    void visualizeCuriositySignals(const class Brain& brain);
    void visualizeNoveltySignals(const class Brain& brain);
    void visualizeNeuromodulationNetwork(const class Brain& brain);
    void monitorBrainState(const class Brain& brain);
    void trackDevelopmentStages(const class Brain& brain);
    void visualizePredictionSystem(const class Brain& brain);
    void visualizeCognitiveFunctions(const class Brain& brain);
    void profilePerformance(const class Brain& brain);
    void detectMemoryLeaks(const class Brain& brain);
    void analyzePlasticityEfficiency(const class Brain& brain);
    void optimizeRendering(const class Brain& brain);
    void logEvent(const std::string& eventType, const std::string& description, 
                  float value, const std::vector<std::string>& tags = {});
    void visualizeEventTimeline(const class Brain& brain);
    void analyzeEventPatterns(const class Brain& brain);
    void integrateWithExperiments(const class Brain& brain);
    void generateExperimentReports(const class Brain& brain);
    void createRealTimeDashboards(const class Brain& brain);
    void exportToAscii(const std::string& filename, const class Brain& brain);
    void exportToJson(const std::string& filename, const class Brain& brain);
    void exportToCsv(const std::string& filename, const class Brain& brain);
    void exportToSvg(const std::string& filename, const class Brain& brain);
    void exportToBinary(const std::string& filename, const class Brain& brain);
    void batchProcess(const std::vector<std::shared_ptr<class Brain>>& brains);
    void processLargeScaleSimulation(const class Brain& brain, size_t steps);
    void streamVisualization(const class Brain& brain);
    void registerPlugin(std::function<void(const class Brain&)> plugin);
    void unregisterPlugin(std::function<void(const class Brain&)> plugin);
    void runPlugins(const class Brain& brain);
    void updateConfig(const VisualizationConfig& config);
    const VisualizationConfig& getConfig() const;
    void clearHistory();
    void setHistorySize(size_t size);
    size_t getHistorySize() const;
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoringActive() const;
    
    // Analysis methods
    void analyzeNeuralDynamics(const class Brain& brain);
    void analyzeMemoryPatterns(const class Brain& brain);
    void analyzePlasticityTrajectories(const class Brain& brain);
    void analyzeDevelopmentTrajectories(const class Brain& brain);
    void analyzePerformanceBottlenecks(const class Brain& brain);
    void performParameterSensitivityAnalysis(
        const class Brain& brain, const std::map<std::string, float>& parameters);
    void analyzeConfigImpact(const VisualizationConfig& config, const class Brain& brain);
    void integrateWithExperimentRunner(const std::shared_ptr<class ExperimentRunner>& runner);
    void integrateWithMetricsSystem(const std::shared_ptr<class Metrics>& metrics);

private:
    std::unique_ptr<VisualizationInterfaceImpl> pImpl;
    
}; // namespace nlm

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

} // namespace nlm

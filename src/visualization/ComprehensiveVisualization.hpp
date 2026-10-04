#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace nlm {

// Forward declarations
class Brain;
class SpikeEvent;
class NeuralRegion;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class SimulationClock;
class CheckpointManager;
class ExperimentRunner;

// Color utilities for visualization
struct Color {
    float r, g, b, a;
    
    Color() : r(0), g(0), b(0), a(1) {}
    Color(float red, float green, float blue, float alpha = 1.0f) 
        : r(red), g(green), b(blue), a(alpha) {}
    
    static Color Red() { return Color(1.0f, 0.0f, 0.0f); }
    static Color Green() { return Color(0.0f, 1.0f, 0.0f); }
    static Color Blue() { return Color(0.0f, 0.0f, 1.0f); }
    static Color Yellow() { return Color(1.0f, 1.0f, 0.0f); }
    static Color Cyan() { return Color(0.0f, 1.0f, 1.0f); }
    static Color Magenta() { return Color(1.0f, 0.0f, 1.0f); }
    static Color White() { return Color(1.0f, 1.0f, 1.0f); }
    static Color Black() { return Color(0.0f, 0.0f, 0.0f); }
    static Color Gray() { return Color(0.5f, 0.5f, 0.5f); }
};

// Data structures for visualization
struct NeuralActivityData {
    Timestamp timestamp;
    std::vector<float> membranePotentials;
    std::vector<bool> spikeStates;
    std::vector<float> firingRates;
    std::vector<int> activeNeurons;
    float globalExcitationInhibitionRatio;
    size_t totalSpikeCount;
    size_t activeNeuronCount;
    size_t firingNeuronCount;
    float averageFiringRate;
};

struct NetworkTopologyData {
    Timestamp timestamp;
    std::vector<RegionId> regionIds;
    std::vector<std::string> regionNames;
    std::vector<std::vector<float>> connectivityMatrix;
    std::vector<std::vector<float>> synapticWeights;
    std::vector<Color> regionColors;
    float minWeight, maxWeight;
};

struct MemoryData {
    Timestamp timestamp;
    
    // Working memory
    std::vector<float> workingMemoryActivation;
    std::vector<std::string> workingMemoryContents;
    
    // Episodic memory
    std::vector<Timestamp> episodeTimestamps;
    std::vector<std::vector<float>> episodePatterns;
    std::vector<std::string> episodeDescriptions;
    
    // Semantic memory
    std::vector<std::string> concepts;
    std::vector<std::vector<float>> conceptVectors;
    std::vector<std::vector<float>> semanticConnections;
};

struct NeuromodulationData {
    Timestamp timestamp;
    
    // Neuromodulator levels
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionErrorLevel;
    
    // Attention and focus
    std::vector<float> attentionWeights;
    std::vector<float> focusAreas;
    
    // Reward prediction error
    float rewardPredictionError;
    float expectedValue;
    float actualValue;
};

class NeuralActivityVisualizer {
public:
    NeuralActivityVisualizer();
    ~NeuralActivityVisualizer();
    
    // Initialize the visualizer
    bool initialize(int width, int height, const std::string& title);
    
    // Update visualization data
    void update(const NeuralActivityData& data);
    
    // Render neural activity
    void render();
    
    // Render spike raster plot
    void renderSpikeRaster(const std::vector<DetailedSpikeEvent>& spikes);
    
    // Render membrane potential time series
    void renderMembranePotential(const std::vector<float>& potentials, 
                                 const std::vector<Timestamp>& timestamps);
    
    // Render firing rate heatmaps
    void renderFiringRateHeatmap(const std::vector<std::vector<float>>& rates);
    
    // Render population activity graphs
    void renderPopulationActivity(const std::vector<float>& activities, 
                                  const std::vector<std::string>& labels);
    
    // Close visualization
    void close();
    
    // Check if active
    bool isActive() const { return active_; }
    
    // Set update rate
    void setUpdateRate(double hz) { updateRate_ = hz; }
    double getUpdateRate() const { return updateRate_; }
    
    // Set visualization parameters
    void setSpikeThreshold(float threshold) { spikeThreshold_ = threshold; }
    void setColorMap(const std::vector<Color>& colormap) { colorMap_ = colormap; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool active_;
    double updateRate_;
    float spikeThreshold_;
    std::vector<Color> colorMap_;
    std::mutex dataMutex_;
    
    // Internal rendering methods
    void renderRealTimeConsole(const NeuralActivityData& data);
    void renderSpikeRasterPlot(const std::vector<DetailedSpikeEvent>& spikes);
    void renderVoltageTimeSeries(const std::vector<float>& potentials);
    void renderRateHeatmap(const std::vector<std::vector<float>>& rates);
    void renderStatistics(const NeuralActivityData& data);
};

class NetworkTopologyVisualizer {
public:
    NetworkTopologyVisualizer();
    ~NetworkTopologyVisualizer();
    
    // Initialize visualizer
    bool initialize(int width, int height, const std::string& title);
    
    // Update network topology data
    void update(const NetworkTopologyData& data);
    
    // Render network graph
    void render();
    
    // Render brain region hierarchy
    void renderRegionHierarchy(const std::vector<RegionId>& regionIds,
                              const std::vector<std::string>& regionNames,
                              const std::vector<Color>& colors);
    
    // Render connectivity matrix
    void renderConnectivityMatrix(const std::vector<std::vector<float>>& weights,
                                  const std::vector<std::string>& rowLabels,
                                  const std::vector<std::string>& colLabels);
    
    // Render synaptic weight visualization
    void renderSynapticWeights(const std::vector<std::vector<float>>& weights,
                              float minWeight, float maxWeight);
    
    // Render neural population visualization
    void renderPopulationVisualization(const std::vector<NeuralPopulation*>& populations);
    
    // Render development and plasticity visualization
    void renderDevelopmentVisualization(float developmentalStage,
                                        const std::vector<float>& plasticityFactors);
    
    // Export network graph to DOT format
    bool exportToDOT(const std::string& filepath, const NetworkTopologyData& data);
    
    // Export network graph to GraphML format
    bool exportToGraphML(const std::string& filepath, const NetworkTopologyData& data);
    
    // Close visualizer
    void close();
    
    bool isActive() const { return active_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool active_;
    std::mutex dataMutex_;
};

class MemorySystemVisualizer {
public:
    MemorySystemVisualizer();
    ~MemorySystemVisualizer();
    
    // Initialize visualizer
    bool initialize(int width, int height, const std::string& title);
    
    // Update memory system data
    void update(const MemoryData& data);
    
    // Render memory system visualization
    void render();
    
    // Render working memory trace
    void renderWorkingMemoryTrace(const std::vector<float>& activations,
                                 const std::vector<std::string>& contents,
                                 const std::vector<Timestamp>& timestamps);
    
    // Render episodic memory timeline
    void renderEpisodicMemoryTimeline(const std::vector<Timestamp>& timestamps,
                                      const std::vector<std::string>& descriptions,
                                      const std::vector<std::vector<float>>& patterns);
    
    // Render concept formation and semantic network
    void renderSemanticNetwork(const std::vector<std::string>& concepts,
                              const std::vector<std::vector<float>>& vectors,
                              const std::vector<std::vector<float>>& connections);
    
    // Render memory consolidation and replay
    void renderMemoryConsolidation(const std::vector<float>& consolidationProgress,
                                   const std::vector<Timestamp>& replayTimestamps,
                                   const std::vector<std::vector<float>>& replayPatterns);
    
    // Export memory data to JSON
    bool exportToJSON(const std::string& filepath, const MemoryData& data);
    
    // Close visualizer
    void close();
    
    bool isActive() const { return active_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool active_;
    std::mutex dataMutex_;
};

class NeuromodulationVisualizer {
public:
    NeuromodulationVisualizer();
    ~NeuromodulationVisualizer();
    
    // Initialize visualizer
    bool initialize(int width, int height, const std::string& title);
    
    // Update neuromodulation data
    void update(const NeuromodulationData& data);
    
    // Render neuromodulation visualization
    void render();
    
    // Render neuromodulator levels
    void renderNeuromodulatorLevels(float dopamine, float curiosity,
                                   float novelty, float predictionError);
    
    // Render attention and focus visualization
    void renderAttentionVisualization(const std::vector<float>& attentionWeights,
                                      const std::vector<float>& focusAreas,
                                      const std::vector<std::string>& regionLabels);
    
    // Render reward prediction error visualization
    void renderRewardPredictionError(float rpe, float expectedValue,
                                     float actualValue, Timestamp timestamp);
    
    // Render plasticity integration visualization
    void renderPlasticityIntegration(const std::vector<float>& learningRates,
                                    const std::vector<float>& plasticityFactors);
    
    // Close visualizer
    void close();
    
    bool isActive() const { return active_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool active_;
    std::mutex dataMutex_;
};

class DebugTools {
public:
    DebugTools(std::shared_ptr<Brain> brain, std::shared_ptr<SimulationClock> clock);
    ~DebugTools();
    
    // Spike propagation debugger
    class SpikeDebugger {
    public:
        SpikeDebugger(std::shared_ptr<Brain> brain);
        ~SpikeDebugger();
        
        // Step-by-step spike tracing
        void stepByStepTrace(SimulationStep step, NeuronId neuronId);
        
        // Get spike propagation path
        std::vector<NeuronId> getSpikePropagationPath(NeuronId sourceNeuron,
                                                    SimulationStep maxSteps = 10);
        
        // Check synapse connectivity
        bool validateSynapseConnectivity(NeuronId pre, NeuronId post);
        
        // Get synapse details
        struct SynapseInfo {
            NeuronId preNeuronId;
            NeuronId postNeuronId;
            float weight;
            float delay;
            bool isPlastic;
            float lastUpdateTime;
        };
        
        std::vector<SynapseInfo> getSynapseInfo(NeuronId neuronId) const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Neural computation debugger
    class ComputationDebugger {
    public:
        ComputationDebugger(std::shared_ptr<Brain> brain);
        ~ComputationDebugger();
        
        // Variable inspection
        struct VariableSnapshot {
            std::string name;
            double value;
            Timestamp timestamp;
            std::string context;
        };
        
        std::vector<VariableSnapshot> inspectVariables(const std::vector<std::string>& variableNames);
        
        // Neural state inspection
        struct NeuralState {
            NeuronId neuronId;
            float membranePotential;
            bool isSpiking;
            Timestamp lastSpikeTime;
            float inputCurrent;
            float outputCurrent;
        };
        
        std::vector<NeuralState> inspectNeuralState(NeuronId neuronId) const;
        std::vector<NeuralState> inspectAllNeuralStates() const;
        
        // Computation step debugging
        void debugStep(SimulationStep step, const std::string& stepName = "");
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Performance profiler
    class PerformanceProfiler {
    public:
        PerformanceProfiler(std::shared_ptr<Brain> brain);
        ~PerformanceProfiler();
        
        // Profile spike processing
        struct SpikeProcessingStats {
            size_t spikeCount;
            double processingTimeMs;
            double averageLatency;
            size_t pendingSpikes;
            size_t delayedSpikes;
        };
        
        SpikeProcessingStats profileSpikeProcessing(SimulationStep step);
        
        // Profile plasticity updates
        struct PlasticityStats {
            size_t stdpUpdates;
            size_t hebbianUpdates;
            size_t structuralUpdates;
            double totalPlasticityTimeMs;
            std::unordered_map<std::string, double> plasticityTimePerRule;
        };
        
        PlasticityStats profilePlasticityUpdates(SimulationStep step);
        
        // Memory usage monitoring
        struct MemoryUsage {
            size_t totalAllocatedBytes;
            size_t neuronMemoryBytes;
            size_t synapseMemoryBytes;
            size_t memoryPoolFragmentation;
            double allocationRate;
        };
        
        MemoryUsage profileMemoryUsage() const;
        
        // Performance bottleneck identification
        std::vector<std::string> identifyPerformanceBottlenecks() const;
        
        // Generate performance report
        std::string generatePerformanceReport() const;
        
        // Export performance data to CSV
        bool exportPerformanceData(const std::string& filepath) const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Memory leak detection
    class MemoryLeakDetector {
    public:
        MemoryLeakDetector(std::shared_ptr<Brain> brain);
        ~MemoryLeakDetector();
        
        // Detect memory leaks
        struct LeakInfo {
            std::string allocationSite;
            size_t bytesAllocated;
            size_t bytesFreed;
            size_t netLeak;
            Timestamp firstSeen;
            Timestamp lastSeen;
            std::string objectType;
        };
        
        std::vector<LeakInfo> detectMemoryLeaks();
        
        // Track specific memory pools
        struct MemoryPoolInfo {
            std::string poolName;
            size_t currentSize;
            size_t maxSize;
            size_t totalAllocations;
            size_t totalDeallocations;
            double growthRate;
        };
        
        std::vector<MemoryPoolInfo> trackMemoryPools() const;
        
        // Generate memory leak report
        std::string generateMemoryLeakReport() const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Get debug tool instances
    SpikeDebugger& getSpikeDebugger() { return *spikeDebugger_; }
    ComputationDebugger& getComputationDebugger() { return *computationDebugger_; }
    PerformanceProfiler& getPerformanceProfiler() { return *performanceProfiler_; }
    MemoryLeakDetector& getMemoryLeakDetector() { return *memoryLeakDetector_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimulationClock> clock_;
    std::unique_ptr<SpikeDebugger> spikeDebugger_;
    std::unique_ptr<ComputationDebugger> computationDebugger_;
    std::unique_ptr<PerformanceProfiler> performanceProfiler_;
    std::unique_ptr<MemoryLeakDetector> memoryLeakDetector_;
};

// File output writers for visualization formats
class VisualizationOutputWriter {
public:
    VisualizationOutputWriter();
    ~VisualizationOutputWriter();
    
    // Write neural activity data to file
    bool writeNeuralActivity(const std::string& filepath, 
                           const NeuralActivityData& data);
    
    // Write network topology data to DOT format
    bool writeNetworkToDOT(const std::string& filepath,
                          const NetworkTopologyData& data);
    
    // Write network topology data to GraphML format
    bool writeNetworkToGraphML(const std::string& filepath,
                              const NetworkTopologyData& data);
    
    // Write memory data to JSON
    bool writeMemoryToJSON(const std::string& filepath,
                         const MemoryData& data);
    
    // Write neuromodulation data to CSV
    bool writeNeuromodulationToCSV(const std::string& filepath,
                                  const NeuromodulationData& data);
    
    // Write spike data to text format
    bool writeSpikesToText(const std::string& filepath,
                         const std::vector<DetailedSpikeEvent>& spikes);
    
    // Write debug data to JSON
    bool writeDebugData(const std::string& filepath,
                      const DebugTools::PerformanceProfiler::PerformanceStats& stats);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Web visualization generator for interactive exploration
class WebVisualizationGenerator {
public:
    WebVisualizationGenerator();
    ~WebVisualizationGenerator();
    
    // Generate complete web visualization
    bool generateWebVisualization(const std::string& outputDir,
                                  const NeuralActivityData& neuralData,
                                  const NetworkTopologyData& networkData,
                                  const MemoryData& memoryData,
                                  const NeuromodulationData& neuromodData,
                                  const std::vector<DetailedSpikeEvent>& spikes);
    
    // Generate interactive HTML page
    bool generateInteractiveHTML(const std::string& outputFile,
                               const std::string& title = "NLM Visualization");
    
    // Generate JavaScript visualization code
    std::string generateJavaScript(const NeuralActivityData& neuralData,
                                   const NetworkTopologyData& networkData,
                                   const MemoryData& memoryData,
                                   const NeuromodulationData& neuromodData,
                                   const std::vector<DetailedSpikeEvent>& spikes);
    
    // Generate D3.js network visualization
    std::string generateD3NetworkVisualization(const NetworkTopologyData& data);
    
    // Generate Chart.js time series visualization
    std::string generateChartTimeSeries(const NeuralActivityData& data);
    
    // Generate memory visualization
    std::string generateMemoryVisualization(const MemoryData& data);
    
    // Generate neuromodulation visualization
    std::string generateNeuromodulationVisualization(const NeuromodulationData& data);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Integration framework for real-time data updates
class VisualizationIntegration {
public:
    VisualizationIntegration(std::shared_ptr<Brain> brain,
                           std::shared_ptr<SimulationClock> clock,
                           std::shared_ptr<ExperimentRunner> experimentRunner,
                           std::shared_ptr<CheckpointManager> checkpointManager);
    ~VisualizationIntegration();
    
    // Initialize all visualizers
    bool initializeVisualization(int width, int height);
    
    // Update all visualizations with current brain state
    void updateVisualizations(SimulationStep step);
    
    // Render all visualizations
    void renderVisualizations();
    
    // Get individual visualizers
    NeuralActivityVisualizer& getNeuralActivityVisualizer() { return *neuralActivityVisualizer_; }
    NetworkTopologyVisualizer& getNetworkTopologyVisualizer() { return *networkTopologyVisualizer_; }
    MemorySystemVisualizer& getMemorySystemVisualizer() { return *memorySystemVisualizer_; }
    NeuromodulationVisualizer& getNeuromodulationVisualizer() { return *neuromodulationVisualizer_; }
    DebugTools& getDebugTools() { return *debugTools_; }
    
    // Export visualization data
    bool exportAllData(const std::string& outputDir, SimulationStep step);
    
    // Generate web visualization
    bool generateWebVisualization(const std::string& outputDir, SimulationStep step);
    
    // Close all visualizations
    void closeVisualizations();
    
    // Check if any visualization is active
    bool isVisualizationActive() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimulationClock> clock_;
    std::shared_ptr<ExperimentRunner> experimentRunner_;
    std::shared_ptr<CheckpointManager> checkpointManager_;
    
    std::unique_ptr<NeuralActivityVisualizer> neuralActivityVisualizer_;
    std::unique_ptr<NetworkTopologyVisualizer> networkTopologyVisualizer_;
    std::unique_ptr<MemorySystemVisualizer> memorySystemVisualizer_;
    std::unique_ptr<NeuromodulationVisualizer> neuromodulationVisualizer_;
    std::unique_ptr<DebugTools> debugTools_;
    std::unique_ptr<VisualizationOutputWriter> outputWriter_;
    std::unique_ptr<WebVisualizationGenerator> webGenerator_;
    
    std::mutex updateMutex_;
    bool initialized_;
    
    // Internal data collection methods
    NeuralActivityData collectNeuralActivityData(SimulationStep step) const;
    NetworkTopologyData collectNetworkTopologyData(SimulationStep step) const;
    MemoryData collectMemoryData(SimulationStep step) const;
    NeuromodulationData collectNeuromodulationData(SimulationStep step) const;
    
    // Real-time console visualization
    void renderRealTimeConsole(const NeuralActivityData& neuralData,
                             const NetworkTopologyData& networkData,
                             const MemoryData& memoryData,
                             const NeuromodulationData& neuromodData);
};

} // namespace nlm

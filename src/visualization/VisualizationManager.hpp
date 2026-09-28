#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class Synapse;

// Visualization output formats
enum class VisualizationFormat {
    TEXT,
    CSV,
    JSON,
    GRAPHML,
    SVG,
    BINARY
};

// Debug levels
enum class DebugLevel {
    NONE,
    INFO,
    WARNING,
    ERROR,
    DETAILED
};

// Brain state snapshot for debugging
struct BrainSnapshot {
    std::chrono::system_clock::time_point timestamp;
    double timestep;
    size_t step;
    std::vector<float> neuronMembranePotentials;
    std::vector<float> neuronActivations;
    std::vector<float> synapticWeights;
    size_t totalSpikeCount;
    size_t activeNeuronCount;
    float averageFiringRate;
    std::map<std::string, float> neuromodulatorLevels;
    std::string developmentalStage;
};

// Visualization interface
class VisualizationSystem {
public:
    virtual ~VisualizationSystem() = default;
    virtual void initialize(std::shared_ptr<Brain> brain) = 0;
    virtual void update(double timestep, size_t step) = 0;
    virtual void render() = 0;
    virtual void saveToFile(const std::string& filename, VisualizationFormat format) = 0;
    virtual void setDebugLevel(DebugLevel level) = 0;
    virtual DebugLevel getDebugLevel() const = 0;
};

// Text-based visualization
class TextVisualizer : public VisualizationSystem {
public:
    void initialize(std::shared_ptr<Brain> brain) override;
    void update(double timestep, size_t step) override;
    void render() override;
    void saveToFile(const std::string& filename, VisualizationFormat format) override;
    void setDebugLevel(DebugLevel level) override;
    DebugLevel getDebugLevel() const override;

    // Text-specific options
    void setShowNeurons(bool show) { showNeurons_ = show; }
    void setShowSynapses(bool show) { showSynapses_ = show; }
    void setShowSpikes(bool show) { showSpikes_ = show; }
    void setMaxNeuronsPerRegion(size_t max) { maxNeuronsPerRegion_ = max; }
    void setNeuronScale(float scale) { neuronScale_ = scale; }

private:
    std::shared_ptr<Brain> brain_;
    DebugLevel debugLevel_;
    bool showNeurons_;
    bool showSynapses_;
    bool showSpikes_;
    size_t maxNeuronsPerRegion_;
    float neuronScale_;
    std::vector<BrainSnapshot> history_;
    size_t currentStep_;
    double currentTime_;
};

// CSV data exporter
class CSVExporter {
public:
    static bool exportBrainState(std::shared_ptr<Brain> brain, const std::string& filename);
    static bool exportSimulationResults(const std::vector<BrainSnapshot>& history, const std::string& filename);
    static bool exportNeurons(std::shared_ptr<Brain> brain, const std::string& filename);
    static bool exportSynapses(std::shared_ptr<Brain> brain, const std::string& filename);
    static bool exportSpikes(std::shared_ptr<Brain> brain, const std::string& filename);

    static void setPrecision(int precision) { precision_ = precision; }
    static int getPrecision() { return precision_; }

private:
    static int precision_;
    static std::ofstream createOutputFile(const std::string& filename);
};

// JSON exporter
class JSONExporter {
public:
    static bool exportBrainState(std::shared_ptr<Brain> brain, const std::string& filename);
    static bool exportNetworkTopology(std::shared_ptr<Brain> brain, const std::string& filename);
    static bool exportSimulationData(const std::vector<BrainSnapshot>& history, const std::string& filename);
    static bool exportNeuromodulation(std::shared_ptr<Brain> brain, const std::string& filename);

    static void setPrettyPrint(bool pretty) { prettyPrint_ = pretty; }
    static bool getPrettyPrint() { return prettyPrint_; }

private:
    static bool prettyPrint_;
    static std::string brainStateToJSON(std::shared_ptr<Brain> brain);
    static std::string networkTopologyToJSON(std::shared_ptr<Brain> brain);
};

// Neural network graph visualizer
class GraphVisualizer {
public:
    static bool exportGraphML(std::shared_ptr<Brain> brain, const std::string& filename);
    static bool exportD3Data(std::shared_ptr<Brain> brain, const std::string& filename);
    static bool exportSVG(std::shared_ptr<Brain> brain, const std::string& filename);

    static void setGraphLayout(const std::string& layout) { graphLayout_ = layout; }
    static std::string getGraphLayout() { return graphLayout_; }

private:
    static std::string graphLayout_;
    static std::string graphMLToString(std::shared_ptr<Brain> brain);
    static std::string d3DataToString(std::shared_ptr<Brain> brain);
    static std::string svgToString(std::shared_ptr<Brain> brain);
};

// Debugger class for detailed neural inspection
class NeuralDebugger {
public:
    void initialize(std::shared_ptr<Brain> brain);
    void step(double timestep, size_t step);
    void setAutoSave(bool autoSave) { autoSave_ = autoSave; }
    bool getAutoSave() const { return autoSave_; }
    void setSaveInterval(size_t interval) { saveInterval_ = interval; }
    size_t getSaveInterval() const { return saveInterval_; }

    // Debug functions
    void printNeuronInfo(size_t neuronId) const;
    void printSynapseInfo(size_t synapseId) const;
    void printRegionInfo(size_t regionId) const;
    void printNetworkStats() const;
    void printPlasticityStats() const;
    void printNeuromodulationStats() const;

    void exportNeuronData(size_t neuronId, const std::string& filename);
    void exportSynapseData(size_t synapseId, const std::string& filename);
    void exportRegionData(size_t regionId, const std::string& filename);
    void exportNetworkData(const std::string& filename);

    std::vector<BrainSnapshot> getHistory() const { return history_; }
    BrainSnapshot getCurrentSnapshot() const { return currentSnapshot_; }

private:
    std::shared_ptr<Brain> brain_;
    bool autoSave_;
    size_t saveInterval_;
    std::vector<BrainSnapshot> history_;
    BrainSnapshot currentSnapshot_;
    size_t stepsSinceLastSave_;
};

// Performance profiler
class PerformanceProfiler {
public:
    struct PerformanceMetrics {
        double cpuTime;
        double memoryUsage;
        size_t neuronCount;
        size_t synapseCount;
        size_t spikeCount;
        double averageFiringRate;
        double plasticityRate;
        double learningRate;
        size_t totalSteps;
        double simulationTime;
    };

    static void initialize(std::shared_ptr<Brain> brain);
    static void startStep(size_t step);
    static void endStep();
    static PerformanceMetrics getCurrentMetrics() { return currentMetrics_; }
    static std::vector<PerformanceMetrics> getHistory() { return history_; }
    static void exportMetrics(const std::string& filename);
    static void reset();

private:
    static PerformanceMetrics currentMetrics_;
    static std::vector<PerformanceMetrics> history_;
    static std::chrono::high_resolution_clock::time_point stepStartTime_;
    static std::shared_ptr<Brain> brain_;
};

// High-level visualization manager
class VisualizationManager {
public:
    static VisualizationManager& getInstance() {
        static VisualizationManager instance;
        return instance;
    }

    void initialize(std::shared_ptr<Brain> brain);
    void shutdown();
    void update(double timestep, size_t step);
    void render();
    void saveState(const std::string& filename);
    void loadState(const std::string& filename);

    void setFormat(VisualizationFormat format) { format_ = format; }
    VisualizationFormat getFormat() const { return format_; }

    void setDebugLevel(DebugLevel level);
    DebugLevel getDebugLevel() const;

    // Convenience functions
    void exportCSV(const std::string& filename) { CSVExporter::exportBrainState(brain_, filename); }
    void exportJSON(const std::string& filename) { JSONExporter::exportBrainState(brain_, filename); }
    void exportGraphML(const std::string& filename) { GraphVisualizer::exportGraphML(brain_, filename); }
    void exportSVG(const std::string& filename) { GraphVisualizer::exportSVG(brain_, filename); }

    void printStats() const;
    void printNetworkTopology() const;
    void printNeuronInfo(size_t neuronId) const;
    void printSynapseInfo(size_t synapseId) const;

private:
    VisualizationManager() = default;
    ~VisualizationManager() = default;
    VisualizationManager(const VisualizationManager&) = delete;
    VisualizationManager& operator=(const VisualizationManager&) = delete;

    std::shared_ptr<Brain> brain_;
    std::unique_ptr<VisualizationSystem> visualizer_;
    std::unique_ptr<NeuralDebugger> debugger_;
    std::unique_ptr<PerformanceProfiler> profiler_;
    VisualizationFormat format_;
    DebugLevel debugLevel_;
};

} // namespace nlm

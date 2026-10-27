// Advanced Visualization and Debugging Tools for NLM
// Comprehensive visualization and debugging system for neural simulation

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>
#include <queue>

using namespace nlm;

// Visualization output formats
enum class OutputFormat {
    ASCII,
    ANSI_COLOR,
    HTML,
    JSON,
    CSV,
    SVG,
    WEBGL,
    THREEJS,
    TERMINAL,
    CONSOLE,
    FILE
};

// Visualization types
enum class VisualizationType {
    NEURAL_NETWORK,
    SPIKE_RATIOS,
    CONNECTION_MAP,
    BRAIN_REGIONS,
    MEMORY_ACTIVITY,
    NEUROMODULATION,
    DEVELOPMENT_PROGRESS,
    PERFORMANCE_METRICS,
    TIME_SERIES,
    COMPARISON_PLOTS,
    3D_BRAIN_MODEL,
    NEURON_STATES,
    SYNAPSE_WEIGHTS,
    ATTENTION_MAPS,
    PREDICTION_RESULTS,
    SOCIAL_INTERACTION
};

// Debug levels
enum class DebugLevel {
    OFF,
    BASIC,
    DETAILED,
    EXTENSIVE,
    REAL_TIME
};

// Visualization data structures
struct VisualizationData {
    std::string experimentName;
    double timestamp;
    VisualizationType type;
    std::map<std::string, std::vector<double>> metrics;
    std::vector<std::vector<double>> matrixData;
    std::vector<std::string> labels;
    std::map<std::string, std::string> metadata;
    std::string title;
    std::string description;
};

// Visualization options
struct VisualizationOptions {
    OutputFormat outputFormat;
    std::string outputFile;
    DebugLevel debugLevel;
    bool showHelp;
    bool showTimestamps;
    bool showLegend;
    bool animated;
    double refreshRate;
    size_t width;
    size_t height;
    std::string theme;
    bool autoSave;
    size_t maxHistory;
    bool enableInteractivity;
    bool enableFiltering;
    std::map<std::string, std::string> customSettings;
};

// Base visualization class
class BaseVisualizer {
public:
    virtual ~BaseVisualizer() = default;
    
    virtual void initialize(const VisualizationOptions& options) = 0;
    virtual void update(const VisualizationData& data) = 0;
    virtual void render() = 0;
    virtual void save(const std::string& filename) = 0;
    virtual void close() = 0;
    
    virtual void setData(const std::string& key, const std::vector<double>& data) = 0;
    virtual void setMatrixData(const std::vector<std::vector<double>>& matrix) = 0;
    virtual void setLabels(const std::vector<std::string>& labels) = 0;
    
    virtual std::string getTypeName() const = 0;
    virtual std::string getVersion() const = 0;
};

// ASCII/Console Visualizer
class ASCIIVisualizer : public BaseVisualizer {
public:
    ASCIIVisualizer();
    ~ASCIIVisualizer() override;

    void initialize(const VisualizationOptions& options) override;
    void update(const VisualizationData& data) override;
    void render() override;
    void save(const std::string& filename) override;
    void close() override;

    void setData(const std::string& key, const std::vector<double>& data) override;
    void setMatrixData(const std::vector<std::vector<double>>& matrix) override;
    void setLabels(const std::vector<std::string>& labels) override;

    std::string getTypeName() const override { return "ASCII Visualizer"; }
    std::string getVersion() const override { return "1.0.0"; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void renderNeuralNetwork() const;
    void renderSpikeRates() const;
    void renderConnectionMap() const;
    void renderMatrixData() const;
    void renderTimeSeries() const;
    std::string formatNumber(double value, size_t precision = 2) const;
};

// ANSI Color Visualizer
class ANSIColorVisualizer : public BaseVisualizer {
public:
    ANSIColorVisualizer();
    ~ANSIColorVisualizer() override;

    void initialize(const VisualizationOptions& options) override;
    void update(const VisualizationData& data) override;
    void render() override;
    void save(const std::string& filename) override;
    void close() override;

    void setData(const std::string& key, const std::vector<double>& data) override;
    void setMatrixData(const std::vector<std::vector<double>>& matrix) override;
    void setLabels(const std::vector<std::string>& labels) override;

    std::string getTypeName() const override { return "ANSI Color Visualizer"; }
    std::string getVersion() const override { return "1.0.0"; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void renderColorGradient(double value, double min, double max) const;
    std::string getColorForValue(double value, double min, double max) const;
};

// HTML/JSON Visualizer
class HTMLVisualizer : public BaseVisualizer {
public:
    HTMLVisualizer();
    ~HTMLVisualizer() override;

    void initialize(const VisualizationOptions& options) override;
    void update(const VisualizationData& data) override;
    void render() override;
    void save(const std::string& filename) override;
    void close() override;

    void setData(const std::string& key, const std::vector<double>& data) override;
    void setMatrixData(const std::vector<std::vector<double>>& matrix) override;
    void setLabels(const std::vector<std::string>& labels) override;

    std::string getTypeName() const override { return "HTML Visualizer"; }
    std::string getVersion() const override { return "1.0.0"; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void generateHTML() const;
    void generateJavaScript() const;
    void generateCSS() const;
    std::string escapeHTML(const std::string& text) const;
};

// SVG Visualizer
class SVGVisualizer : public BaseVisualizer {
public:
    SVGVisualizer();
    ~SVGVisualizer() override;

    void initialize(const VisualizationOptions& options) override;
    void update(const VisualizationData& data) override;
    void render() override;
    void save(const std::string& filename) override;
    void close() override;

    void setData(const std::string& key, const std::vector<double>& data) override;
    void setMatrixData(const std::vector<std::vector<double>>& matrix) override;
    void setLabels(const std::vector<std::string>& labels) override;

    std::string getTypeName() const override { return "SVG Visualizer"; }
    std::string getVersion() const override { return "1.0.0"; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void generateSVG() const;
    void generatePathData(const std::vector<double>& data, double min, double max) const;
    double normalizeValue(double value, double min, double max, double svgWidth) const;
};

// Debugging tools
class DebugTools {
public:
    DebugTools();
    ~DebugTools();

    // Memory debugging
    struct MemoryStats {
        size_t totalAllocated;
        size_t currentAllocated;
        size_t peakAllocated;
        size_t allocationCount;
        double averageAllocationSize;
    };

    MemoryStats getMemoryStats() const;
    void printMemoryReport() const;

    // Performance debugging
    struct PerformanceStats {
        double cpuTime;
        double wallClockTime;
        size_t stepsPerSecond;
        double memoryUsageMB;
        double efficiency;
    };

    PerformanceStats getPerformanceStats() const;
    void printPerformanceReport() const;

    // Neural state debugging
    void dumpBrainState(const std::string& filename) const;
    void dumpNeuronStates(const std::string& filename) const;
    void dumpSynapseWeights(const std::string& filename) const;
    void dumpMemoryStates(const std::string& filename) const;

    // Debug visualization
    void createDebugVisualization(const VisualizationData& data) const;

    // Configuration debugging
    void printConfiguration() const;
    void validateConfiguration() const;

    // Error debugging
    void captureErrorStack() const;
    void generateDebugReport(const std::string& filename) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void updateMemoryStats();
    void updatePerformanceStats();
};

// Visualization manager
class VisualizationManager {
public:
    VisualizationManager();
    ~VisualizationManager();

    // Register visualizer
    void registerVisualizer(std::shared_ptr<BaseVisualizer> visualizer);
    std::shared_ptr<BaseVisualizer> getVisualizer(const std::string& type);

    // Visualization management
    void setCurrentVisualizer(const std::string& type);
    std::string getCurrentVisualizerType() const;

    // Update all visualizers
    void updateAllVisualizers(const VisualizationData& data);
    void renderAllVisualizers();
    void saveAllVisualizers(const std::string& prefix);

    // Configuration
    void setOptions(const VisualizationOptions& options);
    VisualizationOptions getOptions() const;

    // Debug mode
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;

    // Performance
    void setRenderInterval(double interval);
    double getRenderInterval() const;

    // Debug tools access
    DebugTools& getDebugTools();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void updateDebugInfo();
    void processQueue() const;
};

// Convenience functions
namespace Visualization {
    // Create visualizers
    std::shared_ptr<BaseVisualizer> createASCIIVisualizer();
    std::shared_ptr<BaseVisualizer> createANSIColorVisualizer();
    std::shared_ptr<BaseVisualizer> createHTMLVisualizer();
    std::shared_ptr<BaseVisualizer> createSVGVisualizer();

    // Get visualization manager
    VisualizationManager& getManager();

    // Configuration
    void configure(VisualizationOptions options);
    VisualizationOptions getConfiguration();

    // Debug mode
    void enableDebugMode(bool enable);
    void setDebugLevel(DebugLevel level);

    // Output
    void saveAll(const std::string& directory);
    void saveCurrent(const std::string& filename);
    void exportData(const std::string& filename, const std::string& format = "csv");

    // Debug tools
    DebugTools& getDebugTools();

    // Utility functions
    std::string getSystemInfo();
    double getCurrentMemoryUsageMB();
    size_t getCPUCoreCount();
}

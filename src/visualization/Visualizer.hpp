#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <functional>

namespace nlm {

// Visualization formats
enum class VisualizationFormat {
    Text,           // Plain text output
    JSON,           // JSON format for programmatic consumption
    CSV,            // Comma-separated values for spreadsheet analysis
    HTML,           // HTML with embedded plots
    PNG,            // PNG image files
    SVG,            // Scalable vector graphics
    WebGL,          // WebGL for interactive web visualizations
    PDF,            // PDF reports
    Markdown        // Markdown with embedded visualizations
};

// Plot types
enum class PlotType {
    NeuralActivity,    // Neural firing rate over time
    SynapticWeights,   // Synaptic weight distributions
    MemoryTraces,     // Memory content evolution
    Neuromodulation,   // Neuromodulator levels over time
    Development,       // Developmental stage changes
    Performance,       // Performance metrics
    NetworkTopology,   // Network connectivity visualization
    Comparative,       // Comparison between different runs
    Custom             // User-defined plot
};

// Visualization data point
struct VisualizationDataPoint {
    double timestamp;
    double value;
    std::string label;
    std::map<std::string, std::string> metadata;
    
    VisualizationDataPoint() : timestamp(0.0), value(0.0) {}
    VisualizationDataPoint(double ts, double val, const std::string& lbl = "")
        : timestamp(ts), value(val), label(lbl) {}
};

// Visualization configuration
struct VisualizationConfig {
    std::string title;
    std::string xAxisLabel;
    std::string yAxisLabel;
    PlotType plotType;
    VisualizationFormat format;
    double xMin;
    double xMax;
    double yMin;
    double yMax;
    std::string filePath;
    std::map<std::string, std::string> options;
    
    VisualizationConfig() : plotType(PlotType::NeuralActivity), format(VisualizationFormat::Text),
                           xMin(0.0), xMax(100.0), yMin(0.0), yMax(100.0) {}
};

// Base visualization interface
class IVisualizer {
public:
    virtual ~IVisualizer() = default;
    
    virtual bool initialize(const VisualizationConfig& config) = 0;
    virtual bool update(const std::vector<VisualizationDataPoint>& data) = 0;
    virtual bool render() = 0;
    virtual bool save() = 0;
    virtual void close() = 0;
    
    // Configuration methods
    virtual void setOutputPath(const std::string& path) = 0;
    virtual void setFormat(VisualizationFormat format) = 0;
    virtual void setDimensions(int width, int height) = 0;
    virtual void setTitle(const std::string& title) = 0;
    
    // Data management
    virtual void clearData() = 0;
    virtual bool addDataSeries(const std::string& name, const std::vector<VisualizationDataPoint>& data) = 0;
    virtual std::vector<std::string> getAvailableSeries() const = 0;
    
    // Interaction
    virtual bool isInteractive() const = 0;
    virtual void setInteractionCallback(const std::function<void(double)>& callback) = 0;
};

// Text visualization for console output
class TextVisualizer : public IVisualizer {
public:
    TextVisualizer();
    ~TextVisualizer() override;
    
    bool initialize(const VisualizationConfig& config) override;
    bool update(const std::vector<VisualizationDataPoint>& data) override;
    bool render() override;
    bool save() override;
    void close() override;
    
    // Text visualizer specific methods
    void setMaxYAxisValue(double max) { maxYAxisValue = max; }
    void setIncludeGrid(bool include) { includeGrid = include; }
    void setShowDataPoints(bool show) { showDataPoints = show; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    double maxYAxisValue;
    bool includeGrid;
    bool showDataPoints;
};

// JSON visualization for programmatic output
class JSONVisualizer : public IVisualizer {
public:
    JSONVisualizer();
    ~JSONVisualizer() override;
    
    bool initialize(const VisualizationConfig& config) override;
    bool update(const std::vector<VisualizationDataPoint>& data) override;
    bool render() override;
    bool save() override;
    void close() override;
    
    // JSON visualizer specific methods
    void setPrettyPrint(bool pretty) { prettyPrint = pretty; }
    void setIncludeMetadata(bool include) { includeMetadata = include; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool prettyPrint;
    bool includeMetadata;
};

// Visualization manager for coordinating multiple visualizations
class VisualizationManager {
public:
    VisualizationManager();
    ~VisualizationManager();
    
    // Add a visualization
    bool addVisualizer(const std::string& name, std::unique_ptr<IVisualizer> visualizer);
    
    // Remove a visualization
    bool removeVisualizer(const std::string& name);
    
    // Update all visualizations
    bool updateAll(const std::map<std::string, std::vector<VisualizationDataPoint>>& data);
    
    // Render all visualizations
    bool renderAll();
    
    // Save all visualizations
    bool saveAll(const std::string& outputDirectory);
    
    // Configuration
    bool setGlobalConfig(const VisualizationConfig& config);
    VisualizationConfig getGlobalConfig() const;
    
    // Data management
    void clearAllData();
    bool addDataSeries(const std::string& visualizerName, const std::string& seriesName, 
                      const std::vector<VisualizationDataPoint>& data);
    
    // Statistics and metadata
    size_t getVisualizerCount() const;
    std::vector<std::string> getVisualizerNames() const;
    std::string generateReport() const;
    
    // Performance monitoring
    double getRenderingTime() const;
    size_t getTotalDataPointsRendered() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Common visualization helpers
namespace VisualizationHelpers {
    std::string generateUniqueID();
    std::vector<VisualizationDataPoint> smoothData(const std::vector<VisualizationDataPoint>& data, int windowSize);
    std::vector<VisualizationDataPoint> normalizeData(const std::vector<VisualizationDataPoint>& data, double targetMax);
    std::string dataPointToString(const VisualizationDataPoint& point);
    bool saveTextVisualization(const std::string& filepath, const std::vector<VisualizationDataPoint>& data, 
                              const VisualizationConfig& config);
    bool saveJSONVisualization(const std::string& filepath, const std::map<std::string, std::vector<VisualizationDataPoint>>& data, 
                              const VisualizationConfig& config);
}

} // namespace nlm

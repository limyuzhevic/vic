#pragma once

#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/AdvancedConfig.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <mutex>
#include <variant>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <map>
#include <deque>

namespace nlm {

// Advanced visualization types
using VisualizationData = std::variant<
    std::vector<double>,
    std::vector<std::vector<double>>,
    std::vector<std::pair<double, double>>,
    std::unordered_map<std::string, double>,
    std::vector<float>,
    std::vector<std::vector<float>>
>;

using PlotType = enum class PlotType {
    LinePlot,
    ScatterPlot,
    Heatmap,
    NetworkGraph,
    TimeSeries,
    2DHistogram,
    3DPlot,
    ContourPlot,
    StreamPlot,
    PolarPlot,
    BoxPlot,
    ViolinPlot,
    Histogram,
    BarChart,
    StackedBar,
    PieChart,
    SurfacePlot,
    VolumeRendering
};

using ColorScheme = enum class ColorScheme {
    Viridis,
    Plasma,
    Inferno,
    Magma,
    Cividis,
    Sequential,
    Diverging,
    Categorical,
    ColorblindFriendly,
    Scientific,
    Custom
};

using InteractionMode = enum class InteractionMode {
    None,
    Pan,
    Zoom,
    Rotate,
    Select,
    Draw,
    Measure,
    Annotate,
    Filter
};

using ExportFormat = enum class ExportFormat {
    PNG,
    JPEG,
    SVG,
    PDF,
    EPS,
    HTML,
    WebGL,
    JSON,
    CSV,
    MATLAB,
    OCTAVE,
    ROOT,
    Custom
};

// Advanced visualization configuration
struct VisualizationConfig {
    std::string name;
    std::string title;
    std::string xLabel;
    std::string yLabel;
    std::string zLabel;
    PlotType plotType;
    ColorScheme colorScheme;
    double width;
    double height;
    double dpi;
    bool showGrid;
    bool showLegend;
    bool showToolbar;
    bool showAxisLabels;
    bool showTooltip;
    double alpha;
    double lineWidth;
    std::string fontFamily;
    int fontSize;
    bool antialiasing;
    std::unordered_map<std::string, std::string> customStyles;
    
    VisualizationConfig() : width(800), height(600), dpi(150), showGrid(true),
                           showLegend(true), showToolbar(true), showAxisLabels(true),
                           showTooltip(true), alpha(1.0), lineWidth(2.0), fontSize(12),
                           antialiasing(true) {}
};

// Scientific visualization data container
struct ScientificData {
    std::string name;
    std::vector<double> values;
    std::vector<double> timestamps;
    std::vector<std::pair<double, double>> coordinates;
    std::vector<std::vector<double>> matrix;
    std::vector<std::string> metadata;
    std::unordered_map<std::string, std::string> properties;
    
    ScientificData() = default;
    ScientificData(const std::string& n, const std::vector<double>& v) : name(n), values(v) {}
    ScientificData(const std::string& n, const std::vector<std::pair<double, double>>& c) : name(n), coordinates(c) {}
};

// Interactive visualization state
struct VisualizationState {
    std::chrono::system_clock::time_point lastUpdate;
    double currentZoom;
    std::pair<double, double> panOffset;
    double rotationAngle;
    std::vector<std::string> selectedData;
    std::unordered_map<std::string, std::string> annotations;
    std::vector<std::pair<double, double>> measurements;
    bool isAnimating;
    double animationSpeed;
    std::string interactionMode;
    
    VisualizationState() : currentZoom(1.0), panOffset({0.0, 0.0}), rotationAngle(0.0),
                          isAnimating(false), animationSpeed(1.0), interactionMode("none") {}
};

// Scientific visualization filter
struct VisualizationFilter {
    std::string name;
    std::function<bool(const ScientificData&)> filterFunction;
    std::string filterType; // "range", "threshold", "pattern", "statistical"
    double minValue;
    double maxValue;
    std::string units;
    bool isActive;
    
    VisualizationFilter() : minValue(0.0), maxValue(0.0), isActive(true) {}
};

// Advanced visualization tool with real-time updates, scientific analysis, and export capabilities
class AdvancedVisualization {
public:
    AdvancedVisualization(const std::string& name);
    AdvancedVisualization(const VisualizationConfig& config);
    ~AdvancedVisualization();
    
    // Configuration management
    void setConfig(const VisualizationConfig& config);
    const VisualizationConfig& getConfig() const;
    void updateConfig(const std::string& key, const std::string& value);
    
    // Data management
    void addData(const ScientificData& data);
    void updateData(const std::string& name, const ScientificData& data);
    bool hasData(const std::string& name) const;
    ScientificData getData(const std::string& name) const;
    const std::unordered_map<std::string, ScientificData>& getAllData() const;
    
    // Visualization generation
    std::string generatePlot(const std::string& dataName, PlotType plotType,
                           const std::vector<std::string>& options = {});
    std::string generateMultiPanelPlot(const std::vector<std::string>& dataNames,
                                      const std::vector<PlotType>& plotTypes,
                                      const std::string& layout = "grid");
    std::string generateAnimation(const std::string& dataName, double fps = 30.0,
                                 const std::string& outputFormat = "GIF");
    std::string generateInteractive3D(const std::string& dataName);
    std::string generateCustomVisualization(const std::string& script);
    
    // Scientific analysis
    std::string performStatisticalAnalysis(const std::string& dataName);
    std::string performSignalProcessing(const std::string& dataName,
                                       const std::string& processingType,
                                       const std::unordered_map<std::string, double>& params);
    std::string performFourierAnalysis(const std::string& dataName);
    std::string performWaveletAnalysis(const std::string& dataName);
    std::string performPatternRecognition(const std::string& dataName,
                                         const std::string& algorithm);
    
    // Data filtering and transformation
    void addFilter(const VisualizationFilter& filter);
    void removeFilter(const std::string& name);
    void updateFilter(const std::string& name, const VisualizationFilter& filter);
    ScientificData applyFilters(const std::string& dataName) const;
    ScientificData transformData(const std::string& dataName,
                                const std::string& transformationType,
                                const std::unordered_map<std::string, double>& params);
    
    // Export and import
    bool exportToFile(const std::string& filepath, ExportFormat format,
                     const std::unordered_map<std::string, std::string>& options = {});
    bool exportAnimation(const std::string& filepath, double fps, 
                        const std::string& format = "GIF");
    bool importFromFile(const std::string& filepath, const std::string& dataName);
    std::string generateWebPage(const std::string& outputFile, 
                               const std::string& title = "Visualization");
    
    // Real-time updates
    void updateDataFromSource(const std::string& dataName,
                             const std::function<ScientificData()>& dataSource);
    void setUpdateInterval(double intervalSeconds);
    double getUpdateInterval() const;
    void startRealTimeUpdates();
    void stopRealTimeUpdates();
    bool isRealTimeActive() const;
    
    // State management
    void setState(const VisualizationState& state);
    const VisualizationState& getState() const;
    void updateState(const std::string& key, const std::string& value);
    
    // Interaction handling
    void handleMouseClick(double x, double y);
    void handleMouseDrag(double dx, double dy);
    void handleMouseWheel(double delta);
    void handleKeyPress(const std::string& key);
    void selectData(const std::vector<std::string>& dataNames);
    std::vector<std::string> getSelectedData() const;
    
    // Analysis results
    struct AnalysisResult {
        std::string name;
        std::string type;
        std::string description;
        std::string result;
        double confidence;
        std::chrono::system_clock::time_point timestamp;
        std::unordered_map<std::string, double> metrics;
    };
    
    std::vector<AnalysisResult> getAnalysisResults() const;
    AnalysisResult getLatestAnalysis(const std::string& type) const;
    
    // Comparison and overlay
    void overlayVisualization(std::shared_ptr<AdvancedVisualization> otherViz,
                              const std::string& overlayType = "alpha");
    std::string compareVisualizations(const std::vector<std::string>& vizNames,
                                     const std::string& metric = "mse");
    
    // Performance monitoring
    struct PerformanceMetrics {
        double updateTime;
        double renderTime;
        double memoryUsage;
        double fps;
        size_t dataPointsProcessed;
        std::chrono::system_clock::time_point timestamp;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    std::string generatePerformanceReport() const;
    
    // Machine learning integration
    void trainModel(const std::string& dataName, const std::string& modelType,
                   const std::unordered_map<std::string, double>& hyperparameters);
    std::string predict(const std::string& modelName, const std::vector<double>& input);
    void retrainWithNewData(const std::string& modelName, const ScientificData& newData);
    
    // Calibration and validation
    std::string calibrateModel(const std::string& modelName, 
                              const std::vector<std::string>& validationDataNames);
    std::string validateModel(const std::string& modelName,
                             const std::string& validationType = "cross-validation");
    
    // Collaboration and sharing
    std::string generateShareLink(const std::string& title = "Visualization");
    bool loadFromShareLink(const std::string& shareLink);
    std::string exportForCollaboration(const std::string& format = "JSON");
    
    // Utility functions
    static std::string getAvailablePlotTypes();
    static std::string getAvailableColorSchemes();
    static std::string getAvailableExportFormats();
    static ScientificData generateRandomData(const std::string& name, size_t size,
                                           const std::string& distribution = "normal");
    static ScientificData generateTimeSeries(const std::string& name, 
                                            size_t length,
                                            const std::string& pattern = "sinusoidal");
    
    // Integration with existing NLM systems
    void visualizeBrainState(const class Brain& brain, const std::string& plotType = "network");
    void visualizeMemoryState(const class MemorySystem& memory, const std::string& plotType = "activity");
    void visualizeExperimentResults(const class Experiment& experiment);
    void visualizePerformanceMetrics(const std::vector<double>& metrics);
    
    // Batch processing
    std::vector<std::string> batchProcess(const std::vector<std::string>& dataNames,
                                         const std::string& processingType,
                                         const std::unordered_map<std::string, std::string>& options = {});
    
    // Error handling
    std::string getLastError() const;
    void clearErrors();
    bool hasErrors() const;
    
    // Version control and history
    std::string createSnapshot();
    bool restoreFromSnapshot(const std::string& snapshotId);
    std::vector<std::string> getSnapshotHistory() const;
    std::string getVersionInfo() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void validateConfiguration() const;
    void normalizeData(ScientificData& data) const;
    std::string generatePlotHTML(const std::string& plotData, const VisualizationConfig& config) const;
    void updatePerformanceMetrics();
    void cleanupOldData();
    static std::string getCurrentTimestamp();
    static double calculateMSE(const std::vector<double>& pred, const std::vector<double>& actual);
    static std::vector<double> applyFilter(const std::vector<double>& data,
                                         const VisualizationFilter& filter);
};

// Specialized visualization tools for different scientific domains
class ScientificVisualizer {
public:
    ScientificVisualizer();
    ~ScientificVisualizer();
    
    // Neuroscience visualization
    std::string visualizeNeurons(const class Brain& brain);
    std::string visualizeSynapses(const class Brain& brain);
    std::string visualizeSpikingActivity(const class Brain& brain, double timeWindow);
    std::string visualizeNetworkTopography(const class Brain& brain);
    
    // Memory visualization
    std::string visualizeWorkingMemory(const class MemorySystem& memory);
    std::string visualizeEpisodicMemory(const class MemorySystem& memory);
    std::string visualizeAssociativeMemory(const class MemorySystem& memory);
    std::string visualizeMemoryConsolidation(const class MemorySystem& memory);
    
    // Cognitive visualization
    std::string visualizeAttention(const class AttentionSystem& attention);
    std::string visualizePlanning(const class PlanningSystem& planning);
    std::string visualizeLearning(const class LearningSystem& learning);
    
    // Experiment visualization
    std::string visualizeExperimentProgress(const class Experiment& experiment);
    std::string visualizeMetricEvolution(const class Experiment& experiment,
                                        const std::string& metricName);
    std::string visualizeParameterSpace(const class Experiment& experiment);
    
    // Multi-scale visualization
    std::string visualizeMultipleScales(const std::vector<std::string>& dataNames,
                                       const std::vector<PlotType>& plotTypes);
    std::string visualizeHierarchicalData(const std::string& name,
                                         const std::unordered_map<std::string, std::string>& hierarchy);
    
    // 3D and spatial visualization
    std::string visualize3DNetwork(const class Brain& brain);
    std::string visualizeSpatialMemory(const class MemorySystem& memory);
    std::string visualizeEnvironmentalInteraction(const class Environment& environment);
    
    // Statistical visualization
    std::string visualizeStatisticalDistribution(const std::string& dataName,
                                                const std::string& distributionType);
    std::string visualizeHypothesisTest(const std::string& dataName1,
                                       const std::string& dataName2,
                                       const std::string& testType);
    std::string visualizeConfidenceIntervals(const std::vector<std::string>& dataNames);
    
    // Time-series visualization
    std::string visualizeTimeSeries(const std::string& dataName,
                                   const std::string& xAxis = "time",
                                   const std::string& yAxis = "value");
    std::string visualizeEventRelatedPotentials(const std::string& dataName);
    std::string visualizeOscillations(const std::string& dataName,
                                      const std::vector<std::string>& frequencyBands);
    
    // Export and publication
    std::string exportForPublication(const std::string& format = "PDF");
    std::string exportForPresentation(const std::string& format = "PowerPoint");
    std::string exportForWeb(const std::string& outputDir);
    std::string exportForPrint(const std::string& outputDir);
    
    // Quality assurance
    std::string validateVisualizationQuality(const std::string& dataName);
    std::string checkForDataIssues(const std::string& dataName);
    std::string generateQualityReport();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    std::string generateNetworkVisualization(const class Brain& brain);
    std::string generateActivityHeatmap(const class Brain& brain);
    std::string generateSpikeRasterPlot(const class Brain& brain, double timeWindow);
    std::string generateMemoryTrace(const class MemorySystem& memory);
};

// Real-time monitoring and visualization dashboard
class VisualizationDashboard {
public:
    VisualizationDashboard();
    ~VisualizationDashboard();
    
    // Dashboard configuration
    void addVisualization(std::shared_ptr<AdvancedVisualization> viz);
    void removeVisualization(const std::string& name);
    void updateVisualization(const std::string& name,
                            const ScientificData& data);
    std::shared_ptr<AdvancedVisualization> getVisualization(const std::string& name) const;
    
    // Layout management
    void setLayout(const std::string& layoutType, const std::vector<std::string>& vizNames);
    std::string getLayout() const;
    void autoArrange();
    
    // Dashboard controls
    void setAutoRefresh(bool enabled, double intervalSeconds = 1.0);
    bool isAutoRefreshEnabled() const;
    double getRefreshInterval() const;
    void setFilter(const std::string& filterName, const VisualizationFilter& filter);
    VisualizationFilter getFilter(const std::string& filterName) const;
    
    // Data synchronization
    void synchronizeData(const std::string& dataName,
                       const std::function<ScientificData()>& dataSource);
    void addDataSource(const std::string& name, const std::function<ScientificData()>& source);
    
    // Collaboration features
    void enableCollaboration(bool enabled);
    bool isCollaborationEnabled() const;
    std::string getCollaborationSessionId() const;
    void shareDashboard(const std::string& recipient);
    
    // Export and reporting
    std::string exportDashboard(const std::string& format = "HTML");
    std::string generateReport(const std::string& title = "Visualization Dashboard");
    std::string createInteractiveHTMLPage(const std::string& outputFile);
    
    // Performance monitoring
    struct DashboardPerformance {
        double updateTime;
        double renderTime;
        double memoryUsage;
        double fps;
        size_t activeVisualizations;
        std::chrono::system_clock::time_point timestamp;
    };
    
    DashboardPerformance getPerformanceMetrics() const;
    std::string getPerformanceReport() const;
    
    // Error handling and recovery
    std::string getDashboardStatus() const;
    std::vector<std::string> getActiveErrors() const;
    void clearErrors();
    
    // State management
    std::string saveState() const;
    bool restoreState(const std::string& stateData);
    std::string getStateHistory() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void updateAllVisualizations();
    void refreshData();
    void cleanupInactiveVisualizations();
    static std::string generateDashboardHTML(const VisualizationDashboard& dashboard);
    static std::string generateVisualizationHTML(std::shared_ptr<AdvancedVisualization> viz);
};

// Utility functions for visualization
namespace VisualizationUtils {
    // Data processing
    std::vector<double> normalizeData(const std::vector<double>& data);
    std::vector<double> denormalizeData(const std::vector<double>& data,
                                       double min, double max);
    std::vector<double> smoothData(const std::vector<double>& data,
                                  int windowSize = 5);
    std::vector<std::pair<double, double>> calculateRunningAverage(
        const std::vector<double>& data, int windowSize = 10);
    
    // Statistical analysis
    double calculateMean(const std::vector<double>& data);
    double calculateStddev(const std::vector<double>& data, double mean);
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    double calculateRegression(const std::vector<double>& x, const std::vector<double>& y);
    
    // Visualization generation (simplified)
    std::string generatePlotHTML(const std::string& data, const VisualizationConfig& config);
    std::string generateHeatmapHTML(const std::vector<std::vector<double>>& matrix,
                                   const VisualizationConfig& config);
    std::string generateNetworkHTML(const std::string& networkData,
                                   const VisualizationConfig& config);
    
    // File operations
    bool saveVisualization(const std::string& filepath, const std::string& data);
    bool loadVisualization(const std::string& filepath, std::string& data);
    
    // Color utilities
    std::string getColor(const std::string& scheme, int index, int maxIndex);
    std::string interpolateColor(const std::string& color1, const std::string& color2, double t);
    
    // Performance monitoring
    double getCurrentTimestamp();
    void logPerformance(const std::string& operation, double timeTaken);
    
    // Error handling
    std::string formatError(const std::string& operation, const std::string& error);
}

} // namespace nlm

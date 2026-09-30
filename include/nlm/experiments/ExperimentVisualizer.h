#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <functional>

namespace nlm {

/**
 * Experiment result visualization and plotting system
 * for generating visual representations of experiment results
 * using various plotting formats and interactive visualizations.
 */
class ExperimentVisualizer {
public:
    // Plot configuration
    struct PlotConfig {
        std::string title;
        std::string xLabel;
        std::string yLabel;
        std::string outputFile;
        std::string plotType; // line, scatter, bar, histogram
        std::string colorScheme;
        bool showLegend;
        bool showGrid;
        size_t width;
        size_t height;
        double lineWidth;
        
        PlotConfig() : width(800), height(600), lineWidth(2.0), 
                      showLegend(true), showGrid(true) {}
    };
    
    // Time series plot configuration
    struct TimeSeriesConfig {
        std::string metricName;
        std::string label;
        std::string color;
        std::string lineStyle; // solid, dashed, dotted
        bool visible;
        
        TimeSeriesConfig() : visible(true) {}
    };
    
    // Comparison plot configuration
    struct ComparisonConfig {
        std::string title;
        std::string xLabel;
        std::string yLabel;
        std::string outputFile;
        bool overlay;
        std::vector<TimeSeriesConfig> series;
        
        ComparisonConfig() : overlay(false) {}
    };
    
    // Heatmap configuration
    struct HeatmapConfig {
        std::string title;
        std::string outputFile;
        std::vector<std::vector<double>> data;
        std::vector<std::string> xLabels;
        std::vector<std::string> yLabels;
        std::string colormap;
        double minValue;
        double maxValue;
        
        HeatmapConfig() : minValue(0.0), maxValue(1.0), colormap("viridis") {}
    };
    
    // 3D plot configuration
    struct ThreeDPlotConfig {
        std::string title;
        std::string outputFile;
        std::vector<std::vector<std::vector<double>>> data;
        std::vector<std::string> xLabels;
        std::vector<std::string> yLabels;
        std::vector<std::string> zLabels;
        std::string colormap;
        
        ThreeDPlotConfig() : colormap("viridis") {}
    };
    
    ExperimentVisualizer();
    ~ExperimentVisualizer() = default;
    
    // Plot generation
    bool generateTimeSeriesPlot(const std::vector<double>& data,
                               const TimeSeriesConfig& config,
                               const PlotConfig& plotConfig);
    
    bool generateComparisonPlot(const std::map<std::string, std::vector<double>>& data,
                               const ComparisonConfig& config,
                               const PlotConfig& plotConfig);
    
    bool generateHistogramPlot(const std::vector<double>& data,
                              const PlotConfig& config,
                              const std::string& binLabel = "");
    
    bool generateScatterPlot(const std::vector<double>& x,
                            const std::vector<double>& y,
                            const PlotConfig& config,
                            const std::string& label = "");
    
    // Advanced visualization
    bool generateHeatmap(const HeatmapConfig& config);
    bool generate3DPlot(const ThreeDPlotConfig& config);
    bool generateContourPlot(const std::vector<std::vector<double>>& data,
                           const PlotConfig& config,
                           const std::vector<std::string>& xLabels,
                           const std::vector<std::string>& yLabels);
    
    // Result visualization
    bool visualizeResults(const std::string& metricName,
                        const std::vector<double>& values,
                        const std::string& outputFile,
                        const std::string& plotType = "histogram");
    
    bool visualizeResults(const std::map<std::string, std::vector<double>>& results,
                        const std::string& outputFile,
                        const std::string& plotType = "comparison");
    
    // Interactive visualization
    std::string generateInteractivePlot(const std::string& metricName,
                                        const std::vector<double>& values,
                                        const std::string& plotType = "interactive");
    
    // Report generation
    std::string generateAnalysisReport(const std::string& experimentName,
                                       const std::map<std::string, std::vector<double>>& results);
    
    // Configuration management
    void setDefaultPlotConfig(const PlotConfig& config);
    const PlotConfig& getDefaultPlotConfig() const;
    
    // File format support
    bool exportToPNG(const std::string& inputSVG, const std::string& outputPNG);
    bool exportToPDF(const std::string& inputSVG, const std::string& outputPDF);
    
    // Validation
    bool validateConfig(const PlotConfig& config) const;
    bool validateConfig(const ComparisonConfig& config) const;
    
    // Utility functions
    std::vector<double> calculateBinValues(const std::vector<double>& data, size_t binCount) const;
    std::string generateColorScheme(const std::string& schemeName, size_t colorCount) const;
    std::vector<double> normalizeData(const std::vector<double>& data, 
                                      double min = 0.0, double max = 1.0) const;
    
private:
    PlotConfig defaultPlotConfig;
    
    // File generation helpers
    std::string generateSVG(const std::string& plotData) const;
    bool savePlotData(const std::string& filepath, const std::string& data) const;
    
    // Plot generation helpers
    std::string generateLinePlotSVG(const std::vector<double>& data,
                                    const PlotConfig& config) const;
    std::string generateScatterPlotSVG(const std::vector<double>& x,
                                       const std::vector<double>& y,
                                       const PlotConfig& config) const;
    std::string generateHistogramSVG(const std::vector<double>& data,
                                     const PlotConfig& config) const;
};

} // namespace nlm
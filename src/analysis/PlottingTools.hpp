#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <array>
#include <cstdint>
#include <memory>

namespace nlm {

class PlottingTools {
public:
    struct PlotData {
        std::string name;
        std::vector<double> xData;
        std::vector<double> yData;
        std::string xLabel;
        std::string yLabel;
        std::string title;
        std::string fileFormat;
    };
    
    struct PlotConfig {
        double lineWidth;
        std::string lineColor;
        std::string backgroundColor;
        std::string gridColor;
        bool showGrid;
        bool showLegend;
        std::string fontFamily;
        int fontSize;
        std::map<std::string, std::string> customStyles;
    };
    
    struct HeatmapData {
        std::vector<std::vector<double>> data;
        std::vector<double> xLabels;
        std::vector<double> yLabels;
        std::string xLabel;
        std::string yLabel;
        std::string title;
        double minValue;
        double maxValue;
        std::string colormap;
    };
    
    struct TimeSeriesConfig {
        double samplingRate;
        size_t windowSize;
        std::string interpolationMethod;
        bool showTrend;
        bool showConfidenceBands;
    };
    
    static std::unique_ptr<PlottingTools> create() {
        return std::make_unique<PlottingTools>();
    }
    
    // Basic plotting functions
    std::string createLinePlot(const PlotData& data) const;
    std::string createScatterPlot(const PlotData& data) const;
    std::string createBarPlot(const PlotData& data) const;
    std::string createHistogram(const PlotData& data) const;
    
    // Advanced plotting functions
    std::string createHeatmap(const HeatmapData& data) const;
    std::string create2DScatterPlot(const PlotData& data, const PlotConfig& config) const;
    std::string create3DPlot(const PlotData& xData, const PlotData& yData, 
                             const PlotData& zData, const PlotConfig& config) const;
    
    // Time series analysis and plotting
    std::string createTimeSeriesPlot(const std::vector<PlotData>& timeSeries,
                                     const TimeSeriesConfig& config) const;
    std::string createMembranePotentialTrace(const std::vector<double>& trace,
                                             double samplingRate,
                                             const PlotConfig& config) const;
    std::string createSpikeRaster(const std::vector<double>& spikeTimes,
                                  double duration,
                                  const PlotConfig& config) const;
    
    // Network visualization
    std::string createNetworkGraph(const std::map<NeuronId, std::vector<NeuronId>>& connections,
                                  const PlotConfig& config) const;
    std::string createCommunityPlot(const std::vector<std::vector<NeuronId>>& communities,
                                    const PlotConfig& config) const;
    
    // Analysis visualizations
    std::string createCorrelationMatrix(const std::vector<std::vector<double>>& matrix,
                                        const PlotConfig& config) const;
    std::string createCentralityPlot(const std::map<NeuronId, double>& centralityValues,
                                     const PlotConfig& config) const;
    std::string createActivityProfile(const std::vector<double>& activityProfile,
                                      const PlotConfig& config) const;
    
    // Export and integration
    bool exportPlot(const std::string& filepath, const PlotData& data) const;
    bool exportHeatmap(const std::string& filepath, const HeatmapData& data) const;
    bool exportTimeSeries(const std::string& filepath, 
                         const std::vector<PlotData>& timeSeries) const;
    bool exportNetworkVisualization(const std::string& filepath,
                                    const std::map<NeuronId, std::vector<NeuronId>>& connections) const;
    
    // Utility functions
    std::vector<double> interpolateData(const std::vector<double>& x, 
                                        const std::vector<double>& y,
                                        size_t newSize) const;
    std::vector<double> smoothData(const std::vector<double>& data,
                                   int windowSize) const;
    std::vector<double> normalizeData(const std::vector<double>& data,
                                      double min = 0.0, double max = 1.0) const;
    
private:
    PlottingTools() = default;
    
    // Helper methods
    std::string generateHTMLPlot(const PlotData& data, const PlotConfig& config) const;
    std::string generateSVGPlot(const PlotData& data, const PlotConfig& config) const;
    std::string generateJavaScriptChart(const PlotData& data, const PlotConfig& config) const;
    std::vector<double> calculateMovingAverage(const std::vector<double>& data, 
                                               size_t windowSize) const;
    double calculateStandardDeviation(const std::vector<double>& data) const;
};

} // namespace nlm
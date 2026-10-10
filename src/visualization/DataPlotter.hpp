// Data Plotter Header
#pragma once

#include <string>
#include <vector>
#include <map>

namespace nlm {

/**
 * Options for data plotting
 */
struct PlotOptions {
    std::string xLabel;
    std::string yLabel;
    double lineWidth;
    double markerSize;
    bool showGrid;
    std::string legend;
    
    PlotOptions() : lineWidth(2.0), markerSize(5.0), showGrid(true), legend("") {}
};

/**
 * Data plotter for NLM
 */
class DataPlotter {
public:
    DataPlotter();
    ~DataPlotter();
    
    /**
     * Plot data with given options
     */
    void plotData(const std::string& title, const std::vector<double>& xData,
                 const std::vector<double>& yData, const std::string& filePath,
                 const PlotOptions& options = PlotOptions());
    
    /**
     * Plot multiple datasets
     */
    void plotMultiple(const std::vector<std::string>& titles,
                     const std::vector<std::vector<double>>& xData,
                     const std::vector<std::vector<double>>& yData,
                     const std::string& filePath, const PlotOptions& options = PlotOptions());
    
    /**
     * Plot time series data
     */
    void plotTimeSeries(const std::string& title, const std::map<double, double>& dataPoints,
                       const std::string& filePath, const PlotOptions& options = PlotOptions());
    
    /**
     * Plot histogram of data
     */
    void plotHistogram(const std::string& title, const std::vector<double>& data,
                       int bins, const std::string& filePath,
                       const PlotOptions& options = PlotOptions());
};

} // namespace nlm
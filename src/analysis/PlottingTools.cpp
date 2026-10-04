#include "PlottingTools.hpp"

namespace nlm {

std::string PlottingTools::createLinePlot(const PlotData& data) const {
    PlotConfig defaultConfig;
    return generateHTMLPlot(data, defaultConfig);
}

std::string PlottingTools::createScatterPlot(const PlotData& data) const {
    PlotConfig defaultConfig;
    return generateHTMLPlot(data, defaultConfig);
}

std::string PlottingTools::createBarPlot(const PlotData& data) const {
    PlotConfig defaultConfig;
    return generateHTMLPlot(data, defaultConfig);
}

std::string PlottingTools::createHistogram(const PlotData& data) const {
    PlotConfig defaultConfig;
    return generateHTMLPlot(data, defaultConfig);
}

std::string PlottingTools::createHeatmap(const HeatmapData& data) const {
    // Generate heatmap visualization
    return "<div>Heatmap visualization would be generated here</div>";
}

std::string PlottingTools::create2DScatterPlot(const PlotData& data, const PlotConfig& config) const {
    return generateHTMLPlot(data, config);
}

std::string PlottingTools::create3DPlot(const PlotData& xData, const PlotData& yData,
                                        const PlotData& zData, const PlotConfig& config) const {
    // Generate 3D plot visualization
    return "<div>3D plot visualization would be generated here</div>";
}

std::string PlottingTools::createTimeSeriesPlot(const std::vector<PlotData>& timeSeries,
                                                const TimeSeriesConfig& config) const {
    // Generate time series visualization
    return "<div>Time series visualization would be generated here</div>";
}

std::string PlottingTools::createMembranePotentialTrace(const std::vector<double>& trace,
                                                        double samplingRate,
                                                        const PlotConfig& config) const {
    PlotData data;
    data.name = "Membrane Potential Trace";
    data.xLabel = "Time (s)";
    data.yLabel = "Membrane Potential (mV)";
    data.title = "Membrane Potential Trace";
    data.fileFormat = "svg";
    
    // Convert time to x-values
    data.xData.resize(trace.size());
    for (size_t i = 0; i < trace.size(); ++i) {
        data.xData[i] = i / samplingRate;
    }
    
    data.yData = trace;
    
    return generateHTMLPlot(data, config);
}

std::string PlottingTools::createSpikeRaster(const std::vector<double>& spikeTimes,
                                             double duration,
                                             const PlotConfig& config) const {
    PlotData data;
    data.name = "Spike Raster Plot";
    data.xLabel = "Time (s)";
    data.yLabel = "Spike Count";
    data.title = "Spike Raster Plot";
    data.fileFormat = "svg";
    
    // Create spike raster visualization
    data.xData.resize(spikeTimes.size(), 0.0);
    data.yData.resize(spikeTimes.size(), 0.0);
    
    for (size_t i = 0; i < spikeTimes.size(); ++i) {
        data.xData[i] = spikeTimes[i];
        data.yData[i] = 1.0; // All spikes at same height for simplicity
    }
    
    return generateHTMLPlot(data, config);
}

std::string PlottingTools::createNetworkGraph(const std::map<NeuronId, std::vector<NeuronId>>& connections,
                                             const PlotConfig& config) const {
    // Generate network graph visualization
    return "<div>Network graph visualization would be generated here</div>";
}

std::string PlottingTools::createCommunityPlot(const std::vector<std::vector<NeuronId>>& communities,
                                               const PlotConfig& config) const {
    // Generate community visualization
    return "<div>Community plot visualization would be generated here</div>";
}

std::string PlottingTools::createCorrelationMatrix(const std::vector<std::vector<double>>& matrix,
                                                  const PlotConfig& config) const {
    // Generate correlation matrix visualization
    return "<div>Correlation matrix visualization would be generated here</div>";
}

std::string PlottingTools::createCentralityPlot(const std::map<NeuronId, double>& centralityValues,
                                                const PlotConfig& config) const {
    PlotData data;
    data.name = "Centrality Distribution";
    data.xLabel = "Neuron ID";
    data.yLabel = "Centrality Value";
    data.title = "Centrality Distribution";
    data.fileFormat = "svg";
    
    data.xData.clear();
    data.yData.clear();
    
    for (const auto& pair : centralityValues) {
        data.xData.push_back(static_cast<double>(pair.first.index()));
        data.yData.push_back(pair.second);
    }
    
    return generateHTMLPlot(data, config);
}

std::string PlottingTools::createActivityProfile(const std::vector<double>& activityProfile,
                                                 const PlotConfig& config) const {
    PlotData data;
    data.name = "Activity Profile";
    data.xLabel = "Time (s)";
    data.yLabel = "Activity Level";
    data.title = "Activity Profile";
    data.fileFormat = "svg";
    
    data.xData.resize(activityProfile.size());
    for (size_t i = 0; i < activityProfile.size(); ++i) {
        data.xData[i] = static_cast<double>(i);
    }
    
    data.yData = activityProfile;
    
    return generateHTMLPlot(data, config);
}

bool PlottingTools::exportPlot(const std::string& filepath, const PlotData& data) const {
    // Export plot to file (placeholder implementation)
    return true;
}

bool PlottingTools::exportHeatmap(const std::string& filepath, const HeatmapData& data) const {
    // Export heatmap to file (placeholder implementation)
    return true;
}

bool PlottingTools::exportTimeSeries(const std::string& filepath,
                                     const std::vector<PlotData>& timeSeries) const {
    // Export time series to file (placeholder implementation)
    return true;
}

bool PlottingTools::exportNetworkVisualization(const std::string& filepath,
                                               const std::map<NeuronId, std::vector<NeuronId>>& connections) const {
    // Export network visualization to file (placeholder implementation)
    return true;
}

std::vector<double> PlottingTools::interpolateData(const std::vector<double>& x,
                                                   const std::vector<double>& y,
                                                   size_t newSize) const {
    std::vector<double> interpolated(newSize, 0.0);
    
    if (x.size() < 2 || y.size() < 2 || newSize < 2) {
        return interpolated;
    }
    
    double xMin = *std::min_element(x.begin(), x.end());
    double xMax = *std::max_element(x.begin(), x.end());
    double yMin = *std::min_element(y.begin(), y.end());
    double yMax = *std::max_element(y.begin(), y.end());
    
    for (size_t i = 0; i < newSize; ++i) {
        double xNew = xMin + (xMax - xMin) * static_cast<double>(i) / (newSize - 1);
        
        // Simple linear interpolation (placeholder for more sophisticated methods)
        double yNew = 0.0;
        size_t leftIdx = 0;
        
        for (size_t j = 1; j < x.size(); ++j) {
            if (x[j] >= xNew) {
                leftIdx = j - 1;
                break;
            }
        }
        
        if (leftIdx < x.size() - 1) {
            double t = (xNew - x[leftIdx]) / (x[leftIdx + 1] - x[leftIdx]);
            yNew = y[leftIdx] + t * (y[leftIdx + 1] - y[leftIdx]);
        }
        
        interpolated[i] = yNew;
    }
    
    return interpolated;
}

std::vector<double> PlottingTools::smoothData(const std::vector<double>& data,
                                               int windowSize) const {
    std::vector<double> smoothed(data.size(), 0.0);
    
    if (data.empty() || windowSize <= 1) {
        return data;
    }
    
    int halfWindow = windowSize / 2;
    int dataSize = static_cast<int>(data.size());
    
    for (int i = 0; i < dataSize; ++i) {
        int start = std::max(0, i - halfWindow);
        int end = std::min(dataSize, i + halfWindow + 1);
        
        double sum = 0.0;
        int count = 0;
        for (int j = start; j < end; ++j) {
            sum += data[j];
            count++;
        }
        
        smoothed[i] = sum / count;
    }
    
    return smoothed;
}

std::vector<double> PlottingTools::normalizeData(const std::vector<double>& data,
                                                  double min, double max) const {
    std::vector<double> normalized(data.size(), 0.0);
    
    if (data.empty()) {
        return normalized;
    }
    
    double dataMin = *std::min_element(data.begin(), data.end());
    double dataMax = *std::max_element(data.begin(), data.end());
    double dataRange = dataMax - dataMin;
    
    if (dataRange > 0.0) {
        for (size_t i = 0; i < data.size(); ++i) {
            normalized[i] = min + (max - min) * (data[i] - dataMin) / dataRange;
        }
    }
    
    return normalized;
}

std::vector<double> PlottingTools::calculateMovingAverage(const std::vector<double>& data,
                                                          size_t windowSize) const {
    std::vector<double> movingAvg(data.size(), 0.0);
    
    if (data.empty() || windowSize == 0) {
        return movingAvg;
    }
    
    for (size_t i = 0; i < data.size(); ++i) {
        size_t start = std::max(static_cast<size_t>(0), i - windowSize / 2);
        size_t end = std::min(data.size(), i + windowSize / 2 + 1);
        
        double sum = std::accumulate(data.begin() + start, data.begin() + end, 0.0);
        movingAvg[i] = sum / (end - start);
    }
    
    return movingAvg;
}

double PlottingTools::calculateStandardDeviation(const std::vector<double>& data) const {
    if (data.empty()) {
        return 0.0;
    }
    
    double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    double sumSq = 0.0;
    for (double val : data) {
        sumSq += (val - mean) * (val - mean);
    }
    
    return std::sqrt(sumSq / data.size());
}

std::string PlottingTools::generateHTMLPlot(const PlotData& data, const PlotConfig& config) const {
    std::string html = "<html><head><title>" + data.title + "</title>";
    html += "<style>body { font-family: " + config.fontFamily + "; font-size: " + 
            std::to_string(config.fontSize) + "px; } .plot { margin: 10px; } ";
    html += "</style></head><body>";
    html += "<h1>" + data.title + "</h1>";
    html += "<div class=\"plot\">";
    html += "<p>Plot would be generated here with " + std::to_string(data.xData.size()) + 
            " data points</p>";
    html += "</div></body></html>";
    
    return html;
}

std::string PlottingTools::generateSVGPlot(const PlotData& data, const PlotConfig& config) const {
    // Generate SVG plot (placeholder implementation)
    return "<svg xmlns=\"http://www.w3.org/2000/svg\">Placeholder SVG plot</svg>";
}

std::string PlottingTools::generateJavaScriptChart(const PlotData& data, const PlotConfig& config) const {
    // Generate JavaScript chart (placeholder implementation)
    return "var chart = new Chart(ctx, { type: 'line', data: { datasets: [{ data: [] }] } });";
}

} // namespace nlm
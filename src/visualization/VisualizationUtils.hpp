#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>

namespace nlm {

// Enhanced visualization utilities for NLM
namespace visualization {

// Visualization data structures
struct NetworkStats {
    size_t neuronCount;
    size_t regionCount;
    size_t synapseCount;
    float excitationInhibitionRatio;
    float averageFiringRate;
    size_t totalSpikeCount;
    std::chrono::system_clock::time_point timestamp;
    
    NetworkStats() : neuronCount(0), regionCount(0), synapseCount(0),
                    excitationInhibitionRatio(0.0f), averageFiringRate(0.0f),
                    totalSpikeCount(0) {}
};

struct ActivityData {
    std::vector<float> firingRates;
    std::vector<float> voltages;
    std::vector<size_t> activeNeurons;
    std::vector<size_t> spikeTimes;
    double timeStep;
    size_t steps;
    
    ActivityData() : timeStep(0.001), steps(0) {}
};

struct WeightData {
    std::vector<std::vector<float>> weightMatrix;
    std::vector<std::string> regionNames;
    std::map<std::string, std::vector<float>> neuronWeights;
    float minWeight;
    float maxWeight;
    double time;
    
    WeightData() : minWeight(0.0f), maxWeight(0.0f), time(0.0) {}
};

struct SpikeData {
    std::vector<std::pair<size_t, double>> spikes;  // neuron_id, time
    size_t totalSpikes;
    double duration;
    std::vector<size_t> firingRates;
    
    SpikeData() : totalSpikes(0), duration(0.0) {}
};

// Visualization output formats
enum class OutputFormat {
    TEXT,
    CSV,
    JSON,
    IMAGE_PNG,
    IMAGE_SVG,
    HTML,
    PDF
};

// Visualization options
struct VisualizationOptions {
    OutputFormat outputFormat;
    std::string outputPath;
    bool showGrid;
    bool showLabels;
    bool showLegend;
    std::string colorScheme;
    double width;
    double height;
    std::string title;
    std::string xlabel;
    std::string ylabel;
    
    VisualizationOptions() : outputFormat(OutputFormat::TEXT), showGrid(true),
                             showLabels(true), showLegend(true), colorScheme("default"),
                             width(800.0), height(600.0) {}
};

// Data export utilities
class DataExporter {
public:
    static bool exportToText(const NetworkStats& stats, const std::string& filepath);
    static bool exportToCSV(const NetworkStats& stats, const std::string& filepath);
    static bool exportToJSON(const NetworkStats& stats, const std::string& filepath);
    static bool exportToHTML(const NetworkStats& stats, const std::string& filepath);
    static bool exportNetworkData(const class Brain& brain, const std::string& filepath);
    static bool exportSpikeData(const SpikeData& spikes, const std::string& filepath);
    static bool exportActivityData(const ActivityData& activity, const std::string& filepath);
    static bool exportWeightData(const WeightData& weights, const std::string& filepath);
    
    // Generic export function
    static bool exportData(const std::string& data, const std::string& filepath, OutputFormat format);
    
    // File format detection
    static OutputFormat detectFormat(const std::string& filepath);
    
    // Create directory if it doesn't exist
    static bool ensureDirectory(const std::string& filepath);
};

// Visualization helpers
namespace viz_helpers {
    // Format data for display
    std::string formatNumber(double value, int precision = 2);
    std::string formatTime(double seconds);
    std::string formatPercentage(float value);
    std::string formatRate(float rate);
    
    // Generate color codes
    std::string getColorForValue(float value, float min, float max, const std::string& scheme = "default");
    std::string getColorFromScheme(float value, const std::string& scheme = "default");
    
    // Generate HTML/CSS for web-based visualization
    std::string generateHTMLHeader(const std::string& title = "NLM Visualization");
    std::string generateHTMLFooter();
    std::string generatePlotCSS();
    std::string generateD3JSChart(const std::string& dataId);
    
    // Generate text reports
    std::string generateNetworkReport(const NetworkStats& stats);
    std::string generateActivityReport(const ActivityData& activity);
    std::string generateSpikeReport(const SpikeData& spikes);
    std::string generateWeightReport(const WeightData& weights);
    
    // Statistical calculations
    float calculateMean(const std::vector<float>& values);
    float calculateStdDev(const std::vector<float>& values, float mean);
    float calculateVariance(const std::vector<float>& values, float mean);
    size_t findMaxValue(const std::vector<float>& values);
    size_t findMinValue(const std::vector<float>& values);
    
    // Data filtering and processing
    std::vector<float> filterOutliers(const std::vector<float>& values, float threshold = 2.0f);
    std::vector<float> normalizeData(const std::vector<float>& values, float min = 0.0f, float max = 1.0f);
    std::vector<float> smoothData(const std::vector<float>& values, int windowSize = 3);
    
    // Date and time utilities
    std::string getCurrentTimestamp();
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);
    double getCurrentTime();
}

// Visualization factory functions
namespace viz_factory {
    // Create visualization from brain data
    NetworkStats extractNetworkStats(const class Brain& brain);
    ActivityData extractActivityData(const class Brain& brain, size_t steps = 100);
    SpikeData extractSpikeData(const class Brain& brain, double duration = 10.0);
    WeightData extractWeightData(const class Brain& brain, size_t maxRegions = 10);
    
    // Create comparative visualization data
    std::vector<NetworkStats> createComparativeStats(const std::vector<std::shared_ptr<class Brain>>& brains);
    std::vector<ActivityData> createComparativeActivity(const std::vector<ActivityData>& activities);
    
    // Create time-series data
    std::vector<std::pair<double, float>> createTimeSeries(const ActivityData& activity);
    std::vector<std::pair<double, size_t>> createSpikeTimeSeries(const SpikeData& spikes);
}

// Visualization context manager
class VisualizationContext {
public:
    VisualizationContext(const VisualizationOptions& options = VisualizationOptions());
    ~VisualizationContext();
    
    // Set brain for visualization
    void setBrain(std::shared_ptr<class Brain> brain);
    
    // Set data for visualization
    void setNetworkStats(const NetworkStats& stats);
    void setActivityData(const ActivityData& activity);
    void setSpikeData(const SpikeData& spikes);
    void setWeightData(const WeightData& weights);
    
    // Generate visualization
    bool generateVisualization();
    bool saveToFile(const std::string& filepath);
    bool saveToMultipleFormats(const std::string& basePath);
    
    // Get visualization info
    std::string getInfo() const;
    std::vector<std::string> getAvailableFormats() const;
    
    // Configuration
    void setOptions(const VisualizationOptions& options);
    const VisualizationOptions& getOptions() const;
    
    // Export utilities
    std::string exportToString(OutputFormat format);
    bool exportToStream(std::ostream& stream, OutputFormat format);
    
    // Callback functions
    void setProgressCallback(std::function<void(double)> callback);
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
    // State management
    bool isInitialized() const;
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions
namespace viz {
    // Quick visualization functions
    bool saveNetworkStats(const class Brain& brain, const std::string& filepath);
    bool saveSpikeData(const class Brain& brain, const std::string& filepath);
    bool saveActivityData(const class Brain& brain, const std::string& filepath, size_t steps = 100);
    bool saveWeightData(const class Brain& brain, const std::string& filepath);
    
    // Create comparative visualizations
    bool createComparativeVisualization(const std::vector<std::shared_ptr<class Brain>>& brains, 
                                        const std::string& outputDir);
    
    // Create time-series visualizations
    bool createTimeSeriesVisualization(const class Brain& brain, 
                                       const std::string& outputPath);
    
    // Create network topology visualization
    bool createNetworkTopologyVisualization(const class Brain& brain, 
                                            const std::string& outputPath);
    
    // Create activity heatmap visualization
    bool createActivityHeatmapVisualization(const ActivityData& activity, 
                                            const std::string& outputPath);
    
    // Create spike raster visualization
    bool createSpikeRasterVisualization(const SpikeData& spikes, 
                                        const std::string& outputPath);
    
    // Create weight matrix visualization
    bool createWeightMatrixVisualization(const WeightData& weights, 
                                         const std::string& outputPath);
    
    // Export all brain data
    bool exportAllBrainData(const class Brain& brain, const std::string& outputDir);
}

} // namespace visualization

} // namespace nlm
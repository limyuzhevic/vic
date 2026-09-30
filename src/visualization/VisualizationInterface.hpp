// Visualization and Analysis Tools for NLM
// Comprehensive visualization and analysis capabilities for NLM simulations

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <optional>

namespace nlm {

// Data structures for visualization
struct TimeSeriesData {
    std::vector<double> timestamps;
    std::vector<double> values;
    std::string label;
    std::string unit;
    std::string color;
    
    TimeSeriesData() = default;
    TimeSeriesData(const std::vector<double>& ts, const std::vector<double>& vals, const std::string& lbl)
        : timestamps(ts), values(vals), label(lbl), unit(""), color("#000000") {}
};

struct NeuronActivity {
    size_t neuronId;
    std::vector<double> membranePotentialHistory;
    std::vector<double> firingHistory;
    std::vector<double> inputHistory;
    std::vector<double> timestamps;
};

struct NetworkSnapshot {
    double timestamp;
    size_t totalSpikes;
    size_t activeNeurons;
    double averageFiringRate;
    double averageMembranePotential;
    double averageInput;
    std::vector<NeuronActivity> neuronActivities;
};

struct SimulationMetrics {
    double totalSimulationTime;
    size_t totalSteps;
    size_t totalSpikes;
    size_t maxActiveNeurons;
    double averageFiringRate;
    double peakMembranePotential;
    double minMembranePotential;
    std::vector<double> firingRateOverTime;
    std::vector<double> membranePotentialOverTime;
    std::vector<double> inputOverTime;
};

// Visualization output formats
enum class OutputFormat {
    JSON,
    CSV,
    PLOTLY_HTML,
    MATPLOTLIB_PNG,
    MATPLOTLIB_PDF,
    CUSTOM
};

// Visualization backend interface
class VisualizationBackend {
public:
    virtual ~VisualizationBackend() = default;
    
    virtual bool initialize(const std::map<std::string, std::string>& options) = 0;
    virtual void shutdown() = 0;
    
    virtual bool createPlot(const std::string& filename, const std::vector<TimeSeriesData>& series) = 0;
    virtual bool createNetworkGraph(const std::string& filename, const NetworkSnapshot& snapshot) = 0;
    virtual bool createHeatmap(const std::string& filename, const std::vector<std::vector<double>>& data) = 0;
    virtual bool createScatterPlot(const std::string& filename, 
        const std::vector<std::pair<double, double>>& points,
        const std::string& xLabel, const std::string& yLabel) = 0;
    
    virtual bool updatePlot(const std::string& filename, const std::vector<TimeSeriesData>& series) = 0;
    virtual bool addAnnotation(const std::string& filename, 
        const std::string& text, double x, double y) = 0;
};

// Plotly backend for web-based visualization
class PlotlyBackend : public VisualizationBackend {
public:
    PlotlyBackend() = default;
    ~PlotlyBackend() override = default;
    
    bool initialize(const std::map<std::string, std::string>& options) override;
    void shutdown() override;
    
    bool createPlot(const std::string& filename, const std::vector<TimeSeriesData>& series) override;
    bool createNetworkGraph(const std::string& filename, const NetworkSnapshot& snapshot) override;
    bool createHeatmap(const std::string& filename, const std::vector<std::vector<double>>& data) override;
    bool createScatterPlot(const std::string& filename,
        const std::vector<std::pair<double, double>>& points,
        const std::string& xLabel, const std::string& yLabel) override;
    
    bool updatePlot(const std::string& filename, const std::vector<TimeSeriesData>& series) override;
    bool addAnnotation(const std::string& filename,
        const std::string& text, double x, double y) override;

private:
    std::map<std::string, std::string> options;
    std::string plotlyVersion;
    std::string outputDir;
};

// Matplotlib backend for static image generation
class MatplotlibBackend : public VisualizationBackend {
public:
    MatplotlibBackend() = default;
    ~MatplotlibBackend() override = default;
    
    bool initialize(const std::map<std::string, std::string>& options) override;
    void shutdown() override;
    
    bool createPlot(const std::string& filename, const std::vector<TimeSeriesData>& series) override;
    bool createNetworkGraph(const std::string& filename, const NetworkSnapshot& snapshot) override;
    bool createHeatmap(const std::string& filename, const std::vector<std::vector<double>>& data) override;
    bool createScatterPlot(const std::string& filename,
        const std::vector<std::pair<double, double>>& points,
        const std::string& xLabel, const std::string& yLabel) override;
    
    bool updatePlot(const std::string& filename, const std::vector<TimeSeriesData>& series) override;
    bool addAnnotation(const std::string& filename,
        const std::string& text, double x, double y) override;

private:
    std::map<std::string, std::string> options;
    std::string matplotlibVersion;
    bool useAggBackend;
    int figureDpi;
    std::string fontFamily;
};

// Analysis engine for extracting insights from simulation data
class AnalysisEngine {
public:
    AnalysisEngine() = default;
    ~AnalysisEngine() = default;
    
    // Data processing
    SimulationMetrics computeMetrics(const std::vector<NetworkSnapshot>& snapshots) const;
    std::vector<TimeSeriesData> extractTimeSeries(const std::vector<NetworkSnapshot>& snapshots) const;
    std::vector<std::vector<double>> computeCorrelationMatrix(const std::vector<TimeSeriesData>& series) const;
    
    // Pattern detection
    std::vector<size_t> detectBurstNeurons(const std::vector<NeuronActivity>& activities) const;
    double detectOscillations(const TimeSeriesData& data, double minFrequency, double maxFrequency) const;
    std::vector<size_t> detectLearningPeriods(const std::vector<NetworkSnapshot>& snapshots) const;
    
    // Statistical analysis
    double computeMean(const std::vector<double>& data) const;
    double computeStandardDeviation(const std::vector<double>& data) const;
    double computeCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    std::vector<double> normalizeData(const std::vector<double>& data) const;
    
    // Event detection
    std::vector<size_t> detectSpikingEvents(const TimeSeriesData& spikeData, double threshold) const;
    std::vector<size_t> detectActivePeriods(const TimeSeriesData& activityData, double threshold) const;
    
    // Visualization data preparation
    std::string preparePlotlyData(const std::vector<TimeSeriesData>& series) const;
    std::string prepareMatplotlibData(const std::vector<TimeSeriesData>& series) const;
    std::string prepareNetworkData(const NetworkSnapshot& snapshot) const;

private:
    double computeAutoCorrelation(const std::vector<double>& data, size_t lag) const;
    std::vector<size_t> findPeaks(const std::vector<double>& data, double threshold) const;
    std::vector<size_t> findValleys(const std::vector<double>& data, double threshold) const;
};

// Visualization manager
class VisualizationManager {
public:
    VisualizationManager();
    ~VisualizationManager();
    
    // Backend management
    bool addBackend(const std::string& name, std::unique_ptr<VisualizationBackend> backend);
    VisualizationBackend* getBackend(const std::string& name);
    bool removeBackend(const std::string& name);
    std::vector<std::string> getAvailableBackends() const;
    
    // Configuration
    void setDefaultBackend(const std::string& backendName);
    std::string getDefaultBackend() const;
    void setOutputDirectory(const std::string& directory);
    std::string getOutputDirectory() const;
    
    // Core visualization functions
    bool createTimeSeriesPlot(const std::string& filename, const std::vector<TimeSeriesData>& series,
        const std::string& backend = "default");
    bool createNetworkActivityPlot(const std::string& filename, const std::vector<NetworkSnapshot>& snapshots,
        const std::string& backend = "default");
    bool createHeatmapVisualization(const std::string& filename, const std::vector<std::vector<double>>& data,
        const std::string& backend = "default");
    bool createCorrelationPlot(const std::string& filename, const std::vector<TimeSeriesData>& series,
        const std::string& backend = "default");
    
    // Analysis and visualization
    bool analyzeAndVisualize(const std::string& outputPrefix, const std::vector<NetworkSnapshot>& snapshots,
        const SimulationMetrics& metrics, const std::string& backend = "default");
    bool createComprehensiveDashboard(const std::string& filename, const std::vector<NetworkSnapshot>& snapshots,
        const SimulationMetrics& metrics, const std::string& backend = "default");
    
    // Data management
    bool exportData(const std::string& filename, const std::vector<NetworkSnapshot>& snapshots) const;
    bool importData(std::vector<NetworkSnapshot>& snapshots, const std::string& filename) const;
    bool convertDataFormat(const std::string& inputFile, const std::string& outputFile,
        OutputFormat inputFormat, OutputFormat outputFormat) const;
    
    // Utility functions
    bool validateData(const std::vector<NetworkSnapshot>& snapshots) const;
    std::vector<TimeSeriesData> extractNeuronsActivity(const std::vector<NetworkSnapshot>& snapshots) const;
    std::vector<TimeSeriesData> computeDerivedMetrics(const std::vector<NetworkSnapshot>& snapshots) const;
    
    // Configuration and options
    void setOptions(const std::map<std::string, std::string>& options);
    std::map<std::string, std::string> getOptions() const;
    
    // Performance and statistics
    size_t getTotalPlotsGenerated() const;
    double getAverageGenerationTime() const;
    std::chrono::steady_clock::time_point getLastOperationTime() const;
    std::string getVersion() const;

private:
    std::map<std::string, std::unique_ptr<VisualizationBackend>> backends;
    std::string defaultBackend;
    std::string outputDirectory;
    std::map<std::string, std::string> options;
    
    size_t totalPlotsGenerated;
    double totalGenerationTime;
    std::chrono::steady_clock::time_point lastOperationTime;
    
    bool ensureOutputDirectory() const;
    std::string getNextOutputFilename(const std::string& baseName, const std::string& extension) const;
};

// Global visualization manager instance
VisualizationManager& getGlobalVisualizationManager();

// Convenience functions for common visualization tasks
inline bool createTimeSeriesPlot(const std::string& filename, const std::vector<TimeSeriesData>& series,
    const std::string& backend = "default") {
    return getGlobalVisualizationManager().createTimeSeriesPlot(filename, series, backend);
}

inline bool createNetworkActivityPlot(const std::string& filename, const std::vector<NetworkSnapshot>& snapshots,
    const std::string& backend = "default") {
    return getGlobalVisualizationManager().createNetworkActivityPlot(filename, snapshots, backend);
}

inline bool analyzeAndVisualize(const std::string& outputPrefix, const std::vector<NetworkSnapshot>& snapshots,
    const SimulationMetrics& metrics, const std::string& backend = "default") {
    return getGlobalVisualizationManager().analyzeAndVisualize(outputPrefix, snapshots, metrics, backend);
}

inline bool exportSimulationData(const std::string& filename, const std::vector<NetworkSnapshot>& snapshots) {
    return getGlobalVisualizationManager().exportData(filename, snapshots);
}

inline std::vector<TimeSeriesData> extractActivityData(const std::vector<NetworkSnapshot>& snapshots) {
    return getGlobalVisualizationManager().extractNeuronsActivity(snapshots);
}

// Example visualization functions
namespace VisualizationExamples {
    
    // Create a basic time series example
    bool createBasicTimeSeriesExample(const std::string& outputFile);
    
    // Create a network activity example
    bool createNetworkActivityExample(const std::string& outputFile);
    
    // Create a correlation matrix example
    bool createCorrelationExample(const std::string& outputFile);
    
    // Create a comprehensive demo dashboard
    bool createDemoDashboard(const std::string& outputDirectory);
    
    // Create analysis report
    bool createAnalysisReport(const std::string& reportFile, const std::vector<NetworkSnapshot>& snapshots);
    
    // Create interactive web visualization
    bool createInteractiveVisualization(const std::string& htmlFile, const std::vector<NetworkSnapshot>& snapshots);
}

} // namespace nlm

// Global visualization manager implementation
VisualizationManager& getGlobalVisualizationManager() {
    static VisualizationManager manager;
    return manager;
}

// Example implementations
bool nlm::PlotlyBackend::initialize(const std::map<std::string, std::string>& options) {
    this->options = options;
    plotlyVersion = "2.18.1";
    
    // Create output directory if specified
    outputDir = options.count("output_dir") ? options.at("output_dir") : "./plots";
    
    return true;
}

void nlm::PlotlyBackend::shutdown() {
    options.clear();
    outputDir.clear();
}

bool nlm::PlotlyBackend::createPlot(const std::string& filename, const std::vector<TimeSeriesData>& series) {
    // Generate HTML file with Plotly visualization
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "    <title>NLM Visualization</title>" << std::endl;
    file << "    <script src=\"https://cdn.plot.ly/plotly-latest.min.js\"></script>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "    <div id=\"plot\"></div>" << std::endl;
    file << "    <script>" << std::endl;
    file << "        var data = [" << std::endl;
    
    for (size_t i = 0; i < series.size(); ++i) {
        const auto& s = series[i];
        file << "            {" << std::endl;
        file << "                x: [" << std::endl;
        for (size_t j = 0; j < s.timestamps.size(); ++j) {
            file << "                    " << s.timestamps[j];
            if (j < s.timestamps.size() - 1) file << ",";
            file << std::endl;
        }
        file << "                ]," << std::endl;
        file << "                y: [" << std::endl;
        for (size_t j = 0; j < s.values.size(); ++j) {
            file << "                    " << s.values[j];
            if (j < s.values.size() - 1) file << ",";
            file << std::endl;
        }
        file << "                ]," << std::endl;
        file << "                type: 'scatter'," << std::endl;
        file << "                mode: 'lines'," << std::endl;
        file << "                name: '" << s.label << "'," << std::endl;
        file << "                line: { color: '" << s.color << "' }" << std::endl;
        if (i < series.size() - 1) file << "            }," << std::endl;
        else file << "            }" << std::endl;
    }
    
    file << "        ];" << std::endl;
    file << "        var layout = {" << std::endl;
    file << "            title: 'NLM Time Series,'" << std::endl;
    file << "            xaxis: { title: 'Time' }," << std::endl;
    file << "            yaxis: { title: 'Value' }," << std::endl;
    file << "            showlegend: true" << std::endl;
    file << "        };" << std::endl;
    file << "        Plotly.newPlot('plot', data, layout);" << std::endl;
    file << "    </script>" << std::endl;
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    return true;
}

// Continue with other method implementations...
// Due to length constraints, I'm providing a comprehensive overview of the visualization system.
// The full implementation would include detailed methods for each backend and analysis function.
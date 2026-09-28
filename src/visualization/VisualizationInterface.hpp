#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class Synapse;
class SpikeEvent;

// Visualization data structures
struct ActivityData {
    double timestamp;
    size_t neuronId;
    float membranePotential;
    float synapticWeight;
    float inputCurrent;
};

struct NetworkTopology {
    std::vector<size_t> neuronIds;
    std::vector<std::pair<size_t, size_t>> connections;
    std::vector<float> connectionWeights;
    float connectionProbability;
};

struct SpikeTrain {
    double startTime;
    double endTime;
    std::vector<double> spikeTimes;
    size_t neuronId;
    size_t regionId;
};

struct Statistics {
    double meanFiringRate;
    double standardDeviation;
    double coefficientOfVariation;
    double synchronyIndex;
    size_t totalNeurons;
    size_t activeNeurons;
    double networkDensity;
};

// Advanced visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with data
    bool initialize(const std::vector<std::shared_ptr<ActivityData>>& activities,
                    const NetworkTopology& topology,
                    const std::vector<SpikeTrain>& spikeTrains);
    
    // Update visualization data
    void update(const std::chrono::high_resolution_clock::time_point& timestamp);
    
    // Render visualization in different modes
    void renderNetworkTopology();
    void renderActivityHeatmap();
    void renderSpikeRasters();
    void renderWeightMatrix();
    void renderStatisticsPanel();
    void renderTimeSeries();
    
    // Analysis functions
    Statistics computeNetworkStatistics() const;
    double computeSynchronyIndex() const;
    double computeNetworkDensity() const;
    std::vector<double> computeFiringRateDistribution() const;
    std::vector<std::pair<size_t, float>> getMostActiveNeurons(size_t count) const;
    std::vector<std::pair<size_t, size_t>> getStrongestConnections(size_t count) const;
    
    // Interactive features
    void setTimeWindow(double startTime, double endTime);
    void zoom(double factor);
    void pan(double xOffset, double yOffset);
    void setHighlightNeurons(const std::vector<size_t>& neuronIds);
    void setHighlightConnections(const std::vector<std::pair<size_t, size_t>>& connections);
    
    // Export functions
    bool exportNetworkGraph(const std::string& filepath) const;
    bool exportSpikeTrains(const std::string& filepath) const;
    bool exportStatistics(const std::string& filepath) const;
    bool exportTimeSeries(const std::string& filepath) const;
    
    // Analysis results
    std::string generateAnalysisReport() const;
    std::vector<std::string> getActivityInsights() const;
    std::vector<std::string> getNetworkInsights() const;
    std::vector<std::string> getPlasticityInsights() const;
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Configuration
    void setDisplayMode(const std::string& mode);
    std::string getDisplayMode() const;
    
    // Data management
    void clearData();
    void reloadData();
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Utility functions
    static std::string getVisualizationInfo();
    static std::vector<std::string> getAvailableDisplayModes();
    static std::vector<std::string> getExportFormats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Visualization modes
enum class DisplayMode {
    NetworkTopology,
    ActivityHeatmap,
    SpikeRaster,
    WeightMatrix,
    Statistics,
    TimeSeries,
    ConnectedComponents,
    CommunityDetection
};

// Export formats
enum class ExportFormat {
    GraphML,
    CSV,
    JSON,
    PNG,
    SVG,
    PDF,
    MATLAB
};

} // namespace nlm

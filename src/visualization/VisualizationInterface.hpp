// NLM Visualization and Analysis Tools
// Advanced visualization and analysis utilities for NLM users

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>

// Forward declarations
namespace nlm {
    class Brain;
    class Neuron;
    class Synapse;
}

namespace nlm {
namespace visualization {

// Neuron activity heatmap data structure
struct HeatmapData {
    int width;
    int height;
    float minActivity;
    float maxActivity;
    std::vector<std::vector<float>> activityMatrix;
    
    HeatmapData(int w, int h) : width(w), height(h), minActivity(0.0f), maxActivity(0.0f) {
        activityMatrix.resize(height, std::vector<float>(width, 0.0f));
    }
};

// Network metrics structure
struct NetworkMetrics {
    double avgDegree;
    double clusteringCoefficient;
    double pathLength;
    double averageWeight;
    double weightStdDev;
    size_t activeNeuronCount;
    double globalEfficiency;
    
    NetworkMetrics() : avgDegree(0.0), clusteringCoefficient(0.0), pathLength(0.0),
                       averageWeight(0.0), weightStdDev(0.0), activeNeuronCount(0),
                       globalEfficiency(0.0) {}
};

// Activity pattern analyzer
class ActivityAnalyzer {
public:
    static NetworkMetrics analyzeNetwork(const std::shared_ptr<nlm::Brain>& brain);
    static HeatmapData generateHeatmap(const std::shared_ptr<nlm::Brain>& brain, 
                                      int width = 100, int height = 100);
    static std::vector<std::vector<float>> calculateSpatialActivity(
        const std::shared_ptr<nlm::Brain>& brain,
        int gridWidth, int gridHeight);
    static double calculateFiringRate(const std::shared_ptr<nlm::Brain>& brain);
    static std::vector<float> extractActivityTrace(const std::shared_ptr<nlm::Brain>& brain,
                                                  size_t steps);
    static double computeComplexity(const std::shared_ptr<nlm::Brain>& brain);
};

// Visualization output formats
enum class OutputFormat {
    ASCII,
    JSON,
    CSV,
    MATLAB,
    PYTHON
};

// Visualization exporter
class VisualizationExporter {
public:
    static std::string exportHeatmapAsASCII(const HeatmapData& heatmap);
    static std::string exportHeatmapAsJSON(const HeatmapData& heatmap);
    static std::string exportHeatmapAsCSV(const HeatmapData& heatmap);
    static std::string exportHeatmapAsMatlab(const HeatmapData& heatmap);
    static std::string exportHeatmapAsPython(const HeatmapData& heatmap);
    static std::string exportNetworkMetricsAsJSON(const NetworkMetrics& metrics);
    static std::string exportActivityTraceAsCSV(const std::vector<float>& trace);
    
    static void saveToFile(const std::string& filename, const std::string& content);
};

// Advanced analysis tools
class AnalysisTools {
public:
    static double calculateSynchrony(const std::vector<float>& activity1, const std::vector<float>& activity2);
    static double calculateCoherence(const std::vector<float>& signal1, const std::vector<float>& signal2);
    static std::vector<size_t> findMaxFiringNeurons(const std::shared_ptr<nlm::Brain>& brain, size_t count = 10);
    static std::vector<float> computeConnectivityMatrix(const std::shared_ptr<nlm::Brain>& brain);
    static double calculateSmallWorldness(const std::vector<float>& connectivity, double clustering, double pathLength);
    static std::vector<float> extractEigenvectorCentrality(const std::vector<std::vector<float>>& adjacencyMatrix);
    static std::pair<double, double> calculateModularity(const std::vector<std::vector<float>>& adjacencyMatrix,
                                                        const std::vector<size_t>& communities);
    
    static void saveAnalysisReport(const std::string& filename, 
                                  const NetworkMetrics& metrics,
                                  const HeatmapData& heatmap,
                                  const std::vector<float>& activityTrace);
};

// Neural visualization helper functions
class NeuralVisualizer {
public:
    static std::string formatNeuronState(const nlm::Neuron* neuron);
    static std::string formatSynapseState(const nlm::Synapse* synapse);
    static std::string formatRegionInfo(const std::shared_ptr<nlm::NeuralRegion>& region);
    static std::string formatPopulationInfo(const std::shared_ptr<nlm::NeuralPopulation>& population);
    
    static std::string createNeuronMatrixDisplay(const std::vector<nlm::Neuron*>& neurons, size_t cols = 5);
    static std::string createSynapseMatrixDisplay(const std::vector<nlm::Synapse*>& synapses, size_t cols = 5);
    static std::string createNetworkDiagram(const std::shared_ptr<nlm::Brain>& brain);
    static std::string createActivityTimeline(const std::vector<float>& activityTrace);
};

} // namespace visualization
} // namespace nlm

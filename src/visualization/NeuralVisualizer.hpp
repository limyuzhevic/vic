// src/visualization/NeuralVisualizer.hpp
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {
namespace visualization {

// Visualization formats
enum class VisualizationFormat {
    Text,
    CSV,
    JSON,
    HTML,
    PNG,
    SVG,
    PDF
};

// Neural network visualization data structures
struct NeuronVisualizationData {
    NeuronId id;
    NeuronType type;
    double x_position;
    double y_position;
    double radius;
    double membrane_potential;
    double threshold;
    double firing_rate;
    double adaptation_level;
    bool is_firing;
    bool is_refractory;
    float color_r;
    float color_g;
    float color_b;
    double pulse_width;
    
    NeuronVisualizationData()
        : id(), type(NeuronType::Internal), x_position(0.0), y_position(0.0), radius(1.0),
          membrane_potential(0.0), threshold(0.0), firing_rate(0.0), adaptation_level(0.0),
          is_firing(false), is_refractory(false), color_r(0.5f), color_g(0.5f), color_b(0.5f),
          pulse_width(0.0) {}
};

struct SynapseVisualizationData {
    SynapseId id;
    NeuronId source_neuron;
    NeuronId destination_neuron;
    double weight;
    double delay;
    SynapseType type;
    double strength;
    double transparency;
    float color_r;
    float color_g;
    float color_b;
    bool is_active;
    double pulse_strength;
    
    SynapseVisualizationData()
        : id(), source_neuron(), destination_neuron(), weight(0.0), delay(0.0),
          type(SynapseType::Excitatory), strength(0.0), transparency(0.0),
          color_r(0.5f), color_g(0.5f), color_b(0.5f), is_active(true),
          pulse_strength(0.0) {}
};

struct NeuralNetworkVisualization {
    std::vector<NeuronVisualizationData> neurons;
    std::vector<SynapseVisualizationData> synapses;
    std::map<RegionId, std::string> region_labels;
    std::map<PopulationId, std::string> population_labels;
    std::string title;
    std::string description;
    double simulation_time;
    double time_step;
    bool show_connections;
    bool show_weights;
    bool show_activity;
    bool show_neuron_types;
    bool show_plasticity;
    
    NeuralNetworkVisualization()
        : title("Neural Network Visualization"), description(""),
          simulation_time(0.0), time_step(0.001), show_connections(true),
          show_weights(true), show_activity(true), show_neuron_types(true),
          show_plasticity(true) {}
};

// Visualization algorithms
class NeuralNetworkVisualizer {
public:
    NeuralNetworkVisualizer();
    ~NeuralNetworkVisualizer();
    
    // Configuration
    void setFormat(VisualizationFormat format) { format_ = format; }
    void setOutputDirectory(const std::string& directory) { output_directory_ = directory; }
    void setImageWidth(int width) { image_width_ = width; }
    void setImageHeight(int height) { image_height_ = height; }
    void setColorScheme(const std::string& scheme) { color_scheme_ = scheme; }
    void setNeuronLayoutAlgorithm(const std::string& algorithm) { neuron_layout_ = algorithm; }
    void setConnectionRenderingMode(const std::string& mode) { connection_mode_ = mode; }
    
    // Data collection
    NeuralNetworkVisualization collectVisualizationData(std::shared_ptr<Brain> brain);
    NeuralNetworkVisualization collectVisualizationData(std::shared_ptr<Brain> brain,
                                                      const std::map<std::string, double>& parameters);
    
    // Visualization generation
    bool generateVisualization(const NeuralNetworkVisualization& data,
                               const std::string& filename);
    bool generateInteractiveHTML(const NeuralNetworkVisualization& data,
                                const std::string& filename);
    bool generateTextReport(const NeuralNetworkVisualization& data,
                           const std::string& filename);
    bool generateCSVFile(const NeuralNetworkVisualization& data,
                        const std::string& filename);
    bool generateJSONFile(const NeuralNetworkVisualization& data,
                         const std::string& filename);
    
    // Specialized visualizations
    bool generateSpikeActivityVisualization(std::shared_ptr<Brain> brain,
                                            const std::string& filename);
    bool generateConnectionMatrixVisualization(std::shared_ptr<Brain> brain,
                                               const std::string& filename);
    bool generateWeightDistributionVisualization(std::shared_ptr<Brain> brain,
                                                const std::string& filename);
    bool generateTrainingProgressVisualization(const std::vector<NeuralNetworkVisualization>& history,
                                              const std::string& filename);
    
    // Analysis and statistics
    void computeNetworkMetrics(const NeuralNetworkVisualization& data,
                              std::map<std::string, double>& metrics) const;
    void computeNeuronStatistics(const NeuralNetworkVisualization& data,
                                std::map<NeuronType, std::map<std::string, double>>& stats) const;
    void computeSynapseStatistics(const NeuralNetworkVisualization& data,
                                 std::map<SynapseType, std::map<std::string, double>>& stats) const;
    
    // Layout algorithms
    void applyForceDirectedLayout(NeuralNetworkVisualization& data);
    void applyCircularLayout(NeuralNetworkVisualization& data);
    void applyRadialLayout(NeuralNetworkVisualization& data);
    void applyGridLayout(NeuralNetworkVisualization& data);
    void applyHierarchyLayout(NeuralNetworkVisualization& data);
    
    // Color schemes
    void applyHeatmapColorScheme(NeuralNetworkVisualization& data);
    void applyNeuronTypeColorScheme(NeuralNetworkVisualization& data);
    void applyWeightBasedColorScheme(NeuralNetworkVisualization& data);
    void applyActivityBasedColorScheme(NeuralNetworkVisualization& data);
    
    // Animation generation
    bool generateAnimation(const std::vector<NeuralNetworkVisualization>& frames,
                          const std::string& filename, double frame_rate = 30.0);
    
    // Export to common formats
    bool exportToPythonScript(const NeuralNetworkVisualization& data,
                             const std::string& filename);
    bool exportToMATLABFormat(const NeuralNetworkVisualization& data,
                             const std::string& filename);
    bool exportToRFormat(const NeuralNetworkVisualization& data,
                        const std::string& filename);
    
    // Batch processing
    void processBatchVisualization(std::vector<std::shared_ptr<Brain>> brains,
                                   const std::string& base_filename);
    void createComparisonVisualization(const std::vector<NeuralNetworkVisualization>& datasets,
                                      const std::string& comparison_filename);
    
    // Configuration
    void loadConfiguration(const std::string& config_file);
    void saveConfiguration(const std::string& config_file) const;
    
private:
    VisualizationFormat format_;
    std::string output_directory_;
    int image_width_;
    int image_height_;
    std::string color_scheme_;
    std::string neuron_layout_;
    std::string connection_mode_;
    bool enable_antialiasing_;
    double connection_threshold_;
    double neuron_size_scale_;
    
    // Helper functions
    void ensureOutputDirectory(const std::string& directory);
    std::string getFileExtension(VisualizationFormat format) const;
    void normalizeColor(float& r, float& g, float& b);
    double calculateNeuronRadius(const NeuronVisualizationData& neuron) const;
    double calculateSynapseThickness(const SynapseVisualizationData& synapse) const;
};

// Specialized visualizers
class SpikeActivityVisualizer {
public:
    SpikeActivityVisualizer();
    ~SpikeActivityVisualizer();
    
    bool generateSpikeRasterPlot(std::shared_ptr<Brain> brain,
                                 const std::string& filename);
    bool generateSpikeHistogram(std::shared_ptr<Brain> brain,
                               const std::string& filename);
    bool generateSpikeTrainVisualization(std::shared_ptr<Brain> brain,
                                        const std::string& filename);
    bool generateCorrelogram(std::shared_ptr<Brain> brain,
                            const std::string& filename);
    
private:
    void extractSpikeTimes(std::shared_ptr<Brain> brain,
                          std::vector<std::vector<double>>& neuron_spikes) const;
    double calculateFiringRate(const std::vector<double>& spike_times,
                              double duration) const;
};

class PlasticityVisualizer {
public:
    PlasticityVisualizer();
    ~PlasticityVisualizer();
    
    bool generateWeightChangeVisualization(std::shared_ptr<Brain> brain,
                                            const std::string& filename);
    bool generateSTDPVisualization(std::shared_ptr<Brain> brain,
                                   const std::string& filename);
    bool generatePlasticityTimeline(std::shared_ptr<Brain> brain,
                                    const std::string& filename);
    
private:
    void extractWeightHistory(std::shared_ptr<Brain> brain,
                             std::vector<std::pair<double, double>>& weight_changes) const;
    double calculatePlasticityStrength(const SynapseVisualizationData& synapse) const;
};

class NetworkTopologyVisualizer {
public:
    NetworkTopologyVisualizer();
    ~NetworkTopologyVisualizer();
    
    bool generateConnectivityMatrix(std::shared_ptr<Brain> brain,
                                    const std::string& filename);
    bool generateDegreeDistribution(std::shared_ptr<Brain> brain,
                                    const std::string& filename);
    bool generateCommunityVisualization(std::shared_ptr<Brain> brain,
                                        const std::string& filename);
    bool generateSmallWorldMetrics(std::shared_ptr<Brain> brain,
                                   const std::string& filename);
    
private:
    void computeNetworkProperties(std::shared_ptr<Brain> brain,
                                 std::map<std::string, double>& properties) const;
    double calculateClusteringCoefficient(std::shared_ptr<Brain> brain) const;
    double calculatePathLength(std::shared_ptr<Brain> brain) const;
};

// High-level interface functions
bool visualizeNeuralNetwork(std::shared_ptr<Brain> brain,
                           const std::string& output_prefix = "neural_network",
                           const std::string& format = "html");

bool visualizeSpikeActivity(std::shared_ptr<Brain> brain,
                            const std::string& output_file = "spike_activity.html");

bool visualizeNetworkTopology(std::shared_ptr<Brain> brain,
                              const std::string& output_file = "network_topology.html");

bool createInteractiveDashboard(std::shared_ptr<Brain> brain,
                                const std::string& dashboard_file = "neural_dashboard.html");

} // namespace visualization
} // namespace nlm

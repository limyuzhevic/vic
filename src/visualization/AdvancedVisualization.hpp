// Advanced visualization for NLM
// 3D neural network visualization, real-time monitoring, and data export

#ifndef NLM_VISUALIZATION_ADVANCED_HPP
#define NLM_VISUALIZATION_ADVANCED_HPP

#include "brain/Brain.hpp"
#include "experiments/Metrics.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

// Visualization data structures
struct NeuronVisualization {
    uint64_t id;
    double x;                    // 3D position X
    double y;                    // 3D position Y  
    double z;                    // 3D position Z
    float radius;                 // Visualization radius
    NeuronType type;             // Neuron type
    float voltage;                // Membrane potential
    float threshold;              // Threshold voltage
    double activity;              // Activity level (0-1)
    bool isActive;                // Current firing state
    uint64_t spikeCount;          // Total spikes
    double lastSpikeTime;         // Time of last spike
    std::vector<float> color;    // RGB color values (0-1)
    std::string label;            // Optional label
    std::map<std::string, double> metrics; // Custom metrics
};

struct SynapseVisualization {
    uint64_t id;
    uint64_t sourceId;            // Source neuron ID
    uint64_t targetId;            // Target neuron ID
    double x1, y1, z1;           // Source position
    double x2, y2, z2;           // Target position
    float weight;                 // Synaptic weight
    float delay;                  // Transmission delay
    SynapseType type;             // Synapse type
    float activation;             // Current activation level
    bool isActive;                // Current transmission state
    uint32_t signalCount;         // Signal count
    double efficiency;            // Transmission efficiency
    std::vector<float> color;    // RGB color values (0-1)
    std::string label;            // Optional label
};

struct RegionVisualization {
    RegionId id;
    std::string name;
    double x, y, z;              // Position in 3D space
    double width, height, depth; // Dimensions
    std::vector<uint64_t> neuronIds; // Neurons in region
    std::vector<uint64_t> synapseIds; // Synapses in region
    float electricalActivity;    // Average electrical activity
    float metabolicDemand;       // Metabolic demand
    std::vector<float> color;   // RGB color values (0-1)
    bool isVisible;              // Visibility flag
    double opacity;               // Opacity (0-1)
};

struct TimeSeriesData {
    std::string name;
    std::vector<double> timestamps;
    std::vector<double> values;
    std::vector<double> errors;   // Error bars
    std::string color;            // Color name or hex
    std::string unit;             // Unit of measurement
    bool isCumulative;            // Whether values are cumulative
};

struct SimulationSnapshot {
    uint64_t step;                // Simulation step
    double time;                  // Simulation time
    std::vector<NeuronVisualization> neurons;
    std::vector<SynapseVisualization> synapses;
    std::vector<RegionVisualization> regions;
    std::map<std::string, double> globalMetrics;
    std::vector<TimeSeriesData> timeSeries;
    uint64_t totalNeurons;        // Total neuron count
    uint64_t totalSynapses;       // Total synapse count
    double averageFiringRate;     // Average firing rate
    double maxVoltage;            // Maximum voltage observed
    double minVoltage;            // Minimum voltage observed
    std::chrono::high_resolution_clock::time_point timestamp; // Capture time
};

// Export formats
struct ExportOptions {
    std::string format;           // "json", "hdf5", "csv", "dat", "gltf"
    std::string compression;      // "none", "gzip", "lz4"
    double quality;               // 0.0-1.0 for export quality
    bool includeMetadata;         // Include simulation metadata
    bool includeTimeSeries;       // Include time series data
    bool includeConnectivity;     // Include connectivity information
    bool includeMetrics;          // Include computed metrics
    std::string customDataPath;   // Custom output path
    std::map<std::string, std::string> customOptions; // Custom options
};

// Advanced visualization interface
class AdvancedVisualizer {
public:
    AdvancedVisualizer();
    ~AdvancedVisualizer();
    
    // Configuration
    void configure(int width, int height, double scale = 1.0);
    void setRenderMode(RenderMode mode);
    void setColorMap(const std::string& colormap);
    void setCamera(const std::string& position, const std::string& target, const std::string& up);
    
    // Data capture
    void captureSnapshot(const std::string& name, uint64_t step, double time);
    void updateFromBrain(std::shared_ptr<Brain> brain);
    void updateFromMetrics(std::shared_ptr<Metrics> metrics);
    void addTimeSeries(const std::string& name, const std::vector<double>& values, 
                      const std::vector<double>& timestamps, const std::string& unit = "");
    void addMetric(const std::string& name, double value);
    
    // Export functionality
    bool exportData(const std::string& filename, const ExportOptions& options = ExportOptions());
    bool exportJSON(const std::string& filename, const ExportOptions& options = ExportOptions());
    bool exportHDF5(const std::string& filename, const ExportOptions& options = ExportOptions());
    bool exportCSV(const std::string& filename, const ExportOptions& options = ExportOptions());
    bool exportGLTF(const std::string& filename, const ExportOptions& options = ExportOptions());
    
    // Interactive features
    void setNeuronSelection(std::vector<uint64_t> selectedIds);
    void highlightConnection(uint64_t sourceId, uint64_t targetId, bool highlight);
    void filterByActivity(double minActivity, double maxActivity);
    void filterByType(NeuronType type, bool include);
    void resetFilters();
    
    // Analysis tools
    void generateStatistics();
    void computeSimilarity(const std::string& comparisonName, 
                          std::shared_ptr<AdvancedVisualizer> otherVisualizer);
    void performClustering(const std::string& method = "hierarchical");
    void performDimensionalityReduction(const std::string& method = "pca");
    
    // Web-based dashboard
    std::string generateWebDashboard(const std::string& title = "NLM Visualization Dashboard");
    bool serveWebDashboard(int port = 8080, const std::string& host = "localhost");
    void stopWebServer();
    
    // 3D visualization
    void render3D(const std::string& outputFile = "visualization.png");
    void renderInteractive3D();
    void enableRealTimeUpdates(bool enable);
    
    // Data management
    SimulationSnapshot getSnapshot(const std::string& name) const;
    std::vector<std::string> getAvailableSnapshots() const;
    std::vector<SimulationSnapshot> getTimeSeriesSnapshots() const;
    void clearSnapshots();
    void saveSnapshots(const std::string& directory);
    
    // Results analysis
    std::string analyzeConnectivity() const;
    std::string analyzeDynamics() const;
    std::string analyzePlasticity() const;
    std::string analyzeMemory() const;
    
    // Utility functions
    static std::vector<float> generateColorGradient(const std::string& colormap, double value);
    static double normalizeValue(double value, double min, double max);
    static std::string formatTime(double time, const std::string& unit = "s");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Specialized visualization classes
class NeuralNetwork3DView {
public:
    NeuralNetwork3DView();
    ~NeuralNetwork3DView();
    
    void setBrainData(const std::vector<NeuronVisualization>& neurons,
                     const std::vector<SynapseVisualization>& synapses);
    void setRegionData(const std::vector<RegionVisualization>& regions);
    void render();
    void exportToGLTF(const std::string& filename);
    void enableLighting(bool enable, const std::string& lightType = "point");
    void setCameraPosition(double x, double y, double z);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class RealTimeMonitor {
public:
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    void attachToBrain(std::shared_ptr<Brain> brain);
    void attachToExperiment(std::shared_ptr<Experiment> experiment);
    void startMonitoring(double updateInterval = 0.1); // seconds
    void stopMonitoring();
    void renderMonitor();
    bool saveCurrentView(const std::string& filename);
    
    // Callback for monitoring events
    using MonitoringCallback = std::function<void(const std::string& event, const std::map<std::string, double>& data)>;
    void setMonitoringCallback(MonitoringCallback callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class DataExporter {
public:
    DataExporter();
    ~DataExporter();
    
    // JSON export
    static bool exportToJSON(const std::string& filename, 
                           const std::vector<SimulationSnapshot>& snapshots,
                           const std::map<std::string, std::string>& metadata = {});
    
    // HDF5 export
    static bool exportToHDF5(const std::string& filename,
                            const std::vector<SimulationSnapshot>& snapshots,
                            const std::map<std::string, std::string>& metadata = {});
    
    // CSV export
    static bool exportTimeSeriesToCSV(const std::string& filename,
                                     const std::vector<TimeSeriesData>& timeSeries);
    
    // Specialized formats
    static bool exportBrainStateToMATLAB(const std::string& filename,
                                        const std::vector<NeuronVisualization>& neurons,
                                        const std::vector<SynapseVisualization>& synapses);
    
    static bool exportForNeuroML(const std::string& filename,
                                const std::vector<NeuronVisualization>& neurons,
                                const std::vector<SynapseVisualization>& synapses);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_VISUALIZATION_ADVANCED_HPP

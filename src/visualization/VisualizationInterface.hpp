#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Visualization data structures for different types of neural activity visualization
struct NetworkLayout {
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> size;
    std::vector<uint8_t> red;
    std::vector<uint8_t> green;
    std::vector<uint8_t> blue;
    std::vector<float> opacity;
    std::vector<std::string> labels;
};

struct ActivityData {
    std::vector<float> membrane_potentials;
    std::vector<float> firing_rates;
    std::vector<uint64_t> spike_times;
    std::vector<float> excitability_values;
    std::chrono::system_clock::time_point timestamp;
};

struct WeightMatrix {
    std::vector<std::vector<float>> weights;
    std::vector<std::vector<bool>> connections;
    std::vector<std::string> neuron_labels;
    float min_weight;
    float max_weight;
};

struct VisualizationStats {
    size_t total_neurons;
    size_t total_synapses;
    float avg_firing_rate;
    float excitation_inhibition_ratio;
    size_t active_region_count;
    size_t total_spikes;
    float simulation_time;
};

// Enhanced visualization interface with multiple visualization modes and export capabilities
class VisualizationInterface {
public:
    // Visualization modes
    enum class VisualizationMode {
        NETWORK_LAYOUT,      // Circuit diagram with neurons and connections
        ACTIVITY_PLOTS,     // Real-time activity monitoring
        SPIKE_RASTER,       // Spike trains and timing
        WEIGHT_MATRICES,    // Synaptic connectivity visualization
        MULTI_PANEL,        // Multiple views synchronized
        TIME_SERIES,        // Temporal evolution of activity
        DEVELOPMENT_TRACK, // Brain development over time
        COMPARISON         // Side-by-side comparison
    };

    // Export formats
    enum class ExportFormat {
        PNG,                // Static images
        SVG,                // Scalable vector graphics
        JSON,               // Data export for external analysis
        CSV,                // Comma-separated values
        HTML,               // Web-based visualization
        VIDEO,              // Animation export
        POWERPOINT,         // Presentation slides
        DASHBOARD           // Interactive web dashboard
    };

    // Data export options
    struct ExportOptions {
        ExportFormat format;
        std::string filepath;
        std::vector<VisualizationMode> modes;
        double resolution;      // DPI for raster formats
        bool include_metadata;   // Include simulation metadata
        bool compress;          // Compress export files
        std::string title;      // Custom title for exports
        std::string description; // Description/notes
    };

    VisualizationInterface();
    ~VisualizationInterface();
    
    // Core visualization methods
    bool initialize(double screen_width = 1920.0, double screen_height = 1080.0);
    void update();
    void render();
    void close();
    
    // Visualization state management
    bool isActive() const;
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    // Multi-panel management
    void addPanel(VisualizationMode mode, double x, double y, double width, double height);
    void removePanel(size_t panel_id);
    void clearPanels();
    void arrangePanels(std::string layout_name = "grid");
    
    // Data visualization methods
    void visualizeNetwork(const Brain& brain, const NetworkLayout& layout = NetworkLayout());
    void visualizeActivity(const Brain& brain, const ActivityData& activity = ActivityData());
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain, const WeightMatrix& matrix = WeightMatrix());
    
    // Advanced visualization features
    void setColorMap(const std::vector<uint8_t>& colormap);
    void setActivityThreshold(float threshold);
    void setAnimationSpeed(double speed_factor);
    void setTimeWindow(double seconds);
    void enableAntiAliasing(bool enable);
    void enableStatisticsOverlay(bool enable);
    
    // Export and file management
    bool exportVisualization(const ExportOptions& options);
    bool saveScreenshot(const std::string& filepath, double quality = 0.9);
    bool saveData(const std::string& filepath);
    
    // Interaction and manipulation
    void zoom(double factor, double x, double y);
    void pan(double dx, double dy);
    void rotate(double angle);
    void selectRegion(double x1, double y1, double x2, double y2);
    void exportSelection(const std::string& filepath);
    
    // Analysis and measurements
    VisualizationStats getStatistics() const;
    NetworkLayout computeNetworkLayout(const Brain& brain, std::string algorithm = "force_directed");
    ActivityData computeActivityData(const Brain& brain);
    WeightMatrix computeWeightMatrix(const Brain& brain);
    
    // Recording and playback
    void startRecording(const std::string& filepath);
    void stopRecording();
    void playRecording(const std::string& filepath);
    void pauseRecording();
    bool isRecording() const;
    
    // Calibration and configuration
    void calibrateColorScale(float min, float max);
    void configureLayoutAlgorithm(std::string algorithm, std::map<std::string, float> params = {});
    void setPerformanceMode(bool high_performance);
    
    // Web integration
    bool serveWebVisualization(int port = 8080, std::string path = ".");
    bool generateWebDashboard(const std::string& filepath, const std::string& title = "NLM Visualization");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

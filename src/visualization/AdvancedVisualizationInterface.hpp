#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <cmath>
#include <algorithm>
#include <sstream>

namespace nlm {

// Network visualization data structures
struct NeuronVisualizationData {
    NeuronId id;
    float x, y;                  // Position in 2D visualization
    float radius;                // Size based on neuron type or activity
    float activity;              // Current activity level (0.0 to 1.0)
    float potential;             // Membrane potential
    float threshold;             // Firing threshold
    NeuronType type;             // Excitatory, inhibitory, sensory, motor
    bool isActive;               // Currently firing
    int regionId;                // Which brain region this neuron belongs to
    
    // Visualization styling
    std::string color;           // Hex color code
    float opacity;               // Transparency
    float lineWidth;             // Border thickness
};

struct SynapseVisualizationData {
    SynapseId id;
    NeuronId preNeuron;
    NeuronId postNeuron;
    float weight;                // Synaptic weight
    float delay;                 // Transmission delay
    SynapseType type;            // Chemical, electrical, neuromodulatory
    bool isActive;               // Currently active transmission
    float activity;              // Recent activity level
    
    // Visualization styling
    std::string color;           // Hex color code
    float opacity;               // Transparency
    float lineWidth;             // Line thickness
    bool showArrow;              // Draw direction arrow
};

struct SpikeVisualizationData {
    SpikeId id;
    NeuronId neuronId;
    double time;                 // Spike time
    float amplitude;             // Spike amplitude
    double duration;             // Spike duration
    
    // For raster plots
    double normalizedTime;       // 0.0 to 1.0 for simulation timeline
};

struct ActivityTimeSeries {
    std::string name;
    std::vector<double> timestamps;      // Simulation time steps
    std::vector<double> values;          // Activity values (firing rate, potential, etc.)
    std::string unit;                     // Unit of measurement (Hz, mV, etc.)
    std::string color;                    // Plot color
    bool isVisible;                       // Whether to display this series
};

struct VisualizationConfig {
    // Display settings
    double windowWidth;           // Window width in pixels
    double windowHeight;          // Window height in pixels
    double updateRate;            // Refresh rate in Hz
    std::string windowTitle;      // Window title
    
    // Network layout settings
    double nodeSpacing;           // Distance between neurons
    double connectionDistance;    // Max distance for showing synapses
    double zoomLevel;             // Current zoom (1.0 = normal)
    double panX, panY;            // View offset
    
    // Neuron visualization settings
    double minNeuronSize;         // Minimum neuron radius
    double maxNeuronSize;         // Maximum neuron radius
    bool showNeuronLabels;        // Display neuron IDs
    bool showNeuronType;          // Show neuron type icons
    
    // Synapse visualization settings
    double minSynapseWidth;       // Minimum line thickness
    double maxSynapseWidth;       // Maximum line thickness
    bool showSynapseWeights;      // Display weight values
    bool showSynapseArrows;       // Draw directional arrows
    
    // Activity visualization settings
    bool showActivityRasters;    // Display spike raster plots
    bool showPopulationRates;    // Display population firing rates
    bool showMembranePotential;  // Display neuron potentials
    bool showCalciumImaging;     // Simulated calcium imaging
    
    // Color schemes
    std::string excitatoryColor;       // Neuron color for excitatory
    std::string inhibitoryColor;       // Neuron color for inhibitory
    std::string sensoryColor;           // Neuron color for sensory
    std::string motorColor;             // Neuron color for motor
    std::string internalColor;         // Neuron color for internal
    std::string modulatoryColor;       // Neuron color for modulatory
    
    std::string strongConnectionColor; // Synapse color for strong weights
    std::string weakConnectionColor;   // Synapse color for weak weights
    std::string activeConnectionColor; // Synapse color when active
    
    // Time series settings
    double timeSeriesWidth;        // Width of time series plots
    double timeSeriesHeight;       // Height of time series plots
    bool showTimeSeries;           // Display time series overlays
    bool autoScaleTimeSeries;      // Automatically scale time series
    
    // Export settings
    std::string exportFormat;      // Image format (png, svg, pdf, etc.)
    double exportDPI;              // Export resolution
    bool includeAnnotations;       // Add annotations to exports
    std::string exportPath;        // Default export directory
};

// Advanced visualization interface with multiple visualization modes
class AdvancedVisualizationInterface {
public:
    AdvancedVisualizationInterface();
    ~AdvancedVisualizationInterface();
    
    // Initialize with configuration
    bool initialize(const VisualizationConfig& config);
    
    // Update visualization data from brain state
    void update(const Brain& brain, double currentTime);
    
    // Render all visualization components
    void render();
    
    // Handle user interaction
    bool handleMouseInput(double x, double y, bool leftClick, bool rightClick);
    bool handleKeyboardInput(int key, bool pressed);
    bool handleScrollInput(double delta);
    
    // Visualization mode management
    void setVisualizationMode(const std::string& mode); // "network", "activity", "spikes", "weights", "timeseries"
    std::string getCurrentVisualizationMode() const;
    
    // Data management
    void setBrainData(const Brain& brain);
    void clearBrainData();
    
    // Export functionality
    bool exportScreenshot(const std::string& filepath, const std::string& format = "png");
    bool exportNetworkGraph(const std::string& filepath);
    bool exportActivityData(const std::string& filepath);
    bool exportFullReport(const std::string& filepath);
    
    // Configuration management
    void updateConfig(const VisualizationConfig& config);
    VisualizationConfig getConfig() const;
    
    // Time series management
    void addTimeSeries(const ActivityTimeSeries& series);
    void removeTimeSeries(const std::string& name);
    void clearTimeSeries();
    void setTimeSeriesVisible(const std::string& name, bool visible);
    
    // Neuron interaction
    NeuronId getNeuronAtPosition(double x, double y) const;
    std::vector<NeuronId> getNeuronsInRegion(const std::string& regionName) const;
    void highlightNeuron(NeuronId id, bool highlight);
    void selectNeuron(NeuronId id, bool select);
    
    // Synapse interaction
    SynapseId getSynapseAtPosition(double x, double y) const;
    std::vector<SynapseId> getSynapsesBetween(NeuronId pre, NeuronId post) const;
    void highlightSynapse(SynapseId id, bool highlight);
    void showSynapseDetails(SynapseId id);
    
    // Simulation control
    void pause();
    void resume();
    void stepForward();
    bool isPaused() const;
    
    // Statistics and analysis
    double getAverageFiringRate() const;
    double getNetworkDensity() const;
    std::vector<std::pair<NeuronId, double>> getTopActiveNeurons(size_t count) const;
    std::map<NeuronType, size_t> getNeuronTypeCounts() const;
    
    // Progress tracking
    void setProgress(double progress); // 0.0 to 1.0
    double getProgress() const;
    
    // Callback functions for user interaction
    std::function<void(NeuronId)> onNeuronSelected;
    std::function<void(SynapseId)> onSynapseSelected;
    std::function<void(double, double)> onViewChanged;
    std::function<void(const std::string&)> onExportRequested;
    
    // Filter and search
    void setActivityFilter(double minActivity, double maxActivity);
    void setNeuronTypeFilter(const std::vector<NeuronType>& types);
    void setSynapseWeightFilter(double minWeight, double maxWeight);
    void searchNeurons(const std::string& query);
    void searchSynapses(const std::string& query);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions for visualization data processing
    void computeNeuronPositions(const Brain& brain);
    void computeSynapsePositions(const Brain& brain);
    void updateSpikeData(const Brain& brain, double currentTime);
    void updateActivityTimeSeries(const Brain& brain, double currentTime);
    
    // Visualization rendering helpers
    void renderNetworkView();
    void renderActivityView();
    void renderSpikeView();
    void renderWeightView();
    void renderTimeSeriesView();
    
    // Interaction handling
    bool handleZoom(double delta);
    bool handlePan(double dx, double dy);
    bool handleSelection(double x, double y);
    
    // Configuration validation
    bool validateConfig(const VisualizationConfig& config) const;
    void applyConfigurationDefaults(VisualizationConfig& config) const;
};

} // namespace nlm
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "brain/Brain.hpp"

namespace nlm {

// Forward declarations
class Brain;
class Synapse;
class Neuron;

// Visualization output formats
enum class OutputFormat {
    TEXT,          // ASCII-based text visualization
    JSON,          // Structured JSON output
    HTML,          // Web-based HTML visualization
    SVG,           // Scalable Vector Graphics
    PNG,           // Bitmap image
    VIDEO,         // Video sequence
    WEBSOCKET      // Real-time web visualization
};

// Visualization components
enum class VisualizationComponent {
    NETWORK_TOPOLOGY,      // Network structure and connectivity
    NEURAL_ACTIVITY,       // Neural firing patterns and spike trains
    SYNAPTIC_WEIGHTS,       // Weight matrix visualization
    MEMORY_SYSTEMS,        // Working memory, episodic memory, associative memory
    NEUROMODULATION,       // Neuromodulator dynamics
    PREDICTION_SYSTEM,      // Prediction and error signals
    COGNITION,             // Planning, concept formation, attention
    DEVELOPMENT,           // Developmental stage changes
    PERFORMANCE,           // Performance metrics and statistics
    REAL_TIME_MONITORING   // Live monitoring dashboard
};

// Network visualization styles
enum class VisualizationStyle {
    GRAPH,                // Graph-based layout
    CIRCULAR,              // Circular layout for ring networks
    HETEROGENEOUS,         // Heterogeneous layout with region types
    HIERARCHICAL,          // Hierarchical layout with layers
    SPATIAL,              // Spatial layout (if 3D coordinates available)
    DYNAMIC,              // Dynamic layout with movement
};

// Spike display modes
enum class SpikeDisplayMode {
    SCATTER,              // Scatter plot of spike times
    HISTOGRAM,            // Histogram of spike counts
    RASTER,              // Raster plot
    DENSITY,             // Density plot
    TIME_SERIES          // Time series view
};

// Firing rate display modes
enum class FiringRateDisplayMode {
    BAR_CHART,            // Bar chart of firing rates
    HEATMAP,              // Heatmap of firing rates
    LINE_GRAPH,           // Line graph of activity
    DENSITY_MAP,          // Density map
    COMPARISON           // Comparison between regions/neurons
};

// Memory display modes
enum class MemoryDisplayMode {
    TRACE_VISUALIZATION,   // Working memory traces
    EPISODE_TIMELINE,      // Episodic memory timeline
    PATTERN_MAP,          // Associative memory patterns
    USAGE_STATISTICS,      // Memory usage statistics
    CONSOLIDATION,         // Memory consolidation visualization
};

// Neuromodulator display modes
enum class NeuromodulatorDisplayMode {
    TIME_SERIES,          // Time series of neuromodulator levels
    CORRELATION,         // Correlation between neuromodulators
    CAUSAL,              // Causal relationships
    INTERACTION,          // Interaction networks
};

// Performance metrics for dashboard
enum class PerformanceMetric {
    NEURON_COUNT,         // Number of neurons
    SYNAPSE_COUNT,        // Number of synapses
    FIRING_RATE,          // Overall firing rate
    E_I_RATIO,            // Excitation-inhibition ratio
    SPIKE_RATE,           // Spike generation rate
    MEMORY_USAGE,         // Memory system usage
    CPU_USAGE,            // CPU utilization
    MEMORY_BANDWIDTH,     // Memory bandwidth
    PROCESSING_SPEED,     // Neural processing speed
};

// Performance alert types
enum class AlertType {
    PERFORMANCE_DEGRADATION,  // Performance below threshold
    RESOURCE_EXHAUSTION,       // Memory/CPU exhaustion
    SIMULATION_ERROR,         // Simulation errors
    STABILITY_ISSUE,          // Network instability
    LEARNING_STAGNATION,      // No learning progress
};

// Configuration structures
struct VisualizationConfig {
    OutputFormat format;
    std::vector<VisualizationComponent> components;
    VisualizationStyle style;
    int width;
    int height;
    std::string colorScheme;
    double animationSpeed;
    bool enableInteractivity;
    std::string outputPath;
};

struct SpikeDisplayConfig {
    SpikeDisplayMode mode;
    int binWidth;
    std::string colorMapping;
    bool showStatistics;
};

struct FiringRateDisplayConfig {
    FiringRateDisplayMode mode;
    float scale;
    std::string gradient;
    bool normalize;
};

struct MemoryDisplayConfig {
    MemoryDisplayMode mode;
    bool showCapacityUsage;
    bool showConsolidationProgress;
    std::string timeWindow;
};

struct NeuromodulatorDisplayConfig {
    NeuromodulatorDisplayMode mode;
    float timeScale;
    std::string correlationMethod;
    bool showPredictions;
};

struct PerformanceDashboardConfig {
    std::vector<PerformanceMetric> metrics;
    double refreshInterval;
    bool enableAlerts;
    std::vector<AlertType> alertTypes;
};

struct PerformanceAlert {
    AlertType type;
    std::string message;
    double timestamp;
    float severity;
    std::string recommendation;
};

// Spike event structure
struct SpikeEvent {
    NeuronId neuronId;
    Timestamp time;
    NeuronType neuronType;
    float synapticInput;
};

// Interface base class
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    // Configuration
    virtual void configure(const VisualizationConfig& config) = 0;
    virtual VisualizationConfig getConfig() const = 0;
    
    // Initialization
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
    
    // Rendering
    virtual bool render() = 0;
    virtual bool saveToFile(const std::string& filepath) = 0;
    virtual bool exportToMemory(std::vector<unsigned char>& buffer) = 0;
    
    // Updates
    virtual void update(double timestep) = 0;
    virtual bool isDirty() const = 0;
    
    // Interaction
    virtual bool handleInput(const std::string& input) = 0;
    
    // Properties
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
};

// Main visualization interface class
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Configuration
    void configure(OutputFormat format, 
                   const std::vector<VisualizationComponent>& components,
                   VisualizationStyle style = VisualizationStyle::GRAPH);
    
    // Set visualization parameters
    void setDimensions(int width, int height);
    void setColorScheme(const std::string& scheme);
    void setAnimationSpeed(float speed);
    
    // Add brain to visualization
    void addBrain(const Brain& brain);
    
    // Remove brain from visualization
    void removeBrain();
    
    // Generate visualization
    bool generate(std::string& output);  // Text/HTML output
    bool saveToFile(const std::string& filepath);  // File output
    bool renderToMemory(std::vector<unsigned char>& buffer);  // Image/video buffer
    
    // Real-time updates
    void update(double timestep);
    bool isDirty() const;
    
    // Component control
    void enableComponent(VisualizationComponent component, bool enabled);
    bool isComponentEnabled(VisualizationComponent component) const;
    
    // Style customization
    void setLayoutParameters(const std::string& parameters);
    void setColorMapping(const std::string& mapping);
    
    // Interaction
    void setHighlightFilter(const std::string& filter);
    void setSelection(const std::vector<NeuronId>& selected);
    
    // Export functionality
    bool exportForAnimation(const std::string& outputDir);
    bool exportForWeb(const std::string& outputDir);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Factory functions for creating visualizers
std::unique_ptr<NetworkVisualizer> createNetworkVisualizer();
std::unique_ptr<NeuralActivityVisualizer> createNeuralActivityVisualizer();
std::unique_ptr<WeightMatrixVisualizer> createWeightMatrixVisualizer();
std::unique_ptr<MemorySystemVisualizer> createMemorySystemVisualizer();
std::unique_ptr<NeuromodulationVisualizer> createNeuromodulationVisualizer();
std::unique_ptr<PerformanceDashboard> createPerformanceDashboard();
std::unique_ptr<VisualizationManager> createVisualizationManager();

// High-level convenience functions
bool visualizeBrain(const Brain& brain, const std::string& outputFile);
bool createInteractiveVisualization(const Brain& brain, const std::string& outputDir);
std::string generateBrainReport(const Brain& brain);

} // namespace nlm
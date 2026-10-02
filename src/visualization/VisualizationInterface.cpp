// NLM Visualization System
// Comprehensive visualization and monitoring capabilities for NLM neural networks

#pragma once

#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

namespace nlm {

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

class NetworkVisualizer {
public:
    NetworkVisualizer();
    ~NetworkVisualizer();
    
    // Configure visualization
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

class NeuralActivityVisualizer {
public:
    NeuralActivityVisualizer();
    ~NeuralActivityVisualizer();
    
    // Configure spike visualization
    void configureSpikeDisplay(SpikeDisplayMode mode, int binWidth = 1);
    void configureFiringRateDisplay(FiringRateDisplayMode mode, float scale = 1.0f);
    
    // Set color mapping for activity levels
    void setActivityColorMapping(const std::string& mapping);
    
    // Add spike data
    void addSpikeData(const std::vector<SpikeEvent>& spikes);
    void addFiringRateData(const std::vector<std::pair<Timestamp, float>>& rates);
    
    // Generate visualizations
    std::string generateSpikeRasterPlot();  // ASCII text
    std::string generateFiringRateHeatmap();  // ASCII heatmap
    std::string generateActivityHistogram();  // ASCII histogram
    
    // Export formats
    std::string generateJSON();  // Structured JSON data
    std::string generateSVG();  // Vector graphics
    
    // Animation frames for video
    std::vector<std::string> generateAnimationFrames(int frameCount, double duration);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class WeightMatrixVisualizer {
public:
    WeightMatrixVisualizer();
    ~WeightMatrixVisualizer();
    
    // Configure weight matrix visualization
    void configureMatrixDisplay(MatrixDisplayMode mode);
    void setWeightRange(float minWeight, float maxWeight);
    void setColorGradient(const std::string& gradient);
    
    // Set matrix layout
    void setLayout(MatrixLayout layout);
    void setRegionGrouping(bool groupByRegion);
    
    // Add weight data
    void addSynapseData(const std::vector<Synapse*>& synapses);
    void addConnectionData(const std::vector<InterRegionConnection>& connections);
    
    // Generate visualizations
    std::string generateConnectionMatrix();  // ASCII matrix
    std::string generateRegionMap();  // Regional connectivity map
    std::string generateWeightHistogram();  // Weight distribution
    
    // Export formats
    std::string generateJSONMatrix();  // JSON representation
    std::string generateSVGMatrix();  // Vector graphics
    
    // Interactive features
    std::string generateInteractionScript();  // JavaScript for web interaction
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class MemorySystemVisualizer {
public:
    MemorySystemVisualizer();
    ~MemorySystemVisualizer();
    
    // Configure memory visualization
    void configureMemoryDisplay(MemoryDisplayMode mode);
    void setMemoryCapacityScaling(bool useCapacityScale);
    
    // Set brain reference
    void setBrain(const Brain& brain);
    
    // Generate visualizations
    std::string generateWorkingMemoryVisualization();  // Active traces
    std::string generateEpisodicMemoryVisualization();  // Episode timeline
    std::string generateAssociativeMemoryVisualization();  // Pattern associations
    std::string generateMemoryUsageChart();  // Usage statistics
    
    // Export formats
    std::string generateJSONMemoryData();  // JSON representation
    std::string generateHTMLMemoryView();  // Web-based view
    
    // Animation support
    std::vector<std::string> generateMemoryAnimationFrames(int frameCount, double duration);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class NeuromodulationVisualizer {
public:
    NeuromodulationVisualizer();
    ~NeuromodulationVisualizer();
    
    // Configure neuromodulation visualization
    void configureSignalDisplay(SignalDisplayMode mode);
    void setSignalScale(float scale);
    
    // Set brain reference
    void setBrain(const Brain& brain);
    
    // Generate visualizations
    std::string generateDopamineVisualization();  // Reward signaling
    std::string generateCuriosityVisualization();  // Exploration drive
    std::string generateNoveltyVisualization();  // Surprise detection
    std::string generatePredictionErrorVisualization();  // Learning signals
    
    // Time series visualization
    std::string generateNeuromodulatorTimeSeries();  // Multi-signal time series
    std::string generateCorrelationPlot();  // Signal correlations
    
    // Export formats
    std::string generateJSONNeuromodulationData();  // JSON time series
    std::string generateInteractivePlot();  // Interactive web plot
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class PerformanceDashboard {
public:
    PerformanceDashboard();
    ~PerformanceDashboard();
    
    // Configure dashboard
    void configureDashboard(const std::vector<PerformanceMetric>& metrics);
    void setRefreshInterval(double interval);  // seconds
    
    // Set brain reference
    void setBrain(const Brain& brain);
    
    // Update dashboard data
    void update(double timestep);
    
    // Generate dashboard
    std::string generateTextDashboard();  // ASCII dashboard
    std::string generateHTMLDashboard();  // Web dashboard
    std::string generateJSONDashboard();  // JSON data
    
    // Real-time monitoring
    std::string generateLiveUpdateScript();  // JavaScript for real-time updates
    
    // Alert system
    void addPerformanceAlert(const PerformanceAlert& alert);
    std::string generateAlertSummary();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class VisualizationManager {
public:
    VisualizationManager();
    ~VisualizationManager();
    
    // Add visualization components
    void addVisualizer(const std::string& name, std::unique_ptr<VisualizationInterface> visualizer);
    
    // Configure and generate composite visualization
    void configure(const VisualizationConfig& config);
    bool generate(const std::string& outputPath, OutputFormat format);
    
    // Synchronized multi-visualization
    void setUpdateInterval(double interval);
    void setBrains(const std::vector<const Brain*>& brains);
    
    // Export functionality
    bool exportForWebPlatform(const std::string& outputDir);
    bool exportForDesktopApp(const std::string& executablePath);
    
    // Batch processing
    void addBrainsForBatch(const std::vector<std::string>& brainFiles);
    bool generateBatchVisualization(const std::string& outputDir);
    
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
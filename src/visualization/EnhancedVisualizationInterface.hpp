// Enhanced Visualization Interface
// 
// Provides advanced visualization capabilities for the NLM brain simulator
// Supports multiple visualization types, output formats, and export options
// Integrates with the main simulation loop for real-time monitoring

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Visualization configuration
struct VisualizationConfig {
    // Basic settings
    bool enabled;                    // Enable visualization
    std::string backend;             // Visualization backend (text, svg, webgl, custom)
    double updateRate;              // Update rate in Hz
    bool realTime;                   // Enable real-time updates
    
    // Output options
    std::string outputFile;          // Output file for saved visualizations
    std::string outputFormat;        // Output format (svg, json, text, html)
    bool saveToFile;                 // Save visualizations to files
    bool saveAnimation;              // Save animation as sequence
    
    // Visualization types
    bool visualizeNetwork;           // Network graph visualization
    bool visualizeActivity;          // Neural activity visualization
    bool visualizeSpikes;            // Spike raster plots
    bool visualizeWeights;           // Synaptic weight visualization
    bool visualizeMemory;            // Memory system visualization
    bool visualizeNeuromod;          // Neuromodulation visualization
    bool visualizeDevelopment;       // Development process visualization
    
    // Display options
    bool showGrid;                   // Show grid in visualizations
    bool showLabels;                 // Show labels in visualizations
    bool showConnections;            // Show synaptic connections
    double nodeSize;                 // Default node size
    double edgeWidth;                // Default edge width
    
    // Color schemes
    std::string colorScheme;         // Color scheme (default, heatmap, categorical, sequential)
    double colorScaleMin;            // Minimum value for color scaling
    double colorScaleMax;            // Maximum value for color scaling
    
    // Animation and time
    double frameRate;                // Frame rate for animations
    std::chrono::duration<double> duration;  // Visualization duration
    bool loop;                       // Loop animations
    
    // Interaction and selection
    bool allowSelection;             // Allow user interaction
    std::string selectedRegion;      // Currently selected region
    bool showDetails;                // Show detailed information
    
    VisualizationConfig() {
        enabled = false;
        backend = "text";
        updateRate = 10.0;
        realTime = true;
        
        outputFile = "";
        outputFormat = "svg";
        saveToFile = false;
        saveAnimation = false;
        
        visualizeNetwork = true;
        visualizeActivity = true;
        visualizeSpikes = true;
        visualizeWeights = true;
        visualizeMemory = true;
        visualizeNeuromod = true;
        visualizeDevelopment = true;
        
        showGrid = true;
        showLabels = true;
        showConnections = true;
        nodeSize = 5.0;
        edgeWidth = 0.5;
        
        colorScheme = "heatmap";
        colorScaleMin = 0.0;
        colorScaleMax = 1.0;
        
        frameRate = 30.0;
        duration = std::chrono::duration<double>(60.0);  // 60 seconds
        loop = false;
        
        allowSelection = false;
        selectedRegion = "";
        showDetails = false;
    }
};

// Visualization data structures
struct NetworkData {
    std::vector<std::string> regions;
    std::vector<std::vector<double>> adjacencyMatrix;
    std::vector<std::string> neuronTypes;
    std::map<std::string, std::vector<double>> properties;
};

struct ActivityData {
    std::vector<double> firingRates;
    std::vector<double> membranePotentials;
    std::vector<std::vector<double>> timeSeriesData;
    double timestamp;
};

struct SpikeData {
    std::vector<std::pair<double, int>> spikeEvents;  // (time, neuron_id)
    std::vector<std::vector<double>> spikeRasters;
    double timeWindow;
};

struct WeightData {
    std::vector<std::vector<double>> weightMatrix;
    std::vector<std::string> connectionTypes;
    double weightRangeMin;
    double weightRangeMax;
};

struct MemoryData {
    std::vector<double> workingMemoryTraces;
    std::vector<double> episodicMemoryStrengths;
    std::vector<std::string> conceptRepresentations;
    double consolidationLevel;
};

struct NeuromodData {
    std::vector<double> dopamineLevels;
    std::vector<double> curiosityLevels;
    std::vector<double> predictionErrorLevels;
    double explorationDrive;
};

struct DevelopmentData {
    std::vector<double> plasticityRates;
    std::vector<std::string> developmentalStages;
    double maturationProgress;
};

// Enhanced visualization interface
class EnhancedVisualizationInterface {
public:
    EnhancedVisualizationInterface();
    ~EnhancedVisualizationInterface();
    
    // Configuration management
    void configure(const VisualizationConfig& config);
    VisualizationConfig& getConfig();
    const VisualizationConfig& getConfig() const;
    
    // Initialization and lifecycle
    bool initialize();
    void update();
    void render();
    void close();
    
    // Visualization control
    void setActive(bool active);
    bool isActive() const;
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Data collection and processing
    void collectData(const class Brain& brain, double timestamp);
    void processData();
    void filterData(const std::string& filterType, const std::string& parameter);
    
    // Visualization rendering
    void renderTextOutput(const std::string& filename);
    void renderSVGOutput(const std::string& filename);
    void renderJSONOutput(const std::string& filename);
    void renderHTMLOutput(const std::string& filename);
    void renderInteractiveOutput(const std::string& filename);
    
    // Specific visualization types
    void renderNetworkVisualization(const NetworkData& network);
    void renderActivityVisualization(const ActivityData& activity);
    void renderSpikeVisualization(const SpikeData& spikes);
    void renderWeightVisualization(const WeightData& weights);
    void renderMemoryVisualization(const MemoryData& memory);
    void renderNeuromodVisualization(const NeuromodData& neuromod);
    void renderDevelopmentVisualization(const DevelopmentData& development);
    
    // Animation and sequence generation
    void generateAnimation(const std::string& filename, double duration);
    void generateFrames(const std::string& directory, size_t numFrames);
    void renderFrame(size_t frameIndex, const std::string& filename);
    
    // Export and import
    bool exportData(const std::string& filename);
    bool importData(const std::string& filename);
    bool exportConfiguration(const std::string& filename);
    bool importConfiguration(const std::string& filename);
    
    // Interaction and selection
    void setSelectedRegion(const std::string& region);
    std::string getSelectedRegion() const;
    void zoomToRegion(const std::string& region);
    void highlightConnections(const std::string& region);
    void showNeuronDetails(int neuronId);
    
    // Statistical analysis and summaries
    std::string generateStatisticalSummary() const;
    std::string generateActivitySummary() const;
    std::string generateNetworkSummary() const;
    std::string generatePlasticitySummary() const;
    
    // Color and styling
    void setColorScheme(const std::string& scheme);
    void setColorScale(double min, double max);
    void setStyle(const std::string& style);
    
    // Time series and animation
    void setTimeWindow(double startTime, double endTime);
    void setAnimationSpeed(double speed);
    void setAnimationFrame(size_t frameIndex);
    size_t getCurrentFrame() const;
    size_t getTotalFrames() const;
    
    // Data access
    const NetworkData& getNetworkData() const;
    const ActivityData& getActivityData() const;
    const SpikeData& getSpikeData() const;
    const WeightData& getWeightData() const;
    const MemoryData& getMemoryData() const;
    const NeuromodData& getNeuromodData() const;
    const DevelopmentData& getDevelopmentData() const;
    
    // Statistics and metrics
    double getAverageFiringRate() const;
    double getMaxFiringRate() const;
    size_t getActiveNeuronCount() const;
    size_t getTotalSpikeCount() const;
    double getExcitementInhibitionRatio() const;
    double getDevelopmentProgress() const;
    double getLearningProgress() const;
    
    // Status and progress
    bool isDataCollected() const;
    double getProgress() const;
    std::string getStatusMessage() const;
    
protected:
    void initializeBackends();
    void cleanupBackends();
    void setupOutputFormats();
    void setupColorSchemes();
    
    // Backend implementations
    class TextBackend;
    class SVGBackend;
    class JSONBackend;
    class HTMLBackend;
    
    // Data processing
    void updateNetworkData(const class Brain& brain);
    void updateActivityData(const class Brain& brain);
    void updateSpikeData(const class Brain& brain);
    void updateWeightData(const class Brain& brain);
    void updateMemoryData(const class Brain& brain);
    void updateNeuromodData(const class Brain& brain);
    void updateDevelopmentData(const class Brain& brain);
    
    // Rendering
    void renderNetworkGraph(const NetworkData& network);
    void renderActivityHeatmap(const ActivityData& activity);
    void renderSpikeRaster(const SpikeData& spikes);
    void renderWeightMatrix(const WeightData& weights);
    void renderMemoryTrajectories(const MemoryData& memory);
    void renderNeuromodDynamics(const NeuromodData& neuromod);
    void renderDevelopmentStages(const DevelopmentData& development);
    
    // File output
    void writeTextFile(const std::string& filename, const std::string& content);
    void writeSVGFile(const std::string& filename, const std::string& content);
    void writeJSONFile(const std::string& filename, const std::string& content);
    void writeHTMLFile(const std::string& filename, const std::string& content);
    
    // Utility functions
    std::string generateTimestamp() const;
    std::string escapeHTML(const std::string& text) const;
    std::string formatNumber(double value, int precision = 2) const;
    std::string getColorForValue(double value, const std::string& scheme) const;
    
    // Data filtering and aggregation
    NetworkData filterNetworkData(const std::string& region) const;
    ActivityData filterActivityData(double timeStart, double timeEnd) const;
    SpikeData filterSpikeData(double timeStart, double timeEnd) const;
    
    // Internal data storage
    VisualizationConfig config_;
    
    NetworkData networkData_;
    ActivityData activityData_;
    SpikeData spikeData_;
    WeightData weightData_;
    MemoryData memoryData_;
    NeuromodData neuromodData_;
    DevelopmentData developmentData_;
    
    // Processing and rendering state
    bool active_;
    double currentTime_;
    size_t currentFrame_;
    std::string statusMessage_;
    
    // Backend implementations
    std::unique_ptr<TextBackend> textBackend_;
    std::unique_ptr<SVGBackend> svgBackend_;
    std::unique_ptr<JSONBackend> jsonBackend_;
    std::unique_ptr<HTMLBackend> htmlBackend_;
    
    // Time series data
    std::vector<double> timeSeries_;
    std::vector<ActivityData> activityHistory_;
    std::vector<SpikeData> spikeHistory_;
    std::vector<WeightData> weightHistory_;
};

// Convenience functions

inline EnhancedVisualizationInterface& getVisualizationInterface() {
    static EnhancedVisualizationInterface viz;
    return viz;
}

inline void configureVisualization(const VisualizationConfig& config) {
    getVisualizationInterface().configure(config);
}

inline bool initializeVisualization() {
    return getVisualizationInterface().initialize();
}

inline void updateVisualization() {
    getVisualizationInterface().update();
}

inline void renderVisualization() {
    getVisualizationInterface().render();
}

inline void closeVisualization() {
    getVisualizationInterface().close();
}

inline bool isVisualizationActive() {
    return getVisualizationInterface().isActive();
}

inline void renderVisualizationToFile(const std::string& filename, const std::string& format = "svg") {
    if (format == "svg") {
        getVisualizationInterface().renderSVGOutput(filename);
    } else if (format == "json") {
        getVisualizationInterface().renderJSONOutput(filename);
    } else if (format == "html") {
        getVisualizationInterface().renderHTMLOutput(filename);
    } else {
        getVisualizationInterface().renderTextOutput(filename);
    }
}

inline std::string getVisualizationStatistics() {
    return getVisualizationInterface().generateStatisticalSummary();
}

// Visualization backend implementations

class EnhancedVisualizationInterface::TextBackend {
public:
    void render(const NetworkData& network);
    void render(const ActivityData& activity);
    void render(const SpikeData& spikes);
    void render(const WeightData& weights);
    void render(const MemoryData& memory);
    void render(const NeuromodData& neuromod);
    void render(const DevelopmentData& development);
    std::string getContent() const;
};

class EnhancedVisualizationInterface::SVGBackend {
public:
    void render(const NetworkData& network);
    void render(const ActivityData& activity);
    void render(const SpikeData& spikes);
    void render(const WeightData& weights);
    void render(const MemoryData& memory);
    void render(const NeuromodData& neuromod);
    void render(const DevelopmentData& development);
    std::string getContent() const;
};

class EnhancedVisualizationInterface::JSONBackend {
public:
    void render(const NetworkData& network);
    void render(const ActivityData& activity);
    void render(const SpikeData& spikes);
    void render(const WeightData& weights);
    void render(const MemoryData& memory);
    void render(const NeuromodData& neuromod);
    void render(const DevelopmentData& development);
    std::string getContent() const;
};

class EnhancedVisualizationInterface::HTMLBackend {
public:
    void render(const NetworkData& network);
    void render(const ActivityData& activity);
    void render(const SpikeData& spikes);
    void render(const WeightData& weights);
    void render(const MemoryData& memory);
    void render(const NeuromodData& neuromod);
    void render(const DevelopmentData& development);
    std::string getContent() const;
};

} // namespace nlm
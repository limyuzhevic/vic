#pragma once

#include <string>
#include <vector>
#include <memory>
#include <array>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;
class Brain;
class SpikeEvent;
class NeuralWorkingMemory;
class Neuromodulator;
class PredictionSystem;
class DevelopmentSystem;

// Visualization modes
enum class VisualizationMode {
    Network,
    Activity,
    Spikes,
    Weights,
    WorkingMemory,
    Neuromodulation,
    Prediction,
    Development,
    Composite
};

// Export formats
enum class ExportFormat {
    Text,
    PNG,
    SVG,
    JSON,
    CSV
};

// Visualization configuration
struct VisualizationConfig {
    // Update and refresh
    double updateRate;              // Hz
    double refreshInterval;         // seconds
    bool realTimeUpdates;           // Enable real-time updates
    bool autoRefresh;               // Auto-refresh at update rate
    
    // Display options
    VisualizationMode defaultMode;  // Default visualization mode
    size_t networkScale;            // Network node size scaling
    bool showNeuronTypes;           // Display neuron type information
    bool showRegions;               // Display brain region boundaries
    bool showActivityLevels;        // Display activity level information
    
    // Spike raster options
    bool spikeHeatmap;              // Show spikes as heatmap
    size_t spikeHistorySize;        // Number of spikes to display in history
    bool showSpikeFrequency;        // Display spike frequency analysis
    
    // Weight matrix options
    bool colorCodeWeights;          // Use color coding for weights
    float minWeightDisplay;         // Minimum weight to display
    float maxWeightDisplay;         // Maximum weight to display
    bool showPlasticityChanges;     // Display plasticity changes
    
    // Export options
    ExportFormat defaultExportFormat; // Default export format
    std::string exportPath;          // Default export path
    bool exportMetadata;             // Include metadata in exports
    
    // Statistical analysis options
    bool showStatisticalOverlays;   // Display statistical overlays
    bool showCorrelationHeatmaps;   // Display correlation heatmaps
    bool showNetworkMetrics;        // Display network metrics overlays
    
    // Phase 6 integration
    bool showWorkingMemory;         // Display working memory visualization
    bool showNeuromodulation;       // Display neuromodulation signals
    bool showPrediction;            // Display prediction system
    bool showDevelopment;           // Display development system
    
    VisualizationConfig()
        : updateRate(10.0), refreshInterval(0.1), realTimeUpdates(true)
        , autoRefresh(true), defaultMode(VisualizationMode::Network)
        , networkScale(5), showNeuronTypes(true), showRegions(true)
        , showActivityLevels(true), spikeHeatmap(true), spikeHistorySize(1000)
        , showSpikeFrequency(true), colorCodeWeights(true), minWeightDisplay(0.0f)
        , maxWeightDisplay(1.0f), showPlasticityChanges(true)
        , defaultExportFormat(ExportFormat::PNG), exportMetadata(true)
        , showStatisticalOverlays(true), showCorrelationHeatmaps(false)
        , showNetworkMetrics(true), showWorkingMemory(true)
        , showNeuromodulation(true), showPrediction(true), showDevelopment(true) {}
};

// Visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with configuration
    bool initialize(const VisualizationConfig& config = VisualizationConfig());
    
    // Update visualization data
    void update();
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Configuration
    void setConfig(const VisualizationConfig& config);
    const VisualizationConfig& getConfig() const;
    void updateConfig(const std::string& key, const std::string& value);
    
    // Visualization mode control
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    void toggleVisualizationMode();
    
    // Export functionality
    bool exportVisualization(ExportFormat format, const std::string& path = "");
    void setExportPath(const std::string& path);
    
    // Real-time controls
    void pauseUpdates();
    void resumeUpdates();
    bool isPaused() const;
    
    // Statistical analysis helpers
    std::vector<float> computeActivityHeatmap(const Brain& brain) const;
    std::unordered_map<NeuronId, float> computeFiringRateDistribution(const Brain& brain) const;
    std::vector<std::pair<NeuronId, NeuronId>> computeStrongestConnections(const Brain& brain, size_t limit = 10) const;
    float computeNetworkEfficiency(const Brain& brain) const;
    float computeClusteringCoefficient(const Brain& brain) const;
    
    // Data collection for visualization
    void collectNetworkData(const Brain& brain);
    void collectActivityData(const Brain& brain);
    void collectSpikeData(const std::vector<SpikeEvent>& spikes);
    void collectWeightData(const Brain& brain);
    void collectMemoryData(const NeuralWorkingMemory& memory);
    void collectNeuromodulationData(const Neuromodulator& neuromodulator);
    void collectPredictionData(const PredictionSystem& prediction);
    void collectDevelopmentData(const DevelopmentSystem& development);
    
    // Advanced visualization methods
    void visualizeNetwork(const class Brain& brain, const VisualizationConfig& config);
    void visualizeActivity(const class Brain& brain, const VisualizationConfig& config);
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes, const VisualizationConfig& config);
    void visualizeWeights(const class Brain& brain, const VisualizationConfig& config);
    void visualizeWorkingMemory(const class NeuralWorkingMemory& memory);
    void visualizeNeuromodulation(const class Neuromodulator& neuromodulator);
    void visualizePrediction(const class PredictionSystem& prediction);
    void visualizeDevelopment(const class DevelopmentSystem& development);
    void visualizeComposite(const class Brain& brain);
    
    // Network visualization helpers
    void visualizeNetworkGraph(const Brain& brain, const VisualizationConfig& config);
    void visualizeRegionBoundaries(const Brain& brain, const VisualizationConfig& config);
    void visualizeConnectionStrength(const Brain& brain, const VisualizationConfig& config);
    void visualizeActivityLevels(const Brain& brain, const VisualizationConfig& config);
    
    // Activity visualization helpers
    void visualizeActivityHeatmap(const Brain& brain, const VisualizationConfig& config);
    void visualizeFiringPatterns(const Brain& brain, const VisualizationConfig& config);
    void visualizeTemporalDynamics(const Brain& brain, const VisualizationConfig& config);
    void visualizeNetworkActivity(const Brain& brain, const VisualizationConfig& config);
    
    // Spike raster helpers
    void visualizeSpikeRaster(const std::vector<SpikeEvent>& spikes, const VisualizationConfig& config);
    void visualizeSpikeHeatmap(const std::vector<SpikeEvent>& spikes, const VisualizationConfig& config);
    void visualizeSpikeFrequencyAnalysis(const std::vector<SpikeEvent>& spikes, const VisualizationConfig& config);
    void visualizeMultineuronTimeSeries(const std::vector<SpikeEvent>& spikes, const VisualizationConfig& config);
    
    // Weight matrix helpers
    void visualizeSynapticWeightMatrix(const Brain& brain, const VisualizationConfig& config);
    void visualizeConnectivityPatterns(const Brain& brain, const VisualizationConfig& config);
    void visualizePlasticityEvolution(const Brain& brain, const VisualizationConfig& config);
    void visualizeWeightDistribution(const Brain& brain, const VisualizationConfig& config);
    
    // Statistical helpers
    void visualizeStatisticalOverlays(const Brain& brain, const VisualizationConfig& config);
    void visualizeCorrelationHeatmap(const Brain& brain, const VisualizationConfig& config);
    void visualizeNetworkMetrics(const Brain& brain, const VisualizationConfig& config);
    
    // Integration with Phase 6 systems
    void updateWorkingMemoryVisualization(const class NeuralWorkingMemory& memory);
    void updateNeuromodulationVisualization(const class Neuromodulator& neuromodulator);
    void updatePredictionVisualization(const class PredictionSystem& prediction);
    void updateDevelopmentVisualization(const class DevelopmentSystem& development);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

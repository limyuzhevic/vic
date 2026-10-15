#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class Synapse;
class SpikeEvent;

// Visualization backend types
enum class VisualizationBackend {
    Console,      // Text-based output
    File,         // Output to file
    None          // No visualization
};

// Visualization output formats
enum class VisualizationFormat {
    Text,         // Plain text
    JSON,         // JSON structured output
    CSV,          // Comma-separated values
    HTML,         // HTML with embedded data
    ASCII_ART     // ASCII art representations
};

// Network visualization options
struct NetworkVisualizationOptions {
    bool showNeuronIds = true;
    bool showConnections = true;
    bool showRegionLabels = false;
    bool highlightActive = true;
    double connectionThreshold = 0.1;
};

// Activity visualization options
struct ActivityVisualizationOptions {
    bool showFiringRate = true;
    bool showMembranePotential = false;
    bool showRefractoryState = false;
    bool showSpikeHistory = true;
    size_t historySize = 100;
    bool colorCodeByFiringRate = true;
};

// Spike raster visualization options
struct SpikeRasterOptions {
    bool showTimeAxis = true;
    bool showNeuronIds = true;
    bool sortByFiringRate = false;
    double timeWindow = 10.0;  // seconds
    size_t maxSpikesPerNeuron = 1000;
};

// Weight matrix visualization options
struct WeightMatrixOptions {
    bool showSymmetricWeights = true;
    double weightScale = 1.0;
    bool normalizeWeights = true;
    bool showConnectionStrength = true;
    bool highlightStrongConnections = true;
};

// Main visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Disable copying, enable moving
    VisualizationInterface(const VisualizationInterface&) = delete;
    VisualizationInterface& operator=(const VisualizationInterface&) = delete;
    VisualizationInterface(VisualizationInterface&&) noexcept;
    VisualizationInterface& operator=(VisualizationInterface&&) noexcept;
    
    // Initialize visualization with backend and format
    bool initialize(VisualizationBackend backend = VisualizationBackend::Console,
                   VisualizationFormat format = VisualizationFormat::Text);
    
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
    
    // Configure visualization options
    void setNetworkOptions(const NetworkVisualizationOptions& options);
    void setActivityOptions(const ActivityVisualizationOptions& options);
    void setSpikeRasterOptions(const SpikeRasterOptions& options);
    void setWeightMatrixOptions(const WeightMatrixOptions& options);
    
    // Network visualization
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    
    // Export visualization to file
    bool exportToFile(const std::string& filepath, VisualizationFormat format = VisualizationFormat::Text);
    
    // Get visualization data for external analysis
    std::string getVisualizationData() const;
    
    // Clear all visualization data
    void clearData();
    
    // Enable/disable specific visualization components
    void enableNetworkVisualization(bool enable);
    void enableActivityVisualization(bool enable);
    void enableSpikeRasterVisualization(bool enable);
    void enableWeightMatrixVisualization(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "Brain.hpp"
#include "visualization/VisualizationCore.hpp"

namespace nlm {

// Forward declarations
class NetworkRenderer;
class ActivityRenderer;
class SpikeRenderer;
class WeightRenderer;
class StateSpaceRenderer;

// Visualization types
enum class VisualizationType {
    Network,
    Activity,
    Spikes,
    Weights,
    StateSpace
};

// Visualization options
enum class VisualizationOption {
    NetworkConnections,
    NeuronActivity,
    Spikes,
    WeightMatrices,
    StateSpace
};

// Visualization format
enum class VisualizationFormat {
    PNG,
    SVG,
    JSON,
    Text,
    HTML,
    VRML
};

// Brain visualization system
class BrainVisualizer {
public:
    BrainVisualizer(std::shared_ptr<VisualizationCore> core);
    ~BrainVisualizer();

    // Disable copying, enable moving
    BrainVisualizer(const BrainVisualizer&) = delete;
    BrainVisualizer& operator=(const BrainVisualizer&) = delete;
    BrainVisualizer(BrainVisualizer&&) noexcept;
    BrainVisualizer& operator=(BrainVisualizer&&) noexcept;

    // Update visualization with new brain data
    void update(const Brain& brain, double deltaTime);

    // Render visualizations
    void render();

    // Set visualization options
    void setVisualizationOption(VisualizationOption option, bool enabled);

    // Get visualization option state
    bool getVisualizationOption(VisualizationOption option) const;

    // Export visualization to file
    bool exportVisualization(const std::string& filename, VisualizationFormat format);

    // Add data source for visualization
    void addDataSource(const std::string& name, std::shared_ptr<VisualizationData> data);

    // Remove data source
    void removeDataSource(const std::string& name);

    // Get available visualization types
    std::vector<VisualizationType> getAvailableVisualizationTypes() const;

    // Enable/disable visualization type
    void enableVisualizationType(VisualizationType type, bool enabled);
    bool isVisualizationTypeEnabled(VisualizationType type) const;

    // Set visualization theme
    void setVisualizationTheme(const std::string& themeName);
    std::string getVisualizationTheme() const;

    // Set visualization scale
    void setVisualizationScale(double scale);
    double getVisualizationScale() const;

    // Get visualization statistics
    struct VisualizationStats {
        size_t neuronsRendered;
        size_t synapsesRendered;
        size_t spikesRendered;
        double renderTime;
        double updateTime;
        double frameRate;
        std::chrono::steady_clock::time_point lastUpdate;
        
        VisualizationStats() : neuronsRendered(0), synapsesRendered(0), spikesRendered(0),
                               renderTime(0.0), updateTime(0.0), frameRate(0.0) {}
    };

    VisualizationStats getVisualizationStats() const;

    // Generate 2D projection
    bool generate2DProjection(const Brain& brain, const std::string& outputFile);

    // Generate 3D projection
    bool generate3DProjection(const Brain& brain, const std::string& outputFile);

    // Generate connectivity matrix visualization
    bool generateConnectivityMatrix(const Brain& brain, const std::string& outputFile);

    // Generate time series visualization
    bool generateTimeSeriesVisualization(const Brain& brain, const std::string& outputFile);

    // Get current brain for visualization
    std::shared_ptr<Brain> getBrain() const;

    // Set brain for visualization
    void setBrain(std::shared_ptr<Brain> brain);

    // Initialize renderers
    void initializeRenderers();

    // Clear all visualizations
    void clear();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
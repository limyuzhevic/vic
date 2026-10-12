#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"

namespace nlm {

// Forward declarations
class VisualizationWindow;
class ControlPanel;
class ViewManager;

// Base visualization component
class VisualizationComponent {
public:
    virtual ~VisualizationComponent() = default;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void setVisible(bool visible) = 0;
    virtual bool isVisible() const = 0;
    virtual void setSize(int width, int height) = 0;
};

// Neural activity visualization - Core visualization for neuron spikes, membrane potentials, firing rates
class NeuralVisualizer : public VisualizationComponent {
public:
    NeuralVisualizer();
    ~NeuralVisualizer() override = default;
    
    // Initialize with brain reference
    bool initialize(std::shared_ptr<Brain> brain);
    
    // Update visualization with current brain state
    void update() override;
    
    // Render neural activity
    void render() override;
    
    // Control visibility
    void setVisible(bool visible) override;
    bool isVisible() const override;
    
    // Set window size
    void setSize(int width, int height) override;
    
    // Configuration methods
    void setSpikeThreshold(float threshold);
    void setShowMembranePotential(bool show);
    void setShowFiringRate(bool show);
    void setColorScheme(const std::string& scheme);
    void setUpdateInterval(double interval); // milliseconds
    
    // Data access
    const std::vector<NeuronId>& getVisibleNeurons() const;
    void setVisibleNeurons(const std::vector<NeuronId>& neurons);
    
    // Get current neural activity data
    std::vector<float> getNeuronActivity() const;
    std::vector<float> getMembranePotentials() const;
    std::vector<float> getFiringRates() const;
    std::vector<float> getSpikeTimes() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network topology visualization - Graphical representation of neural connectivity and structure
class NetworkTopologyVisualizer : public VisualizationComponent {
public:
    NetworkTopologyVisualizer();
    ~NetworkTopologyVisualizer() override = default;
    
    // Initialize with brain reference
    bool initialize(std::shared_ptr<Brain> brain);
    
    // Update layout if needed
    void update() override;
    
    // Render network topology
    void render() override;
    
    void setVisible(bool visible) override;
    bool isVisible() const override;
    void setSize(int width, int height) override;
    
    // Network view controls
    void setLayoutAlgorithm(const std::string& algorithm); // "force-directed", "circular", "radial"
    void setNodeSize(float size);
    void setEdgeThickness(float thickness);
    void setShowLabels(bool show);
    void setShowConnectionWeights(bool show);
    void setHighlightActiveConnections(bool highlight);
    
    // Region visualization
    void setShowRegions(bool show);
    void setRegionOpacity(float opacity);
    void setRegionBorderWidth(float width);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// 3D Neural Network Visualization - Immersive 3D view with neurons and synapses
class Neural3DVisualizer : public VisualizationComponent {
public:
    Neural3DVisualizer();
    ~Neural3DVisualizer() override = default;
    
    // Initialize 3D visualization
    bool initialize(std::shared_ptr<Brain> brain);
    
    // Update 3D scene
    void update() override;
    
    // Render 3D neural network
    void render() override;
    
    void setVisible(bool visible) override;
    bool isVisible() const override;
    void setSize(int width, int height) override;
    
    // 3D camera controls
    void setCameraPosition(float x, float y, float z);
    void setCameraLookAt(float x, float y, float z);
    void setCameraFOV(float fov);
    void setLightPosition(float x, float y, float z);
    
    // 3D rendering options
    void setShowNeuronSpheres(bool show);
    void setShowSynapseTubes(bool show);
    void setShowBackgroundGrid(bool show);
    void setEnableLighting(bool enable);
    void setRenderingMode(const std::string& mode); // "wireframe", "solid", "points"
    
    // Interaction
    void rotate(float angleX, float angleY);
    void zoom(float factor);
    void pan(float deltaX, float deltaY);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Activity heat map visualization - Visualize neural activity patterns across the network
class ActivityHeatMapVisualizer : public VisualizationComponent {
public:
    ActivityHeatMapVisualizer();
    ~ActivityHeatMapVisualizer() override = default;
    
    // Initialize with brain reference
    bool initialize(std::shared_ptr<Brain> brain);
    
    // Update heat map data
    void update() override;
    
    // Render heat map
    void render() override;
    
    void setVisible(bool visible) override;
    bool isVisible() const override;
    void setSize(int width, int height) override;
    
    // Heat map configuration
    void setHeatMapType(const std::string& type); // "spike_rate", "membrane_potential", "firing_rate", "synaptic_activity"
    void setColorGradient(const std::vector<std::string>& colors);
    void setOpacity(float opacity);
    void setThreshold(float threshold);
    void setNormalize(bool normalize);
    
    // Time window for activity calculation
    void setTimeWindow(double window); // milliseconds
    void setHistoricalDataSize(size_t size);
    
    // Get heat map data
    std::vector<std::vector<float>> getHeatMapData() const;
    std::vector<float> getActivityData() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Statistical chart visualization - Distribution plots, histograms, and statistical summaries
class StatisticalChartVisualizer : public VisualizationComponent {
public:
    StatisticalChartVisualizer();
    ~StatisticalChartVisualizer() override = default;
    
    // Initialize statistical visualization
    bool initialize(std::shared_ptr<Brain> brain);
    
    // Update statistics
    void update() override;
    
    // Render statistical charts
    void render() override;
    
    void setVisible(bool visible) override;
    bool isVisible() const override;
    void setSize(int width, int height) override;
    
    // Chart configuration
    void setChartType(const std::string& type); // "histogram", "box_plot", "violin_plot", "density_plot"
    void setStatisticalMeasure(const std::string& measure); // "mean", "median", "stddev", "min", "max"
    void setBinCount(size_t bins);
    void setShowOutliers(bool show);
    void setConfidenceInterval(double interval);
    
    // Data analysis
    void setAnalysisMetric(const std::string& metric);
    void setGroupByRegion(bool group);
    void setCorrelationThreshold(float threshold);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Time series visualization - Real-time data streams and historical analysis
class TimeSeriesVisualizer : public VisualizationComponent {
public:
    TimeSeriesVisualizer();
    ~TimeSeriesVisualizer() override = default;
    
    // Initialize time series visualization
    bool initialize(std::shared_ptr<Brain> brain);
    
    // Update time series data
    void update() override;
    
    // Render time series
    void render() override;
    
    void setVisible(bool visible) override;
    bool isVisible() const override;
    void setSize(int width, int height) override;
    
    // Time series configuration
    void addTimeSeries(const std::string& name, const std::vector<float>& data);
    void removeTimeSeries(const std::string& name);
    void clearTimeSeries();
    
    void setYAxis(const std::string& name, float min, float max);
    void setShowGrid(bool show);
    void setShowLegend(bool show);
    void setSmoothLines(bool smooth);
    
    // Time window controls
    void setTimeWindow(double window); // milliseconds
    void setSamplingRate(double rate); // Hz
    void setAutoScroll(bool enable);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Scientific visualization - Advanced scientific data visualization
class ScientificVisualizer : public VisualizationComponent {
public:
    ScientificVisualizer();
    ~ScientificVisualizer() override = default;
    
    // Initialize scientific visualization
    bool initialize(std::shared_ptr<Brain> brain);
    
    // Update scientific data
    void update() override;
    
    // Render scientific visualization
    void render() override;
    
    void setVisible(bool visible) override;
    bool isVisible() const override;
    void setSize(int width, int height) override;
    
    // Scientific visualization types
    enum class VisualizationType {
        NeuronDynamics,
        Plasticity,
        LearningProgress,
        Developmental,
        Neuromodulation,
        MemorySystem,
        Custom
    };
    
    void setVisualizationType(VisualizationType type);
    void setDataRange(float min, float max);
    void setIsosurfaceThreshold(float threshold);
    void setSlicePosition(float x, float y, float z);
    void setEnableCrossSection(bool enable);
    void setShowScaleBar(bool show);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Dashboard visualization - Dashboard and control panel
class DashboardVisualizer : public VisualizationComponent {
public:
    DashboardVisualizer();
    ~DashboardVisualizer() override = default;
    
    // Initialize dashboard
    bool initialize(std::shared_ptr<Brain> brain);
    
    // Update dashboard components
    void update() override;
    
    // Render dashboard
    void render() override;
    
    void setVisible(bool visible) override;
    bool isVisible() const override;
    void setSize(int width, int height) override;
    
    // Dashboard layout
    void addWidget(std::shared_ptr<VisualizationComponent> widget);
    void removeWidget(std::shared_ptr<VisualizationComponent> widget);
    void clearWidgets();
    
    void setLayout(const std::string& layout); // "grid", "tabs", "cascade", "floating"
    void setGridColumns(size_t columns);
    void setGridRows(size_t rows);
    void setWidgetSpacing(int spacing);
    
    // Dashboard controls
    void setAutoRefresh(bool enable);
    void setRefreshInterval(double interval); // seconds
    void setShowStatusBar(bool show);
    void setMinimizeUnseenWidgets(bool minimize);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
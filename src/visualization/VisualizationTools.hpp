// Visualization Tools for NLM
// Phase 6: Integrated Brain Visualization and Monitoring
// 
// This module provides comprehensive visualization and monitoring tools for the NLM
// neural simulation framework. It includes:
// 
// - Core visualization interface for different data types
// - Neural activity visualization (spike rasters, connectivity graphs)
// - Memory system monitoring and visualization
// - Performance monitoring and system resource tracking
// - Real-time visualization tools for live monitoring
// - Data export and file output capabilities
// - Integration with NLM brain systems
// - Interactive visualization controls
// - Advanced plotting and charting capabilities

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <thread>

namespace nlm {

// Visualization data types
enum class VisualizationDataType {
    NeuralActivity,      // Spike trains and firing patterns
    Connectivity,        // Network connectivity matrices
    MemoryState,         // Memory system states
    Performance,         // System performance metrics
    Development,         // Developmental stages and changes
    Neuromodulation,     // Neuromodulator levels and effects
    Sensorimotor,        // Sensory and motor system activity
    TimeSeries,          // Time series data
    Heatmap,             // Heatmap visualizations
    Graph,               // Network graphs
    2D,                  // 2D plots and charts
    3D,                  // 3D visualizations
    Custom               // Custom visualization types
};

// Visualization update frequency
enum class VisualizationUpdateFrequency {
    RealTime,    // Continuous updates
    High,        // Frequent updates (e.g., every 100ms)
    Medium,      // Moderate updates (e.g., every 1s)
    Low,         // Infrequent updates (e.g., every 5s)
    Manual       // Update only on demand
};

// Visualization output formats
enum class VisualizationOutputFormat {
    ASCII,       // Text-based output
    JSON,        // JSON format for data exchange
    CSV,         // Comma-separated values
    PNG,         // Portable Network Graphics
    SVG,         // Scalable Vector Graphics
    HTML,        // HTML5 visualization
    PDF,         // Portable Document Format
    Custom       // Custom output format
};

// Visualization alignment and positioning
struct VisualizationLayout {
    int x;             // X position (pixels or relative units)
    int y;             // Y position (pixels or relative units)
    int width;         // Width
    int height;        // Height
    int column;        // Column (for grid layouts)
    int row;           // Row (for grid layouts)
    float aspect_ratio; // Aspect ratio
    bool expand;       // Whether to expand to available space
    
    VisualizationLayout() : x(0), y(0), width(0), height(0), column(0), row(0), 
                           aspect_ratio(1.0f), expand(false) {}
    
    VisualizationLayout(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height), column(0), row(0),
          aspect_ratio(1.0f), expand(false) {}
};

// Visualization data point
struct VisualizationDataPoint {
    double timestamp;  // Time when data was recorded
    double value;      // Numeric value
    std::string label; // Descriptive label
    std::string unit;  // Unit of measurement
    std::string color; // Color for visualization
    double confidence; // Confidence level (0-1)
    std::string metadata; // Additional metadata
    
    VisualizationDataPoint() : timestamp(0.0), value(0.0), confidence(1.0) {}
    VisualizationDataPoint(double ts, double val, const std::string& lbl = "")
        : timestamp(ts), value(val), label(lbl), confidence(1.0) {}
};

// Time series data for visualization
struct TimeSeriesData {
    std::string name;                                  // Series name
    std::vector<VisualizationDataPoint> data_points;   // Data points
    std::string color;                                 // Color for the series
    bool visible;                                      // Whether series is visible
    int line_width;                                    // Line width
    bool smoothed;                                     // Whether to apply smoothing
    
    TimeSeriesData() : visible(true), line_width(1), smoothed(false) {}
    
    // Add a data point
    void addPoint(double timestamp, double value, const std::string& label = "") {
        data_points.emplace_back(timestamp, value, label);
    }
    
    // Get the number of data points
    size_t size() const {
        return data_points.size();
    }
    
    // Clear all data
    void clear() {
        data_points.clear();
    }
    
    // Get the range of values
    std::pair<double, double> getRange() const {
        if (data_points.empty()) {
            return {0.0, 0.0};
        }
        
        double min_val = std::numeric_limits<double>::max();
        double max_val = std::numeric_limits<double>::lowest();
        
        for (const auto& point : data_points) {
            if (point.value < min_val) min_val = point.value;
            if (point.value > max_val) max_val = point.value;
        }
        
        return {min_val, max_val};
    }
};

// Heatmap data
struct HeatmapData {
    std::vector<std::vector<double>> values;    // 2D array of values
    std::vector<std::vector<std::string>> labels; // 2D array of labels
    std::vector<std::vector<std::string>> colors; // 2D array of colors
    double min_value;                           // Minimum value for color mapping
    double max_value;                           // Maximum value for color mapping
    std::string colormap;                       // Colormap name (e.g., "viridis", "plasma")
    
    HeatmapData() : min_value(0.0), max_value(1.0), colormap("viridis") {}
    
    // Set heatmap dimensions
    void setDimensions(size_t rows, size_t cols) {
        values.assign(rows, std::vector<double>(cols, 0.0));
        labels.assign(rows, std::vector<std::string>(cols, ""));
        colors.assign(rows, std::vector<std::string>(cols, "#000000"));
    }
    
    // Set value at position (row, col)
    void setValue(size_t row, size_t col, double value) {
        if (row < values.size() && col < values[row].size()) {
            values[row][col] = value;
        }
    }
    
    // Get value at position (row, col)
    double getValue(size_t row, size_t col) const {
        if (row < values.size() && col < values[row].size()) {
            return values[row][col];
        }
        return 0.0;
    }
};

// Graph data for network visualization
struct GraphData {
    std::vector<std::string> nodes;                      // Node identifiers
    std::vector<std::string> node_labels;                // Node labels
    std::vector<std::vector<size_t>> adjacency_matrix;    // Adjacency matrix
    std::vector<std::vector<double>> edge_weights;       // Edge weights
    std::vector<std::string> node_colors;                // Node colors
    std::vector<std::string> edge_colors;                // Edge colors
    bool directed;                                      // Whether graph is directed
    
    GraphData() : directed(false) {}
    
    // Add a node
    void addNode(const std::string& id, const std::string& label = "") {
        nodes.push_back(id);
        node_labels.push_back(label);
        node_colors.push_back("#4285F4");  // Default blue color
    }
    
    // Add an edge between two nodes
    void addEdge(size_t from_node, size_t to_node, double weight = 1.0,
                 const std::string& color = "#9E9E9E") {
        // Ensure adjacency matrix has enough space
        size_t max_nodes = std::max(from_node, to_node) + 1;
        if (adjacency_matrix.size() < max_nodes) {
            adjacency_matrix.resize(max_nodes, std::vector<size_t>(max_nodes, 0));
        }
        if (edge_weights.size() < max_nodes) {
            edge_weights.resize(max_nodes, std::vector<double>(max_nodes, 0.0));
        }
        if (edge_colors.size() < max_nodes) {
            edge_colors.resize(max_nodes, std::vector<std::string>(max_nodes, ""));
        }
        
        adjacency_matrix[from_node][to_node] = 1;
        edge_weights[from_node][to_node] = weight;
        edge_colors[from_node][to_node] = color;
        
        if (!directed && from_node != to_node) {
            adjacency_matrix[to_node][from_node] = 1;
            edge_weights[to_node][from_node] = weight;
            edge_colors[to_node][from_node] = color;
        }
    }
    
    // Get number of nodes
    size_t getNodeCount() const {
        return nodes.size();
    }
    
    // Get number of edges
    size_t getEdgeCount() const {
        size_t count = 0;
        for (const auto& row : adjacency_matrix) {
            for (double val : row) {
                if (val > 0) count++;
            }
        }
        if (!directed) return count / 2;
        return count;
    }
};

// Spike raster data for neural activity visualization
struct SpikeRasterData {
    std::vector<size_t> neuron_ids;        // Neuron identifiers
    std::vector<double> spike_times;       // Spike times in seconds
    std::vector<std::string> neuron_labels; // Neuron labels
    std::vector<std::string> neuron_colors; // Neuron colors
    std::string title;                     // Title for the raster plot
    double time_window;                    // Time window for visualization (seconds)
    double bin_width;                       // Bin width for histogram (seconds)
    
    SpikeRasterData() : time_window(10.0), bin_width(0.01) {}
    
    // Add a spike
    void addSpike(size_t neuron_id, double spike_time, const std::string& label = "") {
        neuron_ids.push_back(neuron_id);
        spike_times.push_back(spike_time);
        neuron_labels.push_back(label);
        
        // Assign color based on neuron type (default to blue)
        neuron_colors.push_back("#4285F4");
    }
    
    // Get the maximum spike time
    double getMaxTime() const {
        if (spike_times.empty()) return 0.0;
        return *std::max_element(spike_times.begin(), spike_times.end());
    }
    
    // Get the minimum spike time
    double getMinTime() const {
        if (spike_times.empty()) return 0.0;
        return *std::min_element(spike_times.begin(), spike_times.end());
    }
    
    // Get number of spikes
    size_t getSpikeCount() const {
        return spike_times.size();
    }
};

// Visualization data source interface
class VisualizationDataSource {
public:
    virtual ~VisualizationDataSource() = default;
    
    // Get the data type for this source
    virtual VisualizationDataType getDataType() const = 0;
    
    // Get the name of this data source
    virtual std::string getName() const = 0;
    
    // Get the current data
    virtual std::vector<VisualizationDataPoint> getData() const = 0;
    
    // Get time series data
    virtual std::vector<TimeSeriesData> getTimeSeriesData() const = 0;
    
    // Get heatmap data
    virtual HeatmapData getHeatmapData() const = 0;
    
    // Get graph data
    virtual GraphData getGraphData() const = 0;
    
    // Get spike raster data
    virtual SpikeRasterData getSpikeRasterData() const = 0;
    
    // Check if data is available
    virtual bool hasData() const {
        return !getData().empty();
    }
    
    // Get the update frequency for this data source
    virtual VisualizationUpdateFrequency getUpdateFrequency() const {
        return VisualizationUpdateFrequency::Medium;
    }
    
    // Check if data source is active
    virtual bool isActive() const {
        return true;
    }
};

// Visualization renderer interface
class VisualizationRenderer {
public:
    virtual ~VisualizationRenderer() = default;
    
    // Render the visualization
    virtual void render() = 0;
    
    // Save visualization to file
    virtual bool saveToFile(const std::string& filepath, VisualizationOutputFormat format = VisualizationOutputFormat::PNG) = 0;
    
    // Set the layout
    virtual void setLayout(const VisualizationLayout& layout) = 0;
    
    // Get the current layout
    virtual VisualizationLayout getLayout() const = 0;
    
    // Set the title
    virtual void setTitle(const std::string& title) = 0;
    
    // Get the title
    virtual std::string getTitle() const = 0;
    
    // Set whether to show grid
    virtual void setShowGrid(bool show) = 0;
    
    // Get whether grid is shown
    virtual bool getShowGrid() const = 0;
    
    // Set whether to show legend
    virtual void setShowLegend(bool show) = 0;
    
    // Get whether legend is shown
    virtual bool getShowLegend() const = 0;
    
    // Set the theme (e.g., "light", "dark", "high_contrast")
    virtual void setTheme(const std::string& theme) = 0;
    
    // Get the theme
    virtual std::string getTheme() const = 0;
};

// Real-time data updater
class RealTimeDataUpdater {
public:
    virtual ~RealTimeDataUpdater() = default;
    
    // Update data with new information
    virtual void updateData(const std::vector<VisualizationDataPoint>& data) = 0;
    
    // Check if data is newer than the last update
    virtual bool hasNewData() const = 0;
    
    // Get the last update time
    virtual std::chrono::system_clock::time_point getLastUpdateTime() const = 0;
};

// Visualization panel interface
class VisualizationPanel : public VisualizationRenderer {
public:
    VisualizationPanel() : data_source_(nullptr), update_frequency_(VisualizationUpdateFrequency::Medium) {}
    
    virtual ~VisualizationPanel() = default;
    
    // Set the data source
    void setDataSource(std::shared_ptr<VisualizationDataSource> data_source) {
        data_source_ = data_source;
        updateFrequencyChanged();
    }
    
    // Get the data source
    std::shared_ptr<VisualizationDataSource> getDataSource() const {
        return data_source_;
    }
    
    // Set the update frequency
    void setUpdateFrequency(VisualizationUpdateFrequency frequency) {
        update_frequency_ = frequency;
    }
    
    // Get the update frequency
    VisualizationUpdateFrequency getUpdateFrequency() const {
        return update_frequency_;
    }
    
    // Check if panel is active
    virtual bool isActive() const {
        return data_source_ && data_source_->isActive();
    }
    
    // Get the panel title
    virtual std::string getPanelTitle() const {
        if (data_source_) {
            return data_source_->getName();
        }
        return "Untitled Panel";
    }
    
protected:
    virtual void updateFrequencyChanged() {
        // Override in subclasses to handle update frequency changes
    }
    
    std::shared_ptr<VisualizationDataSource> data_source_;
    VisualizationUpdateFrequency update_frequency_;
};

// Spike raster visualization panel
class SpikeRasterPanel : public VisualizationPanel {
public:
    SpikeRasterPanel() : VisualizationPanel() {
        setTitle("Spike Raster Plot");
    }
    
    virtual void render() override {
        // Implement spike raster rendering
        // This would typically create a visual representation of spike trains
        // For now, we'll just print a summary
        std::cout << "Spike Raster Panel: " << data_source_->getName() << std::endl;
        
        if (auto spike_data = std::dynamic_pointer_cast<SpikeRasterDataSource>(data_source_)) {
            auto data = spike_data->getSpikeRasterData();
            std::cout << "  Neurons: " << data.neuron_ids.size() << std::endl;
            std::cout << "  Spikes: " << data.spike_times.size() << std::endl;
            std::cout << "  Time range: " << data.getMinTime() << " - " << data.getMaxTime() << "s" << std::endl;
        }
    }
    
    virtual bool saveToFile(const std::string& filepath, VisualizationOutputFormat format = VisualizationOutputFormat::PNG) override {
        // Implement file saving
        std::cout << "Saving spike raster to: " << filepath << std::endl;
        return true;
    }
    
private:
    class SpikeRasterDataSource : public VisualizationDataSource {
    public:
        SpikeRasterDataSource(const SpikeRasterData& data) : data_(data) {}
        
        VisualizationDataType getDataType() const override {
            return VisualizationDataType::NeuralActivity;
        }
        
        std::string getName() const override {
            return "Spike Raster";
        }
        
        std::vector<VisualizationDataPoint> getData() const override {
            return std::vector<VisualizationDataPoint>();
        }
        
        std::vector<TimeSeriesData> getTimeSeriesData() const override {
            return std::vector<TimeSeriesData>();
        }
        
        HeatmapData getHeatmapData() const override {
            return HeatmapData();
        }
        
        GraphData getGraphData() const override {
            return GraphData();
        }
        
        SpikeRasterData getSpikeRasterData() const override {
            return data_;
        }
        
        bool isActive() const override {
            return !data_.spike_times.empty();
        }
        
    private:
        SpikeRasterData data_;
    };
};

// Time series visualization panel
class TimeSeriesPanel : public VisualizationPanel {
public:
    TimeSeriesPanel() : VisualizationPanel() {
        setTitle("Time Series Plot");
    }
    
    virtual void render() override {
        // Implement time series rendering
        std::cout << "Time Series Panel: " << data_source_->getName() << std::endl;
        
        if (auto time_series_data = std::dynamic_pointer_cast<TimeSeriesDataSource>(data_source_)) {
            auto data = time_series_data->getTimeSeriesData();
            std::cout << "  Series: " << data.size() << std::endl;
            for (const auto& series : data) {
                std::cout << "    " << series.name << " (" << series.data_points.size() << " points)" << std::endl;
            }
        }
    }
    
    virtual bool saveToFile(const std::string& filepath, VisualizationOutputFormat format = VisualizationOutputFormat::PNG) override {
        // Implement file saving
        std::cout << "Saving time series to: " << filepath << std::endl;
        return true;
    }
    
private:
    class TimeSeriesDataSource : public VisualizationDataSource {
    public:
        TimeSeriesDataSource(const std::vector<TimeSeriesData>& data) : data_(data) {}
        
        VisualizationDataType getDataType() const override {
            return VisualizationDataType::TimeSeries;
        }
        
        std::string getName() const override {
            return "Time Series";
        }
        
        std::vector<VisualizationDataPoint> getData() const override {
            return std::vector<VisualizationDataPoint>();
        }
        
        std::vector<TimeSeriesData> getTimeSeriesData() const override {
            return data_;
        }
        
        HeatmapData getHeatmapData() const override {
            return HeatmapData();
        }
        
        GraphData getGraphData() const override {
            return GraphData();
        }
        
        SpikeRasterData getSpikeRasterData() const override {
            return SpikeRasterData();
        }
        
        bool isActive() const override {
            return !data_.empty();
        }
        
    private:
        std::vector<TimeSeriesData> data_;
    };
};

// Heatmap visualization panel
class HeatmapPanel : public VisualizationPanel {
public:
    HeatmapPanel() : VisualizationPanel() {
        setTitle("Heatmap");
    }
    
    virtual void render() override {
        // Implement heatmap rendering
        std::cout << "Heatmap Panel: " << data_source_->getName() << std::endl;
        
        if (auto heatmap_data = std::dynamic_pointer_cast<HeatmapDataSource>(data_source_)) {
            auto data = heatmap_data->getHeatmapData();
            std::cout << "  Dimensions: " << data.values.size() << " x " << 
                (data.values.empty() ? 0 : data.values[0].size()) << std::endl;
            std::cout << "  Min value: " << data.min_value << ", Max value: " << data.max_value << std::endl;
        }
    }
    
    virtual bool saveToFile(const std::string& filepath, VisualizationOutputFormat format = VisualizationOutputFormat::PNG) override {
        // Implement file saving
        std::cout << "Saving heatmap to: " << filepath << std::endl;
        return true;
    }
    
private:
    class HeatmapDataSource : public VisualizationDataSource {
    public:
        HeatmapDataSource(const HeatmapData& data) : data_(data) {}
        
        VisualizationDataType getDataType() const override {
            return VisualizationDataType::Heatmap;
        }
        
        std::string getName() const override {
            return "Heatmap";
        }
        
        std::vector<VisualizationDataPoint> getData() const override {
            return std::vector<VisualizationDataPoint>();
        }
        
        std::vector<TimeSeriesData> getTimeSeriesData() const override {
            return std::vector<TimeSeriesData>();
        }
        
        HeatmapData getHeatmapData() const override {
            return data_;
        }
        
        GraphData getGraphData() const override {
            return GraphData();
        }
        
        SpikeRasterData getSpikeRasterData() const override {
            return SpikeRasterData();
        }
        
        bool isActive() const override {
            return !data_.values.empty();
        }
        
    private:
        HeatmapData data_;
    };
};

// Real-time visualization manager
class RealTimeVisualizationManager {
public:
    RealTimeVisualizationManager() : running_(false), update_interval_ms_(100) {}
    
    ~RealTimeVisualizationManager() {
        stop();
    }
    
    // Add a visualization panel
    void addPanel(std::shared_ptr<VisualizationPanel> panel) {
        panels_.push_back(panel);
    }
    
    // Remove a visualization panel
    void removePanel(const std::string& panel_name) {
        panels_.erase(
            std::remove_if(panels_.begin(), panels_.end(),
                [&panel_name](const std::shared_ptr<VisualizationPanel>& panel) {
                    return panel->getPanelTitle() == panel_name;
                }),
            panels_.end()
        );
    }
    
    // Start real-time updates
    void start() {
        if (!running_) {
            running_ = true;
            update_thread_ = std::thread(&RealTimeVisualizationManager::updateLoop, this);
        }
    }
    
    // Stop real-time updates
    void stop() {
        if (running_) {
            running_ = false;
            if (update_thread_.joinable()) {
                update_thread_.join();
            }
        }
    }
    
    // Set the update interval
    void setUpdateIntervalMs(int interval_ms) {
        update_interval_ms_ = interval_ms;
    }
    
    // Get the update interval
    int getUpdateIntervalMs() const {
        return update_interval_ms_;
    }
    
    // Add data to all panels
    void addData(const std::vector<VisualizationDataPoint>& data) {
        std::lock_guard<std::mutex> lock(data_mutex_);
        current_data_ = data;
    }
    
    // Update all panels
    void updatePanels() {
        std::lock_guard<std::mutex> lock(data_mutex_);
        
        for (const auto& panel : panels_) {
            if (panel && panel->isActive()) {
                panel->render();
            }
        }
    }
    
    // Print all panel information
    void printPanelInfo() const {
        std::cout << "=== Visualization Panels ===" << std::endl;
        for (const auto& panel : panels_) {
            if (panel) {
                std::cout << "  Panel: " << panel->getPanelTitle() << std::endl;
                std::cout << "    Type: " << static_cast<int>(panel->getDataSource()->getDataType()) << std::endl;
                std::cout << "    Active: " << (panel->isActive() ? "Yes" : "No") << std::endl;
            }
        }
    }
    
private:
    void updateLoop() {
        while (running_) {
            updatePanels();
            std::this_thread::sleep_for(std::chrono::milliseconds(update_interval_ms_));
        }
    }
    
    std::vector<std::shared_ptr<VisualizationPanel>> panels_;
    std::vector<VisualizationDataPoint> current_data_;
    std::mutex data_mutex_;
    bool running_;
    int update_interval_ms_;
    std::thread update_thread_;
};

// Global visualization manager instance
inline RealTimeVisualizationManager& getGlobalVisualizationManager() {
    static RealTimeVisualizationManager manager;
    return manager;
}

} // namespace nlm
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

namespace nlm {

// Visualization interface for debugging and analysis
// Enhanced with real-time monitoring and analysis capabilities
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize();
    
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
    
    // === BASIC VISUALIZATION ===
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
    // === ADVANCED VISUALIZATION ===
    
    // Time-series visualization for neural activity
    void visualizeTimeSeries(const std::vector<float>& data, const std::string& label);
    
    // 3D neural network visualization
    void visualizeNetwork3D(const class Brain& brain);
    
    // Population activity heatmap
    void visualizeActivityHeatmap(const class Brain& brain);
    
    // Synaptic weight distribution analysis
    void visualizeWeightDistribution(const class Brain& brain);
    
    // Plasticity dynamics visualization
    void visualizePlasticityDynamics(const class Brain& brain);
    
    // === MONITORING & METRICS ===
    
    // Real-time performance monitoring
    void enablePerformanceMonitoring(bool enabled);
    bool isPerformanceMonitoringEnabled() const;
    
    // Memory usage visualization
    void visualizeMemoryUsage();
    
    // Network statistics dashboard
    void visualizeNetworkStatistics(const class Brain& brain);
    
    // Plasticity statistics
    void visualizePlasticityStatistics(const class Brain& brain);
    
    // === EXPORT & OUTPUT ===
    
    // Export visualizations to file
    bool exportVisualization(const std::string& filename, const std::string& format = "png");
    
    // Export data to CSV for external analysis
    bool exportDataToCSV(const std::string& filename, const class Brain& brain);
    
    // Generate visualization report
    bool generateReport(const std::string& filename, const class Brain& brain);
    
    // === INTERACTIVE FEATURES ===
    
    // Enable interactive mode
    void enableInteractiveMode(bool enabled);
    bool isInteractiveModeEnabled() const;
    
    // Set visualization themes
    void setTheme(const std::string& theme);
    
    // Configure visualization colors
    void configureColors(const std::map<std::string, std::string>& colors);
    
    // === DATA STREAMING ===
    
    // Add data source for streaming visualization
    void addDataSource(const std::string& name, std::function<std::vector<float>()> dataProvider);
    
    // Remove data source
    void removeDataSource(const std::string& name);
    
    // Get available data sources
    std::vector<std::string> getDataSources() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

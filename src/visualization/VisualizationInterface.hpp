#pragma once

#include <string>
#include <vector>
#include <memory>
#include "AdvancedVisualization.hpp"

using namespace nlm;

// Enhanced visualization interface for debugging and analysis
// Utilizes the advanced visualization system with multiple output formats
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();

    // Initialize visualization with advanced capabilities
    bool initialize(const VisualizationOptions& options = VisualizationOptions());

    // Update visualization data from brain
    void update(const std::shared_ptr<Brain>& brain);

    // Render current state using current visualizer
    void render();

    // Close visualization
    void close();

    // Check if visualization is active
    bool isActive() const;

    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;

    // Network visualization
    void visualizeNetwork(const Brain& brain);

    // Activity visualization
    void visualizeActivity(const Brain& brain);

    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);

    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);

    // Advanced visualization methods
    void enableAdvancedVisualization(bool enable);
    bool isAdvancedVisualizationEnabled() const;

    // Debug tools access
    DebugTools& getDebugTools();

    // Configuration
    void setVisualizationOptions(const VisualizationOptions& options);
    VisualizationOptions getVisualizationOptions() const;

    // Output control
    void saveCurrent(const std::string& filename);
    void saveAll(const std::string& directory);
    void exportData(const std::string& filename, const std::string& format = "csv");

    // Visualization management
    std::string getCurrentVisualizerType() const;
    void setCurrentVisualizer(const std::string& type);

    // Debug mode
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    void setDebugLevel(DebugLevel level);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void setupDefaultVisualizers();
    void createVisualizationData(const std::shared_ptr<Brain>& brain);
};

// Convenience functions for common visualization scenarios
namespace Visualization {
    // Create visualization for specific experiment type
    void createPhase3Visualization(const std::shared_ptr<Brain>& brain);
    void createPhase4Visualization(const std::shared_ptr<Brain>& brain);
    void createPhase6Visualization(const std::shared_ptr<Brain>& brain);

    // Create visualization for specific data type
    void visualizeSpikeRasters(const std::vector<SpikeEvent>& spikes);
    void visualizeNetworkTopology(const Brain& brain);
    void visualizeMemoryActivity(const Brain& brain);
    void visualizeNeuromodulation(const Brain& brain);

    // Create comparison visualizations
    void createPerformanceComparison(const std::vector<std::pair<std::string, double>>& data);
    void createBenchmarkComparison(const std::vector<std::string>& benchmarks);

    // Create debug visualizations
    void createDebugNetwork(const Brain& brain);
    void createDebugMemory(const Brain& brain);
    void createDebugPerformance();
    void createDebugConfiguration();
}


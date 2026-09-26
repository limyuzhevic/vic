#pragma once

#include "NVMVisualization.hpp"
#include "../neuromodulation/NeuromodulationSystem.hpp"
#include "../core/Types/Types.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace nlm {

class NeuromodulationVisualizer : public VisualizationComponent {
public:
    NeuromodulationVisualizer();
    ~NeuromodulationVisualizer() override;
    
    // Initialize with neuromodulation system data
    bool initialize(const NeuromodulationSystem& neuromodulationSystem) override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render neuromodulation visualization
    void render() override;
    
    // Handle input events
    void handleInput(const std::string& eventType, const std::map<std::string, float>& parameters) override;
    
    // Check if visualization is active
    bool isActive() const override;
    
    // Set visibility
    void setVisible(bool visible) override;
    
    // Get component name
    std::string getName() const override;
    
    // Get component bounds
    void getBounds(float& x, float& y, float& width, float& height) const override;
    
    // Set component bounds
    void setBounds(float x, float y, float width, float height) override;
    
    // Set neuromodulator to visualize
    void setNeuromodulator(const std::string& neuromodulator); // "dopamine", "acetylcholine", etc.
    
    // Get current neuromodulator
    std::string getNeuromodulator() const;
    
    // Enable/disable dopamine signal visualization
    void setDopamineSignalVisualizationEnabled(bool enabled);
    
    // Check if dopamine signal visualization is enabled
    bool getDopamineSignalVisualizationEnabled() const;
    
    // Enable/disable novelty detection plot
    void setNoveltyDetectionPlotEnabled(bool enabled);
    
    // Check if novelty detection plot is enabled
    bool getNoveltyDetectionPlotEnabled() const;
    
    // Enable/disable curiosity dynamics visualization
    void setCuriosityDynamicsVisualizationEnabled(bool enabled);
    
    // Check if curiosity dynamics visualization is enabled
    bool getCuriosityDynamicsVisualizationEnabled() const;
    
    // Enable/disable prediction error monitoring
    void setPredictionErrorMonitoringEnabled(bool enabled);
    
    // Check if prediction error monitoring is enabled
    bool getPredictionErrorMonitoringEnabled() const;
    
    // Set visualization style
    void setVisualizationStyle(const std::string& style); // "line", "heatmap", "histogram"
    
    // Get current visualization style
    std::string getVisualizationStyle() const;
    
    // Set time window for visualization
    void setTimeWindow(double seconds);
    
    // Get current time window
    double getTimeWindow() const;
    
    // Get neuromodulation data
    const NeuromodulationData& getNeuromodulationData() const;
    
    // Get novelty detection data
    const std::vector<float>& getNoveltyDetectionData() const;
    
    // Get curiosity dynamics data
    const std::vector<float>& getCuriosityDynamicsData() const;
    
    // Get prediction error data
    const std::vector<float>& getPredictionErrorData() const;
    
    // Get time series data
    const std::vector<float>& getTimeSeriesData() const;
    
    // Get time points
    const std::vector<Timestamp>& getTimePoints() const;
    
    // Enable/disable real-time updates
    void setRealTimeUpdates(bool enabled);
    
    // Check if real-time updates are enabled
    bool getRealTimeUpdates() const;
    
    // Set update frequency
    void setUpdateFrequency(double hz);
    
    // Get current update frequency
    double getUpdateFrequency() const;
    
    // Export visualization data
    bool exportData(const std::string& filename, const std::string& format = "json");
    
    // Import visualization data
    bool importData(const std::string& filename, const std::string& format = "json");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
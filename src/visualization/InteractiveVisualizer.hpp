#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <mutex>
#include <thread>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class VisualizationInterface;
struct RegionId;

enum class VisualizationMode {
    NetworkView,
    ActivityView,
    SpikeView,
    WeightView,
    MetricsView
};

enum class NeuronDisplayType {
    Active,
    Inactive,
    Firing,
    Learning,
    Dead
};

class InteractiveVisualizer {
public:
    InteractiveVisualizer();
    ~InteractiveVisualizer();
    
    // Initialize with brain and visualization interface
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<VisualizationInterface> vizInterface);
    
    // Start interactive mode
    void run();
    
    // Stop interactive mode
    void stop();
    
    // Check if running
    bool isRunning() const;
    
    // Handle keyboard input
    void handleInput(int ch);
    
    // Update display
    void update(double dt);
    
    // Render current frame
    void render();
    
    // Save visualization state
    bool saveState(const std::string& filepath) const;
    
    // Export data
    bool exportData(const std::string& filepath, const std::string& format) const;
    
    // Set visualization mode
    void setMode(VisualizationMode mode);
    
    // Toggle debug information
    void toggleDebugInfo();
    
    // Adjust simulation speed
    void adjustSpeed(int delta);
    
    // Set zoom level
    void setZoom(double level);
    
    // Pan view
    void pan(int dx, int dy);
    
    // Select neuron/region
    void selectRegion(RegionId regionId);
    
    // Get help text
    std::string getHelpText() const;
    
    // Get current metrics
    std::map<std::string, std::string> getMetrics() const;
    
    // Get selected regions
    const std::vector<RegionId>& getSelectedRegions() const;
    
    // Set neural activity filter
    void setActivityFilter(float threshold);
    
    // Set developmental stage filter
    void setDevelopmentalStageFilter(DevelopmentalStage stage);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set view focus
    void setViewFocus(const std::string& focusType);
    
    // Get current simulation speed
    int getSpeed() const;
    
    // Get zoom level
    double getZoom() const;
    
    // Get pan offset
    std::pair<int, int> getPan() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
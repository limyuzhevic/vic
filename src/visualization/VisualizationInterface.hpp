#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>

namespace nlm {

// VisualizationInterface: Real-time visualization of neural activity and system state
//
// Provides comprehensive visualization capabilities including:
// - Network topology displays (brain regions and connectivity)
// - Activity maps (real-time neural firing patterns)
// - Spike raster plots (spike timing and patterns)
// - Weight distribution histograms (synaptic weight analysis)
// - System statistics (performance metrics and indicators)
// - Region information (detailed regional statistics)
// - Neurons per region displays (population visualization)
//
// Features:
// - Real-time updates synchronized with simulation
// - Console-based ASCII art visualization
// - Color-coded activity representations
// - Multiple visualization modes
// - Configurable update rates
// - Export capabilities for analysis

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialization
    bool initialize();
    
    // Main update/render loop
    void update();
    void render();
    
    // Control methods
    void close();
    bool isActive() const;
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Visualization targets
    void visualizeNetwork(const Brain& brain);
    void visualizeActivity(const Brain& brain);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain);
    
    // Configuration methods
    void setShowNetworkTopology(bool show) { pImpl->showNetworkTopology = show; }
    void setShowActivityMaps(bool show) { pImpl->showActivityMaps = show; }
    void setShowSpikeRasters(bool show) { pImpl->showSpikeRasters = show; }
    void setShowWeightDistributions(bool show) { pImpl->showWeightDistributions = show; }
    void setShowStatistics(bool show) { pImpl->showStatistics = show; }
    void setShowRegionInfo(bool show) { pImpl->showRegionInfo = show; }
    void setShowNeuronsPerRegion(bool show) { pImpl->showNeuronsPerRegion = show; }
    void setUseColors(bool use) { pImpl->useColors = use; }
    
    // Get configuration
    bool getShowNetworkTopology() const { return pImpl->showNetworkTopology; }
    bool getShowActivityMaps() const { return pImpl->showActivityMaps; }
    bool getShowSpikeRasters() const { return pImpl->showSpikeRasters; }
    bool getShowWeightDistributions() const { return pImpl->showWeightDistributions; }
    bool getShowStatistics() const { return pImpl->showStatistics; }
    bool getShowRegionInfo() const { return pImpl->showRegionInfo; }
    bool getShowNeuronsPerRegion() const { return pImpl->showNeuronsPerRegion; }
    bool getUseColors() const { return pImpl->useColors; }
    
    // Console size configuration
    void setConsoleSize(int width, int height) {
        pImpl->consoleWidth = width;
        pImpl->consoleHeight = height;
    }
    
protected:
    // Visualization rendering methods (protected for extensibility)
    void printHeader();
    int renderNetworkTopology(int startLine);
    int renderActivityMaps(int startLine);
    int renderSpikeRasters(int startLine);
    int renderWeightDistributions(int startLine);
    int renderStatistics(int startLine);
    int renderRegionInfo(int startLine);
    int renderNeuronsPerRegion(int startLine);
    void printFooter(int currentLine);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Reference to current brain for visualization
    const Brain* brain_;
    
    // Helper methods
    const char* stageToString(DevelopmentalStage stage) const;
    const char* motorCommandToString(MotorCommand cmd) const;
};

} // namespace nlm

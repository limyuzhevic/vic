#pragma once

#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../dynamics/NeuralDynamics.hpp"
#include "../neuron/Neuron.hpp"
#include "../synapse/Synapse.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <numeric>

namespace nlm {

class EnhancedVisualization : public VisualizationInterface {
public:
    EnhancedVisualization();
    ~EnhancedVisualization() override;
    
    // Initialize with window settings
    bool initialize(int width = 1600, int height = 900, 
                   const std::string& title = "NLM Neural Network Visualization") override;
    
    // Real-time update with neuroscience data
    void update() override;
    
    // Render all visualization components
    void render() override;
    
    // Close and cleanup
    void close() override;
    
    // Real-time monitoring controls
    void setUpdateRate(double hz) override;
    void setSimulationSpeed(double speed);
    void togglePause();
    bool isPaused() const;
    
    // Network visualization methods
    void visualizeNetwork(const Brain& brain) override;
    void visualizeNetworkGraph(const Brain& brain);
    void visualizeRegionConnections(const Brain& brain);
    
    // Activity visualization methods
    void visualizeActivity(const Brain& brain) override;
    void visualizeFiringRateHeatmap(const Brain& brain);
    void visualizeSpikingActivity(const Brain& brain);
    void visualizeSynapticActivity(const Brain& brain);
    
    // Spike visualization
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes) override;
    void renderSpikeRasterPlot(const std::vector<SpikeEvent>& spikes);
    void renderSpikingHeatmap(const std::vector<SpikeEvent>& spikes, double timeWindow);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain) override;
    void renderWeightMatrix(const Brain& brain);
    void renderConnectionDensityMap(const Brain& brain);
    
    // Neural state inspection
    void inspectNeuralState(const Brain& brain);
    void displayNeuronDetails(const Neuron& neuron);
    void displaySynapseDetails(const Synapse& synapse);
    
    // Memory profiling
    void updateMemoryUsage();
    void renderMemoryProfiler();
    
    // Performance analysis
    void updatePerformanceMetrics();
    void renderPerformanceDashboard();
    void detectPerformanceBottlenecks();
    
    // Event timeline visualization
    void addEvent(const std::string& eventType, const std::string& description, double timestamp);
    void renderEventTimeline();
    
    // Debug controls
    void toggleDebugMode();
    void toggleGrid(bool show);
    void toggleLabels(bool show);
    void setColorScheme(int scheme);
    
    // Export capabilities
    void exportVisualizationData(const std::string& filename);
    void exportFrame(int frameIndex);
    
    // Interactive features
    bool handleInput();
    void processZoom(int delta);
    void processPan(float dx, float dy);
    void processSelection(int neuronId);
    
    // Data management
    void clearEvents();
    void resetMetrics();
    
    // Python integration helpers
    void updateFromPython(const py::dict& data);
    py::dict getVisualizationState() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

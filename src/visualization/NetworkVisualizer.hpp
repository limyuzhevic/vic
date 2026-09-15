#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace nlm {

// Forward declarations
class Brain;
class VisualizationEngine;

class NetworkVisualizer {
public:
    NetworkVisualizer();
    ~NetworkVisualizer();
    
    // Initialize the visualizer
    bool initialize(VisualizationEngine* engine);
    
    // Update network visualization data
    void update(const Brain& brain, double timestep);
    
    // Render the network visualization
    void render();
    
    // Close and cleanup
    void close();
    
    // Configuration
    void setShowNeurons(bool show);
    void setShowConnections(bool show);
    void setNeuronRadius(float radius);
    void setConnectionThickness(float thickness);
    void setColorScheme(int scheme);
    void setNeuronLabels(bool show);
    void setBackgroundColor(float r, float g, float b, float a = 1.0f);
    void setZoom(float factor);
    void setRotation(float x, float y, float z);
    void setProjectionMode(int mode);
    
    // Region and neuron selection
    void selectRegion(int regionId);
    void selectNeuron(int neuronId);
    void clearSelection();
    
    // Data export
    bool exportNetwork(const std::string& filepath, ExportFormat format);
    
    // Getters
    bool isActive() const;
    std::vector<int> getSelectedNeurons() const;
    std::vector<int> getSelectedRegions() const;
    
    // Statistics
    struct NetworkStats {
        int totalNeurons;
        int activeNeurons;
        int totalConnections;
        float averageWeight;
        float excitationInhibitionRatio;
        std::map<NeuronType, int> neuronCounts;
    };
    
    NetworkStats getStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

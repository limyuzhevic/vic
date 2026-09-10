#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../brain/Brain.hpp"

namespace nlm {

// Neural network visualization system
class NetworkVisualizer {
public:
    NetworkVisualizer();
    ~NetworkVisualizer();
    
    // Network visualization methods
    struct NetworkLayout {
        std::vector<std::vector<float>> positions;      // Node positions (x, y)
        std::vector<std::vector<uint64_t>> adjacency;   // Adjacency matrix
        std::vector<std::string> nodeLabels;            // Node labels
        std::vector<std::string> edgeLabels;            // Edge labels
        std::map<std::string, std::vector<float>> nodeAttributes; // Node attributes
        std::map<std::string, std::vector<float>> edgeAttributes; // Edge attributes
    };
    
    NetworkLayout createLayout(const std::shared_ptr<Brain>& brain,
                              const std::string& layoutType = "spring") const;
    
    void drawNetwork(const std::shared_ptr<Brain>& brain,
                    const NetworkLayout& layout,
                    const std::string& outputFile = "network.png");
    
    void drawActivity(const std::shared_ptr<Brain>& brain,
                     const NetworkLayout& layout,
                     double timeWindow = 1.0,
                     const std::string& outputFile = "activity.png");
    
    // Spike visualization
    struct SpikeVisualization {
        std::vector<std::vector<float>> spikeRaster;     // Raster plot data
        std::vector<float> spikeTimes;                  // Spike times
        std::vector<uint64_t> neuronIds;                 // Neuron IDs
        float timeScale;                                 // Time scale for display
        float heightScale;                               // Height scale for display
        std::map<std::string, std::vector<float>> styling; // Styling information
    };
    
    SpikeVisualization createSpikeRaster(const std::vector<SpikeEvent>& spikes,
                                        size_t maxSpikes = 1000) const;
    
    void drawSpikeRaster(const SpikeVisualization& spikes,
                        const std::string& outputFile = "spike_raster.png");
    
    // Weight matrix visualization
    struct WeightVisualization {
        std::vector<std::vector<float>> weightMatrix;    // Weight matrix
        float minWeight;                                 // Minimum weight for coloring
        float maxWeight;                                 // Maximum weight for coloring
        std::string colormap;                            // Colormap name
        bool showWeights;                                // Whether to show weight values
        bool showThreshold;                              // Whether to show threshold crossings
        std::map<std::string, std::vector<float>> weightAttributes;
    };
    
    WeightVisualization createWeightMatrix(const std::shared_ptr<Brain>& brain,
                                          float threshold = 0.1) const;
    
    void drawWeights(const WeightVisualization& weights,
                    const NetworkLayout& layout,
                    const std::string& outputFile = "weights.png");
    
    // Advanced visualization features
    void saveVisualization(const std::string& filename,
                          const std::string& format = "png",
                          const std::map<std::string, std::string>& options = {});
    
    void createAnimation(const std::vector<NetworkLayout>& frames,
                        double fps = 10.0,
                        const std::string& outputFile = "animation.gif");
    
    void createInteractivePlot(const std::shared_ptr<Brain>& brain,
                              const std::string& title = "Neural Network");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

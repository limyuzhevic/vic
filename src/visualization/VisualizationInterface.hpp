#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"

namespace nlm {

// Visualization interface for debugging and analysis
// Provides visualization capabilities for brain state, memory, and neural activity

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize();
    
    // Update visualization data
    void update(const Brain& brain);
    
    // Render current state
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
    
    // Memory visualization
    void visualizeWorkingMemory(const NeuralWorkingMemory& memory);
    void visualizeEpisodicMemory(const NeuralEpisodicMemory& memory);
    
    // Prediction system visualization
    void visualizePredictionSystem(const PredictionSystem& prediction);
    
    // Neuromodulation visualization
    void visualizeNeuromodulation(const Dopamine& dopamine, 
                                 const Curiosity& curiosity,
                                 const Novelty& novelty);
    
    // Export visualization to file
    bool exportVisualization(const std::string& filename, VisualizationFormat format = VisualizationFormat::PNG);
    
    // Save current frame as image
    bool saveFrame(const std::string& filename);
    
    // Get visualization statistics
    struct VisualizationStats {
        size_t neurons;
        size_t synapses;
        size_t activeNeurons;
        size_t activeTraces;
        float averageFiringRate;
        size_t episodeCount;
        float totalMemoryActivity;
        size_t recordingCount;
        
        VisualizationStats() : neurons(0), synapses(0), activeNeurons(0), 
                              activeTraces(0), averageFiringRate(0), 
                              episodeCount(0), totalMemoryActivity(0), 
                              recordingCount(0) {}
    };
    
    VisualizationStats getStats() const;
    
    // Enable/disable specific visualization components
    void enableNetworkVisualization(bool enable) { networkVisualizationEnabled_ = enable; }
    void enableActivityVisualization(bool enable) { activityVisualizationEnabled_ = enable; }
    void enableMemoryVisualization(bool enable) { memoryVisualizationEnabled_ = enable; }
    void enablePredictionVisualization(bool enable) { predictionVisualizationEnabled_ = enable; }
    
    // Get debug info
    std::string getDebugInfo() const;
    
    // Reset visualization
    void reset();
    
    // Test visualization functionality
    bool testVisualization() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Visualization options
    bool networkVisualizationEnabled_;
    bool activityVisualizationEnabled_;
    bool memoryVisualizationEnabled_;
    bool predictionVisualizationEnabled_;
    bool spikeVisualizationEnabled_;
    bool weightVisualizationEnabled_;
    
    // Internal data storage
    std::vector<std::vector<float>> networkData_;
    std::vector<float> activityData_;
    std::vector<std::string> spikeData_;
    std::vector<std::vector<float>> weightData_;
};

// Visualization format enum
enum class VisualizationFormat {
    PNG,
    JPEG,
    BMP,
    SVG,
    PDF
};

// Forward declarations for visualization components
struct SpikeEvent;

} // namespace nlm

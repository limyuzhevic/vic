#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class SpikeEvent;

// Visualization modes
enum class VisualizationMode {
    Network,
    Activity,
    Spikes,
    Weights,
    Connectivity,
    Development,
    Neuromodulation,
    Memory
};

// Visualization data structures
struct NetworkData {
    std::vector<std::pair<float, float>> neuronPositions;
    std::vector<std::pair<size_t, size_t>> connections;
    std::vector<float> neuronExcitability;
    std::vector<float> neuronThickness;
};

struct ActivityData {
    std::vector<double> firingRates;
    std::vector<float> membranePotentials;
    std::vector<size_t> firingCounts;
    double averageFiringRate;
    double maximumFiringRate;
};

struct SpikeData {
    std::vector<std::pair<double, size_t>> spikeTimestamps;
    std::vector<size_t> neuronIds;
    size_t totalSpikes;
    double rateHz;
};

struct WeightData {
    std::vector<std::vector<float>> weightMatrix;
    std::vector<size_t> neuronIndices;
    float minWeight;
    float maxWeight;
    float averageWeight;
};

// Enhanced visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Disable copying, enable moving
    VisualizationInterface(const VisualizationInterface&) = delete;
    VisualizationInterface& operator=(const VisualizationInterface&) = delete;
    VisualizationInterface(VisualizationInterface&&) noexcept;
    VisualizationInterface& operator=(VisualizationInterface&&) noexcept;
    
    // Initialize visualization
    bool initialize(VisualizationMode mode = VisualizationMode::Network);
    
    // Update visualization data
    void update(const Brain& brain, double timestep = 0.001);
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Set visualization mode
    void setMode(VisualizationMode mode);
    VisualizationMode getMode() const;
    
    // Export data for external analysis
    bool exportData(const std::string& filename, VisualizationMode mode = VisualizationMode::Network);
    
    // Get current visualization data
    NetworkData getNetworkData() const;
    ActivityData getActivityData() const;
    SpikeData getSpikeData() const;
    WeightData getWeightData() const;
    
    // Set callbacks for interactive debugging
    void setUpdateCallback(std::function<void()> callback);
    void setClickCallback(std::function<void(size_t neuronId)> callback);
    void setHoverCallback(std::function<void(size_t neuronId)> callback);
    
    // Debug information
    std::string getDebugInfo() const;
    void logDebugInfo(const std::string& message);
    
    // Performance monitoring
    void startPerformanceTimer();
    double getLastFrameTime() const;
    double getAverageFrameTime() const;
    
    // Statistical analysis
    double getFiringRateDistribution() const;
    std::vector<size_t> getNeuronFiringOrder() const;
    std::vector<float> getWeightHistory(size_t synapseId) const;
    
    // Network analysis
    size_t getNetworkDensity() const;
    size_t getClusterCount() const;
    std::vector<std::vector<size_t>> getCommunities() const;
    
    // Neural state analysis
    std::vector<float> getMembranePotentialDistribution() const;
    std::vector<float> getRefractoryPeriodDistribution() const;
    
    // Memory system analysis
    size_t getWorkingMemorySize() const;
    size_t getEpisodicMemoryCount() const;
    
    // Neuromodulation analysis
    double getDopamineLevel() const;
    double getCuriosityLevel() const;
    double getNoveltyLevel() const;
    
    // Development stage analysis
    std::string getDevelopmentalStageInfo() const;
    
    // Save and restore visualization state
    bool saveState(const std::string& filename);
    bool loadState(const std::string& filename);
    
    // Network visualization
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    
    // Connectivity visualization
    void visualizeConnectivity(const Brain& brain);
    
    // Development visualization
    void visualizeDevelopment(const Brain& brain);
    
    // Neuromodulation visualization
    void visualizeNeuromodulation(const Brain& brain);
    
    // Memory visualization
    void visualizeMemory(const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

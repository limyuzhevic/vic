#pragma once

#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <algorithm>

namespace nlm {

class NeuralActivityVisualizer : public VisualizationInterface {
public:
    NeuralActivityVisualizer();
    ~NeuralActivityVisualizer() override;
    
    // Initialize visualization
    bool initialize() override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render current state
    void render() override;
    
    // Close visualization
    void close() override;
    
    // Set brain reference
    void setBrain(std::shared_ptr<Brain> brain);
    
    // Set spike system reference
    void setSpikeSystem(std::shared_ptr<SpikeSystem> spikeSystem);
    
    // Set update interval for data sampling
    void setSamplingInterval(double seconds);
    
    // Set activity threshold for highlighting
    void setActivityThreshold(float threshold);
    
    // Get recent spike history
    const std::deque<std::pair<SimulationStep, NeuronId>>& getRecentSpikes() const;
    
    // Get neuron activity levels
    const std::unordered_map<NeuronId, float>& getNeuronActivity() const;
    
    // Get firing rate statistics
    struct FiringRateStats {
        float average;
        float maximum;
        float minimum;
        float standardDeviation;
    };
    
    FiringRateStats getFiringRateStats() const;
    
    // Get activity heatmap data (region to activity level)
    const std::unordered_map<RegionId, float>& getActivityHeatmap() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

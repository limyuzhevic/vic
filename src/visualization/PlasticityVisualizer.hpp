#pragma once

#include "NVMVisualization.hpp"
#include "../plasticity/PlasticitySystem.hpp"
#include "../core/Types/Types.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace nlm {

class PlasticityVisualizer : public VisualizationComponent {
public:
    PlasticityVisualizer();
    ~PlasticityVisualizer() override;
    
    // Initialize with plasticity system data
    bool initialize(const PlasticitySystem& plasticitySystem) override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render plasticity visualization
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
    
    // Set plasticity type to visualize
    void setPlasticityType(const std::string& type); // "hebbian", "stdp", "reward_modulated"
    
    // Get current plasticity type
    std::string getPlasticityType() const;
    
    // Enable/disable STDP learning dynamics visualization
    void setSTDPVisualizationEnabled(bool enabled);
    
    // Check if STDP visualization is enabled
    bool getSTDPVisualizationEnabled() const;
    
    // Enable/disable Hebbian learning visualization
    void setHebbianLearningVisualizationEnabled(bool enabled);
    
    // Check if Hebbian learning visualization is enabled
    bool getHebbianLearningVisualizationEnabled() const;
    
    // Enable/disable reward-modulated learning visualization
    void setRewardModulatedLearningVisualizationEnabled(bool enabled);
    
    // Check if reward-modulated learning visualization is enabled
    bool getRewardModulatedLearningVisualizationEnabled() const;
    
    // Enable/disable multi-timescale plasticity visualization
    void setMultiTimescalePlasticityVisualizationEnabled(bool enabled);
    
    // Check if multi-timescale plasticity visualization is enabled
    bool getMultiTimescalePlasticityVisualizationEnabled() const;
    
    // Set visualization style
    void setVisualizationStyle(const std::string& style); // "timeline", "heatmap", "graph"
    
    // Get current visualization style
    std::string getVisualizationStyle() const;
    
    // Set time window for visualization
    void setTimeWindow(double seconds);
    
    // Get current time window
    double getTimeWindow() const;
    
    // Get plasticity data
    const PlasticityData& getPlasticityData() const;
    
    // Get STDP learning dynamics data
    const std::vector<std::pair<Timestamp, float>>& getSTDPData() const;
    
    // Get Hebbian learning data
    const std::vector<std::pair<Timestamp, float>>& getHebbianData() const;
    
    // Get reward-modulated learning data
    const std::vector<std::pair<Timestamp, float>>& getRewardModulatedData() const;
    
    // Get multi-timescale plasticity data
    const std::vector<std::pair<Timestamp, float>>& getMultiTimescaleData() const;
    
    // Get synaptic weight data
    const std::vector<float>& getSynapticWeights() const;
    
    // Get eligibility trace data
    const std::vector<float>& getEligibilityTraces() const;
    
    // Get reward prediction data
    const std::vector<float>& getRewardPredictions() const;
    
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
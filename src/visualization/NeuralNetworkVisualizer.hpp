#pragma once

#include "NVMVisualization.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace nlm {

class NeuralNetworkVisualizer : public VisualizationComponent {
public:
    NeuralNetworkVisualizer();
    ~NeuralNetworkVisualizer() override;
    
    // Initialize with brain data
    bool initialize(const Brain& brain) override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render neural network visualization
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
    
    // Set visualization mode
    void setMode(const std::string& mode); // "3d", "2d", "topological", "activity"
    
    // Get current visualization mode
    std::string getMode() const;
    
    // Enable/disable live updates
    void setLiveUpdates(bool enabled);
    
    // Check if live updates are enabled
    bool getLiveUpdates() const;
    
    // Set neuron size scaling
    void setNeuronSizeScaling(float scale);
    
    // Get neuron size scaling
    float getNeuronSizeScaling() const;
    
    // Set synapse width scaling
    void setSynapseWidthScaling(float scale);
    
    // Get synapse width scaling
    float getSynapseWidthScaling() const;
    
    // Set color mapping for neuron activity
    void setActivityColorMapping(const std::string& mapping); // "linear", "logarithmic", "threshold"
    
    // Get current color mapping
    std::string getActivityColorMapping() const;
    
    // Enable/disable neuron selection
    void setNeuronSelectionEnabled(bool enabled);
    
    // Check if neuron selection is enabled
    bool getNeuronSelectionEnabled() const;
    
    // Enable/disable synapse highlighting
    void setSynapseHighlightingEnabled(bool enabled);
    
    // Check if synapse highlighting is enabled
    bool getSynapseHighlightingEnabled() const;
    
    // Highlight specific neurons
    void highlightNeurons(const std::vector<NeuronId>& neuronIds, bool highlight = true);
    
    // Highlight specific synapses
    void highlightSynapses(const std::vector<SynapseId>& synapseIds, bool highlight = true);
    
    // Get highlighted neurons
    const std::vector<NeuronId>& getHighlightedNeurons() const;
    
    // Get highlighted synapses
    const std::vector<SynapseId>& getHighlightedSynapses() const;
    
    // Enable/disable spike visualization
    void setSpikeVisualizationEnabled(bool enabled);
    
    // Check if spike visualization is enabled
    bool getSpikeVisualizationEnabled() const;
    
    // Enable/disable weight visualization
    void setWeightVisualizationEnabled(bool enabled);
    
    // Check if weight visualization is enabled
    bool getWeightVisualizationEnabled() const;
    
    // Enable/disable regional visualization
    void setRegionalVisualizationEnabled(bool enabled);
    
    // Check if regional visualization is enabled
    bool getRegionalVisualizationEnabled() const;
    
    // Set regional visualization style
    void setRegionalVisualizationStyle(const std::string& style); // "color", "border", "gradient"
    
    // Get current regional visualization style
    std::string getRegionalVisualizationStyle() const;
    
    // Get neuron activity heatmap data
    const std::vector<std::vector<float>>& getActivityHeatmap() const;
    
    // Get synapse strength visualization data
    const std::vector<std::vector<float>>& getSynapseStrengthMatrix() const;
    
    // Get network topology data
    const NetworkTopology& getNetworkTopology() const;
    
    // Export visualization data
    bool exportData(const std::string& filename, const std::string& format = "json");
    
    // Import visualization data
    bool importData(const std::string& filename, const std::string& format = "json");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
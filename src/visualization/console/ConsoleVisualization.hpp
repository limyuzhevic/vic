#pragma once

#include "../VisualizationInterface.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

enum class VisualizationColorMode {
    Monochrome,
    Heatmap,
    Category,
    Gradient
};

class ConsoleVisualization : public VisualizationInterface {
public:
    ConsoleVisualization();
    ~ConsoleVisualization() override;
    
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Neural network visualization
    void visualizeNetwork(const Brain& brain) override;
    void visualizeActivity(const Brain& brain) override;
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes) override;
    void visualizeWeights(const Brain& brain) override;
    
    // Brain state visualization
    void visualizeBrainStates(const Brain& brain);
    void visualizeNeuromodulation(const Brain& brain);
    void visualizePredictionErrors(const Brain& brain);
    void visualizeAttention(const Brain& brain);
    
    // Agent interaction visualization
    void visualizeSensoryInput(const Brain& brain);
    void visualizeMotorCommands(const Brain& brain);
    void visualizeAgentPosition(const Brain& brain);
    void visualizeExploration(const Brain& brain);
    
    // Experimental results visualization
    void plotLearningCurves(const std::vector<double>& weights,
                           const std::vector<double>& rewards,
                           const std::vector<double>& errors);
    void visualizeMemoryConsolidation(const Brain& brain);
    void visualizeDevelopment(const Brain& brain);
    
    // Configuration
    void setEnableNetworkViz(bool enable);
    void setEnableActivityViz(bool enable);
    void setEnableSpikesViz(bool enable);
    void setEnableWeightsViz(bool enable);
    void setEnableBrainStatesViz(bool enable);
    void setEnableAgentInteractionViz(bool enable);
    void setEnableResultsViz(bool enable);
    
    void setMaxSpikesDisplay(size_t maxSpikes);
    void setMaxNetworkNodes(size_t maxNodes);
    void setColorMode(VisualizationColorMode mode);
    
    // Console control
    void clearScreen() const;
    void setCursorPosition(int x, int y) const;
    void printHeader(const std::string& title) const;
    void printStats(const Brain& brain, float simTime) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
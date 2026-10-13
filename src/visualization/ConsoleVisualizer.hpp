// NLM Console Visualizer - Basic console-based visualization for NLM
// Provides simple visualization capabilities for debugging and monitoring

#pragma once

#include "visualization/VisualizationInterface.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <vector>
#include <string>
#include <memory>
#include <mutex>

namespace nlm {

// Console visualization mode
enum class ConsoleVisualizationMode {
    Basic,          // Simple neuron/synapse state display
    Activity,       // Focus on activity and spikes
    Network,        // Network connectivity visualization
    Learning,       // Highlight learning-related activity
    Detailed        // Detailed neuron state information
};

// Console visualizer implementation
class ConsoleVisualizer : public VisualizationInterface {
public:
    ConsoleVisualizer();
    ~ConsoleVisualizer() override;

    // VisualizationInterface implementation
    bool initialize() override;
    void shutdown() override;
    void update(double time, const std::shared_ptr<Brain>& brain) override;
    void render() override;
    bool isInitialized() const override;
    void setMode(VisualizationMode mode) override;
    void setScale(float scale) override;
    void setColorScheme(const std::string& scheme) override;

    // Console-specific settings
    void setRefreshRate(int fps);
    void setMaxNeuronsToDisplay(size_t maxNeurons);
    void setHighlightActiveOnly(bool highlightActive);
    void setShowStatistics(bool showStats);
    void setAnimationSpeed(float speed);
    
    // Console output methods
    void clearScreen() const;
    void drawNeuronGrid(const std::vector<Neuron*>& neurons) const;
    void drawActivityHeatmap(const std::vector<Neuron*>& neurons) const;
    void drawNetworkConnections(const std::vector<Synapse*>& synapses) const;
    void drawStatistics(const std::shared_ptr<Brain>& brain, double time) const;
    void drawPerformanceMetrics() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Console-specific drawing helpers
    void drawNeuron(Neuron* neuron, int x, int y, int size) const;
    void drawSynapse(Synapse* synapse, int x1, int y1, int x2, int y2) const;
    void drawText(int x, int y, const std::string& text, const std::string& color = "white") const;
    std::string getNeuronColor(Neuron* neuron) const;
    std::string getActivityColor(float activity) const;
    std::string getSynapseColor(Synapse* synapse) const;
    
    // Console output buffering
    void flushOutput() const;
    void updateDisplay() const;
};

} // namespace nlm

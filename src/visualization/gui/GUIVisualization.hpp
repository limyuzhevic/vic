// GUI Visualization using GLFW/SDL backend
#pragma once

#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

enum class GUIBackend {
    GLFW,
    SDL,
    Both
};

enum class GUILayout {
    Horizontal,
    Vertical,
    Grid,
    Tabbed
};

enum class GUIElementType {
    Label,
    Button,
    Plot,
    Text,
    ProgressBar,
    Gauge,
    Panel,
    TreeView
};

class GUIVisualization : public VisualizationInterface {
public:
    GUIVisualization();
    ~GUIVisualization() override;
    
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Configuration
    void setBackend(GUIBackend backend);
    void setLayout(GUILayout layout);
    void setTheme(const std::string& theme);
    
    // Window management
    void setWindowTitle(const std::string& title);
    void setWindowSize(int width, int height);
    void setWindowPosition(int x, int y);
    
    // Layout management
    void setLayoutMargin(float margin);
    void setLayoutPadding(float padding);
    void setLayoutSpacing(float spacing);
    
    // Element creation and management
    void addElement(GUIElementType type, const std::string& id, const std::string& label);
    void removeElement(const std::string& id);
    void clearElements();
    
    // Visualization configuration
    void configureNetworkPanel(const std::string& panelId, bool showNeurons = true, 
                             bool showConnections = true, bool showActivity = true);
    void configureActivityPanel(const std::string& panelId, bool showSpikes = true,
                               bool showRates = true, bool showHistograms = true);
    void configureBrainStatesPanel(const std::string& panelId, bool showMemory = true,
                                 bool showNeuromodulation = true, bool showPrediction = true);
    void configureAgentPanel(const std::string& panelId, bool showSensory = true,
                           bool showMotor = true, bool showExploration = true);
    void configureResultsPanel(const std::string& panelId, bool showLearning = true,
                             bool showConsolidation = true, bool showDevelopment = true);
    
    // Data binding
    void bindBrainData(const std::string& panelId, const class Brain& brain);
    void bindSpikeData(const std::string& panelId, const std::vector<SpikeEvent>& spikes);
    void bindLearningData(const std::string& panelId, const std::vector<double>& weights,
                         const std::vector<double>& rewards, const std::vector<double>& errors);
    void bindMetricsData(const std::string& panelId, float totalReward, float avgFiringRate,
                       float noveltyLevel, float curiosityLevel, float dopamineLevel);
    
    // Visualization control
    void enableVisualization(const std::string& panelId, bool enable);
    void setVisualizationOpacity(const std::string& panelId, float opacity);
    void setVisualizationColor(const std::string& panelId, const std::string& color);
    
    // Interaction
    bool isElementHovered(const std::string& id) const;
    bool isElementClicked(const std::string& id) const;
    std::string getHoveredElement() const;
    
    // Save/Load visualization state
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
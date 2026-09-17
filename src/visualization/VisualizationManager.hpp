// Main visualization manager and factory for NLM
#pragma once

#include "VisualizationInterface.hpp"
#include "console/ConsoleVisualization.hpp"
#include "gui/GUIVisualization.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

enum class VisualizationBackend {
    None,
    Console,
    GUI,
    Both
};

enum class VisualizationMode {
    Disabled,
    ConsoleOnly,
    GUIOnly,
    BothEnabled
};

class VisualizationManager {
public:
    VisualizationManager();
    ~VisualizationManager();
    
    // Initialize visualization system
    bool initialize(const std::string& configFile = "");
    
    // Update all active visualizations
    void update();
    
    // Render all active visualizations
    void render();
    
    // Close all visualizations
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Configuration
    void setBackend(VisualizationBackend backend);
    void setMode(VisualizationMode mode);
    void setPreferredGUI(std::string backend = "glfw");
    
    // Toggle individual visualization types
    void enableConsoleVisualization(bool enable);
    void enableGUIVisualization(bool enable);
    
    // Console-specific configuration
    void configureConsole(bool enableNetworkViz, bool enableActivityViz, bool enableSpikesViz,
                         bool enableWeightsViz, bool enableBrainStatesViz,
                         bool enableAgentInteractionViz, bool enableResultsViz);
    void setConsoleMaxSpikes(size_t maxSpikes);
    void setConsoleMaxNetworkNodes(size_t maxNodes);
    void setConsoleColorMode(VisualizationColorMode mode);
    
    // GUI-specific configuration
    void configureGUI(std::string backend, int width, int height, int x, int y);
    void setGUITheme(const std::string& theme);
    void setGUILayout(GUILayout layout);
    
    // Add visualization panels to GUI
    void addGUIPanel(const std::string& panelId, GUIElementType elementType, const std::string& label);
    void removeGUIPanel(const std::string& panelId);
    void configureGUIPanel(const std::string& panelId, const std::string& type,
                          const std::unordered_map<std::string, bool>& config);
    
    // Bind data to visualizations
    void bindBrainData(const Brain& brain);
    void bindSpikeData(const std::vector<SpikeEvent>& spikes);
    void bindLearningData(const std::vector<double>& weights,
                         const std::vector<double>& rewards,
                         const std::vector<double>& errors);
    void bindMetricsData(float totalReward, float avgFiringRate,
                        float noveltyLevel, float curiosityLevel, float dopamineLevel);
    
    // Interactive control
    bool isElementHovered(const std::string& panelId) const;
    bool isElementClicked(const std::string& panelId) const;
    
    // Data recording and export
    void recordStepData(SimulationStep step, size_t neuronCount, size_t synapseCount,
                       size_t firingCount, float avgFiringRate);
    bool saveVisualizationState(const std::string& filepath) const;
    bool loadVisualizationState(const std::string& filepath);
    
    // Helper methods
    void clearScreen() const;
    void printStatus() const;
    
    // Static factory methods
    static std::unique_ptr<VisualizationInterface> createVisualization(
        VisualizationBackend backend, VisualizationMode mode = VisualizationMode::Disabled);
    
    static bool isBackendAvailable(VisualizationBackend backend);
    static std::vector<std::string> getAvailableBackends();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
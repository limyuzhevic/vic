#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "brain/Brain.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "core/Types/Types.hpp"
#include "analysis/NetworkAnalyzer.hpp"
#include "analysis/ActivityAnalyzer.hpp"
#include "analysis/PerformanceAnalyzer.hpp"
#include "analysis/StateAnalyzer.hpp"
#include "analysis/PlottingTools.hpp"

namespace nlm {

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize();
    
    // Update visualization data
    void update();
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Set visualization backend (e.g., "glfw", "sdl", "webgl")
    void setBackend(const std::string& backend);
    std::string getBackend() const;
    
    // Core visualization methods (enhanced)
    void visualizeNetwork(const Brain& brain);
    void visualizeActivity(const Brain& brain);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain);
    
    // Advanced visualization methods
    void visualizeNetworkTopology(const Brain& brain);
    void visualizeActivityPatterns(const Brain& brain);
    void visualizeStateEvolution(const Brain& brain);
    void visualizePerformanceMetrics(const Brain& brain);
    
    // Interactive visualization methods
    void enableInteractiveMode(bool enabled);
    bool isInteractiveMode() const;
    void setVisualizationScale(double scale);
    double getVisualizationScale() const;
    
    // Analysis integration
    void integrateAnalysis(const std::shared_ptr<NetworkAnalyzer>& networkAnalyzer);
    void integrateAnalysis(const std::shared_ptr<ActivityAnalyzer>& activityAnalyzer);
    void integrateAnalysis(const std::shared_ptr<PerformanceAnalyzer>& performanceAnalyzer);
    void integrateAnalysis(const std::shared_ptr<StateAnalyzer>& stateAnalyzer);
    void integrateAnalysis(const std::shared_ptr<PlottingTools>& plottingTools);
    
    // Export capabilities
    bool exportVisualization(const std::string& filepath, const std::string& format = "html");
    bool exportAnalysisResults(const std::string& filepath, const std::string& analysisType);
    
    // Visualization control
    void toggleVisualization(bool show);
    void setVisualizationType(const std::string& type);
    void setColorScheme(const std::string& scheme);
    
    // Real-time updates
    void enableRealTimeUpdates(bool enabled);
    bool areRealTimeUpdatesEnabled() const;
    
    // Multi-window support
    void addVisualizationWindow(const std::string& title, const std::string& type);
    void removeVisualizationWindow(const std::string& title);
    void updateWindow(const std::string& title);
    
    // User interaction
    void setMouseCallback(std::function<void(double, double)> callback);
    void setKeyboardCallback(std::function<void(char)> callback);
    void setZoomCallback(std::function<void(double)> callback);
    
    // Analysis and visualization coordination
    void synchronizeWithAnalysis(const std::shared_ptr<NetworkAnalyzer>& networkAnalyzer,
                                const std::shared_ptr<ActivityAnalyzer>& activityAnalyzer,
                                const std::shared_ptr<PerformanceAnalyzer>& performanceAnalyzer,
                                const std::shared_ptr<StateAnalyzer>& stateAnalyzer);
    
    // Performance optimization
    void setRenderQuality(double quality);
    void optimizeForPerformance();
    
    // Configuration
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration(const std::string& configFile) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
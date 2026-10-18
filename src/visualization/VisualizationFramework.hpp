#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "visualization/VisualizationInterface.hpp"

namespace nlm {

enum class VisualizationBackend {
    CONSOLE,
    GUI,
    WEB
};

enum class VisualizationType {
    NEURON,
    NETWORK,
    DYNAMICS,
    SYNAPSES,
    NETWORK_DYNAMICS,
    ACTIVITY_PATTERNS,
    SPACE_TIME,
    BRAIN_STRUCTURE,
    BRAIN_CONNECTIVITY,
    BRAIN_ACTIVITY,
    BRAIN_DEVELOPMENT,
    BRAIN_MEMORY,
    EXPERIMENT_FLOW,
    EXPERIMENT_RESULTS,
    EXPERIMENT_COMPARISON,
    EXPERIMENT_TIMELINE,
    EXPERIMENT_METRICS
};

struct VisualizationConfig {
    std::string name;
    VisualizationType type;
    VisualizationBackend backend;
    std::string outputFormat;
    std::string outputPath;
    double updateRate;
    std::map<std::string, std::string> parameters;
    bool realTime;
    bool interactive;
    int width;
    int height;
};

class VisualizationFramework {
public:
    VisualizationFramework();
    ~VisualizationFramework();
    
    bool initialize();
    bool initialize(const VisualizationConfig& config);
    void update();
    void render();
    void close();
    
    bool addVisualizer(std::unique_ptr<VisualizationInterface> visualizer);
    bool removeVisualizer(const std::string& name);
    VisualizationInterface* getVisualizer(const std::string& name) const;
    
    bool createVisualization(const VisualizationConfig& config);
    bool updateVisualization(const std::string& name, const VisualizationConfig& config);
    bool destroyVisualization(const std::string& name);
    
    bool renderVisualization(const std::string& name);
    bool renderAllVisualizations();
    
    bool exportData(const std::string& visualizationName, const std::string& filePath, const std::string& format) const;
    bool importData(const std::string& visualizationName, const std::string& filePath);
    
    void setTheme(const std::string& theme);
    void setStyle(const std::string& style);
    
    std::vector<std::string> getAvailableThemes() const;
    std::vector<std::string> getAvailableStyles() const;
    std::vector<std::string> getActiveVisualizations() const;
    
    bool isRunning() const;
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Statistics and performance
    double getMemoryUsage() const;
    double getFrameTime() const;
    int getActiveVisualizationsCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

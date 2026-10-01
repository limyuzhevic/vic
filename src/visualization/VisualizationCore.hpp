#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include "../brain/Brain.hpp"

namespace nlm {

// Forward declarations for visualization components
class VisualizationPlugin;
class VisualizationRenderer;

// Visualization data structure
struct VisualizationData {
    std::string name;
    std::string type;
    double timestamp;
    std::vector<double> values;
    std::map<std::string, std::string> metadata;
    
    VisualizationData(const std::string& n = "", const std::string& t = "")
        : name(n), type(t), timestamp(std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count()) {}
};

// Visualization plugin interface
class VisualizationPlugin {
public:
    virtual ~VisualizationPlugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getType() const = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual std::shared_ptr<VisualizationData> getData() const = 0;
    virtual void update(const Brain& brain, double deltaTime) = 0;
    virtual void render() = 0;
};

// Visualization system core
class VisualizationCore {
public:
    VisualizationCore();
    ~VisualizationCore();

    // Initialize visualization system
    bool initialize();

    // Update visualization data
    void update(const Brain& brain, double deltaTime);

    // Render visualizations
    void render();

    // Close visualization system
    void close();

    // Check if visualization is active
    bool isActive() const;

    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;

    // Register visualization plugin
    bool registerPlugin(std::unique_ptr<VisualizationPlugin> plugin);

    // Unregister visualization plugin
    bool unregisterPlugin(const std::string& name);

    // Get plugin by name
    VisualizationPlugin* getPlugin(const std::string& name);

    // Enable/disable specific visualization
    void enableVisualization(const std::string& name);
    void disableVisualization(const std::string& name);
    bool isVisualizationEnabled(const std::string& name) const;

    // Get visualization data
    std::shared_ptr<VisualizationData> getVisualizationData(const std::string& name) const;

    // Export visualization data
    bool exportData(const std::string& format, const std::string& filepath, 
                   const std::vector<std::string>& visualizationNames = {}) const;

    // Performance statistics
    double getRenderTime() const;
    double getUpdateTime() const;
    size_t getDrawCallCount() const;

    // Configuration
    void setConfig(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
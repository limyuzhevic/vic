#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <mutex>
#include <functional>
#include <unordered_map>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Forward declarations
class DebugConsole;
class NetworkVisualizer;
class ActivityVisualizer;
class MemoryVisualizer;
class NeuromodulationVisualizer;
class DevelopmentVisualizer;
class Profiler;
class ExportUtils;
class PluginInterface;

// Visualization output formats
enum class OutputFormat {
    Text,
    JSON,
    Binary,
    CSV,
    HTML,
    WebSocket,
    NetworkStream
};

// Visualization plugins system
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize() = 0;
    virtual void update(double deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    virtual bool isActive() const = 0;
};

// Plugin registry for dynamic loading
class PluginRegistry {
public:
    using PluginFactory = std::function<std::unique_ptr<PluginInterface>()>;
    
    static PluginRegistry& getInstance();
    
    void registerPlugin(const std::string& name, const std::string& version, PluginFactory factory);
    std::unique_ptr<PluginInterface> createPlugin(const std::string& name);
    std::vector<std::string> getRegisteredPlugins() const;
    
private:
    PluginRegistry() = default;
    std::map<std::string, std::pair<std::string, PluginFactory>> plugins;
};

// Main advanced visualization system
class AdvancedVisualization {
public:
    AdvancedVisualization();
    ~AdvancedVisualization();
    
    // Initialize the visualization system
    bool initialize(const std::string& configFile = "");
    
    // Update visualization data
    void update(double deltaTime);
    
    // Render all active visualizations
    void render();
    
    // Shutdown visualization system
    void shutdown();
    
    // Configure output
    void setOutputFormat(OutputFormat format);
    void setOutputPath(const std::string& path);
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Component management
    void enableComponent(const std::string& name);
    void disableComponent(const std::string& name);
    bool isComponentEnabled(const std::string& name) const;
    
    // Plugin management
    bool registerPlugin(const std::string& pluginName);
    void unregisterPlugin(const std::string& pluginName);
    std::vector<std::string> getActivePlugins() const;
    
    // Debug console integration
    void setDebugConsole(DebugConsole* console);
    
    // Main update loop
    void run(const Brain& brain, double deltaTime);
    
    // Export functionality
    bool exportData(const std::string& filename, OutputFormat format = OutputFormat::JSON);
    
    // Performance monitoring
    double getFPS() const;
    double getLastFrameTime() const;
    uint64_t getTotalFramesRendered() const;
    
    // System status
    bool isRunning() const;
    std::string getSystemStatus() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
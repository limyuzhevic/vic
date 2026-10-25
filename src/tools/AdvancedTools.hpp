#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace nlm {

/**
 * Advanced Visualization System
 * 
 * Provides real-time and post-simulation visualization capabilities
 * Supports multiple output formats and customizable visual elements
 */

class VisualizationSystem {
public:
    /**
     * Visualization output formats
     */
    enum class OutputFormat {
        Text,
        JSON,
        CSV,
        Interactive,
        WebGL,
        Video,
        AnimatedGif
    };

    /**
     * Visual element types
     */
    enum class ElementType {
        LineChart,
        ScatterPlot,
        HeatMap,
        NetworkGraph,
        PopulationHistogram,
        TimeSeries,
        3DPlot,
        Custom
    };

    /**
     * Visualization data point
     */
    struct DataPoint {
        double timestamp;
        double value;
        std::string label;
        std::unordered_map<std::string, double> metadata;
        
        DataPoint(double time, double val, const std::string& lbl = "")
            : timestamp(time), value(val), label(lbl) {}
    };

    /**
     * Visualization configuration
     */
    struct VisualizationConfig {
        std::string name;
        ElementType elementType;
        OutputFormat outputFormat;
        std::string title;
        std::string xAxisLabel;
        std::string yAxisLabel;
        double width;
        double height;
        std::string outputPath;
        bool showGrid;
        bool showLegend;
        bool animate;
        double animationDuration;
        std::string colorScheme;
        std::unordered_map<std::string, std::string> customOptions;
        
        VisualizationConfig(const std::string& name, ElementType type,
                          OutputFormat format = OutputFormat::Text,
                          const std::string& title = "Visualization")
            : name(name), elementType(type), outputFormat(format),
              title(title), width(800.0), height(600.0),
              showGrid(true), showLegend(true), animate(false),
              animationDuration(1.0), colorScheme("default") {}
    };

    /**
     * Visualization result
     */
    struct VisualizationResult {
        std::string visualizationId;
        std::chrono::time_point<std::chrono::steady_clock> generationTime;
        double fileSize;
        std::string outputPath;
        std::string format;
        bool success;
        std::string errorMessage;
        std::unordered_map<std::string, std::string> metadata;
        
        VisualizationResult() : success(false), fileSize(0.0) {}
        VisualizationResult(const std::string& id, bool succ, const std::string& path = "")
            : visualizationId(id), success(succ), outputPath(path), fileSize(0.0) {}
    };

    VisualizationSystem();
    ~VisualizationSystem();
    
    // Disable copying, enable moving
    VisualizationSystem(const VisualizationSystem&) = delete;
    VisualizationSystem& operator=(const VisualizationSystem&) = delete;
    VisualizationSystem(VisualizationSystem&&) noexcept;
    VisualizationSystem& operator=(VisualizationSystem&&) noexcept;

    /**
     * Add data to a visualization element
     */
    void addData(const std::string& visualizationId, const DataPoint& dataPoint);
    void addData(const std::string& visualizationId, const std::vector<DataPoint>& dataPoints);
    
    /**
     * Create a new visualization
     */
    std::string createVisualization(const VisualizationConfig& config);
    
    /**
     * Generate visualization output
     */
    VisualizationResult generate(const std::string& visualizationId);
    
    /**
     * Get visualization data
     */
    std::vector<DataPoint> getData(const std::string& visualizationId) const;
    std::map<std::string, std::vector<DataPoint>> getAllData() const;
    
    /**
     * Remove visualization
     */
    bool removeVisualization(const std::string& visualizationId);
    
    /**
     * List all visualizations
     */
    std::vector<std::string> getVisualizationIds() const;
    
    /**
     * Clear all data
     */
    void clear();
    
    /**
     * Check if visualization exists
     */
    bool hasVisualization(const std::string& visualizationId) const;
    
    /**
     * Generate comprehensive report
     */
    std::string generateReport() const;
    
    /**
     * Export data for external tools
     */
    bool exportCSV(const std::string& visualizationId, const std::string& filename) const;
    bool exportJSON(const std::string& visualizationId, const std::string& filename) const;
    
    /**
     * Batch generation of multiple visualizations
     */
    std::vector<VisualizationResult> generateBatch(
        const std::vector<std::string>& visualizationIds);
    
    /**
     * Get system information
     */
    std::string getSystemInfo() const;
    
    /**
     * Configure visualization elements
     */
    bool configureElement(const std::string& visualizationId, 
                         const VisualizationConfig& config);
    
    /**
     * Get configuration
     */
    VisualizationConfig getConfig(const std::string& visualizationId) const;
    
    /**
     * Pre-built visualization generators
     */
    std::string createSpikeRateChart(const std::string& name, double duration = 60.0);
    std::string createFiringRateChart(const std::string& name, size_t neuronCount = 1000);
    std::string createSynapticWeightChart(const std::string& name);
    std::string createMemoryChart(const std::string& name);
    std::string createNeuromodulationChart(const std::string& name);
    std::string createNetworkTopologyChart(const std::string& name, size_t regionCount = 4);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void validateConfig(const VisualizationConfig& config) const;
    std::string generateInteractiveHTML(const std::string& visualizationId) const;
    std::string generateTextReport(const std::string& visualizationId) const;
    std::string generateJSONData(const std::string& visualizationId) const;
    std::string generateCSVData(const std::string& visualizationId) const;
    std::string escapeHTML(const std::string& text) const;
};

/**
 * Advanced Debugging System
 * 
 * Provides comprehensive debugging capabilities for neural network simulation
 * Supports breakpoint debugging, memory inspection, and real-time monitoring
 */

class AdvancedDebugger {
public:
    /**
     * Breakpoint types
     */
    enum class BreakpointType {
        Step,
        Condition,
        Watch,
        Exception,
        Performance,
        Memory
    };

    /**
     * Debug breakpoint
     */
    struct Breakpoint {
        std::string id;
        BreakpointType type;
        std::string location; // function name or code location
        std::string condition; // expression to evaluate
        std::string message; // message to display when triggered
        bool enabled;
        bool hitCount; // number of times hit (if condition-based)
        std::chrono::time_point<std::chrono::steady_clock> firstHitTime;
        std::chrono::time_point<std::chrono::steady_clock> lastHitTime;
        
        Breakpoint(const std::string& id, BreakpointType type, const std::string& location)
            : id(id), type(type), location(location), enabled(true), hitCount(0) {}
    };

    /**
     * Memory snapshot
     */
    struct MemorySnapshot {
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
        size_t totalMemory; // bytes
        size_t availableMemory;
        size_t allocatedObjects;
        std::unordered_map<std::string, size_t> objectCounts; // by type
        std::vector<std::string> stackTrace; // for debugging allocations
        
        MemorySnapshot() : totalMemory(0), availableMemory(0), allocatedObjects(0) {}
    };

    /**
     * Performance snapshot
     */
    struct PerformanceSnapshot {
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
        double cpuUsage; // percentage
        double memoryUsage; // percentage
        double simulationTime; // seconds since last snapshot
        size_t spikeEventsProcessed;
        size_t neuronsActive;
        size_t synapticConnections;
        std::unordered_map<std::string, double> customMetrics;
        
        PerformanceSnapshot() : cpuUsage(0.0), memoryUsage(0.0), 
                               simulationTime(0.0), spikeEventsProcessed(0),
                               neuronsActive(0), synapticConnections(0) {}
    };

    /**
     * Debug command
     */
    struct DebugCommand {
        std::string name;
        std::string description;
        std::function<bool(const std::vector<std::string>&)> execute;
        std::string helpText;
        
        DebugCommand(const std::string& name, const std::string& desc,
                    const std::function<bool(const std::vector<std::string>&)>& exec,
                    const std::string& help = "")
            : name(name), description(desc), execute(exec), helpText(help) {}
    };

    AdvancedDebugger();
    ~AdvancedDebugger();
    
    // Disable copying, enable moving
    AdvancedDebugger(const AdvancedDebugger&) = delete;
    AdvancedDebugger& operator=(const AdvancedDebugger&) = delete;
    AdvancedDebugger(AdvancedDebugger&&) noexcept;
    AdvancedDebugger& operator=(AdvancedDebugger&&) noexcept;

    /**
     * Add breakpoint
     */
    std::string addBreakpoint(const Breakpoint& breakpoint);
    bool removeBreakpoint(const std::string& id);
    bool enableBreakpoint(const std::string& id, bool enabled = true);
    bool setBreakpointCondition(const std::string& id, const std::string& condition);
    
    /**
     * Get breakpoints
     */
    Breakpoint getBreakpoint(const std::string& id) const;
    std::vector<Breakpoint> getBreakpoints() const;
    std::vector<Breakpoint> getBreakpoints(BreakpointType type) const;
    
    /**
     * Take memory snapshot
     */
    std::string takeMemorySnapshot();
    MemorySnapshot getMemorySnapshot(const std::string& snapshotId) const;
    
    /**
     * Take performance snapshot
     */
    std::string takePerformanceSnapshot();
    PerformanceSnapshot getPerformanceSnapshot(const std::string& snapshotId) const;
    
    /**
     * Compare snapshots
     */
    std::string compareMemorySnapshots(const std::string& snapshot1Id, 
                                       const std::string& snapshot2Id) const;
    std::string comparePerformanceSnapshots(const std::string& snapshot1Id,
                                           const std::string& snapshot2Id) const;
    
    /**
     * Add debug command
     */
    bool addDebugCommand(const DebugCommand& command);
    bool removeDebugCommand(const std::string& name);
    bool executeDebugCommand(const std::string& name, 
                           const std::vector<std::string>& args);
    
    /**
     * Get debug commands
     */
    DebugCommand getDebugCommand(const std::string& name) const;
    std::vector<DebugCommand> getDebugCommands() const;
    
    /**
     * Generate debug report
     */
    std::string generateDebugReport() const;
    
    /**
     * Export debug data
     */
    bool exportBreakpoints(const std::string& filename) const;
    bool exportSnapshots(const std::string& filename) const;
    
    /**
     * Get system information
     */
    std::string getDebugInfo() const;
    
    /**
     * Reset debugger state
     */
    void reset();
    
    /**
     * Check if breakpoint should trigger
     */
    bool shouldBreakpointTrigger(const Breakpoint& breakpoint) const;
    
    /**
     * Record breakpoint hit
     */
    void recordBreakpointHit(const std::string& id);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeCommands();
    std::string generateMemoryReport() const;
    std::string generatePerformanceReport() const;
    std::string generateBreakpointReport() const;
};

/**
 * Plugin Interface
 * 
 * Base class for NLM plugins that can extend functionality
 */

class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    
    /**
     * Get plugin name
     */
    virtual std::string getName() const = 0;
    
    /**
     * Get plugin version
     */
    virtual std::string getVersion() const = 0;
    
    /**
     * Get plugin description
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * Get plugin author
     */
    virtual std::string getAuthor() const = 0;
    
    /**
     * Get plugin dependencies
     */
    virtual std::vector<std::string> getDependencies() const = 0;
    
    /**
     * Initialize plugin
     */
    virtual bool initialize(const std::unordered_map<std::string, std::string>& config) = 0;
    
    /**
     * Shutdown plugin
     */
    virtual void shutdown() = 0;
    
    /**
     * Check if plugin is initialized
     */
    virtual bool isInitialized() const = 0;
    
    /**
     * Get plugin capabilities
     */
    virtual std::vector<std::string> getCapabilities() const = 0;
    
    /**
     * Execute plugin action
     */
    virtual bool execute(const std::string& action, const std::vector<std::string>& args) = 0;
    
    /**
     * Get plugin configuration schema
     */
    virtual std::unordered_map<std::string, std::string> getConfigSchema() const = 0;
    
    /**
     * Validate plugin configuration
     */
    virtual bool validateConfig(const std::unordered_map<std::string, std::string>& config) const = 0;
};

/**
 * Plugin Manager
 * 
 * Manages plugin loading, initialization, and lifecycle
 */

class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Disable copying, enable moving
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) noexcept;
    PluginManager& operator=(PluginManager&&) noexcept;

    /**
     * Load plugin from file
     */
    bool loadPlugin(const std::string& pluginPath);
    
    /**
     * Unload plugin
     */
    bool unloadPlugin(const std::string& pluginName);
    
    /**
     * Get loaded plugins
     */
    std::vector<std::string> getLoadedPlugins() const;
    
    /**
     * Get plugin instance
     */
    std::shared_ptr<NLMPlugin> getPlugin(const std::string& name) const;
    
    /**
     * Initialize all plugins
     */
    bool initializeAll(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& pluginConfigs);
    
    /**
     * Shutdown all plugins
     */
    void shutdownAll();
    
    /**
     * Execute plugin action
     */
    bool executePluginAction(const std::string& pluginName, 
                           const std::string& action,
                           const std::vector<std::string>& args);
    
    /**
     * Get plugin information
     */
    std::string getPluginInfo(const std::string& name) const;
    std::string getAllPluginsInfo() const;
    
    /**
     * Scan for available plugins
     */
    std::vector<std::string> scanPlugins(const std::string& directory) const;
    
    /**
     * Set plugin directory
     */
    void setPluginDirectory(const std::string& directory);
    
    /**
     * Get plugin directory
     */
    std::string getPluginDirectory() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

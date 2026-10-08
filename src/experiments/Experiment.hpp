#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>

namespace nlm {

// Experiment: Records data from a single experimental run
class Experiment {
public:
    Experiment();
    Experiment(const std::string& name);
    ~Experiment();
    
    // Experiment metadata
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Random seed
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
    // Configuration
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    
    // Metrics
    void recordMetric(const std::string& name, double value);
    void recordMetric(const std::string& name, float value);
    void recordMetric(const std::string& name, int value);
    
    // Get all metrics
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    
    // Get metric value
    double getMetric(const std::string& name) const;
    
    // Check if metric exists
    bool hasMetric(const std::string& name) const;
    
    // Recording step data
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    
    // Get step data
    struct StepData {
        SimulationStep step;
        size_t neuronCount;
        size_t synapseCount;
        size_t firingCount;
        float avgFiringRate;
    };
    const std::vector<StepData>& getStepData() const;
    
    // Duration
    SimulationStep getStartStep() const;
    void setStartStep(SimulationStep step);
    SimulationStep getEndStep() const;
    void setEndStep(SimulationStep step);
    
    // Notes
    void addNote(const std::string& note);
    const std::vector<std::string>& getNotes() const;
    
    // ========== PLUGIN INTERFACE ==========
    
    // Abstract base class for experiment plugins
    class ExperimentPlugin {
    public:
        virtual ~ExperimentPlugin() = default;
        
        // Initialize plugin with experiment reference
        virtual void initialize(const Experiment& experiment) = 0;
        
        // Process experiment data
        virtual void process() = 0;
        
        // Get plugin name
        virtual const std::string& getName() const = 0;
        
        // Check if plugin is active
        virtual bool isActive() const = 0;
        
        // Enable/disable plugin
        virtual void setActive(bool active) = 0;
    };
    
    // Plugin factory interface
    class PluginFactory {
    public:
        virtual ~PluginFactory() = default;
        
        // Create plugin instance
        virtual std::unique_ptr<ExperimentPlugin> createPlugin(
            const std::string& type,
            const std::vector<float>& parameters) = 0;
        
        // Get available plugin types
        virtual std::vector<std::string> getAvailablePluginTypes() const = 0;
        
        // Get default parameters for plugin type
        virtual std::vector<float> getDefaultParameters(const std::string& type) const = 0;
    };
    
    // Plugin manager interface
    class PluginManager {
    public:
        virtual ~PluginManager() = default;
        
        // Register plugin factory
        virtual void registerPluginFactory(std::unique_ptr<PluginFactory> factory) = 0;
        
        // Create plugin
        virtual std::unique_ptr<ExperimentPlugin> createPlugin(
            const std::string& type,
            const std::vector<float>& parameters = {}) = 0;
        
        // Get all registered plugin types
        virtual std::vector<std::string> getRegisteredPluginTypes() const = 0;
        
        // Get plugin instances
        virtual std::vector<ExperimentPlugin*> getPlugins() const = 0;
        
        // Enable/disable plugin by name
        virtual bool setPluginActive(const std::string& pluginName, bool active) = 0;
        
        // Remove plugin
        virtual bool removePlugin(const std::string& pluginName) = 0;
        
        // Clear all plugins
        virtual void clear() = 0;
    };
    
    // Plugin loader interface (for loading plugins from external sources)
    class PluginLoader {
    public:
        virtual ~PluginLoader() = default;
        
        // Load plugin from file
        virtual bool loadFromFile(const std::string& filepath) = 0;
        
        // Load plugin from directory
        virtual bool loadFromDirectory(const std::string& directory) = 0;
        
        // Get loaded plugin names
        virtual std::vector<std::string> getLoadedPluginNames() const = 0;
        
        // Unload plugin
        virtual bool unloadPlugin(const std::string& pluginName) = 0;
    };
    
    // Experiment extensibility methods
    
    // Set plugin manager
    void setPluginManager(std::unique_ptr<PluginManager> manager);
    
    // Get plugin manager
    PluginManager* getPluginManager() const;
    
    // Register plugin factory
    void registerPluginFactory(std::unique_ptr<PluginFactory> factory);
    
    // Create plugin
    std::unique_ptr<ExperimentPlugin> createPlugin(const std::string& type,
                                                  const std::vector<float>& parameters = {});
    
    // Get plugin instances
    std::vector<ExperimentPlugin*> getPlugins() const;
    
    // Enable/disable plugin by name
    bool setPluginActive(const std::string& pluginName, bool active);
    
    // Remove plugin
    bool removePlugin(const std::string& pluginName);
    
    // Load plugins from file
    bool loadPluginsFromFile(const std::string& filepath);
    
    // Load plugins from directory
    bool loadPluginsFromDirectory(const std::string& directory);
    
    // Process all plugins
    void processPlugins();
    
    // ========== EXPERIMENT ANALYSIS ==========
    
    // Analyze experiment results
    virtual std::string analyzeResults() const;
    
    // Export data to file
    virtual bool exportData(const std::string& filepath) const;
    
    // Compare with other experiments
    virtual bool compareWith(const Experiment& other) const;
    
    // Get experiment summary
    virtual std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <typeindex>

namespace nlm {

// Plugin interface for extending NLM functionality
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const class Config& config) { return true; }
    virtual void shutdown() {}
    virtual void update(double timestep) {}
    
    // Plugin capabilities
    virtual std::vector<std::string> getCapabilities() const { return {}; }
    virtual std::vector<std::type_index> getRequiredComponents() const { return {}; }
    
    // Plugin commands/arguments
    virtual std::vector<std::string> getPluginArguments() const { return {}; }
    virtual bool parseArguments(const std::vector<std::string>& args) { return true; }
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<NLMPlugin>()>;

// Plugin registry for managing loaded plugins
class PluginRegistry {
public:
    PluginRegistry() = default;
    ~PluginRegistry() = default;
    
    // Register a plugin factory
    void registerPlugin(const std::string& name, PluginFactory factory) {
        pluginFactories[name] = factory;
    }
    
    // Get plugin by name
    std::unique_ptr<NLMPlugin> createPlugin(const std::string& name) {
        auto it = pluginFactories.find(name);
        if (it != pluginFactories.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    // List all available plugins
    std::vector<std::string> getAvailablePlugins() const {
        std::vector<std::string> names;
        for (const auto& pair : pluginFactories) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Check if plugin is available
    bool hasPlugin(const std::string& name) const {
        return pluginFactories.find(name) != pluginFactories.end();
    }
    
    // Get plugin metadata (if available)
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::vector<std::string> capabilities;
    };
    
    std::vector<PluginInfo> getPluginInfo() const {
        std::vector<PluginInfo> info;
        for (const auto& pair : pluginFactories) {
            PluginInfo pluginInfo;
            pluginInfo.name = pair.first;
            // Note: In a full implementation, plugin factories would also provide metadata
            info.push_back(pluginInfo);
        }
        return info;
    }
    
private:
    std::map<std::string, PluginFactory> pluginFactories;
};

// Neural network extension plugin
class NeuralNetworkPlugin : public NLMPlugin {
public:
    std::string getName() const override { return "NeuralNetworkExtension"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { 
        return "Extends neural network architecture with custom neuron types"; 
    }
    std::string getAuthor() const override { return "NLM Team"; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"custom_neurons", "modified_plasticity", "special_dynamics"};
    }
    
    void initializeCustomNeurons() {
        NLM_LOG_INFO("NeuralNetworkPlugin: Initializing custom neuron types...");
        // Implementation would register custom neuron types with the brain
    }
    
    void update(double timestep) override {
        // Custom neural dynamics updates
        if (stepCount_ % updateInterval_ == 0) {
            NLM_LOG_INFO("NeuralNetworkPlugin: Processing custom dynamics...");
        }
        stepCount_++;
    }
    
    bool initialize(const class Config& config) override {
        if (!NLMPlugin::initialize(config)) return false;
        
        // Read plugin-specific configuration
        std::string neuronTypes = config.getOr<std::string>("plugin.neuron_types", "");
        if (!neuronTypes.empty()) {
            NLM_LOG_INFO("NeuralNetworkPlugin: Configured with neuron types: " + neuronTypes);
            initializeCustomNeurons();
        }
        
        updateInterval_ = config.getOr<size_t>("plugin.update_interval", 10);
        return true;
    }
    
private:
    size_t stepCount_ = 0;
    size_t updateInterval_ = 10;
};

// Memory management extension plugin
class MemoryManagementPlugin : public NLMPlugin {
public:
    std::string getName() const override { return "MemoryManagementExtension"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { 
        return "Extends memory management with custom algorithms and strategies"; 
    }
    std::string getAuthor() const override { return "NLM Team"; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"custom_memory_pools", "advanced_algorithms", "memory_optimization"};
    }
    
    void update(double timestep) override {
        if (stepCount_ % memoryUpdateInterval_ == 0) {
            NLM_LOG_INFO("MemoryManagementPlugin: Optimizing memory usage...");
        }
        stepCount_++;
    }
    
    bool initialize(const class Config& config) override {
        if (!NLMPlugin::initialize(config)) return false;
        
        memoryUpdateInterval_ = config.getOr<size_t>("plugin.memory_update_interval", 50);
        maxMemoryUsageMB_ = config.getOr<size_t>("plugin.max_memory_mb", 1024);
        
        NLM_LOG_INFO("MemoryManagementPlugin: Initialized with max memory: " + 
                    std::to_string(maxMemoryUsageMB_) + " MB");
        return true;
    }
    
private:
    size_t stepCount_ = 0;
    size_t memoryUpdateInterval_ = 50;
    size_t maxMemoryUsageMB_ = 1024;
};

// Visualization extension plugin
class VisualizationPlugin : public NLMPlugin {
public:
    std::string getName() const override { return "VisualizationExtension"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { 
        return "Extends visualization capabilities with advanced rendering and analysis"; 
    }
    std::string getAuthor() const override { return "NLM Team"; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"3d_rendering", "network_analysis", "temporal_visualization", "statistics"};
    }
    
    std::vector<std::string> getPluginArguments() const override {
        return {"--theme", "--quality", "--output", "--formats"};
    }
    
    bool parseArguments(const std::vector<std::string>& args) override {
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--theme") {
                if (i + 1 < args.size()) {
                    theme_ = args[i + 1];
                    i++;
                }
            } else if (args[i] == "--quality") {
                if (i + 1 < args.size()) {
                    quality_ = std::stoi(args[i + 1]);
                    i++;
                }
            } else if (args[i] == "--output") {
                if (i + 1 < args.size()) {
                    outputDir_ = args[i + 1];
                    i++;
                }
            }
        }
        return true;
    }
    
    void update(double timestep) override {
        if (stepCount_ % vizUpdateInterval_ == 0) {
            NLM_LOG_INFO("VisualizationPlugin: Updating visualization...");
        }
        stepCount_++;
    }
    
    bool initialize(const class Config& config) override {
        if (!NLMPlugin::initialize(config)) return false;
        
        vizUpdateInterval_ = config.getOr<size_t>("plugin.viz_update_interval", 25);
        quality_ = config.getOr<size_t>("plugin.visualization_quality", 3);
        
        NLM_LOG_INFO("VisualizationPlugin: Initialized with quality level: " + std::to_string(quality_));
        return true;
    }
    
private:
    size_t stepCount_ = 0;
    size_t vizUpdateInterval_ = 25;
    size_t quality_ = 3;
    std::string theme_ = "default";
    std::string outputDir_ = "./visualizations";
};

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    PluginManager() : registry(std::make_unique<PluginRegistry>()) {}
    ~PluginManager() = default;
    
    // Initialize plugin manager
    bool initialize(const class Config& config) {
        NLM_LOG_INFO("PluginManager: Initializing plugin system...");
        
        // Register built-in plugins
        registerBuiltInPlugins();
        
        // Load plugins from configuration
        auto pluginsConfig = config.get<std::vector<std::string>>("plugins.enabled");
        for (const auto& pluginName : pluginsConfig) {
            if (registry->hasPlugin(pluginName)) {
                auto plugin = registry->createPlugin(pluginName);
                if (plugin) {
                    plugins_.push_back(std::move(plugin));
                    NLM_LOG_INFO("PluginManager: Loaded plugin: " + pluginName);
                }
            }
        }
        
        return true;
    }
    
    // Update all plugins
    void update(double timestep) {
        for (auto& plugin : plugins_) {
            plugin->update(timestep);
        }
    }
    
    // Get all loaded plugins
    const std::vector<std::unique_ptr<NLMPlugin>>& getPlugins() const {
        return plugins_;
    }
    
    // Get plugin by name
    NLMPlugin* getPlugin(const std::string& name) {
        for (auto& plugin : plugins_) {
            if (plugin->getName() == name) {
                return plugin.get();
            }
        }
        return nullptr;
    }
    
    // Get registry instance
    PluginRegistry* getRegistry() { return registry.get(); }
    
    // Shutdown all plugins
    void shutdown() {
        NLM_LOG_INFO("PluginManager: Shutting down all plugins...");
        for (auto& plugin : plugins_) {
            plugin->shutdown();
        }
        plugins_.clear();
    }
    
private:
    void registerBuiltInPlugins() {
        // Register neural network plugin
        registry->registerPlugin("neural_network", []() {
            return std::make_unique<NeuralNetworkPlugin>();
        });
        
        // Register memory management plugin
        registry->registerPlugin("memory_management", []() {
            return std::make_unique<MemoryManagementPlugin>();
        });
        
        // Register visualization plugin
        registry->registerPlugin("visualization", []() {
            return std::make_unique<VisualizationPlugin>();
        });
        
        NLM_LOG_INFO("PluginManager: Registered built-in plugins:");
        for (const auto& name : registry->getAvailablePlugins()) {
            NLM_LOG_INFO("  - " + name);
        }
    }
    
    std::unique_ptr<PluginRegistry> registry;
    std::vector<std::unique_ptr<NLMPlugin>> plugins_;
};

} // namespace nlm

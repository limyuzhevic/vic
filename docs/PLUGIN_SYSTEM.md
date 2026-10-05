// Plugin System for NLM Framework
// 
// This document outlines the plugin architecture for NLM, enabling extensibility
// and customization of neural network components.

## Plugin System Overview

The NLM plugin system provides a framework for extending and customizing neural network
components, including:

1. **Plugin Types**: Core plugins, component plugins, and integration plugins
2. **Plugin Architecture**: Plugin interface, discovery, and loading
3. **Plugin Lifecycle**: Plugin initialization, execution, and cleanup
4. **Plugin Management**: Plugin registry and dependency management
5. **Plugin Security**: Plugin validation and security checks

## Plugin Architecture

### 1. Plugin Interface

```cpp
// Base plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const PluginConfig& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Plugin capabilities
    virtual std::vector<std::string> getCapabilities() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    
    // Plugin execution
    virtual PluginResult execute(const PluginInput& input) = 0;
    
    // Plugin configuration
    virtual bool configure(const PluginConfig& config) = 0;
    virtual PluginConfig getConfiguration() const = 0;
};
```

### 2. Plugin Registry

```cpp
// Plugin registry for managing plugins
class PluginRegistry {
public:
    static PluginRegistry& instance() {
        static PluginRegistry registry;
        return registry;
    }
    
    // Plugin registration
    template<typename TPlugin>
    bool registerPlugin(const std::string& name, const std::string& version) {
        static_assert(std::is_base_of_v<IPlugin, TPlugin>, "TPlugin must derive from IPlugin");
        
        auto plugin = std::make_shared<TPlugin>();
        plugin->setName(name);
        plugin->setVersion(version);
        
        // Validate dependencies
        if (!validateDependencies(plugin->getDependencies())) {
            return false;
        }
        
        // Store plugin
        plugins_[name] = plugin;
        
        return true;
    }
    
    // Plugin discovery
    std::vector<std::shared_ptr<IPlugin>> discoverPlugins(
        const std::string& directory) {
        
        std::vector<std::shared_ptr<IPlugin>> discovered;
        
        // Scan directory for plugin libraries
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".so") {
                auto plugin = loadPlugin(entry.path());
                if (plugin) {
                    discovered.push_back(plugin);
                }
            }
        }
        
        return discovered;
    }
    
    // Plugin loading
    std::shared_ptr<IPlugin> loadPlugin(const std::string& path) {
        try {
            // Load plugin library
            auto library = std::make_shared<DynamicLibrary>(path);
            
            // Get plugin factory function
            auto factory = library->getFunction<IPlugin*("factory")>("create_plugin");
            if (!factory) {
                return nullptr;
            }
            
            // Create plugin instance
            auto plugin = std::shared_ptr<IPlugin>(factory());
            
            // Store library for cleanup
            libraries_.push_back(library);
            
            return plugin;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load plugin " << path << ": " << e.what() << std::endl;
            return nullptr;
        }
    }
    
    // Plugin retrieval
    std::shared_ptr<IPlugin> getPlugin(const std::string& name) {
        auto it = plugins_.find(name);
        if (it != plugins_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Plugin enumeration
    std::vector<std::string> getPluginNames() const {
        std::vector<std::string> names;
        for (const auto& pair : plugins_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Plugin initialization
    bool initializeAllPlugins() {
        bool all_success = true;
        
        for (auto& pair : plugins_) {
            if (!pair.second->isInitialized()) {
                PluginConfig default_config;
                if (!pair.second->initialize(default_config)) {
                    std::cerr << "Failed to initialize plugin " << pair.first << std::endl;
                    all_success = false;
                }
            }
        }
        
        return all_success;
    }
    
    // Plugin shutdown
    void shutdownAllPlugins() {
        for (auto& pair : plugins_) {
            if (pair.second->isInitialized()) {
                pair.second->shutdown();
            }
        }
    }
    
    // Plugin dependency validation
    bool validateDependencies(const std::vector<std::string>& dependencies) {
        for (const auto& dependency : dependencies) {
            if (plugins_.find(dependency) == plugins_.end()) {
                return false;
            }
        }
        return true;
    }
    
    // Plugin configuration
    bool configurePlugin(const std::string& name, const PluginConfig& config) {
        auto plugin = getPlugin(name);
        if (plugin) {
            return plugin->configure(config);
        }
        return false;
    }
    
private:
    PluginRegistry() = default;
    ~PluginRegistry() {
        shutdownAllPlugins();
    }
    
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> plugins_;
    std::vector<std::shared_ptr<DynamicLibrary>> libraries_;
};
```

### 3. Plugin Types

```cpp
// Core plugin types
enum class PluginType {
    Core,          // Core framework components
    Component,     // Component extensions
    Integration,   // Integration with external systems
    Visualization, // Visualization plugins
    Analysis,      // Analysis and monitoring plugins
    Custom        // User-defined plugins
};

// Plugin capability types
enum class PluginCapability {
    NeuronModification,    // Modify neuron behavior
    SynapseModification,   // Modify synapse behavior
    RegionModification,    // Modify region behavior
    PopulationModification, // Modify population behavior
    NetworkModification,   // Modify network structure
    LearningModification,  // Modify learning rules
    SensorExtension,       // Extend sensor capabilities
    ActuatorExtension,     // Extend actuator capabilities
    WorldExtension,        // Extend world capabilities
    VisualizationExtension, // Extend visualization capabilities
    AnalysisExtension,     // Extend analysis capabilities
    DataExtension,         // Extend data handling capabilities
    IntegrationExtension   // Extend integration capabilities
};
```

### 4. Plugin Lifecycle

```cpp
// Plugin lifecycle stages
enum class PluginLifecycleStage {
    NotLoaded,    // Plugin not loaded
    Loading,      // Plugin is loading
    Initializing, // Plugin is being initialized
    Active,       // Plugin is active
    ShuttingDown, // Plugin is shutting down
    Unloading,    // Plugin is being unloaded
    Error         // Plugin in error state
};

// Plugin lifecycle manager
class PluginLifecycleManager {
public:
    // Plugin lifecycle callback
    using LifecycleCallback = std::function<void(IPlugin*, PluginLifecycleStage)>;
    
    // Register lifecycle callback
    void registerLifecycleCallback(const LifecycleCallback& callback) {
        lifecycle_callbacks_.push_back(callback);
    }
    
    // Notify plugin state change
    void notifyStateChange(IPlugin* plugin, PluginLifecycleStage new_stage) {
        for (const auto& callback : lifecycle_callbacks_) {
            callback(plugin, new_stage);
        }
    }
    
    // Get plugin lifecycle stage
    PluginLifecycleStage getLifecycleStage(const std::string& name) {
        auto it = plugin_stages_.find(name);
        if (it != plugin_stages_.end()) {
            return it->second;
        }
        return PluginLifecycleStage::NotLoaded;
    }
    
    // Set plugin lifecycle stage
    void setLifecycleStage(const std::string& name, PluginLifecycleStage stage) {
        plugin_stages_[name] = stage;
    }
    
private:
    std::vector<LifecycleCallback> lifecycle_callbacks_;
    std::unordered_map<std::string, PluginLifecycleStage> plugin_stages_;
};
```

### 5. Plugin Security

```cpp
// Plugin security manager
class PluginSecurityManager {
public:
    // Plugin validation
    bool validatePlugin(const std::shared_ptr<IPlugin>& plugin) {
        // Check plugin signature
        if (!validatePluginSignature(plugin)) {
            return false;
        }
        
        // Check plugin permissions
        if (!validatePluginPermissions(plugin)) {
            return false;
        }
        
        // Check plugin dependencies
        if (!validatePluginDependencies(plugin)) {
            return false;
        }
        
        return true;
    }
    
    // Plugin signature validation
    bool validatePluginSignature(const std::shared_ptr<IPlugin>& plugin) {
        // TODO: Implement signature validation
        return true;
    }
    
    // Plugin permission validation
    bool validatePluginPermissions(const std::shared_ptr<IPlugin>& plugin) {
        // Check if plugin has required permissions
        auto capabilities = plugin->getCapabilities();
        for (const auto& capability : capabilities) {
            if (!hasPermission(capability)) {
                return false;
            }
        }
        return true;
    }
    
    // Plugin dependency validation
    bool validatePluginDependencies(const std::shared_ptr<IPlugin>& plugin) {
        // Check if all dependencies are available
        auto dependencies = plugin->getDependencies();
        for (const auto& dependency : dependencies) {
            if (!PluginRegistry::instance().getPlugin(dependency)) {
                return false;
            }
        }
        return true;
    }
    
    // Permission checking
    bool hasPermission(const std::string& capability) {
        // TODO: Implement permission checking
        return true;
    }
    
    // Plugin sandboxing
    std::shared_ptr<IPlugin> sandboxPlugin(const std::shared_ptr<IPlugin>& plugin) {
        // TODO: Implement plugin sandboxing
        return plugin;
    }
    
private:
    std::vector<std::string> allowed_capabilities_;
};
```

## Plugin Examples

### 1. Neuron Modification Plugin

```cpp
// Neuron modification plugin
class NeuronModificationPlugin : public IPlugin {
public:
    std::string getName() const override { return "NeuronModification"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override {
        return "Plugin for modifying neuron behavior";
    }
    
    bool initialize(const PluginConfig& config) override {
        // Initialize plugin
        neuron_modification_ = config.getBool("neuron_modification.enabled", false);
        return true;
    }
    
    void shutdown() override {
        // Cleanup plugin
        neuron_modification_ = false;
    }
    
    bool isInitialized() const override {
        return neuron_modification_;
    }
    
    std::vector<std::string> getCapabilities() const override {
        return {"NeuronModification"};
    }
    
    std::vector<std::string> getDependencies() const override {
        return {};
    }
    
    PluginResult execute(const PluginInput& input) override {
        if (!isInitialized()) {
            return PluginResult{PluginResultCode::Error, "Plugin not initialized"};
        }
        
        // Process neuron modification input
        if (input.type == "neuron_modification") {
            auto neuron_config = std::any_cast<NeuronConfig>(input.data["neuron_config"]);
            auto modification_type = std::any_cast<std::string>(input.data["modification_type"]);
            
            // Apply modification
            if (modification_type == "threshold_increase") {
                neuron_config.threshold += neuron_config.threshold * 0.1f;  // Increase by 10%
            } else if (modification_type == "excitability_decrease") {
                neuron_config.excitability *= 0.9f;  // Decrease by 10%
            }
            
            return PluginResult{PluginResultCode::Success, "Neuron modified successfully"};
        }
        
        return PluginResult{PluginResultCode::Error, "Invalid input type"};
    }
    
    bool configure(const PluginConfig& config) override {
        // Configure plugin
        neuron_modification_ = config.getBool("neuron_modification.enabled", false);
        return true;
    }
    
    PluginConfig getConfiguration() const override {
        PluginConfig config;
        config.set("neuron_modification.enabled", neuron_modification_);
        return config;
    }
    
private:
    bool neuron_modification_ = false;
};
```

### 2. Synapse Visualization Plugin

```cpp
// Synapse visualization plugin
class SynapseVisualizationPlugin : public IPlugin {
public:
    std::string getName() const override { return "SynapseVisualization"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override {
        return "Plugin for visualizing synapse connections";
    }
    
    bool initialize(const PluginConfig& config) override {
        // Initialize plugin
        output_format_ = config.getString("visualization.output_format", "PNG");
        output_path_ = config.getString("visualization.output_path", ".");
        return true;
    }
    
    void shutdown() override {
        // Cleanup plugin
    }
    
    bool isInitialized() const override {
        return !output_path_.empty();
    }
    
    std::vector<std::string> getCapabilities() const override {
        return {"VisualizationExtension"};
    }
    
    std::vector<std::string> getDependencies() const override {
        return {"NeuronModification"};
    }
    
    PluginResult execute(const PluginInput& input) override {
        if (!isInitialized()) {
            return PluginResult{PluginResultCode::Error, "Plugin not initialized"};
        }
        
        // Process visualization input
        if (input.type == "synapse_visualization") {
            auto synapses = std::any_cast<std::vector<std::shared_ptr<Synapse>>>(input.data["synapses"]);
            
            // Generate visualization
            std::string filename = output_path_ + "/synapse_visualization_" + 
                                  std::to_string(std::time(nullptr)) + "." + output_format_;
            
            if (generateVisualization(synapses, filename)) {
                return PluginResult{PluginResultCode::Success, "Visualization generated successfully"};
            } else {
                return PluginResult{PluginResultCode::Error, "Failed to generate visualization"};
            }
        }
        
        return PluginResult{PluginResultCode::Error, "Invalid input type"};
    }
    
    bool configure(const PluginConfig& config) override {
        // Configure plugin
        output_format_ = config.getString("visualization.output_format", "PNG");
        output_path_ = config.getString("visualization.output_path", ".");
        
        // Ensure output path exists
        std::filesystem::create_directories(output_path_);
        
        return true;
    }
    
    PluginConfig getConfiguration() const override {
        PluginConfig config;
        config.set("visualization.output_format", output_format_);
        config.set("visualization.output_path", output_path_);
        return config;
    }
    
private:
    bool generateVisualization(
        const std::vector<std::shared_ptr<Synapse>>& synapses,
        const std::string& filename) {
        // TODO: Implement visualization generation
        // This would typically use an external graphics library
        return true;
    }
    
    std::string output_format_ = "PNG";
    std::string output_path_ = ".";
};
```

### 3. Learning Rule Integration Plugin

```cpp
// Learning rule integration plugin
class LearningRuleIntegrationPlugin : public IPlugin {
public:
    std::string getName() const override { return "LearningRuleIntegration"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override {
        return "Plugin for integrating custom learning rules";
    }
    
    bool initialize(const PluginConfig& config) override {
        // Initialize plugin
        learning_rule_ = config.getString("learning_rule.type", "STDP");
        learning_rate_ = config.getFloat("learning_rule.rate", 0.01f);
        return true;
    }
    
    void shutdown() override {
        // Cleanup plugin
    }
    
    bool isInitialized() const override {
        return !learning_rule_.empty();
    }
    
    std::vector<std::string> getCapabilities() const override {
        return {"LearningModification"};
    }
    
    std::vector<std::string> getDependencies() const override {
        return {"NeuronModification", "SynapseModification"};
    }
    
    PluginResult execute(const PluginInput& input) override {
        if (!isInitialized()) {
            return PluginResult{PluginResultCode::Error, "Plugin not initialized"};
        }
        
        // Process learning rule input
        if (input.type == "learning_rule_update") {
            auto pre_neuron = std::any_cast<std::shared_ptr<Neuron>>(input.data["pre_neuron"]);
            auto post_neuron = std::any_cast<std::shared_ptr<Neuron>>(input.data["post_neuron"]);
            auto synapse = std::any_cast<std::shared_ptr<Synapse>>(input.data["synapse"]);
            
            // Apply custom learning rule
            if (learning_rule_ == "STDP") {
                applySTDP(*pre_neuron, *post_neuron, *synapse);
            } else if (learning_rule_ == "Hebbian") {
                applyHebbian(*pre_neuron, *post_neuron, *synapse);
            } else if (learning_rule_ == "Custom") {
                applyCustomRule(*pre_neuron, *post_neuron, *synapse);
            }
            
            return PluginResult{PluginResultCode::Success, "Learning rule applied successfully"};
        }
        
        return PluginResult{PluginResultCode::Error, "Invalid input type"};
    }
    
    bool configure(const PluginConfig& config) override {
        // Configure plugin
        learning_rule_ = config.getString("learning_rule.type", "STDP");
        learning_rate_ = config.getFloat("learning_rule.rate", 0.01f);
        return true;
    }
    
    PluginConfig getConfiguration() const override {
        PluginConfig config;
        config.set("learning_rule.type", learning_rule_);
        config.set("learning_rule.rate", learning_rate_);
        return config;
    }
    
private:
    void applySTDP(Neuron& pre_neuron, Neuron& post_neuron, Synapse& synapse) {
        // Apply STDP learning rule
        float weight_change = learning_rate_ * (pre_neuron.getFiringRate() - 1.0f) *
                             (post_neuron.getFiringRate() - 1.0f);
        synapse.setWeight(synapse.getWeight() + weight_change);
    }
    
    void applyHebbian(Neuron& pre_neuron, Neuron& post_neuron, Synapse& synapse) {
        // Apply Hebbian learning rule
        float weight_change = learning_rate_ * pre_neuron.getExcitability() *
                             post_neuron.getExcitability();
        synapse.setWeight(synapse.getWeight() + weight_change);
    }
    
    void applyCustomRule(Neuron& pre_neuron, Neuron& post_neuron, Synapse& synapse) {
        // Apply custom learning rule
        float weight_change = learning_rate_ * (pre_neuron.getFiringRate() * post_neuron.getFiringRate() -
                                               0.5f);  // Normalize around 0.5
        synapse.setWeight(synapse.getWeight() + weight_change);
    }
    
    std::string learning_rule_ = "STDP";
    float learning_rate_ = 0.01f;
};
```

## Plugin Management

### 1. Plugin Manager

```cpp
// Plugin manager for NLM
class PluginManager {
public:
    // Get plugin manager instance
    static PluginManager& instance() {
        static PluginManager manager;
        return manager;
    }
    
    // Register plugin
    bool registerPlugin(const std::shared_ptr<IPlugin>& plugin) {
        if (!plugin) {
            return false;
        }
        
        // Validate plugin
        if (!PluginSecurityManager::instance().validatePlugin(plugin)) {
            return false;
        }
        
        // Register plugin
        std::string name = plugin->getName();
        plugins_[name] = plugin;
        
        return true;
    }
    
    // Unregister plugin
    bool unregisterPlugin(const std::string& name) {
        auto it = plugins_.find(name);
        if (it != plugins_.end()) {
            it->second->shutdown();
            plugins_.erase(it);
            return true;
        }
        return false;
    }
    
    // Get plugin
    std::shared_ptr<IPlugin> getPlugin(const std::string& name) {
        auto it = plugins_.find(name);
        if (it != plugins_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Get all plugins
    std::vector<std::shared_ptr<IPlugin>> getAllPlugins() {
        std::vector<std::shared_ptr<IPlugin>> all_plugins;
        for (auto& pair : plugins_) {
            all_plugins.push_back(pair.second);
        }
        return all_plugins;
    }
    
    // Get plugins by capability
    std::vector<std::shared_ptr<IPlugin>> getPluginsByCapability(
        const std::string& capability) {
        
        std::vector<std::shared_ptr<IPlugin>> matching_plugins;
        
        for (auto& pair : plugins_) {
            auto plugin_capabilities = pair.second->getCapabilities();
            if (std::find(plugin_capabilities.begin(), plugin_capabilities.end(), capability) !=
                plugin_capabilities.end()) {
                matching_plugins.push_back(pair.second);
            }
        }
        
        return matching_plugins;
    }
    
    // Initialize all plugins
    bool initializeAllPlugins() {
        return PluginRegistry::instance().initializeAllPlugins();
    }
    
    // Shutdown all plugins
    void shutdownAllPlugins() {
        PluginRegistry::instance().shutdownAllPlugins();
    }
    
    // Configure plugin
    bool configurePlugin(const std::string& name, const PluginConfig& config) {
        return PluginRegistry::instance().configurePlugin(name, config);
    }
    
    // Execute plugin
    PluginResult executePlugin(
        const std::string& name,
        const PluginInput& input) {
        
        auto plugin = getPlugin(name);
        if (!plugin) {
            return PluginResult{PluginResultCode::Error, "Plugin not found"};
        }
        
        if (!plugin->isInitialized()) {
            return PluginResult{PluginResultCode::Error, "Plugin not initialized"};
        }
        
        return plugin->execute(input);
    }
    
    // Get plugin status
    PluginStatus getPluginStatus(const std::string& name) {
        PluginStatus status;
        
        auto plugin = getPlugin(name);
        if (plugin) {
            status.name = name;
            status.version = plugin->getVersion();
            status.description = plugin->getDescription();
            status.initialized = plugin->isInitialized();
            
            auto capabilities = plugin->getCapabilities();
            status.capabilities = capabilities;
            
            auto dependencies = plugin->getDependencies();
            status.dependencies = dependencies;
        }
        
        return status;
    }
    
    // Get all plugin statuses
    std::vector<PluginStatus> getAllPluginStatuses() {
        std::vector<PluginStatus> statuses;
        
        for (auto& pair : plugins_) {
            statuses.push_back(getPluginStatus(pair.first));
        }
        
        return statuses;
    }
    
private:
    PluginManager() = default;
    ~PluginManager() {
        shutdownAllPlugins();
    }
    
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> plugins_;
};
```

### 2. Plugin Configuration

```cpp
// Plugin configuration
struct PluginConfig {
    std::unordered_map<std::string, std::any> data;
    
    // Get value with default
    template<typename T>
    T get(const std::string& key, const T& default_value) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return std::any_cast<T>(it->second);
        }
        return default_value;
    }
    
    // Set value
    template<typename T>
    void set(const std::string& key, const T& value) {
        data[key] = value;
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : data) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Clear data
    void clear() {
        data.clear();
    }
};
```

### 3. Plugin Input and Output

```cpp
// Plugin input
struct PluginInput {
    std::string type;                    // Input type
    std::unordered_map<std::string, std::any> data;  // Input data
    std::unordered_map<std::string, std::any> metadata;  // Input metadata
    
    // Get value with default
    template<typename T>
    T get(const std::string& key, const T& default_value) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return std::any_cast<T>(it->second);
        }
        return default_value;
    }
};

// Plugin result
struct PluginResult {
    enum class Code {
        Success,
        Warning,
        Error,
        NotImplemented
    };
    
    Code code;                      // Result code
    std::string message;             // Result message
    std::unordered_map<std::string, std::any> data;  // Result data
    
    // Success result
    static PluginResult success(const std::string& message = "Success") {
        return PluginResult{Code::Success, message, {}, {}};
    }
    
    // Warning result
    static PluginResult warning(const std::string& message = "Warning") {
        return PluginResult{Code::Warning, message, {}, {}};
    }
    
    // Error result
    static PluginResult error(const std::string& message = "Error") {
        return PluginResult{Code::Error, message, {}, {}};
    }
    
    // Not implemented result
    static PluginResult notImplemented(const std::string& message = "Not implemented") {
        return PluginResult{Code::NotImplemented, message, {}, {}};
    }
};
```

### 4. Plugin Status

```cpp
// Plugin status
struct PluginStatus {
    std::string name;                          // Plugin name
    std::string version;                       // Plugin version
    std::string description;                   // Plugin description
    bool initialized;                          // Whether plugin is initialized
    std::vector<std::string> capabilities;     // Plugin capabilities
    std::vector<std::string> dependencies;     // Plugin dependencies
};
```

## Plugin Usage Examples

### 1. Using Built-in Plugins

```cpp
// Example: Using built-in plugins
#include "PluginManager.hpp"

int main() {
    // Initialize plugin manager
    PluginManager& plugin_manager = PluginManager::instance();
    
    // Register built-in plugins
    auto neuron_plugin = std::make_shared<NeuronModificationPlugin>();
    plugin_manager.registerPlugin(neuron_plugin);
    
    auto synapse_plugin = std::make_shared<SynapseVisualizationPlugin>();
    plugin_manager.registerPlugin(synapse_plugin);
    
    auto learning_plugin = std::make_shared<LearningRuleIntegrationPlugin>();
    plugin_manager.registerPlugin(learning_plugin);
    
    // Initialize all plugins
    plugin_manager.initializeAllPlugins();
    
    // Configure synapse plugin
    PluginConfig synapse_config;
    synapse_config.set("visualization.output_format", "PNG");
    synapse_config.set("visualization.output_path", "./visualizations");
    plugin_manager.configurePlugin("SynapseVisualization", synapse_config);
    
    // Configure learning plugin
    PluginConfig learning_config;
    learning_config.set("learning_rule.type", "STDP");
    learning_config.set("learning_rule.rate", 0.01f);
    plugin_manager.configurePlugin("LearningRuleIntegration", learning_config);
    
    // Execute plugins
    PluginInput synapse_input;
    synapse_input.type = "synapse_visualization";
    // Add synapse data to input
    
    auto synapse_result = plugin_manager.executePlugin("SynapseVisualization", synapse_input);
    
    PluginInput learning_input;
    learning_input.type = "learning_rule_update";
    // Add neuron/synapse data to input
    
    auto learning_result = plugin_manager.executePlugin("LearningRuleIntegration", learning_input);
    
    // Get plugin status
    auto synapse_status = plugin_manager.getPluginStatus("SynapseVisualization");
    
    // Shutdown all plugins
    plugin_manager.shutdownAllPlugins();
    
    return 0;
}
```

### 2. Creating Custom Plugins

```cpp
// Example: Creating a custom plugin
#include "PluginManager.hpp"

// User-defined plugin
class CustomPlugin : public IPlugin {
public:
    std::string getName() const override { return "CustomPlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override {
        return "User-defined custom plugin";
    }
    
    bool initialize(const PluginConfig& config) override {
        // Initialize plugin
        custom_parameter_ = config.getFloat("custom.parameter", 1.0f);
        return true;
    }
    
    void shutdown() override {
        // Cleanup plugin
    }
    
    bool isInitialized() const override {
        return initialized_;
    }
    
    std::vector<std::string> getCapabilities() const override {
        return {"CustomCapability"};
    }
    
    std::vector<std::string> getDependencies() const override {
        return {};
    }
    
    PluginResult execute(const PluginInput& input) override {
        if (!isInitialized()) {
            return PluginResult::error("Plugin not initialized");
        }
        
        // Process custom input
        if (input.type == "custom_operation") {
            // Apply custom operation
            auto parameter = input.getFloat("parameter", custom_parameter_);
            // ... custom logic ...
            
            return PluginResult::success("Custom operation completed");
        }
        
        return PluginResult::error("Invalid input type");
    }
    
    bool configure(const PluginConfig& config) override {
        // Configure plugin
        custom_parameter_ = config.getFloat("custom.parameter", 1.0f);
        initialized_ = true;
        return true;
    }
    
    PluginConfig getConfiguration() const override {
        PluginConfig config;
        config.set("custom.parameter", custom_parameter_);
        return config;
    }
    
private:
    bool initialized_ = false;
    float custom_parameter_ = 1.0f;
};

int main() {
    // Create custom plugin
    auto custom_plugin = std::make_shared<CustomPlugin>();
    
    // Register plugin
    PluginManager& plugin_manager = PluginManager::instance();
    plugin_manager.registerPlugin(custom_plugin);
    
    // Configure plugin
    PluginConfig config;
    config.set("custom.parameter", 2.0f);
    plugin_manager.configurePlugin("CustomPlugin", config);
    
    // Initialize plugin
    plugin_manager.initializeAllPlugins();
    
    // Execute plugin
    PluginInput input;
    input.type = "custom_operation";
    input.set("parameter", 3.0f);
    
    auto result = plugin_manager.executePlugin("CustomPlugin", input);
    
    // Get plugin status
    auto status = plugin_manager.getPluginStatus("CustomPlugin");
    
    // Shutdown plugin
    plugin_manager.shutdownAllPlugins();
    
    return 0;
}
```

### 3. Plugin Discovery and Loading

```cpp
// Example: Plugin discovery and loading
#include "PluginManager.hpp"

int main() {
    // Discover plugins in directory
    std::string plugin_directory = "./plugins";
    
    auto discovered_plugins = PluginRegistry::instance().discoverPlugins(plugin_directory);
    
    // Register discovered plugins
    PluginManager& plugin_manager = PluginManager::instance();
    
    for (auto& plugin : discovered_plugins) {
        plugin_manager.registerPlugin(plugin);
    }
    
    // Initialize all plugins
    plugin_manager.initializeAllPlugins();
    
    // Execute discovered plugins
    for (const auto& plugin : discovered_plugins) {
        std::string name = plugin->getName();
        
        PluginInput input;
        input.type = "plugin_discovery";
        input.set("plugin_name", name);
        
        plugin_manager.executePlugin(name, input);
    }
    
    // Shutdown all plugins
    plugin_manager.shutdownAllPlugins();
    
    return 0;
}
```

## Plugin System Summary

The NLM plugin system provides a comprehensive framework for extending and customizing
neural network components, including:

1. **Plugin Interface**: Base interface for all plugins
2. **Plugin Registry**: Centralized plugin management
3. **Plugin Types**: Core, component, integration, visualization, analysis, and custom plugins
4. **Plugin Lifecycle**: Plugin initialization, execution, and cleanup
5. **Plugin Security**: Plugin validation and security checks
6. **Plugin Examples**: Neuron modification, visualization, and learning rule integration
7. **Plugin Management**: Plugin manager for easy plugin usage
8. **Plugin Configuration**: Plugin configuration system
9. **Plugin Input/Output**: Plugin input and output structures

This plugin system significantly enhances the extensibility and customization capabilities
of the NLM framework, enabling users to easily extend functionality, integrate with external
systems, and customize neural network behavior.

Implementation of this plugin system requires careful consideration of:
- Plugin loading and unloading
- Plugin dependency management
- Plugin security and sandboxing
- Plugin lifecycle management
- Plugin configuration and customization
- Plugin communication and data exchange
- Plugin error handling and recovery

The plugin system provides a powerful foundation for building complex, extensible neural
network applications with NLM.
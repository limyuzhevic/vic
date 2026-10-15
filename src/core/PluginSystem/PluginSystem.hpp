// Plugin Architecture for NLM
// Phase 3: Plugin System for Extensible Neural Computation

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>
#include <any>
#include <mutex>

namespace nlm {

// Plugin types
enum class PluginType {
    NeuralNetwork,    // Neural network implementations
    LearningRule,     // Learning algorithms (STDP, Hebbian, etc.)
    Visualization,    // Visualization plugins
    MemorySystem,     // Memory management plugins
    Neuromodulation,  // Neuromodulation plugins
    Development,      // Developmental process plugins
    Sensor,           // Sensory processing plugins
    Motor,            // Motor control plugins
    Custom           // User-defined plugins
};

// Plugin capabilities
struct PluginCapabilities {
    PluginType type;
    std::string name;
    std::string version;
    std::string description;
    std::vector<std::string> dependencies; // Plugin names that must be loaded first
    bool requiresBrain = false;
    bool requiresWorld = false;
    bool isSingleton = false; // Can only have one instance
    
    PluginCapabilities() : type(PluginType::Custom), requiresBrain(false), requiresWorld(false), isSingleton(false) {}
};

// Base plugin interface
class PluginBase {
public:
    virtual ~PluginBase() = default;
    
    // Initialize plugin with configuration
    virtual bool initialize(const std::any& config) = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
    
    // Get plugin capabilities
    virtual PluginCapabilities getCapabilities() const = 0;
    
    // Called when brain is initialized (if required)
    virtual void onBrainInitialized(class Brain* brain) {}
    
    // Called when world is initialized (if required)
    virtual void onWorldInitialized(class Environment* world) {}
    
    // Update plugin (called every simulation step)
    virtual void update(double timestep) {}
    
    // Get plugin-specific data
    virtual std::any getData(const std::string& key) const { return std::any(); }
    
    // Set plugin-specific data
    virtual void setData(const std::string& key, const std::any& value) {}
    
    // Execute plugin-specific operation
    virtual std::any execute(const std::string& operation, const std::any& params) { return std::any(); }
    
    // Check if plugin is enabled
    virtual bool isEnabled() const { return true; }
    
    // Enable/disable plugin
    virtual void setEnabled(bool enabled) {}
};

// Neural Network Plugin Interface
class NeuralNetworkPlugin : public PluginBase {
public:
    virtual void initializeNetwork(const std::string& architecture, const std::any& params) = 0;
    virtual void forward(class Neuron* neuron, double input) = 0;
    virtual double getOutput(class Neuron* neuron) const = 0;
    virtual void reset() = 0;
    
    // Neural network specific data
    virtual std::vector<float> getWeights() const = 0;
    virtual void setWeights(const std::vector<float>& weights) = 0;
    virtual size_t getParameterCount() const = 0;
};

// Learning Rule Plugin Interface
class LearningRulePlugin : public PluginBase {
public:
    virtual void computeWeightChange(class Synapse* synapse, 
                                    class Neuron* preNeuron, 
                                    class Neuron* postNeuron,
                                    double reward = 0.0) = 0;
    virtual void applyWeightChange(class Synapse* synapse, float delta) = 0;
    virtual float getLearningRate() const = 0;
    virtual void setLearningRate(float rate) = 0;
    
    // Learning rule specific data
    virtual float getMomentum() const = 0;
    virtual void setMomentum(float momentum) = 0;
    virtual float getL1Decay() const = 0;
    virtual void setL1Decay(float decay) = 0;
};

// Visualization Plugin Interface
class VisualizationPlugin : public PluginBase {
public:
    virtual void render(class Brain* brain, double timestamp) = 0;
    virtual void setOutputMode(const std::string& mode) = 0;
    virtual std::string getOutputMode() const = 0;
    virtual void saveVisualization(const std::string& filepath) = 0;
    
    // Visualization specific data
    virtual void setStyle(const std::string& style) = 0;
    virtual std::string getStyle() const = 0;
    virtual void setColorMap(const std::vector<float>& colormap) = 0;
    virtual std::vector<float> getColorMap() const = 0;
};

// Memory System Plugin Interface
class MemorySystemPlugin : public PluginBase {
public:
    virtual void store(const std::string& key, const std::any& value, double ttl = -1.0) = 0;
    virtual std::any retrieve(const std::string& key) = 0;
    virtual bool exists(const std::string& key) const = 0;
    virtual void remove(const std::string& key) = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual std::vector<std::string> getKeys() const = 0;
    
    // Memory management
    virtual void setCapacity(size_t capacity) = 0;
    virtual size_t getCapacity() const = 0;
    virtual void setEvictionPolicy(const std::string& policy) = 0; // "lru", "lfu", "fifo"
    virtual std::string getEvictionPolicy() const = 0;
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<PluginBase>(const std::any& config)>;

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Load a plugin from a shared library
    bool loadPlugin(const std::string& filepath);
    
    // Load a plugin from memory (for embedded scenarios)
    bool loadPlugin(const std::string& name, PluginFactory factory, 
                   const std::any& config = std::any());
    
    // Unload a plugin
    bool unloadPlugin(const std::string& name);
    
    // Get loaded plugin
    std::shared_ptr<PluginBase> getPlugin(const std::string& name) const;
    
    // Get all loaded plugins of a specific type
    std::vector<std::shared_ptr<PluginBase>> getPluginsByType(PluginType type) const;
    
    // Get all loaded plugins
    std::vector<std::shared_ptr<PluginBase>> getAllPlugins() const;
    
    // Initialize all plugins
    bool initializeAllPlugins(const std::unordered_map<std::string, std::any>& configs);
    
    // Shutdown all plugins
    void shutdownAllPlugins();
    
    // Check if plugin is loaded
    bool isPluginLoaded(const std::string& name) const;
    
    // Get plugin capabilities
    PluginCapabilities getPluginCapabilities(const std::string& name) const;
    
    // Get plugin names
    std::vector<std::string> getPluginNames() const;
    
    // Set plugin search path
    void setPluginSearchPath(const std::vector<std::string>& paths);
    
    // Discover available plugins
    std::vector<std::string> discoverPlugins(const std::string& directory);
    
    // Plugin lifecycle hooks
    void onBrainInitialized(class Brain* brain);
    void onWorldInitialized(class Environment* world);
    void onUpdate(double timestep);
    
    // Plugin-specific data
    void setGlobalData(const std::string& key, const std::any& value);
    std::any getGlobalData(const std::string& key) const;
    
    // Plugin registry for built-in plugins
    static void registerBuiltinPlugin(const std::string& name, PluginFactory factory, 
                                     const PluginCapabilities& capabilities);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin macros for easier plugin development
#define REGISTER_PLUGIN(name, factory, capabilities) \
    namespace { \
        struct PluginRegistry_##name { \
            PluginRegistry_##name() { \
                PluginManager::registerBuiltinPlugin(name, factory, capabilities); \
            } \
        } plugin_registry_##name; \
    }

#define PLUGIN_FACTORY(PluginClass) \
    [](const std::any& config) -> std::unique_ptr<PluginBase> { \
        return std::make_unique<PluginClass>(); \
    }

// Example built-in plugin implementations

// Simple neural network plugin (placeholder)
class SimpleNeuralNetworkPlugin : public NeuralNetworkPlugin {
public:
    bool initialize(const std::any& config) override;
    void shutdown() override;
    PluginCapabilities getCapabilities() const override;
    
    void initializeNetwork(const std::string& architecture, const std::any& params) override;
    void forward(class Neuron* neuron, double input) override;
    double getOutput(class Neuron* neuron) const override;
    void reset() override;
    
    std::vector<float> getWeights() const override;
    void setWeights(const std::vector<float>& weights) override;
    size_t getParameterCount() const override;
    
    void update(double timestep) override;
    
private:
    std::vector<float> weights_;
    std::string architecture_;
};

// Simple learning rule plugin (placeholder)
class SimpleSTDPPlugin : public LearningRulePlugin {
public:
    bool initialize(const std::any& config) override;
    void shutdown() override;
    PluginCapabilities getCapabilities() const override;
    
    void computeWeightChange(class Synapse* synapse, 
                            class Neuron* preNeuron, 
                            class Neuron* postNeuron,
                            double reward) override;
    void applyWeightChange(class Synapse* synapse, float delta) override;
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
    float getMomentum() const override;
    void setMomentum(float momentum) override;
    float getL1Decay() const override;
    void setL1Decay(float decay) override;
    
    void update(double timestep) override;
    
private:
    float learningRate_;
    float momentum_;
    float l1Decay_;
};

// Simple visualization plugin (placeholder)
class SimpleVisualizationPlugin : public VisualizationPlugin {
public:
    bool initialize(const std::any& config) override;
    void shutdown() override;
    PluginCapabilities getCapabilities() const override;
    
    void render(class Brain* brain, double timestamp) override;
    void setOutputMode(const std::string& mode) override;
    std::string getOutputMode() const override;
    void saveVisualization(const std::string& filepath) override;
    
    void setStyle(const std::string& style) override;
    std::string getStyle() const override;
    void setColorMap(const std::vector<float>& colormap) override;
    std::vector<float> getColorMap() const override;
    
    void update(double timestep) override;
    
private:
    std::string outputMode_;
    std::string style_;
    std::vector<float> colormap_;
};

} // namespace nlm
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "../core/Config/Config.hpp"

namespace nlm {

// Plugin interface for extensibility
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Get plugin name
    virtual std::string getName() const = 0;
    
    // Get plugin version
    virtual std::string getVersion() const = 0;
    
    // Initialize plugin with configuration
    virtual bool initialize(const Config& config) = 0;
    
    // Cleanup plugin
    virtual void cleanup() = 0;
    
    // Get plugin metadata as string
    virtual std::string getMetadata() const = 0;
};

// Template base class for specific plugin types
class IPlasticityRule : public IPlugin {
public:
    virtual ~IPlasticityRule() = default;
    
    // Apply plasticity change based on pre- and post-synaptic activity
    virtual void apply(const PlasticityContext& context) = 0;
    
    // Get learning rate for this rule
    virtual float getLearningRate() const = 0;
    
    // Get rule parameters
    virtual std::unordered_map<std::string, float> getParameters() const = 0;
    
    // Update rule parameters
    virtual void updateParameters(const std::unordered_map<std::string, float>& params) = 0;
};

class IMemorySystem : public IPlugin {
public:
    virtual ~IMemorySystem() = default;
    
    // Store information in memory
    virtual void store(const MemoryEntry& entry) = 0;
    
    // Retrieve information from memory
    virtual std::optional<MemoryEntry> retrieve(const std::string& key) const = 0;
    
    // Clear memory
    virtual void clear() = 0;
    
    // Get memory capacity
    virtual size_t getCapacity() const = 0;
};

class IBehaviorModule : public IPlugin {
public:
    virtual ~IBehaviorModule() = default;
    
    // Select action based on current state
    virtual std::unique_ptr<Action> selectAction(const State& state) = 0;
    
    // Update module with new experience
    virtual void update(const Experience& experience) = 0;
    
    // Get module confidence
    virtual float getConfidence() const = 0;
};

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Disable copying, enable moving
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) noexcept = default;
    PluginManager& operator=(PluginManager&&) noexcept = default;
    
    // Load plugin from shared library
    bool loadPlugin(const std::string& libraryPath);
    
    // Load plugin from inline code (development/testing)
    bool loadInlinePlugin(std::unique_ptr<IPlugin> plugin);
    
    // Unload plugin
    bool unloadPlugin(const std::string& pluginName);
    
    // Get loaded plugin
    std::shared_ptr<IPlugin> getPlugin(const std::string& name) const;
    
    // Get all loaded plugins
    std::vector<std::shared_ptr<IPlugin>> getAllPlugins() const;
    
    // Get plasticity rule plugins
    std::vector<std::shared_ptr<IPlasticityRule>> getPlasticityRules() const;
    
    // Get memory system plugins
    std::vector<std::shared_ptr<IMemorySystem>> getMemorySystems() const;
    
    // Get behavior module plugins
    std::vector<std::shared_ptr<IBehaviorModule>> getBehaviorModules() const;
    
    // Initialize all plugins with configuration
    bool initializeAllPlugins(const Config& config);
    
    // Cleanup all plugins
    void cleanupAllPlugins();
    
    // Create component factory for easy integration
    template<typename T, typename... Args>
    std::unique_ptr<T> createComponent(Args&&... args);
    
    // Register built-in plugins
    void registerBuiltInPlugins();
    
    // Get system capabilities
    std::vector<std::string> getSupportedFormats() const;
    bool supportsPluginFormat(const std::string& format) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Component factory for creating NLM components
class ComponentFactory {
public:
    ComponentFactory();
    ~ComponentFactory();
    
    // Create brain with custom components
    std::shared_ptr<Brain> createBrain(const Config& config,
                                      const std::vector<std::shared_ptr<IPlasticityRule>>& plasticityRules,
                                      const std::vector<std::shared_ptr<IMemorySystem>>& memorySystems,
                                      const std::vector<std::shared_ptr<IBehaviorModule>>& behaviorModules);
    
    // Create neuron with custom properties
    std::unique_ptr<Neuron> createNeuron(NeuronId id,
                                        const std::unordered_map<std::string, float>& properties);
    
    // Create synapse with custom properties
    std::unique_ptr<Synapse> createSynapse(NeuronId source, NeuronId destination,
                                          const std::unordered_map<std::string, float>& properties);
    
    // Register component factory
    template<typename T>
    void registerComponent(const std::string& name,
                          std::function<std::unique_ptr<T>(const Config&, const std::unordered_map<std::string, float>&)> factory);
    
    // Get registered component names
    std::vector<std::string> getRegisteredComponents() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin configuration system
class PluginConfig {
public:
    PluginConfig();
    ~PluginConfig();
    
    // Load plugin configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Load plugin configuration from string
    bool loadFromString(const std::string& configString);
    
    // Get plugin configuration
    std::optional<Config> getPluginConfig(const std::string& pluginName) const;
    
    // Set plugin configuration
    void setPluginConfig(const std::string& pluginName, const Config& config);
    
    // Add plugin dependency
    void addDependency(const std::string& pluginName, const std::string& dependency);
    
    // Get plugin dependencies
    std::vector<std::string> getDependencies(const std::string& pluginName) const;
    
    // Validate plugin configuration
    bool validatePlugin(const std::string& pluginName) const;
    
    // Get all plugin names
    std::vector<std::string> getPluginNames() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Example custom plugins for demonstration
namespace Examples {
    // Advanced STDP rule with eligibility traces
    class AdvancedSTDP : public IPlasticityRule {
    public:
        std::string getName() const override { return "AdvancedSTDP"; }
        std::string getVersion() const override { return "1.0.0"; }
        bool initialize(const Config& config) override;
        void cleanup() override {}
        std::string getMetadata() const override;
        
        void apply(const PlasticityContext& context) override;
        float getLearningRate() const override { return learningRate_; }
        std::unordered_map<std::string, float> getParameters() const override;
        void updateParameters(const std::unordered_map<std::string, float>& params) override;
        
    private:
        float learningRate_ = 0.02f;
        float eligibilityDecay_ = 0.99f;
        float maxTrace_ = 1.0f;
    };
    
    // Hierarchical memory system
    class HierarchicalMemory : public IMemorySystem {
    public:
        std::string getName() const override { return "HierarchicalMemory"; }
        std::string getVersion() const override { return "1.0.0"; }
        bool initialize(const Config& config) override;
        void cleanup() override {}
        std::string getMetadata() const override;
        
        void store(const MemoryEntry& entry) override;
        std::optional<MemoryEntry> retrieve(const std::string& key) const override;
        void clear() override;
        size_t getCapacity() const override { return totalCapacity_; }
        
    private:
        size_t totalCapacity_ = 1000;
        size_t workingCapacity_ = 100;
        size_t episodicCapacity_ = 500;
        size_t semanticCapacity_ = 400;
    };
    
    // Monte Carlo tree search planning
    class MCTPlanning : public IBehaviorModule {
    public:
        std::string getName() const override { return "MCTPlanning"; }
        std::string getVersion() const override { return "1.0.0"; }
        bool initialize(const Config& config) override;
        void cleanup() override {}
        std::string getMetadata() const override;
        
        std::unique_ptr<Action> selectAction(const State& state) override;
        void update(const Experience& experience) override;
        float getConfidence() const override { return confidence_; }
        
    private:
        float confidence_ = 0.8f;
        size_t searchDepth_ = 10;
        float explorationRate_ = 0.1f;
    };
}

} // namespace nlm
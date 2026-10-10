#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <typeindex>
#include <stdexcept>

namespace nlm {

// Plugin interface for brain components
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
};

// Factory function type for creating plugins
using PluginFactory = std::function<std::unique_ptr<IPlugin>()>;

// Plugin registry for managing plugins
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Disable copying
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    
    // Register a plugin factory
    static void registerPlugin(const std::string& name, PluginFactory factory);
    
    // Load a plugin by name
    static std::unique_ptr<IPlugin> loadPlugin(const std::string& name);
    
    // Get all registered plugin names
    static std::vector<std::string> getPluginNames();
    
    // Check if plugin is registered
    static bool isPluginRegistered(const std::string& name);
    
    // Initialize all plugins
    static void initializeAllPlugins();
    
    // Shutdown all plugins
    static void shutdownAllPlugins();
    
private:
    static std::unordered_map<std::string, PluginFactory> pluginFactories_;
    static std::unordered_map<std::string, std::unique_ptr<IPlugin>> loadedPlugins_;
    static bool pluginsInitialized_;
};

// Brain region plugin interface
class IBrainRegionPlugin : public IPlugin {
public:
    virtual ~IBrainRegionPlugin() = default;
    
    // Create a neural region instance
    virtual std::unique_ptr<class NeuralRegion> createRegion(class Brain* brain, class RandomGenerator& rng) = 0;
    
    // Get default configuration for this region type
    virtual std::unordered_map<std::string, std::string> getDefaultConfig() const = 0;
    
    // Get region capabilities
    virtual std::vector<std::string> getCapabilities() const = 0;
};

// Neural population plugin interface
class INeuralPopulationPlugin : public IPlugin {
public:
    virtual ~INeuralPopulationPlugin() = default;
    
    // Create neurons for this population type
    virtual std::vector<class Neuron*> createNeurons(class Brain* brain, size_t count, class RandomGenerator& rng) = 0;
    
    // Get population properties
    virtual std::unordered_map<std::string, double> getProperties() const = 0;
    
    // Get neuron type for this population
    virtual class NeuronType getNeuronType() const = 0;
};

// Plasticity rule plugin interface
class IPlasticityRulePlugin : public IPlugin {
public:
    virtual ~IPlasticityRulePlugin() = default;
    
    // Apply plasticity to a synapse
    virtual void apply(class Synapse* synapse, double weightChange) = 0;
    
    // Get rule parameters
    virtual std::unordered_map<std::string, double> getParameters() const = 0;
    
    // Set rule parameters
    virtual void setParameters(const std::unordered_map<std::string, double>& params) = 0;
};

// Neuromodulation plugin interface
class INeuromodulationPlugin : public IPlugin {
public:
    virtual ~INeuromodulationPlugin() = default;
    
    // Update neuromodulator level based on brain state
    virtual void update(class Brain* brain, double timestep) = 0;
    
    // Get current modulation level
    virtual double getLevel() const = 0;
    
    // Get modulation effect on target (plasticity, excitability, etc.)
    virtual double getEffect(const std::string& target) const = 0;
};

// Default plugins

// Default brain region plugin
class DefaultBrainRegionPlugin : public IBrainRegionPlugin {
public:
    std::string getName() const override { return "DefaultBrainRegion"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Default brain region implementation"; }
    
    bool initialize() override { return true; }
    void shutdown() override {}
    bool isInitialized() const override { return true; }
    
    std::unique_ptr<class NeuralRegion> createRegion(class Brain* brain, class RandomGenerator& rng) override;
    std::unordered_map<std::string, std::string> getDefaultConfig() const override;
    std::vector<std::string> getCapabilities() const override;
};

// Default neural population plugins
class DefaultSensoryPopulationPlugin : public INeuralPopulationPlugin {
public:
    std::string getName() const override { return "DefaultSensoryPopulation"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Default sensory neural population"; }
    
    bool initialize() override { return true; }
    void shutdown() override {}
    bool isInitialized() const override { return true; }
    
    std::vector<class Neuron*> createNeurons(class Brain* brain, size_t count, class RandomGenerator& rng) override;
    std::unordered_map<std::string, double> getProperties() const override;
    class NeuronType getNeuronType() const override;
};

class DefaultInternalPopulationPlugin : public INeuralPopulationPlugin {
public:
    std::string getName() const override { return "DefaultInternalPopulation"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Default internal neural population"; }
    
    bool initialize() override { return true; }
    void shutdown() override {}
    bool isInitialized() const override { return true; }
    
    std::vector<class Neuron*> createNeurons(class Brain* brain, size_t count, class RandomGenerator& rng) override;
    std::unordered_map<std::string, double> getProperties() const override;
    class NeuronType getNeuronType() const override;
};

class DefaultMotorPopulationPlugin : public INeuralPopulationPlugin {
public:
    std::string getName() const override { return "DefaultMotorPopulation"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Default motor neural population"; }
    
    bool initialize() override { return true; }
    void shutdown() override {}
    bool isInitialized() const override { return true; }
    
    std::vector<class Neuron*> createNeurons(class Brain* brain, size_t count, class RandomGenerator& rng) override;
    std::unordered_map<std::string, double> getProperties() const override;
    class NeuronType getNeuronType() const override;
};

// Default plasticity rule plugins
class DefaultSTDPPlugin : public IPlasticityRulePlugin {
public:
    std::string getName() const override { return "DefaultSTDP"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Default Spike-Timing Dependent Plasticity rule"; }
    
    bool initialize() override { return true; }
    void shutdown() override {}
    bool isInitialized() const override { return true; }
    
    void apply(class Synapse* synapse, double weightChange) override;
    std::unordered_map<std::string, double> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, double>& params) override;
};

class DefaultHebbianPlugin : public IPlasticityRulePlugin {
public:
    std::string getName() const override { return "DefaultHebbian"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Default Hebbian learning rule"; }
    
    bool initialize() override { return true; }
    void shutdown() override {}
    bool isInitialized() const override { return true; }
    
    void apply(class Synapse* synapse, double weightChange) override;
    std::unordered_map<std::string, double> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, double>& params) override;
};

// Default neuromodulation plugins
class DefaultDopaminePlugin : public INeuromodulationPlugin {
private:
    double level_ = 0.0f;
public:
    std::string getName() const override { return "DefaultDopamine"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Default dopamine neuromodulator"; }
    
    bool initialize() override { return true; }
    void shutdown() override {}
    bool isInitialized() const override { return true; }
    
    void update(class Brain* brain, double timestep) override;
    double getLevel() const override;
    double getEffect(const std::string& target) const override;
};

class DefaultCuriosityPlugin : public INeuromodulationPlugin {
private:
    double curiosityLevel_ = 0.0f;
public:
    std::string getName() const override { return "DefaultCuriosity"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Default curiosity neuromodulator"; }
    
    bool initialize() override { return true; }
    void shutdown() override {}
    bool isInitialized() const override { return true; }
    
    void update(class Brain* brain, double timestep) override;
    double getLevel() const override;
    double getEffect(const std::string& target) const override;
};

class DefaultNoveltyPlugin : public INeuromodulationPlugin {
private:
    double noveltyLevel_ = 0.0f;
public:
    std::string getName() const override { return "DefaultNovelty"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Default novelty neuromodulator"; }
    
    bool initialize() override { return true; }
    void shutdown() override {}
    bool isInitialized() const override { return true; }
    
    void update(class Brain* brain, double timestep) override;
    double getLevel() const override;
    double getEffect(const std::string& target) const override;
};

// Plugin loader for dynamic loading
class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();
    
    // Load plugin from file
    bool loadFromFile(const std::string& filepath);
    
    // Get loaded plugin
    std::unique_ptr<IPlugin> getPlugin(const std::string& name) const;
    
    // Check if plugin is loaded
    bool isPluginLoaded(const std::string& name) const;
    
    // List loaded plugins
    std::vector<std::string> getLoadedPluginNames() const;
    
    // Unload plugin
    bool unloadPlugin(const std::string& name);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions for working with plugins
namespace Plugins {
    // Register all built-in plugins
    void registerBuiltInPlugins();
    
    // Get default brain region configuration
    std::unordered_map<std::string, std::string> getDefaultBrainRegionConfig(const std::string& regionType);
    
    // Get default neural population configuration
    std::unordered_map<std::string, double> getDefaultPopulationProperties(const std::string& populationType);
    
    // Get default neuromodulation parameters
    std::unordered_map<std::string, double> getDefaultNeuromodulationParams(const std::string& modulatorType);
    
    // Validate plugin compatibility
    bool validatePluginCompatibility(const IPlugin& plugin, const std::string& brainVersion);
    
    // Get plugin information
    std::string getPluginInfo(const std::string& pluginName);
}

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

// Forward declarations for NLM core classes
namespace nlm {
    class Config;
    class Brain;
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class NeuralAssociativeMemory;
    class PredictionSystem;
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
    class DevelopmentSystem;
    class Dopamine;
    class Curiosity;
    class Novelty;
    class PredictionError;
    class SpikeSystem;
    class STDP;
    class Hebbian;
    class StructuralPlasticity;
    class Development;
    class Neuromodulator;
    class SensoryInput;
    class Action;
    class Environment;
    class Observation;
    
    // Developmental stages
    enum class DevelopmentalStage {
        Initial,
        CriticalPeriod,
        Maturation,
        Adult,
        Aging
    };
}

namespace nlm {

// Plugin type enumeration
enum class PluginType {
    MEMORY,
    PLASTICITY,
    NEUROMODULATION,
    PREDICTION,
    COGNITION,
    DEVELOPMENT,
    SENSOR,
    MOTOR,
    UTILITY
};

// Plugin base class
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Get plugin name
    virtual std::string getName() const = 0;
    
    // Get plugin version
    virtual std::string getVersion() const = 0;
    
    // Get plugin type
    virtual PluginType getType() const = 0;
    
    // Get plugin description
    virtual std::string getDescription() const = 0;
    
    // Initialize plugin with configuration
    virtual bool initialize(const std::unordered_map<std::string, std::string>& config) = 0;
    
    // Update plugin state
    virtual void update(double timestep) = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
    
    // Check if plugin is initialized
    virtual bool isInitialized() const = 0;
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<Plugin>()>;

// Plugin registry for easy discovery
class PluginRegistry {
private:
    std::unordered_map<std::string, PluginFactory> factories;
    std::unordered_map<std::string, std::string> metadata;
    std::unordered_map<std::string, PluginType> pluginTypes;
    
public:
    // Register a plugin factory
    static void registerPlugin(const std::string& name, 
                              PluginFactory factory,
                              const std::string& version = "1.0.0",
                              const std::string& description = "",
                              PluginType type = PluginType::UTILITY);
    
    // Create a plugin instance
    static std::unique_ptr<Plugin> createPlugin(const std::string& name);
    
    // Get all registered plugin names
    static std::vector<std::string> getRegisteredPlugins();
    
    // Get plugin metadata
    static std::string getPluginVersion(const std::string& name);
    static std::string getPluginDescription(const std::string& name);
    static PluginType getPluginType(const std::string& name);
    
    // Check if plugin is registered
    static bool isPluginRegistered(const std::string& name);
};

// Plugin manager interface
class PluginManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Plugin>> activePlugins;
    std::unordered_map<PluginType, std::vector<std::string>> pluginsByType;
    std::shared_ptr<Config> config;
    bool initialized;
    
public:
    PluginManager();
    virtual ~PluginManager() = default;
    
    // Initialize manager with configuration
    virtual bool initialize(const std::unordered_map<std::string, std::string>& config);
    
    // Load a plugin
    virtual bool loadPlugin(const std::string& name);
    
    // Unload a plugin
    virtual bool unloadPlugin(const std::string& name);
    
    // Update all active plugins
    virtual void update(double timestep);
    
    // Shutdown all plugins
    virtual void shutdown();
    
    // Get plugin by name
    virtual Plugin* getPlugin(const std::string& name);
    
    // Get all plugins
    virtual std::vector<Plugin*> getAllPlugins();
    
    // Get plugins by type
    virtual std::vector<Plugin*> getPluginsByType(PluginType type);
    
    // Check if plugin is loaded
    virtual bool isPluginLoaded(const std::string& name);
    
    // Get all loaded plugin names
    virtual std::vector<std::string> getLoadedPlugins();
};

// Memory system plugin interface
class MemoryPlugin : public Plugin {
public:
    virtual ~MemoryPlugin() = default;
    
    // Get working memory capacity
    virtual size_t getWorkingMemoryCapacity() const = 0;
    
    // Set working memory capacity
    virtual void setWorkingMemoryCapacity(size_t capacity) = 0;
    
    // Get working memory usage
    virtual size_t getWorkingMemoryUsage() const = 0;
    
    // Get episodic memory capacity
    virtual size_t getEpisodicMemoryCapacity() const = 0;
    
    // Set episodic memory capacity
    virtual void setEpisodicMemoryCapacity(size_t capacity) = 0;
    
    // Get episodic memory usage
    virtual size_t getEpisodicMemoryUsage() const = 0;
};

// Plasticity system plugin interface
class PlasticityPlugin : public Plugin {
public:
    virtual ~PlasticityPlugin() = default;
    
    // Get plasticity strength
    virtual float getPlasticityStrength() const = 0;
    
    // Set plasticity strength
    virtual void setPlasticityStrength(float strength) = 0;
    
    // Get learning rate
    virtual float getLearningRate() const = 0;
    
    // Set learning rate
    virtual void setLearningRate(float rate) = 0;
    
    // Get stability factor
    virtual float getStabilityFactor() const = 0;
    
    // Set stability factor
    virtual void setStabilityFactor(float factor) = 0;
};

// Neuromodulation system plugin interface
class NeuromodulationPlugin : public Plugin {
public:
    virtual ~NeuromodulationPlugin() = default;
    
    // Get dopamine level
    virtual float getDopamineLevel() const = 0;
    
    // Set dopamine level
    virtual void setDopamineLevel(float level) = 0;
    
    // Get curiosity level
    virtual float getCuriosityLevel() const = 0;
    
    // Set curiosity level
    virtual void setCuriosityLevel(float level) = 0;
    
    // Get novelty level
    virtual float getNoveltyLevel() const = 0;
    
    // Set novelty level
    virtual void setNoveltyLevel(float level) = 0;
    
    // Get prediction error
    virtual float getPredictionError() const = 0;
    
    // Set prediction error
    virtual void setPredictionError(float error) = 0;
};

// Development system plugin interface
class DevelopmentPlugin : public Plugin {
public:
    virtual ~DevelopmentPlugin() = default;
    
    // Get developmental stage
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    
    // Set developmental stage
    virtual void setDevelopmentalStage(DevelopmentalStage stage) = 0;
    
    // Get developmental age
    virtual double getDevelopmentalAge() const = 0;
    
    // Set developmental age
    virtual void setDevelopmentalAge(double age) = 0;
    
    // Get maturation rate
    virtual float getMaturationRate() const = 0;
    
    // Set maturation rate
    virtual void setMaturationRate(float rate) = 0;
};

// Plugin utility functions
namespace plugin_utils {
    // Load all plugins from directory
    std::vector<std::string> loadPluginsFromDirectory(const std::string& directory);
    
    // Save plugin configuration
    bool savePluginConfig(const std::string& filename, 
                         const std::unordered_map<std::string, std::string>& config);
    
    // Load plugin configuration
    bool loadPluginConfig(const std::string& filename, 
                         std::unordered_map<std::string, std::string>& config);
    
    // Get available plugin types
    std::vector<PluginType> getAvailablePluginTypes();
    
    // Create default plugin configuration
    std::unordered_map<std::string, std::string> createDefaultPluginConfig(PluginType type);
}

} // namespace nlm
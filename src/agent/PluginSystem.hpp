#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <typeinfo>
#include <nlohmann/json.hpp>

namespace nlm {

// Plugin system version
const std::string PLUGIN_SYSTEM_VERSION = "1.0.0";

// Plugin load result
enum class PluginLoadResult {
    SUCCESS,
    FAILED,
    ALREADY_LOADED,
    VERSION_MISMATCH,
    DEPENDENCY_MISSING,
    INVALID_FORMAT
};

// Plugin type
enum class PluginType {
    CORE,
    UTILITY,
    BEHAVIOR,
    MEMORY,
    COGNITIVE,
    CUSTOM
};

// Plugin interface - base class for all plugins
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginType getType() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const std::map<std::string, std::string>& config) { return true; }
    virtual void shutdown() {}
    
    // Plugin functionality
    virtual void update(double timestep) {}
    virtual std::string getStatus() const { return "Active"; }
    
    // Dependencies
    virtual std::vector<std::string> getDependencies() const { return {}; }
    virtual bool hasDependency(const std::string& pluginName) const {
        return std::find(getDependencies().begin(), getDependencies().end(), pluginName) != getDependencies().end();
    }
    
    // Metadata
    virtual std::map<std::string, std::string> getMetadata() const { return {}; }
    
    // Plugin capabilities
    virtual std::vector<std::string> getCapabilities() const { return {}; }
    virtual bool hasCapability(const std::string& capability) const {
        return std::find(getCapabilities().begin(), getCapabilities().end(), capability) != getCapabilities().end();
    }
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<Plugin>()>;

// Plugin loader interface
class PluginLoader {
public:
    virtual ~PluginLoader() = default;
    
    // Load a plugin from file
    virtual PluginLoadResult loadPlugin(const std::string& filepath) = 0;
    
    // Unload a plugin
    virtual bool unloadPlugin(const std::string& name) = 0;
    
    // Check if plugin is loaded
    virtual bool isPluginLoaded(const std::string& name) const = 0;
    
    // Get loaded plugin names
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
    
    // Get plugin information
    virtual std::string getPluginInfo(const std::string& name) const = 0;
    
    // Reload a plugin
    virtual PluginLoadResult reloadPlugin(const std::string& name) = 0;
};

// Plugin manager - central registry for all plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Disable copying, enable moving
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) noexcept;
    PluginManager& operator=(PluginManager&&) noexcept;
    
    // Get singleton instance
    static PluginManager& getInstance();
    
    // Plugin management
    PluginLoadResult loadPlugin(const std::string& filepath);
    PluginLoadResult loadPlugin(const std::string& name, PluginFactory factory);
    
    bool unloadPlugin(const std::string& name);
    bool unloadAllPlugins();
    
    // Plugin information
    bool isPluginLoaded(const std::string& name) const;
    std::vector<std::string> getLoadedPlugins() const;
    std::string getPluginInfo(const std::string& name) const;
    
    // Plugin lifecycle
    bool initializePlugin(const std::string& name, const std::map<std::string, std::string>& config = {});
    void shutdownPlugin(const std::string& name);
    void initializeAllPlugins(const std::map<std::string, std::map<std::string, std::string>>& configs = {});
    void shutdownAllPlugins();
    
    // Plugin update
    void updateAllPlugins(double timestep);
    std::string getAllPluginStatus() const;
    
    // Plugin querying
    std::vector<std::string> getPluginsByType(PluginType type) const;
    std::vector<std::string> getPluginsWithCapability(const std::string& capability) const;
    std::vector<std::string> getPluginsWithDependency(const std::string& dependency) const;
    
    // Configuration
    bool configurePlugin(const std::string& name, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& name) const;
    
    // Plugin system info
    std::string getSystemInfo() const;
    bool savePluginState(const std::string& filepath) const;
    bool loadPluginState(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void initializeDependencies(const std::string& pluginName);
    bool checkDependencies(const std::string& pluginName) const;
    std::string getDependencyError(const std::string& pluginName) const;
    void cleanupDependencies(const std::string& pluginName);
};

// Plugin base classes for common functionality

// Component plugin - provides a component that can be integrated into the brain
class ComponentPlugin : public Plugin {
public:
    virtual ~ComponentPlugin() = default;
    
    // Get component type
    virtual std::string getComponentType() const = 0;
    
    // Initialize component (called by system)
    virtual bool initializeComponent(std::shared_ptr<void> systemContext) { return true; }
    
    // Get component instance
    virtual std::shared_ptr<void> getComponentInstance() { return nullptr; }
};

// Behavior plugin - provides custom behavior patterns
class BehaviorPlugin : public Plugin {
public:
    virtual ~BehaviorPlugin() = default;
    
    // Behavior execution
    virtual void executeBehavior(const std::map<std::string, std::string>& context) {}
    
    // Behavior conditions
    virtual bool meetsCondition(const std::string& condition) const { return false; }
    
    // Get behavioral effects
    virtual std::map<std::string, double> getBehavioralEffects() const { return {}; }
};

// Memory plugin - provides custom memory systems
class MemoryPlugin : public Plugin {
public:
    virtual ~MemoryPlugin() = default;
    
    // Memory operations
    virtual void store(const std::string& key, const ConfigValue& value) {}
    virtual std::optional<ConfigValue> retrieve(const std::string& key) const { return std::nullopt; }
    virtual void erase(const std::string& key) {}
    virtual bool contains(const std::string& key) const { return false; }
    virtual void clear() {}
    
    // Memory queries
    virtual std::vector<std::string> getKeys() const { return {}; }
    virtual size_t size() const { return 0; }
    virtual double getMemoryUsage() const { return 0.0; }
};

// Built-in plugins

// Core brain plugin
class CoreBrainPlugin : public ComponentPlugin {
public:
    std::string getName() const override { return "core_brain"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Core brain simulation component"; }
    PluginType getType() const override { return PluginType::CORE; }
    std::string getComponentType() const override { return "brain"; }
};

// Memory system plugin
class MemorySystemPlugin : public ComponentPlugin {
public:
    std::string getName() const override { return "memory_system"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Advanced memory management system"; }
    PluginType getType() const override { return PluginType::MEMORY; }
    std::string getComponentType() const override { return "memory_system"; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"working_memory", "episodic_memory", "semantic_memory", "procedural_memory", "associative_memory"};
    }
};

// Neural dynamics plugin
class NeuralDynamicsPlugin : public ComponentPlugin {
public:
    std::string getName() const override { return "neural_dynamics"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Advanced neural dynamics simulation"; }
    PluginType getType() const override { return PluginType::CORE; }
    std::string getComponentType() const override { return "dynamics";; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"lif_dynamics", "event_driven", "plasticity", "structural_plasticity"};
    }
};

// Neuromodulation plugin
class NeuromodulationPlugin : public ComponentPlugin {
public:
    std::string getName() const override { return "neuromodulation"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Advanced neuromodulation system"; }
    PluginType getType() const override { return PluginType::CORE; }
    std::string getComponentType() const override { return "neuromodulation";; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"dopamine", "curiosity", "novelty", "prediction_error", "reward"};
    }
};

// Utility plugins

// Logger plugin
class LoggerPlugin : public ComponentPlugin {
public:
    std::string getName() const override { return "logger"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Advanced logging and monitoring"; }
    PluginType getType() const override { return PluginType::UTILITY; }
    std::string getComponentType() const override { return "logger";; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"console_logging", "file_logging", "log_levels", "log_filtering"};
    }
};

// Random number generator plugin
class RandomGeneratorPlugin : public ComponentPlugin {
public:
    std::string getName() const override { return "random_generator"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Advanced random number generation"; }
    PluginType getType() const override { return PluginType::UTILITY; }
    std::string getComponentType() const override { return "random_generator";; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"mersenne_twister", "pcg", "xoshiro", "seedable", "thread_safe"};
    }
};

// Visualization plugin
class VisualizationPlugin : public ComponentPlugin {
public:
    std::string getName() const override { return "visualization"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Brain state visualization and analysis"; }
    PluginType getType() const override { return PluginType::UTILITY; }
    std::string getComponentType() const override { return "visualization";; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"neural_networks", "spike_raster", "population_dynamics", "connectivity_matrices"};
    }
};

// Behavior plugins

// Exploration behavior plugin
class ExplorationBehaviorPlugin : public BehaviorPlugin {
public:
    std::string getName() const override { return "exploration_behavior"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Curiosity-driven exploration behavior"; }
    PluginType getType() const override { return PluginType::BEHAVIOR; }
    
    std::vector<std::string> getDependencies() const override {
        return {"core_brain", "neuromodulation"};
    }
    
    std::vector<std::string> getCapabilities() const override {
        return {"curiosity_driven", "exploration", "novelty_seeking", "random_actions"};
    }
};

// Learning behavior plugin
class LearningBehaviorPlugin : public BehaviorPlugin {
public:
    std::string getName() const override { return "learning_behavior"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Experience-based learning behavior"; }
    PluginType getType() const override { return PluginType::BEHAVIOR; }
    
    std::vector<std::string> getDependencies() const override {
        return {"core_brain", "neuromodulation", "memory_system"};
    }
    
    std::vector<std::string> getCapabilities() const override {
        return {"reinforcement_learning", " Hebbian_learning", "STDP_learning", "continual_learning"};
    }
};

// Memory plugins

// Working memory plugin
class WorkingMemoryPlugin : public MemoryPlugin {
public:
    std::string getName() const override { return "working_memory"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Short-term working memory system"; }
    PluginType getType() const override { return PluginType::MEMORY; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"active_memory", "item_buffer", "temporal_memory", "short_term_storage"};
    }
};

// Episodic memory plugin
class EpisodicMemoryPlugin : public MemoryPlugin {
public:
    std::string getName() const override { return "episodic_memory"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Long-term episodic memory system"; }
    PluginType getType() const override { return PluginType::MEMORY; }
    
    std::vector<std::string> getCapabilities() const override {
        return {"experience_storage", "replay", "consolidation", "associative_recall"};
    }
};

// Plugin factory functions
namespace PluginFactories {
    // Core plugins
    std::unique_ptr<Plugin> createCoreBrainPlugin();
    std::unique_ptr<Plugin> createMemorySystemPlugin();
    std::unique_ptr<Plugin> createNeuralDynamicsPlugin();
    std::unique_ptr<Plugin> createNeuromodulationPlugin();
    
    // Utility plugins
    std::unique_ptr<Plugin> createLoggerPlugin();
    std::unique_ptr<Plugin> createRandomGeneratorPlugin();
    std::unique_ptr<Plugin> createVisualizationPlugin();
    
    // Behavior plugins
    std::unique_ptr<Plugin> createExplorationBehaviorPlugin();
    std::unique_ptr<Plugin> createLearningBehaviorPlugin();
    
    // Memory plugins
    std::unique_ptr<Plugin> createWorkingMemoryPlugin();
    std::unique_ptr<Plugin> createEpisodicMemoryPlugin();
    
    // Custom plugin factory
    using CustomPluginFactory = std::function<std::unique_ptr<Plugin>()>;
}

// Plugin manifest for metadata
struct PluginManifest {
    std::string name;
    std::string version;
    std::string description;
    PluginType type;
    std::string author;
    std::string license;
    std::vector<std::string> dependencies;
    std::vector<std::string> capabilities;
    std::map<std::string, std::string> metadata;
    
    std::string toString() const;
    static PluginManifest fromString(const std::string& data);
};

// Plugin configuration
struct PluginConfig {
    std::string name;
    PluginType type;
    std::map<std::string, std::string> settings;
    bool enabled;
    double loadPriority;
    
    PluginConfig(const std::string& n, PluginType t = PluginType::CUSTOM)
        : name(n), type(t), enabled(true), loadPriority(1.0) {}
};

} // namespace nlm
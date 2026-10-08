// Plugin System for NLM - Extending NLM with Custom Functionality
// Provides a plugin architecture for extending NLM with custom modules

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/Environment.hpp"

namespace nlm {

// Plugin types and capabilities
enum class PluginType {
    Memory,          // Memory system extension
    Plasticity,       // Plasticity rule extension
    Neuromodulation,  // Neuromodulation extension
    Cognition,       // Cognitive extension
    Sensory,         // Sensory processing extension
    Motor,           // Motor control extension
    Environment,     // Environment extension
    Visualization,   // Visualization extension
    Data,            // Data processing extension
    Analysis,        // Analysis extension
    Custom          // Custom plugin type
};

enum class PluginCapability {
    ReadOnly,
    Write,
    Execute,
    Network,
    FileSystem,
    Database,
    GUI,
    Audio,
    Video,
    Hardware,
    All
};

// Plugin metadata
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string website;
    
    PluginType type;
    std::vector<PluginCapability> capabilities;
    std::vector<std::string> dependencies;
    std::vector<std::string> optionalDependencies;
    
    bool enabled;
    bool loaded;
    
    // Plugin statistics
    uint64_t loadCount;
    uint64_t unloadCount;
    double totalExecutionTime;
    uint64_t callCount;
    
    PluginMetadata()
        : type(PluginType::Custom), enabled(false), loaded(false),
          loadCount(0), unloadCount(0), totalExecutionTime(0.0), callCount(0) {}
};

// Plugin interface - base class for all plugins
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    // Get plugin metadata
    virtual const PluginMetadata& getMetadata() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Plugin entry points
    virtual void onBrainCreated(std::shared_ptr<Brain> brain) {}
    virtual void onBrainDestroyed(std::shared_ptr<Brain> brain) {}
    virtual void onAgentCreated(std::shared_ptr<AgentBrain> agent) {}
    virtual void onAgentDestroyed(std::shared_ptr<AgentBrain> agent) {}
    virtual void onWorldCreated(std::shared_ptr<Environment> world) {}
    virtual void onWorldDestroyed(std::shared_ptr<Environment> world) {}
    
    // Plugin-specific functionality
    virtual std::vector<std::string> getAvailableCommands() const { return {}; }
    virtual std::map<std::string, std::string> executeCommand(const std::string& command, 
                                                            const std::map<std::string, std::string>& args) { return {}; }
    
    // Extension points
    virtual void beforeStep(double timestep) {}
    virtual void afterStep(double timestep) {}
    virtual void beforeReset() {}
    virtual void afterReset() {}
    virtual void beforeSave() {}
    virtual void afterSave() {}
    virtual void beforeLoad() {}
    virtual void afterLoad() {}
    
    // Hook for extending core systems
    virtual void extendMemorySystem(std::shared_ptr<Brain> brain) {}
    virtual void extendPlasticitySystem(std::shared_ptr<Brain> brain) {}
    virtual void extendNeuromodulationSystem(std::shared_ptr<Brain> brain) {}
    virtual void extendCognitionSystem(std::shared_ptr<Brain> brain) {}
    virtual void extendSensorySystem(std::shared_ptr<Brain> brain) {}
    virtual void extendMotorSystem(std::shared_ptr<Brain> brain) {}
    virtual void extendEnvironment(std::shared_ptr<Environment> world) {}
    
    // Data processing hooks
    virtual std::vector<float> processSensoryData(const std::vector<float>& input) { return input; }
    virtual std::vector<float> processMotorOutput(const std::vector<float>& input) { return input; }
    virtual float processReward(float reward) { return reward; }
    
    // Analysis hooks
    virtual void analyzeState(const std::map<std::string, double>& metrics) {}
    virtual void logState() const {}
    
    // Plugin-specific data storage
    virtual void setData(const std::string& key, const std::string& value) {}
    virtual std::string getData(const std::string& key) const { return ""; }
    virtual bool hasData(const std::string& key) const { return false; }
    
    // Plugin discovery
    virtual std::vector<std::string> getDiscoveryInfo() const { return {}; }
    
    // Error handling
    virtual void handleError(const std::string& error) {}
    virtual bool isHealthy() const { return isInitialized(); }
};

// Plugin factory interface
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    virtual std::unique_ptr<PluginInterface> createPlugin() = 0;
    virtual bool canCreatePlugin() const = 0;
};

// Plugin registry - manages all plugins
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Plugin loading
    bool loadPlugin(const std::string& pluginPath, const std::string& name = "");
    bool loadPluginFromString(const std::string& pluginCode, const std::string& name = "");
    bool unloadPlugin(const std::string& name);
    bool reloadPlugin(const std::string& name);
    
    // Plugin discovery and management
    std::vector<std::string> getAvailablePlugins() const;
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getEnabledPlugins() const;
    
    // Plugin configuration
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    bool setPluginConfig(const std::string& name, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& name) const;
    
    // Plugin lifecycle management
    bool initializeAllPlugins(const std::map<std::string, std::map<std::string, std::string>>& configs);
    void shutdownAllPlugins();
    
    // Plugin system integration
    void onBrainCreated(std::shared_ptr<Brain> brain);
    void onBrainDestroyed(std::shared_ptr<Brain> brain);
    void onAgentCreated(std::shared_ptr<AgentBrain> agent);
    void onAgentDestroyed(std::shared_ptr<AgentBrain> agent);
    void onWorldCreated(std::shared_ptr<Environment> world);
    void onWorldDestroyed(std::shared_ptr<Environment> world);
    
    // Plugin execution
    void beforeStep(double timestep);
    void afterStep(double timestep);
    void beforeReset();
    void afterReset();
    void beforeSave();
    void afterSave();
    void beforeLoad();
    void afterLoad();
    
    // Plugin commands and discovery
    std::vector<std::string> getAvailableCommands() const;
    std::map<std::string, std::string> executeCommand(const std::string& command,
                                                     const std::map<std::string, std::string>& args);
    
    // Plugin statistics
    std::map<std::string, std::map<std::string, std::string>> getPluginStats() const;
    std::map<std::string, std::string> getSystemStats() const;
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directory = "plugins");
    std::vector<std::string> getPluginInfo(const std::string& name) const;
    
    // Health and monitoring
    bool isHealthy() const;
    std::vector<std::string> getHealthIssues() const;
    
    // Plugin management
    bool installPlugin(const std::string& source, const std::string& target = "");
    bool uninstallPlugin(const std::string& name);
    bool backupPlugin(const std::string& name, const std::string& backupPath = "");
    bool restorePlugin(const std::string& name, const std::string& backupPath = "");
    
    // Plugin marketplace integration
    std::vector<std::string> searchPlugins(const std::string& query) const;
    bool fetchPlugin(const std::string& pluginName, const std::string& version = "");
    bool updatePlugin(const std::string& name, const std::string& version = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin loader interface
class PluginLoader {
public:
    virtual ~PluginLoader() = default;
    virtual bool canLoad(const std::string& path) const = 0;
    virtual std::unique_ptr<PluginInterface> loadPlugin(const std::string& path) = 0;
    virtual std::string getName() const = 0;
};

// Dynamic library plugin loader
class DynamicLibraryPluginLoader : public PluginLoader {
public:
    DynamicLibraryPluginLoader();
    ~DynamicLibraryPluginLoader() override;
    
    bool canLoad(const std::string& path) const override;
    std::unique_ptr<PluginInterface> loadPlugin(const std::string& path) override;
    std::string getName() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Script plugin loader (for plugins written in Python, Lua, etc.)
class ScriptPluginLoader : public PluginLoader {
public:
    ScriptPluginLoader();
    ~ScriptPluginLoader() override;
    
    bool canLoad(const std::string& path) const override;
    std::unique_ptr<PluginInterface> loadPlugin(const std::string& path) override;
    std::string getName() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Built-in plugin base class
class BuiltInPlugin : public PluginInterface {
public:
    BuiltInPlugin(const std::string& name);
    ~BuiltInPlugin() override;
    
    const PluginMetadata& getMetadata() const override;
    bool initialize(const std::map<std::string, std::string>& config) override;
    void shutdown() override;
    bool isInitialized() const override;
    
protected:
    PluginMetadata metadata;
    bool initialized;
    
    // Helper methods
    bool setString(std::map<std::string, std::string>& config, const std::string& key, 
                   const std::string& defaultValue, std::string& result);
    bool setInt(std::map<std::string, std::string>& config, const std::string& key, 
                int defaultValue, int& result);
    bool setDouble(std::map<std::string, std::string>& config, const std::string& key, 
                   double defaultValue, double& result);
    bool setBool(std::map<std::string, std::string>& config, const std::string& key, 
                 bool defaultValue, bool& result);
};

// Plugin for extending memory systems
class MemoryPlugin : public BuiltInPlugin {
public:
    MemoryPlugin();
    ~MemoryPlugin() override;
    
    void extendMemorySystem(std::shared_ptr<Brain> brain) override;
    
    // Custom memory operations
    virtual std::vector<float> getMemoryState(std::shared_ptr<Brain> brain) const;
    virtual void setMemoryState(std::shared_ptr<Brain> brain, const std::vector<float>& state);
    virtual float calculateMemoryUsage(std::shared_ptr<Brain> brain) const;
    virtual void optimizeMemoryUsage(std::shared_ptr<Brain> brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin for extending plasticity systems
class PlasticityPlugin : public BuiltInPlugin {
public:
    PlasticityPlugin();
    ~PlasticityPlugin() override;
    
    void extendPlasticitySystem(std::shared_ptr<Brain> brain) override;
    
    // Custom plasticity rules
    virtual float applyCustomPlasticity(float weight, float preSpike, float postSpike, 
                                       float dopamine, float timeSincePre, float timeSincePost);
    virtual void setPlasticityParameters(std::shared_ptr<Brain> brain, 
                                         const std::map<std::string, float>& params);
    virtual std::map<std::string, float> getPlasticityParameters(std::shared_ptr<Brain> brain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin for extending neuromodulation systems
class NeuromodulationPlugin : public BuiltInPlugin {
public:
    NeuromodulationPlugin();
    ~NeuromodulationPlugin() override;
    
    void extendNeuromodulationSystem(std::shared_ptr<Brain> brain) override;
    
    // Custom neuromodulation
    virtual float calculateNeuromodulationLevel(const std::map<std::string, float>& factors);
    virtual void setNeuromodulationFactors(std::shared_ptr<Brain> brain,
                                           const std::map<std::string, float>& factors);
    virtual std::map<std::string, float> getNeuromodulationFactors(std::shared_ptr<Brain> brain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin for extending cognition systems
class CognitionPlugin : public BuiltInPlugin {
public:
    CognitionPlugin();
    ~CognitionPlugin() override;
    
    void extendCognitionSystem(std::shared_ptr<Brain> brain) override;
    
    // Custom cognition
    virtual std::vector<float> processCognition(const std::vector<float>& input,
                                                std::shared_ptr<Brain> brain);
    virtual void setCognitionParameters(std::shared_ptr<Brain> brain,
                                         const std::map<std::string, float>& params);
    virtual std::map<std::string, float> getCognitionParameters(std::shared_ptr<Brain> brain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin for extending sensory systems
class SensoryPlugin : public BuiltInPlugin {
public:
    SensoryPlugin();
    ~SensoryPlugin() override;
    
    void extendSensorySystem(std::shared_ptr<Brain> brain) override;
    
    // Custom sensory processing
    virtual std::vector<float> processSensoryInput(const std::vector<float>& input,
                                                  std::shared_ptr<Brain> brain);
    virtual void setSensoryParameters(std::shared_ptr<Brain> brain,
                                     const std::map<std::string, float>& params);
    virtual std::map<std::string, float> getSensoryParameters(std::shared_ptr<Brain> brain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin for extending motor systems
class MotorPlugin : public BuiltInPlugin {
public:
    MotorPlugin();
    ~MotorPlugin() override;
    
    void extendMotorSystem(std::shared_ptr<Brain> brain) override;
    
    // Custom motor control
    virtual std::vector<float> processMotorOutput(const std::vector<float>& input,
                                                 std::shared_ptr<Brain> brain);
    virtual void setMotorParameters(std::shared_ptr<Brain> brain,
                                    const std::map<std::string, float>& params);
    virtual std::map<std::string, float> getMotorParameters(std::shared_ptr<Brain> brain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin for extending environment systems
class EnvironmentPlugin : public BuiltInPlugin {
public:
    EnvironmentPlugin();
    ~EnvironmentPlugin() override;
    
    void extendEnvironment(std::shared_ptr<Environment> world) override;
    
    // Custom environment
    virtual std::vector<float> processEnvironmentalInput(const std::vector<float>& input,
                                                        std::shared_ptr<Environment> world);
    virtual void setEnvironmentParameters(std::shared_ptr<Environment> world,
                                          const std::map<std::string, float>& params);
    virtual std::map<std::string, float> getEnvironmentParameters(std::shared_ptr<Environment> world) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin for extending visualization systems
class VisualizationPlugin : public BuiltInPlugin {
public:
    VisualizationPlugin();
    ~VisualizationPlugin() override;
    
    void beforeStep(double timestep) override;
    void afterStep(double timestep) override;
    
    // Custom visualization
    virtual void addVisualizationData(const std::string& name, const std::vector<float>& data);
    virtual std::vector<float> getVisualizationData(const std::string& name) const;
    virtual void setVisualizationParameters(const std::map<std::string, float>& params);
    virtual std::map<std::string, float> getVisualizationParameters() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin for data processing
class DataPlugin : public BuiltInPlugin {
public:
    DataPlugin();
    ~DataPlugin() override;
    
    // Data processing hooks
    std::vector<float> processSensoryData(const std::vector<float>& input) override;
    std::vector<float> processMotorOutput(const std::vector<float>& input) override;
    float processReward(float reward) override;
    
    // Data analysis
    void analyzeState(const std::map<std::string, double>& metrics) override;
    void logState() const override;
    
    // Data storage
    void setData(const std::string& key, const std::string& value) override;
    std::string getData(const std::string& key) const override;
    bool hasData(const std::string& key) const override;
    
    // Data export
    virtual bool exportData(const std::string& filepath, const std::string& format = "auto");
    virtual bool importData(const std::string& filepath, const std::string& format = "auto");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin for analysis
class AnalysisPlugin : public BuiltInPlugin {
public:
    AnalysisPlugin();
    ~AnalysisPlugin() override;
    
    // Analysis hooks
    void analyzeState(const std::map<std::string, double>& metrics) override;
    void logState() const override;
    
    // Statistical analysis
    virtual double calculateMetric(const std::string& metric, const std::vector<float>& data) const;
    virtual std::vector<double> calculateStatistics(const std::vector<float>& data) const;
    virtual double detectAnomaly(const std::vector<float>& data, double threshold) const;
    
    // Machine learning integration
    virtual void trainModel(const std::vector<std::vector<float>>& trainingData,
                          const std::vector<float>& targets);
    virtual std::vector<float> predict(const std::vector<float>& input) const;
    virtual void updateModel(const std::vector<std::vector<float>>& newData,
                           const std::vector<float>& newTargets);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_PLUGIN_SYSTEM_HPP

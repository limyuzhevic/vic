#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

class PluginBase {
public:
    virtual ~PluginBase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    virtual void onBrainInitialize(std::shared_ptr<Brain> brain) {}
    virtual void onBrainReset(std::shared_ptr<Brain> brain) {}
    virtual void onBrainStep(std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time) {}
    virtual void onNeuronSpike(std::shared_ptr<Brain> brain, std::shared_ptr<Neuron> neuron) {}
    virtual void onSynapseChange(std::shared_ptr<Brain> brain, std::shared_ptr<Synapse> synapse) {}
    virtual void onExperimentStart(std::shared_ptr<Brain> brain) {}
    virtual void onExperimentEnd(std::shared_ptr<Brain> brain) {}
    
    // Extension hooks
    virtual std::vector<std::string> getCommandCompletions(const std::string& partialCommand) const { return {}; }
    virtual std::function<bool(const std::vector<std::string>&)> getCommandHandler(const std::string& command) const { return nullptr; }
    virtual std::vector<std::string> getConfigKeys() const { return {}; }
    virtual std::string getDefaultConfig(const std::string& key) const { return ""; }
    
    virtual std::string getPluginInfo() const {
        std::ostringstream oss;
        oss << "Plugin: " << getName() << "\n";
        oss << "Version: " << getVersion() << "\n";
        oss << "Description: " << getDescription() << "\n";
        oss << "Author: " << getAuthor() << "\n";
        return oss.str();
    }
};

using PluginPtr = std::shared_ptr<PluginBase>;

class PluginSystem {
public:
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string filePath;
        bool enabled;
        std::map<std::string, std::string> config;
        std::string error;
    };
    
    PluginSystem();
    ~PluginSystem();
    
    bool loadPlugin(const std::string& filePath);
    bool loadPlugin(const PluginPtr& plugin);
    bool unloadPlugin(const std::string& name);
    bool unloadPlugin(const PluginPtr& plugin);
    
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    bool isPluginEnabled(const std::string& name) const;
    
    PluginPtr getPlugin(const std::string& name) const;
    std::vector<PluginPtr> getPlugins() const;
    std::vector<std::string> getPluginNames() const;
    
    std::vector<PluginInfo> getPluginInfo() const;
    
    PluginPtr findPluginByName(const std::string& name) const;
    
    bool initializeAllPlugins(const std::map<std::string, std::string>& globalConfig = {});
    void shutdownAllPlugins();
    
    // Plugin events
    void notifyBrainInitialize(std::shared_ptr<Brain> brain);
    void notifyBrainReset(std::shared_ptr<Brain> brain);
    void notifyBrainStep(std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time);
    void notifyNeuronSpike(std::shared_ptr<Brain> brain, std::shared_ptr<Neuron> neuron);
    void notifySynapseChange(std::shared_ptr<Brain> brain, std::shared_ptr<Synapse> synapse);
    void notifyExperimentStart(std::shared_ptr<Brain> brain);
    void notifyExperimentEnd(std::shared_ptr<Brain> brain);
    
    // Extension hooks
    std::vector<std::string> getAllCommandCompletions(const std::string& partialCommand) const;
    std::function<bool(const std::vector<std::string>&)> getCommandHandler(const std::string& command) const;
    std::vector<std::string> getAllConfigKeys() const;
    std::string getDefaultConfigValue(const std::string& key) const;
    
    bool savePluginConfig(const std::string& filename) const;
    bool loadPluginConfig(const std::string& filename);
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directory = "plugins") const;
    
    // Plugin metadata
    std::string getPluginManifest(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
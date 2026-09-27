#ifndef NLM_PLUGIN_EXAMPLES_MEMORYPLUGIN_HPP
#define NLM_PLUGIN_EXAMPLES_MEMORYPLUGIN_HPP

#include "AbstractComponentPlugin.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

namespace nlm {
namespace plugin {

class MemoryPlugin : public AbstractComponentPlugin {
public:
    MemoryPlugin();
    ~MemoryPlugin() override;
    
    // IPlugin interface implementation
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    PluginType getType() const override;
    PluginMetadata getMetadata() const override;
    
    bool initialize(const PluginMetadata& metadata) override;
    void shutdown() override;
    bool activate() override;
    bool deactivate() override;
    bool loadDependencies() override;
    bool checkConflicts() override;
    
    // AbstractComponentPlugin interface implementation
    std::string getComponentType() const override;
    std::string getComponentName() const override;
    
    bool configureComponent(const std::unordered_map<std::string, std::string>& config) override;
    std::unordered_map<std::string, std::string> getComponentConfig() const override;
    
    bool preInitialize() override;
    bool postInitialize() override;
    bool preShutdown() override;
    bool postShutdown() override;
    
    ComponentState getComponentState() const override;
    std::string getComponentStatus() const override;
    
    void* getComponentExtension(const std::string& extensionName) override;
    bool registerComponentExtension(const std::string& extensionName, void* extension) override;
    
    bool connectToComponent(const std::string& componentName) override;
    bool disconnectFromComponent(const std::string& componentName) override;
    bool isConnectedToComponent(const std::string& componentName) const override;
    
    bool validateComponentConfiguration() const override;
    bool validateComponentDependencies() const override;
    
    // Memory system functionality
    class MemorySystem {
    public:
        MemorySystem(const std::string& name, MemoryPlugin* plugin);
        ~MemorySystem();
        
        std::string getName() const { return name_; }
        std::string getType() const { return type_; }
        double getCapacity() const { return capacity_; }
        double getCurrentLoad() const { return currentLoad_; }
        
        void setType(const std::string& type) { type_ = type; }
        void setCapacity(double capacity) { capacity_ = capacity; }
        void setCurrentLoad(double load) { currentLoad_ = load; }
        
        bool store(const std::string& key, const std::string& value);
        bool retrieve(const std::string& key, std::string& value);
        bool remove(const std::string& key);
        bool clear();
        
        std::vector<std::string> getKeys() const;
        size_t getSize() const { return data_.size(); }
        
    private:
        std::string name_;
        std::string type_;
        double capacity_;
        double currentLoad_;
        std::unordered_map<std::string, std::string> data_;
        MemoryPlugin* plugin_;
    };
    
    MemorySystem* createMemorySystem(const std::string& name);
    bool destroyMemorySystem(MemorySystem* memory);
    std::vector<MemorySystem*> getMemorySystems() const;
    MemorySystem* getMemorySystem(const std::string& name) const;
    
    // Memory management
    bool consolidateMemories();
    bool replayMemories();
    bool compressMemories();
    
    // Memory plugin configuration
    struct MemoryConfig {
        double defaultCapacity = 1000.0;
        std::string defaultType = "working_memory";
        bool enableConsolidation = true;
        bool enableReplay = true;
        double consolidationThreshold = 0.8;
        double replayFrequency = 0.1;
        bool enableCompression = false;
        double compressionRatio = 0.5;
    };
    
    MemoryConfig getMemoryConfig() const { return config_; }
    void setMemoryConfig(const MemoryConfig& config) { config_ = config; }
    
private:
    PluginMetadata metadata_;
    PluginState state_;
    std::string error_;
    MemoryConfig config_;
    std::vector<MemorySystem*> memorySystems_;
    std::unordered_map<std::string, void*> extensions_;
    std::unordered_map<std::string, std::string> componentConnections_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_EXAMPLES_MEMORYPLUGIN_HPP
#pragma once

#include "MemorySystemPlugin.hpp"
#include <unordered_map>
#include <vector>

namespace nlm {

class ExampleMemoryPlugin : public MemorySystemPlugin {
public:
    ExampleMemoryPlugin();
    ~ExampleMemoryPlugin() override;
    
    // Plugin identification
    std::string getName() const override { return "ExampleMemory"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { 
        return "Example memory plugin with custom storage mechanisms"; 
    }
    std::string getAuthor() const override { return "NLM Team"; }
    PluginType getType() const override { return PluginType::MEMORY_SYSTEM; }
    
    // Plugin lifecycle
    bool initialize(const PluginConfig& config, void* manager) override;
    void update(double deltaTime) override;
    void shutdown() override;
    
    // Plugin state management
    void pause() override;
    void resume() override;
    PluginState getState() const override;
    
    // Plugin dependencies
    std::vector<std::string> getDependencies() const override;
    bool checkDependencies(const std::unordered_map<std::string, PluginInfo>& plugins) const override;
    
    // Plugin communication
    void receiveMessage(const std::string& sender, const PluginMessage& message) override;
    void sendMessage(const std::string& receiver, const PluginMessage& message) override;
    
    // Plugin configuration
    void configure(const PluginConfig& config) override;
    PluginConfig getConfig() const override;
    
    // Memory extension
    void storeMemory(const MemoryPattern& pattern, float strength) override;
    MemoryPattern retrieveMemory(const MemoryQuery& query) override;
    void updateMemory(double deltaTime) override;
    void consolidateMemory() override;
    
    // Memory mechanisms
    MemoryStorageType getStorageType() const override { return MemoryStorageType::Custom; }
    MemoryAccessPattern getAccessPattern() const override { return MemoryAccessPattern::Sequential; }
    double getMemoryCapacity() const override { return 1000.0; }
    double getMemoryDecayRate() const override { return 0.01; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

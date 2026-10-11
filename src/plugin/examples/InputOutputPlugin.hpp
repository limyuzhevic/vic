#pragma once

#include "InputOutputPlugin.hpp"
#include <memory>

namespace nlm {

class ExampleInputOutputPlugin : public InputOutputPlugin {
public:
    ExampleInputOutputPlugin();
    ~ExampleInputOutputPlugin() override;
    
    // Plugin identification
    std::string getName() const override { return "ExampleInputOutput"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { 
        return "Example I/O plugin with custom sensory/motor interfaces"; 
    }
    std::string getAuthor() const override { return "NLM Team"; }
    PluginType getType() const override { return PluginType::INPUT_OUTPUT; }
    
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
    
    // I/O extension
    std::unique_ptr<Observation> getSensoryInput() override;
    void sendMotorOutput(const Action& action) override;
    void updateIO(double deltaTime) override;
    
    // I/O parameters
    std::string getInputType() const override;
    std::string getOutputType() const override;
    double getSamplingRate() const override;
    double getLatency() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

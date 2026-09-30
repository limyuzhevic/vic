#include "include/nlm/plugin/ISensoryPlugin.h"
#include "src/sensory/SensoryInput.hpp"
#include <iostream>
#include <memory>
#include <algorithm>

namespace nlm {
namespace plugin {

class ExampleSensoryPlugin : public ISensoryPlugin {
public:
    ExampleSensoryPlugin() : state_(PluginState::UNLOADED) {
    }

    ~ExampleSensoryPlugin() {
        shutdown();
    }

    std::string getName() const override {
        return "ExampleSensoryPlugin";
    }

    std::string getVersion() const override {
        return "1.0.0";
    }

    std::string getAuthor() const override {
        return "NLM Plugin Developer";
    }

    std::string getDescription() const override {
        return "Example sensory plugin implementing multi-modal sensory integration";
    }

    std::string getLicense() const override {
        return "MIT License";
    }

    std::vector<PluginCapability> getCapabilities() const override {
        return {PluginCapability::SENSORY};
    }

    std::vector<std::string> getDependencies() const override {
        return {"VisionPlugin"};
    }

    bool initialize(const std::map<std::string, std::string>& config) override {
        std::cout << "Initializing ExampleSensoryPlugin with " << config.size() << " config entries" << std::endl;

        state_ = PluginState::LOADED;
        return true;
    }

    bool shutdown() override {
        std::cout << "Shutting down ExampleSensoryPlugin" << std::endl;

        state_ = PluginState::UNLOADED;
        return true;
    }

    bool update(double deltaTime) override {
        // Process sensory inputs
        for (auto& input : processedInputs_) {
            input.update(deltaTime);
        }
        return true;
    }

    PluginState getState() const override {
        return state_;
    }

    bool isEnabled() const override {
        return enabled_;
    }

    bool enable() override {
        std::cout << "Enabling ExampleSensoryPlugin" << std::endl;
        enabled_ = true;
        state_ = PluginState::INITIALIZED;
        return true;
    }

    bool disable() override {
        std::cout << "Disabling ExampleSensoryPlugin" << std::endl;
        enabled_ = false;
        state_ = PluginState::LOADED;
        return true;
    }

    std::string getType() const override {
        return "SensoryPlugin";
    }

    std::string getID() const override {
        return "example_sensory_plugin_001";
    }

    void onEvent(const std::string& eventName, const std::map<std::string, std::string>& data) override {
        std::cout << "ExampleSensoryPlugin received event: " << eventName << std::endl;
    }

    std::string getSensoryModality() const override {
        return "MultiModal";
    }

    std::shared_ptr<SensoryInput> createSensoryInput() override {
        return std::make_shared<SensoryInput>();
    }

    std::vector<SensoryInput> processInput(const std::vector<SensoryInput>& inputs) override {
        std::vector<SensoryInput> processedInputs = inputs;

        for (auto& input : processedInputs) {
            input.setGain(input.getGain() * getInputGain());
            input.setNoise(input.getNoiseLevel() * getNoiseLevel());
        }

        processedInputs_.assign(processedInputs.begin(), processedInputs.end());
        return processedInputs;
    }

    void configureSensorySystem() override {
        std::cout << "Configuring multi-modal sensory system" << std::endl;
    }

    double getInputGain() const override {
        return 1.5;
    }

    double getNoiseLevel() const override {
        return 0.1;
    }

    std::map<std::string, double> getProcessingParameters() const override {
        return {
            {"threshold", 0.5},
            {"sensitivity", 2.0},
            {"integration_window", 0.1}
        };
    }

    void setProcessingParameters(const std::map<std::string, double>& params) override {
        processingParameters_ = params;
    }

private:
    PluginState state_;
    bool enabled_;
    std::vector<SensoryInput> processedInputs_;
    std::map<std::string, double> processingParameters_;
};

} // namespace plugin
} // namespace nlm

extern "C" nlm::plugin::IPlugin* createPlugin() {
    return new nlm::plugin::ExampleSensoryPlugin();
}

extern "C" void destroyPlugin(nlm::plugin::IPlugin* plugin) {
    delete plugin;
}

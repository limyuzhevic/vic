#include "include/nlm/plugin/IMotorPlugin.h"
#include "src/motor/MotorSystem.hpp"
#include "src/motor/Action.hpp"
#include <iostream>
#include <memory>
#include <algorithm>

namespace nlm {
namespace plugin {

class ExampleMotorPlugin : public IMotorPlugin {
public:
    ExampleMotorPlugin() : state_(PluginState::UNLOADED) {
    }

    ~ExampleMotorPlugin() {
        shutdown();
    }

    std::string getName() const override {
        return "ExampleMotorPlugin";
    }

    std::string getVersion() const override {
        return "1.0.0";
    }

    std::string getAuthor() const override {
        return "NLM Plugin Developer";
    }

    std::string getDescription() const override {
        return "Example motor plugin implementing complex motor control";
    }

    std::string getLicense() const override {
        return "MIT License";
    }

    std::vector<PluginCapability> getCapabilities() const override {
        return {PluginCapability::MOTOR};
    }

    std::vector<std::string> getDependencies() const override {
        return {"BrainPlugin"};
    }

    bool initialize(const std::map<std::string, std::string>& config) override {
        std::cout << "Initializing ExampleMotorPlugin with " << config.size() << " config entries" << std::endl;

        state_ = PluginState::LOADED;
        return true;
    }

    bool shutdown() override {
        std::cout << "Shutting down ExampleMotorPlugin" << std::endl;

        state_ = PluginState::UNLOADED;
        return true;
    }

    bool update(double deltaTime) override {
        // Generate and process motor commands
        for (auto& action : processedActions_) {
            action.update(deltaTime);
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
        std::cout << "Enabling ExampleMotorPlugin" << std::endl;
        enabled_ = true;
        state_ = PluginState::INITIALIZED;
        return true;
    }

    bool disable() override {
        std::cout << "Disabling ExampleMotorPlugin" << std::endl;
        enabled_ = false;
        state_ = PluginState::LOADED;
        return true;
    }

    std::string getType() const override {
        return "MotorPlugin";
    }

    std::string getID() const override {
        return "example_motor_plugin_001";
    }

    void onEvent(const std::string& eventName, const std::map<std::string, std::string>& data) override {
        std::cout << "ExampleMotorPlugin received event: " << eventName << std::endl;
    }

    std::string getMotorCommandType() const override {
        return "ComplexMotorControl";
    }

    std::vector<Action> generateMotorCommands(const std::vector<Action>& previousActions) override {
        std::vector<Action> motorCommands;

        for (const auto& action : previousActions) {
            Action newAction;
            newAction.setCommand(action.getCommand());
            newAction.setStrength(action.getStrength() * getOutputGain());
            newAction.setDuration(action.getDuration() * getExplorationRate());
            motorCommands.push_back(newAction);
        }

        generatedCommands_ = motorCommands;
        return motorCommands;
    }

    std::vector<Action> processMotorOutput(const std::vector<Action>& motorOutput) override {
        std::vector<Action> processedActions = motorOutput;

        for (auto& action : processedActions) {
            action.setStrength(action.getStrength() * 0.8); // Apply damping
            action.setDuration(action.getDuration() * 1.2);
        }

        processedActions_ = processedActions;
        return processedActions;
    }

    void configureMotorControl() override {
        std::cout << "Configuring complex motor control system" << std::endl;
    }

    double getOutputGain() const override {
        return 2.0;
    }

    double getExplorationRate() const override {
        return 0.3;
    }

    std::map<std::string, double> getMotorParameters() const override {
        return {
            {"max_strength", 100.0},
            {"min_strength", 0.1},
            {"acceleration", 2.5},
            {"deceleration", 1.5}
        };
    }

    void setMotorParameters(const std::map<std::string, double>& params) override {
        motorParameters_ = params;
    }

private:
    PluginState state_;
    bool enabled_;
    std::vector<Action> generatedCommands_;
    std::vector<Action> processedActions_;
    std::map<std::string, double> motorParameters_;
};

} // namespace plugin
} // namespace nlm

extern "C" nlm::plugin::IPlugin* createPlugin() {
    return new nlm::plugin::ExampleMotorPlugin();
}

extern "C" void destroyPlugin(nlm::plugin::IPlugin* plugin) {
    delete plugin;
}

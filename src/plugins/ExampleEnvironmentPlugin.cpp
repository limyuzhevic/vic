#include "include/nlm/plugin/IEnvironmentPlugin.h"
#include "src/environment/Environment.hpp"
#include "src/environment/Observation.hpp"
#include "src/environment/Action.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace nlm {
namespace plugin {

class ExampleEnvironmentPlugin : public IEnvironmentPlugin {
public:
    ExampleEnvironmentPlugin() : state_(PluginState::UNLOADED) {
    }

    ~ExampleEnvironmentPlugin() {
        shutdown();
    }

    std::string getName() const override {
        return "ExampleEnvironmentPlugin";
    }

    std::string getVersion() const override {
        return "1.0.0";
    }

    std::string getAuthor() const override {
        return "NLM Plugin Developer";
    }

    std::string getDescription() const override {
        return "Example environment plugin implementing dynamic world simulations with objects";
    }

    std::string getLicense() const override {
        return "MIT License";
    }

    std::vector<PluginCapability> getCapabilities() const override {
        return {PluginCapability::ENVIRONMENT};
    }

    std::vector<std::string> getDependencies() const override {
        return {"WorldPlugin"};
    }

    bool initialize(const std::map<std::string, std::string>& config) override {
        std::cout << "Initializing ExampleEnvironmentPlugin with " << config.size() << " config entries" << std::endl;

        state_ = PluginState::LOADED;
        return true;
    }

    bool shutdown() override {
        std::cout << "Shutting down ExampleEnvironmentPlugin" << std::endl;

        state_ = PluginState::UNLOADED;
        return true;
    }

    bool update(double deltaTime) override {
        // Update physics simulation
        return true;
    }

    PluginState getState() const override {
        return state_;
    }

    bool isEnabled() const override {
        return enabled_;
    }

    bool enable() override {
        std::cout << "Enabling ExampleEnvironmentPlugin" << std::endl;
        enabled_ = true;
        state_ = PluginState::INITIALIZED;
        return true;
    }

    bool disable() override {
        std::cout << "Disabling ExampleEnvironmentPlugin" << std::endl;
        enabled_ = false;
        state_ = PluginState::LOADED;
        return true;
    }

    std::string getType() const override {
        return "EnvironmentPlugin";
    }

    std::string getID() const override {
        return "example_environment_plugin_001";
    }

    void onEvent(const std::string& eventName, const std::map<std::string, std::string>& data) override {
        std::cout << "ExampleEnvironmentPlugin received event: " << eventName << std::endl;
    }

    std::string getEnvironmentType() const override {
        return "DynamicEnvironment";
    }

    std::shared_ptr<Environment> createEnvironment() override {
        return std::make_shared<Environment>();
    }

    std::vector<Observation> simulatePhysics(const std::vector<Observation>& inputs) override {
        std::vector<Observation> outputs = inputs;

        for (auto& observation : outputs) {
            // Add some physics simulation logic
            observation.setPosition(observation.getPosition() + observation.getVelocity() * getTimeStep());
            observation.setVelocity(observation.getVelocity() * 0.99); // Add some damping
        }

        return outputs;
    }

    std::vector<Action> applyForces(const std::vector<Action>& actions) override {
        std::vector<Action> processedActions = actions;

        for (auto& action : processedActions) {
            // Apply forces based on physics parameters
            action.setForce(action.getForce() * getGravity());
        }

        return processedActions;
    }

    void configureEnvironment() override {
        std::cout << "Configuring dynamic environment with physics" << std::endl;
    }

    double getTimeStep() const override {
        return 0.01;
    }

    double getGravity() const override {
        return 9.81;
    }

    bool isDynamic() const override {
        return true;
    }

    std::map<std::string, double> getPhysicsParameters() const override {
        return {
            {"gravity", 9.81},
            {"time_step", 0.01},
            {"friction", 0.5},
            {"elasticity", 0.7},
            {"damping", 0.99}
        };
    }

    void setPhysicsParameters(const std::map<std::string, double>& params) override {
        physicsParameters_ = params;
    }

private:
    PluginState state_;
    bool enabled_;
    std::map<std::string, double> physicsParameters_;
};

} // namespace plugin
} // namespace nlm

extern "C" nlm::plugin::IPlugin* createPlugin() {
    return new nlm::plugin::ExampleEnvironmentPlugin();
}

extern "C" void destroyPlugin(nlm::plugin::IPlugin* plugin) {
    delete plugin;
}

#include "include/nlm/plugin/IExperimentPlugin.h"
#include "src/experiments/Experiment.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace nlm {
namespace plugin {

class ExampleExperimentPlugin : public IExperimentPlugin {
public:
    ExampleExperimentPlugin() : state_(PluginState::UNLOADED) {
    }

    ~ExampleExperimentPlugin() {
        shutdown();
    }

    std::string getName() const override {
        return "ExampleExperimentPlugin";
    }

    std::string getVersion() const override {
        return "1.0.0";
    }

    std::string getAuthor() const override {
        return "NLM Plugin Developer";
    }

    std::string getDescription() const override {
        return "Example experiment plugin implementing custom experiment types and measurement tools";
    }

    std::string getLicense() const override {
        return "MIT License";
    }

    std::vector<PluginCapability> getCapabilities() const override {
        return {PluginCapability::EXPERIMENT};
    }

    std::vector<std::string> getDependencies() const override {
        return {"MemoryPlugin"};
    }

    bool initialize(const std::map<std::string, std::string>& config) override {
        std::cout << "Initializing ExampleExperimentPlugin with " << config.size() << " config entries" << std::endl;

        state_ = PluginState::LOADED;
        return true;
    }

    bool shutdown() override {
        std::cout << "Shutting down ExampleExperimentPlugin" << std::endl;

        state_ = PluginState::UNLOADED;
        return true;
    }

    bool update(double deltaTime) override {
        // Update experiment state
        return true;
    }

    PluginState getState() const override {
        return state_;
    }

    bool isEnabled() const override {
        return enabled_;
    }

    bool enable() override {
        std::cout << "Enabling ExampleExperimentPlugin" << std::endl;
        enabled_ = true;
        state_ = PluginState::INITIALIZED;
        return true;
    }

    bool disable() override {
        std::cout << "Disabling ExampleExperimentPlugin" << std::endl;
        enabled_ = false;
        state_ = PluginState::LOADED;
        return true;
    }

    std::string getType() const override {
        return "ExperimentPlugin";
    }

    std::string getID() const override {
        return "example_experiment_plugin_001";
    }

    void onEvent(const std::string& eventName, const std::map<std::string, std::string>& data) override {
        std::cout << "ExampleExperimentPlugin received event: " << eventName << std::endl;
    }

    std::string getExperimentType() const override {
        return "AdaptiveLearningExperiment";
    }

    std::shared_ptr<Experiment> createExperiment() override {
        return std::make_shared<Experiment>();
    }

    std::vector<double> runExperiment(const std::map<std::string, double>& parameters) override {
        std::cout << "Running adaptive learning experiment with " << parameters.size() << " parameters" << std::endl;

        std::vector<double> results;
        double learningRate = parameters.at("learning_rate");
        double trials = parameters.at("trials");

        for (size_t i = 0; i < trials; ++i) {
            double performance = learningRate * (i + 1) * 0.1;
            results.push_back(performance);
        }

        storeExperimentData("results", results);
        return results;
    }

    bool isExperimentComplete() const override {
        return experimentComplete_;
    }

    void setExperimentComplete(bool complete) {
        experimentComplete_ = complete;
    }

    void configureExperiment() override {
        std::cout << "Configuring adaptive learning experiment" << std::endl;
    }

    size_t getMaxTrials() const override {
        return 100;
    }

    double getTimeLimit() const override {
        return 300.0;
    }

    std::string getMeasurementType() const override {
        return "PerformanceMetrics";
    }

    std::vector<std::vector<double>> getCollectedData() const override {
        return collectedData_;
    }

    std::vector<std::string> getMetricNames() const override {
        return {"performance", "accuracy", "response_time"};
    }

    void storeExperimentData(const std::string& key, const std::vector<double>& data) {
        collectedData_[key] = data;
    }

private:
    PluginState state_;
    bool enabled_;
    bool experimentComplete_;
    std::map<std::string, std::vector<double>> collectedData_;
};

} // namespace plugin
} // namespace nlm

extern "C" nlm::plugin::IPlugin* createPlugin() {
    return new nlm::plugin::ExampleExperimentPlugin();
}

extern "C" void destroyPlugin(nlm::plugin::IPlugin* plugin) {
    delete plugin;
}

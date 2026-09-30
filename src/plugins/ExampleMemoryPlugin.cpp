#include "include/nlm/plugin/IMemoryPlugin.h"
#include "src/memory/Memory.hpp"
#include <iostream>
#include <memory>

namespace nlm {
namespace plugin {

class ExampleMemoryPlugin : public IMemoryPlugin {
public:
    ExampleMemoryPlugin() : state_(PluginState::UNLOADED) {
    }

    ~ExampleMemoryPlugin() {
        shutdown();
    }

    std::string getName() const override {
        return "ExampleMemoryPlugin";
    }

    std::string getVersion() const override {
        return "1.0.0";
    }

    std::string getAuthor() const override {
        return "NLM Plugin Developer";
    }

    std::string getDescription() const override {
        return "Example memory plugin implementing custom memory systems and learning algorithms";
    }

    std::string getLicense() const override {
        return "MIT License";
    }

    std::vector<PluginCapability> getCapabilities() const override {
        return {PluginCapability::MEMORY};
    }

    std::vector<std::string> getDependencies() const override {
        return {"NeuralPlugin"};
    }

    bool initialize(const std::map<std::string, std::string>& config) override {
        std::cout << "Initializing ExampleMemoryPlugin with " << config.size() << " config entries" << std::endl;

        state_ = PluginState::LOADED;
        return true;
    }

    bool shutdown() override {
        std::cout << "Shutting down ExampleMemoryPlugin" << std::endl;

        state_ = PluginState::UNLOADED;
        return true;
    }

    bool update(double deltaTime) override {
        // Update memory systems
        return true;
    }

    PluginState getState() const override {
        return state_;
    }

    bool isEnabled() const override {
        return enabled_;
    }

    bool enable() override {
        std::cout << "Enabling ExampleMemoryPlugin" << std::endl;
        enabled_ = true;
        state_ = PluginState::INITIALIZED;
        return true;
    }

    bool disable() override {
        std::cout << "Disabling ExampleMemoryPlugin" << std::endl;
        enabled_ = false;
        state_ = PluginState::LOADED;
        return true;
    }

    std::string getType() const override {
        return "MemoryPlugin";
    }

    std::string getID() const override {
        return "example_memory_plugin_001";
    }

    void onEvent(const std::string& eventName, const std::map<std::string, std::string>& data) override {
        std::cout << "ExampleMemoryPlugin received event: " << eventName << std::endl;
    }

    std::string getMemorySystemName() const override {
        return "AssociativeMemory";
    }

    std::shared_ptr<Memory> createMemorySystem() override {
        return std::make_shared<Memory>();
    }

    bool storeMemory(const std::string& key, const std::vector<double>& data) override {
        std::cout << "Storing memory with key: " << key << " (" << data.size() << " values)" << std::endl;
        memoryStore_[key] = data;
        return true;
    }

    std::vector<double> retrieveMemory(const std::string& key) override {
        auto it = memoryStore_.find(key);
        if (it != memoryStore_.end()) {
            return it->second;
        }

        return std::vector<double>();
    }

    void configureMemorySystem() override {
        std::cout << "Configuring associative memory system" << std::endl;
    }

    size_t getMemoryCapacity() const override {
        return 10000;
    }

    double getForgettingRate() const override {
        return 0.01;
    }

    std::map<std::string, double> getLearningParameters() const override {
        return {
            {"learning_rate", 0.01},
            {"decay_rate", 0.99},
            {"associativity", 0.8}
        };
    }

    void setLearningParameters(const std::map<std::string, double>& params) override {
        learningParameters_ = params;
    }

private:
    PluginState state_;
    bool enabled_;
    std::map<std::string, std::vector<double>> memoryStore_;
    std::map<std::string, double> learningParameters_;
};

} // namespace plugin
} // namespace nlm

extern "C" nlm::plugin::IPlugin* createPlugin() {
    return new nlm::plugin::ExampleMemoryPlugin();
}

extern "C" void destroyPlugin(nlm::plugin::IPlugin* plugin) {
    delete plugin;
}

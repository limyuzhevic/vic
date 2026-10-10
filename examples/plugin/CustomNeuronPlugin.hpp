// Plugin examples in examples/plugin/
#pragma once

#include "../../src/plugin/neuron/INeuronPlugin.hpp"
#include "../../src/plugin/PluginBase.hpp"

namespace nlm {
namespace plugin {

// Custom neuron plugin example implementing the INeuronPlugin interface
class CustomNeuronPlugin : public INeuronPlugin {
private:
    bool initialized;
    PluginConfig currentConfig;
    std::map<std::string, std::string> neuronParameters;
    
public:
    CustomNeuronPlugin() : initialized(false) {}
    
    std::string getName() const override {
        return "CustomNeuronPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getAuthor() const override {
        return "NLM Developer";
    }
    
    std::string getDescription() const override {
        return "A custom neuron plugin that extends basic LIF neurons with additional capabilities";
    }
    
    bool initialize(const PluginConfig& config) override {
        initialized = true;
        currentConfig = config;
        neuronParameters = getDefaultParameters();
        return true;
    }
    
    void shutdown() override {
        initialized = false;
        neuronParameters.clear();
    }
    
    bool isInitialized() const override {
        return initialized;
    }
    
    std::vector<std::string> getDependencies() const override {
        return {"nlm_core"};
    }
    
    // INeuronPlugin interface implementation
    std::shared_ptr<Neuron> createNeuron(NeuronId id) override {
        auto neuron = std::make_shared<Neuron>(id);
        configureNeuron(*neuron, neuronParameters);
        initializeNeuronDynamics(*neuron);
        return neuron;
    }
    
    bool configureNeuron(Neuron& neuron, const std::map<std::string, std::string>& parameters) override {
        // Apply custom parameters to the neuron
        for (const auto& param : parameters) {
            if (param.first == "threshold") {
                neuron.setThreshold(std::stof(param.second));
            } else if (param.first == "refractoryPeriod") {
                neuron.setRefractoryPeriod(static_cast<uint32_t>(std::stoi(param.second)));
            } else if (param.first == "adaptationEnabled") {
                neuron.enablePlasticity(param.second == "true", false, false);
            }
        }
        return true;
    }
    
    std::vector<std::string> getNeuronTypes() const override {
        return {"LIF", "AdaptiveLIF", "ResonantLIF"};
    }
    
    bool initializeNeuronDynamics(Neuron& neuron) override {
        // Custom initialization logic
        neuron.setLeakConductance(10.0f);
        neuron.setRestingPotential(-65.0f);
        neuron.setResetPotential(-70.0f);
        return true;
    }
    
    void updateNeuronState(Neuron& neuron, Timestamp currentTime, TimestepDuration dt) override {
        // Custom state update logic
        neuron.step(currentTime);
    }
    
    bool onNeuronSpike(Neuron& neuron, Timestamp spikeTime) override {
        // Custom spike handling logic
        return true;
    }
    
    std::map<std::string, std::string> getDefaultParameters() const override {
        return {
            {"threshold", "-55.0"},
            {"refractoryPeriod", "5"},
            {"adaptationEnabled", "true"},
            {"leakConductance", "10.0"},
            {"resetPotential", "-70.0"}
        };
    }
    
    std::vector<std::string> getSupportedParameterTypes() const override {
        return {"threshold", "refractoryPeriod", "adaptationEnabled", "leakConductance", "resetPotential"};
    }
    
    bool createNeuronPopulation(std::vector<std::shared_ptr<Neuron>>& neurons, const std::vector<NeuronId>& ids) override {
        neurons.clear();
        for (const auto& id : ids) {
            neurons.push_back(createNeuron(id));
        }
        return true;
    }
    
    bool configureNeuronPopulation(std::vector<std::shared_ptr<Neuron>>& neurons, const std::map<std::string, std::string>& parameters) override {
        for (auto& neuron : neurons) {
            configureNeuron(*neuron, parameters);
        }
        return true;
    }
};

} // namespace plugin
} // namespace nlm
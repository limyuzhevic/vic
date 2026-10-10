// Izhikevich Neuron Model Plugin
// Example plugin demonstrating a custom neuron model (Izhikevich model)

#include "plugins/PluginSystem.hpp"
#include "brain/Neuron.hpp"
#include <cmath>

namespace nlm {

class IzhikevichNeuronModelPlugin : public NeuronModelPlugin {
public:
    IzhikevichNeuronModelPlugin() : PluginInterface() {
        metadata.name = "izhi2007";
        metadata.version = "1.0.0";
        metadata.author = "NLM Plugin Examples";
        metadata.description = "Izhikevich 2007 model - adaptive exponential integrate-and-fire";
        metadata.license = "MIT";
        metadata.type = PluginType::NeuronModel;
        metadata.tags = {"neuron", "adaptative", "spiking", "biophysical"};
        
        capabilities.type = PluginType::NeuronModel;
        capabilities.name = "izhi2007";
        capabilities.description = "Izhikevich 2007 adaptive exponential integrate-and-fire neuron model";
        capabilities.version = "1.0.0";
        capabilities.dependencies = {};
        capabilities.isCorePlugin = false;
    }
    
    const PluginMetadata& getMetadata() const override {
        return metadata;
    }
    
    bool initialize() override {
        // Load default parameters
        defaultParameters = {
            {"a", 0.02f},
            {"b", -65.0f},
            {"c", 8.0f},
            {"d", 0.2f},
            {"v_peak", 30.0f},
            {"threshold", -50.0f}
        };
        initialized = true;
        return true;
    }
    
    void shutdown() override {
        initialized = false;
    }
    
    bool isInitialized() const override {
        return initialized;
    }
    
    const PluginCapabilities& getCapabilities() const override {
        return capabilities;
    }
    
    std::unique_ptr<Neuron> createNeuron(NeuronId id) override {
        // Create a neuron with Izhikevich dynamics
        auto neuron = std::make_unique<Neuron>(id);
        
        // Set up Izhikevich parameters
        neuron->setType(NeuronType::Internal);
        neuron->setMembranePotential(-70.0f); // Initial v
        neuron->setThreshold(-50.0f); // Firing threshold
        neuron->setRestingPotential(-70.0f);
        neuron->setResetPotential(-70.0f);
        neuron->setFiringState(FiringState::Resting);
        
        // Store model-specific parameters in neuron state
        // In a real implementation, this would be extended with more state variables
        
        return neuron;
    }
    
    const std::unordered_map<std::string, float>& getDefaultParameters() const override {
        return defaultParameters;
    }
    
    bool validateParameters(const std::unordered_map<std::string, float>& params) const override {
        // Validate Izhikevich parameters
        if (params.find("a") == params.end() || params.at("a") < 0.0f) return false;
        if (params.find("b") == params.end()) return false;
        if (params.find("c") == params.end()) return false;
        if (params.find("d") == params.end() || params.at("d") < 0.0f) return false;
        if (params.find("v_peak") == params.end()) return false;
        if (params.find("threshold") == params.end()) return false;
        
        // Additional constraints
        if (params.at("a") > 1.0f) return false;
        if (params.at("d") > 0.5f) return false;
        
        return true;
    }
    
    std::string getModelName() const override {
        return "Izhikevich 2007 Model";
    }
    
private:
    PluginMetadata metadata;
    PluginCapabilities capabilities;
    std::unordered_map<std::string, float> defaultParameters;
    bool initialized = false;
};

// Izhikevich Neuron Model Factory
class IzhikevichNeuronModelPluginFactory : public PluginFactory {
public:
    std::unique_ptr<PluginInterface> createPlugin(const PluginMetadata& metadata) override {
        if (metadata.name == "izhi2007") {
            return std::make_unique<IzhikevichNeuronModelPlugin>();
        }
        return nullptr;
    }
    
    bool canCreate(const PluginMetadata& metadata) const override {
        return metadata.name == "izhi2007" || metadata.name == "izhi" || metadata.name == "Izhikevich";
    }
};

} // namespace nlm

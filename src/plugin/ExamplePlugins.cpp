// Izhikevich Neuron Model Plugin
// Demonstrates plugin-based neuron model extensibility

#include "PluginInterface.hpp"
#include "../brain/Neuron.hpp"
#include <cmath>
#include <random>

namespace nlm {
namespace plugin {

class IzhikevichNeuron : public IPlugin {
public:
    struct Parameters {
        float a = 0.02f;      // Recovery variable time constant
        float b = 0.2f;       // Recovery variable sensitivity
        float c = -65.0f;     // After-spike reset of membrane potential
        float d = 8.0f;       // After-spike reset of recovery variable
        float v_thresh = 30.0f; // Spike threshold
        float dt = 0.005f;    // Time step
    };
    
    struct State {
        float v = -70.0f;     // Membrane potential
        float u = -14.0f;     // Recovery variable
        bool spiking = false; // Spike flag
        float lastSpikeTime = -1000.0f;
    };
    
    IzhikevichNeuron() = default;
    
    PluginMetadata getMetadata() const override {
        static const PluginMetadata metadata = {
            "IzhikevichNeuron",
            "1.0.0",
            "Biophysically detailed neuron model (Izhikevich 2003)",
            "NLM Research Team",
            "MIT",
            PluginType::Extension,
            PluginCapability::NeuralModel,
            {},  // No dependencies
            "https://www.izhikevich.org/publications/2003/2003_Izhikevich_Neuron.pdf",
            "nlm-research@nlm.org"
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override {
        // Load parameters from configuration
        for (const auto& pair : config) {
            if (pair.first == "a") parameters_.a = std::any_cast<float>(pair.second);
            else if (pair.first == "b") parameters_.b = std::any_cast<float>(pair.second);
            else if (pair.first == "c") parameters_.c = std::any_cast<float>(pair.second);
            else if (pair.first == "d") parameters_.d = std::any_cast<float>(pair.second);
            else if (pair.first == "v_thresh") parameters_.v_thresh = std::any_cast<float>(pair.second);
            else if (pair.first == "dt") parameters_.dt = std::any_cast<float>(pair.second);
        }
        
        rng_.seed(std::random_device{}());
        return true;
    }
    
    bool start() override {
        return true;
    }
    
    bool stop() override {
        return true;
    }
    
    bool shutdown() override {
        return true;
    }
    
    bool hasCapability(PluginCapability capability) const override {
        return capability == PluginCapability::NeuralModel;
    }
    
    std::vector<PluginCapability> getCapabilities() const override {
        return {PluginCapability::NeuralModel};
    }
    
    void registerEventCallback(const PluginEventCallback& callback) override {
        eventCallback_ = callback;
    }
    
    void updateConfiguration(const std::unordered_map<std::string, std::any>& config) override {
        initialize(config);
    }
    
    std::unordered_map<std::string, std::any> getConfiguration() const override {
        return {
            {"a", parameters_.a},
            {"b", parameters_.b},
            {"c", parameters_.c},
            {"d", parameters_.d},
            {"v_thresh", parameters_.v_thresh},
            {"dt", parameters_.dt}
        };
    }
    
    // Izhikevich neuron simulation
    void step(float currentTime) {
        State& state = state_;
        const Parameters& params = parameters_;
        
        // Update membrane potential
        float dv = state.v + params.a * (state.u - state.v);
        state.v += params.dt * dv;
        
        // Check for spike
        if (state.v >= params.v_thresh) {
            state.spiking = true;
            state.u += params.d;
            state.v = params.c;
            state.lastSpikeTime = currentTime;
            
            // Emit spike event
            if (eventCallback_) {
                PluginEvent event;
                event.type = PluginEventType::NeuralEvent;
                event.sourcePlugin = getMetadata().name;
                event.timestamp = currentTime;
                event.data = {
                    {"neuron_type", "izhikevich"},
                    {"membrane_potential", state.v},
                    {"recovery_variable", state.u},
                    {"spike_time", currentTime}
                };
                eventCallback_(event);
            }
        }
        
        // Update recovery variable
        state.u += params.dt * (params.b * (state.v - state.u));
    }
    
    bool isSpiking() const {
        return state_.spiking;
    }
    
    float getMembranePotential() const {
        return state_.v;
    }
    
    float getRecoveryVariable() const {
        return state_.u;
    }
    
    float getLastSpikeTime() const {
        return state_.lastSpikeTime;
    }
    
private:
    Parameters parameters_;
    State state_;
    PluginEventCallback eventCallback_;
    std::mt19937 rng_;
};

// Custom Plasticity Rule Plugin
// Demonstrates plugin-based synaptic plasticity rules

class CustomPlasticityRule : public IPlugin {
public:
    struct RuleParameters {
        float learningRate = 0.01f;
        float maxWeightChange = 0.1f;
        float decayRate = 0.99f;
        bool enableLTP = true;
        bool enableLTD = true;
        float ltpThreshold = 0.5f;
        float ltdThreshold = -0.5f;
    };
    
    CustomPlasticityRule() = default;
    
    PluginMetadata getMetadata() const override {
        static const PluginMetadata metadata = {
            "CustomPlasticityRule",
            "1.0.0",
            "Custom synaptic plasticity rule with adaptive learning",
            "NLM Research Team",
            "MIT",
            PluginType::Extension,
            PluginCapability::PlasticityRule,
            {},  // No dependencies
            "",
            "nlm-research@nlm.org"
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override {
        // Load rule parameters
        for (const auto& pair : config) {
            if (pair.first == "learning_rate") parameters_.learningRate = std::any_cast<float>(pair.second);
            else if (pair.first == "max_weight_change") parameters_.maxWeightChange = std::any_cast<float>(pair.second);
            else if (pair.first == "decay_rate") parameters_.decayRate = std::any_cast<float>(pair.second);
            else if (pair.first == "enable_ltp") parameters_.enableLTP = std::any_cast<bool>(pair.second);
            else if (pair.first == "enable_ltd") parameters_.enableLTD = std::any_cast<bool>(pair.second);
            else if (pair.first == "ltp_threshold") parameters_.ltpThreshold = std::any_cast<float>(pair.second);
            else if (pair.first == "ltd_threshold") parameters_.ltdThreshold = std::any_cast<float>(pair.second);
        }
        return true;
    }
    
    bool start() override {
        return true;
    }
    
    bool stop() override {
        return true;
    }
    
    bool shutdown() override {
        return true;
    }
    
    bool hasCapability(PluginCapability capability) const override {
        return capability == PluginCapability::PlasticityRule;
    }
    
    std::vector<PluginCapability> getCapabilities() const override {
        return {PluginCapability::PlasticityRule};
    }
    
    void registerEventCallback(const PluginEventCallback& callback) override {
        eventCallback_ = callback;
    }
    
    void updateConfiguration(const std::unordered_map<std::string, std::any>& config) override {
        initialize(config);
    }
    
    std::unordered_map<std::string, std::any> getConfiguration() const override {
        return {
            {"learning_rate", parameters_.learningRate},
            {"max_weight_change", parameters_.maxWeightChange},
            {"decay_rate", parameters_.decayRate},
            {"enable_ltp", parameters_.enableLTP},
            {"enable_ltd", parameters_.enableLTD},
            {"ltp_threshold", parameters_.ltpThreshold},
            {"ltd_threshold", parameters_.ltdThreshold}
        };
    }
    
    // Apply custom plasticity rule to synapse
    float applyRule(float currentWeight, float preSpikeTime, float postSpikeTime, float currentTime) {
        if (!parameters_.enableLTP && !parameters_.enableLTD) {
            return currentWeight;
        }
        
        float delta = 0.0f;
        
        // Calculate spike timing difference
        float spikeDiff = postSpikeTime - preSpikeTime;
        
        // LTP (Long-Term Potentiation)
        if (parameters_.enableLTP && spikeDiff > 0) {
            float ltpFactor = std::exp(-spikeDiff / 20.0f);  // Modified STDP curve
            delta += parameters_.learningRate * ltpFactor;
        }
        
        // LTD (Long-Term Depression)
        if (parameters_.enableLTD && spikeDiff < 0) {
            float ltdFactor = std::exp(spikeDiff / 20.0f);  // Modified STDP curve
            delta -= parameters_.learningRate * ltdFactor;
        }
        
        // Apply adaptive learning rate based on weight
        float weightAbs = std::abs(currentWeight);
        delta *= (1.0f - weightAbs);  // Less learning near weight bounds
        
        // Clamp weight change
        if (delta > parameters_.maxWeightChange) delta = parameters_.maxWeightChange;
        if (delta < -parameters_.maxWeightChange) delta = -parameters_.maxWeightChange;
        
        // Apply decay
        delta *= parameters_.decayRate;
        
        return delta;
    }
    
private:
    RuleParameters parameters_;
    PluginEventCallback eventCallback_;
};

} // namespace plugin
} // namespace nlm

// Plugin factory implementations
namespace nlm {
namespace plugin {

// Izhikevich Neuron factory
class IzhikevichNeuronFactory : public PluginFactory<IzhikevichNeuron> {
public:
    IzhikevichNeuronFactory() 
        : PluginFactory<IzhikevichNeuron>(
            "IzhikevichNeuron",
            "1.0.0",
            "Biophysically detailed neuron model (Izhikevich 2003)",
            PluginCapability::NeuralModel,
            []() { return std::make_unique<IzhikevichNeuron>(); }
        ) {}
};

// Custom Plasticity Rule factory
class CustomPlasticityRuleFactory : public PluginFactory<CustomPlasticityRule> {
public:
    CustomPlasticityRuleFactory() 
        : PluginFactory<CustomPlasticityRule>(
            "CustomPlasticityRule",
            "1.0.0",
            "Custom synaptic plasticity rule with adaptive learning",
            PluginCapability::PlasticityRule,
            []() { return std::make_unique<CustomPlasticityRule>(); }
        ) {}
};

// Factory registration
static IzhikevichNeuronFactory izhikevichFactory;
static CustomPlasticityRuleFactory customPlasticityFactory;

} // namespace plugin
} // namespace nlm

// External plugin entry point
extern "C" {
    NLM_API std::unique_ptr<nlm::plugin::IPlugin> createPlugin() {
        // This would normally be replaced by actual plugin loading
        return nullptr;
    }
}
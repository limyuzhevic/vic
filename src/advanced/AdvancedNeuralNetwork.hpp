#ifndef ADVANCED_NEURAL_NETWORK_HPP
#define ADVANCED_NEURAL_NETWORK_HPP

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <eigen3/Eigen/Dense>
#include <optional>
#include <variant>

namespace nlm {
namespace advanced {

// Layer type enumeration
enum class LayerType {
    INPUT,
    HIDDEN,
    OUTPUT,
    CONVOLUTIONAL,
    RECURRENT,
    ATTENTION,
    CUSTOM
};

// Connection pattern types
enum class ConnectionPattern {
    FULLY_CONNECTED,
    SPARSE,
    CONVOLUTIONAL,
    RECURRENT,
    RESIDUAL,
    SKIP_CONNECTION,
    CUSTOM
};

// Activation functions
enum class ActivationFunction {
    RELU,
    SIGMOID,
    TANH,
    SOFTMAX,
    LEAKY_RELU,
    ELU,
    SWISH,
    GELU,
    CUSTOM
};

// Advanced neuron model parameters
struct AdvancedNeuronParameters {
    double membrane_potential;      // Current membrane potential
    double threshold;               // Firing threshold
    double refractory_period;       // Refractory period
    double time_constant;           // Membrane time constant
    double capacitance;             // Membrane capacitance
    double reset_potential;         // Reset potential after spike
    double adaptation_strength;     // Spike-frequency adaptation
    double integration_method;      // Integration method (Euler, Runge-Kutta)
    std::string neuron_model;       // Model type (LIF, Izhikevich, etc.)
};

// Advanced synapse parameters
struct AdvancedSynapseParameters {
    double weight;                  // Synaptic weight
    double delay;                   // Transmission delay
    double conductance;             // Synaptic conductance
    double reversal_potential;      // Reversal potential
    double plasticity_rate;         // Plasticity learning rate
    double basal_plasticity;        // Baseline plasticity
    std::string plasticity_rule;    // Plasticity rule type
    std::map<std::string, double> additional_params;
};

// Layer configuration
struct LayerConfig {
    LayerType type;
    size_t neurons;
    ActivationFunction activation;
    std::map<std::string, double> parameters;
    std::string custom_activation_code;
    bool is_trainable;
};

// Network configuration
struct NetworkConfig {
    std::string name;
    std::vector<LayerConfig> layers;
    ConnectionPattern connection_pattern;
    std::map<std::string, double> global_parameters;
    std::map<std::string, std::string> custom_connections;
    bool dynamic_architecture;
    bool use_gpu;
    int num_parallel_threads;
};

// Forward declaration of NetworkBuilder class
class NetworkBuilder;

// Advanced neural network class
class AdvancedNeuralNetwork {
private:
    NetworkConfig config_;
    std::vector<AdvancedNeuronParameters> neurons_;
    std::vector<std::vector<AdvancedSynapseParameters>> synapses_;
    std::vector<std::vector<double>> weights_;
    std::vector<std::vector<double>> biases_;
    std::map<std::string, std::vector<double>> layer_activations_;
    std::map<std::string, std::vector<std::vector<double>>> layer_gradients_;
    bool initialized_;
    size_t next_free_neuron_index_;

public:
    AdvancedNeuralNetwork();
    AdvancedNeuralNetwork(const NetworkConfig& config);
    ~AdvancedNeuralNetwork() = default;

    // Network building interface
    static NetworkBuilder create() { return NetworkBuilder(); }

    // Core functionality
    bool initialize();
    std::vector<double> forwardPropagate(const std::vector<double>& input);
    void backwardPropagate(const std::vector<double>& error, double learning_rate);
    void updateWeights();
    void reset();

    // Custom architecture operations
    bool addLayer(const LayerConfig& layer_config);
    bool connectLayers(const std::string& source_layer, const std::string& target_layer, 
                      const ConnectionPattern& pattern);
    bool removeLayer(const std::string& layer_name);
    bool resizeLayer(const std::string& layer_name, size_t new_size);

    // Dynamic architecture
    bool addNeuron(const std::string& layer_name, const AdvancedNeuronParameters& params);
    bool removeNeuron(const std::string& layer_name, size_t neuron_index);
    bool addSynapse(const std::string& source_layer, size_t source_neuron,
                   const std::string& target_layer, size_t target_neuron,
                   const AdvancedSynapseParameters& params);

    // Performance and optimization
    void enableGPUAcceleration() { config_.use_gpu = true; }
    void setParallelThreads(int threads) { config_.num_parallel_threads = threads; }
    void enableDynamicArchitecture(bool enable) { config_.dynamic_architecture = enable; }

    // Configuration and management
    void setConfig(const NetworkConfig& config) { config_ = config; }
    const NetworkConfig& getConfig() const { return config_; }
    bool isInitialized() const { return initialized_; }

    // Getters
    std::vector<double> getLayerOutput(const std::string& layer_name) const;
    std::vector<std::vector<double>> getLayerWeights(const std::string& layer_name) const;
    std::vector<AdvancedNeuronParameters> getNeurons(const std::string& layer_name) const;
    std::vector<std::vector<AdvancedSynapseParameters>> getSynapses() const { return synapses_; }

    // Serialization
    bool saveToFile(const std::string& filename) const;
    static std::unique_ptr<AdvancedNeuralNetwork> loadFromFile(const std::string& filename);
};

// Network builder class for constructing advanced networks
class NetworkBuilder {
private:
    NetworkConfig config_;
    std::map<std::string, LayerConfig> layer_registry_;

public:
    NetworkBuilder& name(const std::string& name) {
        config_.name = name;
        return *this;
    }

    NetworkBuilder& addLayer(const LayerConfig& layer_config) {
        config_.layers.push_back(layer_config);
        return *this;
    }

    NetworkBuilder& connectionPattern(ConnectionPattern pattern) {
        config_.connection_pattern = pattern;
        return *this;
    }

    NetworkBuilder& globalParameter(const std::string& key, double value) {
        config_.global_parameters[key] = value;
        return *this;
    }

    NetworkBuilder& enableGPU(bool enable = true) {
        config_.use_gpu = enable;
        return *this;
    }

    NetworkBuilder& dynamicArchitecture(bool enable = true) {
        config_.dynamic_architecture = enable;
        return *this;
    }

    std::unique_ptr<AdvancedNeuralNetwork> build() {
        return std::make_unique<AdvancedNeuralNetwork>(config_);
    }
};

} // namespace advanced
} // namespace nlm

#endif // ADVANCED_NEURAL_NETWORK_HPP
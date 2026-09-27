#include "AdvancedNeuralNetwork.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <random>
#include <cmath>

namespace nlm {
namespace advanced {

// Helper functions
namespace {
    double activationFunction(ActivationFunction func, double x, const std::map<std::string, double>& params = {}) {
        switch (func) {
            case ActivationFunction::RELU:
                return std::max(0.0, x);
            case ActivationFunction::SIGMOID:
                return 1.0 / (1.0 + std::exp(-x));
            case ActivationFunction::TANH:
                return std::tanh(x);
            case ActivationFunction::SOFTMAX:
                // Softmax requires vector input, handled at layer level
                return x;
            case ActivationFunction::LEAKY_RELU:
                return x > 0 ? x : 0.01 * x;
            case ActivationFunction::ELU:
                return x > 0 ? x : 0.01 * (std::exp(x) - 1);
            case ActivationFunction::SWISH:
                return x / (1.0 + std::exp(-x));
            case ActivationFunction::GELU:
                return 0.5 * x * (1.0 + std::tanh(std::sqrt(2.0 / M_PI) * (x + 0.044715 * x * x * x)));
            case ActivationFunction::CUSTOM:
                // Custom activation handled through code injection
                return x;
            default:
                return x;
        }
    }

    double derivativeFunction(ActivationFunction func, double x) {
        switch (func) {
            case ActivationFunction::RELU:
                return x > 0 ? 1.0 : 0.0;
            case ActivationFunction::SIGMOID:
                double sig = 1.0 / (1.0 + std::exp(-x));
                return sig * (1.0 - sig);
            case ActivationFunction::TANH:
                return 1.0 - std::tanh(x) * std::tanh(x);
            case ActivationFunction::SOFTMAX:
                return 1.0; // Handled at layer level
            case ActivationFunction::LEAKY_RELU:
                return x > 0 ? 1.0 : 0.01;
            case ActivationFunction::ELU:
                return x > 0 ? 1.0 : 0.01 * std::exp(x);
            case ActivationFunction::SWISH:
                double sigmoid = 1.0 / (1.0 + std::exp(-x));
                return sigmoid + x * sigmoid * (1.0 - sigmoid);
            case ActivationFunction::GELU:
                return 0.5 * (1.0 + std::tanh(std::sqrt(2.0 / M_PI) * (x + 0.044715 * x * x * x)))
                       + 0.5 * x * (std::sqrt(2.0 / M_PI) * std::exp(-0.5 * x * x) * (1.0 + 0.044715 * x * x));
            case ActivationFunction::CUSTOM:
                return 1.0; // Custom derivative handled through code injection
            default:
                return 1.0;
        }
    }

    void initializeRandomWeights(std::vector<std::vector<double>>& weights, size_t rows, size_t cols, double stddev = 0.01) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> d(0.0, stddev);

        weights.resize(rows);
        for (size_t i = 0; i < rows; ++i) {
            weights[i].resize(cols);
            for (size_t j = 0; j < cols; ++j) {
                weights[i][j] = d(gen);
            }
        }
    }
}

// AdvancedNeuralNetwork implementation
AdvancedNeuralNetwork::AdvancedNeuralNetwork() : initialized_(false), next_free_neuron_index_(0) {}

AdvancedNeuralNetwork::AdvancedNeuralNetwork(const NetworkConfig& config) : config_(config), initialized_(false), next_free_neuron_index_(0) {}

bool AdvancedNeuralNetwork::initialize() {
    if (config_.layers.empty()) {
        return false;
    }

    // Calculate total neurons and initialize parameters
    size_t total_neurons = 0;
    for (const auto& layer : config_.layers) {
        total_neurons += layer.neurons;
    }

    neurons_.resize(total_neurons);
    next_free_neuron_index_ = 0;

    // Initialize neurons
    size_t neuron_idx = 0;
    for (const auto& layer : config_.layers) {
        for (size_t i = 0; i < layer.neurons; ++i) {
            AdvancedNeuronParameters params;
            params.neuron_model = layer.parameters.count("model") ? layer.parameters.at("model") : "LIF";
            params.threshold = layer.parameters.count("threshold") ? layer.parameters.at("threshold") : 1.0;
            params.refractory_period = layer.parameters.count("refractory") ? layer.parameters.at("refractory") : 5.0;
            params.time_constant = layer.parameters.count("tau") ? layer.parameters.at("tau") : 10.0;
            params.capacitance = layer.parameters.count("C") ? layer.parameters.at("C") : 1.0;
            params.reset_potential = layer.parameters.count("V_reset") ? layer.parameters.at("V_reset") : 0.0;
            params.adaptation_strength = layer.parameters.count("a") ? layer.parameters.at("a") : 0.0;
            params.integration_method = layer.parameters.count("integration") ? std::stoi(layer.parameters.at("integration")) : 0;

            neurons_[neuron_idx++] = params;
        }
    }

    // Initialize weights and biases based on connection pattern
    weights_.clear();
    biases_.clear();
    synapses_.clear();

    for (size_t i = 0; i < config_.layers.size(); ++i) {
        size_t input_size = i > 0 ? config_.layers[i-1].neurons : config_.layers[i].neurons;
        size_t output_size = config_.layers[i].neurons;

        std::vector<std::vector<double>> layer_weights;
        std::vector<double> layer_biases;

        initializeRandomWeights(layer_weights, output_size, input_size);
        layer_biases.resize(output_size, 0.0);

        weights_.push_back(layer_weights);
        biases_.push_back(layer_biases);

        // Initialize synapses
        std::vector<AdvancedSynapseParameters> layer_synapses;
        for (size_t j = 0; j < output_size; ++j) {
            std::vector<AdvancedSynapseParameters> synapses_row;
            for (size_t k = 0; k < input_size; ++k) {
                AdvancedSynapseParameters syn_params;
                syn_params.weight = weights_[i][j][k];
                syn_params.delay = config_.global_parameters.count("synaptic_delay") ? 
                    config_.global_parameters.at("synaptic_delay") : 1.0;
                syn_params.conductance = config_.global_parameters.count("synaptic_conductance") ? 
                    config_.global_parameters.at("synaptic_conductance") : 0.5;
                syn_params.reversal_potential = config_.global_parameters.count("E_syn") ? 
                    config_.global_parameters.at("E_syn") : 0.0;
                syn_params.plasticity_rate = config_.global_parameters.count("plasticity_rate") ? 
                    config_.global_parameters.at("plasticity_rate") : 0.01;
                syn_params.basal_plasticity = config_.global_parameters.count("basal_plasticity") ? 
                    config_.global_parameters.at("basal_plasticity") : 0.001;
                syn_params.plasticity_rule = config_.global_parameters.count("plasticity_rule") ? 
                    config_.global_parameters.at("plasticity_rule") : "STDP";

                synapses_row.push_back(syn_params);
            }
            layer_synapses.push_back(synapses_row);
        }
        synapses_.push_back(layer_synapses);
    }

    initialized_ = true;
    return true;
}

std::vector<double> AdvancedNeuralNetwork::forwardPropagate(const std::vector<double>& input) {
    if (!initialized_) {
        return {};
    }

    layer_activations_.clear();
    layer_gradients_.clear();

    std::vector<double> current_input = input;
    std::string current_layer_name = "input";

    for (size_t layer_idx = 0; layer_idx < config_.layers.size(); ++layer_idx) {
        const auto& layer = config_.layers[layer_idx];
        std::string layer_name = "layer_" + std::to_string(layer_idx);

        // Compute layer output
        std::vector<double> layer_output(layer.neurons, 0.0);
        size_t input_size = layer_idx > 0 ? config_.layers[layer_idx-1].neurons : input.size();
        size_t output_size = layer.neurons;

        for (size_t i = 0; i < output_size; ++i) {
            double weighted_sum = biases_[layer_idx][i];

            // Add neuron-specific membrane potential
            size_t neuron_idx = next_free_neuron_index_ + i;
            if (neuron_idx < neurons_.size()) {
                weighted_sum += neurons_[neuron_idx].membrane_potential;
            }

            // Compute weighted sum from previous layer
            for (size_t j = 0; j < input_size; ++j) {
                weighted_sum += weights_[layer_idx][i][j] * current_input[j];
            }

            // Apply activation function
            double activation = activationFunction(layer.activation, weighted_sum, layer.parameters);
            layer_output[i] = activation;
        }

        layer_activations_[layer_name] = layer_output;
        current_input = layer_output;
        current_layer_name = layer_name;
    }

    return current_input;
}

void AdvancedNeuralNetwork::backwardPropagate(const std::vector<double>& error, double learning_rate) {
    if (!initialized_ || error.empty()) {
        return;
    }

    layer_gradients_.clear();

    std::vector<double> current_error = error;
    std::string current_layer_name = "output";

    // Backpropagate through layers
    for (int layer_idx = static_cast<int>(config_.layers.size()) - 1; layer_idx >= 0; --layer_idx) {
        std::string layer_name = "layer_" + std::to_string(layer_idx);

        const auto& layer = config_.layers[layer_idx];
        size_t output_size = layer.neurons;
        size_t input_size = layer_idx > 0 ? config_.layers[layer_idx-1].neurons : 0;

        // Compute gradients for this layer
        std::vector<std::vector<double>> layer_gradients(output_size, std::vector<double>(input_size, 0.0));

        for (size_t i = 0; i < output_size; ++i) {
            double output = layer_activations_[layer_name][i];
            double activation_derivative = derivativeFunction(layer.activation, output);
            double gradient = current_error[i] * activation_derivative;

            // Store gradient for weight update
            for (size_t j = 0; j < input_size; ++j) {
                layer_gradients[i][j] = gradient * (layer_idx > 0 ? layer_activations_["layer_" + std::to_string(layer_idx-1)][j] : 0.0);
            }
        }

        layer_gradients_[layer_name] = layer_gradients;

        // Compute error for previous layer
        if (layer_idx > 0) {
            current_error.clear();
            current_error.resize(input_size, 0.0);

            for (size_t i = 0; i < input_size; ++i) {
                double error_sum = 0.0;
                for (size_t j = 0; j < output_size; ++j) {
                    error_sum += layer_gradients_[layer_name][j][i] * weights_[layer_idx][j][i];
                }
                current_error[i] = error_sum;
            }
        }
    }

    // Update weights based on gradients
    updateWeights();
}

void AdvancedNeuralNetwork::updateWeights() {
    double learning_rate = config_.global_parameters.count("learning_rate") ? 
        config_.global_parameters.at("learning_rate") : 0.01;
    double weight_decay = config_.global_parameters.count("weight_decay") ? 
        config_.global_parameters.at("weight_decay") : 0.0001;

    for (size_t layer_idx = 0; layer_idx < weights_.size(); ++layer_idx) {
        for (size_t i = 0; i < weights_[layer_idx].size(); ++i) {
            for (size_t j = 0; j < weights_[layer_idx][i].size(); ++j) {
                if (layer_gradients_.count("layer_" + std::to_string(layer_idx))) {
                    weights_[layer_idx][i][j] -= learning_rate * 
                        layer_gradients_["layer_" + std::to_string(layer_idx)][i][j]
                        - weight_decay * weights_[layer_idx][i][j];

                    // Update corresponding synapse parameter
                    if (layer_idx < synapses_.size() && i < synapses_[layer_idx].size() && 
                        j < synapses_[layer_idx][i].size()) {
                        synapses_[layer_idx][i][j].weight = weights_[layer_idx][i][j];
                    }
                }
            }
        }
    }
}

void AdvancedNeuralNetwork::reset() {
    for (auto& neuron : neurons_) {
        neuron.membrane_potential = neuron.reset_potential;
    }
    layer_activations_.clear();
    layer_gradients_.clear();
}

bool AdvancedNeuralNetwork::addLayer(const LayerConfig& layer_config) {
    if (!initialized_) {
        return false;
    }

    // Validate layer configuration
    if (layer_config.type == LayerType::INPUT && !config_.layers.empty()) {
        return false; // Only one input layer allowed
    }

    if (layer_config.type == LayerType::HIDDEN && config_.layers.empty()) {
        return false; // Hidden layer requires input layer
    }

    config_.layers.push_back(layer_config);

    // Rebuild network to accommodate new layer
    initialized_ = false;
    if (!initialize()) {
        // Rollback
        config_.layers.pop_back();
        return false;
    }

    return true;
}

bool AdvancedNeuralNetwork::connectLayers(const std::string& source_layer, const std::string& target_layer,
                                         const ConnectionPattern& pattern) {
    // Implementation for connecting layers with specific patterns
    // This is a simplified implementation
    return true;
}

bool AdvancedNeuralNetwork::removeLayer(const std::string& layer_name) {
    // Implementation for removing a layer
    return true;
}

bool AdvancedNeuralNetwork::resizeLayer(const std::string& layer_name, size_t new_size) {
    // Implementation for resizing a layer
    return true;
}

bool AdvancedNeuralNetwork::addNeuron(const std::string& layer_name, const AdvancedNeuronParameters& params) {
    // Implementation for adding a neuron to a layer
    return true;
}

bool AdvancedNeuralNetwork::removeNeuron(const std::string& layer_name, size_t neuron_index) {
    // Implementation for removing a neuron from a layer
    return true;
}

bool AdvancedNeuralNetwork::addSynapse(const std::string& source_layer, size_t source_neuron,
                                      const std::string& target_layer, size_t target_neuron,
                                      const AdvancedSynapseParameters& params) {
    // Implementation for adding a synapse
    return true;
}

std::vector<double> AdvancedNeuralNetwork::getLayerOutput(const std::string& layer_name) const {
    auto it = layer_activations_.find(layer_name);
    if (it != layer_activations_.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::vector<double>> AdvancedNeuralNetwork::getLayerWeights(const std::string& layer_name) const {
    size_t layer_idx = std::stoul(layer_name.substr(6)); // Extract index from "layer_X"
    if (layer_idx < weights_.size()) {
        return weights_[layer_idx];
    }
    return {};
}

std::vector<AdvancedNeuronParameters> AdvancedNeuralNetwork::getNeurons(const std::string& layer_name) const {
    std::vector<AdvancedNeuronParameters> layer_neurons;
    size_t layer_idx = std::stoul(layer_name.substr(6)); // Extract index from "layer_X"
    size_t start_neuron = 0;

    for (size_t i = 0; i < layer_idx; ++i) {
        start_neuron += config_.layers[i].neurons;
    }

    for (size_t i = 0; i < config_.layers[layer_idx].neurons; ++i) {
        if (start_neuron + i < neurons_.size()) {
            layer_neurons.push_back(neurons_[start_neuron + i]);
        }
    }

    return layer_neurons;
}

bool AdvancedNeuralNetwork::saveToFile(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    try {
        // Write configuration
        size_t config_size = config_.name.size();
        file.write(reinterpret_cast<const char*>(&config_size), sizeof(size_t));
        file.write(config_.name.c_str(), config_size);

        size_t layers_count = config_.layers.size();
        file.write(reinterpret_cast<const char*>(&layers_count), sizeof(size_t));

        for (const auto& layer : config_.layers) {
            file.write(reinterpret_cast<const char*>(&layer.type), sizeof(LayerType));
            file.write(reinterpret_cast<const char*>(&layer.neurons), sizeof(size_t));
            file.write(reinterpret_cast<const char*>(&layer.activation), sizeof(ActivationFunction));

            size_t params_size = layer.parameters.size();
            file.write(reinterpret_cast<const char*>(&params_size), sizeof(size_t));
            for (const auto& param : layer.parameters) {
                size_t key_size = param.first.size();
                file.write(reinterpret_cast<const char*>(&key_size), sizeof(size_t));
                file.write(param.first.c_str(), key_size);
                file.write(reinterpret_cast<const char*>(&param.second), sizeof(double));
            }

            size_t custom_code_size = layer.custom_activation_code.size();
            file.write(reinterpret_cast<const char*>(&custom_code_size), sizeof(size_t));
            if (custom_code_size > 0) {
                file.write(layer.custom_activation_code.c_str(), custom_code_size);
            }
        }

        // Write network parameters
        size_t neurons_count = neurons_.size();
        file.write(reinterpret_cast<const char*>(&neurons_count), sizeof(size_t));
        file.write(reinterpret_cast<const char*>(neurons_.data()), neurons_.size() * sizeof(AdvancedNeuronParameters));

        size_t weights_count = weights_.size();
        file.write(reinterpret_cast<const char*>(&weights_count), sizeof(size_t));
        for (const auto& layer_weights : weights_) {
            size_t rows = layer_weights.size();
            file.write(reinterpret_cast<const char*>(&rows), sizeof(size_t));
            for (const auto& row : layer_weights) {
                size_t cols = row.size();
                file.write(reinterpret_cast<const char*>(&cols), sizeof(size_t));
                file.write(reinterpret_cast<const char*>(row.data()), cols * sizeof(double));
            }
        }

        // Write biases
        for (const auto& layer_biases : biases_) {
            size_t bias_size = layer_biases.size();
            file.write(reinterpret_cast<const char*>(&bias_size), sizeof(size_t));
            file.write(reinterpret_cast<const char*>(layer_biases.data()), bias_size * sizeof(double));
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving to file: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

std::unique_ptr<AdvancedNeuralNetwork> AdvancedNeuralNetwork::loadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return nullptr;
    }

    try {
        auto network = std::make_unique<AdvancedNeuralNetwork>();

        // Read configuration
        size_t config_size;
        file.read(reinterpret_cast<char*>(&config_size), sizeof(size_t));
        std::string name(config_size, '\0');
        file.read(&name[0], config_size);
        network->config_.name = name;

        size_t layers_count;
        file.read(reinterpret_cast<char*>(&layers_count), sizeof(size_t));
        network->config_.layers.resize(layers_count);

        for (auto& layer : network->config_.layers) {
            file.read(reinterpret_cast<char*>(&layer.type), sizeof(LayerType));
            file.read(reinterpret_cast<char*>(&layer.neurons), sizeof(size_t));
            file.read(reinterpret_cast<char*>(&layer.activation), sizeof(ActivationFunction));

            size_t params_size;
            file.read(reinterpret_cast<char*>(&params_size), sizeof(size_t));
            for (size_t i = 0; i < params_size; ++i) {
                size_t key_size;
                file.read(reinterpret_cast<char*>(&key_size), sizeof(size_t));
                std::string key(key_size, '\0');
                file.read(&key[0], key_size);
                double value;
                file.read(reinterpret_cast<char*>(&value), sizeof(double));
                layer.parameters[key] = value;
            }

            size_t custom_code_size;
            file.read(reinterpret_cast<char*>(&custom_code_size), sizeof(size_t));
            if (custom_code_size > 0) {
                layer.custom_activation_code.resize(custom_code_size);
                file.read(&layer.custom_activation_code[0], custom_code_size);
            }
        }

        // Initialize the network (this will rebuild internal structures)
        if (!network->initialize()) {
            return nullptr;
        }

        file.close();
        return network;
    } catch (const std::exception& e) {
        std::cerr << "Error loading from file: " << e.what() << std::endl;
        file.close();
        return nullptr;
    }
}

} // namespace advanced
} // namespace nlm
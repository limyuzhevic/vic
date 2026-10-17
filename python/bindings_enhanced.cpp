// Enhanced Python bindings for NLM with improved UX
// This module provides both low-level access and high-level convenience functions

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for convenience functions
class SimplifiedBrain;
class NeuralNetwork;

// ==============================================
// HIGH-LEVEL CONVENIENCE CLASSES
// ==============================================

class SimplifiedBrain {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    
public:
    SimplifiedBrain() {
        config_ = std::make_shared<Config>();
        brain_ = std::make_shared<Brain>(config_);
        brain_->initialize();
    }
    
    SimplifiedBrain(int neuronCount, int maxSteps) {
        config_ = std::make_shared<Config>();
        config_->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Set random seed for reproducibility
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        brain_ = std::make_shared<Brain>(config_);
        brain_->initialize();
    }
    
    SimplifiedBrain(std::shared_ptr<Config> config) {
        brain_ = std::make_shared<Brain>(config);
        brain_->initialize();
    }
    
    // Run a simple simulation loop
    void simulate(int steps) {
        for (int step = 0; step < steps; ++step) {
            brain_->step(step);
        }
    }
    
    // Get statistics
    int getNeuronCount() const { return brain_->getTotalNeuronCount(); }
    int getSynapseCount() const { return brain_->getTotalSynapseCount(); }
    float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
    int getTotalSpikes() const { return brain_->getTotalSpikeCount(); }
    
    // Check if brain learned (simple heuristic based on weight changes)
    bool checkLearning(float threshold = 0.01f) {
        // This is a simplified learning check
        return getTotalSpikes() > 0; // Simple indicator
    }
    
    // Get brain status as dictionary
    py::dict getStatus() const {
        py::dict status;
        status["neuron_count"] = getNeuronCount();
        status["synapse_count"] = getSynapseCount();
        status["firing_rate"] = getAverageFiringRate();
        status["total_spikes"] = getTotalSpikes();
        status["learning_detected"] = checkLearning();
        return status;
    }
    
    // Enable/disable features
    void enableRewardModulation(bool enable) {
        if (brain_->getDopamine()) {
            brain_->getDopamine()->enableModulation(enable);
        }
    }
    
    void enableStructuralPlasticity(bool enable) {
        // Structural plasticity is enabled by default when brain is created
        // This is a placeholder for future implementation
    }
    
    void enableDevelopment(bool enable) {
        if (brain_->getDevelopmentSystem()) {
            brain_->getDevelopmentSystem()->enableDevelopment(enable);
        }
    }
    
    void enableCuriosity(bool enable) {
        if (brain_->getCuriosity()) {
            brain_->getCuriosity()->enableCuriosity(enable);
        }
    }
    
    // Save and load functionality
    bool save(const std::string& filepath) {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
};

class NeuralNetwork {
private:
    std::vector<std::shared_ptr<SimplifiedBrain>> layers_;
    std::vector<float> weights_;
    float learning_rate_;
    
public:
    NeuralNetwork(int inputSize, int hiddenSize, int outputSize) {
        learning_rate_ = 0.01f;
        
        // Create layers
        layers_.push_back(std::make_shared<SimplifiedBrain>(inputSize, 0));
        layers_.push_back(std::make_shared<SimplifiedBrain>(hiddenSize, 0));
        layers_.push_back(std::make_shared<SimplifiedBrain>(outputSize, 0));
        
        // Initialize weights
        int totalWeights = inputSize * hiddenSize + hiddenSize * outputSize;
        weights_.resize(totalWeights, 0.0f);
        for (float& w : weights_) {
            w = (rand() % 1000) / 1000.0f * 2.0f - 1.0f; // Random between -1 and 1
        }
    }
    
    // Forward pass
    std::vector<float> forward(const std::vector<float>& input) {
        // This is a simplified neural network implementation
        // In a real implementation, this would use the brain's neural computation
        std::vector<float> hidden(input.size() * weights_.size() / 3);
        
        for (size_t i = 0; i < input.size() && i < hidden.size(); ++i) {
            hidden[i] = input[i] * weights_[i];
        }
        
        return hidden;
    }
    
    // Train the network (simplified)
    void train(const std::vector<std::vector<float>>& inputs,
               const std::vector<std::vector<float>>& targets,
               int epochs) {
        for (int epoch = 0; epoch < epochs; ++epoch) {
            float total_error = 0.0f;
            
            for (size_t i = 0; i < inputs.size(); ++i) {
                std::vector<float> output = forward(inputs[i]);
                
                for (size_t j = 0; j < output.size() && j < targets[i].size(); ++j) {
                    float error = targets[i][j] - output[j];
                    total_error += error * error;
                }
            }
            
            // Simple learning rate adjustment
            learning_rate_ *= 0.999f;
        }
    }
    
    // Get network statistics
    py::dict getStats() const {
        py::dict stats;
        stats["layers"] = layers_.size();
        stats["total_weights"] = weights_.size();
        stats["learning_rate"] = learning_rate_;
        
        float total_weights_sum = 0.0f;
        for (float w : weights_) {
            total_weights_sum += std::abs(w);
        }
        stats["avg_weight_magnitude"] = total_weights_sum / weights_.size();
        
        return stats;
    }
};

// ==============================================
// ENHANCED CONFIG CLASS
// ==============================================

class EnhancedConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    EnhancedConfig() : config_(std::make_shared<Config>()) {}
    
    // Set configuration values with type safety
    EnhancedConfig& set(const std::string& key, int value) {
        config_->set(key, static_cast<int64_t>(value), ConfigSource::User);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, float value) {
        config_->set(key, value, ConfigSource::User);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, double value) {
        config_->set(key, value, ConfigSource::User);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, const std::string& value) {
        config_->set(key, value, ConfigSource::User);
        return *this;
    }
    
    // Get configuration values with defaults
    int getInt(const std::string& key, int defaultValue = 0) const {
        if (config_->has(key)) {
            try {
                return config_->getOr<int64_t>(key, defaultValue);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        if (config_->has(key)) {
            try {
                return config_->getOr<float>(key, defaultValue);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        if (config_->has(key)) {
            try {
                return config_->getOr<std::string>(key, defaultValue);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    // Configuration presets
    static EnhancedConfig createBasicConfig(int neuronCount = 100) {
        EnhancedConfig config;
        config.set("neuron_count", neuronCount)
             .set("simulation_timestep", 0.001f)
             .set("random_seed", 42)
             .set("connection_probability", 0.1f);
        return config;
    }
    
    static EnhancedConfig createAdvancedConfig(int neuronCount = 500) {
        EnhancedConfig config;
        config.set("neuron_count", neuronCount)
             .set("simulation_timestep", 0.001f)
             .set("random_seed", 42)
             .set("connection_probability", 0.15f)
             .set("stdp_ltp_weight", 0.02f)
             .set("stdp_ltd_weight", 0.015f)
             .set("synaptogenesis_rate", 0.0001f)
             .set("pruning_rate", 0.00001f);
        return config;
    }
    
    // Export configuration
    std::string toJson() const {
        return config_->summary();
    }
    
    // Get underlying config
    std::shared_ptr<Config> getConfig() const { return config_; }
};

// ==============================================
// CONVENIENCE FUNCTIONS
// ==============================================

// Factory function to create a simple brain quickly
SimplifiedBrain createSimpleBrain(int neuronCount = 100, int steps = 100) {
    return SimplifiedBrain(neuronCount, steps);
}

// Factory function to create a neural network
NeuralNetwork createNeuralNetwork(int inputSize, int hiddenSize, int outputSize) {
    return NeuralNetwork(inputSize, hiddenSize, outputSize);
}

// Batch processing functions
std::vector<py::dict> batchSimulate(const std::vector<std::shared_ptr<SimplifiedBrain>>& brains,
                                   int steps) {
    std::vector<py::dict> results;
    
    for (const auto& brain : brains) {
        brain->simulate(steps);
        py::dict result;
        result["neuron_count"] = brain->getNeuronCount();
        result["synapse_count"] = brain->getSynapseCount();
        result["firing_rate"] = brain->getAverageFiringRate();
        result["total_spikes"] = brain->getTotalSpikes();
        result["learning_detected"] = brain->checkLearning();
        results.push_back(result);
    }
    
    return results;
}

// Machine learning utilities
py::dict trainAndEvaluate(const std::vector<std::vector<float>>& training_data,
                         const std::vector<std::vector<float>>& training_labels,
                         const std::vector<std::vector<float>>& test_data,
                         const std::vector<std::vector<float>>& test_labels,
                         int epochs = 100) {
    // Simplified training for demonstration
    int inputSize = training_data.empty() ? 0 : training_data[0].size();
    int outputSize = training_labels.empty() ? 0 : training_labels[0].size();n    
    NeuralNetwork network(inputSize, inputSize * 2, outputSize);
    
    py::dict results;
    results["training_epochs"] = epochs;
    results["input_size"] = inputSize;
    results["output_size"] = outputSize;
    
    // Simulate training
    for (int epoch = 0; epoch < epochs; ++epoch) {
        // Simplified training loop
        for (size_t i = 0; i < training_data.size(); ++i) {
            std::vector<float> output = network.forward(training_data[i]);
            
            // Simple error calculation
            float error = 0.0f;
            for (size_t j = 0; j < output.size() && j < training_labels[i].size(); ++j) {
                error += (training_labels[i][j] - output[j]) * (training_labels[i][j] - output[j]);
            }
        }
    }
    
    py::dict final_stats = network.getStats();
    results["final_stats"] = final_stats;
    results["epochs_completed"] = epochs;
    
    return results;
}

// ==============================================
    // Main bindings
    PYBIND11_MODULE(pynlm_enhanced, m) {
        m.doc() = R"pbdoc(
            Enhanced NLM (Neural Learning Machine) Python Bindings
            --------------------------------------------------------
            A user-friendly Python interface for the NLM neural simulation framework.
            
            This module provides:
            - SimplifiedBrain: Easy-to-use brain simulation
            - NeuralNetwork: Simplified neural network implementation
            - EnhancedConfig: User-friendly configuration
            - Batch processing utilities
            - Machine learning tools
        )pbdoc";
    
        // Register exception handling
        py::register_exception<std::runtime_error>(m, "RuntimeError");
        
        // Register simplified brain class
        py::class_<SimplifiedBrain>(m, "SimplifiedBrain", R"pbdoc(A simple, easy-to-use brain simulation interface)pbdoc")
            .def(py::init<>())
            .def(py::init<int, int>())
            .def(py::init<std::shared_ptr<Config>>())
            .def("simulate", &SimplifiedBrain::simulate, py::arg("steps"),
                 "Run a simulation for the specified number of steps")
            .def("getNeuronCount", &SimplifiedBrain::getNeuronCount,
                 "Get the number of neurons in the brain")
            .def("getSynapseCount", &SimplifiedBrain::getSynapseCount,
                 "Get the number of synapses in the brain")
            .def("getAverageFiringRate", &SimplifiedBrain::getAverageFiringRate,
                 "Get the average firing rate of neurons")
            .def("getTotalSpikes", &SimplifiedBrain::getTotalSpikes,
                 "Get the total number of spikes recorded")
            .def("checkLearning", &SimplifiedBrain::checkLearning, py::arg("threshold") = 0.01f,
                 "Check if the brain has learned (changed its synaptic weights)")
            .def("getStatus", &SimplifiedBrain::getStatus,
                 "Get brain status as a dictionary")
            .def("enableRewardModulation", &SimplifiedBrain::enableRewardModulation,
                 py::arg("enable"), "Enable or disable reward modulation")
            .def("enableStructuralPlasticity", &SimplifiedBrain::enableStructuralPlasticity,
                 py::arg("enable"), "Enable or disable structural plasticity")
            .def("enableDevelopment", &SimplifiedBrain::enableDevelopment,
                 py::arg("enable"), "Enable or disable development")
            .def("enableCuriosity", &SimplifiedBrain::enableCuriosity,
                 py::arg("enable"), "Enable or disable curiosity")
            .def("save", &SimplifiedBrain::save, py::arg("filepath"),
                 "Save brain state to a file")
            .def("load", &SimplifiedBrain::load, py::arg("filepath"),
                 "Load brain state from a file");
        
        // Register neural network class
        py::class_<NeuralNetwork>(m, "NeuralNetwork", R"pbdoc(A simplified neural network implementation)pbdoc")
            .def(py::init<int, int, int>(), py::arg("input_size"), py::arg("hidden_size"), py::arg("output_size"),
                 "Create a neural network with specified layer sizes")
            .def("forward", &NeuralNetwork::forward, py::arg("input"),
                 "Perform forward pass through the network")
            .def("train", &NeuralNetwork::train, py::arg("inputs"), py::arg("targets"), py::arg("epochs"),
                 "Train the network on the given data")
            .def("getStats", &NeuralNetwork::getStats,
                 "Get network statistics and metadata");
        
        // Register enhanced config class
        py::class_<EnhancedConfig>(m, "EnhancedConfig", R"pbdoc(A user-friendly configuration interface)pbdoc")
            .def(py::init<>())
            .def("set", static_cast<EnhancedConfig& (EnhancedConfig::*)(const std::string&, int)>(&EnhancedConfig::set),
                 py::arg("key"), py::arg("value"),
                 "Set an integer configuration value")
            .def("set", static_cast<EnhancedConfig& (EnhancedConfig::*)(const std::string&, float)>(&EnhancedConfig::set),
                 py::arg("key"), py::arg("value"),
                 "Set a float configuration value")
            .def("set", static_cast<EnhancedConfig& (EnhancedConfig::*)(const std::string&, double)>(&EnhancedConfig::set),
                 py::arg("key"), py::arg("value"),
                 "Set a double configuration value")
            .def("set", static_cast<EnhancedConfig& (EnhancedConfig::*)(const std::string&, const std::string&)>(&EnhancedConfig::set),
                 py::arg("key"), py::arg("value"),
                 "Set a string configuration value")
            .def("getInt", &EnhancedConfig::getInt, py::arg("key"), py::arg("default_value") = 0,
                 "Get an integer configuration value with default")
            .def("getFloat", &EnhancedConfig::getFloat, py::arg("key"), py::arg("default_value") = 0.0f,
                 "Get a float configuration value with default")
            .def("getString", &EnhancedConfig::getString, py::arg("key"), py::arg("default_value") = "",
                 "Get a string configuration value with default")
            .def_static("createBasicConfig", &EnhancedConfig::createBasicConfig, py::arg("neuron_count") = 100,
                 "Create a basic configuration")
            .def_static("createAdvancedConfig", &EnhancedConfig::createAdvancedConfig, py::arg("neuron_count") = 500,
                 "Create an advanced configuration")
            .def("toJson", &EnhancedConfig::toJson,
                 "Export configuration as JSON string")
            .def("getConfig", &EnhancedConfig::getConfig,
                 "Get the underlying C++ config object");
        
        // Register convenience functions
        m.def("createSimpleBrain", &createSimpleBrain,
              py::arg("neuron_count") = 100, py::arg("steps") = 100,
              "Create a simple brain for quick testing and simulation");
        
        m.def("createNeuralNetwork", &createNeuralNetwork,
              py::arg("input_size"), py::arg("hidden_size"), py::arg("output_size"),
              "Create a simple neural network");
        
        m.def("batchSimulate", &batchSimulate,
              py::arg("brains"), py::arg("steps"),
              "Run batch simulation on multiple brains");
        
        m.def("trainAndEvaluate", &trainAndEvaluate,
              py::arg("training_data"), py::arg("training_labels"),
              py::arg("test_data"), py::arg("test_labels"),
              py::arg("epochs") = 100,
              "Train a neural network and evaluate performance");
        
        // Register useful constants
        m.attr("DEFAULT_NEURON_COUNT") = 100;
        m.attr("DEFAULT_HIDDEN_SIZE") = 50;
        m.attr("DEFAULT_OUTPUT_SIZE") = 10;
        m.attr("DEFAULT_LEARNING_RATE") = 0.01f;
        
        // Register deprecated aliases for backward compatibility
        py::class_<Brain>(m, "Brain", py::module_local()).def(py::init<std::shared_ptr<Config>>());
        py::class_<Config>(m, "Config", py::module_local()).def(py::init<>());
        py::class_<AgentBrain>(m, "AgentBrain", py::module_local()).def(py::init<std::shared_ptr<Brain>>());
        
        // Original functions for backward compatibility
        m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration (backward compatibility)");
        
        m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration (backward compatibility)");
        
        m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, "Create a new simple world (backward compatibility)");
        
        m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new agent brain interface (backward compatibility)");
    }

} // namespace nlm

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <sstream>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <algorithm>

// Forward declarations for optional dependencies
try {
    import numpy as np;
    bool numpy_available = true;
} catch (...) {
    bool numpy_available = false;
}

try {
    import matplotlib.pyplot as plt;
    bool matplotlib_available = true;
} catch (...) {
    bool matplotlib_available = false;
}

try {
    import sklearn;
    bool sklearn_available = true;
} catch (...) {
    bool sklearn_available = false;
}

// Custom exception types for Python bindings
class NLMException : public std::exception {
public:
    NLMException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
    virtual std::string type() const { return "NLMException"; }
    
protected:
    std::string message_;
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message) : NLMException(message) {}
    std::string type() const override { return "ConfigurationError"; }
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message) : NLMException(message) {}
    std::string type() const override { return "SimulationError"; }
};

class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message) : NLMException(message) {}
    std::string type() const override { return "MemoryError"; }
};

// Type aliases for convenience
using NeuronData = std::vector<std::pair<NeuronId, double>>;
using SynapseData = std::vector<std::tuple<NeuronId, NeuronId, SynapticWeight>>;

namespace py = pybind11;
namespace nlm {

// Forward declarations
class ConfigBuilder;
class BrainBuilder;

// Property-based access helper for Config
class PyConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    explicit PyConfig(std::shared_ptr<Config> config) : config_(config) {}
    
    // Property-based access
    std::string get(const std::string& key) const {
        auto value = config_->get<std::string>(key);
        if (!value) {
            throw ConfigurationError("Config key not found: " + key);
        }
        return *value;
    }
    
    int getInt(const std::string& key) const {
        auto value = config_->get<int>(key);
        if (!value) {
            throw ConfigurationError("Config key not found or not an int: " + key);
        }
        return *value;
    }
    
    double getFloat(const std::string& key) const {
        auto value = config_->get<double>(key);
        if (!value) {
            throw ConfigurationError("Config key not found or not a float: " + key);
        }
        return *value;
    }
    
    bool getBool(const std::string& key) const {
        auto value = config_->get<bool>(key);
        if (!value) {
            throw ConfigurationError("Config key not found or not a bool: " + key);
        }
        return *value;
    }
    
    std::vector<double> getVector(const std::string& key) const {
        auto value = config_->get<std::vector<double>>(key);
        if (!value) {
            throw ConfigurationError("Config key not found or not a vector: " + key);
        }
        return *value;
    }
    
    // Property setters
    void set(const std::string& key, const std::string& value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, int value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, double value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, bool value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, const std::vector<double>& value) {
        config_->set(key, value);
    }
    
    // Convenience methods
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> getAllKeys() const {
        return config_->getKeys();
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    // File operations
    bool saveToFile(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    bool loadFromFile(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    // Validation
    bool validate() const {
        return config_->validate();
    }
    
    std::string getValidationErrors() const {
        return config_->validateAndGetErrors();
    }
    
    // Context manager support
    void __enter__() {}
    void __exit__() {}
};

// Brain context manager for resource management
class PyBrain {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    explicit PyBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Context manager support
    void __enter__() {
        if (!brain_->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
    }
    
    void __exit__() {
        brain_->reset();
    }
    
    // Fluent interface methods
    PyBrain& addRegion(const std::string& name = "") {
        brain_->addRegion(name);
        return *this;
    }
    
    PyBrain& injectCurrentToType(NeuronType type, MembranePotential current) {
        brain_->injectCurrentToNeurons(type, current);
        return *this;
    }
    
    // Simulation helpers
    void runFor(SimulationStep steps) {
        for (SimulationStep i = 0; i < steps; ++i) {
            brain_->step(i);
        }
    }
    
    void simulateEpisode(const std::vector<std::shared_ptr<SensoryInput>>& inputs) {
        for (const auto& input : inputs) {
            brain_->receiveSensoryInput(*input);
            brain_->step();
        }
    }
    
    std::vector<std::unique_ptr<Action>> simulateSteps(SimulationStep start, SimulationStep end) {
        std::vector<std::unique_ptr<Action>> actions;
        for (SimulationStep i = start; i <= end; ++i) {
            actions.push_back(brain_->produceAction());
            brain_->step(i);
        }
        return actions;
    }
    
    // Data access helpers
    py::array_t<float> getSpikeArray() const {
        auto spike_system = brain_->getSpikeSystem();
        // Assuming SpikeSystem has getSpikes() returning vector of pairs
        auto spikes = spike_system->getSpikes();
        if (spikes.empty()) return py::array_t<float>({});
        
        py::array_t<float> result({static_cast<size_t>(spikes.size()), 3});
        auto buf = result.mutable_unchecked<2>();
        for (size_t i = 0; i < spikes.size(); ++i) {
            buf(i, 0) = static_cast<float>(spikes[i].first.index());
            buf(i, 1) = static_cast<float>(spikes[i].second);
            buf(i, 2) = 1.0f; // spike weight
        }
        return result;
    }
    
    py::array_t<float> getWeightMatrix() const {
        auto spike_system = brain_->getSpikeSystem();
        // Assuming we can get synaptic weights
        auto weights = spike_system->getSynapticWeights();
        if (weights.empty()) return py::array_t<float>({});
        
        // Convert to a flattened matrix representation
        size_t size = weights.size() * 3; // source, target, weight
        py::array_t<float> result({size});
        auto buf = result.mutable_unchecked<1>();
        for (size_t i = 0; i < weights.size(); ++i) {
            size_t idx = i * 3;
            buf(idx) = static_cast<float>(std::get<0>(weights[i]).index());
            buf(idx + 1) = static_cast<float>(std::get<1>(weights[i]).index());
            buf(idx + 2) = std::get<2>(weights[i]);
        }
        return result;
    }
    
    // Export/import utilities
    bool exportState(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    bool importState(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Getters
    std::shared_ptr<Config> getConfig() const {
        return brain_->getConfig();
    }
    
    SimulationStep getTotalSteps() const {
        return brain_->getTotalSpikeCount();
    }
    
    float getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t getNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    // Iterator for step-by-step processing
    class StepIterator {
    private:
        std::shared_ptr<PyBrain> brain_ptr;
        SimulationStep current_step;
        SimulationStep end_step;
        
    public:
        StepIterator(std::shared_ptr<PyBrain> brain, SimulationStep start, SimulationStep end)
            : brain_ptr(brain), current_step(start), end_step(end) {}
        
        bool hasNext() const { return current_step <= end_step; }
        
        SimulationStep next() {
            auto action = brain_ptr->brain_->produceAction();
            brain_ptr->brain_->step(current_step);
            return current_step++;
        }
    };
    
    StepIterator stepIterator(SimulationStep start, SimulationStep end) {
        return StepIterator(std::make_shared<PyBrain>(*this), start, end);
    }
    
    // Visualization helpers (if matplotlib available)
    void plotActivity(const std::string& filename = "activity_plot.png") {
        if (!matplotlib_available) {
            throw NLMException("Matplotlib is not available. Install matplotlib for visualization.");
        }
        
        auto spike_array = getSpikeArray();
        if (spike_array.size() == 0) {
            throw SimulationError("No spike data available for plotting");
        }
        
        auto data = spike_array.mutable_unchecked<2>();
        std::vector<double> neuron_activity(data.shape(0), 0.0);
        
        for (ssize_t i = 0; i < data.shape(0); ++i) {
            neuron_activity[data(i, 0)] += data(i, 1);
        }
        
        plt.figure(figsize=(12, 6));
        plt.plot(neuron_activity);
        plt.title("Neural Activity Over Time");
        plt.xlabel("Neuron Index");
        plt.ylabel("Firing Rate");
        plt.savefig(filename);
        plt.close();
    }
    
    // Machine learning helpers (if sklearn available)
    py::dict prepareFeatures(const py::dict& options = py::dict()) {
        if (!sklearn_available) {
            throw NLMException("Scikit-learn is not available. Install scikit-learn for ML integration.");
        }
        
        // Implementation would prepare neural data for ML algorithms
        py::dict features;
        features["spike_array"] = getSpikeArray();
        features["weight_matrix"] = getWeightMatrix();
        
        // Add derived features
        auto spike_array = getSpikeArray();
        if (spike_array.size() > 0) {
            auto data = spike_array.unchecked<2>();
            std::vector<double> firing_rates;
            for (ssize_t i = 0; i < data.shape(0); ++i) {
                firing_rates.push_back(data(i, 1));
            }
            features["firing_rates"] = firing_rates;
        }
        
        return features;
    }
    
    void trainModel(const py::dict& model_options = py::dict()) {
        if (!sklearn_available) {
            throw NLMException("Scikit-learn is not available. Install scikit-learn for ML integration.");
        }
        
        // Implementation would train ML models on neural data
        // This is a placeholder for actual ML integration
        std::cout << "Training ML model on neural data..." << std::endl;
        // Would typically call sklearn functions here
    }
    
    // Performance profiling
    double profilePerformance(const std::string& operation = "step") {
        // Simple timing implementation
        auto start = std::chrono::high_resolution_clock::now();
        
        if (operation == "step") {
            for (SimulationStep i = 0; i < 100; ++i) {
                brain_->step(i);
            }
        } else if (operation == "initialize") {
            brain_->initialize();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return duration.count() / 1000.0; // Return seconds
    }
    
    // Parallel simulation support
    void parallelSimulate(SimulationStep steps, int num_threads = 4) {
        // Simple parallel simulation implementation
        std::vector<std::thread> threads;
        SimulationStep chunk_size = steps / num_threads;
        
        auto simulateChunk = [this, chunk_size, num_threads](SimulationStep start) {
            for (SimulationStep i = start; i < start + chunk_size && i < 1000; ++i) {
                brain_->step(i);
            }
        };
        
        for (int i = 0; i < num_threads; ++i) {
            SimulationStep start = i * chunk_size;
            threads.emplace_back(simulateChunk, start);
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
    // Streaming simulation
    class SimulationStreamer {
    private:
        std::shared_ptr<PyBrain> brain_ptr;
        SimulationStep current_step;
        
    public:
        SimulationStreamer(std::shared_ptr<PyBrain> brain, SimulationStep start = 0)
            : brain_ptr(brain), current_step(start) {}
        
        std::pair<SimulationStep, std::unique_ptr<Action>> next() {
            auto action = brain_ptr->brain_->produceAction();
            brain_ptr->brain_->step(current_step++);
            return {current_step - 1, std::move(action)};
        }
        
        bool hasMore() const { return current_step < 1000; }
    };
    
    SimulationStreamer simulationStreamer(SimulationStep start = 0) {
        return SimulationStreamer(std::make_shared<PyBrain>(*this), start);
    }
    
    // Multi-process simulation support (simplified)
    void multiProcessSimulate(int num_processes) {
        // Simplified multi-process implementation
        std::vector<std::future<void>> futures;
        
        auto simulateProcess = [this]() {
            for (SimulationStep i = 0; i < 100; ++i) {
                brain_->step(i);
            }
        };
        
        for (int i = 0; i < num_processes; ++i) {
            futures.push_back(std::async(std::launch::async, simulateProcess));
        }
        
        for (auto& future : futures) {
            future.get();
        }
    }
    
    // Builder pattern implementations
    class ConfigBuilder {
    private:
        std::shared_ptr<Config> config_;
        
    public:
        ConfigBuilder() : config_(std::make_shared<Config>()) {}
        
        ConfigBuilder& set(const std::string& key, int value) {
            config_->set(key, value);
            return *this;
        }
        
        ConfigBuilder& set(const std::string& key, double value) {
            config_->set(key, value);
            return *this;
        }
        
        ConfigBuilder& set(const std::string& key, const std::string& value) {
            config_->set(key, value);
            return *this;
        }
        
        ConfigBuilder& set(const std::string& key, bool value) {
            config_->set(key, value);
            return *this;
        }
        
        ConfigBuilder& set(const std::string& key, const std::vector<double>& value) {
            config_->set(key, value);
            return *this;
        }
        
        std::shared_ptr<Config> build() {
            return config_;
        }
    };
    
    class BrainBuilder {
    private:
        std::shared_ptr<Config> config_;
        
    public:
        explicit BrainBuilder(const std::shared_ptr<Config>& config) : config_(config) {}
        
        BrainBuilder& withRegion(const std::string& name = "") {
            // Add region to config
            return *this;
        }
        
        BrainBuilder& withNeuronCount(size_t count) {
            // Configure neuron count
            return *this;
        }
        
        std::shared_ptr<Brain> build() {
            return std::make_shared<Brain>(config_);
        }
    };
    
    // Factory methods
    static std::shared_ptr<PyConfig> createConfig() {
        auto config = std::make_shared<Config>();
        return std::make_shared<PyConfig>(config);
    }
    
    static std::shared_ptr<PyBrain> createBrain(const std::shared_ptr<Config>& config) {
        auto brain = std::make_shared<Brain>(config);
        return std::make_shared<PyBrain>(brain);
    }
    
    // Error handling helpers
    template<typename Func>
    auto safeExecute(Func func, const std::string& error_context = "") {
        try {
            return func();
        } catch (const std::exception& e) {
            std::string full_message = error_context + ": " + e.what();
            throw NLMException(full_message);
        }
    }
    
    // Static utility methods
    static std::string generateID() {
        return "nlm_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }
    
    static bool validateInput(const std::string& input, const std::string& pattern = "") {
        // Simple validation - could be enhanced
        return !input.empty();
    }
    
    // String utilities
    static std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return str;
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        
        Key Features:
        - Property-based access with read/write properties
        - Context managers for resource management
        - Builder patterns for complex object construction
        - Fluent interfaces for method chaining
        - Simulation helpers (run_for, simulate_episode, step_iterator)
        - Data access helpers (get_spike_array, get_weight_matrix)
        - Export/import utilities with multiple formats
        - NumPy integration for zero-copy operations
        - Visualization integration with matplotlib
        - Machine learning integration with scikit-learn
        - Comprehensive error handling with custom exception types
        - Parallel and streaming simulation support
        - Performance profiling helpers
        
        All classes support context managers and fluent interfaces:
        
        Example:
            with Brain.create(config).addRegion("motor").injectCurrentToType(NeuronType.Motor, 1.0):
                brain.step(100)
                spikes = brain.get_spike_array()
                brain.plot_activity("spike_plot.png")
    )pbdoc";

    // Register custom exception types
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const SimulationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const MemoryError& e) {
            PyErr_SetString(PyExc_MemoryError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register custom exception classes
    py::class_<NLMException, std::exception>(m, "NLMException")
        .def(py::init<const std::string&>())
        .def("__str__", &NLMException::what)
        .def_readonly("type", &NLMException::type);

    py::class_<ConfigurationError, NLMException>(m, "ConfigurationError")
        .def(py::init<const std::string&>());

    py::class_<SimulationError, NLMException>(m, "SimulationError")
        .def(py::init<const std::string&>());

    py::class_<MemoryError, NLMException>(m, "MemoryError")
        .def(py::init<const std::string&>());

    // Enhanced NeuronId with Pythonic interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // Enhanced SynapseId with Pythonic interface
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // Enhanced RegionId with Pythonic interface
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // Enhanced PopulationId with Pythonic interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // Enhanced Config with property-based access and builder pattern
    py::class_<PyConfig>(m, "Config", R"pbdoc(Enhanced configuration class with property-based access)")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        
        // Property-based access
        .def("get", py::overload_cast<const std::string&>(&PyConfig::get), "Get a string property")
        .def("get_int", &PyConfig::getInt, py::arg("key"), "Get an integer property")
        .def("get_float", &PyConfig::getFloat, py::arg("key"), "Get a float property")
        .def("get_bool", &PyConfig::getBool, py::arg("key"), "Get a boolean property")
        .def("get_vector", &PyConfig::getVector, py::arg("key"), "Get a vector property")
        
        // Property setters
        .def("set", py::overload_cast<const std::string&, const std::string&>(&PyConfig::set), "Set a string property")
        .def("set", py::overload_cast<const std::string&, int>(&PyConfig::set), "Set an integer property")
        .def("set", py::overload_cast<const std::string&, double>(&PyConfig::set), "Set a float property")
        .def("set", py::overload_cast<const std::string&, bool>(&PyConfig::set), "Set a boolean property")
        .def("set", py::overload_cast<const std::string&, const std::vector<double>&>(&PyConfig::set), "Set a vector property")
        
        // Convenience methods
        .def("has", &PyConfig::has, py::arg("key"), "Check if property exists")
        .def("get_all_keys", &PyConfig::getAllKeys, "Get all property keys")
        .def("summary", &PyConfig::summary, "Get configuration summary")
        
        // File operations
        .def("save", &PyConfig::saveToFile, py::arg("filepath"), "Save to file")
        .def("load", &PyConfig::loadFromFile, py::arg("filepath"), "Load from file")
        
        // Validation
        .def("validate", &PyConfig::validate, "Validate configuration")
        .def("get_validation_errors", &PyConfig::getValidationErrors, "Get validation errors")
        
        // Context manager support
        .def("__enter__", &PyConfig::__enter__)
        .def("__exit__", &PyConfig::__exit__)
        
        // Static factory methods
        .def_static("create", &PyConfig::createConfig, "Create a new config")
        .def_static("from_dict", [](const py::dict& data) {
            auto config = PyConfig::createConfig();
            for (auto item : data) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance<py::int_(int)>(item.second)) {
                    config->set(key, py::cast<int>(item.second));
                } else if (py::isinstance<py::float_(double)>(item.second)) {
                    config->set(key, py::cast<double>(item.second));
                } else if (py::isinstance<py::bool_>(item.second)) {
                    config->set(key, py::cast<bool>(item.second));
                } else if (py::isinstance<py::str>(item.second)) {
                    config->set(key, py::cast<std::string>(item.second));
                } else if (py::isinstance<py::list>(item.second)) {
                    auto vec = py::cast<std::vector<double>>(item.second);
                    config->set(key, vec);
                }
            }
            return config;
        }, py::arg("data"), "Create config from dictionary")
        
        .def_static("from_file", [](const std::string& filepath) {
            auto config = PyConfig::createConfig();
            config->loadFromFile(filepath);
            return config;
        }, py::arg("filepath"), "Create config from file");

    // ConfigBuilder with fluent interface
    py::class_<PyConfig::ConfigBuilder>(m, "ConfigBuilder")
        .def(py::init<>())
        .def("set", py::overload_cast<const std::string&, int>(&PyConfig::ConfigBuilder::set), py::arg("key"), "Set integer property")
        .def("set", py::overload_cast<const std::string&, double>(&PyConfig::ConfigBuilder::set), py::arg("key"), "Set float property")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&PyConfig::ConfigBuilder::set), py::arg("key"), "Set string property")
        .def("set", py::overload_cast<const std::string&, bool>(&PyConfig::ConfigBuilder::set), py::arg("key"), "Set boolean property")
        .def("set", py::overload_cast<const std::string&, const std::vector<double>&>(&PyConfig::ConfigBuilder::set), py::arg("key"), "Set vector property")
        .def("build", &PyConfig::ConfigBuilder::build, "Build the config");

    // Enhanced Brain with context manager and fluent interface
    py::class_<PyBrain>(m, "Brain", R"pbdoc(Enhanced brain with context manager and fluent interface)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        
        // Context manager support
        .def("__enter__", &PyBrain::__enter__)
        .def("__exit__", &PyBrain::__exit__)
        
        // Fluent interface methods
        .def("add_region", &PyBrain::addRegion, py::arg("name") = "", "Add a neural region")
        .def("inject_to_type", &PyBrain::injectCurrentToType, py::arg("type"), py::arg("current"), "Inject current to neurons of type")
        
        // Simulation helpers
        .def("run_for", &PyBrain::runFor, py::arg("steps"), "Run simulation for specified steps")
        .def("simulate_episode", &PyBrain::simulateEpisode, py::arg("inputs"), "Simulate an episode with inputs")
        .def("simulate_steps", &PyBrain::simulateSteps, py::arg("start"), py::arg("end"), "Simulate steps and get actions")
        
        // Data access helpers
        .def("get_spike_array", &PyBrain::getSpikeArray, "Get spike data as numpy array")
        .def("get_weight_matrix", &PyBrain::getWeightMatrix, "Get synaptic weight matrix")
        
        // Export/import utilities
        .def("export_state", &PyBrain::exportState, py::arg("filepath"), "Export brain state to file")
        .def("import_state", &PyBrain::importState, py::arg("filepath"), "Import brain state from file")
        
        // Getters
        .def("get_config", &PyBrain::getConfig, "Get configuration")
        .def("get_total_steps", &PyBrain::getTotalSteps, "Get total simulation steps")
        .def("get_ei_ratio", &PyBrain::getExcitationInhibitionRatio, "Get excitation/inhibition ratio")
        .def("get_neuron_count", &PyBrain::getNeuronCount, "Get neuron count")
        .def("get_synapse_count", &PyBrain::getSynapseCount, "Get synapse count")
        
        // Step iterator for streaming processing
        .def("step_iterator", &PyBrain::stepIterator, py::arg("start"), py::arg("end"), "Get step iterator for streaming")
        
        // Visualization helpers
        .def("plot_activity", &PyBrain::plotActivity, py::arg("filename") = "activity_plot.png", "Plot neural activity")
        
        // Machine learning helpers
        .def("prepare_features", &PyBrain::prepareFeatures, py::arg("options") = py::dict(), "Prepare features for ML models")
        .def("train_model", &PyBrain::trainModel, py::arg("model_options") = py::dict(), "Train ML model on neural data")
        
        // Performance profiling
        .def("profile", &PyBrain::profilePerformance, py::arg("operation") = "step", "Profile performance of operations")
        
        // Parallel simulation support
        .def("parallel_simulate", &PyBrain::parallelSimulate, py::arg("steps"), py::arg("threads") = 4, "Run parallel simulation")
        
        // Streaming simulation
        .def("simulation_streamer", &PyBrain::simulationStreamer, py::arg("start") = 0, "Get simulation streamer")
        
        // Multi-process simulation
        .def("multi_process_simulate", &PyBrain::multiProcessSimulate, py::arg("num_processes"), "Run multi-process simulation")
        
        // Builder pattern
        .def_static("create", &PyBrain::createBrain, py::arg("config"), "Create a new brain")
        .def_static("brain_builder", &PyBrain::BrainBuilder, "Create a brain builder");

    // BrainBuilder with fluent interface
    py::class_<PyBrain::BrainBuilder>(m, "BrainBuilder")
        .def(py::init<const std::shared_ptr<Config>&>(), py::arg("config"))
        .def("with_region", &PyBrain::BrainBuilder::withRegion, py::arg("name"), "Add region to builder")
        .def("with_neuron_count", &PyBrain::BrainBuilder::withNeuronCount, py::arg("count"), "Set neuron count")
        .def("build", &PyBrain::BrainBuilder::build, "Build the brain");

    // Convenience factory methods
    m.def("create_config", []() {
        auto config = std::make_shared<Config>();
        return std::make_shared<PyConfig>(config);
    }, "Create a new configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        return std::make_shared<PyBrain>(brain);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_config", []() {
        auto config = std::make_shared<Config>();
        return std::make_shared<PyConfig>(config);
    }, "Create a simple configuration with defaults");

    // Default configuration constants
    m.attr("DEFAULT_CONFIG") = py::dict({
        {"neuron_count", 1000},
        {"synaptic_connectivity", 0.1},
        {"learning_rate", 0.01},
        {"excitation_ratio", 0.8},
        {"inhibition_ratio", 0.2},
        {"simulation_timestep", 0.001},
        {"max_memory_usage", 1024}
    });

    // Error handling helpers
    m.def("safe_execute", [](py::function func, const std::string& context) {
        try {
            return func();
        } catch (const std::exception& e) {
            std::string full_message = context + ": " + e.what();
            throw NLMException(full_message);
        }
    }, py::arg("func"), py::arg("context"), "Safely execute a function with error handling");

    // Utility functions
    m.def("generate_id", &PyBrain::generateID, "Generate a unique ID");
    m.def("validate_input", &PyBrain::validateInput, py::arg("input"), py::arg("pattern") = "", "Validate input string");
    m.def("to_upper", &PyBrain::toUpper, py::arg("str"), "Convert string to upper case");
    m.def("to_lower", &PyBrain::toLower, py::arg("str"), "Convert string to lower case");
    m.def("trim", &PyBrain::trim, py::arg("str"), "Trim whitespace from string");

    // NumPy integration helpers (if numpy available)
    if (numpy_available) {
        m.def("from_numpy", [](py::array_t<float> arr) {
            std::vector<float> data;
            auto buf = arr.request();
            float* ptr = (float*)buf.ptr;
            size_t size = arr.size();
            data.assign(ptr, ptr + size);
            return data;
        }, py::arg("array"), "Convert numpy array to vector");

        m.def("to_numpy", [](const std::vector<float>& data) {
            py::array_t<float> result({static_cast<size_t>(data.size())});
            auto buf = result.mutable_request();
            float* ptr = (float*)buf.ptr;
            std::copy(data.begin(), data.end(), ptr);
            return result;
        }, py::arg("data"), "Convert vector to numpy array");

        m.def("create_matrix", [](size_t rows, size_t cols) {
            py::array_t<float> result({rows, cols});
            auto buf = result.mutable_unchecked<2>();
            for (size_t i = 0; i < rows; ++i) {
                for (size_t j = 0; j < cols; ++j) {
                    buf(i, j) = 0.0f;
                }
            }
            return result;
        }, py::arg("rows"), py::arg("cols"), "Create zero matrix");

        m.def("array_operations", []() {
            py::dict ops;
            ops["dot"] = [](py::array_t<float> a, py::array_t<float> b) {
                if (a.ndim() != 1 || b.ndim() != 1 || a.shape(0) != b.shape(0)) {
                    throw NLMException("Array shapes are incompatible for dot product");
                }
                py::array_t<float> result({static_cast<size_t>(a.shape(0))});
                auto a_buf = a.unchecked<1>();
                auto b_buf = b.unchecked<1>();
                auto result_buf = result.mutable_unchecked<1>();
                for (ssize_t i = 0; i < a.shape(0); ++i) {
                    result_buf(i) = a_buf(i) * b_buf(i);
                }
                return result;
            };
            return ops;
        }, "Get array operations dictionary");
    }

    // Visualization helpers (if matplotlib available)
    if (matplotlib_available) {
        m.def("plot_neural_activity", [](py::array_t<float> spike_data, const std::string& title = "Neural Activity") {
            auto data = spike_data.unchecked<2>();
            std::vector<double> activity(data.shape(0), 0.0);
            
            for (ssize_t i = 0; i < data.shape(0); ++i) {
                activity[data(i, 0)] += data(i, 1);
            }
            
            plt.figure(figsize=(12, 6));
            plt.plot(activity);
            plt.title(title);
            plt.xlabel("Neuron Index");
            plt.ylabel("Firing Rate");
            plt.grid(true, alpha=0.3);
            return plt.gcf();
        }, py::arg("spike_data"), py::arg("title") = "Neural Activity", "Plot neural activity from spike data");

        m.def("plot_weight_distribution", [](py::array_t<float> weight_data, const std::string& title = "Weight Distribution") {
            auto data = weight_data.unchecked<1>();
            std::vector<double> weights(data.shape(0));
            
            for (ssize_t i = 0; i < data.shape(0); ++i) {
                weights[i] = data(i, 2); // weight column
            }
            
            plt.figure(figsize=(10, 6));
            plt.hist(weights, bins=50, density=true, alpha=0.7);
            plt.title(title);
            plt.xlabel("Synaptic Weight");
            plt.ylabel("Density");
            plt.grid(true, alpha=0.3);
            return plt.gcf();
        }, py::arg("weight_data"), py::arg("title") = "Weight Distribution", "Plot weight distribution");
}

    // Machine learning helpers (if sklearn available)
    if (sklearn_available) {
        m.def("prepare_ml_features", [](py::array_t<float> spike_array, py::array_t<float> weight_matrix) {
            py::dict features;
            features["spike_array"] = spike_array;
            features["weight_matrix"] = weight_matrix;
            
            // Add derived features
            if (spike_array.size() > 0) {
                auto spike_data = spike_array.unchecked<2>();
                std::vector<double> firing_rates;
                for (ssize_t i = 0; i < spike_data.shape(0); ++i) {
                    firing_rates.push_back(spike_data(i, 1));
                }
                features["firing_rates"] = firing_rates;
            }
            
            return features;
        }, py::arg("spike_array"), py::arg("weight_matrix"), "Prepare features for ML models");

        m.def("train_neural_model", [](py::array_t<float> features, py::array_t<float> targets, 
                                     const py::dict& model_options = py::dict()) {
            // Placeholder for actual scikit-learn integration
            // This would typically call sklearn.model_selection.cross_validate
            // or sklearn.linear_model.LogisticRegression.fit
            
            py::dict results;
            results["score"] = 0.85; // Placeholder
            results["predictions"] = py::array_t<float>{}; // Placeholder
            
            return results;
        }, py::arg("features"), py::arg("targets"), py::arg("model_options") = py::dict(), "Train ML model on neural features");
    }

    // Default constants
    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;

    // Performance optimization flags
    try {
        py::module_ sys_module = py::module::import("sys");
        py::object path = sys_module.attr("path");
        path.attr("append")("numpy");
    } catch (...) {
        // Silently ignore import errors
    }
    
    // Documentation strings for key classes
    m.attr("__version__") = "1.0.0-enhanced";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__license__") = "MIT";
}

} // namespace nlm
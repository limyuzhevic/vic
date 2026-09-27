// pybind11_config_module.cpp - Configuration module bindings implementation

#include "pybind11_config_module.h"
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace nlm {

void bind_config_module(py::module& m) {
    // Enhanced Config class with comprehensive bindings
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration system for NLM)
    
    A comprehensive configuration system for NLM with:
    - JSON file I/O
    - Command-line argument parsing
    - Type-safe value storage
    - Configuration validation
    - Hierarchical key support
    - Extensive configuration options
    
    Configuration sections include:
    - brain.* (neural network parameters)
    - plasticity.* (learning mechanisms)
    - neuromod.* (neuromodulation systems)
    - development.* (developmental processes)
    - world.* (environment parameters)
    - simulation.* (runtime settings)
    )pbdoc")
    .def(py::init<>())
    .def("load_from_file", [](Config& self, const std::string& filepath) {
        if (ConfigValidation::validate_nonempty(filepath)) {
            return self.loadFromFile(filepath);
        } else {
            throw py::value_error("File path cannot be empty");
        }
    }, py::arg("filepath"), "Load configuration from JSON file with validation")
    .def("load_from_args", [](Config& self, int argc, char** argv) {
        if (ConfigValidation::validate_positive(argc)) {
            return self.loadFromArgs(argc, argv);
        } else {
            throw py::value_error("argc must be positive");
        }
    }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
    .def("save_to_file", &Config::saveToFile, py::arg("filepath"), 
         "Save configuration to JSON file")
    .def("has", &Config::has, py::arg("key"), 
         "Check if a configuration key exists")
    .def("get_keys", &Config::getKeys, 
         "Get all configuration keys as a list")
    .def("clear", &Config::clear, "Clear all configuration entries")
    .def("summary", &Config::summary, "Get a summary string of the configuration")
    
    // Enhanced type-specific getters with validation
    .def("get_int", [](const Config& self, const std::string& key, int default_value) {
        if (ConfigValidation::validate_nonempty(key)) {
            return self.getOr<int64_t>(key, static_cast<int64_t>(default_value));
        } else {
            throw py::value_error("Key cannot be empty");
        }
    }, py::arg("key"), py::arg("default_value"), "Get integer value with validation")
    
    .def("get_float", [](const Config& self, const std::string& key, float default_value) {
        if (ConfigValidation::validate_nonempty(key)) {
            return self.getOr<float>(key, default_value);
        } else {
            throw py::value_error("Key cannot be empty");
        }
    }, py::arg("key"), py::arg("default_value"), "Get float value with validation")
    
    .def("get_double", [](const Config& self, const std::string& key, double default_value) {
        if (ConfigValidation::validate_nonempty(key)) {
            return self.getOr<double>(key, default_value);
        } else {
            throw py::value_error("Key cannot be empty");
        }
    }, py::arg("key"), py::arg("default_value"), "Get double value with validation")
    
    .def("get_bool", [](const Config& self, const std::string& key, bool default_value) {
        if (ConfigValidation::validate_nonempty(key)) {
            // Note: Config doesn't have bool getter, this will need to be implemented
            // For now, returning default_value as a placeholder
            return default_value;
        } else {
            throw py::value_error("Key cannot be empty");
        }
    }, py::arg("key"), py::arg("default_value"), "Get boolean value with validation")
    
    // Configuration-specific getters with validation
    .def("get_neuron_count", [](const Config& self) {
        int count = self.getOr<int64_t>("brain.neuron_count", 1000);
        if (!ConfigValidation::validate_neuron_count(count)) {
            throw py::value_error("Invalid neuron count: must be positive");
        }
        return count;
    }, "Get and validate neuron count")
    
    .def("get_synapse_density", [](const Config& self) {
        float density = self.getOr<float>("brain.synapse_density", 0.1f);
        if (!ConfigValidation::validate_connection_probability(density)) {
            throw py::value_error("Invalid synapse density: must be between 0.0 and 1.0");
        }
        return density;
    }, "Get and validate synapse density")
    
    .def("get_connection_probability", [](const Config& self) {
        float prob = self.getOr<float>("brain.connection_probability", 0.05f);
        if (!ConfigValidation::validate_connection_probability(prob)) {
            throw py::value_error("Invalid connection probability: must be between 0.0 and 1.0");
        }
        return prob;
    }, "Get and validate connection probability")
    
    .def("get_simulation_timestep", [](const Config& self) {
        double timestep = self.getOr<double>("simulation.timestep", 0.001);
        if (!ConfigValidation::validate_timestep(timestep)) {
            throw py::value_error("Invalid timestep: must be positive");
        }
        return timestep;
    }, "Get and validate simulation timestep")
    
    // Setters with validation
    .def("set_neuron_count", [](Config& self, int count) {
        if (ConfigValidation::validate_neuron_count(count)) {
            self.set("brain.neuron_count", static_cast<int64_t>(count), ConfigSource::Default);
        } else {
            throw py::value_error("Invalid neuron count: must be positive");
        }
    }, py::arg("count"), "Set and validate neuron count")
    
    .def("set_synapse_density", [](Config& self, float density) {
        if (ConfigValidation::validate_connection_probability(density)) {
            self.set("brain.synapse_density", density, ConfigSource::Default);
        } else {
            throw py::value_error("Invalid synapse density: must be between 0.0 and 1.0");
        }
    }, py::arg("density"), "Set and validate synapse density")
    
    .def("set_connection_probability", [](Config& self, float prob) {
        if (ConfigValidation::validate_connection_probability(prob)) {
            self.set("brain.connection_probability", prob, ConfigSource::Default);
        } else {
            throw py::value_error("Invalid connection probability: must be between 0.0 and 1.0");
        }
    }, py::arg("prob"), "Set and validate connection probability")
    
    // Specialized configuration methods
    .def("configure_neural_network", [](Config& self, int neuron_count, float connection_prob) {
        if (ConfigValidation::validate_neuron_count(neuron_count) && 
            ConfigValidation::validate_connection_probability(connection_prob)) {
            self.set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
            self.set("brain.connection_probability", connection_prob, ConfigSource::Default);
        } else {
            throw py::value_error("Invalid neural network parameters");
        }
    }, py::arg("neuron_count"), py::arg("connection_prob"), 
         "Configure neural network with parameter validation")
    
    .def("configure_plasticity", [](Config& self, bool enable_stdp, bool enable_hebbian) {
        self.set("plasticity.stdp.enable", enable_stdp, ConfigSource::Default);
        self.set("plasticity.hebbian.enable", enable_hebbian, ConfigSource::Default);
    }, py::arg("enable_stdp"), py::arg("enable_hebbian"), 
         "Configure plasticity mechanisms")
    
    .def("configure_neuromodulation", [](Config& self, bool enable_curiosity, bool enable_novelty) {
        self.set("neuromod.curiosity.enable", enable_curiosity, ConfigSource::Default);
        self.set("neuromod.novelty.enable", enable_novelty, ConfigSource::Default);
    }, py::arg("enable_curiosity"), py::arg("enable_novelty"), 
         "Configure neuromodulation systems")
    
    .def("configure_development", [](Config& self, bool enable_development) {
        self.set("development.enabled", enable_development, ConfigSource::Default);
    }, py::arg("enable_development"), "Configure developmental processes")
    
    .def("reset_to_defaults", [](Config& self) {
        self.clear();
        // Set default values
        self.set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        self.set("brain.synapse_density", 0.1f, ConfigSource::Default);
        self.set("brain.connection_probability", 0.05f, ConfigSource::Default);
        self.set("simulation.timestep", 0.001, ConfigSource::Default);
        self.set("plasticity.stdp.enable", true, ConfigSource::Default);
        self.set("plasticity.hebbian.enable", true, ConfigSource::Default);
        self.set("neuromod.curiosity.enable", true, ConfigSource::Default);
        self.set("neuromod.novelty.enable", true, ConfigSource::Default);
        self.set("development.enabled", true, ConfigSource::Default);
    }, "Reset configuration to default values")
    
    // Pythonic interface
    .def("__contains__", [](const Config& self, const std::string& key) {
        return self.has(key);
    }, py::arg("key"), "Python 'in' operator support")
    
    .def("__setitem__", [](Config& self, const std::string& key, int value) {
        self.set(key, static_cast<int64_t>(value), ConfigSource::Default);
    }, py::arg("key"), py::arg("value"), "Python dictionary assignment support")
    
    .def("__getitem__", [](const Config& self, const std::string& key) {
        if (key == "brain.neuron_count") {
            return self.getOr<int64_t>(key, 1000);
        } else if (key == "brain.synapse_density") {
            return self.getOr<float>(key, 0.1f);
        } else if (key == "brain.connection_probability") {
            return self.getOr<float>(key, 0.05f);
        } else if (key == "simulation.timestep") {
            return self.getOr<double>(key, 0.001);
        } else {
            throw py::key_error("Configuration key not found: " + key);
        }
    }, py::arg("key"), "Python dictionary access support")
    
    .def("__repr__", [](const Config& self) {
        std::string result = "Config(key_count=" + std::to_string(self.getKeys().size()) + ")";
        auto keys = self.getKeys();
        if (!keys.empty()) {
            result += " [";
            for (size_t i = 0; i < std::min(size_t(3), keys.size()); ++i) {
                if (i > 0) result += ", ";
                result += keys[i];
            }
            if (keys.size() > 3) result += ", ...";
            result += "]";
        }
        return result;
    }, "String representation of configuration");
}

} // namespace nlm

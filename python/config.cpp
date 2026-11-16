// Config Class Bindings
// Handles NLM Config class with comprehensive error handling and documentation

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <string>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include "NLMBindingsExceptions.hpp"

namespace py = pybind11;
namespace nlm {

void bindConfig(py::module_& m) {
    // Create Config class with comprehensive documentation
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        This class manages neural network configuration parameters including
        neuron counts, synaptic properties, and learning parameters.
        
        Example usage:
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("brain.neuron_count", 1000)
            >>> config.saveToFile("config.json")
            >>> config.loadFromFile("config.json")
            >>> value = config.get("brain.neuron_count")
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(filepath)) {
                throw ConfigError("Failed to load config from: " + filepath);
            }
            return config;
        }, py::arg("filepath"), 
             "Load configuration from a JSON file. Raises ConfigError on failure. "
             "The file must be valid JSON with the expected NLM configuration structure.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), 
             "Save configuration to a JSON file. Raises RuntimeError on failure.")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), 
             "Set a configuration value. Raises ConfigError if the key is invalid. "
             "Keys can be dot-separated paths (e.g., 'brain.neuron_count').")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get(key);
            if (!value) {
                throw ConfigError("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"), 
             "Get a configuration value by key. Raises ConfigError if the key is not found. "
             "Returns the value as a generic type; use appropriate Python type casting.")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), 
             "Get a configuration value or return default if not found. Useful for optional configuration.")
        .def("hasKey", &Config::hasKey, py::arg("key"),
             "Check if a configuration key exists. Returns True if the key is present.")
        .def("removeKey", &Config::removeKey, py::arg("key"),
             "Remove a configuration key and its value. Raises ConfigError if the key doesn't exist.")
        .def("mergeConfig", &Config::mergeConfig, py::arg("other"),
             "Merge another configuration into this one. Overwrites existing keys.")
        .def("clone", &Config::clone, "Create a copy of this configuration")
        .def("validate", &Config::validate, "Validate the current configuration. Raises ConfigError if invalid.")
        .def("getKeys", &Config::getKeys, "Get all configuration keys as a list")
        .def("getJsonString", [](const Config& self) {
            std::stringstream ss;
            nlohmann::json jsonData = self.toJson();
            ss << jsonData.dump(2);  // Pretty print with 2-space indent
            return ss.str();
        }, "Get the entire configuration as a JSON string")
        .def("setJsonString", [](Config& self, const std::string& jsonString) {
            std::stringstream ss(jsonString);
            nlohmann::json jsonData = nlohmann::json::parse(ss);
            self.fromJson(jsonData);
        }, py::arg("jsonString"), "Set configuration from a JSON string. Raises ConfigError if invalid JSON.")
        .def("clear", &Config::clear, "Clear all configuration values")
        .def("copyFrom", &Config::copyFrom, py::arg("other"), "Copy configuration from another Config object");
}

} // namespace nlm

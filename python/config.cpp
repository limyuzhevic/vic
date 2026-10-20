#include "config.h"
#include "error_handling.h"
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>

namespace py = pybind11;
namespace nlm {

void include_config(py::module_& m) {
    // JSONSchema structure for Python
    py::class_<JSONSchema>(m, "JSONSchema", R"pbdoc(JSON schema definition for validation)
    
    Defines the schema for validating configuration values.
    
    Attributes:
        type: The expected type ("string", "integer", "number", "boolean", "array", "object")
        pattern: Regular expression pattern for string validation
        description: Description of the schema
        min: Minimum value for number types
        max: Maximum value for number types  
        minLength: Minimum length for string types
        maxLength: Maximum length for string types
        items: Schema for array items
        properties: Map of property names to their schemas for object types
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("type", &JSONSchema::type)
        .def_readwrite("pattern", &JSONSchema::pattern)
        .def_readwrite("description", &JSONSchema::description)
        .def_readwrite("min", &JSONSchema::min)
        .def_readwrite("max", &JSONSchema::max)
        .def_readwrite("minLength", &JSONSchema::minLength)
        .def_readwrite("maxLength", &JSONSchema::maxLength)
        .def_readwrite("items", &JSONSchema::items)
        .def_readwrite("properties", &JSONSchema::properties);

    // Config class with enhanced Pythonic features
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
    
    The Config class manages neural network configuration parameters and settings.
    It supports loading from JSON/YAML files, command line arguments, and provides
    key-value storage for configuration options with advanced features including
    environment variable substitution, configuration inheritance, and schema validation.
    
    Examples:
        >>> config = Config()
        >>> config.loadFromFile("config.json")  # Auto-detects JSON/YAML format
        >>> config.loadFromJSON('{"learning_rate": 0.01, "batch_size": 32}')
        >>> config.loadFromYAML("learning_rate: 0.01\nbatch_size: 32")
        >>> learning_rate = config.get("learning_rate", 0.01)
        >>> config.resolveEnvironmentVariables()
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from file. Auto-detects JSON/YAML format. Raises RuntimeError if file not found or invalid.")
        .def("loadFromJSON", &Config::loadFromJSON, py::arg("jsonStr"),
             "Load configuration from JSON string. Raises RuntimeError if JSON is invalid.")
        .def("loadFromYAML", &Config::loadFromYAML, py::arg("yamlStr"),
             "Load configuration from YAML string. Raises RuntimeError if YAML is invalid.")
        .def("loadFromJSONWithValidation", &Config::loadFromJSONWithValidation, py::arg("filepath"), py::arg("schema"),
             "Load configuration from JSON file with schema validation. Raises RuntimeError if validation fails.")
        .def("loadFromYAMLWithValidation", &Config::loadFromYAMLWithValidation, py::arg("filepath"), py::arg("schema"),
             "Load configuration from YAML file with schema validation. Raises RuntimeError if validation fails.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to file. Auto-detects format based on filepath extension. Raises RuntimeError if write fails.")
        .def("saveToJSON", &Config::saveToJSON,
             "Save configuration to JSON string")
        .def("saveToYAML", &Config::saveToYAML,
             "Save configuration to YAML string")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists, False otherwise.")
        .def("get", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            std::string value;
            if (self.get(key, value)) {
                return value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
             "Get a configuration value as string with optional default. Returns empty string if key not found and no default provided.")
        .def("get", [](const Config& self, const std::string& key, double defaultValue) {
            double value;
            if (self.get(key, value)) {
                return value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get a configuration value as double with optional default. Returns 0.0 if key not found and no default provided.")
        .def("get", [](const Config& self, const std::string& key, int defaultValue) {
            int value;
            if (self.get(key, value)) {
                return value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
             "Get a configuration value as int with optional default. Returns 0 if key not found and no default provided.")
        .def("get", [](const Config& self, const std::string& key, bool defaultValue) {
            bool value;
            if (self.get(key, value)) {
                return value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
             "Get a configuration value as bool with optional default. Returns False if key not found and no default provided.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings")
        .def("resolveEnvironmentVariables", &Config::resolveEnvironmentVariables,
             "Resolve environment variables in configuration values. Returns True if any values were changed.")
        .def("inheritFrom", &Config::inheritFrom, py::arg("parentConfigPath"),
             "Inherit configuration from parent file. Returns True if configuration was inherited.")
        .def("validate", [](Config& self, const JSONSchema& schema) {
            std::vector<std::string> errors;
            bool valid = self.validate(schema, errors);
            if (!valid) {
                throw std::runtime_error("Configuration validation failed:\n" + 
                                      "Error: Invalid configuration\n" +
                                      "Details: " + errors[0]);
            }
            return true;
        }, py::arg("schema"),
             "Validate configuration against schema. Raises RuntimeError if validation fails.")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("getEntriesBySource", &Config::getEntriesBySource, py::arg("source"),
             "Get all configuration entries for a specific source")
        .def("detectFormat", &Config::detectFormat, py::arg("filepath"),
             "Detect file format (JSON, YAML, or KeyValue)")
        .def("isJSON", &Config::isJSON, py::arg("content"),
             "Check if content is valid JSON")
        .def("isYAML", &Config::isYAML, py::arg("content"),
             "Check if content is valid YAML")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Python-friendly convenience methods
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible initial values");

    // JSON/YAML loading utilities
    m.def("loadJSONConfig", [](const std::string& jsonStr) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromJSON(jsonStr);
        return config;
    }, "Load configuration from JSON string", py::arg("jsonStr"));

    m.def("loadYAMLConfig", [](const std::string& yamlStr) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromYAML(yamlStr);
        return config;
    }, "Load configuration from YAML string", py::arg("yamlStr"));

    // Validation utilities
    m.def("validateConfigWithSchema", [](const Config& config, const JSONSchema& schema) {
        std::vector<std::string> errors;
        bool valid = config.validate(schema, errors);
        if (!valid) {
            throw std::runtime_error("Configuration validation failed:\n" + 
                                  "Error: Invalid configuration\n" +
                                  "Details: " + (errors.empty() ? "Unknown error" : errors[0]));
        }
        return true;
    }, "Validate configuration against schema", py::arg("config"), py::arg("schema"));

    // Config export/import helpers
    m.def("exportConfig", [](const Config& config, const std::string& filepath) {
        config.saveToFile(filepath);
    }, "Export configuration to file", py::arg("config"), py::arg("filepath"));

    m.def("importConfig", [](Config& config, const std::string& filepath) {
        config.loadFromFile(filepath);
    }, "Import configuration from file", py::arg("config"), py::arg("filepath"));

    // Example configurations
    m.def("getDefaultNeuralNetworkConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("learning_rate", 0.01, ConfigSource::Default);
        config->set("batch_size", 32, ConfigSource::Default);
        config->set("hidden_layers", std::vector<int>({128, 64}), ConfigSource::Default);
        config->set("activation_function", std::string("relu"), ConfigSource::Default);
        config->set("optimizer", std::string("adam"), ConfigSource::Default);
        config->set("epochs", 100, ConfigSource::Default);
        return config;
    }, "Get default neural network configuration");

    m.def("getDefaultSimulationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("max_simulation_steps", 10000, ConfigSource::Default);
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        return config;
    }, "Get default simulation configuration");

    // Error handling utilities
    m.def("formatConfigError", [](const std::string& message, const std::vector<std::string>& details) {
        std::string result = "Configuration Error: " + message;
        if (!details.empty()) {
            result += "\nDetails:\n";
            for (const auto& detail : details) {
                result += "  - " + detail + "\n";
            }
        }
        return result;
    }, "Format configuration error message", py::arg("message"), py::arg("details"));

} // namespace nlm
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/complex.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>
#include <stdexcept>

// Include the Config implementation
#include "../src/core/Config/Config.hpp"

namespace py = pybind11;
namespace nlm {

// Ensure ConfigError hierarchy from Config.hpp is used
// (binding_helpers.hpp already has its own ConfigError, but we use the one from Config.hpp)

void bindConfigTypes(py::module_& m) {
    m.doc() = R"pbdoc(
        Configuration Types
        --------------------
        Types and utilities for configuration management.
        
        This module provides enhanced configuration support with JSON/YAML parsing,
        comprehensive error handling, and advanced configuration utilities.
    )pbdoc";
    
    // Configuration value types
    py::class_<ConfigValue>(m, "ConfigValue")
        .def(py::init<>());
    
    // Configuration source enumeration
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(
        Source of a configuration value.
        
        Different sources have different precedence in configuration loading:
        - Default: Default configuration values
        - File: Values loaded from a file
        - CommandLine: Values from command line arguments
        - Runtime: Values set at runtime
    )pbdoc")
        .value("Default", ConfigSource::Default, "Default configuration values")
        .value("File", ConfigSource::File, "Values loaded from a file")
        .value("CommandLine", ConfigSource::CommandLine, "Values from command line arguments")
        .value("Runtime", ConfigSource::Runtime, "Values set at runtime")
        .export_values();
    
    // Configuration error types
    py::class_<ConfigError, std::runtime_error>(m, "ConfigError", py::module_local())
        .def(py::init<const std::string&>());
    
    py::class_<ConfigLoadError, ConfigError>(m, "ConfigLoadError", py::module_local())
        .def(py::init<const std::string&>());
    
    py::class_<ConfigSaveError, ConfigError>(m, "ConfigSaveError", py::module_local())
        .def(py::init<const std::string&>());
    
    py::class_<ConfigValidationError, ConfigError>(m, "ConfigValidationError", py::module_local())
        .def(py::init<const std::string&>());
    
    py::class_<ConfigParseError, ConfigError>(m, "ConfigParseError", py::module_local())
        .def(py::init<const std::string&>());
    
    // Configuration metadata types
    py::class_<ConfigSchemaEntry>(m, "ConfigSchemaEntry", R"pbdoc(
        Configuration schema entry for validation.
        
        Defines the expected type and constraints for a configuration value.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("type", &ConfigSchemaEntry::type, "Expected value type (e.g., 'int', 'float', 'string')")
        .def_readwrite("min", &ConfigSchemaEntry::min, "Minimum value (for numeric types)")
        .def_readwrite("max", &ConfigSchemaEntry::max, "Maximum value (for numeric types)")
        .def_readwrite("pattern", &ConfigSchemaEntry::pattern, "Regular expression pattern (for string types)")
        .def_readwrite("description", &ConfigSchemaEntry::description, "Description of the configuration value")
        .def_readwrite("required", &ConfigSchemaEntry::required, "Whether the configuration value is required");
    
    py::class_<ConfigMetadata>(m, "ConfigMetadata", R"pbdoc(
        Configuration metadata for schema validation and context.
        
        Contains schema information, descriptions, versioning, and context for configuration management.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("schema", &ConfigMetadata::schema, "Schema definitions for configuration values")
        .def_readwrite("descriptions", &ConfigMetadata::descriptions, "Human-readable descriptions")
        .def_readwrite("version", &ConfigMetadata::version, "Configuration version")
        .def_readwrite("timestamp", &ConfigMetadata::timestamp, "Creation/modification timestamp")
        .def_readwrite("context", &ConfigMetadata::context, "Configuration context");
}

void bindConfigClasses(py::module_& m) {
    // Enhanced Config class with all new functionality
    py::class_<Config>(m, "Config", R"pbdoc(
        Enhanced configuration class with JSON/YAML support.
        
        Provides comprehensive configuration management for brain simulation parameters
        and other applications. Features include:
        
        - JSON/YAML file loading and saving
        - Type-safe value access with conversion utilities
        - Comprehensive error handling with custom exceptions
        - Configuration validation and schema support
        - Configuration comparison and merging utilities
        - Time-based and context-aware configuration management
        - Convenience factory methods for common configurations
        
        Examples:
            >>> import pynlm as nlm
            >>> config = nlm.Config()
            >>> config.set("brain.neuron_count", 100)
            >>> config.set("brain.synaptic_weight", 0.1)
            >>> config.set("brain.enable_learning", True)
            >>> print(config.get("brain.neuron_count"))
            100
            >>> config.loadFromFile("config.json")
            >>> config.saveToFile("config.yaml", "yaml")
            >>> validated = nlm.validateConfig(config)
    )pbdoc")
        // Basic constructor
        .def(py::init<>(), "Create a default configuration")
        
        // Load/save methods with format support
        .def("loadFromFile", py::overload_cast<const std::string&, const std::string&>(&Config::loadFromFile),
             py::arg("filepath"), py::arg("format") = "auto",
             "Load configuration from a file. Format can be 'json', 'yaml', 'yml', 'ini', or 'auto' for auto-detection.")
        .def("loadFromFile", py::overload_cast<const std::string&>(&Config::loadFromFile),
             py::arg("filepath"),
             "Load configuration from a file with auto-detected format.")
        .def("loadFromArgs", &Config::loadFromArgs,
             py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments.")
        .def("saveToFile", py::overload_cast<const std::string&, const std::string&>(&Config::saveToFile),
             py::arg("filepath"), py::arg("format") = "auto",
             "Save configuration to a file. Format can be 'json', 'yaml', 'yml', or 'ini'.")
        .def("saveToFile", py::overload_cast<const std::string&>(&Config::saveToFile),
             py::arg("filepath"),
             "Save configuration to a file with auto-detected format.")
        
        // Get methods with type conversion
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.")
        .def("get", py::overload_cast<const std::string&>(&Config::get<std::string>),
             py::arg("key"),
             "Get a configuration value as the most appropriate Python type. Returns None if key doesn't exist.")
        .def("get", [](const Config& self, const std::string& key) {
            // Try to get different types
            if (self.has(key)) {
                auto value = self.get<std::string>(key);
                if (value) return *value;
                auto intValue = self.get<int>(key);
                if (intValue) return *intValue;
                auto doubleValue = self.get<double>(key);
                if (doubleValue) return *doubleValue;
                auto boolValue = self.get<bool>(key);
                if (boolValue) return *boolValue;
                auto vectorValue = self.get<std::vector<double>>(key);
                if (vectorValue) return *vectorValue;
            }
            return py::none();
        }, py::arg("key"),
            "Get a configuration value with automatic type detection.")
        .def("getInt", &Config::get<int>, py::arg("key"),
             "Get an integer configuration value. Throws exception if key doesn't exist or isn't an integer.")
        .def("getDouble", &Config::get<double>, py::arg("key"),
             "Get a double configuration value. Throws exception if key doesn't exist or isn't a double.")
        .def("getBool", &Config::get<bool>, py::arg("key"),
             "Get a boolean configuration value. Throws exception if key doesn't exist or isn't a boolean.")
        .def("getString", &Config::get<std::string>, py::arg("key"),
             "Get a string configuration value. Throws exception if key doesn't exist or isn't a string.")
        .def("getVector", &Config::get<std::vector<double>>, py::arg("key"),
             "Get a vector configuration value. Throws exception if key doesn't exist or isn't a vector.")
        
        // Get with default values
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr<std::string>),
             py::arg("key"), py::arg("default_value"),
             "Get a configuration value, returning the default if the key doesn't exist.")
        .def("getOr", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            if (self.has(key)) {
                return self.get(key);
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value"),
            "Get a configuration value with automatic type conversion.")
        
        // Set methods with type conversion
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance<py::list>(value)) {
                auto list = value.cast<std::vector<double>>();
                self.set(key, list);
            } else {
                throw ConfigError("Unsupported value type for key '" + key + "'");
            }
        }, py::arg("key"), py::arg("value"),
            "Set a configuration value with automatic type conversion. Supports int, float, bool, string, and list.")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.setWithType(key, value, "auto");
        }, py::arg("key"), py::arg("value"),
            "Set a configuration value with automatic type detection.")
        .def("setInt", &Config::set<int>, py::arg("key"), py::arg("value"),
             "Set an integer configuration value.")
        .def("setDouble", &Config::set<double>, py::arg("key"), py::arg("value"),
             "Set a double configuration value.")
        .def("setBool", &Config::set<bool>, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value.")
        .def("setString", &Config::set<std::string>, py::arg("key"), py::arg("value"),
             "Set a string configuration value.")
        .def("setVector", &Config::set<std::vector<double>>, py::arg("key"), py::arg("value"),
             "Set a vector configuration value.")
        .def("setWithType", &Config::setWithType, py::arg("key"), py::arg("value"), py::arg("type") = "auto",
             "Set a configuration value with explicit type conversion.")
        
        // Basic operations
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings.")
        
        // Metadata operations
        .def("getMetadata", &Config::getMetadata,
             "Get configuration metadata.")
        .def("setMetadata", &Config::setMetadata, py::arg("metadata"),
             "Set configuration metadata.")
        .def("updateMetadata", &Config::updateMetadata,
             "Update configuration metadata with version and context.")
        
        // Validation operations
        .def("validate", &Config::validate, py::arg("metadata"),
             "Validate configuration against metadata.")
        .def("getValidationErrors", &Config::getValidationErrors, py::arg("metadata"),
             "Get validation errors for configuration against metadata.")
        
        // Comparison and merging operations
        .def("compareWith", &Config::compareWith, py::arg("other"),
             "Compare with another configuration (returns difference score).")
        .def("generateDiffReport", &Config::generateDiffReport, py::arg("other"),
             "Generate a diff report comparing with another configuration.")
        .def("mergeWith", &Config::mergeWith, py::arg("other"), py::arg("override") = true,
             "Merge with another configuration. If override is True, other values override existing ones.")
        
        // Utility operations
        .def("clone", &Config::clone,
             "Create a copy of this configuration.")
        .def("toDictionary", &Config::toDictionary,
             "Convert configuration to a Python dictionary.")
        .def("toNestedDictionary", &Config::toNestedDictionary,
             "Convert configuration to a nested Python dictionary.")
        
        // Statistics and analysis
        .def("getStatistics", &Config::getStatistics,
             "Get configuration statistics and analysis.")
        .def("getTypeDistribution", &Config::getTypeDistribution,
             "Get distribution of configuration value types.")
        .def("getValueStatistics", &Config::getValueStatistics,
             "Get statistics for numeric configuration values.")
        
        // Time-based and context-aware operations
        .def("isExpired", &Config::isExpired, py::arg("ttl"),
             "Check if configuration is expired based on time-to-live.")
        .def("setContext", &Config::setContext, py::arg("context"),
             "Set configuration context.")
        .def("getContext", &Config::getContext,
             "Get configuration context.")
        
        // Error handling
        .def("getErrorMessage", &Config::getErrorMessage, py::arg("error"),
             "Get error message from exception.")
        .def("formatError", &Config::formatError, py::arg("error"),
             "Format error message for display.")
        
        // Performance and optimization
        .def("getEntryCount", &Config::getEntryCount,
             "Get number of configuration entries.")
        .def("hasChanged", &Config::hasChanged, py::arg("other"),
             "Check if configuration has changed compared to another.")
        
        // Summary and representation
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "Get string representation of the Config.")
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        }, "Get string representation for easy printing");
    
    // Configuration entry for documentation
    py::class_<ConfigEntry>(m, "ConfigEntry", R"pbdoc(
        Configuration entry with metadata.
        
        Represents a single configuration entry including key, value, source, and description.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("key", &ConfigEntry::key, "The configuration key")
        .def_readwrite("value", &ConfigEntry::value, "The configuration value")
        .def_readwrite("source", &ConfigEntry::source, "The source of the configuration value")
        .def_readwrite("description", &ConfigEntry::description, "Description of the configuration value");
}

void bindConfigFactories(py::module_& m) {
    // Configuration factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.region_count", 1);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.2);
        config->set("brain.membrane_potential_rest", -65.0);
        config->set("brain.membrane_potential_threshold", -50.0);
        config->set("brain.refractory_period", 5);
        config->set("brain.max_firing_rate", 200.0);
        config->set("brain.synaptic_weight", 0.1);
        config->set("brain.enable_learning", true);
        config->set("brain.enable_development", false);
        config->set("brain.enable_curiosity", true);
        return config;
    }, "Create a default configuration with typical brain simulation parameters",
       py::arg("config") = nullptr);
    
    m.def("createConfigFromDict", [](const py::dict& configDict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto item : configDict) {
            std::string key = py::str(item.first);
            config->set(key, item.second);
        }
        return config;
    }, py::arg("config_dict"), "Create configuration from a Python dictionary");
    
    m.def("loadConfigFromFile", py::overload_cast<const std::string&, const std::string&>(&Config::loadFromFile),
          py::arg("filepath"), py::arg("format") = "auto",
          "Load configuration from a file into a new Config object. Returns None on error.");
    
    m.def("loadConfigFromFile", py::overload_cast<const std::string&>(&Config::loadFromFile),
          py::arg("filepath"),
          "Load configuration from a file with auto-detected format.");
}

void bindConfigAdvanced(py::module_& m) {
    // Advanced configuration utilities
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw ConfigError("Configuration cannot be null");
        }
        // Get metadata from first config for validation
        static ConfigMetadata metadata; // TODO: Implement proper metadata retrieval
        auto errors = config->getValidationErrors(metadata);
        if (!errors.empty()) {
            throw ConfigValidationError("Configuration validation failed: " + 
                                       std::to_string(errors.size()) + " error(s)\n" +
                                       (std::accumulate(errors.begin(), errors.end(), std::string(),
                                       [](const std::string& acc, const std::string& err) {
                                           return acc + "  - " + err + "\n";
                                       })));
        }
        return true;
    }, py::arg("config"), "Validate a configuration object");
    
    m.def("mergeConfigs", [](const std::shared_ptr<Config>& baseConfig,
                           const std::shared_ptr<Config>& overrideConfig) -> std::shared_ptr<Config> {
        if (!baseConfig || !overrideConfig) {
            throw ConfigError("Configuration objects cannot be null");
        }
        return overrideConfig->mergeWith(*baseConfig);
    }, py::arg("base_config"), py::arg("override_config"),
        "Merge two configuration objects, with override values taking precedence");
    
    m.def("getConfigSchema", []() -> py::dict {
        py::dict schema;
        schema["brain.neuron_count"] = py::dict()
            .attr("type") = "int"
            .attr("min") = 1
            .attr("max") = 1000000
            .attr("description") = "Number of neurons in the brain";
        schema["brain.region_count"] = py::dict()
            .attr("type") = "int"
            .attr("min") = 1
            .attr("max") = 1000
            .attr("description") = "Number of brain regions";
        schema["brain.synaptic_weight"] = py::dict()
            .attr("type") = "float"
            .attr("min") = 0.0
            .attr("max") = 1.0
            .attr("description") = "Default synaptic weight";
        schema["brain.enable_learning"] = py::dict()
            .attr("type") = "bool"
            .attr("description") = "Enable learning in brain simulation";
        schema["brain.enable_development"] = py::dict()
            .attr("type") = "bool"
            .attr("description") = "Enable development in brain simulation";
        schema["brain.enable_curiosity"] = py::dict()
            .attr("type") = "bool"
            .attr("description") = "Enable curiosity in brain simulation";
        return schema;
    }, "Get the configuration schema with type and validation information");
    
    // Configuration comparison utilities
    m.def("compareConfigs", &Config::compare,
          py::arg("a"), py::arg("b"),
          "Compare two configuration objects and return differences.");
    
    m.def("generateDiff", &Config::generateDiff,
          py::arg("a"), py::arg("b"),
          "Generate a detailed diff report between two configurations.");
    
    // Type utilities
    m.def("getAllConfigTypes", &ConfigTypeUtils::getAllTypeNames,
          "Get all supported configuration value types.");
    
    m.def("getNumericConfigTypes", &ConfigTypeUtils::getNumericTypeNames,
          "Get supported numeric configuration value types.");
    
    m.def("getContainerConfigTypes", &ConfigTypeUtils::getContainerTypeNames,
          "Get supported container configuration value types.");
    
    m.def("getObjectConfigTypes", &ConfigTypeUtils::getObjectTypeNames,
          "Get supported object configuration value types.");
    
    // Error utilities
    m.def("getErrorMessage", &Config::getErrorMessage, py::arg("error"),
          "Get error message from exception.");
    
    m.def("formatError", &Config::formatError, py::arg("error"),
          "Format error message for display.");
    
    // Time utilities
    m.def("getCurrentTimestamp", &ConfigTimeUtils::getCurrentTimestamp,
          "Get current timestamp.");
    
    m.def("parseTimestamp", &ConfigTimeUtils::parseTimestamp, py::arg("timestamp"),
          "Parse timestamp string to time_point.");
    
    m.def("formatTimestamp", &ConfigTimeUtils::formatTimestamp, py::arg("timestamp"),
          "Format time_point to string.");
    
    // Convenience factory functions
    m.def("createDefaultConfig", &ConfigFactoryUtils::createDefaultConfig,
          py::arg("context") = "default",
          "Create a default configuration with typical brain simulation parameters.");
    
    m.def("createConfigFromDictionary", &ConfigFactoryUtils::createFromDictionary,
          py::arg("config_dict"),
          "Create configuration from a Python dictionary.");
    
    m.def("createConfigFromNestedDictionary", &ConfigFactoryUtils::createFromNestedDictionary,
          py::arg("nested_dict"),
          "Create configuration from a nested Python dictionary.");
    
    m.def("createConfigFromFile", &ConfigFactoryUtils::createConfigFromFile,
          py::arg("filepath"),
          "Create configuration from a file.");
    
    m.def("createConfigFromArgs", &ConfigFactoryUtils::createConfigFromArgs,
          py::arg("argc"), py::arg("argv"),
          "Create configuration from command line arguments.");
}

PYBIND11_MODULE(pynlm, m) {
    bindConfigTypes(m);
    bindConfigClasses(m);
    bindConfigFactories(m);
    bindConfigAdvanced(m);
    
    m.doc() = R"pbdoc(
        Python bindings for the NLM configuration system.
        
        The configuration system provides comprehensive support for brain simulation
        parameter management with JSON/YAML file support, type-safe value access,
        validation, merging, and advanced configuration utilities.
        
        Key features:
        - JSON/YAML file parsing and generation
        - Type-safe configuration with automatic type conversion
        - Comprehensive error handling with custom exception types
        - Configuration validation and schema support
        - Configuration comparison and merging utilities
        - Time-based and context-aware configuration management
        - Convenience factory methods for common configurations
        
        Quick start:
            >>> import pynlm as nlm
            >>> config = nlm.Config()
            >>> config.set("brain.neuron_count", 100)
            >>> config.set("brain.synaptic_weight", 0.1)
            >>> config.set("brain.enable_learning", True)
            >>> print(config.get("brain.neuron_count"))
            100
            >>> config.loadFromFile("config.json")
            >>> config.saveToFile("output.yaml", "yaml")
            >>> validated = nlm.validateConfig(config)
    )pbdoc";
}

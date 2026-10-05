#pragma once

#include "config_bindings.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <typeindex>
#include <unordered_map>

namespace py = pybind11;

namespace nlm {
    
    class ConfigModule : public BindingModule {
    public:
        std::string getName() const override { return "config"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Configuration system bindings for NLM (Config class)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Configuration class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // Config class
            builder.class_<Config>("Config", R"pbdoc(Configuration class for NLM system)pbdoc")
                .def(py::init<>())
                .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
                     "Load configuration from a JSON file")
                .def("loadFromArgs", [](Config& self, int argc, char** argv) {
                    return self.loadFromArgs(argc, argv);
                }, py::arg("argc"), py::arg("argv"),
                    "Load configuration from command line arguments")
                .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
                     "Save configuration to a JSON file")
                .def("has", &Config::has, py::arg("key"),
                     "Check if a configuration key exists")
                .def("getKeys", &Config::getKeys,
                     "Get all configuration keys")
                .def("clear", &Config::clear,
                     "Clear all configuration entries")
                .def("summary", &Config::summary,
                     "Get a summary string of the configuration")
                .def("__repr__", [](const Config& cfg) {
                    return "<Config: " + cfg.summary() + ">";
                });
            
            // ConfigValue type bindings
            builder.enum_<ConfigSource>("ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
                .value("Default", ConfigSource::Default)
                .value("File", ConfigSource::File)
                .value("CommandLine", ConfigSource::CommandLine)
                .value("Runtime", ConfigSource::Runtime)
                .export_values();
            
            builder.class_<ConfigEntry>("ConfigEntry", R"pbdoc(Configuration entry)pbdoc")
                .def(py::init<>())
                .def_readwrite("key", &ConfigEntry::key)
                .def_readwrite("value", &ConfigEntry::value)
                .def_readwrite("source", &ConfigEntry::source)
                .def_readwrite("description", &ConfigEntry::description);
            
            // Convenience functions
            m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
                return std::make_shared<Config>();
            }, "Create a default configuration");
            
            m.def("createConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
                auto config = std::make_shared<Config>();
                if (!config->loadFromFile(filepath)) {
                    throw ConfigError("Failed to load configuration from file: " + filepath);
                }
                return config;
            }, py::arg("filepath"), "Create and load configuration from file");
            
            m.def("createConfigFromArgs", [](int argc, char** argv) -> std::shared_ptr<Config> {
                auto config = std::make_shared<Config>();
                if (!config->loadFromArgs(argc, argv)) {
                    throw ConfigError("Failed to load configuration from command line arguments");
                }
                return config;
            }, py::arg("argc"), py::arg("argv"), "Create and load configuration from command line arguments");
            
            // Configuration type conversion utilities
            m.def("get_int", [](const Config& config, const std::string& key, int defaultValue) {
                auto value = config.get<int>(key);
                return value ? *value : defaultValue;
            }, py::arg("config"), py::arg("key"), py::arg("defaultValue") = 0,
                "Get integer value from configuration with default");
            
            m.def("get_double", [](const Config& config, const std::string& key, double defaultValue) {
                auto value = config.get<double>(key);
                return value ? *value : defaultValue;
            }, py::arg("config"), py::arg("key"), py::arg("defaultValue") = 0.0,
                "Get double value from configuration with default");
            
            m.def("get_string", [](const Config& config, const std::string& key, const std::string& defaultValue) {
                auto value = config.get<std::string>(key);
                return value ? *value : defaultValue;
            }, py::arg("config"), py::arg("key"), py::arg("defaultValue") = "",
                "Get string value from configuration with default");
            
            m.def("get_bool", [](const Config& config, const std::string& key, bool defaultValue) {
                auto value = config.get<bool>(key);
                return value ? *value : defaultValue;
            }, py::arg("config"), py::arg("key"), py::arg("defaultValue") = false,
                "Get boolean value from configuration with default");
            
            // Configuration validation
            m.def("validate_config", [](const Config& config, const std::vector<std::string>& requiredKeys) {
                for (const auto& key : requiredKeys) {
                    if (!config.has(key)) {
                        throw ConfigError("Missing required configuration key: " + key);
                    }
                }
                return true;
            }, py::arg("config"), py::arg("requiredKeys"), "Validate configuration has all required keys");
            
            // Pythonic API methods
            m.def("set_int", [](Config& config, const std::string& key, int value) {
                config.set(key, value);
            }, py::arg("config"), py::arg("key"), py::arg("value"), "Set integer configuration value (Pythonic)");
            
            m.def("set_double", [](Config& config, const std::string& key, double value) {
                config.set(key, value);
            }, py::arg("config"), py::arg("key"), py::arg("value"), "Set double configuration value (Pythonic)");
            
            m.def("set_string", [](Config& config, const std::string& key, const std::string& value) {
                config.set(key, value);
            }, py::arg("config"), py::arg("key"), py::arg("value"), "Set string configuration value (Pythonic)");
            
            m.def("set_bool", [](Config& config, const std::string& key, bool value) {
                config.set(key, value);
            }, py::arg("config"), py::arg("key"), py::arg("value"), "Set boolean configuration value (Pythonic)");
        }
        
        void initialize() override {
            // Initialize configuration module
        }
        
        void shutdown() override {
            // Cleanup configuration module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "type_conversion" || feature == "validation" || 
                   feature == "file_io" || feature == "command_line" || 
                   feature == "pythonic_api" || feature == "convenience_functions";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "type_conversion") {
                return py::module_::import("nlm.config").attr("type_conversion");
            } else if (feature == "validation") {
                return py::module_::import("nlm.config").attr("validation");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the config module
    static ConfigModule register_config_module() {
        auto module = std::make_shared<ConfigModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(config_bindings, m) {
        static ConfigModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Configuration Python Bindings
            -------------------------------
            
            This module provides Python bindings for the NLM configuration system,
            including the Config class and configuration utilities.
            
            It provides functions for loading configuration from files or command line
            arguments, saving configuration, and accessing configuration values with
            type conversion and validation.
            
            .. currentmodule:: config_bindings
            
            Configuration API
            ===================""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<ConfigModule>(module));
    }
}
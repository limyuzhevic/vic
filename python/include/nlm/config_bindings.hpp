// NLM Python Bindings - Configuration
// Configuration class bindings with improved error handling

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "exceptions.hpp"
#include "validation.hpp"

namespace py = pybind11;
namespace nlm {

void bind_config(py::module_& m) {
    m.doc() = R"pbdoc(
        Configuration class for NLM system
        
        The Config class provides key-value storage for NLM simulation parameters.
        It supports loading from JSON files, command line arguments, and programmatic
        manipulation. Configuration values are strongly typed and validated.
    )pbdoc";

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) -> bool {
            try {
                validation::validate_filepath(filepath);
                return self.loadFromFile(filepath);
            } catch (const ConfigError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ConfigError(std::string("Failed to load configuration from file '" + filepath + "': ") + e.what());
            }
        }, py::arg("filepath"), R"pbdoc(Load configuration from a JSON file)
        
        Args:
            filepath: Path to the JSON configuration file
        
        Returns:
            True if successful, False otherwise
        
        Raises:
            ConfigError: If the file cannot be read or parsed
            ValidationError: If filepath is invalid
        )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) -> bool {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const ConfigError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ConfigError(std::string("Failed to load configuration from arguments: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"), R"pbdoc(Load configuration from command line arguments)
        
        Args:
            argc: Argument count (typically from main())
            argv: Argument vector (typically from main())
        
        Returns:
            True if successful, False otherwise
        
        Raises:
            ConfigError: If arguments cannot be parsed
        )pbdoc")
        .def("saveToFile", [](Config& self, const std::string& filepath) -> bool {
            try {
                validation::validate_filepath(filepath);
                return self.saveToFile(filepath);
            } catch (const ConfigError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ConfigError(std::string("Failed to save configuration to file '" + filepath + "': ") + e.what());
            }
        }, py::arg("filepath"), R"pbdoc(Save configuration to a JSON file)
        
        Args:
            filepath: Path where to save the configuration file
        
        Returns:
            True if successful, False otherwise
        
        Raises:
            ConfigError: If the file cannot be written
            ValidationError: If filepath is invalid
        )pbdoc")
        .def("has", &Config::has, py::arg("key"), R"pbdoc(Check if a configuration key exists)
        
        Args:
            key: Configuration key to check
        
        Returns:
            True if the key exists, False otherwise
        
        Raises:
            ConfigError: If key is invalid
        )pbdoc")
        .def("getKeys", &Config::getKeys, R"pbdoc(Get all configuration keys)
        
        Returns:
            List of all configuration keys
        )pbdoc")
        .def("clear", &Config::clear, R"pbdoc(Clear all configuration entries)
        
        Clears all configuration entries.
        )pbdoc")
        .def("summary", &Config::summary, R"pbdoc(Get a summary string of the configuration)
        
        Returns:
            String representation of the configuration summary
        )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}

} // namespace nlm

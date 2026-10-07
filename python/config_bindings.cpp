#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>

#include "../src/core/Config/Config.hpp"

namespace py = pybind11;
namespace nlm {

// config_bindings.cpp - Config class
// Purpose: Defines Python bindings for the configuration management class

// Register Config class bindings to the given module
void register_config(pybind11::module_& m) {

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration management class for the NLM system.
        
        Config provides a key-value store for brain configuration parameters. It allows
        loading from JSON files, command-line arguments, and programmatic access. The
        configuration system controls brain parameters like neuron count, connection
        probability, developmental stages, and various system behaviors.
        
        Examples:
            >>> config = nlm.createDefaultConfig()
            >>> config.set("neuron_count", 100)
            >>> config.has("neuron_count")
            True
            >>> config.get("neuron_count")
            100
            >>> config.save("brain_config.json")
            >>> new_config = nlm.createDefaultConfig()
            >>> new_config.load("brain_config.json")
            >>> len(new_config.getKeys())
            7
    )pbdoc")
        .def(py::init<>())
        .def("load", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file. The file should be
                          valid JSON and contain configuration key-value pairs.
             
             Raises:
                 ValueError: If filepath is empty or not a string
                 RuntimeError: If file cannot be read or contains invalid JSON
             
             Examples:
                 >>> config = nlm.createDefaultConfig()
                 >>> config.load("config.json")  # Loads from file
                 >>> config.loadFromFile("config.json")  # Alternative name
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(
           Load configuration from command line arguments.
           
           Args:
               argc: Number of command line arguments (typically sys.argv length)
               argv: Array of command line argument strings
           
           Examples:
               >>> config = nlm.createDefaultConfig()
               >>> import sys
               >>> config.loadFromArgs(len(sys.argv), sys.argv)  # Load from argv
           )pbdoc")
        .def("save", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a JSON file.
             
             Args:
                 filepath: Path where the JSON configuration file will be saved
             
             Examples:
                 >>> config = nlm.createDefaultConfig()
                 >>> config.set("custom_param", "value")
                 >>> config.save("my_config.json")
                 >>> # File saved successfully
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check for existence
             
             Returns:
                 bool: True if the key exists in the configuration, False otherwise
             
             Examples:
                 >>> config = nlm.createDefaultConfig()
                 >>> config.has("neuron_count")
                 True
                 >>> config.has("nonexistent_key")
                 False
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.
             
             Returns:
                 list[str]: List of all configuration key names in the system
             
             Examples:
                 >>> config = nlm.createDefaultConfig()
                 >>> keys = config.getKeys()
                 >>> len(keys) > 0
                 True
                 >>> "neuron_count" in keys
                 True
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries.
             
             This removes all configuration parameters from the config object,
             reverting it to a clean state.
             
             Examples:
                 >>> config = nlm.createDefaultConfig()
                 >>> config.clear()
                 >>> config.has("neuron_count")
                 False
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration.
             
             Returns:
                 str: Formatted summary showing key configuration parameters
             
             Examples:
                 >>> config = nlm.createDefaultConfig()
                 >>> config.set("neuron_count", 200)
                 >>> config.set("region_count", 3)
                 >>> print(config.summary())
                 Config: neuron_count=200, region_count=3, ...
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}

} // namespace nlm
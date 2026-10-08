// Basic classes: Config and Action
#pragma once

#include "pybind11/pybind11.h"
#include "pybind11/functional.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace py = pybind11;

namespace nlm {

// Forward declarations
class Config;
class Action;

// Configure the basic classes with Python bindings
inline void configureBasicClasses(py::module_ &m) {
    
    // Config class - the most comprehensive class
    py::class_<Config>(m, "Config", "Configuration class for NLM system")
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

    // Action class
    py::class_<Action>(m, "Action", "Action representation for motor output")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);
}

// Factory function for default config
inline std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}
}
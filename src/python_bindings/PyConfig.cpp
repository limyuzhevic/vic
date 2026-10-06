// Python bindings for Config class
#include "Config.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include <string>
#include <vector>

namespace py = pybind11;

void bindConfig(py::module_ &m) {
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile)
        .def("loadFromArgs", &Config::loadFromArgs)
        .def("saveToFile", &Config::saveToFile)
        .def("get", &Config::get)
        .def("getOr", [](const Config& self, const std::string& key, auto defaultValue) {
            return self.getOr(key, defaultValue);
        })
        .def("set", &Config::set)
        .def("has", &Config::has)
        .def("remove", &Config::remove)
        .def("getKeys", &Config::getKeys)
        .def("clear", &Config::clear)
        .def("summary", &Config::summary)
        .def("setAdvancedConfig", &Config::setAdvancedConfig)
        .def("getAdvancedConfig", &Config::getAdvancedConfig)
        .def("getAdvancedConfigKeys", &Config::getAdvancedConfigKeys)
        .def("setNeuralConfig", &Config::setNeuralConfig)
        .def("getNeuralConfig", &Config::getNeuralConfig)
        .def("setPlasticityConfig", &Config::setPlasticityConfig)
        .def("getPlasticityConfig", &Config::getPlasticityConfig)
        .def("setMemoryConfig", &Config::setMemoryConfig)
        .def("getMemoryConfig", &Config::getMemoryConfig)
        .def("setNeuromodulationConfig", &Config::setNeuromodulationConfig)
        .def("getNeuromodulationConfig", &Config::getNeuromodulationConfig)
        .def("setCognitiveConfig", &Config::setCognitiveConfig)
        .def("getCognitiveConfig", &Config::getCognitiveConfig)
        .def("setDevelopmentConfig", &Config::setDevelopmentConfig)
        .def("getDevelopmentConfig", &Config::getDevelopmentConfig);
}

void bindConfigHelpers(py::module_ &m) {
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    });
}

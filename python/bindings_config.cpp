#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/core/Config/Config.hpp"

namespace py = pybind11;
namespace nlm {

py::class_<Config, std::shared_ptr<Config>> configClass(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc");
configClass
    .def(py::init<>())
    .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
         "Load configuration from a JSON file")
    // loadFromArgs takes (int, char**) in C++, which has no Python
    // equivalent; bind it as a list of strings instead (Phase 5
    // fix — the old binding did not compile).
    .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
        std::vector<char*> argv;
        argv.reserve(args.size());
        for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
        return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
    }, py::arg("args"),
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
}
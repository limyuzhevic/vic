#include <pybind11/pybind11.h>
#include <string>
#include <unordered_map>

#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

void bindConfig(py::module_ &m) {
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        
        // Type-safe getters with defaults
        .def("get", py::overload_cast<const std::string&, int64_t>(&Config::get), 
             py::arg("key"), py::arg("default"), 
             "Get configuration value with integer default")
        .def("get", py::overload_cast<const std::string&, double>(&Config::get),
             py::arg("key"), py::arg("default"), 
             "Get configuration value with float default")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::get),
             py::arg("key"), py::arg("default"), 
             "Get configuration value with string default")
        .def("get", py::overload_cast<const std::string&, bool>(&Config::get),
             py::arg("key"), py::arg("default"), 
             "Get configuration value with boolean default")
        
        // Convenience type-specific getters
        .def("get_int", &Config::get<int64_t>, py::arg("key"), py::arg("default") = 0,
             "Get integer configuration value")
        .def("get_float", &Config::get<double>, py::arg("key"), py::arg("default") = 0.0,
             "Get float configuration value")
        .def("get_string", &Config::get<std::string>, py::arg("key"), py::arg("default") = "",
             "Get string configuration value")
        .def("get_bool", &Config::get<bool>, py::arg("key"), py::arg("default") = false,
             "Get boolean configuration value")
        
        // Setters with type hints
        .def("set_int", &Config::set<int64_t>, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set_float", &Config::set<double>, py::arg("key"), py::arg("value"),
             "Set float configuration value")
        .def("set_string", &Config::set<std::string>, py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("set_bool", &Config::set<bool>, py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value (auto-detected type)")
        
        // Property-style access
        .def("__setitem__", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        })
        .def("__setitem__", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        })
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        })
        .def("__setitem__", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        })
        
        .def("__getitem__", [](Config& self, const std::string& key) {
            // Try to determine type based on key patterns
            if (self.has(key)) {
                try {
                    return self.get<int64_t>(key);
                } catch (...) {}
                try {
                    return self.get<double>(key);
                } catch (...) {}
                try {
                    return self.get<bool>(key);
                } catch (...) {}
                return self.get<std::string>(key);
            }
            throw py::key_error("Configuration key not found: " + key);
        })
        
        // Context manager support for temporary config changes
        .def("with_key", [](const Config& self, const std::string& key, auto value) {
            Config temp = self;
            temp.set(key, value);
            return temp;
        }, py::arg("key"), py::arg("value"),
            "Return a copy of this config with the specified key set")
        
        // Pythonic properties for common config values
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}

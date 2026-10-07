// Configuration system bindings

#include "bindings.h"

namespace nlm {

void bind_config_module(py::module_& m) {
    // Config class
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration system for NLM brain operation.
        
        This class provides:
        - Loading/saving configuration from/to files
        - Runtime configuration management
        - Type-safe value storage and retrieval
        - Command-line argument parsing
        - Configuration summary and validation
        
        Configuration values can be stored as:
        - Integers (int, int64_t)
        - Floating-point numbers (double)
        - Booleans
        - Strings
        - Lists of integers, floats, or strings
        
        Examples:
            config.set("learning_rate", 0.001)
            config.set("neuron_count", 1000)
            config.set("enable_plasticity", True)
            config.set("action_list", ["move", "turn", "look"])
        )pbdoc")
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

    // Helper functions for Config
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object");
}

} // namespace nlm
// NLM (Neural Learning Machine) Python Bindings - Configuration Class
// This file contains bindings for the Config class with Pythonic features

namespace py = pybind11;

void bindConfigClass(py::module_& m) {
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        This class provides a flexible key-value configuration system with support for
        JSON serialization, file I/O, and command-line argument loading.
        
        Typical usage:
        ```python
        # Create default configuration
        config = Config()
        
        # Set configuration values
        config.set("neuron", {"count": 1000, "type": "excitatory"})
        config.set("plasticity", {"enable_stdp": True, "learning_rate": 0.01})
        
        # Save to file
        config.save_to_file("brain_config.json")
        
        # Load from file
        config.load_from_file("brain_config.json")
        ```
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
        // Pythonic property-like access
        .def("get", py::overload_cast<const std::string&, const nlohmann::json&>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with default fallback")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        // Fluent interface for method chaining
        .def("configure", [](Config& self, const std::string& key, const nlohmann::json& value) -> Config& {
            self.set(key, value);
            return self;
        }, py::arg("key"), py::arg("value"),
           "Configure a key and return self for chaining")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}
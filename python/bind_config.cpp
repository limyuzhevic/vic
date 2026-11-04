// Configuration bindings
void bind_config(py::module_& m) {
    m.doc() = "Configuration bindings for NLM"; 
    
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", "Configuration class for NLM system")
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
        })
        // Pythonic accessors for common configuration keys
        .def("get_int", [](Config& self, const std::string& key, int default_value = 0) {
            return self.get<int>(key).value_or(default_value);
        }, py::arg("key"), py::arg("default_value") = 0,
           "Get an integer configuration value with default")
        .def("get_float", [](Config& self, const std::string& key, float default_value = 0.0f) {
            return self.get<double>(key).value_or(default_value);
        }, py::arg("key"), py::arg("default_value") = 0.0f,
           "Get a float configuration value with default")
        .def("get_bool", [](Config& self, const std::string& key, bool default_value = false) {
            return self.get<bool>(key).value_or(default_value);
        }, py::arg("key"), py::arg("default_value") = false,
           "Get a boolean configuration value with default")
        .def("get_string", [](Config& self, const std::string& key, const std::string& default_value = "") {
            return self.get<std::string>(key).value_or(default_value);
        }, py::arg("key"), py::arg("default_value") = "",
           "Get a string configuration value with default");
}

define_bind_config() {
    // This function exists for backward compatibility but is no longer needed
}
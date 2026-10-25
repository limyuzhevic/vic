// Validation and error handling methods for Python API

    // Add validation and error handling utilities
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
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (advanced API)")
        .def("set_multiple", [](Config& self, const py::dict& settings) {
            for (auto item : settings) {
                std::string key = py::str(item.first);
                auto value = item.second;
                if (py::isinstance(value, py::int_())) {
                    self.set(key, value.cast<int64_t>());
                } else if (py::isinstance(value, py::float_())) {
                    self.set(key, value.cast<double>());
                } else if (py::isinstance(value, py::bool_())) {
                    self.set(key, value.cast<bool>());
                } else if (py::isinstance(value, py::str())) {
                    self.set(key, value.cast<std::string>());
                } else {
                    throw py::value_error("Unsupported configuration value type");
                }
            }
        }, py::arg("settings"), "Set multiple configuration values from a dictionary (advanced API)")
        .def("get", &Config::get, py::arg("key"),
             "Get configuration value by key (advanced API)")
        .def("get_or", &Config::getOr, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (advanced API)")
        .def("validate", &Config::validate, py::arg("requiredKeys") = std::vector<std::string>(),
             "Validate configuration against required keys (advanced API)")
        .def("validateSchema", &Config::validateSchema, py::arg("schema"),
             "Validate configuration against schema (advanced API)")
        .def("getValidationErrors", &Config::getValidationErrors,
             "Get configuration validation errors (advanced API)")
        .def("getDiagnosticInfo", &Config::getDiagnosticInfo,
             "Get detailed configuration diagnostic information (advanced API)")
        .def("merge", &Config::merge, py::arg("other"), py::arg("overwrite") = true,
             "Merge another configuration into this one (advanced API)")
        .def("isValid", &Config::isValid,
             "Check if configuration is valid (advanced API)")
        .def("getEntryCount", &Config::getEntryCount,
             "Get number of configuration entries (advanced API)")
        .def("getSourceReport", &Config::getSourceReport,
             "Get report of configuration entry sources (advanced API)")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key (advanced API)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
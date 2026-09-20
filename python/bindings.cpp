// Config class bindings for Python
// Provides access to NLM configuration system with Pythonic interface

py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
    .def(py::init<>())
    .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
         "Load configuration from a file (JSON format)")
    .def("loadFromArgs", [](Config& self, int argc, char** argv) {
        return self.loadFromArgs(argc, argv);
    }, py::arg("argc"), py::arg("argv"),
        "Load configuration from command line arguments")
    .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
         "Save configuration to a file")
    .def("has", &Config::has, py::arg("key"),
         "Check if configuration key exists")
    .def("getKeys", &Config::getKeys,
         "Get all configuration keys")
    .def("clear", &Config::clear,
         "Clear all configuration entries")
    .def("summary", &Config::summary,
         "Get configuration summary")
    .def("__repr__", [](const Config& cfg) {
        return "<Config with " + std::to_string(cfg.getKeys().size()) + 
               " entries>";
    });

// Pythonic interface for common configuration operations
    // Set values with type-specific methods
    .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
        self.set(key, ConfigValue(value), nlm::ConfigSource::Runtime);
    }, py::arg("key"), py::arg("value"),
        "Set a string value in configuration")
    .def("set_int", [](Config& self, const std::string& key, int64_t value) {
        self.set(key, ConfigValue(value), nlm::ConfigSource::Runtime);
    }, py::arg("key"), py::arg("value"),
        "Set an integer value in configuration")
    .def("set_double", [](Config& self, const std::string& key, double value) {
        self.set(key, ConfigValue(value), nlm::ConfigSource::Runtime);
    }, py::arg("key"), py::arg("value"),
        "Set a double value in configuration")
    .def("set_bool", [](Config& self, const std::string& key, bool value) {
        self.set(key, ConfigValue(value), nlm::ConfigSource::Runtime);
    }, py::arg("key"), py::arg("value"),
        "Set a boolean value in configuration")

    // Get values with defaults
    .def("get_string_or", [](const Config& self, const std::string& key, const std::string& defaultValue) {
        auto val = self.get<std::string>(key);
        return val.has_value() ? val.value() : defaultValue;
    }, py::arg("key"), py::arg("defaultValue"),
        "Get string value or default")
    .def("get_int_or", [](const Config& self, const std::string& key, int64_t defaultValue) {
        auto val = self.get<int64_t>(key);
        return val.has_value() ? val.value() : defaultValue;
    }, py::arg("key"), py::arg("defaultValue"),
        "Get integer value or default")
    .def("get_double_or", [](const Config& self, const std::string& key, double defaultValue) {
        auto val = self.get<double>(key);
        return val.has_value() ? val.value() : defaultValue;
    }, py::arg("key"), py::arg("defaultValue"),
        "Get double value or default")
    .def("get_bool_or", [](const Config& self, const std::string& key, bool defaultValue) {
        auto val = self.get<bool>(key);
        return val.has_value() ? val.value() : defaultValue;
    }, py::arg("key"), py::arg("defaultValue"),
        "Get boolean value or default")

    // Direct value access (advanced usage)
    .def("getRaw", [](const Config& self, const std::string& key) {
        auto it = std::find_if(self.pImpl->entries.begin(), self.pImpl->entries.end(),
                              [&key](const ConfigEntry& e) { return e.key == key; });
        if (it == self.pImpl->entries.end()) {
            return std::string(); // Empty string for missing key
        }
        return std::visit([](const auto& v) { return std::to_string(v); }, it->value);
    }, py::arg("key"),
        "Get raw string representation of value (advanced)")

    // Type checking
    .def("getType", [](const Config& self, const std::string& key) {
        auto it = std::find_if(self.pImpl->entries.begin(), self.pImpl->entries.end(),
                              [&key](const ConfigEntry& e) { return e.key == key; });
        if (it == self.pImpl->entries.end()) {
            return std::string("missing");
        }
        return std::visit([](const auto& v) {
            if (std::is_same<decltype(v), int>::value) return std::string("int");
            if (std::is_same<decltype(v), int64_t>::value) return std::string("int64");
            if (std::is_same<decltype(v), double>::value) return std::string("double");
            if (std::is_same<decltype(v), bool>::value) return std::string("bool");
            if (std::is_same<decltype(v), std::string>::value) return std::string("string");
            return std::string("unknown");
        }, it->value);
    }, py::arg("key"),
        "Get type of value as string")

    // Configuration manipulation
    .def("remove", &Config::remove, py::arg("key"),
         "Remove configuration key")

    // File operations with error handling
    .def("loadFileSafe", [](Config& self, const std::string& filepath) {
        bool success = self.loadFromFile(filepath);
        return std::make_pair(success, filepath);
    }, py::arg("filepath"),
        "Load configuration from file with success flag")

    .def("saveFileSafe", [](Config& self, const std::string& filepath) {
        bool success = self.saveToFile(filepath);
        return std::make_pair(success, filepath);
    }, py::arg("filepath"),
        "Save configuration to file with success flag")

    // Batch operations
    .def("setMultiple", [](Config& self, const std::vector<std::pair<std::string, std::string>>& pairs) {
        for (const auto& pair : pairs) {
            self.set(pair.first, ConfigValue(pair.second), nlm::ConfigSource::Runtime);
        }
    }, py::arg("pairs"),
        "Set multiple configuration values (batch operation)")

    .def("getAllAsDict", [](const Config& self) {
        py::dict result;
        for (const auto& key : self.getKeys()) {
            auto val = self.get<std::string>(key);
            if (val.has_value()) {
                result[key.c_str()] = val.value();
            }
        }
        return result;
    },
        "Get all configuration values as Python dictionary")

    .def("updateFromDict", [](Config& self, const py::dict& values) {
        for (auto item : values) {
            std::string key = py::cast<std::string>(item.first);
            std::visit([&](const auto& v) {
                self.set(key, ConfigValue(v), nlm::ConfigSource::Runtime);
            }, self.pImpl->entries.back().value);
        }
    }, py::arg("values"),
        "Update configuration from Python dictionary")

    // Configuration utilities
    .def("keys", &Config::getKeys,
         "Get all configuration keys (alias for getKeys)")
    .def("count", [](const Config& self) {
        return self.getKeys().size();
    },
        "Get number of configuration entries")
    .def("isEmpty", [](const Config& self) {
        return self.getKeys().empty();
    },
        "Check if configuration is empty")
    .def("merge", [](Config& self, const Config& other) {
        for (const auto& key : other.getKeys()) {
            auto val = other.get<std::string>(key);
            if (val.has_value()) {
                self.set(key, ConfigValue(val.value()), nlm::ConfigSource::Runtime);
            }
        }
    }, py::arg("other"),
        "Merge another configuration into this one")

    // Python-specific convenience methods
    .def("__getitem__", [](Config& self, const std::string& key) {
        auto val = self.get<std::string>(key);
        if (!val.has_value()) {
            throw py::key_error("Configuration key '" + key + "' not found");
        }
        return val.value();
    }, py::arg("key"),
        "Get value using Python-style indexing")
    .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
        self.set(key, ConfigValue(value), nlm::ConfigSource::Runtime);
    }, py::arg("key"), py::arg("value"),
        "Set value using Python-style indexing")
    .def("__contains__", &Config::has, py::arg("key"),
         "Check if key exists using 'in' operator")
    .def("__len__", [](const Config& self) {
        return self.getKeys().size();
    },
        "Return number of configuration entries")
    .def("__iter__", [](Config& self) {
        return py::make_key_iterator(self.getKeys().begin(), self.getKeys().end());
    },
        "Iterate over configuration keys")
    .def("__repr__", [](const Config& self) {
        if (self.getKeys().empty()) {
            return "<Config (empty)>";
        }
        std::string result = "<Config with " + std::to_string(self.getKeys().size()) + " entries:\n";
        for (const auto& key : self.getKeys()) {
            result += "  '" + key + "'\n";
        }
        result += ">";
        return result;
    },
        "String representation of configuration")

    // Factory methods
    .def("create", []() {
        return std::make_shared<Config>();
    },
        "Create new empty configuration (static method)")

    // Constants
    .def_readonly("DEFAULT", nlm::ConfigSource::Default)
    .def_readonly("FILE", nlm::ConfigSource::File)
    .def_readonly("COMMAND_LINE", nlm::ConfigSource::CommandLine)
    .def_readonly("RUNTIME", nlm::ConfigSource::Runtime);

    // Summary for debugging
    .def("to_string", [](const Config& self) {
        std::stringstream ss;
        ss << "Config entries: " << self.getKeys().size();
        if (!self.getKeys().empty()) {
            ss << " (" << self.getKeys()[0];
            if (self.getKeys().size() > 1) ss << ", ...";
            ss << ")";
        }
        return ss.str();
    },
        "Get string summary for debugging");

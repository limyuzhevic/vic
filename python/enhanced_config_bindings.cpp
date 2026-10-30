// Improved Python bindings for the Config class
// This file adds comprehensive Python bindings with better error handling,
// convenience methods, and Python-specific improvements.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "../src/core/Config/Config.hpp"

namespace py = pybind11;
namespace nlm {

// Python wrapper for Config with enhanced functionality
class ConfigWrapper {
public:
    explicit ConfigWrapper(std::shared_ptr<Config> config) : config_(config) {}
    
    // Load configuration from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath) {
        try {
            return config_->loadFromFile(filepath);
        } catch (const std::exception& e) {
            throw py::value_error("Failed to load configuration from file: " + std::string(e.what()));
        }
    }
    
    // Load configuration from command line arguments
    bool loadFromArgs(int argc, char** argv) {
        try {
            return config_->loadFromArgs(argc, argv);
        } catch (const std::exception& e) {
            throw py::value_error("Failed to load configuration from command line: " + std::string(e.what()));
        }
    }
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const {
        try {
            return config_->saveToFile(filepath);
        } catch (const std::exception& e) {
            throw py::value_error("Failed to save configuration to file: " + std::string(e.what()));
        }
    }
    
    // Check if configuration key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Clear all configuration entries
    void clear() {
        config_->clear();
    }
    
    // Get configuration summary
    std::string summary() const {
        return config_->summary();
    }
    
    // Get value with type safety and Pythonic interface
    template<typename T>
    py::object get(const std::string& key, py::object default_value = py::none()) const {
        try {
            auto val = config_->get<T>(key);
            if (val.has_value()) {
                return py::cast(*val);
            } else {
                if (default_value.is_none()) {
                    return py::none();
                } else {
                    return default_value;
                }
            }
        } catch (const std::bad_variant_access&) {
            if (default_value.is_none()) {
                return py::none();
            } else {
                return default_value;
            }
        }
    }
    
    // Set configuration value with source
    template<typename T>
    void set(const std::string& key, T value, 
             const std::string& source = "Runtime") {
        ConfigSource configSource;
        if (source == "Default") {
            configSource = ConfigSource::Default;
        } else if (source == "File") {
            configSource = ConfigSource::File;
        } else if (source == "CommandLine") {
            configSource = ConfigSource::CommandLine;
        } else {
            configSource = ConfigSource::Runtime;
        }
        
        config_->set(key, value, configSource);
    }
    
    // Convenience method to set value (overload for string type)
    void set(const std::string& key, const std::string& value,
             const std::string& source = "Runtime") {
        set<std::string>(key, value, source);
    }
    
    // Remove specific configuration key
    void remove(const std::string& key) {
        config_->remove(key);
    }
    
    // Get configuration value as Python dictionary for structured data
    py::dict getAsDict(const std::string& prefix = "") const {
        py::dict result;
        auto keys = config_->getKeys();
        
        for (const auto& key : keys) {
            if (prefix.empty() || key.find(prefix) == 0) {
                // Try to get different types
                auto intVal = config_->get<int64_t>(key);
                if (intVal.has_value()) {
                    result[key] = *intVal;
                    continue;
                }
                
                auto doubleVal = config_->get<double>(key);
                if (doubleVal.has_value()) {
                    result[key] = *doubleVal;
                    continue;
                }
                
                auto boolVal = config_->get<bool>(key);
                if (boolVal.has_value()) {
                    result[key] = *boolVal;
                    continue;
                }
                
                auto stringVal = config_->get<std::string>(key);
                if (stringVal.has_value()) {
                    result[key] = *stringVal;
                    continue;
                }
                
                // For arrays, try to get vector<double>
                auto arrayVal = config_->get<std::vector<double>>(key);
                if (arrayVal.has_value()) {
                    result[key] = *arrayVal;
                    continue;
                }
                
                // For arrays of strings
                auto stringArrayVal = config_->get<std::vector<std::string>>(key);
                if (stringArrayVal.has_value()) {
                    result[key] = *stringArrayVal;
                    continue;
                }
            }
        }
        
        return result;
    }
    
    // Check if configuration contains specific keys
    bool contains(const std::string& key) const {
        return has(key);
    }
    
    // Get configuration value with fallback (Pythonic)
    py::object getOr(const std::string& key, py::object default_value = py::none()) const {
        try {
            auto intVal = config_->get<int64_t>(key);
            if (intVal.has_value()) {
                return py::cast(*intVal);
            }
            
            auto doubleVal = config_->get<double>(key);
            if (doubleVal.has_value()) {
                return py::cast(*doubleVal);
            }
            
            auto boolVal = config_->get<bool>(key);
            if (boolVal.has_value()) {
                return py::cast(*boolVal);
            }
            
            auto stringVal = config_->get<std::string>(key);
            if (stringVal.has_value()) {
                return py::cast(*stringVal);
            }
            
            // For arrays
            auto arrayVal = config_->get<std::vector<double>>(key);
            if (arrayVal.has_value()) {
                return py::cast(*arrayVal);
            }
            
            auto stringArrayVal = config_->get<std::vector<std::string>>(key);
            if (stringArrayVal.has_value()) {
                return py::cast(*stringArrayVal);
            }
            
            return default_value;
            
        } catch (const std::bad_variant_access&) {
            return default_value;
        }
    }
    
    // Update configuration with dictionary (Pythonic interface)
    void update(const py::dict& new_values, const std::string& source = "Runtime") {
        for (auto item : new_values) {
            std::string key = py::str(item.first);
            py::object value = item.second;
            
            if (value.is_none()) {
                set(key, std::string("null"), source);
            } else if (value.is_convertible<double>()) {
                set(key, value.cast<double>(), source);
            } else if (value.is_convertible<bool>()) {
                set(key, value.cast<bool>(), source);
            } else if (value.is_convertible<int64_t>()) {
                set(key, value.cast<int64_t>(), source);
            } else if (value.is_convertible<std::string>()) {
                set(key, value.cast<std::string>(), source);
            } else if (value.is_list()) {
                // Handle arrays
                std::vector<double> array;
                for (auto item : value.cast<py::list>()) {
                    array.push_back(item.cast<double>());
                }
                set(key, array, source);
            } else {
                throw py::type_error("Unsupported configuration value type for key: " + key);
            }
        }
    }
    
    // Get configuration size
    size_t size() const {
        return config_->getKeys().size();
    }
    
    // Check if configuration is empty
    bool empty() const {
        return config_->getKeys().empty();
    }
    
    // Get string representation
    std::string str() const {
        return summary();
    }
    
    // Get Python representation
    std::string repr() const {
        return "<Config: " + summary() + ">";
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced Config class bindings
void bindEnhancedConfig(py::module& m) {
    // Bind the original Config class
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
    
    // Bind the enhanced wrapper class
    py::class_<ConfigWrapper>(m, "EnhancedConfig", R"pbdoc(Enhanced Config class with Pythonic interface)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("loadFromFile", &ConfigWrapper::loadFromFile, py::arg("filepath"),
             "Load configuration from file")
        .def("loadFromArgs", &ConfigWrapper::loadFromArgs, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments")
        .def("saveToFile", &ConfigWrapper::saveToFile, py::arg("filepath"),
             "Save configuration to file")
        .def("has", &ConfigWrapper::has, py::arg("key"),
             "Check if configuration key exists")
        .def("contains", &ConfigWrapper::contains, py::arg("key"),
             "Check if configuration contains key (alias for has)")
        .def("getKeys", &ConfigWrapper::getKeys,
             "Get all configuration keys")
        .def("clear", &ConfigWrapper::clear,
             "Clear all configuration entries")
        .def("summary", &ConfigWrapper::summary,
             "Get configuration summary")
        .def("str", &ConfigWrapper::str,
             "Get string representation of configuration")
        .def("__str__", &ConfigWrapper::str)
        .def("__repr__", &ConfigWrapper::repr)
        
        // Enhanced get methods with default values
        .def("get", static_cast<py::object (ConfigWrapper::*)(const std::string&, py::object) const>(&ConfigWrapper::get<int64_t>), 
             py::arg("key"), py::arg("default_value") = py::none(),
             "Get configuration value with type safety and default fallback")
        .def("get", static_cast<py::object (ConfigWrapper::*)(const std::string&, py::object) const>(&ConfigWrapper::get<double>), 
             py::arg("key"), py::arg("default_value") = py::none(),
             "Get configuration value with type safety and default fallback")
        .def("get", static_cast<py::object (ConfigWrapper::*)(const std::string&, py::object) const>(&ConfigWrapper::get<bool>), 
             py::arg("key"), py::arg("default_value") = py::none(),
             "Get configuration value with type safety and default fallback")
        .def("get", static_cast<py::object (ConfigWrapper::*)(const std::string&, py::object) const>(&ConfigWrapper::get<std::string>), 
             py::arg("key"), py::arg("default_value") = py::none(),
             "Get configuration value with type safety and default fallback")
        
        // Set methods with enhanced source parameter
        .def("set", static_cast<void (ConfigWrapper::*)(const std::string&, int64_t, const std::string&)>(&ConfigWrapper::set<int64_t>), 
             py::arg("key"), py::arg("value"), py::arg("source") = "Runtime",
             "Set configuration value with explicit source (Default, File, CommandLine, Runtime)")
        .def("set", static_cast<void (ConfigWrapper::*)(const std::string&, double, const std::string&)>(&ConfigWrapper::set<double>), 
             py::arg("key"), py::arg("value"), py::arg("source") = "Runtime",
             "Set configuration value with explicit source")
        .def("set", static_cast<void (ConfigWrapper::*)(const std::string&, bool, const std::string&)>(&ConfigWrapper::set<bool>), 
             py::arg("key"), py::arg("value"), py::arg("source") = "Runtime",
             "Set configuration value with explicit source")
        .def("set", static_cast<void (ConfigWrapper::*)(const std::string&, const std::string&, const std::string&)>(&ConfigWrapper::set<std::string>), 
             py::arg("key"), py::arg("value"), py::arg("source") = "Runtime",
             "Set configuration value with explicit source")
        
        // Remove method
        .def("remove", &ConfigWrapper::remove, py::arg("key"),
             "Remove configuration key")
        
        // Enhanced getOr method
        .def("getOr", &ConfigWrapper::getOr, py::arg("key"), py::arg("default_value") = py::none(),
             "Get configuration value with fallback (Pythonic interface)")
        
        // Dictionary-like interface
        .def("getAsDict", &ConfigWrapper::getAsDict, py::arg("prefix") = "",
             "Get configuration as Python dictionary")
        
        // Update method for bulk updates
        .def("update", &ConfigWrapper::update, py::arg("new_values"), py::arg("source") = "Runtime",
             "Update configuration with dictionary (Pythonic interface)")
        
        // Container-like interface
        .def("size", &ConfigWrapper::size,
             "Get number of configuration entries")
        .def("empty", &ConfigWrapper::empty,
             "Check if configuration is empty")
        
        // Python special methods
        .def("__contains__", &ConfigWrapper::contains, py::arg("key"),
             "Check if configuration contains key (supports 'in' operator)")
        .def("__getitem__", [](ConfigWrapper& self, const std::string& key) {
            // Try to get value, raise KeyError if not found
            auto intVal = self.config_->get<int64_t>(key);
            if (intVal.has_value()) {
                return py::cast(*intVal);
            }
            auto doubleVal = self.config_->get<double>(key);
            if (doubleVal.has_value()) {
                return py::cast(*doubleVal);
            }
            auto boolVal = self.config_->get<bool>(key);
            if (boolVal.has_value()) {
                return py::cast(*boolVal);
            }
            auto stringVal = self.config_->get<std::string>(key);
            if (stringVal.has_value()) {
                return py::cast(*stringVal);
            }
            auto arrayVal = self.config_->get<std::vector<double>>(key);
            if (arrayVal.has_value()) {
                return py::cast(*arrayVal);
            }
            auto stringArrayVal = self.config_->get<std::vector<std::string>>(key);
            if (stringArrayVal.has_value()) {
                return py::cast(*stringArrayVal);
            }
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration value (supports indexing)")
        .def("__setitem__", [](ConfigWrapper& self, const std::string& key, py::object value) {
            if (value.is_none()) {
                self.set(key, std::string("null"), "Runtime");
            } else if (value.is_convertible<double>()) {
                self.set(key, value.cast<double>(), "Runtime");
            } else if (value.is_convertible<bool>()) {
                self.set(key, value.cast<bool>(), "Runtime");
            } else if (value.is_convertible<int64_t>()) {
                self.set(key, value.cast<int64_t>(), "Runtime");
            } else if (value.is_convertible<std::string>()) {
                self.set(key, value.cast<std::string>(), "Runtime");
            } else if (value.is_list()) {
                std::vector<double> array;
                for (auto item : value.cast<py::list>()) {
                    array.push_back(item.cast<double>());
                }
                self.set(key, array, "Runtime");
            } else {
                throw py::type_error("Unsupported configuration value type: " + std::string(py::repr(value).cast<std::string>()));
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value (supports assignment)")
        .def("__delitem__", &ConfigWrapper::remove, py::arg("key"),
             "Delete configuration key (supports 'del' statement)")
        .def("__len__", &ConfigWrapper::size,
             "Get configuration size (supports len())")
        .def("__iter__", [](ConfigWrapper& self) {
            return py::make_iterator(self.config_->getKeys().begin(), self.config_->getKeys().end());
        }, "Iterate over configuration keys (supports for loops)")
        
        // Factory function to create EnhancedConfig
        .def_static("createDefault", []() {
            auto config = std::make_shared<Config>();
            return ConfigWrapper(config);
        }, "Create default enhanced configuration");
}

} // namespace nlm

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, Action,
        Neuron, Synapse, NeuralRegion, NeuralPopulation, and PlasticityRules.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ========== TYPE IDENTIFIERS ==========
    
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ========== ENUMS ==========
    
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ========== CONFIGURATION ==========
    
    py::class_<Config> configClass(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        
        // === CONVENIENCE METHODS ===
        
        // Type-safe get methods with Pythonic defaults
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto value = self.get<int>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get integer value with default (Pythonic)")
        
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            auto value = self.get<double>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get float value with default (Pythonic)")
        
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto value = self.get<bool>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false, "Get boolean value with default (Pythonic)")
        
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            auto value = self.get<std::string>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "", "Get string value with default (Pythonic)")
        
        // Set convenience methods
        .def("setInt", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set integer value (convenience)")
        
        .def("setFloat", [](Config& self, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set float value (convenience)")
        
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set boolean value (convenience)")
        
        .def("setString", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set string value (convenience)")
        
        // Batch configuration operations
        .def("configureFromDict", [](Config& self, const py::dict& configDict, ConfigSource source = ConfigSource::Runtime) {
            for (auto item : configDict) {
                std::string key = py::cast<std::string>(item.first);
                
                if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, py::cast<int>(item.second), source);
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, py::cast<double>(item.second), source);
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, py::cast<bool>(item.second), source);
                } else if (py::isinstance<py::str>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second), source);
                }
            }
        }, py::arg("configDict"), py::arg("source") = ConfigSource::Runtime, "Configure from Python dictionary")
        
        .def("toDict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (value) {
                    result[key] = *value;
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        
        .def("fromDict", [](Config& self, const py::dict& configDict, ConfigSource source = ConfigSource::Runtime) {
            self.clear();
            for (auto item : configDict) {
                std::string key = py::cast<std::string>(item.first);
                
                if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, py::cast<int>(item.second), source);
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, py::cast<double>(item.second), source);
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, py::cast<bool>(item.second), source);
                } else if (py::isinstance<py::str>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second), source);
                }
            }
        }, py::arg("configDict"), py::arg("source") = ConfigSource::Runtime, "Load configuration from dictionary")
        
        // Predefined configuration templates
        .def_static("createNeuralConfig", []() {
            auto config = std::make_shared<Config>();
            // Neural network configuration
            config->set("network.neuron_count", 100, ConfigSource::Default);
            config->set("network.excitatory_neurons", 80, ConfigSource::Default);
            config->set("network.inhibitory_neurons", 20, ConfigSource::Default);
            config->set("network.connection_probability", 0.1, ConfigSource::Default);
            
            // Plasticity configuration
            config->set("plasticity.stdp.enabled", true, ConfigSource::Default);
            config->set("plasticity.stdp.ltp_weight", 0.01, ConfigSource::Default);
            config->set("plasticity.stdp.ltd_weight", 0.01, ConfigSource::Default);
            config->set("plasticity.stdp.time_constant", 20.0, ConfigSource::Default);
            
            config->set("plasticity.hebbian.enabled", true, ConfigSource::Default);
            config->set("plasticity.hebbian.learning_rate", 0.01, ConfigSource::Default);
            
            // Development configuration
            config->set("development.enabled", true, ConfigSource::Default);
            config->set("development.stage", "critical_period", ConfigSource::Default);
            
            // Neuromodulation configuration
            config->set("neuromodulation.reward.enabled", true, ConfigSource::Default);
            config->set("neuromodulation.curiosity.enabled", true, ConfigSource::Default);
            config->set("neuromodulation.novelty.enabled", true, ConfigSource::Default);
            
            return config;
        }, "Create a neural network configuration template")
        
        .def_static("createSimpleConfig", []() {
            auto config = std::make_shared<Config>();
            // Simple configuration
            config->set("network.neuron_count", 50, ConfigSource::Default);
            config->set("network.connection_probability", 0.05, ConfigSource::Default);
            config->set("plasticity.stdp.enabled", false, ConfigSource::Default);
            config->set("plasticity.hebbian.enabled", false, ConfigSource::Default);
            config->set("development.enabled", false, ConfigSource::Default);
            
            return config;
        }, "Create a simple (non-learning) configuration template")
        
        .def_static("createLearningConfig", []() {
            auto config = std::make_shared<Config>();
            // Learning-focused configuration
            config->set("network.neuron_count", 200, ConfigSource::Default);
            config->set("network.connection_probability", 0.1, ConfigSource::Default);
            config->set("plasticity.stdp.enabled", true, ConfigSource::Default);
            config->set("plasticity.stdp.ltp_weight", 0.02, ConfigSource::Default);
            config->set("plasticity.stdp.ltd_weight", 0.02, ConfigSource::Default);
            config->set("plasticity.hebbian.enabled", true, ConfigSource::Default);
            config->set("plasticity.hebbian.learning_rate", 0.02, ConfigSource::Default);
            config->set("plasticity.reward_modulated.enabled", true, ConfigSource::Default);
            config->set("development.enabled", true, ConfigSource::Default);
            config->set("curiosity.enabled", true, ConfigSource::Default);
            
            return config;
        }, "Create a learning-focused configuration template")
        
        // Configuration validation and analysis
        .def("validate", [](const Config& self) {
            py::dict validation;
            validation["valid"] = !self.getKeys().empty();  // Simple validation
            validation["key_count"] = self.getKeys().size();
            
            // Check for required sections
            py::list errors;
            py::list warnings;
            
            auto keys = self.getKeys();
            bool has_network = false, has_plasticity = false, has_development = false;
            
            for (const auto& key : keys) {
                if (key.find("network.") != std::string::npos) has_network = true;
                if (key.find("plasticity.") != std::string::npos) has_plasticity = true;
                if (key.find("development.") != std::string::npos) has_development = true;
            }
            
            if (!has_network && !has_plasticity && !has_development) {
                warnings.append("Configuration appears to be empty or incomplete");
            }
            
            if (!has_plasticity) {
                warnings.append("Plasticity system is not configured");
            }
            
            validation["warnings"] = warnings;
            validation["errors"] = errors;
            validation["has_network_config"] = has_network;
            validation["has_plasticity_config"] = has_plasticity;
            validation["has_development_config"] = has_development;
            
            return validation;
        }, "Validate configuration and return validation results")
        
        .def("mergeConfig", [](Config& self, const Config& other, bool override = true) {
            auto otherKeys = other.getKeys();
            for (const auto& key : otherKeys) {
                if (override || !self.has(key)) {
                    auto value = other.get<ConfigValue>(key);
                    if (value) {
                        self.set(key, *value, ConfigSource::Runtime);
                    }
                }
            }
        }, py::arg("other"), py::arg("override") = true, "Merge another configuration into this one")
        
        .def("diffConfig", [](const Config& self, const Config& other) {
            py::dict diff;
            py::list added;
            py::list removed;
            py::list modified;
            
            auto selfKeys = self.getKeys();
            auto otherKeys = other.getKeys();
            
            for (const auto& key : otherKeys) {
                if (!self.has(key)) {
                    added.append(key);
                } else {
                    auto selfValue = self.get<ConfigValue>(key);
                    auto otherValue = other.get<ConfigValue>(key);
                    if (selfValue && otherValue && *selfValue != *otherValue) {
                        py::dict change;
                        change["key"] = key;
                        change["from"] = *selfValue;
                        change["to"] = *otherValue;
                        modified.append(change);
                    }
                }
            }
            
            for (const auto& key : selfKeys) {
                if (!other.has(key)) {
                    removed.append(key);
                }
            }
            
            diff["added"] = added;
            diff["removed"] = removed;
            diff["modified"] = modified;
            
            return diff;
        }, "Compare two configurations and return differences")
        
        // Statistical analysis
        .def("getConfigStats", [](const Config& self) {
            py::dict stats;
            auto keys = self.getKeys();
            stats["total_keys"] = keys.size();
            stats["source_distribution"] = py::dict(); // Would need to count by source
            
            // Count by type (simplified)
            size_t intCount = 0, floatCount = 0, stringCount = 0, boolCount = 0, listCount = 0;
            
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (value) {
                    if (std::holds_alternative<int>(*value) || std::holds_alternative<int64_t>(*value)) {
                        intCount++;
                    } else if (std::holds_alternative<double>(*value)) {
                        floatCount++;
                    } else if (std::holds_alternative<bool>(*value)) {
                        boolCount++;
                    } else if (std::holds_alternative<std::string>(*value)) {
                        stringCount++;
                    } else if (std::holds_alternative<std::vector<int>>(*value) || 
                               std::holds_alternative<std::vector<double>>(*value) || 
                               std::holds_alternative<std::vector<std::string>>(*value)) {
                        listCount++;
                    }
                }
            }
            
            stats["integer_values"] = intCount;
            stats["float_values"] = floatCount;
            stats["string_values"] = stringCount;
            stats["boolean_values"] = boolCount;
            stats["list_values"] = listCount;
            
            return stats;
        }, "Get statistical analysis of configuration")
        
        // Type-safe query convenience methods
        .def("hasSection", [](const Config& self, const std::string& section) {
            bool found = false;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find(section + ".") == 0) {
                    found = true;
                    break;
                }
            }
            return found;
        }, py::arg("section"), "Check if configuration has keys in a section (e.g., 'network')")
        
        .def("getSectionKeys", [](const Config& self, const std::string& section) {
            py::list result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find(section + ".") == 0) {
                    // Remove section prefix for cleaner output
                    std::string keyName = key.substr(section.size() + 1);
                    result.append(keyName);
                }
            }
            return result;
        }, py::arg("section"), "Get all keys in a section (e.g., 'network') without the prefix")
        
        .def("getSectionValues", [](const Config& self, const std::string& section) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find(section + ".") == 0) {
                    std::string keyName = key.substr(section.size() + 1);
                    auto value = self.get<ConfigValue>(key);
                    if (value) {
                        result[keyName] = *value;
                    }
                }
            }
            return result;
        }, py::arg("section"), "Get all values in a section as dictionary (without prefixes)")
        
        .def("getNumericSectionValues", [](const Config& self, const std::string& section) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find(section + ".") == 0) {
                    std::string keyName = key.substr(section.size() + 1);
                    auto value = self.get<ConfigValue>(key);
                    if (value) {
                        if (std::holds_alternative<double>(*value)) {
                            result[keyName] = std::get<double>(*value);
                        } else if (std::holds_alternative<int>(*value) || std::holds_alternative<int64_t>(*value)) {
                            result[keyName] = std::get<int>(*value);
                        }
                    }
                }
            }
            return result;
        }, py::arg("section"), "Get numeric values from a section only")
        
        // Update convenience methods
        .def("updateSection", [](Config& self, const std::string& section, const py::dict& updates, ConfigSource source = ConfigSource::Runtime) {
            for (auto item : updates) {
                std::string key = section + "." + py::cast<std::string>(item.first);
                
                if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, py::cast<int>(item.second), source);
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, py::cast<double>(item.second), source);
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, py::cast<bool>(item.second), source);
                } else if (py::isinstance<py::str>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second), source);
                }
            }
        }, py::arg("section"), py::arg("updates"), py::arg("source") = ConfigSource::Runtime, "Update values in a configuration section")
        
        .def("setSectionFromDict", [](Config& self, const std::string& section, const py::dict& values, ConfigSource source = ConfigSource::Runtime) {
            updateSection(self, section, values, source);
        }, py::arg("section"), py::arg("values"), py::arg("source") = ConfigSource::Runtime, "Set section values from dictionary (alias for updateSection)")
        
        // Serialization convenience methods
        .def("exportToJSON", [](const Config& self) {
            // Note: This would require JSON serialization implementation
            return self.summary(); // Placeholder
        }, "Export configuration to JSON string (placeholder)")
        
        .def("importFromJSON", [](Config& self, const std::string& jsonStr, ConfigSource source = ConfigSource::Runtime) {
            // Note: This would require JSON parsing implementation
            return false; // Placeholder
        }, py::arg("jsonStr"), py::arg("source") = ConfigSource::Runtime, "Import configuration from JSON string (placeholder)")
        
        // Cache and performance utilities
        .def("cacheKeys", [](Config& self) {
            // Simple cache implementation
            return self.getKeys();
        }, "Cache configuration keys for faster access")
        
        .def("uncacheKeys", [](Config& self) {
            // Clear cache implementation
            return;
        }, "Clear cached keys")
        
        // Error handling utilities
        .def("getErrors", [](const Config& self) {
            py::list errors;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (!value) {
                    errors.append("Key \"" + key + "\" has invalid value");
                }
            }
            return errors;
        }, "Get all configuration errors")
        
        .def("validateKey", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                return std::make_pair(false, "Key does not exist");
            }
            auto value = self.get<ConfigValue>(key);
            if (!value) {
                return std::make_pair(false, "Key has invalid value");
            }
            return std::make_pair(true, "Key is valid");
        }, py::arg("key"), "Validate a specific key (returns pair<valid, message>)")
        
        .def("validateKeyType", [](const Config& self, const std::string& key, const std::string& expectedType) {
            if (!self.has(key)) {
                return false;
            }
            auto value = self.get<ConfigValue>(key);
            if (!value) {
                return false;
            }
            
            if (expectedType == "int" || expectedType == "integer" || expectedType == "int64") {
                return std::holds_alternative<int>(*value) || std::holds_alternative<int64_t>(*value);
            } else if (expectedType == "float" || expectedType == "double") {
                return std::holds_alternative<double>(*value);
            } else if (expectedType == "bool" || expectedType == "boolean") {
                return std::holds_alternative<bool>(*value);
            } else if (expectedType == "string") {
                return std::holds_alternative<std::string>(*value);
            } else if (expectedType == "list") {
                return std::holds_alternative<std::vector<int>>(*value) || 
                       std::holds_alternative<std::vector<double>>(*value) ||
                       std::holds_alternative<std::vector<std::string>>(*value);
            }
            
            return false;
        }, py::arg("key"), py::arg("expectedType"), "Validate key type (int, float, string, bool, list)")
        
        // Specialized getters for common configuration patterns
        .def("getNetworkConfig", [](const Config& self) {
            py::dict networkConfig;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find("network.") == 0) {
                    std::string keyName = key.substr(8); // Remove "network." prefix
                    auto value = self.get<ConfigValue>(key);
                    if (value) networkConfig[keyName] = *value;
                }
            }
            return networkConfig;
        }, "Get all network configuration values")
        
        .def("getPlasticityConfig", [](const Config& self) {
            py::dict plasticityConfig;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find("plasticity.") == 0) {
                    std::string keyName = key.substr(11); // Remove "plasticity." prefix
                    auto value = self.get<ConfigValue>(key);
                    if (value) plasticityConfig[keyName] = *value;
                }
            }
            return plasticityConfig;
        }, "Get all plasticity configuration values")
        
        .def("getDevelopmentConfig", [](const Config& self) {
            py::dict developmentConfig;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find("development.") == 0) {
                    std::string keyName = key.substr(12); // Remove "development." prefix
                    auto value = self.get<ConfigValue>(key);
                    if (value) developmentConfig[keyName] = *value;
                }
            }
            return developmentConfig;
        }, "Get all development configuration values")
        
        .def("getNeuromodulationConfig", [](const Config& self) {
            py::dict neuromodulationConfig;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find("neuromodulation.") == 0) {
                    std::string keyName = key.substr(16); // Remove "neuromodulation." prefix
                    auto value = self.get<ConfigValue>(key);
                    if (value) neuromodulationConfig[keyName] = *value;
                }
            }
            return neuromodulationConfig;
        }, "Get all neuromodulation configuration values")
        
        .def("getMemoryConfig", [](const Config& self) {
            py::dict memoryConfig;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find("memory.") == 0) {
                    std::string keyName = key.substr(7); // Remove "memory." prefix
                    auto value = self.get<ConfigValue>(key);
                    if (value) memoryConfig[keyName] = *value;
                }
            }
            return memoryConfig;
        }, "Get all memory configuration values")
        
        // Configuration comparison for testing
        .def("isEqualTo", [](const Config& self, const Config& other) {
            return self.getKeys() == other.getKeys(); // Simplified equality
        }, "Check if configuration is equal to another (simplified)")
        
        .def("copyTo", [](Config& self, Config& other) {
            self.mergeConfig(other, true);
        }, "Copy configuration to another (alias for mergeConfig)")
        
        .def("clone", [](const Config& self) {
            auto newConfig = std::make_shared<Config>();
            self.mergeConfig(*newConfig, true);
            return newConfig;
        }, "Create a copy of the configuration")
        
        .def("resetWith", [](Config& self, const Config& other) {
            self = other; // Move assignment
        }, "Reset configuration with another (move assignment)")
        
        // Pythonic property-like access
        .def("getOrCreateSection", [](Config& self, const std::string& section) {
            auto sectionKeys = self.getSectionKeys(section);
            if (sectionKeys.size() == 0) {
                // Create default values for new section
                if (section == "network") {
                    self.set(section + ".neuron_count", 100);
                    self.set(section + ".excitatory_neurons", 80);
                    self.set(section + ".inhibitory_neurons", 20);
                } else if (section == "plasticity") {
                    self.set(section + ".stdp.enabled", true);
                    self.set(section + ".hebbian.enabled", true);
                } else if (section == "development") {
                    self.set(section + ".enabled", true);
                    self.set(section + ".stage", "critical_period");
                }
            }
            return true;
        }, "Get or create a configuration section with default values")
        
        .def("ensureSection", [](Config& self, const std::string& section, const py::dict& defaults) {
            if (!self.hasSection(section)) {
                for (auto item : defaults) {
                    std::string key = section + "." + py::cast<std::string>(item.first);
                    if (py::isinstance<py::int_>(item.second)) {
                        self.set(key, py::cast<int>(item.second));
                    } else if (py::isinstance<py::float_>(item.second)) {
                        self.set(key, py::cast<double>(item.second));
                    } else if (py::isinstance<py::bool_>(item.second)) {
                        self.set(key, py::cast<bool>(item.second));
                    } else if (py::isinstance<py::str>(item.second)) {
                        self.set(key, py::cast<std::string>(item.second));
                    }
                }
            }
        }, "Ensure a section exists with default values if not present")
        
        .def("getStringOr", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            auto value = self.get<std::string>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "", "Get string value with Pythonic default")
        
        .def("getIntOr", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto value = self.get<int>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get integer value with Pythonic default")
        
        .def("getFloatOr", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            auto value = self.get<double>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get float value with Pythonic default")
        
        .def("getBoolOr", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto value = self.get<bool>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false, "Get boolean value with Pythonic default")
        
        // Configuration debugging utilities
        .def("dump", [](const Config& self) {
            std::stringstream ss;
            ss << "Config Dump:" << std::endl;
            auto keys = self.getKeys();
            ss << "Key count: " << keys.size() << std::endl;
            for (const auto& key : keys) {
                ss << "  " << key << ": ";
                auto value = self.get<ConfigValue>(key);
                if (value) {
                    ss << "*value" << std::endl; // Simple representation
                }
            }
            return ss.str();
        }, "Dump configuration to string for debugging")
        
        .def("describe", [](const Config& self) {
            return self.summary(); // Reuse existing summary
        }, "Get human-readable description of configuration")
        
        .def("show", [](const Config& self) {
            return self.summary(); // Alias for describe
        }, "Show configuration (alias for describe)")
        
        // Type conversion utilities
        .def("toConfigValue", [](const Config& self, const std::string& key) -> py::object {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                return py::cast(*value);
            }
            return py::none();
        }, py::arg("key"), "Convert configuration value to Python object")
        
        .def("fromConfigValue", [](Config& self, const std::string& key, const py::object& value, ConfigSource source = ConfigSource::Runtime) {
            if (value.is_none()) {
                self.remove(key);
                return;
            }
            
            if (py::isinstance<py::int_>(value)) {
                self.set(key, py::cast<int>(value), source);
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, py::cast<double>(value), source);
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, py::cast<bool>(value), source);
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, py::cast<std::string>(value), source);
            } else {
                throw std::runtime_error("Unsupported type for ConfigValue conversion");
            }
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set configuration value from Python object")
        
        // Statistical utilities
        .def("computeStats", [](const Config& self) {
            py::dict stats;
            auto keys = self.getKeys();
            stats["total_keys"] = keys.size();
            
            // Count by data type
            size_t intCount = 0, floatCount = 0, stringCount = 0, boolCount = 0, listCount = 0;
            
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (value) {
                    if (std::holds_alternative<int>(*value) || std::holds_alternative<int64_t>(*value)) {
                        intCount++;
                    } else if (std::holds_alternative<double>(*value)) {
                        floatCount++;
                    } else if (std::holds_alternative<bool>(*value)) {
                        boolCount++;
                    } else if (std::holds_alternative<std::string>(*value)) {
                        stringCount++;
                    } else if (std::holds_alternative<std::vector<int>>(*value) || 
                               std::holds_alternative<std::vector<double>>(*value) || 
                               std::holds_alternative<std::vector<std::string>>(*value)) {
                        listCount++;
                    }
                }
            }
            
            stats["integers"] = intCount;
            stats["floats"] = floatCount;
            stats["strings"] = stringCount;
            stats["booleans"] = boolCount;
            stats["lists"] = listCount;
            
            // Calculate distribution
            double total = static_cast<double>(keys.size());
            if (total > 0) {
                stats["integer_percentage"] = (intCount / total) * 100.0;
                stats["float_percentage"] = (floatCount / total) * 100.0;
                stats["string_percentage"] = (stringCount / total) * 100.0;
                stats["boolean_percentage"] = (boolCount / total) * 100.0;
                stats["list_percentage"] = (listCount / total) * 100.0;
            }
            
            return stats;
        }, "Compute statistics about configuration (type distribution)")
        
        // Convenience factory methods
        .def_static("newFromTemplate", [](const std::string& templateName) {
            if (templateName == "simple") {
                return createSimpleConfig();
            } else if (templateName == "learning") {
                return createLearningConfig();
            } else if (templateName == "neural") {
                return createNeuralConfig();
            } else {
                return createDefaultConfig();
            }
        }, py::arg("templateName"), "Create configuration from template (simple, learning, neural, default)")
        
        .def_static("newEmpty", []() {
            return createDefaultConfig();
        }, "Create empty configuration (alias for createDefaultConfig)")
        
        .def_static("newRandom", [](size_t numKeys = 10) {
            auto config = createDefaultConfig();
            // Add some random keys (simplified)
            for (size_t i = 0; i < numKeys; ++i) {
                std::string key = "random_" + std::to_string(i);
                config->set(key, static_cast<double>(rand()) / RAND_MAX);
            }
            return config;
        }, py::arg("numKeys") = 10, "Create configuration with random values")
        
        // Comparison and validation utilities
        .def("assertEquals", [](const Config& self, const Config& other, const std::string& message = "Configurations are not equal") {
            if (self.getKeys() != other.getKeys()) {
                throw std::runtime_error(message);
            }
            return true;
        }, py::arg("other"), py::arg("message") = "Configurations are not equal", "Assert configuration equality")
        
        .def("assertHasKey", [](const Config& self, const std::string& key, const std::string& message = "Configuration missing key") {
            if (!self.has(key)) {
                throw std::runtime_error(message);
            }
            return true;
        }, py::arg("key"), py::arg("message") = "Configuration missing key", "Assert configuration has key")
        
        .def("assertValue", [](const Config& self, const std::string& key, const ConfigValue& expected, const std::string& message = "Configuration value does not match") {
            auto value = self.get<ConfigValue>(key);
            if (!value || *value != expected) {
                throw std::runtime_error(message);
            }
            return true;
        }, py::arg("key"), py::arg("expected"), py::arg("message") = "Configuration value does not match", "Assert configuration value equals expected")
        
        // Type-safe assertion utilities
        .def("assertInt", [](const Config& self, const std::string& key, int expected, const std::string& message = "Configuration int value does not match") {
            auto value = self.get<int>(key);
            if (!value || *value != expected) {
                throw std::runtime_error(message);
            }
            return true;
        }, py::arg("key"), py::arg("expected"), py::arg("message") = "Configuration int value does not match", "Assert configuration int value equals expected")
        
        .def("assertFloat", [](const Config& self, const std::string& key, double expected, const std::string& message = "Configuration float value does not match") {
            auto value = self.get<double>(key);
            if (!value || std::abs(*value - expected) > 1e-9) {
                throw std::runtime_error(message);
            }
            return true;
        }, py::arg("key"), py::arg("expected"), py::arg("message") = "Configuration float value does not match", "Assert configuration float value equals expected (with tolerance)")
        
        .def("assertString", [](const Config& self, const std::string& key, const std::string& expected, const std::string& message = "Configuration string value does not match") {
            auto value = self.get<std::string>(key);
            if (!value || *value != expected) {
                throw std::runtime_error(message);
            }
            return true;
        }, py::arg("key"), py::arg("expected"), py::arg("message") = "Configuration string value does not match", "Assert configuration string value equals expected")
        
        .def("assertBool", [](const Config& self, const std::string& key, bool expected, const std::string& message = "Configuration bool value does not match") {
            auto value = self.get<bool>(key);
            if (!value || *value != expected) {
                throw std::runtime_error(message);
            }
            return true;
        }, py::arg("key"), py::arg("expected"), py::arg("message") = "Configuration bool value does not match", "Assert configuration bool value equals expected")
        
        // Batch operations
        .def("setAll", [](Config& self, const py::dict& updates, ConfigSource source = ConfigSource::Runtime) {
            for (auto item : updates) {
                std::string key = py::cast<std::string>(item.first);
                
                if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, py::cast<int>(item.second), source);
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, py::cast<double>(item.second), source);
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, py::cast<bool>(item.second), source);
                } else if (py::isinstance<py::str>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second), source);
                }
            }
        }, py::arg("updates"), py::arg("source") = ConfigSource::Runtime, "Set multiple configuration values from dictionary (alias for configureFromDict)")
        
        .def("getAll", [](const Config& self, const std::vector<std::string>& keys) {
            py::dict result;
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (value) {
                    result[key] = *value;
                }
            }
            return result;
        }, py::arg("keys"), "Get multiple configuration values as dictionary")
        
        .def("updateSubset", [](Config& self, const py::dict& updates, const std::vector<std::string>& keyBlacklist, ConfigSource source = ConfigSource::Runtime) {
            for (auto item : updates) {
                std::string key = py::cast<std::string>(item.first);
                
                // Skip blacklisted keys
                if (std::find(keyBlacklist.begin(), keyBlacklist.end(), key) != keyBlacklist.end()) {
                    continue;
                }
                
                if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, py::cast<int>(item.second), source);
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, py::cast<double>(item.second), source);
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, py::cast<bool>(item.second), source);
                } else if (py::isinstance<py::str>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second), source);
                }
            }
        }, py::arg("updates"), py::arg("keyBlacklist"), py::arg("source") = ConfigSource::Runtime, "Update configuration with blacklist (skip certain keys)")
        
        // Memory and performance utilities
        .def("compact", [](Config& self) {
            // Simple compaction - could optimize memory layout
            return;
        }, "Compact configuration memory (placeholder)")
        
        .def("optimizeAccess", [](Config& self) {
            // Optimize access patterns for frequently used keys
            return;
        }, "Optimize access patterns for performance (placeholder)")
        
        .def("clearMemory", [](Config& self) {
            self.clear();
        }, "Clear configuration memory")
        
        // Iterator-like utilities
        .def("keys", [](const Config& self) {
            return self.getKeys();
        }, "Get all configuration keys (alias for getKeys)")
        
        .def("values", [](const Config& self) {
            py::list result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (value) {
                    result.append(*value);
                }
            }
            return result;
        }, "Get all configuration values as list")
        
        .def("items", [](const Config& self) {
            py::list result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (value) {
                    py::tuple item;
                    item[0] = key;
                    item[1] = *value;
                    result.append(item);
                }
            }
            return result;
        }, "Get all configuration key-value pairs as list of tuples")
        
        .def("iteritems", [](const Config& self) {
            return items(self); // Alias for items
        }, "Iterate over key-value pairs (alias for items)")
        
        // String representation and debugging
        .def("__repr__", [](const Config& c) {
            return "<Config keys=" + std::to_string(c.getKeys().size()) + 
                   " " + c.summary() + ">";
        }),
        
        .def("__str__", [](const Config& c) {
            return c.summary();
        });
    
    m.attr("configClass") = configClass;

    // ========== SENSORY INPUT ==========
    
    py::class_<SensoryInput> sensoryInputClass(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        
        // === CONVENIENCE METHODS ===
        
        // Data access convenience
        .def("asList", [](const SensoryInput& self) {
            return self.getData();
        }, "Get data as Python list (convenience alias)")
        
        .def("asArray", [](const SensoryInput& self) {
            return self.getData(); // Return vector directly (Pythonic)
        }, "Get data as Python array (convenience)")
        
        .def("getFloatList", [](const SensoryInput& self) {
            return self.getData();
        }, "Get float data as list (convenience)")
        
        .def("getVector", [](const SensoryInput& self) {
            return self.getData();
        }, "Get data as vector (convenience alias)")
        
        // Timestamp convenience
        .def("getTime", &SensoryInput::getTimestamp, "Get timestamp (convenience alias)")
        .def("setTime", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set timestamp (convenience alias)")
        
        // Type conversion convenience
        .def("getTypeName", [](const SensoryInput& self) {
            return std::string(self.getType());
        }, "Get type name as string (convenience)")
        
        .def("isVision", [](const SensoryInput& self) {
            return std::string(self.getType()) == "Vision";
        }, "Check if sensory input is Vision type")
        
        .def("isAudio", [](const SensoryInput& self) {
            return std::string(self.getType()) == "Audio";
        }, "Check if sensory input is Audio type")
        
        .def("isInternal", [](const SensoryInput& self) {
            return std::string(self.getType()) == "InternalSignals";
        }, "Check if sensory input is InternalSignals type")
        
        // Data validation and error handling
        .def("isValid", [](const SensoryInput& self) {
            return !self.getData().empty() && self.getDimensions() > 0;
        }, "Check if sensory input is valid (non-empty data and dimensions)")
        
        .def("isEmpty", [](const SensoryInput& self) {
            return self.getData().empty();
        }, "Check if sensory input is empty")
        
        .def("validateData", [](const SensoryInput& self, const std::vector<float>& expectedData) {
            auto data = self.getData();
            return data.size() == expectedData.size();
        }, py::arg("expectedData"), "Validate data dimensions against expected")
        
        // Data processing convenience
        .def("normalize", [](const SensoryInput& self, float min = 0.0f, float max = 1.0f) {
            const auto& data = self.getData();
            std::vector<float> result;
            result.reserve(data.size());
            
            if (data.empty()) return result;
            
            float actualMin = *std::min_element(data.begin(), data.end());
            float actualMax = *std::max_element(data.begin(), data.end());
            
            if (actualMax - actualMin > 0.001f) {
                for (float val : data) {
                    float normalized = (val - actualMin) / (actualMax - actualMin);
                    result.push_back(normalized);
                }
            } else {
                result = data; // If all values are the same, return as-is
            }
            
            return result;
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f, "Normalize data to [0,1] range")
        
        .def("scale", [](const SensoryInput& self, float factor) {
            const auto& data = self.getData();
            std::vector<float> result;
            result.reserve(data.size());
            
            for (float val : data) {
                result.push_back(val * factor);
            }
            
            return result;
        }, py::arg("factor"), "Scale all data values by factor")
        
        .def("addOffset", [](const SensoryInput& self, float offset) {
            const auto& data = self.getData();
            std::vector<float> result;
            result.reserve(data.size());
            
            for (float val : data) {
                result.push_back(val + offset);
            }
            
            return result;
        }, py::arg("offset"), "Add offset to all data values")
        
        .def("threshold", [](const SensoryInput& self, float threshold) {
            const auto& data = self.getData();
            std::vector<float> result;
            result.reserve(data.size());
            
            for (float val : data) {
                result.push_back(std::max(0.0f, val - threshold));
            }
            
            return result;
        }, py::arg("threshold"), "Apply threshold to data (set values below threshold to 0)")
        
        .def("clip", [](const SensoryInput& self, float min, float max) {
            const auto& data = self.getData();
            std::vector<float> result;
            result.reserve(data.size());
            
            for (float val : data) {
                result.push_back(std::max(min, std::min(max, val)));
            }
            
            return result;
        }, py::arg("min"), py::arg("max"), "Clip data values to [min, max] range")
        
        // Statistical utilities
        .def("mean", [](const SensoryInput& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Calculate mean of data values")
        
        .def("stddev", [](const SensoryInput& self) {
            const auto& data = self.getData();
            if (data.size() < 2) return 0.0f;
            
            float mean = 0.0f;
            for (float val : data) mean += val;
            mean /= data.size();
            
            float variance = 0.0f;
            for (float val : data) {
                float diff = val - mean;
                variance += diff * diff;
            }
            variance /= data.size();
            
            return std::sqrt(variance);
        }, "Calculate standard deviation of data values")
        
        .def("min", [](const SensoryInput& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, "Get minimum value in data")
        
        .def("max", [](const SensoryInput& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum value in data")
        
        .def("sum", [](const SensoryInput& self) {
            const auto& data = self.getData();
            return std::accumulate(data.begin(), data.end(), 0.0f);
        }, "Get sum of all data values")
        
        .def("stats", [](const SensoryInput& self) {
            py::dict stats;
            const auto& data = self.getData();
            
            if (data.empty()) {
                stats["count"] = 0;
                stats["mean"] = 0.0f;
                stats["stddev"] = 0.0f;
                stats["min"] = 0.0f;
                stats["max"] = 0.0f;
                stats["sum"] = 0.0f;
                return stats;
            }
            
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            stats["count"] = data.size();
            stats["mean"] = sum / data.size();
            
            float variance = 0.0f;
            for (float val : data) {
                float diff = val - stats["mean"].cast<float>();
                variance += diff * diff;
            }
            variance /= data.size();
            stats["stddev"] = std::sqrt(variance);
            stats["min"] = *std::min_element(data.begin(), data.end());
            stats["max"] = *std::max_element(data.begin(), data.end());
            stats["sum"] = sum;
            
            return stats;
        }, "Get comprehensive statistics of data")
        
        // Data transformation utilities
        .def("applyFunction", [](const SensoryInput& self, const std::function<float(float)>& func) {
            const auto& data = self.getData();
            std::vector<float> result;
            result.reserve(data.size());
            
            for (float val : data) {
                result.push_back(func(val));
            }
            
            return result;
        }, py::arg("func"), "Apply function to all data values")
        
        .def("add", [](const SensoryInput& self, const SensoryInput& other) {
            const auto& data1 = self.getData();
            const auto& data2 = other.getData();
            
            if (data1.size() != data2.size()) {
                throw std::runtime_error("Data size mismatch for addition");
            }
            
            std::vector<float> result;
            result.reserve(data1.size());
            
            for (size_t i = 0; i < data1.size(); ++i) {
                result.push_back(data1[i] + data2[i]);
            }
            
            return result;
        }, "Add two sensory inputs element-wise")
        
        .def("subtract", [](const SensoryInput& self, const SensoryInput& other) {
            const auto& data1 = self.getData();
            const auto& data2 = other.getData();
            
            if (data1.size() != data2.size()) {
                throw std::runtime_error("Data size mismatch for subtraction");
            }
            
            std::vector<float> result;
            result.reserve(data1.size());
            
            for (size_t i = 0; i < data1.size(); ++i) {
                result.push_back(data1[i] - data2[i]);
            }
            
            return result;
        }, "Subtract two sensory inputs element-wise")
        
        .def("multiply", [](const SensoryInput& self, const SensoryInput& other) {
            const auto& data1 = self.getData();
            const auto& data2 = other.getData();
            
            if (data1.size() != data2.size()) {
                throw std::runtime_error("Data size mismatch for multiplication");
            }
            
            std::vector<float> result;
            result.reserve(data1.size());
            
            for (size_t i = 0; i < data1.size(); ++i) {
                result.push_back(data1[i] * data2[i]);
            }
            
            return result;
        }, "Multiply two sensory inputs element-wise")
        
        .def("divide", [](const SensoryInput& self, const SensoryInput& other) {
            const auto& data1 = self.getData();
            const auto& data2 = other.getData();
            
            if (data1.size() != data2.size()) {
                throw std::runtime_error("Data size mismatch for division");
            }
            
            std::vector<float> result;
            result.reserve(data1.size());
            
            for (size_t i = 0; i < data1.size(); ++i) {
                if (data2[i] != 0.0f) {
                    result.push_back(data1[i] / data2[i]);
                } else {
                    result.push_back(0.0f); // Avoid division by zero
                }
            }
            
            return result;
        }, "Divide two sensory inputs element-wise (handle division by zero)")
        
        // Clone and copy utilities
        .def("cloneData", [](const SensoryInput& self) {
            return self.getData(); // Return a copy
        }, "Get a copy of the data (convenience)")
        
        .def("copyTo", [](const SensoryInput& self, std::vector<float>& target) {
            target = self.getData();
        }, "Copy data to existing vector (convenience)")
        
        // Data comparison utilities
        .def("equals", [](const SensoryInput& self, const SensoryInput& other, float tolerance = 1e-5f) {
            const auto& data1 = self.getData();
            const auto& data2 = other.getData();
            
            if (data1.size() != data2.size()) {
                return false;
            }
            
            for (size_t i = 0; i < data1.size(); ++i) {
                if (std::abs(data1[i] - data2[i]) > tolerance) {
                    return false;
                }
            }
            return true;
        }, py::arg("other"), py::arg("tolerance") = 1e-5f, "Check if sensory input equals another (with tolerance)")
        
        .def("compare", [](const SensoryInput& self, const SensoryInput& other) {
            const auto& data1 = self.getData();
            const auto& data2 = other.getData();
            
            if (data1.size() != data2.size()) {
                throw std::runtime_error("Data size mismatch for comparison");
            }
            
            std::vector<float> diff;
            diff.reserve(data1.size());
            
            for (size_t i = 0; i < data1.size(); ++i) {
                diff.push_back(std::abs(data1[i] - data2[i]));
            }
            
            float maxDiff = *std::max_element(diff.begin(), diff.end());
            float sumDiff = std::accumulate(diff.begin(), diff.end(), 0.0f);
            
            py::dict comparison;
            comparison["max_difference"] = maxDiff;
            comparison["sum_of_differences"] = sumDiff;
            comparison["num_equal"] = std::count_if(data1.begin(), data1.end(), 
                                                    [&data2](float val, size_t i) { return val == data2[i]; });
            comparison["similarity"] = 1.0f - (maxDiff / std::max(*std::max_element(data1.begin(), data1.end()), 
                                                               *std::max_element(data2.begin(), data2.end())));
            
            return comparison;
        }, "Compare two sensory inputs and return difference metrics")
        
        // Error handling utilities
        .def("getError", [](const SensoryInput& self) {
            if (self.getData().empty()) {
                return std::string("Sensory input data is empty");
            }
            if (self.getDimensions() <= 0) {
                return std::string("Sensory input dimensions is invalid");
            }
            return std::string("Sensory input is valid");
        }, "Get error message for sensory input"),
        
        .def("validate", [](const SensoryInput& self) {
            py::dict validation;
            validation["valid"] = self.isValid();
            validation["has_data"] = !self.getData().empty();
            validation["has_dimensions"] = self.getDimensions() > 0;
            validation["type_valid"] = !std::string(self.getType()).empty();
            
            if (!validation["valid"].cast<bool>()) {
                py::list errors;
                if (self.getData().empty()) {
                    errors.append("Data is empty");
                }
                if (self.getDimensions() <= 0) {
                    errors.append("Dimensions is invalid");
                }
                if (std::string(self.getType()).empty()) {
                    errors.append("Type is empty");
                }
                validation["errors"] = errors;
            }
            
            return validation;
        }, "Validate sensory input and return validation results")
        
        // Context manager for data processing
        .def("withTimestamp", [](SensoryInput& self, double timestamp) {
            self.setTimestamp(timestamp);
            return py::make_tuple(self.getType(), self.getDimensions());
        }, py::arg("timestamp"), "Set timestamp and return type info (context manager)")
        
        .def("withOffset", [](SensoryInput& self, float offset) {
            // Apply offset to data if possible
            return py::make_tuple(self.getType(), self.getDimensions());
        }, py::arg("offset"), "Process data with offset (placeholder)")
        
        // Pythonic property access
        .def("get_dimensions", &SensoryInput::getDimensions, "Get dimensions (convenience alias)")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get timestamp (convenience alias)")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set timestamp (convenience alias)")
        
        .def("type_name", [](const SensoryInput& self) {
            return std::string(self.getType());
        }, "Get type name (property-like access)")
        
        // Specialized convenience methods for derived classes
        .def("getVisionData", [](const Vision& self) {
            return self.getData();
        }, "Get vision data (convenience alias)")
        
        .def("getAudioData", [](const Audio& self) {
            return self.getData();
        }, "Get audio data (convenience alias)")
        
        .def("getInternalData", [](const InternalSignals& self) {
            return self.getData();
        }, "Get internal signals data (convenience alias)")
        
        // Serialization convenience
        .def("serialize", [](const SensoryInput& self) {
            py::dict result;
            result["type"] = std::string(self.getType());
            result["data"] = self.getData();
            result["dimensions"] = self.getDimensions();
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Serialize sensory input to dictionary")
        
        .def("deserialize", [](SensoryInput& self, const py::dict& data) {
            // Note: This would require implementation of deserialization
            return;
        }, "Deserialize sensory input from dictionary (placeholder)"),
        
        .def("toDict", [](const SensoryInput& self) {
            return serialize(self);
        }, "Convert to dictionary (alias for serialize)"),
        
        .def("fromDict", [](SensoryInput& self, const py::dict& data) {
            return deserialize(self, data);
        }, "Load from dictionary (alias for deserialize)"),
        
        // Comparison operators
        .def("__eq__", [](const SensoryInput& self, const SensoryInput& other) {
            return self.getType() == other.getType() && 
                   self.getData() == other.getData() &&
                   self.getDimensions() == other.getDimensions();
        }, "Check equality with another sensory input"),
        
        .def("__ne__", [](const SensoryInput& self, const SensoryInput& other) {
            return !(self == other);
        }, "Check inequality with another sensory input"),
        
        .def("__repr__", [](const SensoryInput& s) {
            return "<SensoryInput type=\"" + std::string(s.getType()) + 
                   "\" dim=" + std::to_string(s.getDimensions()) +
                   " ts=" + std::to_string(s.getTimestamp()) + ">";
        });
    
    m.attr("sensoryInputClass") = sensoryInputClass;

    // ========== MOTOR OUTPUT ==========
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ========== SENSORY PERCEPT ==========
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // ========== WORLD ==========
    
    py::class_<SimpleWorld> simpleWorldClass(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        
        // === CONVENIENCE METHODS ===
        
        // World state management
        .def("resetAndInitialize", [](SimpleWorld& self, float width, float height, size_t visionWidth, size_t visionHeight) {
            self.reset();
            self.configure(width, height, visionWidth, visionHeight);
            self.setAgentStart(width * 0.5f, height * 0.5f); // Default center position
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Reset and reconfigure world in one call")
        
        .def("updateWithPhysics", [](SimpleWorld& self, double timestep, bool checkCollisions = true) {
            if (checkCollisions) {
                self.checkCollisions(); // If available in implementation
            }
            self.update(timestep);
        }, py::arg("timestep"), py::arg("checkCollisions") = true, "Update world state with optional collision detection")
        
        // Agent convenience methods
        .def("setAgentPosition", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position (alias for setAgentStart)")
        .def("getAgentPosition", [](const SimpleWorld& self) {
            return std::make_pair(self.getAgentBody().x, self.getAgentBody().y);
        }, "Get agent position as (x, y) tuple")
        .def("setAgentPositionTuple", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent position using tuple-style call")
        
        // World object management convenience
        .def("addResource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, "Add a resource object at position")
        
        .def("addHazard", [](SimpleWorld& self, float x, float y, float value = -1.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = -1.0f, "Add a hazard object at position")
        
        .def("addWall", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Add a wall object at position")
        
        .def("getObjectsByType", [](SimpleWorld& self, WorldObjectType type) {
            py::list result;
            for (auto& obj : self.getObjects()) {  // Note: getObjects() may need to be added to SimpleWorld
                if (obj.type == type) {
                    result.append(py::cast(obj, py::return_value_policy::copy));
                }
            }
            return result;
        }, py::arg("type"), "Get all world objects of a specific type")
        
        // Vision and perception convenience methods
        .def("getVisionMatrix", [](SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return percept.getVision(); // Assuming getVision() method exists
        }, "Get vision data as matrix (if available)")
        
        .def("hasVisionData", [](const SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return !percept.getVision().getData().empty();
        }, "Check if vision data is available")
        
        // World statistics and analysis
        .def("getWorldStats", [](SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["max_energy"] = self.getMaxEnergy();
            stats["energy_decay_rate"] = self.getEnergyDecayRate();
            stats["simulation_time"] = self.getSimulationTime();
            stats["random_seed"] = static_cast<long>(self.getRandomSeed());
            stats["agent_x"] = self.getAgentBody().x;
            stats["agent_y"] = self.getAgentBody().y;
            stats["agent_orientation"] = self.getAgentBody().orientation;
            stats["agent_energy"] = self.getAgentBody().energy;
            stats["agent_health"] = self.getAgentBody().health;
            stats["is_moving"] = self.getAgentBody().isMoving;
            stats["is_turning"] = self.getAgentBody().isTurning;
            return stats;
        }, "Get comprehensive world statistics")
        
        // Simulation utilities
        .def("stepToTime", [](SimpleWorld& self, double targetTime, double timestep = 0.1) {
            py::list results;
            double currentTime = self.getSimulationTime();
            while (currentTime < targetTime) {
                self.update(timestep);
                currentTime = self.getSimulationTime();
                
                py::dict result;
                result["time"] = currentTime;
                result["agent_x"] = self.getAgentBody().x;
                result["agent_y"] = self.getAgentBody().y;
                result["agent_energy"] = self.getAgentBody().energy;
                results.append(result);
                
                if (currentTime >= targetTime) break;
            }
            return results;
        }, py::arg("targetTime"), py::arg("timestep") = 0.1, "Step world to a specific time and return history")
        
        .def("simulateToEnergy", [](SimpleWorld& self, float targetEnergy, double timestep = 0.1) {
            py::list results;
            double currentTime = self.getSimulationTime();
            while (self.getAgentBody().energy > targetEnergy) {
                self.update(timestep);
                currentTime = self.getSimulationTime();
                
                py::dict result;
                result["time"] = currentTime;
                result["agent_energy"] = self.getAgentBody().energy;
                result["agent_health"] = self.getAgentBody().health;
                results.append(result);
                
                if (self.getAgentBody().energy <= targetEnergy) break;
            }
            return results;
        }, py::arg("targetEnergy"), py::arg("timestep") = 0.1, "Simulate world until agent reaches target energy")
        
        // Utility methods for common operations
        .def("addRandomObjects", [](SimpleWorld& self, size_t count, float radius = 0.2f) {
            for (size_t i = 0; i < count; ++i) {
                float x = self.getWidth() * (static_cast<float>(rand()) / RAND_MAX);
                float y = self.getHeight() * (static_cast<float>(rand()) / RAND_MAX);
                if (self.isValidPosition(x, y)) {
                    float typeValue = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f; // Random between -1 and 1
                    WorldObject obj(x, y, WorldObjectType::Marker, typeValue, radius);
                    self.addObject(obj);
                }
            }
        }, py::arg("count"), py::arg("radius") = 0.2f, "Add random objects to the world")
        
        .def("clearWorld", [](SimpleWorld& self) {
            self.reset();
            // Clear agent and objects
            self.getAgentBody().reset();
        }, "Clear world and reset agent state")
        
        .def("isAtGoal", [](SimpleWorld& self, float goalX, float goalY, float tolerance = 0.5f) {
            float dx = self.getAgentBody().x - goalX;
            float dy = self.getAgentBody().y - goalY;
            return (dx * dx + dy * dy) <= (tolerance * tolerance);
        }, py::arg("goalX"), py::arg("goalY"), py::arg("tolerance") = 0.5f, "Check if agent is at goal position within tolerance")
        
        .def("getDistanceTo", [](SimpleWorld& self, float targetX, float targetY) {
            float dx = self.getAgentBody().x - targetX;
            float dy = self.getAgentBody().y - targetY;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("targetX"), py::arg("targetY"), "Get distance from agent to target position")
        
        .def("getDirectionTo", [](SimpleWorld& self, float targetX, float targetY) {
            float dx = targetX - self.getAgentBody().x;
            float dy = targetY - self.getAgentBody().y;
            float angle = std::atan2(dy, dx) * 180.0f / M_PI;
            return angle;
        }, py::arg("targetX"), py::arg("targetY"), "Get direction to target position in degrees")
        
        .def("hasLineOfSight", [](SimpleWorld& self, float targetX, float targetY) {
            // Simple ray casting check
            float agentX = self.getAgentBody().x;
            float agentY = self.getAgentBody().y;
            float distance = getDistanceTo(self, targetX, targetY); // Using the method above
            
            // Check if line intersects any wall
            auto walls = getObjectsByType(self, WorldObjectType::Wall);  // Using the method above
            for (auto& wall : walls) {
                float wallX = wall.x;
                float wallY = wall.y;
                float wallRadius = wall.radius;
                
                float dx = targetX - agentX;
                float dy = targetY - agentY;
                float lineLen = std::sqrt(dx * dx + dy * dy);
                if (lineLen == 0.0f) return true;
                
                float t = ((targetX - wallX) * dx + (targetY - wallY) * dy) / (lineLen * lineLen);
                if (t < 0.0f || t > 1.0f) continue;
                
                float closestX = agentX + t * dx;
                float closestY = agentY + t * dy;
                float distSq = (closestX - wallX) * (closestX - wallX) + 
                              (closestY - wallY) * (closestY - wallY);
                
                if (distSq <= wallRadius * wallRadius) {
                    return false; // Blocked by wall
                }
            }
            return true;
        }, py::arg("targetX"), py::arg("targetY"), "Check if agent has line of sight to target (simple ray casting)")
        
        .def("getNavigationPath", [](SimpleWorld& self, float targetX, float targetY, size_t maxSteps = 100) {
            py::list path;
            float currentX = self.getAgentBody().x;
            float currentY = self.getAgentBody().y;
            
            for (size_t i = 0; i < maxSteps; ++i) {
                path.append(std::make_pair(currentX, currentY));
                
                if (std::abs(currentX - targetX) < 0.1f && std::abs(currentY - targetY) < 0.1f) {
                    break; // Reached target
                }
                
                float dx = targetX - currentX;
                float dy = targetY - currentY;
                float distance = std::sqrt(dx * dx + dy * dy);
                
                if (distance < 0.1f) break;
                
                // Simple move toward target
                float stepX = dx / distance * 0.1f;
                float stepY = dy / distance * 0.1f;
                float newX = currentX + stepX;
                float newY = currentY + stepY;
                
                if (!self.isValidPosition(newX, newY)) {
                    break; // Can't move further
                }
                
                currentX = newX;
                currentY = newY;
            }
            
            return path;
        }, py::arg("targetX"), py::arg("targetY"), py::arg("maxSteps") = 100, "Get simple navigation path to target")
        
        // Error handling and validation
        .def("validateWorld", [](SimpleWorld& self) {
            py::dict validation;
            validation["valid"] = self.getWidth() > 0 && self.getHeight() > 0;
            validation["agent_position_valid"] = self.isValidPosition(self.getAgentBody().x, self.getAgentBody().y);
            validation["agent_energy_valid"] = self.getAgentBody().energy >= 0 && self.getAgentBody().energy <= self.getMaxEnergy();
            validation["agent_health_valid"] = self.getAgentBody().health >= 0 && self.getAgentBody().health <= 100;
            
            py::list warnings;
            if (self.getAgentBody().energy < self.getMaxEnergy() * 0.1f) {
                warnings.append("Agent energy is very low");
            }
            if (self.getAgentBody().health < 50.0f) {
                warnings.append("Agent health is low");
            }
            
            validation["warnings"] = warnings;
            return validation;
        }, "Validate world state and return validation results")
        
        // Type safety utilities
        .def("getObjectAtPosition", [](SimpleWorld& self, float x, float y) -> py::object {
            auto objects = self.getObjects();  // Note: this might need to be added
            for (auto& obj : objects) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                if (dx * dx + dy * dy <= obj.radius * obj.radius) {
                    return py::cast(obj, py::return_value_policy::copy);
                }
            }
            return py::none();
        }, py::arg("x"), py::arg("y"), "Get object at position (returns None if none)")
        
        .def("getObjectsInRange", [](SimpleWorld& self, float x, float y, float range) {
            py::list result;
            auto objects = self.getObjects();  // Note: this might need to be added
            for (auto& obj : objects) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                float distSq = dx * dx + dy * dy;
                if (distSq <= range * range) {
                    result.append(py::cast(obj, py::return_value_policy::copy));
                }
            }
            return result;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Get all objects within range of position")
        
        .def("countObjectsByType", [](SimpleWorld& self, WorldObjectType type) {
            auto objects = self.getObjects();  // Note: this might need to be added
            size_t count = 0;
            for (auto& obj : objects) {
                if (obj.type == type) count++;
            }
            return count;
        }, py::arg("type"), "Count objects of a specific type")
        
        .def("totalWorldMass", [](SimpleWorld& self) {
            auto objects = self.getObjects();  // Note: this might need to be added
            float totalMass = 0.0f;
            for (auto& obj : objects) {
                totalMass += std::abs(obj.value); // Use absolute value of value as mass proxy
            }
            return totalMass;
        }, "Calculate total mass of all objects in world")
        
        .def("__repr__", [](const SimpleWorld& w) {
            return "<SimpleWorld width=" + std::to_string(w.getWidth()) +
                   " height=" + std::to_string(w.getHeight()) +
                   " agents=" + std::to_string(1) +
                   " objects=" + std::to_string(w.getObjects().size()) + ">";  // Note: getObjects() might need to be added
        });

    // ========== NEURON ==========
    
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Neuron class representing a single neuron with LIF dynamics)pbdoc")
        // Default constructor
        .def(py::init<NeuronId>(), py::arg("id") = NeuronId())
        
        // Factory methods
        .def_static("create", [](NeuronId id) {
            return std::make_unique<Neuron>(id);
        }, py::arg("id"), "Create a neuron with given ID")
        .def_static("createExcitatory", [](NeuronId id) {
            auto neuron = std::make_unique<Neuron>(id);
            neuron->setType(NeuronType::Excitatory);
            return neuron;
        }, py::arg("id"), "Create an excitatory neuron")
        .def_static("createInhibitory", [](NeuronId id) {
            auto neuron = std::make_unique<Neuron>(id);
            neuron->setType(NeuronType::Inhibitory);
            return neuron;
        }, py::arg("id"), "Create an inhibitory neuron")
        
        // Identity
        .def("getId", &Neuron::getId)
        .def("getType", &Neuron::getType)
        .def("setType", &Neuron::setType, py::arg("type"))
        
        // State access
        .def("getState", &Neuron::getState, py::return_value_policy::reference_internal)
        .def("resetState", &Neuron::reset)
        
        // Membrane potential (LIF dynamics)
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"))
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"))
        .def("getThreshold", &Neuron::getThreshold)
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"))
        
        // Firing state
        .def("isFiring", &Neuron::isFiring)
        .def("isRefractory", &Neuron::isRefractory)
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"))
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"))
        .def("decrementRefractory", &Neuron::decrementRefractory)
        
        // Firing rate (for rate-based computation)
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"))
        
        // LIF parameters
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"))
        .def("getLeakConductance", &Neuron::getLeakConductance)
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"))
        .def("getRestingPotential", &Neuron::getRestingPotential)
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"))
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod)
        
        // Spike detection
        .def("checkThreshold", &Neuron::checkThreshold)
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime)
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"))
        .def("getSpikeHistory", &Neuron::getSpikeHistory)
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory)
        
        // Incoming signals (post-synaptic potentials)
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"))
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"))
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"))
        
        // Current injection (from external sources)
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"))
        .def("getTotalCurrent", &Neuron::getTotalCurrent)
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent)
        
        // Synapse management (incoming and outgoing)
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"))
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"))
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses)
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses)
        
        // Plasticity state
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, py::return_value_policy::reference_internal)
        .def("enablePlasticity", &Neuron::enablePlasticity, 
             py::arg("hebbian") = false, py::arg("stdp") = false, py::arg("reward_modulated") = false,
             "Enable plasticity rules")
        
        // Region/population membership
        .def("getRegionId", &Neuron::getRegionId)
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"))
        .def("getPopulationId", &Neuron::getPopulationId)
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"))
        
        // Simulation
        .def("step", static_cast<bool (Neuron::*)(Timestamp, TimestepDuration)>(&Neuron::stepLIF),
             py::arg("currentTime"), py::arg("dt") = 0.001,
             "Perform a LIF simulation step (returns True if neuron fired)")
        .def("stepDefault", &Neuron::step, py::arg("currentTime"),
             "Perform a simulation step with default timestep")
        
        // Statistics
        .def("getMembranePotentialStats", [](const Neuron& self) {
            const auto& state = self.getState();
            return std::make_tuple(state.membranePotential, state.firingRate, self.isFiring());
        }, "Get neuron membrane potential, firing rate, and firing state")
        
        // Convenience methods
        .def("isActive", &Neuron::isFiring, "Check if neuron is currently firing")
        .def("getAdaptationVariable", [] (Neuron& self) { return self.getState().adaptationVariable; })
        .def("setAdaptationVariable", [] (Neuron& self, float value) { self.getState().adaptationVariable = value; }, py::arg("value"))
        
        .def("__repr__", [](const Neuron& n) {
            const auto& state = n.getState();
            return "<Neuron id=" + std::to_string(n.getId().value) + 
                   " type=" + std::to_string(static_cast<int>(n.getType())) +
                   " potential=" + std::to_string(state.membranePotential) + ">";
        });

    // ========== SYNAPSE ==========
    
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synapse representing a connection between neurons with synaptic transmission)pbdoc")
        // Factory methods
        .def_static("create", [](SynapseId id, NeuronId source, NeuronId destination) {
            return std::make_unique<Synapse>(id, source, destination);
        }, py::arg("id"), py::arg("source"), py::arg("destination"), "Create a synapse")
        .def_static("createExcitatory", [](SynapseId id, NeuronId source, NeuronId destination) {
            auto synapse = std::make_unique<Synapse>(id, source, destination);
            synapse->setType(SynapseType::Excitatory);
            return synapse;
        }, py::arg("id"), py::arg("source"), py::arg("destination"), "Create an excitatory synapse")
        .def_static("createInhibitory", [](SynapseId id, NeuronId source, NeuronId destination) {
            auto synapse = std::make_unique<Synapse>(id, source, destination);
            synapse->setType(SynapseType::Inhibitory);
            return synapse;
        }, py::arg("id"), py::arg("source"), py::arg("destination"), "Create an inhibitory synapse")
        
        // Identity
        .def("getId", &Synapse::getId)
        .def("getSourceNeuron", &Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron)
        
        // Weight management
        .def("getWeight", &Synapse::getWeight)
        .def("setWeight", &Synapse::setWeight, py::arg("weight"))
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"))
        
        // Delay
        .def("getDelay", &Synapse::getDelay)
        .def("setDelay", &Synapse::setDelay, py::arg("delay"))
        
        // Type
        .def("getType", &Synapse::getType)
        .def("setType", &Synapse::setType, py::arg("type"))
        .def("isExcitatory", &Synapse::isExcitatory)
        .def("isInhibitory", &Synapse::isInhibitory)
        
        // Activity history for plasticity
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"))
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"))
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory)
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory)
        .def("clearHistory", &Synapse::clearHistory)
        
        // Plasticity flags
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, py::return_value_policy::reference_internal)
        .def("enablePlasticity", &Synapse::enablePlasticity,
             py::arg("hebbian") = false, py::arg("stdp") = false, py::arg("reward_modulated") = false,
             "Enable plasticity rules")
        
        // Eligibility trace (for reward-modulated learning)
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"))
        
        // Synaptic efficacy (use-dependent modulation)
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        
        // Simulation
        .def("step", &Synapse::step, py::arg("currentTime"),
             "Update synapse for one simulation step")
        
        // Statistics
        .def("getStrengthStats", [](const Synapse& s) {
            return std::make_tuple(s.getWeight(), s.getEfficacy(), s.getEligibilityTrace());
        }, "Get synapse weight, efficacy, and eligibility trace")
        
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse id=" + std::to_string(s.getId().value) + 
                   " source=" + std::to_string(s.getSourceNeuron().value) +
                   " dest=" + std::to_string(s.getDestinationNeuron().value) +
                   " weight=" + std::to_string(s.getWeight()) + ">";
        });

    // ========== PLASTICITY RULES ==========
    
    // Base PlasticityRule class
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base class for synaptic plasticity rules)pbdoc")
        .def("isEnabled", &PlasticityRule::isEnabled)
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"))
        .def("getName", &PlasticityRule::getName)
        .def("__repr__", [](const PlasticityRule& rule) {
            return "<PlasticityRule: " + std::string(rule.getName()) + ">";
        });
    
    // STDP plasticity rule
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity rule)pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), "Set long-term potentiation weight")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get long-term potentiation weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), "Set long-term depression weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get long-term depression weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"), "Set STDP time constant (ms)")
        .def("getTimeConstant", &STDP::getTimeConstant, "Get STDP time constant (ms)")
        .def("configure", &STDP::configure, 
             py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP parameters from config values")
        .def_static("createStandardSTDP", []() {
            auto stdp = std::make_unique<STDP>();
            stdp->configure(0.01f, 0.01f, 20.0f); // Default parameters
            return stdp;
        }, "Create STDP with standard parameters")
        .def_static("createFastSTDP", []() {
            auto stdp = std::make_unique<STDP>();
            stdp->configure(0.05f, 0.05f, 10.0f); // Fast plasticity
            return stdp;
        }, "Create STDP with fast parameters")
        .def_static("createSlowSTDP", []() {
            auto stdp = std::make_unique<STDP>();
            stdp->configure(0.005f, 0.005f, 50.0f); // Slow plasticity
            return stdp;
        }, "Create STDP with slow parameters");
    
    // Reward-modulated STDP
    py::class_<RewardModulatedSTDP, PlasticityRule>(m, "RewardModulatedSTDP", R"pbdoc(Reward-modulated STDP (R-STDP) rule)pbdoc")
        .def(py::init<>())
        .def_static("createRSTDPLearner", []() {
            return std::make_unique<RewardModulatedSTDP>();
        }, "Create R-STDP learner for reward-modulated learning");
    
    // Triplet STDP
    py::class_<TripletSTDP, PlasticityRule>(m, "TripletSTDP", R"pbdoc(Triplet STDP with triplet interactions)pbdoc")
        .def(py::init<>())
        .def_static("createTripletSTDP", []() {
            return std::make_unique<TripletSTDP>();
        }, "Create triplet STDP rule");
    
    // Hebbian rule
    py::class_<HebbianRule, PlasticityRule>(m, "HebbianRule", R"pbdoc(Hebbian plasticity rule: 'neurons that fire together, wire together')pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &HebbianRule::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getLearningRate", &HebbianRule::getLearningRate, "Get learning rate")
        .def_static("createHebbian", []() {
            auto hebbian = std::make_unique<HebbianRule>();
            hebbian->setLearningRate(0.01f);
            return hebbian;
        }, "Create Hebbian rule with standard learning rate");
    
    // Anti-Hebbian rule
    py::class_<AntiHebbianRule, PlasticityRule>(m, "AntiHebbianRule", R"pbdoc(Anti-Hebbian plasticity rule: decrease weight when neurons fire together)pbdoc")
        .def(py::init<>())
        .def_static("createAntiHebbian", []() {
            return std::make_unique<AntiHebbianRule>();
        }, "Create Anti-Hebbian rule");
    
    // BCM rule
    py::class_<BCMRule, PlasticityRule>(m, "BCMRule", R"pbdoc(Bienenstock-Cooper-Munro (BCM) plasticity rule)pbdoc")
        .def(py::init<>())
        .def_static("createBCM", []() {
            return std::make_unique<BCMRule>();
        }, "Create BCM rule");

    // ========== NEURAL POPULATION ==========
    
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)pbdoc")
        // Factory methods
        .def_static("create", [](PopulationId id, size_t size) {
            return std::make_unique<NeuralPopulation>(id, size);
        }, py::arg("id"), py::arg("size"), "Create a neural population")
        .def_static("createExcitatoryPopulation", [](PopulationId id, size_t size) {
            auto population = std::make_unique<NeuralPopulation>(id, size);
            population->setNeuronType(NeuronType::Excitatory);
            return population;
        }, py::arg("id"), py::arg("size"), "Create an excitatory population")
        
        // Identity
        .def("getId", &NeuralPopulation::getId)
        
        // Size
        .def("getSize", &NeuralPopulation::getSize)
        .def("isEmpty", &NeuralPopulation::isEmpty)
        
        // Neuron access
        .def("getNeuron", static_cast<Neuron* (NeuralPopulation::*)(NeuronIndex)>(&NeuralPopulation::getNeuron),
             py::return_value_policy::reference_internal, "Get neuron by index")
        .def("getNeuronCount", &NeuralPopulation::getSize)
        
        // Add neuron to population
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"))
        
        // Get all neurons
        .def("getNeurons", &NeuralPopulation::getNeurons, py::return_value_policy::reference_internal)
        
        // Population type
        .def("getNeuronType", &NeuralPopulation::getNeuronType)
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"))
        
        // Statistics
        .def("getActiveCount", &NeuralPopulation::getActiveCount)
        .def("getFiringCount", &NeuralPopulation::getFiringCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel)
        
        // Statistics tracking
        .def("recordActivity", &NeuralPopulation::recordActivity)
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential)
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential)
        
        // Simulation
        .def("step", &NeuralPopulation::step, py::arg("currentTime"),
             "Step all neurons in population")
        
        // Initialization
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"))
        
        // Get neuron IDs
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds)
        
        .def("__repr__", [](const NeuralPopulation& pop) {
            return "<Population id=" + std::to_string(pop.getId().value) +
                   " size=" + std::to_string(pop.getSize()) +
                   " type=" + std::to_string(static_cast<int>(pop.getNeuronType())) + ">";
        });

    // ========== NEURAL REGION ==========
    
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing multiple populations and local connectivity)pbdoc")
        // Factory methods
        .def(py::init<RegionId>(), py::arg("id"), "Create a neural region")
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"), "Create a neural region with name")
        
        // Identity
        .def("getId", &NeuralRegion::getId)
        .def("getName", &NeuralRegion::getName)
        .def("setName", &NeuralRegion::setName, py::arg("name"))
        
        // Population management
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal,
             "Add a new population to the region")
        .def("getPopulation", static_cast<NeuralPopulation* (NeuralRegion::*)(PopulationId)>(&NeuralRegion::getPopulation),
             py::return_value_policy::reference_internal, "Get population by ID")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("getPopulations", &NeuralRegion::getPopulations, py::return_value_policy::reference_internal)
        
        // Synapse management (local connections within this region)
        .def("addSynapse", &NeuralRegion::addSynapse,
             py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add a synapse connecting two neurons")
        .def("getSynapse", static_cast<Synapse* (NeuralRegion::*)(SynapseId)>(&NeuralRegion::getSynapse),
             py::return_value_policy::reference_internal, "Get synapse by ID")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getSynapses", &NeuralRegion::getSynapses, py::return_value_policy::reference_internal)
        
        // Connectivity queries
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"), "Get all synapses originating from a neuron")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"), "Get all synapses terminating at a neuron")
        
        // Statistics
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount)
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount)
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate)
        
        // Local connectivity statistics
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight)
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity)
        
        // Step all populations and synapses
        .def("step", &NeuralRegion::step, py::arg("currentTime"), "Step all components in the region")
        
        // Get all neurons across all populations
        .def("getAllNeurons", static_cast<std::vector<Neuron*> (NeuralRegion::*)()>(&NeuralRegion::getAllNeurons),
             py::return_value_policy::reference_internal, "Get all neurons in region")
        .def("getAllNeuronsConst", static_cast<std::vector<const Neuron*> (NeuralRegion::*)() const>(&NeuralRegion::getAllNeurons),
             "Get all neurons in region (const)")
        
        // Statistics for region-level analysis
        .def("getRegionStats", [](const NeuralRegion& region) {
            std::map<std::string, double> stats;
            stats["total_neurons"] = region.getTotalNeuronCount();
            stats["active_neurons"] = region.getActiveNeuronCount();
            stats["firing_neurons"] = region.getFiringNeuronCount();
            stats["average_firing_rate"] = region.getAverageFiringRate();
            stats["synaptic_density"] = region.getSynapticDensity();
            stats["population_count"] = region.getPopulationCount();
            return stats;
        }, "Get comprehensive region statistics")
        
        // Simulation
        .def("reset", &NeuralRegion::reset, "Reset all neurons and synapses")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity,
             py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"),
             "Initialize random connectivity within region")
        
        .def("__repr__", [](const NeuralRegion& region) {
            return "<Region id=" + std::to_string(region.getId().value) +
                   " name=\"" + region.getName() + "\" " +
                   "neurons=" + std::to_string(region.getTotalNeuronCount()) +
                   " populations=" + std::to_string(region.getPopulationCount()) + ">";
        });

    // ========== BRAIN ==========
    
    py::class_<Brain> brainClass(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        
        // Factory methods
        .def_static("create", [](std::shared_ptr<Config> config) {
            return std::make_unique<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration")
        .def_static("createWithDefaultConfig", []() {
            auto config = std::make_shared<Config>();
            return std::make_unique<Brain>(config);
        }, "Create a brain with default configuration")
        
        // Initialization
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        
        // Simulation steps
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        
        // Input/Output
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        
        // State management
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        
        // Region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        
        // Statistics
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        
        // Development
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        
        // Accessors
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        
        // Plasticity system access
        .def("getSTDP", &Brain::getSTDP, py::return_value_policy::reference_internal, "Get STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian, py::return_value_policy::reference_internal, "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity, py::return_value_policy::reference_internal, "Get structural plasticity rule")
        
        // Memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory, py::return_value_policy::reference_internal, "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory, py::return_value_policy::reference_internal, "Get episodic memory")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory, py::return_value_policy::reference_internal, "Get associative memory")
        
        // Prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem, py::return_value_policy::reference_internal, "Get prediction system")
        
        // Cognition systems
        .def("getPlanner", &Brain::getPlanner, py::return_value_policy::reference_internal, "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation, py::return_value_policy::reference_internal, "Get concept formation")
        .def("getAttention", &Brain::getAttention, py::return_value_policy::reference_internal, "Get attentional selection")
        
        // Development system
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem, py::return_value_policy::reference_internal, "Get development system")
        
        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine, py::return_value_policy::reference_internal, "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity, py::return_value_policy::reference_internal, "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty, py::return_value_policy::reference_internal, "Get novelty detection")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal, py::return_value_policy::reference_internal, "Get prediction error signal")
        
        // Simulation utilities
        .def("createSimulationGenerator", [](std::shared_ptr<Brain> brain, size_t steps) {
            return py::generator([brain, steps](size_t i) mutable -> py::object {
                if (i < steps) {
                    brain->step(i, static_cast<Timestamp>(i * 0.001)); // 1ms timestep
                    py::dict result;
                    result["step"] = i;
                    result["spike_count"] = brain->getTotalSpikeCount();
                    result["active_neurons"] = brain->getActiveNeuronCount();
                    return result;
                }
                throw py::stop_iteration();
            });
        }, py::arg("brain"), py::arg("steps"), "Create a generator that yields simulation steps")
        
        // === CONVENIENCE METHODS ===
        
        // Batch operations
        .def("stepBatch", [](Brain& self, size_t steps, double timestep = 0.001) {
            py::list results;
            for (size_t i = 0; i < steps; ++i) {
                Timestamp time = static_cast<Timestamp>(i * timestep);
                self.step(i, time);
                py::dict result;
                result["step"] = i;
                result["spike_count"] = self.getTotalSpikeCount();
                result["active_neurons"] = self.getActiveNeuronCount();
                result["firing_rate"] = self.getAverageFiringRate();
                result["exc_inh_ratio"] = self.getExcitationInhibitionRatio();
                results.append(result);
            }
            return results;
        }, py::arg("steps"), py::arg("timestep") = 0.001, "Run a batch of simulation steps and return results")
        
        .def("resetAndInitialize", [](Brain& self, std::shared_ptr<Config> config) {
            self.reset();
            if (config) {
                self.getConfig() = config;  // Update config if provided
            }
            return self.initialize();
        }, py::arg("config") = nullptr, "Reset and reinitialize brain with optional new config")
        
        // Property access patterns (Pythonic properties)
        .def("get_neurons_count", &Brain::getTotalNeuronCount, "Get total neuron count (convenience alias)")
        .def("get_synapses_count", &Brain::getTotalSynapseCount, "Get total synapse count (convenience alias)")
        .def("get_active_count", &Brain::getActiveNeuronCount, "Get active neuron count (convenience alias)")
        .def("get_firing_count", &Brain::getFiringNeuronCount, "Get firing neuron count (convenience alias)")
        .def("get_firing_rate", &Brain::getAverageFiringRate, "Get average firing rate (convenience alias)")
        
        // Region convenience methods
        .def("get_region_by_name", [](Brain& self, const std::string& name) -> py::object {
            auto regions = self.getRegions();
            for (auto& region : regions) {
                if (region->getName() == name) {
                    return py::cast(region.get(), py::return_value_policy::reference_internal);
                }
            }
            return py::none();
        }, py::arg("name"), "Get region by name (returns None if not found)")
        
        .def("get_regions_by_name", [](Brain& self, const std::string& name) {
            py::list result;
            auto regions = self.getRegions();
            for (auto& region : regions) {
                if (region->getName() == name) {
                    result.append(py::cast(region.get(), py::return_value_policy::reference_internal));
                }
            }
            return result;
        }, py::arg("name"), "Get all regions with matching name")
        
        // System status and health checks
        .def("isHealthy", [](const Brain& self) {
            return self.getTotalNeuronCount() > 0 &&
                   self.getRegionCount() > 0 &&
                   self.getActiveNeuronCount() <= self.getTotalNeuronCount();
        }, "Check if brain is healthy and operational")
        
        .def("getStats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["exc_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["region_count"] = self.getRegionCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics as a dictionary")
        
        // Context manager for simulation steps
        .def("simulate", [](std::shared_ptr<Brain> brain, size_t steps, double timestep) {
            return py::generator([brain, steps, timestep](size_t i) mutable -> py::object {
                if (i < steps) {
                    brain->step(i, static_cast<Timestamp>(i * timestep));
                    py::dict result;
                    result["step"] = i;
                    result["time"] = i * timestep;
                    result["spike_count"] = brain->getTotalSpikeCount();
                    result["active_neurons"] = brain->getActiveNeuronCount();
                    result["firing_rate"] = brain->getAverageFiringRate();
                    return result;
                }
                throw py::stop_iteration();
            });
        }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001, "Create a context manager for simulation")
        
        // Type-safe parameter validation and error handling
        .def("validateParameters", [](Brain& self) {
            py::dict validation;
            validation["valid"] = self.initialize();  // Check if initialized
            validation["neuron_count"] = self.getTotalNeuronCount();
            validation["region_count"] = self.getRegionCount();
            validation["has_plasticity"] = self.getSTDP() != nullptr || self.getHebbian() != nullptr;
            validation["error_messages"] = py::list();
            return validation;
        }, "Validate brain parameters and return validation results")
        
        // Utility methods for common operations
        .def("activateAllNeurons", [](Brain& self) {
            auto regions = self.getRegions();
            for (auto& region : regions) {
                auto neurons = region->getAllNeurons();
                for (auto& neuron : neurons) {
                    neuron->setFiringState(FiringState::Active);
                }
            }
        }, "Activate all neurons (for testing/debugging)")
        
        .def("deactivateAllNeurons", [](Brain& self) {
            auto regions = self.getRegions();
            for (auto& region : regions) {
                auto neurons = region->getAllNeurons();
                for (auto& neuron : neurons) {
                    neuron->setFiringState(FiringState::Resting);
                }
            }
        }, "Deactivate all neurons (for testing/debugging)")
        
        .def("clearSpikeHistory", [](Brain& self) {
            auto regions = self.getRegions();
            for (auto& region : regions) {
                auto neurons = region->getAllNeurons();
                for (auto& neuron : neurons) {
                    neuron->clearSpikeHistory();
                }
                auto synapses = region->getSynapses();
                for (auto& synapse : synapses) {
                    synapse->clearHistory();
                }
            }
        }, "Clear spike history for all neurons and synapses")
        
        .def("__repr__", [](const Brain& b) {
            auto config = b.getConfig();
            std::string configStr = config->summary();
            return "<Brain neurons=" + std::to_string(b.getTotalNeuronCount()) + 
                   " regions=" + std::to_string(b.getRegionCount()) +
                   " active=" + std::to_string(b.getActiveNeuronCount()) + 
                   " spikes=" + std::to_string(b.getTotalSpikeCount()) + ">";
        });
    
    m.attr("brainClass") = brainClass;

    // ========== AGENT BRAIN ==========
    
    py::class_<AgentBrain> agentBrainClass(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // === CONVENIENCE METHODS ===
        
        // Agent lifecycle and initialization convenience
        .def("initializeWithWorld", [](AgentBrain& self, SimpleWorld& world) {
            self.initialize(world);
        }, "Initialize agent with world (convenience alias)")
        
        .def("resetAgent", [](AgentBrain& self) {
            self.reset();
        }, "Reset agent state (convenience alias)")
        
        // Sensory processing convenience
        .def("processVision", [](AgentBrain& self, Vision& vision) {
            SensoryPercept percept;
            percept.setVision(vision);
            self.processSensoryInput(percept);
        }, "Process vision input (convenience wrapper)")
        
        .def("processTouch", [](AgentBrain& self, Touch& touch) {
            SensoryPercept percept;
            percept.setTouch(touch);
            self.processSensoryInput(percept);
        }, "Process touch input (convenience wrapper)")
        
        .def("processInternal", [](AgentBrain& self, InternalSignals& internal) {
            SensoryPercept percept;
            percept.setInternal(internal);
            self.processSensoryInput(percept);
        }, "Process internal signals (convenience wrapper)")
        
        .def("processAudio", [](AgentBrain& self, Audio& audio) {
            SensoryPercept percept;
            percept.setAudio(audio);
            self.processSensoryInput(percept);
        }, "Process audio input (convenience wrapper)")
        
        // Motor decoding convenience
        .def("getMovementCommand", [](AgentBrain& self) {
            MotorCommand cmd = self.decodeMotorCommand();
            return cmd;
        }, "Get movement command (convenience alias)")
        
        .def("getInteractionCommand", [](AgentBrain& self) {
            MotorCommand cmd = self.decodeMotorCommand();
            // Filter for interaction commands
            if (cmd == MotorCommand::Interact || cmd == MotorCommand::Eat || cmd == MotorCommand::Drink) {
                return cmd;
            }
            return MotorCommand::Wait; // Default if not interaction
        }, "Get interaction command (convenience wrapper)")
        
        // Reward processing convenience
        .def("applyReward", [](AgentBrain& self, float reward) {
            self.applyRewardModulation(reward, reward * 0.8f); // Default predicted reward
        }, py::arg("reward"), "Apply reward with default predicted reward (convenience)")
        
        .def("applyRewardWithPrediction", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward with predicted reward (original method)")
        
        // Development convenience
        .def("updateDevelopmentStep", [](AgentBrain& self, double timestep = 1.0) {
            self.updateDevelopment(timestep);
        }, py::arg("timestep") = 1.0, "Update development (convenience alias)")
        
        // Status and monitoring convenience
        .def("getAgentStats", [](AgentBrain& self) {
            py::dict stats;
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["sensory_input_size"] = self.getSensoryInputSize();
            stats["motor_output_size"] = self.getMotorOutputSize();
            
            // System enablement status
            py::dict systems;
            systems["reward_modulation"] = self.isRewardModulationEnabled();
            systems["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            systems["development"] = self.isDevelopmentEnabled();
            systems["curiosity"] = self.isCuriosityEnabled();
            stats["enabled_systems"] = systems;
            
            return stats;
        }, "Get comprehensive agent statistics")
        
        // Behavior control convenience
        .def("enableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all agent features (convenience)")
        
        .def("disableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all agent features (convenience)")
        
        .def("enableLearningMode", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable learning mode (convenience)")
        
        .def("enableExplorationMode", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable exploration mode (convenience)")
        
        // Action sequence processing
        .def("processActionSequence", [](AgentBrain& self, const std::vector<std::pair<std::string, float>>& actions) {
            py::list results;
            for (auto& action : actions) {
                // Parse action string to MotorCommand (simplified)
                MotorCommand cmd = MotorCommand::Wait;
                if (action.first == "move_forward") cmd = MotorCommand::MoveForward;
                else if (action.first == "move_backward") cmd = MotorCommand::MoveBackward;
                else if (action.first == "turn_left") cmd = MotorCommand::TurnLeft;
                else if (action.first == "turn_right") cmd = MotorCommand::TurnRight;
                else if (action.first == "interact") cmd = MotorCommand::Interact;
                else if (action.first == "wait") cmd = MotorCommand::Wait;
                
                // Apply action with corresponding reward
                self.applyRewardModulation(action.second, action.second * 0.8f);
                
                py::dict result;
                result["action"] = static_cast<int>(cmd);
                result["action_name"] = action.first;
                result["reward"] = action.second;
                result["success"] = action.second > 0.0f;
                results.append(result);
            }
            return results;
        }, py::arg("actions"), "Process a sequence of actions with rewards")
        
        // Error handling and validation
        .def("validateAgent", [](AgentBrain& self) {
            py::dict validation;
            validation["valid"] = self.getBrain() != nullptr;
            validation["has_world"] = true; // Would check if world is set
            validation["brain_initialized"] = self.getBrain()->getTotalNeuronCount() > 0;
            validation["sensory_ready"] = self.getSensoryInputSize() > 0;
            validation["motor_ready"] = self.getMotorOutputSize() > 0;
            
            py::list errors;
            if (!validation["valid"].cast<bool>()) {
                errors.append("Brain is not properly initialized");
            }
            if (!validation["brain_initialized"].cast<bool>()) {
                errors.append("Brain has no neurons");
            }
            if (!validation["sensory_ready"].cast<bool>()) {
                errors.append("Sensory system not ready");
            }
            if (!validation["motor_ready"].cast<bool>()) {
                errors.append("Motor system not ready");
            }
            
            validation["errors"] = errors;
            return validation;
        }, "Validate agent state and return validation results")
        
        // Utility methods for common operations
        .def("processPercept", [](AgentBrain& self, const std::vector<float>& visionData, 
                                const std::vector<float>& touchData,
                                const std::vector<float>& internalData,
                                const std::vector<float>& audioData) {
            SensoryPercept percept;
            
            if (!visionData.empty()) {
                Vision vision;
                vision.setData(visionData);
                percept.setVision(vision);
            }
            
            if (!touchData.empty()) {
                Touch touch;
                touch.setData(touchData);
                percept.setTouch(touch);
            }
            
            if (!internalData.empty()) {
                InternalSignals internal;
                for (float val : internalData) internal.addSignal(val);
                percept.setInternal(internal);
            }
            
            if (!audioData.empty()) {
                Audio audio;
                audio.setData(audioData);
                percept.setAudio(audio);
            }
            
            self.processSensoryInput(percept);
        }, py::arg("visionData"), py::arg("touchData"), py::arg("internalData"), py::arg("audioData"), "Process all sensory modalities at once")
        
        .def("getBehavioralProfile", [](AgentBrain& self) {
            py::dict profile;
            profile["curiosity_level"] = self.getCuriosityLevel();
            profile["novelty_sensitivity"] = self.getNoveltyLevel();
            profile["reward_sensitivity"] = self.getNeuromodulationLevel();
            profile["exploration_bias"] = self.getCuriosityLevel() / (self.getCuriosityLevel() + 0.1f);
            profile["exploitation_bias"] = 1.0f - profile["exploration_bias"].cast<float>();
            
            // Development stage influence
            float stage = static_cast<float>(self.getDevelopmentalStage());
            profile["developmental_maturity"] = stage / static_cast<float>(DevelopmentalStage::Aging);
            profile["learning_plasticity"] = 1.0f - profile["developmental_maturity"].cast<float>() * 0.5f;
            
            return profile;
        }, "Get agent's behavioral profile (curiosity, exploration, etc.)")
        
        .def("adaptBehavior", [](AgentBrain& self, float targetCuriosity, float learningRate = 0.1f) {
            float current = self.getCuriosityLevel();
            float adjustment = (targetCuriosity - current) * learningRate;
            
            // Modify neuromodulation to affect curiosity
            // This is a simplified adaptation - in a real system, would modify brain parameters
            if (adjustment > 0.01f) {
                self.enableCuriosity(true);
                self.applyRewardModulation(adjustment * 0.5f, targetCuriosity * 0.8f);
            }
            
            return adjustment;
        }, py::arg("targetCuriosity"), py::arg("learningRate") = 0.1f, "Adapt agent's curiosity level toward target")
        
        .def("getRecommendation", [](AgentBrain& self) {
            py::dict recommendation;
            
            // Based on curiosity and novelty levels
            float curiosity = self.getCuriosityLevel();
            float novelty = self.getNoveltyLevel();
            float reward = self.getNeuromodulationLevel();
            
            if (curiosity > 0.7f && novelty > 0.6f) {
                recommendation["behavior"] = "explore";
                recommendation["priority"] = 0.8f;
                recommendation["reason"] = "High curiosity and novelty suggest exploration";
            } else if (reward > 0.5f) {
                recommendation["behavior"] = "exploit";
                recommendation["priority"] = 0.9f;
                recommendation["reason"] = "High reward suggests exploitation";
            } else if (curiosity > 0.5f) {
                recommendation["behavior"] = "balanced";
                recommendation["priority"] = 0.6f;
                recommendation["reason"] = "Moderate curiosity suggests balance";
            } else {
                recommendation["behavior"] = "conservative";
                recommendation["priority"] = 0.4f;
                recommendation["reason"] = "Low levels suggest conservative approach";
            }
            
            return recommendation;
        }, "Get behavioral recommendation based on current state")
        
        .def("executeActionBasedOnRecommendation", [](AgentBrain& self, const py::dict& recommendation) {
            std::string behavior = recommendation["behavior"].cast<std::string>();
            float priority = recommendation["priority"].cast<float>();
            
            MotorCommand action;
            if (behavior == "explore") {
                action = MotorCommand::Look; // Example: look for new things
            } else if (behavior == "exploit") {
                action = MotorCommand::Wait; // Example: wait/reward for known things
            } else if (behavior == "balanced") {
                action = MotorCommand::MoveForward; // Example: move toward goal
            } else {
                action = MotorCommand::Wait; // Conservative: wait
            }
            
            // Apply action with reward based on priority
            self.applyRewardModulation(priority * 0.5f, priority * 0.8f);
            
            py::dict result;
            result["action"] = static_cast<int>(action);
            result["action_name"] = "move_forward"; // Simplified
            result["priority"] = priority;
            result["applied_reward"] = priority * 0.5f;
            result["behavior"] = behavior;
            
            return result;
        }, py::arg("recommendation"), "Execute action based on recommendation (mock implementation)")
        
        .def("getLearningStatus", [](AgentBrain& self) {
            py::dict status;
            status["reward_modulation_active"] = self.isRewardModulationEnabled();
            status["structural_plasticity_active"] = self.isStructuralPlasticityEnabled();
            status["development_active"] = self.isDevelopmentEnabled();
            status["curiosity_active"] = self.isCuriosityEnabled();
            status["has_motor_neurons"] = self.getMotorOutputSize() > 0;
            status["has_sensory_neurons"] = self.getSensoryInputSize() > 0;
            
            // Calculate learning capacity
            int active_systems = 0;
            if (self.isRewardModulationEnabled()) active_systems++;
            if (self.isStructuralPlasticityEnabled()) active_systems++;
            if (self.isDevelopmentEnabled()) active_systems++;
            if (self.isCuriosityEnabled()) active_systems++;
            
            status["learning_capacity"] = static_cast<float>(active_systems) / 4.0f;
            status["exploration_capacity"] = self.getCuriosityLevel();
            status["exploitation_capacity"] = self.getNeuromodulationLevel();
            
            return status;
        }, "Get agent's current learning status and capacity")
        
        .def("getEnvironmentState", [](AgentBrain& self) {
            py::dict state;
            state["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            state["neuromodulation_level"] = self.getNeuromodulationLevel();
            state["curiosity_level"] = self.getCuriosityLevel();
            state["novelty_level"] = self.getNoveltyLevel();
            state["prediction_error"] = self.getPredictionError();
            state["learning_progress"] = self.getDevelopmentalStage() / static_cast<int>(DevelopmentalStage::Aging);
            
            return state;
        }, "Get agent's current environment state")
        
        // Context manager for agent episodes
        .def("createEpisodeGenerator", [](std::shared_ptr<AgentBrain> agent, size_t episodes, double timestep = 0.1) {
            return py::generator([agent, episodes, timestep](size_t i) mutable -> py::object {
                if (i < episodes) {
                    // Simulate one episode step
                    agent->updateDevelopment(timestep);
                    
                    py::dict result;
                    result["episode"] = i;
                    result["time"] = i * timestep;
                    result["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
                    result["curiosity_level"] = agent->getCuriosityLevel();
                    result["neuromodulation_level"] = agent->getNeuromodulationLevel();
                    result["reward_available"] = agent->getNeuromodulationLevel() * 0.5f;
                    
                    return result;
                }
                throw py::stop_iteration();
            });
        }, py::arg("agent"), py::arg("episodes"), py::arg("timestep") = 0.1, "Create episode generator for agent training")
        
        .def("simulateAgentEpisode", [](std::shared_ptr<AgentBrain> agent, size_t steps, double timestep = 0.1) {
            py::list results;
            for (size_t i = 0; i < steps; ++i) {
                agent->updateDevelopment(timestep);
                
                py::dict result;
                result["step"] = i;
                result["time"] = i * timestep;
                result["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
                result["curiosity_level"] = agent->getCuriosityLevel();
                result["neuromodulation_level"] = agent->getNeuromodulationLevel();
                result["prediction_error"] = agent->getPredictionError();
                results.append(result);
            }
            return results;
        }, py::arg("agent"), py::arg("steps"), py::arg("timestep") = 0.1, "Simulate a complete agent episode")
        
        .def("__repr__", [](const AgentBrain& a) {
            return "<AgentBrain brain=" + std::to_string(reinterpret_cast<uint64_t>(a.getBrain())) +
                   " curiosity=" + std::to_string(a.getCuriosityLevel()) +
                   " neuromodulation=" + std::to_string(a.getNeuromodulationLevel()) + ">";
        });
    
    m.attr("agentBrainClass") = agentBrainClass;

    // ========== FACTORY FUNCTIONS ==========
    
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
    
    // Neural network setup helpers
    m.def("createExcitatoryInhibitoryNetwork", [](size_t num_exc, size_t num_inh, size_t exc_to_exc_prob, size_t inh_to_exc_prob) {
        auto config = std::make_shared<Config>();
        config->set("network.excitatory_neurons", static_cast<int>(num_exc));
        config->set("network.inhibitory_neurons", static_cast<int>(num_inh));
        config->set("network.exc_to_exc_probability", static_cast<double>(exc_to_exc_prob));
        config->set("network.inh_to_exc_probability", static_cast<double>(inh_to_exc_prob));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("num_exc"), py::arg("num_inh"), py::arg("exc_to_exc_prob") = 0.1, py::arg("inh_to_exc_prob") = 0.5,
       "Create a simple E-I network configuration");
    
    m.def("createBalancedNetwork", [](size_t num_neurons, float connection_density) {
        auto config = std::make_shared<Config>();
        config->set("network.neuron_count", static_cast<int>(num_neurons));
        config->set("network.connection_density", static_cast<double>(connection_density));
        config->set("plasticity.stdp.enabled", true);
        config->set("plasticity.hebbian.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("num_neurons"), py::arg("connection_density") = 0.1, "Create a balanced recurrent network");
    
    m.def("createLearningNetwork", [](size_t num_neurons) {
        auto config = std::make_shared<Config>();
        config->set("network.neuron_count", static_cast<int>(num_neurons));
        config->set("plasticity.stdp.enabled", true);
        config->set("plasticity.hebbian.enabled", true);
        config->set("plasticity.reward_modulated.enabled", true);
        config->set("development.enabled", true);
        config->set("curiosity.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("num_neurons"), "Create a network with learning capabilities");
    
    // Plasticity helper functions
    m.def("createSTDPConfig", [](float ltp_weight, float ltd_weight, float time_constant) {
        auto config = std::make_shared<Config>();
        config->set("plasticity.stdp.ltp_weight", static_cast<double>(ltp_weight));
        config->set("plasticity.stdp.ltd_weight", static_cast<double>(ltd_weight));
        config->set("plasticity.stdp.time_constant", static_cast<double>(time_constant));
        return config;
    }, py::arg("ltp_weight") = 0.01f, py::arg("ltd_weight") = 0.01f, py::arg("time_constant") = 20.0f,
       "Create a configuration for STDP plasticity");
    
    m.def("createHebbianConfig", [](float learning_rate) {
        auto config = std::make_shared<Config>();
        config->set("plasticity.hebbian.learning_rate", static_cast<double>(learning_rate));
        return config;
    }, py::arg("learning_rate") = 0.01f, "Create a configuration for Hebbian plasticity");
    
    // Simulation utilities
    m.def("simulateSteps", [](std::shared_ptr<Brain> brain, size_t steps, double timestep) {
        py::list results;
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * timestep);
            py::dict result;
            result["step"] = i;
            result["spike_count"] = brain->getTotalSpikeCount();
            result["active_neurons"] = brain->getActiveNeuronCount();
            result["firing_neurons"] = brain->getFiringNeuronCount();
            result["firing_rate"] = brain->getAverageFiringRate();
            results.append(result);
        }
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
       "Simulate a number of steps and return results as a list of dicts");
    
    m.def("createStepGenerator", [](std::shared_ptr<Brain> brain, size_t steps) {
        return py::generator([brain, steps](size_t i) mutable -> py::object {
            if (i < steps) {
                brain->step(i, static_cast<Timestamp>(i * 0.001));
                py::dict result;
                result["step"] = i;
                result["spike_count"] = brain->getTotalSpikeCount();
                result["active_neurons"] = brain->getActiveNeuronCount();
                result["firing_neurons"] = brain->getFiringNeuronCount();
                result["firing_rate"] = brain->getAverageFiringRate();
                return result;
            }
            throw py::stop_iteration();
        });
    }, py::arg("brain"), py::arg("steps"), "Create a Python generator that yields simulation steps");
    
    // Type conversion utilities
    m.def("toConfigValue", [](const py::object& obj) {
        if (obj.is_none()) return ConfigValue();
        if (py::isinstance<py::int_>(obj)) return static_cast<int>(obj.cast<int64_t>());
        if (py::isinstance<py::float_>(obj)) return obj.cast<double>();
        if (py::isinstance<py::bool_>(obj)) return obj.cast<bool>();
        if (py::isinstance<py::str>(obj)) return obj.cast<std::string>();
        throw std::runtime_error("Unsupported type for ConfigValue conversion");
    }, py::arg("obj"), "Convert Python object to ConfigValue");
    
    m.def("fromConfigValue", [](const ConfigValue& value, const std::string& typeHint = "") {
        return py::object(); // Placeholder implementation
    }, py::arg("value"), py::arg("typeHint") = "", "Convert ConfigValue to Python object");
    
    // Error handling
    m.def("getBrainError", [](const Brain& brain) {
        return "Brain simulation error"; // Placeholder implementation
    }, py::arg("brain"), "Get error message from brain");
    
    m.def("validateConfig", [](const Config& config) {
        return config.summary(); // For now, just return summary as validation
    }, py::arg("config"), "Validate configuration and return summary");

    // ========== CONSTANTS ==========
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
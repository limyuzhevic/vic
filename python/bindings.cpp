#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Python exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Config error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("World error: " + msg) {}
};

class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& msg) : NLMError("Agent error: " + msg) {}
};

// Helper function for JSON serialization
static std::string to_json_string(const nlm::Config& config) {
    std::ostringstream oss;
    oss << "{\n";
    auto keys = config.getKeys();
    for (size_t i = 0; i < keys.size(); ++i) {
        const auto& key = keys[i];
        oss << "  \"" << key << "\": ";
        
        if (key.find("enabled") != std::string::npos || key.find("flag") != std::string::npos) {
            // Boolean values
            auto val = config.get<bool>(key);
            oss << (val.has_value() && val.value() ? "true" : "false");
        } else if (key.find("count") != std::string::npos || key.find("number") != std::string::npos) {
            // Integer values
            auto val = config.get<int64_t>(key);
            oss << (val.has_value() ? std::to_string(val.value()) : "0");
        } else if (key.find("rate") != std::string::npos || key.find("time") != std::string::npos || 
                   key.find("delay") != std::string::npos) {
            // Float values
            auto val = config.get<double>(key);
            oss << (val.has_value() ? std::to_string(val.value()) : "0.0");
        } else if (key.find("path") != std::string::npos || key.find("file") != std::string::npos || 
                   key.find("name") != std::string::npos) {
            // String values
            auto val = config.get<std::string>(key);
            oss << "\"" << (val.has_value() ? val.value() : "") << "\"";
        } else {
            // Default to string representation
            auto val = config.get<std::string>(key);
            if (!val.has_value()) {
                oss << "null";
            } else {
                // Try to parse as numeric first
                try {
                    size_t pos;
                    double num = std::stod(val.value(), pos);
                    if (pos == val.value().size()) {
                        oss << num;
                    } else {
                        oss << "\"" << val.value() << "\"";
                    }
                } catch (...) {
                    oss << "\"" << val.value() << "\"";
                }
            }
        }
        
        if (i < keys.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    oss << "}";
    return oss.str();
}

// Helper function for pretty-printing configuration
static std::string format_config_summary(const nlm::Config& config) {
    auto keys = config.getKeys();
    std::ostringstream oss;
    oss << "Configuration Summary (" << keys.size() << " entries):\n";
    
    for (const auto& key : keys) {
        oss << "  " << key << ": ";
        
        if (key.find("enabled") != std::string::npos || key.find("flag") != std::string::npos) {
            auto val = config.get<bool>(key);
            oss << (val.has_value() && val.value() ? "✓" : "✗") << " (bool)";
        } else if (key.find("count") != std::string::npos || key.find("number") != std::string::npos) {
            auto val = config.get<int64_t>(key);
            oss << (val.has_value() ? std::to_string(val.value()) : "?") << " (int)";
        } else if (key.find("rate") != std::string::npos || key.find("time") != std::string::npos || 
                   key.find("delay") != std::string::npos || key.find("factor") != std::string::npos) {
            auto val = config.get<double>(key);
            oss << (val.has_value() ? std::to_string(val.value()) : "?") << " (float)";
        } else {
            auto val = config.get<std::string>(key);
            oss << (val.has_value() ? val.value() : "?") << " (string)";
        }
        oss << "\n";
    }
    
    return oss.str();
}

// Helper function to get all configuration values as a Python dictionary
static py::dict config_to_dict(const nlm::Config& config) {
    py::dict result;
    auto keys = config.getKeys();
    
    for (const auto& key : keys) {
        // Try to get appropriate Python type
        auto int_val = config.get<int64_t>(key);
        if (int_val.has_value()) {
            result[key] = int_val.value();
            continue;
        }
        
        auto double_val = config.get<double>(key);
        if (double_val.has_value()) {
            result[key] = double_val.value();
            continue;
        }
        
        auto bool_val = config.get<bool>(key);
        if (bool_val.has_value()) {
            result[key] = bool_val.value();
            continue;
        }
        
        auto string_val = config.get<std::string>(key);
        if (string_val.has_value()) {
            result[key] = string_val.value();
            continue;
        }
        
        // Unknown type - use string representation
        result[key] = "<unknown>";
    }
    
    return result;
}

// Helper function to create configuration from Python dict
static std::shared_ptr<nlm::Config> dict_to_config(const py::dict& py_dict) {
    auto config = std::make_shared<nlm::Config>();
    
    for (auto item : py_dict) {
        std::string key = py::str(item.first);
        
        if (item.second.is(py::bool_())) {
            config->set(key, py::bool_(item.second));
        } else if (item.second.is(py::int_())) {
            config->set(key, py::int_(item.second));
        } else if (item.second.is(py::float_())) {
            config->set(key, py::float_(item.second));
        } else if (item.second.is(py::str())) {
            config->set(key, py::str(item.second));
        } else if (item.second.is(py::list())) {
            // Convert list to vector based on content
            py::list py_list = item.second.cast<py::list>();
            if (!py_list.empty()) {
                if (py_list[0].is(py::int_())) {
                    std::vector<int64_t> int_vec;
                    for (auto elem : py_list) {
                        int_vec.push_back(elem.cast<int64_t>());
                    }
                    config->set(key, int_vec);
                } else if (py_list[0].is(py::float_())) {
                    std::vector<double> double_vec;
                    for (auto elem : py_list) {
                        double_vec.push_back(elem.cast<double>());
                    }
                    config->set(key, double_vec);
                } else {
                    std::vector<std::string> string_vec;
                    for (auto elem : py_list) {
                        string_vec.push_back(elem.cast<std::string>());
                    }
                    config->set(key, string_vec);
                }
            }
        }
    }
    
    return config;
}

// Advanced configuration validation
static bool validate_config(const nlm::Config& config, const std::vector<std::string>& required_keys) {
    bool valid = true;
    std::ostringstream errors;
    
    for (const auto& key : required_keys) {
        if (!config.has(key)) {
            errors << "Missing required key: '" << key << "'\n";
            valid = false;
        }
    }
    
    // Check for negative values where appropriate
    if (config.has("brain.neuron_count") && config.getOr<int64_t>("brain.neuron_count", 0) < 0) {
        errors << "brain.neuron_count cannot be negative\n";
        valid = false;
    }
    
    if (config.has("plasticity.learning_rate") && config.getOr<double>("plasticity.learning_rate", 1.0) < 0.0) {
        errors << "plasticity.learning_rate cannot be negative\n";
        valid = false;
    }
    
    if (!valid) {
        throw ConfigError("Configuration validation failed:\n" + errors.str());
    }
    
    return true;
}

// Batch operations helper
static void batch_inject_current(std::shared_ptr<nlm::Brain> brain, 
                                  const std::vector<std::string>& neuron_types,
                                  const std::vector<double>& current_values) {
    if (neuron_types.size() != current_values.size()) {
        throw BrainError("neuron_types and current_values must have same size for batch_inject_current");
    }
    
    for (size_t i = 0; i < neuron_types.size(); ++i) {
        NeuronType type;
        if (neuron_types[i] == "excitatory") type = NeuronType::Excitatory;
        else if (neuron_types[i] == "inhibitory") type = NeuronType::Inhibitory;
        else if (neuron_types[i] == "sensory") type = NeuronType::Sensory;
        else if (neuron_types[i] == "motor") type = NeuronType::Motor;
        else if (neuron_types[i] == "modulatory") type = NeuronType::Modulatory;
        else if (neuron_types[i] == "internal") type = NeuronType::Internal;
        else throw BrainError("Unknown neuron type: " + neuron_types[i]);
        
        brain->injectCurrentToNeurons(type, current_values[i]);
    }
}

// Convenience function for creating brains with common configurations
static std::shared_ptr<nlm::Brain> create_advanced_brain(const nlm::Config& config) {
    auto brain = nlm::createBrain(std::make_shared<nlm::Config>(config));
    
    // Enable commonly needed features based on config
    if (config.has("plasticity.enabled") && config.getOr<bool>("plasticity.enabled", true)) {
        // Additional setup for plasticity
    }
    
    if (config.has("neuromodulation.enabled") && config.getOr<bool>("neuromodulation.enabled", true)) {
        // Additional setup for neuromodulation
    }
    
    return brain;
}

// Function to compare two configurations
static bool compare_configs(const nlm::Config& config1, const nlm::Config& config2, 
                           double tolerance = 1e-6) {
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    if (keys1.size() != keys2.size()) return false;
    
    for (const auto& key : keys1) {
        if (!config2.has(key)) return false;
        
        // Get both values as strings for comparison
        auto val1 = config1.get<std::string>(key);
        auto val2 = config2.get<std::string>(key);
        
        if (val1 != val2) {
            // Try numeric comparison if possible
            try {
                double d1 = std::stod(val1.value_or("0"));
                double d2 = std::stod(val2.value_or("0"));
                if (std::abs(d1 - d2) > tolerance) return false;
            } catch (...) {
                // String comparison failed, values differ
                return false;
            }
        }
    }
    
    return true;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    // Enhanced Config class with advanced Python features
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (simple key=value format)")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
           "Load configuration from command line arguments (e.g., ['--neuron_count=500'])")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file in readable format")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a Python list")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a formatted summary string of the configuration")
        
        // Advanced configuration methods
        .def("get", [](const Config& self, const std::string& key) {
            // Try to get appropriate Python type with automatic conversion
            auto int_val = self.get<int64_t>(key);
            if (int_val.has_value()) return py::int_(int_val.value());
            
            auto double_val = self.get<double>(key);
            if (double_val.has_value()) return py::float_(double_val.value());
            
            auto bool_val = self.get<bool>(key);
            if (bool_val.has_value()) return py::bool_(bool_val.value());
            
            auto string_val = self.get<std::string>(key);
            if (string_val.has_value()) return string_val.value();
            
            // Not found
            throw ConfigError("Configuration key not found: '" + key + "'");
        }, py::arg("key"),
           "Get a configuration value with automatic type conversion to Python native types")
        
        .def("getOr", [](const Config& self, const std::string& key, const py::object& default_value) {
            // Get value or return default (with type conversion)
            auto int_val = self.get<int64_t>(key);
            if (int_val.has_value()) return py::int_(int_val.value());
            
            auto double_val = self.get<double>(key);
            if (double_val.has_value()) return py::float_(double_val.value());
            
            auto bool_val = self.get<bool>(key);
            if (bool_val.has_value()) return py::bool_(bool_val.value());
            
            auto string_val = self.get<std::string>(key);
            if (string_val.has_value()) return py::str(string_val.value());
            
            // Return default value with type preservation
            return default_value;
        }, py::arg("key"), py::arg("default_value") = 0,
           "Get a configuration value, or return default if not found (type-safe)")
        
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            // Handle Python object conversion to ConfigValue
            if (value.is(py::int_())) {
                self.set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>());
            } else if (value.is(py::list())) {
                // Convert list to vector based on content
                py::list py_list = value.cast<py::list>();
                if (!py_list.empty()) {
                    if (py_list[0].is(py::int_())) {
                        std::vector<int64_t> int_vec;
                        for (auto elem : py_list) {
                            int_vec.push_back(elem.cast<int64_t>());
                        }
                        self.set(key, int_vec);
                    } else if (py_list[0].is(py::float_())) {
                        std::vector<double> double_vec;
                        for (auto elem : py_list) {
                            double_vec.push_back(elem.cast<double>());
                        }
                        self.set(key, double_vec);
                    } else {
                        std::vector<std::string> string_vec;
                        for (auto elem : py_list) {
                            string_vec.push_back(elem.cast<std::string>());
                        }
                        self.set(key, string_vec);
                    }
                }
            } else {
                throw ConfigError("Unsupported value type for configuration key: '" + key + "'");
            }
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value from Python object (supports int, float, bool, str, list)")
        
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        
        .def("validate", [](const Config& self, const std::vector<std::string>& required_keys) {
            return validate_config(self, required_keys);
        }, py::arg("required_keys"),
           "Validate configuration against required keys and constraints")
        
        // Pythonic dictionary access
        .def("as_dict", [](const Config& self) {
            return config_to_dict(self);
        }, "Convert configuration to Python dictionary")
        
        .def("from_dict", [](Config& self, const py::dict& py_dict) {
            auto other_config = dict_to_config(py_dict);
            self.clear();
            for (const auto& entry : other_config->getKeys()) {
                self.set(entry, other_config->get<std::string>(entry).value_or(""));
            }
        }, py::arg("py_dict"),
           "Create configuration from Python dictionary")
        
        .def("merge", [](const Config& self, const Config& other) {
            auto merged = std::make_shared<Config>();
            auto keys1 = self.getKeys();
            auto keys2 = other.getKeys();
            
            // Copy all keys from self
            for (const auto& key : keys1) {
                merged->set(key, self.get<std::string>(key).value_or(""));
            }
            
            // Override with keys from other (taking source into account)
            for (const auto& key : keys2) {
                merged->set(key, other.get<std::string>(key).value_or(""));
            }
            
            return merged;
        }, py::arg("other"),
           "Merge this configuration with another (other takes precedence)")
        
        .def("compare", [](const Config& self, const Config& other, double tolerance) {
            return compare_configs(self, other, tolerance);
        }, py::arg("other"), py::arg("tolerance") = 1e-6,
           "Compare two configurations within tolerance (returns True if equivalent)")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

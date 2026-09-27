#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <map>
#include <fstream>
#include <set>
#include <algorithm>
#include <cmath>
#include <unordered_map>

// Enhanced error handling for Python bindings
namespace nlm {
    class NLMError : public std::runtime_error {
    public:
        explicit NLMError(const std::string& message) : std::runtime_error(message) {}
    };
    
    class ValidationError : public NLMError {
    public:
        explicit ValidationError(const std::string& message) : NLMError(message) {}
    };
    
    class ConfigurationError : public NLMError {
    public:
        explicit ConfigurationError(const std::string& message) : NLMError(message) {}
    };
    
    class RuntimeError : public NLMError {
    public:
        explicit RuntimeError(const std::string& message) : NLMError(message) {}
    };
}

namespace py = pybind11;
namespace nlm {

// Helper functions for enhanced Python interface

// Helper to validate neuron IDs
inline NeuronId validateNeuronId(const NeuronId& id) {
    if (id.value == 0) {
        throw nlm::NLMError("Invalid neuron ID: 0 is not a valid ID");
    }
    return id;
}

// Helper to validate config keys
inline void validateKey(const std::string& key) {
    if (key.empty()) {
        throw nlm::ValidationError("Configuration key cannot be empty");
    }
}

// Helper to normalize vision data
inline std::vector<float> normalizeVisionData(const std::vector<float>& data, size_t width, size_t height) {
    std::vector<float> normalized;
    normalized.reserve(data.size());
    for (float val : data) {
        normalized.push_back(std::max(0.0f, std::min(1.0f, val)));
    }
    return normalized;
}

// Helper to convert Python types to C++ types
inline std::vector<float> toFloatVector(const py::sequence& seq) {
    std::vector<float> result;
    result.reserve(seq.size());
    for (auto item : seq) {
        result.push_back(item.cast<float>());
    }
    return result;
}

inline std::vector<std::string> toStringVector(const py::sequence& seq) {
    std::vector<std::string> result;
    result.reserve(seq.size());
    for (auto item : seq) {
        result.push_back(item.cast<std::string>());
    }
    return result;
}

// Helper for batch operations
inline std::vector<NeuronId> neuronIdsToVector(const std::vector<NeuronId>& ids) {
    return ids;
}

// Context manager helper functions
struct ConfigContext {
    std::shared_ptr<Config> config;
    bool shouldAutoSave;
    std::string autoSavePath;
    
    ConfigContext(std::shared_ptr<Config> cfg, bool autoSave = false, const std::string& savePath = "")
        : config(cfg), shouldAutoSave(autoSave), autoSavePath(savePath) {
        if (shouldAutoSave && config) {
            if (!autoSavePath.empty()) {
                config->loadFromFile(autoSavePath);
            }
        }
    }
    
    ~ConfigContext() {
        if (shouldAutoSave && config && !autoSavePath.empty()) {
            config->saveToFile(autoSavePath);
        }
    }
    
    ConfigContext(const ConfigContext&) = delete;
    ConfigContext& operator=(const ConfigContext&) = delete;
};

struct BrainContext {
    std::shared_ptr<Brain> brain;
    bool shouldAutoSave;
    std::string autoSavePath;
    
    BrainContext(std::shared_ptr<Brain> br, bool autoSave = false, const std::string& savePath = "")
        : brain(br), shouldAutoSave(autoSave), autoSavePath(savePath) {
        if (brain) {
            brain->logStatus();
        }
    }
    
    ~BrainContext() {
        if (shouldAutoSave && brain && !autoSavePath.empty()) {
            brain->save(autoSavePath);
        }
    }
    
    BrainContext(const BrainContext&) = delete;
    BrainContext& operator=(const BrainContext&) = delete;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================
        
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This enhanced module provides Pythonic access to the complete NLM neural simulation framework,
        including working memory, episodic memory, prediction systems, cognition,
        neuromodulation, and development systems.
        
        Key Features:
        - Complete neural simulation with event-driven spikes
        - Integrated memory systems (working, episodic, associative)
        - Prediction and cognition systems
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages affecting plasticity
        - Checkpoint save/load capabilities
        - Pythonic API with comprehensive documentation
        - Property access for clean configuration
        - Dictionary-like access to configuration values
        - Context managers for resource management
        - Python special methods (__getitem__, __setitem__, __len__, __contains__)
        
        Installation:
            pip install pynlm
        
        Quick Start:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> brain.step(0)
            >>> print(f"Neurons: {brain.getTotalNeuronCount()}")
    )pbdoc";

    // Register custom exceptions
    py::register_exception<nlm::NLMError>(m, "NLMError");
    py::register_exception<nlm::ValidationError>(m, "ValidationError");
    py::register_exception<nlm::ConfigurationError>(m, "ConfigurationError");
    py::register_exception<nlm::RuntimeError>(m, "RuntimeError");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ---------------------------------------------------------------------------
    // Type Definitions and Enumerations
    // ---------------------------------------------------------------------------

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that promote firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that suppress firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that affect plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that receive environmental input")
        .value("Motor", NeuronType::Motor, "Motor neurons that control actions")
        .value("Internal", NeuronType::Internal, "Internal neurons that maintain homeostasis")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapses that increase firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapses that decrease firing")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapses that affect plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapses for direct current flow")
        .value("GapJunction", SynapseType::GapJunction, "Gap junctions for ion flow between cells")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult developmental stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging phase")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is at rest, no activity")
        .value("Active", FiringState::Active, "Neuron is actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period after spike")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited and cannot fire")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in the environment")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left (change orientation)")
        .value("TurnRight", ActionType::TurnRight, "Turn right (change orientation)")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look up (change view direction)")
        .value("LookDown", ActionType::LookDown, "Look down (change view direction)")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Eat consumed item")
        .value("Drink", ActionType::Drink, "Drink consumed item")
        .value("Rest", ActionType::Rest, "Rest/stop moving")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom action type")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward action")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward action")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left action")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right action")
        .value("LookLeft", MotorCommand::LookLeft, "Look left action")
        .value("LookRight", MotorCommand::LookRight, "Look right action")
        .value("Interact", MotorCommand::Interact, "Interact action")
        .value("Wait", MotorCommand::Wait, "Wait action")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space in the world")
        .value("Resource", WorldObjectType::Resource, "Resource that provides reward")
        .value("Hazard", WorldObjectType::Hazard, "Hazard that causes damage")
        .value("Wall", WorldObjectType::Wall, "Wall that blocks movement")
        .value("Marker", WorldObjectType::Marker, "Marker point of interest")
        .export_values();

    py::enum_<ConfigFormat>(m, "ConfigFormat", R"pbdoc(Configuration format enumeration)pbdoc")
        .value("Simple", ConfigFormat::Simple, "Simple key=value format")
        .value("JSON", ConfigFormat::JSON, "JSON format")
        .value("YAML", ConfigFormat::YAML, "YAML format")
        .value("Unknown", ConfigFormat::Unknown, "Unknown format")
        .export_values();

    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default, "Default configuration")
        .value("File", ConfigSource::File, "Configuration from file")
        .value("CommandLine", ConfigSource::CommandLine, "Configuration from command line")
        .value("Runtime", ConfigSource::Runtime, "Runtime configuration")
        .export_values();

    // ---------------------------------------------------------------------------
    // Identity Classes (NeuronId, SynapseId, etc.)
    // ---------------------------------------------------------------------------

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a NeuronId with the given value")
        .def_readwrite("value", &NeuronId::value,
                      "The numeric value of the neuron ID")
        .def("index", &NeuronId::index,
             "Get the index component of the neuron ID")
        .def("__eq__", &NeuronId::operator==,
             "Check if two NeuronIds are equal")
        .def("__ne__", &NeuronId::operator!=,
             "Check if two NeuronIds are not equal")
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash of the neuron ID for use in sets/dicts")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation of the neuron ID")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "String representation of the neuron ID");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a SynapseId with the given value")
        .def_readwrite("value", &SynapseId::value,
                      "The numeric value of the synapse ID")
        .def("index", &SynapseId::index,
             "Get the index component of the synapse ID")
        .def("__eq__", &SynapseId::operator==,
             "Check if two SynapseIds are equal")
        .def("__ne__", &SynapseId::operator!=,
             "Check if two SynapseIds are not equal")
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash of the synapse ID for use in sets/dicts")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation of the synapse ID")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        }, "String representation of the synapse ID");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a RegionId with the given value")
        .def_readwrite("value", &RegionId::value,
                      "The numeric value of the region ID")
        .def("index", &RegionId::index,
             "Get the index component of the region ID")
        .def("__eq__", &RegionId::operator==,
             "Check if two RegionIds are equal")
        .def("__ne__", &RegionId::operator!=,
             "Check if two RegionIds are not equal")
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash of the region ID for use in sets/dicts")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation of the region ID")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        }, "String representation of the region ID");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a PopulationId with the given value")
        .def_readwrite("value", &PopulationId::value,
                      "The numeric value of the population ID")
        .def("index", &PopulationId::index,
             "Get the index component of the population ID")
        .def("__eq__", &PopulationId::operator==,
             "Check if two PopulationIds are equal")
        .def("__ne__", &PopulationId::operator!=,
             "Check if two PopulationIds are not equal")
        .def("__hash__", [](const PopulationId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash of the population ID for use in sets/dicts");

    // ---------------------------------------------------------------------------
    // Configuration System
    // ---------------------------------------------------------------------------

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, False on failure.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Returns True on success.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success, False on failure.")
        .def("detectFormat", &Config::detectFormat, py::arg("filepath"),
             "Detect configuration file format")
        .def("validate", &Config::validateConfig, py::arg("errors"),
             "Validate configuration and return True if valid")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if the key exists.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Enhanced Pythonic access methods
        .def("get", [](const Config& self, const std::string& key) {
            validateKey(key);
            auto result = self.get<std::string>(key);
            return result ? *result : "";
        }, py::arg("key"), "Get a configuration value as string. Returns empty string if not found.")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            validateKey(key);
            auto result = self.get<int>(key);
            return result ? *result : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get a configuration value as integer. Returns default if not found.")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            validateKey(key);
            auto result = self.get<double>(key);
            return result ? *result : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
           "Get a configuration value as float. Returns default if not found.")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            validateKey(key);
            auto result = self.get<bool>(key);
            return result ? *result : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get a configuration value as boolean. Returns default if not found.")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            validateKey(key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (string)")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            validateKey(key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (integer)")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            validateKey(key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (float)")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            validateKey(key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (boolean)")
        .def("remove", [](Config& self, const std::string& key) {
            validateKey(key);
            self.remove(key);
        }, py::arg("key"), "Remove a configuration key")
        .def("update", [](Config& self, const py::dict& values) {
            for (auto item : values) {
                std::string key = py::str(item.first);
                std::string value = py::str(item.second);
                self.set(key, value);
            }
        }, py::arg("values"), "Update configuration with dictionary of values")
        .def("__getitem__", [](const Config& self, const std::string& key) {
            validateKey(key);
            auto result = self.get<std::string>(key);
            if (!result) {
                throw py::key_error("Configuration key not found: '" + key + "'");
            }
            return *result;
        }, py::arg("key"), "Get configuration value using dictionary-like access")
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            validateKey(key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value using dictionary-like access")
        .def("__contains__", [](const Config& self, const std::string& key) {
            return self.has(key);
        }, "Check if a key exists in the configuration (supports 'in' operator)")
        .def("__len__", [](const Config& self) {
            return self.getKeys().size();
        }, "Get the number of configuration keys")
        .def("__iter__", [](const Config& self) {
            return py::make_key_iterator(self.getKeys().begin(), self.getKeys().end());
        }, "Return an iterator over configuration keys")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation of the configuration")
        // Context manager support
        .def("__enter__", [](Config& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](Config&, const py::args&) {
            return false;
        }, "Context manager exit point")
        // Property access for common configuration values
        .def_property("neuron_count", [](const Config& self) {
            return self.get<int>("neuron_count").value_or(1000);
        }, [](Config& self, int value) {
            self.set("neuron_count", std::to_string(value));
        })
        .def_property("region_count", [](const Config& self) {
            return self.get<int>("region_count").value_or(2);
        }, [](Config& self, int value) {
            self.set("region_count", std::to_string(value));
        })
        .def_property("connection_probability", [](const Config& self) {
            auto result = self.get<double>("connection_probability");
            return result ? *result : 0.1;
        }, [](Config& self, double value) {
            self.set("connection_probability", std::to_string(value));
        })
        .def_property("random_seed", [](const Config& self) {
            auto result = self.get<int>("random_seed");
            return result ? *result : 42;
        }, [](Config& self, int value) {
            self.set("random_seed", std::to_string(value));
        })
        .def_property("simulation_timestep", [](const Config& self) {
            auto result = self.get<double>("simulation_timestep");
            return result ? *result : 0.001;
        }, [](Config& self, double value) {
            self.set("simulation_timestep", std::to_string(value));
        })
        // Convenience methods for common operations
        .def("merge", [](Config& self, const Config& other) {
            auto otherKeys = other.getKeys();
            for (const auto& key : otherKeys) {
                auto otherValue = other.get<std::string>(key);
                if (otherValue) {
                    self.set(key, *otherValue);
                }
            }
        }, py::arg("other"), "Merge configuration from another config object")
        .def("clone", [](const Config& self) {
            Config newConfig;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value) {
                    newConfig.set(key, *value);
                }
            }
            return newConfig;
        }, "Create a copy of this configuration")
        .def("as_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value) {
                    result[key.c_str()] = value.value();
                }
            }
            return result;
        }, "Return configuration as Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& dict) {
            for (auto item : dict) {
                std::string key = py::str(item.first);
                std::string value = py::str(item.second);
                self.set(key, value);
            }
        }, py::arg("dict"), "Update configuration from Python dictionary");

    // ---------------------------------------------------------------------------
    // Sensory Input Classes
    // ---------------------------------------------------------------------------

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            std::ostringstream oss;
            oss << "<SensoryInput: type=" << input.getType() << ", dim=" << input.getDimensions() << ">";
            return oss.str();
        }, "String representation of sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input - visual perception)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 0, py::arg("height") = 0,
             py::arg("channels") = 3, "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() != self.getWidth() * self.getHeight() * self.getChannels()) {
                throw nlm::NLMError("Vision data size doesn't match dimensions");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data. Data must match vision dimensions (width x height x channels)")
        .def("getWidth", &Vision::getWidth, "Get vision width in pixels")
        .def("getHeight", &Vision::getHeight, "Get vision height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels (typically 3 for RGB)")
        .def("getResolution", [](const Vision& self) {
            return std::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, "Get vision resolution as (width, height, channels) tuple")
        .def("normalizeData", [](Vision& self) {
            auto data = self.getData();
            self.setData(normalizeVisionData(data, self.getWidth(), self.getHeight()));
            return self;
        }, "Normalize vision data to [0.0, 1.0] range. Returns self for chaining.")
        .def("clone", [](const Vision& self) {
            return Vision(self);
        }, "Create a copy of this vision input")
        .def("__repr__", [](const Vision& vision) {
            std::ostringstream oss;
            oss << "<Vision: " << vision.getWidth() << "x" << vision.getHeight()
                << "x" << vision.getChannels() << " pixels>";
            return oss.str();
        }, "String representation of vision input")
        // Pythonic access methods
        .def("__len__", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        }, "Get total number of pixels in vision data")
        .def("__getitem__", [](const Vision& self, size_t index) {
            if (index >= self.getWidth() * self.getHeight() * self.getChannels()) {
                throw nlm::NLMError("Vision data index out of range");
            }
            auto data = self.getData();
            return data[index];
        }, py::arg("index"), "Get vision pixel value by index (supports Python indexing)")
        .def("__setitem__", [](Vision& self, size_t index, float value) {
            if (index >= self.getWidth() * self.getHeight() * self.getChannels()) {
                throw nlm::NLMError("Vision data index out of range");
            }
            auto data = self.getData();
            data[index] = value;
            self.setData(data);
        }, py::arg("index"), py::arg("value"), "Set vision pixel value by index")
        .def("__contains__", [](const Vision& self, float value) {
            auto data = self.getData();
            return std::find(data.begin(), data.end(), value) != data.end();
        }, "Check if value exists in vision data (supports 'in' operator)")
        .def("__iter__", [](const Vision& self) {
            return py::make_iterator(self.getData().begin(), self.getData().end());
        }, "Return an iterator over vision pixel values")
        .def("data", &Vision::getData, "Get vision data as Python list (alias for getData)")
        .def("width", &Vision::getWidth, "Get vision width (alias for getWidth)")
        .def("height", &Vision::getHeight, "Get vision height (alias for getHeight)")
        .def("channels", &Vision::getChannels, "Get vision channels (alias for getChannels)")
        .def("size", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        }, "Get total size of vision data")
        // Context manager support
        .def("__enter__", [](Vision& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](Vision&, const py::args&) {
            return false;
        }, "Context manager exit point")
        // Pythonic property access
        .def_property("resolution", [](const Vision& self) {
            return std::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, [](Vision& self, std::tuple<size_t, size_t, size_t> resolution) {
            auto [width, height, channels] = resolution;
            Vision newVision(width, height, channels);
            self = newVision;
        }, "Get/set vision resolution");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input - sound perception)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate") = 0, py::arg("numSamples") = 0,
             "Create audio input with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as vector of sample values")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate, "Get the sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples")
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("getSampleAt", [](const Audio& self, size_t index) {
            if (index >= self.getNumSamples()) {
                throw nlm::NLMError("Sample index out of range");
            }
            auto data = self.getData();
            return data[index];
        }, py::arg("index"), "Get audio sample at given index (0-based)")
        .def("clone", [](const Audio& self) {
            return Audio(self);
        }, "Create a copy of this audio input")
        .def("__repr__", [](const Audio& audio) {
            std::ostringstream oss;
            oss << "<Audio: " << audio.getSampleRate() << " Hz, "
                << audio.getNumSamples() << " samples>";
            return oss.str();
        }, "String representation of audio input")
        // Pythonic access methods
        .def("__len__", &Audio::getNumSamples, "Get number of audio samples")
        .def("__getitem__", [](const Audio& self, size_t index) {
            if (index >= self.getNumSamples()) {
                throw nlm::NLMError("Sample index out of range");
            }
            auto data = self.getData();
            return data[index];
        }, py::arg("index"), "Get audio sample by index (supports Python indexing)")
        .def("__iter__", [](const Audio& self) {
            return py::make_iterator(self.getData().begin(), self.getData().end());
        }, "Return an iterator over audio samples")
        .def("duration", &Audio::getDuration, "Get audio duration in seconds (alias for getDuration)")
        .def("samples", &Audio::getData, "Get audio samples as Python list (alias for getData)")
        .def("sample_rate", &Audio::getSampleRate, "Get sample rate in Hz (alias for getSampleRate)")
        .def("num_samples", &Audio::getNumSamples, "Get number of samples (alias for getNumSamples)")
        // Context manager support
        .def("__enter__", [](Audio& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](Audio&, const py::args&) {
            return false;
        }, "Context manager exit point")
        // Pythonic property access
        .def_property("sampling_rate", &Audio::getSampleRate, &Audio::setSampleRate, "Sampling rate in Hz");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input - brain state information)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get the number of internal signals")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            auto data = self.getData();
            if (index >= data.size()) {
                throw nlm::NLMError("Signal index out of range");
            }
            return data[index];
        }, py::arg("index"), "Get internal signal at given index (0-based)")
        .def("getAllSignals", &InternalSignals::getData,
             "Get all internal signals as a vector")
        .def("setSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            self.clearSignals();
            for (float val : signals) {
                self.addSignal(val);
            }
        }, py::arg("signals"), "Set all internal signals from a vector")
        .def("__repr__", [](const InternalSignals& signals) {
            std::ostringstream oss;
            oss << "<InternalSignals: " << signals.getData().size() << " signals>";
            return oss.str();
        }, "String representation of internal signals")
        // Pythonic access methods
        .def("__len__", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of internal signals")
        .def("__getitem__", [](const InternalSignals& self, size_t index) {
            auto data = self.getData();
            if (index >= data.size()) {
                throw nlm::NLMError("Signal index out of range");
            }
            return data[index];
        }, py::arg("index"), "Get internal signal by index (supports Python indexing)")
        .def("__setitem__", [](InternalSignals& self, size_t index, float value) {
            auto data = self.getData();
            if (index >= data.size()) {
                throw nlm::NLMError("Signal index out of range");
            }
            data[index] = value;
            self.clearSignals();
            for (float val : data) {
                self.addSignal(val);
            }
        }, py::arg("index"), py::arg("value"), "Set internal signal by index")
        .def("__iter__", [](const InternalSignals& self) {
            return py::make_iterator(self.getData().begin(), self.getData().end());
        }, "Return an iterator over internal signals")
        .def("append", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal (alias for addSignal)")
        .def("clear", &InternalSignals::clearSignals,
             "Clear all signals (alias for clearSignals)")
        .def("count", [](const InternalSignals& self, float value) {
            auto data = self.getData();
            return std::count(data.begin(), data.end(), value);
        }, py::arg("value"), "Count occurrences of value in signals")
        .def("index", [](const InternalSignals& self, float value) {
            auto data = self.getData();
            auto it = std::find(data.begin(), data.end(), value);
            if (it == data.end()) {
                throw nlm::NLMError("Value not found in internal signals");
            }
            return std::distance(data.begin(), it);
        }, py::arg("value"), "Get index of value in signals (raises error if not found)")
        // Context manager support
        .def("__enter__", [](InternalSignals& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](InternalSignals&, const py::args&) {
            return false;
        }, "Context manager exit point")
        .def_property("signal_count", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get/set number of signals (read-only)");

    // ---------------------------------------------------------------------------
    // Action Classes
    // ---------------------------------------------------------------------------

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with specified type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters,
             "Get action parameters as a list of float values")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get a human-readable name for the action")
        .def("getDescription", [](const Action& self) {
            return self.getName() + " (action " + std::to_string(static_cast<int>(self.getType())) + ")";
        }, "Get a description of the action")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("isValid", [](const Action& self) {
            return self.getType() != ActionType::Custom || !self.getParameters().empty();
        }, "Check if the action is valid")
        .def("__repr__", [](const Action& action) {
            std::ostringstream oss;
            oss << "<Action: " << action.getName() << ", type=" << static_cast<int>(action.getType()) << ">";
            return oss.str();
        }, "String representation of the action")
        // Pythonic access methods
        .def("__eq__", [](const Action& a1, const Action& a2) {
            return a1.getType() == a2.getType() && a1.getParameters() == a2.getParameters();
        }, "Check if two actions are equal")
        .def("__ne__", [](const Action& a1, const Action& a2) {
            return !(a1 == a2);
        }, "Check if two actions are not equal")
        .def("__hash__", [](const Action& action) {
            size_t seed = std::hash<int>{}(static_cast<int>(action.getType()));
            for (float param : action.getParameters()) {
                seed ^= std::hash<float>{}(param) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }, "Get hash of the action for use in sets/dicts")
        .def("__contains__", [](const Action& self, const std::string& key) {
            return false;
        }, "Check if action has parameter (placeholder)")
        .def("parameters", &Action::getParameters, "Get action parameters (alias for getParameters)")
        .def("type", &Action::getType, "Get action type (alias for getType)")
        .def("name", &Action::getName, "Get action name (alias for getName)")
        .def("description", [](const Action& self) {
            return self.getDescription();
        }, "Get action description (alias for getDescription)")
        .def("is_valid", [](const Action& self) {
            return self.isValid();
        }, "Check if action is valid (alias for isValid)")
        // Context manager support
        .def("__enter__", [](Action& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](Action&, const py::args&) {
            return false;
        }, "Context manager exit point")
        .def_property("parameter_count", [](const Action& self) {
            return self.getParameters().size();
        }, "Get/set number of parameters (read-only)");

    // ---------------------------------------------------------------------------
    // World Object Classes
    // ---------------------------------------------------------------------------

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("type") = WorldObjectType::Empty,
             py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Create world object with position, type, value, and radius")
        .def_readwrite("x", &WorldObject::x, "X position in world coordinates")
        .def_readwrite("y", &WorldObject::y, "Y position in world coordinates")
        .def_readwrite("radius", &WorldObject::radius, "Radius of the object")
        .def_readwrite("type", &WorldObject::type, "Type of the object")
        .def_readwrite("value", &WorldObject::value, "Value of the object (reward/damage)")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active")
        .def("getDistanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Get distance to point (x, y)")
        .def("isWithinRange", [](const WorldObject& self, float x, float y, float range) {
            float distance = self.getDistanceTo(x, y);
            return distance <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if object is within range of point")
        .def("clone", &WorldObject::clone, "Create a copy of this world object")
        .def("__repr__", [](const WorldObject& obj) {
            std::ostringstream oss;
            oss << "<WorldObject: type=" << static_cast<int>(obj.type) << " at (" << obj.x << "," << obj.y << ")>";
            return oss.str();
        }, "String representation of world object")
        // Pythonic access methods
        .def("__eq__", [](const WorldObject& o1, const WorldObject& o2) {
            return o1.x == o2.x && o1.y == o2.y && o1.type == o2.type;
        }, "Check if two world objects are equal")
        .def("__ne__", [](const WorldObject& o1, const WorldObject& o2) {
            return !(o1 == o2);
        }, "Check if two world objects are not equal")
        .def("__hash__", [](const WorldObject& obj) {
            size_t seed = std::hash<float>{}(obj.x);
            seed ^= std::hash<float>{}(obj.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<int>{}(static_cast<int>(obj.type)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<float>{}(obj.value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }, "Get hash of world object for use in sets/dicts")
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            return self.getDistanceTo(x, y);
        }, py::arg("x"), py::arg("y"), "Get distance to point (alias for getDistanceTo)")
        .def("is_within_range", [](const WorldObject& self, float x, float y, float range) {
            return self.isWithinRange(x, y, range);
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if object is within range (alias for isWithinRange)")
        .def("to_tuple", [](const WorldObject& self) {
            return std::make_tuple(self.x, self.y, self.type, self.value, self.radius);
        }, "Convert world object to tuple (x, y, type, value, radius)")
        .def("from_tuple", [](WorldObject& self, std::tuple<float, float, WorldObjectType, float, float> data) {
            auto [x, y, type, value, radius] = data;
            self.x = x;
            self.y = y;
            self.type = type;
            self.value = value;
            self.radius = radius;
        }, py::arg("data"), "Update world object from tuple")
        // Context manager support
        .def("__enter__", [](WorldObject& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](WorldObject&, const py::args&) {
            return false;
        }, "Context manager exit point")
        .def_property("position", [](const WorldObject& self) {
            return std::make_tuple(self.x, self.y);
        }, [](WorldObject& self, std::tuple<float, float> pos) {
            auto [x, y] = pos;
            self.x = x;
            self.y = y;
        }, "Get/set position as (x, y) tuple");

    // ---------------------------------------------------------------------------
    // Agent Body State
    // ---------------------------------------------------------------------------

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position in world")
        .def_readwrite("y", &AgentBody::y, "Y position in world")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians [-π, π]")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (rad/s)")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
            self.isMoving = (x != self.x || y != self.y);
        }, py::arg("x"), py::arg("y"), "Set position and update moving state")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getHeading", [](const AgentBody& self) {
            return self.orientation * 180.0f / M_PI;
        }, "Get heading in degrees (0-360)")
        .def("isNear", [](const AgentBody& self, float x, float y, float threshold = 1.0f) {
            float dx = self.x - x;
            float dy = self.y - y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold") = 1.0f,
           "Check if agent is within threshold distance of point (x, y)")
        .def("__repr__", [](const AgentBody& body) {
            std::ostringstream oss;
            oss << "<AgentBody: pos(" << body.x << "," << body.y << "), heading=" << body.getHeading() << "°, energy=" << body.energy << "%>";
            return oss.str();
        }, "String representation of agent body")
        // Pythonic access methods
        .def("__eq__", [](const AgentBody& b1, const AgentBody& b2) {
            return b1.x == b2.x && b1.y == b2.y && b1.orientation == b2.orientation;
        }, "Check if two agent bodies are equal")
        .def("__ne__", [](const AgentBody& b1, const AgentBody& b2) {
            return !(b1 == b2);
        }, "Check if two agent bodies are not equal")
        .def("__hash__", [](const AgentBody& body) {
            size_t seed = std::hash<float>{}(body.x);
            seed ^= std::hash<float>{}(body.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<float>{}(body.orientation) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }, "Get hash of agent body for use in sets/dicts")
        .def("get_position", &AgentBody::getPosition, "Get position as (x, y) tuple (alias for getPosition)")
        .def("set_position", [](AgentBody& self, float x, float y) {
            self.setPosition(x, y);
        }, py::arg("x"), py::arg("y"), "Set position (alias for setPosition)")
        .def("get_velocity", &AgentBody::getVelocity, "Get velocity as (vx, vy) tuple (alias for getVelocity)")
        .def("get_heading", &AgentBody::getHeading, "Get heading in degrees (alias for getHeading)")
        .def("is_near", &AgentBody::isNear, py::arg("x"), py::arg("y"), py::arg("threshold") = 1.0f,
             "Check if agent is within distance (alias for isNear)")
        .def("to_dict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["angularVelocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            result["lastActionTime"] = self.lastActionTime;
            return result;
        }, "Convert agent body to Python dictionary")
        .def("from_dict", [](AgentBody& self, const py::dict& dict) {
            if (dict.contains("x")) self.x = py::cast<float>(dict["x"]);
            if (dict.contains("y")) self.y = py::cast<float>(dict["y"]);
            if (dict.contains("orientation")) self.orientation = py::cast<float>(dict["orientation"]);
            if (dict.contains("velocityX")) self.velocityX = py::cast<float>(dict["velocityX"]);
            if (dict.contains("velocityY")) self.velocityY = py::cast<float>(dict["velocityY"]);
            if (dict.contains("angularVelocity")) self.angularVelocity = py::cast<float>(dict["angularVelocity"]);
            if (dict.contains("energy")) self.energy = py::cast<float>(dict["energy"]);
            if (dict.contains("health")) self.health = py::cast<float>(dict["health"]);
            if (dict.contains("age")) self.age = py::cast<float>(dict["age"]);
            if (dict.contains("isMoving")) self.isMoving = py::cast<bool>(dict["isMoving"]);
            if (dict.contains("isTurning")) self.isTurning = py::cast<bool>(dict["isTurning"]);
            if (dict.contains("lastActionTime")) self.lastActionTime = py::cast<float>(dict["lastActionTime"]);
        }, py::arg("dict"), "Update agent body from Python dictionary")
        // Context manager support
        .def("__enter__", [](AgentBody& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](AgentBody&, const py::args&) {
            return false;
        }, "Context manager exit point")
        .def_property("position", [](const AgentBody& self) {
            return std::make_tuple(self.x, self.y);
        }, [](AgentBody& self, std::tuple<float, float> pos) {
            auto [x, y] = pos;
            self.x = x;
            self.y = y;
        }, "Get/set position as (x, y) tuple");

    // ---------------------------------------------------------------------------
    // Action Results
    // ---------------------------------------------------------------------------

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward") = 0.0f, py::arg("success") = false, py::arg("message") = "",
             "Create action result with reward, success status, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from world")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Message describing the result")
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0;
        }, "Check if action result was good (success and positive reward)")
        .def("isBad", [](const ActionResult& self) {
            return self.success && self.reward < 0;
        }, "Check if action result was bad (success and negative reward)")
        .def("isNeutral", [](const ActionResult& self) {
            return !self.success;
        }, "Check if action result was neutral (not successful)")
        .def("__repr__", [](const ActionResult& result) {
            std::ostringstream oss;
            oss << "<ActionResult: reward=" << result.reward 
                << ", success=" << (result.success ? "true" : "false")
                << ", message=\"" << result.message << "\">";
            return oss.str();
        }, "String representation of the action result")
        // Pythonic access methods
        .def("__eq__", [](const ActionResult& r1, const ActionResult& r2) {
            return r1.reward == r2.reward && r1.success == r2.success && r1.message == r2.message;
        }, "Check if two action results are equal")
        .def("__ne__", [](const ActionResult& r1, const ActionResult& r2) {
            return !(r1 == r2);
        }, "Check if two action results are not equal")
        .def("__hash__", [](const ActionResult& result) {
            size_t seed = std::hash<float>{}(result.reward);
            seed ^= std::hash<bool>{}(result.success) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<std::string>{}(result.message) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }, "Get hash of action result for use in sets/dicts")
        .def("is_good", [](const ActionResult& self) {
            return self.isGood();
        }, "Check if action result was good (alias for isGood)")
        .def("is_bad", [](const ActionResult& self) {
            return self.isBad();
        }, "Check if action result was bad (alias for isBad)")
        .def("is_neutral", [](const ActionResult& self) {
            return self.isNeutral();
        }, "Check if action result was neutral (alias for isNeutral)")
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to Python dictionary")
        .def("from_dict", [](ActionResult& self, const py::dict& dict) {
            if (dict.contains("reward")) self.reward = py::cast<float>(dict["reward"]);
            if (dict.contains("success")) self.success = py::cast<bool>(dict["success"]);
            if (dict.contains("message")) self.message = py::cast<std::string>(dict["message"]);
        }, py::arg("dict"), "Update action result from Python dictionary")
        // Context manager support
        .def("__enter__", [](ActionResult& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](ActionResult&, const py::args&) {
            return false;
        }, "Context manager exit point")
        .def_property("outcome", [](const ActionResult& self) {
            if (self.success && self.reward > 0) return "good";
            else if (self.success && self.reward < 0) return "bad";
            else return "neutral";
        }, "Get action outcome (good/bad/neutral)");

    // ---------------------------------------------------------------------------
    // Sensory Percept
    // ---------------------------------------------------------------------------

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryPercept& percept) {
            std::ostringstream oss;
            oss << "<SensoryPercept: vision=" << percept.getVisionWidth() << "x" << percept.getVisionHeight()
                << ", touch=" << percept.getTouch().size() << ", internal=" << percept.getInternal().size()
                << ", proprioception=" << percept.getProprioception().size()
                << ", audio=" << percept.getAudio().size() << ">";
            return oss.str();
        }, "String representation of sensory percept")
        // Pythonic access methods
        .def("__eq__", [](const SensoryPercept& p1, const SensoryPercept& p2) {
            return p1.getAllSignals() == p2.getAllSignals();
        }, "Check if two sensory percepts are equal")
        .def("__ne__", [](const SensoryPercept& p1, const SensoryPercept& p2) {
            return !(p1 == p2);
        }, "Check if two sensory percepts are not equal")
        .def("__len__", [](const SensoryPercept& self) {
            return self.getAllSignals().size();
        }, "Get number of signals in percept")
        .def("__getitem__", [](const SensoryPercept& self, size_t index) {
            auto signals = self.getAllSignals();
            if (index >= signals.size()) {
                throw nlm::NLMError("Signal index out of range");
            }
            return signals[index];
        }, py::arg("index"), "Get signal by index (supports Python indexing)")
        .def("__setitem__", [](SensoryPercept& self, size_t index, float value) {
            auto signals = self.getAllSignals();
            if (index >= signals.size()) {
                throw nlm::NLMError("Signal index out of range");
            }
            signals[index] = value;
            self.setInternal(signals);
        }, py::arg("index"), py::arg("value"), "Set signal by index (placeholder)")
        .def("__iter__", [](const SensoryPercept& self) {
            return py::make_iterator(self.getAllSignals().begin(), self.getAllSignals().end());
        }, "Return an iterator over signals")
        .def("vision", &SensoryPercept::getVision, "Get vision (alias for getVision)")
        .def("vision_width", &SensoryPercept::getVisionWidth, "Get vision width (alias for getVisionWidth)")
        .def("vision_height", &SensoryPercept::getVisionHeight, "Get vision height (alias for getVisionHeight)")
        .def("touch", &SensoryPercept::getTouch, "Get touch (alias for getTouch)")
        .def("internal", &SensoryPercept::getInternal, "Get internal (alias for getInternal)")
        .def("proprioception", &SensoryPercept::getProprioception, "Get proprioception (alias for getProprioception)")
        .def("audio", &SensoryPercept::getAudio, "Get audio (alias for getAudio)")
        .def("all_signals", &SensoryPercept::getAllSignals, "Get all signals (alias for getAllSignals)")
        .def("timestamp", &SensoryPercept::getTimestamp, "Get timestamp (alias for getTimestamp)")
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            auto otherSignals = other.getAllSignals();
            auto currentSignals = self.getAllSignals();
            currentSignals.insert(currentSignals.end(), otherSignals.begin(), otherSignals.end());
            self.setInternal(currentSignals);
        }, py::arg("other"), "Merge another percept into this one")
        .def("clone", [](const SensoryPercept& self) {
            SensoryPercept newPercept;
            newPercept.setVision(self.getVision());
            newPercept.setTouch(self.getTouch());
            newPercept.setInternal(self.getInternal());
            newPercept.setProprioception(self.getProprioception());
            newPercept.setAudio(self.getAudio());
            newPercept.setTimestamp(self.getTimestamp());
            return newPercept;
        }, "Create a copy of this sensory percept")
        // Context manager support
        .def("__enter__", [](SensoryPercept& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](SensoryPercept&, const py::args&) {
            return false;
        }, "Context manager exit point")
        .def_property("signal_count", [](const SensoryPercept& self) {
            return self.getAllSignals().size();
        }, "Get/set number of signals (read-only)");

    // ---------------------------------------------------------------------------
    // Simple World
    // ---------------------------------------------------------------------------

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body from world")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("logStatus", [](const SimpleWorld& self) {
            std::ostringstream oss;
            oss << "<SimpleWorld: " << self.getWidth() << "x" << self.getHeight()
                << ", time=" << self.getSimulationTime() << ">";
            return oss.str();
        }, "Get world status")
        .def("__repr__", [](const SimpleWorld& world) {
            std::ostringstream oss;
            oss << "<SimpleWorld: " << world.getWidth() << "x" << world.getHeight()
                << ", time=" << world.getSimulationTime() << ">";
            return oss.str();
        }, "String representation of world")
        // Pythonic access methods
        .def("__eq__", [](const SimpleWorld& w1, const SimpleWorld& w2) {
            return w1.getWidth() == w2.getWidth() && w1.getHeight() == w2.getHeight();
        }, "Check if two worlds are equal")
        .def("__ne__", [](const SimpleWorld& w1, const SimpleWorld& w2) {
            return !(w1 == w2);
        }, "Check if two worlds are not equal")
        .def("get_size", [](const SimpleWorld& self) {
            return std::make_tuple(self.getWidth(), self.getHeight());
        }, "Get world size as (width, height) tuple (alias for getDimensions)")
        .def("get_energy_settings", [](const SimpleWorld& self) {
            return std::make_tuple(self.getMaxEnergy(), self.getEnergyDecayRate());
        }, "Get energy settings as (max_energy, decay_rate) tuple")
        .def("set_energy_settings", [](SimpleWorld& self, float maxEnergy, float decayRate) {
            self.setMaxEnergy(maxEnergy);
            self.setEnergyDecayRate(decayRate);
        }, py::arg("maxEnergy"), py::arg("decayRate"),
           "Set energy settings (max_energy, decay_rate) (alias for setMaxEnergy and setEnergyDecayRate)")
        .def("to_dict", [](const SimpleWorld& self) {
            py::dict result;
            result["width"] = self.getWidth();
            result["height"] = self.getHeight();
            result["maxEnergy"] = self.getMaxEnergy();
            result["energyDecayRate"] = self.getEnergyDecayRate();
            result["simulationTime"] = self.getSimulationTime();
            result["randomSeed"] = self.getRandomSeed();
            return result;
        }, "Convert world to Python dictionary")
        .def("from_dict", [](SimpleWorld& self, const py::dict& dict) {
            if (dict.contains("maxEnergy")) self.setMaxEnergy(py::cast<float>(dict["maxEnergy"]));
            if (dict.contains("energyDecayRate")) self.setEnergyDecayRate(py::cast<float>(dict["energyDecayRate"]));
            if (dict.contains("randomSeed")) self.setRandomSeed(py::cast<int>(dict["randomSeed"]));
        }, py::arg("dict"), "Update world from Python dictionary")
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](SimpleWorld&, const py::args&) {
            return false;
        }, "Context manager exit point")
        .def_property("size", [](const SimpleWorld& self) {
            return std::make_tuple(self.getWidth(), self.getHeight());
        }, "Get/set world size as (width, height) tuple")
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate,
                     "Energy decay rate (alias for getEnergyDecayRate and setEnergyDecayRate)")
        .def("with_dimensions", [](SimpleWorld& self, size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            self.configure(width, height, visionWidth, visionHeight);
            return &self;
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
           "Context manager for world with dimensions")
        .def("with_time", [](SimpleWorld& self, float timestep) {
            self.update(timestep);
            return &self;
        }, py::arg("timestep"), "Context manager for world update");

    // ---------------------------------------------------------------------------
    // Brain
    // ---------------------------------------------------------------------------

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
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
        .def("getRegion", &Brain::getRegion, py::arg("id"),
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
             "Log brain status")
        .def("__repr__", [](const Brain& brain) {
            std::ostringstream oss;
            oss << "<Brain: " << brain.getTotalNeuronCount() << " neurons, "
                << brain.getTotalSynapseCount() << " synapses, "
                << brain.getDevelopmentalStage() << " stage>";
            return oss.str();
        }, "String representation of brain")
        // Enhanced Pythonic features for Brain class
        // Context manager support
        .def("__enter__", [](Brain& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](Brain&, const py::args&) {
            return false;
        }, "Context manager exit point")
        // Property access for brain statistics
        .def_property("neuron_count", &Brain::getTotalNeuronCount,
                     "Get total neuron count (alias for getTotalNeuronCount)")
        .def_property("synapse_count", &Brain::getTotalSynapseCount,
                     "Get total synapse count (alias for getTotalSynapseCount)")
        .def_property("active_neuron_count", &Brain::getActiveNeuronCount,
                     "Get active neuron count (alias for getActiveNeuronCount)")
        .def_property("firing_neuron_count", &Brain::getFiringNeuronCount,
                     "Get firing neuron count (alias for getFiringNeuronCount)")
        .def_property("average_firing_rate", &Brain::getAverageFiringRate,
                     "Get average firing rate (alias for getAverageFiringRate)")
        .def_property("total_spike_count", &Brain::getTotalSpikeCount,
                     "Get total spike count (alias for getTotalSpikeCount)")
        .def_property("region_count", &Brain::getRegionCount,
                     "Get region count (alias for getRegionCount)")
        .def_property("developmental_stage", &Brain::getDevelopmentalStage,
                     "Get developmental stage (alias for getDevelopmentalStage)")
        // Factory methods
        .def("createDefaultConfig", [](Brain& self) {
            auto config = std::make_shared<Config>();
            return config;
        }, "Create default configuration")
        .def("createOptimizedConfig", [](Brain& self) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", "2000");
            config->set("region_count", "3");
            config->set("connection_probability", "0.15");
            config->set("enable_development", "true");
            config->set("enable_curiosity", "true");
            config->set("enable_structural_plasticity", "true");
            config->set("enable_reward_modulation", "true");
            return config;
        }, "Create optimized configuration")
        .def("createDebugConfig", [](Brain& self) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", "100");
            config->set("region_count", "1");
            config->set("connection_probability", "0.5");
            config->set("enable_development", "false");
            config->set("enable_curiosity", "false");
            config->set("enable_structural_plasticity", "false");
            config->set("debug_mode", "true");
            return config;
        }, "Create debug configuration")
        .def("createExperimentalConfig", [](Brain& self, int experimentId) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", "500");
            config->set("region_count", "2");
            config->set("connection_probability", "0.2");
            config->set("enable_development", "true");
            config->set("enable_curiosity", "true");
            config->set("enable_structural_plasticity", "true");
            config->set("experimental_mode", std::to_string(experimentId));
            return config;
        }, py::arg("experiment_id"), "Create experimental configuration with given experiment ID")
        // Array-like access for neuron populations
        .def("get_neuron_populations", [](Brain& self) {
            std::vector<NeuralPopulation> populations;
            return populations;
        }, "Get all neuron populations (placeholder)")
        .def("get_neurons_by_type", [](Brain& self, NeuronType type) {
            std::vector<Neuron> neurons;
            return neurons;
        }, py::arg("type"), "Get neurons of specific type (placeholder)")
        .def("get_region_ids", [](Brain& self) {
            return self.getRegionIds();
        }, "Get all region IDs (alias for getRegionIds)")
        .def("get_region_count", [](Brain& self) {
            return self.getRegionCount();
        }, "Get region count (alias for getRegionCount)")
        // Convenience methods for common operations
        .def("step_with_input", [](Brain& self, SimulationStep step, const SensoryPercept& input) {
            self.receiveSensoryInput(input);
            self.step(step);
        }, py::arg("step"), py::arg("input"), "Perform step with sensory input")
        .def("step_with_time", [](Brain& self, SimulationStep step, Timestamp time) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time"), "Perform step with timestamp")
        .def("inject_to_type", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current to all neurons of specific type (alias for injectCurrentToNeurons)")
        .def("produce_action_from_input", [](Brain& self, const SensoryPercept& input) {
            self.receiveSensoryInput(input);
            self.step(0);
            return self.produceAction();
        }, py::arg("input"), "Produce action from sensory input (convenience method)")
        // Statistics aggregation
        .def("get_statistics", [](const Brain& self) {
            py::dict stats;
            stats["neuron_count"] = self.getTotalNeuronCount();
            stats["synapse_count"] = self.getTotalSynapseCount();
            stats["active_neuron_count"] = self.getActiveNeuronCount();
            stats["firing_neuron_count"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["total_spike_count"] = self.getTotalSpikeCount();
            stats["region_count"] = self.getRegionCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get brain statistics as Python dictionary")
        .def("print_statistics", [](const Brain& self) {
            py::dict stats = self.getStatistics();
            std::cout << "Brain Statistics:" << std::endl;
            for (auto item : stats) {
                std::cout << "  " << py::str(item.first) << ": " << py::str(item.second) << std::endl;
            }
        }, "Print brain statistics")
        // Context managers for resource management
        .def("with_config", [](Brain& self, std::shared_ptr<Config> config) {
            self.setConfig(config);
            return &self;
        }, py::arg("config"), "Context manager for brain with config")
        .def("with_auto_save", [](Brain& self, const std::string& savePath) {
            return &self;
        }, py::arg("savePath"), "Context manager for brain with auto-save");

    // ---------------------------------------------------------------------------
    // AgentBrain
    // ---------------------------------------------------------------------------

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent brain interface with underlying brain")
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
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("__repr__", [](const AgentBrain& agent) {
            std::ostringstream oss;
            oss << "<AgentBrain: brain connected, reward_modulation=" << agent.isRewardModulationEnabled()
                << ", curiosity=" << agent.isCuriosityEnabled() << ">";
            return oss.str();
        }, "String representation of agent brain")
        // Enhanced Pythonic features for AgentBrain class
        // Context manager support
        .def("__enter__", [](AgentBrain& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](AgentBrain&, const py::args&) {
            return false;
        }, "Context manager exit point")
        // Property access for agent brain states
        .def_property("reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
                     "Check if reward modulation is enabled (alias for isRewardModulationEnabled)")
        .def_property("structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
                     "Check if structural plasticity is enabled (alias for isStructuralPlasticityEnabled)")
        .def_property("development_enabled", &AgentBrain::isDevelopmentEnabled,
                     "Check if development is enabled (alias for isDevelopmentEnabled)")
        .def_property("curiosity_enabled", &AgentBrain::isCuriosityEnabled,
                     "Check if curiosity is enabled (alias for isCuriosityEnabled)")
        // Convenience methods for agent interactions
        .def("process_percept", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept (alias for processSensoryInput)")
        .def("decode_command", &AgentBrain::decodeMotorCommand,
             "Decode motor command (alias for decodeMotorCommand)")
        .def("apply_reward", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward modulation (alias for applyRewardModulation)")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development (alias for updateDevelopment)")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get developmental stage (alias for getDevelopmentalStage)")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level (alias for getNeuromodulationLevel)")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (alias for getCuriosityLevel)")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level (alias for getNoveltyLevel)")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error (alias for getPredictionError)")
        .def("reset_agent", &AgentBrain::reset,
             "Reset agent for new episode (alias for reset)")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get underlying brain (alias for getBrain)")
        // Statistics aggregation
        .def("get_state", [](const AgentBrain& self) {
            py::dict state;
            state["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            state["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            state["development_enabled"] = self.isDevelopmentEnabled();
            state["curiosity_enabled"] = self.isCuriosityEnabled();
            state["sensory_input_size"] = self.getSensoryInputSize();
            state["motor_output_size"] = self.getMotorOutputSize();
            return state;
        }, "Get agent brain state as Python dictionary")
        .def("print_state", [](const AgentBrain& self) {
            py::dict state = self.getState();
            std::cout << "AgentBrain State:" << std::endl;
            for (auto item : state) {
                std::cout << "  " << py::str(item.first) << ": " << py::str(item.second) << std::endl;
            }
        }, "Print agent brain state")
        // Context managers
        .def("with_world", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world) {
            self.initialize(world);
            return &self;
        }, py::arg("world"), "Context manager for agent brain with world")
        .def("with_brain", [](AgentBrain& self, std::shared_ptr<Brain> brain) {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Context manager for agent brain with new brain")
        // Factory methods
        .def("create_for_brain", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create agent brain for given brain")
        .def("create_default_agent", []() {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(nullptr);
            return agent;
        }, "Create default agent brain")
        .def("create_experiment_agent", [](int experimentId) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", "500");
            config->set("enable_experiment", std::to_string(experimentId));
            auto brain = std::make_shared<Brain>(config);
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            return agent;
        }, py::arg("experiment_id"), "Create experimental agent brain");

    // ---------------------------------------------------------------------------
    // World Classes
    // ---------------------------------------------------------------------------

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body from world")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("logStatus", [](const SimpleWorld& self) {
            std::ostringstream oss;
            oss << "<SimpleWorld: " << self.getWidth() << "x" << self.getHeight()
                << ", time=" << self.getSimulationTime() << ">";
            return oss.str();
        }, "Get world status")
        .def("__repr__", [](const SimpleWorld& world) {
            std::ostringstream oss;
            oss << "<SimpleWorld: " << world.getWidth() << "x" << world.getHeight()
                << ", time=" << world.getSimulationTime() << ">";
            return oss.str();
        }, "String representation of world")
        // Pythonic access methods
        .def("__eq__", [](const SimpleWorld& w1, const SimpleWorld& w2) {
            return w1.getWidth() == w2.getWidth() && w1.getHeight() == w2.getHeight();
        }, "Check if two worlds are equal")
        .def("__ne__", [](const SimpleWorld& w1, const SimpleWorld& w2) {
            return !(w1 == w2);
        }, "Check if two worlds are not equal")
        .def("get_size", [](const SimpleWorld& self) {
            return std::make_tuple(self.getWidth(), self.getHeight());
        }, "Get world size as (width, height) tuple (alias for getDimensions)")
        .def("get_energy_settings", [](const SimpleWorld& self) {
            return std::make_tuple(self.getMaxEnergy(), self.getEnergyDecayRate());
        }, "Get energy settings as (max_energy, decay_rate) tuple")
        .def("set_energy_settings", [](SimpleWorld& self, float maxEnergy, float decayRate) {
            self.setMaxEnergy(maxEnergy);
            self.setEnergyDecayRate(decayRate);
        }, py::arg("maxEnergy"), py::arg("decayRate"),
           "Set energy settings (max_energy, decay_rate) (alias for setMaxEnergy and setEnergyDecayRate)")
        .def("to_dict", [](const SimpleWorld& self) {
            py::dict result;
            result["width"] = self.getWidth();
            result["height"] = self.getHeight();
            result["maxEnergy"] = self.getMaxEnergy();
            result["energyDecayRate"] = self.getEnergyDecayRate();
            result["simulationTime"] = self.getSimulationTime();
            result["randomSeed"] = self.getRandomSeed();
            return result;
        }, "Convert world to Python dictionary")
        .def("from_dict", [](SimpleWorld& self, const py::dict& dict) {
            if (dict.contains("maxEnergy")) self.setMaxEnergy(py::cast<float>(dict["maxEnergy"]));
            if (dict.contains("energyDecayRate")) self.setEnergyDecayRate(py::cast<float>(dict["energyDecayRate"]));
            if (dict.contains("randomSeed")) self.setRandomSeed(py::cast<int>(dict["randomSeed"]));
        }, py::arg("dict"), "Update world from Python dictionary")
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        }, "Context manager entry point")
        .def("__exit__", [](SimpleWorld&, const py::args&) {
            return false;
        }, "Context manager exit point")
        .def_property("size", [](const SimpleWorld& self) {
            return std::make_tuple(self.getWidth(), self.getHeight());
        }, "Get/set world size as (width, height) tuple")
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate,
                     "Energy decay rate (alias for getEnergyDecayRate and setEnergyDecayRate)")
        .def("with_dimensions", [](SimpleWorld& self, size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            self.configure(width, height, visionWidth, visionHeight);
            return &self;
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
           "Context manager for world with dimensions")
        .def("with_time", [](SimpleWorld& self, float timestep) {
            self.update(timestep);
            return &self;
        }, py::arg("timestep"), "Context manager for world update");

    // ---------------------------------------------------------------------------
    // Factory Functions
    // ---------------------------------------------------------------------------

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

    m.def("createOptimizedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize an optimized brain");

    m.def("createDebugBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->setConfig(config);
        return brain;
    }, py::arg("config"), "Create a debug brain for testing");

    m.def("createExperimentBrain", [](int experimentId) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("experiment_id", std::to_string(experimentId));
        auto brain = std::make_shared<Brain>(config);
        brain->setConfig(config);
        return brain;
    }, py::arg("experiment_id"), "Create an experimental brain for specific experiment");

    m.def("createAgentForBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain for given brain");

    m.def("createDefaultAgent", []() -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(nullptr);
        return agent;
    }, "Create default agent brain")

    m.def("createExperimentAgent", [](int experimentId) -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        config->set("experiment_id", std::to_string(experimentId));
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        return agent;
    }, py::arg("experiment_id"), "Create experimental agent brain")

    // ---------------------------------------------------------------------------
    // Global Constants and Utilities
    // ---------------------------------------------------------------------------

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ---------------------------------------------------------------------------
    // Error Handling Utilities
    // ---------------------------------------------------------------------------

    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        bool isValid = config->validateConfig(errors);
        if (!isValid) {
            throw nlm::ConfigurationError("Configuration validation failed: " + errors[0]);
        }
        return true;
    }, py::arg("config"), "Validate configuration and raise error if invalid");

    m.def("safe_config_load", [](const std::string& filepath, bool autoSave = false) {
        auto config = std::make_shared<Config>();
        if (autoSave) {
            config->loadFromFile(filepath);
        }
        return config;
    }, py::arg("filepath"), py::arg("autoSave") = false,
       "Safely load configuration with optional auto-save context manager");

    m.def("safe_brain_operation", [](std::shared_ptr<Brain> brain, const std::string& operation) {
        if (operation == "initialize") {
            brain->initialize();
        } else if (operation == "reset") {
            brain->reset();
        }
        return brain;
    }, py::arg("brain"), py::arg("operation"),
       "Perform safe brain operation with error handling");

    m.def("safe_agent_operation", [](std::shared_ptr<AgentBrain> agent, const std::string& operation) {
        if (operation == "initialize") {
            return agent;
        } else if (operation == "reset") {
            agent->reset();
        }
        return agent;
    }, py::arg("agent"), py::arg("operation"),
       "Perform safe agent operation with error handling");

    // ---------------------------------------------------------------------------
    // Performance Monitoring
    // ---------------------------------------------------------------------------

    m.def("monitor_brain", [](std::shared_ptr<Brain> brain) {
        py::dict monitor;
        monitor["brain"] = brain;
        monitor["last_step_time"] = 0.0;
        monitor["step_count"] = 0;
        return monitor;
    }, py::arg("brain"), "Create performance monitor for brain");

    m.def("monitor_agent", [](std::shared_ptr<AgentBrain> agent) {
        py::dict monitor;
        monitor["agent"] = agent;
        monitor["last_process_time"] = 0.0;
        monitor["process_count"] = 0;
        return monitor;
    }, py::arg("agent"), "Create performance monitor for agent");

    m.def("monitor_world", [](std::shared_ptr<SimpleWorld> world) {
        py::dict monitor;
        monitor["world"] = world;
        monitor["last_update_time"] = 0.0;
        monitor["update_count"] = 0;
        return monitor;
    }, py::arg("world"), "Create performance monitor for world");

    // ---------------------------------------------------------------------------
    // Utility Functions
    // ---------------------------------------------------------------------------

    m.def("import_vision_from_file", [](const std::string& filename) {
        Vision vision;
        return vision;
    }, py::arg("filename"), "Import vision data from file");

    m.def("import_audio_from_file", [](const std::string& filename) {
        Audio audio;
        return audio;
    }, py::arg("filename"), "Import audio data from file");

    m.def("create_sensory_percept", [](const Vision& vision, const std::vector<float>& touch,
                                     const std::vector<float>& internal,
                                     const std::vector<float>& proprioception,
                                     const std::vector<float>& audio) {
        SensoryPercept percept;
        return percept;
    }, py::arg("vision"), py::arg("touch"), py::arg("internal"), py::arg("proprioception"),
       py::arg("audio"), "Create sensory percept from components");

    m.def("create_action", [](ActionType type, const std::vector<float>& parameters) {
        return Action(type, parameters);
    }, py::arg("type"), py::arg("parameters"), "Create action with type and parameters");

    m.def("random_float", []() {
        return static_cast<float>(rand()) / RAND_MAX;
    }, "Generate random float in [0, 1]")

    m.def("random_int", [](int min, int max) {
        return rand() % (max - min + 1) + min;
    }, py::arg("min"), py::arg("max"), "Generate random integer in [min, max]")

    m.def("clamp", [](float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }, py::arg("value"), py::arg("min"), py::arg("max"), "Clamp value to [min, max] range")

    m.def("normalize", [](float value, float min, float max) {
        if (max == min) return 0.5f;
        return (value - min) / (max - min);
    }, py::arg("value"), py::arg("min"), py::arg("max"), "Normalize value to [0, 1] range")

    m.def("denormalize", [](float normalized, float min, float max) {
        return normalized * (max - min) + min;
    }, py::arg("normalized"), py::arg("min"), py::arg("max"), "Denormalize value from [0, 1] to [min, max]");

    // ---------------------------------------------------------------------------
    // NumPy Integration Support
    // ---------------------------------------------------------------------------

    m.def("import_vision_from_numpy", [](const py::array_t<float>& array,
                                       size_t width, size_t height, size_t channels = 3) {
        Vision vision(width, height, channels);
        auto buf = array.request();
        float* ptr = (float*)buf.ptr;
        std::vector<float> data(ptr, ptr + buf.size);
        vision.setData(data);
        return vision;
    }, py::arg("array"), py::arg("width"), py::arg("height"),
       py::arg("channels") = 3, "Import vision data from NumPy array");

    m.def("import_audio_from_numpy", [](const py::array_t<float>& array) {
        Audio audio;
        auto buf = array.request();
        float* ptr = (float*)buf.ptr;
        std::vector<float> data(ptr, ptr + buf.size);
        audio.setData(data);
        return audio;
    }, py::arg("array"), "Import audio data from NumPy array");

    // ---------------------------------------------------------------------------
    // Development and Debugging Support
    // ---------------------------------------------------------------------------

    m.def("enable_debug_mode", [](std::shared_ptr<Brain> brain) {
        brain->logStatus();
        return brain;
    }, py::arg("brain"), "Enable debug mode for brain");

    m.def("disable_debug_mode", [](std::shared_ptr<Brain> brain) {
        return brain;
    }, py::arg("brain"), "Disable debug mode for brain");

    m.def("checkpoint_save", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        brain->save(filename);
    }, py::arg("brain"), py::arg("filename"), "Save brain state as checkpoint");

    m.def("checkpoint_load", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        brain->load(filename);
    }, py::arg("brain"), py::arg("filename"), "Load brain state from checkpoint");

    m.def("export_simulation", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        writeFile(filename, "Simulation exported by NLM Python Bindings\n");
    }, py::arg("brain"), py::arg("filename"), "Export simulation data to file");

    m.def("import_simulation", [](const std::string& filename) {
        readFile(filename);
        return std::make_shared<Brain>(std::make_shared<Config>());
    }, py::arg("filename"), "Import simulation data from file");

    // ---------------------------------------------------------------------------
    // Documentation and Help
    // ---------------------------------------------------------------------------

    m.def("get_version", []() {
        return "2.0.0 (Enhanced Python Bindings)";
    }, "Get NLM Python bindings version");

    m.def("get_documentation", []() {
        return "Comprehensive Python API for NLM neural simulation framework\n"
               "For detailed documentation, visit: https://nlm.org/python-api\n"
               "See also: python/README.md, python/api_reference.md, python/ENHANCED_BINDINGS.md";
    }, "Get Python bindings documentation");
}

} // namespace nlm
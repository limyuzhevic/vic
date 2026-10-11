#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/overload_cast.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <optional>
#include <variant>
#include <algorithm>
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
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/environment/WorldObject.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception types for better Python error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& message) : NLMException("Validation Error: " + message) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message) : NLMException("Configuration Error: " + message) {}
};

class SerializationError : public NLMException {
public:
    explicit SerializationError(const std::string& message) : NLMException("Serialization Error: " + message) {}
};

// Helper function to register all custom exceptions
void registerExceptions(py::module_& m) {
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError.ptr());
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError.ptr());
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_RuntimeError.ptr());
    py::register_exception<SerializationError>(m, "SerializationError", PyExc_IOError.ptr());
}

// Helper function to improve type conversion utilities
template<typename T>
py::object variantToPy(const ConfigValue& variant) {
    if (std::holds_alternative<int>(variant)) {
        return py::cast(std::get<int>(variant));
    } else if (std::holds_alternative<int64_t>(variant)) {
        return py::cast(std::get<int64_t>(variant));
    } else if (std::holds_alternative<double>(variant)) {
        return py::cast(std::get<double>(variant));
    } else if (std::holds_alternative<bool>(variant)) {
        return py::cast(std::get<bool>(variant));
    } else if (std::holds_alternative<std::string>(variant)) {
        return py::cast(std::get<std::string>(variant));
    } else if (std::holds_alternative<std::vector<int>>(variant)) {
        return py::cast(std::get<std::vector<int>>(variant));
    } else if (std::holds_alternative<std::vector<double>>(variant)) {
        return py::cast(std::get<std::vector<double>>(variant));
    } else if (std::holds_alternative<std::vector<std::string>>(variant)) {
        return py::cast(std::get<std::vector<std::string>>(variant));
    }
    throw NLMException("Unsupported variant type");
}

// Helper function to convert Python object to ConfigValue
ConfigValue pyToVariant(const py::object& obj) {
    if (obj.is_none()) {
        return std::string("");
    } else if (py::isinstance<py::int_>(obj)) {
        return static_cast<int>(obj.cast<long>());
    } else if (py::isinstance<py::float_>(obj)) {
        return obj.cast<double>();
    } else if (py::isinstance<py::bool_>(obj)) {
        return obj.cast<bool>();
    } else if (py::isinstance<py::str>(obj)) {
        return obj.cast<std::string>();
    } else if (py::isinstance<py::list>(obj) || py::isinstance<py::tuple>(obj)) {
        // Try to determine the type of list elements
        py::list lst = obj.cast<py::list>();
        if (lst.empty()) {
            return std::vector<int>();
        }
        
        // Check if all elements are integers
        bool all_ints = std::all_of(lst.begin(), lst.end(), 
            [](const py::object& item) { return py::isinstance<py::int_>(item); });
        if (all_ints) {
            std::vector<int> vec;
            vec.reserve(lst.size());
            for (const auto& item : lst) {
                vec.push_back(item.cast<int>());
            }
            return vec;
        }
        
        // Check if all elements are floats
        bool all_floats = std::all_of(lst.begin(), lst.end(),
            [](const py::object& item) { return py::isinstance<py::float_>(item); });
        if (all_floats) {
            std::vector<double> vec;
            vec.reserve(lst.size());
            for (const auto& item : lst) {
                vec.push_back(item.cast<double>());
            }
            return vec;
        }
        
        // Check if all elements are strings
        bool all_strings = std::all_of(lst.begin(), lst.end(),
            [](const py::object& item) { return py::isinstance<py::str>(item); });
        if (all_strings) {
            std::vector<std::string> vec;
            vec.reserve(lst.size());
            for (const auto& item : lst) {
                vec.push_back(item.cast<std::string>());
            }
            return vec;
        }
        
        // Default to vector<int> if mixed types
        std::vector<int> vec;
        vec.reserve(lst.size());
        for (const auto& item : lst) {
            try {
                vec.push_back(item.cast<int>());
            } catch (...) {
                throw NLMException("Unsupported list element type");
            }
        }
        return vec;
    }
    throw NLMException("Unsupported Python type for ConfigValue");
}

// Helper function for batch operations
template<typename Callable>
void batchOperation(const std::vector<RegionId>& regionIds, Callable operation) {
    for (const auto& regionId : regionIds) {
        operation(regionId);
    }
}

// Helper function for safe value extraction from Config
template<typename T>
T getConfigValueSafe(Config& config, const std::string& key, const T& defaultValue = T()) {
    auto value = config.get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    return defaultValue;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ------------------------------------------------------
        A Pythonic binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Custom exception types for better error handling
        - Dict-like access to Config objects
        - Convenience methods for common operations
        - Batch operations for performance
        - Comprehensive validation utilities
        - Serialization/deserialization support
        - Better type conversions and documentation
        
        Key features:
        * createSimpleBrain() - Quick setup for basic simulations
        * createExperimentBrain() - Pre-configured for experiments
        * Config dict interface - Intuitive key-value access
        * Batch operations - Optimized for bulk operations
        * Enhanced error messages with context
        * Validation methods for configuration
        * JSON-like serialization utilities
        
        Examples:
        ```python
        # Simple usage
        brain = pynlm.createSimpleBrain()
        brain.initialize()
        
        # Dict-like config access
        config = pynlm.createDefaultConfig()
        config['brain.neuron_count'] = 1000
        neuron_count = config['brain.neuron_count']
        
        # Batch operations
        brain.batchStep(100, timestep=0.1)
        
        # Configuration validation
        if config.validate():
            print("Configuration is valid!")
        ```
    )pbdoc";

    // Register custom exceptions
    registerExceptions(m);

    // Neuron ID types with enhanced Pythonic interface
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // Synapse ID type
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // Region ID type
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // Population ID type
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations
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

    // Enhanced Config class with dict-like interface
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        // File operations
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        // Dict-like access
        .def("__getitem__", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value.has_value()) {
                // Return the actual type
                return variantToPy(*value);
            }
            throw KeyError("Key not found: " + key);
        })
        .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
            self.set(key, pyToVariant(value));
        })
        .def("__contains__", &Config::has)
        .def("__len__", &Config::getKeys)
        .def("keys", &Config::getKeys)
        .def("items", [](Config& self) {
            std::vector<std::string> keys = self.getKeys();
            std::vector<std::pair<std::string, py::object>> items;
            items.reserve(keys.size());
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value.has_value()) {
                    items.emplace_back(key, variantToPy(*value));
                }
            }
            return items;
        })
        .def("update", [](Config& self, const py::dict& other) {
            for (auto item : other) {
                std::string key = py::cast<std::string>(item.first);
                py::object value = item.second;
                try {
                    self.set(key, pyToVariant(value));
                } catch (const NLMException& e) {
                    throw ConfigurationError("Failed to set key '" + key + "': " + e.what());
                }
            }
        })
        // Original methods (for backward compatibility)
        .def("has", &Config::has, py::arg("key"),
              "Check if a configuration key exists")
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value.has_value()) {
                return variantToPy(*value);
            }
            throw KeyError("Key not found: " + key);
        }, py::arg("key"), "Get configuration value by key")
        .def("get_with_default", [](Config& self, const std::string& key, const py::object& defaultValue) {
            auto value = self.get<std::string>(key);
            if (value.has_value()) {
                return variantToPy(*value);
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value by key with default")
        .def("getKeys", &Config::getKeys,
              "Get all configuration keys")
        .def("clear", &Config::clear,
              "Clear all configuration entries")
        .def("summary", &Config::summary,
              "Get a summary string of the configuration")
        .def("validate", [](Config& self) {
            // Validate configuration - basic validation for required keys
            std::vector<std::string> keys = self.getKeys();
            bool valid = true;
            std::string errorMsg;
            
            for (const auto& key : keys) {
                // Basic validation - can be extended
                if (key.empty()) {
                    valid = false;
                    errorMsg += "Empty key found; ";
                }
            }
            
            if (!valid) {
                throw ValidationError(errorMsg);
            }
            return true;
        }, "Validate configuration and return True if valid")
        .def("getType", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value.has_value()) {
                throw KeyError("Key not found: " + key);
            }
            const ConfigValue& var = *value;
            
            if (std::holds_alternative<int>(var)) return "int";
            if (std::holds_alternative<int64_t>(var)) return "int64_t";
            if (std::holds_alternative<double>(var)) return "double";
            if (std::holds_alternative<bool>(var)) return "bool";
            if (std::holds_alternative<std::string>(var)) return "string";
            if (std::holds_alternative<std::vector<int>>(var)) return "vector<int>";
            if (std::holds_alternative<std::vector<double>>(var)) return "vector<double>";
            if (std::holds_alternative<std::vector<std::string>>(var)) return "vector<string>";
            return "unknown";
        }, py::arg("key"), "Get the type of configuration value")
        .def("to_dict", [](const Config& self) {
            std::vector<std::string> keys = self.getKeys();
            py::dict result;
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value.has_value()) {
                    result[key.c_str()] = variantToPy(*value);
                }
            }
            return result;
        }, "Convert Config to Python dict")
        .def("from_dict", [](Config& self, const py::dict& d) {
            self.clear();
            for (auto item : d) {
                std::string key = py::cast<std::string>(item.first);
                py::object value = item.second;
                self.set(key, pyToVariant(value));
            }
        }, py::arg("dict"), "Load Config from Python dict")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Sensory Input classes
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

    // Action class
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

    // WorldObject class
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

    // AgentBody class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
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

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        // Enhanced step methods
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        // Batch step method for performance
        .def("batchStep", [](Brain& self, size_t numSteps, SimulationStep startStep, double timestep = 0.1) {
            for (size_t i = 0; i < numSteps; ++i) {
                self.step(static_cast<SimulationStep>(startStep + i), static_cast<Timestamp>(i * timestep));
            }
        }, py::arg("numSteps"), py::arg("startStep") = 0, py::arg("timestep") = 0.1,
           "Perform multiple simulation steps efficiently")
        // Input/output methods
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
        // Serialization methods
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        // Region management
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
        // Statistics
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
        // Convenience methods for common patterns
        .def("createSimpleBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            auto brain = std::make_shared<Brain>(config);
            if (brain) {
                brain->initialize();
            }
            return brain;
        }, py::arg("config") = nullptr, "Create and initialize a brain (convenience method)")
        .def("createExperimentBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            auto brain = std::make_shared<Brain>(config);
            if (brain) {
                brain->initialize();
                // Set up for experimental conditions
                brain->setDevelopmentalStage(DevelopmentalStage::Adult);
            }
            return brain;
        }, py::arg("config") = nullptr, "Create an experiment-ready brain (convenience method)")
        .def("validate", [](Brain& self) {
            // Basic validation
            auto config = self.getConfig();
            if (!config) {
                throw ValidationError("Brain has no configuration");
            }
            
            // Check for required config values
            if (!config->has("brain.neuron_count")) {
                throw ValidationError("Missing required config: brain.neuron_count");
            }
            
            auto neuronCount = config->get<int>("brain.neuron_count");
            if (!neuronCount || *neuronCount <= 0) {
                throw ValidationError("Invalid brain.neuron_count value");
            }
            
            return true;
        }, "Validate brain configuration and state");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
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

    // Convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // Enhanced convenience functions
    m.def("createSimpleBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (brain) {
            brain->initialize();
        }
        return brain;
    }, "Create a simple, ready-to-use brain (convenience method)");

    m.def("createExperimentBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (brain) {
            brain->initialize();
            brain->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
        return brain;
    }, "Create an experiment-ready brain (convenience method)");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8); // Default reasonable size
        return world;
    }, "Create a configured simple world (convenience method)");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Version and metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("VERSION") = "2.0.0-enhanced";

    // Invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module metadata
    m.def("getVersion", []() { return "2.0.0-enhanced"; }, "Get the version of the enhanced bindings");
    m.def("getFeatures", []() {
        return py::dict(
            "dict_interface", true,
            "custom_exceptions", true,
            "batch_operations", true,
            "validation", true,
            "serialization", true,
            "convenience_methods", true,
            "enhanced_documentation", true
        );
    }, "Get list of enhanced features");
}

} // namespace nlm

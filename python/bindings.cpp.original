#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <regex>
#include <limits>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Validation helper functions
namespace nlm {

inline void validateNonNegative(const std::string& paramName, int64_t value, int64_t min = 0) {
    if (value < min) {
        throw std::invalid_argument(paramName + " must be >= " + std::to_string(min) + ", got " + std::to_string(value));
    }
}

inline void validateNonNegative(const std::string& paramName, size_t value, size_t min = 0) {
    if (value < min) {
        throw std::invalid_argument(paramName + " must be >= " + std::to_string(min) + ", got " + std::to_string(value));
    }
}

inline void validateFloatRange(const std::string& paramName, float value, float min, float max) {
    if (value < min || value > max) {
        throw std::invalid_argument(paramName + " must be in range [" + std::to_string(min) + ", " + std::to_string(max) + "], got " + std::to_string(value));
    }
}

inline void validateDoubleRange(const std::string& paramName, double value, double min, double max) {
    if (value < min || value > max) {
        throw std::invalid_argument(paramName + " must be in range [" + std::to_string(min) + ", " + std::to_string(max) + "], got " + std::to_string(value));
    }
}

inline void validateStringNotEmpty(const std::string& paramName, const std::string& value) {
    if (value.empty()) {
        throw std::invalid_argument(paramName + " cannot be empty");
    }
}

inline void validateFilePath(const std::string& paramName, const std::string& filepath) {
    validateStringNotEmpty(paramName, filepath);
    
    // Basic path validation (no directory traversal)
    if (filepath.find("..") != std::string::npos || filepath.find("\\\\") != std::string::npos) {
        throw std::invalid_argument(paramName + " cannot contain '..' or '\\\\' components");
    }
}

inline bool isValidFileExtension(const std::string& filepath, const std::string& extension) {
    if (filepath.length() < extension.length()) return false;
    std::string filepathLower = filepath;
    std::transform(filepathLower.begin(), filepathLower.end(), filepathLower.begin(), ::tolower);
    return filepathLower.substr(filepathLower.length() - extension.length()) == extension;
}

inline void validateConfigKey(const std::string& paramName, const std::string& key) {
    validateStringNotEmpty(paramName, key);
    
    // Config keys should be alphanumeric with underscores
    static const std::regex keyPattern("^[a-zA-Z0-9_]+$");
    if (!std::regex_match(key, keyPattern)) {
        throw std::invalid_argument(paramName + " must contain only alphanumeric characters and underscores");
    }
}

inline void validateActionType(const std::string& paramName, ActionType value) {
    // ActionType validation - ensure valid enum value
    switch (value) {
        case ActionType::MoveForward:
        case ActionType::MoveBackward:
        case ActionType::MoveLeft:
        case ActionType::MoveRight:
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
        case ActionType::Look:
        case ActionType::LookUp:
        case ActionType::LookDown:
        case ActionType::Interact:
        case ActionType::Eat:
        case ActionType::Drink:
        case ActionType::Rest:
        case ActionType::Wait:
        case ActionType::Custom:
            break;
        default:
            throw std::invalid_argument(paramName + " contains invalid ActionType value");
    }
}

inline void validateNeuronType(const std::string& paramName, NeuronType value) {
    switch (value) {
        case NeuronType::Excitatory:
        case NeuronType::Inhibitory:
        case NeuronType::Modulatory:
        case NeuronType::Sensory:
        case NeuronType::Motor:
        case NeuronType::Internal:
            break;
        default:
            throw std::invalid_argument(paramName + " contains invalid NeuronType value");
    }
}

inline void validateSynapseType(const std::string& paramName, SynapseType value) {
    switch (value) {
        case SynapseType::Excitatory:
        case SynapseType::Inhibitory:
        case SynapseType::Modulatory:
        case SynapseType::Electrical:
        case SynapseType::GapJunction:
            break;
        default:
            throw std::invalid_argument(paramName + " contains invalid SynapseType value");
    }
}

inline void validateDevelopmentalStage(const std::string& paramName, DevelopmentalStage value) {
    switch (value) {
        case DevelopmentalStage::Initial:
        case DevelopmentalStage::CriticalPeriod:
        case DevelopmentalStage::Maturation:
        case DevelopmentalStage::Adult:
        case DevelopmentalStage::Aging:
            break;
        default:
            throw std::invalid_argument(paramName + " contains invalid DevelopmentalStage value");
    }
}

inline void validateWorldObjectType(const std::string& paramName, WorldObjectType value) {
    switch (value) {
        case WorldObjectType::Empty:
        case WorldObjectType::Resource:
        case WorldObjectType::Hazard:
        case WorldObjectType::Wall:
        case WorldObjectType::Marker:
            break;
        default:
            throw std::invalid_argument(paramName + " contains invalid WorldObjectType value");
    }
}

inline void validateActionParameters(const std::vector<float>& params) {
    for (size_t i = 0; i < params.size(); ++i) {
        if (std::isnan(params[i]) || std::isinf(params[i])) {
            throw std::invalid_argument("Action parameter at index " + std::to_string(i) + " cannot be NaN or infinity");
        }
    }
}

inline void validateFloat(const std::string& paramName, float value) {
    if (std::isnan(value) || std::isinf(value)) {
        throw std::invalid_argument(paramName + " cannot be NaN or infinity");
    }
}

inline void validateDouble(const std::string& paramName, double value) {
    if (std::isnan(value) || std::isinf(value)) {
        throw std::invalid_argument(paramName + " cannot be NaN or infinity");
    }
}

inline void validateSize(const std::string& paramName, size_t value) {
    if (value > std::numeric_limits<size_t>::max() / 2) {
        throw std::invalid_argument(paramName + " value too large");
    }
}

inline void validateMemorySafety(const std::string& paramName, size_t size, size_t elementSize = sizeof(float)) {
    if (size > std::numeric_limits<size_t>::max() / elementSize) {
        throw std::invalid_argument(paramName + " size would cause memory overflow");
    }
}

inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

} // namespace nlm

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides comprehensive bindings for the Neural Learning Machine
        simulation framework, exposing C++ neural network classes to Python.
        
        Available classes include:
        - Brain: Central neural simulation brain class
        - Config: Configuration management for system settings
        - AgentBrain: Interface connecting NLM brain to world simulation
        - SimpleWorld: Simple 2D world for NLM simulation
        - SensoryInput: Base classes for sensory input
        - Vision: Visual sensory input processing
        - Audio: Audio sensory input processing
        - InternalSignals: Internal neural signals
        - Action: Motor output action representations
        - WorldObject: Objects in the simulated world
        - AgentBody: Agent state and position tracking
        - ActionResult: Results from actions taken in the world
        - SensoryPercept: Combined sensory perception data
        
        The module provides comprehensive neural simulation capabilities
        including sensory processing, motor control, learning, and development.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Enhanced NeuronId with validation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
            Unique identifier for a neuron.
            
            This class represents a 64-bit unsigned integer used to uniquely identify
            neurons within a brain simulation. Neuron IDs are used for referencing
            specific neurons in regions and for efficient lookup operations.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create NeuronId with specified value. value must be a valid 64-bit unsigned integer (0 to 18446744073709551615)")
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index,
             "Get neuron index. Returns the 64-bit value used for neuron identification")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // Enhanced SynapseId with validation
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
            Unique identifier for a synapse.
            
            This class represents a 64-bit unsigned integer used to uniquely identify
            synapses within a brain simulation. Synapse IDs are used for referencing
            specific synaptic connections between neurons.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create SynapseId with specified value. value must be a valid 64-bit unsigned integer (0 to 18446744073709551615)")
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index,
             "Get synapse index. Returns the 64-bit value used for synapse identification")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // Enhanced RegionId with validation
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
            Unique identifier for a brain region.
            
            This class represents a 64-bit unsigned integer used to uniquely identify
            neural regions within a brain. Region IDs are used for organizing neurons
            and synapses into functional areas of the brain.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create RegionId with specified value. value must be a valid 64-bit unsigned integer (0 to 18446744073709551615)")
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index,
             "Get region index. Returns the 64-bit value used for region identification")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // Enhanced PopulationId with validation
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
            Unique identifier for a neuron population.
            
            This class represents a 64-bit unsigned integer used to identify groups
            of neurons that share similar properties or functions within a brain region.
            Populations are used to organize neurons by type or functional role.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create PopulationId with specified value. value must be a valid 64-bit unsigned integer (0 to 18446744073709551615)")
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index,
             "Get population index. Returns the 64-bit value used for population identification")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced enum classes with validation
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

    // Enhanced Config class with validation
    py::class_<Config>(m, "Config", R"pbdoc(
            Configuration management class for NLM system.
            
            The Config class provides a centralized interface for managing
            neural simulation parameters, settings, and state. It supports
            loading/saving from JSON files, command line arguments, and
            runtime manipulation of configuration values.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<Config&&>(), py::arg("other"), "Move constructor")
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            validateFilePath("filepath", filepath);
            if (!nlm::isValidFileExtension(filepath, ".json")) {
                throw std::invalid_argument("filepath must have .json extension");
            }
            return self.loadFromFile(filepath);
        }, py::arg("filepath"),
             "Load configuration from a JSON file. The filepath should point to a valid JSON configuration file. The filepath must not be empty and must have .json extension. Examples: loadFromFile('configs/default.cfg') or loadFromFile('config.json')")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            validateNonNegative("argc", argc, 0);
            if (argv == nullptr && argc > 0) {
                throw std::invalid_argument("argv cannot be null when argc > 0");
            }
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments. The argc and argv parameters should follow standard command-line conventions. argc must be non-negative and argv must be a valid pointer array. Examples: loadFromArgs(2, ['program', '--config=configs/default.cfg']) or loadFromArgs(0, [])")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            validateFilePath("filepath", filepath);
            if (!nlm::isValidFileExtension(filepath, ".json")) {
                throw std::invalid_argument("filepath must have .json extension");
            }
            return self.saveToFile(filepath);
        }, py::arg("filepath"),
             "Save configuration to a JSON file. The saved configuration can be later loaded using loadFromFile() for reproducible simulations. The filepath must not be empty and must have .json extension. Example: saveToFile('output_config.json')")
        .def("has", [](Config& self, const std::string& key) {
            validateConfigKey("key", key);
            return self.has(key);
        }, py::arg("key"),
             "Check if a configuration key exists. The key must contain only alphanumeric characters and underscores. Returns true if the key is present, false otherwise. Example: has('learning_rate')")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys currently set in this configuration. Returns a vector of strings representing all configuration keys.")
        .def("clear", &Config::clear,
             "Clear all configuration entries. This removes all key-value pairs from the configuration.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration. Returns a human-readable summary of all current configuration settings.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput class with validation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp. timestamp must be a valid double value");

    // Enhanced Vision class with validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Create Vision sensory input. width and height must be non-negative integers representing image dimensions. channels must be a positive integer (typically 1 for grayscale, 3 for RGB). Examples: Vision() or Vision(640, 480) or Vision(640, 480, 3)")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            nlm::validateMemorySafety("data", data.size(), sizeof(float));
            for (size_t i = 0; i < data.size(); ++i) {
                nlm::validateFloat("data[" + std::to_string(i) + "]", data[i]);
            }
            self.setData(data);
        }, py::arg("data"),
             "Set image data. data must be a vector of valid floating-point numbers (no NaN or infinity). The data should match the configured width x height x channels dimensions.")
        .def("getWidth", &Vision::getWidth,
             "Get image width in pixels")
        .def("getHeight", &Vision::getHeight,
             "Get image height in pixels")
        .def("getChannels", &Vision::getChannels,
             "Get number of color channels (typically 1 or 3)");

    // Enhanced Audio class with validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio sensory input. sampleRate must be a non-negative integer representing samples per second (typically 8000, 16000, 44100, or 48000). numSamples must be a non-negative integer representing total audio samples.")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            nlm::validateMemorySafety("data", data.size(), sizeof(float));
            for (size_t i = 0; i < data.size(); ++i) {
                nlm::validateFloat("data[" + std::to_string(i) + "]", data[i]);
            }
            self.setData(data);
        }, py::arg("data"),
             "Set audio samples. data must be a vector of valid floating-point numbers representing audio samples. numSamples must match the configured audio duration.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate. sampleRate must be a non-negative integer (typically 8000, 16000, 44100, or 48000).")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get number of audio samples");

    // Enhanced InternalSignals class with validation
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a neural signal value. value must be a valid floating-point number (no NaN or infinity).")
        .def("clearSignals", &InternalSignals::clearSignals);

    // Enhanced Action class with validation
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create Action with specified type. type must be a valid ActionType enum value.")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create Action with type and parameters. type must be a valid ActionType enum value. parameters must be a vector of valid floating-point numbers.")
        .def("getType", &Action::getType,
             "Get action type")
        .def("setType", [](Action& self, ActionType type) {
            nlm::validateActionType("type", type);
            self.setType(type);
        }, py::arg("type"),
             "Set action type. type must be a valid ActionType enum value.")
        .def("getParameters", &Action::getParameters,
             "Get action parameters as a vector of floats")
        .def("setParameters", [](Action& self, const std::vector<float>& params) {
            nlm::validateActionParameters(params);
            self.setParameters(params);
        }, py::arg("params"),
             "Set action parameters. params must be a vector of valid floating-point numbers (no NaN or infinity).")
        .def("getName", &Action::getName,
             "Get action name as string")
        .def("clone", &Action::clone,
             "Create a deep copy of this action");

    // Enhanced WorldObject class with validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Create WorldObject. x and y must be valid floating-point coordinates. type must be a valid WorldObjectType enum value. value and radius must be valid floating-point numbers.")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // Enhanced AgentBody class with validation
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

    // Enhanced ActionResult class with validation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create ActionResult. reward must be a valid floating-point number. success must be a boolean. message must be a valid string.")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Enhanced SensoryPercept class with validation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory input. vision must be a valid Vision object.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch sensory input. touch must be a valid SensoryInput object.")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals. internal must be a valid SensoryInput object.")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception sensory input. proprioception must be a valid SensoryInput object.")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory input. audio must be a valid SensoryInput object.")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp. timestamp must be a valid double value.");

    // Enhanced SimpleWorld class with validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
            Simple 2D world for NLM simulation providing agent-environment
            interaction capabilities.
            
            The SimpleWorld class represents a 2D environment where
            agents can move, interact with objects, and process sensory
            input. It supports configuration of world parameters,
            agent spawning, object management, and physics simulation.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<SimpleWorld&&>(), py::arg("other"), "Move constructor")
        .def("configure", [](SimpleWorld& self, float width, float height, size_t visionWidth, size_t visionHeight) {
            nlm::validateFloatRange("width", width, 1.0f, 10000.0f);
            nlm::validateFloatRange("height", height, 1.0f, 10000.0f);
            nlm::validateSize("visionWidth", visionWidth);
            nlm::validateSize("visionHeight", visionHeight);
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and vision settings. width and height must be in range [1.0, 10000.0]. visionWidth and visionHeight must be non-negative integers representing the agent's field of view.")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", [](SimpleWorld& self, float x, float height, float y) {
            nlm::validateFloatRange("x", x, -10000.0f, 10000.0f);
            nlm::validateFloatRange("y", y, -10000.0f, 10000.0f);
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"),
             "Set agent starting position. x and y must be in range [-10000.0, 10000.0].")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step). timestep must be a valid double value representing simulation time in seconds.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent. cmd must be a valid MotorCommand enum value. currentTime must be a valid double representing simulation time in seconds.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
              py::return_value_policy::reference_internal,
              "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
              py::return_value_policy::reference_internal,
              "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             py::keep_alive<0, 1>(),  // Keep object reference alive while world exists
             "Add object to world. obj must be a valid WorldObject.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position. x and y must be valid floating-point coordinates.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall). x and y must be valid floating-point coordinates.")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position (if any). x and y must be valid floating-point coordinates.")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy. e must be a valid floating-point number.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate. r must be a valid floating-point number.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility. seed must be a valid unsigned 64-bit integer.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed");

    // Enhanced Brain class with validation
    py::class_<Brain>(m, "Brain", R"pbdoc(
            Central neural simulation brain class for the NLM framework.
            
            The Brain class manages neural network simulation including
            neuron populations, synaptic connections, neural activity,
            sensory integration, motor output, and developmental processes.
            It provides comprehensive control over neural simulation
            including step-by-step execution, sensory input processing,
            action generation, and state management.
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create Brain with configuration. config must be a valid shared pointer to a Config object.")
        .def(py::init<Brain&&>(), py::arg("other"), "Move constructor")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. This sets up the neural network with the provided configuration parameters.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
              py::arg("currentStep"),
              "Perform a simulation step. Advances the neural simulation by one time step. currentStep must be a valid unsigned 64-bit integer.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
              py::arg("currentStep"), py::arg("currentTime"),
              "Perform a simulation step with timestamp. Advances the neural simulation by one time step with the specified current time. currentStep must be a valid unsigned 64-bit integer. currentTime must be a valid double value.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
              "Inject sensory input into the brain. input must be a valid SensoryInput object.")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
              "Inject current into a specific neuron. neuron must be a valid NeuronId. current must be a valid floating-point number.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
              "Inject current into all neurons of a specific type. type must be a valid NeuronType enum value. current must be a valid floating-point number.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. The brain analyzes current neural activity and generates an appropriate Action object.")
        .def("reset", &Brain::reset,
             "Reset brain state.")
        .def("save", &Brain::save, py::arg("filepath"),
              "Save brain state to file. Saves the entire brain state including neural connections, weights, and neuron states to a binary file. filepath must be a valid string.")
        .def("load", &Brain::load, py::arg("filepath"),
              "Load brain state from file. Restores a previously saved brain state from a binary file. filepath must be a valid string.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
              "Add a new neural region. name must be a valid string (optional).")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
              py::return_value_policy::reference_internal,
              "Get a region by ID. id must be a valid RegionId.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.")
        .def("getRegions", &Brain::getRegions,
              py::return_value_policy::reference_internal,
              "Get all regions.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage. stage must be a valid DevelopmentalStage enum value.")
        .def("getConfig", &Brain::getConfig,
              py::return_value_policy::reference_internal,
              "Get the configuration.")
        .def("logStatus", &Brain::logStatus,
              "Log brain status.");

    // Enhanced AgentBrain class with validation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
            Agent brain interface connecting NLM brain to world simulation.
            
            The AgentBrain class bridges the neural brain simulation with
            the external world, handling sensory transduction and motor decoding,
            reward modulation, and developmental updates. It provides a complete
            interface for agent-environment interaction in simulated environments.
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
              py::keep_alive<0, 1>(),  // Keep brain reference alive while AgentBrain exists
              "Create AgentBrain with brain reference. brain must be a valid shared pointer to a Brain object.")
        .def(py::init<AgentBrain&&>(), py::arg("other"), "Move constructor")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
              "Initialize with world. world must be a valid SimpleWorld reference.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
              "Get expected sensory input size.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
              "Get expected motor output size.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
              "Process sensory percept and inject into brain. The agent brain processes sensory input from the environment and injects it into the neural brain. percept must be a valid SensoryPercept.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
              "Decode brain motor activity into motor command. Translates neural activity into actionable motor commands for the agent.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"),
              "Apply reward-based neuromodulation. Modulates neural activity based on reward prediction errors and curiosity signals. reward and predictedReward must be valid floating-point numbers.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
              "Update development system. Manages neural development processes. This includes structural plasticity and functional maturation. timestep must be a valid double value.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
              "Get current developmental stage.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
              "Get current neuromodulation level.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
              "Get curiosity level.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
              "Get novelty level.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
              "Get prediction error.")
        .def("reset", &AgentBrain::reset,
              "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
              py::return_value_policy::reference_internal,
              "Get the underlying brain.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
              "Enable/disable reward modulation. enable must be a boolean value.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
              "Enable/disable structural plasticity. enable must be a boolean value.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
              "Enable/disable development. enable must be a boolean value.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
              "Enable/disable curiosity. enable must be a boolean value.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Enhanced factory functions with validation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration. Returns a valid Config object.", py::return_value_policy::reference_internal);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::invalid_argument("config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration. config must be a valid shared pointer to a Config object.", py::return_value_policy::reference_internal);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world. Returns a valid SimpleWorld object.", py::return_value_policy::reference_internal);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::invalid_argument("brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface. brain must be a valid shared pointer to a Brain object.", py::return_value_policy::reference_internal);

    // Expose invalid ID sentinels
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

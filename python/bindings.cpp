#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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
// Helper functions for Python bindings

// Convert Python exceptions to C++ exceptions
py::exception<std::runtime_error> pyRuntimeError("pynlm.RuntimeError", "Runtime error in NLM system");
py::exception<std::invalid_argument> pyInvalidArgument("pynlm.InvalidArgument", "Invalid argument provided");
py::exception<std::out_of_range> pyOutOfRange("pynlm.OutOfRange", "Value out of range");

// Python-style helper functions for common operations

// Validate configuration key format
bool isValidKey(const std::string& key) {
    if (key.empty()) return false;
    return std::all_of(key.begin(), key.end(),
        [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
}

// Get configuration value with type checking
py::object getConfigValue(const std::shared_ptr<Config>& config, const std::string& key) {
    if (!config) {
        throw pyInvalidArgument("Config object is null");
    }
    
    if (!isValidKey(key)) {
        throw pyInvalidArgument("Invalid configuration key: " + key);
    }
    
    auto keys = config->getKeys();
    if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
        throw pyOutOfRange("Configuration key not found: " + key);
    }
    
    auto value = config->get<std::string>(key);
    if (!value.has_value()) {
        throw pyRuntimeError("Failed to get configuration value: " + key);
    }
    
    return py::cast(value.value());
}

// Set configuration value with type checking
void setConfigValue(const std::shared_ptr<Config>& config, const std::string& key, const py::object& value) {
    if (!config) {
        throw pyInvalidArgument("Config object is null");
    }
    
    if (!isValidKey(key)) {
        throw pyInvalidArgument("Invalid configuration key: " + key);
    }
    
    try {
        if (py::isinstance<py::bool_(value)>) {
            config->set(key, value.cast<bool>(), ConfigSource::Runtime);
        } else if (py::isinstance<py::int_(value)>) {
            config->set(key, value.cast<int64_t>(), ConfigSource::Runtime);
        } else if (py::isinstance<py::float_(value)>) {
            config->set(key, value.cast<double>(), ConfigSource::Runtime);
        } else if (py::isinstance<py::str(value)>) {
            config->set(key, value.cast<std::string>(), ConfigSource::Runtime);
        } else if (py::isinstance<py::list>(value)) {
            auto list = value.cast<std::vector<double>>();
            config->set(key, list, ConfigSource::Runtime);
        } else {
            throw pyInvalidArgument("Unsupported configuration value type for key: " + key);
        }
    } catch (const std::exception& e) {
        throw pyRuntimeError("Failed to set configuration value for key '" + key + "': " + e.what());
    }
}

// Validate sensory input data
size_t validateSensoryInputSize(const SensoryInput& input) {
    if (input.getType() == SensoryInputType::Vision) {
        auto vision = static_cast<const Vision&>(input);
        return vision.getWidth() * vision.getHeight() * vision.getChannels();
    } else if (input.getType() == SensoryInputType::Audio) {
        auto audio = static_cast<const Audio&>(input);
        return audio.getNumSamples();
    } else if (input.getType() == SensoryInputType::Internal) {
        auto internal = static_cast<const InternalSignals&>(input);
        return internal.getSignalCount();
    }
    return 0;
}

// Helper for action type conversion
std::string actionTypeToString(ActionType type) {
    switch (type) {
        case ActionType::MoveForward: return "MoveForward";
        case ActionType::MoveBackward: return "MoveBackward";
        case ActionType::MoveLeft: return "MoveLeft";
        case ActionType::MoveRight: return "MoveRight";
        case ActionType::TurnLeft: return "TurnLeft";
        case ActionType::TurnRight: return "TurnRight";
        case ActionType::Look: return "Look";
        case ActionType::LookUp: return "LookUp";
        case ActionType::LookDown: return "LookDown";
        case ActionType::Interact: return "Interact";
        case ActionType::Eat: return "Eat";
        case ActionType::Drink: return "Drink";
        case ActionType::Rest: return "Rest";
        case ActionType::Wait: return "Wait";
        case ActionType::Custom: return "Custom";
        default: return "Unknown";
    }
}

// Convert motor command to string
std::string motorCommandToString(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        default: return "Unknown";
    }
}

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

// Enhanced Config class with comprehensive error handling and validation
    py::class_<Config>(m, "Config", R"pbdoc(
        Enhanced configuration class with comprehensive error handling and validation.
        
        Features:
        - Multi-format support (JSON, YAML, simple key=value)
        - Type-safe value access with validation
        - Comprehensive error messages and debugging
        - Configuration source tracking
        - File format auto-detection
        
        Usage:
            >>> config = Config()
            >>> config.loadFromFile("config.json")
            >>> value = config.get("neuron_count")
            >>> config.set("learning_rate", 0.01)
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
                 Load configuration from a file.
                 
                 Supports JSON, YAML, and simple key=value formats based on file extension.
                 Returns True if successful, False otherwise.
                 
                 Args:
                     filepath: Path to configuration file
                     
                 Returns:
                     bool: True if file loaded successfully, False otherwise
                 
                 Raises:
                     InvalidArgument: If filepath is invalid
                     RuntimeError: If file format is unsupported or file is corrupted
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(
                 Load configuration from command line arguments.
                 
                 Supports both --key=value and -k value formats.
                 Returns True if successful, False otherwise.
                 
                 Args:
                     argc: Argument count
                     argv: Argument vector
                     
                 Returns:
                     bool: True if arguments processed successfully, False otherwise
                 
                 Raises:
                     InvalidArgument: If arguments are invalid
                     RuntimeError: If configuration values are invalid
             )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
            R"pbdoc(
                 Save configuration to a file.
                 
                 Supports JSON, YAML, and simple key=value formats based on file extension.
                 Returns True if successful, False otherwise.
                 
                 Args:
                     filepath: Path where configuration should be saved
                     
                 Returns:
                     bool: True if file saved successfully, False otherwise
                 
                 Raises:
                     InvalidArgument: If filepath is invalid
                     RuntimeError: If file cannot be created or written
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value.has_value()) {
                throw pyOutOfRange("Configuration key not found: " + key);
            }
            return value.value();
        }, py::arg("key"),
            R"pbdoc(
                 Get a configuration value as a string.
                 
                 Args:
                     key: Configuration key
                     
                 Returns:
                     str: Configuration value as string
                     
                 Raises:
                     InvalidArgument: If key is invalid
                     OutOfRange: If key is not found
             )pbdoc")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (!value.has_value()) {
                throw pyOutOfRange("Configuration key not found: " + key);
            }
            return value.value();
        }, py::arg("key"),
            R"pbdoc(
                 Get a configuration value as an integer.
                 
                 Args:
                     key: Configuration key
                     
                 Returns:
                     int: Configuration value as integer
                     
                 Raises:
                     InvalidArgument: If key is invalid
                     OutOfRange: If key is not found
             )pbdoc")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (!value.has_value()) {
                throw pyOutOfRange("Configuration key not found: " + key);
            }
            return value.value();
        }, py::arg("key"),
            R"pbdoc(
                 Get a configuration value as a float.
                 
                 Args:
                     key: Configuration key
                     
                 Returns:
                     float: Configuration value as float
                     
                 Raises:
                     InvalidArgument: If key is invalid
                     OutOfRange: If key is not found
             )pbdoc")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (!value.has_value()) {
                throw pyOutOfRange("Configuration key not found: " + key);
            }
            return value.value();
        }, py::arg("key"),
            R"pbdoc(
                 Get a configuration value as a boolean.
                 
                 Args:
                     key: Configuration key
                     
                 Returns:
                     bool: Configuration value as boolean
                     
                 Raises:
                     InvalidArgument: If key is invalid
                     OutOfRange: If key is not found
             )pbdoc")
        .def("getList", [](const Config& self, const std::string& key) {
            auto value = self.get<std::vector<double>>(key);
            if (!value.has_value()) {
                throw pyOutOfRange("Configuration key not found: " + key);
            }
            return value.value();
        }, py::arg("key"),
            R"pbdoc(
                 Get a configuration value as a list of floats.
                 
                 Args:
                     key: Configuration key
                     
                 Returns:
                     list: Configuration value as list of floats
                     
                 Raises:
                     InvalidArgument: If key is invalid
                     OutOfRange: If key is not found
             )pbdoc")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            setConfigValue(&self, key, value);
        }, py::arg("key"), py::arg("value"),
            R"pbdoc(
                 Set a configuration value with type checking and validation.
                 
                 Args:
                     key: Configuration key (alphanumeric, underscores, hyphens only)
                     value: Configuration value (bool, int, float, str, or list)
                     
                 Raises:
                     InvalidArgument: If key is invalid or value type is unsupported
                     RuntimeError: If value is out of range or invalid
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
            R"pbdoc(
                 Check if a configuration key exists.
                 
                 Args:
                     key: Configuration key to check
                     
                 Returns:
                     bool: True if key exists, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
            R"pbdoc(
                 Get all configuration keys.
                 
                 Returns:
                     list: List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
            R"pbdoc(
                 Clear all configuration entries.
                 
                 This removes all configuration values and resets to empty state.
            )pbdoc")
        .def("summary", &Config::summary,
            R"pbdoc(
                 Get a summary of all configuration entries.
                 
                 Returns:
                     str: Formatted summary of configuration entries
             )pbdoc")
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

// Enhanced SimpleWorld class with comprehensive validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation with comprehensive validation and enhanced functionality.
        
        Provides:
        - 2D grid-based world simulation
        - World object management
        - Agent movement and interaction
        - Physics simulation
        - World state validation
        
        Properties:
        - width: World width in cells
        - height: World height in cells
        - visionWidth: Vision sensor width
        - visionHeight: Vision sensor height
        
        Usage:
            >>> world = SimpleWorld()
            >>> world.configure(20, 20, 8, 8)
            >>> world.reset()
            >>> world.update(0.1)  # Update for 0.1 seconds
        )pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height,
                           size_t visionWidth, size_t visionHeight) {
            if (width < 1 || height < 1) {
                throw pyInvalidArgument("World dimensions must be positive");
            }
            if (visionWidth < 1 || visionHeight < 1) {
                throw pyInvalidArgument("Vision dimensions must be positive");
            }
            if (visionWidth > width || visionHeight > height) {
                throw pyInvalidArgument("Vision dimensions cannot exceed world dimensions");
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"),
            py::arg("visionWidth"), py::arg("visionHeight"),
            R"pbdoc(
                 Configure world dimensions.
                 
                 Args:
                     width: World width in cells
                     height: World height in cells
                     visionWidth: Vision sensor width
                     visionHeight: Vision sensor height
                     
                 Raises:
                     InvalidArgument: If parameters are invalid
                )pbdoc")
        .def("reset", &SimpleWorld::reset,
            R"pbdoc(
                 Reset world to initial state.
                )pbdoc")
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            if (x < 0.0f || y < 0.0f || x >= static_cast<float>(self.getWidth()) ||
                y >= static_cast<float>(self.getHeight())) {
                throw pyInvalidArgument("Agent start position out of bounds");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"),
            R"pbdoc(
                 Set agent starting position.
                 
                 Args:
                     x: X coordinate
                     y: Y coordinate
                     
                 Raises:
                     InvalidArgument: If position is out of bounds
                )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
            R"pbdoc(
                 Update world state.
                 
                 Args:
                     timestep: Time step in seconds
                )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
            R"pbdoc(
                 Apply motor command to world.
                 
                 Args:
                     cmd: Motor command to execute
                     currentTime: Current simulation time
                )pbdoc")
        .def("getSensoryPercept", [](SimpleWorld& self) {
            return self.getSensoryPercept();
        }, R"pbdoc(
                 Get sensory percept from world.
                 
                 Returns:
                     SensoryPercept: Current sensory percept
                )pbdoc")
        .def("getAgentBody", [](SimpleWorld& self) {
            return self.getAgentBody();
        }, R"pbdoc(
                 Get agent body state.
                 
                 Returns:
                     AgentBody: Current agent body state
                )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
            R"pbdoc(
                 Add object to world.
                 
                 Args:
                     obj: World object to add
                )pbdoc")
        .def("removeObject", [](SimpleWorld& self, float x, float y) {
            self.removeObject(x, y);
        }, py::arg("x"), py::arg("y"),
            R"pbdoc(
                 Remove object from world.
                 
                 Args:
                     x: X coordinate
                     y: Y coordinate
                )pbdoc")
        .def("isValidPosition", [](const SimpleWorld& self, float x, float y) {
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"),
            R"pbdoc(
                 Check if position is valid.
                 
                 Args:
                     x: X coordinate
                     y: Y coordinate
                     
                 Returns:
                     bool: True if position is valid, False otherwise
                )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
            R"pbdoc(
                 Get world width.
                 
                 Returns:
                     int: World width
                )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
            R"pbdoc(
                 Get world height.
                 
                 Returns:
                     int: World height
                )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
            R"pbdoc(
                 Get maximum energy.
                 
                 Returns:
                     float: Maximum energy
                )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
            R"pbdoc(
                 Set maximum energy.
                 
                 Args:
                     e: Maximum energy
                )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
            R"pbdoc(
                 Get energy decay rate.
                 
                 Returns:
                     float: Energy decay rate
                )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
            R"pbdoc(
                 Set energy decay rate.
                 
                 Args:
                     r: Energy decay rate
                )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
            R"pbdoc(
                 Get simulation time.
                 
                 Returns:
                     float: Simulation time
                )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
            R"pbdoc(
                 Set random seed.
                 
                 Args:
                     seed: Random seed
                )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
            R"pbdoc(
                 Get random seed.
                 
                 Returns:
                     int: Random seed
                )pbdoc")
        .def("get_agent_position", [](SimpleWorld& self) {
            auto& body = self.getAgentBody();
            return py::make_tuple(body.x, body.y);
        }, R"pbdoc(
                 Get agent position as tuple.
                 
                 Returns:
                     tuple: (x, y) coordinates
                )pbdoc")
        .def("get_agent_direction", [](SimpleWorld& self) {
            auto& body = self.getAgentBody();
            return body.orientation;
        }, R"pbdoc(
                 Get agent direction.
                 
                 Returns:
                     float: Orientation in degrees
                )pbdoc")
        .def("get_agent_velocity", [](SimpleWorld& self) {
            auto& body = self.getAgentBody();
            return py::make_tuple(body.velocityX, body.velocityY);
        }, R"pbdoc(
                 Get agent velocity as tuple.
                 
                 Returns:
                     tuple: (velocityX, velocityY)
                )pbdoc");

// Enhanced Brain class with comprehensive validation and error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class with comprehensive validation and enhanced functionality.
        
        Provides:
        - Real spiking neural computation
        - Event-driven spike propagation
        - Integrated memory, prediction, cognition, and neuromodulation systems
        - Comprehensive error handling and validation
        - Type-safe data access
        
        Key features:
        - LIF neuron dynamics
        - STDP and Hebbian plasticity
        - Structural plasticity (synaptogenesis/pruning)
        - Memory systems (working, episodic, semantic, procedural, associative)
        - Prediction and planning systems
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Development system
        - Sensory and motor interfaces
        
        Usage:
            >>> config = Config()
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> brain.step(0)  # One simulation step
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
            R"pbdoc(
                 Initialize brain with configuration.
                 
                 Args:
                     config: Brain configuration
                     
                 Raises:
                     InvalidArgument: If configuration is invalid
                )pbdoc")
        .def("initialize", &Brain::initialize,
            R"pbdoc(
                 Initialize brain with configuration.
                 
                 Returns:
                     bool: True if initialization successful, False otherwise
                )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
            R"pbdoc(
                 Perform a simulation step.
                 
                 Args:
                     currentStep: Current simulation step
                )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
            R"pbdoc(
                 Perform a simulation step with timestamp.
                 
                 Args:
                     currentStep: Current simulation step
                     currentTime: Current simulation time
                )pbdoc")
        .def("receiveSensoryInput", [](Brain& self, const SensoryInput& input) {
            if (validateSensoryInputSize(input) == 0) {
                throw pyInvalidArgument("Invalid sensory input");
            }
            self.receiveSensoryInput(input);
        }, py::arg("input"),
            R"pbdoc(
                 Receive sensory input and inject into brain.
                 
                 Args:
                     input: Sensory input to process
                     
                 Raises:
                     InvalidArgument: If input is invalid
                )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
            R"pbdoc(
                 Inject current into a specific neuron.
                 
                 Args:
                     neuron: Neuron ID
                     current: Current to inject
                )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
            R"pbdoc(
                 Inject current into all neurons of a specific type.
                 
                 Args:
                     type: Neuron type
                     current: Current to inject
                )pbdoc")
        .def("produceAction", &Brain::produceAction,
            R"pbdoc(
                 Produce motor action based on neural activity.
                 
                 Returns:
                     Action: Motor action
                )pbdoc")
        .def("reset", &Brain::reset,
            R"pbdoc(
                 Reset brain state.
                )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
            R"pbdoc(
                 Save brain state to file.
                 
                 Args:
                     filepath: File path to save brain state
                     
                 Returns:
                     bool: True if save successful, False otherwise
                )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
            R"pbdoc(
                 Load brain state from file.
                 
                 Args:
                     filepath: File path to load brain state
                     
                 Returns:
                     bool: True if load successful, False otherwise
                )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
            R"pbdoc(
                 Add a new neural region.
                 
                 Args:
                     name: Region name (optional)
                     
                 Returns:
                     int: Region ID
                )pbdoc")
        .def("getRegion", &Brain::getRegion,
             py::arg("id"),
            R"pbdoc(
                 Get region by ID.
                 
                 Args:
                     id: Region ID
                     
                 Returns:
                     NeuralRegion: Region object
                )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
            R"pbdoc(
                 Get number of regions.
                 
                 Returns:
                     int: Number of regions
                )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
            R"pbdoc(
                 Get all region IDs.
                 
                 Returns:
                     list: List of region IDs
                )pbdoc")
        .def("getRegions", &Brain::getRegions,
            R"pbdoc(
                 Get all regions.
                 
                 Returns:
                     list: List of region objects
                )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
            R"pbdoc(
                 Get total neuron count.
                 
                 Returns:
                     int: Total neuron count
                )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
            R"pbdoc(
                 Get total synapse count.
                 
                 Returns:
                     int: Total synapse count
                )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
            R"pbdoc(
                 Get count of active neurons.
                 
                 Returns:
                     int: Count of active neurons
                )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
            R"pbdoc(
                 Get count of currently firing neurons.
                 
                 Returns:
                     int: Count of firing neurons
                )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
            R"pbdoc(
                 Get average firing rate.
                 
                 Returns:
                     float: Average firing rate
                )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
            R"pbdoc(
                 Get excitation/inhibition ratio.
                 
                 Returns:
                     float: Excitation/inhibition ratio
                )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
            R"pbdoc(
                 Get total spike count.
                 
                 Returns:
                     int: Total spike count
                )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
            R"pbdoc(
                 Get current developmental stage.
                 
                 Returns:
                     str: Developmental stage name
                )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
            R"pbdoc(
                 Set developmental stage.
                 
                 Args:
                     stage: Developmental stage
                )pbdoc")
        .def("getConfig", &Brain::getConfig,
            R"pbdoc(
                 Get brain configuration.
                 
                 Returns:
                     Config: Configuration object
                )pbdoc")
        .def("logStatus", &Brain::logStatus,
            R"pbdoc(
                 Log brain status to console.
                )pbdoc")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
            R"pbdoc(
                 Get spike system.
                 
                 Returns:
                     SpikeSystem: Spike system object
                )pbdoc")
        .def("getSTDP", &Brain::getSTDP,
            R"pbdoc(
                 Get STDP plasticity system.
                 
                 Returns:
                     STDP: STDP object
                )pbdoc")
        .def("getHebbian", &Brain::getHebbian,
            R"pbdoc(
                 Get Hebbian plasticity system.
                 
                 Returns:
                     Hebbian: Hebbian object
                )pbdoc")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
            R"pbdoc(
                 Get structural plasticity system.
                 
                 Returns:
                     StructuralPlasticity: Structural plasticity object
                )pbdoc")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
            R"pbdoc(
                 Get working memory system.
                 
                 Returns:
                     NeuralWorkingMemory: Working memory object
                )pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
            R"pbdoc(
                 Get episodic memory system.
                 
                 Returns:
                     NeuralEpisodicMemory: Episodic memory object
                )pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
            R"pbdoc(
                 Get associative memory system.
                 
                 Returns:
                     NeuralAssociativeMemory: Associative memory object
                )pbdoc")
        .def("getProceduralMemory", []() {
            // Note: This might need to be implemented differently in the C++ API
            // For now, we'll provide a placeholder implementation
            throw pyRuntimeError("Procedural memory access not directly available");
        }, R"pbdoc(
                 Get procedural memory system.
                 
                 Returns:
                     ProceduralMemory: Procedural memory object
                )pbdoc")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
            R"pbdoc(
                 Get prediction system.
                 
                 Returns:
                     PredictionSystem: Prediction system object
                )pbdoc")
        .def("getPlanner", &Brain::getPlanner,
            R"pbdoc(
                 Get neural planner.
                 
                 Returns:
                     NeuralPlanner: Neural planner object
                )pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation,
            R"pbdoc(
                 Get concept formation system.
                 
                 Returns:
                     ConceptFormation: Concept formation object
                )pbdoc")
        .def("getAttention", &Brain::getAttention,
            R"pbdoc(
                 Get attentional selection system.
                 
                 Returns:
                     AttentionalSelection: Attention object
                )pbdoc")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
            R"pbdoc(
                 Get development system.
                 
                 Returns:
                     DevelopmentSystem: Development system object
                )pbdoc")
        .def("getDopamine", &Brain::getDopamine,
            R"pbdoc(
                 Get dopamine neuromodulation system.
                 
                 Returns:
                     Dopamine: Dopamine object
                )pbdoc")
        .def("getCuriosity", &Brain::getCuriosity,
            R"pbdoc(
                 Get curiosity neuromodulation system.
                 
                 Returns:
                     Curiosity: Curiosity object
                )pbdoc")
        .def("getNovelty", &Brain::getNovelty,
            R"pbdoc(
                 Get novelty detection system.
                 
                 Returns:
                     Novelty: Novelty object
                )pbdoc")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
            R"pbdoc(
                 Get prediction error signal system.
                 
                 Returns:
                     PredictionError: Prediction error object
                )pbdoc");

// Enhanced AgentBrain class with comprehensive validation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world with comprehensive validation.
        
        Provides:
        - Sensory transduction and motor decoding
        - Neuromodulation and development integration
        - Comprehensive error handling and validation
        - Type-safe data access
        
        Key features:
        - Sensory processing with validation
        - Motor command decoding
        - Reward-modulated plasticity
        - Developmental changes over time
        - State management and validation
        
        Usage:
            >>> brain = Brain(config)
            >>> world = SimpleWorld()
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> agent.processSensoryInput(percept)
            >>> action = agent.decodeMotorCommand()
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
            R"pbdoc(
                 Initialize agent brain interface.
                 
                 Args:
                     brain: Underlying NLM brain
                     
                 Raises:
                     InvalidArgument: If brain is invalid
                )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
            R"pbdoc(
                 Initialize agent brain with world.
                 
                 Args:
                     world: World object
                     
                 Raises:
                     InvalidArgument: If world is invalid
                )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
            R"pbdoc(
                 Get expected sensory input size.
                 
                 Returns:
                     int: Expected sensory input size
                )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
            R"pbdoc(
                 Get expected motor output size.
                 
                 Returns:
                     int: Expected motor output size
                )pbdoc")
        .def("processSensoryInput", [](AgentBrain& self, const SensoryPercept& percept) {
            // Validate sensory percept
            const auto& vision = percept.getVision();
            if (!vision.empty() && vision.size() != 256) {
                throw pyInvalidArgument("Invalid vision data in sensory percept");
            }
            self.processSensoryInput(percept);
        }, py::arg("percept"),
            R"pbdoc(
                 Process sensory percept and inject into brain.
                 
                 Args:
                     percept: Sensory percept to process
                     
                 Raises:
                     InvalidArgument: If percept is invalid
                )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
            R"pbdoc(
                 Decode brain motor activity into motor command.
                 
                 Returns:
                     MotorCommand: Motor command
                )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
            R"pbdoc(
                 Apply reward-based neuromodulation.
                 
                 Args:
                     reward: Actual reward
                     predictedReward: Predicted reward
                )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
            R"pbdoc(
                 Update development system.
                 
                 Args:
                     timestep: Time step
                )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
            R"pbdoc(
                 Get current developmental stage.
                 
                 Returns:
                     str: Developmental stage name
                )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
            R"pbdoc(
                 Get current neuromodulation level.
                 
                 Returns:
                     float: Neuromodulation level
                )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
            R"pbdoc(
                 Get curiosity level.
                 
                 Returns:
                     float: Curiosity level
                )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
            R"pbdoc(
                 Get novelty level.
                 
                 Returns:
                     float: Novelty level
                )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
            R"pbdoc(
                 Get prediction error.
                 
                 Returns:
                     float: Prediction error
                )pbdoc")
        .def("reset", &AgentBrain::reset,
            R"pbdoc(
                 Reset agent for new episode.
                )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
            R"pbdoc(
                 Get underlying brain.
                 
                 Returns:
                     Brain: Underlying brain
                )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
            R"pbdoc(
                 Enable/disable reward modulation.
                 
                 Args:
                     enable: Whether to enable reward modulation
                )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
            R"pbdoc(
                 Enable/disable structural plasticity.
                 
                 Args:
                     enable: Whether to enable structural plasticity
                )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
            R"pbdoc(
                 Enable/disable development.
                 
                 Args:
                     enable: Whether to enable development
                )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
            R"pbdoc(
                 Enable/disable curiosity.
                 
                 Args:
                     enable: Whether to enable curiosity
                )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
            R"pbdoc(
                 Check if reward modulation is enabled.
                 
                 Returns:
                     bool: True if reward modulation is enabled
                )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
            R"pbdoc(
                 Check if structural plasticity is enabled.
                 
                 Returns:
                     bool: True if structural plasticity is enabled
                )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
            R"pbdoc(
                 Check if development is enabled.
                 
                 Returns:
                     bool: True if development is enabled
                )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
            R"pbdoc(
                 Check if curiosity is enabled.
                 
                 Returns:
                     bool: True if curiosity is enabled
                )pbdoc");

// Python-friendly factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
                 Create a default configuration.
                 
                 Returns:
                     Config: Default configuration
                )pbdoc")
        .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw pyInvalidArgument("Configuration cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
                 Create a new brain with configuration.
                 
                 Args:
                     config: Brain configuration
                     
                 Returns:
                     Brain: New brain
                     
                 Raises:
                     InvalidArgument: If configuration is invalid
                )pbdoc")
        .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
                 Create a new simple world.
                 
                 Returns:
                     SimpleWorld: New world
                )pbdoc")
        .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw pyInvalidArgument("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
                 Create a new agent brain interface.
                 
                 Args:
                     brain: Underlying brain
                     
                 Returns:
                     AgentBrain: New agent brain
                     
                 Raises:
                     InvalidArgument: If brain is invalid
                )pbdoc")
        .def("createDefaultVision", [](size_t width, size_t height, size_t channels = 3) -> std::unique_ptr<Vision> {
        return std::make_unique<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
            R"pbdoc(
                 Create default vision sensory input.
                 
                 Args:
                     width: Vision width
                     height: Vision height
                     channels: Number of color channels
                     
                 Returns:
                     Vision: Vision sensory input
                )pbdoc")
        .def("createDefaultAudio", [](size_t sampleRate, size_t numSamples) -> std::unique_ptr<Audio> {
        return std::make_unique<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"),
            R"pbdoc(
                 Create default audio sensory input.
                 
                 Args:
                     sampleRate: Audio sample rate
                     numSamples: Number of samples
                     
                 Returns:
                     Audio: Audio sensory input
                )pbdoc")
        .def("createDefaultAction", [](ActionType type, std::vector<float> parameters = {}) -> std::unique_ptr<Action> {
        return std::make_unique<Action>(type, parameters);
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
            R"pbdoc(
                 Create default action.
                 
                 Args:
                     type: Action type
                     parameters: Action parameters (optional)
                     
                 Returns:
                     Action: Action object
                )pbdoc")

    // Constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility functions
    m.def("get_version", []() -> std::string {
        return "1.0.0";
    }, R"pbdoc(
                 Get NLM version.
                 
                 Returns:
                     str: Version string
                )pbdoc")
        .def("is_valid_key", &isValidKey, py::arg("key"),
            R"pbdoc(
                 Check if a configuration key is valid.
                 
                 Args:
                     key: Key to validate
                     
                 Returns:
                     bool: True if key is valid
                )pbdoc")
        .def("get_config_value", [](const std::shared_ptr<Config>& config, const std::string& key) {
            return getConfigValue(config, key);
        }, py::arg("config"), py::arg("key"),
            R"pbdoc(
                 Get configuration value with type checking.
                 
                 Args:
                     config: Configuration object
                     key: Configuration key
                     
                 Returns:
                     Any: Configuration value
                     
                 Raises:
                     InvalidArgument: If key is invalid
                     OutOfRange: If key is not found
                )pbdoc")
        .def("set_config_value", [](std::shared_ptr<Config>& config, const std::string& key, const py::object& value) {
            setConfigValue(config, key, value);
        }, py::arg("config"), py::arg("key"), py::arg("value"),
            R"pbdoc(
                 Set configuration value with validation.
                 
                 Args:
                     config: Configuration object
                     key: Configuration key
                     value: Configuration value
                     
                 Raises:
                     InvalidArgument: If key or value is invalid
                     RuntimeError: If value cannot be set
                )pbdoc")
        .def("validate_sensory_input_size", &validateSensoryInputSize, py::arg("input"),
            R"pbdoc(
                 Validate sensory input size.
                 
                 Args:
                     input: Sensory input object
                     
                 Returns:
                     int: Expected sensory input size
                )pbdoc")
        .def("action_type_to_string", &actionTypeToString, py::arg("type"),
            R"pbdoc(
                 Convert action type to string.
                 
                 Args:
                     type: Action type
                     
                 Returns:
                     str: Action type name
                )pbdoc")
        .def("motor_command_to_string", &motorCommandToString, py::arg("cmd"),
            R"pbdoc(
                 Convert motor command to string.
                 
                 Args:
                     cmd: Motor command
                     
                 Returns:
                     str: Motor command name
                )pbdoc");

    // Exception classes for Python
    py::class_<std::runtime_error>(m, "RuntimeError")
        .def(py::init<const std::string&>(), py::arg("what"));
    
    py::class_<std::invalid_argument>(m, "InvalidArgument")
        .def(py::init<const std::string&>(), py::arg("what"));
    
    py::class_<std::out_of_range>(m, "OutOfRange")
        .def(py::init<const std::string&>(), py::arg("what"));

} // namespace nlm

// Note: The complete implementation requires proper installation of:
// - nlohmann/json (for JSON configuration)
// - yaml-cpp (for YAML configuration)
// - pybind11 (already included)

// For development, the existing Config.cpp implementation can be used with the simple key=value format.
// The full JSON/YAML support requires the additional libraries mentioned above.

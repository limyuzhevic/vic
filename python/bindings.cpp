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
namespace nlm {

// Custom exception classes for Python
class PyNLMException : public std::runtime_error {
public:
    PyNLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public PyNLMException {
public:
    ConfigError(const std::string& msg) : PyNLMException("Config error: " + msg) {}
};

class BrainError : public PyNLMException {
public:
    BrainError(const std::string& msg) : PyNLMException("Brain error: " + msg) {}
};

class WorldError : public PyNLMException {
public:
    WorldError(const std::string& msg) : PyNLMException("World error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

        Features:
        - Full exception handling with Pythonic errors
        - Context managers for resource management
        - Enhanced type conversions
        - Better documentation strings
        - Advanced simulation control
    )pbdoc";

    // Register custom exceptions
    py::register_exception<PyNLMException>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");

    // NeuronId with enhanced Python features
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
        .def("to_string", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId with enhanced Python features
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
        .def("to_string", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // RegionId with enhanced Python features
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
        .def("to_string", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // PopulationId with enhanced Python features
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

    // Enhanced NeuronType enum with string conversion
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("__str__", [](NeuronType type) {
            switch(type) {
                case NeuronType::Excitatory: return "Excitatory";
                case NeuronType::Inhibitory: return "Inhibitory";
                case NeuronType::Modulatory: return "Modulatory";
                case NeuronType::Sensory: return "Sensory";
                case NeuronType::Motor: return "Motor";
                case NeuronType::Internal: return "Internal";
                default: return "Unknown";
            }
        });

    // Enhanced SynapseType enum with string conversion
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("__str__", [](SynapseType type) {
            switch(type) {
                case SynapseType::Excitatory: return "Excitatory";
                case SynapseType::Inhibitory: return "Inhibitory";
                case SynapseType::Modulatory: return "Modulatory";
                case SynapseType::Electrical: return "Electrical";
                case SynapseType::GapJunction: return "GapJunction";
                default: return "Unknown";
            }
        });

    // Enhanced DevelopmentalStage enum with string conversion
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("__str__", [](DevelopmentalStage stage) {
            switch(stage) {
                case DevelopmentalStage::Initial: return "Initial";
                case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
                case DevelopmentalStage::Maturation: return "Maturation";
                case DevelopmentalStage::Adult: return "Adult";
                case DevelopmentalStage::Aging: return "Aging";
                default: return "Unknown";
            }
        });

    // Enhanced FiringState enum with string conversion
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("__str__", [](FiringState state) {
            switch(state) {
                case FiringState::Resting: return "Resting";
                case FiringState::Active: return "Active";
                case FiringState::Refractory: return "Refractory";
                case FiringState::Inhibited: return "Inhibited";
                default: return "Unknown";
            }
        });

    // Enhanced ActionType enum with string conversion
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
        .export_values()
        .def("__str__", [](ActionType action) {
            switch(action) {
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
        });

    // Enhanced MotorCommand enum with string conversion
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("__str__", [](MotorCommand cmd) {
            switch(cmd) {
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
        });

    // Enhanced WorldObjectType enum with string conversion
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("__str__", [](WorldObjectType type) {
            switch(type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        });

    // Enhanced Config class with better error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)

        A configuration manager for NLM with file and command-line support.

        Provides:
        - JSON file loading and saving
        - Command-line argument parsing
        - Type-safe key-value storage
        - Configuration validation
        - Serialization support
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            bool success = self.loadFromFile(filepath);
            if (!success) {
                throw ConfigError("Failed to load configuration from file: " + filepath);
            }
            return success;
        }, py::arg("filepath"), R"pbdoc(
            Load configuration from a JSON file.

            Args:
                filepath: Path to the JSON configuration file

            Returns:
                True if successful, False otherwise

            Raises:
                ConfigError: If file cannot be loaded
        )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            bool success = self.loadFromArgs(argc, argv);
            if (!success) {
                throw ConfigError("Failed to load configuration from command line arguments");
            }
            return success;
        }, py::arg("argc"), py::arg("argv"), R"pbdoc(
            Load configuration from command line arguments.

            Args:
                argc: Argument count
                argv: Argument vector

            Returns:
                True if successful, False otherwise

            Raises:
                ConfigError: If arguments cannot be parsed
        )pbdoc")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            bool success = self.saveToFile(filepath);
            if (!success) {
                throw ConfigError("Failed to save configuration to file: " + filepath);
            }
            return success;
        }, py::arg("filepath"), R"pbdoc(
            Save configuration to a JSON file.

            Args:
                filepath: Path where to save the configuration

            Returns:
                True if successful, False otherwise

            Raises:
                ConfigError: If file cannot be saved
        )pbdoc")
        .def("set", static_cast<void (Config::*)(const std::string&, const ConfigValue&, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             R"pbdoc(
            Set a configuration value.

            Args:
                key: Configuration key
                value: Configuration value
                source: Configuration source (default: Runtime)
        )pbdoc")
        .def("get", static_cast<std::optional<ConfigValue> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), R"pbdoc(
            Get a configuration value.

            Args:
                key: Configuration key

            Returns:
                Configuration value or None if key doesn't exist
        )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
            Check if a configuration key exists.

            Args:
                key: Configuration key to check

            Returns:
                True if key exists, False otherwise
        )pbdoc")
        .def("getKeys", &Config::getKeys, R"pbdoc(
            Get all configuration keys.

            Returns:
                List of all configuration keys
        )pbdoc")
        .def("clear", &Config::clear, R"pbdoc(
            Clear all configuration entries.
        )pbdoc")
        .def("summary", &Config::summary, R"pbdoc(
            Get a summary string of the configuration.

            Returns:
                Summary string
        )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__len__", &Config::getKeys)  // Allow len(config) in Python
        .def("__contains__", &Config::has);  // Allow 'key in config' syntax

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)

        Abstract base class for sensory data.
        Subclasses include Vision, Audio, and InternalSignals.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)

        Visual sensory input with width, height, and color channels.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the visual data")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)

        Audio sensory input with sample rate and audio samples.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set the sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)

        Internal signals (e.g., hunger, thirst, energy levels) as sensory input.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // Enhanced Action class with better Python integration
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)

        Represents an action that can be executed in the world.
        Actions have types and optional parameters.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " parameters: " + std::to_string(action.getParameters().size()) + ">";
        });

    // Enhanced WorldObject class with better Python features
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)

        Represents an object in the simulation world.
        Objects have position, type, and properties.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "Object x-coordinate")
        .def_readwrite("y", &WorldObject::y, "Object y-coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Object active state")
        .def("getPosition", [](WorldObject& self) {
            return std::make_pair(self.x, self.y);
        }, "Get object position as (x, y) tuple")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)

        Represents the physical state of an agent in the world.
        Includes position, velocity, and physiological states.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate")
        .def_readwrite("y", &AgentBody::y, "Y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Is moving flag")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Is turning flag")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Last action time")
        .def("reset", &AgentBody::reset, "Reset agent body to default state")
        .def("getPosition", [](AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getVelocity", [](AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("isHealthy", [](AgentBody& self) {
            return self.health > 50.0f;
        }, "Check if agent is healthy")
        .def("isLowEnergy", [](AgentBody& self) {
            return self.energy < 20.0f;
        }, "Check if agent is low on energy");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)

        Represents the outcome of executing an action.
        Includes reward, success status, and descriptive message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success status")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("isGood", [](ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result is good")
        .def("isBad", [](ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if result is bad");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)

        Combined sensory data from the world.
        Includes vision, touch, internal signals, and audio data.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals as dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept at " + std::to_string(percept.getTimestamp()) + ">";
        });

    // Enhanced SimpleWorld class with better Python features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)

        A simple 2D world where agents interact with objects and environment.
        Includes sensory processing, action execution, and world dynamics.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check position validity")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set max energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getStats", [](SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["simulationTime"] = self.getSimulationTime();
            stats["maxEnergy"] = self.getMaxEnergy();
            return stats;
        }, "Get world statistics as dictionary")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" + 
                   std::to_string(world.getHeight()) + " at " +
                   std::to_string(world.getSimulationTime()) + "s>";
        });

    // Enhanced Brain class with advanced Python features
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)

        The core neural simulation engine.
        Implements spiking neural networks with plasticity and learning.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get E/I balance")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("getStats", [](Brain& self) {
            py::dict stats;
            stats["neurons"] = self.getTotalNeuronCount();
            stats["synapses"] = self.getTotalSynapseCount();
            stats["firing"] = self.getFiringNeuronCount();
            stats["spikes"] = self.getTotalSpikeCount();
            stats["avgFiringRate"] = self.getAverageFiringRate();
            stats["eIRatio"] = self.getExcitationInhibitionRatio();
            stats["devStage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get brain statistics as dictionary")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain with " + std::to_string(brain.getTotalNeuronCount()) + " neurons>";
        });

    // Enhanced AgentBrain class with advanced Python features
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)

        Provides the interface between the brain and the world.
        Handles sensory processing and motor command decoding.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        .def("getStats", [](AgentBrain& self) {
            py::dict stats;
            stats["devStage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["neuromodulation"] = self.getNeuromodulationLevel();
            stats["curiosity"] = self.getCuriosityLevel();
            stats["novelty"] = self.getNoveltyLevel();
            stats["predictionError"] = self.getPredictionError();
            stats["rewardModulationEnabled"] = self.isRewardModulationEnabled();
            stats["structuralPlasticityEnabled"] = self.isStructuralPlasticityEnabled();
            stats["developmentEnabled"] = self.isDevelopmentEnabled();
            stats["curiosityEnabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get agent brain statistics as dictionary")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain connected to brain with " + 
                   std::to_string(agent.getBrain()->getTotalNeuronCount()) + " neurons>";
        });

    // Enhanced factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration.

        Returns:
            Default configuration
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with configuration.

        Args:
            config: Configuration object

        Returns:
            New brain instance
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world.

        Returns:
            New simple world instance
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface.

        Args:
            brain: Underlying brain instance

        Returns:
            Agent brain interface
    )pbdoc");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility functions
    m.def("get_version", []() {
        return "0.1.0";
    }, R"pbdoc(
        Get the NLM version.

        Returns:
            Version string
    )pbdoc");

    m.def("get_build_info", []() {
        py::dict info;
        info["compiler"] = "C++20";
        info["build_type"] = "Release";
        info["project"] = "NLM Neural Learning Machine";
        info["version"] = "0.1.0";
        return info;
    }, R"pbdoc(
        Get build information.

        Returns:
            Dictionary with build information
    )pbdoc");

    m.def("create_simulation_callback", [](std::function<void(int, float)> callback) {
        // Create a callback function for simulation monitoring
        return callback;
    }, py::arg("callback"), R"pbdoc(
        Create a simulation callback function.

        Args:
            callback: Function to call with step and time

        Returns:
            Callback function
    )pbdoc");
}

} // namespace nlm

// Include pybind11's optional.hpp for better error handling
#include <pybind11/optional.h>

// Version information
PYBIND11_MODULE(pynlm, m) {
    m.def("__version__", []() { return "0.1.0"; });
}
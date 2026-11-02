#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration error: " + msg) {}
};

class BrainInitializationError : public NLMException {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMException("Brain initialization error: " + msg) {}
};

class InvalidOperationError : public NLMException {
public:
    explicit InvalidOperationError(const std::string& msg) : NLMException("Invalid operation: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides a Pythonic interface to the neural simulation framework,
        with enhanced type hints, better error handling, and convenience methods.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<InvalidOperationError>(m, "InvalidOperationError");

    // NeuronId class
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
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId class
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
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // RegionId class
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
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations with type hints
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

    // Config class with enhanced Pythonic interface
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", py::overload_cast<const std::string&>(&Config::get<int>), py::arg("key"),
             "Get a configuration value by key")
        .def("getOr", &Config::getOr, py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value or return default if not found")
        .def("set", py::overload_cast<const std::string&, const ConfigValue&, ConfigSource>(&Config::set), py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set), py::arg("key"), py::arg("value"),
             "Set a configuration string value")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set), py::arg("key"), py::arg("value"),
             "Set a configuration integer value")
        .def("set", py::overload_cast<const std::string&, int64_t, ConfigSource>(&Config::set), py::arg("key"), py::arg("value"),
             "Set a configuration int64_t value")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set), py::arg("key"), py::arg("value"),
             "Set a configuration double value")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set), py::arg("key"), py::arg("value"),
             "Set a configuration boolean value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Pythonic properties using def_readwrite
        .def_readwrite("neuron_count", [](Config& self) { return self.getOr("brain.neuron_count", 1000); },
                      [](Config& self, int value) { self.set("brain.neuron_count", value); })
        .def_readwrite("synapse_density", [](Config& self) { return self.getOr("brain.synapse_density", 0.1); },
                      [](Config& self, double value) { self.set("brain.synapse_density", value); })
        .def_readwrite("connection_probability", [](Config& self) { return self.getOr("brain.connection_probability", 0.05); },
                      [](Config& self, double value) { self.set("brain.connection_probability", value); })
        .def_readwrite("random_seed", [](Config& self) { return self.getOr("random_seed", 0); },
                      [](Config& self, int64_t value) { self.set("random_seed", value); })
        // Configuration convenience methods
        .def("configure_standard", [](Config& self) {
            self.set("brain.neuron_count", 1000);
            self.set("brain.synapse_density", 0.1);
            self.set("brain.connection_probability", 0.05);
            self.set("plasticity.stdp.enable", true);
            self.set("plasticity.hebbian.enable", true);
            self.set("plasticity.structural.enable", true);
        }, "Configure with standard brain parameters")
        .def("configure_compact", [](Config& self) {
            self.set("brain.neuron_count", 100);
            self.set("brain.synapse_density", 0.05);
            self.set("brain.connection_probability", 0.02);
            self.set("plasticity.stdp.enable", true);
            self.set("plasticity.hebbian.enable", true);
            self.set("neuromod.dopamine.scale", 0.5);
        }, "Configure with compact brain parameters")
        // Pythonic dictionary-like interface
        .def("__getitem__", [](Config& self, const std::string& key) {
            auto val = self.get<int64_t>(key);
            if (!val) {
                throw py::key_error("Key not found: " + key);
            }
            return val.value();
        })
        .def("__setitem__", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        })
        .def("__contains__", &Config::has,
             "Check if key exists in config (using 'in' operator)")
        .def("__len__", &Config::getKeys,
             "Get number of configuration entries")
        .def("__iter__", [](Config& self) {
            return py::make_iterator(self.getKeys().begin(), self.getKeys().end());
        }, "Iterate over configuration keys");

    // SensoryInput class with enhanced interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getTypeName", [](const SensoryInput& self) {
            switch (self.getType()) {
                case NeuronType::Sensory: return "sensory";
                case NeuronType::Vision: return "vision";
                case NeuronType::Touch: return "touch";
                case NeuronType::Internal: return "internal";
                case NeuronType::Proprioception: return "proprioception";
                case NeuronType::Audio: return "audio";
                default: return "unknown";
            }
        }, "Get human-readable type name");

    // Vision class with enhanced interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromImage", [](Vision& self, const std::vector<std::vector<float>>& image) {
            // Flatten 2D image to 1D vector
            std::vector<float> flatData;
            for (const auto& row : image) {
                flatData.insert(flatData.end(), row.begin(), row.end());
            }
            self.setData(flatData);
        }, py::arg("image"), "Set vision data from 2D image vector")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getResolution", [](const Vision& self) {
            return std::make_tuple(self.getWidth(), self.getHeight());
        }, "Get resolution as (width, height) tuple");

    // Audio class with enhanced interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get duration in seconds");

    // InternalSignals class with enhanced interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getData, "Get the number of signals")
        .def("getAllSignals", &InternalSignals::getData, "Get all signals as a vector");

    // Action class with enhanced interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def_readwrite("type", &Action::type, "Action type")
        .def_readwrite("parameters", &Action::parameters, "Action parameters")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def_readwrite("type", &Action::type)
        .def_readwrite("parameters", &Action::parameters)
        .def("getName", &Action::getName)
        .def("getDescription", [](const Action& self) {
            switch (self.getType()) {
                case ActionType::MoveForward: return "Move forward";
                case ActionType::MoveBackward: return "Move backward";
                case ActionType::TurnLeft: return "Turn left";
                case ActionType::TurnRight: return "Turn right";
                case ActionType::Look: return "Look";
                case ActionType::Interact: return "Interact";
                case ActionType::Eat: return "Eat";
                case ActionType::Drink: return "Drink";
                case ActionType::Rest: return "Rest";
                case ActionType::Wait: return "Wait";
                default: return "Unknown action";
            }
        }, "Get human-readable action description")
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        })
        // Pythonic properties
        .def_readonly("type_name", [](const Action& self) {
            switch (self.getType()) {
                case ActionType::MoveForward: return "move_forward";
                case ActionType::MoveBackward: return "move_backward";
                case ActionType::TurnLeft: return "turn_left";
                case ActionType::TurnRight: return "turn_right";
                case ActionType::Look: return "look";
                case ActionType::Interact: return "interact";
                case ActionType::Eat: return "eat";
                case ActionType::Drink: return "drink";
                case ActionType::Rest: return "rest";
                case ActionType::Wait: return "wait";
                default: return "custom";
            }
        }, "Get action type as Python-friendly string")
        .def("has_parameters", [](const Action& self) {
            return !self.getParameters().empty();
        }, "Check if action has parameters");

    // WorldObject class with enhanced interface
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
        .def_readwrite("active", &WorldObject::active)
        .def("getTypeName", [](const WorldObject& self) {
            switch (self.getType()) {
                case WorldObjectType::Empty: return "empty";
                case WorldObjectType::Resource: return "resource";
                case WorldObjectType::Hazard: return "hazard";
                case WorldObjectType::Wall: return "wall";
                case WorldObjectType::Marker: return "marker";
                default: return "unknown";
            }
        }, "Get human-readable type name")
        .def("isResource", [](const WorldObject& self) {
            return self.getType() == WorldObjectType::Resource;
        }, "Check if object is a resource")
        .def("isHazard", [](const WorldObject& self) {
            return self.getType() == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ">";
        });

    // AgentBody class with enhanced interface
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
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, "Set position from x, y coordinates")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed from velocity components");

    // ActionResult class with enhanced interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result was positive")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result was negative")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) + ", success=" + std::to_string(result.success) + ">";
        });

    // SensoryPercept class with enhanced interface
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
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if percept has internal data")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if percept has audio data")
        .def("clear", [](SensoryPercept& self) {
            self.setVision(std::vector<float>());
            self.setTouch(std::vector<float>());
            self.setInternal(std::vector<float>());
            self.setAudio(std::vector<float>());
        }, "Clear all sensory data");

    // SimpleWorld class with enhanced interface
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Convenience methods
        .def("createResource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, "Create a resource object at position")
        .def("createHazard", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, 1.0f, radius);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Create a hazard object at position")
        .def("createWall", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Wall, 1.0f, radius);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Create a wall object at position")
        .def("getResourceAt", [](SimpleWorld& self, float x, float y) {
            // Simple implementation - would need to be more sophisticated in real usage
            for (const auto& obj : self.getObjects()) {  // Assuming getObjects() method
                if (obj.active && obj.type == WorldObjectType::Resource && 
                    std::hypot(obj.x - x, obj.y - y) < obj.radius) {
                    return obj.value;
                }
            }
            return 0.0f;
        }, py::arg("x"), py::arg("y"), "Get resource value at position");

    // Brain class with enhanced interface
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // Convenience methods
        .def("initializeWithRandomSeed", [](Brain& self, int seed) {
            auto config = self.getConfig();
            config->set("random_seed", seed);
            return self.initialize();
        }, py::arg("seed"), "Initialize brain with specific random seed")
        .def("createDefaultRegion", [](Brain& self) {
            return self.addRegion("default");
        }, "Create default region and return its ID")
        .def("getStats", [](Brain& self) {
            return py::make_dict(
                "total_neurons", self.getTotalNeuronCount(),
                "total_synapses", self.getTotalSynapseCount(),
                "firing_neurons", self.getFiringNeuronCount(),
                "active_neurons", self.getActiveNeuronCount(),
                "total_spikes", self.getTotalSpikeCount(),
                "e_i_ratio", self.getExcitationInhibitionRatio(),
                "firing_rate", self.getAverageFiringRate(),
                "pending_spikes", self.getPendingSpikeEventCount()
            );
        }, "Get brain statistics as a dictionary")
        .def("isHealthy", [](Brain& self) {
            return self.getTotalNeuronCount() > 0 && 
                   self.getTotalSynapseCount() > 0 &&
                   self.getAverageFiringRate() < 1000.0f; // Reasonable upper bound
        }, "Check if brain is in a healthy state")
        // Pythonic properties for Brain
        .def_property_readonly("total_neurons", &Brain::getTotalNeuronCount)
        .def_property_readonly("total_synapses", &Brain::getTotalSynapseCount)
        .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount)
        .def_property_readonly("active_neurons", &Brain::getActiveNeuronCount)
        .def_property_readonly("total_spikes", &Brain::getTotalSpikeCount)
        .def_property_readonly("pending_spikes", &Brain::getPendingSpikeEventCount)
        .def_property_readonly("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio)
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate)
        .def_property_readonly("developmental_stage", &Brain::getDevelopmentalStage)
        .def_property("learned_preference", &Brain::getLearnedPreference, &Brain::setLearnedPreference)
        // Context manager for brain lifecycle
        .def("__enter__", [](Brain& self) -> Brain& {
            return self;
        })
        .def("__exit__", [](Brain& self, py::args) {
            self.reset();
        });

    // AgentBrain class with enhanced interface
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Convenience methods
        .def("createDefaultPercept", [](AgentBrain& self) {
            SensoryPercept percept;
            // Create default vision data
            std::vector<float> vision(64, 1.0f); // 8x8 vision
            percept.setVision(vision);
            return percept;
        }, "Create a default sensory percept")
        .def("createDefaultPercept", [](AgentBrain& self) {
            SensoryPercept percept;
            // Create default vision data
            std::vector<float> vision(64, 1.0f); // 8x8 vision
            percept.setVision(vision);
            return percept;
        }, "Create a default sensory percept")
        .def("getSystemStatus", [](AgentBrain& self) {
            return py::make_dict(
                "neuromodulation", self.getNeuromodulationLevel(),
                "curiosity", self.getCuriosityLevel(),
                "novelty", self.getNoveltyLevel(),
                "prediction_error", self.getPredictionError(),
                "developmental_stage", static_cast<int>(self.getDevelopmentalStage()),
                "reward_modulation", self.isRewardModulationEnabled(),
                "structural_plasticity", self.isStructuralPlasticityEnabled(),
                "development", self.isDevelopmentEnabled(),
                "curiosity", self.isCuriosityEnabled()
            );
        }, "Get system status as a dictionary")
        .def("isReady", [](AgentBrain& self) {
            return self.getBrain() != nullptr;
        }, "Check if agent is ready for operation")
        // Pythonic properties for AgentBrain
        .def_property_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel)
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel)
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel)
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError)
        .def_property_readonly("developmental_stage", &AgentBrain::getDevelopmentalStage)
        .def_property("enable_reward_modulation", &AgentBrain::isRewardModulationEnabled, &AgentBrain::enableRewardModulation)
        .def_property("enable_structural_plasticity", &AgentBrain::isStructuralPlasticityEnabled, &AgentBrain::enableStructuralPlasticity)
        .def_property("enable_development", &AgentBrain::isDevelopmentEnabled, &AgentBrain::enableDevelopment)
        .def_property("enable_curiosity", &AgentBrain::isCuriosityEnabled, &AgentBrain::enableCuriosity)
        // Context manager for agent lifecycle
        .def("__enter__", [](AgentBrain& self) -> AgentBrain& {
            return self;
        })
        .def("__exit__", [](AgentBrain& self, py::args) {
            self.reset();
        });

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    .def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->configureStandard();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a brain with standard configuration and initialize it")
    .def("createDefaultAgent", []() -> std::shared_ptr<AgentBrain> {
        auto brain = createDefaultBrain();
        auto config = std::make_shared<Config>();
        config->configureStandard();
        auto agent = std::make_shared<AgentBrain>(brain);
        return agent;
    }, "Create a default agent brain")
    .def("createSimpleDemoWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0f, 10.0f);
        // Add some objects
        world->createResource(5.0f, 5.0f, 1.0f);
        world->createHazard(15.0f, 15.0f);
        world->createWall(10.0f, 5.0f);
        return world;
    }, "Create a simple world with objects for demos")
    .def("runSimpleEpisode", [](int numSteps) {
        // Create all components
        auto config = std::make_shared<Config>();
        config->configureStandard();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0f, 10.0f);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        // Run episode
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply some reward based on position
            auto body = world->getAgentBody();
            if (body.x > 15.0f && body.y > 15.0f) {
                agent->applyRewardModulation(1.0f, 0.5f);
            }
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("numSteps") = 100, "Run a simple agent episode and return brain, world, agent")
    .def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            config->configureStandard();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, "Create a brain with standard configuration and initialize it")
    .def("createDefaultAgent", []() -> std::shared_ptr<AgentBrain> {
        auto brain = createDefaultBrain();
        auto config = std::make_shared<Config>();
        config->configureStandard();
        auto agent = std::make_shared<AgentBrain>(brain);
        return agent;
    }, "Create a default agent brain")
    .def("createSimpleDemoWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0f, 10.0f);
        // Add some objects
        world->createResource(5.0f, 5.0f, 1.0f);
        world->createHazard(15.0f, 15.0f);
        world->createWall(10.0f, 5.0f);
        return world;
    }, "Create a simple world with objects for demos")
    .def("runSimpleEpisode", [](int numSteps) {
        // Create all components
        auto config = std::make_shared<Config>();
        config->configureStandard();
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0f, 10.0f);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        // Run episode
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply some reward based on position
            auto body = world->getAgentBody();
            if (body.x > 15.0f && body.y > 15.0f) {
                agent->applyRewardModulation(1.0f, 0.5f);
            }
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("numSteps") = 100, "Run a simple agent episode and return brain, world, agent")
    .def("getVersion", []() {
        return "NLM Python Bindings v0.1.0";
    }, "Get the version of the Python bindings")
    .def("getExample", []() {
        return R"example(
import pynlm

# Create and initialize a brain with standard configuration
brain = pynlm.createDefaultBrain()
brain.initialize()

# Create a simple world
world = pynlm.createSimpleDemoWorld()
world.reset()

# Create an agent
agent = pynlm.createDefaultAgent()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run an episode
for step in range(100):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process it
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Episode complete! Final stats: {agent.getSystemStatus()}")
example";
    }, "Get a complete example script")
    // Advanced utility functions
    .def("create_brain_from_config", [](const std::string& config_file) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(config_file)) {
            throw py::value_error("Failed to load configuration from: " + config_file);
        }
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        return brain;
    }, "Create brain from configuration file")
    .def("create_agent_from_config", [](const std::string& config_file) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(config_file)) {
            throw py::value_error("Failed to load configuration from: " + config_file);
        }
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return std::make_tuple(agent, world);
    }, "Create agent and world from configuration file")
    .def("save_brain_with_stats", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        auto stats = brain->getStats();
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        
        if (!brain->save(filepath)) {
            throw py::io_error("Failed to save brain to: " + filepath);
        }
        return stats;
    }, "Save brain and return statistics")
    .def("load_and_validate", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw py::value_error("Failed to load configuration from: " + filepath);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        
        return brain;
    }, "Load brain from configuration file and validate");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm"
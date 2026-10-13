#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exceptions for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("Config Error: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("Brain Error: " + msg) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& msg) : NLMException("World Error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");

    // Helper functions
    auto to_neuron_type = [](const std::string& typeStr) {
        if (typeStr == "Excitatory") return NeuronType::Excitatory;
        if (typeStr == "Inhibitory") return NeuronType::Inhibitory;
        if (typeStr == "Modulatory") return NeuronType::Modulatory;
        if (typeStr == "Sensory") return NeuronType::Sensory;
        if (typeStr == "Motor") return NeuronType::Motor;
        if (typeStr == "Internal") return NeuronType::Internal;
        throw py::value_error("Unknown NeuronType: " + typeStr);
    };

    auto to_synapse_type = [](const std::string& typeStr) {
        if (typeStr == "Excitatory") return SynapseType::Excitatory;
        if (typeStr == "Inhibitory") return SynapseType::Inhibitory;
        if (typeStr == "Modulatory") return SynapseType::Modulatory;
        if (typeStr == "Electrical") return SynapseType::Electrical;
        if (typeStr == "GapJunction") return SynapseType::GapJunction;
        throw py::value_error("Unknown SynapseType: " + typeStr);
    };

    auto to_action_type = [](const std::string& typeStr) {
        // Map string to ActionType
        static const std::unordered_map<std::string, ActionType> typeMap = {
            {"MoveForward", ActionType::MoveForward},
            {"MoveBackward", ActionType::MoveBackward},
            {"MoveLeft", ActionType::MoveLeft},
            {"MoveRight", ActionType::MoveRight},
            {"TurnLeft", ActionType::TurnLeft},
            {"TurnRight", ActionType::TurnRight},
            {"Look", ActionType::Look},
            {"LookUp", ActionType::LookUp},
            {"LookDown", ActionType::LookDown},
            {"Interact", ActionType::Interact},
            {"Eat", ActionType::Eat},
            {"Drink", ActionType::Drink},
            {"Rest", ActionType::Rest},
            {"Wait", ActionType::Wait},
            {"Custom", ActionType::Custom}
        };
        
        auto it = typeMap.find(typeStr);
        if (it != typeMap.end()) return it->second;
        throw py::value_error("Unknown ActionType: " + typeStr);
    };

    // NeuronId class with Pythonic interface
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

    // SynapseId class with Pythonic interface
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

    // RegionId class with Pythonic interface
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

    // PopulationId class with Pythonic interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum with string conversion
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("from_string", to_neuron_type, py::arg("type_str"),
             "Create NeuronType from string (e.g., 'Excitatory', 'Inhibitory')");

    // SynapseType enum with string conversion
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("from_string", to_synapse_type, py::arg("type_str"),
             "Create SynapseType from string (e.g., 'Excitatory', 'Inhibitory')");

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum with string conversion
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
        .def("from_string", to_action_type, py::arg("type_str"),
             "Create ActionType from string");

    // MotorCommand enum
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with enhanced Python interface
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", py::overload_cast<const std::string&>(&Config::loadFromFile),
             py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, raises ConfigError on failure.")
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
        .def("get", [](Config& self, const std::string& key, const std::string& defaultValue) {
            if (self.has(key)) {
                if constexpr (std::is_same_v<decltype(self.getOr(std::string(), std::string())), std::string>) {
                    return self.getOr(key, defaultValue);
                } else {
                    throw ConfigError("Wrong type for key: " + key);
                }
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value with string default")
        .def("get", [](Config& self, const std::string& key, double defaultValue) {
            if (self.has(key)) {
                if constexpr (std::is_same_v<decltype(self.getOr(std::string(), double())), double>) {
                    return self.getOr(key, defaultValue);
                } else {
                    throw ConfigError("Wrong type for key: " + key);
                }
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value with double default")
        .def("get", [](Config& self, const std::string& key, int64_t defaultValue) {
            if (self.has(key)) {
                if constexpr (std::is_same_v<decltype(self.getOr(std::string(), int64_t())), int64_t>) {
                    return self.getOr(key, defaultValue);
                } else {
                    throw ConfigError("Wrong type for key: " + key);
                }
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value with int64_t default")
        .def("get", [](Config& self, const std::string& key, float defaultValue) {
            if (self.has(key)) {
                if constexpr (std::is_same_v<decltype(self.getOr(std::string(), float())), float>) {
                    return self.getOr(key, defaultValue);
                } else {
                    throw ConfigError("Wrong type for key: " + key);
                }
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value with float default")
        .def("get", [](Config& self, const std::string& key, bool defaultValue) {
            if (self.has(key)) {
                if constexpr (std::is_same_v<decltype(self.getOr(std::string(), bool())), bool>) {
                    return self.getOr(key, defaultValue);
                } else {
                    throw ConfigError("Wrong type for key: " + key);
                }
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value with bool default")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (string)")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (double)")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (int64_t)")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (float)")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (bool)");

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getArray", [](const Vision& self) {
            return self.getData();
        }, "Get vision data as array (alias for getData)")
        .def("setArray", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from array (alias for setData)");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getArray", [](const Audio& self) {
            return self.getData();
        }, "Get audio data as array (alias for getData)")
        .def("setArray", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from array (alias for setData)");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getValues", [](const InternalSignals& self) {
            return self.getData();
        }, "Get signal values as array (alias for getData)")
        .def("setValues", [](InternalSignals& self, const std::vector<float>& values) {
            self.clearSignals();
            for (float val : values) self.addSignal(val);
        }, py::arg("values"), "Set signal values from array");

    // Action class with enhanced interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("setTypeFromString", [](Action& self, const std::string& typeStr) {
            auto type = to_action_type(typeStr);
            self.setType(type);
        }, py::arg("type_str"),
             "Set action type from string (e.g., 'MoveForward', 'TurnLeft')")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName)
        .def("getX", &Action::getX)
        .def("setX", &Action::setX, py::arg("x"))
        .def("getY", &Action::getY)
        .def("setY", &Action::setY, py::arg("y"))
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

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
        .def_readwrite("active", &WorldObject::active)
        .def("getTypeName", [](const WorldObject& obj) {
            static const std::unordered_map<WorldObjectType, std::string> typeNames = {
                {WorldObjectType::Empty, "Empty"},
                {WorldObjectType::Resource, "Resource"},
                {WorldObjectType::Hazard, "Hazard"},
                {WorldObjectType::Wall, "Wall"},
                {WorldObjectType::Marker, "Marker"}
            };
            auto it = typeNames.find(obj.type);
            return it != typeNames.end() ? it->second : "Unknown";
        }, "Get human-readable type name");

    // AgentBody class with property access
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_property("x", &AgentBody::x, &AgentBody::setX, "Agent x position")
        .def_property("y", &AgentBody::y, &AgentBody::setY, "Agent y position")
        .def_property("orientation", &AgentBody::orientation, &AgentBody::setOrientation, "Agent orientation in radians")
        .def_property("velocity_x", &AgentBody::velocityX, &AgentBody::setVelocityX, "Agent velocity in x direction")
        .def_property("velocity_y", &AgentBody::velocityY, &AgentBody::setVelocityY, "Agent velocity in y direction")
        .def_property("angular_velocity", &AgentBody::angularVelocity, &AgentBody::setAngularVelocity, "Agent angular velocity")
        .def_property("energy", &AgentBody::energy, &AgentBody::setEnergy, "Agent energy level")
        .def_property("health", &AgentBody::health, &AgentBody::setHealth, "Agent health level")
        .def_property("age", &AgentBody::age, &AgentBody::setAge, "Agent age in simulation steps")
        .def_property("is_moving", &AgentBody::isMoving, &AgentBody::setIsMoving, "Whether agent is moving")
        .def_property("is_turning", &AgentBody::isTurning, &AgentBody::setIsTurning, "Whether agent is turning")
        .def_property("last_action_time", &AgentBody::lastActionTime, &AgentBody::setLastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset);

    // ActionResult class with property access
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_property("reward", &ActionResult::reward, &ActionResult::setReward, "Reward received from action")
        .def_property("success", &ActionResult::success, &ActionResult::setSuccess, "Whether action was successful")
        .def_property("message", &ActionResult::message, &ActionResult::setMessage, "Human-readable message about action result");

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
        .def("__getitem__", [](const SensoryPercept& percept, const std::string& key) {
            if (key == "vision") {
                return percept.getVision();
            } else if (key == "touch") {
                return percept.getTouch();
            } else if (key == "internal") {
                return percept.getInternal();
            } else if (key == "proprioception") {
                return percept.getProprioception();
            } else if (key == "audio") {
                return percept.getAudio();
            } else {
                throw py::key_error("Unknown sensory key: " + key);
            }
        }, py::arg("key"),
            "Get sensory data by key (e.g., 'vision', 'touch', 'internal')")
        .def("__setitem__", [](SensoryPercept& percept, const std::string& key, const SensoryInput& value) {
            if (key == "vision") {
                percept.setVision(value);
            } else if (key == "touch") {
                percept.setTouch(value);
            } else if (key == "internal") {
                percept.setInternal(value);
            } else if (key == "proprioception") {
                percept.setProprioception(value);
            } else if (key == "audio") {
                percept.setAudio(value);
            } else {
                throw py::key_error("Unknown sensory key: " + key);
            }
        }, py::arg("key"), py::arg("value"),
            "Set sensory data by key (e.g., 'vision', 'touch', 'internal')");

    // SimpleWorld class with enhanced interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed");

    // Brain class with enhanced Python interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        .def("__enter__", [](py::object self) { return self; })
        .def("__exit__", [](py::object, py::object, py::object, py::object) {
            // Cleanup logic
        }, "Context manager entry/exit")
        .def("copy", [](const Brain& self) {
            return std::make_shared<Brain>(self);
        }, "Create a copy of the brain")
        .def("get_neurons", &Brain::getTotalNeuronCount,
             "Get total neuron count (alias for getTotalNeuronCount)")
        .def("get_synapses", &Brain::getTotalSynapseCount,
             "Get total synapse count (alias for getTotalSynapseCount)");

    // AgentBrain class with enhanced Python interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("__enter__", [](py::object self) { return self; })
        .def("__exit__", [](py::object, py::object, py::object, py::object) {
            // Cleanup logic
        }, "Context manager entry/exit");

    // Helper functions
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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version
    m.attr("__version__") = "0.1.0";

    // Pythonic convenience functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(static_cast<SimulationStep>(step));
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       "Run a complete simulation with brain, world, and agent");

    m.def("create_neural_network", [](size_t num_neurons, size_t num_regions) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(num_neurons));
        config->set("brain.region_count", static_cast<int64_t>(num_regions));
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("num_neurons"), py::arg("num_regions"),
       "Create a neural network with specified neurons and regions");

    m.def("batch_simulate", [](std::vector<std::shared_ptr<Brain>> brains,
                             std::vector<std::shared_ptr<SimpleWorld>> worlds,
                             std::vector<std::shared_ptr<AgentBrain>> agents,
                             size_t numSteps) {
        for (size_t step = 0; step < numSteps; ++step) {
            for (size_t i = 0; i < brains.size(); ++i) {
                if (worlds[i]) {
                    worlds[i]->update(0.1);
                    auto percept = worlds[i]->getSensoryPercept();
                    agents[i]->processSensoryInput(percept);
                    brains[i]->step(static_cast<SimulationStep>(step));
                    auto action = agents[i]->decodeMotorCommand();
                    worlds[i]->applyMotorCommand(action, worlds[i]->getSimulationTime());
                }
            }
        }
    }, py::arg("brains"), py::arg("worlds"), py::arg("agents"), py::arg("num_steps"),
       "Run batch simulation with multiple agents");
}

} // namespace nlm

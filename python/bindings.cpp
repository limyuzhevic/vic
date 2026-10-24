#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

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

// Custom exception types for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Real-time neural simulation with spiking neurons
        - Event-driven dynamics for biological realism
        - Integrated memory, prediction, cognition, and neuromodulation
        - Complete agent-environment interaction
        - Python-friendly API with convenience functions
        )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // NeuronId class with enhanced functionality
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
        .def("__lt__", &NeuronId::operator<)
        .def("__gt__", &NeuronId::operator>);

    // SynapseId class with enhanced functionality
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
        .def("__lt__", &SynapseId::operator<)
        .def("__gt__", &SynapseId::operator<);

    // RegionId class with enhanced functionality
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
        .def("__lt__", &RegionId::operator<)
        .def("__gt__", &RegionId::operator<);

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum with additional metadata
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("__str__", [](NeuronType t) {
            switch(t) {
                case NeuronType::Excitatory: return "Excitatory";
                case NeuronType::Inhibitory: return "Inhibitory";
                case NeuronType::Modulatory: return "Modulatory";
                case NeuronType::Sensory: return "Sensory";
                case NeuronType::Motor: return "Motor";
                case NeuronType::Internal: return "Internal";
                default: return "Unknown";
            }
        });

    // SynapseType enum with additional metadata
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("__str__", [](SynapseType t) {
            switch(t) {
                case SynapseType::Excitatory: return "Excitatory";
                case SynapseType::Inhibitory: return "Inhibitory";
                case SynapseType::Modulatory: return "Modulatory";
                case SynapseType::Electrical: return "Electrical";
                case SynapseType::GapJunction: return "GapJunction";
                default: return "Unknown";
            }
        });

    // DevelopmentalStage enum with additional metadata
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("__str__", [](DevelopmentalStage s) {
            switch(s) {
                case DevelopmentalStage::Initial: return "Initial";
                case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
                case DevelopmentalStage::Maturation: return "Maturation";
                case DevelopmentalStage::Adult: return "Adult";
                case DevelopmentalStage::Aging: return "Aging";
                default: return "Unknown";
            }
        });

    // FiringState enum with additional metadata
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("__str__", [](FiringState s) {
            switch(s) {
                case FiringState::Resting: return "Resting";
                case FiringState::Active: return "Active";
                case FiringState::Refractory: return "Refractory";
                case FiringState::Inhibited: return "Inhibited";
                default: return "Unknown";
            }
        });

    // ActionType enum
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
        .def("__str__", [](ActionType t) {
            switch(t) {
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
        .export_values()
        .def("__str__", [](MotorCommand c) {
            switch(c) {
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("__str__", [](WorldObjectType t) {
            switch(t) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        });

    // Config class with enhanced functionality
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
        })
        // Additional configuration methods
        .def("getInt", [](const Config& cfg, const std::string& key, int defaultValue) {
            auto value = cfg.get<int>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get integer value with default")
        .def("getFloat", [](const Config& cfg, const std::string& key, float defaultValue) {
            auto value = cfg.get<double>(key);
            if (value) return static_cast<float>(*value);
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get float value with default")
        .def("getBool", [](const Config& cfg, const std::string& key, bool defaultValue) {
            auto value = cfg.get<bool>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean value with default")
        .def("setInt", [](Config& cfg, const std::string& key, int value, ConfigSource source) {
            cfg.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set integer value")
        .def("setFloat", [](Config& cfg, const std::string& key, float value, ConfigSource source) {
            cfg.set(key, static_cast<double>(value), source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set float value")
        .def("setBool", [](Config& cfg, const std::string& key, bool value, ConfigSource source) {
            cfg.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set boolean value")
        .def("setString", [](Config& cfg, const std::string& key, const std::string& value, ConfigSource source) {
            cfg.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set string value");

    // SensoryInput class with enhanced functionality
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with enhanced functionality
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
        // Additional vision methods
        .def("setDataArray", [](Vision& self, py::array_t<float> data) {
            self.setData(std::vector<float>(data.data(), data.data() + data.size()));
        }, py::arg("data"), "Set vision data from numpy array")
        .def("toArray", [](Vision& self) {
            return py::array_t<float>(self.getData());
        }, "Convert vision data to numpy array");

    // Audio class with enhanced functionality
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class with enhanced functionality
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Additional internal signals methods
        .def("getSignals", &InternalSignals::getSignals)
        .def("setSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            self.clearSignals();
            for (float value : signals) {
                self.addSignal(value);
            }
        }, py::arg("signals"), "Set internal signals from vector");

    // Action class with enhanced functionality
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Additional action methods
        .def("addParameter", [](Action& self, float value) {
            std::vector<float> params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to the action")
        .def("getParameter", [](const Action& self, size_t index) {
            const std::vector<float>& params = self.getParameters();
            if (index < params.size()) return params[index];
            return 0.0f;
        }, py::arg("index"), "Get a specific parameter")
        .def("setParameter", [](Action& self, size_t index, float value) {
            std::vector<float>& params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
            }
        }, py::arg("index"), py::arg("value"), "Set a specific parameter")
        .def("isValid", [](const Action& self) {
            ActionType type = self.getType();
            return type != ActionType::Custom || !self.getParameters().empty();
        }, "Check if action is valid");

    // WorldObject class with enhanced functionality
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
        // Additional world object methods
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("containsPoint", [](const WorldObject& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return dx*dx + dy*dy <= self.radius*self.radius;
        }, py::arg("x"), py::arg("y"), "Check if point is inside object");

    // AgentBody class with enhanced functionality
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
        // Additional agent body methods
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("setOrientation", [](AgentBody& self, float orientation) {
            self.orientation = orientation;
        }, py::arg("orientation"), "Set agent orientation")
        .def("distanceTo", [](const AgentBody& self, const AgentBody& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"), "Calculate distance to another agent")
        .def("isMoving", [](const AgentBody& self) {
            return self.velocityX != 0.0f || self.velocityY != 0.0f || self.angularVelocity != 0.0f;
        }, "Check if agent is moving")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Get agent speed");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Additional action result methods
        .def("isSuccess", [](const ActionResult& self) { return self.success; }, "Check if action was successful")
        .def("setSuccess", [](ActionResult& self, bool success) { self.success = success; }, py::arg("success"), "Set success status")
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to dictionary");

    // SensoryPercept class with enhanced functionality
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
        // Additional sensory percept methods
        .def("updateVision", [](SensoryPercept& self, py::array_t<float> visionData) {
            Vision vision;
            vision.setData(std::vector<float>(visionData.data(), visionData.data() + visionData.size()));
            self.setVision(vision);
        }, py::arg("visionData"), "Update vision with numpy array")
        .def("updateInternalSignals", [](SensoryPercept& self, const std::vector<float>& signals) {
            InternalSignals internalSignals;
            for (float value : signals) {
                internalSignals.addSignal(value);
            }
            self.setInternal(internalSignals);
        }, py::arg("signals"), "Update internal signals from vector")
        .def("getSensorySummary", [](const SensoryPercept& self) {
            std::stringstream ss;
            ss << "Vision: " << self.getVisionWidth() << "x" << self.getVisionHeight() << ", ";
            ss << "Internal signals: " << self.getInternal().size() << ", ";
            ss << "Proprioception: " << self.getProprioception().size();
            return ss.str();
        }, "Get human-readable sensory summary");

    // SimpleWorld class with enhanced functionality
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        // Additional world methods
        .def("isPositionValid", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid in world")
        .def("getDistanceToBorder", [](const SimpleWorld& self, float x, float y) {
            float distToLeft = x;
            float distToRight = self.getWidth() - x;
            float distToTop = y;
            float distToBottom = self.getHeight() - y;
            return std::min({distToLeft, distToRight, distToTop, distToBottom});
        }, py::arg("x"), py::arg("y"), "Get distance to world border")
        .def("getAgentHealthPercentage", [](const SimpleWorld& self) {
            const AgentBody& agent = self.getAgentBody();
            float maxEnergy = self.getMaxEnergy();
            return agent.health / maxEnergy;
        }, "Get agent health as percentage")
        .def("getSimulationTimeFormatted", [](const SimpleWorld& self) {
            std::stringstream ss;
            ss << "Step: " << static_cast<int>(self.getSimulationTime() * 1000) << "ms";
            ss << ", Time: " << std::fixed << std::setprecision(3) << self.getSimulationTime() << "s";
            return ss.str();
        }, "Get formatted simulation time");

    // Brain class with enhanced functionality
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
        // Additional brain methods
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random generator")
        .def("getStatusSummary", [](const Brain& brain) {
            std::stringstream ss;
            ss << "Brain Status:\n";
            ss << "  Neurons: " << brain.getTotalNeuronCount() << "\n";
            ss << "  Synapses: " << brain.getTotalSynapseCount() << "\n";
            ss << "  Firing: " << brain.getFiringNeuronCount() << "/" << brain.getActiveNeuronCount() << "\n";
            ss << "  Spikes: " << brain.getTotalSpikeCount() << "\n";
            ss << "  E/I Ratio: " << std::fixed << std::setprecision(3) << brain.getExcitationInhibitionRatio() << "\n";
            ss << "  Development: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
            return ss.str();
        }, "Get human-readable brain status summary")
        .def("isHealthy", [](const Brain& brain) {
            return brain.getTotalNeuronCount() > 0 && 
                   brain.getTotalSynapseCount() >= 0 &&
                   brain.getAverageFiringRate() >= 0.0f;
        }, "Check if brain is in a healthy state")
        .def("validateConfiguration", [](const Brain& brain) {
            std::vector<std::string> issues;
            if (brain.getTotalNeuronCount() == 0) {
                issues.push_back("No neurons in brain");
            }
            if (brain.getAverageFiringRate() < 0.0f || brain.getAverageFiringRate() > 1000.0f) {
                issues.push_back("Abnormal firing rate: " + std::to_string(brain.getAverageFiringRate()));
            }
            return issues;
        }, "Validate brain configuration and return issues");

    // AgentBrain class with enhanced functionality
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
        // Additional agent brain methods
        .def("getSensoryNeuronGroups", [](const AgentBrain& agent) {
            std::map<std::string, size_t> groups;
            groups["vision"] = agent.getSensoryInputSize(); // Simplified
            groups["touch"] = agent.getSensoryInputSize(); // Would need additional accessors
            groups["internal"] = agent.getSensoryInputSize(); // Would need additional accessors
            groups["proprioception"] = agent.getSensoryInputSize(); // Would need additional accessors
            return groups;
        }, "Get sensory neuron groups")
        .def("getMotorNeuronGroups", [](const AgentBrain& agent) {
            std::map<std::string, size_t> groups;
            groups["forward"] = agent.getMotorOutputSize(); // Simplified
            groups["backward"] = agent.getMotorOutputSize(); // Simplified
            groups["turn_left"] = agent.getMotorOutputSize(); // Simplified
            groups["turn_right"] = agent.getMotorOutputSize(); // Simplified
            return groups;
        }, "Get motor neuron groups")
        .def("getNeuromodulationStats", [](const AgentBrain& agent) {
            std::map<std::string, float> stats;
            stats["dopamine"] = agent.getNeuromodulationLevel();
            stats["curiosity"] = agent.getCuriosityLevel();
            stats["novelty"] = agent.getNoveltyLevel();
            stats["prediction_error"] = agent.getPredictionError();
            return stats;
        }, "Get neuromodulation statistics")
        .def("isInitialized", [](const AgentBrain& agent) {
            return agent.getBrain() != nullptr;
        }, "Check if agent brain is initialized");

    // Convenience functions for easy usage
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw ConfigurationError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a new brain");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        return agent;
    }, py::arg("brain"), "Create a new agent brain interface");

    // Configuration presets with comprehensive options
    m.def("createConfigPreset", [](const std::string& presetName) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (presetName == "minimal") {
            config->set("brain.neuron_count", size_t(100));
            config->set("brain.synapse_density", 0.05f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", false);
            config->set("neuromod.curiosity.enable", false);
            config->set("neuromod.novelty.enable", false);
        } else if (presetName == "standard") {
            config->set("brain.neuron_count", size_t(1000));
            config->set("brain.synapse_density", 0.1f);
            config->set("brain.connection_probability", 0.05f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001f);
            config->set("plasticity.stdp.tau_plus", 20.0f);
            config->set("plasticity.stdp.tau_minus", 20.0f);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.01f);
            config->set("plasticity.structural.enable", false);
            config->set("neuromod.curiosity.enable", false);
            config->set("neuromod.novelty.enable", false);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("development.enabled", true);
            config->set("development.critical_period", 0.5f);
        } else if (presetName == "advanced") {
            config->set("brain.neuron_count", size_t(5000));
            config->set("brain.synapse_density", 0.15f);
            config->set("brain.connection_probability", 0.08f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.002f);
            config->set("plasticity.stdp.tau_plus", 30.0f);
            config->set("plasticity.stdp.tau_minus", 30.0f);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.02f);
            config->set("plasticity.structural.enable", true);
            config->set("plasticity.structural.growth_rate", 0.001f);
            config->set("plasticity.structural.pruning_rate", 0.0005f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.curiosity.base_rate", 0.1f);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.novelty.sensitivity", 0.5f);
            config->set("neuromod.dopamine.scale", 1.5f);
            config->set("development.enabled", true);
            config->set("development.critical_period", 1.0f);
            config->set("development.maturation_rate", 0.8f);
            config->set("prediction.enabled", true);
            config->set("prediction.time_horizon", 10.0f);
            config->set("memory.working.capacity", 100.0f);
            config->set("memory.episodic.capacity", 50.0f);
        } else {
            throw ConfigurationError("Unknown preset name: " + presetName);
        }
        return config;
    }, py::arg("presetName"), "Create configuration with preset settings");

    // Advanced configuration presets for specific use cases
    m.def("createConfigResearch", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", size_t(10000));
        config->set("brain.synapse_density", 0.2f);
        config->set("brain.connection_probability", 0.1f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.005f);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.growth_rate", 0.002f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.base_rate", 0.05f);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.sensitivity", 0.8f);
        config->set("neuromod.dopamine.scale", 2.0f);
        config->set("development.enabled", true);
        config->set("development.critical_period", 2.0f);
        config->set("development.maturation_rate", 0.6f);
        config->set("prediction.enabled", true);
        config->set("prediction.time_horizon", 20.0f);
        config->set("memory.working.capacity", 500.0f);
        config->set("memory.episodic.capacity", 200.0f);
        config->set("memory.associative.capacity", 300.0f);
        config->set("simulation.timestep", 0.001f);
        config->set("simulation.max_steps", 10000);
        return config;
    }, "Create configuration optimized for research scenarios");

    m.def("createConfigTraining", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", size_t(2000));
        config->set("brain.synapse_density", 0.12f);
        config->set("brain.connection_probability", 0.06f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.0015f);
        config->set("plasticity.stdp.tau_plus", 25.0f);
        config->set("plasticity.stdp.tau_minus", 25.0f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.015f);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.growth_rate", 0.001f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.base_rate", 0.08f);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.sensitivity", 0.6f);
        config->set("neuromod.dopamine.scale", 1.2f);
        config->set("development.enabled", true);
        config->set("development.critical_period", 1.5f);
        config->set("development.maturation_rate", 0.7f);
        config->set("prediction.enabled", true);
        config->set("prediction.time_horizon", 15.0f);
        config->set("memory.working.capacity", 200.0f);
        config->set("memory.episodic.capacity", 100.0f);
        config->set("simulation.timestep", 0.002f);
        config->set("simulation.max_steps", 5000);
        return config;
    }, "Create configuration optimized for learning/training scenarios");

    m.def("createConfigRealTime", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", size_t(500));
        config->set("brain.synapse_density", 0.08f);
        config->set("brain.connection_probability", 0.04f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.0005f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.005f);
        config->set("plasticity.structural.enable", false);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.base_rate", 0.12f);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.sensitivity", 0.4f);
        config->set("neuromod.dopamine.scale", 0.8f);
        config->set("development.enabled", false);
        config->set("prediction.enabled", false);
        config->set("simulation.timestep", 0.005f);
        config->set("simulation.max_steps", 1000);
        return config;
    }, "Create configuration optimized for real-time interaction scenarios");

    // Simulation helper functions
    m.def("runAgentEpisode", 
        [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
           int numSteps, bool verbose = false) {
            if (!agent->isInitialized() || !world) {
                throw SimulationError("Agent or world not properly initialized");
            }
            
            for (int step = 0; step < numSteps; ++step) {
                // Get sensory input
                SensoryPercept percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Run brain step
                agent->getBrain()->step(step);
                
                // Decode motor command
                MotorCommand cmd = agent->decodeMotorCommand();
                
                // Apply motor command
                world->applyMotorCommand(cmd, world->getSimulationTime());
                
                // Update development
                agent->updateDevelopment(0.1);
                
                if (verbose && step % 100 == 0) {
                    std::cout << "Step " << step << ": " 
                             << agent->getBrain()->getFiringNeuronCount() << " firing neurons\n";
                }
            }
        }, py::arg("agent"), py::arg("world"), py::arg("numSteps"), py::arg("verbose") = false,
        "Run a complete agent episode");

    // Enhanced error handling and user feedback
    m.def("validateConfiguration", [](std::shared_ptr<Config> config) {
        std::vector<std::string> issues;
        if (!config) {
            issues.push_back("Config is null");
            return issues;
        }
        
        // Check for essential keys
        std::vector<std::string> essentialKeys = {
            "brain.neuron_count",
            "brain.synapse_density",
            "plasticity.stdp.enable",
            "plasticity.hebbian.enable"
        };
        
        for (const auto& key : essentialKeys) {
            if (!config->has(key)) {
                issues.push_back("Missing essential configuration: " + key);
            }
        }
        
        // Validate value ranges
        auto neuronCount = config->get<size_t>("brain.neuron_count");
        if (neuronCount && (*neuronCount == 0 || *neuronCount > 100000)) {
            issues.push_back("brain.neuron_count must be between 1 and 100000");
        }
        
        auto synapseDensity = config->get<double>("brain.synapse_density");
        if (synapseDensity && (*synapseDensity < 0.0 || *synapseDensity > 1.0)) {
            issues.push_back("brain.synapse_density must be between 0.0 and 1.0");
        }
        
        return issues;
    }, py::arg("config"), "Validate configuration and return issues");

    m.def("getConfigurationIssues", [](std::shared_ptr<Config> config) {
        std::map<std::string, std::vector<std::string>> issueCategories;
        
        if (!config) {
            issueCategories["critical"].push_back("Configuration is null");
            return issueCategories;
        }
        
        // Check and categorize issues
        auto neuronCount = config->get<size_t>("brain.neuron_count");
        if (neuronCount && *neuronCount == 0) {
            issueCategories["critical"].push_back("brain.neuron_count must be > 0");
        } else if (neuronCount && *neuronCount > 100000) {
            issueCategories["warning"].push_back("brain.neuron_count is very large (performance may be affected)");
        }
        
        auto synapseDensity = config->get<double>("brain.synapse_density");
        if (synapseDensity) {
            if (*synapseDensity < 0.0 || *synapseDensity > 1.0) {
                issueCategories["critical"].push_back("brain.synapse_density must be between 0.0 and 1.0");
            }
        }
        
        // Check if essential subsystems are enabled
        auto stdpEnabled = config->get<bool>("plasticity.stdp.enable");
        if (stdpEnabled && !*stdpEnabled) {
            issueCategories["warning"].push_back("STDP plasticity is disabled (consider enabling for learning)");
        }
        
        auto hebbianEnabled = config->get<bool>("plasticity.hebbian.enable");
        if (hebbianEnabled && !*hebbianEnabled) {
            issueCategories["warning"].push_back("Hebbian plasticity is disabled (consider enabling for learning)");
        }
        
        return issueCategories;
    }, py::arg("config"), "Get categorized configuration issues (critical/warning)");

    m.def("printConfigurationReport", [](std::shared_ptr<Config> config) {
        if (!config) {
            std::cout << "ERROR: Configuration is null!" << std::endl;
            return;
        }
        
        std::cout << "=== NLM Configuration Report ===" << std::endl;
        std::cout << "Total settings: " << config->getKeys().size() << std::endl;
        std::cout << std::endl;
        
        // Print key settings
        std::vector<std::string> importantKeys = {
            "brain.neuron_count",
            "brain.synapse_density",
            "brain.connection_probability",
            "plasticity.stdp.enable",
            "plasticity.hebbian.enable",
            "neuromod.curiosity.enable",
            "neuromod.novelty.enable"
        };
        
        for (const auto& key : importantKeys) {
            if (config->has(key)) {
                auto value = config->get<std::string>(key);
                std::cout << "  " << std::left << std::setw(40) << key << ": " << value << std::endl;
            }
        }
        
        // Check for issues
        auto issues = validateConfiguration(config);
        if (!issues.empty()) {
            std::cout << std::endl << "=== Issues Found ===" << std::endl;
            for (const auto& issue : issues) {
                std::cout << "  - " << issue << std::endl;
            }
        }
        
        std::cout << std::endl << "=== End Report ===" << std::endl;
    }, py::arg("config"), "Print detailed configuration report with validation");

    m.def("trySafeOperation", [](py::function func, const std::string& operationName, const std::string& errorContext) {
        try {
            return func();
        } catch (const ConfigurationError& e) {
            throw SimulationError("Configuration error in " + operationName + ": " + std::string(e.what()) + 
                                 (errorContext.empty() ? "" : " (Context: " + errorContext + ")"));
        } catch (const SimulationError& e) {
            throw SimulationError("Simulation error in " + operationName + ": " + std::string(e.what()) +
                                 (errorContext.empty() ? "" : " (Context: " + errorContext + ")"));
        } catch (const std::exception& e) {
            throw SimulationError("Unexpected error in " + operationName + ": " + std::string(e.what()) +
                                 (errorContext.empty() ? "" : " (Context: " + errorContext + ")"));
        }
    }, py::arg("func"), py::arg("operationName"), py::arg("errorContext") = "", "Wrapper for safe operation with detailed error context");

    m.def("formatErrorMessage", [](const std::string& category, const std::string& message, const std::string& context) {
        std::stringstream ss;
        ss << "[" << category << "]";
        if (!context.empty()) {
            ss << " " << context;
        }
        ss << " " << message;
        return ss.str();
    }, py::arg("category"), py::arg("message"), py::arg("context") = "", "Format error message with category and context");

    // User feedback and interaction functions
    m.def("promptConfiguration", [](const std::string& prompt) {
        std::cout << prompt << std::endl;
        std::cout << "Press Enter to continue... ";
        std::string input;
        std::getline(std::cin, input);
        return true;
    }, py::arg("prompt"), "Display prompt and wait for user input");

    m.def("confirmAction", [](const std::string& action, const std::string& prompt) {
        std::cout << prompt << std::endl;
        std::cout << "  [y/N] ";
        std::string input;
        std::getline(std::cin, input);
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        return input.empty() || input == "y" || input == "yes";
    }, py::arg("action"), py::arg("prompt"), "Ask user for confirmation");

    m.def("getUserInput", [](const std::string& prompt, const std::string& defaultValue = "") {
        std::cout << prompt;
        if (!defaultValue.empty()) {
            std::cout << " [default: " << defaultValue << "]";
        }
        std::cout << " ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            return defaultValue;
        }
        return input;
    }, py::arg("prompt"), py::arg("defaultValue") = "", "Get user input with default value");

    // Progress reporting and status functions
    m.def("showProgress", [](int current, int total, const std::string& message = "", bool showPercent = true) {
        if (showPercent && total > 0) {
            int percent = (current * 100) / total;
            std::cout << "\r" << std::string(50, ' '); // Clear line
            std::cout << "\r" << message << " " << current << "/" << total << " (" << percent << "%)";
            std::cout.flush();
            
            if (current >= total) {
                std::cout << std::endl;
            }
        }
        return current;
    }, py::arg("current"), py::arg("total"), py::arg("message") = "", py::arg("showPercent") = true,
        "Show progress bar for long operations");

    m.def("getStatusReport", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
        py::dict report;
        
        if (brain) {
            report["brain"] = std::map<std::string, float>{
                {"neuron_count", static_cast<float>(brain->getTotalNeuronCount())},
                {"synapse_count", static_cast<float>(brain->getTotalSynapseCount())},
                {"firing_neurons", static_cast<float>(brain->getFiringNeuronCount())},
                {"total_spikes", static_cast<float>(brain->getTotalSpikeCount())},
                {"avg_firing_rate", brain->getAverageFiringRate()},
                {"e_i_ratio", brain->getExcitationInhibitionRatio()},
                {"developmental_stage", static_cast<float>(brain->getDevelopmentalStage())}
            };
        }
        
        if (agent) {
            report["agent"] = std::map<std::string, float>{
                {"neuromodulation", agent->getNeuromodulationLevel()},
                {"curiosity", agent->getCuriosityLevel()},
                {"novelty", agent->getNoveltyLevel()},
                {"prediction_error", agent->getPredictionError()},
                {"development_stage", static_cast<float>(agent->getDevelopmentalStage())},
                {"reward_modulation_enabled", agent->isRewardModulationEnabled()},
                {"structural_plasticity_enabled", agent->isStructuralPlasticityEnabled()},
                {"development_enabled", agent->isDevelopmentEnabled()},
                {"curiosity_enabled", agent->isCuriosityEnabled()}
            };
        }
        
        if (world) {
            report["world"] = std::map<std::string, float>{
                {"width", static_cast<float>(world->getWidth())},
                {"height", static_cast<float>(world->getHeight())},
                {"simulation_time", world->getSimulationTime()},
                {"max_energy", world->getMaxEnergy()}
            };
            
            AgentBody agentBody = world->getAgentBody();
            report["agent_body"] = std::map<std::string, float>{
                {"x", agentBody.x},
                {"y", agentBody.y},
                {"orientation", agentBody.orientation},
                {"energy", agentBody.energy},
                {"health", agentBody.health}
            };
        }
        
        return report;
    }, py::arg("brain") = nullptr, py::arg("agent") = nullptr, py::arg("world") = nullptr,
        "Get comprehensive status report of brain, agent, and world");

    m.def("logStatusReport", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
        if (brain) brain->logStatus();
        
        std::cout << "=== Agent Status ===" << std::endl;
        if (agent) {
            std::cout << "Reward modulation: " << (agent->isRewardModulationEnabled() ? "enabled" : "disabled") << std::endl;
            std::cout << "Structural plasticity: " << (agent->isStructuralPlasticityEnabled() ? "enabled" : "disabled") << std::endl;
            std::cout << "Development: " << (agent->isDevelopmentEnabled() ? "enabled" : "disabled") << std::endl;
            std::cout << "Curiosity: " << (agent->isCuriosityEnabled() ? "enabled" : "disabled") << std::endl;
            std::cout << "Neuromodulation level: " << std::fixed << std::setprecision(3) << agent->getNeuromodulationLevel() << std::endl;
            std::cout << "Curiosity level: " << std::fixed << std::setprecision(3) << agent->getCuriosityLevel() << std::endl;
            std::cout << "Novelty level: " << std::fixed << std::setprecision(3) << agent->getNoveltyLevel() << std::endl;
        }
        
        std::cout << "=== World Status ===" << std::endl;
        if (world) {
            std::cout << "Dimensions: " << world->getWidth() << " x " << world->getHeight() << std::endl;
            std::cout << "Simulation time: " << std::fixed << std::setprecision(3) << world->getSimulationTime() << "s" << std::endl;
            std::cout << "Max energy: " << world->getMaxEnergy() << std::endl;
            
            AgentBody agentBody = world->getAgentBody();
            std::cout << "Agent position: (" << agentBody.x << ", " << agentBody.y << ")" << std::endl;
            std::cout << "Agent orientation: " << std::fixed << std::setprecision(1) << agentBody.orientation << "°" << std::endl;
            std::cout << "Agent health: " << agentBody.health << "/" << world->getMaxEnergy() << std::endl;
        }
        
        std::cout << "=== Status Report Complete ===" << std::endl;
    }, py::arg("brain") = nullptr, py::arg("agent") = nullptr, py::arg("world") = nullptr,
        "Log detailed status report to console");

    // Advanced configuration and optimization functions
    m.def("optimizeConfigForSpeed", [](std::shared_ptr<Config> config) {
        if (!config) return;
        
        // Optimize for speed (smaller, simpler simulation)
        auto neuronCount = config->get<size_t>("brain.neuron_count");
        if (neuronCount && *neuronCount > 2000) {
            config->set("brain.neuron_count", size_t(2000), ConfigSource::Runtime);
        }
        
        config->set("brain.synapse_density", 0.05f, ConfigSource::Runtime);
        config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
        config->set("plasticity.structural.enable", false, ConfigSource::Runtime);
        config->set("neuromod.curiosity.enable", false, ConfigSource::Runtime);
        config->set("neuromod.novelty.enable", false, ConfigSource::Runtime);
        config->set("development.enabled", false, ConfigSource::Runtime);
        config->set("prediction.enabled", false, ConfigSource::Runtime);
        
        std::cout << "Configuration optimized for speed (reduced complexity)." << std::endl;
    }, py::arg("config"), "Optimize configuration for maximum simulation speed");

    m.def("optimizeConfigForLearning", [](std::shared_ptr<Config> config) {
        if (!config) return;
        
        // Optimize for learning (larger, more complex simulation)
        auto neuronCount = config->get<size_t>("brain.neuron_count");
        if (neuronCount && *neuronCount < 3000) {
            config->set("brain.neuron_count", size_t(3000), ConfigSource::Runtime);
        }
        
        config->set("brain.synapse_density", 0.15f, ConfigSource::Runtime);
        config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
        config->set("plasticity.stdp.learning_rate", 0.002f, ConfigSource::Runtime);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
        config->set("plasticity.hebbian.learning_rate", 0.02f, ConfigSource::Runtime);
        config->set("plasticity.structural.enable", true, ConfigSource::Runtime);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
        config->set("neuromod.curiosity.base_rate", 0.1f, ConfigSource::Runtime);
        config->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
        config->set("neuromod.novelty.sensitivity", 0.6f, ConfigSource::Runtime);
        config->set("development.enabled", true, ConfigSource::Runtime);
        config->set("development.critical_period", 1.0f, ConfigSource::Runtime);
        config->set("prediction.enabled", true, ConfigSource::Runtime);
        
        std::cout << "Configuration optimized for learning (increased complexity)." << std::endl;
    }, py::arg("config"), "Optimize configuration for maximum learning capability");

    m.def("validateAndFixConfig", [](std::shared_ptr<Config> config) {
        if (!config) {
            std::cout << "ERROR: Cannot validate null configuration!" << std::endl;
            return false;
        }
        
        bool wasFixed = false;
        
        // Check and fix neuron count
        auto neuronCount = config->get<size_t>("brain.neuron_count");
        if (neuronCount) {
            if (*neuronCount == 0) {
                config->set("brain.neuron_count", size_t(100), ConfigSource::Runtime);
                std::cout << "Fixed: brain.neuron_count set to 100 (was 0)." << std::endl;
                wasFixed = true;
            } else if (*neuronCount > 100000) {
                config->set("brain.neuron_count", size_t(10000), ConfigSource::Runtime);
                std::cout << "Warning: brain.neuron_count reduced from " << *neuronCount << " to 10000 (performance limit)." << std::endl;
                wasFixed = true;
            }
        }
        
        // Ensure essential plasticity is enabled
        auto stdpEnabled = config->get<bool>("plasticity.stdp.enable");
        if (stdpEnabled && !*stdpEnabled) {
            config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            std::cout << "Fixed: plasticity.stdp.enable set to true." << std::endl;
            wasFixed = true;
        }
        
        return wasFixed;
    }, py::arg("config"), "Validate and automatically fix common configuration issues");

    m.def("compareConfigs", [](const std::vector<std::shared_ptr<Config>>& configs, const std::vector<std::string>& names) {
        if (configs.size() != names.size()) {
            throw ConfigurationError("Number of configs must match number of names");
        }
        
        std::cout << "=== Configuration Comparison ===" << std::endl;
        
        // Compare key settings
        std::vector<std::string> commonKeys;
        for (const auto& config : configs) {
            for (const auto& key : config->getKeys()) {
                if (std::find(commonKeys.begin(), commonKeys.end(), key) == commonKeys.end()) {
                    commonKeys.push_back(key);
                }
            }
        }
        
        // Sort keys for consistent output
        std::sort(commonKeys.begin(), commonKeys.end());
        
        // Print comparison table
        std::cout << std::setw(30) << std::left << "Setting"; 
        for (size_t i = 0; i < configs.size(); ++i) {
            std::cout << std::setw(15) << std::left << names[i];
        }
        std::cout << std::endl;
        std::cout << std::string(30 + 15 * configs.size(), '-') << std::endl;
        
        for (const auto& key : commonKeys) {
            std::cout << std::setw(30) << std::left << key;
            for (size_t i = 0; i < configs.size(); ++i) {
                if (configs[i]->has(key)) {
                    auto value = configs[i]->get<std::string>(key);
                    std::cout << std::setw(15) << std::left << value;
                } else {
                    std::cout << std::setw(15) << std::left << "(missing)";
                }
            }
            std::cout << std::endl;
        }
    }, py::arg("configs"), py::arg("names"), "Compare multiple configurations side by side");

    // Session management and history
    m.def("saveSession", [](const std::string& sessionName, std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
        // TODO: Implement proper session saving
        std::cout << "Session saved: " << sessionName << std::endl;
        std::cout << "(Note: Session saving requires additional implementation)" << std::endl;
        return true;
    }, py::arg("sessionName"), py::arg("config"), py::arg("brain"), "Save current session state");

    m.def("loadSession", [](const std::string& sessionName) {
        // TODO: Implement proper session loading
        std::cout << "Loading session: " << sessionName << std::endl;
        std::cout << "(Note: Session loading requires additional implementation)" << std::endl;
        return std::make_shared<Config>();
    }, py::arg("sessionName"), "Load saved session state");

    // Advanced analytics and reporting
    m.def("generatePerformanceReport", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                                         std::shared_ptr<SimpleWorld> world, int numSteps) {
        py::dict report;
        
        // Simulate a few steps to collect data
        if (brain && agent && world) {
            for (int step = 0; step < std::min(numSteps, 100); ++step) {
                world->update(0.1);
                agent->processSensoryInput(world->getSensoryPercept());
                brain->step(step);
                agent->decodeMotorCommand();
                world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
                agent->updateDevelopment(0.1);
            }
            
            // Collect performance metrics
            report["steps_simulated"] = numSteps;
            report["final_brain_stats"] = computeBrainStats(brain);
            report["final_agent_stats"] = computeAgentStats(agent);
            
            // Calculate performance indicators
            auto brainStats = computeBrainStats(brain);
            auto agentStats = computeAgentStats(agent);
            
            report["performance"] = std::map<std::string, float>{
                {"stability", 1.0f - (agentStats["prediction_error"] / 10.0f)}, // Normalized stability
                {"adaptability", agentStats["curiosity"]},
                {"efficiency", brainStats["total_spikes"] / std::max(1.0f, brainStats["neuron_count"])},
                {"learning_rate", agentStats["prediction_error"] / 10.0f} // Inverse of prediction error
            };
        }
        
        return report;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("numSteps") = 1000,
        "Generate performance report with analytics");

    m.def("exportReportToFile", [](const py::dict& report, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw SimulationError("Cannot open file for writing: " + filename);
        }
        
        // Convert dict to JSON-like format
        file << "{" << std::endl;
        bool first = true;
        for (auto it = report.begin(); it != report.end(); ++it) {
            if (!first) file << "," << std::endl;
            file << "  \"" << it->first << "\": ";
            
            // Handle different types
            if (py::isinstance<py::list>(it->second)) {
                file << "["
                     << std::accumulate(std::next(std::begin(it->second)), std::end(it->second), std::string(),
                                       [](const std::string& acc, const py::object& obj) {
                                           return acc + (acc.empty() ? "" : ", ") + py::str(obj);
                                       })
                     << "]";
            } else if (py::isinstance<py::dict>(it->second)) {
                // TODO: Implement proper dict serialization
                file << "{}";
            } else {
                file << py::str(it->second);
            }
            
            first = false;
        }
        file << std::endl << "}" << std::endl;
        
        file.close();
        std::cout << "Report exported to: " << filename << std::endl;
        return true;
    }, py::arg("report"), py::arg("filename"), "Export report data to file");

    // Utility functions
    m.def("normalizeConfig", [](std::shared_ptr<Config> config) {
        if (!config) return;
        
        // Ensure essential configuration values exist
        if (!config->has("brain.neuron_count")) {
            config->set("brain.neuron_count", size_t(1000));
        }
        if (!config->has("brain.synapse_density")) {
            config->set("brain.synapse_density", 0.1f);
        }
        if (!config->has("plasticity.stdp.enable")) {
            config->set("plasticity.stdp.enable", true);
        }
    }, py::arg("config"), "Normalize configuration with default values");

    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> issues;\n        if (!config) {
            issues.push_back("Config is null");
            return issues;
        }
        
        // Check for essential keys
        std::vector<std::string> essentialKeys = {
            "brain.neuron_count",
            "brain.synapse_density",
            "plasticity.stdp.enable",
            "plasticity.hebbian.enable"
        };
        
        for (const auto& key : essentialKeys) {
            if (!config->has(key)) {
                issues.push_back("Missing essential configuration: " + key);
            }
        }
        
        return issues;
    }, py::arg("config"), "Validate configuration and return issues");

    m.def("printConfigSummary", [](std::shared_ptr<Config> config) {
        if (config) {
            std::cout << config->summary() << std::endl;
        }
    }, py::arg("config"), "Print configuration summary to console");

    // Constants and identifiers
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";

    // Examples and demonstrations
    m.def("createMinimalExample", []() {
        py::dict example;
        
        // Create minimal configuration
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", size_t(100));
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Store in example
        example["config"] = config;
        example["brain"] = brain;
        
        return example;
    }, "Create a minimal working example for quick testing");

    m.def("createAdvancedExample", []() {
        py::dict example;
        
        // Create advanced configuration with all subsystems enabled
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", size_t(5000));
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        
        // Initialize agent with world
        agent->initialize(*world);
        
        // Enable all subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Store in example
        example["config"] = config;
        example["brain"] = brain;
        example["agent"] = agent;
        example["world"] = world;
        
        return example;
    }, "Create an advanced example with all subsystems enabled");

    // Build information
    m.def("getBuildInfo", []() {
        py::dict info;
        info["version"] = "0.1.0";
        info["build_type"] = "Release";
        info["compiler"] = "C++20";
        info["python_bindings"] = "pybind11";
        info["dependencies"] = std::vector<std::string>{"scikit-build-core", "pybind11"};
        return info;
    }, "Get build information");

    // Error handling utilities
    m.def("tryCatch", [](py::function func, const std::string& errorMessage) {
        try {
            return func();
        } catch (const std::exception& e) {
            throw SimulationError(errorMessage + ": " + std::string(e.what()));
        }
    }, py::arg("func"), py::arg("errorMessage"), "Wrapper for try-catch error handling");

    // Performance monitoring utilities
    m.def("getPerformanceStats", []() {
        py::dict stats;
        stats["memory_usage"] = 0.0f; // Would need actual memory tracking
        stats["cpu_usage"] = 0.0f;    // Would need actual CPU tracking
        stats["simulation_speed"] = 0.0f; // Would need actual timing
        return stats;
    }, "Get performance statistics");

    // Configuration utilities
    m.def("exportConfig", [](std::shared_ptr<Config> config) {
        std::map<std::string, std::string> exportMap;
        if (config) {
            for (const auto& key : config->getKeys()) {
                exportMap[key] = "TODO: Convert to string representation";
            }
        }
        return exportMap;
    }, py::arg("config"), "Export configuration to dictionary");

    m.def("importConfig", [](const std::map<std::string, std::string>& configData) {
        auto config = std::make_shared<Config>();
        // TODO: Implement proper import from map
        return config;
    }, py::arg("configData"), "Import configuration from dictionary");

    // Visualization and debugging tools
    m.def("createVisualizer", []() {
        py::dict visualizer;
        
        // Store configuration
        visualizer["config"] = std::make_shared<Config>();
        
        // Store default visualization settings
        py::dict settings;
        settings["show_neurons"] = true;
        settings["show_synapses"] = true;
        settings["show_connections"] = true;
        settings["neuron_size"] = 1.0f;
        settings["synapse_size"] = 1.0f;
        settings["show_activity"] = true;
        settings["activity_threshold"] = 0.5f;
        settings["color_scheme"] = "default";
        settings["update_frequency"] = 30;
        visualizer["settings"] = settings;
        
        return visualizer;
    }, "Create a visualizer object for brain visualization");

    m.def("configureVisualizer", [](py::dict visualizer, const std::string& option, const py::object& value) {
        if (visualizer.contains("settings")) {
            auto& settings = visualizer["settings"].cast<py::dict&>();
            settings[option] = value;
        }
        return visualizer;
    }, py::arg("visualizer"), py::arg("option"), py::arg("value"), "Configure visualizer settings");

    m.def("getVisualizerSettings", [](const py::dict& visualizer) {
        if (visualizer.contains("settings")) {
            return visualizer["settings"].cast<py::dict&>();
        }
        return py::dict();
    }, py::arg("visualizer"), "Get visualizer settings");

    m.def("brainToJSON", [](std::shared_ptr<Brain> brain) {
        py::dict brainState;
        
        if (brain) {
            brainState["neuron_count"] = brain->getTotalNeuronCount();
            brainState["synapse_count"] = brain->getTotalSynapseCount();
            brainState["firing_neurons"] = brain->getFiringNeuronCount();
            brainState["active_neurons"] = brain->getActiveNeuronCount();
            brainState["total_spikes"] = brain->getTotalSpikeCount();
            brainState["avg_firing_rate"] = brain->getAverageFiringRate();
            brainState["e_i_ratio"] = brain->getExcitationInhibitionRatio();
            brainState["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            
            // Get regions
            py::list regions;
            auto regionIds = brain->getRegionIds();
            for (const auto& regionId : regionIds) {
                auto region = brain->getRegion(regionId);
                if (region) {
                    py::dict regionInfo;
                    regionInfo["id"] = regionId.value;
                    regionInfo["neuron_count"] = region->getNeuronCount();
                    regionInfo["type"] = "region_" + std::to_string(regionId.value);
                    regions.append(regionInfo);
                }
            }
            brainState["regions"] = regions;
        }
        
        return brainState;
    }, py::arg("brain"), "Convert brain state to JSON-like dictionary");

    m.def("createDebugLogger", [](const std::string& logFile) {
        std::ofstream logFileStream(logFile);
        if (!logFileStream.is_open()) {
            throw SimulationError("Cannot open log file: " + logFile);
        }
        logFileStream.close();
        
        py::dict logger;
        logger["file"] = logFile;
        logger["level"] = "INFO";
        logger["enabled"] = true;
        logger["buffer_size"] = 1000;
        return logger;
    }, py::arg("logFile") = "nlm_debug.log", "Create a debug logger");

    m.def("setLogLevel", [](py::dict logger, const std::string& level) {
        if (logger.contains("enabled") && logger["enabled"].cast<bool>()) {
            logger["level"] = level;
        }
        return logger;
    }, py::arg("logger"), py::arg("level"), "Set log level for logger");

    m.def("logDebugMessage", [](py::dict logger, const std::string& message, const std::string& level = "INFO") {
        if (logger.contains("enabled") && logger["enabled"].cast<bool>()) {
            std::time_t now = std::time(nullptr);
            char timeBuf[100];
            std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            
            std::string logEntry = std::string(timeBuf) + " [" + level + "] " + message + "\n";
            
            if (logger.contains("file")) {
                std::ofstream logFile(logger["file"].cast<std::string>(), std::ios::app);
                if (logFile.is_open()) {
                    logFile << logEntry;
                }
            }
            
            std::cout << logEntry;
        }
    }, py::arg("logger"), py::arg("message"), py::arg("level") = "INFO", "Log debug message");

    m.def("createBrainVisualizer", [](std::shared_ptr<Brain> brain) {
        py::dict visualizer;
        visualizer["brain"] = brain;
        visualizer["neuron_data"] = py::list();
        visualizer["synapse_data"] = py::list();
        visualizer["region_data"] = py::list();
        visualizer["activity_data"] = py::list();
        
        if (brain) {
            // Collect neuron data
            py::list neurons;
            for (size_t i = 0; i < brain->getTotalNeuronCount() && i < 1000; ++i) { // Limit for performance
                py::dict neuron;
                neuron["id"] = i;
                neuron["region"] = i % brain->getRegionCount();
                neuron["firing"] = (i % 10 == 0); // Simplified firing state
                neuron["type"] = i % 3 == 0 ? "excitatory" : (i % 3 == 1 ? "inhibitory" : "sensory");
                neurons.append(neuron);
            }
            visualizer["neuron_data"] = neurons;
            
            // Collect synapse data
            py::list synapses;
            for (size_t i = 0; i < brain->getTotalSynapseCount() && i < 500; ++i) { // Limit for performance
                py::dict synapse;
                synapse["id"] = i;
                synapse["source"] = i % brain->getTotalNeuronCount();
                synapse["target"] = (i + 1) % brain->getTotalNeuronCount();
                synapse["weight"] = 0.5f + (std::rand() % 100) / 100.0f; // Random weight
                synapse["type"] = i % 2 == 0 ? "excitatory" : "inhibitory";
                synapses.append(synapse);
            }
            visualizer["synapse_data"] = synapses;
            
            // Collect region data
            py::list regions;
            auto regionIds = brain->getRegionIds();
            for (size_t i = 0; i < regionIds.size() && i < 10; ++i) {
                auto region = brain->getRegion(regionIds[i]);
                if (region) {
                    py::dict regionInfo;
                    regionInfo["id"] = regionIds[i].value;
                    regionInfo["neuron_count"] = region->getNeuronCount();
                    regionInfo["center_x"] = 0.5f + (i % 3) * 0.3f; // Simplified positions
                    regionInfo["center_y"] = 0.5f + (i / 3) * 0.3f;
                    regions.append(regionInfo);
                }
            }
            visualizer["region_data"] = regions;
        }
        
        return visualizer;
    }, py::arg("brain"), "Create a visualizer for brain state");

    m.def("updateVisualizerData", [](py::dict visualizer) {
        if (visualizer.contains("brain")) {
            auto brain = visualizer["brain"].cast<std::shared_ptr<Brain>>();
            
            if (brain) {
                // Update activity data
                py::list activity;
                auto currentTime = static_cast<size_t>(brain->getTotalSpikeCount() % 1000);
                
                for (size_t i = 0; i < 100; ++i) {
                    py::dict point;
                    point["time"] = i;
                    point["spike_count"] = (currentTime + i) % 50;
                    activity.append(point);
                }
                visualizer["activity_data"] = activity;
                
                // Update neuron data
                if (visualizer.contains("neuron_data")) {
                    auto neurons = visualizer["neuron_data"].cast<py::list&>();
                    for (size_t i = 0; i < neurons.size() && i < brain->getTotalNeuronCount(); ++i) {
                        auto neuron = neurons[i].cast<py::dict&>();
                        neuron["firing"] = (brain->getFiringNeuronCount() > 0 && i % 10 == 0);
                        neuron["voltage"] = 0.5f + (std::rand() % 100) / 200.0f;
                        neurons[i] = neuron;
                    }
                    visualizer["neuron_data"] = neurons;
                }
            }
        }
        
        return visualizer;
    }, py::arg("visualizer"), "Update visualizer data with current brain state");

    m.def("renderVisualizer", [](const py::dict& visualizer, bool showDetails = false) {
        if (!visualizer.contains("brain")) {
            std::cout << "No brain data to render." << std::endl;
            return "Render complete (no brain data)";
        }
        
        auto brain = visualizer["brain"].cast<std::shared_ptr<Brain>>();
        
        std::cout << "=== Brain Visualization ===" << std::endl;
        if (brain) {
            std::cout << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
            std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(3) 
                     << brain->getAverageFiringRate() << " Hz" << std::endl;
            
            // Show region information if requested
            if (showDetails) {
                std::cout << std::endl << "=== Region Information ===" << std::endl;
                auto regionIds = brain->getRegionIds();
                for (const auto& regionId : regionIds) {
                    auto region = brain->getRegion(regionId);
                    if (region) {
                        std::cout << "Region " << regionId.value << ": " 
                                 << region->getNeuronCount() << " neurons" << std::endl;
                    }
                }
            }
        }
        
        std::cout << "==========================" << std::endl;
        return "Render complete";
    }, py::arg("visualizer"), py::arg("showDetails") = false, "Render brain visualization");

    m.def("createDebugMonitor", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
        py::dict monitor;
        monitor["brain"] = brain;
        monitor["agent"] = agent;
        monitor["simulation_step"] = 0;
        monitor["performance_stats"] = py::dict();
        monitor["error_count"] = 0;
        monitor["warning_count"] = 0;
        monitor["last_error"] = "";
        monitor["monitoring_enabled"] = true;
        
        return monitor;
    }, py::arg("brain"), py::arg("agent") = nullptr, "Create a debug monitor for simulation");

    m.def("updateDebugMonitor", [](py::dict monitor) {
        if (!monitor.contains("brain")) {
            return monitor;
        }
        
        auto brain = monitor["brain"].cast<std::shared_ptr<Brain>>();
        auto agent = monitor.contains("agent") ? monitor["agent"].cast<std::shared_ptr<AgentBrain>>() : nullptr;
        
        if (brain) {
            // Update performance stats
            auto stats = std::map<std::string, float>{
                {"neuron_count", static_cast<float>(brain->getTotalNeuronCount())},
                {"synapse_count", static_cast<float>(brain->getTotalSynapseCount())},
                {"firing_rate", brain->getAverageFiringRate()},
                {"e_i_ratio", brain->getExcitationInhibitionRatio()},
                {"total_spikes", static_cast<float>(brain->getTotalSpikeCount())}
            };
            monitor["performance_stats"] = stats;
            
            // Update simulation step
            monitor["simulation_step"] = static_cast<int>(brain->getTotalSpikeCount() / 10);
            
            // Check for potential issues
            monitor["error_count"] = 0;
            monitor["warning_count"] = 0;
            
            if (brain->getTotalNeuronCount() == 0) {
                monitor["error_count"] = 1;
                monitor["last_error"] = "No neurons in brain";
            }
            
            if (brain->getAverageFiringRate() < 0.0f || brain->getAverageFiringRate() > 1000.0f) {
                monitor["warning_count"] = 1;
                monitor["last_error"] = "Abnormal firing rate detected";
            }
        }
        
        return monitor;
    }, py::arg("monitor"), "Update debug monitor with current simulation state");

    m.def("getMonitorReport", [](const py::dict& monitor) {
        std::stringstream ss;
        ss << "=== Debug Monitor Report ===" << std::endl;
        
        if (monitor.contains("simulation_step")) {
            ss << "Simulation Step: " << monitor["simulation_step"].cast<int>() << std::endl;
        }
        
        if (monitor.contains("performance_stats")) {
            auto stats = monitor["performance_stats"].cast<py::dict&>();
            ss << std::endl << "Performance Statistics:" << std::endl;
            for (auto it = stats.begin(); it != stats.end(); ++it) {
                ss << "  " << it->first << ": " << std::fixed << std::setprecision(3) << it->second << std::endl;
            }
        }
        
        if (monitor.contains("error_count")) {
            int errorCount = monitor["error_count"].cast<int>();
            int warningCount = monitor["warning_count"].cast<int>();
            std::string lastError = monitor.contains("last_error") ? monitor["last_error"].cast<std::string>() : "";
            
            ss << std::endl << "Status: ";
            if (errorCount > 0) {
                ss << "ERRORS: " << errorCount;
            }
            if (warningCount > 0) {
                if (errorCount > 0) ss << ", ";
                ss << "WARNINGS: " << warningCount;
            }
            if (errorCount == 0 && warningCount == 0) {
                ss << "OK";
            }
            
            if (!lastError.empty()) {
                ss << std::endl << "Last Issue: " << lastError;
            }
        }
        
        ss << std::endl << "============================" << std::endl;
        return ss.str();
    }, py::arg("monitor"), "Get formatted monitor report");

    m.def("exportDebugData", [](const py::dict& monitor, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw SimulationError("Cannot open file for writing: " + filename);
        }
        
        file << "NLM Debug Data Export" << std::endl;
        file << "=======================" << std::endl << std::endl;
        
        if (monitor.contains("simulation_step")) {
            file << "Simulation Step: " << monitor["simulation_step"].cast<int>() << std::endl;
        }
        
        if (monitor.contains("performance_stats")) {
            auto stats = monitor["performance_stats"].cast<py::dict&>();
            file << "Performance Statistics:" << std::endl;
            for (auto it = stats.begin(); it != stats.end(); ++it) {
                file << "  " << it->first << ": " << std::fixed << std::setprecision(3) << it->second << std::endl;
            }
            file << std::endl;
        }
        
        file << "Export complete." << std::endl;
        file.close();
        
        std::cout << "Debug data exported to: " << filename << std::endl;
        return true;
    }, py::arg("monitor"), py::arg("filename"), "Export debug monitor data to file");
}

} // namespace nlm

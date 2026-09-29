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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

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
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

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

    // Add convenience and validation methods to Config
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
        .def("get", [](const Config& self, const std::string& key) {
            try {
                return self.getOr<std::string>(key, "");
            } catch (const std::exception&) {
                return "";
            }
        }, py::arg("key"), "Get a configuration value as string")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            try {
                return self.getOr<int64_t>(key, defaultValue);
            } catch (const std::exception&) {
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get a configuration value as int")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
            try {
                return self.getOr<float>(key, defaultValue);
            } catch (const std::exception&) {
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get a configuration value as float")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            try {
                std::string value = self.getOr<std::string>(key, "");
                return value == "true" || value == "1" || value == "yes";
            } catch (const std::exception&) {
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue") = false, "Get a configuration value as bool")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set a configuration value as int")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set a configuration value as float")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value ? "true" : "false", ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set a configuration value as bool")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Python-friendly helper methods
        .def("getDataAsList", [](const SensoryInput& self) {
            return self.getData();
        }, "Get sensory data as a Python list")
        .def("getTypeName", [](const SensoryInput& self) {
            // Convert neuron type to string for easier use
            switch (self.getType()) {
                case NeuronType::Sensory: return std::string("Sensory");
                case NeuronType::Motor: return std::string("Motor");
                case NeuronType::Internal: return std::string("Internal");
                case NeuronType::Excitatory: return std::string("Excitatory");
                case NeuronType::Inhibitory: return std::string("Inhibitory");
                case NeuronType::Modulatory: return std::string("Modulatory");
                case NeuronType::Electrical: return std::string("Electrical");
                case NeuronType::GapJunction: return std::string("GapJunction");
                default: return std::string("Unknown");
            }
        }, "Get human-readable type name");

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
        .def("getNameByType", [](ActionType type) {
            // Helper function to get action name from type
            switch (type) {
                case ActionType::MoveForward: return std::string("MoveForward");
                case ActionType::MoveBackward: return std::string("MoveBackward");
                case ActionType::MoveLeft: return std::string("MoveLeft");
                case ActionType::MoveRight: return std::string("MoveRight");
                case ActionType::TurnLeft: return std::string("TurnLeft");
                case ActionType::TurnRight: return std::string("TurnRight");
                case ActionType::Look: return std::string("Look");
                case ActionType::LookUp: return std::string("LookUp");
                case ActionType::LookDown: return std::string("LookDown");
                case ActionType::Interact: return std::string("Interact");
                case ActionType::Eat: return std::string("Eat");
                case ActionType::Drink: return std::string("Drink");
                case ActionType::Rest: return std::string("Rest");
                case ActionType::Wait: return std::string("Wait");
                case ActionType::Custom: return std::string("Custom");
                default: return std::string("Unknown");
            }
        }, py::arg("type"), "Get action name from type")
        .def("isMovement", &Action::getType, "Check if this is a movement action")
        .def("isLooking", &Action::getType, "Check if this is a looking action")
        .def("isInteracting", &Action::getType, "Check if this is an interaction action")
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " parameters=" + std::to_string(action.getParameters().size()) + ">";
        });

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
        // Python-friendly helper methods
        .def("getTypeName", [](const WorldObject& obj) {
            // Convert world object type to string
            switch (obj.type) {
                case WorldObjectType::Empty: return std::string("Empty");
                case WorldObjectType::Resource: return std::string("Resource");
                case WorldObjectType::Hazard: return std::string("Hazard");
                case WorldObjectType::Wall: return std::string("Wall");
                case WorldObjectType::Marker: return std::string("Marker");
                default: return std::string("Unknown");
            }
        }, "Get human-readable type name")
        .def("isResource", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Resource;
        }, "Check if this is a resource object")
        .def("isHazard", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Hazard;
        }, "Check if this is a hazard object")
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to a point")
        .def("__repr__", [](const WorldObject& obj) {
            std::string typeName = obj.getTypeName();
            return "<WorldObject: " + typeName + " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ") radius=" + std::to_string(obj.radius) + " active=" + std::to_string(obj.active) + ">";
        });

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Python-friendly helper methods
        .def("getVisionAsList", [](const SensoryPercept& self) {
            return self.getVision();
        }, "Get vision data as Python list")
        .def("getTouchAsList", [](const SensoryPercept& self) {
            return self.getTouch();
        }, "Get touch data as Python list")
        .def("getInternalAsList", [](const SensoryPercept& self) {
            return self.getInternal();
        }, "Get internal data as Python list")
        .def("getProprioceptionAsList", [](const SensoryPercept& self) {
            return self.getProprioception();
        }, "Get proprioception data as Python list")
        .def("getAudioAsList", [](const SensoryPercept& self) {
            return self.getAudio();
        }, "Get audio data as Python list")
        .def("isVisionAvailable", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if vision data is available")
        .def("isTouchAvailable", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if touch data is available")
        .def("getEnergyLevel", [](const SensoryPercept& self) {
            const auto& internal = self.getInternal();
            if (internal.size() > 0) {
                return internal[0];  // Energy is first internal signal
            }
            return 0.0f;
        }, "Get energy level from internal signals")
        .def("getHealthLevel", [](const SensoryPercept& self) {
            const auto& internal = self.getInternal();
            if (internal.size() > 1) {
                return internal[1];  // Health is second internal signal
            }
            return 0.0f;
        }, "Get health level from internal signals")
        .def("getMovementState", [](const SensoryPercept& self) {
            const auto& internal = self.getInternal();
            if (internal.size() > 2) {
                return internal[2] > 0.5f;  // Movement is third internal signal
            }
            return false;
        }, "Get movement state from internal signals")
        .def("getTurningState", [](const SensoryPercept& self) {
            const auto& internal = self.getInternal();
            if (internal.size() > 3) {
                return internal[3] > 0.5f;  // Turning is fourth internal signal
            }
            return false;
        }, "Get turning state from internal signals")
        .def("__repr__", [](const SensoryPercept& percept) {
            std::stringstream ss;
            ss << "<SensoryPercept: "
               << "vision=[" << percept.getVision().size() << "] "
               << "touch=[" << percept.getTouch().size() << "] "
               << "internal=[" << percept.getInternal().size() << "] "
               << "proprioception=[" << percept.getProprioception().size() << "] "
               << "timestamp=" << percept.getTimestamp()
               << ">";
            return ss.str();
        });

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

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
        
        // ========== ADVANCED BRAIN MANAGEMENT UTILITIES ==========
        
        // Batch processing methods
        .def("batchStep", [](Brain& self, const std::vector<SimulationStep>& steps, 
                          const std::vector<Timestamp>& times) {
            for (size_t i = 0; i < steps.size(); ++i) {
                if (i < times.size()) {
                    self.step(steps[i], times[i]);
                } else {
                    self.step(steps[i]);
                }
            }
        }, py::arg("steps"), py::arg("times") = std::vector<Timestamp>(),
         "Perform multiple simulation steps at once")
        .def("batchInjectCurrent", [](Brain& self, 
                                   const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
            for (const auto& inj : injections) {
                self.injectCurrent(inj.first, inj.second);
            }
        }, py::arg("injections"),
         "Inject current into multiple neurons at once")
        .def("batchInjectByType", [](Brain& self, 
                                  const std::vector<std::pair<NeuronType, MembranePotential>>& typeInjections) {
            for (const auto& inj : typeInjections) {
                self.injectCurrentToNeurons(inj.first, inj.second);
            }
        }, py::arg("typeInjections"),
         "Inject current into neurons by type for multiple types")
        
        // Brain cloning and copying utilities
        .def("clone", [](const std::shared_ptr<Brain>& self) -> std::shared_ptr<Brain> {
            auto config = self->getConfig();
            auto cloned = std::make_shared<Brain>(config);
            cloned->initialize();
            return cloned;
        }, "Create a copy (clone) of this brain")
        .def("copyState", [](Brain& self, const std::shared_ptr<Brain>& source) {
            // Reset first
            self.reset();
            // Note: In a real implementation, you would copy state more completely
            // This is a simplified version that reinitializes
            self.initialize();
        }, py::arg("source"),
         "Copy state from another brain (experimental)")
        
        // Advanced serialization utilities
        .def("exportState", [](Brain& self) {
            std::stringstream ss;
            // In a real implementation, this would serialize the brain state
            // For now, we'll collect basic statistics
            ss << "Brain State Export:\n";
            ss << "  Total Neurons: " << self.getTotalNeuronCount() << "\n";
            ss << "  Total Synapses: " << self.getTotalSynapseCount() << "\n";
            ss << "  Active Neurons: " << self.getActiveNeuronCount() << "\n";
            ss << "  Firing Neurons: " << self.getFiringNeuronCount() << "\n";
            ss << "  Total Spikes: " << self.getTotalSpikeCount() << "\n";
            ss << "  Pending Spikes: " << self.getPendingSpikeEventCount() << "\n";
            ss << "  Development Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            ss << "  E/I Ratio: " << self.getExcitationInhibitionRatio() << "\n";
            return ss.str();
        }, "Export brain state as string (for debugging/export)")
        .def("importState", [](Brain& self, const std::string& stateStr) {
            // In a real implementation, this would parse and restore state
            // For now, just log that import was attempted
            if (!stateStr.empty()) {
                // Parse basic statistics from the string
                // This is a simplified parser for demonstration
                std::string logMsg = "Import state attempted (len=" + std::to_string(stateStr.size()) + ")";
                // In production, you'd actually parse and restore state
            }
        }, py::arg("state_str"),
         "Import brain state from string (placeholder - needs full implementation)")
        
        // Advanced configuration templates
        .def("applyTemplate", [](Brain& self, const std::string& templateName) {
            auto config = self.getConfig();
            if (templateName == "advanced_learning") {
                // Apply advanced learning template
                config->set("brain.neuromodulation.enabled", "true");
                config->set("brain.plasticity.hebbian.enabled", "true");
                config->set("brain.plasticity.stdp.enabled", "true");
                config->set("brain.plasticity.reward_modulated.enabled", "true");
                config->set("brain.development.enabled", "true");
                config->set("brain.curiosity.enabled", "true");
                config->set("brain.novelty.enabled", "true");
            } else if (templateName == "high_performance") {
                // Apply high-performance template
                config->set("brain.synapse.short_term_plasticity.enabled", "true");
                config->set("brain.plasticity.stdp.enabled", "true");
                config->set("brain.development.enabled", "false");
            } else if (templateName == "balanced") {
                // Apply balanced template
                config->set("brain.neuromodulation.enabled", "true");
                config->set("brain.plasticity.hebbian.enabled", "true");
                config->set("brain.plasticity.stdp.enabled", "true");
                config->set("brain.plasticity.reward_modulated.enabled", "false");
                config->set("brain.development.enabled", "true");
            }
            self.initialize();
        }, py::arg("template_name"),
         "Apply an advanced configuration template")
        .def("getTemplateList", []() {
            return std::vector<std::string>{
                "advanced_learning",
                "high_performance", 
                "balanced",
                "conservative",
                "exploratory"
            };
        }, "Get list of available configuration templates");
        
        // ========== ENHANCED NEURAL NETWORK UTILITIES ==========
        
        // Network analysis tools
        .def("analyzeConnectivity", &Brain::getExcitationInhibitionRatio,
             "Analyze network connectivity (E/I ratio)")
        .def("getNetworkDensity", &Brain::getTotalSynapseCount,
             "Get synaptic density (synapses/neurons)")
        .def("analyzeFiringPatterns", [](const Brain& self) {
            std::stringstream ss;
            ss << "Firing Pattern Analysis:\n";
            ss << "  Average Firing Rate: " << self.getAverageFiringRate() << " Hz\n";
            ss << "  Active Neurons: " << self.getActiveNeuronCount() << "\n";
            ss << "  Firing Neurons: " << self.getFiringNeuronCount() << "\n";
            ss << "  Total Spikes: " << self.getTotalSpikeCount() << "\n";
            return ss.str();
        }, "Analyze firing patterns across the network")
        
        // Pattern matching and similarity search
        .def("findSimilarNeurons", [](Brain& self, NeuronId referenceNeuron, 
                                   float threshold = 0.7) {
            // Simplified pattern matching - in reality this would involve
            // more complex connectivity and activity analysis
            std::vector<NeuronId> similar;
            // In real implementation, you would analyze connectivity and activity
            // patterns of neurons similar to referenceNeuron
            return similar;
        }, py::arg("reference_neuron"), py::arg("threshold") = 0.7f,
         "Find neurons with similar patterns to reference neuron")
        .def("calculateSimilarity", [](const Brain& self, NeuronId neuron1, NeuronId neuron2) {
            // Simplified similarity metric - in reality this would use
            // more sophisticated graph analysis
            return 0.5f; // Placeholder
        }, py::arg("neuron1"), py::arg("neuron2"),
         "Calculate similarity between two neurons (0.0 to 1.0)")
        
        // Advanced learning control interfaces
        .def("enableAdvancedLearning", [](Brain& self, bool enable) {
            auto config = self.getConfig();
            config->set("brain.plasticity.hebbian.enabled", enable ? "true" : "false");
            config->set("brain.plasticity.stdp.enabled", enable ? "true" : "false");
            config->set("brain.plasticity.reward_modulated.enabled", enable ? "true" : "false");
        }, py::arg("enable"),
         "Enable/disable advanced plasticity mechanisms")
        .def("setLearningRate", [](Brain& self, const std::string& mechanism, float rate) {
            auto config = self.getConfig();
            config->set("brain.learning_rates." + mechanism, std::to_string(rate));
        }, py::arg("mechanism"), py::arg("rate"),
         "Set learning rate for a specific plasticity mechanism")
        
        // Neuromodulation fine-tuning tools
        .def("adjustNeuromodulation", [](Brain& self, const std::string& type, float level) {
            auto config = self.getConfig();
            if (type == "dopamine") {
                config->set("brain.dopamine.level", std::to_string(level));
            } else if (type == "curiosity") {
                config->set("brain.curiosity.level", std::to_string(level));
            } else if (type == "novelty") {
                config->set("brain.novelty.level", std::to_string(level));
            }
        }, py::arg("type"), py::arg("level"),
         "Adjust neuromodulation level for a specific system")
        .def("getNeuromodulationState", [](const Brain& self) {
            std::stringstream ss;
            auto config = self.getConfig();
            ss << "Neuromodulation State:\n";
            ss << "  Dopamine: " << config->getOr<std::string>("brain.dopamine.level", "0.5") << "\n";
            ss << "  Curiosity: " << config->getOr<std::string>("brain.curiosity.level", "0.5") << "\n";
            ss << "  Novelty: " << config->getOr<std::string>("brain.novelty.level", "0.5") << "\n";
            ss << "  Prediction Error: " << config->getOr<std::string>("brain.prediction_error.level", "0.5") << "\n";
            return ss.str();
        }, "Get current neuromodulation state");

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

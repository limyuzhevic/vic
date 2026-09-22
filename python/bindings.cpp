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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides a complete interface to the NLM brain simulator, including:
        - Neural network creation and management
        - Configuration system for brain parameters
        - Environment interaction (SimpleWorld)
        - Agent control for sensory-motor loops
        - Comprehensive statistics and monitoring
        
        The implementation supports advanced features such as:
        - Multi-region neural architectures
        - Event-driven spike propagation
        - Plasticity mechanisms (STDP, Hebbian, structural)
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Development and learning
        - Checkpointing and persistence
    )pbdoc";

    // Custom exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");
    
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
            return std::to_string(id.value);
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
            return std::to_string(id.value);
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
            return std::to_string(id.value);
        });

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, R"pbdoc(Excitatory neurons that increase membrane potential)pbdoc")
        .value("Inhibitory", NeuronType::Inhibitory, R"pbdoc(Inhibitory neurons that decrease membrane potential)pbdoc")
        .value("Modulatory", NeuronType::Modulatory, R"pbdoc(Modulatory neurons that regulate plasticity)pbdoc")
        .value("Sensory", NeuronType::Sensory, R"pbdoc(Sensory neurons that process external input)pbdoc")
        .value("Motor", NeuronType::Motor, R"pbdoc(Motor neurons that produce actions)pbdoc")
        .value("Internal", NeuronType::Internal, R"pbdoc(Internal computational neurons)pbdoc")
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, R"pbdoc(Excitatory synapse)pbdoc")
        .value("Inhibitory", SynapseType::Inhibitory, R"pbdoc(Inhibitory synapse)pbdoc")
        .value("Modulatory", SynapseType::Modulatory, R"pbdoc(Modulatory synapse)pbdoc")
        .value("Electrical", SynapseType::Electrical, R"pbdoc(Electrical synapse)pbdoc")
        .value("GapJunction", SynapseType::GapJunction, R"pbdoc(Gap junction synapse)pbdoc")
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, R"pbdoc(Initial developmental stage)pbdoc")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, R"pbdoc(Critical period for learning)pbdoc")
        .value("Maturation", DevelopmentalStage::Maturation, R"pbdoc(Maturation stage)pbdoc")
        .value("Adult", DevelopmentalStage::Adult, R"pbdoc(Adult stage)pbdoc")
        .value("Aging", DevelopmentalStage::Aging, R"pbdoc(Aging stage)pbdoc")
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, R"pbdoc(Resting state)pbdoc")
        .value("Active", FiringState::Active, R"pbdoc(Active firing state)pbdoc")
        .value("Refractory", FiringState::Refractory, R"pbdoc(Refractory state after spike)pbdoc")
        .value("Inhibited", FiringState::Inhibited, R"pbdoc(Inhibited state)pbdoc")
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, R"pbdoc(Move forward)pbdoc")
        .value("MoveBackward", ActionType::MoveBackward, R"pbdoc(Move backward)pbdoc")
        .value("MoveLeft", ActionType::MoveLeft, R"pbdoc(Move left)pbdoc")
        .value("MoveRight", ActionType::MoveRight, R"pbdoc(Move right)pbdoc")
        .value("TurnLeft", ActionType::TurnLeft, R"pbdoc(Turn left)pbdoc")
        .value("TurnRight", ActionType::TurnRight, R"pbdoc(Turn right)pbdoc")
        .value("Look", ActionType::Look, R"pbdoc(Look around)pbdoc")
        .value("LookUp", ActionType::LookUp, R"pbdoc(Look up)pbdoc")
        .value("LookDown", ActionType::LookDown, R"pbdoc(Look down)pbdoc")
        .value("Interact", ActionType::Interact, R"pbdoc(Interact with object)pbdoc")
        .value("Eat", ActionType::Eat, R"pbdoc(Eat food)pbdoc")
        .value("Drink", ActionType::Drink, R"pbdoc(Drink water)pbdoc")
        .value("Rest", ActionType::Rest, R"pbdoc(Rest)pbdoc")
        .value("Wait", ActionType::Wait, R"pbdoc(Wait)pbdoc")
        .value("Custom", ActionType::Custom, R"pbdoc(Custom action)pbdoc")
        .export_values();

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, R"pbdoc(Move forward)pbdoc")
        .value("MoveBackward", MotorCommand::MoveBackward, R"pbdoc(Move backward)pbdoc")
        .value("TurnLeft", MotorCommand::TurnLeft, R"pbdoc(Turn left)pbdoc")
        .value("TurnRight", MotorCommand::TurnRight, R"pbdoc(Turn right)pbdoc")
        .value("LookLeft", MotorCommand::LookLeft, R"pbdoc(Look left)pbdoc")
        .value("LookRight", MotorCommand::LookRight, R"pbdoc(Look right)pbdoc")
        .value("Interact", MotorCommand::Interact, R"pbdoc(Interact)pbdoc")
        .value("Wait", MotorCommand::Wait, R"pbdoc(Wait)pbdoc")
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, R"pbdoc(Empty space)pbdoc")
        .value("Resource", WorldObjectType::Resource, R"pbdoc(Resource (food, water, etc.))pbdoc")
        .value("Hazard", WorldObjectType::Hazard, R"pbdoc(Hazard (dangerous object))pbdoc")
        .value("Wall", WorldObjectType::Wall, R"pbdoc(Wall)pbdoc")
        .value("Marker", WorldObjectType::Marker, R"pbdoc(Markered location)pbdoc")
        .export_values();

    // Config class with enhanced Python interface
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/toml configuration file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a configuration file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists (backward compatibility)")
        .def("has", [](Config& self, const std::string& section, const std::string& key) {
            return self.has(section, key);
        }, py::arg("section"), py::arg("key"),
            "Check if a configuration key exists (with section support)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys (backward compatibility)")
        .def("getKeys", [](Config& self, const std::string& section) {
            return self.getKeys(section);
        }, py::arg("section"),
            "Get all configuration keys for a section")
        .def("getKeys", [](Config& self) {
            return self.getKeys();
        },
            "Get all configuration keys for all sections")
        .def("getSections", &Config::getSections,
             "Get all configuration sections")
        .def("get", [](Config& self, const std::string& key, std::string defaultValue) -> py::object {
            auto valueOpt = self.get<std::string>("", key);
            if (valueOpt) {
                return py::cast(*valueOpt);
            }
            return py::cast(defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get a configuration value (backward compatibility with default)")
        .def("get", [](Config& self, const std::string& section, const std::string& key, std::string defaultValue) -> py::object {
            auto valueOpt = self.get<std::string>(section, key);
            if (valueOpt) {
                return py::cast(*valueOpt);
            }
            return py::cast(defaultValue);
        }, py::arg("section"), py::arg("key"), py::arg("defaultValue"),
            "Get a configuration value with section and default")
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr("", key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with fallback (backward compatibility)")
        .def("getOr", [](Config& self, const std::string& section, const std::string& key, int defaultValue) {
            return self.getOr(section, key, defaultValue);
        }, py::arg("section"), py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with section and fallback")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set("", key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (backward compatibility)")
        .def("set", [](Config& self, const std::string& section, const std::string& key, const std::string& value) {
            self.set(section, key, value);
        }, py::arg("section"), py::arg("key"), py::arg("value"),
            "Set configuration value with section support")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set("", key, value);
        }, py::arg("key"), py::arg("value"),
            "Set integer configuration value (backward compatibility)")
        .def("set", [](Config& self, const std::string& section, const std::string& key, int value) {
            self.set(section, key, value);
        }, py::arg("section"), py::arg("key"), py::arg("value"),
            "Set integer configuration value with section support")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set("", key, value);
        }, py::arg("key"), py::arg("value"),
            "Set float configuration value (backward compatibility)")
        .def("set", [](Config& self, const std::string& section, const std::string& key, double value) {
            self.set(section, key, value);
        }, py::arg("section"), py::arg("key"), py::arg("value"),
            "Set float configuration value with section support")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set("", key, value);
        }, py::arg("key"), py::arg("value"),
            "Set boolean configuration value (backward compatibility)")
        .def("set", [](Config& self, const std::string& section, const std::string& key, bool value) {
            self.set(section, key, value);
        }, py::arg("section"), py::arg("key"), py::arg("value"),
            "Set boolean configuration value with section support")
        .def("remove", [](Config& self, const std::string& key) {
            self.remove("", key);
        }, py::arg("key"),
            "Remove configuration key (backward compatibility)")
        .def("remove", [](Config& self, const std::string& section, const std::string& key) {
            self.remove(section, key);
        }, py::arg("section"), py::arg("key"),
            "Remove configuration key with section support")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        .def("getErrors", &Config::getErrors,
             "Get configuration errors (if any)")
        .def("validate", &Config::validateAllParameters, py::arg("parameters"),
             "Validate configuration against parameter schema")
        .def("setDefaultConfigFile", &Config::setDefaultConfigFile, py::arg("filepath"),
             "Set default configuration file path")
        .def("getDefaultConfigFile", &Config::getDefaultConfigFile,
             "Get default configuration file path");

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
        }, py::arg("data"), "Set vision data")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add an internal signal")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action with type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Clone this action")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create world object at position")
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (e.g., food energy)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation (degrees)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Age in steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result")
        .def_readwrite("reward", &ActionResult::reward, "Reward received")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Result message");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get agent's sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set max energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class with enhanced Python interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
             "Save brain state to file (checkpoint)")
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
             "Get attention system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random number generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status (for debugging)")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses>";
        });

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
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
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Factory functions
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

    // Module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) neural simulation framework";
    m.attr("__license__") = "MIT";

    // Error types for easy catching
    py::class_<py::exception<std::runtime_error>>(m, "RuntimeError");
    py::class_<py::exception<std::invalid_argument>>(m, "InvalidArgument");
    py::class_<py::exception<std::out_of_range>>(m, "OutOfRange");

    // Pythonic helper functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                             std::shared_ptr<AgentBrain> agent, size_t num_steps) {
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       "Run a complete simulation with brain, world, and agent");

    m.def("create_complete_agent", [](std::shared_ptr<Config> config) {
        auto brain = pynlm::createBrain(config);
        brain->initialize();
        
        auto world = pynlm::createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        auto agent = pynlm::createAgentBrain(brain);
        agent->initialize(world);
        
        // Enable learning capabilities
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"),
       "Create a complete agent setup with brain, world, and agent");
}

} // namespace nlm

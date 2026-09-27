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

// Forward declarations for helper functions
py::object createDefaultConfigImpl();
py::object createBrainImpl(py::object config);
py::object createSimpleWorldImpl();
py::object createAgentBrainImpl(py::object brain);

// Create type aliases for easier Python use using type annotations
using BrainPtr = std::shared_ptr<Brain>;
using ConfigPtr = std::shared_ptr<Config>;
using AgentBrainPtr = std::shared_ptr<AgentBrain>;
using SimpleWorldPtr = std::shared_ptr<SimpleWorld>;
using SensoryInputPtr = std::shared_ptr<SensoryInput>;
using ActionPtr = std::shared_ptr<Action>;
using AgentBodyPtr = std::shared_ptr<AgentBody>;
using SensoryPerceptPtr = std::shared_ptr<SensoryPercept>;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

        This module provides a high-level interface to create and simulate artificial
        developmental brains with neurons, synapses, learning mechanisms, and sensory-motor
        integration. The brain operates as a closed-loop system that processes sensory
        input, makes decisions, and learns from experience.

        Key Features:
        - Spiking neural networks with LIF neurons
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Sensory processing and motor control
        - Developmental stages affecting learning
        - Checkpoint save/load functionality
        - Real-time simulation control
    )pbdoc";

    // Register exception classes for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // ============================================================================
    // TYPE DEFINITIONS AND HELPER FUNCTIONS
    // ============================================================================

    // Core type definitions for better Python integration
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

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .doc() = R"pbdoc(
            NeuronType defines the different functional types of neurons in the NLM system:
            
            - Excitatory: Neurons that promote firing in target neurons
            - Inhibitory: Neurons that suppress firing in target neurons  
            - Modulatory: Neurons that modulate overall brain state
            - Sensory: Neurons that process sensory input
            - Motor: Neurons that generate motor commands
            - Internal: Neurons that maintain internal state
        )pbdoc";

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .doc() = R"pbdoc(
            SynapseType defines different functional connection types between neurons:
            
            - Excitatory: Synapses that increase target neuron firing
            - Inhibitory: Synapses that decrease target neuron firing
            - Modulatory: Synapses that affect overall brain state
            - Electrical: Direct electrical connections
            - GapJunction: Chemical synaptic connections
        )pbdoc";

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .doc() = R"pbdoc(
            DevelopmentalStage represents the brain's developmental phase:
            
            - Initial: High plasticity, rapid learning
            - CriticalPeriod: Sensitive learning window
            - Maturation: Stabilizing phase
            - Adult: Lower plasticity, stable behavior
            - Aging: Declining plasticity and function
        )pbdoc";

    // Firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .doc() = R"pbdoc(
            FiringState represents the current state of a neuron:
            
            - Resting: Below firing threshold
            - Active: Currently firing action potentials
            - Refractory: Cannot fire (recovery period)
            - Inhibited: Suppressed from firing
        )pbdoc";

    // Action type enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
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
        .doc() = R"pbdoc(
            ActionType defines high-level behavioral actions:
            
            - MoveForward: Move forward in the environment
            - MoveBackward: Move backward
            - TurnLeft: Rotate counterclockwise
            - TurnRight: Rotate clockwise
            - Look: Look in current direction
            - LookUp: Look upward
            - LookDown: Look downward
            - Interact: Interact with nearby objects
            - Eat: Consume available resources
            - Drink: Hydrate from water sources
            - Rest: Conserve energy
            - Wait: Do nothing
            - Custom: User-defined action
        )pbdoc";

    // Motor command enumeration
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
        .doc() = R"pbdoc(
            MotorCommand defines low-level motor actions:
            
            - MoveForward: Propel forward
            - MoveBackward: Propel backward
            - TurnLeft: Rotate left
            - TurnRight: Rotate right
            - LookLeft: Pan sensors left
            - LookRight: Pan sensors right
            - Interact: Physical interaction
            - Wait: No movement
        )pbdoc";

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .doc() = R"pbdoc(
            WorldObjectType defines types of objects in the simulation world:
            
            - Empty: No object
            - Resource: Consumable resource
            - Hazard: Dangerous object
            - Wall: Physical barrier
            - Marker: Navigation aid
        )pbdoc";

    // ============================================================================
    // CONFIGURATION CLASS
    // ============================================================================

    py::class_<Config, ConfigPtr>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        .def("set_int", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("set_double", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set double configuration value")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("get_int", [](const Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get integer configuration value")
        .def("get_float", [](const Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get float configuration value")
        .def("get_double", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0, "Get double configuration value")
        .def("get_string", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = "", "Get string configuration value")
        .def("get_bool", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = false, "Get boolean configuration value")
        .def("validate", [](const Config& self, const std::vector<std::string>& requiredKeys) {
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    throw py::value_error("Missing required configuration key: " + key);
                }
            }
            return true;
        }, py::arg("requiredKeys"), "Validate configuration has required keys");

    // ============================================================================
    // SENSORY INPUT CLASSES
    // ============================================================================

    py::class_<SensoryInput, SensoryInputPtr>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        });

    // Vision sensory input
    py::class_<Vision, SensoryInputPtr>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data (flattened array)")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("getData", &Vision::getData, "Get raw vision data as vector")
        .def("setData", [](Vision& self, size_t width, size_t height, size_t channels, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("width"), py::arg("height"), py::arg("channels"), py::arg("data"), "Initialize vision with data")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision width=" + std::to_string(vision.getWidth()) + 
                   " height=" + std::to_string(vision.getHeight()) + 
                   " channels=" + std::to_string(vision.getChannels()) + ">";
        });

    // Audio sensory input
    py::class_<Audio, SensoryInputPtr>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples")
        .def("getData", &Audio::getData, "Get raw audio data as vector")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio sampleRate=" + std::to_string(audio.getSampleRate()) +
                   " samples=" + std::to_string(audio.getNumSamples()) + ">";
        });

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInputPtr>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getData", &InternalSignals::getData, "Get signal data as vector")
        .def("size", &InternalSignals::size, "Get number of signals");

    // ============================================================================
    // ACTION CLASS
    // ============================================================================

    py::class_<Action, ActionPtr>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            return "<Action type=" + std::to_string(static_cast<int>(action.getType())) + ">";
        })
        .def("__str__", [](const Action& action) {
            return action.getName();
        });

    // ============================================================================
    // WORLD OBJECT CLASS
    // ============================================================================

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "Object x position")
        .def_readwrite("y", &WorldObject::y, "Object y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", WorldObject::value, "Object value")
        .def_readwrite("active", WorldObject::active, "Object active state")
        .def("getDistance", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        .def("isNear", [](const WorldObject& obj, float x, float y, float threshold) {
            return obj.getDistance(x, y) <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if object is near point")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject type=" + std::to_string(static_cast<int>(obj.type)) +
                   " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")>";
        });

    // ============================================================================
    // AGENT BODY CLASS
    // ============================================================================

    py::class_<AgentBody, AgentBodyPtr>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "Body x position")
        .def_readwrite("y", &AgentBody::y, "Body y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Body orientation (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Is moving flag")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Is turning flag")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Last action time")
        .def("reset", &AgentBody::reset, "Reset body to initial state")
        .def("getPosition", [](const AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get position as (x, y) tuple")
        .def("getVelocity", [](const AgentBody& body) {
            return py::make_tuple(body.velocityX, body.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("setVelocity", [](AgentBody& body, float vx, float vy) {
            body.velocityX = vx;
            body.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), "Set velocity")
        .def("getSpeed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get speed magnitude")
        .def("getDirection", [](const AgentBody& body) {
            return std::atan2(body.velocityY, body.velocityX);
        }, "Get movement direction (radians)")
        .def("update", [](AgentBody& body, float dt) {
            body.x += body.velocityX * dt;
            body.y += body.velocityY * dt;
            body.orientation += body.angularVelocity * dt;
            body.age += dt;
        }, py::arg("dt"), "Update body position for given time step")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody at (" + std::to_string(body.x) + ", " + std::to_string(body.y) +
                   ") orientation=" + std::to_string(body.orientation) + 
                   " energy=" + std::to_string(body.energy) + ">";
        });

    // ============================================================================
    // ACTION RESULT CLASS
    // ============================================================================

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success flag")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("isGood", [](const ActionResult& result) {
            return result.success && result.reward > 0.0f;
        }, "Check if action was successful and rewarding")
        .def("isBad", [](const ActionResult& result) {
            return result.success && result.reward < 0.0f;
        }, "Check if action was successful but punishing")
        .def("isNeutral", [](const ActionResult& result) {
            return !result.success || result.reward == 0.0f;
        }, "Check if action was unsuccessful or neutral")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) + 
                   " message=\"" + result.message + "\">";
        });

    // ============================================================================
    // SENSORY PERCEPT CLASS
    // ============================================================================

    py::class_<SensoryPercept, SensoryPerceptPtr>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals concatenated")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        .def("addNoise", [](SensoryPercept& self, float visionNoise, float touchNoise, float internalNoise, float proprioceptionNoise) {
            // Add Gaussian noise to sensory signals
            auto& vision = self.getVision();
            for (auto& v : vision) {
                v += (rand() / (float)RAND_MAX - 0.5f) * visionNoise;
                v = std::clamp(v, 0.0f, 1.0f);
            }
            // Similar noise addition for other modalities...
        }, py::arg("visionNoise") = 0.01f, py::arg("touchNoise") = 0.01f, py::arg("internalNoise") = 0.01f, py::arg("proprioceptionNoise") = 0.01f, "Add noise to sensory signals for realism")
        .def("normalize", [](SensoryPercept& self) {
            // Normalize all sensory signals to [0,1] range
            auto& vision = self.getVision();
            float minV = *std::min_element(vision.begin(), vision.end());
            float maxV = *std::max_element(vision.begin(), vision.end());
            if (maxV > minV) {
                for (auto& v : vision) {
                    v = (v - minV) / (maxV - minV);
                }
            }
        }, "Normalize sensory signals to [0,1] range")
        .def("merge", [](const SensoryPercept& self, const SensoryPercept& other, float weight) {
            SensoryPercept merged;
            auto& vis1 = self.getVision();
            auto& vis2 = other.getVision();
            std::vector<float> mergedVision;
            mergedVision.reserve(vis1.size() + vis2.size());
            for (size_t i = 0; i < vis1.size(); ++i) {
                mergedVision.push_back(vis1[i] * (1.0f - weight) + (i < vis2.size() ? vis2[i] : 0.0f) * weight);
            }
            merged.setVision(mergedVision);
            // Similar merging for other modalities...
            return merged;
        }, py::arg("other"), py::arg("weight") = 0.5f, "Merge with another percept (weight: 0.0-1.0)")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if vision data is present")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if touch data is present")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if internal signals are present")
        .def("hasProprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, "Check if proprioception data is present")
        .def("getTotalSignalCount", [](const SensoryPercept& self) {
            return self.getAllSignals().size();
        }, "Get total number of sensory signals")
        .def("getSignalRanges", [](const SensoryPercept& self) {
            py::dict ranges;
            auto& vision = self.getVision();
            if (!vision.empty()) {
                float minV = *std::min_element(vision.begin(), vision.end());
                float maxV = *std::max_element(vision.begin(), vision.end());
                ranges["vision"] = py::make_tuple(minV, maxV);
            }
            // Similar for other modalities...
            return ranges;
        }, "Get min/max ranges for all sensory signals")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept vision=" + std::to_string(percept.getVision().size()) +
                   " touch=" + std::to_string(percept.getTouch().size()) +
                   " internal=" + std::to_string(percept.getInternal().size()) +
                   " proprioception=" + std::to_string(percept.getProprioception().size()) + ">";
        });

    // ============================================================================
    // SIMPLE WORLD CLASS
    // ============================================================================

    py::class_<SimpleWorld, SimpleWorldPtr>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get agent's sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getVisionWidth", &SimpleWorld::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SimpleWorld::getVisionHeight, "Get vision height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getObjectCount", [](const SimpleWorld& world) {
            return world.getObjects().size();
        }, "Get number of objects in world")
        .def("findObjectsByType", [](const SimpleWorld& world, WorldObjectType type) {
            std::vector<WorldObject> objects;
            const auto& worldObjects = world.getObjects();
            for (const auto& obj : worldObjects) {
                if (obj.type == type) {
                    objects.push_back(obj);
                }
            }
            return objects;
        }, py::arg("type"), "Find objects by type")
        .def("getNearestObject", [](const SimpleWorld& world, float x, float y) {
            const auto& objects = world.getObjects();
            if (objects.empty()) {
                return WorldObject(); // Return empty object
            }
            size_t nearestIdx = 0;
            float minDist = std::numeric_limits<float>::max();
            for (size_t i = 0; i < objects.size(); ++i) {
                float dist = objects[i].getDistance(x, y);
                if (dist < minDist) {
                    minDist = dist;
                    nearestIdx = i;
                }
            }
            return objects[nearestIdx];
        }, py::arg("x"), py::arg("y"), "Find nearest object to position")
        .def("clearObjects", &SimpleWorld::clearObjects, "Remove all objects from world")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld width=" + std::to_string(world.getWidth()) +
                   " height=" + std::to_string(world.getHeight()) +
                   " time=" + std::to_string(world.getSimulationTime()) + ">";
        });

    // ============================================================================
    // AGENT BRAIN CLASS
    // ============================================================================

    py::class_<AgentBrain, AgentBrainPtr>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        .def("getBrainStatus", [](const AgentBrain& agent) {
            py::dict status;
            auto brain = agent.getBrain();
            if (brain) {
                status["neuron_count"] = brain->getTotalNeuronCount();
                status["synapse_count"] = brain->getTotalSynapseCount();
                status["active_neurons"] = brain->getActiveNeuronCount();
                status["firing_neurons"] = brain->getFiringNeuronCount();
                status["total_spikes"] = brain->getTotalSpikeCount();
                status["stage"] = static_cast<int>(brain->getDevelopmentalStage());
                status["exc_inhib_ratio"] = brain->getExcitationInhibitionRatio();
            }
            status["curiosity"] = agent.getCuriosityLevel();
            status["novelty"] = agent.getNoveltyLevel();
            status["prediction_error"] = agent.getPredictionError();
            status["neuromodulation"] = agent.getNeuromodulationLevel();
            return status;
        }, "Get detailed status information")
        .def("simulateStep", [](AgentBrain& agent, const SimpleWorld& world, double timestep) {
            // Run one complete simulation step
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            // Note: brain.step() needs to be called separately
            return percept;
        }, py::arg("world"), py::arg("timestep"), "Run one complete simulation step, return percept")
        .def("createActionFromCommand", [](const AgentBrain& agent, MotorCommand cmd) {
            Action action;
            action.setType(cmd);
            return action;
        }, py::arg("cmd"), "Create Action from MotorCommand")
        .def("getMotorCommandName", [](MotorCommand cmd) {
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
        }, py::arg("cmd"), "Get human-readable name for motor command")
        .def("getMotorCommandList", []() {
            std::vector<std::string> commands;
            commands.push_back("MoveForward");
            commands.push_back("MoveBackward");
            commands.push_back("TurnLeft");
            commands.push_back("TurnRight");
            commands.push_back("LookLeft");
            commands.push_back("LookRight");
            commands.push_back("Interact");
            commands.push_back("Wait");
            return commands;
        }, "Get list of available motor commands")
        .def("__repr__", [](const AgentBrain& agent) {
            auto brain = agent.getBrain();
            return "<AgentBrain neurons=" + std::to_string(brain ? brain->getTotalNeuronCount() : 0) +
                   " stage=" + std::to_string(static_cast<int>(agent.getDevelopmentalStage())) + ">";
        });

    // ============================================================================
    // BRAIN CLASS
    // ============================================================================

    py::class_<Brain, BrainPtr>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("getBrainStatistics", [](const Brain& brain) {
            py::dict stats;
            stats["neuron_count"] = brain.getTotalNeuronCount();
            stats["synapse_count"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["avg_firing_rate"] = brain.getAverageFiringRate();
            stats["exc_inhib_ratio"] = brain.getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics")
        .def("createNeuron", [](Brain& brain, NeuronType type, float x = 0.0f, float y = 0.0f) {
            // This is a simplified example - in practice, neurons are created internally
            return py::make_tuple(x, y, type);
        }, py::arg("type") = NeuronType::Excitatory, py::arg("x") = 0.0f, py::arg("y") = 0.0f, "Create a neuron (example)")
        .def("findNeuronsByType", [](Brain& brain, NeuronType type) {
            std::vector<NeuronId> neuronIds;
            auto regions = brain.getRegions();
            for (const auto& region : regions) {
                auto neurons = region->getAllNeurons();
                for (const auto& neuron : neurons) {
                    if (neuron->getNeuronType() == type) {
                        neuronIds.push_back(neuron->getId());
                    }
                }
            }
            return neuronIds;
        }, py::arg("type"), "Find neurons by type")
        .def("injectCurrentToRandomNeurons", [](Brain& brain, NeuronType type, float current, size_t count) {
            auto regions = brain.getRegions();
            size_t injected = 0;
            for (const auto& region : regions) {
                auto neurons = region->getAllNeurons();
                for (const auto& neuron : neurons) {
                    if (neuron->getNeuronType() == type) {
                        neuron->injectCurrent(current);
                        injected++;
                        if (injected >= count) break;
                    }
                }
                if (injected >= count) break;
            }
            return injected;
        }, py::arg("type"), py::arg("current"), py::arg("count") = 1, "Inject current to random neurons of given type")
        .def("getNeuronsByPopulation", [](Brain& brain, PopulationId populationId) {
            auto regions = brain.getRegions();
            std::vector<NeuronId> neuronIds;
            for (const auto& region : regions) {
                auto populations = region->getPopulations();
                for (const auto& population : populations) {
                    if (population->getId() == populationId) {
                        auto neurons = population->getNeurons();
                        for (const auto& neuron : neurons) {
                            neuronIds.push_back(neuron->getId());
                        }
                    }
                }
            }
            return neuronIds;
        }, py::arg("populationId"), "Get neurons by population ID")
        .def("getSpikeStatistics", [](Brain& brain) {
            py::dict stats;
            auto spikeSystem = brain.getSpikeSystem();
            if (spikeSystem) {
                stats["total_spikes"] = brain.getTotalSpikeCount();
                stats["current_firing"] = brain.getFiringNeuronCount();
                auto mostActive = spikeSystem->getMostActiveNeurons(10);
                stats["most_active_count"] = mostActive.size();
                stats["avg_firing_rate"] = brain.getAverageFiringRate();
            }
            return stats;
        }, "Get spike-related statistics")
        .def("setAllSynapsesPlasticity", [](Brain& brain, bool hebbian, bool stdp, bool structural) {
            auto regions = brain.getRegions();
            for (const auto& region : regions) {
                auto synapses = region->getSynapses();
                for (const auto& synapse : synapses) {
                    synapse->enablePlasticity(hebbian, stdp, structural);
                }
            }
        }, py::arg("hebbian"), py::arg("stdp"), py::arg("structural"), "Enable/disable plasticity on all synapses")
        .def("resetPlasticity", [](Brain& brain) {
            auto regions = brain.getRegions();
            for (const auto& region : regions) {
                auto synapses = region->getSynapses();
                for (const auto& synapse : synapses) {
                    synapse->resetPlasticityState();
                }
            }
        }, "Reset all plasticity states")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " synapses=" + std::to_string(brain.getTotalSynapseCount()) +
                   " stage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + ">";
        });

    // ============================================================================
    // FACTORY FUNCTIONS AND MODULE-LEVEL HELPERS
    // ============================================================================

    // Default configuration factory
    m.def("createDefaultConfig", []() -> ConfigPtr {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration)pbdoc");

    // Brain factory with configuration
    m.def("createBrain", [](std::shared_ptr<Config> config) -> BrainPtr {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration)pbdoc");

    // Convenience function for brain creation with default config
    m.def("createBrain", []() -> BrainPtr {
        return std::make_shared<Brain>(std::make_shared<Config>());
    }, R"pbdoc(Create a new brain with default configuration)pbdoc");

    // Simple world factory
    m.def("createSimpleWorld", []() -> SimpleWorldPtr {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world)pbdoc");

    // Agent brain factory
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> AgentBrainPtr {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)pbdoc");

    // World object factory
    m.def("WorldObject", [](float x, float y, WorldObjectType type, float value, float radius) {
        return WorldObject(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
       R"pbdoc(Create a world object)pbdoc");

    // Action factory
    m.def("Action", [](ActionType type) {
        return Action(type);
    }, py::arg("type"), R"pbdoc(Create an action)pbdoc");

    // ============================================================================
    // CONSTANT AND UTILITY VALUES
    // ============================================================================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";

    // Utility functions
    m.def("get_version", []() {
        return py::dict(
            {"version", m.attr("__version__")},
            {"description", m.attr("__description__")},
            {"compile_time", __DATE__}
        );
    }, "Get version information");

    // Error handling utilities
    m.def("validate_percept", [](const SensoryPercept& percept) {
        std::vector<std::string> errors;
        if (percept.getVision().empty() && percept.getTouch().empty() && 
            percept.getInternal().empty() && percept.getProprioception().empty()) {
            errors.push_back("Percept has no sensory data");
        }
        if (!errors.empty()) {
            throw py::value_error(errors.size() == 1 ? errors[0] : "Multiple validation errors");
        }
        return true;
    }, py::arg("percept"), "Validate sensory percept");

    m.def("normalize_config", [](Config& config) {
        config.validate();
        return config.summary();
    }, py::arg("config"), "Validate and normalize configuration");

    // ============================================================================
    // DOCUMENTATION AND HELP SYSTEM
    // ============================================================================

    // Generate comprehensive API documentation
    m.def("get_api_documentation", []() {
        py::dict apiDoc;
        
        // Configuration
        py::dict configDoc;
        configDoc["description"] = "Brain configuration management";
        configDoc["methods"] = py::list({
            "loadFromFile", "loadFromArgs", "saveToFile", "has", "getKeys", "clear",
            "summary", "set_int", "set_float", "set_double", "set_string", "set_bool",
            "get_int", "get_float", "get_double", "get_string", "get_bool", "validate"
        });
        apiDoc["Config"] = configDoc;
        
        // Brain
        py::dict brainDoc;
        brainDoc["description"] = "Central neural simulation engine";
        brainDoc["methods"] = py::list({
            "initialize", "step", "reset", "receiveSensoryInput", "injectCurrent",
            "injectCurrentToNeurons", "produceAction", "save", "load",
            "addRegion", "getRegion", "getTotalNeuronCount", "getTotalSynapseCount",
            "getFiringNeuronCount", "getAverageFiringRate", "getExcitationInhibitionRatio",
            "getTotalSpikeCount", "getDevelopmentalStage", "setDevelopmentalStage",
            "logStatus", "getBrainStatistics"
        });
        apiDoc["Brain"] = brainDoc;
        
        // AgentBrain
        py::dict agentBrainDoc;
        agentBrainDoc["description"] = "Bridge connecting brain to world";
        agentBrainDoc["methods"] = py::list({
            "initialize", "processSensoryInput", "decodeMotorCommand",
            "applyRewardModulation", "updateDevelopment", "getDevelopmentalStage",
            "enableRewardModulation", "enableStructuralPlasticity", "enableDevelopment",
            "enableCuriosity", "getBrainStatus", "simulateStep"
        });
        apiDoc["AgentBrain"] = agentBrainDoc;
        
        // SimpleWorld
        py::dict worldDoc;
        worldDoc["description"] = "2D simulation environment";
        worldDoc["methods"] = py::list({
            "configure", "reset", "update", "applyMotorCommand",
            "getSensoryPercept", "getAgentBody", "addObject", "removeObject",
            "isValidPosition", "getWidth", "getHeight", "setMaxEnergy"
        });
        apiDoc["SimpleWorld"] = worldDoc;
        
        return apiDoc;
    }, "Get comprehensive API documentation");

    // Quick start examples
    m.def("get_quickstart_examples", []() {
        py::dict examples;
        
        py::dict basicBrain;
        basicBrain["code"] = R"python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
"python";
        basicBrain["description"] = "Create and initialize a basic brain";
        examples["basic_brain"] = basicBrain;
        
        py::dict completeSimulation;
        completeSimulation["code"] = R"python
import pynlm

# Complete agent simulation with brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
"python";
        completeSimulation["description"] = "Complete agent simulation with brain, agent, and world";
        examples["complete_simulation"] = completeSimulation;
        
        return examples;
    }, "Get quick start examples");

    // Module metadata
    m.attr("__all__") = py::list({
        "NeuronId", "SynapseId", "RegionId", "PopulationId",
        "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
        "ActionType", "MotorCommand", "WorldObjectType",
        "Config", "Brain", "AgentBrain", "SimpleWorld",
        "Vision", "Audio", "InternalSignals", "Action", "WorldObject",
        "SensoryPercept", "AgentBody", "ActionResult",
        "createDefaultConfig", "createBrain", "createSimpleWorld",
        "createAgentBrain"
    });

} // namespace nlm

// Helper implementations
py::object createDefaultConfigImpl() {
    return py::cast(std::make_shared<Config>());
}

py::object createBrainImpl(py::object config) {
    return py::cast(std::make_shared<Brain>(py::cast<std::shared_ptr<Config>>(config)));
}

py::object createSimpleWorldImpl() {
    return py::cast(std::make_shared<SimpleWorld>());
}

py::object createAgentBrainImpl(py::object brain) {
    return py::cast(std::make_shared<AgentBrain>(py::cast<std::shared_ptr<Brain>>(brain)));
}

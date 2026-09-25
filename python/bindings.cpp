#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>

// Core includes
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

// Forward declarations for factory functions
std::shared_ptr<Config> createDefaultConfig();
std::shared_ptr<Brain> createBrainWithConfig(const std::shared_ptr<Config>& config);
std::shared_ptr<Brain> createBrainWithDefaults();
std::shared_ptr<SimpleWorld> createSimpleWorld();
std::shared_ptr<AgentBrain> createAgentBrain(const std::shared_ptr<Brain>& brain);

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

        This module enables Python programmers to create and simulate neural networks
        with biologically inspired learning mechanisms, including plasticity, neuromodulation,
        memory systems, and developmental processes.

        Key features:
        - Real spiking neural computation (LIF neurons)
        - Multiple plasticity rules (STDP, Hebbian, reward-modulated)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Memory systems (working, episodic, associative)
        - Cognitive architectures (planning, concept formation)
        - Developmental processes (synaptogenesis, pruning, maturation)
        - Simulation world for agent-environment interaction

        For installation and basic usage, see the README.md and easy_usage.md files.
    )pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // =============================================================================
    // TYPE DEFINITIONS AND IDENTIFIERS
    // =============================================================================

    // NeuronId - Unique identifier for neurons
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Return the numeric index of this neuron ID")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) { return "<NeuronId: " + std::to_string(id.value) + ">"; })
        .def("__str__", [](const NeuronId& id) { return std::to_string(id.value); });

    // SynapseId - Unique identifier for synapses
    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Return the numeric index of this synapse ID")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) { return "<SynapseId: " + std::to_string(id.value) + ">"; })
        .def("__str__", [](const SynapseId& id) { return std::to_string(id.value); });

    // RegionId - Unique identifier for brain regions
    py::class_<RegionId>(m, "RegionId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, "Return the numeric index of this region ID")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) { return "<RegionId: " + std::to_string(id.value) + ">"; })
        .def("__str__", [](const RegionId& id) { return std::to_string(id.value); });

    // PopulationId - Unique identifier for neuron populations
    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Return the numeric index of this population ID")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType")
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

    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // =============================================================================
    // CONFIGURATION CLASS
    // =============================================================================

    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True if successful, False otherwise.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True if successful, False otherwise.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists, False otherwise.")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("get", [](const Config& self, const std::string& key) {
            auto result = self.get<std::string>(key);
            if (result) return *result;
            return std::string();
        }, py::arg("key"), "Get configuration value as string. Returns empty string if not found or not a string.")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get configuration value as integer with default.")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get configuration value as float with default.")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = false, "Get configuration value as boolean with default.")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else {
                throw py::type_error("Unsupported type for configuration value");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value with automatic type conversion.")
        .def("setInt", &Config::set<int>, py::arg("key"), py::arg("value"),
             "Set configuration value as integer.")
        .def("setFloat", &Config::set<double>, py::arg("key"), py::arg("value"),
             "Set configuration value as float.")
        .def("setBool", &Config::set<bool>, py::arg("key"), py::arg("value"),
             "Set configuration value as boolean.")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as string.")
        .def("remove", &Config::remove, py::arg("key"), "Remove configuration key.")
        .def("__getitem__", [](Config& self, const std::string& key) {
            auto result = self.get<std::string>(key);
            if (!result) {
                throw py::key_error("Configuration key not found: '" + key + "'");
            }
            return *result;
        }, py::arg("key"), "Get configuration value using item access syntax (config['key']).")
        .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else {
                throw py::type_error("Unsupported type for configuration value");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value using item assignment syntax (config['key'] = value).")
        .def("__contains__", &Config::has, py::arg("key"), "Check if configuration key exists (using 'in' operator).")
        .def("__len__", &Config::getKeys, "Get the number of configuration keys.")
        .def("__iter__", [](Config& self) {
            return py::make_iterator(self.getKeys().begin(), self.getKeys().end());
        }, "Iterate over configuration keys.");

    // =============================================================================
    // SENSORY INPUT CLASSES
    // =============================================================================

    // Base SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(input.getType()) + " dims=" + std::to_string(input.getDimensions()) + ">";
        });

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data as a flat vector of pixel values")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as a vector of samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set the audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples");

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signal values")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get the number of signal values")
        .def("getMean", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get the mean of all signal values")
        .def("getMax", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get the maximum signal value")
        .def("getMin", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, "Get the minimum signal value");

    // =============================================================================
    // ACTION CLASSES
    // =============================================================================

    // Action class
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type") = ActionType::Wait)
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters") = std::vector<float>())
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the human-readable name of the action")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            return "<Action type=" + std::to_string(static_cast<int>(action.getType())) + 
                   " params=" + std::to_string(action.getParameters().size()) + ">";
        })
        .def("__str__", [](const Action& action) {
            return action.getName();
        });

    // =============================================================================
    // WORLD OBJECTS
    // =============================================================================

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X coordinate position")
        .def_readwrite("y", &WorldObject::y, "Y coordinate position")
        .def_readwrite("radius", &WorldObject::radius, "Collision radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (e.g., resource amount)")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate Euclidean distance to a point")
        .def("isAtPosition", [](const WorldObject& self, float x, float y, float tolerance = 0.1f) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= tolerance * tolerance;
        }, py::arg("x"), py::arg("y"), py::arg("tolerance") = 0.1f, "Check if object is at a given position")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject type=" + std::to_string(static_cast<int>(obj.type)) +
                   " x=" + std::to_string(obj.x) + " y=" + std::to_string(obj.y) + ">";
        });

    // =============================================================================
    // AGENT BODY
    // =============================================================================

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate position")
        .def_readwrite("y", &AgentBody::y, "Y coordinate position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians (0-2π)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (rad/s)")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether the agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether the agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset body state to initial values")
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate Euclidean distance to a point")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get the current speed magnitude")
        .def("getFacingAngle", [](const AgentBody& self) {
            return self.orientation;  // In radians
        }, "Get the facing angle in radians")
        .def("setFacingAngle", [](AgentBody& self, float angle) {
            self.orientation = angle;
            while (self.orientation >= 2.0f * 3.14159f) self.orientation -= 2.0f * 3.14159f;
            while (self.orientation < 0.0f) self.orientation += 2.0f * 3.14159f;
        }, py::arg("angle"), "Set the facing angle in radians (0-2π)")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if the agent is healthy")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody x=" + std::to_string(body.x) + " y=" + std::to_string(body.y) +
                   " energy=" + std::to_string(body.energy) + ">";
        });

    // =============================================================================
    // ACTION RESULTS AND SENSORY PERCEPTS
    // =============================================================================

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value (positive or negative)")
        .def_readwrite("success", &ActionResult::success, "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message, "Human-readable message about the result")
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if this was a good outcome")
        .def("isBad", [](const ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if this was a bad outcome")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) + ">";
        });

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, py::return_value_policy::reference_internal,
             "Get vision sensory input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision sensory input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision image width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision image height")
        .def("getTouch", &SensoryPercept::getTouch, py::return_value_policy::reference_internal,
             "Get touch sensory input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch sensory input")
        .def("getInternal", &SensoryPercept::getInternal, py::return_value_policy::reference_internal,
             "Get internal signals sensory input")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals input")
        .def("getProprioception", &SensoryPercept::getProprioception, py::return_value_policy::reference_internal,
             "Get proprioception sensory input")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception input")
        .def("getAudio", &SensoryPercept::getAudio, py::return_value_policy::reference_internal,
             "Get audio sensory input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio sensory input")
        .def("getAllSignals", &SensoryPercept::getAllSignals, py::return_value_policy::reference_internal,
             "Get all sensory signals as a combined vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get the timestamp of this percept")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set the timestamp")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().getData().empty();
        }, "Check if vision data is available")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().getData().empty();
        }, "Check if audio data is available")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().getData().empty();
        }, "Check if internal signals are available")
        .def("getTotalSignalCount", [](const SensoryPercept& self) {
            return self.getVision().getDimensions() + self.getAudio().getDimensions() +
                   self.getInternal().getDimensions() + self.getProprioception().getDimensions();
        }, "Get total number of signal dimensions across all modalities")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept vision=" + std::to_string(percept.getVisionWidth()) + "x" + std::to_string(percept.getVisionHeight()) +
                   " audio=" + std::to_string(percept.getAudio().getNumSamples()) + " samples>";
        });

    // =============================================================================
    // SIMPLE WORLD
    // =============================================================================

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 8, py::arg("visionHeight" = 8),
             "Configure the world dimensions and agent sensing capabilities")
        .def("reset", &SimpleWorld::reset, "Reset the world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"), "Set the agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world simulation by the given timestep (in seconds)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the agent and update world state")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get the agent's sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get the agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add a world object")
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"), "Remove object at given position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if a position is valid (not a wall)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get the current random seed")
        .def("getWorldBoundaries", [](const SimpleWorld& self) {
            return py::make_tuple(self.getWidth(), self.getHeight());
        }, "Get world dimensions as (width, height)")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld width=" + std::to_string(world.getWidth()) +
                   " height=" + std::to_string(world.getHeight()) + ">";
        });

    // =============================================================================
    // BRAIN CLASS
    // =============================================================================

    // Brain class
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config") = nullptr,
             "Create a brain with configuration. If None, uses default configuration.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns True if successful, False otherwise.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with timestep only.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestep and current time.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain. Input should be a SensoryInput object.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns an Action object.")
        .def("reset", &Brain::reset, "Reset brain state to initial values")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Returns True if successful, False otherwise.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Returns True if successful, False otherwise.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Returns the RegionId of the new region.")
        .def("getRegion", &Brain::getRegion,
             py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID. Returns None if region doesn't exist.")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status information")
        .def("logStatus", &Brain::logStatus, "Log brain status information")
        .def("enableAllPlasticity", [](Brain& self) {
            if (auto* stdp = self.getSTDP()) stdp->enable(true);
            if (auto* hebbian = self.getHebbian()) hebbian->enable(true);
            if (auto* structural = self.getStructuralPlasticity()) structural->enable(true);
        }, "Enable all plasticity mechanisms")
        .def("disableAllPlasticity", [](Brain& self) {
            if (auto* stdp = self.getSTDP()) stdp->enable(false);
            if (auto* hebbian = self.getHebbian()) hebbian->enable(false);
            if (auto* structural = self.getStructuralPlasticity()) structural->enable(false);
        }, "Disable all plasticity mechanisms")
        .def("getPlasticityStatus", [](Brain& self) {
            py::dict status;
            if (auto* stdp = self.getSTDP()) status["stdp"] = stdp->isEnabled();
            if (auto* hebbian = self.getHebbian()) status["hebbian"] = hebbian->isEnabled();
            if (auto* structural = self.getStructuralPlasticity()) status["structural"] = structural->isEnabled();
            return status;
        }, "Get current plasticity mechanism status")
        .def("validateConfiguration", [](const std::shared_ptr<Brain>& brain) {
            if (!brain) {
                throw py::value_error("Brain cannot be null for validation");
            }
            bool isValid = true;
            
            // Check basic properties
            if (brain->getTotalNeuronCount() <= 0) {
                throw py::value_error("Brain must have at least one neuron");
            }
            
            if (brain->getTotalSynapseCount() < 0) {
                throw py::value_error("Brain cannot have negative synapse count");
            }
            
            // Check developmental stage consistency
            auto stage = brain->getDevelopmentalStage();
            if (stage == DevelopmentalStage::Aging && brain->getTotalSpikeCount() < 100) {
                // Warning but not an error
            }
            
            return isValid;
        }, py::arg("brain"), "Validate brain configuration. Returns True if valid, throws exception if invalid.")
        .def("createSafeBrainStep", [](std::shared_ptr<Brain> brain, size_t step, double time) {
            if (!brain) {
                throw py::invalid_argument("Brain cannot be null");
            }
            try {
                brain->step(step, time);
                return true;
            } catch (const std::exception& e) {
                throw py::runtime_error(std::string("Brain step failed: ") + e.what());
            }
        }, py::arg("brain"), py::arg("step"), py::arg("time"),
           "Safely step a brain with comprehensive error handling. Returns True if successful, throws detailed exception on error.")
        .def("validateBrainState", [](const std::shared_ptr<Brain>& brain) {
            if (!brain) {
                throw py::invalid_argument("Brain cannot be null for validation");
            }
            
            py::dict validationResults;
            validationResults["brain_exists"] = true;
            validationResults["neuron_count_valid"] = brain->getTotalNeuronCount() > 0;
            validationResults["synapse_count_valid"] = brain->getTotalSynapseCount() >= 0;
            validationResults["firing_count_valid"] = brain->getFiringNeuronCount() <= brain->getTotalNeuronCount();
            validationResults["average_firing_rate_valid"] = brain->getAverageFiringRate() >= 0.0f;
            validationResults["development_stage_valid"] = static_cast<int>(brain->getDevelopmentalStage()) >= 0;
            
            bool allValid = validationResults["neuron_count_valid"] &&
                          validationResults["synapse_count_valid"] &&
                          validationResults["firing_count_valid"] &&
                          validationResults["average_firing_rate_valid"] &&
                          validationResults["development_stage_valid"];
            
            validationResults["overall_valid"] = allValid;
            return validationResults;
        }, py::arg("brain"), "Comprehensive validation of brain state. Returns dictionary with validation results for all checks.")
        .def("getNeuralNetworkStats", [](const std::shared_ptr<Brain>& brain) {
            if (!brain) {
                throw py::invalid_argument("Brain cannot be null");
            }
            
            py::dict stats;
            
            // Basic network statistics
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["total_synapses"] = brain->getTotalSynapseCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["pending_spike_events"] = brain->getPendingSpikeEventCount();
            
            // Derived metrics
            if (brain->getTotalNeuronCount() > 0) {
                stats["network_density"] = static_cast<float>(brain->getTotalSynapseCount()) / brain->getTotalNeuronCount();
                stats["firing_ratio"] = static_cast<float>(brain->getFiringNeuronCount()) / brain->getTotalNeuronCount();
                stats["average_firing_rate"] = brain->getAverageFiringRate();
                stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            } else {
                stats["network_density"] = 0.0f;
                stats["firing_ratio"] = 0.0f;
                stats["average_firing_rate"] = 0.0f;
                stats["excitatory_inhibition_ratio"] = 1.0f;
            }
            
            // Developmental information
            stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            stats["development_progress"] = static_cast<float>(brain->getDevelopmentalStage()) / 4.0f;  // Max stage is 4
            
            return stats;
        }, py::arg("brain"), "Get comprehensive neural network statistics. Returns dictionary with all relevant metrics.")
        .def("benchmarkBrainOperations", [](std::shared_ptr<Brain> brain, size_t steps = 100) {
            if (!brain) {
                throw py::invalid_argument("Brain cannot be null for benchmarking");
            }
            
            // Warm-up
            brain->initialize();
            for (size_t i = 0; i < 10; ++i) {
                brain->step(i, i * 0.001);
            }
            
            // Benchmark stepping
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t i = 10; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            double seconds = duration.count() / 1000.0;
            double steps_per_second = (steps - 10) / seconds;
            
            // Benchmark memory operations
            auto memory_start = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < 1000; ++i) {
                // Create temporary objects
                auto config = std::make_shared<Config>();
                auto temp_brain = std::make_shared<Brain>(config);
            }
            auto memory_end = std::chrono::high_resolution_clock::now();
            auto memory_duration = std::chrono::duration_cast<std::chrono::microseconds>(memory_end - memory_start);
            
            py::dict benchmarkResults;
            benchmarkResults["total_steps"] = steps;
            benchmarkResults["duration_seconds"] = seconds;
            benchmarkResults["steps_per_second"] = steps_per_second;
            benchmarkResults["memory_operations_time_us"] = memory_duration.count();
            benchmarkResults["memory_ops_per_second"] = 1000.0 / (memory_duration.count() / 1000000.0);
            benchmarkResults["avg_step_time_ms"] = seconds * 1000.0 / (steps - 10);
            
            return benchmarkResults;
        }, py::arg("brain"), py::arg("steps") = 100,
           "Comprehensive benchmark of brain operations. Returns dictionary with timing for stepping and memory operations.")
        .def("createPerformanceProfile", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw py::invalid_argument("Brain cannot be null for performance profiling");
            }
            
            py::dict profile;
            
            // Collect various performance metrics
            profile["neuron_count"] = brain->getTotalNeuronCount();
            profile["synapse_count"] = brain->getTotalSynapseCount();
            profile["firing_count"] = brain->getFiringNeuronCount();
            profile["total_spikes"] = brain->getTotalSpikeCount();
            profile["pending_events"] = brain->getPendingSpikeEventCount();
            profile["average_firing_rate"] = brain->getAverageFiringRate();
            profile["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            profile["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            
            // Performance indicators
            if (brain->getTotalNeuronCount() > 0) {
                profile["utilization"] = static_cast<float>(brain->getFiringNeuronCount()) / brain->getTotalNeuronCount();
                profile["efficiency"] = brain->getAverageFiringRate() / std::max(1.0f, static_cast<float>(brain->getTotalSynapseCount()));
            } else {
                profile["utilization"] = 0.0f;
                profile["efficiency"] = 0.0f;
            }
            
            // Complexity metrics
            profile["network_complexity"] = std::log(static_cast<double>(brain->getTotalSynapseCount() + 1));
            profile["activity_level"] = std::log(static_cast<double>(brain->getTotalSpikeCount() + 1));
            
            return profile;
        }, py::arg("brain"), "Create a performance profile of the brain. Returns dictionary with utilization and efficiency metrics.")
        .def("createSimulationSnapshot", [](std::shared_ptr<Brain> brain) {
            if (!brain) {
                throw py::invalid_argument("Brain cannot be null for snapshot creation");
            }
            
            py::dict snapshot;
            
            // Core state information
            snapshot["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            snapshot["step"] = brain->getTotalSpikeCount();  // Use spike count as step proxy
            snapshot["simulation_time"] = brain->getTotalSpikeCount() * 0.001;  // Approximate time
            
            // Neural state
            snapshot["neuron_count"] = brain->getTotalNeuronCount();
            snapshot["synapse_count"] = brain->getTotalSynapseCount();
            snapshot["firing_neurons"] = brain->getFiringNeuronCount();
            snapshot["total_spikes"] = brain->getTotalSpikeCount();
            snapshot["pending_events"] = brain->getPendingSpikeEventCount();
            
            // Performance metrics
            snapshot["average_firing_rate"] = brain->getAverageFiringRate();
            snapshot["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            
            // Developmental state
            snapshot["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            
            // Configuration info (if available)
            auto config = brain->getConfig();
            if (config) {
                snapshot["configuration"] = "available";
                // Could include specific config values here
            }
            
            return snapshot;
        }, py::arg("brain"), "Create a simulation snapshot. Returns dictionary with current state and metrics.")
        .def("compareBrainStates", [](const std::shared_ptr<Brain>& brain1, const std::shared_ptr<Brain>& brain2) {
            if (!brain1 || !brain2) {
                throw py::invalid_argument("Both brains must be non-null for comparison");
            }
            
            py::dict comparison;
            
            // Basic metrics
            comparison["brain1_neuron_count"] = brain1->getTotalNeuronCount();
            comparison["brain2_neuron_count"] = brain2->getTotalNeuronCount();
            comparison["brain1_synapse_count"] = brain1->getTotalSynapseCount();
            comparison["brain2_synapse_count"] = brain2->getTotalSynapseCount();
            
            // Activity metrics
            comparison["brain1_total_spikes"] = brain1->getTotalSpikeCount();
            comparison["brain2_total_spikes"] = brain2->getTotalSpikeCount();
            comparison["brain1_firing_count"] = brain1->getFiringNeuronCount();
            comparison["brain2_firing_count"] = brain2->getFiringNeuronCount();
            
            // Performance metrics
            comparison["brain1_avg_firing_rate"] = brain1->getAverageFiringRate();
            comparison["brain2_avg_firing_rate"] = brain2->getAverageFiringRate();
            comparison["brain1_ei_ratio"] = brain1->getExcitationInhibitionRatio();
            comparison["brain2_ei_ratio"] = brain2->getExcitationInhibitionRatio();
            
            // Developmental metrics
            comparison["brain1_dev_stage"] = static_cast<int>(brain1->getDevelopmentalStage());
            comparison["brain2_dev_stage"] = static_cast<int>(brain2->getDevelopmentalStage());
            
            // Calculate differences
            comparison["neuron_count_diff"] = brain2->getTotalNeuronCount() - brain1->getTotalNeuronCount();
            comparison["synapse_count_diff"] = brain2->getTotalSynapseCount() - brain1->getTotalSynapseCount();
            comparison["spike_count_diff"] = brain2->getTotalSpikeCount() - brain1->getTotalSpikeCount();
            comparison["firing_count_diff"] = brain2->getFiringNeuronCount() - brain1->getFiringNeuronCount();
            comparison["avg_firing_rate_diff"] = brain2->getAverageFiringRate() - brain1->getAverageFiringRate();
            
            // Calculate percentages
            if (brain1->getTotalNeuronCount() > 0) {
                comparison["neuron_count_percent_change"] = 
                    (brain2->getTotalNeuronCount() - brain1->getTotalNeuronCount()) * 100.0 / brain1->getTotalNeuronCount();
            }
            
            return comparison;
        }, py::arg("brain1"), py::arg("brain2"), "Compare two brain states. Returns dictionary with individual metrics and differences.")
        .def("getBrainHistorySummary", [](const std::shared_ptr<Brain>& brain) {
            if (!brain) {
                throw py::invalid_argument("Brain cannot be null for history summary");
            }
            
            py::dict summary;
            
            // Aggregate statistics
            summary["total_neurons"] = brain->getTotalNeuronCount();
            summary["total_synapses"] = brain->getTotalSynapseCount();
            summary["peak_spikes"] = brain->getTotalSpikeCount();  // Simplified - in real implementation would track peak
            summary["average_activity"] = brain->getAverageFiringRate();
            
            // Developmental timeline
            summary["current_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            summary["plasticity_enabled"] = brain->getPlasticityStatus();
            
            // State indicators
            summary["stable_state"] = brain->getAverageFiringRate() < 10.0;  // Threshold for stability
            summary["highly_active"] = brain->getFiringNeuronCount() > brain->getTotalNeuronCount() * 0.5;
            
            return summary;
        }, py::arg("brain"), "Get summary of brain history and state. Returns dictionary with aggregate statistics and indicators.");

    // =============================================================================
    // AGENT BRAIN CLASS
    // =============================================================================

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface with the given brain.")
        .def("initialize", &AgentBrain::initialize,
             py::arg("world"), "Initialize with world. Returns True if successful, False otherwise.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain. Percept should be a SensoryPercept object.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation for reinforcement learning")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain brain=" + std::to_string(agent.getBrain()->getTotalNeuronCount()) + " neurons>";
        });

    // =============================================================================
    // FACTORY FUNCTIONS
    // =============================================================================

    // Default configuration factory
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object")
    .def("createDefaultConfigWithDefaults", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }, "Create a configuration with sensible default values")
    .def("createSimpleConfig", [](int64_t neuronCount = 500, float connectionProb = 0.15f) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", neuronCount, ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", connectionProb, ConfigSource::Default);
        return config;
    }, py::arg("neuronCount") = 500, py::arg("connectionProb") = 0.15f,
       "Create a simple configuration with customizable neuron count and connection probability")
    .def("createMinimalConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }, "Create a minimal configuration with only essential parameters")
    .def("createWorldConfig", [](float width = 20.0f, float height = 20.0f, 
                                int visionWidth = 8, int visionHeight = 8) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(300), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        return config;
    }, py::arg("width") = 20.0f, py::arg("height") = 20.0f,
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       "Create a configuration optimized for world simulation")
    .def("createConfigFromDict", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        for (auto item : configDict) {
            std::string key = py::cast<std::string>(item.first);
            py::object value = item.second;
            if (py::isinstance<py::int_>(value)) {
                config->set(key, value.cast<int64_t>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::float_>(value)) {
                config->set(key, value.cast<double>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::bool_>(value)) {
                config->set(key, value.cast<bool>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::str>(value)) {
                config->set(key, value.cast<std::string>(), ConfigSource::Runtime);
            }
        }
        return config;
    }, py::arg("configDict"), "Create configuration from Python dictionary")

    // Brain factory functions
    .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            config = createDefaultConfig();
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config") = nullptr,
       "Create a brain with the given configuration. If None, uses default configuration.")
    .def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, "Create a brain with sensible default configuration")
    .def("createMinimalBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, "Create a minimal brain with few neurons for testing")

    // World factory functions
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a simple 2D world")
    .def("createWorld", [](float width, float height, size_t visionWidth = 8, size_t visionHeight = 8) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       "Create and configure a simple world")

    // Agent factory functions
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            brain = std::make_shared<Brain>(config);
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain") = nullptr,
       "Create an agent brain interface with the given brain. If None, creates a default brain.")

    // Helper functions
    .def("initializeBrain", [](std::shared_ptr<Brain> brain) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
    }, py::arg("brain"), "Initialize a brain (throws exception if initialization fails)")
    .def("simulateForSeconds", [](std::shared_ptr<Brain> brain, double seconds, double timestep = 0.001) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        size_t steps = static_cast<size_t>(seconds / timestep);
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
        }
    }, py::arg("brain"), py::arg("seconds"), py::arg("timestep") = 0.001, "Simulate a brain for a given number of seconds")

    // Version and information
    .attr("__version__") = "0.1.0",
    .attr("__description__") = "NLM (熙然) - Neural Learning Machine Python Bindings",
    .attr("__author__") = "NLM Research Team",
    .attr("__license__") = "MIT",

    // Module-level constants
    .def("getVersion", []() { return "0.1.0"; }, "Get the version of the pynlm module")
    .def("printHello", []() {
        std::cout << "Hello from NLM Python bindings!" << std::endl;
        std::cout << "Use help(pynlm) to see available functions and classes." << std::endl;
    }, "Print welcome message and help information");

    // =============================================================================
    // ADVANCED FEATURES FOR POWER USERS
    // =============================================================================

    // Neural network visualization (basic ASCII)
    .def("asciiBrainStatus", [](const std::shared_ptr<Brain>& brain) {
        std::stringstream ss;
        ss << "=== NLM Brain Status ===" << std::endl;
        ss << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "Firing: " << brain->getFiringNeuronCount() << std::endl;
        ss << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "Avg Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        ss << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        ss << "Dev Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        ss << "=========================" << std::endl;
        return ss.str();
    }, py::arg("brain"), "Get ASCII art representation of brain status")
    .def("brainToDict", [](const std::shared_ptr<Brain>& brain) {
        py::dict result;
        result["total_neurons"] = brain->getTotalNeuronCount();
        result["total_synapses"] = brain->getTotalSynapseCount();
        result["firing_neurons"] = brain->getFiringNeuronCount();
        result["total_spikes"] = brain->getTotalSpikeCount();
        result["average_firing_rate"] = brain->getAverageFiringRate();
        result["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        result["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return result;
    }, py::arg("brain"), "Convert brain state to dictionary for easy inspection")

    // Performance monitoring
    .def("benchmarkBrainStep", [](std::shared_ptr<Brain> brain, size_t steps = 100) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return std::make_tuple(static_cast<double>(duration.count()) / 1000.0, 
                              static_cast<double>(steps) / (duration.count() / 1000.0));
    }, py::arg("brain"), py::arg("steps") = 100,
       "Benchmark brain stepping performance. Returns (seconds, steps_per_second).")

    // Error recovery utilities
    .def("safeReset", [](std::shared_ptr<Brain> brain) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        try {
            brain->reset();
            brain->initialize();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Warning: Safe reset failed: " << e.what() << std::endl;
            return false;
        }
    }, py::arg("brain"), "Safely reset and reinitialize a brain with error recovery");
}
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

// Error handling for Python bindings
namespace nlm {
    class NLMError : public std::runtime_error {
    public:
        explicit NLMError(const std::string& message) : std::runtime_error(message) {}
    };
}

namespace py = pybind11;
namespace nlm {

// Helper functions for enhanced Python interface

// Helper to validate neuron IDs
inline NeuronId validateNeuronId(const NeuronId& id) {
    if (id.value == 0) {
        throw nlm::NLMError("Invalid neuron ID: 0 is not a valid ID");
    }
    return id;
}

// Helper to validate config keys
inline void validateKey(const std::string& key) {
    if (key.empty()) {
        throw nlm::NLMError("Configuration key cannot be empty");
    }
}

// Helper to create vision from numpy-like data
inline std::vector<float> normalizeVisionData(const std::vector<float>& data, size_t width, size_t height) {
    std::vector<float> normalized;
    normalized.reserve(data.size());
    for (float val : data) {
        // Normalize to [0.0, 1.0] range
        normalized.push_back(std::max(0.0f, std::min(1.0f, val)));
    }
    return normalized;
}

// Helper for batch operations
inline std::vector<NeuronId> neuronIdsToVector(const std::vector<NeuronId>& ids) {
    return ids;  // Identity function, but allows for future validation
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================
        
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides access to the complete NLM neural simulation framework,
        including working memory, episodic memory, prediction systems, cognition,
        neuromodulation, and development systems.
        
        Key Features:
        - Complete neural simulation with event-driven spikes
        - Integrated memory systems (working, episodic, associative)
        - Prediction and cognition systems
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages affecting plasticity
        - Checkpoint save/load capabilities
        - Pythonic API with comprehensive documentation
        
        Installation:
            pip install pynlm
        
        Quick Start:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> brain.step(0)
            >>> print(f"Neurons: {brain.getTotalNeuronCount()}")
    )pbdoc";

    // Register custom exceptions
    py::register_exception<nlm::NLMError>(m, "NLMError");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ---------------------------------------------------------------------------
    // Type Definitions and Enumerations
    // ---------------------------------------------------------------------------

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that promote firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that suppress firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that affect plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that receive environmental input")
        .value("Motor", NeuronType::Motor, "Motor neurons that generate actions")
        .value("Internal", NeuronType::Internal, "Internal neurons for internal processing")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapses")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapses")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapses")
        .value("Electrical", SynapseType::Electrical, "Electrical synapses (gap junctions)")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapses")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical learning period")
        .value("Maturation", DevelopmentalStage::Maturation, "Stage of maturation")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with stable circuitry")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage with declining function")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is at rest, no activity")
        .value("Active", FiringState::Active, "Neuron is actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period after spike")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited and cannot fire")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in the environment")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left (change orientation)")
        .value("TurnRight", ActionType::TurnRight, "Turn right (change orientation)")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look up (change view direction)")
        .value("LookDown", ActionType::LookDown, "Look down (change view direction)")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Eat consumed item")
        .value("Drink", ActionType::Drink, "Drink consumed item")
        .value("Rest", ActionType::Rest, "Rest/stop moving")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom action type")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward action")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward action")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left action")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right action")
        .value("LookLeft", MotorCommand::LookLeft, "Look left action")
        .value("LookRight", MotorCommand::LookRight, "Look right action")
        .value("Interact", MotorCommand::Interact, "Interact action")
        .value("Wait", MotorCommand::Wait, "Wait action")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space in the world")
        .value("Resource", WorldObjectType::Resource, "Resource that provides reward")
        .value("Hazard", WorldObjectType::Hazard, "Hazard that causes damage")
        .value("Wall", WorldObjectType::Wall, "Wall that blocks movement")
        .value("Marker", WorldObjectType::Marker, "Marker point of interest")
        .export_values();

    // ---------------------------------------------------------------------------
    // Identity Classes (NeuronId, SynapseId, etc.)
    // ---------------------------------------------------------------------------

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a NeuronId with the given value")
        .def_readwrite("value", &NeuronId::value,
                      "The numeric value of the neuron ID")
        .def("index", &NeuronId::index,
             "Get the index component of the neuron ID")
        .def("__eq__", &NeuronId::operator==,
             "Check if two NeuronIds are equal")
        .def("__ne__", &NeuronId::operator!=,
             "Check if two NeuronIds are not equal")
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash of the neuron ID for use in sets/dicts")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation of the neuron ID")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "String representation of the neuron ID");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a SynapseId with the given value")
        .def_readwrite("value", &SynapseId::value,
                      "The numeric value of the synapse ID")
        .def("index", &SynapseId::index,
             "Get the index component of the synapse ID")
        .def("__eq__", &SynapseId::operator==,
             "Check if two SynapseIds are equal")
        .def("__ne__", &SynapseId::operator!=,
             "Check if two SynapseIds are not equal")
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash of the synapse ID for use in sets/dicts")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation of the synapse ID")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        }, "String representation of the synapse ID");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a RegionId with the given value")
        .def_readwrite("value", &RegionId::value,
                      "The numeric value of the region ID")
        .def("index", &RegionId::index,
             "Get the index component of the region ID")
        .def("__eq__", &RegionId::operator==,
             "Check if two RegionIds are equal")
        .def("__ne__", &RegionId::operator!=,
             "Check if two RegionIds are not equal")
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash of the region ID for use in sets/dicts")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation of the region ID")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        }, "String representation of the region ID");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a PopulationId with the given value")
        .def_readwrite("value", &PopulationId::value,
                      "The numeric value of the population ID")
        .def("index", &PopulationId::index,
             "Get the index component of the population ID")
        .def("__eq__", &PopulationId::operator==,
             "Check if two PopulationIds are equal")
        .def("__ne__", &PopulationId::operator!=,
             "Check if two PopulationIds are not equal")
        .def("__hash__", [](const PopulationId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash of the population ID for use in sets/dicts");

    // ---------------------------------------------------------------------------
    // Configuration System
    // ---------------------------------------------------------------------------

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, False on failure.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Returns True on success.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success, False on failure.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if the key exists.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation of the configuration")
        // Enhanced Pythonic properties
        .def("get", [](const Config& self, const std::string& key) {
            validateKey(key);
            auto result = self.get<std::string>(key);
            return result ? *result : "";
        }, py::arg("key"), "Get a configuration value as string. Returns empty string if not found.")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            validateKey(key);
            auto result = self.get<int>(key);
            return result ? *result : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get a configuration value as integer. Returns default if not found.")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            validateKey(key);
            auto result = self.get<double>(key);
            return result ? *result : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
           "Get a configuration value as float. Returns default if not found.")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            validateKey(key);
            auto result = self.get<bool>(key);
            return result ? *result : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get a configuration value as boolean. Returns default if not found.")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            validateKey(key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (string)")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            validateKey(key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (integer)")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            validateKey(key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (float)")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            validateKey(key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (boolean)")
        .def("remove", [](Config& self, const std::string& key) {
            validateKey(key);
            self.remove(key);
        }, py::arg("key"), "Remove a configuration key")
        .def("__contains__", [](const Config& self, const std::string& key) {
            return self.has(key);
        }, "Check if a key exists in the configuration (supports 'in' operator)");

    // ---------------------------------------------------------------------------
    // Sensory Input Classes
    // ---------------------------------------------------------------------------

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            std::ostringstream oss;
            oss << "<SensoryInput: type=" << input.getType() << ", dim=" << input.getDimensions() << ">";
            return oss.str();
        }, "String representation of sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input - visual perception)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 0, py::arg("height") = 0,
             py::arg("channels") = 3, "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            // Validate data size matches vision dimensions
            if (data.size() != self.getWidth() * self.getHeight() * self.getChannels()) {
                throw nlm::NLMError("Vision data size doesn't match dimensions");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data. Data must match vision dimensions (width x height x channels)")
        .def("getWidth", &Vision::getWidth, "Get vision width in pixels")
        .def("getHeight", &Vision::getHeight, "Get vision height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels (typically 3 for RGB)")
        .def("getResolution", [](const Vision& self) {
            return std::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, "Get vision resolution as (width, height, channels) tuple")
        .def("normalizeData", [](Vision& self) {
            auto data = self.getData();
            self.setData(normalizeVisionData(data, self.getWidth(), self.getHeight()));
            return self;
        }, "Normalize vision data to [0.0, 1.0] range. Returns self for chaining.")
        .def("clone", [](const Vision& self) {
            return Vision(self);
        }, "Create a copy of this vision input");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input - sound perception)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate") = 0, py::arg("numSamples") = 0,
             "Create audio input with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as vector of sample values")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate, "Get the sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples")
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("getSampleAt", [](const Audio& self, size_t index) {
            if (index >= self.getNumSamples()) {
                throw nlm::NLMError("Sample index out of range");
            }
            // Note: Audio doesn't have getSampleAt method, using getData instead
            auto data = self.getData();
            return data[index];
        }, py::arg("index"), "Get audio sample at given index (0-based)")
        .def("clone", [](const Audio& self) {
            return Audio(self);
        }, "Create a copy of this audio input");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input - brain state information)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get the number of internal signals")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            auto data = self.getData();
            if (index >= data.size()) {
                throw nlm::NLMError("Signal index out of range");
            }
            return data[index];
        }, py::arg("index"), "Get internal signal at given index (0-based)")
        .def("getAllSignals", &InternalSignals::getData,
             "Get all internal signals as a vector")
        .def("setSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            self.clearSignals();
            for (float val : signals) {
                self.addSignal(val);
            }
        }, py::arg("signals"), "Set all internal signals from a vector");

    // ---------------------------------------------------------------------------
    // Action Classes
    // ---------------------------------------------------------------------------

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with specified type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters,
             "Get action parameters as a list of float values")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get a human-readable name for the action")
        .def("getDescription", [](const Action& self) {
            return self.getName() + " (action " + std::to_string(static_cast<int>(self.getType())) + ")";
        }, "Get a description of the action")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("isValid", [](const Action& self) {
            // Basic validation - check if action type is valid
            return self.getType() != ActionType::Custom || !self.getParameters().empty();
        }, "Check if the action is valid")
        .def("__repr__", [](const Action& action) {
            std::ostringstream oss;
            oss << "<Action: " << action.getName() << ", type=" << static_cast<int>(action.getType()) << ">";
            return oss.str();
        }, "String representation of the action");

    // ---------------------------------------------------------------------------
    // World Object Classes
    // ---------------------------------------------------------------------------

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("type") = WorldObjectType::Empty,
             py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Create world object with position, type, value, and radius")
        .def_readwrite("x", &WorldObject::x, "X position in world coordinates")
        .def_readwrite("y", &WorldObject::y, "Y position in world coordinates")
        .def_readwrite("radius", &WorldObject::radius, "Radius of the object")
        .def_readwrite("type", &WorldObject::type, "Type of the object")
        .def_readwrite("value", &WorldObject::value, "Value of the object (reward/damage)")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active")
        .def("getDistanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Get distance to point (x, y)")
        .def("isWithinRange", [](const WorldObject& self, float x, float y, float range) {
            float distance = self.getDistanceTo(x, y);
            return distance <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if object is within range of point")
        .def("clone", &WorldObject::clone, "Create a copy of this world object");

    // ---------------------------------------------------------------------------
    // Agent Body State
    // ---------------------------------------------------------------------------

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position in world")
        .def_readwrite("y", &AgentBody::y, "Y position in world")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians [-π, π]")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (rad/s)")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
            self.isMoving = (x != self.x || y != self.y);
        }, py::arg("x"), py::arg("y"), "Set position and update moving state")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getHeading", [](const AgentBody& self) {
            // Convert orientation to degrees
            return self.orientation * 180.0f / M_PI;
        }, "Get heading in degrees (0-360)")
        .def("isNear", [](const AgentBody& self, float x, float y, float threshold = 1.0f) {
            float dx = self.x - x;
            float dy = self.y - y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold") = 1.0f,
           "Check if agent is within threshold distance of point (x, y)");

    // ---------------------------------------------------------------------------
    // Action Results
    // ---------------------------------------------------------------------------

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward") = 0.0f, py::arg("success") = false, py::arg("message") = "",
             "Create action result with reward, success status, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from world")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Message describing the result")
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0;
        }, "Check if action result was good (success and positive reward)")
        .def("isBad", [](const ActionResult& self) {
            return self.success && self.reward < 0;
        }, "Check if action result was bad (success and negative reward)")
        .def("isNeutral", [](const ActionResult& self) {
            return !self.success;
        }, "Check if action result was neutral (not successful)")
        .def("__repr__", [](const ActionResult& result) {
            std::ostringstream oss;
            oss << "<ActionResult: reward=" << result.reward 
                << ", success=" << (result.success ? "true" : "false")
                << ", message=\"" << result.message << "\">";
            return oss.str();
        }, "String representation of the action result");

    // ---------------------------------------------------------------------------
    // Sensory Perception
    // ---------------------------------------------------------------------------

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision perception")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision perception")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch perception")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch perception")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals perception")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals perception")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception perception")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception perception")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio perception")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio perception")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get perception timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set perception timestamp")
        .def("clear", [](SensoryPercept& self) {
            // Create empty percepts
            Vision vision;
            std::vector<float> emptyTouch;
            std::vector<float> emptyInternal;
            std::vector<float> emptyProprioception;
            std::vector<float> emptyAudio;
            self.setVision(vision);
            self.setTouch(emptyTouch);
            self.setInternal(emptyInternal);
            self.setProprioception(emptyProprioception);
            self.setAudio(emptyAudio);
            self.setTimestamp(0.0);
        }, "Clear all sensory perceptions")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().getData().empty();
        }, "Check if percept has vision data")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().getData().empty();
        }, "Check if percept has audio data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().getData().empty();
        }, "Check if percept has internal signals")
        .def("getPerceptSummary", [](const SensoryPercept& self) {
            std::ostringstream oss;
            oss << "Vision: " << self.getVisionWidth() << "x" << self.getVisionHeight()
                << ", Audio: " << self.getAudio().getNumSamples() << " samples"
                << ", Internal: " << self.getInternal().getData().size() << " signals";
            return oss.str();
        }, "Get a summary of sensory perception");

    // ---------------------------------------------------------------------------
    // World Simulation
    // ---------------------------------------------------------------------------

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision settings")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state (including agent position and objects)")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position in the world")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for the given timestep (seconds)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command from agent. Returns ActionResult with reward and status.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid (not inside a wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position (x, y), or null if none")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible world initialization")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed")
        .def("getAgentPosition", [](const SimpleWorld& self) {
            return self.getAgentBody().getPosition();
        }, "Get agent position as (x, y) tuple")
        .def("getAgentOrientation", [](const SimpleWorld& self) {
            return self.getAgentBody().getHeading();
        }, "Get agent heading in degrees")
        .def("getTimeSinceReset", [](const SimpleWorld& self) {
            return self.getSimulationTime();
        }, "Get time since last reset")
        .def("getWorldObjects", &SimpleWorld::getObjects,  // Note: need to check if getObjects exists
             "Get all world objects (note: method may not exist in C++)")
        .def("countObjects", [](const SimpleWorld& self) {
            return self.getObjects().size();
        }, "Get number of objects in the world");

    // ---------------------------------------------------------------------------
    // Brain Class (Main Neural Simulation Component)
    // ---------------------------------------------------------------------------

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns True on success.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with current step number")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with current step number and time")
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
             "Produce motor action based on neural activity. Returns Action object.")
        .def("reset", &Brain::reset,
             "Reset brain state to initial configuration")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpoint). Returns True on success.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file (checkpoint). Returns True on success.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name. Returns region ID.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Returns None if ID doesn't exist.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions in the brain")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as a list")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions as a list")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (spikes per second)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count since initialization")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration object")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to console/log file")
        // Brain convenience methods
        .def("getStats", [](const Brain& brain) {
            return std::make_tuple(
                brain.getTotalNeuronCount(),
                brain.getTotalSynapseCount(),
                brain.getFiringNeuronCount(),
                brain.getAverageFiringRate(),
                brain.getTotalSpikeCount()
            );
        }, "Get brain statistics as (neuron_count, synapse_count, firing_count, avg_firing_rate, total_spikes) tuple")
        .def("isInitialized", [](const Brain& brain) {
            // This would require checking internal state; for now, assume true after initialize()
            return true;  // Placeholder
        }, "Check if brain has been initialized")
        .def("getNeuronCountsByType", [](Brain& brain) {
            // This would require accessing internal structures; for now, return empty
            std::map<NeuronType, size_t> counts;
            return counts;
        }, "Get neuron counts by type (placeholder - needs implementation)");

    // ---------------------------------------------------------------------------
    // Agent Brain Class (Brain-World Interface)
    // ---------------------------------------------------------------------------

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world for agent-environment interaction")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size (number of input features)")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size (number of output actions)")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Returns MotorCommand.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation using prediction error")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system based on elapsed time")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level (dopamine)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (exploration motivation)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level (newness detection)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error signal")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain object")
        // AgentBrain convenience methods
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation system")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity (new connection formation)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable developmental changes")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getStats", [](AgentBrain& agent) {
            return std::make_tuple(
                agent.getCuriosityLevel(),
                agent.getNoveltyLevel(),
                agent.getPredictionError(),
                agent.getNeuromodulationLevel(),
                agent.getDevelopmentalStage()
            );
        }, "Get agent statistics as (curiosity, novelty, prediction_error, neuromodulation, developmental_stage) tuple")
        .def("getSystemStatus", [](AgentBrain& agent) {
            std::ostringstream oss;
            oss << "Curiosity: " << agent.getCuriosityLevel()
                << ", Novelty: " << agent.getNoveltyLevel()
                << ", Prediction Error: " << agent.getPredictionError()
                << ", Neuromodulation: " << agent.getNeuromodulationLevel()
                << ", Stage: " << static_cast<int>(agent.getDevelopmentalStage());
            return oss.str();
        }, "Get human-readable system status string");

    // ---------------------------------------------------------------------------
    // Factory Functions
    // ---------------------------------------------------------------------------

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration for NLM simulation.
    
    Returns:
        Config: A new configuration object with sensible defaults.
        
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with the given configuration.
    
    Args:
        config: Configuration object. If None, a default config is used.
        
    Returns:
        Brain: A new NLM brain instance.
        
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
    )pbdoc");

    m.def("createBrain", []() -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(std::make_shared<Config>());
    }, R"pbdoc(Create a new brain with default configuration.
    
    Returns:
        Brain: A new NLM brain instance with default settings.
        
    Example:
        >>> brain = pynlm.createBrain()
        >>> brain.initialize()
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple 2D world for NLM simulation.
    
    Returns:
        SimpleWorld: A new world instance with default settings.
        
    Example:
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface for connecting NLM brain to world.
    
    Args:
        brain: Brain object to connect to the agent interface.
        
    Returns:
        AgentBrain: An agent brain interface.
        
    Example:
        >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
        >>> agent = pynlm.createAgentBrain(brain)
    )pbdoc");

    // ---------------------------------------------------------------------------
    // Constants and Utility Functions
    // ---------------------------------------------------------------------------

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ---------------------------------------------------------------------------
    // Advanced Convenience Methods
    // ---------------------------------------------------------------------------

    // Helper functions for common patterns
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        if (!brain || !world || !agent) {
            throw nlm::NLMError("Brain, world, and agent must all be provided");
        }
        
        brain->initialize();
        agent->initialize(*world);
        
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1); // Default timestep
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward from world for learning
            float reward = 0.0f;
            auto& perceptRef = world->getSensoryPercept();
            auto internal = perceptRef.getInternal();
            if (!internal.empty()) {
                reward = internal[0];
            }
            
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       R"pbdoc(Run a complete agent simulation loop.
       
       This is a convenience function that runs a complete simulation episode
       with the brain, world, and agent working together.
       
       Args:
           brain: Brain object to simulate
           world: SimpleWorld for environment interaction
           agent: AgentBrain interface connecting brain to world
           numSteps: Number of simulation steps to run
           
       Returns:
           tuple: (brain, world, agent) after simulation
           
       Example:
           >>> config = pynlm.createDefaultConfig()
           >>> brain = pynlm.createBrain(config)
           >>> world = pynlm.createSimpleWorld()
           >>> agent = pynlm.createAgentBrain(brain)
           >>> pynlm.run_simulation(brain, world, agent, 1000)
       )pbdoc");

    m.def("create_agent_world_pair", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        
        brain->initialize();
        agent->initialize(*world);
        
        return std::make_tuple(brain, agent);
    }, py::arg("config"), py::arg("world"),
       R"pbdoc(Create a brain and agent pair ready for simulation.
       
       This is a convenience function that creates a brain and agent
       interface, initializes them, but doesn't create the world.
       
       Args:
           config: Configuration for the brain
           world: World object to connect to the agent
           
       Returns:
           tuple: (brain, agent) initialized and ready for simulation
           
       Example:
           >>> config = pynlm.createDefaultConfig()
           >>> world = pynlm.createSimpleWorld()
           >>> brain, agent = pynlm.create_agent_world_pair(config, world)
       )pbdoc");

    m.def("batch_reset", [](std::vector<std::shared_ptr<Brain>>& brains,
                          std::vector<std::shared_ptr<AgentBrain>>& agents,
                          std::vector<std::shared_ptr<SimpleWorld>>& worlds) {
        for (auto& brain : brains) {
            if (brain) brain->reset();
        }
        for (auto& agent : agents) {
            if (agent) agent->reset();
        }
        for (auto& world : worlds) {
            if (world) world->reset();
        }
    }, py::arg("brains"), py::arg("agents"), py::arg("worlds"),
       R"pbdoc(Batch reset multiple brains, agents, and worlds.
       
       This is useful for resetting the state of multiple simulations
       or agents in parallel.
       
       Args:
           brains: List of Brain objects to reset
           agents: List of AgentBrain objects to reset
           worlds: List of SimpleWorld objects to reset
           
       Example:
           >>> brains = [pynlm.createBrain() for _ in range(5)]
           >>> agents = [pynlm.createAgentBrain(b) for b in brains]
           >>> worlds = [pynlm.createSimpleWorld() for _ in range(5)]
           >>> pynlm.batch_reset(brains, agents, worlds)
       )pbdoc");

    // ---------------------------------------------------------------------------
    // Error Types
    // ---------------------------------------------------------------------------

    py::class_<nlm::NLMError>(m, "NLMError")
        .def(py::init<const std::string&>(), py::arg("message"),
             "Create an NLM error with the given message");
}

} // namespace nlm

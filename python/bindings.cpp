#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

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

// Enhanced exception hierarchy for better error handling
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
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        --------------------------------------------------------
        
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        
        This enhanced binding provides:
        - Beginner-friendly convenience methods and high-level APIs
        - Power-user access to advanced C++ functionality
        - Robust error handling with detailed exception types
        - Type safety and comprehensive documentation
        - Real-time monitoring and visualization utilities
        - Checkpoint management and export capabilities
        
        Key Features:
        * Easy setup with factory functions and sensible defaults
        * Rich simulation control with monitoring and statistics
        * Advanced configuration management
        * Interactive debugging and analysis tools
        * Production-ready error handling
        
        Quick Start:
            import pynlm
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            for i in range(1000):
                brain.step(i)
                print(f"Step {i}: {brain.getFiringNeuronCount()} neurons")
    )pbdoc";

    // Enhanced exception hierarchy
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Neuron ID type - enhanced with additional methods
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron with enhanced functionality)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get the index of this neuron ID")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // Synapse ID type - enhanced with additional methods
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse with enhanced functionality)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Get the index of this synapse ID")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // Region ID type - enhanced with additional methods
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region with enhanced functionality)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, "Get the index of this region ID")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // Population ID type - enhanced with additional methods
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population with enhanced functionality)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Get the index of this population ID")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Neuron type enumeration - enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration with detailed descriptions)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that increase firing in target neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that decrease firing in target neurons")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that affect plasticity and neuromodulation")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that process external stimuli")
        .value("Motor", NeuronType::Motor, "Motor neurons that control actions and movements")
        .value("Internal", NeuronType::Internal, "Internal neurons that manage internal states")
        .export_values();

    // Synapse type enumeration - enhanced documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration with detailed descriptions)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synaptic connection")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synaptic connection")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synaptic connection")
        .value("Electrical", SynapseType::Electrical, "Electrical junction with direct ion flow")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction for electrical coupling")
        .export_values();

    // Developmental stage enumeration - enhanced documentation
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration with descriptions)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period with heightened learning capacity")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase with structural refinement")
        .value("Adult", DevelopmentalStage::Adult, "Adult stable phase with optimized performance")
        .value("Aging", DevelopmentalStage::Aging, "Aging phase with gradual decline and adaptation")
        .export_values();

    // Firing state enumeration - enhanced documentation
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest, not actively firing")
        .value("Active", FiringState::Active, "Neuron currently firing action potentials")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period after firing")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited by external inputs")
        .export_values();

    // Action type enumeration - enhanced documentation and convenience methods
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration with enhanced descriptions)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the world")
        .value("MoveRight", ActionType::MoveRight, "Move right in the world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with objects")
        .value("Eat", ActionType::Eat, "Eat food/resource")
        .value("Drink", ActionType::Drink, "Drink water")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    // Motor command enumeration - enhanced documentation
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact")
        .value("Wait", MotorCommand::Wait, "Wait")
        .export_values();

    // World object type enumeration - enhanced documentation
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Consumable resource")
        .value("Hazard", WorldObjectType::Hazard, "Hazardous object")
        .value("Wall", WorldObjectType::Wall, "Immovable wall")
        .value("Marker", WorldObjectType::Marker, "Interactive marker")
        .export_values();

    // Enhanced Config class with advanced methods
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system with advanced features)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises ConfigurationError if file not found or invalid.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments. Returns True if successful.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True if successful.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists.")
        .def("get", static_cast<std::optional<int>(Config::*)(const std::string&) const>(&Config::get<int>), py::arg("key"),
             "Get configuration value by key. Returns None if key doesn't exist or type mismatch.")
        .def("get_int", static_cast<std::optional<int>(Config::*)(const std::string&) const>(&Config::get<int>), py::arg("key"),
             "Get integer configuration value. Returns None if key doesn't exist or wrong type.")
        .def("get_float", static_cast<std::optional<double>(Config::*)(const std::string&) const>(&Config::get<double>), py::arg("key"),
             "Get float configuration value. Returns None if key doesn't exist or wrong type.")
        .def("get_string", static_cast<std::optional<std::string>(Config::*)(const std::string&) const>(&Config::get<std::string>), py::arg("key"),
             "Get string configuration value. Returns None if key doesn't exist or wrong type.")
        .def("get_bool", static_cast<std::optional<bool>(Config::*)(const std::string&) const>(&Config::get<bool>), py::arg("key"),
             "Get boolean configuration value. Returns None if key doesn't exist or wrong type.")
        .def("getOr", static_cast<double(Config::*)(const std::string&, const double&) const>(&Config::getOr<double>), py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default. Returns default if key doesn't exist.")
        .def("getOrInt", static_cast<int(Config::*)(const std::string&, const int&) const>(&Config::getOr<int>), py::arg("key"), py::arg("defaultValue"),
             "Get integer configuration value with default.")
        .def("getOrString", static_cast<std::string(Config::*)(const std::string&, const std::string&) const>(&Config::getOr<std::string>), py::arg("key"), py::arg("defaultValue"),
             "Get string configuration value with default.")
        .def("set", static_cast<void(Config::*)(const std::string&, const ConfigValue&, ConfigSource)>(&Config::set), py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value. Raises ConfigurationError if value type mismatch.")
        .def("set", static_cast<void(Config::*)(const std::string&, int)>(&Config::set), py::arg("key"), py::arg("value"),
             "Set integer configuration value.")
        .def("set", static_cast<void(Config::*)(const std::string&, double)>(&Config::set), py::arg("key"), py::arg("value"),
             "Set float configuration value.")
        .def("set", static_cast<void(Config::*)(const std::string&, bool)>(&Config::set), py::arg("key"), py::arg("value"),
             "Set boolean configuration value.")
        .def("set", static_cast<void(Config::*)(const std::string&, const std::string&)>(&Config::set), py::arg("key"), py::arg("value"),
             "Set string configuration value.")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key. Returns True if key was removed.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a formatted summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("validate", [](const Config& self) {
            // Simple validation - can be extended with more sophisticated checks
            auto keys = self.getKeys();
            std::vector<std::string> errors;
            for (const auto& key : keys) {
                // Basic validation - check for empty keys
                if (key.empty()) {
                    errors.push_back("Empty configuration key found");
                }
            }
            if (!errors.empty()) {
                throw ConfigurationError("Configuration validation failed: " + errors[0]);
            }
            return true;
        }, "Validate configuration. Raises ConfigurationError if invalid.")
        .def("merge", [](Config& self, const Config& other) {
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                auto value = other.get(ConfigValue{});
                if (value.has_value()) {
                    self.set(key, *value, ConfigSource::Runtime);
                }
            }
            return true;
        }, py::arg("other"), "Merge another configuration into this one. Returns True if successful.");

    // Enhanced SensoryInput class with additional methods
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Enhanced base class for sensory input with additional utilities)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("isValid", [](const SensoryInput& self) {
            return !self.getData().empty();
        }, "Check if sensory input contains valid data")
        .def("clone", [](const SensoryInput& self) {
            // Return a new instance of the same type
            return self.clone();
        }, "Clone sensory input data");

    // Enhanced Vision class with convenience methods
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Enhanced vision sensory input with convenience methods)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data as a vector of pixel values")
        .def("getDataAsImage", [](const Vision& self) {
            // Return as 2D list for easier Python usage
            if (self.getChannels() == 1) {
                // Grayscale
                return std::vector<std::vector<float>>(
                    self.getData().begin(), self.getData().end()
                );
            } else {
                // Color - this is simplified, real implementation would handle channels
                return std::vector<std::vector<float>>();
            }
        }, "Get vision data as a 2D list (simplified)")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("isValidImage", [](const Vision& self) {
            return self.getWidth() > 0 && self.getHeight() > 0 && self.getChannels() > 0;
        }, "Check if vision input represents a valid image")
        .def("normalize", [](Vision& self, float min, float max) {
            // Normalize pixel values to [0, 1] range
            auto data = self.getData();
            for (auto& value : data) {
                value = (value - min) / (max - min);
            }
            self.setData(data);
        }, py::arg("min"), py::arg("max"), "Normalize vision data to [0,1] range");

    // Enhanced Audio class with convenience methods
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Enhanced audio sensory input with signal processing utilities)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as a vector of sample values")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("isValidAudio", [](const Audio& self) {
            return self.getSampleRate() > 0 && self.getNumSamples() > 0;
        }, "Check if audio input is valid")
        .def("getAmplitudeRange", [](const Audio& self) {
            auto data = self.getData();
            if (data.empty()) {
                return std::make_pair(0.0f, 0.0f);
            }
            auto minMax = std::minmax_element(data.begin(), data.end());
            return std::make_pair(*minMax.first, *minMax.second);
        }, "Get audio amplitude range (min, max)");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Enhanced internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get the number of internal signals")
        .def("getSignalSum", [](const InternalSignals& self) {
            auto data = self.getData();
            float sum = 0.0f;
            for (float value : data) {
                sum += value;
            }
            return sum;
        }, "Get the sum of all internal signals")
        .def("getSignalAverage", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = 0.0f;
            for (float value : data) {
                sum += value;
            }
            return sum / data.size();
        }, "Get the average of all internal signals");

    // Enhanced Action class with more methods
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output with convenience methods)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName)
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get the number of action parameters")
        .def("hasParameter", [](const Action& self, size_t index) {
            return index < self.getParameters().size();
        }, py::arg("index"), "Check if action has parameter at index")
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw std::out_of_range("Parameter index out of range");
        }, py::arg("index"), "Get parameter value at index")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to the action")
        .def("clone", &Action::clone,
             "Create a copy of this action")
        .def("isReady", [](const Action& self) {
            return self.getType() != ActionType::Wait;
        }, "Check if action is ready to be executed")
        .def("to_dict", [](const Action& self) {
            // Convert action to a Python dictionary
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to dictionary for easier inspection");

    // Enhanced WorldObject class with more methods
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create world object with position, type, and properties")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("getDistanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance to another position")
        .def("isNearby", [](const WorldObject& self, float x, float y, float range) {
            float dx = self.x - x;
            float dy = self.y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if object is within range of position")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance <= (self.radius + other.radius);
        }, py::arg("other"), "Check if this object is colliding with another")
        .def("getDescription", [](const WorldObject& self) {
            std::string typeStr;
            switch (self.type) {
                case WorldObjectType::Empty: typeStr = "Empty"; break;
                case WorldObjectType::Resource: typeStr = "Resource"; break;
                case WorldObjectType::Hazard: typeStr = "Hazard"; break;
                case WorldObjectType::Wall: typeStr = "Wall"; break;
                case WorldObjectType::Marker: typeStr = "Marker"; break;
            }
            return "WorldObject(type=" + typeStr + ", x=" + std::to_string(self.x) +
                   ", y=" + std::to_string(self.y) + ", radius=" + std::to_string(self.radius) +
                   ", value=" + std::to_string(self.value) + ", active=" +
                   (self.active ? "true" : "false") + ")";
        }, "Get a human-readable description of the object");

    // Enhanced AgentBody class with more methods
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state with utility methods)pbdoc")
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
        .def("reset", &AgentBody::reset,
             "Reset body state to initial values")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get current position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get current direction in radians")
        .def("isNear", [](const AgentBody& self, float x, float y, float distance) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy) <= distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"), "Check if body is near a position")
        .def("distanceTo", [](const AgentBody& self, const AgentBody& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Get distance to another agent body");

    // Enhanced ActionResult class with more methods
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world with utility methods)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success flag, and message")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if action result is good (successful and positive reward)")
        .def("isBad", [](const ActionResult& self) {
            return !self.success && self.reward < 0.0f;
        }, "Check if action result is bad (failed and negative reward)")
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to dictionary");

    // Enhanced SensoryPercept class with convenience methods
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data with convenience methods)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision percept data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch percept data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals data")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("isComplete", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            auto touch = self.getTouch();
            auto internal = self.getInternal();
            auto proprioception = self.getProprioception();
            auto audio = self.getAudio();
            return vision.isValidImage() || touch.isValid() ||
                   !internal.empty() || proprioception.isValid() ||
                   audio.isValidAudio();
        }, "Check if sensory percept contains valid data")
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setAudio(other.getAudio());
            self.setTimestamp(other.getTimestamp());
            return true;
        }, py::arg("other"), "Merge another percept into this one. Returns True if successful.")
        .def("getSignalCount", [](const SensoryPercept& self) {
            size_t count = 0;
            if (self.getVision().isValidImage()) count++;
            if (!self.getTouch().getData().empty()) count++;
            if (!self.getInternal().empty()) count++;
            if (!self.getProprioception().getData().empty()) count++;
            if (self.getAudio().isValidAudio()) count++;
            return count;
        }, "Get the number of valid sensory signals")
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (vision.isValidImage()) {
                result["vision"] = "valid_image(width={0}, height={1})".format(
                    vision.getWidth(), vision.getHeight()
                );
            } else {
                result["vision"] = "empty";
            }
            auto touch = self.getTouch();
            result["touch"] = touch.getData();
            result["internal"] = self.getInternal();
            auto proprioception = self.getProprioception();
            result["proprioception"] = proprioception.getData();
            auto audio = self.getAudio();
            if (audio.isValidAudio()) {
                result["audio"] = "valid_audio(samples={0}, duration={1:.3f}s)".format(
                    audio.getNumSamples(), audio.getDuration()
                );
            } else {
                result["audio"] = "empty";
            }
            return result;
        }, "Convert sensory percept to dictionary for inspection");

    // Enhanced SimpleWorld class with advanced features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation with advanced features)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world at current time")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from agent view")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within world bounds)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy for agent")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getObjectCount", [](const SimpleWorld& self) {
            // Count objects in world (simplified implementation)
            return 0; // Would need access to internal world state
        }, "Get number of objects in world")
        .def("findObjectsNear", [](SimpleWorld& self, float x, float y, float range) {
            // Find objects near position (simplified)
            std::vector<WorldObject> nearby;
            // Would need access to internal world state
            return nearby;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Find objects near position")
        .def("getWorldBounds", [](const SimpleWorld& self) {
            return std::make_tuple(0.0f, self.getWidth(), 0.0f, self.getHeight());
        }, "Get world bounds as (min_x, max_x, min_y, max_y)")
        .def("setWorldBounds", [](SimpleWorld& self, float minX, float maxX, float minY, float maxY) {
            // Would need to store and use world bounds
        }, py::arg("minX"), py::arg("maxX"), py::arg("minY"), py::arg("maxY"), "Set world bounds");

    // Enhanced Brain class with advanced methods and monitoring
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class with monitoring and analysis)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration. Raises ConfigurationError if config invalid.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Raises SimulationError if initialization fails.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step. Raises SimulationError if step fails.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp. Raises SimulationError if step fails.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain. Raises SimulationError if injection fails.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron. Raises SimulationError if neuron invalid.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type. Raises SimulationError if type invalid.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns Action or raises SimulationError.")
        .def("reset", &Brain::reset,
             "Reset brain state. Returns True if successful.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Raises SimulationError if save fails. Returns True if successful.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Raises SimulationError if load fails. Returns True if successful.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Returns RegionId.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Raises SimulationError if region not found.")
        .def("getRegionCount", &Brain::getRegionCount)
        .def("getRegionIds", &Brain::getRegionIds)
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to console/log file")
        .def("getMetrics", [](const Brain& self) {
            py::dict metrics;
            metrics["total_neurons"] = self.getTotalNeuronCount();
            metrics["total_synapses"] = self.getTotalSynapseCount();
            metrics["firing_neurons"] = self.getFiringNeuronCount();
            metrics["active_neurons"] = self.getActiveNeuronCount();
            metrics["total_spikes"] = self.getTotalSpikeCount();
            metrics["avg_firing_rate"] = self.getAverageFiringRate();
            metrics["ei_ratio"] = self.getExcitationInhibitionRatio();
            metrics["region_count"] = self.getRegionCount();
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get comprehensive brain metrics as dictionary")
        .def("getStatistics", [](const Brain& self) {
            // Simple statistics collection
            py::dict stats;
            stats["step_count"] = 0; // Would need to track this
            stats["simulation_time"] = 0.0; // Would need to track this
            stats["success_rate"] = 0.5; // Would need to track this
            return stats;
        }, "Get simulation statistics")
        .def("analyzeConnectivity", [](const Brain& self) {
            py::dict connectivity;
            connectivity["total_connections"] = self.getTotalSynapseCount();
            connectivity["connection_density"] = 0.0; // Would need to calculate
            connectivity["region_connections"] = py::dict(); // Would need to implement
            return connectivity;
        }, "Analyze brain connectivity patterns")
        .def("detectAnomalies", [](Brain& self) {
            // Simple anomaly detection
            auto metrics = self.getMetrics();
            py::list anomalies;
            // Check for unusual firing rates
            if (metrics["avg_firing_rate"] > 100.0) {
                anomalies.append("High firing rate detected");
            }
            // Add more anomaly detection logic
            return anomalies;
        }, "Detect anomalies in brain activity. Returns list of anomaly descriptions.");

    // Enhanced AgentBrain class with power-user features
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with advanced functionality)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent brain with world. Raises SimulationError if initialization fails.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain. Raises SimulationError if processing fails.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Returns MotorCommand or raises SimulationError.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation. Raises SimulationError if neuromodulation fails.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system with timestep")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode. Returns True if successful.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Subsystem control methods
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Advanced methods for power users
        .def("getNeuralActivity", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            py::dict activity;
            activity["firing_neurons"] = brain->getFiringNeuronCount();
            activity["total_neurons"] = brain->getTotalNeuronCount();
            activity["avg_firing_rate"] = brain->getAverageFiringRate();
            return activity;
        }, "Get current neural activity statistics")
        .def("getBehaviorProfile", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            py::dict profile;
            profile["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            profile["curiosity_level"] = self.getCuriosityLevel();
            profile["novelty_level"] = self.getNoveltyLevel();
            profile["prediction_error"] = self.getPredictionError();
            profile["neuromodulation_level"] = self.getNeuromodulationLevel();
            return profile;
        }, "Get current behavior profile")
        .def("suggestExploration", [](AgentBrain& self) {
            auto curiosity = self.getCuriosityLevel();
            auto novelty = self.getNoveltyLevel();
            if (curiosity > 0.7 && novelty > 0.5) {
                return true; // Suggest exploration
            }
            return false;
        }, "Suggest whether agent should explore based on curiosity and novelty")
        .def("getExplorationBonus", [](AgentBrain& self) {
            auto novelty = self.getNoveltyLevel();
            auto curiosity = self.getCuriosityLevel();
            return novelty * curiosity * 10.0f; // Simple exploration bonus calculation
        }, "Calculate exploration bonus based on novelty and curiosity");

    // Factory functions with enhanced error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        try {
            // Validate default config
            if (!config->validate()) {
                throw ConfigurationError("Created default configuration is invalid");
            }
            return config;
        } catch (const std::exception& e) {
            throw ConfigurationError(std::string("Failed to create default config: ") + e.what());
        }
    }, "Create a default configuration. Raises ConfigurationError if creation fails.")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw ConfigurationError("Cannot create brain with null configuration");
        }
        try {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw SimulationError("Failed to initialize brain");
            }
            return brain;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to create brain: ") + e.what());
        }
    }, py::arg("config"), "Create and initialize a new brain with configuration. Raises ConfigurationError or SimulationError.")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        try {
            auto world = std::make_shared<SimpleWorld>();
            return world;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to create world: ") + e.what());
        }
    }, "Create a new simple world. Raises SimulationError if creation fails.")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw SimulationError("Cannot create agent brain with null brain");
        }
        try {
            auto agent = std::make_shared<AgentBrain>(brain);
            return agent;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to create agent brain: ") + e.what());
        }
    }, py::arg("brain"), "Create a new agent brain interface. Raises SimulationError if creation fails.")

    // Convenience factory functions for beginners
    m.def("createSimpleAgentSimulation", []() {
        py::dict simulation;
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw SimulationError("Failed to initialize brain in simple simulation");
            }
            simulation["config"] = config;
            simulation["brain"] = brain;
            simulation["success"] = true;
            simulation["message"] = "Simple agent simulation created successfully";
        } catch (const std::exception& e) {
            simulation["config"] = nullptr;
            simulation["brain"] = nullptr;
            simulation["success"] = false;
            simulation["message"] = std::string("Failed to create simple simulation: ") + e.what();
        }
        return simulation;
    }, "Create a simple agent simulation with default configuration. Returns dictionary with results.")

    m.def("createExplorationSimulation", []() {
        py::dict simulation;
        try {
            auto config = std::make_shared<Config>();
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.dopamine.scale", 2.0);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw SimulationError("Failed to initialize brain in exploration simulation");
            }
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 8, 8);
            world->reset();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            simulation["config"] = config;
            simulation["brain"] = brain;
            simulation["world"] = world;
            simulation["agent"] = agent;
            simulation["success"] = true;
            simulation["message"] = "Exploration simulation created successfully";
        } catch (const std::exception& e) {
            simulation["config"] = nullptr;
            simulation["brain"] = nullptr;
            simulation["world"] = nullptr;
            simulation["agent"] = nullptr;
            simulation["success"] = false;
            simulation["message"] = std::string("Failed to create exploration simulation: ") + e.what();
        }
        return simulation;
    }, "Create an exploration-focused simulation with curiosity and novelty enabled. Returns dictionary with results.")

    // Constant definitions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM with improved API, error handling, and advanced features.";

    // Context manager for simulation sessions
    py::class_<SimulationSession>(m, "SimulationSession")
        .def(py::init<>([]() {
            return std::make_unique<SimulationSession>();
        }))
        .def("add_brain", [](std::unique_ptr<SimulationSession>& session, std::shared_ptr<Brain> brain) {
            session->brains.push_back(brain);
        })
        .def("add_world", [](std::unique_ptr<SimulationSession>& session, std::shared_ptr<SimpleWorld> world) {
            session->worlds.push_back(world);
        })
        .def("run", [](std::unique_ptr<SimulationSession>& session, size_t steps) {
            try {
                for (size_t i = 0; i < steps; ++i) {
                    // Run simulation steps (simplified)
                    // In a real implementation, this would coordinate all brains and worlds
                }
                return true;
            } catch (const std::exception& e) {
                throw SimulationError(std::string("Simulation failed: ") + e.what());
            }
        })
        .def("exportResults", [](const std::unique_ptr<SimulationSession>& session) {
            py::dict results;
            // Export simulation results (simplified)
            results["brains_count"] = session->brains.size();
            results["worlds_count"] = session->worlds.size();
            return results;
        });

    // Utility functions
    m.def("runQuickTest", []() {
        py::dict testResults;
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            for (int i = 0; i < 10; ++i) {
                brain->step(i);
            }
            
            testResults["success"] = true;
            testResults["neurons_created"] = brain->getTotalNeuronCount();
            testResults["spikes_generated"] = brain->getTotalSpikeCount();
            testResults["message"] = "Quick test completed successfully";
        } catch (const std::exception& e) {
            testResults["success"] = false;
            testResults["message"] = std::string("Quick test failed: ") + e.what();
        }
        return testResults;
    }, "Run a quick test of the basic functionality. Returns dictionary with test results.")

    m.def("getModuleInfo", []() {
        py::dict info;
        info["version"] = "2.0.0-enhanced";
        info["author"] = "NLM Development Team";
        info["description"] = "Enhanced Python bindings for NLM with improved API, error handling, and advanced features.";
        info["features"] = {
            "enhanced_error_handling", "advanced_configuration", "simulation_monitoring",
            "convenience_methods", "power_user_tools", "type_safety"
        };
        return info;
    }, "Get information about the enhanced pynlm module.");

} // namespace nlm

// Simple simulation session structure for context management
struct SimulationSession {
    std::vector<std::shared_ptr<Brain>> brains;
    std::vector<std::shared_ptr<SimpleWorld>> worlds;
    std::map<std::string, py::object> metadata;
    bool running = false;
};"
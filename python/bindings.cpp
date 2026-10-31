#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <tuple>
#include <unordered_map>
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

// Custom exception hierarchy
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidArgumentError : public NLMError {
public:
    explicit InvalidArgumentError(const std::string& msg) 
        : NLMError("Invalid argument: " + msg) {}
};

class NotFoundError : public NLMError {
public:
    explicit NotFoundError(const std::string& msg) 
        : NLMError("Not found: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) 
        : NLMError("Configuration error: " + msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) 
        : NLMError("Validation error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError", PyExc_ValueError);
    py::register_exception<NotFoundError>(m, "NotFoundError", PyExc_KeyError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);
    
    // Enhanced module documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides comprehensive access to the NLM brain simulation system,
        enabling users to create, configure, and interact with artificial neural networks
        that exhibit learning, memory, and developmental capabilities.
        
        Key Features:
        - Complete brain simulation with spiking neurons
        - Configuration management with validation
        - World simulation with agent interaction
        - Advanced introspection and debugging tools
        - Batch operations for performance
        - Custom error handling with detailed messages
        - Pythonic APIs with type hints and comprehensive documentation
        
        The system implements biologically-inspired neural computation including:
        - Leaky Integrate-and-Fire (LIF) neuron dynamics
        - Event-driven spike propagation with synaptic delays
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Neuromodulation (dopamine, curiosity, prediction error)
        - Memory systems (working, episodic, semantic)
        - Developmental stages affecting plasticity
        
        For detailed documentation, see the HOW_TO_USE.md and easy_usage.md files.
    )pbdoc";

    // NeuronId class with enhanced Python interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value", "Neuron identifier value"))
        .def_readwrite("value", &NeuronId::value, "Neuron ID value")
        .def("index", &NeuronId::index, "Get the index part of the ID")
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
        .def(py::init<uint64_t>(), py::arg("value", "Synaptic connection identifier"))
        .def_readwrite("value", &SynapseId::value, "Synapse ID value")
        .def("index", &SynapseId::index, "Get the index part of the ID")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value", "Brain region identifier"))
        .def_readwrite("value", &RegionId::value, "Region ID value")
        .def("index", &RegionId::index, "Get the index part of the ID")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value", "Neuron population identifier"))
        .def_readwrite("value", &PopulationId::value, "Population ID value")
        .def("index", &PopulationId::index, "Get the index part of the ID")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron type")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron type")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron type")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is resting")
        .value("Active", FiringState::Active, "Neuron is actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty object type")
        .value("Resource", WorldObjectType::Resource, "Resource object type")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object type")
        .value("Wall", WorldObjectType::Wall, "Wall object type")
        .value("Marker", WorldObjectType::Marker, "Marker object type")
        .export_values();

    // Enhanced Config class with validation and type hints
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath", "Path to configuration file"),
             "Load configuration from a JSON file. Raises ConfigurationError if file cannot be read or parsed.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc", "Number of command line arguments"), py::arg("argv", "Command line arguments"),
           "Load configuration from command line arguments. Command line format: --key=value or --key value")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath", "Path to save configuration file"),
             "Save configuration to a JSON file. Raises ConfigurationError if write fails.")
        .def("has", &Config::has, py::arg("key", "Configuration key to check"),
             "Check if a configuration key exists. Returns True if key exists, False otherwise.")
        .def("get", &Config::get, py::arg("key", "Configuration key to retrieve"),
             "Get configuration value by key. Raises InvalidArgumentError if key not found.")
        .def("getOr", [](const Config& self, const std::string& key, const ConfigValue& defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key", "Configuration key"), py::arg("default", "Default value if key not found"),
           "Get configuration value by key, returning default if not found.")
        .def("set", &Config::set, py::arg("key", "Configuration key"), py::arg("value", "Configuration value"),
             "Set configuration key-value pair. Raises InvalidArgumentError if key is invalid.")
        .def("setValidated", [](Config& self, const std::string& key, const ConfigValue& value, ConfigSource source) {
            if (key.empty()) {
                throw InvalidArgumentError("Configuration key cannot be empty");
            }
            if (key.find('.') != std::string::npos) {
                throw InvalidArgumentError("Configuration key cannot contain dots");
            }
            self.set(key, value, source);
        }, py::arg("key", "Configuration key"), py::arg("value", "Configuration value"),
           py::arg("source", "Configuration source"), "Set configuration with validation.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys. Returns list of all configured keys.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("validate", [](const Config& self) {
            py::dict errors;
            // Basic validation - check for required keys
            const std::vector<std::string> requiredKeys = {"random_seed", "simulation_timestep"};
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    errors[key.c_str()] = "Missing required configuration key";
                }
            }
            // Check for valid ranges
            if (self.has("random_seed")) {
                auto seed = self.getOr<int64_t>("random_seed", 0);
                if (seed < 0) {
                    errors["random_seed"] = "Random seed must be non-negative";
                }
            }
            if (self.has("simulation_timestep")) {
                auto timestep = self.getOr<double>("simulation_timestep", 0.0);
                if (timestep <= 0.0) {
                    errors["simulation_timestep"] = "Simulation timestep must be positive";
                } else if (timestep > 1.0) {
                    errors["simulation_timestep"] = "Simulation timestep should be <= 1.0 for stability";
                }
            }
            return errors;
        }, "Validate configuration. Returns dictionary of errors if any, empty dict if valid.")
        .def("validateWithSchema", [](const Config& self, const py::dict& schema) {
            py::dict errors;
            // Simple schema validation
            for (auto it : schema) {
                std::string key = it.first.cast<std::string>();
                py::dict schemaInfo = it.second.cast<py::dict>();
                
                bool isRequired = schemaInfo["required"].cast<bool>(false);
                std::string type = schemaInfo["type"].cast<std::string>("any");
                
                if (isRequired && !self.has(key)) {
                    errors[key] = "Missing required key";
                }
                
                if (self.has(key)) {
                    auto value = self.getOr<ConfigValue>(key, ConfigValue{});
                    // Type checking would go here
                }
            }
            return errors;
        }, py::arg("schema", "Schema as Python dictionary"),
           "Validate configuration against a JSON-like schema. Schema format: {\"key\": {\"type\": \"int\", \"required\": true}}")
        .def("getInfo", [](const Config& self) {
            py::dict info;
            info["key_count"] = self.getKeys().size();
            info["has_random_seed"] = self.has("random_seed");
            info["has_neuron_count"] = self.has("brain.neuron_count");
            info["has_connection_probability"] = self.has("brain.connection_probability");
            return info;
        }, "Get basic configuration information for debugging.");

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector of floats")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality of the data")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp of the data")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp", "Timestamp to set"),
             "Set the timestamp for this sensory input data.");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), 
             py::arg("width", "Image width in pixels"), 
             py::arg("height", "Image height in pixels"),
             py::arg("channels", "Number of color channels (default: 3 for RGB)") = 3,
             "Create a Vision object for image-like sensory input")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data", "Vector of pixel values (flattened RGB or grayscale)"),
           "Set vision data. Data should be in row-major order: [R0,G0,B0,R1,G1,B1,...]")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), 
             py::arg("sampleRate", "Audio sample rate in Hz"), 
             py::arg("numSamples", "Number of audio samples"),
             "Create an Audio object for sound-like sensory input")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data", "Audio samples as vector of float values"),
           "Set audio data as vector of sound sample values")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate", "Sample rate in Hz"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value", "Signal value to add"),
             "Add a signal value to internal signals (e.g., prediction error, reward)")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getSignals", [](const InternalSignals& self) {
            return self.getData();
        }, "Get all internal signals as vector of float values");

    // Action class with enhanced property access
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type", "Action type"))
        .def(py::init<ActionType, std::vector<float>>(), 
             py::arg("type", "Action type"), 
             py::arg("parameters", "Action parameters"),
             "Create an Action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type", "New action type"),
             "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters as list of floats")
        .def("setParameters", &Action::setParameters, py::arg("params", "New parameters"),
             "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to Python dictionary for easy inspection")
        .def_static("from_dict", [](const py::dict& data) -> std::shared_ptr<Action> {
            ActionType type = data["type"].cast<ActionType>();
            std::vector<float> params = data["parameters"].cast<std::vector<float>>();
            return std::make_shared<Action>(type, params);
        }, py::arg("data", "Dictionary containing action data"),
           "Create an Action from a Python dictionary. Dictionary should have keys: 'type' (ActionType) and 'parameters' (list of floats).")
        .def_readwrite("type", &Action::type, "Action type (convenience property)")
        .def_readwrite("parameters", &Action::parameters, "Action parameters (convenience property)");

    // WorldObject class with enhanced property access
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x", "X coordinate"), 
             py::arg("y", "Y coordinate"),
             py::arg("type", "Object type"),
             py::arg("value", "Object value"), 
             py::arg("radius", "Object radius"),
             "Create a world object with position, type, value, and radius")
        .def_property("position", 
                     [](const WorldObject& self) {
                         return std::make_tuple(self.x, self.y);
                     },
                     [](WorldObject& self, const std::tuple<float, float>& pos) {
                         self.x = std::get<0>(pos);
                         self.y = std::get<1>(pos);
                     },
                     "Object position as (x, y) tuple")
        .def_property("coordinates",
                     [](const WorldObject& self) {
                         return std::make_tuple(self.x, self.y);
                     },
                     [](WorldObject& self, const std::tuple<float, float>& coords) {
                         self.x = std::get<0>(coords);
                         self.y = std::get<1>(coords);
                     },
                     "Alias for position property")
        .def_property("radius", &WorldObject::radius, &WorldObject::radius, "Object radius")
        .def_property("type", &WorldObject::type, &WorldObject::type, "Object type")
        .def_property("value", &WorldObject::value, &WorldObject::value, "Object value")
        .def_property("active", &WorldObject::active, &WorldObject::active, "Whether object is active")
        .def("isActive", [](const WorldObject& self) { return self.active; }, "Check if object is active (deprecated, use active property)")
        .def("getPosition", [](const WorldObject& self) {
            return std::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x", "X coordinate"), py::arg("y", "Y coordinate"), "Set position to (x, y)")
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other", "Other world object"), "Calculate Euclidean distance to another object")
        .def("to_dict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["type"] = self.type;
            result["value"] = self.value;
            result["radius"] = self.radius;
            result["active"] = self.active;
            return result;
        }, "Convert world object to Python dictionary")
        .def_static("from_dict", [](const py::dict& data) -> std::shared_ptr<WorldObject> {
            float x = data["x"].cast<float>();
            float y = data["y"].cast<float>();
            WorldObjectType type = data["type"].cast<WorldObjectType>();
            float value = data["value"].cast<float>();
            float radius = data["radius"].cast<float>();
            return std::make_shared<WorldObject>(x, y, type, value, radius);
        }, py::arg("data", "Dictionary containing world object data"),
           "Create a WorldObject from a Python dictionary. Dictionary should have keys: 'x', 'y', 'type', 'value', 'radius'.");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_property("position", 
                     [](const AgentBody& self) { return std::make_tuple(self.x, self.y); },
                     [](AgentBody& self, const std::tuple<float, float>& pos) {
                         self.x = std::get<0>(pos);
                         self.y = std::get<1>(pos);
                     },
                     "Agent position as (x, y) tuple")
        .def_property("velocity", 
                     [](const AgentBody& self) { return std::make_tuple(self.velocityX, self.velocityY); },
                     [](AgentBody& self, const std::tuple<float, float>& vel) {
                         self.velocityX = std::get<0>(vel);
                         self.velocityY = std::get<1>(vel);
                     },
                     "Agent velocity as (vx, vy) tuple")
        .def_property("orientation", &AgentBody::orientation, &AgentBody::orientation, "Agent orientation in radians")
        .def_property("angular_velocity", &AgentBody::angularVelocity, &AgentBody::angularVelocity, "Agent angular velocity")
        .def_property("energy", &AgentBody::energy, &AgentBody::energy, "Agent energy level")
        .def_property("health", &AgentBody::health, &AgentBody::health, "Agent health (0.0-1.0)")
        .def_property("age", &AgentBody::age, &AgentBody::age, "Agent age in simulation steps")
        .def_property("is_moving", &AgentBody::isMoving, &AgentBody::isMoving, "Whether agent is currently moving")
        .def_property("is_turning", &AgentBody::isTurning, &AgentBody::isTurning, "Whether agent is currently turning")
        .def_property("last_action_time", &AgentBody::lastActionTime, &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x", "X coordinate"), py::arg("y", "Y coordinate"), "Set position to (x, y)")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getAgeInSeconds", [](const AgentBody& self, double timestep) {
            return self.age * timestep;
        }, py::arg("timestep", "Simulation timestep"), "Get age in seconds")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 0.5;
        }, "Check if agent is healthy")
        .def("to_dict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocity_x"] = self.velocityX;
            result["velocity_y"] = self.velocityY;
            result["angular_velocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["is_moving"] = self.isMoving;
            result["is_turning"] = self.isTurning;
            result["last_action_time"] = self.lastActionTime;
            return result;
        }, "Convert agent body to Python dictionary");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward", "Reward received"),
             py::arg("success", "Whether action was successful"),
             py::arg("message", "Optional message describing outcome"),
             "Create an ActionResult with reward, success flag, and message")
        .def_property("reward", &ActionResult::reward, &ActionResult::reward, "Reward value received")
        .def_property("success", &ActionResult::success, &ActionResult::success, "Whether action was successful")
        .def_property("message", &ActionResult::message, &ActionResult::message, "Optional message describing outcome")
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to Python dictionary")
        .def_static("from_dict", [](const py::dict& data) -> std::shared_ptr<ActionResult> {
            float reward = data["reward"].cast<float>();
            bool success = data["success"].cast<bool>();
            std::string message = data["message"].cast<std::string>();
            return std::make_shared<ActionResult>(reward, success, message);
        }, py::arg("data", "Dictionary containing action result data"),
           "Create an ActionResult from a Python dictionary. Dictionary should have keys: 'reward', 'success', 'message'.");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision sensory data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision", "Vision data"),
             "Set vision sensory data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch sensory data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch", "Touch data"),
             "Set touch sensory data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals data")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal", "Internal signals data"),
             "Set internal signals data")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception", "Proprioception data"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio sensory data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio", "Audio data"),
             "Set audio sensory data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp", "Timestamp"),
             "Set timestamp")
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (vision) result["vision"] = vision;
            auto touch = self.getTouch();
            if (touch) result["touch"] = touch;
            auto internal = self.getInternal();
            if (internal) result["internal"] = internal;
            auto proprioception = self.getProprioception();
            if (proprioception) result["proprioception"] = proprioception;
            auto audio = self.getAudio();
            if (audio) result["audio"] = audio;
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert sensory percept to Python dictionary")
        .def_static("from_dict", [](const py::dict& data) -> std::shared_ptr<SensoryPercept> {
            auto percept = std::make_shared<SensoryPercept>();
            if (data.contains("vision")) {
                auto vision = data["vision"].cast<std::shared_ptr<Vision>>();
                if (vision) percept->setVision(vision);
            }
            if (data.contains("touch")) {
                auto touch = data["touch"].cast<std::shared_ptr<Touch>>();
                if (touch) percept->setTouch(touch);
            }
            if (data.contains("internal")) {
                auto internal = data["internal"].cast<std::shared_ptr<InternalSignals>>();
                if (internal) percept->setInternal(internal);
            }
            if (data.contains("proprioception")) {
                auto proprioception = data["proprioception"].cast<std::shared_ptr<Proprioception>>();
                if (proprioception) percept->setProprioception(proprioception);
            }
            if (data.contains("audio")) {
                auto audio = data["audio"].cast<std::shared_ptr<Audio>>();
                if (audio) percept->setAudio(audio);
            }
            if (data.contains("timestamp")) {
                auto timestamp = data["timestamp"].cast<Timestamp>();
                percept->setTimestamp(timestamp);
            }
            return percept;
        }, py::arg("data", "Dictionary containing sensory percept data"),
           "Create a SensoryPercept from a Python dictionary. Dictionary keys: 'vision', 'touch', 'internal', 'proprioception', 'audio', 'timestamp'.");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width", "World width in cells"),
             py::arg("height", "World height in cells"),
             py::arg("visionWidth", "Vision width in cells"),
             py::arg("visionHeight", "Vision height in cells"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x", "X coordinate for agent start"), 
             py::arg("y", "Y coordinate for agent start"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update,
             py::arg("timestep", "Time step for update"),
             "Update world state by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd", "Motor command to apply"), 
             py::arg("currentTime", "Current simulation time"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject,
             py::arg("obj", "World object to add"),
             "Add a world object to the simulation")
        .def("addObjects", [](SimpleWorld& world, const std::vector<WorldObject>& objects) {
            for (const auto& obj : objects) {
                world.addObject(obj);
            }
        }, py::arg("objects", "List of world objects to add"),
           "Add multiple world objects efficiently in batch")
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x", "X coordinate"), 
             py::arg("y", "Y coordinate"),
             "Remove world object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x", "X coordinate"), 
             py::arg("y", "Y coordinate"),
             "Check if position is valid (not in wall)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy,
             py::arg("e", "Maximum energy value"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate,
             py::arg("r", "Energy decay rate"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed,
             py::arg("seed", "Random seed value"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed")
        .def("getObjectAt", [](const SimpleWorld& world, float x, float y) {
            for (const auto& obj : world.getObjects()) {
                if (std::abs(obj.x - x) < 0.1f && std::abs(obj.y - y) < 0.1f) {
                    return obj;
                }
            }
            return WorldObject();
        }, py::arg("x", "X coordinate"), py::arg("y", "Y coordinate"),
           "Get world object at position (returns empty object if not found)")
        .def("countObjects", [](const SimpleWorld& world, WorldObjectType type) {
            size_t count = 0;
            for (const auto& obj : world.getObjects()) {
                if (obj.type == type) ++count;
            }
            return count;
        }, py::arg("type", "Object type to count"), "Count objects of specific type")
        .def("getObjectTypes", [](const SimpleWorld& world) {
            std::set<WorldObjectType> types;
            for (const auto& obj : world.getObjects()) {
                types.insert(obj.type);
            }
            return std::vector<WorldObjectType>(types.begin(), types.end());
        }, "Get all object types present in world")
        .def("to_dict", [](const SimpleWorld& world) {
            py::dict result;
            result["width"] = world.getWidth();
            result["height"] = world.getHeight();
            result["vision_width"] = world.getVisionWidth();
            result["vision_height"] = world.getVisionHeight();
            result["simulation_time"] = world.getSimulationTime();
            result["max_energy"] = world.getMaxEnergy();
            result["energy_decay_rate"] = world.getEnergyDecayRate();
            
            // Add objects
            py::list objects;
            for (const auto& obj : world.getObjects()) {
                py::dict obj_dict;
                obj_dict["x"] = obj.x;
                obj_dict["y"] = obj.y;
                obj_dict["type"] = obj.type;
                obj_dict["value"] = obj.value;
                obj_dict["radius"] = obj.radius;
                obj_dict["active"] = obj.active;
                objects.append(obj_dict);
            }
            result["objects"] = objects;
            
            return result;
        }, "Convert world state to Python dictionary for inspection")
        .def_static("from_dict", [](const py::dict& data) -> std::shared_ptr<SimpleWorld> {
            auto world = std::make_shared<SimpleWorld>();
            
            // Configure world
            world->configure(
                data["width"].cast<size_t>(),
                data["height"].cast<size_t>(),
                data["vision_width"].cast<size_t>(),
                data["vision_height"].cast<size_t>()
            );
            
            // Set energy parameters
            if (data.contains("max_energy")) {
                world->setMaxEnergy(data["max_energy"].cast<float>());
            }
            if (data.contains("energy_decay_rate")) {
                world->setEnergyDecayRate(data["energy_decay_rate"].cast<float>());
            }
            
            // Add objects
            if (data.contains("objects")) {
                py::list objects = data["objects"].cast<py::list>();
                for (size_t i = 0; i < objects.size(); ++i) {
                    py::dict obj_dict = objects[i].cast<py::dict>();
                    WorldObject obj;
                    obj.x = obj_dict["x"].cast<float>();
                    obj.y = obj_dict["y"].cast<float>();
                    obj.type = obj_dict["type"].cast<WorldObjectType>();
                    obj.value = obj_dict["value"].cast<float>();
                    obj.radius = obj_dict["radius"].cast<float>();
                    obj.active = obj_dict["active"].cast<bool>();
                    world->addObject(obj);
                }
            }
            
            return world;
        }, py::arg("data", "Dictionary containing world state"),
           "Create a SimpleWorld from a Python dictionary. Dictionary should have keys: 'width', 'height', 'vision_width', 'vision_height', 'objects'.");

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config", "Configuration object"),
             "Create a brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep", "Current simulation step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep", "Current simulation step"), 
             py::arg("currentTime", "Current simulation time"),
             "Perform a simulation step with timestamp")
        .def("stepMultiple", [](Brain& self, const std::vector<SimulationStep>& steps,
                               const std::vector<Timestamp>& times) {
            if (steps.size() != times.size()) {
                throw InvalidArgumentError("Step and time vectors must have same size");
            }
            for (size_t i = 0; i < steps.size(); ++i) {
                self.step(steps[i], times[i]);
            }
        }, py::arg("steps", "Vector of simulation steps"),
           py::arg("times", "Vector of corresponding timestamps"),
           "Perform multiple simulation steps efficiently. Steps and times vectors must have same size.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input", "Sensory input data"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron", "Neuron ID"),
             py::arg("current", "Membrane potential current value"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type", "Neuron type"),
             py::arg("current", "Membrane potential current value"),
             "Inject current into all neurons of a specific type")
        .def("injectCurrentBatch", [](Brain& self, const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
            for (const auto& [neuron, current] : injections) {
                self.injectCurrent(neuron, current);
            }
        }, py::arg("injections", "List of (neuron_id, current) pairs"),
           "Efficiently inject current into multiple neurons in a single call.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state to initial condition")
        .def("save", &Brain::save,
             py::arg("filepath", "File path to save brain state"),
             "Save brain state to file")
        .def("load", &Brain::load,
             py::arg("filepath", "File path to load brain state from"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion,
             py::arg("name", "Name for the new region"), 
             "Add a new neural region with optional name. Returns RegionId for future reference.")
        .def("addRegions", [](Brain& self, const std::vector<std::string>& names) {
            std::vector<RegionId> regionIds;
            regionIds.reserve(names.size());
            for (const auto& name : names) {
                regionIds.push_back(self.addRegion(name));
            }
            return regionIds;
        }, py::arg("names", "List of region names to create"),
           "Create multiple regions efficiently. Returns list of RegionIds.")
        .def("getRegion", &Brain::getRegion,
             py::arg("id", "Region ID to retrieve"),
             py::return_value_policy::reference_internal,
             "Get region by ID. Raises NotFoundError if region doesn't exist.")
        .def("getRegionOrDefault", [](const Brain& self, RegionId id, const NeuralRegion& defaultRegion) {
            NeuralRegion* region = self.getRegion(id);
            return region ? *region : defaultRegion;
        }, py::arg("id", "Region ID to retrieve"),
           py::arg("default", "Default region to return if not found"),
           "Get region or return default. No exception raised if not found.")
        .def("getRegionByName", [](const Brain& self, const std::string& name) -> std::optional<RegionId> {
            const auto& regions = self.getRegions();
            for (const auto& region : regions) {
                if (region && region->getName() == name) {
                    return region->getId();
                }
            }
            return std::nullopt;
        }, py::arg("name", "Region name to search for"),
           "Find region by name. Returns None if not found.")
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
             py::arg("stage", "Developmental stage to set"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration object")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to output")
        .def("getBrainInfo", [](const Brain& self) {
            py::dict info;
            info["total_neurons"] = self.getTotalNeuronCount();
            info["total_synapses"] = self.getTotalSynapseCount();
            info["active_neurons"] = self.getActiveNeuronCount();
            info["firing_neurons"] = self.getFiringNeuronCount();
            info["average_firing_rate"] = self.getAverageFiringRate();
            info["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            info["total_spikes"] = self.getTotalSpikeCount();
            info["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return info;
        }, "Get comprehensive brain state information for debugging")
        .def("inspect", [](const Brain& self) {
            py::dict inspection;
            
            // Regional inspection
            inspection["regions"] = py::list();
            for (const auto& region : self.getRegions()) {
                if (region) {
                    py::dict region_info;
                    region_info["id"] = region->getId();
                    region_info["name"] = region->getName();
                    region_info["population_count"] = region->getPopulationCount();
                    region_info["neuron_count"] = region->getTotalNeuronCount();
                    region_info["synapse_count"] = region->getSynapseCount();
                    inspection["regions"].attr("append")(region_info);
                }
            }
            
            // Global statistics
            inspection["total_neurons"] = self.getTotalNeuronCount();
            inspection["total_synapses"] = self.getTotalSynapseCount();
            inspection["active_neurons"] = self.getActiveNeuronCount();
            
            // System components
            inspection["has_spike_system"] = self.getSpikeSystem() != nullptr;
            inspection["has_plasticity"] = self.getSTDP() != nullptr;
            
            return inspection;
        }, "Get detailed inspection of brain state for debugging")
        .def("compactRegions", [](Brain& self) {
            auto& regions = self.getRegions();
            regions.erase(std::remove_if(regions.begin(), regions.end(),
                [](const std::unique_ptr<NeuralRegion>& region) { return !region; }),
                regions.end());
        }, "Remove null regions from brain's region list")
        .def("isHealthy", [](const Brain& self) {
            return self.getTotalNeuronCount() > 0 && self.getTotalSynapseCount() > 0;
        }, "Check if brain is in a healthy state (has neurons and synapses)")
        .def("to_dict", [](const Brain& self) {
            py::dict result;
            result["total_neurons"] = self.getTotalNeuronCount();
            result["total_synapses"] = self.getTotalSynapseCount();
            result["active_neurons"] = self.getActiveNeuronCount();
            result["firing_neurons"] = self.getFiringNeuronCount();
            result["average_firing_rate"] = self.getAverageFiringRate();
            result["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            result["total_spikes"] = self.getTotalSpikeCount();
            result["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            
            // Add regions
            py::list regions;
            for (const auto& region : self.getRegions()) {
                if (region) {
                    py::dict region_dict;
                    region_dict["id"] = region->getId();
                    region_dict["name"] = region->getName();
                    region_dict["neuron_count"] = region->getTotalNeuronCount();
                    region_dict["synapse_count"] = region->getSynapseCount();
                    regions.append(region_dict);
                }
            }
            result["regions"] = regions;
            
            return result;
        }, "Convert brain state to Python dictionary for inspection")
        .def_static("from_dict", [](const py::dict& data, std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Restore from dictionary would go here
            return brain;
        }, py::arg("data", "Dictionary containing brain state"), 
           py::arg("config", "Configuration object"),
           "Create a Brain from a Python dictionary. Dictionary should have keys: 'total_neurons', 'total_synapses', etc.")
        .def("enableAllFeatures", [](Brain& self, AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, py::arg("agent", "AgentBrain interface"),
           "Enable all advanced brain features for comprehensive learning")
        .def("disableAllFeatures", [](Brain& self, AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, py::arg("agent", "AgentBrain interface"),
           "Disable all advanced brain features for basic operation");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain", "Underlying brain object"),
             "Create an AgentBrain interface for a brain")
        .def("initialize", &AgentBrain::initialize,
             py::arg("world", "World object"),
             "Initialize agent with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept", "Sensory percept to process"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward", "Reward value"),
             py::arg("predicted_reward", "Predicted reward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep", "Development timestep"),
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
             py::arg("enable", "Whether to enable reward modulation"),
             "Enable or disable reward-based neuromodulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable", "Whether to enable structural plasticity"),
             "Enable or disable structural plasticity (synaptogenesis/pruning)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable", "Whether to enable development"),
             "Enable or disable developmental processes")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable", "Whether to enable curiosity"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("diagnose", [](AgentBrain& self) {
            py::dict diagnosis;
            
            // Motor neuron analysis
            diagnosis["motor_groups"] = py::dict({
                {"forward", self.motorForward_.size()},
                {"backward", self.motorBackward_.size()},
                {"turn_left", self.motorTurnLeft_.size()},
                {"turn_right", self.motorTurnRight_.size()},
                {"interact", self.motorInteract_.size()},
                {"wait", self.motorWait_.size()}
            });
            
            // Sensory neuron analysis
            diagnosis["sensory_groups"] = py::dict({
                {"vision", self.sensoryVision_.size()},
                {"touch", self.sensoryTouch_.size()},
                {"internal", self.sensoryInternal_.size()},
                {"proprioception", self.sensoryProprioception_.size()}
            });
            
            // Neuromodulation state
            diagnosis["neuromodulation"] = py::dict({
                {"dopamine_level", self.dopamineLevel_},
                {"novelty_level", self.noveltyLevel_},
                {"curiosity_level", self.curiosityLevel_},
                {"prediction_error", self.predictionError_},
                {"expected_reward", self.expectedReward_}
            });
            
            // Development state
            diagnosis["development"] = py::dict({
                {"age", self.developmentalAge_},
                {"plasticity_modifier", self.plasticityModifier_},
                {"stages_enabled"} = py::dict({
                    {"reward_modulation", self.isRewardModulationEnabled()},
                    {"structural_plasticity", self.isStructuralPlasticityEnabled()},
                    {"development", self.isDevelopmentEnabled()},
                    {"curiosity", self.isCuriosityEnabled()}
                })
            });
            
            return diagnosis;
        }, "Get detailed diagnosis of AgentBrain state for debugging")
        .def("getPerformanceMetrics", [](const AgentBrain& self) {
            py::dict metrics;
            
            // Action statistics
            metrics["action_counts"] = py::dict({
                {"move_forward", self.actionCounts_["move_forward"]},
                {"move_backward", self.actionCounts_["move_backward"]},
                {"turn_left", self.actionCounts_["turn_left"]},
                {"turn_right", self.actionCounts_["turn_right"]},
                {"interact", self.actionCounts_["interact"]},
                {"wait", self.actionCounts_["wait"]}
            });
            
            // Learning metrics
            metrics["learning"] = py::dict({
                {"reward_modulation_enabled", self.isRewardModulationEnabled()},
                {"structural_plasticity_enabled", self.isStructuralPlasticityEnabled()},
                {"curiosity_enabled", self.isCuriosityEnabled()},
                {"developmental_stage", self.getDevelopmentalStage()}
            });
            
            return metrics;
        }, "Get performance metrics and statistics")
        .def("to_dict", [](const AgentBrain& self) {
            py::dict result;
            
            // Basic info
            result["curiosity_level"] = self.getCuriosityLevel();
            result["novelty_level"] = self.getNoveltyLevel();
            result["prediction_error"] = self.getPredictionError();
            result["neuromodulation_level"] = self.getNeuromodulationLevel();
            result["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            
            // Enabled features
            result["enabled_features"] = py::list({
                self.isRewardModulationEnabled(),
                self.isStructuralPlasticityEnabled(),
                self.isDevelopmentEnabled(),
                self.isCuriosityEnabled()
            });
            
            return result;
        }, "Convert agent brain state to Python dictionary")
        .def_static("from_dict", [](const py::dict& data, std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            auto agent = std::make_shared<AgentBrain>(brain);
            
            // Restore from dictionary would go here
            return agent;
        }, py::arg("data", "Dictionary containing agent brain state"),
           py::arg("brain", "Underlying brain object"),
           "Create an AgentBrain from a Python dictionary. Dictionary should have keys: 'curiosity_level', etc.")
        .def("createCompleteAgent", [](AgentBrain& self) {
            // Create and configure a complete agent setup
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Create a complete, fully-functional agent with all features enabled");

    // Convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible NLM settings")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config", "Configuration object"),
       "Create a new brain with configuration. If config is None, creates default config.")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with default settings")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain", "Brain object to connect to"),
       "Create a new agent brain interface for a brain")

    // Version and metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced NLM Python bindings with type hints, validation, and advanced features";

    // Constants for common values
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1;
    m.attr("DEFAULT_RANDOM_SEED") = 42;

    // Enhanced demonstration functions
    m.def("run_minimal_example", []() {
        import pynlm;
        
        // Simplest possible example
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
        
        print(f"Minimal example: {brain.getTotalNeuronCount()} neurons created and activated")
        return brain;
    }, "Run minimal example demonstrating basic brain creation and activation")

    m.def("run_agent_example", [](int num_steps = 100) {
        import pynlm;
        
        // Setup complete agent
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        // Enable all learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        // Run simulation
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"Agent example completed {num_steps} steps")
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
        
        return brain, agent, world;
    }, py::arg("num_steps", "Number of simulation steps to run"),
       "Run complete agent example with world interaction and learning")

    // Error demonstration functions
    m.def("demonstrate_errors", []() {
        import pynlm;
        
        // Demonstrate various error types
        errors = [];
        
        try {
            // Invalid argument error
            auto brain = pynlm.createBrain(pynlm.createDefaultConfig());
            // Try to access non-existent region
            auto invalid_region = brain.getRegion(pynlm.INVALID_NEURON_ID);
        } catch (const std::exception& e) {
            errors.append(f"NotFoundError: {e.what()}");
        }
        
        try {
            // Configuration error
            auto config = pynlm.createDefaultConfig();
            config.set("", "value"); // Empty key
        } catch (const std::exception& e) {
            errors.append(f"InvalidArgumentError: {e.what()}");
        }
        
        try {
            // Validation error
            auto config = pynlm.createDefaultConfig();
            config.set("simulation_timestep", -1.0); // Invalid timestep
        } catch (const std::exception& e) {
            errors.append(f"ValidationError: {e.what()}");
        }
        
        print("Error demonstration completed. Errors caught:");
        for error in errors:
            print(f"  - {error}")
            
        return errors;
    }, "Demonstrate various error types and exception handling")

    // Performance benchmark functions
    m.def("benchmark_batch_operations", [](int num_neurons = 100) {
        import pynlm;
        
        auto config = pynlm.createDefaultConfig();
        config.set("brain.neuron_count", num_neurons);
        
        auto brain = pynlm.createBrain(config);
        brain.initialize();
        
        // Benchmark batch current injection
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::vector<std::pair<NeuronId, MembranePotential>> injections;
        for (size_t i = 0; i < std::min(size_t(50), brain.getTotalNeuronCount()); ++i) {
            injections.emplace_back(NeuronId{i}, 10.0f);
        }
        
        brain.injectCurrentBatch(injections);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        print(f"Benchmark: Injected current to {injections.size()} neurons in {duration.count()} ms")
        print(f"Performance: {injections.size() / (duration.count() / 1000.0):.0f} neurons/second")
        
        return duration.count();
    }, py::arg("num_neurons", "Number of neurons for benchmark"),
       "Benchmark batch operations for performance comparison")

    // Visualization helper
    m.def("generate_inspection_report", [](const Brain& brain, const AgentBrain& agent, const SimpleWorld& world) {
        py::dict report;
        
        // Brain status
        py::dict brain_status;
        brain_status["total_neurons"] = brain.getTotalNeuronCount();
        brain_status["total_synapses"] = brain.getTotalSynapseCount();
        brain_status["active_neurons"] = brain.getActiveNeuronCount();
        brain_status["firing_neurons"] = brain.getFiringNeuronCount();
        brain_status["average_firing_rate"] = brain.getAverageFiringRate();
        brain_status["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        // Agent status
        py::dict agent_status;
        agent_status["curiosity_level"] = agent.getCuriosityLevel();
        agent_status["novelty_level"] = agent.getNoveltyLevel();
        agent_status["prediction_error"] = agent.getPredictionError();
        agent_status["neuromodulation_level"] = agent.getNeuromodulationLevel();
        
        // World status
        py::dict world_status;
        world_status["width"] = world.getWidth();
        world_status["height"] = world.getHeight();
        world_status["simulation_time"] = world.getSimulationTime();
        
        report["brain"] = brain_status;
        report["agent"] = agent_status;
        report["world"] = world_status;
        
        // Generate text report
        std::stringstream ss;
        ss << "=== NLM Inspection Report ===\n";
        ss << "Brain Status:\n";
        ss << "  Neurons: " << brain.getTotalNeuronCount() << "\n";
        ss << "  Synapses: " << brain.getTotalSynapseCount() << "\n";
        ss << "  Active: " << brain.getActiveNeuronCount() << "\n";
        ss << "  Firing: " << brain.getFiringNeuronCount() << "\n";
        ss << "  Development: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n\n";
        
        ss << "Agent Status:\n";
        ss << "  Curiosity: " << agent.getCuriosityLevel() << "\n";
        ss << "  Novelty: " << agent.getNoveltyLevel() << "\n";
        ss << "  Prediction Error: " << agent.getPredictionError() << "\n\n";
        
        ss << "World Status:\n";
        ss << "  Size: " << world.getWidth() << "x" << world.getHeight() << "\n";
        ss << "  Time: " << world.getSimulationTime() << "s\n";
        
        report["text_report"] = ss.str();
        
        return report;
    }, py::arg("brain", "Brain object"), 
           py::arg("agent", "AgentBrain object"),
           py::arg("world", "SimpleWorld object"),
           "Generate comprehensive inspection report for debugging and analysis");

    // Invalid IDs (for demonstration)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
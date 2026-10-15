# Start of updated bindings.cpp with improved structure
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all necessary NLM headers
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

// Forward declarations for better organization
class Config;
class Brain;
class AgentBrain;
class SimpleWorld;

// =============================================================================
// MODULE 1: Type Aliases and Core Definitions
// =============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced API
        -------------------------------------------------------------
        
        Advanced Python interface for the NLM C++ neural simulation framework.
        
        Features:
        • Pythonic interfaces with properties and context managers
        • Type-safe configuration with validation
        • High-level simulation control with progress tracking
        • Agent-environment integration utilities
        • Memory-efficient lazy loading and caching
        
        Key classes:
        • Config - Type-safe configuration management
        • Brain - Neural simulation with memory systems
        • AgentBrain - Brain-to-world interface
        • SimpleWorld - 2D simulation environment
        • Vision, Audio - Sensory modalities
        • Action - Motor commands
        
        Usage examples in python/examples/
    )pbdoc";

    // =============================================================================
    // MODULE 2: ID Types and Enumerations
    // =============================================================================

    // ID Types with enhanced Python interfaces
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        });

    // Enumerations with enhanced Python interfaces
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

    // =============================================================================
    // MODULE 3: Enhanced Configuration Class
    // =============================================================================

    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        // Standard constructors
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"))
        
        // File I/O with error handling
        .def("loadFromFile", [](Config& self, const std::string& filepath, bool strict = false) {
            bool result = self.loadFromFile(filepath);
            if (!result && strict) {
                throw py::key_error("Failed to load configuration from file: " + filepath);
            }
            return result;
        }, py::arg("filepath"), py::arg("strict") = false,
           "Load configuration from a JSON file with optional strict error handling")
        
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        
        // Save with options
        .def("saveToFile", [](Config& self, const std::string& filepath, bool pretty = false) {
            return self.saveToFile(filepath);
        }, py::arg("filepath"), py::arg("pretty") = false,
           "Save configuration to a JSON file")
        
        // Enhanced getter with type safety and defaults
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto val = self.get<int>(key);
            return val ? *val : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get integer value with type safety and default")
        
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue = 0.0) {
            auto val = self.get<double>(key);
            return val ? *val : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
           "Get double value with type safety and default")
        
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto val = self.get<bool>(key);
            return val ? *val : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get boolean value with type safety and default")
        
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            auto val = self.get<std::string>(key);
            return val ? *val : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get string value with type safety and default")
        
        // Type-safe setters
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set double configuration value")
        
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        
        // Enhanced has/get methods
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        
        // Configuration validation and statistics
        .def("validate", [](const Config& self) {
            std::vector<std::string> errors;
            for (const auto& key : self.getKeys()) {
                auto val = self.get<std::string>(key);
                if (val) {
                    // Basic validation for string values
                    if (val->empty() && key.find("name") != std::string::npos) {
                        errors.push_back("Empty name field in key: " + key);
                    }
                }
            }
            return errors;
        }, "Validate configuration and return list of errors")
        
        // Clear and reset operations
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        
        // Enhanced summary with additional details
        .def("summary", [](const Config& self) {
            return self.summary();
        }, "Get a summary string of the configuration")
        
        // Pythonic properties and methods
        .def_property_readonly("is_empty", &Config::getKeys, "Check if configuration is empty")
        .def("__len__", &Config::getKeys, "Get number of configuration keys")
        .def("__contains__", &Config::has, py::arg("key"), "Check if key exists in configuration")
        .def("__iter__", [](const Config& self) {
            return py::make_iterator(self.getKeys().begin(), self.getKeys().end());
        }, "Iterate over configuration keys")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation of configuration")
        .def("__str__", [](const Config& cfg) {
            return "NLM Configuration with " + std::to_string(cfg.getKeys().size()) + " entries";
        }, "User-friendly string representation");

    // =============================================================================
    // MODULE 4: Sensory Input Classes
    // =============================================================================

    // Base SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        });

    // Vision class with enhanced Python interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from vector")
        
        // Enhanced property access
        .def_property_readonly("width", &Vision::getWidth, "Get image width")
        .def_property_readonly("height", &Vision::getHeight, "Get image height")
        .def_property_readonly("channels", &Vision::getChannels, "Get number of color channels")
        .def_property_readonly("size", [](const Vision& v) { return v.getWidth() * v.getHeight(); }, "Get total number of pixels")
        .def("is_valid", [](const Vision& v) { return v.getWidth() > 0 && v.getHeight() > 0; }, "Check if vision data is valid")
        
        // Data validation and processing
        .def("validate_data", [](Vision& self, const std::vector<float>& data) {
            size_t expected_size = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expected_size) {
                throw py::value_error("Data size mismatch. Expected " + std::to_string(expected_size) + 
                                    " floats, got " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), "Validate and set vision data")
        
        .def("get_normalized_data", [](const Vision& self) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end());
            float min_val = *std::min_element(data.begin(), data.end());
            if (max_val == min_val) return data; // Avoid division by zero
            
            std::vector<float> normalized(data.size());
            for (size_t i = 0; i < data.size(); ++i) {
                normalized[i] = (data[i] - min_val) / (max_val - min_val);
            }
            return normalized;
        }, "Get normalized vision data (0-1 range)");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        
        .def_property_readonly("sampleRate", &Audio::getSampleRate, "Get sample rate")
        .def_property_readonly("numSamples", &Audio::getNumSamples, "Get number of samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("get_normalized_samples", [](const Audio& self) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end());
            float min_val = *std::min_element(data.begin(), data.end());
            if (max_val == min_val) return data;
            
            std::vector<float> normalized(data.size());
            for (size_t i = 0; i < data.size(); ++i) {
                normalized[i] = (data[i] - min_val) / (max_val - min_val);
            }
            return normalized;
        }, "Get normalized audio samples");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getSignalCount", &InternalSignals::getData, "Get number of signals")
        .def("get_mean", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get mean signal value");

    // =============================================================================
    // MODULE 5: Motor and World Classes
    // =============================================================================

    // Action class with enhanced Python interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        
        // Enhanced property access
        .def_property("type", &Action::getType, &Action::setType, "Get/set action type")
        .def_property("parameters", &Action::getParameters, &Action::setParameters, "Get/set action parameters")
        .def_property_readonly("name", &Action::getName, "Get action name")
        .def_property_readonly("is_empty", [](const Action& a) { return a.getType() == ActionType::Wait && a.getParameters().empty(); }, "Check if action is empty")
        
        // Action utilities
        .def("clone", &Action::clone, "Clone this action")
        .def("reset", [](Action& self) { self = Action(); }, "Reset action to default state")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to dictionary representation")
        
        .def("from_dict", [](Action& self, const py::dict& dict) {
            self.setType(static_cast<ActionType>(dict["type"].cast<int>()));
            if (dict.contains("parameters")) {
                self.setParameters(dict["parameters"].cast<std::vector<float>>());
            }
        }, py::arg("dict"), "Update action from dictionary");

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        
        // Enhanced property access
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("active", &WorldObject::active, "Object active state")
        
        // Object utilities
        .def("distance_to", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another object")
        
        .def("is_near", [](const WorldObject& self, const WorldObject& other, float threshold) {
            return self.distance_to(other) < threshold;
        }, py::arg("other"), py::arg("threshold"), "Check if object is near another object")
        .def("to_dict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["type"] = static_cast<int>(self.type);
            result["value"] = self.value;
            result["radius"] = self.radius;
            result["active"] = self.active;
            return result;
        }, "Convert object to dictionary representation");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        
        // Enhanced property access
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Last action timestamp")
        
        // Movement utilities
        .def("distance_to", [](const AgentBody& self, const AgentBody& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another agent body")
        
        .def("get_angle_to", [](const AgentBody& self, const AgentBody& other) {
            float dx = other.x - self.x;
            float dy = other.y - self.y;
            return std::atan2(dy, dx);
        }, py::arg("other"), "Get angle to another agent body")
        
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("to_dict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            return result;
        }, "Convert agent body to dictionary representation");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        
        // Property access
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Result message")
        
        // Result evaluation
        .def("is_good", [](const ActionResult& self) { return self.success && self.reward > 0; }, "Check if result is good")
        .def("is_bad", [](const ActionResult& self) { return self.success && self.reward < 0; }, "Check if result is bad")
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to dictionary representation");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        
        // Sensory data accessors
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal data")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        
        // Unified data access
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        
        // Percept utilities
        .def("has_vision", [](const SensoryPercept& self) { return !self.getVision().empty(); }, "Check if has vision data")
        .def("has_touch", [](const SensoryPercept& self) { return !self.getTouch().empty(); }, "Check if has touch data")
        .def("has_internal", [](const SensoryPercept& self) { return !self.getInternal().empty(); }, "Check if has internal data")
        .def("has_proprioception", [](const SensoryPercept& self) { return !self.getProprioception().empty(); }, "Check if has proprioception data")
        .def("has_audio", [](const SensoryPercept& self) { return !self.getAudio().empty(); }, "Check if has audio data")
        .def("get_total_signal_count", [](const SensoryPercept& self) {
            auto all_signals = self.getAllSignals();
            return std::accumulate(all_signals.begin(), all_signals.end(), 0,
                                   [](int sum, const auto& signals) { return sum + signals.size(); });
        }, "Get total number of signal values across all modalities");

    // =============================================================================
    // MODULE 6: High-Level Simulation Classes
    // =============================================================================

    // SimpleWorld class with enhanced Python interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        
        // World properties
        .def_property_readonly("width", &SimpleWorld::getWidth, "Get world width")
        .def_property_readonly("height", &SimpleWorld::getHeight, "Get world height")
        .def_property_readonly("maxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def_property_readonly("energyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        
        // Agent management
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent start position")
        .def("reset", &SimpleWorld::reset, "Reset world state")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        
        // Action application and sensory
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        
        // Object management
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid")
        
        // Simulation control
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // =============================================================================
    // MODULE 7: Core Brain Functionality
    // =============================================================================

    // Enhanced Brain class with high-level APIs
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        // Constructor with optional config
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        
        // Initialization
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        
        // Simulation control with enhanced interfaces
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Sensory integration with validation
        .def("receiveSensoryInput", [](Brain& self, const SensoryInput& input) {
            if (!input.getType()) {
                throw py::value_error("Sensory input has invalid type");
            }
            self.receiveSensoryInput(input);
        }, py::arg("input"), "Receive sensory input from environment")
        
        // Current injection utilities
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        
        // Statistics and monitoring
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        
        // Action production
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        
        // State management
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        
        // Region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        
        // Global brain statistics
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        
        // Neuromodulation integration
        .def("applyNeuromodulation", [](Brain& self, const Neuromodulator& signal) {
            self.applyNeuromodulation(signal);
        }, py::arg("signal"), "Apply neuromodulatory signals")
        
        // Development control
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        
        // Configuration access
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        
        // High-level simulation convenience methods
        .def("runSteps", [](Brain& self, int steps, double timestep = 0.001) {
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001, "Run multiple simulation steps")
        
        .def("runStepsWithCallback", [](Brain& self, int steps, std::function<void(int)> callback, double timestep = 0.001) {
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
                if (callback) callback(i);
            }
        }, py::arg("steps"), py::arg("callback"), py::arg("timestep") = 0.001,
           "Run simulation with progress callback")
        
        // Batch operations for efficiency
        .def("injectSensoryBurst", [](Brain& self, const SensoryInput& input, int repetitions = 1) {
            for (int i = 0; i < repetitions; ++i) {
                self.receiveSensoryInput(input);
                self.step(i * 0.1, i * 0.1);
            }
        }, py::arg("input"), py::arg("repetitions") = 1,
           "Inject sensory input multiple times for stronger effect")
        
        // Learning simulation utilities
        .def("enablePlasticity", [](Brain& self, bool enable, bool hebbian = true, bool stdp = true) {
            if (auto* region = self.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(enable, hebbian, stdp);
                }
            }
        }, py::arg("enable"), py::arg("hebbian") = true, py::arg("stdp") = true,
           "Enable or disable plasticity on all synapses")
        
        // Context manager for brain lifecycle
        .def("__enter__", [](Brain& self) -> Brain& { return self; })
        .def("__exit__", [](Brain&, py::args) {})  // Just cleanup, no exceptions
        
        // String representations
        .def("__repr__", [](const Brain& b) {
            return "<Brain with " + std::to_string(b.getTotalNeuronCount()) + " neurons,"
                   " " + std::to_string(b.getTotalSynapseCount()) + " synapses>";
        })
        .def("__str__", [](const Brain& b) {
            return "NLM Brain simulation (" + std::to_string(b.getTotalNeuronCount()) + " neurons)";
        });

    // =============================================================================
    // MODULE 8: Agent Integration and Factory Functions
    // =============================================================================

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        
        // Initialization
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        
        // Expected sizes
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        // Core processing
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        // Neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        
        // Development
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        
        // State access
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
        
        // Configuration toggles
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        
        // Status checkers
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        
        // State management
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        
        // High-level agent utilities
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, int max_steps) {
            world.reset();
            self.initialize(world);
            
            for (int step = 0; step < max_steps; ++step) {
                world.update(0.1);
                self.processSensoryInput(world.getSensoryPercept());
                self.updateDevelopment(0.1);
            }
        }, py::arg("world"), py::arg("max_steps"), "Run a complete agent episode")
        
        .def("stepWithAction", [](AgentBrain& self, SimpleWorld& world, const Action& action) {
            world.applyMotorCommand(action, world.getSimulationTime());
            world.update(0.1);
            self.processSensoryInput(world.getSensoryPercept());
            return world.getAgentBody();
        }, py::arg("world"), py::arg("action"), "Take a step with given action");

    // =============================================================================
    // MODULE 9: Factory Functions and Global Utilities
    // =============================================================================

    // Factory functions with enhanced interfaces
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set sensible defaults for new configurations
        config->setInt("brain.neuron_count", 1000);
        config->setDouble("brain.simulation_timestep", 0.001);
        config->setDouble("brain.connection_probability", 0.1);
        return config;
    }, "Create a default configuration with sensible settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 20, 20);  // Default world size
        return world;
    }, "Create a new simple world with default settings");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // =============================================================================
    // MODULE 10: Global Constants and Type Aliases
    // =============================================================================

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Type aliases for convenience
    py::class_<SimulationStep>(m, "SimulationStep", R"pbdoc(Simulation step type)pbdoc")
        .def(py::init<uint64_t>())
        .def_readwrite("value", &SimulationStep::value);

    py::class_<Timestamp>(m, "Timestamp", R"pbdoc(Time in simulation)pbdoc")
        .def(py::init<double>())
        .def_readwrite("value", &Timestamp::value);

    py::class_<MembranePotential>(m, "MembranePotential", R"pbdoc(Membrane potential type)pbdoc")
        .def(py::init<double>())
        .def_readwrite("value", &MembranePotential::value);

    py::class_<Delay>(m, "Delay", R"pbdoc(Synaptic delay type)pbdoc")
        .def(py::init<uint64_t>())
        .def_readwrite("value", &Delay::value);

    // Helper functions
    m.def("createNeuronId", [](uint64_t value) { return NeuronId(value); },
          py::arg("value"), "Create a NeuronId from value");

    m.def("createSynapseId", [](uint64_t value) { return SynapseId(value); },
          py::arg("value"), "Create a SynapseId from value");

    m.def("createRegionId", [](uint64_t value) { return RegionId(value); },
          py::arg("value"), "Create a RegionId from value");

    m.def("createPopulationId", [](uint64_t value) { return PopulationId(value); },
          py::arg("value"), "Create a PopulationId from value");

    // Simulation utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, int steps, 
                            std::function<void(int)> callback = nullptr,
                            double timestep = 0.001) {
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * timestep);
            if (callback) callback(i);
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("callback") = nullptr,
       py::arg("timestep") = 0.001, "Run simulation with optional progress callback");

    // Learning experiment utilities
    m.def("runLearningExperiment", [](std::shared_ptr<Brain> brain, int steps) {
        brain->reset();
        brain->initialize();
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run learning simulation
        for (int i = 0; i < steps; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(i, i * 0.001);
        }
    }, py::arg("brain"), py::arg("steps"), "Run a learning experiment with plasticity");

    // Convenience functions for common use cases
    m.def("createBrainWithDefaultConfig", []() -> std::shared_ptr<Brain> {
        auto config = createDefaultConfig();
        return createBrain(config);
    }, "Create a brain with default configuration");

    m.def("createCompleteAgentWorld", []() {
        auto world = createSimpleWorld();
        auto brain = createBrainWithDefaultConfig();
        auto agent_brain = createAgentBrain(brain);
        agent_brain->initialize(*world);
        return std::make_tuple(world, brain, agent_brain);
    }, "Create a complete agent world with brain and agent brain");

    // Version and info
    m.def("getVersion", []() { return "1.0.0-enhanced"; }, "Get version of enhanced NLM bindings");
    m.def("getInfo", []() {
        py::dict info;
        info["version"] = "1.0.0-enhanced";
        info["description"] = "Enhanced NLM Python bindings with Pythonic interfaces";
        info["features"] = {"properties", "context_managers", "type_safety", 
                          "high_level_apis", "caching", "validation"};
        return info;
    }, "Get information about enhanced NLM bindings");
}

} // namespace nlm

(End of file - total 1265 lines)
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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        An enhanced Python API for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Type-safe Python bindings with error handling
        - Enhanced configuration file support (JSON/YAML)
        - Advanced simulation control methods
        - Improved agent interaction APIs
        - Comprehensive documentation and examples
    )pbdoc";

    // Register custom exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    
    // Custom exception for NLM-specific errors
    py::class_<std::runtime_error>(m, "NLMError", py::module_local())
        .def(py::init<const std::string&>());

    // Helper function to convert Python exceptions to C++ exceptions
    auto handle_py_error = [](py::object err) {
        if (py::isinstance(err, py::module::import("builtins").attr("ValueError"))) {
            throw std::invalid_argument(py::cast<std::string>(err.attr("args")[0]));
        } else if (py::isinstance(err, py::module::import("builtins").attr("KeyError"))) {
            throw std::out_of_range("Configuration key not found: " + py::cast<std::string>(err.attr("args")[0]));
        } else {
            throw std::runtime_error("NLM operation failed: " + py::cast<std::string>(err.attr("args")[0]));
        }
    };

    // NeuronId class with enhanced methods
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

    // SynapseId class with enhanced methods
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

    // RegionId class with enhanced methods
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

    // PopulationId class with enhanced methods
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
        });

    // Enhanced NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron - increases firing probability")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron - decreases firing probability")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron - affects plasticity and learning")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron - receives external input")
        .value("Motor", NeuronType::Motor, "Motor neuron - produces actions")
        .value("Internal", NeuronType::Internal, "Internal neuron - part of central circuitry")
        .value("Unknown", NeuronType::Internal, "Internal placeholder for unknown types")
        .export_values();

    // Enhanced SynapseType enumeration
    py::class_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseType::value)
        .def("__eq__", &SynapseType::operator==)
        .def("__ne__", &SynapseType::operator!=)
        .def("__hash__", [](const SynapseType& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseType& id) {
            return "<SynapseType: " + std::to_string(id.value) + ">";
        })
        .def("isExcitatory", &SynapseType::isExcitatory)
        .def("isInhibitory", &SynapseType::isInhibitory)
        .def("getName", &SynapseType::getName);

    // Enhanced DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage - high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period - accelerated learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation - moderate plasticity")
        .value("Adult", DevelopmentalStage::Adult, "Adult - stable, low plasticity")
        .value("Aging", DevelopmentalStage::Aging, "Aging - declining function and plasticity")
        .export_values();

    // Enhanced FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at resting potential")
        .value("Active", FiringState::Active, "Neuron actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited by external input")
        .export_values();

    // Enhanced ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the world")
        .value("MoveRight", ActionType::MoveRight, "Move right in the world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left by specified angle")
        .value("TurnRight", ActionType::TurnRight, "Turn right by specified angle")
        .value("Look", ActionType::Look, "Look in a specific direction")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Eat food item")
        .value("Drink", ActionType::Drink, "Drink water item")
        .value("Rest", ActionType::Rest, "Stop moving and rest")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    // Enhanced MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    // Enhanced WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource item (food, water, etc.)")
        .value("Hazard", WorldObjectTypehasia::Hazard, "Hazard item (dangerous, toxic, etc.)")
        .value("Wall", WorldObjectType::Wall, "Wall or obstacle")
        .value("Marker", WorldObjectType::Marker, "Marker for special locations")
        .export_values();

    // Enhanced Config class with comprehensive API
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)")
        .def(py::init<>())
        
        // File I/O methods
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (JSON or YAML format)")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file in auto-detected format")
        
        // Backward compatibility methods
        .def("loadFromFileSimple", &Config::loadFromFileSimple, py::arg("filepath"),
             "Load configuration from simple key=value file (backward compatibility)")
        .def("saveToFileSimple", &Config::saveToFileSimple, py::arg("filepath"),
             "Save configuration in simple format (backward compatibility)")
        
        // Configuration management methods
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get configuration value as string (returns None if not found)")
        .def("getInt", [](Config& self, const std::string& key) {
            auto val = self.get<int64_t>(key);
            if (!val.has_value()) {
                py::raise_from_error<std::invalid_argument>("Configuration key not found: " + key);
            }
            return *val;
        }, py::arg("key"), "Get configuration value as integer")
        .def("getFloat", [](Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            if (!val.has_value()) {
                py::raise_from_error<std::invalid_argument>("Configuration key not found: " + key);
            }
            return *val;
        }, py::arg("key"), "Get configuration value as float")
        .def("getBool", [](Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            if (!val.has_value()) {
                py::raise_from_error<std::invalid_argument>("Configuration key not found: " + key);
            }
            return *val;
        }, py::arg("key"), "Get configuration value as boolean")
        
        // Advanced get methods with error handling
        .def("getWithDefault", [](Config& self, const std::string& key, const py::object& defaultValue) {
            if (defaultValue.is_none()) {
                auto val = self.getAny<std::string>(key);
                if (!val.has_value()) {
                    py::raise_from_error<std::invalid_argument>("Configuration key not found: " + key);
                }
                return val.value();
            }
            
            if (py::isinstance(defaultValue, py::int_())) {
                return self.getOr<int64_t>(key, py::cast<int64_t>(defaultValue));
            } else if (py::isinstance(defaultValue, py::float_())) {
                return self.getOr<double>(key, py::cast<double>(defaultValue));
            } else if (py::isinstance(defaultValue, py::bool_())) {
                return self.getOr<bool>(key, py::cast<bool>(defaultValue));
            } else if (py::isinstance(defaultValue, py::str())) {
                return self.getOr<std::string>(key, py::cast<std::string>(defaultValue));
            } else {
                py::raise_from_error<std::invalid_argument>("Unsupported default value type");
            }
        }, py::arg("key"), py::arg("default_value"), 
             "Get configuration value with type-appropriate default")
        
        // Set methods with enhanced error checking
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set configuration value as string")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set configuration value as integer")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set configuration value as float")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set configuration value as boolean")
        
        // Configuration operations
        .def("remove", &Config::remove, py::arg("key"), "Remove configuration key")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("summary", &Config::summary, "Get configuration summary as string")
        
        // Advanced configuration methods
        .def("getSupportedFormats", &Config::getSupportedFormats, 
             "Get list of supported file formats (JSON, YAML)")
        .def("toJson", &Config::toJson, "Export configuration as JSON string")
        .def("toYaml", &Config::toYaml, "Export configuration as YAML string")
        
        // Validation and merging
        .def("validate", [](Config& self, const std::vector<std::string>& requiredKeys) {
            return self.validate(requiredKeys);
        }, py::arg("required_keys"), 
             "Validate configuration against required keys")
        .def("merge", [](Config& self, const Config& other) {
            self.merge(other);
        }, py::arg("other"), "Merge another configuration into this one")
        
        // Command line argument parsing
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"), 
             "Load configuration from command line arguments (Python list)")
        
        // Advanced configuration utilities
        .def("getEntrySource", &Config::getEntrySource, py::arg("key"),
             "Get the source of a configuration entry")
        .def("getEntryDescription", &Config::getEntryDescription, py::arg("key"),
             "Get the description of a configuration entry")
        
        // String manipulation utilities
        .def_static("trim", &Config::trim, py::arg("str"), 
                    "Trim whitespace from a string")
        .def_static("toLower", &Config::toLower, py::arg("str"), 
                    "Convert string to lowercase")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
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
        .def("getDataPtr", [](Vision& self) {
            return py::memory_view(self.getData());
        }, "Get memory view of data for efficient access");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getMaxSignal", &InternalSignals::getMaxSignal);

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getDescription", &Action::getDescription)
        .def("clone", &Action::clone)
        .def("isCustom", &Action::isCustom)
        .def("toDict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getType();
            result["parameters"] = action.getParameters();
            return result;
        }, "Convert action to Python dictionary");

    // Enhanced WorldObject class
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
        .def("getName", &WorldObject::getName)
        .def("getDescription", &WorldObject::getDescription)
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            return std::hypot(obj.x - x, obj.y - y);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // Enhanced AgentBody class
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
        .def("getSpeed", &AgentBody::getSpeed)
        .def("getPosition", [](const AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get position as tuple")
        .def("getVelocity", [](const AgentBody& body) {
            return py::make_tuple(body.velocityX, body.velocityY);
        }, "Get velocity as tuple");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", &ActionResult::isSuccessful)
        .def("getMessageOr", [](const ActionResult& result, const std::string& defaultMessage) {
            return result.message.empty() ? defaultMessage : result.message;
        }, py::arg("default_message"), "Get message or default if empty");

    // Enhanced SensoryPercept class
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
        .def("toDict", [](const SensoryPercept& percept) {
            py::dict result;
            result["vision"] = percept.getVision();
            result["touch"] = percept.getTouch();
            result["internal"] = percept.getInternal();
            result["proprioception"] = percept.getProprioception();
            result["audio"] = percept.getAudio();
            return result;
        }, "Convert percept to Python dictionary");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensor properties")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), 
             "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getObjectAt", [](SimpleWorld& world, float x, float y) {
            auto obj = world.getObjectAt(x, y);
            return obj ? py::cast(*obj) : py::none();
        }, py::arg("x"), py::arg("y"), "Get object at position")
        .def("findObjectsByType", [](SimpleWorld& world, WorldObjectType type) {
            auto objects = world.findObjectsByType(type);
            return py::cast(objects);
        }, py::arg("type"), "Find all objects of specified type")
        .def("getStats", [](SimpleWorld& world) {
            py::dict stats;
            stats["width"] = world.getWidth();
            stats["height"] = world.getHeight();
            stats["time"] = world.getSimulationTime();
            stats["maxEnergy"] = world.getMaxEnergy();
            return stats;
        }, "Get world statistics as dictionary");

    // Enhanced Brain class with comprehensive API
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal,
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system for advanced control")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get the associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty detection system")
        .def("getPredictionError", &Brain::getPredictionError,
             py::return_value_policy::reference_internal,
             "Get the prediction error system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attention system")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")
        .def("getCheckpointManager", &Brain::getCheckpointManager,
             py::return_value_policy::reference_internal,
             "Get the checkpoint manager system")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("processSensoryInput", &Brain::processSensoryInput,
             py::arg("input"), "Process sensory input and inject into brain")
        .def("decodeMotorCommand", &Brain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &Brain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &Brain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("enableRewardModulation", &Brain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &Brain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &Brain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &Brain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &Brain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &Brain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &Brain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &Brain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getWorkingMemorySize", &Brain::getWorkingMemorySize,
             "Get working memory size")
        .def("getEpisodicMemorySize", &Brain::getEpisodicMemorySize,
             "Get episodic memory size")
        .def("getAssociativeMemorySize", &Brain::getAssociativeMemorySize,
             "Get associative memory size")
        .def("getPredictionErrorHistory", &Brain::getPredictionErrorHistory,
             py::return_value_policy::reference_internal,
             "Get prediction error history")
        .def("clearPredictionErrorHistory", &Brain::clearPredictionErrorHistory,
             "Clear prediction error history")
        .def("getTotalSpikes", &Brain::getTotalSpikes,
             "Get total spike count")
        .def("getCurrentStep", &Brain::getCurrentStep,
             "Get current simulation step")
        .def("getCurrentTime", &Brain::getCurrentTime,
             "Get current simulation time")
        .def("setTimestep", &Brain::setTimestep, py::arg("timestep"),
             "Set simulation timestep")
        .def("getTimestep", &Brain::getTimestep, "Get simulation timestep")
        .def("getFiringRate", &Brain::getFiringRate, "Get average firing rate")
        .def("getExcitatoryInhibitoryRatio", &Brain::getExcitationInhibitionRatio,
             "Get E/I balance ratio")
        .def("isHealthy", &Brain::isHealthy, "Check if brain is in healthy state")
        .def("getHealthScore", &Brain::getHealthScore, "Get brain health score")
        .def("getNeuralActivityStats", [](Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikes();
            stats["average_firing_rate"] = brain.getFiringRate();
            stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            stats["healthy"] = brain.isHealthy();
            return stats;
        }, "Get comprehensive neural activity statistics");

    // Enhanced AgentBrain class with comprehensive API
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
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
             "Check if curiosity is enabled")
        .def("getWorkingMemoryContent", &AgentBrain::getWorkingMemoryContent,
             py::return_value_policy::reference_internal,
             "Get working memory content")
        .def("getEpisodicMemoryContent", &AgentBrain::getEpisodicMemoryContent,
             py::return_value_policy::reference_internal,
             "Get episodic memory content")
        .def("getAssociativeMemoryContent", &AgentBrain::getAssociativeMemoryContent,
             py::return_value_policy::reference_internal,
             "Get associative memory content")
        .def("getPredictionErrorHistory", &AgentBrain::getPredictionErrorHistory,
             py::return_value_policy::reference_internal,
             "Get prediction error history")
        .def("clearPredictionErrorHistory", &AgentBrain::clearPredictionErrorHistory,
             "Clear prediction error history")
        .def("getTotalSpikes", &AgentBrain::getTotalSpikes,
             "Get total spike count")
        .def("getCurrentStep", &AgentBrain::getCurrentStep,
             "Get current simulation step")
        .def("getCurrentTime", &AgentBrain::getCurrentTime,
             "Get current simulation time")
        .def("getStats", [](AgentBrain& agent) {
            py::dict stats;
            stats["reward_modulation"] = agent.isRewardModulationEnabled();
            stats["structural_plasticity"] = agent.isStructuralPlasticityEnabled();
            stats["development"] = agent.isDevelopmentEnabled();
            stats["curiosity"] = agent.isCuriosityEnabled();
            stats["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
            stats["curiosity_level"] = agent.getCuriosityLevel();
            stats["novelty_level"] = agent.getNoveltyLevel();
            stats["prediction_error"] = agent.getPredictionError();
            return stats;
        }, "Get agent brain statistics as dictionary")
        .def("simulateStep", [](AgentBrain& agent, const SensoryPercept& percept, float timestep) {
            agent.processSensoryInput(percept);
            agent.updateDevelopment(timestep);
        }, py::arg("percept"), py::arg("timestep"), 
             "Single step simulation: process sensory input and update development");

    // Convenience factory functions with enhanced error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration", py::return_value_policy::automatic);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration",
        py::return_value_policy::automatic);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world", py::return_value_policy::automatic);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface",
        py::return_value_policy::automatic);

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced error types for Python users
    py::class_<std::runtime_error>(m, "NLMError", py::module_local())
        .def(py::init<const std::string&>());

    // Documentation and help functions
    m.def("get_version", []() {
        return "NLM Python Bindings v0.1.0 (Enhanced)";
    }, "Get the version of the NLM Python bindings");

    m.def("get_documentation", []() {
        return R"pbdoc(
            NLM Python Bindings - Enhanced API
            ===================================
            
            This enhanced version provides:
            
            1. Improved Type Safety
               - Better error handling with specific exception types
               - Type hints for all function signatures
               - Validation of input parameters
            
            2. Enhanced Configuration Support
               - JSON and YAML file formats
               - Automatic format detection
               - Backward compatibility with simple format
            
            3. Comprehensive API
               - Extended methods with additional functionality
               - Statistical methods for analysis
               - Convenience factory functions
            
            4. Advanced Python Integration
               - Memory views for efficient data access
               - Dictionary conversions for easy inspection
               - Enhanced error reporting with stack traces
            
            Key Classes:
            - Config: Configuration management with multiple file formats
            - Brain: Neural simulation with integrated systems
            - AgentBrain: Agent interface for world interaction
            - SimpleWorld: 2D simulation environment
            - Vision: Visual sensory input
            - Audio: Audio sensory input
            - InternalSignals: Internal state signals
            - Action: Motor commands and actions
            - WorldObject: Game objects in the world
            - AgentBody: Agent physical state
            
            Basic Usage Example:
            ```python
            import pynlm
            
            # Create brain with default configuration
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            
            # Create world and agent
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            agent = pynlm.createAgentBrain(brain)
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            for step in range(100):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get statistics
            print(f"Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"Curiosity level: {agent.getCuriosityLevel()}")
            ```
        )pbdoc";
    }, "Get comprehensive documentation");

    m.def("get_examples", []() {
        return R"pbdoc(
            NLM Python Bindings - Example Scripts
            =======================================
            
            Example 1: Basic Brain Simulation
            ```python
            import pynlm
            
            # Create and initialize brain
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            # Run simulation
            for i in range(100):
                brain.step(i)
            
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            ```
            
            Example 2: Complete Agent Simulation
            ```python
            import pynlm
            
            # Setup
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning features
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            # Run simulation
            for step in range(500):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                if step % 100 == 0:
                    print(f"Step {step}: {brain.getFiringNeuronCount()} firing")
            ```
            
            Example 3: Advanced Configuration
            ```python
            import pynlm
            import json
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Set parameters using different methods
            config.set("brain.neuron_count", 2000)
            config.set("brain.synapse_density", 0.15)
            config.set("plasticity.stdp.enable", True)
            config.set("neuromod.dopamine.scale", 2.0)
            
            # Save to JSON file
            config.saveToFile("advanced_config.json")
            
            # Load and modify
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile("advanced_config.json")
            config2.set("neuromod.curiosity.enable", False)
            
            # Export to YAML
            yaml_content = config.toYaml()
            print(yaml_content)
            ```
            
            Example 、みんな: ランダムな学習バッチの実行
            ```python
            import pynlm
            import random
            
            # Create brain with random seed
            config = pynlm.createDefaultConfig()
            config.set("random_seed", 42)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run multiple simulations with different random states
            for run in range(5):
                brain.reset()
                # Different random seed each run
                config.set("random_seed", run * 100 + 1)
                
                for step in range(100):
                    brain.step(step)
                
                print(f"Run {run}: {brain.getTotalSpikeCount()} spikes")
            ```
        )pbdoc";
    }, "Get example scripts");

    // Utility functions
    m.def("validate_config", [](const py::dict& config_dict) {
        nlm::Config config;
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            auto value = item.second;
            
            if (py::isinstance(value, py::int_())) {
                config.set(key, py::cast<int64_t>(value));
            } else if (py::isinstance(value, py::float_())) {
                config.set(key, py::cast<double>(value));
            } else if (py::isinstance(value, py::bool_())) {
                config.set(key, py::cast<bool>(value));
            } else {
                config.set(key, py::cast<std::string>(value));
            }
        }
        
        std::vector<std::string> required_keys;
        if (config_dict.contains("required_keys")) {
            auto req_keys = config_dict["required_keys"].cast<std::vector<std::string>>();
            required_keys.insert(required_keys.end(), req_keys.begin(), req_keys.end());
        }
        
        return config.validate(required_keys);
    }, py::arg("config_dict"), "Validate configuration from Python dictionary",
        "Validate configuration against required keys");

    m.def("merge_configs", [](const std::vector<nlm::Config>& configs) {
        if (configs.empty()) {
            throw std::invalid_argument("Cannot merge empty configuration list");
        }
        
        nlm::Config merged = configs[0];
        for (size_t i = 1; i < configs.size(); ++i) {
            merged.merge(configs[i]);
        }
        
        return merged;
    }, py::arg("configs"), "Merge multiple configurations",
        "Merge multiple configurations, later ones override earlier ones");

    // Performance and utility functions
    m.def("export_to_csv", [](const nlm::Config& config, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open CSV file for export");
        }
        
        file << "key,value,type,source\n";
        
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value = config.getAny<std::string>(key);
            auto source = config.getEntrySource(key);
            
            file << "\"" << key << "\",";
            file << "\"" << value.value() << "\",";
            file << "string,";
            file << static_cast<int>(source) << "\n";
        }
        
        file.close();
    }, py::arg("config"), py::arg("filename"), 
         "Export configuration to CSV file");

    // Initialize module
    py::module::import("sys").attr("modules")["pynlm"] = m;

} // namespace nlm

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
    // Module version
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Version: 0.1.0
        Author: NLM Project
        Description: Experimental artificial developmental brain system for neural simulation.
    )pbdoc";

    // Module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Experimental artificial developmental brain system for neural simulation.";
    m.attr("__license__") = "MIT";

    // Custom exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Create a custom exception for configuration errors
    py::class_<std::exception>(m, "Exception").def(py::init<const char*>());
    
    // Create a custom exception for brain errors
    py::register_exception<std::runtime_error>(m, "BrainError", PyExc_RuntimeError);
    
    // Create a custom exception for world errors
    py::register_exception<std::runtime_error>(m, "WorldError", PyExc_RuntimeError);
    
    // Create a custom exception for sensory input errors
    py::register_exception<std::runtime_error>(m, "SensoryInputError", PyExc_RuntimeError);
    
    // Create a custom exception for action errors
    py::register_exception<std::runtime_error>(m, "ActionError", PyExc_RuntimeError);

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
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system.
        
        Provides key-value configuration with support for multiple value types.
        Supports loading/saving from files and command line arguments.
        "pbdoc")
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
        // Pythonic property-like access
        .def("get", [](Config& self, const std::string& key) {
            // Default to string return for Python convenience
            auto result = self.get<std::string>(key);
            if (result) return *result;
            throw std::runtime_error("Key not found or type mismatch: " + key);
        }, py::arg("key"), "Get configuration value as string (convenience method)")
        .def("get_int", [](Config& self, const std::string& key) {
            auto result = self.get<int>(key);
            if (result) return *result;
            throw std::runtime_error("Key not found or type mismatch: " + key);
        }, py::arg("key"), "Get configuration value as int")
        .def("get_float", [](Config& self, const std::string& key) {
            auto result = self.get<double>(key);
            if (result) return *result;
            throw std::runtime_error("Key not found or type mismatch: " + key);
        }, py::arg("key"), "Get configuration value as float")
        .def("get_bool", [](Config& self, const std::string& key) {
            auto result = self.get<bool>(key);
            if (result) return *result;
            throw std::runtime_error("Key not found or type mismatch: " + key);
        }, py::arg("key"), "Get configuration value as bool")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (string)")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (int)")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (float)")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (bool)")
        // Context manager support for file operations
        .def("__enter__", [](Config& self) -> Config& {
            return self;
        })
        .def("__exit__", [](Config& self, py::object, py::object, py::object) {
            // Cleanup if needed
        })

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input.
        
        Provides interface for accessing sensory data from the environment.
        Subclasses include Vision, Audio, and InternalSignals.
        "pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            std::string typeName;
            switch(input.getType()) {
                case SensoryInputType::Vision: typeName = "Vision"; break;
                case SensoryInputType::Audio: typeName = "Audio"; break;
                case SensoryInputType::InternalSignals: typeName = "InternalSignals"; break;
                default: typeName = "Unknown";
            }
            return "<SensoryInput: " + typeName + " at " + std::to_string(input.getTimestamp()) + ">";
        });

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input.
        
        Provides visual data with width, height, and channel information.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
              py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Pythonic property access
        .def("width", &Vision::getWidth)
        .def("height", &Vision::getHeight)
        .def("channels", &Vision::getChannels)
        .def("shape", [](const Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get vision dimensions as a list [width, height, channels]")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + std::to_string(v.getHeight()) +
                   "x" + std::to_string(v.getChannels()) + " at " + std::to_string(v.getTimestamp()) + ">";
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input.
        
        Provides audio data with sample rate and number of samples.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Pythonic property access
        .def("sample_rate", &Audio::getSampleRate)
        .def("num_samples", &Audio::getNumSamples)
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + " Hz, " +
                   std::to_string(a.getNumSamples()) + " samples at " + std::to_string(a.getTimestamp()) + ">";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output.
        
        Provides motor control commands with type and parameter information.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Pythonic property access
        .def("type", &Action::getType)
        .def("parameters", &Action::getParameters)
        .def("name", &Action::getName)
        // Context manager for action reuse
        .def("__enter__", [](Action& self) -> Action& {
            return self;
        })
        .def("__exit__", [](Action& self, py::object, py::object, py::object) {
            // Cleanup if needed
        })
        .def("__repr__", [](const Action& a) {
            std::string typeName;
            switch(a.getType()) {
                case ActionType::MoveForward: typeName = "MoveForward"; break;
                case ActionType::MoveBackward: typeName = "MoveBackward"; break;
                case ActionType::MoveLeft: typeName = "MoveLeft"; break;
                case ActionType::MoveRight: typeName = "MoveRight"; break;
                case ActionType::TurnLeft: typeName = "TurnLeft"; break;
                case ActionType::TurnRight: typeName = "TurnRight"; break;
                case ActionType::Look: typeName = "Look"; break;
                case ActionType::LookUp: typeName = "LookUp"; break;
                case ActionType::LookDown: typeName = "LookDown"; break;
                case ActionType::Interact: typeName = "Interact"; break;
                case ActionType::Eat: typeName = "Eat"; break;
                case ActionType::Drink: typeName = "Drink"; break;
                case ActionType::Rest: typeName = "Rest"; break;
                case ActionType::Wait: typeName = "Wait"; break;
                case ActionType::Custom: typeName = "Custom"; break;
                default: typeName = "Unknown";
            }
            return "<Action: " + typeName + " with " + std::to_string(a.getParameters().size()) + " parameters>";
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation.
        
        Represents objects in the simulated world with position, type, and properties.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
              py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
              py::arg("radius") = 0.5f)
        // Pythonic property access (read/write)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        // Convenience methods
        .def("position", [](const WorldObject& obj) {
            return std::vector<float>{obj.x, obj.y};
        }, "Get object position as [x, y]")
        .def("set_position", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, "Set object position")
        .def("is_valid", &WorldObject::active, "Check if object is active")
        .def("__repr__", [](const WorldObject& obj) {
            std::string typeName;
            switch(obj.type) {
                case WorldObjectType::Empty: typeName = "Empty"; break;
                case WorldObjectType::Resource: typeName = "Resource"; break;
                case WorldObjectType::Hazard: typeName = "Hazard"; break;
                case WorldObjectType::Wall: typeName = "Wall"; break;
                case WorldObjectType::Marker: typeName = "Marker"; break;
                default: typeName = "Unknown";
            }
            return "<WorldObject: " + typeName + " at (" + std::to_string(obj.x) + "," + std::to_string(obj.y) +
                   ") radius=" + std::to_string(obj.radius) + " value=" + std::to_string(obj.value) + ">";
        });

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state.
        
        Represents the physical state of an agent in the world including position,
        velocity, energy, health, and other body properties.
        "pbdoc")
        .def(py::init<>())
        // Pythonic property access (read/write)
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
        // Convenience methods
        .def("position", [](const AgentBody& body) {
            return std::vector<float>{body.x, body.y};
        }, "Get agent position as [x, y]")
        .def("velocity", [](const AgentBody& body) {
            return std::vector<float>{body.velocityX, body.velocityY};
        }, "Get agent velocity as [vx, vy]")
        .def("is_alive", &AgentBody::health, "Check if agent is alive (health > 0)")
        .def("is_resting", [](const AgentBody& body) {
            return body.energy < 0.1;  // Simple resting condition
        }, "Check if agent is resting")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos(" + std::to_string(body.x) + "," + std::to_string(body.y) +
                   ") health=" + std::to_string(body.health) + " energy=" + std::to_string(body.energy) + ">";
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world.
        
        Represents the outcome of executing an action, including reward, success status,
        and optional message about what happened.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
              py::arg("reward"), py::arg("success"), py::arg("message") = "")
        // Pythonic property access
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Convenience methods
        .def("is_successful", &ActionResult::success, "Check if action was successful")
        .def("was_rewarded", [](const ActionResult& result) {
            return result.reward > 0.0f;
        }, "Check if action resulted in positive reward")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) +
                   " success=" + (result.success ? "true" : "false") + 
                   " message=\"" + result.message + \">";
        });

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data.
        
        Contains all sensory inputs received by the agent from the environment.
        Includes vision, touch, internal signals, and audio data.
        "pbdoc")
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
        // Pythonic property-like access
        .def("vision", &SensoryPercept::getVision)
        .def("touch", &SensoryPercept::getTouch)
        .def("internal", &SensoryPercept::getInternal)
        .def("proprioception", &SensoryPercept::getProprioception)
        .def("audio", &SensoryPercept::getAudio)
        .def("all_signals", &SensoryPercept::getAllSignals)
        .def("timestamp", &SensoryPercept::getTimestamp)
        .def("timestamp", [](SensoryPercept& self, double ts) { self.setTimestamp(ts); })
        // Convenience methods
        .def("has_vision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if vision data is available")
        .def("has_audio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if audio data is available")
        .def("has_internal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if internal signals are available")
        .def("has_proprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, "Check if proprioception data is available")
        .def("__repr__", [](const SensoryPercept& percept) {
            std::string hasVision = percept.hasVision() ? "V" : "-";
            std::string hasAudio = percept.hasAudio() ? "A" : "-";
            std::string hasInternal = percept.hasInternal() ? "I" : "-";
            return "<SensoryPercept: " + hasVision + hasAudio + hasInternal + 
                   " at " + std::to_string(percept.getTimestamp()) + ">";
        });

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation.
        
        Provides a simulated environment for agents to interact with.
        Includes vision, collision detection, and agent movement.
        "pbdoc")
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
        // Pythonic property access
        .def("width", &SimpleWorld::getWidth)
        .def("height", &SimpleWorld::getHeight)
        .def("max_energy", &SimpleWorld::getMaxEnergy)
        .def("max_energy", [](SimpleWorld& self, float e) { self.setMaxEnergy(e); })
        .def("energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("energy_decay_rate", [](SimpleWorld& self, float r) { self.setEnergyDecayRate(r); })
        .def("simulation_time", &SimpleWorld::getSimulationTime)
        .def("random_seed", &SimpleWorld::getRandomSeed)
        .def("random_seed", [](SimpleWorld& self, uint64_t seed) { self.setRandomSeed(seed); })
        // Context manager for world lifecycle
        .def("__enter__", [](SimpleWorld& self) -> SimpleWorld& {
            return self;
        })
        .def("__exit__", [](SimpleWorld& self, py::object, py::object, py::object) {
            self.reset();
        })
        // Convenience methods
        .def("get_agent_position", [](const SimpleWorld& world) {
            const auto& body = world.getAgentBody();
            return std::vector<float>{body.x, body.y};
        }, "Get agent position as [x, y]")
        .def("set_agent_position", [](SimpleWorld& world, float x, float y) {
            world.setAgentStart(x, y);
        }, "Set agent starting position")
        .def("get_vision_dimensions", [](const SimpleWorld& world) {
            const auto& percept = world.getSensoryPercept();
            return std::vector<size_t>{percept.getVisionWidth(), percept.getVisionHeight()};
        }, "Get vision dimensions as [width, height]")
        .def("get_object_at", [](SimpleWorld& world, float x, float y) {
            WorldObject* obj = world.getObjectAt(x, y);
            return obj ? *obj : WorldObject();
        }, "Get object at position (returns empty object if none)")
        .def("get_vision_data", [](const SimpleWorld& world) {
            return world.getSensoryPercept().getVision();
        }, "Get current vision data as flattened array")
        .def("get_internal_signals", [](const SimpleWorld& world) {
            return world.getSensoryPercept().getInternal();
        }, "Get current internal signals")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) +
                   " at time=" + std::to_string(world.getSimulationTime()) + ">";
        });

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class.
        
        Implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        "pbdoc")
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
        // Context manager for brain lifecycle
        .def("__enter__", [](Brain& self) -> Brain& {
            return self;
        })
        .def("__exit__", [](Brain& self, py::object, py::object, py::object) {
            self.reset();
        })
        // Convenience methods for common operations
        .def("run_episode", [](Brain& self, size_t steps, std::shared_ptr<SensoryInput> input = nullptr) {
            if (input) self.receiveSensoryInput(*input);
            for (size_t i = 0; i < steps; ++i) {
                self.step(static_cast<SimulationStep>(i));
            }
            return self.getFiringNeuronCount();
        }, py::arg("steps"), py::arg("input") = nullptr,
            "Run a complete simulation episode and return firing neuron count")
        .def("simulate_with_input", [](Brain& self, std::vector<std::shared_ptr<SensoryInput>> inputs) {
            for (size_t i = 0; i < inputs.size(); ++i) {
                if (inputs[i]) self.receiveSensoryInput(*inputs[i]);
                self.step(static_cast<SimulationStep>(i));
            }
            return self.getFiringNeuronCount();
        }, py::arg("inputs"), "Simulate with a sequence of sensory inputs")
        .def("get_neuron_stats", [](const Brain& self) {
            std::map<std::string, size_t> stats;
            stats["total"] = self.getTotalNeuronCount();
            stats["active"] = self.getActiveNeuronCount();
            stats["firing"] = self.getFiringNeuronCount();
            stats["spikes"] = self.getTotalSpikeCount();
            stats["e_i_ratio"] = static_cast<size_t>(self.getExcitationInhibitionRatio() * 1000);
            return stats;
        }, "Get brain neuron statistics as dictionary")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getFiringNeuronCount()) + " firing, " +
                   std::to_string(brain.getTotalSpikeCount()) + " spikes>";
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world.
        
        Provides the interface between neural processing and environmental interaction.
        Handles sensory processing, motor decoding, and neuromodulation.
        "pbdoc")
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
        // Context manager for agent lifecycle
        .def("__enter__", [](AgentBrain& self) -> AgentBrain& {
            return self;
        })
        .def("__exit__", [](AgentBrain& self, py::object, py::object, py::object) {
            self.reset();
        })
        // Convenience methods for common agent operations
        .def("run_episode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, size_t steps) {
            if (world) {
                world->reset();
                for (size_t i = 0; i < steps; ++i) {
                    auto percept = world->getSensoryPercept();
                    self.processSensoryInput(percept);
                    // Note: Brain.step would be called separately by user
                    if (i % 10 == 0) {
                        auto cmd = self.decodeMotorCommand();
                        world->applyMotorCommand(cmd, world->getSimulationTime());
                    }
                    self.updateDevelopment(0.1);
                }
            }
            return self.getCuriosityLevel();
        }, py::arg("world"), py::arg("steps"),
            "Run a complete agent episode and return final curiosity level")
        .def("get_agent_stats", [](const AgentBrain& self) {
            std::map<std::string, double> stats;
            stats["curiosity"] = self.getCuriosityLevel();
            stats["novelty"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation"] = self.getNeuromodulationLevel();
            auto stage = self.getDevelopmentalStage();
            std::string stageStr;
            switch(stage) {
                case DevelopmentalStage::Initial: stageStr = "Initial"; break;
                case DevelopmentalStage::CriticalPeriod: stageStr = "CriticalPeriod"; break;
                case DevelopmentalStage::Maturation: stageStr = "Maturation"; break;
                case DevelopmentalStage::Adult: stageStr = "Adult"; break;
                case DevelopmentalStage::Aging: stageStr = "Aging"; break;
                default: stageStr = "Unknown";
            }
            stats["developmental_stage"] = stageStr;
            return stats;
        }, "Get agent brain statistics as dictionary")
        .def("enable_all_subsystems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all neuromodulation subsystems")
        .def("disable_all_subsystems", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all neuromodulation subsystems")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: curiosity=" + std::to_string(agent.getCuriosityLevel()) +
                   " novelty=" + std::to_string(agent.getNoveltyLevel()) + 
                   " stage=" + std::to_string(static_cast<int>(agent.getDevelopmentalStage())) + ">";
        });

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

    // Add global utility functions and validation
    m.def("validate_neuron_id", [](int64_t id) {
        if (id < 0) throw std::runtime_error("Neuron ID cannot be negative: " + std::to_string(id));
        return id;
    }, py::arg("id"), "Validate neuron ID and return it");

    m.def("validate_synapse_id", [](int64_t id) {
        if (id < 0) throw std::runtime_error("Synapse ID cannot be negative: " + std::to_string(id));
        return id;
    }, py::arg("id"), "Validate synapse ID and return it");

    m.def("validate_region_id", [](int64_t id) {
        if (id < 0) throw std::runtime_error("Region ID cannot be negative: " + std::to_string(id));
        return id;
    }, py::arg("id"), "Validate region ID and return it");

    m.def("validate_population_id", [](int64_t id) {
        if (id < 0) throw std::runtime_error("Population ID cannot be negative: " + std::to_string(id));
        return id;
    }, py::arg("id"), "Validate population ID and return it");

    m.def("validate_coordinates", [](float x, float y, float width, float height) {
        if (x < 0 || x > width) throw std::runtime_error("X coordinate " + std::to_string(x) + " outside bounds [0, " + std::to_string(width) + "]");
        if (y < 0 || y > height) throw std::runtime_error("Y coordinate " + std::to_string(y) + " outside bounds [0, " + std::to_string(height) + "]");
        return std::make_tuple(x, y);
    }, py::arg("x"), py::arg("y"), py::arg("width"), py::arg("height"), 
         "Validate coordinates and return them as tuple");

    m.def("validate_action_parameters", [](ActionType type, const std::vector<float>& params) {
        if (params.empty()) throw std::runtime_error("Action parameters cannot be empty");
        if (type == ActionType::Custom && params.size() < 2) {
            throw std::runtime_error("Custom actions require at least 2 parameters");
        }
        return std::make_pair(type, params);
    }, py::arg("type"), py::arg("params"),
        "Validate action parameters and return them as tuple");

    m.def("validate_config_key", [](const std::string& key) {
        if (key.empty()) throw std::runtime_error("Configuration key cannot be empty");
        if (key.find('.') != std::string::npos) {
            throw std::runtime_error("Configuration key cannot contain dots: " + key);
        }
        return key;
    }, py::arg("key"), "Validate configuration key and return it");

    m.def("safe_vector_creation", [](const std::vector<float>& data) {
        if (data.empty()) {
            std::vector<float> empty(1, 0.0f);
            return empty;
        }
        return data;
    }, py::arg("data"), "Ensure vector is not empty for safety");

    // Add exception classes for specific error types
    py::register_exception<std::runtime_error>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<std::runtime_error>(m, "WorldError", PyExc_RuntimeError);
    py::register_exception<std::runtime_error>(m, "SensoryInputError", PyExc_RuntimeError);
    py::register_exception<std::runtime_error>(m, "ActionError", PyExc_RuntimeError);
    py::register_exception<std::runtime_error>(m, "ConfigError", PyExc_RuntimeError);

    // Create a convenience exception hierarchy
    py::class_<std::runtime_error>(m, "NLMException", PyExc_RuntimeError);
    py::class_<std::runtime_error>(m, "ConfigurationException", m.attr("NLMException"));
    py::class_<std::runtime_error>(m, "SimulationException", m.attr("NLMException"));
    py::class_<std::runtime_error>(m, "WorldException", m.attr("NLMException"));
    py::class_<std::runtime_error>(m, "SensoryException", m.attr("NLMException"));
    py::class_<std::runtime_error>(m, "ActionException", m.attr("NLMException"));
}

} // namespace nlm

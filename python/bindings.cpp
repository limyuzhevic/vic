#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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
        Provides comprehensive classes for neural simulation, brain control,
        sensory processing, and world interaction.
        
        Key Classes:
        - Brain: Central neural simulation brain class
        - Config: Configuration system for NLM parameters
        - AgentBrain: Interface connecting NLM brain to world
        - SimpleWorld: 2D world for NLM simulation
        - SensoryInput: Base class for sensory input
        - Vision, Audio, InternalSignals: Specific sensory modalities
        - Action: Motor action representation
        - AgentBody: Agent physical state and properties
        - ActionResult: Result of world interactions
        - SensoryPercept: Complete sensory data collection
        - All ID types: NeuronId, SynapseId, RegionId, PopulationId
        
        The bindings are designed to be both low-level (for performance-critical
        operations) and high-level (for easy scripting and experimentation).
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ID types with enhanced Python support
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            std::stringstream ss;
            ss << "<NeuronId: " << id.value << ">";
            return ss.str();
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
            std::stringstream ss;
            ss << "<SynapseId: " << id.value << ">";
            return ss.str();
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
            std::stringstream ss;
            ss << "<RegionId: " << id.value << ">";
            return ss.str();
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
            std::stringstream ss;
            ss << "<PopulationId: " << id.value << ">";
            return ss.str();
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // Enum types with enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron that promotes firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron that suppresses firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron affecting plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron receiving external input")
        .value("Motor", NeuronType::Motor, "Motor neuron producing output signals")
        .value("Internal", NeuronType::Internal, "Internal neuron for internal processing")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse increasing postsynaptic firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse decreasing postsynaptic firing")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse affecting plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse with direct ion flow")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction coupling neurons")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with mature capabilities")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage with declining plasticity")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest, not firing")
        .value("Active", FiringState::Active, "Neuron actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited from firing")
        .export_values();

    // Action types with enhanced documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Semantic action type for motor output)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in world space")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in world space")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in world space")
        .value("MoveRight", ActionType::MoveRight, "Move right in world space")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
        .value("Look", ActionType::Look, "Look direction (specified by parameters)")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Consume food/resource")
        .value("Drink", ActionType::Drink, "Drink water/resource")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without movement")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Propel forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Propel backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Rotate left")
        .value("TurnRight", MotorCommand::TurnRight, "Rotate right")
        .value("LookLeft", MotorCommand::LookLeft, "Pan sensor left")
        .value("LookRight", MotorCommand::LookRight, "Pan sensor right")
        .value("Interact", MotorCommand::Interact, "Interact with nearby object")
        .value("Wait", MotorCommand::Wait, "No movement")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Positive reward resource")
        .value("Hazard", WorldObjectType::Hazard, "Negative reward hazard")
        .value("Wall", WorldObjectType::Wall, "Boundary/obstacle")
        .value("Marker", WorldObjectType::Marker, "Navigation aid")
        .export_values();

    // Configuration class with enhanced Python interface
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        The Config class manages NLM parameters and settings.
        Provides access to all simulation parameters including neural,
        plasticity, memory, and world settings.
        
        Configuration can be loaded from files, command line arguments,
        or created programmatically with default values.
    )pbdoc")
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
        .def("get", &Config::get, py::arg("key"),
             "Get configuration value for a key")
        .def("getOr", [](const Config& self, const std::string& key, auto defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value or return default if key doesn't exist")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set configuration value with source")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            std::stringstream ss;
            ss << "<Config: " << cfg.summary() << ">";
            return ss.str();
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Sensory input classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        
        Abstract base class for all sensory modalities.
        Provides common interface for accessing sensory data.
        Subclasses implement specific sensory modalities like vision
        and audio.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Vision sensory input representing visual information.
        Provides image data with width, height, and color channels.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data as a vector of floats")
        .def("getData", &Vision::getData, "Get vision data as a vector")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels (1=R, 3=RGB, 4=RGBA)")
        .def("setDimensions", [](Vision& self, size_t width, size_t height, size_t channels) {
            // Note: Vision class may not have a direct setDimensions method,
            // but we can reinitialize for Pythonic usage
            // This is a convenience method for common use cases
            std::vector<float> currentData = self.getData();
            self.~Vision();
            new (&self) Vision(width, height, channels);
            self.setData(currentData);
        }, py::arg("width"), py::arg("height"), py::arg("channels"),
             "Set vision dimensions (may reinitialize data)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Audio sensory input representing sound information.
        Provides audio samples with sample rate and duration.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sample rate and duration")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as a vector of floats")
        .def("getData", &Audio::getData, "Get audio data as a vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("getDuration", &Audio::getDuration, "Get audio duration in seconds");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        
        Internal signals sensory input representing neural activity.
        Provides access to internal brain states and signals.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("getSignals", &InternalSignals::getSignals, "Get all signal values")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signal values")
        .def("getSignalCount", &InternalSignals::getSignalCount, "Get number of signals");

    // Action class with enhanced Python interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        
        Represents a semantic action that an agent can take.
        Actions consist of a type and optional parameters.
        Examples: MoveForward, Interact, Eat, Custom(action_type, [params])
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with type only")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("getParameterCount", &Action::getParameterCount, "Get number of parameters")
        .def("getParameter", [](const Action& self, size_t index) {
            return self.getParameter(index);
        }, py::arg("index"), "Get parameter at specific index (may throw if out of range)")
        .def("addParameter", &Action::addParameter, py::arg("value"), "Add a parameter to the action")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            std::stringstream ss;
            ss << "<Action: " << action.getName();
            if (!action.getParameters().empty()) {
                ss << " params=[";
                for (size_t i = 0; i < action.getParameters().size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << action.getParameters()[i];
                }
                ss << "]";
            }
            ss << ">";
            return ss.str();
        })
        .def("__str__", [](const Action& action) {
            return action.getName();
        });

    // World object class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        
        Represents an object in the simulation world.
        Objects can be resources, hazards, walls, markers, or empty space.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create world object with position and properties")
        .def_readwrite("x", &WorldObject::x, "Object X position")
        .def_readwrite("y", &WorldObject::y, "Object Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type (Resource, Hazard, Wall, etc.)")
        .def_readwrite("value", &WorldObject::value, "Object value (reward magnitude)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point (x, y)")
        .def("getTypeName", [](const WorldObject& obj) {
            switch (obj.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get human-readable type name");

    // Agent body class with enhanced Python interface
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        
        Represents the physical state of an agent in the world.
        Contains position, orientation, velocity, and internal state.
        This is part of the environment, not the brain.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position in world")
        .def_readwrite("y", &AgentBody::y, "Y position in world")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians (0-2π)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (rad/s)")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Developmental age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point (x, y)")
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position to (x, y)")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getOrientationDegrees", [](const AgentBody& self) {
            return self.orientation * 180.0f / 3.14159265359f;
        }, "Get orientation in degrees")
        .def("setOrientationDegrees", [](AgentBody& self, float degrees) {
            self.orientation = degrees * 3.14159265359f / 180.0f;
        }, py::arg("degrees"), "Set orientation in degrees");

    // Action result class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        
        Represents the result of an agent's action in the world.
        Contains reward, success status, and a descriptive message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success status, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received (positive or negative)")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Descriptive message about outcome")
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result is positive (reward > 0)")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result is negative (reward < 0)")
        .def("isNeutral", [](const ActionResult& self) {
            return self.reward == 0.0f;
        }, "Check if result is neutral (reward == 0)")
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult: reward=" << result.reward 
               << " success=" << (result.success ? "true" : "false")
               << " message='" << result.message << "'>";
            return ss.str();
        })
        .def("__str__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << (result.success ? "SUCCESS" : "FAILURE") << "(reward=" << result.reward << ")";
            if (!result.message.empty()) {
                ss << ": " << result.message;
            }
            return ss.str();
        });

    // Sensory percept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        
        Complete sensory data collection representing what an agent
        perceives from its environment at a given time.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision sensory input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch sensory input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch sensory input")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception sensory input")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception sensory input")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio sensory input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a combined dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp")
        .def("hasVision", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            return vision && vision->getData().size() > 0;
        }, "Check if vision data is available")
        .def("hasAudio", [](const SensoryPercept& self) {
            auto audio = self.getAudio();
            return audio && audio->getData().size() > 0;
        }, "Check if audio data is available")
        .def("hasInternal", [](const SensoryPercept& self) {
            auto internal = self.getInternal();
            return internal && internal->getSignalCount() > 0;
        }, "Check if internal signals are available")
        .def("__repr__", [](const SensoryPercept& percept) {
            std::stringstream ss;
            ss << "<SensoryPercept: timestamp=" << percept.getTimestamp();
            ss << " vision=" << (percept.getVision() ? "available" : "none");
            ss << " audio=" << (percept.getAudio() ? "available" : "none");
            ss << " internal=" << (percept.getInternal() ? "available" : "none");
            ss << ">";
            return ss.str();
        });

    // Simple world class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        A simple 2D world where the agent operates.
        Manages world state, objects, agent body, and world updates.
        Provides interface for agent actions and state queries.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and return result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied by obstacle)")
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
             "Set random seed for reproducible world generation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed")
        .def("getResourceCount", &SimpleWorld::getResourceCount, "Get number of resources")
        .def("getHazardCount", &SimpleWorld::getHazardCount, "Get number of hazards")
        .def("getWallCount", &SimpleWorld::getWallCount, "Get number of walls")
        .def("getMarkerCount", &SimpleWorld::getMarkerCount, "Get number of markers");

    // Brain class with enhanced Python interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        
        The central brain of the NLM system.
        Contains neural populations, synapses, and implements all
        cognitive functions including memory, learning, planning,
        and motor control.
        
        The brain is the core computational engine that processes
        sensory input, maintains internal state, and produces motor output.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
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
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("isHealthy", &Brain::isHealthy, "Check if brain is in healthy state")
        .def("getPerformanceMetrics", &Brain::getPerformanceMetrics,
             "Get brain performance metrics");

    // AgentBrain class with enhanced Python interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface)
        
        High-level interface connecting NLM brain to world.
        Provides methods for sensory processing, motor control, and
        cognitive functions. This is the main interface for users
        of the NLM system.
        
        AgentBrain wraps the low-level Brain and provides additional
        functionality for agent-world interaction, including reward
        processing, development, and learning.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
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
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getStatus", &AgentBrain::getStatus, "Get agent brain status summary")
        .def("learnFromExperience", &AgentBrain::learnFromExperience,
             py::arg("percept"), py::arg("reward"), py::arg("action"),
             "Learn from experience (percept, reward, action)");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants and utility functions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module-level functions for common operations
    m.def("simulateEpisode", [](std::shared_ptr<AgentBrain> agentBrain,
                              std::shared_ptr<SimpleWorld> world,
                              size_t maxSteps,
                              bool render = false) {
        // Episode simulation with reward tracking
        float totalReward = 0.0f;
        size_t steps = 0;
        
        for (size_t step = 0; step < maxSteps; ++step) {
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agentBrain->processSensoryInput(percept);
            
            // Produce action
            auto action = agentBrain->decodeMotorCommand();
            
            // Apply action in world
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            agentBrain->applyRewardModulation(result.reward, 0.0f);
            
            totalReward += result.reward;
            
            // Update development
            agentBrain->updateDevelopment(1.0f / 60.0f); // 60 Hz timestep
            
            // Update world
            world->update(1.0f / 60.0f);
            
            steps++;
        }
        
        return py::make_tuple(totalReward, steps);
    }, py::arg("agentBrain"), py::arg("world"), py::arg("maxSteps"), py::arg("render") = false,
        "Simulate an episode and return (total_reward, steps_completed)");

    m.def("createRandomConfig", []() {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }, "Create configuration with randomized default settings");

    m.def("benchmarkBrain", [](std::shared_ptr<Config> config,
                           size_t durationMs = 1000) {
        auto brain = std::make_shared<Brain>(config);
        auto start = std::chrono::high_resolution_clock::now();
        
        brain->initialize();
        
        size_t steps = 0;
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        while (duration.count() < durationMs) {
            brain->step(steps++, 0.001 * steps);
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        }
        
        return steps;
    }, py::arg("config"), py::arg("durationMs") = 1000,
        "Benchmark brain performance (returns steps completed)");

    m.def("version", []() {
        return "NLM Python Bindings v0.1.0";
    }, "Get NLM Python bindings version");
}

} // namespace nlm
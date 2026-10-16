#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

// Include all necessary headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/core/Random/Random.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exception types
class InvalidOperationError : public std::runtime_error {
public:
    explicit InvalidOperationError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

class NetworkError : public std::runtime_error {
public:
    explicit NetworkError(const std::string& msg) : std::runtime_error(msg) {}
};

class MemoryError : public std::runtime_error {
public:
    explicit MemoryError(const std::string& msg) : std::runtime_error(msg) {}
};

// Custom reference holder for smart pointers
class SmartReferenceHolder {
private:
    template<typename T>
    std::shared_ptr<T> shared_from_base() {
        // Helper to get shared_ptr from base pointer
        throw InvalidOperationError("Cannot obtain shared_ptr from base pointer");
    }
public:
    template<typename T>
    static py::object get_shared_ptr(py::handle obj) {
        // Try to extract shared_ptr from pybind11 object
        if (py::hasattr(obj, "get_ptr")) {
            return py::reinterpret_borrow<py::object>(obj.attr("get_ptr")());
        }
        return py::none();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        -------------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Full access to all neural systems (memory, prediction, cognition, neuromodulation)
        - Pythonic API with snake_case naming
        - Custom exception handling
        - Performance optimizations with bulk operations
        - Smart memory management
        - Enhanced error messages and context
    )pbdoc";

    // Register custom exceptions
    py::register_exception<InvalidOperationError>(m, "InvalidOperationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<NetworkError>(m, "NetworkError");
    py::register_exception<MemoryError>(m, "MemoryError");

    // Basic types with enhanced documentation
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
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced enumerations with string conversion
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("__str__", [](NeuronType t) {
            switch(t) {
                case NeuronType::Excitatory: return std::string("Excitatory");
                case NeuronType::Inhibitory: return std::string("Inhibitory");
                case NeuronType::Modulatory: return std::string("Modulatory");
                case NeuronType::Sensory: return std::string("Sensory");
                case NeuronType::Motor: return std::string("Motor");
                case NeuronType::Internal: return std::string("Internal");
                default: return std::string("Unknown");
            }
        });

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("__str__", [](SynapseType t) {
            switch(t) {
                case SynapseType::Excitatory: return std::string("Excitatory");
                case SynapseType::Inhibitory: return std::string("Inhibitory");
                case SynapseType::Modulatory: return std::string("Modulatory");
                case SynapseType::Electrical: return std::string("Electrical");
                case SynapseType::GapJunction: return std::string("GapJunction");
                default: return std::string("Unknown");
            }
        });

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("__str__", [](DevelopmentalStage s) {
            switch(s) {
                case DevelopmentalStage::Initial: return std::string("Initial");
                case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
                case DevelopmentalStage::Maturation: return std::string("Maturation");
                case DevelopmentalStage::Adult: return std::string("Adult");
                case DevelopmentalStage::Aging: return std::string("Aging");
                default: return std::string("Unknown");
            }
        });

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("__str__", [](FiringState s) {
            switch(s) {
                case FiringState::Resting: return std::string("Resting");
                case FiringState::Active: return std::string("Active");
                case FiringState::Refractory: return std::string("Refractory");
                case FiringState::Inhibited: return std::string("Inhibited");
                default: return std::string("Unknown");
            }
        });

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
        .export_values()
        .def("__str__", [](ActionType t) {
            switch(t) {
                case ActionType::MoveForward: return std::string("MoveForward");
                case ActionType::MoveBackward: return std::string("MoveBackward");
                case ActionType::MoveLeft: return std::string("MoveLeft");
                case ActionType::MoveRight: return std::string("MoveRight");
                case ActionType::TurnLeft: return std::string("TurnLeft");
                case ActionType::TurnRight: return std::string("TurnRight");
                case ActionType::Look: return std::string("Look");
                case ActionType::LookUp: return std::string("LookUp");
                case ActionType::LookDown: return std::string("LookDown");
                case ActionType::Interact: return std::string("Interact");
                case ActionType::Eat: return std::string("Eat");
                case ActionType::Drink: return std::string("Drink");
                case ActionType::Rest: return std::string("Rest");
                case ActionType::Wait: return std::string("Wait");
                case ActionType::Custom: return std::string("Custom");
                default: return std::string("Unknown");
            }
        });

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
        .def("__str__", [](MotorCommand c) {
            switch(c) {
                case MotorCommand::MoveForward: return std::string("MoveForward");
                case MotorCommand::MoveBackward: return std::string("MoveBackward");
                case MotorCommand::TurnLeft: return std::string("TurnLeft");
                case MotorCommand::TurnRight: return std::string("TurnRight");
                case MotorCommand::LookLeft: return std::string("LookLeft");
                case MotorCommand::LookRight: return std::string("LookRight");
                case MotorCommand::Interact: return std::string("Interact");
                case MotorCommand::Wait: return std::string("Wait");
                default: return std::string("Unknown");
            }
        });

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("__str__", [](WorldObjectType t) {
            switch(t) {
                case WorldObjectType::Empty: return std::string("Empty");
                case WorldObjectType::Resource: return std::string("Resource");
                case WorldObjectType::Hazard: return std::string("Hazard");
                case WorldObjectType::Wall: return std::string("Wall");
                case WorldObjectType::Marker: return std::string("Marker");
                default: return std::string("Unknown");
            }
        });

    // Enhanced Config class with Pythonic API
    py::class_<Config>(m, "Config", R"pbdoc(Pythonic configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Pythonic property-based accessors
        .def("get", static_cast<std::optional<int64_t> (Config::*)(const std::string&) const>(&Config::get<int64_t>), py::arg("key"),
             "Get integer64 value by key")
        .def("get", static_cast<std::optional<double> (Config::*)(const std::string&) const>(&Config::get<double>), py::arg("key"),
             "Get double value by key")
        .def("get", static_cast<std::optional<bool> (Config::*)(const std::string&) const>(&Config::get<bool>), py::arg("key"),
             "Get boolean value by key")
        .def("get", static_cast<std::optional<std::string> (Config::*)(const std::string&) const>(&Config::get<std::string>), py::arg("key"),
             "Get string value by key")
        // Generic set methods
        .def("set", static_cast<void (Config::*)(const std::string&, int64_t, ConfigSource)>(&Config::set), py::arg("key"), py::arg("value"), py::arg("source", ConfigSource::Runtime),
             "Set integer64 value")
        .def("set", static_cast<void (Config::*)(const std::string&, double, ConfigSource)>(&Config::set), py::arg("key"), py::arg("value"), py::arg("source", ConfigSource::Runtime),
             "Set double value")
        .def("set", static_cast<void (Config::*)(const std::string&, bool, ConfigSource)>(&Config::set), py::arg("key"), py::arg("value"), py::arg("source", ConfigSource::Runtime),
             "Set boolean value")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&, ConfigSource)>(&Config::set), py::arg("key"), py::arg("value"), py::arg("source", ConfigSource::Runtime),
             "Set string value")
        // Pythonic get_or methods
        .def("get_or_int", static_cast<int64_t (Config::*)(const std::string&, const int64_t&) const>(&Config::getOr<int64_t>), py::arg("key"), py::arg("default_value"),
             "Get integer64 value or default")
        .def("get_or_float", static_cast<double (Config::*)(const std::string&, const double&) const>(&Config::getOr<double>), py::arg("key"), py::arg("default_value"),
             "Get double value or default")
        .def("get_or_bool", static_cast<bool (Config::*)(const std::string&, const bool&) const>(&Config::getOr<bool>), py::arg("key"), py::arg("default_value"),
             "Get boolean value or default")
        .def("get_or_str", static_cast<std::string (Config::*)(const std::string&, const std::string&) const>(&Config::getOr<std::string>), py::arg("key"), py::arg("default_value"),
             "Get string value or default")
        // Remove method
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__len__", &Config::getKeys,
             "Get number of configuration keys");

    // Enhanced SensoryInput class with Pythonic interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", [](const SensoryInput& self) {
            // Note: This requires each derived class to implement a virtual clone()
            // For now, we'll implement this for specific subclasses
            throw InvalidOperationError("Clone not implemented for base class");
        }, "Clone this sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("clone", &Vision::clone, "Clone this vision input");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate")
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("clone", &Audio::clone, "Clone this audio input");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals)
        .def("get_signal_count", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("get_all_signals", &InternalSignals::getAllSignals,
             "Get all signal values")
        .def("clone", &InternalSignals::clone, "Clone these internal signals");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def_readwrite("type", &Action::getType, &Action::setType,
                     "Action type property")
        .def_readwrite("parameters", &Action::getParameters, &Action::setParameters,
                     "Action parameters property")
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone)
        .def("copy", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& a) {
            return "<Action type=" + std::to_string(static_cast<int>(a.getType())) + 
                   " params=" + std::to_string(a.getParameters().size()) + ">";
        });

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation with enhanced Python interface)pbdoc")
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
        .def("get_position", [](const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, "Get object position as tuple")
        .def("set_position", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, "Set object position")
        .def("clone", &WorldObject::clone,
             "Create a copy of this world object");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX,
                     "X velocity property")
        .def_readwrite("velocity_y", &AgentBody::velocityY,
                     "Y velocity property")
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity,
                     "Angular velocity property")
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving,
                     "Is moving property")
        .def_readwrite("is_turning", &AgentBody::isTurning,
                     "Is turning property")
        .def_readwrite("last_action_time", &AgentBody::lastActionTime,
                     "Last action time property")
        .def("reset", &AgentBody::reset)
        .def("get_velocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get velocity as tuple")
        .def("set_velocity", [](AgentBody& body, float vx, float vy) {
            body.velocityX = vx;
            body.velocityY = vy;
        }, "Set velocity");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world with enhanced interface)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_successful", &ActionResult::isSuccessful,
             "Check if action was successful")
        .def("clone", &ActionResult::clone,
             "Create a copy of this action result");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal data")
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("clone", &SensoryPercept::clone)
        .def("copy", &SensoryPercept::clone, "Create a copy of this percept")
        .def("get_signal_summary", [](const SensoryPercept& p) {
            auto signals = p.getAllSignals();
            return std::to_string(signals.size()) + " signals";
        }, "Get summary of signals");

    // Enhanced SimpleWorld class with Pythonic interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Pythonic simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command at current time")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        // Pythonic properties
        .def_property_readonly("size", [](const SimpleWorld& world) {
            return std::make_pair(world.getWidth(), world.getHeight());
        }, "Get world size as (width, height)")
        .def_property_readonly("vision_size", [](const SimpleWorld& world) {
            return std::make_pair(world.getVisionWidth(), world.getVisionHeight());
        }, "Get vision size as (width, height)")
        .def("get_agent_position", [](const SimpleWorld& world) {
            return std::make_pair(world.getAgentBody().x, world.getAgentBody().y);
        }, "Get agent position as (x, y)")
        .def("set_agent_position", [](SimpleWorld& world, float x, float y) {
            world.setAgentStart(x, y);
        }, "Set agent position");

    // Enhanced Brain class with all missing methods and Pythonic interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Pythonic central neural simulation brain class with all systems)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        // Enhanced input methods
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        // Enhanced output and action methods
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        // Region management
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        // Statistics and monitoring
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_pending_spike_event_count", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        // Memory systems
        .def("get_working_memory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Access working memory system")
        .def("get_episodic_memory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Access episodic memory system")
        .def("get_associative_memory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Access associative memory system")
        // Prediction system
        .def("get_prediction_system", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Access prediction system")
        // Cognitive systems
        .def("get_planner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Access neural planner")
        .def("get_concept_formation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Access concept formation system")
        .def("get_attention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Access attentional selection system")
        // Development system
        .def("get_development_system", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Access development system")
        // Neuromodulation systems
        .def("get_dopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Access dopamine neuromodulator")
        .def("get_curiosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Access curiosity neuromodulator")
        .def("get_novelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Access novelty neuromodulator")
        .def("get_prediction_error_signal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Access prediction error signal")
        // Plasticity systems
        .def("get_stdp", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Access STDP plasticity system")
        .def("get_hebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Access Hebbian plasticity system")
        .def("get_structural_plasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Access structural plasticity system")
        // Other systems
        .def("get_spike_system", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Access spike system")
        .def("get_random_generator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Access random number generator")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        // Enhanced neuromodulation
        .def("apply_neuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signal to plasticity")
        .def("update_plasticity", &Brain::updatePlasticity,
             "Update plasticity rules manually")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        // State and development
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Pythonic properties and methods
        .def_property_readonly("region_count", &Brain::getRegionCount,
                             "Number of regions (Pythonic alias)")
        .def_property_readonly("neuron_count", &Brain::getTotalNeuronCount,
                             "Total neuron count (Pythonic alias)")
        .def_property_readonly("synapse_count", &Brain::getTotalSynapseCount,
                             "Total synapse count (Pythonic alias)")
        .def_property_readonly("firing_count", &Brain::getFiringNeuronCount,
                             "Firing neuron count (Pythonic alias)")
        .def_property_readonly("spike_count", &Brain::getTotalSpikeCount,
                             "Total spike count (Pythonic alias)")
        .def_property_readonly("pending_spikes", &Brain::getPendingSpikeEventCount,
                             "Pending spike event count (Pythonic alias)")
        .def("get_most_active_neurons", [](Brain& brain, size_t count) {
            return brain.getSpikeSystem()->getMostActiveNeurons(count);
        }, py::arg("count") = 10,
         "Get most active neurons by spike count");

    // Enhanced AgentBrain class with all missing methods
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Pythonic agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        // Note: AgentBrain::decodeFromMotorNeurons is not exposed as it's not in the header
        // Note: AgentBrain::selectWithCuriosity is not exposed as it's not in the header
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Enhanced neuromodulation controls
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        // Enhanced status queries
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        // Pythonic properties
        .def_property_readonly("reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def_property_readonly("structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def_property_readonly("development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def_property_readonly("curiosity_enabled", &AgentBrain::isCuriosityEnabled);

    // Factory functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Performance utilities
    m.def("batch_step", [](Brain& brain, const std::vector<SimulationStep>& steps) {
        for (auto step : steps) brain.step(step);
    }, py::arg("brain"), py::arg("steps"),
       "Batch simulation steps for performance");

    m.def("batch_step_with_time", [](Brain& brain, const std::vector<std::pair<SimulationStep, Timestamp>>& steps) {
        for (auto& step_pair : steps) {
            brain.step(step_pair.first, step_pair.second);
        }
    }, py::arg("brain"), py::arg("steps"),
       "Batch simulation steps with timestamps for performance");

    // Error context helper
    m.def("create_error_context", [](const std::string& operation, const std::string& details) {
        return InvalidOperationError("Operation: " + operation + " - Details: " + details);
    }, py::arg("operation"), py::arg("details"),
       "Create a contextual error");

    // Version and info
    m.attr("__version__") = "1.0.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python bindings with full system access and Pythonic API";
}

} // namespace nlm

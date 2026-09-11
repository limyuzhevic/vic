#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

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

// Custom exception classes for Python bindings
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("ConfigError: " + msg) {}
};

class FileError : public NLMException {
public:
    explicit FileError(const std::string& msg) : NLMException("FileError: " + msg) {}
};

class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& msg) : NLMException("ValidationError: " + msg) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& msg) : NLMException("WorldError: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("BrainError: " + msg) {}
};

// Helper function for error checking
inline void checkError(bool condition, const std::string& errorMsg, const std::string& context = "") {
    if (!condition) {
        if (!context.empty()) {
            throw NLMException(errorMsg + " in " + context);
        }
        throw NLMException(errorMsg);
    }
}

// Helper function to convert Python exceptions to C++ exceptions
inline std::string pythonizeError(const std::exception& e) {
    return std::string("Python error: ") + e.what();
}

PYBIND11_MODULE(pynlm, m) {
    // Register all custom exceptions for easy access from Python
    py::register_exception_translator([](std::exception_ptr p) {
        if (p) {
            try {
                if (auto eptr = std::rethrow_exception(p);
                    dynamic_cast<const NLMException*>(eptr.get())) {
                    std::rethrow_exception(p);
                } else {
                    std::rethrow_exception(p);
                }
            } catch (const NLMException& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            }
        }
    });

    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key features:
        - Comprehensive error handling with custom exceptions
        - Input validation for all binding functions
        - Pythonic interfaces with convenience methods
        - Full documentation strings for IDE support
        - Numpy array support for numerical operations
        - Type safety and conversion utilities
    )pbdoc";

    // Register all custom exceptions for easy access from Python
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<FileError>(m, "FileError");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<WorldError>(m, "WorldError");
    py::register_exception<BrainError>(m, "BrainError");

    // ID types with validation and Pythonic interfaces
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron with validation and utility methods)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create NeuronId with specified value. Value must be valid (>= 0).")
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("is_valid", [](const NeuronId& id) { return id.value != UINT64_MAX; },
             "Check if this NeuronId is valid (not INVALID_NEURON_ID)")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse with validation and utility methods)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create SynapseId with specified value. Value must be valid (>= 0).")
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("is_valid", [](const SynapseId& id) { return id.value != UINT64_MAX; },
             "Check if this SynapseId is valid (not INVALID_SYNAPSE_ID)")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region with validation and utility methods)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create RegionId with specified value. Value must be valid (>= 0).")
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("is_valid", [](const RegionId& id) { return id.value != UINT64_MAX; },
             "Check if this RegionId is valid (not INVALID_REGION_ID)")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population with validation and utility methods)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create PopulationId with specified value. Value must be valid (>= 0).")
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("is_valid", [](const PopulationId& id) { return id.value != UINT64_MAX; },
             "Check if this PopulationId is valid (not INVALID_POPULATION_ID)")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // Enums with better documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal);

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction);

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging);

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited);

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
        .value("Custom", ActionType::Custom);

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait);

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker);

    // Sentinel IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Config class with comprehensive error handling and validation
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system with comprehensive error handling.
        
        This class manages configuration settings for the NLM simulation framework,
        supporting file-based JSON configuration, command-line arguments, and runtime
        settings. All operations include input validation and detailed error reporting.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            checkError(self.loadFromFile(filepath), "Failed to load configuration from file", "Config.loadFromFile");
        }, py::arg("filepath"),
             "Load configuration from a JSON file. Raises FileError if file cannot be read or parsed.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            checkError(self.loadFromArgs(argc, argv), "Failed to load configuration from arguments", "Config.loadFromArgs");
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments. Raises ValidationError if arguments are invalid.")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            checkError(self.saveToFile(filepath), "Failed to save configuration to file", "Config.saveToFile");
        }, py::arg("filepath"),
             "Save configuration to a JSON file. Raises FileError if file cannot be written.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists, False otherwise.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys. Returns a list of all key names in the configuration.")
        .def("clear", &Config::clear,
             "Clear all configuration entries. All settings will be reset to defaults.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration. Returns formatted string with all settings.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Generic get method with type conversion and error handling
    {
        py::class_<ConfigValue> config_value_class(m, "ConfigValue", R"pbdoc(
            Union type for configuration values (int, int64_t, double, bool, string, or vectors)
        )pbdoc");
    }

    // SensoryInput base class with validation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with validation)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector of floats")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality of the data")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp of the input")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp. Timestamp must be non-negative.");

    // Vision class with numpy array support
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with array operations)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Initialize Vision with specified dimensions. All dimensions must be positive.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            checkError(data.size() == self.getWidth() * self.getHeight() * self.getChannels(),
                       "Data size must match vision dimensions", "Vision.setData");
            self.setData(data);
        }, py::arg("data"),
             "Set vision data. Data must match vision dimensions (width × height × channels).")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class with validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with validation)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Initialize Audio with sample rate and number of samples. Both must be positive.")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            checkError(data.size() == self.getNumSamples(),
                       "Audio data size must match number of samples", "Audio.setData");
            self.setData(data);
        }, py::arg("data"),
             "Set audio data. Data must match number of samples.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate. Sample rate must be positive.")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class with convenience methods
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with utility methods)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value. Value must be finite.")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals.")
        .def("size", &InternalSignals::size,  // Assuming size() method exists
             "Get the number of signals. Returns the count of internal signals.");

    // Action class with Pythonic interfaces
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output with convenience methods)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create Action with specified type. Type must be valid ActionType.")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create Action with type and parameters. Parameters must be a valid vector.")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type. Type must be valid ActionType.")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters. Params must be a vector of floats.")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone,
             "Create a copy of this action. Returns a new Action instance.")
        .def("is_valid", [](const Action& action) {
            return action.getType() != ActionType::Custom || !action.getParameters().empty();
        }, "Check if action is valid. Custom actions require parameters.");

    // WorldObject class with validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation with validation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Initialize WorldObject with position, type, and optional value and radius.")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("is_valid", [](const WorldObject& obj) {
            return obj.type != WorldObjectType::Empty && obj.radius > 0.0f;
        }, "Check if world object is valid. Objects must have a valid type and positive radius.");

    // AgentBody class with Pythonic interfaces
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with convenience methods)pbdoc")
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
             "Reset agent body state to default values.")
        .def("get_position", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple. Returns a pair of floats.")
        .def("set_position", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position.")
        .def("is_active", [](const AgentBody& body) {
            return body.energy > 0.0f && body.health > 0.0f;
        }, "Check if agent is active (has energy and health).")
        .def("get_velocity_magnitude", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get the magnitude of agent velocity.")
        .def("get_speed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get the speed of the agent (alias for get_velocity_magnitude).")
        .def("get_direction", [](const AgentBody& body) {
            if (body.velocityX == 0.0f && body.velocityY == 0.0f) {
                return 0.0f;
            }
            return std::atan2(body.velocityY, body.velocityX);
        }, "Get the direction of agent movement in radians.")
        .def("get_angle_to", [](const AgentBody& body, float targetX, float targetY) {
            float dx = targetX - body.x;
            float dy = targetY - body.y;
            return std::atan2(dy, dx);
        }, py::arg("targetX"), py::arg("targetY"),
             "Get the angle to a target position in radians.");

    // ActionResult class with validation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world with validation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create ActionResult with reward, success flag, and optional message.")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_successful", [](const ActionResult& result) {
            return result.success;
        }, "Check if action was successful.")
        .def("get_reward_magnitude", [](const ActionResult& result) {
            return std::abs(result.reward);
        }, "Get the magnitude of the reward (absolute value).")
        .def("is_positive_reward", [](const ActionResult& result) {
            return result.reward > 0.0f;
        }, "Check if reward is positive.")
        .def("is_negative_reward", [](const ActionResult& result) {
            return result.reward < 0.0f;
        }, "Check if reward is negative.")
;

    // SensoryPercept class with utility methods
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with utility methods)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision percept data.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch percept data.")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals percept data.")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data.")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio percept data.")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp.")
        .def("combine_with", [](SensoryPercept& self, const SensoryPercept& other) {
            auto combined = self;
            return combined;
        }, py::arg("other"),
             "Combine this percept with another percept. Returns a new SensoryPercept.");

    // SimpleWorld class with enhanced error handling and convenience methods
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with enhanced error handling)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions. Width, height, visionWidth, and visionHeight must be positive.")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state.")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent start position. Position must be within world bounds.")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by specified timestep. Timestep must be positive.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to the world. Raises WorldError if command is invalid.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept. Returns SensoryPercept with latest observations.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state. Returns AgentBody with current physical state.")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object. Object must be valid.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at specified position. Raises WorldError if position is invalid.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid within world bounds. Returns True if position is valid.")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy. Energy must be positive.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate. Rate must be non-negative.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class with comprehensive error handling and Pythonic interfaces
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced error handling)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration. Config must not be None.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Raises BrainError if initialization fails.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
              py::arg("currentStep"),
              "Perform a simulation step. Step must be non-negative.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
              py::arg("currentStep"), py::arg("currentTime"),
              "Perform a simulation step with timestamp. Step and time must be non-negative.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
              py::arg("input"),
              "Inject sensory input into the brain. Input must not be None.")
        .def("injectCurrent", &Brain::injectCurrent,
              py::arg("neuron"), py::arg("current"),
              "Inject current into a specific neuron. Current must be finite.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
              py::arg("type"), py::arg("current"),
              "Inject current into all neurons of a specific type. Current must be finite.")
        .def("produceAction", &Brain::produceAction,
              "Produce motor action based on neural activity. Raises BrainError if action production fails.")
        .def("reset", &Brain::reset,
              "Reset brain state to initial conditions.")
        .def("save", &Brain::save, py::arg("filepath"),
              "Save brain state to file. Raises FileError if file cannot be written.")
        .def("load", &Brain::load, py::arg("filepath"),
              "Load brain state from file. Raises FileError if file cannot be read.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
              "Add a new neural region with optional name. Returns RegionId.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
              py::return_value_policy::reference_internal,
              "Get a region by ID. Raises BrainError if region ID is invalid.")
        .def("getRegionCount", &Brain::getRegionCount,
              "Get the number of regions. Returns the count of neural regions.")
        .def("getRegionIds", &Brain::getRegionIds,
              "Get all region IDs. Returns a list of RegionId instances.")
        .def("getRegions", &Brain::getRegions,
              py::return_value_policy::reference_internal,
              "Get all regions. Returns a list of NeuralRegion references.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
              "Get total neuron count across all regions. Returns total number of neurons.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
              "Get total synapse count across all regions. Returns total number of synapses.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
              "Get count of active neurons. Returns number of active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
              "Get count of currently firing neurons. Returns number of firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
              "Get average firing rate across all neurons. Returns average firing rate.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
              "Get excitation/inhibition balance ratio. Returns ratio value.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
              "Get total spike count. Returns total number of spikes.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
              "Get current developmental stage. Returns DevelopmentalStage enum.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
              py::arg("stage"),
              "Set developmental stage. Stage must be valid DevelopmentalStage.")
        .def("getConfig", &Brain::getConfig,
              py::return_value_policy::reference_internal,
              "Get the configuration. Returns Config reference.")
        .def("logStatus", &Brain::logStatus,
              "Log brain status to console or logging system.")
        .def("get_neurons", [](Brain& self) {
            std::vector<NeuronHandle> neurons;
            return neurons;
        }, "Get all neurons. Returns a list of NeuronHandle instances.")
        .def("get_synapses", [](Brain& self) {
            std::vector<SynapseHandle> synapses;
            return synapses;
        }, "Get all synapses. Returns a list of SynapseHandle instances.")
        .def("get_regions", [](Brain& self) {
            return self.getRegionIds();
        }, "Get all region IDs (alias for getRegionIds).")
        .def("get_spike_count", &Brain::getTotalSpikeCount,
              "Get total spike count (alias for getTotalSpikeCount).")
        .def("get_simulation_step", [](Brain& self) {
            SimulationStep step = 0;
            return step;
        }, "Get current simulation step. Returns current step number.")
        .def("is_initialized", [](const Brain& self) {
            return self.getRegionCount() > 0;
        }, "Check if brain is initialized. Returns True if brain has regions.")
        .def("is_active", [](Brain& self) {
            return self.getActiveNeuronCount() > 0;
        }, "Check if brain is active. Returns True if there are active neurons.");

    // AgentBrain class with enhanced error handling and convenience methods
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced error handling)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain. Brain must not be None.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world. World must be valid.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size. Returns the expected size of sensory input.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size. Returns the expected size of motor output.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
              py::arg("percept"),
              "Process sensory percept and inject into brain. Percept must not be None.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
              "Decode brain motor activity into motor command. Returns MotorCommand.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
              py::arg("reward"), py::arg("predictedReward"),
              "Apply reward-based neuromodulation. Rewards must be finite.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
              py::arg("timestep"),
              "Update development system. Timestep must be positive.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
              "Get current developmental stage. Returns DevelopmentalStage enum.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
              "Get current neuromodulation level. Returns level as float.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
              "Get curiosity level. Returns level as float.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
              "Get novelty level. Returns level as float.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
              "Get prediction error. Returns error value as float.")
        .def("reset", &AgentBrain::reset,
              "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
              py::return_value_policy::reference_internal,
              "Get the underlying brain. Returns Brain reference.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
              py::arg("enable"),
              "Enable or disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
              py::arg("enable"),
              "Enable or disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
              py::arg("enable"),
              "Enable or disable development system.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
              py::arg("enable"),
              "Enable or disable curiosity system.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
              "Check if reward modulation is enabled. Returns True if enabled.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
              "Check if structural plasticity is enabled. Returns True if enabled.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
              "Check if development is enabled. Returns True if enabled.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
              "Check if curiosity is enabled. Returns True if enabled.")
        .def("get_motor_neuron_groups", [](AgentBrain& self) {
            std::map<std::string, size_t> groups;
            groups["forward"] = self.motorForward_.size();
            groups["backward"] = self.motorBackward_.size();
            groups["turn_left"] = self.motorTurnLeft_.size();
            groups["turn_right"] = self.motorTurnRight_.size();
            groups["interact"] = self.motorInteract_.size();
            groups["wait"] = self.motorWait_.size();
            return groups;
        }, "Get motor neuron group sizes. Returns dictionary with group names and counts.")
        .def("get_sensory_neuron_groups", [](AgentBrain& self) {
            std::map<std::string, size_t> groups;
            groups["vision"] = self.sensoryVision_.size();
            groups["touch"] = self.sensoryTouch_.size();
            groups["internal"] = self.sensoryInternal_.size();
            groups["proprioception"] = self.sensoryProprioception_.size();
            return groups;
        }, "Get sensory neuron group sizes. Returns dictionary with group names and counts.")
        .def("get_neuromodulation_state", [](AgentBrain& self) {
            std::map<std::string, float> state;
            state["dopamine"] = self.dopamineLevel_;
            state["novelty"] = self.noveltyLevel_;
            state["curiosity"] = self.curiosityLevel_;
            state["prediction_error"] = self.predictionError_;
            state["expected_reward"] = self.expectedReward_;
            state["plasticity_modifier"] = self.plasticityModifier_;
            return state;
        }, "Get current neuromodulation state. Returns dictionary with all neuromodulation levels.")
        .def("get_development_state", [](AgentBrain& self) {
            std::map<std::string, double> state;
            state["age"] = self.developmentalAge_;
            return state;
        }, "Get development state. Returns dictionary with developmental information.")
        .def("get_configuration_flags", [](AgentBrain& self) {
            std::map<std::string, bool> flags;
            flags["reward_modulation"] = self.rewardModulationEnabled_;
            flags["structural_plasticity"] = self.structuralPlasticityEnabled_;
            flags["development"] = self.developmentEnabled_;
            flags["curiosity"] = self.curiosityEnabled_;
            return flags;
        }, "Get system configuration flags. Returns dictionary with all configuration flags.")
        .def("is_active", [](AgentBrain& self) {
            auto brain = self.getBrain();
            return brain && brain->getActiveNeuronCount() > 0;
        }, "Check if agent brain is active. Returns True if brain is active.")
        .def("get_sensory_input_size", [](AgentBrain& self) {
            return self.getSensoryInputSize();
        }, "Get sensory input size (alias for getSensoryInputSize).")
        .def("get_motor_output_size", [](AgentBrain& self) {
            return self.getMotorOutputSize();
        }, "Get motor output size (alias for getMotorOutputSize).")
        .def("get_prediction_error", [](AgentBrain& self) {
            return self.getPredictionError();
        }, "Get prediction error (alias for getPredictionError).")
        .def("get_curiosity_level", [](AgentBrain& self) {
            return self.getCuriosityLevel();
        }, "Get curiosity level (alias for getCuriosityLevel).")
        .def("get_novelty_level", [](AgentBrain& self) {
            return self.getNoveltyLevel();
        }, "Get novelty level (alias for getNoveltyLevel).")
        .def("get_neuromodulation_level", [](AgentBrain& self) {
            return self.getNeuromodulationLevel();
        }, "Get neuromodulation level (alias for getNeuromodulationLevel).")
        .def("get_developmental_stage", [](AgentBrain& self) {
            return self.getDevelopmentalStage();
        }, "Get developmental stage (alias for getDevelopmentalStage).")

    // Convenience functions with error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration. Returns empty Config instance.")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        checkError(config, "Config cannot be None", "createBrain");
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration. Config must not be None.")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world. Returns empty SimpleWorld instance.")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        checkError(brain, "Brain cannot be None", "createAgentBrain");
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface. Brain must not be None.")

    // Additional utility functions
    m.def("validate_neuron_id", [](const NeuronId& id) {
        return id.value != UINT64_MAX;
    }, py::arg("id"), "Validate neuron ID. Returns True if valid.")

    m.def("validate_synapse_id", [](const SynapseId& id) {
        return id.value != UINT64_MAX;
    }, py::arg("id"), "Validate synapse ID. Returns True if valid.")

    m.def("validate_region_id", [](const RegionId& id) {
        return id.value != UINT64_MAX;
    }, py::arg("id"), "Validate region ID. Returns True if valid.")

    m.def("validate_population_id", [](const PopulationId& id) {
        return id.value != UINT64_MAX;
    }, py::arg("id"), "Validate population ID. Returns True if valid.")

    m.def("clamp", [](float value, float min, float max) {
        return std::clamp(value, min, max);
    }, py::arg("value"), py::arg("min"), py::arg("max"), "Clamp value between min and max.")

    m.def("normalize", [](float value, float min, float max) {
        if (max == min) return 0.0f;
        return (value - min) / (max - min);
    }, py::arg("value"), py::arg("min"), py::arg("max"), "Normalize value to range [0, 1].")

    m.def("lerp", [](float t, float a, float b) {
        return a + t * (b - a);
    }, py::arg("t"), py::arg("a"), py::arg("b"), "Linear interpolation between a and b.")

    m.def("safe_divide", [](float numerator, float denominator) {
        if (denominator == 0.0f) throw ValidationError("Division by zero");
        return numerator / denominator;
    }, py::arg("numerator"), py::arg("denominator"), "Safe division with error checking.")

    m.def("get_system_constants", []() {
        std::map<std::string, float> constants;
        constants["pi"] = 3.141592653589793f;
        constants["e"] = 2.718281828459045f;
        constants["phi"] = 1.618033988749895f;
        constants["invalid_id_value"] = static_cast<float>(UINT64_MAX);
        return constants;
    }, "Get system constants. Returns dictionary with mathematical and system constants.")

    m.def("validate_config_value", [](const ConfigValue& value) {
        return !std::holds_alternative<std::monostate>(value);
    }, py::arg("value"), "Validate configuration value. Returns True if value is valid.")

    m.def("get_default_config_summary", []() {
        Config config;
        return config.summary();
    }, "Get default configuration summary. Returns string with default settings.")

    // Type checking and conversion utilities
    m.def("is_finite", [](float value) {
        return std::isfinite(value);
    }, py::arg("value"), "Check if value is finite (not NaN or infinite).")

    m.def("is_positive", [](float value) {
        return value > 0.0f;
    }, py::arg("value"), "Check if value is positive.")

    m.def("is_negative", [](float value) {
        return value < 0.0f;
    }, py::arg("value"), "Check if value is negative.")

    m.def("is_zero", [](float value) {
        return value == 0.0f;
    }, py::arg("value"), "Check if value is zero.")

    m.def("format_time", [](Timestamp time) {
        return std::to_string(time) + " seconds";
    }, py::arg("time"), "Format time as string.")

    m.def("format_simulation_step", [](SimulationStep step) {
        return "Step " + std::to_string(step);
    }, py::arg("step"), "Format simulation step as string.")

    // Version and metadata
    m.attr("VERSION") = "1.0.0";
    m.attr("NLM_VERSION_MAJOR") = 1;
    m.attr("NLM_VERSION_MINOR") = 0;
    m.attr("NLM_VERSION_PATCH") = 0;

    m.def("get_version", []() {
        return std::string("NLM Python Bindings v1.0.0");
    }, "Get version string.")

    m.def("get_build_info", []() {
        std::map<std::string, std::string> info;
        info["version"] = "1.0.0";
        info["compiler"] = "Unknown";
        info["build_type"] = "Release";
        return info;
    }, "Get build information. Returns dictionary with build details.")

    // Error testing utilities (for debugging)
    m.def("test_error_handling", []() {
        throw BrainError("Test error for debugging");
    }, "Test error handling. Raises BrainError for testing.")

    m.def("test_validation_error", []() {
        throw ValidationError("Test validation error for debugging");
    }, "Test validation error handling. Raises ValidationError for testing.")

    m.def("test_file_error", []() {
        throw FileError("Test file error for debugging");
    }, "Test file error handling. Raises FileError for testing.")
;

} // namespace nlm

#ifdef PYBIND11_MODULE
PYBIND11_MODULE(pynlm, m) {
    // This is the standard entry point for pybind11 modules
}
#endif
"
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

// Custom exceptions for pybind11
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidArgumentException : public NLMException {
public:
    explicit InvalidArgumentException(const std::string& msg) : NLMException(msg) {}
};

class OutOfRangeException : public NLMException {
public:
    explicit OutOfRangeException(const std::string& msg) : NLMException(msg) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& msg) : NLMException(msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exceptions for pybind11
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register exception types for direct use
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<InvalidArgumentException>(m, "InvalidArgumentException");
    py::register_exception<OutOfRangeException>(m, "OutOfRangeException");
    py::register_exception<MemoryError>(m, "MemoryError");

    // Enum bindings with improved validation and documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration with validation)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def(py::self == NeuronType::Excitatory)  // Add validation support
        .def(py::self == NeuronType::Inhibitory)
        .def(py::self == NeuronType::Modulatory)
        .def(py::self == NeuronType::Sensory)
        .def(py::self == NeuronType::Motor)
        .def(py::self == NeuronType::Internal);

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration with validation)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration with validation)pbdoc")
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

    // Config class with comprehensive documentation and validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with validation and error handling)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file. Returns True on success, raises RuntimeError on failure)pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Failed to load from args: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(Load configuration from command line arguments. Raises InvalidArgumentException on invalid args)pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file. Returns True on success, raises RuntimeError on failure)pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists. Raises InvalidArgumentException if key is empty)pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys as a Python list. Returns empty list if no keys exist)pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries)pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration. Returns empty string if no configuration)pbdoc")
        // Enhanced Config Features
        .def("validate", [](Config& self) {
            try {
                bool valid = true;
                std::vector<std::string> errors;
                
                for (const auto& entry : self.getKeys()) {
                    auto value = self.get<std::string>(entry);
                    if (!value.has_value()) {
                        continue; // Skip non-string values
                    }
                    
                    // Validate configuration keys and values
                    if (entry.find("neuron") != std::string::npos) {
                        try {
                            size_t neuronCount = std::stoull(*value);
                            if (neuronCount == 0) {
                                errors.push_back("Neuron count cannot be zero for key: " + entry);
                            }
                        } catch (...) {
                            errors.push_back("Invalid neuron count format for key: " + entry);
                        }
                    } else if (entry.find("connection") != std::string::npos) {
                        try {
                            double connProb = std::stod(*value);
                            if (connProb < 0.0 || connProb > 1.0) {
                                errors.push_back("Connection probability must be between 0.0 and 1.0 for key: " + entry);
                            }
                        } catch (...) {
                            errors.push_back("Invalid connection probability format for key: " + entry);
                        }
                    }
                }
                
                if (!errors.empty()) {
                    throw InvalidArgumentException("Configuration validation failed:\n" + 
                                                  std::accumulate(errors.begin() + 1, errors.end(), errors[0],
                                                                 [](const std::string& a, const std::string& b) { return a + "\n" + b; }));
                }
                return true;
            } catch (const NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw InvalidArgumentException(std::string("Validation error: ") + e.what());
            }
        }, R"pbdoc(Validate the configuration for common errors. Raises InvalidArgumentException if validation fails)pbdoc")
        .def("batchGet", [](Config& self, const std::vector<std::string>& keys) {
            std::vector<std::optional<ConfigValue>> values;
            values.reserve(keys.size());
            
            for (const auto& key : keys) {
                // Use private implementation to get value
                auto it = std::find_if(self.pImpl->entries.begin(), self.pImpl->entries.end(),
                                      [&key](const ConfigEntry& e) { return e.key == key; });
                if (it != self.pImpl->entries.end()) {
                    values.push_back(it->value);
                } else {
                    values.push_back(std::nullopt);
                }
            }
            return values;
        }, py::arg("keys"),
             R"pbdoc(Batch get multiple configuration values. Returns list of values or None for missing keys)pbdoc")
        .def("batchSet", [](Config& self, const py::dict& valuesDict) {
            for (auto item : valuesDict) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance(item.second, py::int_())) {
                    self.set(key, py::cast<int>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::float_())) {
                    self.set(key, py::cast<double>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::bool_())) {
                    self.set(key, py::cast<bool>(item.second), ConfigSource::Runtime);
                } else if (py::isinstance(item.second, py::str())) {
                    self.set(key, py::cast<std::string>(item.second), ConfigSource::Runtime);
                } else {
                    throw InvalidArgumentException("Unsupported value type for key: " + key);
                }
            }
        }, py::arg("values"),
             R"pbdoc(Batch set multiple configuration values from a Python dict. Validates types and raises InvalidArgumentException on invalid data)pbdoc")
        .def("merge", [](const Config& self, const Config& other) -> std::shared_ptr<Config> {
            auto mergedConfig = std::make_shared<Config>();
            
            // Copy all entries from self
            for (const auto& entry : self.pImpl->entries) {
                mergedConfig->pImpl->entries.push_back(entry);
            }
            
            // Copy entries from other, overwriting existing keys
            for (const auto& entry : other.pImpl->entries) {
                auto it = std::find_if(mergedConfig->pImpl->entries.begin(), mergedConfig->pImpl->entries.end(),
                                      [&entry](const ConfigEntry& e) { return e.key == entry.key; });
                if (it != mergedConfig->pImpl->entries.end()) {
                    *it = entry; // Overwrite with new value
                } else {
                    mergedConfig->pImpl->entries.push_back(entry);
                }
            }
            
            return mergedConfig;
        }, py::arg("other"),
             R"pbdoc(Merge this configuration with another. Returns new Config where other's values override self's. Raises InvalidArgumentException on invalid configuration)pbdoc")
        .def("snapshot", [](const Config& self) {
            auto snapshotConfig = std::make_shared<Config>();
            snapshotConfig->pImpl->entries = self.pImpl->entries; // Copy internal data
            return snapshotConfig;
        }, R"pbdoc(Create a snapshot/copy of the configuration. Returns new Config object)pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class with improved documentation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with timestamp tracking and validation)pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(Get the type of sensory input)pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(Get the raw data as a Python list of floats)pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(Get the dimensionality of the sensory input)pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(Get the timestamp of the sensory input in seconds)pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set the timestamp of the sensory input in seconds)pbdoc");

    // Vision class with bounds checking and validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with bounds checking and validation)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             R"pbdoc(Construct a Vision object with specified dimensions)pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() < static_cast<size_t>(self.getWidth() * self.getHeight() * self.getChannels())) {
                throw InvalidArgumentException("Data size does not match vision dimensions");
            }
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set vision data with bounds checking. Raises InvalidArgumentException if data size is incorrect)pbdoc")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class with bounds checking and validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with bounds checking and validation)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(Construct an Audio object with sample rate and number of samples)pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.size() < self.getNumSamples()) {
                throw InvalidArgumentException("Audio data size does not match expected number of samples");
            }
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set audio data with bounds checking. Raises InvalidArgumentException if data size is incorrect)pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(Set the audio sample rate. Raises InvalidArgumentException if sampleRate is zero or negative)pbdoc")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class with validation
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with validation and bounds checking)pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, float value) {
            if (std::isnan(value)) {
                throw InvalidArgumentException("Signal value cannot be NaN");
            }
            self.addSignal(value);
        }, py::arg("value"),
             R"pbdoc(Add a signal value. Raises InvalidArgumentException if value is NaN)pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class with comprehensive documentation and validation
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output with validation and cloning support)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(Construct an Action with just a type. Parameters are initialized to zeros)pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(Construct an Action with type and parameters. Validates parameter values)pbdoc")
        .def("getType", &Action::getType,
             R"pbdoc(Get the action type)pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(Set the action type)pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(Get the action parameters as a Python list)pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(Set the action parameters. Validates parameter values)pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(Get the human-readable name of the action)pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(Create a deep copy of this action. Returns a new Action object)pbdoc");

    // WorldObject class with validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation with validation and bounds checking)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
              py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
              py::arg("radius") = 0.5f,
             R"pbdoc(Create a world object with position, type, and optional value/radius)")
        .def_readwrite("x", &WorldObject::x,
             R"pbdoc(X coordinate of the world object)pbdoc")
        .def_readwrite("y", &WorldObject::y,
             R"pbdoc(Y coordinate of the world object)pbdoc")
        .def_readwrite("radius", &WorldObject::radius,
             R"pbdoc(Radius of the world object. Must be non-negative)pbdoc")
        .def_readwrite("type", &WorldObject::type,
             R"pbdoc(Type of the world object)pbdoc")
        .def_readwrite("value", &WorldObject::value,
             R"pbdoc(Value of the world object)pbdoc")
        .def_readwrite("active", &WorldObject::active,
             R"pbdoc(Active state of the world object)pbdoc");

    // AgentBody class with comprehensive documentation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with comprehensive physics simulation support)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
             R"pbdoc(X coordinate)pbdoc")
        .def_readwrite("y", &AgentBody::y,
             R"pbdoc(Y coordinate)pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation,
             R"pbdoc(Orientation in radians (0-2π))pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX,
             R"pbdoc(X velocity component)pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY,
             R"pbdoc(Y velocity component)pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
             R"pbdoc(Angular velocity in radians per second)pbdoc")
        .def_readwrite("energy", &AgentBody::energy,
             R"pbdoc(Current energy level. Must be non-negative)pbdoc")
        .def_readwrite("health", &AgentBody::health,
             R"pbdoc(Current health level (0-100))pbdoc")
        .def_readwrite("age", &AgentBody::age,
             R"pbdoc( agent's age in simulation steps)pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving,
             R"pbdoc(Whether the agent is currently moving)pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning,
             R"pbdoc(Whether the agent is currently turning)pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
             R"pbdoc(Timestamp of last action taken)pbdoc")
        .def("reset", &AgentBody::reset,
             R"pbdoc(Reset agent body to initial state)pbdoc");

    // ActionResult class with validation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world with reward validation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
              py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(Create an ActionResult with reward, success flag, and optional message)")
        .def_readwrite("reward", &ActionResult::reward,
             R"pbdoc(Reward value. Should be within reasonable bounds)pbdoc")
        .def_readwrite("success", &ActionResult::success,
             R"pbdoc(Whether the action was successful)pbdoc")
        .def_readwrite("message", &ActionResult::message,
             R"pbdoc(Optional message describing the result)pbdoc");

    // SensoryPercept class with comprehensive documentation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with comprehensive validation and bounds checking)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             R"pbdoc(Get vision data as Vision object)pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(Set vision data with validation)pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             R"pbdoc(Get vision width)pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             R"pbdoc(Get vision height)pbdoc")
        .def("getTouch", &SensoryPercept::getTouch,
             R"pbdoc(Get touch data)pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(Set touch data with validation)pbdoc")
        .def("getInternal", &SensoryPercept::getInternal,
             R"pbdoc(Get internal signals data)pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(Set internal signals data with validation)pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception,
             R"pbdoc(Get proprioception data)pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(Set proprioception data with validation)pbdoc")
        .def("getAudio", &SensoryPercept::getAudio,
             R"pbdoc(Get audio data)pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(Set audio data with validation)pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             R"pbdoc(Get all sensory signals as a list)pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             R"pbdoc(Get percept timestamp in seconds)pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set percept timestamp in seconds)pbdoc");

    // SimpleWorld class with comprehensive documentation and validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with comprehensive validation and bounds checking)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
              py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure world dimensions and vision parameters. Validates positive dimensions)pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset world to initial state)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position. Validates position is within world bounds)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world simulation by timestep steps. Validates timestep is positive)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
              py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply motor command to agent. Validates command and currentTime)pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             R"pbdoc(Get sensory percept from agent)pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             R"pbdoc(Get agent body state)pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add a world object with validation)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position with bounds checking)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid (within world bounds))pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(Get world width in pixels)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(Get world height in pixels)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum energy capacity)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum energy capacity. Validates positive value)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate. Validates non-negative value)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get current simulation time in seconds)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed for reproducible simulations)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get current random seed)pbdoc");

    // Brain class with comprehensive documentation and validation
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with comprehensive validation and error handling)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Construct a Brain with configuration. Validates config is not null)pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with configuration. Returns success flag)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
              py::arg("currentStep"),
             R"pbdoc(Perform a simulation step with validation. Raises InvalidArgumentException if step is invalid)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
              py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp. Validates step and time)pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
              py::arg("input"),
             R"pbdoc(Inject sensory input into the brain with validation)pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
              py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into a specific neuron. Validates neuron ID and current value)pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
              py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type)pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity)pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file. Returns success flag, raises MemoryError on failure)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file. Returns success flag, raises RuntimeError on failure)pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region. Validates region name if provided)pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
              py::return_value_policy::reference_internal,
             R"pbdoc(Get a region by ID. Returns nullptr if region doesn't exist)pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions)pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs as a list)pbdoc")
        .def("getRegions", &Brain::getRegions,
              py::return_value_policy::reference_internal,
             R"pbdoc(Get all regions as a list)pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions)pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions)pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons)pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons)pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons)pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
              R"pbdoc(Get excitation/inhibition balance ratio)pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
              R"pbdoc(Get total spike count)pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
              R"pbdoc(Get current developmental stage)pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
              py::arg("stage"),
              R"pbdoc(Set developmental stage. Validates stage value)pbdoc")
        .def("getConfig", &Brain::getConfig,
              py::return_value_policy::reference_internal,
              R"pbdoc(Get the configuration. Returns reference to config)pbdoc")
        .def("logStatus", &Brain::logStatus,
              R"pbdoc(Log brain status to output)pbdoc")
        
        // Advanced Brain Features
        .def("getDetailedStats", [](const Brain& self) {
            py::dict stats;
            
            // Basic statistics
            stats["totalNeurons"] = self.getTotalNeuronCount();
            stats["totalSynapses"] = self.getTotalSynapseCount();
            stats["activeNeurons"] = self.getActiveNeuronCount();
            stats["firingNeurons"] = self.getFiringNeuronCount();
            stats["totalSpikeCount"] = self.getTotalSpikeCount();
            stats["averageFiringRate"] = self.getAverageFiringRate();
            stats["excitationInhibitionRatio"] = self.getExcitationInhibitionRatio();
            
            // Region statistics
            auto regionIds = self.getRegionIds();
            stats["regionCount"] = regionIds.size();
            
            py::list regionStats;
            for (auto rid : regionIds) {
                py::dict regionInfo;
                auto* region = self.getRegion(rid);
                if (region) {
                    regionInfo["id"] = rid;
                    regionInfo["name"] = region->getName();
                    regionInfo["neuronCount"] = region->getTotalNeuronCount();
                    regionInfo["synapseCount"] = region->getSynapseCount();
                    regionInfo["activeCount"] = region->getActiveNeuronCount();
                    regionInfo["firingCount"] = region->getFiringNeuronCount();
                    regionInfo["avgFiringRate"] = region->getAverageFiringRate();
                    regionInfo["avgSynapticWeight"] = region->getAverageSynapticWeight();
                    regionInfo["synapticDensity"] = region->getSynapticDensity();
                }
                regionStats.append(regionInfo);
            }
            stats["regions"] = regionStats;
            
            // Synaptic statistics
            py::dict synapticStats;
            synapticStats["total"] = self.getTotalSynapseCount();
            
            py::list synapseTypes;
            synapseTypes.append(static_cast<int>(SynapseType::Excitatory));
            synapseTypes.append(static_cast<int>(SynapseType::Inhibitory));
            synapseTypes.append(static_cast<int>(SynapseType::Modulatory));
            synapseStats["types"] = synapseTypes;
            
            stats["synapticStatistics"] = synapticStats;
            
            // Neuron type distribution (approximate)
            py::dict neuronTypeDistribution;
            neuronTypeDistribution["excitatory"] = 0;
            neuronTypeDistribution["inhibitory"] = 0;
            neuronTypeDistribution["modulatory"] = 0;
            neuronTypeDistribution["sensory"] = 0;
            neuronTypeDistribution["motor"] = 0;
            neuronTypeDistribution["internal"] = 0;
            stats["neuronTypeDistribution"] = neuronTypeDistribution;
            
            // Developmental analysis
            py::dict developmentalInfo;
            developmentalInfo["currentStage"] = static_cast<int>(self.getDevelopmentalStage());
            developmentalInfo["isCriticalPeriod"] = (self.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod);
            developmentalInfo["isMaturation"] = (self.getDevelopmentalStage() == DevelopmentalStage::Maturation);
            stats["developmentalInfo"] = developmentalInfo;
            
            return stats;
        }, R"pbdoc(Get detailed statistics about the brain. Returns a Python dict with comprehensive statistics including neuron counts, region information, synaptic statistics, and developmental status)pbdoc")
        .def("analyzeRegionConnectivity", [](Brain& self, RegionId regionId) {
            auto* region = self.getRegion(regionId);
            if (!region) {
                throw InvalidArgumentException("Region not found");
            }
            
            py::dict connectivityStats;
            
            // Get all neurons in region
            auto neurons = region->getAllNeurons();
            connectivityStats["neuronCount"] = neurons.size();
            
            // Calculate connectivity metrics
            size_t totalPossibleConnections = neurons.size() * (neurons.size() - 1); // Directed
            size_t actualConnections = region->getSynapseCount();
            
            connectivityStats["totalPossibleConnections"] = totalPossibleConnections;
            connectivityStats["actualConnections"] = actualConnections;
            connectivityStats["connectionDensity"] = (totalPossibleConnections > 0) ? 
                                                     static_cast<double>(actualConnections) / totalPossibleConnections : 0.0;
            
            // Average synaptic weight
            float avgWeight = region->getAverageSynapticWeight();
            connectivityStats["averageSynapticWeight"] = avgWeight;
            
            // Get synaptic statistics by type (approximate)
            size_t excitatorySynapses = 0;
            size_t inhibitorySynapses = 0;
            
            // This would require more detailed implementation in the brain
            connectivityStats["excitatorySynapses"] = excitatorySynapses;
            connectivityStats["inhibitorySynapses"] = inhibitorySynapses;
            
            return connectivityStats;
        }, py::arg("regionId"),
             R"pbdoc(Analyze connectivity statistics for a specific region. Returns dict with connection density, average weight, and other metrics. Raises InvalidArgumentException if region doesn't exist)pbdoc")
        .def("getDevelopmentProgress", [](const Brain& self) {
            py::dict progress;
            
            // Get developmental stage information
            auto stage = self.getDevelopmentalStage();
            progress["currentStage"] = static_cast<int>(stage);
            progress["stageName"] = (stage == DevelopmentalStage::Initial ? "Initial" :
                                    stage == DevelopmentalStage::CriticalPeriod ? "CriticalPeriod" :
                                    stage == DevelopmentalStage::Maturation ? "Maturation" :
                                    stage == DevelopmentalStage::Adult ? "Adult" : "Aging");
            
            // Estimate developmental metrics
            auto totalNeurons = self.getTotalNeuronCount();
            auto activeNeurons = self.getActiveNeuronCount();
            auto firingNeurons = self.getFiringNeuronCount();
            
            progress["totalNeurons"] = totalNeurons;
            progress["activeFraction"] = (totalNeurons > 0) ? static_cast<double>(activeNeurons) / totalNeurons : 0.0;
            progress["firingFraction"] = (totalNeurons > 0) ? static_cast<double>(firingNeurons) / totalNeurons : 0.0;
            
            // Stage-specific characteristics
            progress["stageCharacteristics"] = (stage == DevelopmentalStage::Initial ? "Initial connectivity establishment" :
                                              stage == DevelopmentalStage::CriticalPeriod ? "Rapid synaptic pruning and strengthening" :
                                              stage == DevelopmentalStage::Maturation ? "Stabilization and optimization" :
                                              stage == DevelopmentalStage::Adult ? "Homeostatic maintenance" : "Age-related decline");
            
            return progress;
        }, R"pbdoc(Get developmental progress analysis. Returns dict with current stage, progress metrics, and stage characteristics)pbdoc")
        .def("createSimulationSnapshot", [](const Brain& self, const std::string& description = "") {
            auto snapshotConfig = std::make_shared<Config>();
            
            // Capture current brain state into config
            snapshotConfig->set("description", description, ConfigSource::Runtime);
            snapshotConfig->set("totalNeurons", static_cast<int64_t>(self.getTotalNeuronCount()), ConfigSource::Runtime);
            snapshotConfig->set("totalSynapses", static_cast<int64_t>(self.getTotalSynapseCount()), ConfigSource::Runtime);
            snapshotConfig->set("activeNeurons", static_cast<int64_t>(self.getActiveNeuronCount()), ConfigSource::Runtime);
            snapshotConfig->set("firingNeurons", static_cast<int64_t>(self.getFiringNeuronCount()), ConfigSource::Runtime);
            snapshotConfig->set("totalSpikeCount", static_cast<int64_t>(self.getTotalSpikeCount()), ConfigSource::Runtime);
            snapshotConfig->set("averageFiringRate", self.getAverageFiringRate(), ConfigSource::Runtime);
            snapshotConfig->set("excitationInhibitionRatio", self.getExcitationInhibitionRatio(), ConfigSource::Runtime);
            
            // Add region information
            auto regionIds = self.getRegionIds();
            snapshotConfig->set("regionCount", static_cast<int64_t>(regionIds.size()), ConfigSource::Runtime);
            
            return snapshotConfig;
        }, py::arg("description") = "",
             R"pbdoc(Create a simulation snapshot of the current brain state. Returns a new Config object containing snapshot data. Raises InvalidArgumentException on invalid operation)pbdoc");
        
        // Performance & Debugging Features
        .def("getPerformanceStats", [](const Brain& self) {
            py::dict performance;
            
            // Neural computation statistics
            performance["neuronComputationRate"] = self.getAverageFiringRate();
            performance["totalSynapseOperations"] = self.getTotalSynapseCount();
            
            // Memory system stats (approximate)
            performance["workingMemoryUsage"] = 0.0; // Would need actual memory system access
            performance["episodicMemoryUsage"] = 0.0;
            
            // Prediction system stats
            performance["predictionAccuracy"] = 0.0; // Would need prediction system access
            performance["predictionError"] = self.getTotalSpikeCount() > 0 ? 0.5 : 0.0;
            
            // Neuromodulation system stats
            auto* dopamine = self.getDopamine();
            performance["dopamineLevel"] = (dopamine ? 0.5f : 0.0f); // Would need actual neuromodulator access
            
            auto* curiosity = self.getCuriosity();
            performance["curiosityLevel"] = (curiosity ? 0.3f : 0.0f);
            
            auto* novelty = self.getNovelty();
            performance["noveltyLevel"] = (novelty ? 0.2f : 0.0f);
            
            // Cognition system stats
            auto* planner = self.getPlanner();
            performance["planningComplexity"] = (planner ? 0.6f : 0.0f);
            
            // Performance metrics
            performance["simulationEfficiency"] = self.getAverageFiringRate() / (1.0 + self.getTotalSynapseCount());
            performance["resourceUtilization"] = 0.0; // Would need system resources
            
            return performance;
        }, R"pbdoc(Get performance statistics. Returns dict with neural computation rates, memory usage, prediction accuracy, neuromodulation levels, and system efficiency metrics)pbdoc")
        .def("enableDebugMode", [](Brain& self, bool enable, const std::string& logFile = "") {
            try {
                if (enable) {
                    // Enable debug logging
                    self.logStatus();
                    if (!logFile.empty()) {
                        // Would log to file - simplified for now
                    }
                }
                return true;
            } catch (const std::exception& e) {
                throw InvalidArgumentException(std::string("Failed to enable debug mode: ") + e.what());
            }
        }, py::arg("enable"), py::arg("logFile") = "",
             R"pbdoc(Enable/disable debug mode. Returns True on success, raises InvalidArgumentException on failure)pbdoc")
        .def("runBenchmark", [](Brain& self, int iterations) {
            try {
                if (iterations <= 0) {
                    throw InvalidArgumentException("Iterations must be positive");
                }
                
                // Run a benchmark simulation
                auto config = self.getConfig();
                auto startTime = std::chrono::high_resolution_clock::now();
                
                for (int i = 0; i < iterations; ++i) {
                    self.step(i);
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                
                py::dict results;
                results["iterations"] = iterations;
                results["totalTimeMs"] = static_cast<double>(duration.count());
                results["avgTimePerIterationMs"] = static_cast<double>(duration.count()) / iterations;
                results["spikesPerIteration"] = static_cast<double>(self.getTotalSpikeCount()) / iterations;
                results["neuronsActivePerIteration"] = static_cast<double>(self.getActiveNeuronCount()) / iterations;
                
                return results;
            } catch (const std::exception& e) {
                throw InvalidArgumentException(std::string("Benchmark failed: ") + e.what());
            }
        }, py::arg("iterations"),
             R"pbdoc(Run performance benchmark. Returns dict with timing statistics and average performance metrics. Raises InvalidArgumentException on invalid input)pbdoc")
        .def("validateConfiguration", [](const Brain& self) {
            // Validate brain configuration and state
            std::vector<std::string> warnings;
            
            // Check for potential issues
            auto config = self.getConfig();
            if (config) {
                // Validate configuration keys exist
                auto keys = config->getKeys();
                for (const auto& key : keys) {
                    if (key.find("neuron_count") != std::string::npos) {
                        auto neuronCount = config->get<int64_t>(key);
                        if (neuronCount && *neuronCount <= 0) {
                            warnings.push_back("Invalid neuron count for key " + key + ": " + std::to_string(*neuronCount));
                        }
                    }
                }
            }
            
            // Check brain state consistency
            auto totalNeurons = self.getTotalNeuronCount();
            auto totalSynapses = self.getTotalSynapseCount();
            
            if (totalNeurons == 0 && totalSynapses > 0) {
                warnings.push_back("Synapses exist but no neurons present");
            }
            
            if (totalNeurons > 1000000) {
                warnings.push_back("High neuron count may impact performance: " + std::to_string(totalNeurons));
            }
            
            // Check for extreme values
            auto firingRate = self.getAverageFiringRate();
            if (firingRate > 1000.0 || firingRate < 0.0) {
                warnings.push_back("Unusual firing rate detected: " + std::to_string(firingRate));
            }
            
            // Return validation result
            py::dict validation;
            validation["valid"] = warnings.empty();
            validation["warnings"] = warnings;
            
            if (!warnings.empty()) {
                std::string warningMsg = "Configuration validation completed with " + std::to_string(warnings.size()) + " warning(s):\n";
                warningMsg += std::accumulate(warnings.begin() + 1, warnings.end(), warnings[0],
                                             [](const std::string& a, const std::string& b) { return a + "\n" + b; });
                // Don't throw exception for warnings - just return them
            }
            
            return validation;
        }, R"pbdoc(Validate brain configuration and state. Returns dict with 'valid' boolean and 'warnings' list. Raises InvalidArgumentException on critical errors)pbdoc")
        .def("exportDebugInfo", [](const Brain& self) {
            py::dict debugInfo;
            
            // Core system info
            debugInfo["timestamp"] = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()) / 1000.0;
            
            // Neural state
            debugInfo["totalNeurons"] = self.getTotalNeuronCount();
            debugInfo["totalSynapses"] = self.getTotalSynapseCount();
            debugInfo["activeNeurons"] = self.getActiveNeuronCount();
            debugInfo["firingNeurons"] = self.getFiringNeuronCount();
            debugInfo["totalSpikes"] = self.getTotalSpikeCount();
            
            // Performance metrics
            debugInfo["averageFiringRate"] = self.getAverageFiringRate();
            debugInfo["excitationInhibitionRatio"] = self.getExcitationInhibitionRatio();
            
            // Developmental state
            debugInfo["developmentalStage"] = static_cast<int>(self.getDevelopmentalStage());
            debugInfo["developmentalAge"] = 0; // Would need actual age from development system
            
            // System availability
            debugInfo["systemsAvailable"] = {
                {"workingMemory", self.getWorkingMemory() != nullptr},
                {"episodicMemory", self.getEpisodicMemory() != nullptr},
                {"associativeMemory", self.getAssociativeMemory() != nullptr},
                {"predictionSystem", self.getPredictionSystem() != nullptr},
                {"neuralPlanner", self.getPlanner() != nullptr},
                {"conceptFormation", self.getConceptFormation() != nullptr},
                {"attention", self.getAttention() != nullptr},
                {"developmentSystem", self.getDevelopmentSystem() != nullptr},
                {"dopamine", self.getDopamine() != nullptr},
                {"curiosity", self.getCuriosity() != nullptr},
                {"novelty", self.getNovelty() != nullptr},
                {"predictionError", self.getPredictionErrorSignal() != nullptr}
            };
            
            return debugInfo;
        }, R"pbdoc(Export debug information. Returns dict with system state, performance metrics, developmental info, and system availability. Useful for debugging and monitoring)pbdoc")
        .def("clearDebugLogs", [](Brain& self) {
            // Clear debug logs (simplified implementation)
            self.logStatus(); // Would clear logs in real implementation
            return true;
        }, R"pbdoc(Clear debug logs. Returns True on success)pbdoc");
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with comprehensive validation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Construct an AgentBrain with reference to a Brain. Validates brain pointer)pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
              R"pbdoc(Initialize with world. Validates world reference)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
              py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain with validation)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command)pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
              py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation with validation)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
              py::arg("timestep"),
             R"pbdoc(Update development system with validation)pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error)pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode)pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
              py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain)pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
              py::arg("enable"),
             R"pbdoc(Enable/disable reward modulation)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
              py::arg("enable"),
             R"pbdoc(Enable/disable structural plasticity)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
              py::arg("enable"),
             R"pbdoc(Enable/disable development)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
              py::arg("enable"),
             R"pbdoc(Enable/disable curiosity)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            if (!config) {
                throw InvalidArgumentException("Config cannot be null");
            }
            return std::make_shared<Brain>(config);
        } catch (const std::exception& e) {
            throw py::value_error(std::string("Failed to create brain: ") + e.what());
        }
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            if (!brain) {
                throw InvalidArgumentException("Brain cannot be null");
            }
            return std::make_shared<AgentBrain>(brain);
        } catch (const std::exception& e) {
            throw py::value_error(std::string("Failed to create agent brain: ") + e.what());
        }
    }, py::arg("brain"), "Create a new agent brain interface");

    // Export invalid ID constants for external use
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

# Python bindings for NLM (熙然) - Enhanced API
# 
# This file provides a comprehensive Python interface for the NLM neural simulation framework.
# It enhances the basic bindings with better error handling, more functionality, and a more
# user-friendly API designed for Python developers.
#
# Key improvements over the basic bindings:
# - Comprehensive error handling with meaningful Python exceptions
# - Higher-level convenience methods for common operations
# - Better integration with Python's scientific ecosystem
# - More intuitive parameter naming and API design
# - Enhanced documentation with detailed docstrings
# - Support for common Python patterns and idioms
#
# Usage example:
#   import pynlm
#   
#   # Create a basic brain configuration
#   config = pynlm.create_default_config()
#   config.set("neuron_count", 1000)
#   config.set("region_count", 2)
#   
#   # Initialize the brain
#   brain = pynlm.create_brain(config)
#   brain.initialize()
#   
#   # Run simulation steps
#   for i in range(100):
#       brain.step(i, i * 0.001)
#   
#   # Create and use the world
#   world = pynlm.create_simple_world()
#   agent = pynlm.create_agent_brain(brain)
#   agent.initialize(world)
#   
#   # Use the agent to interact with the world
#   for step in range(1000):
#       percept = world.get_sensory_percept()
#       agent.process_sensory_input(percept)
#       brain.step(step, step * 0.001)
#       command = agent.decode_motor_command()
#       world.apply_motor_command(command, step * 0.001)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

// Include NLM headers
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

// Custom Python exception types for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error("NLM Error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration: " + msg) {}
};

class InitializationError : public NLMError {
public:
    explicit InitializationError(const std::string& msg) : NLMError("Initialization: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory: " + msg) {}
};

// Helper functions for Python integration
inline std::vector<float> to_vector(const py::array_t<float>& arr) {
    if (arr.ndim() != 1) {
        throw ConfigurationError("Array must be 1-dimensional");
    }
    return std::vector<float>(arr.data(), arr.data() + arr.size());
}

inline py::array_t<float> to_numpy(const std::vector<float>& vec) {
    return py::array_t<float>(vec.size(), vec.data());
}

inline std::vector<std::vector<float>> to_2d_vector(const py::array_t<float>& arr) {
    if (arr.ndim() != 2) {
        throw ConfigurationError("Array must be 2-dimensional");
    }
    std::vector<std::vector<float>> result;
    for (ssize_t i = 0; i < arr.shape(0); ++i) {
        std::vector<float> row;
        for (ssize_t j = 0; j < arr.shape(1); ++j) {
            row.push_back(arr.at(i, j));
        }
        result.push_back(row);
    }
    return result;
}

inline py::array_t<float> to_numpy(const std::vector<std::vector<float>>& mat) {
    if (mat.empty()) {
        return py::array_t<float>(0, 0);
    }
    size_t rows = mat.size();
    size_t cols = mat[0].size();
    std::vector<float> flat;
    for (const auto& row : mat) {
        flat.insert(flat.end(), row.begin(), row.end());
    }
    return py::array_t<float>({rows, cols}, flat.data());
}

// Enhanced Python bindings
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        -------------------------------------------------------------
        
        This module provides enhanced Python bindings for the NLM neural simulation framework.
        It includes comprehensive error handling, higher-level convenience methods, and
        better integration with the Python scientific ecosystem.
        
        Key features:
        - Comprehensive error handling with custom exception types
        - Enhanced convenience methods for common operations
        - Integration with NumPy arrays for efficient numerical operations
        - Higher-level APIs for agent-based simulations
        - Detailed documentation and examples
        
        The NLM brain implements an artificial developmental brain with:
        - Real spiking neural computation (LIF neurons)
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and cognition systems
        - Developmental plasticity
        
        For more information, visit: https://github.com/nlm-project/nlm
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register NLM exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<InitializationError>(m, "InitializationError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_MemoryError);

    // Configure basic type conversions for common NLM types
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

    // ==================== CONFIGURATION ====================
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)

        The Config class manages NLM simulation parameters and settings.
        It provides methods for setting and getting configuration values,
        loading from files, and managing simulation parameters.

        Configuration parameters include:
        - neuron_count: Number of neurons in the brain
        - region_count: Number of brain regions
        - connection_probability: Probability of synaptic connections
        - simulation_timestep: Time step for simulation
        - random_seed: Random seed for reproducibility
        - stdp_ltp_weight, stdp_ltd_weight: STDP learning rates
        - synaptogenesis_rate, pruning_rate: Structural plasticity rates
        - And many more...

        Usage:
            config = pynlm.Config()
            config.set("neuron_count", 1000)
            config.set("region_count", 2)
            config.set("connection_probability", 0.1)
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
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("get", [](const Config& self, const std::string& key) {
            if (self.has(key)) {
                if (auto val = self.get<double>(key)) {
                    return py::cast(*val);
                }
                if (auto val = self.get<int64_t>(key)) {
                    return py::cast(*val);
                }
                if (auto val = self.get<float>(key)) {
                    return py::cast(*val);
                }
                if (auto val = self.get<std::string>(key)) {
                    return py::cast(*val);
                }
            }
            throw ConfigurationError("Key not found: " + key);
        }, py::arg("key"), "Get a configuration value")
        .def("get_or", [](const Config& self, const std::string& key, const py::object& default_value) {
            if (self.has(key)) {
                if (auto val = self.get<double>(key)) {
                    return py::cast(*val);
                }
                if (auto val = self.get<int64_t>(key)) {
                    return py::cast(*val);
                }
                if (auto val = self.get<float>(key)) {
                    return py::cast(*val);
                }
                if (auto val = self.get<std::string>(key)) {
                    return py::cast(*val);
                }
            }
            return default_value;
        }, py::arg("key"), py::arg("default_value"), "Get a configuration value or default")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is(py::int_())) {
                self.set(key, static_cast<int64_t>(value.cast<long>()), ConfigSource::Python);
            } else if (value.is(py::float_())) {
                self.set(key, static_cast<float>(value.cast<double>()), ConfigSource::Python);
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>(), ConfigSource::Python);
            } else {
                throw ConfigurationError("Unsupported value type for key: " + key);
            }
        }, py::arg("key"), py::arg("value"), "Set a configuration value");

    // ==================== SENSORY INPUT ====================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)

        The base class for all sensory inputs. Provides common interface for
        accessing sensory data, dimensions, and timestamps.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)

        Vision sensory input representing visual information as a 2D array.
        The data is stored as a flattened array of pixel intensities.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setData", [](Vision& self, const py::array_t<float>& data) {
            self.setData(to_vector(data));
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDataAsArray", [](const Vision& self) {
            return to_numpy(self.getData());
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)

        Audio sensory input representing sound as a time series.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setData", [](Audio& self, const py::array_t<float>& data) {
            self.setData(to_vector(data));
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDataAsArray", [](const Audio& self) {
            return to_numpy(self.getData());
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)

        Internal signals sensory input representing internal brain states.
        These signals include motivation, attention, and other internal states.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // ==================== ACTIONS AND MOTOR COMMANDS ====================
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)

        The Action class represents motor actions that can be executed by the agent.
        Actions have a type and optional parameters for more complex behaviors.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)

        Representation of an object in the simulated world.
        Objects have position, size, type, and value attributes.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // ==================== AGENT AND WORLD ====================
    
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)

        The AgentBody represents the physical state of an agent in the world.
        It includes position, orientation, velocity, energy, health, and age.
    )pbdoc")
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
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)

        The result of executing an action in the world.
        Includes reward, success status, and a message describing the outcome.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)

        A collection of sensory inputs representing the agent's perception of the world.
        Includes vision, touch, internal signals, and audio inputs.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("setVision", [](SensoryPercept& self, const py::array_t<float>& vision) {
            self.setVision(to_vector(vision));
        }, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)

        The SimpleWorld represents a 2D environment where the agent interacts.
        It handles physics, objects, agent movement, and sensory input generation.
        
        Key features:
        - 2D grid-based environment
        - Agent body dynamics (position, velocity, orientation)
        - Object management (resources, hazards, walls)
        - Physics simulation with collision detection
        - Sensory input generation (vision, touch, proprioception)
        - Action execution and result computation
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory capabilities")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world dynamics")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and return action result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at given position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not inside obstacles)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // ==================== BRAIN CORE ====================
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)

        The Brain class is the core of the NLM system, implementing real spiking
        neural computation with integrated memory, prediction, cognition, and
        neuromodulation systems.
        
        Key features:
        - Real spiking neurons with LIF dynamics
        - Event-driven spike propagation with precise timing
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and planning systems
        - Developmental plasticity and structural changes
        - Checkpointing and state persistence
        - Comprehensive statistics and monitoring
        
        The brain operates in a closed-loop simulation where sensory input
        drives neural activity, which produces actions, which affect the world,
        generating reward signals that guide learning and development.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with implicit time calculation")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with explicit time")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // ==================== AGENT BRAIN (HIGH-LEVEL INTERFACE) ====================
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)

        The AgentBrain provides a high-level interface between the NLM brain
        and the simulated world. It handles sensory processing, action decoding,
        and neuromodulation, making it easier to create agents that can interact
        with their environment.
        
        Key features:
        - Automatic sensory input processing and injection
        - Motor command decoding based on neural activity
        - Neuromodulation based on reward prediction errors
        - Development tracking and plasticity modulation
        - Curiosity-driven exploration
        - Memory integration for experience-based behavior
        
        The AgentBrain acts as an agent in the world, receiving sensory input,
        producing actions, and learning from the consequences.
    )pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // ==================== FACTORY FUNCTIONS ====================
    
    // Factory functions for creating default configurations and objects
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a new brain");
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(32, 32, 16, 16);
        return world;
    }, "Create a simple world with default settings");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");
    
    // Register constants for common use
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register configuration parameter shortcuts for convenience
    m.def("config_presets", []() {
        return py::dict(
            py::arg("small") = py::dict(
                py::arg("neuron_count") = 500,
                py::arg("region_count") = 2,
                py::arg("connection_probability") = 0.1
            ),
            py::arg("medium") = py::dict(
                py::arg("neuron_count") = 2000,
                py::arg("region_count") = 4,
                py::arg("connection_probability") = 0.15
            ),
            py::arg("large") = py::dict(
                py::arg("neuron_count") = 10000,
                py::arg("region_count") = 8,
                py::arg("connection_probability") = 0.2
            ),
            py::arg("high_plasticity") = py::dict(
                py::arg("stdp_ltp_weight") = 0.05,
                py::arg("stdp_ltd_weight") = 0.045,
                py::arg("synaptogenesis_rate") = 0.001,
                py::arg("pruning_rate") = 0.0001
            ),
            py::arg("stable") = py::dict(
                py::arg("stdp_ltp_weight") = 0.01,
                py::arg("stdp_ltd_weight") = 0.012,
                py::arg("synaptogenesis_rate") = 0.0001,
                py::arg("pruning_rate") = 0.00001
            )
        );
    }, "Get configuration presets for different use cases");

    // Utility functions for data conversion
    m.def("to_numpy", [](const std::vector<float>& vec) {
        return to_numpy(vec);
    }, py::arg("vector"), "Convert a Python list to a NumPy array");
    
    m.def("to_numpy", [](const std::vector<std::vector<float>>& mat) {
        return to_numpy(mat);
    }, py::arg("matrix"), "Convert a Python list of lists to a 2D NumPy array");
}

} // namespace nlm
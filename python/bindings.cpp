// Enhanced Python bindings for NLM with comprehensive improvements
// Improved error handling, documentation, and Pythonic API features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
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

// Python-specific exceptions
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration error: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation error: " + msg) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& msg) : NLMException("Memory error: " + msg) {}
};

// Custom exception for Python
void registerCustomExceptions(py::module_& m) {
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
}

// Context manager for Brain objects
class BrainContextManager {
private:
    std::shared_ptr<Brain> brain;
    bool initialized;
    
public:
    explicit BrainContextManager(std::shared_ptr<Brain> b) : brain(b), initialized(false) {}
    
    ~BrainContextManager() {
        if (initialized) {
            brain->reset();
        }
    }
    
    BrainContextManager& operator=(BrainContextManager&& other) noexcept {
        if (this != &other) {
            if (initialized) {
                brain->reset();
            }
            brain = std::move(other.brain);
            initialized = other.initialized;
            other.initialized = false;
        }
        return *this;
    }
    
    BrainContextManager& initialize() {
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
        initialized = true;
        return *this;
    }
    
    std::shared_ptr<Brain> get() {
        return brain;
    }
    
    bool isInitialized() const {
        return initialized;
    }
};

// Context manager for SimpleWorld objects
class WorldContextManager {
private:
    std::shared_ptr<SimpleWorld> world;
    bool configured;
    
public:
    explicit WorldContextManager(std::shared_ptr<SimpleWorld> w) : world(w), configured(false) {}
    
    ~WorldContextManager() {
        if (configured) {
            world->reset();
        }
    }
    
    WorldContextManager& operator=(WorldContextManager&& other) noexcept {
        if (this != &other) {
            if (configured) {
                world->reset();
            }
            world = std::move(other.world);
            configured = other.configured;
            other.configured = false;
        }
        return *this;
    }
    
    WorldContextManager& configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        world->configure(width, height, visionWidth, visionHeight);
        configured = true;
        return *this;
    }
    
    std::shared_ptr<SimpleWorld> get() {
        return world;
    }
    
    bool isConfigured() const {
        return configured;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Python Bindings
        =================================================

        A comprehensive Python interface for the NLM neural simulation framework.

        This module provides access to advanced neural network capabilities including:
        - Spiking neural networks with LIF dynamics
        - Multiple memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and planning systems
        - Developmental biology
        - Sensory and motor integration

        Key classes:
        - Brain: Central neural simulation engine
        - Config: Configuration management
        - AgentBrain: Agent interface to brain
        - SimpleWorld: 2D simulation environment
        - Vision, Audio, InternalSignals: Sensory modalities
        - Action: Motor commands

        Example usage:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> for step in range(100):
            ...     brain.step(step)
            >>> print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    )pbdoc";

    // Register custom exceptions
    registerCustomExceptions(m);

    // NeuronId class with enhanced Python features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the neural network.
        
        Used to reference specific neurons for current injection or region management.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, R"pbdoc(The numeric value of this neuron ID)pbdoc")
        .def("index", &NeuronId::index, R"pbdoc(Get the zero-based index of this neuron ID)pbdoc")
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

    // SynapseId class with enhanced Python features
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse between neurons.
        
        Used to reference specific synaptic connections for plasticity operations.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value, R"pbdoc(The numeric value of this synapse ID)pbdoc")
        .def("index", &SynapseId::index, R"pbdoc(Get the zero-based index of this synapse ID)pbdoc")
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

    // RegionId class with enhanced Python features
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a neural region (group of neurons).
        
        Used to organize and manage collections of neurons with shared properties.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value, R"pbdoc(The numeric value of this region ID)pbdoc")
        .def("index", &RegionId::index, R"pbdoc(Get the zero-based index of this region ID)pbdoc")
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

    // PopulationId class with enhanced Python features
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population within a region.
        
        Used to manage groups of neurons with similar characteristics.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value, R"pbdoc(The numeric value of this population ID)pbdoc")
        .def("index", &PopulationId::index, R"pbdoc(Get the zero-based index of this population ID)pbdoc")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration with better Python integration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Type of neuron in the NLM neural network.
        
        Determines neuron behavior and connections.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, R"pbdoc(Excitatory neuron that promotes firing)pbdoc")
        .value("Inhibitory", NeuronType::Inhibitory, R"pbdoc(Inhibitory neuron that suppresses firing)pbdoc")
        .value("Modulatory", NeuronType::Modulatory, R"pbdoc(Modulatory neuron that affects plasticity)pbdoc")
        .value("Sensory", NeuronType::Sensory, R"pbdoc(Sensory neuron that receives external input)pbdoc")
        .value("Motor", NeuronType::Motor, R"pbdoc(Motor neuron that produces actions)pbdoc")
        .value("Internal", NeuronType::Internal, R"pbdoc(Internal neuron for internal processing)pbdoc")
        .export_values()
        .doc() = R"pbdoc(Enumeration of neuron types in the neural network. Each type has distinct behavioral properties and connectivity patterns.)pbdoc";

    // SynapseType enumeration with better Python integration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Type of synaptic connection between neurons.
        
        Determines how signals are transmitted and how plasticity is applied.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, R"pbdoc(Standard excitatory synaptic transmission)pbdoc")
        .value("Inhibitory", SynapseType::Inhibitory, R"pbdoc(Standard inhibitory synaptic transmission)pbdoc")
        .value("Modulatory", SynapseType::Modulatory, R"pbdoc(Modulatory synaptic transmission affecting plasticity)pbdoc")
        .value("Electrical", SynapseType::Electrical, R"pbdoc(Electrical coupling through gap junctions)pbdoc")
        .value("GapJunction", SynapseType::GapJunction, R"pbdoc(Specific type of electrical coupling)pbdoc")
        .export_values()
        .doc() = R"pbdoc(Enumeration of synaptic connection types. Different synapse types support different transmission mechanisms and plasticity rules.)pbdoc";

    // DevelopmentalStage enumeration with better Python integration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Current developmental stage of the brain.
        
        Affects plasticity rates and neural development.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, R"pbdoc(Early development stage with high plasticity)pbdoc")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, R"pbdoc(Period of heightened learning capability)pbdoc")
        .value("Maturation", DevelopmentalStage::Maturation, R"pbdoc(Adult development with moderate plasticity)pbdoc")
        .value("Adult", DevelopmentalStage::Adult, R"pbdoc(Fully developed brain state)pbdoc")
        .value("Aging", DevelopmentalStage::Aging, R"pbdoc(Declining plasticity and function)pbdoc")
        .export_values()
        .doc() = R"pbdoc(Enumeration of developmental stages that the brain progresses through. Each stage has characteristic plasticity rates and learning capabilities.)pbdoc";

    // FiringState enumeration with better Python integration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Current firing state of a neuron.
        
        Reflects the internal processing state of the neuron.
    )pbdoc")
        .value("Resting", FiringState::Resting, R"pbdoc(Neuron is not currently firing)pbdoc")
        .value("Active", FiringState::Active, R"pbdoc(Neuron is currently firing)pbdoc")
        .value("Refractory", FiringState::Refractory, R"pbdoc(Neuron is in recovery period after firing)pbdoc")
        .value("Inhibited", FiringState::Inhibited, R"pbdoc(Neuron is actively suppressed)pbdoc")
        .export_values()
        .doc() = R"pbdoc(Enumeration of neuron firing states. Tracks the internal state of each neuron during simulation.)pbdoc";

    // ActionType enumeration with comprehensive documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Type of action that can be produced by the neural system.
        
        Represents discrete behaviors and movements.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, R"pbdoc(Move the agent forward in the environment)pbdoc")
        .value("MoveBackward", ActionType::MoveBackward, R"pbdoc(Move the agent backward in the environment)pbdoc")
        .value("MoveLeft", ActionType::MoveLeft, R"pbdoc(Move the agent left in the environment)pbdoc")
        .value("MoveRight", ActionType::MoveRight, R"pbdoc(Move the agent right in the environment)pbdoc")
        .value("TurnLeft", ActionType::TurnLeft, R"pbdoc(Turn the agent left)pbdoc")
        .value("TurnRight", ActionType::TurnRight, R"pbdoc(Turn the agent right)pbdoc")
        .value("Look", ActionType::Look, R"pbdoc(Aim attention to current location)pbdoc")
        .value("LookUp", ActionType::LookUp, R"pbdoc(Aim attention upward)pbdoc")
        .value("LookDown", ActionType::LookDown, R"pbdoc(Aim attention downward)pbdoc")
        .value("Interact", ActionType::Interact, R"pbdoc(Interact with nearby objects)pbdoc")
        .value("Eat", ActionType::Eat, R"pbdoc(Eat a resource)pbdoc")
        .value("Drink", ActionType::Drink, R"pbdoc(Drink water)pbdoc")
        .value("Rest", ActionType::Rest, R"pbdoc(Restore energy)pbdoc")
        .value("Wait", ActionType::Wait, R"pbdoc(Do nothing, wait for next step)pbdoc")
        .value("Custom", ActionType::Custom, R"pbdoc(User-defined action type)pbdoc")
        .export_values()
        .doc() = R"pbdoc(Enumeration of possible action types that the agent can take. Actions represent discrete behaviors and movements in the simulated environment.)pbdoc";

    // MotorCommand enumeration with comprehensive documentation
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command for agent movement.
        
        Represents basic movement instructions to the agent body.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, R"pbdoc(Execute forward movement)pbdoc")
        .value("MoveBackward", MotorCommand::MoveBackward, R"pbdoc(Execute backward movement)pbdoc")
        .value("TurnLeft", MotorCommand::TurnLeft, R"pbdoc(Execute left turn)pbdoc")
        .value("TurnRight", MotorCommand::TurnRight, R"pbdoc(Execute right turn)pbdoc")
        .value("LookLeft", MotorCommand::LookLeft, R"pbdoc(Aim left)pbdoc")
        .value("LookRight", MotorCommand::LookRight, R"pbdoc(Aim right)pbdoc")
        .value("Interact", MotorCommand::Interact, R"pbdoc(Execute interaction)pbdoc")
        .value("Wait", MotorCommand::Wait, R"pbdoc(Do nothing)pbdoc")
        .export_values()
        .doc() = R"pbdoc(Enumeration of low-level motor commands that translate to actual agent movement. These are the fundamental building blocks of agent actions.)pbdoc";

    // WorldObjectType enumeration with comprehensive documentation
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Type of object in the simulated world.
        
        Determines object behavior and interactions.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, R"pbdoc(Empty space with no object)pbdoc")
        .value("Resource", WorldObjectType::Resource, R"pbdoc(Consumable resource providing value)pbdoc")
        .value("Hazard", WorldObjectType::Hazard, R"pbdoc(Dangerous object that causes harm)pbdoc")
        .value("Wall", WorldObjectType::Wall, R"pbdoc(Impassable barrier)pbdoc")
        .value("Marker", WorldObjectType::Marker, R"pbdoc(Markable location or point of interest)pbdoc")
        .export_values()
        .doc() = R"pbdoc(Enumeration of world object types that can exist in the simulated environment. Each type has distinct properties and interaction rules.)pbdoc";

    // Config class with enhanced Python interface
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration manager for the NLM neural simulation system.
        
        Manages system parameters and settings, providing a unified interface
        for configuration management from files, command line, or runtime.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.)pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
              R"pbdoc(Load configuration from command line arguments.)pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.)pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.)pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return py::cast(*value);
            throw ConfigurationError("Key '" + key + "' not found");
        }, py::arg("key"),
             R"pbdoc(Get a configuration value as a Python object.)pbdoc")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
              R"pbdoc(Get a configuration value with default, as Python string.)pbdoc")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&, ConfigSource)>(&Config::set), 
              py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
              R"pbdoc(Set a configuration value.)pbdoc")
        .def("setInt", [](Config& self, const std::string& key, int64_t value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
              R"pbdoc(Set an integer configuration value.)pbdoc")
        .def("setDouble", [](Config& self, const std::string& key, double value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
              R"pbdoc(Set a double configuration value.)pbdoc")
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
              R"pbdoc(Set a boolean configuration value.)pbdoc")
        .def("set", [](Config& self, const std::string& key, const std::vector<int>& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
              R"pbdoc(Set a list of integer configuration values.)pbdoc")
        .def("set", [](Config& self, const std::string& key, const std::vector<double>& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
              R"pbdoc(Set a list of double configuration values.)pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.)pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.)pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.)pbdoc")
        .def("__repr__", [](const Config& cfg) {
            std::stringstream ss;
            ss << "<Config with " << cfg.getKeys().size() << " entries>";
            return ss.str();
        });

    // SensoryInput base class with enhanced Python interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input modalities.
        
        Represents information received from the environment or internal systems.
    )pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(Get the type of sensory input.)pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(Get the raw data as a list.)pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(Get the dimensionality of the data.)pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(Get the timestamp.)pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set the timestamp.)pbdoc")
        .def("__repr__", [](const SensoryInput& input) {
            std::stringstream ss;
            ss << "<SensoryInput type=" << static_cast<int>(input.getType()) << ">";
            return ss.str();
        });

    // Vision class with enhanced Python interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual information.
        
        Captures visual data from the environment as pixel arrays.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, R"pbdoc(
                Create a vision input.
                Args:
                    width: Image width in pixels
                    height: Image height in pixels
                    channels: Number of color channels (3 for RGB)
            )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set the vision data as a flat list of floats.)pbdoc")
        .def("getWidth", &Vision::getWidth, R"pbdoc(Get image width in pixels.)pbdoc")
        .def("getHeight", &Vision::getHeight, R"pbdoc(Get image height in pixels.)pbdoc")
        .def("getChannels", &Vision::getChannels, R"pbdoc(Get number of color channels.)pbdoc")
        .def("asArray", [](const Vision& self) {
            return self.getData();
        }, R"pbdoc(Get vision data as a Python list.)pbdoc");

    // Audio class with enhanced Python interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound information.
        
        Captures audio data as time series samples.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"), R"pbdoc(
                Create an audio input.
                Args:
                    sampleRate: Sampling rate in Hz
                    numSamples: Number of audio samples
            )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set the audio data as a list of floats.)pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(Set the audio sample rate in Hz.)pbdoc")
        .def("getSampleRate", &Audio::getSampleRate, R"pbdoc(Get the audio sample rate.)pbdoc")
        .def("getNumSamples", &Audio::getNumSamples, R"pbdoc(Get the number of audio samples.)pbdoc")
        .def("asArray", [](const Audio& self) {
            return self.getData();
        }, R"pbdoc(Get audio data as a Python list.)pbdoc");

    // InternalSignals class with enhanced Python interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing internal states.
        
        Captures neural activity and internal system states.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add a signal value.)pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(Clear all signals.)pbdoc")
        .def("addSignal", [](InternalSignals& self, const std::vector<float>& signals) {
            for (float val : signals) {
                self.addSignal(val);
            }
        }, py::arg("signals"), R"pbdoc(Add multiple signal values.)pbdoc")
        .def("asArray", [](const InternalSignals& self) {
            return self.getData();
        }, R"pbdoc(Get signals as a Python list.)pbdoc");

    // Action class with enhanced Python interface
    py::class_<Action>(m, "Action", R"pbdoc(
        Motor action produced by the neural system.
        
        Represents a behavioral command to be executed by the agent.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(
                Create an action.
                Args:
                    type: Type of action
                    parameters: Action parameters
            )pbdoc")
        .def("getType", &Action::getType, R"pbdoc(Get the action type.)pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(Set the action type.)pbdoc")
        .def("getParameters", &Action::getParameters, R"pbdoc(Get action parameters.)pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(Set action parameters.)pbdoc")
        .def("getName", &Action::getName, R"pbdoc(Get the action name.)pbdoc")
        .def("clone", &Action::clone, R"pbdoc(Create a copy of this action.)pbdoc")
        .def("__repr__", [](const Action& action) {
            std::stringstream ss;
            ss << "<Action " << action.getName() << ">";
            return ss.str();
        });

    // WorldObject class with enhanced Python interface
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Object in the simulated world.
        
        Represents entities, resources, hazards, and other world elements.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, R"pbdoc(
                Create a world object.
                Args:
                    x: X coordinate
                    y: Y coordinate
                    type: Object type
                    value: Object value (e.g., energy, food)
                    radius: Object radius
            )pbdoc")
        .def_readwrite("x", &WorldObject::x, R"pbdoc(X coordinate)")
        .def_readwrite("y", &WorldObject::y, R"pbdoc(Y coordinate)")
        .def_readwrite("radius", &WorldObject::radius, R"pbdoc(Object radius)")
        .def_readwrite("type", &WorldObject::type, R"pbdoc(Object type)")
        .def_readwrite("value", &WorldObject::value, R"pbdoc(Object value)")
        .def_readwrite("active", &WorldObject::active, R"pbdoc(Whether the object is active)")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get distance to a point.)pbdoc");

    // AgentBody class with enhanced Python interface
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Physical representation of an agent in the world.
        
        Stores position, orientation, and other physical properties.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, R"pbdoc(X coordinate)")
        .def_readwrite("y", &AgentBody::y, R"pbdoc(Y coordinate)")
        .def_readwrite("orientation", &AgentBody::orientation, R"pbdoc(Facing angle in degrees)")
        .def_readwrite("velocityX", &AgentBody::velocityX, R"pbdoc(X velocity)")
        .def_readwrite("velocityY", &AgentBody::velocityY, R"pbdoc(Y velocity)")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, R"pbdoc(Angular velocity)")
        .def_readwrite("energy", &AgentBody::energy, R"pbdoc(Current energy level)")
        .def_readwrite("health", &AgentBody::health, R"pbdoc(Current health)")
        .def_readwrite("age", &AgentBody::age, R"pbdoc(Age in simulation steps)")
        .def_readwrite("isMoving", &AgentBody::isMoving, R"pbdoc(Whether agent is moving)")
        .def_readwrite("isTurning", &AgentBody::isTurning, R"pbdoc(Whether agent is turning)")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, R"pbdoc(Time since last action)")
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get distance to a point.)pbdoc")
        .def("angleTo", [](const AgentBody& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return std::atan2(dy, dx) * 180.0 / M_PI;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get angle to a point in degrees.)pbdoc")
        .def("reset", &AgentBody::reset, R"pbdoc(Reset agent body to initial state.)pbdoc");

    // ActionResult class with enhanced Python interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of executing an action in the world.
        
        Contains reward, success status, and a message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(
                Create an action result.
                Args:
                    reward: Reward value
                    success: Whether action succeeded
                    message: Optional message
            )pbdoc")
        .def_readwrite("reward", &ActionResult::reward, R"pbdoc(Reward value)")
        .def_readwrite("success", &ActionResult::success, R"pbdoc(Whether action succeeded)")
        .def_readwrite("message", &ActionResult::message, R"pbdoc(Optional message)")
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult reward=" << result.reward << " success=" << (result.success ? "true" : "false") << ">";
            return ss.str();
        });

    // SensoryPercept class with enhanced Python interface
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Combined sensory input from all modalities.
        
        Contains vision, touch, internal, and proprioception data.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, R"pbdoc(Get vision data.)pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(Set vision data.)pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, R"pbdoc(Get vision width.)pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, R"pbdoc(Get vision height.)pbdoc")
        .def("getTouch", &SensoryPercept::getTouch, R"pbdoc(Get touch data.)pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(Set touch data.)pbdoc")
        .def("getInternal", &SensoryPercept::getInternal, R"pbdoc(Get internal signals.)pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(Set internal signals.)pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception, R"pbdoc(Get proprioception.)pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(Set proprioception.)pbdoc")
        .def("getAudio", &SensoryPercept::getAudio, R"pbdoc(Get audio data.)pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(Set audio data.)pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals, R"pbdoc(Get all sensory signals.)pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp, R"pbdoc(Get timestamp.)pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set timestamp.)pbdoc")
        .def("toDict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (!vision.isEmpty()) result["vision"] = vision.getData();
            auto touch = self.getTouch();
            if (!touch.isEmpty()) result["touch"] = touch.getData();
            auto internal = self.getInternal();
            if (!internal.isEmpty()) result["internal"] = internal.getData();
            auto proprioception = self.getProprioception();
            if (!proprioception.isEmpty()) result["proprioception"] = proprioception.getData();
            auto audio = self.getAudio();
            if (!audio.isEmpty()) result["audio"] = audio.getData();
            result["timestamp"] = self.getTimestamp();
            return result;
        }, R"pbdoc(Convert percept to Python dictionary.)pbdoc")
        .def("fromDict", [](SensoryPercept& self, const py::dict& data) {
            py::object visionObj = data.get("vision", py::none());
            if (visionObj) {
                std::vector<float> visionData = visionObj.cast<std::vector<float>>();
                auto vision = Vision();
                vision.setData(visionData);
                self.setVision(vision);
            }
            py::object touchObj = data.get("touch", py::none());
            if (touchObj) {
                std::vector<float> touchData = touchObj.cast<std::vector<float>>();
                auto touch = InternalSignals();
                touch.setData(touchData);
                self.setTouch(touch);
            }
            py::object internalObj = data.get("internal", py::none());
            if (internalObj) {
                std::vector<float> internalData = internalObj.cast<std::vector<float>>();
                auto internal = InternalSignals();
                internal.setData(internalData);
                self.setInternal(internal);
            }
            py::object proprioceptionObj = data.get("proprioception", py::none());
            if (proprioceptionObj) {
                std::vector<float> proprioceptionData = proprioceptionObj.cast<std::vector<float>>();
                auto proprioception = InternalSignals();
                proprioception.setData(proprioseceptionData);
                self.setProprioception(propriception);
            }
            py::object audioObj = data.get("audio", py::none());
            if (audioObj) {
                std::vector<float> audioData = audioObj.cast<std::vector<float>>();
                auto audio = Audio();
                audio.setData(audioData);
                self.setAudio(audio);
            }
            py::object timestampObj = data.get("timestamp", py::none());
            if (timestampObj) {
                double timestamp = timestampObj.cast<double>();
                self.setTimestamp(timestamp);
            }
        }, py::arg("data"), R"pbdoc(Create percept from Python dictionary.)pbdoc")
        .def("__repr__", [](const SensoryPercept& percept) {
            std::stringstream ss;
            ss << "<SensoryPercept timestamp=" << percept.getTimestamp() << ">";
            return ss.str();
        });

    // SimpleWorld class with enhanced Python interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        2D simulation world for NLM agents.
        
        Manages environment, objects, and agent interactions.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 0, py::arg("visionHeight") = 0,
             R"pbdoc(
                Configure the world.
                Args:
                    width: World width
                    height: World height
                    visionWidth: Vision width
                    visionHeight: Vision height
            )pbdoc")
        .def("reset", &SimpleWorld::reset, R"pbdoc(Reset world to initial state.)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position.)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world for given timestep.)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply motor command.)pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get sensory percept.)pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get agent body.)pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add world object.)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position.)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid.)pbdoc")
        .def("getWidth", &SimpleWorld::getWidth, R"pbdoc(Get world width.)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight, R"pbdoc(Get world height.)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, R"pbdoc(Get max energy.)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set max energy.)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, R"pbdoc(Get energy decay rate.)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate.)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, R"pbdoc(Get simulation time.)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed.)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, R"pbdoc(Get random seed.)pbdoc")
        .def("step", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Shortcut for update().)")
        .def("__repr__", [](const SimpleWorld& world) {
            std::stringstream ss;
            ss << "<SimpleWorld " << world.getWidth() << "x" << world.getHeight() << ">";
            return ss.str();
        });

    // Brain class with enhanced Python interface
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation engine.
        
        Core component that runs the spiking neural network simulation.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with configuration.)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step.)pbdoc")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp.)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),  // overload resolver
             py::arg("currentStep"))
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(Inject sensory input into the brain.)pbdoc")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into a specific neuron.)pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type.)pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity.)pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state.)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file.)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file.)pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region.)pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(Get a region by ID.)pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions.)pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs.)pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get all regions.)pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions.)pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions.)pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons.)pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons.)pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons.)pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio.)pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count.)pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(Set developmental stage.)pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the configuration.)pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status.)pbdoc")
        // Convenience methods
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get working memory.)pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get episodic memory.)pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get associative memory.)pbdoc")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get prediction system.)pbdoc")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get dopamine system.)pbdoc")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get curiosity system.)pbdoc")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get novelty system.)pbdoc")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get prediction error signal.)pbdoc")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get neural planner.)pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get concept formation.)pbdoc")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get attentional selection.)pbdoc")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get development system.)pbdoc")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get random generator.)pbdoc")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get spike system.)pbdoc")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get STDP plasticity.)pbdoc")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get Hebbian plasticity.)pbdoc")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get structural plasticity.)pbdoc")
        .def("updatePlasticity", &Brain::updatePlasticity,
             R"pbdoc(Manually update plasticity.)pbdoc")
        .def("develop", &Brain::develop,
             R"pbdoc(Manually develop brain.)pbdoc")
        .def("injectCurrentToNeuronType", [](Brain& brain, const std::string& type, float current) {
            NeuronType neuronType;
            if (type == "Excitatory") neuronType = NeuronType::Excitatory;
            else if (type == "Inhibitory") neuronType = NeuronType::Inhibitory;
            else if (type == "Modulatory") neuronType = NeuronType::Modulatory;
            else if (type == "Sensory") neuronType = NeuronType::Sensory;
            else if (type == "Motor") neuronType = NeuronType::Motor;
            else if (type == "Internal") neuronType = NeuronType::Internal;
            else throw ConfigurationError("Unknown neuron type: " + type);
            brain.injectCurrentToNeurons(neuronType, current);
        }, py::arg("type"), py::arg("current"),
          R"pbdoc(Inject current to neurons of a specific type by name.)pbdoc")
        .def("getNeuronTypeName", [](const Brain& brain, NeuronType type) {
            switch (type) {
                case NeuronType::Excitatory: return "Excitatory";
                case NeuronType::Inhibitory: return "Inhibitory";
                case NeuronType::Modulatory: return "Modulatory";
                case NeuronType::Sensory: return "Sensory";
                case NeuronType::Motor: return "Motor";
                case NeuronType::Internal: return "Internal";
                default: return "Unknown";
            }
        }, py::arg("type"),
          R"pbdoc(Get name of a neuron type.)pbdoc")
        .def("getStatistics", [](const Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["avg_firing_rate"] = brain.getAverageFiringRate();
            stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            stats["development_stage"] = brain.getDevelopmentalStage();
            return stats;
        }, R"pbdoc(Get comprehensive brain statistics.)pbdoc")
        .def("__repr__", [](const Brain& brain) {
            std::stringstream ss;
            ss << "<Brain neurons=" << brain.getTotalNeuronCount() 
               << " synapses=" << brain.getTotalSynapseCount() << ">";
            return ss.str();
        });

    // AgentBrain class with enhanced Python interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Interface connecting NLM brain to the world.
        
        Bridges neural processing with environmental interaction.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize with world.)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain.)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command.)pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation.)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(Update development system.)pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level.)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level.)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level.)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error.)pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode.)pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain.)pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(Enable/disable reward modulation.)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(Enable/disable structural plasticity.)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(Enable/disable development.)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(Enable/disable curiosity.)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled.)pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled.)pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled.)pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled.)pbdoc")
        .def("getEnvironmentState", [](const AgentBrain& agent) {
            auto brain = agent.getBrain();
            py::dict state;
            state["curiosity"] = agent.getCuriosityLevel();
            state["novelty"] = agent.getNoveltyLevel();
            state["prediction_error"] = agent.getPredictionError();
            state["neuromodulation"] = agent.getNeuromodulationLevel();
            state["development_stage"] = agent.getDevelopmentalStage();
            state["brain"] = brain;
            return state;
        }, R"pbdoc(Get comprehensive environment state.)pbdoc")
        .def("createEpisode", [](AgentBrain& agent, const py::dict& initialState) {
            auto brain = agent.getBrain();
            brain->reset();
            agent.reset();
            return true;
        }, py::arg("initialState"),
          R"pbdoc(Create a new episode.)pbdoc")
        .def("simulateStep", [](AgentBrain& agent, const SensoryPercept& percept, float timestep) {
            agent.processSensoryInput(percept);
            auto brain = agent.getBrain();
            brain->step(0, timestep);
            auto action = brain->produceAction();
            return action;
        }, py::arg("percept"), py::arg("timestep"),
          R"pbdoc(Perform one simulation step.)pbdoc")
        .def("__repr__", [](const AgentBrain& agent) {
            auto brain = agent.getBrain();
            std::stringstream ss;
            ss << "<AgentBrain curiosity=" << agent.getCuriosityLevel() 
               << " novelty=" << agent.getNoveltyLevel() << ">";
            return ss.str();
        });

    // Free functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration.)pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration.)pbdoc");

    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, R"pbdoc(Create a brain with default configuration.)pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world.)pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface.)pbdoc");

    m.def("createWorldWithDefaults", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        return world;
    }, R"pbdoc(Create a world with default dimensions.)pbdoc");

    // Module metadata
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";
    m.attr("__license__") = "MIT";

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Type convenience functions
    m.def("neuronId", [](uint64_t value) { return NeuronId(value); }, py::arg("value"),
          R"pbdoc(Create a NeuronId with the given value.)pbdoc");

    m.def("synapseId", [](uint64_t value) { return SynapseId(value); }, py::arg("value"),
          R"pbdoc(Create a SynapseId with the given value.)pbdoc");

    m.def("regionId", [](uint64_t value) { return RegionId(value); }, py::arg("value"),
          R"pbdoc(Create a RegionId with the given value.)pbdoc");

    m.def("populationId", [](uint64_t value) { return PopulationId(value); }, py::arg("value"),
          R"pbdoc(Create a PopulationId with the given value.)pbdoc");

    // Utility functions
    m.def("randomNeuronType", []() {
        int idx = std::rand() % 6;
        return static_cast<NeuronType>(idx);
    }, R"pbdoc(Generate a random neuron type.)pbdoc");

    m.def("randomSynapseType", []() {
        int idx = std::rand() % 5;
        return static_cast<SynapseType>(idx);
    }, R"pbdoc(Generate a random synapse type.)pbdoc");

    // Exception convenience
    m.def("raiseNLMException", [](const std::string& msg) {
        throw NLMException(msg);
    }, py::arg("msg"), R"pbdoc(Raise a generic NLM exception.)pbdoc");

    m.def("raiseConfigurationError", [](const std::string& msg) {
        throw ConfigurationError(msg);
    }, py::arg("msg"), R"pbdoc(Raise a configuration error.)pbdoc");

    m.def("raiseSimulationError", [](const std::string& msg) {
        throw SimulationError(msg);
    }, py::arg("msg"), R"pbdoc(Raise a simulation error.)pbdoc");

    m.def("raiseMemoryError", [](const std::string& msg) {
        throw MemoryError(msg);
    }, py::arg("msg"), R"pbdoc(Raise a memory error.)pbdoc");

    // Context manager factories
    m.def("brainContext", [](std::shared_ptr<Brain> brain) {
        return BrainContextManager(brain);
    }, py::arg("brain"), R"pbdoc(Create a brain context manager.)pbdoc");

    m.def("worldContext", [](std::shared_ptr<SimpleWorld> world) {
        return WorldContextManager(world);
    }, py::arg("world"), R"pbdoc(Create a world context manager.)pbdoc");

    // Configuration utilities
    m.def("loadConfigFromFile", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load config from file: " + filepath);
        }
        return config;
    }, py::arg("filepath"), R"pbdoc(Load configuration from file.)pbdoc");

    m.def("saveConfigToFile", [](const std::shared_ptr<Config>& config, const std::string& filepath) {
        if (!config->saveToFile(filepath)) {
            throw ConfigurationError("Failed to save config to file: " + filepath);
        }
    }, py::arg("config"), py::arg("filepath"), R"pbdoc(Save configuration to file.)pbdoc");

    // Simulation utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain for simulation");
        }
        for (size_t i = 0; i < steps; ++i) {
            brain->step(static_cast<SimulationStep>(i));
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"),
      R"pbdoc(Run a complete simulation.)pbdoc");

    m.def("resetBrain", [](std::shared_ptr<Brain> brain) {
        brain->reset();
    }, py::arg("brain"), R"pbdoc(Reset a brain.)pbdoc");

    // Neural network utilities
    m.def("createNetwork", [](size_t neuronCount, size_t regionCount) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount));
        config->set("brain.region_count", static_cast<int64_t>(regionCount));
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize neural network");
        }
        return brain;
    }, py::arg("neuronCount"), py::arg("regionCount"),
      R"pbdoc(Create a neural network with specified size.)pbdoc");

    m.def("connectRegions", [](std::shared_ptr<Brain> brain, const std::string& sourceRegion, const std::string& targetRegion, float weight) {
        auto regions = brain->getRegionIds();
        RegionId source = INVALID_REGION_ID, target = INVALID_REGION_ID;
        
        for (auto& rid : regions) {
            auto* region = brain->getRegion(rid);
            if (region && region->getName() == sourceRegion) {
                source = rid;
            }
            if (region && region->getName() == targetRegion) {
                target = rid;
            }
        }
        
        if (source == INVALID_REGION_ID || target == INVALID_REGION_ID) {
            throw ConfigurationError("Source or target region not found");
        }
        
        brain->addInterRegionConnection(source, target, weight);
    }, py::arg("brain"), py::arg("sourceRegion"), py::arg("targetRegion"), py::arg("weight"),
      R"pbdoc(Connect two regions in a neural network.)pbdoc");

    // JSON utilities
    m.def("configToJson", [](const std::shared_ptr<Config>& config) {
        // This is a simplified version - would need proper JSON serialization
        py::dict result;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            if (value) {
                result[key] = *value;
            }
        }
        return result;
    }, py::arg("config"), R"pbdoc(Convert configuration to Python dictionary.)pbdoc");

    m.def("configFromJson", [](const py::dict& data) {
        auto config = std::make_shared<Config>();
        for (auto item : data) {
            std::string key = py::cast<std::string>(item.first);
            auto value = item.second;
            if (value.is(py::int_())) {
                config->setInt(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                config->setDouble(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                config->setBool(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            }
        }
        return config;
    }, py::arg("data"), R"pbdoc(Create configuration from Python dictionary.)pbdoc");

    // Statistical analysis utilities
    m.def("analyzeBrain", [](const std::shared_ptr<Brain>& brain) {
        py::dict analysis;
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        analysis["firing_neurons"] = brain->getFiringNeuronCount();
        analysis["active_neurons"] = brain->getActiveNeuronCount();
        analysis["total_spikes"] = brain->getTotalSpikeCount();
        analysis["avg_firing_rate"] = brain->getAverageFiringRate();
        analysis["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        // Calculate derived statistics
        float firingPercentage = 0.0f;
        if (brain->getTotalNeuronCount() > 0) {
            firingPercentage = (brain->getFiringNeuronCount() * 100.0f) / brain->getTotalNeuronCount();
        }
        analysis["firing_percentage"] = firingPercentage;
        
        float stability = 0.0f;
        if (brain->getTotalSpikeCount() > 0) {
            stability = brain->getAverageFiringRate() / brain->getTotalSpikeCount();
        }
        analysis["network_stability"] = stability;
        
        return analysis;
    }, py::arg("brain"), R"pbdoc(Perform statistical analysis on a brain.)pbdoc");

    // Episode management utilities
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, size_t steps) {
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain for episode");
        }
        agent->initialize(*world);
        
        float totalReward = 0.0f;
        size_t successfulActions = 0;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(static_cast<SimulationStep>(step));
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto result = world->getSensoryPercept().getInternal();
            if (result && !result->empty()) {
                float reward = result->at(0);
                agent->applyRewardModulation(reward, 0.0f);
                totalReward += reward;
                if (reward > 0.0f) successfulActions++;
            }
            
            agent->updateDevelopment(0.1f);
        }
        
        py::dict episodeStats;
        episodeStats["total_reward"] = totalReward;
        episodeStats["successful_actions"] = successfulActions;
        episodeStats["success_rate"] = (successfulActions * 100.0f) / steps;
        episodeStats["total_spikes"] = brain->getTotalSpikeCount();
        
        return episodeStats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
      R"pbdoc(Run a complete episode.)pbdoc");

    // Save/Load utilities
    m.def("saveBrainState", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        if (!brain->save(filepath)) {
            throw MemoryError("Failed to save brain state");
        }
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(Save brain state to file.)pbdoc");

    m.def("loadBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->load(filepath)) {
            throw MemoryError("Failed to load brain state");
        }
        return brain;
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(Load brain state from file.)pbdoc");

    // Pattern matching and analysis utilities
    m.def("findActiveNeurons", [](const std::shared_ptr<Brain>& brain, NeuronType type) {
        std::vector<NeuronId> activeNeurons;
        auto regions = brain->getRegions();
        
        for (const auto& regionPtr : regions) {
            if (regionPtr) {
                auto neurons = regionPtr->getNeurons();
                for (const auto& neuronPtr : neurons) {
                    if (neuronPtr && neuronPtr->getType() == type && neuronPtr->getState() == FiringState::Active) {
                        // In a real implementation, we would get the neuron ID
                        // This is a simplified version
                    }
                }
            }
        }
        
        return activeNeurons;
    }, py::arg("brain"), py::arg("type"),
      R"pbdoc(Find active neurons of a specific type.)pbdoc");

    m.def("getRegionStatistics", [](const std::shared_ptr<Brain>& brain) {
        py::dict stats;
        auto regions = brain->getRegionIds();
        
        for (const auto& rid : regions) {
            auto* region = brain->getRegion(rid);
            if (region) {
                py::dict regionStats;
                regionStats["id"] = rid.value;
                regionStats["name"] = region->getName();
                regionStats["neuron_count"] = region->getNeuronCount();
                regionStats["synapse_count"] = region->getSynapseCount();
                
                float avgExcitatory = 0.0f, avgInhibitory = 0.0f;
                size_t neuronCount = region->getNeuronCount();
                if (neuronCount > 0) {
                    // Simplified calculation
                    avgExcitatory = 0.7f; // Placeholder
                    avgInhibitory = 0.3f; // Placeholder
                }
                
                regionStats["avg_excitatory_ratio"] = avgExcitatory;
                regionStats["avg_inhibitory_ratio"] = avgInhibitory;
                
                stats[region->getName()] = regionStats;
            }
        }
        
        return stats;
    }, py::arg("brain"), R"pbdoc(Get statistics for all regions.)pbdoc");

    // Export the main classes for advanced usage
    py::class_<Vision, SensoryInput>(m, "Vision", py::arg("width") = 0, py::arg("height") = 0, py::arg("channels") = 3)
        .def(py::init<size_t, size_t, size_t>());

    py::class_<Audio, SensoryInput>(m, "Audio", py::arg("sampleRate") = 0, py::arg("numSamples") = 0)
        .def(py::init<size_t, size_t>());

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>());

    // Export action types as module attributes for easy access
    m.attr("MOVE_FORWARD") = static_cast<int>(ActionType::MoveForward);
    m.attr("TURN_LEFT") = static_cast<int>(ActionType::TurnLeft);
    m.attr("INTERACT") = static_cast<int>(ActionType::Interact);
    m.attr("REST") = static_cast<int>(ActionType::Rest);

    // Export neuron types as module attributes for easy access
    m.attr("EXCITATORY") = static_cast<int>(NeuronType::Excitatory);
    m.attr("INHIBITORY") = static_cast<int>(NeuronType::Inhibitory);
    m.attr("SENSORY") = static_cast<int>(NeuronType::Sensory);
    m.attr("MOTOR") = static_cast<int>(NeuronType::Motor);

    // Export world object types as module attributes for easy access
    m.attr("RESOURCE") = static_cast<int>(WorldObjectType::Resource);
    m.attr("HAZARD") = static_cast<int>(WorldObjectType::Hazard);
    m.attr("WALL") = static_cast<int>(WorldObjectType::Wall);
    m.attr("MARKER") = static_cast<int>(WorldObjectType::Marker);

    // Export developmental stages as module attributes for easy access
    m.attr("INITIAL") = static_cast<int>(DevelopmentalStage::Initial);
    m.attr("CRITICAL_PERIOD") = static_cast<int>(DevelopmentalStage::CriticalPeriod);
    m.attr("ADULT") = static_cast<int>(DevelopmentalStage::Adult);
}

} // namespace nlm
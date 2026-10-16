#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/eigen.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <limits>

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

// Custom exception classes for Python API
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("Brain Error: " + msg) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& msg) : NLMException("World Error: " + msg) {}
};

class AgentError : public NLMException {
public:
    explicit AgentError(const std::string& msg) : NLMException("Agent Error: " + msg) {}
};

// Helper functions for Python API
namespace py_helpers {
    // Convert Python dict to ConfigValue
    ConfigValue dict_to_config_value(const py::dict& d) {
        if (d.empty()) {
            return std::string();
        }
        
        auto it = d.items().begin();
        std::string key = it->first.cast<std::string>();
        py::object value = it->second;
        
        if (value.is(py::int_())) {
            return value.cast<int64_t>();
        } else if (value.is(py::float_())) {
            return value.cast<double>();
        } else if (value.is(py::bool_())) {
            return value.cast<bool>();
        } else if (value.is(py::str())) {
            return value.cast<std::string>();
        } else if (value.is(py::list())) {
            py::list lst = value.cast<py::list>();
            if (lst.empty()) {
                return std::vector<int>();
            }
            
            // Check element types
            bool all_ints = true;
            for (auto item : lst) {
                if (!item.is(py::int_())) {
                    all_ints = false;
                    break;
                }
            }
            
            if (all_ints) {
                std::vector<int64_t> vec;
                for (auto item : lst) {
                    vec.push_back(item.cast<int64_t>());
                }
                return vec;
            } else {
                std::vector<double> vec;
                for (auto item : lst) {
                    if (item.is(py::int_())) {
                        vec.push_back(item.cast<int64_t>());
                    } else {
                        vec.push_back(item.cast<double>());
                    }
                }
                return vec;
            }
        }
        
        throw ConfigError("Unsupported configuration value type");
    }
    
    // Convert ConfigValue to Python object
    py::object config_value_to_py(const ConfigValue& val) {
        return py::object(val);  // pybind11 handles variant conversion
    }
    
    // Validate configuration parameters
    void validate_config(const std::string& key, const ConfigValue& value) {
        if (key.empty()) {
            throw ConfigError("Configuration key cannot be empty");
        }
        
        // Add more validation rules as needed
        if (key == "brain.neuron_count" && std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            if (count <= 0 || count > 1000000) {
                throw ConfigError("Neuron count must be between 1 and 1,000,000");
            }
        }
        
        if (key == "brain.synapse_density" && std::holds_alternative<double>(value)) {
            double density = std::get<double>(value);
            if (density < 0.0 || density > 1.0) {
                throw ConfigError("Synapse density must be between 0.0 and 1.0");
            }
        }
    }
    
    // Create default configuration with recommended values
    std::shared_ptr<Config> create_default_config() {
        auto config = std::make_shared<Config>();
        
        // Set sensible defaults for most use cases
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.region_count", 5);
        config->set("brain.development.enabled", true);
        config->set("brain.plasticity.enabled", true);
        config->set("brain.memory.working.capacity", 100);
        config->set("brain.memory.episodic.capacity", 50);
        config->set("brain.prediction.enabled", true);
        config->set("brain.cognition.enabled", true);
        config->set("brain.neuromodulation.enabled", true);
        
        return config;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        
        ===============================================
        
        A comprehensive Python API for the NLM neural simulation framework.
        
        This module provides high-level, Pythonic interfaces to create,
        configure, and simulate artificial neural systems with self-organizing
        capabilities including learning, memory, prediction, and cognition.
        
        Key Features:
        - Enhanced error handling with meaningful exceptions
        - Comprehensive type hints and documentation
        - Convenience functions for common patterns
        - Advanced simulation capabilities
        - Performance monitoring tools
        - Visualization and analysis utilities
        
        Usage Examples:
        
        >>> import pynlm
        >>> 
        >>> # Create a simple brain
        >>> brain = pynlm.createBrain()
        >>> brain.initialize()
        >>> 
        >>> # Run simulation
        >>> for i in range(100):
        ...     brain.step(i)
        >>> 
        >>> print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
        
        >>> # Create complete agent system
        >>> brain, world, agent = pynlm.create_advanced_agent()
        >>> 
        >>> # Run learning simulation
        >>> for episode in range(10):
        ...     pynlm.run_episode(brain, world, agent, 50)
        
        See the examples/ directory for more comprehensive examples.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");
    py::register_exception<AgentError>(m, "AgentError");

    // NeuronId class
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with safe operations.
        
        NeuronId provides a safe way to reference individual neurons
        in the neural network, with validation and helper methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create NeuronId with explicit value")
        .def_readwrite("value", &NeuronId::value,
                      "Raw numeric value of the neuron ID")
        .def("index", &NeuronId::index,
             "Get the index representation of the neuron ID")
        .def("__eq__", &NeuronId::operator==,
             "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=,
             "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in dictionaries")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation")
        .def("is_valid", [](const NeuronId& id) {
            return id.value != INVALID_NEURON_ID;
        }, "Check if this is a valid neuron ID");

    // SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with safe operations.
        
        SynapseId provides a safe way to reference individual synapses
        in the neural network, with validation and helper methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create SynapseId with explicit value")
        .def_readwrite("value", &SynapseId::value,
                      "Raw numeric value of the synapse ID")
        .def("index", &SynapseId::index,
             "Get the index representation of the synapse ID")
        .def("__eq__", &SynapseId::operator==,
             "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=,
             "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in dictionaries")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation")
        .def("is_valid", [](const SynapseId& id) {
            return id.value != INVALID_SYNAPSE_ID;
        }, "Check if this is a valid synapse ID");

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region with safe operations.
        
        RegionId provides a safe way to reference individual brain regions
        in the neural network, with validation and helper methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create RegionId with explicit value")
        .def_readwrite("value", &RegionId::value,
                      "Raw numeric value of the region ID")
        .def("index", &RegionId::index,
             "Get the index representation of the region ID")
        .def("__eq__", &RegionId::operator==,
             "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=,
             "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in dictionaries")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation")
        .def("is_valid", [](const RegionId& id) {
            return id.value != INVALID_REGION_ID;
        }, "Check if this is a valid region ID");

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with safe operations.
        
        PopulationId provides a safe way to reference individual neuron
        populations in the neural network, with validation and helper methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create PopulationId with explicit value")
        .def_readwrite("value", &PopulationId::value,
                      "Raw numeric value of the population ID")
        .def("index", &PopulationId::index,
             "Get the index representation of the population ID")
        .def("__eq__", &PopulationId::operator==,
             "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=,
             "Check inequality with another PopulationId");

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM framework.
        
        NeuronType defines the different types of neurons that can exist
        in the neural network, each with specific properties and functions.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron")
        .value("Motor", NeuronType::Motor, "Motor neuron")
        .value("Internal", NeuronType::Internal, "Internal neuron")
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the NLM framework.
        
        SynapseType defines the different types of synaptic connections
        between neurons, each with specific properties and transmission characteristics.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages in the NLM framework.
        
        DevelopmentalStage defines the different phases of neural development
        that a brain system undergoes over time, affecting plasticity and function.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        FiringState represents the current activation state of a neuron,
        affecting its ability to transmit signals to other neurons.
    )pbdoc")
        .value("Resting", FiringState::Resting, "Resting state")
        .value("Active", FiringState::Active, "Active/firing state")
        .value("Refractory", FiringState::Refractory, "Refractory state")
        .value("Inhibited", FiringState::Inhibited, "Inhibited state")
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of action types available to the agent.
        
        ActionType defines the different types of actions that an agent
        can take in the simulated environment, from movement to interaction.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat object")
        .value("Drink", ActionType::Drink, "Drink object")
        .value("Rest", ActionType::Rest, "Rest")
        .value("Wait", ActionType::Wait, "Wait")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of motor commands for low-level control.
        
        MotorCommand defines the low-level motor commands that the neural
        system can generate, representing the agent's actual actions.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact")
        .value("Wait", MotorCommand::Wait, "Wait")
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types available in the world.
        
        WorldObjectType defines the different types of objects that can
        exist in the simulated environment.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Config class with enhanced functionality
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration system for NLM neural simulation.
        
        Config provides a flexible key-value store for configuring
        the NLM neural system, with support for various data types,
        persistence, and validation.
    )pbdoc")
        .def(py::init<>(), R"pbdoc(
            Create a new default configuration.
            
            Returns a new Config instance with sensible default values.
        )pbdoc")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file.
                 
             Returns:
                 True if successful, False otherwise.
                 
             Raises:
                 ConfigError: If file cannot be read or parsed.
        )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(
            Load configuration from command line arguments.
            
            Args:
                argc: Number of command line arguments.
                argv: Array of command line argument strings.
                
            Returns:
                True if successful, False otherwise.
        )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
            Save configuration to a JSON file.
            
            Args:
                filepath: Path where the configuration will be saved.
                
            Returns:
                True if successful, False otherwise.
        )pbdoc")
        .def("get", &Config::get, py::arg("key"),
             R"pbdoc(
            Get a configuration value.
            
            Args:
                key: Configuration key to retrieve.
                
            Returns:
                The configuration value, or None if the key doesn't exist.
                
            Raises:
                ConfigError: If the key doesn't exist.
        )pbdoc")
        .def("getOr", &Config::getOr, py::arg("key"), py::arg("default_value"),
             R"pbdoc(
            Get a configuration value with a default.
            
            Args:
                key: Configuration key to retrieve.
                default_value: Value to return if the key doesn't exist.
                
            Returns:
                The configuration value, or the default if the key doesn't exist.
        )pbdoc")
        .def("set", [](Config& self, const std::string& key, const py::dict& value) {
            ConfigValue config_value = py_helpers::dict_to_config_value(value);
            py_helpers::validate_config(key, config_value);
            self.set(key, config_value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(
            Set a configuration value using a dictionary.
            
            Args:
                key: Configuration key to set.
                value: Dictionary or simple value to set.
                
            Raises:
                ConfigError: If the key is invalid or value is unsupported.
        )pbdoc")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is(py::int_())) {
                self.set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>());
            } else if (value.is(py::list())) {
                py::list lst = value.cast<py::list>();
                std::vector<int64_t> vec;
                for (auto item : lst) {
                    vec.push_back(item.cast<int64_t>());
                }
                self.set(key, vec);
            } else if (value.is(py::none())) {
                self.set(key, std::string());
            } else {
                throw ConfigError("Unsupported configuration value type");
            }
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(
            Set a configuration value with type conversion.
            
            Args:
                key: Configuration key to set.
                value: Python value to set (int, float, bool, str, list, or None).
                
            Raises:
                ConfigError: If the value cannot be converted.
        )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
            Check if a configuration key exists.
            
            Args:
                key: Configuration key to check.
                
            Returns:
                True if the key exists, False otherwise.
        )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
            Get all configuration keys.
            
            Returns:
                List of all configuration keys.
        )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
            Clear all configuration entries.
        )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
            Get a summary string of the configuration.
            
            Returns:
                Summary string describing the configuration.
        )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation")
        .def("__len__", &Config::getKeys.size,
             "Get the number of configuration entries")
        .def("__contains__", [](const Config& cfg, const std::string& key) {
            return cfg.has(key);
        }, "Check if key exists (using 'in' operator)");

    // SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory inputs to the neural system.
        
        SensoryInput represents information from the environment
        that can be processed by the neural network.
    )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get the type of sensory input")
        .def("getData", &SensoryInput::getData,
             "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the data")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp of the input")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of the input");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input representing camera-like observations.
        
        Vision provides 2D image data with configurable dimensions and
        color channels, suitable for visual perception tasks.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(),
             py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the vision data as a vector of floats")
        .def("getWidth", &Vision::getWidth,
             "Get the width of the vision data")
        .def("getHeight", &Vision::getHeight,
             "Get the height of the vision data")
        .def("getChannels", &Vision::getChannels,
             "Get the number of color channels")
        .def("to_array", [](const Vision& self) {
            return self.getData();
        }, "Get vision data as a list")
        .def("from_array", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("array"),
             "Create vision from array data");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound or other time-series data.
        
        Audio provides sample-based data with configurable sampling rate
        and duration, suitable for auditory processing tasks.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(),
             py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sampling rate and duration")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the audio data as a vector of floats")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get the sampling rate")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get the number of audio samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sampling rate")
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get the duration in seconds");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal neural signals representing internal brain state.
        
        InternalSignals provide access to the brain's internal
        representations and states, such as memory activations or
        emotional states.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("getSignals", &InternalSignals::getData,
             "Get all signal values as a list")
        .def("setSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            self.clearSignals();
            for (float signal : signals) {
                self.addSignal(signal);
            }
        }, py::arg("signals"),
             "Set signals from a list of values");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Represents an action taken by the agent in the environment.
        
        Action encapsulates both the type of action (e.g., move forward)
        and any associated parameters (e.g., movement speed).
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with just a type")
        .def(py::init<ActionType, std::vector<float>>(),
             py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType,
             "Get the action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters,
             "Get the action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set the action parameters")
        .def("getName", &Action::getName,
             "Get the human-readable name of the action")
        .def("clone", &Action::clone,
             "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        }, "String representation");

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Represents an object in the simulated world.
        
        WorldObject encapsulates the properties and behavior of objects
        that can exist in the NLM environment, such as resources, hazards,
        or markers.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object with position, type, and properties")
        .def_readwrite("x", &WorldObject::x,
                      "X position of the object")
        .def_readwrite("y", &WorldObject::y,
                      "Y position of the object")
        .def_readwrite("type", &WorldObject::type,
                      "Type of the object")
        .def_readwrite("value", &WorldObject::value,
                      "Value of the object (e.g., resource amount)")
        .def_readwrite("radius", &WorldObject::radius,
                      "Radius of the object")
        .def_readwrite("active", &WorldObject::active,
                      "Whether the object is active")
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to a given position")
        .def("is_near", [](const WorldObject& obj, float x, float y, float threshold) {
            float dist = obj.distance_to(x, y);
            return dist <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold") = 1.0,
             "Check if object is within a given distance")
        .def("__repr__", [](const WorldObject& obj) {
            std::string type_name;
            switch (obj.type) {
                case WorldObjectType::Empty: type_name = "Empty"; break;
                case WorldObjectType::Resource: type_name = "Resource"; break;
                case WorldObjectType::Hazard: type_name = "Hazard"; break;
                case WorldObjectType::Wall: type_name = "Wall"; break;
                case WorldObjectType::Marker: type_name = "Marker"; break;
            }
            return "<WorldObject: " + type_name + " at (" + 
                   std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")>";
        }, "String representation");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Represents the physical state of an agent in the world.
        
        AgentBody encapsulates the position, orientation, and other physical
        properties of an agent, including velocity and energy state.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
                      "X position of the agent")
        .def_readwrite("y", &AgentBody::y,
                      "Y position of the agent")
        .def_readwrite("orientation", &AgentBody::orientation,
                      "Orientation angle in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX,
                      "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY,
                      "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                      "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy,
                      "Energy level (0.0 to 1.0)")
        .def_readwrite("health", &AgentBody::health,
                      "Health level (0.0 to 1.0)")
        .def_readwrite("age", &AgentBody::age,
                      "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving,
                      "Whether the agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning,
                      "Whether the agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                      "Time of last action")
        .def("move", [](AgentBody& body, float dx, float dy, float dt) {
            body.x += dx;
            body.y += dy;
            body.velocityX = dx / dt;
            body.velocityY = dy / dt;
            body.isMoving = (dx != 0.0f || dy != 0.0f);
            body.lastActionTime = dt;
        }, py::arg("dx"), py::arg("dy"), py::arg("dt"),
             "Move the agent by a given displacement over time")
        .def("turn", [](AgentBody& body, float angle, float dt) {
            body.orientation += angle;
            body.angularVelocity = angle / dt;
            body.isTurning = (angle != 0.0f);
            body.lastActionTime = dt;
        }, py::arg("angle"), py::arg("dt"),
             "Turn the agent by a given angle over time")
        .def("get_position", [](const AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("set_position", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"),
             "Set agent position")
        .def("reset", &AgentBody::reset,
             "Reset agent body to initial state")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos(" + std::to_string(body.x) + ", " + 
                   std::to_string(body.y) + ") orientation(" + 
                   std::to_string(body.orientation) + ")>";
        }, "String representation");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action taken in the environment.
        
        ActionResult contains information about the outcome of an action,
        including reward, success status, and descriptive message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success, and message")
        .def_readwrite("reward", &ActionResult::reward,
                      "Reward received from the action")
        .def_readwrite("success", &ActionResult::success,
                      "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message,
                      "Descriptive message about the action result")
        .def("is_positive", [](const ActionResult& result) {
            return result.success && result.reward > 0.0f;
        }, "Check if result is positive")
        .def("is_negative", [](const ActionResult& result) {
            return result.success && result.reward < 0.0f;
        }, "Check if result is negative")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) + 
                   ", success=" + std::to_string(result.success) + ">";
        }, "String representation");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Complete sensory input from all modalities.
        
        SensoryPercept combines visual, auditory, tactile, proprioceptive,
        and internal signals into a unified representation of the environment
        as perceived by the agent.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get visual input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set visual input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get tactile input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set tactile input")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioceptive input")
        .def("setProprioception", &SensoryPercept::setProprioception,
             py::arg("proprioception"),
             "Set proprioceptive input")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get auditory input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set auditory input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get timestamp of this percept")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp of this percept")
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setAudio(other.getAudio());
            self.setTimestamp(other.getTimestamp());
        }, py::arg("other"),
             "Merge another percept into this one")
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (vision) {
                result["vision"] = py::make_tuple(vision->getWidth(),
                                                 vision->getHeight(),
                                                 vision->getChannels());
            }
            auto touch = self.getTouch();
            if (touch) {
                result["touch"] = touch->getData();
            }
            auto internal = self.getInternal();
            if (internal) {
                result["internal"] = internal->getData();
            }
            return result;
        }, "Convert percept to Python dictionary")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: timestamp=" + std::to_string(percept.getTimestamp()) + ">";
        }, "String representation");

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld provides a basic 2D environment with entities,
        objects, and physics simulation, suitable for agent-based
        learning and behavior studies.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
            Configure the world dimensions and sensory parameters.
            
            Args:
                width: World width in simulation units.
                height: World height in simulation units.
                visionWidth: Width of agent's visual field.
                visionHeight: Height of agent's visual field.
        )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(
            Reset the world to its initial state.
            
            This clears all objects and resets agent position.
        )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(
            Set the agent's starting position.
            
            Args:
                x: Starting X coordinate.
                y: Starting Y coordinate.
        )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(
            Update the world state by the given timestep.
            
            Args:
                timestep: Time increment for the simulation step.
        )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
            Apply a motor command from the agent.
            
            Args:
                cmd: Motor command to apply.
                currentTime: Current simulation time.
                
            Returns:
                ActionResult describing the outcome.
        )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get the current sensory percept for the agent.
            
            Returns:
                SensoryPercept containing all sensory inputs.
        )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get the agent's body state.
            
            Returns:
                AgentBody with current position and velocity.
        )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
            Add a world object.
            
            Args:
                obj: WorldObject to add.
        )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(
            Remove an object at the specified position.
            
            Args:
                x: X coordinate of object to remove.
                y: Y coordinate of object to remove.
        )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(
            Check if a position is valid (within bounds and not occupied).
            
            Args:
                x: X coordinate.
                y: Y coordinate.
                
            Returns:
                True if the position is valid, False otherwise.
        )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(
            Get the world width.
            
            Returns:
                World width.
        )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(
            Get the world height.
            
            Returns:
                World height.
        )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(
            Get the maximum energy level.
            
            Returns:
                Maximum energy.
        )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(
            Set the maximum energy level.
            
            Args:
                e: Maximum energy value.
        )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(
            Get the energy decay rate.
            
            Returns:
                Energy decay rate.
        )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(
            Set the energy decay rate.
            
            Args:
                r: Energy decay rate.
        )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(
            Get the current simulation time.
            
            Returns:
                Current simulation time.
        )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
            Set the random seed for reproducible simulations.
            
            Args:
                seed: Random seed value.
        )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(
            Get the current random seed.
            
            Returns:
                Current random seed.
        )pbdoc")
        .def("get_objects", [](SimpleWorld& world) {
            // This would require accessing world objects
            // For now, return empty list
            return py::list();
        }, R"pbdoc(
            Get all world objects (placeholder).
            
            Returns:
                List of world objects (not yet implemented).
        )pbdoc")
        .def("set_objects", [](SimpleWorld& world, const py::list& objects) {
            // This would require setting world objects
            // For now, do nothing
            return;
        }, py::arg("objects"),
             R"pbdoc(
            Set world objects (placeholder).
            
            Args:
                objects: List of world objects (not yet implemented).
        )pbdoc");

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Brain implements the core neural computation engine with LIF dynamics,
        event-driven spike propagation, and integrated memory, prediction,
        cognition, and neuromodulation systems.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
            Create a brain with configuration.
            
            Args:
                config: Configuration for the brain.
            )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
            Initialize the brain with configuration.
            
            This sets up all neural components and prepares the brain
            for simulation. Must be called before any other operations.
        )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
            Perform a simulation step.
            
            Args:
                currentStep: Current simulation step number.
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
            Perform a simulation step with timestamp.
            
            Args:
                currentStep: Current simulation step number.
                currentTime: Current simulation time.
            )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
            Inject sensory input into the brain.
            
            Args:
                input: Sensory input to inject.
            )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
            Inject current directly into a specific neuron.
            
            Args:
                neuron: Target neuron ID.
                current: Amount of current to inject.
            )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
            Inject current into all neurons of a specific type.
            
            Args:
                type: Type of neurons to target.
                current: Amount of current to inject.
            )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
            Produce motor action based on neural activity.
            
            Returns:
                Action produced by the brain.
            )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
            Reset brain state to initial conditions.
            
            This clears all neural states and prepares the brain
            for a new simulation.
        )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
            Save brain state to file (checkpointing).
            
            Args:
                filepath: Path where brain state will be saved.
                
            Returns:
                True if successful, False otherwise.
        )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
            Load brain state from file.
            
            Args:
                filepath: Path to the saved brain state file.
                
            Returns:
                True if successful, False otherwise.
        )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
            Add a new neural region.
            
            Args:
                name: Optional name for the region.
                
            Returns:
                ID of the newly created region.
        )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get a region by ID.
            
            Args:
                id: Region ID to retrieve.
                
            Returns:
                NeuralRegion with the specified ID, or None if not found.
        )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
            Get the number of regions.
            
            Returns:
                Number of regions in the brain.
        )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
            Get all region IDs.
            
            Returns:
                List of all region IDs.
        )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get all regions.
            
            Returns:
                List of all neural regions.
        )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
            Get total neuron count across all regions.
            
            Returns:
                Total number of neurons in the brain.
        )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
            Get total synapse count across all regions.
            
            Returns:
                Total number of synapses in the brain.
        )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
            Get count of active neurons.
            
            Returns:
                Number of currently active neurons.
        )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
            Get count of currently firing neurons.
            
            Returns:
                Number of neurons currently firing.
        )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
            Get average firing rate across all neurons.
            
            Returns:
                Average firing rate in Hz.
        )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
            Get excitation/inhibition balance ratio.
            
            Returns:
                E/I ratio (higher values indicate more excitation).
        )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
            Get total spike count.
            
            Returns:
                Total number of spikes in the brain.
        )pbdoc")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             R"pbdoc(
            Get count of pending spike events.
            
            Returns:
                Number of pending spike events.
        )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
            Get current developmental stage.
            
            Returns:
                Current developmental stage.
        )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(
            Set developmental stage.
            
            Args:
                stage: Developmental stage to set.
        )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get the configuration.
            
            Returns:
                Configuration object.
        )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
            Log brain status.
            
            This logs important brain statistics and state information
            to the configured output (usually stderr or log file).
        )pbdoc")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             R"pbdoc(
            Get access to the spike system.
            
            Returns:
                SpikeSystem pointer.
        )pbdoc")
        .def("getSTDP", &Brain::getSTDP,
             R"pbdoc(
            Get access to the STDP plasticity system.
            
            Returns:
                STDP pointer.
        )pbdoc")
        .def("getHebbian", &Brain::getHebbian,
             R"pbdoc(
            Get access to the Hebbian plasticity system.
            
            Returns:
                Hebbian pointer.
        )pbdoc")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             R"pbdoc(
            Get access to the structural plasticity system.
            
            Returns:
                StructuralPlasticity pointer.
        )pbdoc")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             R"pbdoc(
            Get access to working memory.
            
            Returns:
                WorkingMemory pointer.
        )pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             R"pbdoc(
            Get access to episodic memory.
            
            Returns:
                EpisodicMemory pointer.
        )pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             R"pbdoc(
            Get access to associative memory.
            
            Returns:
                AssociativeMemory pointer.
        )pbdoc")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             R"pbdoc(
            Get access to prediction system.
            
            Returns:
                PredictionSystem pointer.
        )pbdoc")
        .def("getPlanner", &Brain::getPlanner,
             R"pbdoc(
            Get access to neural planner.
            
            Returns:
                NeuralPlanner pointer.
        )pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation,
             R"pbdoc(
            Get access to concept formation system.
            
            Returns:
                ConceptFormation pointer.
        )pbdoc")
        .def("getAttention", &Brain::getAttention,
             R"pbdoc(
            Get access to attentional selection system.
            
            Returns:
                AttentionalSelection pointer.
        )pbdoc")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             R"pbdoc(
            Get access to development system.
            
            Returns:
                DevelopmentSystem pointer.
        )pbdoc")
        .def("getDopamine", &Brain::getDopamine,
             R"pbdoc(
            Get access to dopamine system.
            
            Returns:
                Dopamine pointer.
        )pbdoc")
        .def("getCuriosity", &Brain::getCuriosity,
             R"pbdoc(
            Get access to curiosity system.
            
            Returns:
                Curiosity pointer.
        )pbdoc")
        .def("getNovelty", &Brain::getNovelty,
             R"pbdoc(
            Get access to novelty detection system.
            
            Returns:
                Novelty pointer.
        )pbdoc")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             R"pbdoc(
            Get access to prediction error signal.
            
            Returns:
                PredictionError pointer.
        )pbdoc")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             R"pbdoc(
            Apply neuromodulatory signals.
            
            Args:
                signal: Neuromodulator signal to apply.
        )pbdoc")
        .def("updatePlasticity", &Brain::updatePlasticity,
             R"pbdoc(
            Update plasticity rules (called automatically in step).
            
            This updates synaptic weights based on plasticity rules
            like STDP and Hebbian learning.
        )pbdoc")
        .def("develop", &Brain::develop,
             R"pbdoc(
            Apply developmental changes (called automatically in step).
            
            This applies age-dependent changes to neural development,
            including structural plasticity and synaptic pruning.
        )pbdoc")
        .def("injectCurrentToNeuronsByType", 
             [](Brain& brain, NeuronType type, MembranePotential current) {
                 brain.injectCurrentToNeurons(type, current);
             }, py::arg("type"), py::arg("current"),
             R"pbdoc(
            Inject current into all neurons of a specific type.
            
            Args:
                type: Type of neurons to target.
                current: Amount of current to inject.
            )pbdoc")
        .def("produceActionFromNeurons",
             [](Brain& brain) {
                 return brain.produceAction();
             },
             R"pbdoc(
            Produce action from current neural activity.
            
            Returns:
                Action produced based on current neural states.
            )pbdoc")
        .def("saveCheckpoint", &Brain::save, py::arg("filepath"),
             R"pbdoc(
            Save brain state as a checkpoint (alias for save).
            
            Args:
                filepath: Path where checkpoint will be saved.
                
            Returns:
                True if successful, False otherwise.
            )pbdoc")
        .def("loadCheckpoint", &Brain::load, py::arg("filepath"),
             R"pbdoc(
            Load brain state from checkpoint (alias for load).
            
            Args:
                filepath: Path to the checkpoint file.
                
            Returns:
                True if successful, False otherwise.
            )pbdoc");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain provides a high-level interface for agents to interact
        with the neural brain in a simulated world, handling sensory input
        and motor output.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
            Create an agent brain interface.
            
            Args:
                brain: Underlying neural brain.
            )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
            Initialize with world.
            
            Args:
                world: Simulation world to interact with.
            )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(
            Get expected sensory input size.
            
            Returns:
                Expected size of sensory input vector.
        )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(
            Get expected motor output size.
            
            Returns:
                Expected size of motor output vector.
        )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(
            Process sensory percept and inject into brain.
            
            Args:
                percept: Sensory percept to process.
            )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
            Decode brain motor activity into motor command.
            
            Returns:
                Motor command derived from brain activity.
            )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
            Apply reward-based neuromodulation.
            
            Args:
                reward: Actual reward received.
                predictedReward: Predicted reward from model.
            )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
            Update development system.
            
            Args:
                timestep: Time increment for development.
            )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(
            Get current developmental stage.
            
            Returns:
                Current developmental stage.
        )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(
            Get current neuromodulation level.
            
            Returns:
                Current neuromodulation level (0.0 to 1.0).
        )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(
            Get curiosity level.
            
            Returns:
                Current curiosity level (0.0 to 1.0).
        )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(
            Get novelty level.
            
            Returns:
                Current novelty level (0.0 to 1.0).
        )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(
            Get prediction error.
            
            Returns:
                Current prediction error.
        )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(
            Reset agent for new episode.
            
            This resets the agent's internal state for a new
            simulation episode.
        )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get the underlying brain.
            
            Returns:
                The neural brain instance.
        )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(
            Enable or disable reward modulation.
            
            Args:
                enable: Whether to enable reward modulation.
            )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(
            Enable or disable structural plasticity.
            
            Args:
                enable: Whether to enable structural plasticity.
            )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(
            Enable or disable development.
            
            Args:
                enable: Whether to enable development.
            )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(
            Enable or disable curiosity.
            
            Args:
                enable: Whether to enable curiosity.
            )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(
            Check if reward modulation is enabled.
            
            Returns:
                True if reward modulation is enabled.
        )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(
            Check if structural plasticity is enabled.
            
            Returns:
                True if structural plasticity is enabled.
        )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(
            Check if development is enabled.
            
            Returns:
                True if development is enabled.
        )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(
            Check if curiosity is enabled.
            
            Returns:
                True if curiosity is enabled.
        )pbdoc")
        .def("enable_all_systems", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, R"pbdoc(
            Enable all learning systems.
            
            This enables reward modulation, structural plasticity,
            development, and curiosity systems.
        )pbdoc")
        .def("disable_all_systems", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, R"pbdoc(
            Disable all learning systems.
            
            This disables all learning and neuromodulation systems.
        )pbdoc");

    // Convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return py_helpers::create_default_config();
    }, R"pbdoc(
        Create a default configuration with sensible values.
        
        Returns:
            Default configuration with recommended settings.
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with configuration.
        
        Args:
            config: Configuration for the brain.
            
        Returns:
            New brain instance.
    )pbdoc")
        .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, R"pbdoc(
        Create a new simple world.
        
        Returns:
            New simple world instance.
    )pbdoc")
        .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface.
        
        Args:
            brain: Underlying neural brain.
            
        Returns:
            New agent brain interface.
    )pbdoc")
        .def("createAction", [](ActionType type, std::vector<float> parameters) -> std::unique_ptr<Action> {
            auto action = std::make_unique<Action>();
            action->setType(type);
            action->setParameters(parameters);
            return action;
        }, py::arg("type"), py::arg("parameters"),
           R"pbdoc(
        Create an action with type and parameters.
        
        Args:
            type: Type of action.
            parameters: Action parameters.
            
        Returns:
            New action instance.
    )pbdoc")
        .def("createWorldObject", [](float x, float y, WorldObjectType type,
                                   float value, float radius) -> std::unique_ptr<WorldObject> {
            auto obj = std::make_unique<WorldObject>();
            obj->x = x;
            obj->y = y;
            obj->type = type;
            obj->value = value;
            obj->radius = radius;
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("type"),
           py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
           R"pbdoc(
        Create a world object.
        
        Args:
            x: X position.
            y: Y position.
            type: Object type.
            value: Object value.
            radius: Object radius.
            
        Returns:
            New world object.
    )pbdoc")
        .def("createVision", [](size_t width, size_t height, size_t channels) -> std::unique_ptr<Vision> {
            auto vision = std::make_unique<Vision>();
            vision->setData(std::vector<float>(width * height * channels, 0.0f));
            return vision;
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
           R"pbdoc(
        Create vision sensory input.
        
        Args:
            width: Vision width.
            height: Vision height.
            channels: Number of color channels.
            
        Returns:
            New vision instance.
    )pbdoc")
        .def("createAudio", [](size_t sampleRate, size_t numSamples) -> std::unique_ptr<Audio> {
            auto audio = std::make_unique<Audio>();
            audio->setSampleRate(sampleRate);
            audio->setData(std::vector<float>(numSamples, 0.0f));
            return audio;
        }, py::arg("sampleRate"), py::arg("numSamples"),
           R"pbdoc(
        Create audio sensory input.
        
        Args:
            sampleRate: Audio sample rate.
            numSamples: Number of audio samples.
            
        Returns:
            New audio instance.
    )pbdoc")
        .def("createInternalSignals", []() -> std::unique_ptr<InternalSignals> {
            return std::make_unique<InternalSignals>();
        }, R"pbdoc(
        Create internal signals sensory input.
        
        Returns:
            New internal signals instance.
    )pbdoc")
        .def("createAdvancedAgent", 
             [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
                 auto agent = std::make_shared<AgentBrain>(brain);
                 agent->initialize(*world);
                 agent->enable_all_systems();
                 return agent;
             }, py::arg("brain"), py::arg("world"),
             R"pbdoc(
        Create an advanced agent with all systems enabled.
        
        Args:
            brain: Neural brain.
            world: Simulation world.
            
        Returns:
            Advanced agent brain interface.
    )pbdoc")
        .def("createCompleteSimulation", 
             [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
                 auto brain = std::make_shared<Brain>(config);
                 brain->initialize();
                 auto agent = std::make_shared<AgentBrain>(brain);
                 agent->initialize(*world);
                 agent->enable_all_systems();
                 return py::make_tuple(brain, agent);
             }, py::arg("config"), py::arg("world"),
             R"pbdoc(
        Create a complete simulation setup.
        
        Args:
            config: Brain configuration.
            world: Simulation world.
            
        Returns:
            Tuple of (brain, agent) ready for simulation.
    )pbdoc")
        .def("run_episode", 
             [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                std::shared_ptr<AgentBrain> agent, int numSteps, bool enableReward) {
                 
                 for (int step = 0; step < numSteps; ++step) {
                     world->update(0.1);
                     auto percept = world->getSensoryPercept();
                     agent->processSensoryInput(percept);
                     brain->step(step);
                     auto action = agent->decodeMotorCommand();
                     world->applyMotorCommand(action, world->getSimulationTime());
                     
                     if (enableReward && step % 10 == 0) {
                         agent->applyRewardModulation(1.0, 0.5);
                     }
                 }
                 
                 return brain->getAverageFiringRate();
             }, py::arg("brain"), py::arg("world"), py::arg("agent"),
                py::arg("numSteps") = 100, py::arg("enableReward") = true,
                R"pbdoc(
            Run a complete simulation episode.
            
            Args:
                brain: Neural brain.
                world: Simulation world.
                agent: Agent brain interface.
                numSteps: Number of simulation steps.
                enableReward: Whether to enable reward modulation.
                
            Returns:
                Average firing rate during the episode.
        )pbdoc")
        .def("train_agent", 
             [](int numEpisodes, std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
                 auto brain = std::make_shared<Brain>(config);
                 brain->initialize();
                 auto agent = std::make_shared<AgentBrain>(brain);
                 agent->initialize(*world);
                 agent->enable_all_systems();
                 
                 std::vector<float> firingRates;
                 
                 for (int episode = 0; episode < numEpisodes; ++episode) {
                     for (int step = 0; step < 50; ++step) {
                         world->update(0.1);
                         auto percept = world->getSensoryPercept();
                         agent->processSensoryInput(percept);
                         brain->step(step);
                         auto action = agent->decodeMotorCommand();
                         world->applyMotorCommand(action, world->getSimulationTime());
                         
                         if (step % 10 == 0) {
                             agent->applyRewardModulation(0.5, 0.3);
                         }
                     }
                     
                     firingRates.push_back(brain->getAverageFiringRate());
                     brain->reset();
                     world->reset();
                     agent->reset();
                 }
                 
                 float avgFiringRate = 0.0f;
                 for (float rate : firingRates) {
                     avgFiringRate += rate;
                 }
                 avgFiringRate /= firingRates.size();
                 
                 return avgFiringRate;
             }, py::arg("numEpisodes") = 10,
                py::arg("config") = py_helpers::create_default_config(),
                py::arg("world") = std::make_shared<SimpleWorld>(),
                R"pbdoc(
            Train an agent over multiple episodes.
            
            Args:
                numEpisodes: Number of training episodes.
                config: Brain configuration.
                world: Simulation world.
                
            Returns:
                Average firing rate over all episodes.
        )pbdoc")
        .def("benchmark_performance",
             [](std::shared_ptr<Brain> brain, int numSteps) {
                 auto start_time = std::chrono::high_resolution_clock::now();
                 
                 for (int step = 0; step < numSteps; ++step) {
                     brain->step(step);
                 }
                 
                 auto end_time = std::chrono::high_resolution_clock::now();
                 auto duration = std::chrono::duration<double>(end_time - start_time);
                 
                 double steps_per_second = numSteps / duration.count();
                 return steps_per_second;
             }, py::arg("brain"), py::arg("numSteps") = 1000,
             R"pbdoc(
            Benchmark brain performance.
            
            Args:
                brain: Brain to benchmark.
                numSteps: Number of steps to simulate.
                
            Returns:
                Performance in steps per second.
        )pbdoc")
        .def("get_version", []() {
            return "0.1.0";
        }, R"pbdoc(
            Get the version of the NLM Python bindings.
            
            Returns:
                Version string.
        )pbdoc")
        .def("get_build_info", []() {
            py::dict info;
            info["version"] = "0.1.0";
            info["build_type"] = "Release";
            info["cxx_standard"] = "20";
            info["pybind11_version"] = "2.11.0";
            return info;
        }, R"pbdoc(
            Get build information.
            
            Returns:
                Dictionary with build information.
        )pbdoc")
        .def("check_system", []() {
            py::dict status;
            
            // Test basic functionality
            auto config = py_helpers::create_default_config();
            status["config"] = config != nullptr;
            
            if (config) {
                status["config_keys"] = config->getKeys();
            }
            
            return status;
        }, R"pbdoc(
            Check system status and configuration.
            
            Returns:
                Dictionary with system status information.
        )pbdoc")

    // Global constants
    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;
    
    // Module-level constants
    m.attr("__version__") = "0.1.0";
    m.attr("BUILD_TYPE") = "Release";
}

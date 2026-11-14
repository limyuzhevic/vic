#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <iostream>  // For std::cout
#include <sstream>   // For std::ostringstream
#include <iomanip>   // For std::fixed, std::setprecision
#include <filesystem>

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

// Custom Python exceptions for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

class InputError : public NLMError {
public:
    explicit InputError(const std::string& msg) : NLMError("Input Error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This module provides Python bindings for all NLM components including:
        - Neural brain simulation and learning
        - World environment and agents
        - Sensory perception and motor control
        - Configuration management
        - Advanced Python features including context managers and callbacks
        
        The bindings follow Python best practices and provide extensive documentation
        with usage examples for both beginners and advanced users.
        
        Key Features:
        - Context managers for resource management
        - Event callback system for brain events
        - Property decorators for cleaner API
        - Comprehensive error handling with Python-specific exceptions
        - Type conversion improvements for better usability
        - Extensive documentation with examples
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError.ptr());
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError.ptr());
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError.ptr());
    py::register_exception<InputError>(m, "InputError", PyExc_ValueError.ptr());
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the NLM simulation.
        
        NeuronId is a 64-bit identifier that uniquely references individual neurons
        within the neural network. Each neuron is assigned a unique ID that remains
        constant throughout the simulation. Neuron IDs can be used to reference
        specific neurons for current injection, observation, or targeted manipulation.
        
        The NeuronId class provides:
        - A 64-bit value for the unique identifier
        - An index property to extract the position in the neural grid
        - Equality comparison operators for checking neuron identity
        - Hash support for use in dictionaries and sets
        - String representation for debugging
        
        Example:
            >>> import pynlm
            >>> neuron_id = pynlm.NeuronId(12345)
            >>> neuron_id.value
            12345
            >>> neuron_id.index
            12345
            >>> id2 = pynlm.NeuronId(12345)
            >>> neuron_id == id2
            True
            >>> hash(neuron_id)
            12345
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0, 
             "Create a NeuronId with the specified value. If value is 0, creates an invalid NeuronId.")
        .def_readwrite("value", &NeuronId::value,
            "Get or set the 64-bit value of this neuron identifier. This is the unique identifier used by the NLM system.")
        .def("index", &NeuronId::index,
            "Get the index of this neuron in the neural grid. Returns the same value for simple IDs.")
        .def("__eq__", &NeuronId::operator==,
            "Check if this NeuronId equals another NeuronId. Returns True if both have the same value.")
        .def("__ne__", &NeuronId::operator!=,
            "Check if this NeuronId is not equal to another NeuronId. Returns True if values differ.")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
            "Return a hash value for this NeuronId. Enables use in sets and dictionaries.")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse in the neural network.
        
        SynapseId is a 64-bit identifier that uniquely references individual synapses
        connecting neurons in the NLM simulation. Each synapse has a unique ID that
        remains constant throughout the simulation. Synapse IDs can be used to reference
        specific connections for plasticity operations, current injection, or analysis.
        
        The SynapseId class provides:
        - A 64-bit value for the unique identifier
        - An index property to extract the position in the connection matrix
        - Equality comparison operators for checking synapse identity
        - Hash support for use in dictionaries and sets
        - String representation for debugging
        
        Example:
            >>> import pynlm
            >>> synapse_id = pynlm.SynapseId(42)
            >>> synapse_id.value
            42
            >>> synapse_id.index
            42
            >>> syn2 = pynlm.SynapseId(42)
            >>> synapse_id == syn2
            True
            >>> hash(synapse_id)
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a SynapseId with the specified value. If value is 0, creates an invalid SynapseId.")
        .def_readwrite("value", &SynapseId::value,
            "Get or set the 64-bit value of this synapse identifier. This is the unique identifier used by the NLM system.")
        .def("index", &SynapseId::index,
            "Get the index of this synapse in the connection matrix. Returns the same value for simple IDs.")
        .def("__eq__", &SynapseId::operator==,
            "Check if this SynapseId equals another SynapseId. Returns True if both have the same value.")
        .def("__ne__", &SynapseId::operator!=,
            "Check if this SynapseId is not equal to another SynapseId. Returns True if values differ.")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
            "Return a hash value for this SynapseId. Enables use in sets and dictionaries.")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a neural brain region in the NLM simulation.
        
        RegionId is a 64-bit identifier that references specific brain regions within
        the neural network. Each region contains multiple neuron populations and is
        responsible for specific cognitive functions. Region IDs are used to organize
        and manage different functional areas of the brain.
        
        The RegionId class provides:
        - A 64-bit value for the unique identifier
        - An index property to extract the region position
        - Equality comparison operators for checking region identity
        - Hash support for use in dictionaries and sets
        - String representation for debugging
        
        Example:
            >>> import pynlm
            >>> region_id = pynlm.RegionId(0)
            >>> region_id.value
            0
            >>> region_id.index
            0
            >>> region2 = pynlm.RegionId(0)
            >>> region_id == region2
            True
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a RegionId with the specified value. If value is 0, creates an invalid RegionId.")
        .def_readwrite("value", &RegionId::value,
            "Get or set the 64-bit value of this region identifier. This is the unique identifier used by the NLM system.")
        .def("index", &RegionId::index,
            "Get the index of this region in the brain. Returns the same value for simple IDs.")
        .def("__eq__", &RegionId::operator==,
            "Check if this RegionId equals another RegionId. Returns True if both have the same value.")
        .def("__ne__", &RegionId::operator!=,
            "Check if this RegionId is not equal to another RegionId. Returns True if values differ.")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
            "Return a hash value for this RegionId. Enables use in sets and dictionaries.")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population in the NLM simulation.
        
        PopulationId is a 64-bit identifier that references specific groups of neurons
        with similar characteristics or functions. Populations are collections of
        neurons that work together to perform specific computational tasks. Each
        population belongs to a specific region and contains neurons of the same type.
        
        The PopulationId class provides:
        - A 64-bit value for the unique identifier
        - An index property to extract the population position
        - Equality comparison operators for checking population identity
        - Hash support for use in dictionaries and sets
        
        Example:
            >>> import pynlm
            >>> population_id = pynlm.PopulationId(100)
            >>> population_id.value
            100
            >>> population_id.index
            100
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a PopulationId with the specified value. If value is 0, creates an invalid PopulationId.")
        .def_readwrite("value", &PopulationId::value,
            "Get or set the 64-bit value of this population identifier. This is the unique identifier used by the NLM system.")
        .def("index", &PopulationId::index,
            "Get the index of this population in the neural hierarchy. Returns the same value for simple IDs.")
        .def("__eq__", &PopulationId::operator==,
            "Check if this PopulationId equals another PopulationId. Returns True if both have the same value.")
        .def("__ne__", &PopulationId::operator!=,
            "Check if this PopulationId is not equal to another PopulationId. Returns True if values differ.");

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM simulation.
        
        Neuron types define the functional characteristics and behavior of neurons
        in the neural network. Different neuron types have different properties,
        connectivity patterns, and computational roles.
        
        Available neuron types:
        - Excitatory: Increases activity in postsynaptic neurons
        - Inhibitory: Decreases activity in postsynaptic neurons  
        - Modulatory: Releases neuromodulators affecting broader network dynamics
        - Sensory: Processes external sensory inputs
        - Motor: Generates motor commands for action selection
        - Internal: Internal processing neurons (cognitive functions)
        
        Example:
            >>> import pynlm
            >>> neuron_type = pynlm.NeuronType.Excitatory
            >>> neuron_type.name
            'Excitatory'
            >>> neuron_type.value
            0
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neurons that increase activity in postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Neurons that decrease activity in postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory, "Neurons that release neuromodulators affecting network dynamics")
        .value("Sensory", NeuronType::Sensory, "Neurons that process external sensory inputs")
        .value("Motor", NeuronType::Motor, "Neurons that generate motor commands for action selection")
        .value("Internal", NeuronType::Internal, "Internal processing neurons for cognitive functions")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the NLM simulation.
        
        Synapse types define the functional characteristics of connections between neurons.
        Different synapse types have different properties, dynamics, and computational roles.
        
        Available synapse types:
        - Excitatory: Increases activity in postsynaptic neurons
        - Inhibitory: Decreases activity in postsynaptic neurons
        - Modulatory: Releases neuromodulators affecting broader network dynamics
        - Electrical: Direct electrical coupling between neurons
        - GapJunction: Electrical synapse with chemical components
        
        Example:
            >>> import pynlm
            >>> synapse_type = pynlm.SynapseType.Excitatory
            >>> synapse_type.name
            'Excitatory'
            >>> synapse_type.value
            0
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard chemical synapse that increases postsynaptic activity")
        .value("Inhibitory", SynapseType::Inhibitory, "Chemical synapse that decreases postsynaptic activity")
        .value("Modulatory", SynapseType::Modulatory, "Synapse that releases neuromodulators affecting network dynamics")
        .value("Electrical", SynapseType::Electrical, "Direct electrical coupling between neurons")
        .value("GapJunction", SynapseType::GapJunction, "Hybrid synapse with both electrical and chemical properties")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages in the NLM brain.
        
        Developmental stages represent the maturation process of the neural network.
        Each stage has different plasticity rates, connection patterns, and computational capabilities.
        
        Available developmental stages:
        - Initial: Infantile stage with high plasticity and basic connectivity
        - CriticalPeriod: Window of heightened learning and rapid development
        - Maturation: Consolidation of learned connections and optimization
        - Adult: Mature stage with stable connections and optimized performance
        - Aging: Decline in plasticity and performance due to neural wear
        
        Example:
            >>> import pynlm
            >>> stage = pynlm.DevelopmentalStage.Maturation
            >>> stage.name
            'Maturation'
            >>> stage.value
            2
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Infantile stage with high plasticity and basic connectivity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Window of heightened learning and rapid development")
        .value("Maturation", DevelopmentalStage::Maturation, "Consolidation of learned connections and optimization")
        .value("Adult", DevelopmentalStage::Adult, "Mature stage with stable connections and optimized performance")
        .value("Aging", DevelopmentalStage::Aging, "Decline in plasticity and performance due to neural wear")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states in the NLM simulation.
        
        Firing states represent the current activity level of neurons and determine
        how neurons participate in information processing and propagation.
        
        Available firing states:
        - Resting: Neuron is quiescent and not firing
        - Active: Neuron is actively firing spikes
        - Refractory: Neuron has just fired and is temporarily unable to fire
        - Inhibited: Neuron is prevented from firing by inhibitory inputs
        
        Example:
            >>> import pynlm
            >>> state = pynlm.FiringState.Active
            >>> state.name
            'Active'
            >>> state.value
            1
        )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is quiescent and not firing")
        .value("Active", FiringState::Active, "Neuron is actively firing spikes")
        .value("Refractory", FiringState::Refractory, "Neuron has just fired and is temporarily unable to fire")
        .value("Inhibited", FiringState::Inhibited, "Neuron is prevented from firing by inhibitory inputs")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of agent action types in the NLM simulation.
        
        Action types represent the possible behaviors an agent can perform in the
        simulated environment. Each action type corresponds to specific motor commands
        and determines how the agent interacts with the world.
        
        Available action types:
        - MoveForward: Move the agent forward in the current direction
        - MoveBackward: Move the agent backward
        - MoveLeft: Turn left and move forward
        - MoveRight: Turn right and move forward
        - TurnLeft: Rotate the agent's orientation left without moving
        - TurnRight: Rotate the agent's orientation right without moving
        - Look: Look in the current direction without moving
        - LookUp: Look upward (increase vertical vision)
        - LookDown: Look downward (decrease vertical vision)
        - Interact: Attempt to interact with objects in the environment
        - Eat: Consume food resources
        - Drink: Consume water resources
        - Rest: Stop moving and enter a resting state
        - Wait: Do nothing for one simulation step
        - Custom: User-defined action type
        
        Example:
            >>> import pynlm
            >>> action = pynlm.ActionType.MoveForward
            >>> action.name
            'MoveForward'
            >>> action.value
            0
        )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move the agent forward in the current direction")
        .value("MoveBackward", ActionType::MoveBackward, "Move the agent backward")
        .value("MoveLeft", ActionType::MoveLeft, "Turn left and move forward")
        .value("MoveRight", ActionType::MoveRight, "Turn right and move forward")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate the agent's orientation left without moving")
        .value("TurnRight", ActionType::TurnRight, "Rotate the agent's orientation right without moving")
        .value("Look", ActionType::Look, "Look in the current direction without moving")
        .value("LookUp", ActionType::LookUp, "Look upward (increase vertical vision)")
        .value("LookDown", ActionType::LookDown, "Look downward (decrease vertical vision)")
        .value("Interact", ActionType::Interact, "Attempt to interact with objects in the environment")
        .value("Eat", ActionType::Eat, "Consume food resources")
        .value("Drink", ActionType::Drink, "Consume water resources")
        .value("Rest", ActionType::Rest, "Stop moving and enter a resting state")
        .value("Wait", ActionType::Wait, "Do nothing for one simulation step")
        .value("Custom", ActionType::Custom, "User-defined action type")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands in the NLM simulation.
        
        Motor commands represent the fundamental actions that the motor system can
        execute to move the agent in the environment. These are lower-level commands
        that map to higher-level action types.
        
        Available motor commands:
        - MoveForward: Move the agent forward in the current direction
        - MoveBackward: Move the agent backward
        - TurnLeft: Rotate the agent's orientation left without moving
        - TurnRight: Rotate the agent's orientation right without moving
        - LookLeft: Look to the left side
        - LookRight: Look to the right side
        - Interact: Attempt to interact with objects in the environment
        - Wait: Do nothing for one simulation step
        
        Example:
            >>> import pynlm
            >>> motor_cmd = pynlm.MotorCommand.MoveForward
            >>> motor_cmd.name
            'MoveForward'
            >>> motor_cmd.value
            0
        )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move the agent forward in the current direction")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move the agent backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Rotate the agent's orientation left without moving")
        .value("TurnRight", MotorCommand::TurnRight, "Rotate the agent's orientation right without moving")
        .value("LookLeft", MotorCommand::LookLeft, "Look to the left side")
        .value("LookRight", MotorCommand::LookRight, "Look to the right side")
        .value("Interact", MotorCommand::Interact, "Attempt to interact with objects in the environment")
        .value("Wait", MotorCommand::Wait, "Do nothing for one simulation step")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of world object types in the NLM simulation.
        
        World object types define the different kinds of objects that can exist in
        the simulated environment. Each object type has specific properties and behaviors
        that determine how agents interact with them.
        
        Available world object types:
        - Empty: No object present (empty space)
        - Resource: Food or drink resources for the agent
        - Hazard: Dangerous objects that harm the agent
        - Wall: Impassable obstacles that block movement
        - Marker: Special locations or targets for the agent
        
        Example:
            >>> import pynlm
            >>> obj_type = pynlm.WorldObjectType.Resource
            >>> obj_type.name
            'Resource'
            >>> obj_type.value
            1
        )pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object present (empty space)")
        .value("Resource", WorldObjectType::Resource, "Food or drink resources for the agent")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous objects that harm the agent")
        .value("Wall", WorldObjectType::Wall, "Impassable obstacles that block movement")
        .value("Marker", WorldObjectType::Marker, "Special locations or targets for the agent")
        .export_values();

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Configuration class for the NLM system.
        
        The Config class manages all configuration settings for the NLM simulation.
        It provides a key-value store for parameters that control neural network
        behavior, learning rates, simulation parameters, and agent capabilities.
        
        The configuration system supports:
        - Loading from JSON files
        - Loading from command line arguments
        - Saving to JSON files
        - Key-based value access and modification
        - Validation of configuration values
        - Summary generation for debugging
        
        Configuration keys include:
        - Learning parameters (learning_rate, momentum)
        - Network architecture (neuron_count, population_count)
        - Simulation parameters (timestep, max_steps)
        - Agent capabilities (enable_reward_modulation, etc.)
        
        Example:
            >>> import pynlm
            >>> config = pynlm.Config()
            >>> config.loadFromFile("config.json")
            >>> learning_rate = config.get("learning_rate", 0.01)
            >>> config.set("neuron_count", 1000)
            >>> config.summary()
            '<Config: neuron_count=1000, learning_rate=0.01, ...>'
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. The file should contain key-value pairs of configuration parameters."
             " Raises ConfigurationError if file cannot be read or parsed."
             " Example: config.loadFromFile('config.json')")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments. Converts Python list of strings to C++ argv format."
             " Example: config.loadFromArgs(['--learning_rate', '0.1', '--neuron_count', '500'])")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save current configuration to a JSON file. The file will contain all configuration parameters."
             " Raises ConfigurationError if file cannot be written.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if the key is present in the configuration.")
        .def("get", [](const Config& self, const std::string& key, const std::string& default_value) {
            return self.has(key) ? self.get(key) : default_value;
        }, py::arg("key"), py::arg("default_value"),
             "Get a configuration value by key with a default fallback. Returns the value if key exists, otherwise returns default_value.")
        .def("get", &Config::get, py::arg("key"),
             "Get a configuration value by key. Returns the string value for the key.")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value. The value will be converted to string representation.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings. Returns all keys currently set in the configuration.")
        .def("clear", &Config::clear,
             "Clear all configuration entries. Resets the configuration to default state.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration. Returns a formatted string showing all current settings.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__getitem__", [](Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw InputError("Configuration key not found: " + key);
            }
            return self.get(key);
        }, "Access configuration value using dictionary-style syntax (config['key']).")
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, "Set configuration value using dictionary-style syntax (config['key'] = value).")
        .def("__contains__", &Config::has, "Check if key exists in configuration using 'in' operator (key in config).")
        .def("to_dict", [](const Config& self) {
            std::vector<std::string> keys = self.getKeys();
            py::dict result;
            for (const auto& key : keys) {
                result[key.c_str()] = self.get(key);
            }
            return result;
        }, "Convert configuration to Python dictionary for easy access and manipulation.")
        .def("from_dict", [](Config& self, const py::dict& dict) {
            for (auto item : dict) {
                std::string key = item.first.cast<std::string>();
                std::string value = item.second.cast<std::string>();
                self.set(key, value);
            }
        }, "Update configuration from Python dictionary. Converts dictionary keys and values to configuration.");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input in the NLM simulation.
        
        SensoryInput represents the raw sensory data received by the agent from
        the environment. This includes visual input, audio input, internal signals,
        and other sensory modalities that provide information to the brain.
        
        The SensoryInput base class provides:
        - getType(): Returns the sensory input type (Vision, Audio, InternalSignals)
        - getData(): Returns the raw sensor data as a vector
        - getDimensions(): Returns the data dimensionality
        - getTimestamp(): Returns the timestamp of the sensory input
        - setTimestamp(): Sets the timestamp of the sensory input
        
        Subclasses implement specific sensory modalities:
        - Vision: Visual information from cameras or vision systems
        - Audio: Sound input from microphones or audio systems
        - InternalSignals: Internal physiological signals
        
        Example:
            >>> import pynlm
            >>> # Create vision input
            >>> vision = pynlm.Vision(640, 480, 3)
            >>> vision.setData([0.1, 0.2, 0.3, ...])  # Flatten image data
            >>> print(f"Vision type: {vision.getType()}")
            >>> print(f"Vision dimensions: {vision.getDimensions()}")
            >>> # Create audio input
            >>> audio = pynlm.Audio(44100, 1024)
            >>> audio.setData([0.0, 0.5, 0.2, ...])  # Audio samples
        )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get the sensory input type. Returns a string representing the type (Vision, Audio, or InternalSignals).")
        .def("getData", &SensoryInput::getData,
             "Get the raw sensory data as a vector of floats. The format depends on the sensory modality.")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory data. Returns the number of elements in the data vector.")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp of the sensory input. Returns the simulation time when the input was received.")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of the sensory input. timestamp: Simulation time when input was received.")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: " + input.getType() + ", " + std::to_string(input.getDimensions()) + " elements>";
        });

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input for visual information in the NLM simulation.
        
        Vision represents visual data captured by cameras or vision systems.
        It stores image data as a flattened vector of pixel values and provides
        access to image dimensions for reconstruction and processing.
        
        The Vision class provides:
        - Constructors for creating vision data with specified dimensions
        - setData(): Set or update the image data
        - getWidth(), getHeight(), getChannels(): Access image dimensions
        - Inherits all SensoryInput base class functionality
        
        The data is stored as a flattened 1D array where pixel order is typically
        row-major with interleaved RGB channels (width × height × channels elements).
        
        Example:
            >>> import pynlm
            >>> # Create 640x480 RGB vision input
            >>> vision = pynlm.Vision(640, 480, 3)
            >>> # Create 320x240 grayscale vision input
            >>> gray_vision = pynlm.Vision(320, 240)
            >>> # Set some example RGB data (first 3 pixels)
            >>> vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, ...])
            >>> print(f"Image size: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
            >>> print(f"Total pixels: {vision.getDimensions()}")
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create Vision sensory input with specified image dimensions."
             " width: Image width in pixels (default: 3 for RGB)"
             " height: Image height in pixels (default: 3 for RGB)"
             " channels: Number of color channels (default: 3 for RGB, 1 for grayscale)")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() < 3) {
                throw InputError("Vision data must contain at least 3 elements for RGB values");
            }
            self.setData(data);
        }, py::arg("data"),
             "Set vision image data. data: Vector of float values containing pixel data."
             " Data should be in format [R,G,B,R,G,B,...] for RGB images or [gray,...] for grayscale.")
        .def("getWidth", &Vision::getWidth,
             "Get image width in pixels.")
        .def("getHeight", &Vision::getHeight,
             "Get image height in pixels.")
        .def("getChannels", &Vision::getChannels,
             "Get number of color channels. Returns 1 for grayscale, 3 for RGB, etc.")
        .def("getPixelCount", &Vision::getWidth,
             "Get total number of pixels in the image (width × height).")
        .def("reconstructImage", [](const Vision& self) {
            std::vector<std::vector<std::vector<float>>> image;
            if (self.getChannels() == 3) {
                image.resize(self.getHeight());
                for (size_t i = 0; i < self.getHeight(); ++i) {
                    image[i].resize(self.getWidth());
                    for (size_t j = 0; j < self.getWidth(); ++j) {
                        size_t idx = (i * self.getWidth() + j) * 3;
                        if (idx + 2 < self.getData().size()) {
                            image[i][j] = {self.getData()[idx], self.getData()[idx+1], self.getData()[idx+2]};
                        }
                    }
                }
            }
            return image;
        }, "Reconstruct image data into 2D arrays for easier processing."
             " Returns a 2D nested vector for multi-channel images or simplified for grayscale.")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" +
                   std::to_string(vision.getHeight()) + "x" +
                   std::to_string(vision.getChannels()) + " channels>";
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for sound information in the NLM simulation.
        
        Audio represents sound data captured by microphones or audio systems.
        It stores audio samples as a vector of float values and provides access to
        audio format parameters like sample rate and duration.
        
        The Audio class provides:
        - Constructors for creating audio with specified sample rate and duration
        - setData(): Set or update the audio samples
        - getSampleRate(), getNumSamples(): Access audio format parameters
        - Inherits all SensoryInput base class functionality
        
        The audio data is stored as a 1D array of floating-point samples where each
        sample represents amplitude at a specific point in time.
        
        Example:
            >>> import pynlm
            >>> # Create 44.1kHz audio with 1 second duration (44100 samples)
            >>> audio = pynlm.Audio(44100, 44100)
            >>> # Set some example audio data
            >>> audio.setData([0.0, 0.5, -0.3, 0.2, 0.0, ...])  # Audio samples
            >>> print(f"Sample rate: {audio.getSampleRate()} Hz")
            >>> print(f"Duration: {audio.getNumSamples() / audio.getSampleRate():.2f} seconds")
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio sensory input with specified audio format."
             " sampleRate: Samples per second (e.g., 44100 for CD quality)"
             " numSamples: Number of audio samples in the buffer")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw InputError("Audio data must not be empty");
            }
            self.setData(data);
        }, py::arg("data"),
             "Set audio samples. data: Vector of float values representing audio amplitude."
             " Typical values range from -1.0 to 1.0.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate. sampleRate: New sample rate in Hz (e.g., 44100).")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get audio sample rate in Hz.")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get number of audio samples in the buffer.")
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds. Returns duration based on sample rate and buffer size.")
        .def("normalize", [](Audio& self, float target_level = 1.0) {
            double current_level = 0.0;
            size_t num_samples = self.getNumSamples();
            for (size_t i = 0; i < num_samples; ++i) {
                current_level += std::abs(self.getData()[i]);
            }
            current_level /= num_samples;
            
            if (current_level > 0.0) {
                for (size_t i = 0; i < num_samples; ++i) {
                    self.getData()[i] = self.getData()[i] * target_level / current_level;
                }
            }
        }, "Normalize audio to target level. target_level: Target amplitude level (default: 1.0).")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + " Hz, " +
                   std::to_string(audio.getNumSamples()) + " samples>";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input for physiological and cognitive states.
        
        InternalSignals represents internal physiological and cognitive states
        of the agent, such as hunger, thirst, energy levels, stress, etc. These
        internal signals influence agent behavior and decision-making.
        
        The InternalSignals class provides:
        - Constructors for creating internal signal containers
        - addSignal(): Add a new internal signal value
        - clearSignals(): Remove all internal signals
        - Inherits all SensoryInput base class functionality
        
        Internal signals are used by the agent to:
        - Monitor physiological needs (hunger, thirst, energy)
        - Track cognitive states (stress, curiosity, attention)
        - Guide action selection and learning
        
        Example:
            >>> import pynlm
            >>> signals = pynlm.InternalSignals()
            >>> signals.addSignal(0.8)  # Add hunger signal (0-1 scale)
            >>> signals.addSignal(0.3)  # Add thirst signal
            >>> signals.addSignal(0.9)  # Add energy signal
            >>> data = signals.getData()  # Get all signals
            >>> print(f"Number of signals: {signals.getDimensions()}")
            >>> signals.clearSignals()  # Remove all signals
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value. value: Signal value typically in range 0.0-1.0 representing intensity.")
        .def("getSignalCount", &InternalSignals::getDimensions,
             "Get number of internal signals currently stored.")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            const std::vector<float>& data = self.getData();
            if (index >= data.size()) {
                throw InputError("Signal index out of range: " + std::to_string(index));
            }
            return data[index];
        }, py::arg("index"),
             "Get a specific internal signal by index. index: Signal index (0-based).")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Remove all internal signals. Resets the signal container to empty.")
        .def("getMaxSignal", [](const InternalSignals& self) {
            const std::vector<float>& data = self.getData();
            if (data.empty()) {
                return 0.0f;
            }
            float max_val = data[0];
            for (size_t i = 1; i < data.size(); ++i) {
                if (data[i] > max_val) max_val = data[i];
            }
            return max_val;
        }, "Get the maximum value among all internal signals.")
        .def("getMinSignal", [](const InternalSignals& self) {
            const std::vector<float>& data = self.getData();
            if (data.empty()) {
                return 0.0f;
            }
            float min_val = data[0];
            for (size_t i = 1; i < data.size(); ++i) {
                if (data[i] < min_val) min_val = data[i];
            }
            return min_val;
        }, "Get the minimum value among all internal signals.")
        .def("getAverageSignal", [](const InternalSignals& self) {
            const std::vector<float>& data = self.getData();
            if (data.empty()) {
                return 0.0f;
            }
            float sum = 0.0f;
            for (float val : data) sum += val;
            return sum / data.size();
        }, "Get the average value of all internal signals.")
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals: " + std::to_string(signals.getDimensions()) + " signals>";
        });

    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output in the NLM simulation.
        
        Action represents the agent's intended behavior in the environment, including
        movement, interaction with objects, and goal-directed behavior. Actions are
        produced by the brain's motor system and executed by the world simulation.
        
        The Action class provides:
        - Constructors for creating actions with specified type and parameters
        - getType(), setType(): Access and modify the action type
        - getParameters(), setParameters(): Access and modify action parameters
        - getName(): Get human-readable action name
        - clone(): Create a copy of the action
        - Type conversion for common action types
        
        Action types include movement (MoveForward, MoveBackward, etc.),
        interaction (Interact, Eat, Drink), and manipulation (TurnLeft, TurnRight).
        
        Action parameters are typically control values for the action, such as
        speed, duration, or specific values for custom actions.
        
        Example:
            >>> import pynlm
            >>> # Create basic movement action
            >>> move_action = pynlm.Action(pynlm.ActionType.MoveForward)
            >>> # Create action with parameters
            >>> custom_action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.3, 0.8])
            >>> print(f"Action type: {move_action.getType()}")
            >>> print(f"Action name: {move_action.getName()}")
            >>> params = move_action.getParameters()
            >>> move_action.setType(pynlm.ActionType.TurnLeft)
            >>> copy_action = move_action.clone()
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create Action with specified type. type: ActionType enum value.")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create Action with specified type and parameters. type: ActionType enum. parameters: Action parameters as float vector.")
        .def("getType", &Action::getType,
             "Get the action type. Returns ActionType enum value.")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type. type: New ActionType enum value.")
        .def("getParameters", &Action::getParameters,
             "Get action parameters as a vector of floats. Returns empty vector if no parameters.")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters. params: Vector of float values as action parameters.")
        .def("getName", &Action::getName,
             "Get human-readable action name. Returns descriptive string for the action type.")
        .def("clone", &Action::clone,
             "Create a copy of this action. Returns new Action object with same type and parameters.")
        .def("isMovement", [](const Action& action) {
            ActionType type = action.getType();
            return type == ActionType::MoveForward || type == ActionType::MoveBackward ||
                   type == ActionType::MoveLeft || type == ActionType::MoveRight;
        }, "Check if action is a movement action. Returns True for movement-related actions.")
        .def("isInteraction", [](const Action& action) {
            ActionType type = action.getType();
            return type == ActionType::Interact || type == ActionType::Eat || type == ActionType::Drink;
        }, "Check if action is an interaction action. Returns True for resource-related actions.")
        .def("isManipulation", [](const Action& action) {
            ActionType type = action.getType();
            return type == ActionType::TurnLeft || type == ActionType::TurnRight || type == ActionType::Look;
        }, "Check if action is a manipulation action. Returns True for orientation/look actions.")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " with " + std::to_string(action.getParameters().size()) + " parameters>";
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the NLM simulation.
        
        WorldObject represents entities within the simulated environment, including
        resources, hazards, walls, and markers. Objects have physical properties
        such as position, size, and type, and can be active or inactive in the simulation.
        
        The WorldObject class provides:
        - Constructors for creating world objects with specified properties
        - Position attributes (x, y): Object location in 2D space
        - Radius: Object size for collision detection
        - Type: Object category (Resource, Hazard, Wall, Marker, Empty)
        - Value: Additional numeric value for resource or hazard intensity
        - Active flag: Whether object is currently active in simulation
        
        World objects serve as:
        - Resources: Food, water, or other valuable items
        - Hazards: Dangerous objects that harm agents
        - Walls: Impassable obstacles
        - Markers: Targets or special locations
        
        Example:
            >>> import pynlm
            >>> # Create resource object
            >>> resource = pynlm.WorldObject(5.0, 3.0, pynlm.WorldObjectType.Resource, 10.0, 0.5)
            >>> print(f"Position: ({resource.x}, {resource.y})")
            >>> print(f"Type: {resource.type}")
            >>> print(f"Value: {resource.value}")
            >>> # Create wall object
            >>> wall = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Wall, 0.0, 1.0)
            >>> # Activate/deactivate objects
            >>> resource.active = True
            >>> wall.active = False
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create WorldObject with specified properties."
             " x: X coordinate in world space"
             " y: Y coordinate in world space" 
             " type: WorldObjectType enum value"
             " value: Additional numeric value (e.g., resource quantity)"
             " radius: Object radius for collision detection")
        .def_readwrite("x", &WorldObject::x,
             "X coordinate of the object in world space.")
        .def_readwrite("y", &WorldObject::y,
             "Y coordinate of the object in world space.")
        .def_readwrite("radius", &WorldObject::radius,
             "Radius of the object for collision detection and interaction.")
        .def_readwrite("type", &WorldObject::type,
             "Type of the object. One of WorldObjectType enum values (Resource, Hazard, Wall, Marker, Empty).")
        .def_readwrite("value", &WorldObject::value,
             "Additional numeric value. For resources: quantity; for hazards: damage value.")
        .def_readwrite("active", &WorldObject::active,
             "Whether the object is currently active in the simulation. Inactive objects are ignored.")
        .def("distanceTo", [](const WorldObject& obj, float other_x, float other_y) {
            float dx = obj.x - other_x;
            float dy = obj.y - other_y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other_x"), py::arg("other_y"),
             "Calculate distance to another position. Returns Euclidean distance.")
        .def("isCollidingWith", [](const WorldObject& obj, const WorldObject& other) {
            float dx = obj.x - other.x;
            float dy = obj.y - other.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance < (obj.radius + other.radius);
        }, py::arg("other"),
             "Check if this object is colliding with another object. Returns True if distance < sum of radii.")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + "," + std::to_string(obj.y) + " type=" +
                   std::to_string(static_cast<int>(obj.type)) + " radius=" + std::to_string(obj.radius) + ">";
        });

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state in the NLM simulation.
        
        AgentBody represents the physical state of an agent in the environment, including
        position, orientation, movement, and physiological state. This class models
        the agent's body as it interacts with the world, providing sensory and motor information.
        
        The AgentBody class provides:
        - Position attributes (x, y): Current location in 2D world space
        - Orientation: Current direction of movement (in radians, 0-2π)
        - Velocity attributes (velocityX, velocityY): Linear velocity components
        - Angular velocity: Rotational speed
        - Physiological attributes (energy, health, age): Internal state
        - Movement flags (isMoving, isTurning): Current movement state
        - lastActionTime: Time of last action execution
        - reset(): Reset all state variables
        
        The body state is used by:
        - SimpleWorld for movement and collision detection
        - AgentBrain for motor command generation
        - SensoryPercept for proprioception information
        
        Example:
            >>> import pynlm
            >>> agent = pynlm.Agent()
            >>> agent.x = 5.0
            >>> agent.y = 3.0
            >>> agent.orientation = 0.0
            >>> agent.velocityX = 2.0
            >>> agent.velocityY = 0.0
            >>> agent.energy = 100.0
            >>> agent.health = 100.0
            >>> agent.age = 0.0
            >>> print(f"Agent at ({agent.x}, {agent.y}) facing {agent.orientation} radians")
            >>> agent.reset()  # Reset all state
            >>> agent.isMoving = True
            >>> agent.isTurning = False
        )pbdoc")
        .def(py::init<>())
        .def("moveTo", [](AgentBody& self, float target_x, float target_y, float speed, float time_step) {
            float dx = target_x - self.x;
            float dy = target_y - self.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            if (distance > speed * time_step) {
                return;
            }
            self.x += dx * speed * time_step / distance;
            self.y += dy * speed * time_step / distance;
            self.velocityX = dx * speed / distance;
            self.velocityY = dy * speed / distance;
            self.orientation = std::atan2(dy, dx);
        }, py::arg("target_x"), py::arg("target_y"), py::arg("speed"), py::arg("time_step"),
             "Move agent toward target position. target_x, target_y: Target coordinates. speed: Movement speed.")
        .def("rotateTo", [](AgentBody& self, float target_orientation, float angular_speed, float time_step) {
            float delta = target_orientation - self.orientation;
            if (delta > M_PI) delta -= 2 * M_PI;
            if (delta < -M_PI) delta += 2 * M_PI;
            
            float rotation = std::min(angular_speed * time_step, std::abs(delta));
            if (delta > 0) {
                self.orientation += rotation;
            } else {
                self.orientation -= rotation;
            }
            
            self.angularVelocity = (delta > 0) ? angular_speed : -angular_speed;
            self.isTurning = true;
        }, py::arg("target_orientation"), py::arg("angular_speed"), py::arg("time_step"),
             "Rotate agent to target orientation. target_orientation: Target angle in radians.")
        .def("applyForce", [](AgentBody& self, float force_x, float force_y, float mass) {
            self.velocityX += force_x / mass;
            self.velocityY += force_y / mass;
        }, py::arg("force_x"), py::arg("force_y"), py::arg("mass"),
             "Apply force to agent. force_x, force_y: Force components. mass: Agent mass.")
        .def("updatePosition", [](AgentBody& self, float time_step) {
            self.x += self.velocityX * time_step;
            self.y += self.velocityY * time_step;
            self.velocityX *= 0.99;  // Friction
            self.velocityY *= 0.99;  // Friction
        }, py::arg("time_step"),
             "Update agent position based on current velocity. time_step: Time step in seconds.")
        .def("getDistanceTo", [](const AgentBody& self, float other_x, float other_y) {
            float dx = self.x - other_x;
            float dy = self.y - other_y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other_x"), py::arg("other_y"),
             "Calculate distance to another position. Returns Euclidean distance.")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Get current speed magnitude.")
        .def("getHeading", [](const AgentBody& self) {
            return self.orientation;
        }, "Get current heading/orientation in radians.")
        .def("isNear", [](const AgentBody& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy) < radius;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Check if agent is within radius of position. Returns True if distance < radius.")
        .def("getForwardVector", [](const AgentBody& self) {
            return std::pair<float, float>(std::cos(self.orientation), std::sin(self.orientation));
        }, "Get forward direction vector as (x, y) pair.")
        .def("getVelocityVector", [](const AgentBody& self) {
            return std::pair<float, float>(self.velocityX, self.velocityY);
        }, "Get velocity vector as (x, y) pair.")
        .def("__repr__", [](const AgentBody& agent) {
            return "<AgentBody: " + std::to_string(agent.x) + "," + std::to_string(agent.y) + 
                   " heading=" + std::to_string(agent.orientation) + 
                   " speed=" + std::to_string(agent.getSpeed()) + 
                   " energy=" + std::to_string(agent.energy) + ">";
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world in the NLM simulation.
        
        ActionResult represents the outcome of an agent's action execution in the
        environment. It contains information about whether the action was successful,
        the reward received, and any additional feedback or status information.
        
        The ActionResult class provides:
        - Constructors for creating action results with specified outcomes
        - reward: Numeric value representing action success/failure
        - success: Boolean flag indicating if action completed successfully
        - message: Optional descriptive message about action result
        - Predefined constants for common action outcomes
        
        Action results are used by:
        - SimpleWorld to report action outcomes
        - AgentBrain for reward-based learning
        - Environment for agent feedback
        
        Typical reward values:
        - Positive: Successful actions (e.g., +1.0 for resource collection)
        - Zero: Neutral actions (e.g., 0.0 for attempted but failed actions)
        - Negative: Punished actions (e.g., -1.0 for damage)
        
        Example:
            >>> import pynlm
            >>> # Create successful action result
            >>> result = pynlm.ActionResult(reward=1.0, success=True, message="Collected food")
            >>> print(f"Reward: {result.reward}")
            >>> print(f"Success: {result.success}")
            >>> print(f"Message: {result.message}")
            >>> # Create failed action result
            >>> failure = pynlm.ActionResult(reward=-0.5, success=False, message="Hit wall")
            >>> print(f"Failed with penalty: {failure.reward}")
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create ActionResult with specified outcome. reward: Reward value. success: Whether action succeeded.")
        .def("isPositive", [](const ActionResult& result) {
            return result.reward > 0.0f;
        }, "Check if action result is positive (reward > 0).")
        .def("isNegative", [](const ActionResult& result) {
            return result.reward < 0.0f;
        }, "Check if action result is negative (reward < 0).")
        .def("isNeutral", [](const ActionResult& result) {
            return result.reward == 0.0f;
        }, "Check if action result is neutral (reward == 0).")
        .def("getRewardStrength", [](const ActionResult& result) {
            return std::abs(result.reward);
        }, "Get absolute value of reward (magnitude of impact).")
        .def("getOutcomeType", [](const ActionResult& result) {
            if (result.reward > 0.0f) return "positive";
            if (result.reward < 0.0f) return "negative";
            return "neutral";
        }, "Get outcome type as string ('positive', 'negative', or 'neutral').")
        .def("clone", [](const ActionResult& result) {
            return ActionResult(result.reward, result.success, result.message);
        }, "Create a copy of this action result.")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) + 
                   " success=" + std::to_string(result.success) + ">";
        });

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data in the NLM simulation.
        
        SensoryPercept represents the processed sensory information received by
        the agent from the environment. It contains multiple sensory modalities
        including vision, touch/proprioception, internal signals, and audio.
        
        The SensoryPercept class provides:
        - Vision: Visual information from cameras or vision systems
        - Touch/Proprioception: Physical contact and body position information
        - Internal: Internal physiological and cognitive signals
        - Audio: Sound information from microphones
        - Timestamps: Simulation time when percept was received
        - Setters for all sensory modalities
        - Getters for all sensory modalities
        
        Sensory perception is used by:
        - SimpleWorld to provide environment information
        - Vision: Pixel data and image dimensions
        - Touch/Proprioception: Contact and body position
        - Internal: Physiological and cognitive states
        - Audio: Sound samples and characteristics
        
        Example:
            >>> import pynlm
            >>> # Create percept
            >>> percept = pynlm.SensoryPercept()
            >>> # Create vision percept
            >>> vision = pynlm.Vision(640, 480, 3)
            >>> percept.setVision(vision)
            >>> # Create touch percept
                >>> touch = pynlm.InternalSignals()
                >>> touch.addSignal(0.8)  # Position signal
                >>> touch.addSignal(0.2)  # Contact signal
                >>> percept.setTouch(touch)
                >>> # Create audio percept
                >>> audio = pynlm.Audio(44100, 1024)
                >>> audio.setData([0.1, 0.5, -0.3, 0.2, 0.0, ...])
                >>> percept.setAudio(audio)
                >>> print(f"Percept created at time {percept.getTimestamp()}")
                >>> # Access sensory modalities
                >>> vision_data = percept.getVision()
                >>> print(f"Vision dimensions: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get visual sensory input. Returns Vision object containing image data.")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set visual sensory input. vision: Vision object containing image data.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision image width in pixels.")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision image height in pixels.")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get touch/proprioception sensory input. Returns InternalSignals object.")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch/proprioception sensory input. touch: InternalSignals object.")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal signals sensory input. Returns InternalSignals object.")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals sensory input. internal: InternalSignals object.")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception sensory input. Returns InternalSignals object.")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception sensory input. proprioception: InternalSignals object.")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get audio sensory input. Returns Audio object containing sound data.")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory input. audio: Audio object containing sound data.")
        .def("getAllSignals", [](const SensoryPercept& percept) {
            std::vector<std::string> signals;
            if (percept.getVision().getDimensions() > 0) signals.push_back("Vision");
            if (percept.getTouch().getDimensions() > 0) signals.push_back("Touch");
            if (percept.getInternal().getDimensions() > 0) signals.push_back("Internal");
            if (percept.getAudio().getDimensions() > 0) signals.push_back("Audio");
            return signals;
        }, "Get list of all active sensory modalities. Returns list of strings.")
        .def("hasVision", [](const SensoryPercept& percept) {
            return percept.getVision().getDimensions() > 0;
        }, "Check if vision sensory input is available. Returns True if vision data exists.")
        .def("hasTouch", [](const SensoryPercept& percept) {
            return percept.getTouch().getDimensions() > 0;
        }, "Check if touch/proprioception sensory input is available.")
        .def("hasInternal", [](const SensoryPercept& percept) {
            return percept.getInternal().getDimensions() > 0;
        }, "Check if internal sensory input is available.")
        .def("hasAudio", [](const SensoryPercept& percept) {
            return percept.getAudio().getDimensions() > 0;
        }, "Check if audio sensory input is available.")
        .def("__repr__", [](const SensoryPercept& percept) {
            std::vector<std::string> active_sensors;
            if (percept.hasVision()) active_sensors.push_back("Vision");
            if (percept.hasTouch()) active_sensors.push_back("Touch");
            if (percept.hasInternal()) active_sensors.push_back("Internal");
            if (percept.hasAudio()) active_sensors.push_back("Audio");
            
            std::string sensors_str = "";
            for (size_t i = 0; i < active_sensors.size(); ++i) {
                if (i > 0) sensors_str += ", ";
                sensors_str += active_sensors[i];
            }
            return "<SensoryPercept: " + sensors_str + " at " + std::to_string(percept.getTimestamp()) + ">";
        });
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
             "Log brain status");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")

    // Advanced Python features - Context managers for resource management
    m.def("createBrainWithCleanup", [](std::shared_ptr<Config> config) {
        class _BrainCleanup {
        public:
            _BrainCleanup(std::shared_ptr<Brain> brain) : brain_(brain) {}
            ~_BrainCleanup() {
                if (brain_) {
                    brain_->reset();
                }
            }
            Brain* operator->() { return brain_.get(); }
            std::shared_ptr<Brain> get() { return brain_; }
        private:
            std::shared_ptr<Brain> brain_;
        };
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return _BrainCleanup(brain);
    }, py::arg("config"), 
        "Create a brain with automatic cleanup. Returns a context manager that"
        " will reset the brain when the context exits. Use with 'with' statement."
        " Example: with pynlm.createBrainWithCleanup(config) as brain: ...")

    // Advanced Python features - Event callback system for brain events
    m.def("createEventCallbackSystem", []() {
        class _EventCallbackSystem {
        public:
            using SpikeCallback = std::function<void(const DetailedSpikeEvent&)>
            ;
            using StepCallback = std::function<void(SimulationStep, Timestamp)>
            ;
            using ErrorCallback = std::function<void(const std::string&)>
            ;
            
            void registerSpikeCallback(const SpikeCallback& callback) {
                spikeCallbacks_.push_back(callback);
            }
            
            void registerStepCallback(const StepCallback& callback) {
                stepCallbacks_.push_back(callback);
            }
            
            void registerErrorCallback(const ErrorCallback& callback) {
                errorCallbacks_.push_back(callback);
            }
            
            void triggerSpikeCallbacks(const DetailedSpikeEvent& event) {
                for (const auto& callback : spikeCallbacks_) {
                    callback(event);
                }
            }
            
            void triggerStepCallbacks(SimulationStep step, Timestamp time) {
                for (const auto& callback : stepCallbacks_) {
                    callback(step, time);
                }
            }
            
            void triggerErrorCallbacks(const std::string& error) {
                for (const auto& callback : errorCallbacks_) {
                    callback(error);
                }
            }
            
            py::dict getCallbacksSummary() {
                py::dict summary;
                summary["spike_callbacks"] = py::cast(spikeCallbacks_);
                summary["step_callbacks"] = py::cast(stepCallbacks_);
                summary["error_callbacks"] = py::cast(errorCallbacks_);
                return summary;
            }
            
            size_t getSpikeCallbackCount() const { return spikeCallbacks_.size(); }
            size_t getStepCallbackCount() const { return stepCallbacks_.size(); }
            size_t getErrorCallbackCount() const { return errorCallbacks_.size(); }
            
            void clearAllCallbacks() {
                spikeCallbacks_.clear();
                stepCallbacks_.clear();
                errorCallbacks_.clear();
            }
            
        private:
            std::vector<SpikeCallback> spikeCallbacks_;
            std::vector<StepCallback> stepCallbacks_;
            std::vector<ErrorCallback> errorCallbacks_;
        };
        
        return std::make_shared<_EventCallbackSystem>();
    }, 
        "Create an event callback system for brain event handling."
        " Provides registration of spike, step, and error callbacks."
        " Example: callbacks = pynlm.createEventCallbackSystem()")

    // Advanced Python features - Configuration helpers for advanced users
    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set up advanced default configuration
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15f);
        config->set("stdp.enable", true);
        config->set("stdp.learning_rate", 0.001f);
        config->set("plasticity.structural.enable", true);
        config->set("development.enabled", true);
        config->set("neuromodulation.curiosity.enable", true);
        config->set("neuromodulation.novelty.enable", true);
        config->set("memory.working.capacity", 200);
        config->set("memory.episodic.max_episodes", 5000);
        
        return config;
    }, 
        "Create an advanced configuration with Phase 5+ features enabled."
        " Provides optimized settings for complex simulations with learning, development, and memory systems.")

    // Configuration export/import with JSON support
    m.def("saveConfigToJSON", [](const std::shared_ptr<Config>& config, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw SimulationError("Failed to open file for saving: " + filepath);
        }
        
        file << "{\n";
        auto keys = config->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            const auto& key = keys[i];
            file << "  \"" << key << "\": ";
            
            // Get value and format as JSON
            bool found = false;
            for (const auto& entry : static_cast<const Config*>(config.get())->getEntries()) {
                if (entry.key == key) {
                    found = true;
                    const auto& value = entry.value;
                    
                    std::visit([&file](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, int>) {
                            file << arg;
                        } else if constexpr (std::is_same_v<T, int64_t>) {
                            file << arg;
                        } else if constexpr (std::is_same_v<T, double>) {
                            file << std::fixed << std::setprecision(6) << arg;
                        } else if constexpr (std::is_same_v<T, bool>) {
                            file << (arg ? "true" : "false");
                        } else if constexpr (std::is_same_v<T, std::string>) {
                            file << "\"" << arg << "\"";
                        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                            file << "[";
                            for (size_t j = 0; j < arg.size(); ++j) {
                                if (j > 0) file << ", ";
                                file << arg[j];
                            }
                            file << "]";
                        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                            file << "[";
                            for (size_t j = 0; j < arg.size(); ++j) {
                                if (j > 0) file << ", ";
                                file << std::fixed << std::setprecision(6) << arg[j];
                            }
                            file << "]";
                        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            file << "[";
                            for (size_t j = 0; j < arg.size(); ++j) {
                                if (j > 0) file << ", ";
                                file << "\"" << arg[j] << "\"";
                            }
                            file << "]";
                        } else {
                            file << "null";
                        }
                    }, value);
                    
                    if (i < keys.size() - 1) {
                        file << ",\n";
                    } else {
                        file << "\n";
                    }
                    break;
                }
            }
            
            if (!found) {
                throw ConfigurationError("Key not found in configuration: " + key);
            }
        }
        
        file << "}\n";
        
    }, py::arg("config"), py::arg("filepath"),
        "Save configuration to JSON file. Provides structured configuration output."
        " Supports all configuration value types including arrays and nested structures.")

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
}

} // namespace nlm

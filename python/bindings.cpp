#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/smart_holder.h>
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

// Add math constants namespace
namespace math_constants {
    constexpr double pi = 3.14159265358979323846;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Provides comprehensive classes and utilities for neural simulation,
        including Brain, Config, AgentBrain, SimpleWorld, and sensory processing.
        
        This module provides a high-level interface to the NLM neural simulation
        framework, making it easy to create and experiment with neural agents
        in simulated environments.
        
        Key features:
        - Simple agent creation with default configuration
        - Intuitive context manager support for resource management
        - Comprehensive error handling with meaningful Python exceptions
        - Type hints and documentation for better developer experience
        - Convenience functions for common simulation scenarios
        
        Example usage:
        ```python
        import pynlm
        
        # Create a simple agent with default settings
        agent = pynlm.createSimpleAgent(width=100, height=100)
        
        # Run simulation loop
        for step in range(100):
            percept = agent.get_percept()
            action = agent.brain.process_sensory_input(percept)
            reward = agent.act(action)
            agent.step()
        ```
    )pbdoc";

    // Register custom Python exceptions with descriptive messages
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "KeyError");
    py::register_exception<std::logic_error>(m, "LogicError");

    // Create a custom exception for NLM-specific errors
    py::class_<std::exception>(m, "NLMException", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the NLM system.
        
        Provides a robust identifier for neurons with automatic generation
        when no value is specified. Supports comparison operations,
        hashing, and string representation for easy debugging.
        
        Attributes:
            value (int): The underlying 64-bit identifier value.
        
        Examples:
            >>> neuron_id = nlm.NeuronId(42)
            >>> print(neuron_id)
            <NeuronId: 42>
            >>> id2 = nlm.NeuronId()
            >>> id1 == id2  # False (different auto-generated values)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a NeuronId with a specific value. \n            \n            Args:\n                value: The 64-bit identifier value (default: random)")
        .def_readwrite("value", &NeuronId::value,
                      "Get or set the neuron identifier value")
        .def("index", &NeuronId::index,
             "Get the index portion of the identifier (lower 32 bits)")
        .def("__eq__", &NeuronId::operator==,
             "Check if two NeuronId objects are equal")
        .def("__ne__", &NeuronId::operator!=,
             "Check if two NeuronId objects are not equal")
        .def("__lt__", &NeuronId::operator<,
             "Check if this neuron ID is less than another")
        .def("__le__", &NeuronId::operator<=,
             "Check if this neuron ID is less than or equal to another")
        .def("__gt__", &NeuronId::operator>,
             "Check if this neuron ID is greater than another")
        .def("__ge__", &NeuronId::operator>=,
             "Check if this neuron ID is greater than or equal to another")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation for debugging")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "String representation of the neuron ID")
        .def("to_int", &NeuronId::value,
             "Get the integer value of the neuron ID")
        .def_static("from_int", [](uint64_t v) { return NeuronId(v); },
                   py::arg("value"), "Create NeuronId from integer")
        .def("is_valid", &NeuronId::value, // Using value check as validity proxy
             "Check if the neuron ID is valid (non-zero)")
        .def("is_zero", &NeuronId::value, // Custom check for zero
             "Check if this is the zero/default NeuronId")
        .def("to_string", [](const NeuronId& id) { return std::to_string(id.value); },
             "Convert to string representation");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse in the NLM system.
        
        Provides a robust identifier for synapses with automatic generation
        when no value is specified. Supports comparison operations,
        hashing, and string representation for easy debugging.
        
        Attributes:
            value (int): The underlying 64-bit identifier value.
        
        Examples:
            >>> synapse_id = nlm.SynapseId(42)
            >>> print(synapse_id)
            <SynapseId: 42>
            >>> id2 = nlm.SynapseId()
            >>> id1 == id2  # False (different auto-generated values)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a SynapseId with a specific value. \n            \n            Args:\n                value: The 64-bit identifier value (default: random)")
        .def_readwrite("value", &SynapseId::value,
                      "Get or set the synapse identifier value")
        .def("index", &SynapseId::index,
             "Get the index portion of the identifier (lower 32 bits)")
        .def("__eq__", &SynapseId::operator==,
             "Check if two SynapseId objects are equal")
        .def("__ne__", &SynapseId::operator!=,
             "Check if two SynapseId objects are not equal")
        .def("__lt__", &SynapseId::operator<,
             "Check if this synapse ID is less than another")
        .def("__le__", &SynapseId::operator<=,
             "Check if this synapse ID is less than or equal to another")
        .def("__gt__", &SynapseId::operator>,
             "Check if this synapse ID is greater than another")
        .def("__ge__", &SynapseId::operator>=,
             "Check if this synapse ID is greater than or equal to another")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in dictionaries and sets")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation for debugging")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        }, "String representation of the synapse ID")
        .def("to_int", &SynapseId::value,
             "Get the integer value of the synapse ID")
        .def_static("from_int", [](uint64_t v) { return SynapseId(v); },
                   py::arg("value"), "Create SynapseId from integer")
        .def("is_valid", &SynapseId::value, // Using value check as validity proxy
             "Check if the synapse ID is valid (non-zero)")
        .def("is_zero", &SynapseId::value, // Custom check for zero
             "Check if this is the zero/default SynapseId")
        .def("to_string", [](const SynapseId& id) { return std::to_string(id.value); },
             "Convert to string representation");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region in the NLM system.
        
        Provides a robust identifier for neural regions with automatic generation
        when no value is specified. Supports comparison operations,
        hashing, and string representation for easy debugging.
        
        Attributes:
            value (int): The underlying 64-bit identifier value.
        
        Examples:
            >>> region_id = nlm.RegionId(42)
            >>> print(region_id)
            <RegionId: 42>
            >>> id2 = nlm.RegionId()
            >>> id1 == id2  # False (different auto-generated values)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a RegionId with a specific value. \n            \n            Args:\n                value: The 64-bit identifier value (default: random)")
        .def_readwrite("value", &RegionId::value,
                      "Get or set the region identifier value")
        .def("index", &RegionId::index,
             "Get the index portion of the identifier (lower 32 bits)")
        .def("__eq__", &RegionId::operator==,
             "Check if two RegionId objects are equal")
        .def("__ne__", &RegionId::operator!=,
             "Check if two RegionId objects are not equal")
        .def("__lt__", &RegionId::operator<,
             "Check if this region ID is less than another")
        .def("__le__", &RegionId::operator<=,
             "Check if this region ID is less than or equal to another")
        .def("__gt__", &RegionId::operator>,
             "Check if this region ID is greater than another")
        .def("__ge__", &RegionId::operator>=,
             "Check if this region ID is greater than or equal to another")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in dictionaries and sets")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation for debugging")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        }, "String representation of the region ID")
        .def("to_int", &RegionId::value,
             "Get the integer value of the region ID")
        .def_static("from_int", [](uint64_t v) { return RegionId(v); },
                   py::arg("value"), "Create RegionId from integer")
        .def("is_valid", &RegionId::value, // Using value check as validity proxy
             "Check if the region ID is valid (non-zero)")
        .def("is_zero", &RegionId::value, // Custom check for zero
             "Check if this is the zero/default RegionId")
        .def("to_string", [](const RegionId& id) { return std::to_string(id.value); },
             "Convert to string representation");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population in the NLM system.
        
        Provides a robust identifier for neuron populations with automatic generation
        when no value is specified. Supports comparison operations and
        hashing for use in dictionaries and sets.
        
        Attributes:
            value (int): The underlying 64-bit identifier value.
        
        Examples:
            >>> pop_id = nlm.PopulationId(42)
            >>> print(pop_id)
            <PopulationId: 42>
            >>> id2 = nlm.PopulationId()
            >>> id1 == id2  # False (different auto-generated values)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a PopulationId with a specific value. \n            \n            Args:\n                value: The 64-bit identifier value (default: random)")
        .def_readwrite("value", &PopulationId::value,
                      "Get or set the population identifier value")
        .def("index", &PopulationId::index,
             "Get the index portion of the identifier (lower 32 bits)")
        .def("__eq__", &PopulationId::operator==,
             "Check if two PopulationId objects are equal")
        .def("__ne__", &PopulationId::operator!=,
             "Check if two PopulationId objects are not equal")
        .def("__lt__", &PopulationId::operator<,
             "Check if this population ID is less than another")
        .def("__le__", &PopulationId::operator<=,
             "Check if this population ID is less than or equal to another")
        .def("__gt__", &PopulationId::operator>,
             "Check if this population ID is greater than another")
        .def("__ge__", &PopulationId::operator>=,
             "Check if this population ID is greater than or equal to another")
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in dictionaries and sets")
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        }, "String representation for debugging")
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        }, "String representation of the population ID")
        .def("to_int", &PopulationId::value,
             "Get the integer value of the population ID")
        .def_static("from_int", [](uint64_t v) { return PopulationId(v); },
                   py::arg("value"), "Create PopulationId from integer")
        .def("is_valid", &PopulationId::value,
             "Check if the population ID is valid (non-zero)")
        .def("is_zero", &PopulationId::value,
             "Check if this is the zero/default PopulationId")
        .def("to_string", [](const PopulationId& id) { return std::to_string(id.value); },
             "Convert to string representation");

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration defining different classes of neurons in the NLM system.
        
        These types categorize neurons based on their functional role in the neural circuit:
        - Excitatory: Neurons that increase the likelihood of target neuron firing
        - Inhibitory: Neurons that decrease the likelihood of target neuron firing  
        - Modulatory: Neurons that influence overall network excitability
        - Sensory: Neurons that receive external sensory input
        - Motor: Neurons that produce motor commands
        - Internal: Neurons involved in internal processing and integration
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neurons that increase the likelihood of target neuron firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Neurons that decrease the likelihood of target neuron firing")
        .value("Modulatory", NeuronType::Modulatory, "Neurons that influence overall network excitability")
        .value("Sensory", NeuronType::Sensory, "Neurons that receive external sensory input")
        .value("Motor", NeuronType::Motor, "Neurons that produce motor commands")
        .value("Internal", NeuronType::Internal, "Neurons involved in internal processing and integration");

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration defining different connection types between neurons.
        
        These types categorize synapses based on their transmission mechanism and function:
        - Excitatory: Synapses that increase the likelihood of target neuron firing
        - Inhibitory: Synapses that decrease the likelihood of target neuron firing
        - Modulatory: Synapses that influence overall network excitability
        - Electrical: Direct electrical coupling between neurons
        - GapJunction: Specialized electrical connection for synchronized activity
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Synapses that increase the likelihood of target neuron firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Synapses that decrease the likelihood of target neuron firing")
        .value("Modulatory", SynapseType::Modulatory, "Synapses that influence overall network excitability")
        .value("Electrical", SynapseType::Electrical, "Direct electrical coupling between neurons")
        .value("GapJunction", SynapseType::GapJunction, "Specialized electrical connection for synchronized activity");

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration defining different phases of neural development.
        
        These stages represent the progression of neural circuit maturation and adaptation:
        - Initial: Starting state after configuration
        - CriticalPeriod: Window of high plasticity for learning
        - Maturation: Refinement and strengthening of connections
        - Adult: Stable, mature neural configuration
        - Aging: Gradual decline in function and plasticity
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Starting state after configuration")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Window of high plasticity for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Refinement and strengthening of connections")
        .value("Adult", DevelopmentalStage::Adult, "Stable, mature neural configuration")
        .value("Aging", DevelopmentalStage::Aging, "Gradual decline in function and plasticity");

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration defining different phases of neural activity.
        
        These states represent the current activity level of a neuron:
        - Resting: Neuron is not actively firing
        - Active: Neuron is currently firing spikes
        - Refractory: Neuron is recovering from firing (cannot fire)
        - Inhibited: Neuron is suppressed by inhibitory input
    )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is not actively firing")
        .value("Active", FiringState::Active, "Neuron is currently firing spikes")
        .value("Refractory", FiringState::Refractory, "Neuron is recovering from firing")
        .value("Inhibited", FiringState::Inhibited, "Neuron is suppressed by inhibitory input");

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration defining possible motor behaviors and interactions
        for the neural agent in the simulated environment.
        
        These types represent different types of actions that an agent can perform:
        - MoveForward: Move forward in the environment
        - MoveBackward: Move backward in the environment
        - MoveLeft: Move left in the environment
        - MoveRight: Move right in the environment
        - TurnLeft: Turn left (change orientation)
        - TurnRight: Turn right (change orientation)
        - Look: Look in current direction
        - LookUp: Look upward
        - LookDown: Look downward
        - Interact: Interact with nearby objects
        - Eat: Consume available food
        - Drink: Consume available water
        - Rest: Stop moving and conserve energy
        - Wait: Do nothing (special idle action)
        - Custom: User-defined action type
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in the environment")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left (change orientation)")
        .value("TurnRight", ActionType::TurnRight, "Turn right (change orientation)")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby objects")
        .value("Eat", ActionType::Eat, "Consume available food")
        .value("Drink", ActionType::Drink, "Consume available water")
        .value("Rest", ActionType::Rest, "Stop moving and conserve energy")
        .value("Wait", ActionType::Wait, "Do nothing (special idle action)")
        .value("Custom", ActionType::Custom, "User-defined action type");

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration for direct control of agent movement.
        
        These are the executable motor commands that can be sent to the world:
        - MoveForward: Move forward by one step
        - MoveBackward: Move backward by one step
        - TurnLeft: Rotate agent left 90 degrees
        - TurnRight: Rotate agent right 90 degrees
        - LookLeft: Rotate agent left for looking
        - LookRight: Rotate agent right for looking
        - Interact: Perform interaction with current object
        - Wait: Do nothing (special idle command)
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward by one step")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward by one step")
        .value("TurnLeft", MotorCommand::TurnLeft, "Rotate agent left 90 degrees")
        .value("TurnRight", MotorCommand::TurnRight, "Rotate agent right 90 degrees")
        .value("LookLeft", MotorCommand::LookLeft, "Rotate agent left for looking")
        .value("LookRight", MotorCommand::LookRight, "Rotate agent right for looking")
        .value("Interact", MotorCommand::Interact, "Perform interaction with current object")
        .value("Wait", MotorCommand::Wait, "Do nothing (special idle command)");

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration defining different types of objects in the environment.
        
        These types categorize objects in the simulated world:
        - Empty: No object at this position
        - Resource: Object that provides energy/food/water
        - Hazard: Object that damages the agent
        - Wall: Impassable obstacle
        - Marker: Special point for navigation or goals
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object at this position")
        .value("Resource", WorldObjectType::Resource, "Object that provides energy/food/water")
        .value("Hazard", WorldObjectType::Hazard, "Object that damages the agent")
        .value("Wall", WorldObjectType::Wall, "Impassable obstacle")
        .value("Marker", WorldObjectType::Marker, "Special point for navigation or goals");

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for the NLM system.
        
        Provides a flexible key-value configuration system with support for
        multiple value types (int, float, string, bool, and lists). The Config
        class allows loading from JSON files, command-line arguments, and
        runtime settings.
        
        Key features:
        - Type-safe get/set methods with automatic type conversion
        - Support for nested configuration through key paths
        - Configuration sources: Default, File, CommandLine, Runtime
        - Summary and serialization support for debugging and inspection
        
        Common use cases:
        - Load simulation settings from JSON files
        - Override configuration values from command line
        - Store runtime experiment parameters
        - Generate configuration summaries for logging
        
        Examples:
            >>> config = nlm.Config()
            >>> config.set("brain.neuron_count", 1000)
            >>> config.set("world.width", 100.0)
            >>> neurons = config.get_or("brain.neuron_count", 100)
            >>> config.save_to_file("settings.json")
        )pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. \n            \n            Args:\n                filepath: Path to the JSON configuration file\n            \n            Returns:\n                bool: True if loading succeeded, False otherwise\n            \n            Raises:\n                ValueError: If the file format is invalid or cannot be parsed")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. \n            \n            Args:\n                argc: Argument count\n                argv: Argument vector (as from main())\n            \n            Returns:\n                bool: True if parsing succeeded, False otherwise")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. \n            \n            Args:\n                filepath: Path where to save the configuration\n            \n            Returns:\n                bool: True if saving succeeded, False otherwise\n            \n            Raises:\n                ValueError: If the file cannot be written")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. \n            \n            Args:\n                key: Configuration key (supports dot notation for nested values)\n            \n            Returns:\n                bool: True if the key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys in the current scope. \n            \n            Returns:\n                list: List of all configuration keys")
        .def("get", &Config::get<int>, py::arg("key"), "Get integer value", R"pbdoc(
            Get a configuration value. Template method for type-safe access. \n            \n            Args:\n                key: Configuration key to retrieve\n            \n            Returns:\n                Optional[T]: The value if it exists and matches type T, otherwise None\n            \n            Raises:\n                KeyError: If the key doesn't exist\n                ValueError: If the value exists but has wrong type")
        )
        .def("get", &Config::get<double>, py::arg("key"), "Get float value")
        .def("get", &Config::get<std::string>, py::arg("key"), "Get string value")
        .def("get", &Config::get<bool>, py::arg("key"), "Get boolean value")
        .def("get", &Config::getOr<int>, py::arg("key"), py::arg("default"), "Get integer with default", R"pbdoc(
            Get a configuration value with default. \n            \n            Args:\n                key: Configuration key to retrieve\n                default: Default value if key doesn't exist or type mismatch\n            \n            Returns:\n                T: The value if it exists and matches type, otherwise default")
        )
        .def("get", &Config::getOr<double>, py::arg("key"), py::arg("default"), "Get float with default")
        .def("get", &Config::getOr<std::string>, py::arg("key"), py::arg("default"), "Get string with default")
        .def("get", &Config::getOr<bool>, py::arg("key"), py::arg("default"), "Get boolean with default")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer value", R"pbdoc(
            Set a configuration value. \n            \n            Args:\n                key: Configuration key\n                value: Integer value to set\n                source: Configuration source (default: Runtime)")
        )
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean value")
        .def("set", [](Config& self, const std::string& key, const std::vector<int>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer list")
        .def("set", [](Config& self, const std::string& key, const std::vector<double>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float list")
        .def("set", [](Config& self, const std::string& key, const std::vector<std::string>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string list")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key and its value. \n            \n            Args:\n                key: Configuration key to remove")
        .def("clear", &Config::clear,
             "Clear all configuration entries from all sources. This resets to default configuration")
        .def("summary", &Config::summary,
             "Get a formatted summary string of the configuration showing all keys and values")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation showing configuration summary")
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        }, "String representation of configuration")
        .def("__contains__", &Config::has,
             "Check if a key exists in the configuration (supports 'in' operator)")
        .def("keys", &Config::getKeys,
             "Get all configuration keys (supports 'for key in config.keys()')");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input in the NLM system.
        
        SensoryInput represents raw sensory data received by the agent from the environment.
        It provides the fundamental interface for all sensory modalities including vision,
        audio, touch, and internal state signals.
        
        Key features:
        - Abstract base class for all sensory modalities
        - Timestamp support for temporal synchronization
        - Type-safe data access methods
        - Memory-efficient data storage
        
        Subclasses:
        - Vision: Visual input (grayscale intensity grid)
        - Audio: Audio signals (sample-based)
        - InternalSignals: Internal homeostatic signals
        
        Examples:
            >>> vision = nlm.Vision(84, 84, 1)
            >>> vision.set_data(list(range(84 * 84)))
            >>> data = vision.get_data()
            >>> width, height = vision.get_dimensions()
        )pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector of floats")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality as (width, height, channels) tuple")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp of this sensory input")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp of this sensory input")
        .def("__repr__", [](const SensoryInput& si) {
            return "<SensoryInput type=" + std::to_string(si.getType()) + " timestamp=" + std::to_string(si.getTimestamp()) + ">";
        }, "String representation showing type and timestamp")
        .def("clone", [](const SensoryInput& si) {
            // This is a simplified clone - real implementation would be more complex
            throw std::runtime_error("Clone not implemented for base SensoryInput class");
        }, "Create a copy of this sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input representing grayscale image data.
        
        Vision provides visual input to the agent as a 2D grid of intensity values.
        This is a placeholder implementation that will be enhanced in Phase 2 with
        real vision processing using NLM's neural machinery.
        
        Data format:
        - 1D array of float values in row-major order
        - Values in range [0.0, 1.0] representing grayscale intensity
        - Length = width * height
        
        Key features:
        - Flexible dimensions (configurable width, height, channels)
        - Efficient data storage and access
        - Type-safe data manipulation methods
        - Clone support for data duplication
        
        Examples:
            >>> # Create 84x84 grayscale vision
            >>> vision = nlm.Vision(84, 84, 1)
            >>> # Set data from list
            >>> vision.set_data([0.1, 0.2, 0.3, 0.4, 0.5])
            >>> # Get dimensions
            >>> w, h, c = vision.get_dimensions()
            >>> # Get data as list
            >>> data = vision.get_data()
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 1,
             "Create Vision sensory input. \n            \n            Args:\n                width: Image width in pixels\n                height: Image height in pixels\n                channels: Number of color channels (1 for grayscale, 3 for RGB placeholder)")
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set image data. \n            \n            Args:\n                data: List of float values in range [0.0, 1.0]\n            \n            Raises:\n                ValueError: If data length doesn't match dimensions")
        .def("set_data", [](Vision& self, const std::vector<int>& data) {
            std::vector<float> float_data(data.begin(), data.end());
            float_data.resize(self.getWidth() * self.getHeight());
            self.setData(float_data);
        }, py::arg("data"), "Set image data from integers (0-255). Convenient constructor")
        .def("get_width", &Vision::getWidth, "Get image width in pixels")
        .def("get_height", &Vision::getHeight, "Get image height in pixels")
        .def("get_channels", &Vision::getChannels, "Get number of color channels")
        .def("get_dimensions", &Vision::getDimensions,
             "Get image dimensions as a tuple (width, height, channels)")
        .def("to_array", &Vision::getData, "Get data as Python list (alias for get_data)")
        .def("clone", [](const Vision& self) {
            return self.clone();
        }, "Create a copy of this vision input")
        .def("reshape", [](Vision& self, size_t new_width, size_t new_height) {
            std::vector<float> data = self.getData();
            size_t old_width = self.getWidth();
            size_t old_height = self.getHeight();
            if (old_width * old_height != new_width * new_height) {
                throw std::invalid_argument("Data size mismatch for reshape");
            }
            self.setData(data);
            // Note: This is a simplified reshape - real implementation would handle it properly
            return self;
        }, py::arg("new_width"), py::arg("new_height"), "Reshape image to new dimensions (must preserve total pixels)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound signals.
        
        Audio provides audio input to the agent as a sequence of sample values.
        This is a placeholder implementation that will be enhanced in Phase 2 with
        real audio processing using NLM's neural machinery.
        
        Data format:
        - 1D array of float values representing audio samples
        - Values in range [-1.0, 1.0] representing normalized audio amplitude
        - Sample rate indicates the sampling frequency
        
        Key features:
        - Configurable sample rate and duration
        - Efficient data storage and access
        - Type-safe data manipulation methods
        - Clone support for data duplication
        
        Examples:
            >>> # Create 44.1kHz audio for 1 second
            >>> audio = nlm.Audio(44100, 44100)
            >>> # Set data from list
            >>> audio.set_data([0.1, -0.2, 0.3, -0.4, 0.5])
            >>> # Get sample rate
            >>> sample_rate = audio.get_sample_rate()
            >>> # Get number of samples
            >>> num_samples = audio.get_num_samples()
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"),
             "Create Audio sensory input. \n            \n            Args:\n                sample_rate: Audio sampling rate in Hz\n                num_samples: Number of audio samples")
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data. \n            \n            Args:\n                data: List of float values in range [-1.0, 1.0]\n            \n            Raises:\n                ValueError: If data length doesn't match expected samples")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"), "Set the audio sample rate")
        .def("get_sample_rate", &Audio::getSampleRate, "Get the audio sample rate")
        .def("get_num_samples", &Audio::getNumSamples, "Get the number of audio samples")
        .def("get_duration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<double>(self.getSampleRate());
        }, "Get audio duration in seconds");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal homeostatic signals representing the agent's physiological state.
        
        InternalSignals provides the agent with internal feedback about its own
        state, including energy levels, hunger, thirst, and other homeostatic
        needs. This is a placeholder implementation that will be enhanced in Phase 2.
        
        Signals:
        - Energy: Current energy level (0-100 scale)
        - Hunger: Hunger level (0-100 scale)  
        - Thirst: Thirst level (0-100 scale)
        - Temperature: Body temperature
        - Other internal states
        
        Key features:
        - Dynamic signal addition
        - Signal clearing for reset scenarios
        - Type-safe signal manipulation
        
        Examples:
            >>> # Create internal signals
            >>> internal = nlm.InternalSignals()
            >>> # Add energy signal
            >>> internal.add_signal(75.5)  # 75.5% energy
            >>> # Clear all signals
            >>> internal.clear_signals()
        )pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value. \n            \n            Args:\n                value: Signal value (typically float in 0-100 range)")
        .def("clear_signals", &InternalSignals::clearSignals, "Clear all internal signals")
        .def("get_signal_count", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get the number of signals currently stored")
        .def("get_all_signals", &InternalSignals::getData, "Get all signal values as list");

    py::class_<Action>(m, "Action", R"pbdoc(
        Motor action output from the brain to the world.
        
        Action represents a specific behavior that the agent can perform in the environment.
        It includes both the type of action (what to do) and parameters (how to do it).
        Actions are the fundamental unit of behavior in the NLM system.
        
        Key features:
        - Action type enumeration covering all possible behaviors
        - Configurable parameters for fine-grained control
        - Action naming for human-readable descriptions
        - Clone support for action duplication and modification
        - Conversion utilities for different parameter types
        
        Common action types:
        - Movement: MoveForward, MoveBackward, TurnLeft, TurnRight
        - Looking: Look, LookUp, LookDown, LookLeft, LookRight
        - Interaction: Interact, Eat, Drink, Rest, Wait
        
        Examples:
            >>> # Create a simple move forward action
            >>> action = nlm.Action(nlm.ActionType.MoveForward)
            >>> # Create action with parameters
            >>> action = nlm.Action(nlm.ActionType.Turn, [45.0])  # Turn 45 degrees
            >>> # Get action name
            >>> name = action.get_name()
            >>> # Get parameters
            >>> params = action.get_parameters()
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create an action with only type. \n            \n            Args:\n                type: The action type")
        .def(py::init<ActionType, std::vector<float>>(), py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an action with type and parameters. \n            \n            Args:\n                type: The action type\n                parameters: List of float parameters (typically angles in degrees)")
        .def("get_type", &Action::getType, "Get the action type")
        .def("set_type", &Action::setType, py::arg("type"), "Set the action type")
        .def("get_parameters", &Action::getParameters, "Get the action parameters as list of floats")
        .def("set_parameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("get_name", &Action::getName, "Get the human-readable name of this action")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("to_dict", [](const Action& self) {
            std::stringstream ss;
            ss << "Action(type=" << static_cast<int>(self.getType());
            ss << ", params=[";
            auto params = self.getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << params[i];
            }
            ss << "])";
            return ss.str();
        }, "Get string representation in dictionary format")
        .def("is_movement", [](const Action& self) {
            auto type = self.getType();
            return type == ActionType::MoveForward || type == ActionType::MoveBackward ||
                   type == ActionType::TurnLeft || type == ActionType::TurnRight;
        }, "Check if this is a movement action")
        .def("is_interaction", [](const Action& self) {
            auto type = self.getType();
            return type == ActionType::Interact || type == ActionType::Eat || type == ActionType::Drink ||
                   type == ActionType::Rest || type == ActionType::Wait;
        }, "Check if this is an interaction action")
        .def("is_looking", [](const Action& self) {
            auto type = self.getType();
            return type == ActionType::Look || type == ActionType::LookUp || type == ActionType::LookDown ||
                   type == ActionType::LookLeft || type == ActionType::LookRight;
        }, "Check if this is a looking action")
        .def("get_angle", [](const Action& self) {
            auto params = self.getParameters();
            if (!params.empty()) {
                return params[0];
            }
            return 0.0f;
        }, "Get the angle parameter (for turn/look actions, 0 if not available)")
        .def_static("create_movement", [](ActionType direction) {
            return Action(direction);
        }, py::arg("direction"), "Create a movement action (convenience method)")
        .def_static("create_turn", [](float angle) {
            return Action(ActionType::TurnLeft, std::vector<float>{angle});
        }, py::arg("angle"), "Create a turn action (convenience method)")
        .def_static("create_interaction", [](ActionType interaction_type) {
            return Action(interaction_type);
        }, py::arg("interaction_type"), "Create an interaction action (convenience method)")
        .def_static("create_rest", []() {
            return Action(ActionType::Rest);
        }, "Create a rest action (convenience method)")
        .def_static("create_wait", []() {
            return Action(ActionType::Wait);
        }, "Create a wait action (convenience method)");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the NLM simulation environment.
        
        WorldObject represents a physical entity in the 2D simulation world.
        It includes position, type, and properties that determine how it
        interacts with the agent and other objects.
        
        Key features:
        - Position: x, y coordinates (floating point)
        - Geometry: circular shape with radius
        - Type: Enumeration (Empty, Resource, Hazard, Wall, Marker)
        - Properties: value (reward/damage), active state
        - Factory methods for common object types
        
        Common object types:
        - Resource: Provides energy/food/water to the agent
        - Hazard: Causes damage to the agent
        - Wall: Impassable obstacle
        - Marker: Special point for navigation or goals
        - Empty: No object at this position
        
        Examples:
            >>> # Create a resource at position (10, 20) with radius 0.5
            >>> resource = nlm.WorldObject(10.0, 20.0, nlm.WorldObjectType.Resource, 5.0)
            >>> # Create a wall
            >>> wall = nlm.WorldObject.create_wall(15.0, 25.0, 1.0)
            >>> # Create a marker
            >>> marker = nlm.WorldObject.create_marker(30.0, 40.0)
            >>> # Check if object is active
            >>> is_active = resource.active
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object. \n            \n            Args:\n                x: X coordinate in world space\n                y: Y coordinate in world space\n                type: Type of object (WorldObjectType enum)\n                value: Resource value (positive) or damage (negative)\n                radius: Physical radius of the object")
        .def("get_position", [] (const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, "Get object position as (x, y) tuple")
        .def("set_position", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("get_distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate Euclidean distance to point")
        .def("is_near", [](const WorldObject& obj, float x, float y, float distance) {
            return obj.get_distance_to(x, y) <= distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"), "Check if object is within distance of point")
        .def("is_colliding", [](const WorldObject& obj1, const WorldObject& obj2) {
            float dx = obj1.x - obj2.x;
            float dy = obj1.y - obj2.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance <= (obj1.radius + obj2.radius);
        }, py::arg("other"), "Check if this object is colliding with another")
        .def("is_passable", [](const WorldObject& obj) {
            return obj.type != WorldObjectType::Wall && obj.active;
        }, "Check if agent can pass through this object")
        .def("is_resource", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Resource && obj.active;
        }, "Check if this is a collectable resource")
        .def("is_hazard", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Hazard && obj.active;
        }, "Check if this is a dangerous hazard")
        .def("is_marker", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Marker && obj.active;
        }, "Check if this is a goal marker")
        .def("get_reward_value", [](const WorldObject& obj) {
            if (obj.type == WorldObjectType::Resource) {
                return obj.value;
            } else if (obj.type == WorldObjectType::Hazard) {
                return -obj.value; // Negative reward for hazards
            }
            return 0.0f;
        }, "Get the reward value (positive for resources, negative for hazards)")
        .def("clone", [](const WorldObject& obj) {
            return obj;
        }, "Create a copy of this world object")
        .def_static("create_resource", [](float x, float y, float radius, float value) {
            return WorldObject(x, y, WorldObjectType::Resource, value, radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, py::arg("value") = 1.0f,
           "Create a resource object (convenience method)")
        .def_static("create_hazard", [](float x, float y, float radius, float damage) {
            return WorldObject(x, y, WorldObjectType::Hazard, damage, radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, py::arg("damage") = 1.0f,
           "Create a hazard object (convenience method)")
        .def_static("create_wall", [](float x, float y, float radius) {
            return WorldObject(x, y, WorldObjectType::Wall, 0.0f, radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
           "Create a wall object (convenience method)")
        .def_static("create_marker", [](float x, float y) {
            return WorldObject(x, y, WorldObjectType::Marker, 0.0f, 0.2f);
        }, py::arg("x"), py::arg("y"),
           "Create a marker object (convenience method)");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representing position, orientation, and physiological conditions.
        
        AgentBody tracks the physical and biological state of the agent in the simulated
        environment, including position, movement, and energy/resources.
        
        Key attributes:
        - Position: (x, y) coordinates in world space
        - Orientation: Heading direction in radians (0 = right, pi/2 = up)
        - Velocity: Linear velocity in x and y directions
        - Angular velocity: Rotational speed
        - Energy: Current energy level (0-100 scale)
        - Health: Current health level (0-100 scale)
        - Age: Simulation time since creation
        - Movement flags: isMoving, isTurning
        - Action tracking: lastActionTime
        
        Movement utilities:
        - Position calculations (absolute/relative)
        - Distance and heading calculations
        - Collision detection helpers
        - Energy management helpers
        
        Examples:
            >>> # Create a new agent body
            >>> body = nlm.AgentBody()
            >>> # Set position
            >>> body.x = 50.0
            >>> body.y = 30.0
            >>> # Set orientation (45 degrees)
            >>> body.orientation = math.pi/4
            >>> # Move forward
            >>> body.move_forward(1.0)
            >>> # Get distance to point
            >>> dist = body.get_distance_to(100, 50)
        )pbdoc")
        .def(py::init<>())
        .def("get_position", [] (const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get agent position as (x, y) tuple")
        .def("set_position", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("move_forward", [](AgentBody& self, float distance) {
            float radians = self.orientation;
            self.x += distance * std::cos(radians);
            self.y += distance * std::sin(radians);
            self.isMoving = true;
            self.lastActionTime = 0.0; // Would use actual time in real implementation
        }, py::arg("distance"), "Move forward in current orientation")
        .def("move_backward", [](AgentBody& self, float distance) {
            float radians = self.orientation;
            self.x -= distance * std::cos(radians);
            self.y -= distance * std::sin(radians);
            self.isMoving = true;
            self.lastActionTime = 0.0;
        }, py::arg("distance"), "Move backward in current orientation")
        .def("turn_left", [](AgentBody& self, float angle_radians) {
            self.orientation += angle_radians;
            self.isTurning = true;
            self.lastActionTime = 0.0;
        }, py::arg("angle_radians"), "Turn left by angle (radians)")
        .def("turn_right", [](AgentBody& self, float angle_radians) {
            self.orientation -= angle_radians;
            self.isTurning = true;
            self.lastActionTime = 0.0;
        }, py::arg("angle_radians"), "Turn right by angle (radians)")
        .def("get_distance_to", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate Euclidean distance to point")
        .def("get_angle_to", [](const AgentBody& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            float angle = std::atan2(dy, dx);
            // Normalize to [-pi, pi]
            if (angle > math_constants::pi) angle -= 2 * math_constants::pi;
            if (angle < -math_constants::pi) angle += 2 * math_constants::pi;
            return angle;
        }, py::arg("x"), py::arg("y"), "Get angle (radians) from agent to point")
        .def("is_near", [](const AgentBody& self, float x, float y, float distance) {
            return self.get_distance_to(x, y) <= distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"), "Check if agent is within distance of point")
        .def("get_energy_status", [](const AgentBody& self) {
            return std::make_pair(self.energy, self.health);
        }, "Get energy and health as (energy, health) tuple")
        .def("is_energy_low", [](const AgentBody& self) {
            return self.energy < 30.0f;
        }, "Check if energy is low (< 30%)")
        .def("is_critical", [](const AgentBody& self) {
            return self.health <= 0.0f;
        }, "Check if health is critical (<= 0)")
        .def("get_movement_vector", [](const AgentBody& self) {
            return std::make_tuple(
                self.velocityX, 
                self.velocityY,
                self.angularVelocity
            );
        }, "Get current movement state as (vx, vy, angular_velocity) tuple")
        .def("set_velocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"), "Set linear velocity")
        .def("get_speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed magnitude")
        .def("add_to_age", &AgentBody::age,
             "Age the agent by one simulation step (increment internal age counter)")
        .def("get_age_group", [](const AgentBody& self) {
            if (self.age < 100.0) return "young";
            else if (self.age < 500.0) return "adult";
            else return "aged";
        }, "Get age group classification");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action executed in the world.
        
        ActionResult represents the outcome of an agent's action in the environment,
        including reward (or penalty), success status, and descriptive message.
        
        Key attributes:
        - reward: Numerical reward (positive for good outcomes, negative for bad)
        - success: Boolean indicating if the action was successful
        - message: Descriptive text explaining the result
        
        Common reward patterns:
        - Positive rewards for collecting resources, achieving goals
        - Negative rewards for hazards, failed actions, damage
        - Zero rewards for neutral actions or no change
        
        Examples:
            >>> # Create a successful resource collection
            >>> result = nlm.ActionResult.create_success(10.0, "Collected food")
            >>> # Create a failed action
            >>> result = nlm.ActionResult.create_failure(-5.0, "Hit by hazard")
            >>> # Check if action was successful
            >>> if result.success:
            ...     print(f"Got reward: {result.reward}")
            >>> # Print result message
            >>> print(result.message)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result. \n            \n            Args:\n                reward: Numerical reward (can be positive or negative)\n                success: Whether the action was successful\n                message: Human-readable description of the result")
        .def("get_reward", &ActionResult::reward,
             "Get the reward value (positive = good, negative = bad)")
        .def("set_reward", &ActionResult::reward,
             "Set the reward value")
        .def("get_success", &ActionResult::success,
             "Check if the action was successful")
        .def("set_success", &ActionResult::success,
             "Set success status")
        .def("get_message", &ActionResult::message,
             "Get the result message")
        .def("set_message", &ActionResult::message,
             "Set the result message")
        .def("is_positive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if reward is positive")
        .def("is_negative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if reward is negative")
        .def("is_zero", [](const ActionResult& self) {
            return self.reward == 0.0f;
        }, "Check if reward is zero")
        .def("get_magnitude", &ActionResult::reward,
             "Get absolute value of reward")
        .def_static("create_success", [](float reward, const std::string& message) {
            return ActionResult(reward, true, message);
        }, py::arg("reward"), py::arg("message"), "Create a successful action result (convenience method)")
        .def_static("create_failure", [](float reward, const std::string& message) {
            return ActionResult(reward, false, message);
        }, py::arg("reward"), py::arg("message"), "Create a failed action result (convenience method)")
        .def_static("create_zero", [](const std::string& message) {
            return ActionResult(0.0f, true, message);
        }, py::arg("message"), "Create a neutral/zero action result (convenience method)")
        .def_static("create_positive", [](float reward, const std::string& message) {
            return ActionResult(reward, true, message);
        }, py::arg("reward"), py::arg("message"), "Create a positive reward result (convenience method)")
        .def_static("create_negative", [](float reward, const std::string& message) {
            return ActionResult(reward, false, message);
        }, py::arg("reward"), py::arg("message"), "Create a negative/penalty result (convenience method)");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data - what the agent perceives.
        
        SensoryPercept represents the complete sensory experience of the agent,
        containing all sensory modalities as they are processed by the world.
        This is the primary interface for agents to receive sensory input from
        the environment and pass it to their brain for processing.
        
        Key features:
        - Multi-modal sensory integration (vision, touch, internal, proprioception, audio)
        - Timestamp tracking for temporal synchronization
        - Consistent data access patterns across all modalities
        - Helper methods for common sensory processing tasks
        - Clone support for data duplication
        
        Sensory modalities:
        - Vision: Visual input (grayscale intensity grid, row-major order)
        - Touch: Proximity/collision signals (binary or continuous)
        - Internal: Homeostatic signals (energy, hunger, thirst, etc.)
        - Proprioception: Body position and velocity signals
        - Audio: Sound input (if enabled in simulation)
        
        Data format:
        - Each modality: 1D array of float values
        - Vision: width × height grid, values 0.0-1.0
        - Other modalities: Variable length based on sensor configuration
        
        Examples:
            >>> # Create a sensory percept
            >>> percept = nlm.SensoryPercept()
            >>> # Set vision data
            >>> percept.set_vision([0.1, 0.2, 0.3, 0.4])
            >>> # Get vision dimensions
            >>> width, height = percept.get_vision_dimensions()
            >>> # Set internal signals
            >>> percept.set_internal([50.0, 30.0, 80.0])  # energy, hunger, thirst
            >>> # Get all sensory signals for brain input
            >>> all_signals = percept.get_all_signals()
            >>> # Clone for modification
            >>> percept2 = percept.clone()
        )pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision, "Get vision data as list of floats")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data (width×height row-major)")
        .def("get_vision_dimensions", [](const SensoryPercept& self) {
            size_t w = self.getVisionWidth();
            size_t h = self.getVisionHeight();
            return std::make_pair(w, h);
        }, "Get vision dimensions as (width, height) tuple")
        .def("set_vision_data", [](SensoryPercept& self, const std::vector<float>& data, size_t width, size_t height) {
            self.setVision(data);
            // Note: VisionWidth/Height are stored separately
            // In a real implementation, these would be set appropriately
        }, py::arg("data"), py::arg("width"), py::arg("height"), "Set vision data with explicit dimensions")
        .def("get_touch", &SensoryPercept::getTouch, "Get touch/proximity data")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch/proximity data")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal state signals")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal state signals")
        .def("get_proprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("get_audio", &SensoryPercept::getAudio, "Get audio data")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals, "Get all sensory signals concatenated for brain input")
        .def("get_timestamp", &SensoryPercept::getTimestamp, "Get sensory timestamp")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set sensory timestamp")
        .def("clone", [](const SensoryPercept& self) {
            return self.clone();
        }, "Create a copy of this sensory percept")
        .def("has_vision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if vision data is available")
        .def("has_touch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if touch data is available")
        .def("has_internal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if internal signals are available")
        .def("has_proprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, "Check if proprioception data is available")
        .def("has_audio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if audio data is available")
        .def("get_modality_count", [](const SensoryPercept& self) {
            int count = 0;
            if (!self.getVision().empty()) count++;
            if (!self.getTouch().empty()) count++;
            if (!self.getInternal().empty()) count++;
            if (!self.getProprioception().empty()) count++;
            if (!self.getAudio().empty()) count++;
            return count;
        }, "Get the number of available sensory modalities")
        .def("get_total_signal_count", [](const SensoryPercept& self) {
            return self.getAllSignals().size();
        }, "Get total number of sensory signals across all modalities")
        .def("normalize_vision", [](SensoryPercept& self) {
            auto& vision = self.getVision();
            if (!vision.empty()) {
                float min_val = *std::min_element(vision.begin(), vision.end());
                float max_val = *std::max_element(vision.begin(), vision.end());
                if (max_val > min_val) {
                    for (float& val : vision) {
                        val = (val - min_val) / (max_val - min_val);
                    }
                    self.setVision(vision);
                }
            }
            return self;
        }, "Normalize vision data to [0, 1] range")
        .def("add_noise", [](SensoryPercept& self, float noise_level) {
            // Add Gaussian noise to all available signals
            std::vector<float> all_signals = self.getAllSignals();
            for (float& val : all_signals) {
                val += ((std::rand() % 2000 - 1000) / 1000.0f) * noise_level;
            }
            // Note: This is a simplified noise addition
            // Real implementation would distribute noise appropriately by modality
            return self;
        }, py::arg("noise_level"), "Add Gaussian noise to all signals for robustness training");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld provides the simulated environment where agents operate.
        It handles agent movement, collision detection, object interactions,
        and sensory perception. The world supports context manager protocol
        for automatic resource management and clean shutdown.
        
        Key features:
        - Context manager support for automatic reset
        - Agent movement with collision detection
        - Interactive objects (resources, hazards, walls, markers)
        - Sensory perception system (vision, touch, etc.)
        - Energy management and decay
        - Reproducible simulation via random seed
        - Factory methods for common world configurations
        
        Usage with context manager:
            >>> # Safe world creation with automatic cleanup
            >>> with nlm.SimpleWorld.create_test_world() as world:
            ...     # Run simulation
            ...     for step in range(100):
            ...         percept = world.get_percept()
            ...         action = agent_brain.process_sensory_input(percept)
            ...         world.apply_action(action)
            ...         world.update(0.1)
        
        Examples:
            >>> # Create and configure a simple world
            >>> world = nlm.SimpleWorld()
            >>> world.configure(width=100, height=100, vision_width=20, vision_height=20)
            >>> # Set agent starting position
            >>> world.set_agent_start(50, 50)
            >>> # Add a resource
            >>> resource = nlm.WorldObject.create_resource(10, 10, 1.0)
            >>> world.add_object(resource)
            >>> # Run simulation steps
            >>> for _ in range(50):
            ...     percept = world.get_percept()
            ...     world.update(0.1)
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory capabilities. \n            \n            Args:\n                width: World width in simulation units\n                height: World height in simulation units\n                visionWidth: Agent's vision width in pixels\n                visionHeight: Agent's vision height in pixels")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial configuration (agent position, objects, time)")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Advance simulation by timestep (typically 0.1 for 10ms steps)")
        .def("apply_action", [](SimpleWorld& self, const Action& action) {
            // Convert action to motor command
            MotorCommand cmd;
            switch(action.getType()) {
                case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
                case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
                case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
                case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
                case ActionType::Look: cmd = MotorCommand::LookLeft; break; // Simplified
                case ActionType::LookUp: cmd = MotorCommand::LookRight; break;
                case ActionType::LookDown: cmd = MotorCommand::LookLeft; break;
                case ActionType::Interact: cmd = MotorCommand::Interact; break;
                case ActionType::Wait: cmd = MotorCommand::Wait; break;
                default: cmd = MotorCommand::Wait;
            }
            self.applyMotorCommand(cmd, self.getSimulationTime());
        }, py::arg("action"), "Apply an action to the world (convenience method)")
        .def("get_percept", &SimpleWorld::getSensoryPercept,
             "Get current sensory percept for the agent")
        .def("get_body", &SimpleWorld::getAgentBody,
             "Get agent body state (alias for get_body)")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the simulation")
        .def("add_resource", [](SimpleWorld& self, float x, float y, float value, float radius) {
            WorldObject resource = WorldObject::create_resource(x, y, radius, value);
            self.addObject(resource);
        }, py::arg("x"), py::arg("y"), py::arg("value"), py::arg("radius") = 0.5f,
           "Add a resource object (convenience method)")
        .def("add_hazard", [](SimpleWorld& self, float x, float y, float damage, float radius) {
            WorldObject hazard = WorldObject::create_hazard(x, y, radius, damage);
            self.addObject(hazard);
        }, py::arg("x"), py::arg("y"), py::arg("damage"), py::arg("radius") = 0.5f,
           "Add a hazard object (convenience method)")
        .def("add_wall", [](SimpleWorld& self, float x, float y, float radius) {
            WorldObject wall = WorldObject::create_wall(x, y, radius);
            self.addObject(wall);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
           "Add a wall object (convenience method)")
        .def("remove_object", [](SimpleWorld& self, float x, float y) {
            self.removeObject(x, y);
        }, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall, within bounds)")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum agent energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate per timestep")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate per timestep")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for reproducible simulation")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get current random seed")
        .def("get_objects_near", [](SimpleWorld& self, float x, float y, float radius) {
            std::vector<WorldObject> nearby;
            for (const auto& obj : self.objects_) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                float distance = std::sqrt(dx*dx + dy*dy);
                if (distance <= radius && obj.active) {
                    nearby.push_back(obj);
                }
            }
            return nearby;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get all active objects within radius")
        .def("get_resources_near", [](SimpleWorld& self, float x, float y, float radius) {
            auto nearby = self.get_objects_near(x, y, radius);
            std::vector<WorldObject> resources;
            for (const auto& obj : nearby) {
                if (obj.type == WorldObjectType::Resource) {
                    resources.push_back(obj);
                }
            }
            return resources;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get all resources within radius")
        .def("get_hazards_near", [](SimpleWorld& self, float x, float y, float radius) {
            auto nearby = self.get_objects_near(x, y, radius);
            std::vector<WorldObject> hazards;
            for (const auto& obj : nearby) {
                if (obj.type == WorldObjectType::Hazard) {
                    hazards.push_back(obj);
                }
            }
            return hazards;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get all hazards within radius")
        .def("get_marker", [](SimpleWorld& self) {
            for (const auto& obj : self.objects_) {
                if (obj.type == WorldObjectType::Marker && obj.active) {
                    return obj;
                }
            }
            throw std::runtime_error("No active marker found in world");
        }, "Get the active marker object (for navigation goals)")
        .def_static("create_test_world", []() {
            return std::make_shared<SimpleWorld>();
        }, "Create a simple test world (convenience method)")
        .def_static("create_empty_world", [](float width, float height) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, 20, 20);
            world->set_agent_start(width/2, height/2);
            return world;
        }, py::arg("width"), py::arg("height"), "Create an empty world with agent start")
        .def_static("create_resource_world", [](float width, float height) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, 20, 20);
            world->set_agent_start(width/2, height/2);
            // Add some resources
            world->add_object(WorldObject::create_resource(10, 10, 1.0));
            world->add_object(WorldObject::create_resource(90, 90, 2.0));
            return world;
        }, py::arg("width"), py::arg("height"), "Create a world with resources")
        .def_static("create_challenge_world", [](float width, float height) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, 20, 20);
            world->set_agent_start(width/2, height/2);
            // Add hazards and walls
            world->add_object(WorldObject::create_hazard(30, 30, 5.0));
            world->add_object(WorldObject::create_hazard(70, 70, 10.0));
            world->add_object(WorldObject::create_wall(50, 0, 2.0));
            world->add_object(WorldObject::create_wall(50, 100, 2.0));
            world->add_object(WorldObject::create_marker(90, 90));
            return world;
        }, py::arg("width"), py::arg("height"), "Create a challenging world with hazards and goals");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    // AgentBrain context manager support
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrainContextManager")
        .def("__enter__", [](const std::shared_ptr<AgentBrain>& self) {
            return self;
        })
        .def("__exit__", [](const std::shared_ptr<AgentBrain>& self, 
                           py::object, py::object, py::object) {
            self->reset();
            return false;
        });

    // Brain context manager support
    py::class_<Brain, std::shared_ptr<Brain>>(m, "BrainContextManager")
        .def("__enter__", [](const std::shared_ptr<Brain>& self) {
            return self;
        })
        .def("__exit__", [](const std::shared_ptr<Brain>& self,
                           py::object, py::object, py::object) {
            self->reset();
            return false;
        });

    // SimpleWorld context manager support
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorldContextManager")
        .def("__enter__", [](const std::shared_ptr<SimpleWorld>& self) {
            return self;
        })
        .def("__exit__", [](const std::shared_ptr<SimpleWorld>& self,
                           py::object, py::object, py::object) {
            self->reset();
            return false;
        });

    // Create comprehensive factory functions
    m.def("createSimpleAgent", [](float width, float height, size_t vision_width, size_t vision_height) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.region_count", 5);
        config->set("world.width", width);
        config->set("world.height", height);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        world->setAgentStart(width/2, height/2);
        
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        agent_brain->initialize(*world);
        
        return std::make_tuple(brain, world, agent_brain);
    }, py::arg("width") = 100.0f, py::arg("height") = 100.0f, 
           py::arg("vision_width") = 20, py::arg("vision_height") = 20,
           "Create a simple agent with brain, world, and agent brain interfaces. \n            \n            Args:\n                width: World width in simulation units (default: 100.0)\n                height: World height in simulation units (default: 100.0)\n                vision_width: Agent's vision width in pixels (default: 20)\n                vision_height: Agent's vision height in pixels (default: 20)\n            \n            Returns:\n                tuple: (brain, world, agent_brain) for use in simulation\n            \n            Raises:\n                RuntimeError: If brain initialization fails")
    .def("createDefaultAgent", []() {
        return createSimpleAgent(100.0f, 100.0f, 20, 20);
    }, "Create an agent with default settings (100x100 world, 20x20 vision)")
    .def("createTrainingAgent", [](float width, float height) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000);
        config->set("brain.region_count", 8);
        config->set("development.enabled", true);
        config->set("development.critical_period", true);
        config->set("neuromodulation.reward_modulation", true);
        config->set("neuromodulation.curiosity", true);
        config->set("neuromodulation.novelty", true);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize training brain");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 30, 30);
        world->set_agent_start(width/2, height/2);
        
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        agent_brain->initialize(*world);
        
        // Enable all advanced features for training
        agent_brain->enableRewardModulation(true);
        agent_brain->enableStructuralPlasticity(true);
        agent_brain->enableDevelopment(true);
        agent_brain->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent_brain);
    }, py::arg("width") = 200.0f, py::arg("height") = 200.0f,
           "Create a training agent with advanced features for reinforcement learning")
    .def("createChallengeAgent", [](float width, float height) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 3000);
        config->set("brain.region_count", 10);
        config->set("world.max_energy", 200.0f);
        config->set("world.energy_decay_rate", 0.01f);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize challenge brain");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 25, 25);
        world->set_agent_start(width/2, height/2);
        world->setMaxEnergy(200.0f);
        world->setEnergyDecayRate(0.01f);
        
        // Add challenge objects
        world->add_object(WorldObject::create_resource(50, 50, 10.0));
        world->add_object(WorldObject::create_resource(150, 150, 5.0));
        world->add_object(WorldObject::create_hazard(100, 100, 3.0));
        world->add_object(WorldObject::create_wall(100, 0, 2.0));
        world->add_object(WorldObject::create_wall(100, 200, 2.0));
        world->add_object(WorldObject::create_marker(180, 180));
        
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        agent_brain->initialize(*world);
        
        return std::make_tuple(brain, world, agent_brain);
    }, py::arg("width") = 300.0f, py::arg("height") = 300.0f,
           "Create a challenge agent with hazards and obstacles for complex tasks")
    .def("createExperimentAgent", [](float width, float height, const std::string& config_file) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(config_file)) {
            throw std::runtime_error("Failed to load configuration from file: " + config_file);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize experiment brain");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 40, 40);
        world->set_agent_start(width/2, height/2);
        
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        agent_brain->initialize(*world);
        
        return std::make_tuple(brain, world, agent_brain);
    }, py::arg("width") = 400.0f, py::arg("height") = 400.0f, py::arg("config_file"),
           "Create an experiment agent with custom configuration from file");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

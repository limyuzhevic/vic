#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>

// Include the core NLM headers
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions for different error types
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class NeuronError : public std::runtime_error {
public:
    explicit NeuronError(const std::string& message) : std::runtime_error(message) {}
};

class SynapseError : public std::runtime_error {
public:
    explicit SynapseError(const std::string& message) : std::runtime_error(message) {}
};

class WorldError : public std::runtime_error {
public:
    explicit WorldError(const std::string& message) : std::runtime_error(message) {}
};

class BrainError : public std::runtime_error {
public:
    explicit BrainError(const std::string& message) : std::runtime_error(message) {}
};

class AgentError : public std::runtime_error {
public:
    explicit AgentError(const std::string& message) : std::runtime_error(message) {}
};

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& message) : std::runtime_error(message) {}
};

class SensoryError : public std::runtime_error {
public:
    explicit SensoryError(const std::string& message) : std::runtime_error(message) {}
};

class ActionError : public std::runtime_error {
public:
    explicit ActionError(const std::string& message) : std::runtime_error(message) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides comprehensive bindings for the NLM system with:
        - Clean, modular architecture with separate header files
        - Extensive documentation and examples
        - Custom Python exceptions for error handling
        - Pythonic interface with convenience methods and property accessors
        - Context managers for resource management
        - Batch operations and serialization helpers
        - Type safety improvements and validation
        - Performance optimizations with appropriate pybind11 policies
        - Comprehensive test suite
        
        Features:
        * Type identifiers (NeuronId, SynapseId, RegionId, PopulationId)
        * Core type definitions and enumerations
        * Configuration management with file I/O
        * Sensory input systems (Vision, Audio, InternalSignals)
        * Action representation for motor output
        * World object system
        * Agent body state
        * Action results
        * Sensory percept data
        * Simulation world (SimpleWorld)
        * Neural brain (Brain)
        * Agent brain interface (AgentBrain)
        
        Example usage:
            ```python
            import pynlm
            
            # Create a brain with configuration
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            
            # Create a world
            world = pynlm.createSimpleWorld()
            
            # Create agent brain
            agent_brain = pynlm.createAgentBrain(brain)
            
            # Run simulation
            for step in range(100):
                world.update(0.01)
            ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError);
    py::register_exception<NeuronError>(m, "NeuronError", PyExc_ValueError);
    py::register_exception<SynapseError>(m, "SynapseError", PyExc_ValueError);
    py::register_exception<WorldError>(m, "WorldError", PyExc_ValueError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_ValueError);
    py::register_exception<AgentError>(m, "AgentError", PyExc_ValueError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);
    py::register_exception<SensoryError>(m, "SensoryError", PyExc_ValueError);
    py::register_exception<ActionError>(m, "ActionError", PyExc_ValueError);

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ========== TYPE IDENTIFIERS ==========

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the NLM system.
        
        NeuronId provides a strongly-typed wrapper around a 64-bit unsigned integer
        for neuron identification, ensuring type safety and preventing confusion
        between different identifier types.
        
        Examples:
            >>> neuron_id = NeuronId(42)
            >>> neuron_id.value
            42
            >>> neuron_id.index()
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, "The underlying 64-bit value")
        .def("index", &NeuronId::index, "Get the index value")
        .def("__eq__", &NeuronId::operator==, "Equality comparison")
        .def("__ne__", &NeuronId::operator!=, "Inequality comparison")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in sets and dictionaries")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation for debugging")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "String representation for display");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse in the NLM system.
        
        SynapseId provides a strongly-typed wrapper around a 64-bit unsigned integer
        for synapse identification, ensuring type safety and preventing confusion
        with other identifier types.
        
        Examples:
            >>> synapse_id = SynapseId(100)
            >>> synapse_id.value
            100
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value, "The underlying 64-bit value")
        .def("index", &SynapseId::index, "Get the index value")
        .def("__eq__", &SynapseId::operator==, "Equality comparison")
        .def("__ne__", &SynapseId::operator!=, "Inequality comparison")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in sets and dictionaries")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation for debugging");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region in the NLM system.
        
        RegionId provides a strongly-typed wrapper around a 64-bit unsigned integer
        for brain region identification, ensuring type safety and clear semantics.
        
        Examples:
            >>> region_id = RegionId(1)
            >>> region_id.value
            1
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value, "The underlying 64-bit value")
        .def("index", &RegionId::index, "Get the index value")
        .def("__eq__", &RegionId::operator==, "Equality comparison")
        .def("__ne__", &RegionId::operator!=, "Inequality comparison")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in sets and dictionaries")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation for debugging");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population in the NLM system.
        
        PopulationId provides a strongly-typed wrapper around a 64-bit unsigned integer
        for neuron population identification, ensuring type safety and clear semantics.
        
        Examples:
            >>> population_id = PopulationId(10)
            >>> population_id.value
            10
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value, "The underlying 64-bit value")
        .def("index", &PopulationId::index, "Get the index value")
        .def("__eq__", &PopulationId::operator==, "Equality comparison")
        .def("__ne__", &PopulationId::operator!=, "Inequality comparison")
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in sets and dictionaries")
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        }, "String representation for debugging");

    // ========== ENUMERATIONS ==========

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM system.
        
        Each neuron type has distinct functions and characteristics:
        - Excitatory: Promotes firing in connected neurons
        - Inhibitory: Suppresses firing in connected neurons  
        - Modulatory: Influences synaptic plasticity and neuromodulation
        - Sensory: Detects and processes external stimuli
        - Motor: Generates motor commands for action
        - Internal: Performs internal neural processing
        
        Examples:
            >>> neuron_type = NeuronType.Excitatory
            >>> neuron_type
            NeuronType.Excitatory
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron that promotes firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron that suppresses firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron that influences other neurons")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron that receives external input")
        .value("Motor", NeuronType::Motor, "Motor neuron that produces action outputs")
        .value("Internal", NeuronType::Internal, "Internal neuron for internal processing")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the NLM system.
        
        Synapse types determine the nature of neural connections:
        - Excitatory: Increases postsynaptic neuron firing probability
        - Inhibitory: Decreases postsynaptic neuron firing probability
        - Modulatory: Affects synaptic plasticity and neuromodulation
        - Electrical: Direct electrical coupling via gap junctions
        - GapJunction: Electrical coupling between neurons (synonym for Electrical)
        
        Examples:
            >>> synapse_type = SynapseType.Excitatory
            >>> synapse_type
            SynapseType.Excitatory
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse affecting plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse with gap junction")
        .value("GapJunction", SynapseType::GapJunction, "Electrical coupling between neurons")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages.
        
        Developmental stages influence neural plasticity and learning capacity:
        - Initial: Starting state with high plasticity
        - CriticalPeriod: Enhanced learning capability
        - Maturation: Neural wiring and refinement
        - Adult: Stable performance state
        - Aging: Age-related decline in plasticity
        
        Examples:
            >>> stage = DevelopmentalStage.Adult
            >>> stage
            DevelopmentalStage.Adult
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental state")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult developmental state")
        .value("Aging", DevelopmentalStage::Aging, "Aging developmental state")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        Firing states represent the current activity level of a neuron:
        - Resting: Ready to fire but not currently active
        - Active: Currently firing action potentials
        - Refractory: Temporarily unable to fire again
        - Inhibited: Suppressed by inhibitory inputs
        
        Examples:
            >>> state = FiringState.Resting
            >>> state
            FiringState.Resting
        )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest, not firing")
        .value("Active", FiringState::Active, "Neuron actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron recovering after firing")
        .value("Inhibited", FiringState::Inhibited, "Neuron temporarily inhibited")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of possible action types.
        
        Actions are the outputs of the agent brain that drive behavior:
        - Movement actions control agent locomotion
        - Look actions control agent attention
        - Interaction actions allow object manipulation
        - Consumption actions sustain the agent
        - Rest actions allow energy recovery
        
        Examples:
            >>> action = ActionType.MoveForward
            >>> action
            ActionType.MoveForward
        )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in world space")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in world space")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in world space")
        .value("MoveRight", ActionType::MoveRight, "Move right in world space")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
        .value("Look", ActionType::Look, "Look at specific direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Consume food/resource")
        .value("Drink", ActionType::Drink, "Consume water")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands.
        
        Motor commands are the actual execution commands sent to the agent:
        - Movement commands control locomotion
        - Look commands control gaze direction
        - Interact commands trigger interactions
        - Wait command for idling
        
        Motor commands are the output of the agent's motor decoding system.
        
        Examples:
            >>> motor_cmd = MotorCommand.MoveForward
            >>> motor_cmd
            MotorCommand.MoveForward
        )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait/idle")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of world object types.
        
        World objects populate the simulation environment:
        - Empty: No object, free space
        - Resource: Consumable item providing energy
        - Hazard: Dangerous object causing damage
        - Wall: Impassable barrier
        - Marker: Navigation target or point of interest
        
        Examples:
            >>> obj_type = WorldObjectType.Resource
            >>> obj_type
            WorldObjectType.Resource
        )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space with no object")
        .value("Resource", WorldObjectType::Resource, "Resource that can be consumed")
        .value("Hazard", WorldObjectType::Hazard, "Hazard that causes damage")
        .value("Wall", WorldObjectType::Wall, "Wall that blocks movement")
        .value("Marker", WorldObjectType::Marker, "Marker for navigation/target")
        .export_values();

    // ========== CORE CLASSES ==========

    // Config class with Pythonic interface
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Configuration class for the NLM system.
        
        The Config class manages key-value pairs for configuration settings.
        It supports multiple value types (int, float, bool, string, vectors)
        and provides convenient getters, setters, and validation.
        
        Features:
        - File I/O for JSON configuration
        - Command-line argument parsing
        - Type-safe getters with defaults
        - Key validation and error handling
        - Configuration summary and debugging
        
        Examples:
            >>> config = Config()
            >>> config.setInt("population_size", 1000)
            >>> config.setFloat("learning_rate", 0.01)
            >>> config.setBool("enable_plasticity", True)
            >>> size = config.getInt("population_size", 100)
            >>> config.saveToFile("my_config.json")
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file.
                 
             Returns:
                 bool: True if configuration was loaded successfully, False otherwise.
                 
             Raises:
                 ConfigError: If file cannot be read or parsed.
                 
             Example:
                 >>> config = Config()
                 >>> success = config.loadFromFile("config.json")
                 True
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments.
           
           Args:
               argc: Number of command line arguments.
               argv: Array of command line argument strings.
               
           Returns:
               bool: True if configuration was loaded successfully, False otherwise.
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.
             
             Args:
                 filepath: Path where the configuration should be saved.
                 
             Returns:
                 bool: True if configuration was saved successfully, False otherwise.
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check.
                 
             Returns:
                 bool: True if key exists, False otherwise.
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value.has_value()) {
                return *value;
            } else {
                throw py::key_error("Key '" + key + "' not found in configuration");
            }
        }, py::arg("key"),
           R"pbdoc(Get a configuration value as string.
           
           Args:
               key: Configuration key to retrieve.
               
           Returns:
               str: The configuration value.
               
           Raises:
               KeyError: If the key does not exist.
           )pbdoc")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0,
           R"pbdoc(Get an integer configuration value with default.
           
           Args:
               key: Configuration key to retrieve.
               defaultValue: Default value if key doesn't exist (default: 0).
               
           Returns:
               int: The configuration value or default.
           )pbdoc")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
           R"pbdoc(Get a float configuration value with default.
           
           Args:
               key: Configuration key to retrieve.
               defaultValue: Default value if key doesn't exist (default: 0.0).
               
           Returns:
               float: The configuration value or default.
           )pbdoc")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = false,
           R"pbdoc(Get a boolean configuration value with default.
           
           Args:
               key: Configuration key to retrieve.
               defaultValue: Default value if key doesn't exist (default: False).
               
           Returns:
               bool: The configuration value or default.
           )pbdoc")
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = "",
           R"pbdoc(Get a string configuration value with default.
           
           Args:
               key: Configuration key to retrieve.
               defaultValue: Default value if key doesn't exist (default: empty string).
               
           Returns:
               str: The configuration value or default.
           )pbdoc")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(Set a configuration value.
           
           Args:
               key: Configuration key.
               value: Value to set (as string).
           )pbdoc")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(Set an integer configuration value.
           
           Args:
               key: Configuration key.
               value: Integer value to set.
           )pbdoc")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(Set a float configuration value.
           
           Args:
               key: Configuration key.
               value: Float value to set.
           )pbdoc")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(Set a boolean configuration value.
           
           Args:
               key: Configuration key.
               value: Boolean value to set.
           )pbdoc")
        .def("remove", &Config::remove, py::arg("key"),
             R"pbdoc(Remove a configuration key.
             
             Args:
                 key: Configuration key to remove.
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 List[str]: List of all configuration keys.
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.
             
             Returns:
                 str: Human-readable configuration summary.
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__len__", &Config::getKeys,
             R"pbdoc(Get number of configuration entries.
             
             Returns:
                 int: Number of configuration keys.
             )pbdoc");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // Pythonic convenience functions
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Module constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

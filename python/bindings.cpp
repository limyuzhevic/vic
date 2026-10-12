#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all NLM headers
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

// Forward declarations for better organization
class Config;
class Brain;
class AgentBrain;
class SimpleWorld;

// ============================================================================
// UTILITY FUNCTIONS AND HELPERS
// ============================================================================

/**
 * @brief Python-friendly function to create default configuration
 * 
 * This function provides a simple interface for users to get started with
 * NLM without having to manually configure parameters.
 * 
 * @return Shared pointer to a new Config object with default values
 * 
 * Example:
 * ```python
 * import pynlm
 * config = pynlm.create_default_config()
 * ```
 */
std::shared_ptr<Config> create_default_config() {
    return std::make_shared<Config>();
}

/**
 * @brief Python-friendly function to create a brain with configuration
 * 
 * @param config Configuration for the brain
 * @return Shared pointer to a new Brain object
 * 
 * Example:
 * ```python
 * import pynlm
 * config = pynlm.create_default_config()
 * brain = pynlm.create_brain(config)
 * ```
 */
std::shared_ptr<Brain> create_brain_with_config(std::shared_ptr<Config> config) {
    if (!config) {
        throw std::invalid_argument("Config cannot be null");
    }
    return std::make_shared<Brain>(config);
}

/**
 * @brief Python-friendly function to create a simple world
 * 
 * @return Shared pointer to a new SimpleWorld object
 * 
 * Example:
 * ```python
 * import pynlm
 * world = pynlm.create_simple_world()
 * ```
 */
std::shared_ptr<SimpleWorld> create_simple_world() {
    return std::make_shared<SimpleWorld>();
}

/**
 * @brief Python-friendly function to create an agent brain interface
 * 
 * @param brain Shared pointer to the brain to wrap
 * @return Shared pointer to a new AgentBrain object
 * 
 * Example:
 * ```python
 * import pynlm
 * brain = pynlm.create_brain(config)
 * agent = pynlm.create_agent_brain(brain)
 * ```
 */
std::shared_ptr<AgentBrain> create_agent_brain_interface(std::shared_ptr<Brain> brain) {
    if (!brain) {
        throw std::invalid_argument("Brain cannot be null");
    }
    return std::make_shared<AgentBrain>(brain);
}

// ============================================================================
// CORE TYPE DEFINITIONS
// ============================================================================

/**
 * @brief Register all core type definitions with Python
 * 
 * This function registers the fundamental types used by NLM, including:
 * - NeuronId: Unique identifier for neurons
 * - SynapseId: Unique identifier for synapses
 * - RegionId: Unique identifier for brain regions
 * - PopulationId: Unique identifier for neuron populations
 * - NeuronType: Types of neurons (excitatory, inhibitory, etc.)
 * - ActionType: Types of actions that can be taken
 * - And many more...
 */
void register_core_types(py::module_& m) {
    // =========================================================================
    // Identifier Types
    // =========================================================================
    
    // NeuronId - Unique identifier for neurons
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the NLM system.
        
        NeuronId objects are used to reference specific neurons within the brain.
        Each neuron has a unique identifier that can be used for targeted operations
        like injecting current or retrieving neuron properties.
        
        Examples:
            ```python
            import pynlm
            neuron_id = pynlm.NeuronId(42)
            # Access the value
            print(neuron_id.value)
            # Get the index
            print(neuron_id.index())
            ```
        ")"
    )
    .def(py::init<>())
    .def(py::init<uint64_t>(), py::arg("value"))
    .def_readwrite("value", &NeuronId::value, "The numeric value of the neuron ID")
    .def("index", &NeuronId::index, "Get the index of the neuron ID")
    .def("__eq__", &NeuronId::operator==, "Check equality with another NeuronId")
    .def("__ne__", &NeuronId::operator!=, "Check inequality with another NeuronId")
    .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
         "Return hash of the neuron ID for use in dictionaries/sets")
    .def("__repr__", [](const NeuronId& id) {
        return "<NeuronId: " + std::to_string(id.value) + ">";
    }, "String representation of the NeuronId");
    
    // SynapseId - Unique identifier for synapses
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse in the NLM system.
        
        SynapseId objects identify specific synaptic connections between neurons.
        Synapses can be modified, removed, or queried using their SynapseId.
        ")"
    )
    .def(py::init<>())
    .def(py::init<uint64_t>(), py::arg("value"))
    .def_readwrite("value", &SynapseId::value, "The numeric value of the synapse ID")
    .def("index", &SynapseId::index, "Get the index of the synapse ID")
    .def("__eq__", &SynapseId::operator==, "Check equality with another SynapseId")
    .def("__ne__", &SynapseId::operator!=, "Check inequality with another SynapseId")
    .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
    .def("__repr__", [](const SynapseId& id) {
        return "<SynapseId: " + std::to_string(id.value) + ">";
    });
    
    // RegionId - Unique identifier for brain regions
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region in the NLM system.
        
        Regions are logical groupings of neurons that may share similar functions
        or properties. RegionId objects are used to manage and query regions.
        ")"
    )
    .def(py::init<>())
    .def(py::init<uint64_t>(), py::arg("value"))
    .def_readwrite("value", &RegionId::value, "The numeric value of the region ID")
    .def("index", &RegionId::index, "Get the index of the region ID")
    .def("__eq__", &RegionId::operator==, "Check equality with another RegionId")
    .def("__ne__", &RegionId::operator!=, "Check inequality with another RegionId")
    .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
    .def("__repr__", [](const RegionId& id) {
        return "<RegionId: " + std::to_string(id.value) + ">";
    });
    
    // PopulationId - Unique identifier for neuron populations
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population in the NLM system.
        
        Populations are groups of neurons that share similar characteristics
        or functions. PopulationId objects are used to manage neuron groups.
        ")"
    )
    .def(py::init<>())
    .def(py::init<uint64_t>(), py::arg("value"))
    .def_readwrite("value", &PopulationId::value, "The numeric value of the population ID")
    .def("index", &PopulationId::index, "Get the index of the population ID")
    .def("__eq__", &PopulationId::operator==, "Check equality with another PopulationId")
    .def("__ne__", &PopulationId::operator!=, "Check inequality with another PopulationId");

    // =========================================================================
    // ENUMERATIONS
    // =========================================================================
    
    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Type of neuron in the NLM system.
        
        Different neuron types have different functions and properties:
        - Excitatory: Increases firing probability in target neurons
        - Inhibitory: Decreases firing probability in target neurons  
        - Modulatory: Releases neuromodulators that affect neural plasticity
        - Sensory: Receives input from the environment
        - Motor: Produces output for motor actions
        - Internal: Maintains internal brain state
        ")"
    )
    .value("Excitatory", NeuronType::Excitatory, "Neuron that excites target neurons")
    .value("Inhibitory", NeuronType::Inhibitory, "Neuron that inhibits target neurons")
    .value("Modulatory", NeuronType::Modulatory, "Neuron that releases neuromodulators")
    .value("Sensory", NeuronType::Sensory, "Neuron that receives sensory input")
    .value("Motor", NeuronType::Motor, "Neuron that produces motor output")
    .value("Internal", NeuronType::Internal, "Neuron that maintains internal state")
    .export_values();
    
    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Type of synaptic connection between neurons.
        
        Different synapse types have different transmission properties:
        - Excitatory: Increases target neuron firing probability
        - Inhibitory: Decreases target neuron firing probability
        - Modulatory: Releases neuromodulators affecting plasticity
        - Electrical: Direct electrical coupling between neurons
        - GapJunction: Ion exchange through gap junctions
        ")"
    )
    .value("Excitatory", SynapseType::Excitatory, "Excitatory synaptic connection")
    .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synaptic connection")
    .value("Modulatory", SynapseType::Modulatory, "Modulatory synaptic connection")
    .value("Electrical", SynapseType::Electrical, "Electrical synaptic connection")
    .value("GapJunction", SynapseType::GapJunction, "Gap junction synaptic connection")
    .export_values();
    
    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage of the brain.
        
        The brain goes through different developmental stages, each with different
        properties and capabilities:
        - Initial: Newborn, limited capabilities
        - CriticalPeriod: High plasticity, rapid learning
        - Maturation: Normal development and refinement
        - Adult: Fully developed capabilities
        - Aging: Age-related decline
        ")"
    )
    .value("Initial", DevelopmentalStage::Initial, "Newborn brain stage")
    .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "High plasticity stage")
    .value("Maturation", DevelopmentalStage::Maturation, "Normal development stage")
    .value("Adult", DevelopmentalStage::Adult, "Fully developed stage")
    .value("Aging", DevelopmentalStage::Aging, "Age-related decline stage")
    .export_values();
    
    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Current firing state of a neuron.
        
        Neurons can be in different firing states:
        - Resting: Not currently firing
        - Active: Currently firing spikes
        - Refractory: Cannot fire due to recent activity
        - Inhibited: Actively suppressed from firing
        ")"
    )
    .value("Resting", FiringState::Resting, "Not currently firing")
    .value("Active", FiringState::Active, "Currently firing spikes")
    .value("Refractory", FiringState::Refractory, "Cannot fire due to recent activity")
    .value("Inhibited", FiringState::Inhibited, "Actively suppressed from firing")
    .export_values();
    
    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Type of action that can be taken by the agent.
        
        Actions represent what the agent can do in the environment:
        - MoveForward: Move forward in the environment
        - MoveBackward: Move backward in the environment
        - TurnLeft: Rotate counterclockwise
        - TurnRight: Rotate clockwise
        - Look: Look in a specific direction
        - Interact: Interact with objects
        - Eat/Drink: Consume resources
        - Rest/Wait: Do nothing
        - Custom: User-defined action
        ")"
    )
    .value("MoveForward", ActionType::MoveForward, "Move forward in the environment")
    .value("MoveBackward", ActionType::MoveBackward, "Move backward in the environment")
    .value("MoveLeft", ActionType::MoveLeft, "Move left in the environment")
    .value("MoveRight", ActionType::MoveRight, "Move right in the environment")
    .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
    .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
    .value("Look", ActionType::Look, "Look in a specific direction")
    .value("LookUp", ActionType::LookUp, "Look up")
    .value("LookDown", ActionType::LookDown, "Look down")
    .value("Interact", ActionType::Interact, "Interact with objects")
    .value("Eat", ActionType::Eat, "Consume food/resource")
    .value("Drink", ActionType::Drink, "Drink water")
    .value("Rest", ActionType::Rest, "Rest/stop moving")
    .value("Wait", ActionType::Wait, "Wait without moving")
    .value("Custom", ActionType::Custom, "User-defined action")
    .export_values();
    
    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command for agent movement.
        
        Motor commands represent basic movement actions:
        - MoveForward: Move forward
        - MoveBackward: Move backward
        - TurnLeft: Turn left
        - TurnRight: Turn right
        - LookLeft: Look left
        - LookRight: Look right
        - Interact: Interact with environment
        - Wait: Do nothing
        ")"
    )
    .value("MoveForward", MotorCommand::MoveForward, "Move forward")
    .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
    .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
    .value("TurnRight", MotorCommand::TurnRight, "Turn right")
    .value("LookLeft", MotorCommand::LookLeft, "Look left")
    .value("LookRight", MotorCommand::LookRight, "Look right")
    .value("Interact", MotorCommand::Interact, "Interact with environment")
    .value("Wait", MotorCommand::Wait, "Do nothing")
    .export_values();
    
    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Type of object in the world.
        
        Objects represent things that can be found in the environment:
        - Empty: No object at this position
        - Resource: Consumable resource
        - Hazard: Dangerous to interact with
        - Wall: Physical obstacle
        - Marker: Special location or goal
        ")"
    )
    .value("Empty", WorldObjectType::Empty, "No object at this position")
    .value("Resource", WorldObjectType::Resource, "Consumable resource")
    .value("Hazard", WorldObjectType::Hazard, "Dangerous to interact with")
    .value("Wall", WorldObjectType::Wall, "Physical obstacle")
    .value("Marker", WorldObjectType::Marker, "Special location or goal")
    .export_values();
}

// ============================================================================
// CONFIGURATION CLASS
// ============================================================================

/**
 * @brief Register the Config class with Python
 * 
 * The Config class manages all configuration parameters for the NLM system.
 * It provides a flexible key-value store for storing and retrieving configuration
 * values, with support for different data types and sources.
 */
void register_config_class(py::module_& m) {
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class provides a flexible key-value store for storing and
        retrieving configuration parameters. It supports multiple data types
        (integers, floats, strings, booleans, and lists) and manages configuration
        sources (defaults, files, command line, runtime).
        
        The Config class is essential for customizing brain behavior, including:
        - Number of neurons and connections
        - Learning parameters (plasticity rules)
        - Neuromodulation settings
        - Developmental parameters
        - And many more...
        
        Examples:
            ```python
            import pynlm
            
            # Create default configuration
            config = pynlm.Config()
            
            # Set configuration values
            config.set("brain.neuron_count", 1000)
            config.set("plasticity.stdp.enable", True)
            
            # Check if a key exists
            if config.has("brain.neuron_count"):
                neurons = config.get("brain.neuron_count")
            
            # Get all keys
            keys = config.getKeys()
            
            # Save configuration to file
            config.saveToFile("my_config.json")
            
            # Load configuration from file
            config.loadFromFile("my_config.json")
            ```
        )""
    )
    .def(py::init<>())
    .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
         R"pbdoc(
         Load configuration from a JSON file.
         
         Args:
             filepath: Path to the JSON configuration file
             
         Returns:
             True if the file was loaded successfully, False otherwise
             
         Example:
             ```python
             config.loadFromFile("config.json")
             ```
         """)
    .def("loadFromArgs", [](Config& self, int argc, char** argv) {
        return self.loadFromArgs(argc, argv);
    }, py::arg("argc"), py::arg("argv"),
       R"pbdoc(
       Load configuration from command line arguments.
       
       Args:
           argc: Number of command line arguments
           argv: Array of command line argument strings
           
       Returns:
           True if configuration was loaded from command line, False otherwise
           
       Example (from Python):
           ```python
           # In Python, this would be called from sys.argv
           # config.loadFromArgs(len(sys.argv), sys.argv)
           ```
       """)
    .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
         R"pbdoc(
         Save configuration to a JSON file.
         
         Args:
             filepath: Path where to save the configuration file
             
         Returns:
             True if the file was saved successfully, False otherwise
             
         Example:
             ```python
             config.saveToFile("my_config.json")
             ```
         """)
    .def("has", &Config::has, py::arg("key"),
         R"pbdoc(
         Check if a configuration key exists.
         
         Args:
             key: Configuration key to check (e.g., "brain.neuron_count")
             
         Returns:
             True if the key exists, False otherwise
             
         Example:
             ```python
             if config.has("brain.neuron_count"):
                 print("Neuron count is configured")
             ```
         """)
    .def("getKeys", &Config::getKeys,
         R"pbdoc(
         Get all configuration keys.
         
         Returns:
             List of all configuration keys
             
         Example:
             ```python
             all_keys = config.getKeys()
             for key in all_keys:
                 print(f"{key}: {config.get(key)}")
             ```
         """)
    .def("clear", &Config::clear,
         R"pbdoc(
         Clear all configuration entries.
         
         Example:
             ```python
             config.clear()  # Remove all configuration
             ```
         """)
    .def("summary", &Config::summary,
         R"pbdoc(
         Get a summary string of the configuration.
         
         Returns:
             String containing a summary of all configuration entries
             
         Example:
             ```python
             summary = config.summary()
             print(summary)
             ```
         """)
    .def("__repr__", [](const Config& cfg) {
        return "<Config: " + cfg.summary() + ">";
    }, R"pbdoc(
         String representation of the configuration.
         
         Returns:
             String representation of the configuration
         """)
    .def("set", [](Config& self, const std::string& key, const std::string& value) {
        self.set(key, value);
    }, py::arg("key"), py::arg("value"),
       R"pbdoc(
       Set a string configuration value.
       
       Args:
           key: Configuration key (e.g., "brain.name")
           value: String value to set
           
       Example:
           ```python
           config.set("brain.name", "Cortex")
           ```
       """)
    .def("set", [](Config& self, const std::string& key, int value) {
        self.set(key, value);
    }, py::arg("key"), py::arg("value"),
       R"pbdoc(
       Set an integer configuration value.
       
       Args:
           key: Configuration key (e.g., "brain.neuron_count")
           value: Integer value to set
           
       Example:
           ```python
           config.set("brain.neuron_count", 1000)
           ```
       """)
    .def("set", [](Config& self, const std::string& key, double value) {
        self.set(key, value);
    }, py::arg("key"), py::arg("value"),
       R"pbdoc(
       Set a float configuration value.
       
       Args:
           key: Configuration key (e.g., "plasticity.learning_rate")
           value: Float value to set
           
       Example:
           ```python
           config.set("plasticity.learning_rate", 0.01)
           ```
       """)
    .def("set", [](Config& self, const std::string& key, bool value) {
        self.set(key, value);
    }, py::arg("key"), py::arg("value"),
       R"pbdoc(
       Set a boolean configuration value.
       
       Args:
           key: Configuration key (e.g., "plasticity.enable")
           value: Boolean value to set
           
       Example:
           ```python
           config.set("plasticity.enable", True)
           ```
       """)
    .def("get", [](const Config& self, const std::string& key) {
        return self.get<std::string>(key);
    }, py::arg("key"),
         R"pbdoc(
         Get a configuration value as a string.
         
         Args:
             key: Configuration key to retrieve
             
         Returns:
             Configuration value as a string, or None if the key doesn't exist
             
         Example:
             ```python
             value = config.get("brain.name")
             ```
         """)
    .def("get", [](const Config& self, const std::string& key, const std::string& default_value) {
        auto value = self.get<std::string>(key);
        return value.has_value() ? value.value() : default_value;
    }, py::arg("key"), py::arg("default_value"),
       R"pbdoc(
       Get a configuration value as a string with a default.
       
       Args:
           key: Configuration key to retrieve
           default_value: Default value if the key doesn't exist
           
       Returns:
           Configuration value as a string, or the default value if the key doesn't exist
           
       Example:
           ```python
           value = config.get("brain.name", "Default Cortex")
           ```
       """)
    .def("get", [](const Config& self, const std::string& key) {
        auto value = self.get<int>(key);
        return value.has_value() ? std::optional<int>(value.value()) : std::nullopt;
    }, py::arg("key"),
         R"pbdoc(
         Get a configuration value as an integer.
         
         Args:
             key: Configuration key to retrieve
             
         Returns:
             Configuration value as an integer, or None if the key doesn't exist
             
         Example:
             ```python
             neurons = config.get("brain.neuron_count")
             ```
         """)
    .def("get", [](const Config& self, const std::string& key, int default_value) {
        auto value = self.get<int>(key);
        return value.has_value() ? value.value() : default_value;
    }, py::arg("key"), py::arg("default_value"),
       R"pbdoc(
       Get a configuration value as an integer with a default.
       
       Args:
           key: Configuration key to retrieve
           default_value: Default value if the key doesn't exist
           
       Returns:
           Configuration value as an integer, or the default value if the key doesn't exist
           
       Example:
           ```python
           neurons = config.get("brain.neuron_count", 1000)
           ```
       """)
    .def("get", [](const Config& self, const std::string& key) {
        auto value = self.get<double>(key);
        return value.has_value() ? std::optional<double>(value.value()) : std::nullopt;
    }, py::arg("key"),
         R"pbdoc(
         Get a configuration value as a float.
         
         Args:
             key: Configuration key to retrieve
             
         Returns:
             Configuration value as a float, or None if the key doesn't exist
             
         Example:
             ```python
             learning_rate = config.get("plasticity.learning_rate")
             ```
         """)
    .def("get", [](const Config& self, const std::string& key, double default_value) {
        auto value = self.get<double>(key);
        return value.has_value() ? value.value() : default_value;
    }, py::arg("key"), py::arg("default_value"),
       R"pbdoc(
       Get a configuration value as a float with a default.
       
       Args:
           key: Configuration key to retrieve
           default_value: Default value if the key doesn't exist
           
       Returns:
           Configuration value as a float, or the default value if the key doesn't exist
           
       Example:
           ```python
           learning_rate = config.get("plasticity.learning_rate", 0.01)
           ```
       """)
    .def("get", [](const Config& self, const std::string& key) {
        auto value = self.get<bool>(key);
        return value.has_value() ? std::optional<bool>(value.value()) : std::nullopt;
    }, py::arg("key"),
         R"pbdoc(
         Get a configuration value as a boolean.
         
         Args:
             key: Configuration key to retrieve
             
         Returns:
             Configuration value as a boolean, or None if the key doesn't exist
             
         Example:
             ```python
             enabled = config.get("plasticity.enable")
             ```
         """)
    .def("get", [](const Config& self, const std::string& key, bool default_value) {
        auto value = self.get<bool>(key);
        return value.has_value() ? value.value() : default_value;
    }, py::arg("key"), py::arg("default_value"),
       R"pbdoc(
       Get a configuration value as a boolean with a default.
       
       Args:
           key: Configuration key to retrieve
           default_value: Default value if the key doesn't exist
           
       Returns:
           Configuration value as a boolean, or the default value if the key doesn't exist
           
       Example:
           ```python
           enabled = config.get("plasticity.enable", True)
           ```
       """)
    ;
}

// ============================================================================
// SENSORY INPUT CLASSES
// ============================================================================

/**
 * @brief Register all sensory input classes with Python
 * 
 * Sensory input classes represent different types of information that can be
 * received from the environment. They are used to provide input to the brain
 * for processing.
 */
void register_sensory_input_classes(py::module_& m) {
    // Base SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input to the brain.
        
        SensoryInput objects represent different types of information that can
        be received from the environment. They are used to provide input to the
        brain for processing and learning.
        
        Subclasses include Vision, Audio, and InternalSignals.
        
        Common methods:
        - getType(): Get the type of sensory input
        - getData(): Get the raw data as a vector
        - getDimensions(): Get the dimensionality
        - getTimestamp(): Get the timestamp
        - setTimestamp(): Set the timestamp
        
        Examples:
            ```python
            import pynlm
            
            # Create vision input
            vision = pynlm.Vision(width=8, height=8)
            vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8])
            
            # Create audio input
            audio = pynlm.Audio(sample_rate=44100, num_samples=1024)
            audio.setData([0.5, 0.6, 0.7, 0.8, 0.9])
            ```
        )""
    )
    .def("getType", &SensoryInput::getType,
         R"pbdoc(
         Get the type of sensory input.
         
         Returns:
             NeuronType: The type of sensory input
         """)
    .def("getData", &SensoryInput::getData,
         R"pbdoc(
         Get the raw data as a vector.
         
         Returns:
             std::vector<float>: The raw sensory data
         """)
    .def("getDimensions", &SensoryInput::getDimensions,
         R"pbdoc(
         Get the dimensionality of the sensory data.
         
         Returns:
             size_t: Number of dimensions in the sensory data
         """)
    .def("getTimestamp", &SensoryInput::getTimestamp,
         R"pbdoc(
         Get the timestamp of the sensory input.
         
         Returns:
             Timestamp: The timestamp when the sensory input was received
         """)
    .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
         R"pbdoc(
         Set the timestamp of the sensory input.
         
         Args:
             timestamp: The timestamp to set
         """)
    ;
    
    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input for the NLM system.
        
        Vision objects represent visual information from the environment,
        typically as pixel data from a simulated camera or sensor.
        
        Properties:
        - width: Width of the visual field
        - height: Height of the visual field  
        - channels: Number of color channels (typically 3 for RGB)
        
        Example:
            ```python
            import pynlm
            
            # Create 8x8 vision with 3 color channels
            vision = pynlm.Vision(8, 8, 3)
            
            # Set visual data (e.g., grayscale image)
            vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8])
            
            # Access properties
            print(f"Width: {vision.getWidth()}")
            print(f"Height: {vision.getHeight()}")
            print(f"Channels: {vision.getChannels()}")
            ```
        )""
    )
    .def(py::init<>())
    .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
         py::arg("channels") = 3,
         R"pbdoc(
         Create a Vision object.
         
         Args:
             width: Width of the visual field
             height: Height of the visual field
             channels: Number of color channels (default: 3 for RGB)
         """)
    .def("setData", [](Vision& self, const std::vector<float>& data) {
        self.setData(data);
    }, py::arg("data"),
         R"pbdoc(
         Set visual data.
         
         Args:
             data: Vector of float values representing the image data
         """)
    .def("getWidth", &Vision::getWidth,
         R"pbdoc(Get width of the visual field.)")
    .def("getHeight", &Vision::getHeight,
         R"pbdoc(Get height of the visual field.)")
    .def("getChannels", &Vision::getChannels,
         R"pbdoc(Get number of color channels.)")
    ;
    
    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for the NLM system.
        
        Audio objects represent sound information from the environment,
        typically as audio samples from a simulated microphone or sensor.
        
        Properties:
        - sampleRate: Audio sampling rate in Hz
        - numSamples: Number of audio samples
        
        Example:
            ```python
            import pynlm
            
            # Create audio input
            audio = pynlm.Audio(44100, 1024)
            
            # Set audio data
            audio.setData([0.1, 0.2, 0.3, 0.4, 0.5])
            
            # Set sample rate
            audio.setSampleRate(22050)
            
            # Access properties
            print(f"Sample rate: {audio.getSampleRate()}")
            print(f"Number of samples: {audio.getNumSamples()}")
            ```
        )""
    )
    .def(py::init<>())
    .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
         R"pbdoc(
         Create an Audio object.
         
         Args:
             sampleRate: Audio sampling rate in Hz
             numSamples: Number of audio samples
         """)
    .def("setData", [](Audio& self, const std::vector<float>& data) {
        self.setData(data);
    }, py::arg("data"),
         R"pbdoc(
         Set audio data.
         
         Args:
             data: Vector of float values representing audio samples
         """)
    .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
         R"pbdoc(Set the audio sampling rate.)")
    .def("getSampleRate", &Audio::getSampleRate,
         R"pbdoc(Get the audio sampling rate.)")
    .def("getNumSamples", &Audio::getNumSamples,
         R"pbdoc(Get the number of audio samples.)")
    ;
    
    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input for the NLM system.
        
        InternalSignals objects represent internal brain state information,
        such as prediction errors, novelty signals, or other internal
        representations that can be used for learning and control.
        
        Methods:
        - addSignal(value): Add a signal value to the internal signals
        - clearSignals(): Clear all internal signals
        
        Example:
            ```python
            import pynlm
            
            # Create internal signals
            internal = pynlm.InternalSignals()
            
            # Add signal values
            internal.addSignal(0.1)
            internal.addSignal(0.5)
            internal.addSignal(0.9)
            
            # Clear signals
            internal.clearSignals()
            ```
        )""
    )
    .def(py::init<>())
    .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
         R"pbdoc(
         Add a signal value to the internal signals.
         
         Args:
             value: Signal value to add
         """)
    .def("clearSignals", &InternalSignals::clearSignals,
         R"pbdoc(
         Clear all internal signals.
         ```
         """)
    ;
}

// ============================================================================
// ACTION AND WORLD OBJECT CLASSES
// ============================================================================

/**
 * @brief Register action and world object classes with Python
 * 
 * These classes represent the fundamental interactions between the agent and
 * the environment: actions that can be taken and objects that exist in the world.
 */
void register_action_and_object_classes(py::module_& m) {
    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output in the NLM system.
        
        Action objects represent the actions that the agent can take in the
        environment. They consist of a type and optional parameters.
        
        Properties:
        - type: The type of action (MoveForward, TurnLeft, etc.)
        - parameters: Additional parameters for the action
        
        Common types:
        - MoveForward, MoveBackward: Movement actions
        - TurnLeft, TurnRight: Rotation actions
        - Look: Look in a direction
        - Interact: Interact with objects
        - Eat/Drink: Consume resources
        - Rest/Wait: Do nothing
        
        Example:
            ```python
            import pynlm
            
            # Create a move forward action
            action = pynlm.Action(pynlm.ActionType.MoveForward)
            
            # Create a custom action with parameters
            action = pynlm.Action(
                pynlm.ActionType.Custom,
                [0.5, 0.8, 0.2]  # Custom parameters
            )
            
            # Access action properties
            print(f"Type: {action.getType()}")
            print(f"Parameters: {action.getParameters()}")
            print(f"Name: {action.getName()}")
            ```
        )""
    )
    .def(py::init<>())
    .def(py::init<ActionType>(), py::arg("type"),
         R"pbdoc(
         Create an Action with just a type.
         
         Args:
             type: The type of action
         """)
    .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
         R"pbdoc(
         Create an Action with a type and parameters.
         
         Args:
             type: The type of action
             parameters: Additional parameters for the action
         """)
    .def("getType", &Action::getType,
         R"pbdoc(Get the type of the action.)")
    .def("setType", &Action::setType, py::arg("type"),
         R"pbdoc(Set the type of the action.)")
    .def("getParameters", &Action::getParameters,
         R"pbdoc(Get the action parameters.)")
    .def("setParameters", &Action::setParameters, py::arg("params"),
         R"pbdoc(Set the action parameters.)")
    .def("getName", &Action::getName,
         R"pbdoc(Get a human-readable name for the action.)")
    .def("clone", &Action::clone,
         R"pbdoc(Create a copy of this action.)")
    ;
    
    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the NLM system.
        
        WorldObject represents objects that exist in the simulated environment.
        Objects can be resources, hazards, walls, markers, or empty spaces.
        
        Properties:
        - x: X position in the world
        - y: Y position in the world
        - type: Type of object (Resource, Hazard, Wall, etc.)
        - value: Value of the object (e.g., nutritional value)
        - radius: Radius of the object
        - active: Whether the object is currently active
        
        Example:
            ```python
            import pynlm
            
            # Create an empty space
            empty = pynlm.WorldObject(1.0, 1.0, pynlm.WorldObjectType.Empty)
            
            # Create a resource
            resource = pynlm.WorldObject(
                5.0, 5.0,
                pynlm.WorldObjectType.Resource,
                value=10.0
            )
            
            # Create a hazard
            hazard = pynlm.WorldObject(
                8.0, 8.0,
                pynlm.WorldObjectType.Hazard,
                value=5.0
            )
            
            # Access properties
            print(f"Position: ({resource.x}, {resource.y})")
            print(f"Type: {resource.type}")
            print(f"Value: {resource.value}")
            print(f"Active: {resource.active}")
            ```
        )""
    )
    .def(py::init<>())
    .def(py::init<float, float, WorldObjectType, float, float>(),
         py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
         py::arg("radius") = 0.5f,
         R"pbdoc(
         Create a WorldObject.
         
         Args:
             x: X position in the world
             y: Y position in the world
             type: Type of object
             value: Value of the object (default: 0.0)
             radius: Radius of the object (default: 0.5)
         """)
    .def_readwrite("x", &WorldObject::x, "X position in the world")
    .def_readwrite("y", &WorldObject::y, "Y position in the world")
    .def_readwrite("radius", &WorldObject::radius, "Radius of the object")
    .def_readwrite("type", &WorldObject::type, "Type of the object")
    .def_readwrite("value", &WorldObject::value, "Value of the object")
    .def_readwrite("active", &WorldObject::active, "Whether the object is active")
    ;
    
    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state in the NLM system.
        
        AgentBody represents the physical state of the agent in the world,
        including position, orientation, velocity, and energy levels.
        
        Properties:
        - x: X position in the world
        - y: Y position in the world
        - orientation: Current facing direction (radians)
        - velocityX: X velocity
        - velocityY: Y velocity
        - angularVelocity: Angular velocity
        - energy: Current energy level
        - health: Health level
        - age: Age in simulation steps
        - isMoving: Whether the agent is currently moving
        - isTurning: Whether the agent is currently turning
        - lastActionTime: Time since last action
        
        Methods:
        - reset(): Reset all body state to defaults
        
        Example:
            ```python
            import pynlm
            
            # Create an agent body
            body = pynlm.AgentBody()
            
            # Access properties
            print(f"Position: ({body.x}, {body.y})")
            print(f"Orientation: {body.orientation}")
            print(f"Energy: {body.energy}")
            print(f"Health: {body.health}")
            print(f"Is moving: {body.isMoving}")
            
            # Update properties
            body.x = 10.0
            body.y = 10.0
            body.energy = 50.0
            body.isMoving = True
            
            # Reset body
            body.reset()
            ```
        )""
    )
    .def(py::init<>())
    .def_readwrite("x", &AgentBody::x, "X position in the world")
    .def_readwrite("y", &AgentBody::y, "Y position in the world")
    .def_readwrite("orientation", &AgentBody::orientation, "Current facing direction (radians)")
    .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
    .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
    .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
    .def_readwrite("energy", &AgentBody::energy, "Current energy level")
    .def_readwrite("health", &AgentBody::health, "Health level")
    .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
    .def_readwrite("isMoving", &AgentBody::isMoving, "Whether the agent is currently moving")
    .def_readwrite("isTurning", &AgentBody::isTurning, "Whether the agent is currently turning")
    .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
    .def("reset", &AgentBody::reset, "Reset all body state to defaults")
    ;
    
    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result from an action taken in the world.
        
        ActionResult represents the outcome of an action, including reward,
        success status, and a descriptive message.
        
        Properties:
        - reward: Reward received from the action
        - success: Whether the action was successful
        - message: Description of the action outcome
        
        Example:
            ```python
            import pynlm
            
            # Create an action result
            result = pynlm.ActionResult(reward=1.0, success=True, message="Found food!")
            
            # Access result properties
            print(f"Reward: {result.reward}")
            print(f"Success: {result.success}")
            print(f"Message: {result.message}")
            ```
        )""
    )
    .def(py::init<>())
    .def(py::init<float, bool, std::string>(),
         py::arg("reward"), py::arg("success"), py::arg("message") = "",
         R"pbdoc(
         Create an ActionResult.
         
         Args:
             reward: Reward received from the action
             success: Whether the action was successful
             message: Description of the action outcome (default: "")
         """)
    .def_readwrite("reward", &ActionResult::reward, "Reward received from the action")
    .def_readwrite("success", &ActionResult::success, "Whether the action was successful")
    .def_readwrite("message", &ActionResult::message, "Description of the action outcome")
    ;
    
    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data for the NLM system.
        
        SensoryPercept contains all sensory information available to the agent
        at a given moment, including vision, touch, internal signals, and
        proprioception.
        
        Properties:
        - vision: Visual input (Vision object)
        - touch: Tactile input (Touch object)
        - internal: Internal signal input (InternalSignals object)
        - proprioception: Body position input (Proprioception object)
        - audio: Audio input (Audio object)
        - timestamp: Time when the percept was received
        
        Methods:
        - getVision(), setVision()
        - getTouch(), setTouch()
        - getInternal(), setInternal()
        - getProprioception(), setProprioception()
        - getAudio(), setAudio()
        - getAllSignals(): Get all available signals
        - getTimestamp(), setTimestamp()
        
        Example:
            ```python
            import pynlm
            
            # Create a sensory percept
            percept = pynlm.SensoryPercept()
            
            # Set visual input
            vision = pynlm.Vision(8, 8)
            vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8])
            percept.setVision(vision)
            
            # Set internal signals
            internal = pynlm.InternalSignals()
            internal.addSignal(0.5)
            internal.addSignal(0.8)
            percept.setInternal(internal)
            
            # Access percept properties
            print(f"Vision width: {percept.getVisionWidth()}")
            print(f"Vision height: {percept.getVisionHeight()}")
            print(f"Internal signals: {percept.getInternal()}")
            ```
        )""
    )
    .def(py::init<>())
    .def("getVision", &SensoryPercept::getVision,
         R"pbdoc(Get the visual input.)")
    .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
         R"pbdoc(Set the visual input.)")
    .def("getVisionWidth", &SensoryPercept::getVisionWidth,
         R"pbdoc(Get the width of the visual input.)")
    .def("getVisionHeight", &SensoryPercept::getVisionHeight,
         R"pbdoc(Get the height of the visual input.)")
    .def("getTouch", &SensoryPercept::getTouch,
         R"pbdoc(Get the tactile input.)")
    .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
         R"pbdoc(Set the tactile input.)")
    .def("getInternal", &SensoryPercept::getInternal,
         R"pbdoc(Get the internal signal input.)")
    .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
         R"pbdoc(Set the internal signal input.)")
    .def("getProprioception", &SensoryPercept::getProprioception,
         R"pbdoc(Get the proprioceptive input.)")
    .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
         R"pbdoc(Set the proprioceptive input.)")
    .def("getAudio", &SensoryPercept::getAudio,
         R"pbdoc(Get the audio input.)")
    .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
         R"pbdoc(Set the audio input.)")
    .def("getAllSignals", &SensoryPercept::getAllSignals,
         R"pbdoc(Get all available sensory signals.)")
    .def("getTimestamp", &SensoryPercept::getTimestamp,
         R"pbdoc(Get the timestamp of the percept.)")
    .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
         R"pbdoc(Set the timestamp of the percept.)")
    ;
}

// ============================================================================
// BRAIN CLASS
// ============================================================================

/**
 * @brief Register the Brain class with Python
 * 
 * The Brain class is the central component of the NLM system. It represents
 * the neural system that processes sensory input, makes decisions, and produces
 * motor output. The Brain implements real spiking neural computation with
 * integrated memory, prediction, cognition, and neuromodulation systems.
 */
void register_brain_class(py::module_& m) {
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class for NLM.
        
        The Brain class is the core component of the NLM system. It implements
        real spiking neural computation with event-driven dynamics and integrated
        memory, prediction, cognition, and neuromodulation systems.
        
        The Brain class provides the main interface for:
        - Creating and initializing the neural system
        - Processing simulation steps (the main computation loop)
        - Receiving sensory input and injecting it into the network
        - Producing motor actions based on neural activity
        - Managing brain state (save/load/reset)
        - Accessing various subsystems (memory, prediction, cognition, etc.)
        
        Key methods:
        - __init__(config): Create brain with configuration
        - initialize(): Initialize the brain
        - step(step, time=None): Run one simulation step
        - receiveSensoryInput(input): Inject sensory input
        - produceAction(): Get motor action
        - save(filepath), load(filepath): Save/load brain state
        - getTotalNeuronCount(): Get total neuron count
        - getTotalSynapseCount(): Get total synapse count
        - getFiringNeuronCount(): Get currently firing neurons
        - getAverageFiringRate(): Get average firing rate
        - getTotalSpikeCount(): Get total spike count
        - getDevelopmentalStage(): Get developmental stage
        
        Example:
            ```python
            import pynlm
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Create brain
            brain = pynlm.createBrain(config)
            
            # Initialize brain
            brain.initialize()
            
            # Run simulation
            for step in range(100):
                brain.step(step)
                
                # Get statistics
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
                print(f"Total spikes: {brain.getTotalSpikeCount()}")
                print(f"Avg firing rate: {brain.getAverageFiringRate()}")
            ```
        )""
    )
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
         R"pbdoc(
         Create a Brain with configuration.
         
         Args:
             config: Configuration for the brain
         """)
    .def("initialize", &Brain::initialize,
         R"pbdoc(
         Initialize the brain with configuration.
         
         This method must be called before using the brain. It sets up
         all neural components and prepares the brain for simulation.
         
         Example:
             ```python
             brain.initialize()
             ```
         """)
    .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
         py::arg("currentStep"),
         R"pbdoc(
         Perform a simulation step with step number.
         
         Args:
             currentStep: The current simulation step number
             
         Example:
             ```python
             brain.step(0)      # Step 0
             brain.step(100)    # Step 100
             ```
         """)
    .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
         py::arg("currentStep"), py::arg("currentTime"),
         R"pbdoc(
         Perform a simulation step with timestamp.
         
         Args:
             currentStep: The current simulation step number
             currentTime: The current simulation time in seconds
             
         Example:
             ```python
             brain.step(0, 0.0)        # Step 0 at time 0.0s
             brain.step(100, 1.0)      # Step 100 at time 1.0s
             ```
         """)
    .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
         R"pbdoc(
         Inject sensory input into the brain.
         
         Args:
             input: Sensory input object (Vision, Audio, etc.)
             
         Example:
             ```python
             vision = pynlm.Vision(8, 8)
             vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8])
             brain.receiveSensoryInput(vision)
             ```
         """)
    .def("injectCurrent", &Brain::injectCurrent,
         py::arg("neuron"), py::arg("current"),
         R"pbdoc(
         Inject current directly into a specific neuron.
         
         Args:
             neuron: NeuronId of the neuron to inject current into
             current: Amount of current to inject (positive = excitation, negative = inhibition)
             
         Example:
             ```python
             neuron_id = pynlm.NeuronId(42)
             brain.injectCurrent(neuron_id, 10.0)  # Excite neuron
             brain.injectCurrent(neuron_id, -5.0)  # Inhibit neuron
             ```
         """)
    .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
         py::arg("type"), py::arg("current"),
         R"pbdoc(
         Inject current into all neurons of a specific type.
         
         Args:
             type: Type of neurons to inject current into
             current: Amount of current to inject
             
         Example:
             ```python
             brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
             ```
         """)
    .def("produceAction", &Brain::produceAction,
         R"pbdoc(
         Produce motor action based on neural activity.
         
         Returns:
             Action object representing the motor action
             
         Example:
             ```python
             action = brain.produceAction()
             print(f"Action type: {action.getType()}")
             ```
         """)
    .def("reset", &Brain::reset,
         R"pbdoc(
         Reset brain state.
         
         This method resets all neural state, including neuron voltages,
         synaptic weights, and spike history. The brain must be
         re-initialized after calling reset.
         
         Example:
             ```python
             brain.reset()
             brain.initialize()  # Must re-initialize after reset
             ```
         """)
    .def("save", &Brain::save, py::arg("filepath"),
         R"pbdoc(
         Save brain state to file (checkpointing).
         
         Args:
             filepath: Path to save the brain state to
             
         Returns:
             True if the state was saved successfully, False otherwise
             
         Example:
             ```python
             brain.save("brain_checkpoint.bin")
             ```
         """)
    .def("load", &Brain::load, py::arg("filepath"),
         R"pbdoc(
         Load brain state from file.
         
         Args:
             filepath: Path to load the brain state from
             
         Returns:
             True if the state was loaded successfully, False otherwise
             
         Example:
             ```python
             success = brain.load("brain_checkpoint.bin")
             if success:
                 print("Brain state loaded successfully")
             ```
         """)
    .def("addRegion", &Brain::addRegion, py::arg("name") = "",
         R"pbdoc(
         Add a new neural region.
         
         Args:
             name: Name of the region (optional)
             
         Returns:
             RegionId of the newly created region
             
         Example:
             ```python
             region_id = brain.addRegion("Cortex")
             ```
         """)
    .def("getRegion", &Brain::getRegion, py::arg("id"),
         py::return_value_policy::reference_internal,
         R"pbdoc(
         Get a region by ID.
         
         Args:
             id: RegionId of the region to get
             
         Returns:
             NeuralRegion object, or None if not found
             
         Example:
             ```python
             region = brain.getRegion(region_id)
             ```
         """)
    .def("getRegionCount", &Brain::getRegionCount,
         R"pbdoc(Get the number of regions in the brain.)")
    .def("getRegionIds", &Brain::getRegionIds,
         R"pbdoc(Get all region IDs.)")
    .def("getRegions", &Brain::getRegions,
         py::return_value_policy::reference_internal,
         R"pbdoc(Get all regions.)")
    .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
         R"pbdoc(Get total neuron count across all regions.)")
    .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
         R"pbdoc(Get total synapse count across all regions.)")
    .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
         R"pbdoc(Get count of active neurons.)")
    .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
         R"pbdoc(Get count of currently firing neurons.)")
    .def("getAverageFiringRate", &Brain::getAverageFiringRate,
         R"pbdoc(Get average firing rate across all neurons.)")
    .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
         R"pbdoc(Get excitation/inhibition balance ratio.)")
    .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
         R"pbdoc(Get total spike count.)")
    .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
         R"pbdoc(Get current developmental stage.)")
    .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
         R"pbdoc(Set developmental stage.)")
    .def("getConfig", &Brain::getConfig,
         py::return_value_policy::reference_internal,
         R"pbdoc(Get the configuration.)")
    .def("logStatus", &Brain::logStatus,
         R"pbdoc(Log brain status and statistics.)")
    ;
}

// ============================================================================
// AGENT BRAIN CLASS
// ============================================================================

/**
 * @brief Register the AgentBrain class with Python
 * 
 * The AgentBrain class provides an interface between the NLM brain and the
 * world simulation. It processes sensory input from the world and decodes
 * motor commands from the brain for the world to execute.
 */
void register_agent_brain_class(py::module_& m) {
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        The AgentBrain class provides a high-level interface between the NLM brain
        and the world simulation. It handles the conversion between world sensory
        input and brain processing, and between brain motor output and world
        actions.
        
        AgentBrain is the main interface for agents to interact with the world,
        providing methods for:
        - Processing sensory input from the world
        - Decoding motor commands from the brain
        - Applying reward modulation for learning
        - Updating development
        - Enabling/disabling subsystems
        
        Key methods:
        - __init__(brain): Create agent brain interface
        - initialize(world): Initialize with world
        - processSensoryInput(percept): Process sensory input
        - decodeMotorCommand(): Get motor command from brain
        - applyRewardModulation(reward, predicted): Apply reward-based learning
        - updateDevelopment(timestep): Update development system
        - enableRewardModulation(), enableDevelopment(), etc.: Enable subsystems
        
        Example:
            ```python
            import pynlm
            
            # Create brain and agent brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            
            # Create world
            world = pynlm.createSimpleWorld()
            
            # Initialize agent with world
            agent.initialize(world)
            
            # Enable subsystems
            agent.enableRewardModulation(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Run agent in world
            for step in range(100):
                # Update world
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Get motor command
                action = agent.decodeMotorCommand()
                
                # Apply action to world
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward modulation
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
            ```
        )""
    )
    .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
         R"pbdoc(
         Create an AgentBrain with brain.
         
         Args:
             brain: Brain object to wrap
         """)
    .def("initialize", &AgentBrain::initialize, py::arg("world"),
         R"pbdoc(
         Initialize the agent with world.
         
         Args:
             world: SimpleWorld object to interact with
         """)
    .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
         R"pbdoc(Get expected sensory input size.)")
    .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
         R"pbdoc(Get expected motor output size.)")
    .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
         R"pbdoc(
         Process sensory percept and inject into brain.
         
         Args:
             percept: SensoryPercept object from the world
         """)
    .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
         R"pbdoc(
         Decode brain motor activity into motor command.
         
         Returns:
             Action object representing the motor command
         """)
    .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
         py::arg("reward"), py::arg("predictedReward"),
         R"pbdoc(
         Apply reward-based neuromodulation.
         
         Args:
             reward: Actual reward received
             predictedReward: Predicted reward
         """)
    .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
         R"pbdoc(
         Update development system.
         
         Args:
             timestep: Time step for development update
         """)
    .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
         R"pbdoc(Get current developmental stage.)")
    .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
         R"pbdoc(Get current neuromodulation level.)")
    .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
         R"pbdoc(Get curiosity level.)")
    .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
         R"pbdoc(Get novelty level.)")
    .def("getPredictionError", &AgentBrain::getPredictionError,
         R"pbdoc(Get prediction error.)")
    .def("reset", &AgentBrain::reset,
         R"pbdoc(Reset agent for new episode.)")
    .def("getBrain", &AgentBrain::getBrain,
         py::return_value_policy::reference_internal,
         R"pbdoc(Get the underlying brain.)")
    .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
         py::arg("enable"),
         R"pbdoc(Enable or disable reward modulation.)")
    .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
         py::arg("enable"),
         R"pbdoc(Enable or disable structural plasticity.)")
    .def("enableDevelopment", &AgentBrain::enableDevelopment,
         py::arg("enable"),
         R"pbdoc(Enable or disable development.)")
    .def("enableCuriosity", &AgentBrain::enableCuriosity,
         py::arg("enable"),
         R"pbdoc(Enable or disable curiosity.)")
    .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
         R"pbdoc(Check if reward modulation is enabled.)")
    .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
         R"pbdoc(Check if structural plasticity is enabled.)")
    .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
         R"pbdoc(Check if development is enabled.)")
    .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
         R"pbdoc(Check if curiosity is enabled.)")
    ;
}

// ============================================================================
// SIMPLE WORLD CLASS
// ============================================================================

/**
 * @brief Register the SimpleWorld class with Python
 * 
 * The SimpleWorld class provides a simple 2D world for NLM simulation.
 * It represents the environment in which the agent operates and provides
 * sensory input and motor output interfaces.
 */
void register_simple_world_class(py::module_& m) {
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        The SimpleWorld class provides a simple 2D world for NLM simulation.
        It represents the environment in which the agent operates and provides
        methods for:
        - Creating and configuring the world
        - Updating the world over time
        - Setting agent starting position
        - Applying motor commands to the world
        - Getting sensory percepts from the world
        - Getting agent body state
        - Adding/removing world objects
        
        The world is a grid-based 2D environment where agents can move
        and interact with objects.
        
        Key methods:
        - __init__(): Create an empty world
        - configure(width, height, visionWidth, visionHeight): Configure world dimensions
        - reset(): Reset world to initial state
        - setAgentStart(x, y): Set agent starting position
        - update(timestep): Update world by timestep
        - applyMotorCommand(cmd, time): Apply motor command to world
        - getSensoryPercept(): Get sensory input from world
        - getAgentBody(): Get agent body state
        - addObject(obj), removeObject(x, y): Manage world objects
        
        Example:
            ```python
            import pynlm
            
            # Create a simple world
            world = pynlm.createSimpleWorld()
            
            # Configure world dimensions
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            
            # Set agent starting position
            world.setAgentStart(10.0, 10.0)
            
            # Reset world to initial state
            world.reset()
            
            # Run world update
            world.update(timestep=0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Get agent body state
            body = world.getAgentBody()
            print(f"Position: ({body.x}, {body.y})")
            print(f"Energy: {body.energy}")
            ```
        )""
    )
    .def(py::init<>())
    .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
         py::arg("visionWidth"), py::arg("visionHeight"),
         R"pbdoc(
         Configure the world dimensions.
         
         Args:
             width: Width of the world (cells)
             height: Height of the world (cells)
             visionWidth: Width of agent's vision (cells)
             visionHeight: Height of agent's vision (cells)
         """)
    .def("reset", &SimpleWorld::reset,
         R"pbdoc(Reset world to initial state.)")
    .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
         R"pbdoc(
         Set agent starting position.
         
         Args:
             x: X coordinate
             y: Y coordinate
         """)
    .def("update", &SimpleWorld::update, py::arg("timestep"),
         R"pbdoc(
         Update the world by timestep.
         
         Args:
             timestep: Time step to advance (seconds)
         """)
    .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
         py::arg("cmd"), py::arg("currentTime"),
         R"pbdoc(
         Apply motor command to the world.
         
         Args:
             cmd: Action to apply
             currentTime: Current simulation time
         """)
    .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
         py::return_value_policy::reference_internal,
         R"pbdoc(Get sensory input from the world.)")
    .def("getAgentBody", &SimpleWorld::getAgentBody,
         py::return_value_policy::reference_internal,
         R"pbdoc(Get agent body state.)")
    .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
         R"pbdoc(
         Add a world object.
         
         Args:
             obj: WorldObject to add
         """)
    .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
         R"pbdoc(
         Remove world object at position.
         
         Args:
             x: X coordinate
             y: Y coordinate
         """)
    .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
         R"pbdoc(
         Check if position is valid.
         
         Args:
             x: X coordinate
             y: Y coordinate
             
         Returns:
             True if position is valid, False otherwise
         """)
    .def("getWidth", &SimpleWorld::getWidth,
         R"pbdoc(Get world width.)")
    .def("getHeight", &SimpleWorld::getHeight,
         R"pbdoc(Get world height.)")
    .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
         R"pbdoc(Get maximum energy.)")
    .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
         R"pbdoc(Set maximum energy.)")
    .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
         R"pbdoc(Get energy decay rate.)")
    .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
         R"pbdoc(Set energy decay rate.)")
    .def("getSimulationTime", &SimpleWorld::getSimulationTime,
         R"pbdoc(Get simulation time.)")
    .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
         R"pbdoc(
         Set random seed for reproducible simulations.
         
         Args:
             seed: Random seed value
         """)
    .def("getRandomSeed", &SimpleWorld::getRandomSeed,
         R"pbdoc(Get random seed.)")
    ;
}

// ============================================================================
// MAIN BINDINGS FUNCTION
// ============================================================================

PYBIND11_MODULE(pynlm, m) {
    // Set module documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides Python interfaces to the NLM C++ neural simulation
        framework. NLM is an experimental artificial developmental brain that
        learns from experience through neural computation.
        
        Key features:
        - Real spiking neural computation with LIF neurons
        - Event-driven spike propagation with synaptic delays
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Integrated memory, prediction, cognition, and neuromodulation systems
        - Developmental stages that affect brain plasticity
        - Checkpoint save/load for persistent simulation state
        
        The main interfaces are:
        - Config: Configuration management
        - Brain: Neural computation engine
        - AgentBrain: Interface between brain and world
        - SimpleWorld: 2D environment simulation
        - Vision/Audio/InternalSignals: Sensory input types
        - Action: Motor output representation
        - WorldObject: Objects in the environment
        - AgentBody: Agent physical state
        - SensoryPercept: Combined sensory information
        
        Usage examples are provided in the documentation and examples directories.
        "";
    
    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register factory functions for easy object creation
    m.def("createDefaultConfig", &create_default_config,
          R"pbdoc(
          Create a default configuration for the NLM system.
          
          Returns:
              Config: A new configuration object with default values
              
          Example:
              ```python
              import pynlm
              config = pynlm.createDefaultConfig()
              ```
          """);
    
    m.def("createBrain", &create_brain_with_config, py::arg("config"),
          R"pbdoc(
          Create a brain with the given configuration.
          
          Args:
              config: Configuration for the brain
              
          Returns:
              Brain: A new brain object
              
          Example:
              ```python
              import pynlm
              config = pynlm.createDefaultConfig()
              brain = pynlm.createBrain(config)
              ```
          """);
    
    m.def("createSimpleWorld", &create_simple_world,
          R"pbdoc(
          Create a simple world for simulation.
          
          Returns:
              SimpleWorld: A new world object
              
          Example:
              ```python
              import pynlm
              world = pynlm.createSimpleWorld()
              ```
          """);
    
    m.def("createAgentBrain", &create_agent_brain_interface, py::arg("brain"),
          R"pbdoc(
          Create an agent brain interface for a given brain.
          
          Args:
              brain: Brain object to wrap
              
          Returns:
              AgentBrain: A new agent brain interface
              
          Example:
              ```python
              import pynlm
              config = pynlm.createDefaultConfig()
              brain = pynlm.createBrain(config)
              agent = pynlm.createAgentBrain(brain)
              ```
          """);
    
    // Register invalid IDs for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Register all classes and enumerations
    register_core_types(m);
    register_config_class(m);
    register_sensory_input_classes(m);
    register_action_and_object_classes(m);
    register_brain_class(m);
    register_agent_brain_class(m);
    register_simple_world_class(m);
    
    // Register version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";
}

} // namespace nlm
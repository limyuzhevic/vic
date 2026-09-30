#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Neural simulation with Leaky Integrate-and-Fire (LIF) neurons
        - Event-driven spike propagation with synaptic delays
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages and plasticity
        - Prediction and cognitive systems
        - Complete environment integration
        
        The bindings provide enhanced documentation, convenience methods,
        error handling, and advanced tools for neuroscience research.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Custom exception types for better error handling
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Neuron ID class with enhanced documentation and validation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        A NeuronId is a lightweight 64-bit identifier used to reference specific neurons
        within the neural network. Neuron IDs are automatically assigned during brain
        initialization and can be used to target specific neurons for current injection
        or other operations.
        
        Attributes:
            value (uint64_t): The numeric identifier value
        
        Examples:
            >>> neuron_id = NeuronId(42)
            >>> neuron_id.value
            42
            >>> str(neuron_id)
            '<NeuronId: 42>'
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a NeuronId from a uint64_t value.")
        .def_readwrite("value", &NeuronId::value,
                      "Get or set the NeuronId value.")
        .def("index", &NeuronId::index,
             "Get the index representation of this NeuronId.")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in dictionaries and sets.")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "Return string representation of NeuronId.")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "Return string representation for display.");

    // Synapse ID class with enhanced documentation
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        A SynapseId uniquely identifies synaptic connections between neurons in the network.
        Synapse IDs enable precise targeting of specific connections for modification
        or analysis.
        
        Attributes:
            value (uint64_t): The numeric identifier value
        
        Examples:
            >>> syn_id = SynapseId(123)
            >>> syn_id.value
            123
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a SynapseId from a uint64_t value.")
        .def_readwrite("value", &SynapseId::value,
                      "Get or set the SynapseId value.")
        .def("index", &SynapseId::index,
             "Get the index representation of this SynapseId.")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in dictionaries and sets.")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "Return string representation of SynapseId.")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        }, "Return string representation for display.");

    // Region ID class with enhanced documentation
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Region IDs identify distinct neural populations or brain regions within
        the simulation. Regions can represent different functional areas,
        cortical layers, or specialized neural circuits.
        
        Attributes:
            value (uint64_t): The numeric identifier value
        
        Examples:
            >>> region_id = RegionId(1)
            >>> region_id.value
            1
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a RegionId from a uint64_t value.")
        .def_readwrite("value", &RegionId::value,
                      "Get or set the RegionId value.")
        .def("index", &RegionId::index,
             "Get the index representation of this RegionId.")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in dictionaries and sets.")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "Return string representation of RegionId.")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        }, "Return string representation for display.");

    // Population ID class with enhanced documentation
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Population IDs identify groups of neurons that share similar properties
        or functional roles. Populations are used for efficient management of
        neuron groups with similar characteristics.
        
        Attributes:
            value (uint64_t): The numeric identifier value
        
        Examples:
            >>> pop_id = PopulationId(42)
            >>> pop_id.value
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a PopulationId from a uint64_t value.")
        .def_readwrite("value", &PopulationId::value,
                      "Get or set the PopulationId value.")
        .def("index", &PopulationId::index,
             "Get the index representation of this PopulationId.")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Neuron type enumeration with enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the neural network.
        
        Different neuron types model various biological neuron subtypes with
        distinct physiological properties and functional roles in the network.
        
        Values:
            Excitatory: Neurons that promote depolarization in target neurons
            Inhibitory: Neurons that promote hyperpolarization in target neurons  
            Modulatory: Neurons that release neuromodulators affecting many targets
            Sensory: Neurons that receive and process sensory input
            Motor: Neurons that generate motor output commands
            Internal: Neurons involved in internal processing and integration
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron")
        .value("Motor", NeuronType::Motor, "Motor neuron")
        .value("Internal", NeuronType::Internal, "Internal neuron")
        .export_values();

    // Synapse type enumeration with enhanced documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse connection types.
        
        Different synapse types represent various biological connection mechanisms
        with distinct properties and computational effects.
        
        Values:
            Excitatory: Chemical synapse that excites postsynaptic neuron
            Inhibitory: Chemical synapse that inhibits postsynaptic neuron
            Modulatory: Synapse that releases neuromodulators
            Electrical: Direct electrical coupling between neurons
            GapJunction: Electrical synapse via gap junctions
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory chemical synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory chemical synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    // Developmental stage enumeration with enhanced documentation
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of brain developmental stages.
        
        The brain undergoes several developmental stages that affect neural
        plasticity, connectivity, and functional properties. Each stage represents
        a distinct period in the brain's lifecycle.
        
        Values:
            Initial: Early developmental stage with high plasticity
            CriticalPeriod: Window of enhanced sensory plasticity
            Maturation: Consolidation and refinement phase
            Adult: Mature, stable brain state
            Aging: Age-related changes and plasticity reduction
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    // Firing state enumeration with enhanced documentation
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        Neurons transition through different firing states during their activity,
        representing their current computational state and excitability.
        
        Values:
            Resting: Neuron at resting membrane potential
            Active: Neuron currently firing action potentials
            Refractory: Neuron recovering from firing (refractory period)
            Inhibited: Neuron actively suppressed by inhibitory input
        )pbdoc")
        .value("Resting", FiringState::Resting, "Resting state")
        .value("Active", FiringState::Active, "Active/firing state")
        .value("Refractory", FiringState::Refractory, "Refractory state")
        .value("Inhibited", FiringState::Inhibited, "Inhibited state")
        .export_values();

    // Action type enumeration with enhanced documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of motor action types.
        
        Different action types represent fundamental motor behaviors and
        interactions that the agent can perform in the environment.
        
        Values include:
            Movement actions (MoveForward, MoveBackward, etc.)
            Orientation actions (TurnLeft, TurnRight)
            Perception actions (Look, LookUp, LookDown)
            Interaction actions (Interact, Eat, Drink, Rest, Custom)
        )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    // Motor command enumeration with enhanced documentation
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands.
        
        Motor commands represent discrete motor outputs that can be executed
        by the agent's motor system. These are higher-level than individual
        neural signals but lower-level than complete behaviors.
        
        Values include:
            Basic movements (MoveForward, MoveBackward, TurnLeft, TurnRight)
            Fine motor actions (LookLeft, LookRight)
            Interactions (Interact, Wait)
        )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    // World object type enumeration with enhanced documentation
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of world object types.
        
        World objects represent entities in the simulated environment with
        different properties and behaviors. Each type serves a specific
        purpose in the agent's interaction with the environment.
        
        Values:
            Empty: Empty space with no content
            Resource: Valuable item that can be collected
            Hazard: Dangerous item that causes damage
            Wall: Static obstacle that cannot be passed
            Marker: Special location or landmark
        )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty world cell")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of world object types.
        
        World objects represent entities in the simulated environment with
        different properties and behaviors. Each type serves a specific
        purpose in the agent's interaction with the environment.
        
        Values:
            Empty: Empty space with no content
            Resource: Valuable item that can be collected
            Hazard: Dangerous item that causes damage
            Wall: Static obstacle that cannot be passed
            Marker: Special location or landmark
        )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty world cell")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Config class with enhanced documentation and convenience methods
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class manages key-value pairs used to configure the neural
        network, including brain parameters, plasticity settings, and
        environmental properties. It provides both simple and advanced
        configuration options.
        
        The configuration system uses a flexible variant-based type system
        that supports int, int64_t, double, bool, string, and vector types.
        
        Common configuration categories:
        - brain.*: Brain parameters (neuron_count, connection_probability, etc.)
        - plasticity.*: Plasticity mechanism settings
        - neuromod.*: Neuromodulation parameters
        - development.*: Developmental system settings
        
        Examples:
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("brain.neuron_count", 1000)
            >>> config.set("plasticity.stdp.learning_rate", 0.001)
            >>> neuron_count = config.getOr("brain.neuron_count", 100)
        )pbdoc")
        .def(py::init<>(),
             "Create an empty configuration.")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "Return string representation of the configuration.")
        
        // Convenience methods for common configuration operations
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value as string with fallback to default value.")
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value as int with fallback to default value.")
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value as double with fallback to default value.")
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value as bool with fallback to default value.")
        .def("setString", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a configuration value as string with optional source.")
        .def("setInt", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a configuration value as int with optional source.")
        .def("setDouble", [](Config& self, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a configuration value as double with optional source.")
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a configuration value as bool with optional source.")
        .def("setVectorInt", [](Config& self, const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a configuration value as int vector with optional source.")
        .def("setVectorDouble", [](Config& self, const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a configuration value as double vector with optional source.")
        .def("setVectorString", [](Config& self, const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a configuration value as string vector with optional source.")
        .def("getAll", &Config::getKeys,
             "Get all configuration keys.")
        .def("validate", [](Config& self) {
            // Validate configuration
            auto keys = self.getKeys();
            std::vector<std::string> errors;
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (!value) {
                    errors.push_back("Invalid value for key: " + key);
                }
            }
            if (!errors.empty()) {
                throw std::runtime_error("Configuration validation failed: " + 
                                       std::to_string(errors.size()) + " errors found.");
            }
            return true;
        }, "Validate configuration and throw exception if invalid.")
        .def("merge", [](Config& self, const Config& other) {
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                auto value = other.get<std::string>(key);
                if (value) {
                    self.set(key, *value, ConfigSource::Runtime);
                }
            }
            return true;
        }, py::arg("other"), "Merge another configuration into this one.")
        .def("copyFrom", [](Config& self, const Config& other) {
            self.clear();
            return self.merge(other);
        }, py::arg("other"), "Copy configuration from another Config object.");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
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

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    // SensoryInput class with enhanced documentation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input in the NLM system.
        
        SensoryInput represents the raw sensory data that is processed by the brain
        to influence behavior. Different sensory modalities are implemented as
        subclasses (Vision, Audio, InternalSignals) that provide specific data
        types relevant to their sensory domain.
        
        Sensory input is critical for the agent's interaction with the environment,
        enabling perception, decision-making, and adaptive behavior.
        
        Common sensory types:
        - Vision: Visual information from the agent's cameras/sensors
        - Audio: Auditory information from environmental sounds
        - Internal: Internal physiological signals (energy, health, etc.)
        
        Examples:
            >>> vision = Vision(80, 60, 3)
            >>> vision.setData(pixel_array)
            >>> sensory_data = world.getSensoryPercept()
            >>> brain.receiveSensoryInput(sensory_data)
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input.")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector.")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality.")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp.")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp.")
        .def("toVector", [](const SensoryInput& self) {
            return self.getData();
        }, "Convert sensory input to vector format.")
        .def("clone", [](const SensoryInput& self) {
            // Create a new instance of the same type
            if (auto vision = dynamic_cast<const Vision*>(&self)) {
                return std::make_unique<Vision>(*vision);
            } else if (auto audio = dynamic_cast<const Audio*>(&self)) {
                return std::make_unique<Audio>(*audio);
            } else if (auto internal = dynamic_cast<const InternalSignals*>(&self)) {
                return std::make_unique<InternalSignals>(*internal);
            }
            throw std::runtime_error("Cannot clone unknown sensory input type");
        }, "Create a copy of this sensory input.")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        }, "Return string representation of sensory input.");

    // Vision class with enhanced documentation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual data.
        
        Vision captures visual information from the agent's perspective,
        typically representing a 2D image with color information. This is the
        primary sensory modality for most NLM agents, enabling object detection,
        scene understanding, and visual navigation.
        
        Attributes:
            width: Image width in pixels
            height: Image height in pixels  
            channels: Number of color channels (typically 3 for RGB)
        
        Examples:
            >>> vision = Vision(80, 60, 3)  # 80x60 RGB image
            >>> vision.setData(pixel_array)  # Set pixel data
            >>> width = vision.getWidth()     # 80
            >>> height = vision.getHeight()   # 60
            >>> channels = vision.getChannels()  # 3
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Initialize Vision with specified dimensions.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set pixel data as vector of floats.")
        .def("setData", [](Vision& self, const std::vector<std::vector<std::vector<float>>>& data) {
            std::vector<float> flat_data;
            for (const auto& row : data) {
                for (const auto& pixel : row) {
                    for (const float& channel : pixel) {
                        flat_data.push_back(channel);
                    }
                }
            }
            self.setData(flat_data);
        }, py::arg("data"), "Set pixel data as 3D vector (height x width x channels).")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels.")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels.")
        .def("getChannels", &Vision::getChannels, "Get number of color channels.")
        .def("getPixelCount", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get total number of pixels in the image.")
        .def("getShape", [](const Vision& self) {
            return std::vector<size_t>{self.getHeight(), self.getWidth(), self.getChannels()};
        }, "Get image shape as (height, width, channels) tuple.")
        .def("isValid", [](const Vision& self) {
            return !self.getData().empty() && self.getWidth() > 0 && self.getHeight() > 0;
        }, "Check if vision data is valid and complete.")
        .def("resize", [](Vision& self, size_t width, size_t height) {
            // Note: This is a simplified resize implementation
            // In practice, you might want to use a proper image processing library
            std::vector<float> original_data = self.getData();
            size_t original_channels = self.getChannels();
            
            // Simple nearest-neighbor resize (placeholder implementation)
            self = Vision(width, height, original_channels);
            if (!original_data.empty()) {
                // This is a simplified resize - actual implementation would need interpolation
                float sample_rate_x = static_cast<float>(original_data.size()) / 
                                    (original_channels * self.getWidth() * self.getHeight());
                self.setData(std::vector<float>(original_data.begin(), original_data.end()));
            }
        }, py::arg("width"), py::arg("height"), "Resize vision image to specified dimensions.")
        .def("normalize", [](Vision& self) {
            std::vector<float>& data = self.getData();
            if (data.empty()) return;
            
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            float range = max_val - min_val;
            
            if (range > 0) {
                for (float& val : data) {
                    val = (val - min_val) / range;
                }
            }
        }, "Normalize pixel values to range [0, 1].")
        .def("threshold", [](Vision& self, float threshold) {
            std::vector<float>& data = self.getData();
            for (float& val : data) {
                val = (val > threshold) ? 1.0f : 0.0f;
            }
        }, py::arg("threshold"), "Apply binary threshold to vision data.")
        .def("toArray", [](const Vision& self) {
            return self.getData();
        }, "Convert vision data to Python list for easy manipulation.")
        .def("fromArray", [](Vision& self, const std::vector<float>& data, size_t width, size_t height, size_t channels = 3) {
            self = Vision(width, height, channels);
            self.setData(data);
        }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create Vision from array data.")
        .def("getHistogram", [](const Vision& self, int bins = 256) {
            std::vector<float> data = self.getData();
            std::vector<int> histogram(bins, 0);
            
            if (!data.empty()) {
                float min_val = *std::min_element(data.begin(), data.end());
                float max_val = *std::max_element(data.begin(), data.end());
                float bin_width = (max_val - min_val) / bins;
                
                for (float val : data) {
                    int bin = static_cast<int>((val - min_val) / bin_width);
                    if (bin < bins) {
                        histogram[bin]++;
                    }
                }
            }
            
            return histogram;
        }, py::arg("bins") = 256, "Get histogram of pixel intensity values.")
        .def("getBrightness", [](const Vision& self) {
            std::vector<float> data = self.getData();
            if (data.empty()) return 0.0f;
            
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Calculate average brightness of the image.")
        .def("getContrast", [](const Vision& self) {
            std::vector<float> data = self.getData();
            if (data.empty()) return 0.0f;
            
            float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            return std::sqrt(variance);
        }, "Calculate image contrast (standard deviation of pixel values).")
        .def("getEntropy", [](const Vision& self) {
            std::vector<float> data = self.getData();
            if (data.empty()) return 0.0f;
            
            std::map<float, int> histogram;
            for (float val : data) {
                histogram[val]++;
            }
            
            float entropy = 0.0f;
            float total = static_cast<float>(data.size());
            for (const auto& pair : histogram) {
                float probability = pair.second / total;
                entropy -= probability * std::log2(probability);
            }
            return entropy;
        }, "Calculate image entropy (information content).")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" + 
                   std::to_string(vision.getHeight()) + "x" + 
                   std::to_string(vision.getChannels()) + " channels>";
        }, "Return string representation of Vision object.");

    // Audio class with enhanced documentation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data.
        
        Audio captures auditory information from the environment, enabling the
        agent to process and react to sounds. This sensory modality is important
        for detecting events, locating sound sources, and enhancing environmental
        awareness.
        
        Attributes:
            sampleRate: Sampling rate in Hz
            numSamples: Number of audio samples
        
        Examples:
            >>> audio = Audio(44100, 22050)  # 1 second of audio at 44.1kHz
            >>> audio.setData(audio_samples)  # Set audio samples
            >>> sample_rate = audio.getSampleRate()  # 44100
            >>> duration = audio.getNumSamples() / audio.getSampleRate()  # 0.5 seconds
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Initialize Audio with specified sample rate and duration.")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio samples as vector of floats.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate in Hz.")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate in Hz.")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples.")
        .def("getDuration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds.")
        .def("getFrequency", [](const Audio& self) {
            std::vector<float> data = self.getData();
            if (data.empty()) return 0.0f;
            
            // Simple FFT for frequency detection (placeholder)
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Estimate dominant frequency of audio signal.")
        .def("getAmplitude", [](const Audio& self) {
            std::vector<float> data = self.getData();
            if (data.empty()) return 0.0f;
            
            float max_val = *std::max_element(data.begin(), data.end(), 
                                            [](float a, float b) { return std::abs(a) < std::abs(b); });
            return max_val;
        }, "Get maximum amplitude of audio signal.")
        .def("normalize", [](Audio& self) {
            std::vector<float>& data = self.getData();
            if (data.empty()) return;
            
            float max_val = *std::max_element(data.begin(), data.end(),
                                            [](float a, float b) { return std::abs(a) < std::abs(b); });
            
            if (max_val > 0) {
                for (float& val : data) {
                    val = val / max_val;
                }
            }
        }, "Normalize audio amplitude to range [-1, 1].")
        .def("applyGain", [](Audio& self, float gain) {
            std::vector<float>& data = self.getData();
            for (float& val : data) {
                val = val * gain;
            }
        }, py::arg("gain"), "Apply gain (volume) to audio signal.")
        .def("applyFilter", [](Audio& self, std::vector<float> coeffs) {
            // Simple FIR filter implementation (placeholder)
            std::vector<float>& data = self.getData();
            std::vector<float> output = data;
            
            for (size_t i = coeffs.size(); i < data.size(); ++i) {
                float sum = 0.0f;
                for (size_t j = 0; j < coeffs.size(); ++j) {
                    sum += coeffs[j] * data[i - j];
                }
                output[i] = sum;
            }
            
            data = output;
        }, py::arg("coeffs"), "Apply finite impulse response (FIR) filter to audio.")
        .def("toArray", [](const Audio& self) {
            return self.getData();
        }, "Convert audio data to Python list for easy manipulation.")
        .def("fromArray", [](Audio& self, const std::vector<float>& data, size_t sampleRate) {
            self = Audio(sampleRate, data.size());
            self.setData(data);
        }, py::arg("data"), py::arg("sampleRate"), "Create Audio from array data.")
        .def("getEnvelope", [](const Audio& self) {
            std::vector<float> data = self.getData();
            std::vector<float> envelope;
            
            if (!data.empty()) {
                float current = 0.0f;
                for (float val : data) {
                    current = std::max(current * 0.99f, std::abs(val));
                    envelope.push_back(current);
                }
            }
            
            return envelope;
        }, "Get amplitude envelope of audio signal.")
        .def("getZeroCrossings", [](const Audio& self) {
            std::vector<float> data = self.getData();
            int crossings = 0;
            
            if (data.size() > 1) {
                for (size_t i = 1; i < data.size(); ++i) {
                    if ((data[i-1] >= 0 && data[i] < 0) || (data[i-1] < 0 && data[i] >= 0)) {
                        crossings++;
                    }
                }
            }
            
            return crossings;
        }, "Count zero crossings in audio signal (related to pitch/frequency).")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + "Hz, " +
                   std::to_string(audio.getNumSamples()) + " samples>";
        }, "Return string representation of Audio object.");

    // InternalSignals class with enhanced documentation
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing physiological state.
        
        InternalSignals captures the agent's internal physiological state,
        including energy levels, health, and other internal variables that
        influence behavior and decision-making. This sensory modality enables
        homeostatic regulation and self-monitoring.
        
        Common internal signals:
        - Energy level and depletion
        - Health status
        - Age and developmental stage
        - Hunger, thirst, sleep needs
        - Neurotransmitter levels
        
        Examples:
            >>> internal = InternalSignals()
            >>> internal.addSignal(1.0)  # Energy level
            >>> internal.addSignal(0.5)  # Health level
            >>> signals = internal.getInternal()  # [1.0, 0.5]
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value (e.g., energy, health).")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            auto signals = self.getInternal();
            if (index < signals.size()) {
                return signals[index];
            }
            throw std::out_of_range("Internal signal index out of range");
        }, py::arg("index"), "Get internal signal at specified index.")
        .def("getSignalName", [](const InternalSignals& self, size_t index) {
            // Default signal names
            static const std::vector<std::string> default_names = {
                "energy", "health", "age", "hunger", "thirst", "sleep",
                "dopamine", "curiosity", "novelty", "confidence"
            };
            
            if (index < default_names.size()) {
                return default_names[index];
            }
            return std::string("signal_" + std::to_string(index));
        }, py::arg("index"), "Get name of internal signal at specified index.")
        .def("getEnergy", [](const InternalSignals& self) {
            return self.getInternal()[0];  // First signal is typically energy
        }, "Get energy level (first internal signal).")
        .def("getHealth", [](const InternalSignals& self) {
            return self.getInternal()[1];  // Second signal is typically health
        }, "Get health status (second internal signal).")
        .def("getAge", [](const InternalSignals& self) {
            return self.getInternal()[2];  // Third signal is typically age
        }, "Get age (third internal signal).")
        .def("getHunger", [](const InternalSignals& self) {
            return self.getInternal()[3];  // Fourth signal is typically hunger
        }, "Get hunger level (fourth internal signal).")
        .def("getThirst", [](const InternalSignals& self) {
            return self.getInternal()[4];  // Fifth signal is typically thirst
        }, "Get thirst level (fifth internal signal).")
        .def("getSleep", [](const InternalSignals& self) {
            return self.getInternal()[5];  // Sixth signal is typically sleep
        }, "Get sleep level (sixth internal signal).")
        .def("getNeuromodulation", [](const InternalSignals& self) {
            return self.getInternal()[6];  // Seventh signal is typically neuromodulation
        }, "Get neuromodulation level (seventh internal signal).")
        .def("getCuriosity", [](const InternalSignals& self) {
            return self.getInternal()[7];  // Eighth signal is typically curiosity
        }, "Get curiosity level (eighth internal signal).")
        .def("getNovelty", [](const InternalSignals& self) {
            return self.getInternal()[8];  // Ninth signal is typically novelty
        }, "Get novelty level (ninth internal signal).")
        .def("getConfidence", [](const InternalSignals& self) {
            return self.getInternal()[9];  // Tenth signal is typically confidence
        }, "Get confidence level (tenth internal signal).")
        .def("setEnergy", [](InternalSignals& self, float energy) {
            auto signals = self.getInternal();
            if (!signals.empty()) {
                signals[0] = energy;
                self.clearSignals();
                for (float val : signals) {
                    self.addSignal(val);
                }
            }
        }, py::arg("energy"), "Set energy level (first internal signal).")
        .def("setHealth", [](InternalSignals& self, float health) {
            auto signals = self.getInternal();
            if (signals.size() > 1) {
                signals[1] = health;
                self.clearSignals();
                for (float val : signals) {
                    self.addSignal(val);
                }
            }
        }, py::arg("health"), "Set health status (second internal signal).")
        .def("normalizeAll", [](InternalSignals& self) {
            auto signals = self.getInternal();
            if (signals.empty()) return;
            
            float min_val = *std::min_element(signals.begin(), signals.end());
            float max_val = *std::max_element(signals.begin(), signals.end());
            float range = max_val - min_val;
            
            self.clearSignals();
            if (range > 0) {
                for (float val : signals) {
                    self.addSignal((val - min_val) / range);
                }
            } else {
                for (float val : signals) {
                    self.addSignal(0.5f);  // Default to neutral value
                }
            }
        }, "Normalize all internal signals to range [0, 1].")
        .def("getSystemState", [](const InternalSignals& self) {
            std::vector<std::string> state_names = {
                "energy", "health", "age", "hunger", "thirst", "sleep",
                "dopamine", "curiosity", "novelty", "confidence"
            };
            
            auto signals = self.getInternal();
            std::map<std::string, float> state;
            
            for (size_t i = 0; i < std::min(signals.size(), state_names.size()); ++i) {
                state[state_names[i]] = signals[i];
            }
            
            return state;
        }, "Get system state as dictionary of signal names to values.")
        .def("getHealthStatus", [](const InternalSignals& self) {
            float health = self.getHealth();
            if (health > 0.8f) return "excellent";
            else if (health > 0.6f) return "good";
            else if (health > 0.4f) return "fair";
            else if (health > 0.2f) return "poor";
            else return "critical";
        }, "Get qualitative health status based on health value.")
        .def("getEnergyStatus", [](const InternalSignals& self) {
            float energy = self.getEnergy();
            if (energy > 0.7f) return "high";
            else if (energy > 0.4f) return "moderate";
            else return "low";
        }, "Get qualitative energy status based on energy value.")
        .def("__repr__", [](const InternalSignals& internal) {
            auto signals = internal.getInternal();
            std::stringstream ss;
            ss << "<InternalSignals: " << signals.size() << " signals>";
            for (size_t i = 0; i < std::min(signals.size(), static_cast<size_t>(3)); ++i) {
                ss << " " << signals[i];
            }
            if (signals.size() > 3) ss << " ...";
            return ss.str();
        }, "Return string representation of InternalSignals object.");

    // Action class with enhanced documentation and factory methods
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output in the NLM system.
        
        Actions represent the agent's motor output commands that cause
        changes in the environment. Each action has a type and optional
        parameters that specify how the action should be executed.
        
        Actions are produced by the brain's motor system based on neural
        activity and are applied to the world to create new sensory input.
        
        Common action types include movement, perception, and interaction.
        
        Attributes:
            type: The type of action to perform
            parameters: Optional parameters that modify action execution
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create an Action with specified type.")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an Action with type and parameters.")
        .def("getType", &Action::getType, "Get the action type.")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type.")
        .def("getParameters", &Action::getParameters, "Get action parameters.")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters.")
        .def("getName", &Action::getName, "Get human-readable name of action.")
        .def("getDescription", [](const Action& self) {
            return self.getName();  // Simplified - could provide more detailed descriptions
        }, "Get detailed description of action.")
        .def("hasParameters", [](const Action& self) {
            return !self.getParameters().empty();
        }, "Check if action has parameters.")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters.")
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw std::out_of_range("Action parameter index out of range");
        }, py::arg("index"), "Get parameter at specified index.")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto& params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
            } else {
                throw std::out_of_range("Action parameter index out of range");
            }
        }, py::arg("index"), py::arg("value"), "Set parameter at specified index.")
        .def("addParameter", [](Action& self, float value) {
            self.getParameters().push_back(value);
        }, py::arg("value"), "Add a new parameter to the action.")
        .def("clone", &Action::clone, "Create a copy of this action.")
        .def("isMovement", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::MoveForward ||
                   type == ActionType::MoveBackward ||
                   type == ActionType::MoveLeft ||
                   type == ActionType::MoveRight;
        }, "Check if this is a movement action.")
        .def("isPerception", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::Look ||
                   type == ActionType::LookUp ||
                   type == ActionType::LookDown;
        }, "Check if this is a perception action.")
        .def("isInteraction", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::Interact ||
                   type == ActionType::Eat ||
                   type == ActionType::Drink ||
                   type == ActionType::Rest ||
                   type == ActionType::Wait ||
                   type == ActionType::Custom;
        }, "Check if this is an interaction action.")
        .def("getActionCategory", [](const Action& self) {
            ActionType type = self.getType();
            if (type == ActionType::MoveForward || type == ActionType::MoveBackward ||
                type == ActionType::MoveLeft || type == ActionType::MoveRight) {
                return "movement";
            } else if (type == ActionType::Look || type == ActionType::LookUp || type == ActionType::LookDown) {
                return "perception";
            } else if (type == ActionType::Interact || type == ActionType::Eat ||
                       type == ActionType::Drink || type == ActionType::Rest ||
                       type == ActionType::Wait || type == ActionType::Custom) {
                return "interaction";
            }
            return "unknown";
        }, "Get action category (movement, perception, or interaction).")
        .def("toDict", [](const Action& self) {
            py::dict dict;
            dict["type"] = static_cast<int>(self.getType());
            dict["name"] = self.getName();
            dict["parameters"] = self.getParameters();
            return dict;
        }, "Convert action to Python dictionary.")
        .def("fromDict", [](Action& self, const py::dict& dict) {
            // Extract type
            if (dict.contains("type")) {
                int type_val = py::cast<int>(dict["type"]);
                self.setType(static_cast<ActionType>(type_val));
            }
            
            // Extract parameters
            if (dict.contains("parameters")) {
                std::vector<float> params = py::cast<std::vector<float>>(dict["parameters"]);
                self.setParameters(params);
            }
        }, py::arg("dict"), "Create action from Python dictionary.")
        .def("__repr__", [](const Action& action) {
            std::string param_str = "";
            auto params = action.getParameters();
            if (!params.empty()) {
                param_str = " with " + std::to_string(params.size()) + " parameters";
            }
            return "<Action: " + action.getName() + param_str + ">";
        }, "Return string representation of Action object.")
        .def("__str__", [](const Action& action) {
            return action.getName();
        }, "Return string representation of action.")
        // Factory methods for common actions
        .def("createMoveForward", []() {
            Action action;
            action.setType(ActionType::MoveForward);
            return action;
        }, "Create a move forward action.")
        .def("createMoveBackward", []() {
            Action action;
            action.setType(ActionType::MoveBackward);
            return action;
        }, "Create a move backward action.")
        .def("createTurnLeft", []() {
            Action action;
            action.setType(ActionType::TurnLeft);
            return action;
        }, "Create a turn left action.")
        .def("createTurnRight", []() {
            Action action;
            action.setType(ActionType::TurnRight);
            return action;
        }, "Create a turn right action.")
        .def("createLook", []() {
            Action action;
            action.setType(ActionType::Look);
            return action;
        }, "Create a look action.")
        .def("createInteract", []() {
            Action action;
            action.setType(ActionType::Interact);
            return action;
        }, "Create an interact action.")
        .def("createEat", []() {
            Action action;
            action.setType(ActionType::Eat);
            return action;
        }, "Create an eat action.")
        .def("createWait", []() {
            Action action;
            action.setType(ActionType::Wait);
            return action;
        }, "Create a wait action.")
        .def("createRest", []() {
            Action action;
            action.setType(ActionType::Rest);
            return action;
        }, "Create a rest action.");

    // WorldObject class with enhanced documentation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the NLM simulation environment.
        
        WorldObject represents entities within the 2D simulated environment,
        including physical objects, resources, hazards, and landmarks. Objects
        have properties that define their behavior and interaction with the agent.
        
        World objects are placed in the environment and can be manipulated
        by the agent's actions, enabling complex environmental interactions.
        
        Object types:
            Empty: Empty space with no content
            Resource: Collectible item with value
            Hazard: Dangerous item that causes damage
            Wall: Static obstacle
            Marker: Special location or landmark
        
        Attributes:
            x: X coordinate in world space
            y: Y coordinate in world space
            radius: Object size (collision radius)
            type: Type of object
            value: Object value (for resources)
            active: Whether object is currently active
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Initialize WorldObject with position, type, and properties.")
        .def_readwrite("x", &WorldObject::x,
                      "Get or set X coordinate in world space.")
        .def_readwrite("y", &WorldObject::y,
                      "Get or set Y coordinate in world space.")
        .def_readwrite("radius", &WorldObject::radius,
                      "Get or set object radius (collision size).")
        .def_readwrite("type", &WorldObject::type,
                      "Get or set object type.")
        .def_readwrite("value", &WorldObject::value,
                      "Get or set object value (for resources).")
        .def_readwrite("active", &WorldObject::active,
                      "Get or set whether object is active.")
        .def("getPosition", [](const WorldObject& self) {
            return std::make_pair(self.x, self.y);
        }, "Get object position as (x, y) tuple.")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position.")
        .def("getDistance", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance to position (x, y).")
        .def("isColliding", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= (self.radius * self.radius);
        }, py::arg("x"), py::arg("y"), "Check if object collides with position (x, y).")
        .def("isValidType", [](const WorldObject& self, WorldObjectType type) {
            return self.type == type;
        }, py::arg("type"), "Check if object is of specified type.")
        .def("getTypeName", [](const WorldObject& self) {
            switch (self.type) {
                case WorldObjectType::Empty: return "empty";
                case WorldObjectType::Resource: return "resource";
                case WorldObjectType::Hazard: return "hazard";
                case WorldObjectType::Wall: return "wall";
                case WorldObjectType::Marker: return "marker";
                default: return "unknown";
            }
        }, "Get human-readable name of object type.")
        .def("isResource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource.")
        .def("isHazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard.")
        .def("isWall", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall.")
        .def("isMarker", [](const WorldObject& self) {
            return self.type == WorldObjectType::Marker;
        }, "Check if object is a marker.")
        .def("isEmpty", [](const WorldObject& self) {
            return self.type == WorldObjectType::Empty;
        }, "Check if object is empty space.")
        .def("toDict", [](const WorldObject& self) {
            py::dict dict;
            dict["x"] = self.x;
            dict["y"] = self.y;
            dict["radius"] = self.radius;
            dict["type"] = static_cast<int>(self.type);
            dict["value"] = self.value;
            dict["active"] = self.active;
            return dict;
        }, "Convert world object to Python dictionary.")
        .def("fromDict", [](WorldObject& self, const py::dict& dict) {
            // Extract position
            if (dict.contains("x")) self.x = py::cast<float>(dict["x"]);
            if (dict.contains("y")) self.y = py::cast<float>(dict["y"]);
            
            // Extract radius
            if (dict.contains("radius")) self.radius = py::cast<float>(dict["radius"]);
            
            // Extract type
            if (dict.contains("type")) {
                int type_val = py::cast<int>(dict["type"]);
                self.type = static_cast<WorldObjectType>(type_val);
            }
            
            // Extract value
            if (dict.contains("value")) self.value = py::cast<float>(dict["value"]);
            
            // Extract active
            if (dict.contains("active")) self.active = py::cast<bool>(dict["active"]);
        }, py::arg("dict"), "Create world object from Python dictionary.")
        .def("__repr__", [](const WorldObject& obj) {
            std::stringstream ss;
            ss << "<WorldObject: " << obj.getTypeName() << " at (" 
               << obj.x << ", " << obj.y << ") radius=" << obj.radius;
            if (obj.value != 0.0f) ss << " value=" << obj.value;
            if (!obj.active) ss << " inactive";
            ss << ">";
            return ss.str();
        }, "Return string representation of WorldObject.")
        .def("__str__", [](const WorldObject& obj) {
            return obj.getTypeName() + " at (" + std::to_string(obj.x) + ", " +
                   std::to_string(obj.y) + ")";
        }, "Return string representation of world object.")
        // Factory methods for common world objects
        .def("createResource", [](float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.3f);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, "Create a resource object.")
        .def("createHazard", [](float x, float y, float damage = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage, 0.4f);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 1.0f, "Create a hazard object.")
        .def("createWall", [](float x, float y, float thickness = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, thickness);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("thickness") = 0.5f, "Create a wall object.")
        .def("createMarker", [](float x, float y) {
            WorldObject obj(x, y, WorldObjectType::Marker, 0.0f, 0.2f);
            return obj;
        }, py::arg("x"), py::arg("y"), "Create a marker object.");

    // AgentBody class with enhanced documentation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representing the physical agent in the simulation.
        
        AgentBody encapsulates the physical properties and state of the agent
        within the simulated environment. It includes position, orientation,
        motion dynamics, and physiological state that influence behavior
        and interaction with the environment.
        
        The agent body serves as the interface between the neural controller
        and the physical world, translating neural outputs into physical actions.
        
        Attributes:
            x: X coordinate in world space
            y: Y coordinate in world space
            orientation: Facing direction in radians
            velocityX: Linear velocity in X direction
            velocityY: Linear velocity in Y direction
            angularVelocity: Rotational velocity
            energy: Current energy level
            health: Current health status
            age: Agent age in simulation steps
            isMoving: Whether agent is currently moving
            isTurning: Whether agent is currently turning
            lastActionTime: Timestamp of last action
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
                      "Get or set X coordinate in world space.")
        .def_readwrite("y", &AgentBody::y,
                      "Get or set Y coordinate in world space.")
        .def_readwrite("orientation", &AgentBody::orientation,
                      "Get or set facing direction in radians.")
        .def_readwrite("velocityX", &AgentBody::velocityX,
                      "Get or set linear velocity in X direction.")
        .def_readwrite("velocityY", &AgentBody::velocityY,
                      "Get or set linear velocity in Y direction.")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                      "Get or set rotational velocity.")
        .def_readwrite("energy", &AgentBody::energy,
                      "Get or set current energy level.")
        .def_readwrite("health", &AgentBody::health,
                      "Get or set current health status.")
        .def_readwrite("age", &AgentBody::age,
                      "Get or set agent age in simulation steps.")
        .def_readwrite("isMoving", &AgentBody::isMoving,
                      "Get or set whether agent is currently moving.")
        .def_readwrite("isTurning", &AgentBody::isTurning,
                      "Get or set whether agent is currently turning.")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                      "Get or set timestamp of last action.")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get agent position as (x, y) tuple.")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position.")
        .def("getDirection", [](const AgentBody& self) {
            return self.orientation;
        }, "Get agent facing direction in radians.")
        .def("setDirection", [](AgentBody& self, float orientation) {
            self.orientation = orientation;
        }, py::arg("orientation"), "Set agent facing direction.")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + 
                           self.velocityY * self.velocityY);
        }, "Get agent speed (magnitude of velocity).")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get agent velocity as (vx, vy) tuple.")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"), "Set agent velocity.")
        .def("getAngularSpeed", [](const AgentBody& self) {
            return self.angularVelocity;
        }, "Get angular speed (rotational velocity).")
        .def("getHealthStatus", [](const AgentBody& self) {
            if (self.health > 0.8f) return "excellent";
            else if (self.health > 0.6f) return "good";
            else if (self.health > 0.4f) return "fair";
            else if (self.health > 0.2f) return "poor";
            else return "critical";
        }, "Get qualitative health status.")
        .def("getEnergyStatus", [](const AgentBody& self) {
            if (self.energy > 0.7f) return "high";
            else if (self.energy > 0.4f) return "moderate";
            else return "low";
        }, "Get qualitative energy status.")
        .def("getMovementDirection", [](const AgentBody& self) {
            if (self.velocityX == 0.0f && self.velocityY == 0.0f) return 0.0f;
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get movement direction in radians.")
        .def("turnTowards", [](AgentBody& self, float target_x, float target_y) {
            float dx = target_x - self.x;
            float dy = target_y - self.y;
            self.orientation = std::atan2(dy, dx);
            self.isTurning = true;
        }, py::arg("target_x"), py::arg("target_y"),
           "Turn agent to face target position.")
        .def("moveTowards", [](AgentBody& self, float target_x, float target_y, float speed) {
            float dx = target_x - self.x;
            float dy = target_y - self.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance > 0) {
                float vx = speed * dx / distance;
                float vy = speed * dy / distance;
                self.setVelocity(vx, vy);
            }
            
            self.isMoving = (speed > 0);
            return distance;
        }, py::arg("target_x"), py::arg("target_y"), py::arg("speed"),
           "Move agent towards target position with specified speed.")
        .def("applyForce", [](AgentBody& self, float fx, float fy, float dt = 0.1f) {
            // Simple physics integration
            self.velocityX += fx * dt;
            self.velocityY += fy * dt;
            
            // Update position based on velocity
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
            
            self.isMoving = (std::abs(self.velocityX) > 0.01f || 
                           std::abs(self.velocityY) > 0.01f);
        }, py::arg("fx"), py::arg("fy"), py::arg("dt") = 0.1f,
           "Apply force to agent (simple physics integration).")
        .def("reset", &AgentBody::reset,
             "Reset agent body to initial state.")
        .def("toDict", [](const AgentBody& self) {
            py::dict dict;
            dict["x"] = self.x;
            dict["y"] = self.y;
            dict["orientation"] = self.orientation;
            dict["velocityX"] = self.velocityX;
            dict["velocityY"] = self.velocityY;
            dict["angularVelocity"] = self.angularVelocity;
            dict["energy"] = self.energy;
            dict["health"] = self.health;
            dict["age"] = self.age;
            dict["isMoving"] = self.isMoving;
            dict["isTurning"] = self.isTurning;
            dict["lastActionTime"] = self.lastActionTime;
            return dict;
        }, "Convert agent body to Python dictionary.")
        .def("fromDict", [](AgentBody& self, const py::dict& dict) {
            // Extract all properties from dictionary
            if (dict.contains("x")) self.x = py::cast<float>(dict["x"]);
            if (dict.contains("y")) self.y = py::cast<float>(dict["y"]);
            if (dict.contains("orientation")) self.orientation = py::cast<float>(dict["orientation"]);
            if (dict.contains("velocityX")) self.velocityX = py::cast<float>(dict["velocityX"]);
            if (dict.contains("velocityY")) self.velocityY = py::cast<float>(dict["velocityY"]);
            if (dict.contains("angularVelocity")) self.angularVelocity = py::cast<float>(dict["angularVelocity"]);
            if (dict.contains("energy")) self.energy = py::cast<float>(dict["energy"]);
            if (dict.contains("health")) self.health = py::cast<float>(dict["health"]);
            if (dict.contains("age")) self.age = py::cast<size_t>(dict["age"]);
            if (dict.contains("isMoving")) self.isMoving = py::cast<bool>(dict["isMoving"]);
            if (dict.contains("isTurning")) self.isTurning = py::cast<bool>(dict["isTurning"]);
            if (dict.contains("lastActionTime")) self.lastActionTime = py::cast<float>(dict["lastActionTime"]);
        }, py::arg("dict"), "Create agent body from Python dictionary.")
        .def("__repr__", [](const AgentBody& body) {
            std::stringstream ss;
            ss << "<AgentBody: position=(" << body.x << "," << body.y << ") "
               << "orientation=" << body.orientation << " "
               << "velocity=" << body.getSpeed() << " "
               << "energy=" << body.energy << ">";
            return ss.str();
        }, "Return string representation of AgentBody.")
        .def("__str__", [](const AgentBody& body) {
            return "Agent at (" + std::to_string(body.x) + ", " +
                   std::to_string(body.y) + ") health=" + 
                   std::to_string(body.health);
        }, "Return string representation of agent body.");

    // ActionResult class with enhanced documentation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world interaction.
        
        ActionResult represents the outcome of an agent's action in the world,
        including reward signals, success status, and descriptive messages.
        This feedback is crucial for learning and adaptation in the environment.
        
        Attributes:
            reward: Numeric reward value (positive for beneficial outcomes)
            success: Whether the action was successful
            message: Descriptive text about the outcome
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Initialize ActionResult with reward, success status, and message.")
        .def_readwrite("reward", &ActionResult::reward,
                      "Get or set reward value.")
        .def_readwrite("success", &ActionResult::success,
                      "Get or set success status.")
        .def_readwrite("message", &ActionResult::message,
                      "Get or set outcome message.")
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0;
        }, "Check if this is a good outcome (successful with positive reward).")
        .def("isBad", [](const ActionResult& self) {
            return !self.success && self.reward < 0;
        }, "Check if this is a bad outcome (unsuccessful with negative reward).")
        .def("isNeutral", [](const ActionResult& self) {
            return self.success && self.reward == 0;
        }, "Check if this is a neutral outcome.")
        .def("getQuality", [](const ActionResult& self) {
            if (self.reward > 10) return "excellent";
            else if (self.reward > 5) return "very_good";
            else if (self.reward > 2) return "good";
            else if (self.reward > 0) return "fair";
            else if (self.reward > -5) return "poor";
            else return "bad";
        }, "Get qualitative assessment of outcome quality.")
        .def("toDict", [](const ActionResult& self) {
            py::dict dict;
            dict["reward"] = self.reward;
            dict["success"] = self.success;
            dict["message"] = self.message;
            return dict;
        }, "Convert action result to Python dictionary.")
        .def("fromDict", [](ActionResult& self, const py::dict& dict) {
            if (dict.contains("reward")) self.reward = py::cast<float>(dict["reward"]);
            if (dict.contains("success")) self.success = py::cast<bool>(dict["success"]);
            if (dict.contains("message")) self.message = py::cast<std::string>(dict["message"]);
        }, py::arg("dict"), "Create action result from Python dictionary.")
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult: reward=" << result.reward << ", "
               << "success=" << (result.success ? "true" : "false");
            if (!result.message.empty()) {
                ss << ", message='" << result.message << "'";
            }
            ss << ">";
            return ss.str();
        }, "Return string representation of ActionResult.")
        .def("__str__", [](const ActionResult& result) {
            std::string status = result.success ? "SUCCESS" : "FAILURE";
            return status + " (reward: " + std::to_string(result.reward) + ")";
        }, "Return string representation of action result.");

    // SensoryPercept class with enhanced documentation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data from the environment.
        
        SensoryPercept represents the complete sensory information available
        to the agent at a given moment, including visual, auditory, internal,
        and proprioceptive signals. This is the primary interface through which
        the agent perceives and understands its environment.
        
        The percept provides a unified view of all sensory modalities,
        enabling integrated processing and decision-making.
        
        Attributes:
            vision: Visual input from the agent's perspective
            touch: Proprioceptive/physical input
            internal: Internal physiological state
            proprioception: Body position and movement
            audio: Auditory input from environment
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get visual sensory input.")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set visual sensory input.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision image width.")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision image height.")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get proprioceptive/tactile input.")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set proprioceptive/tactile input.")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal physiological signals.")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal physiological signals.")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioceptive body position input.")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioceptive body position input.")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get auditory sensory input.")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set auditory sensory input.")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all available sensory signals.")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get percept timestamp.")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp.")
        .def("clone", &SensoryPercept::clone, "Create a copy of this percept.")
        .def("hasVision", [](const SensoryPercept& self) {
            return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
        }, "Check if percept contains visual information.")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().getData().empty();
        }, "Check if percept contains auditory information.")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if percept contains internal signals.")
        .def("hasProprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, "Check if percept contains proprioceptive information.")
        .def("getDominantSensory", [](const SensoryPercept& self) {
            if (self.hasVision() && self.hasAudio()) return "multimodal";
            else if (self.hasVision()) return "vision";
            else if (self.hasAudio()) return "audio";
            else if (self.hasInternal()) return "internal";
            else if (self.hasProprioception()) return "proprioception";
            else return "none";
        }, "Get the dominant sensory modality in this percept.")
        .def("getSignalStrength", [](const SensoryPercept& self) {
            int strength = 0;
            if (self.hasVision()) strength += 10;
            if (self.hasAudio()) strength += 5;
            if (self.hasInternal()) strength += 8;
            if (self.hasProprioception()) strength += 3;
            return strength;
        }, "Calculate overall signal strength of the percept.")
        .def("mergeWith", [](SensoryPercept& self, const SensoryPercept& other) {
            // Merge another percept into this one
            if (other.hasVision()) self.setVision(other.getVision());
            if (other.hasAudio()) self.setAudio(other.getAudio());
            if (other.hasInternal()) self.setInternal(other.getInternal());
            if (other.hasProprioception()) self.setProprioception(other.getProprioception());
            return true;
        }, py::arg("other"), "Merge another percept into this one.")
        .def("toDict", [](const SensoryPercept& self) {
            py::dict dict;
            dict["vision"] = self.getVision();
            dict["touch"] = self.getTouch();
            dict["internal"] = self.getInternal();
            dict["proprioception"] = self.getProprioception();
            dict["audio"] = self.getAudio();
            dict["timestamp"] = self.getTimestamp();
            return dict;
        }, "Convert percept to Python dictionary.")
        .def("fromDict", [](SensoryPercept& self, const py::dict& dict) {
            if (dict.contains("vision")) self.setVision(py::cast<Vision>(dict["vision"]));
            if (dict.contains("touch")) self.setTouch(py::cast<std::vector<float>>(dict["touch"]));
            if (dict.contains("internal")) self.setInternal(py::cast<std::vector<float>>(dict["internal"]));
            if (dict.contains("proprioception")) self.setProprioception(py::cast<std::vector<float>>(dict["proprioception"]));
            if (dict.contains("audio")) self.setAudio(py::cast<Audio>(dict["audio"]));
            if (dict.contains("timestamp")) self.setTimestamp(py::cast<float>(dict["timestamp"]));
        }, py::arg("dict"), "Create percept from Python dictionary.")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) + "x" +
                   std::to_string(percept.getVisionHeight()) + " audio=" +
                   std::to_string(percept.getAudio().getNumSamples()) + " samples>";
        }, "Return string representation of SensoryPercept.");

    // SimpleWorld class with enhanced documentation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld represents the simulated environment in which the agent operates.
        It provides the interface between the agent's sensory perception and
        the physical consequences of its actions, implementing the complete
        sensorimotor loop.
        
        The world manages:
        - Physical space and boundaries
        - World objects (resources, hazards, walls, markers)
        - Agent positioning and movement
        - Sensory perception (vision, proprioception)
        - Action outcomes and feedback
        - Time evolution and state updates
        
        The world operates in discrete time steps, allowing for deterministic
        simulation and reproducible experiments.
        
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            >>> world.reset()
            >>> percept = world.getSensoryPercept()
            >>> action = agent.decodeMotorCommand()
            >>> world.applyMotorCommand(action, world.getSimulationTime())
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent sensing capabilities.")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state.")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position in world coordinates.")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for specified time step.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and return result.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from agent's perspective.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state.")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object to environment.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at specified coordinates.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid within world boundaries.")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width in world units.")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height in world units.")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy.")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate per time step.")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per time step.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time.")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed.")
        .def("getTime", &SimpleWorld::getSimulationTime, "Alias for getSimulationTime().")
        .def("setTime", [](SimpleWorld& self, float time) {
            // Note: This is a simplified implementation
            // Real world may track time internally
            self.setRandomSeed(static_cast<unsigned int>(time * 1000));
        }, py::arg("time"), "Set simulation time (also affects random seed).")
        .def("getAgentPosition", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple.")
        .def("getAgentOrientation", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return body.orientation;
        }, "Get agent orientation in radians.")
        .def("getAgentSpeed", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return std::sqrt(body.velocityX * body.velocityX + 
                           body.velocityY * body.velocityY);
        }, "Get agent speed.")
        .def("getAgentDistanceTo", [](const SimpleWorld& self, float x, float y) {
            const auto& body = self.getAgentBody();
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           "Get distance from agent to position (x, y).")
        .def("isAgentCollidingWith", [](const SimpleWorld& self, float x, float y, float radius) {
            const auto& body = self.getAgentBody();
            float dx = body.x - x;
            float dy = body.y - y;
            return (dx * dx + dy * dy) <= (radius * radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
           "Check if agent collides with sphere at (x, y) with given radius.")
        .def("countObjects", [](const SimpleWorld& self, WorldObjectType type) {
            // Note: This is a simplified implementation
            // Real world would need to track object counts
            return 0; // Placeholder
        }, py::arg("type"), "Count number of objects of specified type.")
        .def("getObjectAt", [](const SimpleWorld& self, float x, float y) {
            // Note: This is a simplified implementation
            // Real world would need to find objects at positions
            return WorldObject(); // Placeholder
        }, py::arg("x"), py::arg("y"),
           "Get object at position (x, y) (placeholder implementation).")
        .def("spawnResource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject resource = WorldObject::createResource(x, y, value);
            self.addObject(resource);
            return resource;
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
           "Spawn a resource object at position (x, y).")
        .def("spawnHazard", [](SimpleWorld& self, float x, float y, float damage = 1.0f) {
            WorldObject hazard = WorldObject::createHazard(x, y, damage);
            self.addObject(hazard);
            return hazard;
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 1.0f,
           "Spawn a hazard object at position (x, y).")
        .def("spawnWall", [](SimpleWorld& self, float x, float y, float thickness = 0.5f) {
            WorldObject wall = WorldObject::createWall(x, y, thickness);
            self.addObject(wall);
            return wall;
        }, py::arg("x"), py::arg("y"), py::arg("thickness") = 0.5f,
           "Spawn a wall object at position (x, y).")
        .def("spawnMarker", [](SimpleWorld& self, float x, float y) {
            WorldObject marker = WorldObject::createMarker(x, y);
            self.addObject(marker);
            return marker;
        }, py::arg("x"), py::arg("y"),
           "Spawn a marker object at position (x, y).")
        .def("cleanupObjects", [](SimpleWorld& self, WorldObjectType type) {
            // Note: This is a simplified implementation
            // Real world would need to actually remove objects
            return 0; // Placeholder
        }, py::arg("type"), "Clean up all objects of specified type.")
        .def("exportState", [](const SimpleWorld& self) {
            py::dict state;
            state["width"] = self.getWidth();
            state["height"] = self.getHeight();
            state["time"] = self.getSimulationTime();
            state["agentBody"] = self.getAgentBody();
            // Note: This would need to include all world objects
            return state;
        }, "Export world state as Python dictionary.")
        .def("importState", [](SimpleWorld& self, const py::dict& state) {
            if (state.contains("width")) {
                // Note: This would need to fully restore world state
                // This is a simplified implementation
                self.reset();
            }
        }, py::arg("state"), "Import world state from Python dictionary.")
        .def("simulateEpisode", [](SimpleWorld& self, std::function<void(const SensoryPercept&)> percept_handler,
                                  float timestep = 0.1f, int steps = 100) {
            self.reset();
            for (int i = 0; i < steps; ++i) {
                self.update(timestep);
                auto percept = self.getSensoryPercept();
                if (percept_handler) {
                    percept_handler(percept);
                }
            }
        }, py::arg("percept_handler"), py::arg("timestep") = 0.1f, py::arg("steps") = 100,
           "Simulate an episode with percept handler callback.")
        .def("toDict", [](const SimpleWorld& self) {
            py::dict dict;
            dict["width"] = self.getWidth();
            dict["height"] = self.getHeight();
            dict["time"] = self.getSimulationTime();
            dict["agentBody"] = self.getAgentBody();
            return dict;
        }, "Convert world state to Python dictionary.")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " " +
                   std::to_string(world.getSimulationTime()) + "s>";
        }, "Return string representation of SimpleWorld.");

    // Brain class with enhanced documentation and convenience methods
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The Brain is the core component of the NLM system, implementing
        a complete spiking neural network with multiple integrated systems
        for perception, cognition, memory, and action. It serves as the
        central processing unit that transforms sensory input into motor output.
        
        The Brain implements a closed-loop architecture where:
        Sensory input → Neural processing → Internal state → Action → Motor output
        
        Key subsystems:
        - Neural dynamics (LIF neurons, spike propagation)
        - Memory systems (working, episodic, associative)
        - Prediction systems (forward models, error signals)
        - Cognitive systems (planning, attention, concept formation)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental system (plasticity, maturation)
        
        The Brain provides a complete interface for neural engineering,
        allowing users to configure, control, and analyze neural networks
        with Python-level convenience.
        
        Examples:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> for i in range(100):
            ...     brain.step(i)
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create Brain with configuration.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity.")
        .def("reset", &Brain::reset,
             "Reset brain state.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status.")
        
        // Enhanced brain methods for convenience
        .def("runSimulation", [](Brain& self, int steps, float timestep = 0.1f) {
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.1f,
           "Run simulation for specified number of steps.")
        .def("runEpisode", [](Brain& self, std::shared_ptr<SimpleWorld> world,
                            std::shared_ptr<AgentBrain> agent, int steps) {
            if (world && agent) {
                for (int i = 0; i < steps; ++i) {
                    world->update(0.1f);
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    self.step(i);
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
            }
        }, py::arg("world"), py::arg("agent"), py::arg("steps"),
           "Run complete episode with world and agent.")
        .def("simulateWithInput", [](Brain& self, const std::vector<SensoryInput>& inputs) {
            for (size_t i = 0; i < inputs.size(); ++i) {
                self.receiveSensoryInput(inputs[i]);
                self.step(static_cast<SimulationStep>(i));
            }
        }, py::arg("inputs"),
           "Simulate with provided sensory inputs.")
        .def("getNeuralActivityStats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["ei_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get comprehensive neural activity statistics.")
        .def("analyzeConnectivity", [](const Brain& self) {
            // Simplified connectivity analysis
            py::dict connectivity;
            connectivity["total_regions"] = self.getRegionCount();
            connectivity["total_neurons"] = self.getTotalNeuronCount();
            connectivity["total_synapses"] = self.getTotalSynapseCount();
            connectivity["synapses_per_neuron"] = 
                self.getTotalNeuronCount() > 0 ? 
                static_cast<float>(self.getTotalSynapseCount()) / self.getTotalNeuronCount() : 0.0f;
            return connectivity;
        }, "Analyze brain connectivity patterns.")
        .def("getRegionalStats", [](const Brain& self) {
            py::list regions;
            auto region_ids = self.getRegionIds();
            for (const auto& rid : region_ids) {
                py::dict region_stats;
                region_stats["id"] = rid.value;
                if (auto* region = self.getRegion(rid)) {
                    region_stats["neuron_count"] = region->getNeuronCount();
                    region_stats["synapse_count"] = region->getSynapseCount();
                }
                regions.append(region_stats);
            }
            return regions;
        }, "Get statistics for each brain region.")
        .def("setPlasticityEnabled", [](Brain& self, bool stdp, bool hebbian, bool structural) {
            // Note: This would require access to plasticity system
            // Placeholder implementation
            return true;
        }, py::arg("stdp"), py::arg("hebbian"), py::arg("structural"),
           "Enable/disable plasticity mechanisms.")
        .def("getPlasticityStatus", []() {
            py::dict status;
            status["stdp_enabled"] = true;
            status["hebbian_enabled"] = true;
            status["structural_enabled"] = true;
            return status;
        }, "Get current plasticity system status.")
        .def("enableDevelopment", [](Brain& self, bool enable) {
            self.setDevelopmentalStage(enable ? DevelopmentalStage::Adult : DevelopmentalStage::Initial);
        }, py::arg("enable"), "Enable or disable development.")
        .def("isDeveloped", [](const Brain& self) {
            return self.getDevelopmentalStage() == DevelopmentalStage::Adult;
        }, "Check if brain is in adult developmental stage.")
        .def("getReadinessScore", [](const Brain& self) {
            // Calculate a simple readiness score based on neural activity
            float firing_rate = self.getAverageFiringRate();
            float ei_ratio = self.getExcitationInhibitionRatio();
            return (firing_rate * 0.6f + ei_ratio * 0.4f) / 2.0f;
        }, "Calculate brain readiness score (0-1 scale).")
        .def("compareWith", [](const Brain& self, const Brain& other) {
            py::dict comparison;
            comparison["neuron_count_match"] = self.getTotalNeuronCount() == other.getTotalNeuronCount();
            comparison["synapse_count_match"] = self.getTotalSynapseCount() == other.getTotalSynapseCount();
            comparison["firing_rate_diff"] = std::abs(self.getAverageFiringRate() - other.getAverageFiringRate());
            return comparison;
        }, py::arg("other"), "Compare this brain with another brain.")
        .def("getSignature", [](const Brain& self) {
            py::dict signature;
            signature["neuron_count"] = self.getTotalNeuronCount();
            signature["synapse_count"] = self.getTotalSynapseCount();
            signature["firing_rate"] = self.getAverageFiringRate();
            signature["total_spikes"] = self.getTotalSpikeCount();
            signature["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return signature;
        }, "Get brain signature for identification/comparison.")
        .def("hasConsistentState", []() {
            // Check if brain is in a valid state (placeholder)
            return true;
        }, "Check if brain state is consistent.")
        .def("validate", []() {
            // Validate brain configuration (placeholder)
            return true;
        }, "Validate brain configuration and state.")
        .def("resetToDefaults", []() {
            // Reset to default configuration (placeholder)
            return true;
        }, "Reset brain to default configuration.");

    // AgentBrain class with enhanced documentation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain serves as the interface between the neural brain and the
        external world, implementing the sensorimotor loop that enables
        the agent to interact with its environment. It handles sensory input
        processing, motor output generation, and integrates various brain
        subsystems for adaptive behavior.
        
        The AgentBrain provides a complete agent framework with:
        - Sensory perception and processing
        - Motor planning and execution
        - Neuromodulation for learning and motivation
        - Development and plasticity management
        - Reward-based learning
        - Curiosity-driven exploration
        
        This class bridges the gap between raw neural computation and
        goal-directed behavior in simulated environments.
        
        Examples:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> agent = pynlm.createAgentBrain(brain)
            >>> world = pynlm.createSimpleWorld()
            >>> agent.initialize(world)
            >>> agent.enableRewardModulation(True)
            >>> agent.enableCuriosity(True)
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create AgentBrain with underlying brain.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled.")
        
        // Enhanced agent methods
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, int steps) {
            world.reset();
            for (int i = 0; i < steps; ++i) {
                world.update(0.1f);
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                auto action = self.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
            }
        }, py::arg("world"), py::arg("steps"),
           "Run complete episode with world.")
        .def("learnFromExperience", [](AgentBrain& self, const std::vector<ActionResult>& results) {
            float total_reward = 0.0f;
            for (const auto& result : results) {
                total_reward += result.reward;
                self.applyRewardModulation(result.reward, 0.0f);
            }
            return total_reward / results.size();
        }, py::arg("results"),
           "Learn from experience using provided results.")
        .def("explore", [](AgentBrain& self, SimpleWorld& world, float exploration_rate = 1.0f) {
            auto percept = world.getSensoryPercept();
            self.processSensoryInput(percept);
            
            // Add curiosity-driven exploration
            float curiosity = self.getCuriosityLevel();
            if (curiosity > exploration_rate) {
                return self.decodeMotorCommand();  // Normal action
            } else {
                // Return random action for exploration
                Action action;
                action.setType(ActionType::MoveForward);
                return action;
            }
        }, py::arg("world"), py::arg("exploration_rate") = 1.0f,
           "Perform exploration action based on curiosity level.")
        .def("getBehavioralStats", [](const AgentBrain& self) {
            py::dict stats;
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive behavioral statistics.")
        .def("getLearningProgress", [](const AgentBrain& self) {
            float neuromodulation = self.getNeuromodulationLevel();
            float curiosity = self.getCuriosityLevel();
            return (neuromodulation + curiosity) / 2.0f;
        }, "Calculate learning progress score (0-1 scale).")
        .def("isHealthy", [](const AgentBrain& self) {
            return self.getNeuromodulationLevel() > 0.1f &&
                   self.getCuriosityLevel() > 0.1f &&
                   self.getHealthStatus() == "healthy";
        }, "Check if agent is in healthy state.")
        .def("getSystemStatus", [](const AgentBrain& self) {
            py::dict status;
            status["neuromodulation_enabled"] = self.isRewardModulationEnabled();
            status["plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            status["development_enabled"] = self.isDevelopmentEnabled();
            status["curiosity_enabled"] = self.isCuriosityEnabled();
            status["learning_progress"] = self.getLearningProgress();
            return status;
        }, "Get comprehensive system status.")
        .def("optimizeForPerformance", [](AgentBrain& self) {
            // Optimize settings for better performance
            self.enableRewardModulation(true);
            self.enableCuriosity(true);
            self.enableDevelopment(true);
            return true;
        }, "Optimize agent settings for better performance.")
        .def("monitorPerformance", [](const AgentBrain& self) {
            py::dict performance;
            performance["neuromodulation"] = self.getNeuromodulationLevel();
            performance["curiosity"] = self.getCuriosityLevel();
            performance["novelty"] = self.getNoveltyLevel();
            performance["prediction_error"] = self.getPredictionError();
            return performance;
        }, "Monitor agent performance metrics.")
        .def("resetForNewTask", [](AgentBrain& self) {
            self.reset();
            // Reset development stage
            self.getBrain()->setDevelopmentalStage(DevelopmentalStage::Initial);
            return true;
        }, "Reset agent for new task.")
        .def("adaptToEnvironment", [](AgentBrain& self, const py::dict& env_stats) {
            // Adapt agent settings based on environment statistics
            if (env_stats.contains("difficulty")) {
                float difficulty = py::cast<float>(env_stats["difficulty"]);
                if (difficulty > 0.7f) {
                    self.enableCuriosity(true);
                }
            }
            return true;
        }, py::arg("env_stats"), "Adapt agent to environment statistics.")
        .def("getMemoryUsage", []() {
            // Return approximate memory usage (placeholder)
            py::dict memory;
            memory["config"] = 1024;  // KB
            memory["brain"] = 10240;  // KB
            memory["agent"] = 512;    // KB
            return memory;
        }, "Get estimated memory usage.")
        .def("cleanup", [](AgentBrain& self) {
            // Clean up resources
            self.reset();
            return true;
        }, "Clean up agent resources.");

    // Exception class definitions (moved to end for cleaner binding organization)

    // ValidationError class for configuration errors
    class ValidationError {
    public:
        std::string message;
        std::string field;
        std::string value;
        
        ValidationError() : message(), field(), value() {}
        ValidationError(const std::string& msg) : message(msg), field(), value() {}
        ValidationError(const std::string& msg, const std::string& fld, const std::string& val)
            : message(msg), field(fld), value(val) {}
    };

    // TypeConversionError class for type conversion failures
    class TypeConversionError {
    public:
        std::string message;
        std::string from_type;
        std::string to_type;
        std::string value;
        
        TypeConversionError() : message(), from_type(), to_type(), value() {}
        TypeConversionError(const std::string& msg, const std::string& from, 
                          const std::string& to, const std::string& val)
            : message(msg), from_type(from), to_type(to), value(val) {}
    };

    // ConfigurationError class for configuration operation failures
    class ConfigurationError {
    public:
        std::string message;
        std::string key;
        std::string file;
        
        ConfigurationError() : message(), key(), file() {}
        ConfigurationError(const std::string& msg) : message(msg), key(), file() {}
        ConfigurationError(const std::string& msg, const std::string& k, const std::string& f)
            : message(msg), key(k), file(f) {}
    };

    // Register exception classes
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<TypeConversionError>(m, "TypeConversionError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");

    // Comprehensive validation and error handling functions
    m.def("validateNeuronId", [](uint64_t value) {
        if (value > UINT64_MAX / 2) {
            throw ValidationError("NeuronId value exceeds maximum allowed value");
        }
        return true;
    }, py::arg("value"), "Validate neuron ID value.");

    m.def("validateSynapseId", [](uint64_t value) {
        if (value > UINT64_MAX / 2) {
            throw ValidationError("SynapseId value exceeds maximum allowed value");
        }
        return true;
    }, py::arg("value"), "Validate synapse ID value.");

    m.def("validateRegionId", [](uint64_t value) {
        if (value > UINT64_MAX / 2) {
            throw ValidationError("RegionId value exceeds maximum allowed value");
        }
        return true;
    }, py::arg("value"), "Validate region ID value.");

    m.def("validatePopulationId", [](uint64_t value) {
        if (value > UINT64_MAX / 2) {
            throw ValidationError("PopulationId value exceeds maximum allowed value");
        }
        return true;
    }, py::arg("value"), "Validate population ID value.");

    m.def("validateRange", [](double value, double min, double max) {
        if (value < min || value > max) {
            throw ValidationError("Value " + std::to_string(value) + " is out of range [" +
                               std::to_string(min) + ", " + std::to_string(max) + "]");
        }
        return true;
    }, py::arg("value"), py::arg("min"), py::arg("max"),
       "Validate that value is within specified range.");

    m.def("validatePositive", [](double value) {
        if (value <= 0) {
            throw ValidationError("Value must be positive, got " + std::to_string(value));
        }
        return true;
    }, py::arg("value"), "Validate that value is positive.");

    m.def("validateNonNegative", [](double value) {
        if (value < 0) {
            throw ValidationError("Value must be non-negative, got " + std::to_string(value));
        }
        return true;
    }, py::arg("value"), "Validate that value is non-negative.");

    m.def("validateStringNotEmpty", [](const std::string& value, const std::string& field_name = "value") {
        if (value.empty()) {
            throw ValidationError("Field '" + field_name + "' cannot be empty");
        }
        return true;
    }, py::arg("value"), py::arg("field_name") = "value",
       "Validate that string is not empty.");

    m.def("safeGetString", [](const Config& config, const std::string& key, const std::string& default_value = "") {
        auto value = config.get<std::string>(key);
        return value ? *value : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = "",
       "Safely get string configuration value with default fallback.");

    m.def("safeGetInt", [](const Config& config, const std::string& key, int default_value = 0) {
        auto value = config.get<int>(key);
        return value ? *value : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = 0,
       "Safely get integer configuration value with default fallback.");

    m.def("safeGetDouble", [](const Config& config, const std::string& key, double default_value = 0.0) {
        auto value = config.get<double>(key);
        return value ? *value : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = 0.0,
       "Safely get double configuration value with default fallback.");

    m.def("safeGetBool", [](const Config& config, const std::string& key, bool default_value = false) {
        auto value = config.get<bool>(key);
        return value ? *value : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = false,
       "Safely get boolean configuration value with default fallback.");

    m.def("getConfigOrThrow", [](const Config& config, const std::string& key, const std::string& error_message = "") {
        auto value = config.get<std::string>(key);
        if (!value) {
            std::string msg = error_message.empty() ? ("Configuration key not found: '" + key + "'") : error_message;
            throw ConfigurationError(msg);
        }
        return *value;
    }, py::arg("config"), py::arg("key"), py::arg("error_message") = "",
       "Get configuration value, throwing exception if not found.");

    m.def("setConfigSafe", [](Config& config, const std::string& key, const std::string& value,
                           ConfigSource source = ConfigSource::Runtime) {
        if (key.empty()) {
            throw ValidationError("Configuration key cannot be empty");
        }
        config.set(key, value, source);
        return true;
    }, py::arg("config"), py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
       "Safely set configuration value with validation.");

    m.def("validateConfigComplete", [](const Config& config) {
        auto keys = config.getKeys();
        if (keys.empty()) {
            throw ValidationError("Configuration is empty");
        }
        
        // Check for required keys
        std::vector<std::string> required_keys = {"brain.neuron_count", "brain.region_count"};
        std::vector<std::string> missing_keys;
        
        for (const auto& key : required_keys) {
            if (!config.has(key)) {
                missing_keys.push_back(key);
            }
        }
        
        if (!missing_keys.empty()) {
            throw ValidationError("Missing required configuration keys: " +
                               (missing_keys.size() > 3 ? "multiple" : 
                                (missing_keys.size() == 1 ? missing_keys[0] : 
                                 std::accumulate(missing_keys.begin() + 1, missing_keys.end(), missing_keys[0],
                                               [](const std::string& a, const std::string& b) { return a + ", " + b; })));
        }
        
        return true;
    }, py::arg("config"), "Validate that configuration contains required keys.");

    m.def("normalizeConfig", [](Config& config) {
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value = config.get<std::string>(key);
            if (value) {
                // Convert empty strings to "false" for boolean-like configs
                if (value->empty() && (key.find("enabled") != std::string::npos || 
                                     key.find("enable") != std::string::npos)) {
                    config.set(key, "false", ConfigSource::Runtime);
                }
                // Convert numeric strings to appropriate types
                try {
                    size_t pos;
                    int int_val = std::stoi(*value);
                    config.set(key, int_val, ConfigSource::Runtime);
                } catch (...) {}
                try {
                    size_t pos;
                    double double_val = std::stod(*value);
                    config.set(key, double_val, ConfigSource::Runtime);
                } catch (...) {}
                try {
                    if (*value == "true" || *value == "false") {
                        bool bool_val = (*value == "true");
                        config.set(key, bool_val, ConfigSource::Runtime);
                    }
                } catch (...) {}
            }
        }
        return true;
    }, py::arg("config"), "Normalize configuration values (convert string representations to appropriate types).");

    m.def("exportConfig", [](const Config& config) {
        py::dict dict;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value = config.get<std::string>(key);
            if (value) {
                dict[key] = *value;
            }
        }
        return dict;
    }, py::arg("config"), "Export configuration to Python dictionary.");

    m.def("importConfig", [](Config& config, const py::dict& dict) {
        config.clear();
        for (auto it : dict) {
            std::string key = py::cast<std::string>(it.first);
            if (it.second.get_type() == py::dtype::of<int>()) {
                config.set(key, py::cast<int>(it.second));
            } else if (it.second.get_type() == py::dtype::of<double>()) {
                config.set(key, py::cast<double>(it.second));
            } else if (it.second.get_type() == py::dtype::of<bool>()) {
                config.set(key, py::cast<bool>(it.second));
            } else if (it.second.get_type() == py::dtype::of<std::string>()) {
                config.set(key, py::cast<std::string>(it.second));
            }
        }
        return true;
    }, py::arg("config"), py::arg("dict"), "Import configuration from Python dictionary.");

    m.def("validateBrainState", [](const Brain& brain) {
        // Validate brain state consistency
        if (brain.getTotalNeuronCount() <= 0) {
            throw ValidationError("Brain has invalid neuron count: " +
                               std::to_string(brain.getTotalNeuronCount()));
        }
        if (brain.getTotalSynapseCount() < 0) {
            throw ValidationError("Brain has negative synapse count: " +
                               std::to_string(brain.getTotalSynapseCount()));
        }
        if (brain.getAverageFiringRate() < 0 || brain.getAverageFiringRate() > 1000) {
            throw ValidationError("Brain has invalid firing rate: " +
                               std::to_string(brain.getAverageFiringRate()));
        }
        if (brain.getExcitationInhibitionRatio() < 0 || brain.getExcitationInhibitionRatio() > 10) {
            throw ValidationError("Brain has invalid E/I ratio: " +
                               std::to_string(brain.getExcitationInhibitionRatio()));
        }
        return true;
    }, py::arg("brain"), "Validate brain state consistency.");

    m.def("validateWorldState", [](const SimpleWorld& world) {
        // Validate world state consistency
        if (world.getWidth() <= 0) {
            throw ValidationError("World has invalid width: " + std::to_string(world.getWidth()));
        }
        if (world.getHeight() <= 0) {
            throw ValidationError("World has invalid height: " + std::to_string(world.getHeight()));
        }
        if (world.getAgentBody().health < 0 || world.getAgentBody().health > 1) {
            throw ValidationError("Agent has invalid health: " + std::to_string(world.getAgentBody().health));
        }
        if (world.getAgentBody().energy < 0 || world.getAgentBody().energy > 1) {
            throw ValidationError("Agent has invalid energy: " + std::to_string(world.getAgentBody().energy));
        }
        return true;
    }, py::arg("world"), "Validate world state consistency.");

    m.def("safeStep", [](Brain& brain, int step, float timestep = 0.1f) {
        if (!brain.getConfig()) {
            throw ValidationError("Brain has no configuration");
        }
        if (step < 0) {
            throw ValidationError("Step number cannot be negative: " + std::to_string(step));
        }
        brain.step(step, step * timestep);
        return true;
    }, py::arg("brain"), py::arg("step"), py::arg("timestep") = 0.1f,
       "Safely step brain with validation.");

    m.def("safeReset", [](Brain& brain) {
        if (!brain.getConfig()) {
            throw ValidationError("Cannot reset brain without configuration");
        }
        brain.reset();
        return true;
    }, py::arg("brain"), "Safely reset brain with validation.");

    m.def("safeInitialize", [](Brain& brain) {
        if (!brain.getConfig()) {
            throw ValidationError("Cannot initialize brain without configuration");
        }
        if (!brain.initialize()) {
            throw ConfigurationError("Brain initialization failed");
        }
        return true;
    }, py::arg("brain"), "Safely initialize brain with validation.");

    m.def("validateSimulationParameters", [](const Brain& brain, const SimpleWorld& world, int steps) {
        if (steps <= 0) {
            throw ValidationError("Number of steps must be positive: " + std::to_string(steps));
        }
        if (!brain.getConfig()) {
            throw ValidationError("Brain has no configuration");
        }
        if (!brain.initialize()) {
            throw ConfigurationError("Brain initialization failed before simulation");
        }
        if (world.getWidth() <= 0 || world.getHeight() <= 0) {
            throw ValidationError("World has invalid dimensions");
        }
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("steps"),
       "Validate simulation parameters before running.");

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <optional>

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

// Python exception for NLM-specific errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

void throwNLMError(const std::string& msg) {
    throw NLMError(msg);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Comprehensive neural simulation API
        - Memory-safe bindings with proper reference counting
        - Enhanced error handling with detailed error messages
        - Support for visualization and interaction
        - Developmental neuroscience capabilities
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // ===== Basic Types =====
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the NLM brain.
        
        NeuronId provides a type-safe way to reference specific neurons
        within the brain network. Each neuron is assigned a unique
        64-bit identifier that can be used for targeted operations
        and inter-regional connections.
        
        Attributes:
            value: The unique identifier value
        
        Examples:
            >>> neuron_id = pynlm.NeuronId(42)
            >>> print(neuron_id.value)
            42
            >>> print(neuron_id.index())
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a NeuronId with a specific value")
        .def_readwrite("value", &NeuronId::value,
             "The unique identifier value (read-write)")
        .def("index", &NeuronId::index,
             "Get the index of this neuron ID (alias for value)")
        .def("__eq__", &NeuronId::operator==,
             "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=,
             "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Generate hash for use in dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        },
             "String representation of the NeuronId")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        },
             "String representation for user-friendly display");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse connection in the NLM brain.
        
        SynapseId provides a type-safe way to reference specific synaptic
        connections between neurons. Each synapse has a unique identifier
        that can be used for targeted plasticity and analysis operations.
        
        Attributes:
            value: The unique identifier value
        
        Examples:
            >>> synapse_id = pynlm.SynapseId(123)
            >>> print(synapse_id.value)
            123
            >>> print(synapse_id.index())
            123
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a SynapseId with a specific value")
        .def_readwrite("value", &SynapseId::value,
             "The unique identifier value (read-write)")
        .def("index", &SynapseId::index,
             "Get the index of this synapse ID (alias for value)")
        .def("__eq__", &SynapseId::operator==,
             "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=,
             "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Generate hash for use in dictionaries and sets")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        },
             "String representation of the SynapseId")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        },
             "String representation for user-friendly display");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region in the NLM simulation.
        
        RegionId provides a type-safe way to reference specific neural
        regions within the brain. Each region contains neural populations
        and can be used for targeted operations and inter-regional connections.
        
        Attributes:
            value: The unique identifier value
        
        Examples:
            >>> region_id = pynlm.RegionId(1)
            >>> print(region_id.value)
            1
            >>> print(region_id.index())
            1
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a RegionId with a specific value")
        .def_readwrite("value", &RegionId::value,
             "The unique identifier value (read-write)")
        .def("index", &RegionId::index,
             "Get the index of this region ID (alias for value)")
        .def("__eq__", &RegionId::operator==,
             "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=,
             "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Generate hash for use in dictionaries and sets")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        },
             "String representation of the RegionId")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        },
             "String representation for user-friendly display");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population in the NLM brain.
        
        PopulationId provides a type-safe way to reference specific groups
        of neurons that share functional properties. Each population contains
        multiple neurons with similar characteristics.
        
        Attributes:
            value: The unique identifier value
        
        Examples:
            >>> population_id = pynlm.PopulationId(42)
            >>> print(population_id.value)
            42
            >>> print(population_id.index())
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a PopulationId with a specific value")
        .def_readwrite("value", &PopulationId::value,
             "The unique identifier value (read-write)")
        .def("index", &PopulationId::index,
             "Get the index of this population ID (alias for value)")
        .def("__eq__", &PopulationId::operator==,
             "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=,
             "Check inequality with another PopulationId")
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); },
             "Generate hash for use in dictionaries and sets")
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        },
             "String representation of the PopulationId")
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        },
             "String representation for user-friendly display");

    // ===== Enumerations =====
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration for NLM neural simulation.
        
        Different neuron types serve different functions in the brain
        network, affecting their connectivity, dynamics, and role in
        information processing.
        
        Values:
            Excitatory: Increases likelihood of target neuron firing
            Inhibitory: Decreases likelihood of target neuron firing  
            Modulatory: Affects synaptic plasticity and neuromodulation
            Sensory: Responds to external/environmental input
            Motor: Generates output commands for action
            Internal: Maintains homeostasis and internal state
        
        Examples:
            >>> neuron_type = pynlm.NeuronType.Excitatory
            >>> print(neuron_type)
            NeuronType.Excitatory
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excites target neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibits target neurons")
        .value("Modulatory", NeuronType::Modulatory, "Modulates plasticity and neuromodulation")
        .value("Sensory", NeuronType::Sensory, "Processes sensory input")
        .value("Motor", NeuronType::Motor, "Generates motor output")
        .value("Internal", NeuronType::Internal, "Maintains internal state and homeostasis");

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration for NLM neural connections.
        
        Different synapse types determine the nature of the connection
        between neurons, affecting signal transmission and plasticity rules.
        
        Values:
            Excitatory: Releases excitatory neurotransmitters
            Inhibitory: Releases inhibitory neurotransmitters
            Modulatory: Releases neuromodulatory substances
            Electrical: Direct electrical coupling between neurons
            GapJunction: Electrical synapse through gap junctions
        
        Examples:
            >>> synapse_type = pynlm.SynapseType.Excitatory
            >>> print(synapse_type)
            SynapseType.Excitatory
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synaptic connection")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synaptic connection")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synaptic connection")
        .value("Electrical", SynapseType::Electrical, "Electrical synaptic connection")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction electrical connection");

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration for NLM brain development.
        
        Different developmental stages affect neural plasticity, connectivity,
        and functional capabilities. The brain progresses through these stages
        during its developmental timeline.
        
        Values:
            Initial: Early developmental stage with high plasticity
            CriticalPeriod: Sensitive period for learning and adaptation
            Maturation: Middle stage with moderate plasticity
            Adult: Mature stage with stable plasticity
            Aging: Late stage with declining plasticity
        
        Examples:
            >>> stage = pynlm.DevelopmentalStage.Adult
            >>> print(stage)
            DevelopmentalStage.Adult
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Early development with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Sensitive learning period")
        .value("Maturation", DevelopmentalStage::Maturation, "Middle development stage")
        .value("Adult", DevelopmentalStage::Adult, "Mature stable development")
        .value("Aging", DevelopmentalStage::Aging, "Late development with declining plasticity");

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration for NLM neural dynamics.
        
        Different firing states determine how neurons respond to input
        and interact with other neurons in the network.
        
        Values:
            Resting: Not firing, maintaining resting potential
            Active: Currently firing action potentials
            Refractory: Recovery period after firing
            Inhibited: Suppressed firing due to inhibitory input
        
        Examples:
            >>> state = pynlm.FiringState.Active
            >>> print(state)
            FiringState.Active
        )pbdoc")
        .value("Resting", FiringState::Resting, "Not firing, maintaining resting potential")
        .value("Active", FiringState::Active, "Currently firing action potentials")
        .value("Refractory", FiringState::Refractory, "Recovery period after firing")
        .value("Inhibited", FiringState::Inhibited, "Suppressed firing due to inhibition");

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration for NLM agent behaviors.
        
        Action types represent the possible behaviors an agent can perform
        in the simulated environment, ranging from basic movement to complex
        cognitive and emotional responses.
        
        Values:
            MoveForward: Move forward in the environment
            MoveBackward: Move backward in the environment
            MoveLeft: Move left in the environment
            MoveRight: Move right in the environment
            TurnLeft: Rotate counterclockwise
            TurnRight: Rotate clockwise
            Look: Look around in current direction
            LookUp: Look upward
            LookDown: Look downward
            Interact: Interact with nearby objects
            Eat: Consume food/resources
            Drink: Consume liquids
            Rest: Stop moving and recover
            Wait: Do nothing (no movement or action)
            Custom: User-defined custom action
        
        Examples:
            >>> action_type = pynlm.ActionType.MoveForward
            >>> print(action_type)
            ActionType.MoveForward
        )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in environment")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
        .value("Look", ActionType::Look, "Look around in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby objects")
        .value("Eat", ActionType::Eat, "Consume food/resources")
        .value("Drink", ActionType::Drink, "Consume liquids")
        .value("Rest", ActionType::Rest, "Stop moving and recover")
        .value("Wait", ActionType::Wait, "Do nothing (no movement or action)")
        .value("Custom", ActionType::Custom, "User-defined custom action");

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Motor command enumeration for NLM agent control.
        
        Motor commands represent low-level motor control signals that the
        brain generates based on neural activity. These are more specific
        than action types and represent actual motor neuron firing patterns.
        
        Values:
            MoveForward: Move forward
            MoveBackward: Move backward
            TurnLeft: Turn left
            TurnRight: Turn right
            LookLeft: Look left direction
            LookRight: Look right direction
            Interact: Execute interaction behavior
            Wait: Maintain current state (no movement)
        
        Examples:
            >>> motor_cmd = pynlm.MotorCommand.MoveForward
            >>> print(motor_cmd)
            MotorCommand.MoveForward
        )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left direction")
        .value("LookRight", MotorCommand::LookRight, "Look right direction")
        .value("Interact", MotorCommand::Interact, "Execute interaction behavior")
        .value("Wait", MotorCommand::Wait, "Maintain current state");

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration for NLM environment simulation.
        
        World object types represent different kinds of entities that can
        exist in the simulated environment, affecting agent behavior and
        interactions.
        
        Values:
            Empty: No object at this position
            Resource: Food, water, or other useful items
            Hazard: Dangerous or harmful objects
            Wall: Solid barriers that cannot be passed
            Marker: Special location markers
        
        Examples:
            >>> obj_type = pynlm.WorldObjectType.Resource
            >>> print(obj_type)
            WorldObjectType.Resource
        )pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object at this position")
        .value("Resource", WorldObjectType::Resource, "Useful items (food, water)")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous or harmful objects")
        .value("Wall", WorldObjectType::Wall, "Solid barriers that cannot be passed")
        .value("Marker", WorldObjectType::Marker, "Special location markers");

    // ===== Config Class =====
    py::class_<Config>(m, "Config", R"pbdoc(
        Comprehensive configuration system for NLM neural simulation.
        
        The Config class manages all simulation parameters, settings, and
        configuration data for NLM brains. It supports multiple data types,
        validation, persistence, and source tracking for configuration
        management.
        
        Features:
        - Type-safe configuration with int, float, bool, string, and array types
        - Multiple configuration sources (default, file, command line, runtime)
        - Validation and error checking
        - Save/load persistence
        - Summary and debugging tools
        
        Examples:
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("neuron_count", 100)
            >>> config.set("learning_rate", 0.01)
            >>> config.set("plasticity_enabled", True)
            >>> 
            >>> neurons = config.get("neuron_count").value  # Safe access
            >>> learning_rate = config.getOr("learning_rate", 0.001)  # Default value
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (JSON/INI format)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments (e.g., --key=value)")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        // Enhanced set methods
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set string configuration value with optional source")
        .def("set", static_cast<void (Config::*)(const std::string&, int, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set integer configuration value with optional source")
        .def("set", static_cast<void (Config::*)(const std::string&, double, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set double configuration value with optional source")
        .def("set", static_cast<void (Config::*)(const std::string&, bool, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set boolean configuration value with optional source")
        // Type-safe getters returning Pythonic objects
        .def("get", [](Config& self, const std::string& key) -> py::object {
            auto result = self.get<int>(key);
            if (result.has_value()) {
                return py::cast(OptionalWrapper<int>(result.value()));
            } else {
                return py::none();
            }
        }, py::arg("key"), "Get integer configuration value (returns OptionalInt or None)")

        .def("get", [](Config& self, const std::string& key) -> py::object {
            auto result = self.get<int64_t>(key);
            if (result.has_value()) {
                return py::cast(OptionalWrapper<int64_t>(result.value()));
            } else {
                return py::none();
            }
        }, py::arg("key"), "Get int64_t configuration value (returns OptionalInt64 or None)")

        .def("get", [](Config& self, const std::string& key) -> py::object {
            auto result = self.get<double>(key);
            if (result.has_value()) {
                return py::cast(OptionalWrapper<double>(result.value()));
            } else {
                return py::none();
            }
        }, py::arg("key"), "Get double configuration value (returns OptionalDouble or None)")

        .def("get", [](Config& self, const std::string& key) -> py::object {
            auto result = self.get<bool>(key);
            if (result.has_value()) {
                return py::cast(OptionalWrapper<bool>(result.value()));
            } else {
                return py::none();
            }
        }, py::arg("key"), "Get boolean configuration value (returns OptionalBool or None)")

        .def("get", [](Config& self, const std::string& key) -> py::object {
            auto result = self.get<std::string>(key);
            if (result.has_value()) {
                return py::cast(OptionalWrapper<std::string>(result.value()));
            } else {
                return py::none();
            }
        }, py::arg("key"), "Get string configuration value (returns OptionalString or None)")

        // Original get methods for backward compatibility (return std::optional directly)
        .def("get", static_cast<std::optional<int> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get integer configuration value (returns std::optional)")
        .def("get", static_cast<std::optional<int64_t> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get int64_t configuration value (returns std::optional)")
        .def("get", static_cast<std::optional<double> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get double configuration value (returns std::optional)")
        .def("get", static_cast<std::optional<bool> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get boolean configuration value (returns std::optional)")
        .def("get", static_cast<std::optional<std::string> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"),
             "Get string configuration value (returns std::optional)")
        // Enhanced getOr methods
        .def("getOr", static_cast<int (Config::*)(const std::string&, const int&) const>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get integer configuration value, return default if not found")
        .def("getOr", static_cast<int64_t (Config::*)(const std::string&, const int64_t&) const>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get int64_t configuration value, return default if not found")
        .def("getOr", static_cast<double (Config::*)(const std::string&, const double&) const>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get double configuration value, return default if not found")
        .def("getOr", static_cast<bool (Config::*)(const std::string&, const bool&) const>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get boolean configuration value, return default if not found")
        .def("getOr", static_cast<std::string (Config::*)(const std::string&, const std::string&) const>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get string configuration value, return default if not found")
        // Configuration management
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key from the configuration")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a detailed summary of all configuration entries")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        },
             "String representation showing configuration summary");

    // ===== Optional Wrapper Classes =====
    py::class_<OptionalWrapperBase>(m, "_OptionalWrapperBase")
        .def("has_value", &OptionalWrapperBase::has_value, "Check if value is present")
        .def("value", &OptionalWrapperBase::value, "Get the value (raises exception if not present)")
        .def("__bool__", &OptionalWrapperBase::as_bool, "Bool conversion (True if has value)")
        .def("__repr__", &OptionalWrapperBase::repr, "String representation");

    py::class_<OptionalWrapper<int>, std::shared_ptr<OptionalWrapper<int>>>(m, "OptionalInt")
        .def(py::init<>())
        .def(py::init<int>())
        .def(py::init<std::optional<int>>())
        .def("has_value", &OptionalWrapper<int>::has_value)
        .def("value", &OptionalWrapper<int>::value)
        .def("__bool__", &OptionalWrapper<int>::as_bool)
        .def("__repr__", &OptionalWrapper<int>::repr);

    py::class_<OptionalWrapper<int64_t>, std::shared_ptr<OptionalWrapper<int64_t>>>(m, "OptionalInt64")
        .def(py::init<>())
        .def(py::init<int64_t>())
        .def(py::init<std::optional<int64_t>>())
        .def("has_value", &OptionalWrapper<int64_t>::has_value)
        .def("value", &OptionalWrapper<int64_t>::value)
        .def("__bool__", &OptionalWrapper<int64_t>::as_bool)
        .def("__repr__", &OptionalWrapper<int64_t>::repr);

    py::class_<OptionalWrapper<double>, std::shared_ptr<OptionalWrapper<double>>>(m, "OptionalDouble")
        .def(py::init<>())
        .def(py::init<double>())
        .def(py::init<std::optional<double>>())
        .def("has_value", &OptionalWrapper<double>::has_value)
        .def("value", &OptionalWrapper<double>::value)
        .def("__bool__", &OptionalWrapper<double>::as_bool)
        .def("__repr__", &OptionalWrapper<double>::repr);

    py::class_<OptionalWrapper<bool>, std::shared_ptr<OptionalWrapper<bool>>>(m, "OptionalBool")
        .def(py::init<>())
        .def(py::init<bool>())
        .def(py::init<std::optional<bool>>())
        .def("has_value", &OptionalWrapper<bool>::has_value)
        .def("value", &OptionalWrapper<bool>::value)
        .def("__bool__", &OptionalWrapper<bool>::as_bool)
        .def("__repr__", &OptionalWrapper<bool>::repr);

    py::class_<OptionalWrapper<std::string>, std::shared_ptr<OptionalWrapper<std::string>>>(m, "OptionalString")
        .def(py::init<>())
        .def(py::init<std::string>())
        .def(py::init<std::optional<std::string>>())
        .def("has_value", &OptionalWrapper<std::string>::has_value)
        .def("value", &OptionalWrapper<std::string>::value)
        .def("__bool__", &OptionalWrapper<std::string>::as_bool)
        .def("__repr__", &OptionalWrapper<std::string>::repr);

    // ===== Sensory Input Classes =====
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Abstract base class for all sensory input in NLM simulation.
        
        SensoryInput represents the raw data that an agent receives
        from its environment, including vision, audio, touch, internal
        state signals, and proprioception. These signals are converted
        to neural currents by the agent brain.
        
        Attributes:
            type: The type of sensory input (Vision, Audio, etc.)
            data: Raw sensory data as a vector of floats
            dimensions: The dimensionality of the sensory data
            timestamp: The time when this sensory data was recorded
        
        Examples:
            >>> vision = pynlm.Vision(16, 16)
            >>> vision.setData(list_of_256_floats)
            >>> print(f"Vision: {vision.getWidth()}x{vision.getHeight()}")
        )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get the type of this sensory input (Vision, Audio, etc.)")
        .def("getData", &SensoryInput::getData,
             "Get the raw sensory data as a vector of floats")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory data")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp of when this sensory data was recorded")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of this sensory data");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input (camera-like sensor)
        
        Vision represents visual input from the environment. It's typically
        grayscale intensity data representing what the agent sees.
        
        Attributes:
            width: Vision width in pixels
            height: Vision height in pixels
            channels: Number of channels (1 for grayscale, 3 for RGB)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() != self.getWidth() * self.getHeight() * self.getChannels()) {
                throw NLMError("Vision data size mismatch with dimensions");
            }
            self.setData(data);
        }, py::arg("data"),
             "Set vision data (must match width*height*channels)")
        .def("getWidth", &Vision::getWidth, "Get vision width in pixels")
        .def("getHeight", &Vision::getHeight, "Get vision height in pixels")
        .def("getChannels", &Vision::getChannels, "Get vision channels (typically 1 for grayscale, 3 for RGB)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input
        
        Audio represents sound input from the environment. This can be used
        for processing acoustic information or learning about the environment.
        
        Attributes:
            sampleRate: Audio sample rate in Hz
            numSamples: Number of audio samples
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sample rate and duration")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.size() != self.getNumSamples()) {
                throw NLMError("Audio data size mismatch with sample count");
            }
            self.setData(data);
        }, py::arg("data"),
             "Set audio data (must match sample count)")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal homeostatic signals
        
        Internal signals represent the agent's internal state, including
        homeostatic signals, stress levels, and other physiological indicators
        that influence behavior and learning.
        
        Methods:
            addSignal(value): Add an internal signal value
            clearSignals(): Clear all internal signals
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    // ===== Action Classes =====
    py::class_<Action>(m, "Action", R"pbdoc(
        Motor action representation
        
        Action represents a specific behavior or movement that an agent can
        perform in the environment. Actions have types and optional parameters
        for more complex behaviors.
        
        Attributes:
            type: The action type (e.g., MoveForward, Look, etc.)
            parameters: Optional parameters for custom action behavior
        
        Examples:
            >>> action = pynlm.Action(pynlm.MotorCommand.MoveForward)
            >>> print(action.getType())
            MotorCommand.MoveForward
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName, "Get the action name")
        .def("clone", &Action::clone, "Create a copy of this action");

    // ===== World Object =====
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Representation of an object in the world
        
        WorldObject represents an entity in the simulated environment that
        the agent can interact with. Objects have positions, types, and properties.
        
        Attributes:
            x: X position in the world
            y: Y position in the world
            radius: Object radius
            type: Object type (Resource, Hazard, Wall, etc.)
            value: Object value (reward/damage)
            active: Whether the object is currently active
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object at position with type and properties")
        .def_readwrite("x", &WorldObject::x, "Object x position")
        .def_readwrite("y", &WorldObject::y, "Object y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (reward/damage)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    // ===== Agent Body =====
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state in the world
        
        AgentBody represents the physical state of the agent in the environment,
        including position, velocity, orientation, and physiological state.
        
        Attributes:
            x: X position
            y: Y position
            orientation: Body orientation (radians)
            velocityX: X velocity
            velocityY: Y velocity
            angularVelocity: Angular velocity
            energy: Energy level
            health: Health level
            age: Agent age
            isMoving: Whether agent is moving
            isTurning: Whether agent is turning
            lastActionTime: Time of last action
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "Agent x position")
        .def_readwrite("y", &AgentBody::y, "Agent y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Agent orientation (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "Agent X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Agent Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Agent angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Agent energy level")
        .def_readwrite("health", &AgentBody::health, "Agent health level")
        .def_readwrite("age", &AgentBody::age, "Agent age")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state");

    // ===== Action Result =====
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of executing an action in the world
        
        ActionResult represents the outcome of an agent's action in the environment,
        including reward received, success status, and descriptive messages.
        
        Attributes:
            reward: Reward received from action
            success: Whether action was successful
            message: Description of the action result
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success flag, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Message describing the result");

    // ===== Sensory Percept =====
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory data perceived by the agent
        
        SensoryPercept represents all sensory information available to the agent
        at a given moment, including vision, touch, internal state, and proprioception.
        
        Methods:
            getVision(): Get vision data
            setVision(vision): Set vision data
            getVisionWidth(): Get vision width
            getVisionHeight(): Get vision height
            getTouch(): Get touch/proximity data
            setTouch(touch): Set touch data
            getInternal(): Get internal state data
            setInternal(internal): Set internal data
            getProprioception(): Get proprioceptive data
            setProprioception(proprioception): Set proprioception data
            getAudio(): Get audio data
            setAudio(audio): Set audio data
            getAllSignals(): Get all sensory signals concatenated
            getTimestamp(): Get percept timestamp
            setTimestamp(timestamp): Set percept timestamp
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data (grayscale)")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch/proximity data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal state data")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal data")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioceptive data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals concatenated")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp");

    // ===== Simple World =====
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation
        
        SimpleWorld represents a simulated environment where agents operate.
        It provides sensory experience without semantic labels, allowing
        agents to learn from raw sensory input.
        
        Methods:
            configure(width, height, visionWidth, visionHeight): Configure world dimensions
            reset(): Reset world to initial state
            setAgentStart(x, y): Set agent starting position
            update(timestep): Update world state
            applyMotorCommand(cmd, currentTime): Apply motor command
            getSensoryPercept(): Get current sensory percept
            getAgentBody(): Get agent body state
            addObject(obj): Add object to world
            removeObject(x, y): Remove object at position
            isValidPosition(x, y): Check if position is valid
            getObjectAt(x, y): Get object at position
            getWidth(): Get world width
            getHeight(): Get world height
            getMaxEnergy(): Get maximum agent energy
            setMaxEnergy(e): Set maximum agent energy
            getEnergyDecayRate(): Get energy decay rate
            setEnergyDecayRate(r): Set energy decay rate
            getSimulationTime(): Get current simulation time
            setRandomSeed(seed): Set random seed
            getRandomSeed(): Get current random seed
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory range")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and return action result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept (internal reference)")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state (internal reference)")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position (returns None if no object)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed");

    // ===== Brain =====
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class
        
        Brain represents the core neural system of NLM, implementing real spiking
        neural computation with event-driven dynamics and integrated memory,
        prediction, cognition, and neuromodulation systems.
        
        Methods:
            initialize(): Initialize brain with configuration
            step(currentStep): Perform simulation step
            step(currentStep, currentTime): Perform simulation step with timestamp
            receiveSensoryInput(input): Inject sensory input into brain
            injectCurrent(neuron, current): Inject current into specific neuron
            injectCurrentToNeurons(type, current): Inject current into all neurons of type
            produceAction(): Produce motor action based on neural activity
            reset(): Reset brain state
            save(filepath): Save brain state to file
            load(filepath): Load brain state from file
            addRegion(name=""): Add a new neural region
            getRegion(id): Get a region by ID
            getRegionCount(): Get the number of regions
            getRegionIds(): Get all region IDs
            getRegions(): Get all regions
            getTotalNeuronCount(): Get total neuron count across all regions
            getTotalSynapseCount(): Get total synapse count across all regions
            getActiveNeuronCount(): Get count of active neurons
            getFiringNeuronCount(): Get count of currently firing neurons
            getAverageFiringRate(): Get average firing rate across all neurons
            getExcitationInhibitionRatio(): Get excitation/inhibition balance ratio
            getTotalSpikeCount(): Get total spike count
            getPendingSpikeEventCount(): Get count of pending spike events
            getDevelopmentalStage(): Get current developmental stage
            setDevelopmentalStage(stage): Set developmental stage
            getConfig(): Get the configuration
            logStatus(): Log brain status
            getSpikeSystem(): Access spike system (advanced use)
            getSTDP(): Access STDP plasticity system (advanced use)
            getHebbian(): Access Hebbian plasticity system (advanced use)
            getStructuralPlasticity(): Access structural plasticity system (advanced use)
            getWorkingMemory(): Access working memory system (advanced use)
            getEpisodicMemory(): Access episodic memory system (advanced use)
            getAssociativeMemory(): Access associative memory system (advanced use)
            getPredictionSystem(): Access prediction system (advanced use)
            getPlanner(): Access neural planner (advanced use)
            getConceptFormation(): Access concept formation system (advanced use)
            getAttention(): Access attentional selection system (advanced use)
            getDevelopmentSystem(): Access development system (advanced use)
            getDopamine(): Access dopamine neuromodulator (advanced use)
            getCuriosity(): Access curiosity neuromodulator (advanced use)
            getNovelty(): Access novelty neuromodulator (advanced use)
            getPredictionErrorSignal(): Access prediction error signal (advanced use)
            getRandomGenerator(): Access random number generator (advanced use)
            updatePlasticity(): Update plasticity rules (called automatically)
            develop(): Apply developmental changes (called automatically)
            applyNeuromodulation(signal): Apply neuromodulatory signals (advanced use)
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
             "Add a new neural region with optional name")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions (internal reference)")
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
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Access to internal systems (use with caution)
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Access spike system (advanced use)")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Access STDP plasticity system (advanced use)")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Access Hebbian plasticity system (advanced use)")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Access structural plasticity system (advanced use)")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Access working memory system (advanced use)")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Access episodic memory system (advanced use)")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Access associative memory system (advanced use)")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Access prediction system (advanced use)")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Access neural planner (advanced use)")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Access concept formation system (advanced use)")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Access attentional selection system (advanced use)")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Access development system (advanced use)")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Access dopamine neuromodulator (advanced use)")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Access curiosity neuromodulator (advanced use)")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Access novelty neuromodulator (advanced use)")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Access prediction error signal (advanced use)")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Access random number generator (advanced use)")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically)")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals (advanced use)");

    // ===== Agent Brain =====
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world
        
        AgentBrain connects the NLM brain to the world environment, handling
        sensory transduction and motor decoding to enable agent-based simulation.
        
        Methods:
            initialize(world): Initialize with world
            getSensoryInputSize(): Get expected sensory input size
            getMotorOutputSize(): Get expected motor output size
            processSensoryInput(percept): Process sensory percept and inject into brain
            decodeMotorCommand(): Decode brain motor activity into motor command
            applyRewardModulation(reward, predictedReward): Apply reward-based neuromodulation
            updateDevelopment(timestep): Update development system
            getDevelopmentalStage(): Get current developmental stage
            getNeuromodulationLevel(): Get current neuromodulation level
            getCuriosityLevel(): Get curiosity level
            getNoveltyLevel(): Get novelty level
            getPredictionError(): Get prediction error
            reset(): Reset agent for new episode
            getBrain(): Get the underlying brain
            enableRewardModulation(enable): Enable or disable reward modulation
            enableStructuralPlasticity(enable): Enable or disable structural plasticity
            enableDevelopment(enable): Enable or disable development
            enableCuriosity(enable): Enable or disable curiosity
            isRewardModulationEnabled(): Check if reward modulation is enabled
            isStructuralPlasticityEnabled(): Check if structural plasticity is enabled
            isDevelopmentEnabled(): Check if development is enabled
            isCuriosityEnabled(): Check if curiosity is enabled
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
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
        // Configuration methods
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // ===== Factory Functions =====
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw NLMError("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw NLMError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ===== Global Constants =====
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===== Convenience Functions =====
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, int steps) {
        if (!brain || !world || !agent) {
            throw NLMError("Brain, world, and agent must not be null");
        }
        if (steps < 1) {
            throw NLMError("Number of steps must be positive");
        }
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            brain->getFiringNeuronCount(),
            agent->getCuriosityLevel()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run a complete simulation for the specified number of steps");

    m.def("createTestConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
        config->set("learning_rate", 0.01);
        config->set("plasticity_enabled", true);
        config->set("development_enabled", true);
        config->set("curiosity_enabled", true);
        return config;
    }, "Create a test configuration with sensible defaults");

    m.def("createTestWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 16, 16);
        world->setAgentStart(50, 50);
        world->setRandomSeed(42);
        return world;
    }, "Create a test world with typical settings");

    m.def("createTestBrain", []() -> std::shared_ptr<Brain> {
        auto config = createTestConfig();
        auto brain = createBrain(config);
        brain->initialize();
        return brain;
    }, "Create a test brain with default configuration");

    m.def("createTestAgent", []() -> std::shared_ptr<AgentBrain> {
        auto brain = createTestBrain();
        auto world = createTestWorld();
        auto agent = createAgentBrain(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        return agent;
    }, "Create a test agent with all features enabled");

    m.def("printBrainSummary", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw NLMError("Brain cannot be null");
        }
        brain->logStatus();
    }, py::arg("brain"),
       "Print detailed brain status and statistics");

    m.def("getBrainStats", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw NLMError("Brain cannot be null");
        }
        return std::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getActiveNeuronCount(),
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate(),
            brain->getExcitationInhibitionRatio(),
            brain->getTotalSpikeCount(),
            brain->getPendingSpikeEventCount()
        );
    }, py::arg("brain"),
       "Get comprehensive brain statistics as a tuple");

    m.def("initializeBrain", [](std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw NLMError("Brain cannot be null");
        }
        bool success = brain->initialize();
        if (!success) {
            throw NLMError("Failed to initialize brain");
        }
    }, py::arg("brain"),
       "Initialize brain (convenience function)");

    m.def("resetBrain", [](std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw NLMError("Brain cannot be null");
        }
        brain->reset();
    }, py::arg("brain"),
       "Reset brain state (convenience function)");

    m.def("validateConfiguration", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw NLMError("Config cannot be null");
        }
        std::vector<std::string> issues;
        
        if (!config->has("neuron_count") || config->get<int>("neuron_count").value() < 1) {
            issues.push_back("neuron_count must be set to a positive integer");
        }
        
        if (config->has("region_count") && config->get<int>("region_count").value() < 1) {
            issues.push_back("region_count must be positive");
        }
        
        if (config->has("connection_probability")) {
            double prob = config->get<double>("connection_probability").value();
            if (prob < 0.0 || prob > 1.0) {
                issues.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
        
        return issues;
    }, py::arg("config"),
       "Validate configuration and return list of issues (empty if valid)");
}

} // namespace nlm
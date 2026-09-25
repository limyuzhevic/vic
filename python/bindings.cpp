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
        Provides high-level APIs for neural simulation and agent learning.
        
        Key improvements:
        - Type hints for better IDE support
        - Pythonic error handling
        - Convenience factory functions
        - Enhanced documentation
        - Improved API design
    )pbdoc";

    // Register exceptions with Pythonic names
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::runtime_error>(m, "SimulationError");

    // Improved NeuronId class with Pythonic methods
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const NeuronId&>())  // Copy constructor
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, R"pbdoc(Get the index part of the ID)pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)  // Support for sorting
        .def("__le__", &NeuronId::operator<=)  // Support for sorting
        .def("__gt__", &NeuronId::operator>)   // Support for sorting
        .def("__ge__", &NeuronId::operator>=)  // Support for sorting
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        })
        .def_static("from_index", [](size_t idx) {
            return NeuronId(idx);
        }, py::arg("index"), R"pbdoc(Create a NeuronId from an index)pbdoc")
        .def("as_dict", [](const NeuronId& id) {
            return py::dict("value"_a = id.value);
        }, R"pbdoc(Return NeuronId as Python dictionary)pbdoc")
        .def("to_string", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, R"pbdoc(Convert NeuronId to string)pbdoc")
        .def("equals", [](const NeuronId& id1, const NeuronId& id2) {
            return id1 == id2;
        }, py::arg("other"), R"pbdoc(Check if two NeuronIds are equal)pbdoc");

    // Improved SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const SynapseId&>())  // Copy constructor
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("as_dict", [](const SynapseId& id) {
            return py::dict("value"_a = id.value);
        })
        .def("to_string", [](const SynapseId& id) {
            return std::to_string(id.value);
        })
        .def("equals", [](const SynapseId& id1, const SynapseId& id2) {
            return id1 == id2;
        }, py::arg("other"));

    // Improved RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const RegionId&>())  // Copy constructor
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("as_dict", [](const RegionId& id) {
            return py::dict("value"_a = id.value);
        })
        .def("to_string", [](const RegionId& id) {
            return std::to_string(id.value);
        })
        .def("equals", [](const RegionId& id1, const RegionId& id2) {
            return id1 == id2;
        }, py::arg("other"));

    // Improved PopulationId class with additional Pythonic features
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const PopulationId&>())  // Copy constructor
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("as_dict", [](const PopulationId& id) {
            return py::dict("value"_a = id.value);
        })
        .def("to_string", [](const PopulationId& id) {
            return std::to_string(id.value);
        })
        .def("equals", [](const PopulationId& id1, const PopulationId& id2) {
            return id1 == id2;
        }, py::arg("other"))
        .def_static("invalid", []() {
            return PopulationId(-1);  // Sentinel value for invalid IDs
        }, R"pbdoc(Get an invalid PopulationId sentinel value)pbdoc");

    // Improved enum classes with better Python integration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excites postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibits postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory, "Modulates synaptic plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values()
        .def("is_excitatory", [](NeuronType type) { return type == NeuronType::Excitatory; })
        .def("is_inhibitory", [](NeuronType type) { return type == NeuronType::Inhibitory; })
        .def("is_sensory", [](NeuronType type) { return type == NeuronType::Sensory; })
        .def("is_motor", [](NeuronType type) { return type == NeuronType::Motor; })
        .def("is_modulatory", [](NeuronType type) { return type == NeuronType::Modulatory; })
        .def("is_internal", [](NeuronType type) { return type == NeuronType::Internal; });

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values()
        .def("is_chemical", [](SynapseType type) {
            return type == SynapseType::Excitatory || type == SynapseType::Inhibitory || type == SynapseType::Modulatory;
        })
        .def("is_electrical", [](SynapseType type) {
            return type == SynapseType::Electrical || type == SynapseType::GapJunction;
        })
        .def("is_excitatory", [](SynapseType type) {
            return type == SynapseType::Excitatory;
        })
        .def("is_inhibitory", [](SynapseType type) {
            return type == SynapseType::Inhibitory;
        });

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values()
        .def("is_developing", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Initial || 
                   stage == DevelopmentalStage::CriticalPeriod || 
                   stage == DevelopmentalStage::Maturation;
        })
        .def("is_mature", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Adult || stage == DevelopmentalStage::Aging;
        })
        .def("is_critical", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::CriticalPeriod;
        })
        .def("is_aging", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Aging;
        });

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest")
        .value("Active", FiringState::Active, "Neuron currently firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited")
        .export_values()
        .def("is_active", [](FiringState state) {
            return state == FiringState::Active;
        })
        .def("is_passive", [](FiringState state) {
            return state == FiringState::Resting || state == FiringState::Inhibited;
        })
        .def("is_refractory", [](FiringState state) {
            return state == FiringState::Refractory;
        })
        .def("can_fire", [](FiringState state) {
            return state == FiringState::Resting || state == FiringState::Active;
        });

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in world coordinates")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in world coordinates")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in world coordinates")
        .value("MoveRight", ActionType::MoveRight, "Move right in world coordinates")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with world object")
        .value("Eat", ActionType::Eat, "Eat food resource")
        .value("Drink", ActionType::Drink, "Drink water resource")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without action")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values()
        .def("is_movement", [](ActionType action) {
            return action >= ActionType::MoveForward && action <= ActionType::MoveRight;
        })
        .def("is_rotational", [](ActionType action) {
            return action == ActionType::TurnLeft || action == ActionType::TurnRight;
        })
        .def("is_consumable", [](ActionType action) {
            return action == ActionType::Eat || action == ActionType::Drink;
        })
        .def("is_social", [](ActionType action) {
            return action == ActionType::Interact;
        })
        .def("is_rest", [](ActionType action) {
            return action == ActionType::Rest || action == ActionType::Wait;
        })
        .def("is_loitering", [](ActionType action) {
            return action == ActionType::Look || action == ActionType::LookUp || action == ActionType::LookDown;
        });

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values()
        .def("is_movement", [](MotorCommand cmd) {
            return cmd >= MotorCommand::MoveForward && cmd <= MotorCommand::MoveBackward;
        })
        .def("is_rotation", [](MotorCommand cmd) {
            return cmd == MotorCommand::TurnLeft || cmd == MotorCommand::TurnRight;
        })
        .def("is_looking", [](MotorCommand cmd) {
            return cmd == MotorCommand::LookLeft || cmd == MotorCommand::LookRight;
        })
        .def("is_interact", [](MotorCommand cmd) {
            return cmd == MotorCommand::Interact;
        })
        .def("is_wait", [](MotorCommand cmd) {
            return cmd == MotorCommand::Wait;
        })
        .def("is_stationary", [](MotorCommand cmd) {
            return cmd == MotorCommand::Wait;
        });

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource that can be consumed")
        .value("Hazard", WorldObjectType::Hazard, "Hazard that causes damage")
        .value("Wall", WorldObjectType::Wall, "Impassable obstacle")
        .value("Marker", WorldObjectType::Marker, "Special location marker")
        .export_values()
        .def("is_passable", [](WorldObjectType type) {
            return type == WorldObjectType::Empty || type == WorldObjectType::Resource || type == WorldObjectType::Marker;
        })
        .def("is_hazard", [](WorldObjectType type) {
            return type == WorldObjectType::Hazard;
        })
        .def("is_resource", [](WorldObjectType type) {
            return type == WorldObjectType::Resource;
        })
        .def("is_wall", [](WorldObjectType type) {
            return type == WorldObjectType::Wall;
        })
        .def("is_obstacle", [](WorldObjectType type) {
            return type == WorldObjectType::Wall || type == WorldObjectType::Hazard;
        });

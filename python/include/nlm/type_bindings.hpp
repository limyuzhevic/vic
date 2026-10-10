// NLM Python Bindings - Type Definitions and Enums
// NeuronId, SynapseId, RegionId, PopulationId, and all type enums

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "exceptions.hpp"
#include "validation.hpp"

namespace py = pybind11;
namespace nlm {

void bind_types(py::module_& m) {
    // NeuronId binding with improved error handling
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(Create a NeuronId with a value)
        
        Args:
            value: Integer value for the neuron ID
        
        Raises:
            ValidationError: If value is negative
        )pbdoc")
        .def_readwrite("value", &NeuronId::value, R"pbdoc(Neuron ID value)pbdoc")
        .def("index", &NeuronId::index, R"pbdoc(Get the neuron index)
        
        Returns:
            Neuron index
        )pbdoc")
        .def("__eq__", &NeuronId::operator==, R"pbdoc(Check if two NeuronIds are equal)
        
        Args:
            other: Another NeuronId to compare with
        
        Returns:
            True if equal, False otherwise
        )pbdoc")
        .def("__ne__", &NeuronId::operator!=, R"pbdoc(Check if two NeuronIds are not equal)
        
        Args:
            other: Another NeuronId to compare with
        
        Returns:
            True if not equal, False otherwise
        )pbdoc")
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, R"pbdoc(Get hash of NeuronId for use as dictionary key)
        
        Returns:
            Hash value
        )pbdoc")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(String representation of NeuronId)
        
        Returns:
            String representation
        )pbdoc")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId binding with improved error handling
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(Create a SynapseId with a value)
        
        Args:
            value: Integer value for the synapse ID
        
        Raises:
            ValidationError: If value is negative
        )pbdoc")
        .def_readwrite("value", &SynapseId::value, R"pbdoc(Synapse ID value)pbdoc")
        .def("index", &SynapseId::index, R"pbdoc(Get the synapse index)
        
        Returns:
            Synapse index
        )pbdoc")
        .def("__eq__", &SynapseId::operator==, R"pbdoc(Check if two SynapseIds are equal)
        
        Args:
            other: Another SynapseId to compare with
        
        Returns:
            True if equal, False otherwise
        )pbdoc")
        .def("__ne__", &SynapseId::operator!=, R"pbdoc(Check if two SynapseIds are not equal)
        
        Args:
            other: Another SynapseId to compare with
        
        Returns:
            True if not equal, False otherwise
        )pbdoc")
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, R"pbdoc(Get hash of SynapseId for use as dictionary key)
        
        Returns:
            Hash value
        )pbdoc")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(String representation of SynapseId)
        
        Returns:
            String representation
        )pbdoc")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId binding with improved error handling
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(Create a RegionId with a value)
        
        Args:
            value: Integer value for the region ID
        
        Raises:
            ValidationError: If value is negative
        )pbdoc")
        .def_readwrite("value", &RegionId::value, R"pbdoc(Region ID value)pbdoc")
        .def("index", &RegionId::index, R"pbdoc(Get the region index)
        
        Returns:
            Region index
        )pbdoc")
        .def("__eq__", &RegionId::operator==, R"pbdoc(Check if two RegionIds are equal)
        
        Args:
            other: Another RegionId to compare with
        
        Returns:
            True if equal, False otherwise
        )pbdoc")
        .def("__ne__", &RegionId::operator!=, R"pbdoc(Check if two RegionIds are not equal)
        
        Args:
            other: Another RegionId to compare with
        
        Returns:
            True if not equal, False otherwise
        )pbdoc")
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, R"pbdoc(Get hash of RegionId for use as dictionary key)
        
        Returns:
            Hash value
        )pbdoc")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(String representation of RegionId)
        
        Returns:
            String representation
        )pbdoc")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId binding with improved error handling
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(Create a PopulationId with a value)
        
        Args:
            value: Integer value for the population ID
        
        Raises:
            ValidationError: If value is negative
        )pbdoc")
        .def_readwrite("value", &PopulationId::value, R"pbdoc(Population ID value)pbdoc")
        .def("index", &PopulationId::index, R"pbdoc(Get the population index)
        
        Returns:
            Population index
        )pbdoc")
        .def("__eq__", &PopulationId::operator==, R"pbdoc(Check if two PopulationIds are equal)
        
        Args:
            other: Another PopulationId to compare with
        
        Returns:
            True if equal, False otherwise
        )pbdoc")
        .def("__ne__", &PopulationId::operator!=, R"pbdoc(Check if two PopulationIds are not equal)
        
        Args:
            other: Another PopulationId to compare with
        
        Returns:
            True if not equal, False otherwise
        )pbdoc")
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(String representation of PopulationId)
        
        Returns:
            String representation
        )pbdoc")
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // NeuronType enum with better documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)
    
    Enumerates the different types of neurons in the NLM system.
    "pbdoc")
        .value("Excitatory", NeuronType::Excitatory, R"pbdoc(Excitatory neuron that increases postsynaptic firing)
        
        Most common neuron type, typically generates action potentials that
        increase the likelihood of postsynaptic neuron firing.
        "pbdoc")
        .value("Inhibitory", NeuronType::Inhibitory, R"pbdoc(Inhibitory neuron that decreases postsynaptic firing)
        
        Neuron type that reduces the likelihood of postsynaptic neuron firing,
        maintaining balance in neural networks.
        "pbdoc")
        .value("Modulatory", NeuronType::Modulatory, R"pbdoc(Modulatory neuron that affects neuron excitability)
        
        Neuron that influences other neurons' excitability and plasticity
        without directly transmitting action potentials.
        "pbdoc")
        .value("Sensory", NeuronType::Sensory, R"pbdoc(Sensory neuron that receives external input)
        
        Neuron type that responds to external stimuli and sensory information.
        "pbdoc")
        .value("Motor", NeuronType::Motor, R"pbdoc(Motor neuron that produces action outputs)
        
        Neuron type that generates motor commands for interaction with the world.
        "pbdoc")
        .value("Internal", NeuronType::Internal, R"pbdoc(Internal neuron for internal processing)
        
        Neuron type used for internal system processes and computations.
        "pbdoc")
        .export_values();

    // SynapseType enum with better documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)
    
    Enumerates the different types of synapses in the NLM system.
    "pbdoc")
        .value("Excitatory", SynapseType::Excitatory, R"pbdoc(Excitatory synapse that increases postsynaptic firing)
        
        Most common synapse type, typically releases neurotransmitters
        that excite the postsynaptic neuron.
        "pbdoc")
        .value("Inhibitory", SynapseType::Inhibitory, R"pbdoc(Inhibitory synapse that decreases postsynaptic firing)
        
        Synapse type that releases neurotransmitters that inhibit
        the postsynaptic neuron.
        "pbdoc")
        .value("Modulatory", SynapseType::Modulatory, R"pbdoc(Modulatory synapse that affects neuron excitability)
        
        Synapse that releases neuromodulators affecting neuron
        excitability and plasticity over longer timescales.
        "pbdoc")
        .value("Electrical", SynapseType::Electrical, R"pbdoc(Electrical synapse that allows direct current flow)
        
        Synapse type that allows direct electrical coupling between neurons
        through gap junctions, enabling faster communication.
        "pbdoc")
        .value("GapJunction", SynapseType::GapJunction, R"pbdoc(Gap junction synapse for direct electrical coupling)
        
        Specialized electrical synapse that allows bidirectional
        electrical transmission between neurons.
        "pbdoc")
        .export_values();

    // DevelopmentalStage enum with better documentation
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)
    
    Enumerates the different developmental stages of the brain system.
    "pbdoc")
        .value("Initial", DevelopmentalStage::Initial, R"pbdoc(Early developmental stage with high plasticity)
        
        Initial stage where the brain has high plasticity and is rapidly
        developing basic neural connections and capabilities.
        "pbdoc")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, R"pbdoc(Critical period for sensory development)
        
        Period where specific sensory systems are especially sensitive
        to environmental input and can undergo significant reorganization.
        "pbdoc")
        .value("Maturation", DevelopmentalStage::Maturation, R"pbdoc(Mid developmental stage with balanced plasticity)
        
        Stage where basic neural systems are maturing and fine-tuning
        their connections for optimal function.
        "pbdoc")
        .value("Adult", DevelopmentalStage::Adult, R"pbdoc(Adult developmental stage with reduced plasticity)
        
        Mature stage where the brain has reached adult capabilities
        and plasticity is reduced for stability.
        "pbdoc")
        .value("Aging", DevelopmentalStage::Aging, R"pbdoc(Aging stage with declining plasticity)
        
        Late stage where neural systems begin to decline in function
        due to aging and reduced plasticity.
        "pbdoc")
        .export_values();

    // FiringState enum with better documentation
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)
    
    Enumerates the different firing states of neurons.
    "pbdoc")
        .value("Resting", FiringState::Resting, R"pbdoc(Neuron at rest, not firing)
        
        Neuron is at rest, not currently generating action potentials.
        "pbdoc")
        .value("Active", FiringState::Active, R"pbdoc(Neuron actively firing action potentials)
        
        Neuron is currently generating action potentials and transmitting signals.
        "pbdoc")
        .value("Refractory", FiringState::Refractory, R"pbdoc(Neuron in refractory period after firing)
        
        Neuron cannot fire immediately after an action potential,
        ensuring unidirectional propagation.
        "pbdoc")
        .value("Inhibited", FiringState::Inhibited, R"pbdoc(Neuron actively inhibited and not firing)
        
        Neuron is actively inhibited and unable to fire, even if
        it receives excitatory input.
        "pbdoc")
        .export_values();

    // ActionType enum with better documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)
    
    Enumerates the different types of actions that can be performed.
    "pbdoc")
        .value("MoveForward", ActionType::MoveForward, R"pbdoc(Move forward in the world)
        
        Action to move the agent forward by one unit.
        "pbdoc")
        .value("MoveBackward", ActionType::MoveBackward, R"pbdoc(Move backward in the world)
        
        Action to move the agent backward by one unit.
        "pbdoc")
        .value("MoveLeft", ActionType::MoveLeft, R"pbdoc(Move left in the world)
        
        Action to move the agent left by one unit.
        "pbdoc")
        .value("MoveRight", ActionType::MoveRight, R"pbdoc(Move right in the world)
        
        Action to move the agent right by one unit.
        "pbdoc")
        .value("TurnLeft", ActionType::TurnLeft, R"pbdoc(Turn left)
        
        Action to turn the agent left by 90 degrees.
        "pbdoc")
        .value("TurnRight", ActionType::TurnRight, R"pbdoc(Turn right)
        
        Action to turn the agent right by 90 degrees.
        "pbdoc")
        .value("Look", ActionType::Look, R"pbdoc(Look in current direction)
        
        Action to look in the current facing direction.
        "pbdoc")
        .value("LookUp", ActionType::LookUp, R"pbdoc(Look up)
        
        Action to look upward.
        "pbdoc")
        .value("LookDown", ActionType::LookDown, R"pbdoc(Look down)
        
        Action to look downward.
        "pbdoc")
        .value("Interact", ActionType::Interact, R"pbdoc(Interact with nearby object)
        
        Action to interact with an object in the current location.
        "pbdoc")
        .value("Eat", ActionType::Eat, R"pbdoc(Eat food or item)
        
        Action to consume food or an edible item.
        "pbdoc")
        .value("Drink", ActionType::Drink, R"pbdoc(Drink water or beverage)
        
        Action to drink water or a beverage.
        "pbdoc")
        .value("Rest", ActionType::Rest, R"pbdoc(Rest or stop moving)
        
        Action to rest or stop moving and consuming energy.
        "pbdoc")
        .value("Wait", ActionType::Wait, R"pbdoc(Wait for a time step)
        
        Action to wait without taking any other action.
        "pbdoc")
        .value("Custom", ActionType::Custom, R"pbdoc(Custom action)
        
        Action type for custom-defined behaviors.
        "pbdoc")
        .export_values();

    // MotorCommand enum with better documentation
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)
    
    Enumerates low-level motor commands that can be executed.
    "pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, R"pbdoc(Move forward)
        
        Low-level command to move forward.
        "pbdoc")
        .value("MoveBackward", MotorCommand::MoveBackward, R"pbdoc(Move backward)
        
        Low-level command to move backward.
        "pbdoc")
        .value("TurnLeft", MotorCommand::TurnLeft, R"pbdoc(Turn left)
        
        Low-level command to turn left.
        "pbdoc")
        .value("TurnRight", MotorCommand::TurnRight, R"pbdoc(Turn right)
        
        Low-level command to turn right.
        "pbdoc")
        .value("LookLeft", MotorCommand::LookLeft, R"pbdoc(Look left)
        
        Low-level command to look left.
        "pbdoc")
        .value("LookRight", MotorCommand::LookRight, R"pbdoc(Look right)
        
        Low-level command to look right.
        "pbdoc")
        .value("Interact", MotorCommand::Interact, R"pbdoc(Interact with object)
        
        Low-level command to interact with an object.
        "pbdoc")
        .value("Wait", MotorCommand::Wait, R"pbdoc(Wait without action)
        
        Low-level command to wait without taking action.
        "pbdoc")
        .export_values();

    // WorldObjectType enum with better documentation
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)
    
    Enumerates the different types of objects that can exist in the world.
    "pbdoc")
        .value("Empty", WorldObjectType::Empty, R"pbdoc(Empty space with no object)
        
        World location that is empty.
        "pbdoc")
        .value("Resource", WorldObjectType::Resource, R"pbdoc(Resource that can be collected)
        
        Object that can be collected by the agent for consumption.
        "pbdoc")
        .value("Hazard", WorldObjectType::Hazard, R"pbdoc(Dangerous object that damages the agent)
        
        Object that causes damage to the agent if interacted with.
        "pbdoc")
        .value("Wall", WorldObjectType::Wall, R"pbdoc(Obstruction that cannot be passed)
        
        Solid object that blocks agent movement.
        "pbdoc")
        .value("Marker", WorldObjectType::Marker, R"pbdoc(Markable location or object)
        
        Object or location that can be marked for reference.
        "pbdoc")
        .export_values();

    // Expose invalid ID sentinels as Python constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

} // namespace nlm

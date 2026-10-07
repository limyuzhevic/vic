#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

// enums.cpp - NeuronType, SynapseType, DevelopmentalStage, FiringState, ActionType, MotorCommand, WorldObjectType
// Purpose: Defines Python bindings for enumeration types used throughout the NLM system

// Register enumeration type bindings to the given module
void register_enums(pybind11::module_& m) {

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of different neuron types in the NLM system.
        
        NeuronType defines the functional classification of neurons based on their role
        in neural processing. Each type has specific characteristics and typically
        exhibits different firing patterns and connection rules.
        
        Examples:
            >>> neuron_type = nlm.NeuronType.Excitatory
            >>> neuron_type.name
            'NeuronType.Excitatory'
            >>> str(neuron_type)
            'Excitatory'
            >>> neuron_type == nlm.NeuronType.Excitatory
            True
            >>> neuron_type.value
            0
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of different synapse types in the NLM system.
        
        SynapseType defines the functional classification of synaptic connections based on
        their transmission characteristics and effects. Each type has distinct properties
        for neural dynamics and plasticity.
        
        Examples:
            >>> syn_type = nlm.SynapseType.Excitatory
            >>> syn_type.name
            'SynapseType.Excitatory'
            >>> str(syn_type)
            'Excitatory'
            >>> syn_type == nlm.SynapseType.Excitatory
            True
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages for neural growth and maturation.
        
        DevelopmentalStage represents the temporal progression of neural development,
        from initial formation through critical learning periods to mature functioning.
        Each stage has characteristic plasticity rates and learning capabilities.
        
        Examples:
            >>> stage = nlm.DevelopmentalStage.CriticalPeriod
            >>> str(stage)
            'CriticalPeriod'
            >>> stage == nlm.DevelopmentalStage.Maturation
            False
            >>> brain.setDevelopmentalStage(nlm.DevelopmentalStage.Adult)
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states in the NLM system.
        
        FiringState represents the current activity state of a neuron, reflecting its
        readiness to transmit signals. These states are crucial for understanding neural
        dynamics and network behavior.
        
        Examples:
            >>> state = nlm.FiringState.Active
            >>> str(state)
            'Active'
            >>> state == nlm.FiringState.Resting
            False
            >>> neuron_state = nlm.FiringState.Refractory
        )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of different action types for motor output in the NLM system.
        
        ActionType defines the possible high-level actions that the brain can generate
        to interact with the environment. These represent behavioral responses and
        movements that the agent can execute.
        
        Examples:
            >>> action = nlm.ActionType.MoveForward
            >>> str(action)
            'MoveForward'
            >>> action == nlm.ActionType.TurnLeft
            False
            >>> ActionType.Custom
            <ActionType.Custom: 14>
        )pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor command types in the NLM system.
        
        MotorCommand defines the fundamental motor outputs that can be generated
        by the brain's motor system. These are the basic commands that directly
        control the agent's movement and interaction with the environment.
        
        Examples:
            >>> cmd = nlm.MotorCommand.MoveForward
            >>> str(cmd)
            'MoveForward'
            >>> cmd == nlm.MotorCommand.TurnRight
            False
            >>> MotorCommand.LookLeft
            <MotorCommand.LookLeft: 5>
        )pbdoc")
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
        Enumeration of different object types in the simulation world.
        
        WorldObjectType defines the classification of objects that can exist in the
        simulation environment. Each type has specific properties and interaction rules
        that determine how the brain should respond to them.
        
        Examples:
            >>> obj_type = nlm.WorldObjectType.Resource
            >>> str(obj_type)
            'Resource'
            >>> obj_type == nlm.WorldObjectType.Wall
            False
            >>> WorldObjectType.Hazard
            <WorldObjectType.Hazard: 3>
        )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
}

} // namespace nlm
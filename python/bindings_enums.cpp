// Enumerations for NLM Python bindings

#include "bindings.h"

namespace nlm {

void bind_enums(py::module_& m) {
    // Neuron types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration.
        
        Defines different kinds of neurons in the NLM brain:
        - Excitatory: Neurons that increase the likelihood of postsynaptic firing
        - Inhibitory: Neurons that decrease the likelihood of postsynaptic firing
        - Modulatory: Neurons that release neuromodulators affecting plasticity
        - Sensory: Neurons that receive and process sensory input
        - Motor: Neurons that control action execution
        - Internal: General-purpose neurons for internal processing
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse types
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration.
        
        Defines different kinds of synaptic connections:
        - Excitatory: Increases postsynaptic neuron activity
        - Inhibitory: Decreases postsynaptic neuron activity
        - Modulatory: Releases neuromodulators affecting plasticity
        - Electrical: Direct electrical coupling via gap junctions
        - GapJunction: Specific type of electrical synapse
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration.
        
        Defines the stages of brain development:
        - Initial: Embryonic stage, basic neural circuits forming
        - CriticalPeriod: Window of high plasticity for learning
        - Maturation: Refinement and specialization of circuits
        - Adult: Stable, mature neural network
        - Aging: Age-related decline in plasticity and function
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing states
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration.
        
        Defines the current state of a neuron:
        - Resting: Not currently active
        - Active: Currently firing action potentials
        - Refractory: Cannot fire due to recovery period
        - Inhibited: Cannot fire due to strong inhibition
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Action types
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration.
        
        Defines possible actions that an agent can take:
        - Movement actions: MoveForward, MoveBackward, MoveLeft, MoveRight
        - Turning actions: TurnLeft, TurnRight
        - Gaze actions: Look, LookUp, LookDown
        - Interaction actions: Interact, Eat, Drink
        - Behavioral actions: Rest, Wait, Custom
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

    // Motor commands
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration.
        
        Defines motor system commands:
        - Basic movements: MoveForward, MoveBackward, TurnLeft, TurnRight
        - Gaze control: LookLeft, LookRight
        - Interaction: Interact
        - Default: Wait (no action)
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

    // World object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration.
        
        Defines types of objects in the simulated world:
        - Empty: No object at this position
        - Resource: A consumable resource
        - Hazard: Dangerous object to avoid
        - Wall: Obstacle that cannot be passed
        - Marker: Visual marker or landmark
    )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Simulation step type
    py::enum_<SimulationStep>(m, "SimulationStep", R"pbdoc(
        Simulation step type enumeration.
        
        Defines types of simulation steps:
        - Discrete: Fixed time step simulation
        - Continuous: Real-time simulation with variable timing
        - EventDriven: Step only when significant events occur
    )pbdoc")
        .value("Discrete", SimulationStep::Discrete)
        .value("Continuous", SimulationStep::Continuous)
        .value("EventDriven", SimulationStep::EventDriven)
        .export_values();
}

} // namespace nlm
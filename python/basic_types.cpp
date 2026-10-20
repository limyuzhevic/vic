#include "basic_types.h"
#include <pybind11/functional.h>

namespace py = pybind11;
namespace nlm {

void include_basic_types(py::module_& m) {
    // NeuronId class
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId class
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId class
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)
    
    Defines the different types of neurons in the neural network.
    
    Attributes:
        Excitatory: Neurons that increase the firing probability of other neurons
        Inhibitory: Neurons that decrease the firing probability of other neurons
        Modulatory: Neurons that have broad, diffuse effects on neural activity
        Sensory: Neurons that receive external sensory input
        Motor: Neurons that produce motor output signals
        Internal: Neurons that represent internal states and cognitive processes
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)
    
    Defines the different types of synaptic connections between neurons.
    
    Attributes:
        Excitatory: Synapses that increase postsynaptic neuron firing probability
        Inhibitory: Synapses that decrease postsynaptic neuron firing probability
        Modulatory: Synapses with broad, diffuse effects
        Electrical: Direct electrical connections between neurons
        GapJunction: Electrical synapses via gap junctions
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)
    
    Represents the developmental stage of the neural network.
    
    Attributes:
        Initial: Initial state after network creation
        CriticalPeriod: Period of heightened plasticity and learning
        Maturation: Period of neural maturation and refinement
        Adult: Fully mature, stable neural network
        Aging: Age-related changes and decline
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)
    
    Represents the current firing state of a neuron.
    
    Attributes:
        Resting: Neuron is not firing
        Active: Neuron is currently firing
        Refractory: Neuron is in refractory period
        Inhibited: Neuron is inhibited from firing
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)
    
    Defines the possible action types for motor output.
    
    Attributes:
        MoveForward: Move the agent forward
        MoveBackward: Move the agent backward
        MoveLeft: Move the agent left
        MoveRight: Move the agent right
        TurnLeft: Rotate the agent left
        TurnRight: Rotate the agent right
        Look: Look in current direction
        LookUp: Look up
        LookDown: Look down
        Interact: Interact with world object
        Eat: Consume food/resource
        Drink: Drink water/resource
        Rest: Stop and rest
        Wait: Do nothing for one timestep
        Custom: Custom user-defined action
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

    // MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)
    
    Defines the low-level motor commands for precise control.
    
    Attributes:
        MoveForward: Move forward by one unit
        MoveBackward: Move backward by one unit
        TurnLeft: Rotate left by one unit
        TurnRight: Rotate right by one unit
        LookLeft: Look left
        LookRight: Look right
        Interact: Interact with object
        Wait: Do nothing
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)
    
    Defines the types of objects that can exist in the world.
    
    Attributes:
        Empty: Empty space
        Resource: Food or consumable resource
        Hazard: Dangerous object
        Wall: Impenetrable barrier
        Marker: Reference or landmark object
    )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Register INVALID_* constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
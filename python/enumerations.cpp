// Enumerations Bindings
// Handles all enumeration types with comprehensive documentation

#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace nlm {

void bindEnumerations(py::module_& m) {
    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration.
        
        Defines the different types of neurons in the NLM system:
        - Excitatory: Neurons that release excitatory neurotransmitters
        - Inhibitory: Neurons that release inhibitory neurotransmitters  
        - Modulatory: Neurons that release neuromodulators
        - Sensory: Neurons that receive sensory input
        - Motor: Neurons that control motor output
        - Internal: Internal processing neurons
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, R"pbdoc(Excitatory neuron type)pbdoc")
        .value("Inhibitory", NeuronType::Inhibitory, R"pbdoc(Inhibitory neuron type)pbdoc")
        .value("Modulatory", NeuronType::Modulatory, R"pbdoc(Modulatory neuron type)pbdoc")
        .value("Sensory", NeuronType::Sensory, R"pbdoc(Sensory neuron type)pbdoc")
        .value("Motor", NeuronType::Motor, R"pbdoc(Motor neuron type)pbdoc")
        .value("Internal", NeuronType::Internal, R"pbdoc(Internal neuron type)pbdoc")
        .export_values();
    
    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration.
        
        Defines the different types of synaptic connections in the NLM system:
        - Excitatory: Standard excitatory synapses
        - Inhibitory: Standard inhibitory synapses
        - Modulatory: Synaptic connections that release neuromodulators
        - Electrical: Electrical synapses (gap junctions)
        - GapJunction: Gap junction electrical synapses
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, R"pbdoc(Excitatory synapse type)pbdoc")
        .value("Inhibitory", SynapseType::Inhibitory, R"pbdoc(Inhibitory synapse type)pbdoc")
        .value("Modulatory", SynapseType::Modulatory, R"pbdoc(Modulatory synapse type)pbdoc")
        .value("Electrical", SynapseType::Electrical, R"pbdoc(Electrical synapse type)pbdoc")
        .value("GapJunction", SynapseType::GapJunction, R"pbdoc(Gap junction synapse type)pbdoc")
        .export_values();
    
    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration.
        
        Represents the developmental stages of the NLM brain:
        - Initial: Initial developmental stage
        - CriticalPeriod: Critical period for learning and development
        - Maturation: Maturation stage
        - Adult: Adult stage
        - Aging: Aging stage
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, R"pbdoc(Initial developmental stage)pbdoc")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, R"pbdoc(Critical period for development)pbdoc")
        .value("Maturation", DevelopmentalStage::Maturation, R"pbdoc(Maturation developmental stage)pbdoc")
        .value("Adult", DevelopmentalStage::Adult, R"pbdoc(Adult developmental stage)pbdoc")
        .value("Aging", DevelopmentalStage::Aging, R"pbdoc(Aging developmental stage)pbdoc")
        .export_values();
}

} // namespace nlm

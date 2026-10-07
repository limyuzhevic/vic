// Plasticity system bindings

#include "bindings.h"

namespace nlm {

void bind_plasticity_module(py::module_& m) {
    // PlasticityRule base class
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(
        Abstract base class for synaptic plasticity rules.
        
        Plasticity rules provide:
        - Synaptic weight modification based on neural activity
        - Learning mechanisms for neural networks
        - Activity-dependent changes in connection strength
        - Implementation of various learning algorithms
        
        Key features:
        - Apply weight changes based on spike timing
        - Implement different learning rules
        - Support for reward-modulated plasticity
        - Structural plasticity for connection formation
    )pbdoc")
        .def("update", &PlasticityRule::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update plasticity rule with spike information")
        .def("applyWeightChange", &PlasticityRule::applyWeightChange,
             py::arg("synapse"), py::arg("delta"), "Apply synaptic weight change")
        .def("getName", &PlasticityRule::getName, "Get plasticity rule name")
        .def("configure", &PlasticityRule::configure, "Configure plasticity rule parameters");

    // STDP class
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(
        Spike-Timing-Dependent Plasticity (STDP).
        
        STDP provides:
        - Temporal-based synaptic modification
        - Asymmetric learning windows
        - LTP and LTD mechanisms
        - Spike-timing dependent learning
        
        Key features:
        - Strengthen synapses for causal spikes
        - Weaken synapses for anti-causal spikes
        - Fine-tunable time constants
        - Configurable learning weights
    )pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), "Set long-term potentiation weight")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get long-term potentiation weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), "Set long-term depression weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get long-term depression weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"), "Set STDP time constant (ms)")
        .def("getTimeConstant", &STDP::getTimeConstant, "Get STDP time constant (ms)")
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP parameters in one call");

    // Hebbian class
    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(
        Hebbian plasticity (sum-of-squares learning).
        
        Hebbian provides:
        - Correlation-based learning
        - "Neurons that fire together, wire together"
        - Symmetric weight changes
        - Pattern-based learning
        
        Key features:
        - Strengthen co-active synapses
        - Simple and biologically plausible
        - Fast learning for pattern association
        - Good for memory formation
    )pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"), "Set Hebbian learning rate")
        .def("getLearningRate", &Hebbian::getLearningRate, "Get Hebbian learning rate")
        .def("setDecayRate", &Hebbian::setDecayRate, py::arg("rate"), "Set weight decay rate")
        .def("getDecayRate", &Hebbian::getDecayRate, "Get weight decay rate");

    // StructuralPlasticity class
    py::class_<StructuralPlasticity, PlasticityRule>(m, "StructuralPlasticity", R"pbdoc(
        Structural plasticity for connection formation and elimination.
        
        Structural plasticity provides:
        - Formation of new synaptic connections
        - Elimination of existing connections
        - Network rewiring
        - Dynamic connectivity changes
        
        Key features:
        - Add/remove synapses
        - Modify connection strength
        - Support for structural learning
        - Network topology changes
    )pbdoc")
        .def(py::init<>())
        .def("addSynapse", &StructuralPlasticity::addSynapse,
             py::arg("preNeuron"), py::arg("postNeuron"), py::arg("weight") = 1.0f,
             "Add a new synapse between neurons")
        .def("removeSynapse", &StructuralPlasticity::removeSynapse,
             py::arg("preNeuron"), py::arg("postNeuron"), "Remove synapse between neurons")
        .def("getConnectionCount", &StructuralPlasticity::getConnectionCount, "Get total connection count")
        .def("getConnectionDensity", &StructuralPlasticity::getConnectionDensity, "Get connection density")
        .def("enableGrowth", &StructuralPlasticity::enableGrowth, py::arg("enable"), "Enable connection growth")
        .def("enablePruning", &StructuralPlasticity::enablePruning, py::arg("enable"), "Enable connection pruning");
}

} // namespace nlm
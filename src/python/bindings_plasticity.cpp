// Plasticity Systems Module for NLM Python Bindings
// This file contains Python bindings for plasticity-related systems

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"

namespace py = pybind11;
namespace nlm {

void bind_plasticity_systems(py::module_ &m) {
    // === PLASTICITY RULE BASE CLASS ===
    py::class_<PlasticityRule>(m, "PlasticityRule", "Base class for plasticity rules")
        .def(py::init<>())
        .def("update", &PlasticityRule::update,
            py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("timestep"),
            "Update synapse weights based on spike history")
        .def("setLearningRate", &PlasticityRule::setLearningRate,
            py::arg("rate"), "Set learning rate")
        .def("getLearningRate", &PlasticityRule::getLearningRate,
            "Get current learning rate")
        .def("reset", &PlasticityRule::reset,
            "Reset plasticity rule state");
    
    // === SPIKE-TIMED DEPENDENT PLASTICITY (STDP) ===
    py::class_<STDP, PlasticityRule>(m, "STDP", "Spike-Timing-Dependent Plasticity")
        .def(py::init<>())
        .def("configure", &STDP::configure,
            py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
            "Configure STDP parameters")
        .def("getLTPWeight", &STDP::getLTPWeight,
            "Get Long-Term Potentiation weight")
        .def("getLTDWeight", &STDP::getLTDWeight,
            "Get Long-Term Depression weight")
        .def("getTau", &STDP::getTau,
            "Get time constant")
        .def("setLTPWeight", &STDP::setLTPWeight,
            py::arg("weight"), "Set LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight,
            py::arg("weight"), "Set LTD weight")
        .def("setTau", &STDP::setTau,
            py::arg("tau"), "Set time constant");
    
    // === HEBBIAN PLASTICITY ===
    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", "Hebbian learning plasticity")
        .def(py::init<>())
        .def("configure", &Hebbian::configure,
            py::arg("learningRate"), py::arg("cooperativity"),
            "Configure Hebbian parameters")
        .def("getLearningRate", &Hebbian::getLearningRate,
            "Get learning rate")
        .def("getCooperativity", &Hebbian::getCooperativity,
            "Get cooperativity factor")
        .def("setLearningRate", &Hebbian::setLearningRate,
            py::arg("rate"), "Set learning rate")
        .def("setCooperativity", &Hebbian::setCooperativity,
            py::arg("cooperativity"), "Set cooperativity factor");
    
    // === STRUCTURAL PLASTICITY ===
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity",
        "Structural plasticity with synaptogenesis and pruning")
        .def(py::init<>())
        .def("update", &StructuralPlasticity::update,
            py::arg("brain"), py::arg("rng"),
            "Update structural connectivity")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate,
            py::arg("rate"), "Set synaptogenesis rate")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate,
            py::arg("rate"), "Set pruning rate")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate,
            "Get synaptogenesis rate")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate,
            "Get pruning rate")
        .def("createNewSynapse", &StructuralPlasticity::createNewSynapse,
            py::arg("sourceNeuron"), py::arg("targetNeuron"),
            "Create new synaptic connection")
        .def("removeSynapse", &StructuralPlasticity::removeSynapse,
            py::arg("synapseId"), "Remove synaptic connection")
        .def("getSynapseCount", &StructuralPlasticity::getSynapseCount,
            "Get current number of synapses")
        .def("getConnectionDensity", &StructuralPlasticity::getConnectionDensity,
            "Get connection density")
        .def("updateDevelopmentStage", &StructuralPlasticity::updateDevelopmentStage,
            py::arg("stage"), "Update based on developmental stage");
    
    // === PLASTICITY FLAGS ===
    py::enum_<PlasticityFlags>(m, "PlasticityFlags", "Flags for synapse plasticity")
        .value("STDP", PlasticityFlags::stdp)
        .value("HEBBIAN", PlasticityFlags::hebbian)
        .value("STRUCTURAL", PlasticityFlags::structural)
        .export_values();
    
    // === SYNAPSE PLASTICITY STATE ===
    py::class_<SynapsePlasticityState>(m, "SynapsePlasticityState",
        "Plasticity state of a synapse")
        .def(py::init<>())
        .def_readwrite("eligibilityTrace", &SynapsePlasticityState::eligibilityTrace)
        .def_readwrite("learningRate", &SynapsePlasticityState::learningRate)
        .def_readwrite("lastUpdateTime", &SynapsePlasticityState::lastUpdateTime)
        .def("reset", &SynapsePlasticityState::reset,
            "Reset plasticity state");
}

// === FACTORY FUNCTIONS ===

void bind_plasticity_factories(py::module_ &m) {
    m.def("createSTDP", []() {
        return std::make_unique<STDP>();
    }, "Create an STDP plasticity rule instance");
    
    m.def("createHebbian", []() {
        return std::make_unique<Hebbian>();
    }, "Create a Hebbian plasticity rule instance");
    
    m.def("createStructuralPlasticity", []() {
        return std::make_unique<StructuralPlasticity>();
    }, "Create a structural plasticity instance");
    
    m.def("createDefaultPlasticitySystem", []() {
        auto stdp = std::make_unique<STDP>();
        auto hebbian = std::make_unique<Hebbian>();
        auto structural = std::make_unique<StructuralPlasticity>();
        
        // Configure with default values
        stdp->configure(0.01f, 0.012f, 20.0f);
        hebbian->configure(0.01f, 1.0f);
        structural->setSynaptogenesisRate(0.0001f);
        structural->setPruningRate(0.00001f);
        
        return std::tuple<std::unique_ptr<STDP>,
                         std::unique_ptr<Hebbian>,
                         std::unique_ptr<StructuralPlasticity>>(
            std::move(stdp),
            std::move(hebbian),
            std::move(structural)
        );
    }, "Create a complete default plasticity system");
}

} // namespace nlm
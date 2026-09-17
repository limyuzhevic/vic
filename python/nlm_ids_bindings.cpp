// Template-based bindings for NLM ID types
// This file provides a modular approach to binding NeuronId, SynapseId, RegionId, and PopulationId

#include <pybind11/pybind11.h>
#include "../../include/nlm_ids.hpp"

// Template function to bind a generic ID type
template<typename T, typename Tag>
void bindId(pybind11::class_<T>& cls) {
    cls
        .def(pybind11::init<>())
        .def(pybind11::init<typename T::value_type>(), pybind11::arg("value"))
        .def_readwrite("value", &T::value)
        .def("index", &T::index)
        .def("__eq__", &T::operator==)
        .def("__ne__", &T::operator!=)
        .def("__hash__", [](const T& id) {
            return T::Hash{}(id);
        })
        .def("__repr__", [](const T& id) {
            return id.repr();
        });
}

// Specialization for PopulationId (no __hash__ and __repr__ methods)
template<typename Tag>
void bindId(pybind11::class_<PopulationId, Tag>& cls) {
    cls
        .def(pybind11::init<>())
        .def(pybind11::init<typename PopulationId::value_type>(), pybind11::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
}

// Function to bind all ID types to the Python module
void bindAllIds(pybind11::module_& m) {
    // Bind NeuronId
    bindId<NeuronId, NeuronIdTag>(pybind11::class_<NeuronId>(m, "NeuronId", pybind11::doc("Unique identifier for a neuron")));
    
    // Bind SynapseId
    bindId<SynapseId, SynapseIdTag>(pybind11::class_<SynapseId>(m, "SynapseId", pybind11::doc("Unique identifier for a synapse")));
    
    // Bind RegionId
    bindId<RegionId, RegionIdTag>(pybind11::class_<RegionId>(m, "RegionId", pybind11::doc("Unique identifier for a brain region")));
    
    // Bind PopulationId (uses specialization that doesn't include __hash__ and __repr__)
    bindId<PopulationId, PopulationIdTag>(pybind11::class_<PopulationId>(m, "PopulationId", pybind11::doc("Unique identifier for a neuron population")));
}

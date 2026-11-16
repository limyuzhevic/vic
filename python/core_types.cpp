// Core Types Bindings
// Handles ID classes (NeuronId, SynapseId, RegionId, PopulationId) using template

#include <pybind11/pybind11.h>
#include "NLMBindingsIdTemplate.hpp"

namespace py = pybind11;
namespace nlm {

void bindCoreTypes(py::module_& m) {
    // Register ID classes using template to eliminate duplication
    registerIdClass<NeuronId, struct NeuronTag>(m, "NeuronId")
        .doc() = R"pbdoc(Unique identifier for a neuron)pbdoc";
    
    registerIdClass<SynapseId, struct SynapseTag>(m, "SynapseId")
        .doc() = R"pbdoc(Unique identifier for a synapse)pbdoc";
    
    registerIdClass<RegionId, struct RegionTag>(m, "RegionId")
        .doc() = R"pbdoc(Unique identifier for a brain region)pbdoc";
    
    // PopulationId doesn't have hash and repr (legacy behavior)
    m.def_class_<PopulationId>("PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
}

} // namespace nlm

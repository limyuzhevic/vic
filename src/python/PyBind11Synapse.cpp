// Python bindings for Synapse
#include "brain/Synapse.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(PyBind11Synapse, m) {
    m.doc() = "Python bindings for Synapse class";
    
    py::class_<Synapse, std::shared_ptr<Synapse>>(m, "Synapse")
        .def("getId", &Synapse::getId)
        .def("getWeight", &Synapse::getWeight)
        .def("getDelay", &Synapse::getDelay)
        .def("getMaxWeight", &Synapse::getMaxWeight)
        .def("getMinWeight", &Synapse::getMinWeight)
        .def("getPreSynapseId", &Synapse::getPreSynapseId)
        .def("getPostSynapseId", &Synapse::getPostSynapseId)
        .def("enablePlasticity", &Synapse::enablePlasticity)
        .def("setWeight", &Synapse::setWeight)
        .def("updateWeight", &Synapse::updateWeight)
        .def("isPlastic", &Synapse::isPlastic)
        .def("isActive", &Synapse::isActive);
}

// Python bindings for Neuron
#include "brain/Neuron.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(PyBind11Neuron, m) {
    m.doc() = "Python bindings for Neuron class";
    
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .export_values();
    
    py::class_<Neuron, std::shared_ptr<Neuron>>(m, "Neuron")
        .def("getId", &Neuron::getId)
        .def("getType", &Neuron::getType)
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("getThreshold", &Neuron::getThreshold)
        .def("getRestPotential", &Neuron::getRestPotential)
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod)
        .def("injectCurrent", &Neuron::injectCurrent)
        .def("update", &Neuron::update)
        .def("isSpiking", &Neuron::isSpiking)
        .def("isInRefractory", &Neuron::isInRefractory)
        .def("enablePlasticity", &Neuron::enablePlasticity)
        .def("isActive", &Neuron::isActive)
        .def("getSpikeCount", &Neuron::getSpikeCount)
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("getInputConnectionCount", &Neuron::getInputConnectionCount)
        .def("getOutputConnectionCount", &Neuron::getOutputConnectionCount);
}

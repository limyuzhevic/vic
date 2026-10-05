// Python bindings for Brain
#include "brain/Brain.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(PyBind11Brain, m) {
    m.doc() = "Python bindings for Brain class";
    
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &Brain::initialize)
        .def("reset", &Brain::reset)
        .def("step", &Brain::step)
        .def("getRegions", &Brain::getRegions)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getWorkingMemory", &Brain::getWorkingMemory)
        .def("getEpisodicMemory", &Brain::getEpisodicMemory)
        .def("getAssociativeMemory", &Brain::getAssociativeMemory)
        .def("getDopamine", &Brain::getDopamine)
        .def("getCuriosity", &Brain::getCuriosity)
        .def("getNovelty", &Brain::getNovelty)
        .def("getPredictionSystem", &Brain::getPredictionSystem)
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem)
        .def("getPlanner", &Brain::getPlanner)
        .def("getConceptFormation", &Brain::getConceptFormation)
        .def("getAttention", &Brain::getAttention)
        .def("getSensoryInput", &Brain::getSensoryInput)
        .def("getMotorSystem", &Brain::getMotorSystem)
        .def("getEnvironment", &Brain::getEnvironment)
        .def("save", &Brain::save)
        .def("load", &Brain::load)
        .def("logStatus", &Brain::logStatus)
        .def("getConfig", &Brain::getConfig);
}

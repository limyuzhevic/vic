// Python bindings for Brain class
#include "Brain.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include <string>

namespace py = pybind11;

void bindBrain(py::module_ &m) {
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>&>())
        .def("initialize", &Brain::initialize)
        .def("step", &Brain::step)
        .def("step", [](Brain& b, SimulationStep step, Timestamp time) {
            b.step(step, time);
        })
        .def("reset", &Brain::reset)
        .def("save", &Brain::save)
        .def("load", &Brain::load)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getWorkingMemory", &Brain::getWorkingMemory)
        .def("getEpisodicMemory", &Brain::getEpisodicMemory)
        .def("getAssociativeMemory", &Brain::getAssociativeMemory)
        .def("getPredictionSystem", &Brain::getPredictionSystem)
        .def("getPlanner", &Brain::getPlanner)
        .def("getConceptFormation", &Brain::getConceptFormation)
        .def("getAttention", &Brain::getAttention)
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem)
        .def("getDopamine", &Brain::getDopamine)
        .def("getCuriosity", &Brain::getCuriosity)
        .def("getNovelty", &Brain::getNovelty)
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal)
        .def("getConfig", &Brain::getConfig)
        .def("getRandomGenerator", &Brain::getRandomGenerator)
        .def("logStatus", &Brain::logStatus);
}

// Additional helper functions for Brain
void bindBrainHelpers(py::module_ &m) {
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    });
}

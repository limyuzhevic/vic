// Python bindings for AgentBrain class
#include "AgentBrain.hpp"
#include "World.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include <string>

namespace py = pybind11;

void bindAgentBrain(py::module_ &m) {
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>&>())
        .def("initialize", &AgentBrain::initialize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &AgentBrain::updateDevelopment)
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &AgentBrain::enableDevelopment)
        .def("enableCuriosity", &AgentBrain::enableCuriosity)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getBrain", &AgentBrain::getBrain)
        .def("reset", &AgentBrain::reset);
}

void bindAgentBrainHelpers(py::module_ &m) {
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    });
}

// Python bindings for SimpleWorld class
#include "World.hpp"
#include "SimpleWorld.hpp"
#include "Observation.hpp"
#include "Action.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include <string>

namespace py = pybind11;

void bindWorld(py::module_ &m) {
    // World base class
    py::class_<World>(m, "World")
        .def("reset", &World::reset)
        .def("update", &World::update)
        .def("getSimulationTime", &World::getSimulationTime)
        .def("getSensoryPercept", &World::getSensoryPercept)
        .def("applyMotorCommand", &World::applyMotorCommand)
        .def("computeReward", &World::computeReward)
        .def("setAgentStart", &World::setAgentStart)
        .def("getWidth", &World::getWidth)
        .def("getHeight", &World::getHeight)
        .def("getVisionWidth", &World::getVisionWidth)
        .def("getVisionHeight", &World::getVisionHeight);

    // SimpleWorld class
    py::class_<SimpleWorld, World>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed)
        .def("addObject", &SimpleWorld::addObject)
        .def("removeObject", &SimpleWorld::removeObject)
        .def("isValidPosition", &SimpleWorld::isValidPosition)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy);
}

void bindWorldHelpers(py::module_ &m) {
    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    });
}

// NLM Python Bindings - World System
// This file contains bindings for the world simulation and environment

namespace py = pybind11;
namespace nlm {

py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
    .def(py::init<>())
    .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
         py::arg("visionWidth"), py::arg("visionHeight"))
    .def("reset", &SimpleWorld::reset)
    .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
    .def("update", &SimpleWorld::update, py::arg("timestep"))
    .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
         py::arg("cmd"), py::arg("currentTime"))
    .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
         py::return_value_policy::reference_internal)
    .def("getAgentBody", &SimpleWorld::getAgentBody,
         py::return_value_policy::reference_internal)
    .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
    .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
    .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
    .def("getWidth", &SimpleWorld::getWidth)
    .def("getHeight", &SimpleWorld::getHeight)
    .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
    .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
    .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
    .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
    .def("getSimulationTime", &SimpleWorld::getSimulationTime)
    .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
    .def("getRandomSeed", &SimpleWorld::getRandomSeed);

m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
    return std::make_shared<SimpleWorld>();
}, "Create a new simple world");

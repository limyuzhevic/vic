// NLM Python Bindings - Perception System
// This file contains bindings for sensory perception and world interaction

namespace py = pybind11;
namespace nlm {

py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
    .def(py::init<>())
    .def("getVision", &SensoryPercept::getVision)
    .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
    .def("getVisionWidth", &SensoryPercept::getVisionWidth)
    .def("getVisionHeight", &SensoryPercept::getVisionHeight)
    .def("getTouch", &SensoryPercept::getTouch)
    .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
    .def("getInternal", &SensoryPercept::getInternal)
    .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
    .def("getProprioception", &SensoryPercept::getProprioception)
    .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
    .def("getAudio", &SensoryPercept::getAudio)
    .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
    .def("getAllSignals", &SensoryPercept::getAllSignals)
    .def("getTimestamp", &SensoryPercept::getTimestamp)
    .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

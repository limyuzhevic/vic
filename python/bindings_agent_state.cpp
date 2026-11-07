#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/ActionResult.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

py::class_<AgentBody> agentBodyClass(m, "AgentBody", R"pbdoc(Agent body state)pbdoc");
agentBodyClass
    .def(py::init<>())
    .def_readwrite("x", &AgentBody::x)
    .def_readwrite("y", &AgentBody::y)
    .def_readwrite("orientation", &AgentBody::orientation)
    .def_readwrite("velocityX", &AgentBody::velocityX)
    .def_readwrite("velocityY", &AgentBody::velocityY)
    .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
    .def_readwrite("energy", &AgentBody::energy)
    .def_readwrite("health", &AgentBody::health)
    .def_readwrite("age", &AgentBody::age)
    .def_readwrite("isMoving", &AgentBody::isMoving)
    .def_readwrite("isTurning", &AgentBody::isTurning)
    .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
    .def("reset", &AgentBody::reset);

py::class_<ActionResult> actionResultClass(m, "ActionResult", R"pbdoc(Action result from world)pbdoc");
actionResultClass
    .def(py::init<>())
    .def(py::init<float, bool, std::string>(),
         py::arg("reward"), py::arg("success"), py::arg("message") = "")
    .def_readwrite("reward", &ActionResult::reward)
    .def_readwrite("success", &ActionResult::success)
    .def_readwrite("message", &ActionResult::message);

py::class_<SensoryPercept> sensoryPerceptClass(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc");
sensoryPerceptClass
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
}
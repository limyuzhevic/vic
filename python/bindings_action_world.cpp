#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/motor/Action.hpp"
#include "../src/environment/Observation.hpp"

namespace py = pybind11;
namespace nlm {

py::class_<Action> actionClass(m, "Action", R"pbdoc(Action representation for motor output)pbdoc");
actionClass
    .def(py::init<>())
    .def(py::init<ActionType>(), py::arg("type"))
    .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
    .def("getType", &Action::getType)
    .def("setType", &Action::setType, py::arg("type"))
    .def("getParameters", &Action::getParameters)
    .def("setParameters", &Action::setParameters, py::arg("params"))
    .def("getName", &Action::getName)
    .def("clone", &Action::clone);

py::class_<WorldObject> worldObjectClass(m, "WorldObject", R"pbdoc(World object representation)pbdoc");
worldObjectClass
    .def(py::init<>())
    .def(py::init<float, float, WorldObjectType, float, float>(),
         py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
         py::arg("radius") = 0.5f)
    .def_readwrite("x", &WorldObject::x)
    .def_readwrite("y", &WorldObject::y)
    .def_readwrite("radius", &WorldObject::radius)
    .def_readwrite("type", &WorldObject::type)
    .def_readwrite("value", &WorldObject::value)
    .def_readwrite("active", &WorldObject::active);
}
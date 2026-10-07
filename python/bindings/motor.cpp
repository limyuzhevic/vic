// Action and motor-related class bindings for NLM Python API
// This file provides Python bindings for action and motor processing classes

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "../src/motor/Action.hpp"
#include "../src/world/Observation.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/world/WorldObject.hpp"

namespace py = pybind11;

void bindActionClasses(py::module& m) {
    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
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

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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

    // Observation class
    py::class_<Observation>(m, "Observation", R"pbdoc(Observation from world)pbdoc")
        .def(py::init<>())
        .def("addObject", &Observation::addObject, py::arg("obj"))
        .def("removeObject", &Observation::removeObject, py::arg("x"), py::arg("y"))
        .def("getObjects", &Observation::getObjects,
             py::return_value_policy::reference_internal)
        .def("getObjectCount", &Observation::getObjectCount)
        .def("clear", &Observation::clear);

    // Convenience functions for action creation
    m.def("createAction", [](ActionType type, std::vector<float> params = {}) {
        return Action(type, params);
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
        "Create an Action object");

    m.def("createWorldObject", [](float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
        return WorldObject(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
        "Create a WorldObject");

    m.def("createAgentBody", []() {
        return AgentBody();
    }, "Create an AgentBody");
}
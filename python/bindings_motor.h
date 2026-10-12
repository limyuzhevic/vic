// NLM Python Bindings - Motor System
// This file contains bindings for motor output and action systems

namespace py = pybind11;
namespace nlm {

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

py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
    .def(py::init<>())
    .def(py::init<float, bool, std::string>(),
         py::arg("reward"), py::arg("success"), py::arg("message") = "")
    .def_readwrite("reward", &ActionResult::reward)
    .def_readwrite("success", &ActionResult::success)
    .def_readwrite("message", &ActionResult::message);

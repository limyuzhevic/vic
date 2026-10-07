// Action system bindings

#include "bindings.h"

namespace nlm {

void bind_action_module(py::module_& m) {
    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output and planning.
        
        Actions are the basic units of behavior in the NLM system:
        - Define what an agent can do
        - Include type and parameters for flexibility
        - Can be cloned for experimentation
        - Have human-readable names for debugging
        
        Action types include:
        - Movement: MoveForward, MoveBackward, MoveLeft, MoveRight
        - Rotation: TurnLeft, TurnRight
        - Gaze: Look, LookUp, LookDown
        - Interaction: Interact, Eat, Drink
        - Behavior: Rest, Wait, Custom
        
        Parameters allow for fine-grained control:
        - Movement parameters: speed, duration, rotation angle
        - Gaze parameters: target position, speed
        - Interaction parameters: target object, force
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action with just a type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters as vector")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action");
}

} // namespace nlm
// pybind11_motor_module.cpp - Motor module bindings implementation

#include "pybind11_motor_module.h"
#include <pybind11/stl.h>

namespace nlm {

void bind_motor_module(py::module& m) {
    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Motor action representation)
    
    Represents an action that an agent can perform in the world.
    Actions are the output of the agent's decision-making process and are
    executed by the world simulation to produce effects and potentially rewards.
    
    Action components:
    - type: The type of action (e.g., MoveForward, TurnLeft, Interact)
    - parameters: Additional parameters for complex actions (position, force, etc.)
    
    Action types include:
    - Basic movement: MoveForward, MoveBackward, TurnLeft, TurnRight
    - Specialized movements: Look, LookUp, LookDown
    - Interaction: Interact, Eat, Drink
    - State: Rest, Wait, Custom
    
    Actions can be:
    - Created with just a type for simple actions
    - Created with type and parameters for complex actions
    - Cloned and modified for action planning
    - Serialized and transmitted between agents
    "pbdoc")
    .def(py::init<>())
    .def(py::init<ActionType>(), py::arg("type"),
         "Create action with specified type")
    .def(py::init<ActionType, std::vector<float>>(),
         py::arg("type"), py::arg("parameters"),
         "Create action with type and parameters")
    .def("get_type", &Action::getType, "Get the action type")
    .def("set_type", &Action::setType, py::arg("type"), "Set the action type")
    .def("get_parameters", &Action::getParameters, "Get action parameters")
    .def("set_parameters", &Action::setParameters, py::arg("params"),
         "Set action parameters")
    .def("get_name", &Action::getName, "Get human-readable action name")
    .def("get_description", [](const Action& self) {
        std::string type_str = std::to_string(static_cast<int>(self.getType()));
        auto params = self.getParameters();
        std::string result = "Action[" + type_str + "]";
        if (!params.empty()) {
            result += " params[" + std::to_string(params.size()) + "]";
        }
        return result;
    }, "Get descriptive string of action")
    .def("clone", &Action::clone, "Create a copy of this action")
    .def("is_complex", [](const Action& self) {
        return !self.getParameters().empty();
    }, "Check if action has additional parameters")
    .def("has_parameter", [](const Action& self, size_t index) {
        auto params = self.getParameters();
        return index < params.size();
    }, py::arg("index"), "Check if action has parameter at index")
    .def("get_parameter", [](const Action& self, size_t index) {
        auto params = self.getParameters();
        if (index < params.size()) {
            return params[index];
        } else {
            throw py::index_error("Parameter index out of range");
        }
    }, py::arg("index"), "Get parameter value at index")
    .def("set_parameter", [](Action& self, size_t index, float value) {
        auto params = self.getParameters();
        if (index >= params.size()) {
            params.resize(index + 1, 0.0f);
        }
        params[index] = value;
        self.setParameters(params);
    }, py::arg("index"), py::arg("value"), "Set parameter value at index")
    .def("add_parameter", [](Action& self, float value) {
        auto params = self.getParameters();
        params.push_back(value);
        self.setParameters(params);
    }, py::arg("value"), "Add a parameter to the action")
    .def("clear_parameters", [](Action& self) {
        self.setParameters(std::vector<float>());
    }, "Clear all parameters from the action")
    .def("execute", [](Action& self) {
        // Action execution is handled by the world simulation
        // This method returns a placeholder result
        return ActionResult(0.0f, false, "Action executed (world handles actual execution)");
    }, "Execute the action (world handles actual execution)");
}

} // namespace nlm

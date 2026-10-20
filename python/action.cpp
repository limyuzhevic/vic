#include "action.h"
#include "error_handling.h"
#include <pybind11/stl.h>

namespace py = pybind11;
namespace nlm {

void include_action(py::module_& m) {
    // Action class with enhanced Pythonic features
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
    
    The Action class represents a motor command to be executed by the agent.
    It contains both a type (ActionType) and optional parameters for fine-grained control.
    
    Actions can be created with various constructors and support cloning for reusability.
    
    Examples:
        >>> action = Action(ActionType.MoveForward)
        >>> action = Action(ActionType.Custom, [1.0, 2.0, 0.5])
        >>> action.clone()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create a basic action with specified type.")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an action with type and parameters. Parameters should be normalized to [0, 1] range.")
        .def(py::init<ActionType, std::vector<float>, float>(), py::arg("type"), py::arg("parameters"), py::arg("timestamp"),
             "Create an action with type, parameters, and timestamp.")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters as list")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set the action parameters. Parameters should be in [0, 1] range.")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get the number of parameters")
        .def("getName", &Action::getName, "Get the human-readable name of the action type")
        .def("getDescription", &Action::getDescription, "Get the description of the action type")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to the action")
        .def("removeParameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params.erase(params.begin() + index);
                self.setParameters(params);
            }
        }, py::arg("index"), "Remove a parameter by index")
        .def("normalizeParameters", [](Action& self) {
            self.normalizeParameters();
        }, "Normalize all parameters to [0, 1] range")
        .def("clone", &Action::clone, "Create a deep copy of this action")
        .def("isValid", &Action::isValid, "Check if the action is valid")
        .def("to_dict", [](const Action& self) {
            return std::make_tuple(self.getType(), self.getParameters());
        }, "Convert action to tuple (type, parameters)")
        .def("from_dict", [](Action& self, ActionType type, const std::vector<float>& params) {
            self.setType(type);
            self.setParameters(params);
        }, "Set action from tuple (type, parameters)");

    // WorldObject class with enhanced Pythonic features
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
    
    The WorldObject class represents an object in the NLM world simulation.
    Objects can be resources, hazards, walls, markers, or empty space.
    
    WorldObjects have physical properties (position, radius) and metadata (type, value).
    
    Examples:
        >>> obj = WorldObject(1.0, 2.0, WorldObjectType.Resource, 10.0)
        >>> obj = WorldObject(x=0.0, y=1.5, type=WorldObjectType.Wall, radius=0.5)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object with position, type, value, and radius.")
        .def(py::init<float, float, WorldObjectType>(),
             py::arg("x"), py::arg("y"), py::arg("type"),
             "Create a world object with position and type. Uses default radius 0.5.")
        .def(py::init<float, float, WorldObjectType, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value"),
             "Create a world object with position, type, and value. Uses default radius 0.5.")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance from this object to point (x, y)")
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < (self.radius + other.radius);
        }, py::arg("other"), "Check if this object collides with another object")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set the position of the object")
        .def("getTypeName", [](WorldObjectType type) {
            switch (type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get the string name for a world object type", py::arg("type"))
        .def("getValueDescription", [](WorldObjectType type, float value) {
            if (type == WorldObjectType::Resource) return "Food/Resource amount: " + std::to_string(value);
            if (type == WorldObjectType::Hazard) return "Hazard severity: " + std::to_string(value);
            if (type == WorldObjectType::Marker) return "Marker identifier: " + std::to_string((int)value);
            return "Value: " + std::to_string(value);
        }, "Get a descriptive string for object type and value", py::arg("type"), py::arg("value"));

    // Type conversion and validation utilities
    m.def("createWorldObject", [](float x, float y, WorldObjectType type, 
                                 float value = 0.0f, float radius = 0.5f) {
        return WorldObject(x, y, type, value, radius);
    }, "Factory function to create a WorldObject", py::arg("x"), py::arg("y"), 
       py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f);

    m.def("createResource", [](float x, float y, float value = 1.0f, float radius = 0.3f) {
        return WorldObject(x, y, WorldObjectType::Resource, value, radius);
    }, "Factory function to create a resource object", py::arg("x"), py::arg("y"), 
       py::arg("value") = 1.0f, py::arg("radius") = 0.3f);

    m.def("createHazard", [](float x, float y, float value = 0.5f, float radius = 0.4f) {
        return WorldObject(x, y, WorldObjectType::Hazard, value, radius);
    }, "Factory function to create a hazard object", py::arg("x"), py::arg("y"), 
       py::arg("value") = 0.5f, py::arg("radius") = 0.4f);

    m.def("createWall", [](float x, float y, float radius = 0.2f) {
        return WorldObject(x, y, WorldObjectType::Wall, 0.0f, radius);
    }, "Factory function to create a wall object", py::arg("x"), py::arg("y"), 
       py::arg("radius") = 0.2f);

    m.def("createMarker", [](float x, float y, float id = 0.0f, float radius = 0.1f) {
        return WorldObject(x, y, WorldObjectType::Marker, id, radius);
    }, "Factory function to create a marker object", py::arg("x"), py::arg("y"), 
       py::arg("id") = 0.0f, py::arg("radius") = 0.1f);

} // namespace nlm
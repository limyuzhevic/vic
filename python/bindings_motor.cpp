#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/motor/Action.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

void bindMotor(py::module_ &m) {
    // Action class - Pythonic wrapper
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        
        // Pythonic property access
        .def_readwrite("type", &Action::getType, &Action::setType, "Action type (alias for getType/setType)")
        .def_readwrite("parameters", &Action::getParameters, &Action::setParameters, "Action parameters (alias for getParameters/setParameters)")
        
        // Pythonic helper methods
        .def("is_movement", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::MoveForward || type == ActionType::MoveBackward ||
                   type == ActionType::MoveLeft || type == ActionType::MoveRight;
        }, "Check if this is a movement action")
        
        .def("is_rotation", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::TurnLeft || type == ActionType::TurnRight;
        }, "Check if this is a rotation action")
        
        .def("is_interaction", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::Interact || type == ActionType::Eat || type == ActionType::Drink;
        }, "Check if this is an interaction action")
        
        .def("is_look_action", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::Look || type == ActionType::LookUp || type == ActionType::LookDown;
        }, "Check if this is a look action")
        
        .def("clone", &Action::clone, "Create a copy of this action")
        
        // Factory methods for common actions
        .def_static("move_forward", []() { return Action(ActionType::MoveForward); },
                   "Create a move forward action")
        .def_static("move_backward", []() { return Action(ActionType::MoveBackward); },
                   "Create a move backward action")
        .def_static("turn_left", []() { return Action(ActionType::TurnLeft); },
                   "Create a turn left action")
        .def_static("turn_right", []() { return Action(ActionType::TurnRight); },
                   "Create a turn right action")
        .def_static("look", []() { return Action(ActionType::Look); },
                   "Create a look action")
        .def_static("interact", []() { return Action(ActionType::Interact); },
                   "Create an interact action")
        
        // Conversion to dictionary
        .def("to_dict", [](const Action& self) {
            return py::dict(
                "type", static_cast<int>(self.getType()),
                "name", self.getName(),
                "parameters", self.getParameters()
            );
        }, "Convert action to dictionary")
        
        // Creation from dictionary
        .def_static("from_dict", [](const py::dict& d) {
            Action action;
            if (d.contains("type")) {
                action.setType(static_cast<ActionType>(d["type"].cast<int>()));
            }
            if (d.contains("parameters")) {
                action.setParameters(d["parameters"].cast<std::vector<float>>());
            }
            return action;
        }, py::arg("d"), "Create action from dictionary")
        
        .def("__repr__", [](const Action& action) {
            return "<Action type=" + action.getName() + 
                   " params=" + std::to_string(action.getParameters().size()) + ">";
        });

    // MotorCommand enum - simplified interface
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // WorldObject class - simplified interface
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X position")
        .def_readwrite("y", &WorldObject::y, "Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Object active state")
        
        // Pythonic property access
        .def("position", [](WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, "Get position as tuple")
        
        .def("set_position", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, "Set position from tuple")
        
        .def("distance_to", [](const WorldObject& obj, const WorldObject& other) {
            float dx = obj.x - other.x;
            float dy = obj.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, "Calculate distance to another object")
        
        .def("is_near", [](const WorldObject& obj, const WorldObject& other, float threshold) {
            float dx = obj.x - other.x;
            float dy = obj.y - other.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            return dist < threshold;
        }, "Check if object is near another object within threshold")
        
        .def("to_dict", [](const WorldObject& obj) {
            return py::dict(
                "x", obj.x,
                "y", obj.y,
                "radius", obj.radius,
                "type", static_cast<int>(obj.type),
                "value", obj.value,
                "active", obj.active
            );
        }, "Convert world object to dictionary")
        
        .def_static("from_dict", [](const py::dict& d) {
            WorldObject obj;
            obj.x = d["x"].cast<float>();
            obj.y = d["y"].cast<float>();
            obj.radius = d["radius"].cast<float>();
            obj.type = static_cast<WorldObjectType>(d["type"].cast<int>());
            obj.value = d["value"].cast<float>();
            obj.active = d["active"].cast<bool>();
            return obj;
        }, py::arg("d"), "Create world object from dictionary")
        
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject type=" + std::to_string(static_cast<int>(obj.type)) +
                   " pos=(" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")>";
        });

    // AgentBody class - simplified interface
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Last action time")
        
        // Pythonic property access
        .def("position", [](AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get position as tuple")
        
        .def("velocity", [](AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get velocity as tuple")
        
        .def("speed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX*body.velocityX + body.velocityY*body.velocityY);
        }, "Get speed magnitude")
        
        .def("heading", [](const AgentBody& body) {
            return body.orientation;
        }, "Get heading angle in radians")
        
        .def("update_position", [](AgentBody& body, float dx, float dy) {
            body.x += dx;
            body.y += dy;
        }, "Update position by delta")
        
        .def("set_orientation", [](AgentBody& body, float angle) {
            body.orientation = angle;
            while (body.orientation >= 2.0f * M_PI) body.orientation -= 2.0f * M_PI;
            while (body.orientation < 0.0f) body.orientation += 2.0f * M_PI;
        }, "Set orientation angle (normalized to [0, 2π])")
        
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        
        .def("to_dict", [](const AgentBody& body) {
            return py::dict(
                "x", body.x,
                "y", body.y,
                "orientation", body.orientation,
                "velocityX", body.velocityX,
                "velocityY", body.velocityY,
                "angularVelocity", body.angularVelocity,
                "energy", body.energy,
                "health", body.health,
                "age", body.age,
                "isMoving", body.isMoving,
                "isTurning", body.isTurning,
                "lastActionTime", body.lastActionTime
            );
        }, "Convert agent body to dictionary")
        
        .def_static("from_dict", [](const py::dict& d) {
            AgentBody body;
            body.x = d["x"].cast<float>();
            body.y = d["y"].cast<float>();
            body.orientation = d["orientation"].cast<float>();
            body.velocityX = d["velocityX"].cast<float>();
            body.velocityY = d["velocityY"].cast<float>();
            body.angularVelocity = d["angularVelocity"].cast<float>();
            body.energy = d["energy"].cast<float>();
            body.health = d["health"].cast<float>();
            body.age = d["age"].cast<uint32_t>();
            body.isMoving = d["isMoving"].cast<bool>();
            body.isTurning = d["isTurning"].cast<bool>();
            body.lastActionTime = d["lastActionTime"].cast<float>();
            return body;
        }, py::arg("d"), "Create agent body from dictionary")
        
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody pos=(" + std::to_string(body.x) + ", " + std::to_string(body.y) + ")" +
                   " energy=" + std::to_string(body.energy) + ">";
        });
}

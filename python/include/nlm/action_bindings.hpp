// NLM Python Bindings - Action and World Objects
// Action, WorldObject, AgentBody, ActionResult class bindings

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "exceptions.hpp"
#include "validation.hpp"

namespace py = pybind11;
namespace nlm {

void bind_action(py::module_& m) {
    // Action class binding with improved error handling
    py::class_<Action, std::shared_ptr<Action>>(m, "Action", R"pbdoc(Action representation for motor output)
    
    Represents an action that can be performed by the agent.
    Actions can be created with a type and optional parameters.
    "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), R"pbdoc(Create an Action with type)
        
        Args:
            type: Action type
        
        Raises:
            ValidationError: If type is invalid
        )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), 
             py::arg("type"), py::arg("parameters"), R"pbdoc(Create an Action with type and parameters)
        
        Args:
            type: Action type
            parameters: Action parameters
        
        Raises:
            ValidationError: If type is invalid or parameters are invalid
        )pbdoc")
        .def("getType", &Action::getType, R"pbdoc(Get action type)
        
        Returns:
            Action type
        )pbdoc")
        .def("setType", [](Action& self, ActionType type) {
            try {
                validation::check_allowed("action type", type, 
                                         {ActionType::MoveForward, ActionType::MoveBackward,
                                          ActionType::MoveLeft, ActionType::MoveRight,
                                          ActionType::TurnLeft, ActionType::TurnRight,
                                          ActionType::Look, ActionType::LookUp,
                                          ActionType::LookDown, ActionType::Interact,
                                          ActionType::Eat, ActionType::Drink,
                                          ActionType::Rest, ActionType::Wait,
                                          ActionType::Custom});
                self.setType(type);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ActionError(std::string("Failed to set action type: ") + e.what());
            }
        }, py::arg("type"), R"pbdoc(Set action type)
        
        Args:
            type: Action type
        
        Raises:
            ActionError: If action type cannot be set
            ValidationError: If type is invalid
        )pbdoc")
        .def("getParameters", &Action::getParameters, R"pbdoc(Get action parameters)
        
        Returns:
            Vector of action parameters
        )pbdoc")
        .def("setParameters", [](Action& self, const std::vector<float>& params) {
            try {
                validation::check_vector_bounds("action parameters", params, 0, 100);
                self.setParameters(params);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ActionError(std::string("Failed to set action parameters: ") + e.what());
            }
        }, py::arg("params"), R"pbdoc(Set action parameters)
        
        Args:
            params: Action parameters
        
        Raises:
            ActionError: If parameters cannot be set
            ValidationError: If parameters are invalid
        )pbdoc")
        .def("getName", &Action::getName, R"pbdoc(Get action name)
        
        Returns:
            Action name as string
        )pbdoc")
        .def("getDescription", [](const Action& self) {
            // Add a helper method to get action description
            std::string typeName;
            switch (self.getType()) {
                case ActionType::MoveForward: typeName = "MoveForward"; break;
                case ActionType::MoveBackward: typeName = "MoveBackward"; break;
                case ActionType::MoveLeft: typeName = "MoveLeft"; break;
                case ActionType::MoveRight: typeName = "MoveRight"; break;
                case ActionType::TurnLeft: typeName = "TurnLeft"; break;
                case ActionType::TurnRight: typeName = "TurnRight"; break;
                case ActionType::Look: typeName = "Look"; break;
                case ActionType::LookUp: typeName = "LookUp"; break;
                case ActionType::LookDown: typeName = "LookDown"; break;
                case ActionType::Interact: typeName = "Interact"; break;
                case ActionType::Eat: typeName = "Eat"; break;
                case ActionType::Drink: typeName = "Drink"; break;
                case ActionType::Rest: typeName = "Rest"; break;
                case ActionType::Wait: typeName = "Wait"; break;
                case ActionType::Custom: typeName = "Custom"; break;
                default: typeName = "Unknown";
            }
            return typeName;
        }, R"pbdoc(Get action name/description)
        
        Returns:
            Action name as string
        )pbdoc")
        .def("clone", &Action::clone, R"pbdoc(Create a copy of this action)
        
        Returns:
            Cloned action
        )pbdoc")
        .def("__repr__", [](const Action& action) {
            std::string typeName;
            switch (action.getType()) {
                case ActionType::MoveForward: typeName = "MoveForward"; break;
                case ActionType::MoveBackward: typeName = "MoveBackward"; break;
                case ActionType::MoveLeft: typeName = "MoveLeft"; break;
                case ActionType::MoveRight: typeName = "MoveRight"; break;
                case ActionType::TurnLeft: typeName = "TurnLeft"; break;
                case ActionType::TurnRight: typeName = "TurnRight"; break;
                case ActionType::Look: typeName = "Look"; break;
                case ActionType::LookUp: typeName = "LookUp"; break;
                case ActionType::LookDown: typeName = "LookDown"; break;
                case ActionType::Interact: typeName = "Interact"; break;
                case ActionType::Eat: typeName = "Eat"; break;
                case ActionType::Drink: typeName = "Drink"; break;
                case ActionType::Rest: typeName = "Rest"; break;
                case ActionType::Wait: typeName = "Wait"; break;
                case ActionType::Custom: typeName = "Custom"; break;
                default: typeName = "Unknown";
            }
            return "<Action: " + typeName + ", " + std::to_string(action.getParameters().size()) + " params>";
        });

    // WorldObject class binding
    py::class_<WorldObject, std::shared_ptr<WorldObject>>(m, "WorldObject", R"pbdoc(World object representation)
    
    Represents an object in the world with position, type, and properties.
    "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, R"pbdoc(Create a WorldObject)
        
        Args:
            x: X coordinate
            y: Y coordinate  
            type: Object type
            value: Object value (default: 0.0)
            radius: Object radius (default: 0.5)
        
        Raises:
            ValidationError: If coordinates or radius are invalid
        )pbdoc")
        .def_readwrite("x", &WorldObject::x, R"pbdoc(X coordinate)pbdoc")
        .def_readwrite("y", &WorldObject::y, R"pbdoc(Y coordinate)pbdoc")
        .def_readwrite("radius", &WorldObject::radius, R"pbdoc(Object radius)pbdoc")
        .def_readwrite("type", &WorldObject::type, R"pbdoc(Object type)pbdoc")
        .def_readwrite("value", &WorldObject::value, R"pbdoc(Object value)pbdoc")
        .def_readwrite("active", &WorldObject::active, R"pbdoc(Whether object is active)pbdoc")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            try {
                validation::check_bounds("x coordinate", x, 0.0f, 1000.0f);
                validation::check_bounds("y coordinate", y, 0.0f, 1000.0f);
                float dx = self.x - x;
                float dy = self.y - y;
                return std::sqrt(dx * dx + dy * dy);
            } catch (const ValidationError& e) {
                throw;
            }
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate distance to another position)
        
        Args:
            x: X coordinate
            y: Y coordinate
        
        Returns:
            Distance to the specified position
        
        Raises:
            ValidationError: If coordinates are invalid
        )pbdoc")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float distance = self.distanceTo(other.x, other.y);
            return distance < (self.radius + other.radius);
        }, R"pbdoc(Check if this object is colliding with another)
        
        Args:
            other: Another WorldObject to check
        
        Returns:
            True if objects are colliding, False otherwise
        )pbdoc")
        .def("__repr__", [](const WorldObject& obj) {
            std::string typeName;
            switch (obj.type) {
                case WorldObjectType::Empty: typeName = "Empty"; break;
                case WorldObjectType::Resource: typeName = "Resource"; break;
                case WorldObjectType::Hazard: typeName = "Hazard"; break;
                case WorldObjectType::Wall: typeName = "Wall"; break;
                case WorldObjectType::Marker: typeName = "Marker"; break;
                default: typeName = "Unknown";
            }
            return "<WorldObject: " + typeName + " at (" + std::to_string(obj.x) + ", " +
                   std::to_string(obj.y) + ") radius=" + std::to_string(obj.radius) + ">";
        });

    // AgentBody class binding
    py::class_<AgentBody, std::shared_ptr<AgentBody>>(m, "AgentBody", R"pbdoc(Agent body state)
    
    Represents the physical state of the agent in the world, including
    position, velocity, energy, health, and other attributes.
    "pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, R"pbdoc(X position)pbdoc")
        .def_readwrite("y", &AgentBody::y, R"pbdoc(Y position)pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation, R"pbdoc(Orientation in radians)pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX, R"pbdoc(X velocity)pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY, R"pbdoc(Y velocity)pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, R"pbdoc(Angular velocity)pbdoc")
        .def_readwrite("energy", &AgentBody::energy, R"pbdoc(Current energy level)pbdoc")
        .def_readwrite("health", &AgentBody::health, R"pbdoc(Current health level)pbdoc")
        .def_readwrite("age", &AgentBody::age, R"pbdoc(Age in simulation steps)pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving, R"pbdoc(Whether agent is moving)pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning, R"pbdoc(Whether agent is turning)pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, R"pbdoc(Time since last action)pbdoc")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, R"pbdoc(Get current speed)
        
        Returns:
            Speed magnitude
        )pbdoc")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, R"pbdoc(Get current position)
        
        Returns:
            Tuple of (x, y) coordinates
        )pbdoc")
        .def("getOrientationDegrees", [](const AgentBody& self) {
            return self.orientation * 180.0 / M_PI;
        }, R"pbdoc(Get orientation in degrees)
        
        Returns:
            Orientation in degrees (0-360)
        )pbdoc")
        .def("setOrientationDegrees", [](AgentBody& self, float degrees) {
            try {
                validation::check_bounds("orientation degrees", degrees, 0.0f, 360.0f);
                self.orientation = degrees * M_PI / 180.0;
            } catch (const ValidationError& e) {
                throw;
            }
        }, py::arg("degrees"), R"pbdoc(Set orientation in degrees)
        
        Args:
            degrees: Orientation in degrees (0-360)
        
        Raises:
            ValidationError: If degrees is out of range
        )pbdoc")
        .def("reset", &AgentBody::reset, R"pbdoc(Reset agent body to default state)
        
        Resets the agent body to default state.
        )pbdoc")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos(" + std::to_string(body.x) + ", " + std::to_string(body.y) +
                   ") energy=" + std::to_string(body.energy) + " health=" + std::to_string(body.health) +
                   " age=" + std::to_string(body.age) + ">";
        });

    // ActionResult class binding
    py::class_<ActionResult, std::shared_ptr<ActionResult>>(m, "ActionResult", R"pbdoc(Action result from world)
    
    Represents the result of an action performed by the agent,
    including reward, success status, and any message.
    "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "", R"pbdoc(Create an ActionResult)
        
        Args:
            reward: Reward value
            success: Whether action succeeded
            message: Result message (default: "")
        
        Raises:
            ValidationError: If reward is invalid
        )pbdoc")
        .def_readwrite("reward", &ActionResult::reward, R"pbdoc(Reward value)pbdoc")
        .def_readwrite("success", &ActionResult::success, R"pbdoc(Whether action succeeded)pbdoc")
        .def_readwrite("message", &ActionResult::message, R"pbdoc(Result message)pbdoc")
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0;
        }, R"pbdoc(Check if result is positive)
        
        Returns:
            True if reward is positive
        )pbdoc")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0;
        }, R"pbdoc(Check if result is negative)
        
        Returns:
            True if reward is negative
        )pbdoc")
        .def("isZero", [](const ActionResult& self) {
            return self.reward == 0.0;
        }, R"pbdoc(Check if result is zero)
        
        Returns:
            True if reward is zero
        )pbdoc")
        .def("getFormattedReward", [](const ActionResult& self) {
            // Format reward for display
            if (self.reward > 0) return "+" + std::to_string(self.reward);
            if (self.reward < 0) return std::to_string(self.reward);
            return "0";
        }, R"pbdoc(Get formatted reward string)
        
        Returns:
            Formatted reward string (e.g., "+5.0", "-2.5", "0")
        )pbdoc")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) +
                   ", success=" + (result.success ? "True" : "False") +
                   ", message=\"" + result.message + "\">";
        });

} // namespace nlm

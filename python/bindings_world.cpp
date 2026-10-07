// World and object system bindings

#include "bindings.h"

namespace nlm {

void bind_world_module(py::module_& m) {
    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the NLM environment.
        
        World objects represent entities in the simulated world:
        - Resources: Consumable items that provide energy/nutrition
        - Hazards: Dangerous objects that damage the agent
        - Walls: Obstacles that cannot be passed
        - Markers: Visual reference points or landmarks
        - Empty: Represents no object (used for floor spaces)
        
        Objects have:
        - Position coordinates (x, y)
        - Radius for collision detection
        - Type for behavior classification
        - Value for resource worth or hazard severity
        - Active state for toggling object presence
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object with position, type, and optional properties")
        .def_readwrite("x", &WorldObject::x, "Object X coordinate")
        .def_readwrite("y", &WorldObject::y, "Object Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius for collision detection")
        .def_readwrite("type", &WorldObject::type, "Object type (Resource, Hazard, Wall, etc.)")
        .def_readwrite("value", &WorldObject::value, "Object value (resource worth or hazard severity)")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active/present");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state and physics information.
        
        Represents the physical body of an agent in the NLM world:
        - Position and orientation in 2D space
        - Velocity and angular velocity for movement
        - Energy and health status
        - Age and movement state
        - Last action timing for behavior
        
        Body properties:
        - Position: (x, y) coordinates in world space
        - Orientation: Rotation angle in radians
        - Velocity: Linear velocity vector (vx, vy)
        - Angular velocity: Rotational speed
        - Energy: Current energy level (0-100%)
        - Health: Current health level (0-100%)
        - Age: Developmental age in simulation time
        - Movement state: Whether agent is moving or turning
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate")
        .def_readwrite("y", &AgentBody::y, "Y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (rad/s)")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0-100%)")
        .def_readwrite("health", &AgentBody::health, "Current health level (0-100%)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation time")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Timestamp of last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result from executing an action in the world.
        
        Represents the outcome of an agent's action:
        - Reward: Amount of reward received (positive) or cost (negative)
        - Success: Whether the action was successful
        - Message: Descriptive text about the outcome
        
        Action results provide:
        - Reward signal for reinforcement learning
        - Success feedback for behavior evaluation
        - Human-readable messages for debugging
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success status, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received (positive) or cost (negative)")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Descriptive message about outcome");
}

} // namespace nlm
#include "agent.h"
#include "error_handling.h"
#include <pybind11/stl.h>

namespace py = pybind11;
namespace nlm {

void include_agent(py::module_& m) {
    // AgentBody class with enhanced Pythonic features
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
    
    The AgentBody class represents the physical state of an agent in the NLM simulation.
    It includes position, orientation, velocity, energy, health, age, and movement state.
    
    The body can be reset to initial state and provides methods for movement and interaction.
    
    Examples:
        >>> body = AgentBody()
        >>> body.x = 1.0
        >>> body.y = 2.0
        >>> body.moveForward(1.0)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, float>(), py::arg("x"), py::arg("y"), py::arg("orientation"),
             "Create an agent body with position and orientation")
        .def_readwrite("x", &AgentBody::x, "X position in world coordinates")
        .def_readwrite("y", &AgentBody::y, "Y position in world coordinates")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians (0-2π)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity in rad/s")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation timesteps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether the agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether the agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Timestamp of last action")
        .def("moveForward", [](AgentBody& self, float distance, float timestep = 1.0f) {
            // Add velocity calculation logic here
            self.velocityX = distance / timestep;
            self.velocityY = 0.0f;
            self.isMoving = (distance > 0.0f);
        }, py::arg("distance"), py::arg("timestep") = 1.0f, "Move forward by distance (deprecated, use velocity instead)")
        .def("turnLeft", [](AgentBody& self, float angle, float timestep = 1.0f) {
            // Add angular velocity calculation logic here
            self.angularVelocity = angle / timestep;
            self.isTurning = (angle != 0.0f);
        }, py::arg("angle"), py::arg("timestep") = 1.0f, "Turn left by angle in radians (deprecated, use angularVelocity instead)")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get the current speed (magnitude of velocity)")
        .def("getAngle", [](const AgentBody& self) {
            if (self.velocityX == 0.0f && self.velocityY == 0.0f) {
                return self.orientation;
            }
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get the current movement direction in radians")
        .def("reset", &AgentBody::reset, "Reset body to initial state (x=0, y=0, orientation=0)")
        .def("to_dict", [](const AgentBody& self) {
            return std::make_tuple(self.x, self.y, self.orientation, self.velocityX, self.velocityY, self.energy, self.health, self.age);
        }, "Convert agent body to dictionary representation")
        .def("from_dict", [](AgentBody& self, float x, float y, float orientation, float energy, float health, float age) {
            self.x = x;
            self.y = y;
            self.orientation = orientation;
            self.energy = energy;
            self.health = health;
            self.age = age;
        }, "Set agent body from dictionary representation");

    // ActionResult class with enhanced Pythonic features
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
    
    The ActionResult class represents the outcome of an agent's action in the world.
    It contains the reward received, whether the action was successful, and an optional message.
    
    Action results are used for reinforcement learning and decision making.
    
    Examples:
        >>> result = ActionResult(reward=1.0, success=True)
        >>> result = ActionResult(reward=-0.5, success=False, message="Out of bounds")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(), py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result with reward and success status")
        .def(py::init<float, bool>(), py::arg("reward"), py::arg("success"),
             "Create an action result with reward and success status (empty message)")
        .def_readwrite("reward", &ActionResult::reward, "Reward value (positive or negative)")
        .def_readwrite("success", &ActionResult::success, "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message, "Optional message describing the outcome")
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if the reward is positive")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if the reward is negative")
        .def("isZero", [](const ActionResult& self) {
            return self.reward == 0.0f;
        }, "Check if the reward is zero")
        .def("getRewardClass", [](const ActionResult& self) {
            if (self.reward > 0.5f) return "Large Positive";
            if (self.reward > 0.0f) return "Small Positive";
            if (self.reward < -0.5f) return "Large Negative";
            if (self.reward < 0.0f) return "Small Negative";
            return "Neutral";
        }, "Get a descriptive class for the reward magnitude")
        .def("to_dict", [](const ActionResult& self) {
            return std::make_tuple(self.reward, self.success, self.message);
        }, "Convert action result to tuple (reward, success, message)")
        .def("from_dict", [](ActionResult& self, float reward, bool success, std::string message) {
            self.reward = reward;
            self.success = success;
            self.message = message;
        }, "Set action result from tuple (reward, success, message)");

    // SensoryPercept class with enhanced Pythonic features
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
    
    The SensoryPercept class represents the processed sensory input that an agent receives.
    It combines multiple sensory modalities (vision, audio, touch, internal) into a single object.
    
    Sensory perceptions are used by the agent's brain to make decisions and take actions.
    
    Examples:
        >>> percept = SensoryPercept()
        >>> percept.setVision(vision_data)
        >>> percept.setAudio(audio_data)
        >>> vision = percept.getVision()
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get the vision sensory input as Vision object")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set the vision sensory input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get the width of the vision input in pixels")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get the height of the vision input in pixels")
        .def("getVisionData", [](const SensoryPercept& self) {
            const auto& vision = self.getVision();
            return vision.getData();
        }, "Get raw vision data as vector of floats")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get the touch sensory input as vector of floats")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set the touch sensory input")
        .def("getTouchData", [](const SensoryPercept& self) {
            return self.getTouch();
        }, "Get raw touch data as vector of floats")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get the internal signals sensory input")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set the internal signals sensory input")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get the proprioception sensory input")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set the proprioception sensory input")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get the audio sensory input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set the audio sensory input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as dictionary with modality names")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVisionData().empty();
        }, "Check if vision data is present")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().getData().empty();
        }, "Check if audio data is present")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouchData().empty();
        }, "Check if touch data is present")
        .def("hasInternal", [](const SensoryPercept& self) {
            return self.getInternal().getSignalCount() > 0;
        }, "Check if internal signals are present")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get the timestamp of this sensory percept")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory percept")
        .def("getTotalSignalCount", [](const SensoryPercept& self) {
            int count = 0;
            if (!self.getVisionData().empty()) count += self.getVisionWidth() * self.getVisionHeight() * 3;
            if (!self.getTouchData().empty()) count += self.getTouchData().size();
            if (self.getInternal().getSignalCount() > 0) count += self.getInternal().getSignalCount();
            if (!self.getAudio().getData().empty()) count += self.getAudio().getNumSamples();
            return count;
        }, "Get total number of sensory signals across all modalities")
        .def("createEmptyPercept", []() {
            return SensoryPercept();
        }, "Create an empty sensory percept (factory method)");

    // Agent factory functions with enhanced Pythonic features
    m.def("createAgentBody", []() {
        return AgentBody();
    }, "Create an empty agent body", py::arg("body") = py::none());

    m.def("createActionResult", [](float reward, bool success, std::string message) {
        return ActionResult(reward, success, message);
    }, "Create an action result", py::arg("reward"), py::arg("success"), py::arg("message") = "");

    m.def("createSensoryPercept", []() {
        return SensoryPercept();
    }, "Create an empty sensory percept", py::arg("percept") = py::none());

    // Type conversion utilities
    m.def("getAgentStateSummary", [](const AgentBody& body) {
        std::ostringstream oss;
        oss << "Position: (" << body.x << ", " << body.y << ")";
        oss << " Orientation: " << body.orientation << " rad";
        oss << " Energy: " << body.energy << "/1.0";
        oss << " Health: " << body.health << "/1.0";
        oss << " Age: " << body.age << " steps";
        oss << " Moving: " << (body.isMoving ? "Yes" : "No");
        return oss.str();
    }, "Get a human-readable summary of agent body state", py::arg("body"));

    m.def("getActionResultSummary", [](const ActionResult& result) {
        std::ostringstream oss;
        oss << "Reward: " << result.reward << ", Success: " << (result.success ? "Yes" : "No");
        if (!result.message.empty()) {
            oss << ", Message: " << result.message;
        }
        return oss.str();
    }, "Get a human-readable summary of action result", py::arg("result"));

} // namespace nlm
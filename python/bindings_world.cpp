#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/agent/Action.hpp"
#include "../src/motor/Action.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/brain/Brain.hpp"

namespace py = pybind11;
namespace nlm {

void bindWorld(py::module_ &m) {
    // SimpleWorld class - Pythonic interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        
        // Pythonic helper methods
        .def("get_objects_near", [](SimpleWorld& self, float x, float y, float radius) {
            std::vector<WorldObject> nearby;
            // This would need access to internal world objects - simplified for now
            // In practice, we'd need to expose world object management
            return nearby;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Find objects near a position")
        
        .def("count_objects", [](SimpleWorld& self, WorldObjectType type) {
            // Would need internal counting method
            return 0;
        }, py::arg("type"), "Count objects of a specific type")
        
        .def("get_agent_position", [](SimpleWorld& self) {
            return std::make_pair(self.getAgentBody().x, self.getAgentBody().y);
        }, "Get agent position as tuple")
        
        .def("set_agent_position", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, "Set agent position")
        
        .def("is_agent_at", [](SimpleWorld& self, float x, float y, float tolerance = 0.1f) {
            auto body = self.getAgentBody();
            float dx = body.x - x;
            float dy = body.y - y;
            return (dx*dx + dy*dy) <= (tolerance*tolerance);
        }, py::arg("x"), py::arg("y"), py::arg("tolerance") = 0.1f,
             "Check if agent is at position")
        
        // Context manager for world episodes
        .def("episode", [](SimpleWorld& self, float timestep = 0.1f, int steps = 100) {
            return py::make_tuple(self, timestep, steps);
        }, py::arg("timestep") = 0.1f, py::arg("steps") = 100,
             "Create episode tuple (world, timestep, steps) for simulation")
        
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld size=" + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + "s>";
        });

    // SensoryPercept class - Pythonic interface
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        
        // Vision access
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        
        // Touch (protopathic sense)
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        
        // Internal signals
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        
        // Proprioception
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception,
             py::arg("proprioception"), "Set proprioception data")
        
        // Audio
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        
        // Unified access
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as dictionary")
        
        // Timestamp management
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        
        // Pythonic property access
        .def("vision_shape", [](const SensoryPercept& percept) {
            return std::vector<size_t>{percept.getVisionWidth(), percept.getVisionHeight()};
        }, "Get vision shape as [width, height]")
        
        .def("has_vision", [](const SensoryPercept& percept) {
            return percept.getVisionWidth() > 0 && percept.getVisionHeight() > 0;
        }, "Check if vision data is available")
        
        .def("to_dict", [](const SensoryPercept& percept) {
            return py::dict(
                "vision", percept.getVision(),
                "touch", percept.getTouch(),
                "internal", percept.getInternal(),
                "proprioception", percept.getProprioception(),
                "audio", percept.getAudio(),
                "timestamp", percept.getTimestamp()
            );
        }, "Convert percept to dictionary")
        
        .def_static("from_dict", [](const py::dict& d) {
            SensoryPercept percept;
            if (d.contains("vision")) {
                percept.setVision(d["vision"].cast<std::vector<float>>());
            }
            if (d.contains("touch")) {
                percept.setTouch(d["touch"].cast<std::vector<float>>());
            }
            if (d.contains("internal")) {
                percept.setInternal(d["internal"].cast<std::vector<float>>());
            }
            if (d.contains("proprioception")) {
                percept.setProprioception(d["proprioception"].cast<std::vector<float>>());
            }
            if (d.contains("audio")) {
                percept.setAudio(d["audio"].cast<std::vector<float>>());
            }
            if (d.contains("timestamp")) {
                percept.setTimestamp(d["timestamp"].cast<float>());
            }
            return percept;
        }, py::arg("d"), "Create percept from dictionary")
        
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept vision=" + std::to_string(percept.getVisionWidth()) + 
                   "x" + std::to_string(percept.getVisionHeight()) + ">";
        });

    // ActionResult class - simplified interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward and success status")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success status")
        .def_readwrite("message", &ActionResult::message, "Result message")
        
        // Pythonic properties
        .def("is_positive_reward", [](const ActionResult& result) {
            return result.reward > 0.0f;
        }, "Check if reward is positive")
        
        .def("is_negative_reward", [](const ActionResult& result) {
            return result.reward < 0.0f;
        }, "Check if reward is negative")
        
        .def("to_dict", [](const ActionResult& result) {
            return py::dict(
                "reward", result.reward,
                "success", result.success,
                "message", result.message
            );
        }, "Convert action result to dictionary")
        
        .def_static("from_dict", [](const py::dict& d) {
            ActionResult result;
            result.reward = d["reward"].cast<float>();
            result.success = d["success"].cast<bool>();
            if (d.contains("message")) {
                result.message = d["message"].cast<std::string>();
            }
            return result;
        }, py::arg("d"), "Create action result from dictionary")
        
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) + ">";
        });
}

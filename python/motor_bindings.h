#pragma once

#include "motor_bindings.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <typeindex>
#include <unordered_map>

namespace py = pybind11;

namespace nlm {
    
    class MotorModule : public BindingModule {
    public:
        std::string getName() const override { return "motor"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Motor system bindings for NLM (Action, ActionResult)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Motor class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // Action class
            builder.class_<Action>("Action", R"pbdoc(Action representation for motor output)pbdoc")
                .def(py::init<>())
                .def(py::init<ActionType>(), py::arg("type"))
                .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
                .def("getType", &Action::getType)
                .def("setType", &Action::setType, py::arg("type"))
                .def("getParameters", &Action::getParameters)
                .def("setParameters", &Action::setParameters, py::arg("params"))
                .def("getName", &Action::getName)
                .def("clone", &Action::clone);
            
            // ActionResult class
            builder.class_<ActionResult>("ActionResult", R"pbdoc(Action result from world)pbdoc")
                .def(py::init<>())
                .def(py::init<float, bool, std::string>(),
                     py::arg("reward"), py::arg("success"), py::arg("message") = "")
                .def_readwrite("reward", &ActionResult::reward)
                .def_readwrite("success", &ActionResult::success)
                .def_readwrite("message", &ActionResult::message);
            
            // AgentBody class
            builder.class_<AgentBody>("AgentBody", R"pbdoc(Agent body state)pbdoc")
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
            
            // SensoryPercept class
            builder.class_<SensoryPercept>("SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
                .def(py::init<>())
                .def("getVision", &SensoryPercept::getVision)
                .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
                .def("getVisionWidth", &SensoryPercept::getVisionWidth)
                .def("getVisionHeight", &SensoryPercept::getVisionHeight)
                .def("getTouch", &SensoryPercept::getTouch)
                .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
                .def("getInternal", &SensoryPercept::getInternal)
                .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
                .def("getProprioception", &SensoryPercept::getProprioception)
                .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
                .def("getAudio", &SensoryPercept::getAudio)
                .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
                .def("getAllSignals", &SensoryPercept::getAllSignals)
                .def("getTimestamp", &SensoryPercept::getTimestamp)
                .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));
            
            // Context manager support for motor actions
            builder.class_<Action>("ActionContext", R"pbdoc(Context manager for Action operations)pbdoc")
                .def(py::init<Action>())
                .def("enter", [](Action& self) { return self; })
                .def("exit", [](Action&, const py::args& args) { return true; });
            
            // Action factory functions
            m.def("createAction", [](ActionType type) {
                return std::make_shared<Action>(type);
            }, py::arg("type"), "Create action with given type");
            
            m.def("createActionWithParams", [](ActionType type, const std::vector<float>& params) {
                return std::make_shared<Action>(type, params);
            }, py::arg("type"), py::arg("params"), "Create action with type and parameters");
            
            m.def("createActionFromName", [](const std::string& name) {
                // Find action type from name
                for (int i = 0; i < static_cast<int>(ActionType::Custom); ++i) {
                    ActionType actionType = static_cast<ActionType>(i);
                    if (Action(actionType).getName() == name) {
                        return std::make_shared<Action>(actionType);
                    }
                }
                return std::make_shared<Action>(ActionType::Custom);
            }, py::arg("name"), "Create action from name");
            
            // Action utilities
            m.def("action_to_string", [](const Action& action) {
                return action.getName();
            }, py::arg("action"), "Get action name as string");
            
            m.def("action_equal", [](const Action& a1, const Action& a2) {
                return a1.getType() == a2.getType() && a1.getParameters() == a2.getParameters();
            }, py::arg("a1"), py::arg("a2"), "Compare two actions");
            
            // Motor command utilities
            m.def("get_motor_command_name", [](MotorCommand cmd) {
                switch (cmd) {
                    case MotorCommand::MoveForward: return std::string("MoveForward");
                    case MotorCommand::MoveBackward: return std::string("MoveBackward");
                    case MotorCommand::TurnLeft: return std::string("TurnLeft");
                    case MotorCommand::TurnRight: return std::string("TurnRight");
                    case MotorCommand::LookLeft: return std::string("LookLeft");
                    case MotorCommand::LookRight: return std::string("LookRight");
                    case MotorCommand::Interact: return std::string("Interact");
                    case MotorCommand::Wait: return std::string("Wait");
                    default: return std::string("Unknown");
                }
            }, py::arg("cmd"), "Get motor command name as string");
            
            // Action result utilities
            m.def("is_successful_action", [](const ActionResult& result) {
                return result.success && result.reward > 0.0f;
            }, py::arg("result"), "Check if action was successful with positive reward");
            
            m.def("action_result_to_string", [](const ActionResult& result) {
                std::stringstream ss;
                ss << "ActionResult(reward=" << result.reward
                   << ", success=" << (result.success ? "true" : "false")
                   << ", message=\"" << result.message << "\")";
                return ss.str();
            }, py::arg("result"), "Get action result as string");
        }
        
        void initialize() override {
            // Initialize motor module
        }
        
        void shutdown() override {
            // Cleanup motor module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "action_factory" || feature == "action_utils" || 
                   feature == "motor_commands" || feature == "action_context" ||
                   feature == "agent_body" || feature == "action_parsing";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "action_factory") {
                return py::module_::import("nlm.motor").attr("action_factory");
            } else if (feature == "action_utils") {
                return py::module_::import("nlm.motor").attr("action_utils");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the motor module
    static MotorModule register_motor_module() {
        auto module = std::make_shared<MotorModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(motor_bindings, m) {
        static MotorModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Motor Python Bindings
            -------------------------
            
            This module provides Python bindings for the NLM motor system,
            including Action, ActionResult, AgentBody, and SensoryPercept classes.
            
            It provides motor action interfaces for controlling agent behavior,
            with context managers and action utilities.
            
            .. currentmodule:: motor_bindings
            
            Motor API
            ===========""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<MotorModule>(module));
    }
}
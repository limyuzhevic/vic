#pragma once

#include "agent_bindings.h"
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
    
    class AgentModule : public BindingModule {
    public:
        std::string getName() const override { return "agent"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Agent brain interface bindings for NLM (AgentBrain class)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // AgentBrain class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // AgentBrain class
            builder.class_<AgentBrain>("AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
                .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
                .def("initialize", &AgentBrain::initialize, py::arg("world"),
                     "Initialize with world")
                .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
                     "Get expected sensory input size")
                .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
                     "Get expected motor output size")
                .def("processSensoryInput", &AgentBrain::processSensoryInput,
                     py::arg("percept"),
                     "Process sensory percept and inject into brain")
                .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
                     "Decode brain motor activity into motor command")
                .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
                     py::arg("reward"), py::arg("predictedReward"),
                     "Apply reward-based neuromodulation")
                .def("updateDevelopment", &AgentBrain::updateDevelopment,
                     py::arg("timestep"),
                     "Update development system")
                .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
                     "Get current developmental stage")
                .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
                     "Get current neuromodulation level")
                .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
                     "Get curiosity level")
                .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
                     "Get novelty level")
                .def("getPredictionError", &AgentBrain::getPredictionError,
                     "Get prediction error")
                .def("reset", &AgentBrain::reset,
                     "Reset agent for new episode")
                .def("getBrain", &AgentBrain::getBrain,
                     py::return_value_policy::reference_internal,
                     "Get the underlying brain")
                .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
                     py::arg("enable"))
                .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
                     py::arg("enable"))
                .def("enableDevelopment", &AgentBrain::enableDevelopment,
                     py::arg("enable"))
                .def("enableCuriosity", &AgentBrain::enableCuriosity,
                     py::arg("enable"))
                .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
                .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
                .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
                .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);
            
            // Context manager support for agent brain operations
            builder.class_<AgentBrain>("AgentBrainContext", R"pbdoc(Context manager for AgentBrain operations)pbdoc")
                .def(py::init<AgentBrain>())
                .def("enter", [](AgentBrain& self) { return self; })
                .def("exit", [](AgentBrain&, const py::args& args) { return true; });
            
            // Agent utilities
            m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
                return std::make_shared<AgentBrain>(brain);
            }, py::arg("brain"), "Create a new agent brain interface");
            
            // Agent body factory
            m.def("create_agent_body", []() {
                return std::make_shared<AgentBody>();
            }, "Create agent body state");
            
            m.def("create_sensory_percept", []() {
                return std::make_shared<SensoryPercept>();
            }, "Create sensory percept");
            
            // Agent operation utilities
            m.def("agent_brain_to_string", [](const AgentBrain& agent) {
                std::stringstream ss;
                ss << "AgentBrain(stage=" << static_cast<int>(agent.getDevelopmentalStage())
                   << ", reward_mod=" << (agent.isRewardModulationEnabled() ? "enabled" : "disabled")
                   << ", curiosity=" << agent.getCuriosityLevel()
                   << ", novelty=" << agent.getNoveltyLevel() << ")";
                return ss.str();
            }, py::arg("agent"), "Get agent brain state as string");
            
            // Agent validation
            m.def("validate_agent_brain_state", [](const AgentBrain& agent) {
                // Implementation would validate agent brain state consistency
                return true;
            }, py::arg("agent"), "Validate agent brain state consistency");
            
            // Agent action utilities
            m.def("get_agent_action_size", [](const AgentBrain& agent) {
                return agent.getMotorOutputSize();
            }, py::arg("agent"), "Get agent's expected motor output size");
            
            m.def("get_agent_sensory_size", [](const AgentBrain& agent) {
                return agent.getSensoryInputSize();
            }, py::arg("agent"), "Get agent's expected sensory input size");
            
            // Agent development utilities
            m.def("get_developmental_stage_name", [](DevelopmentalStage stage) {
                switch (stage) {
                    case DevelopmentalStage::Initial: return std::string("Initial");
                    case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
                    case DevelopmentalStage::Maturation: return std::string("Maturation");
                    case DevelopmentalStage::Adult: return std::string("Adult");
                    case DevelopmentalStage::Aging: return std::string("Aging");
                    default: return std::string("Unknown");
                }
            }, py::arg("stage"), "Get developmental stage name as string");
            
            // Agent neuromodulation utilities
            m.def("get_neuromodulation_summary", [](const AgentBrain& agent) {
                std::stringstream ss;
                ss << "Neuromodulation(level=" << agent.getNeuromodulationLevel()
                   << ", curiosity=" << agent.getCuriosityLevel()
                   << ", novelty=" << agent.getNoveltyLevel()
                   << ", prediction_error=" << agent.getPredictionError() << ")";
                return ss.str();
            }, py::arg("agent"), "Get neuromodulation summary");
        }
        
        void initialize() override {
            // Initialize agent module
        }
        
        void shutdown() override {
            // Cleanup agent module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "agent_factory" || feature == "agent_utils" || 
                   feature == "agent_body" || feature == "sensory_percept" ||
                   feature == "agent_context" || feature == "agent_state" ||
                   feature == "agent_development" || feature == "agent_neuromodulation";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "agent_factory") {
                return py::module_::import("nlm.agent").attr("agent_factory");
            } else if (feature == "agent_utils") {
                return py::module_::import("nlm.agent").attr("agent_utils");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the agent module
    static AgentModule register_agent_module() {
        auto module = std::make_shared<AgentModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(agent_bindings, m) {
        static AgentModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Agent Python Bindings
            ------------------------
            
            This module provides Python bindings for the NLM agent brain interface,
            including AgentBrain, AgentBody, and SensoryPercept classes.
            
            It provides agent brain interfaces for connecting NLM brains to worlds,
            with context managers and agent utilities.
            
            .. currentmodule:: agent_bindings
            
            Agent API
            ==========""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<AgentModule>(module));
    }
}
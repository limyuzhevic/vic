#pragma once

#include "brain_bindings.h"
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
    
    class BrainModule : public BindingModule {
    public:
        std::string getName() const override { return "brain"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Brain class bindings for NLM (central neural simulation brain)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Brain class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // Brain class with comprehensive bindings
            builder.class_<Brain>("Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
                .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
                .def("initialize", &Brain::initialize,
                     "Initialize the brain with configuration")
                .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
                     py::arg("currentStep"),
                     "Perform a simulation step")
                .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
                     py::arg("currentStep"), py::arg("currentTime"),
                     "Perform a simulation step with timestamp")
                .def("receiveSensoryInput", &Brain::receiveSensoryInput,
                     py::arg("input"),
                     "Inject sensory input into the brain")
                .def("injectCurrent", &Brain::injectCurrent,
                     py::arg("neuron"), py::arg("current"),
                     "Inject current into a specific neuron")
                .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
                     py::arg("type"), py::arg("current"),
                     "Inject current into all neurons of a specific type")
                .def("produceAction", &Brain::produceAction,
                     "Produce motor action based on neural activity")
                .def("reset", &Brain::reset,
                     "Reset brain state")
                .def("save", &Brain::save, py::arg("filepath"),
                     "Save brain state to file")
                .def("load", &Brain::load, py::arg("filepath"),
                     "Load brain state from file")
                .def("addRegion", &Brain::addRegion, py::arg("name") = "",
                     "Add a new neural region")
                .def("getRegion", &Brain::getRegion, py::arg("id"),
                     py::return_value_policy::reference_internal,
                     "Get a region by ID")
                .def("getRegionCount", &Brain::getRegionCount,
                     "Get the number of regions")
                .def("getRegionIds", &Brain::getRegionIds,
                     "Get all region IDs")
                .def("getRegions", &Brain::getRegions,
                     py::return_value_policy::reference_internal,
                     "Get all regions")
                .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
                     "Get total neuron count across all regions")
                .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
                     "Get total synapse count across all regions")
                .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
                     "Get count of active neurons")
                .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
                     "Get count of currently firing neurons")
                .def("getAverageFiringRate", &Brain::getAverageFiringRate,
                     "Get average firing rate across all neurons")
                .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
                     "Get excitation/inhibition balance ratio")
                .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
                     "Get total spike count")
                .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
                     "Get current developmental stage")
                .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
                     py::arg("stage"),
                     "Set developmental stage")
                .def("getConfig", &Brain::getConfig,
                     py::return_value_policy::reference_internal,
                     "Get the configuration")
                .def("logStatus", &Brain::logStatus,
                     "Log brain status");
            
            // Context manager support for brain operations
            builder.class_<Brain>("BrainContext", R"pbdoc(Context manager for Brain operations)pbdoc")
                .def(py::init<Brain>())
                .def("enter", [](Brain& self) { return self; })
                .def("exit", [](Brain&, const py::args& args) { return true; });
            
            // Brain utility functions
            m.def("createBrain", [](std::shared_ptr<Config> config) {
                return std::make_shared<Brain>(config);
            }, py::arg("config"), "Create a new brain with configuration");
            
            m.def("createBrainFromFile", [](const std::string& filepath) {
                auto config = std::make_shared<Config>();
                if (!config->loadFromFile(filepath)) {
                    throw ConfigError("Failed to load configuration from file: " + filepath);
                }
                return std::make_shared<Brain>(config);
            }, py::arg("filepath"), "Create brain from configuration file");
            
            // Brain simulation utilities
            m.def("simulate_brain_step", [](Brain& brain, SimulationStep step) {
                brain.step(step);
            }, py::arg("brain"), py::arg("step"), "Simulate one brain step");
            
            m.def("simulate_brain_step_with_time", [](Brain& brain, SimulationStep step, Timestamp time) {
                brain.step(step, time);
            }, py::arg("brain"), py::arg("step"), py::arg("time"), "Simulate brain step with timestamp");
            
            // Brain state utilities
            m.def("brain_to_string", [](const Brain& brain) {
                std::stringstream ss;
                ss << "Brain(state=" << brain.getTotalNeuronCount() << " neurons, "
                    << brain.getTotalSynapseCount() << " synapses, "
                    << brain.getActiveNeuronCount() << " active, "
                    << brain.getAverageFiringRate() << " avg_firing_rate)";
                return ss.str();
            }, py::arg("brain"), "Get brain state as string");
            
            // Brain validation and statistics
            m.def("validate_brain_state", [](const Brain& brain) {
                // Implementation would validate brain state consistency
                return true;
            }, py::arg("brain"), "Validate brain state consistency");
            
            m.def("brain_statistics", [](const Brain& brain) {
                // Implementation would compute brain statistics
                return std::map<std::string, double>{{ "firing_rate", brain.getAverageFiringRate() }};
            }, py::arg("brain"), "Get brain statistics");
            
            // Brain development utilities
            m.def("set_development_stage", [](Brain& brain, DevelopmentalStage stage) {
                brain.setDevelopmentalStage(stage);
            }, py::arg("brain"), py::arg("stage"), "Set brain developmental stage");
            
            m.def("get_development_stage_name", [](DevelopmentalStage stage) {
                switch (stage) {
                    case DevelopmentalStage::Initial: return std::string("Initial");
                    case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
                    case DevelopmentalStage::Maturation: return std::string("Maturation");
                    case DevelopmentalStage::Adult: return std::string("Adult");
                    case DevelopmentalStage::Aging: return std::string("Aging");
                    default: return std::string("Unknown");
                }
            }, py::arg("stage"), "Get developmental stage name as string");
            
            // Brain action utilities
            m.def("get_brain_action_count", [](const Brain& brain) {
                return brain.produceAction(); // This would return the action
            }, py::arg("brain"), "Get action produced by brain");
            
            m.def("brain_has_action", [](const Brain& brain) {
                // Implementation would check if brain has produced an action
                return false;
            }, py::arg("brain"), "Check if brain has produced an action");
        }
        
        void initialize() override {
            // Initialize brain module
        }
        
        void shutdown() override {
            // Cleanup brain module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "brain_factory" || feature == "brain_simulation" || 
                   feature == "brain_state" || feature == "brain_development" ||
                   feature == "brain_action" || feature == "brain_context" ||
                   feature == "brain_statistics" || feature == "brain_validation";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "brain_factory") {
                return py::module_::import("nlm.brain").attr("brain_factory");
            } else if (feature == "brain_simulation") {
                return py::module_::import("nlm.brain").attr("brain_simulation");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the brain module
    static BrainModule register_brain_module() {
        auto module = std::make_shared<BrainModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(brain_bindings, m) {
        static BrainModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Brain Python Bindings
            -------------------------
            
            This module provides Python bindings for the NLM brain class,
            which is the central neural simulation engine.
            
            It provides comprehensive brain control interfaces for neural simulation,
            including step-by-step simulation, sensory input processing,
            action production, and state management.
            
            .. currentmodule:: brain_bindings
            
            Brain API
            ===========""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<BrainModule>(module));
    }
}
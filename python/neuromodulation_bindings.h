#pragma once

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
    
    class NeuromodulationModule : public BindingModule {
    public:
        std::string getName() const override { return "neuromodulation"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Neuromodulation system bindings for NLM (Reward, PredictionError, Novelty, etc.)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Neuromodulation class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // Neuromodulator base class
            builder.class_<Neuromodulator>("Neuromodulator", R"pbdoc(Base class for neuromodulators)pbdoc")
                .def(py::init<>())
                .def("getType", &Neuromodulator::getType, "Get neuromodulator type")
                .def("getLevel", &Neuromodulator::getLevel, "Get current modulation level")
                .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set modulation level")
                .def("getInfluence", &Neuromodulator::getInfluence, "Get influence on neural activity")
                .def("update", &Neuromodulator::update, py::arg("timestep"), "Update neuromodulator state")
                .def("reset", &Neuromodulator::reset, "Reset neuromodulator state");
            
            // Reward neuromodulator
            builder.class_<Reward, Neuromodulator>("Reward", R"pbdoc(Reward neuromodulator)pbdoc")
                .def(py::init<>())
                .def("apply", &Reward::apply, py::arg("reward"), "Apply reward signal");
            
            // PredictionError neuromodulator
            builder.class_<PredictionError, Neuromodulator>("PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
                .def(py::init<>())
                .def("apply", &PredictionError::apply, py::arg("error"), "Apply prediction error");
            
            // Novelty neuromodulator
            builder.class_<Novelty, Neuromodulator>("Novelty", R"pbdoc(Novelty detection neuromodulator)pbdoc")
                .def(py::init<>())
                .def("update", &Novelty::update, py::arg("input"), "Update with new input");
            
            // Curiosity neuromodulator
            builder.class_<Curiosity, Neuromodulator>("Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
                .def(py::init<>())
                .def("getExplorationScore", &Curiosity::getExplorationScore,
                     "Get exploration score based on unexpected outcomes");
            
            // Dopamine neuromodulator
            builder.class_<Dopamine, Neuromodulator>("Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
                .def(py::init<>())
                .def("getLearningRate", &Dopamine::getLearningRate,
                     "Get learning rate contribution");
            
            // Serotonin neuromodulator
            builder.class_<Serotonin, Neuromodulator>("Serotonin", R"pbdoc(Serotonin neuromodulator)pbdoc")
                .def(py::init<>())
                .def("getArousalLevel", &Serotonin::getArousalLevel,
                     "Get arousal level contribution");
            
            // Norepinephrine neuromodulator
            builder.class_<Norepinephrine, Neuromodulator>("Norepinephrine", R"pbdoc(Norepinephrine neuromodulator)pbdoc")
                .def(py::init<>())
                .def("getAttentionLevel", &Norepinephrine::getAttentionLevel,
                     "Get attention level contribution");
            
            // Acetylcholine neuromodulator
            builder.class_<Acetylcholine, Neuromodulator>("Acetylcholine", R"pbdoc(Acetylcholine neuromodulator)pbdoc")
                .def(py::init<>())
                .def("getMemoryModulation", &Acetylcholine::getMemoryModulation,
                     "Get memory modulation contribution");
            
            // Context manager support for neuromodulators
            builder.class_<Reward>("RewardContext", R"pbdoc(Context manager for Reward neuromodulator)pbdoc")
                .def(py::init<Reward>())
                .def("enter", [](Reward& self) { return self; })
                .def("exit", [](Reward&, const py::args& args) { return true; });
            
            builder.class_<PredictionError>("PredictionErrorContext", R"pbdoc(Context manager for PredictionError neuromodulator)pbdoc")
                .def(py::init<PredictionError>())
                .def("enter", [](PredictionError& self) { return self; })
                .def("exit", [](PredictionError&, const py::args& args) { return true; });
            
            // Neuromodulation utility functions
            m.def("create_reward_modulator", []() {
                return std::make_shared<Reward>();
            }, "Create a new reward neuromodulator");
            
            m.def("create_prediction_error_modulator", []() {
                return std::make_shared<PredictionError>();
            }, "Create a new prediction error neuromodulator");
            
            m.def("create_novelty_detector", []() {
                return std::make_shared<Novelty>();
            }, "Create a new novelty detector");
            
            m.def("create_curiosity_modulator", []() {
                return std::make_shared<Curiosity>();
            }, "Create a new curiosity modulator");
            
            // Neuromodulation system utilities
            m.def("get_neuromodulator_by_type", [](const std::string& type) {
                if (type == "reward") return std::make_shared<Reward>();
                else if (type == "prediction_error") return std::make_shared<PredictionError>();
                else if (type == "novelty") return std::make_shared<Novelty>();
                else if (type == "curiosity") return std::make_shared<Curiosity>();
                else if (type == "dopamine") return std::make_shared<Dopamine>();
                else if (type == "serotonin") return std::make_shared<Serotonin>();
                else if (type == "norepinephrine") return std::make_shared<Norepinephrine>();
                else if (type == "acetylcholine") return std::make_shared<Acetylcholine>();
                throw std::invalid_argument("Unknown neuromodulator type: " + type);
            }, py::arg("type"), "Get neuromodulator by type");
            
            // Neuromodulation level utilities
            m.def("calculate_total_neuromodulation", [](const std::vector<std::shared_ptr<Neuromodulator>>& modulators) {
                double total = 0.0;
                for (const auto& modulator : modulators) {
                    total += modulator->getLevel();
                }
                return total / modulators.size();
            }, py::arg("modulators"), "Calculate average neuromodulation level");
            
            m.def("get_neuromodulator_level_summary", [](const std::vector<std::shared_ptr<Neuromodulator>>& modulators) {
                std::map<std::string, double> levels;
                for (const auto& modulator : modulators) {
                    std::string typeName;
                    if (dynamic_cast<Reward*>(modulator.get())) typeName = "reward";
                    else if (dynamic_cast<PredictionError*>(modulator.get())) typeName = "prediction_error";
                    else if (dynamic_cast<Novelty*>(modulator.get())) typeName = "novelty";
                    else if (dynamic_cast<Curiosity*>(modulator.get())) typeName = "curiosity";
                    else if (dynamic_cast<Dopamine*>(modulator.get())) typeName = "dopamine";
                    else if (dynamic_cast<Serotonin*>(modulator.get())) typeName = "serotonin";
                    else if (dynamic_cast<Norepinephrine*>(modulator.get())) typeName = "norepinephrine";
                    else if (dynamic_cast<Acetylcholine*>(modulator.get())) typeName = "acetylcholine";
                    
                    levels[typeName] = modulator->getLevel();
                }
                return levels;
            }, py::arg("modulators"), "Get neuromodulator level summary");
            
            // Neuromodulation validation
            m.def("validate_neuromodulation_state", [](const std::vector<std::shared_ptr<Neuromodulator>>& modulators) {
                for (const auto& modulator : modulators) {
                    if (modulator->getLevel() < 0.0 || modulator->getLevel() > 1.0) {
                        return false;
                    }
                }
                return true;
            }, py::arg("modulators"), "Validate neuromodulation state");
            
            // Neuromodulation simulation utilities
            m.def("update_neuromodulators", [](std::vector<std::shared_ptr<Neuromodulator>>& modulators, double timestep) {
                for (auto& modulator : modulators) {
                    modulator->update(timestep);
                }
            }, py::arg("modulators"), py::arg("timestep"), "Update all neuromodulators");
            
            m.def("apply_neuromodulation", [](const std::vector<std::shared_ptr<Neuromodulator>>& modulators, const std::map<std::string, double>& inputs) {
                // Implementation would apply neuromodulators based on inputs
                return std::map<std::string, double>();
            }, py::arg("modulators"), py::arg("inputs"), "Apply neuromodulators to neural system");
        }
        
        void initialize() override {
            // Initialize neuromodulation module
        }
        
        void shutdown() override {
            // Cleanup neuromodulation module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "neuromodulator_factory" || feature == "neuromodulator_utils" || 
                   feature == "neuromodulator_levels" || feature == "neuromodulator_simulation" ||
                   feature == "neuromodulator_validation" || feature == "neuromodulator_context";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "neuromodulator_factory") {
                return py::module_::import("nlm.neuromodulation").attr("neuromodulator_factory");
            } else if (feature == "neuromodulator_utils") {
                return py::module_::import("nlm.neuromodulation").attr("neuromodulator_utils");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the neuromodulation module
    static NeuromodulationModule register_neuromodulation_module() {
        auto module = std::make_shared<NeuromodulationModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(neuromodulation_bindings, m) {
        static NeuromodulationModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Neuromodulation Python Bindings
            ------------------------------------
            
            This module provides Python bindings for the NLM neuromodulation system,
            including Reward, PredictionError, Novelty, Curiosity, and other
            neuromodulators.
            
            It provides neuromodulation interfaces for controlling neural activity
            based on learned signals, with context managers and neuromodulation utilities.
            
            .. currentmodule:: neuromodulation_bindings
            
            Neuromodulation API
            =====================""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<NeuromodulationModule>(module));
    }
}
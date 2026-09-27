// pybind11_bindings.h - Enhanced Python bindings module structure
// Modular bindings system for NLM Python API

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Forward declarations
namespace nlm {
    class Config;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class Action;
    class WorldObject;
    class AgentBody;
    class ActionResult;
    class SensoryPercept;
    class SimpleWorld;
    class Brain;
    class AgentBrain;
    class Neuron;
    class Synapse;
    class NeuralPopulation;
    class NeuralRegion;
    class SpikeSystem;
    class DevelopmentSystem;
    class Synaptogenesis;
    class Pruning;
    class Maturation;
    class Neuromodulator;
    class Reward;
    class PredictionError;
    class Novelty;
    class Curiosity;
    class PlasticityRule;
    class Hebbian;
    class STDP;
    class StructuralPlasticity;
    class WorkingMemory;
    class EpisodicMemory;
    class SemanticMemory;
    class ProceduralMemory;
    class AssociativeMemory;
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class ConceptFormation;
    class NeuralPlanner;
    class NeuralDynamics;
    class PredictionSystem;
    class NeuralPrediction;
    class MotorSystem;
    class Environment;
    class Observation;
    class Experiment;
    class ExperimentRunner;
    class Metrics;
    class VisualizationInterface;
}

namespace py = pybind11;
namespace nlm {

// Helper function for enhanced Python API
template<typename T>
py::class_<T>& add_enhanced_class(py::module& m, const std::string& name, const std::string& docstring) {
    return py::class_<T>(m, name.c_str(), py::module_local())
        .def(py::init<>())
        .def("__repr__", [](const T& obj) {
            return "<" + name + " object>";
        });
}

// Helper for enum bindings
template<typename EnumType>
py::enum_<EnumType>& add_enhanced_enum(py::module& m, const std::string& name, const std::string& docstring) {
    return py::enum_<EnumType>(m, name.c_str(), py::arithmetic(), py::module_local())
        .export_values()
        .doc() = docstring;
}

// Enhanced constructor with documentation
template<typename T, typename... Args>
py::class_<T>& add_enhanced_constructor(py::class_<T>& cls, const std::string& params_doc = "", Args&&... args) {
    cls.def(py::init<Args>(), py::arg("...").noconvert(), params_doc);
    return cls;
}

// Property getter/setter helper
template<typename T, typename PropertyType, PropertyType T::*ptr>
py::class_<T>& add_enhanced_property(py::class_<T>& cls, const std::string& name) {
    cls.def_readwrite(name.c_str(), ptr);
    return cls;
}

// Method with enhanced documentation
template<typename T, typename Ret, typename... Args>
py::class_<T>& add_enhanced_method(py::class_<T>& cls, const std::string& name, 
                                 Ret (T::*)(Args...) const, const std::string& docstring = "") {
    cls.def(name.c_str(), (Ret(T::*)(Args...) const)ret, docstring.c_str());
    return cls;
}

// Overload for non-const methods
template<typename T, typename Ret, typename... Args>
py::class_<T>& add_enhanced_method(py::class_<T>& cls, const std::string& name,
                                 Ret (T::*)(Args...) , const std::string& docstring = "") {
    cls.def(name.c_str(), (Ret(T::*)(Args...))ret, docstring.c_str());
    return cls;
}

// Pythonic function wrapper
template<typename Ret, typename... Args>
py::class_<std::function<Ret(Args...)>> add_pythonic_function(py::module& m, const std::string& name) {
    return py::class_<std::function<Ret(Args...)>>(m, name.c_str(), py::module_local())
        .def(py::init<>())
        .def("__call__", &std::function<Ret(Args...)>::operator());
}

// Type checking and validation helper
inline bool validate_range(float value, float min, float max) {
    return value >= min && value <= max;
}

inline bool validate_positive(int value) {
    return value >= 0;
}

inline bool validate_nonempty(const std::string& str) {
    return !str.empty();
}

// Error handling for Python
inline void throw_py_error(const std::string& message) {
    throw py::value_error(message);
}

// Enhanced config validation
struct ConfigValidation {
    static bool validate_neuron_count(int count) {
        return validate_positive(count) && count > 0;
    }
    
    static bool validate_connection_probability(float prob) {
        return validate_range(prob, 0.0f, 1.0f);
    }
    
    static bool validate_timestep(double timestep) {
        return validate_positive((int)(timestep * 1000));
    }
};

// Factory functions for better Python API
inline std::shared_ptr<Config> create_config_with_validation(const std::string& config_file = "") {
    auto config = std::make_shared<Config>();
    if (!config_file.empty() && !config->loadFromFile(config_file)) {
        throw_py_error("Failed to load configuration from: " + config_file);
    }
    return config;
}

} // namespace nlm

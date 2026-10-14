// Python module exports for validation utilities
#ifndef NLM_VALIDATION_MODULE_H
#define NLM_VALIDATION_MODULE_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "validation_utils.h"

namespace py = pybind11;
namespace nlm {

// Validation Result Python bindings
void bind_validation_result(py::module& m) {
    py::class_<ValidationResult>(m, "ValidationResult", R"pbdoc(
        Result of validation operations with errors and warnings.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("is_valid", &ValidationResult::is_valid,
                      "Whether the validation passed")
        .def_readwrite("errors", &ValidationResult::errors,
                      "List of error messages")
        .def_readwrite("warnings", &ValidationResult::warnings,
                      "List of warning messages")
        .def("add_error", &ValidationResult::add_error,
             py::arg("error"), "Add an error message")
        .def("add_warning", &ValidationResult::add_warning,
             py::arg("warning"), "Add a warning message")
        .def("get_summary", &ValidationResult::get_summary,
             "Get a summary of the validation result")
        .def("__repr__", [](const ValidationResult& result) {
            return "ValidationResult(valid=" + std::to_string(result.is_valid) +
                   ", errors=" + std::to_string(result.errors.size()) +
                   ", warnings=" + std::to_string(result.warnings.size()) + ")";
        }, "String representation of ValidationResult");
}

// ConfigValidator Python bindings
void bind_config_validator(py::module& m) {
    py::class_<ConfigValidator> config_validator(m, "ConfigValidator", R"pbdoc(
        Utility class for validating NLM configurations.
    )pbdoc");
    
    m.def("validate_config", &ConfigValidator::validate,
         py::arg("config"), "Validate a single configuration");
    
    m.def("validate_config_system", &ConfigValidator::validate_all,
         py::arg("configs"), "Validate multiple configurations");
}

// BrainValidator Python bindings
void bind_brain_validator(py::module& m) {
    py::class_<BrainValidator> brain_validator(m, "BrainValidator", R"pbdoc(
        Utility class for validating NLM brains.
    )pbdoc");
    
    m.def("validate_brain", &BrainValidator::validate,
         py::arg("brain"), "Validate a brain");
}

// AgentBrainValidator Python bindings
void bind_agent_brain_validator(py::module& m) {
    py::class_<AgentBrainValidator> agent_brain_validator(m, "AgentBrainValidator", R"pbdoc(
        Utility class for validating NLM agent brains.
    )pbdoc");
    
    m.def("validate_agent_brain", &AgentBrainValidator::validate,
         py::arg("agent"), "Validate an agent brain");
}

// WorldValidator Python bindings
void bind_world_validator(py::module& m) {
    py::class_<WorldValidator> world_validator(m, "WorldValidator", R"pbdoc(
        Utility class for validating NLM worlds.
    )pbdoc");
    
    m.def("validate_world", &WorldValidator::validate,
         py::arg("world"), "Validate a world");
}

// SensoryPerceptValidator Python bindings
void bind_sensory_percept_validator(py::module& m) {
    py::class_<SensoryPerceptValidator> sensory_percept_validator(m, "SensoryPerceptValidator", R"pbdoc(
        Utility class for validating sensory percepts.
    )pbdoc");
    
    m.def("validate_sensory_percept", &SensoryPerceptValidator::validate,
         py::arg("percept"), "Validate a sensory percept");
}

// ValidationUtils Python bindings
void bind_validation_utils(py::module& m) {
    py::class_<ValidationUtils> validation_utils(m, "ValidationUtils", R"pbdoc(
        Comprehensive validation utilities for the entire NLM system.
    )pbdoc");
    
    m.def("validate_system", &ValidationUtils::validate_brain_system,
         py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("percept"),
         "Validate the complete NLM system");
    
    m.def("validate_system_advanced", [](const Brain& brain, const AgentBrain& agent, const SimpleWorld& world, const SensoryPercept& percept) {
        auto result = ValidationUtils::validate_brain_system(brain, agent, world, percept);
        return result;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("percept"),
         "Validate the complete NLM system (advanced version)");
    
    m.def("validation_result_to_string", &ValidationUtils::validation_result_to_string,
         py::arg("result"), "Convert a ValidationResult to a string representation");
    
    m.def("print_validation_result", [](const ValidationResult& result) {
        std::cout << ValidationUtils::validation_result_to_string(result) << std::endl;
    }, py::arg("result"), "Print validation result to console");
}

// Enhanced validation module binding
PYBIND11_MODULE(validation, m) {
    m.doc() = R"pbdoc(
        Enhanced validation utilities for NLM (Neural Learning Machine) Python bindings.
        
        This module provides comprehensive validation and error checking utilities
        for NLM configurations, brains, agents, worlds, and sensory percepts.
        
        Key Features:
        - Configuration validation with range checking
        - Brain state validation with consistency checks
        - Agent validation with configuration analysis
        - World validation with boundary and physics checks
        - SensoryPercept validation with data quality checks
        - Comprehensive system validation
        - Detailed error and warning reporting
        - String representation utilities
    )pbdoc";
    
    // Bind validation result
    bind_validation_result(m);
    
    // Bind validators
    bind_config_validator(m);
    bind_brain_validator(m);
    bind_agent_brain_validator(m);
    bind_world_validator(m);
    bind_sensory_percept_validator(m);
    bind_validation_utils(m);
    
    // Version information
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced validation utilities for NLM Python bindings";
}

} // namespace nlm

#endif // NLM_VALIDATION_MODULE_H
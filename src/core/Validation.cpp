#include "Validation.hpp"

namespace nlm {

void registerValidationClasses(py::module& m) {
    // Register custom exception classes
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<VisionError>(m, "VisionError");
    
    // Register ValidationResult
    py::class_<ValidationResult>(m, "ValidationResult", 
        "Result of validation operations with errors and warnings")
        .def(py::init<>())
        .def_readwrite("isValid", &ValidationResult::isValid)
        .def_readwrite("errors", &ValidationResult::errors)
        .def_readwrite("warnings", &ValidationResult::warnings)
        .def("hasErrors", &ValidationResult::hasErrors)
        .def("hasWarnings", &ValidationResult::hasWarnings)
        .def("getErrorSummary", &ValidationResult::getErrorSummary)
        .def("__repr__", [](const ValidationResult& result) {
            return "ValidationResult(isValid=" + std::to_string(result.isValid) +
                   ", errors=" + std::to_string(result.errors.size()) +
                   ", warnings=" + std::to_string(result.warnings.size()) + ")";
        });
    
    // Register ParameterValidator
    py::class_<ParameterValidator> validator_class(m, "ParameterValidator",
        "Utility class for validating parameters");
    
    validator_class.def_static("inferType", &ParameterValidator::inferType,
        py::arg("value"),
        "Infer parameter type from string value");
    
    validator_class.def_static("validateParameter", 
        py::overload_cast<const std::string&, ParameterValidator::ParameterType, const py::dict&>(
            &ParameterValidator::validateParameter),
        py::arg("value"),
        py::arg("expected_type"),
        py::arg("constraints") = py::dict(),
        "Validate a parameter against expected type and constraints");
    
    // Register MemoryMonitor
    py::class_<MemoryMonitor> monitor_class(m, "MemoryMonitor",
        "Utility class for monitoring memory usage");
    
    monitor_class.def_static("getCurrentUsage", &MemoryMonitor::getCurrentUsage,
        "Get current memory usage information");
    
    // Register MemoryHealthChecker
    py::class_<MemoryHealthChecker> health_checker_class(m, "MemoryHealthChecker",
        "Utility class for checking brain memory health");
    
    health_checker_class.def_static("checkBrainHealth", &MemoryHealthChecker::checkBrainHealth,
        py::arg("brain"),
        "Check brain memory health and return a HealthReport");
    
    // Register MemoryHealthChecker::HealthReport
    py::class_<MemoryHealthChecker::HealthReport>(m, "HealthReport")
        .def_readwrite("isHealthy", &MemoryHealthChecker::HealthReport::isHealthy)
        .def_readwrite("memoryUsage", &MemoryHealthChecker::HealthReport::memoryUsage)
        .def_readwrite("recommendedCapacity", &MemoryHealthChecker::HealthReport::recommendedCapacity)
        .def_readwrite("warningMessages", &MemoryHealthChecker::HealthReport::warningMessages)
        .def_readwrite("errorMessages", &MemoryHealthChecker::HealthReport::errorMessages);
    
    // Register MemoryManager
    py::class_<MemoryManager> memory_manager_class(m, "MemoryManager",
        "Utility class for memory management operations");
    
    memory_manager_class.def_static("cleanupUnusedNeurons", &MemoryManager::cleanupUnusedNeurons,
        py::arg("brain"),
        py::arg("unused_threshold") = 0.8,
        "Clean up unused neurons and synapses to optimize memory usage");
    
    memory_manager_class.def_static("optimizeConnectivity", &MemoryManager::optimizeConnectivity,
        py::arg("brain"),
        py::arg("max_connectivity") = 0.3f,
        "Optimize neural connectivity to stay within specified limits");
}

} // namespace nlm

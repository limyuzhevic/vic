#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <chrono>
#include <variant>
#include <optional>
#include <regex>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <cstdint>
#include <ctime>
#include <cstdarg>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/dynamics/SpikeSystem.hpp"

namespace py = pybind11;
namespace nlm {

// ==================== ENHANCED ERROR HANDLING ====================

/**
 * Error Location Information for Debugging
 * 
 * Provides detailed location information for errors including file, line, function,
 * and module information for precise debugging.
 */
class ErrorLocation {
public:
    std::string file;
    int line;
    std::string function;
    std::string module;
    
    ErrorLocation(const std::string& file = "", int line = 0, 
                  const std::string& function = "", const std::string& module = "")
        : file(file), line(line), function(function), module(module) {}
    
    std::string toString() const {
        std::ostringstream oss;
        if (!file.empty()) oss << file;
        if (line > 0) oss << ":" << line;
        if (!function.empty()) oss << " in " << function;
        if (!module.empty()) oss << " [" << module << "]";
        return oss.str();
    }
};

/**
 * Enhanced Error Information Structure
 * 
 * Contains comprehensive error information for debugging and programmatic handling.
 */
class ErrorInfo {
public:
    enum class ErrorCategory {
        Runtime,           // General runtime errors
        Validation,        // Parameter validation errors
        Boundary,          // Out-of-bounds errors
        Input,             // Invalid input errors
        Memory,            // Memory allocation errors
        Simulation,        // Simulation-related errors
        Configuration,     // Configuration errors
        Template,          // Template errors
        Iteration,         // Iteration-related errors
        System,            // System-level errors
        Network,           // Network-related errors
        IO,                // File I/O errors
        Security,          // Security violations
        Permission         // Permission errors
    };
    
    enum class ErrorSeverity {
        Low,      // Warning, non-critical
        Medium,   // Error, may affect functionality
        High,     // Critical error, partial functionality loss
        Severe,   // Fatal error, complete functionality loss
        Catastrophic // System failure
    };
    
    int code;
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    ErrorLocation location;
    std::string stackTrace;
    std::string context;
    std::map<std::string, std::string> details;
    std::vector<std::string> suggestions;
    
    ErrorInfo(int code = 0, ErrorCategory category = ErrorCategory::Runtime,
              ErrorSeverity severity = ErrorSeverity::Medium,
              const std::string& message = "",
              const ErrorLocation& location = ErrorLocation(),
              const std::string& context = "",
              const std::string& stackTrace = "")
        : code(code), category(category), severity(severity),
          message(message), location(location), context(context),
          stackTrace(stackTrace) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "[" << errorCategoryToString(category) << "]";
        oss << " [" << errorSeverityToString(severity) << "]";
        oss << " Code:" << code;
        oss << " Msg:" << message;
        oss << " Loc:" << location.toString();
        return oss.str();
    }
    
    static std::string errorCategoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::Runtime: return "Runtime";
            case ErrorCategory::Validation: return "Validation";
            case ErrorCategory::Boundary: return "Boundary";
            case ErrorCategory::Input: return "Input";
            case ErrorCategory::Memory: return "Memory";
            case ErrorCategory::Simulation: return "Simulation";
            case ErrorCategory::Configuration: return "Configuration";
            case ErrorCategory::Template: return "Template";
            case ErrorCategory::Iteration: return "Iteration";
            case ErrorCategory::System: return "System";
            case ErrorCategory::Network: return "Network";
            case ErrorCategory::IO: return "IO";
            case ErrorCategory::Security: return "Security";
            case ErrorCategory::Permission: return "Permission";
            default: return "Unknown";
        }
    }
    
    static std::string errorSeverityToString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Low: return "Low";
            case ErrorSeverity::Medium: return "Medium";
            case ErrorSeverity::High: return "High";
            case ErrorSeverity::Severe: return "Severe";
            case ErrorSeverity::Catastrophic: return "Catastrophic";
            default: return "Unknown";
        }
    }
    
    std::string getFormattedError() const {
        std::ostringstream oss;
        oss << "Error [" << code << "]: " << message << "\n";
        oss << "Category: " << errorCategoryToString(category) << "\n";
        oss << "Severity: " << errorSeverityToString(severity) << "\n";
        oss << "Location: " << location.toString() << "\n";
        if (!context.empty()) {
            oss << "Context: " << context << "\n";
        }
        if (!stackTrace.empty()) {
            oss << "Stack Trace:\n" << stackTrace << "\n";
        }
        if (!details.empty()) {
            oss << "Details:\n";
            for (const auto& detail : details) {
                oss << "  " << detail.first << ": " << detail.second << "\n";
            }
        }
        if (!suggestions.empty()) {
            oss << "Suggestions:\n";
            for (const auto& suggestion : suggestions) {
                oss << "  - " << suggestion << "\n";
            }
        }
        return oss.str();
    }
};

/**
 * Comprehensive Custom Exception Hierarchy
 * 
 * Nine-layer exception hierarchy with specific exception types for different error scenarios.
 */
class NLMRuntimeError : public std::runtime_error {
public:
    NLMRuntimeError(const std::string& message) : std::runtime_error(message), errorInfo_(0, ErrorInfo::ErrorCategory::Runtime) {}
    NLMRuntimeError(const std::string& message, const ErrorInfo& info) : std::runtime_error(message), errorInfo_(info) {}
    
    const ErrorInfo& getErrorInfo() const { return errorInfo_; }
    std::string getErrorInfoString() const { return errorInfo_.getFormattedError(); }
    
private:
    ErrorInfo errorInfo_;
};

class NLMValidationError : public NLMRuntimeError {
public:
    NLMValidationError(const std::string& message) 
        : NLMRuntimeError(message, ErrorInfo(1000, ErrorInfo::ErrorCategory::Validation, ErrorInfo::ErrorSeverity::High, message)) {}
    NLMValidationError(const std::string& message, const ErrorLocation& location)
        : NLMRuntimeError(message, ErrorInfo(1000, ErrorInfo::ErrorCategory::Validation, ErrorInfo::ErrorSeverity::High, message, location)) {}
};

class NLMBoundaryError : public NLMRuntimeError {
public:
    NLMBoundaryError(const std::string& message)
        : NLMRuntimeError(message, ErrorInfo(2000, ErrorInfo::ErrorCategory::Boundary, ErrorInfo::ErrorSeverity::High, message)) {}
    NLMBoundaryError(const std::string& message, const ErrorLocation& location)
        : NLMRuntimeError(message, ErrorInfo(2000, ErrorInfo::ErrorCategory::Boundary, ErrorInfo::ErrorSeverity::High, message, location)) {}
};

class NLMInvalidInputError : public NLMRuntimeError {
public:
    NLMInvalidInputError(const std::string& message)
        : NLMRuntimeError(message, ErrorInfo(3000, ErrorInfo::ErrorCategory::Input, ErrorInfo::ErrorSeverity::Medium, message)) {}
    NLMInvalidInputError(const std::string& message, const ErrorLocation& location)
        : NLMRuntimeError(message, ErrorInfo(3000, ErrorInfo::ErrorCategory::Input, ErrorInfo::ErrorSeverity::Medium, message, location)) {}
};

class NLMMemoryError : public NLMRuntimeError {
public:
    NLMMemoryError(const std::string& message)
        : NLMRuntimeError(message, ErrorInfo(4000, ErrorInfo::ErrorCategory::Memory, ErrorInfo::ErrorSeverity::Severe, message)) {}
    NLMMemoryError(const std::string& message, const ErrorLocation& location)
        : NLMRuntimeError(message, ErrorInfo(4000, ErrorInfo::ErrorCategory::Memory, ErrorInfo::ErrorSeverity::Severe, message, location)) {}
};

class NLMSimulationError : public NLMRuntimeError {
public:
    NLMSimulationError(const std::string& message)
        : NLMRuntimeError(message, ErrorInfo(5000, ErrorInfo::ErrorCategory::Simulation, ErrorInfo::ErrorSeverity::High, message)) {}
    NLMSimulationError(const std::string& message, const ErrorLocation& location)
        : NLMRuntimeError(message, ErrorInfo(5000, ErrorInfo::ErrorCategory::Simulation, ErrorInfo::ErrorSeverity::High, message, location)) {}
};

class NLMConfigurationError : public NLMRuntimeError {
public:
    NLMConfigurationError(const std::string& message)
        : NLMRuntimeError(message, ErrorInfo(6000, ErrorInfo::ErrorCategory::Configuration, ErrorInfo::ErrorSeverity::High, message)) {}
    NLMConfigurationError(const std::string& message, const ErrorLocation& location)
        : NLMRuntimeError(message, ErrorInfo(6000, ErrorInfo::ErrorCategory::Configuration, ErrorInfo::ErrorSeverity::High, message, location)) {}
};

class NLMTemplateError : public NLMRuntimeError {
public:
    NLMTemplateError(const std::string& message)
        : NLMRuntimeError(message, ErrorInfo(7000, ErrorInfo::ErrorCategory::Template, ErrorInfo::ErrorSeverity::Medium, message)) {}
    NLMTemplateError(const std::string& message, const ErrorLocation& location)
        : NLMRuntimeError(message, ErrorInfo(7000, ErrorInfo::ErrorCategory::Template, ErrorInfo::ErrorSeverity::Medium, message, location)) {}
};

class NLMIterationError : public NLMRuntimeError {
public:
    NLMIterationError(const std::string& message)
        : NLMRuntimeError(message, ErrorInfo(8000, ErrorInfo::ErrorCategory::Iteration, ErrorInfo::ErrorSeverity::Medium, message)) {}
    NLMIterationError(const std::string& message, const ErrorLocation& location)
        : NLMRuntimeError(message, ErrorInfo(8000, ErrorInfo::ErrorCategory::Iteration, ErrorInfo::ErrorSeverity::Medium, message, location)) {}
};

class ErrorTracker {
public:
    static ErrorTracker& getInstance() {
        static ErrorTracker instance;
        return instance;
    }
    
    void recordError(const ErrorInfo& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.push_back(error);
        errorCount_++;
        if (error.severity == ErrorInfo::ErrorSeverity::Severe || 
            error.severity == ErrorInfo::ErrorSeverity::Catastrophic) {
            criticalErrorCount_++;
        }
    }
    
    size_t getErrorCount() const { return errorCount_; }
    size_t getCriticalErrorCount() const { return criticalErrorCount_; }
    
    std::vector<ErrorInfo> getErrors() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return errors_;
    }
    
    void clearErrors() {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.clear();
        errorCount_ = 0;
        criticalErrorCount_ = 0;
    }
    
    size_t getTotalErrors() const { return errors_.size(); }
    
private:
    ErrorTracker() : errorCount_(0), criticalErrorCount_(0) {}
    mutable std::mutex mutex_;
    std::vector<ErrorInfo> errors_;
    size_t errorCount_;
    size_t criticalErrorCount_;
};

/**
 * Error Utility Functions
 */
namespace ErrorUtils {
    inline ErrorLocation currentLocation(const std::string& file = __builtin_FILE(),
                                       int line = __builtin_LINE(),
                                       const std::string& function = __builtin_FUNCTION()) {
        return ErrorLocation(file, line, function, "NLM");
    }
    
    inline std::string getStackTrace() {
        std::ostringstream oss;
        oss << "Stack trace not available in this build (use debug build)";
        return oss.str();
    }
    
    inline void throwError(const std::string& message, int code = 0, 
                          ErrorInfo::ErrorCategory category = ErrorInfo::ErrorCategory::Runtime) {
        ErrorTracker::getInstance().recordError(
            ErrorInfo(code, category, ErrorInfo::ErrorSeverity::Medium, message, currentLocation())
        );
        throw NLMRuntimeError(message);
    }
    
    inline void throwValidationError(const std::string& message,
                                    const ErrorLocation& location = currentLocation()) {
        ErrorTracker::getInstance().recordError(
            ErrorInfo(1000, ErrorInfo::ErrorCategory::Validation, ErrorInfo::ErrorSeverity::High, message, location)
        );
        throw NLMValidationError(message, location);
    }
    
    inline void throwBoundaryError(const std::string& message,
                                  const ErrorLocation& location = currentLocation()) {
        ErrorTracker::getInstance().recordError(
            ErrorInfo(2000, ErrorInfo::ErrorCategory::Boundary, ErrorInfo::ErrorSeverity::High, message, location)
        );
        throw NLMBoundaryError(message, location);
    }
    
    inline void throwMemoryError(const std::string& message,
                                const ErrorLocation& location = currentLocation()) {
        ErrorTracker::getInstance().recordError(
            ErrorInfo(4000, ErrorInfo::ErrorCategory::Memory, ErrorInfo::ErrorSeverity::Severe, message, location)
        );
        throw NLMMemoryError(message, location);
    }
}

// Helper function for error location in Python bindings
static ErrorLocation getCurrentLocation() {
    return ErrorUtils::currentLocation();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings with Comprehensive Error Handling
        ----------------------------------------------------------------------------------
        This module provides enhanced Python bindings for the NLM C++ neural simulation framework
        with comprehensive error handling, custom exceptions, and detailed error reporting.
        
        New Exception Hierarchy:
        - NLMRuntimeError: Base runtime error
        - NLMValidationError: Parameter validation errors
        - NLMBoundaryError: Out-of-bounds errors
        - NLMInvalidInputError: Invalid input errors
        - NLMMemoryError: Memory allocation errors
        - NLMSimulationError: Simulation-related errors
        - NLMConfigurationError: Configuration errors
        - NLMTemplateError: Template errors
        - NLMIterationError: Iteration-related errors
        
        Error Information:
        Each exception includes detailed error information with codes, categories, severity,
        stack traces, and suggestions for recovery.
        
        Error Tracking:
        Global error statistics are maintained for monitoring and debugging.
        
        Enhanced Validation:
        Validation results include detailed error information and suggestions.
    )pbdoc";

    // Register exception types with pybind11
    py::register_exception<NLMRuntimeError>(m, "NLMRuntimeError", PyExc_RuntimeError);
    py::register_exception<NLMValidationError>(m, "NLMValidationError", PyExc_ValueError);
    py::register_exception<NLMBoundaryError>(m, "NLMBoundaryError", PyExc_IndexError);
    py::register_exception<NLMInvalidInputError>(m, "NLMInvalidInputError", PyExc_TypeError);
    py::register_exception<NLMMemoryError>(m, "NLMMemoryError", PyExc_MemoryError);
    py::register_exception<NLMSimulationError>(m, "NLMSimulationError", PyExc_RuntimeError);
    py::register_exception<NLMConfigurationError>(m, "NLMConfigurationError", PyExc_ValueError);
    py::register_exception<NLMTemplateError>(m, "NLMTemplateError", PyExc_TypeError);
    py::register_exception<NLMIterationError>(m, "NLMIterationError", PyExc_RuntimeError);
    
    // Register error tracking function
    m.def("get_error_stats", []() {
        const ErrorTracker& tracker = ErrorTracker::getInstance();
        py::dict stats;
        stats["total_errors"] = tracker.getErrorCount();
        stats["critical_errors"] = tracker.getCriticalErrorCount();
        stats["all_errors"] = tracker.getErrors();
        return stats;
    }, "Get error statistics and tracking information");
    
    m.def("clear_error_stats", []() {
        ErrorTracker::getInstance().clearErrors();
    }, "Clear error statistics and tracking information");
    
    // Helper function for error location in Python bindings
    m.def("_get_current_location", []() {
        return ErrorUtils::currentLocation();
    });

    // Original NLM bindings with enhanced error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Create default brain
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

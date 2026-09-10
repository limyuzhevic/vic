// Enhanced error handling and diagnostics for NLM
// Provides comprehensive error reporting, exception hierarchy, and diagnostic tools

#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <functional>

namespace nlm {

// Exception hierarchy for NLM errors
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error(message) {}
    virtual const char* getType() const { return "NLMError"; }
};

class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message) : NLMError("ConfigError: " + message) {}
    virtual const char* getType() const { return "ConfigError"; }
};

class BrainError : public NLMError {
public:
    BrainError(const std::string& message) : NLMError("BrainError: " + message) {}
    virtual const char* getType() const { return "BrainError"; }
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message) : NLMError("MemoryError: " + message) {}
    virtual const char* getType() const { return "MemoryError"; }
};

class PlasticityError : public NLMError {
public:
    PlasticityError(const std::string& message) : NLMError("PlasticityError: " + message) {}
    virtual const char* getType() const { return "PlasticityError"; }
};

class NeuromodulationError : public NLMError {
public:
    NeuromodulationError(const std::string& message) : NLMError("NeuromodulationError: " + message) {}
    virtual const char* getType() const { return "NeuromodulationError"; }
};

// Enhanced configuration validation and error reporting
class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double safetyScore; // 0.0 to 1.0
        
        ValidationResult() : isValid(true), safetyScore(1.0) {}
    };
    
    static ValidationResult validate(const class Config& config);
    static ValidationResult validateForSafety(const class Config& config);
    static ValidationResult validateForPerformance(const class Config& config);
    static ValidationResult validateForLearning(const class Config& config);
    static std::string getRecommendations(const class Config& config);
    static std::string getSafetyAnalysis(const class Config& config);
};

// Error recovery and diagnostics for brain operations
class BrainDiagnostics {
public:
    struct BrainState {
        size_t neuronCount;
        size_t synapseCount;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        size_t totalSpikes;
        size_t pendingSpikeEvents;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t regionCount;
        std::vector<std::string> activeSystems;
        std::vector<float> systemHealthScores;
        
        BrainState() : neuronCount(0), synapseCount(0), activeNeuronCount(0),
                      firingNeuronCount(0), totalSpikes(0), pendingSpikeEvents(0),
                      averageFiringRate(0.0f), excitationInhibitionRatio(0.0f),
                      regionCount(0) {}
    };
    
    static BrainDiagnostics& getInstance();
    
    // Brain state management
    void setBrain(std::shared_ptr<class Brain> brain);
    std::shared_ptr<class Brain> getBrain();
    
    // State capture and analysis
    BrainState captureState() const;
    std::string analyzeState(const BrainState& state) const;
    std::vector<std::string> diagnoseIssues(const BrainState& state) const;
    
    // Error detection and recovery
    bool detectAbnormalities(const BrainState& state) const;
    std::vector<std::string> getRecoverySuggestions(const BrainState& state) const;
    bool attemptRecovery(const std::string& issue);
    
    // Monitoring and alerting
    void startMonitoring(double intervalSeconds = 1.0);
    void stopMonitoring();
    void setAlertThresholds(const std::unordered_map<std::string, double>& thresholds);
    bool isAlert(const std::string& metric) const;
    
    // Diagnostic logging
    void enableDiagnosticLogging(bool enable);
    void setDiagnosticLogLevel(int level);
    void logDiagnostic(const std::string& category, const std::string& message);
    std::vector<std::string> getDiagnosticLog() const;
    
    // Performance analysis
    struct PerformanceMetrics {
        double simulationTimePerStep;
        double memoryUsageMB;
        double cpuUsagePercent;
        size_t stepsPerSecond;
        size_t spikesPerStep;
        double stabilityScore; // 0.0 to 1.0
        double efficiencyScore; // 0.0 to 1.0
        
        PerformanceMetrics() : simulationTimePerStep(0.0), memoryUsageMB(0.0),
                              cpuUsagePercent(0.0), stepsPerSecond(0), spikesPerStep(0),
                              stabilityScore(0.0), efficiencyScore(0.0) {}
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    std::string analyzePerformance(const PerformanceMetrics& metrics) const;
    
    // Error handling and recovery
    template<typename TFunc>
    TFunc wrapWithErrorHandling(TFunc func, const std::string& operationName) {
        return [this, func, operationName](auto&&... args) -> decltype(func(std::forward<decltype(args)>(args)...)) {
            try {
                return func(std::forward<decltype(args)>(args)...);
            } catch (const NLMError& e) {
                logDiagnostic("ERROR", "Operation failed: " + std::string(operationName) + " - " + e.what());
                throw;
            } catch (const std::exception& e) {
                logDiagnostic("ERROR", "Unexpected error: " + std::string(operationName) + " - " + e.what());
                throw BrainError("Unexpected error in " + operationName + ": " + e.what());
            }
        };
    }
    
private:
    BrainDiagnostics();
    ~BrainDiagnostics();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced error reporting for plasticity operations
class PlasticityDiagnostics {
public:
    struct PlasticityEvent {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string type; // "STDP", "Hebbian", "Structural"
        std::string operation; // "LTP", "LTD", "Synaptogenesis", "Pruning"
        NeuronId preNeuron;
        NeuronId postNeuron;
        float weightChange;
        float plasticityFactor;
        float neuromodulationLevel;
        bool success;
        std::string errorMessage;
        
        PlasticityEvent() : weightChange(0.0f), plasticityFactor(0.0f), neuromodulationLevel(0.0f), success(false) {}
    };
    
    static PlasticityDiagnostics& getInstance();
    
    // Event recording and analysis
    void recordPlasticityEvent(const PlasticityEvent& event);
    std::vector<PlasticityEvent> getRecentEvents(size_t count = 100) const;
    std::vector<PlasticityEvent> getEventsByType(const std::string& type, size_t count = 100) const;
    
    // Pattern analysis
    std::string analyzePlasticityPatterns() const;
    std::vector<std::string> detectAnomalies() const;
    double calculatePlasticityStability() const;
    
    // Error detection and recovery suggestions
    std::vector<std::string> getRecoverySuggestions() const;
    bool shouldAttemptRecovery(const PlasticityEvent& event) const;
    
    // Statistics and metrics
    struct PlasticityStatistics {
        size_t totalEvents;
        size_t successfulEvents;
        size_t failedEvents;
        float averageWeightChange;
        float maxWeightChange;
        float minWeightChange;
        std::unordered_map<std::string, size_t> eventsByType;
        std::unordered_map<std::string, size_t> operationsByType;
        
        PlasticityStatistics() : totalEvents(0), successfulEvents(0), failedEvents(0),
                               averageWeightChange(0.0f), maxWeightChange(0.0f),
                               minWeightChange(0.0f) {}
    };
    
    PlasticityStatistics getStatistics() const;
    
    // Error handling for plasticity operations
    template<typename TFunc>
    TFunc wrapWithPlasticityErrorHandling(TFunc func, const std::string& operationName) {
        return [this, func, operationName](auto&&... args) -> decltype(func(std::forward<decltype(args)>(args)...)) {
            try {
                return func(std::forward<decltype(args)>(args)...);
            } catch (const PlasticityError& e) {
                PlasticityEvent event;
                event.type = "Error";
                event.operation = operationName;
                event.success = false;
                event.errorMessage = e.what();
                recordPlasticityEvent(event);
                throw;
            } catch (const std::exception& e) {
                PlasticityEvent event;
                event.type = "Error";
                event.operation = operationName;
                event.success = false;
                event.errorMessage = std::string("Unexpected error: ") + e.what();
                recordPlasticityEvent(event);
                throw PlasticityError("Unexpected error in " + operationName + ": " + e.what());
            }
        };
    }
    
private:
    PlasticityDiagnostics();
    ~PlasticityDiagnostics();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced error reporting for neuromodulation operations
class NeuromodulationDiagnostics {
public:
    struct NeuromodulationEvent {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string type; // "Dopamine", "Curiosity", "Novelty", "Acetylcholine", "Norepinephrine"
        std::string operation; // "SignalReward", "Update", "EnhanceAttention"
        float levelChange;
        float currentLevel;
        float targetLevel;
        float plasticityFactor;
        bool success;
        std::string errorMessage;
        
        NeuromodulationEvent() : levelChange(0.0f), currentLevel(0.0f), targetLevel(0.0f),
                                 plasticityFactor(0.0f), success(false) {}
    };
    
    static NeuromodulationDiagnostics& getInstance();
    
    // Event recording and analysis
    void recordNeuromodulationEvent(const NeuromodulationEvent& event);
    std::vector<NeuromodulationEvent> getRecentEvents(size_t count = 100) const;
    std::vector<NeuromodulationEvent> getEventsByType(const std::string& type, size_t count = 100) const;
    
    // Pattern analysis
    std::string analyzeNeuromodulationPatterns() const;
    std::vector<std::string> detectAnomalies() const;
    double calculateNeuromodulationStability() const;
    
    // Error detection and recovery suggestions
    std::vector<std::string> getRecoverySuggestions() const;
    bool shouldAttemptRecovery(const NeuromodulationEvent& event) const;
    
    // Statistics and metrics
    struct NeuromodulationStatistics {
        size_t totalEvents;
        size_t successfulEvents;
        size_t failedEvents;
        float averageLevel;
        float maxLevel;
        float minLevel;
        std::unordered_map<std::string, size_t> eventsByType;
        std::unordered_map<std::string, float> averageEffects;
        
        NeuromodulationStatistics() : totalEvents(0), successfulEvents(0), failedEvents(0),
                                     averageLevel(0.0f), maxLevel(0.0f), minLevel(0.0f) {}
    };
    
    NeuromodulationStatistics getStatistics() const;
    
    // Error handling for neuromodulation operations
    template<typename TFunc>
    TFunc wrapWithNeuromodulationErrorHandling(TFunc func, const std::string& operationName) {
        return [this, func, operationName](auto&&... args) -> decltype(func(std::forward<decltype(args)>(args)...)) {
            try {
                return func(std::forward<decltype(args)>(args)...);
            } catch (const NeuromodulationError& e) {
                NeuromodulationEvent event;
                event.type = "Error";
                event.operation = operationName;
                event.success = false;
                event.errorMessage = e.what();
                recordNeuromodulationEvent(event);
                throw;
            } catch (const std::exception& e) {
                NeuromodulationEvent event;
                event.type = "Error";
                event.operation = operationName;
                event.success = false;
                event.errorMessage = std::string("Unexpected error: ") + e.what();
                recordNeuromodulationEvent(event);
                throw NeuromodulationError("Unexpected error in " + operationName + ": " + e.what());
            }
        };
    }
    
private:
    NeuromodulationDiagnostics();
    ~NeuromodulationDiagnostics();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Comprehensive error reporting and diagnostic utilities
class DiagnosticReporter {
public:
    static DiagnosticReporter& getInstance();
    
    // Error reporting
    void reportError(const std::string& category, const std::string& message, 
                   const std::string& details = "");
    void reportWarning(const std::string& category, const std::string& message);
    void reportInfo(const std::string& category, const std::string& message);
    
    // Diagnostic logging
    void enableFileLogging(const std::string& filepath);
    void enableConsoleLogging(bool enable);
    void setLogLevel(int level); // 0=ERROR, 1=WARN, 2=INFO, 3=DEBUG
    
    // Diagnostic reports generation
    std::string generateSystemReport() const;
    std::string generateBrainReport() const;
    std::string generatePlasticityReport() const;
    std::string generateNeuromodulationReport() const;
    std::string generatePerformanceReport() const;
    
    // Export diagnostic data
    bool exportDiagnosticData(const std::string& filepath) const;
    bool exportBrainState(const std::string& filepath) const;
    bool exportPerformanceMetrics(const std::string& filepath) const;
    
    // Error recovery and analysis
    std::vector<std::string> analyzeRecentErrors() const;
    std::vector<std::string> getRecoveryPlan() const;
    bool attemptSystemRecovery();
    
private:
    DiagnosticReporter();
    ~DiagnosticReporter();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced error handling for critical operations
class ErrorHandler {
public:
    enum class ErrorSeverity {
        LOW,
        MEDIUM,
        HIGH,
        CRITICAL,
        CATASTROPHIC
    };
    
    struct ErrorContext {
        std::chrono::high_resolution_clock::time_point timestamp;
        ErrorSeverity severity;
        std::string category;
        std::string message;
        std::string details;
        std::string operation;
        std::vector<std::string> suggestedActions;
        bool autoRecoverable;
        bool requiresRestart;
        
        ErrorContext() : severity(ErrorSeverity::MEDIUM), autoRecoverable(false), 
                        requiresRestart(false) {}
    };
    
    // Error reporting and handling
    static void reportError(const ErrorContext& context);
    static void reportError(const std::string& category, const std::string& message,
                          ErrorSeverity severity = ErrorSeverity::MEDIUM,
                          const std::string& details = "");
    
    // Error recovery
    static bool attemptRecovery(const ErrorContext& context);
    static bool shouldAttemptRecovery(const ErrorContext& context);
    static std::vector<std::string> getRecoverySteps(const ErrorContext& context);
    
    // Error prevention
    static void setErrorPreventionRules(const std::vector<std::function<bool()>>& rules);
    static bool checkPreventionRules();
    
    // Error analysis
    static std::string analyzeErrorPatterns();
    static std::vector<std::string> getErrorTrends();
    
private:
    static ErrorHandler& getInstance();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Python bindings for error handling and diagnostics
PYBIND11_MODULE(pynlm_diagnostics, m) {
    m.doc() = R"pbdoc(
        NLM Diagnostics and Error Handling
        -----------------------------------
        Enhanced error handling, diagnostics, and recovery for NLM.
    )pbdoc";
    
    // Exception hierarchy
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<PlasticityError>(m, "PlasticityError");
    py::register_exception<NeuromodulationError>(m, "NeuromodulationError");
    
    // Diagnostic utilities
    m.def("validate_config", &ConfigValidator::validate,
          py::arg("config"), "Validate configuration for errors and warnings");
    
    m.def("validate_safety", &ConfigValidator::validateForSafety,
          py::arg("config"), "Validate configuration against safety constraints");
    
    m.def("get_optimization_recommendations", &ConfigValidator::getRecommendations,
          py::arg("config"), "Get optimization recommendations for configuration");
    
    // Brain diagnostics
    m.def("get_brain_diagnostics", []() { return &BrainDiagnostics::getInstance(); },
          "Get brain diagnostics instance");
    
    m.def("get_brain_state", []() { return &BrainDiagnostics::getInstance()->captureState(); },
          "Get current brain state");
    
    // Plasticity diagnostics
    m.def("get_plasticity_diagnostics", []() { return &PlasticityDiagnostics::getInstance(); },
          "Get plasticity diagnostics instance");
    
    m.def("get_plasticity_statistics", []() { return &PlasticityDiagnostics::getInstance()->getStatistics(); },
          "Get plasticity statistics");
    
    // Neuromodulation diagnostics
    m.def("get_neuromodulation_diagnostics", []() { return &NeuromodulationDiagnostics::getInstance(); },
          "Get neuromodulation diagnostics instance");
    
    m.def("get_neuromodulation_statistics", []() { return &NeuromodulationDiagnostics::getInstance()->getStatistics(); },
          "Get neuromodulation statistics");
    
    // Diagnostic reporter
    m.def("get_diagnostic_reporter", []() { return &DiagnosticReporter::getInstance(); },
          "Get diagnostic reporter instance");
    
    m.def("generate_system_report", []() { return &DiagnosticReporter::getInstance()->generateSystemReport(); },
          "Generate comprehensive system diagnostic report");
    
    // Error handling
    m.def("report_error", &ErrorHandler::reportError,
          py::arg("category"), py::arg("message"), py::arg("severity") = ErrorHandler::ErrorSeverity::MEDIUM,
          py::arg("details") = "",
          "Report an error with specified category and severity");
    
    m.def("attempt_recovery", &ErrorHandler::attemptRecovery,
          py::arg("context"), "Attempt to recover from an error");
    
    // Utility functions
    m.def("enable_diagnostic_logging", []() { DiagnosticReporter::getInstance().enableFileLogging("nlm_diagnostics.log"); },
          "Enable file logging for diagnostics");
    
    m.def("disable_diagnostic_logging", []() { DiagnosticReporter::getInstance().enableConsoleLogging(false); },
          "Disable console logging for diagnostics");
    
    m.def("export_diagnostic_data", &DiagnosticReporter::getInstance,
          py::arg("filepath"), "Export diagnostic data to file");
    
    // Enhanced error handling wrapper for brain operations
    m.def("with_error_handling", [](std::function<void()> func, const std::string& operationName) {
        try {
            func();
        } catch (const NLMError& e) {
            ErrorHandler::reportError("Operation", operationName + " failed: " + e.what(),
                                    ErrorHandler::ErrorSeverity::HIGH);
            throw;
        } catch (const std::exception& e) {
            ErrorHandler::reportError("Operation", operationName + " failed: " + e.what(),
                                    ErrorHandler::ErrorSeverity::CRITICAL);
            throw BrainError("Unexpected error in " + operationName + ": " + e.what());
        }
    }, py::arg("func"), py::arg("operation_name"),
        "Execute a function with comprehensive error handling");
    
    // Batch error handling
    m.def("with_error_handling_batch", [](std::vector<std::function<void()>> functions, 
                                        const std::vector<std::string>& operationNames) {
        std::vector<bool> results;
        for (size_t i = 0; i < functions.size(); ++i) {
            try {
                functions[i]();
                results.push_back(true);
            } catch (const NLMError& e) {
                ErrorHandler::reportError("BatchOperation", operationNames[i] + " failed: " + e.what(),
                                        ErrorHandler::ErrorSeverity::HIGH);
                results.push_back(false);
            } catch (const std::exception& e) {
                ErrorHandler::reportError("BatchOperation", operationNames[i] + " failed: " + e.what(),
                                        ErrorHandler::ErrorSeverity::CRITICAL);
                results.push_back(false);
            }
        }
        return results;
    }, py::arg("functions"), py::arg("operation_names"),
        "Execute multiple functions with batch error handling");
}

} // namespace nlm
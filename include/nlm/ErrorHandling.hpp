// NLM Error Handling Framework
// Comprehensive error handling and validation for the Neural Learning Machine
// 
// This file provides:
// 1. Error hierarchy and categorization
// 2. Input validation for all public APIs
// 3. Exception safety and resource management
// 4. Error recovery mechanisms
// 5. System monitoring and error reporting
// 
// The error handling framework integrates with:
// - Core system components (Logger, Config, Random)
// - Brain system (neuron management, synaptic operations)
// - Memory systems (working memory, episodic memory)
// - Neuromodulation systems (dopamine, curiosity)
// - Cognitive systems (NeuralPlanner, ConceptFormation, Attention)
// - Development system (growth and adaptation)
// - Checkpoint system (persistence and recovery)
// 
// Error handling principles:
// 1. Fail fast with clear error messages
// 2. Resource safety - no leaks on error
// 3. Recovery mechanisms where possible
//  děkuji za přečkání - Thank you for reading
// This is a significant improvement over the previous minimal error handling
// approach used in the NLM project.

#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <cmath>

namespace nlm {

// Error categories for NLM system
enum class ErrorCategory {
    System,      // System-level errors (memory allocation, file I/O)
    Validation,  // Input validation errors
    Runtime,     // Runtime errors (null pointers, invalid states)
    Configuration, // Configuration and setup errors
    I/O,         // File and network I/O errors
    Memory,      // Memory management errors
    Plasticity,   // Learning and plasticity errors
    Development,  // Development stage errors
    Integration   // System integration errors
};

// Error severity levels
enum class ErrorSeverity {
    Debug,       // Informational errors (can be ignored)
    Warning,     // Potentially problematic (system can continue)
    Error,       // Serious errors (system can continue with reduced functionality)
    Critical,    // Critical errors (system must stop or recover)
    Fatal,       // Fatal errors (system cannot continue)
};

// Basic error class
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorCategory category = ErrorCategory::System, 
             ErrorSeverity severity = ErrorSeverity::Error,
             const std::string& context = "")
        : std::runtime_error(message), category_(category), severity_(severity), context_(context) {}
    
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getContext() const { return context_; }
    
    std::string toString() const {
        std::string severityStr;
        switch (severity_) {
            case ErrorSeverity::Debug: severityStr = "DEBUG"; break;
            case ErrorSeverity::Warning: severityStr = "WARNING"; break;
            case ErrorSeverity::Error: severityStr = "ERROR"; break;
            case ErrorSeverity::Critical: severityStr = "CRITICAL"; break;
            case ErrorSeverity::Fatal: severityStr = "FATAL"; break;
        }
        
        return "[" + severityStr + "] " + categoryToString(category_) + 
               (context_.empty() ? "" : " (" + context_ + ")") + 
               ": " + what();
    }
    
private:
    std::string categoryToString(ErrorCategory category) const {
        switch (category) {
            case ErrorCategory::System: return "SYSTEM";
            case ErrorCategory::Validation: return "VALIDATION";
            case ErrorCategory::Runtime: return "RUNTIME";
            case ErrorCategory::Configuration: return "CONFIGURATION";
            case ErrorCategory::I/O: return "I/O";
            case ErrorCategory::Memory: return "MEMORY";
            case ErrorCategory::Plasticity: return "PLASTICITY";
            case ErrorCategory::Development: return "DEVELOPMENT";
            case ErrorCategory::Integration: return "INTEGRATION";
        }
        return "UNKNOWN";
    }
    
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string context_;
};

// Input validation utilities
class InputValidator {
public:
    static bool validateFilepath(const std::string& filepath);
    static bool validateNeuronId(uint64_t id);
    static bool validatePopulationId(uint32_t id);
    static bool validateRegionId(uint32_t id);
    static bool validateActionType(int action);
    static bool validateSimulationStep(size_t step);
    static bool validateTimestamp(double time);
    static bool validateCapacity(size_t capacity);
    static bool validateFloatRange(float value, float min, float max);
    static bool validateVectorSize(size_t size, size_t maxSize);
    
    template<typename T>
    static bool validateNotNull(const T* ptr, const std::string& context) {
        if (!ptr) {
            throw NLMError("Null pointer provided", ErrorCategory::Validation, 
                          ErrorSeverity::Critical, context);
        }
        return true;
    }
    
    template<typename T>
    static bool validateRange(const std::vector<T>& vec, size_t maxSize, 
                             const std::string& context) {
        if (vec.size() > maxSize) {
            throw NLMError("Vector size exceeds maximum allowed", 
                          ErrorCategory::Validation, ErrorSeverity::Error, context);
        }
        return true;
    }
};

// Error recovery utilities
class ErrorRecovery {
public:
    static bool attemptRecovery(ErrorCategory category, const std::string& context);
    static void resetToSafeState();
    static bool validateSystemIntegrity();
    static void logErrorContext(const std::string& error, const std::string& context);
};

// Enhanced configuration with error handling
class SafeConfig {
public:
    static bool safeLoadFromFile(const std::string& filepath, Config& config);
    static bool safeLoadFromArgs(int argc, char** argv, Config& config);
    static bool validateConfig(const Config& config);
};

// Enhanced neuron operations with error handling
class SafeNeuronOperations {
public:
    static bool validateNeuronState(const Neuron& neuron);
    static bool validateSynapse(const Synapse& synapse);
    static bool validatePopulation(const NeuralPopulation& population);
    static bool validateRegion(const NeuralRegion& region);
    
    static void safeInjectCurrent(Neuron* neuron, float current, const std::string& context);
    static void safeSetMembranePotential(Neuron* neuron, float potential, const std::string& context);
    static void safeSetWeight(Synapse* synapse, float weight, const std::string& context);
};

// Enhanced memory operations with error handling
class SafeMemoryOperations {
public:
    static bool validateWorkingMemoryState(const NeuralWorkingMemory& wm);
    static bool validateEpisodicMemoryState(const NeuralEpisodicMemory& em);
    static bool validateAssociativeMemoryState(const NeuralAssociativeMemory& am);
    
    static void safeStoreToNeuron(NeuralWorkingMemory& wm, NeuronId neuron, float activation);
    static void safeStoreEpisode(NeuralEpisodicMemory& em, const EpisodicMemoryItem& episode);
};

// Enhanced development operations with error handling
class SafeDevelopmentOperations {
public:
    static bool validateDevelopmentalStage(DevelopmentalStage stage);
    static bool validateDevelopmentParameters(const DevelopmentSystem& dev);
    
    static void safeUpdateDevelopment(DevelopmentSystem& dev, const std::string& context);
    static void safeTransitionDevelopmentStage(DevelopmentSystem& dev, 
                                              DevelopmentalStage newStage);
};

// Enhanced checkpoint operations with error handling
class SafeCheckpointOperations {
public:
    static bool safeSave(const Brain& brain, const std::string& filepath);
    static bool safeLoad(Brain& brain, const std::string& filepath);
    static bool validateCheckpointFile(const std::string& filepath);
    static std::string generateCheckpointFilename(const std::string& prefix = "nlm_checkpoint");
};

// Global error handler
class ErrorHandler {
public:
    static void setErrorHandler(std::function<void(const NLMError&)>& handler);
    static void handleError(const NLMError& error);
    static void enableDetailedErrorReporting(bool enable);
    static bool isErrorReportingEnabled();
    
    static std::vector<NLMError> getRecentErrors();
    static void clearRecentErrors();
    static size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error);
    
private:
    static std::vector<NLMError> recentErrors_;
    static std::function<void(const NLMError&)> errorHandler_;
    static bool detailedErrorReportingEnabled_;
};

// Error handling macros for common operations
#define NLM_VALIDATE(condition, errorMsg, context) \
    do { \
        if (!(condition)) { \
            throw nlm::NLMError(errorMsg, nlm::ErrorCategory::Validation, \
                              nlm::ErrorSeverity::Error, context); \
        } \
    } while(0)

#define NLM_CHECK_NOT_NULL(ptr, context) \
    nlm::InputValidator::validateNotNull(ptr, context)

#define NLM_SAFE_OPERATION(operation, errorMsg, context) \
    try { \
        operation; \
    } catch (const std::exception& e) { \
        throw nlm::NLMError(errorMsg + std::string(" (caused by: ") + e.what() + ")", \
                          nlm::ErrorCategory::System, nlm::ErrorSeverity::Error, context); \
    }

// Error handling helper functions
inline std::string errorCategoryToString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::System: return "System";
        case ErrorCategory::Validation: return "Validation";
        case ErrorCategory::Runtime: return "Runtime";
        case ErrorCategory::Configuration: return "Configuration";
        case ErrorCategory::I/O: return "I/O";
        case ErrorCategory::Memory: return "Memory";
        case ErrorCategory::Plasticity: return "Plasticity";
        case ErrorCategory::Development: return "Development";
        case ErrorCategory::Integration: return "Integration";
    }
    return "Unknown";
}

inline std::string errorSeverityToString(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Debug: return "Debug";
        case ErrorSeverity::Warning: return "Warning";
        case ErrorSeverity::Error: return "Error";
        case ErrorSeverity::Critical: return "Critical";
        case ErrorSeverity::Fatal: return "Fatal";
    }
    return "Unknown";
}

// Global error reporting function
inline void reportError(const NLMError& error) {
    std::cerr << "[NLM " << errorSeverityToString(error.getSeverity()) << ": " 
              << errorCategoryToString(error.getCategory()) << "] " 
              << error.what();
    
    if (!error.getContext().empty()) {
        std::cerr << " (Context: " << error.getContext() << ")";
    }
    
    std::cerr << std::endl;
}

// Global error handler setup
inline void setupGlobalErrorHandler() {
    ErrorHandler::setErrorHandler([](const NLMError& error) {
        reportError(error);
    });
    ErrorHandler::enableDetailedErrorReporting(true);
}

} // namespace nlm
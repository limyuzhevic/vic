// NLM Error Handling System
#pragma once

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <chrono>

namespace nlm {

// Error Categories
enum class ErrorCategory {
    // Core system errors
    System = 0,
    Memory,
    Network,
    Neural,
    Plasticity,
    MemorySystem,
    Neuromodulation,
    Config,
    World,
    Development,
    Prediction,
    Cognition,
    Motor,
    Sensory,
    Visualization,
    Performance,
    Development,
    Experiment,
    MAX_CATEGORY
};

// Error codes for specific error conditions
enum class ErrorCode {
    // System errors
    General = 1000,
    OutOfMemory,
    NotImplemented,
    InvalidState,
    Timeout,
    ResourceExhausted,
    
    // Memory system errors
    MemoryFull = 2000,
    InvalidAccess,
    BufferOverflow,
    
    // Neural errors
    InvalidNeuronState = 3000,
    MembranePotentialOverflow,
    InvalidSynapse,
    ConnectionFailure,
    
    // Plasticity errors
    InvalidPlasticityRule = 4000,
    STDPError,
    HebbianError,
    
    // Config errors
    InvalidConfig = 5000,
    MissingRequiredParameter,
    InvalidValue,
    SchemaValidationFailed,
    
    // World errors
    InvalidWorldState = 6000,
    CollisionError,
    BoundaryError,
    
    // Development errors
    DevelopmentError = 7000,
    CriticalPeriodError,
    MaturationError,
    
    // Prediction errors
    PredictionError = 8000,
    ModelError,
    InferenceError,
    
    // Cognition errors
    PlanningError = 9000,
    ConceptError,
    AttentionError,
    
    // Motor errors
    MotorError = 10000,
    InvalidAction,
    CommandError,
    
    // Sensory errors
    SensoryError = 11000,
    InvalidSensoryInput,
    SensorError,
    
    // Visualization errors
    VisualizationError = 12000,
    RenderError,
    
    // Performance errors
    PerformanceError = 13000,
    OptimizationError,
    ParallelError,
    
    // Experiment errors
    ExperimentError = 14000,
    TrialError,
    RecordingError,
    
    // Python binding errors
    PythonError = 15000,
    ConversionError,
    TypeError,
    AttributeError
};

// Error level for severity
enum class ErrorLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Enhanced error context with stack trace
struct ErrorContext {
    std::string file;
    std::string function;
    int line;
    std::string message;
    std::map<std::string, std::string> details;
    std::vector<std::string> stackTrace;
    
    ErrorContext(const std::string& msg = "", 
                 const std::string& file = "", 
                 const std::string& func = "", 
                 int ln = 0)
        : file(file), function(func), line(ln), message(msg) {}
};

// Base NLM error class
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message,
             ErrorCode code = ErrorCode::General,
             ErrorCategory category = ErrorCategory::System,
             const ErrorContext& context = ErrorContext())
        : std::runtime_error(message),
          code(code),
          category(category),
          context(context),
          level(ErrorLevel::Error),
          timestamp(std::chrono::system_clock::now()) {
        fullMessage = formatFullMessage();
    }
    
    NLMError(ErrorCode code, const std::string& message)
        : std::runtime_error(message),
          code(code),
          category(ErrorCategory::System),
          level(ErrorLevel::Error),
          timestamp(std::chrono::system_clock::now()) {
        fullMessage = formatFullMessage();
    }
    
    // Accessors
    ErrorCode getCode() const { return code; }
    ErrorCategory getCategory() const { return category; }
    ErrorLevel getLevel() const { return level; }
    const ErrorContext& getContext() const { return context; }
    const std::string& getFullMessage() const { return fullMessage; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp; }
    
    // Setters
    void setLevel(ErrorLevel lvl) { level = lvl; }
    void addDetail(const std::string& key, const std::string& value) {
        context.details[key] = value;
    }
    
    // Add stack trace entry
    void addStackFrame(const std::string& frame) {
        context.stackTrace.push_back(frame);
    }
    
    // What() override for compatibility
    const char* what() const noexcept override {
        return fullMessage.c_str();
    }
    
protected:
    std::string formatFullMessage() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(category) << "." << static_cast<int>(code) << "] " << what();
        
        if (!context.file.empty()) {
            oss << " (at " << context.file;
            if (context.line > 0) oss << ":" << context.line;
            oss << ")";
        }
        
        return oss.str();
    }
    
private:
    ErrorCode code;
    ErrorCategory category;
    ErrorLevel level;
    ErrorContext context;
    std::string fullMessage;
    std::chrono::system_clock::time_point timestamp;
};

// Specific error types for common scenarios
class NeuralError : public NLMError {
public:
    NeuralError(const std::string& message, ErrorCode code, const ErrorContext& context = ErrorContext())
        : NLMError(message, code, ErrorCategory::Neural, context) {}
};

class SynapseError : public NeuralError {
public:
    SynapseError(const std::string& message, ErrorCode code = ErrorCode::InvalidSynapse)
        : NeuralError(message, code) {}
};

class NeuronError : public NeuralError {
public:
    NeuronError(const std::string& message, ErrorCode code = ErrorCode::InvalidNeuronState)
        : NeuralError(message, code) {}
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message, ErrorCode code, const ErrorContext& context = ErrorContext())
        : NLMError(message, code, ErrorCategory::MemorySystem, context) {}
};

class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message, ErrorCode code = ErrorCode::InvalidConfig,
                const ErrorContext& context = ErrorContext())
        : NLMError(message, code, ErrorCategory::Config, context) {}
};

class PlasticityError : public NLMError {
public:
    PlasticityError(const std::string& message, ErrorCode code = ErrorCode::InvalidPlasticityRule,
                   const ErrorContext& context = ErrorContext())
        : NLMError(message, code, ErrorCategory::Plasticity, context) {}
};

class WorldError : public NLMError {
public:
    WorldError(const std::string& message, ErrorCode code = ErrorCode::InvalidWorldState,
               const ErrorContext& context = ErrorContext())
        : NLMError(message, code, ErrorCategory::World, context) {}
};

class DevelopmentError : public NLMError {
public:
    DevelopmentError(const std::string& message, ErrorCode code = ErrorCode::DevelopmentError,
                    const ErrorContext& context = ErrorContext())
        : NLMError(message, code, ErrorCategory::Development, context) {}
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const NLMError& error) = 0;
    virtual bool shouldHandle(ErrorCategory category, ErrorCode code) const = 0;
};

// Error manager for centralized error handling
class ErrorManager {
public:
    static ErrorManager& getInstance() {
        static ErrorManager instance;
        return instance;
    }
    
    void registerHandler(std::shared_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        handlers.push_back(handler);
    }
    
    void unregisterHandler(std::shared_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = std::find(handlers.begin(), handlers.end(), handler);
        if (it != handlers.end()) {
            handlers.erase(it);
        }
    }
    
    void handleError(const NLMError& error) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& handler : handlers) {
            if (handler->shouldHandle(error.getCategory(), error.getCode())) {
                handler->handleError(error);
                return;
            }
        }
        // Default handler: log to stderr
        std::cerr << "Unhandled error: " << error.what() << std::endl;
    }
    
    void setDefaultHandler(std::shared_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        defaultHandler = handler;
    }
    
    // Set error level threshold
    void setErrorLevel(ErrorLevel level) {
        std::lock_guard<std::mutex> lock(mutex);
        errorLevelThreshold = level;
    }
    
    ErrorLevel getErrorLevel() const { return errorLevelThreshold; }
    
private:
    ErrorManager() = default;
    ~ErrorManager() = default;
    ErrorManager(const ErrorManager&) = delete;
    ErrorManager& operator=(const ErrorManager&) = delete;
    
    std::vector<std::shared_ptr<IErrorHandler>> handlers;
    std::shared_ptr<IErrorHandler> defaultHandler;
    std::mutex mutex;
    ErrorLevel errorLevelThreshold = ErrorLevel::Error;
};

// Exception translator for Python bindings
namespace PythonBindings {
    // Convert C++ NLMError to Python exception
    inline void translateNLMError(const NLMError& e) {
        // Map error categories to Python exception types
        if (std::is_same<const ConfigError&, const NLMError&>::value ||
            e.getCategory() == ErrorCategory::Config) {
            throw py::value_error("Config error: " + std::string(e.what()));
        } else if (e.getCategory() == ErrorCategory::Neural) {
            throw py::runtime_error("Neural error: " + std::string(e.what()));
        } else if (e.getCategory() == ErrorCategory::MemorySystem) {
            throw py::index_error("Memory error: " + std::string(e.what()));
        } else if (e.getCategory() == ErrorCategory::System) {
            throw py::runtime_error("System error: " + std::string(e.what()));
        } else {
            throw py::runtime_error(std::string(e.what()));
        }
    }
    
    // Helper to capture and translate all exceptions
    inline void registerExceptionTranslator() {
        py::register_exception_translator([](std::exception_ptr p) {
            try {
                if (p) std::rethrow_exception(p);
            } catch (const NLMError& e) {
                translateNLMError(e);
            } catch (const std::exception& e) {
                throw py::runtime_error("C++ exception: " + std::string(e.what()));
            }
        });
    }
}

// Utility functions for error handling
namespace ErrorUtils {
    inline std::string formatErrorDetails(const std::map<std::string, std::string>& details) {
        if (details.empty()) return "";
        
        std::ostringstream oss;
        bool first = true;
        for (const auto& [key, value] : details) {
            if (!first) oss << ", ";
            oss << key << "=" << value;
            first = false;
        }
        return oss.str();
    }
    
    inline ErrorContext createContext(const std::string& message, 
                                      const std::string& file = "",
                                      const std::string& function = "",
                                      int line = 0) {
        return ErrorContext(message, file, function, line);
    }
    
    inline ErrorContext addContext(const ErrorContext& ctx, 
                                  const std::string& key, 
                                  const std::string& value) {
        ErrorContext result = ctx;
        result.addDetail(key, value);
        return result;
    }
}

} // namespace nlm

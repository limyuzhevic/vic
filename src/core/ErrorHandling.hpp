#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <sstream>
#include <chrono>

namespace nlm {

/**
 * @enum ErrorCode
 * @brief Standardized error codes for NLM operations
 * 
 * Provides a comprehensive set of error codes covering all aspects of the
 * neural simulation system, from core neural computation to higher-level
 * functionality.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
enum class ErrorCode {
    // Success
    Success = 0,
    
    // General errors
    UnknownError = 1,
    OutOfMemory = 2,
    InvalidParameter = 3,
    InvalidState = 4,
    NotImplemented = 5,
    FileNotFound = 6,
    PermissionDenied = 7,
    
    // Configuration errors
    ConfigFileError = 100,
    InvalidConfigValue = 101,
    MissingConfigKey = 102,
    InvalidConfigFormat = 103,
    
    // Neural system errors
    NeuronError = 200,
    InvalidNeuronId = 201,
    NeuronAlreadyFiring = 202,
    NeuronRefractory = 203,
    SynapseError = 300,
    InvalidSynapseId = 301,
    SynapseWeightOutOfRange = 302,
    InvalidConnection = 303,
    RegionError = 400,
    InvalidRegionId = 401,
    PopulationError = 500,
    InvalidPopulationId = 501,
    
    // Brain system errors
    BrainError = 600,
    BrainNotInitialized = 601,
    BrainAlreadyInitialized = 602,
    StepError = 603,
    PlasticityError = 604,
    
    // Memory system errors
    MemoryError = 700,
    WorkingMemoryFull = 701,
    EpisodicMemoryFull = 702,
    AssociativeMemoryError = 703,
    
    // Neuromodulation errors
    NeuromodulationError = 800,
    InvalidNeuromodulator = 801,
    NoveltyError = 802,
    CuriosityError = 803,
    PredictionError = 804,
    
    // Development system errors
    DevelopmentError = 900,
    InvalidDevelopmentalStage = 901,
    SynaptogenesisError = 902,
    PruningError = 903,
    MaturationError = 904,
    
    // Prediction system errors
    PredictionErrorCode = 1000,
    InvalidPredictionModel = 1001,
    PredictionComputationError = 1002,
    
    // Cognition system errors
    CognitionError = 1100,
    PlanningError = 1101,
    ConceptFormationError = 1102,
    AttentionError = 1103,
    
    // Environment system errors
    EnvironmentError = 1200,
    InvalidWorldObject = 1201,
    WorldCollisionError = 1202,
    ActionError = 1300,
    InvalidActionType = 1301,
    
    // Performance errors
    PerformanceError = 1400,
    MemoryPoolExhausted = 1401,
    PerformanceMonitorError = 1402,
    
    // IO and persistence errors
    IOError = 1500,
    CheckpointError = 1501,
    SerializationError = 1502,
    DeserializationError = 1503,
    
    // Plugin and extension errors
    PluginError = 1600,
    InvalidPlugin = 1601,
    PluginLoadError = 1602,
    PluginNotFound = 1603,
    
    // Integration and system errors
    IntegrationError = 1700,
    ComponentConnectionError = 1701,
    ConfigurationError = 1702,
    
    // Agent and interface errors
    AgentError = 1800,
    AgentNotInitialized = 1801,
    WorldInteractionError = 1802,
    RewardError = 1803,
    
    // Simulation errors
    SimulationError = 1900,
    StepLimitExceeded = 1901,
    TimeStepError = 1902,
    ConvergenceError = 1903,
    
    // Export and visualization errors
    ExportError = 2000,
    VisualizationError = 2001,
    DataFormatError = 2002,
    
    // System resource errors
    ResourceError = 2100,
    NoFreeCPU = 2101,
    NoFreeMemory = 2102,
    NoFreeDiskSpace = 2103,
    
    // Communication and interface errors
    CommunicationError = 2200,
    NetworkError = 2201,
    InterfaceError = 2202,
    ProtocolError = 2203,
    
    // Security and authentication errors
    SecurityError = 2300,
    AuthenticationError = 2301,
    AuthorizationError = 2302,
    EncryptionError = 2303,
    
    // Database and storage errors
    DatabaseError = 2400,
    QueryError = 2401,
    TransactionError = 2402,
    ConstraintError = 2403,
    
    // Validation and quality control errors
    ValidationError = 2500,
    InputValidationError = 2501,
    OutputValidationError = 2502,
    DataIntegrityError = 2503,
    
    // Testing and verification errors
    TestError = 2600,
    AssertionError = 2601,
    TestTimeout = 2602,
    BenchmarkError = 2603,
    
    // Shutdown and cleanup errors
    ShutdownError = 2700,
    CleanupError = 2701,
    ResourceLeakError = 2702,
    
    // Custom application errors
    CustomError = 2800,
    BusinessLogicError = 2801,
    ApplicationError = 2802,
    ConfigurationErrorCode = 2803,
    
    // Dependency errors
    DependencyError = 2900,
    MissingDependency = 2901,
    VersionConflict = 2902,
    InitializationOrderError = 2903,
    
    // Concurrent execution errors
    ConcurrencyError = 3000,
    RaceConditionError = 3001,
    DeadlockError = 3002,
    SynchronizationError = 3003,
    
    // Memory management errors
    MemoryManagementError = 3100,
    DoubleFreeError = 3101,
    MemoryCorruptionError = 3102,
    BufferOverflowError = 3103,
    
    // Performance and optimization errors
    OptimizationError = 3200,
    SIMDError = 3201,
    VectorizationError = 3202,
    CacheOptimizationError = 3203,
    
    // System monitoring errors
    MonitoringError = 3300,
    SensorError = 3301,
    TelemetryError = 3302,
    MetricsError = 3303,
    
    // Safety and fault tolerance errors
    SafetyError = 3400,
    FaultToleranceError = 3401,
    RecoveryError = 3402,
    FailoverError = 3403,
    
    // Custom neural model errors
    NeuralModelError = 3500,
    InvalidModelParameters = 3501,
    ModelConvergenceError = 3502,
    ModelSerializationError = 3503,
    
    // Learning and training errors
    LearningError = 3600,
    TrainingError = 3601,
    ValidationError = 3602,
    OverfittingError = 3603,
    UnderfittingError = 3604,
    
    // Simulation-specific errors
    SimulationSetupError = 3700,
    InitialConditionError = 3701,
    BoundaryConditionError = 3702,
    EnvironmentSetupError = 3703,
    
    // Analysis and interpretation errors
    AnalysisError = 3800,
    StatisticalError = 3801,
    PatternRecognitionError = 3802,
    FeatureExtractionError = 3803,
    
    // Output and result errors
    OutputError = 3900,
    ResultFormatError = 3901,
    ResultValidationError = 3902,
    ResultInterpretationError = 3903,
    
    // User interface and interaction errors
    UIError = 4000,
    InputValidationError = 4001,
    DisplayError = 4002,
    InteractionError = 4003,
    
    // Documentation and metadata errors
    DocumentationError = 4100,
    MetadataError = 4101,
    SchemaError = 4102,
    VersionError = 4103,
    
    // Deployment and distribution errors
    DeploymentError = 4200,
    DistributionError = 4201,
    UpdateError = 4202,
    RollbackError = 4203,
    
    // Regulatory and compliance errors
    ComplianceError = 4300,
    StandardsError = 4301,
    CertificationError = 4302,
    
    // Research and reproducibility errors
    ResearchError = 4400,
    ReproducibilityError = 4401,
    PeerReviewError = 4402,
    ValidationError = 4403,
    
    // Future extensibility placeholder errors
    FutureError = 9999
};

/**
 * @class ErrorContext
 * @brief Provides context information for errors
 * 
 * Captures additional context information about errors including:
 * - Timestamp when error occurred
 * - Location in source code
 * - Component/system where error occurred
 * - Additional error details and parameters
 * - Nested error information
 * 
 * This context information is essential for debugging and error analysis.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class ErrorContext {
public:
    /**
     * @brief Construct a new ErrorContext object
     * 
     * @param errorCode The error code that occurred
     * @param message Human-readable error message
     * @param component The component/system where error occurred
     * @param file Source file where error occurred
     * @param line Line number in source file
     * @param function Function where error occurred
     */
    ErrorContext(ErrorCode errorCode, const std::string& message,
                 const std::string& component = "Unknown",
                 const std::string& file = "Unknown",
                 int line = 0,
                 const std::string& function = "Unknown")
        : errorCode_(errorCode)
        , message_(message)
        , component_(component)
        , file_(file)
        , line_(line)
        , function_(function)
        , timestamp_(std::chrono::high_resolution_clock::now())
        , parentError_(nullptr)
    {
    }
    
    /**
     * @brief Get the error code
     */
    ErrorCode getErrorCode() const { return errorCode_; }
    
    /**
     * @brief Get the error message
     */
    const std::string& getMessage() const { return message_; }
    
    /**
     * @brief Get the component where error occurred
     */
    const std::string& getComponent() const { return component_; }
    
    /**
     * @brief Get the source file where error occurred
     */
    const std::string& getFile() const { return file_; }
    
    /**
     * @brief Get the line number where error occurred
     */
    int getLine() const { return line_; }
    
    /**
     * @brief Get the function where error occurred
     */
    const std::string& getFunction() const { return function_; }
    
    /**
     * @brief Get the timestamp when error occurred
     */
    std::chrono::time_point<std::chrono::high_resolution_clock> getTimestamp() const { return timestamp_; }
    
    /**
     * @brief Get the parent error (if this is a nested error)
     */
    std::shared_ptr<ErrorContext> getParentError() const { return parentError_; }
    
    /**
     * @brief Set the parent error
     */
    void setParentError(std::shared_ptr<ErrorContext> parent) { parentError_ = parent; }
    
    /**
     * @brief Add a parameter to the error context
     */
    void addParameter(const std::string& name, const std::string& value) {
        parameters_[name] = value;
    }
    
    /**
     * @brief Get all parameters
     */
    const std::unordered_map<std::string, std::string>& getParameters() const { return parameters_; }
    
    /**
     * @brief Get a specific parameter value
     */
    std::string getParameter(const std::string& name) const {
        auto it = parameters_.find(name);
        return it != parameters_.end() ? it->second : "";
    }
    
    /**
     * @brief Convert error context to string representation
     */
    std::string toString() const {
        std::stringstream ss;
        ss << "Error Context:" << std::endl;
        ss << "  Code: " << static_cast<int>(errorCode_) << std::endl;
        ss << "  Message: " << message_ << std::endl;
        ss << "  Component: " << component_ << std::endl;
        ss << "  Location: " << file_ << ":" << line_ << " in " << function_ << std::endl;
        ss << "  Timestamp: " << getTimestampString() << std::endl;
        
        if (!parameters_.empty()) {
            ss << "  Parameters:" << std::endl;
            for (const auto& param : parameters_) {
                ss << "    " << param.first << " = " << param.second << std::endl;
            }
        }
        
        if (parentError_) {
            ss << "  Parent Error:" << std::endl;
            ss << "    " << parentError_->toString() << std::endl;
        }
        
        return ss.str();
    }
    
    /**
     * @brief Get timestamp as string
     */
    std::string getTimestampString() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp_);
        return std::to_string(duration.count()) + " ms ago";
    }

private:
    ErrorCode errorCode_;                                    // Error code
    std::string message_;                                    // Error message
    std::string component_;                                  // Component/system
    std::string file_;                                       // Source file
    int line_;                                               // Line number
    std::string function_;                                   // Function name
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp_;  // Error timestamp
    std::shared_ptr<ErrorContext> parentError_;             // Parent error (for nested errors)
    std::unordered_map<std::string, std::string> parameters_; // Error parameters
};

/**
 * @class NLMException
 * @brief Base exception class for NLM errors
 * 
 * Provides a comprehensive exception hierarchy for NLM errors with:
 * - Standardized error codes
 * - Context information
 * - Exception chaining
 * - Proper serialization support
 * - Detailed error reporting
 * 
 * All NLM-specific exceptions should inherit from this class.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class NLMException : public std::runtime_error {
public:
    /**
     * @brief Construct a new NLMException object
     * 
     * @param errorCode The error code that occurred
     * @param message Human-readable error message
     * @param context Additional error context
     */
    NLMException(ErrorCode errorCode, const std::string& message,
                 std::shared_ptr<ErrorContext> context = nullptr)
        : std::runtime_error(formatMessage(errorCode, message)),
          errorCode_(errorCode),
          context_(context)
    {
    }
    
    /**
     * @brief Get the error code
     */
    ErrorCode getErrorCode() const { return errorCode_; }
    
    /**
     * @brief Get the error context
     */
    std::shared_ptr<ErrorContext> getContext() const { return context_; }
    
    /**
     * @brief Set the error context
     */
    void setContext(std::shared_ptr<ErrorContext> context) { context_ = context; }
    
    /**
     * @brief Get the full error report
     */
    std::string getFullErrorReport() const {
        std::stringstream ss;
        ss << "NLM Exception Report:" << std::endl;
        ss << "  Error Code: " << static_cast<int>(errorCode_) << std::endl;
        ss << "  Message: " << what() << std::endl;
        
        if (context_) {
            ss << "  Context:" << std::endl;
            ss << context_->toString();
        }
        
        return ss.str();
    }
    
    /**
     * @brief Convert error to string representation
     */
    static std::string formatMessage(ErrorCode errorCode, const std::string& message) {
        std::stringstream ss;
        ss << "[NLM Error " << static_cast<int>(errorCode) << "] " << message;
        return ss.str();
    }
    
    /**
     * @brief Create a nested exception with parent context
     */
    static std::shared_ptr<NLMException> createNested(ErrorCode errorCode, const std::string& message,
                                                      const std::shared_ptr<NLMException>& parent) {
        auto context = std::make_shared<ErrorContext>(errorCode, message);
        if (parent) {
            context->setParentError(parent->getContext());
        }
        
        auto nested = std::make_shared<NLMException>(errorCode, message, context);
        return nested;
    }

protected:
    ErrorCode errorCode_;                          // Error code
    std::shared_ptr<ErrorContext> context_;        // Error context
};

/**
 * @class ConfigurationException
 * @brief Exception thrown for configuration-related errors
 * 
 * Thrown when configuration files cannot be loaded, have invalid values,
 * or when configuration validation fails.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message, ErrorCode errorCode = ErrorCode::ConfigFileError)
        : NLMException(errorCode, message) {}
};

/**
 * @class BrainException
 * @brief Exception thrown for brain-related errors
 * 
 * Thrown when brain initialization, simulation, or operations fail.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class BrainException : public NLMException {
public:
    BrainException(const std::string& message, ErrorCode errorCode = ErrorCode::BrainError)
        : NLMException(errorCode, message) {}
};

/**
 * @class MemoryException
 * @brief Exception thrown for memory-related errors
 * 
 * Thrown when memory allocation fails, memory pools are exhausted,
 * or memory management operations fail.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message, ErrorCode errorCode = ErrorCode::OutOfMemory)
        : NLMException(errorCode, message) {}
};

/**
 * @class NeuronException
 * @brief Exception thrown for neuron-related errors
 * 
 * Thrown when neuron operations fail, invalid neuron states,
 * or neuron computation errors occur.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class NeuronException : public NLMException {
public:
    NeuronException(const std::string& message, ErrorCode errorCode = ErrorCode::NeuronError)
        : NLMException(errorCode, message) {}
};

/**
 * @class SynapseException
 * @brief Exception thrown for synapse-related errors
 * 
 * Thrown when synapse operations fail, invalid synapse states,
 * or synapse plasticity errors occur.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class SynapseException : public NLMException {
public:
    SynapseException(const std::string& message, ErrorCode errorCode = ErrorCode::SynapseError)
        : NLMException(errorCode, message) {}
};

/**
 * @class PlasticityException
 * @brief Exception thrown for plasticity-related errors
 * 
 * Thrown when plasticity rule computations fail, invalid plasticity
 * parameters, or learning rule errors occur.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class PlasticityException : public NLMException {
public:
    PlasticityException(const std::string& message, ErrorCode errorCode = ErrorCode::PlasticityError)
        : NLMException(errorCode, message) {}
};

/**
 * @class DevelopmentException
 * @brief Exception thrown for development-related errors
 * 
 * Thrown when development system operations fail, invalid developmental
 * states, or maturation errors occur.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class DevelopmentException : public NLMException {
public:
    DevelopmentException(const std::string& message, ErrorCode errorCode = ErrorCode::DevelopmentError)
        : NLMException(errorCode, message) {}
};

/**
 * @class NeuromodulationException
 * @brief Exception thrown for neuromodulation-related errors
 * 
 * Thrown when neuromodulator operations fail, invalid neuromodulator
 * states, or signal processing errors occur.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class NeuromodulationException : public NLMException {
public:
    NeuromodulationException(const std::string& message, ErrorCode errorCode = ErrorCode::NeuromodulationError)
        : NLMException(errorCode, message) {}
};

/**
 * @class PerformanceException
 * @brief Exception thrown for performance-related errors
 * 
 * Thrown when performance monitoring, profiling, or optimization
 * operations fail.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class PerformanceException : public NLMException {
public:
    PerformanceException(const std::string& message, ErrorCode errorCode = ErrorCode::PerformanceError)
        : NLMException(errorCode, message) {}
};

/**
 * @class ExportException
 * @brief Exception thrown for export-related errors
 * 
 * Thrown when data export, visualization, or output operations fail.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class ExportException : public NLMException {
public:
    ExportException(const std::string& message, ErrorCode errorCode = ErrorCode::ExportError)
        : NLMException(errorCode, message) {}
};

/**
 * @class PluginException
 * @brief Exception thrown for plugin-related errors
 * 
 * Thrown when plugin loading, initialization, or operations fail.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class PluginException : public NLMException {
public:
    PluginException(const std::string& message, ErrorCode errorCode = ErrorCode::PluginError)
        : NLMException(errorCode, message) {}
};

/**
 * @class ErrorHandler
 * @brief Centralized error handling and reporting system
 * 
 * Provides comprehensive error handling capabilities including:
 * - Error classification and categorization
 * - Error recovery and fallback mechanisms
 * - Error logging and reporting
 * - Error statistics and analysis
 * - Error filtering and suppression
 * 
 * The ErrorHandler ensures consistent error handling across the entire
 * NLM system and provides tools for error analysis and debugging.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class ErrorHandler {
public:
    /**
     * @brief Get the singleton instance of ErrorHandler
     */
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    /**
     * @brief Log an error with context
     */
    void logError(const std::shared_ptr<NLMException>& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Log error to stderr for immediate visibility
        std::cerr << "ERROR: " << error->getFullErrorReport() << std::endl;
        
        // Store error for later analysis
        errors_.push_back(error);
        
        // Update error statistics
        updateErrorStatistics(error);
        
        // Apply error handling strategy
        handleError(error);
    }
    
    /**
     * @brief Log a warning
     */
    void logWarning(const std::string& message, const std::string& component = "Unknown") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::cerr << "WARNING: [" << component << "] " << message << std::endl;
        warnings_.push_back({message, component, std::chrono::high_resolution_clock::now()});
    }
    
    /**
     * @brief Log an informational message
     */
    void logInfo(const std::string& message, const std::string& component = "Unknown") {
        // Only log if logging is enabled
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::cout << "INFO: [" << component << "] " << message << std::endl;
    }
    
    /**
     * @brief Get the list of logged errors
     */
    const std::vector<std::shared_ptr<NLMException>>& getErrors() const { return errors_; }
    
    /**
     * @brief Get the list of logged warnings
     */
    const std::vector<std::pair<std::string, std::string>>& getWarnings() const { return warnings_; }
    
    /**
     * @brief Get the error statistics
     */
    std::string getErrorStatistics() const {
        std::stringstream ss;
        ss << "Error Statistics:" << std::endl;
        ss << "  Total Errors: " << errors_.size() << std::endl;
        ss << "  Total Warnings: " << warnings_.size() << std::endl;
        
        if (!errorCodeCounts_.empty()) {
            ss << "  Errors by Code:" << std::endl;
            for (const auto& pair : errorCodeCounts_) {
                ss << "    " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
            }
        }
        
        if (!errorComponentCounts_.empty()) {
            ss << "  Errors by Component:" << std::endl;
            for (const auto& pair : errorComponentCounts_) {
                ss << "    " << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        return ss.str();
    }
    
    /**
     * @brief Clear all logged errors and warnings
     */
    void clearLogs() {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.clear();
        warnings_.clear();
        errorCodeCounts_.clear();
        errorComponentCounts_.clear();
    }
    
    /**
     * @brief Get the most recent error
     */
    std::shared_ptr<NLMException> getMostRecentError() const {
        if (errors_.empty()) {
            return nullptr;
        }
        return errors_.back();
    }
    
    /**
     * @brief Check if any errors have occurred
     */
    bool hasErrors() const { return !errors_.empty(); }
    
    /**
     * @brief Set the error handling strategy
     */
    void setErrorHandlingStrategy(std::function<void(std::shared_ptr<NLMException>)> strategy) {
        errorHandlingStrategy_ = strategy;
    }
    
    /**
     * @brief Set whether to throw exceptions or just log them
     */
    void setThrowExceptions(bool throwExceptions) { throwExceptions_ = throwExceptions; }

private:
    ErrorHandler()
        : throwExceptions_(true)
    {
    }
    
    /**
     * @brief Update error statistics
     */
    void updateErrorStatistics(const std::shared_ptr<NLMException>& error) {
        // Update error code counts
        errorCodeCounts_[error->getErrorCode()]++;
        
        // Update component counts
        if (error->getContext()) {
            errorComponentCounts_[error->getContext()->getComponent()]++;
        }
    }
    
    /**
     * @brief Handle an error according to the configured strategy
     */
    void handleError(const std::shared_ptr<NLMException>& error) {
        if (errorHandlingStrategy_) {
            errorHandlingStrategy_(error);
        } else if (throwExceptions_) {
            throw *error;
        }
    }
    
    std::vector<std::shared_ptr<NLMException>> errors_;                                             // Logged errors
    std::vector<std::pair<std::string, std::string>> warnings_;                                   // Logged warnings
    std::unordered_map<ErrorCode, size_t> errorCodeCounts_;                                     // Errors by code
    std::unordered_map<std::string, size_t> errorComponentCounts_;                             // Errors by component
    std::function<void(std::shared_ptr<NLMException>)> errorHandlingStrategy_; // Error handling strategy
    bool throwExceptions_;                                                                      // Whether to throw exceptions
    mutable std::mutex mutex_;                                                                   // Thread safety
};

/**
 * @brief Global error handler instance
 * 
 * Provides global access to the error handling system.
 */
ErrorHandler& errorHandler = ErrorHandler::getInstance();

/**
 * @brief Helper macro for creating error contexts
 */
#define CREATE_ERROR_CONTEXT(errorCode, message, component) \
    std::make_shared<ErrorContext>(errorCode, message, component, __FILE__, __LINE__, __func__)

/**
 * @brief Helper macro for throwing NLM exceptions
 */
#define THROW_NLM_EXCEPTION(errorCode, message, component) \
    do { \
        auto context = CREATE_ERROR_CONTEXT(errorCode, message, component); \
        errorHandler.logError(std::make_shared<NLMException>(errorCode, message, context)); \
    } while(0)

/**
 * @brief Helper macro for throwing NLM exceptions with nested error
 */
#define THROW_NLM_EXCEPTION_NESTED(errorCode, message, component, parent) \
    do { \
        auto context = CREATE_ERROR_CONTEXT(errorCode, message, component); \
        if (parent) { \
            context->setParentError(parent->getContext()); \
        } \
        auto exception = std::make_shared<NLMException>(errorCode, message, context); \
        errorHandler.logError(exception); \
    } while(0)

/**
 * @brief Helper macro for throwing specific NLM exceptions
 */
#define THROW_CONFIGURATION_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::ConfigFileError, message, "Configuration")

#define THROW_BRAIN_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::BrainError, message, "Brain")

#define THROW_MEMORY_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::OutOfMemory, message, "Memory")

#define THROW_NEURON_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::NeuronError, message, "Neuron")

#define THROW_SYNAPSE_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::SynapseError, message, "Synapse")

#define THROW_PLASTICITY_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::PlasticityError, message, "Plasticity")

#define THROW_DEVELOPMENT_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::DevelopmentError, message, "Development")

#define THROW_NEUROMODULATION_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::NeuromodulationError, message, "Neuromodulation")

#define THROW_PERFORMANCE_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::PerformanceError, message, "Performance")

#define THROW_EXPORT_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::ExportError, message, "Export")

#define THROW_PLUGIN_EXCEPTION(message) \
    THROW_NLM_EXCEPTION(ErrorCode::PluginError, message, "Plugin")

} // namespace nlm

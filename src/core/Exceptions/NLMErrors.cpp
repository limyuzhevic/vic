#include "NLMErrors.hpp"

namespace nlm {

// Helper function to convert enum to string
std::string NLMException::errorCodeToString(ErrorCode code) {
    static const std::unordered_map<ErrorCode, std::string> errorCodeStrings = {
        {ErrorCode::Unknown, "Unknown"},
        {ErrorCode::NotImplemented, "Not implemented"},
        {ErrorCode::InvalidArgument, "Invalid argument"},
        {ErrorCode::OutOfRange, "Out of range"},
        {ErrorCode::NotFound, "Not found"},
        {ErrorCode::AlreadyExists, "Already exists"},
        {ErrorCode::AccessDenied, "Access denied"},
        {ErrorCode::PermissionDenied, "Permission denied"},
        {ErrorCode::Timeout, "Timeout"},
        {ErrorCode::Interrupted, "Interrupted"},
        
        {ErrorCode::ConfigFileNotFound, "Configuration file not found"},
        {ErrorCode::ConfigParseError, "Configuration parse error"},
        {ErrorCode::InvalidParameter, "Invalid parameter"},
        {ErrorCode::MissingRequiredParameter, "Missing required parameter"},
        
        {ErrorCode::InitializationFailed, "Initialization failed"},
        {ErrorCode::ResourceNotAvailable, "Resource not available"},
        {ErrorCode::InvalidState, "Invalid state"},
        
        {ErrorCode::SimulationStepFailed, "Simulation step failed"},
        {ErrorCode::InvalidStep, "Invalid simulation step"},
        {ErrorCode::ConvergenceFailed, "Convergence failed"},
        {ErrorCode::TimeStepTooSmall, "Time step too small"},
        {ErrorCode::NumericalInstability, "Numerical instability"},
        
        {ErrorCode::MemoryAllocationFailed, "Memory allocation failed"},
        {ErrorCode::MemoryCorruption, "Memory corruption"},
        {ErrorCode::OutOfMemory, "Out of memory"},
        {ErrorCode::InvalidDeallocation, "Invalid deallocation"},
        {ErrorCode::MemoryLeak, "Memory leak"},
        
        {ErrorCode::FileNotFound, "File not found"},
        {ErrorCode::FilePermissionDenied, "File permission denied"},
        {ErrorCode::FileReadError, "File read error"},
        {ErrorCode::FileWriteError, "File write error"},
        {ErrorCode::DirectoryNotFound, "Directory not found"},
        {ErrorCode::PathNotFound, "Path not found"},
        {ErrorCode::DeviceError, "Device error"},
        
        {ErrorCode::NeuralNetworkError, "Neural network error"},
        {ErrorCode::InvalidNetworkTopology, "Invalid network topology"},
        {ErrorCode::InvalidWeights, "Invalid weights"},
        {ErrorCode::ConvergenceError, "Convergence error"},
        {ErrorCode::ActivationError, "Activation error"},
        {ErrorCode::BackpropagationError, "Backpropagation error"},
        
        {ErrorCode::SystemError, "System error"},
        {ErrorCode::SystemCallFailed, "System call failed"},
        {ErrorCode::ResourceExhausted, "Resource exhausted"},
        {ErrorCode::InvalidOperation, "Invalid operation"},
        {ErrorCode::NotSupported, "Not supported"},
        {ErrorCode::OutOfBounds, "Out of bounds"}
    };
    
    auto it = errorCodeStrings.find(code);
    return it != errorCodeStrings.end() ? it->second : "Unknown error";
}

// Constructor
NLMException::NLMException(ErrorCode code, const std::string& message,
                           const std::string& file,
                           int line,
                           const std::string& function)
    : code_(code), message_(message), timestamp_(std::chrono::system_clock::now()) {
    if (!file.empty()) {
        file_ = file;
    } else {
        file_ = "";
    }
    
    if (line > 0) {
        line_ = line;
    } else {
        line_ = 0;
    }
    
    if (!function.empty()) {
        function_ = function;
    } else {
        function_ = "";
    }
}

// Copy constructor
NLMException::NLMException(const NLMException& other)
    : message_(other.message_), code_(other.code_), file_(other.file_),
      line_(other.line_), function_(other.function_),
      timestamp_(other.timestamp_) {
    if (other.cause_) {
        // Use dynamic_cast to determine the actual type and clone it
        // This is a safer approach for abstract base classes
        cause_ = other.cause_->clone();
        if (!cause_) {
            // If clone() returns nullptr (as in our default implementation),
            // we need to handle this case gracefully
            // In a real implementation, derived classes should override clone()
        }
    }
    metadata_ = other.metadata_;
}

// Copy assignment operator
NLMException& NLMException::operator=(const NLMException& other) {
    if (this != &other) {
        message_ = other.message_;
        code_ = other.code_;
        file_ = other.file_;
        line_ = other.line_;
        function_ = other.function_;
        timestamp_ = other.timestamp_;
        
        if (other.cause_) {
            cause_ = other.cause_->clone();
        } else {
            cause_.reset();
        }
        
        metadata_ = other.metadata_;
    }
    return *this;
}

// Move constructor
NLMException::NLMException(NLMException&& other) noexcept
    : message_(std::move(other.message_)), code_(other.code_),
      file_(std::move(other.file_)), line_(other.line_),
      function_(std::move(other.function_)),
      timestamp_(other.timestamp_), cause_(std::move(other.cause_)),
      metadata_(std::move(other.metadata_)) {}

// Move assignment operator
NLMException& NLMException::operator=(NLMException&& other) noexcept {
    if (this != &other) {
        message_ = std::move(other.message_);
        code_ = other.code_;
        file_ = std::move(other.file_);
        line_ = other.line_;
        function_ = std::move(other.function_);
        timestamp_ = other.timestamp_;
        cause_ = std::move(other.cause_);
        metadata_ = std::move(other.metadata_);
    }
    return *this;
}

// what() method
const char* NLMException::what() const noexcept {
    return message_.c_str();
}

// serialize() method
std::string NLMException::serialize() const {
    std::ostringstream oss;
    oss << "[ErrorCode=" << static_cast<int>(code_) << "]";
    oss << "[Message=" << message_ << "]";
    oss << "[File=" << file_ << "]";
    oss << "[Line=" << line_ << "]";
    oss << "[Function=" << function_ << "]";
    
    auto time = std::chrono::system_clock::to_time_t(timestamp_);
    oss << "[Timestamp=" << time << "]";
    
    if (cause_) {
        oss << "[Cause=" << cause_->serialize() << "]";
    }
    
    for (const auto& pair : metadata_) {
        oss << "[" << pair.first << "=" << pair.second << "]";
    }
    
    return oss.str();
}

// formatWithCause() method
std::string NLMException::formatWithCause() const {
    std::ostringstream oss;
    oss << "Exception: " << message_;
    oss << " (Code: " << static_cast<int>(code_) << ")";
    oss << " at " << file_ << ":" << line_;
    
    if (hasCause()) {
        oss << " (Caused by: " << cause_->getMessage();
        oss << " (Code: " << static_cast<int>(cause_->getErrorCode()) << ")";
        oss << " at " << cause_->getFile() << ":" << cause_->getLine() << ")";
    }
    
    return oss.str();
}

// clone() method
std::unique_ptr<NLMException> NLMException::clone() const {
    // This is a pure virtual function, but we need to provide a default implementation
    // Since NLMException is abstract, we can't call this directly
    // Derived classes must implement this method
    // For safety, we'll return nullptr and let derived classes override
    return nullptr;
}

// ConfigurationException implementations
ConfigurationException::ConfigurationException(ConfigurationErrorCode code, const std::string& message,
                                              const std::string& file,
                                              int line,
                                              const std::string& function)
    : NLMException(static_cast<ErrorCode>(code), message, file, line, function) {}

std::string ConfigurationException::serialize() const {
    std::string base = NLMException::serialize();
    std::ostringstream oss;
    oss << "[ConfigErrorCode=" << static_cast<int>(getConfigurationErrorCode()) << "]";
    oss << base;
    return oss.str();
}

std::unique_ptr<NLMException> ConfigurationException::clone() const {
    auto cloned = std::make_unique<ConfigurationException>(
        getConfigurationErrorCode(), getMessage(), getFile(), getLine(), getFunction());
    
    // Copy cause if present
    if (hasCause()) {
        cloned->setCause(std::shared_ptr<NLMException>(cause_->clone()));
    }
    
    // Copy metadata
    cloned->metadata_ = metadata_;
    
    return cloned;
}

// InitializationException implementations
InitializationException::InitializationException(InitializationErrorCode code, const std::string& message,
                                                 const std::string& file,
                                                 int line,
                                                 const std::string& function)
    : NLMException(static_cast<ErrorCode>(code), message, file, line, function) {}

std::string InitializationException::serialize() const {
    std::string base = NLMException::serialize();
    std::ostringstream oss;
    oss << "[InitErrorCode=" << static_cast<int>(getInitializationErrorCode()) << "]";
    oss << base;
    return oss.str();
}

std::unique_ptr<NLMException> InitializationException::clone() const {
    auto cloned = std::make_unique<InitializationException>(
        getInitializationErrorCode(), getMessage(), getFile(), getLine(), getFunction());
    
    // Copy cause if present
    if (hasCause()) {
        cloned->setCause(std::shared_ptr<NLMException>(cause_->clone()));
    }
    
    // Copy metadata
    cloned->metadata_ = metadata_;
    
    return cloned;
}

// SimulationException implementations
SimulationException::SimulationException(SimulationErrorCode code, const std::string& message,
                                         const std::string& file,
                                         int line,
                                         const std::string& function)
    : NLMException(static_cast<ErrorCode>(code), message, file, line, function) {}

std::string SimulationException::serialize() const {
    std::string base = NLMException::serialize();
    std::ostringstream oss;
    oss << "[SimErrorCode=" << static_cast<int>(getSimulationErrorCode()) << "]";
    oss << base;
    return oss.str();
}

std::unique_ptr<NLMException> SimulationException::clone() const {
    auto cloned = std::make_unique<SimulationException>(
        getSimulationErrorCode(), getMessage(), getFile(), getLine(), getFunction());
    
    // Copy cause if present
    if (hasCause()) {
        cloned->setCause(std::shared_ptr<NLMException>(cause_->clone()));
    }
    
    // Copy metadata
    cloned->metadata_ = metadata_;
    
    return cloned;
}

// MemoryException implementations
MemoryException::MemoryException(MemoryErrorCode code, const std::string& message,
                                const std::string& file,
                                int line,
                                const std::string& function)
    : NLMException(static_cast<ErrorCode>(code), message, file, line, function) {}

std::string MemoryException::serialize() const {
    std::string base = NLMException::serialize();
    std::ostringstream oss;
    oss << "[MemErrorCode=" << static_cast<int>(getMemoryErrorCode()) << "]";
    oss << base;
    return oss.str();
}

std::unique_ptr<NLMException> MemoryException::clone() const {
    auto cloned = std::make_unique<MemoryException>(
        getMemoryErrorCode(), getMessage(), getFile(), getLine(), getFunction());
    
    // Copy cause if present
    if (hasCause()) {
        cloned->setCause(std::shared_ptr<NLMException>(cause_->clone()));
    }
    
    // Copy metadata
    cloned->metadata_ = metadata_;
    
    return cloned;
}

// IOException implementations
IOException::IOException(IOErrorCode code, const std::string& message,
                     const std::string& file,
                     int line,
                     const std::string& function)
    : NLMException(static_cast<ErrorCode>(code), message, file, line, function) {}

std::string IOException::serialize() const {
    std::string base = NLMException::serialize();
    std::ostringstream oss;
    oss << "[IOErrorCode=" << static_cast<int>(getIOErrorCode()) << "]";
    oss << base;
    return oss.str();
}

std::unique_ptr<NLMException> IOException::clone() const {
    auto cloned = std::make_unique<IOException>(
        getIOErrorCode(), getMessage(), getFile(), getLine(), getFunction());
    
    // Copy cause if present
    if (hasCause()) {
        cloned->setCause(std::shared_ptr<NLMException>(cause_->clone()));
    }
    
    // Copy metadata
    cloned->metadata_ = metadata_;
    
    return cloned;
}

// NeuralException implementations
NeuralException::NeuralException(NeuralErrorCode code, const std::string& message,
                               const std::string& file,
                               int line,
                               const std::string& function)
    : NLMException(static_cast<ErrorCode>(code), message, file, line, function) {}

std::string NeuralException::serialize() const {
    std::string base = NLMException::serialize();
    std::ostringstream oss;
    oss << "[NeuralErrorCode=" << static_cast<int>(getNeuralErrorCode()) << "]";
    oss << base;
    return oss.str();
}

std::unique_ptr<NLMException> NeuralException::clone() const {
    auto cloned = std::make_unique<NeuralException>(
        getNeuralErrorCode(), getMessage(), getFile(), getLine(), getFunction());
    
    // Copy cause if present
    if (hasCause()) {
        cloned->setCause(std::shared_ptr<NLMException>(cause_->clone()));
    }
    
    // Copy metadata
    cloned->metadata_ = metadata_;
    
    return cloned;
}

// SystemException implementations
SystemException::SystemException(SystemErrorCode code, const std::string& message,
                                 const std::string& file,
                                 int line,
                                 const std::string& function)
    : NLMException(static_cast<ErrorCode>(code), message, file, line, function) {}

std::string SystemException::serialize() const {
    std::string base = NLMException::serialize();
    std::ostringstream oss;
    oss << "[SysErrorCode=" << static_cast<int>(getSystemErrorCode()) << "]";
    oss << base;
    return oss.str();
}

std::unique_ptr<NLMException> SystemException::clone() const {
    auto cloned = std::make_unique<SystemException>(
        getSystemErrorCode(), getMessage(), getFile(), getLine(), getFunction());
    
    // Copy cause if present
    if (hasCause()) {
        cloned->setCause(std::shared_ptr<NLMException>(cause_->clone()));
    }
    
    // Copy metadata
    cloned->metadata_ = metadata_;
    
    return cloned;
}

}
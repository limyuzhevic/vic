#include "NLMError.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace nlm {

// Error message lookup table
static const std::map<ErrorCode, std::string> errorMessages = {
    {ErrorCode::Success, "Success"},
    {ErrorCode::Unknown, "Unknown error"},
    {ErrorCode::NotImplemented, "Operation not implemented"},
    {ErrorCode::InvalidArgument, "Invalid argument"},
    {ErrorCode::OutOfRange, "Value out of range"},
    {ErrorCode::NotFound, "Not found"},
    {ErrorCode::AlreadyExists, "Already exists"},
    {ErrorCode::PermissionDenied, "Permission denied"},
    {ErrorCode::ResourceExhausted, "Resource exhausted"},
    {ErrorCode::TimedOut, "Operation timed out"},
    {ErrorCode::Interrupted, "Operation interrupted"},
    
    {ErrorCode::FileNotFound, "File not found"},
    {ErrorCode::FileAccessDenied, "File access denied"},
    {ErrorCode::FileCorrupted, "File is corrupted"},
    {ErrorCode::FileReadError, "File read error"},
    {ErrorCode::FileWriteError, "File write error"},
    
    {ErrorCode::NeuronNotFound, "Neuron not found"},
    {ErrorCode::SynapseNotFound, "Synapse not found"},
    {ErrorCode::RegionNotFound, "Region not found"},
    {ErrorCode::InvalidConnection, "Invalid connection"},
    {ErrorCode::NetworkNotInitialized, "Network not initialized"},
    {ErrorCode::InvalidNetworkState, "Invalid network state"},
    
    {ErrorCode::MemoryAllocationFailed, "Memory allocation failed"},
    {ErrorCode::MemoryCorruption, "Memory corruption detected"},
    
    {ErrorCode::ConfigInvalid, "Invalid configuration"},
    {ErrorCode::ConfigFileError, "Configuration file error"},
    
    {ErrorCode::SimulationNotRunning, "Simulation not running"},
    {ErrorCode::InvalidTimeStep, "Invalid time step"},
    {ErrorCode::StepExecutionFailed, "Step execution failed"},
    
    {ErrorCode::IntegrationFailed, "Integration failed"},
    {ErrorCode::ComponentNotAvailable, "Component not available"},
    
    {ErrorCode::PerformanceThresholdExceeded, "Performance threshold exceeded"}
};

// Helper function to get current timestamp
static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

NLMException::NLMException(const std::string& message, ErrorCode code, const ErrorContext& context)
    : std::runtime_error(getCurrentTimestamp() + " [" + errorMessages.at(code) + "]: " + message), 
      code_(code), context_(context) {
    // Log the error using the logger
    std::stringstream fullMessage;
    fullMessage << "Exception: " << message;
    if (!context.operation.empty()) {
        fullMessage << " (Operation: " << context.operation << ")";
    }
    if (!context.component.empty()) {
        fullMessage << " (Component: " << context.component << ")";
    }
    Logger::getGlobal().error(fullMessage.str(), context.file.empty() ? "" : context.file, 0, "");
}

NLMException::NLMException(ErrorCode code, const ErrorContext& context)
    : std::runtime_error(getCurrentTimestamp() + " [" + errorMessages.at(code) + "]"),
      code_(code), context_(context) {
    Logger::getGlobal().error("Exception occurred", context.file.empty() ? "" : context.file, 0, "");
}

std::string NLMException::getFullMessage() const {
    std::stringstream ss;
    ss << what() << "\n";
    ss << "Error Code: " << static_cast<int>(code_) << "\n";
    ss << "Operation: " << context_.operation << "\n";
    ss << "Component: " << context_.component << "\n";
    ss << "Details: " << context_.details << "\n";
    if (!context_.suggestion.empty()) {
        ss << "Suggestion: " << context_.suggestion << "\n";
    }
    return ss.str();
}

ValidationException::ValidationException(const std::string& message, const ErrorContext& context)
    : NLMException(message, ErrorCode::InvalidArgument, context) {}

FileSystemException::FileSystemException(const std::string& message, ErrorCode code, const ErrorContext& context)
    : NLMException(message, code, context) {}

NetworkException::NetworkException(const std::string& message, ErrorCode code, const ErrorContext& context)
    : NLMException(message, code, context) {}

MemoryException::MemoryException(const std::string& message, const ErrorContext& context)
    : NLMException(message, ErrorCode::MemoryAllocationFailed, context) {}

ConfigurationException::ConfigurationException(const std::string& message, ErrorCode code, const ErrorContext& context)
    : NLMException(message, code, context) {}

SimulationException::SimulationException(const std::string& message, ErrorCode code, const ErrorContext& context)
    : NLMException(message, code, context) {}

// Static members initialization
ErrorCode ErrorHandler::lastErrorCode_ = ErrorCode::Success;
std::string ErrorHandler::lastErrorMessage_ = "";

void ErrorHandler::handle(const NLMException& error) {
    lastErrorCode_ = error.getCode();
    lastErrorMessage_ = error.getFullMessage();
    
    std::stringstream ss;
    ss << "Handled " << errorMessages.at(error.getCode()) << ": " << error.what();
    
    if (error.getContext().operation != "") {
        ss << " (Operation: " << error.getContext().operation << ")";
    }
    
    Logger::getGlobal().warning(ss.str());
}

void ErrorHandler::handle(const std::exception& error) {
    lastErrorCode_ = ErrorCode::Unknown;
    lastErrorMessage_ = error.what();
    
    Logger::getGlobal().warning(std::string("Unhandled exception: ") + error.what());
}

void ErrorHandler::handle(ErrorCode code, const std::string& message, const ErrorContext& context) {
    NLMException error(message, code, context);
    handle(error);
}

std::string ErrorHandler::errorCodeToString(ErrorCode code) {
    auto it = errorMessages.find(code);
    return it != errorMessages.end() ? it->second : "Unknown error code: " + std::to_string(static_cast<int>(code));
}

bool ErrorHandler::canRecover(ErrorCode code) {
    switch (code) {
        case ErrorCode::FileNotFound:
        case ErrorCode::FileAccessDenied:
        case ErrorCode::InvalidArgument:
        case ErrorCode::OutOfRange:
        case ErrorCode::ConfigInvalid:
            return true;
        case ErrorCode::MemoryAllocationFailed:
        case ErrorCode::MemoryCorruption:
            return false; // Cannot recover from memory errors
        default:
            return false;
    }
}

bool ErrorHandler::attemptRecovery(NLMException& error) {
    ErrorCode code = error.getCode();
    
    if (!canRecover(code)) {
        return false;
    }
    
    switch (code) {
        case ErrorCode::FileNotFound:
            Logger::getGlobal().warning("File not found - attempting to create default configuration");
            // Recovery logic could go here
            break;
        case ErrorCode::InvalidArgument:
            Logger::getGlobal().warning("Invalid argument - attempting to use default value");
            // Recovery logic could go here
            break;
        case ErrorCode::ConfigInvalid:
            Logger::getGlobal().warning("Invalid configuration - attempting to restore defaults");
            // Recovery logic could go here
            break;
        default:
            break;
    }
    
    return true;
}

} // namespace nlm
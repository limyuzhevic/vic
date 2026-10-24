#include "ErrorHandling.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>

namespace nlm {
namespace error {

// NLMException implementation
NLMException::NLMException(ErrorCode code, const std::string& message, 
                           const ErrorContext& context, const std::string& suggestion,
                           RecoveryAction recovery)
    : std::runtime_error(message), code_(code), context_(context), 
      suggestion_(suggestion), recovery_(recovery) {
    
    category_ = mapCodeToCategory(code);
    severity_ = mapCodeToSeverity(code);
}

std::string NLMException::getFullMessage() const {
    std::ostringstream oss;
    oss << "Error Code: " << static_cast<int>(code_) << " (" << errorCodeToString(code_) << ")";
    oss << "\nCategory: " << errorCategoryToString(category_);
    oss << "\nSeverity: " << errorSeverityToString(severity_);
    oss << "\nMessage: " << what();
    oss << "\nLocation: " << getLocationInfo();
    
    if (!suggestion_.empty()) {
        oss << "\nSuggestion: " << suggestion_;
    }
    
    return oss.str();
}

std::string NLMException::getLocationInfo() const {
    std::ostringstream oss;
    if (!context_.file.empty()) {
        oss << context_.file;
        if (context_.line > 0) {
            oss << ":" << context_.line;
            if (context_.column > 0) {
                oss << ":" << context_.column;
            }
        }
    }
    if (!context_.function.empty()) {
        if (!context_.file.empty()) oss << " in ";
        oss << context_.function;
    }
    return oss.str();
}

ErrorCategory NLMException::mapCodeToCategory(ErrorCode code) {
    switch (static_cast<int>(code)) {
        case static_cast<int>(ErrorCode::ConfigFileNotFound):
        case static_cast<int>(ErrorCode::ConfigParseError):
        case static_cast<int>(ErrorCode::ConfigValidationError):
        case static_cast<int>(ErrorCode::ConfigMissingRequiredField):
        case static_cast<int>(ErrorCode::ConfigInvalidValue):
        case static_cast<int>(ErrorCode::ConfigDuplicateKey):
        case static_cast<int>(ErrorCode::ConfigSchemaMismatch):
            return ErrorCategory::Configuration;
            
        case static_cast<int>(ErrorCode::BrainInitializationError):
        case static_cast<int>(ErrorCode::NeuronCreationError):
        case static_cast<int>(ErrorCode::SynapseCreationError):
        case static_cast<int>(ErrorCode::RegionError):
        case static_cast<int>(ErrorCode::PopulationError):
        case static_cast<int>(ErrorCode::SpikeSystemError):
        case static_cast<int>(ErrorCode::PlasticityError):
            return ErrorCategory::NeuralSystem;
            
        case static_cast<int>(ErrorCode::ExperimentNotFound):
        case static_cast<int>(ErrorCode::ExperimentLoadError):
        case static_cast<int>(ErrorCode::ExperimentRunError):
        case static_cast<int>(ErrorCode::ExperimentSaveError):
        case static_cast<int>(ErrorCode::ExperimentInvalidConfig):
            return ErrorCategory::Experiment;
            
        case static_cast<int>(ErrorCode::MemoryAllocationError):
        case static_cast<int>(ErrorCode::MemoryPoolError):
        case static_cast<int>(ErrorCode::WorkingMemoryError):
        case static_cast<int>(ErrorCode::EpisodicMemoryError):
        case static_cast<int>(ErrorCode::AssociativeMemoryError):
            return ErrorCategory::Memory;
            
        case static_cast<int>(ErrorCode::FileNotFound):
        case static_cast<int>(ErrorCode::FileReadError):
        case static_cast<int>(ErrorCode::FileWriteError):
        case static_cast<int>(ErrorCode::DirectoryError):
        case static_cast<int>(ErrorCode::PermissionError):
            return ErrorCategory::FileSystem;
            
        case static_cast<int>(ErrorCode::ConnectionError):
        case static_cast<int>(ErrorCode::NetworkTimeout):
        case static_cast<int>(ErrorCode::ProtocolError):
            return ErrorCategory::Network;
            
        case static_cast<int>(ErrorCode::MemoryError):
        case static_cast<int>(ErrorCode::CpuError):
        case static_cast<int>(ErrorCode::GpuError):
        case static_cast<int>(ErrorCode::DiskError):
            return ErrorCategory::Resource;
            
        case static_cast<int>(ErrorCode::SystemError):
        case static_cast<int>(ErrorCode::InitializationError):
        case static_cast<int>(ErrorCode::RuntimeError):
        case static_cast<int>(ErrorCode::TerminationError):
            return ErrorCategory::System;
            
        case static_cast<int>(ErrorCode::CliArgumentError):
        case static_cast<int>(ErrorCode::CliCommandNotFound):
        case static_cast<int>(ErrorCode::CliSubcommandError):
        case static_cast<int>(ErrorCode::CliParseError):
            return ErrorCategory::Cli;
            
        default:
            return ErrorCategory::General;
    }
}

ErrorSeverity NLMException::mapCodeToSeverity(ErrorCode code) {
    switch (code) {
        case ErrorCode::Success:
        case ErrorCode::NotImplemented:
            return ErrorSeverity::Info;
            
        case ErrorCode::ConfigFileNotFound:
        case ErrorCode::FileNotFound:
        case ErrorCode::CliCommandNotFound:
            return ErrorSeverity::Warning;
            
        case ErrorCode::ConfigParseError:
        case ErrorCode::ConfigValidationError:
        case ErrorCode::ExperimentLoadError:
        case ErrorCode::ExperimentInvalidConfig:
        case ErrorCode::FileReadError:
        case ErrorCode::InvalidArgument:
        case ErrorCode::OutOfRange:
            return ErrorSeverity::Error;
            
        case ErrorCode::BrainInitializationError:
        case ErrorCode::NeuronCreationError:
        case ErrorCode::SpikeSystemError:
        case ErrorCode::MemoryError:
        case ErrorCode::SystemError:
        case ErrorCode::UnknownError:
            return ErrorSeverity::Critical;
            
        case ErrorCode::MemoryAllocationError:
        case ErrorCode::CliArgumentError:
        case ErrorCode::CliParseError:
            return ErrorSeverity::Error;
            
        case ErrorCode::Timeout:
        case ErrorCode::Interrupted:
            return ErrorSeverity::Warning;
            
        default:
            return ErrorSeverity::Error;
    }
}

// Specialized exception implementations
ConfigException::ConfigException(ErrorCode code, const std::string& message,
                               const ErrorContext& context, const std::string& suggestion)
    : NLMException(code, message, context, suggestion) {}

BrainException::BrainException(ErrorCode code, const std::string& message,
                              const ErrorContext& context, const std::string& suggestion)
    : NLMException(code, message, context, suggestion) {}

ExperimentException::ExperimentException(ErrorCode code, const std::string& message,
                                        const ErrorContext& context, const std::string& suggestion)
    : NLMException(code, message, context, suggestion) {}

MemoryException::MemoryException(ErrorCode code, const std::string& message,
                                const ErrorContext& context, const std::string& suggestion)
    : NLMException(code, message, context, suggestion) {}

FileException::FileException(ErrorCode code, const std::string& message,
                             const ErrorContext& context, const std::string& suggestion)
    : NLMException(code, message, context, suggestion) {}

CliException::CliException(ErrorCode code, const std::string& message,
                           const ErrorContext& context, const std::string& suggestion)
    : NLMException(code, message, context, suggestion) {}

// ErrorReporter implementation
namespace {
    std::unique_ptr<ErrorReporter> defaultReporter_;
}

ErrorReporter& ErrorReporter::getDefault() {
    if (!defaultReporter_) {
        defaultReporter_ = std::make_unique<ConsoleErrorReporter>();
    }
    return *defaultReporter_;
}

void ErrorReporter::setDefault(std::unique_ptr<ErrorReporter> reporter) {
    defaultReporter_ = std::move(reporter);
}

// ConsoleErrorReporter implementation
void ConsoleErrorReporter::report(const NLMException& error) {
    errorBuffer_.push_back(formatError(error));
    std::cerr << formatError(error) << std::endl;
}

void ConsoleErrorReporter::log(const std::string& message, ErrorSeverity severity) {
    logBuffer_.push_back(severityToString(severity) + ": " + message);
    std::cout << severityToString(severity) << ": " << message << std::endl;
}

void ConsoleErrorReporter::clear() {
    errorBuffer_.clear();
    logBuffer_.clear();
}

std::string ConsoleErrorReporter::severityToString(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Debug: return "DEBUG";
        case ErrorSeverity::Info: return "INFO";
        case ErrorSeverity::Warning: return "WARNING";
        case ErrorSeverity::Error: return "ERROR";
        case ErrorSeverity::Critical: return "CRITICAL";
        case ErrorSeverity::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string ConsoleErrorReporter::formatError(const NLMException& error) {
    std::ostringstream oss;
    oss << "[" << severityToString(error.getSeverity()) << "] "
        << "[" << errorCodeToString(error.getCode()) << "] "
        << error.what();
    return oss.str();
}

// FileErrorReporter implementation
FileErrorReporter::FileErrorReporter(const std::string& filepath) : filepath_(filepath) {
    logFile_.open(filepath_, std::ios::app | std::ios::out);
    if (!logFile_.good()) {
        throw std::runtime_error("Failed to open log file: " + filepath_);
    }
}

FileErrorReporter::~FileErrorReporter() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

void FileErrorReporter::report(const NLMException& error) {
    std::string formatted = formatError(error);
    errorBuffer_.push_back(formatted);
    logFile_ << formatted << std::endl;
}

void FileErrorReporter::log(const std::string& message, ErrorSeverity severity) {
    std::string formatted = severityToString(severity) + ": " + message;
    logBuffer_.push_back(formatted);
    logFile_ << formatted << std::endl;
}

void FileErrorReporter::clear() {
    errorBuffer_.clear();
    logBuffer_.clear();
    if (logFile_.is_open()) {
        logFile_.close();
        logFile_.open(filepath_, std::ios::trunc | std::ios::out);
    }
}

// ErrorRecoveryManager implementation
ErrorRecoveryManager::ErrorRecoveryManager() = default;

void ErrorRecoveryManager::registerHandler(ErrorCode code, std::function<bool(const NLMException&)> handler) {
    errorHandlers_[code] = std::move(handler);
}

bool ErrorRecoveryManager::handleError(const NLMException& error) {
    auto it = errorHandlers_.find(error.getCode());
    if (it != errorHandlers_.end()) {
        return it->second(error);
    }
    return false;
}

bool ErrorRecoveryManager::shouldRecover(const NLMException& error) const {
    RecoveryAction action = getRecoveryAction(error);
    return action != RecoveryAction::None && action != RecoveryAction::Abort && action != RecoveryAction::Shutdown;
}

RecoveryAction ErrorRecoveryManager::getRecoveryAction(const NLMException& error) const {
    switch (error.getCode()) {
        case ErrorCode::ConfigFileNotFound:
        case ErrorCode::FileNotFound:
            return RecoveryAction::Retry;
            
        case ErrorCode::ConfigParseError:
        case ErrorCode::ConfigValidationError:
        case ErrorCode::InvalidArgument:
            return RecoveryAction::Ignore;
            
        case ErrorCode::MemoryError:
        case ErrorCode::BrainInitializationError:
            return RecoveryAction::Reset;
            
        case ErrorCode::Timeout:
            return RecoveryAction::Retry;
            
        case ErrorCode::OutOfRange:
            return RecoveryAction::Fallback;
            
        default:
            return RecoveryAction::None;
    }
}

// ExceptionFactory implementation
NLMException ExceptionFactory::fromCode(ErrorCode code, const std::string& message,
                                       const ErrorContext& context, const std::string& suggestion) {
    switch (code) {
        case ErrorCode::ConfigFileNotFound:
        case ErrorCode::ConfigParseError:
        case ErrorCode::ConfigValidationError:
        case ErrorCode::ConfigMissingRequiredField:
        case ErrorCode::ConfigInvalidValue:
        case ErrorCode::ConfigDuplicateKey:
        case ErrorCode::ConfigSchemaMismatch:
            return ConfigException(code, message, context, suggestion);
            
        case ErrorCode::BrainInitializationError:
        case ErrorCode::NeuronCreationError:
        case ErrorCode::SynapseCreationError:
        case ErrorCode::RegionError:
        case ErrorCode::PopulationError:
        case ErrorCode::SpikeSystemError:
        case ErrorCode::PlasticityError:
            return BrainException(code, message, context, suggestion);
            
        case ErrorCode::ExperimentNotFound:
        case ErrorCode::ExperimentLoadError:
        case ErrorCode::ExperimentRunError:
        case ErrorCode::ExperimentSaveError:
        case ErrorCode::ExperimentInvalidConfig:
            return ExperimentException(code, message, context, suggestion);
            
        case ErrorCode::MemoryAllocationError:
        case ErrorCode::MemoryPoolError:
        case ErrorCode::WorkingMemoryError:
        case ErrorCode::EpisodicMemoryError:
        case ErrorCode::AssociativeMemoryError:
            return MemoryException(code, message, context, suggestion);
            
        case ErrorCode::FileNotFound:
        case ErrorCode::FileReadError:
        case ErrorCode::FileWriteError:
        case ErrorCode::DirectoryError:
        case ErrorCode::PermissionError:
            return FileException(code, message, context, suggestion);
            
        case ErrorCode::CliArgumentError:
        case ErrorCode::CliCommandNotFound:
        case ErrorCode::CliSubcommandError:
        case ErrorCode::CliParseError:
            return CliException(code, message, context, suggestion);
            
        default:
            return NLMException(code, message, context, suggestion);
    }
}

std::string ExceptionFactory::errorCodeToString(ErrorCode code) {
    static const std::map<ErrorCode, std::string> errorStrings = {
        {ErrorCode::Success, "SUCCESS"},
        {ErrorCode::UnknownError, "UNKNOWN_ERROR"},
        {ErrorCode::NotImplemented, "NOT_IMPLEMENTED"},
        {ErrorCode::OutOfRange, "OUT_OF_RANGE"},
        {ErrorCode::InvalidArgument, "INVALID_ARGUMENT"},
        {ErrorCode::Timeout, "TIMEOUT"},
        {ErrorCode::Interrupted, "INTERRUPTED"},
        {ErrorCode::ConfigFileNotFound, "CONFIG_FILE_NOT_FOUND"},
        {ErrorCode::ConfigParseError, "CONFIG_PARSE_ERROR"},
        {ErrorCode::ConfigValidationError, "CONFIG_VALIDATION_ERROR"},
        {ErrorCode::ConfigMissingRequiredField, "CONFIG_MISSING_REQUIRED_FIELD"},
        {ErrorCode::ConfigInvalidValue, "CONFIG_INVALID_VALUE"},
        {ErrorCode::ConfigDuplicateKey, "CONFIG_DUPLICATE_KEY"},
        {ErrorCode::ConfigSchemaMismatch, "CONFIG_SCHEMA_MISMATCH"},
        {ErrorCode::BrainInitializationError, "BRAIN_INITIALIZATION_ERROR"},
        {ErrorCode::NeuronCreationError, "NEURON_CREATION_ERROR"},
        {ErrorCode::SynapseCreationError, "SYNAPSE_CREATION_ERROR"},
        {ErrorCode::RegionError, "REGION_ERROR"},
        {ErrorCode::PopulationError, "POPULATION_ERROR"},
        {ErrorCode::SpikeSystemError, "SPIKE_SYSTEM_ERROR"},
        {ErrorCode::PlasticityError, "PLASTICITY_ERROR"},
        {ErrorCode::ExperimentNotFound, "EXPERIMENT_NOT_FOUND"},
        {ErrorCode::ExperimentLoadError, "EXPERIMENT_LOAD_ERROR"},
        {ErrorCode::ExperimentRunError, "EXPERIMENT_RUN_ERROR"},
        {ErrorCode::ExperimentSaveError, "EXPERIMENT_SAVE_ERROR"},
        {ErrorCode::ExperimentInvalidConfig, "EXPERIMENT_INVALID_CONFIG"},
        {ErrorCode::MemoryAllocationError, "MEMORY_ALLOCATION_ERROR"},
        {ErrorCode::MemoryPoolError, "MEMORY_POOL_ERROR"},
        {ErrorCode::WorkingMemoryError, "WORKING_MEMORY_ERROR"},
        {ErrorCode::EpisodicMemoryError, "EPISODIC_MEMORY_ERROR"},
        {ErrorCode::AssociativeMemoryError, "ASSOCIATIVE_MEMORY_ERROR"},
        {ErrorCode::FileNotFound, "FILE_NOT_FOUND"},
        {ErrorCode::FileReadError, "FILE_READ_ERROR"},
        {ErrorCode::FileWriteError, "FILE_WRITE_ERROR"},
        {ErrorCode::DirectoryError, "DIRECTORY_ERROR"},
        {ErrorCode::PermissionError, "PERMISSION_ERROR"},
        {ErrorCode::ConnectionError, "CONNECTION_ERROR"},
        {ErrorCode::NetworkTimeout, "NETWORK_TIMEOUT"},
        {ErrorCode::ProtocolError, "PROTOCOL_ERROR"},
        {ErrorCode::MemoryError, "MEMORY_ERROR"},
        {ErrorCode::CpuError, "CPU_ERROR"},
        {ErrorCode::GpuError, "GPU_ERROR"},
        {ErrorCode::DiskError, "DISK_ERROR"},
        {ErrorCode::SystemError, "SYSTEM_ERROR"},
        {ErrorCode::InitializationError, "INITIALIZATION_ERROR"},
        {ErrorCode::RuntimeError, "RUNTIME_ERROR"},
        {ErrorCode::TerminationError, "TERMINATION_ERROR"},
        {ErrorCode::CliArgumentError, "CLI_ARGUMENT_ERROR"},
        {ErrorCode::CliCommandNotFound, "CLI_COMMAND_NOT_FOUND"},
        {ErrorCode::CliSubcommandError, "CLI_SUBCOMMAND_ERROR"},
        {ErrorCode::CliParseError, "CLI_PARSE_ERROR"},
    };
    
    auto it = errorStrings.find(code);
    return it != errorStrings.end() ? it->second : "UNKNOWN_ERROR_CODE";
}

std::string ExceptionFactory::errorCategoryToString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::Configuration: return "CONFIGURATION";
        case ErrorCategory::NeuralSystem: return "NEURAL_SYSTEM";
        case ErrorCategory::Experiment: return "EXPERIMENT";
        case ErrorCategory::Memory: return "MEMORY";
        case ErrorCategory::FileSystem: return "FILE_SYSTEM";
        case ErrorCategory::Network: return "NETWORK";
        case ErrorCategory::Resource: return "RESOURCE";
        case ErrorCategory::System: return "SYSTEM";
        case ErrorCategory::Cli: return "CLI";
        case ErrorCategory::General: return "GENERAL";
        default: return "UNKNOWN_CATEGORY";
    }
}

std::string ExceptionFactory::errorSeverityToString(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Debug: return "DEBUG";
        case ErrorSeverity::Info: return "INFO";
        case ErrorSeverity::Warning: return "WARNING";
        case ErrorSeverity::Error: return "ERROR";
        case ErrorSeverity::Critical: return "CRITICAL";
        case ErrorSeverity::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

// Error utilities implementation
namespace utils {
    std::string formatError(const NLMException& error) {
        return ExceptionFactory::errorCodeToString(error.getCode()) + ": " + error.what();
    }
    
    std::string formatError(const std::exception& error) {
        return "Exception: " + std::string(error.what());
    }
    
    std::string getErrorSummary(const std::vector<NLMException>& errors) {
        if (errors.empty()) {
            return "No errors";
        }
        
        std::ostringstream oss;
        oss << "Error summary (" << errors.size() << " errors):";
        for (const auto& error : errors) {
            oss << "\n  - " << formatError(error);
        }
        return oss.str();
    }
    
    bool shouldRetry(const NLMException& error) {
        ErrorRecoveryManager recoveryManager;
        return recoveryManager.shouldRecover(error);
    }
    
    bool shouldIgnore(const NLMException& error) {
        return error.getCode() == ErrorCode::ConfigFileNotFound ||
               error.getCode() == ErrorCode::CliCommandNotFound;
    }
    
    RecoveryAction getRecommendedRecovery(const NLMException& error) {
        ErrorRecoveryManager recoveryManager;
        return recoveryManager.getRecoveryAction(error);
    }
    
    std::string generateErrorId() {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        return "err_" + std::to_string(timestamp);
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
}

} // namespace error
} // namespace nlm
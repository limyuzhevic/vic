// comprehensive_error_handling.cpp - Implementation of error handling system
#include "comprehensive_error_handling.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <filesystem>

namespace nlm {

// GlobalErrorHandler implementation
void GlobalErrorHandler::initialize() {
    errors_.clear();
    errorCallbacks_.clear();
    errorHandlingStrategy_ = ErrorHandlingStrategy::Error;
    maxErrors_ = 100;
    consoleOutput_ = true;
    stackTrace_ = true;
    
    lastErrorTime_ = std::chrono::system_clock::now();
    errorsInLastMinute_ = 0;
    errorRate_ = 0.0;
}

void GlobalErrorHandler::shutdown() {
    errors_.clear();
    errorCallbacks_.clear();
    if (consoleOutput_) {
        std::cout << "Global error handler shutdown complete." << std::endl;
    }
}

bool GlobalErrorHandler::handleError(const NLMException& error, const ErrorContext& context) {
    return processError(error, context);
}

void GlobalErrorHandler::setErrorHandlingStrategy(ErrorHandlingStrategy strategy) {
    errorHandlingStrategy_ = strategy;
    if (consoleOutput_) {
        std::cout << "Error handling strategy set to: " << static_cast<int>(strategy) << std::endl;
    }
}

ErrorHandlingStrategy GlobalErrorHandler::getErrorHandlingStrategy() const {
    return errorHandlingStrategy_;
}

void GlobalErrorHandler::addErrorCallback(std::function<void(const NLMException&)> callback) {
    errorCallbacks_.push_back(callback);
}

void GlobalErrorHandler::removeErrorCallback(std::function<void(const NLMException&)> callback) {
    errorCallbacks_.erase(std::remove(errorCallbacks_.begin(), errorCallbacks_.end(), callback), errorCallbacks_.end());
}

size_t GlobalErrorHandler::getErrorCount(ErrorSeverity severity) const {
    size_t count = 0;
    for (const auto& error : errors_) {
        if (error.getErrorSeverity() == severity) {
            count++;
        }
    }
    return count;
}

std::vector<NLMException> GlobalErrorHandler::getRecentErrors(size_t count) const {
    if (count >= errors_.size()) {
        return errors_;
    }
    size_t start = errors_.size() - count;
    return std::vector<NLMException>(errors_.begin() + start, errors_.end());
}

void GlobalErrorHandler::clearErrors() {
    errors_.clear();
    if (consoleOutput_) {
        std::cout << "All errors cleared." << std::endl;
    }
}

void GlobalErrorHandler::exportErrors(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "[\n";
            for (size_t i = 0; i < errors_.size(); ++i) {
                const auto& error = errors_[i];
                file << "  {\n";
                file << "    \"message\": \"" << error.what() << "\",\n";
                file << "    \"errorCode\": " << static_cast<int>(error.getErrorCode()) << ",\n";
                file << "    \"category\": \"" << error.getCategoryString() << "\",\n";
                file << "    \"severity\": \"" << error.getSeverityString() << "\",\n";
                file << "    \"component\": \"" << error.getContext().component << "\",\n";
                file << "    \"method\": \"" << error.getContext().method << "\",\n";
                file << "    \"file\": \"" << error.getContext().file << "\",\n";
                file << "    \"line\": " << error.getContext().line << "\n";
                if (i < errors_.size() - 1) {
                    file << "  },\n";
                } else {
                    file << "  }\n";
                }
            }
            file << "]\n";
            file.close();
            if (consoleOutput_) {
                std::cout << "Errors exported to " << filepath << " in JSON format." << std::endl;
            }
        }
    } else if (format == "csv") {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "severity,message,error_code,category,component,method,file,line,timestamp\n";
            for (const auto& error : errors_) {
                file << error.getSeverityString() << ",";
                file << "\"" << error.what() << "\",";
                file << static_cast<int>(error.getErrorCode()) << ",";
                file << error.getCategoryString() << ",";
                file << error.getContext().component << ",";
                file << error.getContext().method << ",";
                file << error.getContext().file << ",";
                file << error.getContext().line << ",";
                auto time = std::chrono::system_clock::to_time_t(error.getContext().timestamp);
                file << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
                file << "\n";
            }
            file.close();
            if (consoleOutput_) {
                std::cout << "Errors exported to " << filepath << " in CSV format." << std::endl;
            }
        }
    }
}

void GlobalErrorHandler::setMaxErrors(size_t maxErrors) {
    maxErrors_ = maxErrors;
    if (consoleOutput_) {
        std::cout << "Max errors set to: " << maxErrors << std::endl;
    }
}

void GlobalErrorHandler::setErrorLogFile(const std::string& filepath) {
    errorLogFile_ = filepath;
    if (consoleOutput_) {
        std::cout << "Error log file set to: " << filepath << std::endl;
    }
}

void GlobalErrorHandler::setConsoleOutput(bool enabled) {
    consoleOutput_ = enabled;
    if (consoleOutput_) {
        std::cout << "Console output enabled." << std::endl;
    } else {
        std::cout << "Console output disabled." << std::endl;
    }
}

void GlobalErrorHandler::setStackTraceEnabled(bool enabled) {
    stackTrace_ = enabled;
    if (consoleOutput_) {
        std::cout << "Stack trace " << (enabled ? "enabled" : "disabled") << "." << std::endl;
    }
}

std::string GlobalErrorHandler::generateErrorReport() const {
    std::stringstream ss;
    ss << "=== NLM Error Report ===\n";
    ss << "Generated: " << error_utils::formatError(std::vector<NLMException>(errors_.begin(), errors_.end()).front()) << "\n\n";
    
    ss << "=== Error Summary ===\n";
    ss << "Total Errors: " << errors_.size() << "\n";
    ss << "Errors in Last Minute: " << errorsInLastMinute_ << "\n";
    ss << "Error Rate: " << errorRate_ << " errors/minute\n";
    ss << "Max Errors: " << maxErrors_ << "\n\n";
    
    ss << "=== Error Severity Breakdown ===\n";
    std::map<ErrorSeverity, size_t> severityCounts;
    for (const auto& error : errors_) {
        severityCounts[error.getErrorSeverity()]++;
    }
    for (const auto& pair : severityCounts) {
        ss << error_utils::getErrorSeverityName(pair.first) << ": " << pair.second << "\n";
    }
    
    ss << "\n=== Error Category Breakdown ===\n";
    std::map<ErrorCategory, size_t> categoryCounts;
    for (const auto& error : errors_) {
        categoryCounts[error.getErrorCategory()]++;
    }
    for (const auto& pair : categoryCounts) {
        ss << error_utils::getErrorCategoryName(pair.first) << ": " << pair.second << "\n";
    }
    
    ss << "\n=== Recent Errors ===\n";
    size_t recentCount = std::min<size_t>(10, errors_.size());
    for (size_t i = 0; i < recentCount; ++i) {
        ss << error_utils::formatError(errors_[errors_.size() - 1 - i]) << "\n\n";
    }
    
    return ss.str();
}

std::string GlobalErrorHandler::generateErrorSummary() const {
    std::stringstream ss;
    ss << "NLM Error Summary\n";
    ss << "==================\n\n";
    
    if (errors_.empty()) {
        ss << "No errors recorded.\n";
        return ss.str();
    }
    
    ss << "Total Errors: " << errors_.size() << "\n";
    ss << "Most Recent Error: " << errors_.back().what() << "\n";
    ss << "Highest Severity: " << error_utils::getErrorSeverityName(
        std::max_element(errors_.begin(), errors_.end(),
            [](const NLMException& a, const NLMException& b) {
                return static_cast<int>(a.getErrorSeverity()) < static_cast<int>(b.getErrorSeverity());
            })->getErrorSeverity()) << "\n";
    
    return ss.str();
}

bool GlobalErrorHandler::processError(const NLMException& error, const ErrorContext& context) {
    // Update error tracking
    errors_.push_back(error);
    
    // Check for duplicate errors
    bool duplicate = false;
    for (const auto& existingError : errors_) {
        if (existingError.getErrorCode() == error.getErrorCode() &&
            existingError.getContext().component == context.component &&
            existingError.getContext().method == context.method) {
            duplicate = true;
            break;
        }
    }
    
    // Handle based on error handling strategy
    switch (errorHandlingStrategy_) {
        case ErrorHandlingStrategy::None:
            if (consoleOutput_) {
                std::cout << "[NONE] " << error.what() << std::endl;
            }
            break;
            
        case ErrorHandlingStrategy::Continue:
            if (consoleOutput_) {
                std::cout << "[CONTINUE] " << error.what() << std::endl;
            }
            break;
            
        case ErrorHandlingStrategy::Retry:
            if (consoleOutput_) {
                std::cout << "[RETRY] " << error.what() << std::endl;
            }
            break;
            
        case ErrorHandlingStrategy::Recover:
            if (consoleOutput_) {
                std::cout << "[RECOVER] " << error.what() << std::endl;
            }
            break;
            
        case ErrorHandlingStrategy::Warn:
            if (consoleOutput_) {
                std::cerr << "[WARN] " << error.what() << std::endl;
            }
            break;
            
        case ErrorHandlingStrategy::Error:
            if (consoleOutput_) {
                std::cerr << "[ERROR] " << error.what() << std::endl;
            }
            break;
            
        case ErrorHandlingStrategy::Abort:
            if (consoleOutput_) {
                std::cerr << "[ABORT] " << error.what() << std::endl;
            }
            return false; // Signal to abort
            
        case ErrorHandlingStrategy::GracefulShutdown:
            if (consoleOutput_) {
                std::cerr << "[SHUTDOWN] " << error.what() << std::endl;
            }
            return false; // Signal to shutdown
    }
    
    // Log to error log file
    if (!errorLogFile_.empty()) {
        std::ofstream logFile(errorLogFile_, std::ios::app);
        if (logFile.is_open()) {
            logFile << error_utils::formatError(error) << std::endl;
            logFile.close();
        }
    }
    
    // Call error callbacks
    callErrorCallbacks(error);
    
    // Update error statistics
    auto now = std::chrono::system_clock::now();
    auto timeSinceLastError = std::chrono::duration_cast<std::chrono::seconds>(now - lastErrorTime_);
    
    if (timeSinceLastError.count() < 60) {
        errorsInLastMinute_++;
    } else {
        errorsInLastMinute_ = 1;
    }
    
    lastErrorTime_ = now;
    errorRate_ = (errorsInLastMinute_ * 1000.0) / std::max(1.0, std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceLastError).count());
    
    // Check if we have too many errors
    if (errors_.size() > maxErrors_) {
        // Remove oldest errors
        size_t removeCount = errors_.size() - maxErrors_;
        errors_.erase(errors_.begin(), errors_.begin() + removeCount);
        if (consoleOutput_) {
            std::cout << "Warning: Error buffer exceeded. Oldest errors removed." << std::endl;
        }
    }
    
    return true;
}

void GlobalErrorHandler::callErrorCallbacks(const NLMException& error) {
    for (const auto& callback : errorCallbacks_) {
        try {
            callback(error);
        } catch (const std::exception& e) {
            if (consoleOutput_) {
                std::cerr << "Error in error callback: " << e.what() << std::endl;
            }
        }
    }
}

// ErrorManager implementation
ErrorManager::ErrorManager(const std::string& componentName) 
    : componentName_(componentName), errorHandler_(nullptr), 
      errorToleranceEnabled_(false), componentErrorCount_(0) {
    componentStartTime_ = std::chrono::system_clock::now();
}

ErrorManager::~ErrorManager() = default;

void ErrorManager::setComponentName(const std::string& name) {
    componentName_ = name;
}

std::string ErrorManager::getComponentName() const {
    return componentName_;
}

void ErrorManager::setErrorHandler(ErrorHandler* handler) {
    errorHandler_ = handler;
}

ErrorHandler* ErrorManager::getErrorHandler() const {
    return errorHandler_;
}

void ErrorManager::handleError(const NLMException& error) {
    if (errorHandler_) {
        errorHandler_->handleError(error, ErrorContext());
    }
    componentErrorCount_++;
    
    if (errorToleranceEnabled_ && error.getErrorSeverity() <= ErrorSeverity::Warning) {
        return; // Tolerate this error
    }
    
    if (error.getErrorSeverity() == ErrorSeverity::Fatal || 
        error.getErrorSeverity() == ErrorSeverity::Critical) {
        throw error; // Re-throw fatal errors
    }
}

void ErrorManager::handleError(ErrorCode code, const std::string& message,
                              ErrorCategory category, ErrorSeverity severity) {
    NLMException error(message, code, category, severity, ErrorContext());
    handleError(error);
}

void ErrorManager::logInfo(const std::string& message) {
    if (errorHandler_) {
        errorHandler_->handleError(
            NLMException(message, ErrorCode::UnknownError, ErrorCategory::System, ErrorSeverity::Info),
            ErrorContext()
        );
    }
}

void ErrorManager::logWarning(const std::string& message) {
    if (errorHandler_) {
        errorHandler_->handleError(
            NLMException(message, ErrorCode::UnknownError, ErrorCategory::System, ErrorSeverity::Warning),
            ErrorContext()
        );
    }
}

void ErrorManager::logError(const std::string& message, ErrorCode code, ErrorCategory category) {
    handleError(code, message, category, ErrorSeverity::Error);
}

void ErrorManager::logCritical(const std::string& message) {
    handleError(ErrorCode::UnknownError, message, ErrorCategory::System, ErrorSeverity::Critical);
}

void ErrorManager::logFatal(const std::string& message) {
    handleError(ErrorCode::UnknownError, message, ErrorCategory::System, ErrorSeverity::Fatal);
}

bool ErrorManager::isErrorToleranceEnabled() const {
    return errorToleranceEnabled_;
}

void ErrorManager::setErrorTolerance(bool enabled) {
    errorToleranceEnabled_ = enabled;
}

std::string ErrorManager::getErrorSummary() const {
    std::stringstream ss;
    ss << "Component: " << componentName_ << "\n";
    ss << "Errors: " << componentErrorCount_ << "\n";
    
    auto now = std::chrono::system_clock::now();
    auto lifetime = std::chrono::duration_cast<std::chrono::seconds>(now - componentStartTime_);
    ss << "Lifetime: " << lifetime.count() << " seconds\n";
    
    if (componentErrorCount_ > 0) {
        ss << "Error rate: " << static_cast<double>(componentErrorCount_) / std::max(1.0, lifetime.count()) << " errors/second\n";
    }
    
    return ss.str();
}

// Utility functions implementation
std::string error_utils::getErrorCategoryName(ErrorCategory category) {
    static const std::map<ErrorCategory, std::string> categoryNames = {
        {ErrorCategory::None, "None"},
        {ErrorCategory::Configuration, "Configuration"},
        {ErrorCategory::Initialization, "Initialization"},
        {ErrorCategory::Simulation, "Simulation"},
        {ErrorCategory::Memory, "Memory"},
        {ErrorCategory::Plasticity, "Plasticity"},
        {ErrorCategory::Neuromodulation, "Neuromodulation"},
        {ErrorCategory::Prediction, "Prediction"},
        {ErrorCategory::Cognition, "Cognition"},
        {ErrorCategory::Development, "Development"},
        {ErrorCategory::IO, "IO"},
        {ErrorCategory::System, "System"},
        {ErrorCategory::Network, "Network"},
        {ErrorCategory::Performance, "Performance"},
        {ErrorCategory::Security, "Security"}
    };
    
    auto it = categoryNames.find(category);
    return it != categoryNames.end() ? it->second : "Unknown";
}

std::string error_utils::getErrorSeverityName(ErrorSeverity severity) {
    static const std::map<ErrorSeverity, std::string> severityNames = {
        {ErrorSeverity::Debug, "DEBUG"},
        {ErrorSeverity::Info, "INFO"},
        {ErrorSeverity::Warning, "WARNING"},
        {ErrorSeverity::Error, "ERROR"},
        {ErrorSeverity::Critical, "CRITICAL"},
        {ErrorSeverity::Fatal, "FATAL"}
    };
    
    auto it = severityNames.find(severity);
    return it != severityNames.end() ? it->second : "UNKNOWN";
}

std::string error_utils::getErrorCodeName(ErrorCode code) {
    static const std::map<ErrorCode, std::string> codeNames = {
        {ErrorCode::ConfigFileNotFound, "ConfigFileNotFound"},
        {ErrorCode::ConfigParseError, "ConfigParseError"},
        {ErrorCode::ConfigValidationError, "ConfigValidationError"},
        {ErrorCode::InvalidParameter, "InvalidParameter"},
        {ErrorCode::BrainInitializationFailed, "BrainInitializationFailed"},
        {ErrorCode::MemorySystemNotAvailable, "MemorySystemNotAvailable"},
        {ErrorCode::PlasticitySystemNotAvailable, "PlasticitySystemNotAvailable"},
        {ErrorCode::NeuromodulationSystemNotAvailable, "NeuromodulationSystemNotAvailable"},
        {ErrorCode::PredictionSystemNotAvailable, "PredictionSystemNotAvailable"},
        {ErrorCode::CognitionSystemNotAvailable, "CognitionSystemNotAvailable"},
        {ErrorCode::DevelopmentSystemNotAvailable, "DevelopmentSystemNotAvailable"},
        {ErrorCode::SimulationStepFailed, "SimulationStepFailed"},
        {ErrorCode::InvalidNeuronState, "InvalidNeuronState"},
        {ErrorCode::SynapseError, "SynapseError"},
        {ErrorCode::SpikePropagationError, "SpikePropagationError"},
        {ErrorCode::NetworkStabilityError, "NetworkStabilityError"},
        {ErrorCode::MemoryAllocationFailed, "MemoryAllocationFailed"},
        {ErrorCode::MemoryCorruption, "MemoryCorruption"},
        {ErrorCode::MemoryLeakDetected, "MemoryLeakDetected"},
        {ErrorCode::MemoryCapacityExceeded, "MemoryCapacityExceeded"},
        {ErrorCode::MemoryConsistencyError, "MemoryConsistencyError"},
        {ErrorCode::STDPError, "STDPError"},
        {ErrorCode::HebbianError, "HebbianError"},
        {ErrorCode::StructuralPlasticityError, "StructuralPlasticityError"},
        {ErrorCode::PlasticityLearningRateError, "PlasticityLearningRateError"},
        {ErrorCode::DopamineSystemError, "DopamineSystemError"},
        {ErrorCode::CuriositySystemError, "CuriositySystemError"},
        {ErrorCode::NoveltySystemError, "NoveltySystemError"},
        {ErrorCode::PredictionErrorSystem, "PredictionErrorSystem"},
        {ErrorCode::NeuromodulatorInteractionError, "NeuromodulatorInteractionError"},
        {ErrorCode::PredictionSystemFailure, "PredictionSystemFailure"},
        {ErrorCode::PatternRecognitionError, "PatternRecognitionError"},
        {ErrorCode::ConfidenceCalculationError, "ConfidenceCalculationError"},
        {ErrorCode::PredictionAccuracyError, "PredictionAccuracyError"},
        {ErrorCode::PlanningError, "PlanningError"},
        {ErrorCode::ConceptFormationError, "ConceptFormationError"},
        {ErrorCode::AttentionError, "AttentionError"},
        {ErrorCode::DecisionMakingError, "DecisionMakingError"},
        {ErrorCode::CognitiveLoadExceeded, "CognitiveLoadExceeded"},
        {ErrorCode::DevelopmentalStageError, "DevelopmentalStageError"},
        {ErrorCode::MaturationError, "MaturationError"},
        {ErrorCode::PlasticityRateError, "PlasticityRateError"},
        {ErrorCode::StructuralDevelopmentError, "StructuralDevelopmentError"},
        {ErrorCode::FileReadError, "FileReadError"},
        {ErrorCode::FileWriteError, "FileWriteError"},
        {ErrorCode::CheckpointSaveError, "CheckpointSaveError"},
        {ErrorCode::CheckpointLoadError, "CheckpointLoadError"},
        {ErrorCode::SerializationError, "SerializationError"},
        {ErrorCode::DeserializationError, "DeserializationError"},
        {ErrorCode::OutOfMemory, "OutOfMemory"},
        {ErrorCode::DivisionByZero, "DivisionByZero"},
        {ErrorCode::InvalidPointer, "InvalidPointer"},
        {ErrorCode::ThreadCreationError, "ThreadCreationError"},
        {ErrorCode::ResourceExhaustion, "ResourceExhaustion"},
        {ErrorCode::NetworkPartition, "NetworkPartition"},
        {ErrorCode::ConnectivityError, "ConnectivityError"},
        {ErrorCode::LatencyError, "LatencyError"},
        {ErrorCode::BandwidthError, "BandwidthError"},
        {ErrorCode::PerformanceDegradation, "PerformanceDegradation"},
        {ErrorCode::ThroughputError, "ThroughputError"},
        {ErrorCode::LatencyError, "LatencyError"},
        {ErrorCode::MemoryEfficiencyError, "MemoryEfficiencyError"},
        {ErrorCode::UnauthorizedAccess, "UnauthorizedAccess"},
        {ErrorCode::DataCorruption, "DataCorruption"},
        {ErrorCode::IntegrityViolation, "IntegrityViolation"},
        {ErrorCode::UnknownError, "UnknownError"},
        {ErrorCode::NotImplemented, "NotImplemented"},
        {ErrorCode::FeatureNotAvailable, "FeatureNotAvailable"},
        {ErrorCode::CompatibilityError, "CompatibilityError"},
        {ErrorCode::ConfigurationError, "ConfigurationError"}
    };
    
    auto it = codeNames.find(code);
    return it != codeNames.end() ? it->second : "Unknown";
}

std::string error_utils::formatError(const NLMException& error) {
    std::stringstream ss;
    ss << "[" << error.getSeverityString() << "] " << error.what();
    ss << " (Code: " << static_cast<int>(error.getErrorCode()) << ", Category: " << error.getCategoryString() << ")";
    return ss.str();
}

std::string error_utils::formatErrorContext(const ErrorContext& context) {
    std::stringstream ss;
    ss << "Context: " << context.component << "::" << context.method;
    ss << " (" << context.file << ":" << context.line << ")";
    return ss.str();
}

bool error_utils::isRecoverable(ErrorCode code) {
    switch (code) {
        case ErrorCode::ConfigFileNotFound:
        case ErrorCode::ConfigParseError:
        case ErrorCode::InvalidParameter:
        case ErrorCode::OutOfMemory:
        case ErrorCode::DivisionByZero:
        case ErrorCode::InvalidPointer:
        case ErrorCode::ResourceExhaustion:
        case ErrorCode::NetworkPartition:
        case ErrorCode::ConnectivityError:
        case ErrorCode::PerformanceDegradation:
        case ErrorCode::MemoryEfficiencyError:
            return true;
            
        case ErrorCode::BrainInitializationFailed:
        case ErrorCode::MemorySystemNotAvailable:
        case ErrorCode::PlasticitySystemNotAvailable:
        case ErrorCode::NeuromodulationSystemNotAvailable:
        case ErrorCode::PredictionSystemNotAvailable:
        case ErrorCode::CognitionSystemNotAvailable:
        case ErrorCode::DevelopmentSystemNotAvailable:
        case ErrorCode::FileReadError:
        case ErrorCode::FileWriteError:
            return false;
            
        default:
            return true;
    }
}

ErrorHandlingStrategy error_utils::getDefaultErrorHandling(ErrorCode code) {
    switch (code) {
        case ErrorCode::ConfigFileNotFound:
        case ErrorCode::ConfigParseError:
            return ErrorHandlingStrategy::Recover;
            
        case ErrorCode::InvalidParameter:
            return ErrorHandlingStrategy::Error;
            
        case ErrorCode::BrainInitializationFailed:
        case ErrorCode::MemorySystemNotAvailable:
        case ErrorCode::PlasticitySystemNotAvailable:
        case ErrorCode::NeuromodulationSystemNotAvailable:
        case ErrorCode::PredictionSystemNotAvailable:
        case ErrorCode::CognitionSystemNotAvailable:
        case ErrorCode::DevelopmentSystemNotAvailable:
            return ErrorHandlingStrategy::Fatal;
            
        case ErrorCode::OutOfMemory:
        case ErrorCode::ResourceExhaustion:
            return ErrorHandlingStrategy::Recover;
            
        default:
            return ErrorHandlingStrategy::Error;
    }
}

std::string error_utils::generateErrorReport(const std::vector<NLMException>& errors) {
    std::stringstream ss;
    ss << "=== NLM Error Report ===\n";
    ss << "Total Errors: " << errors.size() << "\n\n";
    
    for (size_t i = 0; i < errors.size(); ++i) {
        ss << "Error " << i + 1 << ":\n";
        ss << error_utils::formatError(errors[i]) << "\n";
        ss << "\n";
    }
    
    return ss.str();
}

std::string error_utils::generateErrorStatistics(const std::vector<NLMException>& errors) {
    std::stringstream ss;
    ss << "=== Error Statistics ===\n";
    ss << "Total Errors: " << errors.size() << "\n";
    
    std::map<ErrorSeverity, size_t> severityCounts;
    std::map<ErrorCategory, size_t> categoryCounts;
    
    for (const auto& error : errors) {
        severityCounts[error.getErrorSeverity()]++;
        categoryCounts[error.getErrorCategory()]++;
    }
    
    ss << "\nSeverity Breakdown:\n";
    for (const auto& pair : severityCounts) {
        ss << "  " << error_utils::getErrorSeverityName(pair.first) << ": " << pair.second << "\n";
    }
    
    ss << "\nCategory Breakdown:\n";
    for (const auto& pair : categoryCounts) {
        ss << "  " << error_utils::getErrorCategoryName(pair.first) << ": " << pair.second << "\n";
    }
    
    return ss.str();
}

// Template implementations for safe_operation

template<typename T>
T error_utils::safe_operation(const std::string& operation, std::function<T()> func,
                            ErrorCode errorCode, ErrorHandlingStrategy strategy) {
    try {
        return func();
    } catch (const std::exception& e) {
        NLMException error(std::string("Operation failed: ") + operation + ": " + e.what(),
                           errorCode, ErrorCategory::System, ErrorSeverity::Error);
        
        switch (strategy) {
            case ErrorHandlingStrategy::None:
                break;
            case ErrorHandlingStrategy::Continue:
                break;
            case ErrorHandlingStrategy::Retry:
                break;
            case ErrorHandlingStrategy::Recover:
                break;
            case ErrorHandlingStrategy::Warn:
                std::cerr << "[WARN] " << error.what() << std::endl;
                break;
            case ErrorHandlingStrategy::Error:
                std::cerr << "[ERROR] " << error.what() << std::endl;
                break;
            case ErrorHandlingStrategy::Abort:
                std::cerr << "[ABORT] " << error.what() << std::endl;
                throw error;
                break;
            case ErrorHandlingStrategy::GracefulShutdown:
                std::cerr << "[SHUTDOWN] " << error.what() << std::endl;
                throw error;
                break;
        }
        
        throw error;
    }
}

// ConfigErrorHandler implementation
bool ConfigErrorHandler::handleError(const NLMException& error, const ErrorContext& context) {
    errors_.push_back(error);
    
    switch (strategy_) {
        case ErrorHandlingStrategy::None:
            break;
        case ErrorHandlingStrategy::Continue:
            break;
        case ErrorHandlingStrategy::Retry:
            break;
        case ErrorHandlingStrategy::Recover:
            break;
        case ErrorHandlingStrategy::Warn:
            std::cerr << "[WARN] " << error.what() << std::endl;
            break;
        case ErrorHandlingStrategy::Error:
            std::cerr << "[ERROR] " << error.what() << std::endl;
            break;
        case ErrorHandlingStrategy::Abort:
            std::cerr << "[ABORT] " << error.what() << std::endl;
            return false;
            break;
        case ErrorHandlingStrategy::GracefulShutdown:
            std::cerr << "[SHUTDOWN] " << error.what() << std::endl;
            return false;
            break;
    }
    
    return true;
}

void ConfigErrorHandler::setErrorHandlingStrategy(ErrorHandlingStrategy strategy) {
    strategy_ = strategy;
}

ErrorHandlingStrategy ConfigErrorHandler::getErrorHandlingStrategy() const {
    return strategy_;
}

void ConfigErrorHandler::addErrorCallback(std::function<void(const NLMException&)> callback) {
    callbacks_.push_back(callback);
}

void ConfigErrorHandler::removeErrorCallback(std::function<void(const NLMException&)> callback) {
    callbacks_.erase(std::remove(callbacks_.begin(), callbacks_.end(), callback), callbacks_.end());
}

size_t ConfigErrorHandler::getErrorCount(ErrorSeverity severity) const {
    size_t count = 0;
    for (const auto& error : errors_) {
        if (error.getErrorSeverity() == severity) {
            count++;
        }
    }
    return count;
}

std::vector<NLMException> ConfigErrorHandler::getRecentErrors(size_t count) const {
    if (count >= errors_.size()) {
        return errors_;
    }
    size_t start = errors_.size() - count;
    return std::vector<NLMException>(errors_.begin() + start, errors_.end());
}

void ConfigErrorHandler::clearErrors() {
    errors_.clear();
}

void ConfigErrorHandler::exportErrors(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "[\n";
            for (size_t i = 0; i < errors_.size(); ++i) {
                const auto& error = errors_[i];
                file << "  {\n";
                file << "    \"message\": \"" << error.what() << "\",\n";
                file << "    \"errorCode\": " << static_cast<int>(error.getErrorCode()) << ",\n";
                file << "    \"category\": \"" << error.getCategoryString() << "\",\n";
                file << "    \"severity\": \"" << error.getSeverityString() << "\",\n";
                file << "    \"component\": \"" << error.getContext().component << "\",\n";
                file << "    \"method\": \"" << error.getContext().method << "\",\n";
                file << "    \"file\": \"" << error.getContext().file << "\",\n";
                file << "    \"line\": " << error.getContext().line << "\n";
                if (i < errors_.size() - 1) {
                    file << "  },\n";
                } else {
                    file << "  }\n";
                }
            }
            file << "]\n";
            file.close();
        }
    }
}

// BrainErrorHandler implementation
bool BrainErrorHandler::handleError(const NLMException& error, const ErrorContext& context) {
    errors_.push_back(error);
    
    switch (strategy_) {
        case ErrorHandlingStrategy::None:
            break;
        case ErrorHandlingStrategy::Continue:
            break;
        case ErrorHandlingStrategy::Retry:
            break;
        case ErrorHandlingStrategy::Recover:
            break;
        case ErrorHandlingStrategy::Warn:
            std::cerr << "[WARN] " << error.what() << std::endl;
            break;
        case ErrorHandlingStrategy::Error:
            std::cerr << "[ERROR] " << error.what() << std::endl;
            break;
        case ErrorHandlingStrategy::Abort:
            std::cerr << "[ABORT] " << error.what() << std::endl;
            return false;
            break;
        case ErrorHandlingStrategy::GracefulShutdown:
            std::cerr << "[SHUTDOWN] " << error.what() << std::endl;
            return false;
            break;
    }
    
    return true;
}

void BrainErrorHandler::setErrorHandlingStrategy(ErrorHandlingStrategy strategy) {
    strategy_ = strategy;
}

ErrorHandlingStrategy BrainErrorHandler::getErrorHandlingStrategy() const {
    return strategy_;
}

void BrainErrorHandler::addErrorCallback(std::function<void(const NLMException&)> callback) {
    callbacks_.push_back(callback);
}

void BrainErrorHandler::removeErrorCallback(std::function<void(const NLMException&)> callback) {
    callbacks_.erase(std::remove(callbacks_.begin(), callbacks_.end(), callback), callbacks_.end());
}

size_t BrainErrorHandler::getErrorCount(ErrorSeverity severity) const {
    size_t count = 0;
    for (const auto& error : errors_) {
        if (error.getErrorSeverity() == severity) {
            count++;
        }
    }
    return count;
}

std::vector<NLMException> BrainErrorHandler::getRecentErrors(size_t count) const {
    if (count >= errors_.size()) {
        return errors_;
    }
    size_t start = errors_.size() - count;
    return std::vector<NLMException>(errors_.begin() + start, errors_.end());
}

void BrainErrorHandler::clearErrors() {
    errors_.clear();
}

void BrainErrorHandler::exportErrors(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "[\n";
            for (size_t i = 0; i < errors_.size(); ++i) {
                const auto& error = errors_[i];
                file << "  {\n";
                file << "    \"message\": \"" << error.what() << "\",\n";
                file << "    \"errorCode\": " << static_cast<int>(error.getErrorCode()) << ",\n";
                file << "    \"category\": \"" << error.getCategoryString() << "\",\n";
                file << "    \"severity\": \"" << error.getSeverityString() << "\",\n";
                file << "    \"component\": \"" << error.getContext().component << "\",\n";
                file << "    \"method\": \"" << error.getContext().method << "\",\n";
                file << "    \"file\": \"" << error.getContext().file << "\",\n";
                file << "    \"line\": " << error.getContext().line << "\n";
                if (i < errors_.size() - 1) {
                    file << "  },\n";
                } else {
                    file << "  }\n";
                }
            }
            file << "]\n";
            file.close();
        }
    }
}

} // namespace nlm

// Global error handler instance
nlm::GlobalErrorHandler g_errorHandler;
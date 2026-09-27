#include "Error.hpp"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <mutex>
#include <algorithm>

namespace nlm {

struct Error::Impl {
    std::mutex mutex;
    std::map<ErrorType, std::string> typeToString;
    std::map<ErrorSeverity, std::string> severityToString;
    std::map<ErrorCategory, std::string> categoryToString;

    Impl() {
        // Initialize mappings
        typeToString[ErrorType::SystemError] = "SystemError";
        typeToString[ErrorType::ConfigurationError] = "ConfigurationError";
        typeToString[ErrorType::MemoryError] = "MemoryError";
        typeToString[ErrorType::NeuralError] = "NeuralError";
        typeToString[ErrorType::FileError] = "FileError";
        typeToString[ErrorType::NetworkError] = "NetworkError";
        typeToString[ErrorType::ValidationError] = "ValidationError";
        typeToString[ErrorType::PlasticityError] = "PlasticityError";
        typeToString[ErrorType::DevelopmentError] = "DevelopmentError";
        typeToString[ErrorType::PredictionError] = "PredictionError";
        typeToString[ErrorType::NeuromodulationError] = "NeuromodulationError";
        typeToString[ErrorType::MemoryErrorCore] = "MemoryError";
        typeToString[ErrorType::AttentionError] = "AttentionError";
        typeToString[ErrorType::MotorError] = "MotorError";
        typeToString[ErrorType::CognitiveError] = "CognitiveError";
        typeToString[ErrorType::DevelopmentalError] = "DevelopmentalError";
        typeToString[ErrorType::Unknown] = "Unknown";

        severityToString[ErrorSeverity::Debug] = "Debug";
        severityToString[ErrorSeverity::Info] = "Info";
        severityToString[ErrorSeverity::Warning] = "Warning";
        severityToString[ErrorSeverity::Error] = "Error";
        severityToString[ErrorSeverity::Critical] = "Critical";

        categoryToString[ErrorCategory::Runtime] = "Runtime";
        categoryToString[ErrorCategory::CompileTime] = "CompileTime";
        categoryToString[ErrorCategory::Validation] = "Validation";
        categoryToString[ErrorCategory::System] = "System";
        categoryToString[ErrorCategory::User] = "User";
        categoryToString[ErrorCategory::Integration] = "Integration";
        categoryToString[ErrorCategory::Performance] = "Performance";
        categoryToString[ErrorCategory::Configuration] = "Configuration";
        categoryToString[ErrorCategory::Resource] = "Resource";
    }
};

std::string Error::format() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << pImpl->severityToString.at(severity_) << "] "
        << pImpl->categoryToString.at(static_cast<ErrorCategory>(type_ % 8)) << ": "
        << message_;
    
    if (!file_.empty()) {
        oss << " (" << file_;
        if (line_ > 0) oss << ":" << line_;
        oss << ")";
    }
    
    if (!function_.empty()) {
        oss << " in " << function_;
    }
    
    return oss.str();
}

std::string Error::formatStackTrace() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Stack Trace:" << std::endl;
    oss << "  Error: " << pImpl->severityToString.at(severity_) << " - "
        << pImpl->typeToString.at(type_) << std::endl;
    oss << "  Message: " << message_ << std::endl;
    oss << "  Location: " << file_ << ":" << line_ << " in " << function_ << std::endl;
    oss << "  Time: " << std::put_time(std::localtime(&std::chrono::system_clock::to_time_t(timestamp_)), "%Y-%m-%d %H:%M:%S");
    
    return oss.str();
}

struct ConsoleErrorReporter::Impl {
    ErrorSeverity level = ErrorSeverity::Error;
    std::mutex mutex;
    std::vector<std::shared_ptr<Error>> errors;
    std::vector<std::function<void(const std::shared_ptr<Error>)>> errorHandlers;

    void addError(const std::shared_ptr<Error>& error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (error->getSeverity() >= level) {
            errors.push_back(error);
        }
        for (auto& handler : errorHandlers) {
            handler(error);
        }
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        errors.clear();
    }

    size_t countErrors(ErrorSeverity severity) const {
        std::lock_guard<std::mutex> lock(mutex);
        return std::count_if(errors.begin(), errors.end(),
            [severity](const std::shared_ptr<Error>& error) {
                return error->getSeverity() == severity;
            });
    }

    std::vector<std::shared_ptr<Error>> getErrors(ErrorSeverity severity) const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::shared_ptr<Error>> filteredErrors;
        std::copy_if(errors.begin(), errors.end(), std::back_inserter(filteredErrors),
            [severity](const std::shared_ptr<Error>& error) {
                return error->getSeverity() == severity;
            });
        return filteredErrors;
    }
};

ConsoleErrorReporter::ConsoleErrorReporter() : pImpl(std::make_unique<Impl>()) {
    pImpl->typeToString = {
        {ErrorType::SystemError, "SystemError"},
        {ErrorType::ConfigurationError, "ConfigurationError"},
        {ErrorType::MemoryError, "MemoryError"},
        {ErrorType::NeuralError, "NeuralError"},
        {ErrorType::FileError, "FileError"},
        {ErrorType::NetworkError, "NetworkError"},
        {ErrorType::ValidationError, "ValidationError"},
        {ErrorType::PlasticityError, "PlasticityError"},
        {ErrorType::DevelopmentError, "DevelopmentError"},
        {ErrorType::PredictionError, "PredictionError"},
        {ErrorType::NeuromodulationError, "NeuromodulationError"},
        {ErrorType::MemoryErrorCore, "MemoryError"},
        {ErrorType::AttentionError, "AttentionError"},
        {ErrorType::MotorError, "MotorError"},
        {ErrorType::CognitiveError, "CognitiveError"},
        {ErrorType::DevelopmentalError, "DevelopmentalError"},
        {ErrorType::Unknown, "Unknown"}
    };

    pImpl->severityToString = {
        {ErrorSeverity::Debug, "DEBUG"},
        {ErrorSeverity::Info, "INFO"},
        {ErrorSeverity::Warning, "WARN"},
        {ErrorSeverity::Error, "ERROR"},
        {ErrorSeverity::Critical, "CRIT"}
    };

    pImpl->categoryToString = {
        {ErrorCategory::Runtime, "Runtime"},
        {ErrorCategory::CompileTime, "CompileTime"},
        {ErrorCategory::Validation, "Validation"},
        {ErrorCategory::System, "System"},
        {ErrorCategory::User, "User"},
        {ErrorCategory::Integration, "Integration"},
        {ErrorCategory::Performance, "Performance"},
        {ErrorCategory::Configuration, "Configuration"},
        {ErrorCategory::Resource, "Resource"}
    };
}

ConsoleErrorReporter::ConsoleErrorReporter(ErrorSeverity level) : ConsoleErrorReporter() {
    pImpl->level = level;
}

ConsoleErrorReporter::~ConsoleErrorReporter() = default;

void ConsoleErrorReporter::report(const std::shared_ptr<Error> error) {
    pImpl->addError(error);
    
    std::cout << error->format() << std::endl;
}

void ConsoleErrorReporter::setErrorLevel(ErrorSeverity level) {
    pImpl->level = level;
}

ErrorSeverity ConsoleErrorReporter::getErrorLevel() const {
    return pImpl->level;
}

void ConsoleErrorReporter::clear() {
    pImpl->clear();
}

size_t ConsoleErrorReporter::getErrorCount(ErrorSeverity severity) const {
    return pImpl->countErrors(severity);
}

std::vector<std::shared_ptr<Error>> ConsoleErrorReporter::getErrors(ErrorSeverity severity) const {
    return pImpl->getErrors(severity);
}

void ConsoleErrorReporter::addErrorHandler(std::function<void(const std::shared_ptr<Error>)> handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorHandlers.push_back(handler);
}

void ConsoleErrorReporter::removeErrorHandler(std::function<void(const std::shared_ptr<Error>)> handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorHandlers.erase(
        std::remove(pImpl->errorHandlers.begin(), pImpl->errorHandlers.end(), handler),
        pImpl->errorHandlers.end()
    );
}

std::string ConsoleErrorReporter::severityToString(ErrorSeverity severity) const {
    static const std::map<ErrorSeverity, std::string> mapping = {
        {ErrorSeverity::Debug, "DEBUG"},
        {ErrorSeverity::Info, "INFO"},
        {ErrorSeverity::Warning, "WARN"},
        {ErrorSeverity::Error, "ERROR"},
        {ErrorSeverity::Critical, "CRIT"}
    };
    auto it = mapping.find(severity);
    return (it != mapping.end()) ? it->second : "UNKNOWN";
}

std::string ConsoleErrorReporter::errorTypeToString(ErrorType type) const {
    static const std::map<ErrorType, std::string> mapping = {
        {ErrorType::SystemError, "SystemError"},
        {ErrorType::ConfigurationError, "ConfigurationError"},
        {ErrorType::MemoryError, "MemoryError"},
        {ErrorType::NeuralError, "NeuralError"},
        {ErrorType::FileError, "FileError"},
        {ErrorType::NetworkError, "NetworkError"},
        {ErrorType::ValidationError, "ValidationError"},
        {ErrorType::PlasticityError, "PlasticityError"},
        {ErrorType::DevelopmentError, "DevelopmentError"},
        {ErrorType::PredictionError, "PredictionError"},
        {ErrorType::NeuromodulationError, "NeuromodulationError"},
        {ErrorType::MemoryErrorCore, "MemoryError"},
        {ErrorType::AttentionError, "AttentionError"},
        {ErrorType::MotorError, "MotorError"},
        {ErrorType::CognitiveError, "CognitiveError"},
        {ErrorType::DevelopmentalError, "DevelopmentalError"},
        {ErrorType::Unknown, "Unknown"}
    };
    auto it = mapping.find(type);
    return (it != mapping.end()) ? it->second : "Unknown";
}

struct FileErrorReporter::Impl {
    std::string filepath;
    std::ofstream fileOut;
    ErrorSeverity level;
    std::mutex mutex;
    std::vector<std::shared_ptr<Error>> errors;
    std::vector<std::function<void(const std::shared_ptr<Error>)>> errorHandlers;
    size_t maxFileSize;
    size_t maxFileCount;
    size_t currentFileCount;

    Impl(const std::string& filepath, bool append)
        : filepath(filepath)
        , level(ErrorSeverity::Error)
        , maxFileSize(10 * 1024 * 1024)  // 10 MB
        , maxFileCount(10)
        , currentFileCount(0) {
        fileOut.open(filepath, std::ios::app);
        if (!fileOut.is_open()) {
            throw FileError("Failed to open error log file: " + filepath);
        }
    }

    void ensureFileSize() {
        if (fileOut.tellp() > maxFileSize) {
            fileOut.close();
            currentFileCount++;
            if (currentFileCount > maxFileCount) {
                // Remove oldest files (simplified implementation)
                // In practice, would use proper file rotation
                currentFileCount = maxFileCount;
            }
            fileOut.open(filepath, std::ios::trunc);
        }
    }

    void addError(const std::shared_ptr<Error>& error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (error->getSeverity() >= level) {
            errors.push_back(error);
            ensureFileSize();
            fileOut << error->format() << std::endl;
            fileOut.flush();
        }
        for (auto& handler : errorHandlers) {
            handler(error);
        }
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        errors.clear();
        fileOut.close();
        fileOut.open(filepath, std::ios::trunc);
    }

    size_t countErrors(ErrorSeverity severity) const {
        std::lock_guard<std::mutex> lock(mutex);
        return std::count_if(errors.begin(), errors.end(),
            [severity](const std::shared_ptr<Error>& error) {
                return error->getSeverity() == severity;
            });
    }

    std::vector<std::shared_ptr<Error>> getErrors(ErrorSeverity severity) const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::shared_ptr<Error>> filteredErrors;
        std::copy_if(errors.begin(), errors.end(), std::back_inserter(filteredErrors),
            [severity](const std::shared_ptr<Error>& error) {
                return error->getSeverity() == severity;
            });
        return filteredErrors;
    }
};

FileErrorReporter::FileErrorReporter(const std::string& filepath, bool append) : pImpl(std::make_unique<Impl>(filepath, append)) {}

FileErrorReporter::~FileErrorReporter() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void FileErrorReporter::report(const std::shared_ptr<Error> error) {
    pImpl->addError(error);
}

void FileErrorReporter::setErrorLevel(ErrorSeverity level) {
    pImpl->level = level;
}

ErrorSeverity FileErrorReporter::getErrorLevel() const {
    return pImpl->level;
}

void FileErrorReporter::clear() {
    pImpl->clear();
}

size_t FileErrorReporter::getErrorCount(ErrorSeverity severity) const {
    return pImpl->countErrors(severity);
}

std::vector<std::shared_ptr<Error>> FileErrorReporter::getErrors(ErrorSeverity severity) const {
    return pImpl->getErrors(severity);
}

void FileErrorReporter::addErrorHandler(std::function<void(const std::shared_ptr<Error>)> handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorHandlers.push_back(handler);
}

void FileErrorReporter::removeErrorHandler(std::function<void(const std::shared_ptr<Error>)> handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorHandlers.erase(
        std::remove(pImpl->errorHandlers.begin(), pImpl->errorHandlers.end(), handler),
        pImpl->errorHandlers.end()
    );
}

void FileErrorReporter::setMaxFileSize(size_t maxBytes) {
    pImpl->maxFileSize = maxBytes;
}

void FileErrorReporter::setMaxFileCount(size_t maxFiles) {
    pImpl->maxFileCount = maxFiles;
}

void FileErrorReporter::rotate() {
    pImpl->clear();
}

struct ErrorReporter::Impl {
    std::vector<std::shared_ptr<IErrorReporter>> reporters;
    std::mutex mutex;
    ErrorSeverity globalLevel = ErrorSeverity::Error;

    void addReporter(std::shared_ptr<IErrorReporter> reporter) {
        std::lock_guard<std::mutex> lock(mutex);
        reporters.push_back(reporter);
    }

    void clearReporters() {
        std::lock_guard<std::mutex> lock(mutex);
        reporters.clear();
    }

    void setGlobalLevel(ErrorSeverity level) {
        std::lock_guard<std::mutex> lock(mutex);
        globalLevel = level;
        for (auto& reporter : reporters) {
            reporter->setErrorLevel(level);
        }
    }

    void reportToAll(const std::shared_ptr<Error>& error) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& reporter : reporters) {
            if (error->getSeverity() >= reporter->getErrorLevel()) {
                reporter->report(error);
            }
        }
    }

    void clearAll() {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& reporter : reporters) {
            reporter->clear();
        }
    }

    size_t getErrorCount(ErrorSeverity severity) const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t count = 0;
        for (auto& reporter : reporters) {
            count += reporter->getErrorCount(severity);
        }
        return count;
    }

    std::vector<std::shared_ptr<Error>> getAllErrors(ErrorSeverity severity) const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::shared_ptr<Error>> allErrors;
        for (auto& reporter : reporters) {
            auto errors = reporter->getErrors(severity);
            allErrors.insert(allErrors.end(), errors.begin(), errors.end());
        }
        return allErrors;
    }
};

ErrorReporter::ErrorReporter() : pImpl(std::make_unique<Impl>()) {
    auto consoleReporter = std::make_shared<ConsoleErrorReporter>(ErrorSeverity::Error);
    pImpl->addReporter(consoleReporter);
}

ErrorReporter::~ErrorReporter() = default;

void ErrorReporter::addReporter(std::shared_ptr<IErrorReporter> reporter) {
    pImpl->addReporter(reporter);
}

void ErrorReporter::clearReporters() {
    pImpl->clearReporters();
}

void ErrorReporter::setErrorLevel(ErrorSeverity level) {
    pImpl->setGlobalLevel(level);
}

void ErrorReporter::report(std::shared_ptr<Error> error) {
    pImpl->reportToAll(error);
}

void ErrorReporter::report(const std::string& message, ErrorType type,
                           ErrorSeverity severity,
                           const std::string& file, int line,
                           const std::string& function,
                           const std::string& category) {
    auto error = std::make_shared<Error>(message, type, severity, file, line, function, category);
    pImpl->reportToAll(error);
}

void ErrorReporter::logDebug(const std::string& message, const std::string& file, int line, const std::string& function) {
    report(message, ErrorType::Unknown, ErrorSeverity::Debug, file, line, function);
}

void ErrorReporter::logInfo(const std::string& message, const std::string& file, int line, const std::string& function) {
    report(message, ErrorType::Unknown, ErrorSeverity::Info, file, line, function);
}

void ErrorReporter::logWarning(const std::string& message, const std::string& file, int line, const std::string& function) {
    report(message, ErrorType::Unknown, ErrorSeverity::Warning, file, line, function);
}

void ErrorReporter::logError(const std::string& message, const std::string& file, int line, const std::string& function) {
    report(message, ErrorType::Unknown, ErrorSeverity::Error, file, line, function);
}

void ErrorReporter::logCritical(const std::string& message, const std::string& file, int line, const std::string& function) {
    report(message, ErrorType::Unknown, ErrorSeverity::Critical, file, line, function);
}

void ErrorReporter::clearAllErrors() {
    pImpl->clearAll();
}

size_t ErrorReporter::getErrorCount(ErrorSeverity severity) const {
    return pImpl->getErrorCount(severity);
}

std::vector<std::shared_ptr<Error>> ErrorReporter::getErrors(ErrorSeverity severity) const {
    return pImpl->getAllErrors(severity);
}

ErrorReporter& ErrorReporter::getGlobal() {
    static ErrorReporter globalReporter;
    return globalReporter;
}

void ErrorReporter::setGlobal(std::shared_ptr<ErrorReporter> reporter) {
    // This is a placeholder for advanced global reporter management
    // In practice, we just use getGlobal() which is already static
}

struct ValidationEngine::Impl {
    std::vector<ValidationRule> rules;
    std::map<std::string, std::map<std::string, std::string>> config;
    ErrorSeverity currentLevel = ErrorSeverity::Warning;

    bool validateAgainstRules(const std::vector<std::string>& data, const std::string& context) const {
        for (const auto& rule : rules) {
            try {
                if (rule.validator(data)) {
                    continue;
                } else {
                    return false;
                }
            } catch (const std::exception& e) {
                // Rule validation failed
                return false;
            }
        }
        return true;
    }
};

ValidationEngine::ValidationEngine() : pImpl(std::make_unique<Impl>()) {}

ValidationEngine::~ValidationEngine() = default;

void ValidationEngine::addRule(const ValidationRule& rule) {
    pImpl->rules.push_back(rule);
}

void ValidationEngine::addRule(const std::string& name, const std::string& description,
                               std::function<bool(const std::vector<std::string>&)> validator,
                               ErrorType errorType,
                               ErrorSeverity severity,
                               bool required) {
    ValidationRule rule(name, description, validator, errorType, severity, required);
    pImpl->rules.push_back(rule);
}

ValidationResult ValidationEngine::validate(const std::vector<std::string>& data, const std::string& context) const {
    ValidationResult result;
    result.valid = pImpl->validateAgainstRules(data, context);
    return result;
}

ValidationResult ValidationEngine::validateConfiguration(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    // Basic configuration validation rules
    if (key == "random_seed") {
        try {
            size_t pos;
            long seed = std::stol(value, &pos);
            if (pos != value.size() || seed < 0) {
                result.valid = false;
                result.errors.push_back("random_seed must be non-negative integer");
            }
        } catch (const std::exception&) {
            result.valid = false;
            result.errors.push_back("random_seed must be an integer");
        }
    } else if (key == "simulation_timestep") {
        try {
            size_t pos;
            double timestep = std::stod(value, &pos);
            if (pos != value.size() || timestep <= 0.0) {
                result.valid = false;
                result.errors.push_back("simulation_timestep must be positive");
            }
        } catch (const std::exception&) {
            result.valid = false;
            result.errors.push_back("simulation_timestep must be a number");
        }
    } else if (key == "neuron_count") {
        try {
            size_t pos;
            long count = std::stol(value, &pos);
            if (pos != value.size() || count <= 0) {
                result.valid = false;
                result.errors.push_back("neuron_count must be positive integer");
            }
        } catch (const std::exception&) {
            result.valid = false;
            result.errors.push_back("neuron_count must be an integer");
        }
    }
    
    return result;
}

ValidationResult ValidationEngine::validateSystemState() const {
    ValidationResult result;
    // Basic system state validation
    // In practice, would check memory, file handles, network connections, etc.
    return result;
}

size_t ValidationEngine::getErrorCount(ErrorSeverity severity) const {
    return 0; // Simplified implementation
}

size_t ValidationEngine::getWarningCount(ErrorSeverity severity) const {
    return 0; // Simplified implementation
}

bool ValidationEngine::attemptRecovery(const std::shared_ptr<Error> error) {
    // Error recovery implementation
    return false;
}

void ValidationEngine::configureValidation(const std::string& section, const std::map<std::string, std::string>& settings) {
    pImpl->config[section] = settings;
}

std::string ErrorHandling::getCurrentLocation(const std::string& file, int line, const std::string& function) {
    std::ostringstream oss;
    if (!file.empty()) {
        oss << file;
        if (line > 0) oss << ":" << line;
    }
    if (!function.empty()) {
        if (!file.empty()) oss << " in ";
        oss << function;
    }
    return oss.str();
}

std::string ErrorHandling::getBacktrace(size_t maxDepth) {
    std::ostringstream oss;
    oss << "Backtrace (placeholder - maxDepth=" << maxDepth << ")";
    return oss.str();
}

void* ErrorHandling::safeMalloc(size_t size) {
    try {
        return std::malloc(size);
    } catch (const std::bad_alloc&) {
        ErrorReporter::getGlobal().report("Memory allocation failed", ErrorType::MemoryError, ErrorSeverity::Critical);
        return nullptr;
    }
}

void ErrorHandling::safeFree(void* ptr) {
    if (ptr) {
        std::free(ptr);
    }
}

bool ErrorHandling::validateResourceAvailability(const std::string& resource, float minAvailable) {
    // Resource availability validation
    // In practice, would check system resources
    return true;
}

bool ErrorHandling::validateMemoryLimit(size_t maxBytes) {
    // Memory limit validation
    // In practice, would check available memory
    return true;
}

bool ErrorHandling::validateFileAccess(const std::string& path, bool writeAccess) {
    // File access validation
    // In practice, would check file permissions
    return true;
}

} // namespace nlm

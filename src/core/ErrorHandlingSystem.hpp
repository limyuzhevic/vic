#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <format>
#include <chrono>
#include <unordered_map>

namespace nlm {

// Error categories for NLM
enum class ErrorCategory {
    Configuration,
    NeuralDynamics,
    Plasticity,
    Memory,
    SensoryMotor,
    Development,
    System,
    Unknown
};

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// NLM exception with enhanced error information
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, 
                  ErrorCategory category = ErrorCategory::Unknown,
                  ErrorSeverity severity = ErrorSeverity::Error,
                  const std::string& operation = "")
        : std::runtime_error(message),
          category(category),
          severity(severity),
          operation(operation),
          timestamp(std::chrono::system_clock::now()) {}
    
    ErrorCategory getCategory() const { return category; }
    ErrorSeverity getSeverity() const { return severity; }
    const std::string& getOperation() const { return operation; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    
    // Format error for display
    std::string formatError() const {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] ";
        
        // Add severity prefix
        switch (severity) {
            case ErrorSeverity::Debug: ss << "DEBUG: "; break;
            case ErrorSeverity::Info: ss << "INFO: "; break;
            case ErrorSeverity::Warning: ss << "WARNING: "; break;
            case ErrorSeverity::Error: ss << "ERROR: "; break;
            case ErrorSeverity::Critical: ss << "CRITICAL: "; break;
        }
        
        // Add category
        ss << "[" << static_cast<int>(category) << "] ";
        
        // Add operation
        if (!operation.empty()) ss << "Operation: " << operation << " ";
        
        // Add message
        ss << std::runtime_error::what();
        
        return ss.str();
    }
    
    // Convert to string for logging
    std::string toString() const { return formatError(); }
    
private:
    ErrorCategory category;
    ErrorSeverity severity;
    std::string operation;
    std::chrono::system_clock::time_point timestamp;
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const NLMException& error) = 0;
    virtual void handleWarning(const std::string& message) = 0;
    virtual void handleInfo(const std::string& message) = 0;
};

// Console error handler
class ConsoleErrorHandler : public IErrorHandler {
public:
    void handleError(const NLMException& error) override {
        std::cerr << error.toString() << std::endl;
    }
    
    void handleWarning(const std::string& message) override {
        std::cerr << "[WARNING] " << message << std::endl;
    }
    
    void handleInfo(const std::string& message) override {
        std::cout << "[INFO] " << message << std::endl;
    }
};

// File error handler
class FileErrorHandler : public IErrorHandler {
public:
    FileErrorHandler(const std::string& filename) : filename(filename) {}
    
    void handleError(const NLMException& error) override {
        std::ofstream file(filename, std::ios::app);
        if (file.is_open()) {
            file << error.toString() << std::endl;
        }
    }
    
    void handleWarning(const std::string& message) override {
        std::ofstream file(filename, std::ios::app);
        if (file.is_open()) {
            file << "[WARNING] " << message << std::endl;
        }
    }
    
    void handleInfo(const std::string& message) override {
        std::ofstream file(filename, std::ios::app);
        if (file.is_open()) {
            file << "[INFO] " << message << std::endl;
        }
    }
    
private:
    std::string filename;
};

// Error manager for centralized error handling
class ErrorManager {
public:
    static ErrorManager& getInstance() {
        static ErrorManager instance;
        return instance;
    }
    
    void addErrorHandler(std::shared_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        handlers.push_back(handler);
    }
    
    void removeErrorHandler(std::shared_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        handlers.erase(
            std::remove(handlers.begin(), handlers.end(), handler),
            handlers.end()
        );
    }
    
    void logError(const NLMException& error) {
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& handler : handlers) {
            handler->handleError(error);
        }
    }
    
    void logWarning(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& handler : handlers) {
            handler->handleWarning(message);
        }
    }
    
    void logInfo(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& handler : handlers) {
            handler->handleInfo(message);
        }
    }
    
    // Initialize with default handlers
    void initialize() {
        addErrorHandler(std::make_shared<ConsoleErrorHandler>());
    }
    
private:
    ErrorManager() = default;
    std::mutex mutex;
    std::vector<std::shared_ptr<IErrorHandler>> handlers;
};

// Configuration validator
class ConfigurationValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    ValidationResult validateConfig(const std::shared_ptr<Config>& config) {
        ValidationResult result;
        result.isValid = true;
        
        // Validate neural parameters
        if (auto neuronCount = config->getOr<int64_t>("neuron_count", -1); neuronCount < 0) {
            result.errors.push_back("neuron_count must be non-negative");
            result.isValid = false;
        } else if (neuronCount > 1000000) {
            result.warnings.push_back("neuron_count is very large (>1,000,000). Performance may be affected.");
        }
        
        if (auto connectionProb = config->getOr<float>("connection_probability", -1.0f); connectionProb < 0.0f || connectionProb > 1.0f) {
            result.errors.push_back("connection_probability must be in range [0.0, 1.0]");
            result.isValid = false;
        }
        
        if (auto timestep = config->getOr<double>("simulation_timestep", -1.0); timestep <= 0.0) {
            result.errors.push_back("simulation_timestep must be positive");
            result.isValid = false;
        } else if (timestep > 0.1) {
            result.warnings.push_back("simulation_timestep is very large (>0.1s). Accuracy may be affected.");
        }
        
        // Validate plasticity parameters
        if (auto stdpLTP = config->getOr<float>("stdp_ltp_weight", 0.0f); stdpLTP < 0.0f) {
            result.errors.push_back("stdp_ltp_weight must be non-negative");
            result.isValid = false;
        }
        
        if (auto stdpLTD = config->getOr<float>("stdp_ltd_weight", 0.0f); stdpLTD < 0.0f) {
            result.errors.push_back("stdp_ltd_weight must be non-negative");
            result.isValid = false;
        }
        
        if (auto stdpTau = config->getOr<float>("stdp_tau", 0.0f); stdpTau <= 0.0f) {
            result.errors.push_back("stdp_tau must be positive");
            result.isValid = false;
        }
        
        // Validate memory parameters
        if (auto workingMemCap = config->getOr<size_t>("working_memory.capacity", 0); workingMemCap > 100000) {
            result.warnings.push_back("working_memory.capacity is very large (>100,000). Memory usage may be affected.");
        }
        
        // Validate development parameters
        if (auto enableDev = config->getOr<bool>("development.enable", false); enableDev) {
            auto devStages = config->getOr<int64_t>("development.stages", 0);
            if (devStages < 1 || devStages > 10) {
                result.warnings.push_back("development.stages should be in range [1, 10].");
            }
        }
        
        // Log validation results
        if (!result.errors.empty()) {
            ErrorManager::getInstance().logError(
                NLMException("Configuration validation failed", ErrorCategory::Configuration, ErrorSeverity::Error, "validateConfig")
            );
            for (const auto& error : result.errors) {
                ErrorManager::getInstance().logError(
                    NLMException(error, ErrorCategory::Configuration, ErrorSeverity::Error, "validateConfig")
                );
            }
        }
        
        if (!result.warnings.empty()) {
            for (const auto& warning : result.warnings) {
                ErrorManager::getInstance().logWarning(warning);
            }
        }
        
        if (result.isValid) {
            ErrorManager::getInstance().logInfo("Configuration validation passed");
        }
        
        return result;
    }
};

// Neural operation validator
class NeuralOperationValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
    };
    
    ValidationResult validateNeuron(const NeuronState& state) {
        ValidationResult result;
        result.isValid = true;
        
        // Validate membrane potential
        if (state.membranePotential < -100.0f || state.membranePotential > 50.0f) {
            result.errors.push_back("membrane potential out of range [-100, 50] mV");
            result.isValid = false;
        }
        
        // Validate threshold
        if (state.threshold < -100.0f || state.threshold > 50.0f) {
            result.errors.push_back("threshold out of range [-100, 50] mV");
            result.isValid = false;
        }
        
        // Validate refractory period
        if (state.refractoryRemaining > state.refractoryPeriod) {
            result.errors.push_back("refractory remaining exceeds refractory period");
            result.isValid = false;
        }
        
        // Validate firing state consistency
        if (state.firingState == FiringState::Active && state.lastSpikeTime < 0.0f) {
            result.errors.push_back("firing state Active but no last spike time");
            result.isValid = false;
        }
        
        return result;
    }
    
    ValidationResult validateSynapse(const Synapse& synapse) {
        ValidationResult result;
        result.isValid = true;
        
        // Validate synaptic weight
        if (synapse.getWeight() < -10.0f || synapse.getWeight() > 10.0f) {
            result.errors.push_back("synaptic weight out of range [-10, 10]");
            result.isValid = false;
        }
        
        return result;
    }
    
    ValidationResult validateStep(const Brain::Impl* impl) {
        ValidationResult result;
        result.isValid = true;
        
        // Validate consistency between regions and neurons
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        
        for (const auto& region : impl->regions) {
            totalNeurons += region->getTotalNeuronCount();
            totalSynapses += region->getSynapseCount();
        }
        
        // Check for consistency
        if (totalNeurons != impl->sensoryNeurons.size() + impl->motorNeurons.size()) {
            result.errors.push_back("neuron count mismatch between regions and I/O neurons");
            result.isValid = false;
        }
        
        // Validate spike counts
        if (impl->totalSpikesThisStep > impl->regions.size() * 100) {
            result.warnings.push_back("very high spike count in this step. Check neuron parameters.");
        }
        
        return result;
    }
};

// Validation manager for comprehensive validation
class ValidationManager {
public:
    ValidationManager() {
        validator = std::make_shared<NeuralOperationValidator>();
        configValidator = std::make_shared<ConfigurationValidator>();
    }
    
    void setConfigValidator(std::shared_ptr<ConfigurationValidator> validator) {
        configValidator = validator;
    }
    
    void setNeuralValidator(std::shared_ptr<NeuralOperationValidator> validator) {
        validator = neuralValidator;
    }
    
    ConfigurationValidator::ValidationResult validateConfiguration(const std::shared_ptr<Config>& config) {
        return configValidator->validateConfig(config);
    }
    
    NeuralOperationValidator::ValidationResult validateNeuron(const NeuronState& state) {
        return validator->validateNeuron(state);
    }
    
    NeuralOperationValidator::ValidationResult validateSynapse(const Synapse& synapse) {
        return validator->validateSynapse(synapse);
    }
    
    NeuralOperationValidator::ValidationResult validateBrainStep(const Brain::Impl* impl) {
        return validator->validateStep(impl);
    }
    
    bool validateFull(Brain* brain) {
        bool isValid = true;
        
        // Validate configuration
        auto configResult = validateConfiguration(brain->getConfig());
        if (!configResult.isValid) {
            isValid = false;
        }
        
        // Validate brain step
        auto stepResult = validateBrainStep(brain->pImpl.get());
        if (!stepResult.isValid) {
            isValid = false;
        }
        
        return isValid;
    }
    
private:
    std::shared_ptr<NeuralOperationValidator> validator;
    std::shared_ptr<ConfigurationValidator> configValidator;
};

// Enhanced logging system
class LoggingManager {
public:
    static LoggingManager& getInstance() {
        static LoggingManager instance;
        return instance;
    }
    
    void setLogLevel(ErrorSeverity level) {
        logLevel = level;
    }
    
    void setLogFile(const std::string& filename) {
        logFile = filename;
        logToFile = true;
    }
    
    void disableFileLogging() {
        logToFile = false;
    }
    
    void log(ErrorSeverity severity, const std::string& category, const std::string& message,
             const std::string& operation = "") {
        if (severity < logLevel) return; // Skip below log level
        
        std::string logEntry = formatLogEntry(severity, category, message, operation);
        
        // Output to console
        std::cout << logEntry << std::endl;
        
        // Output to file if enabled
        if (logToFile) {
            std::ofstream file(logFile, std::ios::app);
            if (file.is_open()) {
                file << logEntry << std::endl;
            }
        }
    }
    
    void debug(const std::string& category, const std::string& message,
               const std::string& operation = "") {
        log(ErrorSeverity::Debug, category, message, operation);
    }
    
    void info(const std::string& category, const std::string& message,
              const std::string& operation = "") {
        log(ErrorSeverity::Info, category, message, operation);
    }
    
    void warning(const std::string& category, const std::string& message,
                 const std::string& operation = "") {
        log(ErrorSeverity::Warning, category, message, operation);
    }
    
    void error(const std::string& category, const std::string& message,
               const std::string& operation = "") {
        log(ErrorSeverity::Error, category, message, operation);
    }
    
    void critical(const std::string& category, const std::string& message,
                  const std::string& operation = "") {
        log(ErrorSeverity::Critical, category, message, operation);
    }
    
private:
    LoggingManager() : logLevel(ErrorSeverity::Info), logToFile(false) {}
    
    std::string formatLogEntry(ErrorSeverity severity, const std::string& category,
                               const std::string& message, const std::string& operation) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] ";
        
        // Add severity prefix
        switch (severity) {
            case ErrorSeverity::Debug: ss << "DEBUG "; break;
            case ErrorSeverity::Info: ss << "INFO  "; break;
            case ErrorSeverity::Warning: ss << "WARN  "; break;
            case ErrorSeverity::Error: ss << "ERROR "; break;
            case ErrorSeverity::Critical: ss << "CRIT "; break;
        }
        
        // Add category
        ss << "[" << category << "] ";
        
        // Add operation
        if (!operation.empty()) ss << "[" << operation << "] ";
        
        // Add message
        ss << message;
        
        return ss.str();
    }
    
    ErrorSeverity logLevel;
    std::string logFile;
    bool logToFile;
};

// Global initialization function
void initializeSystem() {
    ErrorManager::getInstance().initialize();
    LoggingManager::getInstance().setLogLevel(ErrorSeverity::Info);
}

} // namespace nlm

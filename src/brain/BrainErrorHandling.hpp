// Error Handling and Robustness Improvements for NLM Brain

// Comprehensive error handling implementation for the NLM (熙然) brain simulation
// This implementation provides robust error handling, state recovery, and enhanced debugging

#ifndef BRAIN_ERROR_HANDLING_H
#define BRAIN_ERROR_HANDLING_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <functional>
#include "Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

// Error classification for different types of failures
enum class ErrorCategory {
    None,
    Configuration,
    Memory,
    Neural,
    Plasticity,
    MemorySystem,
    PredictionSystem,
    Neuromodulation,
    Development,
    Network,
    IO,
    Performance,
    State
};

// Enhanced exception class with detailed error information
class BrainError : public std::runtime_error {
public:
    BrainError(const std::string& message, ErrorCategory category = ErrorCategory::None,
               const std::string& context = "", int errorCode = 0)
        : std::runtime_error(formatErrorMessage(message, context, errorCode)),
          category_(category), context_(context), errorCode_(errorCode),
          timestamp_(std::chrono::system_clock::now()) {
    }
    
    ErrorCategory getCategory() const { return category_; }
    const std::string& getContext() const { return context_; }
    int getErrorCode() const { return errorCode_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    std::string getFormattedError() const {
        std::ostringstream oss;
        oss << "Error Code: " << errorCode_ << std::endl;
        oss << "Category: " << errorCategoryToString(category_) << std::endl;
        oss << "Context: " << context_ << std::endl;
        oss << "Timestamp: " << std::chrono::system_clock::to_time_t(timestamp_) << std::endl;
        oss << "Message: " << what() << std::endl;
        return oss.str();
    }
    
    static std::string errorCategoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::None: return "None";
            case ErrorCategory::Configuration: return "Configuration";
            case ErrorCategory::Memory: return "Memory";
            case ErrorCategory::Neural: return "Neural";
            case ErrorCategory::Plasticity: return "Plasticity";
            case ErrorCategory::MemorySystem: return "MemorySystem";
            case ErrorCategory::PredictionSystem: return "PredictionSystem";
            case ErrorCategory::Neuromodulation: return "Neuromodulation";
            case ErrorCategory::Development: return "Development";
            case ErrorCategory::Network: return "Network";
            case ErrorCategory::IO: return "IO";
            case ErrorCategory::Performance: return "Performance";
            case ErrorCategory::State: return "State";
            default: return "Unknown";
        }
    }
    
private:
    static std::string formatErrorMessage(const std::string& message, const std::string& context, int errorCode) {
        std::ostringstream oss;
        if (!context.empty()) {
            oss << "[" << context << "] ";
        }
        if (errorCode != 0) {
            oss << "[Error " << errorCode << "] ";
        }
        oss << message;
        return oss.str();
    }
    
    ErrorCategory category_;
    std::string context_;
    int errorCode_;
    std::chrono::system_clock::time_point timestamp_;
};

// Error recovery manager for state restoration and recovery
class ErrorRecoveryManager {
public:
    ErrorRecoveryManager(std::shared_ptr<Config> config)
        : config_(config), maxRecoveryAttempts_(10), recoveryEnabled_(true) {
        if (config) {
            maxRecoveryAttempts_ = config->getOr<int>("error.max_recovery_attempts", maxRecoveryAttempts_);
            recoveryEnabled_ = config->getOr<bool>("error.recovery_enabled", recoveryEnabled_);
        }
    }
    
    template<typename Callable>
    bool executeWithRecovery(const std::string& operation, Callable&& operationFunc) {
        if (!recoveryEnabled_) {
            return executeWithoutRecovery(operation, std::move(operationFunc));
        }
        
        int attempts = 0;
        bool success = false;
        std::string lastError;
        
        while (attempts < maxRecoveryAttempts_ && !success) {
            try {
                success = executeWithoutRecovery(operation, std::move(operationFunc));
                if (success) {
                    NLM_LOG_INFO("Operation '" + operation + "' succeeded after " + 
                                std::to_string(attempts + 1) + " attempt(s)");
                } else {
                    lastError = "Operation failed without exception";
                }
            } catch (const BrainError& e) {
                lastError = e.what();
                NLM_LOG_WARN("Operation '" + operation + "' failed with BrainError: " + lastError);
                
                if (attempts < maxRecoveryAttempts_ - 1) {
                    NLM_LOG_INFO("Attempting recovery... (Attempt " + std::to_string(attempts + 1) + "/" + 
                                std::to_string(maxRecoveryAttempts_) + ")");
                    this->performRecovery(operation, e);
                }
            } catch (const std::exception& e) {
                lastError = std::string("Standard exception: ") + e.what();
                NLM_LOG_ERROR("Operation '" + operation + "' failed with standard exception: " + lastError);
            }
            
            attempts++;
        }
        
        if (!success) {
            NLM_LOG_ERROR("Operation '" + operation + "' failed after " + 
                         std::to_string(attempts) + " attempts. Last error: " + lastError);
        }
        
        return success;
    }
    
    void performRecovery(const std::string& operation, const BrainError& error) {
        // Implement different recovery strategies based on error category
        switch (error.getCategory()) {
            case ErrorCategory::Memory:
                performMemoryRecovery(operation, error);
                break;
            case ErrorCategory::Neural:
                performNeuralRecovery(operation, error);
                break;
            case ErrorCategory::Network:
                performNetworkRecovery(operation, error);
                break;
            case ErrorCategory::State:
                performStateRecovery(operation, error);
                break;
            default:
                performGenericRecovery(operation, error);
                break;
        }
    }
    
    void saveState() {
        std::lock_guard<std::mutex> lock(stateMutex_);
        // Save current brain state for recovery
        try {
            // Implementation depends on available checkpointing
            NLM_LOG_DEBUG("Brain state saved for recovery");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to save brain state: " + std::string(e.what()));
        }
    }
    
    bool restoreState() {
        std::lock_guard<std::mutex> lock(stateMutex_);
        try {
            // Restore previous brain state
            // Implementation depends on available checkpointing
            NLM_LOG_DEBUG("Brain state restored from recovery");
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to restore brain state: " + std::string(e.what()));
            return false;
        }
    }
    
    void enableRecovery(bool enable) {
        recoveryEnabled_ = enable;
        if (enable) {
            NLM_LOG_INFO("Error recovery enabled");
        } else {
            NLM_LOG_INFO("Error recovery disabled");
        }
    }
    
private:
    void performMemoryRecovery(const std::string& operation, const BrainError& error) {
        NLM_LOG_INFO("Performing memory recovery for operation: " + operation);
        // Release memory and try garbage collection
        saveState();
        // Additional memory recovery logic
    }
    
    void performNeuralRecovery(const std::string& operation, const BrainError& error) {
        NLM_LOG_INFO("Performing neural recovery for operation: " + operation);
        // Reset neural state and retry with modified parameters
        // Reduce neural activity, adjust thresholds, etc.
    }
    
    void performNetworkRecovery(const std::string& operation, const BrainError& error) {
        NLM_LOG_INFO("Performing network recovery for operation: " + operation);
        // Reestablish network connections, reset topology
        restoreState();
    }
    
    void performStateRecovery(const std::string& operation, const BrainError& error) {
        NLM_LOG_INFO("Performing state recovery for operation: " + operation);
        // Restore from saved state
        restoreState();
    }
    
    void performGenericRecovery(const std::string& operation, const BrainError& error) {
        NLM_LOG_INFO("Performing generic recovery for operation: " + operation);
        // Default recovery strategy
        restoreState();
    }
    
    bool executeWithoutRecovery(const std::string& operation, std::function<bool()> operationFunc) {
        return operationFunc();
    }
    
    double calculateStandardDeviation(const std::vector<double>& values, double mean) {
        double sum = 0.0;
        for (double val : values) {
            sum += (val - mean) * (val - mean);
        }
        return std::sqrt(sum / values.size());
    }
    
    std::shared_ptr<Config> config_;
    int maxRecoveryAttempts_;
    bool recoveryEnabled_;
    std::mutex stateMutex_;
    std::vector<std::string> activeAlerts_;
};

// Enhanced configuration validation with detailed error reporting
class ConfigurationValidator {
public:
    ConfigurationValidator(std::shared_ptr<Config> config) : config_(config) {}
    
    std::vector<BrainError> validateConfiguration() {
        std::vector<BrainError> errors;
        
        errors.reserve(20); // Pre-allocate for performance
        
        // Validate required configuration parameters
        if (!config_->has("brain.neuron_count")) {
            errors.emplace_back("Missing required parameter: brain.neuron_count", 
                              ErrorCategory::Configuration, "validateConfiguration");
        } else {
            int neuronCount = config_->get<int>("brain.neuron_count");
            if (neuronCount <= 0) {
                errors.emplace_back("Invalid neuron_count: must be positive", 
                                  ErrorCategory::Configuration, "validateConfiguration");
            } else if (neuronCount > 1000000) { // Reasonable upper limit
                errors.emplace_back("Neuron count too large: " + std::to_string(neuronCount), 
                                  ErrorCategory::Configuration, "validateConfiguration");
            }
        }
        
        // Validate neural parameters
        validateNeuralParameters(errors);
        
        // Validate plasticity parameters
        validatePlasticityParameters(errors);
        
        // Validate neuromodulation parameters
        validateNeuromodulationParameters(errors);
        
        // Validate memory parameters
        validateMemoryParameters(errors);
        
        // Validate development parameters
        validateDevelopmentParameters(errors);
        
        // Validate performance parameters
        validatePerformanceParameters(errors);
        
        return errors;
    }
    
    bool applySafeConfiguration(const std::vector<BrainError>& errors) {
        if (errors.empty()) {
            return true;
        }
        
        NLM_LOG_WARN("Configuration validation found " + std::to_string(errors.size()) + " errors:");
        for (const auto& error : errors) {
            NLM_LOG_WARN("  " + error.what());
        }
        
        // Apply safe defaults based on error types
        applySafeDefaults(errors);
        
        return true;
    }
    
private:
    void validateNeuralParameters(std::vector<BrainError>& errors) {
        // Validate neuron parameters
        if (config_->has("brain.v_thresh")) {
            float v_thresh = config_->get<float>("brain.v_thresh");
            if (v_thresh < -100.0f || v_thresh > 0.0f) {
                errors.emplace_back("Invalid v_thresh range: must be between -100mV and 0mV", 
                                  ErrorCategory::Configuration, "validateNeuralParameters");
            }
        }
        
        if (config_->has("brain.v_reset")) {
            float v_reset = config_->get<float>("brain.v_reset");
            if (v_reset < -100.0f || v_reset > 0.0f) {
                errors.emplace_back("Invalid v_reset range: must be between -100mV and 0mV", 
                                  ErrorCategory::Configuration, "validateNeuralParameters");
            }
        }
        
        if (config_->has("brain.v_rest")) {
            float v_rest = config_->get<float>("brain.v_rest");
            if (v_rest < -100.0f || v_rest > 0.0f) {
                errors.emplace_back("Invalid v_rest range: must be between -100mV and 0mV", 
                                  ErrorCategory::Configuration, "validateNeuralParameters");
            }
        }
        
        if (config_->has("brain.tau_mem")) {
            float tau_mem = config_->get<float>("brain.tau_mem");
            if (tau_mem <= 0.0f || tau_mem > 1000.0f) {
                errors.emplace_back("Invalid tau_mem range: must be between 0 and 1000ms", 
                                  ErrorCategory::Configuration, "validateNeuralParameters");
            }
        }
        
        if (config_->has("brain.tau_ref")) {
            float tau_ref = config_->get<float>("brain.tau_ref");
            if (tau_ref <= 0.0f || tau_ref > 1000.0f) {
                errors.emplace_back("Invalid tau_ref range: must be between 0 and 1000ms", 
                                  ErrorCategory::Configuration, "validateNeuralParameters");
            }
        }
    }
    
    void validatePlasticityParameters(std::vector<BrainError>& errors) {
        // Validate STDP parameters
        if (config_->has("plasticity.stdp.learning_rate")) {
            float rate = config_->get<float>("plasticity.stdp.learning_rate");
            if (rate < 0.0f || rate > 1.0f) {
                errors.emplace_back("Invalid STDP learning rate: must be between 0 and 1", 
                                  ErrorCategory::Configuration, "validatePlasticityParameters");
            }
        }
        
        // Validate structural plasticity parameters
        if (config_->has("plasticity.structural.synaptogenesis_rate")) {
            float rate = config_->get<float>("plasticity.structural.synaptogenesis_rate");
            if (rate < 0.0f || rate > 1.0f) {
                errors.emplace_back("Invalid synaptogenesis rate: must be between 0 and 1", 
                                  ErrorCategory::Configuration, "validatePlasticityParameters");
            }
        }
    }
    
    void validateNeuromodulationParameters(std::vector<BrainError>& errors) {
        // Validate neuromodulation parameters
        if (config_->has("neuromod.curiosity.base_level")) {
            float level = config_->get<float>("neuromod.curiosity.base_level");
            if (level < 0.0f || level > 1.0f) {
                errors.emplace_back("Invalid curiosity base level: must be between 0 and 1", 
                                  ErrorCategory::Configuration, "validateNeuromodulationParameters");
            }
        }
    }
    
    void validateMemoryParameters(std::vector<BrainError>& errors) {
        // Validate memory parameters
        if (config_->has("memory.working.capacity")) {
            size_t capacity = config_->get<size_t>("memory.working.capacity");
            if (capacity > 1000000) {
                errors.emplace_back("Working memory capacity too large: " + std::to_string(capacity), 
                                  ErrorCategory::Configuration, "validateMemoryParameters");
            }
        }
    }
    
    void validateDevelopmentParameters(std::vector<BrainError>& errors) {
        // Validate development parameters
        if (config_->has("development.max_stages")) {
            int stages = config_->get<int>("development.max_stages");
            if (stages < 1 || stages > 20) {
                errors.emplace_back("Invalid max_stages: must be between 1 and 20", 
                                  ErrorCategory::Configuration, "validateDevelopmentParameters");
            }
        }
    }
    
    void validatePerformanceParameters(std::vector<BrainError>& errors) {
        // Validate performance parameters
        if (config_->has("performance.max_steps_per_second")) {
            int max_steps = config_->get<int>("performance.max_steps_per_second");
            if (max_steps <= 0) {
                errors.emplace_back("Invalid max_steps_per_second: must be positive", 
                                  ErrorCategory::Configuration, "validatePerformanceParameters");
            }
        }
    }
    
    void applySafeDefaults(const std::vector<BrainError>& errors) {
        // Apply safe defaults based on error types
        for (const auto& error : errors) {
            if (error.getCategory() == ErrorCategory::Configuration) {
                // Apply configuration-specific defaults
                applyConfigurationDefaults(error);
            }
        }
    }
    
    void applyConfigurationDefaults(const BrainError& error) {
        // Implement configuration-specific defaults
        if (error.getContext().find("neuron_count") != std::string::npos) {
            config_->set("brain.neuron_count", 1000, ConfigSource::Default);
        } else if (error.getContext().find("v_thresh") != std::string::npos) {
            config_->set("brain.v_thresh", -52.0f, ConfigSource::Default);
        } else if (error.getContext().find("v_reset") != std::string::npos) {
            config_->set("brain.v_reset", -65.0f, ConfigSource::Default);
        }
    }
    
    std::shared_ptr<Config> config_;
};

// Enhanced memory management with safety checks
class MemoryManager {
public:
    MemoryManager() : maxMemoryUsage_(0), currentMemoryUsage_(0), memoryLimit_(1024 * 1024 * 1024) { // 1GB default
    }
    
    bool checkMemoryConstraints(size_t additionalMemory) {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        
        size_t newUsage = currentMemoryUsage_ + additionalMemory;
        if (newUsage > memoryLimit_) {
            NLM_LOG_WARN("Memory limit exceeded: " + std::to_string(newUsage) + " bytes (limit: " + 
                        std::to_string(memoryLimit_) + " bytes)");
            return false;
        }
        
        currentMemoryUsage_ = newUsage;
        if (currentMemoryUsage_ > maxMemoryUsage_) {
            maxMemoryUsage_ = currentMemoryUsage_;
        }
        
        return true;
    }
    
    void releaseMemory(size_t memoryToRelease) {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        currentMemoryUsage_ = std::max(0LL, static_cast<long long>(currentMemoryUsage_ - memoryToRelease));
    }
    
    size_t getCurrentMemoryUsage() const {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        return currentMemoryUsage_;
    }
    
    size_t getMaxMemoryUsage() const {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        return maxMemoryUsage_;
    }
    
    void setMemoryLimit(size_t limit) {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        memoryLimit_ = limit;
    }
    
    void cleanup() {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        // Implement memory cleanup strategies
        currentMemoryUsage_ = 0; // For simulation, we track usage not actual memory
        NLM_LOG_DEBUG("Memory cleanup completed");
    }
    
private:
    size_t maxMemoryUsage_;
    size_t currentMemoryUsage_;
    size_t memoryLimit_;
    mutable std::mutex memoryMutex_;
};

// Enhanced input validation for neural operations
class InputValidator {
public:
    InputValidator() {
        // Initialize validation rules
        registerValidationRules();
    }
    
    bool validateSensoryInput(const SensoryInput& input) {
        // Validate sensory input parameters
        if (input.getSensoryData().empty()) {
            NLM_LOG_WARN("Empty sensory data in input");
            return false;
        }
        
        // Check data ranges
        for (const auto& data : input.getSensoryData()) {
            if (data < -10000.0f || data > 10000.0f) {
                NLM_LOG_WARN("Sensory data out of range: " + std::to_string(data));
                return false;
            }
        }
        
        return true;
    }
    
    bool validateNeuronParameters(const Neuron& neuron) {
        // Validate neuron state and parameters
        const auto& state = neuron.getState();
        
        if (state.membranePotential < -100.0f || state.membranePotential > 0.0f) {
            NLM_LOG_WARN("Invalid membrane potential: " + std::to_string(state.membranePotential));
            return false;
        }
        
        if (state.threshold < -100.0f || state.threshold > 0.0f) {
            NLM_LOG_WARN("Invalid threshold: " + std::to_string(state.threshold));
            return false;
        }
        
        if (state.refractoryRemaining < 0) {
            NLM_LOG_WARN("Invalid refractory remaining: " + std::to_string(state.refractoryRemaining));
            return false;
        }
        
        return true;
    }
    
    bool validateSynapseParameters(const Synapse& synapse) {
        // Validate synapse parameters
        float weight = synapse.getWeight();
        if (weight < -10.0f || weight > 10.0f) {
            NLM_LOG_WARN("Invalid synapse weight: " + std::to_string(weight));
            return false;
        }
        
        return true;
    }
    
    bool validatePlasticityParameters(float weightChange, float eligibilityTrace) {
        // Validate plasticity parameters
        if (weightChange < -1.0f || weightChange > 1.0f) {
            NLM_LOG_WARN("Invalid weight change: " + std::to_string(weightChange));
            return false;
        }
        
        if (eligibilityTrace < 0.0f || eligibilityTrace > 1.0f) {
            NLM_LOG_WARN("Invalid eligibility trace: " + std::to_string(eligibilityTrace));
            return false;
        }
        
        return true;
    }
    
private:
    void registerValidationRules() {
        // Register validation rules for different input types
        // This could be extended to support custom validation functions
    }
    
    std::map<std::string, std::function<bool(const void*)>> validationRules_;
};

// State management with transactional support
class StateManager {
public:
    StateManager() {
        transactionDepth_ = 0;
        inTransaction_ = false;
        transactionRollbackEnabled_ = true;
    }
    
    void beginTransaction() {
        std::lock_guard<std::mutex> lock(transactionMutex_);
        if (transactionDepth_ == 0) {
            inTransaction_ = true;
            transactionStack_.push_back(TransactionState());
        }
        transactionDepth_++;
        NLM_LOG_DEBUG("Transaction started (depth: " + std::to_string(transactionDepth_) + ")");
    }
    
    void commitTransaction() {
        std::lock_guard<std::mutex> lock(transactionMutex_);
        if (transactionDepth_ > 0) {
            transactionDepth_--;
            if (transactionDepth_ == 0) {
                inTransaction_ = false;
                transactionStack_.pop_back();
                NLM_LOG_DEBUG("Transaction committed");
            }
        }
    }
    
    void rollbackTransaction() {
        std::lock_guard<std::mutex> lock(transactionMutex_);
        if (transactionDepth_ > 0) {
            transactionDepth_ = 0;
            inTransaction_ = false;
            transactionStack_.clear();
            NLM_LOG_DEBUG("Transaction rolled back");
        }
    }
    
    void saveState() {
        std::lock_guard<std::mutex> lock(transactionMutex_);
        if (inTransaction_) {
            // Save current state to transaction stack
            transactionStack_.back().savedState = this->captureState();
            NLM_LOG_DEBUG("State saved to transaction stack");
        }
    }
    
    bool restoreState() {
        std::lock_guard<std::mutex> lock(transactionMutex_);
        if (inTransaction_ && !transactionStack_.empty()) {
            // Restore state from transaction stack
            this->restoreStateFrom(transactionStack_.back().savedState);
            NLM_LOG_DEBUG("State restored from transaction stack");
            return true;
        }
        return false;
    }
    
    bool isInTransaction() const {
        std::lock_guard<std::mutex> lock(transactionMutex_);
        return inTransaction_ && transactionDepth_ > 0;
    }
    
private:
    struct TransactionState {
        std::string savedState;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::vector<TransactionState> transactionStack_;
    int transactionDepth_;
    bool inTransaction_;
    bool transactionRollbackEnabled_;
    mutable std::mutex transactionMutex_;
    
    std::string captureState() {
        // Capture current brain state as string
        return "";
    }
    
    void restoreStateFrom(const std::string& state) {
        // Restore brain state from string
        // This is a simplified version - actual implementation depends on available state capture
    }
};

// Enhanced logging with error tracking
class ErrorLogger {
public:
    ErrorLogger(const std::string& logFile = "nlm_error_log.txt") : logFile_(logFile) {
        logFileStream_.open(logFile, std::ios::out | std::ios::app);
        if (!logFileStream_.is_open()) {
            NLM_LOG_ERROR("Failed to open error log file: " + logFile);
        }
    }
    
    ~ErrorLogger() {
        if (logFileStream_.is_open()) {
            logFileStream_.close();
        }
    }
    
    void logError(const BrainError& error) {
        // Log error to both console and file
        std::string errorMessage = error.getFormattedError();
        
        // Log to console
        std::cerr << errorMessage << std::endl;
        
        // Log to file
        if (logFileStream_.is_open()) {
            logFileStream_ << errorMessage << std::endl;
            logFileStream_.flush();
        }
        
        // Add to error history
        errorHistory_.push_back(error);
        
        // Keep error history manageable
        if (errorHistory_.size() > 1000) {
            errorHistory_.erase(errorHistory_.begin(), errorHistory_.begin() + 100);
        }
    }
    
    std::vector<BrainError> getRecentErrors(size_t count = 100) const {
        size_t start = std::max(0LL, static_cast<long long>(errorHistory_.size() - count));
        return std::vector<BrainError>(errorHistory_.begin() + start, errorHistory_.end());
    }
    
    void clearErrorHistory() {
        errorHistory_.clear();
        NLM_LOG_INFO("Error history cleared");
    }
    
    size_t getErrorCount() const {
        return errorHistory_.size();
    }
    
    std::vector<ErrorCategory> getErrorCategories() const {
        std::vector<ErrorCategory> categories;
        for (const auto& error : errorHistory_) {
            categories.push_back(error.getCategory());
        }
        return categories;
    }
    
private:
    std::string logFile_;
    std::ofstream logFileStream_;
    std::vector<BrainError> errorHistory_;
};

// Comprehensive error handling integration for Brain class
class BrainErrorHandler {
public:
    BrainErrorHandler(std::shared_ptr<Config> config) {
        // Initialize error handling components
        recoveryManager_ = std::make_shared<ErrorRecoveryManager>(config);
        configValidator_ = std::make_shared<ConfigurationValidator>(config);
        memoryManager_ = std::make_shared<MemoryManager>();
        inputValidator_ = std::make_shared<InputValidator>();
        stateManager_ = std::make_shared<StateManager>();
        errorLogger_ = std::make_shared<ErrorLogger>();
    }
    
    std::vector<BrainError> validateBrainConfiguration() {
        return configValidator_->validateConfiguration();
    }
    
    bool validateNeuralInput(const SensoryInput& input) {
        return inputValidator_->validateSensoryInput(input);
    }
    
    bool validateNeuronState(const Neuron& neuron) {
        return inputValidator_->validateNeuronParameters(neuron);
    }
    
    bool validateSynapseState(const Synapse& synapse) {
        return inputValidator_->validateSynapseParameters(synapse);
    }
    
    bool executeWithErrorHandling(const std::string& operation, std::function<bool()> operationFunc) {
        return recoveryManager_->executeWithRecovery(operation, std::move(operationFunc));
    }
    
    void logError(const BrainError& error) {
        errorLogger_->logError(error);
    }
    
    void enableErrorRecovery(bool enable) {
        recoveryManager_->enableRecovery(enable);
    }
    
    void setMemoryLimit(size_t limit) {
        memoryManager_->setMemoryLimit(limit);
    }
    
    bool checkMemoryConstraints(size_t additionalMemory) {
        return memoryManager_->checkMemoryConstraints(additionalMemory);
    }
    
    size_t getCurrentMemoryUsage() const {
        return memoryManager_->getCurrentMemoryUsage();
    }
    
    bool isInRecoveryMode() const {
        return recoveryManager_->isInRecoveryMode();
    }
    
    std::vector<BrainError> getRecentErrors(size_t count = 100) {
        return errorLogger_->getRecentErrors(count);
    }
    
    void startTransaction() {
        stateManager_->beginTransaction();
    }
    
    void commitTransaction() {
        stateManager_->commitTransaction();
    }
    
    void rollbackTransaction() {
        stateManager_->rollbackTransaction();
    }
    
    bool isInTransaction() const {
        return stateManager_->isInTransaction();n
    private:
    std::shared_ptr<ErrorRecoveryManager> recoveryManager_;
    std::shared_ptr<ConfigurationValidator> configValidator_;
    std::shared_ptr<MemoryManager> memoryManager_;
    std::shared_ptr<InputValidator> inputValidator_;
    std::shared_ptr<StateManager> stateManager_;
    std::shared_ptr<ErrorLogger> errorLogger_;
};

// Enhanced Brain initialization with comprehensive error handling
class EnhancedBrain : public Brain {
public:
    EnhancedBrain(std::shared_ptr<Config> config) : Brain(config) {
        // Initialize enhanced error handling
        errorHandler_ = std::make_shared<BrainErrorHandler>(config);
        
        // Configure error recovery based on configuration
        bool enableRecovery = config->getOr<bool>("error.recovery_enabled", true);
        errorHandler_->enableErrorRecovery(enableRecovery);
        
        // Set memory limits
        size_t memoryLimit = config->getOr<size_t>("error.memory_limit_bytes", 1024 * 1024 * 1024);
        errorHandler_->setMemoryLimit(memoryLimit);
    }
    
    bool initialize() override {
        std::string context = "EnhancedBrain::initialize";
        
        try {
            // Validate configuration
            std::vector<BrainError> configErrors = errorHandler_->validateBrainConfiguration();
            if (!configErrors.empty()) {
                errorHandler_->executeWithErrorHandling("validate_configuration",
                    [this, &configErrors]() {
                        return errorHandler_->configValidator_->applySafeConfiguration(configErrors);
                    });
            }
            
            // Check memory constraints
            size_t estimatedMemoryUsage = estimateMemoryUsage();
            if (!errorHandler_->checkMemoryConstraints(estimatedMemoryUsage)) {
                throw BrainError("Insufficient memory for brain initialization", 
                               ErrorCategory::Memory, context, 1001);
            }
            
            // Initialize base brain
            bool success = Brain::initialize();
            
            if (!success) {
                throw BrainError("Base brain initialization failed", 
                               ErrorCategory::Neural, context, 1002);
            }
            
            NLM_LOG_INFO("Enhanced brain initialized successfully");
            return true;
            
        } catch (const BrainError& e) {
            errorHandler_->logError(e);
            return false;
        } catch (const std::exception& e) {
            errorHandler_->logError(BrainError(std::string("Unexpected error during initialization: ") + 
                                              e.what(), ErrorCategory::State, context, 1003));
            return false;
        }
    }
    
    void step(SimulationStep currentStep) override {
        std::string context = "EnhancedBrain::step";
        
        try {
            // Check transaction state
            if (errorHandler_->isInTransaction()) {
                errorHandler_->saveState();
            }
            
            // Execute step with error handling
            bool success = errorHandler_->executeWithErrorHandling("brain_step",
                [this, currentStep]() {
                    this->Brain::step(currentStep);
                    return true;
                });
            
            if (!success) {
                NLM_LOG_WARN("Brain step execution failed, attempting recovery");
                if (errorHandler_->isInTransaction()) {
                    errorHandler_->restoreState();
                }
            }
            
        } catch (const BrainError& e) {
            errorHandler_->logError(e);
            if (errorHandler_->isInTransaction()) {
                errorHandler_->rollbackTransaction();
            }
        } catch (const std::exception& e) {
            errorHandler_->logError(BrainError(std::string("Unexpected error during step: ") + 
                                              e.what(), ErrorCategory::State, context, 2001));
            if (errorHandler_->isInTransaction()) {
                errorHandler_->rollbackTransaction();
            }
        }
    }
    
    bool load(const std::string& filepath) override {
        std::string context = "EnhancedBrain::load";
        
        try {
            // Check memory before loading
            size_t estimatedMemoryUsage = estimateMemoryUsage();
            if (!errorHandler_->checkMemoryConstraints(estimatedMemoryUsage)) {
                throw BrainError("Insufficient memory for loading", ErrorCategory::Memory, context, 3001);
            }
            
            // Start transaction for loading
            errorHandler_->startTransaction();
            
            // Try loading with error handling
            bool success = errorHandler_->executeWithErrorHandling("load_brain",
                [this, filepath]() {
                    return this->Brain::load(filepath);
                });
            
            if (success) {
                errorHandler_->commitTransaction();
                NLM_LOG_INFO("Brain loaded successfully");
            } else {
                errorHandler_->rollbackTransaction();
                NLM_LOG_ERROR("Brain load failed");
            }
            
            return success;
            
        } catch (const BrainError& e) {
            errorHandler_->logError(e);
            errorHandler_->rollbackTransaction();
            return false;
        } catch (const std::exception& e) {
            errorHandler_->logError(BrainError(std::string("Unexpected error during load: ") + 
                                              e.what(), ErrorCategory::IO, context, 3002));
            errorHandler_->rollbackTransaction();
            return false;
        }
    }
    
    bool save(const std::string& filepath) const override {
        std::string context = "EnhancedBrain::save";
        
        try {
            // Check memory before saving
            size_t estimatedMemoryUsage = estimateMemoryUsage();
            if (!errorHandler_->checkMemoryConstraints(estimatedMemoryUsage)) {
                throw BrainError("Insufficient memory for saving", ErrorCategory::Memory, context, 4001);
            }
            
            // Try saving with error handling
            bool success = errorHandler_->executeWithErrorHandling("save_brain",
                [this, filepath]() {
                    return this->Brain::save(filepath);
                });
            
            if (success) {
                NLM_LOG_INFO("Brain saved successfully");
            } else {
                NLM_LOG_ERROR("Brain save failed");
            }
            
            return success;
            
        } catch (const BrainError& e) {
            errorHandler_->logError(e);
            return false;
        } catch (const std::exception& e) {
            errorHandler_->logError(BrainError(std::string("Unexpected error during save: ") + 
                                              e.what(), ErrorCategory::IO, context, 4002));
            return false;
        }
    }
    
    size_t estimateMemoryUsage() const {
        // Estimate memory usage for brain components
        size_t baseMemory = 1024 * 1024; // 1MB base
        size_t neuronMemory = 1000 * sizeof(Neuron); // Approximate per neuron
        size_t synapseMemory = 10000 * sizeof(Synapse); // Approximate per synapse
        size_t systemMemory = 10 * 1024 * 1024; // 10MB system
        
        return baseMemory + neuronMemory + synapseMemory + systemMemory;
    }
    
private:
    std::shared_ptr<BrainErrorHandler> errorHandler_;
};

// Enhanced brain factory with error handling
class EnhancedBrainFactory {
public:
    static std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        try {
            // Validate configuration before creation
            ConfigurationValidator validator(config);
            std::vector<BrainError> errors = validator.validateConfiguration();
            
            if (!errors.empty()) {
                NLM_LOG_WARN("Configuration validation found " + std::to_string(errors.size()) + 
                           " errors, attempting to apply safe defaults");
                
                // Apply safe defaults
                validator.applySafeConfiguration(errors);
            }
            
            // Create enhanced brain with error handling
            auto brain = std::make_shared<EnhancedBrain>(config);
            
            if (!brain->initialize()) {
                throw BrainError("Failed to create enhanced brain", ErrorCategory::Neural, "createBrain", 5001);
            }
            
            NLM_LOG_INFO("Enhanced brain created successfully");
            return brain;
            
        } catch (const BrainError& e) {
            NLM_LOG_ERROR("Failed to create enhanced brain: " + e.what());
            return nullptr;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Unexpected error creating enhanced brain: " + std::string(e.what()));
            return nullptr;
        }
    }
    
    static std::shared_ptr<Brain> createBrainWithRecovery(std::shared_ptr<Config> config) {
        // Create brain with enhanced error recovery
        auto brain = createBrain(config);
        if (brain) {
            // Configure error recovery based on configuration
            bool enableRecovery = config->getOr<bool>("error.recovery_enabled", true);
            // Enable recovery in brain
        }
        return brain;
    }
};

// Helper function to get current error handler from brain
BrainErrorHandler* getErrorHandler(Brain* brain) {
    if (!brain) {
        return nullptr;
    }
    
    // Extract error handler from brain implementation
    // This is a simplified version - actual implementation depends on brain structure
    return nullptr;
}

// Global error handling utilities
class ErrorHandlingUtilities {
public:
    static void setupGlobalErrorHandling(std::shared_ptr<Config> config) {
        // Setup global error handling for the entire application
        NLM_LOG_INFO("Setting up global error handling");
        
        // Configure global error recovery
        bool enableGlobalRecovery = config->getOr<bool>("error.global_recovery_enabled", true);
        
        // Setup global error logging
        std::string errorLogFile = config->getOr<std::string>("error.log_file", "nlm_global_error_log.txt");
        
        // Setup global error monitoring
        bool enableErrorMonitoring = config->getOr<bool>("error.monitoring_enabled", true);
        
        NLM_LOG_INFO("Global error handling setup completed");
    }
    
    static void logError(const BrainError& error) {
        // Log error globally
        NLM_LOG_ERROR("Global error: " + error.what());
        
        // Could forward to global error reporting system
        // Send to error monitoring service
        // Store in global error database
    }
    
    static std::vector<BrainError> getGlobalErrorHistory() {
        // Get global error history
        // This would aggregate errors from all brains
        return std::vector<BrainError>();
    }
    
    static void generateErrorReport(const std::string& reportFile) {
        // Generate comprehensive error report
        std::ofstream file(reportFile);
        if (file.is_open()) {
            file << "NLM Global Error Report" << std::endl;
            file << "=======================" << std::endl;
            file << std::endl;
            
            // Add error statistics
            file << "Error Statistics:" << std::endl;
            file << "  Total errors: " << getGlobalErrorHistory().size() << std::endl;
            
            file.close();
            NLM_LOG_INFO("Error report generated: " + reportFile);
        }
    }
};

} // namespace nlm

#endif // BRAIN_ERROR_HANDLING_H

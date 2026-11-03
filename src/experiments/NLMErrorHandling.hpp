// Enhanced error handling system for NLM
//
// This module provides comprehensive exception classes and error handling utilities
// for better debugging, error reporting, and robust error recovery.

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <chrono>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <typeinfo>

namespace nlm {

/**
 * @brief Base exception class for all NLM errors
 */
class NLMException : public std::runtime_error {
public:
    enum class ErrorCode {
        // Configuration errors
        INVALID_CONFIG = 1000,
        MISSING_CONFIG_KEY = 1001,
        INVALID_CONFIG_VALUE = 1002,
        CONFIG_FILE_NOT_FOUND = 1003,
        CONFIG_FILE_CORRUPTED = 1004,
        
        // Brain simulation errors
        BRAIN_INITIALIZATION_FAILED = 2000,
        SIMULATION_STEP_FAILED =2001,
        NEURON_ACTIVATION_FAILED = 2002,
        SYNAPSE_UPDATE_FAILED = 2003,
        MEMORY_ALLOCATION_FAILED = 2004,
        
        // World simulation errors
        WORLD_INITIALIZATION_FAILED = 3000,
        WORLD_STEP_FAILED = 3001,
        ACTION_EXECUTION_FAILED = 3002,
        SENSOR_INPUT_FAILED = 3003,
        
        // Agent brain errors
        AGENT_INITIALIZATION_FAILED = 4000,
        PROCESSING_FAILED = 4001,
        DECODING_FAILED = 4002,
        MODULATION_FAILED = 4003,
        
        // Memory errors
        WORKING_MEMORY_ERROR = 5000,
        EPISODIC_MEMORY_ERROR = 5001,
        ASSOCIATIVE_MEMORY_ERROR = 5002,
        
        // Development errors
        DEVELOPMENT_FAILED = 6000,
        CRITICAL_PERIOD_FAILED = 6001,
        MATURATION_FAILED = 6002,
        
        // Neuromodulation errors
        NEUROMODULATION_FAILED = 7000,
        DOPAMINE_ERROR = 7001,
        NOVELTY_ERROR = 7002,
        
        // Prediction errors
        PREDICTION_FAILED = 8000,
        TEMPORAL_PREDICTION_FAILED = 8001,
        CAUSAL_PREDICTION_FAILED = 8002,
        
        // General errors
        UNKNOWN_ERROR = 9000,
        FILE_ACCESS_FAILED = 9001,
        INVALID_INPUT = 9002,
        OUT_OF_MEMORY = 9003,
        TIMEOUT = 9004,
        VALIDATION_FAILED = 9005
    };
    
protected:
    ErrorCode code_;
    std::string context_;
    std::chrono::system_clock::time_point timestamp_;
    std::unordered_map<std::string, std::string> details_;
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string errorCodeToString(ErrorCode code) {
        switch (code) {
            case ErrorCode::INVALID_CONFIG: return "INVALID_CONFIG";
            case ErrorCode::MISSING_CONFIG_KEY: return "MISSING_CONFIG_KEY";
            case ErrorCode::INVALID_CONFIG_VALUE: return "INVALID_CONFIG_VALUE";
            case ErrorCode::CONFIG_FILE_NOT_FOUND: return "CONFIG_FILE_NOT_FOUND";
            case ErrorCode::CONFIG_FILE_CORRUPTED: return "CONFIG_FILE_CORRUPTED";
            case ErrorCode::BRAIN_INITIALIZATION_FAILED: return "BRAIN_INITIALIZATION_FAILED";
            case ErrorCode::SIMULATION_STEP_FAILED: return "SIMULATION_STEP_FAILED";
            case ErrorCode::NEURON_ACTIVATION_FAILED: return "NEURON_ACTIVATION_FAILED";
            case ErrorCode::SYNAPSE_UPDATE_FAILED: return "SYNAPSE_UPDATE_FAILED";
            case ErrorCode::MEMORY_ALLOCATION_FAILED: return "MEMORY_ALLOCATION_FAILED";
            case ErrorCode::WORLD_INITIALIZATION_FAILED: return "WORLD_INITIALIZATION_FAILED";
            case ErrorCode::WORLD_STEP_FAILED: return "WORLD_STEP_FAILED";
            case ErrorCode::ACTION_EXECUTION_FAILED: return "ACTION_EXECUTION_FAILED";
            case ErrorCode::SENSOR_INPUT_FAILED: return "SENSOR_INPUT_FAILED";
            case ErrorCode::AGENT_INITIALIZATION_FAILED: return "AGENT_INITIALIZATION_FAILED";
            case ErrorCode::PROCESSING_FAILED: return "PROCESSING_FAILED";
            case ErrorCode::DECODING_FAILED: return "DECODING_FAILED";
            case ErrorCode::MODULATION_FAILED: return "MODULATION_FAILED";
            case ErrorCode::WORKING_MEMORY_ERROR: return "WORKING_MEMORY_ERROR";
            case ErrorCode::EPISODIC_MEMORY_ERROR: return "EPISODIC_MEMORY_ERROR";
            case ErrorCode::ASSOCIATIVE_MEMORY_ERROR: return "ASSOCIATIVE_MEMORY_ERROR";
            case ErrorCode::DEVELOPMENT_FAILED: return "DEVELOPMENT_FAILED";
            case ErrorCode::CRITICAL_PERIOD_FAILED: return "CRITICAL_PERIOD_FAILED";
            case ErrorCode::MATURATION_FAILED: return "MATURATION_FAILED";
            case ErrorCode::NEUROMODULATION_FAILED: return "NEUROMODULATION_FAILED";
            case ErrorCode::DOPAMINE_ERROR: return "DOPAMINE_ERROR";
            case ErrorCode::NOVELTY_ERROR: return "NOVELTY_ERROR";
            case ErrorCode::PREDICTION_FAILED: return "PREDICTION_FAILED";
            case ErrorCode::TEMPORAL_PREDICTION_FAILED: return "TEMPORAL_PREDICTION_FAILED";
            case ErrorCode::CAUSAL_PREDICTION_FAILED: return "CAUSAL_PREDICTION_FAILED";
            case ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
            case ErrorCode::FILE_ACCESS_FAILED: return "FILE_ACCESS_FAILED";
            case ErrorCode::INVALID_INPUT: return "INVALID_INPUT";
            case ErrorCode::OUT_OF_MEMORY: return "OUT_OF_MEMORY";
            case ErrorCode::TIMEOUT: return "TIMEOUT";
            case ErrorCode::VALIDATION_FAILED: return "VALIDATION_FAILED";
            default: return "UNKNOWN_ERROR";
        }
    }
    
public:
    NLMException(ErrorCode code, const std::string& message)
        : std::runtime_error(formatMessage(code, message)), code_(code), context_(message) {
        timestamp_ = std::chrono::system_clock::now();
    }
    
    NLMException(ErrorCode code, const std::string& message, const std::string& context)
        : std::runtime_error(formatMessage(code, message, context)), code_(code), context_(context) {
        timestamp_ = std::chrono::system_clock::now();
    }
    
    ErrorCode getErrorCode() const { return code_; }
    const std::string& getContext() const { return context_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    
    void addDetail(const std::string& key, const std::string& value) {
        details_[key] = value;
    }
    
    std::string getDetail(const std::string& key) const {
        auto it = details_.find(key);
        return (it != details_.end()) ? it->second : "";
    }
    
    const std::unordered_map<std::string, std::string>& getDetails() const { return details_; }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "[" << errorCodeToString(code_) << "] " << what() << "\n";
        ss << "  Context: " << context_ << "\n";
        ss << "  Timestamp: " << getCurrentTimestamp() << "\n";
        
        if (!details_.empty()) {
            ss << "  Details:\n";
            for (const auto& pair : details_) {
                ss << "    " << pair.first << ": " << pair.second << "\n";
            }
        }
        
        return ss.str();
    }
    
    void log() const {
        std::cout << "NLM ERROR: " << toString();
    }
    
    virtual ~NLMException() = default;
    
private:
    std::string formatMessage(ErrorCode code, const std::string& message) {
        return std::string(errorCodeToString(code)) + ": " + message;
    }
    
    std::string formatMessage(ErrorCode code, const std::string& message, const std::string& context) {
        return std::string(errorCodeToString(code)) + ": " + message + " (Context: " + context + ")";
    }
};

/**
 * @brief Exception classes for specific error categories
 */
class ConfigException : public NLMException {
public:
    ConfigException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "Configuration") {}
    
    ConfigException(ErrorCode code, const std::string& message, const std::string& key)
        : NLMException(code, message, "Configuration - Key: " + key) {
        addDetail("key", key);
    }
};

class BrainException : public NLMException {
public:
    BrainException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "Brain") {}
    
    BrainException(ErrorCode code, const std::string& message, uint64_t neuronId)
        : NLMException(code, message, "Brain - Neuron ID: " + std::to_string(neuronId)) {
        addDetail("neuron_id", std::to_string(neuronId));
    }
};

class WorldException : public NLMException {
public:
    WorldException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "World") {}
    
    WorldException(ErrorCode code, const std::string& message, const std::string& position)
        : NLMException(code, message, "World - Position: " + position) {
        addDetail("position", position);
    }
};

class AgentBrainException : public NLMException {
public:
    AgentBrainException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "AgentBrain") {}
    
    AgentBrainException(ErrorCode code, const std::string& message, const std::string& action)
        : NLMException(code, message, "AgentBrain - Action: " + action) {
        addDetail("action", action);
    }
};

/**
 * @brief Error recovery and retry utilities
 */
class ErrorRecovery {
public:
    struct RecoveryStrategy {
        std::string name;
        std::function<bool()> canRecover;
        std::function<void()> recover;
        int maxRetries;
        std::chrono::duration<double> retryDelay;
    };
    
    static ErrorRecovery& getInstance() {
        static ErrorRecovery instance;
        return instance;
    }
    
    void registerRecoveryStrategy(const RecoveryStrategy& strategy) {
        strategies_[strategy.name] = strategy;
    }
    
    bool attemptRecovery(const std::string& strategyName, const NLMException& error) {
        auto it = strategies_.find(strategyName);
        if (it == strategies_.end()) {
            return false;
        }
        
        const auto& strategy = it->second;
        
        if (!strategy.canRecover()) {
            return false;
        }
        
        for (int retry = 0; retry < strategy.maxRetries; ++retry) {
            try {
                strategy.recover();
                return true;
            } catch (const NLMException& e) {
                if (retry == strategy.maxRetries - 1) {
                    throw;
                }
                
                std::this_thread::sleep_for(strategy.retryDelay);
            }
        }
        
        return false;
    }
    
    std::vector<RecoveryStrategy> getStrategies() const {
        std::vector<RecoveryStrategy> strategies;
        for (const auto& pair : strategies_) {
            strategies.push_back(pair.second);
        }
        return strategies;
    }
    
private:
    ErrorRecovery() {}
    std::unordered_map<std::string, RecoveryStrategy> strategies_;
};

/**
 * @brief Error validation and checking utilities
 */
class ErrorValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::chrono::duration<double> validationTime;
    };
    
    static ValidationResult validateConfig(std::shared_ptr<Config> config) {
        ValidationResult result;
        result.validationTime = std::chrono::duration<double>::zero();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Required keys validation
        std::vector<std::string> requiredKeys = {
            "neuron_count", "region_count", "connection_probability", "random_seed"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config->hasKey(key)) {
                result.errors.push_back("Missing required config key: " + key);
            }
        }
        
        // Value range validation
        if (config->hasKey("neuron_count")) {
            int64_t neuronCount = config->getInt64("neuron_count");
            if (neuronCount <= 0 || neuronCount > 1000000) {
                result.errors.push_back("Invalid neuron_count value: " + std::to_string(neuronCount));
            }
        }
        
        if (config->hasKey("connection_probability")) {
            float probability = config->getFloat("connection_probability");
            if (probability < 0.0f || probability > 1.0f) {
                result.errors.push_back("Invalid connection_probability value: " + std::to_string(probability));
            }
        }
        
        // Memory usage validation
        if (config->hasKey("neuron_count") && config->hasKey("region_count")) {
            int64_t neuronCount = config->getInt64("neuron_count");
            int64_t regionCount = config->getInt64("region_count");
            
            if (neuronCount * regionCount > 1000000) {
                result.warnings.push_back("Large network configuration may cause memory issues");
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.validationTime = endTime - startTime;
        
        result.valid = result.errors.empty();
        
        return result;
    }
    
    static ValidationResult validateBrainState(std::shared_ptr<Brain> brain) {
        ValidationResult result;
        result.validationTime = std::chrono::duration<double>::zero();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Basic brain state validation
        if (!brain) {
            result.errors.push_back("Brain pointer is null");
        }
        
        // Neuron count validation
        uint64_t neuronCount = brain->getTotalNeuronCount();
        if (neuronCount == 0) {
            result.warnings.push_back("No neurons in brain");
        } else if (neuronCount > 1000000) {
            result.warnings.push_back("Large number of neurons may cause performance issues");
        }
        
        // Synapse count validation
        uint64_t synapseCount = brain->getTotalSynapseCount();
        if (synapseCount > neuronCount * 100) {
            result.warnings.push_back("High synapse-to-neuron ratio may cause performance issues");
        }
        
        // Memory usage validation
        float memoryUsage = getMemoryUsage(brain);
        if (memoryUsage > 1024.0f) { // 1GB
            result.warnings.push_back("High memory usage detected");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.validationTime = endTime - startTime;
        
        result.valid = result.errors.empty();
        
        return result;
    }
    
    static ValidationResult validateWorldState(std::shared_ptr<SimpleWorld> world) {
        ValidationResult result;
        result.validationTime = std::chrono::duration<double>::zero();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Basic world validation
        if (!world) {
            result.errors.push_back("World pointer is null");
        }
        
        // Environment bounds validation
        float width = world->getEnvironmentWidth();
        float height = world->getEnvironmentHeight();
        
        if (width <= 0.0f || height <= 0.0f) {
            result.errors.push_back("Invalid environment dimensions");
        } else if (width * height > 1000000.0f) {
            result.warnings.push_back("Large environment may cause performance issues");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.validationTime = endTime - startTime;
        
        result.valid = result.errors.empty();
        
        return result;
    }
    
    static ValidationResult validateAgentBrainState(std::shared_ptr<AgentBrain> agent) {
        ValidationResult result;
        result.validationTime = std::chrono::duration<double>::zero();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Basic agent validation
        if (!agent) {
            result.errors.push_back("AgentBrain pointer is null");
        }
        
        // Sensory input validation
        int sensorySize = agent->getSensoryInputSize();
        if (sensorySize <= 0) {
            result.errors.push_back("Invalid sensory input size");
        }
        
        // Motor output validation
        int motorSize = agent->getMotorOutputSize();
        if (motorSize <= 0) {
            result.errors.push_back("Invalid motor output size");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.validationTime = endTime - startTime;
        
        result.valid = result.errors.empty();
        
        return result;
    }
    
private:
    static float getMemoryUsage(std::shared_ptr<Brain> brain) {
        // Estimate memory usage (simplified)
        uint64_t neuronMemory = brain->getTotalNeuronCount() * sizeof(float);
        uint64_t synapseMemory = brain->getTotalSynapseCount() * sizeof(float);
        return static_cast<float>(neuronMemory + synapseMemory) / (1024 * 1024); // MB
    }
};

/**
 * @brief Error reporting and notification system
 */
class ErrorReporter {
public:
    struct ReportInfo {
        std::string filename;
        std::string format;
        std::chrono::system_clock::time_point timestamp;
        std::vector<std::string> errorMessages;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    static void generateReport(const std::vector<NLMException>& errors,
                               const std::string& filename = "error_report.txt",
                               const std::string& format = "text") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw NLMException(NLMException::ErrorCode::FILE_ACCESS_FAILED,
                             "Cannot open error report file: " + filename);
        }
        
        if (format == "text") {
            writeTextReport(file, errors);
        } else if (format == "json") {
            writeJSONReport(file, errors);
        } else {
            throw NLMException(NLMException::ErrorCode::INVALID_INPUT,
                             "Unsupported report format: " + format);
        }
        
        file.close();
    }
    
    static void sendEmailReport(const std::vector<NLMException>& errors,
                                const std::string& smtpServer,
                                const std::string& recipient,
                                const std::string& subject = "NLM Error Report") {
        // Simplified email sending - would need proper email library
        std::cout << "Email report would be sent to: " << recipient << "\n";
        std::cout << "Subject: " << subject << "\n";
        std::cout << "Errors: " << errors.size() << "\n";
        
        // In real implementation, would use SMTP library
        // For now, just log the report
        generateReport(errors, "email_report.txt", "text");
    }
    
    static void logToSystem(const NLMException& error) {
        // Simplified system logging - would use proper logging library
        std::cout << "[ERROR] " << error.toString() << "\n";
    }
    
    static std::string formatErrorList(const std::vector<NLMException>& errors) {
        std::stringstream ss;
        ss << "Error Report (" << errors.size() << " errors):\n\n";
        
        for (size_t i = 0; i < errors.size(); ++i) {
            ss << "Error " << (i + 1) << ":\n";
            ss << errors[i].toString() << "\n\n";
        }
        
        return ss.str();
    }
    
private:
    static void writeTextReport(std::ofstream& file, const std::vector<NLMException>& errors) {
        file << "NLM Error Report\n";
        file << "=================\n\n";
        
        file << "Generated on: " << getCurrentTimestamp() << "\n\n";
        
        file << formatErrorList(errors);
        
        file << "\nSummary:\n";
        file << "  Total Errors: " << errors.size() << "\n";
        
        std::map<NLMException::ErrorCode, int> errorCount;
        for (const auto& error : errors) {
            errorCount[error.getErrorCode()]++;
        }
        
        for (const auto& pair : errorCount) {
            file << "  " << errorCodeToString(pair.first) << ": " << pair.second << "\n";
        }
    }
    
    static void writeJSONReport(std::ofstream& file, const std::vector<NLMException>& errors) {
        file << "{\n";
        file << "  \"report_type\": \"NLM Error Report\",\n";
        file << "  \"generated_at\": \"" << getCurrentTimestamp() << "\",\n";
        file << "  \"error_count\": " << errors.size() << ",\n";
        file << "  \"errors\": [\n";
        
        for (size_t i = 0; i < errors.size(); ++i) {
            const auto& error = errors[i];
            file << "    {\n";
            file << "      \"code\": \"" << errorCodeToString(error.getErrorCode()) << "\",\n";
            file << "      \"message\": \"" << escapeString(error.what()) << "\",\n";
            file << "      \"context\": \"" << escapeString(error.getContext()) << "\",\n";
            file << "      \"timestamp\": \"" << getCurrentTimestamp() << "\"\n";
            file << "    }";
            
            if (i < errors.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    static std::string errorCodeToString(NLMException::ErrorCode code) {
        // Same implementation as in NLMException
        switch (code) {
            case NLMException::ErrorCode::INVALID_CONFIG: return "INVALID_CONFIG";
            case NLMException::ErrorCode::MISSING_CONFIG_KEY: return "MISSING_CONFIG_KEY";
            case NLMException::ErrorCode::INVALID_CONFIG_VALUE: return "INVALID_CONFIG_VALUE";
            case NLMException::ErrorCode::CONFIG_FILE_NOT_FOUND: return "CONFIG_FILE_NOT_FOUND";
            case NLMException::ErrorCode::CONFIG_FILE_CORRUPTED: return "CONFIG_FILE_CORRUPTED";
            case NLMException::ErrorCode::BRAIN_INITIALIZATION_FAILED: return "BRAIN_INITIALIZATION_FAILED";
            case NLMException::ErrorCode::SIMULATION_STEP_FAILED: return "SIMULATION_STEP_FAILED";
            case NLMException::ErrorCode::NEURON_ACTIVATION_FAILED: return "NEURON_ACTIVATION_FAILED";
            case NLMException::ErrorCode::SYNAPSE_UPDATE_FAILED: return "SYNAPSE_UPDATE_FAILED";
            case NLMException::ErrorCode::MEMORY_ALLOCATION_FAILED: return "MEMORY_ALLOCATION_FAILED";
            case NLMException::ErrorCode::WORLD_INITIALIZATION_FAILED: return "WORLD_INITIALIZATION_FAILED";
            case NLMException::ErrorCode::WORLD_STEP_FAILED: return "WORLD_STEP_FAILED";
            case NLMException::ErrorCode::ACTION_EXECUTION_FAILED: return "ACTION_EXECUTION_FAILED";
            case NLMException::ErrorCode::SENSOR_INPUT_FAILED: return "SENSOR_INPUT_FAILED";
            case NLMException::ErrorCode::AGENT_INITIALIZATION_FAILED: return "AGENT_INITIALIZATION_FAILED";
            case NLMException::ErrorCode::PROCESSING_FAILED: return "PROCESSING_FAILED";
            case NLMException::ErrorCode::DECODING_FAILED: return "DECODING_FAILED";
            case NLMException::ErrorCode::MODULATION_FAILED: return "MODULATION_FAILED";
            case NLMException::ErrorCode::WORKING_MEMORY_ERROR: return "WORKING_MEMORY_ERROR";
            case NLMException::ErrorCode::EPISODIC_MEMORY_ERROR: return "EPISODIC_MEMORY_ERROR";
            case NLMException::ErrorCode::ASSOCIATIVE_MEMORY_ERROR: return "ASSOCIATIVE_MEMORY_ERROR";
            case NLMException::ErrorCode::DEVELOPMENT_FAILED: return "DEVELOPMENT_FAILED";
            case NLMException::ErrorCode::CRITICAL_PERIOD_FAILED: return "CRITICAL_PERIOD_FAILED";
            case NLMException::ErrorCode::MATURATION_FAILED: return "MATURATION_FAILED";
            case NLMException::ErrorCode::NEUROMODULATION_FAILED: return "NEUROMODULATION_FAILED";
            case NLMException::ErrorCode::DOPAMINE_ERROR: return "DOPAMINE_ERROR";
            case NLMException::ErrorCode::NOVELTY_ERROR: return "NOVELTY_ERROR";
            case NLMException::ErrorCode::PREDICTION_FAILED: return "PREDICTION_FAILED";
            case NLMException::ErrorCode::TEMPORAL_PREDICTION_FAILED: return "TEMPORAL_PREDICTION_FAILED";
            case NLMException::ErrorCode::CAUSAL_PREDICTION_FAILED: return "CAUSAL_PREDICTION_FAILED";
            case NLMException::ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
            case NLMException::ErrorCode::FILE_ACCESS_FAILED: return "FILE_ACCESS_FAILED";
            case NLMException::ErrorCode::INVALID_INPUT: return "INVALID_INPUT";
            case NLMException::ErrorCode::OUT_OF_MEMORY: return "OUT_OF_MEMORY";
            case NLMException::ErrorCode::TIMEOUT: return "TIMEOUT";
            case NLMException::ErrorCode::VALIDATION_FAILED: return "VALIDATION_FAILED";
            default: return "UNKNOWN_ERROR";
        }
    }
    
    static std::string escapeString(const std::string& str) {
        std::string escaped = str;
        std::replace(escaped.begin(), escaped.end(), '\\', '\\\\');
        std::replace(escaped.begin(), escaped.end(), '"', '\\"');
        std::replace(escaped.begin(), escaped.end(), '\n', '\\n');
        std::replace(escaped.begin(), escaped.end(), '\r', '\\r');
        return escaped;
    }
};

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP

// Error handling and validation for NLM (熙然) Neural Learning Machine
// Phase 6: Final Integration - Comprehensive Error Management

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace nlm {

// ===== ERROR CLASSES =====
class NLMError : public std::runtime_error {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        INVALID_CONFIG,
        MEMORY_ALLOCATION_FAILED,
        FILE_OPERATION_FAILED,
        COMPONENT_INITIALIZATION_FAILED,
        SIMULATION_ERROR,
        VALIDATION_FAILED,
        OUT_OF_RANGE,
        NULL_POINTER_DEREFERENCE,
        PERMISSION_DENIED,
        TIMEOUT,
        UNSUPPORTED_OPERATION,
        INTERNAL_ERROR
    };

    NLMError(ErrorCode code, const std::string& message, const std::string& context = "")
        : std::runtime_error(formatMessage(code, message, context)), code_(code) {}

    ErrorCode getErrorCode() const { return code_; }
    const std::string& getContext() const { return context_; }

    static std::string errorCodeToString(ErrorCode code) {
        static const std::unordered_map<ErrorCode, std::string> errorStrings = {
            {ErrorCode::SUCCESS, "SUCCESS"},
            {ErrorCode::INVALID_CONFIG, "INVALID_CONFIG"},
            {ErrorCode::MEMORY_ALLOCATION_FAILED, "MEMORY_ALLOCATION_FAILED"},
            {ErrorCode::FILE_OPERATION_FAILED, "FILE_OPERATION_FAILED"},
            {ErrorCode::COMPONENT_INITIALIZATION_FAILED, "COMPONENT_INITIALIZATION_FAILED"},
            {ErrorCode::SIMULATION_ERROR, "SIMULATION_ERROR"},
            {ErrorCode::VALIDATION_FAILED, "VALIDATION_FAILED"},
            {ErrorCode::OUT_OF_RANGE, "OUT_OF_RANGE"},
            {ErrorCode::NULL_POINTER_DEREFERENCE, "NULL_POINTER_DEREFERENCE"},
            {ErrorCode::PERMISSION_DENIED, "PERMISSION_DENIED"},
            {ErrorCode::TIMEOUT, "TIMEOUT"},
            {ErrorCode::UNSUPPORTED_OPERATION, "UNSUPPORTED_OPERATION"},
            {ErrorCode::INTERNAL_ERROR, "INTERNAL_ERROR"}
        };
        
        auto it = errorStrings.find(code);
        return it != errorStrings.end() ? it->second : "UNKNOWN";
    }

private:
    std::string formatMessage(ErrorCode code, const std::string& message, const std::string& context) {
        std::stringstream ss;
        ss << "[" << errorCodeToString(code) << "]";
        if (!context.empty()) {
            ss << " [" << context << "]";
        }
        ss << " " << message;
        return ss.str();
    }

    ErrorCode code_;
    std::string context_;
};

// ===== VALIDATION CLASSES =====
class ValidationResult {
public:
    ValidationResult() : isValid_(true), errorCount_(0) {}
    
    void addError(const std::string& error) {
        errors_.push_back(error);
        isValid_ = false;
        errorCount_++;
    }
    
    void addWarning(const std::string& warning) {
        warnings_.push_back(warning);
    }
    
    bool isValid() const { return isValid_; }
    size_t errorCount() const { return errorCount_; }
    size_t warningCount() const { return warnings_.size(); }
    
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Validation: " << (isValid_ ? "PASS" : "FAIL");
        if (errorCount_ > 0) ss << " (" << errorCount_ << " errors)";
        if (warnings_.size() > 0) ss << " (" << warnings_.size() << " warnings)";
        return ss.str();
    }

private:
    bool isValid_;
    size_t errorCount_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

// ===== CONFIGURATION VALIDATION =====
class ConfigValidator {
public:
    static ValidationResult validate(const std::shared_ptr<Config>& config) {
        ValidationResult result;
        
        if (!config) {
            result.addError("Configuration is null");
            return result;
        }
        
        // Validate basic parameters
        auto validateFloat = [&](const std::string& key, float min, float max, bool required = true) {
            auto valueOpt = config->get<float>(key);
            if (required && !valueOpt) {
                result.addError("Required parameter missing: " + key);
                return;
            }
            if (valueOpt) {
                float value = *valueOpt;
                if (value < min || value > max) {
                    result.addError("Parameter " + key + " out of range. Expected: [" + 
                                  std::to_string(min) + ", " + std::to_string(max) + "], got: " + std::to_string(value));
                }
            }
        };
        
        auto validateInt = [&](const std::string& key, int64_t min, int64_t max, bool required = true) {
            auto valueOpt = config->get<int64_t>(key);
            if (required && !valueOpt) {
                result.addError("Required parameter missing: " + key);
                return;
            }
            if (valueOpt) {
                int64_t value = *valueOpt;
                if (value < min || value > max) {
                    result.addError("Parameter " + key + " out of range. Expected: [" + 
                                  std::to_string(min) + ", " + std::to_string(max) + "], got: " + std::to_string(value));
                }
            }
        };
        
        auto validatePositive = [&](const std::string& key, bool required = true) {
            auto valueOpt = config->get<float>(key);
            if (required && !valueOpt) {
                result.addError("Required parameter missing: " + key);
                return;
            }
            if (valueOpt) {
                if (*valueOpt < 0.0f) {
                    result.addError("Parameter " + key + " must be non-negative, got: " + std::to_string(*valueOpt));
                }
            }
        };
        
        // Validate specific configuration parameters
        validateFloat("simulation_timestep", 0.0001f, 1.0f, true);
        validateFloat("connection_probability", 0.0f, 1.0f, true);
        validateFloat("stdp_ltp_weight", 0.0f, 1.0f, false);
        validateFloat("stdp_ltd_weight", 0.0f, 1.0f, false);
        validateFloat("synaptogenesis_rate", 0.0f, 1.0f, false);
        validateFloat("pruning_rate", 0.0f, 1.0f, false);
        
        validateInt("neuron_count", 1, 1000000, true);
        validateInt("region_count", 1, 100, true);
        validateInt("random_seed", 0, std::numeric_limits<int64_t>::max(), false);
        
        validatePositive("replay_interval", false);
        validatePositive("consolidation_interval", false);
        
        return result;
    }
};

// ===== MEMORY VALIDATION =====
class MemoryValidator {
public:
    static ValidationResult validateNeurons(const Brain* brain) {
        ValidationResult result;
        
        if (!brain) {
            result.addError("Brain is null");
            return result;
        }
        
        size_t totalNeurons = brain->getTotalNeuronCount();
        if (totalNeurons == 0) {
            result.addWarning("No neurons in brain - simulation may not be functional");
        } else if (totalNeurons > 1000000) {
            result.addWarning("Large number of neurons (" + std::to_string(totalNeurons) + ") may impact performance");
        }
        
        // Check neuron validity
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            if (!region) {
                result.addError("Null region found");
                continue;
            }
            
            auto populations = region->getPopulations();
            for (const auto& pop : populations) {
                if (!pop) {
                    result.addError("Null population found in region");
                    continue;
                }
                
                auto neurons = pop->getNeurons();
                for (const auto* neuron : neurons) {
                    if (!neuron) {
                        result.addError("Null neuron found in population");
                        continue;
                    }
                    
                    // Validate neuron state
                    const auto& state = neuron->getState();
                    if (state.membranePotential < -100.0f || state.membranePotential > 100.0f) {
                        result.addWarning("Unusual membrane potential: " + std::to_string(state.membranePotential) + " mV");
                    }
                    
                    if (state.threshold < 0.0f || state.threshold > 100.0f) {
                        result.addError("Invalid neuron threshold: " + std::to_string(state.threshold));
                    }
                }
            }
        }
        
        return result;
    }
    
    static ValidationResult validateSynapses(const Brain* brain) {
        ValidationResult result;
        
        if (!brain) {
            result.addError("Brain is null");
            return result;
        }
        
        size_t totalSynapses = brain->getTotalSynapseCount();
        if (totalSynapses == 0) {
            result.addWarning("No synapses in brain - neural connectivity may be impaired");
        }
        
        // Check synapse validity
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            if (!region) continue;
            
            auto synapses = region->getSynapses();
            for (const auto* syn : synapses) {
                if (!syn) {
                    result.addError("Null synapse found");
                    continue;
                }
                
                float weight = syn->getWeight();
                if (std::abs(weight) > 1000.0f) {
                    result.addWarning("Extreme synaptic weight: " + std::to_string(weight));
                }
                
                Delay delay = syn->getDelay();
                if (delay < 0 || delay > 1000) {
                    result.addError("Invalid synapse delay: " + std::to_string(delay));
                }
            }
        }
        
        return result;
    }
};

// ===== ERROR HANDLING FACILITY =====
class ErrorHandling {
public:
    // Thread-safe error reporting
    static void reportError(const std::string& message, const std::string& context = "") {
        std::stringstream ss;
        ss << "[" << context << "] " << message;
        NLM_LOG_ERROR(ss.str());
    }
    
    // Safe memory allocation with error handling
    template<typename T, typename... Args>
    static std::unique_ptr<T> allocateSafe(Args&&... args) {
        try {
            return std::make_unique<T>(std::forward<Args>(args)...);
        } catch (const std::bad_alloc&) {
            reportError("Memory allocation failed", "allocateSafe");
            throw NLMError(NLMError::ErrorCode::MEMORY_ALLOCATION_FAILED, "Failed to allocate memory", "allocateSafe");
        } catch (...) {
            reportError("Unknown error during memory allocation", "allocateSafe");
            throw NLMError(NLMError::ErrorCode::INTERNAL_ERROR, "Unknown error during memory allocation", "allocateSafe");
        }
    }
    
    // Safe file operation with error handling
    static bool safeFileOperation(const std::string& filepath, 
                                 std::function<bool(std::fstream&)> operation,
                                 const std::string& operationName = "file operation") {
        try {
            std::fstream file(filepath, std::ios::in | std::ios::out);
            if (!file.is_open()) {
                reportError("Failed to open file: " + filepath, operationName);
                return false;
            }
            
            if (!operation(file)) {
                reportError("File operation failed: " + filepath, operationName);
                file.close();
                return false;
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            reportError(std::string("Exception in file operation: ") + e.what(), operationName);
            return false;
        }
    }
    
    // Safe configuration loading
    static std::shared_ptr<Config> loadConfigSafe(const std::string& filepath) {
        try {
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(filepath)) {
                reportError("Failed to load configuration from file", "loadConfigSafe");
                return nullptr;
            }
            return config;
        } catch (...) {
            reportError("Exception while loading configuration", "loadConfigSafe");
            return nullptr;
        }
    }
};

// ===== VALIDATION COORDINATOR =====
class ValidationCoordinator {
public:
    static ValidationResult validateBrain(const Brain* brain, const std::string& context = "") {
        ValidationResult result;
        std::string ctx = "validateBrain";
        if (!context.empty()) ctx += " [" + context + "]";
        
        if (!brain) {
            result.addError("Brain is null");
            return result;
        }
        
        // Validate memory systems
        auto memoryValidation = MemoryValidator::validateNeurons(brain);
        for (const auto& error : memoryValidation.getErrors()) {
            result.addError(error);
        }
        for (const auto& warning : memoryValidation.getWarnings()) {
            result.addWarning(warning);
        }
        
        // Validate synapses
        auto synapseValidation = MemoryValidator::validateSynapses(brain);
        for (const auto& error : synapseValidation.getErrors()) {
            result.addError(error);
        }
        for (const auto& warning : synapseValidation.getWarnings()) {
            result.addWarning(warning);
        }
        
        return result;
    }
    
    static ValidationResult validateExperiment(const CommandLineArgs& args) {
        ValidationResult result;
        
        if (args.experiment != "all" && args.experiment != "basic" && args.experiment != "plasticity" &&
            args.experiment != "stdp" && args.experiment != "phase2" && args.experiment != "phase3" &&
            args.experiment != "phase4" && args.experiment != "phase6") {
            result.addError("Unknown experiment mode: " + args.experiment);
        }
        
        if (args.agentProfile != "default" && args.agentProfile != "explorer" &&
            args.agentProfile != "worker" && args.agentProfile != "social" &&
            args.agentProfile != "conservative" && args.agentProfile != "aggressive" &&
            args.agentProfile != "adaptive") {
            result.addError("Unknown agent profile: " + args.agentProfile);
        }
        
        if (args.benchmarkMode != "off" && args.benchmarkMode != "speed" &&
            args.benchmarkMode != "accuracy" && args.benchmarkMode != "memory" &&
            args.benchmarkMode != "learning") {
            result.addError("Unknown benchmark mode: " + args.benchmarkMode);
        }
        
        if (args.numRuns < 1 || args.numRuns > 100) {
            result.addError("Number of runs must be between 1 and 100, got: " + std::to_string(args.numRuns));
        }
        
        if (args.timeLimit < 0.0) {
            result.addError("Time limit must be non-negative, got: " + std::to_string(args.timeLimit));
        }
        
        if (args.saveCheckpoint && args.checkpointFile.empty()) {
            result.addError("Checkpoint filename is required when saving checkpoint");
        }
        
        return result;
    }
};

// ===== EXCEPTION HANDLING =====
class ExceptionHandler {
public:
    static void setupGlobalExceptionHandler() {
        std::set_terminate([]() {
            NLM_LOG_ERROR("Program terminated abnormally");
            std::abort();
        });
        
        std::set_unexpected([]() {
            NLM_LOG_ERROR("Unexpected exception occurred");
            std::abort();
        });
    }
    
    template<typename Func>
    static auto wrapWithExceptionHandling(Func&& func, const std::string& operationName = "operation") -> decltype(func()) {
        try {
            return func();
        } catch (const NLMError& e) {
            NLM_LOG_ERROR(std::string("NLM Error in " + operationName + ": ") + e.what());
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Standard exception in " + operationName + ": ") + e.what());
            throw NLMError(NLMError::ErrorCode::INTERNAL_ERROR, 
                         std::string("Unexpected error in " + operationName + ": ") + e.what(),
                         operationName);
        } catch (...) {
            NLM_LOG_ERROR(std::string("Unknown exception in " + operationName));
            throw NLMError(NLMError::ErrorCode::INTERNAL_ERROR, 
                         "Unknown error in " + operationName, operationName);
        }
    }
};

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP

(End of file - total 506 lines)
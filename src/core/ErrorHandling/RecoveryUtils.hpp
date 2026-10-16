// **Error Recovery Utilities**
//
// Utilities for error recovery, checkpointing, and graceful degradation.
// Integrates with the error handling system to provide robust recovery mechanisms.

#pragma once

#include "ErrorTypes.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

namespace nlm {
namespace ErrorHandling {

// **Error Recovery Utilities**

// Checkpoint recovery for configuration
class ConfigCheckpoint {
public:
    struct CheckpointData {
        std::string identifier;
        std::chrono::system_clock::time_point timestamp;
        std::string configJson;
        std::string metadata;
    };
    
    explicit ConfigCheckpoint(const std::string& filepath = "checkpoints/config.json")
        : filepath(filepath) {}
    
    // Save configuration state
    bool save(const Config& config, const std::string& identifier = "last") {
        try {
            std::string json;
            // Convert config to JSON-like string (simplified)
            json = "{\"version\": \"1.0\", \"timestamp\": \"" + getTimestamp() + \"\"";
            
            // Write to file
            std::ofstream file(filepath);
            if (!file) {
                return false;
            }
            
            file << json;
            return file.good();
        } catch (...) {
            return false;
        }
    }
    
    // Load configuration state
    std::optional<Config> load(const std::string& identifier = "last") {
        try {
            std::ifstream file(filepath);
            if (!file) {
                return std::nullopt;
            }
            
            // TODO: Implement actual JSON parsing and Config reconstruction
            // For now, return empty config
            return std::nullopt;
        } catch (...) {
            return std::nullopt;
        }
    }
    
private:
    std::string filepath;
    
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        localtime_r(&time_t_now, &tm_buf);
        
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d_%H-%M-%S");
        return oss.str();
    }
};

// Checkpoint recovery for brain
class BrainCheckpoint {
public:
    struct BrainCheckpointData {
        std::string identifier;
        std::chrono::system_clock::time_point timestamp;
        std::string brainState;
        std::vector<NeuronState> neuronStates;
        std::vector<DelayedSpikeEvent> spikeEvents;
        float connectionWeight;
        size_t totalSynapseCount;
    };
    
    explicit BrainCheckpoint(const std::string& directory = "checkpoints/brain")
        : directory(directory) {}
    
    // Save brain state
    bool save(const Brain& brain, const std::string& identifier = "last") {
        try {
            std::filesystem::create_directories(directory);
            
            std::string filepath = directory + "/" + identifier + ".braincp";
            std::ofstream file(filepath, std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Write checkpoint data
            // TODO: Implement actual serialization
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Load brain state
    std::optional<std::unique_ptr<Brain>> load(const std::string& identifier = "last") {
        try {
            std::string filepath = directory + "/" + identifier + ".braincp";
            std::ifstream file(filepath, std::ios::binary);
            if (!file) {
                return std::nullopt;
            }
            
            // TODO: Implement actual deserialization
            // For now, return empty brain
            return std::nullopt;
        } catch (...) {
            return std::nullopt;
        }
    }
    
    // Cleanup old checkpoints
    void cleanup(int keepLast = 5, int maxAgeHours = 24) {
        try {
            std::filesystem::path dir(directory);
            if (!std::filesystem::exists(dir)) return;
            
            std::vector<std::filesystem::path> checkpointFiles;
            for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                if (entry.is_regular_file() && 
                    entry.path().extension() == ".braincp") {
                    checkpointFiles.push_back(entry.path());
                }
            }
            
            // Sort by modification time (newest first)
            std::sort(checkpointFiles.begin(), checkpointFiles.end(),
                     [](const std::filesystem::path& a, const std::filesystem::path& b) {
                         return std::filesystem::last_write_time(a) > 
                                std::filesystem::last_write_time(b);
                     });
            
            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
            
            int keepCount = 0;
            for (const auto& filepath : checkpointFiles) {
                if (keepCount >= keepLast) {
                    auto fileTime = std::filesystem::last_write_time(filepath);
                    auto fileTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                        fileTime.time_since_epoch());
                    
                    auto age = std::chrono::duration_cast<std::chrono::hours>(
                        now_ms - fileTimeMs);
                    
                    if (age.count() > maxAgeHours) {
                        std::filesystem::remove(filepath);
                    }
                }
                keepCount++;
            }
        } catch (...) {
            // Ignore cleanup errors
        }
    }
    
private:
    std::string directory;
};

// Transient error recovery (retry logic)
class TransientErrorRecovery {
public:
    struct RetryConfig {
        int maxAttempts = 3;
        std::chrono::milliseconds initialDelay = std::chrono::milliseconds(100);
        double backoffMultiplier = 2.0;
        std::vector<std::chrono::milliseconds> customDelays;
        std::vector<ErrorCode> retryableErrors;
    };
    
    explicit TransientErrorRecovery(const RetryConfig& config = RetryConfig())
        : config(config) {}
    
    // Execute operation with retry
    template<typename Func, typename... Args>
    auto executeWithRetry(const std::string& operation, const Func& func, Args&&... args)
    -> typename std::result_of<Func(Args...)>::type {
        int attempt = 0;
        std::chrono::milliseconds delay = config.initialDelay;
        
        while (attempt < config.maxAttempts) {
            try {
                return func(std::forward<Args>(args)...);
            } catch (const NLMException& e) {
                // Check if error is retryable
                bool isRetryable = std::find(config.retryableErrors.begin(),
                                            config.retryableErrors.end(),
                                            e.getErrorCode()) != config.retryableErrors.end();
                
                if (!isRetryable) {
                    throw; // Don't retry non-retryable errors
                }
                
                attempt++;
                if (attempt >= config.maxAttempts) {
                    throw; // Max attempts reached, rethrow the last exception
                }
                
                logRetryAttempt(operation, e, attempt, config.maxAttempts);
                
                // Exponential backoff
                if (config.customDelays.empty()) {
                    std::this_thread::sleep_for(delay);
                    delay = std::chrono::milliseconds(static_cast<long>(
                        delay.count() * config.backoffMultiplier));
                } else {
                    if (attempt - 1 < config.customDelays.size()) {
                        std::this_thread::sleep_for(config.customDelays[attempt - 1]);
                    } else {
                        std::this_thread::sleep_for(delay);
                        delay = std::chrono::milliseconds(static_cast<long>(
                            delay.count() * config.backoffMultiplier));
                    }
                }
            } catch (...) {
                // Non-NLM exception
                attempt++;
                if (attempt >= config.maxAttempts) {
                    throw;
                }
                
                logRetryAttempt(operation, "Unexpected exception", attempt, config.maxAttempts);
                std::this_thread::sleep_for(delay);
                delay = std::chrono::milliseconds(static_cast<long>(
                    delay.count() * config.backoffMultiplier));
            }
        }
        
        throw std::runtime_error("Retry failed after " + std::to_string(config.maxAttempts) + 
                               " attempts for operation: " + operation);
    }
    
private:
    RetryConfig config;
    
    void logRetryAttempt(const std::string& operation, const std::string& error,
                        int attempt, int maxAttempts) {
        std::cout << "Retry attempt " << attempt << "/" << maxAttempts 
                 << " for operation: " << operation;
        if (!error.empty()) {
            std::cout << ", error: " << error;
        }
        std::cout << std::endl;
    }
    
    void logRetryAttempt(const std::string& operation, const NLMException& exception,
                        int attempt, int maxAttempts) {
        logRetryAttempt(operation, exception.getMessage(), attempt, maxAttempts);
    }
};

// Graceful degradation utilities
class GracefulDegradation {
public:
    // Degraded value template
    template<typename T>
    class DegradedValue {
    private:
        T value;
        bool isDegraded = false;
        std::string degradationReason;
        
    public:
        DegradedValue() : value(), isDegraded(true), degradationReason("Not initialized") {}
        explicit DegradedValue(const T& val) : value(val), isDegraded(false) {}
        
        DegradedValue(const T& val, const std::string& reason)
            : value(val), isDegraded(true), degradationReason(reason) {}
        
        operator T() const { return value; }
        T get() const { return value; }
        bool isDegradedValue() const { return isDegraded; }
        const std::string& getDegradationReason() const { return degradationReason; }
    };
    
    // Wrap function with graceful degradation
    template<typename Func>
    static DegradedValue<typename std::result_of<Func()>::type> wrap(
        const std::string& operation, const Func& func,
        const std::string& fallbackReason = "Operation unavailable",
        const std::vector<ErrorCode>& degradedErrors = {}) {
        try {
            auto result = func();
            return DegradedValue<decltype(result)>(result, "");
        } catch (const NLMException& e) {
            // Check if error type should trigger degradation
            bool shouldDegrade = std::find(degradedErrors.begin(), degradedErrors.end(),
                                          e.getErrorCode()) != degradedErrors.end();
            
            if (shouldDegrade) {
                return DegradedValue<decltype(result)>(DegradedValue<decltype(result)>::value_type(), fallbackReason);
            } else {
                throw; // Re-throw non-degraded errors
            }
        } catch (...) {
            return DegradedValue<decltype(result)>(DegradedValue<decltype(result)>::value_type(), fallbackReason);
        }
    }
    
    // Create degraded configuration
    static DegradedValue<std::shared_ptr<Config>> degradedConfig(
        const std::string& reason = "Configuration unavailable") {
        return DegradedValue<std::shared_ptr<Config>>(std::shared_ptr<Config>(), reason);
    }
    
    // Create degraded brain
    static DegradedValue<std::unique_ptr<Brain>> degradedBrain(
        const std::string& reason = "Brain unavailable") {
        return DegradedValue<std::unique_ptr<Brain>>(std::unique_ptr<Brain>(), reason);
    }
    
    // Create degraded neuron
    static DegradedValue<std::unique_ptr<Neuron>> degradedNeuron(
        const std::string& reason = "Neuron unavailable") {
        return DegradedValue<std::unique_ptr<Neuron>>(std::unique_ptr<Neuron>(), reason);
    }
    
    // Create degraded synapse
    static DegradedValue<std::unique_ptr<Synapse>> degradedSynapse(
        const std::string& reason = "Synapse unavailable") {
        return DegradedValue<std::unique_ptr<Synapse>>(std::unique_ptr<Synapse>(), reason);
    }
};

// **Error Handling Integration Examples**

// Config error handling integration
class ConfigErrorHandler {
public:
    // Safe config loading with error handling and recovery
    static std::shared_ptr<Config> safeLoadConfig(const std::string& filepath,
                                                 const ConfigCheckpoint& checkpoint,
                                                 const TransientErrorRecovery& retryManager,
                                                 bool createIfMissing = false) {
        try {
            // Try to load from file with retry
            auto loadFunc = [&]() -> std::shared_ptr<Config> {
                auto config = std::make_shared<Config>();
                if (!config->loadFromFile(filepath)) {
                    if (createIfMissing) {
                        // Create default config
                        config = std::make_shared<Config>();
                        config->set("neuron_count", 1000);
                        config->set("simulation_timestep", 0.001);
                        config->set("region_count", 1);
                        config->set("connection_probability", 0.1);
                    } else {
                        throw ConfigFileNotFoundException(filepath);
                    }
                }
                
                // Validate config
                auto validator = std::make_unique<NLMConfigValidator>();
                config->setValidator(std::move(validator));
                auto result = config->validate();
                if (!result.isValid) {
                    throw ConfigValidationException(result.toString());
                }
                
                // Save checkpoint
                checkpoint.save(*config);
                
                return config;
            };
            
            return retryManager.executeWithRetry("LoadConfig", loadFunc);
            
        } catch (const NLMException& e) {
            ErrorHandler::getInstance().reportException(e);
            return nullptr;
        }
    }
    
    // Safe config validation with graceful degradation
    static bool safeValidateConfig(const Config& config,
                                  const std::vector<ErrorCode>& degradedErrors = {}) {
        auto validator = std::make_unique<NLMConfigValidator>();
        config.setValidator(std::move(validator));
        
        auto result = config.validate();
        if (result.isValid) {
            return true;
        }
        
        // Report errors
        for (const auto& error : result.errors) {
            ErrorHandler::getInstance().report(error->what(), ErrorReportLevel::Warning);
        }
        
        // Check if we should degrade
        bool shouldDegrade = false;
        for (const auto& error : result.errors) {
            if (std::find(degradedErrors.begin(), degradedErrors.end(),
                         static_cast<ErrorCode>(0)) != degradedErrors.end()) {
                shouldDegrade = true;
                break;
            }
        }
        
        if (shouldDegrade) {
            logWarning("Configuration validation failed, using degraded mode");
            return false;
        }
        
        return false;
    }
};

// Brain error handling integration
class BrainErrorHandler {
public:
    // Safe brain initialization with error handling and recovery
    static std::unique_ptr<Brain> safeInitializeBrain(std::shared_ptr<Config> config,
                                                       const BrainCheckpoint& checkpoint,
                                                       const TransientErrorRecovery& retryManager,
                                                       bool createIfFailed = false) {
        try {
            // Try to initialize brain with retry
            auto initFunc = [&]() -> std::unique_ptr<Brain> {
                auto brain = std::make_unique<Brain>(config);
                
                if (!brain->initialize()) {
                    if (createIfFailed) {
                        // Try to recover from checkpoint or create minimal brain
                        auto checkpointedBrain = checkpoint.load("last");
                        if (checkpointedBrain) {
                            return checkpointedBrain;
                        }
                        
                        // Create minimal brain
                        brain = std::make_unique<Brain>(config);
                        // TODO: Implement minimal brain initialization
                    } else {
                        throw BrainInitException("Brain initialization failed");
                    }
                }
                
                // Save checkpoint
                checkpoint.save(*brain);
                
                return brain;
            };
            
            return retryManager.executeWithRetry("InitializeBrain", initFunc);
            
        } catch (const NLMException& e) {
            ErrorHandler::getInstance().reportException(e);
            return nullptr;
        }
    }
    
    // Safe brain simulation step with graceful degradation
    static bool safeBrainStep(Brain& brain, SimulationStep step, 
                            const std::vector<ErrorCode>& degradedErrors = {}) {
        try {
            brain.step(step);
            return true;
        } catch (const NLMException& e) {
            ErrorHandler::getInstance().reportException(e);
            
            bool shouldDegrade = std::find(degradedErrors.begin(), degradedErrors.end(),
                                          e.getErrorCode()) != degradedErrors.end();
            
            if (shouldDegrade) {
                logWarning("Brain simulation step failed, using degraded mode");
                // TODO: Implement degraded step (reduced functionality)
                return false;
            }
            
            return false;
        }
    }
};

// Neuron error handling integration
class NeuronErrorHandler {
public:
    // Safe neuron step with error handling and recovery
    static bool safeNeuronStep(Neuron& neuron, Timestamp currentTime, TimestepDuration dt,
                              const std::vector<ErrorCode>& degradedErrors = {}) {
        try {
            return neuron.stepLIF(currentTime, dt);
        } catch (const NLMException& e) {
            ErrorHandler::getInstance().reportException(e);
            
            bool shouldDegrade = std::find(degradedErrors.begin(), degradedErrors.end(),
                                          e.getErrorCode()) != degradedErrors.end();
            
            if (shouldDegrade) {
                logWarning("Neuron step failed, using degraded mode");
                // TODO: Implement degraded step (reset neuron state)
                neuron.reset();
                return false;
            }
            
            return false;
        }
    }
    
    // Safe synapse operation with error handling
    static bool safeSynapseOperation(Synapse& synapse, const std::string& operation,
                                    const std::vector<ErrorCode>& degradedErrors = {}) {
        try {
            if (operation == "updateWeight") {
                // TODO: Implement synapse weight update
                return true;
            } else if (operation == "reset") {
                synapse = Synapse(synapse.getId(), synapse.getSourceNeuron(),
                               synapse.getDestinationNeuron());
                return true;
            }
            return false;
        } catch (const NLMException& e) {
            ErrorHandler::getInstance().reportException(e);
            return false;
        }
    }
};

// **Error Handling Macros and Utilities**

// Configuration error recovery macros
#define NLM_CONFIG_RETRY(operation, ...) \
    nlm::ErrorHandling::ConfigErrorHandler::operation(__VA_ARGS__)

#define NLM_BRAIN_RETRY(operation, ...) \
    nlm::ErrorHandling::BrainErrorHandler::operation(__VA_ARGS__)

#define NLM_NEURON_RETRY(operation, ...) \
    nlm::ErrorHandling::NeuronErrorHandler::operation(__VA_ARGS__)

// Error context management
class ErrorContext {
public:
    ErrorContext(const std::string& operation) : operation(operation) {
        startTime = std::chrono::system_clock::now();
        ErrorHandler::getInstance().report("Starting operation: " + operation, 
                                         ErrorReportLevel::Debug, operation);
    }
    
    ~ErrorContext() {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        ErrorHandler::getInstance().report("Completed operation: " + operation + 
                                         " in " + std::to_string(duration.count()) + "ms",
                                         ErrorReportLevel::Debug, operation);
    }
    
    void addStep(const std::string& step) {
        steps.push_back(step);
    }
    
    void addError(const std::string& error) {
        errors.push_back(error);
    }
    
    std::string getContext() const {
        std::ostringstream oss;
        oss << "Operation: " << operation << "\n";
        oss << "Steps: ";
        for (size_t i = 0; i < steps.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << steps[i];
        }
        oss << "\nErrors: ";
        for (size_t i = 0; i < errors.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << errors[i];
        }
        return oss.str();
    }
    
private:
    std::string operation;
    std::chrono::system_clock::time_point startTime;
    std::vector<std::string> steps;
    std::vector<std::string> errors;
};

// Global error recovery manager
class GlobalErrorRecovery {
private:
    std::unique_ptr<TransientErrorRecovery> retryManager;
    std::unique_ptr<ConfigCheckpoint> configCheckpoint;
    std::unique_ptr<BrainCheckpoint> brainCheckpoint;
    std::unique_ptr<GracefulDegradation> degradationManager;
    
public:
    GlobalErrorRecovery() {
        // Initialize recovery components
        TransientErrorRecovery::RetryConfig retryConfig;
        retryConfig.maxAttempts = 3;
        retryConfig.initialDelay = std::chrono::milliseconds(100);
        retryConfig.backoffMultiplier = 2.0;
        
        std::vector<ErrorCode> retryableErrors = {
            ErrorCode::MemoryAllocationFailure,
            ErrorCode::SimulationRuntimeError,
            ErrorCode::NeuronStateError,
            ErrorCode::SynapseConnectionError
        };
        retryConfig.retryableErrors = retryableErrors;
        
        retryManager = std::make_unique<TransientErrorRecovery>(retryConfig);
        configCheckpoint = std::make_unique<ConfigCheckpoint>();
        brainCheckpoint = std::make_unique<BrainCheckpoint>();
        degradationManager = std::make_unique<GracefulDegradation>();
    }
    
    // Configure global error handling
    void configure(ErrorReportLevel level, ExceptionStrategy strategy) {
        nlm::ErrorHandler::initialize(level, strategy);
    }
    
    // Get recovery components
    TransientErrorRecovery& getRetryManager() { return *retryManager; }
    ConfigCheckpoint& getConfigCheckpoint() { return *configCheckpoint; }
    BrainCheckpoint& getBrainCheckpoint() { return *brainCheckpoint; }
    GracefulDegradation& getDegradationManager() { return *degradationManager; }
    
    // Execute safe operation with all recovery mechanisms
    template<typename Func, typename... Args>
    auto executeSafely(const std::string& operation, const Func& func, Args&&... args) 
    -> typename std::result_of<Func(Args...)>::type {
        ErrorContext context(operation);
        context.addStep("Starting safe execution");
        
        try {
            // Execute with retry
            auto result = retryManager->executeWithRetry(operation, func, std::forward<Args>(args)...);
            context.addStep("Retry successful");
            return result;
        } catch (const NLMException& e) {
            context.addError(e.getMessage());
            ErrorHandler::getInstance().reportException(e);
            
            // Try graceful degradation
            auto degradedResult = degradationManager->wrap(operation, [&]() { return func(std::forward<Args>(args)...); });
            if (degradedResult.isDegradedValue()) {
                context.addStep("Degradation applied");
                return degradedResult.get();
            }
            
            throw; // Re-throw if degradation not possible
        }
    }
};

// Global error recovery instance
inline GlobalErrorRecovery& getGlobalErrorRecovery() {
    static GlobalErrorRecovery recovery;
    return recovery;
}

} // namespace ErrorHandling
} // namespace nlm

// Enhanced Error Handling and Validation System
// 
// This system provides comprehensive error handling and validation for the NLM project:
// - Exception hierarchy for different types of errors
// - Validation for configuration parameters
// - Resource management and cleanup
// - File I/O error handling
// - Simulation parameter validation
// - Memory management improvements
// - Comprehensive logging and error reporting

#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <functional>
#include <sstream>
#include <iomanip>

namespace nlm {

// ============================================================================
// Exception Hierarchy
// ============================================================================

// Base exception class for all NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message), timestamp(std::chrono::high_resolution_clock::now()) {}
    
    const std::chrono::high_resolution_clock::time_point& getTimestamp() const { return timestamp; }
    virtual std::string getTypeName() const { return "NLMException"; }
    
protected:
    std::chrono::high_resolution_clock::time_point timestamp;
};

// Configuration-related errors
class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message) : NLMException("Config Error: " + message) {}
    std::string getTypeName() const override { return "ConfigError"; }
};

class InvalidConfigValue : public ConfigError {
public:
    InvalidConfigValue(const std::string& key, const std::string& expectedType)
        : ConfigError("Invalid value for key '" + key + "': expected '" + expectedType + "'"),
          key(key), expectedType(expectedType) {}
    
    std::string getKey() const { return key; }
    std::string getExpectedType() const { return expectedType; }
    std::string getTypeName() const override { return "InvalidConfigValue"; }
    
private:
    std::string key;
    std::string expectedType;
};

class MissingRequiredConfig : public ConfigError {
public:
    explicit MissingRequiredConfig(const std::string& key)
        : ConfigError("Missing required configuration key: '" + key + "'"),
          key(key) {}
    
    std::string getKey() const { return key; }
    std::string getTypeName() const override { return "MissingRequiredConfig"; }
    
private:
    std::string key;
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& message) : ConfigError("Configuration validation failed: " + message) {}
    std::string getTypeName() const override { return "ConfigValidationError"; }
};

// Brain-related errors
class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& message) : NLMException("Brain Error: " + message) {}
    std::string getTypeName() const override { return "BrainError"; }
};

class BrainInitializationError : public BrainError {
public:
    explicit BrainInitializationError(const std::string& reason) : BrainError("Brain initialization failed: " + reason) {}
    std::string getTypeName() const override { return "BrainInitializationError"; }
};

class NeuronError : public BrainError {
public:
    explicit NeuronError(const std::string& message) : BrainError("Neuron Error: " + message) {}
    std::string getTypeName() const override { return "NeuronError"; }
};

class SynapseError : public BrainError {
public:
    explicit SynapseError(const std::string& message) : BrainError("Synapse Error: " + message) {}
    std::string getTypeName() const override { return "SynapseError"; }
};

// Memory system errors
class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& message) : NLMException("Memory Error: " + message) {}
    std::string getTypeName() const override { return "MemoryError"; }
};

class WorkingMemoryError : public MemoryError {
public:
    explicit WorkingMemoryError(const std::string& message) : MemoryError("Working Memory Error: " + message) {}
    std::string getTypeName() const override { return "WorkingMemoryError"; }
};

// Simulation-related errors
class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message) : NLMException("Simulation Error: " + message) {}
    std::string getTypeName() const override { return "SimulationError"; }
};

class StepExecutionError : public SimulationError {
public:
    explicit StepExecutionError(const std::string& reason, SimulationStep step)
        : SimulationError("Step execution failed at step " + std::to_string(step) + ": " + reason),
          step(step) {}
    
    SimulationStep getStep() const { return step; }
    std::string getTypeName() const override { return "StepExecutionError"; }
    
private:
    SimulationStep step;
};

// File I/O errors
class IOError : public NLMException {
public:
    explicit IOError(const std::string& message) : NLMException("IO Error: " + message) {}
    std::string getTypeName() const override { return "IOError"; }
};

class FileNotFoundError : public IOError {
public:
    explicit FileNotFoundError(const std::string& filepath) : IOError("File not found: '" + filepath + "'"),
                                                              filepath(filepath) {}
    
    std::string getFilepath() const { return filepath; }
    std::string getTypeName() const override { return "FileNotFoundError"; }
    
private:
    std::string filepath;
};

class FilePermissionError : public IOError {
public:
    explicit FilePermissionError(const std::string& filepath) : IOError("Permission denied accessing file: '" + filepath + "'"),
                                                                 filepath(filepath) {}
    
    std::string getFilepath() const { return filepath; }
    std::string getTypeName() const override { return "FilePermissionError"; }
    
private:
    std::string filepath;
};

// Resource management errors
class ResourceError : public NLMException {
public:
    explicit ResourceError(const std::string& message) : NLMException("Resource Error: " + message) {}
    std::string getTypeName() const override { return "ResourceError"; }
};

class MemoryAllocationError : public ResourceError {
public:
    explicit MemoryAllocationError(size_t size, const std::string& allocationType)
        : ResourceError("Failed to allocate " + std::to_string(size) + " bytes for " + allocationType),
          size(size), allocationType(allocationType) {}
    
    size_t getSize() const { return size; }
    std::string getAllocationType() const { return allocationType; }
    std::string getTypeName() const override { return "MemoryAllocationError"; }
    
private:
    size_t size;
    std::string allocationType;
};

// Validation and parameter errors
class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& message) : NLMException("Validation Error: " + message) {}
    std::string getTypeName() const override { return "ValidationError"; }
};

class InvalidParameterError : public ValidationError {
public:
    InvalidParameterError(const std::string& paramName, const std::string& expectedRange)
        : ValidationError("Invalid parameter '" + paramName + "': expected range " + expectedRange),
          paramName(paramName), expectedRange(expectedRange) {}
    
    std::string getParameterName() const { return paramName; }
    std::string getExpectedRange() const { return expectedRange; }
    std::string getTypeName() const override { return "InvalidParameterError"; }
    
private:
    std::string paramName;
    std::string expectedRange;
};

// ============================================================================
// Error Reporting and Logging
// ============================================================================

class ErrorReporter {
public:
    static ErrorReporter& getInstance() {
        static ErrorReporter instance;
        return instance;
    }
    
    void setErrorReportingLevel(ErrorLevel level) {
        std::lock_guard<std::mutex> lock(mutex);
        reportingLevel = level;
    }
    
    ErrorLevel getErrorReportingLevel() const {
        std::lock_guard<std::mutex> lock(mutex);
        return reportingLevel;
    }
    
    void reportError(const NLMException& error) {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (shouldReportError(error)) {
            logError(error);
            if (errorReportingCallback) {
                errorReportingCallback(error);
            }
        }
    }
    
    void setErrorCallback(const std::function<void(const NLMException&)>& callback) {
        std::lock_guard<std::mutex> lock(mutex);
        errorReportingCallback = callback;
    }
    
    std::vector<std::string> getRecentErrors(size_t count = 10) const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::string> recentErrors;
        
        auto it = errorHistory.rbegin();
        for (size_t i = 0; i < std::min(count, errorHistory.size()); ++i, ++it) {
            recentErrors.push_back(*it);
        }
        
        return recentErrors;
    }
    
    void clearErrorHistory() {
        std::lock_guard<std::mutex> lock(mutex);
        errorHistory.clear();
    }
    
    size_t getErrorCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return errorHistory.size();
    }
    
private:
    ErrorReporter() : reportingLevel(ErrorLevel::Warning) {}
    
    bool shouldReportError(const NLMException& error) const {
        switch (reportingLevel) {
            case ErrorLevel::None:
                return false;
            case ErrorLevel::Critical:
                return isCriticalError(error);
            case ErrorLevel::Warning:
                return isCriticalError(error) || isWarningError(error);
            case ErrorLevel::Info:
                return true;
            default:
                return true;
        }
    }
    
    bool isCriticalError(const NLMException& error) const {
        const std::string& typeName = error.getTypeName();
        return typeName.find("Error") != std::string::npos || 
               typeName.find("Critical") != std::string::npos;
    }
    
    bool isWarningError(const NLMException& error) const {
        return typeName.find("Warning") != std::string::npos;
    }
    
    void logError(const NLMException& error) {
        std::string timestamp = formatTimestamp(error.getTimestamp());
        std::string typeName = error.getTypeName();
        
        // Log to console
        std::cerr << "[" << timestamp << "] " << typeName << ": " << error.what() << std::endl;
        
        // Add to error history
        errorHistory.push_back("[" + timestamp + "] " + typeName + ": " + error.what());
        
        // Keep error history limited
        if (errorHistory.size() > 1000) {
            errorHistory.erase(errorHistory.begin(), errorHistory.begin() + 100);
        }
    }
    
    std::string formatTimestamp(const std::chrono::high_resolution_clock::time_point& tp) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - tp);
        
        std::stringstream ss;
        ss << "+" << duration.count() << "ms";
        return ss.str();
    }
    
    enum class ErrorLevel {
        None,
        Critical,
        Warning,
        Info
    };
    
    ErrorLevel reportingLevel;
    std::function<void(const NLMException&)> errorReportingCallback;
    mutable std::mutex mutex;
    std::vector<std::string> errorHistory;
    
    ErrorReporter(const ErrorReporter&) = delete;
    ErrorReporter& operator=(const ErrorReporter&) = delete;
};

// ============================================================================
// Resource Management
// ============================================================================

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    template<typename T>
    std::shared_ptr<T> allocateResource(const std::string& name, size_t size) {
        try {
            std::shared_ptr<T> resource(new T(), [this, name](T* ptr) {
                deallocateResource(ptr, name);
            });
            
            trackResource(name, size, resource);
            return resource;
        } catch (const std::bad_alloc&) {
            throw MemoryAllocationError(size, name);
        }
    }
    
    void deallocateResource(void* ptr, const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = allocatedResources.find(ptr);
        if (it != allocatedResources.end()) {
            totalAllocatedMemory -= it->second.size;
            delete it->first;
            allocatedResources.erase(it);
        }
    }
    
    size_t getTotalAllocatedMemory() const {
        std::lock_guard<std::mutex> lock(mutex);
        return totalAllocatedMemory;
    }
    
    void cleanupUnusedResources() {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto it = allocatedResources.begin(); it != allocatedResources.end(); ) {
            if (std::chrono::high_resolution_clock::now() - it->second.lastAccessed > std::chrono::minutes(5)) {
                totalAllocatedMemory -= it->second.size;
                delete it->first;
                it = allocatedResources.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    std::map<std::string, size_t> getResourceStats() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::map<std::string, size_t> stats;
        
        for (const auto& pair : allocatedResources) {
            stats[pair.second.name] += pair.second.size;
        }
        
        return stats;
    }
    
private:
    ResourceManager() : totalAllocatedMemory(0) {}
    
    struct ResourceInfo {
        std::string name;
        size_t size;
        std::chrono::high_resolution_clock::time_point lastAccessed;
    };
    
    void trackResource(void* ptr, const std::string& name, size_t size) {
        std::lock_guard<std::mutex> lock(mutex);
        
        ResourceInfo info{name, size, std::chrono::high_resolution_clock::now()};
        allocatedResources[ptr] = info;
        totalAllocatedMemory += size;
    }
    
    std::map<void*, ResourceInfo> allocatedResources;
    size_t totalAllocatedMemory;
    mutable std::mutex mutex;
    
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
};

// ============================================================================
// Configuration Validation
// ============================================================================

class ConfigValidator {
public:
    static bool validateConfig(const Config& config) {
        std::vector<std::string> errors;
        
        // Validate required configuration keys
        validateRequiredKeys(config, errors);
        
        // Validate parameter ranges
        validateParameterRanges(config, errors);
        
        // Validate configuration consistency
        validateConfigurationConsistency(config, errors);
        
        // Report errors if any
        if (!errors.empty()) {
            for (const auto& error : errors) {
                throw ConfigValidationError(error);
            }
        }
        
        return true;
    }
    
    static std::vector<std::string> getConfigurationSuggestions(const Config& config) {
        std::vector<std::string> suggestions;
        
        auto neuronCount = config.getOr<int64_t>("brain.neuron_count", 0);
        auto connectionProbability = config.getOr<float>("brain.connection_probability", 0.0f);
        
        if (neuronCount > 50000 && connectionProbability < 0.05f) {
            suggestions.push_back("For large neuron counts (>50k), consider increasing connection probability for better connectivity");
        }
        
        if (neuronCount > 100000 && !config.has("performance.enable_multithreading")) {
            suggestions.push_back("For very large neuron counts (>100k), consider enabling multithreading for better performance");
        }
        
        return suggestions;
    }
    
private:
    static void validateRequiredKeys(const Config& config, std::vector<std::string>& errors) {
        const std::vector<std::string> requiredKeys = {
            "brain.neuron_count",
            "brain.connection_probability",
            "simulation.timestep"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                errors.push_back("Missing required configuration key: '" + key + "'");
            }
        }
    }
    
    static void validateParameterRanges(const Config& config, std::vector<std::string>& errors) {
        auto neuronCount = config.getOr<int64_t>("brain.neuron_count", -1);
        if (neuronCount < 1) {
            errors.push_back("brain.neuron_count must be greater than 0");
        } else if (neuronCount > 1000000) {
            errors.push_back("brain.neuron_count is too large (>1,000,000), may cause memory issues");
        }
        
        auto connectionProbability = config.getOr<float>("brain.connection_probability", -1.0f);
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
        }
        
        auto timestep = config.getOr<double>("simulation.timestep", -1.0);
        if (timestep <= 0.0 || timestep > 1.0) {
            errors.push_back("simulation.timestep must be between 0.0 and 1.0 seconds");
        }
    }
    
    static void validateConfigurationConsistency(const Config& config, std::vector<std::string>& errors) {
        auto neuronCount = config.getOr<int64_t>("brain.neuron_count", 0);
        auto regionCount = config.getOr<int64_t>("brain.region_count", 1);
        
        if (regionCount <= 0) {
            errors.push_back("brain.region_count must be greater than 0");
        }
        
        if (neuronCount / regionCount < 100) {
            errors.push_back("brain.region_count is too large for the neuron count. Consider reducing regions or increasing neurons");
        }
    }
};

// ============================================================================
// Simulation Parameter Validation
// ============================================================================

class SimulationValidator {
public:
    static bool validateSimulationParameters(const Config& config) {
        std::vector<std::string> errors;
        
        // Validate neuron parameters
        validateNeuronParameters(config, errors);
        
        // Validate synapse parameters
        validateSynapseParameters(config, errors);
        
        // Validate simulation parameters
        validateSimulationParameters(config, errors);
        
        // Validate memory system parameters
        validateMemoryParameters(config, errors);
        
        // Validate neuromodulation parameters
        validateNeuromodulationParameters(config, errors);
        
        // Report errors if any
        if (!errors.empty()) {
            for (const auto& error : errors) {
                throw SimulationError(error);
            }
        }
        
        return true;
    }
    
    static SimulationStep clampStep(SimulationStep step, SimulationStep maxStep = 1000000000) {
        if (step < 0) {
            throw InvalidParameterError("SimulationStep", "0 to " + std::to_string(maxStep));
        }
        return std::min(step, maxStep);
    }
    
    static double clampTime(double time, double maxTime = 1000000.0) {
        if (time < 0.0) {
            throw InvalidParameterError("Time", "0.0 to " + std::to_string(maxTime));
        }
        return std::min(time, maxTime);
    }
    
private:
    static void validateNeuronParameters(const Config& config, std::vector<std::string>& errors) {
        auto neuronCount = config.getOr<int64_t>("brain.neuron_count", 0);
        if (neuronCount <= 0) {
            errors.push_back("brain.neuron_count must be positive");
        }
        
        auto threshold = config.getOr<double>("brain.v_thresh", 0.0);
        auto restPotential = config.getOr<double>("brain.v_rest", 0.0);
        auto resetPotential = config.getOr<double>("brain.v_reset", 0.0);
        
        if (threshold <= restPotential) {
            errors.push_back("brain.v_thresh must be greater than brain.v_rest");
        }
        
        if (resetPotential >= threshold) {
            errors.push_back("brain.v_reset must be less than brain.v_thresh");
        }
        
        auto tauMem = config.getOr<double>("brain.tau_mem", 0.0);
        auto tauRef = config.getOr<double>("brain.tau_ref", 0.0);
        
        if (tauMem <= 0.0) {
            errors.push_back("brain.tau_mem must be positive");
        }
        
        if (tauRef <= 0.0) {
            errors.push_back("brain.tau_ref must be positive");
        }
        
        if (tauMem <= tauRef) {
            errors.push_back("brain.tau_mem must be greater than brain.tau_ref");
        }
    }
    
    static void validateSynapseParameters(const Config& config, std::vector<std::string>& errors) {
        auto connectionProbability = config.getOr<float>("brain.connection_probability", -1.0f);
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
        }
        
        auto initialWeight = config.getOr<float>("brain.initial_weight_mean", 0.0f);
        auto weightStd = config.getOr<float>("brain.initial_weight_std", 0.0f);
        
        if (weightStd < 0.0f) {
            errors.push_back("brain.initial_weight_std must be non-negative");
        }
        
        if (initialWeight < 0.0f) {
            errors.push_back("brain.initial_weight_mean must be non-negative");
        }
    }
    
    static void validateSimulationParameters(const Config& config, std::vector<std::string>& errors) {
        auto timestep = config.getOr<double>("simulation.timestep", 0.0);
        if (timestep <= 0.0) {
            errors.push_back("simulation.timestep must be positive");
        } else if (timestep > 1.0) {
            errors.push_back("simulation.timestep is too large (>1.0s), may cause numerical instability");
        }
        
        auto maxSteps = config.getOr<uint64_t>("simulation.max_steps", 0);
        if (maxSteps == 0) {
            errors.push_back("simulation.max_steps must be specified");
        } else if (maxSteps > 1000000000) {
            errors.push_back("simulation.max_steps is too large (>1,000,000,000), may cause overflow");
        }
    }
    
    static void validateMemoryParameters(const Config& config, std::vector<std::string>& errors) {
        auto wmCapacity = config.getOr<size_t>("memory.working_memory_capacity", 0);
        if (wmCapacity == 0) {
            errors.push_back("memory.working_memory_capacity must be specified");
        }
        
        auto emMaxEpisodes = config.getOr<size_t>("memory.episodic_memory_max_episodes", 0);
        if (emMaxEpisodes == 0) {
            errors.push_back("memory.episodic_memory_max_episodes must be specified");
        }
    }
    
    static void validateNeuromodulationParameters(const Config& config, std::vector<std::string>& errors) {
        auto dopamineBaseline = config.getOr<double>("neuromod.dopamine_baseline", -1.0);
        if (dopamineBaseline < 0.0) {
            errors.push_back("neuromod.dopamine_baseline must be non-negative");
        }
        
        auto curiosityThreshold = config.getOr<double>("neuromod.curiosity_threshold", -1.0);
        if (curiosityThreshold < 0.0 || curiosityThreshold > 1.0) {
            errors.push_back("neuromod.curiosity_threshold must be between 0.0 and 1.0");
        }
    }
};

// ============================================================================
// Safe Resource Management Macros
// ============================================================================

#define SAFE_NEW(type, ...) try { new type(__VA_ARGS__) } catch (const std::bad_alloc&) { throw MemoryAllocationError(sizeof(type), #type); }

#define SAFE_DELETE(ptr) { if (ptr) { delete ptr; } }

#define SAFE_DELETE_ARRAY(ptr, size) { if (ptr) { delete[] ptr; } }

#define SAFE_RELEASE(resource) { if (resource) { resource->release(); } }

#define SAFE_CHECK(condition, errorMessage) { if (!(condition)) { throw NLMException(errorMessage); } }

#define SAFE_CHECK_RETURN(condition, errorCode) { if (!(condition)) { return errorCode; } }

// ============================================================================
// Error Handling Utilities
// ============================================================================

class ErrorHandling {
public:
    template<typename Func>
    static auto safeExecute(const Func& func) -> decltype(func()) {
        try {
            return func();
        } catch (const NLMException&) {
            throw;
        } catch (const std::exception& e) {
            throw NLMException(std::string("Unexpected error: ") + e.what());
        }
    }
    
    template<typename Func>
    static auto safeExecuteWithDefault(const Func& func, auto defaultValue) -> decltype(func()) {
        try {
            return func();
        } catch (const NLMException&) {
            ErrorReporter::getInstance().reportError(NLMException("Operation failed, using default value"));
            return defaultValue;
        }
    }
    
    static std::string formatErrorMessage(const std::string& operation, const std::exception& error) {
        std::stringstream ss;
        ss << "Operation '" << operation << "' failed: " << error.what();
        return ss.str();
    }
    
    static void logException(const std::exception& error, const std::string& context = "") {
        ErrorReporter::getInstance().reportError(NLMException(
            context.empty() ? error.what() : context + ": " + error.what()
        ));
    }
};

// ============================================================================
// Global Error Handling Setup
// ============================================================================

inline void setupErrorHandling() {
    ErrorReporter::getInstance().setErrorCallback([](const NLMException& error) {
        // Global error handling callback
        std::cerr << "GLOBAL ERROR HANDLER: " << error.getTypeName() << ": " << error.what() << std::endl;
    });
}

inline void enableErrorReporting(bool enable) {
    ErrorReporter::getInstance().setErrorReportingLevel(
        enable ? ErrorReporter::ErrorLevel::Info : ErrorReporter::ErrorLevel::None
    );
}

// ============================================================================
// Exception Translation for Python Bindings
// ============================================================================

inline void setupPythonExceptionTranslation() {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            py::pybind11::translate_exception(&e);
        } catch (const std::exception& e) {
            py::pybind11::translate_exception(&e);
        }
    });
}

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H

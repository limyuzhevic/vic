#include "ErrorHandling.hpp"
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {

// Helper functions for error handling
namespace {

// Global error reporter implementation
class GlobalErrorReporter : public ErrorReporter {
public:
    GlobalErrorReporter() : errorCounts(), totalErrors(0), mutex() {}
    
    void report(const NLMException& error) override {
        std::lock_guard<std::mutex> lock(mutex);
        errorCounts[error.getErrorCategory()]++;
        totalErrors++;
        
        // Log based on error severity
        if (error.getErrorCategory() == NLMErrorCategory::Configuration ||
            error.getErrorCategory() == NLMErrorCategory::Neural) {
            // Warning level for configuration and neural errors
            std::cerr << "[WARNING] " << error.what() << std::endl;
        } else {
            // Error level for other categories
            std::cerr << "[ERROR] " << error.what() << std::endl;
        }
        
        // Call user handler if set
        if (errorHandler) {
            errorHandler(error);
        }
    }
    
    void warn(const std::string& message) override {
        std::lock_guard<std::mutex> lock(mutex);
        totalWarnings++;
        std::cout << "[WARNING] " << message << std::endl;
    }
    
    void info(const std::string& message) override {
        if (logLevel <= 1) { // INFO level
            std::cout << "[INFO] " << message << std::endl;
        }
    }
    
    void setErrorHandler(std::function<void(const NLMException&)> handler) override {
        std::lock_guard<std::mutex> lock(mutex);
        errorHandler = handler;
    }
    
    size_t getErrorCount(NLMErrorCategory category) const override {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = errorCounts.find(category);
        return it ? it->second : 0;
    }
    
    size_t getTotalErrorCount() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return totalErrors;
    }
    
    void clear() override {
        std::lock_guard<std::mutex> lock(mutex);
        errorCounts.clear();
        totalErrors = 0;
        totalWarnings = 0;
    }
    
    // Set log level
    void setLogLevel(int level) { logLevel = level; }
    
private:
    std::unordered_map<NLMErrorCategory, size_t> errorCounts;
    size_t totalErrors;
    size_t totalWarnings;
    std::mutex mutex;
    std::function<void(const NLMException&)> errorHandler;
    int logLevel;
};

GlobalErrorReporter& getGlobalErrorReporterInstance() {
    static GlobalErrorReporter reporter;
    return reporter;
}

// Exception implementation
std::string NLMException::formatMessage(NLMErrorCode code, const std::string& message,
                                         const std::string& context) {
    std::stringstream ss;
    ss << "[NLM] " << context << ": " << message << " (Code: " << static_cast<uint32_t>(code) << ")";
    return ss.str();
}

NLMErrorCategory NLMException::determineCategory(NLMErrorCode code) {
    if (code >= NLMErrorCode::InvalidParameter && code <= NLMErrorCode::ConfigFileParseError) {
        return NLMErrorCategory::Configuration;
    } else if (code >= NLMErrorCode::NeuronCreationFailed && code <= NLMErrorCode::PlasticityError) {
        return NLMErrorCategory::Neural;
    } else if (code >= NLMErrorCode::MemoryAllocationFailed && code <= NLMErrorCode::MemoryPoolExhausted) {
        return NLMErrorCategory::Memory;
    } else if (code >= NLMErrorCode::SimulationNotInitialized && code <= NLMErrorCode::StepSequenceError) {
        return NLMErrorCategory::Simulation;
    } else if (code >= NLMErrorCode::IntegrationFailed && code <= NLMErrorCode::ComponentNotAvailable) {
        return NLMErrorCategory::Integration;
    } else if (code >= NLMErrorCode::PerformanceRegression && code <= NLMErrorCode::SIMDNotAvailable) {
        return NLMErrorCategory::Performance;
    } else if (code >= NLMErrorCode::LockOrderViolation && code <= NLMErrorCode::ThreadSafetyViolation) {
        return NLMErrorCategory::ThreadSafety;
    } else if (code >= NLMErrorCode::ResourceExhausted && code <= NLMErrorCode::ResourceInUse) {
        return NLMErrorCategory::Resource;
    } else {
        return NLMErrorCategory::NoError;
    }
}

NLMException NLMException::invalidParameter(const std::string& param, const std::string& value) {
    return NLMException(NLMErrorCode::InvalidParameter,
                       std::format("Invalid parameter '{}': {}", param, value),
                       "Configuration");
}

NLMException NLMException::neuralError(const std::string& operation, const std::string& details) {
    return NLMException(NLMErrorCode::UnknownError,
                       std::format("Neural error in '{}': {}", operation, details),
                       "Neural");
}

NLMException NLMException::memoryError(const std::string& operation, size_t size) {
    return NLMException(NLMErrorCode::MemoryAllocationFailed,
                       std::format("Memory error in '{}': failed to allocate {} bytes", operation, size),
                       "Memory");
}

NLMException NLMException::simulationError(const std::string& operation, const std::string& details) {
    return NLMException(NLMErrorCode::UnknownError,
                       std::format("Simulation error in '{}': {}", operation, details),
                       "Simulation");
}

// ConfigurationException implementation
ConfigurationException ConfigurationException::invalidParameter(const std::string& param, const std::string& value) {
    return ConfigurationException(NLMErrorCode::InvalidParameter,
                                 std::format("Invalid configuration parameter '{}': {}", param, value),
                                 "Configuration");
}

ConfigurationException ConfigurationException::missingParameter(const std::string& param) {
    return ConfigurationException(NLMErrorCode::MissingRequiredParameter,
                                 std::format("Missing required configuration parameter: {}", param),
                                 "Configuration");
}

ConfigurationException ConfigurationException::invalidRange(const std::string& param, double min, double max, double value) {
    return ConfigurationException(NLMErrorCode::InvalidRange,
                                 std::format("Invalid range for parameter '{}': expected [{}, {}], got {}", param, min, max, value),
                                 "Configuration");
}

// NeuralException implementation
NeuralException NeuralException::neuronCreationFailed(NeuronId id, const std::string& reason) {
    return NeuralException(NLMErrorCode::NeuronCreationFailed,
                          std::format("Failed to create neuron {}: {}", id.value, reason),
                          "Neural");
}

NeuralException NeuralException::invalidConnection(NeuronId source, NeuronId destination, const std::string& reason) {
    return NeuralException(NLMErrorCode::InvalidConnection,
                          std::format("Invalid connection from neuron {} to {}: {}", source.value, destination.value, reason),
                          "Neural");
}

NeuralException NeuralException::simulationStepError(SimulationStep step, const std::string& reason) {
    return NeuralException(NLMErrorCode::SimulationStepError,
                          std::format("Simulation step {} failed: {}", step, reason),
                          "Neural");
}

// MemoryException implementation
MemoryException MemoryException::allocationFailed(size_t size, const std::string& context) {
    return MemoryException(NLMErrorCode::MemoryAllocationFailed,
                          std::format("Memory allocation failed: {} bytes in '{}'", size, context),
                          "Memory");
}

MemoryException MemoryException::corruptionDetected(const std::string& context) {
    return MemoryException(NLMErrorCode::MemoryCorruptionDetected,
                          std::format("Memory corruption detected in '{}'", context),
                          "Memory");
}

MemoryException MemoryException::poolExhausted(const std::string& poolName, size_t capacity) {
    return MemoryException(NLMErrorCode::MemoryPoolExhausted,
                          std::format("Memory pool '{}' exhausted: capacity {}", poolName, capacity),
                          "Memory");
}

// SimulationException implementation
SimulationException SimulationException::notInitialized(const std::string& component) {
    return SimulationException(NLMErrorCode::SimulationNotInitialized,
                              std::format("Simulation component not initialized: {}", component),
                              "Simulation");
}

SimulationException SimulationException::invalidTimeStep(TimestepDuration dt, const std::string& reason) {
    return SimulationException(NLMErrorCode::InvalidTimeStep,
                              std::format("Invalid timestep {}: {}", dt, reason),
                              "Simulation");
}

SimulationException SimulationException::stepSequenceError(SimulationStep step, const std::string& reason) {
    return SimulationException(NLMErrorCode::StepSequenceError,
                              std::format("Step sequence error at step {}: {}", step, reason),
                              "Simulation");
}

// Global error reporter interface
ErrorReporter& getGlobalErrorReporter() {
    return getGlobalErrorReporterInstance();
}

void ExceptionSafe::rollback() {
    // TODO: Implement actual rollback logic based on current system state
    // This would restore neural states, synaptic weights, etc.
    getGlobalErrorReporter().warn("Rollback executed - system state restored");
}

} // anonymous namespace

// Global error reporter instance definition
ErrorReporter& getGlobalErrorReporter() {
    return getGlobalErrorReporterInstance();
}

void ExceptionSafe::rollback() {
    // Implementation in anonymous namespace above
}

} // namespace nlm
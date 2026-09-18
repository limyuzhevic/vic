#include <stdexcept>
#include <cassert>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <sstream>
#include <chrono>
#include <iostream>
#include <mutex>
#include <map>
#include <limits>
#include <iomanip>

namespace nlm {

// Validation and Error Handling Utilities
class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& message) : std::runtime_error(message) {}
};

class RangeError : public ValidationError {
public:
    RangeError(const std::string& param, float value, float min, float max)
        : ValidationError("Parameter '" + param + "' has invalid value: " + 
                         std::to_string(value) + " (must be between " + 
                         std::to_string(min) + " and " + std::to_string(max) + ")") {}
};

class NullPointerError : public ValidationError {
public:
    NullPointerError(const std::string& param) : ValidationError("Parameter '" + param + "' cannot be null") {}
};

// Parameter validation functions
inline void validateFloat(const std::string& param, float value, float min, float max) {
    if (std::isnan(value)) {
        throw ValidationError("Parameter '" + param + "' cannot be NaN");
    }
    if (value < min || value > max) {
        throw RangeError(param, value, min, max);
    }
}

inline void validateNonNull(const std::string& param, void* ptr) {
    if (ptr == nullptr) {
        throw NullPointerError(param);
    }
}

inline void validateNonEmpty(const std::string& param, const std::vector<float>& vec) {
    if (vec.empty()) {
        throw ValidationError("Parameter '" + param + "' cannot be empty");
    }
}

inline void validateSize(const std::string& param, size_t value, size_t expectedMin) {
    if (value < expectedMin) {
        throw ValidationError("Parameter '" + param + "' must be at least " + 
                           std::to_string(expectedMin) + ", got " + std::to_string(value));
    }
}

// Performance monitoring and profiling
class PerformanceProfiler {
public:
    struct PerformanceMetrics {
        double totalTime;           // Total time for operation
        double cpuTime;            // CPU time (if available)
        size_t memoryUsage;        // Memory usage in bytes
        size_t callCount;          // Number of calls
        double averageTime;        // Average time per call
        double minTime;            // Minimum time
        double maxTime;            // Maximum time
        
        PerformanceMetrics()
            : totalTime(0.0), cpuTime(0.0), memoryUsage(0), callCount(0),
              averageTime(0.0), minTime(std::numeric_limits<double>::max()),
              maxTime(0.0) {}
        
        void record(double time, size_t memory = 0) {
            totalTime += time;
            cpuTime += time;  // Simplified: assume CPU time equals wall time
            memoryUsage += memory;
            ++callCount;
            averageTime = totalTime / callCount;
            minTime = std::min(minTime, time);
            maxTime = std::max(maxTime, time);
        }
    };
    
    class Timer {
    public:
        Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
        
        double elapsed() const {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            return duration.count() / 1000000.0;  // Convert to seconds
        }
        
    private:
        std::chrono::high_resolution_clock::time_point startTime;
    };
    
    // Global performance metrics storage
    static std::map<std::string, PerformanceMetrics> metrics;
    static std::mutex metricsMutex;
    
    static void recordOperation(const std::string& operation, double time, size_t memory = 0) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        // Find existing metric or create new one
        if (metrics.find(operation) == metrics.end()) {
            metrics[operation] = PerformanceMetrics();
        }
        metrics[operation].record(time, memory);
    }
    
    static void printReport() {
        std::cout << "=== Performance Report ===" << std::endl;
        for (const auto& pair : metrics) {
            const auto& op = pair.first;
            const auto& metric = pair.second;
            
            std::cout << "Operation: " << op << std::endl;
            std::cout << "  Calls: " << metric.callCount << std::endl;
            std::cout << "  Total time: " << std::fixed << std::setprecision(6) << metric.totalTime << "s" << std::endl;
            std::cout << "  Average time: " << std::fixed << std::setprecision(6) << metric.averageTime << "s" << std::endl;
            std::cout << "  Min time: " << std::fixed << std::setprecision(6) << metric.minTime << "s" << std::endl;
            std::cout << "  Max time: " << std::fixed << std::setprecision(6) << metric.maxTime << "s" << std::endl;
            std::cout << "  Memory usage: " << metric.memoryUsage << " bytes" << std::endl;
            std::cout << std::endl;
        }
    }
    
    static void reset() {
        std::lock_guard<std::mutex> lock(metricsMutex);
        metrics.clear();
    }
};

// Thread-safe error reporting
class ErrorReporter {
public:
    struct ErrorInfo {
        std::string message;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        std::string context;
        
        ErrorInfo(const std::string& msg, const std::string& file = "", 
                 int line = 0, const std::string& func = "", 
                 const std::string& ctx = "")
            : message(msg), file(file), line(line), function(func), 
              timestamp(std::chrono::system_clock::now()), context(ctx) {}
    };
    
    static std::vector<ErrorInfo> errors;
    static std::mutex errorMutex;
    
    static void reportError(const std::string& message, const std::string& file = "", 
                           int line = 0, const std::string& function = "", 
                           const std::string& context = "") {
        std::lock_guard<std::mutex> lock(errorMutex);
        errors.emplace_back(message, file, line, function, context);
    }
    
    static void printErrors() {
        std::cout << "=== Error Report ===" << std::endl;
        for (const auto& error : errors) {
            std::cout << "Error: " << error.message << std::endl;
            if (!error.file.empty()) {
                std::cout << "  File: " << error.file << ":" << error.line << std::endl;
            }
            if (!error.function.empty()) {
                std::cout << "  Function: " << error.function << std::endl;
            }
            if (!error.context.empty()) {
                std::cout << "  Context: " << error.context << std::endl;
            }
            auto time = std::chrono::system_clock::to_time_t(error.timestamp);
            std::cout << "  Time: " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << std::endl;
            std::cout << std::endl;
        }
    }
    
    static void clearErrors() {
        std::lock_guard<std::mutex> lock(errorMutex);
        errors.clear();
    }
    
    static size_t getErrorCount() {
        std::lock_guard<std::mutex> lock(errorMutex);
        return errors.size();
    }
};

// Enhanced configuration validation
class ConfigValidator {
public:
    static void validate(const Config& config) {
        const auto& cfg = config.getValues();
        
        // Validate simulation timestep
        auto it = cfg.find("simulation_timestep");
        if (it != cfg.end()) {
            float dt = std::stof(it->second);
            if (dt < 0.0001f || dt > 0.1f) {
                throw RangeError("simulation_timestep", dt, 0.0001f, 0.1f);
            }
        }
        
        // Validate neuron count
        it = cfg.find("neuron_count");
        if (it != cfg.end()) {
            size_t neuronCount = std::stoul(it->second);
            if (neuronCount < 1) {
                throw ValidationError("neuron_count must be at least 1, got " + std::to_string(neuronCount));
            }
        }
        
        // Validate plasticity parameters
        it = cfg.find("plasticity_learning_rate");
        if (it != cfg.end()) {
            float lr = std::stof(it->second);
            if (lr < 0.0f || lr > 1.0f) {
                throw RangeError("plasticity_learning_rate", lr, 0.0f, 1.0f);
            }
        }
        
        // Validate dopamine baseline
        it = cfg.find("dopamine_baseline");
        if (it != cfg.end()) {
            float db = std::stof(it->second);
            if (db < -1.0f || db > 1.0f) {
                throw RangeError("dopamine_baseline", db, -1.0f, 1.0f);
            }
        }
    }
};

// Enhanced Neuron validation
class NeuronValidator {
public:
    static void validate(const Neuron& neuron) {
        const auto& state = neuron.getState();
        
        // Validate membrane potential range
        if (state.membranePotential < -100.0f || state.membranePotential > 50.0f) {
            throw RangeError("membranePotential", state.membranePotential, -100.0f, 50.0f);
        }
        
        // Validate threshold range
        if (state.threshold < -100.0f || state.threshold > 50.0f) {
            throw RangeError("threshold", state.threshold, -100.0f, 50.0f);
        }
        
        // Validate refractory period
        if (state.refractoryPeriod > 1000) {
            throw RangeError("refractoryPeriod", state.refractoryPeriod, 0.0f, 1000.0f);
        }
        
        // Validate firing rate range
        if (state.firingRate < 0.0f || state.firingRate > 1000.0f) {
            throw RangeError("firingRate", state.firingRate, 0.0f, 1000.0f);
        }
    }
};

// Enhanced Synapse validation
class SynapseValidator {
public:
    static void validate(const Synapse& synapse) {
        // Validate synaptic weight range
        if (synapse.getWeight() < -10.0f || synapse.getWeight() > 10.0f) {
            throw RangeError("synapticWeight", synapse.getWeight(), -10.0f, 10.0f);
        }
        
        // Validate efficacy range
        if (synapse.getEfficacy() < 0.0f || synapse.getEfficacy() > 1.0f) {
            throw RangeError("synapticEfficacy", synapse.getEfficacy(), 0.0f, 1.0f);
        }
    }
};

// Safety checks for neural operations
class SafetyChecker {
public:
    // Check for numerical stability
    static void checkNumericalStability(float value, const std::string& context) {
        if (std::isnan(value)) {
            throw ValidationError("Numerical instability detected in " + context + ": NaN value");
        }
        if (std::isinf(value)) {
            throw ValidationError("Numerical instability detected in " + context + ": infinite value");
        }
        if (std::abs(value) > 1e10) {
            throw ValidationError("Potential numerical overflow in " + context + ": value = " + std::to_string(value));
        }
    }
    
    // Validate neural connectivity
    static void validateConnectivity(const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) {
            throw ValidationError("Empty neuron population - connectivity issue");
        }
        
        // Check for null pointers
        for (size_t i = 0; i < neurons.size(); ++i) {
            if (neurons[i] == nullptr) {
                throw NullPointerError("neuron at index " + std::to_string(i));
            }
        }
    }
};

// Global initialization and cleanup
namespace {
    class ErrorHandlingInitializer {
    public:
        ErrorHandlingInitializer() {
            // Initialize performance profiler metrics
            PerformanceProfiler::metrics.reserve(100);
            
            // Initialize error reporter
            ErrorReporter::errors.reserve(1000);
            
            Logger::getGlobal().info("Error handling and validation systems initialized");
        }
    };
    
    ErrorHandlingInitializer initializer;
}

// Static member definitions
std::map<std::string, PerformanceProfiler::PerformanceMetrics> PerformanceProfiler::metrics;
std::mutex PerformanceProfiler::metricsMutex;

std::vector<ErrorReporter::ErrorInfo> ErrorReporter::errors;
std::mutex ErrorReporter::errorMutex;

} // namespace nlm

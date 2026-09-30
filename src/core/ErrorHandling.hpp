// Enhanced error handling and validation for NLM
// Provides comprehensive error checking, validation, and recovery mechanisms

#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Error categories for NLM errors
enum class NLMErrorCategory {
    None,
    Configuration,
    Memory,
    Neural,
    Plasticity,
    MemorySystem,
    Neuromodulation,
    Simulation,
    IO,
    Runtime,
    Validation,
    Performance
};

// Exception class for NLM errors
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, 
             NLMErrorCategory category = NLMErrorCategory::Runtime,
             const std::string& details = "")
        : std::runtime_error(message), category_(category), details_(details) {}
    
    NLMErrorCategory getCategory() const { return category_; }
    const std::string& getDetails() const { return details_; }
    
    static std::string categoryToString(NLMErrorCategory category) {
        switch (category) {
            case NLMErrorCategory::None: return "None";
            case NLMErrorCategory::Configuration: return "Configuration";
            case NLMErrorCategory::Memory: return "Memory";
            case NLMErrorCategory::Neural: return "Neural";
            case NLMErrorCategory::Plasticity: return "Plasticity";
            case NLMErrorCategory::MemorySystem: return "MemorySystem";
            case NLMErrorCategory::Neuromodulation: return "Neuromodulation";
            case NLMErrorCategory::Simulation: return "Simulation";
            case NLMErrorCategory::IO: return "IO";
            case NLMErrorCategory::Runtime: return "Runtime";
            case NLMErrorCategory::Validation: return "Validation";
            case NLMErrorCategory::Performance: return "Performance";
            default: return "Unknown";
        }
    }
    
    std::string what() const override {
        std::ostringstream oss;
        oss << std::runtime_error::what();
        if (!details_.empty()) {
            oss << " (details: " << details_ << ")";
        }
        return oss.str();
    }
    
private:
    NLMErrorCategory category_;
    std::string details_;
};

// Validation class for parameter and state validation
class Validation {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : valid(true) {}
        
        void addError(const std::string& error) {
            valid = false;
            errors.push_back(error);
        }
        
        void addWarning(const std::string& warning) {
            warnings.push_back(warning);
        }
        
        std::string getSummary() const {
            std::ostringstream oss;
            if (!valid) {
                oss << "INVALID: ";
                for (const auto& error : errors) {
                    oss << error << "; ";
                }
            }
            if (!warnings.empty()) {
                if (!valid) oss << "Warnings: ";
                for (const auto& warning : warnings) {
                    oss << warning << "; ";
                }
            }
            return oss.str();
        }
    };
    
    // Validate neuron parameters
    static ValidationResult validateNeuronParameters(
        float membranePotential, float restingPotential, float threshold,
        float resetPotential, float leakConductance, uint32_t refractoryPeriod) {
        
        ValidationResult result;
        
        // Check basic neuron parameters
        if (threshold <= restingPotential) {
            result.addError("Threshold must be greater than resting potential");
        }
        
        if (resetPotential < restingPotential) {
            result.addWarning("Reset potential is below resting potential (may cause instability)");
        }
        
        if (leakConductance <= 0.0f) {
            result.addError("Leak conductance must be positive");
        }
        
        if (refractoryPeriod == 0) {
            result.addWarning("Refractory period is zero (no refractory effect)");
        }
        
        // Check membrane potential range (reasonable bounds)
        const float MIN_POTENTIAL = -100.0f;
        const float MAX_POTENTIAL = 50.0f;
        
        if (membranePotential < MIN_POTENTIAL || membranePotential > MAX_POTENTIAL) {
            result.addWarning("Membrane potential outside typical bounds [" + 
                            std::to_string(MIN_POTENTIAL) + ", " + 
                            std::to_string(MAX_POTENTIAL) + "]");
        }
        
        return result;
    }
    
    // Validate synaptic parameters
    static ValidationResult validateSynapticParameters(
        float weight, float delay, float efficacy, SynapseType type) {
        
        ValidationResult result;
        
        // Weight bounds
        const float MIN_WEIGHT = -1.0f;
        const float MAX_WEIGHT = 1.0f;
        
        if (weight < MIN_WEIGHT || weight > MAX_WEIGHT) {
            result.addError("Synaptic weight " + std::to_string(weight) + 
                          " outside valid range [" + std::to_string(MIN_WEIGHT) + ", " +
                          std::to_string(MAX_WEIGHT) + "]");
        }
        
        // Delay bounds
        if (delay < 1) {
            result.addWarning("Synaptic delay " + std::to_string(delay) + 
                            " is less than minimum (1 step)");
        }
        
        // Efficacy bounds
        if (efficacy < 0.0f || efficacy > 2.0f) {
            result.addWarning("Synaptic efficacy " + std::to_string(efficacy) + 
                            " outside typical range [0.0, 2.0]");
        }
        
        // Type-specific validation
        if (type == SynapseType::Excitatory && weight < 0) {
            result.addWarning("Excitatory synapse has negative weight");
        }
        
        if (type == SynapseType::Inhibitory && weight > 0) {
            result.addWarning("Inhibitory synapse has positive weight");
        }
        
        return result;
    }
    
    // Validate simulation parameters
    static ValidationResult validateSimulationParameters(
        double timestep, size_t neuronCount, size_t regionCount,
        float connectionProbability) {
        
        ValidationResult result;
        
        if (timestep <= 0.0) {
            result.addError("Simulation timestep must be positive");
        }
        
        if (timestep > 0.1) {
            result.addWarning("Simulation timestep " + std::to_string(timestep) + 
                            " may be too large for accurate results");
        }
        
        if (neuronCount == 0) {
            result.addError("Neuron count must be positive");
        }
        
        if (neuronCount > 1000000) {
            result.addWarning("Neuron count " + std::to_string(neuronCount) + 
                            " is very large (performance impact)");
        }
        
        if (regionCount == 0) {
            result.addError("Region count must be positive");
        }
        
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            result.addError("Connection probability must be in range [0.0, 1.0]");
        }
        
        return result;
    }
    
    // Validate configuration values
    static ValidationResult validateConfiguration(const std::map<std::string, std::string>& config) {
        ValidationResult result;
        
        // Check for required keys
        std::vector<std::string> requiredKeys = {
            "random_seed",
            "simulation_timestep",
            "neuron_count",
            "region_count",
            "connection_probability"
        };
        
        for (const auto& key : requiredKeys) {
            if (config.find(key) == config.end()) {
                result.addError("Missing required configuration key: " + key);
            }
        }
        
        // Validate numeric values where possible
        if (config.find("random_seed") != config.end()) {
            try {
                long seed = std::stol(config.at("random_seed"));
                if (seed < 0) {
                    result.addWarning("Random seed should be non-negative");
                }
            } catch (const std::exception&) {
                result.addError("Invalid random_seed value: " + config.at("random_seed"));
            }
        }
        
        if (config.find("simulation_timestep") != config.end()) {
            try {
                double timestep = std::stod(config.at("simulation_timestep"));
                if (timestep <= 0.0) {
                    result.addError("simulation_timestep must be positive");
                }
            } catch (const std::exception&) {
                result.addError("Invalid simulation_timestep value: " + config.at("simulation_timestep"));
            }
        }
        
        return result;
    }
    
    // Check for potential performance issues
    static ValidationResult checkPerformanceIssues(const std::string& component, 
                                                    size_t elementCount, 
                                                    const std::string& context = "") {
        
        ValidationResult result;
        
        if (elementCount > 100000) {
            result.addWarning("Performance concern: " + component + 
                            " has " + std::to_string(elementCount) + 
                            " elements" + 
                            (context.empty() ? "" : " (" + context + ")"));
        }
        
        if (elementCount == 0) {
            result.addWarning("Performance concern: " + component + 
                            " has zero elements");
        }
        
        return result;
    }
    
    // Safe numeric conversion with validation
    template<typename T>
    static std::optional<T> safeConvert(const std::string& value, 
                                       const std::string& fieldName,
                                       T min = std::numeric_limits<T>::lowest(),
                                       T max = std::numeric_limits<T>::max()) {
        
        try {
            T result = std::stod(value); // Use stod for all numeric types for consistency
            
            // Convert to target type
            if constexpr (std::is_integral_v<T>) {
                if (std::is_unsigned_v<T> && result < 0) {
                    return std::nullopt;
                }
                return static_cast<T>(result);
            } else {
                return result;
            }
            
            // Validate range
            if (result < min || result > max) {
                return std::nullopt;
            }
            
            return result;
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }
};

// Error recovery utilities
class ErrorRecovery {
public:
    // Attempt to recover from invalid configuration by applying defaults
    static bool recoverConfiguration(std::map<std::string, std::string>& config) {
        bool recovered = false;
        
        // Check and fix simulation timestep
        if (config.find("simulation_timestep") == config.end()) {
            config["simulation_timestep"] = "0.001";
            recovered = true;
        } else {
            try {
                double timestep = std::stod(config["simulation_timestep"]);
                if (timestep <= 0.0) {
                    config["simulation_timestep"] = "0.001";
                    recovered = true;
                }
            } catch (...) {
                config["simulation_timestep"] = "0.001";
                recovered = true;
            }
        }
        
        // Check and fix neuron count
        if (config.find("neuron_count") == config.end()) {
            config["neuron_count"] = "500";
            recovered = true;
        } else {
            try {
                long count = std::stol(config["neuron_count"]);
                if (count <= 0) {
                    config["neuron_count"] = "500";
                    recovered = true;
                }
            } catch (...) {
                config["neuron_count"] = "500";
                recovered = true;
            }
        }
        
        // Check and fix region count
        if (config.find("region_count") == config.end()) {
            config["region_count"] = "1";
            recovered = true;
        } else {
            try {
                long count = std::stol(config["region_count"]);
                if (count <= 0) {
                    config["region_count"] = "1";
                    recovered = true;
                }
            } catch (...) {
                config["region_count"] = "1";
                recovered = true;
            }
        }
        
        // Check and fix connection probability
        if (config.find("connection_probability") == config.end()) {
            config["connection_probability"] = "0.1";
            recovered = true;
        } else {
            try {
                double prob = std::stod(config["connection_probability"]);
                if (prob < 0.0 || prob > 1.0) {
                    config["connection_probability"] = "0.1";
                    recovered = true;
                }
            } catch (...) {
                config["connection_probability"] = "0.1";
                recovered = true;
            }
        }
        
        return recovered;
    }
    
    // Validate and fix neuron state
    static bool recoverNeuronState(NeuronState& state) {
        bool recovered = false;
        
        // Ensure membrane potential is within reasonable bounds
        const float MIN_POTENTIAL = -100.0f;
        const float MAX_POTENTIAL = 50.0f;
        
        if (state.membranePotential < MIN_POTENTIAL) {
            state.membranePotential = MIN_POTENTIAL;
            recovered = true;
        } else if (state.membranePotential > MAX_POTENTIAL) {
            state.membranePotential = MAX_POTENTIAL;
            recovered = true;
        }
        
        // Ensure firing rate is non-negative
        if (state.firingRate < 0.0f) {
            state.firingRate = 0.0f;
            recovered = true;
        }
        
        // Ensure refractory counters are reasonable
        if (state.refractoryRemaining > state.refractoryPeriod) {
            state.refractoryRemaining = state.refractoryPeriod;
            recovered = true;
        }
        
        return recovered;
    }
    
    // Validate and fix synaptic state
    static bool recoverSynapticState(Synapse::Impl& synapse) {
        bool recovered = false;
        
        // Clamp weight to valid range
        const float MIN_WEIGHT = -1.0f;
        const float MAX_WEIGHT = 1.0f;
        
        if (synapse.weight < MIN_WEIGHT) {
            synapse.weight = MIN_WEIGHT;
            recovered = true;
        } else if (synapse.weight > MAX_WEIGHT) {
            synapse.weight = MAX_WEIGHT;
            recovered = true;
        }
        
        // Clamp efficacy to valid range
        if (synapse.efficacy < 0.0f) {
            synapse.efficacy = 0.0f;
            recovered = true;
        } else if (synapse.efficacy > 2.0f) {
            synapse.efficacy = 2.0f;
            recovered = true;
        }
        
        // Reset spike history if it gets too large
        const size_t MAX_SPIKE_HISTORY = 100;
        if (synapse.preSpikeHistory.size() > MAX_SPIKE_HISTORY) {
            synapse.preSpikeHistory.erase(synapse.preSpikeHistory.begin(),
                                         synapse.preSpikeHistory.begin() + 
                                         (synapse.preSpikeHistory.size() - MAX_SPIKE_HISTORY));
            recovered = true;
        }
        
        if (synapse.postSpikeHistory.size() > MAX_SPIKE_HISTORY) {
            synapse.postSpikeHistory.erase(synapse.postSpikeHistory.begin(),
                                          synapse.postSpikeHistory.begin() + 
                                          (synapse.postSpikeHistory.size() - MAX_SPIKE_HISTORY));
            recovered = true;
        }
        
        return recovered;
    }
};

// Enhanced logging with error tracking
class ErrorLogger {
public:
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        LogLevel level;
        std::string message;
        std::string file;
        int line;
        std::string function;
        std::vector<std::string> errorContext;
        bool recovered;
    };
    
    static ErrorLogger& getInstance() {
        static ErrorLogger instance;
        return instance;
    }
    
    void logError(const std::string& message, NLMErrorCategory category = NLMErrorCategory::Runtime,
                  const std::string& details = "", bool recovered = false,
                  const std::string& file = "", int line = 0, const std::string& function = "") {
        LogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.level = LogLevel::Error;
        entry.message = message;
        entry.file = file.empty() ? "" : file;
        entry.line = line;
        entry.function = function;
        entry.recovered = recovered;
        
        // Add error category to context
        std::ostringstream oss;
        oss << "Category: " << NLMError::categoryToString(category);
        if (!details.empty()) {
            oss << ", Details: " << details;
        }
        entry.errorContext.push_back(oss.str());
        
        logEntry(entry);
        
        // If this is a serious error and not recovered, store for potential debugging
        if (category == NLMErrorCategory::Critical || category == NLMErrorCategory::Runtime) {
            recentErrors_.push_back(entry);
            if (recentErrors_.size() > 100) {
                recentErrors_.erase(recentErrors_.begin());
            }
        }
    }
    
    void logWarning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        LogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.level = LogLevel::Warning;
        entry.message = message;
        entry.file = file.empty() ? "" : file;
        entry.line = line;
        entry.function = function;
        entry.recovered = false;
        
        logEntry(entry);
    }
    
    void logInfo(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        LogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.level = LogLevel::Info;
        entry.message = message;
        entry.file = file.empty() ? "" : file;
        entry.line = line;
        entry.function = function;
        entry.recovered = false;
        
        logEntry(entry);
    }
    
    // Get recent errors
    const std::vector<LogEntry>& getRecentErrors() const {
        return recentErrors_;
    }
    
    // Clear error log
    void clearErrorLog() {
        recentErrors_.clear();
    }
    
    // Format error log for output
    std::string formatErrorLog() const {
        std::ostringstream oss;
        oss << "=== NLM Error Log ===\n";
        oss << "Total errors: " << recentErrors_.size() << "\n";
        
        for (const auto& entry : recentErrors_) {
            oss << "\n[" << formatTime(entry.timestamp) << "] "
                << levelToString(entry.level) << ": "
                << entry.message;
            
            if (!entry.file.empty()) {
                oss << " (" << entry.file;
                if (entry.line > 0) oss << ":" << entry.line;
                oss << ")";
            }
            
            if (!entry.function.empty()) {
                oss << " in " << entry.function;
            }
            
            if (!entry.errorContext.empty()) {
                oss << "\n  Context:";
                for (const auto& context : entry.errorContext) {
                    oss << "\n    - " << context;
                }
            }
            
            if (entry.recovered) {
                oss << " [Recovered]";
            }
        }
        
        return oss.str();
    }
    
private:
    ErrorLogger() = default;
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    std::string formatTime(const std::chrono::system_clock::time_point& tp) const {
        auto time = std::chrono::system_clock::to_time_t(tp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            tp.time_since_epoch()) % 1000;
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }
    
    void logEntry(const LogEntry& entry) {
        // Store in memory
        recentEntries_.push_back(entry);
        if (recentEntries_.size() > 1000) {
            recentEntries_.erase(recentEntries_.begin());
        }
        
        // Output to console (in production, would use proper logging system)
        std::ostringstream oss;
        oss << "[" << formatTime(entry.timestamp) << "] "
            << levelToString(entry.level) << ": "
            << entry.message;
        
        if (!entry.file.empty()) {
            oss << " (" << entry.file;
            if (entry.line > 0) oss << ":" << entry.line;
            oss << ")";
        }
        
        std::cout << oss.str() << std::endl;
        
        // If it's an error and not recovered, also add to error log
        if (entry.level == LogLevel::Error || entry.level == LogLevel::Critical) {
            recentErrors_.push_back(entry);
            if (recentErrors_.size() > 100) {
                recentErrors_.erase(recentErrors_.begin());
            }
        }
    }
    
    std::vector<LogEntry> recentEntries_;
    std::vector<LogEntry> recentErrors_;
};

// Convenience macros for error logging
#define NLM_VALIDATE(condition, errorMsg, category) \
    if (!(condition)) { \
        ErrorLogger::getInstance().logError(errorMsg, category, #condition, false, __FILE__, __LINE__, __func__); \
        return false; \
    }

#define NLM_VALIDATE_AND_RECOVER(condition, errorMsg, recoverFunc, category) \
    if (!(condition)) { \
        ErrorLogger::getInstance().logError(errorMsg, category, #condition, true, __FILE__, __LINE__, __func__); \
        if (recoverFunc()) { \
            return true; \
        } \
        return false; \
    }

#define NLM_LOG_ERROR(msg, category, details) \
    ErrorLogger::getInstance().logError(msg, category, details, false, __FILE__, __LINE__, __func__)

#define NLM_LOG_WARNING(msg) \
    ErrorLogger::getInstance().logWarning(msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_INFO(msg) \
    ErrorLogger::getInstance().logInfo(msg, __FILE__, __LINE__, __func__)

} // namespace nlm

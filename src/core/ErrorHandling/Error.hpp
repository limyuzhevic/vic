// nlm - Neural Learning Machine
// Core Error Handling System
// Phase 2: Robust Error Management
//
// This phase implements comprehensive error handling with:
// - Exception hierarchies for different error types
// - Error codes and categorization
// - Error recovery mechanisms
// - Error reporting and logging

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Low = 0,
    Medium = 1,
    High = 2,
    Critical = 3,
    Catastrophic = 4
};

// Error categories
enum class ErrorCategory {
    None = 0,
    Memory,
    Network,
    FileSystem,
    Configuration,
    Neural,
    Simulation,
    Resource,
    Timing,
    Hardware,
    Software,
    UserInput,
    Performance,
    Security,
    System
};

// Base exception class
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message,
                 ErrorSeverity severity = ErrorSeverity::High,
                 ErrorCategory category = ErrorCategory::System,
                 const std::string& source = "",
                 const std::string& details = "")
        : std::runtime_error(message),
          severity(severity),
          category(category),
          source(source),
          details(details),
          timestamp(std::chrono::system_clock::now()) {
        generateErrorId();
    }
    
    const std::string& getSource() const { return source; }
    const std::string& getDetails() const { return details; }
    ErrorSeverity getSeverity() const { return severity; }
    ErrorCategory getCategory() const { return category; }
    const std::string& getErrorId() const { return errorId; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp; }
    
    std::string format() const {
        std::ostringstream oss;
        oss << "[" << severityToString(severity) << "] ";
        oss << "[" << errorId << "] ";
        oss << "[" << categoryToString(category) << "] ";
        oss << message();
        
        if (!source.empty()) {
            oss << " [Source: " << source << "]";
        }
        
        if (!details.empty()) {
            oss << " [Details: " << details << "]";
        }
        
        return oss.str();
    }
    
protected:
    void generateErrorId() {
        std::ostringstream oss;
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        oss << "ERR-" << ms << "-" << std::rand();
        errorId = oss.str();
    }
    
    static std::string severityToString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Low: return "LOW";
            case ErrorSeverity::Medium: return "MEDIUM";
            case ErrorSeverity::High: return "HIGH";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Catastrophic: return "CATASTROPHIC";
            default: return "UNKNOWN";
        }
    }
    
    static std::string categoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::None: return "NONE";
            case ErrorCategory::Memory: return "MEMORY";
            case ErrorCategory::Network: return "NETWORK";
            case ErrorCategory::FileSystem: return "FILESYSTEM";
            case ErrorCategory::Configuration: return "CONFIGURATION";
            case ErrorCategory::Neural: return "NEURAL";
            case ErrorCategory::Simulation: return "SIMULATION";
            case ErrorCategory::Resource: return "RESOURCE";
            case ErrorCategory::Timing: return "TIMING";
            case ErrorCategory::Hardware: return "HARDWARE";
            case ErrorCategory::Software: return "SOFTWARE";
            case ErrorCategory::UserInput: return "USERINPUT";
            case ErrorCategory::Performance: return "PERFORMANCE";
            case ErrorCategory::Security: return "SECURITY";
            case ErrorCategory::System: return "SYSTEM";
            default: return "UNKNOWN";
        }
    }
    
private:
    ErrorSeverity severity;
    ErrorCategory category;
    std::string source;
    std::string details;
    std::string errorId;
    std::chrono::system_clock::time_point timestamp;
};

// Memory allocation error
class MemoryAllocationError : public NLMException {
public:
    MemoryAllocationError(size_t size, const std::string& source = "")
        : NLMException("Memory allocation failed", ErrorSeverity::Critical,
                      ErrorCategory::Memory, source,
                      "Requested size: " + std::to_string(size) + " bytes") {}
};

// File not found error
class FileNotFoundError : public NLMException {
public:
    FileNotFoundError(const std::string& filepath, const std::string& source = "")
        : NLMException("File not found", ErrorSeverity::High,
                      ErrorCategory::FileSystem, source,
                      "File: " + filepath) {}
};

// Configuration error
class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& key, const std::string& value,
                      const std::string& source = "")
        : NLMException("Configuration error", ErrorSeverity::High,
                      ErrorCategory::Configuration, source,
                      "Key: " + key + ", Value: " + value) {}
};

// Neural computation error
class NeuralComputationError : public NLMException {
public:
    NeuralComputationError(const std::string& details, const std::string& source = "")
        : NLMException("Neural computation error", ErrorSeverity::Medium,
                      ErrorCategory::Neural, source, details) {}
};

// Simulation error
class SimulationError : public NLMException {
public:
    SimulationError(const std::string& details, const std::string& source = "")
        : NLMException("Simulation error", ErrorSeverity::High,
                      ErrorCategory::Simulation, source, details) {}
};

// Error recovery handler
class ErrorHandler {
public:
    struct Config {
        bool enabled = true;
        bool stopOnCritical = true;
        bool logStackTrace = true;
        bool collectMetrics = true;
    };
    
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    void configure(const Config& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
    }
    
    bool handle(const std::exception& error) {
        if (!config_.enabled) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Log error
        logError(error);
        
        // Determine response based on error severity
        if (error.what() != std::string()) {
            // For runtime errors, check type
            try {
                throw error;
            } catch (const NLMException& nlmError) {
                switch (nlmError.getSeverity()) {
                    case ErrorSeverity::Low:
                        // Log and continue
                        return true;
                        
                    case ErrorSeverity::Medium:
                        // Log and continue
                        return true;
                        
                    case ErrorSeverity::High:
                        // Log and continue unless stopOnCritical
                        if (config_.stopOnCritical) {
                            shutdown();
                            return false;
                        }
                        return true;
                        
                    case ErrorSeverity::Critical:
                        // Log and shutdown
                        shutdown();
                        return false;
                        
                    case ErrorSeverity::Catastrophic:
                        // Log and abort
                        abort();
                        return false;
                }
            } catch (...) {
                // Unknown error type
                logError(error);
                return true;
            }
        }
        
        return true;
    }
    
    void shutdown() {
        NLM_LOG_CRITICAL("System shutdown due to critical error");
        std::exit(1);
    }
    
    void abort() {
        NLM_LOG_CRITICAL("System abort due to catastrophic error");
        std::abort();
    }
    
protected:
    ErrorHandler() = default;
    
    void logError(const std::exception& error) {
        NLM_LOG_ERROR("Error: " + std::string(error.what()));
    }
    
private:
    Config config_;
    mutable std::mutex mutex_;
};

// Convenience macros
#define NLM_THROW_MEMORY_ERROR(size, source) throw nlm::MemoryAllocationError(size, source)
#define NLM_THROW_FILE_ERROR(filepath, source) throw nlm::FileNotFoundError(filepath, source)
#define NLM_THROW_CONFIG_ERROR(key, value, source) throw nlm::ConfigurationError(key, value, source)

} // namespace nlm
#pragma once

#include <string>
#include <chrono>
#include <stdexcept>

namespace nlm {

// Error handling infrastructure for NLM

enum class NLMErrorCategory {
    Config = 0,           // Configuration errors
    Memory = 1,           // Memory allocation errors
    File = 2,             // File I/O errors
    Simulation = 3,       // Simulation-specific errors
    Plasticity = 4,       // Plasticity rule errors
    MemorySystem = 5,     // Working/episodic memory errors
    Neuromodulation = 6,  // Neuromodulation system errors
    Development = 7,      // Development system errors
    Prediction = 8,       // Prediction system errors
    Cognitive = 9,        // Cognitive system errors
    Checkpoint = 10,      // Checkpoint/save-load errors
    Threading = 11,       // Thread synchronization errors
    Network = 12,         // Network communication errors
    Integration = 13,     // Integration/initialization errors
    Unknown = 14          // Unknown/unspecified errors
};

class NLMError : public std::runtime_error {
public:
    NLMError(NLMErrorCategory category, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "")
        : std::runtime_error(formatFullMessage(category, message)),
          category_(category), message_(message), 
          file_(file), line_(line), function_(function),
          timestamp_(std::chrono::system_clock::now()) {}
    
    NLMErrorCategory getCategory() const { return category_; }
    const std::string& getMessage() const { return message_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    auto getTimestamp() const { return timestamp_; }
    
    std::string getFullMessage() const {
        return formatFullMessage(category_, message_);
    }
    
    // Create a more detailed error message with file and line info
    static std::string formatFullMessage(NLMErrorCategory category, const std::string& message) {
        std::string categoryStr;
        switch (category) {
            case NLMErrorCategory::Config: categoryStr = "CONFIG"; break;
            case NLMErrorCategory::Memory: categoryStr = "MEMORY"; break;
            case NLMErrorCategory::File: categoryStr = "FILE"; break;
            case NLMErrorCategory::Simulation: categoryStr = "SIMULATION"; break;
            case NLMErrorCategory::Plasticity: categoryStr = "PLASTICITY"; break;
            case NLMErrorCategory::MemorySystem: categoryStr = "MEMORY_SYSTEM"; break;
            case NLMErrorCategory::Neuromodulation: categoryStr = "NEUROMODULATION"; break;
            case NLMErrorCategory::Development: categoryStr = "DEVELOPMENT"; break;
            case NLMErrorCategory::Prediction: categoryStr = "PREDICTION"; break;
            case NLMErrorCategory::Cognitive: categoryStr = "COGNITIVE"; break;
            case NLMErrorCategory::Checkpoint: categoryStr = "CHECKPOINT"; break;
            case NLMErrorCategory::Threading: categoryStr = "THREADING"; break;
            case NLMErrorCategory::Network: categoryStr = "NETWORK"; break;
            case NLMErrorCategory::Integration: categoryStr = "INTEGRATION"; break;
            case NLMErrorCategory::Unknown: categoryStr = "UNKNOWN"; break;
            default: categoryStr = "UNSPECIFIED"; break;
        }
        
        return "[" + categoryStr + "] " + message;
    }
    
private:
    NLMErrorCategory category_;
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
};

// Convenience exception types for common error scenarios
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, const std::string& key = "")
        : NLMError(NLMErrorCategory::Config, message), key_(key) {}
    
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

class MemoryAllocationError : public NLMError {
public:
    MemoryAllocationError(const std::string& message, size_t size = 0)
        : NLMError(NLMErrorCategory::Memory, message), size_(size) {}
    
    size_t getSize() const { return size_; }
    
private:
    size_t size_;
};

class FileIOError : public NLMError {
public:
    FileIOError(const std::string& message, const std::string& filepath = "")
        : NLMError(NLMErrorCategory::File, message), filepath_(filepath) {}
    
    const std::string& getFilePath() const { return filepath_; }
    
private:
    std::string filepath_;
};

class SimulationError : public NLMError {
public:
    SimulationError(const std::string& message, int step = 0)
        : NLMError(NLMErrorCategory::Simulation, message), step_(step) {}
    
    int getStep() const { return step_; }
    
private:
    int step_;
};

class CheckpointError : public NLMError {
public:
    CheckpointError(const std::string& message, const std::string& filepath = "")
        : NLMError(NLMErrorCategory::Checkpoint, message), filepath_(filepath) {}
    
    const std::string& getFilePath() const { return filepath_; }
    
private:
    std::string filepath_;
};

// Validation error for configuration values
class ValidationError : public ConfigurationError {
public:
    ValidationError(const std::string& message, const std::string& key, 
                    const std::string& value)
        : ConfigurationError(message, key), value_(value) {}
    
    const std::string& getValue() const { return value_; }
    
private:
    std::string value_;
};

} // namespace nlm

// NLM Exception Hierarchy
// Custom exceptions for NLM with proper error categorization and recovery information

#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <map>

namespace nlm {

// Base NLM exception
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error("NLM Error: " + message), message_(message) {}
    
    const char* what() const noexcept override {
        return std::runtime_error::what();
    }
    
    virtual const std::string& getMessage() const { return message_; }
    virtual const char* getCategory() const { return "NLM"; }
    
protected:
    std::string message_;
};

// Configuration-related exceptions
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) 
        : NLMException("Configuration Error: " + message) {}
    
    const char* getCategory() const override { return "CONFIG"; }
};

class ConfigFileNotFoundException : public ConfigException {
public:
    explicit ConfigFileNotFoundException(const std::string& filename)
        : ConfigException("Configuration file not found: " + filename) {}
    
    const char* getCategory() const override { return "CONFIG_FILE_NOT_FOUND"; }
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message, const std::string& key = "")
        : ConfigException("Invalid configuration: " + message + (key.empty() ? "" : " for key: " + key)) {}
    
    const char* getCategory() const override { return "CONFIG_VALIDATION"; }
};

// Brain-related exceptions
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) 
        : NLMException("Brain Error: " + message) {}
    
    const char* getCategory() const override { return "BRAIN"; }
};

class BrainInitializationException : public BrainException {
public:
    explicit BrainInitializationException(const std::string& reason)
        : BrainException("Failed to initialize brain: " + reason) {}
    
    const char* getCategory() const override { return "BRAIN_INITIALIZATION"; }
};

class BrainStateException : public BrainException {
public:
    explicit BrainStateException(const std::string& reason)
        : BrainException("Invalid brain state: " + reason) {}
    
    const char* getCategory() const override { return "BRAIN_STATE"; }
};

// World-related exceptions
class WorldException : public NLMException {
public:
    explicit WorldException(const std::string& message) 
        : NLMException("World Error: " + message) {}
    
    const char* getCategory() const override { return "WORLD"; }
};

// Agent-related exceptions
class AgentException : public NLMException {
public:
    explicit AgentException(const std::string& message) 
        : NLMException("Agent Error: " + message) {}
    
    const char* getCategory() const override { return "AGENT"; }
};

// Simulation-related exceptions
class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message) 
        : NLMException("Simulation Error: " + message) {}
    
    const char* getCategory() const override { return "SIMULATION"; }
};

class SimulationNotInitializedException : public SimulationException {
public:
    explicit SimulationNotInitializedException()
        : SimulationException("Simulation not initialized. Call initialize() first.") {}
    
    const char* getCategory() const override { return "SIMULATION_NOT_INITIALIZED"; }
};

// Plasticity-related exceptions
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message) 
        : NLMException("Plasticity Error: " + message) {}
    
    const char* getCategory() const override { return "PLASTICITY"; }
};

// Memory-related exceptions
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) 
        : NLMException("Memory Error: " + message) {}
    
    const char* getCategory() const override { return "MEMORY"; }
};

// Neuromodulation-related exceptions
class NeuromodulationException : public NLMException {
public:
    explicit NeuromodulationException(const std::string& message) 
        : NLMException("Neuromodulation Error: " + message) {}
    
    const char* getCategory() const override { return "NEUROMODULATION"; }
};

// Development-related exceptions
class DevelopmentException : public NLMException {
public:
    explicit DevelopmentException(const std::string& message) 
        : NLMException("Development Error: " + message) {}
    
    const char* getCategory() const override { return "DEVELOPMENT"; }
};

// Visualization-related exceptions
class VisualizationException : public NLMException {
public:
    explicit VisualizationException(const std::string& message) 
        : NLMException("Visualization Error: " + message) {}
    
    const char* getCategory() const override { return "VISUALIZATION"; }
};

// Exception handling utilities
class ExceptionRegistry {
public:
    static ExceptionRegistry& getInstance() {
        static ExceptionRegistry instance;
        return instance;
    }
    
    void registerException(const std::string& category, std::exception_ptr exception) {
        exceptionHistory_.push_back({category, exception});
    }
    
    std::vector<std::pair<std::string, std::exception_ptr>> getExceptionHistory() const {
        return exceptionHistory_;
    }
    
    void clearHistory() {
        exceptionHistory_.clear();
    }
    
    std::string getLastErrorSummary() const {
        if (exceptionHistory_.empty()) {
            return "No errors recorded.";
        }
        
        std::string summary = "Last error: " + exceptionHistory_.back().first + "\n";
        try {
            std::rethrow_exception(exceptionHistory_.back().second);
        } catch (const std::exception& e) {
            summary += std::string("  Message: ") + e.what();
        }
        return summary;
    }
    
private:
    std::vector<std::pair<std::string, std::exception_ptr>> exceptionHistory_;
};

// Exception recovery utilities
class ExceptionRecovery {
public:
    static bool canRecoverFrom(const std::exception& e) {
        const std::type_info& type = typeid(e);
        
        // Check for different exception types that can be recovered from
        if (type == typeid(SimulationNotInitializedException)) {
            return true; // Can recover from uninitialized simulation
        } else if (type == typeid(BrainStateException)) {
            return true; // Can recover from invalid brain state
        } else if (type == typeid(ConfigValidationException)) {
            return true; // Can recover from invalid configuration
        }
        
        return false;
    }
    
    static bool attemptRecovery(const std::exception& e) {
        const std::type_info& type = typeid(e);
        
        if (type == typeid(SimulationNotInitializedException)) {
            // Attempt to reinitialize simulation
            // Implementation would depend on specific recovery logic
            return true;
        } else if (type == typeid(BrainStateException)) {
            // Attempt to reset brain to safe state
            // Implementation would depend on specific recovery logic
            return true;
        }
        
        return false;
    }
};

// Exception translator for C++ to Python conversion (for Python bindings)
inline std::string translateExceptionToString(const std::exception& e) {
    std::string result = "Exception: ";
    result += e.what();
    
    // Add category information if available
    try {
        const NLMException& nlmEx = dynamic_cast<const NLMException&>(e);
        result += " [Category: " + std::string(nlmEx.getCategory()) + "]";
    } catch (const std::bad_cast&) {
        // Not an NLM exception, just return the basic error
    }
    
    return result;
}

} // namespace nlm

#pragma once

#include <string>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <map>

namespace nlm {

/**
 * Base exception for all NLM brain system errors
 */
class BrainException : public std::runtime_error {
public:
    enum class ErrorCode {
        // Initialization errors
        BrainInitializationFailed = 1000,
        MemorySystemFailed = 1001,
        NeuralSystemFailed =1002,
        ConfigurationError = 1003,
        
        // Memory errors
        MemoryAllocationFailed = 2000,
        MemoryCapacityExceeded = 2001,
        MemoryInvalidState = 2002,
        MemoryCorruption = 2003,
        
        // Plasticity errors
        PlasticityRuleConflict = 3000,
        PlasticityUpdateFailed = 3001,
        PlasticityInvalidParameters = 3002,
        
        // Neuromodulation errors
        NeuromodulationLevelInvalid = 4000,
        NeuromodulatorUpdateFailed = 4001,
        
        // Developmental errors
        DevelopmentalStageTransitionFailed = 5000,
        CriticalPeriodOverrun = 5001,
        
        // Prediction system errors
        PredictionSystemFailed = 6000,
        PredictionErrorComputation = 6001,
        
        // File I/O errors
        FileIOError = 7000,
        SaveFailed = 7001,
        LoadFailed = 7002,
        InvalidFileFormat = 7003,
        
        // Runtime errors
        InvalidParameter = 8000,
        OutOfRange = 8001,
        InvalidState = 8002,
        SystemFailure = 8003,
        
        // System integration errors
        ComponentIntegrationFailed = 9000,
        CrossSystemConflict = 9001
    };

protected:
    ErrorCode errorCode;
    std::string component;
    std::map<std::string, std::string> context;
    std::vector<std::string> suggestions;

    BrainException(ErrorCode code, const std::string& message, const std::string& comp = "")
        : std::runtime_error(message), errorCode(code), component(comp) {}

    BrainException(ErrorCode code, const std::string& message, 
                   const std::map<std::string, std::string>& ctx, 
                   const std::string& comp = "")
        : std::runtime_error(message), errorCode(code), component(comp), context(ctx) {}

public:
    virtual ~BrainException() noexcept = default;

    ErrorCode getErrorCode() const { return errorCode; }
    const std::string& getComponent() const { return component; }
    const std::map<std::string, std::string>& getContext() const { return context; }
    const std::vector<std::string>& getSuggestions() const { return suggestions; }

    void addContext(const std::string& key, const std::string& value) {
        context[key] = value;
    }

    void addSuggestion(const std::string& suggestion) {
        suggestions.push_back(suggestion);
    }

    std::string whatWithDetails() const {
        std::ostringstream oss;
        oss << "[NLM " << errorCode << "] " << std::runtime_error::what();
        if (!component.empty()) oss << " (component: " << component << ")";
        if (!context.empty()) {
            oss << "\nContext:";
            for (const auto& pair : context) {
                oss << "\n  " << pair.first << ": " << pair.second;
            }
        }
        if (!suggestions.empty()) {
            oss << "\nSuggestions:";
            for (const auto& sug : suggestions) {
                oss << "\n  - " << sug;
            }
        }
        return oss.str();
    }
};

/**
 * Exception for brain initialization failures
 */
class BrainInitializationException : public BrainException {
public:
    BrainInitializationException(const std::string& message, const std::string& comp = "Brain")
        : BrainException(ErrorCode::BrainInitializationFailed, message, comp) {}
    
    BrainInitializationException(const std::string& message, 
                                 const std::map<std::string, std::string>& ctx,
                                 const std::string& comp = "Brain")
        : BrainException(ErrorCode::BrainInitializationFailed, message, ctx, comp) {}
};

/**
 * Exception for runtime brain system errors
 */
class BrainRuntimeException : public BrainException {
public:
    BrainRuntimeException(const std::string& message, const std::string& comp = "Runtime")
        : BrainException(ErrorCode::SystemFailure, message, comp) {}
    
    BrainRuntimeException(const std::string& message,
                          const std::map<std::string, std::string>& ctx,
                          const std::string& comp = "Runtime")
        : BrainException(ErrorCode::SystemFailure, message, ctx, comp) {}
};

/**
 * Exception for configuration-related errors
 */
class ConfigurationException : public BrainException {
public:
    ConfigurationException(const std::string& message, const std::string& comp = "Config")
        : BrainException(ErrorCode::ConfigurationError, message, comp) {}
    
    ConfigurationException(const std::string& message,
                          const std::map<std::string, std::string>& ctx,
                          const std::string& comp = "Config")
        : BrainException(ErrorCode::ConfigurationError, message, ctx, comp) {}
};

/**
 * Exception for memory system errors
 */
class MemoryException : public BrainException {
public:
    MemoryException(const std::string& message, const std::string& comp = "Memory")
        : BrainException(ErrorCode::MemoryInvalidState, message, comp) {}
    
    MemoryException(const std::string& message,
                   const std::map<std::string, std::string>& ctx,
                   const std::string& comp = "Memory")
        : BrainException(ErrorCode::MemoryInvalidState, message, ctx, comp) {}
    
    MemoryException(ErrorCode code, const std::string& message, const std::string& comp = "Memory")
        : BrainException(code, message, comp) {}
};

/**
 * Exception for plasticity-related errors
 */
class PlasticityException : public BrainException {
public:
    PlasticityException(const std::string& message, const std::string& comp = "Plasticity")
        : BrainException(ErrorCode::PlasticityInvalidParameters, message, comp) {}
    
    PlasticityException(const std::string& message,
                       const std::map<std::string, std::string>& ctx,
                       const std::string& comp = "Plasticity")
        : BrainException(ErrorCode::PlasticityInvalidParameters, message, ctx, comp) {}
    
    PlasticityException(ErrorCode code, const std::string& message, const std::string& comp = "Plasticity")
        : BrainException(code, message, comp) {}
};

/**
 * Exception for neuromodulation errors
 */
class NeuromodulationException : public BrainException {
public:
    NeuromodulationException(const std::string& message, const std::string& comp = "Neuromodulation")
        : BrainException(ErrorCode::NeuromodulationLevelInvalid, message, comp) {}
    
    NeuromodulationException(const std::string& message,
                           const std::map<std::string, std::string>& ctx,
                           const std::string& comp = "Neuromodulation")
        : BrainException(ErrorCode::NeuromodulationLevelInvalid, message, ctx, comp) {}
};

} // namespace nlm

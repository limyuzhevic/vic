#pragma once

#include <string>
#include <chrono>
#include <stdexcept>
#include <map>
#include <vector>
#include <memory>

namespace nlm {

class NLMException : public std::exception {
protected:
    std::string message_;
    std::string code_;
    std::chrono::system_clock::time_point timestamp_;
    std::map<std::string, std::string> context_;
    std::vector<std::string> stackTrace_;
    
public:
    explicit NLMException(const std::string& message, const std::string& code = "UNKNOWN")
        : message_(message), code_(code) {
        timestamp_ = std::chrono::system_clock::now();
    }
    
    explicit NLMException(const std::string& message, const std::string& code, 
                         const std::map<std::string, std::string>& context)
        : message_(message), code_(code), context_(context) {
        timestamp_ = std::chrono::system_clock::now();
    }
    
    virtual ~NLMException() noexcept = default;
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    virtual const std::string& getCode() const {
        return code_;
    }
    
    virtual const std::string& getMessage() const {
        return message_;
    }
    
    virtual std::chrono::system_clock::time_point getTimestamp() const {
        return timestamp_;
    }
    
    virtual std::string getTimestampString() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp_);
        std::string ts(std::ctime(&time));
        // Remove trailing newline
        if (!ts.empty() && ts.back() == '\n') {
            ts.pop_back();
        }
        return ts;
    }
    
    virtual const std::map<std::string, std::string>& getContext() const {
        return context_;
    }
    
    virtual void addContext(const std::string& key, const std::string& value) {
        context_[key] = value;
    }
    
    virtual std::string getContextString() const {
        if (context_.empty()) return "";
        
        std::string result = "Context: ";
        for (const auto& pair : context_) {
            result += "[" + pair.first + "=" + pair.second + "] ";
        }
        return result;
    }
    
    virtual std::string getFullMessage() const {
        std::string full = message_ + " (Code: " + code_ + ")";
        full += getContextString();
        full += " [Thrown: " + getTimestampString() + "]";
        return full;
    }
    
    virtual void addStackTrace(const std::vector<std::string>& trace) {
        stackTrace_ = trace;
    }
    
    virtual std::string getStackTrace() const {
        if (stackTrace_.empty()) return "No stack trace available";
        
        std::string result = "Stack trace:\n";
        for (size_t i = 0; i < stackTrace_.size(); ++i) {
            result += "  [" + std::to_string(i) + "] " + stackTrace_[i] + "\n";
        }
        return result;
    }
    
    virtual std::string toString() const {
        return getFullMessage() + "\n" + getStackTrace();
    }
};

// Sub-classes for different error categories

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message, const std::string& code = "CONFIG_ERROR")
        : NLMException(message, code) {}
    
    ConfigurationError(const std::string& message, const std::string& code,
                      const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& message, const std::string& code = "MEMORY_ERROR")
        : NLMException(message, code) {}
    
    MemoryError(const std::string& message, const std::string& code,
               const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class NeuralError : public NLMException {
public:
    explicit NeuralError(const std::string& message, const std::string& code = "NEURAL_ERROR")
        : NLMException(message, code) {}
    
    NeuralError(const std::string& message, const std::string& code,
               const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class PlasticityError : public NLMException {
public:
    explicit PlasticityError(const std::string& message, const std::string& code = "PLASTICITY_ERROR")
        : NLMException(message, code) {}
    
    PlasticityError(const std::string& message, const std::string& code,
                   const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class IntegrationError : public NLMException {
public:
    explicit IntegrationError(const std::string& message, const std::string& code = "INTEGRATION_ERROR")
        : NLMException(message, code) {}
    
    IntegrationError(const std::string& message, const std::string& code,
                    const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class DevelopmentError : public NLMException {
public:
    explicit DevelopmentError(const std::string& message, const std::string& code = "DEVELOPMENT_ERROR")
        : NLMException(message, code) {}
    
    DevelopmentError(const std::string& message, const std::string& code,
                    const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class NeuromodulationError : public NLMException {
public:
    explicit NeuromodulationError(const std::string& message, const std::string& code = "NEUROMODULATION_ERROR")
        : NLMException(message, code) {}
    
    NeuromodulationError(const std::string& message, const std::string& code,
                        const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class MemorySystemError : public NLMException {
public:
    explicit MemorySystemError(const std::string& message, const std::string& code = "MEMORY_SYSTEM_ERROR")
        : NLMException(message, code) {}
    
    MemorySystemError(const std::string& message, const std::string& code,
                     const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class PredictionError : public NLMException {
public:
    explicit PredictionError(const std::string& message, const std::string& code = "PREDICTION_ERROR")
        : NLMException(message, code) {}
    
    PredictionError(const std::string& message, const std::string& code,
                   const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class CognitiveError : public NLMException {
public:
    explicit CognitiveError(const std::string& message, const std::string& code = "COGNITIVE_ERROR")
        : NLMException(message, code) {}
    
    CognitiveError(const std::string& message, const std::string& code,
                  const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class SensoryError : public NLMException {
public:
    explicit SensoryError(const std::string& message, const std::string& code = "SENSORY_ERROR")
        : NLMException(message, code) {}
    
    SensoryError(const std::string& message, const std::string& code,
                const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class MotorError : public NLMException {
public:
    explicit MotorError(const std::string& message, const std::string& code = "MOTOR_ERROR")
        : NLMException(message, code) {}
    
    MotorError(const std::string& message, const std::string& code,
              const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& message, const std::string& code = "WORLD_ERROR")
        : NLMException(message, code) {}
    
    WorldError(const std::string& message, const std::string& code,
              const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

class AgentError : public NLMException {
public:
    explicit AgentError(const std::string& message, const std::string& code = "AGENT_ERROR")
        : NLMException(message, code) {}
    
    AgentError(const std::string& message, const std::string& code,
              const std::map<std::string, std::string>& context)
        : NLMException(message, code, context) {}
};

} // namespace nlm
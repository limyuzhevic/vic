// NLM (Neural Learning Machine) Python Bindings - Core Exception Hierarchy
// This file contains the custom exception hierarchy for better error handling

#include <string>

namespace nlm {
    class NLMError : public std::exception {
    public:
        std::string message;
        
        explicit NLMError(const std::string& msg) : message(msg) {}
        
        const char* what() const noexcept override {
            return message.c_str();
        }
    };
    
    class ConfigError : public NLMError {
    public:
        explicit ConfigError(const std::string& msg) : NLMError("Config Error: " + msg) {}
    };
    
    class FileError : public NLMError {
    public:
        explicit FileError(const std::string& msg) : NLMError("File Error: " + msg) {}
    };
    
    class SimulationError : public NLMError {
    public:
        explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
    };
    
    class ValidationError : public NLMError {
    public:
        explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
    };
}
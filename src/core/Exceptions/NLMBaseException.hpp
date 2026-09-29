#pragma once

#include <string>
#include <stdexcept>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Base exception class for NLM system
class NLMBaseException : public std::runtime_error {
public:
    NLMBaseException(const std::string& message, 
                     ErrorCode code,
                     const std::string& file = "",
                     int line = 0,
                     const std::string& function = "");
    
    virtual ~NLMBaseException() = default;
    
    // Error code accessor
    ErrorCode getCode() const { return code_; }
    
    // Location information
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    
    // Context information
    void setContext(const std::string& key, const std::string& value);
    const std::map<std::string, std::string>& getContext() const { return context_; }
    std::string getContextAsJson() const;
    
    // Exception chaining
    void addCause(const NLMBaseException& cause);
    const std::vector<std::string>& getCauses() const { return causes_; }
    
    // Serialization support for checkpoint/restart
    virtual json toJson() const;
    static std::unique_ptr<NLMBaseException> fromJson(const json& j);
    
    // Exception type identification
    virtual const char* what() const noexcept override;
    
protected:
    ErrorCode code_;
    std::string file_;
    int line_;
    std::string function_;
    std::map<std::string, std::string> context_;
    std::vector<std::string> causes_;
    mutable std::string messageWithContext_;
};

// Validation exception - for parameter validation errors
class NLMValidationException : public NLMBaseException {
public:
    NLMValidationException(const std::string& message,
                          const std::string& parameterName = "",
                          const std::string& file = "",
                          int line = 0,
                          const std::string& function = "");
    
    const std::string& getParameterName() const { return parameterName_; }
    
    json toJson() const override;
    static std::unique_ptr<NLMValidationException> fromJson(const json& j);
    
private:
    std::string parameterName_;
};

// Neural network exception - for neural network related errors
class NLMBrainException : public NLMBaseException {
public:
    NLMBrainException(const std::string& message,
                     const std::string& networkName = "",
                     const std::string& file = "",
                     int line = 0,
                     const std::string& function = "");
    
    const std::string& getNetworkName() const { return networkName_; }
    
    json toJson() const override;
    static std::unique_ptr<NLMBrainException> fromJson(const json& j);
    
private:
    std::string networkName_;
};

// Simulation exception - for simulation related errors
class NLMSimulationException : public NLMBaseException {
public:
    NLMSimulationException(const std::string& message,
                          const std::string& simulationName = "",
                          const std::string& file = "",
                          int line = 0,
                          const std::string& function = "");
    
    const std::string& getSimulationName() const { return simulationName_; }
    
    json toJson() const override;
    static std::unique_ptr<NLMSimulationException> fromJson(const json& j);
    
private:
    std::string simulationName_;
};

// System exception - for system level errors
class NLMSystemException : public NLMBaseException {
public:
    NLMSystemException(const std::string& message,
                      const std::string& systemComponent = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::string& function = "");
    
    const std::string& getSystemComponent() const { return systemComponent_; }
    
    json toJson() const override;
    static std::unique_ptr<NLMSystemException> fromJson(const json& j);
    
private:
    std::string systemComponent_;
};

// File system exception - for file related errors
class NLMFileException : public NLMBaseException {
public:
    NLMFileException(const std::string& message,
                    const std::string& filepath = "",
                    const std::string& file = "",
                    int line = 0,
                    const std::string& function = "");
    
    const std::string& getFilepath() const { return filepath_; }
    
    json toJson() const override;
    static std::unique_ptr<NLMFileException> fromJson(const json& j);
    
private:
    std::string filepath_;
};

} // namespace nlm

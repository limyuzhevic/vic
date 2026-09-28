/*
 * nlm/BrainException.hpp - Brain-specific exception classes for the NLM system
 * Specialized exceptions for brain-related errors with detailed context
 */

#pragma once

#include "nlm/Exceptions.hpp"
#include <string>

namespace nlm {

// Brain-specific exception types with additional context
class BrainSaveErrorException : public BrainException {
public:
    explicit BrainSaveErrorException(const std::string& filepath)
        : BrainException("Brain save error: " + filepath) {
        addContext("filepath", filepath);
    }
    
    BrainSaveErrorException(const std::string& filepath,
                            const std::string& file,
                            int line,
                            const std::string& function)
        : BrainException("Brain save error: " + filepath, file, line, function) {
        addContext("filepath", filepath);
    }
    
    virtual const char* type() const noexcept override { return "BrainSaveErrorException"; }
};

class BrainLoadErrorException : public BrainException {
public:
    explicit BrainLoadErrorException(const std::string& filepath)
        : BrainException("Brain load error: " + filepath) {
        addContext("filepath", filepath);
    }
    
    BrainLoadErrorException(const std::string& filepath,
                            const std::string& file,
                            int line,
                            const std::string& function)
        : BrainException("Brain load error: " + filepath, file, line, function) {
        addContext("filepath", filepath);
    }
    
    virtual const char* type() const noexcept override { return "BrainLoadErrorException"; }
};

class BrainInitializationErrorException : public BrainException {
public:
    explicit BrainInitializationErrorException(const std::string& reason)
        : BrainException("Brain initialization error: " + reason) {
        addContext("reason", reason);
    }
    
    BrainInitializationErrorException(const std::string& reason,
                                       const std::string& file,
                                       int line,
                                       const std::string& function)
        : BrainException("Brain initialization error: " + reason, file, line, function) {
        addContext("reason", reason);
    }
    
    virtual const char* type() const noexcept override { return "BrainInitializationErrorException"; }
};

class BrainComputationErrorException : public BrainException {
public:
    explicit BrainComputationErrorException(const std::string& details)
        : BrainException("Brain computation error: " + details) {
        addContext("details", details);
    }
    
    BrainComputationErrorException(const std::string& details,
                                   const std::string& file,
                                   int line,
                                   const std::string& function)
        : BrainException("Brain computation error: " + details, file, line, function) {
        addContext("details", details);
    }
    
    virtual const char* type() const noexcept override { return "BrainComputationErrorException"; }
};

class BrainPlasticityErrorException : public BrainException {
public:
    explicit BrainPlasticityErrorException(const std::string& ruleName, const std::string& details)
        : BrainException("Brain plasticity error in rule '" + ruleName + "': " + details) {
        addContext("ruleName", ruleName);
        addContext("details", details);
    }
    
    BrainPlasticityErrorException(const std::string& ruleName,
                                  const std::string& details,
                                  const std::string& file,
                                  int line,
                                  const std::string& function)
        : BrainException("Brain plasticity error in rule '" + ruleName + "': " + details, file, line, function) {
        addContext("ruleName", ruleName);
        addContext("details", details);
    }
    
    virtual const char* type() const noexcept override { return "BrainPlasticityErrorException"; }
};

} // namespace nlm

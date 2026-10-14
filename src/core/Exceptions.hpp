#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <map>

namespace nlm {

// Base NLM exception
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
    
    NLMError(const std::string& message, const std::string& context)
        : std::runtime_error(message + " (Context: " + context + ")") {}
};

// Configuration-specific exceptions
class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError(message, "Configuration") {}
};

class ConfigValidationError : public ConfigurationError {
public:
    explicit ConfigValidationError(const std::string& message) : ConfigurationError(message) {}
};

class ConfigFileError : public ConfigurationError {
public:
    explicit ConfigFileError(const std::string& message) : ConfigurationError(message) {}
};

// Brain-specific exceptions
class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError(message, "Brain") {}
};

class BrainInitializationError : public BrainError {
public:
    explicit BrainInitializationError(const std::string& message) : BrainError(message) {}
};

class BrainValidationError : public BrainError {
public:
    explicit BrainValidationError(const std::string& message) : BrainError(message) {}
};

// System integration exceptions
class SystemIntegrationError : public NLMError {
public:
    explicit SystemIntegrationError(const std::string& message) : NLMError(message, "System Integration") {}
};

class MemorySystemError : public SystemIntegrationError {
public:
    explicit MemorySystemError(const std::string& message) : SystemIntegrationError(message) {}
};

class PredictionSystemError : public SystemIntegrationError {
public:
    explicit PredictionSystemError(const std::string& message) : SystemIntegrationError(message) {}
};

class CognitionSystemError : public SystemIntegrationError {
public:
    explicit CognitionSystemError(const std::string& message) : SystemIntegrationError(message) {}
};

class NeuromodulationSystemError : public SystemIntegrationError {
public:
    explicit NeuromodulationSystemError(const std::string& message) : SystemIntegrationError(message) {}
};

// Resource validation exceptions
class ResourceError : public NLMError {
public:
    explicit ResourceError(const std::string& message) : NLMError(message, "Resources") {}
};

class MemoryAllocationError : public ResourceError {
public:
    explicit MemoryAllocationError(const std::string& message) : ResourceError(message) {}
};

class FileHandleError : public ResourceError {
public:
    explicit FileHandleError(const std::string& message) : ResourceError(message) {}
};

// Exception factory for better error reporting
class ExceptionManager {
public:
    static void registerExceptionType(const std::string& typeName, std::function<std::exception_ptr()> factory) {
        exceptionFactories_[typeName] = factory;
    }
    
    static std::exception_ptr createException(const std::string& typeName, const std::string& message) {
        auto it = exceptionFactories_.find(typeName);
        if (it != exceptionFactories_.end()) {
            try {
                return it->second();
            } catch (...) {
                // Fallback to runtime_error if factory fails
            }
        }
        return std::make_exception_ptr<NLMError>(message);
    }
    
    static std::map<std::string, std::function<std::exception_ptr()>> exceptionFactories_;
};

// Macro for throwing exceptions with context
#define NLM_THROW_ERROR(exception_type, message) \
    throw exception_type(message, __func__)

#define NLM_THROW_ERROR_WITH_CONTEXT(exception_type, message, context) \
    throw exception_type(message, context)

// Exception translation helper
class ExceptionTranslator {
public:
    template<typename ExceptionType>
    static void registerTranslation() {
        ExceptionManager::registerExceptionType(typeid(ExceptionType).name(),
            []() { return std::make_exception_ptr<ExceptionType>(""); });
    }
    
    static std::exception_ptr translateException(const std::exception& e) {
        // Map C++ exceptions to appropriate NLM exception types
        // This is a simplified version - in practice, you'd want a more sophisticated mapping
        try {
            throw;
        } catch (const std::bad_alloc& e) {
            return std::make_exception_ptr<MemoryAllocationError>(e.what());
        } catch (const std::invalid_argument& e) {
            return std::make_exception_ptr<NLMError>(e.what());
        } catch (const std::out_of_range& e) {
            return std::make_exception_ptr<NLMError>(e.what());
        } catch (const std::runtime_error& e) {
            return std::make_exception_ptr<NLMError>(e.what());
        } catch (...) {
            return std::make_exception_ptr<NLMError>("Unknown exception occurred");
        }
    }
};

} // namespace nlm
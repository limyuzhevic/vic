# Error handling hierarchy for NLM
// Provides comprehensive exception handling for the Neural Learning Machine framework
// Follows modern C++ exception handling best practices with proper error categorization

#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <format>

namespace nlm {

// Error categories for different types of failures
enum class ErrorCategory {
    System,          // System-level errors (memory, file I/O, etc.)
    Configuration,   // Configuration-related errors
    Neural,          // Neural computation errors
    Memory,          // Memory system errors
    Plasticity,      // Plasticity/plasticity rule errors
    Integration,     // Integration/communication errors
    User,            // User input/configuration errors
    Development,     // Development stage errors
    Resource         // Resource allocation errors
};

// Base NLM exception class
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message,
                          ErrorCategory category = ErrorCategory::System,
                          const std::string& component = "NLM")
        : std::runtime_error(formatError(message, category, component)),
          category_(category),
          component_(component),
          timestamp_(std::chrono::system_clock::now()) {}

    virtual ~NLMException() noexcept = default;

    // Get error category
    ErrorCategory getCategory() const noexcept { return category_; }

    // Get component name
    const std::string& getComponent() const noexcept { return component_; }

    // Get timestamp
    std::chrono::system_clock::time_point getTimestamp() const noexcept { return timestamp_; }

    // Get formatted error message
    const std::string& getFormattedMessage() const noexcept { return formattedMessage_; }

protected:
    static std::string formatError(const std::string& message,
                                   ErrorCategory category,
                                   const std::string& component) {
        std::string categoryStr;
        switch (category) {
            case ErrorCategory::System: categoryStr = "SYSTEM"; break;
            case ErrorCategory::Configuration: categoryStr = "CONFIGURATION"; break;
            case ErrorCategory::Neural: categoryStr = "NEURAL"; break;
            case ErrorCategory::Memory: categoryStr = "MEMORY"; break;
            case ErrorCategory::Plasticity: categoryStr = "PLASTICITY"; break;
            case ErrorCategory::Integration: categoryStr = "INTEGRATION"; break;
            case ErrorCategory::User: categoryStr = "USER"; break;
            case ErrorCategory::Development: categoryStr = "DEVELOPMENT"; break;
            case ErrorCategory::Resource: categoryStr = "RESOURCE"; break;
        }

        return std::format("[{}::{}] {} - {}",
                          categoryStr, component, message, getTimestampString());
    }

    static std::string getTimestampString() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

private:
    ErrorCategory category_;
    std::string component_;
    std::chrono::system_clock::time_point timestamp_;
    std::string formattedMessage_;
};

// System-level errors
class SystemError : public NLMException {
public:
    explicit SystemError(const std::string& message,
                        const std::string& component = "System")
        : NLMException(message, ErrorCategory::System, component) {}
};

class MemoryError : public SystemError {
public:
    explicit MemoryError(const std::string& message,
                        const std::string& component = "Memory")
        : SystemError(message, component) {}
};

class FileError : public SystemError {
public:
    explicit FileError(const std::string& message,
                      const std::string& component = "FileIO")
        : SystemError(message, component) {}
};

// Configuration errors
class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message,
                               const std::string& component = "Config")
        : NLMException(message, ErrorCategory::Configuration, component) {}
};

class ConfigParseError : public ConfigurationError {
public:
    explicit ConfigParseError(const std::string& message,
                             const std::string& filepath,
                             const std::string& component = "ConfigParse")
        : ConfigurationError(std::format("Failed to parse {}: {}", filepath, message), component) {}
};

class ConfigValidationError : public ConfigurationError {
public:
    explicit ConfigValidationError(const std::string& message,
                                  const std::string& key,
                                  const std::string& component = "ConfigValidation")
        : ConfigurationError(std::format("Invalid value for key '{}': {}", key, message), component) {}
};

// Neural computation errors
class NeuralError : public NLMException {
public:
    explicit NeuralError(const std::string& message,
                        const std::string& component = "Neural")
        : NLMException(message, ErrorCategory::Neural, component) {}
};

class NeuronError : public NeuralError {
public:
    explicit NeuronError(const std::string& message,
                        const std::string& component = "Neuron")
        : NeuralError(message, component) {}
};

class SynapseError : public NeuralError {
public:
    explicit SynapseError(const std::string& message,
                         const std::string& component = "Synapse")
        : NeuralError(message, component) {}
};

// Memory system errors
class MemorySystemError : public NLMException {
public:
    explicit MemorySystemError(const std::string& message,
                              const std::string& component = "MemorySystem")
        : NLMException(message, ErrorCategory::Memory, component) {}
};

class WorkingMemoryError : public MemorySystemError {
public:
    explicit WorkingMemoryError(const std::string& message,
                               const std::string& component = "WorkingMemory")
        : MemorySystemError(message, component) {}
};

class EpisodicMemoryError : public MemorySystemError {
public:
    explicit EpisodicMemoryError(const std::string& message,
                                const std::string& component = "EpisodicMemory")
        : MemorySystemError(message, component) {}
};

// Plasticity errors
class PlasticityError : public NLMException {
public:
    explicit PlasticityError(const std::string& message,
                           const std::string& component = "Plasticity")
        : NLMException(message, ErrorCategory::Plasticity, component) {}
};

class STDPError : public PlasticityError {
public:
    explicit STDPError(const std::string& message,
                      const std::string& component = "STDP")
        : PlasticityError(message, component) {}
};

class HebbianError : public PlasticityError {
public:
    explicit HebbianError(const std::string& message,
                         const std::string& component = "Hebbian")
        : PlasticityError(message, component) {}
};

// Integration errors
class IntegrationError : public NLMException {
public:
    explicit IntegrationError(const std::string& message,
                             const std::string& component = "Integration")
        : NLMException(message, ErrorCategory::Integration, component) {}
};

class NeuromodulationError : public IntegrationError {
public:
    explicit NeuromodulationError(const std::string& message,
                                 const std::string& component = "Neuromodulation")
        : IntegrationError(message, component) {}
};

// User errors
class UserError : public NLMException {
public:
    explicit UserError(const std::string& message,
                      const std::string& component = "User")
        : NLMException(message, ErrorCategory::User, component) {}
};

class InvalidArgumentError : public UserError {
public:
    explicit InvalidArgumentError(const std::string& message,
                                 const std::string& component = "InvalidArgument")
        : UserError(message, component) {}
};

// Development errors
class DevelopmentError : public NLMException {
public:
    explicit DevelopmentError(const std::string& message,
                             const std::string& component = "Development")
        : NLMException(message, ErrorCategory::Development, component) {}
};

// Resource errors
class ResourceError : public NLMException {
public:
    explicit ResourceError(const std::string& message,
                          const std::string& component = "Resource")
        : NLMException(message, ErrorCategory::Resource, component) {}
};

class OutOfMemoryError : public ResourceError {
public:
    explicit OutOfMemoryError(const std::string& message = "Out of memory",
                             const std::string& component = "Resource")
        : ResourceError(message, component) {}
};

class ConfigFileNotFoundError : public ConfigurationError {
public:
    explicit ConfigFileNotFoundError(const std::string& filepath,
                                    const std::string& component = "ConfigFileNotFound")
        : ConfigurationError(std::format("Configuration file not found: {}", filepath), component) {}
};

// Exception safety utility functions
namespace exception_safety {

    // Safe memory allocation with exception safety
    template<typename T, typename... Args>
    std::unique_ptr<T> make_safe_unique(Args&&... args) {
        try {
            return std::make_unique<T>(std::forward<Args>(args)...);
        } catch (const std::bad_alloc&) {
            throw OutOfMemoryError("Failed to allocate memory for object");
        } catch (...) {
            throw;
        }
    }

    // Safe vector operations with exception safety
    template<typename T>
    std::vector<T> safe_resize(const std::vector<T>& original, size_t new_size) {
        try {
            std::vector<T> result;
            result.reserve(new_size);
            size_t copy_count = std::min(original.size(), new_size);
            for (size_t i = 0; i < copy_count; ++i) {
                result.push_back(original[i]);
            }
            return result;
        } catch (...) {
            throw MemoryError("Failed to resize vector safely");
        }
    }

    // Safe file operations with proper error handling
    class SafeFile {
    public:
        explicit SafeFile(const std::string& filename, std::ios_base::openmode mode = std::ios::in) {
            try {
                file_.open(filename, mode);
                if (!file_.is_open()) {
                    throw FileError(std::format("Failed to open file: {}", filename));
                }
            } catch (...) {
                throw FileError(std::format("Failed to open file safely: {}", filename));
            }
        }

        ~SafeFile() {
            try {
                if (file_.is_open()) {
                    file_.close();
                }
            } catch (...) {
                // Log error but don't throw in destructor
            }
        }

        // Prevent copying
        SafeFile(const SafeFile&) = delete;
        SafeFile& operator=(const SafeFile&) = delete;

        std::ifstream& getInputStream() { return file_; }
        std::ofstream& getOutputStream() { return file_; }
        bool isOpen() const { return file_.is_open(); }
        void close() {
            if (file_.is_open()) {
                file_.close();
            }
        }

    private:
        std::fstream file_;
    };

} // namespace exception_safety

} // namespace nlm

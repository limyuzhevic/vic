#pragma once

#include <string>
#include <stdexcept>
#include <system_error>

namespace nlm {

// NLM Error Categories
enum class NLMErrorCategory {
    Initialization,
    Validation,
    Memory,
    NeuralDynamics,
    Plasticity,
    Configuration,
    Runtime,
    FileIO
};

// Base exception class for all NLM errors
class NLMException : public std::runtime_error {
protected:
    NLMErrorCategory category_;
    std::string message_;

public:
    explicit NLMException(NLMErrorCategory category, const std::string& message)
        : std::runtime_error(message), category_(category), message_(message) {}

    NLMErrorCategory getCategory() const noexcept { return category_; }
    const char* getCategoryName() const {
        switch (category_) {
            case NLMErrorCategory::Initialization: return "Initialization";
            case NLMErrorCategory::Validation: return "Validation";
            case NLMErrorCategory::Memory: return "Memory";
            case NLMErrorCategory::NeuralDynamics: return "NeuralDynamics";
            case NLMErrorCategory::Plasticity: return "Plasticity";
            case NLMErrorCategory::Configuration: return "Configuration";
            case NLMErrorCategory::Runtime: return "Runtime";
            case NLMErrorCategory::FileIO: return "FileIO";
            default: return "Unknown";
        }
    }

    const std::string& getMessage() const noexcept { return message_; }

    virtual const char* what() const noexcept override {
        return message_.c_str();
    }
};

// Specific exception types
class NLMInitializationError : public NLMException {
public:
    explicit NLMInitializationError(const std::string& message)
        : NLMException(NLMErrorCategory::Initialization, message) {}
};

class NLMValidationError : public NLMException {
public:
    explicit NLMValidationError(const std::string& message)
        : NLMException(NLMErrorCategory::Validation, message) {}
};

class NLMMemoryError : public NLMException {
public:
    explicit NLMMemoryError(const std::string& message)
        : NLMException(NLMErrorCategory::Memory, message) {}
};

class NLMNeuralDynamicsError : public NLMException {
public:
    explicit NLMNeuralDynamicsError(const std::string& message)
        : NLMException(NLMErrorCategory::NeuralDynamics, message) {}
};

class NLMPlasticityError : public NLMException {
public:
    explicit NLMPlasticityError(const std::string& message)
        : NLMException(NLMErrorCategory::Plasticity, message) {}
};

class NLMConfigurationError : public NLMException {
public:
    explicit NLMConfigurationError(const std::string& message)
        : NLMException(NLMErrorCategory::Configuration, message) {}
};

// Input validation utilities
class ValidationUtils {
public:
    template<typename T>
    static void validatePointerNotNull(const T* ptr, const std::string& operation) {
        if (!ptr) {
            throw NLMValidationError("Null pointer detected in " + operation);
        }
    }

    template<typename T>
    static void validatePointerNotNull(const T& ptr, const std::string& operation) {
        if (!ptr) {
            throw NLMValidationError("Null pointer detected in " + operation);
        }
    }

    static void validateRange(size_t value, size_t min, size_t max, const std::string& operation) {
        if (value < min || value > max) {
            throw NLMValidationError("Invalid range in " + operation + ": " +
                                   std::to_string(value) + " not in [" +
                                   std::to_string(min) + ", " + std::to_string(max) + "]");
        }
    }

    static void validateNonNegative(float value, const std::string& operation) {
        if (value < 0.0f) {
            throw NLMValidationError("Negative value in " + operation + ": " +
                                   std::to_string(value));
        }
    }

    static void validatePositive(float value, const std::string& operation) {
        if (value <= 0.0f) {
            throw NLMValidationError("Non-positive value in " + operation + ": " +
                                   std::to_string(value));
        }
    }

    static void validateNotEmpty(const std::vector<float>& data, const std::string& operation) {
        if (data.empty()) {
            throw NLMValidationError("Empty data vector in " + operation);
        }
    }

    static void validateMemoryCapacity(size_t current, size_t capacity, const std::string& operation) {
        if (current > capacity) {
            throw NLMMemoryError("Memory capacity exceeded in " + operation + ": " +
                               std::to_string(current) + " > " + std::to_string(capacity));
        }
    }

    static void validateBrainInitialized(bool initialized, const std::string& operation) {
        if (!initialized) {
            throw NLMInitializationError("Brain not initialized in " + operation);
        }
    }

    static void validateNeuronExists(bool exists, const std::string& operation) {
        if (!exists) {
            throw NLMNeuralDynamicsError("Neuron does not exist in " + operation);
        }
    }

    static void validateSynapseExists(bool exists, const std::string& operation) {
        if (!exists) {
            throw NLMNeuralDynamicsError("Synapse does not exist in " + operation);
        }
    }

    static void validatePlasticityEnabled(bool enabled, const std::string& operation) {
        if (!enabled) {
            throw NLMPlasticityError("Plasticity not enabled in " + operation);
        }
    }

    static void validateConfigurationKey(const std::string& key, const std::string& operation) {
        if (key.empty()) {
            throw NLMConfigurationError("Empty configuration key in " + operation);
        }
    }

    static std::string formatError(const std::string& operation, const std::string& details = "") {
        return operation + (details.empty() ? "" : ": " + details);
    }
};

// Exception safety utilities
class ExceptionSafety {
public:
    // RAII wrapper for resource management with exception safety
    template<typename T>
    class SafeResource {
        T* resource_;
        bool owns_;
        std::string operation_;

    public:
        SafeResource(T* resource, bool owns, const std::string& operation)
            : resource_(resource), owns_(owns), operation_(operation) {
            ValidationUtils::validatePointerNotNull(resource_, operation_);
        }

        ~SafeResource() {
            if (owns_ && resource_) {
                delete resource_;
            }
        }

        T* get() const { return resource_; }
        T* release() {
            T* temp = resource_;
            resource_ = nullptr;
            owns_ = false;
            return temp;
        }

        SafeResource(const SafeResource&) = delete;
        SafeResource& operator=(const SafeResource&) = delete;

        SafeResource(SafeResource&& other) noexcept
            : resource_(other.resource_), owns_(other.owns_), operation_(std::move(other.operation_)) {
            other.resource_ = nullptr;
            other.owns_ = false;
        }

        SafeResource& operator=(SafeResource&& other) noexcept {
            if (this != &other) {
                if (owns_ && resource_) {
                    delete resource_;
                }
                resource_ = other.resource_;
                owns_ = other.owns_;
                operation_ = std::move(other.operation_);
                other.resource_ = nullptr;
                other.owns_ = false;
            }
            return *this;
        }
    };

    // Exception-safe state management
    template<typename T>
    class ExceptionSafeState {
        T state_;
        bool dirty_ = false;
        std::string operation_;

    public:
        explicit ExceptionSafeState(const T& initial, const std::string& operation)
            : state_(initial), operation_(operation) {}

        void setState(const T& newState) {
            try {
                state_ = newState;
                dirty_ = true;
            } catch (...) {
                throw NLMException(NLMErrorCategory::Runtime,
                                 "Failed to set state in " + operation_);
            }
        }

        const T& getState() const { return state_; }
        bool isDirty() const { return dirty_; }
        void markClean() { dirty_ = false; }

        // Rollback capability
        void rollback() {
            // In real implementation, this would restore previous state
            dirty_ = false;
        }
    };
};

} // namespace nlm

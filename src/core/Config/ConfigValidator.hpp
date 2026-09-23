#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <type_traits>
#include <optional>
#include <variant>
#include <vector>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include <any>

namespace nlm {

// Configuration error type
class ConfigError : public std::runtime_error {
public:
    enum class ErrorType {
        InvalidType,
        MissingKey,
        InvalidValue,
        ValidationFailed,
        SchemaError,
        ConversionFailed
    };

    ConfigError(ErrorType type, const std::string& message)
        : std::runtime_error(message), errorType(type) {}

    ErrorType getErrorType() const { return errorType; }

private:
    ErrorType errorType;
};

// Base validator interface
class IValidator {
public:
    virtual ~IValidator() = default;
    virtual void validate(const std::any& value) const = 0;
    virtual std::string getErrorMessage() const = 0;
};

// Typed validator implementation
template<typename T>
class TypedValidator : public IValidator {
public:
    using ValidationFunction = std::function<bool(const T&)>;n
    using ErrorMessageFunction = std::function<std::string(const T&)>;

    TypedValidator(ValidationFunction validator, ErrorMessageFunction errorMsg)
        : validator(validator), errorMsg(errorMsg) {}

    void validate(const std::any& value) const override {
        if (validator) {
            const T& val = std::any_cast<const T&>(value);
            if (!validator(val)) {
                throw ConfigError(ConfigError::ErrorType::ValidationFailed, 
                                 errorMsg(val));
            }
        }
    }

    std::string getErrorMessage() const override {
        return "Validation failed";
    }

private:
    ValidationFunction validator;
    ErrorMessageFunction errorMsg;
};

// Generic validator for range checks
class RangeValidator : public IValidator {
public:
    template<typename T>
    RangeValidator(T min, T max, const std::string& errorMsg)
        : minVal(std::any(min)), maxVal(std::any(max)), errorMessage(errorMsg) {
        static_assert(std::is_same_v<T, int> || std::is_same_v<T, double> || 
                     std::is_same_v<T, int64_t>, "RangeValidator only supports numeric types");
    }

    void validate(const std::any& value) const override {
        if (minVal.has_value() && maxVal.has_value()) {
            if (value.type() == typeid(int)) {
                int val = std::any_cast<int>(value);
                if (val < std::any_cast<int>(minVal) || val > std::any_cast<int>(maxVal)) {
                    throw ConfigError(ConfigError::ErrorType::ValidationFailed, errorMessage);
                }
            } else if (value.type() == typeid(double)) {
                double val = std::any_cast<double>(value);
                if (val < std::any_cast<double>(minVal) || val > std::any_cast<double>(maxVal)) {
                    throw ConfigError(ConfigError::ErrorType::ValidationFailed, errorMessage);
                }
            } else if (value.type() == typeid(int64_t)) {
                int64_t val = std::any_cast<int64_t>(value);
                if (val < std::any_cast<int64_t>(minVal) || val > std::any_cast<int64_t>(maxVal)) {
                    throw ConfigError(ConfigError::ErrorType::ValidationFailed, errorMessage);
                }
            }
        }
    }

    std::string getErrorMessage() const override { return errorMessage; }

private:
    std::any minVal;
    std::any maxVal;
    std::string errorMessage;
};

// String length validator
class StringLengthValidator : public IValidator {
public:
    StringLengthValidator(size_t minLen, size_t maxLen, const std::string& errorMsg)
        : minLen(minLen), maxLen(maxLen), errorMessage(errorMsg) {}

    void validate(const std::any& value) const override {
        const std::string& val = std::any_cast<const std::string&>(value);
        if (val.size() < minLen || val.size() > maxLen) {
            throw ConfigError(ConfigError::ErrorType::ValidationFailed, errorMessage);
        }
    }

    std::string getErrorMessage() const override { return errorMessage; }

private:
    size_t minLen;
    size_t maxLen;
    std::string errorMessage;
};

// ConfigSchema definition
struct ConfigSchema {
    enum class Type {
        Integer,
        Integer64,
        Double,
        Boolean,
        String,
        IntVector,
        DoubleVector,
        StringVector,
        Unknown
    };

    std::string name;
    Type type;
    std::any defaultValue;
    bool required;
    std::string description;
    std::vector<std::unique_ptr<IValidator>> validators;
    std::any minValue;
    std::any maxValue;
    std::any minLength;
    std::any maxLength;

    ConfigSchema(const std::string& name, Type type, bool required = false, 
                 const std::string& description = "")
        : name(name), type(type), required(required), description(description) {}

    template<typename T>
    ConfigSchema& withDefault(const T& value) {
        defaultValue = value;
        return *this;
    }

    template<typename T>
    ConfigSchema& withRange(T min, T max) {
        minValue = min;
        maxValue = max;
        return *this;
    }

    template<typename T>
    ConfigSchema& withLengthRange(size_t minLen, size_t maxLen) {
        minLength = minLen;
        maxLength = maxLen;
        return *this;
    }

    template<typename T>
    ConfigSchema& addValidator(const std::function<bool(const T&)>& validator,
                              const std::function<std::string(const T&)>& errorMsg) {
        validators.push_back(std::make_unique<TypedValidator<T>>(validator, errorMsg));
        return *this;
    }

    static Type getTypeFromAny(const std::any& value) {
        if (value.type() == typeid(int)) return Type::Integer;
        if (value.type() == typeid(int64_t)) return Type::Integer64;
        if (value.type() == typeid(double)) return Type::Double;
        if (value.type() == typeid(bool)) return Type::Boolean;
        if (value.type() == typeid(std::string)) return Type::String;
        if (value.type() == typeid(std::vector<int>)) return Type::IntVector;
        if (value.type() == typeid(std::vector<double>)) return Type::DoubleVector;
        if (value.type() == typeid(std::vector<std::string>)) return Type::StringVector;
        return Type::Unknown;
    }
};

// ConfigValidator class for validation logic
class ConfigValidator {
public:
    static bool validateValue(const ConfigSchema& schema, const std::any& value);
    static bool validateAll(const std::unordered_map<std::string, ConfigSchema>& schema, 
                           const std::unordered_map<std::string, std::any>& values);
    static std::string getValidationErrorMessage(const ConfigSchema& schema, 
                                                const std::any& value);
    
    // Convenience validator creators
    template<typename T>
    static std::unique_ptr<IValidator> createRangeValidator(T min, T max, 
                                                            const std::string& errorMsg) {
        return std::make_unique<RangeValidator>(min, max, errorMsg);
    }

    template<typename T>
    static std::unique_ptr<IValidator> createStringLengthValidator(size_t minLen, size_t maxLen,
                                                                   const std::string& errorMsg) {
        return std::make_unique<StringLengthValidator>(minLen, maxLen, errorMsg);
    }

    template<typename T>
    static std::unique_ptr<IValidator> createCustomValidator(
        const std::function<bool(const T&)>& validator,
        const std::function<std::string(const T&)>& errorMsg) {
        return std::make_unique<TypedValidator<T>>(validator, errorMsg);
    }
};

// ConfigBuilder for fluent configuration building
class ConfigBuilder {
public:
    ConfigBuilder& add(const std::string& key, ConfigSchema schema);
    ConfigBuilder& setDefault(const std::string& key, const std::any& value);
    std::unordered_map<std::string, ConfigSchema> build() const;
    
private:
    std::unordered_map<std::string, ConfigSchema> schemas;
    std::unordered_map<std::string, std::any> defaults;
};

// ConfigFactory for common configuration presets
class ConfigFactory {
public:
    static std::unordered_map<std::string, ConfigSchema> createNeuralNetworkSchema();
    static std::unordered_map<std::string, ConfigSchema> createSimulationSchema();
    static std::unordered_map<std::string, ConfigSchema> createLoggingSchema();
    static std::unordered_map<std::string, ConfigSchema> createMemorySchema();
};

} // namespace nlm
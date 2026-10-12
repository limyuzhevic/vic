#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <chrono>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

// Custom exception hierarchy
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string &message) : std::runtime_error(message) {}
    virtual ~NLMError() = default;
    virtual const char *category() const noexcept { return "NLMError"; }
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string &message) : NLMError(message) {}
    virtual const char *category() const noexcept { return "ValidationError"; }
};

class ParameterValidationError : public ValidationError {
public:
    ParameterValidationError(const std::string &message, const std::string &parameterName,
                            const std::string &component)
        : ValidationError(message), parameterName_(parameterName), component_(component) {}

    const std::string &getParameterName() const { return parameterName_; }
    const std::string &getComponent() const { return component_; }
    virtual const char *category() const noexcept override { return "ParameterValidationError"; }

private:
    std::string parameterName_;
    std::string component_;
};

class StateValidationError : public ValidationError {
public:
    StateValidationError(const std::string &message, const std::string &component)
        : ValidationError(message), component_(component) {}

    const std::string &getComponent() const { return component_; }
    virtual const char *category() const noexcept override { return "StateValidationError"; }

private:
    std::string component_;
};

class DependencyValidationError : public ValidationError {
public:
    DependencyValidationError(const std::string &message, const std::string &dependency)
        : ValidationError(message), dependency_(dependency) {}

    const std::string &getDependency() const { return dependency_; }
    virtual const char *category() const noexcept override { return "DependencyValidationError"; }

private:
    std::string dependency_;
};

// Validation result structure
struct ValidationResult {
    bool valid;
    std::string message;
    std::string component;
    std::string parameter;
    std::chrono::system_clock::time_point timestamp;

    ValidationResult(bool v = true, const std::string &msg = "", 
                    const std::string &comp = "", const std::string &param = "")
        : valid(v), message(msg), component(comp), parameter(param),
          timestamp(std::chrono::system_clock::now()) {}

    static ValidationResult success(const std::string &component = "",
                                    const std::string &parameter = "") {
        return ValidationResult(true, "Validation passed", component, parameter);
    }

    static ValidationResult failure(const std::string &message,
                                   const std::string &component = "",
                                   const std::string &parameter = "") {
        return ValidationResult(false, message, component, parameter);
    }
};

// Parameter constraint types
class ParameterConstraint {
public:
    virtual ~ParameterConstraint() = default;
    virtual bool validate(const ConfigValue &value) const = 0;
    virtual std::string getDescription() const = 0;
    virtual const char *typeName() const = 0;
};

class RangeConstraint : public ParameterConstraint {
public:
    RangeConstraint(double min, double max, const std::string &type = "double")
        : min_(min), max_(max), type_(type) {}

    bool validate(const ConfigValue &value) const override {
        if (type_ == "int") {
            if (auto v = std::get_if<int>(&value)) {
                return *v >= min_ && *v <= max_;
            }
        } else if (type_ == "double" || type_ == "float") {
            if (auto v = std::get_if<double>(&value)) {
                return *v >= min_ && *v <= max_;
            }
        } else if (type_ == "bool") {
            if (auto v = std::get_if<bool>(&value)) {
                return true; // Booleans are always in [0, 1] logically
            }
        }
        return false;
    }

    std::string getDescription() const override {
        return type_ + " must be between " + std::to_string(min_) + " and " + std::to_string(max_);
    }

    const char *typeName() const override { return "RangeConstraint"; }

private:
    double min_;
    double max_;
    std::string type_;
};

class EnumConstraint : public ParameterConstraint {
public:
    EnumConstraint(const std::vector<std::string> &allowedValues)
        : allowedValues_(allowedValues) {}

    bool validate(const ConfigValue &value) const override {
        if (auto v = std::get_if<std::string>(&value)) {
            return std::find(allowedValues_.begin(), allowedValues_.end(), *v) != allowedValues_.end();
        }
        return false;
    }

    std::string getDescription() const override {
        std::string desc = "Value must be one of: ";
        for (size_t i = 0; i < allowedValues_.size(); ++i) {
            if (i > 0) desc += ", ";
            desc += "\"" + allowedValues_[i] + "\"";
        }
        return desc;
    }

    const char *typeName() const override { return "EnumConstraint"; }

private:
    std::vector<std::string> allowedValues_;
};

class LengthConstraint : public ParameterConstraint {
public:
    LengthConstraint(size_t minLength, size_t maxLength)
        : minLength_(minLength), maxLength_(maxLength) {}

    bool validate(const ConfigValue &value) const override {
        if (auto v = std::get_if<std::vector<int>>(&value)) {
            return v->size() >= minLength_ && v->size() <= maxLength_;
        } else if (auto v = std::get_if<std::vector<double>>(&value)) {
            return v->size() >= minLength_ && v->size() <= maxLength_;
        } else if (auto v = std::get_if<std::vector<std::string>>(&value)) {
            return v->size() >= minLength_ && v->size() <= maxLength_;
        }
        return false;
    }

    std::string getDescription() const override {
        return "Length must be between " + std::to_string(minLength_) + " and " + std::to_string(maxLength_);
    }

    const char *typeName() const override { return "LengthConstraint"; }

private:
    size_t minLength_;
    size_t maxLength_;
};

// Validation system for parameters and state
class ValidationSystem {
public:
    ValidationSystem() : enabled_(true), validationCount_(0), failureCount_(0) {
        globalLogger_ = &Logger::getGlobal();
    }

    // Enable/disable validation
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }

    // Register parameter constraint
    void registerConstraint(const std::string &parameterName,
                           const ParameterConstraint &constraint) {
        constraints_[parameterName] = constraint;
    }

    // Validate a single parameter
    ValidationResult validateParameter(const std::string &parameterName,
                                       const ConfigValue &value,
                                       const std::string &component = "") {
        if (!enabled_) {
            return ValidationResult::success("ValidationSystem", parameterName);
        }

        validationCount_++;

        // Check type constraints
        auto it = constraints_.find(parameterName);
        if (it != constraints_.end()) {
            const auto &constraint = it->second;
            if (!constraint.validate(value)) {
                failureCount_++;
                std::string message = "Parameter \"" + parameterName + \"\" " + constraint.getDescription();
                
                // Log the validation failure
                globalLogger_->error(message, "ValidationSystem", __LINE__, __func__);
                
                return ValidationResult::failure(message, "ValidationSystem", parameterName);
            }
        }

        return ValidationResult::success("ValidationSystem", parameterName);
    }

    // Validate object state
    template<typename T>
    ValidationResult validateState(const T &object, const std::string &component,
                                  const std::function<ValidationResult(const T &)> &validator) {
        if (!enabled_) {
            return ValidationResult::success(component, "");
        }

        validationCount_++;

        ValidationResult result = validator(object);
        result.component = component;

        if (!result.valid) {
            failureCount_++;
            globalLogger_->error("State validation failed: " + result.message, 
                              component, __LINE__, __func__);
        }

        return result;
    }

    // Register state validator
    template<typename T>
    void registerStateValidator(const std::string &component,
                               const std::function<ValidationResult(const T &)> &validator) {
        stateValidators_[component] = [validator](const void *obj) -> ValidationResult {
            return validator(*static_cast<const T *>(obj));
        };
    }

    // Validate object state
    ValidationResult validateObjectState(const void *object, const std::string &component) {
        if (!enabled_) {
            return ValidationResult::success(component, "");
        }

        auto it = stateValidators_.find(component);
        if (it != stateValidators_.end()) {
            return it->second(object);
        }

        return ValidationResult::success(component, "");
    }

    // Register dependency validator
    void registerDependencyValidator(const std::string &dependentComponent,
                                     const std::string &requiredComponent) {
        dependencyGraph_[dependentComponent].insert(requiredComponent);
    }

    // Validate dependencies
    ValidationResult validateDependencies(const std::string &component,
                                         const std::unordered_map<std::string, bool> &componentStates) {
        if (!enabled_) {
            return ValidationResult::success("ValidationSystem", component);
        }

        auto it = dependencyGraph_.find(component);
        if (it == dependencyGraph_.end()) {
            return ValidationResult::success("ValidationSystem", component);
        }

        for (const auto &dep : it->second) {
            auto stateIt = componentStates.find(dep);
            if (stateIt == componentStates.end() || !stateIt->second) {
                failureCount_++;
                std::string message = "Component \"" + component + \"\" \" requires component \"" + dep + \"\" \" to be active";
                
                globalLogger_->error(message, "ValidationSystem", __LINE__, __func__);
                
                return ValidationResult::failure(message, "ValidationSystem", component);
            }
        }

        return ValidationResult::success("ValidationSystem", component);
    }

    // Get validation statistics
    struct ValidationStats {
        uint64_t validationCount = 0;
        uint64_t failureCount = 0;
        double failureRate() const {
            return validationCount > 0 ? (double)failureCount / validationCount : 0.0;
        }
    };

    ValidationStats getStats() const {
        ValidationStats stats;
        stats.validationCount = validationCount_;
        stats.failureCount = failureCount_;
        return stats;
    }

    // Clear statistics
    void resetStats() {
        validationCount_ = 0;
        failureCount_ = 0;
    }

private:
    bool enabled_;
    Logger *globalLogger_;
    uint64_t validationCount_;
    uint64_t failureCount_;
    std::unordered_map<std::string, ParameterConstraint> constraints_;
    std::unordered_map<std::string, std::function<ValidationResult(const void *)>> stateValidators_;
    std::unordered_map<std::string, std::unordered_set<std::string>> dependencyGraph_;
};

// Global validation system instance
inline ValidationSystem &getValidationSystem() {
    static ValidationSystem instance;
    return instance;
}

// Macros for runtime validation
#define NLM_VALIDATE_PARAM(name, value, constraint) \
    nlm::getValidationSystem().validateParameter(name, value, constraint)

#define NLM_VALIDATE_STATE(component, object, validator) \
    nlm::getValidationSystem().validateState(component, object, validator)

#define NLM_VALIDATE_DEPENDENCIES(component, componentStates) \
    nlm::getValidationSystem().validateDependencies(component, componentStates)

#define NLM_THROW_VALIDATION_ERROR(msg, param, component) \
    throw nlm::ParameterValidationError(msg, param, component)

#define NLM_ASSERT_PARAM(condition, msg, param, component) \
    if (!(condition)) throw nlm::ParameterValidationError(msg, param, component)

#define NLM_ASSERT_STATE(condition, msg, component) \
    if (!(condition)) throw nlm::StateValidationError(msg, component)

#define NLM_VALIDATE_THROW(condition, msg, param, component) \
    do { \
        ValidationResult result = nlm::getValidationSystem().validateParameter(param, std::string(""), component); \
        if (!result.valid) { \
            throw nlm::ParameterValidationError(result.message, param, component); \
        } \
    } while(0)

} // namespace nlm
// Configuration validation base classes and validators
#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>
#include <functional>
#include <memory>
#include <regex>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <limits>

#include "ValidationExceptions.hpp"

namespace nlm {

// Forward declaration
class ConfigSchema;

// Validation context for validators
struct ValidationContext {
    std::shared_ptr<Config> config = nullptr;
    std::string parameterName;
    std::any actualValue;
    const ConfigSchema* schema = nullptr;
    
    // Helper to get value by type
    template<typename T>
    T getValueAs() const {
        if (actualValue.type() == typeid(T)) {
            return std::any_cast<T>(actualValue);
        }
        throw InvalidConfigurationValue(
            "Type mismatch: expected " + typeid(T).name() + ", got " + actualValue.type().name(),
            parameterName,
            typeid(T).name(),
            actualValue.type().name()
        );
    }
};

// Base validator interface
class Validator {
public:
    virtual ~Validator() = default;
    virtual bool validate(const ValidationContext& context) const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getErrorMessage(const ValidationContext& context) const = 0;
};

// Range validator for numeric types
class RangeValidator : public Validator {
public:
    enum class BoundType { Inclusive, Exclusive };
    
    RangeValidator(double minValue, double maxValue, 
                   BoundType minType = BoundType::Inclusive,
                   BoundType maxType = BoundType::Inclusive,
                   const std::string& description = "")
        : minValue_(minValue), maxValue_(maxValue), 
          minType_(minType), maxType_(maxType),
          description_(description.empty() ? "Value must be in range [" + std::to_string(minValue) + ", " + std::to_string(maxValue) + "]" : description) {}
    
    bool validate(const ValidationContext& context) const override {
        try {
            double value = context.getValueAs<double>();
            
            bool minValid = (minType_ == BoundType::Inclusive) ? (value >= minValue_) : (value > minValue_);
            bool maxValid = (maxType_ == BoundType::Inclusive) ? (value <= maxValue_) : (value < maxValue_);
            
            return minValid && maxValid;
        } catch (const InvalidConfigurationValue&) {
            return false;
        }
    }
    
    std::string getDescription() const override { return description_; }
    
    std::string getErrorMessage(const ValidationContext& context) const override {
        try {
            double value = context.getValueAs<double>();
            
            std::stringstream ss;
            ss << "Value '" << value << "' is out of range. ";
            
            bool minValid = (minType_ == BoundType::Inclusive) ? (value >= minValue_) : (value > minValue_);
            bool maxValid = (maxType_ == BoundType::Inclusive) ? (value <= maxValue_) : (value < maxValue_);
            
            if (!minValid) {
                ss << "Must be ";
                if (minType_ == BoundType::Inclusive) ss << ">= ";
                else ss << "> ";
                ss << minValue_;
            }
            
            if (!maxValid) {
                if (!minValid) ss << " and ";
                ss << "must be ";
                if (maxType_ == BoundType::Inclusive) ss << "<= ";
                else ss << "< ";
                ss << maxValue_;
            }
            
            return ss.str();
        } catch (const InvalidConfigurationValue&) {
            return "Value has incorrect type for range validation";
        }
    }
    
private:
    double minValue_;
    double maxValue_;
    BoundType minType_;
    BoundType maxType_;
    std::string description_;
};

// String validator for pattern and constraint validation
class StringValidator : public Validator {
public:
    enum class PatternType { Regex, Contains, StartsWith, EndsWith };
    
    StringValidator(const std::string& pattern, PatternType patternType = PatternType::Regex,
                    size_t minLength = 0, size_t maxLength = std::numeric_limits<size_t>::max(),
                    const std::string& description = "")
        : pattern_(pattern), patternType_(patternType),
          minLength_(minLength), maxLength_(maxLength),
          description_(description.empty() ? "String validation failed" : description) {}
    
    bool validate(const ValidationContext& context) const override {
        try {
            std::string value = context.getValueAs<std::string>();
            
            // Check length constraints
            if (value.length() < minLength_ || value.length() > maxLength_) {
                return false;
            }
            
            // Check pattern constraints
            switch (patternType_) {
                case PatternType::Regex:
                    return std::regex_match(value, std::regex(pattern_));
                case PatternType::Contains:
                    // For contains, we need to specify what's being searched for
                    // This is a simplified version - in practice, you might want to pass the contains value
                    return true;  // Placeholder - actual implementation would need to store contains value
                case PatternType::StartsWith:
                    // This is also a simplified version
                    return true;
                case PatternType::EndsWith:
                    return true;
            }
        } catch (const InvalidConfigurationValue&) {
            return false;
        }
        return true;
    }
    
    std::string getDescription() const override { return description_; }
    
    std::string getErrorMessage(const ValidationContext& context) const override {
        try {
            std::string value = context.getValueAs<std::string>();
            
            std::stringstream ss;
            ss << "String value '" << value << "' is invalid. ";
            
            // Check length constraints
            if (value.length() < minLength_ || value.length() > maxLength_) {
                ss << "Length must be between " << minLength_ << " and " << maxLength_ << ".";
                return ss.str();
            }
            
            // Pattern validation would go here
            return ss.str() + "Pattern validation failed.";
        } catch (const InvalidConfigurationValue&) {
            return "String validation failed - value is not a string";
        }
    }
    
private:
    std::string pattern_;
    PatternType patternType_;
    size_t minLength_;
    size_t maxLength_;
    std::string description_;
};

// List validator for collection constraints
class ListValidator : public Validator {
public:
    ListValidator(size_t minSize = 0, size_t maxSize = std::numeric_limits<size_t>::max(),
                  const std::string& description = "")
        : minSize_(minSize), maxSize_(maxSize),
          description_(description.empty() ? "List validation failed" : description) {}
    
    bool validate(const ValidationContext& context) const override {
        try {
            // Check if it's a vector type - we need to handle different vector types
            // This is a simplified implementation
            auto value = context.getValueAs<std::vector<double>>(); // Try common type
            return value.size() >= minSize_ && value.size() <= maxSize_;
        } catch (const InvalidConfigurationValue&) {
            try {
                auto value = context.getValueAs<std::vector<int>>();
                return value.size() >= minSize_ && value.size() <= maxSize_;
            } catch (const InvalidConfigurationValue&) {
                try {
                    auto value = context.getValueAs<std::vector<std::string>>();
                    return value.size() >= minSize_ && value.size() <= maxSize_;
                } catch (const InvalidConfigurationValue&) {
                    return false;
                }
            }
        }
    }
    
    std::string getDescription() const override { return description_; }
    
    std::string getErrorMessage(const ValidationContext& context) const override {
        try {
            // Try to get the vector to display its size
            std::stringstream ss;
            
            try {
                auto value = context.getValueAs<std::vector<double>>();
                ss << "List size is " << value.size() << ", but must be between " << minSize_ << " and " << maxSize_ << ".";
                return ss.str();
            } catch (const InvalidConfigurationValue&) {}
            
            try {
                auto value = context.getValueAs<std::vector<int>>();
                ss << "List size is " << value.size() << ", but must be between " << minSize_ << " and " << maxSize_ << ".";
                return ss.str();
            } catch (const InvalidConfigurationValue&) {}
            
            try {
                auto value = context.getValueAs<std::vector<std::string>>();
                ss << "List size is " << value.size() << ", but must be between " << minSize_ << " and " << maxSize_ << ".";
                return ss.str();
            } catch (const InvalidConfigurationValue&) {}
            
            return "List validation failed - value is not a list";
        } catch (const InvalidConfigurationValue&) {
            return "List validation failed";
        }
    }
    
private:
    size_t minSize_;
    size_t maxSize_;
    std::string description_;
};

// Configuration schema definition
class ConfigSchema {
public:
    struct ParameterInfo {
        std::string name;
        std::string type;
        bool required = false;
        std::string description;
        std::string defaultValue;
        std::vector<std::unique_ptr<Validator>> validators;
        std::map<std::string, std::string> metadata;
        
        // Conditional validation - this parameter depends on another
        std::string conditionalOnParameter;
        std::string conditionalValue;
        
        ParameterInfo() = default;
        ParameterInfo(const std::string& name, const std::string& type,
                     bool required = false, const std::string& description = "")
            : name(name), type(type), required(required), description(description) {}
        
        void addValidator(std::unique_ptr<Validator> validator) {
            validators.push_back(std::move(validator));
        }
        
        bool validate(const ValidationContext& context) const {
            bool isValid = true;
            for (const auto& validator : validators) {
                if (!validator->validate(context)) {
                    isValid = false;
                }
            }
            return isValid;
        }
        
        std::string getErrorMessages(const ValidationContext& context) const {
            std::string errors;
            for (const auto& validator : validators) {
                if (!validator->validate(context)) {
                    if (!errors.empty()) errors += "\n";
                    errors += validator->getErrorMessage(context);
                }
            }
            return errors;
        }
    };
    
    ConfigSchema(const std::string& name = "") : name_(name) {}
    
    void setName(const std::string& name) { name_ = name; }
    const std::string& getName() const { return name_; }
    
    void addParameter(const ParameterInfo& param) {
        parameters_[param.name] = param;
    }
    
    bool hasParameter(const std::string& name) const {
        return parameters_.find(name) != parameters_.end();
    }
    
    const ParameterInfo* getParameter(const std::string& name) const {
        auto it = parameters_.find(name);
        return it != parameters_.end() ? &it->second : nullptr;
    }
    
    std::vector<std::string> getParameterNames() const {
        std::vector<std::string> names;
        for (const auto& pair : parameters_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get all parameters in this schema
    const std::map<std::string, ParameterInfo>& getParameters() const { return parameters_; }
    
    // Validate a complete configuration against this schema
    ValidationResult validate(const std::shared_ptr<Config>& config) const {
        ValidationResult result;
        
        for (const auto& pair : parameters_) {
            const auto& paramInfo = pair.second;
            
            if (paramInfo.required && !config->has(paramInfo.name)) {
                result.addError("Missing required parameter: " + paramInfo.name);
                continue;
            }
            
            if (!paramInfo.required && !config->has(paramInfo.name)) {
                continue; // Skip optional parameters that don't exist
            }
            
            // Create validation context
            ValidationContext context;
            context.config = config;
            context.parameterName = paramInfo.name;
            context.schema = this;
            
            // Get actual value from config
            auto valueOpt = config->get<std::string>(paramInfo.name);
            if (!valueOpt) {
                // Try different types
                try {
                    context.actualValue = config->get<int>(paramInfo.name).value();
                } catch (...) {
                    try {
                        context.actualValue = config->get<double>(paramInfo.name).value();
                    } catch (...) {
                        try {
                            context.actualValue = config->get<bool>(paramInfo.name).value();
                        } catch (...) {
                            context.actualValue = std::string(); // Empty string as fallback
                        }
                    }
                }
            } else {
                context.actualValue = *valueOpt;
            }
            
            // Validate the parameter
            if (!paramInfo.validate(context)) {
                std::string errorMsg = "Parameter '" + paramInfo.name + "': " + paramInfo.getErrorMessages(context);
                result.addError(errorMsg);
            }
        }
        
        return result;
    }
    
private:
    std::string name_;
    std::map<std::string, ParameterInfo> parameters_;
};

} // namespace nlm
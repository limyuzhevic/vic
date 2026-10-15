#include "Error.hpp"

namespace nlm {

// Global validator instance
Validator validator;

// Implement individual validator methods
ValidationResult Validator::TypeValidator::validate(const std::any& value, const std::string& fieldName) const {
    if (value.type() == typeid(void)) {
        return ValidationResult::failure("Value is not set");
    }
    if (value.type() != type_) {
        return ValidationResult::failure(std::format("Expected type {}, got {}", type_.name(), value.type().name()));
    }
    return ValidationResult::success();
}

ValidationResult Validator::RangeValidator::validate(const std::any& value, const std::string& fieldName) const {
    if (value.type() != typeid(double) && value.type() != typeid(float) &&
        value.type() != typeid(int) && value.type() != typeid(int64_t)) {
        return ValidationResult::failure("Value must be numeric for range validation");
    }
    
    double val = 0.0;
    if (value.type() == typeid(double)) {
        val = std::any_cast<double>(value);
    } else if (value.type() == typeid(float)) {
        val = static_cast<double>(std::any_cast<float>(value));
    } else if (value.type() == typeid(int)) {
        val = static_cast<double>(std::any_cast<int>(value));
    } else if (value.type() == typeid(int64_t)) {
        val = static_cast<double>(std::any_cast<int64_t>(value));
    }
    
    if (inclusive_) {
        if (val < min_ || val > max_) {
            return ValidationResult(std::format("Value {} is outside range [{}, {}]", val, min_, max_));
        }
    } else {
        if (val <= min_ || val >= max_) {
            return ValidationResult(std::format("Value {} is outside range ({}, {})", val, min_, max_));
        }
    }
    
    return ValidationResult::success();
}

ValidationResult Validator::LengthValidator::validate(const std::any& value, const std::string& fieldName) const {
    if (value.type() != typeid(std::string) && 
        value.type() != typeid(std::vector<float>) &&
        value.type() != typeid(std::vector<double>) &&
        value.type() != typeid(std::vector<int>) &&
        value.type() != typeid(std::vector<std::string>)) {
        return ValidationResult::failure("Value must be a string or container for length validation");
    }
    
    size_t len = 0;
    if (value.type() == typeid(std::string)) {
        len = std::any_cast<std::string>(value).size();
    } else if (value.type() == typeid(std::vector<float>) ||
               value.type() == typeid(std::vector<double>) ||
               value.type() == typeid(std::vector<int>) ||
               value.type() == typeid(std::vector<std::string>)) {
        // Create a temporary to get size
        if (value.type() == typeid(std::vector<float>)) {
            len = std::any_cast<std::vector<float>>(value).size();
        } else if (value.type() == typeid(std::vector<double>)) {
            len = std::any_cast<std::vector<double>>(value).size();
        } else if (value.type() == typeid(std::vector<int>)) {
            len = std::any_cast<std::vector<int>>(value).size();
        } else if (value.type() == typeid(std::vector<std::string>)) {
            len = std::any_cast<std::vector<std::string>>(value).size();
        }
    }
    
    if (inclusive_) {
        if (len < min_ || len > max_) {
            return ValidationResult(std::format("Length {} is outside range [{}, {}]", len, min_, max_));
        }
    } else {
        if (len <= min_ || len >= max_) {
            return ValidationResult(std::format("Length {} is outside range ({}, {})", len, min_, max_));
        }
    }
    
    return ValidationResult::success();
}

ValidationResult Validator::PatternValidator::validate(const std::any& value, const std::string& fieldName) const {
    if (value.type() != typeid(std::string)) {
        return ValidationResult::failure("Value must be a string for pattern validation");
    }
    
    const std::string& strValue = std::any_cast<std::string>(value);
    return ValidationHelpers::validatePattern(strValue, pattern_);
}

ValidationResult Validator::PositiveValidator::validate(const std::any& value, const std::string& fieldName) const {
    if (value.type() != typeid(double) && value.type() != typeid(float) &&
        value.type() != typeid(int) && value.type() != typeid(int64_t)) {
        return ValidationResult::failure("Value must be numeric for positive validation");
    }
    
    double val = 0.0;
    if (value.type() == typeid(double)) {
        val = std::any_cast<double>(value);
    } else if (value.type() == typeid(float)) {
        val = static_cast<double>(std::any_cast<float>(value));
    } else if (value.type() == typeid(int)) {
        val = static_cast<double>(std::any_cast<int>(value));
    } else if (value.type() == typeid(int64_t)) {
        val = static_cast<double>(std::any_cast<int64_t>(value));
    }
    
    if (allowZero_) {
        if (val < 0.0) {
            return ValidationResult(std::format("Value {} must be non-negative", val));
        }
    } else {
        if (val <= 0.0) {
            return ValidationResult(std::format("Value {} must be positive", val));
        }
    }
    
    return ValidationResult::success();
}

ValidationResult Validator::NonNegativeValidator::validate(const std::any& value, const std::string& fieldName) const {
    if (value.type() != typeid(double) && value.type() != typeid(float) &&
        value.type() != typeid(int) && value.type() != typeid(int64_t)) {
        return ValidationResult::failure("Value must be numeric for non-negative validation");
    }
    
    double val = 0.0;
    if (value.type() == typeid(double)) {
        val = std::any_cast<double>(value);
    } else if (value.type() == typeid(float)) {
        val = static_cast<double>(std::any_cast<float>(value));
    } else if (value.type() == typeid(int)) {
        val = static_cast<double>(std::any_cast<int>(value));
    } else if (value.type() == typeid(int64_t)) {
        val = static_cast<double>(std::any_cast<int64_t>(value));
    }
    
    if (val < 0.0) {
        return ValidationResult(std::format("Value {} must be non-negative", val));
    }
    
    return ValidationResult::success();
}

template<typename T>
ValidationResult Validator::InListValidator::validate(const std::any& value, const std::string& fieldName) const {
    if (value.type() != typeid(T)) {
        return ValidationResult(std::format("Expected type {} for in-list validation", typeid(T).name()));
    }
    
    const auto& list = std::any_cast<const std::vector<T>&>(values_);
    const T& val = std::any_cast<const T&>(value);
    
    if (std::find(list.begin(), list.end(), val) == list.end()) {
        std::ostringstream oss;
        oss << "Value " << val << " is not in the allowed list";
        return ValidationResult(oss.str());
    }
    
    return ValidationResult::success();
}

ValidationResult Validator::CustomValidator::validate(const std::any& value, const std::string& fieldName) const {
    return validator_(value);
}

} // namespace nlm

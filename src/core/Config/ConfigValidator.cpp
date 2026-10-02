#include "ConfigValidator.hpp"
#include <sstream>
#include <iomanip>
#include <regex>

namespace nlm {

ConfigValidator::ConfigValidator() : totalValidations_(0), passedValidations_(0), failedValidations_(0) {}

ConfigValidator::~ConfigValidator() = default;

std::vector<std::string> ConfigValidator::validate(const std::string& key,
                                                 const std::variant<int, int64_t, double, bool, std::string>& value,
                                                 const std::string& type,
                                                 const std::optional<double>& min,
                                                 const std::optional<double>& max,
                                                 const std::string& pattern) {
    std::vector<std::string> errors;
    totalValidations_++;
    
    // Validate type
    std::vector<std::string> typeErrors = validateType(key, value, type);
    errors.insert(errors.end(), typeErrors.begin(), typeErrors.end());
    
    // Validate range
    if (min.has_value() && max.has_value()) {
        std::vector<std::string> rangeErrors = validateRange(key, value, min.value(), max.value());
        errors.insert(errors.end(), rangeErrors.begin(), rangeErrors.end());
    }
    
    // Validate pattern
    if (!pattern.empty()) {
        std::vector<std::string> patternErrors = validatePattern(key, value, pattern);
        errors.insert(errors.end(), patternErrors.begin(), patternErrors.end());
    }
    
    if (errors.empty()) {
        passedValidations_++;
    } else {
        failedValidations_++;
    }
    
    lastValidationTime_ = std::chrono::system_clock::now();
    return errors;
}

std::vector<std::string> ConfigValidator::validateRange(const std::string& key,
                                                       const std::variant<int, int64_t, double, bool, std::string>& value,
                                                       double min, double max) {
    std::vector<std::string> errors;
    
    double numValue = extractNumericValue(value);
    if (numValue == -std::numeric_limits<double>::max()) {
        errors.push_back("Field '" + key + "' is not numeric and cannot be validated for range");
        return errors;
    }
    
    if (numValue < min) {
        errors.push_back(getRangeValidationErrors(key, numValue, min, max));
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::validatePattern(const std::string& key,
                                                         const std::variant<int, int64_t, double, bool, std::string>& value,
                                                         const std::string& pattern) {
    if (!std::holds_alternative<std::string>(value)) {
        return {"Field '" + key + "' pattern validation requires a string value"};
    }
    
    std::string strValue = std::get<std::string>(value);
    return getDefaultPatternValidaitonErrors(key, pattern, strValue);
}

std::vector<std::string> ConfigValidator::validateCrossParameters(const std::unordered_map<std::string, 
                                             std::variant<int, int64_t, double, bool, std::string>>& values,
                                            const std::vector<std::function<bool(const std::unordered_map<std::string, 
                                             std::variant<int, int64_t, double, bool, std::string>>&)>>& validators) {
    std::vector<std::string> errors;
    for (const auto& validator : validators) {
        if (!validator(values)) {
            errors.push_back("Cross-parameter validation failed");
        }
    }
    return errors;
}

std::vector<std::string> ConfigValidator::validateCustom(const std::string& key,
                                                       const std::variant<int, int64_t, double, bool, std::string>& value,
                                                       std::function<bool(const std::variant<int, int64_t, double, bool, std::string>&)> validator,
                                                       const std::string& errorMessage) {
    if (validator(value)) {
        return {};
    }
    return {errorMessage};
}

std::vector<std::string> ConfigValidator::validateRequired(const std::string& key,
                                                         bool isProvided,
                                                         const std::string& errorMessage) {
    if (isProvided) {
        return {};
    }
    return {errorMessage};
}

std::vector<std::string> ConfigValidator::validateType(const std::string& key,
                                                     const std::variant<int, int64_t, double, bool, std::string>& value,
                                                     const std::string& expectedType) {
    return getTypeValidationErrors(key, value, expectedType);
}

std::string ConfigValidator::formatValue(const std::string& key,
                                        const std::variant<int, int64_t, double, bool, std::string>& value,
                                        const std::string& type,
                                        const std::function<std::string(const std::variant<int, int64_t, double, bool, std::string>&)>& formatter) {
    if (formatter) {
        return formatter(value);
    }
    
    std::ostringstream oss;
    if (std::holds_alternative<int>(value)) {
        oss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::fixed << std::setprecision(6) << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        oss << std::get<std::string>(value);
    }
    
    return oss.str();
}

void ConfigValidator::resetStatistics() {
    totalValidations_ = 0;
    passedValidations_ = 0;
    failedValidations_ = 0;
    lastValidationTime_ = std::chrono::system_clock::now();
}

std::vector<std::string> ConfigValidator::getDefaultPatternValidaitonErrors(const std::string& key,
                                                                       const std::string& pattern,
                                                                       const std::string& value) {
    std::vector<std::string> errors;
    
    try {
        std::regex regex(pattern);
        if (!std::regex_match(value, regex)) {
            errors.push_back("Field '" + key + "' value '" + value + "' does not match pattern '" + pattern + "'");
        }
    } catch (const std::regex_error& e) {
        errors.push_back("Invalid pattern for field '" + key + "': " + e.what());
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::getRangeValidationErrors(const std::string& key,
                                                                 double value,
                                                                 double min,
                                                                 double max) {
    std::ostringstream oss;
    oss << "Field '" << key << "' value " << std::fixed << std::setprecision(6) << value;
    oss << " is out of range [" << min << ", " << max << "]";
    return {oss.str()};
}

std::vector<std::string> ConfigValidator::getTypeValidationErrors(const std::string& key,
                                                                const std::variant<int, int64_t, double, bool, std::string>& value,
                                                                const std::string& expectedType) {
    if (expectedType.empty() || expectedType == "auto" || expectedType == "any") {
        return {};
    }
    
    std::string actualType;
    if (std::holds_alternative<int>(value)) actualType = "int";
    else if (std::holds_alternative<int64_t>(value)) actualType = "int64";
    else if (std::holds_alternative<double>(value)) actualType = "double";
    else if (std::holds_alternative<bool>(value)) actualType = "bool";
    else if (std::holds_alternative<std::string>(value)) actualType = "string";
    
    if (actualType != expectedType) {
        std::ostringstream oss;
        oss << "Field '" << key << "' has type '" << actualType << "', expected '" << expectedType << "'";
        return {oss.str()};
    }
    
    return {};
}

bool ConfigValidator::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos;
    try {
        double val = std::stod(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool ConfigValidator::isBoolean(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || lower == "1" || lower == "0";
}

std::string ConfigValidator::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool ConfigValidator::matchesPattern(const std::string& value, const std::string& pattern) {
    try {
        std::regex regex(pattern);
        return std::regex_match(value, regex);
    } catch (...) {
        return false;
    }
}

double ConfigValidator::extractNumericValue(const std::variant<int, int64_t, double, bool, std::string>& value) const {
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    }
    return -std::numeric_limits<double>::max();
}

} // namespace nlm
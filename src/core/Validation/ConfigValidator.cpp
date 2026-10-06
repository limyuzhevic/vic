// ConfigValidator implementation
#include "ConfigValidator.hpp"

namespace nlm {

ValidationResult ConfigValidator::validateAll() const {
    clearErrors();
    
    // Run all validation checks
    ValidationResult typeResult = validateTypes();
    ValidationResult rangeResult = validateRanges();
    ValidationResult dependencyResult = validateDependencies();
    ValidationResult consistencyResult = validateConsistency();
    
    // Aggregate results
    ValidationResult result(true, errors_, warnings_, info_);
    result.valid = typeResult.valid && rangeResult.valid && 
                   dependencyResult.valid && consistencyResult.valid;
    
    return result;
}

ValidationResult ConfigValidator::validateTypes() const {
    ValidationResult result(true, errors_, warnings_, info_);
    
    // Type validation for each key in configuration
    for (const auto& entry : config_.getKeys()) {
        ValidationResult keyResult = validateType(entry);
        
        if (!keyResult.valid) {
            result.valid = false;
            result.errors.insert(result.errors.end(), 
                                keyResult.errors.begin(), keyResult.errors.end());
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateRanges() const {
    ValidationResult result(true, errors_, warnings_, info_);
    
    // Range validation for each key in configuration
    for (const auto& entry : config_.getKeys()) {
        ValidationResult keyResult = validateRange(entry);
        
        if (!keyResult.valid) {
            result.valid = false;
            result.errors.insert(result.errors.end(), 
                                keyResult.errors.begin(), keyResult.errors.end());
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateDependencies() const {
    ValidationResult result(true, errors_, warnings_, info_);
    
    // Dependency validation for each key in configuration
    for (const auto& entry : config_.getKeys()) {
        ValidationResult keyResult = validateDependency(entry);
        
        if (!keyResult.valid) {
            result.valid = false;
            result.errors.insert(result.errors.end(), 
                                keyResult.errors.begin(), keyResult.errors.end());
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateConsistency() const {
    ValidationResult result(true, errors_, warnings_, info_);
    
    // Consistency validation for each key in configuration
    for (const auto& entry : config_.getKeys()) {
        ValidationResult keyResult = validateConsistency(entry);
        
        if (!keyResult.valid) {
            result.valid = false;
            result.errors.insert(result.errors.end(), 
                                keyResult.errors.begin(), keyResult.errors.end());
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateType(const std::string& key) const {
    ValidationResult result(true);
    
    if (!config_.has(key)) {
        return result;
    }
    
    try {
        std::optional<int> intVal = config_.get<int>(key);
        if (intVal.has_value()) {
            addInfo("Key '" + key + "' is of type int");
            return result;
        }
    } catch (...) {}
    
    try {
        std::optional<double> doubleVal = config_.get<double>(key);
        if (doubleVal.has_value()) {
            addInfo("Key '" + key + "' is of type double");
            return result;
        }
    } catch (...) {}
    
    try {
        std::optional<bool> boolVal = config_.get<bool>(key);
        if (boolVal.has_value()) {
            addInfo("Key '" + key + "' is of type bool");
            return result;
        }
    } catch (...) {}
    
    try {
        std::optional<std::string> stringVal = config_.get<std::string>(key);
        if (stringVal.has_value()) {
            addInfo("Key '" + key + "' is of type string");
            return result;
        }
    } catch (...) {}
    
    try {
        std::optional<std::vector<int>> intVecVal = config_.get<std::vector<int>>(key);
        if (intVecVal.has_value()) {
            addInfo("Key '" + key + "' is of type vector<int>");
            return result;
        }
    } catch (...) {}
    
    try {
        std::optional<std::vector<double>> doubleVecVal = config_.get<std::vector<double>>(key);
        if (doubleVecVal.has_value()) {
            addInfo("Key '" + key + "' is of type vector<double>");
            return result;
        }
    } catch (...) {}
    
    try {
        std::optional<std::vector<std::string>> stringVecVal = config_.get<std::vector<std::string>>(key);
        if (stringVecVal.has_value()) {
            addInfo("Key '" + key + "' is of type vector<string>");
            return result;
        }
    } catch (...) {}
    
    // If we reach here, type validation failed
    result.valid = false;
    result.errors.push_back("Key '" + key + "' has invalid type");
    
    return result;
}

ValidationResult ConfigValidator::validateRange(const std::string& key) const {
    ValidationResult result(true);
    
    if (!config_.has(key)) {
        return result;
    }
    
    // Get value based on type
    bool isValid = false;
    
    try {
        std::optional<int> intVal = config_.get<int>(key);
        if (intVal.has_value()) {
            isValid = isValueWithinRange(key, static_cast<double>(*intVal));
            if (!isValid) {
                result.valid = false;
                result.errors.push_back("Key '" + key + "' value " + std::to_string(*intVal) + " is out of range");
            }
            return result;
        }
    } catch (...) {}
    
    try {
        std::optional<double> doubleVal = config_.get<double>(key);
        if (doubleVal.has_value()) {
            isValid = isValueWithinRange(key, *doubleVal);
            if (!isValid) {
                result.valid = false;
                result.errors.push_back("Key '" + key + "' value " + std::to_string(*doubleVal) + " is out of range");
            }
            return result;
        }
    } catch (...) {}
    
    try {
        std::optional<float> floatVal = config_.get<float>(key);
        if (floatVal.has_value()) {
            isValid = isValueWithinRange(key, static_cast<double>(*floatVal));
            if (!isValid) {
                result.valid = false;
                result.errors.push_back("Key '" + key + "' value " + std::to_string(*floatVal) + " is out of range");
            }
            return result;
        }
    } catch (...) {}
    
    // For array types, check each element
    try {
        std::optional<std::vector<int>> intVecVal = config_.get<std::vector<int>>(key);
        if (intVecVal.has_value()) {
            for (size_t i = 0; i < intVecVal->size(); ++i) {
                isValid = isValueWithinRange(key + "[" + std::to_string(i) + "]", 
                                           static_cast<double>((*intVecVal)[i]));
                if (!isValid) {
                    result.valid = false;
                    result.errors.push_back("Key '" + key + "' array element " + 
                                          std::to_string(i) + " is out of range");
                    break;
                }
            }
            return result;
        }
    } catch (...) {}
    
    try {
        std::optional<std::vector<double>> doubleVecVal = config_.get<std::vector<double>>(key);
        if (doubleVecVal.has_value()) {
            for (size_t i = 0; i < doubleVecVal->size(); ++i) {
                isValid = isValueWithinRange(key + "[" + std::to_string(i) + "]", (*doubleVecVal)[i]);
                if (!isValid) {
                    result.valid = false;
                    result.errors.push_back("Key '" + key + "' array element " + 
                                          std::to_string(i) + " is out of range");
                    break;
                }
            }
            return result;
        }
    } catch (...) {}
    
    return result;
}

ValidationResult ConfigValidator::validateDependency(const std::string& key) const {
    ValidationResult result(true);
    
    if (!config_.has(key)) {
        return result;
    }
    
    if (!checkDependency(key)) {
        result.valid = false;
        result.errors.push_back("Key '" + key + "' has missing dependencies");
    }
    
    return result;
}

ValidationResult ConfigValidator::validateConsistency(const std::string& key) const {
    ValidationResult result(true);
    
    if (!config_.has(key)) {
        return result;
    }
    
    if (!checkConsistency(key)) {
        result.valid = false;
        result.errors.push_back("Key '" + key + "' has consistency issues");
    }
    
    return result;
}

ValidationResult ConfigValidator::validateAgainstSchema(const nlohmann::json& schema) const {
    return validateAgainstSchemaRecursive(schema, "");
}

bool ConfigValidator::isValidType(const std::string& key, T expectedType) const {
    // Implementation would check if value is of expected type
    // This is a simplified version - actual implementation would be more complex
    if (!config_.has(key)) {
        return false;
    }
    
    try {
        auto val = config_.get<T>(key);
        return val.has_value();
    } catch (...) {
        return false;
    }
}

bool ConfigValidator::isValueWithinRange(const std::string& key, double value) const {
    // Example range checks - would be extended with actual range definitions
    if (key.find("port") != std::string::npos) {
        return value >= 1 && value <= 65535;
    }
    
    if (key.find("weight") != std::string::npos) {
        return value >= 0.0 && value <= 1.0;
    }
    
    if (key.find("count") != std::string::npos) {
        return value >= 0;
    }
    
    // Default range check
    return true;
}

bool ConfigValidator::checkDependency(const std::string& key) const {
    // Example dependency checks - would be extended with actual dependency definitions
    if (key == "learning_rate") {
        return config_.has("network_type");
    }
    
    return true;
}

bool ConfigValidator::checkConsistency(const std::string& key) const {
    // Example consistency checks - would be extended with actual consistency rules
    if (key == "batch_size" && config_.has("learning_rate")) {
        try {
            auto batchSize = config_.get<int>("batch_size");
            auto learningRate = config_.get<double>("learning_rate");
            if (batchSize && learningRate) {
                return *batchSize > 0 && *learningRate >= 0.0 && *learningRate <= 1.0;
            }
        } catch (...) {}
    }
    
    return true;
}

ValidationResult ConfigValidator::validateAgainstSchemaRecursive(const nlohmann::json& schema,
                                                               const std::string& prefix) const {
    ValidationResult result(true);
    
    // This is a simplified implementation - would need to be extended
    // with proper recursive validation logic
    
    return result;
}

} // namespace nlm

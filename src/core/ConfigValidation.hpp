// Configuration validation system
struct ConfigValidation {
    std::string key;
    std::string type;  // "int", "double", "bool", "string"
    std::string validation;  // "range:min-max", "list:item1,item2", etc.
    std::string errorMessage;
    bool required;
};

class ConfigValidator {
public:
    ConfigValidator() = default;
    
    // Add validation rule
    void addValidation(const ConfigValidation& validation) {
        validations.push_back(validation);
    }
    
    // Validate configuration
    std::vector<std::string> validate(const Config& config) const {
        std::vector<std::string> errors;
        
        for (const auto& validation : validations) {
            if (validation.required && !config.has(validation.key)) {
                errors.push_back("Missing required configuration: " + validation.key + 
                               " (" + validation.errorMessage + ")");
                continue;
            }
            
            if (!config.has(validation.key)) {
                continue;  // Optional validation
            }
            
            // Get the value based on type
            if (validation.type == "int") {
                auto value = config.getOr<int>(validation.key, 0);
                validateInt(value, validation.validation, errors);
            } else if (validation.type == "double") {
                auto value = config.getOr<double>(validation.key, 0.0);
                validateDouble(value, validation.validation, errors);
            } else if (validation.type == "bool") {
                auto value = config.getOr<bool>(validation.key, false);
                // Boolean values don't need range validation
            } else if (validation.type == "string") {
                auto value = config.getOr<std::string>(validation.key, "");
                validateString(value, validation.validation, errors);
            }
        }
        
        return errors;
    }
    
    // Add common validation rules
    static ConfigValidation intRange(const std::string& key, int min, int max, 
                                   const std::string& errorMsg = "", bool required = true) {
        ConfigValidation validation;
        validation.key = key;
        validation.type = "int";
        validation.validation = "range:" + std::to_string(min) + "-" + std::to_string(max);
        validation.errorMessage = errorMsg.empty() ? "Value must be between " + std::to_string(min) + " and " + std::to_string(max) : errorMsg;
        validation.required = required;
        return validation;
    }
    
    static ConfigValidation doubleRange(const std::string& key, double min, double max,
                                       const std::string& errorMsg = "", bool required = true) {
        ConfigValidation validation;
        validation.key = key;
        validation.type = "double";
        validation.validation = "range:" + std::to_string(min) + "-" + std::to_string(max);
        validation.errorMessage = errorMsg.empty() ? "Value must be between " + std::to_string(min) + " and " + std::to_string(max) : errorMsg;
        validation.required = required;
        return validation;
    }
    
    static ConfigValidation stringList(const std::string& key, 
                                       const std::vector<std::string>& allowedValues,
                                       const std::string& errorMsg = "", bool required = true) {
        ConfigValidation validation;
        validation.key = key;
        validation.type = "string";
        std::string listStr;
        for (size_t i = 0; i < allowedValues.size(); ++i) {
            if (i > 0) listStr += ", ";
            listStr += "\"" + allowedValues[i] + "\"";
        }
        validation.validation = "list:" + listStr;
        validation.errorMessage = errorMsg.empty() ? "Value must be one of: " + listStr : errorMsg;
        validation.required = required;
        return validation;
    }
    
    static ConfigValidation positive(const std::string& key, const std::string& errorMsg = "", bool required = true) {
        ConfigValidation validation;
        validation.key = key;
        validation.type = "int";
        validation.validation = "positive";
        validation.errorMessage = errorMsg.empty() ? "Value must be positive" : errorMsg;
        validation.required = required;
        return validation;
    }
    
private:
    std::vector<ConfigValidation> validations;
    
    void validateInt(int value, const std::string& validation, std::vector<std::string>& errors) const {
        if (validation.substr(0, 6) == "range:") {
            size_t dashPos = validation.find('-', 6);
            if (dashPos != std::string::npos) {
                int min = std::stoi(validation.substr(6, dashPos - 6));
                int max = std::stoi(validation.substr(dashPos + 1));
                
                if (value < min || value > max) {
                    errors.push_back("Configuration '" + getKeyFromValidation(validation) + "': " + 
                                   std::to_string(value) + " is outside valid range [" + 
                                   std::to_string(min) + ", " + std::to_string(max) + "]");
                }
            }
        } else if (validation == "positive") {
            if (value <= 0) {
                errors.push_back("Configuration '" + getKeyFromValidation(validation) + "': " + 
                               std::to_string(value) + " must be positive");
            }
        }
    }
    
    void validateDouble(double value, const std::string& validation, std::vector<std::string>& errors) const {
        if (validation.substr(0, 6) == "range:") {
            size_t dashPos = validation.find('-', 6);
            if (dashPos != std::string::npos) {
                double min = std::stod(validation.substr(6, dashPos - 6));
                double max = std::stod(validation.substr(dashPos + 1));
                
                if (value < min || value > max) {
                    errors.push_back("Configuration '" + getKeyFromValidation(validation) + "': " + 
                                   std::to_string(value) + " is outside valid range [" + 
                                   std::to_string(min) + ", " + std::to_string(max) + "]");
                }
            }
        } else if (validation == "positive") {
            if (value <= 0) {
                errors.push_back("Configuration '" + getKeyFromValidation(validation) + "': " + 
                               std::to_string(value) + " must be positive");
            }
        }
    }
    
    void validateString(const std::string& value, const std::string& validation, std::vector<std::string>& errors) const {
        if (validation.substr(0, 5) == "list:") {
            std::string listStr = validation.substr(5);
            // Check if value is in the allowed list
            bool found = false;
            size_t pos = 0;
            while (pos < listStr.size()) {
                size_t quotePos = listStr.find('"', pos);
                if (quotePos == std::string::npos) break;
                
                size_t endQuotePos = listStr.find('"', quotePos + 1);
                if (endQuotePos == std::string::npos) break;
                
                std::string allowedValue = listStr.substr(quotePos + 1, endQuotePos - quotePos - 1);
                if (value == allowedValue) {
                    found = true;
                    break;
                }
                
                pos = endQuotePos + 1;
            }
            
            if (!found) {
                errors.push_back("Configuration '" + getKeyFromValidation(validation) + "': '" + 
                               value + "' is not in the allowed list");
            }
        }
    }
    
    std::string getKeyFromValidation(const std::string& validation) const {
        // Extract key from validation string (hacky approach, but works for our format)
        return validation.substr(validation.find(':') + 1);
    }
};
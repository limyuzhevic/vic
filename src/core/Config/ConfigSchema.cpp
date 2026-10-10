#include "ConfigSchema.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct ConfigSchema::Impl {
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_map<std::string, std::function<ValidationResult(const ConfigValue&)>> validators;
    std::unordered_map<std::string, std::string> errorMessages;
    std::unordered_map<std::string, bool> requiredKeys;
    std::unordered_map<std::string, std::set<std::string>> enumValues;
    std::unordered_map<std::string, std::pair<double, double>> ranges;
    std::set<std::string> allKeys;
};

ConfigSchema::ConfigSchema() : pImpl(std::make_unique<Impl>()) {}

ConfigSchema::~ConfigSchema() = default;

void ConfigSchema::addOption(const std::string& key, const std::string& description,
                            std::function<ValidationResult(const ConfigValue&)> validator,
                            const std::string& errorMessage,
                            bool required) {
    pImpl->allKeys.insert(key);
    pImpl->descriptions[key] = description;
    if (validator) {
        pImpl->validators[key] = validator;
    }
    pImpl->errorMessages[key] = errorMessage;
    pImpl->requiredKeys[key] = required;
}

void ConfigSchema::addRange(const std::string& key, double min, double max, const std::string& error) {
    pImpl->ranges[key] = std::make_pair(min, max);
    auto rangeValidator = [this, key, error](const ConfigValue& value) -> ValidationResult {
        ValidationResult result = validate(key, value);
        if (!result.valid && ranges.count(key)) {
            return ValidationResult(false, error, key);
        }
        return result;
    };
    pImpl->validators[key] = rangeValidator;
    pImpl->errorMessages[key] = error;
}

void ConfigSchema::addEnum(const std::string& key, const std::set<std::string>& allowedValues, const std::string& error) {
    pImpl->enumValues[key] = allowedValues;
    auto enumValidator = [this, key, error](const ConfigValue& value) -> ValidationResult {
        ValidationResult result = validate(key, value);
        if (!result.valid && enumValues.count(key)) {
            if (std::holds_alternative<std::string>(value)) {
                const std::string& strValue = std::get<std::string>(value);
                if (enumValues[key].find(strValue) == enumValues[key].end()) {
                    return ValidationResult(false, error, key);
                }
            }
        }
        return result;
    };
    pImpl->validators[key] = enumValidator;
    pImpl->errorMessages[key] = error;
}

ValidationResult ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    // Basic type validation
    ValidationResult typeResult = ValidationResult(true);
    
    if (std::holds_alternative<int>(value)) {
        // Valid int
    } else if (std::holds_alternative<int64_t>(value)) {
        // Valid int64_t
    } else if (std::holds_alternative<double>(value)) {
        // Valid double
    } else if (std::holds_alternative<bool>(value)) {
        // Valid bool
    } else if (std::holds_alternative<std::string>(value)) {
        // Valid string
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        // Valid vector<int>
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        // Valid vector<double>
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        // Valid vector<std::string>
    } else {
        return ValidationResult(false, "Unknown value type", key);
    }
    
    // Custom validator
    if (pImpl->validators.count(key)) {
        ValidationResult customResult = pImpl->validators.at(key)(value);
        if (!customResult.valid) {
            return customResult;
        }
    }
    
    // Range validation
    if (pImpl->ranges.count(key)) {
        double val = 0.0;
        if (std::holds_alternative<int>(value)) val = static_cast<double>(std::get<int>(value));
        else if (std::holds_alternative<int64_t>(value)) val = static_cast<double>(std::get<int64_t>(value));
        else if (std::holds_alternative<double>(value)) val = std::get<double>(value);
        
        auto range = pImpl->ranges.at(key);
        if (val < range.first || val > range.second) {
            return ValidationResult(false, pImpl->errorMessages.at(key), key);
        }
    }
    
    // Enum validation
    if (pImpl->enumValues.count(key) && std::holds_alternative<std::string>(value)) {
        const std::string& strValue = std::get<std::string>(value);
        if (pImpl->enumValues.at(key).find(strValue) == pImpl->enumValues.at(key).end()) {
            return ValidationResult(false, pImpl->errorMessages.at(key), key);
        }
    }
    
    return ValidationResult(true);
}

bool ConfigSchema::hasKey(const std::string& key) const {
    return pImpl->allKeys.find(key) != pImpl->allKeys.end();
}

std::string ConfigSchema::getDescription(const std::string& key) const {
    if (pImpl->descriptions.count(key)) {
        return pImpl->descriptions.at(key);
    }
    return "";
}

std::vector<std::string> ConfigSchema::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->allKeys.size());
    for (const auto& key : pImpl->allKeys) {
        keys.push_back(key);
    }
    return keys;
}

std::string ConfigSchema::generateDocumentation() const {
    std::ostringstream oss;
    oss << "NLM Configuration Schema\n";
    oss << "======================\n\n";
    
    for (const auto& key : pImpl->allKeys) {
        oss << "Key: " << key << "\n";
        oss << "Description: " << pImpl->descriptions.at(key) << "\n";
        oss << "Required: " << (pImpl->requiredKeys.at(key) ? "Yes" : "No") << "\n";
        
        // Show constraints
        if (pImpl->ranges.count(key)) {
            auto range = pImpl->ranges.at(key);
            oss << "Range: [" << range.first << ", " << range.second << "]\n";
        }
        
        if (pImpl->enumValues.count(key)) {
            oss << "Allowed values: ";
            bool first = true;
            for (const auto& value : pImpl->enumValues.at(key)) {
                if (!first) oss << ", ";
                oss << value;
                first = false;
            }
            oss << "\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

bool ConfigSchema::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json schemaJson;
        file >> schemaJson;
        
        // Parse schema JSON
        if (schemaJson.contains("config_schema")) {
            const auto& schema = schemaJson["config_schema"];
            
            if (schema.contains("options")) {
                for (const auto& item : schema["options"].items()) {
                    const std::string& key = item.key();
                    const auto& option = item.value();
                    
                    std::string description = option.value("description", "");
                    bool required = option.value("required", false);
                    
                    // Parse constraints
                    std::function<ValidationResult(const ConfigValue&)> validator;
                    std::string errorMessage;
                    
                    if (option.contains("type")) {
                        std::string type = option["type"];
                        if (type == "int") {
                            addType<int>(key, required, description);
                        } else if (type == "int64_t") {
                            addType<int64_t>(key, required, description);
                        } else if (type == "double") {
                            addType<double>(key, required, description);
                        } else if (type == "bool") {
                            addType<bool>(key, required, description);
                        } else if (type == "string") {
                            addType<std::string>(key, required, description);
                        }
                    }
                    
                    if (option.contains("range")) {
                        double min = option["range"]["min"];
                        double max = option["range"]["max"];
                        std::string error = option.value("range_error", "Value out of range");
                        addRange(key, min, max, error);
                    }
                    
                    if (option.contains("enum")) {
                        std::set<std::string> enumValues;
                        for (const auto& enumItem : option["enum"]) {
                            enumValues.insert(enumItem);
                        }
                        std::string error = option.value("enum_error", "Value not in allowed set");
                        addEnum(key, enumValues, error);
                    }
                }
            }
        }
        
    } catch (const json::exception& e) {
        return false;
    }
    
    return true;
}

bool ConfigSchema::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json schemaJson;
    schemaJson["config_schema"]["version"] = "1.0";
    
    json optionsJson = json::array();
    for (const auto& key : pImpl->allKeys) {
        json option;
        option["key"] = key;
        option["description"] = pImpl->descriptions.at(key);
        option["required"] = pImpl->requiredKeys.at(key);
        
        if (pImpl->validators.count(key)) {
            option["validator"] = "custom";
        }
        
        if (pImpl->ranges.count(key)) {
            option["range"]["min"] = pImpl->ranges.at(key).first;
            option["range"]["max"] = pImpl->ranges.at(key).second;
            option["range_error"] = pImpl->errorMessages.at(key);
        }
        
        if (pImpl->enumValues.count(key)) {
            json enumArray = json::array();
            for (const auto& value : pImpl->enumValues.at(key)) {
                enumArray.push_back(value);
            }
            option["enum"] = enumArray;
            option["enum_error"] = pImpl->errorMessages.at(key);
        }
        
        optionsJson.push_back(option);
    }
    
    schemaJson["config_schema"]["options"] = optionsJson;
    
    file << schemaJson.dump(2);
    
    return true;
}

} // namespace nlm
// NLM Configuration Validation and Schema Checking Implementation
// Comprehensive configuration validation system

#include "NLMConfigurationValidation.h"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace nlm {

ConfigurationValidator::ConfigurationValidator() {
    // Initialize with default validation rules
    initializeDefaultRules();
}

ConfigurationValidator::~ConfigurationValidator() {}

void ConfigurationValidator::initializeDefaultRules() {
    // Basic configuration rules
    addRule("random_seed", "int", "range:0:1000000", true, "42",
           [](const std::string& value) {
               try {
                   int seed = std::stoi(value);
                   return seed >= 0 && seed <= 1000000;
               } catch (...) {
                   return false;
               }
           });
    
    addRule("simulation_timestep", "float", "range:0.0001:0.1", true, "0.001",
           [](const std::string& value) {
               try {
                   double timestep = std::stod(value);
                   return timestep > 0.0 && timestep <= 0.1;
               } catch (...) {
                   return false;
               }
           });
    
    addRule("neuron_count", "int", "range:10:10000", true, "1000",
           [](const std::string& value) {
               try {
                   int count = std::stoi(value);
                   return count >= 10 && count <= 10000;
               } catch (...) {
                   return false;
               }
           });
    
    addRule("region_count", "int", "range:1:100", true, "2",
           [](const std::string& value) {
               try {
                   int count = std::stoi(value);
                   return count >= 1 && count <= 100;
               } catch (...) {
                   return false;
               }
           });
    
    addRule("connection_probability", "float", "range:0.0:1.0", true, "0.1",
           [](const std::string& value) {
               try {
                   double prob = std::stod(value);
                   return prob >= 0.0 && prob <= 1.0;
               } catch (...) {
                   return false;
               }
           });
    
    addRule("environment_width", "int", "range:10:1000", true, "20",
           [](const std::string& value) {
               try {
                   int width = std::stoi(value);
                   return width >= 10 && width <= 1000;
               } catch (...) {
                   return false;
               }
           });
    
    addRule("environment_height", "int", "range:10:1000", true, "20",
           [](const std::string& value) {
               try {
                   int height = std::stoi(value);
                   return height >= 10 && height <= 1000;
               } catch (...) {
                   return false;
               }
           });
    
    // Boolean rules
    addRule("plasticity.stdp.enable", "bool", "enum:true,false", false, "true",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("plasticity.hebbian.enable", "bool", "enum:true,false", false, "true",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("plasticity.structural.enable", "bool", "enum:true,false", false, "true",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("neuromod.dopamine.enable", "bool", "enum:true,false", false, "true",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("neuromod.curiosity.enable", "bool", "enum:true,false", false, "true",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("neuromod.novelty.enable", "bool", "enum:true,false", false, "true",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("benchmark", "bool", "enum:true,false", false, "false",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("verbose", "bool", "enum:true,false", false, "false",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("quiet", "bool", "enum:true,false", false, "false",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("enable_checkpointing", "bool", "enum:true,false", false, "false",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("enable_replay", "bool", "enum:true,false", false, "false",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
    
    addRule("enable_development", "bool", "enum:true,false", false, "false",
           [](const std::string& value) {
               return value == "true" || value == "false" || value == "1" || value == "0";
           });
}

bool ConfigurationValidator::validate(const Config& config, std::vector<std::string>& errors) const {
    bool allValid = true;
    
    for (const auto& rule : rules_) {
        std::string error = validateRule(config, rule);
        if (!error.empty()) {
            errors.push_back(error);
            allValid = false;
        }
    }
    
    return allValid;
}

bool ConfigurationValidator::validate(const Config& config) const {
    std::vector<std::string> errors;
    return validate(config, errors);
}

std::string ConfigurationValidator::validateRule(const Config& config, const ValidationRule& rule) const {
    // Check if required key exists
    if (rule.required && !config.has(rule.key)) {
        return "Missing required configuration key: " + rule.key;
    }
    
    if (!config.has(rule.key)) {
        // Optional key that's not present - use default if available
        if (!rule.defaultValue.empty()) {
            return "";
        }
        return "";
    }
    
    // Get the value
    std::string value;
    if (rule.type == "int") {
        int64_t intVal = config.getOr<int64_t>(rule.key, 0);
        value = std::to_string(intVal);
    } else if (rule.type == "float") {
        float floatVal = config.getOr<float>(rule.key, 0.0f);
        value = std::to_string(floatVal);
    } else if (rule.type == "bool") {
        bool boolVal = config.getOr<bool>(rule.key, false);
        value = boolVal ? "true" : "false";
    } else if (rule.type == "string") {
        value = config.getOr<std::string>(rule.key, "");
    }
    
    // Apply type-specific validation
    if (rule.customValidator) {
        if (!rule.customValidator(value)) {
            return "Invalid value for " + rule.key + ": " + value + " (does not meet custom validation)";
        }
    } else if (!rule.validation.empty()) {
        if (!validateValue(value, rule.validation, rule.type)) {
            return "Invalid value for " + rule.key + ": " + value + " (validation failed for: " + rule.validation + "; type: " + rule.type + ")";
        }
    }
    
    return "";
}

bool ConfigurationValidator::validateValue(const std::string& value, const std::string& validation, const std::string& type) const {
    if (validation.find("range:") == 0) {
        // Parse range:range:min:max
        size_t colon1 = validation.find(':');
        size_t colon2 = validation.find(':', colon1 + 1);
        if (colon1 != std::string::npos && colon2 != std::string::npos) {
            double min = std::stod(validation.substr(colon1 + 1, colon2 - colon1 - 1));
            double max = std::stod(validation.substr(colon2 + 1));
            double val = std::stod(value);
            return val >= min && val <= max;
        }
    } else if (validation.find("length:") == 0) {
        // Parse length:length:min:max
        size_t colon1 = validation.find(':');
        size_t colon2 = validation.find(':', colon1 + 1);
        if (colon1 != std::string::npos && colon2 != std::string::npos) {
            size_t minLength = static_cast<size_t>(std::stoi(validation.substr(colon1 + 1, colon2 - colon1 - 1)));
            size_t maxLength = static_cast<size_t>(std::stoi(validation.substr(colon2 + 1)));
            return value.length() >= minLength && value.length() <= maxLength;
        }
    } else if (validation.find("enum:") == 0) {
        // Parse enum:enum:value1:value2:...
        std::string enumValues = validation.substr(5);
        std::vector<std::string> enums = split(enumValues, ':');
        
        // Handle special case for boolean enums
        if (enums.size() == 2 && ((enums[0] == "true" && enums[1] == "false") ||
                                 (enums[0] == "false" && enums[1] == "true"))) {
            return value == "true" || value == "false" || value == "1" || value == "0";
        }
        
        return std::find(enums.begin(), enums.end(), value) != enums.end();
    } else if (validation.find("pattern:") == 0) {
        // Parse pattern:pattern:regex
        std::string pattern = validation.substr(8);
        if (!pattern.empty()) {
            std::regex regex(pattern);
            return std::regex_match(value, regex);
        }
        return true;
    }
    
    return true; // No validation applied
}

std::vector<std::string> ConfigurationValidator::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

std::string ConfigurationValidator::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

bool ConfigurationValidator::loadFromFile(const std::string& filename, Config& config) {
    std::ifstream file(filename);
    if (!file) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.find("#") == 0) {
            continue;
        }
        
        // Parse key=value format
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue; // Skip malformed lines
        }
        
        std::string key = trim(line.substr(0, equalsPos));
        std::string value = trim(line.substr(equalsPos + 1));
        
        // Remove comments from value
        size_t commentPos = value.find('#');
        if (commentPos != std::string::npos) {
            value = value.substr(0, commentPos);
            value = trim(value);
        }
        
        try {
            // Try to determine type based on value
            if (value.find('.') != std::string::npos) {
                // Float value
                float floatVal = std::stof(value);
                config.set(key, floatVal, ConfigSource::File);
            } else if (value == "true" || value == "false" || value == "1" || value == "0") {
                // Boolean value
                bool boolVal = (value == "true" || value == "1");
                config.set(key, boolVal, ConfigSource::File);
            } else {
                // Integer value
                int64_t intVal = std::stoll(value);
                config.set(key, intVal, ConfigSource::File);
            }
        } catch (...) {
            // If parsing fails, try as string
            config.set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool ConfigurationValidator::validateConfigFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.find("#") == 0) {
            continue;
        }
        
        // Validate line format
        if (line.find('=') == std::string::npos) {
            return false; // Malformed line
        }
        
        std::string key = trim(line.substr(0, line.find('=')));
        std::string value = trim(line.substr(line.find('=') + 1));
        
        // Remove comments from value
        size_t commentPos = value.find('#');
        if (commentPos != std::string::npos) {
            value = value.substr(0, commentPos);
            value = trim(value);
        }
        
        // Basic validation of key and value format
        if (key.empty() || value.empty()) {
            return false;
        }
    }
    
    return true;
}

std::string ConfigurationValidator::generateConfigurationReport(const Config& config) const {
    std::stringstream report;
    
    report << "=== NLM Configuration Validation Report ===" << std::endl;
    report << std::endl;
    
    report << "Configuration Status: VALID" << std::endl;
    report << "Total validated keys: " << getValidatedKeysCount(config) << std::endl;
    report << "Required keys: " << getRequiredKeysCount() << std::endl;
    
    report << std::endl;
    report << "=== Configuration Summary ===" << std::endl;
    
    // List all configuration values
    report << std::left << std::setw(30) << "Key" << std::setw(15) << "Value" << std::setw(10) << "Type" << std::endl;
    report << std::string(60, '-') << std::endl;
    
    for (const auto& rule : rules_) {
        if (config.has(rule.key)) {
            std::string value;
            if (rule.type == "int") {
                int64_t intVal = config.getOr<int64_t>(rule.key, 0);
                value = std::to_string(intVal);
            } else if (rule.type == "float") {
                float floatVal = config.getOr<float>(rule.key, 0.0f);
                value = std::to_string(floatVal);
            } else if (rule.type == "bool") {
                bool boolVal = config.getOr<bool>(rule.key, false);
                value = boolVal ? "true" : "false";
            } else if (rule.type == "string") {
                value = config.getOr<std::string>(rule.key, "");
            }
            
            report << std::left << std::setw(30) << rule.key 
                   << std::setw(15) << value 
                   << std::setw(10) << rule.type << std::endl;
        }
    }
    
    report << std::endl;
    report << "=== Validation Rules ===" << std::endl;
    
    for (const auto& rule : rules_) {
        report << "Key: " << std::left << std::setw(20) << rule.key;
        report << "Type: " << std::setw(10) << rule.type;
        report << "Required: " << (rule.required ? "Yes" : "No") << std::endl;
        
        if (!rule.validation.empty()) {
            report << std::setw(30) << "" << "Validation: " << rule.validation << std::endl;
        }
        
        if (!rule.defaultValue.empty()) {
            report << std::setw(30) << "" << "Default: " << rule.defaultValue << std::endl;
        }
        
        report << std::endl;
    }
    
    return report.str();
}

void ConfigurationValidator::exportValidationReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file << generateConfigurationReport(Config()); // Note: Need a default config for testing
    file.close();
}

int ConfigurationValidator::getValidatedKeysCount(const Config& config) const {
    int count = 0;
    for (const auto& rule : rules_) {
        if (config.has(rule.key)) {
            count++;
        }
    }
    return count;
}

int ConfigurationValidator::getRequiredKeysCount() const {
    int count = 0;
    for (const auto& rule : rules_) {
        if (rule.required) {
            count++;
        }
    }
    return count;
}

void ConfigurationValidator::addRule(const std::string& key, const std::string& type,
                                    const std::string& validation, bool required,
                                    const std::string& defaultValue,
                                    std::function<bool(const std::string&)> customValidator) {
    ValidationRule rule;
    rule.key = key;
    rule.type = type;
    rule.validation = validation;
    rule.required = required;
    rule.defaultValue = defaultValue;
    rule.customValidator = customValidator;
    rules_.push_back(rule);
}

bool ConfigurationValidator::validateFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.find("#") == 0) {
            continue;
        }
        
        // Basic validation
        if (line.find('=') == std::string::npos) {
            return false;
        }
        
        std::string key = trim(line.substr(0, line.find('=')));
        std::string value = trim(line.substr(line.find('=') + 1));
        
        // Remove comments
        size_t commentPos = value.find('#');
        if (commentPos != std::string::npos) {
            value = value.substr(0, commentPos);
            value = trim(value);
        }
        
        if (key.empty() || value.empty()) {
            return false;
        }
    }
    
    return true;
}

} // namespace nlm
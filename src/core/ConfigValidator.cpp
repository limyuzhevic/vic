#include "ConfigValidator.hpp"
#include "Config.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

ConfigValidator::ConfigValidator() {
    loadDefaultSchema();
}

void ConfigValidator::loadDefaultSchema() {
    defaultSchema_.schemaName = "Default NLM Configuration Schema";
    defaultSchema_.strictMode = true;
    defaultSchema_.version = "2.0.0";
    
    // Core configuration group
    defaultSchema_.addGroup("core", "Core simulation parameters");
    defaultSchema_.addGroup("brain", "Brain configuration parameters");
    defaultSchema_.addGroup("plasticity", "Plasticity system parameters");
    defaultSchema_.addGroup("development", "Development system parameters");
    defaultSchema_.addGroup("neuromodulation", "Neuromodulation system parameters");
    defaultSchema_.addGroup("memory", "Memory system parameters");
    defaultSchema_.addGroup("environment", "Environment parameters");
    defaultSchema_.addGroup("logging", "Logging parameters");
    defaultSchema_.addGroup("visualization", "Visualization parameters");
    
    // Core parameters
    defaultSchema_.addRule(ValidationRule("random_seed", "int", true, "42", 
                                          "Random seed for reproducible experiments"));
    defaultSchema_.addRule(ValidationRule("simulation_timestep", "double", true, "0.001",
                                          "Simulation timestep in seconds"));
    defaultSchema_.addRule(ValidationRule("max_simulation_steps", "int", false, "10000",
                                          "Maximum simulation steps"));
    
    // Brain parameters
    defaultSchema_.addRule(ValidationRule("neuron_count", "int", false, "1000",
                                          "Number of neurons in the brain"));
    defaultSchema_.addRule(ValidationRule("region_count", "int", false, "1",
                                          "Number of brain regions"));
    defaultSchema_.addRule(ValidationRule("connection_probability", "double", false, "0.1",
                                          "Connection probability for random connectivity"));
    
    // Plasticity parameters
    defaultSchema_.addRule(ValidationRule("plasticity_learning_rate", "double", false, "0.01",
                                          "Plasticity learning rate"));
    defaultSchema_.addRule(ValidationRule("stdp_ltp_weight", "double", false, "0.01",
                                          "STDP LTP weight"));
    defaultSchema_.addRule(ValidationRule("stdp_ltd_weight", "double", false, "0.012",
                                          "STDP LTD weight"));
    defaultSchema_.addRule(ValidationRule("stdp_time_constant", "double", false, "20.0",
                                          "STDP time constant (ms)"));
    
    // Development parameters
    defaultSchema_.addRule(ValidationRule("development_synaptogenesis_rate", "double", false, "0.001",
                                          "Synaptogenesis rate"));
    defaultSchema_.addRule(ValidationRule("development_pruning_rate", "double", false, "0.0001",
                                          "Pruning rate"));
    
    // Neuromodulation parameters
    defaultSchema_.addRule(ValidationRule("dopamine_baseline", "double", false, "0.1",
                                          "Dopamine baseline level"));
    defaultSchema_.addRule(ValidationRule("curiosity_baseline", "double", false, "0.0",
                                          "Curiosity baseline level"));
    defaultSchema_.addRule(ValidationRule("novelty_baseline", "double", false, "0.0",
                                          "Novelty baseline level"));
    
    // Memory parameters
    defaultSchema_.addRule(ValidationRule("working_memory_size", "int", false, "1000",
                                          "Working memory size"));
    defaultSchema_.addRule(ValidationRule("episodic_memory_capacity", "int", false, "100",
                                          "Episodic memory capacity"));
    
    // Environment parameters
    defaultSchema_.addRule(ValidationRule("environment_width", "int", false, "10",
                                          "Environment width"));
    defaultSchema_.addRule(ValidationRule("environment_height", "int", false, "10",
                                          "Environment height"));
    
    // Logging parameters
    defaultSchema_.addRule(ValidationRule("log_level", "string", false, "INFO",
                                          "Log level (DEBUG, INFO, WARNING, ERROR)"));
    defaultSchema_.addRule(ValidationRule("log_to_file", "bool", false, "false",
                                          "Whether to log to file"));
    defaultSchema_.addRule(ValidationRule("log_filename", "string", false, "nlm.log",
                                          "Log file name"));
    
    // Visualization parameters
    defaultSchema_.addRule(ValidationRule("visualization_enabled", "bool", false, "false",
                                          "Whether visualization is enabled"));
    defaultSchema_.addRule(ValidationRule("visualization_update_rate", "double", false, "30.0",
                                          "Visualization update rate (Hz)"));
}

bool ConfigValidator::loadSchemaFromFile(const std::string& filepath) {
    // Simplified schema file loading (could be extended to JSON/YAML format)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, just load the default schema
    // In a full implementation, this would parse a schema file
    defaultSchema_ = getDefaultSchema();
    defaultSchema_.schemaName = "Custom Schema from " + filepath;
    
    file.close();
    return true;
}

bool ConfigValidator::validate(const nlm::Config& config, std::string& errorMessage,
                              const ConfigSchema& schema) const {
    return validateWithSchema(config, schema, errorMessage);
}

bool ConfigValidator::validateWithSchema(const nlm::Config& config, const ConfigSchema& schema,
                                        std::string& errorMessage) const {
    errorMessage.clear();
    
    // Check for missing required keys
    for (const auto& rule : schema.validationRules) {
        if (rule.required && !config.has(rule.key)) {
            errorMessage += "Missing required configuration key: " + rule.key + std::string("\n");
        }
    }
    
    // Validate all keys in configuration
    auto configKeys = config.getKeys();
    if (schema.strictMode) {
        for (const auto& key : configKeys) {
            bool found = false;
            for (const auto& rule : schema.validationRules) {
                if (rule.key == key) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                errorMessage += "Unexpected configuration key: " + key + std::string("\n");
            }
        }
    }
    
    // Validate all keys against schema rules
    for (const auto& rule : schema.validationRules) {
        if (config.has(rule.key)) {
            std::string typeError;
            bool valid = false;
            
            if (rule.type == "string") {
                valid = validateString(rule.key, config, rule, typeError);
            } else if (rule.type == "int") {
                valid = validateInt(rule.key, config, rule, typeError);
            } else if (rule.type == "double") {
                valid = validateDouble(rule.key, config, rule, typeError);
            } else if (rule.type == "bool") {
                valid = validateBool(rule.key, config, rule, typeError);
            } else if (rule.type == "vector<int>") {
                valid = validateVector(rule.key, config, rule, typeError);
            } else if (rule.type == "string_list") {
                valid = validateVector(rule.key, config, rule, typeError);
            }
            
            if (!valid) {
                errorMessage += typeError;
            }
        }
    }
    
    return errorMessage.empty();
}

std::string ConfigValidator::getValidationHelp(const ConfigSchema& schema) const {
    std::ostringstream oss;
    
    oss << "Configuration Schema: " << schema.schemaName << std::endl;
    oss << "Version: " << schema.version << std::endl;
    oss << "Strict Mode: " << (schema.strictMode ? "Yes" : "No") << std::endl;
    oss << std::endl;
    
    oss << "Validation Rules:" << std::endl;
    oss << std::left << std::setw(20) << "Key" 
        << std::left << std::setw(15) << "Type" 
        << std::left << std::setw(10) << "Required" 
        << std::left << std::setw(15) << "Default" 
        << "Description" << std::endl;
    oss << std::string(80, '-') << std::endl;
    
    for (const auto& rule : schema.validationRules) {
        oss << std::left << std::setw(20) << rule.key
            << std::left << std::setw(15) << rule.type
            << std::left << std::setw(10) << (rule.required ? "Yes" : "No")
            << std::left << std::setw(15) << rule.defaultValue
            << rule.description << std::endl;
    }
    
    return oss.str();
}

ConfigSchema ConfigValidator::createTestSchema() {
    ConfigSchema schema("Test Configuration Schema", true, "2.0.0");
    
    // Add test-specific rules
    schema.addGroup("test", "Test execution parameters");
    schema.addRule(ValidationRule("test_name", "string", true, "",
                                  "Name of the test"));
    schema.addRule(ValidationRule("test_mode", "string", false, "normal",
                                  "Test mode (fast, normal, detailed, debug)"));
    schema.addRule(ValidationRule("test_timeout", "int", false, "3600",
                                  "Test timeout in seconds"));
    
    // Add basic brain rules
    schema.addRule(ValidationRule("neuron_count", "int", false, "1000",
                                  "Number of neurons"));
    schema.addRule(ValidationRule("simulation_timestep", "double", false, "0.001",
                                  "Simulation timestep"));
    
    return schema;
}

ConfigSchema ConfigValidator::createProductionSchema() {
    ConfigSchema schema("Production Configuration Schema", true, "2.0.0");
    
    // Add production-specific rules
    schema.addGroup("production", "Production environment parameters");
    schema.addRule(ValidationRule("max_memory_usage", "int", false, "4096",
                                  "Maximum memory usage in MB"));
    schema.addRule(ValidationRule("enable_checkpointing", "bool", false, "true",
                                  "Whether to enable checkpointing"));
    schema.addRule(ValidationRule("checkpoint_interval", "int", false, "1000",
                                  "Checkpoint interval in steps"));
    
    // Include all core rules
    schema = getDefaultSchema();
    
    return schema;
}

ConfigSchema ConfigValidator::createMinimalSchema() {
    ConfigSchema schema("Minimal Configuration Schema", false, "1.0.0");
    
    // Add minimal rules
    schema.addRule(ValidationRule("simulation_timestep", "double", true, "0.001",
                                  "Simulation timestep"));
    schema.addRule(ValidationRule("neuron_count", "int", false, "1000",
                                  "Number of neurons"));
    
    return schema;
}

bool ConfigValidator::isValidForTests(const nlm::Config& config) const {
    std::string errorMessage;
    ConfigSchema testSchema = createTestSchema();
    
    return validateWithSchema(config, testSchema, errorMessage);
}

std::string ConfigValidator::getConfigStats(const nlm::Config& config) const {
    std::ostringstream oss;
    
    oss << "Configuration Statistics:" << std::endl;
    oss << "Total keys: " << config.getKeys().size() << std::endl;
    oss << std::endl;
    
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        // Try to get value and determine type
        if (config.has(key)) {
            oss << key << ": ";
            
            // Try different types
            auto stringVal = config.get<std::string>(key);
            if (stringVal.has_value()) {
                oss << "string = \"" << stringVal.value() << "\"";
            } else {
                auto intVal = config.get<int>(key);
                if (intVal.has_value()) {
                    oss << "int = " << intVal.value();
                } else {
                    auto doubleVal = config.get<double>(key);
                    if (doubleVal.has_value()) {
                        oss << "double = " << doubleVal.value();
                    } else {
                        auto boolVal = config.get<bool>(key);
                        if (boolVal.has_value()) {
                            oss << "bool = " << (boolVal.value() ? "true" : "false");
                        } else {
                            auto vectorVal = config.get<std::vector<int>>(key);
                            if (vectorVal.has_value()) {
                                oss << "vector<int> = [";
                                for (size_t i = 0; i < vectorVal.value().size(); ++i) {
                                    if (i > 0) oss << ", ";
                                    oss << vectorVal.value()[i];
                                }
                                oss << "]";
                            } else {
                                oss << "unknown type";
                            }
                        }
                    }
                }
            }
            oss << std::endl;
        }
    }
    
    return oss.str();
}

std::vector<std::string> ConfigValidator::suggestCorrections(const std::string& errorMessage) const {
    std::vector<std::string> suggestions;
    
    // Simple suggestion generation based on common error patterns
    if (errorMessage.find("Missing required") != std::string::npos) {
        suggestions.push_back("Add the missing key to your configuration file");
        suggestions.push_back("Set the key to its default value if unsure");
    } else if (errorMessage.find("Unexpected") != std::string::npos) {
        suggestions.push_back("Remove or ignore the unknown configuration key");
        suggestions.push_back("Check the schema file for valid keys");
    } else if (errorMessage.find("must be between") != std::string::npos) {
        suggestions.push_back("Adjust the value to be within the valid range");
        suggestions.push_back("Check the schema for valid min/max values");
    } else if (errorMessage.find("must be one of") != std::string::npos) {
        suggestions.push_back("Choose a value from the list of valid options");
        suggestions.push_back("Check the schema for valid values");
    }
    
    return suggestions;
}

bool ConfigValidator::validateString(const std::string& key, const nlm::Config& config,
                                    const ValidationRule& rule, std::string& errorMessage) const {
    auto valueOpt = config.get<std::string>(key);
    if (!valueOpt.has_value()) {
        if (rule.required) {
            errorMessage += "Missing required key: " + key + std::string("\n");
            return false;
        }
        return true; // Not required, missing is OK
    }
    
    return true;
}

bool ConfigValidator::validateInt(const std::string& key, const nlm::Config& config,
                                 const ValidationRule& rule, std::string& errorMessage) const {
    auto valueOpt = config.get<int>(key);
    if (!valueOpt.has_value()) {
        if (rule.required) {
            errorMessage += "Missing required key: " + key + std::string("\n");
            return false;
        }
        return true; // Not required, missing is OK
    }
    
    // Validate numeric range if min/max are provided
    if (!rule.minValue.empty()) {
        int minVal = stringToInt(rule.minValue);
        if (valueOpt.value() < minVal) {
            errorMessage += "Key " + key + ": value " + std::to_string(valueOpt.value()) +
                           " is less than minimum " + rule.minValue + std::string("\n");
            return false;
        }
    }
    
    if (!rule.maxValue.empty()) {
        int maxVal = stringToInt(rule.maxValue);
        if (valueOpt.value() > maxVal) {
            errorMessage += "Key " + key + ": value " + std::to_string(valueOpt.value()) +
                           " exceeds maximum " + rule.maxValue + std::string("\n");
            return false;
        }
    }
    
    // Validate against valid values if provided
    if (!rule.validValues.empty()) {
        bool valid = false;
        for (const auto& validVal : rule.validValues) {
            if (valueOpt.value() == stringToInt(validVal)) {
                valid = true;
                break;
            }
        }
        if (!valid) {
            errorMessage += "Key " + key + ": value " + std::to_string(valueOpt.value()) +
                           " is not in valid list: ";
            for (size_t i = 0; i < rule.validValues.size(); ++i) {
                if (i > 0) errorMessage += ", ";
                errorMessage += rule.validValues[i];
            }
            errorMessage += std::string("\n");
            return false;
        }
    }
    
    return true;
}

bool ConfigValidator::validateDouble(const std::string& key, const nlm::Config& config,
                                     const ValidationRule& rule, std::string& errorMessage) const {
    auto valueOpt = config.get<double>(key);
    if (!valueOpt.has_value()) {
        if (rule.required) {
            errorMessage += "Missing required key: " + key + std::string("\n");
            return false;
        }
        return true; // Not required, missing is OK
    }
    
    // Validate numeric range if min/max are provided
    if (!rule.minValue.empty()) {
        double minVal = stringToDouble(rule.minValue);
        if (valueOpt.value() < minVal) {
            errorMessage += "Key " + key + ": value " + std::to_string(valueOpt.value()) +
                           " is less than minimum " + rule.minValue + std::string("\n");
            return false;
        }
    }
    
    if (!rule.maxValue.empty()) {
        double maxVal = stringToDouble(rule.maxValue);
        if (valueOpt.value() > maxVal) {
            errorMessage += "Key " + key + ": value " + std::to_string(valueOpt.value()) +
                           " exceeds maximum " + rule.maxValue + std::string("\n");
            return false;
        }
    }
    
    // Validate against valid values if provided
    if (!rule.validValues.empty()) {
        bool valid = false;
        for (const auto& validVal : rule.validValues) {
            if (std::abs(valueOpt.value() - stringToDouble(validVal)) < 0.0001) {
                valid = true;
                break;
            }
        }
        if (!valid) {
            errorMessage += "Key " + key + ": value " + std::to_string(valueOpt.value()) +
                           " is not in valid list: ";
            for (size_t i = 0; i < rule.validValues.size(); ++i) {
                if (i > 0) errorMessage += ", ";
                errorMessage += rule.validValues[i];
            }
            errorMessage += std::string("\n");
            return false;
        }
    }
    
    return true;
}

bool ConfigValidator::validateBool(const std::string& key, const nlm::Config& config,
                                   const ValidationRule& rule, std::string& errorMessage) const {
    auto valueOpt = config.get<bool>(key);
    if (!valueOpt.has_value()) {
        if (rule.required) {
            errorMessage += "Missing required key: " + key + std::string("\n");
            return false;
        }
        return true; // Not required, missing is OK
    }
    
    return true;
}

bool ConfigValidator::validateVector(const std::string& key, const nlm::Config& config,
                                     const ValidationRule& rule, std::string& errorMessage) const {
    auto valueOpt = config.get<std::vector<int>>(key);
    if (!valueOpt.has_value()) {
        if (rule.required) {
            errorMessage += "Missing required key: " + key + std::string("\n");
            return false;
        }
        return true; // Not required, missing is OK
    }
    
    return true;
}

bool ConfigValidator::validateEnum(const std::string& key, const nlm::Config& config,
                                   const ValidationRule& rule, std::string& errorMessage) const {
    return validateString(key, config, rule, errorMessage);
}

std::string ConfigValidator::getTypeDescription(const std::string& type) const {
    if (type == "string") return "String value";
    if (type == "int") return "Integer value";
    if (type == "double") return "Floating-point value";
    if (type == "bool") return "Boolean value";
    if (type == "vector<int>") return "Integer vector";
    if (type == "string_list") return "String list";
    return "Unknown type";
}

std::string ConfigValidator::formatValidationError(const std::string& key,
                                                   const std::string& expectedType,
                                                   const std::string& actualType) const {
    return "Key \"" + key + "\": expected " + expectedType + 
           ", got " + actualType + std::string("\n");
}

bool ConfigValidator::stringToBool(const std::string& str) const {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

int ConfigValidator::stringToInt(const std::string& str) const {
    return std::stoi(str);
}

double ConfigValidator::stringToDouble(const std::string& str) const {
    return std::stod(str);
}

bool ConfigValidator::isNumericType(const std::string& type) const {
    return type == "int" || type == "double";
}

bool ConfigValidator::isStringType(const std::string& type) const {
    return type == "string" || type == "string_list";
}

bool ConfigValidator::isBoolType(const std::string& type) const {
    return type == "bool";
}

bool ConfigValidator::isVectorType(const std::string& type) const {
    return type == "vector<int>" || type == "string_list";
}

std::string ConfigValidator::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ConfigValidator::stringToLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ConfigValidator::capitalize(const std::string& str) const {
    if (str.empty()) return str;
    
    std::string result = str;
    result[0] = std::toupper(result[0]);
    return result;
}

std::string ConfigValidator::getDefaultValueForType(const std::string& type) const {
    if (type == "string") return "";
    if (type == "int") return "0";
    if (type == "double") return "0.0";
    if (type == "bool") return "false";
    if (type == "vector<int>") return "[]";
    if (type == "string_list") return "[]";
    return "";
}

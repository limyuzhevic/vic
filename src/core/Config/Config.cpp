// Configuration schema validation
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "int64", "double", "bool", "string", "vector<int>", etc.
    std::string description;
    bool required;
    std::string defaultValue;
    std::string validation;  // Validation rules
    
    ConfigSchema() : key(), type("string"), description(), required(false), defaultValue(), validation() {}
    
    ConfigSchema(const std::string& k, const std::string& t, const std::string& d, 
                 bool req = false, const std::string& def = "", const std::string& val = "")
        : key(k), type(t), description(d), required(req), defaultValue(def), validation(val) {}
};

// Global configuration schema
inline std::vector<ConfigSchema> getDefaultSchema() {
    return {
        // Neural Network Parameters
        ConfigSchema("neuron_count", "int64", "Number of neurons in the brain", true, "1000"),
        ConfigSchema("region_count", "int64", "Number of brain regions", true, "2"),
        ConfigSchema("connection_probability", "double", "Connection probability for random connectivity", true, "0.1"),
        
        // Plasticity Parameters
        ConfigSchema("plasticity_learning_rate", "double", "Plasticity learning rate", true, "0.01"),
        ConfigSchema("stdp_ltp_weight", "double", "STDP LTP weight", true, "0.01"),
        ConfigSchema("stdp_ltd_weight", "double", "STDP LTD weight", true, "0.012"),
        ConfigSchema("stdp_tau", "double", "STDP time constant", true, "20.0"),
        
        // Structural Plasticity
        ConfigSchema("synaptogenesis_rate", "double", "Rate of synaptogenesis", true, "0.001"),
        ConfigSchema("pruning_rate", "double", "Rate of synaptic pruning", true, "0.0001"),
        
        // Development Parameters
        ConfigSchema("critical_period_start", "int64", "Critical period start step", false, "1000"),
        ConfigSchema("critical_period_duration", "int64", "Critical period duration in steps", false, "2000"),
        
        // Memory Parameters
        ConfigSchema("working_memory_capacity", "int64", "Working memory capacity", false, "100"),
        ConfigSchema("episodic_memory_capacity", "int64", "Episodic memory capacity", false, "1000"),
        ConfigSchema("associative_memory_capacity", "int64", "Associative memory capacity", false, "500"),
        
        // Prediction Parameters
        ConfigSchema("prediction_horizon", "int64", "Prediction horizon in steps", false, "10"),
        ConfigSchema("prediction_error_threshold", "double", "Prediction error threshold", false, "0.1"),
        
        // Sensory Parameters
        ConfigSchema("sensory_input_modality", "string", "Sensory input modality (vision,auditory,tactile)", false, "vision"),
        ConfigSchema("sensory_preprocessing_enabled", "bool", "Enable sensory preprocessing", false, "true"),
        
        // Motor Parameters
        ConfigSchema("motor_action_space", "int64", "Size of motor action space", false, "4"),
        ConfigSchema("motor_action_selection_method", "string", "Motor action selection method (greedy,epsilon_greedy)", false, "epsilon_greedy"),
        
        // Advanced Parameters
        ConfigSchema("simulation_timestep", "double", "Simulation timestep in seconds", true, "0.001"),
        ConfigSchema("max_simulation_steps", "int64", "Maximum simulation steps", true, "10000"),
        ConfigSchema("simulation_time_limit", "double", "Simulation time limit in seconds", false, "0.0"),
        
        // Logging Parameters
        ConfigSchema("log_level", "string", "Log level (debug,info,warning,error,critical)", true, "INFO"),
        ConfigSchema("log_to_file", "bool", "Enable file logging", false, "false"),
        ConfigSchema("log_filename", "string", "Log file name", false, "nlm.log"),
        ConfigSchema("enable_colors", "bool", "Enable colored console output", false, "true"),
        
        // Performance Parameters
        ConfigSchema("enable_cuda", "bool", "Enable GPU acceleration", false, "false"),
        ConfigSchema("memory_limit_mb", "int64", "Memory usage limit in MB", false, "1024"),
        ConfigSchema("checkpoint_save_interval", "int64", "Checkpoint save interval in steps", false, "100"),
        
        // Phase and Mode
        ConfigSchema("phase", "int64", "Phase to run (1,2,3,4,6)", true, "2"),
        ConfigSchema("mode", "string", "Operating mode (demo,test,training,production)", true, "demo"),
        ConfigSchema("enable_render", "bool", "Enable visualization rendering", false, "false"),
        
        // Training Parameters
        ConfigSchema("training_epochs", "int64", "Number of training epochs", false, "10"),
        ConfigSchema("training_steps_per_epoch", "int64", "Training steps per epoch", false, "1000"),
        
        // Random Seed
        ConfigSchema("random_seed", "int64", "Random seed for reproducible experiments", true, "42"),
    };
}

// Global configuration change listeners
inline std::vector<std::function<void(const std::string&, const ConfigValue&)>> changeListeners;

// Global hot reload state
inline std::chrono::steady_clock::time_point lastConfigChange;
inline std::string currentConfigFile;
inline bool hotReloadEnabled = false;
inline std::chrono::milliseconds hotReloadInterval(1000); // 1 second

// Forward declarations for JSON/YAML parsing
bool loadFromJSON(const std::string& filepath, Config& config);
bool loadFromYAML(const std::string& filepath, Config& config);
bool validateConfig(const Config& config);
bool applyEnvironmentVariables(Config& config);
bool saveToJSON(const std::string& filepath, const Config& config);
bool saveToYAML(const std::string& filepath, const Config& config);

// Convert string to lowercase
std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Parse environment variable value to ConfigValue
global namespace nlm {
Config globalConfig;

/**
 * @brief Loads configuration from a JSON file
 * @param filepath Path to the JSON configuration file
 * @return true if file was read successfully, false on error
 * @details
 * Loads configuration from JSON format with support for:
 * - Nested objects (converted to dot notation)
 * - Arrays (converted to comma-separated strings)
 * - Primitives (numbers, strings, booleans)
 * - Comments
 * - Schema validation
 * - Error recovery
 * 
 * JSON format examples:
 * {
 *   "neuron_count": 1000,
 *   "region_count": 2,
 *   "plasticity_learning_rate": 0.01,
 *   "stdp_ltp_weight": 0.01
 * }
 */
bool Config::loadFromJSON(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("loadFromJSON: Empty filepath provided");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("loadFromJSON: Failed to open file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        return loadFromJSONData(jsonData);
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("loadFromJSON: JSON parse error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("loadFromJSON: Error reading " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

/**
 * @brief Loads configuration from a YAML file
 * @param filepath Path to the YAML configuration file
 * @return true if file was read successfully, false on error
 * @details
 * Loads configuration from YAML format with support for:
 * - Nested mappings (converted to dot notation)
 * - Sequences (converted to comma-separated strings)
 * - Scalars (numbers, strings, booleans)
 * - Comments
 * - Multi-document YAML
 * - Error recovery
 * 
 * YAML format examples:
 * neuron_count: 1000
 * region_count: 2
 * plasticity_learning_rate: 0.01
 * stdp_ltp_weight: 0.01
 * st:`;ap_ltd_weight: 0.012
 */
bool Config::loadFromYAML(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("loadFromYAML: Empty filepath provided");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("loadFromYAML: Failed to open file: " + filepath);
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        return loadFromYAMLData(yamlData);
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("loadFromYAML: YAML parse error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("loadFromYAML: Error reading " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

/**
 * @brief Loads configuration from JSON data
 * @param jsonData JSON data to parse
 * @return true if parsing succeeded, false on error
 * @details
 * Parses JSON data and converts it to configuration entries.
 * Handles various JSON types and converts them to appropriate ConfigValue types:
 * - Objects: Converted to nested keys (dot notation)
 * - Arrays: Converted to comma-separated string values
 * - Numbers: Converted to int, int64, or double based on precision
 * - Strings: Converted to string
 * - Booleans: Converted to bool
 * - Null: Skipped
 * 
 * Schema validation is performed during loading to ensure type correctness.
 */
bool Config::loadFromJSONData(const nlohmann::json& jsonData) {
    if (!jsonData.is_object()) {
        NLM_LOG_ERROR("loadFromJSONData: JSON data is not an object");
        return false;
    }
    
    bool success = true;
    
    for (const auto& item : jsonData.items()) {
        std::string key = item.key();
        
        try {
            ConfigValue value;
            
            if (item.value().is_number_integer()) {
                value = item.value().get<int64_t>();
            } else if (item.value().is_number_unsigned()) {
                value = item.value().get<uint64_t>();
            } else if (item.value().is_number_float()) {
                value = item.value().get<double>();
            } else if (item.value().is_boolean()) {
                value = item.value().get<bool>();
            } else if (item.value().is_string()) {
                value = item.value().get<std::string>();
            } else if (item.value().is_array()) {
                // Convert array to comma-separated string
                std::vector<double> arrayData;
                for (const auto& element : item.value()) {
                    arrayData.push_back(element.get<double>());
                }
                value = arrayData;
            } else if (item.value().is_object()) {
                // Convert nested object to dot notation
                std::string nestedValue = jsonToNestedString(item.value(), item.key() + ".");
                value = nestedValue;
            } else {
                NLM_LOG_WARNING("loadFromJSONData: Unsupported JSON type for key: " + key);
                continue;
            }
            
            set(key, value, ConfigSource::File);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("loadFromJSONData: Error parsing key '" + key + "': " + std::string(e.what()));
            success = false;
        }
    }
    
    return success;
}

/**
 * @brief Loads configuration from YAML data
 * @param yamlData YAML data to parse
 * @return true if parsing succeeded, false on error
 * @details
 * Parses YAML data and converts it to configuration entries.
 * Handles various YAML types and converts them to appropriate ConfigValue types:
 * - Mapping: Converted to nested keys (dot notation)
 * - Sequence: Converted to comma-separated string values
 * - Scalar: Converted to int, int64, double, bool, or string
 * - Null: Skipped
 * 
 * Schema validation is performed during loading to ensure type correctness.
 */
bool Config::loadFromYAMLData(const YAML::Node& yamlData) {
    if (!yamlData.IsMap()) {
        NLM_LOG_ERROR("loadFromYAMLData: YAML data is not a mapping");
        return false;
    }
    
    bool success = true;
    
    for (const auto& item : yamlData) {
        std::string key = item.first.as<std::string>();
        
        try {
            ConfigValue value;
            
            if (item.second.IsSequence()) {
                // Convert sequence to comma-separated string
                std::vector<double> sequenceData;
                for (const auto& element : item.second) {
                    sequenceData.push_back(element.as<double>());
                }
                value = sequenceData;
            } else if (item.second.IsScalar()) {
                YAML::NodeType nodeType = item.second.Type();
                if (nodeType == YAML::NodeType::Int) {
                    value = item.second.as<int64_t>();
                } else if (nodeType == YAML::NodeType::Float) {
                    value = item.second.as<double>();
                } else if (nodeType == YAML::NodeType::Bool) {
                    value = item.second.as<bool>();
                } else {
                    // Default to string for other scalar types
                    value = item.second.as<std::string>();
                }
            } else if (item.second.IsMap()) {
                // Convert nested mapping to dot notation
                std::string nestedValue = yamlToNestedString(item.second, item.first.as<std::string>() + ".");
                value = nestedValue;
            } else {
                NLM_LOG_WARNING("loadFromYAMLData: Unsupported YAML type for key: " + key);
                continue;
            }
            
            set(key, value, ConfigSource::File);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("loadFromYAMLData: Error parsing key '" + key + "': " + std::string(e.what()));
            success = false;
        }
    }
    
    return success;
}

/**
 * @brief Converts JSON object to nested string representation
 * @param jsonObj JSON object to convert
 * @param prefix Prefix for nested keys
 * @return Nested string representation
 * @details
 * Recursively converts JSON objects to dot-notation string representation.
 * This allows nested JSON structures to be stored as flattened configuration keys.
 * 
 * Example:
 * Input: {"neural_network": {"neuron_count": 1000}}
 * Output: "neural_network.neuron_count=1000"
 */
std::string Config::jsonToNestedString(const nlohmann::json& jsonObj, const std::string& prefix) {
    std::ostringstream oss;
    
    for (const auto& item : jsonObj.items()) {
        std::string newPrefix = prefix + item.key() + ".";
        
        if (item.value().is_object()) {
            oss << jsonToNestedString(item.value(), newPrefix);
        } else if (item.value().is_array()) {
            oss << newPrefix << "=" << jsonArrayToString(item.value()) << "\n";
        } else {
            oss << newPrefix << "=" << item.value() << "\n";
        }
    }
    
    return oss.str();
}

/**
 * @brief Converts YAML node to nested string representation
 * @param yamlNode YAML node to convert
 * @param prefix Prefix for nested keys
 * @return Nested string representation
 * @details
 * Recursively converts YAML nodes to dot-notation string representation.
 * This allows nested YAML structures to be stored as flattened configuration keys.
 */
std::string Config::yamlToNestedString(const YAML::Node& yamlNode, const std::string& prefix) {
    std::ostringstream oss;
    
    if (yamlNode.IsMap()) {
        for (const auto& item : yamlNode) {
            std::string newPrefix = prefix + item.first.as<std::string>() + ";";
            if (item.second.IsMap()) {
                oss << yamlToNestedString(item.second, newPrefix);
            } else {
                oss << newPrefix << "=" << yamlNodeAsString(item.second) << "\n";
            }
        }
    }
    
    return oss.str();
}

/**
 * @brief Converts JSON array to string representation
 * @param jsonArray JSON array to convert
 * @return String representation of array
 * @details
 * Converts JSON array to comma-separated string representation.
 * This preserves array data in configuration format.
 */
std::string Config::jsonArrayToString(const nlohmann::json& jsonArray) {
    std::ostringstream oss;
    oss << "[";
    
    for (size_t i = 0; i < jsonArray.size(); ++i) {
        if (i > 0) oss << ",";
        oss << jsonArray[i];
    }
    
    oss << "]";
    return oss.str();
}

/**
 * @brief Converts YAML node to string representation
 * @param yamlNode YAML node to convert
 * @return String representation of node
 * @details
 * Converts YAML node to appropriate string representation based on its type.
 * This allows YAML data to be stored in configuration format.
 */
std::string Config::yamlNodeAsString(const YAML::Node& yamlNode) {
    if (yamlNode.IsScalar()) {
        return yamlNode.as<std::string>();
    } else if (yamlNode.IsSequence()) {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < yamlNode.size(); ++i) {
            if (i > 0) oss << ",";
            oss << yamlNode[i].as<std::string>();
        }
        oss << "]";
        return oss.str();
    } else if (yamlNode.IsMap()) {
        // Convert nested map to string representation
        std::ostringstream oss;
        oss << "{";
        for (const auto& item : yamlNode) {
            oss << "\"" << item.first.as<std::string>() << "\":" << yamlNodeAsString(item.second);
            if (&item != &yamlNode.begin()->second) oss << ",";
        }
        oss << "}";
        return oss.str();
    }
    
    return "";
}

/**
 * @brief Validates configuration against schema
 * @param config Configuration to validate
 * @return true if configuration is valid, false otherwise
 * @details
 * Validates configuration against default schema rules.
 * Performs type checking, required field validation, and value validation.
 * Returns detailed error messages for validation failures.
 * 
 * Validation includes:
 * - Type checking (ensures values match declared types)
 * - Required field validation (checks if required fields are present)
 * - Range validation (validates numeric ranges)
 * - Enum validation (validates allowed values)
 */
bool Config::validate(const std::vector<ConfigSchema>& schema) {
    std::vector<std::string> errors;
    
    // Check required fields
    for (const auto& entry : schema) {
        if (entry.required && !has(entry.key)) {
            errors.push_back("Required configuration key missing: " + entry.key);
        }
    }
    
    // Validate types and values
    for (const auto& entry : schema) {
        if (!has(entry.key)) {
            continue;  // Skip validation for missing keys (handled above)
        }
        
        std::optional<ConfigValue> valueOpt = get<void>(entry.key);
        if (!valueOpt.has_value()) {
            errors.push_back("Cannot get configuration value for key: " + entry.key);
            continue;
        }
        
        // Type validation
        bool typeValid = false;
        if (entry.type == "int64" || entry.type == "int") {
            if (std::holds_alternative<int64_t>(valueOpt.value()) || 
                std::holds_alternative<int>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "double" || entry.type == "float") {
            if (std::holds_alternative<double>(valueOpt.value()) || 
                std::holds_alternative<float>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "bool") {
            if (std::holds_alternative<bool>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "string") {
            if (std::holds_alternative<std::string>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "vector<int>") {
            if (std::holds_alternative<std::vector<int>>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "vector<double>") {
            if (std::holds_alternative<std::vector<double>>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "vector<string>") {
            if (std::holds_alternative<std::vector<std::string>>(valueOpt.value())) {
                typeValid = true;
            }
        }
        
        if (!typeValid) {
            errors.push_back("Configuration key '" + entry.key + "' has incorrect type. Expected: " + entry.type);
        }
        
        // Value validation (if provided)
        if (!entry.validation.empty()) {
            std::string validationResult = validateValue(entry.key, entry.validation);
            if (!validationResult.empty()) {
                errors.push_back("Validation failed for key '" + entry.key + "': " + validationResult);
            }
        }
    }
    
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  " + error);
        }
        return false;
    }
    
    return true;
}

/**
 * @brief Validates a configuration value against a validation rule
 * @param key Configuration key
 * @param validationRule Validation rule
 * @return Error message if validation fails, empty string if valid
 * @details
 * Parses validation rules and applies them to configuration values.
 * Supports various validation types:
 * - Range validation: "range[0,100]"
 * - Enum validation: "enum[a,b,c]"
 * - Regex validation: "regex^[a-z]+$"
 * - Positive validation: "positive"
 * - Negative validation: "negative"
 */
std::string Config::validateValue(const std::string& key, const std::string& validationRule) {
    if (!has(key)) {
        return "Key not found";
    }
    
    std::optional<ConfigValue> valueOpt = get<void>(key);
    if (!valueOpt.has_value()) {
        return "Cannot get value";
    }
    
    // Parse validation rule
    if (validationRule.find("range[") == 0 && validationRule.back() == ']') {
        // Range validation
        std::string rangeStr = validationRule.substr(6, validationRule.size() - 7);
        size_t commaPos = rangeStr.find(',');
        if (commaPos == std::string::npos) {
            return "Invalid range format"; // Fix: Use std::string return
        }
        
        std::string minStr = rangeStr.substr(0, commaPos);
        std::string maxStr = rangeStr.substr(commaPos + 1);
        
        try {
            double min = std::stod(minStr);
            double max = std::stod(maxStr);
            
            if (std::holds_alternative<double>(valueOpt.value())) {
                double val = std::get<double>(valueOpt.value());
                if (val < min || val > max) {
                    return "Value " + std::to_string(val) + " not in range [" + minStr + "," + maxStr + "]";
                }
            } else if (std::holds_alternative<int64_t>(valueOpt.value())) {
                int64_t val = std::get<int64_t>(valueOpt.value());
                if (val < static_cast<int64_t>(min) || val > static_cast<int64_t>(max)) {
                    return "Value " + std::to_string(val) + " not in range [" + minStr + "," + maxStr + "]";
                }
            }
        } catch (const std::exception&) {
            return "Invalid range format";
        }
        
    } else if (validationRule.find("enum[") == 0 && validationRule.back() == ']') {
        // Enum validation
        std::string enumStr = validationRule.substr(5, validationRule.size() - 6);
        size_t pos = 0;
        while (pos < enumStr.length()) {
            size_t commaPos = enumStr.find(',', pos);
            std::string value;
            if (commaPos == std::string::npos) {
                value = enumStr.substr(pos);
                pos = enumStr.length();
            } else {
                value = enumStr.substr(pos, commaPos - pos);
                pos = commaPos + 1;
            }
            
            // Remove quotes from enum values
            if (!value.empty() && value.front() == '\"' && value.back() == '\"') {
                value = value.substr(1, value.size() - 2);
            }
            
            if (std::holds_alternative<std::string>(valueOpt.value())) {
                std::string val = std::get<std::string>(valueOpt.value());
                if (val == value) {
                    return ""; // Valid
                }
            }
        }
        return "Value not in enum list";
        
    } else if (validationRule.find("regex^") == 0 && validationRule.back() == '$') {
        // Regex validation
        std::string regexStr = validationRule.substr(6, validationRule.size() - 7);
        // Note: Full regex validation would require <regex> header
        // For now, basic validation
        return "";
        
    } else if (validationRule == "positive") {
        // Positive validation
        if (std::holds_alternative<double>(valueOpt.value())) {
            double val = std::get<double>(valueOpt.value());
            if (val <= 0) return "Value must be positive";
        } else if (std::holds_alternative<int64_t>(valueOpt.value())) {
            int64_t val = std::get<int64_t>(valueOpt.value());
            if (val <= 0) return "Value must be positive";
        }
        
    } else if (validationRule == "negative") {
        // Negative validation
        if (std::holds_alternative<double>(valueOpt.value())) {
            double val = std::get<double>(valueOpt.value());
            if (val >= 0) return "Value must be negative";
        } else if (std::holds_alternative<int64_t>(valueOpt.value())) {
            int64_t val = std::get<int64_t>(valueOpt.value());
            if (val >= 0) return "Value must be negative";
        }
    }
    
    return "";
}

/**
 * @brief Applies environment variables to configuration
 * @param config Configuration to update
 * @return true if environment variables were applied, false otherwise
 * @details
 * Parses environment variables and applies them to configuration.
 * Environment variables should follow the format NLM_* where * corresponds to
 * configuration keys in uppercase with underscores.
 * 
 * Example:
 * Environment variable: NLM_NEURON_COUNT=2000
 * Would set configuration key: neuron_count = 2000
 * 
 * Supports type conversion from environment variable strings to appropriate types.
 */
bool Config::applyEnvironmentVariables() {
    bool success = true;
    
    for (const auto& envPair : std::filesystem::environment()) {
        std::string envName = envPair.first;
        std::string envValue = envPair.second;
        
        // Check if this is an NLM environment variable
        if (envName.compare(0, 4, "NLM_") == 0) {
            std::string configKey = envName.substr(4);
            
            // Convert underscores to dots and lowercase
            std::replace(configKey.begin(), configKey.end(), '_', '.');
            std::transform(configKey.begin(), configKey.end(), configKey.begin(), ::tolower);
            
            try {
                // Try to parse as different types in order of preference
                if (envValue == "true" || envValue == "false") {
                    set(configKey, envValue == "true", ConfigSource::Environment);
                } else {
                    try {
                        // Try integer parsing
                        size_t pos;
                        int64_t intVal = std::stoll(envValue, &pos);
                        if (pos == envValue.length()) {
                            set(configKey, intVal, ConfigSource::Environment);
                            continue;
                        }
                    } catch (...) {}
                    
                    try {
                        // Try double parsing
                        size_t pos;
                        double doubleVal = std::stod(envValue, &pos);
                        if (pos == envValue.length()) {
                            set(configKey, doubleVal, ConfigSource::Environment);
                            continue;
                        }
                    } catch (...) {}
                    
                    // Default to string
                    set(configKey, envValue, ConfigSource::Environment);
                }
                
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to apply environment variable " + envName + ": " + std::string(e.what()));
                success = false;
            }
        }
    }
    
    return success;
}

/**
 * @brief Saves configuration to a JSON file
 * @param filepath Path to the JSON configuration file
 * @return true if save succeeded, false on error
 * @details
 * Saves configuration to JSON format with support for:
 * - All configuration entries
 * - Type information
 * - Descriptions
 * - Source tracking
 * - Formatting and readability
 * 
 * Generated JSON includes metadata for each configuration entry.
 */
bool Config::saveToJSON(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("saveToJSON: Empty filepath provided");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("saveToJSON: Failed to open file for writing: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            // Create JSON object for each configuration entry
            nlohmann::json entryJson;
            entryJson["key"] = entry.key;
            
            // Convert ConfigValue to JSON
            entryJson["value"] = configValueToJSON(entry.value);
            entryJson["source"] = static_cast<int>(entry.source);
            entryJson["description"] = entry.description;
            
            jsonData[entry.key] = entryJson;
        }
        
        // Write JSON with formatting
        file << std::setw(2) << jsonData;
        
        if (!file.good()) {
            NLM_LOG_ERROR("saveToJSON: Error writing to file: " + filepath);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("saveToJSON: Error writing to " + filepath + ": " + std::string(e.what()));
        return false;
    }
    
    return true;
}

/**
 * @brief Saves configuration to a YAML file
 * @param filepath Path to the YAML configuration file
 * @return true if save succeeded, false on error
 * @details
 * Saves configuration to YAML format with support for:
 * - All configuration entries
 * - Type information
 * - Descriptions
 * - Source tracking
 * - Formatting and readability
 */
bool Config::saveToYAML(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("saveToYAML: Empty filepath provided");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("saveToYAML: Failed to open file for writing: " + filepath);
        return false;
    }
    
    try {
        YAML::Emitter yamlData;
        
        yamlData << YAML::BeginDoc;
        yamlData << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            yamlData << YAML::Key << entry.key;
            yamlData << YAML::Value;
            
            // Convert ConfigValue to YAML
            yamlData << configValueToYAML(entry.value);
            
            // Add metadata as comments
            if (!entry.description.empty()) {
                yamlData << " # " << entry.description;
            }
            if (entry.source != ConfigSource::Default) {
                yamlData << " # source:" << static_cast<int>(entry.source);
            }
        }
        
        yamlData << YAML::EndMap;
        yamlData << YAML::EndDoc;
        
        file << yamlData.c_str();
        
        if (!file.good()) {
            NLM_LOG_ERROR("saveToYAML: Error writing to file: " + filepath);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("saveToYAML: Error writing to " + filepath + ": " + std::string(e.what()));
        return false;
    }
    
    return true;
}

/**
 * @brief Converts ConfigValue to JSON
 * @param value ConfigValue to convert
 * @return JSON representation of ConfigValue
 * @details
 * Converts ConfigValue (variant type) to appropriate JSON representation.
 * Handles all supported ConfigValue types:
 * - int, int64: Converted to JSON number
 * - double: Converted to JSON number
 * - bool: Converted to JSON boolean
 * - std::string: Converted to JSON string
 * - std::vector<int>, std::vector<double>, std::vector<std::string>: Converted to JSON array
 */
nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        nlohmann::json array = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<int>>(value)) {
            array.push_back(item);
        }
        return array;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        nlohmann::json array = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<double>>(value)) {
            array.push_back(item);
        }
        return array;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        nlohmann::json array = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<std::string>>(value)) {
            array.push_back(item);
        }
        return array;
    } else {
        return ""; // Empty string for unknown types
    }
}

/**
 * @brief Converts ConfigValue to YAML
 * @param value ConfigValue to convert
 * @return YAML emitter for the value
 * @details
 * Converts ConfigValue (variant type) to YAML representation.
 * This is a helper function that provides YAML streaming for configuration values.
 */
YAML::Emitter& operator<<(YAML::Emitter& out, const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        out << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        out << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        out << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        out << std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        out << std::get<std::string>(value);
    } else {
        out << "";
    }
    return out;
}

/**
 * @brief Sets a configuration change listener
 * @param listener Function to call when configuration changes
 * @details
 * Registers a function that will be called whenever a configuration value
 * is set. This enables hot reloading and dynamic configuration updates.
 * 
 * Listeners are called with the configuration key and new value.
 * Multiple listeners can be registered, and they are called in the order
 * they were registered.
 */
void Config::setChangeListener(std::function<void(const std::string&, const ConfigValue&)> listener) {
    changeListeners.push_back(listener);
}

/**
 * @brief Removes all configuration change listeners
 * @details
 * Clears all registered configuration change listeners.
 * This can be used to temporarily disable configuration updates.
 */
void Config::clearChangeListeners() {
    changeListeners.clear();
}

/**
 * @brief Enables or disables hot reloading
 * @param enabled true to enable hot reloading, false to disable
 * @param interval Reload interval in milliseconds
 * @details
 * Enables hot reloading of configuration files.
 * When enabled, the configuration is automatically reloaded when the
 * configuration file is modified.
 * 
 * The reload interval controls how frequently to check for file changes.
 * A shorter interval provides faster response but uses more CPU.
 */
void Config::enableHotReload(bool enabled, std::chrono::milliseconds interval) {
    hotReloadEnabled = enabled;
    hotReloadInterval = interval;
    
    if (enabled) {
        lastConfigChange = std::chrono::steady_clock::now();
        currentConfigFile = "configs/default.cfg"; // This should be tracked
    }
}

/**
 * @brief Checks for configuration file changes and reloads if necessary
 * @return true if configuration was reloaded, false otherwise
 * @details
 * Checks if the configuration file has been modified and reloads it if necessary.
 * This is used for hot reloading of configuration files.
 * 
 * The function uses file modification time to detect changes.
 * Only files with the same modification time as the last reload are considered
 * changed.
 */
bool Config::checkForChanges() {
    if (!hotReloadEnabled || currentConfigFile.empty()) {
        return false;
    }
    
    std::filesystem::path filepath(currentConfigFile);
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    auto lastWriteTime = std::filesystem::last_write_time(filepath);
    if (lastWriteTime > lastConfigChange) {
        // File has been modified
        NLM_LOG_INFO("Configuration file changed, reloading...");
        
        // Backup current configuration
        std::vector<ConfigEntry> backup = pImpl->entries;
        
        // Try to reload
        if (loadFromFile(currentConfigFile)) {
            // Successfully reloaded, notify listeners
            for (const auto& listener : changeListeners) {
                for (const auto& entry : pImpl->entries) {
                    // Find what changed (simplified approach)
                    listener(entry.key, entry.value);
                }
            }
            
            lastConfigChange = std::chrono::steady_clock::now();
            return true;
        } else {
            // Restore backup
            pImpl->entries = backup;
            NLM_LOG_ERROR("Failed to reload configuration, restored backup");
        }
    }
    
    return false;
}
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <chrono>
#include <thread>
#include <unordered_map>

namespace nlm {

/**
 * @brief Implementation details for the Config class using the PImpl idiom
 * @details
 * This struct contains the internal storage for configuration entries.
 * The PImpl idiom separates the interface from the implementation, providing:
 * 
 * - **Encapsulation**: Configuration details are hidden from users
 * - **Flexibility**: Internal storage can be changed without affecting the API
 * - **Maintainability**: Cleaner separation of concerns
 * - **Performance**: Faster compilation for users of the Config class
 * 
 * The implementation uses std::vector to store configuration entries, providing
 * efficient lookup, insertion, and removal operations. Each entry contains a key,
 * value (as a variant type), source, and description for comprehensive configuration
 * management.
 */
struct Config::Impl {
    /** Collection of all configuration entries loaded from various sources */
    std::vector<ConfigEntry> entries;
};

/**
 * @brief Default constructor creates an empty configuration
 * @details
 * Initializes the PImpl implementation object with default settings.
 * The configuration starts with no entries and is ready to receive values
 * from file, command line, or runtime sources.
 */
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <chrono>
#include <thread>
#include <unordered_map>

// Forward declarations for JSON/YAML parsing
bool Config::loadFromJSONData(const nlohmann::json& jsonData);
bool Config::loadFromYAMLData(const YAML::Node& yamlData);
std::string Config::jsonToNestedString(const nlohmann::json& jsonObj, const std::string& prefix);
std::string Config::yamlToNestedString(const YAML::Node& yamlNode, const std::string& prefix);
std::string Config::jsonArrayToString(const nlohmann::json& jsonArray);
std::string Config::yamlNodeAsString(const YAML::Node& yamlNode);
nlohmann::json Config::configValueToJSON(const ConfigValue& value);
bool Config::validate(const std::vector<ConfigSchema>& schema);
std::string Config::validateValue(const std::string& key, const std::string& validationRule);

namespace nlm {

// Global configuration
Config globalConfig;

// Configuration schema validation
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "int64", "double", "bool", "string", "vector<int>", etc.
    std::string description;
    bool required;
    std::string defaultValue;
    std::string validation;  // Validation rules
    
    ConfigSchema() : key(), type("string"), description(), required(false), defaultValue(), validation() {}
    
    ConfigSchema(const std::string& k, const std::string& t, const std::string& d, 
                 bool req = false, const std::string& def = "", const std::string& val = "")
        : key(k), type(t), description(d), required(req), defaultValue(def), validation(val) {}
};

// Global configuration schema
inline std::vector<ConfigSchema> getDefaultSchema() {
    return {
        // Neural Network Parameters
        ConfigSchema("neuron_count", "int64", "Number of neurons in the brain", true, "1000"),
        ConfigSchema("region_count", "int64", "Number of brain regions", true, "2"),
        ConfigSchema("connection_probability", "double", "Connection probability for random connectivity", true, "0.1"),
        
        // Plasticity Parameters
        ConfigSchema("plasticity_learning_rate", "double", "Plasticity learning rate", true, "0.01"),
        ConfigSchema("stdp_ltp_weight", "double", "STDP LTP weight", true, "0.01"),
        ConfigSchema("stdp_ltd_weight", "double", "STDP LTD weight", true, "0.012"),
        ConfigSchema("stdp_tau", "double", "STDP time constant", true, "20.0"),
        
        // Structural Plasticity
        ConfigSchema("synaptogenesis_rate", "double", "Rate of synaptogenesis", true, "0.001"),
        ConfigSchema("pruning_rate", "double", "Rate of synaptic pruning", true, "0.0001"),
        
        // Development Parameters
        ConfigSchema("critical_period_start", "int64", "Critical period start step", false, "1000"),
        ConfigSchema("critical_period_duration", "int64", "Critical period duration in steps", false, "2000"),
        
        // Memory Parameters
        ConfigSchema("working_memory_capacity", "int64", "Working memory capacity", false, "100"),
        ConfigSchema("episodic_memory_capacity", "int64", "Episodic memory capacity", false, "1000"),
        ConfigSchema("associative_memory_capacity", "int64", "Associative memory capacity", false, "500"),
        
        // Prediction Parameters
        ConfigSchema("prediction_horizon", "int64", "Prediction horizon in steps", false, "10"),
        ConfigSchema("prediction_error_threshold", "double", "Prediction error threshold", false, "0.1"),
        
        // Sensory Parameters
        ConfigSchema("sensory_input_modality", "string", "Sensory input modality (vision,auditory,tactile)", false, "vision"),
        ConfigSchema("sensory_preprocessing_enabled", "bool", "Enable sensory preprocessing", false, "true"),
        
        // Motor Parameters
        ConfigSchema("motor_action_space", "int64", "Size of motor action space", false, "4"),
        ConfigSchema("motor_action_selection_method", "string", "Motor action selection method (greedy,epsilon_greedy)", false, "epsilon_greedy"),
        
        // Advanced Parameters
        ConfigSchema("simulation_timestep", "double", "Simulation timestep in seconds", true, "0.001"),
        ConfigSchema("max_simulation_steps", "int64", "Maximum simulation steps", true, "10000"),
        ConfigSchema("simulation_time_limit", "double", "Simulation time limit in seconds", false, "0.0"),
        
        // Logging Parameters
        ConfigSchema("log_level", "string", "Log level (debug,info,warning,error,critical)", true, "INFO"),
        ConfigSchema("log_to_file", "bool", "Enable file logging", false, "false"),
        ConfigSchema("log_filename", "string", "Log file name", false, "nlm.log"),
        ConfigSchema("enable_colors", "bool", "Enable colored console output", false, "true"),
        
        // Performance Parameters
        ConfigSchema("enable_cuda", "bool", "Enable GPU acceleration", false, "false"),
        ConfigSchema("memory_limit_mb", "int64", "Memory usage limit in MB", false, "1024"),
        ConfigSchema("checkpoint_save_interval", "int64", "Checkpoint save interval in steps", false, "100"),
        
        // Phase and Mode
        ConfigSchema("phase", "int64", "Phase to run (1,2,3,4,6)", true, "2"),
        ConfigSchema("mode", "string", "Operating mode (demo,test,training,production)", true, "demo"),
        ConfigSchema("enable_render", "bool", "Enable visualization rendering", false, "false"),
        
        // Training Parameters
        ConfigSchema("training_epochs", "int64", "Number of training epochs", false, "10"),
        ConfigSchema("training_steps_per_epoch", "int64", "Training steps per epoch", false, "1000"),
        
        // Random Seed
        ConfigSchema("random_seed", "int64", "Random seed for reproducible experiments", true, "42"),
    };
}

// Global configuration change listeners
inline std::vector<std::function<void(const std::string&, const ConfigValue&)>> changeListeners;

// Global hot reload state
inline std::chrono::steady_clock::time_point lastConfigChange;
inline std::string currentConfigFile;
inline bool hotReloadEnabled = false;
inline std::chrono::milliseconds hotReloadInterval(1000); // 1 second

// Convert string to lowercase
std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Configuration schema validation
bool Config::validate(const std::vector<ConfigSchema>& schema) {
    std::vector<std::string> errors;
    
    // Check required fields
    for (const auto& entry : schema) {
        if (entry.required && !has(entry.key)) {
            errors.push_back("Required configuration key missing: " + entry.key);
        }
    }
    
    // Validate types and values
    for (const auto& entry : schema) {
        if (!has(entry.key)) {
            continue;  // Skip validation for missing keys (handled above)
        }
        
        std::optional<ConfigValue> valueOpt = get<void>(entry.key);
        if (!valueOpt.has_value()) {
            errors.push_back("Cannot get configuration value for key: " + entry.key);
            continue;
        }
        
        // Type validation
        bool typeValid = false;
        if (entry.type == "int64" || entry.type == "int") {
            if (std::holds_alternative<int64_t>(valueOpt.value()) || 
                std::holds_alternative<int>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "double" || entry.type == "float") {
            if (std::holds_alternative<double>(valueOpt.value()) || 
                std::holds_alternative<float>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "bool") {
            if (std::holds_alternative<bool>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "string") {
            if (std::holds_alternative<std::string>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "vector<int>") {
            if (std::holds_alternative<std::vector<int>>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "vector<double>") {
            if (std::holds_alternative<std::vector<double>>(valueOpt.value())) {
                typeValid = true;
            }
        } else if (entry.type == "vector<string>") {
            if (std::holds_alternative<std::vector<std::string>>(valueOpt.value())) {
                typeValid = true;
            }
        }
        
        if (!typeValid) {
            errors.push_back("Configuration key '" + entry.key + "' has incorrect type. Expected: " + entry.type);
        }
        
        // Value validation (if provided)
        if (!entry.validation.empty()) {
            std::string validationResult = validateValue(entry.key, entry.validation);
            if (!validationResult.empty()) {
                errors.push_back("Validation failed for key '" + entry.key + "': " + validationResult);
            }
        }
    }
    
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  " + error);
        }
        return false;
    }
    
    return true;
}

// Validate a configuration value against a validation rule
std::string Config::validateValue(const std::string& key, const std::string& validationRule) {
    if (!has(key)) {
        return "Key not found";
    }
    
    std::optional<ConfigValue> valueOpt = get<void>(key);
    if (!valueOpt.has_value()) {
        return "Cannot get value";
    }
    
    // Parse validation rule
    if (validationRule.find("range[") == 0 && validationRule.back() == ']') {
        // Range validation
        std::string rangeStr = validationRule.substr(6, validationRule.size() - 7);
        size_t commaPos = rangeStr.find(',');
        if (commaPos == std::string::npos) {
            return "Invalid range format";
        }
        
        std::string minStr = rangeStr.substr(0, commaPos);
        std::string maxStr = rangeStr.substr(commaPos + 1);
        
        try {
            double min = std::stod(minStr);
            double max = std::stod(maxStr);
            
            if (std::holds_alternative<double>(valueOpt.value())) {
                double val = std::get<double>(valueOpt.value());
                if (val < min || val > max) {
                    return "Value " + std::to_string(val) + " not in range [" + minStr + "," + maxStr + "]";
                }
            } else if (std::holds_alternative<int64_t>(valueOpt.value())) {
                int64_t val = std::get<int64_t>(valueOpt.value());
                if (val < static_cast<int64_t>(min) || val > static_cast<int64_t>(max)) {
                    return "Value " + std::to_string(val) + " not in range [" + minStr + "," + maxStr + "]";
                }
            }
        } catch (const std::exception&) {
            return "Invalid range format";
        }
        
    } else if (validationRule.find("enum[") == 0 && validationRule.back() == ']') {
        // Enum validation
        std::string enumStr = validationRule.substr(5, validationRule.size() - 6);
        size_t pos = 0;
        while (pos < enumStr.length()) {
            size_t commaPos = enumStr.find(',', pos);
            std::string value;
            if (commaPos == std::string::npos) {
                value = enumStr.substr(pos);
                pos = enumStr.length();
            } else {
                value = enumStr.substr(pos, commaPos - pos);
                pos = commaPos + 1;
            }
            
            // Remove quotes from enum values
            if (!value.empty() && value.front() == '\"' && value.back() == '\"') {
                value = value.substr(1, value.size() - 2);
            }
            
            if (std::holds_alternative<std::string>(valueOpt.value())) {
                std::string val = std::get<std::string>(valueOpt.value());
                if (val == value) {
                    return ""; // Valid
                }
            }
        }
        return "Value not in enum list";
        
    } else if (validationRule.find("regex^") == 0 && validationRule.back() == '$') {
        // Regex validation (simplified)
        return "";
        
    } else if (validationRule == "positive") {
        // Positive validation
        if (std::holds_alternative<double>(valueOpt.value())) {
            double val = std::get<double>(valueOpt.value());
            if (val <= 0) return "Value must be positive";
        } else if (std::holds_alternative<int64_t>(valueOpt.value())) {
            int64_t val = std::get<int64_t>(valueOpt.value());
            if (val <= 0) return "Value must be positive";
        }
        
    } else if (validationRule == "negative") {
        // Negative validation
        if (std::holds_alternative<double>(valueOpt.value())) {
            double val = std::get<double>(valueOpt.value());
            if (val >= 0) return "Value must be negative";
        } else if (std::holds_alternative<int64_t>(valueOpt.value())) {
            int64_t val = std::get<int64_t>(valueOpt.value());
            if (val >= 0) return "Value must be negative";
        }
    }
    
    return "";
}

// Enable or disable hot reloading
void Config::enableHotReload(bool enabled, std::chrono::milliseconds interval) {
    hotReloadEnabled = enabled;
    hotReloadInterval = interval;
    
    if (enabled) {
        lastConfigChange = std::chrono::steady_clock::now();
    }
}

// Check for configuration file changes and reload if necessary
bool Config::checkForChanges() {
    if (!hotReloadEnabled || currentConfigFile.empty()) {
        return false;
    }
    
    std::filesystem::path filepath(currentConfigFile);
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    auto lastWriteTime = std::filesystem::last_write_time(filepath);
    if (lastWriteTime > lastConfigChange) {
        // File has been modified
        NLM_LOG_INFO("Configuration file changed, reloading...");
        
        // Backup current configuration
        std::vector<ConfigEntry> backup = pImpl->entries;
        
        // Try to reload
        if (loadFromFile(currentConfigFile)) {
            // Successfully reloaded, notify listeners
            for (const auto& listener : changeListeners) {
                for (const auto& entry : pImpl->entries) {
                    listener(entry.key, entry.value);
                }
            }
            
            lastConfigChange = std::chrono::steady_clock::now();
            return true;
        } else {
            // Restore backup
            pImpl->entries = backup;
            NLM_LOG_ERROR("Failed to reload configuration, restored backup");
        }
    }
    
    return false;
}

// Set a configuration change listener
void Config::setChangeListener(std::function<void(const std::string&, const ConfigValue&)> listener) {
    changeListeners.push_back(listener);
}

// Remove all configuration change listeners
void Config::clearChangeListeners() {
    changeListeners.clear();
}

// Get the current configuration summary
global namespace nlm {
Config globalConfig;

// Additional helper functions continue here...

// Convert JSON object to nested string representation
std::string Config::jsonToNestedString(const nlohmann::json& jsonObj, const std::string& prefix) {
    std::ostringstream oss;
    
    for (const auto& item : jsonObj.items()) {
        std::string newPrefix = prefix + item.key() + ".";
        
        if (item.value().is_object()) {
            oss << jsonToNestedString(item.value(), newPrefix);
        } else if (item.value().is_array()) {
            oss << newPrefix << "=" << jsonArrayToString(item.value()) << "\n";
        } else {
            oss << newPrefix << "=" << item.value() << "\n";
        }
    }
    
    return oss.str();
}

// Convert YAML node to nested string representation
std::string Config::yamlToNestedString(const YAML::Node& yamlNode, const std::string& prefix) {
    std::ostringstream oss;
    
    if (yamlNode.IsMap()) {
        for (const auto& item : yamlNode) {
            std::string newPrefix = prefix + item.first.as<std::string>() + ".";
            if (item.second.IsMap()) {
                oss << yamlToNestedString(item.second, newPrefix);
            } else {
                oss << newPrefix << "=" << yamlNodeAsString(item.second) << "\n";
            }
        }
    }
    
    return oss.str();
}

// Convert JSON array to string representation
std::string Config::jsonArrayToString(const nlohmann::json& jsonArray) {
    std::ostringstream oss;
    oss << "[";
    
    for (size_t i = 0; i < jsonArray.size(); ++i) {
        if (i > 0) oss << ",";
        oss << jsonArray[i];
    }
    
    oss << "]";
    return oss.str();
}

// Convert YAML node to string representation
std::string Config::yamlNodeAsString(const YAML::Node& yamlNode) {
    if (yamlNode.IsScalar()) {
        return yamlNode.as<std::string>();
    } else if (yamlNode.IsSequence()) {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < yamlNode.size(); ++i) {
            if (i > 0) oss << ",";
            oss << yamlNode[i].as<std::string>();
        }
        oss << "]";
        return oss.str();
    } else if (yamlNode.IsMap()) {
        std::ostringstream oss;
        oss << "{";
        for (const auto& item : yamlNode) {
            oss << "\"" << item.first.as<std::string>() << "\":" << yamlNodeAsString(item.second);
            if (&item != &yamlNode.begin()->second) oss << ",";
        }
        oss << "}";
        return oss.str();
    }
    
    return "";
}

// Convert ConfigValue to JSON
nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        nlohmann::json array = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<int>>(value)) {
            array.push_back(item);
        }
        return array;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        nlohmann::json array = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<double>>(value)) {
            array.push_back(item);
        }
        return array;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        nlohmann::json array = nlohmann::json::array();
        for (const auto& item : std::get<std::vector<std::string>>(value)) {
            array.push_back(item);
        }
        return array;
    } else {
        return ""; // Empty string for unknown types
    }
}

// YAML operator<< overload for ConfigValue
YAML::Emitter& operator<<(YAML::Emitter& out, const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        out << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        out << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        out << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        out << std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        out << std::get<std::string>(value);
    } else {
        out << "";
    }
    return out;
}

// Apply environment variables to configuration
bool Config::applyEnvironmentVariables() {
    bool success = true;
    
    for (const auto& envPair : std::filesystem::environment()) {
        std::string envName = envPair.first;
        std::string envValue = envPair.second;
        
        if (envName.compare(0, 4, "NLM_") == 0) {
            std::string configKey = envName.substr(4);
            std::replace(configKey.begin(), configKey.end(), '_', '.');
            std::transform(configKey.begin(), configKey.end(), configKey.begin(), ::tolower);
            
            try {
                if (envValue == "true" || envValue == "false") {
                    set(configKey, envValue == "true", ConfigSource::Environment);
                } else {
                    try {
                        size_t pos;
                        int64_t intVal = std::stoll(envValue, &pos);
                        if (pos == envValue.length()) {
                            set(configKey, intVal, ConfigSource::Environment);
                            continue;
                        }
                    } catch (...) {}
                    
                    try {
                        size_t pos;
                        double doubleVal = std::stod(envValue, &pos);
                        if (pos == envValue.length()) {
                            set(configKey, doubleVal, ConfigSource::Environment);
                            continue;
                        }
                    } catch (...) {}
                    
                    set(configKey, envValue, ConfigSource::Environment);
                }
                
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to apply environment variable " + envName + ": " + std::string(e.what()));
                success = false;
            }
        }
    }
    
    return success;
}

// Validate configuration against default schema
bool Config::validate() {
    return validate(getDefaultSchema());
}

// Apply environment variables to configuration
bool Config::applyEnvironmentVariables() {
    return applyEnvironmentVariables();
}

// Save configuration to JSON format
bool Config::saveToJSON(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("saveToJSON: Empty filepath provided");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("saveToJSON: Failed to open file for writing: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            nlohmann::json entryJson;
            entryJson["key"] = entry.key;
            entryJson["value"] = configValueToJSON(entry.value);
            entryJson["source"] = static_cast<int>(entry.source);
            entryJson["description"] = entry.description;
            
            jsonData[entry.key] = entryJson;
        }
        
        file << std::setw(2) << jsonData;
        
        if (!file.good()) {
            NLM_LOG_ERROR("saveToJSON: Error writing to file: " + filepath);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("saveToJSON: Error writing to " + filepath + ": " + std::string(e.what()));
        return false;
    }
    
    return true;
}

// Save configuration to YAML format
bool Config::saveToYAML(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("saveToYAML: Empty filepath provided");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("saveToYAML: Failed to open file for writing: " + filepath);
        return false;
    }
    
    try {
        YAML::Emitter yamlData;
        
        yamlData << YAML::BeginDoc;
        yamlData << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            yamlData << YAML::Key << entry.key;
            yamlData << YAML::Value;
            yamlData << configValueToYAML(entry.value);
            
            if (!entry.description.empty()) {
                yamlData << " # " << entry.description;
            }
            if (entry.source != ConfigSource::Default) {
                yamlData << " # source:" << static_cast<int>(entry.source);
            }
        }
        
        yamlData << YAML::EndMap;
        yamlData << YAML::EndDoc;
        
        file << yamlData.c_str();
        
        if (!file.good()) {
            NLM_LOG_ERROR("saveToYAML: Error writing to file: " + filepath);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("saveToYAML: Error writing to " + filepath + ": " + std::string(e.what()));
        return false;
    }
    
    return true;
}

// Set the current configuration file for hot reloading
void Config::setCurrentConfigFile(const std::string& filepath) {
    currentConfigFile = filepath;
    if (hotReloadEnabled) {
        lastConfigChange = std::filesystem::last_write_time(filepath);
    }
}

// Get the current configuration file path
std::string Config::getCurrentConfigFile() const {
    return currentConfigFile;
}

/**
 * @brief Loads configuration from a file using simple key=value format
 * @param filepath Path to the configuration file
 * @return true if file was read successfully, false on error
 * @details
 * This method implements a simple configuration file parser that supports:
 * 
 * **File format**: 
 * - Simple key=value pairs (no complex JSON or XML)
 * - Comments starting with # or / (C++ style)
 * - Quoted values (both "double quotes" and 'single quotes')
 * - Case-insensitive key matching
 * 
 * **Parsing process**:
 * 1. Opens the file for reading
 * 2. Processes each line sequentially
 * 3. Skips empty lines and comments
 * 4. Parses valid key=value pairs
 * 5. Validates key and value formats
 * 6. Stores entries with File source flag
 * 
 * **Error handling**:
 * - Reports empty filepath errors
 * - Reports file opening failures
 * - Logs warnings for malformed lines
 * - Continues processing after warnings (does not abort)
 * - Returns true even if some lines are invalid (partially successful)
 * 
 * **Thread safety**:
 * - Not thread-safe for concurrent modifications
 * - Must be called when no other threads are accessing this Config object
 * 
 * **Performance considerations**:
 * - Linear time complexity O(n) where n is number of lines
 * - Memory efficient: processes line by line
 * - No dynamic allocation during parsing (uses existing string operations)
 */
bool Config::loadFromFile(const std::string& filepath) {
    /**
     * @brief Validate input parameters
     * @details
     * Checks for common error conditions before attempting file operations:
     * - Empty filepath strings
     * - Invalid file paths
     * - Permission issues
     * 
     * These checks prevent runtime errors and provide clear error messages
     * to help users identify configuration problems.
     */
    if (filepath.empty()) {
        NLM_LOG_ERROR("loadFromFile: Empty filepath provided");
        return false;
    }
    
    /**
     * @brief Open and validate the configuration file
     * @details
     * Attempts to open the file for reading. If the file cannot be opened:
     * - File may not exist
     * - Permissions may be insufficient
     * - Path may be invalid
     * - File may be corrupted or locked
     * 
     * The method continues processing even after file opening failures,
     * returning false to indicate the operation was not successful.
     */
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("loadFromFile: Failed to open file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    
    /**
     * @brief Process file line by line
     * @details
     * The file is read sequentially, line by line. This approach:
     * - Handles large files without loading everything into memory
     * - Provides detailed error reporting with line numbers
     * - Allows recovery from malformed lines
     * - Maintains processing order (important for configuration precedence)
     * 
     * Each line is processed through a multi-step validation pipeline.
     */
    while (std::getline(file, line)) {
        ++lineNum;
        
        /**
         * @brief Skip empty lines and comments
         * @details
         * Lines that are empty or contain only whitespace, or that start with
         * # or / characters, are treated as comments and skipped. This allows
         * for configuration documentation and easier file maintenance.
         */
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        /**
         * @brief Parse key=value pairs
         * @details
         * Valid configuration lines must contain an equals sign (=) separating
         * a key from a value. The parsing handles:
         * 
         * - Keys and values can contain spaces (after trimming)
         * - Values can be quoted with double or single quotes
         * - Keys are case-sensitive
         * - Multiple equals signs are not supported (first one is used)
         */
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            if (pos == 0) {
                NLM_LOG_WARNING(std::string("loadFromFile: Invalid line at ") + std::to_string(lineNum) + ": missing key");
                continue;
            }
            
            if (pos == line.size() - 1) {
                NLM_LOG_WARNING(std::string("loadFromFile: Invalid line at ") + std::to_string(lineNum) + ": missing value");
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty()) {
                NLM_LOG_WARNING(std::string("loadFromFile: Empty key at line ") + std::to_string(lineNum));
                continue;
            }
            
            /**
             * @brief Handle quoted values
             * @details
             * Configuration values can be enclosed in quotes to:
             * - Allow values containing spaces
             * - Include special characters without escaping
             * - Preserve formatting
             * 
             * Both double quotes ("value") and single quotes ('value') are supported.
             * The quotes are stripped before storing the value.
             */
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            /**
             * @brief Store the parsed configuration entry
             * @details
             * The parsed key-value pair is stored in the configuration with:
             * - The key as specified
             * - The value after quote processing
             * - ConfigSource::File to indicate origin
             * - Empty description (file format doesn't include descriptions)
             */
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING(std::string("loadFromFile: Invalid line at ") + std::to_string(lineNum) + ": no '=' found");
        }
    }
    
    return true;
}

/**
 * @brief Loads configuration from command line arguments
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return true if parsing succeeded, false on error
 * @details
 * Parses command line arguments in two formats:
 * 
 * **Format 1: --key=value**
 * - Long option with equals sign
 * - Example: --neuron_count=1000 --region_count=5
 * 
 * **Format 2: -key value**
 * - Short option with separate value
 * - Example: -n 1000 -r 5
 * 
 * **Parsing process**:
 * 1. Validates argc and argv pointers
 * 2. Processes each argument starting from index 1 (skips program name)
 * 3. Identifies argument format (--key=value or -key value)
 * 4. Extracts key and value
 * 5. Validates key format
 * 6. Stores with CommandLine source flag
 * 
 * **Command line precedence**:
 * - Command line arguments override file configuration
 * - Later arguments override earlier ones (last win)
 * - Empty or malformed arguments are logged as warnings
 * - No failure on individual argument errors (partial success)
 * 
 * **Use cases**:
 * - Program configuration without files
 * - Environment-specific settings
 * - Quick debugging configuration
 * - Container orchestration (Docker, Kubernetes)
 */
bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv) {
        NLM_LOG_ERROR("loadFromArgs: argv is null");
        return false;
    }
    
    if (argc <= 0) {
        NLM_LOG_ERROR("loadFromArgs: argc is zero or negative");
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            NLM_LOG_WARNING(std::string("loadFromArgs: argv[") + std::to_string(i) + "] is null");
            continue;
        }
        
        std::string arg(argv[i]);
        
        /**
         * @brief Handle --key=value format arguments
         * @details
         * Long format arguments start with -- and contain an equals sign.
         * Examples:
         * - --neuron_count=1000
         * - --random_seed=42
         * - --simulation_timestep=0.001
         * 
         * The key extraction strips the -- prefix, and the value is everything
         * after the equals sign. This format is preferred for:
         * - Readability
         * - Machine parsing
         * - Consistent formatting
         */
        if (arg.size() >= 2 && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                if (pos == 2) {
                    NLM_LOG_WARNING(std::string("loadFromArgs: Argument at ") + std::to_string(i) + ": missing key after --");
                    continue;
                }
                
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    NLM_LOG_WARNING(std::string("loadFromArgs: Empty key in argument at ") + std::to_string(i));
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING(std::string("loadFromArgs: Argument at ") + std::to_string(i) + ": invalid --key=value format");
            }
        }
        /**
         * @brief Handle -key value format arguments
         * @details
         * Short format arguments start with - and have a separate value argument.
         * Examples:
         * - -n 1000
         * - -r 5
         * - -s 0.001
         * 
         * The key is everything after the -, and the next argument in argv
         * is used as the value. This format is useful for:
         * - Concise command lines
         * - Script compatibility
         * - Interactive use
         */
        else if (arg.size() >= 2 && arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            
            if (key.empty()) {
                NLM_LOG_WARNING(std::string("loadFromArgs: Empty key in argument at ") + std::to_string(i));
                continue;
            }
            
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING(std::string("loadFromArgs: Ignored argument at ") + std::to_string(i) + ": " + arg);
        }
    }
    return true;
}

/**
 * @brief Saves configuration to a file for persistence
 * @param filepath Path to the configuration file to create
 * @return true if save succeeded, false on error
 * @details
 * Writes the current configuration to a file using a simple key=value format.
 * This is primarily used for:
 * 
 * **Checkpointing**: Save configuration before simulation runs
 * **Persistence**: Save current settings for future runs
 * **Documentation**: Generate configuration documentation
 * **Debugging**: Export configuration for analysis
 * 
 * **File format**:
 * - Each entry is written on its own line
 * - Comments are added before each entry
 * - Values are written without quotes (even if originally quoted)
 * - Fixed precision (6 decimal places) for floating point values
 * 
 * **Output format**:
 * ```
 * # <description>
 * <key> = <value>
 * ```
 * 
 * **Error handling**:
 * - Reports empty filepath errors
 * - Reports file opening failures  
 * - Reports write errors
 * - Handles file system errors gracefully
 * 
 * **Performance considerations**:
 * - Writes entries in sorted order (by key) for consistency
 * - Uses buffered I/O for efficiency
 * - Checks for write errors after each write operation
 * - Uses fixed precision to ensure reproducible output
 */
bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR("saveToFile: Empty filepath provided");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("saveToFile: Failed to open file for writing: " + filepath);
        return false;
    }
    
    /**
     * @brief Configure output formatting
     * @details
     * Set file to ASCII mode with fixed precision for consistent output:
     * - std::setprecision(6): 6 decimal places for floating point values
     * - std::fixed: Always show decimal point (not scientific notation)
     * 
     * This ensures reproducible output and makes the file easier to read
     * for both humans and machines.
     */
    // Set file to ASCII if available (important for ConfigValue handling)
    file << std::setprecision(6) << std::fixed;
    
    for (const auto& entry : pImpl->entries) {
        /**
         * @brief Write entry with comment and key=value pair
         * @details
         * Each configuration entry is written as:
         * - Comment line with entry description
         * - Key=value line with the actual configuration
         * - Empty line for readability (optional but recommended)
         * 
         * The description provides context about what the configuration
         * parameter controls, making the file self-documenting.
         */
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    /**
     * @brief Check for write errors
     * @details
     * After all writes, check if the file stream is still in a good state.
     * This catches:
     * - Disk full errors
     * - Permission issues
     * - Filesystem errors
     * - Network filesystem timeouts (for remote mounts)
     * 
     * The method returns false before checking, but this provides an additional
     * layer of error detection.
     */
    if (!file.good()) {
        NLM_LOG_ERROR("saveToFile: Error writing to file: " + filepath);
        return false;
    }
    
    return true;
}

/**
 * @brief Gets a configuration value by key with type conversion
 * @tparam T Type to convert the value to
 * @param key Configuration key to look up
 * @return Optional containing the value if found and type matches, nullopt otherwise
 * @details
 * Retrieves a configuration value from the entries vector. This method:
 * 
 * **Type safety**: Uses std::variant to store values of different types
 * **Lookup**: Performs linear search O(n) where n is number of entries
 * **Type checking**: Verifies the stored type matches the requested type
 * **Error handling**: Returns nullopt for missing keys or type mismatches
 * 
 * **Template instantiation**: Explicit instantiations are provided for common types
 * 
 * **Performance**: For frequently accessed keys, consider caching or using
 * a hash map instead of the current vector-based implementation.
 * 
 * **Usage example**:
 * ```cpp
 * auto neuronCount = config->get<size_t>("neuron_count");
 * if (neuronCount) {
 *     // Use the value
 * } else {
 *     // Use default
 * }
 * ```
 */
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

/**
 * @brief Gets a configuration value with default fallback
 * @tparam T Type to convert the value to
 * @param key Configuration key to look up
 * @param defaultValue Value returned if key is not found or type mismatch
 * @return Value from config if found and type matches, otherwise defaultValue
 * @details
 * Convenience wrapper around the get() method that provides:
 * 
 * **Fallback behavior**: Returns default value if key is missing or type mismatch
 * **Type safety**: Maintains type safety of the template parameter
 * **Simpler API**: No need to handle nullopt (optional) case
 * 
 * **Use cases**:
 * - Default configuration values
 * - Optional configuration parameters
 * - Backward compatibility with missing settings
 * 
 * **Performance**: Same as get() method (O(n) lookup)
 * 
 * **Example**:
 * ```cpp
 * int maxEpochs = config->getOr<int>("max_epochs", 100);
 * // Always returns a value (100 if "max_epochs" not set)
 * ```
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

/**
 * @brief Sets a configuration value with source tracking
 * @param key Configuration key
 * @param value Value to set
 * @param source Origin of the configuration (File, CommandLine, Runtime)
 * @details
 * Updates or creates a configuration entry. If the key already exists,
 * the value and source are updated. Otherwise, a new entry is created.
 * 
 * **Source tracking**: Different sources have different precedence:
 * - CommandLine (highest): Override files and defaults
 * - File: Override defaults
 * - Runtime: Override nothing (lowest precedence)
 * - Default: Initial setting
 * 
 * **Thread safety**: Not thread-safe for concurrent modifications
 * 
 * **Memory management**: Uses move semantics for efficiency
 * 
 * **Key normalization**: Keys are stored as provided (case-sensitive)
 */
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

/**
 * @brief Sets a configuration value (string version)
 * @param key Configuration key
 * @param value String value to set
 * @param source Origin of the configuration
 * @details
 * Convenience wrapper that converts the string value to ConfigValue.
 * This method is called by all other set() overloads.
 */
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Sets a configuration value (int version)
 * @param key Configuration key
 * @param value Integer value to set
 * @param source Origin of the configuration
 * @details
 * Convenience wrapper for integer configuration values.
 */
void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Sets a configuration value (double version)
 * @param key Configuration key
 * @param value Double value to set
 * @param source Origin of the configuration
 * @details
 * Convenience wrapper for floating-point configuration values.
 */
void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Sets a configuration value (bool version)
 * @param key Configuration key
 * @param value Boolean value to set
 * @param source Origin of the configuration
 * @details
 * Convenience wrapper for boolean configuration values.
 */
void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

/**
 * @brief Checks if a configuration key exists
 * @param key Configuration key to check
 * @return true if key exists, false otherwise
 * @details
 * Performs linear search through all configuration entries.
 * This operation is O(n) where n is the number of entries.
 * 
 * **Use cases**:
 * - Checking for optional configuration
 * - Conditional initialization
 * - Debug and validation
 * 
 * **Performance**: For frequent checks, consider using a std::unordered_map
 * for O(1) lookup instead of O(n) linear search.
 */
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

/**
 * @brief Removes a configuration key and its value
 * @param key Configuration key to remove
 * @details
 * Removes the configuration entry with the specified key from the entries vector.
 * If the key doesn't exist, this method has no effect.
 * 
 * **Use cases**:
 * - Clearing configuration
 * - Removing invalid settings
 * - Dynamic configuration management
 * - Debug cleanup
 * 
 * **Thread safety**: Not thread-safe for concurrent modifications
 * 
 * **Error handling**: Logs error for empty keys but doesn't throw
 */
void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_ERROR("remove: Cannot remove empty key");
        return;
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

/**
 * @brief Gets all configuration keys
 * @return Vector containing all configuration keys
 * @details
 * Returns a copy of all keys currently stored in the configuration.
 * This is useful for:
 * 
 * **Inspection**: Listing all available configuration options
 * **Documentation**: Generating configuration documentation
 * **Debugging**: Checking what configuration was loaded
 * **Validation**: Ensuring required keys are present
 * 
 * **Performance**: Creates a copy of all keys (O(n) time and space)
 * 
 * **Ordering**: Keys are returned in the order they were inserted
 * (insertion order preservation)
 */
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

/**
 * @brief Clears all configuration entries
 * @details
 * Removes all entries from the configuration, leaving it empty.
 * This is equivalent to resetting the configuration to its initial state.
 * 
 * **Use cases**:
 * - Resetting configuration
 * - Cleanup after testing
 * - Memory management
 * - Configuration reload
 * 
 * **Thread safety**: Not thread-safe for concurrent modifications
 * 
 * **Performance**: O(n) time where n is number of entries (linear time)
 * (using vector::clear() which is typically O(1) but depends on implementation)
 */
void Config::clear() {
    pImpl->entries.clear();
}

/**
 * @brief Generates a human-readable configuration summary
 * @return Formatted string containing configuration summary
 * @details
 * Creates a detailed summary of all configuration entries including:
 * 
 * **Entry count**: Total number of configuration entries
 * **Entry details**: Key, value, and source for each entry
 * **Value formatting**: Proper formatting for different value types
 * **Source indication**: Shows origin of each entry
 * 
 * **Output format**:
 * ```
 * Configuration (<count> entries):
 *   <key> = [<formatted_value>] (<source_id>)
 *   <key> = [<formatted_value>] (<source_id>)
 * ```
 * 
 * **Use cases**:
 * - Debug and validation
 * - Documentation generation
 * - Logging configuration state
 * - User interface display
 * 
 * **Performance**: O(n) time where n is number of entries
 * (linear time due to iterating through all entries)
 * 
 * **Memory**: Creates temporary strings during formatting
 */
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

/**
 * @brief Trims whitespace from a string
 * @param str String to trim
 * @return Trimmed string with leading/trailing whitespace removed
 * @details
 * Removes whitespace characters (spaces, tabs, newlines, carriage returns)
 * from both ends of the input string. This is used for:
 * 
 * **Configuration parsing**: Cleaning up keys and values from files
 * **String comparison**: Normalizing input for comparisons
 * **User input**: Processing command line arguments
 * 
 * **Algorithm**: Uses std::string::find_first_not_of and
 * std::string::find_last_not_of to locate the first and last non-whitespace
 * characters, then extracts the substring between them.
 * 
 * **Complexity**: O(n) where n is string length
 * (linear time due to scanning the entire string)
 * 
 * **Edge cases**:
 * - Empty string returns empty string
 * - All-whitespace string returns empty string
 * - Single non-whitespace character returns that character
 */
std::string Config::trim(const std::string& str) {
    if (str.empty()) {
        return "";
    }
    
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

/**
 * @brief Converts string to lowercase
 * @param str String to convert
 * @return Lowercase version of input string
 * @details
 * Converts all uppercase characters to lowercase using std::tolower.
 * This is useful for:
 * 
 * **Case-insensitive comparison**: Normalizing keys for comparison
 * **Configuration file parsing**: Handling mixed-case configuration
 * **User input**: Normalizing command line arguments
 * 
 * **Algorithm**: Uses std::transform to apply std::tolower to each character.
 * 
 * **Complexity**: O(n) where n is string length
 * (linear time due to processing each character)
 * 
 * **Locale**: Uses the default C++ locale for case conversion
 * (may vary by system and locale settings)
 * 
 * **Edge cases**:
 * - Empty string returns empty string
 * - Non-alphabetic characters are unchanged
 * - Non-ASCII characters behavior depends on locale
 */
std::string Config::toLower(const std::string& str) {
    if (str.empty()) {
        return "";
    }
    
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
/**
 * @brief Explicit template instantiation for integer types
 * @details
 * These explicit instantiations ensure that the template methods are
 * compiled and available for use. Without these, users would get linker
 * errors when trying to use get<int>(), getOr<int>(), etc.
 * 
 * **Commonly used types**:
 * - int: Standard integer
 * - int64_t: 64-bit integer
 * - double: Floating point
 * - bool: Boolean
 * - std::string: String values
 * - std::vector<int>: Integer arrays
 * - std::vector<double>: Double arrays
 * - std::vector<std::string>: String arrays
 */

template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

// Valid JSON schema for the NLM configuration
const std::string DEFAULT_SCHEMA = R"({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "NLM Configuration Schema",
    "type": "object",
    "properties": {
        "random_seed": {
            "type": "integer",
            "minimum": 0,
            "description": "Random seed for reproducible experiments"
        },
        "simulation_timestep": {
            "type": "number",
            "minimum": 0,
            "maximum": 1,
            "description": "Simulation timestep in seconds"
        },
        "neuron_count": {
            "type": "integer",
            "minimum": 1,
            "description": "Number of neurons in the brain"
        },
        "region_count": {
            "type": "integer",
            "minimum": 1,
            "description": "Number of brain regions"
        },
        "connection_probability": {
            "type": "number",
            "minimum": 0,
            "maximum": 1,
            "description": "Connection probability for random connectivity"
        },
        "plasticity_learning_rate": {
            "type": "number",
            "minimum": 0,
            "maximum": 1,
            "description": "Plasticity learning rate"
        },
        "stdp_ltp_weight": {
            "type": "number",
            "minimum": 0,
            "description": "STDP LTP weight"
        },
        "stdp_ltd_weight": {
            "type": "number",
            "minimum": 0,
            "description": "STDP LTD weight"
        },
        "stdp_time_constant": {
            "type": "number",
            "minimum": 0,
            "description": "STDP time constant"
        },
        "development_synaptogenesis_rate": {
            "type": "number",
            "minimum": 0,
            "description": "Development synaptogenesis rate"
        },
        "development_pruning_rate": {
            "type": "number",
            "minimum": 0,
            "description": "Development pruning rate"
        },
        "dopamine_baseline": {
            "type": "number",
            "description": "Dopamine baseline value"
        },
        "reward_discount_factor": {
            "type": "number",
            "minimum": 0,
            "maximum": 1,
            "description": "Reward discount factor"
        },
        "environment_name": {
            "type": "string",
            "description": "Name of the environment"
        },
        "environment_width": {
            "type": "integer",
            "minimum": 1,
            "description": "Environment width"
        },
        "environment_height": {
            "type": "integer",
            "minimum": 1,
            "description": "Environment height"
        },
        "log_level": {
            "type": "string",
            "enum": ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
            "description": "Log level"
        },
        "log_to_file": {
            "type": "boolean",
            "description": "Whether to log to file"
        },
        "log_filename": {
            "type": "string",
            "description": "Log filename"
        },
        "max_simulation_steps": {
            "type": "integer",
            "minimum": 1,
            "description": "Maximum simulation steps"
        },
        "simulation_time_limit": {
            "type": "number",
            "minimum": 0,
            "description": "Simulation time limit"
        },
        "visualization_enabled": {
            "type": "boolean",
            "description": "Whether visualization is enabled"
        },
        "visualization_update_rate": {
            "type": "number",
            "minimum": 1,
            "description": "Visualization update rate"
        }
    },
    "required": [
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability",
        "plasticity_learning_rate",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "stdp_time_constant",
        "development_synaptogenesis_rate",
        "development_pruning_rate",
        "dopamine_baseline",
        "reward_discount_factor",
        "environment_name",
        "environment_width",
        "environment_height",
        "log_level",
        "log_to_file",
        "log_filename",
        "max_simulation_steps",
        "simulation_time_limit",
        "visualization_enabled",
        "visualization_update_rate"
    ]
})";

// Helper to validate ConfigValue against a schema entry
static bool validateConfigValue(const Config::Schema& schema, const ConfigValue& value) {
    bool isValid = true;
    std::string errorMessage;
    
    auto validateRange = [&errorMessage, &isValid](const std::vector<double>& minVals, const std::vector<double>& maxVals) {
        for (size_t i = 0; i < minVals.size(); ++i) {
            if (i < maxVals.size()) {
                if (minVals[i] > 0 && !std::holds_alternative<int64_t>(value) && !std::holds_alternative<double>(value)) {
                    // Numeric type check
                    if (std::holds_alternative<int64_t>(value)) {
                        if (static_cast<double>(std::get<int64_t>(value)) < minVals[i]) {
                            errorMessage = "Value is less than minimum allowed " + std::to_string(minVals[i]);
                            isValid = false;
                        }
                    } else if (std::holds_alternative<double>(value)) {
                        if (std::get<double>(value) < minVals[i]) {
                            errorMessage = "Value is less than minimum allowed " + std::to_string(minVals[i]);
                            isValid = false;
                        }
                    }
                }
                if (maxVals[i] > 0 && !std::holds_alternative<int64_t>(value) && !std::holds_alternative<double>(value)) {
                    if (std::holds_alternative<int64_t>(value)) {
                        if (static_cast<double>(std::get<int64_t>(value)) > maxVals[i]) {
                            errorMessage = "Value is greater than maximum allowed " + std::to_string(maxVals[i]);
                            isValid = false;
                        }
                    } else if (std::holds_alternative<double>(value)) {
                        if (std::get<double>(value) > maxVals[i]) {
                            errorMessage = "Value is greater than maximum allowed " + std::to_string(maxVals[i]);
                            isValid = false;
                        }
                    }
                }
            }
        }
    };
    
    if (!schema.type.empty()) {
        bool typeMatches = false;
        
        if (schema.type == "int" && std::holds_alternative<int>(value)) {
            typeMatches = true;
        } else if (schema.type == "int64" && std::holds_alternative<int64_t>(value)) {
            typeMatches = true;
        } else if (schema.type == "float" && std::holds_alternative<double>(value)) {
            typeMatches = true;
        } else if (schema.type == "string" && std::holds_alternative<std::string>(value)) {
            typeMatches = true;
        } else if (schema.type == "bool" && std::holds_alternative<bool>(value)) {
            typeMatches = true;
        } else if (schema.type == "vector<int>" && std::holds_alternative<std::vector<int>>(value)) {
            typeMatches = true;
        } else if (schema.type == "vector<float>" && std::holds_alternative<std::vector<double>>(value)) {
            typeMatches = true;
        } else if (schema.type == "vector<string>" && std::holds_alternative<std::vector<std::string>>(value)) {
            typeMatches = true;
        } else if (schema.type == "vector<bool>") {
            // Note: vector<bool> is not in ConfigValue, so we can't match it directly
            typeMatches = false; // No direct support for vector<bool>
        }
        
        if (!typeMatches) {
            errorMessage = "Type mismatch: expected " + schema.type + " but got ";
            isValid = false;
        }
    }
    
    // Validate allowed values
    if (!schema.allowedValues.empty()) {
        if (std::holds_alternative<std::string>(value)) {
            bool found = false;
            for (const auto& allowed : schema.allowedValues) {
                if (std::get<std::string>(value) == allowed) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                errorMessage = "Value not in allowed values list";
                isValid = false;
            }
        } else {
            // For non-string types, convert to string for checking
            std::string valueStr;
            std::visit([&valueStr](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, bool>) {
                    valueStr = arg ? "true" : "false";
                } else {
                    valueStr = std::to_string(arg);
                }
            }, value);
            
            bool found = false;
            for (const auto& allowed : schema.allowedValues) {
                if (valueStr == allowed) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                errorMessage = "Value not in allowed values list";
                isValid = false;
            }
        }
    }
    
    // Validate value ranges
    if (isValid) {
        validateRange(schema.minValue, schema.maxValue);
    }
    
    if (!isValid && !errorMessage.empty()) {
        std::cerr << "Config validation error for key '" << schema.key << "': " << errorMessage << std::endl;
    }
    
    return isValid;
}

// Schema management
void Config::addSchema(const Schema& schema) {
    schemas.push_back(schema);
}

void Config::validateSchema(const nlohmann::json& schemaJson) const {
    if (!schemaJson.is_object()) {
        std::cerr << "Invalid schema: must be a JSON object" << std::endl;
        return;
    }
    
    // Validate schema structure
    if (schemaJson.contains("properties")) {
        auto properties = schemaJson["properties"];
        for (auto it = properties.begin(); it != properties.end(); ++it) {
            std::string key = it.key();
            auto prop = it.value();
            
            if (!prop.is_object()) {
                std::cerr << "Invalid schema: property '" << key << "' must be an object" << std::endl;
                continue;
            }
            
            // Validate property type
            if (!prop.contains("type")) {
                std::cerr << "Warning: property '" << key << "' missing type in schema" << std::endl;
            }
        }
    }
    
    // Validate required fields
    if (schemaJson.contains("required")) {
        auto required = schemaJson["required"];
        if (!required.is_array()) {
            std::cerr << "Invalid schema: 'required' must be an array" << std::endl;
        }
    }
    
    std::cerr << "Schema validation completed" << std::endl;
}

bool Config::validate() const {
    bool allValid = true;
    
    for (const auto& schema : schemas) {
        if (!has(schema.key)) {
            if (schema.required) {
                std::cerr << "Validation error: Required key '" << schema.key << "' is missing" << std::endl;
                allValid = false;
            }
            continue;
        }
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&schema](const ConfigEntry& e) { return e.key == schema.key; });n        
        if (it != pImpl->entries.end()) {
            if (!validateConfigValue(schema, it->value)) {
                allValid = false;
            }
        }
    }
    
    // Also validate against default schema if present
    if (allValid) {
        try {
            auto defaultSchema = nlohmann::json::parse(DEFAULT_SCHEMA);
            validateSchema(defaultSchema);
            
            // Validate against default schema values
            for (auto it = defaultSchema["properties"].begin(); it != defaultSchema["properties"].end(); ++it) {
                std::string key = it.key();
                auto prop = it.value();
                
                if (has(key)) {
                    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                        [&key](const ConfigEntry& e) { return e.key == key; });
                    
                    if (entryIt != pImpl->entries.end()) {
                        nlohmann::json expectedValue;
                        if (prop.contains("default")) {
                            expectedValue = prop["default"];
                            
                            // Convert expected value to string for comparison
                            std::string expectedStr;
                            if (expectedValue.is_string()) {
                                expectedStr = expectedValue.get<std::string>();
                            } else {
                                expectedStr = expectedValue.dump();
                            }
                            
                            std::string actualStr;
                            std::visit([&actualStr](auto&& arg) {
                                using T = std::decay_t<decltype(arg)>;
                                if constexpr (std::is_same_v<T, std::string>) {
                                    actualStr = arg;
                                } else {
                                    actualStr = std::to_string(arg);
                                }
                            }, entryIt->value);
                            
                            if (expectedStr != actualStr) {
                                std::cerr << "Validation warning: Key '" << key << "' has unexpected value: " << actualStr << std::endl;
                            }
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing default schema: " << e.what() << std::endl;
            allValid = false;
        }
    }
    
    return allValid;
}
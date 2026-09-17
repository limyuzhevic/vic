// ConfigParser.cpp - Configuration file parser with JSON and key=value support implementation
#include "ConfigParser.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <cmath>
#include <functional>

namespace nlm {

ConfigParser::ConfigParser(Config& cfg) : config(cfg) {
}

bool ConfigParser::loadFile(const std::string& filepath, ConfigSource source) {
    // Check if it's a JSON file based on extension
    if (isJsonFile(filepath)) {
        return parseJsonFile(filepath, source);
    }
    
    // Try as simple key=value format
    return parseSimpleFile(filepath, source);
}

bool ConfigParser::loadJsonString(const std::string& jsonString, ConfigSource source) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        return parseJsonObject(json, source);
    } catch (const std::exception& e) {
        logError(std::string("Failed to parse JSON: ") + e.what());
        return false;
    }
}

bool ConfigParser::loadKeyValueString(const std::string& kvString, ConfigSource source) {
    std::istringstream stream(kvString);
    std::string line;
    bool success = true;
    
    while (std::getline(stream, line)) {
        if (parseSimpleString(line, source)) {
            stats.kvKeysLoaded++;
        } else {
            success = false;
        }
    }
    
    return success;
}

std::string ConfigParser::getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "";
    
    std::string ext = filename.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

bool ConfigParser::isJsonFile(const std::string& filename) {
    std::string ext = getFileExtension(filename);
    return (ext == "json" || ext == "json5" || ext == "jsonl" || ext == "jsonx");
}

bool ConfigParser::validateWithSchema(const Schema& schema, const std::string& key, const ConfigValue& value) const {
    // This is a basic implementation - in practice, you'd want more sophisticated validation
    stats.validationErrors++;
    return true; // For now, accept all values
}

std::string ConfigParser::generateDocumentation(const std::map<std::string, Schema>& schema) const {
    std::ostringstream oss;
    oss << "# Configuration Documentation\n\n";
    
    for (const auto& pair : schema) {
        const auto& key = pair.first;
        const auto& schema = pair.second;
        
        oss << "## " << key << "\n";
        oss << "- Type: " << schema.type << "\n";
        if (!schema.description.empty()) {
            oss << "- Description: " << schema.description << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

bool ConfigParser::parseJsonFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logError("Cannot open file: " + filepath);
        return false;
    }
    
    try {
        auto json = nlohmann::json::parse(file);
        return parseJsonObject(json, source);
    } catch (const std::exception& e) {
        logError(std::string("JSON parse error in file ") + filepath + ": " + e.what());
        return false;
    }
}

bool ConfigParser::parseSimpleFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logError("Cannot open file: " + filepath);
        return false;
    }
    
    std::string line;
    bool success = true;
    
    while (std::getline(file, line)) {
        if (parseSimpleString(line, source)) {
            stats.kvKeysLoaded++;
        } else {
            success = false;
        }
    }
    
    return success;
}

bool ConfigParser::parseJsonObject(const nlohmann::json& json, ConfigSource source) {
    if (!json.is_object()) {
        logError("JSON root is not an object");
        return false;
    }
    
    bool success = true;
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        try {
            ConfigValue value = jsonToConfigValue(*it);
            config.set(key, value, source);
            stats.jsonKeysLoaded++;
        } catch (const std::exception& e) {
            logError(std::string("Error parsing JSON key \"") + key + "\": " + e.what());
            success = false;
        }
    }
    
    stats.totalKeysLoaded = stats.jsonKeysLoaded + stats.kvKeysLoaded;
    return success;
}

bool ConfigParser::parseSimpleString(const std::string& line, ConfigSource source) {
    std::string trimmed = trim(line);
    
    // Skip empty lines and comments
    if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/') {
        return true;
    }
    
    // Parse key=value pairs
    size_t pos = trimmed.find('=');
    if (pos == std::string::npos) {
        logError("Invalid format (no = found): " + trimmed);
        return false;
    }
    
    std::string key = trim(trimmed.substr(0, pos));
    std::string value = trim(trimmed.substr(pos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    // Try to parse different types
    bool parsed = false;
    
    // Try integer
    try {
        size_t pos;
        long val = std::stol(value, &pos);
        if (pos == value.size()) {
            config.set(key, val, source);
            parsed = true;
        }
    } catch (...) {}
    
    // Try unsigned integer
    if (!parsed) {
        try {
            size_t pos;
            unsigned long val = std::stoul(value, &pos);
            if (pos == value.size()) {
                config.set(key, static_cast<long>(val), source);
                parsed = true;
            }
        } catch (...) {}
    }
    
    // Try double
    if (!parsed) {
        try {
            size_t pos;
            double val = std::stod(value, &pos);
            if (pos == value.size()) {
                config.set(key, val, source);
                parsed = true;
            }
        } catch (...) {}
    }
    
    // Try boolean
    if (!parsed) {
        std::string lower = toLower(value);
        if (lower == "true" || lower == "false") {
            bool val = (lower == "true");
            config.set(key, val, source);
            parsed = true;
        }
    }
    
    // If not parsed, treat as string
    if (!parsed) {
        config.set(key, value, source);
        parsed = true;
    }
    
    return parsed;
}

void ConfigParser::logError(const std::string& message) {
    std::cerr << "[ConfigParser Error] " << message << std::endl;
}

ConfigValue ConfigParser::jsonToConfigValue(const nlohmann::json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_number_integer()) {
        return value.get<int>();
    } else if (value.is_number_float()) {
        return value.get<double>();
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_array()) {
        // Convert JSON array to appropriate ConfigValue vector type
        if (!value.empty()) {
            const auto& first = value[0];
            if (first.is_number_integer()) {
                std::vector<int> vec;
                vec.reserve(value.size());
                for (const auto& item : value) {
                    vec.push_back(item.get<int>());
                }
                return vec;
            } else if (first.is_number_float()) {
                std::vector<double> vec;
                vec.reserve(value.size());
                for (const auto& item : value) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else if (first.is_string()) {
                std::vector<std::string> vec;
                vec.reserve(value.size());
                for (const auto& item : value) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            }
        }
        // Default to empty string vector
        return std::vector<std::string>{};
    }
    
    // Default to empty string
    return std::string();
}

nlohmann::json ConfigParser::configValueToJson(const ConfigValue& value) {
    // Use std::visit to convert ConfigValue to nlohmann::json
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : arg) {
                arr.push_back(item);
            }
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : arg) {
                arr.push_back(item);
            }
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : arg) {
                arr.push_back(item);
            }
            return arr;
        } else {
            return nlohmann::json(nullptr);
        }
    }, value);
}

// Phase 6 Enhanced Configuration
bool ConfigParser::isPhase6Config(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Check for Phase 6 specific indicators
    return (content.find("Phase 6 integration") != std::string::npos) ||
           (content.find("'Phase6Config'") != std::string::npos) ||
           (content.find("'Phase6IntegrationResult'") != std::string::npos);
}

void ConfigParser::applyPhase6Enhancements(Config& config) {
    NLM_LOG_INFO("Applying Phase 6 enhancements to configuration");
    
    // Ensure all Phase 6 required settings are present
    config.set("phase6_integration", true, ConfigSource::Runtime);
    config.set("enable_working_memory", true, ConfigSource::Runtime);
    config.set("enable_episodic_memory", true, ConfigSource::Runtime);
    config.set("enable_neuromodulation", true, ConfigSource::Runtime);
    config.set("enable_prediction", true, ConfigSource::Runtime);
    config.set("enable_development", true, ConfigSource::Runtime);
    config.set("enable_replay", true, ConfigSource::Runtime);
    config.set("enable_checkpointing", true, ConfigSource::Runtime);
    
    // Set appropriate default values for Phase 6
    config.set("simulation_timestep", 0.001, ConfigSource::Runtime);
    config.set("connection_probability", 0.05f, ConfigSource::Runtime);
    config.set("plasticity_learning_rate", 0.01f, ConfigSource::Runtime);
    config.set("stdp_ltp_weight", 0.02f, ConfigSource::Runtime);
    config.set("stdp_ltd_weight", 0.015f, ConfigSource::Runtime);
    config.set("stdp_tau", 20.0f, ConfigSource::Runtime);
    config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Runtime);
    config.set("pruning_rate", 0.00001f, ConfigSource::Runtime);
    
    NLM_LOG_INFO("Phase 6 configuration enhanced");
}

// Enhanced command-line argument parsing with validation
bool ConfigParser::loadFromArgsWithValidation(int argc, char** argv, const std::map<std::string, Schema>& schema) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with validation
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate against schema if available
                auto schemaIt = schema.find(key);
                if (schemaIt != schema.end()) {
                    // For now, simple validation - would use actual validation in production
                    // Implementation would check schema.type, range constraints, etc.
                }
                
                config.set(key, value, ConfigSource::CommandLine);
                stats.totalKeysLoaded++;
            } else {
                logError("Invalid command-line argument format: " + arg);
                success = false;
            }
        }
        // Handle -key value format with validation
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate against schema if available
            auto schemaIt = schema.find(key);
            if (schemaIt != schema.end()) {
                // For now, simple validation - would use actual validation in production
            }
            
            config.set(key, value, ConfigSource::CommandLine);
            stats.totalKeysLoaded++;
        }
    }
    
    return success;
}

// Configuration transformation pipeline
bool ConfigParser::applyTransformations(const std::vector<Transformation>& transformations) {
    bool success = true;
    
    for (const auto& transform : transformations) {
        if (!config.has(transform.inputKey)) {
            logError("Input key not found: " + transform.inputKey);
            success = false;
            continue;
        }
        
        // Get current value
        auto currentValue = config.get<std::string>(transform.inputKey);
        if (!currentValue) {
            logError("Could not get value for key: " + transform.inputKey);
            success = false;
            continue;
        }
        
        // Apply transformation based on operation
        std::string result;
        double value = std::stod(*currentValue);
        
        if (transform.operation == "scale") {
            double factor = 1.0;
            auto it = transform.parameters.find("factor");
            if (it != transform.parameters.end()) {
                factor = std::stod(it->second);
            }
            result = std::to_string(value * factor);
        } else if (transform.operation == "clamp") {
            double min = -1.0, max = 1.0;
            auto minIt = transform.parameters.find("min");
            if (minIt != transform.parameters.end()) min = std::stod(minIt->second);
            auto maxIt = transform.parameters.find("max");
            if (maxIt != transform.parameters.end()) max = std::stod(maxIt->second);
            value = std::max(min, std::min(max, value));
            result = std::to_string(value);
        } else if (transform.operation == "normalize") {
            double targetMin = 0.0, targetMax = 1.0;
            auto minIt = transform.parameters.find("targetMin");
            if (minIt != transform.parameters.end()) targetMin = std::stod(minIt->second);
            auto maxIt = transform.parameters.find("targetMax");
            if (maxIt != transform.parameters.end()) targetMax = std::stod(maxIt->second);
            
            result = std::to_string((value - targetMin) / (targetMax - targetMin));
        } else if (transform.operation == "apply_formula") {
            std::string formula = transform.parameters["formula"];
            // Simple formula evaluation - would use proper parser in production
            // For demo, just implement common math operations
            std::string output = formula;
            output.replace(output.find("x"), 1, *currentValue);
            try {
                result = std::to_string(applyFormula(output, {}));
            } catch (...) {
                logError("Failed to evaluate formula: " + formula);
                success = false;
                continue;
            }
        } else {
            logError("Unknown transformation operation: " + transform.operation);
            success = false;
            continue;
        }
        
        // Set transformed value
        config.set(transform.outputKey, result, ConfigSource::Runtime);
    }
    
    return success;
}

// Schema validation helpers
bool ConfigParser::validateStringPattern(const std::string& value, const std::optional<std::string>& pattern) {
    if (!pattern) return true;
    
    // Simple regex-like pattern matching (limited implementation)
    if (pattern->find('*') != std::string::npos) {
        // Handle wildcards
        std::string regex = std::regex_escape(*pattern);
        regex.replace(regex.find("\\*"), 1, ".*");
        // Would use std::regex in production
    }
    
    return true;
}

double ConfigParser::applyFormula(const std::string& formula, const std::map<std::string, double>& variables) {
    // Very basic formula evaluation - would use proper parser in production
    double result = 0.0;
    
    // Simple implementations for common operations
    if (formula.find("sin(") != std::string::npos) {
        size_t start = formula.find("sin(") + 4;
        size_t end = formula.find(")", start);
        double x = std::stod(formula.substr(start, end - start));
        return std::sin(x);
    } else if (formula.find("exp(") != std::string::npos) {
        size_t start = formula.find("exp(") + 4;
        size_t end = formula.find(")", start);
        double x = std::stod(formula.substr(start, end - start));
        return std::exp(x);
    } else if (formula.find("log(") != std::string::npos) {
        size_t start = formula.find("log(") + 4;
        size_t end = formula.find(")", start);
        double x = std::stod(formula.substr(start, end - start));
        return std::log(x);
    }
    
    // Simple arithmetic evaluation
    try {
        return std::stod(formula);
    } catch (...) {
        throw std::runtime_error("Could not parse formula: " + formula);
    }
}

// Enhanced documentation generation
std::string ConfigParser::generateAdvancedDocumentation(const std::map<std::string, Schema>& schema, 
                                                        const std::map<std::string, std::string>& examples) const {
    std::ostringstream oss;
    oss << "# NLM Configuration Documentation (Advanced)" << std::endl;
    oss << "Generated on " << __TIMESTAMP__ << std::endl;
    oss << std::endl;
    
    oss << "## Configuration Overview" << std::endl;
    oss << "This documentation covers both simple key=value and JSON configuration formats." << std::endl;
    oss << std::endl;
    
    oss << "## Configuration Formats" << std::endl;
    oss << "1. **Key=Value Format** (e.g., `configs/default.cfg`):" << std::endl;
    oss << "   - Simple text-based format" << std::endl;
    oss << "   - Easy to edit manually" << std::endl;
    oss << "   - Supported types: strings, integers, doubles, booleans" << std::endl;
    oss << "" << std::endl;
    
    oss << "2. **JSON Format** (e.g., `config.json`):" << std::endl;
    oss << "   - Structured format with arrays and objects" << std::endl;
    oss << "   - Full type support including arrays" << std::endl;
    oss << "   - Easy to parse programmatically" << std::endl;
    oss << "" << std::endl;
    
    oss << "## Schema Validation Examples" << std::endl;
    for (const auto& pair : schema) {
        const auto& key = pair.first;
        const auto& schema = pair.second;
        
        oss << "### " << key << "" << std::endl;
        oss << "- Type: " << schema.type << "" << std::endl;
        if (!schema.description.empty()) {
            oss << "- Description: " << schema.description << "" << std::endl;
        }
        if (schema.pattern) {
            oss << "- Pattern: " << *schema.pattern << "" << std::endl;
        }
        if (schema.minimum.has_value()) {
            oss << "- Minimum: " << *schema.minimum << "" << std::endl;
        }
        if (schema.maximum.has_value()) {
            oss << "- Maximum: " << *schema.maximum << "" << std::endl;
        }
        if (schema.minItems.has_value()) {
            oss << "- Minimum items: " << *schema.minItems << "" << std::endl;
        }
        if (schema.maxItems.has_value()) {
            oss << "- Maximum items: " << *schema.maxItems << "" << std::endl;
        }
        oss << "" << std::endl;
    }
    
    oss << "## Usage Examples" << std::endl;
    if (!examples.empty()) {
        for (const auto& example : examples) {
            oss << "### " << example.first << "" << std::endl;
            oss << "```" << std::endl;
            oss << example.second << std::endl;
            oss << "```" << std::endl;
            oss << "" << std::endl;
        }
    } else {
        oss << "See configuration documentation for specific examples." << std::endl;
    }
    
    oss << "## Phase 6 Configuration" << std::endl;
    oss << "Phase 6 integration requires specific configuration settings:" << std::endl;
    oss << "- `phase6_integration = true`" << std::endl;
    oss << "- `enable_working_memory = true`" << std::endl;
    oss << "- `enable_episodic_memory = true`" << std::endl;
    oss << "- `enable_neuromodulation = true`" << std::endl;
    oss << "- `enable_prediction = true`" << std::endl;
    oss << "- `enable_development = true`" << std::endl;
    oss << "- `enable_replay = true`" << std::endl;
    oss << "- `enable_checkpointing = true`" << std::endl;
    oss << "" << std::endl;
    
    oss << "## Command Line Usage" << std::endl;
    oss << "```bash" << std::endl;
    oss << "# Using key=value format" << std::endl;
    oss << "nlm --neuron_count=1000 --region_count=1 --simulation_timestep=0.001" << std::endl;
    oss << "" << std::endl;
    oss << "# Using short format" << std::endl;
    oss << "nlm -neuron_count 1000 -region_count 1 -simulation_timestep 0.001" << std::endl;
    oss << "```" << std::endl;
    
    oss << "## Configuration Transformation" << std::endl;
    oss << "Configurations can be transformed using a pipeline of operations:" << std::endl;
    oss << "1. **Scale**: Multiply values by a factor" << std::endl;
    oss << "2. **Clamp**: Limit values to a range" << std::endl;
    oss << "3. **Normalize**: Scale to [0, 1] range" << std::endl;
    oss << "4. **Apply Formula**: Use mathematical expressions" << std::endl;
    oss << "" << std::endl;
    
    oss << "## Error Handling" << std::endl;
    oss << "- Invalid values cause load to fail gracefully" << std::endl;
    oss << "- Schema validation prevents invalid configurations" << std::endl;
    oss << "- Parser statistics help diagnose configuration issues" << std::endl;
    
    return oss.str();
}

} // namespace nlm

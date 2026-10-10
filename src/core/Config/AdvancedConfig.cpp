#include "AdvancedConfig.hpp"
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {
namespace config {

const std::string EnvironmentConfig::NLM_CONFIG_PREFIX = "NLM_";

void ConfigSchema::addField(const std::string& name, const std::string& type, 
                           bool required, const std::string& description,
                           const std::vector<std::variant<int, double, bool, std::string>>& enumVals) {
    FieldInfo field(name, type, required, description);
    field.enumValues = enumVals;
    fields[name] = field;
}

void ConfigSchema::addStringField(const std::string& name, bool required,
                                  const std::string& description,
                                  const std::string& pattern) {
    FieldInfo field(name, "string", required, description);
    field.pattern = std::regex(pattern);
    fields[name] = field;
}

void ConfigSchema::addIntField(const std::string& name, bool required,
                               const std::string& description,
                               int defaultValue,
                               const std::vector<int>& enumVals) {
    FieldInfo field(name, "int", required, description);
    field.defaultValue = defaultValue;
    field.enumValues = enumVals;
    fields[name] = field;
}

void ConfigSchema::addFloatField(const std::string& name, bool required,
                                 const std::string& description,
                                 double defaultValue,
                                 const std::vector<double>& enumVals) {
    FieldInfo field(name, "float", required, description);
    field.defaultValue = defaultValue;
    field.enumValues = enumVals;
    fields[name] = field;
}

void ConfigSchema::addBoolField(const std::string& name, bool required,
                                const std::string& description,
                                bool defaultValue) {
    FieldInfo field(name, "bool", required, description);
    field.defaultValue = defaultValue;
    fields[name] = field;
}

bool ConfigSchema::validate(const std::map<std::string, std::variant<int, double, bool, std::string>>& config) const {
    for (const auto& pair : fields) {
        const std::string& fieldName = pair.first;
        const FieldInfo& fieldInfo = pair.second;
        
        // Check if field is present
        if (config.find(fieldName) == config.end()) {
            if (fieldInfo.required) {
                return false;
            }
            continue;
        }
        
        // Validate field type
        const auto& value = config.at(fieldName);
        std::string error = getFieldTypeError(fieldName, value);
        if (!error.empty()) {
            return false;
        }
        
        // Validate enum values
        if (!fieldInfo.enumValues.empty()) {
            bool validEnum = false;
            for (const auto& enumVal : fieldInfo.enumValues) {
                if (value == enumVal) {
                    validEnum = true;
                    break;
                }
            }
            if (!validEnum) {
                return false;
            }
        }
        
        // Validate pattern (for strings)
        if (fieldInfo.type == "string" && !fieldInfo.pattern.str().empty()) {
            std::string strValue;
            if (std::holds_alternative<std::string>(value)) {
                strValue = std::get<std::string>(value);
            } else {
                return false; // Wrong type
            }
            
            if (!std::regex_match(strValue, fieldInfo.pattern)) {
                return false;
            }
        }
    }
    
    return true;
}

std::string ConfigSchema::getFieldTypeError(const std::string& field, 
                                           const std::variant<int, double, bool, std::string>& value) const {
    if (fields.find(field) == fields.end()) {
        return "Field not defined in schema: " + field;
    }
    
    const FieldInfo& fieldInfo = fields.at(field);
    
    // Check type compatibility
    bool typeMatches = false;
    if (fieldInfo.type == "int") {
        typeMatches = std::holds_alternative<int>(value);
    } else if (fieldInfo.type == "float") {
        typeMatches = (std::holds_alternative<int>(value) || std::holds_alternative<double>(value));
    } else if (fieldInfo.type == "bool") {
        typeMatches = std::holds_alternative<bool>(value);
    } else if (fieldInfo.type == "string") {
        typeMatches = std::holds_alternative<std::string>(value);
    } else if (fieldInfo.type == "vec_int") {
        typeMatches = std::holds_alternative<std::vector<int>>(value);
    } else if (fieldInfo.type == "vec_float") {
        typeMatches = std::holds_alternative<std::vector<double>>(value);
    } else if (fieldInfo.type == "vec_string") {
        typeMatches = std::holds_alternative<std::vector<std::string>>(value);
    }
    
    if (!typeMatches) {
        return "Field '" + field + "' has wrong type. Expected " + fieldInfo.type + ".";
    }
    
    return "";
}

std::vector<std::string> ConfigSchema::getFieldNames() const {
    std::vector<std::string> names;
    names.reserve(fields.size());
    for (const auto& pair : fields) {
        names.push_back(pair.first);
    }
    return names;
}

ConfigSchema::FieldInfo ConfigSchema::getFieldInfo(const std::string& fieldName) const {
    return fields.at(fieldName);
}

std::map<std::string, std::variant<int, double, bool, std::string>> 
EnvironmentConfig::loadFromEnvironment(const std::map<std::string, FieldInfo>& schema) {
    std::map<std::string, std::variant<int, double, bool, std::string>> result;
    
    for (const auto& pair : schema) {
        const std::string& fieldName = pair.first;
        const FieldInfo& fieldInfo = pair.second;
        
        std::string envVar = NLM_CONFIG_PREFIX + fieldName;
        std::string envValue;
        
        // Try to get environment variable
        const char* envStr = std::getenv(envVar.c_str());
        if (envStr != nullptr) {
            envValue = envStr;
        } else {
            continue; // Skip if not set
        }
        
        try {
            std::variant<int, double, bool, std::string> parsedValue = 
                parseEnvValue(envValue, fieldInfo);
            result[fieldName] = parsedValue;
        } catch (const std::exception& e) {
            // Silently ignore malformed environment variables
            // This allows partial environment configuration
            continue;
        }
    }
    
    return result;
}

std::string EnvironmentConfig::boolToString(bool value) {
    return value ? "true" : "false";
}

std::string EnvironmentConfig::intToString(int value) {
    return std::to_string(value);
}

std::string EnvironmentConfig::floatToString(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << value;
    return ss.str();
}

std::variant<int, double, bool, std::string> EnvironmentConfig::parseEnvValue(
    const std::string& str, const FieldInfo& fieldInfo) {
    if (fieldInfo.type == "bool") {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
            return true;
        } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
            return false;
        } else {
            throw std::runtime_error("Invalid boolean value: " + str);
        }
    } else if (fieldInfo.type == "int") {
        try {
            size_t pos;
            int value = std::stoi(str, &pos);
            if (pos != str.size()) {
                throw std::runtime_error("Extra characters after integer: " + str.substr(pos));
            }
            return value;
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid integer value: " + str);
        }
    } else if (fieldInfo.type == "float") {
        try {
            size_t pos;
            double value = std::stod(str, &pos);
            if (pos != str.size()) {
                throw std::runtime_error("Extra characters after float: " + str.substr(pos));
            }
            return value;
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid float value: " + str);
        }
    } else if (fieldInfo.type == "string") {
        return str;
    } else {
        throw std::runtime_error("Unsupported field type: " + fieldInfo.type);
    }
}

void DynamicConfig::addValidator(const std::string& field,
                                 std::function<bool(const std::variant<int, double, bool, std::string>&)> validator) {
    validators[field] = validator;
}

bool DynamicConfig::setField(const std::string& field, 
                            const std::variant<int, double, bool, std::string>& value) {
    // Check if field exists in base config
    if (!baseConfig->has(field)) {
        return false;
    }
    
    // Apply validators
    if (validators.find(field) != validators.end()) {
        if (!validators[field](value)) {
            return false;
        }
    }
    
    overrides[field] = value;
    return true;
}

std::variant<int, double, bool, std::string> DynamicConfig::getField(const std::string& field) const {
    if (overrides.find(field) != overrides.end()) {
        return overrides.at(field);
    }
    
    // Try to get from base config
    if (baseConfig->has(field)) {
        auto value = baseConfig->get<std::string>(field);
        if (value) {
            // For simplicity, return as string - in practice, you'd want type-specific access
            return *value;
        }
    }
    
    throw std::runtime_error("Field not found: " + field);
}

bool DynamicConfig::isValid() const {
    return !getValidationError().empty();
}

std::string DynamicConfig::getValidationError() const {
    for (const auto& pair : overrides) {
        const std::string& field = pair.first;
        const auto& value = pair.second;
        
        // Check if field has a validator
        if (validators.find(field) != validators.end()) {
            if (!validators[field](value)) {
                return "Invalid value for field '" + field + "': " + std::to_string(value);
            }
        }
    }
    
    return "";
}

std::shared_ptr<Config> DynamicConfig::applyToConfig() const {
    auto result = std::make_shared<Config>(*baseConfig);
    
    for (const auto& pair : overrides) {
        const std::string& field = pair.first;
        const auto& value = pair.second;
        
        if (std::holds_alternative<int>(value)) {
            result->set(field, std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            result->set(field, std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            result->set(field, std::get<bool>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            result->set(field, std::get<std::string>(value));
        }
    }
    
    return result;
}

void DynamicConfig::clear() {
    overrides.clear();
}

std::string ConfigExporter::toJSON(const std::map<std::string, std::variant<int, double, bool, std::string>>& config) {
    std::stringstream ss;
    ss << "{\n";
    
    bool first = true;
    for (const auto& pair : config) {
        if (!first) {
            ss << ",\n";
        }
        first = false;
        
        ss << "  \"" << pair.first << "\": ";
        
        const auto& value = pair.second;
        if (std::holds_alternative<int>(value)) {
            ss << std::get<int>(value);
        } else if (std::holds_alternative<double>(value)) {
            ss << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            ss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            ss << "\"" << std::get<std::string>(value) << "\"";
        }
    }
    
    ss << "\n}";
    return ss.str();
}

std::string ConfigExporter::toTOML(const std::map<std::string, std::variant<int, double, bool, std::string>>& config) {
    std::stringstream ss;
    ss << "# NLM Configuration File (TOML format)\n\n";
    
    for (const auto& pair : config) {
        ss << pair.first << " = ";
        
        const auto& value = pair.second;
        if (std::holds_alternative<int>(value)) {
            ss << std::get<int>(value);
        } else if (std::holds_alternative<double>(value)) {
            ss << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            ss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            ss << "\"" << std::get<std::string>(value) << "\"";
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string ConfigExporter::toYAML(const std::map<std::string, std::variant<int, double, bool, std::string>>& config) {
    std::stringstream ss;
    ss << "# NLM Configuration File (YAML format)\n";
    ss << "nlm:\n";
    
    for (const auto& pair : config) {
        ss << "  " << pair.first << ": ";
        
        const auto& value = pair.second;
        if (std::holds_alternative<int>(value)) {
            ss << std::get<int>(value);
        } else if (std::holds_alternative<double>(value)) {
            ss << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            ss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            ss << "\"" << std::get<std::string>(value) << "\"";
        }
        ss << "\n";
    }
    
    return ss.str();
}

bool ConfigExporter::fromJSON(std::map<std::string, std::variant<int, double, bool, std::string>>& config,
                               const std::string& json) {
    // Simplified JSON parsing - in production, use a proper JSON library
    try {
        // Remove braces and split by commas
        std::string content = json;
        if (content.front() == '{' && content.back() == '}') {
            content = content.substr(1, content.size() - 2);
        }
        
        // Split by ", " (naive implementation)
        std::vector<std::string> pairs;
        size_t pos = 0;
        while ((pos = content.find('"', pos)) != std::string::npos) {
            size_t keyEnd = content.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 3; // Skip ": "
            
            size_t valueEnd = content.find('"', pos);
            if (valueEnd == std::string::npos) {
                // Try without quotes (number/bool)
                valueEnd = content.find(',', pos);
                if (valueEnd == std::string::npos) {
                    valueEnd = content.size();
                }
            }
            
            std::string valueStr = content.substr(pos, valueEnd - pos);
            
            // Parse value
            std::variant<int, double, bool, std::string> value;
            if (valueStr == "true" || valueStr == "false") {
                value = (valueStr == "true");
            } else {
                try {
                    size_t decimalPos = valueStr.find('.');
                    if (decimalPos != std::string::npos) {
                        value = std::stod(valueStr);
                    } else {
                        value = std::stoi(valueStr);
                    }
                } catch (...) {
                    value = valueStr;
                }
            }
            
            config[key] = value;
            pos = valueEnd + 1;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigExporter::fromTOML(std::map<std::string, std::variant<int, double, bool, std::string>>& config,
                               const std::string& toml) {
    // Simplified TOML parsing - in production, use a proper TOML library
    try {
        std::stringstream ss(toml);
        std::string line;
        
        while (std::getline(ss, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Split by '=' or ':'
            size_t delimiter = std::min(line.find('='), line.find(':'));
            if (delimiter == std::string::npos) {
                continue;
            }
            
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            // Remove quotes if present
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value
            std::variant<int, double, bool, std::string> parsedValue;
            if (value == "true" || value == "false") {
                parsedValue = (value == "true");
            } else {
                try {
                    size_t decimalPos = value.find('.');
                    if (decimalPos != std::string::npos) {
                        parsedValue = std::stod(value);
                    } else {
                        parsedValue = std::stoi(value);
                    }
                } catch (...) {
                    parsedValue = value;
                }
            }
            
            config[key] = parsedValue;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigExporter::fromYAML(std::map<std::string, std::variant<int, double, bool, std::string>>& config,
                               const std::string& yaml) {
    // Simplified YAML parsing - in production, use a proper YAML library
    return fromTOML(config, yaml); // TOML and YAML are similar for simple key-value pairs
}

void ConfigMigrator::addMigration(int fromVersion, int toVersion,
                                   const std::function<std::map<std::string, std::variant<int, double, bool, std::string>>(
                                       const std::map<std::string, std::variant<int, double, bool, std::string>>&)>& transform,
                                   const std::string& description) {
    Migration migration;
    migration.version = toVersion;
    migration.transform = transform;
    migration.description = description;
    migrations.push_back(migration);
}

std::map<std::string, std::variant<int, double, bool, std::string>> ConfigMigrator::migrate(
    const std::map<std::string, std::variant<int, double, bool, std::string>>& config,
    int targetVersion) const {
    std::map<std::string, std::variant<int, double, bool, std::string>> result = config;
    
    // Apply migrations in order
    for (const auto& migration : migrations) {
        if (migration.version <= targetVersion) {
            result = migration.transform(result);
        }
    }
    
    return result;
}

int ConfigMigrator::getCurrentVersion() const {
    if (migrations.empty()) {
        return 1; // Default version
    }
    
    return migrations.back().version;
}

ConfigSchema PredefinedSchemas::getResearchSchema() {
    ConfigSchema schema;
    
    schema.addStringField("experiment_name", true, "Name of the research experiment");
    schema.addIntField("neuron_count", true, "Number of neurons for research simulation", 2000);
    schema.addFloatField("simulation_timestep", false, "Simulation timestep", 0.001);
    schema.addBoolField("enable_stdp", false, "Enable spike-timing-dependent plasticity", true);
    schema.addBoolField("enable_curiosity", false, "Enable curiosity-driven exploration", true);
    schema.addFloatField("curiosity_scale", false, "Curiosity motivation scale", 1.0);
    schema.addStringField("dataset_path", false, "Path to training dataset");
    schema.addIntField("training_epochs", false, "Number of training epochs", 100);
    schema.addFloatField("learning_rate", false, "Learning rate for plasticity", 0.001);
    
    return schema;
}

ConfigSchema PredefinedSchemas::getProductionSchema() {
    ConfigSchema schema;
    
    schema.addStringField("system_name", true, "Production system name");
    schema.addIntField("neuron_count", true, "Number of neurons", 5000);
    schema.addFloatField("simulation_timestep", false, "Simulation timestep", 0.0005);
    schema.addBoolField("enable_optimization", false, "Enable performance optimizations", true);
    schema.addIntField("cache_size", false, "Cache size in MB", 512);
    schema.addBoolField("enable_checkpointing", false, "Enable automatic checkpointing", true);
    schema.addStringField("checkpoint_path", false, "Checkpoint directory");
    schema.addIntField("max_memory_mb", false, "Maximum memory usage in MB", 2048);
    schema.addFloatField("energy_efficiency", false, "Target energy efficiency", 0.8);
    
    return schema;
}

ConfigSchema PredefinedSchemas::getDevelopmentSchema() {
    ConfigSchema schema;
    
    schema.addStringField("development_mode", true, "Development mode");
    schema.addBoolField("enable_debug_mode", false, "Enable debug logging", true);
    schema.addBoolField("enable_validation", false, "Enable configuration validation", true);
    schema.addIntField("neuron_count", true, "Number of neurons", 1000);
    schema.addStringField("debug_output_path", false, "Path for debug output");
    schema.addIntField("validation_iterations", false, "Number of validation iterations", 10);
    schema.addBoolField("enable_profiling", false, "Enable performance profiling", false);
    
    return schema;
}

ConfigSchema PredefinedSchemas::getPerformanceSchema() {
    ConfigSchema schema;
    
    schema.addIntField("neuron_count", true, "Number of neurons", 10000);
    schema.addFloatField("simulation_timestep", false, "Simulation timestep", 0.0002);
    schema.addBoolField("enable_simd", false, "Enable SIMD optimization", true);
    schema.addBoolField("enable_parallel", false, "Enable parallel processing", true);
    schema.addIntField("thread_count", false, "Number of threads", std::thread::hardware_concurrency());
    schema.addBoolField("enable_memory_pool", false, "Enable memory pooling", true);
    schema.addFloatField("target_fps", false, "Target simulation frames per second", 1000.0);
    schema.addBoolField("enable_compression", false, "Enable result compression", true);
    
    return schema;
}

ConfigSchema PredefinedSchemas::getVisualizationSchema() {
    ConfigSchema schema;
    
    schema.addBoolField("enable_visualization", false, "Enable visualization output", true);
    schema.addStringField("visualization_backend", false, "Visualization backend (matplotlib/plotly)", "matplotlib");
    schema.addIntField("update_rate", false, "Visualization update rate in Hz", 30);
    schema.addFloatField("simulation_speed", false, "Visualization simulation speed factor", 1.0);
    schema.addBoolField("show_neurons", false, "Show individual neurons in visualization", false);
    schema.addBoolField("show_synapses", false, "Show synaptic connections in visualization", false);
    schema.addStringField("output_format", false, "Output format for visualizations (png/svg/pdf)", "png");
    schema.addIntField("resolution", false, "Visualization resolution in pixels", 1920);
    
    return schema;
}

std::map<std::string, std::variant<int, double, bool, std::string>> 
PredefinedSchemas::applySchema(const ConfigSchema& schema,
                               const std::map<std::string, std::variant<int, double, bool, std::string>>& config) {
    std::map<std::string, std::variant<int, double, bool, std::string>> result;
    
    // Apply configuration values that pass schema validation
    for (const auto& pair : schema.fields) {
        const std::string& fieldName = pair.first;
        const ConfigSchema::FieldInfo& fieldInfo = pair.second;
        
        if (config.find(fieldName) != config.end()) {
            // Copy validated value
            result[fieldName] = config.at(fieldName);
        } else if (fieldInfo.required) {
            // Use default value if required and not provided
            if (std::holds_alternative<int>(fieldInfo.defaultValue)) {
                result[fieldName] = std::get<int>(fieldInfo.defaultValue);
            } else if (std::holds_alternative<double>(fieldInfo.defaultValue)) {
                result[fieldName] = std::get<double>(fieldInfo.defaultValue);
            } else if (std::holds_alternative<bool>(fieldInfo.defaultValue)) {
                result[fieldName] = std::get<bool>(fieldInfo.defaultValue);
            } else if (std::holds_alternative<std::string>(fieldInfo.defaultValue)) {
                result[fieldName] = std::get<std::string>(fieldInfo.defaultValue);
            }
        }
    }
    
    return result;
}

} // namespace config
} // namespace nlm

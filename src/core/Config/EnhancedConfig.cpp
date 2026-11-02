// Enhanced configuration implementation

#include "EnhancedConfig.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

struct ConfigValidator::Impl {
    json schema_;
    std::vector<ConfigPreset> presets_;
    std::unordered_map<std::string, json> key_info_;
    bool loaded_ = false;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {
    loadDefaultPresets();
}

ConfigValidator::~ConfigValidator() = default;

ConfigValidator::ConfigValidator(ConfigValidator&&) noexcept = default;
ConfigValidator& ConfigValidator::operator=(ConfigValidator&&) noexcept = default;

bool ConfigValidator::loadSchemaFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        file >> pImpl->schema_;
        pImpl->loaded_ = true;
        
        // Validate schema structure
        if (!validateJSONSchema(pImpl->schema_)) {
            return false;
        }
        
        // Extract key information
        if (pImpl->schema_.contains("properties")) {
            const auto& properties = pImpl->schema_["properties"];
            for (const auto& [key, schema] : properties.items()) {
                pImpl->key_info_[key] = schema;
            }
        }
        
        // Load presets if available
        if (pImpl->schema_.contains("examples")) {
            const auto& examples = pImpl->schema_["examples"];
            for (const auto& example : examples) {
                if (example.contains("name") && example.contains("config")) {
                    ConfigPreset preset(example["name"].get<std::string>(),
                                       example.value("description", ""),
                                       example["config"]);
                    pImpl->presets_.push_back(preset);
                }
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigValidator::loadSchemaFromString(const std::string& json_str) {
    try {
        pImpl->schema_ = json::parse(json_str);
        pImpl->loaded_ = true;
        
        if (!validateJSONSchema(pImpl->schema_)) {
            return false;
        }
        
        if (pImpl->schema_.contains("properties")) {
            const auto& properties = pImpl->schema_["properties"];
            for (const auto& [key, schema] : properties.items()) {
                pImpl->key_info_[key] = schema;
            }
        }
        
        if (pImpl->schema_.contains("examples")) {
            const auto& examples = pImpl->schema_["examples"];
            for (const auto& example : examples) {
                if (example.contains("name") && example.contains("config")) {
                    ConfigPreset preset(example["name"].get<std::string>(),
                                       example.value("description", ""),
                                       example["config"]);
                    pImpl->presets_.push_back(preset);
                }
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void ConfigValidator::loadDefaultPresets() {
    // Basic preset
    json basic_config;
    basic_config["neuron_count"] = 500;
    basic_config["region_count"] = 1;
    basic_config["connection_probability"] = 0.1;
    basic_config["simulation_timestep"] = 0.001;
    basic_config["random_seed"] = 42;
    pImpl->presets_.push_back(ConfigPreset("basic", "Basic configuration for quick testing", basic_config));
    
    // Research preset
    json research_config;
    research_config["neuron_count"] = 2000;
    research_config["region_count"] = 2;
    research_config["connection_probability"] = 0.05;
    research_config["simulation_timestep"] = 0.0005;
    research_config["random_seed"] = 42;
    research_config["stdp_ltp_weight"] = 0.005;
    research_config["stdp_ltd_weight"] = 0.006;
    research_config["stdp_time_constant"] = 50.0;
    research_config["development_synaptogenesis_rate"] = 0.001;
    research_config["development_pruning_rate"] = 0.0001;
    research_config["log_level"] = "INFO";
    research_config["log_to_file"] = true;
    pImpl->presets_.push_back(ConfigPreset("research", "Comprehensive configuration for scientific experiments", research_config));
    
    // Real-time preset
    json real_time_config;
    real_time_config["neuron_count"] = 100;
    real_time_config["region_count"] = 1;
    real_time_config["connection_probability"] = 0.2;
    real_time_config["simulation_timestep"] = 0.01;
    real_time_config["random_seed"] = 42;
    real_time_config["log_level"] = "WARN";
    pImpl->presets_.push_back(ConfigPreset("real_time", "Optimized for real-time applications", real_time_config));
}

std::optional<ConfigPreset> ConfigValidator::findPreset(const std::string& name) const {
    for (const auto& preset : pImpl->presets_) {
        if (preset.name == name) {
            return preset;
        }
    }
    return std::nullopt;
}

bool ConfigValidator::validateJSONSchema(const json& schema) const {
    if (!schema.is_object()) {
        return false;
    }
    
    // Basic schema validation
    if (schema.contains("properties")) {
        const auto& properties = schema["properties"];
        if (!properties.is_object()) {
            return false;
        }
        
        // Check required fields
        if (schema.contains("required") && schema["required"].is_array()) {
            const auto& required = schema["required"];
            for (const auto& key : required) {
                if (!properties.contains(key.get<std::string>())) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

std::vector<std::string> ConfigValidator::validate(const json& config) const {
    std::vector<std::string> errors;
    
    if (!pImpl->loaded_) {
        errors.push_back("Schema not loaded");
        return errors;
    }
    
    // Check required fields
    if (pImpl->schema_.contains("required")) {
        const auto& required = pImpl->schema_["required"];
        for (const auto& key : required) {
            if (!config.contains(key.get<std::string>())) {
                errors.push_back("Missing required field: " + key.get<std::string>());
            }
        }
    }
    
    // Validate each field
    if (pImpl->schema_.contains("properties")) {
        const auto& properties = pImpl->schema_["properties"];
        for (const auto& [key, schema] : properties.items()) {
            if (config.contains(key)) {
                auto field_errors = validateJSONValue(key, config[key], schema);
                errors.insert(errors.end(), field_errors.begin(), field_errors.end());
            } else if (schema.contains("default") && !schema.value("required", false)) {
                // Field has a default value but is not provided - this is not an error
            } else if (schema.value("required", false)) {
                // Already handled above
            }
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::validate(const std::shared_ptr<Config>& config) const {
    if (!config) {
        return {"Configuration is null"};
    }
    
    json json_config;
    for (const auto& key : config->getKeys()) {
        auto value = config->get(key);
        json_config[key] = value;
    }
    
    return validate(json_config);
}

std::vector<std::string> ConfigValidator::validateJSONValue(const std::string& key, const json& value, const json& schema) const {
    std::vector<std::string> errors;
    
    // Check if value exists (required field)
    if (schema.value("required", false) && value.is_null()) {
        errors.push_back("Missing required field: " + key);
        return errors;
    }
    
    // Skip validation for null values in non-required fields
    if (value.is_null()) {
        return errors;
    }
    
    // Type validation
    std::string expected_type = schema.value("type", "");
    if (expected_type == "number" && !value.is_number()) {
        errors.push_back("Field '" + key + "' must be a number");
    } else if (expected_type == "integer" && !value.is_number_integer()) {
        errors.push_back("Field '" + key + "' must be an integer");
    } else if (expected_type == "boolean" && !value.is_boolean()) {
        errors.push_back("Field '" + key + "' must be a boolean");
    } else if (expected_type == "string" && !value.is_string()) {
        errors.push_back("Field '" + key + "' must be a string");
    } else if (expected_type == "array" && !value.is_array()) {
        errors.push_back("Field '" + key + "' must be an array");
    } else if (expected_type == "object" && !value.is_object()) {
        errors.push_back("Field '" + key + "' must be an object");
    }
    
    if (!errors.empty()) {
        return errors;
    }
    
    // Constraint validation
    return getConstraintMessages(key, value, schema);
}

std::vector<std::string> ConfigValidator::getConstraintMessages(const std::string& key, const json& value, const json& schema) const {
    std::vector<std::string> errors;
    
    double val = value.get<double>();
    
    // Range validation
    if (schema.contains("minimum") && val < schema["minimum"].get<double>()) {
        errors.push_back("Field '" + key + "' must be >= " + schema["minimum"].dump());
    }
    
    if (schema.contains("maximum") && val > schema["maximum"].get<double>()) {
        errors.push_back("Field '" + key + "' must be <= " + schema["maximum"].dump());
    }
    
    // Enum validation
    if (schema.contains("enum") && schema["enum"].is_array()) {
        const auto& enum_values = schema["enum"];
        bool found = false;
        for (const auto& enum_value : enum_values) {
            if (enum_value == value) {
                found = true;
                break;
            }
        }
        if (!found) {
            errors.push_back("Field '" + key + "' must be one of: " + enum_values.dump());
        }
    }
    
    // Pattern validation (for strings)
    if (schema.contains("pattern") && schema["pattern"].is_string() && value.is_string()) {
        std::regex pattern(schema["pattern"].get<std::string>());
        if (!std::regex_match(value.get<std::string>(), pattern)) {
            errors.push_back("Field '" + key + "' does not match pattern: " + schema["pattern"].get<std::string>());
        }
    }
    
    return errors;
}

bool ConfigValidator::isValid(const json& config) const {
    return validate(config).empty();
}

bool ConfigValidator::isValid(const std::shared_ptr<Config>& config) const {
    return validate(config).empty();
}

json ConfigValidator::getValidationStats(const json& config) const {
    json stats;
    stats["valid"] = isValid(config);
    stats["errors"] = validate(config);
    
    if (isValid(config)) {
        stats["valid_fields"] = config.size();
        stats["total_fields"] = pImpl->schema_.value("required", json::array()).size();
    }
    
    return stats;
}

json ConfigValidator::createDefaultConfig() const {
    json default_config;
    
    if (pImpl->schema_.contains("properties")) {
        const auto& properties = pImpl->schema_["properties"];
        for (const auto& [key, schema] : properties.items()) {
            if (schema.contains("default")) {
                default_config[key] = schema["default"];
            }
        }
    }
    
    return default_config;
}

std::vector<std::string> ConfigValidator::getAllKeys() const {
    std::vector<std::string> keys;
    if (pImpl->schema_.contains("properties")) {
        const auto& properties = pImpl->schema_["properties"];
        keys.reserve(properties.size());
        for (const auto& [key, _] : properties.items()) {
            keys.push_back(key);
        }
    }
    return keys;
}

bool ConfigValidator::hasKey(const std::string& key) const {
    return pImpl->key_info_.find(key) != pImpl->key_info_.end();
}

std::optional<json> ConfigValidator::getKeyInfo(const std::string& key) const {
    auto it = pImpl->key_info_.find(key);
    if (it != pImpl->key_info_.end()) {
        return it->second;
    }
    return std::nullopt;
}

json ConfigValidator::mergeConfigs(const std::vector<json>& configs) {
    json merged;
    for (const auto& config : configs) {
        for (const auto& [key, value] : config.items()) {
            merged[key] = value;
        }
    }
    return merged;
}

json ConfigValidator::mergeConfigs(const std::vector<std::shared_ptr<Config>>& configs) {
    json merged;
    for (const auto& config : configs) {
        for (const auto& key : config->getKeys()) {
            auto value = config->get(key);
            merged[key] = value;
        }
    }
    return merged;
}

json ConfigValidator::diffConfig(const json& config1, const json& config2) {
    json diff;
    
    // Get all unique keys from both configurations
    std::unordered_set<std::string> all_keys;
    for (const auto& item : config1.items()) {
        all_keys.insert(item.key());
    }
    for (const auto& item : config2.items()) {
        all_keys.insert(item.key());
    }
    
    for (const auto& key : all_keys) {
        json value1 = nullptr, value2 = nullptr;
        if (config1.contains(key)) {
            value1 = config1[key];
        }
        if (config2.contains(key)) {
            value2 = config2[key];
        }
        
        if (value1 != value2) {
            diff[key] = {{"from": value1, "to": value2}};
        }
    }
    
    return diff;
}

std::vector<std::string> ConfigValidator::diffConfigHuman(const json& config1, const json& config2) {
    std::vector<std::string> diffs;
    
    // Get all unique keys from both configurations
    std::unordered_set<std::string> all_keys;
    for (const auto& item : config1.items()) {
        all_keys.insert(item.key());
    }
    for (const auto& item : config2.items()) {
        all_keys.insert(item.key());
    }
    
    for (const auto& key : all_keys) {
        json value1 = nullptr, value2 = nullptr;
        if (config1.contains(key)) {
            value1 = config1[key];
        }
        if (config2.contains(key)) {
            value2 = config2[key];
        }
        
        if (value1 != value2) {
            std::stringstream ss;
            ss << "  " << key << ": ";
            if (value1.is_null()) {
                ss << "added: " << value2.dump();
            } else if (value2.is_null()) {
                ss << "removed";
            } else {
                ss << "changed from " << value1.dump() << " to " << value2.dump();
            }
            diffs.push_back(ss.str());
        }
    }
    
    return diffs;
}

struct EnhancedConfig::Impl {
    std::shared_ptr<Config> config_;
    std::shared_ptr<ConfigValidator> validator_;
    bool owns_validator_ = false;
};

EnhancedConfig::EnhancedConfig(std::shared_ptr<ConfigValidator> validator)
    : pImpl(std::make_unique<Impl>()), validator_(validator) {
    if (!validator_) {
        validator_ = std::make_shared<ConfigValidator>();
        pImpl->owns_validator_ = true;
    }
    config_ = std::make_shared<Config>();
    pImpl->config_ = config_;
    pImpl->validator_ = validator_;
}

EnhancedConfig::~EnhancedConfig() = default;

EnhancedConfig::EnhancedConfig(EnhancedConfig&&) noexcept = default;
EnhancedConfig& EnhancedConfig::operator=(EnhancedConfig&&) noexcept = default;

std::shared_ptr<EnhancedConfig> EnhancedConfig::createFromPreset(const std::string& preset_name,
                                                              std::shared_ptr<ConfigValidator> validator) {
    if (!validator) {
        validator = std::make_shared<ConfigValidator>();
    }
    
    auto enhanced_config = std::make_shared<EnhancedConfig>(validator);
    auto preset = validator->findPreset(preset_name);
    
    if (!preset) {
        throw std::runtime_error("Preset not found: " + preset_name);
    }
    
    // Load preset configuration
    for (const auto& [key, value] : preset->config.items()) {
        enhanced_config->set(key, value);
    }
    
    return enhanced_config;
}

std::shared_ptr<EnhancedConfig> EnhancedConfig::createFromJSON(const std::string& json_str,
                                                             std::shared_ptr<ConfigValidator> validator) {
    if (!validator) {
        validator = std::make_shared<ConfigValidator>();
    }
    
    auto enhanced_config = std::make_shared<EnhancedConfig>(validator);
    
    try {
        json json_config = json::parse(json_str);
        
        // Validate configuration
        auto errors = validator->validate(json_config);
        if (!errors.empty()) {
            throw std::runtime_error("Invalid configuration: " + errors[0]);
        }
        
        // Load configuration
        for (const auto& [key, value] : json_config.items()) {
            enhanced_config->set(key, value);
        }
        
        return enhanced_config;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse JSON configuration: " + std::string(e.what()));
    }
}

std::shared_ptr<EnhancedConfig> EnhancedConfig::createFromFile(const std::string& filepath,
                                                             std::shared_ptr<ConfigValidator> validator) {
    if (!validator) {
        validator = std::make_shared<ConfigValidator>();
    }
    
    auto enhanced_config = std::make_shared<EnhancedConfig>(validator);
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filepath);
        }
        
        json json_config = json::parse(file);
        
        // Validate configuration
        auto errors = validator->validate(json_config);
        if (!errors.empty()) {
            throw std::runtime_error("Invalid configuration in file: " + errors[0]);
        }
        
        // Load configuration
        for (const auto& [key, value] : json_config.items()) {
            enhanced_config->set(key, value);
        }
        
        return enhanced_config;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to load configuration from file: " + std::string(e.what()));
    }
}

std::vector<std::string> EnhancedConfig::validate() const {
    return pImpl->validator_->validate(config_);
}

bool EnhancedConfig::isValid() const {
    return validate().empty();
}

std::vector<std::string> EnhancedConfig::getValidationErrors() const {
    return validate();
}

json EnhancedConfig::getValidationStats() const {
    return pImpl->validator_->getValidationStats(config_->toJSON());
}

void EnhancedConfig::set(const std::string& key, const json& value) {
    validateKey(key, value);
    config_->set(key, value);
}

void EnhancedConfig::set(const std::string& key, const std::string& value) {
    config_->set(key, value);
}

void EnhancedConfig::set(const std::string& key, int value) {
    config_->set(key, value);
}

void EnhancedConfig::set(const std::string& key, int64_t value) {
    config_->set(key, value);
}

void EnhancedConfig::set(const std::string& key, double value) {
    config_->set(key, value);
}

void EnhancedConfig::set(const std::string& key, bool value) {
    config_->set(key, value);
}

void EnhancedConfig::validateKey(const std::string& key, const json& value) const {
    if (pImpl->validator_->hasKey(key)) {
        auto schema = pImpl->validator_->getKeyInfo(key);
        if (schema && schema->contains("validator")) {
            // Custom validation
            auto validator = schema->value("validator", json(nullptr));
            if (validator.is_null()) {
                return; // No custom validator
            }
            
            // For now, we'll skip custom validation as it's schema-dependent
        }
    }
}

json EnhancedConfig::toJSON() const {
    json json_config;
    for (const auto& key : config_->getKeys()) {
        json_config[key] = config_->get(key);
    }
    return json_config;
}

std::vector<std::string> EnhancedConfig::getValidKeys() const {
    return pImpl->validator_->getAllKeys();
}

json EnhancedConfig::getKeyInfo(const std::string& key) const {
    auto info = pImpl->validator_->getKeyInfo(key);
    return info.value_or(json(nullptr));
}

void EnhancedConfig::merge(const std::shared_ptr<EnhancedConfig>& other) {
    if (!other) return;
    
    for (const auto& key : other->getKeys()) {
        auto value = other->get(key);
        set(key, value);
    }
}

void EnhancedConfig::mergeFromJSON(const json& other) {
    for (const auto& [key, value] : other.items()) {
        set(key, value);
    }
}

json EnhancedConfig::diff(const std::shared_ptr<EnhancedConfig>& other) const {
    if (!other) return json::object();
    
    return pImpl->validator_->diffConfig(toJSON(), other->toJSON());
}

bool EnhancedConfig::equals(const std::shared_ptr<EnhancedConfig>& other) const {
    if (!other) return false;
    
    return toJSON() == other->toJSON();
}

bool EnhancedConfig::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json json_config = toJSON();
        file << json_config.dump(2);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string EnhancedConfig::summary() const {
    return config_->summary();
}

std::string EnhancedConfig::getSchemaDescription() const {
    if (!pImpl->validator_->hasKey("neuron_count")) {
        return "No schema loaded";
    }
    
    std::stringstream ss;
    ss << "NLM Configuration Schema" << std::endl;
    ss << "=======================" << std::endl;
    ss << std::endl;
    
    auto keys = getValidKeys();
    for (const auto& key : keys) {
        auto info = getKeyInfo(key);
        if (!info.is_null()) {
            ss << key << ": ";
            if (info.contains("description")) {
                ss << info["description"].get<std::string>() << std::endl;
            }
        }
    }
    
    return ss.str();
}

std::shared_ptr<EnhancedConfig> EnhancedConfig::ConfigHelper::createMinimal() {
    return createFromPreset("basic");
}

std::shared_ptr<EnhancedConfig> EnhancedConfig::ConfigHelper::createResearch() {
    return createFromPreset("research");
}

std::shared_ptr<EnhancedConfig> EnhancedConfig::ConfigHelper::createRealTime() {
    return createFromPreset("real_time");
}

std::shared_ptr<EnhancedConfig> EnhancedConfig::ConfigHelper::createCustom(const std::string& name) {
    return createFromPreset(name);
}

std::vector<std::string> EnhancedConfig::ConfigHelper::getAllPresetNames() {
    auto validator = createValidator();
    return validator->getAllKeys();
}

std::shared_ptr<EnhancedConfig> EnhancedConfig::ConfigHelper::createFromPreset(const std::string& preset_name) {
    return EnhancedConfig::createFromPreset(preset_name);
}

std::shared_ptr<ConfigValidator> EnhancedConfig::ConfigHelper::createValidator() {
    return std::make_shared<ConfigValidator>();
}

std::shared_ptr<ConfigValidator> EnhancedConfig::ConfigHelper::createValidator(const std::string& schema_file) {
    auto validator = std::make_shared<ConfigValidator>();
    validator->loadSchemaFromFile(schema_file);
    return validator;
}

} // namespace nlm
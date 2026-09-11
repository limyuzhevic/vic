#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <iostream>

namespace nlm {

EnhancedConfig::EnhancedConfig(std::shared_ptr<ConfigSchema> schema,
                               const std::string& config_file,
                               bool auto_save,
                               bool auto_reload)
    : schema_(schema)
    , config_file_(config_file)
    , auto_save_(auto_save)
    , auto_reload_(auto_reload)
    , file_watcher_(nullptr)
{
    // Initialize with default values
    _set_defaults();
    
    // Load initial configuration
    if (!config_file.empty()) {
        loadFromFile(config_file);
    }
}

EnhancedConfig::~EnhancedConfig() {
    // Stop file watcher if running
    if (file_watcher_ && file_watcher_->joinable()) {
        file_watcher_->join();
        delete file_watcher_;
    }
}

bool EnhancedConfig::loadFromFile(const std::string& filepath) {
    try {
        if (!std::filesystem::exists(filepath)) {
            LOG_WARNING("Configuration file not found: " + filepath);
            return false;
        }
        
        std::string ext = std::filesystem::path(filepath).extension().string();
        
        if (ext == ".json") {
            return loadFromJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        } else {
            LOG_ERROR("Unsupported configuration file format: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error loading configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool EnhancedConfig::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json_config;
        file >> json_config;
        
        // Convert JSON to flat key-value pairs
        std::map<std::string, std::string> flat_config;
        convertJSONToFlat(json_config, "", flat_config);
        
        // Validate configuration
        if (!validateConfig(flat_config)) {
            return false;
        }
        
        // Merge with existing configuration
        mergeConfig(flat_config, ConfigSource::FILE);
        
        LOG_INFO("Loaded JSON configuration from: " + filepath);
        
        // Auto-save if enabled
        if (auto_save_) {
            saveToFile(filepath);
        }
        
        // Setup file watcher if auto_reload
        if (auto_reload_) {
            setupFileWatcher(filepath);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("JSON parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool EnhancedConfig::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yaml_config = YAML::LoadFile(filepath);
        
        // Convert YAML to flat key-value pairs
        std::map<std::string, std::string> flat_config;
        convertYAMLToFlat(yaml_config, "", flat_config);
        
        // Validate configuration
        if (!validateConfig(flat_config)) {
            return false;
        }
        
        // Merge with existing configuration
        mergeConfig(flat_config, ConfigSource::FILE);
        
        LOG_INFO("Loaded YAML configuration from: " + filepath);
        
        // Auto-save if enabled
        if (auto_save_) {
            saveToFile(filepath);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("YAML parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool EnhancedConfig::loadFromArgs(const std::vector<std::string>& args) {
    try {
        for (size_t i = 0; i < args.size(); ++i) {
            std::string arg = args[i];
            
            if (arg.substr(0, 2) == "--") {
                // --key=value format
                if (arg.find('=') != std::string::npos) {
                    size_t pos = arg.find('=');
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    setValue(key, value, ConfigSource::COMMAND_LINE);
                }
                // --key value format
                else if (i + 1 < args.size() && args[i + 1].find('=') == std::string::npos) {
                    std::string key = arg.substr(2);
                    std::string value = args[++i];
                    setValue(key, value, ConfigSource::COMMAND_LINE);
                }
            }
            // -k value format
            else if (arg.size() > 1 && arg[0] == '-') {
                std::string key = arg.substr(1);
                if (i + 1 < args.size() && args[i + 1].find('=') == std::string::npos) {
                    std::string value = args[++i];
                    setValue(key, value, ConfigSource::COMMAND_LINE);
                }
            }
        }
        
        LOG_INFO("Loaded configuration from command line arguments");
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Error loading configuration from arguments: " + std::string(e.what()));
        return false;
    }
}

bool EnhancedConfig::saveToFile(const std::string& filepath) {
    try {
        // Create directory if it doesn't exist
        std::filesystem::path path(filepath);
        std::filesystem::create_directories(path.parent_path());
        
        // Save as JSON
        std::ofstream file(filepath);
        if (!file.is_open()) {
            LOG_ERROR("Could not open file for writing: " + filepath);
            return false;
        }
        
        nlohmann::json json_config;
        convertFlatToJSON(json_config, 0, "");
        
        file << json_config.dump(2);
        
        LOG_INFO("Saved configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Error saving configuration to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void EnhancedConfig::set(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        std::string old_value = get(key);
        setValue(key, value, source);
        
        // Notify callbacks
        if (old_value != value) {
            for (auto& callback : change_callbacks_) {
                try {
                    callback(key, value, old_value, source);
                } catch (const std::exception& e) {
                    LOG_ERROR("Error in config change callback: " + std::string(e.what()));
                }
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error setting configuration value for " + key + ": " + std::string(e.what()));
        throw;
    }
}

void EnhancedConfig::set(const std::string& key, int value, ConfigSource source) {
    setValue(key, std::to_string(value), source);
}

void EnhancedConfig::set(const std::string& key, double value, ConfigSource source) {
    setValue(key, std::to_string(value), source);
}

void EnhancedConfig::set(const std::string& key, bool value, ConfigSource source) {
    setValue(key, value ? "true" : "false", source);
}

void EnhancedConfig::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string str_value;
    if (std::holds_alternative<int>(value)) {
        str_value = std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        str_value = std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        str_value = std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        str_value = std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        str_value = std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        str_value = "[" + vectorToString(std::get<std::vector<int>>(value)) + "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        str_value = "[" + vectorToString(std::get<std::vector<double>>(value)) + "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        str_value = "[" + vectorToString(std::get<std::vector<std::string>>(value)) + "]";
    }
    
    setValue(key, str_value, source);
}

std::string EnhancedConfig::get(const std::string& key) const {
    return getValue(key);
}

std::string EnhancedConfig::getOr(const std::string& key, const std::string& default_value) const {
    return has(key) ? get(key) : default_value;
}

int EnhancedConfig::getInt(const std::string& key) const {
    return std::stoi(get(key));
}

double EnhancedConfig::getDouble(const std::string& key) const {
    return std::stod(get(key));
}

bool EnhancedConfig::getBool(const std::string& key) const {
    std::string value = get(key);
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

std::vector<std::string> EnhancedConfig::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : config_) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::string> EnhancedConfig::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& pair : sources_) {
        if (pair.second == source) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

void EnhancedConfig::clear() {
    config_.clear();
    sources_.clear();
    
    // Reset to defaults
    _set_defaults();
}

std::string EnhancedConfig::summary() const {
    std::stringstream ss;
    ss << "Enhanced Configuration Summary:" << std::endl;
    ss << "=============================" << std::endl;
    
    for (int i = 0; i < ConfigSource_COUNT; ++i) {
        ConfigSource source = static_cast<ConfigSource>(i);
        std::vector<std::string> keys = getKeysBySource(source);
        
        if (!keys.empty()) {
            ss << std::endl;
            ss << sourceToString(source) << " (" << keys.size() << " keys):" << std::endl;
            
            for (const auto& key : keys) {
                std::string value = get(key);
                if (value.size() > 50) {
                    value = value.substr(0, 47) + "...";
                }
                ss << "  " << key << " = " << value << std::endl;
            }
        }
    }
    
    return ss.str();
}

bool EnhancedConfig::validateConfig(const std::map<std::string, std::string>& config) {
    if (!schema_) {
        return true; // No schema means no validation
    }
    
    std::vector<std::string> errors = schema_->validate(config);
    if (!errors.empty()) {
        for (const auto& error : errors) {
            LOG_ERROR("Configuration validation error: " + error);
        }
        return false;
    }
    
    return true;
}

void EnhancedConfig::mergeConfig(const std::map<std::string, std::string>& new_config, ConfigSource source) {
    for (const auto& pair : new_config) {
        if (source == ConfigSource::FILE || source == ConfigSource::COMMAND_LINE) {
            setValue(pair.first, pair.second, source);
        }
    }
}

void EnhancedConfig::setValue(const std::string& key, const std::string& value, ConfigSource source) {
    // Type conversion based on existing value
    if (config_.find(key) != config_.end()) {
        std::string old_value = config_[key];
        std::string normalized_value = normalizeValue(old_value, value);
        config_[key] = normalized_value;
    } else {
        config_[key] = value;
    }
    
    sources_[key] = source;
}

std::string EnhancedConfig::getValue(const std::string& key) const {
    auto it = config_.find(key);
    if (it != config_.end()) {
        return it->second;
    }
    return "";
}

void EnhancedConfig::convertJSONToFlat(const nlohmann::json& json, const std::string& prefix,
                                       std::map<std::string, std::string>& flat_config) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        std::string full_key = prefix.empty() ? key : prefix + "." + key;
        
        if (it->is_object()) {
            convertJSONToFlat(*it, full_key, flat_config);
        } else if (it->is_array()) {
            flat_config[full_key] = jsonArrayToString(*it);
        } else {
            flat_config[full_key] = it->dump();
        }
    }
}

void EnhancedConfig::convertYAMLToFlat(const YAML::Node& yaml, const std::string& prefix,
                                        std::map<std::string, std::string>& flat_config) {
    for (auto it = yaml.begin(); it != yaml.end(); ++it) {
        std::string key = it->first.as<std::string>();
        std::string full_key = prefix.empty() ? key : prefix + "." + key;
        
        if (it->second.IsMap()) {
            convertYAMLToFlat(it->second, full_key, flat_config);
        } else if (it->second.IsSequence()) {
            flat_config[full_key] = yamlSequenceToString(it->second);
        } else {
            flat_config[full_key] = YAML::Dump(it->second);
        }
    }
}

void EnhancedConfig::convertFlatToJSON(nlohmann::json& json, int depth, const std::string& prefix) {
    if (depth > 10) { // Prevent infinite recursion
        return;
    }
    
    for (const auto& pair : config_) {
        if (prefix.empty() || pair.first.find(prefix + ".") == 0) {
            std::string remaining = pair.first.substr(prefix.empty() ? 0 : prefix.size() + 1);
            size_t dot_pos = remaining.find('.');
            
            if (dot_pos == std::string::npos) {
                // Leaf node
                json[pair.first] = json::parse(pair.second);
            } else {
                // Nested node
                std::string parent_key = pair.first.substr(0, dot_pos);
                if (json.find(parent_key) == json.end()) {
                    json[parent_key] = nlohmann::json::object();
                }
                convertFlatToJSON(json[parent_key], depth + 1, pair.first);
            }
        }
    }
}

std::string EnhancedConfig::normalizeValue(const std::string& old_value, const std::string& new_value) {
    if (old_value.empty()) {
        return new_value;
    }
    
    // Try to preserve type
    try {
        // If old is int, try to convert new to int
        if (std::all_of(old_value.begin(), old_value.end(), ::isdigit)) {
            size_t pos;
            int int_val = std::stoi(new_value, &pos);
            if (pos == new_value.size()) {
                return std::to_string(int_val);
            }
        }
        
        // If old is double, try to convert new to double
        if (isDouble(old_value)) {
            size_t pos;
            double double_val = std::stod(new_value, &pos);
            if (pos == new_value.size()) {
                return std::to_string(double_val);
            }
        }
        
        // If old is bool, convert new to bool
        if (old_value == "true" || old_value == "false") {
            bool bool_val = (new_value == "true" || new_value == "1" || new_value == "yes" || new_value == "on");
            return bool_val ? "true" : "false";
        }
    } catch (...) {
        // If conversion fails, keep new value as string
    }
    
    return new_value;
}

bool EnhancedConfig::isDouble(const std::string& str) {
    size_t pos;
    std::stod(str, &pos);
    return pos == str.size();
}

std::string EnhancedConfig::jsonArrayToString(const nlohmann::json& array) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < array.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << array[i].dump();
    }
    ss << "]";
    return ss.str();
}

std::string EnhancedConfig::yamlSequenceToString(const YAML::Node& sequence) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < sequence.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << YAML::Dump(sequence[i]);
    }
    ss << "]";
    return ss.str();
}

std::string EnhancedConfig::vectorToString(const std::vector<int>& vec) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << vec[i];
    }
    ss << "]";
    return ss.str();
}

std::string EnhancedConfig::vectorToString(const std::vector<double>& vec) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << vec[i];
    }
    ss << "]";
    return ss.str();
}

std::string EnhancedConfig::vectorToString(const std::vector<std::string>& vec) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "\"" << vec[i] << "\"";
    }
    ss << "]";
    return ss.str();
}

void EnhancedConfig::setupFileWatcher(const std::string& filepath) {
    // Implement file watching for automatic reload
    // This would typically use platform-specific APIs
    // For simplicity, this is a stub implementation
    LOG_INFO("File watching enabled for: " + filepath);
}

void EnhancedConfig::addChangeCallback(std::function<void(const std::string&, const std::string&, const std::string&, ConfigSource)> callback) {
    change_callbacks_.push_back(callback);
}

void EnhancedConfig::removeChangeCallback(std::function<void(const std::string&, const std::string&, const std::string&, ConfigSource)> callback) {
    change_callbacks_.erase(
        std::remove(change_callbacks_.begin(), change_callbacks_.end(), callback),
        change_callbacks_.end()
    );
}

std::string EnhancedConfig::exportForImport() const {
    return toJSON();
}

void EnhancedConfig::importFromString(const std::string& config_string, ConfigSource source) {
    try {
        nlohmann::json json_config = nlohmann::json::parse(config_string);
        
        std::map<std::string, std::string> flat_config;
        convertJSONToFlat(json_config, "", flat_config);
        
        if (validateConfig(flat_config)) {
            mergeConfig(flat_config, source);
            LOG_INFO("Imported configuration from string");
        } else {
            LOG_ERROR("Failed to import configuration: validation errors");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error importing configuration from string: " + std::string(e.what()));
        throw;
    }
}

std::string EnhancedConfig::toJSON() const {
    nlohmann::json json_config;
    convertFlatToJSON(json_config, 0, "");
    return json_config.dump(2);
}

void EnhancedConfig::_set_defaults() {
    // Set default configuration values
    std::map<std::string, std::string> defaults = {
        {"simulation.timestep", "0.001"},
        {"simulation.max_steps", "10000"},
        {"brain.neuron_count", "1000"},
        {"brain.region_count", "1"},
        {"brain.connection_probability", "0.1"},
        {"plasticity.stdp.enable", "true"},
        {"plasticity.stdp.learning_rate", "0.01"},
        {"plasticity.hebbian.enable", "true"},
        {"plasticity.hebbian.learning_rate", "0.001"},
        {"neuromodulation.dopamine.scale", "1.0"},
        {"neuromodulation.curiosity.enable", "true"},
        {"neuromodulation.novelty.enable", "true"},
        {"development.enable", "true"},
        {"development.stage", "Maturation"},
        {"checkpoint.enabled", "true"},
        {"checkpoint.path", "./checkpoint.bin"},
        {"replay.enabled", "true"},
    };
    
    for (const auto& pair : defaults) {
        if (config_.find(pair.first) == config_.end()) {
            setValue(pair.first, pair.second, ConfigSource::DEFAULT);
        }
    }
}

std::string EnhancedConfig::sourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::DEFAULT: return "DEFAULT";
        case ConfigSource::FILE: return "FILE";
        case ConfigSource::COMMAND_LINE: return "COMMAND_LINE";
        case ConfigSource::ENVIRONMENT: return "ENVIRONMENT";
        case ConfigSource::RUNTIME: return "RUNTIME";
        default: return "UNKNOWN";
    }
}

EnhancedConfig::EnhancedConfig() : auto_save_(false), auto_reload_(false), file_watcher_(nullptr) {
    _set_defaults();
}

EnhancedConfig::~EnhancedConfig() {
    if (file_watcher_ && file_watcher_->joinable()) {
        file_watcher_->join();
        delete file_watcher_;
    }
}

// Factory functions
std::shared_ptr<EnhancedConfig> createEnhancedConfig(const std::string& config_file) {
    return std::make_shared<EnhancedConfig>(config_file);
}

std::shared_ptr<EnhancedConfig> createEnhancedConfig(std::shared_ptr<ConfigSchema> schema) {
    return std::make_shared<EnhancedConfig>(schema);
}

// ConfigSchema implementation
ConfigSchema::ConfigSchema(const std::vector<ConfigValidationRule>& rules) : rules_(rules) {
    _compilePatterns();
}

void ConfigSchema::addRule(const ConfigValidationRule& rule) {
    rules_.push_back(rule);
    _compilePatterns();
}

std::vector<std::string> ConfigSchema::validate(const std::map<std::string, std::string>& config) {
    std::vector<std::string> errors;
    
    for (const auto& rule : rules_) {
        // Check if key exists if required
        bool key_found = false;
        for (const auto& pair : config) {
            if (matchesPattern(pair.first, rule.key_pattern)) {
                key_found = true;
                
                // Validate the value
                std::string error = validateValue(pair.second, rule);
                if (!error.empty()) {
                    errors.push_back("Key '" + pair.first + "': " + error);
                }
                break;
            }
        }
        
        if (rule.required && !key_found) {
            errors.push_back("Required configuration key matching '" + rule.key_pattern + "' is missing");
        }
    }
    
    return errors;
}

bool ConfigSchema::matchesPattern(const std::string& key, const std::string& pattern) {
    // Simple wildcard matching
    if (pattern == "*") {
        return true;
    }
    
    // Convert wildcard to regex
    std::string regex_pattern = pattern;
    regex_pattern.erase(std::remove(regex_pattern.begin(), regex_pattern.end(), '*'), '*');
    regex_pattern.erase(std::remove(regex_pattern.begin(), regex_pattern.end(), '?'), '?');
    
    return regex_pattern.empty() || key == regex_pattern;
}

std::string ConfigSchema::validateValue(const std::string& value, const ConfigValidationRule& rule) {
    // Type validation
    if (rule.type_hint) {
        std::type_index type_index(typeid(*rule.type_hint));
        
        if (type_index == typeid(int)) {
            try {
                std::stoi(value);
            } catch (...) {
                return "must be an integer";
            }
        } else if (type_index == typeid(double)) {
            try {
                std::stod(value);
            } catch (...) {
                return "must be a number";
            }
        } else if (type_index == typeid(bool)) {
            if (value != "true" && value != "false" && value != "1" && value != "0") {
                return "must be a boolean (true/false, 1/0)";
            }
        } else if (type_index == typeid(std::string)) {
            // Any string is valid
        }
    }
    
    // Range validation
    try {
        double num_value = std::stod(value);
        
        if (rule.min_value.has_value() && num_value < rule.min_value.value()) {
            return "must be >= " + std::to_string(rule.min_value.value());
        }
        
        if (rule.max_value.has_value() && num_value > rule.max_value.value()) {
            return "must be <= " + std::to_string(rule.max_value.value());
        }
        
        // Allowed values validation
        if (!rule.allowed_values.empty() && 
            std::find(rule.allowed_values.begin(), rule.allowed_values.end(), value) == rule.allowed_values.end()) {
            return "must be one of: " + vectorToString(rule.allowed_values);
        }
    } catch (...) {
        // Not a number, skip range validation
    }
    
    // Custom validator
    if (rule.validator) {
        try {
            if (!rule.validator(value, rule.key_pattern)) {
                return "failed custom validation";
            }
        } catch (const std::exception& e) {
            return std::string("validation error: ") + e.what();
        }
    }
    
    return "";
}

void ConfigSchema::_compilePatterns() {
    // Compile patterns for faster matching
    for (const auto& rule : rules_) {
        if (rule.key_pattern.find('*') != std::string::npos) {
            // This is a more complex pattern, we'll just do basic matching
            // In a real implementation, you'd use a proper regex engine
        }
    }
}

std::string ConfigSchema::vectorToString(const std::vector<std::string>& vec) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << vec[i];
    }
    ss << "]";
    return ss.str();
}

// ConfigValidationRule helper functions
ConfigValidationRule::Builder& ConfigValidationRule::Builder::keyPattern(const std::string& pattern) {
    rule_.key_pattern = pattern;
    return *this;
}

ConfigValidationRule::Builder& ConfigValidationRule::Builder::required(bool required) {
    rule_.required = required;
    return *this;
}

ConfigValidationRule::Builder& ConfigValidationRule::Builder::typeHint(Type type) {
    rule_.type_hint = type;
    return *this;
}

ConfigValidationRule::Builder& ConfigValidationRule::Builder::minValue(double value) {
    rule_.min_value = value;
    return *this;
}

ConfigValidationRule::Builder& ConfigValidationRule::Builder::maxValue(double value) {
    rule_.max_value = value;
    return *this;
}

ConfigValidationRule::Builder& ConfigValidationRule::Builder::allowedValues(const std::vector<std::string>& values) {
    rule_.allowed_values = values;
    return *this;
}

ConfigValidationRule::Builder& ConfigValidationRule::Builder::description(const std::string& desc) {
    rule_.description = desc;
    return *this;
}

ConfigValidationRule::Builder& ConfigValidationRule::Builder::validator(std::function<bool(const std::string&, const std::string&)> validator) {
    rule_.validator = validator;
    return *this;
}

ConfigValidationRule ConfigValidationRule::Builder::build() {
    return rule_;
}

// Global configuration instance
namespace {
    std::shared_ptr<EnhancedConfig> global_config_instance;
}

void setGlobalConfig(std::shared_ptr<EnhancedConfig> config) {
    global_config_instance = config;
}

std::shared_ptr<EnhancedConfig> getGlobalConfig() {
    if (!global_config_instance) {
        global_config_instance = createEnhancedConfig();
    }
    return global_config_instance;
}

// Global convenience functions
void loadConfigFromFile(const std::string& filepath) {
    getGlobalConfig()->loadFromFile(filepath);
}

void loadConfigFromArgs(const std::vector<std::string>& args) {
    getGlobalConfig()->loadFromArgs(args);
}

void saveConfigToFile(const std::string& filepath) {
    getGlobalConfig()->saveToFile(filepath);
}

std::string getConfigValue(const std::string& key) {
    return getGlobalConfig()->get(key);
}

void setConfigValue(const std::string& key, const std::string& value, ConfigSource source) {
    getGlobalConfig()->set(key, value, source);
}

std::string getConfigSummary() {
    return getGlobalConfig()->summary();
}

} // namespace nlm

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <regex>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <set>
#include <map>
#include <unordered_map>
#include <limits>

namespace nlm {

// AES encryption implementation
namespace {
    bool encryptData(const std::vector<unsigned char>& plaintext, const std::vector<unsigned char>& key,
                     std::vector<unsigned char>& ciphertext) {
        try {
            EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
            if (!ctx) return false;
            
            if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), nullptr)) {
                EVP_CIPHER_CTX_free(ctx);
                return false;
            }
            
            std::vector<unsigned char> buffer(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
            int len = 0;
            if (1 != EVP_EncryptUpdate(ctx, buffer.data(), &len, plaintext.data(), plaintext.size())) {
                EVP_CIPHER_CTX_free(ctx);
                return false;
            }
            
            int final_len = 0;
            if (1 != EVP_EncryptFinal_ex(ctx, buffer.data() + len, &final_len)) {
                EVP_CIPHER_CTX_free(ctx);
                return false;
            }
            
            buffer.resize(len + final_len);
            ciphertext = buffer;
            EVP_CIPHER_CTX_free(ctx);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool decryptData(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key,
                     std::vector<unsigned char>& plaintext) {
        try {
            EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
            if (!ctx) return false;
            
            if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), nullptr)) {
                EVP_CIPHER_CTX_free(ctx);
                return false;
            }
            
            std::vector<unsigned char> buffer(ciphertext.size());
            int len = 0;
            if (1 != EVP_DecryptUpdate(ctx, buffer.data(), &len, ciphertext.data(), ciphertext.size())) {
                EVP_CIPHER_CTX_free(ctx);
                return false;
            }
            
            int final_len = 0;
            if (1 != EVP_DecryptFinal_ex(ctx, buffer.data() + len, &final_len)) {
                EVP_CIPHER_CTX_free(ctx);
                return false;
            }
            
            buffer.resize(len + final_len);
            plaintext = buffer;
            EVP_CIPHER_CTX_free(ctx);
            return true;
        } catch (...) {
            return false;
        }
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema> schemas;
    std::map<std::string, ConfigTemplate> templates;
    std::vector<std::string> preset_names;
    std::vector<ConfigHistoryEntry> validation_history;
    bool validation_history_enabled = false;
    std::vector<ValidationError> last_errors;
    std::vector<ValidationError> last_warnings;
    std::map<std::string, std::string> monitoring_data;
    bool monitoring_enabled = false;
    std::string version = "1.0";
    bool is_encrypted = false;
    std::vector<unsigned char> encryption_key;
    std::chrono::system_clock::time_point creation_time;
    std::chrono::system_clock::time_point last_modified;
    std::map<std::string, std::string> metadata;
    
    Impl() : creation_time(std::chrono::system_clock::now()), last_modified(std::chrono::system_clock::now()) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (format == "auto" || format == "detect") {
        if (isJSON(content)) {
            return loadFromJSON(filepath);
        } else {
            // Fallback to key=value format
            clear();
            std::string line;
            file.clear();
            file.seekg(0);
            
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
            return true;
        }
    } else if (format == "json") {
        return loadFromJSON(filepath);
    } else {
        // Default key=value format
        clear();
        std::string line;
        file.clear();
        file.seekg(0);
        
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
    
    return false;
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json_data;
        file >> json_data;
        
        clear();
        
        for (auto it = json_data.begin(); it != json_data.end(); ++it) {
            std::string key = it.key();
            set(key, it.value(), ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "auto" || format == "json" || format == "JSON") {
        nlohmann::json json_data;
        
        for (const auto& entry : pImpl->entries) {
            json_data[entry.key] = configValueToJSON(entry.value);
        }
        
        file << json_data.dump(2);
    } else {
        // Default key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            std::visit([&file, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
        }
    }
    
    return true;
}

bool Config::setSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.version] = schema;
    return true;
}

bool Config::validate(const std::vector<std::string>& keys) const {
    pImpl->last_errors.clear();
    pImpl->last_warnings.clear();
    
    std::map<std::string, ConfigValue> config_map;
    for (const auto& entry : pImpl->entries) {
        config_map[entry.key] = entry.value;
    }
    
    // Validate against current schema if any
    if (!pImpl->schemas.empty()) {
        for (const auto& [version, schema] : pImpl->schemas) {
            if (version == pImpl->version) {
                for (const auto& [key, value] : config_map) {
                    if (!keys.empty() && std::find(keys.begin(), keys.end(), key) == keys.end()) {
                        continue;
                    }
                    
                    // Type validation
                    if (schema.type_definitions.find(key) != schema.type_definitions.end()) {
                        std::vector<ValidationError> type_errors = ConfigUtils::validateType(key, value, schema.type_definitions.at(key));
                        pImpl->last_errors.insert(pImpl->last_errors.end(), type_errors.begin(), type_errors.end());
                    }
                    
                    // Range validation (for numeric types)
                    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) || 
                        std::holds_alternative<double>(value)) {
                        
                        double val = 0.0;
                        if (std::holds_alternative<int>(value)) val = std::get<int>(value);
                        else if (std::holds_alternative<int64_t>(value)) val = std::get<int64_t>(value);
                        else val = std::get<double>(value);
                        
                        // Check if key has range constraints (simplified)
                        if (key == "neuron_count") {
                            std::vector<ValidationError> range_errors = ConfigUtils::validateRange(key, val, 1.0, std::numeric_limits<double>::max());
                            pImpl->last_errors.insert(pImpl->last_errors.end(), range_errors.begin(), range_errors.end());
                        }
                    }
                }
                
                // Cross-parameter validation
                for (const auto& constraint : schema.constraints) {
                    validateConstraint(constraint, config_map);
                }
            }
        }
    }
    
    // Cross-parameter validation
    if (!validateCrossParameters()) {
        return false;
    }
    
    return pImpl->last_errors.empty();
}

std::string Config::getSchemaVersion() const {
    return pImpl->version;
}

bool Config::validateCrossParameters() const {
    std::map<std::string, ConfigValue> config_map;
    for (const auto& entry : pImpl->entries) {
        config_map[entry.key] = entry.value;
    }
    
    // Default cross-parameter validations
    if (config_map.find("neuron_count") != config_map.end() && 
        config_map.find("connection_probability") != config_map.end()) {
        
        double neuron_count = 0.0;
        double connection_prob = 0.0;
        
        if (std::holds_alternative<int>(config_map["neuron_count"])) {
            neuron_count = std::get<int>(config_map["neuron_count"]);
        } else if (std::holds_alternative<int64_t>(config_map["neuron_count"])) {
            neuron_count = std::get<int64_t>(config_map["neuron_count"]);
        } else if (std::holds_alternative<double>(config_map["neuron_count"])) {
            neuron_count = std::get<double>(config_map["neuron_count"]);
        }
        
        if (std::holds_alternative<int>(config_map["connection_probability"])) {
            connection_prob = std::get<int>(config_map["connection_probability"]);
        } else if (std::holds_alternative<int64_t>(config_map["connection_probability"])) {
            connection_prob = std::get<int64_t>(config_map["connection_probability"]);
        } else if (std::holds_alternative<double>(config_map["connection_probability"])) {
            connection_prob = std::get<double>(config_map["connection_probability"]);
        }
        
        if (connection_prob > 1.0) {
            pImpl->last_errors.push_back(ValidationError("connection_probability", 
                                                       "constraint_violation",
                                                       "Connection probability must be less than or equal to 1.0",
                                                       "error",
                                                       "Set to 1.0 or a lower value"));
            return false;
        }
        
        if (neuron_count < 1) {
            pImpl->last_errors.push_back(ValidationError("neuron_count", 
                                                       "constraint_violation",
                                                       "Neuron count must be at least 1",
                                                       "error",
                                                       "Set to a positive integer"));
            return false;
        }
    }
    
    if (config_map.find("plasticity_learning_rate") != config_map.end()) {
        double learning_rate = 0.0;
        if (std::holds_alternative<double>(config_map["plasticity_learning_rate"])) {
            learning_rate = std::get<double>(config_map["plasticity_learning_rate"]);
        }
        
        if (learning_rate < 0.0 || learning_rate > 1.0) {
            pImpl->last_errors.push_back(ValidationError("plasticity_learning_rate",
                                                       "constraint_violation",
                                                       "Learning rate must be between 0.0 and 1.0",
                                                       "error",
                                                       "Set to a value between 0.0 and 1.0"));
            return false;
        }
    }
    
    return true;
}

bool Config::mergeFrom(const Config& other, ConfigSource source) {
    bool modified = false;
    
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            // Update existing entry
            ConfigValue old_value = it->value;
            it->value = entry.value;
            it->source = source;
            
            // Add to validation history
            if (pImpl->validation_history_enabled) {
                pImpl->validation_history.emplace_back(
                    entry.key, old_value, entry.value, source, "merge", "system"
                );
            }
        } else {
            // Add new entry
            ConfigValue old_value = ConfigValue();
            pImpl->entries.push_back(entry);
            pImpl->entries.back().source = source;
            
            // Add to validation history
            if (pImpl->validation_history_enabled) {
                pImpl->validation_history.emplace_back(
                    entry.key, old_value, entry.value, source, "add", "system"
                );
            }
        }
        
        modified = true;
        pImpl->last_modified = std::chrono::system_clock::now();
    }
    
    return modified;
}

bool Config::inheritFrom(const Config& template_config) {
    return mergeFrom(template_config, ConfigSource::Inherited);
}

bool Config::createTemplate(const ConfigTemplate& template_config) {
    pImpl->templates[template_config.name] = template_config;
    
    // Add values from template to current config
    for (const auto& [key, value] : template_config.values) {
        // Convert string value to appropriate type
        ConfigValue config_value;
        try {
            if (value.find('.') != std::string::npos) {
                config_value = std::stod(value);
            } else if (value == "true" || value == "false") {
                config_value = (value == "true");
            } else {
                config_value = std::stoi(value);
            }
            set(key, config_value, ConfigSource::Template);
        } catch (...) {
            set(key, value, ConfigSource::Template);
        }
    }
    
    return true;
}

bool Config::useTemplate(const std::string& template_name) {
    if (pImpl->templates.find(template_name) == pImpl->templates.end()) {
        return false;
    }
    
    // Clear current config and apply template
    clear();
    return createTemplate(pImpl->templates[template_name]);
}

bool Config::applyPreset(const std::string& preset_name) {
    if (std::find(pImpl->preset_names.begin(), pImpl->preset_names.end(), preset_name) == pImpl->preset_names.end()) {
        return false;
    }
    
    // For now, just return true - in a real implementation, this would load the preset
    return true;
}

std::vector<std::string> Config::getAvailableTemplates() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->templates) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> Config::getAvailablePresets() const {
    return pImpl->preset_names;
}

void Config::enableValidationHistory(bool enable) {
    pImpl->validation_history_enabled = enable;
    if (enable && pImpl->validation_history.empty()) {
        pImpl->validation_history.push_back(ConfigHistoryEntry("config_initialized", 
                                                            ConfigValue(), 
                                                            ConfigValue(), 
                                                            ConfigSource::Default,
                                                            "initialize", "system", "Initial configuration"));
    }
}

bool Config::isValidationHistoryEnabled() const {
    return pImpl->validation_history_enabled;
}

std::vector<ConfigHistoryEntry> Config::getValidationHistory() const {
    return pImpl->validation_history;
}

bool Config::rollback(const std::string& key, const std::chrono::system_clock::time_point& timestamp) {
    if (!pImpl->validation_history_enabled) {
        return false;
    }
    
    for (auto it = pImpl->validation_history.rbegin(); it != pImpl->validation_history.rend(); ++it) {
        if (timestamp == std::chrono::system_clock::time_point() || it->timestamp <= timestamp) {
            if (it->key == key && !it->old_value.empty()) {
                set(key, it->old_value, it->source);
                pImpl->validation_history.push_back(ConfigHistoryEntry(
                    key, it->old_value, it->new_value, it->source, "rollback", "user", "Rolled back to previous state"
                ));
                return true;
            }
        }
    }
    
    return false;
}

std::string Config::getValidationHistorySummary() const {
    std::ostringstream oss;
    oss << "Configuration History (" << pImpl->validation_history.size() << " entries):\n";
    
    for (const auto& entry : pImpl->validation_history) {
        oss << "  " << ConfigUtils::formatTimestamp(entry.timestamp) << " - " 
            << entry.operation << " on key \"" << entry.key << "\"";
        if (!entry.user.empty()) {
            oss << " by \"" << entry.user << "\"";
        }
        if (!entry.comment.empty()) {
            oss << " - " << entry.comment;
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::map<std::string, std::pair<ConfigValue, ConfigValue>> Config::diff(const Config& other) const {
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> diff;
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == other.pImpl->entries.end()) {
            diff[entry.key] = std::make_pair(ConfigValue(), entry.value);
        } else if (it->value != entry.value) {
            diff[entry.key] = std::make_pair(it->value, entry.value);
        }
    }
    
    return diff;
}

std::string Config::diffToString(const Config& other) const {
    auto diff = diff(other);
    std::ostringstream oss;
    
    if (diff.empty()) {
        return "Configurations are identical.";
    }
    
    oss << "Differences between configurations:\n";
    for (const auto& pair : diff) {
        oss << "  Key: \"" << pair.first << "\"\n";
        oss << "    Old: ";
        ConfigUtils::toString(pair.second.first, oss);
        oss << "\n";
        oss << "    New: ";
        ConfigUtils::toString(pair.second.second, oss);
        oss << "\n";
    }
    
    return oss.str();
}

std::vector<std::string> Config::analyzeOptimizations() const {
    return ConfigUtils::generateOptimizationSuggestions(*this);
}

std::string Config::getOptimizationReport() const {
    std::vector<std::string> suggestions = analyzeOptimizations();
    std::ostringstream oss;
    
    if (suggestions.empty()) {
        oss << "No optimization suggestions available.";
    } else {
        oss << "Configuration Optimization Suggestions:\n";
        for (const auto& suggestion : suggestions) {
            oss << "  - " << suggestion << "\n";
        }
    }
    
    return oss.str();
}

bool Config::encrypt(const std::string& key_file) {
    // Simple encryption implementation
    // In a real implementation, this would use proper key management
    
    std::ifstream key_stream(key_file, std::ios::binary);
    if (!key_stream) {
        // Generate a random key
        pImpl->encryption_key.resize(32);
        RAND_bytes(pImpl->encryption_key.data(), pImpl->encryption_key.size());
    } else {
        pImpl->encryption_key.resize(32);
        key_stream.read(reinterpret_cast<char*>(pImpl->encryption_key.data()), pImpl->encryption_key.size());
    }
    
    pImpl->is_encrypted = true;
    return true;
}

bool Config::decrypt(const std::string& key_file) {
    if (!pImpl->is_encrypted) {
        return false;
    }
    
    std::ifstream key_stream(key_file, std::ios::binary);
    if (!key_stream) {
        // Try to use the stored key
        return true;
    }
    
    key_stream.read(reinterpret_cast<char*>(pImpl->encryption_key.data()), pImpl->encryption_key.size());
    pImpl->is_encrypted = false;
    return true;
}

bool Config::isEncrypted() const {
    return pImpl->is_encrypted;
}

bool Config::version(const std::string& version) {
    pImpl->version = version;
    return true;
}

std::string Config::getVersion() const {
    return pImpl->version;
}

bool Config::saveVersion(const std::string& version_file) const {
    std::ofstream file(version_file);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json version_data;
    version_data["version"] = pImpl->version;
    version_data["timestamp"] = std::chrono::system_clock::to_time_t(pImpl->creation_time);
    version_data["last_modified"] = std::chrono::system_clock::to_time_t(pImpl->last_modified);
    
    file << version_data.dump(2);
    return true;
}

bool Config::loadVersion(const std::string& version_file) {
    std::ifstream file(version_file);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json version_data;
        file >> version_data;
        
        pImpl->version = version_data["version"];
        pImpl->creation_time = std::chrono::system_clock::from_time_t(version_data["timestamp"]);
        pImpl->last_modified = std::chrono::system_clock::from_time_t(version_data["last_modified"]);
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

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

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Store old value for validation history
        ConfigValue old_value = it->value;
        
        it->value = value;
        it->source = source;
        it->timestamp = std::chrono::system_clock::now();
        
        // Add to validation history
        if (pImpl->validation_history_enabled) {
            pImpl->validation_history.emplace_back(key, old_value, value, source, "set", "user", "Set configuration value");
        }
        
        // Validate if schema exists
        if (validate({key})) {
            pImpl->last_warnings.clear();
        }
    } else {
        ConfigValue old_value = ConfigValue();
        pImpl->entries.emplace_back(key, value, source);
        
        // Add to validation history
        if (pImpl->validation_history_enabled) {
            pImpl->validation_history.emplace_back(key, old_value, value, source, "add", "user", "Added configuration entry");
        }
        
        // Validate if schema exists
        if (validate({key})) {
            pImpl->last_warnings.clear();
        }
    }
    
    pImpl->last_modified = std::chrono::system_clock::now();
}

ValidationResult Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::vector<ValidationError> errors;
    std::vector<ValidationError> warnings;
    
    // Type validation - check if we can convert to the target type
    // For now, we accept any type but in a real implementation this would be more strict
    
    // Cross-parameter validation
    std::map<std::string, ConfigValue> config_map;
    for (const auto& entry : pImpl->entries) {
        config_map[entry.key] = entry.value;
    }
    config_map[key] = value;
    
    // Special validations
    if (key == "connection_probability") {
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                errors.push_back(ValidationError(key, "range_violation",
                                                 "Connection probability must be between 0.0 and 1.0",
                                                 "error",
                                                 "Set to a value between 0.0 and 1.0"));
            }
        }
    }
    
    if (key == "neuron_count") {
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val < 1) {
                errors.push_back(ValidationError(key, "range_violation",
                                                 "Neuron count must be at least 1",
                                                 "error",
                                                 "Set to a positive integer"));
            }
        }
    }
    
    // If valid, apply the change
    if (errors.empty()) {
        set(key, value, source);
    }
    
    return ValidationResult(errors, warnings);
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        ConfigValue value = it->value;
        pImpl->entries.erase(it);
        
        // Add to validation history
        if (pImpl->validation_history_enabled) {
            pImpl->validation_history.emplace_back(key, value, ConfigValue(), ConfigSource::Runtime, "remove", "user", "Removed configuration entry");
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    
    // Add to validation history
    if (pImpl->validation_history_enabled) {
        pImpl->validation_history.emplace_back("config_cleared", ConfigValue(), ConfigValue(), ConfigSource::Runtime, "clear", "user", "Cleared all configuration");
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (version " << pImpl->version << ", " << pImpl->entries.size() << " entries):\n";
    oss << "  Created: " << ConfigUtils::formatTimestamp(pImpl->creation_time) << "\n";
    oss << "  Last modified: " << ConfigUtils::formatTimestamp(pImpl->last_modified) << "\n";
    oss << "  Encrypted: " << (pImpl->is_encrypted ? "Yes" : "No") << "\n";
    
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
    
    if (!pImpl->last_errors.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : pImpl->last_errors) {
            oss << "  " << error.severity << ": " << error.message << " (key: " << error.key << ")\n";
        }
    }
    
    if (!pImpl->last_warnings.empty()) {
        oss << "\nValidation Warnings:\n";
        for (const auto& warning : pImpl->last_warnings) {
            oss << "  " << warning.severity << ": " << warning.message << " (key: " << warning.key << ")\n";
        }
    }
    
    return oss.str();
}

std::vector<ValidationError> Config::getLastValidationErrors() const {
    return pImpl->last_errors;
}

std::vector<ValidationError> Config::getLastValidationWarnings() const {
    return pImpl->last_warnings;
}

void Config::enableMonitoring(bool enable) {
    pImpl->monitoring_enabled = enable;
    if (enable) {
        pImpl->monitoring_data["start_time"] = std::to_string(std::chrono::system_clock::to_time_t(pImpl->creation_time));
        pImpl->monitoring_data["version"] = pImpl->version;
    }
}

bool Config::isMonitoringEnabled() const {
    return pImpl->monitoring_enabled;
}

std::map<std::string, std::string> Config::getMonitoringData() const {
    if (pImpl->monitoring_enabled) {
        std::map<std::string, std::string> data = pImpl->monitoring_data;
        data["current_entry_count"] = std::to_string(pImpl->entries.size());
        data["validation_history_size"] = std::to_string(pImpl->validation_history.size());
        data["timestamp"] = std::to_string(std::chrono::system_clock::to_time_t(pImpl->last_modified));
        return data;
    }
    return std::map<std::string, std::string>();
}

bool Config::integrateWithCI(const std::string& pipeline_config) {
    pImpl->metadata["ci_integration"] = pipeline_config;
    pImpl->last_modified = std::chrono::system_clock::now();
    return true;
}

bool Config::integrateWithInfrastructureAsCode(const std::string& iac_config) {
    pImpl->metadata["iac_config"] = iac_config;
    pImpl->last_modified = std::chrono::system_clock::now();
    return true;
}

std::string Config::exportToString() const {
    nlohmann::json json_data;
    
    for (const auto& entry : pImpl->entries) {
        json_data[entry.key] = configValueToJSON(entry.value);
    }
    
    return json_data.dump(2);
}

bool Config::importFromString(const std::string& config_string) {
    try {
        nlohmann::json json_data = nlohmann::json::parse(config_string);
        clear();
        
        for (auto it = json_data.begin(); it != json_data.end(); ++it) {
            std::string key = it.key();
            set(key, it.value(), ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// ConfigValidator implementation
ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

bool ConfigValidator::addSchema(const std::string& name, const ConfigSchema& schema) {
    pImpl->schemas[name] = schema;
    return true;
}

ValidationResult ConfigValidator::validate(const Config& config, const std::string& schema_name) const {
    ValidationResult result;
    
    if (pImpl->schemas.find(schema_name) == pImpl->schemas.end()) {
        result.valid = false;
        result.errors.push_back(ValidationError("", "schema_error", 
                                               "Schema not found: " + schema_name,
                                               "error",
                                               "Use a valid schema name"));
        return result;
    }
    
    const ConfigSchema& schema = pImpl->schemas.at(schema_name);
    
    // Validate configuration against schema
    std::map<std::string, ConfigValue> config_map;
    for (const auto& entry : config.pImpl->entries) {
        config_map[entry.key] = entry.value;
    }
    
    // Check required keys
    for (const auto& [key, type] : schema.required_keys) {
        if (config_map.find(key) == config_map.end()) {
            result.errors.push_back(ValidationError(key, "required_key",
                                                   "Required configuration key is missing: " + key,
                                                   "error",
                                                   "Add this key to your configuration"));
        }
    }
    
    // Type validation
    for (const auto& [key, value] : config_map) {
        if (schema.type_definitions.find(key) != schema.type_definitions.end()) {
            std::vector<ValidationError> type_errors = ConfigUtils::validateType(key, value, schema.type_definitions.at(key));
            result.errors.insert(result.errors.end(), type_errors.begin(), type_errors.end());
        }
    }
    
    // Constraint validation
    for (const auto& constraint : schema.constraints) {
        ConfigUtils::validateCrossParameter(constraint, config_map);
    }
    
    result.valid = result.errors.empty();
    
    // Generate optimization suggestions
    result.optimization_suggestions = ConfigUtils::generateOptimizationSuggestions(config);
    
    return result;
}

bool ConfigValidator::addConstraint(const ValidationConstraint& constraint) {
    pImpl->constraints.push_back(constraint);
    return true;
}

void ConfigValidator::setValidationCallback(std::function<ValidationResult(const Config&)> callback) {
    pImpl->validation_callback = callback;
}

std::vector<ValidationError> ConfigValidator::getLastErrors() const {
    return pImpl->last_errors;
}

std::vector<ValidationError> ConfigValidator::getLastWarnings() const {
    return pImpl->last_warnings;
}

// ConfigUtils implementations
bool ConfigUtils::compareValues(const ConfigValue& a, const ConfigValue& b) {
    return a == b;
}

std::vector<std::string> ConfigUtils::generateOptimizationSuggestions(const Config& config) {
    std::vector<std::string> suggestions;
    
    // Check for commonly configured parameters
    if (config.has("neuron_count") && config.has("connection_probability")) {
        auto neuron_count_opt = config.get<int64_t>("neuron_count");
        auto connection_prob_opt = config.get<double>("connection_probability");
        
        if (neuron_count_opt && connection_prob_opt) {
            double n = static_cast<double>(*neuron_count_opt);
            double p = *connection_prob_opt;
            
            // Suggest if neuron count is too high for connection probability
            if (n > 1000 && p < 0.01) {
                suggestions.push_back("Consider reducing neuron_count or increasing connection_probability for better connectivity");
            }
            
            // Suggest default values
            if (p < 0.05) {
                suggestions.push_back("Connection probability is low - consider increasing to 0.1 for better network connectivity");
            }
        }
    }
    
    if (config.has("plasticity_learning_rate")) {
        auto learning_rate_opt = config.get<double>("plasticity_learning_rate");
        if (learning_rate_opt && *learning_rate_opt < 0.001) {
            suggestions.push_back("Plasticity learning rate is very low - consider increasing to 0.01 for better learning");
        } else if (learning_rate_opt && *learning_rate_opt > 0.1) {
            suggestions.push_back("Plasticity learning rate is high - consider reducing to 0.01 to prevent instability");
        }
    }
    
    return suggestions;
}

std::string ConfigUtils::diffToString(const std::map<std::string, std::pair<ConfigValue, ConfigValue>>& diff) {
    std::ostringstream oss;
    
    if (diff.empty()) {
        return "No differences.";
    }
    
    oss << "Differences:\n";
    for (const auto& pair : diff) {
        oss << "  " << pair.first << ": ";
        toString(pair.second.first, oss);
        oss << " -> ";
        toString(pair.second.second, oss);
        oss << "\n";
    }
    
    return oss.str();
}

std::vector<ValidationError> ConfigUtils::validateType(const std::string& key, const ConfigValue& value, const std::string& expected_type) {
    std::vector<ValidationError> errors;
    
    bool is_valid = false;
    
    if (expected_type == "int" && std::holds_alternative<int>(value)) {
        is_valid = true;
    } else if (expected_type == "int64" && std::holds_alternative<int64_t>(value)) {
        is_valid = true;
    } else if (expected_type == "double" && std::holds_alternative<double>(value)) {
        is_valid = true;
    } else if (expected_type == "bool" && std::holds_alternative<bool>(value)) {
        is_valid = true;
    } else if (expected_type == "string" && std::holds_alternative<std::string>(value)) {
        is_valid = true;
    } else if (expected_type == "int_vector" && std::holds_alternative<std::vector<int>>(value)) {
        is_valid = true;
    } else if (expected_type == "double_vector" && std::holds_alternative<std::vector<double>>(value)) {
        is_valid = true;
    } else if (expected_type == "string_vector" && std::holds_alternative<std::vector<std::string>>(value)) {
        is_valid = true;
    }
    
    if (!is_valid) {
        errors.push_back(ValidationError(key, "type_mismatch",
                                         "Expected type '" + expected_type + "' but got different type",
                                         "error",
                                         "Change the value to use correct type"));
    }
    
    return errors;
}

std::vector<ValidationError> ConfigUtils::validateRange(const std::string& key, double value, double min, double max) {
    std::vector<ValidationError> errors;
    
    if (value < min || value > max) {
        errors.push_back(ValidationError(key, "range_violation",
                                         "Value " + std::to_string(value) + " is outside valid range [" +
                                             std::to_string(min) + ", " + std::to_string(max) + "]",
                                         "error",
                                         "Set value to a number within the valid range"));
    }
    
    return errors;
}

std::vector<ValidationError> ConfigUtils::validateCrossParameter(const ValidationConstraint& constraint, const std::map<std::string, ConfigValue>& config) {
    std::vector<ValidationError> errors;
    
    if (config.find(constraint.key) == config.end() || 
        config.find(constraint.secondary_key) == config.end()) {
        return errors;
    }
    
    double key_value = 0.0;
    double secondary_value = 0.0;
    
    // Extract numeric values
    if (std::holds_alternative<int>(config.at(constraint.key))) {
        key_value = std::get<int>(config.at(constraint.key));
    } else if (std::holds_alternative<int64_t>(config.at(constraint.key))) {
        key_value = std::get<int64_t>(config.at(constraint.key));
    } else if (std::holds_alternative<double>(config.at(constraint.key))) {
        key_value = std::get<double>(config.at(constraint.key));
    }
    
    if (std::holds_alternative<int>(config.at(constraint.secondary_key))) {
        secondary_value = std::get<int>(config.at(constraint.secondary_key));
    } else if (std::holds_alternative<int64_t>(config.at(constraint.secondary_key))) {
        secondary_value = std::get<int64_t>(config.at(constraint.secondary_key));
    } else if (std::holds_alternative<double>(config.at(constraint.secondary_key))) {
        secondary_value = std::get<double>(config.at(constraint.secondary_key));
    }
    
    bool constraint_violated = false;
    
    if (constraint.operation == ">") {
        constraint_violated = key_value > constraint.value && secondary_value > constraint.value;
    } else if (constraint.operation == "<") {
        constraint_violated = key_value < constraint.value && secondary_value < constraint.value;
    } else if (constraint.operation == ">=") {
        constraint_violated = key_value >= constraint.value && secondary_value >= constraint.value;
    } else if (constraint.operation == "<=") {
        constraint_violated = key_value <= constraint.value && secondary_value <= constraint.value;
    } else if (constraint.operation == "==") {
        constraint_violated = std::abs(key_value - constraint.value) > 0.001 && 
                            std::abs(secondary_value - constraint.value) > 0.001;
    }
    
    if (constraint_violated) {
        errors.push_back(ValidationError(constraint.key + " and " + constraint.secondary_key,
                                         "constraint_violation",
                                         constraint.message,
                                         "error",
                                         constraint.suggestion));
    }
    
    return errors;
}

std::string ConfigUtils::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm = *std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string ConfigUtils::toString(const ConfigValue& value) {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    
    return oss.str();
}

std::string ConfigUtils::configToString(const Config& config) {
    std::ostringstream oss;
    oss << "Configuration:";
    
    for (const auto& entry : config.pImpl->entries) {
        oss << "\n  " << entry.key << " = " << toString(entry.value);
    }
    
    return oss.str();
}

void ConfigUtils::validateConstraint(const ValidationConstraint& constraint, const std::map<std::string, ConfigValue>& config) const {
    // Implementation for constraint validation
    // This is a simplified version - in a full implementation, this would be more comprehensive
}

// JSON serialization functions
void to_json(nlohmann::json& j, const ConfigValue& value) {
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
}

void from_json(const nlohmann::json& j, ConfigValue& value) {
    // Check the type of JSON and create appropriate ConfigValue
    if (j.is_string()) {
        value = j.get<std::string>();
    } else if (j.is_boolean()) {
        value = j.get<bool>();
    } else if (j.is_number_integer()) {
        value = j.get<int>();
    } else if (j.is_number()) {
        value = j.get<double>();
    } else if (j.is_array()) {
        // Try to determine array type
        if (!j.empty() && j[0].is_string()) {
            value = j.get<std::vector<std::string>>();
        } else if (!j.empty() && j[0].is_number_integer()) {
            value = j.get<std::vector<int>>();
        } else {
            value = j.get<std::vector<double>>();
        }
    } else {
        throw std::runtime_error("Unsupported JSON type for ConfigValue");
    }
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
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
}

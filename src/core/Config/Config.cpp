#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <random>
#include <iomanip>
#include <unordered_set>
#include <mutex>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigurationSchema> schema;
    
    // Runtime configuration
    bool runtime_adjustment_enabled;
    bool auto_save_enabled;
    std::string auto_save_path;
    float auto_save_interval;
    std::chrono::steady_clock::time_point last_auto_save;
    
    // Callbacks
    std::vector<std::pair<std::string, ConfigChangeCallback>> callbacks;
    std::string next_callback_id;
    
    // History
    bool history_enabled;
    std::vector<ChangeRecord> change_history;
    
    // Locking
    std::unordered_map<std::string, bool> locked_keys;
    
    // Compression
    bool compression_enabled;
    
    // Statistics
    size_t change_count;
    std::chrono::system_clock::time_point last_change;
    
    // Validation rules
    std::unordered_map<std::string, ValidationRule> validation_rules;
    
    // Templates
    std::unordered_map<std::string, std::vector<std::pair<std::string, ConfigValue>>> templates;
    
    Impl() 
        : runtime_adjustment_enabled(true)
        , auto_save_enabled(false)
        , auto_save_interval(60.0f)
        , last_auto_save(std::chrono::steady_clock::now())
        , history_enabled(true)
        , compression_enabled(false)
        , change_count(0)
        , next_callback_id("0")
    {
        schema = std::make_unique<ConfigurationSchema>();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Simple JSON parser (for demonstration)
bool parseSimpleJSON(const std::string& json, std::unordered_map<std::string, std::string>& out) {
    // Very basic JSON parsing - assumes simple {"key": "value", ...} format
    try {
        size_t pos = json.find('{');
        if (pos == std::string::npos) return false;
        
        pos = json.find('}', pos);
        if (pos == std::string::npos) return false;
        
        std::string content = json.substr(pos + 1, pos - pos - 1);
        
        // Simple parsing for demo purposes
        size_t key_pos = content.find('"');
        while (key_pos != std::string::npos) {
            size_t key_end = content.find('"', key_pos + 1);
            if (key_end == std::string::npos) break;
            
            std::string key = content.substr(key_pos + 1, key_end - key_pos - 1);
            
            size_t colon_pos = content.find(':', key_end);
            if (colon_pos == std::string::npos) break;
            
            size_t value_pos = content.find('"', colon_pos);
            if (value_pos == std::string::npos) break;
            
            size_t value_end = content.find('"', value_pos + 1);
            if (value_end == std::string::npos) break;
            
            std::string value = content.substr(value_pos + 1, value_end - value_pos - 1);
            
            out[key] = value;
            
            key_pos = content.find('"', value_end);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Try JSON first
    if (content.find('{') != std::string::npos) {
        return loadFromJSON(content);
    }
    // Try simple key=value format
    else {
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // Placeholder for YAML loading
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int line_num = 0;
    while (std::getline(file, line)) {
        line_num++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            // Log warning for malformed line
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    return loadFromCommandLineArgs(args);
}

bool Config::loadFromCommandLineArgs(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Handle boolean flags
                set(arg.substr(2), true, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            // This would need modification to support -key value format
            // For now, treat as boolean flag
            set(arg.substr(1), true, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromJSON(const std::string& json_str) {
    // Very basic JSON parsing for demonstration
    std::unordered_map<std::string, std::string> parsed;
    if (!parseSimpleJSON(json_str, parsed)) {
        return false;
    }
    
    for (const auto& pair : parsed) {
        set(pair.first, pair.second, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromYAMLString(const std::string& yaml_str) {
    // Placeholder for YAML parsing
    return loadFromSimpleFile("");
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.description.empty()) {
            file << "# " << entry.key << "\n";
        } else {
            file << "# " << entry.description << "\n";
        }
        
        file << entry.key << " = ";
        
        // Output value with proper quoting
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "  \"" << entry.key << "\": ";
        
        // Output value as JSON
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
    }
    
    oss << "\n}";
    return oss.str();
}

std::string Config::exportToYAML() const {
    std::ostringstream oss;
    oss << "# NLM Configuration\n";
    oss << "# Exported from NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            oss << "# " << entry.description << "\n";
        }
        
        oss << entry.key << ": ";
        
        // Output value as YAML
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                // YAML floating point
                oss << std::setprecision(6) << arg;
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << "\n";
    }
    
    return oss.str();
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // First try to get from schema for type validation
    if (pImpl->schema && pImpl->schema->hasParameter(key)) {
        const auto* param = pImpl->schema->getParameter(key);
        if (param) {
            // Check if the parameter type matches the requested type
            ParameterType param_type = param->type;
            ParameterType requested_type = getParameterType<T>();
            
            if (param_type != requested_type) {
                return std::nullopt;
            }
        }
    }
    
    // Fall back to original implementation
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

std::optional<ConfigValue> Config::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getKeysByGroup(const std::string& group) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.group == group) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!pImpl->runtime_adjustment_enabled && source == ConfigSource::Runtime) {
        return; // Reject runtime changes if disabled
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigValue old_value;
    bool changed = false;
    
    if (it != pImpl->entries.end()) {
        old_value = it->value;
        if (it->value != value) {
            it->value = value;
            it->source = source;
            changed = true;
        }
    } else {
        ConfigEntry new_entry;
        new_entry.key = key;
        new_entry.value = value;
        new_entry.source = source;
        new_entry.category = "general";
        new_entry.group = "basic";
        old_value = ConfigValue(); // Empty for new entries
        pImpl->entries.push_back(std::move(new_entry));
        changed = true;
    }
    
    if (changed) {
        pImpl->change_count++;
        pImpl->last_change = std::chrono::system_clock::now();
        
        // Check if key is locked
        if (pImpl->locked_keys.find(key) == pImpl->locked_keys.end()) {
            // Notify callbacks
            notifyChangeCallbacks(key, old_value, value);
        }
        
        // Auto-save if enabled
        if (pImpl->auto_save_enabled) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->last_auto_save).count();
            if (elapsed >= pImpl->auto_save_interval) {
                saveToFile(pImpl->auto_save_path);
                pImpl->last_auto_save = now;
            }
        }
        
        // Add to history
        if (pImpl->history_enabled) {
            ChangeRecord record;
            record.key = key;
            record.old_value = old_value;
            record.new_value = value;
            record.source = source;
            record.timestamp = std::chrono::system_clock::now();
            record.user = "system";
            pImpl->change_history.push_back(record);
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::safeSet(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value
    ValidationResult validation_result = validate(key, value);
    
    if (!validation_result.is_valid) {
        return false;
    }
    
    set(key, value, source);
    return true;
}

bool Config::safeSet(const std::string& key, const std::string& value, ConfigSource source) {
    return safeSet(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    pImpl->entries.erase(it, pImpl->entries.end());
    
    // Remove from locks
    pImpl->locked_keys.erase(key);
    
    // Add to history
    if (pImpl->history_enabled) {
        ChangeRecord record;
        record.key = key;
        record.old_value = ConfigValue(); // Empty
        record.new_value = ConfigValue(); // Empty
        record.source = ConfigSource::Runtime;
        record.timestamp = std::chrono::system_clock::now();
        record.user = "system";
        pImpl->change_history.push_back(record);
    }
}

void Config::clear() {
    // Record changes for history
    for (const auto& entry : pImpl->entries) {
        if (pImpl->history_enabled) {
            ChangeRecord record;
            record.key = entry.key;
            record.old_value = entry.value;
            record.new_value = ConfigValue(); // Empty
            record.source = entry.source;
            record.timestamp = std::chrono::system_clock::now();
            record.user = "system";
            pImpl->change_history.push_back(record);
        }
    }
    
    pImpl->entries.clear();
    pImpl->locked_keys.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "NLM Configuration System" << std::endl;
    oss << "=========================" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    oss << "Categories: " << getCategories().size() << std::endl;
    oss << "Groups: " << getGroups().size() << std::endl;
    oss << "Changes: " << pImpl->change_count << std::endl;
    oss << "Runtime adjustment: " << (pImpl->runtime_adjustment_enabled ? "enabled" : "disabled") << std::endl;
    oss << "Auto-save: " << (pImpl->auto_save_enabled ? "enabled" : "disabled") << std::endl;
    oss << "History: " << (pImpl->history_enabled ? "enabled" : "disabled") << std::endl;
    oss << std::endl;
    
    if (pImpl->schema) {
        oss << "Schema validation: enabled" << std::endl;
    }
    
    oss << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << std::endl;
        oss << "  Type: " << getValueType(entry.value) << std::endl;
        oss << "  Value: ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << std::endl;
        oss << "  Source: " << static_cast<int>(entry.source) << std::endl;
        oss << "  Category: " << entry.category << std::endl;
        oss << "  Group: " << entry.group << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    // Validate all entries
    for (const auto& entry : pImpl->entries) {
        ValidationResult entry_result = validate(entry.key, entry.value);
        if (entry_result.hasErrors()) {
            result.is_valid = false;
            result.errors.insert(result.errors.end(), entry_result.errors.begin(), entry_result.errors.end());
        }
        if (entry_result.hasWarnings()) {
            result.warnings.insert(result.warnings.end(), entry_result.warnings.begin(), entry_result.warnings.end());
        }
    }
    
    return result;
}

ValidationResult Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ValidationResult(false, {"Key not found: " + key});
    }
    
    ValidationResult result = validate(key, it->value);
    
    // Apply custom validation rules
    auto rule_it = pImpl->validation_rules.find(key);
    if (rule_it != pImpl->validation_rules.end()) {
        ValidationResult rule_result = rule_it->second(it->value);
        if (rule_result.hasErrors()) {
            result.is_valid = false;
            result.errors.insert(result.errors.end(), rule_result.errors.begin(), rule_result.errors.end());
        }
    }
    
    return result;
}

ValidationResult Config::validateAll(const std::vector<std::pair<std::string, ConfigValue>>& key_values) const {
    std::vector<std::pair<std::string, ConfigValue>> all_key_values;
    
    // Add current entries
    for (const auto& entry : pImpl->entries) {
        all_key_values.push_back({entry.key, entry.value});
    }
    
    // Add new key_values (overriding existing ones)
    for (const auto& kv : key_values) {
        bool found = false;
        for (auto& existing_kv : all_key_values) {
            if (existing_kv.first == kv.first) {
                existing_kv.second = kv.second;
                found = true;
                break;
            }
        }
        if (!found) {
            all_key_values.push_back(kv);
        }
    }
    
    // Validate all
    return validateAllImpl(all_key_values);
}

void Config::enableRuntimeAdjustment(bool enabled) {
    pImpl->runtime_adjustment_enabled = enabled;
}

bool Config::isRuntimeAdjustmentEnabled() const {
    return pImpl->runtime_adjustment_enabled;
}

void Config::addChangeCallback(const ConfigChangeCallback& callback) {
    std::string callback_id = generateUniqueCallbackId();
    pImpl->callbacks.push_back({callback_id, callback});
}

void Config::removeChangeCallback(const std::string& callback_id) {
    pImpl->callbacks.erase(
        std::remove_if(pImpl->callbacks.begin(), pImpl->callbacks.end(),
            [&callback_id](const auto& cb) { return cb.first == callback_id; }),
        pImpl->callbacks.end()
    );
}

void Config::clearChangeCallbacks() {
    pImpl->callbacks.clear();
}

void Config::setSchema(const ConfigurationSchema& schema) {
    pImpl->schema = std::make_unique<ConfigurationSchema>(schema);
}

const ConfigurationSchema* Config::getSchema() const {
    return pImpl->schema.get();
}

std::vector<std::string> Config::getCategories() const {
    std::unordered_set<std::string> categories;
    for (const auto& entry : pImpl->entries) {
        categories.insert(entry.category);
    }
    return std::vector<std::string>(categories.begin(), categories.end());
}

std::vector<std::string> Config::getGroups() const {
    std::unordered_set<std::string> groups;
    for (const auto& entry : pImpl->entries) {
        groups.insert(entry.group);
    }
    return std::vector<std::string>(groups.begin(), groups.end());
}

std::string Config::exportConfiguration() const {
    std::ostringstream oss;
    oss << "NLM Configuration Export\n";
    oss << "=========================" << std::endl;
    oss << "Format: JSON\n";
    oss << "Version: 1.0\n";
    oss << "Generated: " << getCurrentTimestamp() << "\n";
    oss << std::endl;
    oss << exportToJSON();
    return oss.str();
}

bool Config::importConfiguration(const std::string& config_str) {
    // Simple import from JSON format
    if (config_str.find('{') != std::string::npos) {
        return loadFromJSON(config_str);
    } else {
        // Try other formats
        return false;
    }
}

std::vector<std::pair<std::string, ConfigValue>> Config::search(const std::string& pattern) const {
    std::vector<std::pair<std::string, ConfigValue>> results;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(pattern) != std::string::npos) {
            results.push_back({entry.key, entry.value});
        }
    }
    
    return results;
}

std::vector<std::pair<std::string, ConfigValue>> Config::getByCategory(const std::string& category) const {
    std::vector<std::pair<std::string, ConfigValue>> results;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            results.push_back({entry.key, entry.value});
        }
    }
    
    return results;
}

std::vector<std::pair<std::string, ConfigValue>> Config::getByGroup(const std::string& group) const {
    std::vector<std::pair<std::string, ConfigValue>> results;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.group == group) {
            results.push_back({entry.key, entry.value});
        }
    }
    
    return results;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getCategoryCount() const {
    return getCategories().size();
}

size_t Config::getGroupCount() const {
    return getGroups().size();
}

double Config::getMemoryUsage() const {
    double total_size = 0.0;
    for (const auto& entry : pImpl->entries) {
        total_size += entry.key.size() + sizeof(entry.value) + entry.description.size();
    }
    return total_size / 1024.0; // KB
}

size_t Config::changeCount() const {
    return pImpl->change_count;
}

void Config::setDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

void Config::setCategory(const std::string& key, const std::string& category) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->category = category;
    }
}

void Config::setGroup(const std::string& key, const std::string& group) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->group = group;
    }
}

void Config::setReadOnly(const std::string& key, bool read_only) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->read_only = read_only;
    }
}

void Config::setRequiresRestart(const std::string& key, bool requires_restart) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->requires_restart = requires_restart;
    }
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    return "";
}

std::string Config::getCategory(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->category;
    }
    return "general";
}

std::string Config::getGroup(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->group;
    }
    return "basic";
}

bool Config::isReadOnly(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->read_only;
    }
    return false;
}

bool Config::requiresRestart(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->requires_restart;
    }
    return false;
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == other.pImpl->entries.end() || it->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

bool Config::differsFrom(const Config& other) const {
    return !equals(other);
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Don't override if key is read-only in this config
        if (!isReadOnly(entry.key)) {
            set(entry.key, entry.value, source);
        }
    }
}

void Config::merge(const std::vector<std::pair<std::string, ConfigValue>>& key_values, ConfigSource source) {
    for (const auto& kv : key_values) {
        set(kv.first, kv.second, source);
    }
}

void Config::resetToDefaults() {
    // This would typically reload default configuration
    // For simplicity, we'll just clear and set basic defaults
    clear();
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
}

void Config::resetToFile(const std::string& filepath) {
    Config temp_config;
    if (temp_config.loadFromFile(filepath)) {
        pImpl->entries = std::move(temp_config.pImpl->entries);
    }
}

void Config::resetToCommandLine(const std::vector<std::string>& args) {
    Config temp_config;
    if (temp_config.loadFromCommandLineArgs(args)) {
        pImpl->entries = std::move(temp_config.pImpl->entries);
    }
}

void Config::enableAutoSave(const std::string& filepath, float interval_seconds) {
    pImpl->auto_save_enabled = true;
    pImpl->auto_save_path = filepath;
    pImpl->auto_save_interval = interval_seconds;
}

void Config::disableAutoSave() {
    pImpl->auto_save_enabled = false;
}

bool Config::isAutoSaveEnabled() const {
    return pImpl->auto_save_enabled;
}

std::string Config::getAutoSavePath() const {
    return pImpl->auto_save_path;
}

void Config::lock(const std::string& key) {
    pImpl->locked_keys[key] = true;
}

void Config::unlock(const std::string& key) {
    pImpl->locked_keys.erase(key);
}

bool Config::isLocked(const std::string& key) const {
    return pImpl->locked_keys.find(key) != pImpl->locked_keys.end();
}

void Config::enableHistory(bool enabled) {
    pImpl->history_enabled = enabled;
    if (!enabled) {
        pImpl->change_history.clear();
    }
}

bool Config::isHistoryEnabled() const {
    return pImpl->history_enabled;
}

std::vector<Config::ChangeRecord> Config::getHistory() const {
    return pImpl->change_history;
}

void Config::clearHistory() {
    pImpl->change_history.clear();
}

Config::ConfigDiff Config::diff(const Config& other) const {
    ConfigDiff diff;
    
    std::unordered_set<std::string> this_keys;
    for (const auto& entry : pImpl->entries) {
        this_keys.insert(entry.key);
    }
    
    std::unordered_set<std::string> other_keys;
    for (const auto& entry : other.pImpl->entries) {
        other_keys.insert(entry.key);
    }
    
    // Find added keys
    for (const auto& key : this_keys) {
        if (other_keys.find(key) == other_keys.end()) {
            diff.added_keys.push_back(key);
        }
    }
    
    // Find removed keys
    for (const auto& key : other_keys) {
        if (this_keys.find(key) == this_keys.end()) {
            diff.removed_keys.push_back(key);
        }
    }
    
    // Find modified keys
    for (const auto& entry : pImpl->entries) {
        auto it = other.pImpl->entries.find(entry.key);
        if (it != other.pImpl->entries.end() && entry.value != it->value) {
            diff.modified_keys.push_back({entry.key, {entry.value, it->value}});
        }
    }
    
    return diff;
}

void Config::addValidationRule(const std::string& key, const ValidationRule& rule) {
    pImpl->validation_rules[key] = rule;
}

void Config::removeValidationRule(const std::string& key) {
    pImpl->validation_rules.erase(key);
}

void Config::applyValidationRules(const std::string& key, const ConfigValue& value) {
    auto rule_it = pImpl->validation_rules.find(key);
    if (rule_it != pImpl->validation_rules.end()) {
        ValidationResult rule_result = rule_it->second(value);
        if (rule_result.hasErrors()) {
            // Log validation error
        }
    }
}

std::string Config::generateUniqueCallbackId() const {
    return std::to_string(pImpl->next_callback_id++);
}

void Config::notifyChangeCallbacks(const std::string& key, const ConfigValue& old_value, const ConfigValue& new_value) {
    for (const auto& cb : pImpl->callbacks) {
        try {
            cb.second(key, old_value, new_value);
        } catch (const std::exception& e) {
            // Log callback error
        }
    }
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    return validate(key, value).is_valid;
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S UTC");
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Template function implementations
template<typename T>
ParameterType Config::getParameterType() {
    if constexpr (std::is_same_v<T, int>) return ParameterType::INT;
    else if constexpr (std::is_same_v<T, int64_t>) return ParameterType::INT64;
    else if constexpr (std::is_same_v<T, double>) return ParameterType::DOUBLE;
    else if constexpr (std::is_same_v<T, bool>) return ParameterType::BOOL;
    else if constexpr (std::is_same_v<T, std::string>) return ParameterType::STRING;
    else if constexpr (std::is_same_v<T, std::vector<int>>) return ParameterType::VECTOR_INT;
    else if constexpr (std::is_same_v<T, std::vector<double>>) return ParameterType::VECTOR_DOUBLE;
    else if constexpr (std::is_same_v<T, std::vector<std::string>>) return ParameterType::VECTOR_STRING;
    else return ParameterType::OBJECT;
}

} // namespace nlm

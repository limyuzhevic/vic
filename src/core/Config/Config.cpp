#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), history() {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool Config::loadFromFile(const std::string& filepath) {
    ConfigErrors errors;
    return loadFromFileWithRecovery(filepath, errors);
}

bool Config::loadFromFileWithRecovery(const std::string& filepath, ConfigErrors& errors) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        errors.errors.push_back("Cannot open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        // Skip empty lines and comments
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
            
            // Try to set the value, capture any errors
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                errors.errors.push_back("Line " + std::to_string(lineNumber) + ": " + e.what() + " (key: '" + key + "')");
            }
        } else {
            errors.warnings.push_back("Line " + std::to_string(lineNumber) + ": Invalid format (missing '=')");
        }
    }
    
    if (errors.hasErrors()) {
        errors.warnings.push_back("Configuration loaded with errors. Check errors list for details.");
    } else {
        addToHistory("Loaded configuration from " + filepath);
    }
    
    return true;
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
    
    if (argc > 1) {
        addToHistory("Loaded configuration from command line arguments");
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
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
}

bool Config::validate(ConfigErrors& errors, const ConfigSchema& schema) const {
    bool valid = true;
    
    // Validate schema if provided
    if (!schema.filename.empty()) {
        ConfigErrors schemaErrors;
        if (!schema.validate(schemaErrors)) {
            errors.errors.insert(errors.errors.end(), schemaErrors.errors.begin(), schemaErrors.errors.end());
            valid = false;
        }
    }
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        // Check if key is defined in schema
        if (!schema.filename.empty()) {
            bool keyFound = false;
            for (const auto& rule : schema.rules) {
                if (rule.key == entry.key) {
                    keyFound = true;
                    
                    // Validate type
                    if (!validateValue(entry, errors)) {
                        valid = false;
                    }
                    break;
                }
            }
            
            if (!keyFound && schema.requiredSections.empty()) {
                errors.warnings.push_back("Key '" + entry.key + "' is not defined in schema");
            }
        }
    }
    
    return valid;
}

template<typename T>
bool Config::validateValue(const ConfigEntry& entry, ConfigErrors& errors) const {
    // This is a simplified implementation - in a real system, you'd want
    // more sophisticated type checking and validation rules
    
    try {
        auto value = std::get<T>(entry.value);
        
        // Basic validation based on type
        if constexpr (std::is_same_v<T, std::string>) {
            if (value.empty()) {
                errors.warnings.push_back("String key '" + entry.key + "' is empty");
            }
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            if (value < 0) {
                errors.warnings.push_back("Integer key '" + entry.key + "' has negative value: " + std::to_string(value));
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (value < 0.0) {
                errors.warnings.push_back("Double key '" + entry.key + "' has negative value: " + std::to_string(value));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // No validation for boolean values
        }
        
        return true;
        
    } catch (const std::bad_variant_access&) {
        errors.errors.push_back("Key '" + entry.key + "' has value of wrong type");
        return false;
    }
}

void Config::reportError(ConfigErrors& errors, const std::string& error) const {
    errors.errors.push_back(error);
}

void Config::reportWarning(ConfigErrors& errors, const std::string& warning) const {
    errors.warnings.push_back(warning);
}

std::vector<Config::ValidationError> Config::getValidationErrors(const std::string& key) const {
    std::vector<ValidationError> errors;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        // Add validation errors for this key
        // In a real implementation, you'd have validation rules defined elsewhere
        if (it->source == ConfigSource::Default) {
            errors.emplace_back(key, "Value is from default source");
        }
        
        // Check for potentially problematic values
        try {
            if (std::holds_alternative<double>(it->value)) {
                double val = std::get<double>(it->value);
                if (val < 0.0) {
                    errors.emplace_back(key, "Negative value: " + std::to_string(val));
                }
            }
        } catch (...) {
            errors.emplace_back(key, "Invalid value type");
        }
    }
    
    return errors;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            // Update existing entry
            it->value = entry.value;
            it->source = source; // Override with new source
        } else {
            // Add new entry
            ConfigEntry newEntry = entry;
            newEntry.source = source;
            pImpl->entries.push_back(newEntry);
        }
    }
}

Config::ConfigSnapshot Config::createSnapshot() const {
    ConfigSnapshot snapshot;
    snapshot.entries = pImpl->entries;
    snapshot.timestamp = getCurrentTimestamp();
    addToHistory("Created configuration snapshot");
    return snapshot;
}

bool Config::restoreSnapshot(const ConfigSnapshot& snapshot) {
    // Validate the snapshot
    if (snapshot.entries.empty()) {
        reportError(pImpl->errors, "Cannot restore empty snapshot");
        return false;
    }
    
    // Clear current configuration
    clear();
    
    // Restore entries from snapshot
    for (const auto& entry : snapshot.entries) {
        try {
            // Use a different method to set value without validation errors
            if (std::holds_alternative<int>(entry.value)) {
                set(entry.key, std::get<int>(entry.value), entry.source);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                set(entry.key, std::get<int64_t>(entry.value), entry.source);
            } else if (std::holds_alternative<double>(entry.value)) {
                set(entry.key, std::get<double>(entry.value), entry.source);
            } else if (std::holds_alternative<bool>(entry.value)) {
                set(entry.key, std::get<bool>(entry.value), entry.source);
            } else if (std::holds_alternative<std::string>(entry.value)) {
                set(entry.key, std::get<std::string>(entry.value), entry.source);
            }
        } catch (...) {
            reportError(pImpl->errors, "Failed to restore entry: " + entry.key);
        }
    }
    
    addToHistory("Restored configuration from snapshot");
    return true;
}

bool Config::operator==(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    // Count entries by source
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default:
                stats.entriesBySource[0]++;
                break;
            case ConfigSource::File:
                stats.entriesBySource[1]++;
                break;
            case ConfigSource::CommandLine:
                stats.entriesBySource[2]++;
                break;
            case ConfigSource::Runtime:
                stats.entriesBySource[3]++;
                break;
        }
    }
    
    // For simplicity, we're not tracking timestamps
    // In a real implementation, you'd have proper timestamp tracking
    return stats;
}

void Config::addToHistory(const std::string& action) {
    history.push_back("[".getCurrentTimestamp() + "] " + action);
    // Keep history limited to prevent memory issues
    if (history.size() > 100) {
        history.erase(history.begin());
    }
}

std::vector<std::string> Config::getHistory() const {
    return history;
}

void Config::clearHistory() {
    history.clear();
}

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
    
    if (!history.empty()) {
        oss << "\nRecent history:";
        size_t start = std::max(size_t(0), history.size() - 10);
        for (size_t i = start; i < history.size(); ++i) {
            oss << "\n  " << history[i];
        }
    }
    
    return oss.str();
}

void Config::showConfigWithErrors(std::ostream& out, const ConfigErrors& errors) const {
    out << summary() << "\n";
    
    if (errors.hasErrors()) {
        out << "\n=== ERRORS ===\n";
        for (const auto& error : errors.errors) {
            out << "ERROR: " << error << "\n";
        }
    }
    
    if (errors.hasWarnings()) {
        out << "\n=== WARNINGS ===\n";
        for (const auto& warning : errors.warnings) {
            out << "WARNING: " << warning << "\n";
        }
    }
    
    if (errors.hasValidationErrors()) {
        out << "\n=== VALIDATION ERRORS ===\n";
        for (const auto& error : errors.validationErrors) {
            out << "VALIDATION ERROR: " << error.key << " - " << error.message << "\n";
        }
    }
}

// Implementation of ConfigSchema methods
void ConfigSchema::addRule(const std::string& key, const std::string& type, bool required, 
                          const std::string& description, double min, double max,
                          size_t minLen, size_t maxLen) {
    ValidationRules rule;
    rule.key = key;
    rule.type = type;
    rule.required = required;
    rule.description = description;
    rule.minValue = min;
    rule.maxValue = max;
    rule.minLength = minLen;
    rule.maxLength = maxLen;
    
    rules.push_back(rule);
}

std::vector<Config::ValidationError> ConfigSchema::validate() const {
    std::vector<ValidationError> errors;
    
    // Check for required keys
    for (const auto& rule : rules) {
        if (rule.required) {
            // This is a placeholder - in a real implementation, you'd have
            // the actual configuration to check against
            errors.emplace_back(rule.key, "Required key is missing");
        }
    }
    
    return errors;
}

bool ConfigSchema::loadSchemaFromFile(const std::string& schemaFilepath) {
    // This is a placeholder - in a real implementation, you would parse
    // a schema file format (JSON, YAML, etc.)
    filename = schemaFilepath;
    return true;
}

// Explicit template instantiations
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

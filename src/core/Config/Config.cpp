#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>
#include <cctype>
#include <regex>
#include <cmath>
#include <cstring>
#include <limits>

namespace nlm {

// Helper functions for config parsing
inline bool isNumeric(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos = 0;
    bool hasDigit = false;
    bool hasDecimal = false;
    bool hasSign = false;
    
    if (str[0] == '+' || str[0] == '-') {
        hasSign = true;
        pos = 1;
    }
    
    for (size_t i = pos; i < str.size(); ++i) {
        if (std::isdigit(str[i])) {
            hasDigit = true;
        } else if (str[i] == '.') {
            if (hasDecimal) return false; // Multiple decimal points
            hasDecimal = true;
        } else {
            return false; // Invalid character
        }
    }
    
    return hasDigit;
}

inline bool isInteger(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos = 0;
    bool hasSign = false;
    
    if (str[0] == '+' || str[0] == '-') {
        hasSign = true;
        pos = 1;
    }
    
    for (size_t i = pos; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return str.size() > pos; // Must have at least one digit
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Phase 2: Enhanced key=value format parser with validation
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with better validation
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key (must be non-empty and valid identifier)
            if (key.empty() || !isValidKey(key)) {
                NLM_LOG_WARNING("Invalid key in config file: '" + key + "'");
                continue;
            }
            
            // Remove quotes if present and validate
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Set the value
            set(key, value, ConfigSource::File);
        }
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

// Helper methods
bool Config::validateKey(const std::string& key) const {
    // Check if key is valid
    if (key.empty()) {
        return false;
    }
    
    // Allow alphanumeric characters, underscores, and dots
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.') {
            return false;
        }
    }
    
    return true;
}

bool Config::isValidKey(const std::string& key) const {
    return validateKey(key);
}

bool Config::setAdvancedConfig(const std::string& key, const std::string& value) {
    // Advanced config validation and setting
    if (!isValidKey(key)) {
        NLM_LOG_WARNING("Invalid advanced config key: '" + key + "'");
        return false;
    }
    
    // Try to parse value as different types
    if (isNumeric(value)) {
        if (isInteger(value)) {
            set(key, std::stoll(value), ConfigSource::Runtime);
        } else {
            set(key, std::stod(value), ConfigSource::Runtime);
        }
    } else if (value == "true" || value == "false") {
        set(key, value == "true", ConfigSource::Runtime);
    } else if (value == "null" || value == "none") {
        set(key, std::string(""), ConfigSource::Runtime);
    } else {
        // Treat as string
        set(key, value, ConfigSource::Runtime);
    }
    
    return true;
}

std::vector<std::string> Config::getConfigCommands() const {
    std::vector<std::string> commands;
    commands.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        commands.push_back("--" + entry.key + "=<value>");
        
        // Add type-specific hints
        if (std::holds_alternative<int>(entry.value)) {
            commands.back() += " (int)";
        } else if (std::holds_alternative<double>(entry.value)) {
            commands.back() += " (float)";
        } else if (std::holds_alternative<bool>(entry.value)) {
            commands.back() += " (bool)";;
        }
    }
    
    return commands;
}

bool Config::executeConfigCommand(const std::string& command) {
    if (command.empty() || command[0] != '-') {
        NLM_LOG_WARNING("Invalid config command: '" + command + "'");
        return false;
    }
    
    std::string cmd = command.substr(1);
    size_t pos = cmd.find('=');
    
    if (pos == std::string::npos) {
        NLM_LOG_WARNING("Invalid config command format: '" + command + "'");
        return false;
    }
    
    std::string key = cmd.substr(0, pos);
    std::string value = cmd.substr(pos + 1);
    
    return setAdvancedConfig(key, value);
}

// JSON/YAML parsing helpers (Phase 2)
bool Config::parseJson(const std::string& content) {
    // Basic JSON parsing implementation
    // This is a simplified implementation - a real one would use a proper JSON library
    if (content.empty() || content[0] != '{') {
        return false;
    }
    
    // Simple JSON parsing - find key-value pairs
    size_t pos = 1;
    while (pos < content.size() && content[pos] != '}') {
        // Find next key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) {
            break;
        }
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = trim(content.substr(keyStart + 1, keyEnd - keyStart - 1));
        
        // Find value
        size_t valueStart = content.find(':', keyEnd);
        if (valueStart == std::string::npos) {
            break;
        }
        
        size_t valueEnd = content.find('}', valueStart);
        if (valueEnd == std::string::npos) {
            valueEnd = content.find(',', valueStart);
            if (valueEnd == std::string::npos) {
                break;
            }
        }
        
        std::string valueStr = trim(content.substr(valueStart + 1, valueEnd - valueStart - 1));
        
        set(key, parseJsonValue(valueStr), ConfigSource::File);
        
        pos = valueEnd + 1;
    }
    
    return true;
}

bool Config::parseYaml(const std::string& content) {
    // Basic YAML parsing implementation
    if (content.empty() || content.find(':') == std::string::npos) {
        return false;
    }
    
    // Simple YAML parsing - find key: value pairs
    size_t pos = 0;
    while (pos < content.size()) {
        size_t colonPos = content.find(':', pos);
        if (colonPos == std::string::npos) {
            break;
        }
        
        std::string key = trim(content.substr(0, colonPos));
        
        // Find end of content
        size_t endPos = content.find_last_not_of('\n', content.size() - 1);
        if (endPos == std::string::npos) {
            break;
        }
        
        std::string valueStr = trim(content.substr(colonPos + 1, endPos - colonPos));
        
        set(key, parseYamlValue(valueStr), ConfigSource::File);
        
        pos = endPos + 1;
    }
    
    return true;
}

ConfigValue Config::parseJsonValue(const std::string& str) {
    // Simplified JSON value parsing
    if (str.empty()) {
        return "";
    }
    
    // Check for JSON types
    if (str[0] == '"' && str.back() == '"') {
        return trimJsonString(str.substr(1, str.size() - 2));
    } else if (str == "true" || str == "false") {
        return str == "true";
    } else if (str == "null") {
        return std::string("");
    } else if (isNumeric(str)) {
        // Try integer first, then double
        try {
            if (str.find('.') == std::string::npos) {
                return std::stoll(str);
            }
        } catch (...) {}
        
        try {
            return std::stod(str);
        } catch (...) {}
    }
    
    return std::string(str);
}

ConfigValue Config::parseYamlValue(const std::string& str) {
    // Simplified YAML value parsing
    if (str.empty()) {
        return "";
    }
    
    // Check for YAML types
    if (str[0] == '"' || str[0] == '\'') {
        if (str.back() == '"' || str.back() == '\'') {
            return str.substr(1, str.size() - 2);
        }
    }
    
    if (str == "true" || str == "false") {
        return str == "true";
    } else if (str == "null" || str == "none") {
        return std::string("");
    } else if (isNumeric(str)) {
        // Try integer first, then double
        try {
            if (str.find('.') == std::string::npos) {
                return std::stoll(str);
            }
        } catch (...) {}
        
        try {
            return std::stod(str);
        } catch (...) {}
    }
    
    return std::string(str);
}

std::string Config::trimJsonString(const std::string& str) {
    std::string result = str;
    if (result.size() >= 2 && result.front() == '"' && result.back() == '"') {
        result = result.substr(1, result.size() - 2);
    }
    return result;
}

// Template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
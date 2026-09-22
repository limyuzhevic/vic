#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to detect file format
    std::string line;
    bool isJSON = false;
    bool isYAML = false;
    
    // Read first few lines to detect format
    for (int i = 0; i < 5 && std::getline(file, line); ++i) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        // Check for JSON format (starts with {)
        if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
            isJSON = true;
        }
        // Check for YAML format (key: value with indentation)
        else if (line.find(':') != std::string::npos && 
                 (line.find(' ') == std::string::npos || line.find('\t') != std::string::npos)) {
            isYAML = true;
        }
        // Check for simple key=value format
        else if (line.find('=') != std::string::npos && 
                 line.find('{') == std::string::npos && 
                 line.find('[') == std::string::npos) {
            // This is simple format, continue reading
            break;
        }
    }
    
    // Reset file to beginning
    file.clear();
    file.seekg(0, std::ios::beg);
    
    if (isJSON) {
        return loadFromJSON(filepath);
    } else if (isYAML) {
        return loadFromYAML(filepath);
    } else {
        // Fall back to simple key=value format
        return loadFromSimple(filepath);
    }
}

// Forward declaration for JSON parser
namespace nlm { 
    bool Config::parseJSONValue(const std::string& str, ConfigValue& value);
    bool Config::parseSimpleValue(const std::string& str, ConfigValue& value);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // TODO: Implement proper JSON parsing
    // For now, use nlohmann::json if available
    // This is a placeholder for the Phase 2 implementation
    
    NLM_LOG_WARNING("JSON parsing not yet implemented - using simple format");
    return loadFromSimple(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // TODO: Implement proper YAML parsing
    // For now, use yaml-cpp if available
    // This is a placeholder for the Phase 2 implementation
    
    NLM_LOG_WARNING("YAML parsing not yet implemented - using simple format");
    return loadFromSimple(filepath);
}

bool Config::loadFromSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": missing '='");
            hasErrors = true;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": empty key");
            hasErrors = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate value format
        ConfigValue configValue;
        if (!parseSimpleValue(value, configValue)) {
            NLM_LOG_WARNING("Invalid value format for key '" + key + "': '" + value + "'");
            hasErrors = true;
            continue;
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Configuration file loaded with errors");
    }
    
    return true;
}

bool Config::parseSimpleValue(const std::string& str, ConfigValue& value) {
    // Try to parse as boolean
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    // Try to parse as integer
    if (isInteger(str)) {
        value = std::stoll(str);
        return true;
    }
    
    // Try to parse as floating point
    if (isDouble(str)) {
        value = std::stod(str);
        return true;
    }
    
    // Default to string
    value = str;
    return true;
}

bool Config::parseJSONValue(const std::string& str, ConfigValue& value) {
    // TODO: Implement proper JSON value parsing
    return parseSimpleValue(str, value);
}

bool Config::isInteger(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }
    
    for (size_t i = start; i < str.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
            return false;
        }
    }
    
    return true;
}

bool Config::isDouble(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t decimalCount = 0;
    size_t start = 0;
    
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }
    
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            decimalCount++;
            if (decimalCount > 1) return false;
        } else if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
            return false;
        }
    }
    
    return decimalCount > 0;
}

// Add validation method
bool Config::validate(const std::string& key, const ConfigValue& value) const {
    // Basic validation rules
    if (key == "neuron_count") {
        if (std::holds_alternative<size_t>(value)) {
            size_t count = std::get<size_t>(value);
            return count > 0 && count < 1000000; // Reasonable limits
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            return count > 0 && count < 1000000;
        }
    }
    
    if (key == "region_count") {
        if (std::holds_alternative<size_t>(value)) {
            size_t count = std::get<size_t>(value);
            return count > 0 && count < 100;
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            return count > 0 && count < 100;
        }
    }
    
    if (key == "connection_probability") {
        if (std::holds_alternative<double>(value)) {
            double prob = std::get<double>(value);
            return prob >= 0.0 && prob <= 1.0;
        } else if (std::holds_alternative<float>(value)) {
            float prob = std::get<float>(value);
            return prob >= 0.0f && prob <= 1.0f;
        }
    }
    
    // Default validation - pass any value
    return true;
}

// Add validation to set method
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value before setting
    if (!validate(key, value)) {
        NLM_LOG_WARNING("Invalid configuration value for key '" + key + "': validation failed");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

// Add helper method for setting typed values with validation
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue;
    if (parseSimpleValue(value, configValue) && validate(key, configValue)) {
        set(key, configValue, source);
    } else {
        NLM_LOG_WARNING("Failed to set configuration key '" + key + "' with value '" + value + "'");
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    ConfigValue configValue = value;
    if (validate(key, configValue)) {
        set(key, configValue, source);
    } else {
        NLM_LOG_WARNING("Invalid integer value for key '" + key + "': " + std::to_string(value));
    }
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigValue configValue = value;
    if (validate(key, configValue)) {
        set(key, configValue, source);
    } else {
        NLM_LOG_WARNING("Invalid double value for key '" + key + "': " + std::to_string(value));
    }
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigValue configValue = value;
    if (validate(key, configValue)) {
        set(key, configValue, source);
    } else {
        NLM_LOG_WARNING("Invalid boolean value for key '" + key + "': " + (value ? "true" : "false"));
    }
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
// Complete the implementation by adding missing methods at the end of Config.cpp

// Update loadFromArgs to use the new set method
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

// Add default implementations for missing helper methods
bool Config::loadFromJSON(const std::string& filepath) {
    // TODO: Implement proper JSON parsing
    NLM_LOG_WARNING("JSON parsing not yet implemented - falling back to simple format");
    return loadFromSimple(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
    // TODO: Implement proper YAML parsing
    NLM_LOG_WARNING("YAML parsing not yet implemented - falling back to simple format");
    return loadFromSimple(filepath);
}

bool Config::loadFromSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": missing '='");
            hasErrors = true;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": empty key");
            hasErrors = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate value format
        ConfigValue configValue;
        if (!parseSimpleValue(value, configValue)) {
            NLM_LOG_WARNING("Invalid value format for key '" + key + "': '" + value + "'");
            hasErrors = true;
            continue;
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Configuration file loaded with errors");
    }
    
    return true;
}

bool Config::parseSimpleValue(const std::string& str, ConfigValue& value) {
    // Try to parse as boolean
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    // Try to parse as integer (int64_t)
    if (isInteger(str)) {
        try {
            value = std::stoll(str);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Try to parse as floating point (double)
    if (isDouble(str)) {
        try {
            value = std::stod(str);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Default to string
    value = str;
    return true;
}

bool Config::parseJSONValue(const std::string& str, ConfigValue& value) {
    // TODO: Implement proper JSON value parsing
    return parseSimpleValue(str, value);
}

bool Config::isInteger(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }
    
    for (size_t i = start; i < str.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
            return false;
        }
    }
    
    return true;
}

bool Config::isDouble(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t decimalCount = 0;
    size_t start = 0;
    
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }
    
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            decimalCount++;
            if (decimalCount > 1) return false;
        } else if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
            return false;
        }
    }
    
    return decimalCount > 0;
}

bool Config::validate(const std::string& key, const ConfigValue& value) const {
    // Basic validation rules
    if (key == "neuron_count") {
        if (std::holds_alternative<size_t>(value)) {
            size_t count = std::get<size_t>(value);
            return count > 0 && count < 1000000; // Reasonable limits
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            return count > 0 && count < 1000000;
        }
    }
    
    if (key == "region_count") {
        if (std::holds_alternative<size_t>(value)) {
            size_t count = std::get<size_t>(value);
            return count > 0 && count < 100;
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            return count > 0 && count < 100;
        }
    }
    
    if (key == "connection_probability") {
        if (std::holds_alternative<double>(value)) {
            double prob = std::get<double>(value);
            return prob >= 0.0 && prob <= 1.0;
        } else if (std::holds_alternative<float>(value)) {
            float prob = std::get<float>(value);
            return prob >= 0.0f && prob <= 1.0f;
        }
    }
    
    // Default validation - pass any value
    return true;
}

// Update set method to use validation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value before setting
    if (!validate(key, value)) {
        NLM_LOG_WARNING("Invalid configuration value for key '" + key + "': validation failed");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

// Update set methods to use validation
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue;
    if (parseSimpleValue(value, configValue) && validate(key, configValue)) {
        set(key, configValue, source);
    } else {
        NLM_LOG_WARNING("Failed to set configuration key '" + key + "' with value '" + value + "'");
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    ConfigValue configValue = value;
    if (validate(key, configValue)) {
        set(key, configValue, source);
    } else {
        NLM_LOG_WARNING("Invalid integer value for key '" + key + "': " + std::to_string(value));
    }
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigValue configValue = value;
    if (validate(key, configValue)) {
        set(key, configValue, source);
    } else {
        NLM_LOG_WARNING("Invalid double value for key '" + key + "': " + std::to_string(value));
    }
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigValue configValue = value;
    if (validate(key, configValue)) {
        set(key, configValue, source);
    } else {
        NLM_LOG_WARNING("Invalid boolean value for key '" + key + "': " + (value ? "true" : "false"));
    }
}

// Add helper methods definition
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromSimple(filepath);
}

// Add missing method definitions
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
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

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

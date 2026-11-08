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
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schema;
    std::mutex mutex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

void Config::addSchema(const std::string& key, const std::string& description, 
                       ConfigSource source, bool required, const std::string& type,
                       const std::vector<std::pair<std::string, double>>& range,
                       const std::vector<std::string>& allowedValues) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ConfigSchema schema;
    schema.key = key;
    schema.description = description;
    schema.defaultSource = source;
    schema.required = required;
    schema.type = type;
    schema.range = range;
    schema.allowedValues = allowedValues;
    
    // Remove existing schema for this key
    pImpl->schema.erase(std::remove_if(pImpl->schema.begin(), pImpl->schema.end(),
                                     [&key](const ConfigSchema& s) { return s.key == key; }),
                       pImpl->schema.end());
    
    pImpl->schema.push_back(schema);
}

void Config::removeSchema(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->schema.erase(std::remove_if(pImpl->schema.begin(), pImpl->schema.end(),
                                       [&key](const ConfigSchema& s) { return s.key == key; }),
                       pImpl->schema.end());
}

Config::ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& schema : pImpl->schema) {
        bool isValid = true;
        std::string error;
        
        // Check if required key exists
        if (schema.required && !has(schema.key)) {
            result.invalidKeys.push_back(schema.key);
            isValid = false;
            error += "Missing required key: " + schema.key + ". ";
        }
        
        if (!isValid) {
            continue;
        }
        
        // Validate if key exists
        if (has(schema.key)) {
            auto value = get<std::string>(schema.key);
            
            // Validate type
            if (schema.type == "int" || schema.type == "int64_t") {
                auto val = get<int64_t>(schema.key);
                if (!val.has_value()) {
                    result.invalidKeys.push_back(schema.key);
                    isValid = false;
                    error += "Value for key '" + schema.key + "' must be an integer. ";
                }
                
                if (isValid && !schema.range.empty()) {
                    double min = schema.range[0].second; // second is the value in pair
                    double max = schema.range.size() > 1 ? schema.range[1].second : min;
                    if (val.value() < min || val.value() > max) {
                        result.invalidKeys.push_back(schema.key);
                        isValid = false;
                        error += "Value for key '" + schema.key + "' must be between " + 
                                 std::to_string(min) + " and " + std::to_string(max) + ". ";
                    }
                }
            } else if (schema.type == "double" || schema.type == "float") {
                auto val = get<double>(schema.key);
                if (!val.has_value()) {
                    result.invalidKeys.push_back(schema.key);
                    isValid = false;
                    error += "Value for key '" + schema.key + "' must be a floating-point number. ";
                }
                
                if (isValid && !schema.range.empty()) {
                    double min = schema.range[0].second;
                    double max = schema.range.size() > 1 ? schema.range[1].second : min;
                    if (val.value() < min || val.value() > max) {
                        result.invalidKeys.push_back(schema.key);
                        isValid = false;
                        error += "Value for key '" + schema.key + "' must be between " + 
                                 std::to_string(min) + " and " + std::to_string(max) + ". ";
                    }
                }
            } else if (schema.type == "bool") {
                auto val = get<bool>(schema.key);
                if (!val.has_value()) {
                    result.invalidKeys.push_back(schema.key);
                    isValid = false;
                    error += "Value for key '" + schema.key + "' must be a boolean. ";
                }
            } else if (schema.type == "string") {
                auto val = get<std::string>(schema.key);
                if (!val.has_value()) {
                    result.invalidKeys.push_back(schema.key);
                    isValid = false;
                    error += "Value for key '" + schema.key + "' must be a string. ";
                } else if (!schema.allowedValues.empty()) {
                    bool found = false;
                    for (const auto& allowed : schema.allowedValues) {
                        if (val.value() == allowed) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        result.invalidKeys.push_back(schema.key);
                        isValid = false;
                        error += "Value for key '" + schema.key + "' must be one of: ";
                        for (size_t i = 0; i < schema.allowedValues.size(); ++i) {
                            if (i > 0) error += ", ";
                            error += schema.allowedValues[i];
                        }
                        error += ". ";
                    }
                }
            } else if (schema.type == "vector") {
                // Check if it's a vector type
                auto val = get<std::vector<int>>(schema.key);
                if (!val.has_value()) {
                    val = get<std::vector<double>>(schema.key);
                }
                if (!val.has_value()) {
                    val = get<std::vector<std::string>>(schema.key);
                }
                if (!val.has_value()) {
                    result.invalidKeys.push_back(schema.key);
                    isValid = false;
                    error += "Value for key '" + schema.key + "' must be a vector. ";
                }
            }
        }
        
        if (!isValid) {
            result.valid = false;
            result.errorMessage += error;
        }
    }
    
    return result;
}

std::vector<Config::ConfigSchema> Config::getSchema() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->schema;
}

std::optional<Config::ConfigSchema> Config::getSchema(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                          [&key](const ConfigSchema& s) { return s.key == key; });
    if (it != pImpl->schema.end()) {
        return *it;
    }
    return std::nullopt;
}

std::string Config::getSchemaSummary() const {
    std::ostringstream oss;
    oss << "Configuration Schema (" << pImpl->schema.size() << " entries):\n";
    for (const auto& schema : pImpl->schema) {
        oss << "  " << schema.key << ": " << schema.description << " (" << schema.type << ")";
        if (schema.required) oss << " [REQUIRED]";
        if (!schema.allowedValues.empty()) {
            oss << " [allowed: ";
            for (size_t i = 0; i < schema.allowedValues.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << schema.allowedValues[i];
            }
            oss << "]";
        }
        oss << "\n";
    }
    return oss.str();
}

void Config::merge(const Config& other, bool override) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& otherEntry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (it != pImpl->entries.end()) {
            if (override) {
                // Replace if override is true
                *it = otherEntry;
            } else {
                // Keep original if sources are higher priority
                // CommandLine > File > Default > Runtime
                if (static_cast<int>(otherEntry.source) > static_cast<int>(it->source)) {
                    *it = otherEntry;
                }
            }
        } else {
            pImpl->entries.push_back(otherEntry);
        }
    }
}

void Config::loadFromEnvironment(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& entry : pImpl->schema) {
        std::string envKey = prefix + toLower(entry.key);
        
        // Check if environment variable exists
        const char* envValue = std::getenv(envKey.c_str());
        if (envValue) {
            std::string value(envValue);
            
            // Convert to appropriate type
            ConfigValue configValue;
            if (entry.type == "int" || entry.type == "int64_t") {
                configValue = std::stoll(value);
            } else if (entry.type == "double" || entry.type == "float") {
                configValue = std::stod(value);
            } else if (entry.type == "bool") {
                configValue = (value == "true" || value == "1" || value == "yes");
            } else {
                configValue = value;
            }
            
            set(entry.key, configValue, ConfigSource::Runtime);
        }
    }
}

Config Config::copy() const {
    Config other;
    other.pImpl->entries = pImpl->entries;
    other.pImpl->schema = pImpl->schema;
    return other;
}

std::string Config::getStats() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    oss << "  Total schema: " << pImpl->schema.size() << "\n";
    
    // Count by source
    int defaultCount = 0, fileCount = 0, commandLineCount = 0, runtimeCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: commandLineCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    
    oss << "  By source: Default=" << defaultCount 
        << ", File=" << fileCount 
        << ", CommandLine=" << commandLineCount 
        << ", Runtime=" << runtimeCount << "\n";
    
    // Count by type
    int intCount = 0, doubleCount = 0, stringCount = 0, boolCount = 0, vectorCount = 0;
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<int>(entry.value)) {
            intCount++;
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            intCount++;
        } else if (std::holds_alternative<double>(entry.value)) {
            doubleCount++;
        } else if (std::holds_alternative<bool>(entry.value)) {
            boolCount++;
        } else if (std::holds_alternative<std::string>(entry.value)) {
            stringCount++;
        } else if (std::holds_alternative<std::vector<int>>(entry.value) ||
                   std::holds_alternative<std::vector<double>>(entry.value) ||
                   std::holds_alternative<std::vector<std::string>>(entry.value)) {
            vectorCount++;
        }
    }
    
    oss << "  By type: int=" << intCount 
        << ", double=" << doubleCount 
        << ", string=" << stringCount 
        << ", bool=" << boolCount 
        << ", vector=" << vectorCount << "\n";
    
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

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
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

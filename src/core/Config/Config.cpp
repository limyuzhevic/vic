#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyToIndex;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema_() {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)), schema_(std::move(other.schema_)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        schema_ = std::move(other.schema_);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath, const std::vector<ConfigSchema>& schema) {
    setSchema(schema);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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
            
            if (!setFromString(key, value, ConfigSource::File)) {
                return false;  // File format error
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv, const std::vector<ConfigSchema>& schema) {
    setSchema(schema);
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    return false;  // Invalid command line argument
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setFromString(key, value, ConfigSource::CommandLine)) {
                return false;  // Invalid command line argument
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated: " << "TODO_TIMESTAMP\n";
    file << "\n";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == ConfigSource::CommandLine) {
            continue;  // Skip runtime overrides
        }
        
        file << "# " << getDescription(entry.key) << " (" << getUnit(entry.key) << ")\n";
        file << entry.key << " = " << "TODO_VALUE\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(pImpl->entries[index].value);
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
    // Remove from lookup map first
    auto it_map = pImpl->keyToIndex.find(key);
    if (it_map != pImpl->keyToIndex.end()) {
        size_t index = it_map->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
            return;
        }
    }
    
    // Add new entry
    size_t newIndex = pImpl->entries.size();
    pImpl->entries.emplace_back(key, value, source);
    pImpl->keyToIndex[key] = newIndex;
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

bool Config::setFromString(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to infer type from schema
    std::optional<ConfigSchema::Type> inferredType;
    
    // Check schema for this key
    for (const auto& schema : schema_) {
        if (schema.key == key) {
            inferredType = schema.type;
            break;
        }
    }
    
    // Infer type if not in schema
    if (!inferredType) {
        if (value.empty()) {
            return false;
        }
        if (value == "true" || value == "false") {
            inferredType = ConfigSchema::Type::BOOL;
        } else if (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit) ||
                   std::all_of(value.begin(), value.end(), ::isdigit)) {
            inferredType = ConfigSchema::Type::INT;
        } else {
            // Try to parse as double
            char* endptr;
            strtod(value.c_str(), &endptr);
            if (*endptr == '\0') {
                inferredType = ConfigSchema::Type::DOUBLE;
            } else {
                inferredType = ConfigSchema::Type::STRING;
            }
        }
    }
    
    ConfigValue configValue;
    
    try {
        switch (*inferredType) {
            case ConfigSchema::Type::INT: {
                int intValue = std::stoi(value);
                configValue = intValue;
                break;
            }
            case ConfigSchema::Type::INT64: {
                int64_t int64Value = std::stoll(value);
                configValue = int64Value;
                break;
            }
            case ConfigSchema::Type::DOUBLE: {
                double doubleValue = std::stod(value);
                configValue = doubleValue;
                break;
            }
            case ConfigSchema::Type::BOOL: {
                bool boolValue = (value == "true");
                configValue = boolValue;
                break;
            }
            case ConfigSchema::Type::STRING: {
                configValue = value;
                break;
            }
            default: {
                configValue = value;
                break;
            }
        }
    } catch (const std::exception&) {
        return false;  // Failed to parse
    }
    
    set(key, configValue, source);
    return true;
}

bool Config::has(const std::string& key) const {
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return;
    }
    
    size_t index = it->second;
    if (index < pImpl->entries.size()) {
        pImpl->entries.erase(pImpl->entries.begin() + index);
        pImpl->keyToIndex.erase(it);
        
        // Update indices for remaining entries
        for (auto& kv : pImpl->keyToIndex) {
            if (kv.second > index) {
                kv.second--;
            }
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
    pImpl->keyToIndex.clear();
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

void Config::setSchema(const std::vector<ConfigSchema>& schema) {
    schema_ = schema;
    
    // Validate schema for duplicates
    std::unordered_map<std::string, size_t> keyCounts;
    for (const auto& s : schema_) {
        keyCounts[s.key]++;
    }
    
    for (const auto& kv : keyCounts) {
        if (kv.second > 1) {
            // TODO: Warning for duplicate schema entries
        }
    }
}

bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& schema : schema_) {
        if (schema.required && !has(schema.key)) {
            errors.push_back("Required parameter missing: " + schema.key);
            continue;
        }
        
        if (has(schema.key)) {
            auto it = pImpl->keyToIndex.find(schema.key);
            if (it != pImpl->keyToIndex.end()) {
                size_t index = it->second;
                if (index < pImpl->entries.size()) {
                    if (!validateValue(schema, pImpl->entries[index].value)) {
                        errors.push_back("Invalid value for parameter: " + schema.key);
                    }
                }
            }
        }
    }
    
    return errors;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            return pImpl->entries[index].source;
        }
    }
    return ConfigSource::Default;
}

std::string Config::getDescription(const std::string& key) const {
    for (const auto& schema : schema_) {
        if (schema.key == key && schema.description) {
            return *schema.description;
        }
    }
    return "";
}

std::string Config::getUnit(const std::string& key) const {
    for (const auto& schema : schema_) {
        if (schema.key == key && schema.unit) {
            return *schema.unit;
        }
    }
    return "";
}

template<typename T>
std::optional<T> Config::getFromString(const std::string& value) const {
    try {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::stoll(value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return value == "true" || value == "1" || value == "yes";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return value;
        }
    } catch (const std::exception&) {
        return std::nullopt;
    }
    return std::nullopt;
}

bool Config::validateValue(const ConfigSchema& schema, const ConfigValue& value) const {
    try {
        switch (schema.type) {
            case ConfigSchema::Type::INT: {
                int val = std::get<int>(value);
                if (schema.range) {
                    if (val < schema.range->first || val > schema.range->second) {
                        return false;
                    }
                }
                break;
            }
            case ConfigSchema::Type::INT64: {
                int64_t val = std::get<int64_t>(value);
                if (schema.range) {
                    if (val < static_cast<int64_t>(schema.range->first) ||
                        val > static_cast<int64_t>(schema.range->second)) {
                        return false;
                    }
                }
                break;
            }
            case ConfigSchema::Type::DOUBLE: {
                double val = std::get<double>(value);
                if (schema.range) {
                    if (val < schema.range->first || val > schema.range->second) {
                        return false;
                    }
                }
                break;
            }
            case ConfigSchema::Type::BOOL: {
                // Always valid for bool
                break;
            }
            case ConfigSchema::Type::STRING: {
                std::string val = std::get<std::string>(value);
                if (schema.minSize && val.size() < schema.minSize) {
                    return false;
                }
                break;
            }
            default: {
                return false;
            }
        }
    } catch (const std::bad_variant_access&) {
        return false;
    }
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

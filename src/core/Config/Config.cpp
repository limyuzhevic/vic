#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Configuration validation implementation
bool Config::isValidKey(const std::string& key) {
    if (key.empty() || key.length() > 255) {
        return false;
    }
    
    // Check for invalid characters
    static const std::regex invalidChars("[\\s\\p{Punct}]");
    if (std::regex_search(key, invalidChars)) {
        return false;
    }
    
    // Should start with letter or underscore
    if (!std::isalpha(static_cast<unsigned char>(key[0])) && key[0] != '_') {
        return false;
    }
    
    return true;
}

bool Config::isValidValue(const ConfigValue& value) {
    // A value is valid if it can be visited by std::visit
    struct Validator {
        bool operator()(const std::string&) const { return true; }
        bool operator()(int) const { return true; }
        bool operator()(int64_t) const { return true; }
        bool operator()(double) const { return true; }
        bool operator()(bool) const { return true; }
        bool operator()(float) const { return true; }
        bool operator()(uint64_t) const { return true; }
        bool operator()(const std::vector<int>&) const { return true; }
        bool operator()(const std::vector<double>&) const { return true; }
        bool operator()(const std::vector<std::string>&) const { return true; }
        bool operator()(const ConfigValue&) const { return false; } // Should never happen
    };
    
    return std::visit(Validator{}, value);
}

// Configuration exception implementation
ConfigException::ConfigException(ConfigError error, const std::string& message)
    : error_(error), message_(message) {}

ConfigError ConfigException::getError() const {
    return error_;
}

const std::string& ConfigException::getMessage() const {
    return message_;
}

bool Config::has(const std::string& key) const {
    // Validate key first
    if (!isValidKey(key)) {
        throw ConfigException(ConfigError::InvalidKey, "Invalid configuration key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end();
}

const ConfigValue& Config::get(const std::string& key) const {
    // Validate key
    if (!isValidKey(key)) {
        throw ConfigException(ConfigError::InvalidKey, "Invalid configuration key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        throw ConfigException(ConfigError::KeyNotFound, "Configuration key not found: " + key);
    }
    
    return it->value;
}

template<typename T>
T Config::get(const std::string& key) const {
    try {
        const ConfigValue& value = get(key);
        return std::get<T>(value);
    } catch (const std::bad_variant_access&) {
        throw ConfigException(ConfigError::ConversionError, 
            "Type conversion failed for key: " + key + ". Expected " + typeid(T).name());
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        return get<T>(key);
    } catch (const ConfigException&) {
        // Key not found or type conversion failed - return default
        return defaultValue;
    } catch (...) {
        // Other exception - return default
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate inputs
    if (!isValidKey(key)) {
        throw ConfigException(ConfigError::InvalidKey, "Invalid configuration key: " + key);
    }
    
    if (!isValidValue(value)) {
        throw ConfigException(ConfigError::InvalidValue, "Invalid configuration value for key: " + key);
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

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}
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
            
            // Try to convert to different types based on content
            bool converted = false;
            
            // Try integer
            try {
                size_t posEnd;
                long long intVal = std::stoll(value, &posEnd);
                if (posEnd == value.size()) {
                    set(key, intVal, ConfigSource::File);
                    converted = true;
                }
            } catch (...) {}
            
            // Try unsigned integer if integer failed
            if (!converted) {
                try {
                    size_t posEnd;
                    unsigned long long uintVal = std::stoull(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, uint64_t(uintVal), ConfigSource::File);
                        converted = true;
                    }
                } catch (...) {}
            }
            
            // Try float/double if integer failed
            if (!converted) {
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        // Check if it's actually an integer
                        if (static_cast<long long>(doubleVal) == doubleVal) {
                            set(key, static_cast<int>(doubleVal), ConfigSource::File);
                        } else {
                            set(key, doubleVal, ConfigSource::File);
                        }
                        converted = true;
                    }
                } catch (...) {}
            }
            
            // Try float if double failed
            if (!converted) {
                try {
                    size_t posEnd;
                    float floatVal = std::stof(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, floatVal, ConfigSource::File);
                        converted = true;
                    }
                } catch (...) {}
            }
            
            // Fall back to string if numeric conversion failed
            if (!converted) {
                set(key, value, ConfigSource::File);
            }
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
        
        // Write the actual value based on its type
        if (std::holds_alternative<std::string>(entry.value)) {
            file << entry.key << " = \"" << std::get<std::string>(entry.value) << "\"\n";
        } else if (std::holds_alternative<int>(entry.value)) {
            file << entry.key << " = " << std::get<int>(entry.value) << "\n";
        } else if (std::holds_alternative<double>(entry.value)) {
            file << entry.key << " = " << std::get<double>(entry.value) << "\n";
        } else if (std::holds_alternative<float>(entry.value)) {
            file << entry.key << " = " << std::get<float>(entry.value) << "\n";
        } else if (std::holds_alternative<uint64_t>(entry.value)) {
            file << entry.key << " = " << std::get<uint64_t>(entry.value) << "\n";
        } else {
            // Fallback for unknown types
            file << entry.key << " = \"UNKNOWN_TYPE\"\n";
        }
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

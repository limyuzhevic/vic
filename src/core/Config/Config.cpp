#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <string>
#include <cctype>
#include <cstdlib>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> envCache;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() {}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

ConfigResult Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return ConfigResult(ConfigErrorCode::FileNotFound, "Failed to open file: " + filepath);
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
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
            
            // Validate key
            if (key.empty() || key.length() > ConfigConstants::MAX_KEY_LENGTH) {
                return ConfigResult(ConfigErrorCode::InvalidFormat, 
                    "Invalid key at line " + std::to_string(lineNum) + ": " + key);
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return ConfigResult();
}

ConfigResult Config::loadFromArgs(int argc, char** argv) {
    int validArgs = 0;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (key.empty() || key.length() > ConfigConstants::MAX_KEY_LENGTH) {
                    return ConfigResult(ConfigErrorCode::InvalidFormat, "Invalid key: " + key);
                }
                
                set(key, value, ConfigSource::CommandLine);
                validArgs++;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key
            if (key.empty() || key.length() > ConfigConstants::MAX_KEY_LENGTH) {
                return ConfigResult(ConfigErrorCode::InvalidFormat, "Invalid key: " + key);
            }
            
            set(key, value, ConfigSource::CommandLine);
            validArgs++;
        }
    }
    
    if (validArgs == 0) {
        return ConfigResult(ConfigErrorCode::InvalidFormat, "No valid command-line arguments provided");
    }
    
    return ConfigResult();
}

ConfigResult Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return ConfigResult(ConfigErrorCode::PermissionDenied, "Failed to open file for writing: " + filepath);
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        // Visit the variant and extract the actual value
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n";
    }
    
    return ConfigResult();
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

static ConfigResult validateKey(const std::string& key) {
    if (key.empty()) {
        return ConfigResult(ConfigErrorCode::InvalidFormat, "Key cannot be empty");
    }
    if (key.length() > ConfigConstants::MAX_KEY_LENGTH) {
        return ConfigResult(ConfigErrorCode::InvalidFormat, 
            "Key too long (max " + std::to_string(ConfigConstants::MAX_KEY_LENGTH) + " chars)");
    }
    return ConfigResult();
}

static ConfigResult validateValue(const ConfigValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        const auto& str = std::get<std::string>(value);
        if (str.length() > ConfigConstants::MAX_STRING_LENGTH) {
            return ConfigResult(ConfigErrorCode::InvalidFormat,
                "String value too long (max " + std::to_string(ConfigConstants::MAX_STRING_LENGTH) + " chars)");
        }
    } else if (std::holds_alternative<int>(value)) {
        const auto& num = std::get<int>(value);
        if (num < ConfigConstants::MIN_INT_VALUE || num > ConfigConstants::MAX_INT_VALUE) {
            return ConfigResult(ConfigErrorCode::InvalidFormat, "Integer value out of range");
        }
    } else if (std::holds_alternative<double>(value)) {
        const auto& num = std::get<double>(value);
        if (num < ConfigConstants::MIN_DOUBLE_VALUE || num > ConfigConstants::MAX_DOUBLE_VALUE) {
            return ConfigResult(ConfigErrorCode::InvalidFormat, "Double value out of range");
        }
    }
    return ConfigResult();
}

static ConfigResult convertStringToValue(const std::string& str, ConfigValue& outValue) {
    if (str.empty()) {
        return ConfigResult(ConfigErrorCode::InvalidFormat, "Cannot convert empty string to value");
    }
    
    // Try parsing as boolean
    if (str == "true" || str == "false") {
        bool val = (str == "true");
        outValue = val;
        return ConfigResult();
    }
    
    // Try parsing as integer
    try {
        size_t pos;
        long long_val = std::stoll(str, &pos);
        if (pos == str.length()) {
            // Check for 64-bit overflow
            if (long_val > INT64_MAX) {
                // Try double instead
                double double_val = std::stod(str);
                outValue = double_val;
                return ConfigResult();
            }
            outValue = static_cast<int64_t>(long_val);
            return ConfigResult();
        }
    } catch (const std::exception&) {}
    
    // Try parsing as double
    try {
        double double_val = std::stod(str);
        outValue = double_val;
        return ConfigResult();
    } catch (const std::exception&) {}
    
    // Use as string
    outValue = str;
    return ConfigResult();
}

static std::string toEnvKey(const std::string& key) {
    std::string envKey = ConfigConstants::ENV_PREFIX + key;
    // Convert to uppercase
    std::transform(envKey.begin(), envKey.end(), envKey.begin(), ::toupper);
    return envKey;
}

ConfigResult Config::loadFromJsonFile(const std::string& filepath) {
    // TODO: Add nlohmann::json support
    // For now, treat as INI file
    return loadFromIniFile(filepath);
}

ConfigResult Config::loadFromIniFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return ConfigResult(ConfigErrorCode::FileNotFound, "Failed to open file: " + filepath);
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            auto keyResult = validateKey(key);
            if (!keyResult.success()) {
                return ConfigResult(ConfigErrorCode::InvalidFormat, 
                    keyResult.message + " at line " + std::to_string(lineNum));
            }
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigValue val;
            auto convertResult = convertStringToValue(value, val);
            if (!convertResult.success()) {
                return ConfigResult(ConfigErrorCode::InvalidType, 
                    convertResult.message + " at line " + std::to_string(lineNum));
            }
            
            auto valResult = validateValue(val);
            if (!valResult.success()) {
                return ConfigResult(ConfigErrorCode::InvalidFormat, 
                    valResult.message + " at line " + std::to_string(lineNum));
            }
            
            // Check for duplicates
            if (has(key)) {
                return ConfigResult(ConfigErrorCode::DuplicateKey, "Duplicate key: " + key);
            }
            
            set(key, val, ConfigSource::File);
        }
    }
    
    return ConfigResult();
}

ConfigResult Config::loadFromEnv(const std::string& prefix) {
    clear();
    
    const char* env = std::getenv("PATH");
    
    // TODO: Implement environment variable parsing
    // For now, clear and return success
    
    return ConfigResult();
}

ConfigResult Config::saveToJsonFile(const std::string& filepath) const {
    // TODO: Add nlohmann::json support
    // For now, save as INI
    return saveToFile(filepath);
}

template<typename T>
ConfigResult Config::get(const std::string& key, T& outValue) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigResult(ConfigErrorCode::KeyNotFound, "Key not found: " + key);
    }
    
    try {
        outValue = std::get<T>(it->value);
        return ConfigResult();
    } catch (const std::bad_variant_access&) {
        return ConfigResult(ConfigErrorCode::InvalidType, 
            "Invalid type for key: " + key);
    }
}

template<typename T>
T Config::getEnv(const std::string& key, const T& defaultValue) const {
    std::string envKey = toEnvKey(key);
    
    const char* envVal = std::getenv(envKey.c_str());
    if (!envVal) {
        return defaultValue;
    }
    
    try {
        if constexpr (std::is_same_v<T, bool>) {
            std::string valStr(envVal);
            if (valStr == "true" || valStr == "1") {
                return true;
            } else if (valStr == "false" || valStr == "0") {
                return false;
            } else {
                return defaultValue;
            }
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return static_cast<T>(std::stoll(envVal));
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(envVal);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::string(envVal);
        } else {
            return defaultValue;
        }
    } catch (const std::exception&) {
        return defaultValue;
    }
}
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

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
            
            // Try to parse the value
            ConfigValue parsedValue;
            if (stringToValue(value, parsedValue)) {
                // Extract description from line if present (after #)
                std::string description = "";
                size_t commentPos = line.find('#');
                if (commentPos != std::string::npos) {
                    description = trim(line.substr(commentPos + 1));
                    // If we have a key=value format, we need to reconstruct the line
                    // For now, use the line itself as description
                }
                
                set(key, parsedValue, ConfigSource::File, description);
            } else {
                // If parsing fails, store as string
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
                
                // Try to parse the value
                ConfigValue parsedValue;
                if (stringToValue(value, parsedValue)) {
                    set(key, parsedValue, ConfigSource::CommandLine, "Command line argument");
                } else {
                    // If parsing fails, store as string
                    set(key, value, ConfigSource::CommandLine, "Command line argument");
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Try to parse the value
            ConfigValue parsedValue;
            if (stringToValue(value, parsedValue)) {
                set(key, parsedValue, ConfigSource::CommandLine, "Command line argument");
            } else {
                // If parsing fails, store as string
                set(key, value, ConfigSource::CommandLine, "Command line argument");
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
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        file << entry.key << " = " << valueToString(entry.value) << "\n";
        file << "\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->description = description; // Update description if key exists
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
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

bool Config::loadFromEnv(const std::string& envVar, ConfigSource source) {
    std::string value = getEnvVar(envVar);
    if (value.empty()) {
        return false;
    }
    
    std::string key = toLower(envVar);
    // Remove NLM_ prefix if present
    if (key.size() > 4 && key.substr(0, 4) == "nlm_") {
        key = key.substr(4);
    }
    
    set(key, value, source);
    return true;
}

// Get value with default and validation
template<typename T>
std::pair<T, bool> Config::getValidated(const std::string& key, const T& defaultValue,
                                       std::function<bool(const T&)> validator) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        return {defaultValue, validator ? validator(defaultValue) : true};
    }
    return {*val, validator ? validator(*val) : true};
}

// Check if key exists with validation
template<typename T>
bool Config::hasValidated(const std::string& key, std::function<bool(const T&)> validator) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        return false;
    }
    return !validator || validator(*val);
}

// Set with validation
template<typename T>
bool Config::setValidated(const std::string& key, const T& value,
                         std::function<bool(const T&)> validator,
                         ConfigSource source, const std::string& description) {
    if (validator && !validator(value)) {
        return false;
    }
    set(key, value, source, description);
    return true;
}

// Implementation of internal helpers
std::string Config::getEnvVar(const std::string& varName) {
    const char* val = std::getenv(varName.c_str());
    return val ? val : "";
}

bool Config::stringToValue(const std::string& str, ConfigValue& outValue) {
    if (isBoolean(str)) {
        bool val;
        if (stringToBool(str, val)) {
            outValue = val;
            return true;
        }
    } else if (isNumber(str)) {
        double val;
        if (stringToNumber(str, val)) {
            // Try to determine if it's an int or double based on decimal point
            if (str.find('.') != std::string::npos || str.find('e') != std::string::npos || str.find('E') != std::string::npos) {
                outValue = val;
            } else {
                // Try to parse as int64_t first, then int
                try {
                    size_t pos;
                    int64_t intVal = std::stoll(str, &pos);
                    if (pos == str.size()) {
                        outValue = intVal;
                        return true;
                    }
                } catch (...) {}
                outValue = static_cast<int>(std::stoi(str));
            }
            return true;
        }
    } else {
        // String value (trim quotes)
        std::string trimmed = trim(str);
        if ((trimmed.size() >= 2 && 
             ((trimmed.front() == '"' && trimmed.back() == '"') ||
              (trimmed.front() == '\'' && trimmed.back() == '\'')))) {
            trimmed = trimmed.substr(1, trimmed.size() - 2);
        }
        outValue = trimmed;
        return true;
    }
    return false;
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", "; oss << arg[i];
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", "; oss << arg[i];
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", "; oss << arg[i];
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", "; oss << arg[i];
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", "; oss << "\"" << arg[i] << "\"";
            } oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::isNumber(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }
    for (size_t i = start; i < str.size(); ++i) {
        if (!(std::isdigit(str[i]) || str[i] == '.' || str[i] == 'e' || str[i] == 'E' || str[i] == '-' || str[i] == '+')) {
            return false;
        }
    }
    return true;
}

bool Config::isBoolean(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || lower == "yes" || lower == "no" || 
           lower == "on" || lower == "off" || lower == "1" || lower == "0";
}

bool Config::stringToBool(const std::string& str, bool& outValue) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "yes" || lower == "on" || lower == "1") {
        outValue = true;
        return true;
    } else if (lower == "false" || lower == "no" || lower == "off" || lower == "0") {
        outValue = false;
        return true;
    }
    return false;
}

bool Config::stringToNumber(const std::string& str, double& outValue) {
    try {
        outValue = std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

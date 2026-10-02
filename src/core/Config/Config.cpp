#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cstring>
#include <cctype>

// Include JSON library if available
#ifdef HAS_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationRule> validationRules;
    bool strictValidation;
    
    Impl() : strictValidation(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced file loading with JSON/YAML support
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    
    // Try JSON first, then fallback to simple key=value format
#ifdef HAS_NLOHMANN_JSON
    try {
        auto json = nlohmann::json::parse(content);
        loadFromJson(json);
        return true;
    } catch (const nlohmann::json::parse_error&) {
        // JSON parsing failed, try simple format
    }
#endif
    
    // Fallback to simple key=value format
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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

// Enhanced command line loading with better parsing
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
        // Handle boolean flags like --enable-feature
        else if (arg.size() >= 3 && arg[0] == '-' && arg[1] == '-' && 
                 arg.find('=') == std::string::npos) {
            std::string key = arg.substr(2);
            set(key, true, ConfigSource::CommandLine);
        }
    }
    return true;
}

// Enhanced save with proper JSON/YAML output
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to create a pretty JSON representation
    std::ostringstream jsonStream;
    jsonStream << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            jsonStream << ",\n";
        }
        first = false;
        
        jsonStream << "  \"" << escapeJsonString(entry.key) << "\": ";
        
        std::visit([&jsonStream](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonStream << "\"" << escapeJsonString(val) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonStream << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || 
                                 std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t>) {
                jsonStream << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonStream << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) jsonStream << ", ";
                    jsonStream << val[j];
                }
                jsonStream << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonStream << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) jsonStream << ", ";
                    jsonStream << val[j];
                }
                jsonStream << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonStream << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) jsonStream << ", ";
                    jsonStream << "\"" << escapeJsonString(val[j]) << "\"";
                }
                jsonStream << "]";
            } else {
                jsonStream << "null"; // Fallback for unknown types
            }
        }, entry.value);
    }
    jsonStream << "\n}";
    
    file << jsonStream.str();
    return true;
}

// Enhanced get method with better type conversion
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        T result;
        if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(it->value)) {
                result = std::get<std::string>(it->value);
                return result;
            } else {
                // Try to convert from other types
                result = convertToString(it->value);
                return result;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(it->value)) {
                result = std::get<bool>(it->value);
                return result;
            } else {
                // Try to convert from other types
                result = convertToBool(it->value);
                return result;
            }
        } else if constexpr (std::is_same_v<T, int> || 
                             std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<T>(it->value)) {
                result = std::get<T>(it->value);
                return result;
            } else {
                // Try to convert from other types
                result = convertToNumber<T>(it->value);
                return result;
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(it->value)) {
                result = std::get<double>(it->value);
                return result;
            } else {
                // Try to convert from other types
                result = convertToDouble(it->value);
                return result;
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (std::holds_alternative<std::vector<int>>(it->value)) {
                result = std::get<std::vector<int>>(it->value);
                return result;
            } else {
                // Try to convert from other types
                result = convertToIntVector(it->value);
                return result;
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(it->value)) {
                result = std::get<std::vector<double>>(it->value);
                return result;
            } else {
                // Try to convert from other types
                result = convertToDoubleVector(it->value);
                return result;
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(it->value)) {
                result = std::get<std::vector<std::string>>(it->value);
                return result;
            } else {
                // Try to convert from other types
                result = convertToStringVector(it->value);
                return result;
            }
        }
        
        return std::nullopt;
        
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Enhanced getOr method with validation
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Enhanced set method with validation and source tracking
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the key (alphanumeric with underscores and dashes)
    if (!isValidKey(key)) {
        throw std::invalid_argument("Invalid configuration key: " + key);
    }
    
    // Validate the value against any existing validation rules
    auto rule = findValidationRule(key);
    if (rule && pImpl->strictValidation) {
        ValidationResult validation = rule->validator(value);
        if (!validation.valid) {
            throw std::invalid_argument("Configuration validation failed for key '" + key + 
                                      "': " + validation.errorMessage);
        }
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

// Enhanced has method
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

// Enhanced remove method
void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

// Enhanced getKeys method
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

// Enhanced clear method
void Config::clear() {
    pImpl->entries.clear();
}

// Enhanced summary method
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeForDisplay(val) << "\"";
            } else {
                oss << val;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    
    if (!pImpl->validationRules.empty()) {
        oss << "\nValidation Rules: (" << pImpl->validationRules.size() << ")\n";
        for (const auto& rule : pImpl->validationRules) {
            oss << "  - " << rule.key;
            if (!rule.errorMessage.empty()) {
                oss << " (error: " << rule.errorMessage << ")";
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

// Internal helper functions
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

// Additional helper methods for the enhanced configuration system
void Config::loadFromJson(const nlohmann::json& json) {
    if (!json.is_object()) {
        return;
    }
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        const auto& value = it.value();
        
        ConfigValue configValue;
        try {
            if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number()) {
                configValue = value.get<double>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                if (value.empty()) {
                    configValue = std::vector<double>{};
                } else if (value.front().is_number_integer()) {
                    configValue = value.get<std::vector<int64_t>>();
                } else if (value.front().is_number()) {
                    configValue = value.get<std::vector<double>>();
                } else {
                    configValue = value.get<std::vector<std::string>>();
                }
            } else {
                configValue = std::string(); // Skip unknown types
            }
            
            set(key, configValue, ConfigSource::File);
        } catch (const std::exception& e) {
            // Skip invalid entries but continue processing others
        }
    }
}

std::string Config::escapeJsonString(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 0x20 || c == 0x7f) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
        }
    }
    return oss.str();
}

std::string Config::escapeForDisplay(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        if (c == '\"') {
            oss << "\\\"";
        } else if (c == '\\') {
            oss << "\\\\";
        } else {
            oss << c;
        }
    }
    return oss.str();
}

bool Config::isValidKey(const std::string& key) {
    if (key.empty() || key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.") != std::string::npos) {
        return false;
    }
    return true;
}

ValidationRule* Config::findValidationRule(const std::string& key) {
    for (auto& rule : pImpl->validationRules) {
        if (rule.key == key) {
            return &rule;
        }
    }
    return nullptr;
}

// Template helper functions for type conversion
std::string Config::convertToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || 
                             std::is_same_v<T, int64_t>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << val[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << val[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << val[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

template<typename T>
T Config::convertToNumber(const ConfigValue& value) {
    T result = T();
    std::visit([&result](const auto& val) {
        using SrcType = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<SrcType, int> || 
                      std::is_same_v<SrcType, int64_t>) {
            result = static_cast<T>(val);
        } else if constexpr (std::is_same_v<SrcType, double>) {
            result = static_cast<T>(val);
        } else if constexpr (std::is_same_v<SrcType, bool>) {
            result = static_cast<T>(val ? 1 : 0);
        } else if constexpr (std::is_same_v<SrcType, std::string>) {
            result = stringToNumber<T>(val);
        }
    }, value);
    return result;
}

double Config::convertToDouble(const ConfigValue& value) {
    double result = 0.0;
    std::visit([&result](const auto& val) {
        using SrcType = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<SrcType, int> || 
                      std::is_same_v<SrcType, int64_t>) {
            result = static_cast<double>(val);
        } else if constexpr (std::is_same_v<SrcType, double>) {
            result = val;
        } else if constexpr (std::is_same_v<SrcType, bool>) {
            result = val ? 1.0 : 0.0;
        } else if constexpr (std::is_same_v<SrcType, std::string>) {
            result = stringToDouble(val);
        }
    }, value);
    return result;
}

bool Config::convertToBool(const ConfigValue& value) {
    bool result = false;
    std::visit([&result](const auto& val) {
        using SrcType = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<SrcType, bool>) {
            result = val;
        } else if constexpr (std::is_same_v<SrcType, int> || 
                             std::is_same_v<SrcType, int64_t>) {
            result = (val != 0);
        } else if constexpr (std::is_same_v<SrcType, double>) {
            result = (val != 0.0);
        } else if constexpr (std::is_same_v<SrcType, std::string>) {
            result = stringToBool(val);
        }
    }, value);
    return result;
}

std::vector<int> Config::convertToIntVector(const ConfigValue& value) {
    std::vector<int> result;
    std::visit([&result](const auto& val) {
        using SrcType = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<SrcType, std::vector<int>>) {
            result = val;
        } else if constexpr (std::is_same_v<SrcType, std::vector<double>>) {
            result.reserve(val.size());
            for (const auto& v : val) {
                result.push_back(static_cast<int>(v));
            }
        } else if constexpr (std::is_same_v<SrcType, std::vector<std::string>>) {
            result.reserve(val.size());
            for (const auto& v : val) {
                result.push_back(stringToInt(v));
            }
        }
    }, value);
    return result;
}

std::vector<double> Config::convertToDoubleVector(const ConfigValue& value) {
    std::vector<double> result;
    std::visit([&result](const auto& val) {
        using SrcType = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<SrcType, std::vector<double>>) {
            result = val;
        } else if constexpr (std::is_same_v<SrcType, std::vector<int>>) {
            result.reserve(val.size());
            for (const auto& v : val) {
                result.push_back(static_cast<double>(v));
            }
        } else if constexpr (std::is_same_v<SrcType, std::vector<std::string>>) {
            result.reserve(val.size());
            for (const auto& v : val) {
                result.push_back(stringToDouble(v));
            }
        }
    }, value);
    return result;
}

std::vector<std::string> Config::convertToStringVector(const ConfigValue& value) {
    std::vector<std::string> result;
    std::visit([&result](const auto& val) {
        using SrcType = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<SrcType, std::vector<std::string>>) {
            result = val;
        } else if constexpr (std::is_same_v<SrcType, std::vector<int>>) {
            result.reserve(val.size());
            for (const auto& v : val) {
                result.push_back(std::to_string(v));
            }
        } else if constexpr (std::is_same_v<SrcType, std::vector<double>>) {
            result.reserve(val.size());
            for (const auto& v : val) {
                result.push_back(std::to_string(v));
            }
        }
    }, value);
    return result;
}

template<typename T>
T Config::stringToNumber(const std::string& str) {
    std::istringstream iss(str);
    T result;
    iss >> result;
    return result;
}

double Config::stringToDouble(const std::string& str) {
    return std::stod(str);
}

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "yes" || lower == "on" || lower == "1") {
        return true;
    } else if (lower == "false" || lower == "no" || lower == "off" || lower == "0") {
        return false;
    }
    return false; // Default to false for unrecognized values
}

int Config::stringToInt(const std::string& str) {
    return std::stoi(str);
}

// Explicit template instantiations for the enhanced configuration system
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
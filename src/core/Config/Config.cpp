#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigChangeCallback> changeCallbacks;
    std::unordered_map<std::string, std::string> validationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    try {
        // Try JSON parsing first
        auto json = nlohmann::json::parse(content);
        
        clear();
        
        for (const auto& [key, value] : json.items()) {
            ConfigValue configValue;
            if (jsonToConfigValue(value, configValue)) {
                set(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error&) {
        // Fall back to simple key=value format
        file.open(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        clear();
        std::string line;
        while (std::getline(file, line)) {
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
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    }
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
    
    // Write as JSON format
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": " << valueToString(entry.value);
    }
    
    file << "\n}\n";
    
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
    setWithValidation(key, value, source);
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

void Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if key exists
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigValue oldValue;
    bool keyExists = it != pImpl->entries.end();
    
    if (keyExists) {
        oldValue = it->value;
    }
    
    // Validate the value
    validateKeyValue(key, value);
    
    // Update or add the entry
    if (keyExists) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Notify callbacks
    for (const auto& callback : pImpl->changeCallbacks) {
        callback(key, oldValue, value);
    }
}

void Config::addChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.push_back(callback);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
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
    pImpl->validationRules.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " 
        << pImpl->validationRules.size() << " validation rules):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    return oss.str();
}

void Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (entry.isValid()) {
            validateKeyValue(entry.key, entry.value);
        }
    }
}

void Config::addValidationRule(const std::string& key, const std::string& rule) {
    pImpl->validationRules[key] = rule;
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::stringToValue(const std::string& str, ConfigValue& outValue) {
    // Try to parse as different types
    if (str == "true" || str == "false") {
        outValue = (str == "true");
        return true;
    }
    
    try {
        // Try integer
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            outValue = intVal;
            return true;
        }
        
        // Try int64
        long long intVal64 = std::stoll(str, &pos);
        if (pos == str.size()) {
            outValue = intVal64;
            return true;
        }
        
        // Try double
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            outValue = doubleVal;
            return true;
        }
    } catch (...) {
        // Not a number, try as string (remove quotes)
        std::string result = str;
        if (result.size() >= 2 && ((result.front() == '"' && result.back() == '"') ||
                                  (result.front() == '\'' && result.back() == '\''))) {
            result = result.substr(1, result.size() - 2);
        }
        outValue = result;
        return true;
    }
    
    return false;
}

bool Config::parseJsonValue(const std::string& str, ConfigValue& outValue) {
    try {
        auto json = nlohmann::json::parse(str);
        if (jsonIsConfigValue(json, outValue)) {
            return true;
        }
    } catch (...) {
    }
    return false;
}

std::string Config::jsonValueToString(const nlohmann::json& j) {
    std::ostringstream oss;
    
    if (j.is_string()) {
        oss << "\"" << j.get<std::string>() << "\"";
    } else if (j.is_boolean()) {
        oss << (j.get<bool>() ? "true" : "false");
    } else if (j.is_number_integer()) {
        oss << j.get<int64_t>();
    } else if (j.is_number()) {
        oss << j.get<double>();
    } else if (j.is_array()) {
        oss << "[";
        for (size_t i = 0; i < j.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << jsonValueToString(j[i]);
        }
        oss << "]";
    } else {
        oss << j.dump();
    }
    
    return oss.str();
}

void Config::validateKeyValue(const std::string& key, const ConfigValue& value) const {
    auto ruleIt = pImpl->validationRules.find(key);
    if (ruleIt == pImpl->validationRules.end()) {
        return;
    }
    
    const std::string& rule = ruleIt->second;
    
    if (rule == "required" && key.empty()) {
        throw ConfigValidationError(key, "Configuration key cannot be empty");
    }
    
    if (rule.find("range") != std::string::npos) {
        // Parse range validation
        // Format: "range:min-max"
        size_t colonPos = rule.find("range:");
        if (colonPos != std::string::npos) {
            std::string rangeSpec = rule.substr(colonPos + 6);
            size_t dashPos = rangeSpec.find('-');
            if (dashPos != std::string::npos) {
                std::string minStr = rangeSpec.substr(0, dashPos);
                std::string maxStr = rangeSpec.substr(dashPos + 1);
                
                try {
                    double minVal = std::stod(minStr);
                    double maxVal = std::stod(maxStr);
                    
                    // Check value type and range
                    bool validRange = false;
                    std::visit([&](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, int> || 
                                    std::is_same_v<T, int64_t> || 
                                    std::is_same_v<T, double>) {
                            double val = (std::is_same_v<T, int> ? static_cast<double>(arg) :
                                         std::is_same_v<T, int64_t> ? static_cast<double>(arg) : arg);
                            validRange = (val >= minVal && val <= maxVal);
                        }
                    }, value);
                    
                    if (!validRange) {
                        throw ConfigValidationError(key, "Value " + valueToString(value) + " is not in range " + minStr + "-" + maxStr);
                    }
                } catch (...) {
                    // If range parsing fails, skip validation
                }
            }
        }
    }
}

std::string Config::getValueType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "not_found";
    }
    
    const ConfigValue& value = it->value;
    
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "int_array";
    if (std::holds_alternative<std::vector<double>>(value)) return "double_array";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string_array";
    
    return "unknown";
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

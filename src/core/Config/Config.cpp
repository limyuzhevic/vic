#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <iostream>

// nlohmann/json for serialization
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> keyToDescription; // For backward compatibility
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::unique_ptr<ConfigValidator>>>> validators;
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

    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    try {
        // Try to parse as JSON first
        auto json = nlohmann::json::parse(content);
        return parseJSON(json);
    } catch (const nlohmann::json::parse_error&) {
        // If JSON fails, try to parse as simple key=value format
        return parseSimpleFormat(content);
    }
}

bool Config::parseJSON(const nlohmann::json& json) {
    clear();
    
    if (!json.is_object()) {
        return false;
    }
    
    for (const auto& [key, value] : json.items()) {
        ConfigValue configValue;
        if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_number_integer()) {
            configValue = value.get<int>();
        } else if (value.is_number_unsigned()) {
            configValue = static_cast<int64_t>(value.get<uint64_t>());
        } else if (value.is_number_float()) {
            configValue = value.get<double>();
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_array()) {
            // Try to determine array type
            if (!value.empty()) {
                if (std::all_of(value.begin(), value.end(), 
                            [](const auto& v) { return v.is_number_integer(); })) {
                    configValue = value.get<std::vector<int>>();
                } else if (std::all_of(value.begin(), value.end(),
                            [](const auto& v) { return v.is_number_float(); })) {
                    configValue = value.get<std::vector<double>>();
                } else if (std::all_of(value.begin(), value.end(),
                            [](const auto& v) { return v.is_string(); })) {
                    configValue = value.get<std::vector<std::string>>();
                } else {
                    // Mixed types - store as string
                    configValue = value.dump();
                }
            }
        } else {
            // For null or other types
            configValue = "";
        }
        
        // Check for description field
        std::string description = "";
        auto descIt = json.find("_description_");
        if (descIt != json.end() && descIt->is_object()) {
            auto keyDesc = descIt->find(key);
            if (keyDesc != descIt->end() && keyDesc->is_string()) {
                description = keyDesc->get<std::string>();
            }
        }
        
        // Validate before setting
        validateKey(key, configValue);
        
        set(key, configValue, ConfigSource::File);
        if (!description.empty()) {
            pImpl->keyToDescription[key] = description;
        }
    }
    
    return true;
}

bool Config::parseSimpleFormat(const std::string& content) {
    clear();
    
    std::istringstream stream(content);
    std::string line;
    std::string currentDescription;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty()) continue;
        
        // Skip comments
        if (line[0] == '#' || line[0] == '/') {
            currentDescription = line.substr(1);
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            ConfigValue configValue;
            
            // Parse numeric values
            try {
                if (valueStr.find('.') != std::string::npos) {
                    configValue = std::stod(valueStr);
                } else {
                    configValue = std::stoll(valueStr);
                }
            } catch (...) {
                // Check for boolean
                if (valueStr == "true" || valueStr == "false") {
                    configValue = (valueStr == "true");
                } else {
                    // String value - remove quotes
                    if (valueStr.size() >= 2 && 
                        ((valueStr.front() == '"' && valueStr.back() == '"') ||
                         (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                        valueStr = valueStr.substr(1, valueStr.size() - 2);
                    }
                    configValue = valueStr;
                }
            }
            
            // Validate before setting
            validateKey(key, configValue);
            
            set(key, configValue, ConfigSource::File);
            if (!currentDescription.empty()) {
                pImpl->keyToDescription[key] = currentDescription;
                currentDescription.clear();
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
    
    nlohmann::json json;
    
    // Add description metadata
    if (!pImpl->keyToDescription.empty()) {
        nlohmann::json descJson;
        for (const auto& [key, desc] : pImpl->keyToDescription) {
            descJson[key] = desc;
        }
        json["_description_"] = descJson;
    }
    
    // Add configuration values
    for (const auto& entry : pImpl->entries) {
        nlohmann::json valueJson;
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueJson = val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                valueJson = val;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueJson = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueJson = val;
            } else {
                valueJson = val;
            }
        }, entry.value);
        
        json[entry.key] = valueJson;
    }
    
    file << json.dump(4) << std::endl;
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
    pImpl->keyToDescription.erase(key);
    pImpl->validators.erase(key);
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
    pImpl->keyToDescription.clear();
    pImpl->validators.clear();
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
                    if (i > 0) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << arg[i] << "\"";
                    } else {
                        oss << arg[i];
                    }
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

void Config::addValidator(const std::string& key, std::unique_ptr<ConfigValidator> validator) {
    pImpl->validators[key].emplace_back(validator->name(), std::move(validator));
}

void Config::removeValidator(const std::string& key) {
    pImpl->validators.erase(key);
}

void Config::clearValidators(const std::string& key) {
    if (auto it = pImpl->validators.find(key); it != pImpl->validators.end()) {
        it->second.clear();
    }
}

void Config::clearAllValidators() {
    pImpl->validators.clear();
}

const std::vector<std::pair<std::string, std::unique_ptr<ConfigValidator>>>&
Config::getValidators(const std::string& key) const {
    static const std::vector<std::pair<std::string, std::unique_ptr<ConfigValidator>>> empty;
    auto it = pImpl->validators.find(key);
    return (it != pImpl->validators.end()) ? it->second : empty;
}

void Config::validateKey(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->validators.find(key);
    if (it != pImpl->validators.end()) {
        for (const auto& validatorPair : it->second) {
            validatorPair.second->validate(key, value);
        }
    }
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

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    nlohmann::json jsonData;
    bool parseSuccess = false;
    
    // Try JSON parsing first
    try {
        jsonData = nlohmann::json::parse(content);
        parseSuccess = convertJsonToConfig(jsonData, pImpl->entries);
    } catch (const nlohmann::json::parse_error& e) {
        // Try YAML parsing
        try {
            YAML::Node yamlNode = YAML::Load(content);
            // Convert YAML to JSON
            convertYamlNode(yamlNode, jsonData);
            parseSuccess = convertJsonToConfig(jsonData, pImpl->entries);
        } catch (const YAML::ParserException& e) {
            parseSuccess = false;
        }
    }
    
    return parseSuccess;
}

bool Config::loadFromArgs(int argc, char** argv) noexcept {
    for (int i = 1; i < argc; ++i) {
        auto arg = std::string(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            auto pos = arg.find('=');
            if (pos != std::string::npos) {
                auto key = arg.substr(2, pos - 2);
                auto value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            auto key = arg.substr(1);
            auto value = std::string(argv[++i]);
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
    
    // Convert config entries to JSON
    nlohmann::json j = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        try {
            // Create nested structure if key contains dots
            size_t dotPos = entry.key.find('.');
            if (dotPos != std::string::npos) {
                std::string parentKey = entry.key.substr(0, dotPos);
                std::string childKey = entry.key.substr(dotPos + 1);
                
                if (!j.contains(parentKey)) {
                    j[parentKey] = nlohmann::json::object();
                }
                
                // For nested values, we need to handle them differently
                j[parentKey][childKey] = configValueToString(entry.value);
            } else {
                j[entry.key] = configValueToString(entry.value);
            }
        } catch (const std::exception&) {
            // Fallback to simple string representation
            j[entry.key] = configValueToString(entry.value);
        }
    }
    
    file << j.dump(2);
    return true;
}

void Config::convertYamlNode(const YAML::Node& node, nlohmann::json& result) {
    if (node.IsScalar()) {
        std::string value = node.as<std::string>();
        
        // Try to convert to appropriate type
        try {
            size_t pos;
            long long intValue = std::stoll(value, &pos);
            if (pos == value.size()) {
                result = intValue;
                return;
            }
        } catch (...) {}
        
        try {
            size_t pos;
            double doubleValue = std::stod(value, &pos);
            if (pos == value.size()) {
                result = doubleValue;
                return;
            }
        } catch (...) {}
        
        if (value == "true") {
            result = true;
            return;
        } else if (value == "false") {
            result = false;
            return;
        }
        
        result = value;
    } else if (node.IsSequence()) {
        result = nlohmann::json::array();
        for (const auto& item : node) {
            convertYamlNode(item, result[result.size()]);
        }
    } else if (node.IsMap()) {
        result = nlohmann::json::object();
        for (const auto& it : node) {
            result[it.first.as<std::string>()] = nlohmann::json();
            convertYamlNode(it.second, result[it.first.as<std::string>()]);
        }
    }
}

bool Config::convertJsonToConfig(const nlohmann::json& j, std::vector<ConfigEntry>& entries) {
    entries.clear();
    
    for (const auto& [key, value] : j.items()) {
        std::string strValue = value.dump();
        ConfigValue configValue = stringToConfigValue(strValue);
        ConfigEntry entry(key, configValue, ConfigSource::File);
        
        if (validateConfigEntry(entry)) {
            entries.push_back(std::move(entry));
        } else {
            return false;
        }
    }
    return true;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::string result;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            result = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        }
    }, value);
    return result;
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    // Try to determine type based on content
    if (str.empty()) {
        return std::string("");
    }
    
    // Try int
    try {
        size_t pos;
        long long intValue = std::stoll(str, &pos);
        if (pos == str.size()) {
            return intValue;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleValue = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleValue;
        }
    } catch (...) {}
    
    // Try bool
    if (str == "true") {
        return true;
    } else if (str == "false") {
        return false;
    }
    
    // Default to string
    return str;
}

bool Config::validateConfigEntry(const ConfigEntry& entry) {
    // Basic validation - can be expanded
    if (entry.key.empty()) {
        return false;
    }
    
    return true;
}

// Implementation of template methods
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
    return val.has_value() ? std::move(*val) : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) noexcept {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) noexcept {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) noexcept {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) noexcept {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) noexcept {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const noexcept {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) noexcept {
    auto newEnd = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    pImpl->entries.erase(newEnd, pImpl->entries.end());
}

std::vector<std::string> Config::getKeys() const noexcept {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() noexcept {
    pImpl->entries.clear();
}

std::string Config::summary() const noexcept {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\\\"" << arg << "\\\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) noexcept {
    size_t start = str.find_first_not_of(" \\t\\r\\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \\t\\r\\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) noexcept {
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

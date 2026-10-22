#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

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
    
    // Try to parse as JSON first
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        file.close();
        
        // Clear existing entries
        clear();
        
        // Parse JSON recursively
        fromJson(jsonData, ConfigSource::File);
        return true;
    } catch (const std::exception&) {
        // Not valid JSON, try old key=value format
        file.clear();
        file.seekg(0, std::ios::beg);
    }
    
    // Fallback to simple key=value format
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
    nlohmann::json jsonData;
    
    // Convert all entries to JSON
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = entry.value;  // Use ConfigValue's JSON conversion
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write formatted JSON with comments
    file << jsonData.dump(2); // 2-space indentation
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

// Helper method to convert JSON to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& json) {
    if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_number_integer()) {
        return json.get<int64_t>();
    } else if (json.is_number_unsigned()) {
        return json.get<uint64_t>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_array()) {
        // Determine array type based on first element
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int64_t> intArray;
                for (const auto& item : json) {
                    intArray.push_back(item.get<int64_t>());
                }
                return intArray;
            } else if (json[0].is_number_float()) {
                std::vector<double> doubleArray;
                for (const auto& item : json) {
                    doubleArray.push_back(item.get<double>());
                }
                return doubleArray;
            } else if (json[0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : json) {
                    stringArray.push_back(item.get<std::string>());
                }
                return stringArray;
            }
        }
        // Default to string array
        std::vector<std::string> stringArray;
        for (const auto& item : json) {
            stringArray.push_back(item.get<std::string>());
        }
        return stringArray;
    }
    return json.get<std::string>(); // Fallback to string
}

// Helper method to convert ConfigValue to JSON
void configValueToJson(const ConfigValue& value, nlohmann::json& json) {
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int> >) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double> >) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string> >) {
            json = arg;
        }
    }, value);
}

nlohmann::json Config::toJson() const {
    nlohmann::json json;
    for (const auto& entry : pImpl->entries) {
        nlohmann::json jsonValue;
        configValueToJson(entry.value, jsonValue);
        json[entry.key] = jsonValue;
    }
    return json;
}

Config Config::fromJson(const nlohmann::json& json, ConfigSource source) {
    Config config;
    for (const auto& [key, value] : json.items()) {
        ConfigValue configValue = jsonToConfigValue(value);
        config.set(key, configValue, source);
    }
    return config;
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

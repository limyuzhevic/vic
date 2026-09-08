#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <limits>

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
    
    try {
        // Parse JSON file
        auto j = nlohmann::json::parse(file);
        pImpl->entries.clear();
        parseJsonObject(j, pImpl->entries);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
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
    
    nlohmann::json j = nlohmann::json::array();
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json entryJson;
        entryJson["key"] = entry.key;
        entryJson["description"] = entry.description;
        entryJson["value"] = convertConfigValueToJson(entry.value);
        entryJson["source"] = static_cast<int>(entry.source);
        j.push_back(entryJson);
    }
    
    file << j.dump(2);
    return true;
}

// Convert ConfigValue to JSON
template<typename T>
nlohmann::json toJson(const T& value) {
    return value;
}

nlohmann::json Config::convertConfigValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        return toJson(arg);
    }, value);
}

// Convert JSON to ConfigValue
void Config::convertJsonToConfigValue(const nlohmann::json& j, ConfigValue& value) {
    if (j.is_string()) {
        value = j.get<std::string>();
    } else if (j.is_number_integer()) {
        int64_t intVal = j.get<int64_t>();
        if (intVal >= std::numeric_limits<int>::min() && 
            intVal <= std::numeric_limits<int>::max()) {
            value = static_cast<int>(intVal);
        } else {
            value = intVal;
        }
    } else if (j.is_number_float()) {
        value = j.get<double>();
    } else if (j.is_boolean()) {
        value = j.get<bool>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                value = j.get<std::vector<int>>();
            } else if (j[0].is_number_float()) {
                value = j.get<std::vector<double>>();
            } else if (j[0].is_string()) {
                value = j.get<std::vector<std::string>>();
            } else {
                throw std::runtime_error("Unsupported array element type in JSON");
            }
        } else {
            value = std::vector<std::string>();
        }
    } else {
        throw std::runtime_error("Unsupported JSON type for configuration value");
    }
}

void Config::parseJsonObject(const nlohmann::json& j, std::vector<ConfigEntry>& entries) {
    if (j.is_array()) {
        for (const auto& item : j) {
            ConfigEntry entry;
            entry.key = item.value("key", "");
            entry.description = item.value("description", "");
            entry.source = static_cast<ConfigSource>(item.value("source", 0));
            convertJsonToConfigValue(item["value"], entry.value);
            entries.push_back(entry);
        }
    } else if (j.is_object()) {
        for (const auto& [key, value] : j.items()) {
            ConfigEntry entry;
            entry.key = key;
            entry.description = "";
            entry.source = ConfigSource::File;
            convertJsonToConfigValue(value, entry.value);
            entries.push_back(entry);
        }
    } else {
        throw std::runtime_error("Invalid JSON format for configuration file");
    }
}

// Implementation of template get methods
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

// Implementation of template getOr methods
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Set methods implementation
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

// Other methods
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

// Helper function implementations
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

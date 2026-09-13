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
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        clear();
        
        if (!jsonData.is_object()) {
            return false;
        }
        
        for (const auto& [key, value] : jsonData.items()) {
            ConfigValue configValue;
            ConfigSource source = ConfigSource::File;
            std::string description = "";
            
            if (value.is_object() && value.contains("value") && value.contains("source")) {
                // Object with metadata
                if (value["value"].is_string()) {
                    configValue = value["value"].get<std::string>();
                } else if (value["value"].is_number_integer()) {
                    configValue = value["value"].get<int>();
                } else if (value["value"].is_number_unsigned()) {
                    configValue = value["value"].get<int64_t>();
                } else if (value["value"].is_number()) {
                    configValue = value["value"].get<double>();
                } else if (value["value"].is_boolean()) {
                    configValue = value["value"].get<bool>();
                } else if (value["value"].is_array()) {
                    // Try to determine array element type
                    auto arr = value["value"].get<std::vector<nlohmann::json>>();
                    if (arr.empty()) {
                        configValue = std::vector<int>{};
                    } else {
                        // Check if all elements are numbers
                        bool allInt = true;
                        for (const auto& elem : arr) {
                            if (!elem.is_number_integer()) {
                                allInt = false;
                                break;
                            }
                        }
                        if (allInt) {
                            configValue = arr.get<std::vector<int>>();
                        } else {
                            // Check if all elements are floats
                            bool allFloat = true;
                            for (const auto& elem : arr) {
                                if (!elem.is_number()) {
                                    allFloat = false;
                                    break;
                                }
                            }
                            if (allFloat) {
                                configValue = arr.get<std::vector<double>>();
                            } else {
                                configValue = arr.get<std::vector<std::string>>();
                            }
                        }
                    }
                }
                
                if (value.contains("source")) {
                    int sourceVal = value["source"].get<int>();
                    if (sourceVal >= 0 && sourceVal <= 3) {
                        source = static_cast<ConfigSource>(sourceVal);
                    }
                }
                
                if (value.contains("description")) {
                    description = value["description"].get<std::string>();
                }
            } else {
                // Simple value
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    auto arr = value.get<std::vector<nlohmann::json>>();
                    if (arr.empty()) {
                        configValue = std::vector<int>{};
                    } else {
                        bool allInt = true;
                        for (const auto& elem : arr) {
                            if (!elem.is_number_integer()) {
                                allInt = false;
                                break;
                            }
                        }
                        if (allInt) {
                            configValue = arr.get<std::vector<int>>();
                        } else {
                            bool allFloat = true;
                            for (const auto& elem : arr) {
                                if (!elem.is_number()) {
                                    allFloat = false;
                                    break;
                                }
                            }
                            if (allFloat) {
                                configValue = arr.get<std::vector<double>>();
                            } else {
                                configValue = arr.get<std::vector<std::string>>();
                            }
                        }
                    }
                } else {
                    return false; // Unsupported value type
                }
            }
            
            set(key, configValue, source);
            setDescription(key, description);
        }
        
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::setDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

bool Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    return "";
}

bool Config::hasDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end() && !it->description.empty();
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
    
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json entryJson;
        
        if (entry.description.empty()) {
            // Simple value entry
            if (std::holds_alternative<int>(entry.value)) {
                entryJson = entry.value.get<int>();
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                entryJson = entry.value.get<int64_t>();
            } else if (std::holds_alternative<double>(entry.value)) {
                entryJson = entry.value.get<double>();
            } else if (std::holds_alternative<bool>(entry.value)) {
                entryJson = entry.value.get<bool>();
            } else if (std::holds_alternative<std::string>(entry.value)) {
                entryJson = entry.value.get<std::string>();
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                entryJson = entry.value.get<std::vector<int>>();
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                entryJson = entry.value.get<std::vector<double>>();
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                entryJson = entry.value.get<std::vector<std::string>>();
            }
        } else {
            // Object with metadata
            nlohmann::json valueJson;
            if (std::holds_alternative<int>(entry.value)) {
                valueJson = entry.value.get<int>();
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                valueJson = entry.value.get<int64_t>();
            } else if (std::holds_alternative<double>(entry.value)) {
                valueJson = entry.value.get<double>();
            } else if (std::holds_alternative<bool>(entry.value)) {
                valueJson = entry.value.get<bool>();
            } else if (std::holds_alternative<std::string>(entry.value)) {
                valueJson = entry.value.get<std::string>();
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                valueJson = entry.value.get<std::vector<int>>();
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                valueJson = entry.value.get<std::vector<double>>();
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                valueJson = entry.value.get<std::vector<std::string>>();
            }
            
            entryJson["value"] = valueJson;
            entryJson["source"] = static_cast<int>(entry.source);
            entryJson["description"] = entry.description;
        }
        
        jsonData[entry.key] = entryJson;
    }
    
    file << jsonData.dump(2) << std::endl;
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

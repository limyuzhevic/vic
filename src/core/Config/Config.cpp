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
    // Clear existing entries
    clear();
    
    try {
        // Parse JSON file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Convert JSON to ConfigEntry vector
        if (jsonData.is_object()) {
            for (const auto& [key, value] : jsonData.items()) {
                ConfigValue configValue;
                ConfigSource source = ConfigSource::File;
                
                // Convert JSON value to ConfigValue
                if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_array()) {
                    // Check if it's an int array
                    bool isIntArray = true;
                    std::vector<int64_t> intArray;
                    for (const auto& item : value) {
                        if (item.is_number_integer()) {
                            intArray.push_back(item.get<int64_t>());
                        } else {
                            isIntArray = false;
                            break;
                        }
                    }
                    if (isIntArray) {
                        configValue = intArray;
                    } else {
                        // Check if it's a double array
                        bool isDoubleArray = true;
                        std::vector<double> doubleArray;
                        for (const auto& item : value) {
                            if (item.is_number_float()) {
                                doubleArray.push_back(item.get<double>());
                            } else {
                                isDoubleArray = false;
                                break;
                            }
                        }
                        if (isDoubleArray) {
                            configValue = doubleArray;
                        } else {
                            // Check if it's a string array
                            bool isStringArray = true;
                            std::vector<std::string> stringArray;
                            for (const auto& item : value) {
                                if (item.is_string()) {
                                    stringArray.push_back(item.get<std::string>());
                                } else {
                                    isStringArray = false;
                                    break;
                                }
                            }
                            if (isStringArray) {
                                configValue = stringArray;
                            } else {
                                // Fallback to storing as JSON string
                                configValue = value.dump();
                            }
                        }
                    }
                } else {
                    // Store as JSON string
                    configValue = value.dump();
                }
                
                pImpl->entries.emplace_back(key, configValue, source, "");
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse config file: " + std::string(e.what()));
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
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            // Convert ConfigValue to JSON
            nlohmann::json jsonValue;
            
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonValue = arg;
                } else {
                    jsonValue = "";
                }
            }, entry.value);
            
            jsonData[entry.key] = jsonValue;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonData.dump(2); // Pretty print with 2-space indentation
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save config file: " + std::string(e.what()));
        return false;
    }
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
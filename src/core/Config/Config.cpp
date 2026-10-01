#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <cerrno>

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
        throw ConfigFileError("Cannot open file: " + filepath);
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        if (!jsonData.is_object()) {
            throw ConfigParseError("JSON data is not an object");
        }
        
        // Clear existing entries
        clear();
        
        // Parse JSON object
        for (const auto& [key, value] : jsonData.items()) {
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        throw ConfigParseError(std::string("JSON parsing error: ") + e.what());
    } catch (const std::exception& e) {
        throw ConfigFileError(std::string("Error reading file ") + filepath + ": " + e.what());
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
        throw ConfigFileError("Cannot open file for writing: " + filepath + ": " + std::strerror(errno));
    }
    
    try {
        nlohmann::json jsonData;
        
        // Convert config entries to JSON
        for (const auto& entry : pImpl->entries) {
            // Convert ConfigValue to nlohmann::json
            nlohmann::json jsonValue;
            std::visit([&jsonValue](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonValue = value;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonValue = value;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonValue = value;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonValue = value;
                } else {
                    // For basic types, use to_json which is automatically defined for primitive types
                    jsonValue = value;
                }
            }, entry.value);
            
            jsonData[entry.key] = jsonValue;
        }
        
        // Write JSON to file with indentation for readability
        file << jsonData.dump(4);
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        throw ConfigParseError(std::string("JSON serialization error: ") + e.what());
    } catch (const std::exception& e) {
        throw ConfigFileError(std::string("Error writing to file ") + filepath + ": " + e.what());
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    T result;
    try {
        std::visit([&result](auto&& value) {
            using U = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<U, T>) {
                result = std::move(value);
            } else if constexpr (std::is_same_v<T, std::string> && std::is_same_v<U, std::string>) {
                result = std::move(value);
            } else if constexpr (std::is_same_v<T, int64_t> && (std::is_same_v<U, int> || std::is_same_v<U, int64_t>)) {
                result = static_cast<T>(value);
            } else if constexpr (std::is_same_v<T, std::vector<int>> && std::is_same_v<U, std::vector<int>>) {
                result = std::move(value);
            } else if constexpr (std::is_same_v<T, std::vector<double>> && std::is_same_v<U, std::vector<double>>) {
                result = std::move(value);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>> && std::is_same_v<U, std::vector<std::string>>) {
                result = std::move(value);
            } else {
                throw std::bad_variant_access();
            }
        }, it->value);
        return result;
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    ConfigValue configValue;
    
    try {
        if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_number_integer()) {
            // Try int64_t first to handle large integers
            try {
                configValue = value.get<int64_t>();
            } catch (...) {
                configValue = value.get<int>();
            }
        } else if (value.is_number_unsigned()) {
            configValue = value.get<uint64_t>();
        } else if (value.is_number()) {
            // Try double first, then check if it's actually an integer
            double doubleVal = value.get<double>();
            if (doubleVal == static_cast<int64_t>(doubleVal)) {
                configValue = static_cast<int64_t>(doubleVal);
            } else {
                configValue = doubleVal;
            }
        } else if (value.is_array()) {
            // Handle arrays
            if (!value.empty()) {
                const auto& firstElement = value[0];
                if (firstElement.is_string()) {
                    std::vector<std::string> vec = value.get<std::vector<std::string>>();
                    configValue = std::move(vec);
                } else if (firstElement.is_number_integer()) {
                    std::vector<int> vec = value.get<std::vector<int>>();
                    configValue = std::move(vec);
                } else if (firstElement.is_number()) {
                    std::vector<double> vec = value.get<std::vector<double>>();
                    configValue = std::move(vec);
                } else {
                    throw ConfigParseError("Unsupported array element type in JSON configuration");
                }
            } else {
                throw ConfigParseError("Empty array in JSON configuration");
            }
        } else {
            throw ConfigParseError("Unsupported JSON type in configuration");
        }
        
        set(key, configValue, source);
    } catch (const nlohmann::json::exception& e) {
        throw ConfigParseError(std::string("JSON parsing error: ") + e.what());
    } catch (const std::exception& e) {
        throw ConfigParseError(std::string("Error converting JSON value: ") + e.what());
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

// Helper function to convert ConfigValue to JSON for saveToFile
static nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json jsonValue;
    std::visit([&jsonValue](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = val;
        }
    }, value);
    return jsonValue;
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

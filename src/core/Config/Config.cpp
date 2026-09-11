// Include the main Config header
#include "Config.hpp"

// Include nlohmann::json for JSON parsing
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, ConfigEntry> configMap;
    
    Impl() = default;
};

Config::Config() : pImpl(new Impl) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

// Include the main Config header
#include "Config.hpp"

// Include nlohmann::json for JSON parsing
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, ConfigEntry> configMap;
    
    Impl() = default;
};

Config::Config() : pImpl(new Impl) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Log the attempt to load config from file
        Logger::getGlobal().info("Attempting to load config from file: " + filepath, "", 0, "loadFromFile");
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            Logger::getGlobal().warning("Failed to open config file: " + filepath, "", 0, "loadFromFile");
            return false;
        }
        
        // Check if file is empty
        file.seekg(0, std::ios::end);
        if (file.tellg() == 0) {
            Logger::getGlobal().warning("Config file is empty: " + filepath, "", 0, "loadFromFile");
            clear();
            return true; // Empty file is valid - means no config loaded
        }
        file.seekg(0, std::ios::beg);
        
        json j;
        try {
            file >> j;
        } catch (const json::parse_error& e) {
            Logger::getGlobal().error("Failed to parse JSON config file " + filepath + ": " + e.what(), "", 0, "loadFromFile");
            // Try to continue with default values instead of failing completely
            clear();
            return true; // Successfully cleared (fallback)
        }
        
        clear();
        
        int successCount = 0;
        int errorCount = 0;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            const std::string& key = it.key();
            const json& valueJson = it.value();
            
            ConfigValue configValue;
            bool converted = false;
            
            // Try to convert JSON to ConfigValue based on type
            try {
                if (valueJson.is_number_integer()) {
                    configValue = valueJson.get<int64_t>();
                    converted = true;
                } else if (valueJson.is_number()) {
                    configValue = valueJson.get<double>();
                    converted = true;
                } else if (valueJson.is_boolean()) {
                    configValue = valueJson.get<bool>();
                    converted = true;
                } else if (valueJson.is_string()) {
                    configValue = valueJson.get<std::string>();
                    converted = true;
                } else if (valueJson.is_array()) {
                    // Handle arrays
                    const json& arr = valueJson;
                    // Determine element type
                    if (!arr.empty()) {
                        if (arr[0].is_number_integer()) {
                            std::vector<int64_t> intVec;
                            for (const auto& elem : arr) {
                                intVec.push_back(elem.get<int64_t>());
                            }
                            configValue = intVec;
                            converted = true;
                        } else if (arr[0].is_number()) {
                            std::vector<double> doubleVec;
                            for (const auto& elem : arr) {
                                doubleVec.push_back(elem.get<double>());
                            }
                            configValue = doubleVec;
                            converted = true;
                        } else if (arr[0].is_string()) {
                            std::vector<std::string> stringVec;
                            for (const auto& elem : arr) {
                                stringVec.push_back(elem.get<std::string>());
                            }
                            configValue = stringVec;
                            converted = true;
                        }
                    }
                }
            } catch (const std::exception& e) {
                Logger::getGlobal().warning("Failed to convert JSON value for key '" + key + "': " + e.what(), "", 0, "loadFromFile");
                errorCount++;
                continue;
            }
            
            if (converted) {
                try {
                    set(key, configValue, ConfigSource::File);
                    successCount++;
                } catch (const std::exception& e) {
                    Logger::getGlobal().error("Failed to set config key '" + key + "': " + e.what(), "", 0, "loadFromFile");
                    errorCount++;
                }
            } else {
                Logger::getGlobal().warning("Failed to convert JSON value for key '" + key + "' - unknown type", "", 0, "loadFromFile");
                errorCount++;
            }
        }
        
        Logger::getGlobal().info("Config loaded from file " + filepath + ": " + std::to_string(successCount) + " entries loaded, " + std::to_string(errorCount) + " errors", "", 0, "loadFromFile");
        
        return !j.empty(); // Return true if we attempted to load (even with errors)
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Unexpected error loading config from file " + filepath + ": " + e.what(), "", 0, "loadFromFile");
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool loaded = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(0, pos);
            std::string valueStr = arg.substr(pos + 1);
            
            // Try to parse value as different types
            bool parsed = false;
            
            // Try int
            try {
                size_t posEnd;
                int64_t intVal = std::stoll(valueStr, &posEnd);
                if (posEnd == valueStr.length()) {
                    set(key, intVal, ConfigSource::CommandLine);
                    loaded = true;
                    parsed = true;
                }
            } catch (...) {}
            
            // Try double
            if (!parsed) {
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(valueStr, &posEnd);
                    if (posEnd == valueStr.length()) {
                        set(key, doubleVal, ConfigSource::CommandLine);
                        loaded = true;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try bool
            if (!parsed) {
                if (valueStr == "true" || valueStr == "1") {
                    set(key, true, ConfigSource::CommandLine);
                    loaded = true;
                    parsed = true;
                } else if (valueStr == "false" || valueStr == "0") {
                    set(key, false, ConfigSource::CommandLine);
                    loaded = true;
                    parsed = true;
                }
            }
            
            // Try string (fallback)
            if (!parsed) {
                set(key, valueStr, ConfigSource::CommandLine);
                loaded = true;
            }
        }
    }
    
    return loaded;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        json j;
        
        for (const auto& pair : pImpl->configMap) {
            const ConfigEntry& entry = pair.second;
            
            // Convert ConfigValue to JSON
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                
                if constexpr (std::is_same_v<T, int64_t>) {
                    j[pair.first] = val;
                } else if constexpr (std::is_same_v<T, double>) {
                    j[pair.first] = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    j[pair.first] = val;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    j[pair.first] = val;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    json arr = json::array();
                    for (const auto& item : val) {
                        arr.push_back(item);
                    }
                    j[pair.first] = arr;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    json arr = json::array();
                    for (const auto& item : val) {
                        arr.push_back(item);
                    }
                    j[pair.first] = arr;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    json arr = json::array();
                    for (const auto& item : val) {
                        arr.push_back(item);
                    }
                    j[pair.first] = arr;
                }
            }, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2); // Pretty print with 2-space indentation
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config to file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->configMap.find(key);
    if (it == pImpl->configMap.end()) {
        return std::nullopt;
    }
    
    const ConfigValue& value = it->second.value;
    
    try {
        if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(value)) {
                return std::get<int64_t>(value);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(value)) {
                return std::get<std::string>(value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            if (std::holds_alternative<std::vector<int64_t>>(value)) {
                return std::get<std::vector<int64_t>>(value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(value)) {
                return std::get<std::vector<double>>(value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(value)) {
                return std::get<std::vector<std::string>>(value);
            }
        }
    } catch (...) {}
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return *val;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Key cannot be empty");
    }
    
    pImpl->configMap[key] = ConfigEntry(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(static_cast<int64_t>(value)), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return pImpl->configMap.find(key) != pImpl->configMap.end();
}

void Config::remove(const std::string& key) {
    pImpl->configMap.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->configMap.size());
    
    for (const auto& pair : pImpl->configMap) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

void Config::clear() {
    pImpl->configMap.clear();
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Config Summary (" << pImpl->configMap.size() << " entries):" << std::endl;
    
    for (const auto& pair : pImpl->configMap) {
        const ConfigEntry& entry = pair.second;
        ss << "  " << pair.first << " = ";
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, int64_t>) {
                ss << val;
            } else if constexpr (std::is_same_v<T, double>) {
                ss << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                ss << '"' << val << '"';
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                ss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << val[i];
                }
                ss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                ss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << val[i];
                }
                ss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                ss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << '"' << val[i] << '"';
                }
                ss << "]";
            }
        }, entry.value);
        
        ss << " (from ";
        switch (entry.source) {
            case ConfigSource::Default: ss << "Default"; break;
            case ConfigSource::File: ss << "File"; break;
            case ConfigSource::CommandLine: ss << "Command Line"; break;
            case ConfigSource::Runtime: ss << "Runtime"; break;
        }
        ss << ")" << std::endl;
    }
    
    return ss.str();
}

std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace nlm
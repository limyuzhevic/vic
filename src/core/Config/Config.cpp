#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <limits>
#include <cstdint>

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
    
    // Parse JSON file using nlohmann::json
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Clear existing entries
        clear();
        
        // Process JSON object
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            const auto& value = it.value();
            
            // Convert JSON value to ConfigValue based on type
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else         if (value.is_number_integer()) {
                // Check if it fits into int64_t (handles large numbers)
                if (value.get<int64_t>() <= std::numeric_limits<int>::max()) {
                    configValue = value.get<int>();
                } else {
                    configValue = value.get<int64_t>();
                }
            } else if (value.is_number_unsigned()) {
                // Handle unsigned numbers - convert to int64_t
                uint64_t unsignedVal = value.get<uint64_t>();
                if (unsignedVal <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
                    configValue = static_cast<int64_t>(unsignedVal);
                } else {
                    // If too large for int64_t, use string representation
                    configValue = std::to_string(unsignedVal);
                }
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle arrays
                if (value.empty()) {
                    configValue = std::vector<int>{};
                } else {
                    // Check array element type
                    bool allInts = true;
                    bool allDoubles = true;
                    bool allStrings = true;
                    
                    for (const auto& item : value) {
                        if (!item.is_number_integer()) allInts = false;
                        if (!item.is_number_float()) allDoubles = false;
                        if (!item.is_string()) allStrings = false;
                    }
                    
                    if (allInts) {
                        std::vector<int> intArray;
                        intArray.reserve(value.size());
                        for (const auto& item : value) {
                            intArray.push_back(item.get<int>());
                        }
                        configValue = intArray;
                    } else if (allDoubles) {
                        std::vector<double> doubleArray;
                        doubleArray.reserve(value.size());
                        for (const auto& item : value) {
                            doubleArray.push_back(item.get<double>());
                        }
                        configValue = doubleArray;
                    } else if (allStrings) {
                        std::vector<std::string> stringArray;
                        stringArray.reserve(value.size());
                        for (const auto& item : value) {
                            stringArray.push_back(item.get<std::string>());
                        }
                        configValue = stringArray;
                    } else {
                        // Mixed types - convert to vector of doubles
                        std::vector<double> mixedArray;
                        mixedArray.reserve(value.size());
                        for (const auto& item : value) {
                            if (item.is_number()) {
                                mixedArray.push_back(item.get<double>());
                            } else {
                                mixedArray.push_back(0.0); // Default for non-numeric
                            }
                        }
                        configValue = mixedArray;
                    }
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Fall back to simple key=value format for backward compatibility
        file.clear();
        file.seekg(0);
        
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
                std::string valueStr = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (valueStr.size() >= 2 && 
                    ((valueStr.front() == '"' && valueStr.back() == '"') ||
                     (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                    valueStr = valueStr.substr(1, valueStr.size() - 2);
                }
                
                set(key, valueStr, ConfigSource::File);
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
    
    // Create JSON object
    nlohmann::json jsonConfig;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to appropriate JSON type
        std::visit([&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, int>) {
                jsonConfig[entry.key] = value;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonConfig[entry.key] = value;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonConfig[entry.key] = value;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonConfig[entry.key] = value;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonConfig[entry.key] = value;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonConfig[entry.key] = value;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonConfig[entry.key] = value;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonConfig[entry.key] = value;
            }
        }, entry.value);
    }
    
    // Write JSON to file with indentation
    file << jsonConfig.dump(4);
    
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "["; bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "["; bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "["; bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << "\"" << item << "\"";
                    first = false;
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
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm
